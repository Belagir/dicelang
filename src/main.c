
#include <lexer.h>
#include <stdio.h>

static const char test_code[] =
        "R\n"
        "VARIABLE\n"
        "W42\n"
        "SOME_VAR_78 SOME OTHER VARS\n"
        "\n"
        "X\n"
        "42\n"
        "Y";

int main(int argc, const char *argv[])
{
    RANGE_TOKEN *tokens = dicelang_tokenize(test_code, make_system_allocator());
    dicelang_token_dump(tokens, stdout);

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(tokens));

    return 0;
}
