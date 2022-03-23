#include "atbm_hal.h"
#include "atbm_sysops.h"


extern struct atbmwifi_common g_hw_prv;

extern atbm_void* atbm_wifi_vif_get(int id);

int atbmwifi_event_OsCallback(atbm_void *prv,int eventid,atbm_void *param)
{
	struct atbmwifi_vif *priv = prv;

	if(atbmwifi_is_ap_mode(priv->iftype)){
		switch(eventid){
			case ATBM_WIFI_DEAUTH_EVENT:
				break;
			case ATBM_WIFI_AUTH_EVENT:
				break;
			case ATBM_WIFI_ASSOC_EVENT:
				break;
			case ATBM_WIFI_ASSOCRSP_TXOK_EVENT: 
				break;
			case ATBM_WIFI_DEASSOC_EVENT:
				break;
			case ATBM_WIFI_JOIN_EVENT:			
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
				break;
			case ATBM_WIFI_DEAUTH_EVENT:				
				break;
			case ATBM_WIFI_AUTH_EVENT:
				break;
			case ATBM_WIFI_ASSOC_EVENT:								
				break;
			case ATBM_WIFI_DEASSOC_EVENT:
				break;
			case ATBM_WIFI_JOIN_EVENT:
				break;
			case ATBM_WIFI_ENABLE_NET_EVENT:
				break;
			case ATBM_WIFI_WPS_SUCCESS:
				{
					struct atbm_wpa_ssid *ssid = (struct atbm_wpa_ssid *)param;
					wifi_printk(WIFI_ALWAYS, "ssid:%s ssidlen:%d key:%s keylen:%d mgmt:%d id:%d\n", ssid->ssid, ssid->ssid_len, ssid->passphrase, strlen(ssid->passphrase), ssid->key_mgmt, ssid->wep_tx_keyidx);
					wifi_ConnectAP_vif(priv->if_id, ssid->ssid, ssid->ssid_len, ssid->passphrase, strlen(ssid->passphrase), 0);
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

extern int atbm_sdio_probe(struct atbm_sdio_func *func,const struct atbm_sdio_device_id *id);
extern int atbm_sdio_disconnect(struct atbm_sdio_func *func);

static struct atbm_sdio_device_id atbm_sdio_ids[] = {
	{0},
};

static struct atbm_sdio_driver atmbwifi_driver = {
	.name		 = "atbm_wlan",
	.id_table	 = atbm_sdio_ids,
	.probe_func  = atbm_sdio_probe,
	.discon_func = atbm_sdio_disconnect,
};

int atbm_sdio_register_init()
{
	int ret = 0;

	ret = atbm_sdio_register(&atmbwifi_driver);
	if (ret){
		wifi_printk(WIFI_DBG_ERROR,"atbmwifi sdio driver register error\n");	
	}

	return ret;
}

int atbm_sdio_register_deinit()
{
	atbm_sdio_deregister(&atmbwifi_driver);
	return 0;
}

ATBM_NETIF *atbm_priv_get_netif(struct atbm_net_device *dev)
{
	return dev->nif;
}

atbm_uint8 atbm_get_wifimode(struct atbmwifi_vif * priv)
{
	return priv->iftype;
}

atbm_uint32 atbm_os_random(void)
{
	atbm_uint32 data = atbm_random()/3;
	return (data>>1);
}

