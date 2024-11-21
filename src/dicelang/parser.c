/**
 * @file parser.c
 * @author gabriel
 * @brief Parser implementation file. Builds a parser tree from tokens so they can be interpreted.
 * @version 0.1
 * @date 2024-11-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <ustd/range.h>

#include <dicelang.h>
#include <lexer.h>
#include <parser.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static struct dicelang_parse_node *dicelang_parse_node_create(struct dicelang_token token, struct dicelang_parse_node *parent, struct allocator alloc);

// -------------------------------------------------------------------------------------------------

static bool expect(RANGE_TOKEN *tokens, enum dicelang_token_flavour what, struct dicelang_parse_node *parent, struct allocator alloc);
static bool accept(RANGE_TOKEN *tokens, enum dicelang_token_flavour what, struct dicelang_parse_node *parent, struct allocator alloc);
static bool next_is(const RANGE_TOKEN *tokens, enum dicelang_token_flavour what);

// -------------------------------------------------------------------------------------------------

static void assignment(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc);
static void variable  (RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc);
static void expression(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc);
static void dice      (RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc);
static void factor    (RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc);
static void operand   (RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Constructs a tree of syntax nodes by consuming a set of tokens, and returns its root node.
 *
 * @param[inout] tokens
 * @param[in] alloc
 *
 * @return struct dicelang_parse_node *
 */
struct dicelang_parse_node *dicelang_parse(RANGE_TOKEN *tokens, struct allocator alloc)
{
    if (!tokens || !tokens->data) {
        return nullptr;
    }

    struct dicelang_parse_node *program = dicelang_parse_node_create((struct dicelang_token) { .flavour = DSTX_program }, nullptr, alloc);

    accept(tokens, DTOK_line_end, program, alloc);
    assignment(tokens, program, alloc);
    while (accept(tokens, DTOK_line_end, program, alloc)) {
        if (!accept(tokens, DTOK_file_end, program, alloc)) {
            assignment(tokens, program, alloc);
        }
    }

    return program;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Recursively prints a parse tree to some file.
 *
 * @param[in] node
 * @param[in] to_file
 */
void dicelang_parse_node_print(const struct dicelang_parse_node *node, FILE *to_file)
{
    if (!node) {
        return;
    }

    dicelang_token_print(node->token, to_file);

    for (size_t i = 0 ; i < node->children->length ; i++) {
        dicelang_parse_node_print(node->children->data[i], to_file);
    }
}

/**
 * @brief Recursively destroys a parse tree, releasing the memory used by a node and all its children.
 *
 * @param[inout] node
 * @param[in] alloc
 */
void dicelang_parse_node_destroy(struct dicelang_parse_node **node, struct allocator alloc)
{
    if (!node || !*node) {
        return;
    }

    for (size_t i = 0 ; i < (*node)->children->length ; i++) {
        dicelang_parse_node_destroy((*node)->children->data + i, alloc);
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*node)->children));
    alloc.free(alloc, *node);

    *node = nullptr;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a new syntax node representing a token.
 * It may be linked to a parent node, in which case this parent's node will have its children collection modified, and maybe reallocated.
 *
 * @param[in] token
 * @param[inout] parent
 * @param[in] alloc
 * @return struct dicelang_parse_node*
 */
