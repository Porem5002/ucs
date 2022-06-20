#include <stdio.h>
#include <string.h>
#include <io.h>

#include "include/assetman_setup.h"
#include "include/scenario_loader.h"
#include "include/strplus.h"
#include "include/lexer.h"
#include "include/logger.h"
#include "include/rendering.h"

static void scenario_loader_eat_property(scenario_loader_t* scenario_loader, uint8_t expected_property_token_type);
static void scenario_loader_eat_token(scenario_loader_t* scenario_loader, uint8_t type_to_eat);
static void scenario_loader_eat_symbol(scenario_loader_t* scenario_loader, char symbol);
static uint8_t id_to_scenario_type(string_t id);
static bool id_to_peon_movement_option(string_t id);
static bool id_to_boolean(string_t id);
static cell_value_t id_to_piece_type(string_t id);
static void scenario_loader_load_id(scenario_loader_t* scenario_loader);
static void scenario_loader_load_singlecell_piece_assignment(scenario_loader_t* scenario_loader);
static void scenario_loader_load_multicell_piece_assignment(scenario_loader_t* scenario_loader);
static void scenario_loader_load_statement(scenario_loader_t* scenario_loader);
static void scenario_loader_load_challenge_moves(scenario_loader_t* scenario_loader);
static void parse_single_challenge_move(scenario_loader_t* scenario_loader, dynarray(move_info_t)* challenge_moves);

void load_scenario_from_token_array(scenario_t* destination, array(token_t) token_array)
{
    scenario_loader_t scenario_loader;
    scenario_loader.destination = destination;
    scenario_loader.token_array = token_array;
    scenario_loader.iterator = 0;
    scenario_loader.prev_token = NULL;
    scenario_loader.current_token = NULL;

    scenario_set_default(destination);

    while(scenario_loader.iterator < array_size(&scenario_loader.token_array))
    {
        scenario_loader.prev_token = scenario_loader.current_token;
        scenario_loader.current_token = rrr_array_ele(&scenario_loader.token_array, sizeof(token_t), scenario_loader.iterator);
        scenario_loader_load_statement(&scenario_loader);
    }
}

void load_scenario_from_file(scenario_t* destination, string_t scenario_file_name)
{
    FILE* f;
    size_t scenario_src_size;
    string_t scenario_src;
    
    string_t scenario_file_path = string_heap_concat(PATH_FOLDER_SCENARIOS, scenario_file_name);
    
    f = fopen(scenario_file_path, "rb");

    free(scenario_file_path);

    fseek(f, 0, SEEK_END);
    scenario_src_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    scenario_src = malloc(scenario_src_size+1);
    fread(scenario_src, 1, scenario_src_size, f);
    scenario_src[scenario_src_size] = '\0';

    fclose(f);

    lexer_t lexer;
    array(token_t) tokens;
    
    lexer_init(&lexer, scenario_src);
    
    tokens = lexer_collect_tokens(&lexer);

    load_scenario_from_token_array(destination, tokens);

    free(scenario_src);

    for (size_t i = 0; i < array_size(&tokens); i++)
    {
        token_t* current_token = rrr_array_ele(&tokens, sizeof(token_t), i);
        token_free(current_token);
    }

    array_free(&tokens);
}

array(string_t) get_scenario_file_names_list()
{
    dynarray(string_t) file_names_list = dynarray_new(string_t, 0);
    struct _finddata_t current_file_data;
    intptr_t hFile;

    hFile = _findfirst(PATH_FOLDER_SCENARIOS "*" SCENARIO_FILE_EXTENSION, &current_file_data);

    if(hFile != -1L)
    {
        string_t current_file_name = string_heap_copy(current_file_data.name);
        dynarray_add(&file_names_list, string_t, &current_file_name);

        while(_findnext(hFile, &current_file_data) == 0)
        {
            current_file_name = string_heap_copy(current_file_data.name);
            dynarray_add(&file_names_list, string_t, &current_file_name);
        }
            
        _findclose(hFile);
    }

    return dynarray_to_array(&file_names_list, string_t);
}

