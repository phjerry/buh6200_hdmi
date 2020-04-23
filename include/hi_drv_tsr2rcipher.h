/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher kernel export interface definition.
 */

#ifndef __HI_DRV_TSR2RCIPHER_H__
#define __HI_DRV_TSR2RCIPHER_H__

#include "drv_tsr2rcipher_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tsr2rcipher_session;

typedef hi_s32 (*fn_tsr2rcipher_get_capability)(tsr2rcipher_capability *cap);

typedef struct {
    fn_tsr2rcipher_get_capability pfn_tsr2rcipher_get_capability;
} fn_tsr2rcipher_export_func;

hi_s32 hi_drv_tsr2rcipher_get_capability(tsr2rcipher_capability *cap);

hi_s32 hi_drv_tsr2rcipher_create_session(struct tsr2rcipher_session **new_session);
hi_s32 hi_drv_tsr2rcipher_destroy_session(struct tsr2rcipher_session *session);

/* tsr2rcipher channel */
hi_s32 hi_drv_tsr2rcipher_create(const tsr2rcipher_attr *tsc_attr, hi_handle *handle, struct tsr2rcipher_session *session);
hi_s32 hi_drv_tsr2rcipher_destroy(hi_handle handle);
hi_s32 hi_drv_tsr2rcipher_get_attr(hi_handle handle, tsr2rcipher_attr *tsc_attr);
hi_s32 hi_drv_tsr2rcipher_set_attr(hi_handle handle, const tsr2rcipher_attr *tsc_attr);
hi_s32 hi_drv_tsr2rcipher_get_keyslot_handle(hi_handle tsc_handle, hi_handle *ks_handle);
hi_s32 hi_drv_tsr2rcipher_attach_keyslot(hi_handle tsc_handle, hi_handle ks_handle);
hi_s32 hi_drv_tsr2rcipher_detach_keyslot(hi_handle tsc_handle, hi_handle ks_handle);
hi_s32 hi_drv_tsr2rcipher_set_iv(hi_handle handle, tsr2rcipher_iv_type iv_type, hi_u8 *iv, hi_u32 iv_len);
hi_s32 hi_drv_tsr2rcipher_encrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len);
hi_s32 hi_drv_tsr2rcipher_decrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len);

hi_s32 hi_drv_tsr2rcipher_suspend(hi_void);
hi_s32 hi_drv_tsr2rcipher_resume(hi_void);

hi_s32  tsr2rcipher_mod_init(hi_void);
hi_void tsr2rcipher_mod_exit(hi_void);

#ifdef __cplusplus
}
#endif

#endif  /* __HI_DRV_TSR2RCIPHER_H__ */
