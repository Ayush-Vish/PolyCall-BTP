#ifndef REFLECT_ATTRIBUTE_DECL_H
#define REFLECT_ATTRIBUTE_DECL_H 1

#ifdef __cplusplus
extern "C" {
#endif

struct attribute_type;

typedef void *attribute_impl;

typedef struct attribute_type *attribute;

typedef void (*attribute_impl_interface_destroy)(attribute, attribute_impl);

typedef struct attribute_interface_type
{
	attribute_impl_interface_destroy destroy;

} * attribute_interface;

typedef attribute_interface (*attribute_impl_interface_singleton)(void);

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_ATTRIBUTE_DECL_H */
