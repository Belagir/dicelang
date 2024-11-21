/**
 * @file dicelang.h
 * @author gabriel
 * @brief Main dicelang interpretation header.
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
 * @see parser.h tolookup what makes an "expression", "statement", "variable", "function", "mutator", or "array".
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
    DTOK_addition,              ///< Addition binary operand.
    DTOK_substraction,          ///< Substraction binary operand.
    DTOK_d,                     ///< Dice distribution binary operand.
    DTOK_designator,            ///< Assignment token to link an expression to a variable.
    DTOK_open_parenthesis,      ///< Token to start either a function call's argument list or to start isolating part of an expression.
    DTOK_close_parenthesis,     ///< Token to finish either a function call's argument list or to finish isolating part of an expression.
    DTOK_open_bracket,          ///< Token to start a mutator call's argument list.
    DTOK_close_bracket,         ///< Token to finish a mutator call's argument list.
    DTOK_open_sq_bracket,       ///< Token to start either an array access or to start an array declaration.
    DTOK_close_sq_bracket,      ///< Token to finish either an array access or to finish an array declaration.

    DTOK_NUMBER,                ///< Meta enum member to have a count the number of other members.
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

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
enum dicelang_syntax_flavour {
    DSTX_program = DTOK_NUMBER,

    DSTX_assignment,
    DSTX_variable,
    DSTX_expression,
    DSTX_dice,
    DSTX_factor,
    DSTX_operand,

    DSTX_NUMBER,
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

extern const char *DTOK_DSTX_names[];

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct dicelang_program {
    RANGE(const char) *text;
    struct dicelang_parse_node *parse_tree;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct dicelang_program dicelang_program_create_from_file(FILE *from_file, allocator alloc);

void dicelang_program_destroy(struct dicelang_program *program, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif