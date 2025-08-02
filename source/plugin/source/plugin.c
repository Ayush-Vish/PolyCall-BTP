

/* -- Headers -- */

#include <metacall/metacall_version.h>

#include <plugin/plugin.h>

/* -- Methods -- */

const char *plugin_print_info(void)
{
	static const char plugin_info[] =
		"Plugin Library " METACALL_VERSION "\n"
		"Copyright (C) 2016 - 2025 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>\n"

#ifdef PLUGIN_STATIC_DEFINE
		"Compiled as static library type\n"
#else
		"Compiled as shared library type\n"
#endif

		"\n";

	return plugin_info;
}
