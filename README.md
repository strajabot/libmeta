# libmeta

```cpp
#include <cstdint>
#include <libmeta/metadata.hpp>

static constexpr libmeta::sstring::container section(".meta.profiler");
using ProfilerMetadata = libmeta::Metadata<section,
                                           libmeta::String,            // Marker
                                           libmeta::String,            // File
                                           libmeta::Integral<uint64_t> // Line
                                           >;

uintptr_t profiler_timestamp(uintptr_t token)
{
    // 1. Read WALL TIME
    // 2. Store { token, timestamp } entry to buffer
    // 3. Host reads entry from buffer
    // 4. Host decodes Metadata based on the token and elf file
    // 5. Host correlates Metadata with timestamp
    // 6. Host prints MARKER ( FILE:LINE ) hit at TIMESTAMP
    return token;
}

#define PROFILER_TIMESTAMP(marker)                                             \
    profiler_timestamp(                                                        \
        []()                                                                   \
        {                                                                      \
            static constexpr libmeta::sstring::container meta_marker(marker);  \
            static constexpr libmeta::sstring::container meta_file(__FILE__);  \
            static constexpr uint64_t meta_line = __LINE__;                    \
            return ProfilerMetadata::insert<meta_marker, meta_file,            \
                                            meta_line>();                      \
        }())

int main()
{
    volatile uintptr_t val;
    val = PROFILER_TIMESTAMP("MAIN");
    return 0;
}
```