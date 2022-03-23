#ifndef ATBM_OS_MUTEX_H
#define ATBM_OS_MUTEX_H
#include "atbm_type.h"
#include "rtdef.h"


#define OS_WAIT_FOREVER    0xffffffff // block forever until get the source
#define OS_NO_WAIT         0  //no block
//mutex
#define OS_MUTEX_INHERIT     1
#define OS_MUTEX_NO_INHERIT  0

typedef   rt_sem_t atbm_mutex  ;
typedef   rt_event_t atbm_os_wait_queue_head_t  ;

/*sem*/
//atbm_uint32  atbm_os_CreateSem(atbm_uint8 ubSemValue);
//atbm_uint8 atbm_os_DeleteSem(atbm_uint32  ulSemId);
//atbm_uint8 atbm_os_QuerySem(atbm_uint32  ulSemId, atbm_uint16 *usCount);
atbm_uint8 atbm_os_wait_event_timeout(atbm_os_wait_queue_head_t * pulSemId, atbm_uint32 ulTimeout);
atbm_uint8 atbm_os_wakeup_event(atbm_os_wait_queue_head_t * pulSemId);
atbm_int32 atbm_os_init_waitevent(atbm_os_wait_queue_head_t* pulSemId);
atbm_uint8 atbm_os_DeleteSem(atbm_os_wait_queue_head_t* pulSem);
#define atbm_os_delete_waitevent(x) atbm_os_DeleteSem((atbm_os_wait_queue_head_t*)x)

/*mutex*/
atbm_uint32 atbm_os_mutexLockInit(atbm_mutex * pmutex_id);
atbm_uint8 atbm_os_DeleteMutex(atbm_mutex * pmutex_id);
atbm_uint8 atbm_os_mutexLock(atbm_mutex * pmutex_id,atbm_uint32 ulTimeout);
atbm_uint8 atbm_os_mutexUnLock(atbm_mutex * pmutex_id);
#endif /* ATBM_OS_MUTEX_H */

