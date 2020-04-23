/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-11-22
*/

#ifndef __HI_DRV_PANEL_H__
#define __HI_DRV_PANEL_H__

#include "hi_disp_type.h"
#include "drv_panel_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    HI_DRV_PANEL_LINKTYPE_1LINK,
    HI_DRV_PANEL_LINKTYPE_2LINK,
    HI_DRV_PANEL_LINKTYPE_4LINK,
    HI_DRV_PANEL_LINKTYPE_8LINK,
    HI_DRV_PANEL_LINKTYPE_16LINK,
    HI_DRV_PANEL_LINKTYPE_MAX,
} hi_drv_panel_link_type;


typedef enum hi_drv_panel_flip_type {
    HI_DRV_PANEL_FLIP_TYPE_NONE,        /* < normal */
    HI_DRV_PANEL_FLIP_TYPE_FLIP,        /* < only flip */
    HI_DRV_PANEL_FLIP_TYPE_MIRROR,      /* < only mirror */
    HI_DRV_PANEL_FLIP_TYPE_FLIP_MIRROR, /* < flip and mirror  */
    HI_DRV_PANEL_FLIP_TYPE_MAX,
} hi_drv_panel_flip_type;

typedef struct {
    hi_u32 pixel_clk;     /* <60 000 000 ~ 300 000 000 */
    hi_u16 htotal;        /* <1280~ 5000 */
    hi_u16 hsync_fp;
    hi_u16 hsync_width;
    hi_u16 vtotal;        /* <720 ~ 3000 */
    hi_u16 vsync_fp;      /* <vsync fp */
    hi_u16 vsync_width;
} hi_drv_panel_timing;

typedef enum {
    HI_DRV_PANEL_TIMING_CHANGE_INSTANT,     /* normal mode change, instant change */
    HI_DRV_PANEL_TIMING_CHANGE_GRADUAL_VT,  /* vtotal change gradual */
    HI_DRV_PANEL_TIMING_CHANGE_GRADUAL_HT,  /* htotal change gradual */
    HI_DRV_PANEL_TIMING_CHANGE_GRADUAL_CLK, /* pixel_clk change gradual */
    HI_DRV_PANEL_TIMING_CHANGE_MAX,
} hi_drv_panel_timing_change_type;

typedef struct {
    hi_drv_panel_timing_change_type type;
    hi_u32 step;
    hi_bool mute;
    hi_u32 mute_time;
} hi_drv_panel_timing_change_info;

typedef struct {
    hi_bool hsync_output;
    hi_bool vsync_output;
    hi_bool hsync_negative;
    hi_bool vsync_negative;
} hi_drv_panel_timing_sync_info;


/** panel DIVISION type */
typedef enum {
    HI_DRV_PANEL_DIVISION_ONE_OE,     /* < DIVISION type is odd_even */
    HI_DRV_PANEL_DIVISION_ONE,        /* < DIVISION type is one */
    HI_DRV_PANEL_DIVISION_TWO,        /* < DIVISION type is two */
    HI_DRV_PANEL_DIVISION_FOUR,       /* < DIVISION type is four */
    HI_DRV_PANEL_DIVISION_EIGHT,      /* < DIVISION type is eight */
    HI_DRV_PANEL_DIVISION_SIXTEEN,    /* < DIVISION type is sixteen */
    HI_DRV_PANEL_DIVISION_MAX,        /* < invalid value */
} hi_drv_panel_division_type;

/*******************************************************************************
                                dim struct and enum
*******************************************************************************/
typedef enum {
    HI_DRV_PANEL_PWM_TYPE_PWM = 0x0,
    HI_DRV_PANEL_PWM_TYPE_PWM1D1,
    HI_DRV_PANEL_PWM_TYPE_PWM1D2,
    HI_DRV_PANEL_PWM_TYPE_PWM1D3,
    HI_DRV_PANEL_PWM_TYPE_LRSYNC,
    HI_DRV_PANEL_PWM_TYPE_LRGLASS,
    HI_DRV_PANEL_PWM_TYPE_BUTT,
} hi_drv_panel_pwm_type;

typedef struct {
    hi_u16 intf_on_delay;
    hi_u16 blacklight_on_delay;
    hi_u16 intf_off_delay;
    hi_u16 tcon_off_delay;
} hi_drv_panel_power_time_sequence;

typedef struct {
    hi_u8 port_use_cnt;
    hi_u8 pair_use_cnt;
    hi_u8 pair_pn_swap;
    hi_u8 reserved;
} hi_drv_panel_pair_info;


typedef struct {
    hi_u8 com_voltage;              /** panel pair com voltage */
    hi_u8 drv_current;              /** panel pair drv current */
    hi_u8 emphasis;                 /** panel pair emphasis */
    hi_u8 reserved;                 /** <1 byte reserved */
} hi_drv_panel_signal_attr;

typedef struct hi_drv_panel_spread {
    hi_u8  spread_enable;           /* <spread enable */
    hi_u8  reserved[3];             /* 3 is reserved */
    hi_u32 spread_ratio;            /* <spread ratio */
    hi_u32 spread_freq;             /* <spread frequency */
} hi_drv_panel_spread;

