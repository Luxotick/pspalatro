#include "global.h"

#define TEXTURE_CARD_WIDTH  69
#define TEXTURE_CARD_HEIGHT 93

int tex_enhancers, tex_deck, tex_deck2, tex_gamepad_ui, tex_editions, tex_shop, tex_ui_assets;
int tex_jokers[2][4];
int tex_tarots[2][2];
int tex_boosters[2];

int font_big, font_small;

#define GAMEPAD_UI_CROSS            0
#define GAMEPAD_UI_CIRCLE           1
#define GAMEPAD_UI_SQUARE           2
#define GAMEPAD_UI_TRIANGLE         3
#define GAMEPAD_UI_LEFT_TRIGGER     4
#define GAMEPAD_UI_RIGHT_TRIGGER    5

#define GAMEPAD_UI_COUNT            6

#define UI_ASSETS_CHIP              0
#define UI_ASSETS_ACE               1
#define UI_ASSETS_FACE              2
#define UI_ASSETS_NUMBER            3
#define UI_ASSETS_HEARTS            4
#define UI_ASSETS_DIAMONDS          5
#define UI_ASSETS_CLUBS             6
#define UI_ASSETS_SPADES            7

#define UI_ASSETS_COUNT             8

struct TexCoords
{
    int x, y;
}
g_enhancement_tex_coords[CARD_ENHANCEMENT_COUNT] = {
    { 71, 1 },  // CARD_ENHANCEMENT_NONE
    { 71, 95 },   // CARD_ENHANCEMENT_BONUS
    { 142, 95 },   // CARD_ENHANCEMENT_MULT
    { 213, 95 },   // CARD_ENHANCEMENT_WILD
    { 355, 95 },   // CARD_ENHANCEMENT_GLASS
    { 426, 95 },   // CARD_ENHANCEMENT_STEEL
    { 355, 1 },   // CARD_ENHANCEMENT_STONE
    { 426, 1 },   // CARD_ENHANCEMENT_GOLD
    { 284, 95 }    // CARD_ENHANCEMENT_LUCKY
},
g_gamepad_ui_text_coords[GAMEPAD_UI_COUNT] = {
    { 0, 32 }, // GAMEPAD_UI_CROSS
    { 32, 32 }, // GAMEPAD_UI_CIRCLE        
    { 64, 32 }, // GAMEPAD_UI_SQUARE        
    { 96, 32 }, // GAMEPAD_UI_TRIANGLE      
    { 128, 64 }, // GAMEPAD_UI_LEFT_TRIGGER  
    { 160, 64 }  // GAMEPAD_UI_RIGHT_TRIGGER 
},
g_editions_tex_coords[CARD_EDITION_COUNT] = {
    { 1, 1 },   // CARD_EDITION_BASE
    { 143, 1 }, // CARD_EDITION_FOIL        
    { 72, 1 }, // CARD_EDITION_HOLOGRAPHIC        
    { 1, 1 }, // CARD_EDITION_POLYCHROME      
    { 1, 1 },   // CARD_EDITION_NEGATIVE  
},
g_ui_assets_tex_coords[UI_ASSETS_COUNT] = {
    { 0, 0 }, // UI_ASSETS_CHIP
    { 18, 0 }, // UI_ASSETS_ACE
    { 36, 0 }, // UI_ASSETS_FACE
    { 54, 0 }, // UI_ASSETS_NUMBER
    { 0, 18 }, // UI_ASSETS_HEARTS
    { 18, 18 }, // UI_ASSETS_DIAMONDS
    { 36, 18 }, // UI_ASSETS_CLUBS
    { 54, 18 }  // UI_ASSETS_SPADES
};

int32_t g_time = 0;

void game_init_draw_load_fonts()
{
    // font_big = graphics_load_font("font_big.png", 8, 8, 16, 16);
    // font_small = graphics_load_font("font_small.png", 6, 8, 16, 16);
    font_big = graphics_load_big_font();
    font_small = graphics_load_small_font();
}

bool game_init_draw()
{
    char str[64];
    
    if ((tex_enhancers = graphics_load_texture_from_archive_16bit("resources/textures/1x/Enhancers.png", 0, 0)) < 0) return false;

    struct Image deck_image = graphics_load_image_from_archive("resources/textures/1x/8BitDeck.png");
    if (deck_image.data == NULL) return false;
    game_draw_loading_text("Creating deck texture 0", COLOR_WHITE, COLOR_BLACK);
    tex_deck = graphics_load_texture_from_image_16bit(&deck_image, 0, 0);
    game_draw_loading_text("Creating deck texture 1", COLOR_WHITE, COLOR_BLACK);
    tex_deck2 = graphics_load_texture_from_image_16bit(&deck_image, 71*7, 0);
    graphics_destroy_image(&deck_image);

    struct Image joker_image = graphics_load_image_from_archive("resources/textures/1x/Jokers.png");
    if (joker_image.data == NULL) return false;
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            DEBUG_PRINTF("Loading Jokers (%d; %d)\n", i, j);
            sprintf(str, "Creating joker texture %d", i*4+j);
            game_draw_loading_text(str, COLOR_WHITE, COLOR_BLACK);
            tex_jokers[i][j] = graphics_load_texture_from_image_16bit(&joker_image, i * (TEXTURE_CARD_WIDTH + 2) * 7, j * (TEXTURE_CARD_HEIGHT + 2) * 5);
        }
    }
    graphics_destroy_image(&joker_image);

    tex_gamepad_ui = graphics_load_texture_from_archive_16bit("resources/textures/1x/gamepad_ui.png", 0, 0);
    if (tex_gamepad_ui < 0) return false;

    game_draw_loading_text("Loading Tarots.png", COLOR_WHITE, COLOR_BLACK);
    struct Image tarots_image = graphics_load_image_from_archive("resources/textures/1x/Tarots.png");
    if (tarots_image.data == NULL) return false;
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            DEBUG_PRINTF("Loading Tarots (%d; %d)\n", i, j);
            sprintf(str, "Creating tarots texture %d", i*2+j);
            game_draw_loading_text(str, COLOR_WHITE, COLOR_BLACK);
            tex_tarots[i][j] = graphics_load_texture_from_image_16bit(&tarots_image, i * (TEXTURE_CARD_WIDTH + 2) * 7, j * (TEXTURE_CARD_HEIGHT + 2) * 5);
        }
    }
    graphics_destroy_image(&tarots_image);

    tex_editions = graphics_load_texture_16bit("editions.png", 0, 0);
    if (tex_editions < 0) return false;

    struct Image boosters_image = graphics_load_image_from_archive("resources/textures/1x/boosters.png");
    if (boosters_image.data == NULL) return false;
    for(int j = 0; j < 2; j++)
    {
        DEBUG_PRINTF("Loading boosters (%d)\n", j);
        sprintf(str, "Creating boosters texture %d", j);
        game_draw_loading_text(str, COLOR_WHITE, COLOR_BLACK);
        tex_boosters[j] = graphics_load_texture_from_image_16bit(&boosters_image, 0, j * (TEXTURE_CARD_HEIGHT + 2) * 5);
    }
    graphics_destroy_image(&boosters_image);

    tex_shop = graphics_load_texture_from_archive_16bit("resources/textures/1x/ShopSignAnimation.png", 0, 0);
    if (tex_shop < 0) return false;

    tex_ui_assets = graphics_load_texture_from_archive_16bit("resources/textures/1x/ui_assets.png", 0, 0);
    if (tex_ui_assets < 0) return false;

    sceKernelDcacheWritebackInvalidateAll();

    return true;
}

float g_loading_text_y = 0.0f;

void game_draw_clear_loading(uint32_t color)
{
    graphics_clear_loading_text(color);

    graphics_show_loading_text(font_small, "Port by S\x82rgio Vieira & luxotick", SCREEN_WIDTH - 180.0f, SCREEN_HEIGHT - 16.0f, 1.0f, COLOR_LIGHT_GREY, false, COLOR_BLACK);
    graphics_show_loading_text(font_small, "https://github.com/internalregister & https://github.com/luxotick", SCREEN_WIDTH - 210.0f, SCREEN_HEIGHT - 8.0f, 1.0f, COLOR_LIGHT_GREY, false, COLOR_BLACK);
}

void game_draw_loading_text(const char *text, uint32_t color, uint32_t back_color)
{
    if (g_loading_text_y == 0.0f) game_draw_clear_loading(back_color); 
    graphics_show_loading_text(font_small, text, 0, g_loading_text_y, 1.0f, color, false, COLOR_BLACK);
    g_loading_text_y += 8.0f;
    if (g_loading_text_y >= SCREEN_HEIGHT) g_loading_text_y = 0.0f;
}

void game_draw_card_hint(struct Card *card)
{
    int line_count = 1;
    if (card->extra_chips > 0) line_count++;
    if (card->enhancement != CARD_ENHANCEMENT_NONE && card->enhancement != CARD_ENHANCEMENT_STONE) line_count++;
    if (card->edition != CARD_EDITION_BASE) line_count++;

    float left = card->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;
    float top = card->draw.y - height - 4.0f;
    if (top < 0.0f)
    {
        top = card->draw.y + CARD_HEIGHT + 4.0f;
    }
    graphics_set_texture(-1, -1);
    graphics_draw_quad(left, top, CARD_HINT_WIDTH, height, 0, 0, 0, 0, 0xFF888888);
    graphics_draw_quad(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, 0, 0, 0, 0, COLOR_WHITE);

    char str[64];
    game_set_card_name(card, str);
    graphics_draw_text_center(font_small, str, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, 1.0f, COLOR_WHITE);
    
    sprintf(str, "#3+%d#- chips", game_util_get_card_original_chips(card));
    float text_y = top + 14.0f + 12.0f;
    graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
    text_y += 8.0f;

    if (card->extra_chips > 0)
    {
        sprintf(str, "#3+%d#- extra chips", card->extra_chips);
        graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
        text_y += 8.0f;
    }

    if (card->enhancement != CARD_ENHANCEMENT_NONE && card->enhancement != CARD_ENHANCEMENT_STONE)
    {
        graphics_draw_text_formatted_center(font_small, g_enhancement_hint[card->enhancement], NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
        text_y += 8.0f;
    }

    if (card->edition != CARD_EDITION_BASE)
    {
        graphics_draw_text_formatted_center(font_small, g_edition_hint[card->edition], NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
    }
}

void game_draw_joker_hint(struct Joker *joker)
{
    int line_count = 0;
    for (int i = HINT_JOKER_TYPE_LENGTH - 1; i >= 0; i--)
    {
        if (g_joker_types[joker->type].hint_lines[i][0] != '\0')
        {
            line_count = i + 1;
            break;
        }
    }

    float top = joker->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count + 10.0f + (joker->edition != CARD_EDITION_BASE ? 10.0f : 0.0f);
    float left = joker->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }

    if (top + height > SCREEN_HEIGHT)
    {
        top = joker->draw.y - height - 4.0f;
    }

    graphics_set_texture(-1, -1);
    graphics_draw_quad(left, top, CARD_HINT_WIDTH, height, 0, 0, 0, 0, 0xFF888888);
    graphics_draw_quad(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, height - CARD_HINT_MIN_HEIGHT + 8.0f, 0, 0, 0, 0, COLOR_WHITE);

    graphics_draw_text_center(font_small, g_joker_types[joker->type].name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, 1.0f, COLOR_WHITE);

    for (int i = 0; i < line_count; i++)
    {
        graphics_draw_text_formatted_center(font_small, g_joker_types[joker->type].hint_lines[i], (void*)joker, left + CARD_HINT_WIDTH / 2.0f, top + 14.0f + 12.0f + (8.0f * i), 1.0f, COLOR_BLACK);
    }

    float current_y = top + 14.0f + 12.0f + (8.0f * line_count);

    if (joker->edition != CARD_EDITION_BASE)
    {
        graphics_draw_text_formatted_center(font_small, g_edition_hint[joker->edition], NULL, left + CARD_HINT_WIDTH / 2.0f, current_y + 2.0f, 1.0f, COLOR_BLACK);
        current_y += 10.0f;
    }

    graphics_draw_text_formatted_center(font_small, g_rarity_hint[g_joker_types[joker->type].rarity], NULL, left + CARD_HINT_WIDTH / 2.0f, current_y + 2.0f, 1.0f, COLOR_BLACK);
}

void game_draw_tarot_hint(struct Tarot *tarot)
{
    int line_count = 0;
    for (int i = HINT_TAROT_TYPE_LENGTH - 1; i >= 0; i--)
    {
        if (g_tarot_types[tarot->type].hint_lines[i][0] != '\0')
        {
            line_count = i + 1;
            break;
        }
    }

    float top = tarot->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;    
    float left = tarot->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }

    if (top + height > SCREEN_HEIGHT)
    {
        top = tarot->draw.y - height - 4.0f;
    }

    graphics_set_texture(-1, -1);
    graphics_draw_quad(left, top, CARD_HINT_WIDTH, height, 0, 0, 0, 0, 0xFF888888);
    graphics_draw_quad(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, 0, 0, 0, 0, COLOR_WHITE);

    graphics_draw_text_center(font_small, g_tarot_types[tarot->type].name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, 1.0f, COLOR_WHITE);
        

    for (int i = 0; i < line_count; i++)
    {
        graphics_draw_text_formatted_center(font_small, g_tarot_types[tarot->type].hint_lines[i], (void*)tarot, left + CARD_HINT_WIDTH / 2.0f, top + 14.0f + 12.0f + (8.0f * i), 1.0f, COLOR_BLACK);
    }
}

void game_draw_planet_hint(struct Planet *planet)
{
    char str[32];
    int line_count = 4;

    float top = planet->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;    
    float left = planet->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }

    if (top + height > SCREEN_HEIGHT)
    {
        top = planet->draw.y - height - 4.0f;
    }

    graphics_set_texture(-1, -1);
    graphics_draw_quad(left, top, CARD_HINT_WIDTH, height, 0, 0, 0, 0, 0xFF888888);
    graphics_draw_quad(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, 0, 0, 0, 0, COLOR_WHITE);

    graphics_draw_text_center(font_small, g_planet_types[planet->type].name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, 1.0f, COLOR_WHITE);
    
    struct PlanetType *planet_type = &(g_planet_types[planet->type]);

    float text_y = top + 14.0f + 12.0f;
    sprintf(str, "[lvl.%d] Level Up", g_game_state.poker_hand_level[planet_type->poker_hand]);
    graphics_draw_text_formatted_center(font_small, str, (void*)planet, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
    text_y += 8.0f;
    sprintf(str, "#5%s", g_poker_hand_names[planet_type->poker_hand]);
    graphics_draw_text_formatted_center(font_small, str, (void*)planet, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
    text_y += 8.0f;
    sprintf(str, "#2+%d#- Mult and", planet_type->mult);
    graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
    text_y += 8.0f;
    sprintf(str, "#3+%d#- chips", planet_type->chips);
    graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, 1.0f, COLOR_BLACK);
}

void game_draw_booster_hint(struct BoosterPack *booster)
{
    struct BoosterPackType *booster_type = &(g_booster_types[booster->type][booster->size][booster->image]);

    int line_count = 0;
    for (int i = HINT_BOOSTER_TYPE_LENGTH - 1; i >= 0; i--)
    {
        if (booster_type->hint_lines[i][0] != '\0')
        {
            line_count = i + 1;
            break;
        }
    }

    float top = booster->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;    
    float left = booster->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;

    if (top + height > SCREEN_HEIGHT)
    {
        top = booster->draw.y - height - 14.0f;
    }

    graphics_set_texture(-1, -1);
    graphics_draw_quad(left, top, CARD_HINT_WIDTH, height, 0, 0, 0, 0, 0xFF888888);
    graphics_draw_quad(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, 0, 0, 0, 0, COLOR_WHITE);

    graphics_draw_text_center(font_small, booster_type->name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, 1.0f, COLOR_WHITE);        

    for (int i = 0; i < line_count; i++)
    {
        graphics_draw_text_formatted_center(font_small, booster_type->hint_lines[i], NULL, left + CARD_HINT_WIDTH / 2.0f, top + 14.0f + 12.0f + (8.0f * i), 1.0f, COLOR_BLACK);
    }
}

bool g_freeze_cards = false;

float sin_table[256] = {
 0.0000000f,  0.0245412f,  0.0490677f,  0.0735646f,  0.0980171f,  0.1224107f,  0.1467305f,  0.1709619f,
 0.1950903f,  0.2191012f,  0.2429802f,  0.2667128f,  0.2902847f,  0.3136818f,  0.3368898f,  0.3598951f,
 0.3826835f,  0.4052413f,  0.4275551f,  0.4496113f,  0.4713967f,  0.4928982f,  0.5141027f,  0.5349976f,
 0.5555702f,  0.5758082f,  0.5956993f,  0.6152316f,  0.6343933f,  0.6531729f,  0.6715590f,  0.6895406f,
 0.7071068f,  0.7242470f,  0.7409511f,  0.7572088f,  0.7730104f,  0.7883464f,  0.8032075f,  0.8175848f,
 0.8314696f,  0.8448536f,  0.8577286f,  0.8700870f,  0.8819212f,  0.8932243f,  0.9039893f,  0.9142098f,
 0.9238795f,  0.9329928f,  0.9415441f,  0.9495282f,  0.9569404f,  0.9637761f,  0.9700313f,  0.9757021f,
 0.9807853f,  0.9852777f,  0.9891765f,  0.9924796f,  0.9951847f,  0.9972904f,  0.9987954f,  0.9996988f,
 1.0000000f,  0.9996988f,  0.9987954f,  0.9972904f,  0.9951847f,  0.9924795f,  0.9891765f,  0.9852777f,
 0.9807853f,  0.9757022f,  0.9700313f,  0.9637761f,  0.9569403f,  0.9495282f,  0.9415441f,  0.9329928f,
 0.9238796f,  0.9142097f,  0.9039893f,  0.8932243f,  0.8819213f,  0.8700870f,  0.8577286f,  0.8448535f,
 0.8314697f,  0.8175848f,  0.8032075f,  0.7883465f,  0.7730105f,  0.7572088f,  0.7409511f,  0.7242471f,
 0.7071068f,  0.6895405f,  0.6715590f,  0.6531729f,  0.6343933f,  0.6152315f,  0.5956994f,  0.5758082f,
 0.5555702f,  0.5349977f,  0.5141028f,  0.4928981f,  0.4713968f,  0.4496114f,  0.4275551f,  0.4052412f,
 0.3826835f,  0.3598951f,  0.3368898f,  0.3136818f,  0.2902847f,  0.2667127f,  0.2429801f,  0.2191013f,
 0.1950903f,  0.1709618f,  0.1467306f,  0.1224107f,  0.0980171f,  0.0735644f,  0.0490677f,  0.0245412f,
-0.0000001f, -0.0245411f, -0.0490677f, -0.0735646f, -0.0980170f, -0.1224106f, -0.1467305f, -0.1709620f,
-0.1950902f, -0.2191012f, -0.2429802f, -0.2667127f, -0.2902847f, -0.3136818f, -0.3368900f, -0.3598950f,
-0.3826834f, -0.4052414f, -0.4275550f, -0.4496113f, -0.4713968f, -0.4928983f, -0.5141027f, -0.5349976f,
-0.5555703f, -0.5758081f, -0.5956993f, -0.6152316f, -0.6343932f, -0.6531728f, -0.6715590f, -0.6895406f,
-0.7071067f, -0.7242471f, -0.7409512f, -0.7572088f, -0.7730104f, -0.7883465f, -0.8032076f, -0.8175849f,
-0.8314695f, -0.8448535f, -0.8577285f, -0.8700870f, -0.8819213f, -0.8932244f, -0.9039894f, -0.9142097f,
-0.9238795f, -0.9329928f, -0.9415441f, -0.9495282f, -0.9569404f, -0.9637761f, -0.9700312f, -0.9757021f,
-0.9807853f, -0.9852777f, -0.9891765f, -0.9924796f, -0.9951847f, -0.9972904f, -0.9987954f, -0.9996988f,
-1.0000000f, -0.9996988f, -0.9987954f, -0.9972904f, -0.9951847f, -0.9924796f, -0.9891765f, -0.9852777f,
-0.9807853f, -0.9757021f, -0.9700312f, -0.9637761f, -0.9569404f, -0.9495282f, -0.9415441f, -0.9329928f,
-0.9238794f, -0.9142098f, -0.9039894f, -0.8932244f, -0.8819213f, -0.8700870f, -0.8577285f, -0.8448535f,
-0.8314697f, -0.8175849f, -0.8032076f, -0.7883464f, -0.7730104f, -0.7572088f, -0.7409510f, -0.7242472f,
-0.7071069f, -0.6895406f, -0.6715590f, -0.6531728f, -0.6343932f, -0.6152315f, -0.5956995f, -0.5758083f,
-0.5555703f, -0.5349976f, -0.5141027f, -0.4928981f, -0.4713965f, -0.4496115f, -0.4275552f, -0.4052414f,
-0.3826834f, -0.3598950f, -0.3368897f, -0.3136815f, -0.2902848f, -0.2667129f, -0.2429802f, -0.2191012f,
-0.1950902f, -0.1709617f, -0.1467302f, -0.1224108f, -0.0980172f, -0.0735646f, -0.0490676f, -0.0245411f
};

