
#include <dicelang.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct dicelang_entry { f32 val, weight; };
struct dicelang_distrib { RANGE(struct dicelang_entry) *values; RANGE(const char *) *formula; };
struct dicelang_variable { u32 hash; struct dicelang_distrib distr; };

struct dicelang_exec_context {
    struct dicelang_parse_node *node;
    size_t values_stack_index;
};

struct dicelang_interpreter {
    RANGE(struct dicelang_variable) *vars_hashmap;

    RANGE(struct dicelang_distrib) *values_stack;
    RANGE(struct dicelang_exec_context) *exec_stack;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static struct dicelang_interpreter dicelang_interpreter_create(size_t start_stack_size, size_t start_hashmap_size, struct allocator alloc);
static void dicelang_interpreter_destroy(struct dicelang_interpreter *interp, struct allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void dicelang_interpret(struct dicelang_parse_node *tree, struct dicelang_error *error_sink, struct allocator alloc)
{
    struct dicelang_interpreter interpreter = { };

    if (!tree) {
        error_sink->flavour = DERR_INTERNAL;
        error_sink->what = "Tried to interpret a null-ed parse tree.";
    }

    interpreter = dicelang_interpreter_create(16, 8, alloc);




    dicelang_interpreter_destroy(&interpreter, alloc);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
static struct dicelang_interpreter dicelang_interpreter_create(size_t start_stack_size, size_t start_hashmap_size, struct allocator alloc)
{
    struct dicelang_interpreter interp = {
            .vars_hashmap = range_create_dynamic(alloc, sizeof(*interp.vars_hashmap->data), start_hashmap_size),

            .values_stack = range_create_dynamic(alloc, sizeof(*interp.values_stack->data), start_stack_size),
            .exec_stack = range_create_dynamic(alloc, sizeof(*interp.exec_stack->data), start_stack_size),
    };

    return interp;
}

/**
 * @brief
 *
 * @param interp
 */
static void dicelang_interpreter_destroy(struct dicelang_interpreter *interp, struct allocator alloc)
{
    if (!interp) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(interp->vars_hashmap));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(interp->values_stack));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(interp->exec_stack));

    *interp = (struct dicelang_interpreter) { 0 };
}
