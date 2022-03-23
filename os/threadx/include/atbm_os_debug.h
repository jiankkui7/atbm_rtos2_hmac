/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#ifndef ATBM_OS_DEBUG_H
#define ATBM_OS_DEBUG_H

#include "dprintf.h"
#define iot_printf dbg_printf
#define  atbm_printk dbg_printf
#endif //ATBM_OS_DEBUG_H
