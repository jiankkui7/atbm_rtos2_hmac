
#include "atbm_hal.h"

#define ATBM_URB_TASK 0

/*It's follow usbwlan.c defination*/
typedef enum
{
    ATBM_DRV_CONTROL_PIPE = 0,
    ATBM_DRV_DATA_IN_PIPE,
    ATBM_DRV_DATA_OUT_PIPE,
    ATBM_DRV_MAX_PIPES
} atbm_drv_pipe;

#if ATBM_URB_TASK
#define ATBM_URB_TX_MSGQ_NUM 4
static TX_QUEUE atbm_urb_tx_msgQ;
static atbm_urb_s atbm_urb_tx_msqQbuf[ATBM_URB_TX_MSGQ_NUM+1];

#define ATBM_URB_RX_MSGQ_NUM 8
static TX_QUEUE atbm_urb_rx_msgQ ;
static atbm_urb_s atbm_urb_rx_msqQbuf[ATBM_URB_RX_MSGQ_NUM+1];

static TX_THREAD atbm_urb_tx_thread;
static TX_THREAD atbm_urb_rx_thread;

#define ATBM_URB_TX_THREAD_PRIORITY		14
#define ATBM_URB_RX_THREAD_PRIORITY		14
#define ATBM_URB_TX_THREAD_STACKSIZE		4096
#define ATBM_URB_RX_THREAD_STACKSIZE		4096

static atbm_uint8 atbm_urb_tx_thread_stack[ATBM_URB_TX_THREAD_STACKSIZE] __attribute__ ((aligned (8)));
static atbm_uint8 atbm_urb_rx_thread_stack[ATBM_URB_RX_THREAD_STACKSIZE] __attribute__ ((aligned (8)));


static atbm_int32 atbm_urb_msgQ_init_state = 0;
#endif

struct atbm_usb_interface usb_intf;

extern struct atbmwifi_common g_hw_prv;

extern atbm_int32 atbm_usb_probe(struct atbm_usb_interface *intf,const struct atbm_usb_device_id *id);
extern atbm_void atbm_usb_disconnect(struct atbm_usb_interface *intf);

extern int __atbm_usb_suspend(struct sbus_priv *self);
extern int __atbm_usb_resume(struct sbus_priv *self);
atbm_int32 atbm_usb_suspend(atbm_void)
{
	struct sbus_priv *self = g_hw_prv.vif_list[0]->hw_priv->sbus_priv;
	__atbm_usb_suspend(self);
	return 0;

}

atbm_int32 atbm_usb_resume(atbm_void)
{
	struct sbus_priv *self = g_hw_prv.vif_list[0]->hw_priv->sbus_priv;
	__atbm_usb_resume(self);
	return 0;
}

atbm_int32 atbm_usb_register(atbm_void)
{
	wifi_printk(WIFI_ALWAYS, "atbm_usb_register ==>\n");

	atbm_memset(&usb_intf, 0, sizeof(struct atbm_usb_interface));
	atbm_usb_probe(&usb_intf, ATBM_NULL);
	
	wifi_printk(WIFI_ALWAYS, "atbm_usb_register <==\n");
	return 0;
}

atbm_void atbm_usb_deregister(atbm_void)
{
	wifi_printk(WIFI_ALWAYS, "atbm_usb_deregister ==>\n");
	atbm_usb_disconnect(&usb_intf);
	wifi_printk(WIFI_ALWAYS, "atbm_usb_deregister <==\n");
	return;
}

//#define atbm_usb_init_urb(a)        usb_init_urb(a)
atbm_urb_s *atbm_usb_alloc_urb(atbm_int32 iso_packets, atbm_int32 mem_flags)
{
	atbm_urb_s *purb;

	iso_packets = iso_packets;/*No used*/
	mem_flags = mem_flags;    /*No used*/
	
	purb = (atbm_urb_s*) atbm_kzalloc(sizeof(atbm_urb_s),GFP_KERNEL);
	if (purb == ATBM_NULL) {
		wifi_printk(WIFI_ALWAYS ,"atbm_usb_alloc_urb fail \n");
		return ATBM_NULL;
	}

	return purb;	
}

atbm_void atbm_usb_free_urb(atbm_urb_s *purb)
{
	if(purb == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS ,"atbm_usb_free_urb fail \n");
		return;
	}

	atbm_kfree(purb);
	
	return;
}

