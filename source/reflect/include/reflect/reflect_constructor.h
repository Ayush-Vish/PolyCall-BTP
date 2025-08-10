#ifndef REFLECT_CONSTRUCTOR_H
#define REFLECT_CONSTRUCTOR_H 1

#include <reflect/reflect_api.h>

#include <reflect/reflect_class_visibility.h>
#include <reflect/reflect_type.h>
#include <reflect/reflect_value.h>

#include <reflect/reflect_constructor_decl.h>

#ifdef __cplusplus
extern "C" {
#endif

REFLECT_API constructor constructor_create(size_t count, enum class_visibility_id visibility);

REFLECT_API size_t constructor_count(constructor ctor);

REFLECT_API size_t constructor_get_index(constructor ctor, const char *name);

REFLECT_API const char *constructor_get_name(constructor ctor, size_t index);

REFLECT_API type constructor_get_type(constructor ctor, size_t index);

REFLECT_API void constructor_set(constructor ctor, size_t index, const char *name, type t);

REFLECT_API int constructor_compare(constructor ctor, type_id args[], size_t size);

REFLECT_API enum class_visibility_id constructor_visibility(constructor ctor);

REFLECT_API value constructor_metadata(constructor ctor);

REFLECT_API void constructor_destroy(constructor ctor);

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_CONSTRUCTOR_H */
