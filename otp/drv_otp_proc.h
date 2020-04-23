/* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :Add proc node in virtual fs.
* Author : Linux SDK team
* Created : 2019-06-21
*/
#ifndef __DRV_OTP_PROC_H__
#define __DRV_OTP_PROC_H__

#include "hi_drv_proc.h"
#include "drv_otp_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****** proc virtualotp begin ******* */
hi_s32 fake_otp_virtual_read(hi_u32 addr, hi_u32 *value);
hi_s32 fake_otp_virtual_write_byte(hi_u32 addr, hi_u8 value);
hi_s32 otp_virtual_test(hi_u32 cmd, hi_void *arg, hi_void *private_data);
/* ****** proc virtualotp end ******* */
hi_s32 otp_register_proc(hi_void);
hi_void otp_remove_proc(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
