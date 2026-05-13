#include "global.h"

#include <vorbis/vorbisfile.h>

#define AUDIO_BUFFER_CHUNKS  8
#define AUDIO_BUFFER_SIZE   4096

struct audio_buffer_data
{
    char data[AUDIO_BUFFER_SIZE];    
};

struct audio_buffer
{
    struct audio_buffer_data chunks[AUDIO_BUFFER_CHUNKS];
    int read_pos;
    int write_pos;
    int written;

    int ogg_id;

    float speed;
    int src_buffer_size;
};

volatile struct audio_buffer g_audio_buffer;

#define MAX_OGG_FILES   10
#define AUDIO_SFX_CHANNELS 6

struct OggFile
{
    bool in_use;
    char* cur_ptr;
    char* file_ptr;
    size_t file_size;

    OggVorbis_File vorbis_file;
} g_ogg_files[MAX_OGG_FILES];

struct sample_t
{
    short l, r;
};

struct AudioSfx
{
    bool in_use;
    struct sample_t *samples;
    int sample_count;
    float volume;
};

struct AudioSfxChannel
{
    bool active;
    int sfx_id;
    int position;
};

struct AudioFlameChannel
{
    int position;
    float volume;
    float target_volume;
};

struct AudioSfx g_audio_sfx[AUDIO_SFX_COUNT];
volatile struct AudioSfxChannel g_audio_sfx_channels[AUDIO_SFX_CHANNELS];
volatile struct AudioFlameChannel g_audio_flame_channel;

static short audio_clamp_sample(int sample)
{
    if (sample > 32767) return 32767;
    if (sample < -32768) return -32768;
    return sample;
}

void audio_callback(void* buf, unsigned int length, void *userdata)
{    
    unsigned int buffer_size = length * sizeof(struct sample_t);
    struct sample_t *out = (struct sample_t *)buf;
    int sample_count = length;

    if (!g_settings.audio)
    {
        memset(buf, 0, buffer_size);
        return;
    }

    if (g_audio_buffer.ogg_id > -1 && !g_debug_info.force_score_flames)
    {
        if (g_audio_buffer.written > 0)
        {
            memcpy((void*)buf, (void*)(g_audio_buffer.chunks[g_audio_buffer.read_pos].data), buffer_size);
            g_audio_buffer.read_pos = (g_audio_buffer.read_pos + 1) % AUDIO_BUFFER_CHUNKS;
            g_audio_buffer.written--;
        }
        else
        {
            g_debug_info.audio_wait_read++;
            memset(buf, 0, AUDIO_BUFFER_SIZE);
        }
    }
    else
    {
        memset(buf, 0, buffer_size);
    }

    for (int channel_index = 0; channel_index < AUDIO_SFX_CHANNELS; channel_index++)
    {
        volatile struct AudioSfxChannel *channel = &g_audio_sfx_channels[channel_index];
        if (!channel->active) continue;
        if (channel->sfx_id < 0 || channel->sfx_id >= AUDIO_SFX_COUNT)
        {
            channel->active = false;
            continue;
        }

        struct AudioSfx *sfx = &g_audio_sfx[channel->sfx_id];
        if (!sfx->in_use)
        {
            channel->active = false;
            continue;
        }

        for (int i = 0; i < sample_count; i++)
        {
            if (channel->position >= sfx->sample_count)
            {
                channel->active = false;
                break;
            }

            struct sample_t sample = sfx->samples[channel->position++];
            out[i].l = audio_clamp_sample(out[i].l + (int)((float)sample.l * sfx->volume));
            out[i].r = audio_clamp_sample(out[i].r + (int)((float)sample.r * sfx->volume));
        }
    }

    struct AudioSfx *flame_sfx = &g_audio_sfx[AUDIO_SFX_FLAME];
    if (flame_sfx->in_use && flame_sfx->sample_count > 0 &&
        (g_audio_flame_channel.volume > 0.001f || g_audio_flame_channel.target_volume > 0.001f))
    {
        float volume = g_audio_flame_channel.volume;
        float target = g_audio_flame_channel.target_volume;
        float volume_step = (target - volume) / (float)sample_count;
        int position = g_audio_flame_channel.position;

        for (int i = 0; i < sample_count; i++)
        {
            if (position >= flame_sfx->sample_count) position = 0;

            volume += volume_step;
            struct sample_t sample = flame_sfx->samples[position++];
            out[i].l = audio_clamp_sample(out[i].l + (int)((float)sample.l * flame_sfx->volume * volume));
            out[i].r = audio_clamp_sample(out[i].r + (int)((float)sample.r * flame_sfx->volume * volume));
        }

        g_audio_flame_channel.volume = volume;
        g_audio_flame_channel.position = position;
    }
}

