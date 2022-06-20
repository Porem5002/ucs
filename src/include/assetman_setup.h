#ifndef ASSETMAN_SETUP_HEADER
#define ASSETMAN_SETUP_HEADER

#include <assert.h>

#include "SDL2/SDL.h"
#include "assetman.h"

enum 
{
    STATIC_ID_WHITE_PEON_TEXTURE,
    STATIC_ID_WHITE_QUEEN_TEXTURE,
    STATIC_ID_BLACK_PEON_TEXTURE,
    STATIC_ID_BLACK_QUEEN_TEXTURE,
    
    STATIC_ID_DEFAULT_SCENARIO_ICON_TEXTURE,
    STATIC_ID_DEFAULT_SCENARIO_NAME_TEXTURE,
    
    STATIC_ID_CORRECT_MOVE_IMAGE,
    STATIC_ID_INCORRECT_MOVE_IMAGE,
    STATIC_ID_REMOVE_PIECE_TEXTURE,

    STATIC_ID_MAIN_FONT,
    STATIC_ID_TITLE_FONT,
    STATIC_ID_BROWSER_FONT,

    STATIC_ID_COUNT
};

#define MAIN_FONT_ID                        (assetman_static_id(STATIC_ID_MAIN_FONT))
#define TITLE_FONT_ID                       (assetman_static_id(STATIC_ID_TITLE_FONT))
#define BROWSER_FONT_ID                     (assetman_static_id(STATIC_ID_BROWSER_FONT))
#define WHITE_PEON_TEXTURE_ID               (assetman_static_id(STATIC_ID_WHITE_PEON_TEXTURE))
#define WHITE_QUEEN_TEXTURE_ID              (assetman_static_id(STATIC_ID_WHITE_QUEEN_TEXTURE))
#define BLACK_PEON_TEXTURE_ID               (assetman_static_id(STATIC_ID_BLACK_PEON_TEXTURE))
#define BLACK_QUEEN_TEXTURE_ID              (assetman_static_id(STATIC_ID_BLACK_QUEEN_TEXTURE))
#define CORRECT_MOVE_IMAGE_ID               (assetman_static_id(STATIC_ID_CORRECT_MOVE_IMAGE))
#define INCORRECT_MOVE_IMAGE_ID             (assetman_static_id(STATIC_ID_INCORRECT_MOVE_IMAGE))
#define REMOVE_PIECE_IMAGE_ID               (assetman_static_id(STATIC_ID_REMOVE_PIECE_TEXTURE))
#define DEFAULT_SCENARIO_ICON_TEXTURE_ID    (assetman_static_id(STATIC_ID_DEFAULT_SCENARIO_ICON_TEXTURE))
#define DEFAULT_SCENARIO_NAME_TEXTURE_ID    (assetman_static_id(STATIC_ID_DEFAULT_SCENARIO_NAME_TEXTURE))

enum
{
    FONT_ASSET_TYPE,
    TEXTURE_ASSET_TYPE
};

void setup_initial_assets(SDL_Renderer* renderer);

void checkers_free_asset_function(asset_info_t* asset);

#endif