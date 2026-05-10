#include "global.h"

int inner_game_util_joker_type(int index, int starting_index, bool first)
{
    if (index == starting_index && !first)
    {
        return g_game_state.jokers.jokers[index].type;
    }

    if (index >= g_game_state.jokers.joker_count || index < 0)
    {
        return -1;
    }

    if (g_game_state.jokers.jokers[index].type == JOKER_TYPE_BLUEPRINT && index < g_game_state.jokers.joker_count - 1)
    {
        return inner_game_util_joker_type(index + 1, starting_index, false);
    }

    if (g_game_state.jokers.jokers[index].type == JOKER_TYPE_BRAINSTORM && index > 0 && g_game_state.jokers.joker_count > 1)
    {
        return inner_game_util_joker_type(0, starting_index, false);
    }

    return g_game_state.jokers.jokers[index].type;
}

int game_util_get_joker_type(int index)
{
    return inner_game_util_joker_type(index, index, true);
}

int game_util_get_real_joker_type(int index)
{
    return g_game_state.jokers.jokers[index].type;
}

void game_util_set_joker_param(int index, int value)
{
    g_game_state.jokers.jokers[index].param1 = value;
}

int inner_game_util_get_joker_param(int index, int starting_index, bool first)
{
    if (index == starting_index && !first)
    {
        return 0;
    }

    if (index >= g_game_state.jokers.joker_count || index < 0)
    {
        return 0;
    }

    if (g_game_state.jokers.jokers[index].type == JOKER_TYPE_BLUEPRINT && index < g_game_state.jokers.joker_count - 1)
    {
        return inner_game_util_get_joker_param(index + 1, starting_index, false);
    }

    if (g_game_state.jokers.jokers[index].type == JOKER_TYPE_BRAINSTORM && index > 0 && g_game_state.jokers.joker_count > 1)
    {
        return inner_game_util_get_joker_param(0, starting_index, false);
    }

    return g_game_state.jokers.jokers[index].param1;
}

int game_util_get_joker_param(int index)
{
    return inner_game_util_get_joker_param(index, index, true);
}

bool game_util_has_joker_type(int joker_type)
{
    for (int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        if (game_util_get_joker_type(i) == joker_type) return true;
    }

    return false;
}

int game_util_get_joker_type_count(int joker_type)
{
    int count = 0;

    for (int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        if (game_util_get_joker_type(i) == joker_type) count++;
    }

    return count;
}

int game_util_get_real_joker_type_count(int joker_type)
{
    int count = 0;

    for (int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        if (game_util_get_real_joker_type(i) == joker_type) count++;
    }

    return count;
}

bool game_util_is_card_suit(struct Card *card, int suit)
{
    if (card->enhancement == CARD_ENHANCEMENT_STONE) return false;
    if (card->enhancement == CARD_ENHANCEMENT_WILD) return true;
    if (game_util_has_joker_type(JOKER_TYPE_SMEARED_JOKER))
    {
        return (card->suit & 1) == (suit & 1);
    }
    return card->suit == suit;
}

int game_util_sort_by_rank_func(const void *a, const void *b)
{
    struct Card *card_a = *((struct Card **)a);
    struct Card *card_b = *((struct Card **)b);

    if (card_a == NULL) return 1;
    if (card_b == NULL) return -1;

    int rank_a = card_a->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_a->rank;
    int rank_b = card_b->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_b->rank;

    return rank_a - rank_b;
}

bool game_util_are_2_cards_same_rank(struct Card *card1, struct Card *card2)
{
    if (card1->enhancement == CARD_ENHANCEMENT_STONE || card2->enhancement == CARD_ENHANCEMENT_STONE) return false;

    return card1->rank == card2->rank;
}

bool game_util_are_cards_rank_consecutive(struct Card *card1, struct Card *card2)
{
    if (card1->enhancement == CARD_ENHANCEMENT_STONE || card2->enhancement == CARD_ENHANCEMENT_STONE) return false;

    if (card1->rank == card2->rank - 1) return true;
    if (card1->rank - 1 == card2->rank) return true;

    return false;
}

