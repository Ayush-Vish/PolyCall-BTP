

#ifndef PLUGIN_DESCRIPTOR_H
#define PLUGIN_DESCRIPTOR_H 1

/* -- Headers -- */

#include <plugin/plugin_api.h>

#include <dynlink/dynlink.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Declarations  -- */

struct plugin_descriptor_type
{
	dynlink handle;
	char *library_name;
	char *symbol_iface_name;
	void *(*iface_singleton)(void);
};

/* -- Type Declarations  -- */

typedef struct plugin_descriptor_type *plugin_descriptor;

/* -- Methods -- */

PLUGIN_API plugin_descriptor plugin_descriptor_create(char *path, char *library_name, char *symbol_iface_name);

PLUGIN_API void plugin_descriptor_destroy(plugin_descriptor descriptor);

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_DESCRIPTOR_H */