float cos_table[256] = {
 1.0000000f,  0.9996988f,  0.9987954f,  0.9972904f,  0.9951847f,  0.9924796f,  0.9891765f,  0.9852777f,
 0.9807853f,  0.9757021f,  0.9700313f,  0.9637761f,  0.9569404f,  0.9495282f,  0.9415441f,  0.9329928f,
 0.9238795f,  0.9142098f,  0.9039893f,  0.8932243f,  0.8819213f,  0.8700870f,  0.8577286f,  0.8448536f,
 0.8314696f,  0.8175848f,  0.8032075f,  0.7883465f,  0.7730105f,  0.7572088f,  0.7409511f,  0.7242471f,
 0.7071068f,  0.6895406f,  0.6715590f,  0.6531729f,  0.6343933f,  0.6152316f,  0.5956993f,  0.5758082f,
 0.5555702f,  0.5349976f,  0.5141028f,  0.4928982f,  0.4713968f,  0.4496113f,  0.4275551f,  0.4052413f,
 0.3826834f,  0.3598951f,  0.3368898f,  0.3136818f,  0.2902846f,  0.2667128f,  0.2429802f,  0.2191012f,
 0.1950904f,  0.1709619f,  0.1467305f,  0.1224106f,  0.0980171f,  0.0735646f,  0.0490676f,  0.0245413f,
-0.0000000f, -0.0245412f, -0.0490676f, -0.0735646f, -0.0980171f, -0.1224107f, -0.1467305f, -0.1709619f,
-0.1950903f, -0.2191012f, -0.2429802f, -0.2667127f, -0.2902847f, -0.3136817f, -0.3368898f, -0.3598951f,
-0.3826834f, -0.4052413f, -0.4275551f, -0.4496114f, -0.4713966f, -0.4928982f, -0.5141028f, -0.5349977f,
-0.5555702f, -0.5758082f, -0.5956994f, -0.6152315f, -0.6343933f, -0.6531729f, -0.6715590f, -0.6895405f,
-0.7071068f, -0.7242472f, -0.7409511f, -0.7572088f, -0.7730105f, -0.7883465f, -0.8032075f, -0.8175848f,
-0.8314697f, -0.8448535f, -0.8577286f, -0.8700870f, -0.8819212f, -0.8932243f, -0.9039893f, -0.9142098f,
-0.9238795f, -0.9329928f, -0.9415441f, -0.9495282f, -0.9569404f, -0.9637761f, -0.9700313f, -0.9757021f,
-0.9807853f, -0.9852777f, -0.9891765f, -0.9924795f, -0.9951847f, -0.9972905f, -0.9987954f, -0.9996988f,
-1.0000000f, -0.9996988f, -0.9987954f, -0.9972904f, -0.9951847f, -0.9924796f, -0.9891765f, -0.9852777f,
-0.9807853f, -0.9757021f, -0.9700313f, -0.9637761f, -0.9569404f, -0.9495282f, -0.9415441f, -0.9329928f,
-0.9238795f, -0.9142097f, -0.9039893f, -0.8932243f, -0.8819212f, -0.8700869f, -0.8577287f, -0.8448536f,
-0.8314695f, -0.8175849f, -0.8032075f, -0.7883464f, -0.7730106f, -0.7572089f, -0.7409511f, -0.7242470f,
-0.7071068f, -0.6895406f, -0.6715589f, -0.6531729f, -0.6343933f, -0.6152316f, -0.5956992f, -0.5758080f,
-0.5555704f, -0.5349978f, -0.5141028f, -0.4928982f, -0.4713967f, -0.4496112f, -0.4275549f, -0.4052415f,
-0.3826836f, -0.3598951f, -0.3368899f, -0.3136817f, -0.2902845f, -0.2667126f, -0.2429804f, -0.2191014f,
-0.1950904f, -0.1709619f, -0.1467304f, -0.1224105f, -0.0980169f, -0.0735648f, -0.0490678f, -0.0245413f,
 0.0000000f,  0.0245413f,  0.0490678f,  0.0735648f,  0.0980170f,  0.1224106f,  0.1467304f,  0.1709619f,
 0.1950904f,  0.2191014f,  0.2429804f,  0.2667126f,  0.2902846f,  0.3136817f,  0.3368899f,  0.3598951f,
 0.3826836f,  0.4052411f,  0.4275549f,  0.4496112f,  0.4713967f,  0.4928982f,  0.5141028f,  0.5349978f,
 0.5555701f,  0.5758080f,  0.5956993f,  0.6152316f,  0.6343933f,  0.6531729f,  0.6715591f,  0.6895404f,
 0.7071066f,  0.7242470f,  0.7409511f,  0.7572089f,  0.7730106f,  0.7883465f,  0.8032074f,  0.8175848f,
 0.8314696f,  0.8448536f,  0.8577287f,  0.8700871f,  0.8819214f,  0.8932242f,  0.9039893f,  0.9142097f,
 0.9238796f,  0.9329928f,  0.9415441f,  0.9495283f,  0.9569403f,  0.9637761f,  0.9700313f,  0.9757021f,
 0.9807853f,  0.9852777f,  0.9891766f,  0.9924795f,  0.9951847f,  0.9972904f,  0.9987954f,  0.9996988f
};

void game_draw_get_oscillating_position(struct DrawObject *draw, float *x, float *y)
{
    *x = draw->x;
    *y = draw->y;

    if (g_settings.move_cards && !g_freeze_cards)
    {
        float t = (float)(g_time + draw->r) / 40.0f;
        float t2 = t / (2 * M_PI);
        int t3 = (t2 - (float)((int)t2)) * 256;
        t3 = CLAMP(t3, 0, 255);
        *x = draw->x + cos_table[t3] * 1.0f;
        *y = draw->y + sin_table[t3] * 1.1f;
    }
}

