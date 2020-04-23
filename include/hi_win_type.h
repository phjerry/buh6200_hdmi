/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __HI_WIN_TYPE_H__
#define __HI_WIN_TYPE_H__

#include "hi_drv_disp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef hi_advca_function_release
#define hi_fatal_win(fmt...) HI_FATAL_PRINT(HI_ID_WIN, fmt)
#define hi_err_win(fmt...)   HI_ERR_PRINT(HI_ID_WIN, fmt)
#define hi_warn_win(fmt...)  HI_WARN_PRINT(HI_ID_WIN, fmt)
#define hi_info_win(fmt...)  HI_INFO_PRINT(HI_ID_WIN, fmt)
#else
#define hi_fatal_win(fmt...)
#define hi_err_win(fmt...)
#define hi_warn_win(fmt...)
#define hi_info_win(fmt...)
#endif

#define DEF_MAX_WIN_NUM_ON_SINGLE_DISP  17
#define DEF_MAX_WIN_NUM_ON_VIRTUAL_DISP 17

#define DEF_MAX_WIN_NEED_EDID_SIZE 256

#define WIN_HDR_CMD_ARGS_SIZE 128

/* window type */
typedef enum {
    HI_DRV_DISP_WIN = 0,
    HI_DRV_VITUAL_WIN,
    HI_DRV_AI_WIN,
    HI_DRV_WIN_MAX
} hi_drv_win_type;

typedef enum {
    HI_DRV_WIN_FREEZE_DISABLE = 0,
    HI_DRV_WIN_FREEZE_LAST,
    HI_DRV_WIN_FREEZE_BLACK,
    HI_DRV_WIN_FREEZE_MAX
} hi_drv_win_freeze_mode;

typedef enum {
    HI_DRV_WIN_RESET_LAST = 0,
    HI_DRV_WIN_RESET_BLACK,
    HI_DRV_WIN_RESET_MAX
} hi_drv_win_reset_mode;

typedef enum {
    WIN_FRAME_FORCE_CLOSE = 0,
    WIN_FRAME_FORCE_PROG,
    WIN_FRAME_FORCE_INTERLEAVE,
    WIN_FRAME_FORCE_MAX
} hi_drv_win_forceframeprog_flag;

typedef enum {
    WIN_FIELDMODE_AUTO = 0,
    WIN_FIELDMODE_TOP,
    WIN_FIELDMODE_BOTTOM,
    WIN_FIELDMODE_ALL,
    WIN_FIELDMODE_MAX
} hi_drv_win_fieldmode;

typedef enum {
    WIN_FIELDORDER_TOPFIRST = 0,
    WIN_FIELDORDER_BOTTOMFIRST,
    WIN_FIELDORDER_UNKOWN,
    WIN_FIELDORDER_MAX
} hi_drv_win_fieldorder;

typedef enum {
    WIN_TUNNEL_CTRL_AUTO = 0,
    WIN_TUNNEL_CTRL_ON = 0x12345678,
    WIN_TUNNEL_CTRL_OFF,
    WIN_TUNNEL_CTRL_MAX
} hi_drv_win_tunnel_ctrl;

typedef enum {
    WIN_SRCWH_DISABLE = 0,
    WIN_SRCWH_ENABLE,
    WIN_SRCWH_MAX
} hi_drv_win_srcwh;

typedef enum {
    WIN_FRAME_SHUTDOWN_CTRL = 0,
    WIN_FRAME_SOFTALG_CLOSE,
    WIN_FRAME_SOFTALG_OPEN,
    WIN_FRAME_SOFTALG_BUTT
} hi_win_close_softalg_flag;

typedef enum {
    WIN_HW_CONFIG_CHECK_CLOSE = 0,
    WIN_HW_CONFIG_CHECK_OPEN,
    WIN_HW_CONFIG_CHECK_BUTT
} hi_win_hw_config_check_flag;

typedef enum {
    WIN_QUEUE_CTRL_NORMAL = 0,
    WIN_QUEUE_CTRL_STEP_BY_STEP,
    WIN_QUEUE_CTRL_BREAK,
    WIN_QUEUE_CTRL_BUTT
} hi_win_queue_ctrl_flag;

