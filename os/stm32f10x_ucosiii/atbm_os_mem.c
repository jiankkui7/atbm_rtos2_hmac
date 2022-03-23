#include "atbm_hal.h"
#include "atbm_os_mem.h"

void *zalloc(size_t size)
{
	void *tmp = NULL;

	tmp = malloc(size);
	if (tmp != NULL)
	{
		memset(tmp, 0, size);
	}

	return tmp;
}


