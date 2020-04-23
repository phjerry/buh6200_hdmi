/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of common functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMI_COMMON_H__
#define __DRV_HDMI_COMMON_H__

#include "hi_type.h"
#include "hi_osal.h"
#include "hi_drv_video.h"
#include "hi_drv_hdmirx.h"
#include "hi_drv_hdmirx.h"

#define HDR_ARRAY_SIZE 9
#define SUPPORT_CEC 0
#define SUPPORT_OTP 0
#define SUPPORT_HDMI_AFFECT_ATV 0

#define HDMIRX_CHECK_NULL_PTR(ptr, ret)       \
    do {                                      \
        if ((ptr) == NULL) {                    \
            hi_err_hdmirx("null pointer!\n"); \
            (ret) = HI_FAILURE;               \
        }                                     \
    } while (0)

typedef struct hi_hdmirx_call_func {
    hi_s32 (*hdmirx_drv_ctrl_connect)(hi_drv_hdmirx_port port);
    hi_s32 (*hdmirx_drv_ctrl_disconnect)(hi_drv_hdmirx_port port);
    hi_s32 (*hdmirx_drv_ctrl_get_sig_status)(hi_drv_sig_info *sig_sta);
    hi_s32 (*hdmirx_drv_ctrl_get_audio_status)(hi_drv_sig_info *sig_sta);
    hi_s32 (*hdmirx_drv_ctrl_get_timing)(hi_drv_hdmirx_timing *timing_info);
    hi_s32 (*hdmirx_drv_ctrl_get_audio_info)(hi_drv_hdmirx_aud_info *audio_info);
    hi_s32 (*hdmirx_drv_ctrl_get_offline_det_status)(hi_drv_hdmirx_offline_status *offline_stat);
    hi_s32 (*hdmirx_drv_ctrl_load_hdcp)(hi_drv_hdmirx_hdcp_info *hdcp_key);
    hi_s32 (*hdmirx_drv_ctrl_update_edid)(hi_drv_hdmirx_edid_info *edid);
    hi_s32 (*hdmirx_drv_ctrl_set_hpd_value)(hi_drv_hdmirx_hpd *hpd);
    hi_s32 (*hdmirx_drv_ctrl_init_edid)(hi_drv_hdmirx_edid_init_info *init_edid);
    hi_s32 (*hdmirx_drv_ctrl_set_edid_mode)(hi_drv_hdmirx_edid_mode_info *edid_mode);
#if SUPPORT_CEC
    hi_s32 (*hdmirx_drv_ctrl_cec_enable)(hi_bool *enable);
    hi_s32 (*hdmirx_drv_ctrl_cec_set_command)(hi_drv_hdmirx_cec_cmd *cec_cmd);
    hi_s32 (*hdmirx_drv_ctrl_cec_get_command)(hi_drv_hdmirx_cec_cmd *cec_cmd);
    hi_s32 (*hdmirx_drv_ctrl_cec_get_cur_state)(hi_drv_hdmirx_cec_cmd_state_data *cmd_state);
    hi_s32 (*hdmirx_drv_ctrl_cec_standby_enable)(hi_bool enable);
    hi_s32 (*hdmirx_drv_ctrl_cec_get_msg_cnt)(hi_u8 *cmd_cnt);
#endif
} HDMIRX_CALL_FUNC, *HDMIRX_CALL_FUNC_PTR;

typedef struct hi_hdmirx_ctrl_func {
    hi_bool used;
    HDMIRX_CALL_FUNC *fun;
} HDMIRX_CTRL_FUNC;

typedef enum hi_hdmirx_edid_type {
    HDMIRX_EDID_14 = 0,
    HDMIRX_EDID_20,
    HDMIRX_EDID_21,
    HDMIRX_EDID_MHL,
    HDMIRX_EDID_MAX
} hdmirx_edid_type;

typedef enum hi_hdmirx_signal {
    HDMIRX_SIGNAL_HDRDATA = 0,
    HDMIRX_SIGNAL_ARCEN,
    HDMIRX_SIGNAL_MAX
} hdmirx_signal;

typedef enum hi_hdmirx_rgb_range {
    HDMIRX_RGB_DEFAULT,
    HDMIRX_RGB_LIMIT_RANGE,
    HDMIRX_RGB_FULL_RANGE,
    HDMIRX_RGB_RANGE_BUTT
} hdmirx_rgb_range;

typedef enum hi_hdmirx_range {
    HDMIRX_LIMIT_RANGE,
    HDMIRX_FULL_RANGE,
    HDMIRX_RANGE_BUTT
} hdmirx_range;

typedef struct hi_hi_hdmirx_dolby_info {
    hi_bool low_latency;
    hi_bool backlt_ctrl_enable;
    hi_bool auxiliary_enable;
    hi_u32 eff_max_luminance;
    hi_u32 auxiliary_runmode;
    hi_u32 auxiliary_runversion;
    hi_u32 auxiliary_debug;
} hi_hdmirx_dolby_info;

typedef struct hi_hi_hdmirx_hdr10_plus_dynamic_metadata_info {
    hi_u8 application_version;
    hi_u8 targeted_system_display_maximum_luminance;
    hi_u8 average_maxrgb;
    hi_u8 distribution_values[HDR_ARRAY_SIZE]; /* 9: array size */
    hi_u8 num_bezier_curve_anchors;
    hi_u32 knee_point_x;
    hi_u32 knee_point_y;
    hi_u8 bezier_curve_anchors[HDR_ARRAY_SIZE]; /* 9: array size */
    hi_u8 graphics_overlayflag;
    hi_u8 no_delay_flag;
} hi_hdmirx_hdr10_plus_dynamic_metadata_info;

typedef struct {
    hi_bool edid_state;
    hi_bool hdcp_state;
} hdmirx_intf_context;

typedef hi_s32 (*hdmirx_cmd_fp)(hi_char *argv);
typedef hi_s32 (*hdmirx_io_ctrl_fp)(hi_void *arg, hdmirx_intf_context *intf_ctx_p);

typedef struct {
    unsigned int cmd;
    hdmirx_io_ctrl_fp func;
} hdmirx_ioctrlfunc;

typedef struct {
    hi_u8 *cmd;
    hdmirx_cmd_fp hdmirx_cmd;
} hdmirx_drv_proc_cmdlist;

typedef struct {
    hi_u32 disp_primaries_x0;
    hi_u32 disp_primaries_y0;
    hi_u32 disp_primaries_x1;
    hi_u32 disp_primaries_y1;
    hi_u32 disp_primaries_x2;
    hi_u32 disp_primaries_y2;
    hi_u32 white_point_x;
    hi_u32 white_point_y;
    hi_u32 max_disp_mastering_luminance;
    hi_u32 min_disp_mastering_luminance;
    hi_u32 max_content_light_level;
    hi_u32 max_frm_average_light_level;
} hdmirx_hdr10_metadata_stream;

typedef enum {
    HI_DRV_HDMIRX_DATA_ROUTE_CTRL0, /* 2.0 ctrl HDMIRX data route to HDMIRX control core 0. */
    HI_DRV_HDMIRX_DATA_ROUTE_CTRL1, /* 1.4 ctrl HDMIRX data route to HDMIRX control core 1. */
    HI_DRV_HDMIRX_DATA_ROUTE_BUTT   /* core 0 and 1 with the data left and right. */
} drv_hdmirx_data_route;

hi_void hdmirx_register_fun(hi_u32 ctrl, HDMIRX_CALL_FUNC *fun);

hi_void hdmirx_send_signal(hi_u32 ctrl, hdmirx_signal signal, hi_void *arg);

long hdmirx_time_diff_us(osal_timeval cur_time, osal_timeval last_time);

long hdmirx_time_diff_ms(osal_timeval cur_time, osal_timeval last_time);

#endif /* __DRV_HDMIRX_COMMON_H__ */