void game_draw_card(struct Card *card, struct DrawObject *draw_override)
{
    struct DrawObject *draw = &card->draw;
    if (draw_override != NULL)
    {
        draw = draw_override;
    }

    float x, y;
    game_draw_get_oscillating_position(draw, &x, &y);

    x -= ((CARD_WIDTH * draw->scale) - CARD_WIDTH) / 2.0f;
    y -= ((CARD_HEIGHT * draw->scale) - CARD_HEIGHT) / 2.0f;
    float w = CARD_WIDTH * draw->scale;
    float h = CARD_HEIGHT * draw->scale;

    graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_rotated_quad(x, y, w, h, g_enhancement_tex_coords[card->enhancement].x, g_enhancement_tex_coords[card->enhancement].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, card->draw.angle);

    if (card->enhancement != CARD_ENHANCEMENT_STONE)
    {
        if (card->rank < 7)
        {
            graphics_set_texture(tex_deck, GRAPHICS_TEXTURE_FILTER_LINEAR);
            graphics_draw_rotated_quad(x, y, w, h, 1 + (TEXTURE_CARD_WIDTH + 2) * card->rank, 1 + (TEXTURE_CARD_HEIGHT + 2) * card->suit, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, card->draw.angle);
        }
        else
        {
            graphics_set_texture(tex_deck2, GRAPHICS_TEXTURE_FILTER_LINEAR);
            graphics_draw_rotated_quad(x, y, w, h, 1 + (TEXTURE_CARD_WIDTH + 2) * (card->rank - 7), 1 + (TEXTURE_CARD_HEIGHT + 2) * card->suit, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, card->draw.angle);
        }
    }

    if (card->edition != CARD_EDITION_BASE && card->edition != CARD_EDITION_NEGATIVE)
    {        
        graphics_set_texture(tex_editions, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_rotated_quad(x, y, w, h, g_editions_tex_coords[card->edition].x, g_editions_tex_coords[card->edition].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0x7FFFFFFF, card->draw.angle);
    }

    if (card->draw.white_factor > 0.0f)
    {
        graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
        uint32_t color = 0xFFFFFF | ((uint32_t)(255.0f * (card->draw.white_factor > 1.0f ? 1.0f : card->draw.white_factor))<<24);        
        graphics_draw_quad(x, y, w, h, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, color);        
    }
}

void game_draw_joker(struct Joker *joker)
{
    int tex_joker_x, tex_joker_y = 0;
    struct JokerType *joker_type = &g_joker_types[joker->type];
    tex_joker_x = joker_type->u / 7;
    tex_joker_y = joker_type->v / 5; 
    
    float x, y;
    game_draw_get_oscillating_position(&(joker->draw), &x, &y);

    x -= ((CARD_WIDTH * joker->draw.scale) - CARD_WIDTH) / 2.0f;
    y -= ((CARD_HEIGHT * joker->draw.scale) - CARD_HEIGHT) / 2.0f;
    float w = CARD_WIDTH * joker->draw.scale;
    float h = CARD_HEIGHT * joker->draw.scale;

    if (joker->edition == CARD_EDITION_NEGATIVE)
    {
        graphics_flush_quads();    
        sceGuEnable(GU_COLOR_LOGIC_OP);
        sceGuLogicalOp(GU_COPY_INVERTED);    
        sceGuEnable(GU_ALPHA_TEST);
        sceGuAlphaFunc(GU_GREATER, 0, 0xFF);
    }

    graphics_set_texture(tex_jokers[tex_joker_x][tex_joker_y], GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_rotated_quad(
        x, y, w, h,
        1 + (joker_type->u - tex_joker_x * 7) * (TEXTURE_CARD_WIDTH + 2),
        1 + (joker_type->v - tex_joker_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xFFFFFFFF, joker->draw.angle);

    if (joker->type == JOKER_TYPE_HOLOGRAM)
    {
        tex_joker_x = 2 / 7;
        tex_joker_y = 9 / 5;
        graphics_set_texture(tex_jokers[tex_joker_x][tex_joker_y], GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_rotated_quad(
            x, y, w, h,
            1 + (2 - tex_joker_x * 7) * (TEXTURE_CARD_WIDTH + 2),
            1 + (9 - tex_joker_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
            TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xFFFFFFFF, joker->draw.angle);
    }

    if (joker->edition == CARD_EDITION_NEGATIVE)
    {
        graphics_flush_quads();
        sceGuDisable(GU_ALPHA_TEST);
        sceGuDisable(GU_COLOR_LOGIC_OP);
    }

    if (joker->edition != CARD_EDITION_BASE && joker->edition != CARD_EDITION_NEGATIVE)
    {        
        graphics_set_texture(tex_editions, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_rotated_quad(x, y, w, h, g_editions_tex_coords[joker->edition].x, g_editions_tex_coords[joker->edition].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0x7FFFFFFF, joker->draw.angle);
    }
}

void game_draw_card_frame(struct DrawObject *draw)
{
    float card_x = draw->x, card_y = draw->y;
    graphics_set_no_texture();
    graphics_draw_quad(card_x - 3.0f, card_y - 3.0f, CARD_WIDTH + 6.0f, 2.0f, 0, 0, 0, 0, COLOR_WHITE);
    graphics_draw_quad(card_x - 3.0f, card_y - 3.0f, 2.0f, CARD_HEIGHT + 6.0f, 0, 0, 0, 0, COLOR_WHITE);
    graphics_draw_quad(card_x - 3.0f, card_y + CARD_HEIGHT + 1.0f, CARD_WIDTH + 6.0f, 2.0f, 0, 0, 0, 0, COLOR_WHITE);
    graphics_draw_quad(card_x + CARD_WIDTH + 1.0f, card_y - 3.0f, 2.0f, CARD_HEIGHT + 6.0f, 0, 0, 0, 0, COLOR_WHITE);
}

void game_draw_booster_frame(struct DrawObject *draw)
{
    float card_x = draw->x, card_y = draw->y;
    graphics_set_no_texture();
    graphics_draw_quad(card_x - 3.0f, card_y - 3.0f, BOOSTER_WIDTH + 6.0f, 2.0f, 0, 0, 0, 0, COLOR_WHITE);
    graphics_draw_quad(card_x - 3.0f, card_y - 3.0f, 2.0f, BOOSTER_HEIGHT + 6.0f, 0, 0, 0, 0, COLOR_WHITE);
    graphics_draw_quad(card_x - 3.0f, card_y + BOOSTER_HEIGHT + 1.0f, BOOSTER_WIDTH + 6.0f, 2.0f, 0, 0, 0, 0, COLOR_WHITE);
    graphics_draw_quad(card_x + BOOSTER_WIDTH + 1.0f, card_y - 3.0f, 2.0f, BOOSTER_HEIGHT + 6.0f, 0, 0, 0, 0, COLOR_WHITE);
}

void game_draw_item_button(struct DrawObject *draw, int gamepad_ui, char *text, char *text2, uint32_t button_color, bool left)
{
    float item_x = draw->x, item_y = draw->y;
    float x = left ? item_x - 40.0f : item_x + CARD_WIDTH;
    float height = text2 != NULL ? 35.0f : 30.0f;
    graphics_set_no_texture();
    graphics_draw_quad(x, item_y + (CARD_HEIGHT / 2.0f) - 20.0f, 40.0f, height, 0, 0, 0, 0, button_color);
    graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(x + 7.0f, item_y + (CARD_HEIGHT / 2.0f) - 22.0f, 18.0f, 18.0f,
        g_gamepad_ui_text_coords[gamepad_ui].x,
        g_gamepad_ui_text_coords[gamepad_ui].y,
        32, 32, COLOR_WHITE);
    graphics_draw_text_center(font_small, text, x + 20.0f, item_y + (CARD_HEIGHT / 2.0f), 1.0f, COLOR_WHITE);
    if (text2 != NULL) graphics_draw_text_center(font_small, text2, x + 20.0f, item_y + (CARD_HEIGHT / 2.0f) + 10.0f, 1.0f, COLOR_WHITE);
}

void game_draw_price_tag(struct DrawObject *draw, int price)
{
    float card_x = draw->x, card_y = draw->y;
    graphics_set_no_texture();
    graphics_draw_quad(card_x + (CARD_WIDTH / 2.0f) - 15.0f, card_y - 15.0f, 30.0f, 16.0f, 0, 0, 0, 0, COLOR_DARK_GREY_2);
    char str[16];
    sprintf(str, "$%d", price);
    graphics_draw_text_center(font_small, str, card_x + (CARD_WIDTH / 2.0f), card_y - 7.0f, 1.0f, COLOR_WHITE);
}

void game_draw_tarot(struct Tarot *tarot)
{
    int tex_tarot_x, tex_tarot_y = 0;
    struct TarotType *tarot_type = &g_tarot_types[tarot->type];
    tex_tarot_x = tarot_type->u / 7;
    tex_tarot_y = tarot_type->v / 5;

    float x, y;
    game_draw_get_oscillating_position(&(tarot->draw), &x, &y);

    graphics_set_texture(tex_tarots[tex_tarot_x][tex_tarot_y], GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_rotated_quad(
        x, y, CARD_WIDTH, CARD_HEIGHT,
        1 + (tarot_type->u - tex_tarot_x * 7) * (TEXTURE_CARD_WIDTH + 2),
        1 + (tarot_type->v - tex_tarot_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, tarot->draw.angle);
    
    if (tarot->draw.white_factor > 0.0f)
    {
        graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
        uint32_t color = 0xFFFFFF | ((uint32_t)(255.0f * (tarot->draw.white_factor > 1.0f ? 1.0f : tarot->draw.white_factor))<<24);        
        graphics_draw_quad(x, y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, color);
    }
}

void game_draw_planet(struct Planet *planet)
{
    int tex_planet_x, tex_planet_y = 0;
    struct PlanetType *planet_type = &g_planet_types[planet->type];
    tex_planet_x = planet_type->u / 7;
    tex_planet_y = planet_type->v / 5;

    float x, y;
    game_draw_get_oscillating_position(&(planet->draw), &x, &y);
    
    graphics_set_texture(tex_tarots[tex_planet_x][tex_planet_y], GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_rotated_quad(
        x, y, CARD_WIDTH, CARD_HEIGHT,
        1 + (planet_type->u - tex_planet_x * 7) * (TEXTURE_CARD_WIDTH + 2),
        1 + (planet_type->v - tex_planet_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, planet->draw.angle);
    
    if (planet->draw.white_factor > 0.0f)
    {
        graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
        uint32_t color = 0xFFFFFF | ((uint32_t)(255.0f * (planet->draw.white_factor > 1.0f ? 1.0f : planet->draw.white_factor))<<24);        
        graphics_draw_quad(x, y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, color);
    }
}

void game_draw_booster(struct BoosterPack *booster)
{
    int text_booster_y = 0;
    struct BoosterPackType *booster_type = &(g_booster_types[booster->type][booster->size][booster->image]);
    text_booster_y = booster_type->v / 5;

    float x, y;
    game_draw_get_oscillating_position(&(booster->draw), &x, &y);

    graphics_set_texture(tex_boosters[text_booster_y], GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_rotated_quad(
        x, y, BOOSTER_WIDTH, BOOSTER_HEIGHT,
        1 + (booster_type->u) * (TEXTURE_CARD_WIDTH + 2),
        1 + (booster_type->v - text_booster_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, booster->draw.angle);
    game_draw_price_tag(&(booster->draw), game_util_get_booster_price(booster));
}

void game_draw_shop_singles()
{
    for (int i = 0; i < g_game_state.shop.total_items; i++)
    {
        if (!g_game_state.shop.items[i].available) continue;
        
        if (g_game_state.shop.items[i].type == ITEM_TYPE_JOKER)
        {
            struct Joker *joker = &(g_game_state.shop.items[i].info.joker);
            game_draw_joker(joker);
            game_draw_price_tag(&joker->draw, game_util_get_joker_buy_price(joker));
        }
        else if (g_game_state.shop.items[i].type == ITEM_TYPE_PLANET)
        {
            struct Planet *planet = &(g_game_state.shop.items[i].info.planet);
            game_draw_planet(planet);
            game_draw_price_tag(&planet->draw, game_util_get_planet_buy_price(planet));
        }
        else if (g_game_state.shop.items[i].type == ITEM_TYPE_TAROT)
        {
            struct Tarot *tarot = &(g_game_state.shop.items[i].info.tarot);
            game_draw_tarot(tarot);
            game_draw_price_tag(&tarot->draw, game_util_get_tarot_buy_price(tarot));
        }
    }
}
void game_draw_shop_boosters()
{
    for (int i = 0; i < g_game_state.shop.total_boosters; i++)
    {
        if (!g_game_state.shop.boosters[i].available) continue;
        
        struct BoosterPack *booster = &(g_game_state.shop.boosters[i].booster);
        game_draw_booster(booster);
    }
}

void game_draw_shop_selected_single()
{
    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_SINGLES)
    {
        struct DrawObject *draw = NULL;
        switch (g_game_state.shop.items[g_game_state.highlighted_item].type)
        {
            case ITEM_TYPE_JOKER:
                game_draw_joker_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.joker));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.joker.draw;                
                break;
            case ITEM_TYPE_CARD:
                game_draw_card_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.card));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.card.draw;
                break;
            case ITEM_TYPE_PLANET:
                game_draw_planet_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.planet));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.planet.draw;
                break;
            case ITEM_TYPE_TAROT:
                game_draw_tarot_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.tarot));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.tarot.draw;
                break;
            default:
                break;
        }

        game_draw_card_frame(draw);
        game_draw_item_button(draw, GAMEPAD_UI_RIGHT_TRIGGER, "Buy", NULL, COLOR_RED_BUTTON, false);
        if (g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_PLANET ||
            g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_TAROT)
        {
            game_draw_item_button(draw, GAMEPAD_UI_LEFT_TRIGGER, "Buy", "&Use", COLOR_GREEN_BUTTON, true);
        }
    }
}

void game_draw_shop_selected_booster()
{
    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_BOOSTERS)
    {
        struct DrawObject *draw = &(g_game_state.shop.boosters[g_game_state.highlighted_item].booster.draw);        

        game_draw_booster_frame(draw);
        game_draw_item_button(draw, GAMEPAD_UI_RIGHT_TRIGGER, "Open", NULL, COLOR_GREEN_BUTTON, false);
        game_draw_booster_hint(&(g_game_state.shop.boosters[g_game_state.highlighted_item].booster));
    }
}

void game_draw_consumables()
{
    graphics_draw_solid_quad(DRAW_CONSUMABLES_X, DRAW_CONSUMABLES_y, DRAW_CONSUMABLES_WIDTH, CARD_HEIGHT, 0x66000000);

    for (int i = 0; i < g_game_state.consumables.item_count; i++)
    {
        if (i == g_game_state.highlighted_item && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_CONSUMABLES) continue;
        if (g_game_state.consumables.items[i].type == ITEM_TYPE_PLANET) game_draw_planet(&(g_game_state.consumables.items[i].planet));
        if (g_game_state.consumables.items[i].type == ITEM_TYPE_TAROT) game_draw_tarot(&(g_game_state.consumables.items[i].tarot));
    }

    char str[24];
    sprintf(str, "%d/%d", g_game_state.consumables.item_count, g_game_state.consumable_slots);
    graphics_draw_text(font_small, str, DRAW_CONSUMABLES_X + 2.0f, DRAW_CONSUMABLES_y + CARD_HEIGHT + 1.0f, 1.0f, COLOR_WHITE);

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_CONSUMABLES)
    {        
        if (g_game_state.consumables.items[g_game_state.highlighted_item].type == ITEM_TYPE_PLANET)
        {
            struct Planet *selected_planet = &(g_game_state.consumables.items[g_game_state.highlighted_item].planet);
            game_draw_card_frame(&selected_planet->draw);
            game_draw_planet(selected_planet);
            sprintf(str, "$%d", game_util_get_planet_sell_price(selected_planet));
            game_draw_item_button(&selected_planet->draw, GAMEPAD_UI_LEFT_TRIGGER, "Sell", str, COLOR_GREEN_BUTTON, true);
            game_draw_item_button(&selected_planet->draw, GAMEPAD_UI_RIGHT_TRIGGER, "Use", NULL, COLOR_RED_BUTTON, false);
            game_draw_planet_hint(selected_planet);
        }
        else if (g_game_state.consumables.items[g_game_state.highlighted_item].type == ITEM_TYPE_TAROT)
        {
            struct Tarot *selected_tarot = &(g_game_state.consumables.items[g_game_state.highlighted_item].tarot);
            game_draw_card_frame(&selected_tarot->draw);
            game_draw_tarot(selected_tarot);
            sprintf(str, "$%d", game_util_get_tarot_sell_price(selected_tarot));
            game_draw_item_button(&selected_tarot->draw, GAMEPAD_UI_LEFT_TRIGGER, "Sell", str, COLOR_GREEN_BUTTON, true);
            game_draw_item_button(&selected_tarot->draw, GAMEPAD_UI_RIGHT_TRIGGER, "Use", NULL, COLOR_RED_BUTTON, false);
            game_draw_tarot_hint(selected_tarot);
        }
    }
}

void game_draw_top_jokers()
{
    graphics_draw_solid_quad(DRAW_JOKERS_X, DRAW_JOKERS_Y, DRAW_JOKERS_WIDTH, CARD_HEIGHT, 0x66000000);

    for (int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        if (i == g_game_state.highlighted_item && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_JOKERS) continue;
        game_draw_joker(&g_game_state.jokers.jokers[i]);
    }

    char str[24];
    sprintf(str, "%d/%d", g_game_state.jokers.joker_count, g_game_state.joker_slots + g_game_state.jokers.negative_count);
    graphics_draw_text(font_small, str, DRAW_JOKERS_X + 2.0f, DRAW_JOKERS_Y + CARD_HEIGHT + 1.0f, 1.0f, COLOR_WHITE);

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_JOKERS)
    {
        struct Joker *selected_joker = &g_game_state.jokers.jokers[g_game_state.highlighted_item];
        game_draw_card_frame(&selected_joker->draw);
        game_draw_joker(selected_joker);
        sprintf(str, "$%d", game_util_get_joker_sell_price(selected_joker));
        game_draw_item_button(&selected_joker->draw, GAMEPAD_UI_LEFT_TRIGGER, "Sell", str, COLOR_GREEN_BUTTON, true);
        game_draw_joker_hint(&(g_game_state.jokers.jokers[g_game_state.highlighted_item]));
    }
}

