#include <inttypes.h> 

#include "include/game.h"
#include "include/assetman_setup.h"
#include "include/scenario_loader.h"
#include "include/rendering.h"

#define GSELECTOR_STANDARD NULL
#define GSELECTOR_EDITOR ((void*)1)

#define SELECTOR_ITEMS_PER_PAGE 8
#define ID_ANCHOR 7
#define SCENARIO_ICON_SIDE 250
#define SCENARIO_TEXT_OFFSET 180
#define SCENARIO_SPACING 450

static void selector_refresh();
static void selector_section_navbar(bool is_standard_section);
static void selector_go_to_next_page(void* event_data);
static void selector_go_to_prev_page(void* event_data);

void game_set_mode_selector(void* event_data)
{
    LOGGER_LOGS("Started loading Scenario Browser!");

    game_free_dependencies();
    
    game.mode = MODE_SELECTOR;
    game.update = game_update_selector;
    game.selector.is_standard_section = event_data == GSELECTOR_STANDARD;
    event_data = game.selector.is_standard_section ? PATH_SCENARIOS_STANDARD : PATH_SCENARIOS_EDITOR;

    sui_clear_elements();

    SDL_Texture* default_scenario_icon = assetman_get_asset("$DefaultSchIcon");
    SDL_Texture* default_scenario_name = assetman_get_asset("$DefaultSchName");
    
    SDL_Texture* back_button_text_texture = sui_texture_from_text(game.renderer, assetman_get_asset("$MainFont"), UI_BACK_BUTTON_TEXT, (SDL_Color){ 0, 0, 0, 255 });
    SDL_Texture* next_page_button_texture = sui_load_texture(PATH_IMAGES "next_page.png", game.renderer, NULL);
    SDL_Texture* prev_page_button_texture = sui_load_texture(PATH_IMAGES "prev_page.png", game.renderer, NULL);

    SDL_Texture* selected_standard_section_texture = sui_texture_from_text(game.renderer, assetman_get_asset("$MainFont"), "STANDARD", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
    SDL_Texture* selected_editor_section_texture = sui_texture_from_text(game.renderer, assetman_get_asset("$MainFont"), "EDITOR", (SDL_Color){ MIDDLE_COLOR_VALS, 255 });

    SDL_Texture* standard_section_texture = sui_texture_from_text(game.renderer, assetman_get_asset("$MainFont"), "STANDARD", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    SDL_Texture* editor_section_texture = sui_texture_from_text(game.renderer, assetman_get_asset("$MainFont"), "EDITOR", (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });

    assetman_set_asset(true, "SelectorBackButton", TEXTURE_ASSET_TYPE, back_button_text_texture);
    assetman_set_asset(true, "SelectorNextPage", TEXTURE_ASSET_TYPE, next_page_button_texture);
    assetman_set_asset(true, "SelectorPrevPage", TEXTURE_ASSET_TYPE, prev_page_button_texture);

    assetman_set_asset(true, "SelectorStdSelected", TEXTURE_ASSET_TYPE, selected_standard_section_texture);
    assetman_set_asset(true, "SelectorEditorSelected", TEXTURE_ASSET_TYPE, selected_editor_section_texture);

    assetman_set_asset(true, "SelectorStd", TEXTURE_ASSET_TYPE, standard_section_texture);
    assetman_set_asset(true, "SelectorEditor", TEXTURE_ASSET_TYPE, editor_section_texture);

    uint8_t num_id = 0;
    char sch_icon_id [] = "SelectorSchIcon000";
    char sch_name_id [] = "SelectorSchName000"; 

    game.selector.file_paths = get_scenario_paths_from_dir(event_data);

    for (size_t i = 0; i < array_size(&game.selector.file_paths); i++)
    {
        string_t current_file_path = array_ele(&game.selector.file_paths, string_t, i);
        scenario_info_t given_scenario_info = get_scenario_info_from_file(current_file_path);

        if(given_scenario_info.icon_texture != NULL)
            assetman_set_asset(false, sch_icon_id, TEXTURE_ASSET_TYPE, given_scenario_info.icon_texture);
        else
            assetman_set_asset(false, sch_icon_id, UNHANDLED_ASSET, default_scenario_icon);

        if(given_scenario_info.name_texture != NULL)
            assetman_set_asset(false, sch_name_id, TEXTURE_ASSET_TYPE, given_scenario_info.name_texture);
        else
            assetman_set_asset(false, sch_name_id, UNHANDLED_ASSET, default_scenario_name);

        num_id++;
        sprintf(&sch_icon_id[15], "%03"PRIu8, num_id);
        sprintf(&sch_name_id[15], "%03"PRIu8, num_id);
    }

    pager_init(&game.selector.pager, array_size(&game.selector.file_paths), SELECTOR_ITEMS_PER_PAGE);
    pager_next_page(&game.selector.pager);

    selector_refresh();

    LOGGER_LOGS("Finished loading Scenario Browser!");
}

static void selector_refresh()
{
    SDL_Rect row_area_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/2 };
    SDL_Rect row_rects[SELECTOR_ITEMS_PER_PAGE];

    sui_clear_elements();

    selector_section_navbar(game.selector.is_standard_section);

    sui_rect_row(&row_area_rect, row_rects, SELECTOR_ITEMS_PER_PAGE/2, SCENARIO_ICON_SIDE, SCENARIO_ICON_SIDE, 20);    

    if(game.selector.pager.current_page_end - game.selector.pager.current_page_start > SELECTOR_ITEMS_PER_PAGE/2)
    {
        /* Ajusts second row to be below the first row. */
        row_area_rect.y += SCENARIO_SPACING;
        sui_rect_row(&row_area_rect, row_rects + SELECTOR_ITEMS_PER_PAGE/2, SELECTOR_ITEMS_PER_PAGE/2, SCENARIO_ICON_SIDE, SCENARIO_ICON_SIDE, 20);
    }

    char sch_icon_id [] = "SelectorSchIcon000";
    char sch_name_id [] = "SelectorSchName000"; 

    for (size_t i = game.selector.pager.current_page_start; i < game.selector.pager.current_page_end; i++)
    {
        size_t i_relative_to_page = game.selector.pager.current_page_end - i - 1;
        string_t path_of_scenario_to_load = array_ele(&game.selector.file_paths, string_t, i);
        
        sui_button_element_add(&row_rects[i_relative_to_page], game_set_mode_scenario, path_of_scenario_to_load);
        sui_texture_element_add_v1(&row_rects[i_relative_to_page], assetman_get_asset(sch_icon_id));

        SDL_Texture* name_texture = assetman_get_asset(sch_name_id);

        int text_width;
        int text_height;
        SDL_QueryTexture(name_texture, NULL, NULL, &text_width, &text_height);

        SDL_Rect text_rect = sui_rect_centered(&row_rects[i_relative_to_page], text_width, text_height);

        text_rect.y += SCENARIO_TEXT_OFFSET;

        sui_texture_element_add_v1(&text_rect, name_texture);

        sprintf(&sch_icon_id[15], "%03"PRIu8, (uint8_t)i);
        sprintf(&sch_name_id[15], "%03"PRIu8, (uint8_t)i);
    }

    if(!pager_is_first_page(&game.selector.pager))
    {
        SDL_Rect prev_page_button_rect = { 120, 0, 120, 160 };

        prev_page_button_rect.y = SCREEN_HEIGHT/2 - 160/2;

        sui_button_element_add(&prev_page_button_rect, selector_go_to_prev_page, NULL);
        sui_texture_element_add_v1(&prev_page_button_rect, assetman_get_asset("SelectorPrevPage"));
    }

    if(!pager_is_last_page(&game.selector.pager))
    {
        SDL_Rect next_page_button_rect = { SCREEN_WIDTH - 120 - 120, 0, 120, 160 };
        
        next_page_button_rect.y = SCREEN_HEIGHT/2 - 160/2;

        sui_button_element_add(&next_page_button_rect, selector_go_to_next_page, NULL);
        sui_texture_element_add_v1(&next_page_button_rect, assetman_get_asset("SelectorNextPage"));
    }

    SDL_Color background_color = { 135, 131, 209, 255 };
    SDL_Rect back_button_rect = sui_rect_centered(&game.screen_rect, 300, 100);

    back_button_rect.y = SCREEN_HEIGHT - 80 - 20;

    SDL_Texture* back_button_text_texture = assetman_get_asset("SelectorBackButton");
    
    sui_simple_button_with_texture_add(&back_button_rect, back_button_text_texture, game_set_mode_menu, NULL, background_color);
}

static void selector_section_navbar(bool is_standard_section)
{
    SDL_Texture* selected_section_label;
    SDL_Rect* selected_section_background_rect;
    SDL_Rect section_rects [2];

    sui_rect_row(&game.screen_rect, section_rects, 2, game.screen_rect.w/6, 100, 0);

    section_rects[0].y = 0;
    section_rects[1].y = 0;

    if(is_standard_section)
    {
        sui_simple_button_with_texture_add(&section_rects[1], assetman_get_asset("SelectorEditor"), game_set_mode_selector, GSELECTOR_EDITOR, (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
        selected_section_label = assetman_get_asset("SelectorStdSelected");
        selected_section_background_rect = &section_rects[0];
    }
    else
    {
        sui_simple_button_with_texture_add(&section_rects[0], assetman_get_asset("SelectorStd"), game_set_mode_selector, GSELECTOR_STANDARD, (SDL_Color){ MIDDLE_COLOR_VALS, 255 });
        selected_section_label = assetman_get_asset("SelectorEditorSelected");
        selected_section_background_rect = &section_rects[1];
    }
    
    SDL_Rect selected_section_label_rect;
    int selected_section_label_width;
    int selected_section_label_height;

    SDL_QueryTexture(selected_section_label, NULL, NULL, &selected_section_label_width, &selected_section_label_height);
    selected_section_label_rect = sui_rect_centered(selected_section_background_rect, selected_section_label_width, selected_section_label_height);
    sui_solid_rect_element_add(selected_section_background_rect, (SDL_Color){ ATTRACTIVE_COLOR_VALS, 255 });
    sui_texture_element_add_v1(&selected_section_label_rect, selected_section_label);
}

static void selector_go_to_next_page(void* event_data)
{
    pager_next_page(&game.selector.pager);
    selector_refresh();
}

static void selector_go_to_prev_page(void* event_data)
{
    pager_prev_page(&game.selector.pager);
    selector_refresh();
}