/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */

#ifndef __VENC_RC_EXT_H__
#define __VENC_RC_EXT_H__

#include "venc_osal.h"

#define  VENC_NULL_FUN_PTR    (-1)

typedef enum {
    RC_TYPE_CBR,
    RC_TYPE_VBR,
} rc_control_type;

typedef enum {
    ENCODE_TYPE_I,
    ENCODE_TYPE_P,
} rc_encode_type;

typedef enum {
    RC_EVENT_TYPE_TIMING,                     /* this event means time after a second */
} rc_event_type;

typedef struct {
    hi_u32 min_qp;
    hi_u32 max_qp;
    hi_u32 pic_width;
    hi_u32 pic_height;
    hi_u32 vo_frm_rate;
    hi_u32 bit_rate;
    hi_u32 gop;
    rc_control_type RCType;
} venc_rc_cfg;

typedef struct {
    hi_u32 target_bits;
    hi_u32 start_qp;
} venc_rc_output;

typedef struct {
    rc_encode_type enc_type;
    hi_u32 i_frm_insert_flag;
    hi_u32 re_encode_flag;
    hi_u32 reduce_bits_flag;
} venc_rc_input;

typedef struct {
    hi_s32 (*pfun_rc_create_instance)(hi_handle *rc, venc_rc_cfg *configure);
    hi_s32 (*pfun_rc_destroy_instance)(hi_handle rc);

    hi_s32 (*pfun_rc_set_config)(hi_handle rc, venc_rc_cfg *configure);
    hi_s32 (*pfun_rc_get_config)(hi_handle rc, venc_rc_cfg *configure);

    hi_s32 (*pfun_rc_reset)(hi_handle rc, venc_rc_cfg *configure);
    hi_s32 (*pfun_rc_set_last_pic_bits)(hi_handle rc, hi_u32 pic_bits);

    hi_s32 (*pfun_rc_calc_one_frame)(hi_handle rc, venc_rc_input *param_in venc_rc_output *param_out);

    hi_s32 (*pfun_rc_notify_event)(hi_handle rc, rc_event_type event_type);
} venc_rc_fun_ptr;

extern venc_rc_fun_ptr g_venc_rc_fun_ptr;

#define venc_rc_create_instance(rc, configure) \
    (g_venc_rc_fun_ptr.pfun_rc_create_instance ? \
    g_venc_rc_fun_ptr.pfun_rc_create_instance(rc, configure) : \
    VENC_NULL_FUN_PTR)

#define venc_rc_destroy_instance(rc) \
    (g_venc_rc_fun_ptr.pfun_rc_destroy_instance ? \
    g_venc_rc_fun_ptr.pfun_rc_destroy_instance(rc) : \
    VENC_NULL_FUN_PTR)

#define venc_rc_set_config(rc, configure) \
    (g_venc_rc_fun_ptr.pfun_rc_set_config ? \
    g_venc_rc_fun_ptr.pfun_rc_set_config(rc, configure) : \
    VENC_NULL_FUN_PTR)

#define venc_rc_get_config(rc, configure) \
    (g_venc_rc_fun_ptr.pfun_rc_get_config ? \
    g_venc_rc_fun_ptr.pfun_rc_get_config(rc, configure) : \
    VENC_NULL_FUN_PTR)

#define venc_rc_reset(rc, configure) \
    (g_venc_rc_fun_ptr.pfun_rc_reset ? \
    g_venc_rc_fun_ptr.pfun_rc_reset(rc, configure) : \
    VENC_NULL_FUN_PTR)

#define venc_rc_set_last_pic_bits(rc, pic_bits) \
    (g_venc_rc_fun_ptr.pfun_rc_set_last_pic_bits ? \
    g_venc_rc_fun_ptr.pfun_rc_set_last_pic_bits(rc, pic_bits) : \
    VENC_NULL_FUN_PTR)

#define venc_rc_calc_one_frame(rc, param_in, param_out) \
    (g_venc_rc_fun_ptr.pfun_rc_calc_one_frame ? \
    g_venc_rc_fun_ptr.pfun_rc_calc_one_frame(rc, param_in, param_out) : \
    VENC_NULL_FUN_PTR)

#define venc_rc_notify_event(rc, event_type) \
    (g_venc_rc_fun_ptr.pfun_rc_notify_event ? \
    g_venc_rc_fun_ptr.pfun_rc_notify_event(rc, event_type) : \
    VENC_NULL_FUN_PTR)


#endif  /* __VENC_RC_EXT_H__ */

