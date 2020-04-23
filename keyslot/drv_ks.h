/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KS comon macro and API.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KS_H__
#define __DRV_KS_H__

#include "hi_osal.h"
#include "hi_type.h"
#include "drv_ks_define.h"
#include "hi_drv_keyslot.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

hi_s32 ks_mmap_impl(osal_vm *vm, unsigned long start, hi_ulong end, hi_ulong vm_pgoff, hi_void *private_data);
hi_s32 ks_open_impl(hi_void **private_data);
hi_s32 ks_release_impl(hi_void *private_data);
hi_s32 drv_ks_init(hi_void);
hi_void drv_ks_deinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __DRV_KS_H__ */
