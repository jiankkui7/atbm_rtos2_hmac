#include "atbm_hal.h"
#include "atbm_os_skbuf.h"
#include "tcpip_api.h"
#include "wifi_api.h"

#ifndef CONFIG_ETH_MTU
#define CONFIG_ETH_MTU                                                    1500
#endif

#define DEBUG_TEST_CODE 0

#if DEBUG_TEST_CODE
static atbm_uint32 recv_time;
static atbm_uint32 send_time;
#endif

struct eth_hdr
{
    atbm_uint8 dest[ATBM_ETH_ALEN];
    atbm_uint8 src[ATBM_ETH_ALEN];
    atbm_uint16 type;
};

extern struct atbmwifi_common g_hw_prv;

atbm_void atbm_skbbuffer_init()
{
}

struct atbm_net_device * atbm_alloc_netdev(atbm_int32 size)
{
	struct atbm_net_device *  netdev = atbm_kmalloc(size + sizeof(struct atbm_net_device),GFP_KERNEL);

	ATBM_ASSERT((netdev != ATBM_NULL));
	if(netdev)
		atbm_memset(netdev,0,(size + sizeof(struct atbm_net_device)));

	netdev->nif = atbm_kmalloc(sizeof(ATBM_NETIF),GFP_KERNEL);
	ATBM_ASSERT(netdev->nif != ATBM_NULL);
	if(netdev->nif)
		atbm_memset(netdev->nif,0,sizeof(ATBM_NETIF));
	atbm_os_init_waitevent(&netdev->tx_enable);
	atbm_os_init_waitevent(&netdev->rx_enable);
	wifi_printk(WIFI_ALWAYS,"atbm_alloc_netdev,netdev(%x),nif(%x),priv(%x)\n",netdev,netdev->nif,&netdev->drv_priv[0]);
	return  netdev;
}

atbm_void *netdev_drv_priv(struct atbm_net_device *ndev)
{
	return &ndev->drv_priv[0];

}
atbm_void atbm_free_netdev(struct atbm_net_device * netdev)
{
	if(netdev != ATBM_NULL){
		if(netdev->nif){
			atbm_kfree(netdev->nif);
		}
		atbm_os_delete_waitevent(&netdev->tx_enable);
		atbm_os_delete_waitevent(&netdev->rx_enable);
		atbm_kfree(netdev);
	}
}

atbm_void atbm_set_netif(struct netif *pNetIf)
{
}
atbm_void atbm_netdev_registed(struct netif *pNetIf)
{
	
}

static int atbm_wifi_tx_pkt_netvif(const TCPIP_PACKET_INFO_T *skb);
static int atbm_set_multicast(TCPIP_NETID_T netid, atbm_uint8* mc_mac, atbm_uint8 num);

