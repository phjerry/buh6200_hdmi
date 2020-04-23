/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: klad basic function impl.
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_KLAD_FUNCTION_H__
#define __DRV_KLAD_FUNCTION_H__

#include "drv_klad_sw_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 klad_slot_mgmt_init(hi_void);
hi_void klad_slot_mgmt_exit(hi_void);
hi_s32 klad_slot_mgmt_suspend(hi_void);
hi_s32 klad_slot_mgmt_resume(hi_void);
hi_s32 klad_slot_mgmt_create_slot(hi_handle *handle);
hi_s32 klad_slot_mgmt_destroy_slot(hi_handle handle);

hi_s32 klad_slot_mgmt_com_create(hi_handle handle);
hi_s32 klad_slot_mgmt_com_destroy(hi_handle handle);
hi_s32 klad_slot_mgmt_ta_create(hi_handle handle);
hi_s32 klad_slot_mgmt_ta_destroy(hi_handle handle);
hi_s32 klad_slot_mgmt_fp_create(hi_handle handle);
hi_s32 klad_slot_mgmt_fp_destroy(hi_handle handle);
hi_s32 klad_slot_mgmt_nonce_create(hi_handle handle);
hi_s32 klad_slot_mgmt_nonce_destroy(hi_handle handle);
hi_s32 klad_slot_mgmt_clr_create(hi_handle handle);
hi_s32 klad_slot_mgmt_clr_destroy(hi_handle handle);

hi_s32 klad_slot_mgmt_instance_destroy(hi_handle handle);
hi_s32 klad_slot_mgmt_instance_create(hi_handle handle, hi_u32 klad_type);

/*
 * API : klad software instance defination.
 */
hi_s32 klad_slot_com_set_rootkey_attr(hi_handle handle, hi_rootkey_attr *rootkey);
hi_s32 klad_slot_com_set_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_com_get_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_com_attach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_com_detach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_com_set_session_key(hi_handle handle, hi_klad_session_key *session_key);
hi_s32 klad_slot_com_set_content_key(hi_handle handle, hi_klad_content_key *content_key);
hi_s32 klad_slot_com_start(hi_handle handle);


hi_s32 klad_slot_fp_set_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_fp_get_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_fp_attach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_fp_detach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_fp_set_session_key(hi_handle handle, hi_klad_session_key *session_key);
hi_s32 klad_slot_fp_set_fp_key(hi_handle handle, hi_klad_nonce_key *fp_key);
hi_s32 klad_slot_fp_start(hi_handle handle);

hi_s32 klad_slot_ta_set_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_ta_get_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_ta_attach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_ta_detach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_ta_set_session_key(hi_handle handle, hi_klad_ta_key *ta_key);
hi_s32 klad_slot_ta_set_trans_data(hi_handle handle, hi_klad_trans_data *trans_data);
hi_s32 klad_slot_ta_set_content_key(hi_handle handle, hi_klad_ta_key *ta_key);
hi_s32 klad_slot_ta_start(hi_handle handle);


hi_s32 klad_slot_nonce_set_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_nonce_get_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_nonce_attach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_nonce_detach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_nonce_set_session_key(hi_handle handle, hi_klad_session_key *session_key);
hi_s32 klad_slot_nonce_set_nonce_key(hi_handle handle, hi_klad_nonce_key *nonce_key);
hi_s32 klad_slot_nonce_start(hi_handle handle);

hi_s32 klad_slot_clr_set_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_clr_get_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_clr_attach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_clr_detach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_clr_set_key(hi_handle handle, hi_klad_clear_key *clr_key);
hi_s32 klad_slot_clr_start(hi_handle handle);


hi_s32 klad_slot_instance_set_attr(hi_handle handle, hi_klad_attr *attr);
hi_s32 klad_slot_instance_get_attr(hi_handle handle, hi_klad_attr *attr);

hi_s32 klad_slot_instance_attach(hi_handle handle, hi_handle target);
hi_s32 klad_slot_instance_detach(hi_handle handle, hi_handle target);

hi_s32 klad_slot_instance_set_session_key(hi_handle handle, hi_klad_session_key *session_key);
hi_s32 klad_slot_instance_set_content_key(hi_handle handle, hi_klad_content_key *content_key);
hi_s32 klad_slot_instance_start(hi_handle handle);

struct klad_slot_mgmt *__get_klad_slot_mgmt(hi_void);
struct klad_slot_mgmt *get_klad_slot_mgmt(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_KLAD_FUNCTION_H__

