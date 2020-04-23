/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of driver hdmitx modes
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __DRV_HDMITX_MODES_H__
#define __DRV_HDMITX_MODES_H__

#include "hi_osal.h"
#include "drv_hdmitx.h"
#include "hi_hdmi_types.h"

/* CTA-861-G Table 3 */
#define VIC_640X480P60_4_3         1
#define VIC_720X480P60_4_3         2
#define VIC_720X480P60_16_9        3
#define VIC_1280X720P60_16_9       4
#define VIC_1920X1080I60_16_9      5
#define VIC_1440X480I60_4_3        6
#define VIC_1440X480I60_16_9       7
#define VIC_1440X240P60_4_3        8
#define VIC_1440X240P60_16_9       9
#define VIC_2880X480I60_4_3        10
#define VIC_2880X480I60_16_9       11
#define VIC_2880X240P60_4_3        12
#define VIC_2880X240P60_16_9       13
#define VIC_1440X480P60_4_3        14
#define VIC_1440X480P60_16_9       15
#define VIC_1920X1080P60_16_9      16
#define VIC_720X576P50_4_3         17
#define VIC_720X576P50_16_9        18
#define VIC_1280X720P50_16_9       19
#define VIC_1920X1080I50_16_9      20
#define VIC_1440X576I50_4_3        21
#define VIC_1440X576I50_16_9       22
#define VIC_1440X288P50_4_3        23
#define VIC_1440X288P50_16_9       24
#define VIC_2880X576I50_4_3        25
#define VIC_2880X576I50_16_9       26
#define VIC_2880X288P50_4_3        27
#define VIC_2880X288P50_16_9       28
#define VIC_1440X576P50_4_3        29
#define VIC_1440X576P50_16_9       30
#define VIC_1920X1080P50_16_9      31
#define VIC_1920X1080P24_16_9      32
#define VIC_1920X1080P25_16_9      33
#define VIC_1920X1080P30_16_9      34
#define VIC_2880X480P60_4_3        35
#define VIC_2880X480P60_16_9       36
#define VIC_2880X576P50_4_3        37
#define VIC_2880X576P50_16_9       38
#define VIC_1920X1080I50_1250_16_9 39
#define VIC_1920X1080I100_16_9     40
#define VIC_1280X720P100_16_9      41
#define VIC_720X576P100_4_3        42
#define VIC_720X576P100_16_9       43
#define VIC_1440X576I100_4_3       44
#define VIC_1440X576I100_16_9      45
#define VIC_1920X1080I120_16_9     46
#define VIC_1280X720P120_16_9      47
#define VIC_720X480P120_4_3        48
#define VIC_720X480P120_16_9       49
#define VIC_1440X480I120_4_3       50
#define VIC_1440X480I120_16_9      51
#define VIC_720X576P200_4_3        52
#define VIC_720X576P200_16_9       53
#define VIC_1440X576I200_4_3       54
#define VIC_1440X576I200_16_9      55
#define VIC_720X480P240_4_3        56
#define VIC_720X480P240_16_9       57
#define VIC_1440X480I240_4_3       58
#define VIC_1440X480I240_16_9      59
#define VIC_1280X720P24_16_9       60
#define VIC_1280X720P25_16_9       61
#define VIC_1280X720P30_16_9       62
#define VIC_1920X1080P120_16_9     63
#define VIC_1920X1080P100_16_9     64
#define VIC_1280X720P24_64_27      65
#define VIC_1280X720P25_64_27      66
#define VIC_1280X720P30_64_27      67
#define VIC_1280X720P50_64_27      68
#define VIC_1280X720P60_64_27      69
#define VIC_1280X720P100_64_27     70
#define VIC_1280X720P120_64_27     71
#define VIC_1920X1080P24_64_27     72
#define VIC_1920X1080P25_64_27     73
#define VIC_1920X1080P30_64_27     74
#define VIC_1920X1080P50_64_27     75
#define VIC_1920X1080P60_64_27     76
#define VIC_1920X1080P100_64_27    77
#define VIC_1920X1080P120_64_27    78
#define VIC_1680X720P24_64_27      79
#define VIC_1680X720P25_64_27      80
#define VIC_1680X720P30_64_27      81
#define VIC_1680X720P50_64_27      82
#define VIC_1680X720P60_64_27      83
#define VIC_1680X720P100_64_27     84
#define VIC_1680X720P120_64_27     85
#define VIC_2560X1080P24_64_27     86
#define VIC_2560X1080P25_64_27     87
#define VIC_2560X1080P30_64_27     88
#define VIC_2560X1080P50_64_27     89
#define VIC_2560X1080P60_64_27     90
#define VIC_2560X1080P100_64_27    91
#define VIC_2560X1080P120_64_27    92
#define VIC_3840X2160P24_16_9      93
#define VIC_3840X2160P25_16_9      94
#define VIC_3840X2160P30_16_9      95
#define VIC_3840X2160P50_16_9      96
#define VIC_3840X2160P60_16_9      97
#define VIC_4096X2160P24_256_135   98
#define VIC_4096X2160P25_256_135   99
#define VIC_4096X2160P30_256_135   100
#define VIC_4096X2160P50_256_135   101
#define VIC_4096X2160P60_256_135   102
#define VIC_3840X2160P24_64_27     103
#define VIC_3840X2160P25_64_27     104
#define VIC_3840X2160P30_64_27     105
#define VIC_3840X2160P50_64_27     106
#define VIC_3840X2160P60_64_27     107
#define VIC_1280X720P48_16_9       108
#define VIC_1280X720P48_64_27      109
#define VIC_1680X720P48_64_27      110
#define VIC_1920X1080P48_16_9      111
#define VIC_1920X1080P48_64_27     112
#define VIC_2560X1080P48_64_27     113
#define VIC_3840X2160P48_16_9      114
#define VIC_4096X2160P48_256_135   115
#define VIC_3840X2160P48_64_27     116
#define VIC_3840X2160P100_16_9     117
#define VIC_3840X2160P120_16_9     118
#define VIC_3840X2160P100_64_27    119
#define VIC_3840X2160P120_64_27    120
#define VIC_5120X2160P24_64_27     121
#define VIC_5120X2160P25_64_27     122
#define VIC_5120X2160P30_64_27     123
#define VIC_5120X2160P48_64_27     124
#define VIC_5120X2160P50_64_27     125
#define VIC_5120X2160P60_64_27     126
#define VIC_5120X2160P100_64_27    127
/* 128-192  Forbidden */
#define VIC_5120X2160P120_64_27   193
#define VIC_7680X4320P24_16_9     194
#define VIC_7680X4320P25_16_9     195
#define VIC_7680X4320P30_16_9     196
#define VIC_7680X4320P48_16_9     197
#define VIC_7680X4320P50_16_9     198
#define VIC_7680X4320P60_16_9     199
#define VIC_7680X4320P100_16_9    200
#define VIC_7680X4320P120_16_9    201
#define VIC_7680X4320P24_64_27    202
#define VIC_7680X4320P25_64_27    203
#define VIC_7680X4320P30_64_27    204
#define VIC_7680X4320P48_64_27    205
#define VIC_7680X4320P50_64_27    206
#define VIC_7680X4320P60_64_27    207
#define VIC_7680X4320P100_64_27   208
#define VIC_7680X4320P120_64_27   209
#define VIC_10240X4320P24_64_27   210
#define VIC_10240X4320P25_64_27   211
#define VIC_10240X4320P30_64_27   212
#define VIC_10240X4320P48_64_27   213
#define VIC_10240X4320P50_64_27   214
#define VIC_10240X4320P60_64_27   215
#define VIC_10240X4320P100_64_27  216
#define VIC_10240X4320P120_64_27  217
#define VIC_4096X2160P100_256_135 218
#define VIC_4096X2160P120_256_135 219
/* 220-255  Reserved for the Future */
/* VESA TIMING */
#define VIC_VESA_800X600P60     257
#define VIC_VESA_800X600P56     258
#define VIC_VESA_640X480P75     259
#define VIC_VESA_640X480P72     260
#define VIC_APPLE_640X480P67    261
#define VIC_IBM_640X480P60      262
#define VIC_IBM_720X400P88      263
#define VIC_IBM_720X400P70      264
#define VIC_VESA_1280X1024P75   265
#define VIC_VESA_1024X768P75    266
#define VIC_VESA_1024X768P70    267
#define VIC_VESA_1024X768P60    268
#define VIC_IBM_1024X768I43     269
#define VIC_APPLE_832X624P75    270
#define VIC_VESA_800X600P75     271
#define VIC_VESA_800X600P72     272
#define VIC_APPLE_1152X864P75   273
#define VIC_VESA_640X350P85     274
#define VIC_VESA_640X400P85     275
#define VIC_VESA_720X400P85     276
#define VIC_VESA_640X480P60     277
#define VIC_VESA_640X480P85     278
#define VIC_VESA_800X600P85     279
#define VIC_VESA_800X600P120    280
#define VIC_VESA_848X480P60     281
#define VIC_VESA_1024X768I43    282
#define VIC_VESA_1024X768P85    283
#define VIC_VESA_1024X768P120   284
#define VIC_VESA_1152X846P75    285
#define VIC_VESA_1280X720P60    286
#define VIC_VESA_1280X768P60    287
#define VIC_VESA_1280X768P75    288
#define VIC_VESA_1280X768P85    289
#define VIC_VESA_1280X768P120   290
#define VIC_VESA_1280X800P60    291
#define VIC_VESA_1280X800P75    292
#define VIC_VESA_1280X800P85    293
#define VIC_VESA_1280X800P120   294
#define VIC_VESA_1280X960P60    295
#define VIC_VESA_1280X960P85    296
#define VIC_VESA_1280X960P120   297
#define VIC_VESA_1280X1024P60   298
#define VIC_VESA_1280X1024P85   299
#define VIC_VESA_1280X1024P120  300
#define VIC_VESA_1360X768P60    301
#define VIC_VESA_1360X768P120   302
#define VIC_VESA_1366X768P60    303
#define VIC_VESA_1400X1050P60   304
#define VIC_VESA_1400X1050P75   305
#define VIC_VESA_1400X1050P85   306
#define VIC_VESA_1400X1050P120  307
#define VIC_VESA_1400X900P60    308
#define VIC_VESA_1400X900P75    309
#define VIC_VESA_1400X900P85    310
#define VIC_VESA_1400X900P120   311
#define VIC_VESA_1600X900P60    312
#define VIC_VESA_1600X1200P60   313
#define VIC_VESA_1600X1200P65   314
#define VIC_VESA_1600X1200P70   315
#define VIC_VESA_1600X1200P75   316
#define VIC_VESA_1600X1200P85   317
#define VIC_VESA_1600X1200P120  318
#define VIC_VESA_1680X1050P60   319
#define VIC_VESA_1680X1050P75   320
#define VIC_VESA_1680X1050P85   321
#define VIC_VESA_1680X1050P120  322
#define VIC_VESA_1792X1344P60   323
#define VIC_VESA_1792X1344P75   324
#define VIC_VESA_1792X1344P120  325
#define VIC_VESA_1856X1392P60   326
#define VIC_VESA_1856X1392P75   327
#define VIC_VESA_1856X1392P120  328
#define VIC_VESA_1920X1080P60   329
#define VIC_VESA_1920X1200P60   330
#define VIC_VESA_1920X1200P75   331
#define VIC_VESA_1920X1200P85   332
#define VIC_VESA_1920X1200P120  333
#define VIC_VESA_1920X1440P60   334
#define VIC_VESA_1920X1440P75   335
#define VIC_VESA_1920X1440P120  336
#define VIC_VESA_2048X1152P60   337
#define VIC_VESA_2560X1600P60   338
#define VIC_VESA_2560X1600P75   339
#define VIC_VESA_2560X1600P85   340
#define VIC_VESA_2560X1600P120  341
#define VIC_VESA_2560X1440P60RB 342
#define VIC_VESA_1680X1050P60RB 343

