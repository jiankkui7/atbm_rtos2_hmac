/**************************************************************************************************************
 * Pantum RTOS wifi hmac source code 
 *
 * Copyright (c) 2020, pantum   All rights reserved.
 *
 *  The source code contains proprietary information of Pantum, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of Pantum.
*****************************************************************************************************************/
#include <stdio.h>
#include "atbm_hal.h"

#define SPIN_NAME_LEN 32
static char spinName[SPIN_NAME_LEN];
static char *spin_prefix = "spin_";
static atbm_uint8 spinIndex = 0;

//unsigned int cpu_disable_interrupts( void );
//void cpu_restore_interrupts( unsigned int previous_interrupt_state );

//unsigned int atbm_interrupt;

/*spin lock*/
atbm_int8 atbm_spin_lock_init(atbm_spinlock_t *spinlock)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	atbm_memset(spinName, 0, SPIN_NAME_LEN);
	sprintf(spinName, "%s%d", spin_prefix, spinIndex++);

#if 0//semaphore
	status = tx_semaphore_create(spinlock, spinName, 1);
#else
	status = tx_mutex_create(spinlock, spinName, TX_INHERIT);
#endif
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_lock_init error 0x%02x(%d)\n", status, spinIndex);
		ret = WIFI_ERROR;
		goto error;
	}

	if(spinlock == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_lock_init error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	
error:
	return ret;
}

atbm_int8 atbm_spin_lock_destroy(atbm_spinlock_t *spinlock)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	if(spinlock == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_lock_destroy error\n");
		ret = WIFI_ERROR;
		goto error;
	}

#if 0
	status = tx_semaphore_delete(spinlock);
#else
	status = tx_mutex_delete(spinlock);
#endif
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_lock_destroy error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}
	
error:
	return ret;
}


atbm_int8 atbm_spin_lock(atbm_spinlock_t *spinlock)
{
	atbm_int8 ret = WIFI_OK;

#if 1	
	return ret;	//No used function
#else
	atbm_uint32 status;

	if(spinlock == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"atbm_spin_lock error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	status = tx_semaphore_get(spinlock, TX_WAIT_FOREVER);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_lock error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
#endif
}

atbm_int8 atbm_spin_unlock(atbm_spinlock_t *spinlock)
{
	atbm_int8 ret = WIFI_OK;

#if 1
	return ret;//No used function
#else
	atbm_uint32 status;

	if(spinlock == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_unlock error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	status = tx_semaphore_put(spinlock);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_unlock error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
#endif
}

atbm_int8 atbm_spin_lock_irqsave(atbm_spinlock_t *spinlock, atbm_uint32 *flag)
{
#if 1
	atbm_int8 ret = WIFI_OK;
	atbm_int8 status;
	atbm_uint32 time_val;

	if(spinlock == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"atbm_spin_lock_irqsave error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	//if(*flag == 0x1234)
	//	time_val = TX_NO_WAIT;
	//else
		time_val = TX_WAIT_FOREVER;

#if 0
	status = tx_semaphore_get(spinlock, time_val);
#else
	status = tx_mutex_get(spinlock, time_val);
#endif
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_lock_irqsave error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
	
#else
	//spinlock = spinlock;
	//*flag = tx_interrupt_control(TX_INT_DISABLE);
	*flag = cpu_disable_interrupts();
	return 0;
#endif
}

atbm_int8 atbm_spin_unlock_irqrestore(atbm_spinlock_t *spinlock, atbm_uint32 flag)
{
#if 1
	atbm_int8 ret = WIFI_OK;
	atbm_int8 status;

	flag = flag;
	
	if(spinlock == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_unlock_irqrestore error\n");
		ret = WIFI_ERROR;
		goto error;
	}
#if 0
	status = tx_semaphore_put(spinlock);
#else
	status = tx_mutex_put(spinlock);
#endif
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_unlock_irqrestore error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}
	
error:
	return ret;
#else
	spinlock = spinlock;
	//tx_interrupt_control(flag);
	cpu_restore_interrupts(flag);
	return 0;
#endif
}

atbm_int8 atbm_spin_lock_bh(atbm_spinlock_t *spinlock)
{
	atbm_int8 ret = WIFI_OK;
	atbm_int8 status;
	
return ret;//No used function

	status = atbm_spin_lock(spinlock);
	if(status != WIFI_OK){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_lock_bh error %d\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
}

atbm_int8 atbm_spin_unlock_bh(atbm_spinlock_t *spinlock)
{
	atbm_int8 ret = WIFI_OK;
	atbm_int8 status;

return ret;//No used function

	status = atbm_spin_unlock(spinlock);
	if(status != WIFI_OK){
		wifi_printk(WIFI_DBG_MSG,"atbm_spin_unlock_bh error %d\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

