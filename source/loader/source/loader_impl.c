#include <loader/loader_impl.h>
#include <loader/loader_manager_impl.h>

#include <reflect/reflect_context.h>
#include <reflect/reflect_type.h>

#include <adt/adt_hash.h>
#include <adt/adt_set.h>
#include <adt/adt_vector.h>

#include <format/format_print.h>

#include <log/log.h>

#include <configuration/configuration.h>
#include <configuration/configuration_object.h>

#include <portability/portability_library_path.h>

#include <stdlib.h>
#include <string.h>


#define loader_iface(l) \
	plugin_iface_type(l, loader_impl_interface)


#define LOADER_IMPL_FUNCTION_INIT "__metacall_initialize__"
#define LOADER_IMPL_FUNCTION_FINI "__metacall_finalize__"

#if defined(WIN32) || defined(_WIN32) || \
	defined(__CYGWIN__) || defined(__CYGWIN32__) || \
	defined(__MINGW32__) || defined(__MINGW64__)

	#define LOADER_PATH_DELIMITER ';'

#else
	#define LOADER_PATH_DELIMITER ':'
#endif


struct loader_handle_impl_type;


typedef struct loader_handle_impl_type *loader_handle_impl;

struct loader_impl_type
{
	plugin p;					  
	int init;					  
	set handle_impl_path_map;	  
	set handle_impl_map;		   
	vector handle_impl_init_order;
	loader_impl_data data;		   
	context ctx;				  
	set type_info_map;			   
	value options;				   
	set exec_path_map;			   
	configuration config;		   
	set library_map;			   
	detour d;					   
	set detour_map;				   
};

struct loader_handle_impl_type
{
	uintptr_t magic;			 
	loader_impl impl;			 
	loader_impl_interface iface; 
	loader_path path;			 
	loader_handle module;		
	context ctx;				 
	int populated;				 
	vector populated_handles;	 
};

static loader_impl loader_impl_allocate(const loader_tag tag);

static void loader_impl_configuration_execution_paths(loader_impl_interface iface, loader_impl impl);

static int loader_impl_dependencies_self_list(const char *library, void *data);

static int loader_impl_dependencies_self_find(loader_impl impl, const char *key_str, vector dependencies_self);

static int loader_impl_dependencies_load(loader_impl impl, const char *key_str, value *paths_array, size_t paths_size);

#if defined(WIN32) || defined(_WIN32)
static void loader_impl_dependencies_search_paths(loader_impl impl, const loader_tag tag);
#endif

static configuration loader_impl_initialize_configuration(const loader_tag tag);

static int loader_impl_initialize_registered(plugin_manager manager, plugin p);

static loader_handle_impl loader_impl_load_handle(loader_impl impl, loader_impl_interface iface, loader_handle module, const char *path, size_t size);

static int loader_impl_handle_init(loader_impl impl, const char *path, loader_handle_impl handle_impl, void **handle_ptr, int populated);

static int loader_impl_handle_register(plugin_manager manager, loader_impl impl, const char *path, loader_handle_impl handle_impl, void **handle_ptr);

static size_t loader_impl_handle_name(plugin_manager manager, const loader_path path, loader_path result);

static int loader_impl_function_hook_call(context ctx, const char func_name[]);

static value loader_impl_metadata_handle_name(loader_handle_impl handle_impl);

static value loader_impl_metadata_handle_context(loader_handle_impl handle_impl);

static value loader_impl_metadata_handle(loader_handle_impl handle_impl);

static void loader_impl_destroy_handle(loader_handle_impl handle_impl);


static const char loader_handle_impl_magic_alloc[] = "loader_handle_impl_magic_alloc";
static const char loader_handle_impl_magic_free[] = "loader_handle_impl_magic_free";


loader_impl loader_impl_allocate(const loader_tag tag)
{
	loader_impl impl = malloc(sizeof(struct loader_impl_type));

	if (impl == NULL)
	{
		goto alloc_error;
	}

	memset(impl, 0, sizeof(struct loader_impl_type));

	impl->handle_impl_path_map = set_create(&hash_callback_str, &comparable_callback_str);

	if (impl->handle_impl_path_map == NULL)
	{
		goto alloc_handle_impl_path_map_error;
	}

	impl->handle_impl_map = set_create(&hash_callback_ptr, &comparable_callback_ptr);

	if (impl->handle_impl_map == NULL)
	{
		goto alloc_handle_impl_map_error;
	}

	impl->handle_impl_init_order = vector_create_type(loader_handle_impl);

	if (impl->handle_impl_init_order == NULL)
	{
		goto alloc_handle_impl_init_order_error;
	}

	impl->type_info_map = set_create(&hash_callback_str, &comparable_callback_str);

	if (impl->type_info_map == NULL)
	{
		goto alloc_type_info_map_error;
	}

	impl->ctx = context_create(tag);

	if (impl->ctx == NULL)
	{
		goto alloc_ctx_error;
	}

	impl->exec_path_map = set_create(&hash_callback_str, &comparable_callback_str);

	if (impl->exec_path_map == NULL)
	{
		goto alloc_exec_path_map_error;
	}

	impl->library_map = set_create(&hash_callback_str, &comparable_callback_str);

	if (impl->library_map == NULL)
	{
		goto alloc_library_map_error;
	}

	impl->detour_map = set_create(&hash_callback_str, &comparable_callback_str);

	if (impl->detour_map == NULL)
	{
		goto alloc_detour_map_error;
	}

	return impl;

alloc_detour_map_error:
	set_destroy(impl->library_map);
alloc_library_map_error:
	set_destroy(impl->exec_path_map);
alloc_exec_path_map_error:
	context_destroy(impl->ctx);
alloc_ctx_error:
	set_destroy(impl->type_info_map);
alloc_type_info_map_error:
	vector_destroy(impl->handle_impl_init_order);
alloc_handle_impl_init_order_error:
	set_destroy(impl->handle_impl_map);
alloc_handle_impl_map_error:
	set_destroy(impl->handle_impl_path_map);
alloc_handle_impl_path_map_error:
	free(impl);
alloc_error:
	return NULL;
}

