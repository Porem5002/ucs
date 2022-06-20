#include <stdlib.h>

#include "include/assetman.h"

#define DTS_USE_ARRAY
#define DTS_USE_DYNARRAY
#define DTS_USE_TREE

#include "include/dtstructs.h"

typedef struct
{
    bool was_initialized;
    asset_info_t static_storage [ASSETMAN_STATIC_STORAGE_SIZE];
    dynarray(asset_info_t) dynamic_assets;
    free_asset_function custom_free_asset_function;
} assetman_t;

static const asset_info_t empty_asset_value = { NO_ASSET, NULL };
static assetman_t assetman = {0};

bool assetman_init(free_asset_function custom_free_asset_function)
{
    if(assetman.was_initialized) return false; 

    assetman.was_initialized = true;
    assetman.custom_free_asset_function = custom_free_asset_function;
           
    for (size_t i = 0; i < ASSETMAN_STATIC_STORAGE_SIZE; i++) assetman.static_storage[i] = empty_asset_value;

    assetman.dynamic_assets = dynarray_new(asset_info_t, 0);

    return true;
}

asset_info_t* assetman_get_asset_info(asset_id_t id)
{
    if(id.save_location_id == ASSETMAN_STATIC_ID)
    {
        if(id.raw_id >= ASSETMAN_STATIC_STORAGE_SIZE) return NULL;

        return &assetman.static_storage[id.raw_id];
    }
    else if(id.save_location_id == ASSETMAN_DYNAMIC_ID)
    {
        if(id.raw_id >= dynarray_size(&assetman.dynamic_assets)) return NULL;

        return rrr_dynarray_ele(&assetman.dynamic_assets, id.raw_id);
    }

    return NULL;
}

void assetman_set_asset(asset_id_t id, asset_type_t asset_type, void* asset_data)
{
    if(id.save_location_id == ASSETMAN_STATIC_ID)
    {
       assetman.static_storage[id.raw_id] = (asset_info_t){ asset_type, asset_data };
    }
    else if(id.save_location_id == ASSETMAN_DYNAMIC_ID)
    {
        if(id.raw_id >= dynarray_size(&assetman.dynamic_assets))
        {
            size_t number_of_assets_slots_to_add = id.raw_id - dynarray_size(&assetman.dynamic_assets) + 1;
            dynarray_grow(&assetman.dynamic_assets, asset_info_t, number_of_assets_slots_to_add, &empty_asset_value);
        }

        dynarray_ele(&assetman.dynamic_assets, asset_info_t, id.raw_id) = (asset_info_t){ asset_type, asset_data };
    }
}

void* assetman_get_asset(asset_id_t id)
{
    asset_info_t* asset = assetman_get_asset_info(id);

    if(asset == NULL) return NULL;

    return asset->asset_data;
}

void assetman_free_asset(asset_id_t id)
{
    asset_info_t* asset = assetman_get_asset_info(id);

    if(asset == NULL || asset->asset_type == NO_ASSET) return;

    if(asset->asset_type != UNHANDLED_ASSET) 
        assetman.custom_free_asset_function(asset);
 
    *asset = empty_asset_value;
}

bool assetman_free_static_assets()
{
    if(!assetman.was_initialized) return false;

    for (size_t i = 0; i < ASSETMAN_STATIC_STORAGE_SIZE; i++)
    {
        asset_info_t* asset = &assetman.static_storage[i];

        if(asset->asset_type != NO_ASSET && asset->asset_type != UNHANDLED_ASSET) 
            assetman.custom_free_asset_function(asset);
    }

    return true;
}

bool assetman_free_dynamic_assets()
{
    if(!assetman.was_initialized) return false;

    for (size_t i = 0; i <  dynarray_size(&assetman.dynamic_assets); i++)
    {
        asset_info_t* asset = rrr_dynarray_ele(&assetman.dynamic_assets, i);

        if(asset->asset_type != NO_ASSET && asset->asset_type != UNHANDLED_ASSET) 
            assetman.custom_free_asset_function(asset);
    }

    return true;
}

bool assetman_finish(bool free_left_over_assets)
{
    if(!assetman.was_initialized) return false;

    if(free_left_over_assets)
    {
        assetman_free_static_assets();
        assetman_free_dynamic_assets();
    }

    assetman.was_initialized = false;
    dynarray_free(&assetman.dynamic_assets);
    
    return true;
}