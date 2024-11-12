
#include <ustd/range.h>

#include <dicelang.h>
#include <parser.h>

#define DICELANG_STX_LOOKAHEAD (4)

static enum dicelang_ext_syntax dicelang_match_syntax(const struct dicelang_token *token);

// RULES
// VARIABLE   --> identifier
// EXPRESSION --> EXPRESSION _d EXPRESSION
// EXPRESSION --> EXPRESSION + EXPRESSION
// EXPRESSION --> VARIABLE
// ASSIGNMENT --> VARIABLE designator EXPRESSION

static const struct { u32 grammar[DICELANG_STX_LOOKAHEAD]; enum dicelang_ext_syntax what; } dicelang_syntax_definitions[] =
{
        { { DTOK_identifier }, .what = DSTX_variable },

        { { DSTX_expression, DTOK_d, DSTX_expression },        .what = DSTX_expression },
        { { DSTX_expression, DTOK_addition, DSTX_expression }, .what = DSTX_expression },
        { { DSTX_variable },                                   .what = DSTX_expression },
        { { DTOK_value },                                      .what = DSTX_expression },

        { { DSTX_variable, DTOK_designator, DSTX_expression }, .what = DSTX_assignment },

};

// R : 4 + 2_d20

void dicelang_parse(RANGE_TOKEN *tokens, allocator alloc)
{
    RANGE(u32) *syntax_stack = nullptr;

    if (!tokens || !tokens->data) {
        return;
    }

    for (const struct dicelang_token *index = tokens->data; index < (tokens->data + tokens->length) ; index += 1) {
        printf("%d\n", dicelang_match_syntax(index));
    }
}

static enum dicelang_ext_syntax dicelang_match_syntax(const struct dicelang_token *token)
{
    bool match_found = false;
    size_t pos_stx = 0u;
    size_t pos_tok = 0u;
    bool token_in_range = false;
    bool matches_current_syntax = false;

    if (!token) {
        return DTOK_invalid;
    }

    while (!match_found && (pos_stx < COUNT_OF(dicelang_syntax_definitions))) {
        pos_tok = 0u;

        do {
            token_in_range = (token[pos_tok].flavour != DTOK_line_end) && (token[pos_tok].flavour != DTOK_file_end);
            matches_current_syntax = token_in_range && (token[pos_tok].flavour == dicelang_syntax_definitions[pos_stx].grammar[pos_tok]);

            pos_tok += matches_current_syntax;

        } while (matches_current_syntax && !match_found);

        match_found = ((pos_tok >= DICELANG_STX_LOOKAHEAD) || (dicelang_syntax_definitions[pos_stx].grammar[pos_tok] == DTOK_invalid));
        pos_stx += !match_found;
    }

    if (match_found) {
        return dicelang_syntax_definitions[pos_stx].what;
    }
    return DTOK_invalid;
}