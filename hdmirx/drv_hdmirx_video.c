/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of video functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "drv_hdmirx_video.h"
#include "drv_hdmirx_common.h"
#include "drv_hdmirx_packet.h"
#include "drv_hdmirx_ctrl.h"
#include "hal_hdmirx_video.h"
#include "hal_hdmirx_comm.h"
#include "hal_hdmirx_ctrl.h"
#include <securec.h>

#define HDMIRX_HD_HEIGHT 720
#define HDMIRX_INTERLACE_FIELD_THRESHOLD 2

static hdmirx_video_ctx g_hdmirx_video_ctx_v2[HI_DRV_HDMIRX_PORT_MAX];
static hdmirx_color_metry hdmirxv2_packet_avi_get_color_metry_again(hi_drv_hdmirx_timing_info *timing_info);

static const video_timing_define g_video_timing_table[TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES + 1] = {
    {
        1, 0, 0, { 640, 480 }, { 800, 525 }, { 160, 45 }, { 16, 10 },        /* 0    640*480*60_hz */
        { 96, 2 }, 31, 60, 25, 2518, PROG, NEG, NEG, 0, RP1, 48
    }, {
        2, 3, 0, { 720, 480 }, { 858, 525 }, { 138, 45 }, { 16, 9 },         /* 1    480p */
        { 62, 6 }, 31, 60, 27, 2700, PROG, NEG, NEG, NTSC, RP1, 48
    }, {
        0, 4, 0, { 1280, 720 }, { 1650, 750 }, { 370, 30 }, { 110, 5 },      /* 2    720p60 */
        { 40, 5 }, 45, 60, 74, 7425, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 5, 0, { 1920, 1080 }, { 2200, 1125 }, { 280, 22 }, { 88, 2 },     /* 3    1080i60 */
        { 44, 5 }, 34, 60, 74, 7425, INTL, POS, POS, NTSC, RP1, 192
    }, {
        6, 7, 0, { 1440, 480 }, { 1716, 525 }, { 276, 22 }, { 38, 4 },       /* 4    480i*2 */
        { 124, 3 }, 16, 60, 27, 2700, INTL, NEG, NEG, NTSC, RP2, 48
    }, {
        8, 9, 0, { 1440, 240 }, { 1716, 262 }, { 276, 22 }, { 38, 4 },       /* 5    240p*2 */
        { 124, 3 }, 16, 60, 27, 2700, PROG, NEG, NEG, NTSC, RP2, 48
    }, {
        8, 9, 0, { 1440, 240 }, { 1716, 263 }, { 276, 23 }, { 38, 5 },       /* 6    240p*2 */
        { 124, 3 }, 16, 60, 27, 2700, PROG, NEG, NEG, NTSC, RP2, 48
    }, {
        10, 11, 0, { 2880, 480 }, { 3432, 525 }, { 552, 22 }, { 76, 4 },     /* 7    480i*4 */
        { 248, 3 }, 16, 60, 54, 5400, INTL, NEG, NEG, NTSC, RP4, 96
    }, {
        12, 13, 0, { 2880, 240 }, { 3432, 262 }, { 552, 22 }, { 76, 4 },     /* 8    240p*4 */
        { 248, 3 }, 16, 60, 54, 5400, PROG, NEG, NEG, NTSC, RP4, 96
    }, {
        12, 13, 0, { 2880, 240 }, { 3432, 263 }, { 552, 23 }, { 76, 5 },     /* 9    240p*4 */
        { 248, 3 }, 16, 60, 54, 5400, PROG, NEG, NEG, NTSC, RP4, 96
    }, {
        14, 15, 0, { 1440, 480 }, { 1716, 525 }, { 276, 45 }, { 32, 9 },     /* 10  480p*2 */
        { 124, 6 }, 31, 60, 54, 5400, PROG, NEG, NEG, NTSC, RP1 | RP2, 96
    }, {
        0, 16, 0, { 1920, 1080 }, { 2200, 1125 }, { 280, 45 }, { 88, 4 },    /* 11  1080p60 */
        { 44, 5 }, 67, 60, 148, 14850, PROG, POS, POS, NTSC, RP1, 192
    }, {
        17, 18, 0, { 720, 576 }, { 864, 625 }, { 144, 49 }, { 12, 5 },       /* 12  576p */
        { 64, 5 }, 31, 50, 27, 2700, PROG, NEG, NEG, PAL, RP1, 48
    }, {
        0, 19, 0, { 1280, 720 }, { 1980, 750 }, { 700, 30 }, { 440, 5 },     /* 13   720p50 */
        { 40, 5 }, 38, 50, 74, 7425, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 20, 0, { 1920, 1080 }, { 2640, 1125 }, { 720, 22 }, { 528, 2 },   /* 14  1080i50 */
        { 44, 5 }, 28, 50, 74, 7425, INTL, POS, POS, PAL, RP1, 192
    }, {
        21, 22, 0, { 1440, 576 }, { 1728, 625 }, { 288, 24 }, { 24, 2 },     /* 15  1440*576i */
        { 126, 3 }, 16, 50, 27, 2700, INTL, NEG, NEG, PAL, RP2, 48
    }, {
        23, 24, 0, { 1440, 288 }, { 1728, 312 }, { 288, 24 }, { 24, 2 },     /* 16  1440*288p */
        { 126, 3 }, 16, 50, 27, 2700, PROG, NEG, NEG, PAL, RP2, 48
    }, {
        23, 24, 0, { 1440, 288 }, { 1728, 313 }, { 288, 25 }, { 24, 3 },     /* 17  1440*288p */
        { 126, 3 }, 16, 49, 27, 2700, PROG, NEG, NEG, PAL, RP2, 48
    }, {
        23, 24, 0, { 1440, 288 }, { 1728, 314 }, { 288, 26 }, { 24, 4 },     /* 18  1440*288p */
        { 126, 3 }, 16, 49, 27, 2700, PROG, NEG, NEG, PAL, RP2, 48
    }, {
        25, 26, 0, { 2880, 576 }, { 3456, 625 }, { 576, 24 }, { 48, 2 },     /* 19  2880*576i */
        { 252, 3 }, 16, 50, 54, 5400, INTL, NEG, NEG, PAL, RP4, 96
    }, {
        27, 28, 0, { 2880, 288 }, { 3456, 312 }, { 576, 24 }, { 48, 2 },     /* 20  2880*288p */
        { 252, 3 }, 16, 50, 54, 5400, PROG, NEG, NEG, PAL, RP4, 96
    }, {
        27, 28, 0, { 2880, 288 }, { 3456, 313 }, { 576, 25 }, { 48, 3 },     /* 21  2880*288p */
        { 252, 3 }, 16, 49, 54, 5400, PROG, NEG, NEG, PAL, RP4, 96
    }, {
        27, 28, 0, { 2880, 288 }, { 3456, 314 }, { 576, 26 }, { 48, 4 },     /* 22  2880*288p */
        { 252, 3 }, 16, 49, 54, 5400, PROG, NEG, NEG, PAL, RP4, 96
    }, {
        29, 30, 0, { 1440, 576 }, { 1728, 625 }, { 288, 49 }, { 24, 5 },     /* 23  1440*576P */
        { 128, 5 }, 31, 50, 54, 5400, PROG, NEG, NEG, PAL, RP1 | RP2, 96
    }, {
        0, 31, 0, { 1920, 1080 }, { 2640, 1125 }, { 720, 45 }, { 528, 4 },   /* 24  1080p50 */
        { 44, 5 }, 56, 50, 148, 14850, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 32, 0, { 1920, 1080 }, { 2750, 1125 }, { 830, 45 }, { 638, 4 },   /* 25  1080P24 */
        { 44, 5 }, 27, 24, 74, 7425, PROG, POS, POS, 0, RP1, 192
    }, {
        0, 33, 0, { 1920, 1080 }, { 2640, 1125 }, { 720, 45 }, { 528, 4 },   /* 26  1080p25 */
        { 44, 5 }, 28, 25, 74, 7425, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 34, 0, { 1920, 1080 }, { 2200, 1125 }, { 280, 45 }, { 88, 4 },    /* 27  1080p30 */
        { 44, 5 }, 34, 30, 74, 7425, PROG, POS, POS, NTSC, RP1, 192
    }, {
        35, 36, 0, { 2880, 480 }, { 3432, 525 }, { 552, 45 }, { 96, 9 },     /* 28  2880*480p */
        { 248, 6 }, 31, 60, 108, 10800, PROG, NEG, NEG, NTSC, RP1 | RP2 | RP4, 192
    }, {
        37, 38, 0, { 2880, 576 }, { 3456, 625 }, { 576, 49 }, { 48, 5 },     /* 29  2880*576p */
        { 256, 5 }, 31, 50, 108, 10800, PROG, NEG, NEG, PAL, RP1 | RP2 | RP4, 192
    }, {
        0, 39, 0, { 1920, 1080 }, { 2304, 1250 }, { 384, 85 }, { 32, 23 },
        { 168, 5 }, 31, 50, 72, 7200, INTL, POS, NEG, PAL, RP1, 192 /* 30 H-pos, V-neg, 1,2 blanks = 85 1080i50 */
    }, {
        0, 40, 0, { 1920, 1080 }, { 2640, 1125 }, { 720, 22 }, { 528, 2 },   /* 31  1080i*100 */
        { 44, 5 }, 56, 100, 148, 14850, INTL, POS, POS, PAL, RP1, 192
    }, {
        0, 41, 0, { 1280, 720 }, { 1980, 750 }, { 700, 30 }, { 440, 5 },     /* 32  720p100 */
        { 40, 5 }, 75, 100, 148, 14850, PROG, POS, POS, PAL, RP1, 192
    }, {
        42, 43, 0, { 720, 576 }, { 864, 625 }, { 144, 49 }, { 12, 5 },       /* 33  576p100 */
        { 64, 5 }, 63, 100, 54, 5400, PROG, NEG, NEG, PAL, RP1, 96
    }, {
        44, 45, 0, { 1440, 576 }, { 1728, 625 }, { 288, 24 }, { 24, 2 },     /* 34  1440*576i*100 */
        { 126, 3 }, 31, 100, 54, 5400, INTL, NEG, NEG, PAL, RP2, 96
    }, {
        0, 46, 0, { 1920, 1080 }, { 2200, 1125 }, { 280, 22 }, { 88, 2 },    /* 35  1080i*120 */
        { 44, 5 }, 68, 120, 148, 14850, INTL, POS, POS, NTSC, RP1, 192
    }, {
        0, 47, 0, { 1280, 720 }, { 1650, 750 }, { 370, 30 }, { 110, 5 },     /* 36  720p*120 */
        { 40, 5 }, 90, 120, 148, 14850, PROG, POS, POS, NTSC, RP1, 192
    }, {
        48, 49, 0, { 720, 480 }, { 858, 525 }, { 138, 45 }, { 16, 9 },       /* 37  480p*120 */
        { 62, 6 }, 63, 120, 54, 5400, PROG, NEG, NEG, NTSC, RP1, 96
    }, {
        50, 51, 0, { 1440, 480 }, { 1716, 525 }, { 276, 22 }, { 38, 4 },     /* 38  1440*480i*120 */
        { 124, 3 }, 32, 120, 54, 5400, INTL, NEG, NEG, NTSC, RP2, 96
    }, {
        52, 53, 0, { 720, 576 }, { 864, 625 }, { 144, 49 }, { 12, 5 },       /* 39  720*576p*200 */
        { 64, 5 }, 125, 200, 108, 10800, PROG, NEG, NEG, PAL, RP1, 192
    }, {
        54, 55, 0, { 1440, 576 }, { 1728, 625 }, { 288, 24 }, { 24, 2 },     /* 40  1440*576i*200 */
        { 126, 3 }, 63, 200, 108, 10800, INTL, NEG, NEG, PAL, RP2, 192
    }, {
        56, 57, 0, { 720, 480 }, { 858, 525 }, { 138, 45 }, { 16, 9 },       /* 41  720*480p*240 */
        { 62, 6 }, 126, 240, 108, 10800, PROG, NEG, NEG, NTSC, RP1, 192
    }, {
        58, 59, 0, { 1440, 480 }, { 1716, 525 }, { 276, 22 }, { 38, 4 },     /* 42  1440*480i*240 */
        { 124, 3 }, 63, 240, 108, 10800, INTL, NEG, NEG, NTSC, RP2, 192
    }, {
        0, 60, 0, { 1280, 720 }, { 3300, 750 }, { 2020, 30 }, { 1760, 5 },   /* 43 720P24 */
        { 40, 5 }, 18, 24, 59, 5940, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 61, 0, { 1280, 720 }, { 3960, 750 }, { 2680, 30 }, { 2420, 5 },   /* 44 720P25 */
        { 40, 5 }, 18, 25, 74, 7425, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 62, 0, { 1280, 720 }, { 3300, 750 }, { 2020, 30 }, { 1760, 5 },   /* 45 720P30 */
        { 40, 5 }, 22, 30, 74, 7425, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0,  63, 0, { 1920, 1080 }, { 2200, 1125 }, { 280, 45 }, { 88, 4 },  // 46 1920*1080p*120
        { 44, 5 }, 135, 120, 297, 29700, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 64, 0, { 1920, 1080 }, { 2640, 1125 }, { 720, 45 }, { 528, 4 },   /* 47 1920*1080p*100 */
        { 44, 5 }, 113, 100, 297, 29700, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 79, 0, { 1680, 720 }, { 3300, 750 }, { 1620, 30 }, { 1360, 5 },   /* 48 1680*720p *24 */
        { 40, 5 }, 18, 24, 59, 5940, PROG, POS, POS, 0, RP1, 192
    }, {
        0, 80, 0, { 1680, 720 }, { 3168, 750 }, { 1488, 30 }, { 1228, 5 },   /* 49 1680*720p *25 */
        { 40, 5 }, 19, 25, 59, 5940, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 81, 0, { 1680, 720 }, { 2640, 750 }, { 960, 30 }, { 700, 5 },     /* 50 1680*720p *30 */
        { 40, 5 }, 23, 30, 59, 5940, PROG, POS, POS, 0, RP1, 192
    }, {
        0, 82, 0, { 1680, 720 }, { 2200, 750 }, { 520, 30 }, { 260, 5 },     /* 51 1680*720p *50 */
        { 40, 5 }, 38, 50, 82, 8250, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 83, 0, { 1680, 720 }, { 2200, 750 }, { 520, 30 }, { 260, 5 },     /* 52 1680*720p *60 */
        { 40, 5 }, 45, 60, 99, 9900, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 84, 0, { 1680, 720 }, { 2000, 825 }, { 320, 105 }, { 60, 5 },     /* 53 1680*720p *100 */
        { 40, 5 }, 83, 100, 165, 16500, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 85, 0, { 1680, 720 }, { 2000, 825 }, { 320, 105 }, { 60, 5 },     /* 54 1680*720p *120 */
        { 40, 5 }, 99, 120, 198, 19800, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 86, 0, { 2560, 1080 }, { 3750, 1100 }, { 1190, 20 }, { 998, 4 },  /* 55 2560*1080p*24 */
        { 44, 5 }, 26, 24, 99, 9900, PROG, POS, POS, 0, RP1, 192
    }, {
        0, 87, 0, { 2560, 1080 }, { 3200, 1125 }, { 640, 45 }, { 448, 4 },   /* 56 2560*1080p*25 */
        { 44, 5 }, 28, 25, 90, 9000, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 88, 0, { 2560, 1080 }, { 3520, 1125 }, { 960, 45 }, { 768, 4 },   /* 57 2560*1080p*30 */
        { 44, 5 }, 34, 30, 118, 11880, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 89, 0, { 2560, 1080 }, { 3300, 1125 }, { 740, 45 }, { 548, 4 },   /* 58 2560*1080p*50 */
        { 44, 5 }, 56, 50, 185, 18562, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 90, 0, { 2560, 1080 }, { 3000, 1100 }, { 440, 20 }, { 248, 4 },   /* 59 2560*1080p*60 */
        { 44, 5 }, 66, 60, 198, 19800, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 91, 0, { 2560, 1080 }, { 2970, 1250 }, { 410, 170 }, { 218, 4 },  /* 60 2560*1080p*100 */
        { 44, 5 }, 125, 100, 371, 37125, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 92, 0, { 2560, 1080 }, { 3300, 1250 }, { 740, 170 }, { 548, 4 },  /* 61 2560*1080p*120 */
        { 44, 5 }, 150, 120, 495, 49500, PROG, POS, POS, NTSC, RP1, 192
    }, { /* HDMI 1.4a video modes */
        0, 93, 3, { 3840, 2160 }, { 5500, 2250 }, { 1660, 90 }, { 1276, 8 }, /* 62 4k*2k*24 */
        { 88, 10 }, 132, 24, 297, 29700, PROG, POS, POS, 0, RP1, 192
    }, {
        0, 94, 2, { 3840, 2160 }, { 5280, 2250 }, { 1440, 90 }, { 1056, 8 }, /* 63 4k*2k*25 */
        { 88, 10 }, 132, 25, 297, 29700, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 95, 1, { 3840, 2160 }, { 4400, 2250 }, { 560, 90 }, { 176, 8 },   /* 64 4k*2k*30 */
        { 88, 10 }, 132, 30, 297, 29700, PROG, POS, POS, NTSC, RP1, 96
    }, {
        0, 96, 0, { 3840, 2160 }, { 5280, 2250 }, { 1440, 90 }, { 1056, 8 }, /* 65 3840*2160p*50 */
        { 88, 10 }, 113, 50, 594, 59400, PROG, POS, POS, PAL, RP1, 96
    }, {
        0, 97, 0, { 3840, 2160 }, { 4400, 2250 }, { 560, 90 }, { 176, 8 },   /* 66 3840*2160p*60 */
        { 88, 10 }, 135, 60, 594, 59400, PROG, POS, POS, NTSC, RP1, 192
    }, { /* 67 SMPTE (caannot be distingushed from #3) */
        0, 98, 4, { 4096, 2160 }, { 5500, 2250 }, { 1404, 90 }, { 1020, 8 },
        { 88, 10 }, 132, 24, 297, 29700, PROG, POS, POS, 0, RP1, 192
    }, {
        0, 99, 0, { 4096, 2160 }, { 5280, 2250 }, { 1184, 90 }, { 968, 8 },  /* 68 4096*2160p*25 */
        { 88, 10 }, 56, 25, 297, 29700, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 100, 0, { 4096, 2160 }, { 4400, 2250 }, { 304, 90 }, { 88, 8 },   /* 69 4096*2160p*30 */
        { 88, 10 }, 68, 30, 297, 29700, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 101, 0, { 4096, 2160 }, { 5280, 2250 }, { 1184, 90 }, { 968, 8 }, /* 70 4096*2160p*50 */
        { 88, 10 }, 113, 50, 594, 59400, PROG, POS, POS, PAL, RP1, 192
    }, {
        0, 102, 0, { 4096, 2160 }, { 4400, 2250 }, { 304, 90 }, { 88, 8 },   /* 71 4096*2160p*60 */
        { 88, 10 }, 135, 60, 594, 59400, PROG, POS, POS, NTSC, RP1, 192
    }, {
        0, 0, 0, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

static const hi_u32 g_vic2_idx_table[LAST_KNOWN_VIC + 1] = {
    TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES,
    /* no VIC (dummy, to start real indexes from 1, but not from 0) */
    0,  /* VIC:1 */
    1,  /* VIC:2 */
    1,  /* VIC:3 */
    2,  /* VIC:4 */
    3,  /* VIC:5 */
    4,  /* VIC:6 */
    4,  /* VIC:7 */
    5,  /* VIC:8  possible indexes: 5,6 */
    5,  /* VIC:9  possible indexes: 5,6 */
    7,  /* VIC:10 */
    7,  /* VIC:11 */
    8,  /* VIC:12 possible indexes: 8,9 */
    8,  /* VIC:13 possible indexes: 8,9 */
    10, /* VIC:14 */
    10, /* VIC:15 */
    11, /* VIC:16 */
    12, /* VIC:17 */
    12, /* VIC:18 */
    13, /* VIC:19 */
    14, /* VIC:20 */
    15, /* VIC:21 */
    15, /* VIC:22 */
    16, /* VIC:23 possible indexes: 16,17,18 */
    16, /* VIC:24 possible indexes: 16,17,18 */
    19, /* VIC:25 */
    19, /* VIC:26 */
    20, /* VIC:27 possible indexes: 20,21,22 */
    20, /* VIC:28 possible indexes: 20,21,22 */
    23, /* VIC:29 */
    23, /* VIC:30 */
    24, /* VIC:31 */
    25, /* VIC:32 */
    26, /* VIC:33 */
    27, /* VIC:34 */
    28, /* VIC:35 */
    28, /* VIC:36 */
    29, /* VIC:37 */
    29, /* VIC:38 */
    30, /* VIC:39 */
    31, /* VIC:40 */
    32, /* VIC:41 */
    33, /* VIC:42 */
    33, /* VIC:43 */
    34, /* VIC:44 */
    34, /* VIC:45 */
    35, /* VIC:46 */
    36, /* VIC:47 */
    37, /* VIC:48 */
    37, /* VIC:49 */
    38, /* VIC:50 */
    38, /* VIC:51 */
    39, /* VIC:52 */
    39, /* VIC:53 */
    40, /* VIC:54 */
    40, /* VIC:55 */
    41, /* VIC:56 */
    41, /* VIC:57 */
    42, /* VIC:58 */
    42, /* VIC:59 */
    43, /* VIC:60 */
    44, /* VIC:61 */
    45, /* VIC:62 */
    46, /* VIC:63 */
    47, /* VIC:64 */
    43, /* VIC:65 */
    44, /* VIC:66 */
    45, /* VIC:67 */
    13, /* VIC:68 */
    2,  /* VIC:69 */
    32, /* VIC:70 */
    36, /* VIC:71 */
    25, /* VIC:72 */
    26, /* VIC:73 */
    27, /* VIC:74 */
    24, /* VIC:75 */
    11, /* VIC:76 */
    47, /* VIC:77 */
    46, /* VIC:78 */
    48, /* VIC:79 */
    49, /* VIC:80 */
    50, /* VIC:81 */
    51, /* VIC:82 */
    52, /* VIC:83 */
    53, /* VIC:84 */
    54, /* VIC:85 */
    55, /* VIC:86 */
    56, /* VIC:87 */
    57, /* VIC:88 */
    58, /* VIC:89 */
    59, /* VIC:90 */
    60, /* VIC:91 */
    61, /* VIC:92 */
    62, /* VIC:93 */
    63, /* VIC:94 */
    64, /* VIC:95 */
    65, /* VIC:96 */
    66, /* VIC:97 */
    67, /* VIC:98 */
    68, /* VIC:99 */
    69, /* VIC:100 */
    70, /* VIC:101 */
    71, /* VIC:102 */
};

static hi_void hdmirx_video_correct_sync_info(hi_drv_hdmirx_port port, signal_timing_info *sync_info)
{
    hdmirx_video_ctx *video_ctx;

    hi_u32 vactive;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    if (video_ctx->special_case != hdmirx_special_case_vsync_invert) {
        return;
    }

    vactive = hal_video_get_vactive(port, HI_FALSE);

    switch (vactive) {
        case V_ACTIVE_480:
            sync_info->total_lines = V_TOTAL_480;
            break;
        case V_ACTIVE_576:
            sync_info->total_lines = V_TOTAL_576;
            break;
        case V_ACTIVE_720:
            sync_info->total_lines = V_TOTAL_720;
            break;
        case V_ACTIVE_1080:
            sync_info->total_lines = V_TOTAL_1080;
            break;
        default:
            break;
    }
}

static hi_void hdmirx_video_get_sync_info(hi_drv_hdmirx_port port, signal_timing_info *sync_info)
{
    hdmirx_color_space color_space;
    hi_u32 value;
    hdmirx_video_ctx *video_ctx;
    hi_u32 tmp1, tmp2;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    color_space = hdmirxv2_packet_avi_get_color_space(port);
    if (color_space == HDMIRX_COLOR_SPACE_YCBCR420) {
        value = 2; /* 2: 420 color space 2 times htotal, pixel_clock */
    } else {
        value = 1;
    }
    sync_info->clocks_per_line = hal_video_get_htotal(port, HI_FALSE) * value;
    sync_info->total_lines = hal_video_get_vtotal(port, HI_FALSE);

    sync_info->pixel_freq = hal_video_get_pix_clk(port) * value;

    sync_info->interlaced = hal_video_get_interlance(port);
    sync_info->h_pol = hal_video_get_hpol(port);
    sync_info->v_pol = hal_video_get_vpol(port);
    sync_info->hactive = hal_video_get_hactive(port, HI_FALSE) * value;
    sync_info->vactive = hal_video_get_vactive(port, HI_FALSE);
    if (sync_info->interlaced == HI_TRUE) {
        tmp1 = hal_video_get_vtotal4_odd(port, HI_FALSE);
        tmp2 = hal_video_get_vtotal(port, HI_FALSE);
        if ((abs_diff(tmp1, tmp2) > HDMIRX_INTERLACE_FIELD_THRESHOLD)) { /* 10: check value */
            video_ctx->special_case = hdmirx_special_case_vsync_invert;
            sync_info->interlaced = HI_FALSE;
            hdmirx_video_correct_sync_info(port, sync_info);
        } else {
            sync_info->total_lines = tmp1 + tmp2;
            video_ctx->special_case = hdmirx_special_case_null;
        }
    } else {
        video_ctx->special_case = hdmirx_special_case_null;
    }
}

hi_void hdmirxv2_video_analyse_mode_chg_type(hdmirx_video_ctx *video_ctx, signal_timing_info sync_info,
    hi_u32 hactive_measured, hi_u32 vactive_measured, hi_u32* mode_chg_type)
{
    if ((abs_diff(sync_info.clocks_per_line, video_ctx->hdmirx_timing_info.htotal) > MODE_CHANGE_H_TOTAL_THR) ||
        (abs_diff(sync_info.total_lines, video_ctx->hdmirx_timing_info.vtotal) > MODE_CHANGE_V_TOTAL_THR)) {
        hi_dbg_hdmirx("new Htotal: %d, Vtotal: %d\n", sync_info.clocks_per_line, sync_info.total_lines);
        hi_dbg_hdmirx("old Htotal: %d, Vtotal: %d\n", video_ctx->hdmirx_timing_info.htotal,
            video_ctx->hdmirx_timing_info.vtotal);
        *mode_chg_type |= MODE_CHG_HVRES;
    }

    if ((abs_diff(hactive_measured, video_ctx->hdmirx_timing_info.hactive) > MODE_CHANGE_H_TOTAL_THR) ||
        (abs_diff(vactive_measured, video_ctx->hdmirx_timing_info.vactive) > MODE_CHANGE_V_TOTAL_THR)) {
        hi_dbg_hdmirx("new Hactive: %d, Vactive: %d\n", hactive_measured, vactive_measured);
        hi_dbg_hdmirx("old Hactive: %d, Vactive: %d\n", video_ctx->hdmirx_timing_info.hactive,
            video_ctx->hdmirx_timing_info.vactive);
        *mode_chg_type |= MODE_CHG_HVRES;
    }

    if (abs_diff(sync_info.pixel_freq, video_ctx->hdmirx_timing_info.pix_freq)\
        > (video_ctx->hdmirx_timing_info.pix_freq / 10)) { /* 10: div 10 */
        hi_dbg_hdmirx("new Pixel_freq: %d\n", sync_info.pixel_freq);
        hi_dbg_hdmirx("old Pixel_freq: %d\n", video_ctx->hdmirx_timing_info.pix_freq);

        *mode_chg_type |= MODE_CHG_PIXCLK;
    }
}

hi_u32 hdmirxv2_video_get_mode_chg_type(hi_drv_hdmirx_port port)
{
    signal_timing_info sync_tmp;
    hi_u32 mode_chg_type = 0;
    hdmirx_video_ctx *video_ctx;
    hi_u32 hactive_measured;
    hi_u32 vactive_measured;

    hi_u32 value;
    hdmirx_color_space color_space;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    hdmirx_video_get_sync_info(port, &sync_tmp);

    color_space = hdmirxv2_packet_avi_get_color_space(port);
    if (color_space == HDMIRX_COLOR_SPACE_YCBCR420) {
        value = 2; /* 2: 420 color space 2 times hactive */
    } else {
        value = 1;
    }

    hactive_measured = hal_video_get_hactive(port, HI_FALSE) * value;
    vactive_measured = hal_video_get_vactive(port, HI_FALSE);
    vactive_measured = (sync_tmp.interlaced ? (vactive_measured << 1) : vactive_measured);

    hdmirxv2_video_analyse_mode_chg_type(video_ctx, sync_tmp, hactive_measured, vactive_measured, &mode_chg_type);
    return mode_chg_type;
}

hi_bool hdmirxv2_video_is_need_mode_change(hi_drv_hdmirx_port port, hi_u32 type)
{
    hdmirx_video_ctx *video_ctx;
    hi_u32 cnt;
    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    if (video_ctx->hdmirx_timing_info.video_idx == TIMING_PC_MODE) {
        cnt = 2; /* PC mode, mode change check 2 times */
    } else {
        cnt = 4; /* other mode, mode change check 4 times */
    }

    if (type & MODE_CHG_HVRES) {
        video_ctx->mode_chg_hv_res_cnt++;
    } else {
        video_ctx->mode_chg_hv_res_cnt = 0;
    }
    if (type & MODE_CHG_PIXCLK) {
        video_ctx->mode_chg_pix_clk_cnt++;
    } else {
        video_ctx->mode_chg_pix_clk_cnt = 0;
    }
    if ((video_ctx->mode_chg_hv_res_cnt > cnt) || (video_ctx->mode_chg_pix_clk_cnt > cnt)) {
        video_ctx->mode_chg_hv_res_cnt = 0;
        video_ctx->mode_chg_pix_clk_cnt = 0;
        return HI_TRUE;
    }
    if (video_ctx->hdmi_mode != hdmirxv2_video_get_hdmi_mode(port)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}
hi_bool hdmirxv2_video_get_hdmi_mode(hi_drv_hdmirx_port port)
{
    return hal_video_get_hdmi_mode(port);
}
hi_bool hdmirxv2_video_is_support(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    if ((video_ctx->hdmirx_timing_info.video_idx != TIMING_NOT_SUPPORT) &&
        (video_ctx->hdmirx_timing_info.video_idx != TIMING_NOSIGNAL)) {
        return HI_TRUE;
    }
    hi_dbg_hdmirx_print_u32(video_ctx->hdmirx_timing_info.video_idx);
    return HI_FALSE;
}
hi_bool hdmirxv2_video_is_dolby_vision(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    if (video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_DOLBY) {
        return HI_TRUE;
    }
    hi_dbg_hdmirx_print_u32(video_ctx->hdmirx_hdr_data.src_type);
    return HI_FALSE;
}
hi_bool hdmirxv2_video_check_stable(hi_drv_hdmirx_port port)
{
    signal_timing_info sync_tmp;
    hdmirx_video_ctx *video_ctx;
    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    hdmirx_video_get_sync_info(port, &sync_tmp);
    if ((sync_tmp.clocks_per_line < H_V_TOTAL_STABLE_TOLERANCE) ||
        (sync_tmp.total_lines < H_V_TOTAL_STABLE_TOLERANCE)) {
        hi_dbg_hdmirx("total value error! htotal: %d, vtotal: %d\n", sync_tmp.clocks_per_line, sync_tmp.total_lines);
        return HI_FALSE;
    }
    if (abs_diff(sync_tmp.clocks_per_line, video_ctx->hdmirx_timing_info.htotal) \
        < PIXELS_TOLERANCE) {
        if (abs_diff(sync_tmp.total_lines, video_ctx->hdmirx_timing_info.vtotal) \
            < LINES_TOLERANCE) {
            video_ctx->mode_stable_cnt++;
            if (video_ctx->mode_stable_cnt >= MODE_STABLE_CNT_THR) {
                video_ctx->hdmirx_timing_info.htotal = sync_tmp.clocks_per_line;
                video_ctx->hdmirx_timing_info.vtotal = sync_tmp.total_lines;
                video_ctx->mode_stable_cnt = 0;
                return HI_TRUE;
            }
        } else {
            hi_dbg_hdmirx("wait: total_lines unstable! htotal: %d, vtotal: %d\n",
                sync_tmp.clocks_per_line, sync_tmp.total_lines);
            video_ctx->mode_stable_cnt = 0;
        }
    } else {
        hi_dbg_hdmirx("wait: clocks_per_line unstable!\n");
        video_ctx->mode_stable_cnt = 0;
    }

    video_ctx->hdmirx_timing_info.htotal = sync_tmp.clocks_per_line;
    video_ctx->hdmirx_timing_info.vtotal = sync_tmp.total_lines;
    return HI_FALSE;
}

hi_void hdmirxv2_video_rst_timing_data(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    video_ctx->hdmirx_timing_info.video_idx = TIMING_NOSIGNAL;
}
hi_void hdmirxv2_video_timing_data_init(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;
    errno_t err_ret;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    err_ret = memset_s(&video_ctx->hdmirx_timing_info, sizeof(video_ctx->hdmirx_timing_info),
        0, sizeof(hdmirx_timing_info));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    err_ret = memset_s(&video_ctx->hdmirx_hdr_data, sizeof(video_ctx->hdmirx_hdr_data),
        0, sizeof(hi_hdmirx_dynamic_source_data));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    video_ctx->hdmirx_hdr_data.src_type = HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN;
    video_ctx->hdmirx_timing_info.video_idx = TIMING_NOSIGNAL;
    video_ctx->input_width = HDMIRX_INPUT_WIDTH_MAX;
    video_ctx->mode_chg_hv_res_cnt = 0;
    video_ctx->mode_chg_pix_clk_cnt = 0;
}
hi_void hdmirxv2_video_set_deep_color_mode(hi_drv_hdmirx_port port)
{
    hdmirx_input_width pd;
    hdmirx_video_ctx *video_ctx;
    hdmirx_color_space in_color_space;
    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    /* read incoming pixel depth from latest general control packet */
    pd = hal_video_get_deep_color(port);
    in_color_space = hdmirxv2_packet_avi_get_color_space(port);
    if (in_color_space == HDMIRX_COLOR_SPACE_YCBCR422) {
        pd = HDMIRX_INPUT_WIDTH_36;
    }
    if (pd != video_ctx->input_width) { /* if it differs from the current setting update current setting */
        video_ctx->input_width = pd;
        /* update the value in hardware */
        if (port < HI_DRV_HDMIRX_PORT_MAX) {
        }
    }
}
static hi_u32 hdmirx_video_get_timing_idx_from_vsif(hi_drv_hdmirx_port port)
{
    hi_u32 index = TIMING_NOT_SUPPORT;
    hi_u32 cea_vic = 0;
    hi_bool got_avi;
    hi_bool got3d;
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    got_avi = hdmirxv2_packet_avi_is_got(port);
    got3d = hdmirxv2_packet_vsif_is_got3d(port);
    if ((got_avi == HI_TRUE) && (got3d == HI_TRUE)) {
        cea_vic = video_ctx->hdmirx_timing_info.cea861_vic;
        if ((cea_vic > 0) && (cea_vic <= LAST_KNOWN_VIC)) {
            index = g_vic2_idx_table[cea_vic] | HDMI_3D_RESOLUTION_MASK;
        }
    }

    return index;
}
static hi_bool hdmirx_video_is_timing_in_range(signal_timing_info *sync_info)
{
    hi_bool pass = HI_FALSE;
    hi_u32 pixel_clk = 0;
    hi_u32 h_freq = 0;
    hi_u32 v_freq = 0;

    if ((sync_info->clocks_per_line > H_V_TOTAL_IN_RANGE_TOLERANCE) &&
        (sync_info->total_lines > H_V_TOTAL_IN_RANGE_TOLERANCE)) {
        pixel_clk = sync_info->pixel_freq;
        h_freq = ((sync_info->pixel_freq) * 10 + 5) / sync_info->clocks_per_line; // 10k to k, times 10,add 5 tolerance
        v_freq = (h_freq * 1000 + 500) / sync_info->total_lines; /* 1000: from MHz to KHz. 500: for rounding */
        if ((pixel_clk <= (((hi_u32)VIDEO_MAX_PIX_CLK_10MHZ) * 1000 + FPIX_TOLERANCE)) && /* M to K, times 1000 */
            /* 2M to k, times 1000 */
            (pixel_clk > (((hi_u32)2) * 1000)) && (h_freq + FH_TOLERANCE >= VIDEO_MIN_H_KHZ) &&
                (h_freq <= (VIDEO_MAX_H_KHZ + FH_TOLERANCE))) { /* + FPIX_TOLERANCE)))  in 1 k_hz units */
            if ((v_freq + FV_TOLERANCE >= VIDEO_MIN_V_HZ) && (v_freq <= VIDEO_MAX_V_HZ + FV_TOLERANCE)) {
                pass = HI_TRUE;
            }
        }
    }
    if ((sync_info->hactive < H_V_ACTIVE_IN_RANGE_TOLERANCE) || (sync_info->vactive < H_V_ACTIVE_IN_RANGE_TOLERANCE)) {
        hi_dbg_hdmirx("invalid hactive: %d, vactive: %d\n", sync_info->hactive, sync_info->vactive);
        pass = HI_FALSE;
    }
    return pass;
}
hi_void hdmirx_video_get861_table_index(signal_timing_info *sync_info, hi_u32 hactive_measured,
    hi_u32 vactive_measured, hi_u32* timing_index)
{
    hi_u32 i;
    hi_bool is_interlaced;
    hi_u32 vtotal_measured;
    video_timing_define *video_table_p = HI_NULL;

    for (i = 0; g_video_timing_table[i].vic4x3 || g_video_timing_table[i].vic16x9; i++) {
        video_table_p = (video_timing_define *)(&g_video_timing_table[i]);
        is_interlaced = sync_info->interlaced;
        vtotal_measured = sync_info->total_lines;
        if (is_interlaced != video_table_p->interlaced) {
            continue;
        }
        /* check number of lines */
        if (abs_diff(vtotal_measured, video_table_p->total.v) > LINES_TOLERANCE) {
            continue;
        }

        /* check number of clocks per line (it works for all possible replications) */
        if (abs_diff(sync_info->clocks_per_line, video_table_p->total.h) > PIXELS_TOLERANCE) {
            continue;
        }

        if (hactive_measured > 1920) { /* 1920 : fhd Hactive */
            if (abs_diff(hactive_measured, video_table_p->active.h) > ACTIVE_TOLERANCE) {
                continue;
            }
        } else {
            if (abs_diff(hactive_measured, video_table_p->active.h) >= ACTIVE_TOLERANCE) {
                continue;
            }
        }

        if (abs_diff(vactive_measured, video_table_p->active.v) > ACTIVE_TOLERANCE) {
            continue;
        }

        if (sync_info->pixel_freq > 34000) { /* 34000: 340Mhz, 4K */
            if (abs_diff(sync_info->pixel_freq, video_table_p->pix_clk) > FPIX_TOLERANCE * 2) { /* 2 times tolerance */
                continue;
            }
        } else {
            if (abs_diff(sync_info->pixel_freq, video_table_p->pix_clk) > FPIX_TOLERANCE) {
                continue;
            }
        }
        /* check pixel freq (in 10k_hz units) */
        *timing_index = i;
        break;
    }
}
static hi_u32 hdmirx_video_search861(hi_drv_hdmirx_port port, signal_timing_info *sync_info)
{
    hi_u32 timing = TIMING_NOT_SUPPORT;
    hi_u32 value;
    hi_u32 hactive_measured;
    hi_u32 vactive_measured;
    hdmirx_color_space color_space;

    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        port = HI_DRV_HDMIRX_PORT0;
    }
    color_space = hdmirxv2_packet_avi_get_color_space(port);
    if (color_space == HDMIRX_COLOR_SPACE_YCBCR420) {
        value = 2; /* 2: 420 color space 2 times hactive */
    } else {
        value = 1;
    }

    hactive_measured = hal_video_get_hactive(port, HI_FALSE) * value;
    vactive_measured = hal_video_get_vactive(port, HI_FALSE);
    vactive_measured = (sync_info->interlaced ? (vactive_measured << 1) : vactive_measured);

    hdmirx_video_get861_table_index(sync_info, hactive_measured, vactive_measured, &timing);

    return timing;
}

static hi_u32 hdmirx_video_timing_search(hi_drv_hdmirx_port port, signal_timing_info *sync_info)
{
    hi_u32 timing_idx = TIMING_PC_MODE;  /* TIMING_NOT_SUPPORT; */
    hi_bool in_range;

    hdmirx_video_get_sync_info(port, sync_info);
    hi_dbg_hdmirx("set pol\n");
    hi_dbg_hdmirx("hpol: %d, vpol: %d\n", sync_info->h_pol, sync_info->v_pol);

    in_range = hdmirx_video_is_timing_in_range(sync_info);
    if (in_range == HI_TRUE) {
        hi_dbg_hdmirx("timing in range\n");
        timing_idx = hdmirx_video_search861(port, sync_info);
        if (timing_idx == TIMING_NOT_SUPPORT) {
            hi_bool is_interlace = hal_video_get_interlance(port);
            if (is_interlace) { /* CEA表之外的，如果隔行就返回不支持，不PC Mode */
                timing_idx = TIMING_NOT_SUPPORT;
            } else {
                timing_idx = TIMING_PC_MODE;
            }
        }
#if defined (CHIP_TYPE_hi3751v350)
        if (sync_info->pixel_freq > 180 * 100) { /* 180 * 100: 180Mhz in 10 khz */
            timing_idx = TIMING_NOT_SUPPORT;
        }
#endif
    } else {
        timing_idx = TIMING_NOT_SUPPORT;
    }

    return timing_idx;
}
static hi_bool hdmirx_video_fill_timing_info_from_table(signal_timing_info *info, hi_u32 idx)
{
    hi_bool success = HI_FALSE;

    if (idx < (TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES)) {
        const video_timing_define *past_table = &g_video_timing_table[idx];
        info->interlaced = past_table->interlaced;
        info->clocks_per_line = past_table->total.h;
        info->total_lines = past_table->total.v;
        info->pixel_freq = past_table->pix_clk;
        info->h_pol = past_table->h_pol;
        info->v_pol = past_table->v_pol;
        info->hactive = past_table->active.h;
        info->vactive = past_table->active.v;
        if (info->interlaced) {
            info->total_lines /= 2; /* 2: div 2 */
        }
        success = HI_TRUE;
    }
    return success;
}
static hi_void hdmirx_video_correct_timing_info_to3d(hi_drv_hdmirx_port port, signal_timing_info *sync_info)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    switch (video_ctx->hdmirx_timing_info.hdmi3d_structure) {
        case 0: /* case 0, frame packing: progressive/interlaced */
            if (sync_info->interlaced) {
                /*
                 * one frame contains even and odd images and even it is interlaced ia a scaler perspective,
                 * it appears as progressive format for HDMI chip HW.
                 */
                sync_info->total_lines *= 2; /* interlace v_total times 2 */
            }
            /* fall-through */
        case 2: /* case 2, line alternative: progressive only */
            /* L + depth: progressive only */
            /* multiply lines x2; multiply clock x2 */
            sync_info->total_lines *= 2; /* 2: multiply lines */
            sync_info->pixel_freq *= 2; /* 2: multiply clock */
            break;

        case 1: /* case 1, field alternative: interlaced only */
            /* multiply clock x2 */
            sync_info->pixel_freq *= 2; /* 2: multiply clock */
            break;

        case 3: /* case 3, side-by-side (full): progressive/interlaced */
            /* multiply pixel x2; multiply clock x2 */
            sync_info->clocks_per_line *= 2; /* 2: multiply clock */
            sync_info->pixel_freq *= 2; /* 2: multiply pixel */
            break;
        default: /* 2D timing compatible: progressive/interlaced */
            break;
    }
}

hi_void hdmirxv2_video_update_timing_info(hi_drv_hdmirx_port port, hdmirx_video_ctx *video_ctx, hi_u32 temp)
{
    hdmirx_3d_type type;

    video_ctx->hdmirx_timing_info.frame_rate = g_video_timing_table[temp].v_freq;
    video_ctx->hdmirx_timing_info.vactive = hal_video_get_vactive(port, HI_FALSE);
    type = video_ctx->hdmirx_timing_info.hdmi3d_structure;
    if ((type == HDMIRX_3D_TYPE_SBS_FULL) || (type == HDMIRX_3D_TYPE_SBS_HALF) \
        || (type == HDMIRX_3D_TYPE_TB)) {
        if (hal_video_get_interlance(port) == HI_TRUE) {
            video_ctx->hdmirx_timing_info.vactive *= 2; /* interlace 2 times vactive */
        }
    }
    video_ctx->hdmirx_timing_info.hactive = g_video_timing_table[temp].active.h;
    if (video_ctx->hdmirx_timing_info.hdmi3d_structure == HDMIRX_3D_TYPE_SBS_FULL) {
        video_ctx->hdmirx_timing_info.hactive *= 2; /* 3d sbs full, 2 times havtive */
    }
    return;
}

hi_s32 hdmirxv2_video_mode_match(hi_drv_hdmirx_port port, hdmirx_video_ctx *video_ctx, hi_u32 timing_idx)
{
    hdmirx_color_space color_space;
    hi_u32 value;
    hi_u32 temp = 0;

    if (timing_idx < (TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES)) { /* 2d case */
        video_ctx->hdmirx_timing_info.vactive = g_video_timing_table[timing_idx].active.v;
        video_ctx->hdmirx_timing_info.hactive = g_video_timing_table[timing_idx].active.h;
        video_ctx->hdmirx_timing_info.frame_rate = g_video_timing_table[timing_idx].v_freq;

        return HI_SUCCESS;
    } else if (timing_idx == TIMING_PC_MODE) { /* pc timing case */
        color_space = hdmirxv2_packet_avi_get_color_space(port);
        if (color_space == HDMIRX_COLOR_SPACE_YCBCR420) {
            value = 2; /* 2: 420 color space 2 times hactive */
        } else {
            value = 1;
        }
        video_ctx->hdmirx_timing_info.vactive = hal_video_get_vactive(port, HI_FALSE);

        if (hal_video_get_interlance(port) == HI_TRUE) {
            video_ctx->hdmirx_timing_info.vactive *= 2; /* interlace 2 times vactive */
        }
        video_ctx->hdmirx_timing_info.hactive = hal_video_get_hactive(port, HI_FALSE) * value;
        video_ctx->hdmirx_timing_info.frame_rate = hdmirxv2_video_get_frame_rate(port);
        return HI_SUCCESS;
    } else if ((timing_idx & HDMI_3D_RESOLUTION_MASK) && (timing_idx < TIMING_NOT_SUPPORT)) { /* 3d case */
        temp = (timing_idx & ~HDMI_3D_RESOLUTION_MASK);
        if (temp < TIMING_MAX) {
            hdmirxv2_video_update_timing_info(port, video_ctx, temp);
            return HI_SUCCESS;
        }
    }
    return HI_FAILURE;
}

hi_void hdmirxv2_video_set_unsupport_timing_info(hi_drv_hdmirx_port port,
    hdmirx_video_ctx *video_ctx, hi_u32 timing_idx, signal_timing_info sync_info)
{
    hi_u32 value;
    hdmirx_color_space color_space;

    color_space = hdmirxv2_packet_avi_get_color_space(port);
    if (color_space == HDMIRX_COLOR_SPACE_YCBCR420) {
        value = 2; /* 2: 420 color space 2 times hactive */
    } else {
        value = 1;
    }
    video_ctx->hdmirx_timing_info.vactive = hal_video_get_vactive(port, HI_FALSE);

    if (hal_video_get_interlance(port) == HI_TRUE) {
        video_ctx->hdmirx_timing_info.vactive *= 2; /* interlace 2 times vactive */
    }
    video_ctx->hdmirx_timing_info.hactive = hal_video_get_hactive(port, HI_FALSE) * value;
    video_ctx->hdmirx_timing_info.frame_rate = hdmirxv2_video_get_frame_rate(port);
    video_ctx->hdmirx_timing_info.video_idx = timing_idx;
    video_ctx->hdmirx_timing_info.pix_freq = sync_info.pixel_freq;
}

hi_void hdmirxv2_video_mode_det(hi_drv_hdmirx_port port)
{
    signal_timing_info sync_info;
    hi_u32 timing_idx;
    hi_bool result;
    hi_bool valid;
    hi_s32 ret;
    hi_u32 temp = 0;
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    timing_idx = hdmirx_video_get_timing_idx_from_vsif(port);
    sync_info.pixel_freq = 0;
    if (timing_idx == TIMING_NOT_SUPPORT) {
        timing_idx = hdmirx_video_timing_search(port, &sync_info);
        if (timing_idx == TIMING_NOT_SUPPORT) { /* beyond 861 video table and interlace, keep not support */
            hdmirxv2_video_set_unsupport_timing_info(port, video_ctx, timing_idx, sync_info);
            return;
        }
    } else if (timing_idx & HDMI_3D_RESOLUTION_MASK) {
        temp = (timing_idx & ~HDMI_3D_RESOLUTION_MASK);
        result = hdmirx_video_fill_timing_info_from_table(&sync_info, temp);
        if (result == HI_TRUE) {
            hdmirx_video_correct_timing_info_to3d(port, &sync_info);
            valid = hdmirx_video_is_timing_in_range(&sync_info);
            if (valid == HI_FALSE) {
                /* input video is out of range, do not use it. */
                timing_idx = TIMING_NOT_SUPPORT;
            }
        } else {
            timing_idx = TIMING_NOT_SUPPORT;
        }
    }
    video_ctx->hdmirx_timing_info.video_idx = timing_idx;
    video_ctx->hdmirx_timing_info.pix_freq = sync_info.pixel_freq;
    ret = hdmirxv2_video_mode_match(port, video_ctx, timing_idx);
    if (ret == HI_SUCCESS) { /* match success */
        return;
    }
    video_ctx->hdmirx_timing_info.vactive = 1;
    video_ctx->hdmirx_timing_info.hactive = 1;
    video_ctx->hdmirx_timing_info.frame_rate = 1;
}

static hi_void hdmirx_video_set_blank_lv(hi_drv_hdmirx_port port,
    hdmirx_color_space en_output_color_space, hi_u32 idx)
{
    hi_u32 blank_levels[3]; /* 3: array size */

    /* default are levels for RGB PC modes */
    blank_levels[0] = 0;
    blank_levels[1] = 0;
    blank_levels[2] = 0; /* 2: assign blank level 3st data */

    switch (en_output_color_space) {
        case HDMIRX_COLOR_SPACE_RGB:
            switch (idx) {
                case TIMING_PC_MODE:
                case TIMING_1_640X480P:
                case (TIMING_1_640X480P | HDMI_3D_RESOLUTION_MASK):
                    /* RGB IT - all 0s */
                    break;
                default:
                    /* RGB CE */
                    blank_levels[0] = 0x10;
                    blank_levels[1] = 0x10;
                    blank_levels[2] = 0x10; /* 2: assign blank level 3st data */
            }
            break;
        case HDMIRX_COLOR_SPACE_YCBCR444:
            /* y_cb_cr 4:4:4 */
            blank_levels[0] = 0x80;
            blank_levels[1] = 0x10;
            blank_levels[2] = 0x80; /* 2: assign blank level 3st data */
            break;
        case HDMIRX_COLOR_SPACE_YCBCR422:
            /* y_cb_cr 4:2:2 */
            blank_levels[0] = 0x00;
            blank_levels[1] = 0x10;
            blank_levels[2] = 0x80; /* 2: assign blank level 3st data */
            break;
        case HDMIRX_COLOR_SPACE_YCBCR420:
            /* y_cb_cr 4:2:2 */
            blank_levels[0] = 0x00;
            blank_levels[1] = 0x10;
            blank_levels[2] = 0x80; /* 2: assign blank level 3st data */
            break;
        default:
            break;
    }
}

static hi_void hdmirx_video_set_color_path(hi_drv_hdmirx_port port,
    hdmirx_color_space output_format, hdmirx_color_space in_color_space)
{
    hdmirx_oversample replication;

    replication = hdmirxv2_packet_avi_get_replication(port);
    hal_video_set_channel_map(port, output_format);
}

hi_void hdmirxv2_video_set_video_path(hi_drv_hdmirx_port port)
{
    hi_u32 idx;
    hdmirx_color_space in_color_space;
    hdmirx_color_space out_color_space;
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    idx = video_ctx->hdmirx_timing_info.video_idx;

    /* get the color space of signal. */
    if (hdmirxv2_video_get_hdmi_mode(port) == HI_TRUE) {
        in_color_space = hdmirxv2_packet_avi_get_color_space(port);
    } else {
        in_color_space = HDMIRX_COLOR_SPACE_RGB;
    }

    /* set the output format. */
    if (in_color_space == HDMIRX_COLOR_SPACE_RGB) {
        out_color_space = HDMIRX_COLOR_SPACE_RGB;
    } else if (in_color_space == HDMIRX_COLOR_SPACE_YCBCR420) {
        out_color_space = HDMIRX_COLOR_SPACE_YCBCR420;
    } else if (in_color_space == HDMIRX_COLOR_SPACE_YCBCR422) {
        out_color_space = HDMIRX_COLOR_SPACE_YCBCR422;
    } else {
        out_color_space = HDMIRX_COLOR_SPACE_YCBCR444;
    }

    /* set the blank level accroding the cs. */
    if (out_color_space != HDMIRX_COLOR_SPACE_MAX) {
        hdmirx_video_set_blank_lv(port, out_color_space, idx);
    }

    hdmirx_video_set_color_path(port, out_color_space, in_color_space /* , en_color_m, pc_mode */);
    hal_video_set_mute_value(port, out_color_space);
}
hi_bool hdmirxv2_video_hdmi_dvi_trans(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    if (video_ctx->hdmi_mode != hdmirxv2_video_get_hdmi_mode(port)) {
        video_ctx->hdmi_mode = hdmirxv2_video_get_hdmi_mode(port);
        if (video_ctx->hdmi_mode == HI_TRUE) {
            hal_ctrl_clear_t4_error(port);
        } else {
            /* forget all HDMI settings */
            hdmirxv2_packet_reset_data(port);
            hdmirxv2_packet_avi_set_no_avi_int_en(port, HI_FALSE);
        }
        return HI_TRUE;
    }
    return HI_FALSE;
}

static hi_void hdmirx_video_set_vres_chg_int_en(hi_drv_hdmirx_port port, hi_bool en)
{
}

hi_void hdmirxv2_video_set_res_chg_events_en(hi_drv_hdmirx_port port, hi_bool en)
{
    /* V resolution change interrupts cannot be used in 3D. */
    hdmirx_video_set_vres_chg_int_en(port, !en);
}

hi_void hdmirxv2_video_clear4k3d_info(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    video_ctx->hdmirx_timing_info.hdmi3d_structure = 0;
    video_ctx->hdmirx_timing_info.hdmi3d_ext_data = 0;
}
hi_void hdmirxv2_video_verify1_p4_format(hi_drv_hdmirx_port port, hi_bool vsif_received)
{
    hdmirxv2_ctrl_mode_change(port);
    hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_WAIT);
    hdmirxv2_video_set_res_chg_events_en(port, vsif_received);
}
hi_void hdmirxv2_video_set_video_idx(hi_drv_hdmirx_port port, video_timing_idx idx)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    video_ctx->hdmirx_timing_info.video_idx = idx;
}
hi_void hdmirxv2_video_set861_vic(hi_drv_hdmirx_port port, hi_u32 vic)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    video_ctx->hdmirx_timing_info.cea861_vic = vic;
}
hdmirx_3d_type hdmirxv2_video_get_cur3d_structure(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    return video_ctx->hdmirx_timing_info.hdmi3d_structure;
}
hi_u32 hdmirxv2_video_get_cur3d_ext_data(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    return video_ctx->hdmirx_timing_info.hdmi3d_ext_data;
}
hi_void hdmirxv2_video_set_cur3d_ext_data(hi_drv_hdmirx_port port, hi_u32 td_ext_data)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    video_ctx->hdmirx_timing_info.hdmi3d_ext_data = td_ext_data;
}
hi_void hdmirxv2_video_set_cur3d_structure(hi_drv_hdmirx_port port, hdmirx_3d_type structure)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    video_ctx->hdmirx_timing_info.hdmi3d_structure = structure;
}
hi_u32 hdmirxv2_video_get_pixel_clk(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    return video_ctx->hdmirx_timing_info.pix_freq;
}
hdmirx_input_width hdmirxv2_video_get_input_width(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    return video_ctx->input_width;
}

