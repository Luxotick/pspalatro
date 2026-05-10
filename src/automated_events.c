// Auto actions

#include "global.h"

float g_current_speedup = 1.0f;
int g_score_count = 0;

#define VARIABLE_TIME(t)    ((int)((float)t * g_current_speedup))

#define SPEEDUP_MIN     0.25f
#define SPEEDUP_STEP    0.8f

void inc_score_count()
{
    g_score_count++;
    if (g_score_count % 6 == 0)
    {
        if (g_current_speedup >= SPEEDUP_MIN)
        {
            g_current_speedup *= SPEEDUP_STEP;
            g_current_speedup = MAX(g_current_speedup, SPEEDUP_MIN);
        }
    }
}

bool automated_event_score();
bool automated_event_discard();
bool automated_event_hand_card_swap();
bool automated_event_draw_cards();
bool automated_event_cash_out();
bool automated_event_sort_hand();
bool automated_event_buy_joker();
bool automated_event_sell_joker();
bool automated_event_buy_consumable();
bool automated_event_sell_consumable();
bool automated_event_use_planet();
bool automated_event_use_tarot();
bool automated_event_add_score();
bool automated_event_wait();
bool automated_event_select_blind();
bool automated_event_ceremonial_dagger();
bool automated_event_open_booster();
bool automated_event_add_booster_card();

struct AutomatedEventType
{
    int type;
    bool (*func)(void);
} g_automated_event_types[AUTOMATED_EVENT_COUNT] = {
    { AUTOMATED_EVENT_NONE                , NULL },
    { AUTOMATED_EVENT_SCORE               , automated_event_score },
    { AUTOMATED_EVENT_DISCARD             , automated_event_discard },
    { AUTOMATED_EVENT_HAND_CARD_SWAP      , automated_event_hand_card_swap },
    { AUTOMATED_EVENT_DRAW                , automated_event_draw_cards },
    { AUTOMATED_EVENT_CASH_OUT            , automated_event_cash_out },
    { AUTOMATED_EVENT_SORT_HAND           , automated_event_sort_hand },
    { AUTOMATED_EVENT_BUY_JOKER           , automated_event_buy_joker },
    { AUTOMATED_EVENT_SELL_JOKER          , automated_event_sell_joker },
    { AUTOMATED_EVENT_BUY_CONSUMABLE      , automated_event_buy_consumable },
    { AUTOMATED_EVENT_SELL_CONSUMABLE     , automated_event_sell_consumable },
    { AUTOMATED_EVENT_USE_PLANET          , automated_event_use_planet },
    { AUTOMATED_EVENT_USE_TAROT           , automated_event_use_tarot },
    { AUTOMATED_EVENT_ADD_SCORE           , automated_event_add_score },
    { AUTOMATED_EVENT_WAIT                , automated_event_wait },
    { AUTOMATED_EVENT_SELECT_BLIND        , automated_event_select_blind },
    { AUTOMATED_EVENT_CEREMONIAL_DAGGER   , automated_event_ceremonial_dagger },
    { AUTOMATED_EVENT_OPEN_BOOSTER        , automated_event_open_booster },
    { AUTOMATED_EVENT_ADD_BOOSTER_CARD    , automated_event_add_booster_card }
};

#define AUTO_EVENT_MAX_PARAMS 20

struct AutomatedEvent
{
    int event_id;
    int stage;

    int params[AUTO_EVENT_MAX_PARAMS];
    struct Planet *planet;
    struct Tarot *tarot;

    uint32_t frame_count;
};

#define MAX_AUTOMATED_EVENTS 20

struct AutomatedEvent   g_automated_event_stack[MAX_AUTOMATED_EVENTS];
int                     g_automated_event_stack_pos = -1;
struct AutomatedEvent  *g_automated_event = NULL;

void automated_event_inc_stage()
{
    g_automated_event->frame_count = -1;
    g_automated_event->stage++;
}

void automated_event_set_stage(int stage)
{    
    g_automated_event->frame_count = -1;
    g_automated_event->stage = stage;
}

int get_scoring_card_index(int n)
{
    int count = 0;
    for(int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        if (g_game_state.played_hand.cards[i]->selected)
        {
            if (count == n) return i;
            count++;
        }
    }

    return -1;
}

bool g_automated_event_leave = false;

#define AUTO_EVENT_START() \
    switch(g_automated_event->stage) \
    { \
        case 0:

#define AUTO_EVENT_STAGE() \
        g_automated_event->frame_count = 0; \
    case __LINE__: \
        if (g_automated_event->stage != __LINE__) DEBUG_PRINTF("AUTO EVENT stage %d\n", __LINE__); \
        g_automated_event->stage = __LINE__; \
        if (g_automated_event_leave) \
        { \
            g_automated_event_leave = false; \
            break; \
        }        

#define AUTO_EVENT_NAMED_STAGE(name) \
    g_automated_event->frame_count = 0; \
    case __LINE__: \
    STAGE_##name: \
        if (g_automated_event->stage != __LINE__) DEBUG_PRINTF("AUTO EVENT stage " #name "\n"); \
        g_automated_event->stage = __LINE__; \
        if (g_automated_event_leave) \
        { \
            g_automated_event_leave = false; \
            break; \
        }

#define AUTO_EVENT_STAGE_WAIT(duration) \
    if (g_automated_event->stage != __LINE__) DEBUG_PRINTF("AUTO EVENT stage %d\n", __LINE__); \
    g_automated_event->stage = __LINE__; \
    g_automated_event->frame_count = 0; \
    case __LINE__: \
        if (g_automated_event_leave) \
        { \
            g_automated_event_leave = false; \
            break; \
        } \
        { \
            if (g_automated_event->frame_count < duration) \
            { \
                break;\
            }\
        }

#define AUTO_EVENT_GO_TO_STAGE(name) \
    g_automated_event->frame_count = 0; \
    goto STAGE_##name;

#define AUTO_EVENT_GO_TO_END() \
    goto __auto_event_end;

#define AUTO_EVENT_END() \
        g_automated_event->stage = __LINE__; \
        default: \
            if (g_automated_event_leave) \
            { \
                g_automated_event_leave = false; \
                break; \
            } \
            __auto_event_end: \
            { \
                if (g_automated_event == &(g_automated_event_stack[g_automated_event_stack_pos])) \
                { \
                    automated_event_pop(); \
                } \
            } \
    } \
    __auto_event_post_end:

#define AUTO_EVENT_CALL(event_id, num, ...) \
    automated_event_push(event_id, num, __VA_ARGS__); \
    g_automated_event_leave = true;

#define AUTO_EVENT_EXIT_AND_CALL(event_id, num, ...) \
    automated_event_pop(); \
    automated_event_push(event_id, num, __VA_ARGS__); \
    goto __auto_event_post_end;

#define AUTO_EVENT_VAL(id)                              (g_automated_event->params[id])
#define AUTO_EVENT_SET_VAL(id, val)                     g_automated_event->params[id] = (val);
#define AUTO_EVENT_INC(id)                              g_automated_event->params[id]++;
#define AUTO_EVENT_DEC(id)                              g_automated_event->params[id]--;

#define MOVE_TIMESPAN   10
#define SCORE_TIMESPAN  40

#define WAIT_TIMESPAN   0

bool automated_event_wait()
{
    AUTO_EVENT_START()
    AUTO_EVENT_STAGE_WAIT(AUTO_EVENT_VAL(WAIT_TIMESPAN))
    AUTO_EVENT_END()
    return true;
}

struct DrawObject *aux_automated_event_get_card_draw(int card_index, int card_location)
{
    switch(card_location)
    {
        case EVENT_CARD_LOCATION_HAND:
            return &(g_game_state.hand.cards[card_index]->draw);
        case EVENT_CARD_LOCATION_PLAYED:
            return &(g_game_state.played_hand.cards[card_index]->draw);
        case EVENT_CARD_LOCATION_JOKER:
            return &(g_game_state.jokers.jokers[card_index].draw);
    }
    return NULL;
}

#define ADD_SCORE_PARAM_CHIPS           0
#define ADD_SCORE_PARAM_MULT            1
#define ADD_SCORE_PARAM_MULT_MULT_10    2
#define ADD_SCORE_PARAM_MONEY           3
#define ADD_SCORE_CARD_INDEX            4
#define ADD_SCORE_CARD_LOCATION         5
#define SCORE_PARAM_RELATED_JOKER_INDEX 6

bool automated_event_add_score()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        inc_score_count();

        if (AUTO_EVENT_VAL(ADD_SCORE_PARAM_CHIPS) > 0)
        {
            if (AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX) >= 0)
            {
                event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX), EVENT_CARD_LOCATION_JOKER), VARIABLE_TIME(SCORE_TIMESPAN));
            }
            g_game_state.current_base_chips += AUTO_EVENT_VAL(ADD_SCORE_PARAM_CHIPS);
            event_add_show_number(AUTO_EVENT_VAL(ADD_SCORE_PARAM_CHIPS), AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION), SCORE_NUMBER_ADD_CHIPS, VARIABLE_TIME(SCORE_TIMESPAN));
            event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION)), VARIABLE_TIME(SCORE_TIMESPAN));
        }
        else
        {
            AUTO_EVENT_GO_TO_STAGE(ADD_SCORE_MULT)
        }
    }

    AUTO_EVENT_STAGE_WAIT(VARIABLE_TIME(SCORE_TIMESPAN))

    AUTO_EVENT_NAMED_STAGE(ADD_SCORE_MULT)
    {
        if (AUTO_EVENT_VAL(ADD_SCORE_PARAM_MULT) > 0)
        {
            if (AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX) >= 0)
            {
                event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX), EVENT_CARD_LOCATION_JOKER), VARIABLE_TIME(SCORE_TIMESPAN));
            }
            g_game_state.current_base_mult += AUTO_EVENT_VAL(ADD_SCORE_PARAM_MULT);
            event_add_show_number(AUTO_EVENT_VAL(ADD_SCORE_PARAM_MULT), AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION), SCORE_NUMBER_ADD_MULT, VARIABLE_TIME(SCORE_TIMESPAN));
            event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION)), VARIABLE_TIME(SCORE_TIMESPAN));
        }
        else
        {
            AUTO_EVENT_GO_TO_STAGE(ADD_SCORE_MULT_MULT)
        }
    }

    AUTO_EVENT_STAGE_WAIT(VARIABLE_TIME(SCORE_TIMESPAN))

    AUTO_EVENT_NAMED_STAGE(ADD_SCORE_MULT_MULT)
    {
        if (AUTO_EVENT_VAL(ADD_SCORE_PARAM_MULT_MULT_10) > 0)
        {
            if (AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX) >= 0)
            {
                event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX), EVENT_CARD_LOCATION_JOKER), VARIABLE_TIME(SCORE_TIMESPAN));
            }
            double mult_mult = AUTO_EVENT_VAL(ADD_SCORE_PARAM_MULT_MULT_10) / 100.0;
            g_game_state.current_base_mult *= mult_mult;
            event_add_show_number(mult_mult, AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION), SCORE_NUMBER_MULT_MULT, VARIABLE_TIME(SCORE_TIMESPAN));
            event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION)), VARIABLE_TIME(SCORE_TIMESPAN));
        }
        else
        {
            AUTO_EVENT_GO_TO_STAGE(ADD_SCORE_MONEY)
        }
    }

    AUTO_EVENT_STAGE_WAIT(VARIABLE_TIME(SCORE_TIMESPAN))

    AUTO_EVENT_NAMED_STAGE(ADD_SCORE_MONEY)
    {
        if (AUTO_EVENT_VAL(ADD_SCORE_PARAM_MONEY) > 0)
        {
            if (AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX) >= 0)
            {                
                event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(SCORE_PARAM_RELATED_JOKER_INDEX), EVENT_CARD_LOCATION_JOKER), VARIABLE_TIME(SCORE_TIMESPAN));
            }
            g_game_state.wealth += AUTO_EVENT_VAL(ADD_SCORE_PARAM_MONEY);
            event_add_show_number(AUTO_EVENT_VAL(ADD_SCORE_PARAM_MONEY), AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION), SCORE_NUMBER_ADD_MONEY, VARIABLE_TIME(SCORE_TIMESPAN));
            event_add_pop_item(aux_automated_event_get_card_draw(AUTO_EVENT_VAL(ADD_SCORE_CARD_INDEX), AUTO_EVENT_VAL(ADD_SCORE_CARD_LOCATION)), VARIABLE_TIME(SCORE_TIMESPAN));
        }
        else
        {
            AUTO_EVENT_GO_TO_END()
        }
    }

    AUTO_EVENT_STAGE_WAIT(VARIABLE_TIME(SCORE_TIMESPAN))

    AUTO_EVENT_END()

    return true;
}

