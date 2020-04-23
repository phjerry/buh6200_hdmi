/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: klad basic function impl.
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_KLAD_HW_FUNCTION_H__
#define __DRV_KLAD_HW_FUNCTION_H__

#include <linux/delay.h>
#include "drv_klad_hw_define.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 klad_r_get_raw(struct klad_r_base *obj);
hi_void klad_r_put(struct klad_r_base *obj);

hi_s32 klad_hw_com_hkl_find(hi_handle handle, struct klad_r_com_hkl **obj);
hi_s32 klad_hw_com_hkl_open(struct klad_entry_key *attrs, struct klad_r_com_hkl **obj);
hi_s32 klad_hw_com_hkl_start(struct klad_r_com_hkl *com_hkl, const hi_klad_com_entry *instance);
hi_s32 klad_hw_com_hkl_close(struct klad_r_com_hkl *com_hkl);

hi_s32 klad_hw_ta_hkl_find(hi_handle handle, struct klad_r_ta_hkl **obj);
hi_s32 klad_hw_ta_hkl_open(struct klad_entry_key *attrs, struct klad_r_ta_hkl **obj);
hi_s32 klad_hw_ta_hkl_start(struct klad_r_ta_hkl *ta_hkl, const hi_klad_ta_entry *instance);
hi_s32 klad_hw_ta_hkl_close(struct klad_r_ta_hkl *ta_hkl);

hi_s32 klad_hw_fp_hkl_find(hi_handle handle, struct klad_r_fp_hkl **obj);
hi_s32 klad_hw_fp_hkl_open(struct klad_entry_key *attrs, struct klad_r_fp_hkl **obj);
hi_s32 klad_hw_fp_hkl_start(struct klad_r_fp_hkl *fp_hkl, const hi_klad_fp_entry *instance);
hi_s32 klad_hw_fp_hkl_close(struct klad_r_fp_hkl *fp_hkl);

hi_s32 klad_hw_nonce_hkl_find(hi_handle handle, struct klad_r_nonce_hkl **obj);
hi_s32 klad_hw_nonce_hkl_open(struct klad_entry_key *attrs, struct klad_r_nonce_hkl **obj);
hi_s32 klad_hw_nonce_hkl_start(struct klad_r_nonce_hkl *nonce_hkl, const hi_klad_nonce_entry *instance);
hi_s32 klad_hw_nonce_hkl_close(struct klad_r_nonce_hkl *nonce_hkl);

hi_s32 klad_hw_clr_route_open(struct klad_entry_key *attrs, struct klad_r_clr_route **obj);
hi_s32 klad_hw_clr_route_start(struct klad_r_clr_route *clr_route, const hi_klad_clr_entry *instance);
hi_s32 klad_hw_clr_route_close(struct klad_r_clr_route *clr_route);

/*
 * KLAD mgmt helper functions.
 */
hi_s32 klad_mgmt_init(hi_void);
hi_void klad_mgmt_exit(hi_void);

hi_s32 klad_mgmt_suspend(hi_void);
hi_s32 klad_mgmt_resume(hi_void);

hi_s32 klad_mgmt_create_com_hkl(struct klad_entry_key *attrs, struct klad_r_com_hkl **com_hkl);
hi_s32 klad_mgmt_destroy_com_hkl(struct klad_r_base *obj);

hi_s32 klad_mgmt_create_ta_hkl(struct klad_entry_key *attrs, struct klad_r_ta_hkl **ta_hkl);
hi_s32 klad_mgmt_destroy_ta_hkl(struct klad_r_base *obj);

hi_s32 klad_mgmt_create_fp_hkl(struct klad_entry_key *attrs, struct klad_r_fp_hkl **fp_hkl);
hi_s32 klad_mgmt_destroy_fp_hkl(struct klad_r_base *obj);

hi_s32 klad_mgmt_create_nonce_hkl(struct klad_entry_key *attrs, struct klad_r_nonce_hkl **nonce_hkl);
hi_s32 klad_mgmt_destroy_nonce_hkl(struct klad_r_base *obj);

hi_s32 klad_mgmt_create_clr_route(struct klad_entry_key *attrs, struct klad_r_clr_route **clr_route);
hi_s32 klad_mgmt_destroy_clr_route(struct klad_r_base *obj);

struct klad_mgmt *__get_klad_mgmt(hi_void);
struct klad_mgmt *get_klad_mgmt(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_KLAD_HW_FUNCTION_H__