#define VIC_DETAIL_TIMING_BASE 400

/* positive pulse */
#define POS 0
/* negative pulse */
#define NEG 1
/* interlaced scan */
#define I 0
/* progressive scan */
#define P 1

/* color depth define */
#define CD_24 0
#define CD_30 1
#define CD_36 2
#define CD_48 3

/* color format define */
#define RGB444   0
#define YCBCR422 1
#define YCBCR444 2
#define YCBCR420 3

#define COLOR_RGB_24  (1 << 0)
#define COLOR_RGB_30  (1 << 1)
#define COLOR_RGB_36  (1 << 2)
#define COLOR_RGB_48  (1 << 3)
#define COLOR_Y444_24 (1 << 4)
#define COLOR_Y444_30 (1 << 5)
#define COLOR_Y444_36 (1 << 6)
#define COLOR_Y444_48 (1 << 7)
#define COLOR_Y420_24 (1 << 8)
#define COLOR_Y420_30 (1 << 9)
#define COLOR_Y420_36 (1 << 10)
#define COLOR_Y420_48 (1 << 11)
#define COLOR_Y422    (1 << 12)

#define COLOR_RGB_MASK  (0xf << 0)
#define COLOR_Y444_MASK (0xf << 4)
#define COLOR_Y420_MASK (0xf << 8)
#define COLOR_Y422_MASK (1 << 12)

