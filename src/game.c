#include "global.h"

struct Settings g_settings = {
    "Balatro.exe",
    8,
    4,
    3,
    0,
    5,
    2,
    2,
    2,
    false,
    true,
    false,
    2,
    1
};

struct GameState g_game_state;

char *g_card_suits[4] = {
    "Hearts",
    "Clubs",
    "Diamonds",
    "Spades"
};

char *g_poker_hand_names[GAME_POKER_HAND_COUNT] = 
{
    "",                 // GAME_POKER_HAND_NONE
    "High card",        // GAME_POKER_HAND_HIGH_CARD
    "Pair",             // GAME_POKER_HAND_PAIR
    "Two pair",         // GAME_POKER_HAND_TWO_PAIR
    "Three of a kind",  // GAME_POKER_HAND_THREE_OF_A_KIND
    "Straight",         // GAME_POKER_HAND_STRAIGHT
    "Flush",            // GAME_POKER_HAND_FLUSH
    "Full house",       // GAME_POKER_HAND_FULL_HOUSE
    "Four of a kind",   // GAME_POKER_HAND_FOUR_OF_A_KIND
    "Straight flush",   // GAME_POKER_HAND_STRAIGHT_FLUSH
    "Five of a kind",   // GAME_POKER_HAND_FIVE_OF_A_KIND
    "Flush house",      // GAME_POKER_HAND_FLUSH_HOUSE
    "Flush five"        // GAME_POKER_HAND_FLUSH_FIVE
};

