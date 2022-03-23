/**************************************************************************************************************
 * altobeam LINUX wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include "atbm_hal.h"
/******** Functions below is Wlan API **********/
#include "atbm_sysops.h"

#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio.h>
#include<linux/platform_device.h>

#if (ATBM_WIFI_PLATFORM == PLATFORM_XUNWEI)
#define CARD_INSERT_GPIO EXYNOS4_GPX3(2)
#endif
#if (ATBM_WIFI_PLATFORM == PLATFORM_SUN6I)
#include <mach/sys_config.h>
#include <mach/gpio.h>
#include <linux/regulator/consumer.h>
#include <mach/sys_config.h>
//#include <linux/platform_device.h>
#endif
#if (ATBM_WIFI_PLATFORM == 10)
extern int rockchip_wifi_power(int on);
extern int rockchip_wifi_set_carddetect(int val);
#endif
#if ((ATBM_WIFI_PLATFORM == PLATFORM_AMLOGIC_S805) || (ATBM_WIFI_PLATFORM == PLATFORM_AMLOGIC_905))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
extern void wifi_teardown_dt(void);
extern int wifi_setup_dt(void);
#endif
#endif //#if (ATBM_WIFI_PLATFORM == PLATFORM_AMLOGIC_S805)
#if (ATBM_WIFI_PLATFORM == PLATFORM_FRIENDLY)
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/ioport.h>
#include <plat/gpio-cfg.h>
#include <plat/sdhci.h>
#include <plat/devs.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <mach/board-wlan.h>
#endif
#if (ATBM_WIFI_PLATFORM == PLATFORM_SUN6I_64)
extern void sunxi_mmc_rescan_card(unsigned ids);
extern void sunxi_wlan_set_power(int on);
extern int sunxi_wlan_get_bus_index(void);
//extern int sunxi_wlan_get_oob_irq(void);
//extern unsigned int oob_irq;
#endif

struct atbm_platform_data {
	const char *mmc_id;
	const int irq_gpio;
	const int power_gpio;
	const int reset_gpio;
	int (*power_ctrl)(const struct atbm_platform_data *pdata,
			  bool enable);
	int (*clk_ctrl)(const struct atbm_platform_data *pdata,
			  bool enable);
	int (*insert_ctrl)(const struct atbm_platform_data *pdata,
			  bool enable);
};

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

static int atbm_platform_power_ctrl(const struct atbm_platform_data *pdata,bool enabled)
{
	int ret = 0; 

	#if (ATBM_WIFI_PLATFORM == PLATFORM_XUNWEI) ||(ATBM_WIFI_PLATFORM == PLATFORM_FRIENDLY)
	{
#if (PROJ_TYPE==ARES_A)//for ARESA chip hw reset pin bug
	enabled = 1;
#endif  //#if (PROJ_TYPE>=ARES_A)
		#ifndef WIFI_FW_DOWNLOAD
		enabled = 1;
		#endif //#ifdef WIFI_FW_DOWNLOAD
		if (gpio_request(pdata->power_gpio, "WIFI_POWERON")!=0) {
			wifi_printk(WIFI_ALWAYS,"[altobeam] ERROR:Cannot request WIFI_POWERON\n");
		} else {
			gpio_direction_output(pdata->power_gpio, 1);/* WLAN_CHIP_PWD */
			gpio_set_value(pdata->power_gpio, enabled);
			mdelay(1);
			gpio_free(pdata->power_gpio);
			wifi_printk(WIFI_ALWAYS,"[altobeam] + %s :EXYNOS4_GPC1(0) wlan powerwew %s\n",__func__, enabled?"on":"off");
		}
	}
	#endif //(ATBM_WIFI_PLATFORM == PLATFORM_XUNWEI) ||(ATBM_WIFI_PLATFORM == PLATFORM_FRIENDLY)

	#if (ATBM_WIFI_PLATFORM == PLATFORM_SUN6I)
	{
		extern void wifi_pm_power(int on);
		pdata = pdata;
		//just because sun6I gpio bug, wifi_pm_power(0) ,gpio input val will error,so not call poweroff
		if(enabled)
		wifi_pm_power(enabled);
	}
	#endif

	#if (ATBM_WIFI_PLATFORM == PLATFORM_FRIENDLY)
	{
		
	}
	#endif
#if (ATBM_WIFI_PLATFORM == PLATFORM_SUN6I_64)	
	//int wlan_bus_index = sunxi_wlan_get_bus_index();
	mdelay(100);
	sunxi_wlan_set_power(enabled);
	//gpio_free(354);	gpio_free(355);
#endif
#if ((ATBM_WIFI_PLATFORM == PLATFORM_AMLOGIC_S805) || (ATBM_WIFI_PLATFORM == PLATFORM_AMLOGIC_905))

extern void sdio_reinit(void);
extern void extern_wifi_set_enable(int is_on);
	if (enabled) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
	    if (wifi_setup_dt()) {
	        wifi_printk(WIFI_ALWAYS,"%s : fail to setup dt\n", __func__);
	    }
#endif
		extern_wifi_set_enable(0);
		msleep(200);
		extern_wifi_set_enable(1);
		msleep(200);
		sdio_reinit();
		wifi_printk(WIFI_ALWAYS,"atbm sdio extern_wifi_set_enable 1\n");
	} else {
		extern_wifi_set_enable(0);
		msleep(200);
		wifi_printk(WIFI_ALWAYS,"atbm sdio extern_wifi_set_enable 0\n");
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
    	wifi_teardown_dt();
#endif
	}

#endif//PLATFORM_AMLOGIC_S805

#if (ATBM_WIFI_PLATFORM == 10)
	mdelay(100);
    rockchip_wifi_power(enabled);
#endif

	wifi_printk(WIFI_ALWAYS,"platform set power [%d]\n",enabled);
	return ret;
}

static int atbm_platform_insert_crtl(const struct atbm_platform_data *pdata,bool enabled)
{
	int ret = 0;

	#if (ATBM_WIFI_PLATFORM == PLATFORM_XUNWEI)
	{
		int outValue;

		if (enabled) {
			outValue = 0;
		} else {
			outValue = 1;
		}

		mdelay(10);
		if (gpio_request(CARD_INSERT_GPIO, "WIFI_GPIO2")!=0) {
			wifi_printk(WIFI_ALWAYS,"[altobeam] ERROR:Cannot request WIFI_GPIO2\n");
		} else {
			gpio_direction_output(CARD_INSERT_GPIO, 1);/* SDIO_CARD_PWD */
			gpio_set_value(CARD_INSERT_GPIO, outValue);
			gpio_free(CARD_INSERT_GPIO);
			wifi_printk(WIFI_ALWAYS,"[altobeam] + %s : wlan card %s\n",__func__, enabled?"insert":"remmove");
		}
	}
	#endif

	#if (ATBM_WIFI_PLATFORM == PLATFORM_SUN6I)
	{
		extern void sw_mci_rescan_card(unsigned id, unsigned insert);
		script_item_u val;
		script_item_value_type_e type;
		static int sdc_id = -1;
		pdata = pdata;
		type = script_get_item("wifi_para", "wifi_sdc_id", &val);
		sdc_id = val.val;
		wifi_printk(WIFI_ALWAYS,"scan scd_id(%d)\n",sdc_id);
		sw_mci_rescan_card(sdc_id, enabled);
	}
	#endif

	#if (ATBM_WIFI_PLATFORM == PLATFORM_FRIENDLY)
	{
		atbm_sdio_insert(enabled);
	}
	#endif
	#if (ATBM_WIFI_PLATFORM == PLATFORM_SUN6I_64)
	{
		int wlan_bus_index = sunxi_wlan_get_bus_index();
		if(wlan_bus_index < 0)
			return wlan_bus_index;
		if (enabled){
			sunxi_mmc_rescan_card(wlan_bus_index);
		}else{		
	
		}
	
		//oob_irq = sunxi_wlan_get_oob_irq();
	}
	#endif
#if (ATBM_WIFI_PLATFORM == 10)
     mdelay(100);
     rockchip_wifi_set_carddetect(enabled);
#endif
	 wifi_printk(WIFI_ALWAYS,"platform insert ctrl [%d]\n",enabled);
	return ret;
}

