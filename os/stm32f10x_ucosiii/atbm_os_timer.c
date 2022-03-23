#include "atbm_hal.h"
#include "atbm_os_timer.h"
#include "sdio_porting.h"
#include "os.h"
#include "os_cfg_app.h"

#define STRUCT_OFFSET(stru_name, element) (unsigned long)&((stru_name*)0)->element


void OS_TMR_CALLBACK(void *p_tmr, void *p_arg)
{
	OS_TIMER *pTimer = (OS_TIMER *)((unsigned long)p_tmr - STRUCT_OFFSET(OS_TIMER,TimerHander));
	if (pTimer->pCallback)
		pTimer->pCallback(p_arg);
}

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
	OS_ERR err;
	OS_TICK sigle_tick_ms;
	OS_TICK tick;

	if(pTimer->bTimerStart){
		OSTmrDel(&(pTimer->TimerHander), &err);
		pTimer->bTimerStart = 0;
	}

	sigle_tick_ms = 1000/OS_CFG_TMR_TASK_RATE_HZ;
	if (sigle_tick_ms >= Interval)
	{
		tick = 1;
	}
	else
	{
		tick = Interval/sigle_tick_ms + (Interval%sigle_tick_ms?1:0);
	}

	OSTmrCreate(&pTimer->TimerHander, NULL, tick, 0, OS_OPT_TMR_ONE_SHOT, OS_TMR_CALLBACK, pTimer->pCallRef, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSTmrCreate err\n");
		return -1;
	}

	OSTmrStart(&pTimer->TimerHander, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSTmrStart err\n");
		OSTmrDel(&(pTimer->TimerHander), &err);
		return -1;
	}

	pTimer->bTimerStart = 1;
	return 0;
}

int atbm_CancelTimer(OS_TIMER *pTimer)
{
	OS_ERR err;

	if (!pTimer->bTimerStart){
		return -1;
	}

	OSTmrDel(&(pTimer->TimerHander), &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSTmrDel err\n");
		return -1;
	}

	pTimer->bTimerStart = 0;
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
	sdio_time_delay(uiMiliSecond);
}

ATBM_BOOL atbm_TimeAtSameRange(atbm_uint32 TimeFi,atbm_uint32 TimeSe)
{
	return ATBM_TRUE;
}

unsigned int atbm_GetOsTimeMs(atbm_void)
{
	OS_ERR err;
	return OSTimeGet(&err)/SystemCoreClock;
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

