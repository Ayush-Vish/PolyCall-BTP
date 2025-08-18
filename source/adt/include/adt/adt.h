

#ifndef ADT_H
#define ADT_H 1

#include <adt/adt_api.h>

#include <adt/adt_comparable.h>
#include <adt/adt_hash.h>
#include <adt/adt_trie.h>
#include <adt/adt_vector.h>

#ifdef __cplusplus
extern "C" {
#endif

ADT_API const char *adt_print_info(void);

#ifdef __cplusplus
}
#endif

#endif /* ADT_H */
