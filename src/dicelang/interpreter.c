
#include "maths/distribution.h"

#include <dicelang.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct dicelang_variable { u32 hash; struct dicelang_distrib distr; };

struct dicelang_exec_context {
    struct dicelang_parse_node *node;

    size_t children_index;
    size_t values_stack_index;
};

struct dicelang_interpreter {
    RANGE(struct dicelang_variable) *vars_hashmap;

    RANGE(struct dicelang_distrib) *values_stack;
    RANGE(struct dicelang_exec_context) *exec_stack;
};

typedef void (*dicelang_exec_routine)(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static struct dicelang_interpreter dicelang_interpreter_create(size_t start_stack_size, size_t start_hashmap_size, struct allocator alloc);
static void dicelang_interpreter_destroy(struct dicelang_interpreter *interp, struct allocator alloc);
static struct dicelang_exec_context *dicelang_interpreter_push_context(struct dicelang_interpreter *interp, struct dicelang_parse_node *node, struct allocator alloc);
static struct dicelang_exec_context *dicelang_interpreter_pop_context(struct dicelang_interpreter interp);

// -------------------------------------------------------------------------------------------------

static void dicelang_exec_routine_value(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);
static void dicelang_exec_routine_assignment(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);
static void dicelang_exec_routine_addition(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);
static void dicelang_exec_routine_dice(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);
static void dicelang_exec_routine_multiplication(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static const dicelang_exec_routine dicelang_exec_routine_map[DSTX_NUMBER] = {
        [DTOK_value]      = &dicelang_exec_routine_value,

        [DSTX_assignment] = &dicelang_exec_routine_assignment,
        [DSTX_addition] = &dicelang_exec_routine_addition,
        [DSTX_dice]       = &dicelang_exec_routine_dice,
        [DSTX_multiplication]     = &dicelang_exec_routine_multiplication,
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param tree
 * @param error_sink
 * @param alloc
 */
void dicelang_interpret(struct dicelang_parse_node *tree, struct dicelang_error *error_sink, struct allocator alloc)
{
    struct dicelang_interpreter interpreter = { };
    struct dicelang_exec_context *current_context = NULL;
    struct dicelang_exec_context *next_context = NULL;
    bool executing = 0;

    interpreter = dicelang_interpreter_create(16, 8, alloc);
    current_context = dicelang_interpreter_push_context(&interpreter, tree, alloc);

    executing = 1;
    do {
        if (current_context->children_index < current_context->node->children->length) {
            next_context = dicelang_interpreter_push_context(&interpreter, current_context->node->children->data[current_context->children_index], alloc);
            current_context->children_index += 1;
            current_context = next_context;
        } else {
            if (dicelang_exec_routine_map[current_context->node->token.flavour]) {
                dicelang_exec_routine_map[current_context->node->token.flavour](&interpreter, current_context);
            }

            current_context = dicelang_interpreter_pop_context(interpreter);
        }

        executing = current_context != NULL;
    } while (executing);


    dicelang_interpreter_destroy(&interpreter, alloc);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param start_stack_size
 * @param start_hashmap_size
 * @param alloc
 * @return struct dicelang_interpreter
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
 * @param alloc
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

/**
 * @brief
 *
 * @param interp
 * @param node
 * @param alloc
 */
static struct dicelang_exec_context *dicelang_interpreter_push_context(struct dicelang_interpreter *interp, struct dicelang_parse_node *node, struct allocator alloc)
{
    if (!node || !interp) {
        return NULL;
    }

    interp->exec_stack = range_ensure_capacity(alloc, RANGE_TO_ANY(interp->exec_stack), 1);
    range_push(RANGE_TO_ANY(interp->exec_stack), &(struct dicelang_exec_context) { .node = node, .children_index = 0, .values_stack_index = interp->values_stack->length });

    return interp->exec_stack->data + (interp->exec_stack->length - 1);
}

/**
 * @brief
 *
 * @param interp
 * @return struct dicelang_exec_context*
 */
static struct dicelang_exec_context *dicelang_interpreter_pop_context(struct dicelang_interpreter interp)
{
    range_pop(RANGE_TO_ANY(interp.exec_stack));

    if (interp.exec_stack->length == 0) {
        return NULL;
    }
    return interp.exec_stack->data + interp.exec_stack->length - 1;
}

/**
 * @brief
 *
 * @param interpreter
 * @param context
 */
static void dicelang_exec_routine_value(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
}

static void dicelang_exec_routine_assignment(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
}

static void dicelang_exec_routine_addition(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
}

static void dicelang_exec_routine_dice(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
}

static void dicelang_exec_routine_multiplication(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
}
