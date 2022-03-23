
/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include <stdio.h>
#include "atbm_hal.h"

#define MUTEX_NAME_LEN 32
static char mutexName[MUTEX_NAME_LEN];
static char *mutex_prefix = "mutex_";
static atbm_uint8 mutexIndex = 0;

#define EVENT_NAME_LEN 32
static char eventName[EVENT_NAME_LEN];
static char *event_prefix = "event_";
static atbm_uint8 eventIndex = 0;
#define ATBM_EVENT_WAIT_FLAG   0x00000001


extern atbm_os_wait_queue_head_t *debug_wsm_startup_done;
extern atbm_os_wait_queue_head_t *debug_work_wq;
extern atbm_os_wait_queue_head_t *debug_bh_wq;
extern atbm_os_wait_queue_head_t *debug_wsm_cmd_wq;
extern atbm_os_wait_queue_head_t *debug_atbm_wpa_event_wake;
extern atbm_os_wait_queue_head_t *debug_atbm_event_ack;

atbm_int8 *atbm_get_event_string(atbm_os_wait_queue_head_t *event)
{
	atbm_int8 *ptr;
	atbm_os_wait_queue_head_t *val = event;
	
	if(val==debug_wsm_startup_done)
		ptr = (atbm_int8 *)"wsm_startup_done";
    else if(val==debug_work_wq)
		ptr = (atbm_int8 *)"work_wq";
	else if(val==debug_bh_wq)
		ptr = (atbm_int8 *)"bh_wq";
	else if(val==debug_wsm_cmd_wq)
		ptr = (atbm_int8 *)"wsm_cmd_wq";
	else if(val==debug_atbm_wpa_event_wake)
		ptr = (atbm_int8 *)"atbm_wpa_event_wake";
	else if(val==debug_atbm_event_ack)
		ptr = (atbm_int8 *)"atbm_event_ack";
	else
		ptr = (atbm_int8 *)"not found event string";

	return ptr;
}
atbm_int8 atbm_os_mutexLockInit(atbm_mutex *pmutex)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	atbm_memset((atbm_void*)mutexName, 0, MUTEX_NAME_LEN);
	sprintf(mutexName, "%s%d", mutex_prefix, mutexIndex++);

	status = tx_mutex_create(pmutex, mutexName, TX_INHERIT);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_mutexLockInit error 0x%02x(%d)\n", status, mutexIndex);
		ret = WIFI_ERROR;
		goto error;
	}

	if(pmutex == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_mutexLockInit error\n");
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
}

atbm_int8 atbm_os_DeleteMutex(atbm_mutex *pmutex)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	if(pmutex == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_DeleteMutex error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	
	status = tx_mutex_delete(pmutex);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_DeleteMutex error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

atbm_int8 atbm_os_mutexLock(atbm_mutex *pmutex, atbm_uint32 val)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	val = val;//No used
	
	if(pmutex == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_mutexLock error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	
	status = tx_mutex_get(pmutex, TX_WAIT_FOREVER);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_mutexLock error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

atbm_int8 atbm_os_mutexUnLock(atbm_mutex *pmutex)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	if(pmutex == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_mutexUnLock error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	
	status = tx_mutex_put(pmutex);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_mutexUnLock error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

/*event*/
atbm_int8 atbm_os_init_waitevent(atbm_os_wait_queue_head_t *pevent)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	atbm_memset(eventName, 0, EVENT_NAME_LEN);
	sprintf(eventName, "%s%d", event_prefix, eventIndex++);

	status = tx_event_flags_create(pevent, eventName);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS,"atbm_os_init_waitevent error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

	if(pevent == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_os_init_waitevent error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	wifi_printk(WIFI_ALWAYS,"atbm_os_init_waitevent 0x%x\n", pevent);
error:
	return ret;
}

atbm_int8 atbm_os_delete_waitevent(atbm_os_wait_queue_head_t *pevent)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	if(pevent == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_delete_waitevent error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	status = tx_event_flags_delete(pevent);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_delete_waitevent error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

atbm_int8 atbm_os_wait_event(atbm_os_wait_queue_head_t *pevent)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;
	unsigned long flags;

	if(pevent == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_os_wait_event error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	wifi_printk(WIFI_DBG_MSG,"atbm_os_wait_event 0x%x %s\n", pevent, atbm_get_event_string(pevent));
	
	status = tx_event_flags_get(pevent, ATBM_EVENT_WAIT_FLAG, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER); 	
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS,"atbm_os_wait_event error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
}

atbm_int8 atbm_os_wait_event_timeout(atbm_os_wait_queue_head_t *pevent,atbm_uint32 val/*tick ?*/)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;
	unsigned long flags;

	if(pevent == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_os_wait_event_timeout error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	//wifi_printk(WIFI_DBG_MSG,"atbm_os_wait_event_timeout 0x%x %s\n", pevent, atbm_get_event_string(pevent));

	status = tx_event_flags_get(pevent, ATBM_EVENT_WAIT_FLAG, TX_OR_CLEAR, &flags, val); 
	if(status != TX_SUCCESS){
		//wifi_printk(WIFI_DBG_MSG,"atbm_os_wait_event_timeout 0x%x error 0x%02x\n", pevent, status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
}

atbm_int8 atbm_os_wakeup_event(atbm_os_wait_queue_head_t *pevent)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	if(pevent == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_os_wakeup_event error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	wifi_printk(WIFI_DBG_MSG,"atbm_os_wakeup_event 0x%x %s\n", pevent, atbm_get_event_string(pevent));

	status = tx_event_flags_set(pevent, ATBM_EVENT_WAIT_FLAG, TX_OR);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS,"atbm_os_wakeup_event error 0x%02x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}
	
error:
	return ret;
}



