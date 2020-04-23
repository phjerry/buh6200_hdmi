/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver connector header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __DRV_HDMITX_CONNECTOR_H__
#define __DRV_HDMITX_CONNECTOR_H__

#include "hi_osal.h"
#include "drv_hdmitx_modes.h"
#include "hi_hdmi_types.h"

struct hdmi_timing_data;

#define INT_AON_NAME_LEN           20

#define CONNECTOR_TYPE_Unknown     0
#define CONNECTOR_TYPE_VGA         1
#define CONNECTOR_TYPE_DVII        2
#define CONNECTOR_TYPE_DVID        3
#define CONNECTOR_TYPE_DVIA        4
#define CONNECTOR_TYPE_Composite   5
#define CONNECTOR_TYPE_SVIDEO      6
#define CONNECTOR_TYPE_LVDS        7
#define CONNECTOR_TYPE_Component   8
#define CONNECTOR_TYPE_9PinDIN     9
#define CONNECTOR_TYPE_DisplayPort 10
#define CONNECTOR_TYPE_HDMIA       11
#define CONNECTOR_TYPE_HDMIB       12
#define CONNECTOR_TYPE_TV          13
#define CONNECTOR_TYPE_eDP         14
#define CONNECTOR_TYPE_VIRTUAL     15
#define CONNECTOR_TYPE_DSI         16
#define CONNECTOR_TYPE_DPI         17
#define CONNECTOR_TYPE_WRITEBACK   18

/* max tmds clock , in kHz */
#define HDMI14_MAX_TMDS_CLK 340000
#define HDMI20_MAX_TMDS_CLK 600000

/* hdmi mode define */
#define HDMI_MODE_DVI 0
#define HDMI_MODE_14  1
#define HDMI_MODE_20  2
#define HDMI_MODE_21  3

/* EDID source type */
#define EDID_EMPTY        0
#define EDID_FROM_SINK    1
#define EDID_DEFAULT_DVI  2
#define EDID_DEFAULT_HDMI 3
#define EDID_DEBUG        4

struct hdmi_hpd_cfg {
    hi_bool fillter_en;
    hi_u32 high_reshold; /* in ms */
    hi_u32 low_reshold;  /* in ms */
};

/* connectror status */
struct connector_status {
    hi_bool hw_hotplug;
    hi_bool hw_rxsens;
    hi_u32 hw_hpd_cnt;
    hi_u32 hw_unhpd_cnt;
    hi_u32 hw_rsen_cnt;
    hi_u32 hw_unrsen_cnt;
    hi_u32 edid_src_type; /* see EDID source type */

    /* for software debug */
    hi_bool sw_hotplug;
    hi_bool sw_hpd_en; /* enable software simulate hotplug.when true ,ingore hardware hotplug. */
};

/* quirk config */
struct quirk_config {
};

struct hdr_property {
    struct eotf eotf;
    struct st_metadata st_metadata; /* Static Metadata Type */
    struct dy_metadata dy_metadata; /* HDR Dynamic Metadata Type */
};

struct scdc_property {
    hi_bool present;     /* SCDC_Present */
    hi_bool rr_capable;  /* RR_capable */
    hi_bool lte_340mcsc; /* LTE_340Mcsc_Scramble */
    hi_bool version;     /* source & sink version */
};

struct dsc_property {
    hi_bool dsc_1p2;         /* DSC_1p2 */
    hi_bool y420;            /* DSC_Native_420 */
    hi_bool all_bpp;         /* DSC_ALL_bpp */
    hi_bool dsc_16bpc;       /* DSC_16bpc */
    hi_bool dsc_12bpc;       /* DSC_12bpc */
    hi_bool dsc_10bpc;       /* DSC_10bpc */
    hi_u8 dsc_max_rate;      /* DSC_Max_FRL_Rate */
    hi_u8 max_slice;         /* DSC_MaxSlices */
    hi_u8 total_chunk_bytes; /* DSC_TotalChunkBytes */
};

struct vrr_property {
    hi_bool fva;              /* FVA */
    hi_bool cnm_vrr;          /* CNMVRR */
    hi_bool cinema_vrr;       /* CinemaVRR */
    hi_bool m_delta;          /* MDelta */
    hi_bool fapa_start_locat; /* FAPA_start_location */
    hi_bool allm;             /* ALLM */
    hi_u16 vrr_min;           /* VRRMin */
    hi_u16 vrr_max;           /* VRRMax */
};

