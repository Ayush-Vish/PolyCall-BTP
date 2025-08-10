/* -- Headers -- */

#include <threading/threading_mutex.h>

int threading_mutex_initialize(threading_mutex m)
{
	return pthread_mutex_init(m, NULL);
}

int threading_mutex_lock(threading_mutex m)
{
	return pthread_mutex_lock(m);
}

int threading_mutex_try_lock(threading_mutex m)
{
	return pthread_mutex_trylock(m);
}

int threading_mutex_unlock(threading_mutex m)
{
	return pthread_mutex_unlock(m);
}

int threading_mutex_destroy(threading_mutex m)
{
	return pthread_mutex_destroy(m);
}
