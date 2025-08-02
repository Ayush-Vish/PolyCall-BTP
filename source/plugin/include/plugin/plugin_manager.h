

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H 1

/* -- Headers -- */

#include <plugin/plugin_api.h>

#include <plugin/plugin_impl.h>
#include <plugin/plugin_loader.h>

#include <adt/adt_set.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Macros -- */

#define plugin_manager_declare(name) \
	struct plugin_manager_type name = { 0 }

#define plugin_manager_impl_type(manager, type_name) \
	((type_name)plugin_manager_impl(manager))

/* -- Forward Declarations -- */

struct plugin_manager_interface_type;

/* -- Type Definitions -- */

typedef struct plugin_manager_interface_type *plugin_manager_interface;

/* -- Declarations -- */

struct plugin_manager_type
{
	char *name;						/* Defines the plugin manager name (a pointer to a static string defining the manager type) */
	char *library_path;				/* Defines current library path */
	set plugins;					/* Contains the plugins indexed by name */
	plugin_manager_interface iface; /* Hooks into the plugin manager from the implementation */
	void *impl;						/* User defined plugin manager data */
	plugin_loader l;				/* Pointer to the loader, it defines the low level details for loading and unloading libraries */
};

struct plugin_manager_interface_type
{
	void (*clear)(plugin_manager, plugin);	 /* Hook for clearing the plugin implementation */
	void (*destroy)(plugin_manager, void *); /* Hook for destroying the plugin manager implementation */
};

/* -- Methods  -- */

PLUGIN_API int plugin_manager_initialize(plugin_manager manager, const char *name, const char *environment_library_path, const char *default_library_path, plugin_manager_interface iface, void *impl);

PLUGIN_API const char *plugin_manager_name(plugin_manager manager);

PLUGIN_API char *plugin_manager_library_path(plugin_manager manager);

PLUGIN_API void *plugin_manager_impl(plugin_manager manager);

PLUGIN_API size_t plugin_manager_size(plugin_manager manager);

PLUGIN_API int plugin_manager_register(plugin_manager manager, plugin p);

PLUGIN_API plugin plugin_manager_create(plugin_manager manager, const char *name, void *impl, void (*dtor)(plugin));

PLUGIN_API plugin plugin_manager_get(plugin_manager manager, const char *name);

PLUGIN_API int plugin_manager_clear(plugin_manager manager, plugin p);

PLUGIN_API void plugin_manager_destroy(plugin_manager manager);

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_MANAGER_H */
