#include "atbm_hal.h"
#include "atbm_os_thread.h"


#define WIFI_GENERAL_CPU_STK_CNT (256)//(1024 * 1)


pAtbm_thread_t atbm_createThread(atbm_void(*task)(atbm_void *p_arg),atbm_void *p_arg,int prio)
{
	OS_ERR err;
	OS_TCB *p_tcb = (OS_TCB *)malloc(sizeof(OS_TCB));
	CPU_STK *p_stk = (CPU_STK *)malloc(WIFI_GENERAL_CPU_STK_CNT * sizeof(CPU_STK));

	OSTaskCreate(p_tcb, NULL, task, p_arg, 8, p_stk, 0, WIFI_GENERAL_CPU_STK_CNT, 0, 0, NULL, 0, &err);
	if (err){
		wifi_printk(WIFI_DBG_MSG,"OSTaskCreate err:%d\n", err);
		free(p_tcb);
		free(p_stk);
		return NULL;
	}

	return p_tcb;
}

int atbm_stopThread(pAtbm_thread_t thread_id)
{
	OS_ERR err;

	OSTaskDel(thread_id, &err);
	if (err){
		wifi_printk(WIFI_DBG_MSG,"OSTaskDel err\n");
		return -1;
	}

	free(thread_id);
	return 0;
}

int atbm_changeThreadPriority(int prio)
{
	OS_ERR err;

	OSTaskChangePrio(NULL, prio+2, &err);
	if (err){
		wifi_printk(WIFI_DBG_MSG,"OSTaskChangePrio err\n");
		return -1;
	}

	return 0;
}

int atbm_IncThreadPriority(int prio)
{
	return 0;
}

atbm_uint32 atbm_getThreadStackFreesize(void)
{
	return WIFI_GENERAL_CPU_STK_CNT * sizeof(CPU_STK);
}

int atbm_ThreadStopEvent(pAtbm_thread_t thread_id)
{
	return 0;
}

