
#ifndef METACALL_LINK_H
#define METACALL_LINK_H 1

/* -- Headers -- */

#include <metacall/metacall_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Methods -- */

/**
*  @brief
*    Initialize link detours and allocate shared memory
*
*  @return
*    Zero if success, different from zero otherwise
*/
METACALL_API int metacall_link_initialize(void);

/**
*  @brief
*    Register a function pointer in order to allow function
*    interposition when loading a library, if you register a
*    function @symbol called 'foo', when you try to dlsym (or the equivalent
*    on every platform), you will get the pointer to @fn, even if
*    the symbol does not exist in the library, it will work.
*    Function interposition is required in order to hook into runtimes
*    and dynamically interpose our functions.
*
*  @param[in] tag
*    Name of the loader which the @library belongs to
*
*  @param[in] library
*    Name of the library that is going to be hooked
*
*  @param[in] symbol
*    Name of the function to be interposed
*
*  @param[in] fn
*    Function pointer that will be returned by dlsym (or equivalent) when accessing to @symbol
*
*  @return
*    Zero if success, different from zero otherwise
*/
METACALL_API int metacall_link_register(const char *tag, const char *library, const char *symbol, void (*fn)(void));

/**
*  @brief
*    Register a function pointer in order to allow function
*    interposition when loading a library, if you register a
*    function @symbol called 'foo', when you try to dlsym (or the equivalent
*    on every platform), you will get the pointer to @fn, even if
*    the symbol does not exist in the library, it will work.
*    Function interposition is required in order to hook into runtimes
*    and dynamically interpose our functions.
*
*  @param[in] loader
*    Pointer to the loader which the @library belongs to
*
*  @param[in] library
*    Name of the library that is going to be hooked
*
*  @param[in] symbol
*    Name of the function to be interposed
*
*  @param[in] fn
*    Function pointer that will be returned by dlsym (or equivalent) when accessing to @symbol
*
*  @return
*    Zero if success, different from zero otherwise
*/
METACALL_API int metacall_link_register_loader(void *loader, const char *library, const char *symbol, void (*fn)(void));

/**
*  @brief
*    Remove the hook previously registered
*
*  @param[in] tag
*    Name of the loader which the @library belongs to
*
*  @param[in] library
*    Name of the library that is going to be hooked
*
*  @param[in] symbol
*    Name of the function to be interposed
*
*  @return
*    Zero if success, different from zero otherwise
*/
METACALL_API int metacall_link_unregister(const char *tag, const char *library, const char *symbol);

/**
*  @brief
*    Unregister link detours and destroy shared memory
*/
METACALL_API void metacall_link_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* METACALL_LINK_H */
