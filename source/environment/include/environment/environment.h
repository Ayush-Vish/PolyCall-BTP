#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H 1


#include <environment/environment_api.h>

#include <environment/environment_variable.h>
#include <environment/environment_variable_path.h>

#ifdef __cplusplus
extern "C" {
#endif


ENVIRONMENT_API const char *environment_print_info(void);

#ifdef __cplusplus
}
#endif

#endif 
