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

struct OggFile
{
    bool in_use;
    char* cur_ptr;
    char* file_ptr;
    size_t file_size;

    OggVorbis_File vorbis_file;
} g_ogg_files[MAX_OGG_FILES];

void audio_callback(void* buf, unsigned int length, void *userdata)
{    
    if (g_audio_buffer.ogg_id > -1)
    {
        if (g_audio_buffer.written > 0)
        {
            memcpy((void*)buf, (void*)(g_audio_buffer.chunks[g_audio_buffer.read_pos].data), AUDIO_BUFFER_SIZE);
            g_audio_buffer.read_pos = (g_audio_buffer.read_pos + 1) % AUDIO_BUFFER_CHUNKS;
            g_audio_buffer.written--;
        }
        else
        {
            g_debug_info.audio_wait_read++;
        }
    }
    else
    {
        memset(buf, 0, AUDIO_BUFFER_SIZE);
    }
}

void audio_init()
{
    pspAudioInit();
    pspAudioSetChannelCallback(0, audio_callback, NULL);

    g_audio_buffer.ogg_id = -1;

    g_debug_info.audio_wait_read = 0;
    g_debug_info.audio_wait_write = 0;
}

struct sample_t
{
    short l, r;
};
char temp_buffer[AUDIO_BUFFER_SIZE];

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
    pspAudioEnd();
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
    FILE *fp_ogg = fopen(filename, "rb");
    fseek(fp_ogg, 0, SEEK_END);
    long fsize = ftell(fp_ogg);
    fseek(fp_ogg, 0, SEEK_SET);
    g_ogg_files[0].file_ptr = malloc(fsize + 1);
    fread(g_ogg_files[0].file_ptr, fsize, 1, fp_ogg);
    fclose(fp_ogg);

    ov_callbacks callbacks;
    
    g_ogg_files[0].cur_ptr = g_ogg_files[0].file_ptr;
    g_ogg_files[0].file_size = fsize;
    g_ogg_files[0].in_use = true;

    callbacks.read_func = audio_ogg_callback_read_ogg;
    callbacks.seek_func = audio_ogg_callback_seek_ogg;
    callbacks.close_func = audio_ogg_callback_close_ogg;
    callbacks.tell_func = audio_ogg_callback_tell_ogg;

    ov_open_callbacks((void *)&(g_ogg_files[0]), &(g_ogg_files[0].vorbis_file), NULL, -1, callbacks);

#ifdef DEBUG    
    {
        DEBUG_PRINTF("Ogg file \"%s\" loaded.\n", filename);

        char **ptr=ov_comment(&(g_ogg_files[0].vorbis_file),-1)->user_comments;
        vorbis_info *vi=ov_info(&(g_ogg_files[0].vorbis_file),-1);
        while(*ptr){
          DEBUG_PRINTF("\t%s\n",*ptr);
          ++ptr;
        }
        DEBUG_PRINTF("\n\tBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
        DEBUG_PRINTF("\n\tDecoded length: %ld samples\n", (long)ov_pcm_total(&(g_ogg_files[0].vorbis_file),-1));
        DEBUG_PRINTF("\tEncoded by: %s\n\n",ov_comment(&(g_ogg_files[0].vorbis_file),-1)->vendor);
    }
#endif

    return 0;
}

int audio_load_ogg_from_archive(char *filename)
{
    size_t file_size = 0;
    g_ogg_files[0].file_ptr = (char *)archive_load_file_entry(filename, &file_size);

    ov_callbacks callbacks;
    
    g_ogg_files[0].cur_ptr = g_ogg_files[0].file_ptr;
    g_ogg_files[0].file_size = file_size;
    g_ogg_files[0].in_use = true;

    callbacks.read_func = audio_ogg_callback_read_ogg;
    callbacks.seek_func = audio_ogg_callback_seek_ogg;
    callbacks.close_func = audio_ogg_callback_close_ogg;
    callbacks.tell_func = audio_ogg_callback_tell_ogg;

    ov_open_callbacks((void *)&(g_ogg_files[0]), &(g_ogg_files[0].vorbis_file), NULL, -1, callbacks);

#ifdef DEBUG    
    {
        DEBUG_PRINTF("Ogg file \"%s\" loaded.\n", filename);

        char **ptr=ov_comment(&(g_ogg_files[0].vorbis_file),-1)->user_comments;
        vorbis_info *vi=ov_info(&(g_ogg_files[0].vorbis_file),-1);
        while(*ptr){
          DEBUG_PRINTF("\t%s\n",*ptr);
          ++ptr;
        }
        DEBUG_PRINTF("\n\tBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
        DEBUG_PRINTF("\n\tDecoded length: %ld samples\n", (long)ov_pcm_total(&(g_ogg_files[0].vorbis_file),-1));
        DEBUG_PRINTF("\tEncoded by: %s\n\n",ov_comment(&(g_ogg_files[0].vorbis_file),-1)->vendor);
    }
#endif

    return 0;
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
    if (g_audio_buffer.ogg_id == ogg_id)
    {
        audio_stop();
    }

    ov_clear(&(g_ogg_files[ogg_id].vorbis_file));
    free(g_ogg_files[ogg_id].file_ptr);
    g_ogg_files[ogg_id].in_use = false;
}


