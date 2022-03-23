/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"
#include "atbm_os_api.h"

extern atbm_int32 atbm_usb_register(atbm_void);
extern atbm_void atbm_usb_deregister(atbm_void);
extern void atbm_thead_init(void);
extern int atbm_local_irq_init(void);
extern int atbm_local_irq_destroy(void);
int atbmwifi_event_OsCallback(struct atbmwifi_vif *priv,int eventid,atbm_void *param)
{
	return 0;
}

 
atbm_uint32 __get_unaligned_cpu32 (const atbm_void *p)
{
	 const struct __una_u32  *ptr = (const struct __una_u32  *)p;
	 return ptr->x;
}

//static __INLINE uint16 get_unaligned_le16(const void *p)
//{
// 	return __get_unaligned_cpu16((const atbm_uint8 *)p);
//}

OS_TIMER test_timer;
atbm_void atbm_timer_test_func(atbm_void * param)
{
	param = param;
	wifi_printk(WIFI_DBG_MSG,"%s: \n\n", __func__);
	return;
}
extern 	TX_QUEUE atbm_timer_msgq ;
extern atbm_void eloop_timeout_func(atbm_void * userdata1);
static atbm_void atbm_test_thread(atbm_void *arg)
{
	atbm_uint32 run_cnt = 0;
	atbm_uint32 time_cnt = 0;
	
	wifi_printk(WIFI_DBG_MSG,"\n\n%s: start...\n\n", __func__);
	
	//atbm_InitTimer(&test_timer, atbm_timer_test_func,(atbm_void*)ATBM_NULL);

	atbm_wifi_on(ATBM_WIFI_AP_MODE);
	atbm_SleepMs(2000);
	atbm_wifi_on(ATBM_WIFI_STA_MODE);
	while (1)
	{
#if 1
		while(1){
			atbm_SleepMs(500);
			wifi_printk(WIFI_DBG_MSG,"...\n");
			if(run_cnt==0 && time_cnt++ > 20){
				time_cnt = 0;
				break;
			}else if(time_cnt++ > 20){
				time_cnt = 0;
				break;
			}
		}
		
		wifi_printk(WIFI_DBG_MSG,"\n\n%s: %d\n\n", __func__, run_cnt++);
		//atbm_wifi_scan_network(ATBM_NULL, 0);

#if 0
		atbm_SleepMs(5000);

		atbm_StartTimer(&test_timer, 2000);

		atbm_SleepMs(10000);

		atbm_CancelTimer(&test_timer);
		
		atbm_SleepMs(1000);
		
		atbm_StartTimer(&test_timer, 2000);
		
		atbm_SleepMs(10000);

		atbm_FreeTimer(&test_timer);
#endif
#if 1
		if(run_cnt ==1){
			if(atbm_wifi_sta_join_ap("TP-LINK_3028", ATBM_NULL, WLAN_WPA2_AUTH_PSK, WLAN_ENCRYPT_AES, "12345678") < 0){
				wifi_printk(WIFI_DBG_MSG,"\n\n%s: sta join ap failed !!!\n\n", __func__);
			}
			atbm_SleepMs(5000);
		}
//#else
		atbm_wifi_ap_create("atbm6032", WLAN_WPA2_AUTH_PSK, WLAN_ENCRYPT_AES, "12345678"/**password*/, 11/*channel*/, 0/*hidder ssid*/);

		while(1)
			atbm_SleepMs(50000000);
#endif
#endif

#if 0
		int data;
		data=0x1234;
		wifi_printk(WIFI_DBG_ERROR,"%s 1. %d 0x%x 0x%x\n",__func__,__LINE__,&data, data);
		atbm_os_MsgQ_Send(&atbm_timer_msgq,&data,TX_NO_WAIT);
		atbm_SleepMs(2000);
		
		wifi_printk(WIFI_DBG_ERROR,"%s 2. %d 0x%x 0x%x\n",__func__,__LINE__,&data, data);
		eloop_timeout_func(&data);
		atbm_SleepMs(2000);
		
		struct wpa_timer wpa_timer;
		wifi_printk(WIFI_DBG_ERROR,"%s 3. %d 0x%x\n",__func__,__LINE__,&wpa_timer);
		eloop_timeout_func(&wpa_timer);
		atbm_SleepMs(2000);

		int timer_addr;
		timer_addr = (int)&wpa_timer;
		wifi_printk(WIFI_DBG_ERROR,"%s 4. %d 0x%x 0x%x\n",__func__,__LINE__,timer_addr,&timer_addr);
		eloop_timeout_func(&timer_addr);
		atbm_SleepMs(5000);
#endif
	}
}
atbm_uint32 get_unaligned_le32(const atbm_void *p)
{
	return __get_unaligned_cpu32 ((const atbm_uint8 *)p);
}

int atbm_usb_register_init(atbm_void)
{
	int ret =0;

	atbm_thead_init();
	atbm_local_irq_init();

	ret = atbm_usb_register();
	if (ret){
		wifi_printk(WIFI_DBG_ERROR,"atbmwifi usb driver register error\n");	
		return ret;
	}
	wifi_printk(WIFI_DBG_MSG,"atbm: usb register successed...\n");

	atbm_createThread(atbm_test_thread,(atbm_void*)ATBM_NULL, WORK_TASK_PRIO);

	return 0;
}
int atbm_usb_register_deinit(atbm_void)
{
	atbm_usb_deregister();
	atbm_local_irq_destroy();
	return 0;
}

atbm_uint32 atbm_os_random(void)
{
	atbm_uint32 data = atbm_random();
	return data;
}

