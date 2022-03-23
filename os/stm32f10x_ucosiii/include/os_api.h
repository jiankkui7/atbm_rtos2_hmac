#ifndef OS_API_H
#define OS_API_H

#include <stdio.h>
#include "atbm_type.h"

#define inline
#define __INLINE        inline
#define iot_printf      printf
#define atbm_random()   rand()

typedef unsigned int size_t;

atbm_uint32 atbm_os_random(void);
//#define ZEROSIZE 0
#define ZEROSIZE 1

#endif //OS_API_H