void game_draw_ingame_hand()
{
    graphics_set_no_texture();
    graphics_draw_quad(DRAW_HAND_X, game_get_hand_y(), DRAW_HAND_WIDTH, CARD_HEIGHT, 0, 0, 0, 0, 0x66000000);

    for (int i = 0; i < g_game_state.hand.card_count; i++)
    {
        game_draw_card(g_game_state.hand.cards[i], NULL);
    }

    if (g_game_state.show_highlighted_card && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND)
    {
        if (g_game_state.highlighted_item > -1 && g_game_state.highlighted_item < g_game_state.hand.card_count)
        {
            game_draw_card_hint(g_game_state.hand.cards[g_game_state.highlighted_item]);
        }
    }

    for (int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        game_draw_card(g_game_state.played_hand.cards[i], NULL);
    }

    // Draw card count
    char str[24];
    sprintf(str, "%d/%d", g_game_state.hand.card_count, game_util_get_hand_size());
    graphics_draw_text(font_small, str, DRAW_HAND_X + 2.0f, game_get_hand_y() + CARD_HEIGHT + 1.0f, 1.0f, COLOR_WHITE);

    // Draw other buttons
    if (game_get_hand_y() == DRAW_HAND_Y)
    {
        graphics_set_no_texture();
        float x = DRAW_HAND_X + (DRAW_HAND_WIDTH / 2.0f) - 40.0f;
        float y = DRAW_HAND_Y + CARD_HEIGHT;
        graphics_draw_quad(x, y, 80.0f, 30.0f, 0, 0, 0, 0, COLOR_DARK_GREY);
        graphics_draw_text_center(font_small, "Sort Hand", x + 40.0f, y + 7.0f, 1.0f, COLOR_WHITE);
        graphics_set_no_texture();
        if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SORT_HAND_RANK)
        {
            graphics_draw_quad(x + 1.0f, y + 13.0f, 37.0f, 16.0f, 0, 0, 0, 0, COLOR_WHITE);
        }
        graphics_draw_quad(x + 3.0f, y + 14.0f, 34.0f, 14.0f, 0, 0, 0, 0, 0xFF0097FC);        
        graphics_draw_text_center(font_small, "Rank", x + 20.0f, y + 20.0f, 1.0f, COLOR_WHITE);
        graphics_set_no_texture();
        if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SORT_HAND_SUIT)
        {
            graphics_draw_quad(x + 41.0f, y + 13.0f, 37.0f, 16.0f, 0, 0, 0, 0, COLOR_WHITE);
        }
        graphics_draw_quad(x + 43.0f, y + 14.0f, 34.0f, 14.0f, 0, 0, 0, 0, 0xFF0097FC);
        graphics_draw_text_center(font_small, "Suit", x + 62.0f, y + 20.0f, 1.0f, COLOR_WHITE);

        // Play Hand
        x = DRAW_HAND_X + (DRAW_HAND_WIDTH / 2.0f) - 120.0f;
        y = DRAW_HAND_Y + CARD_HEIGHT + 2.0f;
        graphics_set_no_texture();
        if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND_PLAY)
        {
            graphics_draw_quad(x - 2.0f, y - 2.0f, 82.0f, 30.0f, 0, 0, 0, 0, COLOR_WHITE);
        }
        graphics_draw_quad(x, y, 78.0f, 26.0f, 0, 0, 0, 0, COLOR_LIGHT_BLUE);
        graphics_draw_text_center(font_small, "Play Hand", x + 48.0f, y + 12.0f, 1.0f, COLOR_WHITE);
        graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(x + 2.0f, y + 4.0f, 16.0f, 16.0f,
            g_gamepad_ui_text_coords[GAMEPAD_UI_SQUARE].x,
            g_gamepad_ui_text_coords[GAMEPAD_UI_SQUARE].y,
            32, 32, COLOR_WHITE);

        // Discard
        x = DRAW_HAND_X + (DRAW_HAND_WIDTH / 2.0f) + 42.0f;
        graphics_set_no_texture();
        if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND_DISCARD)
        {
            graphics_draw_quad(x - 2.0f, y - 2.0f, 82.0f, 30.0f, 0, 0, 0, 0, COLOR_WHITE);
        }
        graphics_draw_quad(x, y, 78.0f, 26.0f, 0, 0, 0, 0, COLOR_LIGHT_RED);
        graphics_draw_text_center(font_small, "Discard", x + 30.0f, y + 12.0f, 1.0f, COLOR_WHITE);
        graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(x + 60.0f, y + 4.0f, 16.0f, 16.0f,
            g_gamepad_ui_text_coords[GAMEPAD_UI_TRIANGLE].x,
            g_gamepad_ui_text_coords[GAMEPAD_UI_TRIANGLE].y,
            32, 32, COLOR_WHITE);        
    }
}

void game_draw_booster_hand()
{
    graphics_set_no_texture();
    graphics_draw_quad(DRAW_HAND_X, DRAW_HAND_Y_IN_BOOSTER, DRAW_HAND_WIDTH, CARD_HEIGHT, 0, 0, 0, 0, 0x66000000);

    for (int i = 0; i < g_game_state.hand.card_count; i++)
    {
        game_draw_card(g_game_state.hand.cards[i], NULL);
    }
}

void game_draw_booster_hand_selected_card()
{
    if (g_game_state.show_highlighted_card && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND)
    {
        if (g_game_state.highlighted_item > -1 && g_game_state.highlighted_item < g_game_state.hand.card_count)
        {
            game_draw_card_hint(g_game_state.hand.cards[g_game_state.highlighted_item]);
        }
    }
}

void game_draw_deck()
{
    char str[24];

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_DECK)
    {
        struct DrawObject draw;
        draw.x = DRAW_DECK_X;
        draw.y = DRAW_DECK_Y;
        game_draw_card_frame(&(draw));
    }

    // Draw deck
    graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(DRAW_DECK_X, DRAW_DECK_Y, CARD_WIDTH, CARD_HEIGHT, 0, 0, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE);    
    // Deck size
    sprintf(str, "%d/%d", g_game_state.current_deck.card_count, g_game_state.full_deck.card_count);
    graphics_draw_text_center(font_small, str, DRAW_DECK_X + (CARD_WIDTH / 2.0f), DRAW_DECK_Y + CARD_HEIGHT + 8.0f, 1.0f, COLOR_WHITE);
}

#define DRAW_LEFT_INFO_WIDTH 96

void game_draw_left_info()
{
    static int shop_anim = 0;
    char str[24];

    graphics_set_no_texture();
    graphics_draw_quad(2, 2, DRAW_LEFT_INFO_WIDTH, SCREEN_HEIGHT - 4, 0, 0, 0, 0, COLOR_DARK_GREY);

    int y = 10.0f;

    switch (g_game_state.stage)
    {
        case GAME_STAGE_BLINDS:
        {
            graphics_draw_text(font_big, "Choose your", 6, y, 1.0f, COLOR_WHITE);
            y += 16;
            graphics_draw_text(font_big, "next Blind", 6, y, 1.0f, COLOR_WHITE);
            y += 28;
            break;
        }
        case GAME_STAGE_INGAME:
        {
            switch (g_game_state.blind)
            {
                case GAME_BLIND_SMALL:
                    graphics_draw_text(font_big, "Small Blind", 6, y, 1.0f, COLOR_WHITE);
                    break;
                case GAME_BLIND_LARGE:
                    graphics_draw_text(font_big, "Large Blind", 6, y, 1.0f, COLOR_WHITE);
                    break;
                case GAME_BLIND_BOSS:
                    graphics_draw_text(font_big, "Boss Blind", 6, y, 1.0f, COLOR_WHITE);
                    break;
            }
            y += 12;
            graphics_draw_text(font_small, "Score at least", 6, y, 1.0f, COLOR_WHITE);
            y += 8;
            sprintf(str, "%g", game_get_current_blind_score());
            graphics_draw_text(font_small, str, 6, y, 1.0f, COLOR_WHITE);
            y += 24;            

            break;
        }
        case GAME_STAGE_SHOP:
        {
            if (g_game_counter % 10 == 0) shop_anim++;
            if (shop_anim > 3) shop_anim = 0;

            graphics_set_texture(tex_shop, GRAPHICS_TEXTURE_FILTER_LINEAR);
            graphics_draw_quad(4.0f, y - 8.0f, DRAW_LEFT_INFO_WIDTH - 4.0f, 50.0f, 113 * shop_anim, 0, 113, 57, COLOR_WHITE);
            y += 44;
            break;
        }
        default:
        {
            y += 44;
            break;
        }
    }

    graphics_set_no_texture();
    graphics_draw_quad(4, y - 2, DRAW_LEFT_INFO_WIDTH - 4, 30, 0, 0, 0, 0, COLOR_DARK_GREY_2);

    graphics_draw_text(font_big, "Round Score", 6, y, 1.0f, COLOR_WHITE);    
    
    y += 12;
    graphics_set_no_texture();
    graphics_draw_quad(6, y, 88, 12, 0, 0, 0, 0, 0xFF666666);
    y += 2;
    sprintf(str, "%g", g_game_state.score);
    graphics_draw_text_center(font_big, str, 2.0f + (DRAW_LEFT_INFO_WIDTH / 2.0f), y + 4, 1.0f, COLOR_WHITE);


    y += 22;

    graphics_set_no_texture();
    graphics_draw_quad(4, y - 2, DRAW_LEFT_INFO_WIDTH - 4, 36, 0, 0, 0, 0, COLOR_DARK_GREY_2);
    // Draw poker hand
    graphics_draw_text_center(font_small, g_poker_hand_names[g_game_state.current_poker_hand], DRAW_LEFT_INFO_WIDTH / 2.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    // Draw poker hand level
    y += 10;
    if (g_game_state.current_poker_hand != GAME_POKER_HAND_NONE)
    {
        sprintf(str, "Level %d", g_game_state.poker_hand_level[g_game_state.current_poker_hand]);
        graphics_draw_text_center(font_small, str, DRAW_LEFT_INFO_WIDTH / 2.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    }

    y += 10;
    graphics_set_no_texture();
    if (g_game_state.stage == GAME_STAGE_INGAME && g_game_state.sub_stage == GAME_SUBSTAGE_INGAME_PICK_HAND &&
        g_game_state.current_base_chips *  g_game_state.current_base_mult >= game_get_current_blind_score())
    {
        graphics_draw_quad(6 + (rand()%5 - 2), y + (rand()%5 - 2), 38, 12, 0, 0, 0, 0, COLOR_LIGHT_BLUE);
        graphics_draw_quad(56 + (rand()%5 - 2), y + (rand()%5 - 2), 38, 12, 0, 0, 0, 0, COLOR_LIGHT_RED);
    }
    else
    {
        graphics_draw_quad(6, y, 38, 12, 0, 0, 0, 0, COLOR_LIGHT_BLUE);
        graphics_draw_quad(56, y, 38, 12, 0, 0, 0, 0, COLOR_LIGHT_RED);
    }    

    y += 2;
    sprintf(str, "%ld", g_game_state.current_base_chips);
    graphics_draw_text_center(font_small, str, DRAW_LEFT_INFO_WIDTH / 4.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "x");
    graphics_draw_text(font_small, str, 47, y, 1.0f, 0xFF8888FF);
    sprintf(str, "%ld", g_game_state.current_base_mult);
    graphics_draw_text_center(font_small, str, 2.0f + 3.0f * (DRAW_LEFT_INFO_WIDTH / 4.0f), y + 4.0f, 1.0f, COLOR_WHITE);

    y += 22;
    graphics_set_no_texture();
    graphics_draw_quad(4, y - 4, DRAW_LEFT_INFO_WIDTH - 4, 32, 0, 0, 0, 0, COLOR_DARK_GREY_2);    

    graphics_draw_text(font_small, "Hands", 10, y, 1.0f, COLOR_WHITE);
    graphics_draw_text(font_small, "Discards", 48, y, 1.0f, COLOR_WHITE);
    y += 15;
    graphics_set_no_texture();
    graphics_draw_quad(8, y - 2, (DRAW_LEFT_INFO_WIDTH / 2.0f) - 8, 11, 0, 0, 0, 0, COLOR_DARK_GREY);
    graphics_draw_quad((DRAW_LEFT_INFO_WIDTH / 2.0f) + 4, y - 2, (DRAW_LEFT_INFO_WIDTH / 2.0f) - 8, 11, 0, 0, 0, 0, COLOR_DARK_GREY);

    sprintf(str, "%d", g_game_state.current_hands);
    graphics_draw_text_center(font_small, str, 2.0f + DRAW_LEFT_INFO_WIDTH / 4.0f, y + 4, 1.0f, COLOR_TEXT_BLUE);
    sprintf(str, "%d", g_game_state.current_discards);
    graphics_draw_text_center(font_small, str, 3.0f * (DRAW_LEFT_INFO_WIDTH / 4.0f), y + 4, 1.0f, COLOR_LIGHT_RED);

    y += 21;
    graphics_set_no_texture();
    graphics_draw_quad(4, y - 6, DRAW_LEFT_INFO_WIDTH - 4, 19, 0, 0, 0, 0, COLOR_DARK_GREY_2);
    graphics_draw_quad(8, y - 2, DRAW_LEFT_INFO_WIDTH - 12, 11, 0, 0, 0, 0, COLOR_DARK_GREY);

    sprintf(str, "$%d", g_game_state.wealth);
    graphics_draw_text_center(font_big, str, DRAW_LEFT_INFO_WIDTH / 2, y + 4.0f, 1.0f, COLOR_TEXT_YELLOW);

    y += 20;
    graphics_set_no_texture();
    graphics_draw_quad(4, y - 4, DRAW_LEFT_INFO_WIDTH - 4, 32, 0, 0, 0, 0, COLOR_DARK_GREY_2);    

    graphics_draw_text_center(font_small, "Ante", 2.0f + DRAW_LEFT_INFO_WIDTH / 4.0f, y + 4, 1.0f, COLOR_WHITE);
    graphics_draw_text_center(font_small, "Round", 3.0f * (DRAW_LEFT_INFO_WIDTH / 4.0f), y + 4, 1.0f, COLOR_WHITE);
    y += 15;
    graphics_set_no_texture();
    graphics_draw_quad(8, y - 2, (DRAW_LEFT_INFO_WIDTH / 2.0f) - 8, 11, 0, 0, 0, 0, COLOR_DARK_GREY);
    graphics_draw_quad((DRAW_LEFT_INFO_WIDTH / 2.0f) + 4, y - 2, (DRAW_LEFT_INFO_WIDTH / 2.0f) - 8, 11, 0, 0, 0, 0, COLOR_DARK_GREY);

    sprintf(str, "#5%d#-/8", g_game_state.ante);
    graphics_draw_text_formatted_center(font_small, str, NULL, 2.0f + DRAW_LEFT_INFO_WIDTH / 4.0f, y + 4, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.round);
    graphics_draw_text_center(font_small, str, 3.0f * (DRAW_LEFT_INFO_WIDTH / 4.0f), y + 4, 1.0f, COLOR_TEXT_ORANGE);
    
    y += 20;
    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_RUN_INFO)
    {
        graphics_draw_solid_quad(4, y - 2, DRAW_LEFT_INFO_WIDTH - 4, 24.0f, COLOR_WHITE);        
    }
    graphics_draw_solid_quad(6, y, DRAW_LEFT_INFO_WIDTH - 8, 20.0f, COLOR_LIGHT_RED);
    graphics_draw_text_center(font_small, "Run Info", (DRAW_LEFT_INFO_WIDTH / 2.0f) + 4, y + 10, 1.0f, COLOR_WHITE);
}

void game_draw_cash_out_panel()
{
    char str[32];

    int x = 140;
    float y = g_game_state.cash_out_panel_y;

    graphics_set_no_texture();
    graphics_draw_quad(x, y, 250, SCREEN_HEIGHT - 100, 0, 0, 0, 0, COLOR_DARK_GREY);
    y += 16;
    if (g_game_state.cash_out_done)
    {        
        sprintf(str, "Cash Out: $%d", g_game_state.cash_out_value);
        float str_size = (float)strlen(str) * 8.0f;
        graphics_draw_solid_quad((140.0f + 125.0f) - (str_size / 2.0f) - 6.0f, y - 10.0f, str_size + 12.0f, 20.0f, COLOR_WHITE);
        graphics_draw_solid_quad((140.0f + 125.0f) - (str_size / 2.0f) - 4.0f, y - 8.0f, str_size + 8.0f, 16.0f, COLOR_LIGHT_RED);
        graphics_draw_text_center(font_big, str, (140.0f + 125.0f), y, 1.0f, COLOR_WHITE);
    }
    y += 20;
    if (g_game_state.cash_out_blind > -1)
    {
        graphics_draw_text(font_small, game_util_get_blind_name(g_game_state.blind), x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_blind);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
    y += 16;
    if (g_game_state.cash_out_hands > -1)
    {
        graphics_draw_text(font_small, "Remaining hands", x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_hands);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
    y += 16;
    if (g_game_state.cash_out_interest > -1)
    {
        graphics_draw_text(font_small, "Interest", x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_interest);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
    y += 16;
    if (g_game_state.cash_out_jokers > -1)
    {
        graphics_draw_text(font_small, "Jokers", x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_jokers);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
}

void game_draw_booster_items()
{
    graphics_set_no_texture();
    graphics_draw_quad(DRAW_BOOSTER_ITEMS_X, DRAW_BOOSTER_ITEMS_Y, DRAW_BOOSTER_ITEMS_WIDTH, CARD_HEIGHT, 0, 0, 0, 0, 0x66000000);

    for (int i = 0; i < g_game_state.shop.booster_total_items; i++)
    {
        if (g_game_state.shop.booster_items[i].available)
        {
            switch (g_game_state.shop.booster_items[i].type)
            {
                case ITEM_TYPE_CARD:
                    game_draw_card(&(g_game_state.shop.booster_items[i].info.card), NULL);
                    break;
                case ITEM_TYPE_JOKER:
                    game_draw_joker(&(g_game_state.shop.booster_items[i].info.joker));
                    break;
                case ITEM_TYPE_PLANET:
                    game_draw_planet(&(g_game_state.shop.booster_items[i].info.planet));
                    break;
                case ITEM_TYPE_TAROT:
                    game_draw_tarot(&(g_game_state.shop.booster_items[i].info.tarot));
                    break;                    
            }
        }
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_BOOSTER_ITEMS)
    {
        if (g_game_state.highlighted_item > -1 && g_game_state.highlighted_item < g_game_state.shop.booster_total_items)
        {
            struct DrawObject *draw = &(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.draw);
            game_draw_card_frame(draw);
            game_draw_item_button(draw, GAMEPAD_UI_RIGHT_TRIGGER, "Select", NULL, COLOR_RED_BUTTON, false);
            switch(g_game_state.shop.booster_items[g_game_state.highlighted_item].type)
            {
                case ITEM_TYPE_CARD:
                    game_draw_card_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card));
                    break;
                case ITEM_TYPE_JOKER:
                    game_draw_joker_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.joker));
                    break;
                case ITEM_TYPE_PLANET:
                    game_draw_planet_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.planet));
                    break;
                case ITEM_TYPE_TAROT:
                    game_draw_tarot_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.tarot));
                    break;
            }
        }
    }
}

