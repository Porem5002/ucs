#include "include/game.h"
#include "include/assetman_setup.h"
#include "include/file_browser.h"
#include "include/scenario_loader.h"
#include "include/rendering.h"

#define FILE_BROWSER_SCENARIOS_PER_PAGE 6
#define ID_ANCHOR 1
#define SCENARIO_ICON_SIDE 250
#define SCENARIO_TEXT_OFFSET 180
#define SCENARIO_SPACING 450

static void file_browser_update();
static void file_browser_go_to_next_page(void* event_data);
static void file_browser_go_to_prev_page(void* event_data);

void game_set_mode_file_browser(void* event_data)
{
    LOGGER_LOGS("Started loading Scenario Browser!");

    game_free_dependencies();
    game.mode = MODE_FILE_BROWSER;
    sui_clear_elements();

    TTF_Font* main_font = assetman_get_asset(MAIN_FONT_ID);
    SDL_Texture* default_scenario_icon = assetman_get_asset(DEFAULT_SCENARIO_ICON_TEXTURE_ID);
    SDL_Texture* default_scenario_name = assetman_get_asset(DEFAULT_SCENARIO_NAME_TEXTURE_ID);
    
    SDL_Texture* back_button_text_texture = sui_texture_from_text(game.renderer, main_font, UI_BACK_BUTTON_TEXT, (SDL_Color){ 0, 0, 0, 255 });

    assetman_set_asset(assetman_dynamic_id(0), TEXTURE_ASSET_TYPE, back_button_text_texture);

    size_t id = ID_ANCHOR;

    game.file_browser_file_names_list = get_scenario_file_names_list();

    for (size_t i = 0; i < array_size(&game.file_browser_file_names_list); i++)
    {
        string_t scenario_file_name = array_ele(&game.file_browser_file_names_list, string_t, i);
        scenario_info_t given_scenario_info = get_scenario_info_from_file(scenario_file_name);
        
        if(given_scenario_info.icon_texture != NULL)
            assetman_set_asset(assetman_dynamic_id(id), TEXTURE_ASSET_TYPE, given_scenario_info.icon_texture);
        else
            assetman_set_asset(assetman_dynamic_id(id), UNHANDLED_ASSET, default_scenario_icon);
        
        id++;

        if(given_scenario_info.name_texture != NULL)
            assetman_set_asset(assetman_dynamic_id(id), TEXTURE_ASSET_TYPE, given_scenario_info.name_texture);
        else
            assetman_set_asset(assetman_dynamic_id(id), UNHANDLED_ASSET, default_scenario_name);

        id++;
    }

    pager_init(&game.file_browser_pager, array_size(&game.file_browser_file_names_list), FILE_BROWSER_SCENARIOS_PER_PAGE);
    pager_next_page(&game.file_browser_pager);

    file_browser_update();

    LOGGER_LOGS("Finished loading Scenario Browser!");
}

static void file_browser_update()
{
    SDL_Rect row_area_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/2 };
    SDL_Rect row_rects[FILE_BROWSER_SCENARIOS_PER_PAGE];

    sui_clear_elements();

    sui_rect_row(&row_area_rect, row_rects, FILE_BROWSER_SCENARIOS_PER_PAGE/2, SCENARIO_ICON_SIDE, SCENARIO_ICON_SIDE, 20);    

    if(game.file_browser_pager.current_page_end - game.file_browser_pager.current_page_start > FILE_BROWSER_SCENARIOS_PER_PAGE/2)
    {
        /* Ajusts second row to be below the first row. */
        row_area_rect.y += SCENARIO_SPACING;
        sui_rect_row(&row_area_rect, row_rects + FILE_BROWSER_SCENARIOS_PER_PAGE/2, FILE_BROWSER_SCENARIOS_PER_PAGE/2, SCENARIO_ICON_SIDE, SCENARIO_ICON_SIDE, 20);
    }

    for (size_t i = game.file_browser_pager.current_page_start; i < game.file_browser_pager.current_page_end; i++)
    {
        size_t icon_asset_id = i*2 + ID_ANCHOR; 
        size_t name_asset_id = i*2 + ID_ANCHOR + 1;

        size_t i_relative_to_page = game.file_browser_pager.current_page_end - i - 1;
        string_t name_of_scenario_file_to_load = array_ele(&game.file_browser_file_names_list, string_t, i);
        
        sui_button_element_add(&row_rects[i_relative_to_page], game_set_mode_scenario, name_of_scenario_file_to_load);
        sui_texture_element_add(&row_rects[i_relative_to_page], assetman_get_asset(assetman_dynamic_id(icon_asset_id)));

        SDL_Texture* name_texture = assetman_get_asset(assetman_dynamic_id(name_asset_id));

        int text_width;
        int text_height;
        SDL_QueryTexture(name_texture, NULL, NULL, &text_width, &text_height);

        SDL_Rect text_rect = sui_rect_centered(&row_rects[i_relative_to_page], text_width, text_height);

        text_rect.y += SCENARIO_TEXT_OFFSET;

        sui_texture_element_add(&text_rect, name_texture);
    }

    if(!pager_is_first_page(&game.file_browser_pager))
    {
        SDL_Rect prev_page_button_rect = { 0, 0, 40, 40 };

        prev_page_button_rect.y = SCREEN_HEIGHT/2 - 20;

        sui_button_element_add(&prev_page_button_rect, file_browser_go_to_prev_page, NULL);
        sui_solid_rect_element_add(&prev_page_button_rect, (SDL_Color){ 255, 255, 255, 255 });
    }

    if(!pager_is_last_page(&game.file_browser_pager))
    {
        SDL_Rect next_page_button_rect = { SCREEN_WIDTH - 40, 0, 40, 40 };
        
        next_page_button_rect.y = SCREEN_HEIGHT/2 - 20;

        sui_button_element_add(&next_page_button_rect, file_browser_go_to_next_page, NULL);
        sui_solid_rect_element_add(&next_page_button_rect, (SDL_Color){ 255, 255, 255, 255 });
    }

    SDL_Color background_color = { 135, 131, 209, 255 };
    SDL_Rect back_button_rect = sui_rect_centered(&game.screen_rect, 300, 100);

    back_button_rect.y = SCREEN_HEIGHT - 80 - 20;

    SDL_Texture* back_button_text_texture = assetman_get_asset(assetman_dynamic_id(0));
    
    sui_simple_button_with_texture_add(&back_button_rect, back_button_text_texture, game_set_mode_menu, NULL, background_color);
}

static void file_browser_go_to_next_page(void* event_data)
{
    pager_next_page(&game.file_browser_pager);
    file_browser_update();
}

static void file_browser_go_to_prev_page(void* event_data)
{
    pager_prev_page(&game.file_browser_pager);
    file_browser_update();
}