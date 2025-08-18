#ifndef PLUGIN_EXTENSION_H
#define PLUGIN_EXTENSION_H 1

#include <plugin_extension/plugin_extension_api.h>

#ifdef __cplusplus
extern "C" {
#endif

PLUGIN_EXTENSION_API int plugin_extension(void *loader, void *handle);

#ifdef __cplusplus
}
#endif

#endif 
