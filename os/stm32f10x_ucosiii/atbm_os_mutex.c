#include "atbm_hal.h"
#include "atbm_os_mutex.h"

atbm_uint32 atbm_os_CreateSem(atbm_uint8 ubSemValue)
{
	OS_SEM *sem = NULL;
	OS_ERR err;

	if (ubSemValue == 0){
		wifi_printk(WIFI_DBG_ERROR,"invalid ubSemValue: %d err\n", ubSemValue);
		return -1;
	}
	
	sem = (OS_SEM *)atbm_kmalloc(sizeof(OS_SEM), GFP_KERNEL);
	if (sem == NULL){
		wifi_printk(WIFI_DBG_ERROR,"atbm_kmalloc err\n");
		return -1;
	}

	OSSemCreate(sem, NULL, ubSemValue, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSSemCreate err\n");
		atbm_kfree(sem);
		return -1;
	}

	return (atbm_uint32)sem;
}

atbm_int32 atbm_os_init_waitevent(atbm_os_wait_queue_head_t* wait_queue_head)
{
	OS_ERR err;

	if (wait_queue_head == NULL){
		wifi_printk(WIFI_DBG_ERROR,"wait_queue_head err\n");
		return -1;
	}

	OSSemCreate(wait_queue_head, NULL, 0, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSSemCreate err\n");
		return -1;
	}

	return 0;
}

atbm_uint8 atbm_os_DeleteSem(atbm_os_wait_queue_head_t* wait_queue_head)
{
	OS_ERR err;

	if (wait_queue_head == NULL){
		wifi_printk(WIFI_DBG_ERROR,"wait_queue_head err\n");
		return -1;
	}

	OSSemDel(wait_queue_head, 0, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSSemDel err\n");
		return -1;
	}

    return 0;
}

atbm_uint8 atbm_os_wait_event_timeout(atbm_os_wait_queue_head_t * wait_queue_head, atbm_uint32 ulTimeout)
{
	OS_ERR err;

	if (wait_queue_head == NULL){
		wifi_printk(WIFI_DBG_ERROR,"wait_queue_head err\n");
		return -1;
	}

	OSSemPend(wait_queue_head, 0, 0, NULL, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSSemPend err: %d\n", err);
		return -1;
	}

	return 1;
}

atbm_uint8 atbm_os_wakeup_event(atbm_os_wait_queue_head_t * wait_queue_head)
{
	OS_ERR err;

	if (wait_queue_head == NULL){
		wifi_printk(WIFI_DBG_ERROR,"wait_queue_head err\n");
		return -1;
	}

	OSSemPost(wait_queue_head, 0, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSSemPost err: %d\n", err);
		return -1;
	}

	return 0;
}

atbm_uint32 atbm_os_mutexLockInit(atbm_mutex * pmutex)
{
	OS_ERR err;

	if (pmutex == NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmutex err\n");
		return -1;
	}

	OSMutexCreate(pmutex, NULL, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSMutexCreate err\n");
		return -1;
	}

	return 0;
}

atbm_uint8 atbm_os_DeleteMutex(atbm_mutex * pmutex_id)
{
	OS_ERR err;

	if (pmutex_id == NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmutex_id err\n");
		return -1;
	}

	OSMutexDel(pmutex_id, 0, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSMutexDel err\n");
		return -1;
	}

    return 0;
}

atbm_uint8 atbm_os_mutexLock(atbm_mutex * pmutex_id,atbm_uint32 ulTimeout)
{
	OS_ERR err;

	if (pmutex_id == NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmutex_id err\n");
		return -1;
	}

	OSMutexPend(pmutex_id, 0, 0, NULL, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSMutexPend err\n");
		return -1;
	}

	return 0;
}

atbm_uint8 atbm_os_mutexUnLock(atbm_mutex * pmutex_id)
{
	OS_ERR err;

	if (pmutex_id == NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmutex_id err\n");
		return -1;
	}

	OSMutexPost(pmutex_id, 0, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSMutexPost err\n");
		return -1;
	}

	return 0;
}

atbm_uint8 atbm_os_mutextryLock(atbm_mutex * pmutex_id)
{
	OS_ERR err;

	if (pmutex_id == NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmutex_id err\n");
		return -1;
	}

	OSMutexPend(pmutex_id, 1, OS_OPT_PEND_NON_BLOCKING, NULL, &err);
    if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSMutexPend err\n");
		return -1;
	}

	return 0;
}

