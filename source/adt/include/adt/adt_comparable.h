
#ifndef ADT_COMPARABLE_H
#define ADT_COMPARABLE_H 1

#include <adt/adt_api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *comparable;

typedef int (*comparable_callback)(const comparable, const comparable);

ADT_API int comparable_callback_str(const comparable a, const comparable b);

ADT_API int comparable_callback_int(const comparable a, const comparable b);

ADT_API int comparable_callback_ptr(const comparable a, const comparable b);

#ifdef __cplusplus
}
#endif

#endif /* ADT_COMPARABLE_H */
