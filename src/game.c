#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "include/interaction.h"
#include "include/assetman_setup.h"
#include "include/sui.h"
#include "include/game.h"
#include "include/editor.h"
#include "include/file_browser.h"
#include "include/scenario_loader.h"
#include "include/rendering.h"

game_t game = {0};

void game_update()
{
    if(game.mode == MODE_SCENARIO && game.scenario_data.scenario_mode == SCENARIO_MODE_CHALLENGE && game.scenario_data.team != game.current_team)
        challenge_auto_play();
}

void game_mouse_motion()
{
    if(game.mode == MODE_SCENARIO || game.mode == MODE_EDITOR)
    {
        update_currently_hovered_cell();
    }
}

void game_mouse_button_down(Uint8 mouse_button, Sint32 mouse_x, Sint32 mouse_y)
{
    if(game.mode == MODE_SCENARIO && game.scenario_data.scenario_mode == SCENARIO_MODE_CHALLENGE && game.scenario_data.team != game.current_team)
        return;

    on_screen_clicked(mouse_button, mouse_x, mouse_y);
}

void game_key_down(SDL_Keycode key)
{
    if(game.mode == MODE_SCENARIO && game.mode == MODE_EDITOR) return;

    if(key == SDLK_ESCAPE) game_set_mode_menu(NULL);

    if(game.mode == MODE_EDITOR && key == SDLK_SPACE) save_scenario_as_sch_file(&game.scenario_data);
}

void game_set_mode_menu(void* event_data)
{
    LOGGER_LOGS("Started loading Menu!");

    game_free_dependencies();
    game.mode = MODE_MENU;
    sui_clear_elements();

    TTF_Font* main_font = assetman_get_asset(MAIN_FONT_ID);
    TTF_Font* title_font = assetman_get_asset(TITLE_FONT_ID);

    SDL_Color text_color = { MIDDLE_COLOR_VALS, 255};
    SDL_Color button_background_color = { ATTRACTIVE_COLOR_VALS, 255};
    SDL_Color menu_options_bg = { MIDDLE_COLOR_VALS, 255};

    SDL_Rect button_rects [3];

    SDL_Rect game_title_rect = { .x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT/6 };
    SDL_Rect menu_options_rect = { .x = 0, .y = SCREEN_HEIGHT - SCREEN_HEIGHT/6, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT/6 };

    SDL_Texture* title_text_texture = sui_texture_from_text(game.renderer, title_font, "UCS", text_color);

    int title_width;
    int title_height;

    SDL_QueryTexture(title_text_texture, NULL, NULL, &title_width, &title_height);

    SDL_Rect title_rect = sui_rect_centered(&game_title_rect, title_width, title_height);

    sui_texture_element_add(&title_rect, title_text_texture);
    sui_solid_rect_element_add(&menu_options_rect, menu_options_bg);

    sui_rect_row(&menu_options_rect, button_rects, 3, 300, 100, 50);

    sui_simple_button_t* start_button = sui_simple_button_with_text_add(game.renderer, &button_rects[0], main_font, UI_START_BUTTON_TEXT, game_set_mode_file_browser, NULL, text_color, button_background_color);
    sui_simple_button_t* editor_button = sui_simple_button_with_text_add(game.renderer, &button_rects[1], main_font, UI_EDITOR_BUTTON_TEXT, game_set_mode_editor, NULL, text_color, button_background_color);
    sui_simple_button_t* quit_button = sui_simple_button_with_text_add(game.renderer, &button_rects[2], main_font, UI_QUIT_BUTTON_TEXT, game_quit, NULL, text_color, button_background_color);
    
    assetman_set_asset(assetman_dynamic_id(0), TEXTURE_ASSET_TYPE, title_text_texture);
    assetman_set_asset(assetman_dynamic_id(1), TEXTURE_ASSET_TYPE, start_button->text.as_texture_element.texture);
    assetman_set_asset(assetman_dynamic_id(2), TEXTURE_ASSET_TYPE, editor_button->text.as_texture_element.texture);
    assetman_set_asset(assetman_dynamic_id(3), TEXTURE_ASSET_TYPE, quit_button->text.as_texture_element.texture);

    LOGGER_LOGS("Finished loading Menu!");
}

void game_set_mode_scenario(void* scenario_file_name)
{
    LOGGER_LOGS("Started loading Scenario!");

    string_t safely_stored_scenario_file_name = string_heap_copy(scenario_file_name);

    game_free_dependencies();
    sui_clear_elements();

    load_scenario_from_file(&game.scenario_data, safely_stored_scenario_file_name);

    game.mode = MODE_SCENARIO;
    game.scenario_game_over_reached = false;
    game.force_capture_move = false;
    game.is_piece_selected = false;
    game.contains_last_move_info = false;
    game.current_team = game.scenario_data.team;
    
    game.screen_scenario_board_rect = sui_rect_centered(&game.screen_rect, BOARD_SECTION_WIDTH + UI_SECTION_WIDTH, BOARD_SECTION_HEIGHT);
    game.screen_scenario_ui_rect.x = game.screen_scenario_board_rect.x + BOARD_SECTION_WIDTH;
    game.screen_scenario_ui_rect.y = game.screen_scenario_board_rect.y;
    game.screen_scenario_ui_rect.w = UI_SECTION_WIDTH;
    game.screen_scenario_ui_rect.h = BOARD_SECTION_HEIGHT;

    free(safely_stored_scenario_file_name);

    SDL_Rect team_displayer_rect = sui_rect_centered(&game.screen_scenario_ui_rect, 2 * UI_SECTION_WIDTH / 3, 2 * UI_SECTION_WIDTH / 3);

    sui_solid_rect_element_add(&game.screen_scenario_ui_rect, (SDL_Color){ UI_BACKGROUND_COLOR_VALS, 255 });
    
    game.team_displayer = sui_solid_rect_element_add(&team_displayer_rect, (SDL_Color){ 0,0,0,0 });
    update_team_displayer();

    if(game.scenario_data.scenario_mode == SCENARIO_MODE_1V1)
    {
        game_1v1_scenario_set_capture_data();
    }
    else if(game.scenario_data.scenario_mode == SCENARIO_MODE_CHALLENGE)
    {
        game.current_challenge_move_index = 0;
        game.auto_play_current_cooldown = AUTO_PLAY_COOLDOWN;

        SDL_Rect challenge_feedback_displayer_rect = team_displayer_rect;
        challenge_feedback_displayer_rect.y += 300;
        game.challenge_feedback_displayer = sui_texture_element_add(&challenge_feedback_displayer_rect, NULL);
    }


    LOGGER_LOGS("Finished loading Scenario!");
}