hi_u32 hdmirxv2_video_get_frame_rate(hi_drv_hdmirx_port port)
{
    hi_u32 rate = 1;
    hi_u32 temp;
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    if (video_ctx->hdmirx_timing_info.video_idx < TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES) {
        rate = g_video_timing_table[video_ctx->hdmirx_timing_info.video_idx].v_freq;
    } else if (video_ctx->hdmirx_timing_info.video_idx == TIMING_PC_MODE) {
        rate = video_ctx->hdmirx_timing_info.pix_freq;
        temp = video_ctx->hdmirx_timing_info.vtotal * video_ctx->hdmirx_timing_info.htotal;
        if (temp == 0) {
            rate = 1;
            return rate;
        }
        rate *= 10000; /* 10k to 1, times 10000 */
        rate = (rate + temp / 2 - 1) / temp; /* add temp div 2 regard as tolerance value */
        if ((rate <= 31) && (rate >= 29)) { /* check from 29 to 31 */
            rate = 30; /* between 29 and 31, set rate to 30 */
        } else if ((rate <= 51) && (rate >= 49)) { /* check from 49 to 51 */
            rate = 50; /* between 49 and 51, set rate to 50 */
        } else if ((rate <= 57) && (rate >= 55)) { /* check from 55 to 57 */
            rate = 56; /* between 55 and 57, set rate to 56 */
        } else if ((rate <= 61) && (rate >= 59)) { /* check from 59 to 61 */
            rate = 60; /* between 59 and 61, set rate to 60 */
        } else if ((rate <= 68) && (rate >= 66)) { /* check from 66 to 68 */
            rate = 67; /* between 66 and 68, set rate to 67 */
        } else if ((rate <= 71) && (rate >= 69)) { /* check from 69 to 71 */
            rate = 70; /* between 69 and 71, set rate to 70 */
        } else if ((rate <= 73) && (rate >= 71)) { /* check from 73 to 71 */
            rate = 72; /* between 73 and 71, set rate to 72 */
        } else if ((rate <= 76) && (rate >= 74)) { /* check from 74 to 76 */
            rate = 75; /* between 74 and 76, set rate to 75 */
        } else if ((rate <= 86) && (rate >= 84)) { /* check from 84 to 86 */
            rate = 85; /* between 84 and 86, set rate to 85 */
        }
    } else if (((hi_u32)video_ctx->hdmirx_timing_info.video_idx) & HDMI_3D_RESOLUTION_MASK) {
        if ((video_ctx->hdmirx_timing_info.video_idx - HDMI_3D_RESOLUTION_MASK) <
            (TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES + 1)) {
            rate = g_video_timing_table[video_ctx->hdmirx_timing_info.video_idx - HDMI_3D_RESOLUTION_MASK].v_freq;
        } else {
            rate = 1;
        }
    }

    return rate;
}
hi_u32 hdmirxv2_video_get_hactive(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    return video_ctx->hdmirx_timing_info.hactive;
}

