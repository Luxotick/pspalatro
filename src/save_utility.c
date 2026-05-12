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

extern unsigned char _binary_media_pspalatro_icon_png_start[];
extern unsigned char _binary_media_pspalatro_icon_png_size[];

static void* icon_buffer = NULL;
static size_t icon_size = 0;

void load_save_icon() {
    if (icon_buffer) return;
    icon_size = (size_t)&_binary_media_pspalatro_icon_png_size;
    icon_buffer = memalign(64, icon_size);
    if (!icon_buffer) {
        return;
    }
    memcpy(icon_buffer, _binary_media_pspalatro_icon_png_start, icon_size);
}


// Small dedicated draw list for save dialog rendering (separate from game's draw list)
static unsigned int __attribute__((aligned(16))) save_draw_list[2048];

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
    memset(dialog.key, 0, sizeof(dialog.key));
    strncpy(dialog.key, save_key, sizeof(dialog.key) - 1);

    memset(dialog.gameName, 0, sizeof(dialog.gameName));
    strncpy(dialog.gameName, "PSPALATRO", sizeof(dialog.gameName) - 1);

    memset(dialog.saveName, 0, sizeof(dialog.saveName));
    strncpy(dialog.saveName, "0000", sizeof(dialog.saveName) - 1);
    dialog.saveNameList = nameMultiple;
    dialog.overwrite = 1;
    dialog.focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST;

    memset(dialog.fileName, 0, sizeof(dialog.fileName));
    strncpy(dialog.fileName, "SAVE.BIN", sizeof(dialog.fileName) - 1);

    size_t state_size = sizeof(void*) + sizeof(g_game_state);
    // Real PSP hardware strictly requires dataBufSize to be 64-byte aligned
    size_t aligned_state_size = (state_size + 63) & ~63;
    save_debug_log("Allocating save buffer. State size: %d, Aligned: %d", state_size, aligned_state_size);

    dialog.dataBufSize = aligned_state_size;
    dialog.dataSize = state_size; // Keep original dataSize, but buffer is aligned
    dialog.dataBuf = memalign(64, aligned_state_size);
    if (!dialog.dataBuf) {
        save_debug_log("memalign failed for %u bytes", (unsigned)aligned_state_size);
        return;
    }
    memset(dialog.dataBuf, 0, aligned_state_size);

    memset(dialog.sfoParam.title, 0, sizeof(dialog.sfoParam.title));
    strncpy(dialog.sfoParam.title, "PSPalatro", sizeof(dialog.sfoParam.title) - 1);
    memset(dialog.sfoParam.savedataTitle, 0, sizeof(dialog.sfoParam.savedataTitle));
    strncpy(dialog.sfoParam.savedataTitle, "PSPalatro Save", sizeof(dialog.sfoParam.savedataTitle) - 1);
    memset(dialog.sfoParam.detail, 0, sizeof(dialog.sfoParam.detail));
    strncpy(dialog.sfoParam.detail, "Game Progress", sizeof(dialog.sfoParam.detail) - 1);
    dialog.sfoParam.parentalLevel = 1;

    // Load and set Icon
    load_save_icon();
    if (icon_buffer) {
        dialog.icon0FileData.buf = icon_buffer;
        dialog.icon0FileData.bufSize = icon_size;
        dialog.icon0FileData.size = icon_size;
    }

    // Set title and icon for New Save slots
    memset(&newData, 0, sizeof(PspUtilitySavedataListSaveNewData));
    newData.title = titleShow;
    if (icon_buffer) {
        newData.icon0.buf = icon_buffer;
        newData.icon0.bufSize = icon_size;
        newData.icon0.size = icon_size;
    }
    dialog.newData = &newData;
}

void process_dialog_loop()
{
    save_dialog_running = true;
    bool dialog_was_visible = false;

    while(save_dialog_running)
    {
        // 1. Render a clear frame and CLOSE the GU list before dialog update
        sceGuStart(GU_DIRECT, save_draw_list);
        sceGuClearColor(0xFF000000);
        sceGuClear(GU_COLOR_BUFFER_BIT);
        sceGuFinish();
        sceGuSync(0, 0);

        // 2. Update dialog (GU list must be closed at this point)
        int status = sceUtilitySavedataGetStatus();
        save_debug_log("status: %d", status);

        if (status == PSP_UTILITY_DIALOG_VISIBLE) {
            dialog_was_visible = true;
            sceUtilitySavedataUpdate(1);
        } else if (status == PSP_UTILITY_DIALOG_QUIT) {
            sceUtilitySavedataShutdownStart();
        } else if (status == PSP_UTILITY_DIALOG_FINISHED) {
            save_dialog_running = false;
        } else if (status == PSP_UTILITY_DIALOG_NONE && dialog_was_visible) {
            save_dialog_running = false;
        }

        // 3. VBlank wait + swap buffers
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    save_debug_log("dialog result: %d", dialog.base.result);
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

    int init_res = sceUtilitySavedataInitStart(&dialog);
    save_debug_log("InitStart result: %08x", init_res);
    process_dialog_loop();

    free(dialog.dataBuf);

    extern void audio_resume();
    audio_resume();

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



    int init_res = sceUtilitySavedataInitStart(&dialog);
    save_debug_log("InitStart result: %08x", init_res);
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

    extern void audio_resume();
    audio_resume();

    save_debug_log("=== LOAD UTILITY COMPLETE ===");
}
