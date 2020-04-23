/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: define for external use
* Author: vdp
* Create: 2019-06-28
 */

#ifndef __HI_DISP_TYPE_H__
#define __HI_DISP_TYPE_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_video.h"
#include "hi_drv_module.h"
#include "hi_drv_mipi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef hi_advca_function_release

#define hi_fatal_disp(fmt...) HI_FATAL_PRINT(HI_ID_DISP, fmt)
#define hi_err_disp(fmt...)   HI_ERR_PRINT(HI_ID_DISP, fmt)
#define hi_warn_disp(fmt...)  HI_WARN_PRINT(HI_ID_DISP, fmt)
#define hi_info_disp(fmt...)  HI_INFO_PRINT(HI_ID_DISP, fmt)
#else

#define hi_fatal_disp(fmt...)
#define hi_err_disp(fmt...)
#define hi_warn_disp(fmt...)
#define hi_info_disp(fmt...)
#endif

typedef struct {
    hi_s32 x;
    hi_s32 y;
    hi_s32 width;
    hi_s32 height;
} hi_rect;

typedef enum {
    HI_ZORDER_MOVETOP = 0,
    HI_ZORDER_MOVEUP,
    HI_ZORDER_MOVEBOTTOM,
    HI_ZORDER_MOVEDOWN,
    HI_ZORDER_BUTT
} hi_layer_zorder;

