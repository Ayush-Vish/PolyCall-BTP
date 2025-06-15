#ifndef polycall_SWIG_WRAPPER_JS_PORT_I
#define polycall_SWIG_WRAPPER_JS_PORT_I 1

/* -- Headers -- */

#if defined(SWIG) && defined(SWIGJAVASCRIPT) && defined(SWIG_JAVASCRIPT_V8)

	#if (!defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG) || defined(__DEBUG) || defined(__DEBUG__))
		%module js_portd
		%{
			#define JS_PORT_INITIALIZE_NAME js_portd_initialize
		%}
	#else
		%module js_port
		%{
			#define JS_PORT_INITIALIZE_NAME js_port_initialize
		%}
	#endif

	%{
		#include <js_port/js_port.h>

		#include <polycall/polycall_api.h>
		#include <polycall/polycall.h>
		#include <polycall/polycall_value.h>

		#include <libplatform/libplatform.h>
		#include <v8.h>

		#ifdef ENABLE_DEBUGGER_SUPPORT
		#	include <v8-debug.h>
		#endif /* ENABLE_DEBUGGER_SUPPORT */

		using namespace v8;
	%}

	%include <js_port/js_port.h>

	/* Note: This should not be necessary because we do not allow to use ports outside polycall */
	/*
	%init
	%{
		polycall_initialize();
	%}
	*/

	%import <js_port/js_port_impl.i>

	%include <polycall/polycall_api.h>

	%{
		#if (NODE_MODULE_VERSION < 0x000C)
			extern "C" polycall_API void JS_PORT_INITIALIZE_NAME(v8::Handle<v8::Object> exports);
		#else
			extern "C" polycall_API void JS_PORT_INITIALIZE_NAME(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module);
		#endif
	%}

	#ifdef polycall_API
	#	undef polycall_API
	#	define polycall_API
	#endif

	%include <polycall/polycall.h>

#endif /* SWIG && SWIGJAVASCRIPT && SWIG_JAVASCRIPT_V8 */

#endif /* polycall_SWIG_WRAPPER_JS_PORT_I */
