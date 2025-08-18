
#include <metacall/metacall_version.h>

#include <adt/adt.h>

const char *adt_print_info(void)
{
	static const char adt_info[] =
		"Abstract Data Type Library " METACALL_VERSION "\n"
		"Copyright (C) 2016 - 2025 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>\n"

#ifdef ADT_STATIC_DEFINE
		"Compiled as static library type"
#else
		"Compiled as shared library type"
#endif

		"\n";

	return adt_info;
}
