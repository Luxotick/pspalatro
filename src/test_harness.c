// Temporary in-game regression tests for issue #6.
// Built only with `make TEST=1`; runs every scenario headlessly, prints
// PASS/FAIL lines and dumps screenshots (BMP) next to the EBOOT.

#include <pspdisplay.h>

static int g_test_pass = 0;
static int g_test_fail = 0;
static unsigned int g_test_buttons = 0;
static FILE *g_test_log = NULL;

static void test_log(const char *fmt, ...)
{
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (!g_test_log) g_test_log = fopen("test_results.txt", "w");
    if (g_test_log) { fputs(buf, g_test_log); fflush(g_test_log); }
}

#define TCHECK(cond, ...) do { \
    if (cond) { g_test_pass++; test_log("PASS: "); } \
    else { g_test_fail++; test_log("FAIL: "); } \
    test_log(__VA_ARGS__); test_log("\n"); \
} while (0)

static bool test_idle()
{
    return g_automated_event_stack_pos < 0 && g_event_count == 0;
}

static void test_frame()
{
    memcpy(&g_last_pad, &g_pad, sizeof(SceCtrlData));
    memset(&g_pad, 0, sizeof(SceCtrlData));
    g_pad.Buttons = g_test_buttons;
    game_update();
    game_draw();
}

static void test_step(int frames)
{
    for (int i = 0; i < frames; i++) test_frame();
}

static int test_wait_idle(int max_frames)
{
    int i = 0;
    for (; i < max_frames; i++)
    {
        if (test_idle()) break;
        test_frame();
    }
    test_step(2);
    return i;
}

static void test_press(unsigned int button)
{
    g_test_buttons = button;
    test_frame();
    g_test_buttons = 0;
    test_frame();
}

static void test_screenshot(const char *name)
{
    void *addr = NULL;
    int width = 0, fmt = 0;
    sceDisplayGetFrameBuf(&addr, &width, &fmt, PSP_DISPLAY_SETBUF_NEXTFRAME);
    if (addr == NULL) { test_log("SHOT %s: no framebuffer\n", name); return; }

    char path[64];
    snprintf(path, sizeof(path), "shot_%s.bmp", name);
    FILE *f = fopen(path, "wb");
    if (!f) { test_log("SHOT %s: cannot open file\n", name); return; }

    const int w = SCREEN_WIDTH, h = SCREEN_HEIGHT;
    uint32_t data_size = w * h * 3;
    uint8_t header[54] = { 'B', 'M' };
    uint32_t file_size = 54 + data_size;
    memcpy(header + 2, &file_size, 4);
    uint32_t v = 54; memcpy(header + 10, &v, 4);
    v = 40; memcpy(header + 14, &v, 4);
    int32_t iw = w, ih = h; memcpy(header + 18, &iw, 4); memcpy(header + 22, &ih, 4);
    uint16_t planes = 1, bpp = 24; memcpy(header + 26, &planes, 2); memcpy(header + 28, &bpp, 2);
    memcpy(header + 34, &data_size, 4);
    fwrite(header, 1, 54, f);

    uint8_t *row = malloc(w * 3);
    for (int y = h - 1; y >= 0; y--)
    {
        for (int x = 0; x < w; x++)
        {
            uint8_t r, g, b;
            if (fmt == PSP_DISPLAY_PIXEL_FORMAT_8888)
            {
                uint32_t p = ((uint32_t *)((uintptr_t)addr | 0x40000000))[y * width + x];
                r = p & 0xFF; g = (p >> 8) & 0xFF; b = (p >> 16) & 0xFF;
            }
            else if (fmt == PSP_DISPLAY_PIXEL_FORMAT_4444)
            {
                uint16_t p = ((uint16_t *)((uintptr_t)addr | 0x40000000))[y * width + x];
                r = (p & 0xF) * 17; g = ((p >> 4) & 0xF) * 17; b = ((p >> 8) & 0xF) * 17;
            }
            else if (fmt == PSP_DISPLAY_PIXEL_FORMAT_5551)
            {
                uint16_t p = ((uint16_t *)((uintptr_t)addr | 0x40000000))[y * width + x];
                r = (p & 0x1F) << 3; g = ((p >> 5) & 0x1F) << 3; b = ((p >> 10) & 0x1F) << 3;
            }
            else
            {
                uint16_t p = ((uint16_t *)((uintptr_t)addr | 0x40000000))[y * width + x];
                r = (p & 0x1F) << 3; g = ((p >> 5) & 0x3F) << 2; b = ((p >> 11) & 0x1F) << 3;
            }
            row[x * 3 + 0] = b; row[x * 3 + 1] = g; row[x * 3 + 2] = r;
        }
        fwrite(row, 1, w * 3, f);
    }
    free(row);
    fclose(f);
    test_log("SHOT %s\n", path);
}