/* see HDMI14b SPEC Table 8-16 HDMI-LLC Vendor-Specific Data Block (HDMI VSDB) */
struct color_depth {
    hi_bool rgb_24; /* no define in VSDB.default true */
    hi_bool rgb_30; /* DC_30bit */
    hi_bool rgb_36; /* DC_36bit */
    hi_bool rgb_48; /* DC_48bit */

    hi_bool y444_24; /* no define in VSDB.if ycbcr444 is true ,set true. */
    hi_bool y444_30; /* DC_30bit && DC_Y444 */
    hi_bool y444_36; /* DC_36bit && DC_Y444 */
    hi_bool y444_48; /* DC_36bit && DC_Y444 */

    hi_bool y420_24; /* no define in HF-VSDB.if ycbcr420 is true,set true */
    hi_bool y420_30; /* DC_420_30 */
    hi_bool y420_36; /* DC_420_36 */
    hi_bool y420_48; /* DC_420_48 */

    hi_u32 bpc; /* Maximum bits per color channel. Used by HDMI and DP outputs. */
};

/* see CTA-861-G Table 40 CEA Extension Version 2 (deprecated) */
struct color_format {
    /* no define in VSDB.default true */
    hi_bool rgb;
    /* If byte 3 of bit 5 (YCBCR 4:4:4) = 0b1 , in CTA Extension Version 2/3,set true. */
    hi_bool ycbcr444;
    /* If byte 3 of bit 4 (YCBCR 4:2:2) = 0b1 , in CTA Extension Version 2/3,set true. */
    hi_bool ycbcr422;
    /* If a YCC420-VIC present, set true.see YCC420 Capability Map Data Block or YCC420 Video Data Block in CTA-861-G */
    hi_bool ycbcr420;
};

/* see CTA-861-G Table 70 Colorimetry Data Block */
struct colorimetry {
    hi_bool xvycc601;     /* Standard Definition Colorimetry based on IEC 61966-2-4 [5 ] */
    hi_bool xvycc709;     /* High Definition Colorimetry based on IEC 61966-2-4 [5 ] */
    hi_bool sycc601;      /* Colorimetry based on IEC 61966-2-1/Amendment 1 [34] */
    hi_bool adobe_ycc601; /* Colorimetry based on IEC 61966-2-5 [32], Annex A */
    hi_bool adobe_rgb;    /* Colorimetry based on IEC 61966-2-5 [32] */
    hi_bool bt2020_cycc;  /* Colorimetry based on ITU-R BT.2020 [39]  Y'cC'bcC'rc */
    hi_bool bt2020_ycc;   /* Colorimetry based on ITU-R BT.2020 [39]  Y'C'BC'R */
    hi_bool bt2020_rgb;   /* Colorimetry based on ITU-R BT.2020 [39]  R'G'B' */
    hi_bool dci_p3;       /* Colorimetry based on DCI-P3 [51][52] */
};

/* see CTG-861-G Table 74 */
struct quantization {
    /*
     * @rgb_qs_selecable
     * If the Sink declares a selectable RGB Quantization  Range (QS=1;rgb_qs==true) then it shall expect Limited Range
     * pixel values if it receives Q=1 (in AVI) and it shall expect Full Range pixel values if it receives Q=2.
     * For other values of Q,the Sink shall expect pixel values with the default range for the transmitted Video Format
     * (Limited Range when receiving a CE Video Format and a Full Range when receiving an IT format).
     */
    hi_bool rgb_qs_selecable;

    /*
     * @ycc_qy_selecable
     * If the Sink declares a selectable YCC Quantization Range (QY=1;ycc_qy==true), then it shall expect Limited Range
     * pixel values if it receives AVI YQ=0 and it shall expect Full Range pixel values if it receives AVI YQ=1.
     * For other values of YQ,the Sink shall expect pixel values with the default range for the transmitted Video Format
     * (Limited Range when receiving a CE Video Format and a Full Range when receiving an IT format)
     */
    hi_bool ycc_qy_selecable;
};

struct color_property {
    struct color_depth depth;         /* color depth */
    struct color_format format;       /* color format(Pixel Encoding format) */
    struct colorimetry colorimetry;   /* colorimetry */
    struct quantization quantization; /* quantization */
};

#define MAX_SAD_AUDIO_CNT (0x9 + 0xe)

