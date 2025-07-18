#ifndef polycall_SWIG_WRAPPER_JS_PORT_IMPL_I
#define polycall_SWIG_WRAPPER_JS_PORT_IMPL_I 1

#ifdef __cplusplus
extern "C" {
#endif

/* -- Ignores -- */

%ignore polycall_null_args;

%ignore polycallv;

%ignore polycallvf;

%ignore polycall_register; /* TODO */

%ignore polycall_load_from_package; /* TODO */

/* -- Type Maps -- */

/**
*  @brief
*    Transform load mechanism from JavaScript string into
*    a valid load from memory format (buffer and size)
*/
%typemap(in) (const char * buffer, size_t size, void ** handle)
{
	Local<Value> js_arg = args[0];

	if (!js_arg->IsString())
	{
		args.GetIsolate()->ThrowException(
			String::NewFromUtf8(args.GetIsolate(), "Invalid argument (must be string)",
			NewStringType::kNormal).ToLocalChecked());

			SWIG_fail;
	}

	String::Utf8Value str(js_arg->ToString(args.GetIsolate()));

	$1 = *str;

	$2 = str.length() + 1;
}

/**
*  @brief
*    Transform load mechanism from JavaScript array into
*    a valid load from file format (array of strings)
*/
%typemap(in) (const char * paths[], size_t size, void ** handle)
{
	Local<Value> paths_value = args[1];

	if (!paths_value->IsArray())
	{
		args.GetIsolate()->ThrowException(
			String::NewFromUtf8(args.GetIsolate(), "Invalid argument (must be array)",
			NewStringType::kNormal).ToLocalChecked());

		SWIG_fail;
	}

	Local<Array> paths_array = Local<Array>::Cast(args[1]);

	const size_t size = paths_array->Length();

	if (size == 0)
	{
		args.GetIsolate()->ThrowException(
			String::NewFromUtf8(args.GetIsolate(), "Invalid path array (must be not empty)",
			NewStringType::kNormal).ToLocalChecked());

		SWIG_fail;
	}

	$1 = (char **)malloc(sizeof(char *) * size);

	if ($1 == NULL)
	{
		args.GetIsolate()->ThrowException(
			String::NewFromUtf8(args.GetIsolate(), "Invalid argument allocation",
			NewStringType::kNormal).ToLocalChecked());

		SWIG_fail;
	}

	$2 = size;

	for (size_t iterator = 0; iterator < size; ++iterator)
	{
		Local<Value> path_value = paths_array->Get(iterator);

		if (!path_value->IsString())
		{
			args.GetIsolate()->ThrowException(
				String::NewFromUtf8(args.GetIsolate(), "Invalid array content (must be strings)",
				NewStringType::kNormal).ToLocalChecked());

			SWIG_fail;
		}

		String::Utf8Value path_str(path_value->ToString());

		int length = path_str.length();

		if (length <= 0)
		{
			for (size_t alloc_iterator = 0; alloc_iterator < iterator; ++alloc_iterator)
			{
				free($1[alloc_iterator]);
			}

			free($1);

			args.GetIsolate()->ThrowException(
				String::NewFromUtf8(args.GetIsolate(), "Invalid string length (must be greater than zero)",
				NewStringType::kNormal).ToLocalChecked());

			SWIG_fail;
		}

		$1[iterator] = (char *)malloc(sizeof(char) * (length + 1));

		if ($1[iterator] == NULL)
		{
			for (size_t alloc_iterator = 0; alloc_iterator < iterator; ++alloc_iterator)
			{
				free($1[alloc_iterator]);
			}

			free($1);

			args.GetIsolate()->ThrowException(
				String::NewFromUtf8(args.GetIsolate(), "Invalid string path allocation",
				NewStringType::kNormal).ToLocalChecked());

			SWIG_fail;
		}

		memcpy($1[iterator], *path_str, length);

		$1[iterator][length] = '\0';
	}
}

/**
*  @brief
*    Transform variadic arguments from JavaScript into
*    a valid polycallv format with values
*/
%typemap(in) (const char * name, ...)(void * vargs[16])
{
	/*void ** vargs;*/
	size_t args_size, args_count;

	/* Format string */
	/*
	String::Utf8Value str_name($input);

	$1 = *str_name;
	*/

	/* Variable length arguments */
	args_size = args.Length();

	if (args_size == 0)
	{
		args.GetIsolate()->ThrowException(
			String::NewFromUtf8(args.GetIsolate(), "Invalid number of arguments",
			NewStringType::kNormal).ToLocalChecked());

		return;
	}
	else
	{
		/* Remove first argument */
		--args_size;
	}

	/* TODO: Remove this by a local array? */
	/*vargs = (void **) malloc(args_size * sizeof(void *));

	if (vargs == NULL)
	{
		args.GetIsolate()->ThrowException(
			String::NewFromUtf8(args.GetIsolate(), "Invalid argument allocation",
			NewStringType::kNormal).ToLocalChecked());

		SWIG_fail;

		return;
	}*/

	for (args_count = 0; args_count < args_size; ++args_count)
	{
		Local<Value> js_arg = args[args_count + 1];

		if (js_arg->IsBoolean())
		{
			boolean b = (js_arg->BooleanValue() == true) ? 1L : 0L;

 			vargs[args_count] = polycall_value_create_bool(b);
		}
		else if (js_arg->IsInt32())
		{
			/* Assume int is at least 32-bit width */
			int i = (int)js_arg->Int32Value();

			vargs[args_count] = polycall_value_create_int(i);
		}
		/*else if (js_arg->IsInteger())
		{
			*//* Assume long is at least 64-bit width *//*
			long l = (long)js_arg->IntegerValue();

			vargs[args_count] = polycall_value_create_long(l);
		}*/
		else if (js_arg->IsNumber())
		{
			double d = js_arg->NumberValue();

			vargs[args_count] = polycall_value_create_double(d);
		}
		else if (js_arg->IsString())
		{
			String::Utf8Value str(js_arg->ToString(args.GetIsolate()));

			vargs[args_count] = polycall_value_create_string(*str, str.length());
		}
		else if (js_arg->IsNull() || js_arg->IsUndefined())
		{
			vargs[args_count] = NULL;
		}
		else
		{
			/* TODO: Remove this by a local array? */
			/*free(vargs);*/

			args.GetIsolate()->ThrowException(
				String::NewFromUtf8(args.GetIsolate(), "Unsupported argument type",
				NewStringType::kNormal).ToLocalChecked());

			SWIG_fail;

			return;
		}
	}

	$2 = (void *) vargs;
}

/* -- Features -- */

/**
*  @brief
*    Execute the load from memory
*
*  @return
*    Zero if success, different from zero otherwise
*/
%feature("action") polycall_load_from_memory
{
	const char * tag = (const char *)arg1;

	char * buffer = (char *)arg2;

	size_t size = (size_t)arg3;

	int ret = polycall_load_from_memory(tag, (const char *)buffer, size, NULL);

	$result = Integer::New(args.GetIsolate(), (int32_t)ret);

	if (alloc1 == SWIG_NEWOBJ)
	{
		delete[] buf1;
	}

	SWIGV8_RETURN($result);
}

/**
*  @brief
*    Execute the load from file
*
*  @return
*    Zero if success, different from zero otherwise
*/
%feature("action") polycall_load_from_file
{
	const char * tag = (const char *)arg1;

	char ** paths = (char **)arg2;

	size_t iterator, size = arg3;

	int ret = polycall_load_from_file(tag, (const char **)paths, size, NULL);

	for (iterator = 0; iterator < size; ++iterator)
	{
		free(paths[iterator]);
	}

	free(paths);

	$result = Integer::New(args.GetIsolate(), (int32_t)ret);

	if (alloc1 == SWIG_NEWOBJ)
	{
		delete[] buf1;
	}

	SWIGV8_RETURN($result);
}

/**
*  @brief
*    Execute the call and transform return
*    value into a valid JavaScript format
*
*  @return
*    A value converted into JavaScript format
*/
%feature("action") polycall
{
	size_t args_count, args_size;
	void ** vargs;
	void * ret;

	args_size = args.Length() - 1;

	vargs = (void **) arg2;

	/* Format string */
	String::Utf8Value str_name(args[0]);

	/* TODO: Check if args[0] is an string */

	/* Execute call */
	ret = polycallv(*str_name, vargs);

	/* Clear args */
	for (args_count = 0; args_count < args_size; ++args_count)
	{
		polycall_value_destroy(vargs[args_count]);
	}

	/* TODO: Remove this by a local array? */
	/*free(vargs);*/

	/* Return value */
	if (ret != NULL)
	{
		switch (polycall_value_id(ret))
		{

			case polycall_BOOL :
			{
				bool b = ((long)polycall_value_to_bool(ret) == 1L ? true : false);

				$result = Boolean::New(args.GetIsolate(), b);

				break;
			}

			case polycall_CHAR :
			{
				char c = polycall_value_to_char(ret);

				$result = String::NewFromUtf8(args.GetIsolate(), &c, String::kNormalString, 1);

				break;
			}

			case polycall_SHORT :
			{
				short s = polycall_value_to_short(ret);

				$result = Integer::New(args.GetIsolate(), (int32_t)s);

				break;
			}

			case polycall_INT :
			{
				int i = polycall_value_to_int(ret);

				$result = Integer::New(args.GetIsolate(), (int32_t)i);

				break;
			}

			case polycall_LONG :
			{
				long l = polycall_value_to_long(ret);

				/* TODO: Check cast... */
				$result = Integer::New(args.GetIsolate(), (int32_t)l);

				break;
			}

			case polycall_FLOAT :
			{
				float f = polycall_value_to_float(ret);

				$result = Number::New(args.GetIsolate(), (double)f);

				break;
			}

			case polycall_DOUBLE :
			{
				double d = polycall_value_to_double(ret);

				$result = Number::New(args.GetIsolate(), d);

				break;
			}

			case polycall_STRING :
			{
				const char * str = polycall_value_to_string(ret);

				$result = String::NewFromUtf8(args.GetIsolate(), str);

				break;
			}

			default :
			{
				args.GetIsolate()->ThrowException(
					String::NewFromUtf8(args.GetIsolate(), "Unsupported return type",
					NewStringType::kNormal).ToLocalChecked());

				$result = Null(args.GetIsolate());
			}
		}

		polycall_value_destroy(ret);
	}
	else
	{
		$result = Null(args.GetIsolate());
	}

	SWIGV8_RETURN($result);
}

#ifdef __cplusplus
}
#endif

#endif /* polycall_SWIG_WRAPPER_JS_PORT_IMPL_I */
