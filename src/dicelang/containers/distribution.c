
#include <ustd/math.h>
#include <ustd/sorting.h>
#include <ustd/testutilities.h>

#include "distribution.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static f32 dicelang_token_value(const char *bytes, size_t length);


static struct dicelang_distrib dicelang_distrib_create_empty(struct allocator alloc);
static void dicelang_distrib_push_value(struct dicelang_distrib *target, f32 value, u32 count, struct allocator alloc);
static void dicelang_distrib_push_distrib(struct dicelang_distrib *out_into, struct dicelang_distrib from, struct allocator alloc);

static i32 dicelang_entry_compare(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct dicelang_entry (*dicelang_distrib_modif_func)(struct dicelang_entry lhs, struct dicelang_entry rhs);

static void dicelang_distrib_combine(struct dicelang_distrib *out_into, dicelang_distrib_modif_func f, struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc);
static void dicelang_distrib_transform(struct dicelang_distrib *target, dicelang_distrib_modif_func f, struct dicelang_entry seed);

static struct dicelang_entry dicelang_distrib_add_entries(struct dicelang_entry lhs, struct dicelang_entry rhs);
static struct dicelang_entry dicelang_distrib_sub_entries(struct dicelang_entry lhs, struct dicelang_entry rhs);
static struct dicelang_entry dicelang_distrib_mult_entries(struct dicelang_entry lhs, struct dicelang_entry rhs);

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
 * @param from
 * @param alloc
 * @return
 */
struct dicelang_distrib dicelang_distrib_copy(struct dicelang_distrib from, struct allocator alloc)
{
    struct dicelang_distrib new_distrib = { };
    new_distrib.values = range_create_dynamic_from_copy_of(alloc, RANGE_TO_ANY(from.values));

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
 * @param d
 * @return
 */
bool dicelang_distrib_is_empty(struct dicelang_distrib d)
{
    return ((d.values) && (d.values->length > 0));
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @param alloc
 * @return * struct dicelang_distrib
 */
struct dicelang_distrib dicelang_distrib_add(struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc)
{
    struct dicelang_distrib added = { };

    if (!lhs.values || !rhs.values) {
        return (struct dicelang_distrib) { };
    }

    added = dicelang_distrib_create_empty(alloc);

    if ((lhs.values->length == 0) || (rhs.values->length == 0)) {
        dicelang_distrib_push_distrib(&added, lhs, alloc);
        dicelang_distrib_push_distrib(&added, rhs, alloc);
        return added;
    }

    dicelang_distrib_combine(&added, &dicelang_distrib_add_entries, lhs, rhs, alloc);
    return added;
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @param alloc
 * @return * struct dicelang_distrib
 */
struct dicelang_distrib dicelang_distrib_substract(struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc)
{
    struct dicelang_distrib diff = { };

    if (!lhs.values || !rhs.values) {
        return (struct dicelang_distrib) { };
    }

    if (rhs.values->length == 0) {
        dicelang_distrib_push_distrib(&diff, lhs, alloc);
        return diff;
    }

    if (rhs.values->length == 0) {
        dicelang_distrib_push_distrib(&diff, lhs, alloc);
        dicelang_distrib_transform(&diff, &dicelang_distrib_mult_entries, (struct dicelang_entry) { .val = -1, .count = 1 });
        return diff;
    }

    diff = dicelang_distrib_create_empty(alloc);
    dicelang_distrib_combine(&diff, &dicelang_distrib_sub_entries, lhs, rhs, alloc);

    return diff;
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @param alloc
 * @return
 */
struct dicelang_distrib dicelang_distrib_multiply(struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc)
{
    struct dicelang_distrib mult = { };
    struct dicelang_distrib addition = { };
    struct dicelang_distrib buffer = { };

    if (!lhs.values || !rhs.values) {
        return (struct dicelang_distrib) { };
    }

    mult = dicelang_distrib_create_empty(alloc);
    addition = dicelang_distrib_create_empty(alloc);
    buffer = dicelang_distrib_create_empty(alloc);

    dicelang_distrib_destroy(&addition, alloc);
    dicelang_distrib_destroy(&buffer, alloc);

    return mult;
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @param alloc
 * @return
 */
struct dicelang_distrib dicelang_distrib_union(struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc)
{
    struct dicelang_distrib new_distrib = { };

    if (!lhs.values || !rhs.values) {
        return (struct dicelang_distrib) { };
    }

    new_distrib = dicelang_distrib_create_empty(alloc);

    dicelang_distrib_push_distrib(&new_distrib, lhs, alloc);
    dicelang_distrib_push_distrib(&new_distrib, rhs, alloc);

    return new_distrib;
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @param alloc
 * @return
 */
struct dicelang_distrib dicelang_distrib_dice(struct dicelang_distrib from, struct allocator alloc)
{
    struct dicelang_distrib new_distrib = { };

    if (!from.values) {
        return (struct dicelang_distrib) { };
    }

    new_distrib = dicelang_distrib_create_empty(alloc);

    for (size_t i = 0 ; i < from.values->length ; i++) {
        for (f32 k = 0 ; k < from.values->data[i].val ; k++) {
            dicelang_distrib_push_value(&new_distrib, k + 1.f, from.values->data[i].count, alloc);
        }
    }

    return new_distrib;
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
 * @param out_into
 * @param from
 * @param alloc
 */
static void dicelang_distrib_push_distrib(struct dicelang_distrib *out_into, struct dicelang_distrib from, struct allocator alloc)
{
    if (!out_into || !from.values) {
        return;
    }

    for (size_t i = 0 ; i < from.values->length ; i++) {
        dicelang_distrib_push_value(out_into, from.values->data[i].val, from.values->data[i].count, alloc);
    }
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

/**
 * @brief
 *
 * @param out_into
 * @param f
 * @param lhs
 * @param rhs
 * @param alloc
 */
static void dicelang_distrib_combine(struct dicelang_distrib *out_into, dicelang_distrib_modif_func f, struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc)
{
    struct dicelang_entry tmp_entry = { };

    for (size_t i_lhs = 0 ; i_lhs < lhs.values->length ; i_lhs++) {
        for (size_t i_rhs = 0 ; i_rhs < rhs.values->length ; i_rhs++) {
            tmp_entry = f(lhs.values->data[i_lhs], rhs.values->data[i_rhs]);
            dicelang_distrib_push_value(out_into, tmp_entry.val, tmp_entry.count, alloc);
        }
    }
}

/**
 * @brief
 *
 * @param target
 * @param f
 * @param seed
 */
static void dicelang_distrib_transform(struct dicelang_distrib *target, dicelang_distrib_modif_func f, struct dicelang_entry seed)
{
    for (size_t i = 0 ; i < target->values->length ; i++) {
        target->values->data[i] = f(target->values->data[i], seed);
    }
}

/**
 * @brief
 *
 */
static struct dicelang_entry dicelang_distrib_add_entries(struct dicelang_entry lhs, struct dicelang_entry rhs)
{
    return (struct dicelang_entry) { .val = lhs.val + rhs.val, .count = lhs.count * rhs.count };
}

/**
 * @brief
 *
 */
static struct dicelang_entry dicelang_distrib_sub_entries(struct dicelang_entry lhs, struct dicelang_entry rhs)
{
    return (struct dicelang_entry) { .val = lhs.val - rhs.val, .count = lhs.count * rhs.count };
}

/**
 * @brief
 *
 */
static struct dicelang_entry dicelang_distrib_mult_entries(struct dicelang_entry lhs, struct dicelang_entry rhs)
{
    return (struct dicelang_entry) { .val = lhs.val * rhs.val, .count = lhs.count * rhs.count };
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

            struct dicelang_distrib added = dicelang_distrib_add(mock_distrib_lhs, mock_distrib_rhs, make_system_allocator());

            if (!added.values) {
                tst_assert(false, "addition result has not been allocated");
                return;
            }

            if (added.values->length != data->expected.length) {
                tst_assert_equal(data->expected.length, added.values->length, "length of %d");
                return;
            }

            for (size_t i = 0 ; i < data->expected.length ; i++) {
                tst_assert(float_equal(data->expected.data[i].val, added.values->data[i].val, 1), "values mismatch : expected %f, got %f", data->expected.data[i].val, added.values->data[i].val);
                tst_assert_equal_ext(data->expected.data[i].count, added.values->data[i].count, "count of %d", "at index %d", i);
            }

            dicelang_distrib_destroy(&added, make_system_allocator());
        }
)

tst_CREATE_TEST_CASE(distr_add_nominal, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = 2, .count = 1 }, { .val = 3, .count = 2 }, { .val = 4, .count = 1 }, }),
)
tst_CREATE_TEST_CASE(distr_add_empty_left, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),
)
tst_CREATE_TEST_CASE(distr_add_empty_right, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),
)
tst_CREATE_TEST_CASE(distr_add_empty_empty, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { }),
)
tst_CREATE_TEST_CASE(distr_add_nominal_counted, distr_add,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 2, .count = 1 }, { .val = 3, .count = 2 }, { .val = 4, .count = 1 } }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = 3, .count = 1 }, { .val = 4, .count = 3 }, { .val = 5, .count = 3 }, { .val = 6, .count = 1 }, }),
)

tst_CREATE_TEST_SCENARIO(distr_sub,
        {
            RANGE(struct dicelang_entry, 6) lhs;
            RANGE(struct dicelang_entry, 6) rhs;

            RANGE(struct dicelang_entry, 36) expected;
        },
        {
            struct dicelang_distrib mock_distrib_lhs = { .values = (void *) &data->lhs };
            struct dicelang_distrib mock_distrib_rhs = { .values = (void *) &data->rhs };

            struct dicelang_distrib diff = dicelang_distrib_substract(mock_distrib_lhs, mock_distrib_rhs, make_system_allocator());

            if (!diff.values) {
                tst_assert(false, "addition result has not been allocated");
                return;
            }

            if (diff.values->length != data->expected.length) {
                tst_assert_equal(data->expected.length, diff.values->length, "length of %d");
                return;
            }

            for (size_t i = 0 ; i < data->expected.length ; i++) {
                tst_assert(float_equal(data->expected.data[i].val, diff.values->data[i].val, 1), "values mismatch : expected %f, got %f", data->expected.data[i].val, diff.values->data[i].val);
                tst_assert_equal_ext(data->expected.data[i].count, diff.values->data[i].count, "count of %d", "at index %d", i);
            }

            dicelang_distrib_destroy(&diff, make_system_allocator());
        }
)

tst_CREATE_TEST_CASE(distr_sub_nominal, distr_sub,
        .lhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),
        .rhs = RANGE_CREATE_STATIC(struct dicelang_entry, 6, { { .val = 1, .count = 1 }, { .val = 2, .count = 1 } }),

        .expected = RANGE_CREATE_STATIC(struct dicelang_entry, 36, { { .val = -1, .count = 1 }, { .val = 0, .count = 2 }, { .val = 1, .count = 1 }, }),
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
    tst_run_test_case(distr_add_nominal_counted);

    tst_run_test_case(distr_sub_nominal);
}