#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pspctrl.h>
#include <psppower.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psprtc.h>
#include <memory.h>
#include <malloc.h>

#ifndef M_PI
#define M_PI    3.141592f
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#ifdef DEBUG
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) ((void)0)
#endif

#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   272

#define BUFFER_WIDTH    512
#define BUFFER_HEIGHT   272

extern uint32_t g_game_counter;

struct Settings
{
    char archive_file_name[128];
    int hand_size;
    int hands;
    int discards;
    int wealth;
    int joker_slots;
    int consumable_slots;
    int shop_item_slots;
    int shop_booster_slots;
    bool audio;
    bool move_cards;
    bool overclock;
    int ante_score_scaling;
    int speed;
};

extern struct Settings g_settings;

#define CARD_RANK_2     0
#define CARD_RANK_3     1
#define CARD_RANK_4     2
#define CARD_RANK_5     3
#define CARD_RANK_6     4
#define CARD_RANK_7     5
#define CARD_RANK_8     6
#define CARD_RANK_9     7
#define CARD_RANK_10    8
#define CARD_RANK_JACK  9
#define CARD_RANK_QUEEN 10
#define CARD_RANK_KING  11
#define CARD_RANK_ACE   12

#define CARD_RANK_COUNT 13

extern const char *g_short_rank_name[CARD_RANK_COUNT];

#define CARD_SUIT_HEARTS    0
#define CARD_SUIT_CLUBS     1
#define CARD_SUIT_DIAMONDS  2
#define CARD_SUIT_SPADES    3

extern char *g_card_suits[4];

#define CARD_ENHANCEMENT_NONE   0
#define CARD_ENHANCEMENT_BONUS  1
#define CARD_ENHANCEMENT_MULT   2
#define CARD_ENHANCEMENT_WILD   3
#define CARD_ENHANCEMENT_GLASS  4
#define CARD_ENHANCEMENT_STEEL  5
#define CARD_ENHANCEMENT_STONE  6
#define CARD_ENHANCEMENT_GOLD   7
#define CARD_ENHANCEMENT_LUCKY  8
#define CARD_ENHANCEMENT_COUNT  9

extern const char *g_enhancement_names[CARD_ENHANCEMENT_COUNT];
extern const char *g_enhancement_hint[CARD_ENHANCEMENT_COUNT];

#define CARD_EDITION_BASE          0
#define CARD_EDITION_FOIL          1
#define CARD_EDITION_HOLOGRAPHIC   2
#define CARD_EDITION_POLYCHROME    3
#define CARD_EDITION_NEGATIVE      4
#define CARD_EDITION_COUNT         5

extern const char *g_edition_names[CARD_EDITION_COUNT];
extern const char *g_edition_hint[CARD_EDITION_COUNT];

#define CARD_SEAL_NONE      0
#define CARD_SEAL_GOLD      1
#define CARD_SEAL_RED       2
#define CARD_SEAL_BLUE      3
#define CARD_SEAL_PURPLE    4

#define CARD_SEAL_COUNT     5

struct DrawObject
{
    float x, y;
    float initial_x, initial_y;
    float final_x, final_y;

    float angle;
    float scale;
    float alpha;

    float white_factor;

    float r;
};

struct DebugInfo
{
    int audio_wait_read, audio_wait_write;
    bool force_score_flames;
};

extern struct DebugInfo g_debug_info;

struct Card
{
    struct DrawObject draw;

    uint8_t rank;
    uint8_t suit;

    uint16_t extra_chips;

    uint8_t enhancement, edition, seal;

    bool selected;    
};

#define MAX_CARDS  1024

struct Cards
{
    struct Card cards[MAX_CARDS];
    uint16_t    card_count;
};

struct CardIndex
{
    struct Card *card;
    int index;  
};

struct CardReferences
{
    struct Card *cards[MAX_CARDS];
    uint16_t    card_count;
};