void audio_init()
{
    pspAudioInit();
    pspAudioSetChannelCallback(0, audio_callback, NULL);

    g_audio_buffer.ogg_id = -1;

    g_debug_info.audio_wait_read = 0;
    g_debug_info.audio_wait_write = 0;

    for (int i = 0; i < AUDIO_SFX_COUNT; i++)
    {
        g_audio_sfx[i].in_use = false;
        g_audio_sfx[i].samples = NULL;
        g_audio_sfx[i].sample_count = 0;
        g_audio_sfx[i].volume = 1.0f;
    }

    for (int i = 0; i < AUDIO_SFX_CHANNELS; i++)
    {
        g_audio_sfx_channels[i].active = false;
    }

    g_audio_flame_channel.position = 0;
    g_audio_flame_channel.volume = 0.0f;
    g_audio_flame_channel.target_volume = 0.0f;
}
char temp_buffer[AUDIO_BUFFER_SIZE];

static int audio_get_free_ogg_slot()
{
    for (int i = 0; i < MAX_OGG_FILES; i++)
    {
        if (!g_ogg_files[i].in_use) return i;
    }

    return -1;
}

void audio_update()
{
    if (g_audio_buffer.ogg_id > -1 && g_audio_buffer.written < AUDIO_BUFFER_CHUNKS)
    {
        int current_section;
        long ret = 0, total = 0;
        char *buf_ptr = temp_buffer;
        while (total < g_audio_buffer.src_buffer_size)
        {
            ret = ov_read(&(g_ogg_files[g_audio_buffer.ogg_id].vorbis_file),buf_ptr,g_audio_buffer.src_buffer_size - total,0,2,1,&current_section);
            if (ret == 0)
            {
                ov_pcm_seek(&(g_ogg_files[g_audio_buffer.ogg_id].vorbis_file), 0);
                continue;
            }
            total += ret;
            buf_ptr += ret;
        }

        struct sample_t *dst = (struct sample_t *)g_audio_buffer.chunks[g_audio_buffer.write_pos].data;
        struct sample_t *src = (struct sample_t *)temp_buffer;
        float i_src = 0.0f;
        float inc = g_audio_buffer.speed;

        int size = (AUDIO_BUFFER_SIZE / 4);

        int index/*, index2*/ = 0;
        for (int i = 0; i < size; i++)
        {
            // index = floor(i_src);
            // index2 = index + 1;
            // if (index2 >= size)
            // {
            //     dst[i].l = src[index].l;
            //     dst[i].r = src[index].r;
            // }
            // else
            // {
            //     float f = i_src - (float)index;
            //     dst[i].l = src[index].l * (1.0f - f) + src[index2].l * f;
            //     dst[i].r = src[index].r * (1.0f - f) + src[index2].r * f;
            // }

            index = roundf(i_src);
            dst[i].l = src[index].l;
            dst[i].r = src[index].r;

            i_src += inc;
        }

        g_audio_buffer.write_pos = (g_audio_buffer.write_pos + 1) % AUDIO_BUFFER_CHUNKS;
        g_audio_buffer.written++;
    }
    else
    {
        g_debug_info.audio_wait_write++;
    }
}

void audio_end()
{
    pspAudioSetChannelCallback(0, NULL, NULL);
}

void audio_resume()
{
pspAudioSetChannelCallback(0, audio_callback, NULL);
}

