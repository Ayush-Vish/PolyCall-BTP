

#ifndef PORTABILITY_EXECUTABLE_PATH_H
#define PORTABILITY_EXECUTABLE_PATH_H 1

/* -- Headers -- */

#include <portability/portability_api.h>

#include <portability/portability_path.h>

/* -- Type Definitions -- */

typedef char portability_executable_path_str[PORTABILITY_PATH_SIZE];

#if defined(WIN32) || defined(_WIN32) || \
	defined(__CYGWIN__) || defined(__CYGWIN32__) || \
	defined(__MINGW32__) || defined(__MINGW64__)
typedef DWORD portability_executable_path_length;
#elif defined(unix) || defined(__unix__) || defined(__unix) || \
	defined(linux) || defined(__linux__) || defined(__linux) || defined(__gnu_linux) || \
	defined(__NetBSD__) || defined(__DragonFly__)

typedef ssize_t portability_executable_path_length;
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(__MACOSX__)
typedef uint32_t portability_executable_path_length;
#elif defined(__FreeBSD__)
typedef size_t portability_executable_path_length;
#elif defined(sun) || defined(__sun)
typedef size_t portability_executable_path_length;
#else
	#error "Unimplemented platform, please add support to it"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* -- Methods -- */

PORTABILITY_API int portability_executable_path(portability_executable_path_str path, portability_executable_path_length *length);

#ifdef __cplusplus
}
#endif

#endif /* PORTABILITY_EXECUTABLE_PATH_H */
