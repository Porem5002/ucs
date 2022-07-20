#include <inttypes.h>

#include "include/game.h"
#include "include/assetman_setup.h"
#include "include/scenario_loader.h"
#include "include/editor.h"
#include "include/rendering.h"

#define BOARD_8X8_TEXTURE_ID (assetman_dynamic_id(0))
#define BOARD_10X10_TEXTURE_ID (assetman_dynamic_id(1))
#define BOARD_12X12_TEXTURE_ID (assetman_dynamic_id(2))

#define DOUBLE_CORNER_LEFT_TEXTURE_ID (assetman_dynamic_id(3))
#define DOUBLE_CORNER_RIGHT_TEXTURE_ID (assetman_dynamic_id(4))

#define SAVE_BUTTON_TEXTURE_ID (assetman_dynamic_id(5))

#define FALSE_VALUE_TEXTURE_ID (assetman_dynamic_id(6))
#define TRUE_VALUE_TEXTURE_ID (assetman_dynamic_id(7))

#define MAIN_SECTION_TEXTURE_ID (assetman_dynamic_id(8))
#define RULES_SECTION_TEXTURE_ID (assetman_dynamic_id(9))
#define PIECES_SECTION_TEXTURE_ID (assetman_dynamic_id(10))

#define SELECTED_MAIN_SECTION_TEXTURE_ID (assetman_dynamic_id(11))
#define SELECTED_RULES_SECTION_TEXTURE_ID (assetman_dynamic_id(12))
#define SELECTED_PIECES_SECTION_TEXTURE_ID (assetman_dynamic_id(13))

#define BOARD_FIELD_TEXTURE_ID (assetman_dynamic_id(14))
#define DOUBLE_CORNER_FIELD_TEXTURE_ID (assetman_dynamic_id(15))

#define INITIAL_PLAYER_FIELD_TEXTURE_ID (assetman_dynamic_id(16))
#define PEONS_CAPTURE_BACKWARDS_FIELD_TEXTURE_ID (assetman_dynamic_id(17))
#define LAW_OF_QUANTITY_FIELD_TEXTURE_ID (assetman_dynamic_id(18))
#define LAW_OF_QUALITY_FIELD_TEXTURE_ID (assetman_dynamic_id(19))
#define FLYING_KINGS_FIELD_TEXTURE_ID (assetman_dynamic_id(20))

static void editor_section_navbar(uint8_t selected_section);
static void editor_set_main_section(void* event_data);
static void editor_set_rules_section(void* event_data);
static void editor_set_pieces_section(void* event_data);

static void save_single_cell_assignment(FILE* f, cell_id_t cid, cell_value_t piece_type);
static void save_multi_cell_assignment(FILE* f, cell_id_t start_cid, cell_id_t end_cid, cell_value_t piece_type);

static void clear_placeable_piece();
static void set_placeable_white_peon();
static void set_placeable_black_peon();
static void set_placeable_white_queen();
static void set_placeable_black_queen();
static void switch_starting_team(void*);
static void switch_flying_kings(void*);
static void switch_law_of_quantity(void*);
static void switch_law_of_quality(void*);
static void switch_double_corner_side(void*);
static void switch_board_size(void*);

static string_t scenario_mode_to_str(uint8_t mode);
static string_t boolean_to_str(bool boolean);
static string_t team_to_str(team_t team);
static string_t peon_movement_to_str(bool is_white_peon_forward_top_to_bottom);

