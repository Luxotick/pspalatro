#include <pspkernel.h>
#include <psputility.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <pspgu.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "global.h"

__attribute__((aligned(64))) static SceUtilitySavedataParam dialog;
static bool save_dialog_running = false;

extern void graphics_begin_draw();
extern void graphics_end_draw();
extern void graphics_clear(uint32_t color);

void configure_dialog()
{
    memset(&dialog, 0, sizeof(SceUtilitySavedataParam));
    dialog.base.size = sizeof(SceUtilitySavedataParam);
    dialog.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
    dialog.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
    dialog.base.graphicsThread = 17;
    dialog.base.accessThread = 19;
    dialog.base.fontThread = 18;
    dialog.base.soundThread = 16;
    
    strcpy(dialog.gameName, "PSPALATRO");
    strcpy(dialog.saveName, "0000");
    strcpy(dialog.saveNameList[0], "0000");
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
}

void process_dialog_loop()
{
    save_dialog_running = true;
    while(save_dialog_running)
    {
        graphics_begin_draw();
        graphics_clear(0xFF000000);
        
        switch(sceUtilitySavedataGetStatus())
        {
            case PSP_UTILITY_DIALOG_VISIBLE:
                sceUtilitySavedataUpdate(2);
                break;
            case PSP_UTILITY_DIALOG_QUIT:
                sceUtilitySavedataShutdownStart();
                break;
            case PSP_UTILITY_DIALOG_NONE:
                save_dialog_running = false;
                break;
            case PSP_UTILITY_DIALOG_FINISHED:
                break;
        }
        graphics_end_draw();
        sceDisplayWaitVblankStart();
    }
}

void run_save_utility()
{
    configure_dialog();
    dialog.mode = PSP_UTILITY_SAVEDATA_SAVE;
    
    void *base_addr = &g_game_state.all_cards.cards[0];
    memcpy(dialog.dataBuf, &base_addr, sizeof(void*));
    memcpy((char*)dialog.dataBuf + sizeof(void*), &g_game_state, sizeof(g_game_state));
    
    sceUtilitySavedataInitStart(&dialog);
    process_dialog_loop();
    free(dialog.dataBuf);
}

void run_load_utility()
{
    configure_dialog();
    dialog.mode = PSP_UTILITY_SAVEDATA_LOAD;
    
    sceUtilitySavedataInitStart(&dialog);
    process_dialog_loop();
    
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
}
