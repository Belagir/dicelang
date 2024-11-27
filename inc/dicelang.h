/**
 * @file dicelang.h
 * @author gabriel
 * @brief Main dicelang interpretation private header.
 * @version 0.1
 * @date 2024-11-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __DICELANG_H__
#define __DICELANG_H__

#include <stdio.h>

#include <ustd/range.h>

/**
 * @brief Possible tokens that exist in the scripting language.
 *
 * @see parser.h tolookup what makes an "addition", "statement", "variable", "function", "mutator", or "array".
 */
enum dicelang_token_flavour {
    DTOK_invalid = 0,           ///< Lexer error token to indicate some unrecognized base syntax.
    DTOK_empty,                 ///< Token not containing anything, used as a base for token lexing.

    DTOK_line_end,              ///< Statements separator.
    DTOK_file_end,              ///< Script ender token.
    DTOK_identifier,            ///< User-written symbol, such as a variable or a built-in function.
    DTOK_value,                 ///< Integer constant.
    DTOK_value_real,            ///< Real constant.
    DTOK_separator,             ///< Argument or array elements separator.
    DTOK_op_addition,              ///< Addition binary operand.
    DTOK_op_substraction,          ///< Substraction binary operand.
    DTOK_op_multiplication,        ///< Substraction binary operand.
    DTOK_op_division,              ///< Substraction binary operand.
    DTOK_op_d,                     ///< Dice distribution binary operand.
    DTOK_designator,            ///< Assignment token to link an expression to a variable.
    DTOK_open_parenthesis,      ///< Token to start either a function call's argument list or to start isolating part of an expression.
    DTOK_close_parenthesis,     ///< Token to finish either a function call's argument list or to finish isolating part of an expression.
    DTOK_open_bracket,          ///< Token to start a mutator call's argument list.
    DTOK_close_bracket,         ///< Token to finish a mutator call's argument list.
    DTOK_open_sq_bracket,       ///< Token to start either an array access or to start an array declaration.
    DTOK_close_sq_bracket,      ///< Token to finish either an array access or to finish an array declaration.

    DSTX_program,
    DSTX_statement,
    DSTX_function_call,
    DSTX_assignment,
    DSTX_variable_access,
    DSTX_addition,
    DSTX_dice,
    DSTX_multiplication,
    DSTX_operand,
    DSTX_expression_set,

    DSTX_NUMBER,                ///< Meta enum member to have a count the number of other members.
};

#define DTOK_NUMBER (DSTX_program)

/**
 * @brief
 *
 */
enum dicelang_error_flavour {
    DERR_NONE,

    DERR_INTERNAL,

    DERR_TOKEN,
    DERR_SYNTAX,
    DERR_INTERPRET,
};

/**
 * @brief Dicelang token, dependent on some source code.
 */
struct dicelang_token {
    /** Token nature. */
    enum dicelang_token_flavour flavour;

    /** Value information, references the source code. */
    struct { const char *source; size_t source_length; } value;
    /** Position information of the token in the source code file. */
    struct { u32 line, col; } where;
};

/** Further range definition specificaly t store tokens. Defined so the compiler knows what it is working with. */
typedef RANGE(struct dicelang_token) RANGE_TOKEN;

/**
 * @brief Node of a parse tree.
 * Contains a syntax node linked to a set of children and a parent ;
 * if the syntax is a terminal one (a token), then the node shouldn't have children.
 */
struct dicelang_parse_node {
    struct dicelang_token token;

    struct dicelang_parse_node *parent;
    RANGE(struct dicelang_parse_node *) *children;
};

/**
 * @brief
 *
 */
struct dicelang_error {
    enum dicelang_error_flavour flavour;

    struct dicelang_token token;
    const char *what;
};

/**
 * @brief Contains all the information needed to represent & interpet a dicelang program.
 *
 */
struct dicelang_program {
    /** Text from the read file. */
    RANGE(const char) *text;
    /** Parse tree generated from the text. */
    struct dicelang_parse_node *parse_tree;

    /** Current error. Set by functions lexing, parsing and interpreting the script. */
    struct dicelang_error error;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// Array of static strings indexed to the syntax and token flavours, giving each of their names.
extern const char *DTOK_DSTX_names[];

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// Load a program from a file.
struct dicelang_program dicelang_program_create_from_file(FILE *from_file, allocator alloc);
// Releases memory taken by a loaded program.
void dicelang_program_destroy(struct dicelang_program *program, allocator alloc);
// Prints the curretn error to some file.
void dicelang_error_print(struct dicelang_error err, FILE *to_file);

// Creates a set of tokens representing the given source code.
RANGE_TOKEN *dicelang_tokenize(const char *source_code, struct dicelang_error *error_sink, struct allocator alloc);
// Prints debug token information to some file.
void dicelang_token_dump(RANGE_TOKEN *tokens, FILE *to_file);
// Prints one token to a file.
void dicelang_token_print(struct dicelang_token token, FILE *to_file);

//
struct dicelang_parse_node *dicelang_parse(RANGE_TOKEN *tokens, struct dicelang_error *error_sink, allocator alloc);
//
void dicelang_parse_node_dump(const struct dicelang_parse_node *node, FILE *to_file);
//
void dicelang_parse_node_print(const struct dicelang_parse_node *node, FILE *to_file);
//
void dicelang_parse_node_destroy(struct dicelang_parse_node **node, struct allocator alloc);

//
void dicelang_interpret(struct dicelang_parse_node *tree, struct dicelang_error *error_sink, struct allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif