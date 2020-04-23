/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdcp1.x hal layer head file
 * Author: Hisilicon HDMI software group
 * Create: 2019/11/15
 */
#ifndef __HAL_HDMITX_HDCP1X_H__
#define __HAL_HDMITX_HDCP1X_H__

#include "hi_type.h"

struct hisilicon_hdmi;

#define HDCP1X_SIZE_5BYTES_KSV         5
#define HDCP1X_SIZE_2BYTES_RI          2
#define HDCP1X_SIZE_20BYTES_VI         20
#define HDCP1X_SIZE_1BYTES_BCAPS       1
#define HDCP1X_SIZE_2BYTES_BSTATUS     2
#define HDCP1X_SIZE_8BYTES_AN          8
#define HDCP1X_SIZE_MAX_BKSV_LIST      (HDCP1X_SIZE_5BYTES_KSV * 128)

struct hdcp1x_ri_result {
    hi_u8 cnt_127frm_err;
    hi_u8 cnt_000frm_err;
    hi_u8 cnt_notchg_err;
    hi_u8 cnt_nodone_err;
    hi_u8 fsm_state;
};

struct hdcp1x_src_status {
    hi_u8 an[HDCP1X_SIZE_8BYTES_AN];
    hi_u8 a_ksv[HDCP1X_SIZE_5BYTES_KSV];
    hi_u8 a_r0[HDCP1X_SIZE_2BYTES_RI];
    hi_u8 a_vi[HDCP1X_SIZE_20BYTES_VI];
    hi_bool hdcp1x_mode;
    hi_bool load_key;
    hi_u8 crc_err;
    hi_bool repeater_en;
    hi_bool encrypiton_en;
    hi_bool sha_ready;
    hi_bool sha_en;
    hi_bool rsrt_en;
    hi_bool ri_validate_en;
    struct hdcp1x_ri_result ri_result;
};

struct hdcp1x_hal_ops {
    hi_void (*set_mode)(struct hisilicon_hdmi *hdmi);
    hi_s32 (*load_key)(struct hisilicon_hdmi *hdmi);
    hi_void (*gerarate_an)(struct hisilicon_hdmi *hdmi, hi_u8 *an_buf, hi_u32 size);
    hi_void (*set_b_ksv)(struct hisilicon_hdmi *hdmi, hi_u8 *bksv, hi_u32 size);
    hi_void (*get_a_ksv)(struct hisilicon_hdmi *hdmi, hi_u8 *aksv, hi_u32 size);
    hi_void (*get_a_r0)(struct hisilicon_hdmi *hdmi, hi_u8 *r0, hi_u32 size);
    hi_void (*set_repeater)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_void (*set_encryption)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_void (*set_b_status)(struct hisilicon_hdmi *hdmi, hi_u16 bstatus);
    hi_void (*get_a_vi)(struct hisilicon_hdmi *hdmi, hi_u8 *a_vi, hi_u32 size);
    hi_void (*set_b_ksv_list)(struct hisilicon_hdmi *hdmi, hi_u8 *bksv_list, hi_u32 size);
    hi_bool (*is_sha_ready)(struct hisilicon_hdmi *hdmi);
    hi_void (*set_no_downstream)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_void (*set_sha_calculate)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_void (*set_b_vi)(struct hisilicon_hdmi *hdmi, hi_u8 *v_bi, hi_u32 size);
    hi_bool (*is_vi_valid_ok)(struct hisilicon_hdmi *hdmi);
    hi_void (*clear_ri_err)(struct hisilicon_hdmi *hdmi);
    hi_void (*set_ri_auto_validate)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_void (*get_ri_validate_result)(struct hisilicon_hdmi *hdmi, struct hdcp1x_ri_result *result);
    hi_void (*set_1x_srst)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_void (*get_irq_status)(struct hisilicon_hdmi *hdmi, hi_u32 *irq_status);
    hi_void (*clear_irq_status)(struct hisilicon_hdmi *hdmi, hi_u32 irq_status);
    hi_void (*set_irq)(struct hisilicon_hdmi *hdmi, hi_bool enable);
    hi_void (*get_hw_status)(struct hisilicon_hdmi *hdmi, struct hdcp1x_src_status *status);
};

struct hdcp1x_hal_ops *hal_hdmitx_hdcp1x_get_ops(hi_void);

#endif /* __HAL_HDMITX_HDCP1X_H__ */

