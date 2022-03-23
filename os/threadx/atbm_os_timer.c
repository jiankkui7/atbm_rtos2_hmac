
#include <stdio.h>
#include "atbm_hal.h"
#include "os_api.h"

#define TIMER_INIT_TICK 0xFFFFFFFF
#define TIMER_NAME_LEN 32
static char timerName[TIMER_NAME_LEN];
static char *prefixName = "timer_";
static atbm_uint8 timerIndex = 0;
PUBLIC atbm_int32 atbm_InitTimer(OS_TIMER *pTimer, TIMER_CALLBACK pCallback, atbm_void * CallRef)
{
	atbm_int32 ret = WIFI_OK;
	atbm_int32 timer_ret;

	memset(timerName, 0, TIMER_NAME_LEN);
	
	sprintf(timerName, "%s%d", prefixName, timerIndex++);
    timer_ret = tx_timer_create(pTimer, 
									timerName, 
									(void *)pCallback, 
									(unsigned long)CallRef, 
									(unsigned long)TIMER_INIT_TICK, 
									(unsigned long)TIMER_INIT_TICK,
									TX_NO_ACTIVATE);

    if(timer_ret != TX_SUCCESS){
        wifi_printk(WIFI_DBG_MSG,"tx_timer_create error (threadx_rcode=0x%02x)\n", timer_ret);
		ret = WIFI_ERROR;
        goto error;
    }

	if(pTimer == ATBM_NULL){
        wifi_printk(WIFI_DBG_MSG,"tx_timer_create error\n");
		ret = WIFI_ERROR;
        goto error;
    }

error:
	return ret;
}

PUBLIC atbm_int32  atbm_StartTimer(OS_TIMER *pTimer, int Interval/*ms*/)
{
	atbm_int32 ret = WIFI_OK;
	atbm_int32 timer_ret;
	
	wifi_printk(WIFI_DBG_MSG, "atbm: atbm_StartTimer()====>\n");
	
	if(pTimer == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG, "atbm: atbm_StartTimer() error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	if((Interval / 10) == 0){
		wifi_printk(WIFI_DBG_MSG, "atbm: atbm_StartTimer() error %d ms\n", Interval);
		Interval = 10;
	}

	timer_ret = tx_timer_deactivate(pTimer);
    if(timer_ret != TX_SUCCESS){
        wifi_printk(WIFI_DBG_MSG,"tx_timer_deactivate error 0x%02x\n", timer_ret);
    }

	timer_ret = tx_timer_change(pTimer, (Interval*HZ)/1000, 0/*(Interval*HZ)/1000*/);
    if(timer_ret != TX_SUCCESS){
        wifi_printk(WIFI_DBG_MSG,"tx_timer_change error 0x%02x\n", timer_ret);
    }

	timer_ret = tx_timer_activate(pTimer);
    if(timer_ret != TX_SUCCESS){
        wifi_printk(WIFI_DBG_MSG,"tx_timer_activate error 0x%02x\n", timer_ret);
    }

	wifi_printk(WIFI_DBG_MSG, "atbm: atbm_StartTimer()<====\n");

error:	
	return ret;
}


PUBLIC atbm_int32 atbm_CancelTimer(OS_TIMER *pTimer)
{
	atbm_int32 ret = WIFI_OK;
	atbm_int32 timer_ret;
	
	wifi_printk(WIFI_DBG_MSG, "atbm: atbm_CancelTimer()====>\n");
	if(pTimer == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG, "atbm: atbm_CancelTimer() error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	timer_ret = tx_timer_deactivate(pTimer);
    if(timer_ret != TX_SUCCESS){
        wifi_printk(WIFI_DBG_MSG,"tx_timer_deactivate error 0x%02x\n", timer_ret);
    }

	wifi_printk(WIFI_DBG_MSG, "atbm: atbm_CancelTimer()<====\n");

error:	
	return ret;
}

PUBLIC atbm_void atbm_FreeTimer(OS_TIMER *pTimer)
{
	atbm_int32 timer_ret;

	wifi_printk(WIFI_DBG_MSG, "atbm: atbm_FreeTimer()====>\n");

	if(pTimer == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG, "atbm: atbm_FreeTimer() error\n");
		return;
	}

	timer_ret = tx_timer_delete(pTimer);
     if(timer_ret != TX_SUCCESS){
        wifi_printk(WIFI_DBG_MSG,"tx_timer_delete error 0x%02x\n", timer_ret);
    }
	 
	wifi_printk(WIFI_DBG_MSG, "atbm: atbm_FreeTimer()<====\n");
	
	return;
}

PUBLIC atbm_void atbm_SleepMs(atbm_uint32 uiMiliSecond)
{
	atbm_int32 timer_ret;

	if(uiMiliSecond < 10){
		wifi_printk(WIFI_DBG_MSG, "atbm: atbm_SleepMs() warning %d\n", uiMiliSecond);
		uiMiliSecond = 10;
	}
	
	timer_ret = tx_thread_sleep((uiMiliSecond*HZ)/1000);
     if(timer_ret != TX_SUCCESS){
        wifi_printk(WIFI_DBG_MSG,"atbm_SleepMs error 0x%02x\n", timer_ret);
    }

	return;
}


unsigned int atbm_GetOsTime(atbm_void)
{
	return tx_time_get();//system tick
}

atbm_void atbm_wifi_ticks_timer_init(atbm_void)
{
//	atbm_InitTimer(&atbm_wifi_tick,atbm_wifi_ticks_timer,&atbm_wifi_ticks);
//	atbm_wifi_ticks = 0;
//	atbm_wifi_tick_start_tick = atbm_GetOsTime();
//	atbm_StartTimer(&atbm_wifi_tick,ATBM_RK_OS_TICKS_MS);
}
atbm_void atbm_wifi_ticks_timer_cancle(atbm_void)
{
//	atbm_CancelTimer(&atbm_wifi_tick);
}