typedef enum {
    HI_DRV_DISP_FMT_NTSC  = 0,   /* (M)NTSC */
    HI_DRV_DISP_FMT_NTSC_J,   /* NTSC-J */
    HI_DRV_DISP_FMT_PAL_M,    /* M PAL, 525 lines */
    HI_DRV_DISP_FMT_SECAM_SIN, /* < SECAM_SIN */
    HI_DRV_DISP_FMT_SECAM_COS, /* < SECAM_COS */
    HI_DRV_DISP_FMT_NTSC_443, /* (M)PAL */
    HI_DRV_DISP_FMT_PAL_60,   /* 60 PAL */
    HI_DRV_DISP_FMT_1440x480I_60, /* sequnce:36 */
    HI_DRV_DISP_FMT_SECAM_L,   /* < France */
    HI_DRV_DISP_FMT_SECAM_B,   /* < Middle East */
    HI_DRV_DISP_FMT_SECAM_G,   /* < Middle East */
    HI_DRV_DISP_FMT_SECAM_D,   /* < Eastern Europe */
    HI_DRV_DISP_FMT_SECAM_K,   /* < Eastern Europe */
    HI_DRV_DISP_FMT_SECAM_H,   /* < Line SECAM:34 */

    HI_DRV_DISP_FMT_PAL = 0x10,    /* B D G H I PAL */
    HI_DRV_DISP_FMT_PAL_N,  /* N PAL, Jamaica/Uruguay */
    HI_DRV_DISP_FMT_PAL_NC, /* Nc PAL, Argentina:21 */
    HI_DRV_DISP_FMT_PAL_B,  /* B PAL， Australia */
    HI_DRV_DISP_FMT_PAL_B1, /* B1 PAL, Hungary */
    HI_DRV_DISP_FMT_PAL_D,  /* D PAL, China */
    HI_DRV_DISP_FMT_PAL_D1, /* D1 PAL， Poland */
    HI_DRV_DISP_FMT_PAL_G,  /* G PAL， Europe */
    HI_DRV_DISP_FMT_PAL_H,  /* H PAL， Europe */
    HI_DRV_DISP_FMT_PAL_K,  /* K PAL， Europe */
    HI_DRV_DISP_FMT_PAL_I,  /* I PAL，U.K. */
    HI_DRV_DISP_FMT_1440x576I_50,

    HI_DRV_DISP_FMT_480P_60 = 0x20, /* <480p 60 Hz */
    HI_DRV_DISP_FMT_576P_50, /* <576p 50 Hz */
    HI_DRV_DISP_FMT_720P_50, /* <720p 50 Hz */
    HI_DRV_DISP_FMT_720P_59_94,
    HI_DRV_DISP_FMT_720P_60, /* <720p 60 Hz */
    HI_DRV_DISP_FMT_1080I_50, /* <1080i 60 Hz */
    HI_DRV_DISP_FMT_1080I_59_94,
    HI_DRV_DISP_FMT_1080I_60, /* <1080i 60 Hz */

    HI_DRV_DISP_FMT_1080P_23_976 = 0x30,
    HI_DRV_DISP_FMT_1080P_24,     /* <1080p 24 Hz */
    HI_DRV_DISP_FMT_1080P_25,     /* <1080p 25 Hz */
    HI_DRV_DISP_FMT_1080P_29_97,
    HI_DRV_DISP_FMT_1080P_30,     /* <1080p 30 Hz */
    HI_DRV_DISP_FMT_1080P_50,     /* <1080p 50 Hz */
    HI_DRV_DISP_FMT_1080P_59_94,
    HI_DRV_DISP_FMT_1080P_60,
    HI_DRV_DISP_FMT_1080P_100,
    HI_DRV_DISP_FMT_1080P_119_88,
    HI_DRV_DISP_FMT_1080P_120,

    HI_DRV_DISP_FMT_3840X2160_23_976 = 0x40,
    HI_DRV_DISP_FMT_3840X2160_24, /* reserve for extention */
    HI_DRV_DISP_FMT_3840X2160_25,
    HI_DRV_DISP_FMT_3840X2160_29_97,
    HI_DRV_DISP_FMT_3840X2160_30,
    HI_DRV_DISP_FMT_3840X2160_50,
    HI_DRV_DISP_FMT_3840X2160_59_94,
    HI_DRV_DISP_FMT_3840X2160_60,
    HI_DRV_DISP_FMT_3840X2160_100,
    HI_DRV_DISP_FMT_3840X2160_119_88,
    HI_DRV_DISP_FMT_3840X2160_120,

    HI_DRV_DISP_FMT_4096X2160_23_976 = 0x50,
    HI_DRV_DISP_FMT_4096X2160_24,
    HI_DRV_DISP_FMT_4096X2160_25,
    HI_DRV_DISP_FMT_4096X2160_29_97,
    HI_DRV_DISP_FMT_4096X2160_30,
    HI_DRV_DISP_FMT_4096X2160_50,
    HI_DRV_DISP_FMT_4096X2160_59_94,
    HI_DRV_DISP_FMT_4096X2160_60,
    HI_DRV_DISP_FMT_4096X2160_100,
    HI_DRV_DISP_FMT_4096X2160_119_88,
    HI_DRV_DISP_FMT_4096X2160_120,

    HI_DRV_DISP_FMT_7680X4320_23_976 = 0x60,
    HI_DRV_DISP_FMT_7680X4320_24,
    HI_DRV_DISP_FMT_7680X4320_25,
    HI_DRV_DISP_FMT_7680X4320_29_97,
    HI_DRV_DISP_FMT_7680X4320_30,
    HI_DRV_DISP_FMT_7680X4320_50,
    HI_DRV_DISP_FMT_7680X4320_59_94,
    HI_DRV_DISP_FMT_7680X4320_60,
    HI_DRV_DISP_FMT_7680X4320_100,
    HI_DRV_DISP_FMT_7680X4320_119_88,
    HI_DRV_DISP_FMT_7680X4320_120,

    HI_DRV_DISP_FMT_1080P_24_FP = 0x90,
    HI_DRV_DISP_FMT_720P_60_FP,
    HI_DRV_DISP_FMT_720P_50_FP,

    HI_DRV_DISP_FMT_861D_640X480_60 = 0x98,
    HI_DRV_DISP_FMT_VESA_800X600_60,
    HI_DRV_DISP_FMT_VESA_1024X768_60,
    HI_DRV_DISP_FMT_VESA_1280X720_60,
    HI_DRV_DISP_FMT_VESA_1280X800_60,
    HI_DRV_DISP_FMT_VESA_1280X1024_60,
    HI_DRV_DISP_FMT_VESA_1360X768_60,  // Rowe
    HI_DRV_DISP_FMT_VESA_1366X768_60,
    HI_DRV_DISP_FMT_VESA_1400X1050_60,  // Rowe
    HI_DRV_DISP_FMT_VESA_1440X900_60,
    HI_DRV_DISP_FMT_VESA_1440X900_60_RB,
    HI_DRV_DISP_FMT_VESA_1600X900_60_RB,
    HI_DRV_DISP_FMT_VESA_1600X1200_60,
    HI_DRV_DISP_FMT_VESA_1680X1050_60,     // Rowe
    HI_DRV_DISP_FMT_VESA_1680X1050_60_RB,  // Rowe
    HI_DRV_DISP_FMT_VESA_1920X1080_60,
    HI_DRV_DISP_FMT_VESA_1920X1200_60,
    HI_DRV_DISP_FMT_VESA_1920X1440_60,
    HI_DRV_DISP_FMT_VESA_2048X1152_60,
    HI_DRV_DISP_FMT_VESA_2560X1440_60_RB,
    HI_DRV_DISP_FMT_VESA_2560X1600_60_RB, /* sequence:60 */

    // ADD
    HI_DRV_DISP_FMT_CUSTOM = 0xC0,
    HI_DRV_DISP_FMT_BUTT
} hi_drv_disp_fmt;

