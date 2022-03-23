#ifndef ATBM_OS_TIMER_H
#define ATBM_OS_TIMER_H
#include "atbm_type.h"
#include "os_api.h"

typedef atbm_void (*TIMER_CALLBACK)(atbm_void * CallRef);
#define TIMER_1ST_PARAM
/*** Timer Management ***/
typedef void *os_timer_arg_t;

typedef struct{
	TIMER_FUN callback;
	SCI_TIMER_PTR timer;
}OS_TIMER;

typedef enum os_timer_reload {
	OS_TIMER_ONE_SHOT,
	OS_TIMER_PERIODIC,
} os_timer_reload_t;

typedef enum os_timer_activate {
	OS_TIMER_AUTO_ACTIVATE,
	OS_TIMER_NO_ACTIVATE,
} os_timer_activate_t;

atbm_uint32   atbm_InitTimer(OS_TIMER *pTimer, TIMER_CALLBACK pCallback, atbm_void * CallRef);
atbm_uint32  atbm_StartTimer(OS_TIMER *pTimer, atbm_uint32 Interval/*ms*/);
atbm_uint32 atbm_CancelTimer(OS_TIMER *pTimer);
atbm_uint32 atbm_FreeTimer(OS_TIMER *pTimer);
atbm_uint32  atbm_GetOsTime(atbm_void);
ATBM_BOOL atbm_TimeAfter(atbm_uint32 a) ;
atbm_uint64 atbm_GetOsTimeMs();
#define atbm_mdelay(x) atbm_SleepMs(x)
#define atbm_GetOsTime atbm_GetOsTimeMs
#endif /* ATBM_OS_TIMER_H */
