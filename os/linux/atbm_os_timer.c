#include "atbm_hal.h"
#include "atbm_os_timer.h"


int atbm_InitTimer(OS_TIMER *pTimer, TIMER_CALLBACK pCallback, atbm_void * CallRef)
{
	pTimer->pCallback = pCallback;
	pTimer->pCallRef = CallRef;
	pTimer->bTimerStart  = 0;
	pTimer->hnext = ATBM_NULL;
	return 0;
}

int atbm_StartTimer(OS_TIMER *pTimer, int Interval)
{
	if(pTimer->bTimerStart){
		del_timer(&(pTimer->TimerHander));
	}

	init_timer(&(pTimer->TimerHander));
	pTimer->TimerHander.data = (unsigned long)(pTimer->pCallRef);
	pTimer->TimerHander.function = pTimer->pCallback;
	mod_timer(&(pTimer->TimerHander), msecs_to_jiffies(Interval)+jiffies);
	pTimer->bTimerStart = 1;

	return 0;
}

int atbm_CancelTimer(OS_TIMER *pTimer)
{	
	if (!pTimer->bTimerStart){
		return -1;
	}

	pTimer->bTimerStart = 0;
	del_timer(&(pTimer->TimerHander));
	return 0;
}

atbm_void atbm_FreeTimer(OS_TIMER *pTimer)
{
	if(pTimer->bTimerStart){
		atbm_CancelTimer(pTimer);
	}
}

atbm_void atbm_SleepMs(atbm_uint32 uiMiliSecond)
{
	msleep(uiMiliSecond);
}

ATBM_BOOL atbm_TimeAtSameRange(atbm_uint32 TimeFi,atbm_uint32 TimeSe)
{
	return ATBM_TRUE;
}

unsigned int atbm_GetOsTimeMs(atbm_void)
{
	struct timespec ts = current_kernel_time();
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

unsigned int atbm_GetOsTime(atbm_void)
{
	return atbm_GetOsTimeMs();
}

int os_time_get(atbm_void)
{
	return 	atbm_GetOsTime();
}

ATBM_BOOL atbm_TimeAfter(atbm_uint32 tickMs)
{
	atbm_uint32 current_time = atbm_GetOsTimeMs();

	return ((signed int)((signed int)current_time - (signed int)tickMs < 0));	
}

atbm_void atbm_wifi_ticks_timer_init(atbm_void)
{
}

atbm_void atbm_wifi_ticks_timer_cancle(atbm_void)
{
}

