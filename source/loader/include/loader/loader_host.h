#ifndef LOADER_HOST_H
#define LOADER_HOST_H 1


#include <loader/loader_api.h>

#include <loader/loader.h>
#include <loader/loader_impl.h>

#include <reflect/reflect_context.h>

#include <portability/portability_assert.h>

#include <plugin/plugin_impl.h>

#ifdef __cplusplus
extern "C" {
#endif


LOADER_API plugin loader_host_initialize(void);

LOADER_API plugin loader_host_get(void);

LOADER_API int loader_host_register(loader_impl host, context ctx, const char *name, loader_register_invoke invoke, function *func, type_id return_type, size_t arg_size, type_id args_type_id[]);

#ifdef __cplusplus
}
#endif

#endif 
