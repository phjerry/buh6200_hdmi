/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher basic function decl.
 */

#ifndef __DRV_TSR2RCIPHER_FUNC_H__
#define __DRV_TSR2RCIPHER_FUNC_H__

#include "drv_tsr2rcipher_ioctl.h"
#include "drv_tsr2rcipher_define.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tsr2rcipher_r_ch_ops {
    hi_s32(*create)(struct tsr2rcipher_r_ch *rch);
    hi_s32(*get_attr)(struct tsr2rcipher_r_ch *rch, tsr2rcipher_attr *attr);
    hi_s32(*set_attr)(struct tsr2rcipher_r_ch *rch, const tsr2rcipher_attr *attr);
    hi_s32(*get_ks_handle)(struct tsr2rcipher_r_ch *rch, hi_handle *ks_handle);
    hi_s32(*attach_ks)(struct tsr2rcipher_r_ch *rch, hi_handle ks_handle);
    hi_s32(*detach_ks)(struct tsr2rcipher_r_ch *rch, hi_handle ks_handle);

    hi_s32(*set_iv)(struct tsr2rcipher_r_ch *rch, tsr2rcipher_iv_type iv_type, const hi_u8 *iv, hi_u32 len);
    hi_s32(*encrypt)(struct tsr2rcipher_r_ch *rch, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len);
    hi_s32(*decrypt)(struct tsr2rcipher_r_ch *rch, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len);
    hi_s32(*destroy)(struct tsr2rcipher_r_ch *rch);

    hi_s32(*suspend)(struct tsr2rcipher_r_ch *rch);
    hi_s32(*resume)(struct tsr2rcipher_r_ch *rch);
};

hi_s32 tsr2rcipher_ch_create(const tsr2rcipher_attr *attr, hi_handle *handle, struct tsr2rcipher_session *session);
hi_s32 tsr2rcipher_ch_get_attr(hi_handle handle, tsr2rcipher_attr *attr);
hi_s32 tsr2rcipher_ch_set_attr(hi_handle handle, const tsr2rcipher_attr *attr);
hi_s32 tsr2rcipher_ch_get_ks_handle(hi_handle tsc_handle, hi_handle *ks_handle);
hi_s32 tsr2rcipher_ch_attach_ks(hi_handle tsc_handle, hi_handle ks_handle);
hi_s32 tsr2rcipher_ch_detach_ks(hi_handle tsc_handle, hi_handle ks_handle);
hi_s32 tsr2rcipher_ch_set_iv(hi_handle handle, tsr2rcipher_iv_type iv_type, const hi_u8 *iv, hi_u32 iv_len);
hi_s32 tsr2rcipher_ch_encrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len);
hi_s32 tsr2rcipher_ch_decrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len);
hi_s32 tsr2rcipher_ch_destroy(hi_handle handle);

/* global tsr2rcipher resource management. */
struct tsr2rcipher_mgmt *_tsr2rcipher_get_mgmt(hi_void);
struct tsr2rcipher_mgmt *tsr2rcipher_get_mgmt(hi_void);

hi_s32  tsr2rcipher_mgmt_init(hi_void);
hi_void tsr2rcipher_mgmt_exit(hi_void);

hi_s32  tsr2rcipher_mgmt_get_cap(tsr2rcipher_capability *cap);

hi_s32  tsr2rcipher_mgmt_suspend(hi_void);
hi_s32  tsr2rcipher_mgmt_resume(hi_void);

#ifdef __cplusplus
}
#endif

#endif  /* __DRV_TSR2RCIPHER_FUNC_H__ */
