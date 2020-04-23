/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KLAD comon macro and API.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KLAD_HW_H__
#define __DRV_KLAD_HW_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_drv_klad.h"
#include "drv_klad_com.h"
#include "drv_ioctl_klad.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

struct klad_r_base_attr {
    hi_u32                     klad_type;
    hi_u32                     vendor_id;
    hi_rootkey_select          root_slot;
    hi_bool                    unique;
    hi_bool                    is_secure_key;
    hi_u32                     module_id[MODULE_ID_CNT];
};

struct klad_entry_key {
    hi_void                    *session;
    struct klad_r_base_attr    hkl_base_attr;
};

hi_void hi_klad_dump_hex(const hi_char *str, const hi_u8 *buf, hi_u32 len);
#define dbg_hi_klad_dump_hex  hi_klad_dump_hex

hi_s32 hi_drv_hw_com_klad_process(struct klad_entry_key *attrs, hi_klad_com_entry *entry);
hi_s32 hi_drv_hw_fp_klad_process(struct klad_entry_key *attrs, hi_klad_fp_entry *entry);
hi_s32 hi_drv_hw_ta_klad_process(struct klad_entry_key *attrs, hi_klad_ta_entry *entry);
hi_s32 hi_drv_hw_nonce_klad_process(struct klad_entry_key *attrs, hi_klad_nonce_entry *entry);
hi_s32 hi_drv_hw_clr_route_process(struct klad_entry_key *attrs, hi_klad_clr_entry *entry);
hi_s32 hi_drv_hw_clr_iv_process(const klad_clear_iv_param *clr_iv);

hi_s32 hi_drv_hw_com_klad_create(hi_handle *handle, struct klad_entry_key *attrs);
hi_s32 hi_drv_hw_com_klad_startup(hi_handle handle, hi_klad_com_entry *entry);
hi_s32 hi_drv_hw_com_klad_destroy(hi_handle handle);

hi_s32 hi_drv_hw_fp_klad_create(hi_handle *handle, struct klad_entry_key *attrs);
hi_s32 hi_drv_hw_fp_klad_startup(hi_handle handle, hi_klad_fp_entry *entry);
hi_s32 hi_drv_hw_fp_klad_destroy(hi_handle handle);

hi_s32 hi_drv_hw_ta_klad_create(hi_handle *handle, struct klad_entry_key *attrs);
hi_s32 hi_drv_hw_ta_klad_startup(hi_handle handle, hi_klad_ta_entry *entry);
hi_s32 hi_drv_hw_ta_klad_destroy(hi_handle handle);

hi_s32 hi_drv_hw_nonce_klad_create(hi_handle *handle, struct klad_entry_key *attrs);
hi_s32 hi_drv_hw_nonce_klad_startup(hi_handle handle, hi_klad_nonce_entry *entry);
hi_s32 hi_drv_hw_nonce_klad_destroy(hi_handle handle);

hi_s32 hi_drv_hw_klad_init(hi_void);
hi_void hi_drv_hw_klad_exit(hi_void);

hi_void dump_hkl_attr(struct klad_r_base_attr *attrs);
hi_void dump_hkl_clr_route(const hi_klad_clr_entry *entry);
hi_void dump_hkl_com(const hi_klad_com_entry *entry);
hi_void dump_hkl_create_attr(const hi_klad_create_attr *attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* endof #ifdef __cplusplus */

#endif /* endof #ifndef __DRV_KLAD_HW_H__ */
