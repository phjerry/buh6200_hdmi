/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __DRV_WIN_IOCTL_H__
#define __DRV_WIN_IOCTL_H__

#include "hi_type.h"
#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_win.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_handle win_handle;
    hi_drv_win_attr win_attr;
    hi_bool is_virt_screen;
    hi_bool is_mce;
} win_create;

typedef struct {
    hi_handle win_handle;
    hi_bool mce;
} win_destroy;

typedef struct {
    hi_handle win_handle;
    hi_bool is_enable;
    hi_bool mce;
} win_enable;

typedef struct {
    hi_handle win_handle;
    hi_drv_win_src_handle src_info;
    hi_bool mce;
} win_source;

typedef struct {
    hi_handle win_handle;
    hi_drv_win_info priv_info;
} win_priv_info;

typedef struct {
    hi_handle win_handle;
    hi_drv_win_play_info play_info;
} win_play_info;

typedef struct {
    hi_handle win_handle;
    hi_drv_win_play_ctrl play_ctrl_info;
} win_play_ctrl;

typedef struct {
    hi_handle win_handle;
    hi_drv_disp_zorder zflag;
} win_zorder;

typedef struct {
    hi_handle win_handle;
    hi_u32 order;
} win_order;

typedef struct {
    hi_handle win_handle;
    hi_drv_win_freeze_mode freeze_mode;
} win_freeze;

typedef struct {
    hi_handle win_handle;
    hi_bool enable;
}win_quickout;

typedef struct {
    hi_handle win_handle;
    hi_drv_win_reset_mode reset_mode;
} win_reset;

typedef struct {
    hi_handle win_handle;
    hi_drv_video_frame frame;
    hi_drv_dolby_meta_info dolby_hdrcfg;
} win_frame;

typedef struct {
    hi_handle win_handle;
    hi_drv_hdr_type frmtype;
    hi_drv_disp_out_type disp_outtype;
    hi_drv_disp_xdr_engine xdr_engine;
} win_out_type_and_engine;

typedef struct {
    hi_handle win_handle;
    hi_u8 edid_data[DEF_MAX_WIN_NEED_EDID_SIZE];
    hi_drv_disp_hdr_priority_mode priority_mode;
} win_sink_info;

typedef struct {
    hi_handle win_handle;
    hi_drv_disp_out_type disp_out_type;
    hi_bool need_process_gfx;
} win_disp_out_type_and_gfx_state;

typedef struct {
    hi_handle win_handle;
    hi_s32  fence_fd;
    hi_bool enhance_valid;            /* dolby frame may has enhance frame(el). */
    hi_drv_video_frame base_frame;    /* sdr/hdr10/hlg/slf/bl frame. */
    hi_drv_video_frame enhance_frame; /* el frame. */
    hi_drv_dolby_meta_info dolby_info;
    hi_drv_disp_out_type disp_out_type;
    hi_drv_disp_xdr_engine xdr_engine;
    hi_u64 slhdr_psparamaddr;
} win_xdr_frame;

typedef struct {
    win_xdr_frame xdr_frame;
    hi_u32 fencefd;
} win_syncframe;

typedef struct {
    hi_handle win_handle;
    hi_u32 times;
} win_unload;

typedef struct {
    hi_u32 start_phyaddr;
    hi_u32 length;
} win_capture_driver_supply_addr;

typedef struct {
    hi_handle win;
    hi_drv_video_frame captured_pic;
    win_capture_driver_supply_addr driver_supply_addr;
} win_capture;

typedef struct {
    hi_handle win_handle;
    hi_bool enable;
} win_pause;

typedef struct {
    hi_handle win;
    hi_bool step;
} win_step_mode;