int atbm_wifi_netif_init(struct atbm_net_device *dev)
{
	struct atbmwifi_vif *priv = netdev_drv_priv(dev);
	TCPIP_NETIF_CFG_T netif_cfg = {0};

	switch (priv->iftype) {
	case ATBM_NL80211_IFTYPE_P2P_CLIENT:
	case ATBM_NL80211_IFTYPE_STATION:
		netif_cfg.haddr.addr_ptr = priv->mac_addr;
		netif_cfg.haddr.addr_len = ATBM_ETH_ALEN;
		netif_cfg.pkt_type		 = TCPIP_PKTTYPE_ETHER;
		netif_cfg.tx_req_fptr	 = atbm_wifi_tx_pkt_netvif;
		netif_cfg.set_mcast_fptr = atbm_set_multicast;
		netif_cfg.is_async_tx	 = FALSE;
		netif_cfg.enable_ip4	 = TRUE;
		netif_cfg.enable_ip6	 = FALSE;
		netif_cfg.saddr.ipaddr	 = 0;
		netif_cfg.saddr.gateway  = 0;
		netif_cfg.saddr.snmask	 = 0;
		netif_cfg.saddr.dns1	 = 0;
		netif_cfg.saddr.dns2	 = 0;
		netif_cfg.mtu			 = CONFIG_ETH_MTU;
		netif_cfg.rx_flctrl_fptr = NULL;
		netif_cfg.netif_name_ptr = CONFIG_WIFI_STA_DRV_NAME;
		netif_cfg.tx_rate		 = 16000;
#ifdef CONFIG_WIFI_ZERO_COPY
		netif_cfg.is_pkt_zero_cpy = TRUE;
		wifi_printk(WIFI_DBG_MSG, "wifi zero copy\n");
#else /* CONFIG_WIFI_ZERO_COPY */
		netif_cfg.is_pkt_zero_cpy = FALSE;
#endif /* CONFIG_WIFI_ZERO_COPY */
		dev->nif->netid = TCPIP_RegNetInterface(&netif_cfg);
		wifi_printk(WIFI_DBG_ERROR, "%s net_id %u\n", __func__, dev->nif->netid);
		sci_setip6autoip(dev->nif->netid, FALSE, 0, 0);
		break;

	case ATBM_NL80211_IFTYPE_P2P_GO:
	case ATBM_NL80211_IFTYPE_AP:
		// TODO: register ipv4 or ipv6 due to PDP retval net id.
		netif_cfg.haddr.addr_ptr = priv->mac_addr;
		netif_cfg.haddr.addr_len = ATBM_ETH_ALEN;
		netif_cfg.pkt_type		 = TCPIP_PKTTYPE_ETHER;
		netif_cfg.tx_req_fptr	 = atbm_wifi_tx_pkt_netvif;
		netif_cfg.set_mcast_fptr = atbm_set_multicast;
		netif_cfg.is_async_tx	 = FALSE;
		netif_cfg.enable_ip4	 = TRUE;
		netif_cfg.enable_ip6	 = FALSE;
		//netif_cfg.act_as_host	= TRUE;
/*
		#define CONFIG_WIFI_AP_IPV4_ADDR                             0xc0a82801
		#define CONFIG_WIFI_AP_IPV4_NETMASK                          0xffffff00
		netif_cfg.saddr.ipaddr   = atbm_htonl(CONFIG_WIFI_AP_IPV4_ADDR);
		netif_cfg.saddr.gateway  = atbm_htonl(CONFIG_WIFI_AP_IPV4_ADDR);
		netif_cfg.saddr.snmask	 = atbm_htonl(CONFIG_WIFI_AP_IPV4_NETMASK);
		netif_cfg.saddr.dns1	 = atbm_htonl(CONFIG_WIFI_AP_IPV4_ADDR);
		netif_cfg.saddr.dns2	 = atbm_htonl(CONFIG_WIFI_AP_IPV4_ADDR);
*/
		netif_cfg.mtu			 = CONFIG_ETH_MTU;
		netif_cfg.rx_flctrl_fptr = NULL;
		netif_cfg.netif_name_ptr = CONFIG_WIFI_AP_DRV_NAME;
		netif_cfg.tx_rate		 = 16000;
		netif_cfg.act_as_host = TRUE;
#ifdef CONFIG_WIFI_ZERO_COPY
		netif_cfg.is_pkt_zero_cpy = TRUE;
#else /* CONFIG_WIFI_ZERO_COPY */
		netif_cfg.is_pkt_zero_cpy = FALSE;
#endif /* CONFIG_WIFI_ZERO_COPY */
		dev->nif->netid = TCPIP_RegNetInterface(&netif_cfg);
		wifi_printk(WIFI_DBG_MSG, "%s get ap mode net_id =%u \n", __func__, dev->nif->netid );
		break;
	default:
		wifi_printk(WIFI_DBG_ERROR, "Not supported mode: %d\n",priv->iftype);
		break;
	}
}

/**
 * @brief  wifi_netif_deinit, remove netif of wifi
 * @param  : 
 * @retval void
 */
void atbm_wifi_netif_deinit(struct atbm_net_device *dev)
{
	TCPIP_DeregNetInterface(dev->nif->netid);
	dev->nif->netid = 0;
}