struct JokerType g_joker_types[JOKER_TYPE_COUNT] = {
    { JOKER_TYPE_JOKER            ,"Joker",                 2, JOKER_RARITY_COMMON,   true , 0, 0, { "#2+4#- Mult", "", "", "", "", ""}},
    { JOKER_TYPE_GREEDY_JOKER     ,"Greedy Joker",          5, JOKER_RARITY_COMMON,   true , 6, 1, { "Played cards with #5Diamond#-", "suit give #2+3#- Mult", "when scored", "", "", ""}},
    { JOKER_TYPE_LUSTY_JOKER      ,"Lusty Joker",           5, JOKER_RARITY_COMMON,   true , 7, 1, { "Played cards with #3Heart#-", "suit give #2+3#- Mult", "when scored", "", "", ""}},
    { JOKER_TYPE_WRATHFUL_JOKER   ,"Wrathful Joker",        5, JOKER_RARITY_COMMON,   true , 8, 1, { "Played cards with Spade", "suit give #2+3#- Mult", "when scored", "", "", ""}},
    { JOKER_TYPE_GLUTTONOUS_JOKER ,"Gluttonous Joker",      5, JOKER_RARITY_COMMON,   true , 9, 1, { "Played cards with Club", "suit give #2+3#- Mult", "when scored", "", "", ""}},
    { JOKER_TYPE_JOLLY_JOKER      ,"Jolly Joker",           3, JOKER_RARITY_COMMON,   true , 2, 0, { "#2+8#- Mult if played", "hand contains a #5Pair#-", "", "", "", ""}},
    { JOKER_TYPE_ZANY_JOKER       ,"Zany Joker",            4, JOKER_RARITY_COMMON,   true , 3, 0, { "#2+12#- Mult if played", "hand contains a", "#5Three of a Kind#-", "", "", ""}},
    { JOKER_TYPE_MAD_JOKER        ,"Mad Joker",             4, JOKER_RARITY_COMMON,   true , 4, 0, { "#2+10#- Mult if played", "hand contains a", "#5Two Pair#-", "", "", ""}},
    { JOKER_TYPE_CRAZY_JOKER      ,"Crazy Joker",           4, JOKER_RARITY_COMMON,   true , 5, 0, { "#2+12#- Mult if played", "hand contains a", "#5Straight#-", "", "", ""}},
    { JOKER_TYPE_DROLL_JOKER      ,"Droll Joker",           4, JOKER_RARITY_COMMON,   true , 6, 0, { "#2+10#- Mult if played", "hand contains a #5Flush#-", "", "", "", ""}},
    { JOKER_TYPE_SLY_JOKER        ,"Sly Joker",             3, JOKER_RARITY_COMMON,   true , 0, 14,{ "#3+50#- Chips if played", "hand contains a #5Pair#-", "", "", "", ""}},
    { JOKER_TYPE_WILY_JOKER       ,"Wily Joker",            4, JOKER_RARITY_COMMON,   true , 1, 14,{ "#3+100#- Chips if played", "hand contains a", "#5Three of a Kind#-", "", "", ""}},
    { JOKER_TYPE_CLEVER_JOKER     ,"Clever Joker",          4, JOKER_RARITY_COMMON,   true , 2, 14,{ "#3+80#- Chips if played", "hand contains a", "#5Two Pair#-", "", "", ""}},
    { JOKER_TYPE_DEVIOUS_JOKER    ,"Devious Joker",         4, JOKER_RARITY_COMMON,   true , 3, 14,{ "#3+100#- Chips if played", "hand contains a", "#5Straight#-", "", "", ""}},
    { JOKER_TYPE_CRAFTY_JOKER     ,"Crafty Joker",          4, JOKER_RARITY_COMMON,   true , 4, 14,{ "#3+80#- Chips if played", "hand contains a #5Flush#-", "", "", "", ""}},
    { JOKER_TYPE_HALF_JOKER       ,"Half Joker",            5, JOKER_RARITY_COMMON,   true , 7, 0, { "#2+20#- Mult if played", " hand contains #53#- or", "fewer cards", "", "", ""}},
    { JOKER_TYPE_JOKER_STENCIL    ,"Joker Stencil",         8, JOKER_RARITY_UNCOMMON, true , 2, 5, { "#1#cx1#- Mult for each", "empty #5Joker#- slot.", "Joker Stencil included", "", "(Currently #1#cx#j#-)", ""}},
    { JOKER_TYPE_FOUR_FINGERS     ,"Four Fingers",          7, JOKER_RARITY_UNCOMMON, false, 6, 6, { "All #5Flushes#- and #5Straights#-", "can be made with 4 cards", "", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_MIME             ,"Mime",                  5, JOKER_RARITY_UNCOMMON, true , 4, 1, { "Retrigger all card", "#5held in hand#- abilities", "", "", "", ""}},
    { JOKER_TYPE_CREDIT_CARD      ,"Credit Card",           1, JOKER_RARITY_COMMON,   true , 5, 1, { "Go up to #2-$20#- in debt", "", "", "", "", ""}},
    { JOKER_TYPE_CEREMONIAL_DAGGER,"Ceremonial Dagger",     6, JOKER_RARITY_UNCOMMON, true , 5, 5, { "When #5Blind#- is selected,", "destroy Joker to the", "right and permanently", "add #5double#- its", "sell value to this #2Mult#-", "(Currently #2+#j#- Mult)"}},
    { JOKER_TYPE_BANNER           ,"Banner",                5, JOKER_RARITY_COMMON,   true , 1, 2, { "#3+30#- Chips for each", "remaining #5discard#-", "", "", "", ""}},
    { JOKER_TYPE_MYSTIC_SUMMIT    ,"Mystic Summit",         5, JOKER_RARITY_COMMON,   true , 2, 2, { "#2+15#- Mult when", "#50#- discards remaining", "", "", "", ""}},
    { JOKER_TYPE_MARBLE_JOKER     ,"Marble Joker",          6, JOKER_RARITY_UNCOMMON, true , 3, 2, { "Adds one #5Stone#- card", "to the deck when", "#5Blind#- is selected", "", "", ""}},
    { JOKER_TYPE_LOYALTY_CARD     ,"Loyalty Card",          5, JOKER_RARITY_UNCOMMON, false, 4, 2, { "#c#1x4#- Mult every", "#56#- hands played", "", "#j remaining", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_8_BALL           ,"8 Ball",                5, JOKER_RARITY_COMMON,   false, 0, 5, { "#41 in 4#- chance for each", "played #58#- to create", "a #6Tarot#- card when scored", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_MISPRINT         ,"Misprint",              4, JOKER_RARITY_COMMON,   true , 6, 2, { "#2+0-23#- Mult", "", "", "", "", ""}},
    { JOKER_TYPE_DUSK             ,"Dusk",                  5, JOKER_RARITY_UNCOMMON, true , 4, 7, { "Retrigger all played", "cards in #5final#-", "#5hand#- of the round", "", "", ""}},
    { JOKER_TYPE_RAISED_FIST      ,"Raised Fist",           5, JOKER_RARITY_COMMON,   true , 8, 2, { "Adds #5double#- the rank", "of #5lowest#- ranked", "card held in hand", "to Mult", "", ""}},
    { JOKER_TYPE_CHAOS_THE_CLOWN  ,"Chaos The Clown",       4, JOKER_RARITY_COMMON,   true , 1, 0, { "#51#- free #4Reroll#- per shop", "", "", "", "", ""}},
    { JOKER_TYPE_FIBONACCI        ,"Fibonacci",             8, JOKER_RARITY_UNCOMMON, true , 1, 5, { "Each played #5Ace#-, #52#-, #53#-,", "#55#-, or #58#- gives", "#2+8#- Mult when scored", "", "", ""}},
    { JOKER_TYPE_STEEL_JOKER      ,"Steel Joker",           7, JOKER_RARITY_UNCOMMON, false, 7, 2, { "Gives #c#1x0.2#- Mult for each", "#5Steel Card#- in your", "#5full deck#-", "(Currently #c#1x#j#- Mult)", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_SCARY_FACE       ,"Scary Face",            4, JOKER_RARITY_COMMON,   true , 2, 3, { "Played #5face#- cards give", " #3+30#- Chips when scored", "", "", "", ""}},
    { JOKER_TYPE_ABSTRACT_JOKER   ,"Abstract Joker",        4, JOKER_RARITY_COMMON,   true , 3, 3, { "#2+3#- Mult for each", "Joker card", "(Currently #2+#j#- Mult)", "", "", ""}},
    { JOKER_TYPE_DELAYED_GRAT     ,"Delayed Gratification", 4, JOKER_RARITY_COMMON,   false, 4, 3, { "Earn #5$2#- per #5discard#-", "if no discards are used", "by end of the round", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_HACK             ,"Hack",                  6, JOKER_RARITY_UNCOMMON, true,  5, 2, { "Retrigger each played", "#52#-, #53#-, #54#-, or #55#-", "", "", "", ""}},
    { JOKER_TYPE_PAREIDOLIA       ,"Pareidolia",            5, JOKER_RARITY_UNCOMMON, true , 6, 3, { "All cards are considered", "#5face#- cards", "", "", "", ""}},
    { JOKER_TYPE_GROS_MICHEL      ,"Gros Michel",           5, JOKER_RARITY_COMMON,   true , 7, 6, { "#2+15#- Mult", "#41 in 6#- chance this", "is destroyed at", "the end of round.", "", ""}},
    { JOKER_TYPE_EVEN_STEVEN      ,"Even Steven",           4, JOKER_RARITY_COMMON,   true , 8, 3, { "Played cards with", "#5even#- rank give", "#2+4#- Mult when scored", "", "", ""}},
    { JOKER_TYPE_ODD_TODD         ,"Odd Todd",              4, JOKER_RARITY_COMMON,   true , 9, 3, { "Played cards with", "#5odd#- rank give", "#3+31#- Chips when scored", "", "", ""}},
    { JOKER_TYPE_SCHOLAR          ,"Scholar",               4, JOKER_RARITY_COMMON,   true , 0, 4, { "Played #5Aces#- give", "#3+20#- Chips and", "#2+4#- Mult when scored", "", "", ""}},
    { JOKER_TYPE_BUSINESS_CARD    ,"Business Card",         4, JOKER_RARITY_COMMON,   true , 1, 4, { "Played #5face#- cards have", " a #41 in 2#- chance", "to give #5$2#- when scored", "", "", ""}},
    { JOKER_TYPE_SUPERNOVA        ,"Supernova",             5, JOKER_RARITY_COMMON,   true , 2, 4, { "Adds the number of times", "#5poker hand#- has been", "played this run to Mult", "", "", ""}},
    { JOKER_TYPE_RIDE_THE_BUS     ,"Ride The Bus",          6, JOKER_RARITY_COMMON,   true,  1, 6, { "This Joker gains", "#2+1#- Mult per #5consecutive#-", "hand played without", "a scoring #5face#- card", "(Currently #2+#j#- Mult)", ""}},
    { JOKER_TYPE_SPACE_JOKER      ,"Space Joker",           5, JOKER_RARITY_UNCOMMON, true , 3, 5, { "#41 in 4#- chance to", "upgrade level of", "played #5poker hand#-", "", "", ""}},
    { JOKER_TYPE_EGG              ,"Egg",                   4, JOKER_RARITY_COMMON,   false, 0, 10,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_BURGLAR          ,"Burglar",               6, JOKER_RARITY_UNCOMMON, true , 1, 10,{ "When #5Blind#- is selected,", " gain #3+3#- Hands and", "lose #5all discards#-", "", "", ""}},
    { JOKER_TYPE_BLACKBOARD       ,"Blackboard",            6, JOKER_RARITY_UNCOMMON, false, 2, 10,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_RUNNER           ,"Runner",                5, JOKER_RARITY_COMMON,   false, 3, 10,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_ICE_CREAM        ,"Ice Cream",             5, JOKER_RARITY_COMMON,   true , 4, 10,{ "#3+#j#- Chips", "#3-5#- Chips for", "every hand played", "", "", ""}},
    { JOKER_TYPE_DNA              ,"DNA",                   8, JOKER_RARITY_RARE,     true , 5, 10,{ "If #5first hand#- of round has", "only #51#- card, add a", "permanent copy to deck", "and draw it to #5hand#-", "", ""}},
    { JOKER_TYPE_SPLASH           ,"Splash",                3, JOKER_RARITY_COMMON,   true , 6, 10,{ "Every #5played card#-", "counts in scoring", "", "", "", ""}},
    { JOKER_TYPE_BLUE_JOKER       ,"Blue Joker",            5, JOKER_RARITY_COMMON,   true,  7, 10,{ "#3+2#- Chips for each", "remaining card in #5deck#-", "", "(Currently #3+#j#- Chips)", "", ""}},
    { JOKER_TYPE_SIXTH_SENSE      ,"Sixth Sense",           6, JOKER_RARITY_UNCOMMON, false, 8, 10,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_CONSTELLATION    ,"Constellation",         6, JOKER_RARITY_UNCOMMON, false, 9, 10,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_HIKER            ,"Hiker",                 5, JOKER_RARITY_UNCOMMON, true,  0, 11,{ "Every played #5card#-", "permanently gains", "#2+5#- Chips when scored", "", "", ""}},
    { JOKER_TYPE_FACELESS_JOKER   ,"Faceless Joker",        4, JOKER_RARITY_COMMON,   true , 1, 11,{ "Earn #5$5#- if #53#- or more", "#5face cards#- are", "discarded at", "the same time", "", ""}},
    { JOKER_TYPE_GREEN_JOKER      ,"Green Joker",           4, JOKER_RARITY_COMMON,   true , 2, 11,{ "#2+1#- Mult per hand played", "#2-1#- Mult per discard", "(Currently #2+#j#- Mult)", "", "", ""}},
    { JOKER_TYPE_SUPERPOSITION    ,"Superposition",         4, JOKER_RARITY_COMMON,   true , 3, 11,{ "Create a #6Tarot#- card", "if poker hand contains", "an #5Ace#- and a #5Straight#-", "", "", ""}},
    { JOKER_TYPE_TO_DO_LIST       ,"To Do List",            4, JOKER_RARITY_COMMON,   true , 4, 11,{ "Earn #5$4#- if", " poker hand is a", "#5#j#-", "", "(poker hand changes", "at end of round)"}},
    { JOKER_TYPE_CAVENDISH        ,"Cavendish",             4, JOKER_RARITY_COMMON,   true , 5, 11,{ "#c#1x3#- Mult", "#41 in 1000#- chance this", "card is destroyed", "at the end of round", "", ""}},
    { JOKER_TYPE_CARD_SHARP       ,"Card Sharp",            6, JOKER_RARITY_UNCOMMON, false, 6, 11,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_RED_CARD         ,"Red Card",              5, JOKER_RARITY_COMMON,   false, 7, 11,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_MADNESS          ,"Madness",               7, JOKER_RARITY_UNCOMMON, false, 8, 11,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_SQUARE_JOKER     ,"Square Joker",          4, JOKER_RARITY_COMMON,   false, 9, 11,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_SEANCE           ,"Seance",                6, JOKER_RARITY_UNCOMMON, false, 0, 12,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_RIFF_RAFF        ,"Riff Raff",             6, JOKER_RARITY_COMMON,   true , 1, 12,{ "When #5Blind#- is selected,", " create #52#- #3Common#- #5Jokers#-", "", "", "", ""}},
    { JOKER_TYPE_VAMPIRE          ,"Vampire",               7, JOKER_RARITY_UNCOMMON, false, 2, 12,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_SHORTCUT         ,"Shortcut",              7, JOKER_RARITY_UNCOMMON, false, 3, 12,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_HOLOGRAM         ,"Hologram",              7, JOKER_RARITY_UNCOMMON, false, 4, 12,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_VAGABOND         ,"Vagabond",              8, JOKER_RARITY_RARE,     true , 5, 12,{ "Create a #6Tarot#- card", "if hand is played", "with #5$4#- or less", "", "", ""}},
    { JOKER_TYPE_BARON            ,"Baron",                 8, JOKER_RARITY_RARE,     true , 6, 12,{ "Each #5King#- held", "in hand gives", "#c#1x1.5#- Mult", "", "", ""}},
    { JOKER_TYPE_CLOUD_9          ,"Cloud 9",               7, JOKER_RARITY_UNCOMMON, false, 7, 12,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_ROCKET           ,"Rocket",                6, JOKER_RARITY_UNCOMMON, true , 8, 12,{ "Earn #5$#j#- at end", "of round. Payout", "increases by #5$2#-", "when #5Boss Blind#-", "is defeated", ""}},
    { JOKER_TYPE_OBELISK          ,"Obelisk",               8, JOKER_RARITY_RARE,     false, 9, 12,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_MIDAS_MASK       ,"Midas Mask",            7, JOKER_RARITY_UNCOMMON, true , 0, 13,{ "All played #5face#- cards", "become #5Gold#- cards", "when scored", "", "", ""}},
    { JOKER_TYPE_LUCHADOR         ,"Luchador",              5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_PHOTOGRAPH       ,"Photograph",            5, JOKER_RARITY_COMMON,   true , 2, 13,{ "First played #5face#-", "card gives #c#1x2#- Mult", "when scored", "", "", ""}},
    { JOKER_TYPE_GIFT_CARD        ,"Gift Card",             6, JOKER_RARITY_UNCOMMON, false, 3, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_TURTLE_BEAN      ,"Turtle Bean",           6, JOKER_RARITY_UNCOMMON, false, 4, 13,{ "#5+5#- hand size,", "reduces by #21#-", "each round", "", "", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_EROSION          ,"Erosion",               6, JOKER_RARITY_UNCOMMON, false, 5, 13,{ "#2+4#- Mult for each", "card below", "#5[the deck's starting size]#-", "in your full deck", "(Currently #2+#j#- Mult)", ""}}, // NOT IMPLEMENTED
    { JOKER_TYPE_RESERVED_PARKING ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_MAIL_IN_REBATE   ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_TO_THE_MOON      ,"To the Moon",           5, JOKER_RARITY_UNCOMMON, true , 8, 13,{ "Earn an extra #5$1#-", "of interest for", "every #5$5#- you have", "at end of round", "", ""}},
    { JOKER_TYPE_HALLUCINATION    ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_FORTUNE_TELLER   ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_JUGGLER          ,"Juggler",               4, JOKER_RARITY_COMMON,   true , 0,  1,{ "#5+1#- hand size", "", "", "", "", ""}},
    { JOKER_TYPE_DRUNKARD         ,"Drunkard",              4, JOKER_RARITY_COMMON,   true , 1,  1,{ "#2+1#- discard", "each round", "", "", "", ""}},
    { JOKER_TYPE_STONE_JOKER      ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_GOLDEN_JOKER     ,"Golden Joker",          6, JOKER_RARITY_COMMON,   true,  9,  2,{ "Earn #5$4#-", "at end of round", "", "", "", ""}},
    { JOKER_TYPE_LUCKY_CAT        ,"Lucky Cat",             6, JOKER_RARITY_UNCOMMON, false, 5, 14,{ "This Joker gains", "#c#1x0.25#- Mult every time", "a #5Lucky#- card", "#4successfully#- triggers", "(Currently #c#1x#j#- Mult)", ""}},
    { JOKER_TYPE_BASEBALL_CARD    ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_BULL             ,"Bull",                  6, JOKER_RARITY_UNCOMMON, true , 7, 14,{ "#3+2#- Chips for each", "#5$1#- you have", "(Currently #3+#j#- Chips)", "", "", ""}},
    { JOKER_TYPE_DIET_COLA        ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_TRADING_CARD     ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_FLASH_CARD       ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_POPCORN          ,"Popcorn",               5, JOKER_RARITY_COMMON,   true , 1, 15,{ "#2+#j#- Mult", "#2-4#- Mult per", "round played", "", "", ""}}, 
    { JOKER_TYPE_SPARE_TROUSERS   ,"Spare Trousers",        6, JOKER_RARITY_UNCOMMON, true , 4, 15,{ "This Joker gains", "#2+2#- Mult if played hand", "contains a #5Two Pair#-", "(Currently #2+#j#- Mult)", "", ""}},
    { JOKER_TYPE_ANCIENT_JOKER    ,"Ancient Joker",         8, JOKER_RARITY_RARE,     true , 7, 15,{ "Each played card with", "#5#j#- gives", "#c#1x1.5#- Mult when scored", "", "(suit changes at", "end of round)"}},
    { JOKER_TYPE_RAMEN            ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_WALKIE_TALKIE    ,"Walkie Talkie",         4, JOKER_RARITY_COMMON,   true , 8, 15,{ "Each played #510#- or #54#-", "gives #3+10#- Chips and", "#2+4#- Mult when scored", "", "", ""}},
    { JOKER_TYPE_SELTZER          ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_CASTLE           ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_SMILEY_FACE      ,"Smiley Face",           4, JOKER_RARITY_COMMON,   true,  6, 15,{ "Played #5face#- cards", "give #2+5#- Mult", "when scored", "", "", ""}}, 
    { JOKER_TYPE_CAMPFIRE         ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_GOLDEN_TICKET    ,"Golden Ticket",         5, JOKER_RARITY_COMMON,   false, 5,  3,{ "Played #5Gold#- cards", "earn #5$4#- when scored", "", "", "", ""}},
    { JOKER_TYPE_MR_BONES         ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_ACROBAT          ,"Acrobat",               6, JOKER_RARITY_UNCOMMON, true,  2,  1,{ "#1#cx3#- Mult on", "#5final hand#-", "of round", "", "", ""}},
    { JOKER_TYPE_SOCK_AND_BUSKIN  ,"Sock and Buskin",       6, JOKER_RARITY_UNCOMMON, true,  3,  1,{ "Retrigger all played", "#5face#- cards", "", "", "", ""}},
    { JOKER_TYPE_SWASHBUCKLER     ,"Swashbuckler",          4, JOKER_RARITY_COMMON,   true,  9,  5,{ "Adds the sell value", "of all other owned", "#5Jokers#- to Mult", "", "(Currently #2+#j#- Mult)", ""}},
    { JOKER_TYPE_TROUBADOUR       ,"Troubador",             6, JOKER_RARITY_UNCOMMON, true,  0,  2,{ "#5+2#- hand size,", "#3-1#- hand per round", "", "", "", ""}},
    { JOKER_TYPE_CERTIFICATE      ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_SMEARED_JOKER    ,"Smeared Joker",         7, JOKER_RARITY_UNCOMMON, true , 4,  6,{ "#5Hearts#- and #5Diamonds#-", "count as the same suit,", "#5Spades#- and #5Clubs#-", "count as the same suit", "", ""}},
    { JOKER_TYPE_THROWBACK        ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_HANGING_CHAD     ,"Hanging Chad",          4, JOKER_RARITY_COMMON  , true , 9,  6,{ "Retrigger #5first#- played", "card used in scoring", "#52#- additional times", "", "", ""}},
    { JOKER_TYPE_ROUGH_GEM        ,"Rough Gem",             7, JOKER_RARITY_UNCOMMON, true , 9,  7,{ "Played cards with", "#5Diamond#- suit earn", "#5$1#- when scored ", "", "", ""}},
    { JOKER_TYPE_BLOODSTONE       ,"Bloodstone",            7, JOKER_RARITY_UNCOMMON, true , 0,  8,{ "#41 in 2#- chance for", "played cards with", "#5Heart#- suit to give", "#1#cx1.5#- Mult when scored", "", ""}},
    { JOKER_TYPE_ARROWHEAD        ,"Arrowhead",             7, JOKER_RARITY_UNCOMMON, true , 1,  8,{ "Played cards with", "#5Spade#- suit give", "#3+50#- Chips when scored", "", "", ""}},
    { JOKER_TYPE_ONYX_AGATE       ,"Onyx Agate",            7, JOKER_RARITY_UNCOMMON, true , 2,  8,{ "Played cards with", "#5Club#- suit give", "#2+7#- Mult when scored", "", "", ""}},
    { JOKER_TYPE_GLASS_JOKER      ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_SHOWMAN          ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_FLOWERPOT        ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_BLUEPRINT        ,"Blueprint",            10, JOKER_RARITY_RARE,     true , 0,  3,{ "Copies ability of", "#5Joker#- to the right", "", "", "", ""}},
    { JOKER_TYPE_WEE_JOKER        ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_MERRY_ANDY       ,"Merry Andy",            7, JOKER_RARITY_UNCOMMON, false, 8,  0,{ "#2+3#- discards each", "round,", "#2-1#- hand size", "", "", ""}},
    { JOKER_TYPE_OOPS_ALL_6S      ,"Oops! All 6s",          4, JOKER_RARITY_UNCOMMON, true , 5,  6,{ "Doubles all #5listed#-", "probabilities", "", "", "", ""}},
    { JOKER_TYPE_THE_IDOL         ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_SEEING_DOUBLE    ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_MATADOR          ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_HIT_THE_ROAD     ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_THE_DUO          ,"The Duo",               8, JOKER_RARITY_RARE,     true,  5,  4,{ "#1#cx2#- Mult if played", "hand contains a #5Pair#-", "", "", "", ""}},
    { JOKER_TYPE_THE_TRIO         ,"The Trio",              8, JOKER_RARITY_RARE,     true,  6,  4,{ "#1#cx3#- Mult if played", "hand contains a", "#5Three of a Kind#-", "", "", ""}},
    { JOKER_TYPE_THE_FAMILY       ,"The Family",            8, JOKER_RARITY_RARE,     true,  7,  4,{ "#1#cx4#- Mult if played", "hand contains a", "#5Four of a Kind#-", "", "", ""}},
    { JOKER_TYPE_THE_ORDER        ,"The Order",             8, JOKER_RARITY_RARE,     true,  8,  4,{ "#1#cx3#- Mult if played", "hand contains a", "#5Straight#-", "", "", ""}},
    { JOKER_TYPE_THE_TRIBE        ,"The Tribe",             8, JOKER_RARITY_RARE,     true,  9,  4,{ "#1#cx2#- Mult if played", "hand contains a", "#5Flush#-", "", "", ""}},
    { JOKER_TYPE_STUNTMAN         ,"Stuntman",              7, JOKER_RARITY_RARE,     true,  8,  6,{ "#3+250#- Chips,", "#5-2#- hand size", "", "", "", ""}},
    { JOKER_TYPE_INVISIBLE_JOKER  ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_BRAINSTORM       ,"Brainstorm",           10, JOKER_RARITY_RARE,     true,  7,  7,{ "Copies the ability", "of leftmost #5Joker#-", "", "", "", ""}},
    { JOKER_TYPE_SATELLITE        ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_SHOOT_THE_MOON   ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_DRIVERS_LICENSE  ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_CARTOMANCER      ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_ASTRONOMER       ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_BURNT_JOKER      ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_BOOTSTRAPS       ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_CANIO            ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_TRIBOULET        ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_YORICK           ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_CHICOT           ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED 
    { JOKER_TYPE_PARKEO           ,"name",                  5, JOKER_RARITY_UNCOMMON, false, 1, 13,{ "test", "test", "test", "", "", ""}}  // NOT IMPLEMENTED 
};

struct PlanetType g_planet_types[PLANET_TYPE_COUNT] = { 
    { PLANET_TYPE_PLUTO   , "Pluto",    GAME_POKER_HAND_HIGH_CARD,       1, 10, 8, 3},
    { PLANET_TYPE_MERCURY , "Mercury",  GAME_POKER_HAND_PAIR,            1, 15, 0, 3},
    { PLANET_TYPE_URANUS  , "Uranus",   GAME_POKER_HAND_TWO_PAIR,        1, 20, 6, 3},
    { PLANET_TYPE_VENUS   , "Venus",    GAME_POKER_HAND_THREE_OF_A_KIND, 2, 20, 1, 3},
    { PLANET_TYPE_SATURN  , "Saturn",   GAME_POKER_HAND_STRAIGHT,        4, 30, 5, 3},
    { PLANET_TYPE_JUPITER , "Jupiter",  GAME_POKER_HAND_FLUSH,           2, 15, 4, 3},
    { PLANET_TYPE_EARTH   , "Earth",    GAME_POKER_HAND_FULL_HOUSE,      2, 25, 2, 3},
    { PLANET_TYPE_MARS    , "Mars",     GAME_POKER_HAND_FOUR_OF_A_KIND,  3, 30, 3, 3},
    { PLANET_TYPE_NEPTUNE , "Neptune",  GAME_POKER_HAND_STRAIGHT_FLUSH,  4, 40, 7, 3},
    { PLANET_TYPE_PLANET_X, "Planet X", GAME_POKER_HAND_FIVE_OF_A_KIND,  3, 35, 9, 2},
    { PLANET_TYPE_CERES    ,"Ceres",    GAME_POKER_HAND_FLUSH_HOUSE,     4, 40, 8, 2},
    { PLANET_TYPE_ERIS     ,"Eris",     GAME_POKER_HAND_FLUSH_FIVE,      3, 50, 3, 2}
};

struct TarotType g_tarot_types[TAROT_TYPE_COUNT] = {
    { TAROT_TYPE_FOOL         , "The Fool",             0, 0, 0, 0, { "Creates a copy of the", "last Tarot or", "Planet card used.", "(#5#t#-)" } },
    { TAROT_TYPE_MAGICIAN     , "The Magician",         1, 2, 1, 0, { "Enhances #52#- selected", "cards to #5Lucky Cards#-", "", "" } },
    { TAROT_TYPE_PRIESTESS    , "The High Priestess",   0, 0, 2, 0, { "Creates up to #52#- random", "Planet cards", "", "" } },
    { TAROT_TYPE_EMPRESS      , "The Empress",          1, 2, 3, 0, { "Enhances #52#- selected", "cards to Mult Cards", "", "" } },
    { TAROT_TYPE_EMPEROR      , "The Emperor",          0, 0, 4, 0, { "Creates up to", "#52#- random #6Tarot#- cards", "", "" } },
    { TAROT_TYPE_HIEROPHANT   , "The Hierophant",       1, 2, 5, 0, { "Enhances #52#- selected", "cards to #5Bonus Cards#-", "", "" } },
    { TAROT_TYPE_LOVERS       , "The Lovers",           1, 1, 6, 0, { "Enhances #51#- selected", "card into a #5Wild Card#-", "", "" } },
    { TAROT_TYPE_CHARIOT      , "The Chariot",          1, 1, 7, 0, { "Enhances #51#- selected", "card into a #5Steel Card#-", "", "" } },
    { TAROT_TYPE_JUSTICE      , "Justice",              1, 1, 8, 0, { "Enhances #51#- selected", "card into a #5Glass Card#-", "", "" } },
    { TAROT_TYPE_HERMIT       , "The Hermit",           0, 0, 9, 0, { "Doubles money", "", "(Max of #5$20#-)", "" } },
    { TAROT_TYPE_WHEEL_FORTUNE, "The Wheel Fortune",    0, 0, 0, 1, { "#41 in 4#- chance to add", "Foil, Holographic,", "or Polychrome edition", "to a random #5Joker#-" } },
    { TAROT_TYPE_STRENGTH     , "Strength",             1, 2, 1, 1, { "Increases rank of up", "to #52#- selected cards by #51#-", "", "" } },
    { TAROT_TYPE_HANGED_MAN   , "Hanged Man",           1, 2, 2, 1, { "Destroys up to", "#52#- selected cards", "", "" } },
    { TAROT_TYPE_DEATH        , "Death",                2, 2, 3, 1, { "Select #52#- cards,", "convert the #5left#- card", "into the #5right#- card", "" } },
    { TAROT_TYPE_TEMPERANCE   , "Temperance",           0, 0, 4, 1, { "Gives the total", "sell value of all", "current #5Jokers#-", "(Max of #5$50#-)" } },
    { TAROT_TYPE_DEVIL        , "The Devil",            1, 1, 5, 1, { "Enhances #51#- selected", "card into a #5Gold Card#-", "", "" } },
    { TAROT_TYPE_TOWER        , "The Tower",            1, 1, 6, 1, { "Enhances #51#- selected", "card into a #5Stone Card#-", "", "" } },
    { TAROT_TYPE_STAR         , "The Star",             1, 3, 7, 1, { "Converts up to #53#-", "selected cards", "to #5Diamonds#-", "" } },
    { TAROT_TYPE_MOON         , "The Moon",             1, 3, 8, 1, { "Converts up to #53#-", "selected cards", "to #5Clubs#-", "" } },
    { TAROT_TYPE_SUN          , "The Sun",              1, 3, 9, 1, { "Converts up to #53#-", "selected cards", "to #5Hearts#-", "" } },
    { TAROT_TYPE_JUDGEMENT    , "Judgement",            0, 0, 0, 2, { "Creates a random", "#5Joker#- card", "", "" } },
    { TAROT_TYPE_WORLD        , "The World",            1, 3, 1, 2, { "Converts up to #53#-", "selected cards", "to #Spades#-", "" } }
};

struct SpectralType g_spectral_types[SPECTRAL_TYPE_COUNT] = {
    { SPECTRAL_TYPE_FAMILIAR   , "Familiar",     0, 0, 0, 4, { "Destroy #51#- random card", "add #53#- random Enhanced", "face cards to your hand", "" } },
    { SPECTRAL_TYPE_GRIM       , "Grim",         0, 0, 1, 4, { "Destroy #51#- random card", "add #52#- random Enhanced", "Aces to your hand", "" } },
    { SPECTRAL_TYPE_INCANTATION, "Incantation",  0, 0, 2, 4, { "Destroy #51#- random card", "add #54#- random Enhanced", "numbered cards to your hand", "" } },
    { SPECTRAL_TYPE_TALISMAN   , "Talisman",     1, 1, 3, 4, { "Add a #5Gold Seal#-", "to #51#- selected card", "in your hand", "" } },
    { SPECTRAL_TYPE_AURA       , "Aura",         1, 1, 4, 4, { "Add Foil, Holographic", "or Polychrome edition", "to #51#- selected card", "" } },
    { SPECTRAL_TYPE_WRAITH     , "Wraith",       0, 0, 5, 4, { "Creates a random", "#5Rare Joker#-", "sets money to #5$0#-", "" } },
    { SPECTRAL_TYPE_SIGIL      , "Sigil",        0, 0, 6, 4, { "Converts all cards", "in hand to a single", "random #5suit#-", "" } },
    { SPECTRAL_TYPE_OUIJA      , "Ouija",        0, 0, 7, 4, { "Converts all cards", "in hand to a single", "random #5rank#-", "#2-1#- hand size" } },
    { SPECTRAL_TYPE_ECTOPLASM  , "Ectoplasm",    0, 0, 8, 4, { "Add #5Negative#- to", "a random Joker,", "#2-1#- hand size", "" } },
    { SPECTRAL_TYPE_IMMOLATE   , "Immolate",     0, 0, 9, 4, { "Destroys #55#- random", "cards in hand,", "gain #5$20#-", "" } },
    { SPECTRAL_TYPE_ANKH       , "Ankh",         0, 0, 0, 5, { "Create a copy of a", "random #5Joker#-, destroy", "all other Jokers", "" } },
    { SPECTRAL_TYPE_DEJA_VU    , "Deja Vu",      1, 1, 1, 5, { "Add a #5Red Seal#-", "to #51#- selected card", "in your hand", "" } },
    { SPECTRAL_TYPE_HEX        , "Hex",          0, 0, 2, 5, { "Add #5Polychrome#- to", "a random Joker, destroy", "all other Jokers", "" } },
    { SPECTRAL_TYPE_TRANCE     , "Trance",       1, 1, 3, 5, { "Add a #5Blue Seal#-", "to #51#- selected card", "in your hand", "" } },
    { SPECTRAL_TYPE_MEDIUM     , "Medium",       1, 1, 4, 5, { "Add a #5Purple Seal#-", "to #51#- selected card", "in your hand", "" } },
    { SPECTRAL_TYPE_CRYPTID    , "Cryptid",      1, 1, 5, 5, { "Create #52#- copies of", "#51#- selected card", "in your hand", "" } },
    { SPECTRAL_TYPE_SOUL       , "The Soul",     0, 0, 2, 2, { "Creates a", "#5Legendary Joker#-", "(Must have room)", "" } },
    { SPECTRAL_TYPE_BLACK_HOLE , "Black Hole",   0, 0, 9, 3, { "Upgrade every", "#5poker hand#-", "by #51#- level", "" } }
};

struct BoosterPackType g_booster_types[BOOSTER_PACK_TYPE_COUNT][BOOSTER_PACK_SIZE_COUNT][BOOSTER_PACK_MAX_IMAGES] = {
    {
        {
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_NORMAL, 0, 0, 6, "Standard Pack", { "Choose 1 of up to 3", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_NORMAL, 1, 1, 6, "Standard Pack", { "Choose 1 of up to 3", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_NORMAL, 2, 2, 6, "Standard Pack", { "Choose 1 of up to 3", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_NORMAL, 3, 3, 6, "Standard Pack", { "Choose 1 of up to 3", "Playing cards to", "add to your deck" }}
        },
        {
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_JUMBO, 0, 0, 7, "Jumbo Standard Pack", { "Choose 1 of up to 5", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_JUMBO, 1, 1, 7, "Jumbo Standard Pack", { "Choose 1 of up to 5", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_JUMBO, 2, 0, 7, "Jumbo Standard Pack", { "Choose 1 of up to 5", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_JUMBO, 3, 1, 7, "Jumbo Standard Pack", { "Choose 1 of up to 5", "Playing cards to", "add to your deck" }}
        },
        {
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_MEGA, 0, 2, 7, "Mega Standard Pack", { "Choose 2 of up to 5", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_MEGA, 1, 3, 7, "Mega Standard Pack", { "Choose 2 of up to 5", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_MEGA, 2, 2, 7, "Mega Standard Pack", { "Choose 2 of up to 5", "Playing cards to", "add to your deck" }},
            {BOOSTER_PACK_TYPE_STANDARD, BOOSTER_PACK_SIZE_MEGA, 3, 3, 7, "Mega Standard Pack", { "Choose 2 of up to 5", "Playing cards to", "add to your deck" }}
        }
    },
    {
        {
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_NORMAL, 0, 0, 0, "Arcana Pack", { "Choose #51#- of up", "to #53#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_NORMAL, 1, 1, 0, "Arcana Pack", { "Choose #51#- of up", "to #53#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_NORMAL, 2, 2, 0, "Arcana Pack", { "Choose #51#- of up", "to #53#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_NORMAL, 3, 3, 0, "Arcana Pack", { "Choose #51#- of up", "to #53#- #6Tarot#- cards", "to be used immediately" }}
        },
        {
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_JUMBO, 0, 0, 2, "Jumbo Arcana Pack", { "Choose #51#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_JUMBO, 1, 1, 2, "Jumbo Arcana Pack", { "Choose #51#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_JUMBO, 2, 0, 2, "Jumbo Arcana Pack", { "Choose #51#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_JUMBO, 3, 1, 2, "Jumbo Arcana Pack", { "Choose #51#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }}
        },
        {
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_MEGA, 0, 2, 2, "Mega Arcana Pack", { "Choose #52#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_MEGA, 1, 3, 2, "Mega Arcana Pack", { "Choose #52#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_MEGA, 2, 2, 2, "Mega Arcana Pack", { "Choose #52#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_ARCANA, BOOSTER_PACK_SIZE_MEGA, 3, 3, 2, "Mega Arcana Pack", { "Choose #52#- of up", "to #55#- #6Tarot#- cards", "to be used immediately" }}
        }
    },
    {
        {
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_NORMAL, 0, 0, 1, "Celestial Pack", { "Choose #51#- of up", "to #53#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_NORMAL, 1, 1, 1, "Celestial Pack", { "Choose #51#- of up", "to #53#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_NORMAL, 2, 2, 1, "Celestial Pack", { "Choose #51#- of up", "to #53#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_NORMAL, 3, 3, 1, "Celestial Pack", { "Choose #51#- of up", "to #53#- #7Planet#- cards", "to be used immediately" }}
        },
        {
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_JUMBO, 0, 0, 3, "Jumbo Celestial Pack", { "Choose #51#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_JUMBO, 1, 1, 3, "Jumbo Celestial Pack", { "Choose #51#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_JUMBO, 2, 0, 3, "Jumbo Celestial Pack", { "Choose #51#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_JUMBO, 3, 1, 3, "Jumbo Celestial Pack", { "Choose #51#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }}
        },
        {
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_MEGA, 0, 2, 3, "Mega Celestial Pack", { "Choose #52#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_MEGA, 1, 3, 3, "Mega Celestial Pack", { "Choose #52#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_MEGA, 2, 2, 3, "Mega Celestial Pack", { "Choose #52#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_CELESTIAL, BOOSTER_PACK_SIZE_MEGA, 3, 3, 3, "Mega Celestial Pack", { "Choose #52#- of up", "to #55#- #7Planet#- cards", "to be used immediately" }}
        }
    },
    {
        {
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_NORMAL, 0, 0, 8, "Buffoon Pack", { "Choose #51#- of up", "to #52#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_NORMAL, 1, 1, 8, "Buffoon Pack", { "Choose #51#- of up", "to #52#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_NORMAL, 2, 0, 8, "Buffoon Pack", { "Choose #51#- of up", "to #52#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_NORMAL, 3, 1, 8, "Buffoon Pack", { "Choose #51#- of up", "to #52#- Joker cards", "" }}
        },
        {
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_JUMBO, 0, 2, 8, "Jumbo Buffoon Pack", { "Choose #51#- of up", "to #54#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_JUMBO, 1, 2, 8, "Jumbo Buffoon Pack", { "Choose #51#- of up", "to #54#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_JUMBO, 2, 2, 8, "Jumbo Buffoon Pack", { "Choose #51#- of up", "to #54#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_JUMBO, 3, 2, 8, "Jumbo Buffoon Pack", { "Choose #51#- of up", "to #54#- Joker cards", "" }}
        },
        {
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_MEGA, 0, 3, 8, "Mega Buffoon Pack", { "Choose #52#- of up", "to #54#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_MEGA, 1, 3, 8, "Mega Buffoon Pack", { "Choose #52#- of up", "to #54#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_MEGA, 2, 3, 8, "Mega Buffoon Pack", { "Choose #52#- of up", "to #54#- Joker cards", "" }},
            {BOOSTER_PACK_TYPE_BUFFOON, BOOSTER_PACK_SIZE_MEGA, 3, 3, 8, "Mega Buffoon Pack", { "Choose #52#- of up", "to #54#- Joker cards", "" }}
        }
    },  
    {
        {
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_NORMAL, 0, 0, 4, "Spectral Pack", { "Choose #51#- of up", "to #52#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_NORMAL, 1, 1, 4, "Spectral Pack", { "Choose #51#- of up", "to #52#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_NORMAL, 2, 0, 4, "Spectral Pack", { "Choose #51#- of up", "to #52#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_NORMAL, 3, 1, 4, "Spectral Pack", { "Choose #51#- of up", "to #52#- #3Spectral#- cards", "to be used immediately" }}
        },
        {
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_JUMBO, 0, 2, 4, "Jumbo Spectral Pack", { "Choose #51#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_JUMBO, 1, 2, 4, "Jumbo Spectral Pack", { "Choose #51#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_JUMBO, 2, 2, 4, "Jumbo Spectral Pack", { "Choose #51#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_JUMBO, 3, 2, 4, "Jumbo Spectral Pack", { "Choose #51#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }}
        },
        {
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_MEGA, 0, 3, 4, "Mega Spectral Pack", { "Choose #52#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_MEGA, 1, 3, 4, "Mega Spectral Pack", { "Choose #52#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_MEGA, 2, 3, 4, "Mega Spectral Pack", { "Choose #52#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }},
            {BOOSTER_PACK_TYPE_SPECTRAL, BOOSTER_PACK_SIZE_MEGA, 3, 3, 4, "Mega Spectral Pack", { "Choose #52#- of up", "to #54#- #3Spectral#- cards", "to be used immediately" }}
        }
    }
};

double g_booster_packs_weights[BOOSTER_PACK_TYPE_COUNT][BOOSTER_PACK_SIZE_COUNT] = {
    // BOOSTER_PACK_SIZE_NORMAL, BOOSTER_PACK_SIZE_JUMBO, BOOSTER_PACK_SIZE_MEGA
    {4, 2, 0.5},        // BOOSTER_PACK_TYPE_STANDARD
    {4, 2, 0.5},        // BOOSTER_PACK_TYPE_ARCANA
    {4, 2, 0.5},        // BOOSTER_PACK_TYPE_CELESTIAL
    {1.2, 0.6, 0.15},   // BOOSTER_PACK_TYPE_BUFFOON
    {0.6, 0.3, 0.07}    // BOOSTER_PACK_TYPE_SPECTRAL
};

double g_booster_packs_weights_total = 0.0;

const char *g_enhancement_names[CARD_ENHANCEMENT_COUNT] = {
    "None",
    "Bonus Card",
    "Mult Card",
    "Wild Card",
    "Glass Card",
    "Steel Card",
    "Stone Card",
    "Gold Card",
    "Lucky Card"
};

const char *g_enhancement_hint[CARD_ENHANCEMENT_COUNT] = {
    "",
    "#d#1Bonus Card#-",
    "#d#1Mult Card#-",
    "#d#1Wild Card#-",
    "#d#1Glass Card#-",
    "#d#1Steel Card#-",
    "#d#1Stone Card#-",
    "#d#1Gold Card#-",
    "#d#1Lucky Card#-"
};

const char *g_edition_names[CARD_EDITION_COUNT] = {
    "Standard",
    "Foil",
    "Holographic",
    "Polychrome",
    "Negative"
};

const char *g_edition_hint[CARD_EDITION_COUNT] = {
    "",
    "#d#1Foil#- (#3+50#- Chips)",
    "#d#1Holographic#- (#2+10#- Mult)",
    "#d#1Polychrome#- (#1#cx1.5#- Mult)",
    "#d#1Negative#-"
};

const char *g_rarity_hint[JOKER_RARITY_COUNT] = {
    "#e#1Common#-",
    "#f#1Uncommon#-",
    "#g#1Rare#-",
    "#d#1Legendary#-"
};

const char *g_short_rank_name[CARD_RANK_COUNT] = {
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "J",
    "Q",
    "K",
    "A"
};

double game_get_ante_base_score()
{
    double ante_score[3][9] = {
        {100, 300, 800, 2000, 5000, 11000, 20000, 35000, 50000},
        {100, 300, 900, 2600, 8000, 20000, 36000, 60000, 100000},
        {100, 300, 1000, 3200, 9000, 25000, 60000, 110000, 200000}
    };

    return ante_score[g_game_state.ante_score_scaling][g_game_state.ante];
}

void game_init_joker(struct Joker *joker)
{
    joker->param1 = 0;
    joker->repeat = 0;
    joker->value = g_joker_types[joker->type].value;
    game_init_draw_object(&(joker->draw));
    switch(joker->type)
    {
        case JOKER_TYPE_LOYALTY_CARD:
            joker->param1 = 5;
            break;
        case JOKER_TYPE_POPCORN:
            joker->param1 = 5;
            break;
        case JOKER_TYPE_ANCIENT_JOKER:
            joker->param1 = game_util_rand(0, 3);
            break;
        case JOKER_TYPE_TO_DO_LIST:
            joker->param1 = game_util_get_random_poker_hand();
            break;
        case JOKER_TYPE_ICE_CREAM:
            joker->param1 = 20;
            break;
        case JOKER_TYPE_LUCKY_CAT:
            joker->param1 = 100;
            break;
        case JOKER_TYPE_ROCKET:
            joker->param1 = 1;
            break;
        case JOKER_TYPE_GREEN_JOKER:
            joker->param1 = 0;
            break;
        default:
            break;
    }
}

double game_get_current_blind_score()
{
    double score = game_get_ante_base_score();

    return score * (1.0 + (double)g_game_state.blind * 0.5);
}

void game_set_card_name(struct Card *card, char *name)
{
    char rank[10];

    if (card->enhancement == CARD_ENHANCEMENT_STONE)
    {
        strcpy(name, "Stone Card");
        return;
    }

    switch (card->rank)
    {
        case CARD_RANK_JACK:
            strcpy(rank, "Jack");
            break;
        case CARD_RANK_QUEEN:
            strcpy(rank, "Queen");
            break;
        case CARD_RANK_KING:
            strcpy(rank, "King");
            break;
        case CARD_RANK_ACE:
            strcpy(rank, "Ace");
            break;
        default:
            sprintf(rank, "%d", card->rank + 2);
    }

    char suit[10];

    switch (card->suit)
    {
        case CARD_SUIT_HEARTS:
            strcpy(suit, "Hearts");
            break;
        case CARD_SUIT_DIAMONDS:
            strcpy(suit, "Diamonds");
            break;
        case CARD_SUIT_CLUBS:
            strcpy(suit, "Clubs");
            break;
        case CARD_SUIT_SPADES:
            strcpy(suit, "Spades");
            break;
    }

    sprintf(name, "%s of %s", rank, suit);
}

int game_poker_hand_sort_by_rank(const void *a, const void *b)
{
    struct CardIndex card_a = *((struct CardIndex *)a);
    struct CardIndex card_b = *((struct CardIndex *)b);

    if (card_a.card == NULL) return 1;
    if (card_b.card == NULL) return -1;

    int rank_a = card_a.card->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_a.card->rank;
    int rank_b = card_b.card->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_b.card->rank;

    return rank_a - rank_b;
}

// Get selected cards poker hand
//
// cards -> Array of card pointers to get the poker hand from
// card_count -> Length of the array of card pointers
// scoring_cards -> Array of indexes, used as an output
// scoring_cards_count -> Length of the array of indexes, used as an output

int game_get_selected_poker_hand(struct Card *cards[], int card_count, int scoring_cards[], int *scoring_cards_count)
{
    int i;
    bool skip = false;

    struct CardIndex selected_cards[5];
    int current_select_card = 0;
    
    // get selected cards
    for(i = 0; i < card_count; i++)
    {
        selected_cards[current_select_card].card = cards[i];
        selected_cards[current_select_card++].index = i;
    }
    for(i = card_count; i < 5; i++)
    {
        selected_cards[current_select_card].card = NULL;
        selected_cards[current_select_card++].index = -1;
    }

    // sort selected cards by rank
    qsort((void*)selected_cards, 5, sizeof(struct CardIndex), game_poker_hand_sort_by_rank);

    // Set by default the scoring cards to all cards if card_count is 5
    if (card_count == 5)
    {
        for (int i = 0; i < 5; i++)
        {
            scoring_cards[i] = i;            
        }        
        *scoring_cards_count = 5;
    }

    // Check for flush five
    if (card_count == 5)
    {
        skip = false;
        for(i = 1; i < 5; i++)
        {
            if (!game_util_are_2_cards_same_rank(selected_cards[i].card, selected_cards[i-1].card))
            {
                skip = true;
                break;
            }
        }
        if (!game_util_are_cards_same_suit(cards, 5)) skip = true;
        if (!skip) return GAME_POKER_HAND_FLUSH_FIVE;
    }

    // Check for five of a kind
    if (card_count == 5)
    {
        skip = false;
        for(i = 1; i < 5; i++)
        {
            if (!game_util_are_2_cards_same_rank(selected_cards[i].card, selected_cards[i-1].card))
            {
                skip = true;
                break;
            }
        }
        if (!skip) return GAME_POKER_HAND_FIVE_OF_A_KIND;
    }

    // Check for flush five
    // XXYYY or XXXYY
    if (card_count == 5)
    {
        if (((game_util_are_2_cards_same_rank(selected_cards[0].card, selected_cards[1].card) &&
              game_util_are_2_cards_same_rank(selected_cards[2].card, selected_cards[3].card) &&
              game_util_are_2_cards_same_rank(selected_cards[3].card, selected_cards[4].card)) ||
            (game_util_are_2_cards_same_rank(selected_cards[0].card, selected_cards[1].card) &&
             game_util_are_2_cards_same_rank(selected_cards[1].card, selected_cards[2].card) &&
             game_util_are_2_cards_same_rank(selected_cards[3].card, selected_cards[4].card))) &&
            game_util_are_cards_same_suit(cards, 5))
        {
            return GAME_POKER_HAND_FLUSH_FIVE;
        }
    }

    // Check for straight flush
    if (card_count == 5)
    {
        skip = false;
        for(i = 1; i < 5; i++)
        {
            if (!game_util_are_cards_rank_consecutive(selected_cards[i].card, selected_cards[i-1].card))
            {
                skip = true;
                break;
            }
        }
        if (!skip && game_util_are_cards_same_suit(cards, 5)) return GAME_POKER_HAND_STRAIGHT_FLUSH;
    }

    // Check for four of a kind   
    if (card_count >= 4)
    {
        int same_rank_count = 0;
        for(i = 1; i < card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i].card, selected_cards[i-1].card))
            {
                if (same_rank_count == 0) same_rank_count = 2; else same_rank_count++;

                if (same_rank_count == 4)
                {
                    scoring_cards[0] = selected_cards[i-3].index;
                    scoring_cards[1] = selected_cards[i-2].index;
                    scoring_cards[2] = selected_cards[i-1].index;
                    scoring_cards[3] = selected_cards[i].index;
                    *scoring_cards_count = 4;

                    if (selected_cards[0].card->enhancement == CARD_ENHANCEMENT_STONE)
                    {
                        scoring_cards[4] = selected_cards[0].index;
                        *scoring_cards_count = 5;
                    }

                    return GAME_POKER_HAND_FOUR_OF_A_KIND;
                }
            }
            else
            {
                same_rank_count = 0;
            }
        }
    }

    // Check for flush
    if (card_count == 5)
    {
        if (game_util_are_cards_same_suit(cards, 5))
        {
            return GAME_POKER_HAND_FLUSH;
        }
    }

    // Check for full house
    // XXYYY or XXXYY
    if (card_count == 5)
    {
        if ((game_util_are_2_cards_same_rank(selected_cards[0].card, selected_cards[1].card) &&
        game_util_are_2_cards_same_rank(selected_cards[2].card, selected_cards[3].card) &&
        game_util_are_2_cards_same_rank(selected_cards[3].card, selected_cards[4].card)) ||
      (game_util_are_2_cards_same_rank(selected_cards[0].card, selected_cards[1].card) &&
       game_util_are_2_cards_same_rank(selected_cards[1].card, selected_cards[2].card) &&
       game_util_are_2_cards_same_rank(selected_cards[3].card, selected_cards[4].card)))
        {
            return GAME_POKER_HAND_FULL_HOUSE;
        }
    }

    // Check for straight
    if (card_count == 5)
    {
        skip = false;
        for(i = 1; i < 5; i++)
        {
            if (!game_util_are_cards_rank_consecutive(selected_cards[i].card, selected_cards[i-1].card))
            {
                skip = true;
                break;
            }
        }
        if (!skip) return GAME_POKER_HAND_STRAIGHT;
    }

    // Check for three of a kind   
    if (card_count >= 3)
    {
        int same_rank_count = 0;
        for(i = 1; i < card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i].card, selected_cards[i-1].card))
            {
                if (same_rank_count == 0) same_rank_count = 2; else same_rank_count++;

                if (same_rank_count == 3)
                {
                    scoring_cards[0] = selected_cards[i-2].index;
                    scoring_cards[1] = selected_cards[i-1].index;
                    scoring_cards[2] = selected_cards[i].index;
                    *scoring_cards_count = 3;

                    for(int j = 0; j < card_count - 3; j++)
                    {
                        if (selected_cards[j].card->enhancement == CARD_ENHANCEMENT_STONE)
                        {
                            scoring_cards[*scoring_cards_count] = selected_cards[j].index;
                            *scoring_cards_count = *scoring_cards_count + 1;
                        }
                    }

                    return GAME_POKER_HAND_THREE_OF_A_KIND;
                }
            }
            else
            {
                same_rank_count = 0;
            }
        }
    }

    // Check for two pairs
    if (card_count >= 4)
    {
        int same_rank_count = 0;
        int pair_count = 0;
        for(i = 1; i < card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i].card, selected_cards[i-1].card))
            {
                if (same_rank_count == 0) same_rank_count = 2; else same_rank_count++;

                if (same_rank_count == 2 && pair_count == 1)
                {
                    scoring_cards[2] = selected_cards[i-1].index;
                    scoring_cards[3] = selected_cards[i].index;
                    *scoring_cards_count = 4;

                    if (selected_cards[0].card->enhancement == CARD_ENHANCEMENT_STONE)
                    {
                        scoring_cards[4] = selected_cards[0].index;
                        *scoring_cards_count = 5;
                    }

                    return GAME_POKER_HAND_TWO_PAIR;
                }
            }
            else
            {
                if (same_rank_count == 2)
                {
                    if (pair_count == 0)
                    {
                        scoring_cards[0] = selected_cards[i-2].index;
                        scoring_cards[1] = selected_cards[i-1].index;
                    }
                    pair_count++;
                }
                same_rank_count = 0;
            }
        }
    }

    // Check for pair
    if (card_count >= 2)
    {
        for(i = 1; i < card_count; i++)
        {
            if (game_util_are_2_cards_same_rank(selected_cards[i].card, selected_cards[i-1].card))
            {
                scoring_cards[0] = selected_cards[i-1].index;
                scoring_cards[1] = selected_cards[i].index;
                *scoring_cards_count = 2;

                for(int j = 0; j < card_count - 2; j++)
                {
                    if (selected_cards[j].card->enhancement == CARD_ENHANCEMENT_STONE)
                    {
                        scoring_cards[*scoring_cards_count] = selected_cards[j].index;
                        *scoring_cards_count = *scoring_cards_count + 1;
                    }
                }

                return GAME_POKER_HAND_PAIR;
            }
        }
    }

    // Check high card
    if (card_count > 0)
    {
        scoring_cards[0] = selected_cards[card_count-1].index;
        *scoring_cards_count = 1;

        for(int j = 0; j < card_count - 1; j++)
        {
            if (selected_cards[j].card->enhancement == CARD_ENHANCEMENT_STONE)
            {
                scoring_cards[*scoring_cards_count] = selected_cards[j].index;
                *scoring_cards_count = *scoring_cards_count + 1;
            }
        }

        return GAME_POKER_HAND_HIGH_CARD;
    }

    return GAME_POKER_HAND_NONE;
}

void game_discard_card(int i)
{
    for(int j = i+1; j < g_game_state.hand.card_count; j++)
    {
        g_game_state.hand.cards[j - 1] = g_game_state.hand.cards[j];
    }

    g_game_state.hand.card_count--;
}

void game_set_card_hand_positions()
{
    // set cards positions in hand
    for(int i = 0; i < g_game_state.hand.card_count; i++)
    {
        g_game_state.hand.cards[i]->draw.initial_x = g_game_state.hand.cards[i]->draw.final_x = g_game_state.hand.cards[i]->draw.x = DRAW_HAND_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_HAND_WIDTH / ((float)g_game_state.hand.card_count + 1.0f);
        g_game_state.hand.cards[i]->draw.initial_y = g_game_state.hand.cards[i]->draw.final_y = g_game_state.hand.cards[i]->draw.y = g_game_state.hand.cards[i]->selected ? game_get_hand_y() - 10.0f : game_get_hand_y();
    }
}

bool game_buy_booster()
{
    struct BoosterPack *booster = &(g_game_state.shop.boosters[g_game_state.highlighted_item].booster);
    if (!game_util_can_buy(game_util_get_booster_price(booster))) return false;
    automated_event_push(AUTOMATED_EVENT_OPEN_BOOSTER, 0);
    return true;
}

float g_hand_y = DRAW_HAND_Y;

void game_set_hand_y(float y)
{
    g_hand_y = y;
}

float game_get_hand_y()
{
    return g_hand_y;
}

void game_set_initial_position(struct DrawObject *draw)
{
    draw->initial_x = draw->x;
    draw->initial_y = draw->y;
}

void game_set_card_hand_initial_positions()
{
    // set cards positions in hand
    for(int i = 0; i < g_game_state.hand.card_count; i++)
    {
        g_game_state.hand.cards[i]->draw.initial_x = g_game_state.hand.cards[i]->draw.x;
        g_game_state.hand.cards[i]->draw.initial_y = g_game_state.hand.cards[i]->draw.y;
    }
}

void game_set_card_hand_final_positions()
{
    // set cards positions in hand
    for(int i = 0; i < g_game_state.hand.card_count; i++)
    {
        g_game_state.hand.cards[i]->draw.final_x = DRAW_HAND_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_HAND_WIDTH / ((float)g_game_state.hand.card_count + 1.0f);
        g_game_state.hand.cards[i]->draw.final_y = g_game_state.hand.cards[i]->selected ? game_get_hand_y() - 10.0f : game_get_hand_y();
    }
}

void game_draw_card_into_hand()
{
    g_game_state.hand.cards[g_game_state.hand.card_count] = g_game_state.current_deck.cards[g_game_state.current_deck.card_count - 1];
    g_game_state.hand.cards[g_game_state.hand.card_count]->selected = false;
    g_game_state.hand.cards[g_game_state.hand.card_count]->draw.x = g_game_state.hand.cards[g_game_state.hand.card_count]->draw.initial_x = 400;
    g_game_state.hand.cards[g_game_state.hand.card_count]->draw.y = g_game_state.hand.cards[g_game_state.hand.card_count]->draw.initial_y = 200;
    g_game_state.hand.card_count++;
    g_game_state.current_deck.card_count--;

    // game_set_card_hand_positions();
}

void game_ingame_discard_played_hand()
{
    g_game_state.played_hand.card_count = 0;
}

bool game_shop_buy_highlighted_item(bool use_item)
{ 
    if (g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_JOKER)
    {
        if (g_game_state.shop.items[g_game_state.highlighted_item].info.joker.edition != CARD_EDITION_NEGATIVE && !game_util_is_joker_slot_available()) return false;

        struct Joker *joker = &(g_game_state.shop.items[g_game_state.highlighted_item].info.joker);

        if (!game_util_can_buy(game_util_get_joker_buy_price(joker))) return false;
        
        g_game_state.wealth -= game_util_get_joker_buy_price(joker);
        audio_play_sfx(AUDIO_SFX_COIN);

        if (!use_item) automated_event_set(AUTOMATED_EVENT_BUY_JOKER, 0);

        return true;
    }
    else if (g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_PLANET)
    {
        if (!use_item && !game_util_is_consumable_slot_available()) return false;

        struct Planet *planet = &(g_game_state.shop.items[g_game_state.highlighted_item].info.planet);

        if (!game_util_can_buy(game_util_get_planet_buy_price(planet))) return false;
        
        g_game_state.wealth -= game_util_get_planet_buy_price(planet);
        audio_play_sfx(use_item ? AUDIO_SFX_TAROT : AUDIO_SFX_COIN);

        if (!use_item) automated_event_set(AUTOMATED_EVENT_BUY_CONSUMABLE, 0);
        if (use_item) automated_event_set(AUTOMATED_EVENT_USE_PLANET, 0);

        return true;
    }
    else if (g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_TAROT)
    {
        if (!use_item && !game_util_is_consumable_slot_available()) return false;

        struct Tarot *tarot = &(g_game_state.shop.items[g_game_state.highlighted_item].info.tarot);        

        if (!use_item)
        {
            if (!game_util_can_buy(game_util_get_tarot_buy_price(tarot))) return false;
            g_game_state.wealth -= game_util_get_tarot_buy_price(tarot);
            audio_play_sfx(AUDIO_SFX_COIN);
            automated_event_set(AUTOMATED_EVENT_BUY_CONSUMABLE, 0);
        }

        if (use_item)
        {
            if (!game_util_can_tarot_be_used(tarot->type)) return false;
            if (!game_util_can_buy(game_util_get_tarot_buy_price(tarot))) return false;
            g_game_state.wealth -= game_util_get_tarot_buy_price(tarot);
            audio_play_sfx(AUDIO_SFX_TAROT);
            automated_event_set(AUTOMATED_EVENT_USE_TAROT, 0);
        }

        return true;
    }
    else if (g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_SPECTRAL)
    {
        if (!use_item && !game_util_is_consumable_slot_available()) return false;

        struct Spectral *spectral = &(g_game_state.shop.items[g_game_state.highlighted_item].info.spectral);

        if (!use_item)
        {
            if (!game_util_can_buy(game_util_get_spectral_buy_price(spectral))) return false;
            g_game_state.wealth -= game_util_get_spectral_buy_price(spectral);
            audio_play_sfx(AUDIO_SFX_COIN);
            automated_event_set(AUTOMATED_EVENT_BUY_CONSUMABLE, 0);
        }

        if (use_item)
        {
            if (!game_util_can_spectral_be_used(spectral->type)) return false;
            if (!game_util_can_buy(game_util_get_spectral_buy_price(spectral))) return false;
            g_game_state.wealth -= game_util_get_spectral_buy_price(spectral);
            audio_play_sfx(AUDIO_SFX_TAROT);
            automated_event_set(AUTOMATED_EVENT_USE_SPECTRAL, 0);
        }

        return true;
    }

    return false;
}

void game_discard_selected_cards()
{
    if (g_game_state.selected_cards_count > 0 && g_game_state.current_discards > 0)
    {
        g_game_state.current_poker_hand = GAME_POKER_HAND_NONE;
        g_game_state.current_base_chips = 0;
        g_game_state.current_base_mult = 0;

        g_game_state.stats.cards_discarded += g_game_state.selected_cards_count;

        automated_event_set(AUTOMATED_EVENT_DISCARD, 0);
        g_game_state.current_discards--;
    }
}

void game_set_shop_item_position()
{
    for(int i = 0; i < g_game_state.shop.total_items; i++)
    {
        if (g_game_state.shop.items[i].available)
        {
            struct DrawObject *draw = NULL;
            switch (g_game_state.shop.items[i].type)
            {
                case ITEM_TYPE_JOKER:
                {
                    draw = &(g_game_state.shop.items[i].info.joker.draw);
                    break;
                }                
                case ITEM_TYPE_PLANET:
                {
                    draw = &(g_game_state.shop.items[i].info.planet.draw);
                    break;
                }
                case ITEM_TYPE_TAROT:
                {
                    draw = &(g_game_state.shop.items[i].info.tarot.draw);
                    break;
                }
                case ITEM_TYPE_SPECTRAL:
                {
                    draw = &(g_game_state.shop.items[i].info.spectral.draw);
                    break;
                }
                case ITEM_TYPE_CARD:
                {
                    draw = &(g_game_state.shop.items[i].info.card.draw);
                    break;
                }
            }
            game_init_draw_object(draw);
            draw->initial_x = draw->final_x = draw->x = DRAW_SHOP_SINGLE_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_SHOP_SINGLE_WIDTH / ((float)g_game_state.shop.total_items + 1.0f);
            draw->initial_y = draw->final_y = draw->y = DRAW_SHOP_SINGLE_Y;
        }
    }
}

void game_set_shop_booster_position()
{
    for(int i = 0; i < g_game_state.shop.total_boosters; i++)
    {
        if (g_game_state.shop.boosters[i].available)
        {
            struct DrawObject *draw = &(g_game_state.shop.boosters[i].booster.draw);
            game_init_draw_object(draw);
            draw->initial_x = draw->final_x = draw->x = DRAW_SHOP_BOOSTER_X - (BOOSTER_WIDTH / 2.0f) + (i + 1) * DRAW_SHOP_BOOSTER_WIDTH / ((float)g_game_state.shop.total_boosters + 1.0f);
            draw->initial_y = draw->final_y = draw->y = DRAW_SHOP_BOOSTER_Y;
        }
    }
}

void game_set_initial_final_shop_booster_item_position()
{
    for (int i = 0; i < g_game_state.shop.booster_item_count; i++)
    {
        if (g_game_state.shop.booster_items[i].available)
        {
            struct DrawObject *draw = NULL;
            switch (g_game_state.shop.booster_items[i].type)
            {
                case ITEM_TYPE_CARD:
                    draw = &(g_game_state.shop.booster_items[i].info.card.draw);
                    break;
                case ITEM_TYPE_JOKER:
                    draw = &(g_game_state.shop.booster_items[i].info.joker.draw);
                    break;
                case ITEM_TYPE_PLANET:
                    draw = &(g_game_state.shop.booster_items[i].info.planet.draw);
                    break;
                case ITEM_TYPE_TAROT:
                    draw = &(g_game_state.shop.booster_items[i].info.tarot.draw);
                    break;
                case ITEM_TYPE_SPECTRAL:
                    draw = &(g_game_state.shop.booster_items[i].info.spectral.draw);
                    break;
            }
            game_init_draw_object(draw);
            draw->initial_x = draw->final_x = draw->x = game_util_get_item_position(i, g_game_state.shop.booster_item_count, DRAW_BOOSTER_ITEMS_X, DRAW_BOOSTER_ITEMS_WIDTH, CARD_WIDTH);
            draw->initial_y = draw->final_y = draw->y = DRAW_BOOSTER_ITEMS_Y;
        }
    }
}

void game_init_booster_items()
{
    switch(g_game_state.shop.opened_booster.type)
    {
        case BOOSTER_PACK_TYPE_ARCANA:
        {
            int excluded_tarots[200];
            int excluded_tarots_count = 0;

            g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 3;
            if (g_game_state.shop.opened_booster.size != BOOSTER_PACK_SIZE_NORMAL) g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 5;
            for(int i = 0; i < g_game_state.shop.booster_item_count; i++)
            {
                g_game_state.shop.booster_items[i].available = true;
                g_game_state.shop.booster_items[i].type = ITEM_TYPE_TAROT;
                g_game_state.shop.booster_items[i].info.tarot.type =  game_util_get_new_tarot_type(excluded_tarots, excluded_tarots_count);
                g_game_state.shop.booster_items[i].info.tarot.edition = CARD_EDITION_BASE;

                excluded_tarots[excluded_tarots_count++] = g_game_state.shop.booster_items[i].info.tarot.type;
            }
            break;
        }
        case BOOSTER_PACK_TYPE_BUFFOON:
        {
            int excluded_joker_types[200];
            int excluded_joker_types_count = 0;
            for(int j = 0; j < g_game_state.jokers.joker_count; j++)
            {
                excluded_joker_types[excluded_joker_types_count++] = g_game_state.jokers.jokers[j].type;
            }

            g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 2;
            if (g_game_state.shop.opened_booster.size != BOOSTER_PACK_SIZE_NORMAL) g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 4;
            for(int i = 0; i < g_game_state.shop.booster_item_count; i++)
            {
                g_game_state.shop.booster_items[i].available = true;
                g_game_state.shop.booster_items[i].type = ITEM_TYPE_JOKER;
                g_game_state.shop.booster_items[i].info.joker.type = game_util_get_new_joker_type(excluded_joker_types, excluded_joker_types_count, false);                
                g_game_state.shop.booster_items[i].info.joker.edition = game_util_get_shop_joker_edition();
                game_init_joker(&(g_game_state.shop.booster_items[i].info.joker));

                excluded_joker_types[excluded_joker_types_count++] = g_game_state.shop.booster_items[i].info.joker.type;
            }
            break;
        }
        case BOOSTER_PACK_TYPE_CELESTIAL:
        {
            int excluded_planets[200];
            int excluded_planets_count = 0;

            g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 3;
            if (g_game_state.shop.opened_booster.size != BOOSTER_PACK_SIZE_NORMAL) g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 5;
            for(int i = 0; i < g_game_state.shop.booster_item_count; i++)
            {
                g_game_state.shop.booster_items[i].available = true;
                g_game_state.shop.booster_items[i].type = ITEM_TYPE_PLANET;
                g_game_state.shop.booster_items[i].info.planet.type = game_util_get_new_planet_type(excluded_planets, excluded_planets_count);
                g_game_state.shop.booster_items[i].info.planet.edition = CARD_EDITION_BASE;

                excluded_planets[excluded_planets_count++] = g_game_state.shop.booster_items[i].info.planet.type;
            }
            break;
        }
        case BOOSTER_PACK_TYPE_SPECTRAL:
        {
            int excluded_spectrals[200];
            int excluded_spectrals_count = 0;

            g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 2;
            if (g_game_state.shop.opened_booster.size != BOOSTER_PACK_SIZE_NORMAL) g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 4;
            for(int i = 0; i < g_game_state.shop.booster_item_count; i++)
            {
                g_game_state.shop.booster_items[i].available = true;
                g_game_state.shop.booster_items[i].type = ITEM_TYPE_SPECTRAL;
                g_game_state.shop.booster_items[i].info.spectral.type = game_util_get_new_spectral_type(excluded_spectrals, excluded_spectrals_count, true);
                g_game_state.shop.booster_items[i].info.spectral.edition = CARD_EDITION_BASE;

                excluded_spectrals[excluded_spectrals_count++] = g_game_state.shop.booster_items[i].info.spectral.type;
            }
            break;
        }
        case BOOSTER_PACK_TYPE_STANDARD:
        {
            g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 3;
            if (g_game_state.shop.opened_booster.size != BOOSTER_PACK_SIZE_NORMAL) g_game_state.shop.booster_total_items = g_game_state.shop.booster_item_count = 5;
            for(int i = 0; i < g_game_state.shop.booster_item_count; i++)
            {
                g_game_state.shop.booster_items[i].available = true;
                g_game_state.shop.booster_items[i].type = ITEM_TYPE_CARD;
                g_game_state.shop.booster_items[i].info.card.rank = rand() % 13;
                g_game_state.shop.booster_items[i].info.card.suit = rand() % 4;
                g_game_state.shop.booster_items[i].info.card.edition = game_util_get_random_booster_card_edition();
                int enhancement = (rand() % (CARD_ENHANCEMENT_COUNT + 3)) - 3;
                g_game_state.shop.booster_items[i].info.card.enhancement = MAX(0, enhancement);
                g_game_state.shop.booster_items[i].info.card.extra_chips = 0;
                g_game_state.shop.booster_items[i].info.card.seal = (rand() % 100 < 20) ? (rand() % (CARD_SEAL_COUNT - 1) + 1) : CARD_SEAL_NONE;
                g_game_state.shop.booster_items[i].info.card.selected = false;
            }
            break;
        }
    }

    game_set_initial_final_shop_booster_item_position();

    g_game_state.highlighted_item = 0;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_BOOSTER_ITEMS;
}

void game_set_initial_final_shop_item_position()
{
    for(int i = 0; i < g_game_state.shop.item_count; i++)
    {
        if (g_game_state.shop.items[i].available)
        {
            switch (g_game_state.shop.items[i].type)
            {
                case ITEM_TYPE_JOKER:
                {
                    g_game_state.shop.items[i].info.joker.draw.initial_x = g_game_state.shop.items[i].info.joker.draw.x;
                    g_game_state.shop.items[i].info.joker.draw.final_x = game_util_get_item_position(i, g_game_state.shop.item_count, DRAW_SHOP_SINGLE_X, DRAW_SHOP_SINGLE_WIDTH, CARD_WIDTH); // DRAW_SHOP_SINGLE_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_SHOP_SINGLE_WIDTH / ((float)g_game_state.shop.item_count + 1.0f);
                    g_game_state.shop.items[i].info.joker.draw.final_y = DRAW_SHOP_SINGLE_Y;
                    g_game_state.shop.items[i].info.joker.draw.initial_y = g_game_state.shop.items[i].info.joker.draw.y;
                    g_game_state.shop.items[i].info.joker.draw.angle = 0.0f;
                    g_game_state.shop.items[i].info.joker.draw.scale = 1.0f;
                    g_game_state.shop.items[i].info.joker.draw.white_factor = 0.0f;
                    g_game_state.shop.items[i].info.joker.draw.alpha = 1.0f;
                }
                    break;
                default:
                    break;
            }
        }
    }
}

void game_sell_consumable()
{
    automated_event_set(AUTOMATED_EVENT_SELL_CONSUMABLE, 0);
}

bool game_use_consumable()
{
    if (g_game_state.consumables.items[g_game_state.highlighted_item].type == ITEM_TYPE_PLANET)
    {
        audio_play_sfx(AUDIO_SFX_TAROT);
        automated_event_set(AUTOMATED_EVENT_USE_PLANET, 0);
    }
    else if (g_game_state.consumables.items[g_game_state.highlighted_item].type == ITEM_TYPE_TAROT)
    {
        struct Tarot *tarot = &(g_game_state.consumables.items[g_game_state.highlighted_item].tarot);
        if (!game_util_can_tarot_be_used(tarot->type))
        {
            return false;
        }
        audio_play_sfx(AUDIO_SFX_TAROT);
        automated_event_set(AUTOMATED_EVENT_USE_TAROT, 0);
    }
    else if (g_game_state.consumables.items[g_game_state.highlighted_item].type == ITEM_TYPE_SPECTRAL)
    {
        struct Spectral *spectral = &(g_game_state.consumables.items[g_game_state.highlighted_item].spectral);
        if (!game_util_can_spectral_be_used(spectral->type))
        {
            return false;
        }
        audio_play_sfx(AUDIO_SFX_TAROT);
        automated_event_set(AUTOMATED_EVENT_USE_SPECTRAL, 0);
    }

    return true;
}

bool game_use_booster_item()
{
    switch (g_game_state.shop.booster_items[g_game_state.highlighted_item].type)
    {
        case ITEM_TYPE_CARD:
        {
            audio_play_sfx(AUDIO_SFX_CARD);
            automated_event_push(AUTOMATED_EVENT_ADD_BOOSTER_CARD, 0);
            break;
        }
        case ITEM_TYPE_JOKER:
        {
            if (g_game_state.shop.booster_items[g_game_state.highlighted_item].info.joker.edition != CARD_EDITION_NEGATIVE && !game_util_is_joker_slot_available()) return false;
            
            audio_play_sfx(AUDIO_SFX_COIN);
            automated_event_set(AUTOMATED_EVENT_BUY_JOKER, 0);
            break;
        }
        case ITEM_TYPE_PLANET:
        {
            audio_play_sfx(AUDIO_SFX_TAROT);
            automated_event_set(AUTOMATED_EVENT_USE_PLANET, 0);
            break;
        }
        case ITEM_TYPE_TAROT:
        {
            struct Tarot *tarot = &(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.tarot);
            if (!game_util_can_tarot_be_used(tarot->type))
            {
                return false;
            }
            audio_play_sfx(AUDIO_SFX_TAROT);
            automated_event_set(AUTOMATED_EVENT_USE_TAROT, 0);
            break;
        }
        case ITEM_TYPE_SPECTRAL:
        {
            struct Spectral *spectral = &(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.spectral);
            if (!game_util_can_spectral_be_used(spectral->type))
            {
                return false;
            }
            audio_play_sfx(AUDIO_SFX_TAROT);
            automated_event_set(AUTOMATED_EVENT_USE_SPECTRAL, 0);
            break;
        }
    }

    return true;
}

void game_sell_joker()
{
    automated_event_set(AUTOMATED_EVENT_SELL_JOKER, 0);
}

void game_remove_joker(int index)
{
    if (g_game_state.jokers.jokers[index].edition == CARD_EDITION_NEGATIVE) g_game_state.jokers.negative_count--;
    for(int j = index+1; j < g_game_state.jokers.joker_count; j++)
    {
        g_game_state.jokers.jokers[j - 1] = g_game_state.jokers.jokers[j];
    }
    g_game_state.jokers.joker_count--;
}

void game_set_initial_final_joker_position()
{
    for(int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        g_game_state.jokers.jokers[i].draw.initial_x = g_game_state.jokers.jokers[i].draw.x;
        g_game_state.jokers.jokers[i].draw.final_x = game_util_get_item_position(i, g_game_state.jokers.joker_count, DRAW_JOKERS_X, DRAW_JOKERS_WIDTH, CARD_WIDTH); // DRAW_JOKERS_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_JOKERS_WIDTH / ((float)g_game_state.jokers.joker_count + 1.0f);
        g_game_state.jokers.jokers[i].draw.final_y = DRAW_JOKERS_Y;
        g_game_state.jokers.jokers[i].draw.initial_y = g_game_state.jokers.jokers[i].draw.y;
        g_game_state.jokers.jokers[i].draw.angle = 0.0f;
        g_game_state.jokers.jokers[i].draw.scale = 1.0f;
    }
}

void game_set_joker_position()
{
    for(int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        g_game_state.jokers.jokers[i].draw.initial_x = g_game_state.jokers.jokers[i].draw.final_x = g_game_state.jokers.jokers[i].draw.x = game_util_get_item_position(i, g_game_state.jokers.joker_count, DRAW_JOKERS_X, DRAW_JOKERS_WIDTH, CARD_WIDTH); //DRAW_JOKERS_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_JOKERS_WIDTH / ((float)g_game_state.jokers.joker_count + 1.0f);
        g_game_state.jokers.jokers[i].draw.initial_y = g_game_state.jokers.jokers[i].draw.final_y = g_game_state.jokers.jokers[i].draw.y = DRAW_JOKERS_Y;
        g_game_state.jokers.jokers[i].draw.angle = 0.0f;
        g_game_state.jokers.jokers[i].draw.scale = 1.0f;
    }
}

void game_set_initial_final_consumable_position()
{
    struct DrawObject *draw = NULL;
    for(int i = 0; i < g_game_state.consumables.item_count; i++)
    {
        if (g_game_state.consumables.items[i].type == ITEM_TYPE_PLANET)
            draw = &(g_game_state.consumables.items[i].planet.draw);
        else if (g_game_state.consumables.items[i].type == ITEM_TYPE_TAROT)
            draw = &(g_game_state.consumables.items[i].tarot.draw);
        else
            draw = &(g_game_state.consumables.items[i].spectral.draw);
        draw->initial_x = draw->x;
        draw->final_x = game_util_get_item_position(i, g_game_state.consumables.item_count, DRAW_CONSUMABLES_X, DRAW_CONSUMABLES_WIDTH, CARD_WIDTH); //DRAW_CONSUMABLES_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_CONSUMABLES_WIDTH / ((float)g_game_state.consumables.item_count + 1.0f);
        draw->final_y = DRAW_JOKERS_Y;
        draw->initial_y = draw->y;
        draw->angle = 0.0f;
        draw->scale = 1.0f;
    }
}

void game_set_card_played_hand_final_position()
{
    for(int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        g_game_state.played_hand.cards[i]->draw.initial_x = g_game_state.played_hand.cards[i]->draw.x;
        g_game_state.played_hand.cards[i]->draw.initial_y = g_game_state.played_hand.cards[i]->draw.y;
        g_game_state.played_hand.cards[i]->draw.final_x = DRAW_HAND_X - (CARD_WIDTH / 2.0f) + (i + 1) * DRAW_HAND_WIDTH / ((float)g_game_state.played_hand.card_count + 1.0f);
        g_game_state.played_hand.cards[i]->draw.final_y = (g_game_state.played_hand.cards[i]->selected ? DRAW_SCORING_CARDS_Y : DRAW_PLAYED_CARDS_Y);
    }
}

void game_move_selected_cards_to_played_cards()
{
    struct Card *new_hand[MAX_CARDS];
    int count = 0;
    g_game_state.played_hand.card_count = 0;
    for(int i = 0; i < g_game_state.hand.card_count; i++)
    {
        if (g_game_state.hand.cards[i]->selected)
        {
            g_game_state.hand.cards[i]->selected = false;
            g_game_state.played_hand.cards[g_game_state.played_hand.card_count++] = g_game_state.hand.cards[i];
        }
        else
        {
            new_hand[count++] = g_game_state.hand.cards[i];
        }
    }

    g_game_state.selected_cards_count = 0;
    for(int i = 0; i < count; i++)
    {
        g_game_state.hand.cards[i] = new_hand[i];
    }
    g_game_state.hand.card_count = count;
}

void game_set_scoring_cards_in_played_hand()
{
    int scoring_cards[5];
    int scoring_card_count = 0;
    
    struct Card *cards[5];
    for(int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        cards[i] = g_game_state.played_hand.cards[i];
    }

    g_game_state.current_poker_hand = game_get_selected_poker_hand(cards, g_game_state.played_hand.card_count, scoring_cards, &scoring_card_count);
    if (game_util_has_joker_type(JOKER_TYPE_SPLASH))
    {
        for(int i = 0; i < g_game_state.played_hand.card_count; i++)
        {
            g_game_state.played_hand.cards[i]->selected = true;
        }
        g_game_state.scoring_card_count = g_game_state.played_hand.card_count;
    }
    else
    {
        for(int i = 0; i < scoring_card_count; i++)
        {
            g_game_state.played_hand.cards[scoring_cards[i]]->selected = true;
        }
        g_game_state.scoring_card_count = scoring_card_count;
    }
}

int game_cmp_sort_by_rank(const void *a, const void *b)
{
    int rank_a = (*(struct Card **)a)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)a)->rank;
    int rank_b = (*(struct Card **)b)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)b)->rank;
    int suit_a = (*(struct Card **)a)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)a)->suit;
    int suit_b = (*(struct Card **)b)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)b)->suit;
    int r_a = (int)(*(struct Card **)a)->draw.r;
    int r_b = (int)(*(struct Card **)b)->draw.r;
    return (rank_a == rank_b ? (suit_a == suit_b ? r_b - r_a : suit_b - suit_a) : rank_b - rank_a);
}

void game_sort_hand_by_rank()
{
    qsort(g_game_state.hand.cards, g_game_state.hand.card_count, sizeof(struct Card*), game_cmp_sort_by_rank);
}

int game_cmp_sort_by_suit(const void *a, const void *b)
{
    int suit_a = (*(struct Card **)a)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)a)->suit;
    int suit_b = (*(struct Card **)b)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)b)->suit;
    int rank_a = (*(struct Card **)a)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)a)->rank;
    int rank_b = (*(struct Card **)b)->enhancement == CARD_ENHANCEMENT_STONE ? -1 : (*(struct Card **)b)->rank;
    int r_a = (int)(*(struct Card **)a)->draw.r;
    int r_b = (int)(*(struct Card **)b)->draw.r;
    return suit_a == suit_b ? (rank_a == rank_b ? r_b - r_a : rank_b - rank_a) : suit_b - suit_a;
}

void game_sort_hand_by_suit()
{
    qsort(g_game_state.hand.cards, g_game_state.hand.card_count, sizeof(struct Card*), game_cmp_sort_by_suit);
}

void game_sort_hand()
{
    switch (g_game_state.sort_criteria)
    {
        case SORT_CRITERIA_RANK:
            game_sort_hand_by_rank();
            break;
        case SORT_CRITERIA_SUIT:
            game_sort_hand_by_suit();
            break;
    }
}

void game_start_ingame()
{
    g_game_state.stage = GAME_STAGE_INGAME;
    g_game_state.sub_stage = GAME_SUBSTAGE_INGAME_PICK_HAND;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_HAND;
    game_util_copy_deck(&g_game_state.full_deck, &g_game_state.current_deck);
    game_util_shuffle_deck(&g_game_state.current_deck);
    g_game_state.hand.card_count = 0;
    g_game_state.played_hand.card_count = 0;
    g_game_state.highlighted_item = 0;
    g_game_state.selected_cards_count = 0;
    g_game_state.current_poker_hand = GAME_POKER_HAND_NONE;
    g_game_state.score = 0;
    g_game_state.current_base_chips = 0;
    g_game_state.current_base_mult = 0;
    g_game_state.show_highlighted_card = true;
    g_game_state.cash_out_value = 0;
    g_game_state.cash_out_blind = -1;
    g_game_state.cash_out_hands = -1;
    g_game_state.cash_out_interest = -1;
    g_game_state.cash_out_jokers = -1;
    g_game_state.cash_out_done = false;
    g_game_state.round++;
    
    g_game_state.hands_played_in_round = 0;

    g_game_state.score_number.show_score_number = false;
    g_game_state.cash_out_panel_y = SCREEN_HEIGHT;

    g_game_state.moving_card = false;

    g_game_state.current_hands = game_util_get_hands();
    g_game_state.current_discards = game_util_get_discards();

    game_set_hand_y(DRAW_HAND_Y);

    automated_event_push(AUTOMATED_EVENT_DRAW, 1, game_util_get_hand_size());
}

void game_init_full_deck()
{
    int card_count = 0;
    for(int suit = 0; suit < 4; suit++)
    {
        for(int rank = 0; rank < 13; rank++)
        {
            g_game_state.all_cards.cards[card_count].rank = rank;
            g_game_state.all_cards.cards[card_count].suit = suit;
            g_game_state.all_cards.cards[card_count].extra_chips = 0;
            g_game_state.all_cards.cards[card_count].enhancement = CARD_ENHANCEMENT_NONE; // rand() % CARD_ENHANCEMENT_COUNT;
            g_game_state.all_cards.cards[card_count].edition =  CARD_EDITION_BASE;
            g_game_state.all_cards.cards[card_count].seal = CARD_SEAL_NONE;
            game_init_draw_object(&(g_game_state.all_cards.cards[card_count].draw));
            g_game_state.full_deck.cards[card_count] = &(g_game_state.all_cards.cards[card_count]);
            card_count++;
        }
    }

    g_game_state.all_cards.card_count = card_count;
    g_game_state.full_deck.card_count = card_count;
}

void game_start_shop(bool init);

bool game_init_load_file_values()
{
    char buffer[128];

    game_draw_loading_text("Loading \"settings.ini\"", COLOR_WHITE, COLOR_BLACK);
    if (!ini_open("settings.ini"))
    {
        game_draw_loading_text("Could not open \"settings.ini\"", COLOR_TEXT_YELLOW, COLOR_BLACK);
        return true;
    }

    enum IniTokenType token_type;
    do
    {
        token_type = ini_read_token(buffer, 128);
        if (token_type == INI_TOKEN_KEY)
        {
            if (!strcmp(buffer, "archive_file_name"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                strcpy(g_settings.archive_file_name, buffer);
            }
            else if (!strcmp(buffer, "hand_size"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.hand_size = CLAMP(value, 1, 100);
            }
            else if (!strcmp(buffer, "hands"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.hands = CLAMP(value, 1, 100);
            }
            else if (!strcmp(buffer, "discards"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.discards = CLAMP(value, 1, 100);
            }
            else if (!strcmp(buffer, "wealth"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.wealth = CLAMP(value, 0, 10000);
            }
            else if (!strcmp(buffer, "joker_slots"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.joker_slots = CLAMP(value, 0, 100);
            }
            else if (!strcmp(buffer, "consumable_slots"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.consumable_slots = CLAMP(value, 0, MAX_CONSUMABLES - 1);
            }
            else if (!strcmp(buffer, "shop_item_slots"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.shop_item_slots = CLAMP(value, 0, MAX_ITEM_TYPES - 1);
            }
            else if (!strcmp(buffer, "shop_booster_slots"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.shop_booster_slots = CLAMP(value, 0, MAX_ITEM_TYPES - 1);
            }
            else if (!strcmp(buffer, "audio"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                g_settings.audio = !strcmp(buffer, "true") || !strcmp(buffer, "1");
            }
            else if (!strcmp(buffer, "move_cards"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                g_settings.move_cards = !(!strcmp(buffer, "false") || !strcmp(buffer, "0"));
            }
            else if (!strcmp(buffer, "overclock"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                g_settings.overclock = (!strcmp(buffer, "true") || !strcmp(buffer, "1"));   
            }
            else if (!strcmp(buffer, "ante_score_scaling"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.ante_score_scaling = CLAMP(value, 1, 3);
            }
            else if (!strcmp(buffer, "speed"))
            {
                token_type = ini_read_token(buffer, 128);
                if (token_type != INI_TOKEN_VALUE) return false;
                int value = atoi(buffer);
                g_settings.speed = CLAMP(value, 1, 5);
            }
        }
        else if (token_type == INI_TOKEN_ERROR)
        {
            game_draw_loading_text("Error reading \"settings.ini\"", COLOR_TEXT_RED, COLOR_BLACK);
            return false;
        }
    } while (token_type != INI_TOKEN_EOF);

    ini_close();

    return true;
}

void game_init_logic()
{
    srand(time(0));

    for (int i = 0; i < BOOSTER_PACK_TYPE_COUNT; i++)
    {
        for (int j = 0; j < BOOSTER_PACK_SIZE_COUNT; j++)
        {
            g_booster_packs_weights_total += g_booster_packs_weights[i][j];
        }
    }

    // Jokers locked by default
    g_joker_types[JOKER_TYPE_CAVENDISH].enabled = false;
    g_joker_types[JOKER_TYPE_STEEL_JOKER].enabled = false;
    g_joker_types[JOKER_TYPE_GOLDEN_TICKET].enabled = false;
    g_joker_types[JOKER_TYPE_LUCKY_CAT].enabled = false;

    g_game_state.score = 0.0;
    g_game_state.ante = 1;
    g_game_state.blind = GAME_BLIND_SMALL;
    g_game_state.total_hands = g_settings.hands;
    g_game_state.total_discards = g_settings.discards;
    g_game_state.round = 0;
    g_game_state.wealth = g_settings.wealth;
    g_game_state.base_hand_size = g_settings.hand_size;
    g_game_state.joker_slots = g_settings.joker_slots;
    g_game_state.consumable_slots = g_settings.consumable_slots;

    g_game_state.last_used_consumable_item_type = -1;
    g_game_state.last_used_consumable_tarot_planet_type = -1;

    g_game_state.stage = GAME_STAGE_BLINDS;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_BLIND;

    g_game_state.sort_criteria = SORT_CRITERIA_RANK;

    g_game_state.flush_five_enabled = false;
    g_game_state.flush_house_enabled = false;
    g_game_state.five_of_a_kind_enabled = false;

    g_game_state.ante_score_scaling = g_settings.ante_score_scaling - 1;

    g_game_state.hand.card_count = 0;

    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_NONE] = 0;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_NONE] = 0;
    g_game_state.poker_hand_level[GAME_POKER_HAND_NONE] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_HIGH_CARD] = 5;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_HIGH_CARD] = 1;
    g_game_state.poker_hand_level[GAME_POKER_HAND_HIGH_CARD] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_PAIR] = 10;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_PAIR] = 2;
    g_game_state.poker_hand_level[GAME_POKER_HAND_PAIR] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_TWO_PAIR] = 20;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_TWO_PAIR] = 2;
    g_game_state.poker_hand_level[GAME_POKER_HAND_TWO_PAIR] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_THREE_OF_A_KIND] = 30;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_THREE_OF_A_KIND] = 3;    
    g_game_state.poker_hand_level[GAME_POKER_HAND_THREE_OF_A_KIND] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_STRAIGHT] = 30;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_STRAIGHT] = 4;
    g_game_state.poker_hand_level[GAME_POKER_HAND_STRAIGHT] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_FLUSH] = 35;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_FLUSH] = 4;
    g_game_state.poker_hand_level[GAME_POKER_HAND_FLUSH] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_FULL_HOUSE] = 40;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_FULL_HOUSE] = 4;
    g_game_state.poker_hand_level[GAME_POKER_HAND_FULL_HOUSE] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_FOUR_OF_A_KIND] = 60;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_FOUR_OF_A_KIND] = 7;
    g_game_state.poker_hand_level[GAME_POKER_HAND_FOUR_OF_A_KIND] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_STRAIGHT_FLUSH] = 100;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_STRAIGHT_FLUSH] = 8;
    g_game_state.poker_hand_level[GAME_POKER_HAND_STRAIGHT_FLUSH] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_FIVE_OF_A_KIND] = 120;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_FIVE_OF_A_KIND] = 12;
    g_game_state.poker_hand_level[GAME_POKER_HAND_FIVE_OF_A_KIND] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_FLUSH_HOUSE] = 140;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_FLUSH_HOUSE] = 14;
    g_game_state.poker_hand_level[GAME_POKER_HAND_FLUSH_HOUSE] = 1;
    g_game_state.poker_hand_base_chips[GAME_POKER_HAND_FLUSH_FIVE] = 160;
    g_game_state.poker_hand_base_mult[GAME_POKER_HAND_FLUSH_FIVE] = 16;
    g_game_state.poker_hand_level[GAME_POKER_HAND_FLUSH_FIVE] = 1;

    game_init_full_deck();
    game_util_copy_deck(&g_game_state.full_deck, &g_game_state.current_deck);

    // For testing purposes
    g_game_state.jokers.jokers[0].type = JOKER_TYPE_FACELESS_JOKER;
    g_game_state.jokers.jokers[0].edition = CARD_EDITION_BASE;
    g_game_state.jokers.jokers[0].param1 = 0;
    g_game_state.jokers.jokers[1].type = JOKER_TYPE_GREEN_JOKER;
    g_game_state.jokers.jokers[1].edition = CARD_EDITION_BASE;
    g_game_state.jokers.jokers[2].type = JOKER_TYPE_SOCK_AND_BUSKIN;
    g_game_state.jokers.jokers[2].edition = CARD_EDITION_BASE;
    g_game_state.jokers.jokers[3].type = JOKER_TYPE_BLUEPRINT;
    g_game_state.jokers.jokers[3].edition = CARD_EDITION_BASE;
    g_game_state.jokers.jokers[4].type = JOKER_TYPE_SMILEY_FACE;
    g_game_state.jokers.jokers[4].edition = CARD_EDITION_BASE;
    g_game_state.jokers.joker_count = 0;
    g_game_state.jokers.negative_count = 0;

    for (int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        game_init_joker(&(g_game_state.jokers.jokers[i]));
    }

    g_game_state.consumables.items[0].type = ITEM_TYPE_TAROT;
    g_game_state.consumables.items[0].tarot.edition = 0;
    g_game_state.consumables.items[0].tarot.type = TAROT_TYPE_DEATH;
    g_game_state.consumables.item_count = 0;

    game_set_joker_position();

    // Shop stuff
    g_game_state.shop.total_items = g_settings.shop_item_slots;
    g_game_state.shop.total_boosters = g_settings.shop_booster_slots;
    g_game_state.shop.reroll_cost = 5;
    // game_start_shop(true);

    // TEMP
    // game_start_ingame();

    // Initialize stats
    g_game_state.stats.cards_played = 0;
    g_game_state.stats.cards_discarded = 0;
    g_game_state.stats.cards_purchased = 0;
    g_game_state.stats.max_score = 0.0;
    g_game_state.stats.times_rerolled = 0;
    g_game_state.stats.most_played_poker_hand = GAME_POKER_HAND_NONE;
    for(int i = 0; i < GAME_POKER_HAND_COUNT; i++)
    {
        g_game_state.stats.poker_hands_played[i] = 0;
    }

    g_game_state.deck_info.partial = false;
    g_game_state.deck_info.effective = false;
}