void game_draw_shop()
{
    game_draw_left_info();    

    game_draw_deck();

    switch (g_game_state.sub_stage)
    {
        case GAME_SUBSTAGE_SHOP_MAIN:
        {
            float x = 106.0f;
            float y = 90.0f;

            graphics_draw_solid_quad(x, y, 300.0f, SCREEN_HEIGHT - y - 2.0f, COLOR_DARK_GREY_2);

            // Buttons
            if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_NEXT_ROUND)
            {
                graphics_draw_solid_quad(x + 2.0f, y + 2.0f, 84.0f, 34.0f, COLOR_WHITE);
            }
            graphics_draw_solid_quad(x + 4.0f, y + 4.0f, 80.0f, 30.0f, COLOR_RED_BUTTON);
            graphics_draw_text_center(font_small, "Next Round", x + 4.0f + 40.0f, y + 4.0f + 15.0f, 1.0f, COLOR_WHITE);

            if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_REROLL)
            {
                graphics_draw_solid_quad(x + 2.0f, y + 36.0f, 84.0f, 34.0f, COLOR_WHITE);
            }
            graphics_draw_solid_quad(x + 4.0f, y + 38.0f, 80.0f, 30.0f, COLOR_GREEN_BUTTON);
            graphics_draw_text_center(font_small, "Reroll", x + 4.0f + 40.0f, y + 8.0f + 30.0f + 10.0f, 1.0f, COLOR_WHITE);
            char str[16];
            sprintf(str, "$%d", game_util_get_reroll_cost());
            graphics_draw_text_center(font_small, str, x + 4.0f + 40.0f, y + 8.0f + 30.0f + 15.0f + 8.0f, 1.0f, COLOR_WHITE);
            
            // Singles
            graphics_draw_solid_quad(x + 92.0f, y + 4.0f, 200.0f, 80.0f, COLOR_DARK_GREY);
            game_draw_shop_singles();

            // Boosters
            graphics_draw_solid_quad(x + 122.0f, y + 92.0f, 170.0f, 80.0f, COLOR_DARK_GREY);
            game_draw_shop_boosters();

            game_draw_shop_selected_single();
            
            game_draw_shop_selected_booster();

            break;
        }
        case GAME_SUBSTAGE_SHOP_BOOSTER_HAND:
        {
            game_draw_booster_hand();
        }
        case GAME_SUBSTAGE_SHOP_BOOSTER_NO_HAND:
        {
            graphics_draw_solid_quad(DRAW_BOOSTER_ITEMS_X + (DRAW_BOOSTER_ITEMS_WIDTH / 2.0f) - 50.0f, DRAW_BOOSTER_ITEMS_Y + CARD_HEIGHT, 100.0f, 30.0f, COLOR_DARK_GREY_2);            
            graphics_draw_text_center(font_big, g_game_state.shop.opened_booster.size == BOOSTER_PACK_SIZE_MEGA ? "Choose 2" : "Choose 1",
                DRAW_BOOSTER_ITEMS_X + (DRAW_BOOSTER_ITEMS_WIDTH / 2.0f), DRAW_BOOSTER_ITEMS_Y + CARD_HEIGHT + 15.0f, 1.0f, COLOR_WHITE);

            game_draw_booster_items();
            
            break;
        }
    }

    game_draw_top_jokers();

    game_draw_consumables();

    switch (g_game_state.sub_stage)
    {
        case GAME_SUBSTAGE_SHOP_BOOSTER_HAND:
        {
            game_draw_booster_hand_selected_card();
            break;
        }
    }
}

void game_draw_score_number()
{
    if (g_game_state.score_number.show_score_number)
    {
        float x = 0.0f, y = 0.0f;
        if (g_game_state.score_number.card != NULL)
        {
            x = g_game_state.score_number.card->draw.x + CARD_WIDTH / 2.0f;
            y = g_game_state.score_number.card->draw.y - 16.0f;
        }
        else if (g_game_state.score_number.joker != NULL)
        {
            x = g_game_state.score_number.joker->draw.x + CARD_WIDTH / 2.0f;
            y = g_game_state.score_number.joker->draw.y + CARD_HEIGHT + 10.0f;
        }

        graphics_set_no_texture();
        float quad_size = 16.0f * g_game_state.score_number.back_quad_scale;

        switch (g_game_state.score_number.score_number_type)
        {
            case SCORE_NUMBER_ADD_CHIPS:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_BLUE, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale, COLOR_WHITE);
                break;
            case SCORE_NUMBER_ADD_MULT:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_RED, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale * 0.8, COLOR_WHITE);
                break;
            case SCORE_NUMBER_MULT_MULT:
            case SCORE_NUMBER_AGAIN:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_RED, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale * 0.8, COLOR_WHITE);
                break;
            case SCORE_NUMBER_ADD_MONEY:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_YELLOW, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale, COLOR_WHITE);
            default:
                break;
        }
    }
}

#define END_GAME_WIDTH 240
#define END_GAME_HEIGHT 220