#define JOKER_TYPE_JOKER                0
#define JOKER_TYPE_GREEDY_JOKER         1
#define JOKER_TYPE_LUSTY_JOKER          2
#define JOKER_TYPE_WRATHFUL_JOKER       3
#define JOKER_TYPE_GLUTTONOUS_JOKER     4
#define JOKER_TYPE_JOLLY_JOKER          5
#define JOKER_TYPE_ZANY_JOKER           6
#define JOKER_TYPE_MAD_JOKER            7
#define JOKER_TYPE_CRAZY_JOKER          8
#define JOKER_TYPE_DROLL_JOKER          9
#define JOKER_TYPE_SLY_JOKER            10
#define JOKER_TYPE_WILY_JOKER           11
#define JOKER_TYPE_CLEVER_JOKER         12
#define JOKER_TYPE_DEVIOUS_JOKER        13
#define JOKER_TYPE_CRAFTY_JOKER         14
#define JOKER_TYPE_HALF_JOKER           15
#define JOKER_TYPE_JOKER_STENCIL        16
#define JOKER_TYPE_FOUR_FINGERS         17
#define JOKER_TYPE_MIME                 18
#define JOKER_TYPE_CREDIT_CARD          19
#define JOKER_TYPE_CEREMONIAL_DAGGER    20
#define JOKER_TYPE_BANNER               21
#define JOKER_TYPE_MYSTIC_SUMMIT        22
#define JOKER_TYPE_MARBLE_JOKER         23
#define JOKER_TYPE_LOYALTY_CARD         24
#define JOKER_TYPE_8_BALL               25
#define JOKER_TYPE_MISPRINT             26
#define JOKER_TYPE_DUSK                 27
#define JOKER_TYPE_RAISED_FIST          28
#define JOKER_TYPE_CHAOS_THE_CLOWN      29
#define JOKER_TYPE_FIBONACCI            30
#define JOKER_TYPE_STEEL_JOKER          31
#define JOKER_TYPE_SCARY_FACE           32
#define JOKER_TYPE_ABSTRACT_JOKER       33
#define JOKER_TYPE_DELAYED_GRAT         34
#define JOKER_TYPE_HACK                 35
#define JOKER_TYPE_PAREIDOLIA           36
#define JOKER_TYPE_GROS_MICHEL          37
#define JOKER_TYPE_EVEN_STEVEN          38
#define JOKER_TYPE_ODD_TODD             39
#define JOKER_TYPE_SCHOLAR              40
#define JOKER_TYPE_BUSINESS_CARD        41
#define JOKER_TYPE_SUPERNOVA            42
#define JOKER_TYPE_RIDE_THE_BUS         43
#define JOKER_TYPE_SPACE_JOKER          44
#define JOKER_TYPE_EGG                  45
#define JOKER_TYPE_BURGLAR              46
#define JOKER_TYPE_BLACKBOARD           47
#define JOKER_TYPE_RUNNER               48
#define JOKER_TYPE_ICE_CREAM            49
#define JOKER_TYPE_DNA                  50
#define JOKER_TYPE_SPLASH               51
#define JOKER_TYPE_BLUE_JOKER           52
#define JOKER_TYPE_SIXTH_SENSE          53
#define JOKER_TYPE_CONSTELLATION        54
#define JOKER_TYPE_HIKER                55
#define JOKER_TYPE_FACELESS_JOKER       56
#define JOKER_TYPE_GREEN_JOKER          57
#define JOKER_TYPE_SUPERPOSITION        58
#define JOKER_TYPE_TO_DO_LIST           59
#define JOKER_TYPE_CAVENDISH            60
#define JOKER_TYPE_CARD_SHARP           61
#define JOKER_TYPE_RED_CARD             62
#define JOKER_TYPE_MADNESS              63
#define JOKER_TYPE_SQUARE_JOKER         64
#define JOKER_TYPE_SEANCE               65
#define JOKER_TYPE_RIFF_RAFF            66
#define JOKER_TYPE_VAMPIRE              67
#define JOKER_TYPE_SHORTCUT             68
#define JOKER_TYPE_HOLOGRAM             69
#define JOKER_TYPE_VAGABOND             70
#define JOKER_TYPE_BARON                71
#define JOKER_TYPE_CLOUD_9              72
#define JOKER_TYPE_ROCKET               73
#define JOKER_TYPE_OBELISK              74
#define JOKER_TYPE_MIDAS_MASK           75
#define JOKER_TYPE_LUCHADOR             76
#define JOKER_TYPE_PHOTOGRAPH           77
#define JOKER_TYPE_GIFT_CARD            78
#define JOKER_TYPE_TURTLE_BEAN          79
#define JOKER_TYPE_EROSION              80
#define JOKER_TYPE_RESERVED_PARKING     81
#define JOKER_TYPE_MAIL_IN_REBATE       82
#define JOKER_TYPE_TO_THE_MOON          83
#define JOKER_TYPE_HALLUCINATION        84
#define JOKER_TYPE_FORTUNE_TELLER       85
#define JOKER_TYPE_JUGGLER              86
#define JOKER_TYPE_DRUNKARD             87
#define JOKER_TYPE_STONE_JOKER          88
#define JOKER_TYPE_GOLDEN_JOKER         89
#define JOKER_TYPE_LUCKY_CAT            90
#define JOKER_TYPE_BASEBALL_CARD        91
#define JOKER_TYPE_BULL                 92
#define JOKER_TYPE_DIET_COLA            93
#define JOKER_TYPE_TRADING_CARD         94
#define JOKER_TYPE_FLASH_CARD           95
#define JOKER_TYPE_POPCORN              96
#define JOKER_TYPE_SPARE_TROUSERS       97
#define JOKER_TYPE_ANCIENT_JOKER        98
#define JOKER_TYPE_RAMEN                99
#define JOKER_TYPE_WALKIE_TALKIE        100
#define JOKER_TYPE_SELTZER              101
#define JOKER_TYPE_CASTLE               102
#define JOKER_TYPE_SMILEY_FACE          103
#define JOKER_TYPE_CAMPFIRE             104
#define JOKER_TYPE_GOLDEN_TICKET        105
#define JOKER_TYPE_MR_BONES             106
#define JOKER_TYPE_ACROBAT              107
#define JOKER_TYPE_SOCK_AND_BUSKIN      108
#define JOKER_TYPE_SWASHBUCKLER         109
#define JOKER_TYPE_TROUBADOUR           110
#define JOKER_TYPE_CERTIFICATE          111
#define JOKER_TYPE_SMEARED_JOKER        112
#define JOKER_TYPE_THROWBACK            113
#define JOKER_TYPE_HANGING_CHAD         114
#define JOKER_TYPE_ROUGH_GEM            115
#define JOKER_TYPE_BLOODSTONE           116
#define JOKER_TYPE_ARROWHEAD            117
#define JOKER_TYPE_ONYX_AGATE           118
#define JOKER_TYPE_GLASS_JOKER          119
#define JOKER_TYPE_SHOWMAN              120
#define JOKER_TYPE_FLOWERPOT            121
#define JOKER_TYPE_BLUEPRINT            122
#define JOKER_TYPE_WEE_JOKER            123
#define JOKER_TYPE_MERRY_ANDY           124
#define JOKER_TYPE_OOPS_ALL_6S          125
#define JOKER_TYPE_THE_IDOL             126
#define JOKER_TYPE_SEEING_DOUBLE        127
#define JOKER_TYPE_MATADOR              128
#define JOKER_TYPE_HIT_THE_ROAD         129
#define JOKER_TYPE_THE_DUO              130
#define JOKER_TYPE_THE_TRIO             131
#define JOKER_TYPE_THE_FAMILY           132
#define JOKER_TYPE_THE_ORDER            133
#define JOKER_TYPE_THE_TRIBE            134
#define JOKER_TYPE_STUNTMAN             135
#define JOKER_TYPE_INVISIBLE_JOKER      136
#define JOKER_TYPE_BRAINSTORM           137
#define JOKER_TYPE_SATELLITE            138
#define JOKER_TYPE_SHOOT_THE_MOON       139
#define JOKER_TYPE_DRIVERS_LICENSE      140
#define JOKER_TYPE_CARTOMANCER          141
#define JOKER_TYPE_ASTRONOMER           142
#define JOKER_TYPE_BURNT_JOKER          143
#define JOKER_TYPE_BOOTSTRAPS           144
#define JOKER_TYPE_CANIO                145
#define JOKER_TYPE_TRIBOULET            146
#define JOKER_TYPE_YORICK               147
#define JOKER_TYPE_CHICOT               148
#define JOKER_TYPE_PARKEO               149