hi_u32 hdmirxv2_video_get_vactive(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    return video_ctx->hdmirx_timing_info.vactive;
}

hi_bool hdmirxv2_video_is_timing_active(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;
    hi_u32 timing_idx;
    hdmirx_oversample over_sample;
    hi_u32 frame_rate;
    hi_u32 width, height;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    timing_idx = video_ctx->hdmirx_timing_info.video_idx;
    over_sample = hdmirxv2_packet_avi_get_replication(port);
    frame_rate = hdmirxv2_video_get_frame_rate(port);
    width = video_ctx->hdmirx_timing_info.hactive;
    height = video_ctx->hdmirx_timing_info.vactive;

    switch (over_sample) {
        case HDMIRX_OVERSAMPLE_2X:
            width >>= 1;
            break;
        case HDMIRX_OVERSAMPLE_4X:
            width >>= 2; /* 4x oversample, right move 2 bits */
            break;
        default:
            break;
    }

    if (frame_rate > VIDEO_FRAME_RATE_LIMIT) { /* 90: frame rate invalid tolerance */
        return HI_FALSE;
    }
    if (timing_idx == TIMING_NOT_SUPPORT) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

hi_void hdmirxv2_video_get_table_info(hi_u32 u32id, video_timing_define *table_info)
{
    if (u32id < TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES + 1) {
        errno_t err_ret = memcpy_s(table_info, sizeof(video_timing_define),
            &g_video_timing_table[u32id], sizeof(video_timing_define));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return;
        }
    }
}

