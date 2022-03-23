
#include "atbm_hal.h"
#include "atbm_os_skbuf.h"

int lwip_queue_enable = 0;
extern struct atbmwifi_common g_hw_prv;
extern void atbm_wifi_intf_sta_set_link_status(int state);
extern void atbm_wifi_intf_uap_set_link_status(int state);
atbm_void * netdev_drv_priv(struct atbm_net_device *ndev)
{
	return &ndev->drv_priv[0];

}

atbm_void atbm_lwip_init(struct atbm_net_device *dev)
{
	//hal_create_mutex(&lwip_mutex);
	return;
}

atbm_void atbm_lwip_enable(struct atbm_net_device *dev)
{

	struct atbmwifi_vif *priv = ATBM_NULL;
	
	if(!dev){
		wifi_printk(WIFI_ALWAYS, "%s dev error\n", __func__);
		return;
	}
	
	dev->lwip_enable=1;
	dev->lwip_queue_enable=1;	
	
	priv = (struct atbmwifi_vif *)netdev_drv_priv(dev);

	wifi_printk(WIFI_ALWAYS, "%s ====>\n", __func__);

	if(atbm_memcmp(priv->if_name,ATBMWLAN,5) == 0)
		atbm_wifi_intf_sta_set_link_status(1);
	else if(atbm_memcmp(priv->if_name,ATBMP2P,4) == 0)
		atbm_wifi_intf_uap_set_link_status(1);
	else
		wifi_printk(WIFI_ALWAYS, "%s error\n", __func__);
	
	wifi_printk(WIFI_ALWAYS, "%s <====\n", __func__);
	return;
}

atbm_void atbm_lwip_disable(struct atbm_net_device *dev)
{
	struct atbmwifi_vif *priv = ATBM_NULL;
	
	if(!dev){
		wifi_printk(WIFI_ALWAYS, "%s dev error\n", __func__);
		return;
	}

	wifi_printk(WIFI_ALWAYS, "%s ====>\n", __func__);

	dev->lwip_enable=0;
	dev->lwip_queue_enable=0;

	priv = (struct atbmwifi_vif *)netdev_drv_priv(dev);

	wifi_printk(WIFI_ALWAYS, "%s ====>\n", __func__);

	if(atbm_memcmp(priv->if_name,ATBMWLAN,5) == 0)
		atbm_wifi_intf_sta_set_link_status(0);
	else if(atbm_memcmp(priv->if_name,ATBMP2P,4) == 0)
		atbm_wifi_intf_uap_set_link_status(0);
	else
		wifi_printk(WIFI_ALWAYS, "%s error\n", __func__);
	

	wifi_printk(WIFI_ALWAYS, "%s <====\n", __func__);
	return;
}

atbm_void atbm_lwip_txdone(struct atbm_net_device *dev)
{
	wifi_printk(WIFI_ALWAYS, "%s error, I can need to be active\n", __func__);

	return;
}

atbm_void atbm_lwip_wake_queue(struct atbm_net_device *dev,int num)
{
	wifi_printk(WIFI_ALWAYS, "%s ====>\n", __func__);
	if(!dev->lwip_queue_enable && dev->lwip_enable){
		dev->lwip_queue_enable = 1;
	}
	wifi_printk(WIFI_ALWAYS, "%s <====\n", __func__);

	return;
}

atbm_void atbm_lwip_stop_queue(struct atbm_net_device *dev,int num)
{

	wifi_printk(WIFI_ALWAYS, "%s ====>\n", __func__);
	if(dev->lwip_queue_enable && dev->lwip_enable){
		//hal_wait_for_mutex(&lwip_mutex,2000);
		dev->lwip_queue_enable = 0;
	}
	wifi_printk(WIFI_ALWAYS, "%s <====\n", __func__);

	return;
}

atbm_void atbm_lwip_task_event(struct atbm_net_device *dev)
{
	wifi_printk(WIFI_ALWAYS, "%s error, I can need to be active\n", __func__);

	return;
}

#if 0//#ifndef ATBM_COMB_IF
struct atbm_net_device * atbm_alloc_netdev(atbm_int32 size)
{
	struct atbm_net_device *  netdev = atbm_kmalloc(size + sizeof(struct atbm_net_device),GFP_KERNEL);

	ATBM_ASSERT((netdev != ATBM_NULL));
	if(netdev)
		atbm_memset(netdev,0,(size + sizeof(struct atbm_net_device)));

	netdev->nif =pOsNetIf;
	if(pOsNetIf == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"\n<ERROR> atbmwifi pOsNetIf not set yet may ERROR!!!!!!\n\n\n");
	}
	return  netdev;
}
#else
struct atbm_net_device * atbm_alloc_netdev(atbm_int32 size)
{
	struct atbm_net_device *  netdev = atbm_kmalloc(size + sizeof(struct atbm_net_device),GFP_KERNEL);

	ATBM_ASSERT((netdev != ATBM_NULL));
	if(netdev)
		atbm_memset(netdev,0,(size + sizeof(struct atbm_net_device)));