typedef enum {
    DISP_INTERRUPT_NONE = 0,
    DISP_INTERRUPT_D0_0_PERCENT = 0x10,
    DISP_INTERRUPT_D0_90_PERCENT = 0x20,
    DISP_INTERRUPT_D0_100_PERCENT = 0x40,
    DISP_INTERRUPT_D0_UFINT = 0x80,

    DISP_INTERRUPT_D1_0_PERCENT = 0x1,
    DISP_INTERRUPT_D1_90_PERCENT = 0x2,
    DISP_INTERRUPT_D1_100_PERCENT = 0x4,
    DISP_INTERRUPT_D1_UFINT = 0x8,

    DISP_INTERRUPT_WBCDHD_PARTFNI = 0x20000000,

    DISP_INTERRUPT_GP1_RES = 0x100,
    DISP_INTERRUPT_MC1_UFINT = 0x200,
    DISP_INTERRUPT_WBC_VP = 0x800,
    DISP_INTERRUPT_SMMU = 0x30000000,

    DISP_INTERRUPT_ALL = 0xFFFFFFFF,
} hi_drv_disp_interrupt_type;

/* display id */
typedef enum {
    HI_DRV_DISPLAY_0 = 0,
    HI_DRV_DISPLAY_1,
    HI_DRV_DISPLAY_2,
    HI_DRV_DISPLAY_BUTT
} hi_drv_display;

typedef enum {
    HI_DRV_DISP_LAYER_NONE = 0,
    HI_DRV_DISP_LAYER_VIDEO,
    HI_DRV_DISP_LAYER_GFX,
    HI_DRV_DISP_LAYER_BUTT
} hi_drv_disp_layer;

typedef enum {
    HI_DRV_DISP_STEREO_NONE = 0, /* 2D Mode */
    HI_DRV_DISP_STEREO_FRAME_PACKING,
    HI_DRV_DISP_STEREO_SBS_HALF,
    HI_DRV_DISP_STEREO_TAB,
    HI_DRV_DISP_STEREO_FIELD_ALT,
    HI_DRV_DISP_STEREO_LINE_ALT,
    HI_DRV_DISP_STEREO_SBS_FULL,
    HI_DRV_DISP_STEREO_L_DEPTH,
    HI_DRV_DISP_STEREO_L_DEPTH_G_DEPTH,
    HI_DRV_DISP_STEREO_MODE_BUTT
} hi_drv_disp_stereo_mode;

typedef enum {
    HI_DRV_DISP_ZORDER_MOVETOP = 0,
    HI_DRV_DISP_ZORDER_MOVEUP,
    HI_DRV_DISP_ZORDER_MOVEBOTTOM,
    HI_DRV_DISP_ZORDER_MOVEDOWN,
    HI_DRV_DISP_ZORDER_BUTT
} hi_drv_disp_zorder;

typedef enum {
    HI_DRV_DISP_INTF_TYPE_HDMI,
    HI_DRV_DISP_INTF_TYPE_LCD,
    HI_DRV_DISP_INTF_TYPE_BT1120,
    HI_DRV_DISP_INTF_TYPE_BT656,
    HI_DRV_DISP_INTF_TYPE_YPBPR,
    HI_DRV_DISP_INTF_TYPE_RGB,
    HI_DRV_DISP_INTF_TYPE_CVBS,
    HI_DRV_DISP_INTF_TYPE_SVIDEO,
    HI_DRV_DISP_INTF_TYPE_VGA,
    HI_DRV_DISP_INTF_TYPE_MIPI,
    HI_DRV_DISP_INTF_TYPE_ALL,

    HI_DRV_DISP_INTF_TYPE_MAX
} disp_intf_type;

