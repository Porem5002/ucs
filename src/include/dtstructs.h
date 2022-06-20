#ifndef DTS_LIB_MAIN_DEFS
#define DTS_LIB_MAIN_DEFS

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef DTS_DEBUG_CHECKS
#include <stdio.h>
#include <inttypes.h>
#endif

/* Inline only if the standard supports it */
#ifndef __STDC_VERSION__
#define DTSDEF_DEFAULT static
#else
#define DTSDEF_DEFAULT static inline
#endif

#ifndef DTSDEF
#define DTSDEF DTSDEF_DEFAULT
#endif

#endif

#if !defined(DTS_LIB_ARRAY_DEFS) && defined(DTS_USE_ARRAY)
#define DTS_LIB_ARRAY_DEFS

typedef struct ARRAY_STRUCT 
{
    size_t size;
    void* data;
} array_t;

#define array(TYPE) array_t

// ARRAY: Main Functions

#define array_stk(TYPE, ...) (array_t){ .size = (sizeof((TYPE[]){ __VA_ARGS__ }) / sizeof(TYPE)), .data = (TYPE[]){ __VA_ARGS__ } }
#define array_stt(SIZE, DATA_PTR) (array_t){ .size = SIZE, .data = DATA_PTR }
#define array_new(TYPE, COUNT) rrr_array_new(COUNT, sizeof(TYPE))
#define array_ele(ARRAY, TYPE, INDEX) (*(TYPE*)rrr_array_ele(ARRAY, sizeof(TYPE), INDEX))

DTSDEF size_t array_size(array_t* array)
{
    return array->size;
}

DTSDEF void array_free(array_t* array)
{
    #ifdef DTS_DEBUG_CHECKS
    if(array == NULL)
    {
        fputs("Attempting to free array, but the pointer to the array is null!\n", stdout);
        exit(1);
    }
    if(array_size(array) == 0 && array->data != NULL)
    {
        fputs("Attempting to free an invalid array!\n", stdout);
        printf("More Info:\n\tThe array has no size but a block of data is associated to it (data: %p).\n", array->data);
        exit(1);
    }
    #endif

    free(array->data);
    array->data = NULL;
}

// ARRAY: BACKING Functions
DTSDEF array_t rrr_array_new(size_t element_count, size_t element_size)
{
    array_t array;
    array.size = element_count;
    array.data = malloc(element_size * element_count);
    return array;
}

DTSDEF void* rrr_array_ele(array_t* array, size_t element_size, size_t index)
{
    #ifdef DTS_DEBUG_CHECKS
    if(array_size(array) <= index)
    {
        fputs("Attempting to access an out of bounds element from an array!\n", stdout);
        printf("More Info:\n\t(array size: %"PRIu64", element index: %"PRIu64")\n", array_size(array), index);
        exit(1);
    }
    #endif

    return &((char*)array->data)[element_size * index];
}

#endif

#if !defined(DTS_LIB_DYNARRAY_DEFS) && defined(DTS_USE_DYNARRAY)
#define DTS_LIB_DYNARRAY_DEFS

typedef struct DYNARRAY_STRUCT
{
    size_t element_count;
    size_t element_size;
    size_t top_element_index;
    char* data;
} dynarray_t;

#define DYNARRAY_DEFAULT_ELEMENT_COUNT 8

#define dynarray(TYPE) dynarray_t

// DYNARRAY: Main Functions

#define dynarray_new(TYPE, ELEMENT_COUNT) rrr_dynarray_new(sizeof(TYPE), ELEMENT_COUNT)

#define dynarray_add(ARRAY, TYPE, DATA) rrr_dynarray_add(ARRAY, DATA)

#define dynarray_grow(ARRAY, TYPE, GROWTH_FACTOR, DATA) rrr_dynarray_grow(ARRAY, GROWTH_FACTOR, DATA)

#define dynarray_ele(ARRAY, TYPE, INDEX) (*(TYPE*)(rrr_dynarray_ele(ARRAY, INDEX)))

DTSDEF size_t dynarray_allocated_size(dynarray_t* array)
{
    return array->element_count;
}

DTSDEF size_t dynarray_size(dynarray_t* array)
{
    return array->top_element_index;
}

DTSDEF void dynarray_free(dynarray_t* array)
{
    free(array->data);
    array->data = NULL;  
}

// DYNARRAY: Backing Functions

DTSDEF dynarray_t rrr_dynarray_new(size_t element_size, size_t element_count)
{
    dynarray_t array;
    
    array.top_element_index = element_count;
    array.element_size = element_size;

    if(element_count != 0)
        array.element_count = (DYNARRAY_DEFAULT_ELEMENT_COUNT - element_count % DYNARRAY_DEFAULT_ELEMENT_COUNT) + element_count;
    else
        array.element_count = DYNARRAY_DEFAULT_ELEMENT_COUNT;

    array.data = malloc(array.element_size * array.element_count);

    return array;
}

