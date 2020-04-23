/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Keyladder drvier level head file.
 * Author: linux SDK team
 * Create: 2019/07/10
 */

#ifndef __HI_DRV_KLAD_H__
#define __HI_DRV_KLAD_H__

#include "drv_klad_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*************************** structure definition ****************************/
/** \addtogroup      KLAD */
/** @{ */  /** <!-- [KLAD] */

typedef struct {
    hi_handle ks_handle;
    hi_klad_attr attr;
    hi_klad_clear_key clr_key;
} klad_clear_cw_param;

typedef struct {
    hi_handle ks_handle;
    hi_bool is_odd;                    /* odd or even key flag. */
    hi_u8  iv[HI_KLAD_MAX_IV_LEN];
} klad_clear_iv_param;

typedef hi_s32(*klad_clear_cw_func)(const klad_clear_cw_param *clear_cw);

typedef hi_s32(*klad_clear_iv_func)(const klad_clear_iv_param *clear_iv);

typedef struct {
    klad_clear_cw_func klad_set_clear_key;
    klad_clear_iv_func klad_set_clear_iv;   /* set iv only used for tscipher. */
} klad_export_func;

hi_s32 hi_drv_klad_mod_init(hi_void);

hi_void hi_drv_klad_mod_exit(hi_void);

hi_s32 hi_drv_klad_create(hi_handle *create_handle);

hi_s32 hi_drv_klad_destroy(hi_handle handle);

hi_s32 hi_drv_klad_set_attr(hi_handle handle, hi_klad_attr *attr);

hi_s32 hi_drv_klad_get_attr(hi_handle handle, hi_klad_attr *attr);

hi_s32 hi_drv_klad_attach(hi_handle handle, hi_handle target);

hi_s32 hi_drv_klad_detach(hi_handle handle, hi_handle target);

hi_s32 hi_drv_klad_set_session_key(hi_handle handle, hi_klad_session_key *session_key);

hi_s32 hi_drv_klad_set_content_key(hi_handle handle, hi_klad_content_key *content_key);

hi_s32 hi_drv_klad_set_clear_key(hi_handle handle, hi_klad_clear_key *clear_key);

hi_s32 hi_drv_klad_clear_cw(const klad_clear_cw_param *clear_cw);

hi_s32 hi_drv_klad_clear_iv(const klad_clear_iv_param *clear_iv);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DRV_KLAD_H__ */