void game_draw_end_game_info(const char *title, int color)
{
    char str[32];

    float x = (SCREEN_WIDTH - END_GAME_WIDTH) / 2.0f;
    float y = (SCREEN_HEIGHT - END_GAME_HEIGHT) / 2.0f;
    graphics_draw_solid_quad(x, y, END_GAME_WIDTH, END_GAME_HEIGHT, COLOR_DARK_GREY);

    y += 12.0f;
    graphics_draw_text_center(font_big, title, SCREEN_WIDTH / 2.0f, y, 1.0f, color);

    y += 20.0f;
    graphics_draw_solid_quad(x + 6.0f,  y - 6.0f, (END_GAME_WIDTH / 2.0f) - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + 70.0f, y - 2.0f, (END_GAME_WIDTH / 4.0f) - 24.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Ante", x + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.ante);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 4.0f) + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    graphics_draw_solid_quad((END_GAME_WIDTH / 2.0f) + x + 6.0f, y - 6.0f, (END_GAME_WIDTH / 2.0f) - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad((END_GAME_WIDTH / 2.0f) + x + 70.0f, y - 2.0f, (END_GAME_WIDTH / 4.0f) - 24.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Round", (END_GAME_WIDTH / 2.0f) + x + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.round);
    graphics_draw_text_center(font_small, str, (END_GAME_WIDTH / 2.0f) + x + (END_GAME_WIDTH / 4.0f) + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Best Hand", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%g", g_game_state.stats.max_score);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Most Played Hand", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    graphics_draw_text_center(font_small, g_poker_hand_names[g_game_state.stats.most_played_poker_hand], x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Cards Played", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.cards_played);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Cards Discarded", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.cards_discarded);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Cards Purchased", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.cards_purchased);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Times Rerolled", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.times_rerolled);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);    
    y += 30.0f;
    graphics_draw_solid_quad(x + (END_GAME_WIDTH - 100.0f) / 2.0f - 2.0f, y - 6.0f - 2.0f, 100.0f + 4.0f, 24.0f, COLOR_WHITE);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH - 100.0f) / 2.0f, y - 6.0f, 100.0f, 20.0f, COLOR_RED_BUTTON);
    graphics_draw_text_center(font_small, "Start New Game", x + (END_GAME_WIDTH / 2.0f), y + 4.0f, 1.0f, COLOR_WHITE);
}

void game_draw_you_win_panel()
{
    game_draw_end_game_info("YOU WIN", COLOR_LIGHT_BLUE);
}

void game_draw_game_over_panel()
{
    game_draw_end_game_info("GAME OVER", COLOR_LIGHT_RED);
}

void game_draw_ingame()
{
    game_draw_left_info();

    game_draw_deck();

    game_draw_top_jokers();

    game_draw_consumables();

    switch (g_game_state.sub_stage)
    {
        case GAME_SUBSTAGE_INGAME_PICK_HAND:
        {
            game_draw_ingame_hand();            
            break;
        }
        case GAME_SUBSTAGE_INGAME_WON:
        {
            game_draw_cash_out_panel();
            break;
        }
        case GAME_SUBSTAGE_INGAME_WON_END:
        {
            game_draw_you_win_panel();
            break;
        }
        case GAME_SUBSTAGE_INGAME_LOST_END:
        {
            game_draw_game_over_panel();
            break;
        }
    }

    game_draw_score_number();
}

void game_draw_blind_select()
{
    char str[24];

    game_draw_left_info();    

    game_draw_deck();

    int x = 106;
    for(int i = 0; i < 3; i++)
    {
        graphics_set_no_texture();

        int y = 90;

        if (i == g_game_state.blind && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_BLIND)
        {
            graphics_draw_quad(x - 2, y - 2, 94, SCREEN_HEIGHT - 88, 0, 0, 0, 0, COLOR_WHITE);
        }
        
        graphics_draw_quad(x, y, 90, SCREEN_HEIGHT - 90, 0, 0, 0, 0, COLOR_DARK_GREY);

        y += 10;
        graphics_draw_text(font_small, game_util_get_blind_name(i), x + 2, y, 1.0f, COLOR_WHITE);

        y += 12;
        graphics_draw_text(font_small, "Score at least", x + 2, y, 1.0f, COLOR_WHITE);

        y += 10;
        sprintf(str, "%g", game_get_ante_base_score() * (1.0 + ((double)i * 0.5)));
        graphics_draw_text(font_small, str, x + 2, y, 1.0f, COLOR_LIGHT_RED);

        if (i != g_game_state.blind)
        {
            graphics_set_no_texture();
            graphics_draw_quad(x, 90, 90, SCREEN_HEIGHT - 90, 0, 0, 0, 0, 0xAA666666);
        }

        x += 108;
    }

    game_draw_top_jokers();

    game_draw_consumables();

    game_draw_score_number();
}

int inner_game_draw_offscreen_deck_info_rank_func(const void *a, const void *b)
{
    struct Card *card_a = *((struct Card **)a);
    struct Card *card_b = *((struct Card **)b);

    if (card_a == NULL) return 1;
    if (card_b == NULL) return -1;

    int rank_a = card_a->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_a->rank;
    int rank_b = card_b->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_b->rank;

    return rank_b - rank_a;
}

void game_draw_offscreen_deck_info()
{
    graphics_set_offscreen_render_target();

    graphics_draw_solid_quad(0, 0, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(2, 2, DRAW_DECK_INFO_WIDTH - 4.0f, DRAW_DECK_INFO_HEIGHT - 4.0f, COLOR_DARK_GREY);

    for (int i = 0; i < 4; i++)
    {
        g_game_state.deck_info.card_count[i] = 0;
    }

    for (int j = 0; j < CARD_RANK_COUNT; j++) { g_game_state.deck_info.rank_count[j] = 0; g_game_state.deck_info.effective_rank_count[j] = 0; }
    for (int j = 0; j < 4; j++) { g_game_state.deck_info.suit_count[j] = 0; g_game_state.deck_info.effective_suit_count[j] = 0; }

    g_game_state.deck_info.face_count = 0;
    g_game_state.deck_info.effective_face_count = 0;
    g_game_state.deck_info.number_count = 0;
    g_game_state.deck_info.effective_number_count = 0;

    for (int i = 0; i < (g_game_state.deck_info.partial ? g_game_state.current_deck.card_count : g_game_state.full_deck.card_count); i++)
    {
        struct Card *card = g_game_state.full_deck.cards[i];
        if (g_game_state.deck_info.partial)
        {
            card = g_game_state.current_deck.cards[i];
        }

        for (int j = 0; j < CARD_RANK_COUNT; j++)
        {
            if (card->rank == j)
            {
                g_game_state.deck_info.rank_count[j]++;
            }

            if (game_util_is_card_rank(card, j))
            {
                g_game_state.deck_info.effective_rank_count[j]++;
            }
        }

        for (int j = 0; j < 4; j++)
        {
            if (card->suit == j)
            {
                g_game_state.deck_info.suit_count[j]++;
            }

            if (game_util_is_card_suit(card, j))
            {
                g_game_state.deck_info.effective_suit_count[j]++;
            }
        }

        if (card->rank == CARD_RANK_JACK || card->rank == CARD_RANK_QUEEN || card->rank == CARD_RANK_KING)
        {
            g_game_state.deck_info.face_count++;
        }
        else
        {
            g_game_state.deck_info.number_count++;
        }

        if (game_util_is_card_face(card))
        {
            g_game_state.deck_info.effective_face_count++;
        }
        else
        {
            g_game_state.deck_info.effective_number_count++;
        }
    }

    for (int i = 0; i < g_game_state.full_deck.card_count; i++)
    {
        struct Card *card = g_game_state.full_deck.cards[i];

        g_game_state.deck_info.cards[card->suit][g_game_state.deck_info.card_count[card->suit]] = card;
        g_game_state.deck_info.card_count[card->suit]++;
    }

    for (int i = 0; i < 4; i++)
    {
        qsort((void*)(g_game_state.deck_info.cards[i]), g_game_state.deck_info.card_count[i], sizeof(struct Card *), inner_game_draw_offscreen_deck_info_rank_func);
    }

    bool in_current_deck = false;

    g_freeze_cards = true;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < g_game_state.deck_info.card_count[i]; j++)
        {
            struct Card *card = g_game_state.deck_info.cards[i][j];
            struct DrawObject *draw = &(g_game_state.deck_info.draw[i][j]);
            game_init_draw_object(draw);
            draw->x = game_util_get_item_position(j, g_game_state.deck_info.card_count[i], 90.0f, DRAW_DECK_INFO_WIDTH - 96.0f, CARD_WIDTH);
            draw->y = 6.0f + 50.0f * i;

            game_draw_card(card, draw);

            in_current_deck = false;
            for(int w = 0; w < g_game_state.current_deck.card_count; w++)
            {
                if (g_game_state.current_deck.cards[w] == card)
                {
                    in_current_deck = true;
                    break;
                }
            }

            g_game_state.deck_info.in_current_deck[i][j] = in_current_deck;

            if (g_game_state.deck_info.partial && !in_current_deck)
            {
                graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
                graphics_draw_quad(draw->x, draw->y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xC0FFFFFF & COLOR_DARK_GREY);
            }

            draw->x += (SCREEN_WIDTH - DRAW_DECK_INFO_WIDTH) / 2.0f;
            draw->y += (SCREEN_HEIGHT - DRAW_DECK_INFO_HEIGHT) / 2.0f;
        }
    }    

    graphics_unset_offscreen_render_target();
}

// void game_draw_deck_draw_cards()
// {
//     float left = (SCREEN_WIDTH - DRAW_DECK_INFO_WIDTH) / 2.0f;
//     float top = (SCREEN_HEIGHT - DRAW_DECK_INFO_HEIGHT) / 2.0f;

//     graphics_draw_solid_quad(left + 0, top + 0, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, COLOR_DARK_GREY_2);
//     graphics_draw_solid_quad(left + 2, top + 2, DRAW_DECK_INFO_WIDTH - 4.0f, DRAW_DECK_INFO_HEIGHT - 4.0f, COLOR_DARK_GREY);

//     for (int i = 0; i < 4; i++)
//     {
//         for (int j = 0; j < g_game_state.deck_info.card_count[i]; j++)
//         {
//             struct Card *card = g_game_state.deck_info.cards[i][j];
//             struct DrawObject *draw = &(g_game_state.deck_info.draw[i][j]);

//             game_draw_card(card, draw);

//             if (g_game_state.deck_info.partial && !g_game_state.deck_info.in_current_deck[i][j])
//             {
//                 graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
//                 graphics_draw_quad(draw->x, draw->y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xC0FFFFFF & COLOR_DARK_GREY);
//             }
//         }
//     }
// }

