#include <stdlib.h>

#include "include/assetman.h"

#define DTS_USE_ARRAY
#define DTS_USE_DYNARRAY
#define DTS_USE_TREE

#include "include/dtstructs.h"

#define ASSETMAN_TABLE_SIZE 200

typedef struct assetman_table_NODE_STRUCT
{
    char* key;
    asset_info_t value;
    struct assetman_table_NODE_STRUCT* next;
    bool is_safe_key;
} assetman_node_t;

typedef struct
{
    assetman_node_t* buckets [ASSETMAN_TABLE_SIZE];
} assetman_table_t;

typedef struct
{
    bool was_initialized;
    assetman_table_t static_assets;
    assetman_table_t dynamic_assets;
    free_asset_function custom_free_asset_function;
} assetman_t;

static assetman_t assetman = {0};

static assetman_table_t assetman_table_init();
static void assetman_table_set(assetman_table_t* table, bool is_safe_key, const char* key, asset_info_t value);
static void* assetman_table_get(assetman_table_t* table, const char* key);
static void assetman_table_clear(assetman_table_t* table, free_asset_function asset_cleanup_function);
static size_t assetman_table_hash(const void* key);

bool assetman_init(free_asset_function custom_free_asset_function)
{
    if(assetman.was_initialized) return false; 

    assetman.was_initialized = true;
    assetman.custom_free_asset_function = custom_free_asset_function;

    assetman.static_assets = assetman_table_init();
    assetman.dynamic_assets = assetman_table_init();

    return true;
}

void assetman_set_asset(bool is_id_safe, const char* id, asset_type_t asset_type, void* asset_data)
{
    assetman_table_t* target_table;

    target_table = *id == '$' ? &assetman.static_assets : &assetman.dynamic_assets;

    assetman_table_set(target_table, is_id_safe, id, (asset_info_t){ asset_type, asset_data });
}

void* assetman_get_asset(const char* id)
{
    assetman_table_t* target_table;

    target_table = *id == '$' ? &assetman.static_assets : &assetman.dynamic_assets;

    return assetman_table_get(target_table, id);
}

bool assetman_free_static_assets()
{
    if(!assetman.was_initialized) return false;

    assetman_table_clear(&assetman.static_assets, assetman.custom_free_asset_function);

    return true;
}

bool assetman_free_dynamic_assets()
{
    if(!assetman.was_initialized) return false;

    assetman_table_clear(&assetman.dynamic_assets, assetman.custom_free_asset_function);

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
    
    return true;
}

static assetman_table_t assetman_table_init()
{
    return (assetman_table_t){0};
}

static void assetman_table_set(assetman_table_t* table, bool is_safe_key, const char* key, asset_info_t value)
{
    size_t hash = assetman_table_hash(key) % ASSETMAN_TABLE_SIZE;

    assetman_node_t** dest = &table->buckets[hash];

    while(*dest != NULL)
    {
        if(strcmp(key, (*dest)->key) == 0)
        {
            (*dest)->value = value;
            return;
        }

        dest = &(*dest)->next;
    }

    *dest = malloc(sizeof(assetman_node_t));
    (*dest)->is_safe_key = is_safe_key;
    (*dest)->value = value;
    (*dest)->next = NULL;

    if(is_safe_key)
    {
        (*dest)->key = (char*)key;
        return;
    }

    (*dest)->key = malloc(strlen(key) + 1);
    memcpy((*dest)->key, key, strlen(key));
    (*dest)->key[strlen(key)] = '\0';
}

static void* assetman_table_get(assetman_table_t* table, const char* key)
{
    size_t hash = assetman_table_hash(key) % ASSETMAN_TABLE_SIZE;

    assetman_node_t* curr_node = table->buckets[hash];

    while(curr_node != NULL)
    {
        if(strcmp(key, curr_node->key) == 0)
            return curr_node->value.asset_data;

        curr_node = curr_node->next;
    }

    return NULL;
}

static void assetman_table_clear(assetman_table_t* table, free_asset_function asset_cleanup_function)
{
    for (size_t i = 0; i < ASSETMAN_TABLE_SIZE; i++)
    {
        assetman_node_t* curr_node = table->buckets[i];
        
        while(curr_node != NULL)
        {
            assetman_node_t* next = curr_node->next;

            if(!curr_node->is_safe_key)
                free(curr_node->key);

            if(curr_node->value.asset_type != UNHANDLED_ASSET)
                asset_cleanup_function(&curr_node->value);

            free(curr_node);
            curr_node = next;
        }
    }

    *table = assetman_table_init();
}

static size_t assetman_table_hash(const void* key)
{
    const unsigned char* str = key;
    size_t hash = 5381;
    int c;

    while ((c = *str++)) 
        hash = ((hash << 5) + hash) + c;

    return hash;
}