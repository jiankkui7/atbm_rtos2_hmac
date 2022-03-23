
/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include "atbm_hal.h"
#include "atbm_os_msgQ.h"

atbm_int8 atbm_os_MsgQ_Create(atbm_os_msgq *pmsgQ, atbm_uint32 *pstack, atbm_uint32 item_size, atbm_uint32 item_num)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;
	*pmsgQ = AK_Create_Queue((atbm_void*)pstack, item_size * item_num, AK_FIXED_SIZE, item_size, AK_FIFO);
	if(AK_IS_INVALIDHANDLE(*pmsgQ))
	{
		wifi_printk(WIFI_ALWAYS,"create queue err:%d", pmsgQ);
		return -1;
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
	status = AK_Delete_Queue(*pmsgQ);
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
	unsigned long actual_size = 0;

	val = val;//No used

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Get error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	//Receive MessageQ
	status = AK_Receive_From_Queue(*pmsgQ, pbuf, len, &actual_size, val);
	if(status != TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Get recv failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}	
error:
	return ret;

}

atbm_int8 atbm_os_MsgQ_Send(atbm_os_msgq *pmsgQ, atbm_void *pbuf, int len, atbm_uint32 val)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	
	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Put error\n");
		ret = WIFI_ERROR;
		goto error;
	}

	//Send MessageQ, and no wait
	status = AK_Send_To_Queue(*pmsgQ, pbuf, len, val);
	if (TX_SUCCESS != status){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Put send failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}
