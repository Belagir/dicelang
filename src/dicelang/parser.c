
#include <ustd/range.h>

#include <dicelang.h>
#include <lexer.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void assignment(RANGE_TOKEN *tokens);
static void variable  (RANGE_TOKEN *tokens);
static void expression(RANGE_TOKEN *tokens);
static void dice      (RANGE_TOKEN *tokens);
static void factor    (RANGE_TOKEN *tokens);
static void operand   (RANGE_TOKEN *tokens);


// -------------------------------------------------------------------------------------------------

static bool expect(RANGE_TOKEN *tokens, enum dicelang_token_flavour what);
static bool accept(RANGE_TOKEN *tokens, enum dicelang_token_flavour what);
static bool next_is(RANGE_TOKEN *tokens, enum dicelang_token_flavour what);

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
    assignment(tokens);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param tokens
 * @param what
 * @return true
 * @return false
 */
static bool expect(RANGE_TOKEN *tokens, enum dicelang_token_flavour what)
{
    if (accept(tokens, what)) {
        return true;
    }
    printf("syntax error near %s (%d:%d) : expected %s\n",
            DTOK_names[tokens->data[0].flavour],
            tokens->data[0].where.line,
            tokens->data[0].where.col,
            DTOK_names[what]);
    return false;
}

/**
 * @brief
 *
 * @param tokens
 * @param what
 * @return true
 * @return false
 */
static bool accept(RANGE_TOKEN *tokens, enum dicelang_token_flavour what)
{
    if (next_is(tokens, what)) {
        printf("%s\n", DTOK_names[what]);
        return range_remove(RANGE_TO_ANY(tokens), 0);
    }
    return false;
}

/**
 * @brief
 *
 * @param tokens
 * @param what
 * @return true
 * @return false
 */
static bool next_is(RANGE_TOKEN *tokens, enum dicelang_token_flavour what)
{
    if (!tokens || !tokens->data || (tokens->length == 0)) {
        return false;
    }

    return tokens->data[0].flavour == what;
}


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void assignment(RANGE_TOKEN *tokens)
{
    printf("start -- assignment\n");
    variable(tokens);
    expect(tokens, DTOK_designator);
    expression(tokens);
    printf("end -- assignment\n");
}

static void variable(RANGE_TOKEN *tokens)
{
    printf("-- variable\n");
    expect(tokens, DTOK_identifier);
}

static void expression(RANGE_TOKEN *tokens)
{
    printf("start -- expression\n");
    dice(tokens);
    while (accept(tokens, DTOK_addition)) {
        dice(tokens);
    }
    printf("end -- expression\n");
}

static void dice(RANGE_TOKEN *tokens)
{
    printf("start -- dice\n");
    factor(tokens);
    while (accept(tokens, DTOK_d)) {
        factor(tokens);
    }
    printf("end -- dice\n");
}

static void factor(RANGE_TOKEN *tokens)
{
    operand(tokens);
}

static void operand(RANGE_TOKEN *tokens)
{
    printf("start -- operand\n");

    if (accept(tokens, DTOK_open_parenthesis)) {
        expression(tokens);
        expect(tokens, DTOK_close_parenthesis);
    } else {
        expect(tokens, DTOK_value);
    }
    printf("end -- operand\n");
}
