#ifndef MEMORY_H
#define MEMORY_H 1


#include <memory/memory_api.h>

#include <memory/memory_allocator.h>
#include <memory/memory_allocator_nginx.h>
#include <memory/memory_allocator_std.h>

#ifdef __cplusplus
extern "C" {
#endif


MEMORY_API const char *memory_print_info(void);

#ifdef __cplusplus
}
#endif

#endif 
