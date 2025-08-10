#include <reflect/reflect_future.h>

#include <log/log.h>

#include <stdlib.h>
#include <string.h>

struct future_type
{
	future_impl impl;
	future_interface interface;
};

future future_create(future_impl impl, future_impl_interface_singleton singleton)
{
	future f = malloc(sizeof(struct future_type));

	if (f == NULL)
	{
		return NULL;
	}

	f->impl = impl;

	if (singleton)
	{
		f->interface = singleton();
	}
	else
	{
		f->interface = NULL;
	}

	if (f->interface != NULL && f->interface->create != NULL)
	{
		if (f->interface->create(f, impl) != 0)
		{
			log_write("metacall", LOG_LEVEL_ERROR, "Invalid future create callback <%p>", f->interface->create);

			free(f);

			return NULL;
		}
	}

	return f;
}

future_return future_await(future f, future_resolve_callback resolve_callback, future_reject_callback reject_callback, void *context)
{
	if (f != NULL)
	{
		if (f->interface != NULL && f->interface->await != NULL)
		{
			/* TODO: Disable logs here until log is completely thread safe and async signal safe */

			/*
			log_write("metacall", LOG_LEVEL_DEBUG, "Await future (%p)", (void *)f);
			*/

			return f->interface->await(f, f->impl, resolve_callback, reject_callback, context);
		}
	}

	return NULL;
}

void future_destroy(future f)
{
	if (f != NULL)
	{
		if (f->interface != NULL && f->interface->destroy != NULL)
		{
			f->interface->destroy(f, f->impl);
		}

		free(f);
	}
}