scenario_info_t get_scenario_info_from_file(string_t scenario_file_name)
{
    FILE* f;
    size_t scenario_src_size;
    string_t scenario_src;
    
    string_t scenario_file_path = string_heap_concat(PATH_FOLDER_SCENARIOS, scenario_file_name);
    
    f = fopen(scenario_file_path, "rb");

    free(scenario_file_path);

    fseek(f, 0, SEEK_END);
    scenario_src_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    scenario_src = malloc(scenario_src_size+1);
    fread(scenario_src, 1, scenario_src_size, f);
    scenario_src[scenario_src_size] = '\0';

    fclose(f);

    scenario_info_t scenario_info = { NULL, NULL};

    lexer_t lexer;
    token_t icon_property_token = { .type = TOKEN_ID, .identifier = "ICON" };
    token_t name_property_token = { .type = TOKEN_ID, .identifier = "NAME" };

    lexer_init(&lexer, scenario_src);

    bool found_icon = lexer_go_to_next_token_equal_to(&lexer, &icon_property_token);

    if(found_icon)
    {
        token_t assigment_symbol_token = lexer_collect_next_token(&lexer); 
        token_t icon_file_path_token = lexer_collect_next_token(&lexer);

        SDL_Surface* icon_surface = IMG_Load(icon_file_path_token.string_value);
        scenario_info.icon_texture = SDL_CreateTextureFromSurface(game.renderer, icon_surface);

        SDL_FreeSurface(icon_surface);
        token_free(&assigment_symbol_token);
        token_free(&icon_file_path_token);
    }

    lexer_restart(&lexer);

    bool found_name = lexer_go_to_next_token_equal_to(&lexer, &name_property_token);

    if(found_name)
    {
        TTF_Font* browser_font = assetman_get_asset(BROWSER_FONT_ID);
        token_t assigment_symbol_token = lexer_collect_next_token(&lexer); 
        token_t name_file_path_token = lexer_collect_next_token(&lexer);

        scenario_info.name_texture = sui_texture_from_text(game.renderer, browser_font, name_file_path_token.string_value, (SDL_Color){ 135, 131, 209, 255});

        token_free(&assigment_symbol_token);
        token_free(&name_file_path_token);
    }

    free(scenario_src);
    return scenario_info;
}

static void scenario_loader_eat_property(scenario_loader_t* scenario_loader, uint8_t expected_property_token_type)
{
    scenario_loader_eat_token(scenario_loader, TOKEN_ID);
    scenario_loader_eat_symbol(scenario_loader, ':');
    scenario_loader_eat_token(scenario_loader, expected_property_token_type);
}

static void scenario_loader_eat_token(scenario_loader_t* scenario_loader, uint8_t type_to_eat)
{
    if(scenario_loader->current_token->type != type_to_eat)
    {
        LOGGER_ERRORF("Unexpected token found while loading scenario file (expected %d, received %d)", type_to_eat, scenario_loader->current_token->type);
        exit(EXIT_FAILURE);
    }

    scenario_loader->iterator++;
    scenario_loader->prev_token = scenario_loader->current_token;

    if(scenario_loader->iterator >= array_size(&scenario_loader->token_array))
    {
        scenario_loader->current_token = NULL;
        return;
    }

    scenario_loader->current_token = rrr_array_ele(&scenario_loader->token_array, sizeof(token_t), scenario_loader->iterator);
}

static void scenario_loader_eat_symbol(scenario_loader_t* scenario_loader, char symbol)
{
    scenario_loader_eat_token(scenario_loader, TOKEN_SYMBOL);

    if(scenario_loader->prev_token->symbol != symbol)
    {
        LOGGER_ERRORF("Unexpected symbol found while loading scenario file (expected symbol %c, received symbol %c)", symbol, scenario_loader->current_token->symbol);
        exit(EXIT_FAILURE);
    }
}

static uint8_t id_to_scenario_type(string_t id)
{
    if(string_equals(id, "SCENARIO_1V1"))       return SCENARIO_MODE_1V1;
    if(string_equals(id, "SCENARIO_CHALLENGE")) return SCENARIO_MODE_CHALLENGE;

    LOGGER_ERRORF("Identifier \'%s\' does not represent a possible peon movement option!", id);
    exit(EXIT_FAILURE);
}

