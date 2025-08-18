#ifndef ENVIRONMENT_VARIABLE_PATH_H
#define ENVIRONMENT_VARIABLE_PATH_H 1


#include <environment/environment_api.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>


#if defined(WIN32) || defined(_WIN32) || \
	defined(__CYGWIN__) || defined(__CYGWIN32__) || \
	defined(__MINGW32__) || defined(__MINGW64__)
	#define ENVIRONMENT_VARIABLE_PATH_SEPARATOR_C	'\\'
	#define ENVIRONMENT_VARIABLE_PATH_SEPARATOR_STR "\\"
#elif defined(unix) || defined(__unix__) || defined(__unix) || \
	defined(linux) || defined(__linux__) || defined(__linux) || defined(__gnu_linux) || \
	(defined(__APPLE__) && defined(__MACH__)) || defined(__MACOSX__) || \
	defined(__HAIKU__) || defined(__BEOS__)
	#define ENVIRONMENT_VARIABLE_PATH_SEPARATOR_C	'/'
	#define ENVIRONMENT_VARIABLE_PATH_SEPARATOR_STR "/"
#else
	#error "Unknown environment variable path separator"
#endif


ENVIRONMENT_API char *environment_variable_path_create(const char *name, const char *default_path, size_t default_path_size, size_t *env_size);

ENVIRONMENT_API void environment_variable_path_destroy(char *variable_path);

#ifdef __cplusplus
}
#endif

#endif 
