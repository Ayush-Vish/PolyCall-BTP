/* -- Headers -- */

#include <threading/threading_mutex.h>

#include <string.h>

int threading_mutex_initialize(threading_mutex m)
{
	memset(m, 0, sizeof(os_unfair_lock));

	return 0;
}

int threading_mutex_lock(threading_mutex m)
{
	os_unfair_lock_lock(m);

	return 0;
}

int threading_mutex_try_lock(threading_mutex m)
{
	if (os_unfair_lock_trylock(m) == false)
	{
		return 1;
	}

	return 0;
}

int threading_mutex_unlock(threading_mutex m)
{
	os_unfair_lock_unlock(m);

	return 0;
}

int threading_mutex_destroy(threading_mutex m)
{
	(void)m;
	return 0;
}
