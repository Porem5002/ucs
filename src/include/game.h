#ifndef CHECKERS_HEADER
#define CHECKERS_HEADER

#include <stdint.h>
#include <stdbool.h>

#include "sui.h"
#include "board.h"
#include "pager.h"
#include "strplus.h"
#include "logger.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#define DTS_USE_ARRAY
#define DTS_USE_DYNARRAY
#define DTS_USE_TREE

#include "dtstructs.h"

/* LOG and UI text */

#define PT_MENUS 1

#define LOG_MESSAGE_CHALLENGE_COMPLETED "Challenge Completed"
#define LOG_MESSAGE_BLACK_TEAM_WON      "Black Team Won"
#define LOG_MESSAGE_WHITE_TEAM_WON      "White Team Won"

#if PT_MENUS == 1
#define UI_MESSAGE_CHALLENGE_COMPLETED  "Desafio Concluido"
#define UI_MESSAGE_BLACK_TEAM_WON       "Negras Venceram"
#define UI_MESSAGE_WHITE_TEAM_WON       "Brancas Venceram"

#define UI_START_BUTTON_TEXT "JOGAR"
#define UI_EDITOR_BUTTON_TEXT "EDITOR"
#define UI_QUIT_BUTTON_TEXT "SAIR"
#define UI_BACK_BUTTON_TEXT "VOLTAR"

#else
#define UI_MESSAGE_CHALLENGE_COMPLETED  LOG_MESSAGE_CHALLENGE_COMPLETED
#define UI_MESSAGE_BLACK_TEAM_WON       LOG_MESSAGE_BLACK_TEAM_WON
#define UI_MESSAGE_WHITE_TEAM_WON       LOG_MESSAGE_WHITE_TEAM_WON

#define UI_START_BUTTON_TEXT "START"
#define UI_EDITOR_BUTTON_TEXT "EDITOR"
#define UI_QUIT_BUTTON_TEXT "QUIT"
#define UI_BACK_BUTTON_TEXT "BACK"

#endif

/* RGB COLORS */

#define BACKGROUND_COLOR_VALS 50, 50, 50
#define ATTRACTIVE_COLOR_VALS 135, 131, 209
#define MIDDLE_COLOR_VALS 0, 0, 0
#define COMMON_COLOR_VALS 50, 50, 50

#define BLACK_CELLS_COLOR_VALS 118, 65, 52
#define WHITE_CELLS_COLOR_VALS 170, 143, 102

#define BLACK_PIECE_COLOR_VALS 0, 0, 0
#define WHITE_PIECE_COLOR_VALS 255, 255, 255

#define SELECTED_CELL_COLOR_VALS 165, 180, 82
#define HOVERED_CELL_COLOR_VALS 165, 180, 82
#define LAST_MOVE_COLOR_VALS 135, 131, 209

#define UI_BACKGROUND_COLOR_VALS 120, 133, 133

/* DIMENSIONS, CELLS AND PIECES */

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define BOARD_SECTION_WIDTH SCREEN_HEIGHT
#define BOARD_SECTION_HEIGHT SCREEN_HEIGHT
#define UI_SECTION_WIDTH ((SCREEN_WIDTH - SCREEN_HEIGHT)/3)

#define CELL_WIDTH            (BOARD_SECTION_WIDTH / game.scenario_data.board_side_size)
#define CELL_HEIGHT           (BOARD_SECTION_HEIGHT / game.scenario_data.board_side_size)
#define PIECE_WIDTH           (2 * BOARD_SECTION_WIDTH / game.scenario_data.board_side_size / 3)
#define PIECE_HEIGHT          (2 * BOARD_SECTION_HEIGHT / game.scenario_data.board_side_size / 3)

#define PIECE_PIXEL_OFFSET_X  ((CELL_WIDTH - PIECE_WIDTH)/2)
#define PIECE_PIXEL_OFFSET_Y  ((CELL_HEIGHT - PIECE_HEIGHT)/2)
#define CELL_COUNT                    (game.scenario_data.board_side_size * game.scenario_data.board_side_size)
#define PLAYABLE_CELL_COUNT           (CELL_COUNT / 2)
#define PLAYABLE_CELL_COUNT_PER_LINE  (game.scenario_data.board_side_size / 2)

