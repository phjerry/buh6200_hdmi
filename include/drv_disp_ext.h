/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: define for external use
* Author: vdp
* Create: 2019-06-28
 */

#ifndef __DRV_DISP_EXT_H__
#define __DRV_DISP_EXT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_dev.h"

#define GFX_WIN_RECT_SYNC_NUM 16

typedef struct {
    hi_rect video_rect;
    hi_rect output_rect;
    hi_bool sync_enable;
    hi_u32 handle;
} hi_drv_gfx_win_rect_sync_info;

typedef struct {
    hi_drv_gfx_win_rect_sync_info rect_sync_info[GFX_WIN_RECT_SYNC_NUM];
} hi_drv_gfx_win_sync_info;

typedef struct {
    hi_u32 delay_cycle_times;
} hi_drv_disp_gfx_sync_info_delay;

#define MODE_STATIC_TIMING  1 << 0
#define MODE_STATIC_HDR     1 << 1
#define MODE_DYNAMIC_TIMING 1 << 2

typedef struct {
    hi_u32 vic;
    hi_u32 pixel_clock;
    hi_bool progressive;

    hi_u32 h_active;
    hi_u32 h_total;
    hi_u32 h_blank;
    hi_u32 h_front;
    hi_u32 h_sync;
    hi_u32 h_back;
    hi_bool h_pol;

    hi_u32 v_active;
    hi_u32 v_total;
    hi_u32 v_blank;
    hi_u32 v_front;
    hi_u32 v_sync;
    hi_u32 v_back;
    hi_bool v_pol;

    hi_u32 mode_3d;
} disp_intf_timing;

typedef struct {
    hi_bool vrr_en;
    hi_bool qms_en;
    hi_bool fva_factor;
    hi_u32 base_vfront;
    hi_u32 base_refresh_rate;
} disp_intf_dynamic_timing;

typedef struct {
    hi_u8 scan_info;               /* AVI Byte1 S [1:0],Scan information */
    hi_bool active_aspect_present; /* AVI Byte1 A[0], Active Format Information Present */
    hi_u8 bar_present;             /* AVI Byte1 B [1:0],Bar Data Present */
    hi_u32 picture_aspect_ratio;   /* AVI Byte2 M[1:0],Picture Aspect Ratio */
    hi_u32 active_aspect_ratio;    /* AVI Byte2 R[3:0],Active Portion Aspect Ratio.support in both DVB & ATSC mode */
    hi_u32 colormetry;             /* AVI Byte2 C[1:0],Colorimetry ;Byte3 EC[2:0]Extended Colorimetry */
    hi_u8 picture_scal;            /* AVI Byte3 SC[1:0],Non-Uniform Picture Scaling */
    hi_u8 rgb_quantization;        /* AVI Byte3 Q[1:0] ,RGB Quantization Range */
    hi_bool it_content_valid;      /* AVI Byte3 ITC,IT content */
    hi_u8 it_content_type;         /* AVI Byte5 CN[1:0],IT content type */
    hi_u8 ycc_quantization;        /* AVI Byte5 YQ[1:0],YCC Quantization Range */
    hi_u8 pixel_repet;             /* AVI Byte5 PR[3:0],Pixel Repetition Factor */
    hi_u16 top_bar;                /* AVI Byte7-6 ETB15-ETB00 (Line Number of End of Top Bar) */
    hi_u16 bottom_bar;             /* AVI Byte9-8 SBB15-SBB00 (Line Number of Start of Bottom Bar) */
    hi_u16 left_bar;               /* AVI Byte11-10 ELB15-ELB08 (Pixel Number of End of Left Bar) */
    hi_u16 right_bar;              /* AVI Byte13-12 SRB07-SRB00 (Pixel Number of Start of Right Bar) */
    hi_u8 mode_3d;                 /* VS  PB5[7:4],3D_Structure */
} disp_intf_avi_info;