#define SCORE_PARAM_SCORING_CARD_COUNTER    0
#define SCORE_PARAM_CURRENT_SCORING_CARD    1
#define SCORE_PARAM_CURRENT_JOKER_FOR_CARD  2
#define SCORE_PARAM_HAND_CARD_COUNTER       3
#define SCORE_PARAM_FINAL_JOKER_COUNTER     4
#define SCORE_PARAM_HAND_JOKER_COUNTER      5
#define SCORE_PARAM_HAND_CARD_HAS_SCORED    6
#define SCORE_PARAM_HAND_CARD_REPEAT        7
#define SCORE_PARAM_SCORING_CARD_REPEAT     8

void automated_event_score_init_jokers()
{
    for(int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        switch(game_util_get_joker_type(i))
        {
            case JOKER_TYPE_MIME:
            case JOKER_TYPE_DUSK:
            case JOKER_TYPE_HACK:
            case JOKER_TYPE_SOCK_AND_BUSKIN:
                g_game_state.jokers.jokers[i].repeat = 1;
                break;
            case JOKER_TYPE_HANGING_CHAD:
                g_game_state.jokers.jokers[i].repeat = 2;
                break;
        }
    }
}

bool automated_event_score()
{
    AUTO_EVENT_START()
    
    AUTO_EVENT_STAGE() // Move cards to played area
    {
        g_current_speedup = powf(SPEEDUP_STEP, (float)(g_settings.speed - 1));
        g_current_speedup = MAX(g_current_speedup, SPEEDUP_MIN);
        g_score_count = 0;
        g_freeze_cards = true;
        
        // Init jokers
        automated_event_score_init_jokers();

        game_move_selected_cards_to_played_cards();
        game_set_hand_y(DRAW_HAND_Y_IN_SCORE);
        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, -1, 0, 0, MOVE_TIMESPAN);
        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_PLAYED, -1, 0, 0, MOVE_TIMESPAN);
    }

    AUTO_EVENT_STAGE_WAIT(MOVE_TIMESPAN * 3)

    AUTO_EVENT_STAGE() // Move scoring cards into position
    {
        game_set_scoring_cards_in_played_hand();
        if (g_game_state.current_poker_hand == GAME_POKER_HAND_FIVE_OF_A_KIND) g_game_state.five_of_a_kind_enabled = true;
        if (g_game_state.current_poker_hand == GAME_POKER_HAND_FLUSH_FIVE) g_game_state.flush_five_enabled = true;
        if (g_game_state.current_poker_hand == GAME_POKER_HAND_FLUSH_HOUSE) g_game_state.flush_house_enabled = true;
        game_increment_played_poker_hand(g_game_state.current_poker_hand);
        g_game_state.stats.cards_played += g_game_state.scoring_card_count;
        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_PLAYED, -1, 0, 0, MOVE_TIMESPAN);
        g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER] = 0;
        g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT] = 0;
        g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD] = 0;
    }

    AUTO_EVENT_STAGE_WAIT(MOVE_TIMESPAN)

    // *************
    //   Pre-Score
    // *************

    // Pre-scoring (i.e. Vampire Joker, Runner Joker, Square Joker, Green Joker, etc)
    AUTO_EVENT_NAMED_STAGE(SCORE_PRE_SCORE_JOKER)
    {
        switch (game_util_get_joker_type(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)))
        {
            case JOKER_TYPE_DNA:
            {
                if (g_game_state.played_hand.card_count == 1 && g_game_state.hands_played_in_round == 0)
                {
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                    struct Card *card = game_set_new_card();
                    *card = *g_game_state.played_hand.cards[0];
                    card->selected = false;
                    game_add_card_to_full_deck(card);
                    game_add_card_to_hand(card);
                    card->draw.x = card->draw.initial_x = g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)].draw.x;
                    card->draw.y = card->draw.initial_y = g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)].draw.y;
                    game_sort_hand();
                    event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, -1, 0, 0, 10);
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                }
                break;
            }
            case JOKER_TYPE_VAGABOND:
            {
                if (g_game_state.wealth <= 4)
                {
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                    if (game_util_has_room_in_consumables())
                    {
                        int excluded_tarots[100];
                        int excluded_tarots_count = 0;
                        for (int j = 0; j < g_game_state.consumables.item_count; j++) { if (g_game_state.consumables.items[j].type == ITEM_TYPE_TAROT) {excluded_tarots[excluded_tarots_count++] = g_game_state.consumables.items[j].tarot.type;} }
                        int tarot_type = game_util_get_new_tarot_type(excluded_tarots, excluded_tarots_count);
                        g_game_state.consumables.items[g_game_state.consumables.item_count].type = ITEM_TYPE_TAROT;
                        game_init_tarot(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot), tarot_type, CARD_EDITION_BASE);
                        game_set_object_off_screen(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot.draw));
                        g_game_state.consumables.item_count++;
                        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
                    }
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                }
                break;
            }
            case JOKER_TYPE_SUPERPOSITION:
            {
                if (g_game_state.current_poker_hand == GAME_POKER_HAND_STRAIGHT && game_util_has_scoring_cards_rank(CARD_RANK_ACE))
                {
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                    if (game_util_has_room_in_consumables())
                    {
                        int excluded_tarots[100];
                        int excluded_tarots_count = 0;
                        for (int j = 0; j < g_game_state.consumables.item_count; j++) { if (g_game_state.consumables.items[j].type == ITEM_TYPE_TAROT) {excluded_tarots[excluded_tarots_count++] = g_game_state.consumables.items[j].tarot.type;} }
                        int tarot_type = game_util_get_new_tarot_type(excluded_tarots, excluded_tarots_count);
                        g_game_state.consumables.items[g_game_state.consumables.item_count].type = ITEM_TYPE_TAROT;
                        game_init_tarot(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot), tarot_type, CARD_EDITION_BASE);
                        game_set_object_off_screen(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot.draw));
                        g_game_state.consumables.item_count++;
                        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
                    }
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                }
                break;
            }
            case JOKER_TYPE_SPACE_JOKER:
            {
                if (game_util_chance_occurs(1, 4))
                {
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                    int planet_type = game_util_get_planet_type_of_poker_hand(g_game_state.current_poker_hand);
                    g_game_state.poker_hand_base_chips[g_game_state.current_poker_hand] += g_planet_types[planet_type].chips;
                    g_game_state.poker_hand_base_mult[g_game_state.current_poker_hand] += g_planet_types[planet_type].mult;
                    g_game_state.poker_hand_level[g_game_state.current_poker_hand]++;

                    g_game_state.current_base_chips = g_game_state.poker_hand_base_chips[g_game_state.current_poker_hand];
                    g_game_state.current_base_mult = g_game_state.poker_hand_base_mult[g_game_state.current_poker_hand];
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                }
                break;
            }
            case JOKER_TYPE_TO_DO_LIST:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD));
                if (g_game_state.current_poker_hand == param)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 0, 4, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
        }

        switch (game_util_get_real_joker_type(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)))
        {
            case JOKER_TYPE_SPARE_TROUSERS:
            {
                if (game_util_played_hand_contains_two_pair())
                {
                    int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD));
                    game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), param + 1);
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                }
                break;
            }
            case JOKER_TYPE_RIDE_THE_BUS:
            {
                if (game_util_played_hand_contains_scoring_face())
                {
                    int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD));
                    if (param > 0)
                    {
                        game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), 0);
                        event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                        AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                    }
                }
                else
                {
                    int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD));
                    game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), param + 1);
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                }
                break;
            }
            case JOKER_TYPE_MIDAS_MASK:
            {
                bool used = false;
                for (int i = 0; i < g_game_state.played_hand.card_count; i++)
                {
                    if (g_game_state.played_hand.cards[i]->selected &&
                        game_util_is_card_face(g_game_state.played_hand.cards[i]))
                    {
                        g_game_state.played_hand.cards[i]->enhancement = CARD_ENHANCEMENT_GOLD;
                        event_add_pop_item(&(g_game_state.played_hand.cards[i]->draw), VARIABLE_TIME(30));
                        used = true;

                        g_joker_types[JOKER_TYPE_GOLDEN_TICKET].enabled = true;
                    }
                }
                if (used)
                {
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), VARIABLE_TIME(30));
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(30))
                }
                break;
            }
            case JOKER_TYPE_GREEN_JOKER:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD));
                game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), param + 1);
                event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD), 30);
                break;
            }
        }
    }

    AUTO_EVENT_STAGE()
    {
        g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD]++;
        if (g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD] < g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(SCORE_PRE_SCORE_JOKER)
        }
    }

    // *************
    //  Score cards
    // *************

    AUTO_EVENT_NAMED_STAGE(SCORE_ADD_CHIPS) // Add chips
    {
        g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD] = get_scoring_card_index(g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER]);
                
        uint32_t chips = game_util_get_card_chips(g_game_state.played_hand.cards[g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD]]);

        AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, chips, 0, 0, 0, g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD], EVENT_CARD_LOCATION_PLAYED, -1)
    }

    AUTO_EVENT_STAGE() // Enhancements
    {
        g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD] = 0;
        switch (g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)]->enhancement)
        {
            case CARD_ENHANCEMENT_BONUS:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 30, 0, 0, 0, g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD], EVENT_CARD_LOCATION_PLAYED, -1)
                break;
            case CARD_ENHANCEMENT_MULT:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 4, 0, 0, g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD], EVENT_CARD_LOCATION_PLAYED, -1)
                break;
            case CARD_ENHANCEMENT_GLASS:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 200, 0, g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD], EVENT_CARD_LOCATION_PLAYED, -1)
                break;
            case CARD_ENHANCEMENT_LUCKY:
                {
                    bool has_mult = game_util_chance_occurs(1, 5);
                    bool has_money = game_util_chance_occurs(1, 15);
                    if (has_mult || has_money)
                    {
                        for (int i = 0; i < g_game_state.jokers.joker_count; i++)
                        {
                            if (game_util_get_real_joker_type(i) == JOKER_TYPE_LUCKY_CAT)
                            {
                                g_game_state.jokers.jokers[i].param1 += 25;
                                event_add_pop_joker(i, VARIABLE_TIME(30));
                            }
                        }
                        AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, (has_mult ? 20 : 0), 0, (has_money ? 20 : 0), AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, -1)
                    }
                }
                break;
            default:
                break;
        }
    }

    AUTO_EVENT_STAGE() // Editions
    {
        switch (g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)]->edition)
        {
            case CARD_EDITION_FOIL:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 50, 0, 0, 0, g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD], EVENT_CARD_LOCATION_PLAYED, -1)
                break;
            case CARD_EDITION_HOLOGRAPHIC:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 10, 0, 0, g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD], EVENT_CARD_LOCATION_PLAYED, -1)
                break;
            case CARD_EDITION_POLYCHROME:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 150, 0, g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD], EVENT_CARD_LOCATION_PLAYED, -1)
                break;
            default:
                break;
        }
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_CARD_JOKER) // Joker per card
    {        
        switch (game_util_get_joker_type(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)))
        {
            case JOKER_TYPE_GREEDY_JOKER:
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_DIAMONDS))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 3, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))                    
                }
                break;
            case JOKER_TYPE_LUSTY_JOKER:
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_HEARTS))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 3, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))                    
                }
                break;
            case JOKER_TYPE_WRATHFUL_JOKER:
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_SPADES))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 3, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))                    
                }
                break;
            case JOKER_TYPE_GLUTTONOUS_JOKER:
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_CLUBS))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 3, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))                    
                }
                break;
            case JOKER_TYPE_FIBONACCI:
                if (game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_ACE) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_2) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_3) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_5) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_8))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 8, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))                    
                }
                break;
            case JOKER_TYPE_SCARY_FACE:
                if (game_util_is_card_face(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)]))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 30, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))                    
                }
                break;
            case JOKER_TYPE_EVEN_STEVEN:
                if (game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_2) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_4) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_6) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_8) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_10))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 4, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))                    
                }
                break;
            case JOKER_TYPE_ODD_TODD:
                if (game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_ACE) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_3) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_5) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_7) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_9))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 31, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_SCHOLAR:
                if (game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_ACE))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 20, 4, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_BUSINESS_CARD:
                if (game_util_is_card_face(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)]) & game_util_chance_occurs(1, 2))
                {                    
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 0, 2, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_HACK:
                {
                    struct Card *card = g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)];
                    struct Joker *joker = &(g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)]);                    
                    if (game_util_is_card_rank(card, CARD_RANK_2) ||
                        game_util_is_card_rank(card, CARD_RANK_3) ||
                        game_util_is_card_rank(card, CARD_RANK_4) ||
                        game_util_is_card_rank(card, CARD_RANK_5))                        
                    {
                        if (joker->repeat > 0)
                        {
                            joker->repeat--;
                            event_add_pop_joker(g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], VARIABLE_TIME(SCORE_TIMESPAN));
                            event_add_show_number_joker(0.0, g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], SCORE_NUMBER_AGAIN, VARIABLE_TIME(SCORE_TIMESPAN));
                            AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(SCORE_TIMESPAN))
                            g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT]++;
                        }
                    }
                }
                break;
            case JOKER_TYPE_DUSK:
                {
                    struct Joker *joker = &(g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)]);                    
                    if (g_game_state.current_hands == 0)
                    {
                        if (joker->repeat > 0)
                        {
                            joker->repeat--;
                            event_add_pop_joker(g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], VARIABLE_TIME(SCORE_TIMESPAN));
                            event_add_show_number_joker(0.0, g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], SCORE_NUMBER_AGAIN, VARIABLE_TIME(SCORE_TIMESPAN));
                            AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(SCORE_TIMESPAN))
                            g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT]++;
                        }
                    }
                }
                break;
            case JOKER_TYPE_HIKER:
                {
                    struct Card *card = g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)];
                    card->extra_chips += 5;
                    event_add_pop_joker(g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], VARIABLE_TIME(SCORE_TIMESPAN));
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(SCORE_TIMESPAN))
                }
                break;
            case JOKER_TYPE_PHOTOGRAPH:
            {
                if (game_util_get_first_scoring_face_card_index() == AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 200, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            }
            case JOKER_TYPE_SOCK_AND_BUSKIN:
            {
                struct Card *card = g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)];
                struct Joker *joker = &(g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)]);
                if (game_util_is_card_face(card))
                {                    
                    if (joker->repeat > 0)
                    {
                        joker->repeat--;
                        event_add_pop_joker(g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], SCORE_TIMESPAN);
                        event_add_show_number_joker(0.0, g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], SCORE_NUMBER_AGAIN, VARIABLE_TIME(SCORE_TIMESPAN));
                        AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(SCORE_TIMESPAN))
                        g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT]++;
                    }
                }
                break;
            }
            case JOKER_TYPE_HANGING_CHAD:
            {
                struct Joker *joker = &(g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD)]);
                if (game_util_get_index_first_scoring_card() == AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD))
                {                    
                    if (joker->repeat > 0)
                    {
                        joker->repeat--;
                        event_add_pop_joker(g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], VARIABLE_TIME(SCORE_TIMESPAN));
                        event_add_show_number_joker(0.0, g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD], SCORE_NUMBER_AGAIN, VARIABLE_TIME(SCORE_TIMESPAN));
                        AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(SCORE_TIMESPAN))
                        g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT]++;
                    }
                }
                break;
            }
            case JOKER_TYPE_WALKIE_TALKIE:
                if (game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_10) ||
                    game_util_is_card_rank(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_RANK_4)) 
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 10, 4, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_SMILEY_FACE:
                if (game_util_is_card_face(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)])) 
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 5, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_ROUGH_GEM:
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_DIAMONDS))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 0, 1, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_BLOODSTONE:
                if (game_util_chance_occurs(1, 2) && game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_HEARTS))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 150, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_ARROWHEAD:
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_SPADES))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 50, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_ONYX_AGATE:
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], CARD_SUIT_CLUBS))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 7, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            case JOKER_TYPE_ANCIENT_JOKER:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD));
                if (game_util_is_card_suit(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)], param))
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 150, 0, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
                break;
            }
            case JOKER_TYPE_GOLDEN_TICKET:
            {
                if (g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)]->enhancement == CARD_ENHANCEMENT_GOLD)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 0, 4, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD), EVENT_CARD_LOCATION_PLAYED, AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_JOKER_FOR_CARD))
                }
            }
            default:
                break;
        }
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_CARD_NEXT_JOKER)
    {        
        g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD]++;
        if (g_automated_event->params[SCORE_PARAM_CURRENT_JOKER_FOR_CARD] < g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(SCORE_CARD_JOKER)
        }
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_NEXT_CARD) // Move to next card
    {
        if (g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT] > 0)
        {
            g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT]--;
            AUTO_EVENT_GO_TO_STAGE(SCORE_ADD_CHIPS)
        }
        else
        {
            automated_event_score_init_jokers();
            g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER]++;
            if (g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER] < g_game_state.scoring_card_count)
            {
                AUTO_EVENT_GO_TO_STAGE(SCORE_ADD_CHIPS)
            }
            g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER] = 0;
            g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER] = 0;
        }
    }

    // ******************
    //  Score hand cards
    // ******************

    AUTO_EVENT_NAMED_STAGE(SCORE_HAND) // Score hand cards
    {
        if (g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER] == g_game_state.hand.card_count)
        {            
            g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER] = 0;
            AUTO_EVENT_GO_TO_STAGE(SCORE_JOKERS)
        }

        g_automated_event->params[SCORE_PARAM_HAND_CARD_HAS_SCORED] = 0;

        if (g_game_state.hand.cards[g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER]]->enhancement == CARD_ENHANCEMENT_STEEL)
        {
            AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 150, 0, AUTO_EVENT_VAL(SCORE_PARAM_HAND_CARD_COUNTER), EVENT_CARD_LOCATION_HAND, -1)
            g_automated_event->params[SCORE_PARAM_HAND_CARD_HAS_SCORED] = 1;            
        }
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_HAND_CARD_JOKER) // Check each Joker for each card in hand
    {
        if (g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER] >= g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(SCORE_NEXT_CARD_HAND)
        }

        struct Joker *joker = &(g_game_state.jokers.jokers[g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER]]);
        switch(game_util_get_joker_type(g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER]))
        {
            case JOKER_TYPE_MIME:
            {
                if (joker->repeat > 0)
                {
                    joker->repeat--;
                    event_add_pop_joker(g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER], VARIABLE_TIME(SCORE_TIMESPAN));
                    event_add_shake_item(&(g_game_state.hand.cards[g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER]]->draw), VARIABLE_TIME(SCORE_TIMESPAN));
                    event_add_show_number_joker(0.0, g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER], SCORE_NUMBER_AGAIN, VARIABLE_TIME(SCORE_TIMESPAN));                    
                    g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT]++;
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, VARIABLE_TIME(SCORE_TIMESPAN))                    
                }
                break;
            }
            case JOKER_TYPE_BARON:
            {
                if (g_game_state.hand.cards[g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER]]->rank == CARD_RANK_KING)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 150, 0, AUTO_EVENT_VAL(SCORE_PARAM_HAND_CARD_COUNTER), EVENT_CARD_LOCATION_HAND, AUTO_EVENT_VAL(SCORE_PARAM_HAND_JOKER_COUNTER))
                }
                break;
            }
        }
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_HAND_CARD_NEXT_JOKER) // Move to next Joker for each card in hand
    {        
        g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER]++;
        AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_CARD_JOKER)
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_NEXT_CARD_HAND) // Move to next hand card
    {
        g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER] = 0;
        if (g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT] > 0)
        {            
            g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT]--;
            AUTO_EVENT_GO_TO_STAGE(SCORE_HAND)
        }
        else
        {
            automated_event_score_init_jokers();
            g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER]++;
            AUTO_EVENT_GO_TO_STAGE(SCORE_HAND)
        }
    }

    // **************
    //  Score jokers
    // **************

    AUTO_EVENT_NAMED_STAGE(SCORE_JOKERS) // Score Jokers
    {
        if (g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER] == g_game_state.jokers.joker_count)
        {
            if (g_game_state.score + g_game_state.current_base_chips * g_game_state.current_base_mult < game_get_current_blind_score())
            {
                AUTO_EVENT_GO_TO_STAGE(DISCARD_DEALT_HAND)
            }
            else
            {
                g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER] = 0;
                g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER] = 0;
                automated_event_score_init_jokers();
                AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_2)
            }
        }

        switch (game_util_get_joker_type(g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER]))
        {
            case JOKER_TYPE_JOKER:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 4, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            case JOKER_TYPE_JOLLY_JOKER:
                if (game_util_played_hand_contains_pair())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 8, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_ZANY_JOKER:
                if (game_util_played_hand_contains_three_of_a_kind())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 12, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_MAD_JOKER:
                if(game_util_played_hand_contains_two_pair())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 10, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_CRAZY_JOKER:
                if(game_util_played_hand_contains_straight())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 12, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_DROLL_JOKER:
                if(game_util_played_hand_contains_flush())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 10, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            case JOKER_TYPE_SLY_JOKER:
                if(game_util_played_hand_contains_pair())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 50, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_WILY_JOKER:
                if(game_util_played_hand_contains_three_of_a_kind())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 100, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_CLEVER_JOKER:
                if(game_util_played_hand_contains_two_pair())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 80, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_DEVIOUS_JOKER:
                if(game_util_played_hand_contains_straight())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 100, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_CRAFTY_JOKER:
                if(game_util_played_hand_contains_flush())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 80, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_HALF_JOKER:
                if(g_game_state.played_hand.card_count <= 3)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 20, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            case JOKER_TYPE_JOKER_STENCIL:
            {
                double mult = 1.0;
                if ((g_game_state.joker_slots + g_game_state.jokers.negative_count) - g_game_state.jokers.joker_count > 0)
                {
                    mult = 1.0 + (double)(g_game_state.joker_slots - g_game_state.jokers.joker_count);
                }
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, (int)(mult * 100), 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            }
            case JOKER_TYPE_BANNER:
            {                
                if (g_game_state.current_discards > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 30 * g_game_state.current_discards, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            }
            case JOKER_TYPE_MYSTIC_SUMMIT:
            {
                if (g_game_state.current_discards == 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 15, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            }
            case JOKER_TYPE_MISPRINT:
            {
                int mult = game_util_rand(0, 23);
                if (mult > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, mult, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            }
            case JOKER_TYPE_RAISED_FIST:
            {
                int rank = game_util_get_lowest_rank_held_in_hand();
                if (rank > -1)
                {
                    int mult = (rank + 2) * 2;
                    if (rank == CARD_RANK_ACE) mult = 11 * 2;
                    if (rank >= CARD_RANK_JACK && rank <= CARD_RANK_KING) mult = 10 * 2;
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, mult, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)                    
                }
                break;
            }
            case JOKER_TYPE_ABSTRACT_JOKER:
            {
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 3 * g_game_state.jokers.joker_count, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            }
            case JOKER_TYPE_CEREMONIAL_DAGGER:
            {
                int mult = g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER)].param1;
                if (mult > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, mult, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_BLUE_JOKER:
            {
                if (g_game_state.current_deck.card_count > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, g_game_state.current_deck.card_count * 2, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_GROS_MICHEL:
            {
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 15, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            }
            case JOKER_TYPE_CAVENDISH:
            {
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 300, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            }
            case JOKER_TYPE_SUPERNOVA:
            {
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, g_game_state.stats.poker_hands_played[g_game_state.current_poker_hand], 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            }
            case JOKER_TYPE_BULL:
            {
                if (g_game_state.wealth > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, g_game_state.wealth * 2, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_POPCORN:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, param * 4, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_SPARE_TROUSERS:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, param * 2, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_ACROBAT:
            {
                if (g_game_state.current_hands == 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 300, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_SWASHBUCKLER:
            {
                int value = 0;
                for (int i = 0; i < g_game_state.jokers.joker_count; i++)
                {
                    if (i != AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER))
                    {
                        value += game_util_get_joker_sell_price(&(g_game_state.jokers.jokers[i]));
                    }
                }
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, value, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            }
            case JOKER_TYPE_THE_DUO:
            {
                if (game_util_played_hand_contains_pair())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 200, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_THE_TRIO:
            {
                if (game_util_played_hand_contains_three_of_a_kind())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 300, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_THE_FAMILY:
            {
                if (game_util_played_hand_contains_four_of_a_kind())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 400, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_THE_ORDER:
            {
                if (game_util_played_hand_contains_straight())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 300, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_THE_TRIBE:
            {
                if (game_util_played_hand_contains_flush())
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 200, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_RIDE_THE_BUS:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, param, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_ICE_CREAM:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, param * 5, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_STUNTMAN:
            {
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 250, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            }
            case JOKER_TYPE_LUCKY_CAT:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 100)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, param, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            case JOKER_TYPE_GREEN_JOKER:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 0)
                {
                    AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, param, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                }
                break;
            }
            default:
                break;
        }
    }

    AUTO_EVENT_STAGE() // Joker Editions
    {
        switch (g_game_state.jokers.jokers[AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER)].edition)
        {
            case CARD_EDITION_FOIL:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 50, 0, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            case CARD_EDITION_HOLOGRAPHIC:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 10, 0, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            case CARD_EDITION_POLYCHROME:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 150, 0, AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                break;
            default:
                break;
        }
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_NEXT_JOKER) // Go to next Joker
    {
        g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER]++;        
        AUTO_EVENT_GO_TO_STAGE(SCORE_JOKERS)        
    }

    // ***************************************
    //  Process cards in hand at end of round
    //  Score hand cards #2
    // ***************************************

    AUTO_EVENT_NAMED_STAGE(SCORE_HAND_2) // Score hand cards
    {
        if (g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER] == g_game_state.hand.card_count)
        {
            AUTO_EVENT_SET_VAL(SCORE_PARAM_SCORING_CARD_COUNTER, 0)
            AUTO_EVENT_GO_TO_STAGE(CHECK_GLASS_CARDS)
        }

        g_automated_event->params[SCORE_PARAM_HAND_CARD_HAS_SCORED] = 0;

        if (g_game_state.hand.cards[g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER]]->enhancement == CARD_ENHANCEMENT_GOLD)
        {
            AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 0, 3, AUTO_EVENT_VAL(SCORE_PARAM_HAND_CARD_COUNTER), EVENT_CARD_LOCATION_HAND, -1)
            g_automated_event->params[SCORE_PARAM_HAND_CARD_HAS_SCORED] = 1;
        }
        else
        {
            AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_CARD_JOKER_2)
        }
    }

    AUTO_EVENT_STAGE_WAIT(VARIABLE_TIME(SCORE_TIMESPAN))

    AUTO_EVENT_NAMED_STAGE(SCORE_HAND_CARD_JOKER_2) // Check each Joker for each card in hand
    {
        if (g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER] >= g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(SCORE_NEXT_CARD_HAND_2)
        }

        g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT] = 0;
        struct Joker *joker = &(g_game_state.jokers.jokers[g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER]]);
        switch(game_util_get_joker_type(g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER]))
        {
            case JOKER_TYPE_MIME:
            {
                joker->repeat--;
                if (joker->repeat >= 0 && g_automated_event->params[SCORE_PARAM_HAND_CARD_HAS_SCORED] == 1)
                {                    
                    event_add_pop_joker(g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER], VARIABLE_TIME(SCORE_TIMESPAN));
                    event_add_show_number_joker(0.0, g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER], SCORE_NUMBER_AGAIN, VARIABLE_TIME(SCORE_TIMESPAN));                    
                    g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT] = 1;
                    break;
                }
                AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_CARD_NEXT_JOKER_2)
                break;
            }
            default:
                AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_CARD_NEXT_JOKER_2)
        }
    }

    AUTO_EVENT_STAGE_WAIT(VARIABLE_TIME(SCORE_TIMESPAN))

    AUTO_EVENT_NAMED_STAGE(SCORE_HAND_CARD_NEXT_JOKER_2) // Move to next Joker for each card in hand
    {
        if (g_automated_event->params[SCORE_PARAM_HAND_CARD_REPEAT] == 1)
        {
            AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_2)
        }
        g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER]++;
        AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_CARD_JOKER_2)
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_NEXT_CARD_HAND_2) // Move to next hand card
    {
        automated_event_score_init_jokers();
        g_automated_event->params[SCORE_PARAM_HAND_CARD_COUNTER]++;
        g_automated_event->params[SCORE_PARAM_HAND_JOKER_COUNTER] = 0;
        AUTO_EVENT_GO_TO_STAGE(SCORE_HAND_2)
    }

    // ********************
    //  Check glass cards
    // ********************

    AUTO_EVENT_NAMED_STAGE(CHECK_GLASS_CARDS)
    {
        g_automated_event->params[SCORE_PARAM_CURRENT_SCORING_CARD] = get_scoring_card_index(g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER]);

        if (g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)]->enhancement == CARD_ENHANCEMENT_GLASS)
        {
            if (game_util_chance_occurs(1, 4))
            {
                game_remove_card_from_full_deck(g_game_state.played_hand.cards[AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD)]);
                game_remove_card_from_played_hand(AUTO_EVENT_VAL(SCORE_PARAM_CURRENT_SCORING_CARD));
                g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER]--;
            }
        }
    }

    AUTO_EVENT_STAGE()
    {
        g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER]++;
        if (g_automated_event->params[SCORE_PARAM_SCORING_CARD_COUNTER] < g_game_state.scoring_card_count)
        {
            AUTO_EVENT_GO_TO_STAGE(CHECK_GLASS_CARDS)
        }
    }

    // ********************
    //  Discard dealt hand
    // ********************

    AUTO_EVENT_NAMED_STAGE(DISCARD_DEALT_HAND) // Discard dealt hand
    {
        g_game_state.score += g_game_state.current_base_chips * g_game_state.current_base_mult;
        g_game_state.current_poker_hand = GAME_POKER_HAND_NONE;
        g_game_state.current_base_chips = 0;
        g_game_state.current_base_mult = 0;

        for(int i = 0; i < g_game_state.played_hand.card_count; i++)
        {
            struct Card *card = g_game_state.played_hand.cards[i];
            event_add(EVENT_MOVE_CARD, 0, EVENT_CARD_LOCATION_PLAYED, i, SCREEN_WIDTH + 10, card->draw.y, 20);
        }
    }

    AUTO_EVENT_STAGE_WAIT(20)

    // ****************
    //  Move hand back
    // ****************

    AUTO_EVENT_STAGE() // Move hand back
    {
        game_ingame_discard_played_hand();

        game_set_hand_y(DRAW_HAND_Y);
        game_set_card_hand_initial_positions();
        game_set_card_hand_final_positions();
        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, -1, 0, 0, MOVE_TIMESPAN);

        AUTO_EVENT_SET_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER, 0)
    }

    AUTO_EVENT_STAGE_WAIT(MOVE_TIMESPAN)

    AUTO_EVENT_NAMED_STAGE(SCORE_END_PLAYED_HAND_JOKERS)
    {
        if (AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER) == g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(SCORE_CHECK_IF_END_ROUND)
        }

        switch (game_util_get_real_joker_type(g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER]))
        {
            case JOKER_TYPE_ICE_CREAM:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 0)
                {
                    game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), param - 1);
                    if (param - 1 == 0)
                    {
                        game_remove_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                        AUTO_EVENT_DEC(SCORE_PARAM_FINAL_JOKER_COUNTER)
                        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, -1, 0, 0, MOVE_TIMESPAN);
                    }
                    else
                    {
                        event_add_shake_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), SCORE_TIMESPAN);
                    }
                }
                break;
            }
        }
    }

    AUTO_EVENT_STAGE() // Go to next Joker
    {
        g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER]++;
        AUTO_EVENT_GO_TO_STAGE(SCORE_END_PLAYED_HAND_JOKERS)        
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_CHECK_IF_END_ROUND)
    {
        g_game_state.hands_played_in_round++;
        AUTO_EVENT_SET_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER, 0)
        if (g_game_state.current_hands > 0 && g_game_state.score < game_get_current_blind_score())
        {        
            if (game_util_get_hand_size() - g_game_state.hand.card_count > 0)
            {
                AUTO_EVENT_EXIT_AND_CALL(AUTOMATED_EVENT_DRAW, 1, game_util_get_hand_size() - g_game_state.hand.card_count);            
            }
            else
            {
                AUTO_EVENT_GO_TO_END()
            }
        }
    }

    // Go through the Jokers and execute the end of round events
    AUTO_EVENT_NAMED_STAGE(SCORE_END_ROUND_JOKERS)
    {
        if (AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER) == g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(END_ROUND)
        }

        switch (game_util_get_real_joker_type(g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER]))
        {
            case JOKER_TYPE_GROS_MICHEL:
            {
                if (game_util_chance_occurs(1, 6))
                {
                    game_remove_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                    AUTO_EVENT_DEC(SCORE_PARAM_FINAL_JOKER_COUNTER)
                    event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, -1, 0, 0, MOVE_TIMESPAN);
                    g_joker_types[JOKER_TYPE_CAVENDISH].enabled = true;
                }
                break;
            }
            case JOKER_TYPE_CAVENDISH:
            {
                if (game_util_chance_occurs(1, 1000))
                {
                    game_remove_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                    AUTO_EVENT_DEC(SCORE_PARAM_FINAL_JOKER_COUNTER)
                    event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, -1, 0, 0, MOVE_TIMESPAN);
                }
                break;
            }
            case JOKER_TYPE_POPCORN:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                if (param > 0)
                {
                    game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), param - 1);
                    if (param - 1 == 0)
                    {
                        game_remove_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER));
                        AUTO_EVENT_DEC(SCORE_PARAM_FINAL_JOKER_COUNTER)
                        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, -1, 0, 0, MOVE_TIMESPAN);
                    }
                    else
                    {
                        event_add_shake_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), SCORE_TIMESPAN);
                    }
                }
                break;
            }
            case JOKER_TYPE_ANCIENT_JOKER:
            {
                game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), game_util_rand(0, 3));
                event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), SCORE_TIMESPAN);
                break;
            }
            case JOKER_TYPE_TO_DO_LIST:
            {
                game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), game_util_get_random_poker_hand());
                event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), SCORE_TIMESPAN);
                break;
            }
            case JOKER_TYPE_ROCKET:
            {
                if (g_game_state.blind == GAME_BLIND_BOSS)
                {
                    game_util_set_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), game_util_get_joker_param(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER)) + 2);
                    event_add_pop_joker(AUTO_EVENT_VAL(SCORE_PARAM_FINAL_JOKER_COUNTER), SCORE_TIMESPAN);
                }
                break;
            }
            default:
                break;
        }
    }

    AUTO_EVENT_NAMED_STAGE(SCORE_END_ROUND_NEXT_JOKER) // Go to next Joker
    {
        g_automated_event->params[SCORE_PARAM_FINAL_JOKER_COUNTER]++;
        AUTO_EVENT_GO_TO_STAGE(SCORE_END_ROUND_JOKERS)
    }

    AUTO_EVENT_NAMED_STAGE(END_ROUND)
    {
        game_util_copy_deck(&g_game_state.full_deck, &g_game_state.current_deck);
        game_util_shuffle_deck(&g_game_state.current_deck);
        g_game_state.stats.max_score = MAX(g_game_state.stats.max_score, g_game_state.score);
        if (g_game_state.score >= game_get_current_blind_score())
        {            
            if (g_game_state.ante == 8 && g_game_state.blind == GAME_BLIND_BOSS)
            {
                game_go_to_stage(GAME_STAGE_INGAME, GAME_SUBSTAGE_INGAME_WON_END);
            }
            else
            {
                game_go_to_stage(GAME_STAGE_INGAME, GAME_SUBSTAGE_INGAME_WON);
                AUTO_EVENT_EXIT_AND_CALL(AUTOMATED_EVENT_CASH_OUT, 0, 0)
            }
        }
        else if (g_game_state.current_hands == 0)
        {
            g_game_state.stats.max_score = MAX(g_game_state.stats.max_score, g_game_state.score);
            game_go_to_stage(GAME_STAGE_INGAME, GAME_SUBSTAGE_INGAME_LOST_END);
        }
    }

    AUTO_EVENT_END()

    return true;
}

