#include <stdlib.h>
#include <string.h>

#include "include/token.h"

void token_free(token_t* token)
{
    switch (token->type)
    {
        case TOKEN_ID:
            free(token->identifier);
            token->identifier = NULL;
            break;
        case TOKEN_STRING:  
            free(token->string_value);
            token->string_value = NULL;
            break;
        default: break;
    }
}

bool token_equals(token_t* token1, token_t* token2)
{
    if(token1->type != token2->type) return false;

    switch (token1->type)
    {
        case TOKEN_SYMBOL:  return token1->symbol == token2->symbol;
        case TOKEN_ID:      return strcmp(token1->identifier, token2->identifier) == 0;
        case TOKEN_CHAR:    return token1->char_value == token2->char_value;
        case TOKEN_STRING:  return strcmp(token1->string_value, token2->string_value) == 0;
        case TOKEN_INTEGER: return token1->integer_value == token2->integer_value;
        default: break;
    }

    return true;
}

bool token_is_symbol(token_t* token, char symbol)
{
    return token->type == TOKEN_SYMBOL && token->symbol == symbol;
}

bool token_is_number(token_t* token)
{
    return token->type == TOKEN_INTEGER;
}

bool token_is_literal(token_t* token)
{
    return token->type == TOKEN_CHAR || token->type == TOKEN_STRING || token_is_number(token);
}