void game_restart_game()
{
    game_init_logic();
}

void game_increment_played_poker_hand(int poker_hand)
{
    g_game_state.stats.poker_hands_played[poker_hand]++;
    if (g_game_state.stats.poker_hands_played[poker_hand] > g_game_state.stats.poker_hands_played[g_game_state.stats.most_played_poker_hand])
    {
        g_game_state.stats.most_played_poker_hand = poker_hand;
    }
}

void game_go_to_next_blind()
{
    g_game_state.stage = GAME_STAGE_BLINDS;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_BLIND;
    g_game_state.blind++;
    if (g_game_state.blind > GAME_BLIND_BOSS)
    {
        g_game_state.blind = GAME_BLIND_SMALL;
        g_game_state.ante++;
    }
}

#define CHANCE_JOKER_SHOP_SINGLE 71
#define CHANCE_TAROT_SHOP_SINGLE 14

void game_shop_set_singles()
{
    int excluded_joker_types[200];
    int excluded_joker_types_count = 0;
    for(int j = 0; j < g_game_state.jokers.joker_count; j++)
    {
        excluded_joker_types[excluded_joker_types_count++] = g_game_state.jokers.jokers[j].type;
    }

    int excluded_tarots[200];
    int excluded_tarots_count = 0;
    int excluded_planets[200];
    int excluded_planets_count = 0;
    for(int j = 0; j < g_game_state.consumables.item_count; j++)
    {
        if (g_game_state.consumables.items[j].type == ITEM_TYPE_TAROT)
        {
            excluded_tarots[excluded_tarots_count++] = g_game_state.consumables.items[j].tarot.type;
        }
        else if (g_game_state.consumables.items[j].type == ITEM_TYPE_PLANET)
        {
            excluded_planets[excluded_planets_count++] = g_game_state.consumables.items[j].planet.type;
        }
    }

    g_game_state.shop.item_count = g_game_state.shop.total_items;
    for (int i = 0; i < g_game_state.shop.total_items; i++)
    {
        g_game_state.shop.items[i].available = true;
        int r = game_util_rand(1, 100);
        if (r <= CHANCE_JOKER_SHOP_SINGLE)
        {
            g_game_state.shop.items[i].type = ITEM_TYPE_JOKER;
            g_game_state.shop.items[i].info.joker.type = game_util_get_new_joker_type(excluded_joker_types, excluded_joker_types_count, false);
            game_init_joker(&g_game_state.shop.items[i].info.joker);
            g_game_state.shop.items[i].info.joker.edition = game_util_get_shop_joker_edition();
            excluded_joker_types[excluded_joker_types_count++] = g_game_state.shop.items[i].info.joker.type;
        }
        else if (r <= CHANCE_JOKER_SHOP_SINGLE + CHANCE_TAROT_SHOP_SINGLE)
        {
            g_game_state.shop.items[i].type = ITEM_TYPE_TAROT;
            g_game_state.shop.items[i].info.tarot.type = game_util_get_new_tarot_type(excluded_tarots, excluded_tarots_count);
            excluded_tarots[excluded_tarots_count++] = g_game_state.shop.items[i].info.tarot.type;
        }
        else
        {
            g_game_state.shop.items[i].type = ITEM_TYPE_PLANET;
            g_game_state.shop.items[i].info.planet.type = game_util_get_new_planet_type(excluded_planets, excluded_planets_count);
            excluded_planets[excluded_planets_count++] = g_game_state.shop.items[i].info.planet.type;
        }
    }
    game_set_shop_item_position();
}

