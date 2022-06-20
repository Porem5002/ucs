#include "include/pager.h"

void pager_init(pager_t* pager, size_t total_element_count, size_t max_elements_per_page)
{
    pager->total_element_count = total_element_count;
    pager->max_elements_per_page = max_elements_per_page;
    pager->current_page_end = 0;
}

bool pager_next_page(pager_t* pager)
{
    if(pager_is_last_page(pager)) return false;

    pager->current_page_start = pager->current_page_end;
    pager->current_page_end += pager->max_elements_per_page;

    if(pager->current_page_end > pager->total_element_count)
    {
        pager->current_page_end = pager->total_element_count;
    }

    return true;
}

bool pager_prev_page(pager_t* pager)
{
    if(pager_is_first_page(pager)) return false;

    pager->current_page_end = pager->current_page_start;
    pager->current_page_start -= pager->max_elements_per_page;

    return true;
}

bool pager_is_first_page(pager_t* pager)
{
    return pager->current_page_start == 0;
}

bool pager_is_last_page(pager_t* pager)
{
    return pager->current_page_end == pager->total_element_count;
}