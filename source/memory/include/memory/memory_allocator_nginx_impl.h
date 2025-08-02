#ifndef MEMORY_ALLOCATOR_NGINX_IMPL_H
#define MEMORY_ALLOCATOR_NGINX_IMPL_H 1


#include <memory/memory_api.h>

#include <memory/memory_allocator_iface.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


struct memory_allocator_nginx_ctx_type;


typedef void *(*memory_allocator_nginx_impl_palloc)(void *, size_t);

typedef void *(*memory_allocator_nginx_impl_pcopy)(void *, const void *, size_t);

typedef intptr_t (*memory_allocator_nginx_impl_pfree)(void *, void *);

typedef struct memory_allocator_nginx_ctx_type *memory_allocator_nginx_ctx;

struct memory_allocator_nginx_ctx_type
{
	void *pool;
	memory_allocator_nginx_impl_palloc palloc;
	memory_allocator_nginx_impl_pcopy pcopy;
	memory_allocator_nginx_impl_pfree pfree;
};


MEMORY_API memory_allocator_iface memory_allocator_nginx_iface(void);

#ifdef __cplusplus
}
#endif

#endif
