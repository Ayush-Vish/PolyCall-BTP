

#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H 1

/* -- Headers -- */

#include <plugin/plugin_api.h>

#include <plugin/plugin_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Forward Declarations  -- */

struct plugin_manager_type;
struct plugin_loader_type;

/* -- Type Declarations  -- */

typedef struct plugin_manager_type *plugin_manager;
typedef struct plugin_loader_type *plugin_loader;

/* -- Methods  -- */

PLUGIN_API plugin_loader plugin_loader_create(plugin_manager manager);

PLUGIN_API plugin plugin_loader_load(plugin_loader l, const char *name, void *impl, void (*dtor)(plugin));

PLUGIN_API void plugin_loader_destroy(plugin_loader l);

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_LOADER_H */