int get_first_selected_card()
{
    for (int i = 0; i < g_game_state.hand.card_count; i++)
    {
        if (g_game_state.hand.cards[i]->selected) return i;
    }

    return -1;
}

#define DISCARD_TIMESPAN    10
#define DISCARD_PARAM_JOKER_COUNTER 0

bool automated_event_discard()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        AUTO_EVENT_SET_VAL(DISCARD_PARAM_JOKER_COUNTER, 0)
    }

    AUTO_EVENT_NAMED_STAGE(DISCARD_JOKERS) // Discard jokers
    {
        if (AUTO_EVENT_VAL(DISCARD_PARAM_JOKER_COUNTER) == g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(DISCARD)
        }

        switch (game_util_get_joker_type(AUTO_EVENT_VAL(DISCARD_PARAM_JOKER_COUNTER)))
        {
            case JOKER_TYPE_FACELESS_JOKER:
            {
                int face_count = 0;
                for(int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected && game_util_is_card_face(g_game_state.hand.cards[i]))
                    {
                        face_count++;
                        if (face_count == 3)
                        {                            
                            AUTO_EVENT_CALL(AUTOMATED_EVENT_ADD_SCORE, 7, 0, 0, 0, 5, AUTO_EVENT_VAL(DISCARD_PARAM_JOKER_COUNTER), EVENT_CARD_LOCATION_JOKER, -1)
                            break;
                        }
                    }
                }                
                break;
            }
            default:
                break;
        }

        switch (game_util_get_real_joker_type(AUTO_EVENT_VAL(DISCARD_PARAM_JOKER_COUNTER)))
        {
            case JOKER_TYPE_GREEN_JOKER:
            {
                int param = game_util_get_joker_param(AUTO_EVENT_VAL(DISCARD_PARAM_JOKER_COUNTER));
                if (param > 0)
                {
                    game_util_set_joker_param(AUTO_EVENT_VAL(DISCARD_PARAM_JOKER_COUNTER), param - 1);
                    event_add_shake_joker(AUTO_EVENT_VAL(DISCARD_PARAM_JOKER_COUNTER), 30);
                }
                break;
            }
            default:
                break;
        }
    }

    AUTO_EVENT_NAMED_STAGE() // Go to next Joker
    {
        AUTO_EVENT_INC(DISCARD_PARAM_JOKER_COUNTER)
        AUTO_EVENT_GO_TO_STAGE(DISCARD_JOKERS)
    }

    AUTO_EVENT_NAMED_STAGE(DISCARD)
    {
        g_automated_event->params[0] = get_first_selected_card();
        if (g_automated_event->params[0] < 0)
        {
            int cards_to_draw = game_util_get_hand_size() - g_game_state.hand.card_count;
            g_game_state.selected_cards_count = 0;
            g_game_state.highlighted_item = 0;
            if (cards_to_draw > 0 && g_game_state.current_deck.card_count > 0)
            {
                automated_event_set(AUTOMATED_EVENT_DRAW, 1, cards_to_draw <= g_game_state.current_deck.card_count ? cards_to_draw : g_game_state.current_deck.card_count );                
                g_automated_event->frame_count = -1;
                return true;
            }
            else
            {
                event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, 0, 0, 0, DISCARD_TIMESPAN);
                AUTO_EVENT_GO_TO_END()
            }
        }
        struct Card *card = g_game_state.hand.cards[g_automated_event->params[0]];
        event_add(EVENT_MOVE_CARD, 0, EVENT_CARD_LOCATION_HAND, g_automated_event->params[0], SCREEN_WIDTH + 10, card->draw.y, DISCARD_TIMESPAN);
    }

    AUTO_EVENT_STAGE_WAIT(DISCARD_TIMESPAN)

    AUTO_EVENT_STAGE()
    {
        g_game_state.hand.cards[g_automated_event->params[0]]->selected = false;
        game_discard_card(g_automated_event->params[0]);
        AUTO_EVENT_GO_TO_STAGE(DISCARD)
    }

    AUTO_EVENT_END()

    return true;
}

