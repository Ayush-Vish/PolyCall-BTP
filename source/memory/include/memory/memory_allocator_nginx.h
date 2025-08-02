#ifndef MEMORY_ALLOCATOR_NGINX_H
#define MEMORY_ALLOCATOR_NGINX_H 1


#include <memory/memory_api.h>

#include <memory/memory_allocator.h>
#include <memory/memory_allocator_nginx_impl.h>

#ifdef __cplusplus
extern "C" {
#endif


MEMORY_API memory_allocator memory_allocator_nginx(void *pool, memory_allocator_nginx_impl_palloc palloc, memory_allocator_nginx_impl_pcopy pcopy, memory_allocator_nginx_impl_pfree pfree);

#ifdef __cplusplus
}
#endif

#endif 
