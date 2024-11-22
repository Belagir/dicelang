
#include <dicelang.h>

struct dicelang_entry { f32 val, weight; };
struct dicelang_distrib { RANGE(struct dicelang_entry) *values; RANGE(const char *) *formula; };
struct dicelang_variable { u32 hash; struct dicelang_distrib distr; };

struct dicelang_interpreter {
    RANGE(struct dicelang_variable) *vars;
    struct dicelang_distrib live_expression;
};

void dicelang_interpret(struct dicelang_parse_node *tree, struct dicelang_error *error_sink, struct allocator alloc)
{
    if (!tree) {
        error_sink->flavour = DERR_INTERNAL;
        error_sink->what = "Tried to interpret a null-ed parse tree.";
    }

    dicelang_parse_node_print(tree, stdout);
}
