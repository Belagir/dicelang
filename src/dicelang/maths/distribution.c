
#include <ustd/math.h>
#include <ustd/sorting.h>
#include <ustd/testutilities.h>

#include "distribution.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static f32 dicelang_token_value(const char *bytes, size_t length);


static struct dicelang_distrib dicelang_distrib_create_empty(struct allocator alloc);
static void dicelang_distrib_push_value(struct dicelang_distrib *target, f32 value, u32 count, struct allocator alloc);

static i32 dicelang_entry_compare(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------


/**
 * @brief
 *
 * @param token
 * @param alloc
 * @return struct dicelang_distrib
 */
struct dicelang_distrib dicelang_distrib_create(struct dicelang_token token, struct allocator alloc)
{
    struct dicelang_distrib new_distrib = { };

    if ((token.flavour != DTOK_value) || !token.value.source || !token.value.source_length) {
        return (struct dicelang_distrib) { };
    }

    new_distrib = dicelang_distrib_create_empty(alloc);

    if (!new_distrib.values) {
        return (struct dicelang_distrib) { };
    }

    dicelang_distrib_push_value(&new_distrib, dicelang_token_value(token.value.source, token.value.source_length), 1, alloc);

    return new_distrib;
}

/**
 * @brief
 *
 * @param distrib
 * @param alloc
 */
void dicelang_distrib_destroy(struct dicelang_distrib *distrib, struct allocator alloc)
{
    if (!distrib) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(distrib->values));

    *distrib = (struct dicelang_distrib) { };
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @param alloc
 * @return * struct dicelang_distrib
 */
struct dicelang_distrib dicelang_distrib_add(struct dicelang_distrib *lhs, struct dicelang_distrib *rhs, struct allocator alloc)
{
    struct dicelang_distrib added = { };

    if (!lhs || !lhs->values || !rhs || !rhs->values) {
        return (struct dicelang_distrib) { };
    }

    added = dicelang_distrib_create_empty(alloc);

    for (size_t i_lhs = 0 ; i_lhs < lhs->values->length ; i_lhs++) {
        for (size_t i_rhs = 0 ; i_rhs < rhs->values->length ; i_rhs++) {
            dicelang_distrib_push_value(&added,
                    lhs->values->data[i_lhs].val   + rhs->values->data[i_rhs].val,
                    lhs->values->data[i_lhs].count + rhs->values->data[i_rhs].count,
                    alloc);
        }
    }

    return added;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param bytes
 * @param length
 * @return
 */
static f32 dicelang_token_value(const char *bytes, size_t length)
{
    f32 read_integral = 0.f;
    f32 read_fractional = 0.f;
    size_t read_bytes = 0;
    f32 power = 0.f;

    if (!bytes || (length == 0)) {
        return 0.f;
    }

    // remove leading zeroes
    while ((read_bytes < length) && (bytes[read_bytes] == '0')) {
        read_bytes += 1;
    }

    // read integral part
    while ((read_bytes < length) && (bytes[read_bytes] != '.')) {
        read_integral *= 10.f;
        read_integral += (f32) (bytes[read_bytes] - '0');
        read_bytes += 1;
    }

    read_bytes += (bytes[read_bytes] == '.');

    // read fractional part
    power = 0.1f;
    while (read_bytes < length) {
        read_fractional += (f32) (bytes[read_bytes] - '0') * power;
        power /= 10.f;
        read_bytes += 1;
    }

    return read_integral + read_fractional;
}

/**
 * @brief
 *
 * @param alloc
 * @return struct dicelang_distrib
 */
static struct dicelang_distrib dicelang_distrib_create_empty(struct allocator alloc)
{
    struct dicelang_distrib new_distrib = { };

    new_distrib = (struct dicelang_distrib) {
            .values = range_create_dynamic(alloc, sizeof(*new_distrib.values->data), 8),
    };

    if (!new_distrib.values) {
        return (struct dicelang_distrib) { };
    }

    return new_distrib;
}

/**
 * @brief
 *
 * @param target
 * @param pushed
 * @param alloc
 */
static void dicelang_distrib_push_value(struct dicelang_distrib *target, f32 value, u32 count, struct allocator alloc)
{
    size_t index = 0;

    if (!target || (count == 0)) {
        return;
    }

    if (sorted_range_find_in(RANGE_TO_ANY(target->values), &dicelang_entry_compare, &(struct dicelang_entry) { .val = value }, &index)) {
        target->values->data[index].count += count;
        return;
    }

    target->values = range_ensure_capacity(alloc, RANGE_TO_ANY(target->values), 1);
    range_insert_value(RANGE_TO_ANY(target->values), index, &(struct dicelang_entry) { .val = value, .count = count });
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
static i32 dicelang_entry_compare(const void *lhs, const void *rhs)
{
    f32 lhs_val = *(f32 *) lhs;
    f32 rhs_val = *(f32 *) rhs;

    if (float_equal(lhs_val, rhs_val, 1u)) {
        return 0;
    }

    return (lhs_val > rhs_val) - (lhs_val < rhs_val);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_SCENARIO(bytes_to_f32,
        {
            const char *value;
            size_t length;

            f32 expected;
        },
        {
            f32 val = dicelang_token_value(data->value, data->length);
            tst_assert(float_equal(data->expected, val, 1), "values mismatch : expected %f, got %f", data->expected, val);
        }
)

tst_CREATE_TEST_CASE(bytes_to_f32_empty, bytes_to_f32,
        .value = NULL,
        .length = 0,
        .expected = 0.f
)
tst_CREATE_TEST_CASE(bytes_to_f32_null, bytes_to_f32,
        .value = NULL,
        .length = 2,
        .expected = 0.f
)
tst_CREATE_TEST_CASE(bytes_to_f32_whole, bytes_to_f32,
        .value = "42",
        .length = 2,
        .expected = 42.f
)
tst_CREATE_TEST_CASE(bytes_to_f32_decimal, bytes_to_f32,
        .value = "0.42",
        .length = 4,
        .expected = 0.42f
)
tst_CREATE_TEST_CASE(bytes_to_f32_nominal, bytes_to_f32,
        .value = "3112.043",
        .length = 8,
        .expected = 3112.043f
)
tst_CREATE_TEST_CASE(bytes_to_f32_leading_zeroes, bytes_to_f32,
        .value = "0003112.043",
        .length = 11,
        .expected = 3112.043f
)
tst_CREATE_TEST_CASE(bytes_to_f32_no_leading_zero, bytes_to_f32,
        .value = ".04323",
        .length = 6,
        .expected = 0.04323f
)
tst_CREATE_TEST_CASE(bytes_to_f32_trailing_zeroes, bytes_to_f32,
        .value = ".432300",
        .length = 7,
        .expected = 0.4323f
)
tst_CREATE_TEST_CASE(bytes_to_f32_dot, bytes_to_f32,
        .value = ".",
        .length = 1,
        .expected = 0.f
)

tst_CREATE_TEST_SCENARIO(distr_add,
        {
            RANGE(struct dicelang_entry, 6) lhs;
            RANGE(struct dicelang_entry, 6) rhs;

            RANGE(struct dicelang_entry, 36) expected;
        },
        {
            struct dicelang_distrib mock_distrib_lhs = { .values = (void *) &data->lhs };
            struct dicelang_distrib mock_distrib_rhs = { .values = (void *) &data->rhs };

            struct dicelang_distrib added = dicelang_distrib_add(&mock_distrib_lhs, &mock_distrib_rhs, make_system_allocator());

            tst_assert_equal(data->expected.length, added.values->length, "length of %d");

            if (added.values->length == data->expected.length) {
                for (size_t i = 0 ; i < data->expected.length ; i++) {
                    tst_assert(float_equal(data->expected.data[i].val, added.values->data[i].val, 1), "values mismatch : expected %f, got %f", data->expected.data[i].val, added.values->data[i].val);
                    tst_assert_equal_ext(data->expected.data[i].count, added.values->data[i].count, "count of %d", "at index %d", i);
                }
            }

            dicelang_distrib_destroy(&added, make_system_allocator());
        }
)

tst_CREATE_TEST_CASE(distr_add_nominal, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = 2, .count = 2 }, { .val = 3, .count = 4 }, { .val = 4, .count = 2 }, }),
)
tst_CREATE_TEST_CASE(distr_add_empty_left, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 }, }),
)
tst_CREATE_TEST_CASE(distr_add_empty_right, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 }, }),
)
tst_CREATE_TEST_CASE(distr_add_empty_empty, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { }),
)


void dicelang_distrib_test(void)
{
    tst_run_test_case(bytes_to_f32_empty);
    tst_run_test_case(bytes_to_f32_null);
    tst_run_test_case(bytes_to_f32_whole);
    tst_run_test_case(bytes_to_f32_decimal);
    tst_run_test_case(bytes_to_f32_nominal);
    tst_run_test_case(bytes_to_f32_leading_zeroes);
    tst_run_test_case(bytes_to_f32_no_leading_zero);
    tst_run_test_case(bytes_to_f32_trailing_zeroes);
    tst_run_test_case(bytes_to_f32_dot);

    tst_run_test_case(distr_add_nominal);
    tst_run_test_case(distr_add_empty_left);
    tst_run_test_case(distr_add_empty_right);
    tst_run_test_case(distr_add_empty_empty);
}