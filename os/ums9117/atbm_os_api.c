/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include "atbm_hal.h"
/******** Functions below is Wlan API **********/
#include "atbm_sysops.h"
#include <stdint.h>

extern struct atbmwifi_common g_hw_prv;


int atbmwifi_event_OsCallback(atbm_void *prv,int eventid,atbm_void *param)
{
	struct atbmwifi_vif *priv = prv;

	if(atbmwifi_is_ap_mode(priv->iftype)){
		atbm_uint8 * staMacAddr ;
		switch(eventid){
			case ATBM_WIFI_DEAUTH_EVENT:
				wifi_printk(WIFI_ALWAYS,"event_OsCallback DEAUTH\n");
				break;
			case ATBM_WIFI_AUTH_EVENT:
				break;
			case ATBM_WIFI_ASSOC_EVENT:
				break;
			case ATBM_WIFI_ASSOCRSP_TXOK_EVENT: 
				staMacAddr =(atbm_uint8 *) param;

				break;
			case ATBM_WIFI_DEASSOC_EVENT:
				break;
			case ATBM_WIFI_JOIN_EVENT:			
				staMacAddr =(atbm_uint8 *) param;
				wifi_printk(WIFI_ALWAYS, "ATBM_WIFI_JOIN_EVENT\n");
				break;
			case ATBM_WIFI_ENABLE_NET_EVENT:
				break;
			default:
				break;
		}
	}
	else {	
		switch(eventid){
			case ATBM_WIFI_SCANSTART_EVENT:
				break;
			case ATBM_WIFI_SCANDONE_EVENT:				
//				event.event_type = WLAN_E_SCAN_COMPLETE;
//				WLAN_SYS_StatusCallback(&event);	
				break;
			case ATBM_WIFI_DEAUTH_EVENT:
				break;
			case ATBM_WIFI_AUTH_EVENT:
				break;
			case ATBM_WIFI_ASSOC_EVENT:	
				break;
			case ATBM_WIFI_DEASSOC_EVENT:
				//AtbmWifiDisconnectCallback();	
				break;
			case ATBM_WIFI_JOIN_EVENT:
				wifi_printk(WIFI_ALWAYS, "~~~~ATBM_WIFI_JOIN_EVENT\n");
				break;
			case ATBM_WIFI_ENABLE_NET_EVENT:
				/*When disconnected disallow auto connect*/
				priv->auto_connect_when_lost = 0;
				break;
			case ATBM_WIFI_SMARTCONFIG_SUCCESS:
				break;
#if CONFIG_WPS
			case ATBM_WIFI_WPS_SUCCESS:
				{
					struct atbm_wpa_ssid *ssid = (struct atbm_wpa_ssid *)param;
					atbm_wifi_sta_join_ap(ssid->ssid, NULL, 0, 0, ssid->passphrase);
				}
				break;
#endif
			default:
				break;
		}
	}
}

atbm_uint8 atbm_get_wifimode(struct atbmwifi_vif * priv)
{
	return priv->iftype;
}

atbm_uint32 atbm_os_random()
{
	atbm_uint32 data = atbm_random()/3;
	return (data>>1);
}

