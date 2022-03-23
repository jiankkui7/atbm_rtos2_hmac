/**
 * @file gpio_config.h
 * @brief gpio function header file
 * Copyright (C) 2010 Anyka (Guangzhou) Microelectronics Technology Co., Ltd
 * @AUTHOR
 * @date 2010-12-10
 * @VERSION 1.0
 * @REF
 * @NOTE:
 * 1. ����mmiϵͳ���Ѷ����˵�gpio������Ҫɾ����ش��룬ֻ�轫�䶨��ΪINVALID_GPIO
 * 2. �����Ҫ�õ���չio��ֻ��Ҫ��GPIO_MULTIPLE_USE�꣬�����ö�Ӧ��gpio
 *    GPIO_EXPAND_OUT1��GPIO_EXPAND_OUT2�����ֻ��һ����չio,���Խ�GPIO_EXPAND_OUT2
 *	  ��ΪINVALID_GPIO����
 */
#ifndef __GPIO_CONFIG_H__
#define __GPIO_CONFIG_H__


#include "drv_gpio.h"

#define GPIO_CAMERA_RESET           39 //49: V1.1 chip borad
#define GPIO_CAMERA_AVDD            40
#define GPIO_I2C_SCL                27
#define GPIO_I2C_SDA                28 

#define GPIO_WIFI_POWERDOWN		    48

#define GPIO_IRCUT_CTRL					5		//ircut��������
#define GPIO_IRCUT_DECTECT				0		//����������

#endif //#ifndef __GPIO_CONFIG_H__

