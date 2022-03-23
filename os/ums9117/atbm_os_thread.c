#include "atbm_hal.h"
#include "atbm_os_thread.h"
#include "os_api.h"

typedef void *os_thread_arg_t;
typedef void (*os_pthread) (os_thread_arg_t argument); 
#define WIFI_TASK_TICK 5
#define WIFI_TASK_NAME_LEN 16
//#define WIFI_TASK_NUM  8
typedef struct os_thread_def  {
	const char name[WIFI_TASK_NAME_LEN];
	os_pthread entry;
	atbm_uint32 stack_size;
	atbm_uint8 priority;
	atbm_uint32 tick;
	void *arg;
} AtbmThreadDef_t;

AtbmThreadDef_t ATBMTHREAD[WIFI_TASK_NUM]={0};
static atbm_uint32 TaskNum=0;


#define MAX_WIFI_TASK 6
#define ATBM_BH_PRO_BASE       (10)
#define WIFI_GENERAL_STACK_SIZE 1024*6

static void thread_entry_func( uint32 argc, void* argv ){
	AtbmThreadDef_t *thread;

	ATBM_ASSERT(argc == 1);
	thread = (AtbmThreadDef_t *)argv;

	ATBM_ASSERT(thread && thread->entry);
	thread->entry(thread->arg);
}

//static int b_first_init =0;
pAtbm_thread_t atbm_createThread(atbm_void(*task)(atbm_void *p_arg),atbm_void *p_arg,int prio)
{
	pAtbm_thread_t thread;	
	do{
		if(prio==BH_TASK_PRIO){
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_BH",WIFI_TASK_NAME_LEN);
		}else if (prio==TXURB_TASK_PRIO){
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_TX",WIFI_TASK_NAME_LEN);
		}else if (prio==RXURB_TASK_PRIO){
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_RX",WIFI_TASK_NAME_LEN);
		}else if (prio==ELOOP_TASK_PRIO){
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_LOOP",WIFI_TASK_NAME_LEN);
		}else if (prio==WORK_TASK_PRIO){
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_WORK",WIFI_TASK_NAME_LEN);
		}else if(prio==HIF_TASK_PRIO){ 
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_HIF",WIFI_TASK_NAME_LEN);
#if ATBM_SUPPORT_SMARTCONFIG
		}else if(prio==SMARTCONFIG_MONNITOR_TASK_PRIO){ 
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_ST",WIFI_TASK_NAME_LEN);
#endif
		}else if(prio == TEST_TASK_PRIO){
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_TEST",WIFI_TASK_NAME_LEN);
		}else{
			atbm_memcpy(&ATBMTHREAD[TaskNum].name,"Atbm_MAIL",WIFI_TASK_NAME_LEN);
		}
		ATBMTHREAD[TaskNum].stack_size=8192;
		ATBMTHREAD[TaskNum].priority= (prio == RXURB_TASK_PRIO) ? TXURB_TASK_PRIO : prio;
		ATBMTHREAD[TaskNum].tick=WIFI_TASK_TICK;
		ATBMTHREAD[TaskNum].entry=task;
		ATBMTHREAD[TaskNum].arg = p_arg;
	}while(0);
	//wifi_printk(WIFI_ALWAYS,"NAME=%s\n",ATBMTHREAD[TaskNum].name);
	//wifi_printk(WIFI_ALWAYS,"stack_size=%d\n",ATBMTHREAD[TaskNum].stack_size);
	//wifi_printk(WIFI_ALWAYS,"priority=%d\n",ATBMTHREAD[TaskNum].priority);
	//wifi_printk(WIFI_ALWAYS,"tick=%d\n",ATBMTHREAD[TaskNum].tick);
	//wifi_printk(WIFI_ALWAYS,"arg=%p\n",ATBMTHREAD[TaskNum].arg);
	//wifi_printk(WIFI_ALWAYS,"ATBMTHREAD=%p\n",&ATBMTHREAD[TaskNum]);
	

	thread = SCI_CreateThread(ATBMTHREAD[TaskNum].name, "atbm", thread_entry_func, 1, &ATBMTHREAD[TaskNum], ATBMTHREAD[TaskNum].stack_size,
		1, ATBMTHREAD[TaskNum].priority, SCI_PREEMPT, SCI_AUTO_START);

	//wifi_printk(WIFI_ALWAYS,"create thread succeed!\n");
	 TaskNum++;
	 return thread;
}


int atbm_stopThread(pAtbm_thread_t thread_id)
{	
	atbm_uint32 result;
	result = SCI_DeleteThread(thread_id);
	if (result == SCI_SUCCESS)
		return 0;
	else
		return -1;
}

int atbm_ThreadStopEvent(pAtbm_thread_t thread_id)
{
	//Not used
	return 0;
}

