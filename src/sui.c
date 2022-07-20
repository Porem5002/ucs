#include <string.h>
#include <stdbool.h>

#include "include/sui.h"

static size_t elements_current_index = 0;
static sui_generic_element_t elements [MAX_GLOBAL_ELEMENT_COUNT];

static bool is_position_inside_rect(SDL_Rect* rect, int pixelX, int pixelY)
{
    return pixelX >= rect->x && pixelX <= rect->x + rect->w && pixelY >= rect->y && pixelY <= rect->y + rect->h;
}

SDL_Texture* sui_load_texture(char* file, SDL_Renderer* renderer, SDL_Surface** out_surface)
{
    SDL_Surface* surface = IMG_Load(file);

    if(surface == NULL) return NULL;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    if(out_surface == NULL)
    {
       SDL_FreeSurface(surface);
    }
    else
    {
        *out_surface = surface;
    }

    return texture;
}

SDL_Rect sui_get_texture_rect(SDL_Texture* texture, int x, int y)
{
    SDL_Rect texture_rect = { .x = x, .y = y };
    SDL_QueryTexture(texture, NULL, NULL, &texture_rect.w, &texture_rect.h);
    return texture_rect;
}

SDL_Texture* sui_texture_from_text(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color text_color)
{
    SDL_Texture* text_texture;
    SDL_Surface* text_surface;

    text_surface = TTF_RenderText_Blended(font, text, text_color);

    if(text_surface == NULL) return NULL;

    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_FreeSurface(text_surface);

    return text_texture;
}

SDL_Texture* sui_texture_from_utf8_text(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color text_color)
{
    SDL_Texture* text_texture;
    SDL_Surface* text_surface;

    text_surface = TTF_RenderUTF8_Blended(font, text, text_color);

    if(text_surface == NULL) return NULL;

    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_FreeSurface(text_surface);

    return text_texture;
}

sui_element_t* sui_element_add(sui_element_type_t type, SDL_Rect* rect)
{
    if(elements_current_index >= MAX_GLOBAL_ELEMENT_COUNT) return NULL;

    sui_element_t* element = &elements[elements_current_index].element;

    memset(element, 0, sizeof(sui_generic_element_t));
    element->type = type;
    if(rect != NULL) element->rect = *rect;

    elements_current_index++;

    return element;
}

sui_button_t* sui_button_element_add(SDL_Rect* rect, sui_click_event_t event, void* event_data)
{
    sui_button_t* button_component = (sui_button_t*)sui_element_add(SUI_BUTTON_COMPONENT_TYPE, rect);
    button_component->event = event;
    button_component->event_data = event_data;
    return button_component;
}

sui_texture_t* sui_texture_element_add_v1(SDL_Rect* rect, SDL_Texture* texture)
{
    sui_texture_t* texture_component = (sui_texture_t*)sui_element_add(SUI_TEXTURE_COMPONENT_TYPE, rect);
    texture_component->texture = texture;
    return texture_component;
}

sui_texture_t* sui_texture_element_add_v2(int x, int y, SDL_Texture* texture)
{
    SDL_Rect rect = sui_get_texture_rect(texture, x, y);
    sui_texture_t* texture_component = (sui_texture_t*)sui_element_add(SUI_TEXTURE_COMPONENT_TYPE, &rect);
    
    texture_component->texture = texture;
    return texture_component;
}

sui_solid_rect_t* sui_solid_rect_element_add(SDL_Rect* rect, SDL_Color color)
{
    sui_solid_rect_t* solid_rect_component = (sui_solid_rect_t*)sui_element_add(SUI_SOLID_RECT_COMPONENT_TYPE, rect);
    solid_rect_component->color = color;
    return solid_rect_component;
}

sui_simple_button_t* sui_simple_button_with_text_add(SDL_Renderer* renderer, SDL_Rect* rect, TTF_Font* font, char* text, sui_click_event_t event, void* event_data, SDL_Color text_color, SDL_Color background_color)
{
    sui_button_t* button_trigger = sui_button_element_add(rect, event, event_data);
    sui_solid_rect_t* button_background = sui_solid_rect_element_add(rect, background_color);
    sui_texture_t* button_text = (sui_texture_t*)sui_element_add(SUI_TEXTURE_COMPONENT_TYPE, NULL);

    button_text->texture = sui_texture_from_text(renderer, font, text, text_color);

    int button_text_width;
    int button_text_height;
    SDL_QueryTexture(button_text->texture, NULL, NULL, &button_text_width, &button_text_height);

    button_text->element.rect = sui_rect_centered(rect, button_text_width, button_text_height);

    return (sui_simple_button_t*)button_trigger;
}

