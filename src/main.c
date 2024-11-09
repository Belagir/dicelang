
#include <lexer.h>
#include <stdio.h>

static const char test_code[] =
        "R : 4\n"
        "R : 1d3\n"
        "R : 4d6 + 1 - d20\n"
        "R : [1, 2, 2, 3, 4, 10]\n"
        "R : 2d20 + [3,4]\n"
        "R : 2d20 * 1d6\n"
        "R : (3d6 + 1.5) / 1d4\n";

int main(int argc, const char *argv[])
{
    RANGE_TOKEN *tokens = dicelang_tokenize(test_code, make_system_allocator());
    dicelang_token_dump(tokens, stdout);


    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(tokens));

    return 0;
}
