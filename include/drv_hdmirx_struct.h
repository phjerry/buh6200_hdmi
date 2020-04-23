/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description : hdmirx struct-part
 * Author : Hisilicon multimedia interface software group
 * Create : 2019-12-5
 */
#ifndef __DRV_HDMIRX_STRUCT_H__
#define __DRV_HDMIRX_STRUCT_H__

#include "hi_type.h"
#include "hi_drv_atv.h"
#include "hi_drv_video.h"
#include "hi_drv_audio.h"

#define HDMIRX_EDID_LENGTH                 256
#define HDMIRX_EDID_NUM                    2
#define HDMIRX_HDCP_KEY_MAX_LENGTH         1024
#define HDMIRX_HDCP_ENCRYPT_KEY_MAX_LENGTH 16
#define HDMIRX_HDCP_IV_MAX_LENGTH          16
#define HDMIRX_CEC_ARG_MAX_COUNT           16

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* the config parameter of CEC OPCODE */
typedef enum {
    HI_DRV_HDMIRX_CEC_OPCODE_FEATURE_ABORT                 = 0x00,
    HI_DRV_HDMIRX_CEC_OPCODE_IMAGE_VIEW_ON                 = 0x04,
    HI_DRV_HDMIRX_CEC_OPCODE_TUNER_STEP_INCREMENT          = 0x05,
    HI_DRV_HDMIRX_CEC_OPCODE_TUNER_STEP_DECREMENT          = 0x06,
    HI_DRV_HDMIRX_CEC_OPCODE_TUNER_DEVICE_STATUS           = 0x07,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS      = 0x08,
    HI_DRV_HDMIRX_CEC_OPCODE_RECORD_ON                     = 0x09,
    HI_DRV_HDMIRX_CEC_OPCODE_RECORD_STATUS                 = 0x0A,
    HI_DRV_HDMIRX_CEC_OPCODE_RECORD_OFF                    = 0x0B,
    HI_DRV_HDMIRX_CEC_OPCODE_TEXT_VIEW_ON                  = 0x0D,
    HI_DRV_HDMIRX_CEC_OPCODE_RECORD_TV_SCREEN              = 0x0F,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_DECK_STATUS              = 0x1A,
    HI_DRV_HDMIRX_CEC_OPCODE_DECK_STATUS                   = 0x1B,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_MENU_LANGUAGE             = 0x32,
    HI_DRV_HDMIRX_CEC_OPCODE_CLEAR_ANALOGUE_TIMER          = 0x33,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_ANALOGUE_TIMER            = 0x34,
    HI_DRV_HDMIRX_CEC_OPCODE_TIMER_STATUS                  = 0x35,
    HI_DRV_HDMIRX_CEC_OPCODE_STANDBY                       = 0x36,
    HI_DRV_HDMIRX_CEC_OPCODE_PLAY                          = 0x41,
    HI_DRV_HDMIRX_CEC_OPCODE_DECK_CONTROL                  = 0x42,
    HI_DRV_HDMIRX_CEC_OPCODE_TIMER_CLEARED_STATUS          = 0x43,
    HI_DRV_HDMIRX_CEC_OPCODE_USER_CONTROL_PRESSED          = 0x44,
    HI_DRV_HDMIRX_CEC_OPCODE_USER_CONTROL_RELEASED         = 0x45,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_OSD_NAME                 = 0x46,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_OSD_NAME                  = 0x47,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_OSD_STRING                = 0x64,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_TIMER_PROGRAM_TITLE       = 0x67,
    HI_DRV_HDMIRX_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST     = 0x70,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_AUDIO_STATUS             = 0x71,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_SYSTEM_AUDIO_MODE         = 0x72,
    HI_DRV_HDMIRX_CEC_OPCODE_REPORT_AUDIO_STATUS           = 0x7A,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS = 0x7D,
    HI_DRV_HDMIRX_CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS      = 0x7E,
    HI_DRV_HDMIRX_CEC_OPCODE_ROUTING_CHANGE                = 0x80,
    HI_DRV_HDMIRX_CEC_OPCODE_ROUTING_INFORMATION           = 0x81,
    HI_DRV_HDMIRX_CEC_OPCODE_ACTIVE_SOURCE                 = 0x82,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS         = 0x83,
    HI_DRV_HDMIRX_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS       = 0x84,
    HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_ACTIVE_SOURCE         = 0x85,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_STREAM_PATH               = 0x86,
    HI_DRV_HDMIRX_CEC_OPCODE_DEVICE_VENDOR_ID              = 0x87,
    HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_COMMAND                = 0x89,
    HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN     = 0x8A,
    HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP       = 0x8B,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID         = 0x8C,
    HI_DRV_HDMIRX_CEC_OPCODE_MENU_REQUEST                  = 0x8D,
    HI_DRV_HDMIRX_CEC_OPCODE_MENU_STATUS                   = 0x8E,
    HI_DRV_HDMIRX_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS      = 0x8F,
    HI_DRV_HDMIRX_CEC_OPCODE_REPORT_POWER_STATUS           = 0x90,
    HI_DRV_HDMIRX_CEC_OPCODE_GET_MENU_LANGUAGE             = 0x91,
    HI_DRV_HDMIRX_CEC_OPCODE_SELECT_ANALOGUE_SERVICE       = 0x92,
    HI_DRV_HDMIRX_CEC_OPCODE_SELECT_DIGITAL_SERVICE        = 0x93,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_DIGITAL_TIMER             = 0x97,
    HI_DRV_HDMIRX_CEC_OPCODE_CLEAR_DIGITAL_TIMER           = 0x99,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_AUDIO_RATE                = 0x9A,
    HI_DRV_HDMIRX_CEC_OPCODE_INACTIVE_SOURCE               = 0x9D,
    HI_DRV_HDMIRX_CEC_OPCODE_CEC_VERSION                   = 0x9E,
    HI_DRV_HDMIRX_CEC_OPCODE_GET_CEC_VERSION               = 0x9F,
    HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_COMMAND_WITH_ID        = 0xA0,
    HI_DRV_HDMIRX_CEC_OPCODE_CLEAR_EXTERNAL_TIMER          = 0xA1,
    HI_DRV_HDMIRX_CEC_OPCODE_SET_EXTERNAL_TIMER            = 0xA2,
    HI_DRV_HDMIRX_CEC_OPCODE_REPORT_SHORT_AUDIO            = 0xA3,
    HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_SHORT_AUDIO           = 0xA4,
    HI_DRV_HDMIRX_CEC_OPCODE_INITIATE_ARC                  = 0xC0,
    HI_DRV_HDMIRX_CEC_OPCODE_REPORT_ARC_INITIATED          = 0xC1,
    HI_DRV_HDMIRX_CEC_OPCODE_REPORT_ARC_TERMINATED         = 0xC2,
    HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_ARC_INITIATION        = 0xC3,
    HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_ARC_TERMINATION       = 0xC4,
    HI_DRV_HDMIRX_CEC_OPCODE_TERMINATE_ARC                 = 0xC5,
    HI_DRV_HDMIRX_CEC_OPCODE_HEADER                        = 0xF8,
    HI_DRV_HDMIRX_CEC_OPCODE_ABORT                         = 0xFF,
    HI_DRV_HDMIRX_CEC_OPCODE_SENDPING                      = 0x100
} hi_drv_hdmirx_cec_opcode;