typedef struct {
    hi_handle win[HI_DRV_DISPLAY_BUTT][DEF_MAX_WIN_NUM_ON_SINGLE_DISP];
    hi_handle virtual_win[DEF_MAX_WIN_NUM_ON_VIRTUAL_DISP];
    hi_handle capture[HI_DRV_DISPLAY_BUTT][DEF_MAX_WIN_NUM_ON_SINGLE_DISP];

    hi_u64    thread_file_magic;
} win_state;

typedef struct {
    hi_handle win;
    hi_drv_rot_angle rotation;
} win_rotation;

typedef struct {
    hi_handle win;
    hi_bool flip_vert;
    hi_bool flip_hori;
} win_flip;

typedef struct {
    hi_handle win;
    hi_drv_window_mode win_mode;
} win_mode;

typedef struct {
    hi_handle win;
    hi_drv_video_buffer_pool buf_pool;
} win_buf_pool;

typedef struct {
    hi_handle win;
    hi_bool quick_enable;
} win_set_quick;

typedef struct {
    hi_drv_display disp;
    hi_drv_win_handle win_handle;
} win_get_handle;

typedef enum {
    ATTACH_TYPE_SRC = 0,
    ATTACH_TYPE_SINK,
    ATTACH_TYPE_BUTT
} win_attach_type;

typedef struct {
    win_attach_type type;
    hi_handle win;
    hi_handle mutual;
} win_attach;

typedef struct {
    hi_u32 techni_bright;
} win_techni_disp_bright;

typedef struct {
    hi_handle win;
    hi_drv_display disp;
    hi_u32 alpha;
} win_alpha;

typedef struct {
    hi_handle win;
    hi_drv_win_forceframeprog_flag force_frame_prog_flag;
} win_forceframe_flag;

typedef struct {
    hi_handle win_handle;
    hi_drv_win_sync sync_mode;
} win_sync;

typedef enum hiIOC_VO_E {
    IOC_WIN_CREATE = 0,
    IOC_WIN_DESTROY,

    IOC_WIN_SET_ENABLE,
    IOC_WIN_GET_ENABLE,

    IOC_WIN_SET_ATTR,
    IOC_WIN_GET_ATTR,

    IOC_WIN_GET_LATEST_FRAME,
    IOC_WIN_GET_HDR_TYPE,

    IOC_WIN_SET_SOURCE,
    IOC_WIN_GET_SOURCE,

    IOC_WIN_GET_INFO,
    IOC_WIN_GET_PLAY_INFO,

    IOC_WIN_QU_FRAME,
    IOC_WIN_QUDOLBY_FRAME,
    IOC_WIN_QUSYNC_FRAME,
    IOC_WIN_QU_ULSFRAME,
    IOC_WIN_DQ_FRAME,

    IOC_WIN_FREEZE,
    IOC_WIN_GET_FREEZE_STATUS,
    IOC_WIN_RESET,

    IOC_WIN_PAUSE,
    IOC_WIN_GET_PAUSE_STATUS,
    IOC_WIN_SET_QUICK,
    IOC_WIN_GET_QUICK,

    IOC_WIN_SET_ZORDER,
    IOC_WIN_GET_ORDER,

    IOC_WIN_CAP_FRAME,
    IOC_WIN_CAP_REL_FRAME,

    IOC_WIN_SEND_FRAME,

    IOC_WIN_STEP_MODE,
    IOC_WIN_STEP_PLAY,

    IOC_WIN_VIR_ACQUIRE,
    IOC_WIN_VIR_RELEASE,
    IOC_WIN_VIR_EXTERNBUF,

    IOC_WIN_SUSPEND,
    IOC_WIN_RESUME,

    IOC_WIN_ATTACH,
    IOC_WIN_DETACH,

    IOC_WIN_GET_LATESTFRAME_INFO,
    IOC_VO_WIN_CAPTURE_START,
    IOC_VO_WIN_CAPTURE_RELEASE,
    IOC_VO_WIN_CAPTURE_FREE,
    IOC_WIN_SET_ROTATION,
    IOC_WIN_GET_ROTATION,
    IOC_WIN_SET_FLIP,
    IOC_WIN_GET_FLIP,
    IOC_WIN_SET_MODE,
    IOC_WIN_GET_OUT_TYPE_AND_ENGINE,
    IOC_WIN_GET_DISP_OUT_TYPE_AND_GFX_STATE,
    IOC_WIN_GET_UNLOAD,
    IOC_WIN_DEBUG_GET_HANDLE,

    IOC_WIN_CLOSE_HDR_PATH,
    IOC_WIN_SET_DOLBY_LIB_STATUS,
    IOC_WIN_SET_TECHNI_LIB_STATUS,
    IOC_WIN_DEBUG,
    IOC_WIN_SET_FROST_MODE,
    IOC_WIN_GET_FROST_MODE,

    IOC_WIN_SET_ALPHA,
    IOC_WIN_GET_ALPHA,
    IOC_WIN_SET_FRAME_PROGINTERLEAVE,
    IOC_WIN_GET_FRAME_PROGINTERLEAVE,
    IOC_WIN_RESET_FIRST_FRAME_STATUS,

    IOC_WIN_GET_TECHNI_OUT_BRIGHT,
    IOC_WIN_DESTROY_MCE,

    IOC_WIN_SET_PLAY_CTRL,
    IOC_WIN_GET_PLAY_CTRL,
    IOC_WIN_GET_SINK_INFO,

    IOC_WIN_SET_SYNC,
    IOC_WIN_BUTT
} IOC_WIN_E;

