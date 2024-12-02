
#include "containers/distribution.h"
#include "containers/var_hashmap.h"
#include "containers/func_hashmap.h"

#include <dicelang.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct dicelang_exec_context {
    struct dicelang_parse_node *node;

    size_t children_index;
    size_t values_stack_index;
};

/**
 * @brief
 *
 */
struct dicelang_interpreter {
    struct allocator alloc;

    struct dicelang_variable_map variables;
    struct dicelang_function_map functions;

    RANGE(struct dicelang_distrib) *values_stack;
    RANGE(struct dicelang_exec_context) *exec_stack;
};

/**
 * @brief
 *
 */
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
static void dicelang_exec_routine_function_call(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);
static void dicelang_exec_routine_variable_access(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context);

// -------------------------------------------------------------------------------------------------

static void dicelang_builtin_print(struct dicelang_distrib *input, struct dicelang_distrib **output, struct allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
static const dicelang_exec_routine dicelang_exec_routine_map[DSTX_NUMBER] = {
        [DTOK_value]      = &dicelang_exec_routine_value,

        [DSTX_assignment]       = &dicelang_exec_routine_assignment,
        [DSTX_addition]         = &dicelang_exec_routine_addition,
        [DSTX_dice]             = &dicelang_exec_routine_dice,
        [DSTX_multiplication]   = &dicelang_exec_routine_multiplication,
        [DSTX_function_call]    = &dicelang_exec_routine_function_call,
        [DSTX_variable_access]  = &dicelang_exec_routine_variable_access,
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

    (void) error_sink;

    interpreter = dicelang_interpreter_create(16, 8, alloc);

    current_context = dicelang_interpreter_push_context(&interpreter, tree, alloc);
    dicelang_function_map_set(&interpreter.functions, "print", 5, &dicelang_builtin_print, 1, alloc);

    if (!current_context) {
        error_sink->flavour = DERR_INTERNAL;
        error_sink->what = "interpreter could not init a context to interpret from.";
        return;
    }

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
            .alloc = alloc,

            .variables = dicelang_variable_map_create(start_hashmap_size, alloc),
            .functions = dicelang_function_map_create(start_hashmap_size, alloc),

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
 * @param alloc
 */
static void dicelang_interpreter_destroy(struct dicelang_interpreter *interp, struct allocator alloc)
{
    if (!interp) {
        return;
    }

    dicelang_variable_map_destroy(&interp->variables, alloc);
    dicelang_function_map_destroy(&interp->functions, alloc);

    for (size_t i = 0 ; i < interp->values_stack->length ; i++) {
        dicelang_distrib_destroy(interp->values_stack->data + i, alloc);
    }

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

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param interpreter
 * @param context
 */
static void dicelang_exec_routine_value(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
    struct dicelang_distrib new_distrib = dicelang_distrib_create(context->node->token, interpreter->alloc);

    if (!new_distrib.values) {
        return;
    }

    interpreter->values_stack = range_ensure_capacity(interpreter->alloc, RANGE_TO_ANY(interpreter->values_stack), 1);
    range_push(RANGE_TO_ANY(interpreter->values_stack), &new_distrib);
}

/**
 * @brief
 *
 * @param interpreter
 * @param context
 */
static void dicelang_exec_routine_assignment(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
    struct dicelang_token indentifier = { };
    struct dicelang_distrib val = { };

    if ((context->node->children->length == 0) || ((context->values_stack_index + 1) != interpreter->values_stack->length)) {
        return;
    }

    indentifier = context->node->children->data[0]->token;
    val = RANGE_LAST(interpreter->values_stack);

    if (dicelang_variable_map_set(&interpreter->variables, indentifier.value.source, indentifier.value.source_length, &val, interpreter->alloc)) {
        range_pop(RANGE_TO_ANY(interpreter->values_stack));
        return;
    }

    dicelang_distrib_destroy(&RANGE_LAST(interpreter->values_stack), interpreter->alloc);
    range_pop(RANGE_TO_ANY(interpreter->values_stack));
}

/**
 * @brief
 *
 * @param interpreter
 * @param context
 */
static void dicelang_exec_routine_addition(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
    struct dicelang_distrib tmp_distrib = { };

    while (context->values_stack_index + 1 < interpreter->values_stack->length) {
        tmp_distrib = dicelang_distrib_add(RANGE_LAST(interpreter->values_stack), RANGE_LAST(interpreter->values_stack, -1), interpreter->alloc);

        dicelang_distrib_destroy(&RANGE_LAST(interpreter->values_stack), interpreter->alloc);
        range_pop(RANGE_TO_ANY(interpreter->values_stack));
        dicelang_distrib_destroy(&RANGE_LAST(interpreter->values_stack), interpreter->alloc);
        range_pop(RANGE_TO_ANY(interpreter->values_stack));

        range_push(RANGE_TO_ANY(interpreter->values_stack), &tmp_distrib);
    }
}

/**
 * @brief
 *
 */
static void dicelang_exec_routine_dice(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
    struct dicelang_distrib tmp_distrib = { };

    if (context->values_stack_index + 1 == interpreter->values_stack->length) {
        tmp_distrib = dicelang_distrib_dice(RANGE_LAST(interpreter->values_stack), interpreter->alloc);

        dicelang_distrib_destroy(&RANGE_LAST(interpreter->values_stack), interpreter->alloc);
        range_pop(RANGE_TO_ANY(interpreter->values_stack));

        range_push(RANGE_TO_ANY(interpreter->values_stack), &tmp_distrib);
    }
}

/**
 * @brief
 *
 */
static void dicelang_exec_routine_multiplication(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
    struct dicelang_distrib tmp_distrib = { };

    while (context->values_stack_index + 1 < interpreter->values_stack->length) {
        tmp_distrib = dicelang_distrib_multiply(RANGE_LAST(interpreter->values_stack), RANGE_LAST(interpreter->values_stack, -1), interpreter->alloc);

        dicelang_distrib_destroy(&RANGE_LAST(interpreter->values_stack), interpreter->alloc);
        range_pop(RANGE_TO_ANY(interpreter->values_stack));
        dicelang_distrib_destroy(&RANGE_LAST(interpreter->values_stack), interpreter->alloc);
        range_pop(RANGE_TO_ANY(interpreter->values_stack));

        range_push(RANGE_TO_ANY(interpreter->values_stack), &tmp_distrib);
    }
}

/**
 * @brief
 *
 */
static void dicelang_exec_routine_function_call(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
    struct dicelang_token indentifier = { };
    struct dicelang_function called = { };

    if (context->node->children->length == 0) {
        return;
    }

    indentifier = context->node->children->data[0]->token;

    if (dicelang_function_map_get(interpreter->functions, indentifier.value.source, indentifier.value.source_length, &called)) {

        if (called.nb_args != (interpreter->values_stack->length - context->values_stack_index)) {
            return;
        }

        called.func_impl(interpreter->values_stack->data + context->values_stack_index, NULL, interpreter->alloc);
    }
}

/**
 * @brief
 *
 */
static void dicelang_exec_routine_variable_access(struct dicelang_interpreter *interpreter, struct dicelang_exec_context *context)
{
    (void) interpreter;

    struct dicelang_token indentifier = { };
    struct dicelang_distrib var_value = { };

    if (context->node->children->length == 0) {
        return;
    }

    indentifier = context->node->children->data[0]->token;

    if (dicelang_variable_map_get(interpreter->variables, indentifier.value.source, indentifier.value.source_length, &var_value, interpreter->alloc)) {
        range_push(RANGE_TO_ANY(interpreter->values_stack), &var_value);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void dicelang_builtin_print(struct dicelang_distrib *input, struct dicelang_distrib **output, struct allocator alloc)
{
    (void) output;
    (void) alloc;

    printf("%ld ---\n", input->values->length);
    for (size_t i = 0 ; i < input->values->length ; i++) {
        printf("%d\t%d\n", input->values->data[i].val, input->values->data[i].count);
    }
}
