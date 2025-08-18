#ifndef REFLECT_H
#define REFLECT_H 1

#include <reflect/reflect_api.h>

#include <reflect/reflect_class.h>
#include <reflect/reflect_context.h>
#include <reflect/reflect_function.h>
#include <reflect/reflect_future.h>
#include <reflect/reflect_object.h>
#include <reflect/reflect_scope.h>
#include <reflect/reflect_signature.h>
#include <reflect/reflect_type.h>
#include <reflect/reflect_type_id.h>
#include <reflect/reflect_value.h>
#include <reflect/reflect_value_type.h>
#include <reflect/reflect_value_type_cast.h>
#include <reflect/reflect_value_type_demotion.h>
#include <reflect/reflect_value_type_id_size.h>
#include <reflect/reflect_value_type_promotion.h>

#ifndef __cplusplus
	#include <reflect/reflect_memory_tracker.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

REFLECT_API const char *reflect_print_info(void);

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_H */
