/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:OTP drvier level head file.
 * Author: gaochen
 * Create: 2019/06/20
 */

#ifndef __HI_DRV_OTP_H__
#define __HI_DRV_OTP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define BYTES_PER_WORD   0x04
#define OTP_UCHAR_MASK   0xff

typedef hi_s32(*otp_write_byte_func)(hi_u32 addr, hi_u8 value);

typedef hi_s32(*otp_read_byte_func)(hi_u32 addr, hi_u8 *value);

typedef struct {
    otp_write_byte_func write_byte;
    otp_read_byte_func read_byte;
} otp_export_func;

hi_s32 hi_drv_otp_reset(hi_void);
hi_s32 hi_drv_otp_read(hi_u32 addr, hi_u32 *value);
hi_s32 hi_drv_otp_read_byte(hi_u32 addr, hi_u8 *value);
hi_s32 hi_drv_otp_read_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 *value);
hi_s32 hi_drv_otp_write(hi_u32 addr, hi_u32 value);
hi_s32 hi_drv_otp_write_byte(hi_u32 addr, hi_u8 value);
hi_s32 hi_drv_otp_write_bit(hi_u32 addr, hi_u32 bit_pos);
hi_s32 hi_drv_otp_write_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 value);

hi_s32 hi_drv_otp_mod_init(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* __HI_DRV_OTP_H__ */

