#ifndef THREADING_H
#define THREADING_H 1

#include <threading/threading_api.h>

#include <threading/threading_atomic.h>
#include <threading/threading_thread_id.h>

#ifdef __cplusplus
extern "C" {
#endif

THREADING_API const char *threading_print_info(void);

#ifdef __cplusplus
}
#endif

#endif /* THREADING_H */