hi_void hdmirxv2_video_get_hdr_data(hi_drv_hdmirx_port port, hi_hdmirx_dynamic_source_data *data)
{
    hdmirx_video_ctx *video_ctx;
    errno_t err_ret;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    err_ret = memcpy_s(data, sizeof(hi_hdmirx_dynamic_source_data),
        &video_ctx->hdmirx_hdr_data, sizeof(hi_hdmirx_dynamic_source_data));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
}

hi_void hdmirxv2_video_set_hdr_gamma_type(hi_drv_hdmirx_port port, hi_drv_color_descript *color_desc_info)
{
    hi_u8 etof = 0;

    hdmirxv2_packet_hdr10_get_etof(port, &etof);
}
hi_void hdmirxv2_video_set_hdr_luminace_sys(hi_drv_hdmirx_port port, hi_drv_color_descript *color_desc_info)
{
}

hi_void hdmirxv2_video_set_color_prime_coef(hi_drv_hdmirx_port port, hi_drv_color_descript *color_desc_info)
{
}

hi_void hdmirxv2_video_set_hdr_range_flag(hi_drv_hdmirx_port port, hi_drv_color_descript *color_desc_info)
{
}

hi_void hdmirxv2_video_set_dolby_data(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    hi_hdmirx_dolby_vision_info dolby_info;
    errno_t err_ret;
    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    hdmirxv2_packet_dolby_get_meta_data(port, &dolby_info);
    err_ret = memcpy_s(&video_ctx->hdmirx_hdr_data.dolby_info, sizeof(video_ctx->hdmirx_hdr_data.dolby_info),
        &dolby_info, sizeof(hi_hdmirx_dolby_vision_info));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
}