#define CARD_SWAP_TIMESPAN  7

// params 0 -> 1st card/joker to be swapped
// params 1 -> 2nd card/joker to be swapped
// params 2 -> card joker location

bool automated_event_hand_card_swap()
{
    AUTO_EVENT_START()
    
    AUTO_EVENT_STAGE()
    {
        switch(g_automated_event->params[2])
        {
            case EVENT_CARD_LOCATION_HAND:
            {
                struct Card *temp_card = g_game_state.hand.cards[g_automated_event->params[1]];
                g_game_state.hand.cards[g_automated_event->params[1]] = g_game_state.hand.cards[g_automated_event->params[0]];
                g_game_state.hand.cards[g_automated_event->params[0]] = temp_card;
                break;
            }
            case EVENT_CARD_LOCATION_JOKER:
            {
                struct Joker temp_joker = g_game_state.jokers.jokers[g_automated_event->params[1]];
                g_game_state.jokers.jokers[g_automated_event->params[1]] = g_game_state.jokers.jokers[g_automated_event->params[0]];
                g_game_state.jokers.jokers[g_automated_event->params[0]] = temp_joker;                
                break;
            }
            case EVENT_CARD_LOCATION_CONSUMABLES:
            {
                struct ConsumableItem temp_item = g_game_state.consumables.items[g_automated_event->params[1]];
                g_game_state.consumables.items[g_automated_event->params[1]] = g_game_state.consumables.items[g_automated_event->params[0]];
                g_game_state.consumables.items[g_automated_event->params[0]] = temp_item;
                break;
            }
        }
        g_game_state.highlighted_item = g_automated_event->params[1];
        event_add(EVENT_ARRANGE_CARDS, 0, g_automated_event->params[2], 0, 0, 0, CARD_SWAP_TIMESPAN);
    }
    AUTO_EVENT_STAGE_WAIT(CARD_SWAP_TIMESPAN)

    AUTO_EVENT_END()

    return true;
}

