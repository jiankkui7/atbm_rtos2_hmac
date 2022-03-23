#ifndef ATBM_OS_THREAD_H
#define ATBM_OS_THREAD_H
#include <stdio.h>
#include "tx_api.h"

#define pAtbm_thread_t TX_THREAD_PTR

/*priotry */
enum  ATBM_THREAD_PRIO{
	WORK_TASK_PRIO,
	//BH_TASK_PRIO,
	BH_TASK_PRIO,
	
	HIF_TASK_PRIO,

	ELOOP_TASK_PRIO,
};

pAtbm_thread_t atbm_createThread(atbm_void(*task)(atbm_void *p_arg),atbm_void *p_arg,int prio);
int atbm_stopThread(pAtbm_thread_t pthread);
int atbm_ThreadStopEvent(pAtbm_thread_t pthread);
int atbm_thread_identify(void);

#endif /* ATBM_OS_THREAD_H */
