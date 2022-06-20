#ifndef VALIDATION_HEADER
#define VALIDATION_HEADER

#include "game.h"
#include "interaction.h"

bool validation_is_peon_moving_forward(cell_value_t piece_type, board_position_t movement);

bool validate_move(move_info_t* move, bool* output_rule_validation, bool* output_solution_validation);

bool validate_move_based_on_rules(incomplete_move_info_t move, tree_t* out_updated_capture_tree);

void validation_capture_tree_apply_law_of_quantity(tree_t capture_tree);

void validation_capture_tree_apply_law_of_quality(tree_t capture_tree);

#endif