#define CMD_WIN_CREATE      _IOWR(HI_ID_WIN, IOC_WIN_CREATE, win_create)
#define CMD_WIN_DESTROY     _IOW(HI_ID_WIN, IOC_WIN_DESTROY, hi_handle)
#define CMD_WIN_DESTROY_MCE _IOW(HI_ID_WIN, IOC_WIN_DESTROY_MCE, win_destroy)

#define CMD_WIN_SET_ENABLE _IOW(HI_ID_WIN, IOC_WIN_SET_ENABLE, win_enable)
#define CMD_WIN_GET_ENABLE _IOWR(HI_ID_WIN, IOC_WIN_GET_ENABLE, win_enable)

#define CMD_WIN_SET_ATTR _IOW(HI_ID_WIN, IOC_WIN_SET_ATTR, win_create)
#define CMD_WIN_GET_ATTR _IOWR(HI_ID_WIN, IOC_WIN_GET_ATTR, win_create)

#define CMD_WIN_GET_LATEST_FRAME _IOWR(HI_ID_WIN, IOC_WIN_GET_LATEST_FRAME, win_frame)

#define CMD_WIN_SET_SOURCE _IOW(HI_ID_WIN, IOC_WIN_SET_SOURCE, win_source)
#define CMD_WIN_GET_SOURCE _IOW(HI_ID_WIN, IOC_WIN_GET_SOURCE, win_source)

#define CMD_WIN_GET_INFO      _IOWR(HI_ID_WIN, IOC_WIN_GET_INFO, win_priv_info)
#define CMD_WIN_GET_PLAY_INFO _IOWR(HI_ID_WIN, IOC_WIN_GET_PLAY_INFO, win_play_info)

#define CMD_WIN_QU_FRAME      _IOWR(HI_ID_WIN, IOC_WIN_QU_FRAME, win_xdr_frame)
#define CMD_WIN_QUDOLBY_FRAME _IOW(HI_ID_WIN, IOC_WIN_QUDOLBY_FRAME, win_xdr_frame)

#define CMD_WIN_QUSYNC_FRAME _IOWR(HI_ID_WIN, IOC_WIN_QUSYNC_FRAME, win_syncframe)
#define CMD_WIN_QU_ULSFRAME  _IOW(HI_ID_WIN, IOC_WIN_QU_ULSFRAME, win_frame)
#define CMD_WIN_DQ_FRAME     _IOWR(HI_ID_WIN, IOC_WIN_DQ_FRAME, win_frame)

