#ifndef ATBM_OS_MSGQ_H
#define ATBM_OS_MSGQ_H

#include "os.h"

struct os_msgq
{
	atbm_uint32 *pstack;
	atbm_uint32 item_size;
	atbm_uint32 item_num;
	atbm_uint32 item_cur;
	OS_Q sys_Q;
};

typedef struct os_msgq atbm_os_msgq;

atbm_int8 atbm_os_MsgQ_Create(atbm_os_msgq *pmsgQ, atbm_uint32 *pstack, atbm_uint32 item_size, atbm_uint32 item_num);
atbm_int8 atbm_os_MsgQ_Delete(atbm_os_msgq *pmsgQ);
atbm_int8 atbm_os_MsgQ_Recv(atbm_os_msgq *pmsgQ, atbm_void *pbuf, int len, atbm_uint32 val);
atbm_int8 atbm_os_MsgQ_Send(atbm_os_msgq *pmsgQ, atbm_void *pbuf, int len, atbm_uint32 val);

#endif /* ATBM_OS_MSGQ_H */