#define DRAW_TIMESPAN    7

// params 0 -> number of cards to draw

bool automated_event_draw_cards()
{
    AUTO_EVENT_START()

    AUTO_EVENT_NAMED_STAGE(DRAW_CARD)
    {
        game_draw_card_into_hand();
        game_sort_hand();
        g_automated_event->params[0]--;

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, 0, 0, 0, DRAW_TIMESPAN);
    }

    AUTO_EVENT_STAGE_WAIT(DRAW_TIMESPAN)

    AUTO_EVENT_STAGE()
    {
        if (g_automated_event->params[0] > 0)
        {
            AUTO_EVENT_GO_TO_STAGE(DRAW_CARD)
        }
    }

    AUTO_EVENT_END()

    return true;
}

#define CASHOUT_LINE_TIMESPAN   20

#define CASHOUT_PARAM_JOKER_COUNTER   0

bool automated_event_cash_out()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE() // Rise cash out window
    {
        event_add_interpolate_value(&g_game_state.cash_out_panel_y, SCREEN_HEIGHT, 100.0f, 20);
    }

    AUTO_EVENT_STAGE_WAIT(20)
    
    AUTO_EVENT_STAGE() // Blind money
    {
        switch(g_game_state.blind)
        {
            case GAME_BLIND_SMALL:
                g_game_state.cash_out_blind = 3;
                g_game_state.cash_out_value += g_game_state.cash_out_blind;
                break;
            case GAME_BLIND_LARGE:
                g_game_state.cash_out_blind = 4;
                g_game_state.cash_out_value += g_game_state.cash_out_blind;
                break;
            case GAME_BLIND_BOSS:
                g_game_state.cash_out_blind = 5;
                g_game_state.cash_out_value += g_game_state.cash_out_blind;
                break;
        }
    }

    AUTO_EVENT_STAGE_WAIT(CASHOUT_LINE_TIMESPAN)

    AUTO_EVENT_STAGE() // Remaining hands
    {
        g_game_state.cash_out_hands = g_game_state.current_hands;
        g_game_state.cash_out_value += g_game_state.cash_out_hands;
    }

    AUTO_EVENT_STAGE_WAIT(CASHOUT_LINE_TIMESPAN)

    AUTO_EVENT_STAGE() // Interest money
    {
        int value_to_consider = CLAMP(g_game_state.wealth, 0, 25);
        g_game_state.cash_out_interest = value_to_consider / 5;
        g_game_state.cash_out_value += g_game_state.cash_out_interest;

        AUTO_EVENT_SET_VAL(CASHOUT_PARAM_JOKER_COUNTER, 0)        
    }

    AUTO_EVENT_STAGE_WAIT(CASHOUT_LINE_TIMESPAN)

    AUTO_EVENT_NAMED_STAGE(CASHOUT_JOKERS) // Cashout jokers
    {
        if (AUTO_EVENT_VAL(CASHOUT_PARAM_JOKER_COUNTER) == g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(END_ROUND)
        }

        switch (game_util_get_joker_type(AUTO_EVENT_VAL(CASHOUT_PARAM_JOKER_COUNTER)))
        {
            case JOKER_TYPE_GOLDEN_JOKER:
            {
                if (g_game_state.cash_out_jokers < 0) g_game_state.cash_out_jokers = 0;
                g_game_state.cash_out_jokers += 4;
                g_game_state.cash_out_value += g_game_state.cash_out_jokers;
                event_add_pop_joker(AUTO_EVENT_VAL(CASHOUT_PARAM_JOKER_COUNTER), 30);
                AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, 30)
                break;
            }
            case JOKER_TYPE_TO_THE_MOON:
            {
                if (g_game_state.cash_out_jokers < 0) g_game_state.cash_out_jokers = 0;
                int value_to_consider = CLAMP(g_game_state.wealth, 0, 25);
                g_game_state.cash_out_jokers += value_to_consider / 5;
                g_game_state.cash_out_value += g_game_state.cash_out_jokers;
                event_add_pop_joker(AUTO_EVENT_VAL(CASHOUT_PARAM_JOKER_COUNTER), 30);
                AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, 30)
                break;
            }
            case JOKER_TYPE_ROCKET:
            {
                if (g_game_state.cash_out_jokers < 0) g_game_state.cash_out_jokers = 0;
                g_game_state.cash_out_jokers += game_util_get_joker_param(AUTO_EVENT_VAL(CASHOUT_PARAM_JOKER_COUNTER));
                g_game_state.cash_out_value += g_game_state.cash_out_jokers;
                event_add_pop_joker(AUTO_EVENT_VAL(CASHOUT_PARAM_JOKER_COUNTER), 30);
                AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, 30)
                break;
            }
            default:
                break;
        }
    }

    AUTO_EVENT_NAMED_STAGE() // Go to next Joker
    {
        AUTO_EVENT_INC(CASHOUT_PARAM_JOKER_COUNTER)
        AUTO_EVENT_GO_TO_STAGE(CASHOUT_JOKERS)
    }

    AUTO_EVENT_NAMED_STAGE(END_ROUND)
    {
        g_game_state.cash_out_done = true;
    }

    AUTO_EVENT_END()

    return true;
}

bool automated_event_sort_hand()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        game_sort_hand();
        game_set_card_hand_initial_positions();
        game_set_card_hand_final_positions();

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, 0, 0, 0, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)
    
    AUTO_EVENT_END()

    return true;
}