#define CMD_WIN_FREEZE            _IOW(HI_ID_WIN, IOC_WIN_FREEZE, win_freeze)
#define CMD_WIN_GET_FREEZE_STATUS _IOWR(HI_ID_WIN, IOC_WIN_GET_FREEZE_STATUS, win_freeze)
#define CMD_WIN_RESET             _IOW(HI_ID_WIN, IOC_WIN_RESET, win_reset)
#define CMD_WIN_PAUSE             _IOW(HI_ID_WIN, IOC_WIN_PAUSE, win_pause)
#define CMD_WIN_GET_PAUSE_STATUS _IOWR(HI_ID_WIN, IOC_WIN_GET_PAUSE_STATUS, win_pause)

#define CMD_WIN_SET_QUICK  _IOW(HI_ID_WIN, IOC_WIN_SET_QUICK, win_set_quick)
#define CMD_WIN_GET_QUICK  _IOWR(HI_ID_WIN, IOC_WIN_GET_QUICK, win_set_quick)
#define CMD_WIN_SET_ZORDER _IOW(HI_ID_WIN, IOC_WIN_SET_ZORDER, win_zorder)
#define CMD_WIN_GET_ORDER  _IOWR(HI_ID_WIN, IOC_WIN_GET_ORDER, win_order)

#define CMD_WIN_CAPTURE     _IOWR(HI_ID_WIN, IOC_WIN_CAP_FRAME, win_capture)
#define CMD_WIN_CAP_RELEASE _IOWR(HI_ID_WIN, IOC_WIN_CAP_REL_FRAME, win_capture)

#define CMD_WIN_SEND_FRAME _IOW(HI_ID_WIN, IOC_WIN_SEND_FRAME, win_frame)

#define CMD_WIN_STEP_MODE _IOW(HI_ID_WIN, IOC_WIN_STEP_MODE, win_step_mode)
#define CMD_WIN_STEP_PLAY _IOW(HI_ID_WIN, IOC_WIN_STEP_PLAY, hi_handle)

#define CMD_WIN_VIR_ACQUIRE   _IOWR(HI_ID_WIN, IOC_WIN_VIR_ACQUIRE, win_frame)
#define CMD_WIN_VIR_RELEASE   _IOWR(HI_ID_WIN, IOC_WIN_VIR_RELEASE, win_frame)
#define CMD_WIN_VIR_EXTERNBUF _IOW(HI_ID_WIN, IOC_WIN_VIR_EXTERNBUF, win_buf_pool)

#define CMD_WIN_SUSPEND _IOW(HI_ID_WIN, IOC_WIN_SUSPEND, hi_u32)
#define CMD_WIN_RESUM   _IOW(HI_ID_WIN, IOC_WIN_RESUME, hi_u32)

#define CMD_WIN_GET_HANDLE _IOWR(HI_ID_WIN, IOC_WIN_DEBUG_GET_HANDLE, win_get_handle)

#define CMD_WIN_ATTACH _IOWR(HI_ID_WIN, IOC_WIN_ATTACH, win_attach)
#define CMD_WIN_DETACH _IOWR(HI_ID_WIN, IOC_WIN_DETACH, win_attach)

#define CMD_WIN_GET_LATESTFRAME_INFO _IOWR(HI_ID_WIN, IOC_WIN_GET_LATESTFRAME_INFO, win_frame)
#define CMD_VO_WIN_CAPTURE_START     _IOWR(HI_ID_WIN, IOC_VO_WIN_CAPTURE_START, win_capture)
#define CMD_VO_WIN_CAPTURE_RELEASE   _IOWR(HI_ID_WIN, IOC_VO_WIN_CAPTURE_RELEASE, win_capture)
#define CMD_VO_WIN_CAPTURE_FREE      _IOWR(HI_ID_WIN, IOC_VO_WIN_CAPTURE_FREE, win_capture)

