
#ifndef ADT_STRING_H
#define ADT_STRING_H 1

#include <adt/adt_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Headers -- */

#include <stdlib.h>
#include <string.h>

/* -- Macros -- */

/* This is a temporary solution for safe strings, it can be improved in the future */
#define string_copy(dest, src, dest_size) \
	do \
	{ \
		size_t __string_copy_length = strnlen(src, dest_size - 1); \
		memcpy(dest, src, __string_copy_length); \
		dest[__string_copy_length] = '\0'; \
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* ADT_STRING_H */