static bool id_to_peon_movement_option(string_t id)
{
    if(string_equals(id, "WHITE_BOTTOM_TO_TOP")|| string_equals(id, "BLACK_TOP_TO_BOTTOM")) return false;
    if(string_equals(id, "WHITE_TOP_TO_BOTTOM") || string_equals(id, "BLACK_BOTTOM_TO_TOP")) return true;

    LOGGER_ERRORF("Identifier \'%s\' does not represent a possible peon movement option!", id);
    exit(EXIT_FAILURE);
}

static bool id_to_boolean(string_t id)
{
    if(string_equals(id, "TRUE"))   return true;
    if(string_equals(id, "FALSE"))  return false;

    LOGGER_ERRORF("Identifier \'%s\' does not represent a boolean value!", id);
    exit(EXIT_FAILURE);
}

static team_t id_to_team(string_t id)
{
    if(string_equals(id, "WHITE")) return WHITE_TEAM;
    if(string_equals(id, "BLACK")) return BLACK_TEAM;

    LOGGER_ERRORF("Identifier \'%s\' does not represent a valid team value!", id);
    exit(EXIT_FAILURE);
}

static cell_value_t id_to_piece_type(string_t id)
{
    if(string_equals(id, "WHITE_PEON"))     return PIECE_WHITE_PEON;
    if(string_equals(id, "WHITE_QUEEN"))    return PIECE_WHITE_QUEEN;
    if(string_equals(id, "BLACK_PEON"))     return PIECE_BLACK_PEON;
    if(string_equals(id, "BLACK_QUEEN"))    return PIECE_BLACK_QUEEN;
    if(string_equals(id, "EMPTY"))          return NO_PIECE;

    LOGGER_ERRORF("Identifier \'%s\' does not represent a valid piece type!", id);
    exit(EXIT_FAILURE);
}

