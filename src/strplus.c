#include <stdlib.h>
#include <stdarg.h>

#include "include/strplus.h"

static void copy_bytes(void* source, void* destination, size_t byte_count)
{
    size_t i;

    for (i = 0; i < byte_count; i++)
    {
        ((char*)destination)[i] = ((char*)source)[i];
    }
}

size_t string_length(string_t string)
{
    size_t length = 0;
    char* current_char = string;

    while (*current_char != '\0')
    {
        length++;
        current_char++;    
    }
    
    return length;
}

bool string_equals(string_t stringA, string_t stringB)
{
    size_t i;
    size_t lengthA = string_length(stringA);
    size_t lengthB = string_length(stringB);

    if(lengthA != lengthB) return false;

    for (i = 0; i < lengthA; i++)
    {
        if(stringA[i] != stringB[i]) 
            return false;
    }
    
    return true;   
}

bool string_starts_with(string_t string, string_t target)
{
    size_t i;
    size_t length_string = string_length(string);
    size_t length_target = string_length(target);

    if(length_string < length_target) return false;

    for (i = 0; i < length_target; i++)
    {
        if(string[i] != target[i]) 
            return false;
    }
    
    return true;   
}

bool string_ends_with(string_t string, string_t target)
{
    size_t i;
    size_t length_string = string_length(string);
    size_t length_target = string_length(target);

    if(length_string < length_target) return false;

    for (i = 0; i < length_target; i++)
    {
        size_t end_offset = length_string - length_target; 

        if(string[end_offset + i] != target[i]) 
            return false;
    }
    
    return true; 
}

char* string_find(string_t string, string_t target)
{
    char* current_char_ptr = string;

    while (*current_char_ptr == '\0')
    {
        if(string_starts_with(current_char_ptr, target))
            return current_char_ptr;

        current_char_ptr++;
    }
    
    return NULL;
}

string_t string_heap_copy(string_t string)
{
    size_t length = string_length(string);
    string_t copy = malloc(length + 1);

    copy_bytes(string, copy, length);
    copy[length] = '\0';

    return copy;
}

string_t string_heap_concat(string_t stringA, string_t stringB)
{
    size_t lengthA = string_length(stringA);
    size_t lengthB = string_length(stringB);
    string_t concat = malloc(lengthA + lengthB + 1);

    copy_bytes(stringA, concat, lengthA);
    copy_bytes(stringB, concat + lengthA, lengthB);
    concat[lengthA + lengthB] = '\0';

    return concat;
}

bool string_copy_to(string_t source, string_t destination, size_t destination_size)
{
    size_t length = string_length(source);

    if(destination_size < length + 1) return false;

    copy_bytes(source, destination, length);
    destination[length] = '\0';

    return true;
}

bool string_concat_to(string_t stringA, string_t stringB, string_t destination, size_t destination_size)
{
    size_t lengthA = string_length(stringA);
    size_t lengthB = string_length(stringB);

    if(destination_size < lengthA + lengthB + 1) return false;

    copy_bytes(stringA, destination, lengthA);
    copy_bytes(stringB, destination + lengthA, lengthB);
    destination[lengthA + lengthB] = '\0';

    return true;
}

size_t string_view_length(string_view_t string_view)
{
    return string_view.length;
}

string_t string_view_to_heap_string(string_view_t view)
{
    string_t string = malloc(view.length + 1);

    copy_bytes(view.start, string, view.length);
    string[view.length] = '\0';

    return string;
}

bool string_view_to_string(string_view_t view, string_t destination, size_t destination_size)
{
    if(destination_size < view.length + 1) return false;

    copy_bytes(view.start, destination, view.length);
    destination[view.length] = '\0';

    return true;
}

string_view_t string_extract_string_view(string_t string, size_t start, size_t end)
{
    string_view_t view;
    view.length = end - start + 1;
    view.start = string + start;
    return view;
}