static struct dicelang_parse_node *dicelang_parse_node_create(struct dicelang_token token, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *new_node = alloc.malloc(alloc, sizeof(*new_node));

    if (!new_node) {
        return nullptr;
    }

    *new_node = (struct dicelang_parse_node) {
            .token = token,

            .parent = parent,
            .children = range_create_dynamic(alloc, sizeof(*new_node->children->data), 8),
    };

    if (parent) {
        parent->children = range_ensure_capacity(alloc, RANGE_TO_ANY(parent->children), 1);
        range_push(RANGE_TO_ANY(parent->children), &new_node);
    }

    return new_node;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Requires that the leading token in the set is of some flavour. If not, a syntax error is generated.
 * On success, this will consume the leading token and add a new child node under the supplied parent.
 * In this case, if the parent is null, no node will be created but the leading token will still be removed.
 *
 * @param[inout] tokens
 * @param[in] what
 * @param[in] parent
 * @param[in] alloc
 * @return true
 * @return false
 */
static bool expect(RANGE_TOKEN *tokens, enum dicelang_token_flavour what, struct dicelang_parse_node *parent, struct allocator alloc)
{
    if (accept(tokens, what, parent, alloc)) {
        return true;
    }

    printf("syntax error near %s (%d:%d) : expected %s\n",
            DTOK_DSTX_names[tokens->data[0].flavour],
            tokens->data[0].where.line,
            tokens->data[0].where.col,
            DTOK_DSTX_names[what]);
    return false;
}

/**
 * @brief Tries to match the leading token against some flavour, without creating an error on failure.
 * On success, this will consume the leading token and add a new child node under the supplied parent.
 * In this case, if the parent is null, no node will be created but the leading token will still be removed.
 *
 * @param[inout] tokens
 * @param[in] what
 * @param[in] parent
 * @param[in] alloc
 * @return true
 * @return false
 */
static bool accept(RANGE_TOKEN *tokens, enum dicelang_token_flavour what, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *new_node = nullptr;

    if (parent && next_is(tokens, what)) {
        if (parent) new_node = dicelang_parse_node_create(tokens->data[0], parent, alloc);
        return range_remove(RANGE_TO_ANY(tokens), 0);
    }
    return false;
}

/**
 * @brief Peeks at the leading token, returning true if it matches some flavour without consuming it.
 *
 * @param[in] tokens
 * @param[in] what
 * @return true
 * @return false
 */
static bool next_is(const RANGE_TOKEN *tokens, enum dicelang_token_flavour what)
{
    if (!tokens || !tokens->data || (tokens->length == 0)) {
        return false;
    }

    return tokens->data[0].flavour == what;
}


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
static void assignment(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *assignment_node = dicelang_parse_node_create(
            (struct dicelang_token) { .flavour = DSTX_assignment, }, parent, alloc);

    variable(tokens, assignment_node, alloc);
    expect(tokens, DTOK_designator, assignment_node, alloc);
    expression(tokens, assignment_node, alloc);
}

/**
 * @brief
 *
 */
static void variable(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *variable_node = dicelang_parse_node_create(
            (struct dicelang_token) { .flavour = DSTX_variable, }, parent, alloc);

    expect(tokens, DTOK_identifier, variable_node, alloc);
}

/**
 * @brief
 *
 */
static void expression(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *expression_node = dicelang_parse_node_create(
            (struct dicelang_token) { .flavour = DSTX_expression, }, parent, alloc);

    dice(tokens, expression_node, alloc);
    while (accept(tokens, DTOK_addition, expression_node, alloc) || accept(tokens, DTOK_substraction, expression_node, alloc)) {
        dice(tokens, expression_node, alloc);
    }
}

/**
 * @brief
 *
 */
static void dice(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *dice_node = dicelang_parse_node_create(
            (struct dicelang_token) { .flavour = DSTX_dice, }, parent, alloc);

    factor(tokens, dice_node, alloc);
    while (accept(tokens, DTOK_d, dice_node, alloc)) {
        factor(tokens, dice_node, alloc);
    }
}

/**
 * @brief
 *
 */
static void factor(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *factor_node = dicelang_parse_node_create(
            (struct dicelang_token) { .flavour = DSTX_factor, }, parent, alloc);

    operand(tokens, factor_node, alloc);
}

/**
 * @brief
 *
 */
static void operand(RANGE_TOKEN *tokens, struct dicelang_parse_node *parent, struct allocator alloc)
{
    struct dicelang_parse_node *operand_node = dicelang_parse_node_create(
            (struct dicelang_token) { .flavour = DSTX_operand, }, parent, alloc);

    if (accept(tokens, DTOK_open_parenthesis, operand_node, alloc)) {
        expression(tokens, operand_node, alloc);
        expect(tokens, DTOK_close_parenthesis, operand_node, alloc);
    } else {
        expect(tokens, DTOK_value, operand_node, alloc);
    }
}
