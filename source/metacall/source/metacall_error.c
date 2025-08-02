

/* -- Headers -- */

#include <metacall/metacall_error.h>

#include <reflect/reflect_value_type.h>

#include <log/log.h>

/* -- Methods -- */

int metacall_error_from_value(void *v, metacall_exception ex)
{
	if (v == NULL || ex == NULL)
	{
		return 1;
	}

	if (type_id_throwable(value_type_id(v)) == 0)
	{
		v = throwable_value(value_to_throwable(v));
	}

	if (type_id_exception(value_type_id(v)) != 0)
	{
		return 1;
	}

	void *ex_impl = value_to_exception(v);

	ex->message = exception_message(ex_impl);
	ex->label = exception_label(ex_impl);
	ex->code = exception_error_code(ex_impl);
	ex->stacktrace = exception_stacktrace(ex_impl);

	return 0;
}

int metacall_error_last(metacall_exception ex)
{
	// TODO
	(void)ex;

	return 1;
}

void metacall_error_clear(void)
{
	// TODO
}
