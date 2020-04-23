/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019.
 * Description: hdmi driver hdmi ext header file
 * Author: hdmi sw team
 * Create: 2019-06-22
 */

#ifndef DRV_HDMI_MODULE_EXT_H
#define DRV_HDMI_MODULE_EXT_H

struct notifier_block;

#define AO_INTF_HDMI0 0
#define AO_INTF_HDMI1 1

#define DISP_INFT_HDMI0 0
#define DISP_INFT_HDMI1 1

/* color depth */
#define HDMI_BPC_24 0
#define HDMI_BPC_30 1
#define HDMI_BPC_36 2

/* AVI Byte1 S [1:0],Scan information */
#define HDMI_SCAN_NO_DATA    0
#define HDMI_SCAN_OVER_SCAN  1
#define HDMI_SCAN_UNDER_SCAN 2

/* AVI Byte1 B [1:0],Bar Data Present */
#define HDMI_BAR_DATA_NO_PRESENT  0
#define HDMI_BAR_DATA_VER_PRESENT 1
#define HDMI_BAR_DATA_HOR_PRESENT 2
#define HDMI_BAR_DATA_V_H_PRESENT 3

/* AVI Byte1 Y[2:0], RGB  or YCBCR .color format(pixel encoding format) */
#define HDMI_COLOR_FORMAT_RGB      0
#define HDMI_COLOR_FORMAT_YCBCR422 1
#define HDMI_COLOR_FORMAT_YCBCR444 2
#define HDMI_COLOR_FORMAT_YCBCR420 3

/**
 * AVI Byte2 R[3:0],Active Portion Aspect Ratio,
 * for ATSC .see CEA-861-G H.1 ATSC Active Format Description.
 */
#define HDMI_ACT_ASP_RATIO_ATSC_G_16_9           4
#define HDMI_ACT_ASP_RATIO_ATSC_SAME_PIC         8
#define HDMI_ACT_ASP_RATIO_ATSC_4_3              9
#define HDMI_ACT_ASP_RATIO_ATSC_16_9             10
#define HDMI_ACT_ASP_RATIO_ATSC_14_9             11
#define HDMI_ACT_ASP_RATIO_ATSC_4_3_CENTER_14_9  13
#define HDMI_ACT_ASP_RATIO_ATSC_16_9_CENTER_14_9 14
#define HDMI_ACT_ASP_RATIO_ATSC_16_9_CENTER_4_3  15

/**
 * AVI Byte2 R[3:0],Active Portion Aspect Ratio,
 * for DVB.see CEA-861-G H.2 DVB Active Format Description .
 */
#define HDMI_ACT_ASP_RATIO_DVB_BOX_16_9         2
#define HDMI_ACT_ASP_RATIO_DVB_BOX_14_9         3
#define HDMI_ACT_ASP_RATIO_DVB_BOX_16_9_CENTER  4
#define HDMI_ACT_ASP_RATIO_DVB_SAME_PIC         8
#define HDMI_ACT_ASP_RATIO_DVB_CENTER_4_3       9
#define HDMI_ACT_ASP_RATIO_DVB_CENTER_16_9      10
#define HDMI_ACT_ASP_RATIO_DVB_CENTER_14_9      11
#define HDMI_ACT_ASP_RATIO_DVB_4_3_CENTER_14_9  13
#define HDMI_ACT_ASP_RATIO_DVB_16_9_CENTER_14_9 14
#define HDMI_ACT_ASP_RATIO_DVB_16_9_CENTER_4_3  15

/* AVI Byte2 M[1:0],Picture Aspect Ratio */
#define HDMI_PIC_ASPECT_RATIO_NO_DATA 0
#define HDMI_PIC_ASPECT_RATIO_4_3     1
#define HDMI_PIC_ASPECT_RATIO_16_9    2
#define HDMI_PIC_ASPECT_RATIO_64_27   3
#define HDMI_PIC_ASPECT_RATIO_256_135 4

/* AVI Byte2 C[1:0],Colorimetry ;Byte3 EC[2:0]Extended Colorimetry
 * define [1:0] for C[1:0]; define [6:4] for EC[2:0]
 */
