
/**************************************************************************************************************
 * altobeam LINUX wifi hmac source code 
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
	OS_ERR err;

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmsgQ is null\n");
		ret = WIFI_ERROR;
		goto error;
	}

	if((pstack == ATBM_NULL)
		|| (item_size == 0)
		|| (item_num == 0)){
		wifi_printk(WIFI_DBG_ERROR,"param err\n");
		ret = WIFI_ERROR;
		goto error;
	}

	OSQCreate(&(pmsgQ->sys_Q), NULL, item_num, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSQCreate err\n");
		ret = WIFI_ERROR;
		goto error;
	}

	pmsgQ->item_size = item_size;
	pmsgQ->item_num = item_num;
	pmsgQ->item_cur = 0;
	pmsgQ->pstack = pstack;

error:
	return ret;
}

atbm_int8 atbm_os_MsgQ_Delete(atbm_os_msgq *pmsgQ)
{
	atbm_int8 ret = WIFI_OK;
	OS_ERR err;

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmsgQ is null\n");
		ret = WIFI_ERROR;
		goto error;
	}

	OSQDel(&(pmsgQ->sys_Q), 0, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSQDel err\n");
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
}

atbm_int8 atbm_os_MsgQ_Recv(atbm_os_msgq *pmsgQ, atbm_void *pbuf, int len, atbm_uint32 val)
{
	atbm_int8 ret = WIFI_OK;
	void *msg;
	OS_MSG_SIZE msg_len;
	OS_ERR err;
	

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmsgQ is null\n");
		ret = WIFI_ERROR;
		goto error;
	}

	if((pbuf == ATBM_NULL)
		|| (len == 0)){
		wifi_printk(WIFI_DBG_ERROR,"param err\n");
		ret = WIFI_ERROR;
		goto error;
	}

	msg = OSQPend(&(pmsgQ->sys_Q), 0, OS_OPT_PEND_BLOCKING, &msg_len, NULL, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSQPend err: %d\n", err);
		ret = WIFI_ERROR;
		goto error;
	}

	atbm_memcpy(pbuf, msg, len);

error:
	return ret;
}

atbm_int8 atbm_os_MsgQ_Send(atbm_os_msgq *pmsgQ, atbm_void *pbuf, int len, atbm_uint32 val)
{
	atbm_int8 ret = WIFI_OK;
	OS_ERR err;
	atbm_uint32 *addr = NULL;

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"pmsgQ is null\n");
		ret = WIFI_ERROR;
		goto error;
	}

	if((pbuf == ATBM_NULL)
		|| (len == 0)){
		wifi_printk(WIFI_DBG_ERROR,"param err\n");
		ret = WIFI_ERROR;
		goto error;
	}

	addr = pmsgQ->pstack + pmsgQ->item_cur * pmsgQ->item_size;
	pmsgQ->item_cur++;
	if (pmsgQ->item_cur >= pmsgQ->item_num)
	{
		pmsgQ->item_cur = 0;
	}
	atbm_memcpy(addr, pbuf, len);

	OSQPost(&(pmsgQ->sys_Q), addr, len, OS_OPT_POST_FIFO, &err);
	if (err){
		wifi_printk(WIFI_DBG_ERROR,"OSQPost err: %d\n", err);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;
}