#define JOKER_TYPE_COUNT                150

#define JOKER_RARITY_COMMON             0
#define JOKER_RARITY_UNCOMMON           1
#define JOKER_RARITY_RARE               2
#define JOKER_RARITY_LEGENDARY          3

#define JOKER_RARITY_COUNT              4

extern const char *g_rarity_hint[JOKER_RARITY_COUNT];

#define HINT_JOKER_TYPE_LENGTH          6

struct JokerType
{
    uint16_t type;
    char *name;
    int value;
    int rarity;
    bool enabled;
    int u, v;
    const char *hint_lines[HINT_JOKER_TYPE_LENGTH];
};

extern struct JokerType g_joker_types[JOKER_TYPE_COUNT];

struct Joker
{
    struct DrawObject draw;

    uint8_t type;
    uint8_t edition;
    int param1;
    int repeat;

    int value;    
};

#define MAX_JOKERS  1024

struct Jokers
{
    struct Joker jokers[MAX_JOKERS];
    uint16_t joker_count;
    uint16_t negative_count;
};

#define PLANET_TYPE_PLUTO       0
#define PLANET_TYPE_MERCURY     1
#define PLANET_TYPE_URANUS      2
#define PLANET_TYPE_VENUS       3
#define PLANET_TYPE_SATURN      4
#define PLANET_TYPE_JUPITER     5
#define PLANET_TYPE_EARTH       6
#define PLANET_TYPE_MARS        7
#define PLANET_TYPE_NEPTUNE     8
#define PLANET_TYPE_PLANET_X    9
#define PLANET_TYPE_CERES       10
#define PLANET_TYPE_ERIS        11

#define PLANET_TYPE_COUNT       12

#define HINT_PLANET_TYPE_LENGTH  4

struct PlanetType
{
    uint16_t type;
    const char *name;
    uint16_t poker_hand;
    int mult, chips;
    int u, v;
};

extern struct PlanetType g_planet_types[PLANET_TYPE_COUNT];

struct Planet
{
    struct DrawObject draw;
    uint8_t type;
    int edition;
};

#define TAROT_TYPE_FOOL             0
#define TAROT_TYPE_MAGICIAN         1
#define TAROT_TYPE_PRIESTESS        2
#define TAROT_TYPE_EMPRESS          3
#define TAROT_TYPE_EMPEROR          4
#define TAROT_TYPE_HIEROPHANT       5
#define TAROT_TYPE_LOVERS           6
#define TAROT_TYPE_CHARIOT          7
#define TAROT_TYPE_JUSTICE          8
#define TAROT_TYPE_HERMIT           9
#define TAROT_TYPE_WHEEL_FORTUNE    10
#define TAROT_TYPE_STRENGTH         11
#define TAROT_TYPE_HANGED_MAN       12
#define TAROT_TYPE_DEATH            13
#define TAROT_TYPE_TEMPERANCE       14
#define TAROT_TYPE_DEVIL            15
#define TAROT_TYPE_TOWER            16
#define TAROT_TYPE_STAR             17
#define TAROT_TYPE_MOON             18
#define TAROT_TYPE_SUN              19
#define TAROT_TYPE_JUDGEMENT        20
#define TAROT_TYPE_WORLD            21

#define TAROT_TYPE_COUNT            22

#define HINT_TAROT_TYPE_LENGTH      4

struct TarotType
{
    uint8_t type;
    const char *name;
    int min_targets, max_targets;
    int u, v;
    const char *hint_lines[HINT_TAROT_TYPE_LENGTH];
};

extern struct TarotType g_tarot_types[TAROT_TYPE_COUNT];

struct Tarot
{
    struct DrawObject draw;
    uint8_t type;
    int edition;
};

#define SPECTRAL_TYPE_FAMILIAR      0
#define SPECTRAL_TYPE_GRIM          1
#define SPECTRAL_TYPE_INCANTATION   2
#define SPECTRAL_TYPE_TALISMAN      3
#define SPECTRAL_TYPE_AURA          4
#define SPECTRAL_TYPE_WRAITH        5
#define SPECTRAL_TYPE_SIGIL         6
#define SPECTRAL_TYPE_OUIJA         7
#define SPECTRAL_TYPE_ECTOPLASM     8
#define SPECTRAL_TYPE_IMMOLATE      9
#define SPECTRAL_TYPE_ANKH          10
#define SPECTRAL_TYPE_DEJA_VU       11
#define SPECTRAL_TYPE_HEX           12
#define SPECTRAL_TYPE_TRANCE        13
#define SPECTRAL_TYPE_MEDIUM        14
#define SPECTRAL_TYPE_CRYPTID       15
#define SPECTRAL_TYPE_SOUL          16
#define SPECTRAL_TYPE_BLACK_HOLE    17

#define SPECTRAL_TYPE_COUNT         18

#define HINT_SPECTRAL_TYPE_LENGTH   4

struct SpectralType
{
    uint8_t type;
    const char *name;
    int min_targets, max_targets;
    int u, v;
    const char *hint_lines[HINT_SPECTRAL_TYPE_LENGTH];
};

extern struct SpectralType g_spectral_types[SPECTRAL_TYPE_COUNT];

