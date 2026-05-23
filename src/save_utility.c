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
static const char autosave_path[] = "autosave.bin";
static const char profile_path[] = "profile.bin";
static int autosave_exists_cache = -1;

#define SAVE_MAGIC "PSPALTRO"
#define SAVE_MAGIC_SIZE 8
#define SAVE_VERSION 1
#define PROFILE_MAGIC "PSPAPROF"
#define PROFILE_VERSION 1

struct SaveHeader
{
    char magic[SAVE_MAGIC_SIZE];
    int version;
    void *base_addr;
    unsigned int state_size;
};

struct ProfileHeader
{
    char magic[SAVE_MAGIC_SIZE];
    int version;
    unsigned int profile_size;
};

int profile_get_discovered_items_count();

static int profile_popcount(uint32_t value)
{
    int count = 0;
    while (value)
    {
        count += value & 1;
        value >>= 1;
    }
    return count;
}

static void profile_init_defaults()
{
    memset(&g_profile, 0, sizeof(g_profile));
    g_profile.unlocked_decks = 1u << DECK_TYPE_RED;
}

static void profile_update_deck_unlocks()
{
    int discovered = profile_get_discovered_items_count();
    g_profile.unlocked_decks |= 1u << DECK_TYPE_RED;

    if (discovered >= 20) g_profile.unlocked_decks |= 1u << DECK_TYPE_BLUE;
    if (discovered >= 50) g_profile.unlocked_decks |= 1u << DECK_TYPE_YELLOW;
    if (discovered >= 75) g_profile.unlocked_decks |= 1u << DECK_TYPE_GREEN;
    if (discovered >= 100) g_profile.unlocked_decks |= 1u << DECK_TYPE_BLACK;

    if (g_profile.won_decks & (1u << DECK_TYPE_RED)) g_profile.unlocked_decks |= 1u << DECK_TYPE_MAGIC;
    if (g_profile.won_decks & (1u << DECK_TYPE_BLUE)) g_profile.unlocked_decks |= 1u << DECK_TYPE_NEBULA;
    if (g_profile.won_decks & (1u << DECK_TYPE_YELLOW)) g_profile.unlocked_decks |= 1u << DECK_TYPE_GHOST;
    if (g_profile.won_decks & (1u << DECK_TYPE_GREEN)) g_profile.unlocked_decks |= 1u << DECK_TYPE_ABANDONED;
    if (g_profile.won_decks & (1u << DECK_TYPE_BLACK)) g_profile.unlocked_decks |= 1u << DECK_TYPE_CHECKERED;

    if (g_profile.highest_stake_win >= 1) g_profile.unlocked_decks |= 1u << DECK_TYPE_ZODIAC;
    if (g_profile.highest_stake_win >= 2) g_profile.unlocked_decks |= 1u << DECK_TYPE_PAINTED;
    if (g_profile.highest_stake_win >= 3) g_profile.unlocked_decks |= 1u << DECK_TYPE_ANAGLYPH;
    if (g_profile.highest_stake_win >= 4) g_profile.unlocked_decks |= 1u << DECK_TYPE_PLASMA;
    if (g_profile.highest_stake_win >= 5) g_profile.unlocked_decks |= 1u << DECK_TYPE_ERRATIC;
}

bool profile_save()
{
    FILE *file = fopen(profile_path, "wb");
    if (!file) return false;

    struct ProfileHeader header;
    memset(&header, 0, sizeof(header));
    memcpy(header.magic, PROFILE_MAGIC, SAVE_MAGIC_SIZE);
    header.version = PROFILE_VERSION;
    header.profile_size = sizeof(g_profile);

    bool ok = fwrite(&header, 1, sizeof(header), file) == sizeof(header);
    if (ok) ok = fwrite(&g_profile, 1, sizeof(g_profile), file) == sizeof(g_profile);
    fclose(file);
    return ok;
}

