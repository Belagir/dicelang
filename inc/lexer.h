/**
 * @file lexer.h
 * @author gabriel
 * @brief Dicelang lexer header file. Contains utilities to translate some script into a collection of tokens, as a first intermediate representation.
 * @version 0.1
 * @date 2024-11-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __DICELANG_LEXER_H__
#define __DICELANG_LEXER_H__

#include <stdio.h>

#include <ustd/common.h>
#include <ustd/range.h>

#include <dicelang.h>

//  ------------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------

// Creates a set of tokens representing the given source code.
RANGE_TOKEN *dicelang_tokenize(const char *source_code, struct allocator alloc);
// Prints debug token information to some file.
void dicelang_token_dump(RANGE_TOKEN *tokens, FILE *to_file);
// Prints one token to a file.
void dicelang_token_print(struct dicelang_token token, FILE *to_file);

//  ------------------------------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------

#endif