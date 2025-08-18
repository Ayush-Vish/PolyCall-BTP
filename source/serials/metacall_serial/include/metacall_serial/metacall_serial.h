#ifndef METACALL_SERIAL_H
#define METACALL_SERIAL_H 1

/* -- Headers -- */

#include <metacall_serial/metacall_serial_api.h>

#include <serial/serial_interface.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Methods -- */

/**
*  @brief
*    Instance of interface implementation
*
*  @return
*    Returns pointer to interface to be used by implementation
*
*/
METACALL_SERIAL_API serial_interface metacall_serial_impl_interface_singleton(void);

/**
*  @brief
*    Provide the module information
*
*  @return
*    Static string containing module information
*
*/
METACALL_SERIAL_API const char *metacall_serial_print_info(void);

#ifdef __cplusplus
}
#endif

#endif /* METACALL_SERIAL_H */