struct Spectral
{
    struct DrawObject draw;
    uint8_t type;
    int edition;
};

#define BOOSTER_PACK_TYPE_STANDARD  0
#define BOOSTER_PACK_TYPE_ARCANA    1
#define BOOSTER_PACK_TYPE_CELESTIAL 2
#define BOOSTER_PACK_TYPE_BUFFOON   3
#define BOOSTER_PACK_TYPE_SPECTRAL  4

#define BOOSTER_PACK_TYPE_COUNT     5

#define BOOSTER_PACK_SIZE_NORMAL    0
#define BOOSTER_PACK_SIZE_JUMBO     1
#define BOOSTER_PACK_SIZE_MEGA      2

#define BOOSTER_PACK_SIZE_COUNT     3

#define HINT_BOOSTER_TYPE_LENGTH    3

struct BoosterPack
{
    struct DrawObject draw;
    int type;
    int size;
    int image;
};

struct BoosterPackType
{
    int type;
    int size;
    int image;
    int u, v;
    const char *name;
    const char *hint_lines[HINT_TAROT_TYPE_LENGTH];
};

#define BOOSTER_PACK_MAX_IMAGES 4

extern struct BoosterPackType g_booster_types[BOOSTER_PACK_TYPE_COUNT][BOOSTER_PACK_SIZE_COUNT][BOOSTER_PACK_MAX_IMAGES];
extern double g_booster_packs_weights[BOOSTER_PACK_TYPE_COUNT][BOOSTER_PACK_SIZE_COUNT];
extern double g_booster_packs_weights_total;

#define GAME_STAGE_BLINDS       0
#define GAME_STAGE_INGAME       1
#define GAME_STAGE_SHOP         2
#define GAME_STAGE_PAUSE_MENU   3

#define GAME_SUBSTAGE_BLINDS_DEFAULT        0

#define GAME_SUBSTAGE_INGAME_PICK_HAND      0
#define GAME_SUBSTAGE_INGAME_WON            1
#define GAME_SUBSTAGE_INGAME_WON_END        2
#define GAME_SUBSTAGE_INGAME_LOST_END       3

#define GAME_SUBSTAGE_SHOP_MAIN             0
#define GAME_SUBSTAGE_SHOP_BOOSTER_HAND     1
#define GAME_SUBSTAGE_SHOP_BOOSTER_NO_HAND  2

#define GAME_SUBSTAGE_DECK_INFO             4
#define GAME_SUBSTAGE_RUN_INFO              5

#define GAME_POKER_HAND_NONE            0
#define GAME_POKER_HAND_HIGH_CARD       1
#define GAME_POKER_HAND_PAIR            2
#define GAME_POKER_HAND_TWO_PAIR        3
#define GAME_POKER_HAND_THREE_OF_A_KIND 4
#define GAME_POKER_HAND_STRAIGHT        5
#define GAME_POKER_HAND_FLUSH           6
#define GAME_POKER_HAND_FULL_HOUSE      7
#define GAME_POKER_HAND_FOUR_OF_A_KIND  8
#define GAME_POKER_HAND_STRAIGHT_FLUSH  9
#define GAME_POKER_HAND_FIVE_OF_A_KIND  10
#define GAME_POKER_HAND_FLUSH_HOUSE     11
#define GAME_POKER_HAND_FLUSH_FIVE      12

#define GAME_POKER_HAND_COUNT           13

extern char *g_poker_hand_names[GAME_POKER_HAND_COUNT];

#define GAME_BLIND_SMALL    0
#define GAME_BLIND_LARGE    1
#define GAME_BLIND_BOSS     2

#define SCORE_NUMBER_ADD_CHIPS  0
#define SCORE_NUMBER_ADD_MULT   1
#define SCORE_NUMBER_MULT_MULT  2
#define SCORE_NUMBER_ADD_MONEY  3
#define SCORE_NUMBER_AGAIN      4

// #define SORT_CRITERIA_RANK  0
// #define SORT_CRITERIA_SUIT  1

#define MAX_ITEM_TYPES  20

enum ItemLocation
{
    ITEM_LOCATION_TOP,
    ITEM_LOCATION_CONSUMIBLE,
    ITEM_LOCATION_HAND,
    ITEM_LOCATION_SHOP_SINGLES
};

enum ItemType
{
    ITEM_TYPE_JOKER,
    ITEM_TYPE_CARD,
    ITEM_TYPE_TAROT,
    ITEM_TYPE_PLANET,
    ITEM_TYPE_SPECTRAL
};

#define MAX_CONSUMABLES 100

struct Consumables
{
    struct ConsumableItem {
        enum ItemType type;
        struct Planet planet;
        struct Tarot tarot;
        struct Spectral spectral;
    } items[MAX_CONSUMABLES];
    int item_count;
};

#define MAX_BOOSTERS    30

struct Item
{
    bool available;
    enum ItemType type;
    union
    {
        struct Joker joker;
        struct Card card;
        struct Planet planet;
        struct Tarot tarot;
        struct Spectral spectral;
    } info;
};

struct GameState
{        
    struct Jokers jokers;
    struct Consumables consumables;
    struct Cards all_cards;
    struct CardReferences full_deck;
    struct CardReferences current_deck;
    struct CardReferences hand, played_hand;
    int base_hand_size;
    int ante, blind, round, total_hands, total_discards;
    int wealth;
    int joker_slots;
    int consumable_slots;
    int ante_score_scaling;

    int poker_hand_base_chips[GAME_POKER_HAND_COUNT];
    int poker_hand_base_mult[GAME_POKER_HAND_COUNT];
    int poker_hand_level[GAME_POKER_HAND_COUNT];