typedef struct {                                        /* for vbo/lvds panel vir */
    hi_char name[PANEL_NAME_MAX_LEN];                   /* panel name */
    hi_u16 width;                                       /* panel width */
    hi_u16 height;                                      /* panel height */
    hi_drv_panel_intf_type intf_type;                   /* panel interface type */
    hi_drv_panel_bit_depth data_bit_depth;              /* panel data bit width */
    hi_drv_panel_timing timing_50hz;                    /* panel 50_hz/100_hz timing info */
    hi_drv_panel_timing timing_60hz;                    /* panel 60_hz/120_hz timing info */
    hi_drv_panel_power_time_sequence  time_sequence;
    hi_drv_panel_division_type division_type;
    hi_drv_panel_pair_info pair_info;
    hi_drv_panel_signal_attr signal_attr;
    hi_drv_panel_lvds_attr lvds_attr;
    hi_drv_panel_timing_sync_info timing_sync_info;
    hi_drv_panel_spread spread_info;
    hi_drv_panel_3d_type type_3d;                   /* output 3d type */
    hi_drv_panel_bit_depth blacklight_bit_width;
    hi_drv_panel_range backlight_range;
    hi_u16 backlight_level;
    hi_u16 backlight_freq_50hz;
    hi_u16 backlight_freq_60hz;
    hi_u8 reserved0[14];                             /* 14 byte reserved */
} hi_drv_panel_info;

typedef enum {
    HI_DRV_PANEL_BYTE_NUM_3,                /** <pixel 3 byte */
    HI_DRV_PANEL_BYTE_NUM_4,                /** <pixel 3 byte */
    HI_DRV_PANEL_BYTE_NUM_5,                /** <pixel 5 byte */
    HI_DRV_PANEL_BYTE_NUM_MAX,              /** <Invalid value */
} hi_drv_panel_byte_num;

typedef enum {
    HI_DRV_PANEL_OSD_MODE_NORMAL,
    HI_DRV_PANEL_OSD_MODE_ARGB6888,
    HI_DRV_PANEL_OSD_MODE_MAX
} hi_drv_panel_osd_mode;

typedef struct {
    hi_bool enable;
    hi_u32 backlight;
} hi_drv_panel_dolby_info;

typedef struct {
    hi_bool is_3d;
    hi_bool is_left;
} hi_drv_panel_3d_info;

typedef struct {
    hi_u16 width;
    hi_u16 height;
    hi_u32 frame_rate;
    hi_bool vrr;
    hi_bool scene_change;
    hi_drv_panel_osd_mode osd_mode;
    hi_drv_panel_3d_info panel_3d_info;
    hi_drv_panel_dolby_info dolby_info;
} hi_drv_panel_disp_info;

typedef struct {
    hi_u16 width;
    hi_u16 height;
    hi_u32 frame_rate;
    hi_bool vrr_support;
    hi_drv_panel_3d_type type_3d;
    hi_drv_panel_timing  timing;
    hi_drv_panel_timing_change_info timing_change_info;
    hi_drv_panel_timing_sync_info   timing_sync_info;
    hi_drv_panel_intf_type intf_type;
    hi_drv_panel_byte_num  byte_num;
    hi_drv_panel_link_type link_type;
    hi_drv_panel_flip_type flip_type;
    hi_drv_panel_division_type division_type;
    hi_drv_panel_pair_info pair_info;
    hi_drv_panel_bit_depth bit_width;
} hi_drv_panel_cfg;

typedef enum hi_drvpanel_type_id {
    HI_DRV_PANEL_TYPE_ID_2DOD = 0x1000,
    HI_DRV_PANEL_TYPE_ID_PQ_INFO = 0x1003,
    HI_DRV_PANEL_TYPE_ID_LINEOD = 0x1005,
    HI_DRV_PANEL_TYPE_ID_VAC = 0x1006,
    HI_DRV_PANEL_TYPE_ID_RGBW = 0x1007,
    HI_DRV_PANEL_TYPE_ID_DEMURA = 0x1008,
    HI_DRV_PANEL_TYPE_ID_NORCTG_2D = 0x2000,
    HI_DRV_PANEL_TYPE_ID_NORCTG_3D = 0x2010,
    HI_DRV_PANEL_TYPE_ID_PANEL = 0x2020,
    HI_DRV_PANEL_TYPE_ID_COMBO = 0x2030,
    HI_DRV_PANEL_TYPE_ID_MLVDS = 0x2040,
    HI_DRV_PANEL_TYPE_ID_PINMUX = 0x2050,
    HI_DRV_PANEL_TYPE_ID_PHY = 0x2060,
    HI_DRV_PANEL_TYPE_ID_TCON_POS = 0x4000,
    HI_DRV_PANEL_TYPE_ID_PQBINPATH = 0xF000, /* to pq.bin patch, don't update  */
    HI_DRV_PANEL_TYPE_ID_MAX,
} hi_drv_panel_type_id;

typedef struct {
    hi_s32 (*panel_isr_main)(hi_drv_panel_disp_info *disp_info, hi_drv_panel_cfg *panel_config);
    hi_s32 (*panel_backlight_change)(hi_u32 out_frame_rate);
    hi_s32 (*panel_localdimming_regist)(hi_drv_disp_intf *intf, hi_bool enable);
    hi_s32 (*panel_change_mode)(hi_void);
} hi_drv_panel_export_func;

/*************************** API declaration ****************************/
hi_s32 hi_drv_panel_isr_main(hi_drv_panel_disp_info *disp_info, hi_drv_panel_cfg *panel_config);

hi_s32 hi_drv_panel_backlight_change(hi_u32 out_frm_rate);

hi_s32 hi_drv_panel_localdimming_regist(hi_drv_disp_intf *intf, hi_bool enable);

hi_s32 hi_drv_panel_change_mode(hi_void);

/* ==== API declaration end ==== */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
