#ifndef ASSETMAN_SETUP_HEADER
#define ASSETMAN_SETUP_HEADER

#include <assert.h>

#include "SDL2/SDL.h"
#include "assetman.h"

enum
{
    FONT_ASSET_TYPE,
    TEXTURE_ASSET_TYPE
};

void setup_initial_assets(SDL_Renderer* renderer);

void checkers_free_asset_function(asset_info_t* asset);

#endif