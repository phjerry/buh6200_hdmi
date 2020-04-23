/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2007-2019. All rights reserved.
 * Description:
 */

#ifndef __DRV_I2C_IOCTL_H__
#define __DRV_I2C_IOCTL_H__

#include "hi_unf_i2c.h"
#include "hi_drv_i2c.h"

#define HI_GPIO_GROUP_BIT 8

typedef struct {
    hi_u32    i2c_num;
    hi_u8     i2c_dev_addr;
    hi_u32    i2c_reg_addr;
    hi_u32    i2c_reg_count;
    hi_u8     *buf;
    hi_u32    buf_len;
} i2c_data;

typedef struct {
    hi_u32    i2c_num;
    hi_u8     i2c_dev_addr;
    hi_u32    i2c_reg_addr;
    hi_u32    i2c_reg_count;
    hi_u32    buf_addr;
    hi_u32    buf_len;
} i2c_data_compat;

typedef struct {
    hi_u32 i2c_num;
    hi_u32 rate_value;
} i2c_rate;

typedef struct {
    hi_u32  i2c_num;
    hi_u32  scl_gpio_num;
    hi_u32  sda_gpio_num;
    hi_bool is_used;
    hi_u32  count;
} i2c_gpio;

/* Ioctl definitions */
#define CMD_I2C_READ _IOW(HI_ID_I2C, 0x1, hi_u32) /* 1:check keyup */
#define CMD_I2C_WRITE _IOW(HI_ID_I2C, 0x2, hi_u32) /* 1:check repkey, 0:hardware behave */
#define CMD_I2C_SET_RATE _IOW(HI_ID_I2C, 0x3, hi_u32)
#define CMD_I2C_CONFIG _IOWR(HI_ID_I2C, 0x4, i2c_gpio) /* config  gpioi2c */
#define CMD_I2C_DESTROY _IOW(HI_ID_I2C, 0x5, hi_u32) /* destroy gpioi2c */

#endif /* End of #ifndef __DRV_I2C_IOCTL_H__ */