bool automated_event_buy_joker()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        AUTO_EVENT_SET_VAL(0, g_game_state.input_focused_zone)

        g_game_state.input_focused_zone = -1;

        if (AUTO_EVENT_VAL(0) == INPUT_FOCUSED_ZONE_SHOP_SINGLES)
        {
            game_add_joker(&(g_game_state.shop.items[g_game_state.highlighted_item].info.joker));
            g_game_state.shop.items[g_game_state.highlighted_item].available = false;
            g_game_state.shop.item_count--;            
        }
        else if (AUTO_EVENT_VAL(0) == INPUT_FOCUSED_ZONE_BOOSTER_ITEMS)
        {
            game_add_joker(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.joker));
            g_game_state.shop.booster_items[g_game_state.highlighted_item].available = false;
            g_game_state.shop.booster_item_count--;
        }

        g_game_state.stats.cards_purchased++;

        game_set_initial_final_joker_position();

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, 0, 0, 0, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)
    
    AUTO_EVENT_STAGE()
    {
        if (AUTO_EVENT_VAL(0) == INPUT_FOCUSED_ZONE_SHOP_SINGLES)
        {
            if (g_game_state.shop.item_count > 0)
            {
                g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_SHOP_SINGLES;
                g_game_state.highlighted_item = game_util_get_first_shop_single_index();
            }
            else
            {
                g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_SHOP_NEXT_ROUND;
            }
        }
        else if (AUTO_EVENT_VAL(0) == INPUT_FOCUSED_ZONE_BOOSTER_ITEMS)
        {
            // Check if all items picked
            if (g_game_state.shop.booster_total_items - g_game_state.shop.booster_item_count == (g_game_state.shop.opened_booster.size == BOOSTER_PACK_SIZE_MEGA ? 2 : 1))
            {
                g_game_state.highlighted_item = 0;
                game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_MAIN);
            }
            else
            {
                g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_BOOSTER_ITEMS;
                g_game_state.highlighted_item = game_util_get_first_shop_booster_item_index();
            }
        }
    }

    AUTO_EVENT_END()

    return true;
}

bool automated_event_buy_consumable()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        g_game_state.input_focused_zone = -1;        

        g_game_state.consumables.items[g_game_state.consumables.item_count].type = g_game_state.shop.items[g_game_state.highlighted_item].type;
        g_game_state.consumables.items[g_game_state.consumables.item_count].planet = g_game_state.shop.items[g_game_state.highlighted_item].info.planet;
        g_game_state.consumables.items[g_game_state.consumables.item_count].tarot = g_game_state.shop.items[g_game_state.highlighted_item].info.tarot;
        g_game_state.consumables.item_count++;
        g_game_state.shop.items[g_game_state.highlighted_item].available = false;
        g_game_state.shop.item_count--;

        g_game_state.stats.cards_purchased++;

        game_set_initial_final_consumable_position();

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)
    
    AUTO_EVENT_STAGE()
    {
        if (g_game_state.shop.item_count > 0)
        {
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_SHOP_SINGLES;
            g_game_state.highlighted_item = game_util_get_first_shop_single_index();
        }
        else
        {
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_SHOP_NEXT_ROUND;
        }
    }

    AUTO_EVENT_END()

    return true;
}

bool automated_event_sell_joker()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        g_game_state.input_focused_zone = -1;       

        g_game_state.wealth += game_util_get_joker_sell_price(&(g_game_state.jokers.jokers[g_game_state.highlighted_item]));

        // for(int j = g_game_state.highlighted_item+1; j < g_game_state.jokers.joker_count; j++)
        // {
        //     g_game_state.jokers.jokers[j - 1] = g_game_state.jokers.jokers[j];
        // }
        // g_game_state.jokers.joker_count--;

        game_remove_joker(g_game_state.highlighted_item);

        g_game_state.highlighted_item--;
        if (g_game_state.highlighted_item < 0) g_game_state.highlighted_item = 0;

        game_set_initial_final_joker_position();

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, 0, 0, 0, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)
    
    AUTO_EVENT_STAGE()
    {
        // TODO: it should go to another focused zone if there are no jokers
        if (g_game_state.jokers.joker_count > 0)
        {
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_JOKERS;
        }
        else
        {
            g_game_state.input_focused_zone = game_util_get_default_focus_zone();
        }
    }

    AUTO_EVENT_END()

    return true;
}

bool automated_event_sell_consumable()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        g_game_state.input_focused_zone = -1;       

        switch (g_game_state.consumables.items[g_game_state.highlighted_item].type)
        {
            case ITEM_TYPE_PLANET:
            {
                g_game_state.wealth += game_util_get_planet_sell_price(&(g_game_state.consumables.items[g_game_state.highlighted_item].planet));
                break;
            }
            case ITEM_TYPE_TAROT:
            {
                g_game_state.wealth += game_util_get_tarot_sell_price(&(g_game_state.consumables.items[g_game_state.highlighted_item].tarot));
                break;
            }
            default:
                break;
        }

        for(int j = g_game_state.highlighted_item+1; j < g_game_state.consumables.item_count; j++)
        {
            g_game_state.consumables.items[j - 1] = g_game_state.consumables.items[j];
        }
        g_game_state.consumables.item_count--;        

        g_game_state.highlighted_item--;
        if (g_game_state.highlighted_item < 0) g_game_state.highlighted_item = 0;

        game_set_initial_final_consumable_position();

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)
    
    AUTO_EVENT_STAGE()
    {
        // TODO: it should go to another focused zone if there are no jokers
        if (g_game_state.consumables.item_count > 0)
        {
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_CONSUMABLES;
        }
        else
        {
            g_game_state.input_focused_zone = game_util_get_default_focus_zone();
        }
    }

    AUTO_EVENT_END()

    return true;
}

bool automated_event_use_planet()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        g_automated_event->params[0] = g_game_state.input_focused_zone;

        g_game_state.input_focused_zone = -1;

        switch(g_automated_event->params[0])
        {
            case INPUT_FOCUSED_ZONE_CONSUMABLES:
            {
                g_automated_event->planet = &g_game_state.consumables.items[g_game_state.highlighted_item].planet;
                break;
            }
            case INPUT_FOCUSED_ZONE_SHOP_SINGLES:
            {
                g_automated_event->planet = &g_game_state.shop.items[g_game_state.highlighted_item].info.planet;
                break;
            }
            case INPUT_FOCUSED_ZONE_BOOSTER_ITEMS:
            {
                g_automated_event->planet = &g_game_state.shop.booster_items[g_game_state.highlighted_item].info.planet;
                break;
            }
            default:
            {
                break;
            }
        }

        event_add_shake_item(&g_automated_event->planet->draw, 30);

        struct PlanetType *planet_type = &(g_planet_types[g_automated_event->planet->type]);
        int poker_hand = planet_type->poker_hand;
        g_game_state.current_poker_hand = poker_hand;
        g_game_state.current_base_chips = g_game_state.poker_hand_base_chips[poker_hand];
        g_game_state.current_base_mult = g_game_state.poker_hand_base_mult[poker_hand];

        g_game_state.last_used_consumable_item_type = ITEM_TYPE_PLANET;
        g_game_state.last_used_consumable_tarot_planet_type = g_automated_event->planet->type;
    }

    AUTO_EVENT_STAGE_WAIT(20)

    AUTO_EVENT_STAGE()
    {
        event_add_interpolate_value(&(g_automated_event->planet->draw.white_factor), 0.0f, 1.0f, 40);

        struct PlanetType *planet_type = &(g_planet_types[g_automated_event->planet->type]);
        int poker_hand = planet_type->poker_hand;
        g_game_state.poker_hand_base_chips[poker_hand] += planet_type->chips;
        g_game_state.poker_hand_base_mult[poker_hand] += planet_type->mult;
        g_game_state.poker_hand_level[poker_hand]++;

        g_game_state.current_poker_hand = poker_hand;
        g_game_state.current_base_chips = g_game_state.poker_hand_base_chips[poker_hand];
        g_game_state.current_base_mult = g_game_state.poker_hand_base_mult[poker_hand];
    }

    AUTO_EVENT_STAGE_WAIT(41)

    AUTO_EVENT_STAGE()
    {
        g_game_state.current_poker_hand = GAME_POKER_HAND_NONE;
        g_game_state.current_base_chips = 0;
        g_game_state.current_base_mult = 0;

        switch(g_automated_event->params[0])
        {
            case INPUT_FOCUSED_ZONE_CONSUMABLES:
            {
                for(int j = g_game_state.highlighted_item+1; j < g_game_state.consumables.item_count; j++)
                {
                    g_game_state.consumables.items[j - 1] = g_game_state.consumables.items[j];
                }
                g_game_state.consumables.item_count--;        

                g_game_state.highlighted_item--;
                if (g_game_state.highlighted_item < 0) g_game_state.highlighted_item = 0;

                game_set_initial_final_consumable_position();

                event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
                break;
            }
            case INPUT_FOCUSED_ZONE_SHOP_SINGLES:
            {
                g_game_state.shop.items[g_game_state.highlighted_item].available = false;
                g_game_state.shop.item_count--;
                break;
            }
            case INPUT_FOCUSED_ZONE_BOOSTER_ITEMS:
            {
                g_game_state.shop.booster_items[g_game_state.highlighted_item].available = false;            
                g_game_state.shop.booster_item_count--;
                break;
            }
            default:
            {
                break;
            }
        }        
    }

    AUTO_EVENT_STAGE_WAIT(10)

    AUTO_EVENT_STAGE()
    {
        if (g_automated_event->params[0] == INPUT_FOCUSED_ZONE_CONSUMABLES && g_game_state.consumables.item_count > 0)
        {
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_CONSUMABLES;
        }
        else if (g_automated_event->params[0] == INPUT_FOCUSED_ZONE_BOOSTER_ITEMS)
        {
            // Check if all items picked
            if (g_game_state.shop.booster_total_items - g_game_state.shop.booster_item_count == (g_game_state.shop.opened_booster.size == BOOSTER_PACK_SIZE_MEGA ? 2 : 1))
            {
                g_game_state.highlighted_item = 0;
                game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_MAIN);
            }
            else
            {
                g_game_state.highlighted_item = game_util_get_first_shop_booster_item_index();
                g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_BOOSTER_ITEMS;
            }
        }
        else
        {
            g_game_state.input_focused_zone = game_util_get_default_focus_zone();
        }
    }

    AUTO_EVENT_END()

    return true;
}

#define USE_TAROT_CREATE_CONSUMABLES 1

#define USE_TAROT_CREATE_CONSUMABLES_VALUE_LAST     1
#define USE_TAROT_CREATE_CONSUMABLES_VALUE_TAROTS   2
#define USE_TAROT_CREATE_CONSUMABLES_VALUE_PLANETS  3