/* Short Audio Descriptors.see CTA-861-G 7.5.2 Audio Data Block */
struct sad_fmt_audio {
    hi_u8 fmt_code;      /* Audio Format Code */
    hi_u8 ext_code;      /* Audio Coding Extension Type Code */
    /*
     * Max Number of channels.Audio Format Code 1 to 14;
     * Audio Coding Extension Type Codes 4 to 6,8 to 10;
     * Audio Coding Extension Type 0x0D (L-PCM 3D Audio), bits MC4:MC0
     */
    hi_u32 max_channel;
    hi_bool samp_32k;    /* 32   kHz */
    hi_bool samp_44p1k;  /* 44.1 kHz */
    hi_bool samp_48k;    /* 48   kHz */
    hi_bool samp_88p2k;  /* 88.2 kHz */
    hi_bool samp_96k;    /* 96   kHz */
    hi_bool samp_176p4k; /* 176.4kHz */
    hi_bool samp_192k;   /* 192 kHz */
    hi_bool width_16;    /* 16bit.Only to Audio Format Code = 1(L-PCM) & Audio Extension Type Code 13(L-PCM 3D Audio) */
    hi_bool width_20;    /* 20bit.Only to Audio Format Code = 1(L-PCM) & Audio Extension Type Code 13(L-PCM 3D Audio) */
    hi_bool width_24;    /* 24bit.Only to Audio Format Code = 1(L-PCM) & Audio Extension Type Code 13(L-PCM 3D Audio) */
    hi_u32 max_bit_rate; /* Maximum bit rate in Hz.Only to Audio Format Codes 2 to 8 */
    hi_u8 dependent;     /* Audio Format Code dependent value.Only to Audio Format Codes 9 to 13 */
    hi_u8 profile;       /* Profile.Only to Audio Format Codes 14 (WMA pro) */
    hi_bool len_1024_tl; /* 1024_TL.AAC audio frame lengths 1024_TL.Only to extension Type Codes 4 to 6 */
    hi_bool len_960_tl;  /* 960_TL. AAC audio frame lengths 960_TL.Only to extension Type Codes 4 to 6 */
    hi_bool mps_l;       /* MPS_L. Only to Extension Type Codes 8 to 10 */
};

struct audio_property {
    /*
     * @basic :basic audio support.
     * Basic Audio¡ª Uncompressed, two channel, digital audio.
     * e.g., 2 channel IEC 60958-3 [12] L-PCM, 32, 44.1, and 48 kHz sampling rates, 16 bits/sample.
     */
    hi_bool basic;
    /* @sad_count:Short Audio Descriptors audio format support total number. */
    hi_u32 sad_count;
    /* @sad :see Short Audio Descriptors. */
    struct sad_fmt_audio sad[MAX_SAD_AUDIO_CNT];
};

struct audio_in {
    struct hdmi_timing_mode mode;
    hi_u32 bpc; /* 8 10 12 */
    hi_u32 pixel_format;
    hi_u32 audio_rate;
    hi_u32 packet_type;
    hi_u32 layout;
    hi_u32 acat;
};

struct latency_property {
    hi_bool latency_present; /* latency present */
    hi_u32 p_video;          /* progressive  video_Latency,in milliseconds */
    hi_u32 p_audio;          /* progresive Audio_Latency,in milliseconds */
    hi_u32 i_video;          /* Interlaced_Video_Latency,in milliseconds */
    hi_u32 i_audio;          /* Interlaced_Audio_Latency,in milliseconds */
};

struct chromaticity {
    hi_u16 red_x;
    hi_u16 red_y;
    hi_u16 green_x;
    hi_u16 green_y;
    hi_u16 blue_x;
    hi_u16 blue_y;
    hi_u16 white_x;
    hi_u16 white_y;
};

struct vendor_info {
    hi_u8 mfc_name[4];  /* ID Manufacturer Name size 4 */
    hi_u8 product_code; /* ID Product Code */
    hi_u32 serial_num;  /* ID Serial Number */
    hi_u32 mfc_year;    /* Year of Manufacture */
    hi_u32 mfc_week;    /* Week of Manufacture */
};

/*
 * struct drm_hdmi_info - runtime information about the connected HDMI sink
 * Describes if a given display supports advanced HDMI 2.0 features.
 * This information is available in CEA-861-F extension blocks (like HF-VSDB).
 */
