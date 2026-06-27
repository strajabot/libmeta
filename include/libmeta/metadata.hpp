#ifndef LIBMETA_METADATA_HPP
#define LIBMETA_METADATA_HPP

#include "libmeta/settings.hpp"

#if LIBMETA_DEVICE_ENABLE
#include "libmeta/sstring.hpp"
#endif

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

namespace libmeta {

#if LIBMETA_DEVICE_ENABLE
template<size_t align>
constexpr auto
pad_directive()
{
    return sstring::concat(".balign ", sstring::from_integral<align>(), "\n\t");
}
#endif

#if LIBMETA_DEVICE_ENABLE
template<typename T>
constexpr auto
value_directive()
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 ||
                    sizeof(T) == 8,
                  "libmeta: directive: unsupported type size");

    if constexpr (sizeof(T) == 1)
        return sstring::concat(".byte ");
    else if constexpr (sizeof(T) == 2)
        return sstring::concat(".short ");
    else if constexpr (sizeof(T) == 4)
        return sstring::concat(".long ");
    else if constexpr (sizeof(T) == 8)
        return sstring::concat(".quad ");
}
#endif

template<typename T>
struct Integral
{
    static_assert(std::is_integral_v<T>,
                  "libmeta: Integral: only supports integral types");

    static constexpr auto size() { return sizeof(T); }

    static constexpr auto align() { return alignof(T); }

#if LIBMETA_DEVICE_ENABLE
    template<const auto& /* Section*/, T Value>
    static constexpr auto compile()
    {
        constexpr auto string = sstring::from_integral<Value>();
        return sstring::concat(
          pad_directive<alignof(T)>(), value_directive<T>(), string, "\n\t");
    }
#endif
};

struct String
{

    static constexpr auto size() { return sizeof(uintptr_t); }

    static constexpr auto align() { return alignof(uintptr_t); }

#if LIBMETA_DEVICE_ENABLE
    template<const auto& Section, const auto& Value>
    static constexpr auto compile()
    {
        // todo: technically, entsize == 1 can be a wrong assumption
        // clang-format off
        return sstring::concat(
            ".pushsection ", Section, ".strings", ", \"MS\", @progbits, 1\n\t",
            "2: .asciz \"", sstring::escape<Value>(), "\"\n\t",
            ".popsection\n\t",
            pad_directive<alignof(uintptr_t)>(),
            value_directive<uintptr_t>(), "2b", "\n\t"
        );
        // clang-format on
    }
#endif
};

template<const auto& Section, typename... Fields>
static void
handle(void);

template<const auto& Section, typename... Fields>
class Metadata
{

    static constexpr bool check_section()
    {
        if (Section.size() < 2)
            return false;

        if (Section.value[0] != '.')
            return false;

        for (size_t i = 1; i < Section.size(); ++i) {
            const char c = Section.value[i];
            const bool first = i == 1;

            if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') &&
                (first || c < '0' || c > '9') && c != '_' &&
                (first || c != '.'))
                return false;
        }
        return true;
    }

    static_assert(
      check_section(),
      "libmeta: Metadata section name must be \"\\.[a-zA-Z_][a-zA-Z0-9_.]*\"");

    template<typename First, typename... Rest>
    static constexpr size_t size_impl(size_t offset)
    {
        constexpr size_t field_align = First::align();
        offset = (offset + field_align - 1) & ~(field_align - 1);
        offset += First::size();
        if constexpr (sizeof...(Rest) == 0)
            return offset;
        else
            return size_impl<Rest...>(offset);
    }

    static constexpr size_t size()
    {
        if constexpr (sizeof...(Fields) == 0)
            return 0;
        else {
            constexpr size_t raw = size_impl<Fields...>(0);
            constexpr size_t a = align();
            return (raw + a - 1) & ~(a - 1);
        }
    }

    static constexpr size_t align()
    {
        if constexpr (sizeof...(Fields) == 0)
            return 1;
        else
            return std::max({ Fields::align()... });
    }

    static constexpr auto offsets()
    {
        std::array<size_t, sizeof...(Fields)> result{};
        if constexpr (sizeof...(Fields) > 0) {
            constexpr size_t sizes[] = { Fields::size()... };
            constexpr size_t aligns[] = { Fields::align()... };
            result[0] = 0;
            for (size_t i = 1; i < sizeof...(Fields); ++i) {
                size_t end = result[i - 1] + sizes[i - 1];
                size_t a = aligns[i];
                result[i] = (end + a - 1) & ~(a - 1);
            }
        }
        return result;
    }

    static constexpr size_t offset(size_t idx)
    {
        constexpr auto table = offsets();
        return table[idx];
    }

#if LIBMETA_DEVICE_ENABLE
    template<size_t I, auto V, auto... Vs>
    static constexpr auto compile_fields_impl()
    {
        using Field = __type_pack_element<I, Fields...>;
        auto this_field = Field::template compile<Section, V>();
        if constexpr (sizeof...(Vs) == 0) {
            return this_field;
        } else {
            // clang-format off
            return sstring::concat(
                this_field,
                compile_fields_impl<I + 1, Vs...>()
            );
            // clang-format on
        }
    }
#endif

#if LIBMETA_DEVICE_ENABLE
    template<auto... Values>
    static constexpr auto compile_fields()
    {
        if constexpr (sizeof...(Fields) == 0)
            return sstring::container<1>{};
        else
            return compile_fields_impl<0, Values...>();
    }
#endif

#if LIBMETA_DEVICE_ENABLE
    template<auto... Values>
    static constexpr auto compile_struct()
    {
        constexpr auto entsize = sstring::from_integral<size()>();
        // clang-format off
        return sstring::concat(
            ".pushsection ", sstring::concat(Section, ".structs"), ", \"M\", @progbits, ", entsize, "\n\t",
            pad_directive<align()>(),
            "1:\n\t",
            compile_fields<Values...>(),
            pad_directive<align()>(),
            ".popsection\n\t"
        );
        // clang-format on
    }
#endif

  public:
#if LIBMETA_DEVICE_ENABLE
    template<auto... Values>
    static constexpr auto compile()
    {
        constexpr auto entsize = sstring::from_integral<sizeof(uintptr_t)>();

        // clang-format off
        return sstring::concat(
            ".pushsection ", Section, ".%cc0, ", "\"MG\", @progbits, ", entsize, ", %cc0, comdat\n\t",
            compile_struct<Values...>(),
            pad_directive<alignof(uintptr_t)>(),
            ".global %cc0", "\n\t"
            "%cc0: ", value_directive<uintptr_t>(), "1b", "\n\t",
            pad_directive<alignof(uintptr_t)>(), "\n\t",
            ".popsection\n\t"
        );
        // clang-format on
    }
#endif

#if LIBMETA_DEVICE_ENABLE
    template<auto... Values>
    static uintptr_t insert()
    {
        static_assert(sizeof...(Values) == sizeof...(Fields),
                      "libmeta: Metadata::insert: number of values must match "
                      "number of fields");

        asm volatile((compile<Values...>())::":"(handle<Section, Fields...>));

        return reinterpret_cast<uintptr_t>(&handle<Section, Fields...>);
    }
#endif
};

} // namespace libmeta

#endif // !LIBMETA_METADATA_HPP