atbm_void atbm_lwip_init(struct atbm_net_device *dev)
{
	//hal_create_mutex(&lwip_mutex);
}

atbm_void atbm_lwip_enable(struct atbm_net_device *dev)
{
	int queue_id;
	struct atbmwifi_vif *priv = netdev_drv_priv(dev);
	struct atbmwifi_common *hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);

	wifi_printk(WIFI_ALWAYS, "atbm_lwip_enable!!!\n");
	if(!dev->lwip_enable){
		dev->lwip_enable=1;
		dev->lwip_queue_enable=1;
		atbm_os_wakeup_event(&dev->tx_enable);
		wifi_printk(WIFI_ALWAYS, "atbm_lwip_enable1!!!\n");
		atbm_wifi_netif_init(dev);

		for(queue_id = ATBM_IEEE80211_AC_VO; queue_id <= ATBM_IEEE80211_AC_BK; queue_id++){
			if(hw_priv->tx_queue[queue_id].overfull == ATBM_TRUE){
				dev->lwip_queue_enable=0;
			}
		}
		if(atbmwifi_is_sta_mode(priv->iftype)){
			AtbmWifiConnectCallback();
		}else{
			AtbmHostApOnCallback(dev->nif->netid);
		}
	}
}

atbm_void atbm_lwip_disable(struct atbm_net_device *dev)
{
	if(dev->lwip_enable){
		struct atbmwifi_vif *priv = netdev_drv_priv(dev);
		dev->lwip_enable=0;
		dev->lwip_queue_enable=0;
		atbm_wifi_netif_deinit(dev);
		if(atbmwifi_is_sta_mode(priv->iftype)){
			AtbmWifiDisconnectCallback();
		}else{
			AtbmHostApOffCallback();
		}
	}
}

atbm_void atbm_lwip_txdone(struct atbm_net_device *dev)
{
}

atbm_void atbm_lwip_wake_queue(struct atbm_net_device *dev,int num)
{
	if(!dev->lwip_queue_enable && dev->lwip_enable){
		dev->lwip_queue_enable = 1;
		atbm_os_wakeup_event(&dev->tx_enable);
	}
}

atbm_void atbm_lwip_stop_queue(struct atbm_net_device *dev,int num)
{
	if(dev->lwip_queue_enable && dev->lwip_enable){
		//hal_wait_for_mutex(&lwip_mutex,2000);
		dev->lwip_queue_enable = 0;
	}
}

atbm_void *atbm_get_priv_fromid(int netid)
{
	struct atbmwifi_vif *priv;
	int if_id;
	atbm_for_each_vif(&g_hw_prv,priv,if_id){
		if(priv->ndev->nif->netid == netid)
			return priv;
	}
	return ATBM_NULL;
}