#define LOG_FILE_PATH "log/log.txt"

#define AUTO_PLAY_COOLDOWN 0.3F

#define TEXT_INPUT_FIELD_MAX_LENGTH 14
#define TEXT_INPUT_FIELD_SIZE (TEXT_INPUT_FIELD_MAX_LENGTH+1)

enum
{
    MODE_MENU,
    MODE_EDITOR,
    MODE_SELECTOR,
    MODE_SCENARIO
};

enum
{
    SCENARIO_MODE_1V1,
    SCENARIO_MODE_CHALLENGE
};

typedef enum
{
    GAME_INPUT_NONE,
    GAME_INPUT_MOUSE_BUTTON_DOWN,
    GAME_INPUT_KEY_DOWN
} game_input_type_t;

typedef struct
{
    game_input_type_t type;
    Sint32 mouseX;
    Sint32 mouseY;

    union
    {
        Uint8 mouse_button_pressed;
        SDL_Keycode key_pressed;
    };
} game_input_t;

typedef struct
{
    board_unit_t board_side_size;

    board_t board;
    team_t team;
    uint8_t scenario_mode;

    array(move_info_t) challenge_moves;

    bool double_corner_on_right;

    bool applies_law_of_quantity;
    bool applies_law_of_quality;

    bool peons_capture_backwards;
    bool flying_kings;
    bool is_white_peon_forward_top_to_bottom;
} scenario_t;

typedef struct
{
    array(string_t) file_paths;
    pager_t pager;
    bool is_standard_section;
} game_selector_t;

typedef struct
{    
    SDL_Rect screen_rect;
    SDL_Rect screen_scenario_board_rect;
    SDL_Rect screen_scenario_ui_rect;

    SDL_Window* window;
    SDL_Renderer* renderer;

    char text_input_field [TEXT_INPUT_FIELD_SIZE];
    game_input_t input;

    void (*update) ();
    void (*on_text_input_field_changed) ();

    union
    {
        struct 
        {
            sui_solid_rect_t* team_displayer;
            sui_texture_t* challenge_feedback_displayer;
            
            sui_texture_t* board_size_texture;
            sui_texture_t* double_corner_side_texture;

            float auto_play_current_cooldown;

            cell_value_t piece_type_to_place;

            tree_t capture_tree;
            tree(move_info_t) current_capture_subtree;

            size_t current_challenge_move_index;

            cell_id_t eat_chaining_piece;
            cell_id_t last_move_source_cell_id;
            cell_id_t last_move_dest_cell_id;
            cell_id_t selected_piece_cell_id;
            cell_id_t currently_hovered_cell_id;

            team_t current_team; 

            scenario_t scenario_data;

            bool force_capture_move;
            bool is_piece_selected;
            bool contains_last_move_info;
            bool is_cell_hovered;

            bool scenario_game_over_reached;
        };

        game_selector_t selector;
    };

    float delta_time;

    bool is_playing;
    bool is_text_input_field_active;
    uint8_t mode;
} game_t;

extern game_t game;

void game_catch_input(const SDL_Event* event);

void game_update_menu();
void game_update_selector();
void game_update_scenario();
void game_update_editor();

void game_set_mode_menu(void* event_data);
void game_set_mode_selector(void* event_data);
void game_set_mode_scenario(void* event_data);
void game_set_mode_editor(void* event_data);
void game_1v1_scenario_set_capture_data();

void game_free_dependencies();
void game_quit(void* event_data);

void game_check_for_and_activate_victory();
void game_activate_game_over_panel(char* text_message);

void scenario_set_default(scenario_t* scenario);

void game_text_input_field_start();
void game_text_input_field_stop();
void game_text_input_field_receive(string_t);
void game_text_input_field_back();
void game_text_input_field_clear();

#endif