struct hdmi_hdmi_info {
    /* @y420_cmdb_map: bitmap of SVD index, to extraxt vcb modes */
    hi_u64 y420_cmdb_map;

    /* @y420_dc_modes: bitmap of deep color support index */
    /* hi_u8 y420_dc_modes; */
};

/*
 * struct hdmi_display_info - runtime data about the connected sink
 * Describes a given display (e.g. CRT or flat panel) and its limitations. For
 * fixed display sinks like built-in panels there's not much difference between
 * this and &struct drm_connector. But for sinks with a real cable this
 * structure is meant to describe all the things at the other end of the cable.
 * For sinks which provide an EDID this can be filled out by calling drm_add_edid_modes.
 */
struct hdmi_display_info {
    /* @width_cm: Physical width in cm */
    hi_u32 width_cm;

    /* @height_cm: Physical height in cm */
    hi_u32 height_cm;

    /* @dvi_dual: Dual-link DVI sink? */
    hi_bool dvi_dual;

    /* @has_hdmi_infoframe: Does the sink support the HDMI infoframe? */
    hi_bool has_hdmi_infoframe;

    /* @cea_rev: CEA revision of the HDMI sink */
    hi_u8 cea_rev;

    hi_bool force_max_tmds; /* when dvi,force max_tmds_clk to 340MHz */

    hi_u32 max_tmds_clock; /* Max_TMDS_Charater_Rate,in kHz. */

    struct hdmi_hdmi_info hdmi;

    hi_u8 ffe_level; /* source's FFE level.default 3.no define in edid. */

    hi_u8 max_frl_rate; /* Max_FRL_Rate */
};

struct base_property {
    hi_u8 version;
    hi_u8 revision;
    hi_u8 ext_block_num;
    struct vendor_info vendor;
    struct chromaticity chromaticity;
};

/* see Table 3.19-Standard Timings in VESA ENHANCED EXTENDED DISPLAY IDENTIFICATION DATA STANDARD (September 25,2006) */
struct standard_timing {
    hi_u32 h_active;
    hi_u32 v_active;
    hi_u32 refresh_rate;
};

#define MAX_Y420_ONLY_CNT 128
#define MAX_Y420_CMDB_CNT 128
#define MAX_VIC_CNT       0xff
#define MAX_STD_CNT       32
#define MAX_DTD_CNT       32
#define MAX_ESTAB_CNT     32

struct timing_property {
    hi_u32 native_vic;
    hi_u32 total_cnt;              /* total vic count, fillter the same vic in hi_vesa & svd & y420's vic */
    hi_u32 total_vic[MAX_VIC_CNT]; /* total vic, fillter the same vic in hi_vesa & svd & y420's vic */

    hi_u32 hi_vesa_cnt;
    hi_u32 svd_cnt;
    hi_u32 y420_only_cnt;
    hi_u32 y420cmdb_cnt;
    hi_u32 svr_cnt;
    hi_u32 estabblished_cnt;
    hi_u32 dtds_cnt;
    hi_u32 std_cnt;

    hi_u32 svd_vic[MAX_VIC_CNT];
    /* Established Timing or Standard Timings or  Detailed Timing  conver to hisilicon's self-define vic */
    hi_u32 hi_vesa_vic[MAX_VIC_CNT];
    /*
     * vic which can support ycbcr420output only (not normal RGB/YCBCR444/422 outputs).
     * There are total 107 VICs defined by CEA-861-F spec, so the size is 128 bits to mapupto 128 VICs;
     */
    hi_u32 y420_only_vic[MAX_Y420_ONLY_CNT];
    /*
     * YCBCR4:2:0 Capability Map Data Block.modes which can support ycbcr420 output also,along with normal HDMI outputs.
     * There are total 107 VICs defined by CEA-861-F spec, so the size is 128 bits to map upto 128 VICs;
     */
    hi_u32 y420_cmdb_vic[MAX_Y420_CMDB_CNT];
    /* Short Video Reference .see CTA-861-G Table 83  Video Format Preference Data Block */
    hi_u32 svr_vic[MAX_VIC_CNT];
    /*
     * Established Timings or Manufacturer's Timings.
     * see VESA ENHANCED EXTENDED DISPLAY IDENTIFICATION DATA STANDARD
     * (September 25, 2006) Table 3.18 - Established Timings I & II
     */
    hi_u32 estabblished[MAX_ESTAB_CNT];
    struct standard_timing std[MAX_STD_CNT]; /* Standard Timings */
};