int atbm_power_ctrl(const struct atbm_platform_data *pdata,bool enabled)
{
	return atbm_platform_power_ctrl(pdata,enabled);

}
int atbm_insert_crtl(const struct atbm_platform_data *pdata,bool enabled)
{	
	return atbm_platform_insert_crtl(pdata,enabled);
}

struct atbm_platform_data platform_data = {
#if (ATBM_WIFI_PLATFORM == 10)
	.mmc_id       = "mmc1",
#else
	.mmc_id       = "mmc2",
#endif
	.clk_ctrl     = NULL,
	.power_ctrl   = atbm_power_ctrl,
	.insert_ctrl  = atbm_insert_crtl,
#if(ATBM_WIFI_PLATFORM == PLATFORM_XUNWEI)
	.irq_gpio	= EXYNOS4_GPX2(4),
	.power_gpio	= EXYNOS4_GPC1(1),
#endif
#if(ATBM_WIFI_PLATFORM == PLATFORM_AMLOGIC_S805)
	.irq_gpio	= INT_GPIO_4,
	.power_gpio	= 0,
#endif
#if(ATBM_WIFI_PLATFORM == PLATFORM_AMLOGIC_905)
	.irq_gpio	= 100,
	.power_gpio	= 0,
#endif
#if(ATBM_WIFI_PLATFORM == PLATFORM_FRIENDLY)
	.power_gpio	= EXYNOS4_GPK3(2),
#endif
	.reset_gpio = 0,
};

struct atbm_platform_data *atbm_get_platform_data(void)
{
	return &platform_data;
}


#if ATBM_USB_BUS

extern int atbm_usb_probe(struct atbm_usb_interface *intf,const struct atbm_usb_device_id *id);
extern atbm_void atbm_usb_disconnect(struct atbm_usb_interface *intf);

static const struct usb_device_id atbm_usb_ids[] = {
	/* Asus */
	{USB_DEVICE(0x0906, 0x5678)},
	{USB_DEVICE(0x007a, 0x8888)},
	{ /* end: all zeroes */}
};

static struct usb_driver atmbwifi_driver = {
	.name		= "atbm_wlan",
	.id_table	= atbm_usb_ids,
	.probe		= atbm_usb_probe,
	.disconnect	= atbm_usb_disconnect,
	//.suspend	= atbm_usb_suspend,
	//.resume		= atbm_usb_resume,
	//.reset_resume	= atbm_resume,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,0))
	.disable_hub_initiated_lpm = 1,
#endif
	.supports_autosuspend = 1,
};

static int atbm_usb_off(const struct atbm_platform_data *pdata)
{
	int ret = 0;

	//if (pdata->insert_ctrl)
	//	ret = pdata->insert_ctrl(pdata, false);
	return ret;
}

static int atbm_usb_on(const struct atbm_platform_data *pdata)
{
	int ret = 0;

   // if (pdata->insert_ctrl)
	//	ret = pdata->insert_ctrl(pdata, true);

	return ret;
}

