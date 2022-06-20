#ifndef SCENARIO_LOADER_HEADER
#define SCENARIO_LOADER_HEADER

#include <stdint.h>

#include "game.h"
#include "lexer.h"
#include "strplus.h"

#define DTS_USE_ARRAY
#define DTS_USE_DYNARRAY
#define DTS_USE_TREE

#include "dtstructs.h"

#define PATH_FOLDER_IMAGES      "sprites/"
#define PATH_FOLDER_SCENARIOS   "scenarios/"
#define SCENARIO_FILE_EXTENSION   ".sch"

typedef struct 
{
    token_t* current_token;
    token_t* prev_token;
    scenario_t* destination;
    array(token_t) token_array;
    size_t iterator;
} scenario_loader_t;

typedef struct
{
    SDL_Texture* icon_texture;
    SDL_Texture* name_texture;
} scenario_info_t;

void load_scenario_from_token_array(scenario_t* destination, array(token_t) scenario_file_token_array);

void load_scenario_from_file(scenario_t* destination, string_t scenario_file_name);

array(string_t) get_scenario_file_names_list();

scenario_info_t get_scenario_info_from_file(string_t scenario_file_name);

#endif