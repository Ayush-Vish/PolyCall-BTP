#ifndef LOADER_NAMING_H
#define LOADER_NAMING_H 1

#include <loader/loader_api.h>

#include <portability/portability_path.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOADER_PATH_SIZE PORTABILITY_PATH_SIZE
#define LOADER_NAME_SIZE 0xFF
#define LOADER_TAG_SIZE	 0x40

typedef char loader_path[LOADER_PATH_SIZE];
typedef char loader_name[LOADER_NAME_SIZE];
typedef char loader_tag[LOADER_TAG_SIZE];

#ifdef __cplusplus
}
#endif

#endif 