int atbm_usb_register_init()
{
	const struct atbm_platform_data *pdata;
	int ret;
	pdata = atbm_get_platform_data();

	if (pdata->clk_ctrl) {
		ret = pdata->clk_ctrl(pdata, true);
		if (ret)
			goto err_clk;
	}

	if (pdata->power_ctrl) {
		ret = pdata->power_ctrl(pdata, true);
		if (ret)
			goto err_power;
	}

	ret = atbm_usb_register(&atmbwifi_driver);
	if (ret){
		wifi_printk(WIFI_DBG_ERROR,"atbmwifi usb driver register error\n");	
		goto err_reg;
	}

	ret = atbm_usb_on(pdata);
	if (ret)
		goto err_on;

	return 0;

err_on:
	if (pdata->power_ctrl)
		pdata->power_ctrl(pdata, false);
err_power:
	if (pdata->clk_ctrl)
		pdata->clk_ctrl(pdata, false);
err_clk:
	atbm_usb_deregister(&atmbwifi_driver);
err_reg:
	return ret;
}

int atbm_usb_register_deinit()
{
	const struct atbm_platform_data *pdata;
	pdata = atbm_get_platform_data();
	atbm_usb_deregister(&atmbwifi_driver);
	atbm_usb_off(pdata);
	if (pdata->power_ctrl)
		pdata->power_ctrl(pdata, false);
	if (pdata->clk_ctrl)
		pdata->clk_ctrl(pdata, false);
}
#elif ATBM_SDIO_BUS

extern int atbm_sdio_probe(struct atbm_sdio_func *func,const struct atbm_sdio_device_id *id);
extern void atbm_sdio_disconnect(struct atbm_sdio_func *func);

static const struct sdio_device_id atbm_sdio_ids[] = {
	{ SDIO_DEVICE(SDIO_ANY_ID, SDIO_ANY_ID) },
	{ /* end: all zeroes */			},
};

static struct sdio_driver atmbwifi_driver = {
	.name		= "atbm_wlan",
	.id_table	= atbm_sdio_ids,
	.probe		= atbm_sdio_probe,
	.remove		= atbm_sdio_disconnect,
};

#if ((ATBM_WIFI_PLATFORM != 10) && (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_S805) \
	&& (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_905))
static int atbm_detect_card(const struct atbm_platform_data *pdata)
{
	/* HACK!!!
	 * Rely on mmc->class_dev.class set in mmc_alloc_host
	 * Tricky part: a new mmc hook is being (temporary) created
	 * to discover mmc_host class.
	 * Do you know more elegant way how to enumerate mmc_hosts?
	 */

	struct mmc_host *mmc = NULL;
	struct class_dev_iter iter;
	struct device *dev;
	static struct platform_device *sdio_platform_dev = NULL;
	int status = 0;
	
	sdio_platform_dev = platform_device_alloc("atbmsdiowifi",0);
	if(sdio_platform_dev == NULL){
		status = -ENOMEM;
		goto platform_dev_err;
	}

	if(platform_device_add(sdio_platform_dev) != 0){
		status = -ENOMEM;
		goto platform_dev_err;
	}
	
	mmc = mmc_alloc_host(0, &sdio_platform_dev->dev);
	
	if (!mmc){
		status = -ENOMEM;
		goto exit;
	}

	BUG_ON(!mmc->class_dev.class);
	class_dev_iter_init(&iter, mmc->class_dev.class, NULL, NULL);
	for (;;) {
		dev = class_dev_iter_next(&iter);
		if (!dev) {
			wifi_printk(WIFI_DBG_ERROR,"atbm: %s is not found.\n",
				pdata->mmc_id);
			break;
		} else {
			struct mmc_host *host = container_of(dev,
				struct mmc_host, class_dev);

			wifi_printk(WIFI_ALWAYS,"apollo:  found. %s\n",
				dev_name(&host->class_dev));

			if (dev_name(&host->class_dev) &&
				strcmp(dev_name(&host->class_dev),
					pdata->mmc_id))
				continue;

			if(host->card == NULL)
				mmc_detect_change(host, 10);
			else
				wifi_printk(WIFI_DBG_ERROR,"%s has been attached\n",pdata->mmc_id);
			break;
		}
	}
	mmc_free_host(mmc);
exit:
	if(sdio_platform_dev)
		platform_device_unregister(sdio_platform_dev);
	return status;
platform_dev_err:
	if(sdio_platform_dev)
		platform_device_put(sdio_platform_dev);
	return status;
}
#endif //PLATFORM_AMLOGIC_S805