static struct Card *test_find_card(int rank, int suit)
{
    for (int i = 0; i < g_game_state.all_cards.card_count; i++)
    {
        struct Card *c = &g_game_state.all_cards.cards[i];
        if (c->rank == rank && c->suit == suit) return c;
    }
    return NULL;
}

// Put `card` on top of the current deck so the next draw picks it.
static void test_put_on_deck_top(struct Card *card)
{
    struct CardReferences *deck = &g_game_state.current_deck;
    for (int i = 0; i < deck->card_count; i++)
    {
        if (deck->cards[i] == card)
        {
            deck->cards[i] = deck->cards[deck->card_count - 1];
            deck->cards[deck->card_count - 1] = card;
            return;
        }
    }
    deck->cards[deck->card_count++] = card;
}

static void test_new_run(int deck_type)
{
    game_start_new_run(deck_type);
    test_wait_idle(600);
}

static void test_add_joker(int type)
{
    struct Joker joker;
    memset(&joker, 0, sizeof(joker));
    joker.type = type;
    joker.edition = CARD_EDITION_BASE;
    game_add_joker(&joker);
    game_set_initial_final_joker_position();
}

// Enter a round against the given boss and let the opening draw finish.
static void test_enter_boss_round(int boss)
{
    g_game_state.blind = GAME_BLIND_BOSS;
    g_game_state.boss_blind_type = boss;
    game_go_to_stage(GAME_STAGE_INGAME, GAME_SUBSTAGE_INGAME_PICK_HAND);
    test_wait_idle(1200);
}

static void test_select_only(int hand_index)
{
    for (int i = 0; i < g_game_state.hand.card_count; i++) g_game_state.hand.cards[i]->selected = false;
    g_game_state.hand.cards[hand_index]->selected = true;
    g_game_state.selected_cards_count = 1;
    struct Card *cards[1] = { g_game_state.hand.cards[hand_index] };
    int scoring[5], scoring_count = 0;
    g_game_state.current_poker_hand = game_get_selected_poker_hand(cards, 1, scoring, &scoring_count);
    g_game_state.current_base_chips = g_game_state.poker_hand_base_chips[g_game_state.current_poker_hand];
    g_game_state.current_base_mult = g_game_state.poker_hand_base_mult[g_game_state.current_poker_hand];
}

// ---------------------------------------------------------------------------

static void test_1_2_face_down_hover()
{
    test_log("== T1/T2 face-down card hover (Blue Deck, The Fish)\n");
    test_new_run(DECK_TYPE_BLUE);
    test_enter_boss_round(BOSS_BLIND_FISH);
    for (int i = 0; i < g_game_state.hand.card_count; i++) g_game_state.hand.cards[i]->face_down = true;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_HAND;
    g_game_state.highlighted_item = 0;
    test_press(INPUT_BUTTON_RIGHT);   // hover card 1 -> starts the white flash
    test_step(4);
    test_screenshot("t1_t2_facedown_hover");
}

static void test_3_boss_effect_in_shop()
{
    test_log("== T3 boss effects must not leak into the shop\n");
    test_new_run(DECK_TYPE_BLUE);
    test_enter_boss_round(BOSS_BLIND_MARK);

    struct Card *king = test_find_card(CARD_RANK_KING, CARD_SUIT_HEARTS);
    test_put_on_deck_top(king);
    game_draw_card_into_hand();
    TCHECK(king->face_down, "control: The Mark flips a face card face down during the round");

    // Leave the round for the shop (blind stays BOSS while shopping)
    test_add_joker(JOKER_TYPE_JOKER);
    g_game_state.jokers.jokers[0].face_down = true;   // e.g. left over by Amber Acorn
    g_game_state.jokers.jokers[0].disabled = true;    // e.g. left over by Crimson Heart
    automated_event_clear();
    event_init();
    game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_MAIN);
    test_wait_idle(600);
    TCHECK(!g_game_state.jokers.jokers[0].face_down, "jokers are face up again in the shop");
    TCHECK(!g_game_state.jokers.jokers[0].disabled, "jokers are re-enabled in the shop");

    game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_BOOSTER_HAND);
    game_util_copy_deck(&g_game_state.full_deck, &g_game_state.current_deck);
    g_game_state.hand.card_count = 0;
    struct Card *queen = test_find_card(CARD_RANK_QUEEN, CARD_SUIT_SPADES);
    test_put_on_deck_top(queen);
    game_draw_card_into_hand();
    TCHECK(!queen->face_down, "The Mark does not flip cards drawn for an Arcana pack");

    g_game_state.boss_blind_type = BOSS_BLIND_CLUB;
    struct Card *club = test_find_card(CARD_RANK_5, CARD_SUIT_CLUBS);
    TCHECK(!game_util_is_card_debuffed(club), "The Club does not debuff cards in the shop");

    g_game_state.boss_blind_type = BOSS_BLIND_MANACLE;
    int hs = game_util_get_hand_size();
    g_game_state.blind = GAME_BLIND_SMALL;
    TCHECK(hs == game_util_get_hand_size(), "The Manacle does not shrink the Arcana pack hand");
    g_game_state.blind = GAME_BLIND_BOSS;
}

