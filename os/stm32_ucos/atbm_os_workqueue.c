#include "atbm_os_workqueue.h"
atbm_uint32 work_queue_table_mux;
atbm_uint8 atbm_os_GetMessage(atbm_uint32 ulMQId, atbm_void **msg, atbm_uint32 ulTimeout)
{
    return 0;//MMPF_OS_GetMessage(ulMQId, msg, ulTimeout);
}
atbm_uint8 atbm_os_PutMessage(atbm_uint32 ulMQId, atbm_void *msg)
{
    return 0;//MMPF_OS_PutMessage(ulMQId, msg);
}
atbm_uint8 atbm_os_DeleteMQueue(atbm_uint32 ulMQId)
{
    return 0;//MMPF_OS_DeleteMQueue(ulMQId);
}
