#ifndef ATBM_OS_TIMER_H
#define ATBM_OS_TIMER_H


#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>


#define TIMER_1ST_PARAM

typedef atbm_void (*TIMER_CALLBACK)(atbm_void * CallRef);

struct OS_TIMER_S
{
    atbm_uint32          TimerId;
    TIMER_CALLBACK  pCallback;
    atbm_void *          pCallRef;
	atbm_int8 *         pTimerName;
	struct timer_list	TimerHander;
	int 			bTimerStart;
	struct OS_TIMER_S *hnext;
};


typedef struct OS_TIMER_S OS_TIMER;

 int atbm_InitTimer(OS_TIMER *pTimer, TIMER_CALLBACK pCallback, atbm_void * CallRef);
 int atbm_StartTimer(OS_TIMER *pTimer, int Interval/*ms*/);
 int atbm_CancelTimer(OS_TIMER *pTimer);
 atbm_void atbm_FreeTimer(OS_TIMER *pTimer);
 atbm_void atbm_SleepMs(atbm_uint32 uiMiliSecond);

 unsigned int atbm_GetOsTimeMs(atbm_void);
 unsigned int atbm_GetOsTime(atbm_void);

ATBM_BOOL atbm_TimeAfter(atbm_uint32 tickMs);
#define atbm_mdelay atbm_SleepMs
atbm_void atbm_wifi_ticks_timer_init(atbm_void);
#endif /* ATBM_OS_TIMER_H */

