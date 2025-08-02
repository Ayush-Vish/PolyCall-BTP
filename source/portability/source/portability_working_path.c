#include <portability/portability_working_path.h>

#include <string.h>

#if defined(WIN32) || defined(_WIN32) || \
	defined(__CYGWIN__) || defined(__CYGWIN32__) || \
	defined(__MINGW32__) || defined(__MINGW64__)
	#include <winbase.h>
#else
	#include <unistd.h>
#endif

int portability_working_path(portability_working_path_str path, portability_working_path_length *length)
{
	const portability_working_path_length path_max_length = PORTABILITY_PATH_SIZE;

	/* Reset the path */
	memset(path, 0, path_max_length);

#if defined(WIN32) || defined(_WIN32) || \
	defined(__CYGWIN__) || defined(__CYGWIN32__) || \
	defined(__MINGW32__) || defined(__MINGW64__)
	*length = GetCurrentDirectory(0, NULL);

	if (*length == 0)
	{
		/* TODO: DWORD dw = GetLastError(); */
		return 1;
	}

	if (*length > path_max_length)
	{
		/* TODO: Handle error */
		return 1;
	}

	if (GetCurrentDirectory(*length, path) == 0)
	{
		/* TODO: DWORD dw = GetLastError(); */
		return 1;
	}
#else
	if (getcwd(path, path_max_length) == NULL)
	{
		*length = 0;
		/* TODO: Handle error */
		return 1;
	}

	*length = strnlen(path, path_max_length);
#endif

	return 0;
}