static void scenario_loader_load_id(scenario_loader_t* scenario_loader)
{
    if(string_equals(scenario_loader->current_token->identifier, "TEAM"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->team = id_to_team(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "BOARD"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_INTEGER);
        scenario_loader->destination->board_side_size = scenario_loader->prev_token->integer_value;
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "DOUBLE_CORNER_SIDE"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->double_corner_on_right = id_to_boolean(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "APPLY_LAW_OF_QUANTITY"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->applies_law_of_quantity = id_to_boolean(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "APPLY_LAW_OF_QUALITY"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->applies_law_of_quality = id_to_boolean(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "FLYING_KINGS"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->flying_kings = id_to_boolean(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "PEONS_CAPTURE_BACKWARDS"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->peons_capture_backwards = id_to_boolean(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "PEONS_MOVEMENT"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->is_white_peon_forward_top_to_bottom = id_to_peon_movement_option(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "SCENARIO_TYPE"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_ID);
        scenario_loader->destination->scenario_mode = id_to_scenario_type(scenario_loader->prev_token->identifier);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "CHALLENGE"))
    {
        scenario_loader_load_challenge_moves(scenario_loader);
        return;
    }

    if(string_equals(scenario_loader->current_token->identifier, "NAME") || string_equals(scenario_loader->current_token->identifier, "ICON"))
    {
        scenario_loader_eat_property(scenario_loader, TOKEN_STRING);
        return;
    }

    LOGGER_ERRORF("Identifier \'%s\' does not represent a valid property!", scenario_loader->current_token->identifier);
    exit(EXIT_FAILURE);
}

static void scenario_loader_load_singlecell_piece_assignment(scenario_loader_t* scenario_loader)
{
    scenario_loader_eat_token(scenario_loader, TOKEN_INTEGER);

    cell_id_t cell_id = scenario_loader->prev_token->integer_value - 1;

    scenario_loader_eat_symbol(scenario_loader, ':');
    scenario_loader_eat_token(scenario_loader, TOKEN_ID);

    cell_value_t piece_type = id_to_piece_type(scenario_loader->prev_token->identifier);
    
    scenario_loader->destination->board.playable_cells[cell_id] = piece_type;
}

static void scenario_loader_load_multicell_piece_assignment(scenario_loader_t* scenario_loader)
{
    scenario_loader_eat_symbol(scenario_loader, '[');
    scenario_loader_eat_token(scenario_loader, TOKEN_INTEGER);

    cell_id_t start_cell_id = scenario_loader->prev_token->integer_value - 1;

    scenario_loader_eat_symbol(scenario_loader, ';');
    scenario_loader_eat_token(scenario_loader, TOKEN_INTEGER);

    cell_id_t end_cell_id = scenario_loader->prev_token->integer_value - 1;

    scenario_loader_eat_symbol(scenario_loader, ']');
    scenario_loader_eat_symbol(scenario_loader, ':');

    if(start_cell_id > end_cell_id)
    {
        cell_id_t temp = start_cell_id;
        start_cell_id = end_cell_id;
        end_cell_id = temp;
    }

    scenario_loader_eat_token(scenario_loader, TOKEN_ID);

    cell_value_t piece_type = id_to_piece_type(scenario_loader->prev_token->identifier);

    for(cell_id_t cell_id = start_cell_id; cell_id < end_cell_id + 1; cell_id++)
    {
        scenario_loader->destination->board.playable_cells[cell_id] = piece_type;
    }
}

static void scenario_loader_load_statement(scenario_loader_t* scenario_loader)
{
    switch (scenario_loader->current_token->type)
    {
        case TOKEN_ID:
            scenario_loader_load_id(scenario_loader);
            break;
        case TOKEN_INTEGER:
            scenario_loader_load_singlecell_piece_assignment(scenario_loader);
            break;
        case TOKEN_SYMBOL:
            if(scenario_loader->current_token->symbol == '[')
                scenario_loader_load_multicell_piece_assignment(scenario_loader);
            break;
        default:
            break;
    }
}

static void scenario_loader_load_challenge_moves(scenario_loader_t* scenario_loader)
{
    if(scenario_loader->destination->scenario_mode != SCENARIO_MODE_CHALLENGE)
    {
        LOGGER_LOGS("Property CHALLENGE was ignored since SCENARIO_TYPE is not set to SCENARIO_CHALLENGE.");

        scenario_loader_eat_token(scenario_loader, TOKEN_ID);
        scenario_loader_eat_symbol(scenario_loader, ':');
        scenario_loader_eat_symbol(scenario_loader, '{');

        while(!token_is_symbol(scenario_loader->current_token, '}'))
            scenario_loader_eat_token(scenario_loader, scenario_loader->current_token->type);

        scenario_loader_eat_symbol(scenario_loader, '}');
        
        return;
    }

    scenario_loader_eat_token(scenario_loader, TOKEN_ID);
    scenario_loader_eat_symbol(scenario_loader, ':');
    scenario_loader_eat_symbol(scenario_loader, '{');

    dynarray(move_info_t) challenge_moves = dynarray_new(move_info_t, 0);

    while(!token_is_symbol(scenario_loader->current_token, '}')) 
        parse_single_challenge_move(scenario_loader, &challenge_moves);

    scenario_loader_eat_symbol(scenario_loader, '}');

    scenario_loader->destination->challenge_moves = dynarray_to_array(&challenge_moves, move_info_t);
}

static void parse_single_challenge_move(scenario_loader_t* scenario_loader, dynarray(move_info_t)* challenge_moves)
{
    move_info_t current_move;

    current_move.is_capture_move = false;
    scenario_loader_eat_symbol(scenario_loader, '(');

    scenario_loader_eat_token(scenario_loader, TOKEN_INTEGER);
    current_move.source_cell = scenario_loader->prev_token->integer_value - 1;
    scenario_loader_eat_symbol(scenario_loader, ',');

    scenario_loader_eat_token(scenario_loader, TOKEN_INTEGER);
    current_move.destination_cell = scenario_loader->prev_token->integer_value - 1;

    if(token_is_symbol(scenario_loader->current_token, ','))
    {
        scenario_loader_eat_symbol(scenario_loader, ',');
        scenario_loader_eat_token(scenario_loader, TOKEN_INTEGER);
        current_move.is_capture_move = true;
        current_move.capture_cell = scenario_loader->prev_token->integer_value - 1;
    }
    
    scenario_loader_eat_symbol(scenario_loader, ')');

    dynarray_add(challenge_moves, move_info_t, &current_move);
}