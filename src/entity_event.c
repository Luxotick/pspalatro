// Entity events

#include "global.h"

struct Event
{
    bool active;

    int type, sub_type;
    int card_location;
    int card_index;
    struct Position position;
    double number;
    int duration;
    int current_time;
    float start_value, end_value;
    float *value_pointer;
    struct DrawObject *draw;
};

void event_init_move_card(int index);
void event_func_move_card(int index);
void event_init_arrange_cards(int index);
void event_func_arrange_cards(int index);
void event_init_show_number(int index);
void event_func_show_number(int index);
void event_init_shake(int index);
void event_func_shake(int index);
void event_init_pop(int index);
void event_func_pop(int index);
void event_init_interpolate_value(int index);
void event_func_interpolate_value(int index);

struct EventType
{
    int type;
    void (*init)(int);
    void (*func)(int);
} g_event_types[EVENT_TYPE_COUNT] = {
    {EVENT_MOVE_CARD, event_init_move_card, event_func_move_card},
    {EVENT_ARRANGE_CARDS, event_init_arrange_cards, event_func_arrange_cards},
    {EVENT_POP_UP_CARD, NULL, NULL},
    {EVENT_SHOW_NUMBER, event_init_show_number, event_func_show_number},
    {EVENT_SHAKE, event_init_shake, event_func_shake},
    {EVENT_INTERPOLATE_VALUE, event_init_interpolate_value, event_func_interpolate_value},
    {EVENT_POP, event_init_pop, event_func_pop}
};

#define MAX_EVENTS  10

struct Event g_events[MAX_EVENTS];
int g_event_count = 0;

void event_init()
{
    for(int i = 0; i < MAX_EVENTS; i++)
    {
        g_events[i].active = false;
    }
    g_event_count = 0;
}

int event_get_empty_slot()
{
    for(int i = 0; i < MAX_EVENTS; i++)
    {
        if (!g_events[i].active)
        {
            return i;
        }
    }

    // No slots available
    DEBUG_PRINTF("ERROR: No slots available for new event\n");
    return -1;
}

int event_add(int type, int sub_type, int card_location, int card_index, float position_x, float position_y, int duration)
{
    int i = event_get_empty_slot();
    if (i != -1)
    {
        g_event_count++;

        g_events[i].active = true;
        g_events[i].type = type;
        g_events[i].sub_type = sub_type;
        g_events[i].card_location = card_location;
        g_events[i].card_index = card_index;
        g_events[i].position.x = position_x;
        g_events[i].position.y = position_y;
        g_events[i].duration = duration;
        g_events[i].current_time = 0;

        g_event_types[g_events[i].type].init(i);
        return i;
    }

    return -1;
}

int event_add_show_number(double number, int card_index, int card_location, int number_type, int duration)
{
    int i = event_get_empty_slot();
    if (i != -1)
    {
        g_event_count++;

        g_events[i].active = true;
        g_events[i].type = EVENT_SHOW_NUMBER;
        g_events[i].sub_type = number_type;
        g_events[i].card_location = card_location;
        g_events[i].card_index = card_index;
        g_events[i].duration = duration;
        g_events[i].number = number;
        g_events[i].current_time = 0;

        g_event_types[g_events[i].type].init(i);
        return i;
    }

    return -1;
}

int event_add_show_number_joker(double number, int joker_index, int number_type, int duration)
{
    int i = event_get_empty_slot();
    if (i != -1)
    {
        g_event_count++;

        g_events[i].active = true;
        g_events[i].type = EVENT_SHOW_NUMBER;
        g_events[i].sub_type = number_type;
        g_events[i].card_location = EVENT_CARD_LOCATION_JOKER;
        g_events[i].card_index = joker_index;
        g_events[i].duration = duration;
        g_events[i].number = number;
        g_events[i].current_time = 0;

        g_event_types[g_events[i].type].init(i);
        return i;
    }

    return -1;
}

int event_add_shake_joker(int joker_index, int duration)
{
    int i = event_get_empty_slot();
    if (i != -1)
    {
        g_event_count++;

        g_events[i].active = true;
        g_events[i].type = EVENT_SHAKE;
        g_events[i].card_location = EVENT_CARD_LOCATION_JOKER;
        g_events[i].card_index = joker_index;
        g_events[i].duration = duration;
        g_events[i].current_time = 0;
        g_events[i].draw = &g_game_state.jokers.jokers[joker_index].draw;

        g_event_types[g_events[i].type].init(i);
        return i;
    }

    return -1;
}

