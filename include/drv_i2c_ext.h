/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description:
 */

#ifndef _DRV_I2C_EXT_H
#define _DRV_I2C_EXT_H

#include "hi_type.h"
#ifdef __KERNEL__
#include "hi_drv_dev.h"
#endif
hi_s32 i2c_drv_module_init(hi_void);
hi_void i2c_drv_module_exit(hi_void);

typedef hi_s32 (*fn_i2c_write_config)(hi_u32, hi_u8);
typedef hi_s32 (*fn_i2c_write)(hi_u32, hi_u8, hi_u32, hi_u32, hi_u8 *, hi_u32);
typedef hi_s32 (*fn_i2c_read)(hi_u32, hi_u8, hi_u32, hi_u32, hi_u8 *, hi_u32);
typedef hi_s32 (*fn_i2c_set_rate)(hi_u32 , hi_u32);

typedef struct {
    fn_i2c_write_config pfn_i2c_write_config;
    fn_i2c_write        pfn_i2c_write;
    fn_i2c_read         pfn_i2c_read;
    fn_i2c_write        pfn_i2c_write_nostop;
    fn_i2c_read         pfn_i2c_read_directly;
    fn_i2c_set_rate     pfn_i2c_set_rate;
} i2c_ext_func;

#endif