typedef struct {
    hi_u8 dac_y;
    hi_u8 dac_pb;
    hi_u8 dac_pr;
} hi_drv_disp_intf_ypbpr;

typedef struct {
    hi_u8 dac_r;
    hi_u8 dac_g;
    hi_u8 dac_b;
    hi_bool sync;
} hi_drv_disp_intf_rgb;

typedef struct {
    hi_u8 dac_r;
    hi_u8 dac_g;
    hi_u8 dac_b;
} hi_drv_disp_intf_vga;

typedef struct {
    hi_u8 dac_cvbs;
} hi_drv_disp_intf_cvbs;

typedef struct {
    hi_u8 dac_y;
    hi_u8 dac_c;
} hi_drv_disp_intfvideo;

typedef enum {
    HI_DRV_DISP_BT1120_0,
    HI_DRV_DISP_BT1120_BUTT,
} hi_drv_disp_bt1120;

typedef enum {
    HI_DRV_HDMI_ID_0 = 0,
    HI_DRV_HDMI_ID_1,
    HI_DRV_HDMI_ID_BUTT
} hi_drv_disp_hdmi;

typedef enum {
    HI_DRV_DISP_BT656_0,
    HI_DRV_DISP_BT656_BUTT,
} hi_drv_disp_bt656;

typedef enum {
    HI_DRV_DISP_LCD_0,
    HI_DRV_DISP_LCD_BUTT,
} hi_drv_disp_lcd;

typedef struct {
    hi_drv_mipi_id mipi_id;
    hi_drv_mipi_mode mipi_mode;
} hi_drv_disp_intf_mipi;

typedef struct {
    disp_intf_type intf_type;
    union {
        hi_drv_disp_hdmi hdmi;
        hi_drv_disp_bt1120 bt1120;
        hi_drv_disp_bt656 bt656;
        hi_drv_disp_lcd lcd;

        hi_drv_disp_intf_ypbpr ypbpr;
        hi_drv_disp_intf_rgb rgb;
        hi_drv_disp_intf_vga vga;
        hi_drv_disp_intf_cvbs cvbs;
        hi_drv_disp_intfvideo svideo;
        hi_drv_disp_intf_mipi mipi;
    } un_intf;
} hi_drv_disp_intf;

#define INTF_NUM_MAX 8
typedef struct {
    hi_u32 number;
    hi_drv_disp_intf intf[INTF_NUM_MAX];
} disp_intf_link;

typedef enum {
    HI_DRV_DISP_VDAC_NONE = 0,
    HI_DRV_DISP_VDAC_CVBS,
    HI_DRV_DISP_VDAC_Y,
    HI_DRV_DISP_VDAC_PB,
    HI_DRV_DISP_VDAC_PR,
    HI_DRV_DISP_VDAC_SV_Y,
    HI_DRV_DISP_VDAC_SV_C,
    HI_DRV_DISP_VDAC_R,
    HI_DRV_DISP_VDAC_G,
    HI_DRV_DISP_VDAC_B,
    HI_DRV_DISP_VDAC_G_NOSYNC,
    HI_DRV_DISP_VDAC_SIGNAL_BUTT
} hi_drv_disp_vdac_signal;

typedef enum {
    HI_DRV_DISP_INTF_YPBPR0 = 0,
    HI_DRV_DISP_INTF_RGB0,
    HI_DRV_DISP_INTF_SVIDEO0,
    HI_DRV_DISP_INTF_CVBS0,
    HI_DRV_DISP_INTF_VGA0,

    HI_DRV_DISP_INTF_HDMI0,
    HI_DRV_DISP_INTF_HDMI1,

    HI_DRV_DISP_INTF_MIPI0,
    HI_DRV_DISP_INTF_MIPI1,

    HI_DRV_DISP_INTF_BT656_0,

    HI_DRV_DISP_INTF_BT1120_0,

    HI_DRV_DISP_INTF_LCD0,

    HI_DRV_DISP_INTF_ID_MAX
} hi_drv_disp_intf_id;

