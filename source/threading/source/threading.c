#include <metacall/metacall_version.h>

#include <threading/threading.h>

const char *threading_print_info(void)
{
	static const char threading_info[] =
		"Threading Library " METACALL_VERSION "\n"
		"Copyright (C) 2016 - 2025 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>\n"

#ifdef ADT_STATIC_DEFINE
		"Compiled as static library type"
#else
		"Compiled as shared library type"
#endif

		"\n";

	return threading_info;
}