extern int atbm_wlan_write_data_async(atbm_urb_s *purb);
extern int atbm_wlan_usb_submit_rx_urb(atbm_urb_s *ctx);
atbm_int32 atbm_usb_submit_urb(atbm_urb_s *purb, int param)
{
	atbm_int32 ret = 0;
#if ATBM_URB_TASK
	atbm_uint32 status;
#endif

	param = param; /*No used*/
	
	wifi_printk(WIFI_ALWAYS, "%s pipe %d, len %d, pbuf 0x%x\n", __func__, purb->pipe, purb->transfer_buffer_length, purb->transfer_buffer);
	
	switch(purb->pipe){

		case ATBM_DRV_DATA_IN_PIPE:

#if ATBM_URB_TASK
			status = tx_queue_send(&atbm_urb_rx_msgQ, (atbm_void *)purb, TX_NO_WAIT);
			if (TX_SUCCESS != status){
				wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_msgQ send failed 0x%x\n", status);
				ret = -1;
			}
#else
			atbm_wlan_usb_submit_rx_urb(purb);
#endif
			break;
		case ATBM_DRV_DATA_OUT_PIPE:
#if ATBM_URB_TASK
			status = tx_queue_send(&atbm_urb_tx_msgQ, (atbm_void *)purb, TX_NO_WAIT);
			if (TX_SUCCESS != status){
				wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_msgQ send failed 0x%x\n", status);
				ret = -1;
			}
#else
			atbm_wlan_write_data_async(purb);
#endif
			break;
		default:
			wifi_printk(WIFI_ALWAYS, "atbm_usb_submit_urb pipe error %d\n", purb->pipe);
			ret = -1;
			break;
	}
	
	return ret;
}

//#define atbm_usb_unlink_urb(a)      usb_unlink_urb(a)

atbm_void atbm_usb_kill_urb(atbm_urb_s *purb)
{
	if(purb == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS ,"atbm_usb_kill_urb fail \n");
		return;
	}

	atbm_kfree(purb);
	
	return;
}

extern int atbm_wlan_usb_control_msg(unsigned char requestType, unsigned char req, unsigned short addr, unsigned short index, unsigned char *buffer, unsigned short len);
atbm_int32 atbm_usb_control_msg(struct atbm_usb_device *udev, unsigned int pipe, unsigned char request, unsigned char requestType,
                    unsigned short value, unsigned short index, unsigned char *reqdata, unsigned short len, unsigned short timeout)
{
	atbm_int32 ret;

	udev = udev;       /*No used*/
	pipe = pipe;       /*No used*/
	timeout = timeout; /*No used*/
	
	ret = atbm_wlan_usb_control_msg(requestType, request, value, index, reqdata, len);
	return ret;
}

atbm_void atbm_usb_fill_bulk_urb(struct urb *purb, struct atbm_usb_device *udev,unsigned int pipe,
	atbm_void *txdata, atbm_int32 tx_len, usb_complete_t complete_fn, struct sbus_urb *tx_urb)
{
	udev = udev; /*No used*/
	
	purb->pipe = pipe;
	purb->transfer_buffer = txdata;
	purb->transfer_buffer_length = tx_len;
	purb->actual_length = 0;
	purb->complete = complete_fn;
	purb->context = (atbm_void *)tx_urb;
	
	return ;
}

atbm_uint32 atbm_usb_rcvbulkpipe(struct atbm_usb_device *udev, atbm_int32 pipe)
{
	udev = udev; /*No used*/
	pipe = pipe; /*No used*/
	return (atbm_uint32)ATBM_DRV_DATA_IN_PIPE;
}

atbm_uint32 atbm_usb_sndbulkpipe(struct atbm_usb_device *udev, atbm_int32 pipe)
{
	udev = udev; /*No used*/
	pipe = pipe; /*No used*/
	return (atbm_uint32)ATBM_DRV_DATA_OUT_PIPE;
}

atbm_uint32 atbm_usb_sndctrlpipe(struct atbm_usb_device *udev, atbm_uint32 pipe)
{
	udev = udev; /*No used*/
	pipe = pipe; /*No used*/
	return (atbm_uint32)ATBM_DRV_CONTROL_PIPE;
}
atbm_uint32 atbm_usb_rcvctrlpipe(struct atbm_usb_device *udev, atbm_uint32 pipe)
{
	udev = udev; /*No used*/
	pipe = pipe; /*No used*/
	return (atbm_uint32)ATBM_DRV_CONTROL_PIPE;
}

//#define atbm_usb_device_id      usb_device_id
struct atbm_usb_device *atbm_usb_get_dev(struct atbm_usb_device *udev)
{
	udev = udev; /*No used*/
	return ATBM_NULL;
}
struct atbm_usb_interface *atbm_usb_get_intf(struct atbm_usb_interface *intf)
{
	intf = intf; /*No used*/
	return ATBM_NULL;
}
struct atbm_usb_device *atbm_interface_to_usbdev(struct atbm_usb_interface *intf)
{
	intf = intf; /*No used*/
	return ATBM_NULL;
}