typedef struct {
    hi_drv_color_descript color_space;  /* AVI Byte1 Y[2:0], RGB  or YCBCR */
    hi_drv_disp_pix_format pixel_format;
    hi_drv_pixel_bitwidth data_width;  /*  data depth */
} hi_drv_disp_data_info;

typedef struct {
    hi_drv_disp_out_type out_type; /* sdr hdr10 hlg dolbyvision */
    hi_drv_disp_timing_cfg disp_timing;
    hi_drv_disp_data_info in_info;
    hi_drv_disp_data_info out_info;
} hi_drv_disp_intf_info;

typedef enum {
    HI_DRV_GFX_EVENT_DATA_UPDATE = 0,
    HI_DRV_GFX_EVENT_BUTT
} hi_drv_gfx_event;

typedef enum {
    HI_DRV_DISP_SCAN_MODE_NONE,
    HI_DRV_DISP_SCAN_MODE_OVERSCAN,
    HI_DRV_DISP_SCAN_MODE_UNDERSCAN,
    HI_DRV_DISP_SCAN_MODE_RESERVED,
}hi_drv_disp_scan_mode;

/* non-uniform picture scaling */
typedef enum {
    HI_DRV_DISP_NUPS_UNKNOWN,
    HI_DRV_DISP_NUPS_HORIZONTAL,
    HI_DRV_DISP_NUPS_VERTICAL,
    HI_DRV_DISP_NUPS_BOTH,
}hi_drv_disp_non_uniform;

typedef enum {
    HI_DRV_DISP_CONTENT_TYPE_GRAPHICS,
    HI_DRV_DISP_CONTENT_TYPE_PHOTO,
    HI_DRV_DISP_CONTENT_TYPE_CINEMA,
    HI_DRV_DISP_CONTENT_TYPE_GAME,
    HI_DRV_DISP_CONTENT_TYPE_NONE,
}hi_drv_disp_content_type;

/* avi info frame for HDMI */
typedef struct {
    hi_u32  vic;
    hi_u32  pixel_repeat;

    hi_drv_disp_pix_format pix_format;     /* hdmi out color space info */
    hi_drv_disp_scan_mode scan_mode;       /* default: none */
    hi_drv_color_descript color_space;        /* hdmi in color space info */
    hi_drv_disp_non_uniform nups;          /* default: unknow */
    hi_drv_disp_content_type content_type; /* default: HI_DRV_DISP_CONTENT_TYPE_NONE */
    hi_drv_aspect_ratio picture_aspect;
    hi_drv_video_afd_type active_aspect;   /* default: HI_DRV_AFD_TYPE_SAME_AS_SRC */

    hi_u32  top_bar;
    hi_u32  bottom_bar;
    hi_u32  left_bar;
    hi_u32  right_bar;
    hi_drv_disp_stereo_mode disp_3d_mode;
}hi_drv_disp_avi_infoframe;

/* HDMI MIPI */
typedef hi_s32 (*fn_intf_suspend)(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state);
typedef hi_s32 (*fn_intf_resume)(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state);

typedef hi_s32 (*fn_intf_check_validate)(hi_drv_disp_intf intf, hi_u32 set_mode, hi_drv_disp_intf_info *intf_info);
typedef hi_s32 (*fn_intf_enable)(hi_drv_disp_intf intf, hi_bool enable);

typedef hi_s32 (*fn_intf_detach)(hi_drv_disp_intf intf, void *data);
typedef hi_s32 (*fn_intf_attach)(hi_drv_disp_intf intf, void *data);
typedef hi_s32 (*fn_intf_prepare)(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info);

typedef hi_s32 (*fn_intf_config)(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info);
typedef hi_s32 (*fn_intf_atomic_config)(hi_drv_disp_intf intf, hi_u32 mode,
                                        hi_drv_disp_intf_info *intf_info);
