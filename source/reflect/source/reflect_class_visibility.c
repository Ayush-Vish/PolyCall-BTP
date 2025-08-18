#include <reflect/reflect_class_visibility.h>

#include <reflect/reflect_value_type.h>

struct visibility_string_length_type
{
	const char *str;
	size_t length;
};

#define CLASS_VISIBILITY_STRUCT(name, str, id) \
	{ \
		str, sizeof(str) - 1 \
	}

static struct visibility_string_length_type visibility_array[] = {
	CLASS_VISIBILITY_X(CLASS_VISIBILITY_STRUCT)
};

const char *class_visibility_string(enum class_visibility_id visibility)
{
	return visibility_array[visibility].str;
}

value class_visibility_value(enum class_visibility_id visibility)
{
	return value_create_string(visibility_array[visibility].str, visibility_array[visibility].length);
}

value class_visibility_value_pair(enum class_visibility_id visibility)
{
	static const char visibility_str[] = "visibility";
	value v = value_create_array(NULL, 2);
	value *v_array;

	if (v == NULL)
	{
		return NULL;
	}

	v_array = value_to_array(v);
	v_array[0] = value_create_string(visibility_str, sizeof(visibility_str) - 1);

	if (v_array[0] == NULL)
	{
		value_type_destroy(v);
		return NULL;
	}

	v_array[1] = class_visibility_value(visibility);

	if (v_array[1] == NULL)
	{
		value_type_destroy(v);
		return NULL;
	}

	return v;
}
