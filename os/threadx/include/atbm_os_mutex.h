#ifndef ATBM_OS_MUTEX_H
#define ATBM_OS_MUTEX_H

#include "tx_api.h"
#include "atbm_os_atomic.h"


#define OS_WAIT_FOREVER 0xffffffff
typedef  TX_MUTEX atbm_mutex;
typedef  TX_EVENT_FLAGS_GROUP  atbm_os_wait_queue_head_t;

/*mutex*/
atbm_int8 atbm_os_mutexLockInit(atbm_mutex *pmutex);
atbm_int8 atbm_os_DeleteMutex(atbm_mutex *pmutex);
atbm_int8 atbm_os_mutexLock(atbm_mutex *pmutex, atbm_uint32 val);
atbm_int8 atbm_os_mutexUnLock(atbm_mutex *pmutex);



/*event*/
atbm_int8 atbm_os_init_waitevent(atbm_os_wait_queue_head_t *pevent);
atbm_int8 atbm_os_delete_waitevent(atbm_os_wait_queue_head_t *pevent);
atbm_int8 atbm_os_wait_event(atbm_os_wait_queue_head_t *pevent);
atbm_int8 atbm_os_wait_event_timeout(atbm_os_wait_queue_head_t *pevent,atbm_uint32 val/*tick ?*/);
atbm_int8 atbm_os_wakeup_event(atbm_os_wait_queue_head_t *pevent);

#define atbm_os_mutextryLock(x)	(0) //SDIO module
#endif /* ATBM_OS_MUTEX_H */

