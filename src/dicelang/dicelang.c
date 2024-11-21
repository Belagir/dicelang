
#include <dicelang.h>

#include <lexer.h>
#include <parser.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Maps token flavours to some text representing each of them.
 */
const char *DTOK_DSTX_names[DSTX_NUMBER] = {
        [DTOK_invalid]            = "invalid",
        [DTOK_empty]              = "empty",

        [DTOK_line_end]           = "line_end",
        [DTOK_file_end]           = "file_end",
        [DTOK_identifier]         = "identifier",
        [DTOK_value]              = "value",
        [DTOK_value_real]         = "value_real",
        [DTOK_separator]          = "separator",
        [DTOK_addition]           = "addition",
        [DTOK_substraction]       = "substraction",
        [DTOK_d]                  = "_d",
        [DTOK_designator]         = "designator",
        [DTOK_open_parenthesis]   = "open parenthesis",
        [DTOK_close_parenthesis]  = "close parenthesis",
        [DTOK_open_bracket]       = "open bracket",
        [DTOK_close_bracket]      = "close bracket",
        [DTOK_open_sq_bracket]    = "open square bracket",
        [DTOK_close_sq_bracket]   = "close square bracket",

        [DSTX_program]            = "program",
        [DSTX_assignment]         = "assignment",
        [DSTX_variable]           = "variable",
        [DSTX_expression]         = "expression",
        [DSTX_dice]               = "dice",
        [DSTX_factor]             = "factor",
        [DSTX_operand]            = "operand",
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param from_file
 * @param alloc
 * @return struct dicelang_program
 */
struct dicelang_program dicelang_program_create_from_file(FILE *from_file, allocator alloc)
{
    struct dicelang_program new_program = { 0u };
    char read_char = 0;
    RANGE_TOKEN *tokens = nullptr;

    if (!from_file) {
        return (struct dicelang_program) { 0u };
    }

    // reading from file
    new_program.text = range_create_dynamic(alloc, sizeof(*new_program.text->data), 512);
    while ((read_char = fgetc(from_file)) != EOF) {
        new_program.text = range_ensure_capacity(alloc, RANGE_TO_ANY(new_program.text), 1);
        range_push(RANGE_TO_ANY(new_program.text), &read_char);
    } ;

    // adding terminator
    new_program.text = range_ensure_capacity(alloc, RANGE_TO_ANY(new_program.text), 1);
    range_push(RANGE_TO_ANY(new_program.text), &(char) { '\0' });

    // tokenizing & creating parse tree
    tokens = dicelang_tokenize(new_program.text->data, alloc);
    new_program.parse_tree = dicelang_parse(tokens, alloc);
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(tokens));

    dicelang_parse_node_print(new_program.parse_tree, stdout);

    return new_program;
}

/**
 * @brief
 *
 * @param program
 * @param alloc
 */
void dicelang_program_destroy(struct dicelang_program *program, allocator alloc)
{
    if (!program) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(program->text));
    dicelang_parse_node_destroy(&program->parse_tree, alloc);

    *program = (struct dicelang_program) { 0u };
}

/**
 * @brief Prints one token to a file;
 *
 * @param[in] token
 * @param[in] to_file
 */
void dicelang_token_print(struct dicelang_token token, FILE *to_file)
{
    if (!to_file || (token.flavour >= DSTX_NUMBER)) {
        return;
    }

    fprintf(to_file, "(%d:%d)\t%c%- 20s`",
            token.where.line,
            token.where.col,
            (token.flavour < DTOK_NUMBER)? '*' : ' ',
            DTOK_DSTX_names[token.flavour]);

    for (size_t i = 0u ; i < token.value.source_length ; i++) {
        if (token.value.source[i] != '\n') {
            fprintf(to_file, "%c", token.value.source[i]);
        }
    }

    fprintf(to_file, "`\n");
}
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