#define HDMI_COLORIMETRY_NO_DATA                 0x00
#define HDMI_COLORIMETRY_ITU601                  0x01
#define HDMI_COLORIMETRY_ITU709                  0x02
#define HDMI_COLORIMETRY_XVYCC_601               0x03
#define HDMI_COLORIMETRY_XVYCC_709               0x13
#define HDMI_COLORIMETRY_S_YCC_601               0x23
#define HDMI_COLORIMETRY_ADOBE_YCC_601           0x33
#define HDMI_COLORIMETRY_ADOBE_RGB               0x43
#define HDMI_COLORIMETRY_2020_CONST_LUMINOUS     0x53
#define HDMI_COLORIMETRY_2020_NON_CONST_LUMINOUS 0x63
#define HDMI_COLORIMETRY_ADDITION_EXTENSION      0x73

/* AVI Byte3 Q[1:0] ,RGB Quantization Range */
#define HDMI_RGB_QUANTIZEION_DEFAULT 0
#define HDMI_RGB_QUANTIZEION_LIMITED 1
#define HDMI_RGB_QUANTIZEION_FULL    2

/* AVI Byte5 Q[7:6] ,YCC Quantization Range */
#define HDMI_YCC_QUANTIZEION_LIMITED 0
#define HDMI_YCC_QUANTIZEION_FULL    1

/* AVI Byte5 CN[1:0],IT content type */
#define HDMI_IT_CONTENT_GRAPHYICS 0
#define HDMI_IT_CONTENT_PHOTO     1
#define HDMI_IT_CONTENT_CINIMA    2
#define HDMI_IT_CONTENT_GAME      3

/* AVI Byte5 PR[3:0],Pixel Repetition Factor */
#define HDMI_PIXEL_REPEAT_NO_REPEAT 0
#define HDMI_PIXEL_REPEAT_2_TIMES   1
#define HDMI_PIXEL_REPEAT_3_TIMES   2
#define HDMI_PIXEL_REPEAT_4_TIMES   3
#define HDMI_PIXEL_REPEAT_5_TIMES   4
#define HDMI_PIXEL_REPEAT_6_TIMES   5
#define HDMI_PIXEL_REPEAT_7_TIMES   6
#define HDMI_PIXEL_REPEAT_8_TIMES   7
#define HDMI_PIXEL_REPEAT_9_TIMES   8
#define HDMI_PIXEL_REPEAT_10_TIMES  9

/* 3D -- mode_3d */
#define HDMI_3D_FRAME_PACKETING        0x00
#define HDMI_3D_FIELD_ALTERNATIVE      0x01
#define HDMI_3D_LINE_ALTERNATIVE       0x02
#define HDMI_3D_SIDE_BY_SIDE_FULL      0x03
#define HDMI_3D_L_DEPTH                0x04
#define HDMI_3D_L_DEPTH_GRAPHICS_DEPTH 0x05
#define HDMI_3D_TOP_AND_BOTTOM         0x06
#define HDMI_3D_SIDE_BY_SIDE_HALF      0x08
#define HDMI_3D_NONE                   0x09

struct color_info {
    hi_u32 colorimetry;
    hi_u32 color_format;
    hi_u8 rgb_quantization;
    hi_u8 ycc_quantization;
};

struct timing_detail_info {
    hi_u32 htotal;
    hi_u32 hactive;
    hi_u32 hfront;
    hi_u32 hsync;
    hi_u32 hback;
    hi_u32 vtotal;
    hi_u32 vactive;
    hi_u32 vfront;
    hi_u32 vsync;
    hi_u32 vback;
    hi_u32 refresh_rate;
};

struct hdmi_out_data {
    hi_u32 color_depth; /* out color depth */
    struct color_info color;
};

struct hdmi_in_data {
    hi_u32 color_depth; /* in color depth */
    hi_bool v_sync_pol; /* Vsync */
    hi_bool h_sync_pol; /* Hsync */
    hi_bool de_pol;     /* DE	 */
    hi_u32 pixel_clock; /* in kHz */