size_t audio_ogg_callback_read_ogg(void* dst, size_t size1, size_t size2, void* fh)
{
    struct OggFile* of = (struct OggFile*)(fh);
    size_t len = size1 * size2;
    if ( of->cur_ptr + len > of->file_ptr + of->file_size )
    {
        len = of->file_ptr + of->file_size - of->cur_ptr;
    }
    memcpy( dst, of->cur_ptr, len );
    of->cur_ptr += len;
    return len;
}

int audio_ogg_callback_seek_ogg( void *fh, ogg_int64_t to, int type ) {
    struct OggFile* of = (struct OggFile*)(fh);

    switch( type ) {
        case SEEK_CUR:
            of->cur_ptr += to;
            break;
        case SEEK_END:
            of->cur_ptr = of->file_ptr + of->file_size - to;
            break;
        case SEEK_SET:
            of->cur_ptr = of->file_ptr + to;
            break;
        default:
            return -1;
    }
    if ( of->cur_ptr < of->file_ptr ) {
        of->cur_ptr = of->file_ptr;
        return -1;
    }
    if ( of->cur_ptr > of->file_ptr + of->file_size ) {
        of->cur_ptr = of->file_ptr + of->file_size;
        return -1;
    }
    return 0;
}

int audio_ogg_callback_close_ogg(void* fh)
{
    return 0;
}

long audio_ogg_callback_tell_ogg( void *fh )
{
    struct OggFile* of = (struct OggFile*)(fh);
    return (of->cur_ptr - of->file_ptr);
}

