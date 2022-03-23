#ifndef ATBM_OS_SPINLOCK_H
#define ATBM_OS_SPINLOCK_H

#include "atbm_type.h"
#include "atbm_debug.h"
#include "os.h"

typedef unsigned int atbm_spinlock_t;


void spin_lock_init(atbm_spinlock_t *spinlock);
void spin_lock(atbm_spinlock_t *spinlock);
void spin_unlock(atbm_spinlock_t *spinlock);
void spin_lock_irqsave(atbm_spinlock_t *spinlock, unsigned long flag);
void spin_unlock_irqrestore(atbm_spinlock_t *spinlock, unsigned long flag);


#define atbm_spin_lock_init(x)           spin_lock_init(x)
#define atbm_spin_lock(x)                spin_lock(x)
#define atbm_spin_unlock(x)              spin_unlock(x)
#define atbm_spin_lock_irqsave(x,f)      spin_lock_irqsave((x),(*((unsigned long *)(f))))
#define atbm_spin_unlock_irqrestore(x,f) spin_unlock_irqrestore((x),(*((unsigned long *)(f))))
#define atbm_spin_lock_bh(x)             spin_lock(x)
#define atbm_spin_unlock_bh(x)           spin_unlock(x)

#endif /* ATBM_OS_SPINLOCK_H */