sui_simple_button_t* sui_simple_button_with_texture_add(SDL_Rect* rect, SDL_Texture* texture, sui_click_event_t event, void* event_data, SDL_Color background_color)
{
    sui_button_t* button_trigger = sui_button_element_add(rect, event, event_data);
    sui_solid_rect_t* button_background = sui_solid_rect_element_add(rect, background_color);
    sui_texture_t* button_text = (sui_texture_t*)sui_element_add(SUI_TEXTURE_COMPONENT_TYPE, NULL);

    button_text->texture = texture;

    int button_text_width;
    int button_text_height;
    SDL_QueryTexture(button_text->texture, NULL, NULL, &button_text_width, &button_text_height);

    button_text->element.rect = sui_rect_centered(rect, button_text_width, button_text_height);

    return (sui_simple_button_t*)button_trigger;
}

int sui_rect_center_x(SDL_Rect* container, int width)
{
    return (container->w - width)/2 + container->x;
}

int sui_rect_center_y(SDL_Rect* container, int height)
{
    return (container->h - height)/2 + container->y;
}

SDL_Rect sui_rect_centered(SDL_Rect* container, int width, int height)
{
    SDL_Rect final_rect;

    final_rect.x = sui_rect_center_x(container, width);
    final_rect.y = sui_rect_center_y(container, height);
    final_rect.w = width;
    final_rect.h = height;

    return final_rect;
}

SDL_Rect sui_texture_rect_centered(SDL_Rect* container, SDL_Texture* texture)
{
    int width;
    int height;

    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    SDL_Rect final_rect = sui_rect_centered(container, width, height);

    return final_rect;
}

void sui_rect_row(SDL_Rect* container, SDL_Rect* rect_array, size_t count, size_t individual_width, size_t individual_height, size_t spacing)
{
    for (size_t i = 0; i < count; i++)
    {
        SDL_Rect* current_rect = &rect_array[i];

        current_rect->x = container->x + (container->w - (count * (individual_width + spacing) - spacing))/2 + i*(individual_width + spacing);
        current_rect->y = sui_rect_center_y(container, individual_height);
        current_rect->w = individual_width;
        current_rect->h = individual_height;
    }
}

void sui_rect_column(SDL_Rect* container, SDL_Rect* rect_array, size_t count, size_t individual_width, size_t individual_height, size_t spacing)
{
    for (size_t i = 0; i < count; i++)
    {
        SDL_Rect* current_rect = &rect_array[i];

        current_rect->x = sui_rect_center_x(container, individual_width);
        current_rect->y = (container->h - (count * (individual_height + spacing) - spacing))/2 + i*(individual_height + spacing);
        current_rect->w = individual_width;
        current_rect->h = individual_height;
    }
}

void sui_check_buttons(int mouseX, int mouseY)
{
    size_t i;

    for (i = 0; i < elements_current_index; i++)
    {
        sui_generic_element_t* element = &elements[i];

        if(element->element.type != SUI_BUTTON_COMPONENT_TYPE) continue;
        
        if(is_position_inside_rect(&element->element.rect, mouseX, mouseY))
        {
            element->as_button_element.event(element->as_button_element.event_data);
            return;
        }
    }
}

void sui_draw_elements(SDL_Renderer* renderer)
{
    size_t i;

    for (i = 0; i < elements_current_index; i++)
    {
        sui_generic_element_t* element = &elements[i];

        switch (element->element.type)
        {
            case SUI_BUTTON_COMPONENT_TYPE:
                break;                
            case SUI_TEXTURE_COMPONENT_TYPE:
                if(element->as_texture_element.texture == NULL) break;
                SDL_RenderCopy(renderer, element->as_texture_element.texture, NULL, &element->element.rect);
                break;
            case SUI_SOLID_RECT_COMPONENT_TYPE:
                SDL_SetRenderDrawColor(renderer, 
                                        element->as_solidrect_element.color.r, 
                                        element->as_solidrect_element.color.g, 
                                        element->as_solidrect_element.color.b, 
                                        element->as_solidrect_element.color.a);
                
                SDL_RenderFillRect(renderer, &element->element.rect);
                break;
            default:
                break;
        }
    }
}

void sui_clear_elements()
{
    elements_current_index = 0;
}