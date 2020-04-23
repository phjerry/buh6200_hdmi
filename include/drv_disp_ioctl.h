/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: ioctl
* Author: vdp
* Create: 2019-06-28
 */

#ifndef __DRV_DISP_IOCTL_H__
#define __DRV_DISP_IOCTL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_drv_video.h"
#include "hi_drv_disp.h"

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_out_type disp_type;
} disp_set_output;

typedef struct {
    hi_drv_display master;
    hi_drv_display slave;
} disp_attach;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_intf intf;
    hi_bool enable;
} disp_set_intf_enable;

typedef struct {
    hi_drv_display disp;
    hi_bool enable;
} disp_enable;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_intf intf;
} disp_set_intf;

#define HI_DRV_DISP_INTF_MAX_NUMBER (8)

typedef struct {
    hi_drv_display disp;
    hi_u32 intf_num;
    hi_drv_disp_intf intf[HI_DRV_DISP_INTF_MAX_NUMBER];
} disp_get_intf;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_stereo_mode stereo;
    hi_drv_disp_fmt format;
} disp_format;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_timing timing_para;
} disp_timing;

typedef struct {
    hi_drv_display disp;
    hi_bool refirst;
} disp_r_eye_first;

typedef struct {
    hi_drv_display disp;
    hi_rect vir_screen;
} disp_vir_screen;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_offset screen_offset;
} disp_screen_offset;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_layer layer;
    hi_drv_disp_zorder zflag;
} disp_zorder;


typedef struct {
    hi_drv_display disp;
    hi_drv_disp_color bgcolor;
} disp_bgc;

typedef struct {
    hi_drv_display disp;
    hi_rect rect;
} disp_screen;

typedef struct {
    hi_handle hvbi;
    hi_drv_disp_vbi_data vbi_data;
} disp_vbi;

typedef struct {
    hi_handle hvbi;
    hi_drv_disp_compat_vbi_data vbi_data;
} disp_compat_vbi;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_vbi_cfg cfg;
    hi_handle hvbi;
} disp_vbi_create_channel;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_wss_data wss_data;
} disp_wss;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_macrovision mcrvsn;
} disp_mcrvsn;

/* cgms parameter struct */
typedef struct {
    hi_drv_display disp;
    hi_drv_disp_cgmsa_cfg cgms_cfg;
} disp_cgms;

/* cgms parameter struct */
typedef struct {
    hi_drv_display disp;
    hi_rect outrect_cfg;
} disp_outrect;

typedef struct {
    hi_drv_display disp;
    hi_bool enable;
} disp_tccsc;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_aspect_mode aspect_mode;
    hi_u32 w;
    hi_u32 h;
} disp_aspect_ratio;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_cast_cfg cfg;
    hi_handle hcast;
} disp_cast_create;

typedef struct {
    hi_handle hcast;
} disp_cast_destroy;

typedef struct {
    hi_handle hcast;
    hi_bool enable;
} disp_cast_enable;

typedef struct {
    hi_handle hcast;
    hi_drv_video_frame frame;
} disp_cast_frame;

typedef struct {
    hi_drv_display disp_layer;
    hi_drv_video_frame frame;
} disp_snapshot_frame;

typedef enum {
    EXT_ATTACH_TYPE_SRC = 0,
    EXT_ATTACH_TYPE_SINK,
    EXT_ATTACH_TYPE_BUTT
} disp_ext_attach_type;

typedef struct {
    disp_ext_attach_type type;
    hi_handle hcast;
    hi_handle hmutual;
} disp_ext_attach;

typedef struct {
    hi_handle hcast;
    hi_drv_disp_cast_attr cast_attr;
} disp_cast_ext_attr;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_output_color_space color_space;
} disp_colorspace;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_xdr_engine xdr_engine;
} disp_xdr_engine;