loader_impl loader_impl_create_host(const loader_tag tag)
{
	loader_impl impl = loader_impl_allocate(tag);

	if (impl == NULL)
	{
		return NULL;
	}

	impl->init = 0; 
	impl->options = NULL;

	return impl;
}

void loader_impl_attach(loader_impl impl, plugin p)
{
	if (impl != NULL && p != NULL)
	{
		impl->p = p;
	}
}

plugin loader_impl_plugin(loader_impl impl)
{
	if (impl != NULL)
	{
		return impl->p;
	}

	return NULL;
}

void loader_impl_configuration_execution_paths(loader_impl_interface iface, loader_impl impl)
{
	value execution_paths_value = configuration_value_type(impl->config, "execution_paths", TYPE_ARRAY);

	if (execution_paths_value != NULL)
	{
		size_t size = value_type_count(execution_paths_value);
		value *execution_paths_array = value_to_array(execution_paths_value);
		size_t iterator;

		for (iterator = 0; iterator < size; ++iterator)
		{
			if (value_type_id(execution_paths_array[iterator]) == TYPE_STRING)
			{
				loader_path execution_path;

				configuration_object_child_path(impl->config, execution_paths_array[iterator], execution_path);

				if (iface->execution_path(impl, execution_path) != 0)
				{
					log_write("metacall", LOG_LEVEL_ERROR, "Failed to load execution path %s in configuration %s", execution_path, configuration_object_name(impl->config));
				}
			}
		}
	}
}

int loader_impl_dependencies_self_list(const char *library, void *data)
{
	vector dependencies_self = (vector)data;

	vector_push_back_empty(dependencies_self);

	strncpy(vector_back(dependencies_self), library, strnlen(library, PORTABILITY_PATH_SIZE) + 1);

	return 0;
}

int loader_impl_dependencies_self_find(loader_impl impl, const char *key_str, vector dependencies_self)
{
	size_t iterator, size = vector_size(dependencies_self);
	char library_self_name[PORTABILITY_PATH_SIZE];
	dynlink handle;

	for (iterator = 0; iterator < size; ++iterator)
	{
		const char *library_self = vector_at(dependencies_self, iterator);

		portability_path_get_fullname(library_self, strnlen(library_self, PORTABILITY_PATH_SIZE) + 1, library_self_name, PORTABILITY_PATH_SIZE);

		if (strstr(library_self_name, key_str) != NULL)
		{
			handle = dynlink_load_absolute(library_self, DYNLINK_FLAGS_BIND_LAZY | DYNLINK_FLAGS_BIND_GLOBAL);

			goto dependencies_map_insert;
		}
	}

	handle = dynlink_load_self(DYNLINK_FLAGS_BIND_LAZY | DYNLINK_FLAGS_BIND_GLOBAL);

dependencies_map_insert:

	if (handle != NULL && set_insert(impl->library_map, (const set_key)key_str, (set_value)handle) == 0)
	{
		return 0;
	}

	dynlink_unload(handle);

	return 1;
}

int loader_impl_dependencies_load(loader_impl impl, const char *key_str, value *paths_array, size_t paths_size)
{
	size_t iterator;

	for (iterator = 0; iterator < paths_size; ++iterator)
	{
		if (value_type_id(paths_array[iterator]) == TYPE_STRING)
		{
			const char *library_path = value_to_string(paths_array[iterator]);

			if (library_path != NULL)
			{
				dynlink handle = dynlink_load_absolute(library_path, DYNLINK_FLAGS_BIND_LAZY | DYNLINK_FLAGS_BIND_GLOBAL);

				if (handle != NULL && set_insert(impl->library_map, (const set_key)key_str, (set_value)handle) == 0)
				{
					return 0;
				}

				dynlink_unload(handle);
			}
		}
	}

	return 1;
}

#if defined(WIN32) || defined(_WIN32)
void loader_impl_dependencies_search_paths(loader_impl impl, const loader_tag tag)
{
	value search_paths_value = configuration_value_type(impl->config, "search_paths", TYPE_ARRAY);
	if (search_paths_value != NULL)
	{
		size_t size = value_type_count(search_paths_value);
		value *search_paths_array = value_to_array(search_paths_value);
		size_t iterator;

		for (iterator = 0; iterator < size; ++iterator)
		{
			if (value_type_id(search_paths_array[iterator]) == TYPE_STRING)
			{
				const char *key_str = value_to_string(search_paths_array[iterator]);

				if (SetDllDirectoryA(key_str) == FALSE)
				{
					log_write("metacall", LOG_LEVEL_ERROR, "Failed to register the DLL directory %s in loader '%s'; dependencies with other dependant DLLs may fail to load", key_str, tag);
				}
			}
		}
	}
}
#endif

int loader_impl_dependencies(loader_impl impl, detour d, const loader_tag tag)
{
	
	value dependencies_value = configuration_value_type(impl->config, "dependencies", TYPE_MAP);
	
	impl->d = d;

#if defined(WIN32) || defined(_WIN32)
	loader_impl_dependencies_search_paths(impl, tag);
#endif

	if (dependencies_value != NULL)
	{
		size_t size = value_type_count(dependencies_value);
		value *dependencies_map = value_to_map(dependencies_value);
		vector dependencies_self = NULL;
		size_t iterator;
		const int host = loader_impl_get_option_host(impl);

		if (host == 1)
		{
			dependencies_self = vector_create(sizeof(char) * PORTABILITY_PATH_SIZE);

			if (dependencies_self == NULL)
			{
				return 1;
			}

			if (portability_library_path_list(&loader_impl_dependencies_self_list, (void *)dependencies_self) != 0)
			{
				vector_destroy(dependencies_self);
				return 1;
			}
		}

		for (iterator = 0; iterator < size; ++iterator)
		{
			if (value_type_id(dependencies_map[iterator]) == TYPE_ARRAY)
			{
				value *library_tuple = value_to_array(dependencies_map[iterator]);

				if (value_type_id(library_tuple[0]) == TYPE_STRING)
				{
					const char *key_str = value_to_string(library_tuple[0]);

					if (host == 0)
					{
						if (value_type_id(library_tuple[1]) == TYPE_ARRAY)
						{
							value *paths_array = value_to_array(library_tuple[1]);
							size_t paths_size = value_type_count(library_tuple[1]);

							if (loader_impl_dependencies_load(impl, key_str, paths_array, paths_size) != 0)
							{
								log_write("metacall", LOG_LEVEL_ERROR, "Failed to load dependency '%s' from loader '%s' configuration", key_str, tag);
								return 1;
							}
						}
					}
					else
					{
						if (loader_impl_dependencies_self_find(impl, key_str, dependencies_self) != 0)
						{
							log_write("metacall", LOG_LEVEL_ERROR, "Failed to load dependency '%s' from loader '%s' as a host", key_str, tag);
							vector_destroy(dependencies_self);
							return 1;
						}
					}
				}
			}
		}

		vector_destroy(dependencies_self);
	}

	return 0;
}

