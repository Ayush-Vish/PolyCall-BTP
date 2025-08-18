#ifndef MEMORY_ALLOCATOR_IFACE_H
#define MEMORY_ALLOCATOR_IFACE_H 1


#include <memory/memory_api.h>

#include <memory/memory_allocator_impl.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>


struct memory_allocator_iface_type;


typedef memory_allocator_impl (*memory_allocator_create_ptr)(void *);

typedef void *(*memory_allocator_allocate_ptr)(memory_allocator_impl, size_t);

typedef void *(*memory_allocator_reallocate_ptr)(memory_allocator_impl, void *, size_t, size_t);

typedef void (*memory_allocator_deallocate_ptr)(memory_allocator_impl, void *);

typedef void (*memory_allocator_destroy_ptr)(memory_allocator_impl);

typedef struct memory_allocator_iface_type *memory_allocator_iface;


struct memory_allocator_iface_type
{
	memory_allocator_create_ptr create;
	memory_allocator_allocate_ptr allocate;
	memory_allocator_reallocate_ptr reallocate;
	memory_allocator_deallocate_ptr deallocate;
	memory_allocator_destroy_ptr destroy;
};

#ifdef __cplusplus
}
#endif

#endif
