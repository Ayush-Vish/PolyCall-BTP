

#include <adt/adt_hash.h>

#include <string.h>

hash hash_callback_str(const hash_key key)
{
	/* djb2 */
	const char *str = (const char *)key;

	hash h = 0x1505;

	while (*str++ != '\0')
	{
		h = (hash)(((h << 5) + h) + *str);
	}

	return h;
}

hash hash_callback_ptr(const hash_key key)
{
	/* https://stackoverflow.com/a/12996028 */
	uintptr_t x = (uintptr_t)key;

#if UINTPTR_MAX == 0xFFFFFFFF
	/* 32-bit */
	x = ((x >> 16) ^ x) * 0x45D9F3B;
	x = ((x >> 16) ^ x) * 0x45D9F3B;
	x = (x >> 16) ^ x;
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
	/* 64-bit */
	x = (x ^ (x >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
	x = (x ^ (x >> 27)) * UINT64_C(0x94D049BB133111EB);
	x = x ^ (x >> 31);
#else
	#error "Architecture not implemented for hash function"
#endif

	return x;
}
