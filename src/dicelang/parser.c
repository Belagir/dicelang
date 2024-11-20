
#include <ustd/range.h>

#include <dicelang.h>
#include <lexer.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

enum parser_element_flavour {
    PARSER_ELEMENT_func,
    PARSER_ELEMENT_val,
};

struct parser_element {
    enum parser_element_flavour flavour;
    union {
        size_t (*func)(RANGE_TOKEN *tokens, size_t index);
        enum dicelang_token_flavour val;
    };
};

struct parser_rule {
    RANGE_TOKEN *tokens;
    size_t offset;
    size_t nb_elements;
    struct parser_element *what;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static size_t assignment(RANGE_TOKEN *tokens, size_t index);
static size_t variable  (RANGE_TOKEN *tokens, size_t index);
static size_t expression(RANGE_TOKEN *tokens, size_t index);
static size_t term      (RANGE_TOKEN *tokens, size_t index);
static size_t dice      (RANGE_TOKEN *tokens, size_t index);
static size_t addition  (RANGE_TOKEN *tokens, size_t index);

// -------------------------------------------------------------------------------------------------

static size_t expect(struct parser_rule rule);
static size_t one_of(size_t nb, struct parser_rule rules[]);

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
    printf("%d\n", expect((struct parser_rule) { tokens, 0, 2, (struct parser_element[]) {
            { PARSER_ELEMENT_func, .func = &assignment },
            { PARSER_ELEMENT_val, .val   = DTOK_line_end },
    }}));
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
static size_t assignment(RANGE_TOKEN *tokens, size_t index)
{
    return expect((struct parser_rule) { tokens, index, 3, (struct parser_element[]) {
            { PARSER_ELEMENT_func, .func = &variable },
            { PARSER_ELEMENT_val, .val   = DTOK_designator },
            { PARSER_ELEMENT_func, .func = &expression },
    }});
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @return true
 * @return false
 */
static size_t variable(RANGE_TOKEN *tokens, size_t index)
{
    return expect((struct parser_rule) { tokens, index, 1, (struct parser_element[]) {
            { PARSER_ELEMENT_val, .val =  DTOK_identifier },
    }});
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @return true
 * @return false
 */
static size_t expression(RANGE_TOKEN *tokens, size_t index)
{
    return one_of(3, (struct parser_rule[]) {
            { tokens, index, 1, (struct parser_element[]) {
                    { PARSER_ELEMENT_func, .func = &addition },
            }},
            { tokens, index, 1, (struct parser_element[]) {
                    { PARSER_ELEMENT_func, .func = &dice },
            }},
            { tokens, index, 1, (struct parser_element[]) {
                    { PARSER_ELEMENT_func, .func = &term },
            }},
    });
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @param out_read_tokens
 * @return true
 * @return false
 */
static size_t term(RANGE_TOKEN *tokens, size_t index)
{
    return one_of(2, (struct parser_rule[]) {
            { tokens, index, 1, (struct parser_element[]) {
                    { PARSER_ELEMENT_val, .val = DTOK_value },
            }},
            { tokens, index, 3, (struct parser_element[]) {
                    { PARSER_ELEMENT_val, .val   = DTOK_open_parenthesis },
                    { PARSER_ELEMENT_func, .func = &expression },
                    { PARSER_ELEMENT_val, .val   = DTOK_close_parenthesis },
            }},
    });
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @param out_read_tokens
 * @return true
 * @return false
 */
static size_t dice(RANGE_TOKEN *tokens, size_t index)
{
    return expect((struct parser_rule) { tokens, index, 3, (struct parser_element[]) {
            { PARSER_ELEMENT_func, .func = &term },
            { PARSER_ELEMENT_val, .val = DTOK_d },
            { PARSER_ELEMENT_func, .func = &term },
    }});
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @return size_t
 */
static size_t addition(RANGE_TOKEN *tokens, size_t index)
{
    return one_of(2, (struct parser_rule[]) {
            { tokens, index, 3, (struct parser_element[]) {
                    { PARSER_ELEMENT_func, .func = &dice },
                    { PARSER_ELEMENT_val, .val = DTOK_addition },
                    { PARSER_ELEMENT_func, .func = &expression },
            }},
            { tokens, index, 3, (struct parser_element[]) {
                    { PARSER_ELEMENT_func, .func = &term },
                    { PARSER_ELEMENT_val, .val = DTOK_addition },
                    { PARSER_ELEMENT_func, .func = &expression },
            }},
    });
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
static size_t expect(struct parser_rule rule)
{
    size_t pos = 0;
    size_t read_tokens = 0;
    size_t all_read_tokens = 0;
    bool matches = false;

    if (!rule.what || !rule.nb_elements) {
        return 0;
    }

    matches = true;
    while ((pos < rule.nb_elements) && matches) {
        read_tokens = 0;

        switch (rule.what[pos].flavour) {
            case PARSER_ELEMENT_func:
                read_tokens = rule.what[pos].func(rule.tokens, rule.offset + all_read_tokens);
                break;
            case PARSER_ELEMENT_val:
                read_tokens = (rule.what[pos].val == rule.tokens->data[rule.offset + all_read_tokens].flavour);
                break;
        }
        matches = read_tokens > 0;

        pos += 1;
        all_read_tokens += read_tokens;
    }

    if (matches) {
        return all_read_tokens;
    }
    return 0;
}

/**
 * @brief
 *
 * @param nb
 * @param rules
 * @return size_t
 */
static size_t one_of(size_t nb, struct parser_rule rules[])
{
    size_t all_read_tokens = 0;
    size_t pos = 0;

    while ((pos < nb) && (all_read_tokens == 0)) {
        all_read_tokens = expect(rules[pos]);
        pos += 1;
    }

    return all_read_tokens;
}
