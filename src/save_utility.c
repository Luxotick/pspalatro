#include <pspkernel.h>
#include <string.h>
#include <stdio.h>
#include "global.h"

void run_save_utility()
{
    SceUID fd;
    
    sceIoMkdir("ms0:/PSP/SAVEDATA", 0777); 
    sceIoMkdir("ms0:/PSP/SAVEDATA/PSPALATRO", 0777);

    fd = sceIoOpen("ms0:/PSP/SAVEDATA/PSPALATRO/save_data.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (fd >= 0)
    {
        // 1. Önce all_cards referans adresini kaydedelim ki pointer'ları düzeltebilelim
        void *base_addr = &g_game_state.all_cards.cards[0];
        sceIoWrite(fd, &base_addr, sizeof(void*));
        
        // 2. Tüm oyunu kaydedelim
        sceIoWrite(fd, &g_game_state, sizeof(g_game_state));
        sceIoClose(fd);
    }
}

void run_load_utility()
{
    SceUID fd = sceIoOpen("ms0:/PSP/SAVEDATA/PSPALATRO/save_data.bin", PSP_O_RDONLY, 0777);
    if (fd >= 0)
    {
        void *old_base_addr;
        sceIoRead(fd, &old_base_addr, sizeof(void*));
        
        // Tüm state'i okuyalım
        sceIoRead(fd, &g_game_state, sizeof(g_game_state));
        sceIoClose(fd);

        // PRX memory kaymışsa (ASLR veya custom firmware adres değişimi) pointerları onaralım
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
}