typedef hi_s32 (*fn_intf_avi_infoframe_pack)(hi_drv_disp_avi_infoframe *frame,
                                             hi_u8 *buffer, hi_u32 size);
typedef hi_s32 (*fn_intf_hdr10_infoframe_pack)(hi_drv_hdr_static_metadata *frame,
                                               hi_u8 *buffer, hi_u32 size);
typedef hi_s32 (*fn_intf_hlg_infoframe_pack)(hi_drv_hdr_hlg_metadata *frame,
                                             hi_u8 *buffer, hi_u32 size);

/* VENC */
typedef hi_s32 (*fn_cast_release_frame)(hi_handle cast, hi_drv_video_frame *cast_frame);
typedef hi_s32 (*fn_cast_detach)(hi_handle cast, hi_handle sink);
typedef hi_s32 (*fn_cast_set_attr)(hi_handle cast, hi_u32 width, hi_u32 height, hi_u32 frame_rate);

/* GFX */
typedef hi_s32 (*fn_set_gfx_win_rect_sync_delay)(hi_drv_display disp,
                                                 hi_drv_disp_gfx_sync_info_delay sync_info);
typedef hi_s32 (*fn_set_gfx_update_event)(hi_drv_display disp, hi_drv_gfx_event gfx_event);
typedef hi_s32 (*fn_set_gfx_win_sync)(hi_drv_display disp,
                                           hi_drv_gfx_win_sync_info *rect_sync_info);

typedef hi_s32 (*fn_get_displayinfo)(hi_drv_display disp, hi_disp_display_info *disp_Info);
typedef hi_s32 (*fn_register_call_back)(hi_drv_display disp, hi_drv_disp_callback_type type,
                                hi_drv_disp_callback *callback);
typedef hi_s32 (*fn_unregister_call_back)(hi_drv_display disp, hi_drv_disp_callback_type type,
                                hi_drv_disp_callback *callback);

/* MCE */
typedef hi_s32 (*fn_disp_init)(hi_void);
typedef hi_s32 (*fn_disp_deinit)(hi_void);
typedef hi_s32 (*fn_disp_open)(hi_drv_display disp);
typedef hi_s32 (*fn_disp_close)(hi_drv_display disp);

/* PM */
typedef hi_s32 (*fn_disp_suspend)(hi_void *pdev, hi_void *state);
typedef hi_s32 (*fn_disp_resume)(hi_void *pdev);

typedef struct {
    fn_intf_suspend intf_suspend;
    fn_intf_resume intf_resume;
    fn_intf_check_validate intf_check_validate;
    fn_intf_enable intf_enable;
    fn_intf_detach intf_detach;
    fn_intf_attach intf_attach;
    fn_intf_prepare intf_prepare;
    fn_intf_config intf_config;
    fn_intf_atomic_config intf_atomic_config;
    fn_intf_avi_infoframe_pack intf_avi_infoframe_pack;
    fn_intf_hdr10_infoframe_pack intf_hdr10_infoframe_pack;
    fn_intf_hlg_infoframe_pack intf_hlg_infoframe_pack;
} disp_intf_func;

typedef struct {
    fn_disp_init disp_init;     /* mce */
    fn_disp_deinit disp_deinit; /* mce */
    fn_disp_open disp_open;     /* mce */
    fn_disp_close disp_close;   /* mce */

    fn_cast_release_frame disp_cast_release_frame; /* VENC */
    fn_cast_set_attr disp_cast_set_attr;           /* VENC */
    fn_cast_detach disp_cast_detach;               /* VENC */

    fn_get_displayinfo disp_get_displayinfo; /* FB */
    fn_register_call_back disp_register_call_back;     /* FB */
    fn_unregister_call_back disp_unregister_call_back; /* FB */

    fn_disp_suspend disp_suspend; /* PM */
    fn_disp_resume disp_resume;   /* PM */

} disp_export_func;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_DISP_EXT_H__ */