static void test_4_cerulean_bell()
{
    test_log("== T4 Cerulean Bell forced card\n");
    test_new_run(DECK_TYPE_RED);
    test_enter_boss_round(BOSS_BLIND_CERULEAN_BELL);
    struct Card *forced = g_game_state.boss_forced_selected_card;
    TCHECK(forced != NULL && forced->selected, "a card is forced at round start");
    TCHECK(forced != NULL && forced->draw.y <= game_get_hand_y() - 9.0f, "forced card is raised without any input (y=%.1f)", forced ? forced->draw.y : -1.0f);
    test_screenshot("t4_cerulean_start");

    // DNA: playing a single card as first hand refills the hand by itself -> no draw happens
    test_add_joker(JOKER_TYPE_DNA);
    test_press(INPUT_BUTTON_SQUARE);  // play the forced card
    test_wait_idle(4000);
    TCHECK(g_game_state.hand.card_count == game_util_get_hand_size(), "DNA refilled the hand (%d cards)", g_game_state.hand.card_count);
    TCHECK(g_game_state.boss_forced_selected_card != NULL && g_game_state.boss_forced_selected_card->selected,
        "a card is forced after a hand that needed no draw");

    // Discard with an empty deck -> nothing to draw
    g_game_state.current_deck.card_count = 0;
    g_game_state.current_discards = 1;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_HAND;
    test_press(INPUT_BUTTON_TRIANGLE);
    test_wait_idle(4000);
    TCHECK(g_game_state.boss_forced_selected_card != NULL && g_game_state.boss_forced_selected_card->selected,
        "a card is forced after a discard that drew nothing");
}

static void test_5_6_mouth_and_scoring_hint()
{
    test_log("== T5 The Mouth blocked hand / T6 hint during scoring\n");
    test_new_run(DECK_TYPE_RED);
    test_add_joker(JOKER_TYPE_JOKER);
    test_enter_boss_round(BOSS_BLIND_MOUTH);
    g_game_state.boss_played_hand_mask = 1 << GAME_POKER_HAND_PAIR;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_HAND;
    g_game_state.highlighted_item = 0;
    test_select_only(0);
    double score_before = g_game_state.score;
    int money_before = g_game_state.wealth;
    double sel_chips = g_game_state.current_base_chips, sel_mult = g_game_state.current_base_mult;

    test_press(INPUT_BUTTON_SQUARE);
    double max_chips = 0, max_mult = 0;
    bool popup = false, shot = false;
    for (int i = 0; i < 4000 && !test_idle(); i++)
    {
        test_frame();
        if (g_game_state.current_base_chips > max_chips) max_chips = g_game_state.current_base_chips;
        if (g_game_state.current_base_mult > max_mult) max_mult = g_game_state.current_base_mult;
        if (g_game_state.score_number.show_score_number) popup = true;
        if (i == 45) { test_screenshot("t5_t6_mouth_blocked"); shot = true; }
    }
    TCHECK(shot, "screenshot taken during the blocked hand");
    TCHECK(max_chips <= sel_chips && max_mult <= sel_mult, "chips/mult never count up for a disallowed hand (max %.0f x %.0f, base %.0f x %.0f)", max_chips, max_mult, sel_chips, sel_mult);
    TCHECK(!popup, "no scoring pop-ups for a disallowed hand");
    TCHECK(g_game_state.score == score_before, "round score unchanged (%.0f)", g_game_state.score);
    TCHECK(g_game_state.wealth == money_before, "money unchanged");

    // T6: a normal (allowed) hand, screenshot mid-scoring
    g_game_state.boss_played_hand_mask = 0;
    g_game_state.highlighted_item = 3;
    test_select_only(0);
    test_press(INPUT_BUTTON_SQUARE);
    test_step(70);
    test_screenshot("t6_scoring_no_hint");
    test_wait_idle(4000);
}

