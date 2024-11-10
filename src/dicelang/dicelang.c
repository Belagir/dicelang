
#include <dicelang.h>

#include <lexer.h>

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

    if (!from_file) {
        return (struct dicelang_program) { 0u };
    }

    new_program.text = range_create_dynamic(alloc, sizeof(*new_program.text->data), 512);
    while ((read_char = fgetc(from_file)) != EOF) {
        new_program.text = range_ensure_capacity(alloc, RANGE_TO_ANY(new_program.text), 1);
        range_push(RANGE_TO_ANY(new_program.text), &read_char);
    } ;
    new_program.text = range_ensure_capacity(alloc, RANGE_TO_ANY(new_program.text), 1);
    range_push(RANGE_TO_ANY(new_program.text), &(char) { '\0' });

    new_program.tokens = dicelang_tokenize(new_program.text->data, alloc);

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
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(program->tokens));

    *program = (struct dicelang_program) { 0u };
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
