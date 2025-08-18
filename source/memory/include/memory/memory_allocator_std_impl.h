#ifndef MEMORY_ALLOCATOR_STD_IMPL_H
#define MEMORY_ALLOCATOR_STD_IMPL_H 1

#include <memory/memory_api.h>

#include <memory/memory_allocator_iface.h>

#ifdef __cplusplus
extern "C" {
#endif


struct memory_allocator_std_ctx_type;


typedef void *(*memory_allocator_std_impl_malloc)(size_t);

typedef void *(*memory_allocator_std_impl_realloc)(void *, size_t);

typedef void (*memory_allocator_std_impl_free)(void *);

typedef struct memory_allocator_std_ctx_type *memory_allocator_std_ctx;


struct memory_allocator_std_ctx_type
{
	memory_allocator_std_impl_malloc malloc;
	memory_allocator_std_impl_realloc realloc;
	memory_allocator_std_impl_free free;
};


MEMORY_API memory_allocator_iface memory_allocator_std_iface(void);

#ifdef __cplusplus
}
#endif

#endif 
