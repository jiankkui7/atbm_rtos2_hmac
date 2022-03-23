/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"

#define AK_MUTEX_DEBUG_LEVEL	WIFI_DBG_MSG
#define AK_MUTEX_ERR_LEVEL		WIFI_DBG_ERROR

/*
static inline unsigned long os_msec_to_ticks(unsigned long msecs)
{
	return ((msecs) * (RT_TICK_PER_SECOND)) / 1000;
}
*/

atbm_int32 atbm_os_init_waitevent(atbm_os_wait_queue_head_t* pulSem)
{
	*pulSem = SCI_CreateSemaphore("waitQueue", 0);
	if (*pulSem){
		wifi_printk(WIFI_OS,"OS: Semaphore Init: handle %p\r\n", *pulSem);
		return 0;
	}else{
		ATBM_BUG_ON(1);
		return -1;
	}
}


//------------------------------------------------------------------------------
//  Function    : atbm_os_wait_event_timeout
//  Description :
//------------------------------------------------------------------------------
/** @brief This function waits for a semaphore.

@param[in] ulSemId : The semaphore ID that return by @ref atbm_os_CreateSem 
@param[in] ulTimeout : is an optional timeout period (in clock ticks).  If non-zero, your task will
                            wait for the resource up to the amount of time specified by this argument.
                            If you specify 0, however, your task will wait forever at the specified
                            semaphore or, until the resource becomes available.
@retval 0xFE for bad input semaphore id,
		0xFF for acquire semaphore internal error from OS
		0 for getting the resource.
		1 for time out happens
*/

