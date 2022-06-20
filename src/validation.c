#include "include/validation.h"
#include "include/logger.h"
#include "include/assetman_setup.h"
#include "include/rendering.h"

extern board_position_t movement_directions [4];

static size_t validation_capture_tree_max_points(tree_t tree);
static void internal_validation_capture_tree_max_points(tree_t tree, size_t* current_max_points, size_t current_points);
static void internal_validation_capture_tree_apply_law_of_quantity(tree_t tree, size_t value);
static void internal_validation_capture_tree_apply_law_of_quality(tree_t tree, size_t value);

bool validation_is_peon_moving_forward(cell_value_t piece_type, board_position_t movement)
{
    if(game.scenario_data.is_white_peon_forward_top_to_bottom)
    {
        return (piece_type == PIECE_WHITE_PEON && movement.y > 0) || (piece_type == PIECE_BLACK_PEON && movement.y < 0);
    } 

    return (piece_type == PIECE_WHITE_PEON && movement.y < 0) || (piece_type == PIECE_BLACK_PEON && movement.y > 0); 
}

bool validate_move_based_on_rules(incomplete_move_info_t move, tree_t* out_updated_capture_tree)
{
    cell_value_t piece_to_move = game.scenario_data.board.playable_cells[move.source_cell];
    cell_value_t piece_occupying_destination = game.scenario_data.board.playable_cells[move.destination_cell];

    if(piece_to_move == NO_PIECE || piece_occupying_destination != NO_PIECE) return false;

    if(piece_team(piece_to_move) != game.current_team) return false;

    *out_updated_capture_tree = NULL;

    if(game.force_capture_move) 
    {
        for (size_t i = 0; i < tree_child_count(game.current_capture_subtree); i++)
        {
            tree_t child = tree_get_subtree(game.current_capture_subtree, i);
            move_info_t child_move = tree_value(child, move_info_t); 

            if(child_move.source_cell == move.source_cell && 
               child_move.destination_cell == move.destination_cell)
            {
                *out_updated_capture_tree = child;
                return true;
            }
        }

        return false;
    }
    
    board_position_t source_position = cell_id_to_cell_position(move.source_cell);
    board_position_t destination_position = cell_id_to_cell_position(move.destination_cell);

    board_position_t movement_vector;
    movement_vector.x = destination_position.x - source_position.x;
    movement_vector.y = destination_position.y - source_position.y; 

    if(!IS_DIAGONAL(movement_vector)) return false;

    board_coordinate_t distance_to_move = movement_vector.x < 0 ? -movement_vector.x : movement_vector.x;

    if(piece_is_queen(piece_to_move))
    {
        if(distance_to_move > 1 && !game.scenario_data.flying_kings) return false;

        for (size_t i = 1; i < distance_to_move; i++)
        {
            board_coordinate_t x_multiplier = movement_vector.x > 0 ? 1 : -1; 
            board_coordinate_t y_multiplier = movement_vector.y > 0 ? 1 : -1;

            board_position_t middle_movement_position;
            middle_movement_position.x = source_position.x + i * x_multiplier;
            middle_movement_position.y = source_position.y + i * y_multiplier;

            cell_id_t cell_of_middle_movement = cell_position_to_cell_id(middle_movement_position);
            
            if(game.scenario_data.board.playable_cells[cell_of_middle_movement] != NO_PIECE) return false;
        }

        return true;
    }

    return validation_is_peon_moving_forward(piece_to_move, movement_vector) && distance_to_move == 1;
}

void validation_capture_tree_apply_law_of_quantity(tree_t tree)
{
    internal_validation_capture_tree_apply_law_of_quantity(tree, tree_max_depth(tree));
}

void validation_capture_tree_apply_law_of_quality(tree_t tree)
{
    internal_validation_capture_tree_apply_law_of_quality(tree, validation_capture_tree_max_points(tree));
}

static size_t validation_capture_tree_max_points(tree_t tree)
{
    size_t max_points = 0;
    internal_validation_capture_tree_max_points(tree, &max_points, 0);
    return max_points;
}

static void internal_validation_capture_tree_max_points(tree_t tree, size_t* current_max_points, size_t current_points)
{
    if(current_points >= *current_max_points) *current_max_points = current_points;

    for (size_t i = 0; i < tree->leaf_count; i++)
    {
        move_info_t current_move = tree_value(tree->leafs[i], move_info_t);
        
        if(piece_is_queen(game.scenario_data.board.playable_cells[current_move.capture_cell]))
        {
            internal_validation_capture_tree_max_points(tree->leafs[i], current_max_points, current_points + 2);
        }
        else
        {
            internal_validation_capture_tree_max_points(tree->leafs[i], current_max_points, current_points + 1);
        }
    }
}

static void internal_validation_capture_tree_apply_law_of_quantity(tree_t tree, size_t value)
{
    for (size_t i = 0; i < tree->leaf_count; i++)
    {
        size_t max_depth = tree_max_depth(tree->leafs[i]);

        if(max_depth < value - 1) 
        {
            tree_remove_subtree(tree, i);
            i--;
            continue;
        }

        internal_validation_capture_tree_apply_law_of_quantity(tree->leafs[i], value - 1);
    }
}

static void internal_validation_capture_tree_apply_law_of_quality(tree_t tree, size_t value)
{
    for (size_t i = 0; i < tree->leaf_count; i++)
    {
        size_t max_points = validation_capture_tree_max_points(tree->leafs[i]);
        move_info_t current_move = tree_value(tree->leafs[i], move_info_t);
        size_t point_decrement;
        
        if(piece_is_queen(game.scenario_data.board.playable_cells[current_move.capture_cell]))
            point_decrement = 2;
        else
            point_decrement = 1;

        if(max_points < value - point_decrement) 
        {
            tree_remove_subtree(tree, i);
            i--;
            continue;
        }

        internal_validation_capture_tree_apply_law_of_quality(tree->leafs[i], value - point_decrement);
    }
}