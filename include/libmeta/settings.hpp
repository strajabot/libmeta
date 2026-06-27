#ifndef LIBMETA_SETTINGS_HPP
#define LIBMETA_SETTINGS_HPP

#include <limits.h>
#include <type_traits>

#ifndef LIBMETA_SETTING_DEVICE
#define LIBMETA_SETTING_DEVICE 0
#endif

static_assert(std::is_same_v<decltype(LIBMETA_SETTING_DEVICE), int>,
              "libmeta: \"LIBMETA_SETTING_DEVICE\" must be an integer");
static_assert(LIBMETA_SETTING_DEVICE == 0 || LIBMETA_SETTING_DEVICE == 1,
              "libmeta: \"LIBMETA_SETTING_DEVICE\" must be 0 or 1");

#define LIBMETA_DEVICE_ENABLE LIBMETA_SETTING_DEVICE

#ifndef LIBMETA_SETTING_HOST
#define LIBMETA_SETTING_HOST 0
#endif

static_assert(std::is_same_v<decltype(LIBMETA_SETTING_HOST), int>,
              "libmeta: \"LIBMETA_SETTING_HOST\" must be an integer");
static_assert(LIBMETA_SETTING_HOST == 0 || LIBMETA_SETTING_HOST == 1,
              "libmeta: \"LIBMETA_SETTING_HOST\" must be 0 or 1");

static_assert(!LIBMETA_SETTING_HOST,
              "libmeta: \"LIBMETA_SETTING_HOST\" is currently unimplemented");

#define LIBMETA_HOST_ENABLE LIBMETA_SETTING_HOST

#if LIBMETA_DEVICE_ENABLE

// preconditions
static_assert(CHAR_BIT == 8, "libmeta: fatal: only 8bit chars supported");

#if !defined(__GNUG__)
static_assert(false,
              "libmeta: fatal: only GCC toolchain is supported currently");
#endif

#if !defined(__ELF__)
static_assert(false, "libmeta: fatal: ELF is the only supported object format");
#endif

#if !defined(__x86_64__) && !defined(__riscv)
static_assert(false, "libmeta: fatal: unsupported architecture");
#endif

#endif // LIBMETA_DEVICE_ENABLE

#endif // LIBMETA_SETTINGS_HPP