    // input
    enum
    {
        INPUT_FOCUSED_ZONE_NONE           ,
        INPUT_FOCUSED_ZONE_HAND           ,
        INPUT_FOCUSED_ZONE_JOKERS         ,
        INPUT_FOCUSED_ZONE_CONSUMABLES    ,
        INPUT_FOCUSED_ZONE_SHOP_SINGLES   ,
        INPUT_FOCUSED_ZONE_SHOP_BOOSTERS  ,
        INPUT_FOCUSED_ZONE_SHOP_NEXT_ROUND,
        INPUT_FOCUSED_ZONE_SHOP_REROLL    ,
        INPUT_FOCUSED_ZONE_BLIND          ,
        INPUT_FOCUSED_ZONE_CASH_OUT_PANEL ,
        INPUT_FOCUSED_ZONE_SORT_HAND_RANK ,
        INPUT_FOCUSED_ZONE_SORT_HAND_SUIT ,
        INPUT_FOCUSED_ZONE_HAND_PLAY      ,
        INPUT_FOCUSED_ZONE_HAND_DISCARD   ,
        INPUT_FOCUSED_ZONE_BOOSTER_ITEMS  ,
        INPUT_FOCUSED_ZONE_YOU_WIN_PANEL  ,
        INPUT_FOCUSED_ZONE_GAME_OVER_PANEL,
        INPUT_FOCUSED_ZONE_DECK           ,
        INPUT_FOCUSED_ZONE_DECK_INFO_CLOSE,
        INPUT_FOCUSED_ZONE_DECK_INFO_CARD ,
        INPUT_FOCUSED_ZONE_RUN_INFO_CLOSE ,
        INPUT_FOCUSED_ZONE_RUN_INFO       ,
        INPUT_FOCUSED_ZONE_PAUSE_MENU     ,
        INPUT_FOCUSED_ZONE_SETTINGS_MENU  ,
    } input_focused_zone, previous_input_focused_zone;

    // ingame
    double score;
    int current_hands, current_discards;
    int stage, sub_stage;
    int previous_stage, previous_sub_stage;
    int selected_cards_count;
    int current_poker_hand;
    int scoring_card_count;
    uint32_t current_base_chips, current_base_mult;
    enum
    {
        SORT_CRITERIA_RANK,
        SORT_CRITERIA_SUIT
    } sort_criteria;

    int highlighted_item;

    // ingame - cash out
    int cash_out_value;
    int cash_out_blind;
    int cash_out_hands;
    int cash_out_interest;
    int cash_out_jokers;
    bool cash_out_done;

    int hands_played_in_round;

    struct
    {
        int total_items;
        int total_boosters;
        int rerolls;
        int reroll_cost;
        struct Item items[MAX_ITEM_TYPES];
        int item_count;
        struct
        {
            bool available;
            struct BoosterPack booster;
        } boosters[MAX_BOOSTERS];
        int booster_count;
        struct
        {
            int type;
            int size;
        } opened_booster;
        struct Item booster_items[MAX_ITEM_TYPES];
        int booster_item_count;
        int booster_total_items;
    } shop;

    // graphics
    struct
    {
        bool show_score_number;
        int score_number_type;
        char text[32];
        struct Card *card;
        struct Joker *joker;
        double text_scale;
        double back_quad_scale;
        double back_quad_angle;
    } score_number;
    
    bool show_highlighted_card;
    float cash_out_panel_y;

    bool moving_card;
    bool cross_pressed;

    int last_used_consumable_item_type;
    int last_used_consumable_tarot_planet_type;

    struct
    {
        int cards_played;
        int cards_discarded;
        double max_score;
        int poker_hands_played[GAME_POKER_HAND_COUNT];
        int most_played_poker_hand;
        int cards_purchased;
        int times_rerolled;
    } stats;


    bool flush_five_enabled;
    bool flush_house_enabled;
    bool five_of_a_kind_enabled;

    struct
    {
        struct Card *cards[4][MAX_CARDS];
        struct DrawObject draw[4][MAX_CARDS];
        bool in_current_deck[4][MAX_CARDS];
        int card_count[4];

        int rank_count[CARD_RANK_COUNT];
        int suit_count[4];
        int effective_rank_count[CARD_RANK_COUNT];
        int effective_suit_count[4];

        int face_count;
        int number_count;
        int effective_face_count;
        int effective_number_count;

        bool partial;
        bool effective;

        int current_card_x, current_card_y;
        struct Card current_card;
    } deck_info;
    
};

extern struct GameState g_game_state;

// GAME UTIL

