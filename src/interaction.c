#include <stdio.h>

#include "include/game.h"
#include "include/sui.h"
#include "include/interaction.h"
#include "include/validation.h"
#include "include/rendering.h"
#include "include/logger.h"
#include "include/assetman_setup.h"

static void move_selected_piece_in_1v1_scenario(incomplete_move_info_t incomplete_move);
static void move_selected_piece_in_challenge_scenario(incomplete_move_info_t incomplete_move);
static void switch_teams();
static bool is_crowning_cell_of_team(team_t team, cell_id_t cell);
static void promote_to_queen_if_valid(cell_id_t piece_cell);
static void place_piece_on_hovered_cell();

void challenge_auto_play()
{
    if(game.auto_play_current_cooldown > 0)
    {
        game.auto_play_current_cooldown -= game.delta_time;
        return;
    }

    game.auto_play_current_cooldown = AUTO_PLAY_COOLDOWN;

    move_info_t expected_move = array_ele(&game.scenario_data.challenge_moves, move_info_t, game.current_challenge_move_index);
    game.current_challenge_move_index++;
    
    board_apply_move(&game.scenario_data.board, expected_move, false);

    move_info_t next_expected_move = array_ele(&game.scenario_data.challenge_moves, move_info_t, game.current_challenge_move_index);

    if(expected_move.destination_cell == next_expected_move.source_cell) return;

    promote_to_queen_if_valid(expected_move.destination_cell);
    switch_teams();
}

void on_screen_clicked(Uint8 button_clicked, Sint32 pixelX, Sint32 pixelY)
{
    uint8_t initial_mode = game.mode;

    if(button_clicked == SDL_BUTTON_LEFT) sui_check_buttons(pixelX, pixelY);

    if(game.mode != initial_mode) return;
    
    if(game.mode == MODE_EDITOR)
    {
        place_piece_on_hovered_cell();
        LOGGER_LOGS("Editor Place Piece");
        return;
    }

    if(game.mode != MODE_SCENARIO) return;

    switch (button_clicked)
    {
        case SDL_BUTTON_LEFT:
            select_hovered_piece();
            break;
        case SDL_BUTTON_RIGHT:
            move_selected_piece_to_hovered_cell();
            break;
        default:
            break;
    }
}

static void place_piece_on_hovered_cell()
{
    if(game.is_cell_hovered) 
        game.scenario_data.board.playable_cells[game.currently_hovered_cell_id] = game.piece_type_to_place;
}

void select_hovered_piece()
{
    if(!game.is_cell_hovered) return;

    if(game.scenario_data.board.playable_cells[game.currently_hovered_cell_id] == NO_PIECE) return;

    game.is_piece_selected = true;
    game.selected_piece_cell_id = game.currently_hovered_cell_id;
}

void move_selected_piece_to_hovered_cell()
{
    if(!game.is_piece_selected || !game.is_cell_hovered) return;

    incomplete_move_info_t incomplete_move;
    incomplete_move.source_cell = game.selected_piece_cell_id;
    incomplete_move.destination_cell = game.currently_hovered_cell_id;

    switch (game.scenario_data.scenario_mode)
    {
        case SCENARIO_MODE_1V1:
            move_selected_piece_in_1v1_scenario(incomplete_move);
            break;
        case SCENARIO_MODE_CHALLENGE:
            move_selected_piece_in_challenge_scenario(incomplete_move);
            break;
        default:
            break;
    }

    game_check_for_and_activate_victory();
}

void update_currently_hovered_cell()
{
    int windowX; 
    int windowY;

    float pixelX;
    float pixelY;

    SDL_GetMouseState(&windowX, &windowY);

    SDL_RenderWindowToLogical(game.renderer, windowX, windowY, &pixelX, &pixelY);

    if(pixelX < game.screen_scenario_board_rect.x || pixelX > game.screen_scenario_board_rect.x + BOARD_SECTION_WIDTH)
    {
        game.is_cell_hovered = false;
        return;
    }

    board_position_t cell_position;
    cell_position.x = game.scenario_data.board_side_size * ((int)pixelX - game.screen_scenario_board_rect.x) / BOARD_SECTION_WIDTH;
    cell_position.y = game.scenario_data.board_side_size * (int)pixelY / BOARD_SECTION_HEIGHT;

    game.currently_hovered_cell_id = cell_position_to_cell_id(cell_position);
    game.is_cell_hovered = true;
}