int event_add_shake_item(struct DrawObject *draw, int duration)
{
    int i = event_get_empty_slot();
    if (i != -1)
    {
        g_event_count++;

        g_events[i].active = true;
        g_events[i].type = EVENT_SHAKE;
        g_events[i].duration = duration;
        g_events[i].current_time = 0;
        g_events[i].draw = draw;

        g_event_types[g_events[i].type].init(i);
        return i;
    }

    return -1;
}

int event_add_pop_joker(int joker_index, int duration)
{
    return event_add_pop_item(&(g_game_state.jokers.jokers[joker_index].draw), duration);
}

int event_add_pop_item(struct DrawObject *draw, int duration)
{
    if (draw == NULL) return -1;

    int i = event_get_empty_slot();
    if (i != -1)
    {
        g_event_count++;

        g_events[i].active = true;
        g_events[i].type = EVENT_POP;
        g_events[i].duration = duration;
        g_events[i].current_time = 0;
        g_events[i].draw = draw;

        g_event_types[g_events[i].type].init(i);
        return i;
    }

    return -1;
}

int event_add_interpolate_value(float *value, float start_y, float end_y, int duration)
{
    int i = event_get_empty_slot();
    if (i != -1)
    {
        g_event_count++;

        g_events[i].active = true;
        g_events[i].type = EVENT_INTERPOLATE_VALUE;
        g_events[i].value_pointer = value;
        g_events[i].start_value = start_y;
        g_events[i].end_value = end_y;
        g_events[i].duration = duration;
        g_events[i].current_time = 0;

        g_event_types[g_events[i].type].init(i);
        return i;
    }

    return -1;
}

void event_remove(int index)
{
    g_events[index].active = false;
    g_event_count--;
}

void event_run()
{
    for(int i = 0; i < MAX_EVENTS; i++)
    {
        if (g_events[i].active)
        {
            DEBUG_PRINTF("Running Event %d\n", g_events[i].type);
            g_event_types[g_events[i].type].func(i);
            if (g_events[i].current_time == g_events[i].duration)
            {
                g_events[i].active = false;
                continue;
            }
            g_events[i].current_time++;
        }
    }
}

// Event funcs

float lerp(float start, float end, float t)
{
    if (end == start) return end;
    if (t >= 1.0f) return end;
    return (end - start) * t + start;
}

float lerp4(float a, float b, float t, float exponent)
{
    if (t < 0.0f) return a;
    if (t > 1.0f) return b;
  
    float normalizedT;
    if (exponent > 0.0f) {
      normalizedT = powf(t, exponent);
    } else if (exponent < 0.0f) {
      normalizedT = 1.0f - powf(1.0f - t, -exponent);
    } else {
      normalizedT = t; // Linear interpolation if exponent is 0
    }
  
    return (1.0f - normalizedT) * a + normalizedT * b;
}

// EVENT_MOVE_CARD

void event_init_move_card(int index)
{
    struct Event *event = &(g_events[index]);

    switch(event->card_location)
    {
        case EVENT_CARD_LOCATION_HAND:
        {
            g_game_state.hand.cards[event->card_index]->draw.initial_x = g_game_state.hand.cards[event->card_index]->draw.x;
            g_game_state.hand.cards[event->card_index]->draw.initial_y = g_game_state.hand.cards[event->card_index]->draw.y;
            g_game_state.hand.cards[event->card_index]->draw.final_x = event->position.x;
            g_game_state.hand.cards[event->card_index]->draw.final_y = event->position.y;
            break;
        }
        case EVENT_CARD_LOCATION_PLAYED:
        {
            g_game_state.played_hand.cards[event->card_index]->draw.initial_x = g_game_state.played_hand.cards[event->card_index]->draw.x;
            g_game_state.played_hand.cards[event->card_index]->draw.initial_y = g_game_state.played_hand.cards[event->card_index]->draw.y;
            g_game_state.played_hand.cards[event->card_index]->draw.final_x = event->position.x;
            g_game_state.played_hand.cards[event->card_index]->draw.final_y = event->position.y;
            break;
        }
        case EVENT_CARD_LOCATION_JOKER:
        {
            g_game_state.jokers.jokers[event->card_index].draw.initial_x = g_game_state.jokers.jokers[event->card_index].draw.x;
            g_game_state.jokers.jokers[event->card_index].draw.initial_y = g_game_state.jokers.jokers[event->card_index].draw.y;
            g_game_state.jokers.jokers[event->card_index].draw.final_x = event->position.x;
            g_game_state.jokers.jokers[event->card_index].draw.final_y = event->position.y;
            break;
        }
    }
}

