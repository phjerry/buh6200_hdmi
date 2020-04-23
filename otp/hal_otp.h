/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:OTP driver in register level.
 * Author: Linux SDK team
 * Create: 2019/06/21
 */
#ifndef __HAL_OTP_H__
#define __HAL_OTP_H__

#include "drv_otp_define.h"
#include "hal_otp_reg.h"

hi_s32 hal_otp_init(hi_void);
hi_void hal_otp_deinit(hi_void);
hi_s32 hal_otp_reset(hi_void);
hi_s32 hal_otp_read(hi_u32 addr, hi_u32 *value);
hi_s32 hal_otp_read_byte(hi_u32 addr, hi_u8 *value);
hi_s32 hal_otp_read_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 *value);
hi_s32 hal_otp_write(hi_u32 addr, hi_u32 value);
hi_s32 hal_otp_write_byte(hi_u32 addr, hi_u8 value);
hi_s32 hal_otp_write_bit(hi_u32 addr, hi_u32 bit_pos);
hi_s32 hal_otp_write_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 value);

#endif /* __HAL_OTP_H__ */