int game_util_get_joker_type(int index);
int game_util_get_real_joker_type(int index);
void game_util_set_joker_param(int index, int value);
int game_util_get_joker_param(int index);
bool game_util_has_joker_type(int joker_type);
int game_util_get_joker_type_count(int joker_type);
int game_util_get_real_joker_type_count(int joker_type);
bool game_util_is_card_suit(struct Card *card, int suit);
bool game_util_played_hand_contains_pair();
bool game_util_played_hand_contains_three_of_a_kind();
bool game_util_played_hand_contains_four_of_a_kind();
bool game_util_played_hand_contains_two_pair();
bool game_util_played_hand_contains_straight();
bool game_util_played_hand_contains_flush();
bool game_util_played_hand_contains_scoring_face();
char *game_util_get_blind_name(int blind);
bool game_util_is_joker_slot_available();
bool game_util_is_consumable_slot_available();
int game_util_get_joker_buy_price(struct Joker *joker);
int game_util_get_joker_sell_price(struct Joker *joker);
int game_util_get_booster_price(struct BoosterPack *booster);
int game_util_get_first_shop_single_index();
// int game_util_get_new_shop_joker_type();
int game_util_get_new_joker_type(int excluded_joker_types[], int excluded_joker_types_count, bool only_commons);
int game_util_get_new_tarot_type(int excluded_tarot_types[], int excluded_tarot_types_count);
int game_util_get_new_planet_type(int excluded_planet_types[], int excluded_planet_types_count);
int game_util_get_new_spectral_type(int excluded_spectral_types[], int excluded_spectral_types_count, bool include_hidden);
int game_util_get_planet_buy_price(struct Planet *planet);
int game_util_get_planet_sell_price(struct Planet *planet);
int game_util_get_tarot_buy_price(struct Tarot *tarot);
int game_util_get_tarot_sell_price(struct Tarot *tarot);
int game_util_get_spectral_buy_price(struct Spectral *spectral);
int game_util_get_spectral_sell_price(struct Spectral *spectral);
int game_util_get_reroll_cost();
bool game_util_can_buy(float price);
int game_util_get_default_focus_zone();
void game_util_copy_deck(struct CardReferences *src, struct CardReferences *dst);
void game_util_shuffle_deck(struct CardReferences *deck);
bool game_util_chance_occurs(int num, int den);
int game_util_rand(int min, int max);
int game_util_get_lowest_rank_held_in_hand();
bool game_util_is_card_rank(struct Card *card, int rank);
bool game_util_is_card_face(struct Card *card);
bool game_util_has_scoring_cards_rank(int rank);
void game_init_joker(struct Joker *joker);
void game_init_tarot(struct Tarot *tarot, int type, int edition);
void game_init_planet(struct Planet *planet, int type, int edition);
void game_init_spectral(struct Spectral *spectral, int type, int edition);
float game_util_get_item_position(int i, int item_count, float start_x, float width, float item_width);
int game_util_get_first_shop_booster_index();
int game_util_get_first_shop_item_index();
int game_util_get_first_shop_booster_item_index();
double random_double(double min, double max);
float random_float(float min, float max);
int random_int(int min, int max);
void game_util_get_new_booster_pack(int *type, int *size);
void game_util_get_joker_hint_value(struct Joker *joker, char *dst);
void game_util_get_tarot_hint_value(struct Tarot *tarot, char *dst);
bool game_util_can_tarot_be_used(int type);
bool game_util_can_spectral_be_used(int type);
bool game_util_has_room_in_consumables();
bool game_util_has_room_in_jokers();
int game_util_get_card_original_chips(struct Card *card);
int game_util_get_card_chips(struct Card *card);
int game_util_get_planet_type_of_poker_hand(int poker_hand);
int game_util_get_first_scoring_face_card_index();
int game_util_get_index_first_scoring_card();
void game_util_get_first_deck_info_card(int *card_x, int *card_y);
bool game_util_are_cards_same_suit(struct Card *cards[], int card_count);
int game_util_get_random_poker_hand();
int game_util_get_hand_size();
int game_util_get_hands();
int game_util_get_discards();
int game_util_get_shop_joker_edition();
int game_util_get_random_booster_card_edition();

// AUTOMATED EVENTS

#define AUTOMATED_EVENT_NONE                0
#define AUTOMATED_EVENT_SCORE               1
#define AUTOMATED_EVENT_DISCARD             2
#define AUTOMATED_EVENT_HAND_CARD_SWAP      3
#define AUTOMATED_EVENT_DRAW                4
#define AUTOMATED_EVENT_CASH_OUT            5
#define AUTOMATED_EVENT_SORT_HAND           6
#define AUTOMATED_EVENT_BUY_JOKER           7
#define AUTOMATED_EVENT_SELL_JOKER          8
#define AUTOMATED_EVENT_BUY_CONSUMABLE      9
#define AUTOMATED_EVENT_SELL_CONSUMABLE     10
#define AUTOMATED_EVENT_USE_PLANET          11
#define AUTOMATED_EVENT_USE_TAROT           12
#define AUTOMATED_EVENT_ADD_SCORE           13
#define AUTOMATED_EVENT_WAIT                14
#define AUTOMATED_EVENT_SELECT_BLIND        15
#define AUTOMATED_EVENT_CEREMONIAL_DAGGER   16
#define AUTOMATED_EVENT_OPEN_BOOSTER        17
#define AUTOMATED_EVENT_ADD_BOOSTER_CARD    18
#define AUTOMATED_EVENT_USE_SPECTRAL        19

#define AUTOMATED_EVENT_COUNT               20

void automated_event_set(int event_id, int num_params, ...);
void automated_event_push(int event_id, int num_params, ...);
void automated_event_pop();
bool automated_event_run();

bool game_init_load_file_values();

double game_get_ante_base_score();
double game_get_current_blind_score();
void game_sort_hand_by_rank();
void game_sort_hand();
void game_set_card_hand_positions();
void game_set_card_hand_initial_positions();
void game_set_card_hand_final_positions();
void game_draw_card_into_hand();
void game_discard_card(int i);
void game_set_card_played_hand_final_position();
void game_move_selected_cards_to_played_cards();
void game_set_hand_y(float y);
float game_get_hand_y();
void game_ingame_discard_played_hand();
void game_set_scoring_cards_in_played_hand();

int game_get_card_chips(struct Card *card);
void game_set_card_name(struct Card *card, char *name);

int game_get_selected_poker_hand(struct Card *cards[], int card_count, int scoring_cards[], int *scoring_cards_count);

void game_start_ingame();
void game_go_to_next_blind();
void game_start_ingame();

void game_go_back_to_previous_stage();
void game_go_to_stage(int stage, int sub_stage);

void game_set_initial_final_shop_item_position();
void game_set_initial_final_joker_position();

void game_set_initial_final_consumable_position();

bool game_shop_buy_highlighted_item(bool use_item);

void game_shop_reroll();
void game_sell_joker();

void game_sell_consumable();
bool game_use_consumable();
bool game_use_booster_item();

void game_init_draw_object(struct DrawObject *draw);

void game_set_initial_position(struct DrawObject *draw);

void game_remove_card_from_played_hand(int index);
void game_remove_card_from_full_deck(struct Card *card);
void game_remove_invalid_cards_full_deck();

struct Card *game_set_new_card();
void game_add_card_to_full_deck(struct Card *card);
void game_add_card_shuffled_to_current_deck(struct Card *card);

void game_remove_joker(int index);