hi_void hdmirxv2_video_set_hdr10_plus_data(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    hi_hdmirx_hdr10_plus_info hdr10_plus_info;
    errno_t err_ret;
    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    hdmirxv2_packet_hdr10_plus_get_meta_data(port, &hdr10_plus_info);
    err_ret = memcpy_s(&video_ctx->hdmirx_hdr_data.hdr10_plus_info, sizeof(video_ctx->hdmirx_hdr_data.hdr10_plus_info),
        &hdr10_plus_info, sizeof(hi_hdmirx_hdr10_plus_info));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
}

hi_void hdmirxv2_video_set_hdr_basic_data(hi_drv_hdmirx_port port,
    hdmirx_video_ctx *video_ctx, hdmirx_hdr10_metadata_stream metadata)
{
}

hi_void hdmirxv2_video_set_hdr_rgb_color_space(hi_drv_hdmirx_port port, hdmirx_video_ctx *video_ctx,
    hdmirx_color_metry color_metry, hdmirx_rgb_range rgb_range)
{
}

hi_void hdmirxv2_video_set_hdr_data_by_color_metry(hi_drv_hdmirx_port port, hdmirx_video_ctx *video_ctx,
    hdmirx_color_metry color_metry, hdmirx_color_space color_space)
{
}

hi_void hdmirxv2_video_set_hdr_data(hi_drv_hdmirx_port port)
{
}

