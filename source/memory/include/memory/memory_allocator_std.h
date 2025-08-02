#ifndef MEMORY_ALLOCATOR_STD_H
#define MEMORY_ALLOCATOR_STD_H 1

#include <memory/memory_api.h>

#include <memory/memory_allocator.h>
#include <memory/memory_allocator_std_impl.h>

#ifdef __cplusplus
extern "C" {
#endif


MEMORY_API memory_allocator memory_allocator_std(memory_allocator_std_impl_malloc malloc, memory_allocator_std_impl_realloc realloc, memory_allocator_std_impl_free free);

#ifdef __cplusplus
}
#endif

#endif 