DTSDEF size_t rrr_dynarray_add(dynarray_t* array, void* data_to_add)
{
    char* new_element;

    if(array->element_count <= array->top_element_index)
    {
        array->element_count *= 2;
        array->data = realloc(array->data, array->element_size * array->element_count);
    }

    new_element = &array->data[array->top_element_index * array->element_size]; 

    if(data_to_add != NULL)
    {
        memcpy(new_element, data_to_add, array->element_size);
    }

    array->top_element_index++;

    return array->top_element_index - 1;
}

DTSDEF size_t rrr_dynarray_grow(dynarray_t* array, size_t growth_factor, void* default_value_data)
{
    size_t first_element_added_index = dynarray_size(array);

    if(growth_factor == 0) return first_element_added_index;

    for (size_t i = 0; i < growth_factor; i++)
        rrr_dynarray_add(array, default_value_data);

    return first_element_added_index;
}

DTSDEF void* rrr_dynarray_ele(dynarray_t* array, size_t index)
{
    #ifdef DTS_DEBUG_CHECKS
    if(dynarray_size(array) <= index)
    {
        fputs("Attempting to access an out of bounds element from a dynamic array!\n", stdout);
        printf("More Info:\n\t(dynamic array size: %"PRIu64", element index: %"PRIu64")\n", dynarray_size(array), index);
        exit(1);
    }
    #endif

    return &array->data[index * array->element_size];   
}

#endif

#if !defined(DTS_LIB_LIST_DEFS) && defined(DTS_USE_LIST)
#define DTS_LIB_LIST_DEFS

typedef struct NODE_STRUCT
{
    struct NODE_STRUCT* next;
    char data [0];
} lnode_t;

typedef lnode_t* list_t;

#define list(TYPE) list_t
#define EMPTY_LIST NULL

// LIST: MAIN FUNCTIONS

#define node_new(TYPE, DATA)    rrr_node_new(sizeof(TYPE), DATA)
#define node_new_empty(TYPE)    rrr_node_new(sizeof(TYPE), NULL)

#define list_append(LIST, TYPE, DATA)           rrr_list_append(LIST, sizeof(TYPE), DATA)
#define list_insert(LIST, TYPE, DATA, INDEX)    rrr_list_insert(LIST, sizeof(TYPE), DATA, INDEX)
#define list_eleptr(LIST, INDEX)                (&list_get_node(LIST, INDEX)->data) 
#define list_ele(LIST, TYPE, INDEX)             (*(TYPE*)(list_eleptr(LIST, INDEX)))

DTSDEF size_t list_size(list_t* list)
{
    lnode_t* last_node = *list;
    size_t size = 0;

    while (last_node != NULL) 
    {
        last_node = last_node->next;
        size++;
    }

    return size;
}

DTSDEF lnode_t* list_get_node(list_t* list, size_t index)
{
    lnode_t* node = *list;

    if(node == NULL) return NULL;

    for (size_t i = 0; i < index; i++)
    {
        if(node->next == NULL) return NULL;

        node = node->next;
    }
    
    return node;
}

DTSDEF void list_free(list_t* list)
{
    lnode_t* current_node = *list;

    while (current_node != NULL) 
    {
        lnode_t* next_node = current_node->next;

        free(current_node);

        current_node = next_node;
    }
}

// LIST: BACKING FUNCTIONS

DTSDEF lnode_t* rrr_node_new(size_t data_size, void* data)
{
    lnode_t* node = malloc(sizeof(lnode_t) + data_size);
    node->next = NULL;
    
    if(data != NULL) memcpy((void*)node->data, data, data_size);
    
    return node;
}

DTSDEF void rrr_list_append(list_t* list, size_t data_size, void* data)
{
    lnode_t** next_of_last = list;

    while (*next_of_last != NULL)
    {
        next_of_last = &((*next_of_last)->next);
    }

    *next_of_last = rrr_node_new(data_size, data);
}

DTSDEF void rrr_list_insert(list_t* list, size_t data_size, void* data, size_t index)
{
    lnode_t* new_node = rrr_node_new(data_size, data);

    if(index == 0)
    {
        new_node->next = *list;
        *list = new_node;
    }
    else
    {
        lnode_t* prev_node = list_get_node(list, index - 1);
        new_node->next = prev_node->next;
        prev_node->next = new_node;
    }
}

#endif

