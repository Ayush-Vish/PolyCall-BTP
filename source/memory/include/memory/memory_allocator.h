#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H 1


#include <memory/memory_api.h>

#include <memory/memory_allocator_iface.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>


struct memory_allocator_type;


typedef struct memory_allocator_type *memory_allocator;


MEMORY_API memory_allocator memory_allocator_create(memory_allocator_iface iface, void *ctx);

MEMORY_API void *memory_allocator_allocate(memory_allocator allocator, size_t size);

MEMORY_API void *memory_allocator_reallocate(memory_allocator allocator, void *data, size_t size, size_t new_size);

MEMORY_API void memory_allocator_deallocate(memory_allocator allocator, void *data);

MEMORY_API size_t memory_allocator_used(memory_allocator allocator);

MEMORY_API void memory_allocator_destroy(memory_allocator allocator);

#ifdef __cplusplus
}
#endif

#endif