/****************************************************************************
* Function:   	atbm_wifi_tx_pkt
*
* Purpose:   	This function is used to send packet to wifi driver
*
* Parameters: point to buffer of packet
*
* Returns:	None.
******************************************************************************/
static int atbm_wifi_tx_pkt_netvif(const TCPIP_PACKET_INFO_T *skb)
{
	struct atbm_buff *AtbmBuf = ATBM_NULL;
	struct atbmwifi_vif *priv;
	struct atbmwifi_common * hw_priv = priv->hw_priv;
#if DEBUG_TEST_CODE
	atbm_uint8 dhcpMagic[4] = {0x63,0x82,0x53,0x63};
	atbm_uint8 *pdata;
	atbm_uint16 data_len;
	atbm_uint32 interval;
#endif

	wifi_printk(WIFI_DBG_MSG, "atbm_wifi_tx_pkt_netvif:\n");

	priv = atbm_get_priv_fromid(skb->net_id);
	if(priv == ATBM_NULL)
	{
		return -1;
	}

	//If the atbmQueue is full,pls drop??? 
	while(!priv->ndev->lwip_queue_enable){
		atbm_os_wait_event_timeout(&priv->ndev->tx_enable, HZ);
	}

	AtbmBuf = atbm_dev_alloc_skb(1600);
	if (!AtbmBuf)
	{
		ATBM_BUG_ON(1);
		wifi_printk(WIFI_TX,"<ERROR> tx_pkt alloc skb \n");
		return;
	}

	while (skb) {
		/*Here should copy pubf chain packet to atbmBuf*/
		atbm_memcpy(atbm_skb_put(AtbmBuf, skb->data_len), skb->data_ptr, skb->data_len);
		skb = skb->frag_next;
	}

#if DEBUG_TEST_CODE
	pdata = ATBM_OS_SKB_DATA(AtbmBuf);
	data_len = ATBM_OS_SKB_LEN(AtbmBuf);

	wifi_printk(WIFI_ALWAYS,"atbm: wifi TX, len=%d \n", data_len);
	
	//dump_mem(pdata, data_len);
	
	if((pdata[12] == 0x08) && (pdata[13] == 0x00) && (data_len > 282) &&
		(0 == atbm_memcmp(&pdata[278], dhcpMagic, 4))) 
		//(((pdata[34] == 0) && (pdata[35] == 68) && (pdata[36] == 0) && (pdata[37] == 67)) ||
		//((pdata[34] == 0) && (pdata[35] == 67) && (pdata[36] == 0) && (pdata[37] == 68))))
	{
		wifi_printk(WIFI_ALWAYS,"TX DHCP %d.%d.%d.%d to %d.%d.%d.%d, opt %d\n",
			pdata[26],pdata[27],pdata[28],pdata[29],pdata[30],pdata[31],pdata[32],pdata[33],pdata[42]);
		wifi_printk(WIFI_ALWAYS,"TX DHCP %02x:%02x:%02x:%02x:%02x:%02x to %02x:%02x:%02x:%02x:%02x:%02x\n",
			pdata[6],pdata[7],pdata[8],pdata[9],pdata[10],pdata[11],
			pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[284]);
	}


	if((pdata[12] == 0x08) && (pdata[13] == 0x00) && (pdata[14] == 0x45) &&
		(pdata[23] == 0x01) && (pdata[35] == 0x00))
	{
		wifi_printk(WIFI_ALWAYS,"TX PING %d.%d.%d.%d to %d.%d.%d.%d\n",
			pdata[26],pdata[27],pdata[28],pdata[29],pdata[30],pdata[31],pdata[32],pdata[33]);
		wifi_printk(WIFI_ALWAYS,"TX PING %02x:%02x:%02x:%02x:%02x:%02x to %02x:%02x:%02x:%02x:%02x:%02x\n",
			pdata[6],pdata[7],pdata[8],pdata[9],pdata[10],pdata[11],
			pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5]);
	}
	
	send_time = atbm_GetOsTimeMs();
	//send_time = atbm_GetOsTime();
	interval = send_time - recv_time;
	wifi_printk(WIFI_ALWAYS,"[%d] atbm: reply interval=%d ms\n", send_time, interval);
#endif

	if(priv->ndev && priv->ndev->netdev_ops){
		priv->ndev->netdev_ops->ndo_start_xmit(priv, AtbmBuf);
	}else{
		ATBM_BUG_ON(1);
		atbm_dev_kfree_skb(AtbmBuf);
	}

	return 0;
}

static int atbm_set_multicast(TCPIP_NETID_T netid, atbm_uint8* mc_mac, atbm_uint8 num)
{
	return 0;
}


