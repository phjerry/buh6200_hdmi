/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:OTP ioctl defination.
 * Author: Linux SDK team
 * Create: 2019/06/21
 */
#ifndef __DRV_OTP_IOCTL_H__
#define __DRV_OTP_IOCTL_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WORD_SIZE         4
#define BYTE_WIDTH        8
#define SHORT_WIDTH       16
#define WORD_WIDTH        32

#define OTP_START_BIT(value)  ((((value) & 0xff0000) >> SHORT_WIDTH) & 0xff)
#define OTP_BIT_WIDTH(value)  ((((value) & 0xff00) >> BYTE_WIDTH) & 0xff)
#define OTP_VALUE_BYTE(value) ((value) & 0xff)
#define OTP_GET_PARA(start_bit, bit_width, value) \
    ((((start_bit) << SHORT_WIDTH) & 0xff0000) | (((bit_width) << BYTE_WIDTH) & 0xff00) | (((hi_u8)(value)) & 0xff))

typedef struct {
    hi_u32 addr;
    hi_u32 value;
} otp_entry;

/* ****** proc function begin ******* */
typedef struct {
    hi_u8 reserved[0x10];
} otp_test_data;
#define CMD_OTP_TEST                    _IOWR(HI_ID_OTP,  0xff, otp_test_data)
/* ****** proc function end   ******* */
#define CMD_OTP_READ                    _IOWR(HI_ID_OTP,  0x1, otp_entry)
#define CMD_OTP_READ_BYTE               _IOWR(HI_ID_OTP,  0x2, otp_entry)
#define CMD_OTP_READ_BITS_ONEBYTE       _IOWR(HI_ID_OTP,  0x3, otp_entry)

#define CMD_OTP_WRITE                   _IOW (HI_ID_OTP,  0x4, otp_entry)
#define CMD_OTP_WRITE_BYTE              _IOW (HI_ID_OTP,  0x5, otp_entry)
#define CMD_OTP_WRITE_BIT               _IOW (HI_ID_OTP,  0x6, otp_entry)
#define CMD_OTP_WRITE_BITS_ONEBYTE      _IOW (HI_ID_OTP,  0x7, otp_entry)

#define CMD_OTP_RESET                   _IO  (HI_ID_OTP,  0x8)

#define CMD_OTP_MAX                     0xffffffff

#ifdef __cplusplus
}
#endif
#endif /* __DRV_OTP_IOCTL_H__ */
