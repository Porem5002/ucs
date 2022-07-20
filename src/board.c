#include "include/game.h"
#include "include/validation.h"

board_position_t movement_directions [4] = 
{
    { .x = 1,  .y = 1 },
    { .x = -1, .y = 1 },
    { .x = 1,  .y = -1 },
    { .x = -1, .y = -1 }
};

static inline bool are_coords_within_board_bounds(board_coordinate_t x, board_coordinate_t y)
{
    return (x >= 0 && x < game.scenario_data.board_side_size) && (y >= 0 && y < game.scenario_data.board_side_size);
}

static tree_t board_generate_capture_tree_for_move(board_t* current_board, move_info_t move);
static void board_get_all_capture_moves_of_team(board_t* board, dynarray(move_info_t)* capture_moves_array, team_t playing_team);
static void board_get_all_capture_moves_of_piece(board_t* board, dynarray(move_info_t)* capture_moves_array, cell_id_t piece_id);

team_t piece_team(cell_value_t piece_type)
{
    switch (piece_type)
    {
        case PIECE_WHITE_PEON:
        case PIECE_WHITE_QUEEN:
            return WHITE_TEAM;
        case PIECE_BLACK_PEON:
        case PIECE_BLACK_QUEEN:
            return BLACK_TEAM;
        default:
            return NO_TEAM;
    }
}

board_position_t cell_id_to_cell_position(cell_id_t cell_id)
{
    board_unit_t playable_cell_count_per_line = game.scenario_data.board_side_size/2;

    board_position_t cell_position;
    cell_position.x = (cell_id % playable_cell_count_per_line) * 2;
    cell_position.y = cell_id / playable_cell_count_per_line;
    
    if((!game.scenario_data.double_corner_on_right && cell_position.y % 2 != 0) || 
        (game.scenario_data.double_corner_on_right && cell_position.y % 2 == 0)) 
        cell_position.x++;

    return cell_position;
}

cell_id_t cell_position_to_cell_id(board_position_t cell_position)
{
    if((!game.scenario_data.double_corner_on_right && cell_position.y % 2 != 0) || 
        (game.scenario_data.double_corner_on_right && cell_position.y % 2 == 0)) 
        cell_position.x--;

    return cell_position.y * (game.scenario_data.board_side_size / 2) + cell_position.x / 2;
}

void board_apply_move(board_t* board, move_info_t move)
{
    board->playable_cells[move.destination_cell] = board->playable_cells[move.source_cell];
    board->playable_cells[move.source_cell] = NO_PIECE;

    if(!move.is_capture_move) return;

    board->playable_cells[move.capture_cell] = NO_PIECE;
}

tree_t board_generate_capture_tree(board_t* initial_board)
{
    board_t internal_board;
    tree_t capture_tree = rrr_tree_new(0, NULL);
    dynarray(move_info_t) capture_moves = dynarray_new(move_info_t, 0);
    
    board_get_all_capture_moves_of_team(initial_board, &capture_moves, game.current_team);

    for (size_t i = 0; i < dynarray_size(&capture_moves); i++)
    {
        tree_t subtree;
        move_info_t current_move = dynarray_ele(&capture_moves, move_info_t, i);

        internal_board = *initial_board;
        board_apply_move(&internal_board, current_move);
        
        subtree = board_generate_capture_tree_for_move(&internal_board, current_move);
        tree_insert_subtree(capture_tree, subtree);
    }

    dynarray_free(&capture_moves);

    if(game.scenario_data.applies_law_of_quantity) validation_capture_tree_apply_law_of_quantity(capture_tree);
    if(game.scenario_data.applies_law_of_quality) validation_capture_tree_apply_law_of_quality(capture_tree);

    return capture_tree;
}