static int atbm_wifi_rx_pkt(struct atbm_net_device *dev, struct atbm_buff *at_skb)
{
	TCPIP_PACKET_INFO_T  pkt = {0};
	struct eth_hdr *eth;
	atbm_uint8 *pdata = ATBM_OS_SKB_DATA(at_skb);

#if DEBUG_TEST_CODE	
	atbm_uint8 dhcpMagic[4] = {0x63,0x82,0x53,0x63};
	atbm_uint16 data_len = at_skb->dlen;
	
	wifi_printk(WIFI_ALWAYS,"atbm: wifi RX, len=%d \n", data_len);

	if((pdata[12] == 0x08) && (pdata[13] == 0x00) && (data_len >= 282) &&
		(0 == atbm_memcmp(&pdata[278], dhcpMagic, 4)))
		//(((pdata[34] == 0) && (pdata[35] == 68) && /*src port = 0x0044*/
		//(pdata[36] == 0) && (pdata[37] == 67)) || /*dst port = 0x0043*/
		//((pdata[34] == 0) && (pdata[35] == 67) && /*src port = 0x0043*/
		//(pdata[36] == 0) && (pdata[37] == 68)))) /*dst port = 0x0044*/
	{
		wifi_printk(WIFI_ALWAYS,"RX DHCP %d.%d.%d.%d to %d.%d.%d.%d, MsgType: %d\n",
			pdata[26],pdata[27],pdata[28],pdata[29],pdata[30],pdata[31],pdata[32],pdata[33], pdata[42]);
		wifi_printk(WIFI_ALWAYS,"RX DHCP %02x:%02x:%02x:%02x:%02x:%02x to %02x:%02x:%02x:%02x:%02x:%02x, opt %d\n",
			pdata[6],pdata[7],pdata[8],pdata[9],pdata[10],pdata[11],
			pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5],pdata[284]);
	}

	if((pdata[12] == 0x08) && (pdata[13] == 0x00) && /*type ipv4 = 0x0800*/
		(pdata[14] == 0x45) && /*version = 0x4, header = 0x5*/
		(pdata[23] == 0x01) && /*ICMP*/
		(pdata[35] == 0x00)) /*code = 0x0*/
	{
		wifi_printk(WIFI_ALWAYS,"RX PING %d.%d.%d.%d to %d.%d.%d.%d\n",
			pdata[26],pdata[27],pdata[28],pdata[29],pdata[30],pdata[31],pdata[32],pdata[33]);
		
		wifi_printk(WIFI_ALWAYS,"RX PING %02x:%02x:%02x:%02x:%02x:%02x to %02x:%02x:%02x:%02x:%02x:%02x\n",
			pdata[6],pdata[7],pdata[8],pdata[9],pdata[10],pdata[11],
			pdata[0],pdata[1],pdata[2],pdata[3],pdata[4],pdata[5]);
	}

	recv_time = atbm_GetOsTimeMs();
	//recv_time = atbm_GetOsTime();
#endif

	if (!at_skb || ATBM_OS_SKB_LEN(at_skb) == 0) {
		atbm_dev_kfree_skb(at_skb);
		return -ATBM_EINVAL;
	}

	eth = (struct eth_hdr *)pdata;
	if (eth->type == htons(0x86DDU))
		if(atbm_compare_ether_addr(pdata, pdata + ATBM_ETH_ALEN)) {
			wifi_printk(WIFI_DBG_MSG, "%s, drop loopback pkt, macaddr:%02x:%02x:"
				"%02x:%02x:%02x:%02x\n", __func__, pdata[0],
				pdata[1], pdata[2], pdata[3], pdata[4], pdata[5]);
			atbm_dev_kfree_skb(at_skb);
			return -ATBM_EFAULT;
		}

// TODO: tcp_ack
//	sprdwl_fileter_rx_tcp_ack(pdata);

	pkt.data_ptr = pdata;
	pkt.data_len = ATBM_OS_SKB_LEN(at_skb);
	pkt.pkt_type = TCPIP_PKTTYPE_ETHER;
	pkt.net_id = dev->nif->netid;
	wifi_printk(WIFI_DBG_MSG, "%s tcpip nid %u\n", __func__, dev->nif->netid);
	TCPIP_RxInd(&pkt);
	atbm_dev_kfree_skb(at_skb);
	return 0;
}

atbm_void atbm_lwip_task_event(struct atbm_net_device *dev)
{
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
