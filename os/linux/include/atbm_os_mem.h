/**************************************************************************************************************
 * altobeam LINUX wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#ifndef ATBM_OS_MEM_H
#define ATBM_OS_MEM_H

#include <linux/slab.h>
#include <asm/memory.h>

void *atbm_kcalloc(atbm_size_t n, atbm_size_t size);

#define atbm_kmalloc(x,y)   kmalloc(x,y)
#define atbm_kzalloc(x,y)   kzalloc(x,y)  
#define realloc(x,y)        krealloc(x,y,GFP_KERNEL)
#define atbm_calloc         atbm_kcalloc
#define atbm_kfree(x)       kfree(x)           
#define atbm_memcpy         memcpy 
#define atbm_memset         memset 
#define atbm_memmove  		memmove
#define atbm_memcmp 		memcmp 

int atoi(const char *str);

#endif /* ATBM_OS_MEM_H */