typedef struct {
    hi_drv_display disp;
    hi_u32 alpha;
} disp_alpha;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_sink_capability sink_capability;
} disp_sink_capability;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_capability disp_capability;
} disp_get_capability;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_match_content_mode match_content_mode;
} disp_match_mode;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_output_status output_status;
} disp_output_status;

typedef struct {
    hi_drv_display disp;
    hi_s32 techni_bright;
} disp_techni_bright;

typedef struct {
    hi_drv_display disp_channel;
    hi_drv_disp_hdr_priority_mode hdr_priority_mode;
} disp_hdr_priority;

typedef struct {
    hi_u32 vic;
    hi_drv_disp_fmt fmt;
} disp_get_fmt_from_vic;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_vrr disp_vrr;
} disp_set_vrr;

typedef struct {
    hi_drv_display disp;
    hi_bool enable;
} disp_set_qms;

typedef struct {
    hi_drv_display disp;
    hi_bool enable;
} disp_set_allm;

typedef struct {
    hi_drv_display disp;
    hi_u32 factor;
} disp_set_qft;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_format_param format_param;
} disp_set_timing;

typedef struct {
    disp_set_timing disp_timing[DISP_CHANNEL_MAX_NUMBER];
} disp_set_multi_channel_timing;

typedef enum {
    IOC_DISP_ATTACH = 0,
    IOC_DISP_DETACH,

    IOC_DISP_OPEN,
    IOC_DISP_CLOSE,

    IOC_DISP_SET_ENABLE,
    IOC_DISP_GET_ENABLE,

    IOC_DISP_ADD_INTF,
    IOC_DISP_DEL_INTF,
    IOC_DISP_GET_INTF_STATUS,

    IOC_DISP_SET_INTF_ENABLE,
    IOC_DISP_GET_INTF_ENABLE,

    IOC_DISP_SET_FORMAT,
    IOC_DISP_GET_FORMAT,

    IOC_DISP_SET_R_EYE_FIRST,
    IOC_DISP_GET_R_EYE_FIRST,

    IOC_DISP_SET_VIRTSCREEN,
    IOC_DISP_GET_VIRTSCREEN,

    IOC_DISP_SET_SCREENOFFSET,
    IOC_DISP_GET_SCREENOFFSET,

    IOC_DISP_SET_DAC,
    IOC_DISP_GET_DAC,

    IOC_DISP_SET_TIMING,
    IOC_DISP_GET_TIMING,
    IOC_DISP_SET_CUSTOM_TIMING,

    IOC_DISP_SET_ZORDER,
    IOC_DISP_GET_ORDER,

    IOC_DISP_SET_BGC,
    IOC_DISP_GET_BGC,

    IOC_DISP_SET_SCREEN,
    IOC_DISP_GET_SCREEN,

    IOC_DISP_CREATE_VBI_CHANNEL,
    IOC_DISP_DESTROY_VBI_CHANNEL,
    IOC_DISP_SEND_VBI,
    IOC_DISP_SET_WSS,

    IOC_DISP_SET_MCRVSN,
    IOC_DISP_GET_MCRVSN,

    IOC_DISP_SET_HDMIINF,
    IOC_DISP_GET_HDMIINF,

    IOC_DISP_SET_CGMS,
    IOC_DISP_SET_DISP_SCREEN,
    IOC_DISP_GET_DISP_SCREEN,

    IOC_DISP_SET_DEV_RATIO,
    IOC_DISP_GET_DEV_RATIO,

    IOC_DISP_ACQUIRE_SNAPSHOT,
    IOC_DISP_RELEASE_SNAPSHOT,

    IOC_DISP_CREATE_CAST,
    IOC_DISP_DESTROY_CAST,
    IOC_DISP_SET_CAST_ENABLE,
    IOC_DISP_GET_CAST_ENABLE,
    IOC_DISP_ACQUIRE_CAST_FRAME,
    IOC_DISP_RELEASE_CAST_FRAME,
    IOC_DISP_EXT_ATTACH,
    IOC_DISP_EXT_DEATTACH,
    IOC_DISP_SET_CASTATTR,
    IOC_DISP_GET_CASTATTR,
    IOC_DISP_SUSPEND,
    IOC_DISP_RESUME,
    IOC_DISP_FORCESET_DAC_ENABLE,
    IOC_DISP_SET_OUTPUT_TYPE,
    IOC_DISP_GET_OUTPUT_TYPE,

    IOC_DISP_SET_OUTPUT_COLOR_SPACE,
    IOC_DISP_GET_OUTPUT_COLOR_SPACE,
    IOC_DISP_SET_XDR_ENGINE,
    IOC_DISP_GET_XDR_ENGINE,
    IOC_DISP_SET_ALPHA,
    IOC_DISP_GET_ALPHA,

    IOC_DISP_SET_SINK_CAPABILITY,
    IOC_DISP_GET_CAPABILITY,
    IOC_DISP_GET_OUTPUT_STATUS,

    IOC_DISP_SET_MATCH_CONTENT_MODE,
    IOC_DISP_GET_MATCH_CONTENT_MODE,

    IOC_DISP_SET_TECHNI_BRIGHT,
    IOC_DISP_GET_TECHNI_BRIGHT,

    IOC_DISP_SET_HDR_PRIORITY,
    IOC_DISP_GET_HDR_PRIORITY,

    IOC_DISP_GET_FMT_FROM_VIC,

    IOC_DISP_SET_VRR,
    IOC_DISP_GET_VRR,

    IOC_DISP_SET_QMS,
    IOC_DISP_GET_QMS,

    IOC_DISP_SET_ALLM,
    IOC_DISP_GET_ALLM,

    IOC_DISP_SET_QFT,
    IOC_DISP_GET_QFT,

    IOC_DISP_SET_COLOR_SPACE,
    IOC_DISP_GET_COLOR_SPACE,

    IOC_DISP_SET_BUTT
} IOC_DISP_E;

