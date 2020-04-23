/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: gpio header file
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "hi_type.h"

/* 1 register addr */
#define  GPIO23_BASE_ADDR   0x00860000
#define  GPIO24_BASE_ADDR   0x00861000
#define  GPIO25_BASE_ADDR   0x00862000
#define  GPIO26_BASE_ADDR   0x00863000
#define  GPIO27_BASE_ADDR   0x00864000

#define  AON_GPIO0    23
#define  AON_GPIO1    24
#define  AON_GPIO2    25
#define  AON_GPIO3    26
#define  AON_GPIO4    27

#define  HI_GPIO_BIT_NUM  8
#define  GPIO_REG_DIR     0x400
#define  GPIO_REG_IS      0x404
#define  GPIO_REG_IBE     0x408
#define  GPIO_REG_IEV     0x40c
#define  GPIO_REG_IE      0x410
#define  GPIO_REG_RIS     0x414
#define  GPIO_REG_MIS     0x418
#define  GPIO_REG_IC      0x41c
#define  GPIO_REG_RSV     0x420

typedef enum {
    HI_C51_GPIO_INTTYPE_UP,
    HI_C51_GPIO_INTTYPE_DOWN,
    HI_C51_GPIO_INTTYPE_UPDOWN,
    HI_C51_GPIO_INTTYPE_HIGH,
    HI_C51_GPIO_INTTYPE_LOW,
    HI_C51_GPIO_INTTYPE_MAX,
} gpio_interrupt_type;

__xdata_reentrant hi_void gpio_dirset_bit(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8 value);
__xdata_reentrant hi_void gpio_write_bit(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8 value);
hi_void gpio_read_bit(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8* value);
hi_void gpio_set_interrupt_type(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_u8 gpio_type);
hi_void gpio_interrupt_enable(hi_u8 gpio_group_num, hi_u8 gpio_bit_num, hi_bool enable);
hi_void gpio_isr(hi_u8 gpio_group_num, hi_u8 gpio_bit_num);

#endif
