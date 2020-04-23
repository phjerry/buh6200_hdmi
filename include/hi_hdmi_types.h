/*
  * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
  * Description: hdmi tx types definations for user/kernel communication
  * Author: hdmi sw team
  * Create: 2019-06-25
  */
#ifndef __HI_HDMI_TYPES_H__
#define __HI_HDMI_TYPES_H__

#include "hi_type.h"

/* hdcp mode */
#define HDCP_MODE_AUTO    0
#define HDCP_MODE_1X      1
#define HDCP_MODE_22      2
#define HDCP_MODE_23      3
#define HDCP_MODE_BUTT    4

#define HDCP_DEFAULT_REAUTH_TIMES  0xffffffff

/* hdcp version */
#define HDCP_VERSION_NONE      0
#define HDCP_VERSION_HDCP1X    1
#define HDCP_VERSION_HDCP22    2
#define HDCP_VERSION_HDCP23    3

enum hdcp_err_code {
    HDCP_ERR_UNDO = 0x00,         /* do not start hdcp. */
    HDCP_ERR_NONE,                /* no authentication error. */
    HDCP_ERR_PLUG_OUT,            /* cable plug out. */
    HDCP_ERR_NO_SIGNAL,           /* signal output disable. */
    HDCP_ERR_NO_KEY,              /* do not load key. */
    HDCP_ERR_INVALID_KEY,         /* key is invalid. */
    HDCP_ERR_DDC,                 /* DDC link error. */
    HDCP_ERR_ON_SRM,              /* on revocation list. */
    HDCP_ERR_NO_CAP,              /* no hdcp capability. */
    HDCP_1X_NO_CAP = 0x10,        /* no hdcp1x capability. */
    HDCP_1X_BCAP_FAIL,            /* read BCAP failed. */
    HDCP_1X_BSKV_FAIL,            /* read BKSV failed. */
    HDCP_1X_INTEGRITY_FAIL_R0,    /* R0(R0') integrety failure. */
    HDCP_1X_WATCHDOG_TIMEOUT,     /* Repeater watch dog timeout. */
    HDCP_1X_VI_CHCECK_FAIL,       /* Vi(Vi') check failed. */
    HDCP_1X_EXCEEDED_TOPOLOGY,    /* Exceeded toplogy. */
    HDCP_1X_INTEGRITY_FAIL_RI,    /* Ri(Ri') integrety failure. */
    HDCP_2X_NO_CAP = 0x20,        /* no hdcp2x capability. */
    HDCP_2X_SIGNATURE_FAIL,       /* signature error. */
    HDCP_2X_MISMATCH_H,           /* mismatch  H(H'). */
    HDCP_2X_AKE_TIMEOUT,          /* AKE timeout. */
    HDCP_2X_LOCALITY_FAIL,        /* locality check failed. */
    HDCP_2X_REAUTH_REQ,           /* REAUTH_REQ request. */
    HDCP_2X_WATCHDOG_TIMEOUT,     /* watchdog timeout. */
    HDCP_2X_V_MISMATCH,           /* mismatch  V(V'). */
    HDCP_2X_ROLL_OVER,            /* no roll-over of seq_num_V. */
    HDCP_2X_EXCEEDED_TOPOLOGY     /* Exceeded toplogy. */
};

/* sink info Colorimety Info struct */
typedef struct {
    hi_bool xvycc601;     /* Standard Definition Colorimetry based on IEC 61966-2-4 [5 ] */
    hi_bool xvycc709;     /* High Definition Colorimetry based on IEC 61966-2-4 [5 ] */
    hi_bool sycc601;      /* Colorimetry based on IEC 61966-2-1/Amendment 1 [34] */
    hi_bool adobe_ycc601; /* Colorimetry based on IEC 61966-2-5 [32], Annex A */
    hi_bool adobe_rgb;    /* Colorimetry based on IEC 61966-2-5 [32] */
    hi_bool bt2020_cycc;  /* Colorimetry based on ITU-R BT.2020 [39]  Y'cC'bcC'rc */
    hi_bool bt2020_ycc;   /* Colorimetry based on ITU-R BT.2020 [39]  Y'C'BC'R */
    hi_bool bt2020_rgb;   /* Colorimetry based on ITU-R BT.2020 [39]  R'G'B' */
    hi_bool dci_p3;       /* Colorimetry based on DCI-P3 [51][52] */
} hdmi_colorimetry;

/* edid manufacture info struct */
typedef struct {
    hi_u8 mfrs_name[4];   /* manufacture name */
    hi_u32 product_code;  /* product code */
    hi_u32 serial_number; /* serial numeber of manufacture */
    hi_u32 week;          /* the week of manufacture */
    hi_u32 year;          /* the year of manufacture */
    hi_u8 sinkname[14];   /* sink name */
} hdmi_manufacture_info;

