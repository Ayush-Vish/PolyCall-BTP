#ifndef REFLECT_METHOD_DECL_H
#define REFLECT_METHOD_DECL_H 1

#ifdef __cplusplus
extern "C" {
#endif

struct method_type;

typedef void *method_impl;

typedef struct method_type *method;

typedef void (*method_impl_interface_destroy)(method, method_impl);

typedef struct method_interface_type
{
	method_impl_interface_destroy destroy;

} * method_interface;

typedef method_interface (*method_impl_interface_singleton)(void);

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_METHOD_DECL_H */
