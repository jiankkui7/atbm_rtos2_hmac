#ifndef ATBM_OS_SPINLOCK_H
#define ATBM_OS_SPINLOCK_H
#include "tx_api.h"

typedef TX_SEMAPHORE atbm_spinlock_t;

/*spin lock*/
atbm_int8 atbm_spin_lock_init(atbm_spinlock_t *spinlock);
atbm_int8 atbm_spin_lock_destroy(atbm_spinlock_t *spinlock);
atbm_int8 atbm_spin_lock(atbm_spinlock_t *spinlock);
atbm_int8 atbm_spin_unlock(atbm_spinlock_t *spinlock);
atbm_int8 atbm_spin_lock_irqsave(atbm_spinlock_t *spinlock, atbm_uint32 *flag);
atbm_int8 atbm_spin_unlock_irqrestore(atbm_spinlock_t *spinlock, atbm_uint32 flag);
atbm_int8 atbm_spin_lock_bh(atbm_spinlock_t *spinlock);
atbm_int8 atbm_spin_unlock_bh(atbm_spinlock_t *spinlock);

#endif /* ATBM_OS_SPINLOCK_H */

