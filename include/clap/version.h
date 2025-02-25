#pragma once

#include "private/macros.h"
#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_version {
   // This is the major ABI and API design
   // Version 0.X.Y correspond to the development stage, API and ABI are not stable
   // Version 1.X.Y correspont to the release stage, API and ABI are stable
   uint32_t major;
   uint32_t minor;
   uint32_t revision;
} clap_version_t;

#ifdef __cplusplus
}
#endif

static CLAP_CONSTEXPR const uint32_t CLAP_VERSION_MAJOR = 0;
static CLAP_CONSTEXPR const uint32_t CLAP_VERSION_MINOR = 24;
static CLAP_CONSTEXPR const uint32_t CLAP_VERSION_REVISION = 1;

static CLAP_CONSTEXPR const clap_version_t CLAP_VERSION = {
   CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION};

// For version 0, we require the same minor version because
// we may still break the ABI at this point
CLAP_NODISCARD static inline CLAP_CONSTEXPR bool
clap_version_is_compatible(const clap_version_t v) {
   return v.major == CLAP_VERSION_MAJOR && v.minor == CLAP_VERSION_MINOR;
}