static int atbm_sdio_off(const struct atbm_platform_data *pdata)
{
	int ret = 0;

	if (pdata->insert_ctrl)
		ret = pdata->insert_ctrl(pdata, false);
	return ret;
}

#if ((ATBM_WIFI_PLATFORM != 10) && (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_S805) \
	&& (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_905))
static int atbm_sdio_on(const struct atbm_platform_data *pdata)
{
	int ret = 0;
    if (pdata->insert_ctrl)
		ret = pdata->insert_ctrl(pdata, true);
	msleep(200);
	atbm_detect_card(pdata);
	return ret;
}
#endif //#if ((ATBM_WIFI_PLATFORM != 10) && (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_S805))

int atbm_sdio_register_init()
{
	const struct atbm_platform_data *pdata;
	int ret;
	pdata = atbm_get_platform_data();

	if (pdata->clk_ctrl) {
		ret = pdata->clk_ctrl(pdata, true);
		if (ret)
			goto err_clk;
	}
	/*
	* modify for rockchip platform
	*/
#if (ATBM_WIFI_PLATFORM == 10)
	if (pdata->insert_ctrl&&pdata->power_ctrl)
	{
		ret = pdata->insert_ctrl(pdata, false);
		ret = pdata->power_ctrl(pdata, false);
		if (ret)
			goto err_power;
		ret = pdata->power_ctrl(pdata, true);
		if (ret)
			goto err_power;
		ret = pdata->insert_ctrl(pdata, true);
	}
	else
	{
		goto err_power;
	}
#else
	if (pdata->power_ctrl) {
		ret = pdata->power_ctrl(pdata, true);
		if (ret)
			goto err_power;
	}
#endif

	ret = atbm_sdio_register(&atmbwifi_driver);
	if (ret){
		wifi_printk(WIFI_DBG_ERROR,"atbmwifi sdio driver register error\n");	
		goto err_reg;
	}

#if ((ATBM_WIFI_PLATFORM != 10) && (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_S805)\
		&& (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_905))
	ret = atbm_sdio_on(pdata);
	if (ret)
		goto err_on;
#endif

	return 0;
	
#if ((ATBM_WIFI_PLATFORM != 10) && (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_S805)\
		&& (ATBM_WIFI_PLATFORM != PLATFORM_AMLOGIC_905))
err_on:
	if (pdata->power_ctrl)
		pdata->power_ctrl(pdata, false);
#endif
err_power:
	if (pdata->clk_ctrl)
		pdata->clk_ctrl(pdata, false);
err_clk:
	atbm_sdio_deregister(&atmbwifi_driver);
err_reg:
	return ret;
}

int atbm_sdio_register_deinit()
{
	const struct atbm_platform_data *pdata;

	pdata = atbm_get_platform_data();

	atbm_sdio_deregister(&atmbwifi_driver);
	atbm_sdio_off(pdata);

	if (pdata->power_ctrl) {
		pdata->power_ctrl(pdata, false);
	}

	if (pdata->clk_ctrl) {
		pdata->clk_ctrl(pdata, false);
	}
	return 0;
}
#endif

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
	//atbm_uint32 data = atbm_random()/3;
	return (atbm_random());
}



#ifdef LINUX_OS
#include <linux/module.h>

static int atbm_module_init(void){
	atbm_wifi_hw_init();
	return 0;
}

static void atbm_module_exit(void){
	atbm_wifi_hw_deinit();
}


module_init(atbm_module_init);
module_exit(atbm_module_exit);
MODULE_LICENSE("GPL");
#endif

