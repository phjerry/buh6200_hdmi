/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:OTP comon macro and API.
 * Author: Linux SDK team
 * Create: 2019/06/20
 */
#ifndef __DRV_OTP_H__
#define __DRV_OTP_H__

#include "hi_osal.h"
#include "drv_otp_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

struct otp_mgmt{
    osal_mutex     lock;
    hi_void        *io_base;
};

struct otp_mgmt *__get_otp_mgmt(hi_void);

hi_s32 drv_otp_init(hi_void);
hi_void drv_otp_deinit(hi_void);
hi_s32 otp_mmap_impl(osal_vm *vm, hi_ulong start, hi_ulong end, hi_ulong vm_pgoff, hi_void *private_data);
hi_s32 otp_open_impl(hi_void *private_data);
hi_s32 otp_release_impl(hi_void *private_data);
hi_slong otp_ioctl_impl(struct file *file, unsigned int cmd, hi_void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __DRV_OTP_H__ */
