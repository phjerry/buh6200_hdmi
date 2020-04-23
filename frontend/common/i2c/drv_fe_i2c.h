/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend i2c driver
 * Author: SDK
 * Created: 2017-06-30
 */

#ifndef __DRV_FE_I2C_H__
#define __DRV_FE_I2C_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 drv_fe_i2c_read(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                       hi_u32 buf_len);
hi_s32 drv_fe_i2c_write(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                        hi_u32 buf_len);
hi_s32 demod_i2c_read(hi_u32 tuner_port, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len);
hi_s32 demod_i2c_write(hi_u32 tuner_port, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len);
hi_s32 demod_read_byte(hi_u32 tuner_port, hi_u8 reg_addr, hi_u8 *reg_val);
hi_s32 demod_write_byte(hi_u32 tuner_port, hi_u8 reg_addr, hi_u8 reg_val);
hi_s32 demod_read_bit(hi_u32 tuner_port, hi_u8 reg_addr, hi_u8 bit_shift, hi_u8 *bit_val);
hi_s32 demod_write_bit(hi_u32 tuner_port, hi_u8 reg_addr, hi_u8 bit_shift, hi_u8 bit_val);
hi_s32 tuner_i2c_read(hi_u32 port_id, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len);
hi_s32 tuner_i2c_write(hi_u32 port_id, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len);
hi_s32 tuner_read_byte(hi_u32 tuner_port, hi_u8 reg_addr, hi_u8 *reg_val);
hi_s32 tuner_write_byte(hi_u32 tuner_port, hi_u8 reg_addr, hi_u8 reg_val);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif

