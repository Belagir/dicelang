
#include <ustd/math.h>
#include <ustd/testutilities.h>

#include "distribution.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static f32 dicelang_token_value(const char *bytes, size_t length);

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

    new_distrib = (struct dicelang_distrib) {
            .values = range_create_dynamic(alloc, sizeof(*new_distrib.values->data), 8),
    };

    if (!new_distrib.values) {
        return (struct dicelang_distrib) { };
    }

    range_push(RANGE_TO_ANY(new_distrib.values), &(struct dicelang_entry) { .val = dicelang_token_value(token.value.source, token.value.source_length), .weight = 1.f });

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
}