#define HDMI_CONNECTOR_NAME_LEN 32
#define HDMI_DISPLAY_MODE_LEN   32
#define HDMI_PROP_NAME_LEN      32

#define HDMI_MODE_TYPE_BUILTIN   (1 << 0)                            /* deprecated */
#define HDMI_MODE_TYPE_CLOCK_C   ((1 << 1) | HDMI_MODE_TYPE_BUILTIN) /* deprecated */
#define HDMI_MODE_TYPE_CRTC_C    ((1 << 2) | HDMI_MODE_TYPE_BUILTIN) /* deprecated */
#define HDMI_MODE_TYPE_PREFERRED (1 << 3)
#define HDMI_MODE_TYPE_DEFAULT   (1 << 4) /* deprecated */
#define HDMI_MODE_TYPE_USERDEF   (1 << 5)
#define HDMI_MODE_TYPE_DRIVER    (1 << 6)

#define HDMI_MODE_TYPE_ALL (HDMI_MODE_TYPE_PREFERRED |  \
                            HDMI_MODE_TYPE_USERDEF |  \
                            HDMI_MODE_TYPE_DRIVER)

/* Video mode flags */
/* bit compatible with the xrandr RR_ definitions (bits 0-13)
 *
 * ABI warning: Existing userspace really expects
 * the mode flags to match the xrandr definitions. Any
 * changes that don't match the xrandr definitions will
 * likely need a new client cap or some other mechanism
 * to avoid breaking existing userspace. This includes
 * allocating new flags in the previously unused bits!
 */
