#include <metacall/metacall_version.h>

#include <memory/memory.h>

const char *memory_print_info(void)
{
	static const char memory_info[] =
		"Memory Library " METACALL_VERSION "\n"
		"Copyright (C) 2016 - 2025 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>\n"

#ifdef MEMORY_STATIC_DEFINE
		"Compiled as static library type"
#else
		"Compiled as shared library type"
#endif

		"\n";

	return memory_info;
}
