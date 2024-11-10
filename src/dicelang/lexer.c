
#include <lexer.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
static const char *DTOK_names[] = {
        [DTOK_invalid]            = "invalid",
        [DTOK_empty]              = "empty",

        [DTOK_line_end]           = "line_end",
        [DTOK_file_end]           = "file_end",
        [DTOK_identifier]         = "identifier",
        [DTOK_value]              = "value",
        [DTOK_value_real]         = "value_real",
        [DTOK_separator]          = "separator",
        [DTOK_addition]           = "addition",
        [DTOK_substraction]       = "substraction",
        [DTOK_d]                  = "d",
        [DTOK_designator]         = "designator",
        [DTOK_open_parenthesis]   = "open parenthesis",
        [DTOK_close_parenthesis]  = "close parenthesis",
        [DTOK_open_bracket]       = "open bracket",
        [DTOK_close_bracket]      = "close bracket",
        [DTOK_open_sq_bracket]    = "open square bracket",
        [DTOK_close_sq_bracket]   = "close square bracket",
};

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct dicelang_token_transition {
    enum dicelang_token_flavour to;
    bool is_endpoint;
};

#define dicelang_token_no_transition (struct dicelang_token_transition) { .to = DTOK_invalid, .is_endpoint = false }
#define dicelang_token_empty_transition (struct dicelang_token_transition) { .to = DTOK_empty, .is_endpoint = false }

/**
 * @brief
 *
 */
static const struct dicelang_token_transition dicelang_transitions[256][DTOK_NUMBER] = {
        ['\n']  = { [DTOK_empty] = { DTOK_line_end, true },   [DTOK_line_end] = { DTOK_line_end, true } },

        ['a']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['b']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['c']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['d']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['e']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['f']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['g']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['h']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['i']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['j']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['k']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['l']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['m']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['n']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['o']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['p']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['q']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['r']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['s']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['t']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['u']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['v']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['w']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['x']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['y']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['z']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },

        ['A']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['B']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['C']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['D']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['E']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['F']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['G']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['H']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['I']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['J']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['K']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['L']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['M']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['N']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['O']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['P']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['Q']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['R']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['S']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['T']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['U']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['V']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['W']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['X']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['Y']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },
        ['Z']   = { [DTOK_empty] = { DTOK_identifier, true }, [DTOK_identifier] = { DTOK_identifier, true } },

        ['0']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['1']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['2']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['3']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['4']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['5']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['6']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['7']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['8']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['9']   = { [DTOK_identifier] = { DTOK_identifier, true } },
        ['_']   = { [DTOK_identifier] = { DTOK_identifier, true } },
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void dicelang_print_token(struct dicelang_token token, FILE *to_file);
static struct dicelang_token dicelang_token_read(const char **text, u32 line, u32 col);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param source_code
 * @param alloc
 * @return RANGE_TOKEN*
 */
RANGE_TOKEN *dicelang_tokenize(const char *source_code, struct allocator alloc)
{
    RANGE_TOKEN *read_tokens = nullptr;
    struct dicelang_token tok = { .flavour = DTOK_empty };
    u32 line = 0u;
    u32 col = 0u;

    if (!source_code) {
        return nullptr;
    }

    read_tokens = range_create_dynamic(alloc, sizeof(*read_tokens->data), 512u);

    while ((*source_code != '\0') && (tok.flavour != DTOK_invalid)) {
        while (*source_code == ' ') {
            source_code += 1;
            col += 1;
        }

        tok = dicelang_token_read(&source_code, line, col);

        if (tok.flavour == DTOK_line_end) {
            line += tok.value.source_length;
        } else {
            col += tok.value.source_length;
        }

        read_tokens = range_ensure_capacity(alloc, RANGE_TO_ANY(read_tokens), 1u);
        range_push(RANGE_TO_ANY(read_tokens), &tok);
    }

    return read_tokens;
}

/**
 * @brief
 *
 * @param tokens
 * @param to_file
 */
void dicelang_token_dump(RANGE_TOKEN *tokens, FILE *to_file)
{
    if (!tokens || !tokens->data) {
        return;
    }

    for (size_t i = 0u ; i < tokens->length ; i++) {
        dicelang_print_token(tokens->data[i], to_file);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param token
 * @param to_file
 */
static void dicelang_print_token(struct dicelang_token token, FILE *to_file)
{
    if (!to_file || (token.flavour >= DTOK_NUMBER)) {
        return;
    }

    fprintf(to_file, "(%d:%d) %s\t", token.where.line, token.where.col, DTOK_names[token.flavour]);
    for (size_t i = 0u ; i < token.value.source_length ; i++) {
        fprintf(to_file, "%c", token.value.source[i]);
    }
    fprintf(to_file, "\n");
}

/**
 * @brief
 *
 * @param text
 * @return struct DTOK
 */
static struct dicelang_token dicelang_token_read(const char **text, u32 line, u32 col)
{
    struct dicelang_token_transition current_transition = dicelang_token_empty_transition;
    struct dicelang_token_transition next_transition = dicelang_token_no_transition;
    const char *value = *text;

    if(!text || !*text) {
        return (struct dicelang_token) { .flavour = DTOK_invalid, .where = { line, col } };
    }

    do {
        next_transition = dicelang_transitions[**text][current_transition.to];

        if (next_transition.to == DTOK_invalid) {
            break;
        }
        *text += 1;
        current_transition = next_transition;
    } while (**text != '\0');

    if (current_transition.is_endpoint) {
        return (struct dicelang_token) {
                .flavour = current_transition.to,
                .value = { value, (uintptr_t) *text - (uintptr_t) value },
                .where = { line, col }
        };
    }

    return (struct dicelang_token) { .flavour = DTOK_invalid, .where = { line, col } };
}