int audio_load_ogg(char *filename)
{
    int ogg_id = audio_get_free_ogg_slot();
    if (ogg_id < 0) return -1;

    FILE *fp_ogg = fopen(filename, "rb");
    if (!fp_ogg) return -1;

    fseek(fp_ogg, 0, SEEK_END);
    long fsize = ftell(fp_ogg);
    fseek(fp_ogg, 0, SEEK_SET);
    g_ogg_files[ogg_id].file_ptr = malloc(fsize + 1);
    if (!g_ogg_files[ogg_id].file_ptr)
    {
        fclose(fp_ogg);
        return -1;
    }
    fread(g_ogg_files[ogg_id].file_ptr, fsize, 1, fp_ogg);
    fclose(fp_ogg);

    ov_callbacks callbacks;
    
    g_ogg_files[ogg_id].cur_ptr = g_ogg_files[ogg_id].file_ptr;
    g_ogg_files[ogg_id].file_size = fsize;
    g_ogg_files[ogg_id].in_use = true;

    callbacks.read_func = audio_ogg_callback_read_ogg;
    callbacks.seek_func = audio_ogg_callback_seek_ogg;
    callbacks.close_func = audio_ogg_callback_close_ogg;
    callbacks.tell_func = audio_ogg_callback_tell_ogg;

    if (ov_open_callbacks((void *)&(g_ogg_files[ogg_id]), &(g_ogg_files[ogg_id].vorbis_file), NULL, -1, callbacks) < 0)
    {
        free(g_ogg_files[ogg_id].file_ptr);
        g_ogg_files[ogg_id].in_use = false;
        return -1;
    }

#ifdef DEBUG    
    {
        DEBUG_PRINTF("Ogg file \"%s\" loaded.\n", filename);

        char **ptr=ov_comment(&(g_ogg_files[ogg_id].vorbis_file),-1)->user_comments;
        vorbis_info *vi=ov_info(&(g_ogg_files[ogg_id].vorbis_file),-1);
        while(*ptr){
          DEBUG_PRINTF("\t%s\n",*ptr);
          ++ptr;
        }
        DEBUG_PRINTF("\n\tBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
        DEBUG_PRINTF("\n\tDecoded length: %ld samples\n", (long)ov_pcm_total(&(g_ogg_files[ogg_id].vorbis_file),-1));
        DEBUG_PRINTF("\tEncoded by: %s\n\n",ov_comment(&(g_ogg_files[ogg_id].vorbis_file),-1)->vendor);
    }
#endif

    return ogg_id;
}

int audio_load_ogg_from_archive(char *filename)
{
    int ogg_id = audio_get_free_ogg_slot();
    if (ogg_id < 0) return -1;

    size_t file_size = 0;
    g_ogg_files[ogg_id].file_ptr = (char *)archive_load_file_entry(filename, &file_size);
    if (!g_ogg_files[ogg_id].file_ptr) return -1;

    ov_callbacks callbacks;
    
    g_ogg_files[ogg_id].cur_ptr = g_ogg_files[ogg_id].file_ptr;
    g_ogg_files[ogg_id].file_size = file_size;
    g_ogg_files[ogg_id].in_use = true;

    callbacks.read_func = audio_ogg_callback_read_ogg;
    callbacks.seek_func = audio_ogg_callback_seek_ogg;
    callbacks.close_func = audio_ogg_callback_close_ogg;
    callbacks.tell_func = audio_ogg_callback_tell_ogg;

    if (ov_open_callbacks((void *)&(g_ogg_files[ogg_id]), &(g_ogg_files[ogg_id].vorbis_file), NULL, -1, callbacks) < 0)
    {
        free(g_ogg_files[ogg_id].file_ptr);
        g_ogg_files[ogg_id].in_use = false;
        return -1;
    }

#ifdef DEBUG    
    {
        DEBUG_PRINTF("Ogg file \"%s\" loaded.\n", filename);

        char **ptr=ov_comment(&(g_ogg_files[ogg_id].vorbis_file),-1)->user_comments;
        vorbis_info *vi=ov_info(&(g_ogg_files[ogg_id].vorbis_file),-1);
        while(*ptr){
          DEBUG_PRINTF("\t%s\n",*ptr);
          ++ptr;
        }
        DEBUG_PRINTF("\n\tBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
        DEBUG_PRINTF("\n\tDecoded length: %ld samples\n", (long)ov_pcm_total(&(g_ogg_files[ogg_id].vorbis_file),-1));
        DEBUG_PRINTF("\tEncoded by: %s\n\n",ov_comment(&(g_ogg_files[ogg_id].vorbis_file),-1)->vendor);
    }
#endif

    return ogg_id;
}

int audio_load_sfx_from_archive_limited(int sfx_id, char *filename, float volume, float seconds)
{
    if (sfx_id < 0 || sfx_id >= AUDIO_SFX_COUNT) return -1;

    int ogg_id = audio_load_ogg_from_archive(filename);
    if (ogg_id < 0) return -1;

    long total_pcm = (long)ov_pcm_total(&(g_ogg_files[ogg_id].vorbis_file), -1);
    vorbis_info *info = ov_info(&(g_ogg_files[ogg_id].vorbis_file), -1);
    if (total_pcm <= 0 || !info || info->channels <= 0)
    {
        audio_destroy_ogg(ogg_id);
        return -1;
    }

    long sample_limit = total_pcm;
    if (seconds > 0.0f && info->rate > 0)
    {
        sample_limit = MIN(total_pcm, (long)((float)info->rate * seconds));
    }

    struct sample_t *samples = malloc(sample_limit * sizeof(struct sample_t));
    if (!samples)
    {
        audio_destroy_ogg(ogg_id);
        return -1;
    }

    int current_section = 0;
    long total_samples = 0;
    while (total_samples < sample_limit)
    {
        long ret = ov_read(&(g_ogg_files[ogg_id].vorbis_file), temp_buffer, AUDIO_BUFFER_SIZE, 0, 2, 1, &current_section);
        if (ret <= 0) break;

        short *decoded = (short *)temp_buffer;
        int frame_count = ret / (sizeof(short) * info->channels);
        for (int i = 0; i < frame_count && total_samples < sample_limit; i++)
        {
            if (info->channels == 1)
            {
                samples[total_samples].l = decoded[i];
                samples[total_samples].r = decoded[i];
            }
            else
            {
                samples[total_samples].l = decoded[i * info->channels];
                samples[total_samples].r = decoded[i * info->channels + 1];
            }
            total_samples++;
        }
    }

    audio_destroy_ogg(ogg_id);

    if (total_samples <= 0)
    {
        free(samples);
        return -1;
    }

    if (g_audio_sfx[sfx_id].in_use)
    {
        free(g_audio_sfx[sfx_id].samples);
    }

    g_audio_sfx[sfx_id].samples = samples;
    g_audio_sfx[sfx_id].sample_count = total_samples;
    g_audio_sfx[sfx_id].volume = volume;
    g_audio_sfx[sfx_id].in_use = true;

    return sfx_id;
}

int audio_load_sfx_from_archive(int sfx_id, char *filename, float volume)
{
    return audio_load_sfx_from_archive_limited(sfx_id, filename, volume, 0.0f);
}

void audio_play_ogg(int ogg_id, float speed)
{
    g_audio_buffer.ogg_id = ogg_id;
    g_audio_buffer.read_pos = 0;
    g_audio_buffer.write_pos = 0;
    g_audio_buffer.written = 0;
    g_audio_buffer.speed = (speed > 1.0f ? 1.0f : (speed <= 0.0f ? 1.0f : speed));
    if (g_audio_buffer.speed == 1.0f)
    {
        g_audio_buffer.src_buffer_size = AUDIO_BUFFER_SIZE;
    }
    else
    {
        g_audio_buffer.src_buffer_size = (int)((float)(AUDIO_BUFFER_SIZE / 4) * (float)g_audio_buffer.speed) * 4;
    }
}

void audio_stop()
{
    g_audio_buffer.ogg_id = -1;
    g_audio_buffer.read_pos = 0;
    g_audio_buffer.write_pos = 0;
    g_audio_buffer.written = 0;
}

void audio_destroy_ogg(int ogg_id)
{
    if (ogg_id < 0 || ogg_id >= MAX_OGG_FILES || !g_ogg_files[ogg_id].in_use) return;

    if (g_audio_buffer.ogg_id == ogg_id)
    {
        audio_stop();
    }

    ov_clear(&(g_ogg_files[ogg_id].vorbis_file));
    free(g_ogg_files[ogg_id].file_ptr);
    g_ogg_files[ogg_id].in_use = false;
}

void audio_play_sfx(int sfx_id)
{
    if (!g_settings.audio) return;
    if (sfx_id < 0 || sfx_id >= AUDIO_SFX_COUNT || !g_audio_sfx[sfx_id].in_use) return;

    int channel_index = -1;
    for (int i = 0; i < AUDIO_SFX_CHANNELS; i++)
    {
        if (!g_audio_sfx_channels[i].active)
        {
            channel_index = i;
            break;
        }
    }

    if (channel_index < 0) channel_index = 0;

    g_audio_sfx_channels[channel_index].sfx_id = sfx_id;
    g_audio_sfx_channels[channel_index].position = 0;
    g_audio_sfx_channels[channel_index].active = true;
}

void audio_set_score_flame_intensity(float intensity)
{
    if (!g_settings.audio)
    {
        g_audio_flame_channel.target_volume = 0.0f;
        return;
    }

    float target_volume = CLAMP(intensity / 4.0f, 0.0f, 1.0f) * 2.35f;
    g_audio_flame_channel.target_volume = target_volume;
}

void audio_destroy_sfx()
{
    for (int i = 0; i < AUDIO_SFX_CHANNELS; i++)
    {
        g_audio_sfx_channels[i].active = false;
    }

    g_audio_flame_channel.target_volume = 0.0f;
    g_audio_flame_channel.volume = 0.0f;
    g_audio_flame_channel.position = 0;

    for (int i = 0; i < AUDIO_SFX_COUNT; i++)
    {
        if (g_audio_sfx[i].in_use)
        {
            free(g_audio_sfx[i].samples);
            g_audio_sfx[i].samples = NULL;
            g_audio_sfx[i].sample_count = 0;
            g_audio_sfx[i].in_use = false;
        }
    }
}
