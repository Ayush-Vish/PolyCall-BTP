#ifndef THREADING_THREAD_ID_H
#define THREADING_THREAD_ID_H 1

/* -- Headers -- */

#include <threading/threading_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Headers -- */

#include <stdint.h>

/* -- Definitions -- */

#define THREAD_ID_INVALID UINT64_MAX

/* -- Methods -- */

/**
*  @brief
*    Return the current thread id depending on the platform
*
*  @return
*    Curent thread id casted to an uint64_t
*/
THREADING_API uint64_t thread_id_get_current(void);

#ifdef __cplusplus
}
#endif

#endif /* THREADING_THREAD_ID_H */