/* HDMI port ID */
typedef enum {
    HI_DRV_HDMIRX_PORT0,
    HI_DRV_HDMIRX_PORT1,
    HI_DRV_HDMIRX_PORT2,
    HI_DRV_HDMIRX_PORT3,
    HI_DRV_HDMIRX_PORT_MAX
} hi_drv_hdmirx_port;

typedef enum {
    HI_DRV_OVERSAMPLE_1X = 0,
    HI_DRV_OVERSAMPLE_2X,
    HI_DRV_OVERSAMPLE_4X,
    HI_DRV_OVERSAMPLE_MAX
} hi_drv_hdmirx_oversample;

typedef enum {
    HI_DRV_FRAME_PACKING_TYPE_2D,               /* <Normal frame, not a 3D frame */
    HI_DRV_FRAME_PACKING_TYPE_SIDE_BY_SIDE,     /* <Side by side */
    HI_DRV_FRAME_PACKING_TYPE_TOP_AND_BOTTOM,   /* <Top and bottom */
    HI_DRV_FRAME_PACKING_TYPE_TIME_INTERLACED,  /* <Time interlaced */
    HI_DRV_FRAME_PACKING_TYPE_FRAME_PACKING,    /* <frame packing */
    HI_DRV_FRAME_PACKING_TYPE_MAX
} hi_drv_video_frame_packing_type;