void game_1v1_scenario_set_capture_data()
{
    game.capture_tree = board_generate_capture_tree(&game.scenario_data.board);
    game.current_capture_subtree = game.capture_tree;
    game.force_capture_move = tree_child_count(game.capture_tree) > 0;
}

void game_quit(void* event_data)
{
    game.is_playing = false;
}

void game_check_for_and_activate_victory()
{
    if(game.scenario_data.scenario_mode == SCENARIO_MODE_CHALLENGE && array_size(&game.scenario_data.challenge_moves) == game.current_challenge_move_index)
    {
        LOGGER_LOGS(LOG_MESSAGE_CHALLENGE_COMPLETED);
        game_activate_game_over_panel(UI_MESSAGE_CHALLENGE_COMPLETED);
        return;
    }

    if(!board_contains_any_valid_moves_for_team(&game.scenario_data.board, game.current_team))
    {
        if(game.current_team == WHITE_TEAM)
        {
            LOGGER_LOGS(LOG_MESSAGE_BLACK_TEAM_WON);
            game_activate_game_over_panel(UI_MESSAGE_BLACK_TEAM_WON);
        }
        else if(game.current_team == BLACK_TEAM)
        {
            LOGGER_LOGS(LOG_MESSAGE_WHITE_TEAM_WON);
            game_activate_game_over_panel(UI_MESSAGE_WHITE_TEAM_WON);
        }
    }
}

void game_activate_game_over_panel(char* text_message)
{
    if(game.scenario_game_over_reached) return;

    TTF_Font* main_font = assetman_get_asset(MAIN_FONT_ID);

    SDL_Rect panel_rect = sui_rect_centered(&game.screen_rect, 900, 700);

    sui_solid_rect_element_add(&game.screen_rect, (SDL_Color) { 0, 0, 0, 127 });
    sui_solid_rect_element_add(&panel_rect, (SDL_Color) { COMMON_COLOR_VALS, 255 });

    SDL_Texture* game_over_text_texture = sui_texture_from_text(game.renderer, main_font, text_message, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255});

    int game_over_text_width;
    int game_over_text_height;

    SDL_QueryTexture(game_over_text_texture, NULL, NULL, &game_over_text_width, &game_over_text_height);

    SDL_Rect text_rect = sui_rect_centered(&game.screen_rect, game_over_text_width, game_over_text_height);
    text_rect.y -= 250;

    sui_texture_element_add(&text_rect, game_over_text_texture);

    SDL_Rect button_rect = sui_rect_centered(&game.screen_rect, 300, 100);
    button_rect.y += 250;
    
    SDL_Texture* menu_button_texture = sui_texture_from_text(game.renderer, main_font, "MENU", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });

    sui_simple_button_with_texture_add(&button_rect, menu_button_texture, game_set_mode_menu, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });

    assetman_set_asset(assetman_dynamic_id(0), TEXTURE_ASSET_TYPE, game_over_text_texture);
    assetman_set_asset(assetman_dynamic_id(1), TEXTURE_ASSET_TYPE, menu_button_texture);

    game.scenario_game_over_reached = true;
}

void game_free_dependencies()
{
    switch (game.mode)
    {
        case MODE_MENU:
            break;
        case MODE_EDITOR:
            break;
        case MODE_FILE_BROWSER:
            for (size_t i = 0; i < array_size(&game.file_browser_file_names_list); i++)
            {
                string_t current_name = array_ele(&game.file_browser_file_names_list, string_t, i);
                free(current_name);
            }

            array_free(&game.file_browser_file_names_list);
            break;
        case MODE_SCENARIO:
            if(game.scenario_data.scenario_mode == SCENARIO_MODE_1V1)
                tree_free(game.capture_tree);
            else if(game.scenario_data.scenario_mode == SCENARIO_MODE_CHALLENGE)
                array_free(&game.scenario_data.challenge_moves);
            break;
        default:
            break;
    }

    assetman_free_dynamic_assets();
}

void scenario_set_default(scenario_t* scenario)
{
    scenario->scenario_mode = SCENARIO_MODE_1V1;
    scenario->team = WHITE_TEAM;
    scenario->board_side_size = 8;
    scenario->flying_kings = true;
    scenario->peons_capture_backwards = false;
    scenario->is_white_peon_forward_top_to_bottom = true;
    scenario->applies_law_of_quantity = true;
    scenario->applies_law_of_quality = false;
    scenario->double_corner_on_right = true;
    memset(scenario->board.playable_cells, NO_PIECE, MAX_BOARD_PLAYABLE_CELL_COUNT);
}