static void test_7_tarot_money_preview()
{
    test_log("== T7 Hermit / Temperance current payout\n");
    test_new_run(DECK_TYPE_RED);
#ifndef BASELINE
    g_game_state.wealth = 13;
    TCHECK(game_util_get_tarot_money_preview(TAROT_TYPE_HERMIT) == 13, "Hermit preview with $13 is $13");
    g_game_state.wealth = 45;
    TCHECK(game_util_get_tarot_money_preview(TAROT_TYPE_HERMIT) == 20, "Hermit preview is capped at $20");
    g_game_state.wealth = -3;
    TCHECK(game_util_get_tarot_money_preview(TAROT_TYPE_HERMIT) == 0, "Hermit preview in debt is $0");
    test_add_joker(JOKER_TYPE_JOKER);
    test_add_joker(JOKER_TYPE_DNA);
    int expected = game_util_get_joker_sell_price(&g_game_state.jokers.jokers[0]) + game_util_get_joker_sell_price(&g_game_state.jokers.jokers[1]);
    TCHECK(game_util_get_tarot_money_preview(TAROT_TYPE_TEMPERANCE) == expected, "Temperance preview equals joker sell value ($%d)", expected);
#else
    test_add_joker(JOKER_TYPE_JOKER);
    test_add_joker(JOKER_TYPE_DNA);
#endif

    g_game_state.wealth = 13;
    game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_MAIN);
    test_wait_idle(600);
    g_game_state.shop.opened_booster.type = BOOSTER_PACK_TYPE_ARCANA;
    g_game_state.shop.opened_booster.size = BOOSTER_PACK_SIZE_NORMAL;
    game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_BOOSTER_HAND);
    test_wait_idle(600);
    g_game_state.shop.booster_items[0].type = ITEM_TYPE_TAROT;
    game_init_tarot(&g_game_state.shop.booster_items[0].info.tarot, TAROT_TYPE_TEMPERANCE, CARD_EDITION_BASE);
    g_game_state.shop.booster_items[1].type = ITEM_TYPE_TAROT;
    game_init_tarot(&g_game_state.shop.booster_items[1].info.tarot, TAROT_TYPE_HERMIT, CARD_EDITION_BASE);
    game_set_initial_final_shop_booster_item_position();
    test_wait_idle(600);
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_BOOSTER_ITEMS;
    g_game_state.highlighted_item = 0;
    test_step(30);
    test_screenshot("t7_temperance_hint");
    g_game_state.highlighted_item = 1;
    test_step(30);
    test_screenshot("t7_hermit_hint_t8_skip");
}

static void test_9_boss_text()
{
    test_log("== T9 boss blind description readability\n");
    test_new_run(DECK_TYPE_RED);
    g_game_state.boss_blind_type = BOSS_BLIND_HOOK;
    g_game_state.blind = GAME_BLIND_LARGE;
    game_go_to_next_blind();
    test_wait_idle(600);
    test_step(5);
    test_screenshot("t9_blind_select_hook");
    g_game_state.boss_blind_type = BOSS_BLIND_VERDANT_LEAF;
    test_step(5);
    test_screenshot("t9_blind_select_verdant");

    test_enter_boss_round(BOSS_BLIND_SERPENT);
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_RUN_INFO;
    test_step(5);
    test_screenshot("t9_ingame_run_info_boss");
}

static void test_10_autosave_after_loss()
{
    test_log("== T10 losing a run removes the autosave\n");
    test_new_run(DECK_TYPE_RED);
    TCHECK(save_autosave_exists(), "control: a new run writes an autosave");
    test_enter_boss_round(BOSS_BLIND_HOOK);
    g_game_state.current_hands = 1;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_HAND;
    test_select_only(0);
    test_press(INPUT_BUTTON_SQUARE);
    test_wait_idle(4000);
    TCHECK(g_game_state.sub_stage == GAME_SUBSTAGE_INGAME_LOST_END, "run is lost");
    TCHECK(!save_autosave_exists(), "autosave is gone after losing");

    // Pause -> Main Menu from the game over screen used to re-write it
    game_go_to_stage(GAME_STAGE_PAUSE_MENU, 0);
    game_go_back_to_previous_stage();
    save_write_autosave();
    TCHECK(!save_autosave_exists(), "autosave is not re-created from the game over screen");
    FILE *f = fopen("autosave.bin", "rb");
    TCHECK(f == NULL, "autosave.bin is not on disk");
    if (f) fclose(f);
}

void test_harness_run()
{
    test_log("=== PSPalatro issue #6 test run ===\n");
    test_1_2_face_down_hover();
    test_3_boss_effect_in_shop();
    test_4_cerulean_bell();
    test_5_6_mouth_and_scoring_hint();
    test_7_tarot_money_preview();
    test_9_boss_text();
    test_10_autosave_after_loss();
    test_log("=== RESULT: %d passed, %d failed ===\n", g_test_pass, g_test_fail);
    if (g_test_log) fclose(g_test_log);
    sceKernelExitGame();
}