typedef struct {
    hi_drv_disp_intf_id id;
    hi_u8 dac_y_g;
    hi_u8 dac_pb_b;
    hi_u8 dac_pr_r;
    hi_drv_mipi_mode mipi_mode;
    hi_bool dac_sync;
} disp_intf;

typedef struct {
    hi_u32 vfb;
    hi_u32 vbb;
    hi_u32 vact;
    hi_u32 hfb;
    hi_u32 hbb;
    hi_u32 hact;
    hi_u32 vpw;
    hi_u32 hpw;
    hi_bool idv;
    hi_bool ihs;
    hi_bool ivs;
    hi_bool clkreversal;

    hi_u32 pix_freq;
    hi_u32 refresh_rate;
} hi_drv_disp_timing;

typedef enum  {
    HI_DRV_DISP_CAP_TIMING_SD,
    HI_DRV_DISP_CAP_TIMING_FHD,
    HI_DRV_DISP_CAP_TIMING_4K_60HZ,
    HI_DRV_DISP_CAP_TIMING_8K_60HZ,
    HI_DRV_DISP_CAP_TIMING_8K_120HZ,
    HI_DRV_DISP_CAP_TIMING_MAX
} hi_drv_disp_cap_timing;

typedef struct  {
    hi_bool        sdr;
    hi_bool        hdr10;
    hi_bool        hdr10_plus;
    hi_bool        hlg;
    hi_bool        dolby_vision;
    hi_bool        dolby_low_latency;
} hi_drv_disp_cap_hdr;

typedef enum {
    HI_DRV_DISP_PIXEL_RGB = 0,
    HI_DRV_DISP_PIXEL_YUV422,
    HI_DRV_DISP_PIXEL_YUV444,
    HI_DRV_DISP_PIXEL_YUV420,
    HI_DRV_DISP_PIXEL_MAX
} hi_drv_disp_pix_format;

#define DISP_CHANNEL_MAX_NUMBER  2
#define DISP_INTF_MAX_NUMBER  8

typedef struct  {
    hi_bool support;
    hi_drv_disp_cap_timing  timing_cap;
    hi_drv_disp_cap_hdr    hdr_cap;
} hi_drv_disp_capability;

typedef struct {
    hi_bool enable;
    hi_u32 vrr_min;
    hi_u32 vrr_max;
} hi_drv_disp_vrr;

typedef struct {
    hi_bool enable;
    hi_u32 refresh_rate;
} hi_drv_disp_qms;

typedef struct {
    hi_bool enable;
    hi_u32 factor;
} hi_drv_disp_qft;

typedef struct {
    hi_drv_disp_fmt disp_fmt;
    hi_u32 vic_num;
    hi_drv_disp_stereo_mode disp_3d_mode;
    hi_bool interlace;
    hi_u32 pix_repeat;
    hi_drv_aspect_ratio aspect_ratio;
    hi_drv_disp_timing timing;
} hi_drv_disp_static_timing;

typedef struct {
    hi_drv_disp_vrr vrr_cfg;
    hi_drv_disp_qms qms_cfg;
    hi_drv_disp_qft qft_cfg;
    hi_bool qms_enable;
    hi_u32 qft_factor;
    hi_bool allm_en;
} hi_drv_disp_dynamic_timing;

typedef struct {
    hi_drv_disp_static_timing static_timing;
    hi_drv_disp_dynamic_timing dynamic_timing;
} hi_drv_disp_timing_cfg;

typedef struct {
    hi_drv_disp_intf    intf;
    hi_drv_pixel_bitwidth bit_width;
    hi_drv_disp_pix_format pixel_fmt;
    hi_bool enable;
} hi_drv_disp_intf_output;

typedef struct {
    hi_drv_disp_fmt format;
    hi_drv_disp_stereo_mode disp_3d_mode;
    hi_drv_disp_timing custom_timing;
    hi_drv_disp_intf_output intf_output[DISP_INTF_MAX_NUMBER];
    hi_u32 number;
} hi_drv_disp_format_param;

typedef struct {
    hi_u8 red;
    hi_u8 green;
    hi_u8 blue;
    hi_drv_pixel_bitwidth bit_depth;
} hi_drv_disp_color;

