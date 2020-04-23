/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description:
 */

#ifndef _GPIO_I2C_EXT_H
#define _GPIO_I2C_EXT_H

#include "hi_type.h"
#include <linux/kernel.h>
#include <linux/fs.h>

hi_s32 gpio_i2c_drv_module_init(hi_void);
hi_void gpio_i2c_drv_module_exit(hi_void);

typedef hi_slong (*fn_gpio_i2c_ioctl)(hi_void *, hi_u32, hi_size_t);
typedef hi_s32 (*fn_gpio_i2c_write)(hi_u32, hi_u8, hi_u8, hi_u8);
typedef hi_s32 (*fn_gpio_i2c_write_ext)(hi_u32, hi_u8, hi_u32, hi_u32, hi_u8 *, hi_u32);
typedef hi_s32 (*fn_gpio_i2c_read)(hi_u32, hi_u8, hi_u8, hi_u8 *);
typedef hi_s32 (*fn_gpio_i2c_read_ext)(hi_u32, hi_u8, hi_u32, hi_u32, hi_u8 *, hi_u32);
typedef hi_s32 (*fn_gpio_i2c_sccb_read)(hi_u32, hi_u8, hi_u8, hi_u8 *);
typedef hi_s32 (*fn_gpio_i2c_create_channel)(hi_u32 *, hi_u32, hi_u32);
typedef hi_s32 (*fn_gpio_i2c_destroy_channel)(hi_u32);
typedef hi_s32 (*fn_gpio_i2c_is_used)(hi_u32, hi_bool *);

typedef struct {
    fn_gpio_i2c_ioctl           pfn_gpio_i2c_ioctl;
    fn_gpio_i2c_ioctl           pfn_gpio_i2c_compat_ioctl;
    fn_gpio_i2c_write           pfn_gpio_i2c_write;
    fn_gpio_i2c_write_ext       pfn_gpio_i2c_write_ext;
    fn_gpio_i2c_write_ext       pfn_gpio_i2c_write_ext_nostop;
    fn_gpio_i2c_read            pfn_gpio_i2c_read;
    fn_gpio_i2c_read_ext        pfn_gpio_i2c_read_ext;
    fn_gpio_i2c_read_ext        pfn_gpio_i2c_read_ext_directly;
    fn_gpio_i2c_sccb_read       pfn_gpio_i2c_sccb_read;
    fn_gpio_i2c_create_channel  pfn_gpio_i2c_create_channel;
    fn_gpio_i2c_destroy_channel pfn_gpio_i2c_destroy_channel;
    fn_gpio_i2c_is_used         pfn_gpio_i2c_is_used;
} gpio_i2c_ext_func;

#endif
