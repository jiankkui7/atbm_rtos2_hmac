
#include <stdio.h>
#include "atbm_hal.h"
#include "atbm_os_thread.h"
#include "os_api.h"

#define MAX_TASK_NUM		6 // max task number
#define TASK_STACK_SIZE		20*1024 //stack size is 128K

TX_THREAD thread_ptr[MAX_TASK_NUM];
unsigned char thread_stack[MAX_TASK_NUM][TASK_STACK_SIZE] __attribute__ ((aligned (8)));
unsigned char task_cnt = 0;


void atbm_thead_init(void)
{
	task_cnt = 0;
	atbm_memset(&thread_ptr[0], 0, sizeof(TX_THREAD)*MAX_TASK_NUM);
	//memset(&thread_stack[0][0], 0, sizeof(unsigned char)*MAX_TASK_NUM*TASK_STACK_SIZE);
	return;
}

pAtbm_thread_t atbm_createThread(atbm_void(*task)(atbm_void *p_arg),atbm_void *p_arg,int prio)
{
	atbm_uint32 task_prio =14;
	atbm_uint32 threadx_rcode;
	char task_name[32];
	pAtbm_thread_t pthread=ATBM_NULL;
	
	wifi_printk(WIFI_ALWAYS, "atbm_createThread ==>\n");
	
	//Find an un-used ID
	if(task_cnt > MAX_TASK_NUM){
		wifi_printk(WIFI_ALWAYS, "atbm_createThread max num error !!!\n");
		return ATBM_NULL;
	}

	//Build task name according to task count
	atbm_memset(&task_name[0], 0, 32);
	sprintf(&task_name[0], "wifi_thread_%d", task_cnt);

	//Convert task priority
	//WORK_TASK_PRIO,
	//BH_TASK_PRIO,
	//ELOOP_TASK_PRIO,
	//HIF_TASK_PRIO,
	task_prio -= prio;

	pthread = &thread_ptr[task_cnt];
    threadx_rcode = tx_thread_create(pthread,
                      &task_name[0],
                      (void(*)(unsigned long))task,
                      (unsigned long)p_arg,
                      (atbm_void *) &thread_stack[task_cnt],
                      TASK_STACK_SIZE, 
                      task_prio, 
                      task_prio,
                      1, 
                      TX_AUTO_START);

	if(threadx_rcode == TX_SUCCESS)
		wifi_printk(WIFI_ALWAYS, "atbm_createThread(%s) success!\n", &task_name[0]);
	else
		wifi_printk(WIFI_ALWAYS, "atbm_createThread(%s) failed!\n", &task_name[0]);

	wifi_printk(WIFI_ALWAYS, "atbm_createThread <==\n");
	
	//Task count num start from 1
	task_cnt++;

	return pthread;
}

int atbm_wakeThread(void)
{
	wifi_printk(WIFI_DBG_MSG, "atbm_wakeThread() not support.\n");
	return 0;
}

int atbm_ThreadStopEvent(pAtbm_thread_t pthread)
{	
#if 0
	atbm_uint32 threadx_rcode;

	threadx_rcode = tx_thread_terminate(pthread);
	if(threadx_rcode == TX_SUCCESS)
		wifi_printk(WIFI_DBG_MSG, "atbm_ThreadStopEvent terminate success!\n");
	else
		wifi_printk(WIFI_DBG_MSG, "atbm_ThreadStopEvent terminate failed!\n");

	threadx_rcode = tx_thread_delete(pthread);
	if(threadx_rcode == TX_SUCCESS)
		wifi_printk(WIFI_DBG_MSG, "atbm_ThreadStopEvent delete success!\n");
	else
		wifi_printk(WIFI_DBG_MSG, "atbm_ThreadStopEvent delete failed!\n");
#else
	pthread = pthread;
	wifi_printk(WIFI_DBG_MSG, "atbm_ThreadStopEvent not support, warning!!!\n");
#endif
	return 0;
}

int atbm_stopThread(pAtbm_thread_t pthread)
{	
	atbm_uint32 threadx_rcode;
	
	//Find correct thread id in the pool
	wifi_printk(WIFI_DBG_MSG, "atbm_stopThread ==>\n");

	threadx_rcode = tx_thread_delete(pthread);
	if(threadx_rcode == TX_SUCCESS)
		wifi_printk(WIFI_DBG_MSG, "atbm_createThread success!\n");
	else
		wifi_printk(WIFI_DBG_MSG, "atbm_createThread failed!\n");

	wifi_printk(WIFI_DBG_MSG, "atbm_stopThread <==\n");

	return 0;
}

int atbm_thread_identify(void)
{
	TX_THREAD *thread;
	thread = tx_thread_identify();

	if(thread == TX_SUCCESS)
		wifi_printk(WIFI_DBG_MSG, "thread name %s\n", thread->tx_thread_name);

	return 0;
}

