#ifndef TOKEN_HEADER
#define TOKEN_HEADER

#include <stdint.h>
#include <stdbool.h>

enum
{
    TOKEN_EMPTY,
    TOKEN_SYMBOL,
    TOKEN_ID,  
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN_INTEGER
};

typedef unsigned char token_type_t;

typedef struct
{
    token_type_t type;

    union 
    {
        char symbol;
        char* identifier;
        char char_value;
        char* string_value;
        uint16_t integer_value;
    };

} token_t;

void token_free(token_t* token);

bool token_equals(token_t* token1, token_t* token2);

bool token_is_symbol(token_t* token, char symbol);

bool token_is_number(token_t* token);

bool token_is_literal(token_t* token);

#endif