/* edid latency capability,see <hdmi1.4b> page154 */
typedef struct {
    hi_bool present; /* latency info is valid. */
    hi_u32 p_video;  /* video latency for progressive video formats.in milliseconds */
    hi_u32 p_audio;  /* audio latency for progressive video formats.in milliseconds */
    hi_u32 i_video;  /* video latency for interlaced video formats.in milliseconds */
    hi_u32 i_audio;  /* audio latency for interlaced video formats.in milliseconds */
} hdmi_latency;

/* EDID Display Base Parameters struct */
typedef struct {
    hi_u8 max_image_width;  /* the disp image max width  (0~255)cm */
    hi_u8 max_image_height; /* the disp image max height (0~255)cm */
} disp_para;

struct eotf {
    hi_bool traditional_sdr; /* Traditional gamma - SDR Luminance Range */
    hi_bool traditional_hdr; /* Traditional gamma - HDR Luminance Range */
    hi_bool st2084_hdr;      /* SMPTE ST 2084 */
    hi_bool hlg;             /* Hybrid Log-Gamma */
};

/* HDR Dynamic Metadata Type */
struct dy_metadata {
    hi_bool d_type1_support;
    hi_u8 d_type1_version; /* type_1_hdr_metadata_version */
    hi_bool d_type2_support;
    hi_u8 d_type2_version; /* ts_103_433_spec_version */
    hi_bool d_type3_support;
    hi_bool d_type4_support;
    hi_u8 d_type4_version; /* type_4_hdr_metadata_version */
};

/* Static Metadata Type */
struct st_metadata {
    hi_bool s_type1;   /* Static Metadata Type 1 */
    hi_u8 max_lum_cv;  /* Desired Content Max Luminance Data. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    hi_u8 aver_lum_cv; /* Desired Content Max Frame-average Luminance. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    hi_u8 min_lum_cv;  /* Desired Content Min Luminance Data. Real value(in 1cd/m^2) = hi_u8MaxLuminance_CV * (hi_u8MinLuminance_CV/255)^2 / 100 */
};

typedef struct {
    struct eotf eotf;
    struct st_metadata st_metadata; /* Static Metadata Type */
    struct dy_metadata dy_metadata; /* HDR Dynamic Metadata Type */
} hdmi_hdr_info;

