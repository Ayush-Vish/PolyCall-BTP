#ifndef LOADER_IMPL_INTERFACE_H
#define LOADER_IMPL_INTERFACE_H 1

#include <loader/loader_api.h>

#include <loader/loader_handle.h>
#include <loader/loader_impl_data.h>
#include <loader/loader_naming.h>

#include <reflect/reflect_context.h>

#include <configuration/configuration.h>

#ifdef __cplusplus
extern "C" {
#endif

struct loader_impl_type;

typedef struct loader_impl_type *loader_impl;

typedef loader_impl_data (*loader_impl_interface_initialize)(loader_impl, configuration);

typedef int (*loader_impl_interface_execution_path)(loader_impl, const loader_path);

typedef loader_handle (*loader_impl_interface_load_from_file)(loader_impl, const loader_path[], size_t);

typedef loader_handle (*loader_impl_interface_load_from_memory)(loader_impl, const loader_name, const char *, size_t);

typedef loader_handle (*loader_impl_interface_load_from_package)(loader_impl, const loader_path);

typedef int (*loader_impl_interface_clear)(loader_impl, loader_handle);

typedef int (*loader_impl_interface_discover)(loader_impl, loader_handle, context);

typedef int (*loader_impl_interface_destroy)(loader_impl);

typedef struct loader_impl_interface_type
{
	loader_impl_interface_initialize initialize;
	loader_impl_interface_execution_path execution_path;
	loader_impl_interface_load_from_file load_from_file;
	loader_impl_interface_load_from_memory load_from_memory;
	loader_impl_interface_load_from_package load_from_package;
	loader_impl_interface_clear clear;
	loader_impl_interface_discover discover;
	loader_impl_interface_destroy destroy;

} * loader_impl_interface;

typedef loader_impl_interface (*loader_impl_interface_singleton)(void);

#ifdef __cplusplus
}
#endif

#endif
