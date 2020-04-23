/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description:
 */

#ifndef __HI_DRV_GPIO_I2C_H__
#define __HI_DRV_GPIO_I2C_H__

#include <linux/kernel.h>
#include "hi_type.h"

hi_s32  hi_drv_gpio_i2c_init(hi_void);
hi_void hi_drv_gpio_i2c_deinit(hi_void);

hi_slong hi_drv_gpio_i2c_ioctl(hi_void *file, hi_u32 cmd, hi_size_t arg);

hi_slong hi_drv_gpio_i2c_compat_ioctl(hi_void *file, hi_u32 cmd, hi_size_t arg);

hi_s32 hi_drv_gpio_i2c_sccb_read(hi_u32 i2c_num, hi_u8 dev_addr, hi_u8 reg_addr, hi_u8 *buf);

hi_s32 hi_drv_gpio_i2c_read(hi_u32 i2c_num, hi_u8 dev_addr, hi_u8 reg_addr, hi_u8 *buf);
hi_s32 hi_drv_gpio_i2c_write(hi_u32 i2c_num, hi_u8 dev_addr, hi_u8 reg_addr, hi_u8 data);
hi_s32 hi_drv_gpio_i2c_create_gpio_i2c(hi_u32 *i2c_num, hi_u32 scl_gpio_num, hi_u32 sda_gpio_num);
hi_s32 hi_drv_gpio_i2c_destroy_gpio_i2c(hi_u32 i2c_num);

hi_s32 hi_drv_gpio_i2c_read_ext(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                              hi_u32 buf_len);
hi_s32 hi_drv_gpio_i2c_read_si_labs(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                              hi_u32 buf_len);
hi_s32 hi_drv_gpio_i2c_read_ext_directly(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset,
                                      hi_u8 *buf, hi_u32 buf_len);
hi_s32 hi_drv_gpio_i2c_write_ext(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                               hi_u32 buf_len);
hi_s32 hi_drv_gpio_write_ext_nostop(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset,
                                     hi_u8 *buf, hi_u32 buf_len);

#endif