bool game_buy_booster();

void game_set_object_off_screen(struct DrawObject *draw);

void game_discard_selected_cards();

void game_increment_played_poker_hand(int poker_hand);

void game_restart_game();

void game_add_card_to_hand(struct Card *card);

void game_add_joker(struct Joker *joker);

// GAME INPUT

void game_input_update(bool no_input);

// DRAW

#define CARD_WIDTH  48
#define CARD_HEIGHT 65

#define BOOSTER_WIDTH  55
#define BOOSTER_HEIGHT 80

#define DRAW_HAND_Y             170.0f
#define DRAW_HAND_Y_IN_SCORE    194.0f
#define DRAW_HAND_Y_IN_BOOSTER  100.0f
#define DRAW_HAND_X             100.0f
#define DRAW_HAND_WIDTH         350.0f
#define DRAW_SELECTED_CARD_Y    140.0f
#define DRAW_PLAYED_CARDS_Y     120.0f
#define DRAW_SCORING_CARDS_Y    110.0f
#define DRAW_JOKERS_X           100.0f
#define DRAW_JOKERS_WIDTH       250.0f
#define DRAW_JOKERS_Y           10.0f
#define DRAW_CONSUMABLES_X      360.0f
#define DRAW_CONSUMABLES_WIDTH  110.0f
#define DRAW_CONSUMABLES_y      10.0f
#define DRAW_DECK_X             420.0f
#define DRAW_DECK_Y             190.0f
#define DRAW_SHOP_SINGLE_X      198.0f
#define DRAW_SHOP_SINGLE_WIDTH  200.0f
#define DRAW_SHOP_SINGLE_Y      100.0f
#define DRAW_SHOP_BOOSTER_X      228.0f
#define DRAW_SHOP_BOOSTER_WIDTH  170.0f
#define DRAW_SHOP_BOOSTER_Y      182.0f
#define DRAW_BOOSTER_ITEMS_X    100.0f
#define DRAW_BOOSTER_ITEMS_Y    180.0f
#define DRAW_BOOSTER_ITEMS_WIDTH 350.0f
#define DRAW_DECK_INFO_WIDTH    400.0f
#define DRAW_DECK_INFO_HEIGHT   250.0f
#define DRAW_RUN_INFO_WIDTH     312.0f
#define DRAW_RUN_INFO_HEIGHT    250.0f

#define CARD_HINT_WIDTH             146.0f
#define CARD_HINT_MIN_HEIGHT        30.0f

float lerp(float min, float max, float t);

bool game_init_draw();
void game_init_draw_load_fonts();
void game_draw_clear_loading(uint32_t color);
void game_draw_loading_text(const char *text, uint32_t color, uint32_t back_color);
void game_draw_offscreen_deck_info();

#define EVENT_MOVE_CARD             0
#define EVENT_ARRANGE_CARDS         1
#define EVENT_POP_UP_CARD           2
#define EVENT_SHOW_NUMBER           3
#define EVENT_SHAKE                 4
#define EVENT_INTERPOLATE_VALUE     5
#define EVENT_POP                   6

#define EVENT_TYPE_COUNT            7

#define EVENT_CARD_LOCATION_HAND          1
#define EVENT_CARD_LOCATION_PLAYED        2
#define EVENT_CARD_LOCATION_JOKER         3
#define EVENT_CARD_LOCATION_CONSUMABLES   4
#define EVENT_CARD_LOCATION_SHOP_SINGLES  5
#define EVENT_CARD_LOCATION_BOOSTER_ITEMS 6

struct Position
{
    float x, y;
};

void event_init();
int event_add(int type, int sub_type, int card_location, int card_index, float position_x, float position_y, int duration);
int event_add_show_number(double number, int card_index, int card_location, int number_type, int duration);
int event_add_show_number_joker(double number, int joker_index, int number_type, int duration);
int event_add_shake_joker(int joker_index, int duration);
int event_add_shake_item(struct DrawObject *draw, int duration);
int event_add_pop_joker(int joker_index, int duration);
int event_add_pop_item(struct DrawObject *draw, int duration);
int event_add_interpolate_value(float *value, float start_y, float end_y, int duration);

// AUDIO

void audio_init();
void audio_end();
void audio_update();
int audio_load_ogg(char *filename);
int audio_load_ogg_from_archive(char *filename);
void audio_play_ogg(int ogg_id, float speed);
void audio_stop();
void audio_destroy_ogg(int ogg_id);
int audio_load_sfx_from_archive(int sfx_id, char *filename, float volume);
int audio_load_sfx_from_archive_limited(int sfx_id, char *filename, float volume, float seconds);
void audio_play_sfx(int sfx_id);
void audio_set_score_flame_intensity(float intensity);
void audio_destroy_sfx();

#define AUDIO_SFX_BUTTON      0
#define AUDIO_SFX_CANCEL      1
#define AUDIO_SFX_HIGHLIGHT   2
#define AUDIO_SFX_CARD        3
#define AUDIO_SFX_CHIPS       4
#define AUDIO_SFX_MULT        5
#define AUDIO_SFX_COIN        6
#define AUDIO_SFX_TAROT       7
#define AUDIO_SFX_WHOOSH      8
#define AUDIO_SFX_WIN         9
#define AUDIO_SFX_FLAME       10
#define AUDIO_SFX_COUNT       11


// GRAPHICS

#define GRAPHICS_TEXTURE_FILTER_NEAREST     GU_NEAREST
#define GRAPHICS_TEXTURE_FILTER_LINEAR      GU_LINEAR

struct Image
{
    uint8_t *data;
    int w, h, channels;
};

