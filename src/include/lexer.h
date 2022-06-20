#ifndef LEXER_HEADER
#define LEXER_HEADER

#include "token.h"

#define DTS_USE_ARRAY
#define DTS_USE_DYNARRAY
#define DTS_USE_TREE

#include "dtstructs.h"

typedef struct
{
    char* src;
    char* current_char_ptr;
} lexer_t;

void lexer_init(lexer_t* lexer, char* src);

void lexer_restart(lexer_t* lexer);

array(token_t) lexer_collect_tokens(lexer_t* lexer);

token_t lexer_collect_next_token(lexer_t* lexer);

token_t lexer_collect_symbol(lexer_t* lexer);

token_t lexer_collect_id(lexer_t* lexer);

token_t lexer_collect_char(lexer_t* lexer);

token_t lexer_collect_string(lexer_t* lexer);

token_t lexer_collect_number(lexer_t* lexer);

token_t lexer_collect_next_token_of_type(lexer_t* lexer, token_type_t token_type);

bool lexer_go_to_next_token_equal_to(lexer_t* lexer, token_t* token_to_find);

#endif