#define CMD_DISP_ATTACH _IOW(HI_ID_DISP, IOC_DISP_ATTACH, disp_attach)
#define CMD_DISP_DETACH _IOW(HI_ID_DISP, IOC_DISP_DETACH, disp_attach)

#define CMD_DISP_OPEN  _IOW(HI_ID_DISP, IOC_DISP_OPEN, hi_drv_display)
#define CMD_DISP_CLOSE _IOW(HI_ID_DISP, IOC_DISP_CLOSE, hi_drv_display)

#define CMD_DISP_SET_ENABLE _IOW(HI_ID_DISP, IOC_DISP_SET_ENABLE, disp_enable)
#define CMD_DISP_GET_ENABLE _IOWR(HI_ID_DISP, IOC_DISP_GET_ENABLE, disp_enable)

#define CMD_DISP_ADD_INTF _IOW(HI_ID_DISP, IOC_DISP_ADD_INTF, disp_set_intf)
#define CMD_DISP_DEL_INTF _IOW(HI_ID_DISP, IOC_DISP_DEL_INTF, disp_set_intf)
#define CMD_DISP_GET_INTF_STATUS _IOWR(HI_ID_DISP, IOC_DISP_GET_INTF_STATUS, disp_get_intf)

#define CMD_DISP_SET_FORMAT _IOW(HI_ID_DISP, IOC_DISP_SET_FORMAT, disp_format)
#define CMD_DISP_GET_FORMAT _IOWR(HI_ID_DISP, IOC_DISP_GET_FORMAT, disp_format)

#define CMD_DISP_SET_R_E_FIRST _IOW(HI_ID_DISP, IOC_DISP_SET_R_EYE_FIRST, disp_r_eye_first)
#define CMD_DISP_GET_R_E_FIRST _IOWR(HI_ID_DISP, IOC_DISP_GET_R_EYE_FIRST, disp_r_eye_first)