void update_team_displayer()
{
    if(game.current_team == WHITE_TEAM)
    {
        game.team_displayer->color = (SDL_Color){ WHITE_PIECE_COLOR_VALS, 255 };
    }
    else
    {
        game.team_displayer->color = (SDL_Color){ BLACK_PIECE_COLOR_VALS, 255 };
    }
}

static bool is_crowning_cell_of_team(team_t team, cell_id_t cell)
{
    if(game.scenario_data.is_white_peon_forward_top_to_bottom)
    {
        if(team == WHITE_TEAM) return cell < PLAYABLE_CELL_COUNT && cell >= (PLAYABLE_CELL_COUNT - PLAYABLE_CELL_COUNT_PER_LINE);
        
        return cell < PLAYABLE_CELL_COUNT_PER_LINE && cell >= 0;
    }
    
    if(team == WHITE_TEAM) return cell < PLAYABLE_CELL_COUNT_PER_LINE && cell >= 0;
    
    return cell < PLAYABLE_CELL_COUNT && cell >= (PLAYABLE_CELL_COUNT - PLAYABLE_CELL_COUNT_PER_LINE);        
}

static void move_selected_piece_in_1v1_scenario(incomplete_move_info_t incomplete_move)
{
    tree_t updated_current_capture_tree;
    move_info_t complete_move;
    bool was_move_validated = validate_move_based_on_rules(incomplete_move, &updated_current_capture_tree);

    if(!was_move_validated) return;

    if(game.force_capture_move)
    {
        complete_move = tree_value(updated_current_capture_tree, move_info_t);
    }
    else
    {
        complete_move.is_capture_move = false;
        complete_move.source_cell = incomplete_move.source_cell;
        complete_move.destination_cell = incomplete_move.destination_cell;
    }

    board_apply_move(&game.scenario_data.board, complete_move, false);

    if(updated_current_capture_tree != NULL && tree_child_count(updated_current_capture_tree) > 0) 
    {
        game.force_capture_move = true;
        game.is_piece_selected = true;
        game.selected_piece_cell_id = complete_move.destination_cell;
        game.current_capture_subtree = updated_current_capture_tree;
        return;
    }

    promote_to_queen_if_valid(complete_move.destination_cell);
    switch_teams();

    game.contains_last_move_info = true;
    game.last_move_source_cell_id = complete_move.source_cell;
    game.last_move_dest_cell_id = complete_move.destination_cell;
}

static void move_selected_piece_in_challenge_scenario(incomplete_move_info_t incomplete_move)
{
    move_info_t expected_move = array_ele(&game.scenario_data.challenge_moves, move_info_t, game.current_challenge_move_index);

    if(incomplete_move.source_cell != expected_move.source_cell || incomplete_move.destination_cell != expected_move.destination_cell)
    {
        game.challenge_feedback_displayer->texture = assetman_get_asset(INCORRECT_MOVE_IMAGE_ID);
        return;
    }

    game.challenge_feedback_displayer->texture = assetman_get_asset(CORRECT_MOVE_IMAGE_ID);
    
    game.current_challenge_move_index++;

    board_apply_move(&game.scenario_data.board, expected_move, false);

    if(array_size(&game.scenario_data.challenge_moves) == game.current_challenge_move_index) return;

    move_info_t next_expected_move = array_ele(&game.scenario_data.challenge_moves, move_info_t, game.current_challenge_move_index);

    if(expected_move.destination_cell == next_expected_move.source_cell)
    {
        game.is_piece_selected = true;
        game.selected_piece_cell_id = expected_move.destination_cell;
        return;
    }
    
    promote_to_queen_if_valid(expected_move.destination_cell);
    switch_teams();

    game.contains_last_move_info = true;
    game.last_move_source_cell_id = expected_move.source_cell;
    game.last_move_dest_cell_id = expected_move.destination_cell;
}

static void promote_to_queen_if_valid(cell_id_t piece_cell)
{
    cell_value_t type_of_moved_piece = game.scenario_data.board.playable_cells[piece_cell];

    if(piece_is_peon(type_of_moved_piece) && is_crowning_cell_of_team(game.current_team, piece_cell))
    {
        game.scenario_data.board.playable_cells[piece_cell] = piece_promote_to_queen(type_of_moved_piece);
    }
}

static void switch_teams()
{
    game.is_piece_selected = false;

    game.current_team = game.current_team == WHITE_TEAM ? BLACK_TEAM : WHITE_TEAM;
    update_team_displayer();

    if(game.scenario_data.scenario_mode == SCENARIO_MODE_1V1)
    {
        tree_free(game.capture_tree);
        game_1v1_scenario_set_capture_data();
    }
}