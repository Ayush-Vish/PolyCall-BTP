#ifndef REFLECT_CLASS_VISIBILITY_H
#define REFLECT_CLASS_VISIBILITY_H 1

#include <reflect/reflect_api.h>

#include <reflect/reflect_value.h>

#include <preprocessor/preprocessor_concatenation.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLASS_VISIBILITY_X(X) \
	X(PUBLIC, "public", 0), \
		X(PROTECTED, "protected", 1), \
		X(PRIVATE, "private", 2)

#define CLASS_VISIBILITY_ENUM(name, str, id) \
	PREPROCESSOR_CONCAT(VISIBILITY_, name) = id

enum class_visibility_id
{
	CLASS_VISIBILITY_X(CLASS_VISIBILITY_ENUM)
};

REFLECT_API const char *class_visibility_string(enum class_visibility_id visibility);

REFLECT_API value class_visibility_value(enum class_visibility_id visibility);

REFLECT_API value class_visibility_value_pair(enum class_visibility_id visibility);

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_CLASS_VISIBILITY_H */
