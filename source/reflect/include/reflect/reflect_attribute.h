#ifndef REFLECT_ATTRIBUTE_H
#define REFLECT_ATTRIBUTE_H 1

#include <reflect/reflect_api.h>

#include <reflect/reflect_class_decl.h>
#include <reflect/reflect_class_visibility.h>

#include <reflect/reflect_type.h>

#include <reflect/reflect_attribute_decl.h>

#ifdef __cplusplus
extern "C" {
#endif

REFLECT_API attribute attribute_create(klass cls, const char *name, type t, attribute_impl impl, enum class_visibility_id visibility, attribute_impl_interface_singleton singleton);

REFLECT_API klass attribute_class(attribute attr);

REFLECT_API char *attribute_name(attribute attr);

REFLECT_API type attribute_type(attribute attr);

REFLECT_API attribute_impl attribute_data(attribute attr);

REFLECT_API enum class_visibility_id attribute_visibility(attribute attr);

REFLECT_API value attribute_metadata(attribute attr);

REFLECT_API void attribute_destroy(attribute attr);

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_ATTRIBUTE_H */
