#include <cstdint>

#include <doctest/doctest.h>

#include "libmeta/helpers.hpp"

using namespace libmeta::helper;

TEST_CASE("get_magnitude")
{
    SUBCASE("positive signed values")
    {
        CHECK(get_magnitude(0) == 0);
        CHECK(get_magnitude(1) == 1);
        CHECK(get_magnitude(42) == 42);
    }

    SUBCASE("negative signed values")
    {
        CHECK(get_magnitude(-1) == 1);
        CHECK(get_magnitude(-42) == 42);
        CHECK(get_magnitude(INT32_MIN) == 2147483648u);
    }

    SUBCASE("unsigned values")
    {
        CHECK(get_magnitude(0u) == 0);
        CHECK(get_magnitude(42u) == 42);
    }
}

TEST_CASE("is_negative")
{
    SUBCASE("signed values")
    {
        CHECK_FALSE(is_negative(0));
        CHECK_FALSE(is_negative(1));
        CHECK(is_negative(-1));
        CHECK(is_negative(INT32_MIN));
    }

    SUBCASE("unsigned values")
    {
        CHECK_FALSE(is_negative(0u));
        CHECK_FALSE(is_negative(42u));
    }
}

TEST_CASE("count_digits")
{
    SUBCASE("base 10")
    {
        CHECK(count_digits(0) == 1);
        CHECK(count_digits(1) == 1);
        CHECK(count_digits(9) == 1);
        CHECK(count_digits(10) == 2);
        CHECK(count_digits(99) == 2);
        CHECK(count_digits(100) == 3);
        CHECK(count_digits(999) == 3);
    }

    SUBCASE("base 16")
    {
        CHECK(count_digits(0, 16) == 1);
        CHECK(count_digits(15, 16) == 1);
        CHECK(count_digits(16, 16) == 2);
        CHECK(count_digits(255, 16) == 2);
        CHECK(count_digits(256, 16) == 3);
    }
}