static tree_t board_generate_capture_tree_for_move(board_t* current_board, move_info_t move)
{
    board_t internal_board;
    tree_t capture_tree = tree_new(move_info_t, &move);
    dynarray(move_info_t) capture_moves = dynarray_new(move_info_t, 0);
    
    board_position_t initial_move_source_position = cell_id_to_cell_position(move.source_cell);
    board_position_t initial_move_destination_position = cell_id_to_cell_position(move.destination_cell);

    board_position_t initial_movement_vector;
    initial_movement_vector.x = initial_move_destination_position.x - initial_move_source_position.x;
    initial_movement_vector.y = initial_move_destination_position.y - initial_move_source_position.y;

    board_coordinate_t initial_movement_magnitude = initial_movement_vector.x < 0 ? -initial_movement_vector.x : initial_movement_vector.x;

    board_position_t initial_movement_direction;
    initial_movement_direction.x = initial_movement_vector.x / initial_movement_magnitude;
    initial_movement_direction.y = initial_movement_vector.y / initial_movement_magnitude;

    board_get_all_capture_moves_of_piece(current_board, &capture_moves, move.destination_cell);

    for (size_t i = 0; i < dynarray_size(&capture_moves); i++)
    {
        tree_t subtree;
        move_info_t current_move = dynarray_ele(&capture_moves, move_info_t, i);

        board_position_t current_move_source_position = cell_id_to_cell_position(current_move.source_cell);
        board_position_t current_move_destination_position = cell_id_to_cell_position(current_move.destination_cell);

        board_position_t current_movement_vector;
        current_movement_vector.x = current_move_destination_position.x - current_move_source_position.x;
        current_movement_vector.y = current_move_destination_position.y - current_move_source_position.y;

        board_coordinate_t current_movement_magnitude = current_movement_vector.x < 0 ? -current_movement_vector.x : current_movement_vector.x;

        board_position_t current_movement_direction;
        current_movement_direction.x = current_movement_vector.x / current_movement_magnitude;
        current_movement_direction.y = current_movement_vector.y / current_movement_magnitude;

        if(initial_movement_direction.x == -current_movement_direction.x && initial_movement_direction.y == -current_movement_direction.y)
            continue;

        internal_board = *current_board;
        board_apply_move(&internal_board, current_move);

        subtree = board_generate_capture_tree_for_move(&internal_board, current_move);
        tree_insert_subtree(capture_tree, subtree);
    }

    dynarray_free(&capture_moves);

    return capture_tree;
}

static void board_get_all_capture_moves_of_team(board_t* board, dynarray(move_info_t)* capture_moves_array, team_t playing_team)
{
    for (cell_id_t cid = 0; cid < PLAYABLE_CELL_COUNT; cid++)
    {
        if(piece_team(board->playable_cells[cid]) != playing_team) continue;

        board_get_all_capture_moves_of_piece(board, capture_moves_array, cid);
    }
}