hi_void hdmirxv2_video_set_stream_data(hi_drv_hdmirx_port port)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    hdmirxv2_video_set_hdr_data(port);
    hdmirxv2_video_set_dolby_data(port);
    hdmirxv2_video_set_hdr10_plus_data(port);
    video_ctx->hdmirx_hdr_data.src_type = hdmirx_packet_stream_type(port);
}

hi_void hdmirxv2_video_get_yuv_cs_info(hi_drv_hdmirx_port port,
    hi_drv_hdmirx_timing_info *timing_info, hdmirx_range yuv_range)
{
    if (hdmirxv2_packet_avi_get_color_metry(port) == HDMIRX_COLOR_METRY_ITU601) {
        if (yuv_range == HDMIRX_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
    } else if (hdmirxv2_packet_avi_get_color_metry(port) == HDMIRX_COLOR_METRY_ITU709) {
        if (yuv_range == HDMIRX_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
    } else if ((hdmirxv2_packet_avi_get_color_metry(port) == HDMIRX_COLOR_METRY_BT2020_YCCBCCRC) ||
               (hdmirxv2_packet_avi_get_color_metry(port) == HDMIRX_COLOR_METRY_BT2020_YCBCR)) {
        if (yuv_range == HDMIRX_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT2020;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT;
    } else if (timing_info->height < HDMIRX_HD_HEIGHT) { // 720: check height value
        if (yuv_range == HDMIRX_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
    } else {
        if (yuv_range == HDMIRX_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
    }
    timing_info->color_space.color_space = HI_DRV_COLOR_CS_YUV;
}

static hdmirx_color_metry hdmirxv2_packet_avi_get_color_metry_again(hi_drv_hdmirx_timing_info *timing_info)
{
    hdmirx_color_metry metry;

    if (timing_info->height < HDMIRX_HD_HEIGHT) {
        metry = HDMIRX_COLOR_METRY_ITU601; /* low definition */
    } else {
        metry = HDMIRX_COLOR_METRY_ITU709; /* high definition */
    }
    return metry;
}

static hi_void hdmirxv2_video_get_rgb_default_cs_info(hdmirx_video_ctx *video_ctx,
    hi_drv_hdmirx_timing_info *timing_info, hdmirx_color_metry color_metry)
{
    timing_info->color_space.color_space = HI_DRV_COLOR_CS_RGB;
    if ((timing_info->pc_mode) || (video_ctx->hdmirx_timing_info.video_idx == TIMING_1_640X480P)) {
        if (color_metry == HDMIRX_COLOR_METRY_ITU709) {
            timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
            timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
        } else if (color_metry == HDMIRX_COLOR_METRY_ITU601) {
            timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
            timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
        }
        timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
    } else {
        timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        if (color_metry == HDMIRX_COLOR_METRY_ITU709) {
            timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
            timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
        } else if (color_metry == HDMIRX_COLOR_METRY_ITU601) {
            timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
            timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
        } else {
            if (timing_info->height < HDMIRX_HD_HEIGHT) { // 720: check height value
                timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
                timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
            } else {
                timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
                timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
            }
        }
    }
}

hi_void hdmirxv2_video_get_rgb_cs_info(hi_drv_hdmirx_port port,
    hi_drv_hdmirx_timing_info *timing_info, hdmirx_rgb_range rgb_range)
{
    hdmirx_video_ctx *video_ctx = HI_NULL;
    hdmirx_color_metry color_metry;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    color_metry = hdmirxv2_packet_avi_get_color_metry(port);
    timing_info->color_space.color_space = HI_DRV_COLOR_CS_RGB;
    if (color_metry == HDMIRX_COLOR_METRY_BT2020_RGB) {
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT2020;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT;
        if (rgb_range == HDMIRX_RGB_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
    } else {
        if (color_metry == 0) { // c1 c0 are 0, judge by height according to sections 5.1
            color_metry = hdmirxv2_packet_avi_get_color_metry_again(timing_info);
        }
        if (rgb_range == HDMIRX_RGB_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
            if (color_metry == HDMIRX_COLOR_METRY_ITU709) {
                timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
                timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
            } else if (color_metry == HDMIRX_COLOR_METRY_ITU601) {
                timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
                timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
            }
        } else if (rgb_range == HDMIRX_RGB_LIMIT_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
            if (color_metry == HDMIRX_COLOR_METRY_ITU709) {
                timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
                timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
            } else if (color_metry == HDMIRX_COLOR_METRY_ITU601) {
                timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
                timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
            }
        } else {
             // default quantization
            hdmirxv2_video_get_rgb_default_cs_info(video_ctx, timing_info, color_metry);
        }
    }
}

hi_void hdmirxv2_video_get_yuv_cs_info_again(hi_drv_hdmirx_timing_info *timing_info, hdmirx_range yuv_range)
{
    timing_info->color_space.color_space = HI_DRV_COLOR_CS_YUV;
    if (timing_info->height < HDMIRX_HD_HEIGHT) { // 720: check height value
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT601_525;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
        if (yuv_range == HDMIRX_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
    } else {
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
        if (yuv_range == HDMIRX_FULL_RANGE) {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        } else {
            timing_info->color_space.quantify_range = HI_DRV_COLOR_LIMITED_RANGE;
        }
    }
}

hi_void hdmirxv2_video_get_rgb_range_info(hdmirx_video_ctx *video_ctx, hi_drv_hdmirx_timing_info *timing_info)
{
    switch (video_ctx->input_width) {
        case HDMIRX_INPUT_WIDTH_24:
            timing_info->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
            break;
        case HDMIRX_INPUT_WIDTH_30:
            timing_info->bit_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
            break;
        case HDMIRX_INPUT_WIDTH_36:
            timing_info->bit_width = HI_DRV_PIXEL_BITWIDTH_12BIT;
            break;
        default:
            timing_info->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
            break;
    }
}

hi_void hdmirxv2_video_get_pixel_fmt_info(hi_drv_hdmirx_timing_info *timing_info, hdmirx_color_space color_space)
{
    timing_info->pixel_fmt = HI_DRV_PIXEL_FMT_NV42;
    if (color_space == HDMIRX_COLOR_SPACE_YCBCR422) {
        timing_info->pixel_fmt = HI_DRV_PIXEL_FMT_NV61_2X1;
    } else if (color_space == HDMIRX_COLOR_SPACE_YCBCR444) {
        timing_info->pixel_fmt = HI_DRV_PIXEL_FMT_NV42;
    } else if (color_space == HDMIRX_COLOR_SPACE_RGB) {
        timing_info->pixel_fmt = HI_DRV_PIXEL_FMT_NV42_RGB;
    } else if (color_space == HDMIRX_COLOR_SPACE_YCBCR420) {
        timing_info->pixel_fmt = HI_DRV_PIXEL_FMT_NV21;
    }
}

hi_void hdmirxv2_video_get_over_sample_info(hi_drv_hdmirx_port port, hi_drv_hdmirx_timing_info *timing_info)
{
    hdmirx_oversample over_sample;
    over_sample = hdmirxv2_packet_avi_get_replication(port);
    switch (over_sample) {
        case HDMIRX_OVERSAMPLE_2X:
            timing_info->oversample = HI_DRV_OVERSAMPLE_2X;
            timing_info->width >>= 1;
            break;
        case HDMIRX_OVERSAMPLE_4X:
            timing_info->oversample = HI_DRV_OVERSAMPLE_4X;
            timing_info->width >>= 2; /* 2: 4x oversample, right mv 2bits means div 4 */
            break;
        default:
            timing_info->oversample = HI_DRV_OVERSAMPLE_1X;
            break;
    }
}

hi_void hdmirxv2_video_get_color_space_info(hi_drv_hdmirx_port port, hi_drv_hdmirx_timing_info *timing_info)
{
    hdmirx_rgb_range rgb_range;
    hdmirx_range yuv_range;
    hdmirx_color_space color_space;
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);

    color_space = hdmirxv2_packet_avi_get_color_space(port);
    rgb_range = hdmirxv2_packet_avi_get_rgb_range(port);
    yuv_range = hdmirxv2_packet_avi_get_yuv_range(port);
    if ((color_space == HDMIRX_COLOR_SPACE_YCBCR422) || (color_space == HDMIRX_COLOR_SPACE_YCBCR444) ||
        (color_space == HDMIRX_COLOR_SPACE_YCBCR420)) {
        hdmirxv2_video_get_yuv_cs_info(port, timing_info, yuv_range);
    } else if (color_space == HDMIRX_COLOR_SPACE_RGB) {
        hdmirxv2_video_get_rgb_cs_info(port, timing_info, rgb_range);
    } else if (video_ctx->hdmirx_timing_info.video_idx < TIMING_MAX) {
        hdmirxv2_video_get_yuv_cs_info_again(timing_info, yuv_range);
    } else if (video_ctx->hdmirx_timing_info.video_idx == TIMING_PC_MODE) {
        timing_info->color_space.color_space = HI_DRV_COLOR_CS_RGB;
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
        timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
    }
    hdmirxv2_video_get_rgb_range_info(video_ctx, timing_info);
    hdmirxv2_video_get_pixel_fmt_info(timing_info, color_space);
    hdmirxv2_video_get_over_sample_info(port, timing_info);
}

hi_void hdmirxv2_video_show_video_timing(hi_drv_hdmirx_port port)
{
    hi_drv_hdmirx_timing_info timing_info;

    hdmirxv2_video_get_timing_info(port, &timing_info);
    hi_dbg_hdmirx("show timing");
    hi_dbg_hdmirx_print_u32(timing_info.timing_idx);
    hi_dbg_hdmirx_print_u32(timing_info.width);
    hi_dbg_hdmirx_print_u32(timing_info.height);
    hi_dbg_hdmirx_print_u32(timing_info.frame_rate);
    hi_dbg_hdmirx_print_u32(timing_info.color_space);
    hi_dbg_hdmirx_print_u32(timing_info.pixel_fmt);
    hi_dbg_hdmirx_print_u32(timing_info.bit_width);
    hi_dbg_hdmirx_print_u32(timing_info.interlace);
    hi_dbg_hdmirx_print_u32(timing_info.oversample);
    hi_dbg_hdmirx_print_u32(timing_info.fmt);
    hi_dbg_hdmirx_print_u32(timing_info.hdmi_mode);
    hi_dbg_hdmirx_print_u32(timing_info.vblank);
    hi_dbg_hdmirx_print_u32(timing_info.pc_mode);
    hi_dbg_hdmirx_print_u32(timing_info.mhl);
    hi_dbg_hdmirx_print_u32(timing_info.color_space.color_primary);
    hi_dbg_hdmirx_print_u32(timing_info.color_space.color_space);
    hi_dbg_hdmirx_print_u32(timing_info.color_space.quantify_range);
    hi_dbg_hdmirx_print_u32(timing_info.color_space.transfer_type);
    hi_dbg_hdmirx_print_u32(timing_info.color_space.matrix_coef);
}

hi_void hdmirxv2_video_get_timing_info_by_mode(hi_drv_hdmirx_port port, hi_drv_hdmirx_timing_info *timing_info)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    if (video_ctx->hdmi_mode == HI_FALSE) {
        timing_info->color_space.color_space = HI_DRV_COLOR_CS_RGB;
        timing_info->color_space.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
        timing_info->color_space.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
        timing_info->color_space.quantify_range = HI_DRV_COLOR_FULL_RANGE;
        timing_info->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
        timing_info->oversample = HI_DRV_OVERSAMPLE_1X;
        timing_info->pixel_fmt = HI_DRV_PIXEL_FMT_NV42_RGB;
        timing_info->fmt = HI_DRV_FRAME_PACKING_TYPE_2D;
        timing_info->vblank = 0;
    } else {
        hdmirxv2_video_get_color_space_info(port, timing_info);
        timing_info->fmt = HI_DRV_FRAME_PACKING_TYPE_2D;
        timing_info->vblank = 0;
        if (hdmirxv2_packet_vsif_is_got3d(port) == HI_TRUE) {
            switch (video_ctx->hdmirx_timing_info.hdmi3d_structure) {
                case HDMIRX_3D_TYPE_FP:
                    timing_info->fmt = HI_DRV_FRAME_PACKING_TYPE_FRAME_PACKING;
                    break;
                case HDMIRX_3D_TYPE_SBS_FULL:
                case HDMIRX_3D_TYPE_SBS_HALF:
                    timing_info->fmt = HI_DRV_FRAME_PACKING_TYPE_SIDE_BY_SIDE;
                    break;
                case HDMIRX_3D_TYPE_TB:
                    timing_info->fmt = HI_DRV_FRAME_PACKING_TYPE_TOP_AND_BOTTOM;
                    break;
                case HDMIRX_3D_TYPE_FIELD_ALT:
                    timing_info->fmt = HI_DRV_FRAME_PACKING_TYPE_TIME_INTERLACED;
                    break;
                default:
                    timing_info->fmt = HI_DRV_FRAME_PACKING_TYPE_2D;
                    break;
            }

            if (timing_info->fmt == HI_DRV_FRAME_PACKING_TYPE_FRAME_PACKING &&
                ((video_ctx->hdmirx_timing_info.video_idx - HDMI_3D_RESOLUTION_MASK) \
                     < (TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES + 1)) &&
                    (((hi_u32)video_ctx->hdmirx_timing_info.video_idx) & HDMI_3D_RESOLUTION_MASK)) {
                    hi_u32 index = video_ctx->hdmirx_timing_info.video_idx - HDMI_3D_RESOLUTION_MASK;
                    timing_info->vblank = g_video_timing_table[index].blank.v;
            }
        }
    }
}

hi_void hdmirxv2_video_get_timing_info(hi_drv_hdmirx_port port, hi_drv_hdmirx_timing_info *timing_info)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    timing_info->color_space.transfer_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
    timing_info->mhl = HI_FALSE;
    timing_info->hdmi_mode = video_ctx->hdmi_mode;
    if (port < HI_DRV_HDMIRX_PORT_MAX) {
        if (video_ctx->special_case == hdmirx_special_case_vsync_invert) {
            timing_info->interlace = HI_FALSE;
        } else {
            timing_info->interlace = hal_video_get_interlance(port);
        }
    } else {
        timing_info->interlace = HI_FALSE;
    }
    /* 3D can not detect interlace from logic */
    if ((hi_u32)(video_ctx->hdmirx_timing_info.video_idx) & HDMI_3D_RESOLUTION_MASK) {
        if (((hi_u32)(video_ctx->hdmirx_timing_info.video_idx) & ~HDMI_3D_RESOLUTION_MASK) <
            (TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES + 1)) {
            hi_u32 index = (hi_u32)(video_ctx->hdmirx_timing_info.video_idx) & ~HDMI_3D_RESOLUTION_MASK;
            timing_info->interlace = g_video_timing_table[index].interlaced;
        }
    }

    if (timing_info->interlace == HI_TRUE) {
        timing_info->pc_mode = HI_FALSE;
    } else if (hdmirxv2_packet_avi_get_itc(port) == HI_TRUE) {
        timing_info->pc_mode = HI_TRUE;
    } else {
        if (video_ctx->hdmirx_timing_info.video_idx == TIMING_PC_MODE) {
            timing_info->pc_mode = HI_TRUE;
        } else if (video_ctx->hdmirx_timing_info.video_idx == TIMING_1_640X480P) {
            timing_info->pc_mode = HI_TRUE;
        } else {
            timing_info->pc_mode = HI_FALSE;
        }
    }
    timing_info->height = video_ctx->hdmirx_timing_info.vactive;
    timing_info->width = video_ctx->hdmirx_timing_info.hactive;
    timing_info->frame_rate = hdmirxv2_video_get_frame_rate(port);
    timing_info->timing_idx = video_ctx->hdmirx_timing_info.video_idx;
    hdmirxv2_video_get_timing_info_by_mode(port, timing_info);
}

hi_void hdmirxv2_video_print_hdr10_info(hi_void *s, hdmirx_video_ctx *video_ctx)
{
}

hi_void hdmirxv2_video_print_hdr10_plus_info(hi_void *s, hdmirx_video_ctx *video_ctx,
    hi_hdmirx_hdr10_plus_dynamic_metadata_info hdr10_plus_info)
{
    osal_proc_print(s, "Application_Version            :   %d\n", hdr10_plus_info.application_version);
    osal_proc_print(s, "Targeted_System_Display_Max_Lu :   %d\n",
                    hdr10_plus_info.targeted_system_display_maximum_luminance);
    osal_proc_print(s, "Average_Maxrgb                 :   %d\n", hdr10_plus_info.average_maxrgb);
    osal_proc_print(s, "Distribution_Values[0]         :   %d\n", hdr10_plus_info.distribution_values[0]);
    osal_proc_print(s, "Distribution_Values[1]         :   %d\n", hdr10_plus_info.distribution_values[1]);
    osal_proc_print(s, "Distribution_Values[2]         :   %d\n",
        hdr10_plus_info.distribution_values[2]); /* 2: 3st array data */
    osal_proc_print(s, "Distribution_Values[3]         :   %d\n",
        hdr10_plus_info.distribution_values[3]); /* 3: 4st array data */
    osal_proc_print(s, "Distribution_Values[4]         :   %d\n",
        hdr10_plus_info.distribution_values[4]); /* 4: 5st array data */
    osal_proc_print(s, "Distribution_Values[5]         :   %d\n",
        hdr10_plus_info.distribution_values[5]); /* 5: 6st array data */
    osal_proc_print(s, "Distribution_Values[6]         :   %d\n",
        hdr10_plus_info.distribution_values[6]); /* 6: 7st array data */
    osal_proc_print(s, "Distribution_Values[7]         :   %d\n",
        hdr10_plus_info.distribution_values[7]); /* 7: 8st array data */
    osal_proc_print(s, "Distribution_Values[8]         :   %d\n",
        hdr10_plus_info.distribution_values[8]); /* 8: 9st array data */

    osal_proc_print(s, "Num_Bezier_Curve_Anchors       :   %d\n", hdr10_plus_info.num_bezier_curve_anchors);
    osal_proc_print(s, "Knee_Point_X                   :   %d\n", hdr10_plus_info.knee_point_x);
    osal_proc_print(s, "Knee_Point_Y                   :   %d\n", hdr10_plus_info.knee_point_y);
    osal_proc_print(s, "Bezier_Curve_Anchors[0]        :   %d\n", hdr10_plus_info.bezier_curve_anchors[0]);
    osal_proc_print(s, "Bezier_Curve_Anchors[1]        :   %d\n", hdr10_plus_info.bezier_curve_anchors[1]);
    osal_proc_print(s, "Bezier_Curve_Anchors[2]        :   %d\n",
        hdr10_plus_info.bezier_curve_anchors[2]); /* 2: 3st array data */
    osal_proc_print(s, "Bezier_Curve_Anchors[3]        :   %d\n",
        hdr10_plus_info.bezier_curve_anchors[3]); /* 3: 4st array data */
    osal_proc_print(s, "Bezier_Curve_Anchors[4]        :   %d\n",
        hdr10_plus_info.bezier_curve_anchors[4]); /* 4: 5st array data */
    osal_proc_print(s, "Bezier_Curve_Anchors[5]        :   %d\n",
        hdr10_plus_info.bezier_curve_anchors[5]); /* 5: 6st array data */
    osal_proc_print(s, "Bezier_Curve_Anchors[6]        :   %d\n",
        hdr10_plus_info.bezier_curve_anchors[6]); /* 6: 7st array data */
    osal_proc_print(s, "Bezier_Curve_Anchors[7]        :   %d\n",
        hdr10_plus_info.bezier_curve_anchors[7]); /* 7: 8st array data */
    osal_proc_print(s, "Bezier_Curve_Anchors[8]        :   %d\n",
        hdr10_plus_info.bezier_curve_anchors[8]); /* 8: 9st array data */
    osal_proc_print(s, "Graphics_Overlayflag           :   %s\n",
        hdr10_plus_info.graphics_overlayflag ? "Yes" : "No");
    osal_proc_print(s, "No_Delay_Flag                  :   %s\n",
        hdr10_plus_info.no_delay_flag ? "Yes" : "No");
}

hi_void hdmirxv2_video_proc_read(hi_drv_hdmirx_port port, hi_void *s)
{
    hdmirx_video_ctx *video_ctx;

    video_ctx = HDMIRXV2_VIDEO_GET_CTX(port);
    osal_proc_print(s, "\n---------------HDR Attr---------------\n");
    if (video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN) {
        osal_proc_print(s, "Src Type             :   Unknown\n");
    } else if (video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_SDR) {
        osal_proc_print(s, "Src Type             :   SDR\n");
    } else if ((video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10) ||
        (video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_HLG)) {
        if (video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10) {
            osal_proc_print(s, "Src Type                   :   HDR10\n");
        } else {
            osal_proc_print(s, "Src Type                   :   HLG\n");
        }
        hdmirxv2_video_print_hdr10_info(s, video_ctx);
    } else if (video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_DOLBY) {
        hi_hdmirx_dolby_info stdolby_info;
        osal_proc_print(s, "Src Type            :   Dolby\n");
        hdmirxv2_packet_vsif_get_dolby_info(port, &stdolby_info);
        osal_proc_print(s, "LowLatency              :   %s\n", stdolby_info.low_latency ? "Yes" : "No");
        osal_proc_print(s, "BackltCtrl_Enable       :   %s\n", stdolby_info.backlt_ctrl_enable ? "Yes" : "No");
        osal_proc_print(s, "EffMaxLuminance         :   %d\n", stdolby_info.eff_max_luminance);
        osal_proc_print(s, "Auxiliary_Enable        :   %s\n", stdolby_info.auxiliary_enable ? "Yes" : "No");
        osal_proc_print(s, "AuxiliaryRunmode        :   %d\n", stdolby_info.auxiliary_runmode);
        osal_proc_print(s, "AuxiliaryRunversion     :   %d\n", stdolby_info.auxiliary_runversion);
        osal_proc_print(s, "AuxiliaryDebug          :   %d\n", stdolby_info.auxiliary_debug);
    } else if (video_ctx->hdmirx_hdr_data.src_type == HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10PLUS) {
        hi_hdmirx_hdr10_plus_dynamic_metadata_info hdr10_plus_info;
        osal_proc_print(s, "Src Type                         :   HDR10PLUS\n");
        hdmirxv2_packet_vsif_get_hdr10_plus_info(port, &hdr10_plus_info);
        hdmirxv2_video_print_hdr10_plus_info(s, video_ctx, hdr10_plus_info);
    }
}
