#ifndef PAGER_HEADER
#define PAGER_HEADER

#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    size_t total_element_count;
    size_t max_elements_per_page;
    size_t current_page_start;
    size_t current_page_end;
} pager_t;

void pager_init(pager_t* pager, size_t total_element_count, size_t max_elements_per_page);

bool pager_next_page(pager_t* pager);

bool pager_prev_page(pager_t* pager);

bool pager_is_first_page(pager_t* pager);

bool pager_is_last_page(pager_t* pager);

#endif