typedef enum {
    HI_DRV_WIN_PROGRESSIVE_INTERLEAVE_REVISE_CTRL,
    HI_DRV_WIN_SOFTALG_CTRL,
    HI_DRV_WIN_HW_CONFIG_CHECK,
    HI_DRV_WIN_QUEUE_CTRL,
    HI_DRV_WIN_TUNNEL_CTRL,
    HI_DRV_WIN_SRC_WH_CTRL,
    HI_DRV_WIN_SRC_FIELDMODE_CTRL,
    HI_DRV_WIN_SRC_FIELDORDER_CTRL,
    HI_DRV_WIN_ONOFFLINE_REG_CHECK,
    HI_DRV_WIN_PLAYCTRL_BUTT
} hi_drv_win_playctrl_type;

typedef struct {
    hi_drv_win_playctrl_type eplayctrltype;
    struct {
        hi_drv_win_forceframeprog_flag progrevise;
        hi_drv_win_fieldmode field_mode_revise;
        hi_drv_win_fieldorder field_order_revise;
        hi_drv_win_tunnel_ctrl tunnle_ctrl_enum;
        hi_drv_win_srcwh srcwh_revise;
        hi_win_close_softalg_flag soft_alg_ctrl;
        hi_win_hw_config_check_flag win_config_check;
        hi_win_queue_ctrl_flag queue_ctrl;
        hi_bool online_offline_state;
    } stplayctrlinfor;
    struct {
        hi_u32 src_width_user_cfg;
        hi_u32 src_height_user_cfg;
    } data;

} hi_drv_win_playctrl_operation_infor;

typedef struct {
    /* indicate whether sence has changed,
        true when receive first frm under follow first frm mode . */
    hi_bool sence_change;
    hi_drv_disp_out_type disp_outtype;
    hi_drv_color_space out_colorspace;
} win_first_frame_status;

/* window attribute */
typedef struct {
    hi_bool is_virtual;

    /* not change when window lives */
    hi_drv_display disp;

    /* may change when window lives */
    hi_drv_aspect_ratio aspect_ratio;
    hi_drv_asp_rat_mode aspect_ratio_mode;

    hi_bool use_crop_rect;
    hi_rect in_rect;
    hi_drv_crop_rect crop_rect;

    hi_rect out_rect;

    /* only for virtual window */
    hi_bool user_alloc_buffer;
    hi_u32 buf_number; /* [1,16] */
    hi_drv_pixel_format data_format;
    /* debug for hisi-self path:1->hisi-self hdr path,0->other hdr path */
    hi_bool hisi_hdr;
} hi_drv_win_attr;

/* window information */
typedef struct {
    hi_drv_win_type type;

    hi_handle prim_handle;
    hi_handle sec_handle;
} hi_drv_win_info;

typedef enum {
    HI_DRV_WINDOW_MODE_NORMAL = 0, /* normal win mode */
    HI_DRV_WINDOW_MODE_DOLBY,      /* dolby hdr win mode,will occupy two video layers. */
    HI_DRV_WINDOW_MODE_HDR,        /* hdr win mode is the same with dolby hdr win. */
    HI_DRV_WINDOW_MODE_BUTT
} hi_drv_window_mode;

typedef enum {
    HI_DRV_WIN_FRAME_TYPE_CONTINUOUS = 0, /* frame continuous */
    HI_DRV_WIN_FRAME_TYPE_DROPPED,        /* frame dropped */

    HI_DRV_WIN_FRAME_TYPE_BUTT
} hi_drv_win_frame_type;

typedef struct {
    hi_u32 phy_addr;

    hi_u32 comp_len;
    hi_u32 dmcfg_len;
    hi_u32 dmreg_len;
    hi_u32 hdr10infofrm_len;
    hi_u32 upmd_len;
    hi_u32 total_len;  // buffer length to store these 4 structures,currently is 64kb.

} hi_drv_dolby_meta_info;

/* source information.
   window will get / release frame or send private info to sourec
   by function pointer */
typedef struct {
    hi_handle src_handle;
    hi_handle second_src_handle;
} hi_drv_win_src_info;

/* source information.
   window will get / release frame or send private info to sourec
   by function pointer */
typedef struct {
    hi_handle src;
} hi_drv_win_src_handle;

/* window current play information, player gets it and adjust audio and video
   play rate */
typedef struct {
    hi_u32 delay_time;                        /* if q one frame delay time to display */
    hi_u32 out_put_frame_rate;                /* disp out put frame rate */
    hi_u32 remain_frame_num;
    hi_drv_video_frame current_play_frame; /* current frame */
    hi_u32 underload_times;
    hi_s64 disp_frame_pts;
    hi_u32 disp_frame_disp_time;
} hi_drv_win_play_info;