/* EDID Dolby capability in VSVDB version 0 */
struct dolby_v0 {
    hi_bool y422_36bit; /* support(true) or not support(false) a YUV422-12Bit dolby singal */
    hi_bool is_2160p60; /* capable of processing a max timming 3840X2160p60(true) /3840X2160p30(false) */
    hi_bool global_dimming; /* support(true) or not support(false) global dimming. */
    /*
     * white point chromaticity coordinate x,
     * bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N), only support when u8VSVDBVersion = 0.
     */
    hi_u16 white_x;
    /*
     * white point chromaticity coordinate y,
     * bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N), only support when u8VSVDBVersion = 0.
     */
    hi_u16 white_y;
    hi_u16 dm_major_ver; /* the major version of display management implemented. only support when u8VSVDBVersion = 0 */
    hi_u8 dm_minor_ver; /* the minor version of display management implemented. only support when u8VSVDBVersion = 0. */
    hi_u16 target_min_pq; /* Perceptual quantization(PQ)-encoded value of minimum display luminance */
    hi_u16 target_max_pq; /* PQ-encoded value of maximum display luminance */
    hi_u16 red_x; /* red primary chromaticity coordinate x   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 red_y; /* red primary chromaticity coordinate y   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_x; /* green primary chromaticity coordinate x ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_y; /* green primary chromaticity coordinate y ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_x; /* blue primary chromaticity coordinate x  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_y; /* blue primary chromaticity coordinate y  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
};

/* EDID Dolby capability in VSVDB version 1 */
struct dolby_v1 {
    /*
     * support(true) or not support(false) a YUV422-12Bit dolby singal;
     * For low-latency dolbyvision this flag is ingnored
     */
    hi_bool y422_36bit;
    /*
     * capable of processing a max timming 3840X2160p60(true) /3840X2160p30(false);
     * For low-latency dolbyvision this bit maybe ingnored,relay on supported video format from the E-EDID
     */
    hi_bool is_2160p60;
    hi_bool global_dimming; /* support(true) or not support(false) global dimming. */
    /*
     * this bit is valid only u8VSVDBVersion = 1.
     * 0: Dolby Vision HDMI sink's colorimetry is close to Rec.709,
     * 1: EDR HDMI sink's colorimetry is close to P3, if Byte[9] to Byte[14] are present, ignores this bit.
     */
    hi_bool colorimetry;
    /*
     * 0:based on display management v2.x; 1:based on the video and blending pipeline v3.x;
     * 2-7: reserved. only support when u8VSVDBVersion = 1.
     */
    hi_u8 dm_version;
    /*
     * 0:supports only standard DolbyVison;
     * 1: Supports low latency with 12-bit YCC422 interface using
     * the HDMI native 12-bit YCC422 pixel encoding and standard Dolby Vision interface; 2-3:reserved
     */
    hi_u8 low_latency;
    hi_u16 target_min_lum; /* minimum display luminance = (100+50*CV)cd/m2, where CV is the value */
    hi_u16 target_max_lum; /* maximum display luminance = (CV/127)^2cd/m2, where CV is the value */
    hi_u16 red_x; /* red primary chromaticity coordinate x,bit[11:0]valid.Real value=SUM OF bit[N]*2^-(12-N) */
    hi_u16 red_y; /* red primary chromaticity coordinate y,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_x; /* green primary chromaticity coordinate x,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_y; /* green primary chromaticity coordinate y,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_x; /* blue primary chromaticity coordinate x,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_y; /* blue primary chromaticity coordinate y,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
};

/* EDID Dolby capability in VSVDB version 2 */
struct dolby_v2 {
    /* support(true) or not support(false) a YUV422-12Bit dolby singal */
    hi_bool y422_36bit;
    /* supports Backlight Control */
    hi_bool back_light_ctrl;
    /* support(true) or not support(false) global dimming. */
    hi_bool global_dimming;
    /*
     * 0:based on display management v2.x;
     * 1:based on the video and blending pipeline v3.x;
     * 2-7: reserved. only support when u8VSVDBVersion = 1.
     */
    hi_u8 dm_version;
    /* minimum luminance level 0:25cd/m2 1:50cd/m2 2:75cd/m2 3:100cd/m2 */
    hi_u8 back_lt_min_lum;
    /*
     * 0:support only "low latency with YUV422"
     * 1:support both "low latency with YUV422" and YUV444/RGB_10/12bit
     * 2:support both "standard DolbyVision" and "low latency with YUV422"
     * 3:support "standard DolbyVision" "low latency YUV422 YUV444/RGB_10/12bit"
     */
    hi_u8 interface;
    /* 0:not support 1:support YUV444/RGB444_10bit 2:support YUV444/RGB444_12bit 3:reserved */
    hi_u8 y444_rgb_30b36b;
    /* minimum display luminance, in the PQ-encoded value= 2055+u16TargetMaxPQv2*65. */
    hi_u16 target_min_pq_v2;
    /*
     * maximum display luminance, in the PQ-encoded value= u16TargetMinPQv2*20.
     * A code value of 31 is approximately equivalent to 1cd/m2
     */
    hi_u16 target_max_pq_v2;
    /* red primary chromaticity coordinate x   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 red_x;
    /* red primary chromaticity coordinate y   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 red_y;
    /* green primary chromaticity coordinate x ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_x;
    /* green primary chromaticity coordinate y ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_y;
    /* blue primary chromaticity coordinate x  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_x;
    /* blue primary chromaticity coordinate y  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_y;
};

struct dolby_property {
    hi_bool support_v0;
    hi_bool support_v1;
    hi_bool support_v2;
    struct dolby_v0 v0;
    struct dolby_v1 v1;
    struct dolby_v2 v2;
};

struct src_hdmi {
    /* tmds */
    hi_bool dvi_support;
    hi_bool hdmi_support;
    hi_u32 max_tmds_clock; /* in kHz */

