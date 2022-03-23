/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#include "atbm_hal.h"
#include "tx_api.h"
#include <trsocket.h>
#include <trmacro.h>
#include <trtype.h>
#include <trglobal.h>

//unsigned int cpu_disable_interrupts( void );
//void cpu_restore_interrupts( unsigned int previous_interrupt_state );
//TX_INTERRUPT_SAVE_AREA
//TX_RESTORE
//TX_DISABLE

static TX_SEMAPHORE atbm_local_irq;

/*
Context: Any
Parameters: None
Description: Disable all interrupts, using the HAL_INTERRUPT_DISABLE macro.
*/
unsigned int atbm_local_irq_save(atbm_void)
{
#if 1
	ATBM_OS_CPU_SR cpu_sr = 0u;
	//cpu_sr = tx_interrupt_control(TX_INT_DISABLE);
	tm_kernel_set_critical;
	return (unsigned int)cpu_sr;
#else
	atbm_uint32 status;
	status = tx_semaphore_put(&atbm_local_irq);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_local_irq_save error 0x%02x\n", status);
	}

	return 0;
#endif
}


/*
Context: Any
Parameters: cpu_sr
Description: Enable all interrupts, using the HAL_INTERRUPT_ENABLE macro.
*/
atbm_void atbm_local_irq_restore(unsigned int cpu_sr)
{
#if 1
	//TX_INTERRUPT_SAVE_AREA;

	//tx_interrupt_control(cpu_sr);
	tm_kernel_release_critical;
	return;
#else
	atbm_uint32 status;
	status = tx_semaphore_get(&atbm_local_irq, TX_WAIT_FOREVER);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_local_irq_restore error 0x%02x\n", status);
	}

	return;
#endif
}
int atbm_local_irq_init(void)
{
	int ret = WIFI_OK;
	atbm_uint32 status;
	TX_SEMAPHORE *ptr_sem = &atbm_local_irq;

	status = tx_semaphore_create(ptr_sem, "atbm_local_irq", 1);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_local_irq_init error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

	if(ptr_sem == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_local_irq_init error\n");
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
}

int atbm_local_irq_destroy(void)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	status = tx_semaphore_delete(&atbm_local_irq);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_local_irq_destroy error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}
	
error:
	return ret;
}

atbm_void atbm_atomic_set(atbm_atomic_t *at, int val)
{
	ATBM_OS_CPU_SR cpu_sr = atbm_local_irq_save();
	at->val = val;
	atbm_local_irq_restore(cpu_sr);
}

int atbm_atomic_read(atbm_atomic_t *at)
{
	int val = 0;
	ATBM_OS_CPU_SR cpu_sr = atbm_local_irq_save();
	
	val = at->val;
	atbm_local_irq_restore(cpu_sr);

	return val;
}

int atbm_atomic_add_return(int val,atbm_atomic_t *at)
{

	ATBM_OS_CPU_SR cpu_sr = atbm_local_irq_save();
	at->val += val;
	atbm_local_irq_restore(cpu_sr);
	
	return  at->val;	
}

int atbm_atomic_xchg(atbm_atomic_t * v, int val)
{
	int tmp = 0;
	ATBM_OS_CPU_SR cpu_sr = atbm_local_irq_save();
	tmp = v->val;
	v->val = val;
	atbm_local_irq_restore(cpu_sr);
	return tmp;
}


int atbm_set_bit(int nr,atbm_uint32* addr)
{
	int mask,retval;

	ATBM_OS_CPU_SR cpu_sr = atbm_local_irq_save();
	addr += nr >>5;
	mask = 1<<(nr & 0x1f);

	retval = (mask & *addr) != 0;
	*addr |= mask;
	atbm_local_irq_restore(cpu_sr);

	return  retval;

}

int atbm_clear_bit(int nr,atbm_uint32 * addr)
{
	int mask,retval;
	ATBM_OS_CPU_SR cpu_sr = atbm_local_irq_save();

	addr += nr >>5;
	mask = 1<<(nr & 0x1f);

	retval = (mask & *addr) != 0;
	*addr &= ~mask;
	atbm_local_irq_restore(cpu_sr);

	return  retval;

}


int atbm_test_bit(int nr,atbm_uint32 * addr)
{

	int mask;

	addr += nr >>5;
	mask = 1<<(nr & 0x1f);

	return  ((mask & *addr) != 0);

}
int atbm_find_first_zero_bit(atbm_uint32 * addr,int size)
{
	int i =0;
	for (i = 0; i <size; i++) {
		if(atbm_test_bit(i,addr) ==0)
			return i;
	}
	return -1;
}

