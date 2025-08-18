#ifndef REFLECT_ACCESSOR_H
#define REFLECT_ACCESSOR_H 1

#include <reflect/reflect_attribute_decl.h>
#include <reflect/reflect_constructor_decl.h>
#include <reflect/reflect_method_decl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
* Depending on the class or object accessor type,
* if it is static, there will be an error when accessing
* an attribute that was not registered, otherwise
* it will allow setting or getting the attribute dynamically
*/
enum accessor_type_id
{
	ACCESSOR_TYPE_STATIC = 0,
	ACCESSOR_TYPE_DYNAMIC = 1
};

struct accessor_type
{
	enum accessor_type_id id;

	/*
	* In addition to attribute, there is constructor and method
	* also included for future uses
	*/
	union
	{
		const char *key;
		constructor ctor;
		attribute attr;
		method m;
	} data;
};

#ifdef __cplusplus
}
#endif

#endif /* REFLECT_ACCESSOR_H */
