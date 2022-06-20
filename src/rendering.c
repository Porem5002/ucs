#include "include/assetman_setup.h"
#include "include/rendering.h"
#include "include/sui.h"

static void render_piece(cell_value_t piece, int x, int y);
static void render_cell(cell_id_t cell, Uint8 r, Uint8 g, Uint8 b);
static void render_frame_scenario();
static void render_frame_editor();

void render_frame()
{
    switch (game.mode)
    {
        case MODE_MENU:
            sui_draw_elements(game.renderer);
            break;
        case MODE_EDITOR:
            render_frame_editor();
            break;
        case MODE_FILE_BROWSER:
            sui_draw_elements(game.renderer);
            break;
        case MODE_SCENARIO:
            render_frame_scenario();
            break;
        default:
            break;
    }
}

static void render_piece(cell_value_t piece, int x, int y)
{
    SDL_Rect piece_rect;
    piece_rect.x = game.screen_scenario_board_rect.x + x * CELL_WIDTH + PIECE_PIXEL_OFFSET_X;
    piece_rect.y = game.screen_scenario_board_rect.y + y * CELL_HEIGHT + PIECE_PIXEL_OFFSET_Y; 
    piece_rect.w = PIECE_WIDTH; 
    piece_rect.h = PIECE_HEIGHT;
    
    switch (piece)
    {
        case PIECE_WHITE_PEON:
            SDL_RenderCopy(game.renderer, assetman_get_asset(WHITE_PEON_TEXTURE_ID), NULL, &piece_rect);
            break;
        case PIECE_WHITE_QUEEN:
            SDL_RenderCopy(game.renderer, assetman_get_asset(WHITE_QUEEN_TEXTURE_ID), NULL, &piece_rect);
            break;
        case PIECE_BLACK_PEON:
            SDL_RenderCopy(game.renderer, assetman_get_asset(BLACK_PEON_TEXTURE_ID), NULL, &piece_rect);
            break;
        case PIECE_BLACK_QUEEN:
            SDL_RenderCopy(game.renderer, assetman_get_asset(BLACK_QUEEN_TEXTURE_ID), NULL, &piece_rect);
            break;
    }

    SDL_SetRenderDrawColor(game.renderer, WHITE_CELLS_COLOR_VALS, 255);
}

static void render_cell(cell_id_t cell, Uint8 r, Uint8 g, Uint8 b)
{
    board_position_t cell_position = cell_id_to_cell_position(cell);
    
    SDL_Rect cell_rect;
    cell_rect.x = game.screen_scenario_board_rect.x + cell_position.x * CELL_WIDTH;
    cell_rect.y = game.screen_scenario_board_rect.y + cell_position.y * CELL_HEIGHT;
    cell_rect.w = CELL_WIDTH;
    cell_rect.h = CELL_HEIGHT;

    SDL_SetRenderDrawColor(game.renderer, r, g, b, 255);
    SDL_RenderFillRect(game.renderer, &cell_rect);
}

static void render_board(board_t* board)
{
    bool is_playable_cell = !game.scenario_data.double_corner_on_right;

    SDL_Rect cell_rect;
    cell_rect.w = CELL_WIDTH;
    cell_rect.h = CELL_HEIGHT;

    board_unit_t x;
    board_unit_t y;

    for(y = 0; y < game.scenario_data.board_side_size; y++)
    {
        for(x = 0; x < game.scenario_data.board_side_size; x++)
        {
            cell_rect.x = game.screen_scenario_board_rect.x + x * CELL_WIDTH;
            cell_rect.y = game.screen_scenario_board_rect.y + y * CELL_HEIGHT;

            if(is_playable_cell) 
            {   
                SDL_SetRenderDrawColor(game.renderer, WHITE_CELLS_COLOR_VALS, 255);
                SDL_RenderFillRect(game.renderer, &cell_rect);
            }
            else
            {
                SDL_SetRenderDrawColor(game.renderer, BLACK_CELLS_COLOR_VALS, 255);
                SDL_RenderFillRect(game.renderer, &cell_rect);
            }
            
            is_playable_cell = !is_playable_cell;
        }

        is_playable_cell = !is_playable_cell;
    }
}

static void render_board_pieces(board_t* board)
{
    bool is_playable_cell = !game.scenario_data.double_corner_on_right;

    board_unit_t x;
    board_unit_t y;

    cell_id_t cell_index = 0;

    for(y = 0; y < game.scenario_data.board_side_size; y++)
    {
        for(x = 0; x < game.scenario_data.board_side_size; x++)
        {
            if(is_playable_cell) 
            {   
                if(board->playable_cells[cell_index] != NO_PIECE)
                    render_piece(board->playable_cells[cell_index], x, y);
                
                cell_index++;
            }
            
            is_playable_cell = !is_playable_cell;
        }

        is_playable_cell = !is_playable_cell;
    }
}

static void render_frame_scenario()
{
    render_board(&game.scenario_data.board);

    /*----------------------------
            Player Feedback
    -----------------------------*/

    if(game.contains_last_move_info)
    {
        render_cell(game.last_move_source_cell_id, LAST_MOVE_COLOR_VALS);
        render_cell(game.last_move_dest_cell_id, LAST_MOVE_COLOR_VALS);
    }

    if(game.is_cell_hovered) render_cell(game.currently_hovered_cell_id, HOVERED_CELL_COLOR_VALS);

    if(game.is_piece_selected) render_cell(game.selected_piece_cell_id, SELECTED_CELL_COLOR_VALS);

    render_board_pieces(&game.scenario_data.board);
    sui_draw_elements(game.renderer);
}

static void render_frame_editor()
{
    render_board(&game.scenario_data.board);
    
    if(game.is_cell_hovered) render_cell(game.currently_hovered_cell_id, HOVERED_CELL_COLOR_VALS);
    
    render_board_pieces(&game.scenario_data.board);
    sui_draw_elements(game.renderer);
}