typedef struct {
    hi_u32 leftoffset;
    hi_u32 topoffset;
    hi_u32 rightoffset;
    hi_u32 bottomoffset;
} hi_drv_disp_crop;

typedef struct {
    hi_u32 left;   /* left offset */
    hi_u32 top;    /* top offset */
    hi_u32 right;  /* right offset */
    hi_u32 bottom; /* bottom offset */
} hi_drv_disp_offset;

typedef struct {
    hi_drv_display disp;
    hi_drv_disp_fmt format;
} hi_drv_disp_isogeny_attr;

typedef enum {
    HI_DRV_DISP_TYPE_NORMAL, /* 普通信号显示 BT601 BT709 */
    HI_DRV_DISP_TYPE_HDR10,      /* 标准HDR信号显示 */
    HI_DRV_DISP_TYPE_HDR10PLUS,
    HI_DRV_DISP_TYPE_HLG,
    HI_DRV_DISP_TYPE_DOLBY,      /* DolbyVision信号显示 */
    HI_DRV_DISP_TYPE_DOLBY_LL,               /* DolbyVision low latency 信号显示 */
    HI_DRV_DISP_TYPE_SDR_CERT,   /* SDR输出认证时需要 */
    HI_DRV_DISP_TYPE_HDR10_CERT,
    HI_DRV_DISP_TYPE_TECHNICOLOR,
    HI_DRV_DISP_TYPE_BUTT
} hi_drv_disp_out_type;

typedef enum {
    DRV_XDR_ENGINE_AUTO = 0x0,
    DRV_XDR_ENGINE_SDR,
    DRV_XDR_ENGINE_HDR10,
    DRV_XDR_ENGINE_HLG,
    DRV_XDR_ENGINE_SLF,
    DRV_XDR_ENGINE_DOLBY,
    DRV_XDR_ENGINE_JTP,

    DRV_XDR_ENGINE_BUTT
} hi_drv_disp_xdr_engine;

typedef struct {
    hi_bool supporthdr10;
    hi_bool supporthlg;
    hi_bool supportslf;
    hi_bool supportdolby;
    hi_bool supportjtp;
} drv_disp_xdr_hardware_cap;

typedef struct {
    hi_drv_disp_out_type disp_type; /* 显示类型 */

} hi_drv_disp_hdr_attr;

typedef enum {
    HI_DRV_DISP_COLOR_SPACE_BT709,
    HI_DRV_DISP_COLOR_SPACE_BT601,
    HI_DRV_DISP_COLOR_SPACE_BT2020,
    HI_DRV_DISP_COLOR_SPACE_BUTT
} hi_drv_disp_output_color_space;

#define HI_DRV_DISP_VSVD_DATA_MAX_SIZE (256)

typedef struct {
    hi_bool support_bt601;
    hi_bool support_bt709;
    hi_bool support_bt2020;

    hi_bool support_sdr;
    hi_bool support_hdr10;
    hi_bool support_hdr10plus;
    hi_bool support_hlg;
    hi_bool support_dolby_vision;
    hi_bool support_dolby_low_latency;
    hi_u8   vsvdb_data[HI_DRV_DISP_VSVD_DATA_MAX_SIZE];
}hi_drv_disp_sink_capability;

typedef struct {
    hi_drv_disp_out_type actual_output_type;
    hi_drv_disp_output_color_space actual_output_colorspace;
} hi_drv_disp_output_status;

typedef struct {
    hi_rect video_rect;
    hi_rect output_rect;
    hi_bool sync_enable;
    hi_u32 handle;
} hi_drv_disp_gfx_sync_info;

typedef enum {
    HI_DRV_DISP_HDR_PRIORITY_VIDEO = 0, /**< video and graphic tomapping to video   luminance. */
    HI_DRV_DISP_HDR_PRIORITY_GRAPHIC,   /**< video and graphic tomapping to graphic luminance. */
    HI_DRV_DISP_HDR_PRIORITY_AUTO,      /**< video and graphic tomapping automatically according to scene.  Not support currently. */
    HI_DRV_DISP_HDR_PRIORITY_MAX,
} hi_drv_disp_hdr_priority_mode;