void event_func_move_card(int index)
{
    struct Event *event = &(g_events[index]);

    // struct Card *card = NULL;
    struct DrawObject *draw = NULL;

    switch(event->card_location)
    {
        case EVENT_CARD_LOCATION_HAND:
        {
            draw = &(g_game_state.hand.cards[event->card_index]->draw);
            break;
        }
        case EVENT_CARD_LOCATION_PLAYED:
        {
            draw = &(g_game_state.played_hand.cards[event->card_index]->draw);
            break;
        }
        case EVENT_CARD_LOCATION_JOKER:
        {
            draw = &(g_game_state.jokers.jokers[event->card_index].draw);
            break;
        }
        default:
            return;
    }

    draw->x = lerp(draw->initial_x, draw->final_x, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
    draw->y = lerp(draw->initial_y, draw->final_y, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
}

// EVENT_ARRANGE_CARDS

void event_init_arrange_cards(int index)
{
    struct Event *event = &(g_events[index]);

    switch(event->card_location)
    {
        case EVENT_CARD_LOCATION_HAND:
        {
            game_set_card_hand_initial_positions();
            game_set_card_hand_final_positions();
            break;
        }
        case EVENT_CARD_LOCATION_PLAYED:
        {
            game_set_card_played_hand_final_position();
            break;
        }
        case EVENT_CARD_LOCATION_JOKER:
        {
            game_set_initial_final_joker_position();
            break;
        }
        case EVENT_CARD_LOCATION_CONSUMABLES:
        {
            game_set_initial_final_consumable_position();
            break;
        }
        case EVENT_CARD_LOCATION_BOOSTER_ITEMS:
        {
            break;
        }
    }
}

void event_func_arrange_cards(int index)
{
    struct Event *event = &(g_events[index]);

    switch(event->card_location)
    {
        case EVENT_CARD_LOCATION_HAND:
        {
            for(int i = 0; i < g_game_state.hand.card_count; i++)
            {
                struct Card *card = g_game_state.hand.cards[i];
                card->draw.x = lerp(card->draw.initial_x, card->draw.final_x, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
                card->draw.y = lerp(card->draw.initial_y, card->draw.final_y, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
            }
            break;
        }
        case EVENT_CARD_LOCATION_PLAYED:
        {
            for(int i = 0; i < g_game_state.played_hand.card_count; i++)
            {
                struct Card *card = g_game_state.played_hand.cards[i];
                card->draw.x = lerp(card->draw.initial_x, card->draw.final_x, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
                card->draw.y = lerp(card->draw.initial_y, card->draw.final_y, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
            }
            break;
        }
        case EVENT_CARD_LOCATION_JOKER:
        {
            for(int i = 0; i < g_game_state.jokers.joker_count; i++)
            {
                struct Joker *joker = &(g_game_state.jokers.jokers[i]);
                joker->draw.x = lerp(joker->draw.initial_x, joker->draw.final_x, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
                joker->draw.y = lerp(joker->draw.initial_y, joker->draw.final_y, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
            }
            break;
        }
        case EVENT_CARD_LOCATION_CONSUMABLES:
        {
            for(int i = 0; i < g_game_state.consumables.item_count; i++)
            {
                struct DrawObject *draw = (g_game_state.consumables.items[i].type == ITEM_TYPE_PLANET ? &(g_game_state.consumables.items[i].planet.draw) : &(g_game_state.consumables.items[i].tarot.draw));
                draw->x = lerp(draw->initial_x, draw->final_x, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
                draw->y = lerp(draw->initial_y, draw->final_y, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
            }
            break;
        }
        case EVENT_CARD_LOCATION_BOOSTER_ITEMS:
        {
            for(int i = 0; i < g_game_state.shop.booster_item_count; i++)
            {
                struct DrawObject *draw = &(g_game_state.shop.booster_items[i].info.card.draw);
                draw->x = lerp(draw->initial_x, draw->final_x, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
                draw->y = lerp(draw->initial_y, draw->final_y, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
            }
            break;
        }
    }
}

// EVENT_SCORE_NUMBER

double g_current_quad_angle = 0.0;

void event_init_show_number(int index)
{
    struct Event *event = &(g_events[index]);

    struct Card *card = NULL;
    struct Joker *joker = NULL;

    switch(event->card_location)
    {
        case EVENT_CARD_LOCATION_HAND:
        {
            card = g_game_state.hand.cards[event->card_index];
            break;
        }
        case EVENT_CARD_LOCATION_PLAYED:
        {
            card = g_game_state.played_hand.cards[event->card_index];
            break;
        }
        case EVENT_CARD_LOCATION_JOKER:
        {
            joker = &g_game_state.jokers.jokers[event->card_index];
            break;
        }
    }

    g_game_state.score_number.show_score_number = true;
    g_game_state.score_number.score_number_type = event->sub_type;
    g_game_state.score_number.card = card;
    g_game_state.score_number.joker = joker;
    switch(event->sub_type)
    {
        case SCORE_NUMBER_ADD_CHIPS:
            sprintf(g_game_state.score_number.text, "+%g", event->number);
            break;
        case SCORE_NUMBER_ADD_MULT:
            sprintf(g_game_state.score_number.text, "+%g MULT", event->number);
            break;
        case SCORE_NUMBER_MULT_MULT:
            sprintf(g_game_state.score_number.text, "x%g MULT", event->number);
            break;
        case SCORE_NUMBER_ADD_MONEY:
            sprintf(g_game_state.score_number.text, "$%g", event->number);
            break;
        case SCORE_NUMBER_AGAIN:
            strcpy(g_game_state.score_number.text, "AGAIN");
            break;
    }
    g_game_state.score_number.back_quad_scale = 0.0;    
    g_current_quad_angle += 0.1;
    g_game_state.score_number.back_quad_angle = g_current_quad_angle;
}

void event_func_show_number(int index)
{
    struct Event *event = &(g_events[index]);

    if (event->duration == event->current_time)
    {
        g_game_state.score_number.show_score_number = false;
    }
    else
    {
        g_game_state.score_number.show_score_number = true;
    }

    g_game_state.score_number.text_scale = lerp4(0.0, 1.5, event->duration == 0 ? 0.0f : ((float)event->current_time / ((float)event->duration / 4.0f)), -2.0f); 
    g_game_state.score_number.back_quad_scale = lerp4(0.0, 2.0, event->duration == 0 ? 0.0f : ((float)event->current_time / ((float)event->duration / 2.0f)), -2.0f); 
    g_game_state.score_number.back_quad_angle = lerp4(g_current_quad_angle, g_current_quad_angle + 0.2, event->duration == 0 ? 0.0f : ((float)event->current_time / ((float)event->duration / 2.0f)), -2.0f);
}

// EVENT_SHAKE

void event_init_shake(int index)
{
    struct Event *event = &(g_events[index]);

    event->draw->angle = 0.0f;
}

void event_func_shake(int index)
{
    struct Event *event = &(g_events[index]);

    float shake_amount = 0.5f * sinf(10.0f * (float)event->current_time);

    float dampening_factor = 1.0f - ((float)event->current_time / (float)event->duration);
    if (dampening_factor < 0.0f) {
        dampening_factor = 0.0f;
    }

    event->draw->angle += shake_amount;
    event->draw->angle *= dampening_factor;
}

// EVENT_POP

void event_init_pop(int index)
{
    struct Event *event = &(g_events[index]);

    event->draw->scale = 1.0f;
}

void event_func_pop(int index)
{
    struct Event *event = &(g_events[index]);

    float frequency = 10.0f;
    float damping_ratio = 1.0f;

    float progress = (float)event->current_time / (float)event->duration;

    event->draw->scale = 1.0f + 0.5f * powf(2.0f, -10.0f * progress * damping_ratio) * sinf(frequency * M_PI * progress);
}

// EVENT_INTERPOLATE_VALUE

void event_init_interpolate_value(int index)
{
    struct Event *event = &(g_events[index]);

    *(event->value_pointer) = event->start_value;
}

void event_func_interpolate_value(int index)
{
    struct Event *event = &(g_events[index]);

    *(event->value_pointer) = lerp(event->start_value, event->end_value, event->duration == 0 ? 0.0f : ((float)event->current_time / (float)event->duration));
}