    hi_u32 vic; /* AVI Byte4 VIC,Video ID Code */

    hi_u8 scan_info;               /* AVI Byte1 S [1:0],Scan information	 */
    hi_bool active_aspect_present; /* AVI Byte1 A[0], Active Format Information Present */
    hi_u8 bar_present;             /* AVI Byte1 B [1:0],Bar Data Present */

    hi_u32 picture_aspect_ratio; /* AVI Byte2 M[1:0],Picture Aspect Ratio */
    hi_u32 active_aspect_ratio;  /* AVI Byte2 R[3:0],Active Portion Aspect Ratio.support in both DVB & ATSC mode */

    hi_u8 picture_scal;       /* AVI Byte3 SC[1:0],Non-Uniform Picture Scaling */
    hi_bool it_content_valid; /* AVI Byte3 ITC,IT content */

    hi_u8 it_content_type; /* AVI Byte5 CN[1:0],IT content type */
    hi_u8 pixel_repeat;    /* AVI Byte5 PR[3:0],Pixel Repetition Factor	 */

    hi_u16 top_bar;    /* AVI Byte7-6 ETB15-ETB00 (Line Number of End of Top Bar) */
    hi_u16 bottom_bar; /* AVI Byte9-8 SBB15-SBB00 (Line Number of Start of Bottom Bar) */
    hi_u16 left_bar;   /* AVI Byte11-10 ELB15-ELB08 (Pixel Number of End of Left Bar) */
    hi_u16 right_bar;  /* AVI Byte13-12 SRB07-SRB00 (Pixel Number of Start of Right Bar)	 */

    hi_u8 mode_3d; /* VS	PB5[7:4],3D_Structure */
    struct color_info color;
    struct timing_detail_info detail;
};

/* hdmi_mode_type */
#define HDMI_MODE_TYPE_IN  (1 << 0)
#define HDMI_MODE_TYPE_OUT (1 << 1)
#define HDMI_MODE_TYPE_HDR (1 << 2)
#define HDMI_MODE_TYPE_VRR (1 << 3)

/* HDR mode type */
#define HDMI_HDR_MODE_SDR            0x00
#define HDMI_HDR_MODE_STATIC_TRD_SDR 0x10
#define HDMI_HDR_MODE_STATIC_TRD_HDR 0x11
#define HDMI_HDR_MODE_STATIC_ST2084  0x12
#define HDMI_HDR_MODE_STATIC_HLG     0x13
#define HDMI_HDR_MODE_DOLBY_V0       0x20
#define HDMI_HDR_MODE_DOLBY_V1       0x21
#define HDMI_HDR_MODE_DOLBY_V2       0x22
#define HDMI_HDR_MODE_DYNAMIC_TYPE1  0x31
#define HDMI_HDR_MODE_DYNAMIC_TYPE2  0x32
#define HDMI_HDR_MODE_DYNAMIC_TYPE3  0x33
#define HDMI_HDR_MODE_DYNAMIC_TYPE4  0x34

struct hdmi_hdr_data {
    hi_u32 hdr_mode_type;
    /* hi_bool	 hdmi_transmit; */
    /* hi_u32	 s_data_len; */
    /* void	 *s_data; */
};

struct hdmi_timing_data {
    struct hdmi_in_data in;
    struct hdmi_out_data out;
};

/* VRR mode type */
#define HDMI_VRR_MODE_VRR  0
#define HDMI_VRR_MODE_QMS  1
#define HDMI_VRR_MODE_QFT  2
#define HDMI_VRR_MODE_ALLM 3

struct hdmi_vrr_data {
    hi_u32 vrr_mode_type;
};

struct hi_display_mode {
    struct hdmi_hdr_data hdr_data;
    struct hdmi_vrr_data vrr_data;
    struct hdmi_timing_data timing_data;
    hi_u32 reserved;
};

