#include <stdio.h>
#include <time.h>

#include "include/logger.h"
#include "include/sui.h"
#include "include/game.h"
#include "include/interaction.h"
#include "include/scenario_loader.h"
#include "include/rendering.h"
#include "include/assetman_setup.h"
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_ttf.h"

#define GAME_WINDOW_FLAGS SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
#define GAME_LOOP_DELAY_MS 15

static void safe_exit();

int main(int argc, char** argv)
{
    atexit(safe_exit);

    game.window = NULL;
    game.renderer = NULL;

    FILE* log_file = freopen(LOG_FILE_PATH, "wb", stderr);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    game.window = SDL_CreateWindow("Checkers", 100, 100, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, GAME_WINDOW_FLAGS);
    
    if(game.window == NULL)
    {
        LOGGER_ERRORF("SDL could not create the Window!, %s", SDL_GetError());
        return 1;
    }

    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if(game.renderer == NULL)
    {
        LOGGER_ERRORF("SDL could not create the Renderer!, %s", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawBlendMode(game.renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(game.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SDL_Surface* icon_surface = IMG_Load(PATH_IMAGES "ucs_icon.png");
    
    SDL_SetWindowIcon(game.window, icon_surface);

    SDL_FreeSurface(icon_surface);

    assetman_init(checkers_free_asset_function);
    setup_initial_assets(game.renderer);

    game.screen_rect = (SDL_Rect){ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    game.is_playing = true;
    game.input.mouseX = 0;
    game.input.mouseY = 0;
    
    game_set_mode_menu(NULL);

    SDL_Event event;
    Uint32 previous_time = SDL_GetTicks();
    Uint32 current_time;

    while (game.is_playing)
    {
        current_time = SDL_GetTicks();
        game.delta_time = (float) (current_time - previous_time) / 1000;

        game.input.type = GAME_INPUT_NONE;

        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN:
                    game_catch_input(&event);
                    break;
                case SDL_QUIT: 
                    game_quit(NULL);
                    break;
                default: 
                    break;
            }
        }

        game.update();
            
        SDL_SetRenderDrawColor(game.renderer, BACKGROUND_COLOR_VALS, 255);
        SDL_RenderClear(game.renderer);
        render_frame();
        SDL_RenderPresent(game.renderer);

        SDL_Delay(GAME_LOOP_DELAY_MS);

        previous_time = current_time;
    }

    fclose(log_file);

    return 0;
}

static void safe_exit()
{
    assetman_finish(true);

    if(game.renderer != NULL) SDL_DestroyRenderer(game.renderer);
    if(game.window != NULL) SDL_DestroyWindow(game.window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}