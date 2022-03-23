#include "atbm_os_spinlock.h"
#include "os_api.h"

void spin_lock_init(atbm_spinlock_t *spinlock)
{
	return;
}

void spin_lock(atbm_spinlock_t *spinlock)
{
	OS_ERR err;

	OSSchedLock(&err);
	if (err){
		wifi_printk(WIFI_DBG_MSG,"OSSchedLock err\n");
	}
}

void spin_unlock(atbm_spinlock_t *spinlock)
{
	OS_ERR err;
	
	OSSchedUnlock(&err);
	if (err){
		wifi_printk(WIFI_DBG_MSG,"OSSchedUnlock err\n");
	}
}

void spin_lock_irqsave(atbm_spinlock_t *spinlock, unsigned long flag)
{
	OS_ERR err;
	CPU_SR cpu_sr = (CPU_SR)flag;

	OS_CRITICAL_ENTER();
	OSSchedLock(&err);
	if (err){
		wifi_printk(WIFI_DBG_MSG,"OSSchedLock err\n");
	}
}

void spin_unlock_irqrestore(atbm_spinlock_t *spinlock, unsigned long flag)
{
	OS_ERR err;
	CPU_SR cpu_sr = (CPU_SR)flag;

	OSSchedUnlock(&err);
	if (err){
		wifi_printk(WIFI_DBG_MSG,"OSSchedUnlock err\n");
	}
	OS_CRITICAL_EXIT();
}

