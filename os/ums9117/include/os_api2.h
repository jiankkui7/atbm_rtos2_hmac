#include <stdio.h>
#include "stddef.h"

#define __INLINE __inline

#define iot_printf(...) SCI_TraceLow("[Wifi_ATBM] " __VA_ARGS__)
#define atbm_random() rand()

atbm_uint32 atbm_os_random();
#define ZEROSIZE 0

