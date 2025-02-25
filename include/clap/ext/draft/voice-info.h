#pragma once

#include "../../plugin.h"

// This extensions indicates the number of voices the synthesizer.
// It is useful for the host when performing polyphonic modulations,
// because the host needs its own voice management and should try to follow
// what the plugin is doing:
// - make the host's voice pool coherent with what the plugin has
// - turn the host's voice management to mono when the plugin is mono

static const char CLAP_EXT_VOICE_INFO[] = "clap.voice-info.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_voice_info {
   // voice_count is the current number of voices that the patch can use
   // voice_capacity is the number of voices allocated voices
   // voice_count should not be confused with the number of active voices.
   //
   // 1 <= voice_count <= voice_capacity
   //
   // For example, a synth can have a capacity of 8 voices, but be configured
   // to only use 4 voices: {count: 4, capacity: 8}.
   //
   // If the voice_count is 1, then the synth is working in mono and the host
   // can decide to only use global modulation mapping.
   uint32_t voice_count;
   uint32_t voice_capacity;
} clap_voice_info_t;

typedef struct clap_plugin_voice_info {
   // gets the voice info, returns true on success
   // [main-thread && active]
   bool (*get)(const clap_plugin_t *plugin, clap_voice_info_t *info);
} clap_plugin_voice_info_t;

typedef struct clap_host_voice_info {
   // informs the host that the voice info have changed
   // [main-thread]
   void (*changed)(const clap_host_t *host);
} clap_host_voice_info_t;

#ifdef __cplusplus
}
#endif