#define CMD_DISP_SET_VIRTSCREEN _IOW(HI_ID_DISP, IOC_DISP_SET_VIRTSCREEN, disp_vir_screen)
#define CMD_DISP_GET_VIRTSCREEN _IOWR(HI_ID_DISP, IOC_DISP_GET_VIRTSCREEN, disp_vir_screen)

#define CMD_DISP_SET_SCREENOFFSET _IOW(HI_ID_DISP, IOC_DISP_SET_SCREENOFFSET, disp_screen_offset)
#define CMD_DISP_GET_SCREENOFFSET _IOWR(HI_ID_DISP, IOC_DISP_GET_SCREENOFFSET, disp_screen_offset)

#define CMD_DISP_SET_DAC _IOW(HI_ID_DISP, IOC_DISP_SET_DAC, disp_set_intf)
#define CMD_DISP_GET_DAC _IOR(HI_ID_DISP, IOC_DISP_GET_DAC, disp_set_intf)

#define CMD_DISP_SET_TIMING _IOW(HI_ID_DISP, IOC_DISP_SET_TIMING, disp_set_multi_channel_timing)
#define CMD_DISP_GET_TIMING _IOWR(HI_ID_DISP, IOC_DISP_GET_TIMING, disp_set_timing)
#define CMD_DISP_SET_CUSTOM_TIMING _IOW(HI_ID_DISP, IOC_DISP_SET_CUSTOM_TIMING, disp_set_timing)

#define CMD_DISP_SET_ZORDER _IOW(HI_ID_DISP, IOC_DISP_SET_ZORDER, disp_zorder)
#define CMD_DISP_GET_ORDER  _IOWR(HI_ID_DISP, IOC_DISP_GET_ORDER, disp_zorder)

#define CMD_DISP_SET_BGC _IOW(HI_ID_DISP, IOC_DISP_SET_BGC, disp_bgc)
#define CMD_DISP_GET_BGC _IOWR(HI_ID_DISP, IOC_DISP_GET_BGC, disp_bgc)

#define CMD_DISP_SET_SCREEN _IOW(HI_ID_DISP, IOC_DISP_SET_SCREEN, disp_screen)
#define CMD_DISP_GET_SCREEN _IOWR(HI_ID_DISP, IOC_DISP_GET_SCREEN, disp_screen)

#define CMD_DISP_CREATE_VBI_CHANNEL  _IOWR(HI_ID_DISP, IOC_DISP_CREATE_VBI_CHANNEL, disp_vbi_create_channel)
#define CMD_DISP_DESTROY_VBI_CHANNEL _IOW(HI_ID_DISP, IOC_DISP_DESTROY_VBI_CHANNEL, hi_handle)
#define CMD_DISP_SEND_VBI            _IOW(HI_ID_DISP, IOC_DISP_SEND_VBI, disp_vbi)
#define CMD_DISP_COMPAT_SEND_VBI     _IOW(HI_ID_DISP, IOC_DISP_SEND_VBI, disp_compat_vbi)
#define CMD_DISP_SET_WSS             _IOW(HI_ID_DISP, IOC_DISP_SET_WSS, disp_wss)

#define CMD_DISP_SET_MCRVSN _IOW(HI_ID_DISP, IOC_DISP_SET_MCRVSN, disp_mcrvsn)
#define CMD_DISP_GET_MCRVSN _IOWR(HI_ID_DISP, IOC_DISP_GET_MCRVSN, disp_mcrvsn)

#define CMD_DISP_SET_CGMS _IOW(HI_ID_DISP, IOC_DISP_SET_CGMS, disp_cgms)

#define CMD_DISP_GET_DISP_SCREEN _IOWR(HI_ID_DISP, IOC_DISP_GET_DISP_SCREEN, disp_outrect)
#define CMD_DISP_SET_DISP_SCREEN _IOW(HI_ID_DISP, IOC_DISP_SET_DISP_SCREEN, disp_outrect)