void profile_load()
{
    profile_init_defaults();

    FILE *file = fopen(profile_path, "rb");
    if (!file)
    {
        profile_save();
        return;
    }

    struct ProfileHeader header;
    bool ok = fread(&header, 1, sizeof(header), file) == sizeof(header);
    if (ok &&
        memcmp(header.magic, PROFILE_MAGIC, SAVE_MAGIC_SIZE) == 0 &&
        header.version == PROFILE_VERSION &&
        header.profile_size == sizeof(g_profile))
    {
        ok = fread(&g_profile, 1, sizeof(g_profile), file) == sizeof(g_profile);
    }
    fclose(file);

    if (!ok) profile_init_defaults();
    profile_update_deck_unlocks();
    profile_save();
}

bool profile_is_deck_unlocked(int deck_type)
{
    if (deck_type < 0 || deck_type >= DECK_TYPE_COUNT) return false;
    profile_update_deck_unlocks();
    return (g_profile.unlocked_decks & (1u << deck_type)) != 0;
}

int profile_get_discovered_items_count()
{
    int count = 0;
    for (int i = 0; i < 5; i++) count += profile_popcount(g_profile.discovered_jokers[i]);
    count += profile_popcount(g_profile.discovered_tarots);
    count += profile_popcount(g_profile.discovered_planets);
    count += profile_popcount(g_profile.discovered_spectrals);
    count += profile_popcount(g_profile.discovered_boosters);
    count += profile_popcount(g_profile.discovered_decks);
    return count;
}

void profile_discover_joker(int joker_type)
{
    if (joker_type < 0 || joker_type >= 160) return;
    uint32_t mask = 1u << (joker_type % 32);
    if (!(g_profile.discovered_jokers[joker_type / 32] & mask))
    {
        g_profile.discovered_jokers[joker_type / 32] |= mask;
        profile_update_deck_unlocks();
        profile_save();
    }
}

void profile_discover_tarot(int tarot_type)
{
    if (tarot_type < 0 || tarot_type >= 32) return;
    uint32_t mask = 1u << tarot_type;
    if (!(g_profile.discovered_tarots & mask))
    {
        g_profile.discovered_tarots |= mask;
        profile_update_deck_unlocks();
        profile_save();
    }
}

void profile_discover_planet(int planet_type)
{
    if (planet_type < 0 || planet_type >= 32) return;
    uint32_t mask = 1u << planet_type;
    if (!(g_profile.discovered_planets & mask))
    {
        g_profile.discovered_planets |= mask;
        profile_update_deck_unlocks();
        profile_save();
    }
}

void profile_discover_spectral(int spectral_type)
{
    if (spectral_type < 0 || spectral_type >= 32) return;
    uint32_t mask = 1u << spectral_type;
    if (!(g_profile.discovered_spectrals & mask))
    {
        g_profile.discovered_spectrals |= mask;
        profile_update_deck_unlocks();
        profile_save();
    }
}

void profile_discover_booster(int booster_type)
{
    if (booster_type < 0 || booster_type >= 32) return;
    uint32_t mask = 1u << booster_type;
    if (!(g_profile.discovered_boosters & mask))
    {
        g_profile.discovered_boosters |= mask;
        profile_update_deck_unlocks();
        profile_save();
    }
}

void profile_discover_deck(int deck_type)
{
    if (deck_type < 0 || deck_type >= DECK_TYPE_COUNT) return;
    uint32_t mask = 1u << deck_type;
    if (!(g_profile.discovered_decks & mask))
    {
        g_profile.discovered_decks |= mask;
        profile_update_deck_unlocks();
        profile_save();
    }
}

void profile_mark_run_won(int deck_type)
{
    if (deck_type < 0 || deck_type >= DECK_TYPE_COUNT) return;
    g_profile.won_decks |= 1u << deck_type;
    profile_discover_deck(deck_type);
    profile_update_deck_unlocks();
    profile_save();
}

void profile_debug_lock_all_progress()
{
    profile_init_defaults();
    profile_save();
}