#define HDMI_MODE_FLAG_PHSYNC    (1 << 0)
#define HDMI_MODE_FLAG_NHSYNC    (1 << 1)
#define HDMI_MODE_FLAG_PVSYNC    (1 << 2)
#define HDMI_MODE_FLAG_NVSYNC    (1 << 3)
#define HDMI_MODE_FLAG_INTERLACE (1 << 4)
#define HDMI_MODE_FLAG_DBLSCAN   (1 << 5)
#define HDMI_MODE_FLAG_CSYNC     (1 << 6)
#define HDMI_MODE_FLAG_PCSYNC    (1 << 7)
#define HDMI_MODE_FLAG_NCSYNC    (1 << 8)
#define HDMI_MODE_FLAG_HSKEW     (1 << 9)  /* hskew provided */
#define HDMI_MODE_FLAG_BCAST     (1 << 10) /* deprecated */
#define HDMI_MODE_FLAG_PIXMUX    (1 << 11) /* deprecated */
#define HDMI_MODE_FLAG_DBLCLK    (1 << 12)
#define HDMI_MODE_FLAG_CLKDIV2   (1 << 13)

/* FRL_Rate define.see Table 10-21: SCDCS - Sink Configuration */
#define FRL_RATE_DISABLE 0
#define FRL_RATE_3G3L    1
#define FRL_RATE_6G3L    2
#define FRL_RATE_6G4L    3
#define FRL_RATE_8G4L    4
#define FRL_RATE_10G4L   5
#define FRL_RATE_12G4L   6
#define FRL_RATE_RSVD    7