#define CMD_DISP_SET_DEV_RATIO _IOW(HI_ID_DISP, IOC_DISP_SET_DEV_RATIO, disp_aspect_ratio)
#define CMD_DISP_GET_DEV_RATIO _IOWR(HI_ID_DISP, IOC_DISP_GET_DEV_RATIO, disp_aspect_ratio)

#define CMD_DISP_CREATE_CAST        _IOWR(HI_ID_DISP, IOC_DISP_CREATE_CAST, disp_cast_create)
#define CMD_DISP_DESTROY_CAST       _IOW(HI_ID_DISP, IOC_DISP_DESTROY_CAST, disp_cast_create)
#define CMD_DISP_SET_CAST_ENABLE    _IOW(HI_ID_DISP, IOC_DISP_SET_CAST_ENABLE, disp_cast_enable)
#define CMD_DISP_GET_CAST_ENABLE    _IOWR(HI_ID_DISP, IOC_DISP_GET_CAST_ENABLE, disp_cast_enable)
#define CMD_DISP_ACQUIRE_CAST_FRAME _IOWR(HI_ID_DISP, IOC_DISP_ACQUIRE_CAST_FRAME, disp_cast_frame)
#define CMD_DISP_RELEASE_CAST_FRAME _IOW(HI_ID_DISP, IOC_DISP_RELEASE_CAST_FRAME, disp_cast_frame)
#define CMD_DISP_EXT_ATTACH         _IOWR(HI_ID_DISP, IOC_DISP_EXT_ATTACH, disp_ext_attach)
#define CMD_DISP_EXT_DEATTACH       _IOWR(HI_ID_DISP, IOC_DISP_EXT_DEATTACH, disp_ext_attach)

#define CMD_DISP_SET_CASTATTR     _IOWR(HI_ID_DISP, IOC_DISP_SET_CASTATTR, disp_cast_ext_attr)
#define CMD_DISP_GET_CASTATTR     _IOWR(HI_ID_DISP, IOC_DISP_GET_CASTATTR, disp_cast_ext_attr)
#define CMD_DISP_ACQUIRE_SNAPSHOT _IOWR(HI_ID_DISP, IOC_DISP_ACQUIRE_SNAPSHOT, disp_snapshot_frame)
#define CMD_DISP_RELEASE_SNAPSHOT _IOW(HI_ID_DISP, IOC_DISP_RELEASE_SNAPSHOT, disp_snapshot_frame)

#define CMD_DISP_SUSPEND _IOW(HI_ID_DISP, IOC_DISP_SUSPEND, hi_u32)
#define CMD_DISP_RESUME  _IOW(HI_ID_DISP, IOC_DISP_RESUME, hi_u32)

#define CMD_DISP_FORCESET_DAC_ENABLE _IOW(HI_ID_DISP, IOC_DISP_FORCESET_DAC_ENABLE, hi_bool)
#define CMD_DISP_SET_OUTPUT_TYPE     _IOW(HI_ID_DISP, IOC_DISP_SET_OUTPUT_TYPE, disp_set_output)
#define CMD_DISP_GET_OUTPUT_TYPE     _IOWR(HI_ID_DISP, IOC_DISP_GET_OUTPUT_TYPE, disp_set_output)

#define CMD_DISP_SET_ALPHA _IOW(HI_ID_DISP, IOC_DISP_SET_ALPHA, disp_alpha)
#define CMD_DISP_GET_ALPHA _IOWR(HI_ID_DISP, IOC_DISP_GET_ALPHA, disp_alpha)