void profile_debug_unlock_next_deck_tier()
{
    int discovered = profile_get_discovered_items_count();

    if (discovered < 20)
    {
        for (int i = 0; i < 20; i++) profile_discover_joker(i);
    }
    else if (discovered < 50)
    {
        for (int i = 20; i < 50; i++) profile_discover_joker(i);
    }
    else if (discovered < 75)
    {
        for (int i = 50; i < 75; i++) profile_discover_joker(i);
    }
    else if (discovered < 100)
    {
        for (int i = 75; i < 100; i++) profile_discover_joker(i);
    }
    else if (!(g_profile.won_decks & (1u << DECK_TYPE_RED)))
    {
        profile_mark_run_won(DECK_TYPE_RED);
    }
    else if (!(g_profile.won_decks & (1u << DECK_TYPE_BLUE)))
    {
        profile_mark_run_won(DECK_TYPE_BLUE);
    }
    else if (!(g_profile.won_decks & (1u << DECK_TYPE_YELLOW)))
    {
        profile_mark_run_won(DECK_TYPE_YELLOW);
    }
    else if (!(g_profile.won_decks & (1u << DECK_TYPE_GREEN)))
    {
        profile_mark_run_won(DECK_TYPE_GREEN);
    }
    else if (!(g_profile.won_decks & (1u << DECK_TYPE_BLACK)))
    {
        profile_mark_run_won(DECK_TYPE_BLACK);
    }
    else if (g_profile.highest_stake_win < 5)
    {
        g_profile.highest_stake_win++;
        profile_update_deck_unlocks();
        profile_save();
    }
    else
    {
        g_profile.unlocked_decks = (1u << DECK_TYPE_COUNT) - 1;
        profile_save();
    }
}

static PspUtilitySavedataListSaveNewData newData;
static char *titleShow = "New Save";

static size_t save_payload_size()
{
    return sizeof(struct SaveHeader) + sizeof(g_game_state);
}

static size_t save_aligned_payload_size()
{
    return (save_payload_size() + 63) & ~63;
}

static void save_relocate_game_state_pointers(void *old_base_addr)
{
    long ptr_diff = (long)(&g_game_state.all_cards.cards[0]) - (long)old_base_addr;

    if (ptr_diff == 0) return;

    for(int i=0; i<g_game_state.full_deck.card_count; i++)
        g_game_state.full_deck.cards[i] = (struct Card*)((char*)g_game_state.full_deck.cards[i] + ptr_diff);

    for(int i=0; i<g_game_state.current_deck.card_count; i++)
        g_game_state.current_deck.cards[i] = (struct Card*)((char*)g_game_state.current_deck.cards[i] + ptr_diff);

    for(int i=0; i<g_game_state.hand.card_count; i++)
        g_game_state.hand.cards[i] = (struct Card*)((char*)g_game_state.hand.cards[i] + ptr_diff);

    for(int i=0; i<g_game_state.played_hand.card_count; i++)
        g_game_state.played_hand.cards[i] = (struct Card*)((char*)g_game_state.played_hand.cards[i] + ptr_diff);

    if (g_game_state.boss_forced_selected_card != NULL)
        g_game_state.boss_forced_selected_card = (struct Card*)((char*)g_game_state.boss_forced_selected_card + ptr_diff);

    for(int k=0; k<4; k++)
    {
        for(int i=0; i<g_game_state.deck_info.card_count[k]; i++)
        {
            g_game_state.deck_info.cards[k][i] = (struct Card*)((char*)g_game_state.deck_info.cards[k][i] + ptr_diff);
        }
    }
}

static bool save_pack_game_state(void *buffer, size_t buffer_size, size_t *data_size)
{
    size_t payload_size = save_payload_size();
    if (!buffer || buffer_size < payload_size) return false;

    struct SaveHeader header;
    memset(&header, 0, sizeof(header));
    memcpy(header.magic, SAVE_MAGIC, SAVE_MAGIC_SIZE);
    header.version = SAVE_VERSION;
    header.base_addr = &g_game_state.all_cards.cards[0];
    header.state_size = sizeof(g_game_state);

    memcpy(buffer, &header, sizeof(header));
    memcpy((char*)buffer + sizeof(header), &g_game_state, sizeof(g_game_state));
    if (data_size) *data_size = payload_size;
    return true;
}

