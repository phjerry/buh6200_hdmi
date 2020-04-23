/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: vdp
* Create: 2019-04-12
 */


#ifndef __DRV_DISPLAY_H__
#define __DRV_DISPLAY_H__

#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "drv_disp_ioctl.h"
#include "drv_disp_ext.h"

#define HI_DRV_DISP_OFFSET_MAX              200
#define HI_DRV_DISP_OFFSET_HORIZONTAL_ALIGN 0xFFFFFFFEul
#define HI_DRV_DISP_OFFSET_VERTICAL_ALIGN   0xFFFFFFFCul

#define HI_DRV_DISP_VIRTSCREEN_MAX 3840
#define HI_DRV_DISP_VIRTSCREEN_MIN 480
#define DISPLAY_DEFAULT_VIRT_SCREEN_WIDTH 1280
#define DISPLAY_DEFAULT_VIRT_SCREEN_HEIGHT 720
#define SCREEN_DISP_WIDTH_MIN 64
#define SCREEN_DISP_HEIGHT_MIN 64
#define HI_DRV_DISP_ASPECT_RATIO_MAX 16
#define HI_DRV_DISP_ALPHA_MIN 0
#define HI_DRV_DISP_ALPHA_MAX 100
#define HI_DRV_DISP_LOGIC_ALPHA_MAX 255


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
#define DISP_CHECK_ID(id)    \
    {                                \
        if ((id >= HI_DRV_DISPLAY_2) ) { \
            hi_err_disp("DISP ERROR! Invalid display in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_INVALID_PARA;  \
        }                             \
    }
#define DISP_CHECK_NULL_POINTER(ptr) \
    {                                \
        if (ptr == HI_NULL) { \
            hi_err_disp("DISP ERROR! Input null pointer in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_NULL_PTR;  \
        }                             \
    }
typedef struct {

    hi_bool master;
    hi_bool slave;
    hi_drv_display attached_disp;
    hi_bool enable;
    hi_bool licensed;

    hi_rect virtaul_screen;
    hi_drv_disp_offset offset_info;
    hi_drv_aspect_ratio aspect_ratio;

    hi_drv_disp_stereo_mode stereo;
    hi_bool right_eye_first;
    hi_drv_disp_fmt format;
    hi_drv_disp_timing custom_timing;

    hi_drv_disp_vrr vrr_cfg;
    hi_drv_disp_qms qms_cfg;
    hi_drv_disp_qft qft_cfg;
    hi_bool allm_en;
    hi_drv_disp_color bg_color;
    hi_u32 alpha;

    hi_drv_disp_hdr_priority_mode hdr_priority_mode;
    hi_drv_disp_out_type disp_type;
    hi_drv_disp_output_color_space color_space_mode;

    hi_drv_pixel_bitwidth data_width;
    hi_drv_disp_pix_format pixel_format;

    hi_drv_disp_match_content_mode match_mode;
    hi_drv_disp_sink_capability sink_capability;
    hi_drv_disp_output_status output_status;
    hi_bool qms_enable;
    hi_u32 factor;
    disp_intf_link intf_status;
} disp_setting;

typedef struct {
    hi_drv_pixel_bitwidth debug_data_width;
    hi_drv_disp_pix_format debug_pixel_format;
} disp_proc_debug;

typedef enum {
    DISP_OUTPUT_INTF_TYPE = 0,
    DISP_OUTPUT_PANEL_TYPE,
    DISP_OUTPUT_BUTT
} disp_output_connect_type;

typedef struct {
    hi_drv_display disp;
    hi_bool open;

    disp_setting setting;
    hi_bool disp_setting_change;
    disp_output_connect_type out_connect_type;

    hi_disp_display_info disp_info;
    disp_proc_debug disp_debug;
} disp_channel;


hi_s32 drv_disp_init(hi_void);
hi_s32 drv_disp_de_init(hi_void);
hi_drv_display *drv_disp_get_channel_id(hi_drv_display disp);

hi_s32 drv_display_suspend(hi_void);
hi_s32 drv_display_resume(hi_void);
hi_bool drv_disp_is_licensed(hi_drv_display disp);

hi_s32 drv_disp_attach_src(hi_drv_display master, hi_drv_display slave);
hi_s32 drv_disp_detach_src(hi_drv_display master, hi_drv_display slave);

hi_s32 drv_disp_set_format(hi_drv_display disp, hi_drv_disp_stereo_mode stereo, hi_drv_disp_fmt fmt);
hi_s32 drv_disp_get_format(hi_drv_display disp, hi_drv_disp_stereo_mode *stereo, hi_drv_disp_fmt *fmt);
hi_void drv_disp_dfx_force_format(hi_drv_display disp, hi_drv_disp_stereo_mode stereo, hi_drv_disp_fmt fmt);

hi_s32 drv_disp_set_custom_timing(hi_drv_display disp, hi_drv_disp_format_param *format_param);
hi_s32 drv_disp_set_timing(hi_drv_display disp, hi_drv_disp_format_param *format_param);
hi_s32 drv_disp_get_timing(hi_drv_display disp, hi_drv_disp_format_param *format_param);

hi_s32 drv_disp_get_fmt_timing(hi_drv_disp_fmt fmt, hi_drv_disp_timing *timing);

hi_s32 drv_disp_add_intf(hi_drv_display disp, hi_drv_disp_intf intf);
hi_s32 drv_disp_del_intf(hi_drv_display disp, hi_drv_disp_intf intf);
hi_s32 drv_disp_get_intf_status(hi_drv_display disp, disp_get_intf *intf);


hi_s32 drv_disp_open(hi_drv_display disp);
hi_s32 drv_disp_close(hi_drv_display disp);

hi_s32 drv_disp_set_enable(hi_drv_display disp, hi_bool enable);
hi_s32 drv_disp_get_enable(hi_drv_display disp, hi_bool *enable);

hi_s32 drv_disp_set_right_eye_first(hi_drv_display disp, hi_bool enable);
hi_s32 drv_disp_set_virt_screen(hi_drv_display disp, hi_rect *vir_screen);
hi_s32 drv_disp_get_virt_screen(hi_drv_display disp, hi_rect *vir_screen);

hi_s32 drv_disp_set_screen_offset(hi_drv_display disp, hi_drv_disp_offset *screen_offset);
hi_s32 drv_disp_get_screen_offset(hi_drv_display disp, hi_drv_disp_offset *screen_offset);

hi_s32 drv_disp_set_bgcolor(hi_drv_display disp, hi_drv_disp_color *bg_color);
hi_s32 drv_disp_get_bgcolor(hi_drv_display disp, hi_drv_disp_color *bg_color);


// set aspect ratio: 0 and 0 means auto
hi_s32 drv_disp_set_aspect_ratio(hi_drv_display disp,
                                 hi_drv_disp_aspect_mode drv_aspect_mode,
                                 hi_u32 ratio_h,
                                 hi_u32 ratio_v);
hi_s32 drv_disp_get_aspect_ratio(hi_drv_display disp,
                                 hi_drv_disp_aspect_mode *drv_aspect_mode,
                                 hi_u32 *ratio_h,
                                 hi_u32 *ratio_v);

hi_s32 drv_disp_set_layer_zorder(hi_drv_display disp, hi_drv_disp_zorder layer, hi_drv_disp_zorder z_flag);
hi_s32 drv_disp_get_layer_zorder(hi_drv_display disp, hi_drv_disp_zorder layer, hi_u32 *zorder);

// miracast
hi_s32 drv_disp_create_cast(hi_drv_display disp, hi_drv_disp_cast_cfg *cfg, hi_handle *hcast);
hi_s32 drv_disp_destroy_cast(hi_handle h_cast);
hi_s32 drv_disp_set_cast_enable(hi_handle h_cast, hi_bool enable);
hi_s32 drv_disp_get_cast_enable(hi_handle h_cast, hi_bool *enable);

hi_s32 drv_disp_acquire_cast_frame(hi_handle h_cast, hi_drv_video_frame *cast_frame);
hi_s32 drv_disp_release_cast_frame(hi_handle h_cast, hi_drv_video_frame *cast_frame);
hi_s32 drv_disp_external_attach(hi_handle h_cast, hi_handle hsink);
hi_s32 drv_disp_external_de_attach(hi_handle h_cast, hi_handle hsink);

hi_s32 drv_disp_set_cast_attr(hi_handle h_cast, hi_drv_disp_cast_attr *cast_attr);
hi_s32 drv_disp_get_cast_attr(hi_handle h_cast, hi_drv_disp_cast_attr *cast_attr);
hi_s32 drv_disp_get_cast_handle(hi_drv_display disp, hi_handle *hcast, hi_void **cast_ptr);

// snapshot
hi_s32 drv_disp_acquire_snapshot(hi_drv_display disp, hi_drv_video_frame *snap_shot_frame,
                             hi_void **snapshot_handle_out);
hi_s32 drv_disp_release_snapshot(hi_drv_display disp, hi_drv_video_frame *snap_shot_frame, hi_void *snapshot_handle);
hi_s32 drv_disp_destroy_snapshot(hi_void *h_snapshot);

// Macrovision
hi_s32 drv_disp_set_macrovision(hi_drv_display disp, hi_drv_disp_macrovision mode);
hi_s32 drv_disp_get_macrovision(hi_drv_display disp, hi_drv_disp_macrovision *mode);

// cgms-a
hi_s32 drv_disp_set_cgms(hi_drv_display disp, hi_drv_disp_cgmsa_cfg *cfg);

// vbi
hi_s32 drv_disp_create_vbichannel(hi_drv_display disp, hi_drv_disp_vbi_cfg *cfg, hi_handle *hvbi);
hi_s32 drv_disp_destroy_vbichannel(hi_handle hvbi);
hi_s32 drv_disp_send_vbi_data(hi_handle hvbi, hi_drv_disp_vbi_data *vbi_data);
hi_s32 drv_disp_set_wss(hi_handle hvbi, hi_drv_disp_wss_data *wss_data);

hi_s32 drv_disp_set_output(hi_drv_display disp, const hi_drv_disp_out_type  drv_disp_type);
hi_s32 drv_disp_get_output(hi_drv_display disp, hi_drv_disp_out_type *drv_disp_type);
hi_s32 drv_disp_set_techni_bright(hi_drv_display disp, hi_s32 techni_drv_disp_bright);
hi_s32 drv_disp_get_techni_bright(hi_drv_display disp, hi_s32 *techni_drv_disp_bright);

hi_s32 drv_disp_set_output_color_space(hi_drv_display disp, hi_drv_disp_output_color_space color_space);
hi_s32 drv_disp_get_output_color_space(hi_drv_display disp, hi_drv_disp_output_color_space *color_space);

hi_s32 drv_disp_set_xdr_engine_type(hi_drv_display disp, hi_drv_disp_xdr_engine xdr_engine);
hi_s32 drv_disp_get_xdr_engine_type(hi_drv_display disp, hi_drv_disp_xdr_engine *xdr_engine);

hi_s32 drv_disp_get_alpha(hi_drv_display disp,  hi_u32 *alpha);
hi_s32 drv_disp_set_alpha(hi_drv_display disp, hi_u32 alpha);

hi_s32 drv_disp_set_sink_capability(hi_drv_display disp, hi_drv_disp_sink_capability *sink_cap);
hi_s32 drv_disp_get_sink_capability(hi_drv_display disp, hi_drv_disp_sink_capability *sink_cap);
hi_s32 drv_disp_get_capability(hi_drv_display disp, hi_drv_disp_capability *disp_cap);

hi_s32 drv_disp_set_match_content_mode(hi_drv_display disp, hi_drv_disp_match_content_mode match_content_mode);
hi_s32 drv_disp_get_match_content_mode(hi_drv_display disp, hi_drv_disp_match_content_mode *match_content_mode);

hi_s32 drv_disp_get_output_status(hi_drv_display disp, hi_drv_disp_output_status *drv_disp_output_status);

hi_s32 drv_disp_set_stop_hdmiinfo(hi_drv_display disp, hi_bool stop_send_hdmi_info);

hi_s32 drv_disp_set_intf_enable(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool enable);
hi_s32 drv_disp_get_intf_enable(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool *enable);

hi_s32 drv_disp_get_fmt_from_vic(hi_u32 vic, hi_drv_disp_fmt *fmt);
hi_s32 drv_disp_set_vrr(hi_drv_display disp, hi_drv_disp_vrr *disp_vrr);
hi_s32 drv_disp_get_vrr(hi_drv_display disp, hi_drv_disp_vrr *disp_vrr);
hi_s32 drv_disp_set_qms(hi_drv_display disp, hi_bool enable);
hi_s32 drv_disp_get_qms(hi_drv_display disp, hi_bool *enable);
hi_s32 drv_disp_set_allm(hi_drv_display disp, hi_bool enable);
hi_s32 drv_disp_get_allm(hi_drv_display disp, hi_bool *enable);
hi_s32 drv_disp_set_qft(hi_drv_display disp, hi_u32 factor);
hi_s32 drv_disp_get_qft(hi_drv_display disp, hi_u32 *factor);
hi_s32 drv_disp_set_color_space(hi_drv_display disp, hi_drv_disp_output_color_space color_space);
hi_s32 drv_disp_get_color_space(hi_drv_display disp, hi_drv_disp_output_color_space *color_space);
hi_s32 drv_disp_check_intf_validate(hi_drv_display disp, hi_drv_disp_format_param *format_param);

/*****************************************************/
// internal state
hi_s32  drv_disp_get_init_flag(hi_bool *inited);

hi_s32 drv_disp_get_display_info(hi_drv_display disp, hi_disp_display_info *info);

hi_s32 drv_disp_update_display_info(hi_drv_display disp,
                                    hi_drv_disp_out_type drv_disp_type,
                                    hi_drv_disp_output_color_space color_space_mode);
hi_void drv_disp_produce_display_info(disp_setting *disp_setting, hi_disp_display_info *info,
                                      hi_drv_disp_out_type drv_disp_type,
                                      hi_drv_disp_output_color_space color_space_mode);

hi_s32 drv_disp_get_current_timing_status(hi_drv_display disp, hi_drv_disp_timing_status *current_timing_status);

hi_s32 drv_disp_get_display_setting(hi_drv_display disp, disp_setting *setting);

hi_s32 drv_disp_set_bit_width(hi_drv_display disp, hi_drv_pixel_bitwidth data_width);
hi_s32 drv_disp_set_pixel_format(hi_drv_display disp, hi_drv_disp_pix_format pixel_format);
hi_s32 drv_disp_get_hdmi_intf_info(hi_drv_display disp,hi_drv_disp_intf_info *intf_info);

#ifndef __DISP_PLATFORM_BOOT__

#define  CAST_BUFFER_NUM  20
typedef struct tag_disp_cast_proc_info {
    hi_bool     enable;
    hi_bool     low_delay;
    hi_bool     user_allocate;
    hi_bool     attached;

    hi_u32      out_resolution_width;
    hi_u32      out_resolution_height;
    hi_u32      cast_out_frame_rate;

    hi_u32      total_buf_num;
    hi_u32      buf_size;
    hi_u32      buf_stride;
    hi_u32      cast_acquire_try_cnt;
    hi_u32      cast_acquire_ok_cnt;
    hi_u32      cast_release_try_cnt;
    hi_u32      cast_release_ok_cnt;

    hi_u32      cast_intr_cnt;

    hi_u32      cast_empty_buffer_num;
    hi_u32      cast_full_buffer_num;
    /* be written by wbc. */
    hi_u32      cast_write_buffer_num;
    /* be used by user, not returned back. */
    hi_u32      cast_used_buffer_num;
    hi_u32     state[CAST_BUFFER_NUM];
    hi_u32       frame_index[CAST_BUFFER_NUM];
} disp_cast_proc_info;


typedef struct tag_disp_proc_info {
    hi_bool enable;
    hi_bool master;
    hi_bool slave;
    hi_bool licensed;
    hi_drv_display attached_disp;

    // about encoding format
    hi_drv_disp_stereo_mode stereo;
    hi_rect virtaul_screen;
    hi_drv_disp_offset offset_info;
    hi_drv_disp_offset actual_offset_info;
    hi_bool right_eye_first;
    hi_drv_disp_fmt fmt;

    hi_drv_disp_vrr vrr_cfg;
    hi_bool qms_enable;
    hi_u32 qft_factor;
    hi_bool allm_en;

    hi_bool cust_aspect_ratio;
    hi_u32 ar_w;
    hi_u32 ar_h;
    hi_drv_disp_aspect_mode aspect_mode;

    hi_u32 bright;
    hi_u32 hue;
    hi_u32 satur;
    hi_u32 contrst;
    hi_u32 alpha;

    hi_drv_disp_layer layer[HI_DRV_DISP_LAYER_BUTT]; /* Z-order is from bottom to top */
    hi_drv_color_descript disp_color_space;

    /* about macrovision and cgms */
    hi_drv_disp_macrovision mcrvsn;
    hi_drv_disp_cgmsa_cfg cgms_cfg;

    hi_drv_disp_color bg_color;

    // interface
    hi_u32 intf_number;
    hi_drv_disp_intf intf[HI_DRV_DISP_INTF_ID_MAX];
    hi_u32 link[HI_DRV_DISP_INTF_ID_MAX];


    hi_drv_disp_timing timing;
    hi_u32 underflow;
    hi_u32 smmu_err_cnt;

    /*********HDR Pro info**********/
    /* Disp output tyep */
    hi_drv_disp_match_content_mode match_mode;
    hi_drv_disp_out_type  disp_type;
    hi_drv_disp_output_color_space color_space_mode;
    hi_drv_disp_out_type actual_output_type;
    hi_drv_disp_output_color_space actual_output_colorspace;
    hi_drv_color_descript color_space;

    hi_bool  boot_args_shutdown_cvbs;
    hi_bool  boot_args_shutdown_ypbpr;

    /* color space set by user. */
    hi_drv_disp_output_color_space user_set_color_space;
    hi_drv_disp_sink_capability sink_cap;
    hi_drv_disp_hdr_priority_mode hdr_priority_mode;

    hi_drv_pixel_bitwidth data_width;
    hi_drv_disp_pix_format pixel_format;
} disp_proc_info;

hi_s32 drv_disp_get_proc_into(hi_drv_display disp, disp_proc_info *info);

hi_s32 drv_disp_get_xdr_engine_list(hi_drv_hdr_type frm_type,
                                hi_drv_disp_out_type    out_type,
                                hi_drv_disp_xdr_engine     *pen_xdr_engine_list);


hi_s32 drv_disp_set_hdr_priority_mode(hi_drv_display disp_channel,
                                  hi_drv_disp_hdr_priority_mode hdr_priority_mode);


hi_s32 drv_isp_get_hdr_priority_mode(hi_drv_display disp_channel,
                                  hi_drv_disp_hdr_priority_mode *hdr_priority_mode);

hi_s32 drv_disp_set_color_bar(hi_drv_display disp, hi_bool enable);
hi_s32 drv_disp_proc_set_intf_enable(hi_drv_display disp,
                                     disp_intf *intf_attr,
                                     hi_bool enable);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_DISPLAY_H__ */