#define COLOR_WHITE         0xFFFFFFFF
#define COLOR_BLACK         0xFF000000
#define COLOR_TEAL          0xFF808000
#define COLOR_RED           0xFF0000FF
#define COLOR_GREEN         0xFF00FF00
#define COLOR_BLUE          0xFFFF0000
#define COLOR_LIGHT_GREY    0xFF888888
#define COLOR_LIGHT_GREY_2  0xFFC0C0C0
#define COLOR_DARK_GREY     0xFF666666
#define COLOR_DARK_GREY_2   0xFF444444
#define COLOR_LIGHT_BLUE    0xFFEF8700
#define COLOR_LIGHT_RED     0xFF8080FF
#define COLOR_LIGHT_BLUE_2  0xFFFF4040
#define COLOR_LIGHT_RED_2   0xFF4040FF
#define COLOR_YELLOW        0xFF49B3EF

#define COLOR_BACKGROUND    0xFF608000
#define COLOR_BACKGROUND_2  0xFF808000

#define COLOR_TEXT_RED      0xFF404CFF
#define COLOR_TEXT_BLUE     0xFFFF9300
#define COLOR_TEXT_GREEN    0xFF86BD35
#define COLOR_TEXT_ORANGE   0xFF008FFF
#define COLOR_TEXT_PURPLE   0xFF9C557B
#define COLOR_TEXT_CYAN     0xFFCAA800
#define COLOR_TEXT_YELLOW   0xFF49B3EF

#define COLOR_RED_BUTTON    0xFF8080FF
#define COLOR_GREEN_BUTTON  0xFF86BD35

#define COLOR_SCORE_NUMBER_TEXT_CHIPS       0xFFFFFFFF
#define COLOR_SCORE_NUMBER_TEXT_ADD_MULT    0xFF8888FF
#define COLOR_SCORE_NUMBER_TEXT_MULT_MULT   0xFF3333FF
#define COLOR_SCORE_NUMBER_TEXT_MONEY       0xFFFFFF33

void graphics_init();
int graphics_load_small_font();
int graphics_load_big_font();
struct Image graphics_load_image_from_archive(const char *filename);
struct Image graphics_load_image(const char *filename);
void graphics_destroy_image(struct Image *image);
int graphics_load_texture_from_image(struct Image *loaded_image, int start_x, int start_y);
int graphics_load_texture_from_image_16bit(struct Image *loaded_image, int start_x, int start_y);
int graphics_load_texture_from_archive(const char *filename, int start_x, int start_y);
int graphics_load_texture_from_archive_16bit(const char *filename, int start_x, int start_y);
int graphics_load_texture(const char *filename, int start_x, int start_y);
int graphics_load_texture_16bit(const char *filename, int start_x, int start_y);
void graphics_destroy_texture(int texture);
int graphics_load_font(const char *filename, int width, int height, int length_x, int length_y);
void graphics_destroy_font(int font);
void graphics_begin_draw();
void graphics_end_draw();
void graphics_clear(uint32_t color);
void graphics_set_no_texture();
void graphics_set_texture(int texture, int filter);
void graphics_draw_rotated_quad(float x, float y, float w, float h, int16_t u, int16_t v, int16_t uw, int16_t vh, uint32_t color, float angle);
void graphics_draw_quad(float x, float y, float w, float h, int16_t u, int16_t v, int16_t uw, int16_t vh, uint32_t color);
void graphics_draw_solid_quad(float x, float y, float w, float h, uint32_t color);
void graphics_draw_text_center(int font, const char *text, float x, float y, float size, uint32_t color);
void graphics_draw_text(int font, const char *text, float x, float y, float size, uint32_t color);
void graphics_draw_text_formatted_center(int font, const char *text, void *item, float x, float y, float size, uint32_t color);
void graphics_draw_text_formatted(int font, const char *text, void *item, float x, float y, float size, uint32_t color);
void graphics_clear_loading_text(uint32_t clear_color);
void graphics_show_loading_text(int font_tex, const char *text, float x, float y, float size, uint32_t text_color, bool clear, uint32_t clear_color);
void graphics_set_offscreen_render_target();
void graphics_unset_offscreen_render_target();
void graphics_flush_quads();

extern bool g_freeze_cards;

#define INPUT_BUTTON_LEFT           PSP_CTRL_LEFT
#define INPUT_BUTTON_RIGHT          PSP_CTRL_RIGHT
#define INPUT_BUTTON_UP             PSP_CTRL_UP
#define INPUT_BUTTON_DOWN           PSP_CTRL_DOWN
#define INPUT_BUTTON_CROSS          PSP_CTRL_CROSS
#define INPUT_BUTTON_CIRCLE         PSP_CTRL_CIRCLE
#define INPUT_BUTTON_SQUARE         PSP_CTRL_SQUARE
#define INPUT_BUTTON_TRIANGLE       PSP_CTRL_TRIANGLE
#define INPUT_BUTTON_LEFT_TRIGGER   PSP_CTRL_LTRIGGER
#define INPUT_BUTTON_RIGHT_TRIGGER  PSP_CTRL_RTRIGGER
#define INPUT_BUTTON_START          PSP_CTRL_START

// INPUT

void input_init();
void input_update();
bool input_is_button_down(unsigned int button);
bool input_was_button_pressed(unsigned int button);
bool input_was_button_released(unsigned int button);

// ARCHIVE

bool archive_open(const char *filename);
void archive_close();
uint8_t *archive_load_file_entry(const char *filename, size_t *file_size);
size_t archive_get_file_entry_size(const char *filename);

// INI

enum IniTokenType {
    INI_TOKEN_SECTION,
    INI_TOKEN_KEY,
    INI_TOKEN_VALUE,
    INI_TOKEN_COMMENT,
    INI_TOKEN_EOF,
    INI_TOKEN_ERROR
};

bool ini_open(const char *filename);
void ini_close();
enum IniTokenType ini_read_token(char *buffer, int buffer_size);

// Game Constants

#define GAME_CHANCE_COMMON_JOKER        70  // 70%
#define GAME_CHANCE_UNCOMMON_JOKER      25  // 25%
#define GAME_CHANCE_RARE_JOKER           5  //  5%
