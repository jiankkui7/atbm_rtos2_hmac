/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"
#include "atbm_os_mem.h"

int atoi(const char *str)
{
	char *after;
	return simple_strtol(str,&after,10);
}

void *atbm_kcalloc(atbm_size_t n, atbm_size_t size)
{
	return kcalloc(n,size,GFP_KERNEL);
}


