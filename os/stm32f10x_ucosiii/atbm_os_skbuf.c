#include "atbm_hal.h"

#include "atbm_os_skbuf.h"
#include "atbm_os_iw_wsm.h"


atbm_void atbm_lwip_init(struct atbm_net_device *dev)
{
	//hal_create_mutex(&lwip_mutex);
}

atbm_void atbm_lwip_enable(struct atbm_net_device *dev)
{
	int queue_id;
	struct atbmwifi_vif *priv = netdev_drv_priv(dev);
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);

	dev->lwip_enable=1;
	dev->lwip_queue_enable=1;

	for(queue_id = ATBM_IEEE80211_AC_VO; queue_id <= ATBM_IEEE80211_AC_BK; queue_id++){
		if(hw_priv->tx_queue[queue_id].overfull == ATBM_TRUE){
			dev->lwip_queue_enable=0;
		}
	}
}

atbm_void atbm_lwip_disable(struct atbm_net_device *dev)
{
	dev->lwip_enable=0;
	dev->lwip_queue_enable=0;
}

atbm_void atbm_lwip_txdone(struct atbm_net_device *dev)
{
}

atbm_void atbm_lwip_wake_queue(struct atbm_net_device *dev,int num)
{
	if(!dev->lwip_queue_enable && dev->lwip_enable){
		dev->lwip_queue_enable = 1;
	}
}

atbm_void atbm_lwip_stop_queue(struct atbm_net_device *dev,int num)
{
	if(dev->lwip_queue_enable && dev->lwip_enable){
		//hal_wait_for_mutex(&lwip_mutex,2000);
		dev->lwip_queue_enable = 0;
	}
}

atbm_void atbm_lwip_task_event(struct atbm_net_device *dev)
{
}

atbm_uint32 atbm_wifi_rx_pkt(struct atbm_net_device *dev, struct atbm_buff *at_skb)
{
	return 0;
}

struct tcpip_opt lwip_tcp_opt ={
	.net_init = atbm_lwip_init,
	.net_enable = atbm_lwip_enable,//
	.net_disable = atbm_lwip_disable,//
	.net_rx = atbm_wifi_rx_pkt,
	.net_tx_done =	atbm_lwip_txdone,
	.net_start_queue =	atbm_lwip_wake_queue,
	.net_stop_queue =	atbm_lwip_stop_queue,
	.net_task_event =	atbm_lwip_task_event,//
};

atbm_void atbm_skbbuffer_init()
{
}

static char *wifi_name[2] =
{
	"wlan0", "wlan1"
};

struct atbm_net_device * atbm_alloc_netdev(atbm_int32 size)
{
	struct atbm_net_device *netdev = (struct atbm_net_device *)atbm_kmalloc(size + sizeof(struct atbm_net_device),GFP_KERNEL);
	ATBM_ASSERT((netdev != ATBM_NULL));
	if(netdev)
		atbm_memset(netdev,0,(size + sizeof(struct atbm_net_device)));

	netdev->nif = atbm_kmalloc(sizeof(ATBM_NETIF),GFP_KERNEL);
	ATBM_ASSERT(netdev->nif != ATBM_NULL);
	if(netdev->nif)
		atbm_memset(netdev->nif,0,sizeof(ATBM_NETIF));
	wifi_printk(WIFI_ALWAYS,"atbm_alloc_netdev,netdev(%x),nif(%x)\n",netdev,netdev->nif);

	return  netdev;
}

atbm_void * netdev_drv_priv(struct atbm_net_device *ndev)
{
	return &ndev->drv_priv[0];
}

atbm_void atbm_free_netdev(struct atbm_net_device * netdev)
{
	if(netdev != ATBM_NULL){
		atbm_kfree(netdev);
	}
}

