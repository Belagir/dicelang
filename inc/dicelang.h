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
 * Terminal tokens are prefixed as "DTOK_" (for dicelang token) and nonterminals are prefixed as "DSTX_" (for dicelang syntax).
 */
enum dicelang_token_flavour {
    DTOK_invalid = 0,           ///< Lexer error token to indicate some unrecognized base syntax.
    DTOK_empty,                 ///< Token not containing anything, used as a base for token lexing.

    DTOK_line_end,              ///< Statements separator.
    DTOK_file_end,              ///< Script ender token.
    DTOK_identifier,            ///< User-written symbol, such as a variable or a built-in function.
    DTOK_value,                 ///< Integer constant.
    DTOK_separator,             ///< Argument or array elements separator.
    DTOK_op_addition,           ///< Addition binary operand.
    DTOK_op_substraction,       ///< Substraction binary operand.
    DTOK_op_multiplication,     ///< Substraction binary operand.
    DTOK_op_d,                  ///< Dice distribution binary operand.
    DTOK_designator,            ///< Assignment token to link an expression to a variable.
    DTOK_open_parenthesis,      ///< Token to start either a function call's argument list or to start isolating part of an expression.
    DTOK_close_parenthesis,     ///< Token to finish either a function call's argument list or to finish isolating part of an expression.
    DTOK_open_bracket,          ///< Token to start a mutator call's argument list.
    DTOK_close_bracket,         ///< Token to finish a mutator call's argument list.
    DTOK_open_sq_bracket,       ///< Token to start either an array access or to start an array declaration.
    DTOK_close_sq_bracket,      ///< Token to finish either an array access or to finish an array declaration.

    DSTX_program,               ///< Root of a program made of statements.
    DSTX_statement,             ///< Statement syntax, made of either a function call or an assignment.
    DSTX_function_call,         ///< Call to some identifier, with eventual arguments.
    DSTX_assignment,            ///< Variable declaration or modification.
    DSTX_variable_access,       ///< Variable reference.
    DSTX_addition,              ///< Sum of two expressions (addition or substraction)
    DSTX_dice,                  ///< Dice expression.
    DSTX_multiplication,        ///< Multiplication of two expressions.
    DSTX_operand,               ///< basic operand : a value, a variable name, a dice expression or an expression between parenthesis.
    DSTX_expression_set,        ///< Expressions separated by a specific character.

    DSTX_NUMBER,                ///< Meta enum member to have a count the number of other members.
};

#define DTOK_NUMBER (DSTX_program)      ///< Separation between terminal and nonterminal in the dicelang_token_flavour enum.

/**
 * @brief Kinds of error the system can recognize.
 */
enum dicelang_error_flavour {
    DERR_NONE,          ///< No error is reported.

    DERR_INTERNAL,      ///< Internal error that if happens, means an implementation/integration-related bug has occured !

    DERR_TOKEN,         ///< There was some problem with a syntax token.
    DERR_SYNTAX,        ///< There was some problem when composing a valid syntax with tokens.
    DERR_INTERPRET,     ///< There was some problem when interpreting some otherwise valid syntax.
};

/**
 * @brief Dicelang token, dependent on some text source code.
 * The characters pointed in the value::source field should have a longer or matching lifetime as tokens referencing them.
 */
struct dicelang_token {
    /** Token nature. */
    enum dicelang_token_flavour flavour;

    /** Value information, references the source code. */
    struct { const char *source; size_t source_length; } value;
    /** Position information of the token in the source code file. */
    struct { u32 line, col; } where;
};

/** Further range definition specificaly to store tokens. Defined so the compiler knows what it is working with. */
typedef RANGE(struct dicelang_token) RANGE_TOKEN;

/**
 * @brief Node of a parse tree.
 * Contains a syntax node linked to a set of children and a parent ;
 * if the syntax is a terminal one (a token), then the node shouldn't have children.
 */
struct dicelang_parse_node {
    /** Token (terminal or nonterminal) that generated the node, and leads how it will be interpreted. */
    struct dicelang_token token;

    /** Eventual parent node ; might be NULL. */
    struct dicelang_parse_node *parent;
    /** Eventual children nodes ; might be NULL, especially for terminal tokens. */
    RANGE(struct dicelang_parse_node *) *children;
};

/**
 * @brief Describes an error the system can report.
 */
struct dicelang_error {
    /** Kind of error reported. */
    enum dicelang_error_flavour flavour;

    /** Token associated to the error ; might be empty. */
    struct dicelang_token token;
    /** Optional description string ; might be NULL. Best associated to a static string. */
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

// Create a parse tree from an array of tokens.
struct dicelang_parse_node *dicelang_parse(RANGE_TOKEN *tokens, struct dicelang_error *error_sink, allocator alloc);
// Dumps the description of the whole tree of nodes to some file, depth-wise.
void dicelang_parse_node_dump(const struct dicelang_parse_node *node, FILE *to_file);
// Prints a single parse tree node to a file.
void dicelang_parse_node_print(const struct dicelang_parse_node *node, FILE *to_file);
// Destroys a parse node and all its children recursively.
void dicelang_parse_node_destroy(struct dicelang_parse_node **node, struct allocator alloc);

// Interprets a parse tree to produce a the user can work with.
void dicelang_interpret(struct dicelang_parse_node *tree, struct dicelang_error *error_sink, struct allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif