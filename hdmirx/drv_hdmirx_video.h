/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of video functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_VIDEO_H__
#define __DRV_HDMIRX_VIDEO_H__

#include "hi_type.h"
#include "hal_hdmirx_comm.h"
#include "hi_drv_hdmirx.h"

#define DC_CLK_8BPP_1X          0x00
#define DC_CLK_8BPP_2X          0x02
#define DC_CLK_10BPP_1X         0x04
#define DC_CLK_12BPP_1X         0x05
#define DC_CLK_10BPP_2X         0x06
#define DC_CLK_12BPP_2X         0x07
#define LAST_KNOWN_HDMI_VIC     0
#define LAST_KNOWN_VIC          102  /* 59 for cea-861d, 64 for cea-861e 92 for cea-861f */
#define HDMI_3D_RESOLUTION_MASK 0x80
#define NMB_OF_HDMI_VIDEO_MODES LAST_KNOWN_HDMI_VIC

/* range limits to satisfy all 861D video modes */
#define VIDEO_MIN_V_HZ          22
#define VIDEO_MAX_V_HZ          243
#define VIDEO_MIN_H_KHZ         14
#define VIDEO_MAX_H_KHZ         134
#define VIDEO_MAX_PIX_CLK_10MHZ 60    /* to support 300 m_hz resolutions */
#define FPIX_TOLERANCE   700 /* 400 */ /* in 10 k_hz units, i.e. 100 means +-1_m_hz */
#define PIXELS_TOLERANCE 30  /* should be no more then 55 to distinguish all CEA861C modes */
#define LINES_TOLERANCE  3   /* should be no more then 29 to distinguish all CEA861C modes */
#define H_FREQ_TOLERANCE 2   /* H freq tolerance in k_hz, used for analog video detection */
#define FH_TOLERANCE     4   /* H freq in 1 k_hz units, used for range check */
#define FV_TOLERANCE     2   /* V freq in 1 hz units, used for range check */
#define ACTIVE_TOLERANCE 1

#define V_ACTIVE_480     480
#define V_ACTIVE_576     576
#define V_ACTIVE_720     720
#define V_ACTIVE_1080    1080

#define V_TOTAL_480      525
#define V_TOTAL_576      625
#define V_TOTAL_720      750
#define V_TOTAL_1080     1125

#define MODE_CHANGE_V_TOTAL_THR         2
#define MODE_CHANGE_H_TOTAL_THR         5
#define H_V_TOTAL_STABLE_TOLERANCE      100
#define H_V_TOTAL_IN_RANGE_TOLERANCE     100
#define H_V_ACTIVE_IN_RANGE_TOLERANCE     100

/* repetition factor */
#define RP1  0x01 /* x1 (no repetition) */
#define RP2  0x02 /* x2 (doubled) */
#define RP4  0x04 /* x4 */
#define RP5  0x08 /* x5 */
#define RP7  0x10 /* x7 */
#define RP8  0x20 /* x8 */
#define RP10 0x40 /* x10 */

#define PROG 0 /* progressive scan */
#define INTL 1 /* interlaced scan */
#define POS  0 /* positive pulse */
#define NEG  1 /* negative pulse */

#define NTSC 1 /* NTSC system (60_hz) */
#define PAL  2 /* PAL system (50_hz) */

#define PASS_REPLICATED_PIXELS_MASK 0x80 /* if true- pixel clock is multiplied by pixel repeatition rate */

#define MODE_STABLE_CNT_THR 20

#define VIDEO_FRAME_RATE_LIMIT 120

#define abs_diff(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

enum {
    MODE_CHG_HVRES = 1,
    MODE_CHG_PIXCLK = 2,
    MODE_CHG_BUTT
};
typedef struct {
    hi_u16 h; /* number of horizontal pixels */
    hi_u16 v; /* number of vertical pixels */
} pixel_type;

typedef struct {
    hi_u32 vic4x3;           /* CEA VIC for 4:3 picture aspect rate, 0 if not avaliable */
    hi_u32 vic16x9;          /* CEA VIC for 16:9 picture aspect rate, 0 if not avaliable */
    hi_u32 hdmi_vic;         /* HDMI VIC for 16:9 picture aspect rate, 0 if not avaliable */
    pixel_type active;       /* number of active pixels */
    pixel_type total;        /* total number of pixels */
    pixel_type blank;        /* number of blank pixels */
    pixel_type sync_offset;  /* front porch */
    pixel_type sync_width;   /* width of sync pulse */
    hi_u32 h_freq;           /* in k_hz */
    hi_u32 v_freq;           /* in hz */
    hi_u32 pix_freq;         /* in m_hz */
    hi_u32 pix_clk;          /* in 10k_hz units */
    hi_bool interlaced;      /* true for interlaced video */
    hi_bool h_pol;           /* true on negative polarity for horizontal pulses */
    hi_bool v_pol;           /* true on negative polarity for vertical pulses */
    hi_bool ntsc_pal;        /* 60/120/240_hz (false) or 50/100/200_hz (true) t_vs */
    hi_u32 repetition;       /* allowed video pixel repetition */
    hi_u32 max_audio_sr8_ch; /* maximum allowed audio sample rate for 8 channel audio in k_hz */
} video_timing_define;

typedef enum {
    TIMING_1_640X480P,
    TIMING_2_3_720X480P,
    TIMING_4_69_1280X720P,
    TIMING_5_1920X1080I,             /* 3 */
    TIMING_6_7_720_1440X480I,        /* 4 */
    TIMING_8_9_720_1440X240P_1,      /* 5 */
    TIMING_8_9_720_1440X240P_2,      /* 6 */
    TIMING_10_11_2880X480I,          /* 7 */
    TIMING_12_13_2880X240P_1,        /* 8 */
    TIMING_12_13_2880X240P_2,        /* 9 */
    TIMING_14_15_1440X480P,          /* 10 */
    TIMING_16_76_1920X1080P,         /* 11 */
    TIMING_17_18_720X576P,           /* 12 */
    TIMING_19_68_1280X720P,          /* 13 */
    TIMING_20_1920X1080I,            /* 14 */
    TIMING_21_22_720_1440X576I,      /* 15 */
    TIMING_23_24_720_1440X288P_1,    /* 16 */
    TIMING_23_24_720_1440X288P_2,    /* 17 */
    TIMING_23_24_720_1440X288P_3,    /* 18 */
    TIMING_25_26_2880X576I,          /* 19 */
    TIMING_27_28_2880X288P_1,        /* 20 */
    TIMING_27_28_2880X288P_2,        /* 21 */
    TIMING_27_28_2880X288P_3,        /* 22 */
    TIMING_29_30_1440X576P,          /* 23 */
    TIMING_31_75_1920X1080P,         /* 24 */
    TIMING_32_72_1920X1080P,         /* 25 */
    TIMING_33_73_1920X1080P,         /* 26 */
    TIMING_34_74_1920X1080P,         /* 27 */
    TIMING_35_36_2880X480P,          /* 28 */
    TIMING_37_38_2880X576P,          /* 29 */
    TIMING_39_1920X1080I_1250_TOTAL, /* 30 */
    TIMING_40_1920X1080I,            /* 31 */
    TIMING_41_70_1280X720P,          /* 32 */
    TIMING_42_43_720X576P,           /* 33 */
    TIMING_44_45_720_1440X576I,      /* 34 */
    TIMING_46_1920X1080I,            /* 35 */
    TIMING_47_71_1280X720P,          /* 36 */
    TIMING_48_49_720X480P,           /* 37 */
    TIMING_50_51_720_1440X480I,      /* 38 */
    TIMING_52_53_720X576P,           /* 39 */
    TIMING_54_55_720_1440X576I,      /* 40 */
    TIMING_56_57_720X480P,           /* 41 */
    TIMING_58_59_720_1440X480I,      /* 42 */
    TIMING_60_65_1280X720_24,        /* 43 */
    TIMING_61_66_1280X720_25,        /* 44 */
    TIMING_62_67_1280X720_30,        /* 45 */
    TIMING_63_78_1920X1080P_120,     /* 46 */
    TIMING_64_77_1920X1080P_100,     /* 47 */
    TIMING_79_1680X720P_24,          /* 48 */
    TIMING_80_1680X720P_25,          /* 49 */
    TIMING_81_1680X720P_30,          /* 50 */
    TIMING_82_1680X720P_50,          /* 51 */
    TIMING_83_1680X720P_60,          /* 52 */
    TIMING_84_1680X720P_100,         /* 53 */
    TIMING_85_1680X720P_120,         /* 54 */
    TIMING_86_2560X1080P_24,         /* 55 */
    TIMING_87_2560X1080P_25,         /* 56 */
    TIMING_88_2560X1080P_30,         /* 57 */
    TIMING_89_2560X1080P_50,         /* 58 */
    TIMING_90_2560X1080P_60,         /* 59 */
    TIMING_91_2560X1080P_100,        /* 60 */
    TIMING_92_2560X1080P_120,        /* 61 */
    TIMING_93_3840X2160P_24,         /* 62 */
    TIMING_94_3840X2160P_25,         /* 63 */
    TIMING_95_3840X2160P_30,         /* 64 */
    TIMING_96_3840X2160P_50,         /* 65 */
    TIMING_97_3840X2160P_60,         /* 66 */
    TIMING_98_4096X2160P_24,         /* 67 */
    TIMING_99_4096X2160P_25,         /* 68 */
    TIMING_100_4096X2160P_30,        /* 69 */
    TIMING_101_4096X2160P_50,        /* 70 */
    TIMING_102_4096X2160P_60,        /* 71 */
    TIMING_MAX,
    TIMING_PC_MODE = 0x200,
    TIMING_NOT_SUPPORT,
    TIMING_NOSIGNAL
} video_timing_idx;

typedef enum {
    PATH_RGB, /* RGB, single edge clock */
    PATH_Y_CB_CR444, /* y_cb_cr 4:4:4, single edge clock */
    /* PATH_RGB_2_EDGE, */ /* RGB, clock on both edges */
    /* path_y_cb_cr444_2_edge, */ /* y_cb_cr 4:4:4, clock on both edges */
    /* PATH_RGB_48B, */ /* RGB, 2 pixels at single edge clock */
    /* path_y_cb_cr444_48_b, */ /* y_cb_cr 4:4:4, 2 pixels per clock (both edges used) */
    PATH_Y_CB_CR422, /* y_cb_cr 4:2:2, single edge clock, Y is separate, cb and cr multiplexed */
    /* path_y_cb_cr422_mux8_b, */ /* y_cb_cr 4:2:2, single edge at 2x clock, */
                                    /* Y multiplexed with cb and cr, 8 bit bus */
    /* path_y_cb_cr422_mux10_b, */ /* y_cb_cr 4:2:2, single edge at 2x clock, */
                                /* Y multiplexed with cb and cr, 10 bit bus */
    /* path_y_cb_cr422_16_b, */ /* y_cb_cr 4:2:2, single edge clock, */
                                /* Y is separate, cb and cr multiplexed, 16 bit bus */
    /* path_y_cb_cr422_20_b, */ /* y_cb_cr 4:2:2, single edge clock, */
                                  /* Y is separate, cb and cr multiplexed, 20 bit bus */
    /* path_y_cb_cr422_16_b_2_pix_clk, */ /* y_cb_cr 4:2:2, clock on both edges, */
                                       /* Y is separate, cb and cr multiplexed, 16 bit bus */
    /* path_y_cb_cr422_20_b_2_pix_clk, */ /* y_cb_cr 4:2:2, clock on both edges, */
                                       /* Y is separate, cb and cr multiplexed, 20 bit bus */
    PATH_BUTT
} hdmirx_output_format;

typedef struct {
    hi_u32 clocks_per_line; /* number of pixel clocks per line */
    hi_u32 total_lines;     /* number of lines */
    hi_u32 hactive;         /* hactive */
    hi_u32 vactive;         /* vactive */
    hi_u32 pixel_freq;      /* pixel frequency in 10k_hz units */
    hi_bool interlaced;     /* true for interlaced video */
    hi_bool h_pol;          /* true on negative polarity for horizontal pulses */
    hi_bool v_pol;          /* true on negative polarity for vertical pulses */
} signal_timing_info;

typedef struct {
    video_timing_idx video_idx;
    hi_u32 cea861_vic;
    hi_u32 hdmi3d_structure;
    hi_u32 hdmi3d_ext_data;
    hi_u32 pix_freq; /* pixel frequency in 10k_hz units */
    hi_u32 vactive;
    hi_u32 hactive;
    hi_u32 frame_rate;
    hi_u32 htotal;
    hi_u32 vtotal;
} hdmirx_timing_info;

typedef enum {
    HDMIRX_3D_TYPE_FP = 0,
    HDMIRX_3D_TYPE_FIELD_ALT,
    HDMIRX_3D_TYPE_LINE_ALT,
    HDMIRX_3D_TYPE_SBS_FULL,
    HDMIRX_3D_TYPE_L_DEPTH,
    HDMIRX_3D_TYPE_L_DEPTH_GRAP,
    HDMIRX_3D_TYPE_TB,
    HDMIRX_3D_TYPE_RESERVE,
    HDMIRX_3D_TYPE_SBS_HALF,
    HDMIRX_3D_TYPE_BUTT
} hdmirx_3d_type;

typedef enum {
    hdmirx_special_case_null = 0,
    hdmirx_special_case_vsync_invert,
    hdmirx_special_case_butt
} hdmirx_special_case;

typedef struct {
    hdmirx_timing_info hdmirx_timing_info;
    hi_hdmirx_dynamic_source_data hdmirx_hdr_data;
    hdmirx_input_width input_width;
    hi_bool hdmi_mode;
    hi_u32 mode_chg_hv_res_cnt;
    hi_u32 mode_chg_pix_clk_cnt;
    hi_u32 mode_stable_cnt;
    hdmirx_special_case special_case;
} hdmirx_video_ctx;

#endif
#ifndef __DRV_HDMIRX2_VIDEO_H__
#define __DRV_HDMIRX2_VIDEO_H__

#define HDMIRXV2_VIDEO_GET_CTX(port) (&(g_hdmirx_video_ctx_v2[port]))

hi_void hdmirxv2_video_rst_timing_data(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_timing_data_init(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_set_deep_color_mode(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_video_check_stable(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_mode_det(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_video_is_support(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_video_is_dolby_vision(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_set_video_path(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_video_get_hdmi_mode(hi_drv_hdmirx_port port);
hi_u32 hdmirxv2_video_get_mode_chg_type(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_video_is_need_mode_change(hi_drv_hdmirx_port port, hi_u32 type);
hi_bool hdmirxv2_video_hdmi_dvi_trans(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_clear4k3d_info(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_verify1_p4_format(hi_drv_hdmirx_port port, hi_bool vsif_received);
hi_u32 hdmirxv2_video_get_cur3d_structure(hi_drv_hdmirx_port port);
hi_u32 hdmirxv2_video_get_cur3d_ext_data(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_set_cur3d_structure(hi_drv_hdmirx_port port, hi_u32 structure);
hi_void hdmirxv2_video_set_cur3d_ext_data(hi_drv_hdmirx_port port, hi_u32 td_ext_data);
hi_void hdmirxv2_video_set861_vic(hi_drv_hdmirx_port port, hi_u32 vic);
hi_void hdmirxv2_video_set_res_chg_events_en(hi_drv_hdmirx_port port, hi_bool en);
hi_u32 hdmirxv2_video_get_pixel_clk(hi_drv_hdmirx_port port);
hdmirx_input_width hdmirxv2_video_get_input_width(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_get_timing_info(hi_drv_hdmirx_port port, hi_drv_hdmirx_timing_info *timing_info);
hi_u32 hdmirxv2_video_get_frame_rate(hi_drv_hdmirx_port port);
hi_u32 hdmirxv2_video_get_hactive(hi_drv_hdmirx_port port);
hi_u32 hdmirxv2_video_get_vactive(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_set_video_idx(hi_drv_hdmirx_port port, video_timing_idx idx);
hi_bool hdmirxv2_video_is_timing_active(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_get_table_info(hi_u32 u32id, video_timing_define *table_info);
hi_void hdmirxv2_video_get_hdr_data(hi_drv_hdmirx_port port, hi_hdmirx_dynamic_source_data *data);
hi_void hdmirxv2_video_set_hdr_data(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_set_hdr10_plus_data(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_set_stream_data(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_proc_read(hi_drv_hdmirx_port port, hi_void *s);
hi_void hdmirxv2_video_set_dolby_data(hi_drv_hdmirx_port port);
hi_void hdmirxv2_video_show_video_timing(hi_drv_hdmirx_port port);

#endif
