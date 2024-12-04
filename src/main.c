
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <dicelang.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING
#include "dicelang/containers/distribution.h"
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------


// General usage helper.
static void print_usage(const char *prog_name, FILE *stream);
// File reading failure helper.
static void print_failed_fileread(const char *file_name, FILE *stream);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// MAIN BEGINS HERE
int main(int argc, const char *argv[])
{
    FILE *f = nullptr;

#ifdef UNITTESTING
    dicelang_distrib_test();
    return 0;
#endif

    if (argc != 2) {
        print_usage(argv[0], stderr);
        return -1;
    }

    f = fopen(argv[1], "r");

    if (!f) {
        print_failed_fileread(argv[1], stderr);
        return -2;
    }


    struct dicelang_program program = dicelang_program_create_from_file(f, make_system_allocator());
    fclose(f);

    dicelang_interpret(program.parse_tree, &program.error, make_system_allocator());
    dicelang_error_print(program.error, stderr);

    dicelang_program_destroy(&program, make_system_allocator());

    return 0;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Prints the usage of the program to some file.
 *
 * @param[in] prog_name
 * @param[in] stream
 */
static void print_usage(const char *prog_name, FILE *stream)
{
    if (!prog_name || !stream) {
        return;
    }

    fprintf(stream, "I need a script to work ! Usage :\n\t$%s FILE\n\n", prog_name);
    fprintf(stream, "with FILE being a dicelang script.\n");
}

/**
 * @brief Prints the file reading error to some file.
 *
 * @param[in] file_name
 * @param[in] stream
 */
static void print_failed_fileread(const char *file_name, FILE *stream)
{
    if (!file_name || !stream) {
        return;
    }

    fprintf(stream, "Failed to open file \"%s\" : %s\n", file_name, strerror(errno));
}