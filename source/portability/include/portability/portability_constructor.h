
#ifndef PORTABILITY_CONSTRUCTOR_H
#define PORTABILITY_CONSTRUCTOR_H 1

/* -- Headers -- */

#include <portability/portability_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -- Headers -- */

#include <assert.h>

#include <preprocessor/preprocessor_concatenation.h>
#include <preprocessor/preprocessor_stringify.h>

/* -- Macros -- */

#ifndef portability_constructor

	#ifdef __cplusplus
		#define portability_constructor(ctor) \
			static void ctor(void); \
			static struct PREPROCESSOR_CONCAT(ctor, _type) \
			{ \
				PREPROCESSOR_CONCAT(ctor, _type) \
				(void) \
				{ \
					ctor(); \
				} \
			} PREPROCESSOR_CONCAT(ctor, _ctor); \
			static void ctor(void)
	#elif defined(_MSC_VER)
		/* TODO: Test MSVC version in release mode */
		#pragma section(".CRT$XCU", read)
		#define portability_constructor_impl(ctor, prefix) \
			static void ctor(void); \
			__declspec(allocate(".CRT$XCU")) void (*PREPROCESSOR_CONCAT(ctor, _ptr))(void) = ctor; \
			__pragma(comment(linker, "/include:" prefix PREPROCESSOR_STRINGIFY(ctor) "_ptr")) static void ctor(void)
		#ifdef _WIN64
			#define portability_constructor(ctor) portability_constructor_impl(ctor, "")
		#else
			#define portability_constructor(ctor) portability_constructor_impl(ctor, "_")
		#endif
	#else
		#define portability_constructor(ctor) \
			static void ctor(void) __attribute__((constructor)); \
			static void ctor(void)
	#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTABILITY_CONSTRUCTOR_H */