	netdev->nif = ATBM_NULL;
	//ATBM_ASSERT(netdev->nif != ATBM_NULL);
	wifi_printk(WIFI_ALWAYS,"atbm_alloc_netdev,netdev(%x),nif(%x)\n",netdev,netdev->nif);
	return  netdev;
}
#endif

atbm_void atbm_free_netdev(struct atbm_net_device * netdev)
{
	if(netdev != ATBM_NULL)
		atbm_kfree(netdev);

	return;
}


/****************************************************************************
* Function:   	atbm_wifi_tx_pkt
*
* Purpose:   	This function is used to send packet to wifi driver
*
* Parameters: 
*	pdata: point to buffer of data
*	data_len: the data length
*
* Returns:	None.
******************************************************************************/
atbm_int32 atbm_wifi_tx_pkt(atbm_int8 *pbuf, atbm_uint32 pktlen, atbm_int8 if_id)
{

	struct atbmwifi_vif *priv;
	struct atbm_buff *pSkbuf;
	struct atbm_net_device *netdev;

	wifi_printk(WIFI_ALWAYS,"atbm_wifi_tx_pkt()====>\n");
	
	/*******if_id**********/ 
	/****0: sta 1: ap*******/
	priv = g_hw_prv.vif_list[if_id];
	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "atbm_wifi_tx_pkt() wifi drv is not init!\n");
		return -1;
	}

	netdev = priv->ndev;
	if(netdev == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "atbm_wifi_tx_pkt() netdev is not init!\n");
		return -1;
	}

	//If the atbmQueue is full,pls drop??? 
	if(!netdev->lwip_queue_enable){
		wifi_printk(WIFI_ALWAYS, "atbm_wifi_tx_pkt() drop pkt\n");
		return 0;
	}
	
	pSkbuf = atbm_dev_alloc_skb(pktlen);
	if(pSkbuf == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "atbm_wifi_tx_pkt() skb alloc failed!!!\n");
		return -1;
	}

	/*Here should copy pubf chain packet to atbmBuf*/
	atbm_memcpy(atbm_skb_put(pSkbuf,pktlen), pbuf, pktlen);

	if(priv->ndev && priv->ndev->netdev_ops){
		priv->ndev->netdev_ops->ndo_start_xmit(priv, pSkbuf);
	}else{
		atbm_dev_kfree_skb(pSkbuf);
	}
	
	wifi_printk(WIFI_DBG_MSG,"atbm_wifi_tx_pkt()<====\n");
	atbm_wifi_intf_tx_packet_sent((char *)pbuf, pktlen, pktDesc, if_id);
	return 0;
}

extern void wifi_intf_get_rx_buffer(char** rxBuff, int* buffLen, void** buffDesc);
extern void atbm_wifi_uap_intf_rx_packet_recvd(char* pktBuff, unsigned int pktLen, void* buffDesc);
extern void atbm_wifi_intf_rx_packet_recvd(char* pktBuff, unsigned int pktLen, void* buffDesc);
atbm_uint32 atbm_wifi_rx_pkt(struct atbm_net_device *dev, struct atbm_buff *at_skb)
{
	char *dataBuf=ATBM_NULL;
	void *bufDesc=ATBM_NULL;
	int	bufLen=0;
	
	struct atbmwifi_vif *priv = (struct atbmwifi_vif *)netdev_drv_priv(dev);

	wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt()====>\n");

	if(at_skb == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt() at_skb is NULL\n");
		return WIFI_ERROR;
	}
	
	if(priv == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt() priv is NULL\n");
		goto free_skb;
	}

	//Malloc memory from TCP/IP
	wifi_intf_get_rx_buffer(&dataBuf, &bufLen, &bufDesc);
	if (bufDesc == ATBM_NULL) {
		wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt() bufDesc is NULL\n");
		goto free_skb;
	}
	if (dataBuf == ATBM_NULL) {
		wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt() dataBuf is NULL\n");
		goto free_skb;
	}
	if (bufLen == 0) {
		wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt() bufLen is zero\n");
		goto free_skb;
	}

	if(at_skb->dlen <= bufLen){
		atbm_memcpy(dataBuf, at_skb->abuf, at_skb->dlen);
	}else{
		at_skb->dlen = 0;
		wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt() len error %d, %d\n", at_skb->dlen, bufLen);
		goto free_skb;
	}

	//0: STA Interface 1:AP Interface
	if(priv->if_id == 0){
		atbm_wifi_intf_rx_packet_recvd(dataBuf, at_skb->dlen, bufDesc);
	}else if(priv->if_id == 1){
		atbm_wifi_uap_intf_rx_packet_recvd(dataBuf, at_skb->dlen, bufDesc);
	}

free_skb:
	atbm_dev_kfree_skb(at_skb);
	
	wifi_printk(WIFI_ALWAYS,"atbm_wifi_rx_pkt()<====\n");
	
	return WIFI_OK;

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
atbm_void atbm_skbbuffer_init(void)
{
	return;
}

