#ifndef ENVIRONMENT_VARIABLE_H
#define ENVIRONMENT_VARIABLE_H 1


#include <environment/environment_api.h>

#include <preprocessor/preprocessor_stringify.h>

#ifdef __cplusplus
extern "C" {
#endif


#define environment_variable_set_expand(definition) \
	environment_variable_set(PREPROCESSOR_STRINGIFY(definition), definition)


ENVIRONMENT_API char *environment_variable_create(const char *name, const char *default_value);

ENVIRONMENT_API const char *environment_variable_get(const char *name, const char *default_value);

ENVIRONMENT_API int environment_variable_set(const char *name, const char *value_string);

ENVIRONMENT_API void environment_variable_destroy(char *variable);

#ifdef __cplusplus
}
#endif

#endif