bool game_util_are_2_cards_same_suit(struct Card *card1, struct Card *card2)
{
    if (card1->enhancement == CARD_ENHANCEMENT_STONE || card2->enhancement == CARD_ENHANCEMENT_STONE) return false;
    if (card1->enhancement == CARD_ENHANCEMENT_WILD || card2->enhancement == CARD_ENHANCEMENT_WILD) return true;

    if (game_util_has_joker_type(JOKER_TYPE_SMEARED_JOKER))
    {
        return (card1->suit & 1) == (card2->suit & 1);
    }

    return card1->suit == card2->suit;
}

bool game_util_are_cards_same_suit(struct Card *cards[], int card_count)
{
    for(int i = 1; i < card_count; i++)
    {
        for(int j = 0; j < i; j++)
        {
            if (!game_util_are_2_cards_same_suit(cards[i], cards[j])) return false;
        }
    }

    return true;
}

bool game_util_played_hand_contains_pair()
{
    struct Card *selected_cards[5];

    if (g_game_state.played_hand.card_count < 2)
    {
        return false;
    }

    for(int i = 0; i < 5; i++)
    {
        if (i < g_game_state.played_hand.card_count)
        {
            selected_cards[i] = g_game_state.played_hand.cards[i];
        }
        else
        {
            selected_cards[i] = NULL;
        }
    }

    qsort((void*)selected_cards, 5, sizeof(struct Card *), game_util_sort_by_rank_func);

    if (g_game_state.played_hand.card_count >= 2)
    {
        for(int i = 1; i < g_game_state.played_hand.card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i], selected_cards[i-1]))
            {
                return true;
            }
        }
    }

    return false;
}

bool game_util_played_hand_contains_three_of_a_kind()
{
    struct Card *selected_cards[5];

    if (g_game_state.played_hand.card_count < 2)
    {
        return false;
    }

    for(int i = 0; i < 5; i++)
    {
        if (i < g_game_state.played_hand.card_count)
        {
            selected_cards[i] = g_game_state.played_hand.cards[i];
        }
        else
        {
            selected_cards[i] = NULL;
        }
    }

    qsort((void*)selected_cards, 5, sizeof(struct Card *), game_util_sort_by_rank_func);

    if (g_game_state.played_hand.card_count >= 3)
    {
        int same_rank_count = 0;
        for(int i = 1; i < g_game_state.played_hand.card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i], selected_cards[i-1]))
            {
                if (same_rank_count == 0) same_rank_count = 2; else same_rank_count++;

                if (same_rank_count == 3)
                {
                    return true;
                }
            }
            else
            {
                same_rank_count = 0;
            }
        }
    }

    return false;
}

bool game_util_played_hand_contains_four_of_a_kind()
{
    struct Card *selected_cards[5];

    if (g_game_state.played_hand.card_count < 2)
    {
        return false;
    }

    for(int i = 0; i < 5; i++)
    {
        if (i < g_game_state.played_hand.card_count)
        {
            selected_cards[i] = g_game_state.played_hand.cards[i];
        }
        else
        {
            selected_cards[i] = NULL;
        }
    }

    qsort((void*)selected_cards, 5, sizeof(struct Card *), game_util_sort_by_rank_func);

    if (g_game_state.played_hand.card_count >= 3)
    {
        int same_rank_count = 0;
        for(int i = 1; i < g_game_state.played_hand.card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i], selected_cards[i-1]))
            {
                if (same_rank_count == 0) same_rank_count = 2; else same_rank_count++;

                if (same_rank_count == 4)
                {
                    return true;
                }
            }
            else
            {
                same_rank_count = 0;
            }
        }
    }

    return false;
}

