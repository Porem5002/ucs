#ifndef MOVES_HEADER 
#define MOVES_HEADER

#include <stdbool.h>

#include "game.h"

#define IS_DIAGONAL(MOVEMENT_VECTOR) ((MOVEMENT_VECTOR).x == (MOVEMENT_VECTOR).y || (MOVEMENT_VECTOR).x == -(MOVEMENT_VECTOR).y)

void on_screen_clicked(Uint8 button_clicked, Sint32 pixelX, Sint32 pixelY);

void select_hovered_piece();

void move_selected_piece_to_hovered_cell();

void update_currently_hovered_cell();

void update_team_displayer();

void challenge_auto_play();

#endif