atbm_void atbm_usb_set_intfdata(struct atbm_usb_interface *usb_intf, struct dvobj_priv *pdvobjpriv)
{
	usb_intf->pdvobjpriv = pdvobjpriv;
	return;
}

struct dvobj_priv *atbm_usb_get_intfdata(struct atbm_usb_interface *intf)
{
	return intf->pdvobjpriv;
}
//#define atbm_usb_endpoint_is_bulk_in(a) usb_endpoint_is_bulk_in(a)
//#define atbm_usb_endpoint_num(a) usb_endpoint_num(a)
//#define atbm_usb_endpoint_is_bulk_out(a) usb_endpoint_is_bulk_out(a)
atbm_void atbm_usb_put_dev(struct atbm_usb_device *udev)
{
	udev = udev; /*No used*/
	return;
}
//#define atbm_usb_reset_device(a) usb_reset_device(a)

#if ATBM_URB_TASK
extern int atbm_wlan_write_data_sync(char *pmbuf, unsigned int len, unsigned int *actual_len, unsigned char pipe, unsigned int timeout);
extern int atbm_wlan_read_data_sync(char *pmbuf, unsigned int len, unsigned int *actual_len, unsigned char pipe, unsigned int timeout);
atbm_int32 atbm_urb_tx_queue_task(void *unused)
{
	atbm_int32 ret;
	atbm_uint32 status;
	atbm_urb_s urb_tx;
	atbm_urb_s *purb = &urb_tx;
	
	wifi_printk(WIFI_ALWAYS, "%s, running...\n", __func__);

	while(1){

		status = tx_queue_receive (&atbm_urb_tx_msgQ, (atbm_void *)&urb_tx, TX_WAIT_FOREVER);
		if(status != TX_SUCCESS){
			wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_msgQ recv failed 0x%x\n", status);
			tx_thread_sleep(50);
			continue;
		}
		
		wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_queue_task got msg......\n");
		wifi_printk(WIFI_ALWAYS, "%s pipe %d, len %d, pbuf 0x%x\n", __func__, purb->pipe, purb->transfer_buffer_length, purb->transfer_buffer);
		
		ret = atbm_wlan_write_data_sync(urb_tx.transfer_buffer, urb_tx.transfer_buffer_length, &urb_tx.actual_length, urb_tx.pipe, 100);
		if(ret < 0){
			wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_queue_task write failed \n");
			tx_thread_sleep(50);
			urb_tx.status = -1;
			(*urb_tx.complete)(&urb_tx);
			continue;
		}

		if(urb_tx.transfer_buffer_length != urb_tx.actual_length){
			wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_queue_task warning len %d %d\n", urb_tx.transfer_buffer_length, urb_tx.actual_length);
		}

		urb_tx.status = 0;
		(*urb_tx.complete)(&urb_tx);
	}

	return 0;
}


atbm_int32 atbm_urb_rx_queue_task(void *unused)
{
	atbm_int32 ret;
	atbm_uint32 status;
	atbm_urb_s urb_rx;
	//int i;
	//char *ptr;
	
	wifi_printk(WIFI_ALWAYS, "%s, running...\n", __func__);
	
	while(1){

		status = tx_queue_receive (&atbm_urb_rx_msgQ, (atbm_void *)&urb_rx, TX_WAIT_FOREVER);
		if(status != TX_SUCCESS){
			wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_msgQ recv failed 0x%x\n", status);
			tx_thread_sleep(50);
			continue;
		}

		wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_queue_task got msg......\n");
		wifi_printk(WIFI_ALWAYS, "atbm: rx pipe %d len %d reserver %d\n", urb_rx.pipe, urb_rx.transfer_buffer_length, urb_rx.reserver);

		ret = atbm_wlan_read_data_sync(urb_rx.transfer_buffer, urb_rx.transfer_buffer_length, &urb_rx.actual_length, urb_rx.pipe, 100);
		if(ret < 0){
			wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_queue_task read failed \n");
			tx_thread_sleep(50);
			urb_rx.status = -1;
			(*urb_rx.complete)(&urb_rx);
			continue;
		}
		
		wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_queue_task len %d, actual %d\n", urb_rx.transfer_buffer_length, urb_rx.actual_length);
		//ptr = (char *)urb_rx.transfer_buffer;
		//for(i=0; i<64; i+=8){
		//	wifi_printk(WIFI_ALWAYS, "[%d] %2x %2x %2x %2x %2x %2x %2x %2x\n", i, ptr[i], ptr[i+1], ptr[i+2],ptr[i+3], ptr[i+4], ptr[i+5], ptr[i+6], ptr[i+7]);
		//}

		if(urb_rx.transfer_buffer_length != urb_rx.actual_length){
			wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_queue_task warning len %d %d\n", urb_rx.transfer_buffer_length, urb_rx.actual_length);
		}
			
		urb_rx.status = 0;
		(*urb_rx.complete)(&urb_rx);
	}
	
	return 0;
}
#endif

