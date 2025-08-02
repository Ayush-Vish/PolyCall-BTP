#ifndef LOADER_H
#define LOADER_H 1

#include <loader/loader_api.h>

#include <loader/loader_impl.h>
#include <loader/loader_impl_interface.h>
#include <loader/loader_naming.h>

#ifdef __cplusplus
extern "C" {
#endif


struct loader_type;


typedef value (*loader_register_invoke)(size_t, void *[], void *);

typedef struct loader_type *loader;


LOADER_API void loader_initialization_register(loader_impl impl);

LOADER_API int loader_initialize(void);

LOADER_API int loader_is_initialized(const loader_tag tag);

LOADER_API int loader_register(const char *name, loader_register_invoke invoke, function *func, type_id return_type, size_t arg_size, type_id args_type_id[]);

LOADER_API int loader_register_impl(void *impl, void *handle, const char *name, loader_register_invoke invoke, type_id return_type, size_t arg_size, type_id args_type_id[]);

LOADER_API void loader_detour(detour d);

LOADER_API detour_handle loader_hook(const loader_tag tag, const char *library, int (*load_cb)(detour, detour_handle));

LOADER_API detour_handle loader_hook_impl(void *impl, const char *library, int (*load_cb)(detour, detour_handle));

LOADER_API const char *loader_library_path(void);

LOADER_API int loader_execution_path(const loader_tag tag, const loader_path path);

LOADER_API int loader_load_from_file(const loader_tag tag, const loader_path paths[], size_t size, void **handle);

LOADER_API int loader_load_from_memory(const loader_tag tag, const char *buffer, size_t size, void **handle);

LOADER_API int loader_load_from_package(const loader_tag tag, const loader_path path, void **handle);

LOADER_API int loader_load_from_configuration(const loader_path path, void **handle, void *allocator);

LOADER_API loader_impl loader_get_impl(const loader_tag tag);

LOADER_API value loader_get(const char *name);

LOADER_API void *loader_get_handle(const loader_tag tag, const char *name);

LOADER_API int loader_set_options(const loader_tag tag, value options);

LOADER_API value loader_get_options(const loader_tag tag);

LOADER_API value loader_get_option(const loader_tag tag, const char *field);

LOADER_API int loader_handle_initialize(loader_impl impl, const loader_path name, void **handle_ptr);

LOADER_API const char *loader_handle_id(void *handle);

LOADER_API void *loader_handle_export(void *handle);

LOADER_API value loader_handle_get(void *handle, const char *name);

LOADER_API int loader_handle_populate(void *handle_dest, void *handle_src);

LOADER_API value loader_metadata(void);

LOADER_API int loader_clear(void *handle);

LOADER_API int loader_is_destroyed(loader_impl impl);

LOADER_API void loader_set_destroyed(loader_impl impl);

LOADER_API void loader_unload_children(loader_impl impl);

LOADER_API void loader_destroy(void);

LOADER_API const char *loader_print_info(void);

#ifdef __cplusplus
}
#endif

#endif 
