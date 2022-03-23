#ifndef OS_API_H
#define OS_API_H

#include "atbm_type.h"
#include <linux/random.h>

#define __INLINE        inline
#define iot_printf      printk
#define atbm_random()   random32()

#define TargetUsb_lmac_start()

atbm_uint32 atbm_os_random(void);
#define ZEROSIZE 0

#endif //OS_API_H

