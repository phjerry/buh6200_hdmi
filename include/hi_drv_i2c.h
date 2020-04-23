/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2007-2019. All rights reserved.
 * Description:
 */


#ifndef __HI_DRV_I2C_H__
#define __HI_DRV_I2C_H__

#include "hi_type.h"
#include "hi_debug.h"

#define HI_FATAL_I2C(fmt...)    HI_FATAL_PRINT(HI_ID_I2C, fmt)
#define HI_ERR_I2C(fmt...)      HI_ERR_PRINT(HI_ID_I2C, fmt)
#define HI_WARN_I2C(fmt...)     HI_WARN_PRINT(HI_ID_I2C, fmt)
#define HI_INFO_I2C(fmt...)     HI_INFO_PRINT(HI_ID_I2C, fmt)

#define HI_STD_I2C_NUM 6
#define HI_I2C_MAX_NUM_USER 15
#define HI_I2C_MAX_NUM_RESERVE 1
#define HI_I2C_MAX_NUM (HI_I2C_MAX_NUM_USER + HI_I2C_MAX_NUM_RESERVE)

#define HI_I2C_MAX_LENGTH  2048

hi_s32  hi_drv_i2c_init(hi_void);
hi_void hi_drv_i2c_deinit(hi_void);

hi_s32 hi_drv_i2c_write_config(hi_u32 channel, hi_u8 dev_addr);
hi_s32 hi_drv_i2c_write(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 * buf,
                        hi_u32 buf_len);
hi_s32 hi_drv_i2c_read(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 * buf,
                       hi_u32 buf_len);
hi_s32 hi_drv_i2c_read_si_labs(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                       hi_u32 buf_len);
hi_s32 hi_drv_i2c_write_sony(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len, hi_u8 mode);
hi_s32 hi_drv_i2c_read_sony(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len, hi_u8 mode);
hi_s32 hi_drv_i2c_read_directly(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset,
                               hi_u8 *buf, hi_u32 buf_len);
hi_s32 hi_drv_i2c_write_nostop(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset,
                               hi_u8 *buf, hi_u32 buf_len);
hi_s32 hi_drv_i2c_read_2stop(hi_u32 channel, hi_u8 dev_addr,
        hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len);

hi_s32 hi_drv_i2c_set_rate(hi_u32 channel, hi_u32 i2c_rate);

#endif
