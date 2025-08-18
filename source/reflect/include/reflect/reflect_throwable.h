#ifndef REFLECT_THROWABLE_H
#define REFLECT_THROWABLE_H 1

#include <reflect/reflect_api.h>

#include <reflect/reflect_value.h>

#ifdef __cplusplus
extern "C" {
#endif

struct throwable_type;

typedef struct throwable_type *throwable;

REFLECT_API throwable throwable_create(value v);

REFLECT_API value throwable_value(throwable th);

REFLECT_API void throwable_destroy(throwable th);

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_THROWABLE_H */
