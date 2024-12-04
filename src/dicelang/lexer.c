/**
 * @file lexer.c
 * @author gabriel
 * @brief Dicelang lexer implementation file.
 * @version 0.1
 * @date 2024-11-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <dicelang.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data representing some partial transition. Used to map a token flavour to some other token flavour in a transition array.
 *
 * @see dicelang_token_definitions
 */
struct dicelang_token_transition {
    enum dicelang_token_flavour to;
    bool is_endpoint;
};

/// Trap transition. Must translate to a zeroed-out transition !
#define dicelang_token_no_transition (struct dicelang_token_transition) { .to = DTOK_invalid, .is_endpoint = false }
/// Empty transition. Used as a starting point before parsing a token.
#define dicelang_token_empty_transition (struct dicelang_token_transition) { .to = DTOK_empty, .is_endpoint = false }

/**
 * @brief Map from a character, to a token flavour, to a transition to another token flavour.
 * Accessing [c][tok] will give a transition to the valid token that the language knows of. If there is none, a dicelang_token_no_transition is given.
 */
static const struct dicelang_token_transition dicelang_token_definitions[][DTOK_NUMBER] = {
        ['\0']  = { [DTOK_empty] = { DTOK_file_end, true } },

        ['\n']  = { [DTOK_empty] = { DTOK_line_end, true },
                    [DTOK_line_end] = { DTOK_line_end, true } },

        ['a']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['b']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['c']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },

        ['d']   = { [DTOK_empty]        = { DTOK_op_d, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },

        ['e']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['f']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['g']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['h']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['i']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['j']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['k']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['l']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['m']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['n']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['o']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['p']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['q']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['r']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['s']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['t']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['u']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['v']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['w']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['x']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['y']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['z']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },

        ['A']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['B']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['C']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['D']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['E']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['F']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['G']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['H']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['I']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['J']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['K']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['L']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['M']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['N']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['O']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['P']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['Q']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['R']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['S']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['T']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['U']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['V']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['W']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['X']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['Y']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },
        ['Z']   = { [DTOK_empty]        = { DTOK_identifier, true },
                    [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },

        ['0']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['1']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['2']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['3']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['4']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['5']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['6']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['7']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['8']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },
        ['9']   = { [DTOK_empty]        = { DTOK_value, true },
                    [DTOK_value]        = { DTOK_value, true },
                    [DTOK_identifier]   = { DTOK_identifier, true } },


        ['_']   = { [DTOK_identifier]   = { DTOK_identifier, true },
                    [DTOK_op_d]            = { DTOK_identifier, true } },

        [':']   = { [DTOK_empty] = { DTOK_designator,     true } },
        [',']   = { [DTOK_empty] = { DTOK_separator,      true } },
        ['+']   = { [DTOK_empty] = { DTOK_op_addition,       true } },
        ['-']   = { [DTOK_empty] = { DTOK_op_substraction,   true } },
        ['*']   = { [DTOK_empty] = { DTOK_op_multiplication, true } },

        ['(']   = { [DTOK_empty] = { DTOK_open_parenthesis,  true } },
        [')']   = { [DTOK_empty] = { DTOK_close_parenthesis, true } },
        ['{']   = { [DTOK_empty] = { DTOK_open_bracket,      true } },
        ['}']   = { [DTOK_empty] = { DTOK_close_bracket,     true } },
        ['[']   = { [DTOK_empty] = { DTOK_open_sq_bracket,   true } },
        [']']   = { [DTOK_empty] = { DTOK_close_sq_bracket,  true } },
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// Reads one token from a string and consumes the characters read.
static struct dicelang_token dicelang_token_read(const char **text, u32 line, u32 col);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates an array of tokens from a script in an ascii string.
 * The array will contain the different tokens encoded in the string in the order they were encountered, and ends either with a newline token), end of file token, or an invalid token.
 *
 * @attention The returned array is allocated and its memory should be managed.
 *
 * @param[in] source_code Script to translate into tokens. If NULL, the array retiurned is also NULL.
 * @param[in] alloc Allocator used to create the token range.
 * @return RANGE_TOKEN*
 */
