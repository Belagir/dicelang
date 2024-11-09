
#ifndef __DICELANG_LEXER_H__
#define __DICELANG_LEXER_H__

#include <stdio.h>

#include <ustd/common.h>
#include <ustd/range.h>

enum dicelang_token_flavour {
    DICELANG_TOKEN_invalid,

    DICELANG_TOKEN_line_end,
    DICELANG_TOKEN_file_end,
    DICELANG_TOKEN_identifier,
    DICELANG_TOKEN_value,
    DICELANG_TOKEN_value_real,
    DICELANG_TOKEN_separator,
    DICELANG_TOKEN_addition,
    DICELANG_TOKEN_substraction,
    DICELANG_TOKEN_d,
    DICELANG_TOKEN_designator,
    DICELANG_TOKEN_open_parenthesis,
    DICELANG_TOKEN_close_parenthesis,
    DICELANG_TOKEN_open_bracket,
    DICELANG_TOKEN_close_bracket,
    DICELANG_TOKEN_open_sq_bracket,
    DICELANG_TOKEN_close_sq_bracket,

    DICELANG_TOKEN_NUMBER,
};

struct dicelang_token {
    enum dicelang_token_flavour flavour;

    struct { const char *source; size_t source_length; } value;
    struct { u32 line, col; } where;
};

typedef RANGE(struct dicelang_token) RANGE_TOKEN;

RANGE_TOKEN *dicelang_tokenize(const char *source_code, struct allocator alloc);

void dicelang_token_dump(RANGE_TOKEN *tokens, FILE *to_file);

#endif