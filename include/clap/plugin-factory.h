#pragma once

#include "plugin.h"

static const CLAP_CONSTEXPR char CLAP_PLUGIN_FACTORY_ID[] = "clap.plugin-factory";

#ifdef __cplusplus
extern "C" {
#endif

// Every methods must be thread-safe.
// It is very important to be able to scan the plugin as quickly as possible.
//
// If the content of the factory may change due to external events, like the user installed
typedef struct clap_plugin_factory {
   /* Get the number of plugins available.
    * [thread-safe] */
   uint32_t (*get_plugin_count)(const struct clap_plugin_factory *factory);

   /* Retrieves a plugin descriptor by its index.
    * Returns null in case of error.
    * The descriptor must not be freed.
    * [thread-safe] */
   const clap_plugin_descriptor_t *(*get_plugin_descriptor)(
      const struct clap_plugin_factory *factory, uint32_t index);

   /* Create a clap_plugin by its plugin_id.
    * The returned pointer must be freed by calling plugin->destroy(plugin);
    * The plugin is not allowed to use the host callbacks in the create method.
    * Returns null in case of error.
    * [thread-safe] */
   const clap_plugin_t *(*create_plugin)(const struct clap_plugin_factory *factory,
                                         const clap_host_t                *host,
                                         const char                       *plugin_id);
} clap_plugin_factory_t;

#ifdef __cplusplus
}
#endif
