#ifndef CHECKERS_HEADER
#define CHECKERS_HEADER

#include <stdint.h>
#include <stdbool.h>

#include "sui.h"
#include "board.h"
#include "pager.h"
#include "strplus.h"
#include "logger.h"
#include "static_text.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#define DTS_USE_ARRAY
#define DTS_USE_DYNARRAY
#define DTS_USE_TREE

#include "dtstructs.h"

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

enum
{
    MODE_MENU,
    MODE_EDITOR,
    MODE_FILE_BROWSER,
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
    SDL_Rect screen_rect;
    SDL_Rect screen_scenario_board_rect;
    SDL_Rect screen_scenario_ui_rect;

    SDL_Window* window;
    SDL_Renderer* renderer;

    game_input_t input;

    void (*update) ();

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

        struct
        {
            array(string_t) file_browser_file_paths;
            pager_t file_browser_pager;
        };
    };

    float delta_time;

    bool is_playing;
    uint8_t mode;
} game_t;

extern game_t game;

void game_catch_input(const SDL_Event* event);

void game_update_menu();
void game_update_file_browser();
void game_update_scenario();
void game_update_editor();

void game_set_mode_menu(void* event_data);
void game_set_mode_file_browser(void* event_data);
void game_set_mode_scenario(void* event_data);
void game_1v1_scenario_set_capture_data();

void game_free_dependencies();
void game_quit(void* event_data);

void game_check_for_and_activate_victory();
void game_activate_game_over_panel(char* text_message);

void scenario_set_default(scenario_t* scenario);

#endif