typedef struct {
    hi_drv_win_frame_type frame_type;        /* hi_drv_win_frame_type_continuous :
        continue frame; hi_drv_win_frame_type_dropped :  discontinue frame */
    hi_u32 speed_integer; /* *<integral part of tplay speed */ /* *<cncomment: 倍数的整数部分 */
    hi_u32 speed_decimal; /* *<fractional part (calculated to three decimal places)
        of tplay speed */ /* *<cncomment: 倍数的小数部分，保留3位小数 */
} hi_drv_win_play_ctrl;

typedef struct {
    hi_void *pfacqframe;
    hi_void *pfrlsframe;
    hi_void *pfsetwinattr;

    hi_u32 resrve0;
    hi_u32 resrve1;
} hi_drv_win_intf;

#define WIN_BUILD_TIME_SIZE 128
#define WIN_VERSION_STR_SIZE 128
#define WIN_LIB_NAME_STR_SIZE 32
#define WIN_LIB_ID_SIZE 4

typedef struct {
    hi_char build_time[WIN_BUILD_TIME_SIZE];
    hi_char version[WIN_VERSION_STR_SIZE];
    hi_char user_name[WIN_LIB_NAME_STR_SIZE];
    hi_u32 user_id[WIN_LIB_ID_SIZE];
    hi_u32 valid;
} win_dolby_lib_info;

typedef struct {
    hi_char build_time[WIN_BUILD_TIME_SIZE];
    hi_char version[WIN_VERSION_STR_SIZE];
    hi_u32 valid;
} win_technicolor_lib_info;

/* commands for debug. */

typedef enum {
    WIN_DEBUG_SET_HISI_HDR_PATH = 0x0,
    WIN_DEBUG_CMD_BUTT
} win_debug_cmd;

typedef struct {
    hi_handle win_handle;
    win_debug_cmd debug_cmd;
    hi_u8 args[WIN_HDR_CMD_ARGS_SIZE];
} win_debug_cmd_and_args;

typedef struct {
    hi_u32 speed_integer;
    hi_u32 speed_decimal;
} hi_drv_win_tplay_opt;

typedef enum {
    HI_DRV_WIN_SYNC_PLAY = 0,
    /* *<normal disable. */ /* *<cncomment: 正常播放 */
    HI_DRV_WIN_SYNC_DISCARD = 1,
    /* *<discard one frame. */ /* *<cncomment: 丢掉一帧 */
    HI_DRV_WIN_SYNC_REPEAT = 2,
    /* *<repeat one frame. */ /* *<cncomment: 重复一帧 */
    HI_DRV_WIN_SYNC_PLAY_LASTEST = 3,
    /* *<play latest frame. */ /* *<cncomment: 播放buffer里面的最新帧 */
    HI_DRV_WIN_SYNC_PLAY_LAST = 4,
    /* *<play last frame. */ /* *<cncomment: 一直重复上一帧 */
    HI_DRV_WIN_SYNC_BUTT
} hi_drv_win_sync;

typedef enum {
    HI_DRV_WIN_ATTR_TYPE_OUT_RECT = 0x0,
    HI_DRV_WIN_ATTR_TYPE_ASPECT,
    HI_DRV_WIN_ATTR_TYPE_IN_RECT,
    HI_DRV_WIN_ATTR_TYPE_ZORDER,
    HI_DRV_WIN_ATTR_TYPE_BUTT
} hi_drv_win_attr_type;

typedef struct {
    hi_bool use_crop_rect;
    hi_drv_crop_rect crop_rect;
    hi_rect input_rect;
} hi_drv_win_input_attr;

typedef struct {
    hi_drv_aspect_ratio custom_aspect;
    hi_drv_asp_rat_mode aspect_mode;
} hi_drv_win_aspect_attr;

typedef struct {
    hi_handle win;
    union {
        hi_drv_win_aspect_attr aspect;
        hi_drv_win_input_attr input;
        hi_rect output;
        hi_u32 z_order;
    } un_win_attr;
} hi_drv_win_multi_attr;

typedef struct {
    hi_u32 ai_config_reserve;
} hi_drv_win_ai_config;

typedef struct {
    hi_rect src_crop_rect;
    hi_drv_rot_angle rotation_angle;
    hi_drv_pixel_bitwidth pixel_bitwidth;
    hi_rect win_video_rect;
    hi_rect win_out_rect;
} hi_drv_win_out_config;

typedef struct {
    hi_handle   main_win;
    hi_handle   sub_win;
} hi_drv_win_handle;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __hi_drv_win_H__ */
