/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi tx types definations for user/kernel communication
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-25
 */
#ifndef __DRV_HDMI_IOC_IOCTL_H__
#define __DRV_HDMI_IOC_IOCTL_H__

#include "hi_drv_module.h"
#include "hi_type.h"
#include "hi_hdmi_types.h"

#define CEC_MAX_MSG_SIZE            16
#define CEC_MIN_MSG_SIZE            1
#define CEC_INVALID_LOGICAL_ADDR    0xf
#define CEC_INVALID_PHYSICAL_ADDR   0xffff

typedef struct {
    hdmi_avail_mode native_mode;
    hdmi_avail_mode max_mode;
    hi_s32 mode_count;
    hi_u32 max_tmds_clock;
    hi_s32 dvi_only;
} hdmitx_ioctl_getcapbility;

typedef struct {
    hi_u64 mode_buf_ptr;
    hi_u32 mode_count;
} hdmitx_ioctl_getmodes;

typedef struct {
    hi_u64 edid_ptr;
    hi_u32 length;
    hi_s32 force;
} hdmitx_ioctl_getedid;

typedef struct {
    hdmi_manufacture_info manufacture;
    hdmi_edid_version edid_version;
    hdmi_colorimetry colorimetry;
    hdmi_latency latency;
    disp_para disp_para;
    hdmi_audio_speaker speaker;
    hdmi_audio_info audio;
    hdmi_dolby_info dolby;
    hdmi_hdr_info hdr;
    hdmi_vrr_info vrr;
} hdmitx_ioctl_getsinkinfo;

typedef struct {
    hi_s32 hotplug;
    hi_s32 rsen;
    hi_bool output_enable;
} hdmitx_ioctl_getstatus;

typedef struct {
    hi_u8 *srm_data;
    hi_u32 length;
} hdmitx_ioctl_setsrm;

typedef struct hdcp_usr_status{
    hi_bool auth_start;                /* HDCP authentication start. */
    hi_bool auth_success;              /* HDCP authentication success. */
    hi_u32 cur_reauth_times;           /* HDCP current re-authen times. */
    hi_u32 work_version;               /* HDCP authentication version.see HDCP_VERSION_XXX */
    hi_u32 err_code;                   /* HDCP authentication error code.see enum hdcp_err_code */
} hdmitx_ioctl_hdcp_status;

typedef struct hdcp_cap{
    hi_bool support_1x;
    hi_bool support_22;
    hi_bool support_23;
} hdmitx_ioctl_hdcp_cap;

typedef enum {
    CEC_LOGIC_ADDR_VALID,
    CEC_LOGIC_ADDR_INVALID,
    CEC_LOGIC_ADDR_NOT_CONFIG,
    CEC_LOGIC_ADDR_CONFIGURING,
} cec_logic_addr_status;

typedef enum {
    CEC_DEVICE_TYPE_TV = 0,
    CEC_DEVICE_TYPE_TUNER = 3,
    CEC_DEVICE_TYPE_PLAYBACK = 4,
    CEC_DEVICE_TYPE_MAX = 8,
} cec_device_type;

typedef enum {
    CEC_EVENT_STATUS_CHANGE,
    CEC_EVENT_RECEIVED,
    CEC_EVENT_MAX,
} cec_event_type;

typedef enum {
    CEC_TX_RESULT_ACK,
    CEC_TX_RESULT_NACK,
    CEC_TX_RESULT_FAILED,
    CEC_TX_RESULT_MAX,
} cec_transmit_result;

typedef struct {
    hi_u8 logical_addr;
    hi_u16 physical_addr;
    cec_logic_addr_status logic_addr_status;
} cec_status;

typedef struct {
    hi_u8 data[CEC_MAX_MSG_SIZE];
    hi_u8 len;
} cec_msg;

typedef union {
    cec_status status;
    cec_msg rx_msg;
} cec_event_data;

typedef struct {
    cec_event_type type;
    cec_event_data data;
} cec_events;

#define HDMITX_IOC_READ_EDID          _IOWR(HI_ID_HDMITX, 1, hdmitx_ioctl_getedid)
#define HDMITX_IOC_GET_SINKINFO       _IOWR(HI_ID_HDMITX, 2, hdmitx_ioctl_getsinkinfo)
#define HDMITX_IOC_GET_STATUS         _IOR(HI_ID_HDMITX, 3, hdmitx_ioctl_getstatus)
#define HDMITX_IOC_GET_AVAILCAPBILITY _IOR(HI_ID_HDMITX, 4, hdmitx_ioctl_getcapbility)
#define HDMITX_IOC_GET_AVAILMODES     _IOR(HI_ID_HDMITX, 5, hdmitx_ioctl_getmodes)
#define HDMITX_IOC_HDCP_LOADKEY       _IOWR(HI_ID_HDMITX, 6, hi_u32)
#define HDMITX_IOC_HDCP_SETSRM        _IOWR(HI_ID_HDMITX, 7, hdmitx_ioctl_setsrm)
#define HDMITX_IOC_HDCP_GETCAPABILITY _IOWR(HI_ID_HDMITX, 8, hdmitx_ioctl_hdcp_cap)
#define HDMITX_IOC_HDCP_SET_REAUTH    _IOWR(HI_ID_HDMITX, 9, hi_u32)
#define HDMITX_IOC_HDCP_START         _IOR(HI_ID_HDMITX, 10, hi_u32)
#define HDMITX_IOC_HDCP_STOP          _IO(HI_ID_HDMITX, 11)
#define HDMITX_IOC_HDCP_GETSTATUS     _IOR(HI_ID_HDMITX, 12, hdmitx_ioctl_hdcp_status)
#define HDMITX_IOC_CEC_OPEN           _IO(HI_ID_HDMITX, 13)
#define HDMITX_IOC_CEC_CLOSE          _IO(HI_ID_HDMITX, 14)
#define HDMITX_IOC_CEC_READ_EVENTS    _IOR(HI_ID_HDMITX, 15, cec_events)
#define HDMITX_IOC_CEC_GET_STATUS     _IOR(HI_ID_HDMITX, 16, cec_status)
#define HDMITX_IOC_CEC_SET_DEVICE_TYPE _IOW(HI_ID_HDMITX, 17, hi_u8)
#define HDMITX_IOC_CEC_TRANSMIT       _IOW(HI_ID_HDMITX, 18, cec_msg)
#define HDMITX_IOC_SET_FORCEOUTPUT    _IOWR(HI_ID_HDMITX, 19, hi_bool)

#endif

