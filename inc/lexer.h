
#ifndef __DICELANG_LEXER_H__
#define __DICELANG_LEXER_H__

#include <stdio.h>

#include <ustd/common.h>
#include <ustd/range.h>

enum dicelang_token_flavour {
    DTOK_invalid,
    DTOK_empty,

    DTOK_line_end,
    DTOK_file_end,
    DTOK_identifier,
    DTOK_value,
    DTOK_value_real,
    DTOK_separator,
    DTOK_addition,
    DTOK_substraction,
    DTOK_d,
    DTOK_designator,
    DTOK_open_parenthesis,
    DTOK_close_parenthesis,
    DTOK_open_bracket,
    DTOK_close_bracket,
    DTOK_open_sq_bracket,
    DTOK_close_sq_bracket,

    DTOK_NUMBER,
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