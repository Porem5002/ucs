#if !defined(SUI_HEADER)
#define SUI_HEADER 

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

#ifndef MAX_GLOBAL_ELEMENT_COUNT
#define MAX_GLOBAL_ELEMENT_COUNT 100
#endif

enum
{
    SUI_BUTTON_COMPONENT_TYPE,
    SUI_TEXTURE_COMPONENT_TYPE,
    SUI_SOLID_RECT_COMPONENT_TYPE
};

typedef void (*sui_click_event_t) (void*);

typedef char sui_element_type_t;

typedef struct
{   
    sui_element_type_t type;
    SDL_Rect rect;
} sui_element_t;

typedef struct
{
    sui_element_t element;
    sui_click_event_t event;
    void* event_data;
} sui_button_t;

typedef struct
{
    sui_element_t element;
    SDL_Texture* texture;
} sui_texture_t;

typedef struct
{
    sui_element_t element;
    SDL_Color color;
} sui_solid_rect_t;

typedef union
{
    sui_element_t element;
    sui_button_t as_button_element;
    sui_texture_t as_texture_element;
    sui_solid_rect_t as_solidrect_element;
} sui_generic_element_t;

typedef struct
{
    sui_generic_element_t button_trigger;
    sui_generic_element_t background;
    sui_generic_element_t text;
} sui_simple_button_t;

SDL_Texture* sui_load_texture(char* file, SDL_Renderer* renderer, SDL_Surface** out_surface);

SDL_Rect sui_get_texture_rect(SDL_Texture* texture, int x, int y);

SDL_Texture* sui_texture_from_text(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color text_color);

SDL_Texture* sui_texture_from_utf8_text(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color text_color);

sui_element_t* sui_element_add(sui_element_type_t type, SDL_Rect* rect); 

sui_button_t* sui_button_element_add(SDL_Rect* rect, sui_click_event_t event, void* event_data);

sui_texture_t* sui_texture_element_add(SDL_Rect* rect, SDL_Texture* texture);

sui_solid_rect_t* sui_solid_rect_element_add(SDL_Rect* rect, SDL_Color color);

sui_simple_button_t* sui_simple_button_with_text_add(SDL_Renderer* renderer, SDL_Rect* rect, TTF_Font* font, char* text, sui_click_event_t event, void* event_data, SDL_Color text_color, SDL_Color background_color);

sui_simple_button_t* sui_simple_button_with_texture_add(SDL_Rect* rect, SDL_Texture* texture, sui_click_event_t event, void* event_data, SDL_Color background_color);

int sui_rect_center_x(SDL_Rect* container, int width);

int sui_rect_center_y(SDL_Rect* container, int height);

SDL_Rect sui_rect_centered(SDL_Rect* container, int width, int height);

SDL_Rect sui_texture_rect_centered(SDL_Rect* container, SDL_Texture* texture);

void sui_rect_row(SDL_Rect* container, SDL_Rect* rect_array, size_t count, size_t individual_width, size_t individual_height, size_t spacing);

void sui_rect_column(SDL_Rect* container, SDL_Rect* rect_array, size_t count, size_t individual_width, size_t individual_height, size_t spacing);

void sui_check_buttons(int pixelX, int pixelY);

void sui_draw_elements(SDL_Renderer* renderer);

void sui_clear_elements();

#endif