typedef enum {
    HI_DRV_DISP_MATCH_CONTENT_MODE_NONE = 0,     /* disable HDR match content function */
    HI_DRV_DISP_MATCH_CONTENT_MODE_DEFAULT,      /* only single window and be set full screen*/
    HI_DRV_DISP_MATCH_CONTENT_MODE_MAX,
} hi_drv_disp_match_content_mode;

typedef struct {
    hi_bool enable;
    hi_bool master;
    hi_bool slave;

    hi_u32 vline;
    hi_u32 last_vline;
    hi_u64 last_systime;
    hi_drv_disp_fmt fmt;
    hi_rect virtaul_screen;

    hi_drv_disp_stereo_mode stereo;
    hi_bool righteye_first;
    hi_bool interlace;
    hi_bool bottom_field;
    hi_rect fmt_resolution;
    hi_rect pixel_fmt_resolution;
    hi_drv_aspect_ratio aspect_ratio;
    hi_u32 refresh_rate;

    hi_drv_disp_vrr vrr_cfg;
    hi_drv_disp_qms qms_cfg;
    hi_drv_disp_qft qft_cfg;
    hi_bool allm_en;

    hi_drv_disp_offset offset_info;
    hi_u32 alpha;

    hi_drv_color_descript color_space;
    hi_drv_disp_out_type disp_type;
    hi_drv_disp_output_color_space color_space_mode;
    hi_drv_disp_match_content_mode match_mode;
} hi_disp_display_info;

typedef struct {

    hi_u32 vline;  /*current running line*/
    hi_u32 int_cnt; /*int count*/
    hi_u32 circle_time_us;/*one circle time, */
    hi_u32 left_time; /*end of cycle time */
    hi_bool btm;    /*field */
} hi_drv_disp_timing_status;

/*************** ip protect ***************/
// macrovision
typedef enum {
    HI_DRV_DISP_MACROVISION_TYPE0,
    HI_DRV_DISP_MACROVISION_TYPE1,
    HI_DRV_DISP_MACROVISION_TYPE2,
    HI_DRV_DISP_MACROVISION_TYPE3,
    HI_DRV_DISP_MACROVISION_CUSTOMER,
    HI_DRV_DISP_MACROVISION_BUTT
} hi_drv_disp_macrovision;

// cgms-a
typedef enum {
    HI_DRV_DISP_CGMSA_A = 0x00,
    HI_DRV_DISP_CGMSA_B,
    HI_DRV_DISP_CGMSA_TYPE_BUTT
} hi_drv_disp_cgmsa_type;

typedef enum {
    HI_DRV_DISP_CGMSA_COPY_FREELY = 0,
    HI_DRV_DISP_CGMSA_COPY_NO_MORE = 0x01,
    HI_DRV_DISP_CGMSA_COPY_ONCE = 0x02,
    HI_DRV_DISP_CGMSA_COPY_NEVER = 0x03,

    HI_DRV_DISP_CGMSA_MODE_BUTT
} hi_drv_disp_cgmsa_mode;

typedef struct {
    hi_bool enable;
    hi_drv_disp_cgmsa_type type;
    hi_drv_disp_cgmsa_mode mode;
} hi_drv_disp_cgmsa_cfg;

/*************** about vbi ***************/
typedef enum {
    HI_DRV_DISP_VBI_TTX = 0,
    HI_DRV_DISP_VBI_CC,
    HI_DRV_DISP_VBI_VCHIP,
    HI_DRV_DISP_VBI_WSS,
    HI_DRV_DISP_VBI_TYPE_BUTT
} hi_drv_disp_vbi_type;

typedef struct {
    hi_drv_disp_vbi_type type;
    hi_u32 in_buffer_size;
    hi_u32 work_buffer_size;
} hi_drv_disp_vbi_cfg;

typedef struct {
    hi_u8 *data_addr;
    hi_u32 data_len;
} hi_drv_disp_ttx_data;

typedef struct {
    hi_drv_disp_vbi_type type;
    hi_u8 *data_addr;
    hi_u32 data_len;
} hi_drv_disp_vbi_data;

typedef struct {
    hi_drv_disp_vbi_type type;
    hi_u32 data_addr;
    hi_u32 data_len;
} hi_drv_disp_compat_vbi_data;

typedef struct {
    hi_bool enable;
    hi_u16 data;
} hi_drv_disp_wss_data;

