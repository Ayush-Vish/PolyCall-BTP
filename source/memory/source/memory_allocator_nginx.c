#include <memory/memory_allocator_nginx.h>


memory_allocator memory_allocator_nginx(void *pool, memory_allocator_nginx_impl_palloc palloc, memory_allocator_nginx_impl_pcopy pcopy, memory_allocator_nginx_impl_pfree pfree)
{
	struct memory_allocator_nginx_ctx_type nginx_ctx;

	nginx_ctx.pool = pool;
	nginx_ctx.palloc = palloc;
	nginx_ctx.pcopy = pcopy;
	nginx_ctx.pfree = pfree;

	return memory_allocator_create(memory_allocator_nginx_iface(), &nginx_ctx);
}
