#include <memory/memory_allocator_std.h>

memory_allocator memory_allocator_std(memory_allocator_std_impl_malloc malloc, memory_allocator_std_impl_realloc realloc, memory_allocator_std_impl_free free)
{
	struct memory_allocator_std_ctx_type std_ctx;

	std_ctx.malloc = malloc;
	std_ctx.realloc = realloc;
	std_ctx.free = free;

	return memory_allocator_create(memory_allocator_std_iface(), &std_ctx);
}