#define FRL_CLOCK_3G 3
#define FRL_CLOCK_6G 6
#define FRL_CLOCK_8G 8
#define FRL_CLOCK_10G 10
#define FRL_CLOCK_12G 12

#define FRL_FFE_LEVELS_MAX 3

#define HDMI_DISPLAY_MODE_LEN   32

/*
 * Note on terminology:  here, for brevity and convenience, we refer to connector
 * control chips as 'CRTCs'.  They can control any type of connector, VGA, LVDS,
 * DVI, etc.  And 'screen' refers to the whole of the visible display, which
 * may span multiple monitors (and therefore multiple CRTC and connector
 * structures).
 */

enum hdmi_mode_status {
    MODE_OK = 0,          /* Mode OK */
    MODE_UNVERIFIED = -3, /* mode needs to reverified */
    MODE_BAD = -2,        /* unspecified reason */
    MODE_ERROR = -1,      /* error condition */
    MODE_TRAIN_FAIL = -4,
    MODE_SCDC_FAIL = -5
};

/* band width mode */
struct band_mode {
    hi_u32 vic;          /* video id code */
    hi_u32 color_format; /* pixel encoding format */
    hi_u32 color_depth;  /* color depth */
};

/* frl mode */
struct frl_mode {
    hi_bool frl_uncompress; /* transmit in frl uncompress mode */
    hi_u32 min_frl_rate;    /* min frl rate */
    hi_u32 max_frl_rate;    /* max frl rate */
    hi_u8 ffe_levels;       /* ffe levels */
};

/* dsc mode.see HDMI2.1 SPEC Table 7-35~7-36 */
struct dsc_mode {
    hi_bool frl_compress;    /* tansmit in frl compress mode (true-dsc enable;false-dsc disable) */
    hi_u32 min_dsc_frl_rate; /* mix dsc frl rate */
    hi_u32 max_dsc_frl_rate; /* max dsc frl rate */
    hi_u32 bpp_target;       /* dsc bpptarget */
    hi_u32 slice_width;      /* dsc slicewidth */
    hi_u32 hcactive;         /* dsc hcactive */
    hi_u32 hcblank;          /* dsc hcblank */
};

/* hdmi mode config */
struct hdmi_mode_config {
    struct band_mode band; /* band width mode */
    hi_u32 tmds_clock;     /* tmds clock */
    hi_bool tmds_encode;   /* transmit in tmds mode */
    hi_u32 hdmi_mode;      /* hdmi mode */
    hi_bool tmds_scdc_en;  /* tmds scdc scramble enable */
    struct frl_mode frl;   /* frl mode */
    struct dsc_mode dsc;   /* dsc mode */
};

struct hdmi_valid_mode {
    struct osal_list_head head;
    struct band_mode band; /* band width mode */
    hi_bool tmds_encode;   /* can be transmit in tmds mode */
    hi_u32 hdmi_mode;      /* hdmi mode when transmit in tmds mode */
    hi_bool tmds_scdc_en;  /* tmds scdc scramble enable */
    hi_u32 tmds_clock;     /* tmds clock */
    struct frl_mode frl;   /* frl mode */
    struct dsc_mode dsc;   /* dsc mode */
    hi_bool valid;
};