void game_shop_set_boosters()
{
    g_game_state.shop.booster_count = g_game_state.shop.total_boosters;
    for (int i = 0; i < g_game_state.shop.total_boosters; i++)
    {
        g_game_state.shop.boosters[i].available = true;
        if (i == 0 && g_game_state.round == 1 && g_game_state.ante == 1)
        {
            g_game_state.shop.boosters[i].booster.type = BOOSTER_PACK_TYPE_BUFFOON;
            g_game_state.shop.boosters[i].booster.size = BOOSTER_PACK_SIZE_NORMAL;
        }
        else
        {
            game_util_get_new_booster_pack(&(g_game_state.shop.boosters[i].booster.type), &(g_game_state.shop.boosters[i].booster.size));
        }
        g_game_state.shop.boosters[i].booster.image = random_int(0, BOOSTER_PACK_MAX_IMAGES - 1);
    }
    
    game_set_shop_booster_position();
}

void game_start_shop(bool init)
{
    g_game_state.score = 0.0;
    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_SHOP_NEXT_ROUND;

    if (init)
    {
        g_game_state.shop.rerolls = 0;

        game_shop_set_singles();
        game_shop_set_boosters();
    }
}

void game_shop_reroll()
{
    if (!game_util_can_buy(game_util_get_reroll_cost())) return;

    g_game_state.wealth -= game_util_get_reroll_cost();

    g_game_state.shop.rerolls++;

    game_shop_set_singles();

    g_game_state.stats.times_rerolled++;
}