/* aud_codec */
#define HDMI_AUDIO_CODE_TYPE_PCM     1
#define HDMI_AUDIO_CODE_TYPE_AC3     2
#define HDMI_AUDIO_CODE_TYPE_MPEG1   3
#define HDMI_AUDIO_CODE_TYPE_MP3     4
#define HDMI_AUDIO_CODE_TYPE_MPEG2   5
#define HDMI_AUDIO_CODE_TYPE_AAC_LC  6
#define HDMI_AUDIO_CODE_TYPE_DTS     7
#define HDMI_AUDIO_CODE_TYPE_ATRAC   8
#define HDMI_AUDIO_CODE_TYPE_DSD     9
#define HDMI_AUDIO_CODE_TYPE_EAC3    10
#define HDMI_AUDIO_CODE_TYPE_DTS_HD  11
#define HDMI_AUDIO_CODE_TYPE_MAT     12
#define HDMI_AUDIO_CODE_TYPE_DST     13
#define HDMI_AUDIO_CODE_TYPE_WMA_PRO 14

/* aud_sample_size */
#define HDMI_AUDIO_SAMP_SIZE_UNKNOW 0
#define HDMI_AUDIO_SAMP_SIZE_8      8
#define HDMI_AUDIO_SAMP_SIZE_16     16
#define HDMI_AUDIO_SAMP_SIZE_18     18
#define HDMI_AUDIO_SAMP_SIZE_20     20
#define HDMI_AUDIO_SAMP_SIZE_24     24
#define HDMI_AUDIO_SAMP_SIZE_32     32

/* aud_input_type */
#define HDMI_AUDIO_SAMPLE_RATE_UNKNOW 0
#define HDMI_AUDIO_SAMPLE_RATE_8K     8000
#define HDMI_AUDIO_SAMPLE_RATE_11K    11025
#define HDMI_AUDIO_SAMPLE_RATE_12K    12000
#define HDMI_AUDIO_SAMPLE_RATE_16K    16000
#define HDMI_AUDIO_SAMPLE_RATE_22K    22050
#define HDMI_AUDIO_SAMPLE_RATE_24K    24000
#define HDMI_AUDIO_SAMPLE_RATE_32K    32000
#define HDMI_AUDIO_SAMPLE_RATE_44K    44100
#define HDMI_AUDIO_SAMPLE_RATE_48K    48000
#define HDMI_AUDIO_SAMPLE_RATE_88K    88200
#define HDMI_AUDIO_SAMPLE_RATE_96K    96000
#define HDMI_AUDIO_SAMPLE_RATE_176K   176400
#define HDMI_AUDIO_SAMPLE_RATE_192K   192000
#define HDMI_AUDIO_SAMPLE_RATE_768K   768000

/* aud_input_type */
#define HDMI_AUDIO_INPUT_TYPE_I2S   1
#define HDMI_AUDIO_INPUT_TYPE_SPDIF 2
#define HDMI_AUDIO_INPUT_TYPE_HBR   3

/* aud_channels */
#define HDMI_AUDIO_CHANNEL_2CH 2
#define HDMI_AUDIO_CHANNEL_3CH 3
#define HDMI_AUDIO_CHANNEL_4CH 4
#define HDMI_AUDIO_CHANNEL_5CH 5
#define HDMI_AUDIO_CHANNEL_6CH 6
#define HDMI_AUDIO_CHANNEL_7CH 7
#define HDMI_AUDIO_CHANNEL_8CH 8

struct ao_attr {
    hi_s32 aud_codec;
    hi_s32 aud_sample_size;
    hi_s32 aud_input_type;
    hi_s32 aud_sample_rate;
    hi_s32 aud_channels;
    hi_u8 channel_status[7]; /* array size 7 */
};

struct hi_hdmi_ext_intf {
    void *vo_data; /* Pointer to the vo interface operation */
    void *ao_data; /* Pointer to the ao interface operation */
};

/* vo set mode */
#define TIMING_SET_MODE (1 << 0)
#define HDR_SET_MODE    (1 << 1)
#define VRR_SET_MODE    (1 << 2)

#define DISPLAY_MODE_OK    0