/* see CTA-861-G Table1 & Table 3 */
enum hi_hdmi_picture_aspect {
    ASP_NONE, /* no data */
    ASP_4_3,
    ASP_16_9,
    ASP_64_27,    /* resverd */
    ASP_256_135,  /* resverd */
    ASP_RESERVED, /* resverd */
};

struct hdmi_timing_mode {
    hi_u32 vic;
    hi_u32 pixel_clock;
    hi_u32 h_active;
    hi_u32 v_active;
    hi_bool progressive;
    hi_u32 pic_aspect;
    hi_u32 h_total;
    hi_u32 v_total;
    hi_u32 h_blank;
    hi_u32 v_blank;
    hi_u32 h_front;
    hi_u32 h_sync;
    hi_u32 h_back;
    hi_bool h_pol;
    hi_u32 v_front;
    hi_u32 v_sync;
    hi_u32 v_back;
    hi_bool v_pol;
    hi_u32 field_rate;
    hi_char *name;
};

/* see HDMI2.1 SPEC Table 7-32~7-34 */
struct frl_requirements {
    struct band_mode band_mode; /* band mode */
    hi_u32 min_frl_rate;        /* min frl rate */
    hi_bool frl_uncompress;     /* frl uncompress transmit */

    hi_bool frl_compress;    /* tansmit in frl compress mode (true-dsc enable;false-dsc disable) */
    hi_u32 min_dsc_frl_rate; /* mix dsc frl rate */
    hi_u32 dsc_bpp_target;   /* dsc bpptarget */
    hi_u32 dsc_hcactive;     /* dsc hcactive */
    hi_u32 dsc_hcblank;      /* dsc hcblank */
};

struct hdmi_display_mode {
    /*
     * @head:
     * struct osal_list_head for mode lists.
     */
    struct osal_list_head head;

    hi_u32 vic;

    struct hdmi_detail detail;

    /* struct color_cap color_cap; */
    hi_u32 color_cap;
    hi_bool native_mode;

    /* First mode in the video data block of EDID */
    hi_bool first_mode;
    hi_s32 parse_type;

    /*
     * @flags:
     * Sync and timing flags:
     *  - HDMI_MODE_FLAG_PHSYNC: horizontal sync is active high.
     *  - HDMI_MODE_FLAG_NHSYNC: horizontal sync is active low.
     *  - HDMI_MODE_FLAG_PVSYNC: vertical sync is active high.
     *  - HDMI_MODE_FLAG_NVSYNC: vertical sync is active low.
     *  - HDMI_MODE_FLAG_INTERLACE: mode is interlaced.
     *  - HDMI_MODE_FLAG_DBLSCAN: mode uses doublescan.
     *  - HDMI_MODE_FLAG_CSYNC: mode uses composite sync.
     *  - HDMI_MODE_FLAG_PCSYNC: composite sync is active high.
     *  - HDMI_MODE_FLAG_NCSYNC: composite sync is active low.
     *  - HDMI_MODE_FLAG_HSKEW: hskew provided (not used?).
     *  - HDMI_MODE_FLAG_BCAST: <deprecated>
     *  - HDMI_MODE_FLAG_PIXMUX: <deprecated>
     *  - HDMI_MODE_FLAG_DBLCLK: double-clocked mode.
     *  - HDMI_MODE_FLAG_CLKDIV2: half-clocked mode.
     * Additionally there's flags to specify how 3D modes are packed:
     *  - HDMI_MODE_FLAG_3D_NONE: normal, non-3D mode.
     *  - HDMI_MODE_FLAG_3D_FRAME_PACKING: 2 full frames for left and right.
     *  - HDMI_MODE_FLAG_3D_FIELD_ALTERNATIVE: interleaved like fields.
     *  - HDMI_MODE_FLAG_3D_LINE_ALTERNATIVE: interleaved lines.
     *  - HDMI_MODE_FLAG_3D_SIDE_BY_SIDE_FULL: side-by-side full frames.
     *  - HDMI_MODE_FLAG_3D_L_DEPTH: ?
     *  - HDMI_MODE_FLAG_3D_L_DEPTH_GFX_GFX_DEPTH: ?
     *  - HDMI_MODE_FLAG_3D_TOP_AND_BOTTOM: frame split into top and bottom
     *    parts.
     *  - HDMI_MODE_FLAG_3D_SIDE_BY_SIDE_HALF: frame split into left and
     *    right parts.
     */
    hi_u32 flags;

