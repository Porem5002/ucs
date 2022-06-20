#ifndef DEF_ASSSETMAN_HEADER
#define DEF_ASSSETMAN_HEADER

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned char asset_type_t;

#define UNHANDLED_ASSET ((asset_type_t)254)
#define NO_ASSET        ((asset_type_t)255)

#define ASSETMAN_STATIC_STORAGE_SIZE 20 

enum
{
    ASSETMAN_STATIC_ID,
    ASSETMAN_DYNAMIC_ID
};

typedef struct
{
    uint8_t save_location_id;
    size_t raw_id;
} asset_id_t;

typedef struct
{
    asset_type_t asset_type;
    void* asset_data;
} asset_info_t;

typedef void (*free_asset_function)(asset_info_t*);

#define assetman_static_id(raw_id) (asset_id_t){ ASSETMAN_STATIC_ID, raw_id }
#define assetman_dynamic_id(raw_id) (asset_id_t){ ASSETMAN_DYNAMIC_ID, raw_id }

bool assetman_init(free_asset_function custom_free_asset_function);

asset_info_t* assetman_get_asset_info(asset_id_t id);

void assetman_set_asset(asset_id_t id, asset_type_t asset_type, void* asset_data);

void* assetman_get_asset(asset_id_t id);

void assetman_free_asset(asset_id_t id);

bool assetman_free_static_assets();

bool assetman_free_dynamic_assets();

bool assetman_finish(bool free_left_over_assets);

#endif