#include "atbm_os_timer.h"
#include "atbm_hal.h"

atbm_uint32  atbm_InitTimer(OS_TIMER *pTimer, TIMER_CALLBACK pCallback, atbm_void * CallRef)
{
	atbm_uint8 flag = 0;

	//os_msec_to_ticks(10) timer is right???
    pTimer->timer = SCI_CreateTimer("AtbmTimer", pCallback, CallRef, 10, flag);
	pTimer->callback = pCallback;
    if(pTimer->timer == ATBM_NULL){
	    return -1;
    }else{
	    return 0;
    }
}
/**************************************************************************/
atbm_uint32  atbm_StartTimer(OS_TIMER *pTimer, atbm_uint32 Interval)
{
	atbm_uint32 result;
	//change time
    result = SCI_ChangeTimer(pTimer->timer, pTimer->callback, Interval);
    if (result != SCI_SUCCESS){
        return -1;
    }
	//start time
    result = SCI_ActiveTimer(pTimer->timer);
    if (result == SCI_SUCCESS){
        return 0;
    }else{
        return -1;
    }
}
atbm_uint32 atbm_CancelTimer(OS_TIMER *pTimer)
{
	atbm_uint32 result;
	
    result = SCI_DeactiveTimer(pTimer->timer);
	
    if (result == SCI_SUCCESS){
        return 0;
    }else{
        return -1;
    }
}
atbm_uint32 atbm_FreeTimer(OS_TIMER *pTimer)
{
	atbm_uint32 result;
	result = SCI_DeleteTimer(pTimer->timer);
    if (result == SCI_SUCCESS){
        return 0;
    }else{
        return -1;
    }
}
atbm_uint64 atbm_GetOsTimeMs()
{
	int ret;
	SCI_TICK_TIME_T time;

	ret = SCI_GetTickTime(&time);
	if(ret != SCI_SUCCESS)
		return 0;
	//wifi_printk(WIFI_ALWAYS, "mil:%d sec:%d\n", time.milliseconds, time.second);
	return (atbm_uint64)(time.milliseconds + time.second * 1000);
}

ATBM_BOOL atbm_TimeAfter(atbm_uint32 tickMs)
{
	atbm_uint32 current_time = atbm_GetOsTimeMs();
	
	return ((signed int)((signed int)current_time - (signed int)tickMs < 0));
		
}

atbm_void atbm_wifi_ticks_timer_init(atbm_void)
{
}

atbm_void atbm_SleepMs(atbm_uint32 ms){
	SCI_Sleep(ms);
}