/* EDID Dolby capability in VSVDB version 0 */
typedef struct {
    hi_bool y422_36bit; /* support(true) or not support(false) a YUV422-12Bit dolby singal */
    hi_bool is_2160p60; /* capable of processing a max timming 3840X2160p60(true) /3840X2160p30(false) */
    hi_bool global_dimming; /* support(true) or not support(false) global dimming. */
    hi_u16 white_x; /* white point chromaticity coordinate x   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N), only support when hi_u8VSVDBVersion = 0. */
    hi_u16 white_y; /* white point  chromaticity coordinate y  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N), only support when hi_u8VSVDBVersion = 0. */
    hi_u16 dm_major_ver; /* the major version of display management implemented. only support when hi_u8VSVDBVersion = 0. */
    hi_u8 dm_minor_ver; /* the minor version of display management implemented. only support when hi_u8VSVDBVersion = 0. */
    hi_u16 target_min_pq; /* Perceptual quantization(PQ)-encoded value of minimum display luminance */
    hi_u16 target_max_pq; /* PQ-encoded value of maximum display luminance */
    hi_u16 red_x; /* red primary chromaticity coordinate x   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 red_y; /* red primary chromaticity coordinate y   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_x; /* green primary chromaticity coordinate x ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_y; /* green primary chromaticity coordinate y ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_x; /* blue primary chromaticity coordinate x  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_y; /* blue primary chromaticity coordinate y  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
} hdmi_dolby_v0;

/* EDID Dolby capability in VSVDB version 1 */
typedef struct {
    hi_bool y422_36bit;     /* support(true) or not support(false) a YUV422-12Bit dolby singal; For low-latency dolbyvision this flag is ingnored */
    hi_bool is_2160p60;     /* capable of processing a max timming 3840X2160p60(true) /3840X2160p30(false); For low-latency dolbyvision this bit maybe ingnored,relay on supported video format from the E-EDID */
    hi_bool global_dimming; /* support(true) or not support(false) global dimming. */
    hi_bool colorimetry;    /* this bit is valid only hi_u8VSVDBVersion = 1. 0: Dolby Vision HDMI sink's colorimetry is close to Rec.709, 1: EDR HDMI sink's colorimetry is close to P3, if Byte[9] to Byte[14] are present, ignores this bit. */
    hi_u8 dm_version;       /* 0:based on display management v2.x; 1:based on the video and blending pipeline v3.x; 2-7: reserved. only support when hi_u8VSVDBVersion = 1. */
    hi_u8 low_latency;      /* 0:supports only standard DolbyVison; 1: Supports low latency with 12-bit YCbCr 4:2:2 interface using the HDMI native 12-bit YCbCr4:2:2 pixel encoding and standard Dolby Vision interface; 2-3:reserved */
    hi_u16 target_min_lum;  /* minimum display luminance = (100+50*CV)cd/m2, where CV is the value */
    hi_u16 target_max_lum;  /* maximum display luminance = (CV/127)^2cd/m2, where CV is the value */
    hi_u16 red_x;           /* red primary chromaticity coordinate x   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 red_y;           /* red primary chromaticity coordinate y   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_x;         /* green primary chromaticity coordinate x ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_y;         /* green primary chromaticity coordinate y ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_x;          /* blue primary chromaticity coordinate x  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_y;          /* blue primary chromaticity coordinate y  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
} hdmi_dolby_v1;

/* EDID Dolby capability in VSVDB version 2 */
typedef struct {
    hi_bool y422_36bit;      /* support(true) or not support(false) a YUV422-12Bit dolby singal */
    hi_bool back_light_ctrl; /* supports Backlight Control */
    hi_bool global_dimming;  /* support(true) or not support(false) global dimming. */
    hi_u8 dm_version;        /* 0:based on display management v2.x; 1:based on the video and blending pipeline v3.x; 2-7: reserved. only support when hi_u8VSVDBVersion = 1. */
    hi_u8 back_lt_min_lum;   /* minimum luminance level 0:25cd/m2 1:50cd/m2 2:75cd/m2 3:100cd/m2 */
    hi_u8 interface;         /* 0:support only "low latency with YUV422" 1:support both "low latency with YUV422" and YUV444/RGB_10/12bit* 2:support both "standard DolbyVision" and "low latency with YUV422" 3:support "standard DolbyVision" "low latency YUV422 YUV444/RGB_10/12bit" */
    hi_u8 y444_rgb_30b36b;   /* 0:not support 1:support YUV444/RGB444_10bit 2:support YUV444/RGB444_12bit 3:reserved */
    hi_u16 target_min_pq_v2; /* maximum display luminance, in the PQ-encoded value= hi_u16TargetMinPQv2*20. A code value of 31 is approximately equivalent to 1cd/m2 */
    hi_u16 target_max_pq_v2; /* minimum display luminance, in the PQ-encoded value= 2055+hi_u16TargetMaxPQv2*65. */
    hi_u16 red_x;            /* red primary chromaticity coordinate x   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 red_y;            /* red primary chromaticity coordinate y   ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_x;          /* green primary chromaticity coordinate x ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 green_y;          /* green primary chromaticity coordinate y ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_x;           /* blue primary chromaticity coordinate x  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
    hi_u16 blue_y;           /* blue primary chromaticity coordinate y  ,bit[11:0]valid.Real value =SUM OF bit[N]*2^-(12-N) */
} hdmi_dolby_v2;

typedef struct {
    hi_bool support_v0;
    hi_bool support_v1;
    hi_bool support_v2;

    hdmi_dolby_v0 v0;
    hdmi_dolby_v1 v1;
    hdmi_dolby_v2 v2;
} hdmi_dolby_info;

typedef struct {
    hi_bool fva;              /* FVA */
    hi_bool cnm_vrr;          /* CNMVRR */
    hi_bool cinema_vrr;       /* CinemaVRR */
    hi_bool m_delta;          /* MDelta */
    hi_bool fapa_start_locat; /* FAPA_start_location */
    hi_bool allm;             /* ALLM */
    hi_u16 vrr_min;           /* VRRMin */
    hi_u16 vrr_max;           /* VRRMax */
} hdmi_vrr_info;

typedef struct {
    hi_bool fl_fr;
    hi_bool lfe;
    hi_bool fc;
    hi_bool rl_rr;
    hi_bool rc;
    hi_bool flc_frc;
    hi_bool rlc_rrc;
    hi_bool flw_frw;
    hi_bool flh_frh;
    hi_bool tc;
    hi_bool fch;
} hdmi_audio_speaker;

/* Short Audio Descriptors.see CTA-861-G 7.5.2 Audio Data Block */
typedef struct {
    hi_u8 fmt_code;      /* Audio Format Code */
    hi_u8 ext_code;      /* Audio Coding Extension Type Code */
    hi_u32 max_channel;  /* Max Number of channels.Audio Format Code 1 to 14;Audio Coding Extension Type Codes 4 to 6,8 to 10;Audio Coding Extension Type 0x0D (L-PCM 3D Audio), bits MC4:MC0 */
    hi_bool samp_32k;    /* 32   kHz */
    hi_bool samp_44p1k;  /* 44.1 kHz */
    hi_bool samp_48k;    /* 48   kHz */
    hi_bool samp_88p2k;  /* 88.2 kHz */
    hi_bool samp_96k;    /* 96   kHz */
    hi_bool samp_176p4k; /* 176.4kHz */
    hi_bool samp_192k;   /* 192 kHz */
    hi_bool width_16;    /* 16bit.Only to Audio Format Code = 1 (L-PCM) & Audio Extension Type Code 13 (L-PCM 3D Audio) */
    hi_bool width_20;    /* 20bit.Only to Audio Format Code = 1 (L-PCM) & Audio Extension Type Code 13 (L-PCM 3D Audio) */
    hi_bool width_24;    /* 24bit.Only to Audio Format Code = 1 (L-PCM) & Audio Extension Type Code 13 (L-PCM 3D Audio) */
    hi_u32 max_bit_rate; /* Maximum bit rate in Hz.Only to Audio Format Codes 2 to 8 */
    hi_u8 dependent;     /* Audio Format Code dependent value.Only to Audio Format Codes 9 to 13 */
    hi_u8 profile;       /* Profile.Only to Audio Format Codes 14 (WMA pro) */
    hi_bool len_1024_tl; /* 1024_TL.AAC audio frame lengths 1024_TL.Only to extension Type Codes 4 to 6 */
    hi_bool len_960_tl;  /* 960_TL. AAC audio frame lengths 960_TL.Only to extension Type Codes 4 to 6 */
    hi_bool mps_l;       /* MPS_L. Only to Extension Type Codes 8 to 10 */

} hdmi_audio_fmt;

#define MAX_AUDIO_INFO_CNT (0xe + 0x9)

typedef struct {
    hi_bool basic_audio; /* basic audio support.Basic Audio-Uncompressed, two channel, digital audio.e.g., 2 channel IEC 60958-3 [12] L-PCM, 32, 44.1, and 48 kHz sampling rates, 16 bits/sample. */
    hi_u32  audio_fmt_cnt; /* Short Audio Descriptors audio format support total number. */
    hdmi_audio_fmt sad[MAX_AUDIO_INFO_CNT]; /* Short Audio Descriptors audio format.see CTA-861-G 7.5.2 */
} hdmi_audio_info;

typedef struct {
    hi_u8 version;
    hi_u8 revision;
} hdmi_edid_version;

/* 3D video capibility definations */
#define HDMI_3D_BZ_NONE                  0
#define HDMI_3D_BZ_FRAME_PACKING         (1 << 1)
#define HDMI_3D_BZ_FIELD_ALTERNATIVE     (1 << 2)
#define HDMI_3D_BZ_LINE_ALTERNATIVE      (1 << 3)
#define HDMI_3D_BZ_SIDE_BY_SIDE_FULL     (1 << 4)
#define HDMI_3D_BZ_L_DEPTH               (1 << 5)
#define HDMI_3D_BZ_L_DEPTH_GFX_GFX_DEPTH (1 << 6)
#define HDMI_3D_BZ_TOP_AND_BOTTOM        (1 << 7)
#define HDMI_3D_BZ_SIDE_BY_SIDE_HALF     (1 << 8)

/* hdmi_detail_timing flag */
#define HDMI_TIMING_FLAG_NATIVE          (1 << 0)
#define HDMI_TIMING_FLAG_ALTERNATE_RATE  (1 << 1)

typedef enum {
    HDMI_PIC_ASP_NONE,
    HDMI_PIC_ASP_4_3,
    HDMI_PIC_ASP_16_9,
    HDMI_PIC_ASP_64_27,
    HDMI_PIC_ASP_256_135,
    HDMI_PIC_ASP_RESERVED,
} picture_aspect;



typedef struct hdmi_detail {
    hi_u32 pixel_clock;
    hi_bool progressive;
    hi_u32 field_rate;

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

    picture_aspect pic_asp_ratio;
    hi_u32 flag;
    hi_u32 mode_3d;
} hdmi_detail_timing;

typedef struct {
    hi_u32 vic;
    hdmi_detail_timing detail_timing;
    hi_u32 color_space;
} hdmi_avail_mode;

#endif /* __MPI_HDMI_H__ */
