#ifndef ATBM_OS_SPINLOCK_H
#define ATBM_OS_SPINLOCK_H
#include "atbm_type.h"
#include "spin_lock.h"

typedef   spinlock_t atbm_spinlock_t;
/*spin lock*/
#define atbm_spin_lock_init(x)
#define atbm_spin_lock(x) SCI_DisableIRQ()
#define atbm_spin_unlock(x) SCI_RestoreIRQ()
#define atbm_spin_lock_irqsave(x,f) SCI_DisableIRQ()
#define atbm_spin_unlock_irqrestore(x,f) SCI_RestoreIRQ()
#define atbm_spin_lock_bh(x) SCI_DisableIRQ()
#define atbm_spin_unlock_bh(x) SCI_RestoreIRQ()


#endif /* ATBM_OS_SPINLOCK_H */

