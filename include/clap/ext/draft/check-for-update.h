#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_CHECK_FOR_UPDATE[] = "clap.check_for_update.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_check_for_update_info {
   const char *version;
   const char *release_date; // YYYY-MM-DD
   const char *url;

   bool is_stable;
} clap_check_for_update_info_t;

typedef struct clap_plugin_check_for_update {
   // [main-thread]
   void (*check)(const clap_host_t *host, bool include_beta);
} clap_plugin_check_for_update;

typedef struct clap_host_check_for_update {
   // [main-thread]
   void (*on_new_version)(const clap_host_t *host, const clap_check_for_update_info_t *update_info);
} clap_host_check_for_update_t;

#ifdef __cplusplus
}
#endif