bool game_util_played_hand_contains_two_pair()
{
    struct Card *selected_cards[5];

    if (g_game_state.played_hand.card_count < 2)
    {
        return false;
    }

    for(int i = 0; i < 5; i++)
    {
        if (i < g_game_state.played_hand.card_count)
        {
            selected_cards[i] = g_game_state.played_hand.cards[i];
        }
        else
        {
            selected_cards[i] = NULL;
        }
    }

    qsort((void*)selected_cards, 5, sizeof(struct Card *), game_util_sort_by_rank_func);

    if (g_game_state.played_hand.card_count >= 4)
    {
        int same_rank_count = 0;
        int pair_count = 0;
        for(int i = 1; i < g_game_state.played_hand.card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i], selected_cards[i-1]))
            {
                if (same_rank_count == 0) same_rank_count = 2; else same_rank_count++;

                if (same_rank_count >= 2 && pair_count == 1)
                {
                    return true;
                }
            }
            else
            {
                if (same_rank_count >= 2)
                {
                    pair_count++;
                }
                same_rank_count = 0;
            }
        }
    }

    return false;
}

bool game_util_played_hand_contains_straight()
{
    struct Card *selected_cards[5];

    if (g_game_state.played_hand.card_count < 2)
    {
        return false;
    }

    for(int i = 0; i < 5; i++)
    {
        if (i < g_game_state.played_hand.card_count)
        {
            selected_cards[i] = g_game_state.played_hand.cards[i];
        }
        else
        {
            selected_cards[i] = NULL;
        }
    }

    qsort((void*)selected_cards, 5, sizeof(struct Card *), game_util_sort_by_rank_func);

    if (g_game_state.played_hand.card_count == 5)
    {
        for(int i = 1; i < 5; i++)
        {
            if (!game_util_are_cards_rank_consecutive(selected_cards[i], selected_cards[i-1]))
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

bool game_util_played_hand_contains_flush()
{
    if (g_game_state.played_hand.card_count == 5)
    {
        for(int i = 1; i < g_game_state.played_hand.card_count; i++)
        {
            for(int j = 0; j < i; j++)
            {
                if (!game_util_are_2_cards_same_suit(g_game_state.played_hand.cards[i], g_game_state.played_hand.cards[j])) return false;
            }
        }

        return true;
    }

    return false;
}

bool game_util_played_hand_contains_scoring_face()
{
    for(int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        if (g_game_state.played_hand.cards[i]->selected && game_util_is_card_face(g_game_state.played_hand.cards[i]))
        {
            return true;
        }
    }

    return false;
}

char *game_util_get_blind_name(int blind)
{
    switch(blind)
    {
        case GAME_BLIND_SMALL:
            return "Small Blind";
        case GAME_BLIND_LARGE:
            return "Big Blind";
        case GAME_BLIND_BOSS:
            return "Boss Blind";
    }

    return "";
}

bool game_util_is_joker_slot_available()
{
    return (g_game_state.jokers.joker_count - g_game_state.jokers.negative_count) < g_game_state.joker_slots;
}

bool game_util_is_consumable_slot_available()
{
    return g_game_state.consumables.item_count < g_game_state.consumable_slots;
}

int game_util_get_joker_buy_price(struct Joker *joker)
{
    return g_joker_types[joker->type].value;
}

int game_util_get_joker_sell_price(struct Joker *joker)
{
    return g_joker_types[joker->type].value / 2;
}

int game_util_get_booster_price(struct BoosterPack *booster)
{
    switch(booster->size)
    {
        case BOOSTER_PACK_SIZE_NORMAL:
            return 4;
        case BOOSTER_PACK_SIZE_JUMBO:
            return 6;
        case BOOSTER_PACK_SIZE_MEGA:
            return 8;
    }

    return 0;
}

int game_util_get_first_shop_single_index()
{
    int index = -1;
    
    do
    {
        index++;
    } while (!g_game_state.shop.items[index].available && index < g_game_state.shop.total_items - 1);

    if (!g_game_state.shop.items[index].available) return -1;

    return index;
}

struct IntListElement {
    int value;
    struct IntListElement *prev, *next;
};

struct IntList {
    struct IntListElement elements[100];
    struct IntListElement *first;
    int count;
};

void add_int_list_sorted_if_unique(struct IntList *list, int value)
{
    if (list->count == 0)
    {
        list->elements[0].value = value;
        list->elements[0].prev = NULL;
        list->elements[0].next = NULL;
        list->first = list->elements;        
        list->count++;
        return;
    }

    struct IntListElement *current = list->first;    
    for (int i = 0; i < list->count; i++)
    {
        if (value == current->value) return;

        if (value < current->value)
        {
            list->elements[list->count].value = value;
            list->elements[list->count].prev = current->prev;
            list->elements[list->count].next = current;
            if (current->prev != NULL) current->prev->next = (list->elements + list->count);
            current->prev = (list->elements + list->count);
            if (current == list->first) list->first = (list->elements + list->count);
            list->count++;
            return;
        }

        if (i == list->count - 1)
        {
            list->elements[list->count].value = value;
            list->elements[list->count].prev = current;
            list->elements[list->count].next = NULL;
            current->next = (list->elements + list->count);
            list->count++;
            return;
        }
        
        current = current->next;
    }
}

int game_util_get_new_joker_type(int excluded_joker_types[], int excluded_joker_types_count, bool only_commons)
{
    // Odds:
    // Common Joker:    70%
    // Uncommon Joker:  25%
    // Rare Joker:       5%

    int possible_jokers[200];
    int possible_joker_count = 0;

    int rarity = JOKER_RARITY_COMMON;
    if (!only_commons)
    {
        rarity = JOKER_RARITY_RARE;
        int r = rand()%100;
        if (r < GAME_CHANCE_COMMON_JOKER) { rarity = JOKER_RARITY_COMMON; }
        else if (r < (GAME_CHANCE_COMMON_JOKER + GAME_CHANCE_UNCOMMON_JOKER)) { rarity = JOKER_RARITY_UNCOMMON; }
    }

    bool skip = false;
    for (int i = 0; i < JOKER_TYPE_COUNT; i++)
    {
        if (!g_joker_types[i].enabled) continue;

        if (g_joker_types[i].rarity == rarity)
        {
            skip = false;

            for (int j = 0; j < excluded_joker_types_count; j++)
            {
                if (excluded_joker_types[j] == i)
                {
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                possible_jokers[possible_joker_count] = i;
                possible_joker_count++;
            }
        }
    }

    // TODO : add repeated jokers if possible_joker_count == 0
    if (possible_joker_count == 0) return 0;

    return possible_jokers[rand()%possible_joker_count];
}

int game_util_get_new_tarot_type(int excluded_tarot_types[], int excluded_tarot_types_count)
{
    int possible_tarot[TAROT_TYPE_COUNT];
    int possible_tarot_count = 0;

    bool skip = false;
    for (int i = 0; i < TAROT_TYPE_COUNT; i++)
    {
        skip = false;

        for (int j = 0; j < excluded_tarot_types_count; j++)
        {
            if (excluded_tarot_types[j] == i)
            {
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            possible_tarot[possible_tarot_count++] = i;
        }
    }

    if (possible_tarot_count == 0)
    {
        return rand()%TAROT_TYPE_COUNT;
    }

    return possible_tarot[rand()%possible_tarot_count];
}

int game_util_get_new_planet_type(int excluded_planet_types[], int excluded_planet_types_count)
{
    int possible_planet[PLANET_TYPE_COUNT];
    int possible_planet_count = 0;

    bool skip = false;
    for (int i = 0; i < PLANET_TYPE_COUNT; i++)
    {
        skip = false;

        for (int j = 0; j < excluded_planet_types_count; j++)
        {
            if (excluded_planet_types[j] == i)
            {
                skip = true;
                break;
            }
        }

        if (!g_game_state.flush_five_enabled && i == PLANET_TYPE_ERIS) continue;
        if (!g_game_state.flush_house_enabled && i == PLANET_TYPE_CERES) continue;
        if (!g_game_state.five_of_a_kind_enabled && i == PLANET_TYPE_PLANET_X) continue;

        if (!skip)
        {
            possible_planet[possible_planet_count++] = i;
        }
    }

    if (possible_planet_count == 0)
    {
        return rand()%PLANET_TYPE_COUNT;
    }

    return possible_planet[rand()%possible_planet_count];
}

int game_util_get_planet_buy_price(struct Planet *planet)
{
    return 3;
}

int game_util_get_planet_sell_price(struct Planet *planet)
{
    return 1;
}

int game_util_get_tarot_buy_price(struct Tarot *tarot)
{
    return 3;
}

int game_util_get_tarot_sell_price(struct Tarot *tarot)
{
    return 1;
}

int game_util_get_reroll_cost()
{
    if (game_util_get_joker_type_count(JOKER_TYPE_CHAOS_THE_CLOWN) >= (g_game_state.shop.rerolls + 1)) return 0;
    return g_game_state.shop.rerolls + g_game_state.shop.reroll_cost;
}

bool game_util_can_buy(float price)
{
    if (game_util_has_joker_type(JOKER_TYPE_CREDIT_CARD))
    {
        return g_game_state.wealth - price >= -20;
    }

    return g_game_state.wealth >= price;
}

int game_util_get_default_focus_zone()
{
    switch (g_game_state.stage)
    {
        case GAME_STAGE_BLINDS:
            return INPUT_FOCUSED_ZONE_BLIND;
        case GAME_STAGE_INGAME:
        {
            switch (g_game_state.sub_stage)
            {
                case GAME_SUBSTAGE_INGAME_WON:
                    return INPUT_FOCUSED_ZONE_CASH_OUT_PANEL;
                case GAME_SUBSTAGE_INGAME_PICK_HAND:
                    return INPUT_FOCUSED_ZONE_HAND;
            }
            break;
        }
        case GAME_STAGE_SHOP:
        {
            switch (g_game_state.sub_stage)
            {
                case GAME_SUBSTAGE_SHOP_BOOSTER_HAND:
                case GAME_SUBSTAGE_SHOP_BOOSTER_NO_HAND:
                    return INPUT_FOCUSED_ZONE_BOOSTER_ITEMS;
                default:
                    return INPUT_FOCUSED_ZONE_SHOP_NEXT_ROUND;
            }
        }
    }

    return -1;
}

void game_util_copy_deck(struct CardReferences *src, struct CardReferences *dst)
{
    for(uint16_t i = 0; i < src->card_count; i++)
    {
        dst->cards[i] = src->cards[i];
        dst->cards[i]->selected = false;
        game_init_draw_object(&(dst->cards[i]->draw));
    }
    dst->card_count = src->card_count;
}

void game_util_shuffle_deck(struct CardReferences *deck)
{
    for(int i = deck->card_count - 1; i > 0; i--)
    {
        int from = i;
        int to = game_util_rand(0, i);

        if (from != to)
        {
            struct Card *temp;
            temp = deck->cards[to];
            deck->cards[to] = deck->cards[from];
            deck->cards[from] = temp;
        }
    }
}

int game_util_rand(int min, int max)
{
    return rand() % ((max + 1) - min) + min;
}

bool game_util_chance_occurs(int num, int den)
{
    int oops_count = game_util_get_joker_type_count(JOKER_TYPE_OOPS_ALL_6S);
    for(int i = 0; i < oops_count; i++)
    {
        num *= 2;
    }

    if (num > den)
    {
        return true;
    }

    return game_util_rand(0, den - 1) < num;
}

int game_util_get_lowest_rank_held_in_hand()
{
    int rank = 100;
    if (g_game_state.hand.card_count > 0)
    {
        for (int i = 0; i < g_game_state.hand.card_count; i++)
        {
            if (g_game_state.hand.cards[i]->rank < rank)
            {
                rank = g_game_state.hand.cards[i]->rank;
            }
        }

        return rank;
    }

    return -1;
}

bool game_util_is_card_rank(struct Card *card, int rank)
{
    return card->enhancement != CARD_ENHANCEMENT_STONE && card->rank == rank;
}

bool game_util_has_scoring_cards_rank(int rank)
{
    for (int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        if (g_game_state.played_hand.cards[i]->selected && game_util_is_card_rank(g_game_state.played_hand.cards[i], rank))
        {
            return true;
        }
    }

    return false;
}

bool game_util_is_card_face(struct Card *card)
{    
    if (game_util_has_joker_type(JOKER_TYPE_PAREIDOLIA)) return true;
    if (card->enhancement == CARD_ENHANCEMENT_STONE) return false;
    return game_util_is_card_rank(card, CARD_RANK_JACK) || game_util_is_card_rank(card, CARD_RANK_QUEEN) || game_util_is_card_rank(card, CARD_RANK_KING);
}

float game_util_get_item_position(int i, int item_count, float start_x, float width, float item_width)
{
    if (item_count * item_width > width)
    {
        return start_x + (i * (width - item_width) / (item_count - 1));
    }
    return start_x - (item_width / 2.0f) + (i + 1) * width / ((float)item_count + 1.0f);
}

int game_util_get_first_shop_booster_index()
{
    for (int i = 0; i < g_game_state.shop.total_boosters; i++)
    {
        if (g_game_state.shop.boosters[i].available) return i;
    }

    return -1;
}

int game_util_get_first_shop_item_index()
{
    for (int i = 0; i < g_game_state.shop.total_items; i++)
    {
        if (g_game_state.shop.items[i].available) return i;
    }

    return -1;
}

int game_util_get_first_shop_booster_item_index()
{
    for (int i = 0; i < g_game_state.shop.booster_total_items; i++)
    {
        if (g_game_state.shop.booster_items[i].available) return i;
    }

    return -1;
}

double random_double(double min, double max)
{
    double range = max - min;
    return ((double)rand() / (double)RAND_MAX) * range + min;
}

float random_float(float min, float max)
{
    float range = max - min;
    return ((float)rand() / (float)RAND_MAX) * range + min;
}

int random_int(int min, int max)
{
    int range = (max) - min + 1;
    return round(((double)rand() / (double)RAND_MAX) * (double)range + (double)min - 0.5);
}

void game_util_get_new_booster_pack(int *type, int *size)
{
    double r = random_double(0.0, g_booster_packs_weights_total);

    double t = 0.0;
    // TODO: ignoring spectral boosters for now
    for (int i = 0; i < BOOSTER_PACK_TYPE_COUNT - 1; i++)
    {
        for (int j = 0; j < BOOSTER_PACK_SIZE_COUNT; j++)
        {
            t += g_booster_packs_weights[i][j];
            if (r < t) {
                *type = i;
                *size = j;
                return;
            }
        }
    }

    *type = BOOSTER_PACK_TYPE_COUNT - 2;
    *size = BOOSTER_PACK_SIZE_COUNT - 1;
}

void game_util_get_joker_hint_value(struct Joker *joker, char *dst)
{
    int joker_type = joker->type;
    int joker_param = joker->param1;

    switch(joker_type)
    {
        case JOKER_TYPE_JOKER_STENCIL:
        {
            sprintf(dst, "%d", (g_game_state.joker_slots + g_game_state.jokers.negative_count) - g_game_state.jokers.joker_count + 1);
            return;
        }
        case JOKER_TYPE_BLUE_JOKER:
        {
            sprintf(dst, "%d", g_game_state.current_deck.card_count * 2);
            return;
        }
        case JOKER_TYPE_ABSTRACT_JOKER:
        {
            sprintf(dst, "%d", g_game_state.jokers.joker_count * 3);
            return;
        }
        case JOKER_TYPE_BULL:
        {
            sprintf(dst, "%d", MAX(0, g_game_state.wealth) * 2);
            return;
        }
        case JOKER_TYPE_POPCORN:
        {
            sprintf(dst, "%d", joker_param * 4);
            return;
        }
        case JOKER_TYPE_SPARE_TROUSERS:
        {
            sprintf(dst, "%d", joker_param * 2);
            return;
        }
        case JOKER_TYPE_CEREMONIAL_DAGGER:
        {
            sprintf(dst, "%d", joker_param);
            return;
        }
        case JOKER_TYPE_SWASHBUCKLER:
        {
            int value = 0;
            for (int i = 0; i < g_game_state.jokers.joker_count; i++)
            {
                if (&(g_game_state.jokers.jokers[i]) != joker)
                {
                    value += game_util_get_joker_sell_price(&(g_game_state.jokers.jokers[i]));
                }
            }
            sprintf(dst, "%d", value);
            return;
        }
        case JOKER_TYPE_RIDE_THE_BUS:
        {
            sprintf(dst, "%d", joker_param);
            return;
        }
        case JOKER_TYPE_ANCIENT_JOKER:
        {
            sprintf(dst, "%s", g_card_suits[joker_param]);
            return;
        }
        case JOKER_TYPE_TO_DO_LIST:
        {
            sprintf(dst, "%s", g_poker_hand_names[joker_param]);
            return;
        }
        case JOKER_TYPE_ICE_CREAM:
        {
            sprintf(dst, "%d", joker_param * 5);
            return;
        }
        case JOKER_TYPE_LUCKY_CAT:
        {
            sprintf(dst, "%g", (float)joker_param / 100.0f);
            return;
        }
        case JOKER_TYPE_ROCKET:
        {
            sprintf(dst, "%d", joker_param);
            return;
        }
        case JOKER_TYPE_GREEN_JOKER:
        {
            sprintf(dst, "%d", joker_param);
            return;
        }
    }

    sprintf(dst, "N/A");
}

void game_util_get_tarot_hint_value(struct Tarot *tarot, char *dst)
{
    if (tarot->type == TAROT_TYPE_FOOL)
    {
        switch (g_game_state.last_used_consumable_item_type)
        {
            case ITEM_TYPE_TAROT:
            {
                sprintf(dst, "%s", g_tarot_types[g_game_state.last_used_consumable_tarot_planet_type].name);
                break;
            }
            case ITEM_TYPE_PLANET:
            {
                sprintf(dst, "%s", g_planet_types[g_game_state.last_used_consumable_tarot_planet_type].name);
                break;
            }
            default:
            {
                sprintf(dst, "none");
                break;
            }
        }
        return;        
    }

    sprintf(dst, "N/A");
}

bool game_util_can_tarot_be_used(int type)
{
    struct TarotType *tarot_type = &g_tarot_types[type];
    if (tarot_type->min_targets > 0 && tarot_type->max_targets && (g_game_state.selected_cards_count < tarot_type->min_targets || g_game_state.selected_cards_count > tarot_type->max_targets)) return false;
    if (type == TAROT_TYPE_JUDGEMENT && !game_util_has_room_in_jokers()) return false; // TODO: Account for negatives
    if (type == TAROT_TYPE_FOOL && g_game_state.last_used_consumable_item_type == -1) return false;
    return true;
}

bool game_util_has_room_in_consumables()
{
    return g_game_state.consumables.item_count < g_game_state.consumable_slots;
}

bool game_util_has_room_in_jokers()
{
    // TODO: Account for negative jokers
    return g_game_state.jokers.joker_count < g_game_state.joker_slots;
}

int game_util_get_card_original_chips(struct Card *card)
{
    int base_chips = 0;

    if (card->enhancement == CARD_ENHANCEMENT_STONE)
    {
        base_chips = 50;
    }
    else
    {
        switch (card->rank)
        {
            case CARD_RANK_ACE:
                base_chips = 11;
                break;
            case CARD_RANK_JACK:
            case CARD_RANK_QUEEN:
            case CARD_RANK_KING:
                base_chips = 10;
                break;
            default:
                base_chips = card->rank + 2;
        }
    }

    return base_chips;
}

int game_util_get_card_chips(struct Card *card)
{
    return game_util_get_card_original_chips(card) + card->extra_chips;
}

int game_util_get_planet_type_of_poker_hand(int poker_hand)
{
    for (int i = 0; i < PLANET_TYPE_COUNT; i++)
    {
        if (g_planet_types[i].poker_hand == poker_hand) return i;
    }

    return -1;
}

int game_util_get_first_scoring_face_card_index()
{
    for (int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        if (g_game_state.played_hand.cards[i]->selected && game_util_is_card_face(g_game_state.played_hand.cards[i]))
        {
            return i;
        }
    }

    return -1;    
}

int game_util_get_index_first_scoring_card()
{
    for (int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        if (g_game_state.played_hand.cards[i]->selected)
        {
            return i;
        }
    }

    return -1; 
}

void game_util_get_first_deck_info_card(int *card_x, int *card_y)
{
    for(int i = 3; i >= 0; i--)
    {
        if (g_game_state.deck_info.card_count[i] > 0)
        {
            *card_x = 0;
            *card_y = i;
            return;
        }
    }

    *card_x = -1;
    *card_y = -1;
}

int game_util_get_random_poker_hand()
{
    int possible_poker_hands[GAME_POKER_HAND_COUNT-1];
    int poker_hands_count = 0;

    for(int i = 1; i < GAME_POKER_HAND_COUNT; i++)
    {
        if ((i != GAME_POKER_HAND_FLUSH_FIVE &&
            i != GAME_POKER_HAND_FLUSH_HOUSE &&
            i != GAME_POKER_HAND_FIVE_OF_A_KIND) ||
            (i == GAME_POKER_HAND_FLUSH_FIVE && g_game_state.flush_five_enabled) ||
            (i == GAME_POKER_HAND_FLUSH_HOUSE && g_game_state.flush_house_enabled) ||
            (i == GAME_POKER_HAND_FIVE_OF_A_KIND && g_game_state.five_of_a_kind_enabled))
        {
            possible_poker_hands[poker_hands_count++] = i;
        }
    }

    return possible_poker_hands[game_util_rand(0, poker_hands_count - 1)];
}

int game_util_get_hands()
{
    int hands = g_game_state.total_hands;

    hands -= game_util_get_real_joker_type_count(JOKER_TYPE_TROUBADOUR);
    
    return MAX(1, hands);
}

int game_util_get_discards()
{
    int discards = g_game_state.total_discards;

    discards += game_util_get_real_joker_type_count(JOKER_TYPE_DRUNKARD);
    discards += game_util_get_real_joker_type_count(JOKER_TYPE_MERRY_ANDY) * 3;

    return MAX(0, discards);
}

int game_util_get_hand_size()
{
    int hand_size = g_game_state.base_hand_size;

    hand_size += game_util_get_real_joker_type_count(JOKER_TYPE_JUGGLER);
    hand_size += game_util_get_real_joker_type_count(JOKER_TYPE_TROUBADOUR) * 2;
    hand_size -= game_util_get_real_joker_type_count(JOKER_TYPE_MERRY_ANDY);
    hand_size -= game_util_get_real_joker_type_count(JOKER_TYPE_STUNTMAN) * 2;

    return MAX(1, hand_size);
}

int game_util_get_shop_joker_edition()
{
    if (random_int(1, 1000) <= 3) return CARD_EDITION_NEGATIVE;
    if (random_int(1, 100) == 1) return CARD_EDITION_POLYCHROME;
    if (random_int(1, 50) == 3) return CARD_EDITION_HOLOGRAPHIC;
    if (random_int(1, 25) == 1) return CARD_EDITION_FOIL;

    return CARD_EDITION_BASE;
}

int game_util_get_random_booster_card_edition()
{
    int r = random_int(0, 99);
    if (r < 50) return CARD_EDITION_BASE;
    else if (r < 50 + 25) return CARD_EDITION_FOIL;
    else if (r < 50 + 25 + 17) return CARD_EDITION_HOLOGRAPHIC;
    else return CARD_EDITION_POLYCHROME;
    
    return CARD_EDITION_BASE;
}
