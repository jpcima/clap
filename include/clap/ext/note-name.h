#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_NOTE_NAME[] = "clap.note-name";

typedef struct clap_note_name {
   char    name[CLAP_NAME_SIZE];
   int16_t port;    // -1 for every ports
   int16_t key;     // -1 for every keys
   int16_t channel; // -1 for every channels
} clap_note_name_t;

typedef struct clap_plugin_note_name {
   // Return the number of note names
   // [main-thread]
   uint32_t (*count)(const clap_plugin_t *plugin);

   // Returns true on success and stores the result into note_name
   // [main-thread]
   bool (*get)(const clap_plugin_t *plugin, uint32_t index, clap_note_name_t *note_name);
} clap_plugin_note_name;

typedef struct clap_host_note_name {
   // Informs the host that the note names has changed.
   // [main-thread]
   void (*changed)(const clap_host_t *host);
} clap_host_note_name_t;

#ifdef __cplusplus
}
#endif
