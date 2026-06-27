#include <cstring>

#include <doctest/doctest.h>

#include "libmeta/sstring.hpp"

using namespace libmeta::sstring;

TEST_CASE("container")
{
    SUBCASE("default constructed")
    {
        constexpr container<4> c;
        CHECK(c.size() == 3);
        CHECK(c.value[0] == '\0');
    }

    SUBCASE("from string literal")
    {
        constexpr container c("abc");
        CHECK(c.size() == 3);
        CHECK(std::strcmp(c.data(), "abc") == 0);
    }
}

TEST_CASE("concat")
{
    SUBCASE("two string literals")
    {
        constexpr auto result = concat("hello", " world");
        CHECK(std::strcmp(result.data(), "hello world") == 0);
        CHECK(result.size() == 11);
    }

    SUBCASE("two containers")
    {
        constexpr container a("foo");
        constexpr container b("bar");
        constexpr auto result = concat(a, b);
        CHECK(std::strcmp(result.data(), "foobar") == 0);
    }

    SUBCASE("container and literal")
    {
        constexpr container a("foo");
        constexpr auto result = concat(a, "bar");
        CHECK(std::strcmp(result.data(), "foobar") == 0);
    }

    SUBCASE("literal and container")
    {
        constexpr container b("bar");
        constexpr auto result = concat("foo", b);
        CHECK(std::strcmp(result.data(), "foobar") == 0);
    }

    SUBCASE("single literal passthrough")
    {
        constexpr auto result = concat("only");
        CHECK(std::strcmp(result.data(), "only") == 0);
    }

    SUBCASE("single container passthrough")
    {
        constexpr container c("only");
        constexpr auto result = concat(c);
        CHECK(std::strcmp(result.data(), "only") == 0);
    }

    SUBCASE("variadic concat")
    {
        constexpr auto result = concat("a", "b", "c", "d");
        CHECK(std::strcmp(result.data(), "abcd") == 0);
        CHECK(result.size() == 4);
    }

    SUBCASE("empty strings")
    {
        constexpr auto result = concat("", "hello");
        CHECK(std::strcmp(result.data(), "hello") == 0);

        constexpr auto result2 = concat("hello", "");
        CHECK(std::strcmp(result2.data(), "hello") == 0);
    }
}

static constexpr container esc_plain("hello");
static constexpr container esc_backslash("a\\b");
static constexpr container esc_quote("a\"b");
static constexpr container esc_nonprint("\x01");
static constexpr container esc_lowchars(" !");
static constexpr container esc_empty("");

TEST_CASE("escape")
{
    SUBCASE("plain printable string unchanged")
    {
        constexpr auto result = escape<esc_plain>();
        CHECK(std::strcmp(result.data(), "hello") == 0);
    }

    SUBCASE("backslash escaped")
    {
        constexpr auto result = escape<esc_backslash>();
        CHECK(std::strcmp(result.data(), "a\\\\b") == 0);
    }

    SUBCASE("double quote escaped")
    {
        constexpr auto result = escape<esc_quote>();
        CHECK(std::strcmp(result.data(), "a\\\"b") == 0);
    }

    SUBCASE("non-printable uses octal")
    {
        constexpr auto result = escape<esc_nonprint>();
        CHECK(std::strcmp(result.data(), "\\001") == 0);
        CHECK(result.size() == 4);
    }

    SUBCASE("null-adjacent chars use octal")
    {
        constexpr auto result = escape<esc_lowchars>();
        CHECK(std::strcmp(result.data(), "\\040\\041") == 0);
    }

    SUBCASE("empty string")
    {
        constexpr auto result = escape<esc_empty>();
        CHECK(result.size() == 0);
        CHECK(std::strcmp(result.data(), "") == 0);
    }
}

TEST_CASE("from_magnitude")
{
    SUBCASE("zero")
    {
        constexpr auto result = from_magnitude<0>();
        CHECK(std::strcmp(result.data(), "0") == 0);
        CHECK(result.size() == 1);
    }

    SUBCASE("single digit")
    {
        constexpr auto result = from_magnitude<7>();
        CHECK(std::strcmp(result.data(), "7") == 0);
    }

    SUBCASE("multi digit")
    {
        constexpr auto result = from_magnitude<12345>();
        CHECK(std::strcmp(result.data(), "12345") == 0);
        CHECK(result.size() == 5);
    }
}

TEST_CASE("from_integral")
{
    SUBCASE("positive")
    {
        constexpr auto result = from_integral<42>();
        CHECK(std::strcmp(result.data(), "42") == 0);
    }

    SUBCASE("zero")
    {
        constexpr auto result = from_integral<0>();
        CHECK(std::strcmp(result.data(), "0") == 0);
    }

    SUBCASE("negative")
    {
        constexpr auto result = from_integral<-1>();
        CHECK(std::strcmp(result.data(), "-1") == 0);
    }

    SUBCASE("large negative")
    {
        constexpr auto result = from_integral<-12345>();
        CHECK(std::strcmp(result.data(), "-12345") == 0);
    }

    SUBCASE("unsigned")
    {
        constexpr auto result = from_integral<255u>();
        CHECK(std::strcmp(result.data(), "255") == 0);
    }
}
