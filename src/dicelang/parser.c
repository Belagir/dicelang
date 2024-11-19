
#include <ustd/range.h>

#include <dicelang.h>
#include <parser.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

enum parser_element_flavour {
    PARSER_ELEMENT_func,
    PARSER_ELEMENT_val,
};

struct parser_element {
    enum parser_element_flavour flavour;
    union {
        bool (*func)(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
        enum dicelang_token_flavour val;
    };
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static bool assignment(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
static bool variable(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
static bool expression(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);

// -------------------------------------------------------------------------------------------------

static size_t expect(RANGE_TOKEN *tokens, size_t offset, size_t nb_elements, struct parser_element *what);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param tokens
 * @param alloc
 */
void dicelang_parse(RANGE_TOKEN *tokens, struct allocator alloc)
{
    printf("%d\n", expect(tokens, 0, 2, (struct parser_element[]) {
            { PARSER_ELEMENT_func, .func = &assignment },
            { PARSER_ELEMENT_val, .val   = DTOK_line_end },
    }));
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @return true
 * @return false
 */
static bool assignment(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens)
{
    *out_read_tokens = expect(tokens, index, 3, (struct parser_element[]) {
            { PARSER_ELEMENT_func, .func = &variable },
            { PARSER_ELEMENT_val, .val   = DTOK_designator },
            { PARSER_ELEMENT_func, .func = &expression },
    });

    return *out_read_tokens > 0;
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @return true
 * @return false
 */
static bool variable(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens)
{
    *out_read_tokens = expect(tokens, index, 1, (struct parser_element[]) {
            { PARSER_ELEMENT_val, .val =  DTOK_identifier },
    });

    return *out_read_tokens > 0;
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @return true
 * @return false
 */
static bool expression(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens)
{
    if (!*out_read_tokens) {
        *out_read_tokens = expect(tokens, index, 3, (struct parser_element[]) {
                { PARSER_ELEMENT_val, .val = DTOK_value },
                { PARSER_ELEMENT_val, .val = DTOK_d },
                { PARSER_ELEMENT_val, .val = DTOK_value },
        });
    }

    if (!*out_read_tokens) {
        *out_read_tokens = expect(tokens, index, 3, (struct parser_element[]) {
                { PARSER_ELEMENT_val, .val = DTOK_value },
                { PARSER_ELEMENT_val, .val = DTOK_addition },
                { PARSER_ELEMENT_val, .val = DTOK_value },
        });
    }

    if (!*out_read_tokens) {
        *out_read_tokens = expect(tokens, index, 1, (struct parser_element[]) {
                { PARSER_ELEMENT_val, .val   = DTOK_value },
        });
    }

    return *out_read_tokens > 0;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param tokens
 * @param offset
 * @param nb_elements
 * @param what
 * @return u32
 */
static size_t expect(RANGE_TOKEN *tokens, size_t offset, size_t nb_elements, struct parser_element *what)
{
    size_t pos = 0;
    size_t read_tokens = 0;
    size_t all_read_tokens = 0;
    bool matches = false;

    if (!what || !nb_elements) {
        return 0;
    }

    matches = true;
    while ((pos < nb_elements) && matches) {
        read_tokens = 0;

        switch (what[pos].flavour)
        {
            case PARSER_ELEMENT_func:
                matches = what[pos].func(tokens, offset + pos, &read_tokens);
                break;
            case PARSER_ELEMENT_val:
                matches = (what[pos].val == tokens->data[offset + pos].flavour);
                read_tokens += matches;
                break;
        }

        pos += 1;
        all_read_tokens += read_tokens;
    }

    if (matches) {
        return all_read_tokens;
    }
    return 0;
}
