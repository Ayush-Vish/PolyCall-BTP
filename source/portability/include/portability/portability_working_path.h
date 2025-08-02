
#ifndef PORTABILITY_WORKING_PATH_H
#define PORTABILITY_WORKING_PATH_H 1

/* -- Headers -- */

#include <portability/portability_api.h>

#include <portability/portability_path.h>

/* -- Type Definitions -- */

typedef char portability_working_path_str[PORTABILITY_PATH_SIZE];

#if defined(WIN32) || defined(_WIN32) || \
	defined(__CYGWIN__) || defined(__CYGWIN32__) || \
	defined(__MINGW32__) || defined(__MINGW64__)
typedef DWORD portability_working_path_length;
#else
typedef size_t portability_working_path_length;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* -- Methods -- */

PORTABILITY_API int portability_working_path(portability_working_path_str path, portability_working_path_length *length);

#ifdef __cplusplus
}
#endif

#endif /* PORTABILITY_WORKING_PATH_H */