    /*
     * @name:
     * Human-readable name of the mode, filled out with hdmi_mode_set_name().
     */
    hi_s8 name[HDMI_DISPLAY_MODE_LEN];

    /*
     * @status:
     * Status of the mode, used to filter out modes not supported by the
     * hardware. See enum &hdmi_mode_status.
     */
    enum hdmi_mode_status status;

    /*
     * @type:
     * A bitmask of flags, mostly about the source of a mode. Possible flags
     * are:
     *  - HDMI_MODE_TYPE_PREFERRED: Preferred mode, usually the native
     *    resolution of an LCD panel. There should only be one preferred
     *    mode per connector at any given time.
     *  - HDMI_MODE_TYPE_DRIVER: Mode created by the driver, which is all of
     *    them really. Drivers must set this bit for all modes they create
     *    and expose to userspace.
     *  - HDMI_MODE_TYPE_USERDEF: Mode defined via kernel command line
     * Plus a big list of flags which shouldn't be used at all, but are
     * still around since these flags are also used in the userspace ABI.
     * We no longer accept modes with these types though:
     *  - HDMI_MODE_TYPE_BUILTIN: Meant for hard-coded modes, unused.
     *    Use HDMI_MODE_TYPE_DRIVER instead.
     *  - HDMI_MODE_TYPE_DEFAULT: Again a leftover, use
     *    HDMI_MODE_TYPE_PREFERRED instead.
     *  - HDMI_MODE_TYPE_CLOCK_C and HDMI_MODE_TYPE_CRTC_C: Define leftovers
     *    which are stuck around for hysterical raisins only. No one has an
     *    idea what they were meant for. Don't use.
     */
    hi_u32 type;
};

struct hdmi_timing_mode *vic2timing_mode(hi_u32 vic);
struct frl_requirements *frl_req_search_by_band(struct band_mode *in);
struct hdmi_display_mode *hdmi_mode_create(struct hisilicon_hdmi *hdmi_dev);
void hdmi_mode_destroy(struct hisilicon_hdmi *hdmi_dev, struct hdmi_display_mode *mode);
struct hdmi_display_mode *hdmi_mode_create_from_vic(struct hisilicon_hdmi *hdmi_dev, hi_u32 vic);
enum hi_hdmi_picture_aspect hdmi_get_cea_aspect_ratio(hi_u32 vic);
struct hdmi_display_mode *hdmi_mode_create_by_detailed_timing(hi_u32 pixel_clk,
                                                              hi_u32 hactive,
                                                              hi_u32 vactive,
                                                              hi_u32 hblank,
                                                              hi_u32 vblank);
struct hdmi_display_mode *hdmi_mode_create_std_timing(struct hisilicon_hdmi *hdmi_dev,
                                                      hi_u32 hsize,
                                                      hi_u32 vsize,
                                                      hi_u32 refresh);
hi_void hdmi_mode_probed_add(struct hdmi_connector *connector, struct hdmi_display_mode *mode);

struct hdmi_display_mode *hdmi_mode_duplicate(struct hisilicon_hdmi *hdmi_dev,
                                              const struct hdmi_display_mode *mode);
hi_s32 hdmi_mode_vrefresh(struct hdmi_display_mode *mode);
void hdmi_mode_set_name(struct hdmi_display_mode *mode);
hi_u32 hdmi_mode_clock_pixel2tmds(hi_u32 pix_clk, hi_u32 color_depth, hi_u32 color_format);

#endif /* __DRV_HDMITX_MODES_H__ */

