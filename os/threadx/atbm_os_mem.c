/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include "atbm_hal.h"

void * atbm_kzalloc(int size,int flag)
{  
	void * buf=	atbm_kmalloc(size,ATBM_GFP_KERNEL);
	atbm_memset(buf,0,size);
	return  buf;
}