bool automated_event_use_tarot()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        g_automated_event->params[0] = g_game_state.input_focused_zone;

        g_game_state.input_focused_zone = -1;

        switch(g_automated_event->params[0])
        {
            case INPUT_FOCUSED_ZONE_CONSUMABLES:
            {
                g_automated_event->tarot = &(g_game_state.consumables.items[g_game_state.highlighted_item].tarot);
                break;
            }
            case INPUT_FOCUSED_ZONE_SHOP_SINGLES:
            {
                g_automated_event->tarot = &(g_game_state.shop.items[g_game_state.highlighted_item].info.tarot);
                break;
            }
            case INPUT_FOCUSED_ZONE_BOOSTER_ITEMS:
            {
                g_automated_event->tarot = &(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.tarot);
                break;
            }
            default:
            {
                break;
            }
        }

        event_add_shake_item(&g_automated_event->tarot->draw, 30);
    }

    AUTO_EVENT_STAGE_WAIT(20)

    AUTO_EVENT_STAGE()
    {
        event_add_interpolate_value(&(g_automated_event->tarot->draw.white_factor), 0.0f, 1.0f, 20);

        AUTO_EVENT_SET_VAL(USE_TAROT_CREATE_CONSUMABLES, 0)

        // TODO: Add tarot effect
        switch (g_automated_event->tarot->type)
        {
            case TAROT_TYPE_FOOL:
            {
                AUTO_EVENT_SET_VAL(USE_TAROT_CREATE_CONSUMABLES, USE_TAROT_CREATE_CONSUMABLES_VALUE_LAST)
                break;
            }
            case TAROT_TYPE_MAGICIAN:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement = CARD_ENHANCEMENT_LUCKY;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_joker_types[JOKER_TYPE_LUCKY_CAT].enabled = true;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_MAGICIAN;
                break;
            }
            case TAROT_TYPE_PRIESTESS:
            {
                AUTO_EVENT_SET_VAL(USE_TAROT_CREATE_CONSUMABLES, USE_TAROT_CREATE_CONSUMABLES_VALUE_PLANETS)

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_PRIESTESS;
                break;
            }
            case TAROT_TYPE_EMPRESS:
            {                
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement = CARD_ENHANCEMENT_MULT;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_EMPRESS;
                break;
            }
            case TAROT_TYPE_EMPEROR:
            {
                AUTO_EVENT_SET_VAL(USE_TAROT_CREATE_CONSUMABLES, USE_TAROT_CREATE_CONSUMABLES_VALUE_TAROTS)

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_EMPEROR;
                break;
            }
            case TAROT_TYPE_HIEROPHANT:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement  = CARD_ENHANCEMENT_BONUS;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_HIEROPHANT;
                break;
            }
            case TAROT_TYPE_LOVERS:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement  = CARD_ENHANCEMENT_WILD;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_LOVERS;
                break;
            }
            case TAROT_TYPE_CHARIOT:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement  = CARD_ENHANCEMENT_STEEL;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_CHARIOT;
                break;
            }
            case TAROT_TYPE_JUSTICE:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement  = CARD_ENHANCEMENT_GLASS;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_JUSTICE;
                break;
            }
            case TAROT_TYPE_HERMIT:
            {
                if (g_game_state.wealth > 0)
                {
                    g_game_state.wealth += g_game_state.wealth > 20 ? 20 : g_game_state.wealth;
                }

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_HERMIT;
                break;
            }
            case TAROT_TYPE_WHEEL_FORTUNE:
            {
                if (game_util_chance_occurs(1, 4))
                {
                    int possible_jokers[100];
                    int possible_jokers_count = 0;

                    for (int i = 0; i < g_game_state.jokers.joker_count; i++)
                    {
                        if (g_game_state.jokers.jokers[i].edition == CARD_EDITION_BASE)
                        {
                            possible_jokers[possible_jokers_count] = i;
                            possible_jokers_count++;
                        }
                    }

                    if (possible_jokers_count > 0)
                    {
                        int joker_index = possible_jokers[game_util_rand(0, possible_jokers_count - 1)];
                        struct Joker *joker = &(g_game_state.jokers.jokers[joker_index]);
                        int random_number = game_util_rand(0, 100);
                        if (random_number < 50)
                        {
                            joker->edition = CARD_EDITION_FOIL;
                        }
                        else if (random_number < 50+35)
                        {
                            joker->edition = CARD_EDITION_HOLOGRAPHIC;
                        }
                        else
                        {
                            joker->edition = CARD_EDITION_POLYCHROME;
                        }
                        event_add_shake_item(&(joker->draw), 20);
                    }
                }

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_WHEEL_FORTUNE;
                break;
            }
            case TAROT_TYPE_STRENGTH:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->rank++;
                        if (g_game_state.hand.cards[i]->rank > CARD_RANK_ACE) g_game_state.hand.cards[i]->rank = 0;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_STRENGTH;
                break;
            }
            case TAROT_TYPE_HANGED_MAN:
            {
                int count = 0;
                int card_indexes[2];
                struct Card *cards[2];
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        card_indexes[count] = i;
                        cards[count] = g_game_state.hand.cards[i];
                        count++;
                    }
                }
                g_game_state.selected_cards_count = 0;

                for(int i = 0; i < count; i++)
                {
                    game_discard_card(card_indexes[i] - i);
                    game_remove_card_from_full_deck(cards[i]);
                }

                event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, -1, 0, 0, 20);

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_HANGED_MAN;
                break;
            }
            case TAROT_TYPE_DEATH:
            {
                struct Card *card_to_be_copied = NULL;
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        if (card_to_be_copied == NULL)
                        {
                            card_to_be_copied = g_game_state.hand.cards[i];
                        }
                        else
                        {
                            struct DrawObject draw = card_to_be_copied->draw;
                            *(card_to_be_copied) = *(g_game_state.hand.cards[i]);
                            card_to_be_copied->draw = draw;
                            card_to_be_copied->selected = false;
                        }
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_DEATH;
                break;
            }
            case TAROT_TYPE_TEMPERANCE:
            {
                int joker_value = 0;
                for (int i = 0; i < g_game_state.jokers.joker_count; i++)
                {
                    joker_value += game_util_get_joker_sell_price(&(g_game_state.jokers.jokers[i]));
                }
                g_game_state.wealth += MIN(joker_value, 50);

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_TEMPERANCE;
                break;
            }
            case TAROT_TYPE_DEVIL:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement  = CARD_ENHANCEMENT_GOLD;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);

                        g_joker_types[JOKER_TYPE_GOLDEN_TICKET].enabled = true;
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_DEVIL;
                break;
            }
            case TAROT_TYPE_TOWER:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->enhancement  = CARD_ENHANCEMENT_STONE;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_TOWER;
                break;
            }
            case TAROT_TYPE_STAR:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->suit  = CARD_SUIT_DIAMONDS;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_STAR;
                break;
            }
            case TAROT_TYPE_MOON:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->suit  = CARD_SUIT_CLUBS;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_SUN;
                break;
            }
            case TAROT_TYPE_SUN:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->suit  = CARD_SUIT_HEARTS;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_SUN;
                break;
            }
            case TAROT_TYPE_JUDGEMENT:
            {
                int joker_types[100];
                for(int i = 0; i < g_game_state.jokers.joker_count; i++) { joker_types[i] = g_game_state.jokers.jokers[i].type; }
                int new_joker_type = game_util_get_new_joker_type(joker_types, g_game_state.jokers.joker_count, false);
                g_game_state.jokers.jokers[g_game_state.jokers.joker_count].type = new_joker_type;
                game_init_joker(&(g_game_state.jokers.jokers[g_game_state.jokers.joker_count]));
                game_set_object_off_screen(&(g_game_state.jokers.jokers[g_game_state.jokers.joker_count].draw));
                g_game_state.jokers.joker_count++;
                event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, 0, 0, 0, 10);

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_JUDGEMENT;
                break;
            }
            case TAROT_TYPE_WORLD:
            {
                for (int i = 0; i < g_game_state.hand.card_count; i++)
                {
                    if (g_game_state.hand.cards[i]->selected)
                    {
                        g_game_state.hand.cards[i]->suit  = CARD_SUIT_SPADES;
                        g_game_state.hand.cards[i]->selected = false;
                        event_add_shake_item(&(g_game_state.hand.cards[i]->draw), 20);
                    }
                }
                g_game_state.selected_cards_count = 0;

                g_game_state.last_used_consumable_item_type = ITEM_TYPE_TAROT;
                g_game_state.last_used_consumable_tarot_planet_type = TAROT_TYPE_WORLD;
                break;
            }
        }
    }

    AUTO_EVENT_STAGE_WAIT(21)

    AUTO_EVENT_STAGE()
    {
        switch(g_automated_event->params[0])
        {
            case INPUT_FOCUSED_ZONE_CONSUMABLES:
            {
                for(int j = g_game_state.highlighted_item+1; j < g_game_state.consumables.item_count; j++)
                {
                    g_game_state.consumables.items[j - 1] = g_game_state.consumables.items[j];
                }
                g_game_state.consumables.item_count--;        

                g_game_state.highlighted_item--;
                if (g_game_state.highlighted_item < 0) g_game_state.highlighted_item = 0;

                game_set_initial_final_consumable_position();

                event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
                break;
            }
            case INPUT_FOCUSED_ZONE_SHOP_SINGLES:
            {
                g_game_state.shop.items[g_game_state.highlighted_item].available = false;
                g_game_state.shop.item_count--;
                break;
            }
            case INPUT_FOCUSED_ZONE_BOOSTER_ITEMS:
            {
                g_game_state.shop.booster_items[g_game_state.highlighted_item].available = false;
                g_game_state.shop.booster_item_count--;
                break;
            }
            default:
            {        
                break;
            }
        }
        game_set_card_hand_positions();
    }

    AUTO_EVENT_STAGE_WAIT(10)

    AUTO_EVENT_STAGE()
    {
        if (AUTO_EVENT_VAL(USE_TAROT_CREATE_CONSUMABLES) == 0)
        {
            AUTO_EVENT_GO_TO_STAGE(USE_TAROT_END)
        }
    }

    // Create consumable
    AUTO_EVENT_STAGE()
    {
        switch(AUTO_EVENT_VAL(USE_TAROT_CREATE_CONSUMABLES))
        {
            case USE_TAROT_CREATE_CONSUMABLES_VALUE_LAST:
            {
                if (game_util_has_room_in_consumables())
                {
                    if (g_game_state.last_used_consumable_item_type == ITEM_TYPE_TAROT)
                    {
                        g_game_state.consumables.items[g_game_state.consumables.item_count].type = ITEM_TYPE_TAROT;
                        game_init_tarot(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot), g_game_state.last_used_consumable_tarot_planet_type, CARD_EDITION_BASE);
                        game_set_object_off_screen(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot.draw));
                        g_game_state.consumables.item_count++;                
                    }
                    else
                    {
                        g_game_state.consumables.items[g_game_state.consumables.item_count].type = ITEM_TYPE_PLANET;
                        game_init_planet(&(g_game_state.consumables.items[g_game_state.consumables.item_count].planet), g_game_state.last_used_consumable_tarot_planet_type, CARD_EDITION_BASE);
                        game_set_object_off_screen(&(g_game_state.consumables.items[g_game_state.consumables.item_count].planet.draw));
                        g_game_state.consumables.item_count++;
                    }
                    event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
                }
                break;
            }
            case USE_TAROT_CREATE_CONSUMABLES_VALUE_TAROTS:
            {
                if (game_util_has_room_in_consumables())
                {
                    int empty_slots = g_game_state.consumable_slots - g_game_state.consumables.item_count;
                    for(int i = 0; i < MIN(2, empty_slots); i++)
                    {
                        int excluded_tarots[100];
                        int excluded_tarots_count = 0;
                        for (int j = 0; j < g_game_state.consumables.item_count; j++) { if (g_game_state.consumables.items[j].type == ITEM_TYPE_TAROT) {excluded_tarots[excluded_tarots_count++] = g_game_state.consumables.items[j].tarot.type;} }
                        int tarot_type = game_util_get_new_tarot_type(excluded_tarots, excluded_tarots_count);
                        g_game_state.consumables.items[g_game_state.consumables.item_count].type = ITEM_TYPE_TAROT;
                        game_init_tarot(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot), tarot_type, CARD_EDITION_BASE);
                        game_set_object_off_screen(&(g_game_state.consumables.items[g_game_state.consumables.item_count].tarot.draw));
                        g_game_state.consumables.item_count++;
                    }
                    event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
                }
                break;
            }
            case USE_TAROT_CREATE_CONSUMABLES_VALUE_PLANETS:
            {
                if (game_util_has_room_in_consumables())
                {
                    int empty_slots = g_game_state.consumable_slots - g_game_state.consumables.item_count;
                    for(int i = 0; i < MIN(2, empty_slots); i++)
                    {
                        int excluded_planets[100];
                        int excluded_planet_count = 0;
                        for (int j = 0; j < g_game_state.consumables.item_count; j++) { if (g_game_state.consumables.items[j].type == ITEM_TYPE_PLANET) {excluded_planets[excluded_planet_count++] = g_game_state.consumables.items[j].planet.type;} }
                        int planet_type = game_util_get_new_planet_type(excluded_planets, excluded_planet_count);
                        g_game_state.consumables.items[g_game_state.consumables.item_count].type = ITEM_TYPE_PLANET;
                        game_init_planet(&(g_game_state.consumables.items[g_game_state.consumables.item_count].planet), planet_type, CARD_EDITION_BASE);
                        game_set_object_off_screen(&(g_game_state.consumables.items[g_game_state.consumables.item_count].planet.draw));
                        g_game_state.consumables.item_count++;
                    }
                    event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_CONSUMABLES, 0, 0, 0, 10);
                }
                break;
            }
        }
    }

    AUTO_EVENT_STAGE_WAIT(10)

    AUTO_EVENT_NAMED_STAGE(USE_TAROT_END)
    {
        if (g_automated_event->params[0] == INPUT_FOCUSED_ZONE_CONSUMABLES && g_game_state.consumables.item_count > 0)
        {
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_CONSUMABLES;
        }
        else if (g_automated_event->params[0] == INPUT_FOCUSED_ZONE_BOOSTER_ITEMS)
        {
            // Check if all items picked
            if (g_game_state.shop.booster_total_items - g_game_state.shop.booster_item_count == (g_game_state.shop.opened_booster.size == BOOSTER_PACK_SIZE_MEGA ? 2 : 1))
            {
                g_game_state.highlighted_item = 0;
                game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_MAIN);
            }
            else
            {
                g_game_state.highlighted_item = game_util_get_first_shop_booster_item_index();
                g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_BOOSTER_ITEMS;
            }
        }
        else
        {
            g_game_state.input_focused_zone = game_util_get_default_focus_zone();
        }
    }

    AUTO_EVENT_END()

    return true;
}

