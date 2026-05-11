#include <pspkernel.h>
#include <psputility.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <pspgu.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdarg.h>
#include "global.h"

void save_debug_log(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    FILE *f = fopen("save_debug.txt", "a");
    if (f) {
        fprintf(f, "%s\n", buffer);
        fclose(f);
    }
}

__attribute__((aligned(64))) static SceUtilitySavedataParam dialog;
static bool save_dialog_running = false;

extern void graphics_begin_draw();
extern void graphics_end_draw();
extern void graphics_clear(uint32_t color);

static char nameMultiple[][20] = {
    "0000",
    "0001",
    "0002",
    "0003",
    "0004",
    ""
};

static const char save_key[] = "BALATRO_KEY_001"; // Encryption key for savedata

static PspUtilitySavedataListSaveNewData newData;
static char *titleShow = "New Save";

void configure_dialog()
{
    memset(&dialog, 0, sizeof(SceUtilitySavedataParam));
    dialog.base.size = sizeof(SceUtilitySavedataParam);
    dialog.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
    dialog.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
    dialog.base.graphicsThread = 0x11;
    dialog.base.accessThread = 0x13;
    dialog.base.fontThread = 0x12;
    dialog.base.soundThread = 0x10;
    
    // Set encryption key (Crucial for newer firmwares)
    strncpy(dialog.key, save_key, sizeof(dialog.key));

    strcpy(dialog.gameName, "PSPALATRO");
    strcpy(dialog.saveName, "0000");
    dialog.saveNameList = nameMultiple;
    dialog.overwrite = 1;
    dialog.focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST;

    strcpy(dialog.fileName, "save_data.bin");
    
    size_t state_size = sizeof(void*) + sizeof(g_game_state);
    dialog.dataBufSize = state_size;
    dialog.dataSize = state_size;
    dialog.dataBuf = memalign(64, state_size);
    memset(dialog.dataBuf, 0, state_size);
    
    strcpy(dialog.sfoParam.title, "PSPalatro");
    strcpy(dialog.sfoParam.savedataTitle, "PSPalatro Save");
    strcpy(dialog.sfoParam.detail, "Game Progress");
    dialog.sfoParam.parentalLevel = 1;

    // Set title for New Save slots
    memset(&newData, 0, sizeof(PspUtilitySavedataListSaveNewData));
    newData.title = titleShow;
    dialog.newData = &newData;
}

void process_dialog_loop()
{
    save_dialog_running = true;
    while(save_dialog_running)
    {
        int status = sceUtilitySavedataGetStatus();

        if (status == PSP_UTILITY_DIALOG_VISIBLE) {
            sceUtilitySavedataUpdate(1);
        } else if (status == PSP_UTILITY_DIALOG_QUIT) {
            sceUtilitySavedataShutdownStart();
        } else if (status == PSP_UTILITY_DIALOG_FINISHED) {
            // Success or Finished
        } else if (status == PSP_UTILITY_DIALOG_NONE) {
            save_dialog_running = false;
        }

        sceDisplayWaitVblankStart();
    }
}

void run_save_utility()
{
    save_debug_log("=== STARTING LISTSAVE UTILITY ===");
    
    extern void audio_end();
    audio_end();

    configure_dialog();
    dialog.mode = PSP_UTILITY_SAVEDATA_LISTSAVE; // Use the slot-based menu mode
    dialog.focus = PSP_UTILITY_SAVEDATA_FOCUS_FIRSTEMPTY;

    void *base_addr = &g_game_state.all_cards.cards[0];
    memcpy(dialog.dataBuf, &base_addr, sizeof(void*));
    memcpy((char*)dialog.dataBuf + sizeof(void*), &g_game_state, sizeof(g_game_state));
    
    sceKernelDcacheWritebackAll();
    sceGuDisplay(GU_FALSE);

    int init_res = sceUtilitySavedataInitStart(&dialog);
    save_debug_log("InitStart result: %08x", init_res);
    process_dialog_loop();
    
    sceGuDisplay(GU_TRUE);
    free(dialog.dataBuf);
    
    extern void audio_init();
    audio_init();

    save_debug_log("=== SAVE UTILITY COMPLETE ===");
}

void run_load_utility()
{
    save_debug_log("=== STARTING LISTLOAD UTILITY ===");
    
    extern void audio_end();
    audio_end();

    configure_dialog();
    dialog.mode = PSP_UTILITY_SAVEDATA_LISTLOAD; // Use the slot-based menu mode
    dialog.focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST;

    sceGuDisplay(GU_FALSE);

    int init_res = sceUtilitySavedataInitStart(&dialog);
    save_debug_log("InitStart result: %08x", init_res);
    process_dialog_loop();
    
    sceGuDisplay(GU_TRUE);

    if (dialog.base.result == 0) // SUCCESS
    {
        void *old_base_addr;
        memcpy(&old_base_addr, dialog.dataBuf, sizeof(void*));
        memcpy(&g_game_state, (char*)dialog.dataBuf + sizeof(void*), sizeof(g_game_state));
        
        long ptr_diff = (long)(&g_game_state.all_cards.cards[0]) - (long)old_base_addr;
        
        if (ptr_diff != 0)
        {
            for(int i=0; i<g_game_state.full_deck.card_count; i++)
                g_game_state.full_deck.cards[i] = (struct Card*)((char*)g_game_state.full_deck.cards[i] + ptr_diff);
                
            for(int i=0; i<g_game_state.current_deck.card_count; i++)
                g_game_state.current_deck.cards[i] = (struct Card*)((char*)g_game_state.current_deck.cards[i] + ptr_diff);
                
            for(int i=0; i<g_game_state.hand.card_count; i++)
                g_game_state.hand.cards[i] = (struct Card*)((char*)g_game_state.hand.cards[i] + ptr_diff);
                
            for(int i=0; i<g_game_state.played_hand.card_count; i++)
                g_game_state.played_hand.cards[i] = (struct Card*)((char*)g_game_state.played_hand.cards[i] + ptr_diff);
                
            for(int k=0; k<4; k++)
            {
                for(int i=0; i<g_game_state.deck_info.card_count[k]; i++)
                {
                    g_game_state.deck_info.cards[k][i] = (struct Card*)((char*)g_game_state.deck_info.cards[k][i] + ptr_diff);
                }
            }
        }
    }
    
    free(dialog.dataBuf);

    extern void audio_init();
    audio_init();

    save_debug_log("=== LOAD UTILITY COMPLETE ===");
}
