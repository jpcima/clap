#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TAIL[] = "clap.tail";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_tail {
   // Returns tail length in samples.
   // [main-thread,audio-thread]
   uint32_t (*get)(const clap_plugin_t *plugin);
} clap_plugin_tail_t;

typedef struct clap_host_tail {
   // Tell the host that the tail has changed.
   // [audio-thread]
   void (*changed)(const clap_host_t *host);
} clap_host_tail_t;

#ifdef __cplusplus
}
#endif