int loader_impl_link(plugin p, loader_impl impl)
{
	plugin_descriptor desc = plugin_desc(p);

	
#if defined(WIN32) || defined(_WIN32)
	if (loader_impl_get_option_host(impl) == 1)
	{
		
		detour_handle loader_handle = detour_load_handle(impl->d, desc->handle);

		if (loader_handle != NULL)
		{
			unsigned int position = 0;
			const char *name = NULL;
			void (**addr)(void) = NULL;

			while (detour_enumerate(impl->d, loader_handle, &position, &name, &addr) == 0)
			{
				struct set_iterator_type it;

				for (set_iterator_begin(&it, impl->library_map); set_iterator_end(&it) != 0; set_iterator_next(&it))
				{
					dynlink library_handle = set_iterator_value(&it);

					if (library_handle != NULL && library_handle != desc->handle)
					{
						dynlink_symbol_addr symbol = NULL;

						if (dynlink_symbol(library_handle, name, &symbol) == 0 && symbol != NULL)
						{
							if (detour_replace(impl->d, loader_handle, name, symbol, NULL) == 0)
							{
								break;
							}
						}
					}
				}
			}

			detour_unload(impl->d, loader_handle);
		}
	}
#endif

	if (set_insert(impl->library_map, (set_key)desc->library_name, (set_value)desc->handle) != 0)
	{
		return 1;
	}

	return 0;
}

dynlink loader_impl_dependency(loader_impl impl, const char *library)
{
	dynlink library_handle = set_get(impl->library_map, (const set_key)library);

	return library_handle;
}

detour_handle loader_impl_detour(loader_impl impl, const char *library, int (*load_cb)(detour, detour_handle))
{
	detour_handle handle = set_get(impl->detour_map, (const set_key)library);

	if (handle == NULL)
	{
		dynlink library_handle = set_get(impl->library_map, (const set_key)library);

		if (library_handle == NULL)
		{
			return NULL;
		}

		handle = detour_load_handle(impl->d, library_handle);

		if (handle == NULL)
		{
			return NULL;
		}

		if (load_cb(impl->d, handle) != 0)
		{
			detour_unload(impl->d, handle);
			return NULL;
		}

		if (set_insert(impl->detour_map, (set_key)library, handle) != 0)
		{
			detour_unload(impl->d, handle);
			return NULL;
		}
	}

	return handle;
}

configuration loader_impl_initialize_configuration(const loader_tag tag)
{
	static const char configuration_key_suffix[] = "_loader";
#define CONFIGURATION_KEY_SIZE ((size_t)sizeof(configuration_key_suffix) + LOADER_TAG_SIZE - 1)
	char configuration_key[CONFIGURATION_KEY_SIZE];

	size_t tag_size = strnlen(tag, LOADER_TAG_SIZE) + 1;

	strncpy(configuration_key, tag, tag_size);

	strncat(configuration_key, configuration_key_suffix, CONFIGURATION_KEY_SIZE - tag_size);
#undef CONFIGURATION_KEY_SIZE

	return configuration_scope(configuration_key);
}

int loader_impl_initialize_registered(plugin_manager manager, plugin p)
{
	loader_manager_impl manager_impl = plugin_manager_impl_type(manager, loader_manager_impl);
	size_t iterator, size = vector_size(manager_impl->initialization_order);

	for (iterator = 0; iterator < size; ++iterator)
	{
		loader_initialization_order order = vector_at(manager_impl->initialization_order, iterator);

		if (order->p == p)
		{
			return 0;
		}
	}

	return 1;
}

int loader_impl_initialize(plugin_manager manager, plugin p, loader_impl impl)
{
	static const char loader_library_path[] = "loader_library_path";
	value loader_library_path_value = NULL;
	char *library_path = NULL;
	vector script_paths, paths;

	if (impl->init == 0)
	{
		return 0;
	}

	library_path = plugin_manager_library_path(manager);

	if (impl->config != NULL && configuration_value_type(impl->config, loader_library_path, TYPE_STRING) == NULL)
	{
		loader_library_path_value = value_create_string(library_path, strnlen(library_path, LOADER_PATH_SIZE));
		configuration_define(impl->config, loader_library_path, loader_library_path_value);
	}

	impl->data = loader_iface(p)->initialize(impl, impl->config);

	if (impl->config != NULL && loader_library_path_value != NULL)
	{
		configuration_undefine(impl->config, loader_library_path);
		value_type_destroy(loader_library_path_value);
	}

	if (impl->data == NULL)
	{
		log_write("metacall", LOG_LEVEL_ERROR, "Loader (%s) returned NULL value on the initialization", plugin_name(p));
		return 1;
	}

	if (loader_impl_initialize_registered(manager, p) != 0)
	{
		log_write("metacall", LOG_LEVEL_ERROR, "Loader (%s) was not registered in the initialization list, "
											   "in order to solve this problem it must call to the function 'loader_initialization_register' "
											   "at the end of the initialization (i.e %s_loader_impl_initialize)",
			plugin_name(p), plugin_name(p));
		return 1;
	}

	impl->init = 0;

	if (impl->config != NULL)
	{
		loader_impl_configuration_execution_paths(loader_iface(p), impl);
	}

	loader_library_path_value = configuration_value_type(impl->config, loader_library_path, TYPE_STRING);

	if (loader_library_path_value != NULL)
	{
		library_path = value_to_string(loader_library_path_value);
	}

	if (loader_impl_execution_path(p, impl, library_path) != 0)
	{
		log_write("metacall", LOG_LEVEL_ERROR, "Error when loading path %s", library_path);
	}

	script_paths = plugin_manager_impl_type(manager, loader_manager_impl)->script_paths;

	if (script_paths != NULL)
	{
		size_t iterator, size = vector_size(script_paths);

		for (iterator = 0; iterator < size; ++iterator)
		{
			char *path = vector_at_type(script_paths, iterator, char *);

			if (loader_impl_execution_path(p, impl, path) != 0)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Loader (%s) failed to define execution path: %s", plugin_name(p), path);
			}
		}
	}

	paths = set_get(impl->exec_path_map, (const set_key)plugin_name(p));

	if (paths != NULL)
	{
		size_t iterator, size = vector_size(paths);

		for (iterator = 0; iterator < size; ++iterator)
		{
			char *path = vector_at(paths, iterator);

			if (loader_impl_execution_path(p, impl, path) != 0)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Loader (%s) failed to load path: %s", plugin_name(p), path);
			}
		}
	}

	return 0;
}

