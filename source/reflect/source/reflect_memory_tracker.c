#include <reflect/reflect_memory_tracker.h>

#include <reflect/reflect_class.h>
#include <reflect/reflect_exception.h>
#include <reflect/reflect_function.h>
#include <reflect/reflect_object.h>

void reflect_memory_tracker_debug(void)
{
#if defined(REFLECT_MEMORY_TRACKER) && REFLECT_MEMORY_TRACKER == 1
	function_stats_debug();
	class_stats_debug();
	object_stats_debug();
	exception_stats_debug();
#endif
}
