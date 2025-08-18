
#ifndef PORTABILITY_ATEXIT_H
#define PORTABILITY_ATEXIT_H 1

/* -- Headers -- */

#include <portability/portability_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Type Definitions -- */

typedef void (*portability_atexit_fn)(void);

/* -- Methods -- */

/**
*  @brief
*    Initialize atexit instance for custom at exit handlers
*
*  @return
*    Zero if success, different from zero otherwise
*/
PORTABILITY_API int portability_atexit_initialize(void);

/**
*  @brief
*    Register handler to be run at exit
*
*  @param[in] handler
*    Function pointer to the handler that will be executed at exit
*
*  @return
*    Zero if success, different from zero otherwise
*/
PORTABILITY_API int portability_atexit_register(portability_atexit_fn handler);

#ifdef __cplusplus
}
#endif

#endif /* PORTABILITY_ATEXIT_H */
