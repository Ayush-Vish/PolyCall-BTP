#ifndef RAPID_JSON_SERIAL_H
#define RAPID_JSON_SERIAL_H 1

/* -- Headers -- */

#include <rapid_json_serial/rapid_json_serial_api.h>

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
RAPID_JSON_SERIAL_API serial_interface rapid_json_serial_impl_interface_singleton(void);

/**
*  @brief
*    Provide the module information
*
*  @return
*    Static string containing module information
*
*/
RAPID_JSON_SERIAL_API const char *rapid_json_serial_print_info(void);

#ifdef __cplusplus
}
#endif

#endif /* RAPID_JSON_SERIAL_H */
