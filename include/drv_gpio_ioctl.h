/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:
*/

#ifndef __DRV_GPIO_IOCTL_H__
#define __DRV_GPIO_IOCTL_H__

#include "hi_drv_gpio.h"

typedef struct {
    hi_u32 gpio_num;
    hi_gpio_outputtype output_type;
}gpio_output_type;

typedef struct {
    hi_u32 gpio_num;
    hi_bool input;
}gpio_direction;

typedef struct {
    hi_u32 gpio_num;
    hi_u32 set;
}gpio_value;

/* Ioctl definitions */
#define CMD_GPIO_SET_INT_TYPE _IOW(HI_ID_GPIO, 0x1, gpio_data) /* set interruput condition */
#define CMD_GPIO_SET_INT_ENABLE _IOW(HI_ID_GPIO, 0x2, gpio_data) /* set interruput enable or disable */
#define CMD_GPIO_GET_INT _IOR(HI_ID_GPIO, 0x3, gpio_interrupt) /* get interruput occur */
#define CMD_GPIO_GET_GPIONUM _IOR(HI_ID_GPIO, 0x4, gpio_get_gpio_num) /* get gpio max number and group number */
#define CMD_GPIO_SET_OUTPUTTYPE _IOW(HI_ID_GPIO, 0x5, gpio_output_type)
#define CMD_GPIO_GET_OUTPUTTYPE _IOWR(HI_ID_GPIO, 0x6, gpio_output_type)
#define CMD_GPIO_GET_DIRECTION _IOWR(HI_ID_GPIO, 0x7, gpio_direction)
#define CMD_GPIO_SET_DIRECTION _IOW(HI_ID_GPIO, 0x8, gpio_direction)
#define CMD_GPIO_WRITE_BIT _IOW(HI_ID_GPIO, 0x9, gpio_value)
#define CMD_GPIO_READ_BIT _IOWR(HI_ID_GPIO, 0xa, gpio_value)

#endif /* End of #ifndef __HI_DRV_GPIO_H__ */