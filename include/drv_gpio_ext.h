/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description:
 */

#ifndef _DRV_GPIO_EXT_H
#define _DRV_GPIO_EXT_H

#include "hi_type.h"

#ifdef __KERNEL__
#include "hi_drv_dev.h"
#include <linux/cdev.h>
#endif
#include "hi_drv_gpio.h"

hi_s32 gpio_drv_module_init(hi_void);
hi_void gpio_drv_module_exit(hi_void);

typedef hi_s32 (*fn_gpio_get_bit)(hi_u32, hi_u32*);
typedef hi_s32 (*fn_gpio_set_bit)(hi_u32, hi_u32);
typedef hi_s32 (*fn_gpio_direction_get_bit)(hi_u32, hi_u32*);
typedef hi_s32 (*fn_gpio_direction_set_bit)(hi_u32, hi_u32);
typedef hi_s32 (*fn_gpio_get_num)(gpio_get_gpio_num*);

typedef hi_s32 (*fn_gpio_register_server_func)(hi_u32, hi_void (*func)(hi_u32));
typedef hi_s32 (*fn_gpio_unregister_server_func)(hi_u32);
typedef hi_s32 (*fn_gpio_set_interrupt_type)(hi_u32, hi_gpio_interrupt_type);
typedef hi_s32 (*fn_gpio_set_interrupt_enable)(hi_u32, hi_bool);
typedef hi_s32 (*fn_gpio_clear_group_interrupt)(hi_u32);
typedef hi_s32 (*fn_gpio_clear_bit_interrupt)(hi_u32);
#ifdef __KERNEL__
typedef hi_s32 (*fn_gpio_suspend)(hi_void *private_data);
typedef hi_s32 (*fn_gpio_resume)(hi_void *private_data);
#endif

typedef struct {
    fn_gpio_direction_get_bit            pfn_gpio_direction_get_bit;
    fn_gpio_direction_set_bit            pfn_gpio_direction_set_bit;
    fn_gpio_get_bit                      pfn_gpio_read_bit;
    fn_gpio_set_bit                      pfn_gpio_write_bit;
    fn_gpio_get_num                      pfn_gpio_get_num;
    fn_gpio_register_server_func         pfn_gpio_register_server_func;
    fn_gpio_unregister_server_func       pfn_gpio_unregister_server_func;
    fn_gpio_set_interrupt_type           pfn_gpio_set_interrupt_type;
    fn_gpio_set_interrupt_enable         pfn_gpio_set_interrupt_enable;
    fn_gpio_clear_group_interrupt        pfn_gpio_clear_group_interrupt;
    fn_gpio_clear_bit_interrupt          pfn_gpio_clear_bit_interrupt;
#ifdef __KERNEL__
    fn_gpio_suspend                      pfn_gpio_suspend;
    fn_gpio_resume                       pfn_gpio_resume;
#endif

} gpio_ext_func;

#endif