void game_draw_deck_info()
{
    char str[16];
    float left = (SCREEN_WIDTH - DRAW_DECK_INFO_WIDTH) / 2.0f;
    float top = (SCREEN_HEIGHT - DRAW_DECK_INFO_HEIGHT) / 2.0f;

    graphics_set_texture(-2, 0);
    graphics_draw_quad(left, top, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, 0, 0, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, COLOR_WHITE);

    graphics_draw_solid_quad(left + 4.0f, top + 6.0f, 80.0f, 14.0f, COLOR_DARK_GREY_2);
    if (g_game_state.deck_info.partial)
    {
        graphics_draw_text_center(font_small, "Remaining", left + 35.0f, top + 7.0f + 6.0f, 1.0f, COLOR_WHITE);
    }
    else
    {
        graphics_draw_text_center(font_small, "Full", left + 35.0f, top + 7.0f + 6.0f, 1.0f, COLOR_WHITE);
    }
    graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(left + 4.0f + 60.0f, top + 5.0f, 18.0f, 18.0f, g_gamepad_ui_text_coords[GAMEPAD_UI_LEFT_TRIGGER].x, g_gamepad_ui_text_coords[GAMEPAD_UI_LEFT_TRIGGER].y, 32, 32, COLOR_WHITE);

    graphics_draw_solid_quad(left + 4.0f, top + 4.0f + 20.0f, 80.0f, 14.0f, COLOR_DARK_GREY_2);
    if (g_game_state.deck_info.effective)
    {
        graphics_draw_text_center(font_small, "Effective", left + 35.0f, top + 20.0f + 4.0f + 7.0f, 1.0f, COLOR_LIGHT_BLUE);
    }
    else
    {
        graphics_draw_text_center(font_small, "Base", left + 35.0f, top + 20.0f + 4.0f + 7.0f, 1.0f, COLOR_WHITE);
    }
    graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(left + 4.0f + 60.0f, top + 5.0f + 18.0f, 18.0f, 18.0f, g_gamepad_ui_text_coords[GAMEPAD_UI_RIGHT_TRIGGER].x, g_gamepad_ui_text_coords[GAMEPAD_UI_RIGHT_TRIGGER].y, 32, 32, COLOR_WHITE);

    float suit_left = 60.0f;
    float suit_top = 48.0f;

    int c = 0;
    for (int i = CARD_RANK_COUNT - 1; i >= 0; i--, c++)
    {
        graphics_draw_solid_quad(left + suit_left - 7.0f, top + suit_top - 6.0f + c * 14.0f, 14.0f, 12.0f, COLOR_DARK_GREY_2);
        graphics_draw_text_center(font_small, g_short_rank_name[i], left + suit_left, top + suit_top + c * 14.0f, 1.0f, COLOR_LIGHT_GREY);
        if (g_game_state.deck_info.effective)
        {
            sprintf(str, "%d", g_game_state.deck_info.effective_rank_count[i]);
        }
        else
        {
            sprintf(str, "%d", g_game_state.deck_info.rank_count[i]);
        }        
        graphics_draw_text_center(font_small, str, left + suit_left + 18.0f, top + suit_top + c * 14.0f, 1.0f, g_game_state.deck_info.effective ? COLOR_LIGHT_BLUE : COLOR_WHITE);
    }

    float other_left = 8.0f;
    float other_top = 54.0f;

    for (int i = 0; i < UI_ASSETS_COUNT - 2; i++)
    {
        graphics_draw_solid_quad(left + other_left - 2.0f, top + other_top - 2.0f + i * 28.0f, 22.0f, 22.0f, COLOR_LIGHT_GREY_2);
        graphics_set_texture(tex_ui_assets, GRAPHICS_TEXTURE_FILTER_NEAREST);

        int asset = i+2;
        if (asset == 5) { asset = 6; }
        else if (asset == 6) { asset = 5; }
        graphics_draw_quad(left + other_left, top + other_top + i * 28.0f, 18, 18, g_ui_assets_tex_coords[asset].x, g_ui_assets_tex_coords[asset].y, 18, 18, COLOR_WHITE);
        if (g_game_state.deck_info.effective)
        {
            if (i == 0) { sprintf(str, "%d", g_game_state.deck_info.effective_face_count); }
            else if (i == 1) { sprintf(str, "%d", g_game_state.deck_info.effective_number_count); }
            else { sprintf(str, "%d", g_game_state.deck_info.effective_suit_count[i - 2]); }
        }
        else
        {
            if (i == 0) { sprintf(str, "%d", g_game_state.deck_info.face_count); }
            else if (i == 1) { sprintf(str, "%d", g_game_state.deck_info.number_count); }
            else { sprintf(str, "%d", g_game_state.deck_info.suit_count[i - 2]); }
        }
        graphics_draw_text_center(font_small, str, left + other_left + 32.0f, top + other_top + i * 28.0f + 9.0f, 1.0f, g_game_state.deck_info.effective ? COLOR_LIGHT_BLUE : COLOR_WHITE);
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_DECK_INFO_CARD)
    {
        g_freeze_cards = true;
        game_draw_card(&g_game_state.deck_info.current_card, NULL);
        if (g_game_state.deck_info.partial && !g_game_state.deck_info.in_current_deck[g_game_state.deck_info.current_card_y][g_game_state.deck_info.current_card_x])
        {
            graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
            graphics_draw_quad(g_game_state.deck_info.current_card.draw.x, g_game_state.deck_info.current_card.draw.y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xC0FFFFFF & COLOR_DARK_GREY);
        }
        game_draw_card_frame(&(g_game_state.deck_info.current_card.draw));
        game_draw_card_hint(&g_game_state.deck_info.current_card);
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_DECK_INFO_CLOSE)
    {
        graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f - 2.0f, 237.0f - 2.0f, 50.0f + 4.0f, 15.0f + 4.0f, COLOR_WHITE);
    }
    graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f, 237.0f, 50.0f, 15.0f, COLOR_RED_BUTTON);
    graphics_draw_text_center(font_small, "Close", (SCREEN_WIDTH) / 2.0f, 244.0f, 1.0f, COLOR_WHITE);

}

void game_draw_run_info()
{
    float left = (SCREEN_WIDTH - DRAW_RUN_INFO_WIDTH) / 2.0f;
    float top = (SCREEN_HEIGHT - DRAW_RUN_INFO_HEIGHT) / 2.0f;

    graphics_draw_solid_quad(left, top, DRAW_RUN_INFO_WIDTH, DRAW_RUN_INFO_HEIGHT, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(left + 2, top + 2, DRAW_RUN_INFO_WIDTH - 4.0f, DRAW_RUN_INFO_HEIGHT - 4.0f, COLOR_DARK_GREY);

    graphics_draw_text_center(font_big, "Poker Hands", SCREEN_WIDTH / 2.0f, top + 12.0f, 1.0f, COLOR_WHITE);

    float y = 8.0f;
    char str[16];
    top += 20.0f;
    for (int i = 1; i < GAME_POKER_HAND_COUNT; i++)
    {
        graphics_draw_solid_quad(left + 4.0f, top + y - 2.0f, 304.0f, 12.0f, COLOR_LIGHT_GREY);
        
        graphics_draw_solid_quad(left + 179.0f, top + y - 2.0f, 92.0f, 12.0f, COLOR_DARK_GREY_2);
        graphics_draw_solid_quad(left + 180.0f, top + y - 1.0f, 40.0f, 10.0f, COLOR_LIGHT_BLUE);
        graphics_draw_solid_quad(left + 230.0f, top + y - 1.0f, 40.0f, 10.0f, COLOR_LIGHT_RED);

        graphics_draw_solid_quad(left + 285.0f, top + y - 1.0f, 20.0f, 10.0f, COLOR_DARK_GREY_2);

        graphics_draw_text_center(font_small, "Lvl.", left + 20.0f, top + y + 4.0f, 1.0f, COLOR_WHITE);

        sprintf(str, "%d", g_game_state.poker_hand_level[i]);
        graphics_draw_text_center(font_small, str, left + 40.0f, top + y + 4.0f, 1.0f, COLOR_WHITE);

        graphics_draw_text_center(font_small, g_poker_hand_names[i], left + 110.0f, top + y + 4.0f, 1.0f, COLOR_WHITE);

        sprintf(str, "%d", g_game_state.poker_hand_base_chips[i]);
        graphics_draw_text_center(font_small, str, left + 200.0f, top + y + 4.0f, 1.0f, COLOR_WHITE);

        graphics_draw_text_center(font_small, "x", left + 226.0f, top + y + 4.0f, 1.0f, COLOR_LIGHT_RED);

        sprintf(str, "%d", g_game_state.poker_hand_base_mult[i]);
        graphics_draw_text_center(font_small, str, left + 250.0f, top + y + 4.0f, 1.0f, COLOR_WHITE);

        graphics_draw_text_center(font_small, "#", left + 280.0f, top + y + 4.0f, 1.0f, COLOR_WHITE);
        sprintf(str, "%d", g_game_state.stats.poker_hands_played[i]);
        graphics_draw_text_center(font_small, str, left + 295.0f, top + y + 4.0f, 1.0f, COLOR_TEXT_ORANGE);

        y += 17.0f;
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_RUN_INFO_CLOSE)
    {
        graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f - 2.0f, 240.0f - 2.0f, 50.0f + 4.0f, 15.0f + 4.0f, COLOR_WHITE);
    }
    graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f, 240.0f, 50.0f, 15.0f, COLOR_RED_BUTTON);
    graphics_draw_text_center(font_small, "Close", (SCREEN_WIDTH) / 2.0f, 247.0f, 1.0f, COLOR_WHITE);
}

uint64_t last_tick = 0;
uint32_t tick_res = 0;
float curr_ms = 1.0f;
float curr_fps = 0.0f;
int g_frame_count;

void game_draw_debug_info()
{
    g_time++;

    if (last_tick == 0) { sceRtcGetCurrentTick(&last_tick); }
    if (tick_res == 0) { tick_res = sceRtcGetTickResolution(); }

    g_frame_count++;

    uint64_t curr_tick;
	sceRtcGetCurrentTick(&curr_tick);

    if ((curr_tick-last_tick) >= tick_res)
    {
        curr_fps = 1.0f / curr_ms;

        double time_span = ((curr_tick-last_tick)) / (float)tick_res;
        curr_ms = time_span / g_frame_count;
        g_frame_count = 0;
        sceRtcGetCurrentTick(&last_tick);
    }

    #ifdef DEBUG
    char str[32];
    sprintf(str, "FPS: %0.2f", curr_fps);
    graphics_draw_text(font_small, str, 400, 0, 1.0f, COLOR_WHITE);
    if (g_settings.audio)
    {
        sprintf(str, "audio w read: %d", g_debug_info.audio_wait_read);
        graphics_draw_text(font_small, str, 370, 8, 1.0f, COLOR_WHITE);
        sprintf(str, "audio w write: %d", g_debug_info.audio_wait_write);
        graphics_draw_text(font_small, str, 364, 16, 1.0f, COLOR_WHITE);
    }
    #endif
}

void game_draw_pause_menu()
{
    // Draw a dark overlay
    graphics_draw_solid_quad(0, 0, 480, 272, 0x88000000);

    // Draw the dialog box
    float box_w = 200.0f;
    float box_h = 130.0f;
    float box_x = (480.0f - box_w) / 2.0f;
    float box_y = (272.0f - box_h) / 2.0f;

    graphics_draw_solid_quad(box_x, box_y, box_w, box_h, COLOR_BLACK);
    
    // Draw the border
    graphics_draw_solid_quad(box_x - 2.0f, box_y - 2.0f, box_w + 4.0f, 2.0f, COLOR_WHITE);
    graphics_draw_solid_quad(box_x - 2.0f, box_y + box_h, box_w + 4.0f, 2.0f, COLOR_WHITE);
    graphics_draw_solid_quad(box_x - 2.0f, box_y, 2.0f, box_h, COLOR_WHITE);
    graphics_draw_solid_quad(box_x + box_w, box_y, 2.0f, box_h, COLOR_WHITE);

    graphics_draw_text_center(font_big, "PAUSED", 240.0f, box_y + 10.0f, 1.5f, COLOR_WHITE);

    // Menu options
    uint32_t color_continue = (g_game_state.highlighted_item == 0) ? COLOR_TEXT_YELLOW : COLOR_WHITE;
    uint32_t color_savequit = (g_game_state.highlighted_item == 1) ? COLOR_TEXT_YELLOW : COLOR_WHITE;
    uint32_t color_load = (g_game_state.highlighted_item == 2) ? COLOR_TEXT_YELLOW : COLOR_WHITE;

    graphics_draw_text_center(font_small, "Continue", 240.0f, box_y + 40.0f, 1.0f, color_continue);
    graphics_draw_text_center(font_small, "Save", 240.0f, box_y + 70.0f, 1.0f, color_savequit);
    graphics_draw_text_center(font_small, "Load", 240.0f, box_y + 100.0f, 1.0f, color_load);
}

void game_draw()
{
    graphics_begin_draw();

    graphics_clear(COLOR_BACKGROUND_2);

    int draw_stage = (g_game_state.stage == GAME_STAGE_PAUSE_MENU) ? g_game_state.previous_stage : g_game_state.stage;

    switch (draw_stage)
    {
        case GAME_STAGE_BLINDS:
            game_draw_blind_select();            
            break;
        case GAME_STAGE_INGAME:
            game_draw_ingame();
            break;
        case GAME_STAGE_SHOP:
        {
            game_draw_shop();
            break;
        }
    }

    if (g_game_state.sub_stage == GAME_SUBSTAGE_DECK_INFO)
    {
        game_draw_deck_info();
    }

    if (g_game_state.sub_stage == GAME_SUBSTAGE_RUN_INFO)
    {
        game_draw_run_info();
    }

    if (g_game_state.stage == GAME_STAGE_PAUSE_MENU)
    {
        game_draw_pause_menu();
    }

    game_draw_debug_info();

    graphics_end_draw();
}