#define CMD_WIN_SET_ROTATION _IOWR(HI_ID_WIN, IOC_WIN_SET_ROTATION, win_rotation)
#define CMD_WIN_GET_ROTATION _IOWR(HI_ID_WIN, IOC_WIN_GET_ROTATION, win_rotation)
#define CMD_WIN_SET_FLIP     _IOWR(HI_ID_WIN, IOC_WIN_SET_FLIP, win_flip)
#define CMD_WIN_GET_FLIP     _IOWR(HI_ID_WIN, IOC_WIN_GET_FLIP, win_flip)

#define CMD_WIN_SET_MODE                        _IOWR(HI_ID_WIN, IOC_WIN_SET_MODE, win_mode)
#define CMD_WIN_GET_OUT_TYPE_AND_ENGINE  _IOWR(HI_ID_WIN, IOC_WIN_GET_OUT_TYPE_AND_ENGINE, win_out_type_and_engine)
#define CMD_WIN_GET_DISP_OUT_TYPE_AND_GFX_STATE _IOWR(HI_ID_WIN,  \
            IOC_WIN_GET_DISP_OUT_TYPE_AND_GFX_STATE, win_disp_out_type_and_gfx_state)
#define CMD_WIN_GET_UNLOAD                      _IOWR(HI_ID_WIN, IOC_WIN_GET_UNLOAD, win_unload)

#define CMD_WIN_CLOSE_HDR_PATH        _IOWR(HI_ID_WIN, IOC_WIN_CLOSE_HDR_PATH, win_enable)
#define CMD_WIN_SET_DOLBY_LIB_STATUS  _IOWR(HI_ID_WIN, IOC_WIN_SET_DOLBY_LIB_STATUS, win_dolby_lib_info)
#define CMD_WIN_SET_TECHNI_LIB_STATUS _IOWR(HI_ID_WIN, IOC_WIN_SET_TECHNI_LIB_STATUS, win_technicolor_lib_info)
#define CMD_WIN_DEBUG                 _IOWR(HI_ID_WIN, IOC_WIN_DEBUG, win_debug_cmd_and_args)

#define CMD_WIN_SET_ALPHA           _IOWR(HI_ID_WIN, IOC_WIN_SET_ALPHA, win_alpha)
#define CMD_WIN_GET_ALPHA           _IOWR(HI_ID_WIN, IOC_WIN_GET_ALPHA, win_alpha)
#define CMD_WIN_SET_FRAME_PROGINTER _IOWR(HI_ID_WIN, IOC_WIN_SET_FRAME_PROGINTERLEAVE, win_forceframe_flag)
#define CMD_WIN_GET_FRAME_PROGINTER _IOWR(HI_ID_WIN, IOC_WIN_GET_FRAME_PROGINTERLEAVE, win_forceframe_flag)

#define CMD_WIN_RESET_FIRST_FRAME_STATUS _IOWR(HI_ID_WIN, IOC_WIN_RESET_FIRST_FRAME_STATUS, win_alpha)
#define CMD_WIN_SET_SYNC _IOWR(HI_ID_WIN, IOC_WIN_SET_SYNC, win_sync)

#define CMD_WIN_GET_TECHNI_OUT_BRIGHT _IOWR(HI_ID_WIN, IOC_WIN_GET_TECHNI_OUT_BRIGHT, win_alpha)

#define CMD_WIN_SET_PLAY_CTRL _IOW(HI_ID_WIN, IOC_WIN_SET_PLAY_CTRL, win_play_ctrl)
#define CMD_WIN_GET_PLAY_CTRL _IOWR(HI_ID_WIN, IOC_WIN_GET_PLAY_CTRL, win_play_ctrl)
#define CMD_WIN_GET_SINK_INFO _IOWR(HI_ID_WIN, IOC_WIN_GET_SINK_INFO, win_sink_info)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_WIN_IOCTL_H__ */