    /* csc */
    hi_bool rgb2yuv;
    hi_bool yuv2rgb;
    hi_bool ycbcr420;
    hi_bool ycbcr422;
    hi_bool ycbcr444;
    hi_bool dither_support;

    /* scdc */
    hi_bool scdc_present;
    hi_bool scdc_lte_340mcsc;

    /* color depth */
    hi_bool bpc_30;
    hi_bool bpc_36;
    hi_bool bpc_48;

    /* frl */
    hi_u8 max_frl_rate;
    hi_u8 ffe_levels;

    /* dsc */
    hi_bool dsc_support;
    hi_u8 max_dsc_frl_rate;
    hi_bool native_y420;            /* DSC support native 420 */
    hi_bool dsc_10bpc;              /* DSC support 10bpc */
    hi_bool dsc_12bpc;              /* DSC support 12bpc */
    hi_u8 max_silce_count;          /* DSC max slice count */
    hi_u32 max_pixel_clk_per_slice; /* DSC max pixel clk per slice */
};

struct source_capability {
    hi_bool hdcp14_support;
    hi_bool hdcp2x_support;
    hi_bool cec_support;
    struct src_hdmi hdmi;
    struct hdmi_hpd_cfg hpd;
};

enum hdmi_rsen_status {
    RSEN_DISCONNECT,
    RSEN_CONNECT,
    RSEN_DET_FAIL
};

enum hdmi_connector_status {
    HPD_DETECTING = 0,
    HPD_PLUGIN = 1,
    HPD_PLUGOUT = 2,
    HPD_DET_FAIL = 3
};

struct hdmi_connector {
    struct hisilicon_hdmi *hdmi_dev;
    hi_char *irq_name;
    hi_bool poll_hpd;
    /* Lock for general connector state */
    osal_mutex mutex;

    /** @priv: priv pointer */
    void *priv;

    /** @name: human readable name, can be overwritten by the driver */
    hi_s8 *name;

    /* one of the CONNECTOR_TYPE_<foo> types from drv_hdmi_connector.h */
    hi_s32 connector_type;

    hi_bool user_dvi_mode;

    /*
     * struct hdmi_available_mode
     * Modes available on this connector (from fill_modes() + user).
     * Protected by &hdmi_connector.mutex.
     */
    struct osal_list_head valid_modes;

    /*
     * These are modes added by probing with DDC or the BIOS, before
     * filtering is applied. Used by the probe helpers. Protected by
     * &drm_mode_config.mutex.
     */
    struct osal_list_head probed_modes;

    hi_s32 probed_mode_valid_cnt;

    hi_s32 probed_mode_cnt;

    hi_u32 detail_vic_base;