atbm_uint8 atbm_os_wait_event_timeout(atbm_os_wait_queue_head_t * pSem, atbm_uint32 ulTimeout)
{
	uint32 ret;
	if(*pSem==ATBM_NULL)
		ATBM_BUG_ON(1);
	if(0 == ulTimeout){
	    ret = SCI_GetSemaphore(*pSem, SCI_NO_WAIT);
	}else if(OS_WAIT_FOREVER == ulTimeout){
	    ret = SCI_GetSemaphore(*pSem, SCI_WAIT_FOREVER);
	}else{
	    ret = SCI_GetSemaphore(*pSem, ulTimeout);
	}
	//wifi_printk(WIFI_ALWAYS,"OS: Semaphore take:handle %p ulTimeout/5 is %dms\r\n",*pSem, ulTimeout/5);
	if(ret == SCI_SUCCESS)//OS_SUCCESS;
		return 1;
	else if (ret == SCI_NO_INSTANCE)//RT_ETIMEOUT;
		return 0;
	else{
		wifi_printk(WIFI_ALWAYS, "wait err:%d\n", ret);
		return -1;
	}
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_wakeup_event
//  Description :
//------------------------------------------------------------------------------
/** @brief This function signals a semaphore

@param[in] ulSemId : The semaphore ID that return by @ref atbm_os_CreateSem 
@retval 0xFE for bad input semaphore id,
		0xFF for release semaphore internal error from OS
		0 for getting the resource.
		1 If the semaphore count exceeded its limit.
*/
atbm_uint8 atbm_os_wakeup_event(atbm_os_wait_queue_head_t * pSem)
{
	int ret; 
	
	//ret = AK_Release_Semaphore(*pSem);
	ret = SCI_PutSemaphore(*pSem);

	if(SCI_SUCCESS != ret){		
		wifi_printk(AK_MUTEX_ERR_LEVEL,"atbm_os_wakeup_event err\n");
		return -1;
	}

	return 0;
}

atbm_uint8 atbm_os_delete_waitevent(atbm_os_wait_queue_head_t * pSem)
{
	int ret; 
	
	ret = SCI_DeleteSemaphore(*pSem);

	if(SCI_SUCCESS != ret){		
		wifi_printk(AK_MUTEX_ERR_LEVEL,"atbm_os_delete_waitevent err\n");
		return -1;
	}

	return 0;
}

#if 0
//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexLockInit
//  Description :
//------------------------------------------------------------------------------

atbm_uint32 atbm_os_mutexLockInit(atbm_mutex *pmutex)
{
	*pmutex= SCI_CreateMutex("AtbmMutex", SCI_INHERIT);

	return *pmutex ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_DeleteMutex
//  Description :
//------------------------------------------------------------------------------
/** @brief This function deletes a mutual exclusion semaphore and readies all tasks pending on the it.

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_mutexLockInit 
@retval 0xFF for delete mutex internal error from OS
		0, return delete success.
*/

atbm_uint8 atbm_os_DeleteMutex(atbm_mutex *pmutex)
{
   // return ak_thread_mutex_destroy(pmutex_id);
   int status;
	
	//status = AK_Delete_Semaphore(*pmutex_id);
	status = SCI_DeleteMutex(*pmutex);
	
	return status == SCI_SUCCESS ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexLock
//  Description :
//------------------------------------------------------------------------------
/** @brief This function waits for a mutual exclusion semaphore

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_mutexLockInit 
@param[in] ulTimeout : is an optional timeout period (in clock ticks).  If non-zero, your task will
                            wait for the resource up to the amount of time specified by this argument.
                            If you specify 0, however, your task will wait forever at the specified
                            semaphore or, until the resource becomes available.
@retval 0xFE for bad input mutex id,
		0xFF for acquire mutex internal error from OS
		0 for getting the resource.
		1 for time out happens
*/

atbm_uint8 atbm_os_mutexLock(atbm_mutex * pmutex,atbm_uint32 ulTimeout)
{
	atbm_uint8 ret = -1;

	if(0 == ulTimeout)
		ret = SCI_GetMutex(*pmutex, SCI_WAIT_FOREVER);
	else
		ret = SCI_GetMutex(*pmutex, ulTimeout);

	return ret == SCI_SUCCESS ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexUnLock
//  Description :
//------------------------------------------------------------------------------
/** @brief This function signals a mutual exclusion semaphore

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_CreateSem 
@retval 0xFE for bad input mutex id,
		0xFF for release mutex internal error from OS
		0 for getting the resource.
*/
atbm_uint8 atbm_os_mutexUnLock(atbm_mutex * pmutex_id)
{
	atbm_uint8 ret;

	ret = SCI_PutMutex(*pmutex_id);
	return ret == SCI_SUCCESS ? 0 : -1;
}
#else
//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexLockInit
//  Description :
//------------------------------------------------------------------------------

atbm_uint32 atbm_os_mutexLockInit(atbm_mutex *pmutex)
{
	*pmutex= SCI_CreateSemaphore("AtbmMutex", 1);

	return *pmutex ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_DeleteMutex
//  Description :
//------------------------------------------------------------------------------
/** @brief This function deletes a mutual exclusion semaphore and readies all tasks pending on the it.

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_mutexLockInit 
@retval 0xFF for delete mutex internal error from OS
		0, return delete success.
*/

atbm_uint8 atbm_os_DeleteMutex(atbm_mutex *pmutex)
{
   // return ak_thread_mutex_destroy(pmutex_id);
   int status;
	
	//status = AK_Delete_Semaphore(*pmutex_id);
	status = SCI_DeleteSemaphore(*pmutex);
	
	return status == SCI_SUCCESS ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexLock
//  Description :
//------------------------------------------------------------------------------
/** @brief This function waits for a mutual exclusion semaphore

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_mutexLockInit 
@param[in] ulTimeout : is an optional timeout period (in clock ticks).  If non-zero, your task will
                            wait for the resource up to the amount of time specified by this argument.
                            If you specify 0, however, your task will wait forever at the specified
                            semaphore or, until the resource becomes available.
@retval 0xFE for bad input mutex id,
		0xFF for acquire mutex internal error from OS
		0 for getting the resource.
		1 for time out happens
*/

atbm_uint8 atbm_os_mutexLock(atbm_mutex *pmutex,atbm_uint32 ulTimeout)
{
	atbm_uint8 ret = -1;

	if(0 == ulTimeout)
		ret = SCI_GetSemaphore(*pmutex, SCI_WAIT_FOREVER);
	else
		ret = SCI_GetSemaphore(*pmutex, ulTimeout);

	return ret == SCI_SUCCESS ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexUnLock
//  Description :
//------------------------------------------------------------------------------
/** @brief This function signals a mutual exclusion semaphore

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_CreateSem 
@retval 0xFE for bad input mutex id,
		0xFF for release mutex internal error from OS
		0 for getting the resource.
*/
atbm_uint8 atbm_os_mutexUnLock(atbm_mutex * pmutex_id)
{
	atbm_uint8 ret;

	ret = SCI_PutSemaphore(*pmutex_id);
	return ret == SCI_SUCCESS ? 0 : -1;
}

#endif