int loader_impl_is_initialized(loader_impl impl)
{
	return impl->init;
}

loader_impl loader_impl_create(const loader_tag tag)
{
	loader_impl impl = loader_impl_allocate(tag);

	if (impl == NULL)
	{
		return NULL;
	}

	impl->init = 1;
	impl->options = NULL;

	impl->config = loader_impl_initialize_configuration(tag);

	return impl;
}

loader_impl_data loader_impl_get(loader_impl impl)
{
	if (impl != NULL)
	{
		return impl->data;
	}

	return NULL;
}

value loader_impl_get_value(loader_impl impl, const char *name)
{
	context ctx = loader_impl_context(impl);
	scope sp = context_scope(ctx);
	return scope_get(sp, name);
}

context loader_impl_context(loader_impl impl)
{
	if (impl != NULL)
	{
		return impl->ctx;
	}

	return NULL;
}

type loader_impl_type(loader_impl impl, const char *name)
{
	if (impl != NULL && impl->type_info_map != NULL && name != NULL)
	{
		return (type)set_get(impl->type_info_map, (const set_key)name);
	}

	return NULL;
}

int loader_impl_type_define(loader_impl impl, const char *name, type t)
{
	if (impl != NULL && impl->type_info_map != NULL && name != NULL)
	{
		return set_insert(impl->type_info_map, (const set_key)name, (set_value)t);
	}

	return 1;
}

loader_handle_impl loader_impl_load_handle(loader_impl impl, loader_impl_interface iface, loader_handle module, const char *path, size_t size)
{
	loader_handle_impl handle_impl = malloc(sizeof(struct loader_handle_impl_type));

	if (handle_impl == NULL)
	{
		goto alloc_loader_handle_impl_error;
	}

	handle_impl->impl = impl;
	handle_impl->iface = iface;
	strncpy(handle_impl->path, path, size);
	handle_impl->module = module;
	handle_impl->ctx = context_create(handle_impl->path);

	if (handle_impl->ctx == NULL)
	{
		goto alloc_context_error;
	}

	handle_impl->populated_handles = vector_create_type(loader_handle_impl);

	if (handle_impl->populated_handles == NULL)
	{
		goto alloc_populated_handles_error;
	}

	handle_impl->magic = (uintptr_t)loader_handle_impl_magic_alloc;

	return handle_impl;

alloc_populated_handles_error:
	context_destroy(handle_impl->ctx);
alloc_context_error:
	handle_impl->magic = (uintptr_t)loader_handle_impl_magic_free;
	free(handle_impl);
alloc_loader_handle_impl_error:
	return NULL;
}

void loader_impl_destroy_handle(loader_handle_impl handle_impl)
{
	if (handle_impl != NULL)
	{
		static const char func_fini_name[] = LOADER_IMPL_FUNCTION_FINI;
		size_t iterator;

		if (handle_impl->impl->init == 0)
		{

			if (loader_impl_function_hook_call(handle_impl->ctx, func_fini_name) != 0)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Error when calling destructor from handle impl: %p (%s)", (void *)handle_impl, func_fini_name);
			}

			if (handle_impl->module != NULL && handle_impl->iface->clear(handle_impl->impl, handle_impl->module) != 0)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Error when clearing handle impl: %p", (void *)handle_impl);
			}
		}

		if (handle_impl->populated == 0)
		{
			context_remove(handle_impl->impl->ctx, handle_impl->ctx);
		}

		for (iterator = 0; iterator < vector_size(handle_impl->populated_handles); ++iterator)
		{
			loader_handle_impl populated_handle_impl = vector_at_type(handle_impl->populated_handles, iterator, loader_handle_impl);

			if (populated_handle_impl->populated == 0)
			{
				context_remove(populated_handle_impl->impl->ctx, handle_impl->ctx);
			}

			context_remove(populated_handle_impl->ctx, handle_impl->ctx);
		}

		context_destroy(handle_impl->ctx);
		vector_destroy(handle_impl->populated_handles);
		handle_impl->magic = (uintptr_t)loader_handle_impl_magic_free;

		free(handle_impl);
	}
}

int loader_impl_execution_path(plugin p, loader_impl impl, const loader_path path)
{
	if (impl != NULL)
	{
		if (impl->init == 0)
		{
			loader_impl_interface iface = loader_iface(p);

			if (iface != NULL)
			{
				return iface->execution_path(impl, path);
			}
		}
		else
		{
			vector paths = set_get(impl->exec_path_map, (const set_key)plugin_name(p));

			if (paths == NULL)
			{
				paths = vector_create(sizeof(char) * LOADER_PATH_SIZE);

				if (paths == NULL)
				{
					return 1;
				}

				if (set_insert(impl->exec_path_map, (set_key)plugin_name(p), paths) != 0)
				{
					vector_destroy(paths);

					return 1;
				}
			}

			vector_push_back(paths, (void *)path);

			return 0;
		}
	}

	return 1;
}

int loader_impl_function_hook_call(context ctx, const char func_name[])
{
	scope sp = context_scope(ctx);

	value val = scope_get(sp, func_name);

	function func_init = NULL;

	if (val != NULL)
	{
		func_init = value_to_function(val);
	}

	if (func_init != NULL)
	{
		void *null_args[1] = { NULL };

		function_return ret = function_call(func_init, null_args, 0);

		if (ret != NULL)
		{
			int result = value_to_int(ret);

			value_destroy(ret);

			return result;
		}
	}

	return 0;
}