RANGE_TOKEN *dicelang_tokenize(const char *source_code, struct dicelang_error *error_sink, struct allocator alloc)
{
    RANGE_TOKEN *read_tokens = nullptr;
    struct dicelang_token tok = { .flavour = DTOK_empty };
    u32 line = 1u;
    u32 col = 1u;

    if (!source_code) {
        error_sink->flavour = DERR_INTERNAL;
        error_sink->what = "Tried to tokenize some source that could not be read.";
        return nullptr;
    }

    read_tokens = range_create_dynamic(alloc, sizeof(*read_tokens->data), 512u);

    while ((tok.flavour != DTOK_invalid) && (tok.flavour != DTOK_file_end)) {
        // skipping whitespaces & comments
        while ((*source_code == ' ') || (*source_code == '\t') || (*source_code == '#')) {
            // whitespaces until some other char
            if ((*source_code == ' ') || (*source_code == '\t')) {
                source_code += 1;
                col += 1;
            }
            // comments until end of line
            if (*source_code == '#') {
                while (*source_code && (*source_code != '\n')) {
                    source_code += 1;
                }
            }
        }

        // one token read at a time
        tok = dicelang_token_read(&source_code, line, col);

        // updating the line / column pair
        if (tok.flavour == DTOK_line_end) {
            col = 1;
            line += tok.value.source_length;
        } else {
            col += tok.value.source_length;
        }

        if ((tok.flavour != DTOK_line_end)
            || (read_tokens->length && (read_tokens->data[read_tokens->length-1].flavour != DTOK_line_end))) {
            // adding the token in the range if not a repeating newline
            read_tokens = range_ensure_capacity(alloc, RANGE_TO_ANY(read_tokens), 1u);
            range_push(RANGE_TO_ANY(read_tokens), &tok);
        }
    }

    if (tok.flavour == DTOK_invalid) {
        error_sink->flavour = DERR_TOKEN;
        error_sink->token = tok;
        error_sink->what = "unrecognized token";
    }

    return read_tokens;
}

/**
 * @brief Prints a range of token to a file.
 * For debug purposes.
 *
 * @param[in] tokens
 * @param[in] to_file
 */
void dicelang_token_dump(RANGE_TOKEN *tokens, FILE *to_file)
{
    if (!tokens) {
        return;
    }

    for (size_t i = 0u ; i < tokens->length ; i++) {
        dicelang_token_print(tokens->data[i], to_file);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Reads one token from a string, and consumes the character(s) representing this token.
 *
 * @param[inout] text Actual script from which the token is read.
 * @param[in] line Line we are at, very nicely, generously and fabulously provided by the caller.
 * @param[in] col Column we are at, very nicely, generously and fabulously provided by the caller.
 * @return struct DTOK
 */
static struct dicelang_token dicelang_token_read(const char **text, u32 line, u32 col)
{
    struct dicelang_token_transition current_transition = dicelang_token_empty_transition;
    struct dicelang_token_transition next_transition = dicelang_token_no_transition;
    const char *value = nullptr;

    if(!text || !*text) {
        return (struct dicelang_token) { .flavour = DTOK_invalid, .where = { line, col } };
    }

    value = *text;

    do {
        // fetching the eventual transition
        next_transition = dicelang_token_definitions[(size_t) **text][current_transition.to];

        // no transition exists, we are either at the end of a valid token (.is_endpoint is set) or a syntax error occurred.
        if (next_transition.to == DTOK_invalid) {
            break;
        }

        // happy path, we advance through the token
        *text += 1;
        current_transition = next_transition;
    } while (**text != '\0');

    // actual token is valid !
    if (current_transition.is_endpoint) {
        return (struct dicelang_token) {
                .flavour = current_transition.to,
                .value = { value, (uintptr_t) *text - (uintptr_t) value },
                .where = { line, col }
        };
    }

    // synatx error....
    return (struct dicelang_token) { .flavour = DTOK_invalid, .where = { line, col } };
}