void game_remove_card_from_played_hand(int index)
{
    if (g_game_state.played_hand.cards[index]->selected)
    {
        g_game_state.scoring_card_count--;
    }

    for (int i = index + 1; i < g_game_state.played_hand.card_count; i++)
    {
        g_game_state.played_hand.cards[i - 1] = g_game_state.played_hand.cards[i];
    }
    g_game_state.played_hand.card_count--;
}

void game_remove_card_from_full_deck(struct Card *card)
{
    bool found = false;
    for (int i = 0; i < g_game_state.full_deck.card_count; i++)
    {
        if (card == g_game_state.full_deck.cards[i])
        {
            found = true;
        }

        if (found && i < g_game_state.full_deck.card_count - 1)
        {
            g_game_state.full_deck.cards[i] = g_game_state.full_deck.cards[i+1];
        }
    }

    if (found)
    {
        g_game_state.full_deck.card_count--;
    }
}

void game_init_draw_object(struct DrawObject *draw)
{
    draw->alpha = 1.0f;
    draw->angle = 0.0f;
    draw->scale = 1.0f;
    draw->white_factor = 0.0f;
    draw->r = random_float(0.0f, 1000.0f);
}

struct Card *game_set_new_card()
{
    // TODO: Check if over the limit
    g_game_state.all_cards.card_count++;
    return &g_game_state.all_cards.cards[g_game_state.all_cards.card_count-1];
}