#define HDMI_ERR_INPUT_PARAM_INVALID    -1
#define HDMI_ERR_MODULE_UNINIT          -2
#define HDMI_ERR_CABLE_PLUG_OUT         -3
#define HDMI_ERR_MODE_EXCEED_PORT20_CAP -4
#define HDMI_ERR_MODE_NOT_AVAIL         -5

/*
 * @attach: This callback function is used when bind a vo to a display interface
 * @dettach: This callback function is used when unbind a vo with a display
 *			 interface
 * @mode_validate: This called is used to validate the if the mode is supported
 *				   by the interface or not before setting the display mode.
 * @prepare: This callback should prepare the display interface for a subsequent
 *			 modeset
 * @mode_set: This callback is used to update the display mode of an display
 *			  interface.
 * @display_on: This callback is used to turn on the interface signal to the
 *				display
 * @display_off: This callback is used to turn off the interface signal to the
 *				display
 *
 */
struct hi_vo_intf_module_ops {
    hi_s32 (*attach)(hi_s32 display_interface_id, void *data);
    hi_s32 (*detach)(hi_s32 display_interface_id, void *data);
    hi_s32 (*mode_validate)(hi_s32 display_interface_id, hi_s32 mode, struct hi_display_mode *mode_data);
    hi_s32 (*prepare)(hi_s32 display_interface_id, hi_s32 mode, struct hi_display_mode *mode_data);
    hi_s32 (*mode_set)(hi_s32 display_interface_id, hi_s32 mode, struct hi_display_mode *mode_data);
    hi_s32 (*atomic_mode_set)(hi_s32 display_interface_id, hi_s32 mode, struct hi_display_mode *mode_data);
    hi_s32 (*display_on)(hi_s32 display_interface_id);
    hi_s32 (*display_off)(hi_s32 display_interface_id);
    /* void (*suspend)(pm_msg_t msg); */
    /* void (*resume)(pm_msg_t msg); */
};

/* HDMI notify define */
#define HDMI_BEFORE_OE_DISABLE 0x1
#define HDMI_AFTER_OE_DISABLE  0x2
#define HDMI_OE_ENABLE         0x3
#define HDMI_HOT_PLUG_IN       0x4
#define HDMI_HOT_PLUG_OUT      0x5

/* HDMI hpd detect status define */
#define HDMI_HPD_PLUGIN  1
#define HDMI_HPD_PLUGOUT 2

/*
 * @get_eld: This callback is used by the ao to get the sink's audio edid data
 * @hw_params_validate: This callback is used to check audio params validate
 * @hw_params: This callback is used to set the audio mode for hdmi
 * @digital_mute: This callback is used to mute/unmute the hdmi
 * @hpd_detect: This callback is used to get the hdp status, this is used when
 *              ao is init for the first time.
 * @register_notifier: This callback is used to register callback to process
 *              hdmi event, for example hotplug event.
 * @unregister_notifier: This callback is used to unregister callback to process
 *              hdmi event, for example hotplug event.
 */
struct hi_ao_intf_module_ops {
    /* get sink's audio capability */
    hi_s32 (*get_eld)(hi_s32 ao_intf_id, void *data, hi_u8 *buf, size_t len);
    /* check audio attr valid */
    hi_s32 (*hw_params_validate)(hi_s32 ao_intf_id, struct ao_attr *attr);
    /* set audio attr */
    hi_s32 (*hw_params)(hi_s32 ao_intf_id, struct ao_attr *attr);
    /* mute hdmi audio */
    hi_s32 (*digital_mute)(hi_s32 ao_intf_id, void *data, hi_bool enable);
    /* detect hdmi hotplug status */
    hi_s32 (*hpd_detect)(hi_s32 ao_intf_id, hi_u32 *status);
    /* used to received audio interface broadcast msg */
    hi_s32 (*register_notifier)(hi_s32 ao_intf_id, struct notifier_block *nb);
    hi_s32 (*unregister_notifier)(hi_s32 ao_intf_id, struct notifier_block *nb);
};

#ifndef MODULE

int drv_hdmi_mod_init(void);

void drv_hdmi_mod_exit(void);

#endif

#endif
