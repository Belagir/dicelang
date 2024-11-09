
#include <lexer.h>

static const char *token_names[] = {
    [DICELANG_TOKEN_invalid]            = "invalid",

    [DICELANG_TOKEN_line_end]           = "line_end",
    [DICELANG_TOKEN_file_end]           = "file_end",
    [DICELANG_TOKEN_identifier]         = "identifier",
    [DICELANG_TOKEN_value]              = "value",
    [DICELANG_TOKEN_value_real]         = "value_real",
    [DICELANG_TOKEN_separator]          = "separator",
    [DICELANG_TOKEN_addition]           = "addition",
    [DICELANG_TOKEN_substraction]       = "substraction",
    [DICELANG_TOKEN_d]                  = "d",
    [DICELANG_TOKEN_designator]         = "designator",
    [DICELANG_TOKEN_open_parenthesis]   = "open parenthesis",
    [DICELANG_TOKEN_close_parenthesis]  = "close parenthesis",
    [DICELANG_TOKEN_open_bracket]       = "open bracket",
    [DICELANG_TOKEN_close_bracket]      = "close bracket",
    [DICELANG_TOKEN_open_sq_bracket]    = "open square bracket",
    [DICELANG_TOKEN_close_sq_bracket]   = "close square bracket",
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void dicelang_print_token(struct dicelang_token token, FILE *to_file);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param source_code
 * @param alloc
 * @return RANGE_TOKEN*
 */
RANGE_TOKEN *dicelang_tokenize(const char *source_code, struct allocator alloc)
{
    RANGE_TOKEN *read_tokens = nullptr;

    if (!source_code) {
        return nullptr;
    }

    read_tokens = range_create_dynamic(alloc, sizeof(*read_tokens->data), 16u);



    return read_tokens;
}

/**
 * @brief
 *
 * @param tokens
 * @param to_file
 */
void dicelang_token_dump(RANGE_TOKEN *tokens, FILE *to_file)
{
    if (!tokens || !tokens->data) {
        return;
    }

    for (size_t i = 0u ; i < tokens->length ; i++) {
        dicelang_print_token(tokens->data[i], to_file);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void dicelang_print_token(struct dicelang_token token, FILE *to_file)
{
    if (!to_file || (token.flavour >= DICELANG_TOKEN_NUMBER)) {
        return;
    }

    fprintf(to_file, "(%d:%d) %s\t", token.where.line, token.where.col, token_names[token.flavour]);
    for (size_t i = 0u ; i < token.value.source_length ; i++) {
        fprintf(to_file, "%c", token.value.source[i]);
    }
    fprintf(to_file, "\n");
}

