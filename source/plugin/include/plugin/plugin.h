
#ifndef PLUGIN_H
#define PLUGIN_H 1

/* -- Headers -- */

#include <plugin/plugin_api.h>

#include <plugin/plugin_manager.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Methods  -- */

PLUGIN_API const char *plugin_print_info(void);

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_H */
