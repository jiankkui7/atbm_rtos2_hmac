/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#ifndef ATBM_OS_ATOMIC_H
#define ATBM_OS_ATOMIC_H

#include "tx_api.h"

typedef struct _atbm_atomic_t
{
	int val;
} atbm_atomic_t;

int atbm_test_bit(int nr,atbm_uint32 * addr);
int atbm_clear_bit(int nr,atbm_uint32 * addr);
int atbm_set_bit(int nr,atbm_uint32* addr);

int atbm_find_first_zero_bit(atbm_uint32 * addr,int size);
atbm_void atbm_atomic_set(atbm_atomic_t *at, int val);
int atbm_atomic_read(atbm_atomic_t *at);
int atbm_atomic_xchg(atbm_atomic_t *count,int value);
int atbm_atomic_add_return(int value,atbm_atomic_t *count);
#define atbm_atomic_add					atbm_atomic_add_return
#define atbm_atomic_sub_return(i, v)	(atbm_atomic_add_return(-(int)(i), (v)))
unsigned int atbm_local_irq_save(atbm_void);
atbm_void atbm_local_irq_restore(unsigned int Istate);

#define atbm_TimeAfter(a) ((a) - (10*tx_time_get()) >= 0)
#define atbm_GetOsTimeMs() ((unsigned int)(10*tx_time_get())) //per tick is 10ms
#define msecs_to_jiffies(Ms) (Ms)

#endif /* ATBM_OS_ATOMIC_H */