#if !defined(DTS_LIB_TREE_DEFS) && defined(DTS_USE_TREE)
#define DTS_LIB_TREE_DEFS

typedef struct TREE_NODE_STRUCT
{
    struct TREE_NODE_STRUCT** leafs;
    size_t leaf_count;
    char data [0];
} tnode_t;

typedef tnode_t* tree_t;

#define tree(TYPE) tree_t

DTSDEF void rrr_tree_max_depth(tree_t tree, size_t* current_max_depth, size_t current_depth)
{
    if(current_depth >= *current_max_depth) *current_max_depth = current_depth;

    for (size_t i = 0; i < tree->leaf_count; i++)
    {
        rrr_tree_max_depth(tree->leafs[i], current_max_depth, current_depth + 1);
    }
}

DTSDEF size_t tree_max_depth(tree_t tree)
{
    size_t max_depth = 0;

    rrr_tree_max_depth(tree, &max_depth, 0);
    
    return max_depth;
}

DTSDEF size_t tree_child_count(tree_t tree)
{
    return tree->leaf_count;
}

DTSDEF void tree_free(tree_t tree)
{
    for (size_t i = 0; i < tree->leaf_count; i++)
        tree_free(tree->leafs[i]);

    if(tree->leafs != NULL)
        free(tree->leafs);
    
    free(tree);
}

DTSDEF void tree_insert_subtree(tree_t tree, tree_t subtree)
{
    tree->leaf_count++;
    tree->leafs = realloc(tree->leafs, sizeof(tree_t) * tree->leaf_count);
    tree->leafs[tree->leaf_count - 1] = subtree; 
}

DTSDEF void tree_remove_subtree(tree_t tree, size_t subtree_index)
{
    tree_free(tree->leafs[subtree_index]);

    for (size_t i = subtree_index; i < tree->leaf_count - 1; i++)
    {
        tree->leafs[i] = tree->leafs[i + 1];
    }

    tree->leaf_count--;
    tree->leafs = realloc(tree->leafs, sizeof(tree_t) * tree->leaf_count);
}

DTSDEF tree_t tree_get_subtree(tree_t tree, size_t subtree_index)
{
    if(tree->leaf_count <= subtree_index) return NULL;

    return tree->leafs[subtree_index];
}

#define tree_new_empty(TYPE) rrr_tree_new(sizeof(TYPE), NULL)

#define tree_new(TYPE, DATA) rrr_tree_new(sizeof(TYPE), DATA)

#define tree_value(TREE, TYPE) *((TYPE*)rrr_tree_value(TREE))

#define tree_insert_empty(TREE, TYPE) rrr_tree_insert(TREE, sizeof(TYPE), NULL)

#define tree_insert(TREE, TYPE, DATA) rrr_tree_insert(TREE, sizeof(TYPE), DATA)´

DTSDEF tree_t rrr_tree_new(size_t data_size, void* data)
{
    tree_t tree = malloc(sizeof(tnode_t) + data_size);
    tree->leafs = NULL;
    tree->leaf_count = 0;

    if(data != NULL) memcpy(tree->data, data, data_size);
    
    return tree;
}

DTSDEF void* rrr_tree_value(tree_t tree)
{
    return tree->data;
}

DTSDEF tree_t rrr_tree_insert(tree_t tree, size_t data_size, void* data)
{
    tree->leaf_count++;
    tree->leafs = realloc(tree->leafs, sizeof(tree_t) * tree->leaf_count);
    tree->leafs[tree->leaf_count - 1] = rrr_tree_new(data_size, data);
    return tree->leafs[tree->leaf_count - 1];
}

#endif

#if !defined(DTS_LIB_CAST_ARRAY_DYNARRAY_DEFS) && defined(DTS_USE_DYNARRAY) && defined(DTS_USE_ARRAY)
#define DTS_LIB_CAST_ARRAY_DYNARRAY_DEFS

#define array_to_dynarray(ARRAY, TYPE)            rrr_array_to_dynarray(ARRAY, sizeof(TYPE))
#define dynarray_to_array(DYNAMIC_ARRAY, TYPE)    rrr_dynarray_to_array(DYNAMIC_ARRAY)

DTSDEF dynarray_t rrr_array_to_dynarray(array_t* array, size_t data_size)
{
    dynarray_t dynarray;
    dynarray.element_count = array->size;
    dynarray.element_size = data_size;
    dynarray.top_element_index = array->size;
    dynarray.data = array->data;
    return dynarray;
}

DTSDEF array_t rrr_dynarray_to_array(dynarray_t* dynarray)
{
    array_t array;
    array.size = dynarray->top_element_index;
    array.data = realloc(dynarray->data, array.size * dynarray->element_size);
    return array;
}

#endif