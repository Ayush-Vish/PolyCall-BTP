

#ifndef PORTABILITY_H
#define PORTABILITY_H 1

/* -- Headers -- */

#include <portability/portability_api.h>

#include <portability/portability_assert.h>
#include <portability/portability_executable_path.h>
#include <portability/portability_library_path.h>
#include <portability/portability_path.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Methods -- */

PORTABILITY_API const char *portability_print_info(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTABILITY_H */
