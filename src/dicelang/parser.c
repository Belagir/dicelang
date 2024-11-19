
#include <ustd/range.h>

#include <dicelang.h>
#include <parser.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef bool (*parser_func)(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
typedef RANGE(parser_func) RANGE_PARSER_F;

static size_t expect(RANGE_TOKEN *tokens, size_t offset, size_t nb_elements, parser_func *what);

// -------------------------------------------------------------------------------------------------

static bool assignment(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
static bool variable(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
static bool designator(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
static bool expression(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);
static bool identifier(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens);

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
    assignment(tokens, 0, &(size_t) { 0 });
}

// -------------------------------------------------------------------------------------------------
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
static size_t expect(RANGE_TOKEN *tokens, size_t offset, size_t nb_elements, parser_func *what)
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
        matches = what[pos](tokens, offset + pos, &read_tokens);

        pos += 1;
        all_read_tokens += read_tokens;
    }

    if (matches) {
        return all_read_tokens;
    }
    return 0;
}

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
    *out_read_tokens = expect(tokens, index, 3, (parser_func[]) { &variable, &designator, &expression });

    if (*out_read_tokens) printf("assignment\n");

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
    *out_read_tokens = expect(tokens, index, 1, (parser_func[]) { &identifier });

    if (*out_read_tokens) printf("variable\n");

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
static bool designator(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens)
{
    if (!tokens || !tokens->data || (index >= tokens->length)) {
        return false;
    }

    *out_read_tokens = (tokens->data[index].flavour == DTOK_designator);
    if (*out_read_tokens) printf("designator\n");
    return *out_read_tokens;
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
    if (!tokens || !tokens->data || (index >= tokens->length)) {
        return false;
    }

    *out_read_tokens = tokens->data[index].flavour == DTOK_value;
    if (*out_read_tokens) printf("expression\n");
    return *out_read_tokens;
}

/**
 * @brief
 *
 * @param tokens
 * @param index
 * @return true
 * @return false
 */
static bool identifier(RANGE_TOKEN *tokens, size_t index, size_t *out_read_tokens)
{
    if (!tokens || !tokens->data || (index >= tokens->length)) {
        return false;
    }

    *out_read_tokens = tokens->data[index].flavour == DTOK_identifier;
    if (*out_read_tokens) printf("identifier\n");
    return *out_read_tokens;
}
