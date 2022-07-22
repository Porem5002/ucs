#include "include/game.h"
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_image.h"
#include "include/SDL2/SDL_ttf.h"
#include "include/scenario_loader.h"
#include "include/assetman_setup.h"
#include "include/rendering.h"

void setup_initial_assets(SDL_Renderer* renderer)
{
    TTF_Font* main_font = TTF_OpenFont(PATH_FONTS "main_text.ttf", 45);
    TTF_Font* title_font = TTF_OpenFont(PATH_FONTS "main_text.ttf", 150);
    TTF_Font* browser_font = TTF_OpenFont(PATH_FONTS "main_text.ttf", 35);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    SDL_Texture* white_peon_texture = sui_load_texture(PATH_IMAGES "white_peon.png", renderer, NULL);
    SDL_Texture* white_queen_texture = sui_load_texture(PATH_IMAGES "white_queen.png", renderer, NULL);
    SDL_Texture* black_peon_texture = sui_load_texture(PATH_IMAGES "black_peon.png", renderer, NULL);
    SDL_Texture* black_queen_texture = sui_load_texture(PATH_IMAGES "black_queen.png", renderer, NULL);
    SDL_Texture* correct_move_texture = sui_load_texture(PATH_IMAGES "correct_move.png", renderer, NULL);
    SDL_Texture* incorrect_move_texture = sui_load_texture(PATH_IMAGES "incorrect_move.png", renderer, NULL);
    SDL_Texture* remove_piece_texture = sui_load_texture(PATH_IMAGES "remove_piece.png", renderer, NULL);
    SDL_Texture* default_scenario_icon_texture = sui_load_texture(PATH_IMAGES "default_scenario_icon.png", renderer, NULL);
    SDL_Texture* default_scenario_name_texture = sui_texture_from_text(renderer, browser_font, "???", (SDL_Color){ ATTRACTIVE_COLOR_VALS , 255});

    assetman_set_asset(true, "$MainFont", FONT_ASSET_TYPE, main_font);
    assetman_set_asset(true, "$TitleFont", FONT_ASSET_TYPE, title_font);
    assetman_set_asset(true, "$SelectorFont", FONT_ASSET_TYPE, browser_font);
    assetman_set_asset(true, "$WhitePeon", TEXTURE_ASSET_TYPE, white_peon_texture);
    assetman_set_asset(true, "$WhiteQueen", TEXTURE_ASSET_TYPE, white_queen_texture);
    assetman_set_asset(true, "$BlackPeon", TEXTURE_ASSET_TYPE, black_peon_texture);
    assetman_set_asset(true, "$BlackQueen", TEXTURE_ASSET_TYPE, black_queen_texture);
    assetman_set_asset(true, "$ChallengeCorrect", TEXTURE_ASSET_TYPE, correct_move_texture);
    assetman_set_asset(true, "$ChallengeWrong", TEXTURE_ASSET_TYPE, incorrect_move_texture);
    assetman_set_asset(true, "$EditorRemovePiece", TEXTURE_ASSET_TYPE, remove_piece_texture);
    assetman_set_asset(true, "$DefaultSchIcon", TEXTURE_ASSET_TYPE, default_scenario_icon_texture);
    assetman_set_asset(true, "$DefaultSchName", TEXTURE_ASSET_TYPE, default_scenario_name_texture);
}

void checkers_free_asset_function(asset_info_t* asset)
{
    switch (asset->asset_type)
    {
        case FONT_ASSET_TYPE:
            TTF_CloseFont((TTF_Font*)asset->asset_data);
            asset->asset_data = NULL;
            break;
        case TEXTURE_ASSET_TYPE:
            SDL_DestroyTexture((SDL_Texture*)asset->asset_data);
            asset->asset_data = NULL;
            break;
    }
}