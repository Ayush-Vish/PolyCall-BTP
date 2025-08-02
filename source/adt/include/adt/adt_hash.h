

#ifndef ADT_HASH_H
#define ADT_HASH_H 1

#include <adt/adt_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Headers -- */

#include <stdint.h>

/* -- Type Definitions -- */

typedef uintptr_t hash;

typedef void *hash_key;

typedef hash (*hash_callback)(const hash_key);

/* -- Methods -- */

ADT_API hash hash_callback_str(const hash_key key);

ADT_API hash hash_callback_ptr(const hash_key key);

#ifdef __cplusplus
}
#endif

#endif /* ADT_HASH_H */