#define CMD_DISP_SET_OUTPUT_COLOR_SPACE _IOW(HI_ID_DISP, IOC_DISP_SET_OUTPUT_COLOR_SPACE, disp_colorspace)
#define CMD_DISP_GET_OUTPUT_COLOR_SPACE _IOWR(HI_ID_DISP, IOC_DISP_GET_OUTPUT_COLOR_SPACE, disp_colorspace)
#define CMD_DISP_SET_XDR_ENGINE         _IOW(HI_ID_DISP, IOC_DISP_SET_XDR_ENGINE, disp_xdr_engine)
#define CMD_DISP_GET_XDR_ENGINE         _IOWR(HI_ID_DISP, IOC_DISP_GET_XDR_ENGINE, disp_xdr_engine)

#define CMD_DISP_SET_SINK_CAPABILITY _IOW(HI_ID_DISP, IOC_DISP_SET_SINK_CAPABILITY, disp_sink_capability)
#define CMD_DISP_GET_CAPABILITY      _IOWR(HI_ID_DISP, IOC_DISP_GET_CAPABILITY, disp_get_capability)

#define CMD_DISP_GET_OUTPUT_STATUS   _IOWR(HI_ID_DISP, IOC_DISP_GET_OUTPUT_STATUS, disp_output_status)

#define CMD_DISP_SET_MATCH_CONTENT_MODE _IOW(HI_ID_DISP, IOC_DISP_SET_MATCH_CONTENT_MODE, disp_match_mode)
#define CMD_DISP_GET_MATCH_CONTENT_MODE _IOWR(HI_ID_DISP, IOC_DISP_GET_MATCH_CONTENT_MODE, disp_match_mode)

#define CMD_DISP_SET_TECHNI_BRIGHT _IOW(HI_ID_DISP, IOC_DISP_SET_TECHNI_BRIGHT, disp_techni_bright)
#define CMD_DISP_GET_TECHNI_BRIGHT _IOWR(HI_ID_DISP, IOC_DISP_GET_TECHNI_BRIGHT, disp_techni_bright)

#define CMD_DISP_SET_HDR_PRIORITY _IOW(HI_ID_DISP, IOC_DISP_SET_HDR_PRIORITY, disp_hdr_priority)
#define CMD_DISP_GET_HDR_PRIORITY _IOWR(HI_ID_DISP, IOC_DISP_GET_HDR_PRIORITY, disp_hdr_priority)

#define CMD_DISP_GET_FMT_FROM_VIC _IOWR(HI_ID_DISP, IOC_DISP_GET_FMT_FROM_VIC, disp_get_fmt_from_vic)

#define CMD_DISP_SET_INTF_ENABLE _IOW(HI_ID_DISP, IOC_DISP_SET_INTF_ENABLE, disp_set_intf_enable)
#define CMD_DISP_GET_INTF_ENABLE _IOWR(HI_ID_DISP, IOC_DISP_GET_INTF_ENABLE, disp_set_intf_enable)

#define CMD_DISP_SET_VRR _IOW(HI_ID_DISP, IOC_DISP_SET_VRR, disp_set_vrr)
#define CMD_DISP_GET_VRR _IOWR(HI_ID_DISP, IOC_DISP_GET_VRR, disp_set_vrr)

#define CMD_DISP_SET_QMS _IOW(HI_ID_DISP, IOC_DISP_SET_QMS, disp_set_qms)
#define CMD_DISP_GET_QMS _IOWR(HI_ID_DISP, IOC_DISP_GET_QMS, disp_set_qms)

#define CMD_DISP_SET_ALLM _IOW(HI_ID_DISP, IOC_DISP_SET_ALLM, disp_set_allm)
#define CMD_DISP_GET_ALLM _IOWR(HI_ID_DISP, IOC_DISP_GET_ALLM, disp_set_allm)

#define CMD_DISP_SET_QFT _IOW(HI_ID_DISP, IOC_DISP_SET_QFT, disp_set_qft)
#define CMD_DISP_GET_QFT _IOWR(HI_ID_DISP, IOC_DISP_GET_QFT, disp_set_qft)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_DISP_IOCTL_H__ */

