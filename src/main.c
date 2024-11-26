
#include <stdio.h>

#include <dicelang.h>
#include "dicelang/maths/distribution.h"

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    dicelang_distrib_test();

    FILE *f = fopen("test_script.dicescript", "r");
    struct dicelang_program program = dicelang_program_create_from_file(f, make_system_allocator());
    fclose(f);

    dicelang_interpret(program.parse_tree, &program.error, make_system_allocator());

    dicelang_error_print(program.error, stdout);

    dicelang_program_destroy(&program, make_system_allocator());

    return 0;
}
