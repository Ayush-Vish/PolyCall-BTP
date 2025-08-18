#include <reflect/reflect_type_id.h>

#include <portability/portability_assert.h>

static const char *type_id_name_map[] = {
	"Boolean",
	"Char",
	"Short",
	"Int",
	"Long",
	"Float",
	"Double",
	"String",
	"Buffer",
	"Array",
	"Map",
	"Pointer",
	"Future",
	"Function",
	"Null",
	"Class",
	"Object",
	"Exception",
	"Throwable"
};

portability_static_assert((int)sizeof(type_id_name_map) / sizeof(type_id_name_map[0]) == (int)TYPE_SIZE,
	"Size of type id name map does not match the type size");

const char *type_id_name(type_id id)
{
	if (id >= 0 && id < TYPE_SIZE)
	{
		return type_id_name_map[id];
	}

	return "Invalid";
}

int type_id_boolean(type_id id)
{
	return !(id == TYPE_BOOL);
}

int type_id_char(type_id id)
{
	return !(id == TYPE_CHAR);
}

int type_id_integer(type_id id)
{
	return !(id == TYPE_BOOL || id == TYPE_CHAR || id == TYPE_SHORT ||
			 id == TYPE_INT || id == TYPE_LONG);
}

int type_id_decimal(type_id id)
{
	return !(id == TYPE_FLOAT || id == TYPE_DOUBLE);
}

int type_id_string(type_id id)
{
	return !(id == TYPE_STRING);
}

int type_id_buffer(type_id id)
{
	return !(id == TYPE_BUFFER);
}

int type_id_array(type_id id)
{
	return !(id == TYPE_ARRAY);
}

int type_id_map(type_id id)
{
	return !(id == TYPE_MAP);
}

int type_id_ptr(type_id id)
{
	return !(id == TYPE_PTR);
}

int type_id_future(type_id id)
{
	return !(id == TYPE_FUTURE);
}

int type_id_function(type_id id)
{
	return !(id == TYPE_FUNCTION);
}

int type_id_null(type_id id)
{
	return !(id == TYPE_NULL);
}

int type_id_class(type_id id)
{
	return !(id == TYPE_CLASS);
}

int type_id_object(type_id id)
{
	return !(id == TYPE_OBJECT);
}

int type_id_exception(type_id id)
{
	return !(id == TYPE_EXCEPTION);
}

int type_id_throwable(type_id id)
{
	return !(id == TYPE_THROWABLE);
}

int type_id_invalid(type_id id)
{
	return !(id >= TYPE_SIZE);
}
