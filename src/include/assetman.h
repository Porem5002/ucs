#ifndef DEF_ASSSETMAN_HEADER
#define DEF_ASSSETMAN_HEADER

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define statasset "$"
#define dynasset ""

typedef unsigned char asset_type_t;

#define UNHANDLED_ASSET ((asset_type_t)255)

typedef struct
{
    asset_type_t asset_type;
    void* asset_data;
} asset_info_t;

typedef void (*free_asset_function)(asset_info_t*);

bool assetman_init(free_asset_function custom_free_asset_function);

void assetman_set_asset(bool is_id_safe, const char* id, asset_type_t asset_type, void* asset_data);

void* assetman_get_asset(const char* id);

bool assetman_free_static_assets();

bool assetman_free_dynamic_assets();

bool assetman_finish(bool free_left_over_assets);

#endif