void game_add_card_to_full_deck(struct Card *card)
{
    g_game_state.full_deck.cards[g_game_state.full_deck.card_count] = card;
    g_game_state.full_deck.card_count++;
}

void game_add_card_to_hand(struct Card *card)
{
    g_game_state.hand.cards[g_game_state.hand.card_count] = card;
    g_game_state.hand.card_count++;
}

void game_add_card_shuffled_to_current_deck(struct Card *card)
{
    g_game_state.current_deck.card_count++;
    int card_position = rand() % g_game_state.current_deck.card_count;

    if (card_position < g_game_state.current_deck.card_count - 1)
    {
        for (int i = g_game_state.current_deck.card_count - 1; i > g_game_state.current_deck.card_count - 1; i--)
        {
            g_game_state.current_deck.cards[i] = g_game_state.current_deck.cards[i - 1];
        }
    }
    g_game_state.current_deck.cards[card_position] = card;
}

void game_init_tarot(struct Tarot *tarot, int type, int edition)
{
    tarot->type = type;
    tarot->edition = edition;
    game_init_draw_object(&(tarot->draw));
}

void game_init_planet(struct Planet *planet, int type, int edition)
{
    planet->type = type;
    planet->edition = edition;
    game_init_draw_object(&(planet->draw));
}

