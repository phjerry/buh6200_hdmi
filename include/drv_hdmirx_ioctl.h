/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description : hdmirx ioctl-part
 * Author : Hisilicon multimedia interface software group
 * Create : 2019-12-5
 */
#ifndef __DRV_HDMIRX_IOCTL_H__
#define __DRV_HDMIRX_IOCTL_H__

#include "hi_type.h"
#include "drv_hdmirx_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum {
    HI_DRV_HDMIRX_CEC_KEY_VOLUME_UP,
    HI_DRV_HDMIRX_CEC_KEY_VOLUME_DOWN,
    HI_DRV_HDMIRX_CEC_KEY_VOLUME_MUTE
} hi_drv_hdmirx_cec_key_code;

typedef enum {
    IOC_HDMIRX_CONNECT,
    IOC_HDMIRX_DISCONNECT,
    IOC_HDMIRX_G_STATUS,
    IOC_HDMIRX_G_TIMING_INFO,
    IOC_HDMIRX_S_UPDATE_EDID,
    IOC_HDMIRX_S_LOAD_HDCP,
    IOC_HDMIRX_G_OFFLINE_STATTUS,
    IOC_HDMIRX_G_AUDIO_INFO,
    IOC_HDMIRX_G_AUDIO_STATUS,
    IOC_HDMIRX_S_CEC_CMD,
    IOC_HDMIRX_S_CEC_ENABLE,
    IOC_HDMIRX_G_CEC_CMD,
    IOC_HDMIRX_G_CEC_CMDSTATE,
    IOC_HDMIRX_S_CEC_STANDBY_ENABLE,
    IOC_HDMIRX_S_HPD_VALUE,
    IOC_HDMIRX_S_INIT_EDID,
    IOC_HDMIRX_S_EDID_MODE
} ioc_hdmirx_func;

#define HIIOC_HDMIRX_CONNECT              _IOW(HI_ID_HDMIRX, IOC_HDMIRX_CONNECT, hi_drv_hdmirx_port)
#define HIIOC_HDMIRX_DISCONNECT           _IOW(HI_ID_HDMIRX, IOC_HDMIRX_DISCONNECT, hi_drv_hdmirx_port)
#define HIIOC_HDMIRX_G_STATUS             _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_G_STATUS, hi_drv_sig_info)
#define HIIOC_HDMIRX_G_TIMING_INFO        _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_G_TIMING_INFO, hi_drv_hdmirx_timing)
#define HIIOC_HDMIRX_S_UPDATE_EDID        _IOW(HI_ID_HDMIRX, IOC_HDMIRX_S_UPDATE_EDID, hi_drv_hdmirx_edid_info)
#define HIIOC_HDMIRX_S_LOAD_HDCP          _IOW(HI_ID_HDMIRX, IOC_HDMIRX_S_LOAD_HDCP, hi_drv_hdmirx_hdcp_info)
#define HIIOC_HDMIRX_G_OFF_LINE_DET_STA   _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_G_OFFLINE_STATTUS, hi_drv_hdmirx_offline_info)
#define HIIOC_HDMIRX_G_AUDIO_INFO         _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_G_AUDIO_INFO, hi_drv_hdmirx_aud_info)
#define HIIOC_HDMIRX_G_AUDIO_STATUS       _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_G_AUDIO_STATUS, hi_drv_sig_info)
#define HIIOC_HDMIRX_S_CEC_CMD            _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_S_CEC_CMD, hi_drv_hdmirx_cec_cmd)
#define HIIOC_HDMIRX_S_CEC_ENABLE         _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_S_CEC_ENABLE, hi_bool)
#define HIIOC_HDMIRX_G_CEC_CMD            _IOR(HI_ID_HDMIRX, IOC_HDMIRX_G_CEC_CMD, hi_drv_hdmirx_cec_cmd)
#define HIIOC_HDMIRX_G_CEC_CMDSTATE       _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_G_CEC_CMDSTATE, hi_drv_hdmirx_cec_cmd_state_data)
#define HIIOC_HDMIRX_S_CEC_STANDBY_ENABLE _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_S_CEC_STANDBY_ENABLE, hi_bool)
#define HIIOC_HDMIRX_S_HPD_VALUE          _IOWR(HI_ID_HDMIRX, IOC_HDMIRX_S_HPD_VALUE, hi_drv_hdmirx_hpd)
#define HIIOC_HDMIRX_S_INIT_EDID          _IOW(HI_ID_HDMIRX, IOC_HDMIRX_S_INIT_EDID, hi_drv_hdmirx_edid_init_info)
#define HIIOC_HDMIRX_S_EDID_MODE          _IOW(HI_ID_HDMIRX, IOC_HDMIRX_S_EDID_MODE, hi_drv_hdmirx_edid_mode_info)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __DRV_HDMIRX_IOCTL_H__ */