int loader_impl_handle_init(loader_impl impl, const char *path, loader_handle_impl handle_impl, void **handle_ptr, int populated)
{
	static const char func_init_name[] = LOADER_IMPL_FUNCTION_INIT;

	int result = loader_impl_function_hook_call(impl->ctx, func_init_name);

	handle_impl->populated = populated;

	if (result != 0)
	{
		log_write("metacall", LOG_LEVEL_ERROR, "Error when calling to init hook function (" LOADER_IMPL_FUNCTION_INIT ") of handle: %s", path);
	}

	if (handle_ptr != NULL)
	{
		*handle_ptr = handle_impl;
	}

	return result;
}

int loader_impl_handle_register(plugin_manager manager, loader_impl impl, const char *path, loader_handle_impl handle_impl, void **handle_ptr)
{
	
	if (handle_ptr == NULL)
	{
		struct set_iterator_type it;

		for (set_iterator_begin(&it, manager->plugins); set_iterator_end(&it) != 0; set_iterator_next(&it))
		{
			plugin p = set_iterator_value(&it);
			loader_impl other_impl = plugin_impl_type(p, loader_impl);
			char *duplicated_key = NULL;
			if (context_contains(other_impl->ctx, handle_impl->ctx, &duplicated_key) == 0 && duplicated_key != NULL)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Duplicated symbol found named '%s' already defined in the global scope by handle: %s", duplicated_key, path);
				return 1;
			}
		}

		if (context_append(impl->ctx, handle_impl->ctx) == 0)
		{
			return loader_impl_handle_init(impl, path, handle_impl, handle_ptr, 0);
		}
	}
	else
	{
		if (*handle_ptr != NULL)
		{
			loader_handle_impl target_handle = (loader_handle_impl)*handle_ptr;
			char *duplicated_key;

			if (context_contains(handle_impl->ctx, target_handle->ctx, &duplicated_key) == 0 && duplicated_key != NULL)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Duplicated symbol found named '%s' already defined in the handle scope by handle: %s", duplicated_key, path);
				return 1;
			}
			else if (context_append(target_handle->ctx, handle_impl->ctx) == 0)
			{
				vector_push_back_var(handle_impl->populated_handles, target_handle);

				return loader_impl_handle_init(impl, path, handle_impl, NULL, 1);
			}
		}
		else
		{
			return loader_impl_handle_init(impl, path, handle_impl, handle_ptr, 1);
		}
	}

	return 1;
}

size_t loader_impl_handle_name(plugin_manager manager, const loader_path path, loader_path result)
{
	vector script_paths = plugin_manager_impl_type(manager, loader_manager_impl)->script_paths;

	if (script_paths != NULL)
	{
		size_t iterator, size = vector_size(script_paths);

		
		for (iterator = 0; iterator < size; ++iterator)
		{
			char *script_path = vector_at_type(script_paths, iterator, char *);
			size_t script_path_size = strnlen(script_path, LOADER_PATH_SIZE) + 1;
			size_t path_size = strnlen(path, LOADER_PATH_SIZE) + 1;

			if (portability_path_is_subpath(script_path, script_path_size, path, path_size))
			{
				return portability_path_get_relative(script_path, script_path_size, path, path_size, result, LOADER_PATH_SIZE) - 1;
			}
		}
	}

	size_t length = strnlen(path, LOADER_PATH_SIZE);

	memcpy(result, path, length + 1);

	return length;
}