typedef struct {
    hi_u32 width;                      /* HDMIRX hactive */
    hi_u32 height;                     /* HDMIRX vactive */
    hi_u32 frame_rate;                 /* HDMIRX frequency of vsync */
    hi_drv_color_descript color_space; /* HDMIRX color space */
    hi_drv_pixel_format   pixel_fmt;   /* HDMIRX video formate */
    hi_drv_pixel_bitwidth bit_width;   /* HDMIRX video bit_width */
    hi_bool               interlace;   /* HDMIRX video interlace or progressive */
    hi_drv_hdmirx_oversample oversample;   /* HDMIRX video oversample */
    /* Rx2.0 adapting unf after finishing unf improving */
    hi_drv_video_frame_packing_type fmt;   /* <HDMIRX video 3D formate */
    hi_bool hdmi_mode;                 /* HDMIRX video HDMI or DVI */
    /* HDMIRX video vblank the value must set when the 3D formate is frame PACKING */
    hi_u32  vblank;
    hi_bool pc_mode;                   /* HDMIRX PC timing mode flag */
    hi_u32  timing_idx;                /* HDMIRX timing inedex table */
    hi_bool mhl;                       /* HDMIRX MHL flag */
} hi_drv_hdmirx_timing_info;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_hdmirx_timing_info timing_info;
} hi_drv_hdmirx_timing;

typedef struct {
    hi_u32 edid_length;
    hi_u8  edid_data[HDMIRX_EDID_LENGTH]; /* HDMIRX EDID value */
    hi_u32 cec_addr;
} hi_drv_hdmirx_edid;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_hdmirx_edid edid;
} hi_drv_hdmirx_edid_info;

typedef struct {
    hi_drv_hdmirx_edid edid_group[HDMIRX_EDID_NUM];
    hi_u32 count;
} hi_drv_hdmirx_edid_init;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_hdmirx_edid_init init_edid;
} hi_drv_hdmirx_edid_init_info;

typedef enum {
    HI_DRV_HDMIRX_HDCPTYPE_14,
    HI_DRV_HDMIRX_HDCPTYPE_22,
    HI_DRV_HDMIRX_HDCPTYPE_MAX
} hi_drv_hdmirx_hdcp_type;

typedef struct {
    hi_u32 hdcp_length;
    hi_u8 hdcp_data[HDMIRX_HDCP_KEY_MAX_LENGTH];        /* HDMIRX HDCP key data */
    hi_u32 hdcp_key_length;
    hi_u8 hdcp_key[HDMIRX_HDCP_ENCRYPT_KEY_MAX_LENGTH]; /* HDMIRX HDCP encrypt key data */
    hi_u32 hdcp_iv_length;
    hi_u8 hdcp_iv[HDMIRX_HDCP_IV_MAX_LENGTH];           /* HDMIRX HDCP Iv data */
} hi_drv_hdmirx_hdcp;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_hdmirx_hdcp_type hdcp_type;
    hi_drv_hdmirx_hdcp hdcp;
}hi_drv_hdmirx_hdcp_info;

typedef enum {
    HI_DRV_HDMIRX_FORMAT_HDMI,
    HI_DRV_HDMIRX_FORMAT_DVI,
    HI_DRV_HDMIRX_FORMAT_MHL,
    HI_DRV_HDMIRX_FORMAT_MAX
} hi_drv_hdmirx_format;

typedef struct {
    hi_bool connected;                  /* HDMIRX off_line_detect status for callback */
    hi_drv_hdmirx_format format;
}hi_drv_hdmirx_offline_info;

/* the config parameter of off_line_detect status interface */
typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_hdmirx_offline_info info;
} hi_drv_hdmirx_offline_status;

/* defines the HDMI RX audio data format. */
typedef enum {
    HI_DRV_AI_HDMI_FORMAT_LPCM,
    HI_DRV_AI_HDMI_FORMAT_LBR,
    HI_DRV_AI_HDMI_FORMAT_HBR,
    HI_DRV_AI_HDMI_FORMAT_MAX
} hi_drv_hdmirx_aud_format;