void game_init_spectral(struct Spectral *spectral, int type, int edition)
{
    spectral->type = type;
    spectral->edition = edition;
    game_init_draw_object(&(spectral->draw));
}

void game_add_joker(struct Joker *joker)
{
    g_game_state.jokers.jokers[g_game_state.jokers.joker_count] = *joker;
    game_init_joker(&(g_game_state.jokers.jokers[g_game_state.jokers.joker_count]));
    g_game_state.jokers.joker_count++;
    if (joker->edition == CARD_EDITION_NEGATIVE) g_game_state.jokers.negative_count++;
}

void game_set_object_off_screen(struct DrawObject *draw)
{
    draw->initial_x = draw->x = (SCREEN_WIDTH - CARD_WIDTH) / 2.0f;
    draw->initial_y = draw->y = -CARD_HEIGHT;
}

void game_go_back_to_previous_stage()
{
    g_game_state.stage = g_game_state.previous_stage;
    g_game_state.sub_stage = g_game_state.previous_sub_stage;
    g_game_state.input_focused_zone = g_game_state.previous_input_focused_zone;
}

void game_go_to_stage(int stage, int sub_stage)
{
    g_game_state.previous_stage = g_game_state.stage;
    g_game_state.previous_sub_stage = g_game_state.sub_stage;
    g_game_state.previous_input_focused_zone = g_game_state.input_focused_zone;
    g_game_state.stage = stage;
    g_game_state.sub_stage = sub_stage;

    if (sub_stage == GAME_SUBSTAGE_DECK_INFO && g_game_state.previous_sub_stage != sub_stage)
    {
        game_draw_offscreen_deck_info();
        g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_DECK_INFO_CLOSE;
        g_game_state.deck_info.current_card_x = -1;
        g_game_state.deck_info.current_card_y = -1;
    }

    if (sub_stage == GAME_SUBSTAGE_RUN_INFO)
    {
        g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_RUN_INFO_CLOSE;
    }

    if (stage == GAME_STAGE_PAUSE_MENU)
    {
        g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_PAUSE_MENU;
        g_game_state.highlighted_item = 0;
    }

    switch(stage)
    {
        case GAME_STAGE_INGAME:
        {
            switch(sub_stage)
            {
                case GAME_SUBSTAGE_INGAME_PICK_HAND:
                {
                    game_start_ingame();
                    break;
                }
                case GAME_SUBSTAGE_INGAME_WON:
                {
                    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_CASH_OUT_PANEL;
                    break;
                }
                case GAME_SUBSTAGE_INGAME_WON_END:
                {
                    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_YOU_WIN_PANEL;
                    break;
                }
                case GAME_SUBSTAGE_INGAME_LOST_END:
                {
                    g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_GAME_OVER_PANEL;
                    break;
                }
            }
            break;
        }
        case GAME_STAGE_BLINDS:
        {
            if (g_game_state.previous_stage != GAME_STAGE_BLINDS)
            {
                game_go_to_next_blind();
            }
            break;
        }
        case GAME_STAGE_SHOP:
        {
            switch (sub_stage)
            {
                case GAME_SUBSTAGE_SHOP_MAIN:
                {
                    game_start_shop(g_game_state.previous_stage != GAME_STAGE_SHOP);
                    break;
                }
                case GAME_SUBSTAGE_SHOP_BOOSTER_HAND:
                case GAME_SUBSTAGE_SHOP_BOOSTER_NO_HAND:
                {
                    game_init_booster_items();
                    break;
                }
            }            
            break;
        }
    }
}

uint32_t g_game_counter = 0;

void game_update()
{
    bool no_input = automated_event_run();
    event_run();

    game_input_update(no_input);

    if (!no_input) g_freeze_cards = false;

    g_game_counter++;
}
