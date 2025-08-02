#include <metacall/metacall_version.h>

#include <portability/portability.h>

const char *portability_print_info(void)
{
	static const char portability_info[] =
		"Portability Library " METACALL_VERSION "\n"
		"Copyright (C) 2016 - 2025 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>\n"

#ifdef PORTABILITY_STATIC_DEFINE
		"Compiled as static library type"
#else
		"Compiled as shared library type"
#endif

		"\n";

	return portability_info;
}
