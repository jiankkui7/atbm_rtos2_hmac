
/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include <stdio.h>
#include "atbm_hal.h"

atbm_int8 atbm_os_MsgQ_Create(atbm_os_msgq *pmsgQ, atbm_uint32 *pstack, atbm_uint32 item_size, atbm_uint32 item_num)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;
	//atbm_uint32 msgNum, msgLen, totalLen;

	//msgLen = length;
	//msgNum = (msgLen % sizeof(unsigned long) == 0)? msgLen/sizeof(unsigned long) : (msgLen/sizeof(unsigned long))+1;

//	if(msgNum > TX_16_ULONG){
	//	wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Create error msgNum %d\n", msgNum);
		//ret = WIFI_ERROR;
		//goto error;
//	}
	
	//totalLen = msgNum*sizeof(unsigned long);
	status = tx_queue_create(pmsgQ, "wifi_msgq", TX_1_ULONG, (void *)pstack, item_size * item_num);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Create create failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Create create null \n");
		ret = WIFI_ERROR;
		goto error;
	}
	
error:
	return ret;
}

atbm_int8 atbm_os_MsgQ_Delete(atbm_os_msgq *pmsgQ)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Delete error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	
	//Delete MessageQ
	status = tx_queue_delete(pmsgQ);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Delete delete failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

atbm_int8 atbm_os_MsgQ_Recv(atbm_os_msgq *pmsgQ, atbm_void *pbuf, int len, atbm_uint32 val)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;
	atbm_uint32 data;

	val = val;//No used

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Get error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	//Receive MessageQ
	status = tx_queue_receive (pmsgQ, &data, val);
	//wifi_printk(WIFI_DBG_MSG,"%s %d 0x%x\n",__func__,__LINE__,data);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Get recv failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}	

	atbm_memcpy(pbuf, &data, 4);
error:
	return ret;

}

atbm_int8 atbm_os_MsgQ_Send(atbm_os_msgq *pmsgQ, atbm_void *pbuf, int len, atbm_uint32 val)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	val = val; //No used
	
	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Put error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	//wifi_printk(WIFI_DBG_MSG,"%s %d 0x%x\n",__func__,__LINE__,pbuf);
	//Send MessageQ, and no wait
	status = tx_queue_send(pmsgQ, pbuf, TX_NO_WAIT);
	if (TX_SUCCESS != status){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Put send failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

////

