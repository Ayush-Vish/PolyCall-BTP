/* -- Headers -- */

#include <threading/threading_mutex.h>

int threading_mutex_initialize(threading_mutex m)
{
	InitializeCriticalSection(m);

	return 0;
}

int threading_mutex_lock(threading_mutex m)
{
	EnterCriticalSection(m);

	return 0;
}

int threading_mutex_try_lock(threading_mutex m)
{
	if (TryEnterCriticalSection(m) == 0)
	{
		return 1;
	}

	return 0;
}

int threading_mutex_unlock(threading_mutex m)
{
	LeaveCriticalSection(m);

	return 0;
}

int threading_mutex_destroy(threading_mutex m)
{
	DeleteCriticalSection(m);

	return 0;
}