int loader_impl_load_from_file(plugin_manager manager, plugin p, loader_impl impl, const loader_path paths[], size_t size, void **handle_ptr)
{
	if (impl != NULL)
	{
		loader_impl_interface iface = loader_iface(p);


		if (iface != NULL)
		{
			loader_handle handle;
			loader_path path;
			size_t init_order;

			if (loader_impl_initialize(manager, p, impl) != 0)
			{
				return 1;
			}

			if (loader_impl_handle_name(manager, paths[0], path) > 1 && loader_impl_get_handle(impl, path) != NULL)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Load from file handle failed, handle with name %s already loaded", path);

				return 1;
			}

			init_order = vector_size(impl->handle_impl_init_order);

			vector_push_back_empty(impl->handle_impl_init_order);

			handle = iface->load_from_file(impl, paths, size);

			/* TODO: Disable logs here until log is completely thread safe and async signal safe */
			/* log_write("metacall", LOG_LEVEL_DEBUG, "Loader interface: %p - Loader handle: %p", (void *)iface, (void *)handle); */

			if (handle != NULL)
			{
				loader_handle_impl handle_impl = loader_impl_load_handle(impl, iface, handle, path, LOADER_PATH_SIZE);

				/* TODO: Disable logs here until log is completely thread safe and async signal safe */
				/* log_write("metacall", LOG_LEVEL_DEBUG, "Loader handle impl: %p", (void *)handle_impl); */

				if (handle_impl != NULL)
				{
					handle_impl->populated = 1;

					if (set_insert(impl->handle_impl_path_map, handle_impl->path, handle_impl) == 0)
					{
						if (set_insert(impl->handle_impl_map, handle_impl->module, handle_impl) == 0)
						{
							if (iface->discover(impl, handle_impl->module, handle_impl->ctx) == 0)
							{
								if (loader_impl_handle_register(manager, impl, path, handle_impl, handle_ptr) == 0)
								{
									vector_set_var(impl->handle_impl_init_order, init_order, handle_impl);

									return 0;
								}
							}

							set_remove(impl->handle_impl_map, handle_impl->module);
						}

						set_remove(impl->handle_impl_path_map, handle_impl->path);
					}

					{
						size_t iterator;

						for (iterator = init_order + 1; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
						{
							loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

							loader_impl_destroy_handle(iterator_handle_impl);
						}

						vector_pop_back(impl->handle_impl_init_order);
					}

					log_write("metacall", LOG_LEVEL_ERROR, "Error when loading handle: %s", path);

					loader_impl_destroy_handle(handle_impl);
				}
			}
			else
			{
				size_t iterator;

				for (iterator = init_order + 1; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
				{
					loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

					loader_impl_destroy_handle(iterator_handle_impl);
				}

				vector_pop_back(impl->handle_impl_init_order);
			}
		}
	}

	return 1;
}

int loader_impl_load_from_memory_name(loader_impl impl, loader_name name, const char *buffer, size_t size)
{
	/* TODO: Improve name with time or uuid */
	static const char format[] = "%p-%p-%" PRIuS "-%u";

	hash h = hash_callback_str((const hash_key)buffer);

	size_t length = snprintf(NULL, 0, format, (const void *)impl, (const void *)buffer, size, (unsigned int)h);

	if (length > 0 && length < LOADER_NAME_SIZE)
	{
		size_t written = snprintf(name, length + 1, format, (const void *)impl, (const void *)buffer, size, (unsigned int)h);

		if (written == length)
		{
			return 0;
		}
	}

	return 1;
}

int loader_impl_load_from_memory(plugin_manager manager, plugin p, loader_impl impl, const char *buffer, size_t size, void **handle_ptr)
{
	if (impl != NULL && buffer != NULL && size > 0)
	{
		loader_impl_interface iface = loader_iface(p);

		/* TODO: Disable logs here until log is completely thread safe and async signal safe */
		/* log_write("metacall", LOG_LEVEL_DEBUG, "Loading from memory %.10s...", buffer); */

		if (iface != NULL)
		{
			loader_name name;
			loader_handle handle = NULL;
			size_t init_order;

			if (loader_impl_initialize(manager, p, impl) != 0)
			{
				return 1;
			}

			if (loader_impl_load_from_memory_name(impl, name, buffer, size) != 0)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Load from memory handle failed, name could not be generated correctly");

				return 1;
			}

			if (loader_impl_get_handle(impl, name) != NULL)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Load from memory handle failed, handle with name %s already loaded", name);

				return 1;
			}

			init_order = vector_size(impl->handle_impl_init_order);

			vector_push_back_empty(impl->handle_impl_init_order);

			handle = iface->load_from_memory(impl, name, buffer, size);

			/* TODO: Disable logs here until log is completely thread safe and async signal safe */
			/* log_write("metacall", LOG_LEVEL_DEBUG, "Loader interface: %p - Loader handle: %p", (void *)iface, (void *)handle); */

			if (handle != NULL)
			{
				loader_handle_impl handle_impl = loader_impl_load_handle(impl, iface, handle, name, LOADER_NAME_SIZE);

				if (handle_impl != NULL)
				{
					handle_impl->populated = 1;

					if (set_insert(impl->handle_impl_path_map, handle_impl->path, handle_impl) == 0)
					{
						if (set_insert(impl->handle_impl_map, handle_impl->module, handle_impl) == 0)
						{
							if (iface->discover(impl, handle_impl->module, handle_impl->ctx) == 0)
							{
								if (loader_impl_handle_register(manager, impl, name, handle_impl, handle_ptr) == 0)
								{
									vector_set_var(impl->handle_impl_init_order, init_order, handle_impl);

									return 0;
								}
							}

							set_remove(impl->handle_impl_map, handle_impl->module);
						}

						set_remove(impl->handle_impl_path_map, handle_impl->path);
					}

					{
						size_t iterator;

						for (iterator = init_order + 1; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
						{
							loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

							loader_impl_destroy_handle(iterator_handle_impl);
						}

						vector_pop_back(impl->handle_impl_init_order);
					}

					log_write("metacall", LOG_LEVEL_ERROR, "Error when loading handle: %s", name);

					loader_impl_destroy_handle(handle_impl);
				}
			}
			else
			{
				size_t iterator;

				for (iterator = init_order + 1; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
				{
					loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

					loader_impl_destroy_handle(iterator_handle_impl);
				}

				vector_pop_back(impl->handle_impl_init_order);
			}
		}
	}

	return 1;
}

int loader_impl_load_from_package(plugin_manager manager, plugin p, loader_impl impl, const loader_path path, void **handle_ptr)
{
	if (impl != NULL)
	{
		loader_impl_interface iface = loader_iface(p);
		loader_path subpath;
		size_t init_order;

		if (iface != NULL && loader_impl_handle_name(manager, path, subpath) > 1)
		{
			loader_handle handle;

			if (loader_impl_initialize(manager, p, impl) != 0)
			{
				return 1;
			}

			if (loader_impl_get_handle(impl, subpath) != NULL)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Load from package handle failed, handle with name %s already loaded", subpath);

				return 1;
			}

			init_order = vector_size(impl->handle_impl_init_order);

			vector_push_back_empty(impl->handle_impl_init_order);

			handle = iface->load_from_package(impl, path);

			/* TODO: Disable logs here until log is completely thread safe and async signal safe */
			/* log_write("metacall", LOG_LEVEL_DEBUG, "Loader interface: %p - Loader handle: %p", (void *)iface, (void *)handle); */

			if (handle != NULL)
			{
				loader_handle_impl handle_impl = loader_impl_load_handle(impl, iface, handle, subpath, LOADER_PATH_SIZE);

				if (handle_impl != NULL)
				{
					handle_impl->populated = 1;

					if (set_insert(impl->handle_impl_path_map, handle_impl->path, handle_impl) == 0)
					{
						if (set_insert(impl->handle_impl_map, handle_impl->module, handle_impl) == 0)
						{
							if (iface->discover(impl, handle_impl->module, handle_impl->ctx) == 0)
							{
								if (loader_impl_handle_register(manager, impl, subpath, handle_impl, handle_ptr) == 0)
								{
									vector_set_var(impl->handle_impl_init_order, init_order, handle_impl);

									return 0;
								}
							}

							set_remove(impl->handle_impl_map, handle_impl->module);
						}

						set_remove(impl->handle_impl_path_map, handle_impl->path);
					}

					{
						size_t iterator;

						for (iterator = init_order + 1; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
						{
							loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

							loader_impl_destroy_handle(iterator_handle_impl);
						}

						vector_pop_back(impl->handle_impl_init_order);
					}

					log_write("metacall", LOG_LEVEL_ERROR, "Error when loading handle: %s", subpath);

					loader_impl_destroy_handle(handle_impl);
				}
			}
			else
			{
				size_t iterator;

				for (iterator = init_order + 1; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
				{
					loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

					loader_impl_destroy_handle(iterator_handle_impl);
				}

				vector_pop_back(impl->handle_impl_init_order);
			}
		}
	}

	return 1;
}

void *loader_impl_get_handle(loader_impl impl, const char *name)
{
	if (impl != NULL && name != NULL)
	{
		return (void *)set_get(impl->handle_impl_path_map, (set_key)name);
	}

	return NULL;
}

void loader_impl_set_options(loader_impl impl, value options)
{
	if (impl != NULL && options != NULL)
	{
		impl->options = options;
	}
}

value loader_impl_get_options(loader_impl impl)
{
	if (impl != NULL)
	{
		return impl->options;
	}

	return NULL;
}

value loader_impl_get_option(loader_impl impl, const char *field)
{
	value *options_map = value_to_map(impl->options);
	size_t i, size = value_type_count(impl->options);

	for (i = 0; i < size; ++i)
	{
		value *options_tuple = value_to_array(options_map[i]);

		if (value_type_id(options_tuple[0]) == TYPE_STRING)
		{
			const char *str = value_to_string(options_tuple[0]);

			if (strncmp(str, field, value_type_size(options_tuple[0])) == 0)
			{
				return options_tuple[1];
			}
		}
	}

	return NULL;
}

int loader_impl_get_option_host(loader_impl impl)
{
	value host = loader_impl_get_option(impl, "host");

	if (host != NULL && value_type_id(host) == TYPE_BOOL)
	{
		return value_to_bool(host);
	}

	return 0;
}

int loader_impl_handle_initialize(plugin_manager manager, plugin p, loader_impl impl, const loader_path name, void **handle_ptr)
{
	if (impl != NULL)
	{
		loader_impl_interface iface = loader_iface(p);

		if (iface != NULL)
		{
			loader_path path;
			size_t init_order;

			if (loader_impl_initialize(manager, p, impl) != 0)
			{
				return 1;
			}

			if (loader_impl_handle_name(manager, name, path) > 1 && loader_impl_get_handle(impl, path) != NULL)
			{
				log_write("metacall", LOG_LEVEL_ERROR, "Initialize handle failed, handle with name %s already loaded", path);

				return 1;
			}

			init_order = vector_size(impl->handle_impl_init_order);

			vector_push_back_empty(impl->handle_impl_init_order);

			loader_handle_impl handle_impl = loader_impl_load_handle(impl, iface, NULL, path, LOADER_PATH_SIZE);

			if (handle_impl != NULL)
			{
				handle_impl->populated = 1;

				if (set_insert(impl->handle_impl_path_map, handle_impl->path, handle_impl) == 0)
				{
					if (loader_impl_handle_register(manager, impl, path, handle_impl, handle_ptr) == 0)
					{
						vector_set_var(impl->handle_impl_init_order, init_order, handle_impl);

						return 0;
					}

					set_remove(impl->handle_impl_path_map, handle_impl->path);
				}

				{
					size_t iterator;

					for (iterator = init_order + 1; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
					{
						loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

						loader_impl_destroy_handle(iterator_handle_impl);
					}

					vector_pop_back(impl->handle_impl_init_order);
				}

				log_write("metacall", LOG_LEVEL_ERROR, "Error when loading handle: %s", path);

				loader_impl_destroy_handle(handle_impl);
			}
		}
	}

	return 1;
}

vector loader_impl_handle_populated(void *handle)
{
	loader_handle_impl handle_impl = handle;

	return handle_impl->populated_handles;
}

const char *loader_impl_handle_id(void *handle)
{
	loader_handle_impl handle_impl = handle;

	return handle_impl->path;
}

value loader_impl_handle_export(void *handle)
{
	loader_handle_impl handle_impl = handle;

	return scope_export(context_scope(handle_impl->ctx));
}

context loader_impl_handle_context(void *handle)
{
	loader_handle_impl handle_impl = handle;

	return handle_impl->ctx;
}

void *loader_impl_handle_container_of(loader_impl impl, void *handle)
{
	if (handle != NULL)
	{
		return (void *)((loader_handle_impl)set_get(impl->handle_impl_map, (set_key)handle));
	}

	return NULL;
}

int loader_impl_handle_validate(void *handle)
{
	loader_handle_impl handle_impl = handle;

	return !(handle_impl != NULL && handle_impl->magic == (uintptr_t)loader_handle_impl_magic_alloc);
}

value loader_impl_metadata_handle_name(loader_handle_impl handle_impl)
{
	static const char name[] = "name";

	value *v_ptr, v = value_create_array(NULL, 2);

	if (v == NULL)
	{
		return NULL;
	}

	v_ptr = value_to_array(v);

	v_ptr[0] = value_create_string(name, sizeof(name) - 1);

	if (v_ptr[0] == NULL)
	{
		value_type_destroy(v);

		return NULL;
	}

	v_ptr[1] = value_create_string(handle_impl->path, strlen(handle_impl->path));

	if (v_ptr[1] == NULL)
	{
		value_type_destroy(v);

		return NULL;
	}

	return v;
}

value loader_impl_metadata_handle_context(loader_handle_impl handle_impl)
{
	static const char name[] = "scope";

	value *v_ptr, v = value_create_array(NULL, 2);

	if (v == NULL)
	{
		return NULL;
	}

	v_ptr = value_to_array(v);

	v_ptr[0] = value_create_string(name, sizeof(name) - 1);

	if (v_ptr[0] == NULL)
	{
		value_type_destroy(v);

		return NULL;
	}

	v_ptr[1] = scope_metadata(context_scope(handle_impl->ctx));

	if (v_ptr[1] == NULL)
	{
		value_type_destroy(v);

		return NULL;
	}

	return v;
}

value loader_impl_metadata_handle(loader_handle_impl handle_impl)
{
	value *v_ptr, v = value_create_map(NULL, 2);

	if (v == NULL)
	{
		return NULL;
	}

	v_ptr = value_to_map(v);

	v_ptr[0] = loader_impl_metadata_handle_name(handle_impl);

	if (v_ptr[0] == NULL)
	{
		value_type_destroy(v);

		return NULL;
	}

	v_ptr[1] = loader_impl_metadata_handle_context(handle_impl);

	if (v_ptr[1] == NULL)
	{
		value_type_destroy(v);

		return NULL;
	}

	return v;
}

value loader_impl_metadata(loader_impl impl)
{
	value *values, v = value_create_array(NULL, set_size(impl->handle_impl_path_map));
	struct set_iterator_type it;
	size_t values_it;

	if (v == NULL)
	{
		return NULL;
	}

	values = value_to_map(v);

	for (set_iterator_begin(&it, impl->handle_impl_path_map), values_it = 0; set_iterator_end(&it) != 0; set_iterator_next(&it))
	{
		loader_handle_impl handle_impl = set_iterator_value(&it);

		values[values_it] = loader_impl_metadata_handle(handle_impl);

		if (values[values_it] != NULL)
		{
			++values_it;
		}
	}

	return v;
}

int loader_impl_clear(void *handle)
{
	if (handle != NULL)
	{
		loader_handle_impl handle_impl = handle;

		loader_impl impl = handle_impl->impl;

		size_t iterator;

		/* Remove the handle from the path indexing set */
		int result = !(set_remove(impl->handle_impl_path_map, (set_key)handle_impl->path) == handle_impl);

		/* Remove the handle from the pointer indexing set */
		result |= !(set_remove(impl->handle_impl_map, (set_key)handle_impl->module) == handle_impl);

		/* Search for the handle in the initialization order list and remove it */
		for (iterator = 0; iterator < vector_size(impl->handle_impl_init_order); ++iterator)
		{
			loader_handle_impl iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

			if (handle_impl == iterator_handle_impl)
			{
				vector_erase(impl->handle_impl_init_order, iterator);
				break;
			}
		}

		loader_impl_destroy_handle(handle_impl);

		return result;
	}

	return 1;
}

void loader_impl_destroy_objects(loader_impl impl)
{
	/* This iterates through all functions, classes objects and types,
	* it is necessary to be executed on demand because those can have
	* implementations in the loader implementation which need to be GCed
	* or freed properly before the runtime goes down but after the
	* destroy has been issued, so while it is destroying, we can still
	* retrieve the data for introspection or for whatever we need
	*/
	if (impl != NULL)
	{
		/* Destroy all handles in inverse creation order */
		size_t iterator = vector_size(impl->handle_impl_init_order);

		if (iterator > 0)
		{
			loader_handle_impl iterator_handle_impl;

			do
			{
				--iterator;

				iterator_handle_impl = vector_at_type(impl->handle_impl_init_order, iterator, loader_handle_impl);

				loader_impl_destroy_handle(iterator_handle_impl);

			} while (iterator > 0);
		}

		vector_destroy(impl->handle_impl_init_order);

		/* Destroy the path to handle implementation indexing */
		set_destroy(impl->handle_impl_path_map);

		/* Destroy the handle to handle implementation indexing */
		set_destroy(impl->handle_impl_map);

		/* Destroy all the types */
		{
			struct set_iterator_type it;

			for (set_iterator_begin(&it, impl->type_info_map); set_iterator_end(&it) != 0; set_iterator_next(&it))
			{
				type t = set_iterator_value(&it);

				type_destroy(t);
			}
		}

		set_destroy(impl->type_info_map);
	}
}

void loader_impl_destroy_deallocate(loader_impl impl)
{
	struct set_iterator_type it;

	/* Destroy execution path map */
	for (set_iterator_begin(&it, impl->exec_path_map); set_iterator_end(&it) != 0; set_iterator_next(&it))
	{
		vector paths = set_iterator_value(&it);

		vector_destroy(paths);
	}

	set_destroy(impl->exec_path_map);

	/* Destroy context */
	context_destroy(impl->ctx);

	/* Destroy options */
	if (impl->options != NULL)
	{
		value_type_destroy(impl->options);
	}

	/* Destroy detour map */
	for (set_iterator_begin(&it, impl->detour_map); set_iterator_end(&it) != 0; set_iterator_next(&it))
	{
		detour_handle handle = set_iterator_value(&it);

		detour_unload(impl->d, handle);
	}

	set_destroy(impl->detour_map);

	/* Unload all the dependencies.
	This must be done when the plugin dynlink handle (aka the loader) gets unloaded,
	at this point it is not unloaded yet, because the plugin destructor is called before doing:
		dynlink_unload(p->descriptor->handle);
	As the destroy mechanism requires the loaders to be unloaded at the end after all the destroy methods of all
	loaders have been called, this generates an ourobros that cannot be solved easily. In any case,
	this method still should work because normally those handles are reference counted and we increment
	the reference counter at the beginning, so they will be properly unloaded when the dynlink handle gets unloaded.
	*/
	for (set_iterator_begin(&it, impl->library_map); set_iterator_end(&it) != 0; set_iterator_next(&it))
	{
		dynlink dependency = set_iterator_value(&it);

		dynlink_unload(dependency);
	}

	set_destroy(impl->library_map);

	free(impl);
}

void loader_impl_destroy_dtor(plugin p)
{
	loader_impl_destroy(p, plugin_impl_type(p, loader_impl));
}

void loader_impl_destroy(plugin p, loader_impl impl)
{
	if (impl != NULL)
	{
		if (p != NULL)
		{
			if (impl->init == 0)
			{
				loader_impl_interface iface = loader_iface(p);

				/* TODO: Disable logs here until log is completely thread safe and async signal safe */
				/* log_write("metacall", LOG_LEVEL_DEBUG, "Destroy loader implementation %s", plugin_name(p)); */

				if (iface != NULL && iface->destroy(impl) != 0)
				{
					log_write("metacall", LOG_LEVEL_ERROR, "Invalid loader implementation (%s) interface destruction <%p>", plugin_name(p), iface->destroy);
				}

				impl->init = 1;
			}

			/* Remove the loader library from the library list */
			plugin_descriptor desc = plugin_desc(p);

			set_remove(impl->library_map, (set_key)desc->library_name);
		}
		else
		{
			loader_impl_destroy_objects(impl);
		}

		loader_impl_destroy_deallocate(impl);
	}
}
