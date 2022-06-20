#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "include/lexer.h"

static void lexer_skip_spaces(lexer_t* lexer)
{
    while(*lexer->current_char_ptr == ' ' || 
          *lexer->current_char_ptr == '\t' || 
          *lexer->current_char_ptr == '\r' || 
          *lexer->current_char_ptr == '\n')
    {
        lexer->current_char_ptr++;
    }
}

void lexer_init(lexer_t* lexer, char* src)
{
    lexer->src = src;
    lexer->current_char_ptr = src;
}

void lexer_restart(lexer_t* lexer)
{
    lexer->current_char_ptr = lexer->src;
}

array(token_t) lexer_collect_tokens(lexer_t* lexer)
{
    dynarray(token_t) tokens = dynarray_new(token_t, 0);

    lexer_skip_spaces(lexer);

    while (*lexer->current_char_ptr != '\0')
    {
        token_t token = lexer_collect_next_token(lexer);
        dynarray_add(&tokens, token_t, &token);
        lexer_skip_spaces(lexer);
    }

    return dynarray_to_array(&tokens, token_t);
}

token_t lexer_collect_next_token(lexer_t* lexer)
{
    if(isdigit(*lexer->current_char_ptr))
    {
       return lexer_collect_number(lexer); 
    }

    if(isalpha(*lexer->current_char_ptr) || *lexer->current_char_ptr == '_')
    {
       return lexer_collect_id(lexer); 
    }

    switch (*lexer->current_char_ptr)
    {
        case '\'': return lexer_collect_char(lexer);
        case '\"': return lexer_collect_string(lexer);
        default: return lexer_collect_symbol(lexer);
    }
}

token_t lexer_collect_symbol(lexer_t* lexer)
{
    token_t token;
    token.type = TOKEN_SYMBOL;
    token.symbol = *lexer->current_char_ptr;
    lexer->current_char_ptr++;
    return token;
}

token_t lexer_collect_id(lexer_t* lexer)
{
    token_t token;
    token.type = TOKEN_ID;

    char* start_id = lexer->current_char_ptr;
    size_t id_size;

    while(isalnum(*lexer->current_char_ptr) || *lexer->current_char_ptr == '_') lexer->current_char_ptr++;
    
    id_size = lexer->current_char_ptr - start_id;

    token.identifier = malloc(id_size + 1);
    memcpy(token.identifier, start_id, id_size);
    token.identifier[id_size] = '\0';

    return token;
}

token_t lexer_collect_char(lexer_t* lexer)
{
    token_t token;
    token.type = TOKEN_CHAR;

    lexer->current_char_ptr++;

    token.char_value = *lexer->current_char_ptr;
    
    lexer->current_char_ptr++;
    lexer->current_char_ptr++;

    return token;
}

token_t lexer_collect_string(lexer_t* lexer)
{
    token_t token;
    token.type = TOKEN_STRING;

    lexer->current_char_ptr++;

    char* start_string = lexer->current_char_ptr;
    size_t string_size;

    while(*lexer->current_char_ptr != '\"') lexer->current_char_ptr++;
    
    string_size = lexer->current_char_ptr - start_string;

    token.identifier = malloc(string_size + 1);
    memcpy(token.identifier, start_string, string_size);
    token.identifier[string_size] = '\0';

    lexer->current_char_ptr++;

    return token;
}

token_t lexer_collect_number(lexer_t* lexer)
{
    token_t token;
    char* end_char;

    token.type = TOKEN_INTEGER;
    token.integer_value = strtoull(lexer->current_char_ptr, &end_char, 10);

    lexer->current_char_ptr = end_char;
    return token;
}

token_t lexer_collect_next_token_of_type(lexer_t* lexer, token_type_t token_type)
{
    while (*lexer->current_char_ptr != '\0')
    {
        token_t token = lexer_collect_next_token(lexer);

        if(token.type == token_type)
        {
            return token;
        }
    }

    return (token_t){ .type = TOKEN_EMPTY };
}

bool lexer_go_to_next_token_equal_to(lexer_t* lexer, token_t* token_to_find)
{
    while (*lexer->current_char_ptr != '\0')
    {
        token_t token = lexer_collect_next_token(lexer);

        if(token_equals(&token, token_to_find))
        {
            token_free(&token);
            return true;
        }

        token_free(&token);
    }

    return false;
}