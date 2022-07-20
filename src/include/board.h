#ifndef DEF_BOARD_HEADER
#define DEF_BOARD_HEADER

#include <stdint.h>
#include <stdbool.h>

#define DTS_USE_ARRAY
#define DTS_USE_DYNARRAY
#define DTS_USE_TREE

#include "dtstructs.h"

#define MAX_BOARD_SIDE_DIMENSION 12
#define MAX_BOARD_PLAYABLE_CELL_COUNT ((MAX_BOARD_SIDE_DIMENSION*MAX_BOARD_SIDE_DIMENSION)/2) 

#define NO_TEAM 0
#define WHITE_TEAM 1
#define BLACK_TEAM 2

#define piece_promote_to_queen(PIECE_TYPE) (-PIECE_TYPE)
#define piece_is_queen(PIECE_TYPE) (PIECE_TYPE == PIECE_WHITE_QUEEN || PIECE_TYPE == PIECE_BLACK_QUEEN)
#define piece_is_peon(PIECE_TYPE) (PIECE_TYPE == PIECE_WHITE_PEON || PIECE_TYPE == PIECE_BLACK_PEON)
#define piece_is_white(PIECE_TYPE) (PIECE_TYPE == PIECE_WHITE_PEON || PIECE_TYPE == PIECE_WHITE_QUEEN)
#define piece_is_black(PIECE_TYPE) (PIECE_TYPE == PIECE_BLACK_PEON || PIECE_TYPE == PIECE_BLACK_QUEEN)
#define piece_same_team(PIECE_TYPE_1, PIECE_TYPE_2) (PIECE_TYPE_1 == PIECE_TYPE_2 || PIECE_TYPE_1 == (-PIECE_TYPE_2))

typedef uint8_t  board_unit_t;
typedef int16_t  board_coordinate_t; 
typedef uint16_t cell_id_t;
typedef int8_t   cell_value_t;
typedef int8_t   team_t;

enum
{
    NO_PIECE           =  0,
    PIECE_BLACK_QUEEN  = -2,
    PIECE_WHITE_QUEEN  = -1,
    PIECE_WHITE_PEON   =  1,
    PIECE_BLACK_PEON   =  2
};

typedef struct 
{
    bool is_capture_move;
    cell_id_t capture_cell;
    cell_id_t source_cell;
    cell_id_t destination_cell;
} move_info_t;

typedef struct 
{
    cell_id_t source_cell;
    cell_id_t destination_cell;
} incomplete_move_info_t;

typedef struct 
{
    board_coordinate_t x;
    board_coordinate_t y;
} board_position_t;

typedef struct 
{
    cell_value_t playable_cells [MAX_BOARD_PLAYABLE_CELL_COUNT];
} board_t;

team_t piece_team(cell_value_t piece_type);

board_position_t cell_id_to_cell_position(cell_id_t cell_id);

cell_id_t cell_position_to_cell_id(board_position_t cell_position);

void board_apply_move(board_t* board, move_info_t move);

tree_t board_generate_capture_tree(board_t* initial_board);

bool board_contains_any_valid_moves_for_team(board_t* board, team_t playing_team);

#endif