typedef enum {
    HI_DRV_AI_HDMI_DATA_PCM = 0,
    HI_DRV_AI_HDMI_DATA_DD = 1,
    HI_DRV_AI_HDMI_DATA_DDP = 2,
    HI_DRV_AI_HDMI_DATA_DTS = 3,
    HI_DRV_AI_HDMI_DATA_DTSHD = 4,
    HI_DRV_AI_HDMI_DATA_TRUEHD = 5,
    HI_DRV_AI_HDMI_DATA_UNKNOWN = 6,
    HI_DRV_AI_HDMI_DATA_MAT = 7,
    HI_DRV_AI_HDMI_DATA_MAX
} hi_drv_hdmirx_aud_data_type;

/* defines  HDMI audio inputport attribute */
typedef struct {
    /* channel number */
    hi_audio_channel channel;
    /* bit depth */
    hi_bit_depth bit_depth;
    /* sample rate */
    hi_sample_rate sample_rate;
    /* HDMI audio data format */
    hi_drv_hdmirx_aud_format hdmi_audio_data_format;
    /* HDMI passthrough audio data type */
    hi_drv_hdmirx_aud_data_type hdmi_audio_data_type;
} hi_drv_ai_hdmirx_attr;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_ai_hdmirx_attr attr;
}hi_drv_hdmirx_aud_info;

/* the config parameter of MHL RCP key */
typedef enum {
    HI_DRV_HDMIRX_RCP_KEY_SELECT,
    HI_DRV_HDMIRX_RCP_KEY_UP,
    HI_DRV_HDMIRX_RCP_KEY_DOWN,
    HI_DRV_HDMIRX_RCP_KEY_LEFT,
    HI_DRV_HDMIRX_RCP_KEY_RIGHT,
    HI_DRV_HDMIRX_RCP_KEY_ROOT_MENU,
    HI_DRV_HDMIRX_RCP_KEY_EXIT,
    HI_DRV_HDMIRX_RCP_KEY_PLAY,
    HI_DRV_HDMIRX_RCP_KEY_STOP,
    HI_DRV_HDMIRX_RCP_KEY_PAUSE,
    HI_DRV_HDMIRX_RCP_KEY_REWIND,
    HI_DRV_HDMIRX_RCP_KEY_FAST_FWD,
    HI_DRV_HDMIRX_RCP_KEY_MAX
} hi_drv_hdmirx_rcp_key;

/* the CEC command state of sending */
typedef enum {
    HI_DRV_HDMIRX_CEC_CMD_STATE_SUCCESS,
    HI_DRV_HDMIRX_CEC_CMD_STATE_FAILED,
    HI_DRV_HDMIRX_CEC_CMD_STATE_MAX
} hi_drv_hdmirx_cec_cmd_state;

typedef struct {
    hi_u8 args[HDMIRX_CEC_ARG_MAX_COUNT]; /* array size 16 */
    hi_u8 arg_count;
} hi_drv_hdmirx_cec_operand;

typedef struct {
    hi_u8  src_addr;                   /* logical address of source */
    hi_u8  dst_addr;                   /* logical address of destination */
    hi_drv_hdmirx_cec_opcode opcode;   /* opration code */
    hi_drv_hdmirx_cec_operand operand; /* operand */
} hi_drv_hdmirx_cec_cmd;

typedef struct {
    hi_drv_hdmirx_cec_opcode cur_opcode; /* opration code */
    hi_drv_hdmirx_cec_cmd_state cur_state;
    hi_u32 dst_addr; /* logical address of destination */
} hi_drv_hdmirx_cec_cmd_state_data;

typedef struct {
    hi_drv_hdmirx_port port; /* HDMIRX port ID */
    hi_bool value;           /* HDMIRX hot_plug value */
} hi_drv_hdmirx_hpd;

/* the config parameter of EDID mode */
typedef enum {
    HI_DRV_HDMIRX_EDID_MODE_MENU, /* manual operation mode */
    HI_DRV_HDMIRX_EDID_MODE_AUTO, /* auto operation mode */
    HI_DRV_HDMIRX_EDID_MODE_MAX
} hi_drv_hdmirx_edid_mode;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_hdmirx_edid_mode mode;
} hi_drv_hdmirx_edid_mode_info;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_drv_sig_status status;
} hi_drv_sig_info;

typedef struct {
    hi_drv_hdmirx_port port;
    hi_bool status;
} hi_drv_hdmirx_status;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __DRV_HDMIRX_STRUCT_H__ */