static bool save_unpack_game_state(const void *buffer, size_t data_size)
{
    if (!buffer) return false;

    const struct SaveHeader *header = (const struct SaveHeader*)buffer;
    if (data_size >= sizeof(struct SaveHeader) &&
        memcmp(header->magic, SAVE_MAGIC, SAVE_MAGIC_SIZE) == 0 &&
        header->version == SAVE_VERSION &&
        header->state_size <= sizeof(g_game_state) &&
        data_size >= sizeof(struct SaveHeader) + header->state_size)
    {
        memset(&g_game_state, 0, sizeof(g_game_state));
        memcpy(&g_game_state, (const char*)buffer + sizeof(struct SaveHeader), header->state_size);
        if (header->state_size < sizeof(g_game_state)) g_game_state.deck_type = DECK_TYPE_RED;
        save_relocate_game_state_pointers(header->base_addr);
        return true;
    }

    if (data_size > sizeof(void*))
    {
        void *old_base_addr;
        size_t state_size = MIN(data_size - sizeof(void*), sizeof(g_game_state));
        memcpy(&old_base_addr, buffer, sizeof(void*));
        memset(&g_game_state, 0, sizeof(g_game_state));
        memcpy(&g_game_state, (const char*)buffer + sizeof(void*), state_size);
        if (state_size < sizeof(g_game_state)) g_game_state.deck_type = DECK_TYPE_RED;
        save_relocate_game_state_pointers(old_base_addr);
        return true;
    }

    return false;
}

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

    size_t state_size = save_payload_size();
    size_t aligned_state_size = save_aligned_payload_size();
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

bool save_autosave_exists()
{
    if (autosave_exists_cache >= 0) return autosave_exists_cache != 0;

    FILE *file = fopen(autosave_path, "rb");
    if (!file)
    {
        autosave_exists_cache = 0;
        return false;
    }
    fclose(file);
    autosave_exists_cache = 1;
    return true;
}

bool save_write_autosave()
{
    size_t data_size = 0;
    size_t buffer_size = save_aligned_payload_size();
    void *buffer = memalign(64, buffer_size);
    if (!buffer) return false;

    memset(buffer, 0, buffer_size);
    if (!save_pack_game_state(buffer, buffer_size, &data_size))
    {
        free(buffer);
        return false;
    }

    FILE *file = fopen(autosave_path, "wb");
    if (!file)
    {
        free(buffer);
        return false;
    }

    bool ok = fwrite(buffer, 1, data_size, file) == data_size;
    fclose(file);
    free(buffer);
    if (ok) autosave_exists_cache = 1;
    save_debug_log("autosave write: %s", ok ? "ok" : "failed");
    return ok;
}

bool save_load_autosave()
{
    FILE *file = fopen(autosave_path, "rb");
    if (!file)
    {
        autosave_exists_cache = 0;
        return false;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (file_size <= 0)
    {
        fclose(file);
        return false;
    }

    void *buffer = malloc(file_size);
    if (!buffer)
    {
        fclose(file);
        return false;
    }

    bool ok = fread(buffer, 1, file_size, file) == (size_t)file_size;
    fclose(file);

    if (ok) ok = save_unpack_game_state(buffer, file_size);
    free(buffer);
    save_debug_log("autosave load: %s", ok ? "ok" : "failed");
    return ok;
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

    size_t data_size = 0;
    if (save_pack_game_state(dialog.dataBuf, dialog.dataBufSize, &data_size))
    {
        dialog.dataSize = data_size;
    }

    sceKernelDcacheWritebackAll();

    int init_res = sceUtilitySavedataInitStart(&dialog);
    save_debug_log("InitStart result: %08x", init_res);
    process_dialog_loop();

    free(dialog.dataBuf);

    extern void audio_resume();
    audio_resume();

    save_debug_log("=== SAVE UTILITY COMPLETE ===");
}

bool run_load_utility()
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



    bool loaded = false;
    if (dialog.base.result == 0) // SUCCESS
    {
        loaded = save_unpack_game_state(dialog.dataBuf, dialog.dataSize);
    }

    free(dialog.dataBuf);

    extern void audio_resume();
    audio_resume();

    save_debug_log("=== LOAD UTILITY COMPLETE ===");
    return loaded;
}