    hi_s32 edid_src_type;
    /*
     * One of the hdmi_connector_status enums (connected, not, or unknown).
     * Protected by &hdmi_connector.mutex.
     */
    enum hdmi_connector_status status;

    enum hdmi_connector_status hotplug;

#define MAX_ELD_BYTES 128
    /* EDID-like data, if present */
    hi_u8 eld[MAX_ELD_BYTES];

    struct edid *edid_raw;

    hi_s32 edid_size;

    struct hdmi_display_info display_info;

    /* base property of the connected sink */
    struct base_property base;

    /* timing property of the connected sink */
    struct timing_property timing;

    /* color property of the connected sink */
    struct color_property color;

    /* vrr property of the connected sink */
    struct vrr_property vrr;

    /* hdr property of the connected sink */
    struct hdr_property hdr;

    /* dolby property of the connected sink */
    struct dolby_property dolby;

    /* frl property of the connected sink */
    struct scdc_property scdc;

    /* dsc property of the connected sink */
    struct dsc_property dsc;

    /* audio property of the connected sink */
    struct audio_property audio;

    /* latency property of the connected sink */
    struct latency_property latency;

    struct source_capability src_cap;

    hi_u64 quirk;

    struct quirk_config quick_cfg;
};

struct hdmi_connector *hdmi_connector_init(struct hisilicon_hdmi *hdmi);

void hdmi_connector_deinit(struct hisilicon_hdmi *hdmi);

void hdmi_connector_enable_hpd(struct hisilicon_hdmi *hdmi,
                               struct hdmi_connector *connector);

/*
 * hdmi_connector_get_edid - get EDID data, if available
 * @connector: hdmi connector we're probing
 * @ddc: ddc operation struct
 * Poke the given I2C channel to grab EDID data if possible.  If found,
 * attach it to the connector.
 * Return: Pointer to valid EDID or NULL if we couldn't find any.
 */
struct edid *hdmi_connector_get_edid(struct hdmi_connector *connector, struct hdmi_ddc *ddc);

hi_s32 hdmi_connector_edid_reset(struct hdmi_connector *connector);

/*
 * hdmi_connector_mode_validate - validate the specified display mode
 * @connector: connector we're probing
 * @mode: display mode to be validated
 * Return: true if the specified mode is supported or otherwise false.
 */
hi_bool hdmi_connector_mode_validate(struct hdmi_connector *connector, struct hdmi_timing_data *timing);

/*
 * hdmi_audio_caps - Ability to calculate audio support
 * @pst_audio_in: Audio parameter input
 * return:true if the current format supports this audio transfer.
 */
hi_bool hdmi_audio_caps(struct audio_in *pst_audio_in);

hi_void drv_hdmitx_connector_create_force_mode(struct hdmi_connector *connector, struct hdmi_mode_config *mode);

/*
 * hdmi_connector_search_mode - search hdmi mode from display mode, if available
 * @connector: connector we're probing
 * @mode: display mode
 * iterate the connector's mode list, and find out the specified display
 * mode's matching hdmi mode, hdmi mode is used internally in hdmi and
 * display mode is used to communicate with vo.
 * hdmi mode includes frl/dsc/scdc setting of the specified display mode.
 * Return: The target display mode or NULL if the display mode is invalid.
 */
hi_bool hdmi_connector_search_mode(struct hdmi_connector *connector, struct hdmi_mode_config *mode);

/* delete modes which tmds clock */
hi_s32 hdmi_connector_delete_modes(struct hdmi_connector *connector, struct hdmi_mode_config *mode);

hi_s32 hdmi_connector_get_availmode(struct hdmi_connector *connector,
                                    hdmi_avail_mode *mode_buf, hi_u32 cnt);

hi_s32 hdmi_connector_get_native_mode(struct hdmi_connector *connector, hdmi_avail_mode *avail_mode);

hi_s32 hdmi_connector_get_max_mode(struct hdmi_connector *connector, hdmi_avail_mode *avail_mode);

void hdmi_connector_set_debug_edid(struct hdmi_connector *connector, hi_u8 *edid, hi_s32 size);

enum hdmi_rsen_status hdmi_connector_get_rsen(struct hisilicon_hdmi *hdmi);

void hdmi_set_hpd_irq_mask(struct hisilicon_hdmi *hdmi, hi_bool mask);

#endif /* __DRV_HDMITX_CONNECTOR_H__ */