atbm_void atbm_urb_queue_init(atbm_void)
{
#if ATBM_URB_TASK
	atbm_uint32 status;
	atbm_uint32 msgNum, msgLen, totalLen;

	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_init ==>\n");
	
	if(atbm_urb_msgQ_init_state == 1){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_init error!!!\n");
		return;
	}

    // Calculate Message Number
	msgLen = sizeof(atbm_urb_s);
    msgNum = (msgLen % sizeof(unsigned long) == 0)? msgLen/sizeof(unsigned long) : (msgLen/sizeof(unsigned long))+1;

	if(msgNum > TX_16_ULONG){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_init error msgNum %d\n", msgNum);
		return;
	}
	
    if (msgNum <= 1)
        msgNum = TX_1_ULONG;
    else if (msgNum <= 2)
        msgNum = TX_2_ULONG;
    else if (msgNum <= 4)
        msgNum = TX_4_ULONG;
    else if (msgNum <= 8)
        msgNum = TX_8_ULONG;
    else
        msgNum = TX_16_ULONG;

	
	// Init TX MessageQ
	totalLen = ATBM_URB_TX_MSGQ_NUM*msgNum*sizeof(unsigned long);
	status = tx_queue_create(&atbm_urb_tx_msgQ, "wifi_queue_tx", msgNum, (void *)&atbm_urb_tx_msqQbuf[0], totalLen);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_msgQ create failed 0x%x\n", status);
		return;
	}

	// Init RX MessageQ
	totalLen = ATBM_URB_RX_MSGQ_NUM*msgNum*sizeof(unsigned long);
	status = tx_queue_create(&atbm_urb_rx_msgQ, "wifi_queue_rx", msgNum, (void *)&atbm_urb_rx_msqQbuf[0], totalLen);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_msgQ create failed 0x%x\n", status);
		return;
	}


	//Init TX Task
	atbm_memset(atbm_urb_tx_thread_stack, 0, ATBM_URB_TX_THREAD_STACKSIZE);
	status = tx_thread_create(&atbm_urb_tx_thread,
					  "wifi_tx_task",
					  (void(*)(unsigned long))atbm_urb_tx_queue_task,
					  (unsigned long)0,
					  (atbm_void *)&atbm_urb_tx_thread_stack[0],
					  ATBM_URB_TX_THREAD_STACKSIZE, 
					  ATBM_URB_TX_THREAD_PRIORITY, 
					  ATBM_URB_TX_THREAD_PRIORITY-1,
					  TX_NO_TIME_SLICE, 
					  TX_AUTO_START);

	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_thread create failed 0x%x\n", status);
		return;
	}

	//Init RX Task
	atbm_memset(atbm_urb_rx_thread_stack, 0, ATBM_URB_RX_THREAD_STACKSIZE);
	status = tx_thread_create(&atbm_urb_rx_thread,
					  "wifi_rx_task",
					  (void(*)(unsigned long))atbm_urb_rx_queue_task,
					  (unsigned long)0,
					  (atbm_void *)&atbm_urb_rx_thread_stack[0],
					  ATBM_URB_RX_THREAD_STACKSIZE, 
					  ATBM_URB_RX_THREAD_PRIORITY, 
					  ATBM_URB_RX_THREAD_PRIORITY-1,
					  TX_NO_TIME_SLICE, 
					  TX_AUTO_START);

	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_thread create failed 0x%x\n", status);
		return;
	}

	//Urb Init Ready
	atbm_urb_msgQ_init_state = 1;
	
	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_init <==\n");
#endif
	return ;
}
atbm_void atbm_urb_queue_exit(atbm_void)
{
#if ATBM_URB_TASK
	atbm_uint32 status;
	
	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_exit ==>\n");

	if(atbm_urb_msgQ_init_state == 0){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_exit error!!!\n");
		return;
	}

	//Delete TX MessageQ
	status = tx_queue_delete(&atbm_urb_tx_msgQ);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_msgQ delete failed 0x%x\n", status);
	}

	//Delete RX MessageQ
	status = tx_queue_delete(&atbm_urb_rx_msgQ);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_msgQ delete failed 0x%x\n", status);
	}

	//Delete TX Task
	status = tx_thread_delete(&atbm_urb_tx_thread);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_tx_thread delete failed 0x%x\n", status);
	}

	//Delete RX Task
	status = tx_thread_delete(&atbm_urb_rx_thread);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_urb_rx_thread delete failed 0x%x\n", status);
	}

	//Urb Init Flag Clear
	atbm_urb_msgQ_init_state = 0;
	
	wifi_printk(WIFI_ALWAYS, "atbm_urb_queue_exit <==\n");
#endif
	return;
}


