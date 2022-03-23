#include "atbm_hal.h"
#include "atbm_os_thread.h"

#define MAX_WIFI_TASK           (6)

#define WIFI_GENERAL_STACK_SIZE (1024 * 256)

char *atbm_bh_name[MAX_WIFI_TASK] = 
{
	"work",
	"bhtask",
	"eloop",
	"hif",
#if CONFIG_P2P
	"p2p_task",
#else
	"ATBM_NULL",
#endif
#if ATBM_SUPPORT_SMARTCONFIG
	"st",
#else
	"ATBM_NULL",
#endif
};

atbm_void * atbm_createThread(int(*task)(atbm_void *p_arg),atbm_void *p_arg,int prio)
{
	void *thread = NULL;
	unsigned int ID = prio;

	thread = kthread_create(task, p_arg, atbm_bh_name[ID]);
	if (IS_ERR(thread)){
		return NULL;
	}

	wake_up_process(thread);
	return thread;
}

int atbm_stopThread(atbm_void * thread_id)
{
	kthread_stop(thread_id);
	return 0;
}

int atbm_changeThreadPriority(int prio)
{
	struct sched_param param;

	param.sched_priority = prio;
	sched_setscheduler(current, SCHED_FIFO, &param);
	return 0;
}

int atbm_IncThreadPriority(int prio)
{
#if 0
	struct sched_param param;
	int prio_old = sys_sched_getscheduler(current);

	param.sched_priority = prio_old + prio;
	
	sched_setscheduler(current, SCHED_FIFO, &param);
#endif
	return 0;
}

atbm_uint32 atbm_getThreadStackFreesize(void)
{
	return WIFI_GENERAL_STACK_SIZE;
}

int atbm_ThreadStopEvent(pAtbm_thread_t thread_id)
{
	return 0;
}

