#ifndef STRPLUS_HEADER
#define STRPLUS_HEADER

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

typedef char* string_t;

size_t string_length(string_t string);

bool string_equals(string_t stringA, string_t stringB);

bool string_starts_with(string_t string, string_t target);

bool string_ends_with(string_t string, string_t target);

bool string_contains(string_t string, string_t target);

char* string_find(string_t string, string_t target);

/**
* Creates a HEAP allocated copy of the specified string.
*
* \returns a pointer to the allocated memory block. 
*/
string_t string_heap_copy(string_t string);

/**
* Concatenates 2 strings and stores the resulting string in a HEAP allocated memory block.
*
* \returns a pointer to the allocated memory block. 
*/
string_t string_heap_concat(string_t stringA, string_t stringB);

/**
* Copies a string into a given destination if the size of the destination is sufficient to contain all the data of the original string.
*
* \param source string to be copied.
* \param destination destination of the copied string.
* \param destination_size size of the destination block.
*
* \returns true if the operation was successful (if the destination was big enough to acomudate the data), otherwise returns false. 
*/
bool string_copy_to(string_t source, string_t destination, size_t destination_size);

/**
* Performs a concatenation of 2 strings and stores the resulting string in a given destination if the size of the destination is sufficient to contain all the data of the concatenation result.
*
* \param destination destination of the concatenated string.
* \param destination_size size of the destination block.
*
* \returns true if the operation was successful (if the destination was big enough to acomudate the data), otherwise returns false. 
*/
bool string_concat_to(string_t stringA, string_t stringB, string_t destination, size_t destination_size);

typedef struct 
{
    size_t length;
    char* start;
} string_view_t;

size_t string_view_length(string_view_t string_view);

/**
* Creates a HEAP allocated string containing the characters within the given string view interval.
*
* \returns a pointer to the allocated memory block. 
*/
string_t string_view_to_heap_string(string_view_t view);

/**
* Extracts all the characters contained within the given string view interval and stores them in the given destination, if the size of the destination is sufficient to contain all the characters.
*
* \param destination destination of the characters.
* \param destination_size size of the destination block.
*
* \returns true if the operation was successful (if the destination was big enough to acomudate the data), otherwise returns false. 
*/
bool string_view_to_string(string_view_t view, string_t destination, size_t destination_size);

/**
* Generates a string view of the given string based on the interval defined by start and end.
*
* \param start start of the interval (inclusive).
* \param end end of the interval (inclusive).
*
* \returns the generated string view. 
*/
string_view_t string_extract_string_view(string_t string, size_t start, size_t end);

#endif