#define CAST_BUFF_MAX_NUM 16

typedef struct {
    /* frame config */
    hi_drv_pixel_format format; /* support ... */
    hi_u32 width;
    hi_u32 height;

    /* buffer config */
    hi_u32 buf_number; /* not more than hi_disp_mirror_buffer_max_number */

    hi_bool user_alloc; /* true: user alloc buffers; false: disp alloc buffers */
    hi_bool low_delay;

    hi_u32 buf_size;   /* every buffer size in byte */
    hi_u32 buf_stride; /* only for 'buseralloc = true' */

    hi_bool crop;
    hi_drv_crop_rect crop_rect;
    hi_u32 buf_phy_addr[CAST_BUFF_MAX_NUM]; /* only for 'buseralloc = true' */
} hi_drv_disp_cast_cfg;

typedef enum {
    HI_DRV_DISP_C_TYPE_NONE = 0,
    // HI_DRV_DISP_C_SHOW_MODE,
    HI_DRV_DISP_C_INTPOS_0_PERCENT,
    HI_DRV_DISP_C_INTPOS_90_PERCENT,
    HI_DRV_DISP_C_INTPOS_100_PERCENT,
    HI_DRV_DISP_C_DHD0_WBC,
    HI_DRV_DISP_C_VID_WBC,
    HI_DRV_DISP_C_GFX_WBC,
    HI_DRV_DISP_C_REG_UP,
    HI_DRV_DISP_C_SMMU,
    HI_DRV_DISP_C_RETURN_FRAME_ISR,
    HI_DRV_DISP_C_LOWBANDWIDTH,
    HI_DRV_DISP_C_FRAME_FINISH,
    HI_DRV_DISP_C_FRAME_TIMEOUT0,
    HI_DRV_DISP_C_FRAME_TIMEOUT1,
    HI_DRV_DISP_C_FRAME_RXERR,
    HI_DRV_DISP_C_TYPE_BUTT
} hi_drv_disp_callback_type;

typedef enum {
    HI_DRV_DISP_C_EVET_NONE = 0,
    HI_DRV_DISP_C_PREPARE_CLOSE,
    HI_DRV_DISP_C_OPEN,
    HI_DRV_DISP_C_PREPARE_TO_PEND = 0x10,
    HI_DRV_DISP_C_RESUME,
    HI_DRV_DISP_C_DISPLAY_SETTING_CHANGE = 0x20,
    HI_DRV_DISP_C_VT_INT = 0x100,
    HI_DRV_DISP_C_EVENT_BUTT
} hi_drv_disp_callback_event;

typedef enum {
    HI_DRV_DISP_FIELD_PROGRESSIVE = 0,
    HI_DRV_DISP_FIELD_TOP,
    HI_DRV_DISP_FIELD_BOTTOM,
    HI_DRV_DISP_FIELD_FLAG_BUTT
} hi_drv_disp_field_flag;

typedef struct {
    hi_drv_disp_callback_event event_type;
    hi_drv_disp_field_flag field;

    hi_disp_display_info disp_info;
} hi_drv_disp_callback_info;

typedef struct {
    hi_s32 width;
    hi_s32 height;
} hi_drv_disp_cast_attr;

typedef enum {
    HI_DRV_DISP_CALLBACK_PRIORTY_HIGH = 0,
    HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE,
    HI_DRV_DISP_CALLBACK_PRIORTY_LOW,
    HI_DRV_DISP_CALLBACK_PRIORTY_BUTT

} hi_drv_disp_callback_priorty;

typedef struct {
    hi_void (*pf_disp_callback)(hi_void *hdst, const hi_drv_disp_callback_info *cb_info);
    hi_drv_disp_callback_priorty callback_prior;
    hi_void *hdst;
} hi_drv_disp_callback;

typedef struct {
    hi_bool video_logo_mode;
    hi_drv_pixel_format pixel_fmt;

    hi_u32 logo_width;
    hi_u32 logo_height;

    hi_u32 logo_y_stride;
    hi_u32 logo_cb_stride;
    hi_u32 logo_cr_stride;

    hi_u32 logo_y_addr;
    hi_u32 logo_cb_addr;
    hi_u32 logo_cr_addr;

} hi_drv_disp_video_logo_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