void game_set_mode_editor(void* event_data)
{
    LOGGER_LOGS("Started loading Editor!");

    game_free_dependencies();
    sui_clear_elements();

    game.piece_type_to_place = NO_PIECE;

    game.mode = MODE_EDITOR;
    game.update = game_update_editor;

    scenario_set_default(&game.scenario_data);

    game.screen_scenario_board_rect = (SDL_Rect){ 0, 0, BOARD_SECTION_WIDTH, BOARD_SECTION_HEIGHT };
    game.screen_scenario_ui_rect = (SDL_Rect){ BOARD_SECTION_WIDTH, 0, SCREEN_WIDTH - BOARD_SECTION_WIDTH, BOARD_SECTION_HEIGHT };

    SDL_Texture* main_section_label = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "PRINCIPAL", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* rules_section_label = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "REGRAS", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* pieces_section_label = sui_texture_from_utf8_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "PEÇAS", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });

    SDL_Texture* selected_main_section_label = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "PRINCIPAL", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
    SDL_Texture* selected_rules_section_label = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "REGRAS", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
    SDL_Texture* selected_pieces_section_label = sui_texture_from_utf8_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "PEÇAS", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });

    SDL_Texture* true_value_label = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "SIM", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
    SDL_Texture* false_value_label = sui_texture_from_utf8_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "NÃO", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });

    SDL_Texture* board_size_label_8x8 = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "8x8", (SDL_Color){0,0,0,255});
    SDL_Texture* board_size_label_10x10 = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "10x10", (SDL_Color){0,0,0,255});
    SDL_Texture* board_size_label_12x12 = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "12x12", (SDL_Color){0,0,0,255});

    SDL_Texture* double_corner_label_right = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "DIREITO", (SDL_Color){0,0,0,255});
    SDL_Texture* double_corner_label_left = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "ESQUERDO", (SDL_Color){0,0,0,255});

    SDL_Texture* save_label = sui_texture_from_text(game.renderer, assetman_get_asset(MAIN_FONT_ID), "SALVAR", (SDL_Color){0,0,0,255});

    assetman_set_asset(MAIN_SECTION_TEXTURE_ID, TEXTURE_ASSET_TYPE, main_section_label);
    assetman_set_asset(RULES_SECTION_TEXTURE_ID, TEXTURE_ASSET_TYPE, rules_section_label);
    assetman_set_asset(PIECES_SECTION_TEXTURE_ID, TEXTURE_ASSET_TYPE, pieces_section_label);

    assetman_set_asset(SELECTED_MAIN_SECTION_TEXTURE_ID, TEXTURE_ASSET_TYPE, selected_main_section_label);
    assetman_set_asset(SELECTED_RULES_SECTION_TEXTURE_ID, TEXTURE_ASSET_TYPE, selected_rules_section_label);
    assetman_set_asset(SELECTED_PIECES_SECTION_TEXTURE_ID, TEXTURE_ASSET_TYPE, selected_pieces_section_label);

    assetman_set_asset(TRUE_VALUE_TEXTURE_ID, TEXTURE_ASSET_TYPE, true_value_label);
    assetman_set_asset(FALSE_VALUE_TEXTURE_ID, TEXTURE_ASSET_TYPE, false_value_label);

    assetman_set_asset(BOARD_8X8_TEXTURE_ID, TEXTURE_ASSET_TYPE, board_size_label_8x8);
    assetman_set_asset(BOARD_10X10_TEXTURE_ID, TEXTURE_ASSET_TYPE, board_size_label_10x10);
    assetman_set_asset(BOARD_12X12_TEXTURE_ID, TEXTURE_ASSET_TYPE, board_size_label_12x12);
    assetman_set_asset(DOUBLE_CORNER_LEFT_TEXTURE_ID, TEXTURE_ASSET_TYPE, double_corner_label_left);
    assetman_set_asset(DOUBLE_CORNER_RIGHT_TEXTURE_ID, TEXTURE_ASSET_TYPE, double_corner_label_right);
    
    assetman_set_asset(SAVE_BUTTON_TEXTURE_ID, TEXTURE_ASSET_TYPE, save_label);

    SDL_Texture* board_field_label = sui_texture_from_text(game.renderer, assetman_get_asset(BROWSER_FONT_ID), "TABULEIRO", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* double_corner_field_label = sui_texture_from_text(game.renderer, assetman_get_asset(BROWSER_FONT_ID), "CANTO DUPLO", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });

    assetman_set_asset(BOARD_FIELD_TEXTURE_ID, TEXTURE_ASSET_TYPE, board_field_label);
    assetman_set_asset(DOUBLE_CORNER_FIELD_TEXTURE_ID, TEXTURE_ASSET_TYPE, double_corner_field_label);

    SDL_Texture* initial_player_field_label = sui_texture_from_text(game.renderer, assetman_get_asset(BROWSER_FONT_ID), "JOGADOR INICIAL", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* law_of_quantity_field_label = sui_texture_from_text(game.renderer, assetman_get_asset(BROWSER_FONT_ID), "LEI DA QUANTIDADE", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* law_of_quality_field_label = sui_texture_from_text(game.renderer, assetman_get_asset(BROWSER_FONT_ID), "LEI DA QUALIDADE", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* peons_capture_backwards_field_label = sui_texture_from_text(game.renderer, assetman_get_asset(BROWSER_FONT_ID), "PEÕS CAPTURAM PARA TRÁS", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* flying_kings_field_label = sui_texture_from_text(game.renderer, assetman_get_asset(BROWSER_FONT_ID), "DAMAS VOADORAS", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });

    assetman_set_asset(INITIAL_PLAYER_FIELD_TEXTURE_ID, TEXTURE_ASSET_TYPE, initial_player_field_label);
    assetman_set_asset(FLYING_KINGS_FIELD_TEXTURE_ID, TEXTURE_ASSET_TYPE, flying_kings_field_label);
    assetman_set_asset(PEONS_CAPTURE_BACKWARDS_FIELD_TEXTURE_ID, TEXTURE_ASSET_TYPE, peons_capture_backwards_field_label);
    assetman_set_asset(LAW_OF_QUANTITY_FIELD_TEXTURE_ID, TEXTURE_ASSET_TYPE, law_of_quantity_field_label);
    assetman_set_asset(LAW_OF_QUALITY_FIELD_TEXTURE_ID, TEXTURE_ASSET_TYPE, law_of_quality_field_label);

    editor_set_main_section(NULL);
    
    LOGGER_LOGS("Finished loading Editor!");
}

void save_scenario_as_sch_file(void* event_data)
{
    scenario_t* scenario = event_data;

    FILE* f = fopen(PATH_SCENARIOS_EDITOR "editor.sch", "wb");

    if(f == NULL) exit(EXIT_FAILURE);

    fprintf(f, "SCENARIO_TYPE : %s\n", scenario_mode_to_str(scenario->scenario_mode));

    fprintf(f, "TEAM : %s\n", team_to_str(scenario->team));
    fprintf(f, "BOARD : %"PRId16"\n", scenario->board_side_size);
    fprintf(f, "FLYING_KINGS : %s\n", boolean_to_str(scenario->flying_kings));
    fprintf(f, "PEONS_CAPTURE_BACKWARDS : %s\n", boolean_to_str(scenario->peons_capture_backwards));
    fprintf(f, "PEONS_MOVEMENT : %s\n", peon_movement_to_str(scenario->is_white_peon_forward_top_to_bottom));
    fprintf(f, "APPLY_LAW_OF_QUANTITY : %s\n", boolean_to_str(scenario->applies_law_of_quantity));
    fprintf(f, "APPLY_LAW_OF_QUALITY : %s\n", boolean_to_str(scenario->applies_law_of_quality));
    fprintf(f, "DOUBLE_CORNER_SIDE : %s\n", boolean_to_str(scenario->double_corner_on_right));

    for (cell_id_t cid = 0; cid < PLAYABLE_CELL_COUNT; cid++)
    {
        cell_value_t piece_type = scenario->board.playable_cells[cid];

        cell_id_t start_cid = cid;

        while (cid+1 < PLAYABLE_CELL_COUNT && scenario->board.playable_cells[cid+1] == piece_type) cid++;

        if(start_cid == cid)
            save_single_cell_assignment(f, cid, piece_type);
        else
            save_multi_cell_assignment(f, start_cid, cid, piece_type);
    }
    
    fclose(f);
}

static void editor_section_navbar(uint8_t selected_section)
{
    SDL_Texture* selected_section_label;
    SDL_Rect section_rects [3];

    sui_rect_row(&game.screen_scenario_ui_rect, section_rects, 3, game.screen_scenario_ui_rect.w/3, 100, 0);

    section_rects[0].y = 0;
    section_rects[1].y = 0;
    section_rects[2].y = 0;

    if(selected_section != 0)
        sui_simple_button_with_texture_add(&section_rects[0], assetman_get_asset(MAIN_SECTION_TEXTURE_ID), editor_set_main_section, NULL, (SDL_Color){ COMMON_COLOR_VALS, 255 });
    else
        selected_section_label = assetman_get_asset(SELECTED_MAIN_SECTION_TEXTURE_ID);

    if(selected_section != 1)
        sui_simple_button_with_texture_add(&section_rects[1], assetman_get_asset(RULES_SECTION_TEXTURE_ID), editor_set_rules_section, NULL, (SDL_Color){ COMMON_COLOR_VALS, 255 });
    else
        selected_section_label = assetman_get_asset(SELECTED_RULES_SECTION_TEXTURE_ID);

    if(selected_section != 2)
        sui_simple_button_with_texture_add(&section_rects[2], assetman_get_asset(PIECES_SECTION_TEXTURE_ID), editor_set_pieces_section, NULL, (SDL_Color){ COMMON_COLOR_VALS, 255 });
    else
        selected_section_label = assetman_get_asset(SELECTED_PIECES_SECTION_TEXTURE_ID);

    SDL_Rect selected_section_label_rect;
    int selected_section_label_width;
    int selected_section_label_height;

    SDL_QueryTexture(selected_section_label, NULL, NULL, &selected_section_label_width, &selected_section_label_height);
    selected_section_label_rect = sui_rect_centered(&section_rects[selected_section], selected_section_label_width, selected_section_label_height);
    sui_solid_rect_element_add(&section_rects[selected_section], (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_texture_element_add_v1(&selected_section_label_rect, selected_section_label);
}

static void editor_set_main_section(void* event_data)
{
    sui_clear_elements();

    sui_solid_rect_element_add(&game.screen_scenario_ui_rect, (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
    editor_section_navbar(0);

    SDL_Texture* double_corner_side_value_texture = game.scenario_data.double_corner_on_right ? assetman_get_asset(DOUBLE_CORNER_RIGHT_TEXTURE_ID) : assetman_get_asset(DOUBLE_CORNER_LEFT_TEXTURE_ID);
    SDL_Texture* board_size_value_texture = NULL;

    switch (game.scenario_data.board_side_size)
    {
        case 8: board_size_value_texture = assetman_get_asset(BOARD_8X8_TEXTURE_ID); break;
        case 10: board_size_value_texture = assetman_get_asset(BOARD_10X10_TEXTURE_ID); break;
        case 12: board_size_value_texture = assetman_get_asset(BOARD_12X12_TEXTURE_ID); break;
        default: break;
    }

    SDL_Rect button_row1_rects [2];
    SDL_Rect save_button_rect;

    sui_rect_row(&game.screen_scenario_ui_rect, button_row1_rects, 2, 260, 100, 15);
    save_button_rect = sui_rect_centered(&game.screen_scenario_ui_rect, 260, 100);
    save_button_rect.y += 400;

    sui_simple_button_t* board_size_button = sui_simple_button_with_texture_add(&button_row1_rects[0], board_size_value_texture, switch_board_size, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_t* double_corner_side_button = sui_simple_button_with_texture_add(&button_row1_rects[1], double_corner_side_value_texture, switch_double_corner_side, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_with_texture_add(&save_button_rect, assetman_get_asset(SAVE_BUTTON_TEXTURE_ID), save_scenario_as_sch_file, &game.scenario_data, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });

    board_size_button->button_trigger.as_button_element.event_data = &board_size_button->text.as_texture_element;
    double_corner_side_button->button_trigger.as_button_element.event_data = &double_corner_side_button->text.as_texture_element;

    sui_texture_element_add_v2(button_row1_rects[0].x, button_row1_rects[0].y - 75, assetman_get_asset(BOARD_FIELD_TEXTURE_ID));
    sui_texture_element_add_v2(button_row1_rects[1].x, button_row1_rects[1].y - 75, assetman_get_asset(DOUBLE_CORNER_FIELD_TEXTURE_ID));
}

static void editor_set_rules_section(void* event_data)
{
    sui_clear_elements();

    sui_solid_rect_element_add(&game.screen_scenario_ui_rect, (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
    editor_section_navbar(1);

    SDL_Color starting_team_color = game.scenario_data.team == WHITE_TEAM ? (SDL_Color){ WHITE_PIECE_COLOR_VALS, 255 } : (SDL_Color){ BLACK_PIECE_COLOR_VALS, 255 };
    SDL_Texture* flying_kings_value_texture = game.scenario_data.flying_kings ? assetman_get_asset(TRUE_VALUE_TEXTURE_ID) : assetman_get_asset(FALSE_VALUE_TEXTURE_ID);
    SDL_Texture* law_quantity_value_texture = game.scenario_data.applies_law_of_quantity ? assetman_get_asset(TRUE_VALUE_TEXTURE_ID) : assetman_get_asset(FALSE_VALUE_TEXTURE_ID);
    SDL_Texture* law_quality_value_texture = game.scenario_data.applies_law_of_quality ? assetman_get_asset(TRUE_VALUE_TEXTURE_ID) : assetman_get_asset(FALSE_VALUE_TEXTURE_ID);

    SDL_Rect rule_buttons_rects [4];
    SDL_Rect starting_team_color_rect;

    sui_rect_column(&game.screen_scenario_ui_rect, rule_buttons_rects, 4, 260, 100, 15);

    for (size_t i = 0; i < 4; i++) rule_buttons_rects[i].x += 200;

    starting_team_color_rect = sui_rect_centered(&rule_buttons_rects[0], 200, 65);

    sui_solid_rect_element_add(&rule_buttons_rects[0], (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_solid_rect_t* starting_team_displayer = sui_solid_rect_element_add(&starting_team_color_rect, starting_team_color);
    sui_button_element_add(&rule_buttons_rects[0], switch_starting_team, starting_team_displayer);

    sui_simple_button_t* flying_kings_button = sui_simple_button_with_texture_add(&rule_buttons_rects[1], flying_kings_value_texture, switch_flying_kings, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_t* law_quantity_button = sui_simple_button_with_texture_add(&rule_buttons_rects[2], law_quantity_value_texture, switch_law_of_quantity, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_t* law_quality_button = sui_simple_button_with_texture_add(&rule_buttons_rects[3], law_quality_value_texture, switch_law_of_quality, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });

    flying_kings_button->button_trigger.as_button_element.event_data = &flying_kings_button->text.as_texture_element;
    law_quantity_button->button_trigger.as_button_element.event_data = &law_quantity_button->text.as_texture_element;
    law_quality_button->button_trigger.as_button_element.event_data = &law_quality_button->text.as_texture_element;

    sui_texture_element_add_v2(rule_buttons_rects[0].x - 400, rule_buttons_rects[0].y, assetman_get_asset(INITIAL_PLAYER_FIELD_TEXTURE_ID));
    sui_texture_element_add_v2(rule_buttons_rects[1].x - 400, rule_buttons_rects[1].y, assetman_get_asset(FLYING_KINGS_FIELD_TEXTURE_ID));
    sui_texture_element_add_v2(rule_buttons_rects[2].x - 400, rule_buttons_rects[2].y, assetman_get_asset(LAW_OF_QUANTITY_FIELD_TEXTURE_ID));
    sui_texture_element_add_v2(rule_buttons_rects[3].x - 400, rule_buttons_rects[3].y, assetman_get_asset(LAW_OF_QUALITY_FIELD_TEXTURE_ID));
}

static void editor_set_pieces_section(void* event_data)
{
    sui_clear_elements();

    sui_solid_rect_element_add(&game.screen_scenario_ui_rect, (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
    editor_section_navbar(2);

    SDL_Rect placeables_rects [5];

    sui_rect_column(&game.screen_scenario_ui_rect, placeables_rects, 5, 128, 128, 15);

    sui_simple_button_with_texture_add(&placeables_rects[0], assetman_get_asset(REMOVE_PIECE_IMAGE_ID), clear_placeable_piece, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_with_texture_add(&placeables_rects[1], assetman_get_asset(WHITE_PEON_TEXTURE_ID), set_placeable_white_peon, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_with_texture_add(&placeables_rects[2], assetman_get_asset(BLACK_PEON_TEXTURE_ID), set_placeable_black_peon, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_with_texture_add(&placeables_rects[3], assetman_get_asset(WHITE_QUEEN_TEXTURE_ID), set_placeable_white_queen, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_simple_button_with_texture_add(&placeables_rects[4], assetman_get_asset(BLACK_QUEEN_TEXTURE_ID), set_placeable_black_queen, NULL, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
}

static void save_single_cell_assignment(FILE* f, cell_id_t cid, cell_value_t piece_type)
{
    switch(piece_type)
    {
        case PIECE_WHITE_PEON:
            fprintf(f, "%" PRIu16 " : %s\n", cid+1, "WHITE_PEON");
            break;
        case PIECE_BLACK_PEON:
            fprintf(f, "%" PRIu16 " : %s\n", cid+1, "BLACK_PEON");
            break;
        case PIECE_WHITE_QUEEN:
            fprintf(f, "%" PRIu16 " : %s\n", cid+1, "WHITE_QUEEN");
            break;
        case PIECE_BLACK_QUEEN:
            fprintf(f, "%" PRIu16 " : %s\n", cid+1, "BLACK_QUEEN");
            break;
        default:
            break;
    }
}

static void save_multi_cell_assignment(FILE* f, cell_id_t start_cid, cell_id_t end_cid, cell_value_t piece_type)
{
    switch(piece_type)
    {
        case PIECE_WHITE_PEON:
            fprintf(f, "[%"PRIu16 ";%" PRIu16 "] : %s\n", start_cid+1, end_cid+1, "WHITE_PEON");
            break;
        case PIECE_BLACK_PEON:
            fprintf(f, "[%"PRIu16 ";%" PRIu16 "] : %s\n", start_cid+1, end_cid+1, "BLACK_PEON");
            break;
        case PIECE_WHITE_QUEEN:
            fprintf(f, "[%"PRIu16 ";%" PRIu16 "] : %s\n", start_cid+1, end_cid+1, "WHITE_QUEEN");
            break;
        case PIECE_BLACK_QUEEN:
            fprintf(f, "[%"PRIu16 ";%" PRIu16 "] : %s\n", start_cid+1, end_cid+1, "BLACK_QUEEN");
            break;
        default:
            break;
    }
}

static void clear_placeable_piece()
{
    game.piece_type_to_place = NO_PIECE;
}

static void set_placeable_white_peon()
{
    game.piece_type_to_place = PIECE_WHITE_PEON;
}

static void set_placeable_black_peon()
{
    game.piece_type_to_place = PIECE_BLACK_PEON;
}

static void set_placeable_white_queen()
{
    game.piece_type_to_place = PIECE_WHITE_QUEEN;
}

static void set_placeable_black_queen()
{
    game.piece_type_to_place = PIECE_BLACK_QUEEN;
}

static void switch_starting_team(void* sui_element_to_update)
{
    sui_solid_rect_t* sui_solid_rect_to_update = sui_element_to_update;
    
    if(game.scenario_data.team == WHITE_TEAM)
    {
        game.scenario_data.team = BLACK_TEAM;
        sui_solid_rect_to_update->color = (SDL_Color){ BLACK_PIECE_COLOR_VALS, 255 };
    }
    else
    {
        game.scenario_data.team = WHITE_TEAM;
        sui_solid_rect_to_update->color = (SDL_Color){ WHITE_PIECE_COLOR_VALS, 255 };
    }
}

static void switch_flying_kings(void* sui_element_to_update)
{
    sui_texture_t* sui_texture_to_update = sui_element_to_update;
    game.scenario_data.flying_kings = !game.scenario_data.flying_kings;
    sui_texture_to_update->texture = game.scenario_data.flying_kings ? assetman_get_asset(TRUE_VALUE_TEXTURE_ID) : assetman_get_asset(FALSE_VALUE_TEXTURE_ID);
    sui_texture_to_update->element.rect = sui_texture_rect_centered(&sui_texture_to_update->element.rect, sui_texture_to_update->texture);
}

static void switch_law_of_quantity(void* sui_element_to_update)
{
    sui_texture_t* sui_texture_to_update = sui_element_to_update;
    game.scenario_data.applies_law_of_quantity = !game.scenario_data.applies_law_of_quantity;
    sui_texture_to_update->texture = game.scenario_data.applies_law_of_quantity ? assetman_get_asset(TRUE_VALUE_TEXTURE_ID) : assetman_get_asset(FALSE_VALUE_TEXTURE_ID);
    sui_texture_to_update->element.rect = sui_texture_rect_centered(&sui_texture_to_update->element.rect, sui_texture_to_update->texture);
}

static void switch_law_of_quality(void* sui_element_to_update)
{
    sui_texture_t* sui_texture_to_update = sui_element_to_update;
    game.scenario_data.applies_law_of_quality = !game.scenario_data.applies_law_of_quality;
    sui_texture_to_update->texture = game.scenario_data.applies_law_of_quality ? assetman_get_asset(TRUE_VALUE_TEXTURE_ID) : assetman_get_asset(FALSE_VALUE_TEXTURE_ID);
    sui_texture_to_update->element.rect = sui_texture_rect_centered(&sui_texture_to_update->element.rect, sui_texture_to_update->texture);
}

static void switch_double_corner_side(void* sui_element_to_update)
{
    sui_texture_t* sui_texture_to_update = sui_element_to_update;

    game.scenario_data.double_corner_on_right = !game.scenario_data.double_corner_on_right;
    
    if(game.scenario_data.double_corner_on_right)
        sui_texture_to_update->texture = assetman_get_asset(DOUBLE_CORNER_RIGHT_TEXTURE_ID);
    else
        sui_texture_to_update->texture = assetman_get_asset(DOUBLE_CORNER_LEFT_TEXTURE_ID);

    sui_texture_to_update->element.rect = sui_texture_rect_centered(&sui_texture_to_update->element.rect, sui_texture_to_update->texture);
}

static void switch_board_size(void* sui_element_to_update)
{
    sui_texture_t* sui_texture_to_update = sui_element_to_update;

    switch(game.scenario_data.board_side_size)
    {
        case 8:
            game.scenario_data.board_side_size = 10;
            sui_texture_to_update->texture = assetman_get_asset(BOARD_10X10_TEXTURE_ID);
            break;
        case 10:
            game.scenario_data.board_side_size = 12;
            sui_texture_to_update->texture = assetman_get_asset(BOARD_12X12_TEXTURE_ID);
            break;
        case 12:
            game.scenario_data.board_side_size = 8;
            sui_texture_to_update->texture = assetman_get_asset(BOARD_8X8_TEXTURE_ID);
            break;
        default:
            break;
    }

    sui_texture_to_update->element.rect = sui_texture_rect_centered(&sui_texture_to_update->element.rect, sui_texture_to_update->texture);
}

static string_t scenario_mode_to_str(uint8_t mode)
{
    return mode == SCENARIO_MODE_1V1 ? "SCENARIO_1V1" : "SCENARIO_CHALLENGE"; 
}

static string_t boolean_to_str(bool boolean)
{
    return boolean ? "TRUE" : "FALSE";
}

static string_t team_to_str(team_t team)
{
    return team == WHITE_TEAM ? "WHITE" : "BLACK";
}

static string_t peon_movement_to_str(bool is_white_peon_forward_top_to_bottom)
{
    return is_white_peon_forward_top_to_bottom ? "WHITE_TOP_TO_BOTTOM" : "WHITE_BOTTOM_TO_TOP";
}