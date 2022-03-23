#ifndef ATBM_OS_MEM_H
#define ATBM_OS_MEM_H

#include <stdlib.h>
#include <string.h>

void *zalloc(size_t size);

#define atbm_kmalloc(x,y)   malloc(x)
#define atbm_kzalloc(x,y)   zalloc(x)  
//#define realloc(x,y)        realloc(x,y)
#define atbm_calloc         calloc
#define atbm_kfree(x)       free(x)           
#define atbm_memcpy         memcpy 
#define atbm_memset         memset 
#define atbm_memmove  		memmove
#define atbm_memcmp 		memcmp 

#endif /* ATBM_OS_MEM_H */