static void board_get_all_capture_moves_of_piece(board_t* board, dynarray(move_info_t)* capture_moves, cell_id_t piece_id)
{
    cell_value_t piece_type = board->playable_cells[piece_id];
    board_position_t piece_position = cell_id_to_cell_position(piece_id);

    if(piece_is_peon(piece_type) || !game.scenario_data.flying_kings)
    {
        for (size_t i = 0; i < 4; i++)
        {
            if(!game.scenario_data.peons_capture_backwards && !piece_is_queen(piece_type) && !validation_is_peon_moving_forward(piece_type, movement_directions[i])) continue;

            board_position_t destination_position;
            destination_position.x = piece_position.x + movement_directions[i].x*2;
            destination_position.y = piece_position.y + movement_directions[i].y*2;

            cell_id_t destination_id = cell_position_to_cell_id(destination_position);

            if(!are_coords_within_board_bounds(destination_position.x, destination_position.y) || board->playable_cells[destination_id] != NO_PIECE)
                continue;

            board_position_t position_of_piece_to_capture;
            position_of_piece_to_capture.x = piece_position.x + movement_directions[i].x;
            position_of_piece_to_capture.y = piece_position.y + movement_directions[i].y;

            cell_id_t id_of_piece_to_capture = cell_position_to_cell_id(position_of_piece_to_capture);

            if(board->playable_cells[id_of_piece_to_capture] != NO_PIECE && !piece_same_team(piece_type, board->playable_cells[id_of_piece_to_capture]))
            {
                move_info_t current_capture_move;
                current_capture_move.is_capture_move = true;
                current_capture_move.source_cell = piece_id;
                current_capture_move.capture_cell = id_of_piece_to_capture;
                current_capture_move.destination_cell = destination_id;

                dynarray_add(capture_moves, move_info_t, &current_capture_move); 
            }
        }
    }
    else if(piece_is_queen(piece_type))
    {
        for (size_t i = 0; i < 4; i++)
        {
            board_coordinate_t distance = 1;
            bool found_possible_capture_cell = false;
            cell_id_t possible_capture_cell;

            while (are_coords_within_board_bounds(piece_position.x + movement_directions[i].x*distance, piece_position.y + movement_directions[i].y*distance))
            {
                board_position_t target_piece_position;
                target_piece_position.x = piece_position.x + movement_directions[i].x * distance;
                target_piece_position.y = piece_position.y + movement_directions[i].y * distance;

                cell_id_t current_cell = cell_position_to_cell_id(target_piece_position);

                cell_value_t piece_to_be_eaten = board->playable_cells[current_cell];

                if(piece_to_be_eaten != NO_PIECE)
                {
                    possible_capture_cell = current_cell;
                    found_possible_capture_cell = !piece_same_team(piece_to_be_eaten, piece_type);
                    break;
                }

                distance++;
            }

            if(!found_possible_capture_cell) continue;

            distance++;

            move_info_t move;
            move.is_capture_move = true;
            move.source_cell = piece_id;
            move.capture_cell = possible_capture_cell;

            while (are_coords_within_board_bounds(piece_position.x + movement_directions[i].x*distance, piece_position.y + movement_directions[i].y*distance))
            {
                board_position_t target_piece_position;
                target_piece_position.x = piece_position.x + movement_directions[i].x * distance;
                target_piece_position.y = piece_position.y + movement_directions[i].y * distance;

                cell_id_t current_cell = cell_position_to_cell_id(target_piece_position);

                cell_value_t limiter_piece = board->playable_cells[current_cell];

                if(limiter_piece != NO_PIECE) break;

                move.destination_cell = current_cell;

                dynarray_add(capture_moves, move_info_t, &move); 

                distance++;
            }
        }
    }
}

bool board_contains_any_valid_moves_for_team(board_t* board, team_t playing_team)
{
    for (cell_id_t cid = 0; cid < PLAYABLE_CELL_COUNT; cid++)
    {
        if(piece_team(board->playable_cells[cid]) != playing_team) continue;

        cell_value_t piece_type = board->playable_cells[cid];
        board_position_t piece_position = cell_id_to_cell_position(cid);

        for (size_t i = 0; i < 4; i++)
        {
            board_position_t possible_pos1;
            possible_pos1.x = piece_position.x + movement_directions[i].x; 
            possible_pos1.y = piece_position.y + movement_directions[i].y;
            
            board_position_t possible_pos2;
            possible_pos2.x = piece_position.x + movement_directions[i].x*2; 
            possible_pos2.y = piece_position.y + movement_directions[i].y*2;

            if(!are_coords_within_board_bounds(possible_pos1.x, possible_pos1.y)) continue;

            bool is_valid_non_eat_movement = piece_is_queen(piece_type) || (piece_is_peon(piece_type) && validation_is_peon_moving_forward(piece_type, movement_directions[i]));

            cell_id_t cell_id_pos1 = cell_position_to_cell_id(possible_pos1);

            if(is_valid_non_eat_movement && board->playable_cells[cell_id_pos1] == NO_PIECE) return true;

            if(!are_coords_within_board_bounds(possible_pos2.x, possible_pos2.y)) continue;
            
            cell_id_t cell_id_pos2 = cell_position_to_cell_id(possible_pos2);

            if((is_valid_non_eat_movement || game.scenario_data.peons_capture_backwards) && piece_team(board->playable_cells[cell_id_pos1]) != playing_team && board->playable_cells[cell_id_pos2] == NO_PIECE) 
                return true;
        }
    }

    return false;
}