#define CEREMONIAL_DAGGER_INDEX 0

bool automated_event_ceremonial_dagger()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        if (AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX) == g_game_state.jokers.joker_count - 1)
        {
            AUTO_EVENT_GO_TO_END()
        }
    }

    AUTO_EVENT_STAGE()
    {
        event_add_shake_joker(AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX), 60);
        event_add(EVENT_MOVE_CARD, 0, EVENT_CARD_LOCATION_JOKER, AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX), g_game_state.jokers.jokers[AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX) + 1].draw.x - (CARD_WIDTH / 2), g_game_state.jokers.jokers[AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX) + 1].draw.y, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)

    AUTO_EVENT_STAGE()
    {
        int mult = game_util_get_joker_sell_price(&(g_game_state.jokers.jokers[AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX) + 1])) * 2;
        g_game_state.jokers.jokers[AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX)].param1 += mult;
        event_add_show_number((double)mult, AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX), EVENT_CARD_LOCATION_JOKER, SCORE_NUMBER_ADD_MULT, 20);
    }

    AUTO_EVENT_STAGE_WAIT(20)

    AUTO_EVENT_STAGE()
    {
        game_remove_joker(AUTO_EVENT_VAL(CEREMONIAL_DAGGER_INDEX) + 1);

        game_set_initial_final_joker_position();
        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, 0, 0, 0, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)

    AUTO_EVENT_END()

    return true;
}

#define SELECT_BLIND_CURRENT_JOKER  0

// Select blind events

bool automated_event_select_blind()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        AUTO_EVENT_SET_VAL(SELECT_BLIND_CURRENT_JOKER, 0)

        if (g_game_state.jokers.joker_count == 0)
        {
            AUTO_EVENT_GO_TO_STAGE(AFTER_JOKERS)
        }
    }

    AUTO_EVENT_NAMED_STAGE(JOKER)
    {
        switch(game_util_get_joker_type(AUTO_EVENT_VAL(SELECT_BLIND_CURRENT_JOKER)))
        {
            case JOKER_TYPE_MARBLE_JOKER:
                event_add_shake_joker(AUTO_EVENT_VAL(SELECT_BLIND_CURRENT_JOKER), 30);
                struct Card *card = game_set_new_card();
                card->edition = CARD_EDITION_BASE;
                card->enhancement = CARD_ENHANCEMENT_STONE;
                card->extra_chips = 0;
                card->rank = rand() % 13;
                card->suit = rand() % 4;
                game_add_card_to_full_deck(card);
                AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, 30)
                break;
            case JOKER_TYPE_CEREMONIAL_DAGGER:
                AUTO_EVENT_CALL(AUTOMATED_EVENT_CEREMONIAL_DAGGER, 1, AUTO_EVENT_VAL(SELECT_BLIND_CURRENT_JOKER));
                break;
            case JOKER_TYPE_BURGLAR:
                event_add_shake_joker(AUTO_EVENT_VAL(SELECT_BLIND_CURRENT_JOKER), 30);
                g_game_state.current_hands += 3;
                g_game_state.current_discards = 0;
                AUTO_EVENT_CALL(AUTOMATED_EVENT_WAIT, 1, 30)
                break;
            case JOKER_TYPE_RIFF_RAFF:
            {
                int joker_types[100];
                for(int i = 0; i < g_game_state.jokers.joker_count; i++) { joker_types[i] = g_game_state.jokers.jokers[i].type; }
                for(int i = 0; i < 2; i++)
                {
                    if (!game_util_has_room_in_jokers()) break;
                    int new_joker_type = game_util_get_new_joker_type(joker_types, g_game_state.jokers.joker_count, true);
                    g_game_state.jokers.jokers[g_game_state.jokers.joker_count].type = new_joker_type;
                    game_init_joker(&(g_game_state.jokers.jokers[g_game_state.jokers.joker_count]));
                    game_set_object_off_screen(&(g_game_state.jokers.jokers[g_game_state.jokers.joker_count].draw));
                    joker_types[g_game_state.jokers.joker_count] = new_joker_type;
                    g_game_state.jokers.joker_count++;
                }
                event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_JOKER, 0, 0, 0, 10);
                break;
            }
            default:
                break;
        }
    }

    AUTO_EVENT_STAGE()
    {
        AUTO_EVENT_INC(SELECT_BLIND_CURRENT_JOKER)
        if (AUTO_EVENT_VAL(SELECT_BLIND_CURRENT_JOKER) < g_game_state.jokers.joker_count)
        {
            AUTO_EVENT_GO_TO_STAGE(JOKER)
        }
    }

    AUTO_EVENT_NAMED_STAGE(AFTER_JOKERS)
    {
        game_go_to_stage(GAME_STAGE_INGAME, GAME_SUBSTAGE_INGAME_PICK_HAND);
    }

    AUTO_EVENT_END()

    return true;
}

bool automated_event_open_booster()
{
    AUTO_EVENT_START()

    // Rip the booster

    AUTO_EVENT_STAGE()
    {
        g_game_state.wealth -= game_util_get_booster_price(&(g_game_state.shop.boosters[g_game_state.highlighted_item].booster));
        
        game_util_copy_deck(&g_game_state.full_deck, &g_game_state.current_deck);
        game_util_shuffle_deck(&g_game_state.current_deck);
        g_game_state.hand.card_count = 0;
        g_game_state.played_hand.card_count = 0;
    }

    AUTO_EVENT_STAGE_WAIT(30)

    // Go to the substage

    AUTO_EVENT_STAGE()
    {        
        g_game_state.shop.opened_booster.type = g_game_state.shop.boosters[g_game_state.highlighted_item].booster.type;
        g_game_state.shop.opened_booster.size = g_game_state.shop.boosters[g_game_state.highlighted_item].booster.size;
        
        g_game_state.shop.boosters[g_game_state.highlighted_item].available = false;
        g_game_state.shop.booster_count--;
        
        if (g_game_state.shop.opened_booster.type == BOOSTER_PACK_TYPE_ARCANA ||
            g_game_state.shop.opened_booster.type == BOOSTER_PACK_TYPE_SPECTRAL)
        {
            game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_BOOSTER_HAND);
            g_automated_event->params[0] = game_util_get_hand_size();
            game_util_copy_deck(&g_game_state.full_deck, &g_game_state.current_deck);
            game_util_shuffle_deck(&g_game_state.current_deck);
            game_set_hand_y(DRAW_HAND_Y_IN_BOOSTER);
        }
        else
        {
            game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_BOOSTER_NO_HAND);
            AUTO_EVENT_GO_TO_STAGE(DRAW_ITEM)
        }
    }

    // Draw hand

    AUTO_EVENT_NAMED_STAGE(DRAW_CARD)
    {
        game_draw_card_into_hand();
        game_sort_hand();
        g_automated_event->params[0]--;

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_HAND, 0, 0, 0, DRAW_TIMESPAN);
    }

    AUTO_EVENT_STAGE_WAIT(DRAW_TIMESPAN)

    AUTO_EVENT_STAGE()
    {
        if (g_automated_event->params[0] > 0)
        {
            AUTO_EVENT_GO_TO_STAGE(DRAW_CARD)
        }
    }

    // Draw items

    AUTO_EVENT_NAMED_STAGE(DRAW_ITEM)
    {

    }

    AUTO_EVENT_END()

    return true;
}

bool automated_event_add_booster_card()
{
    AUTO_EVENT_START()

    AUTO_EVENT_STAGE()
    {
        g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.draw.final_x = DRAW_DECK_X;
        g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.draw.final_y = DRAW_DECK_Y;

        event_add(EVENT_ARRANGE_CARDS, 0, EVENT_CARD_LOCATION_BOOSTER_ITEMS, 0, 0, 0, 10);
    }

    AUTO_EVENT_STAGE_WAIT(10)

    AUTO_EVENT_STAGE()
    {
        struct Card *card = game_set_new_card();
        card->edition = g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.edition;
        card->enhancement = g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.enhancement;
        card->extra_chips = 0;
        card->rank = g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.rank;
        card->suit = g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.suit;
        game_add_card_to_full_deck(card);

        if (card->enhancement == CARD_ENHANCEMENT_GOLD)
        {
            g_joker_types[JOKER_TYPE_GOLDEN_TICKET].enabled = true;
        }
        else if (card->enhancement == CARD_ENHANCEMENT_LUCKY)
        {
            g_joker_types[JOKER_TYPE_LUCKY_CAT].enabled = true;
        }

        g_game_state.stats.cards_purchased++;

        g_game_state.shop.booster_items[g_game_state.highlighted_item].available = false;
        g_game_state.shop.booster_item_count--;
        g_game_state.highlighted_item = game_util_get_first_shop_booster_item_index();

        // Check if all items picked
        if (g_game_state.shop.booster_total_items - g_game_state.shop.booster_item_count == (g_game_state.shop.opened_booster.size == BOOSTER_PACK_SIZE_MEGA ? 2 : 1))
        {
            g_game_state.highlighted_item = 0;
            game_go_to_stage(GAME_STAGE_SHOP, GAME_SUBSTAGE_SHOP_MAIN);
        }
    }

    AUTO_EVENT_END()

    return true;
}

// ############################
//      Generic functions
// ############################

void automated_event_set(int event_id, int num_params, ...)
{
    g_automated_event_stack_pos = 0;
    g_automated_event = &(g_automated_event_stack[0]);
    g_automated_event->event_id = event_id;
    g_automated_event->stage = 0;
    g_automated_event->frame_count = 0;

    va_list arguments;

    va_start(arguments, num_params);
    for(int i = 0; i < num_params; i++)
    {
        g_automated_event->params[i] = va_arg(arguments, int);
    }
    va_end(arguments);

    DEBUG_PRINTF("AUTO EVENT set %d\n", event_id);
}

void automated_event_push(int event_id, int num_params, ...)
{
    g_automated_event_stack_pos++;
    // g_automated_event = &(g_automated_event_stack[g_automated_event_stack_pos]);
    g_automated_event_stack[g_automated_event_stack_pos].event_id = event_id;
    g_automated_event_stack[g_automated_event_stack_pos].stage = 0;
    g_automated_event_stack[g_automated_event_stack_pos].frame_count = 0;

    va_list arguments;

    va_start(arguments, num_params);
    for(int i = 0; i < num_params; i++)
    {
        g_automated_event_stack[g_automated_event_stack_pos].params[i] = va_arg(arguments, int);
    }
    va_end(arguments);

    DEBUG_PRINTF("AUTO EVENT push %d\n", event_id);
}

void automated_event_pop()
{
    DEBUG_PRINTF("AUTO EVENT pop %d\n", g_automated_event_stack[g_automated_event_stack_pos].event_id);

    if (g_automated_event_stack_pos > -1)
    {
        g_automated_event_stack_pos--;
    }

    if (g_automated_event_stack_pos >= 0)
    {
        // g_automated_event = &(g_automated_event_stack[g_automated_event_stack_pos]);

        DEBUG_PRINTF("AUTO EVENT back to %d\n", g_automated_event_stack[g_automated_event_stack_pos].event_id);
    }
    else
    {
        // g_automated_event = NULL;
        DEBUG_PRINTF("AUTO EVENT no event\n");
    }
}

bool automated_event_run()
{
    if (g_automated_event_stack_pos < 0) return false;

    g_automated_event = &(g_automated_event_stack[g_automated_event_stack_pos]);

    bool in_event = (g_automated_event->event_id != AUTOMATED_EVENT_NONE);
    bool block_input = false;    

    if (g_automated_event->event_id != AUTOMATED_EVENT_NONE)
    {
        block_input = g_automated_event_types[g_automated_event->event_id].func();
    }

    if (in_event)
    {
        g_automated_event->frame_count++;
    }

    return block_input;
}
