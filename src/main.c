#include "global.h"

PSP_MODULE_INFO("PSPalatro", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#include "callbacks.c"
#include "archive.c"
#include "ini.c"
#include "audio.c"
#include "graphics.c"
#include "input.c"
#include "draw.c"
#include "game_util.c"
#include "entity_event.c"
#include "automated_events.c"
#include "game_input.c"
#include "save_utility.c"
#include "game.c"

struct DebugInfo g_debug_info;

int ogg_id = -1;

bool init_more_stuff()
{
    if (!game_init_load_file_values()) return false;

    if (g_settings.overclock)
    {
        game_draw_loading_text("Setting CPU clock to 333Mhz", COLOR_WHITE, COLOR_BLACK);
        scePowerSetClockFrequency(333, 333, 166);
    }
    
    if (!archive_open(g_settings.archive_file_name)) return false;

    game_init_logic();    

    if (!game_init_draw()) return false;
    
    if (g_settings.audio)
    {
        game_draw_loading_text("Loading audio file", COLOR_WHITE, COLOR_BLACK);
        ogg_id = audio_load_ogg_from_archive("resources/sounds/music1.ogg");
        if (ogg_id < 0)
        {
            game_draw_loading_text("Error loading file \"resources/sounds/music1.ogg\" from archive", COLOR_TEXT_RED, COLOR_BLACK);
            return false;
        }
        audio_play_ogg(ogg_id, 0.8f);
    }

    archive_close();

    event_init();

    return true;
}

int main(int argc, char **argv)
{
    DEBUG_PRINTF("PSPalatro");

    setup_callbacks();

    sceKernelDcacheWritebackAll();

    audio_init();

    graphics_init();

    input_init();

    game_init_draw_load_fonts();

    if (init_more_stuff())
    {
        while(running())
        {
            if (g_settings.audio) audio_update();
            input_update();
            game_update();
            game_draw();
        }
    }
    else
    {
        while(running())
        {
            graphics_begin_draw();
            graphics_end_draw();
        }
    }

    if (g_settings.audio)
    {
        audio_stop();
        audio_destroy_ogg(ogg_id);
    }

    audio_end();    

    sceGuTerm();
	sceKernelExitGame();

    return 0;
}
