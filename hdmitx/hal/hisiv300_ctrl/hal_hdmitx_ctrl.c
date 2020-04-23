/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver hdmi controller aon reg header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>
#include "hi_osal.h"
#include "hal_hdmitx_io.h"
#include "drv_hdmitx_infoframe.h"
#include "drv_hdmitx_connector.h"
#include "drv_hdmitx_ddc.h"
#include "hal_hdmitx_ctrl.h"
#include "hal_hdmitx_frl.h"
#include "hal_hdmitx_dsc.h"
#include "hal_hdmitx_ctrl_reg.h"
#include "hal_hdmitx_reg.h"
#include "hal_hdmitx_audio_path_reg.h"
#include "hal_hdmitx_video_path_reg.h"
#include "hal_hdmitx_video_process_reg.h"

#define VIDEO_BLACK_DATA_YUV_CR (0x80 << 2)
#define VIDEO_BLACK_DATA_YUV_Y  (0x10 << 2)
#define VIDEO_BLACK_DATA_YUV_CB (0x80 << 2)

#define VIDEO_BLACK_DATA_RGB_R (0x10 << 2)
#define VIDEO_BLACK_DATA_RGB_G (0x10 << 2)
#define VIDEO_BLACK_DATA_RGB_B (0x10 << 2)

#define NCTS_TMDS_DEVIATION 20
#define NCTS_INVALID_DATA   0xffffffff

enum audio_clk_accuracy {
    AUDIO_CLK_ACCURACY_LEVEL2 = 0x0,
    AUDIO_CLK_ACCURACY_LEVEL1,
    AUDIO_CLK_ACCURACY_LEVEL3,
    AUDIO_CLK_ACCURACY_NOTMATCH
};

struct ctrl_i2s_status {
    /* cfg0 */
    enum audio_clk_accuracy clk_accuracy;
    hi_bool professional_app;
    hi_bool compress;
    hi_u32 sample_rate;
    /* cfg1 */
    hi_u32 org_rate;
    hi_u32 out_bit_length;
};

struct ctrl_i2s_ctrl {
    hi_u32 channel_lr_swap;
    hi_u32 in_bit_length;
    hi_bool vbit_compress;
    hi_bool data_dir_msb_first;
    hi_bool justify_ws_right;
    hi_bool polarity_ws_msb_left;
    hi_bool sck_shift_one;
};

struct hdmi_audio_samp_bit {
    hi_u32 bit_value;
    hi_u32 bit_reg;
};

struct hdmi_i2s_samp_rate {
    hi_u32 rate_value;
    hi_u32 rate_reg;
    hi_u32 rate_org_reg;
};

static struct hdmi_audio_samp_bit g_au_samp_bit[] = {
    /* bits, reg */
    { 16, 0x2 },
    { 17, 0xc },
    { 18, 0x4 },
    { 19, 0x8 },
    { 20, 0xa },
    { 21, 0xd },
    { 22, 0x5 },
    { 23, 0x9 },
    { 24, 0xb },
};

static struct hdmi_i2s_samp_rate g_i2s_samp_rate[] = {
    /* sample rate value, Reg      orgReg, */
    { HDMI_AUDIO_SAMPLE_RATE_8K,   0x01, 0x6 },
    { HDMI_AUDIO_SAMPLE_RATE_11K,  0x01, 0xa },
    { HDMI_AUDIO_SAMPLE_RATE_12K,  0x01, 0x2 },
    { HDMI_AUDIO_SAMPLE_RATE_16K,  0x01, 0x8 },
    { HDMI_AUDIO_SAMPLE_RATE_22K,  0x04, 0xb },
    { HDMI_AUDIO_SAMPLE_RATE_24K,  0x06, 0x9 },
    { HDMI_AUDIO_SAMPLE_RATE_32K,  0x03, 0xc },
    { HDMI_AUDIO_SAMPLE_RATE_44K,  0x00, 0xf },
    { HDMI_AUDIO_SAMPLE_RATE_48K,  0x02, 0xd },
    { HDMI_AUDIO_SAMPLE_RATE_88K,  0x08, 0x7 },
    { HDMI_AUDIO_SAMPLE_RATE_96K,  0x0a, 0x5 },
    { HDMI_AUDIO_SAMPLE_RATE_176K, 0x0c, 0x3 },
    { HDMI_AUDIO_SAMPLE_RATE_192K, 0x0e, 0x1 },
    { HDMI_AUDIO_SAMPLE_RATE_768K, 0x09, 0x1 },
};

struct hdmi_audio_n_cts {
    hi_u32 sample_rate;
    hi_u32 clock;
    hi_u32 n_value;
    hi_u32 cts_value;
};

static struct hdmi_audio_n_cts g_audio_n_cts_tmds[] = {
    /* Rate, TMDS,   N,     CTS */
    { 48000, 25174,  6864,  28125 },
    { 48000, 25200,  6144,  25200 },
    { 48000, 27000,  6144,  27000 },
    { 48000, 27027,  6144,  27027 },
    { 48000, 54000,  6144,  54000 },
    { 48000, 54054,  6144,  54054 },
    { 48000, 74175,  11648, 140625 },
    { 48000, 74250,  6144,  74250 },
    { 48000, 148351, 5824,  140625 },
    { 48000, 148500, 6144,  148500 },
    { 48000, 296703, 5824,  281250 },
    { 48000, 297000, 6144,  297000 },
    { 48000, 593406, 5824,  562500 },
    { 48000, 594000, 6144,  594000 },
    { 48000, 0,      6144,  0     },

    { 96000, 25174,  13728, 28125 },
    { 96000, 25200,  12288, 25200 },
    { 96000, 27000,  12288, 27000 },
    { 96000, 27027,  12288, 27027 },
    { 96000, 54000,  12288, 54000 },
    { 96000, 54054,  12288, 54054 },
    { 96000, 74175,  23296, 140625 },
    { 96000, 74250,  12288, 74250 },
    { 96000, 148351, 11648, 140625 },
    { 96000, 148500, 12288, 148500 },
    { 96000, 296703, 11648, 281250 },
    { 96000, 297000, 10240, 247500 },
    { 96000, 593406, 11648, 562500 },
    { 96000, 594000, 12288, 594000 },
    { 96000, 0,      12288, 0      },

    { 192000, 25174,  27456, 28125 },
    { 192000, 25200,  24576, 25200 },
    { 192000, 27000,  24576, 27000 },
    { 192000, 27027,  24576, 27027 },
    { 192000, 54000,  24576, 54000 },
    { 192000, 54054,  24576, 54054 },
    { 192000, 74175,  46592, 140625 },
    { 192000, 74250,  24576, 74250 },
    { 192000, 148351, 23296, 140625 },
    { 192000, 148500, 24576, 148500 },
    { 192000, 296703, 23296, 281250 },
    { 192000, 297000, 20480, 247500 },
    { 192000, 593406, 23296, 562500 },
    { 192000, 594000, 24576, 594000 },
    { 192000, 0,      24576, 0      },

    { 384000, 25174,  54912, 28125 },
    { 384000, 25200,  49152, 25200 },
    { 384000, 27000,  49152, 27000 },
    { 384000, 27027,  49152, 27027 },
    { 384000, 54000,  49152, 54000 },
    { 384000, 54054,  49152, 54054 },
    { 384000, 74175,  93184, 140625 },
    { 384000, 74250,  49152, 74250 },
    { 384000, 148351, 46592, 140625 },
    { 384000, 148500, 49152, 148500 },
    { 384000, 296703, 46592, 281250 },
    { 384000, 297000, 40960, 247500 },
    { 384000, 593406, 46592, 562500 },
    { 384000, 594000, 49152, 594000 },
    { 384000, 0,      49152, 0      },

    { 32000, 25174,  4576,  28125 },
    { 32000, 25200,  4096,  25200 },
    { 32000, 27000,  4096,  27000 },
    { 32000, 27027,  4096,  27027 },
    { 32000, 54000,  4096,  54000 },
    { 32000, 54054,  4096,  54054 },
    { 32000, 74175,  11648, 210937 },
    { 32000, 74250,  4096,  74250 },
    { 32000, 148351, 11648, 421875 },
    { 32000, 148500, 4096,  148500 },
    { 32000, 296703, 5824,  421875 },
    { 32000, 297000, 3072,  222750 },
    { 32000, 593406, 5824,  843750 },
    { 32000, 594000, 3072,  445500 },
    { 32000, 0,      4096,  0      },

    { 64000, 25174,  9152,  28125 },
    { 64000, 25200,  8192,  25200 },
    { 64000, 27000,  8192,  27000 },
    { 64000, 27027,  8192,  27027 },
    { 64000, 54000,  8192,  54000 },
    { 64000, 54054,  8192,  54054 },
    { 64000, 74175,  23296, 210937 },
    { 64000, 74250,  8192,  74250 },
    { 64000, 148351, 23296, 421875 },
    { 64000, 148500, 8192,  148500 },
    { 64000, 296703, 11648, 421875 },
    { 64000, 297000, 8192,  297000 },
    { 64000, 593406, 11648, 843750 },
    { 64000, 594000, 8192,  594000 },
    { 64000, 0,      8192,  0      },

    { 128000, 25174,  18304, 28125 },
    { 128000, 25200,  16384, 25200 },
    { 128000, 27000,  16384, 27000 },
    { 128000, 27027,  16384, 27027 },
    { 128000, 54000,  16384, 54000 },
    { 128000, 54054,  16384, 54054 },
    { 128000, 74175,  46592, 210937 },
    { 128000, 74250,  16384, 74250 },
    { 128000, 148351, 46592, 421875 },
    { 128000, 148500, 16384, 148500 },
    { 128000, 296703, 23296, 421875 },
    { 128000, 297000, 16384, 297000 },
    { 128000, 593406, 23296, 843750 },
    { 128000, 594000, 16384, 594000 },
    { 128000, 0,      16384, 0      },

    { 256000, 25174,  36608, 28125 },
    { 256000, 25200,  32768, 25200 },
    { 256000, 27000,  32768, 27000 },
    { 256000, 27027,  32768, 27027 },
    { 256000, 54000,  32768, 54000 },
    { 256000, 54054,  32768, 54054 },
    { 256000, 74175,  46592, 210937 },
    { 256000, 74250,  32768, 74250 },
    { 256000, 148351, 46592, 421875 },
    { 256000, 148500, 32768, 148500 },
    { 256000, 296703, 23296, 421875 },
    { 256000, 297000, 32768, 297000 },
    { 256000, 593406, 23296, 843750 },
    { 256000, 594000, 32768, 594000 },
    { 256000, 0,      32768, 0      },

    { 44100, 25174,  7007,  31250 },
    { 44100, 25200,  6272,  28000 },
    { 44100, 27000,  6272,  30000 },
    { 44100, 27027,  6272,  30030 },
    { 44100, 54000,  6272,  60000 },
    { 44100, 54054,  6272,  60060 },
    { 44100, 74175,  17836, 234375 },
    { 44100, 74250,  6272,  82500 },
    { 44100, 148351, 8918,  234375 },
    { 44100, 148500, 6272,  165000 },
    { 44100, 296703, 4459,  234375 },
    { 44100, 297000, 4704,  247500 },
    { 44100, 593406, 8918,  937500 },
    { 44100, 594000, 9408,  990000 },
    { 44100, 0,      6272,  0      },

    { 88200, 25174,  14014, 31250 },
    { 88200, 25200,  12544, 28000 },
    { 88200, 27000,  12544, 30000 },
    { 88200, 27027,  12544, 30030 },
    { 88200, 54000,  12544, 60000 },
    { 88200, 54054,  12544, 60060 },
    { 88200, 74175,  35672, 234375 },
    { 88200, 74250,  12544, 82500 },
    { 88200, 148351, 17836, 234375 },
    { 88200, 148500, 12544, 165000 },
    { 88200, 296703, 8918,  234375 },
    { 88200, 297000, 9408,  247500 },
    { 88200, 593406, 17836, 937500 },
    { 88200, 594000, 18816, 990000 },
    { 88200, 0,      12544, 0      },

    { 176400, 25174,  28028, 31250 },
    { 176400, 25200,  25088, 28000 },
    { 176400, 27000,  25088, 27000 },
    { 176400, 27027,  25088, 30000 },
    { 176400, 54000,  25088, 30030 },
    { 176400, 54054,  25088, 60000 },
    { 176400, 74175,  71344, 60060 },
    { 176400, 74250,  25088, 334375 },
    { 176400, 148351, 35672, 82500 },
    { 176400, 148500, 25088, 165000 },
    { 176400, 296703, 17836, 234375 },
    { 176400, 297000, 18816, 247500 },
    { 176400, 593406, 35672, 937500 },
    { 176400, 594000, 27632, 990000 },
    { 176400, 0,      25088, 0      },

    { 352800, 25174,  56056,  31250 },
    { 352800, 25200,  50176,  28000 },
    { 352800, 27000,  50176,  27000 },
    { 352800, 27027,  50176,  30000 },
    { 352800, 54000,  50176,  30030 },
    { 352800, 54054,  50176,  60000 },
    { 352800, 74175,  142688, 60060 },
    { 352800, 74250,  50176,  334375 },
    { 352800, 148351, 71344,  82500 },
    { 352800, 148500, 50176,  165000 },
    { 352800, 296703, 35672,  234375 },
    { 352800, 297000, 37632,  247500 },
    { 352800, 593406, 71344,  937500 },
    { 352800, 594000, 75264,  990000 },
    { 352800, 0,      50176,  0      },

};

static struct hdmi_audio_n_cts g_audio_n_cts_frl[] = {

    /* Rate, rate, N,  CTS */
    { 32000, 3,  4224, 171875 },
    { 32000, 6,  4032, 328125 },
    { 32000, 8,  4032, 437500 },
    { 32000, 10, 3456, 468750 },
    { 32000, 12, 3072, 500000 },

    { 44100, 3,  5292, 156250 },
    { 44100, 6,  5292, 312500 },
    { 44100, 8,  3969, 312500 },
    { 44100, 10, 3969, 390625 },
    { 44100, 12, 3969, 468750 },

    { 48000, 3,  5760, 156250 },
    { 48000, 6,  6048, 328125 },
    { 48000, 8,  6048, 437500 },
    { 48000, 10, 5184, 468750 },
    { 48000, 12, 4752, 515625 },

    { 64000, 3,  8448, 171875 },
    { 64000, 6,  8064, 328125 },
    { 64000, 8,  8064, 437500 },
    { 64000, 10, 6912, 468750 },
    { 64000, 12, 6144, 500000 },

    { 96000, 3,  11520, 156250 },
    { 96000, 6,  12096, 328125 },
    { 96000, 8,  12096, 437500 },
    { 96000, 10, 10368, 468750 },
    { 96000, 12, 9504,  515625 },

    { 128000, 3,  16896, 171875 },
    { 128000, 6,  16128, 328125 },
    { 128000, 8,  16128, 437500 },
    { 128000, 10, 13824, 468750 },
    { 128000, 12, 12288, 500000 },

    { 176400, 3,  21168, 156250 },
    { 176400, 6,  21168, 312500 },
    { 176400, 8,  15876, 312500 },
    { 176400, 10, 15876, 390625 },
    { 176400, 12, 15876, 468750 },

    { 192000, 3,  23040, 156250 },
    { 192000, 6,  24192, 328125 },
    { 192000, 8,  24192, 437500 },
    { 192000, 10, 20736, 468750 },
    { 192000, 12, 19008, 515625 },

    { 256000, 3,  33792, 171875 },
    { 256000, 6,  32256, 328125 },
    { 256000, 8,  32256, 437500 },
    { 256000, 10, 27648, 468750 },
    { 256000, 12, 24576, 500000 },

    { 352800, 3,  42336, 156250 },
    { 352800, 6,  42336, 312500 },
    { 352800, 8,  31752, 312500 },
    { 352800, 10, 31752, 390625 },
    { 352800, 12, 31752, 468750 },

    { 384000, 3,  46080, 156250 },
    { 384000, 6,  48384, 328125 },
    { 384000, 8,  48384, 437500 },
    { 384000, 10, 41472, 468750 },
    { 384000, 12, 38016, 515625 },
};

static hi_u32 ctrl_tmds_recommended_n(hi_u32 freq, hi_u32 clock)
{
    hi_u32 i;
    struct hdmi_audio_n_cts *audio_n_cts = &g_audio_n_cts_tmds[0];
    hi_u32 n_value = 6144;

    for (i = 0; i < ARRAY_SIZE(g_audio_n_cts_tmds); i++, audio_n_cts++) {
        if ((audio_n_cts->sample_rate == freq) &&
            (audio_n_cts->clock == clock)) {
            n_value = audio_n_cts->n_value;
            break;
        } else if ((audio_n_cts->sample_rate == freq) &&
                   (audio_n_cts->clock >= (clock - NCTS_TMDS_DEVIATION)) &&
                   (audio_n_cts->clock <= (clock + NCTS_TMDS_DEVIATION))) {
            n_value = audio_n_cts->n_value;
            break;
        } else if ((audio_n_cts->sample_rate == freq) &&
                   (audio_n_cts->clock == 0)) {
            n_value = audio_n_cts->n_value;
            break;
        }
    }

    return n_value;
}

static hi_u32 ctrl_tmds_recommended_cts(hi_u32 freq, hi_u32 clock)
{
    hi_u32 i;
    struct hdmi_audio_n_cts *audio_n_cts = &g_audio_n_cts_tmds[0];
    hi_u32 tmp_cts = NCTS_INVALID_DATA;
    hi_u32 tmp_n = NCTS_INVALID_DATA;

    for (i = 0; i < ARRAY_SIZE(g_audio_n_cts_tmds); i++, audio_n_cts++) {
        if ((audio_n_cts->sample_rate == freq) &&
            (audio_n_cts->clock == clock)) {
            tmp_cts = audio_n_cts->cts_value;
            tmp_n = audio_n_cts->n_value;
            break;
        } else if ((audio_n_cts->sample_rate == freq) &&
                   (audio_n_cts->clock >= (clock - NCTS_TMDS_DEVIATION)) &&
                   (audio_n_cts->clock <= (clock + NCTS_TMDS_DEVIATION))) {
            tmp_cts = audio_n_cts->cts_value;
            tmp_n = audio_n_cts->n_value;
            break;
        } else if ((audio_n_cts->sample_rate == freq) &&
                   (audio_n_cts->clock == 0)) {
            tmp_cts = audio_n_cts->cts_value;
            tmp_n = audio_n_cts->n_value;
            break;
        }
    }
    /* Measured */
    if (tmp_cts == 0 && freq) {
        /* 128*freq = TMDS_CLK*N/CTS; the unit of clock and freq is 1khz = 1000Hz */
        tmp_cts = (tmp_n / 128) * clock * 1000 / freq;
    } else if (tmp_cts == NCTS_INVALID_DATA && freq) { /* can't find , N default 48k samprate */
        HDMI_ERR("can't find cts! u32TmpCts=%d,u32SampleRate=%d,u32TmdsClk=%d\n",
                 tmp_cts, freq, clock);
        /* 128*freq = TMDS_CLK*N/CTS; default n is 6144, the unit of clock and freq is 1khz = 1000Hz */
        tmp_cts = (6144 / 128) * clock * 1000 / freq;
    }

    return tmp_cts;
}

static hi_u32 ctrl_frl_recommended_n(hi_u32 freq, hi_u32 frl_rate)
{
    hi_u32 i;
    hi_u32 clock = FRL_CLOCK_3G;
    struct hdmi_audio_n_cts *audio_n_cts = &g_audio_n_cts_frl[0];
    hi_u32 n_value = 5760;

    switch (frl_rate) {
        case FRL_RATE_3G3L:
            clock = FRL_CLOCK_3G;
            break;
        case FRL_RATE_6G3L:
        case FRL_RATE_6G4L:
            clock = FRL_CLOCK_6G;
            break;
        case FRL_RATE_8G4L:
            clock = FRL_CLOCK_8G;
            break;
        case FRL_RATE_10G4L:
            clock = FRL_CLOCK_10G;
            break;
        case FRL_RATE_12G4L:
            clock = FRL_CLOCK_12G;
            break;
        default:
            break;
    }

    for (i = 0; i < ARRAY_SIZE(g_audio_n_cts_frl); i++, audio_n_cts++) {
        if ((audio_n_cts->sample_rate == freq) &&
            (audio_n_cts->clock == clock)) {
            n_value = audio_n_cts->n_value;
            break;
        }
    }

    return n_value;
}

static hi_u32 ctrl_frl_recommended_cts(hi_u32 freq, hi_u32 frl_rate)
{
    hi_u32 i;
    hi_u32 clock = FRL_CLOCK_3G;
    struct hdmi_audio_n_cts *audio_n_cts = &g_audio_n_cts_frl[0];
    hi_u32 cts_value = 5760;

    switch (frl_rate) {
        case FRL_RATE_3G3L:
            clock = FRL_CLOCK_3G;
            break;
        case FRL_RATE_6G3L:
        case FRL_RATE_6G4L:
            clock = FRL_CLOCK_6G;
            break;
        case FRL_RATE_8G4L:
            clock = FRL_CLOCK_8G;
            break;
        case FRL_RATE_10G4L:
            clock = FRL_CLOCK_10G;
            break;
        case FRL_RATE_12G4L:
            clock = FRL_CLOCK_12G;
            break;
        default:
            break;
    }

    for (i = 0; i < ARRAY_SIZE(g_audio_n_cts_frl); i++, audio_n_cts++) {
        if ((audio_n_cts->sample_rate == freq) &&
            (audio_n_cts->clock == clock)) {
            cts_value = audio_n_cts->cts_value;
            break;
        }
    }

    return cts_value;
}

static hi_u32 ctrl_get_audio_bit_reg(hi_u32 bit_value)
{
    hi_u32 i = 0;
    struct hdmi_audio_samp_bit *samp_bit;

    for (samp_bit = &g_au_samp_bit[0];
         samp_bit && (i < ARRAY_SIZE(g_au_samp_bit));
         samp_bit++, i++) {
        if (samp_bit->bit_value == bit_value) {
            return samp_bit->bit_reg;
        }
    }

    HDMI_ERR("can't find audio bit= %u,force 16\n", bit_value);
    return 0x02; /* 16bit */
}

static hi_u32 ctrl_get_i2s_rate_reg(hi_u32 rate_value)
{
    hi_u32 i = 0;
    struct hdmi_i2s_samp_rate *i2s_rate;

    for (i2s_rate = &g_i2s_samp_rate[0];
         i2s_rate && (i < ARRAY_SIZE(g_i2s_samp_rate));
         i2s_rate++, i++)
        if (i2s_rate->rate_value == rate_value) {
            return i2s_rate->rate_reg;
        }

    HDMI_ERR("can't find aduio rate= %u,force 48k\n", rate_value);

    return 0x02; /* 48KHZ */
}

static hi_u32 ctrl_get_i2s_rate_org_reg(hi_u32 rate_value)
{
    hi_u32 i = 0;
    struct hdmi_i2s_samp_rate *i2s_rate;

    for (i2s_rate = &g_i2s_samp_rate[0];
         i2s_rate && (i < ARRAY_SIZE(g_i2s_samp_rate));
         i2s_rate++, i++)
        if (i2s_rate->rate_value == rate_value) {
            return i2s_rate->rate_org_reg;
        }

    HDMI_ERR("can't find aduio rate= %u,force 48k\n", rate_value);

    return 0x0d; /* 48KHZ */
}

static hi_void ctrl_avi_infoframe_config(struct hdmi_controller *ctrl,
    struct hdmi_avi_infoframe *frame)
{
    struct hdmi_in_data *in = NULL;
    struct hdmi_out_data *out = NULL;
    hi_u32 colorimetry;
    hi_u32 extended_colorimetry;

    if (ctrl == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    in = &ctrl->mode.timing_data.in;
    out = &ctrl->mode.timing_data.out;
    colorimetry = in->color.colorimetry & 0xf;
    extended_colorimetry = (in->color.colorimetry >> 4) & 0xf; /* The upper 4 bits is extended colorimetry. */

    frame->pixel_repeat = in->pixel_repeat;
    frame->video_code = (in->vic > 255) ? 0 : (in->vic & 0xff); /* vic > 255 is not CEA timing. */

    /*
     * The video code must be 0 in the avi infoframe, when the timing
     * is HDMI_3840X2160P24_16_9(93), HDMI_3840X2160P25_16_9(94),
     * HDMI_3840X2160P30_16_9(95), HDMI_4096X2160P24_256_135(98) at 2D mode.
     */
    if ((frame->video_code == VIC_3840X2160P24_16_9 ||
        frame->video_code == VIC_3840X2160P25_16_9 ||
        frame->video_code == VIC_3840X2160P30_16_9 ||
        frame->video_code == VIC_4096X2160P24_256_135) &&
        in->mode_3d == HDMI_3D_NONE) {
        frame->video_code = 0;
    }

    frame->picture_aspect = in->picture_aspect_ratio;
    frame->content_type = in->it_content_type;
    frame->itc = in->it_content_valid;
    frame->active_aspect = in->active_aspect_ratio;
    frame->scan_mode = in->scan_info;
    frame->colorspace = out->color.color_format;
    frame->colorimetry = colorimetry;
    frame->extended_colorimetry = extended_colorimetry;
    frame->quantization_range = in->color.rgb_quantization;
    frame->ycc_quantization_range = in->color.ycc_quantization;
    frame->top_bar = in->top_bar;
    frame->bottom_bar = in->bottom_bar;
    frame->left_bar = in->left_bar;
    frame->right_bar = in->right_bar;
    frame->nups = in->bar_present;

    /* Y2 = 1 or vic >= 128, version shall use 3 */
    if (frame->video_code > VIC_5120X2160P100_64_27) {
        frame->version = 3; /* Avi infoframe version need be 3, when vic > 127. */
    } else if (frame->colorimetry == HDMI_COLORIMETRY_EXTENDED &&
        frame->extended_colorimetry == HDMI_EXTENDED_COLORIMETRY_RESERVED) {
        frame->version = 4; /* Avi infoframe version need be 4, when extended colorimetry is reserved. */
    }

    return;
}

hi_void hal_ctrl_hw_send_info_frame(struct hisilicon_hdmi *hdmi, hi_u8 *buffer,
    hi_u8 len)
{
    hi_u8 *ptr = buffer;
    hi_u32 i;
    hi_u32 data;
    hi_u32 head_offset;
    hi_u32 pkt_l_offset;
    hi_u32 pkt_h_offset;
    hi_u32 cfg_offset;

    switch (*ptr) {
        case HDMI_INFOFRAME_TYPE_AVI:
            head_offset = REG_AVI_PKT_HEADER;
            pkt_l_offset = REG_AVI_SUB_PKT0_L;
            pkt_h_offset = REG_AVI_SUB_PKT0_H;
            cfg_offset = REG_CEA_AVI_CFG;
            break;

        case HDMI_INFOFRAME_TYPE_AUDIO:
            head_offset = REG_AIF_PKT_HEADER;
            pkt_l_offset = REG_AIF_SUB_PKT0_L;
            pkt_h_offset = REG_AIF_SUB_PKT0_H;
            cfg_offset = REG_CEA_AUD_CFG;
            break;

        case HDMI_INFOFRAME_TYPE_VENDOR:
            head_offset = REG_VSIF_PKT_HEADER;
            pkt_l_offset = REG_VSIF_SUB_PKT0_L;
            pkt_h_offset = REG_VSIF_SUB_PKT0_H;
            cfg_offset = REG_CEA_VSIF_CFG;
            break;

        default:
            /* HDMI_ERR("unsupported infoframe type: %#x\n", *ptr); */
            return;
    }

    /* Write PKT header */
    data = reg_sub_pkt_hb0(*ptr++);
    data |= reg_sub_pkt_hb1(*ptr++);
    data |= reg_sub_pkt_hb2(*ptr++);
    hdmi_writel(hdmi->hdmi_regs, head_offset, data);

    /* Write PKT body */
    for (i = 0; i < MAX_SUB_PKT_NUM; i++) {
        data = reg_sub_pktx_pb0(*ptr++);
        data |= reg_sub_pktx_pb1(*ptr++);
        data |= reg_sub_pktx_pb2(*ptr++);
        data |= reg_sub_pktx_pb3(*ptr++);
        /* the address offset of two subpackages is 8bytes */
        hdmi_writel(hdmi->hdmi_regs, pkt_l_offset + i * 8, data);
        data = reg_sub_pktx_pb4(*ptr++);
        data |= reg_sub_pktx_pb5(*ptr++);
        data |= reg_sub_pktx_pb6(*ptr++);
        /* the address offset of two subpackages is 8bytes */
        hdmi_writel(hdmi->hdmi_regs, pkt_h_offset + i * 8, data);
    }

    /* Enable PKT send */
    hdmi_clrset(hdmi->hdmi_regs, cfg_offset,
                REG_EN_M | REG_RPT_EN_M, reg_en(1) | reg_rpt_en(1));
}

static hi_void ctrl_hw_read_dsc_emp(struct hisilicon_hdmi *hdmi, hi_u8 *buffer, hi_u8 len)
{
    hi_u32 i = 0;
    hi_u32 n, j;
    hi_u32 data;

    if ((hdmi == NULL) || (buffer == NULL)) {
        HDMI_ERR("ptr null err.\n");
        return;
    }

    /* head pkt, i is buffer offset */
    for (n = 0; n < MAX_DSC_EMP_NUM; n++) {
        data = hdmi_readl(hdmi->hdmi_regs, reg_dsc_em_pkt_header(n));
        buffer[i++] = (data >> 0) & 0xff; /* shift 0 bit, 0xff is byte mask */
        buffer[i++] = (data >> 8) & 0xff; /* shift 8 bit, 0xff is byte mask */
        buffer[i++] = (data >> 16) & 0xff; /* shift 16 bit, 0xff is byte mask */

        /* sub pkt */
        for (j = 0; j < MAX_SUB_PKT_NUM; j++) { /* [0,8] is sub_pkt number */
            data = hdmi_readl(hdmi->hdmi_regs, reg_dsc_em_packet_sub0_l(n) + i * 8); /* 8: offset */
            buffer[i++] = (data >> 0) & 0xff; /* shift 0 bit, 0xff is byte mask */
            buffer[i++] = (data >> 8) & 0xff; /* shift 8 bit, 0xff is byte mask */
            buffer[i++] = (data >> 16) & 0xff; /* shift 16 bit, 0xff is byte mask */
            buffer[i++] = (data >> 24) & 0xff; /* shift 24 bit, 0xff is byte mask */
            data = hdmi_readl(hdmi->hdmi_regs, reg_dsc_em_packet_sub0_h(n) + i * 8); /* 8: offset */
            buffer[i++] = (data >> 0) & 0xff; /* shift 0 bit, 0xff is byte mask */
            buffer[i++] = (data >> 8) & 0xff; /* shift 8 bit, 0xff is byte mask */
            buffer[i++] = (data >> 16) & 0xff; /* shift 16 bit, 0xff is byte mask */
        }
    }
}

static hi_void ctrl_hw_send_dsc_emp(struct hisilicon_hdmi *hdmi, hi_u8 *buffer,
                                    hi_u8 len, hi_bool enable)
{
    hi_u8 *ptr = buffer;
    hi_u32 i;
    hi_u32 n;
    hi_u32 data;
    hi_u32 head_offset;
    hi_u32 pkt_l_offset;
    hi_u32 pkt_h_offset;

    for (n = 0; n < MAX_DSC_EMP_NUM; n++) {
        head_offset = reg_dsc_em_pkt_header(n);
        pkt_l_offset = reg_dsc_em_packet_sub0_l(n);
        pkt_h_offset = reg_dsc_em_packet_sub0_h(n);

        /* Write PKT header */
        data = reg_sub_pkt_hb0(*ptr++);
        data |= reg_sub_pkt_hb1(*ptr++);
        data |= reg_sub_pkt_hb2(*ptr++);
        hdmi_writel(hdmi->hdmi_regs, head_offset, data);

        /* Write PKT body */
        for (i = 0; i < MAX_SUB_PKT_NUM; i++) {
            data = reg_sub_pktx_pb0(*ptr++);
            data |= reg_sub_pktx_pb1(*ptr++);
            data |= reg_sub_pktx_pb2(*ptr++);
            data |= reg_sub_pktx_pb3(*ptr++);
            /* the address offset of two subpackages is 8bytes */
            hdmi_writel(hdmi->hdmi_regs, pkt_l_offset + i * 8, data);
            data = reg_sub_pktx_pb4(*ptr++);
            data |= reg_sub_pktx_pb5(*ptr++);
            data |= reg_sub_pktx_pb6(*ptr++);
            /* the address offset of two subpackages is 8bytes */
            hdmi_writel(hdmi->hdmi_regs, pkt_h_offset + i * 8, data);
        }
    }

    if (enable) {
        /* Enable PKT send */
        hdmi_clrset(hdmi->hdmi_regs, REG_CEA_DSC_CONTROL,
                    REG_EN_M | REG_RPT_EN_M, reg_en(1) | reg_rpt_en(1));
    } else {
        /* Enable PKT send */
        hdmi_clrset(hdmi->hdmi_regs, REG_CEA_DSC_CONTROL,
                    REG_EN_M | REG_RPT_EN_M, reg_en(1) | reg_rpt_en(1));
        osal_msleep(100); /* need sleep 100ms */
        /* Disable PKT send */
        hdmi_clrset(hdmi->hdmi_regs, REG_CEA_DSC_CONTROL,
                    REG_EN_M | REG_RPT_EN_M, reg_en(0) | reg_rpt_en(0));
    }
}

static hi_void ctrl_hw_set_vdp_packet(struct hisilicon_hdmi *hdmi)
{
    /* Reset vdp packet path */
    hdmi_clr(hdmi->hdmi_regs, REG_TX_METADATA_CTRL_ARST_REQ,
             REG_TX_METADATA_ARST_REQ_M);

    /* enable vdp packet path */
    hdmi_clrset(hdmi->hdmi_regs, REG_TX_METADATA_CTRL,
                REG_TXMETA_VDP_PATH_EN_M | REG_TXMETA_VDP_SMD_EN_M | REG_TXMETA_VDP_AVI_EN_M,
                reg_txmeta_vdp_path_en(1) | reg_txmeta_vdp_smd_en(1) | reg_txmeta_vdp_avi_en(1));
}

static hi_void ctrl_hw_set_hdmi_mode(struct hisilicon_hdmi *hdmi, hi_bool hdmi_mode)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AVMIXER_CONFIG,
                REG_HDMI_MODE_M, reg_hdmi_mode(hdmi_mode));
}

static hi_void ctrl_hw_send_av_mute(struct hisilicon_hdmi *hdmi)
{
    /* set av mute */
    hdmi_clrset(hdmi->hdmi_regs, REG_CP_PKT_AVMUTE,
                REG_CP_CLR_AVMUTE_M | REG_CP_SET_AVMUTE_M,
                reg_cp_clr_avmute(0) | reg_cp_set_avmute(1));

    /* Enable PKT send */
    hdmi_clrset(hdmi->hdmi_regs, REG_CEA_CP_CFG,
                REG_EN_M | REG_RPT_EN_M,
                reg_en(1) | reg_rpt_en(1));
}

static hi_void ctrl_hw_send_av_unmute(struct hisilicon_hdmi *hdmi)
{
    /* set av unmute */
    hdmi_clrset(hdmi->hdmi_regs, REG_CP_PKT_AVMUTE,
                REG_CP_CLR_AVMUTE_M | REG_CP_SET_AVMUTE_M,
                reg_cp_clr_avmute(1) | reg_cp_set_avmute(0));

    /* Enable PKT send */
    hdmi_clrset(hdmi->hdmi_regs, REG_CEA_CP_CFG,
                REG_EN_M | REG_RPT_EN_M,
                reg_en(1) | reg_rpt_en(1));
}

static hi_void ctrl_hw_set_deepcolor(struct hisilicon_hdmi *hdmi, hi_u8 mode,
                                     hi_bool enable)
{
    /* set tmds pack mode */
    hdmi_clrset(hdmi->hdmi_regs, REG_TX_PACK_FIFO_CTRL,
                REG_TMDS_PACK_MODE_M, reg_pack_mode(mode));

    /* Enable/disable PKT send, and enable/disable avmute in phase */
    hdmi_clrset(hdmi->hdmi_regs, REG_AVMIXER_CONFIG,
                REG_DC_PKT_EN_M | REG_AVMUTE_IN_PHASE_M,
                reg_dc_pkt_en(enable) | reg_avmute_in_phase(enable));
}

static hi_void ctrl_hw_enable_scramble(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_HDMI_ENC_CTRL,
                REG_ENC_HDMI2_ON_M | REG_ENC_SCR_ON_M,
                reg_enc_hdmi2_on(enable) | reg_enc_scr_on(enable));
}

/*
 * @ppc: pix per cecle
 */
static hi_void ctrl_hw_set_csc_rgb_yuv(struct hisilicon_hdmi *hdmi)
{
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF00, 0x174e);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF01, 0x4e67);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF02, 0x7ea);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF10, 0xf327);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF11, 0xd4c9);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF12, 0x3810);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF20, 0x3810);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF21, 0xcd23);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF22, 0xfadc);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_SCALE, 0xf);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_IDC0, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_IDC1, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_IDC2, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_ODC0, 0x100);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_ODC1, 0x800);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_ODC2, 0x800);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MIN_Y, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MIN_C, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MAX_Y, 0xfff);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MAX_C, 0xfff);

    if (hdmi->is_hdmi21) {
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF00, 0x100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF01, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF02, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF10, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF11, 0x100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF12, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF20, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF21, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF22, 0x100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_SCALE, 0x8);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_IDC0, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_IDC1, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_IDC2, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_ODC0, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_ODC1, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_ODC2, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MIN_Y, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MIN_C, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MAX_Y, 0xfff);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MAX_C, 0xfff);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_INK_CTRL, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_INK_POS, 0x0);
    }
}

static hi_void ctrl_hw_set_csc_yuv_rgb(struct hisilicon_hdmi *hdmi)
{
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF00, 0x400);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF01, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF02, 0x628);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF10, 0x400);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF11, 0xff45);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF12, 0xfe2a);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF20, 0x400);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF21, 0x743);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_COEF22, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_SCALE, 0xa);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_IDC0, 0x1f00);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_IDC1, 0x1800);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_IDC2, 0x1800);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_ODC0, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_ODC1, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_ODC2, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MIN_Y, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MIN_C, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MAX_Y, 0xfff);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_MAX_C, 0xfff);

    if (hdmi->is_hdmi21) {
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF00, 0x100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF01, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF02, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF10, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF11, 0x100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF12, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF20, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF21, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_COEF22, 0x100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_SCALE, 0x8);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_IDC0, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_IDC1, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_IDC2, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_ODC0, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_ODC1, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_ODC2, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MIN_Y, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MIN_C, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MAX_Y, 0xfff);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC2_MAX_C, 0xfff);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_INK_CTRL, 0x0);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_INK_POS, 0x0);
    }
}

static hi_void ctrl_hw_set_csc_en(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_HIPP_CSC_CTRL,
                REG_HIPP_CSC_EN_M, reg_hipp_csc_en(enable));

    hdmi_clrset(hdmi->hdmi_regs, REG_CSC_DITHER_DFIR_EN,
                REG_CSC_CHN_EN_M, reg_csc_chn_en(enable));
}

static hi_void ctrl_hw_set_dither_12b_10b(struct hisilicon_hdmi *hdmi)
{
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_DEMO_CTRL, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_THR, 0x3ff0000);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_TAPEMODE, 0x33333333);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(0), 0x7fffffff);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(0), 0x70103153);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(0), 0x7f010305);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(0), 0x7ff00031);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_INK_CTRL, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_INK_POS, 0x0);

    if (hdmi->is_hdmi21) {
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(1), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(1), 0x7fffffff);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(1), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(1), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(2), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(2), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(2), 0x7fffffff);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(2), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(3), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(3), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(3), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(3), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(4), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(4), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(4), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(4), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(5), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(5), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(5), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(5), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(6), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(6), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(6), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(6), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(7), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(7), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(7), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(7), 0x7fff0003);
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL,
                REG_DITHER_RESET_M, reg_dither_reset(0x3));
    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL,
                REG_BITDEPTHIN_M, reg_bitdepthin(0xc));
    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL,
                REG_BITDEPTHTEMP_M, reg_bitdepthtemp(0xc));
    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL,
                REG_BITDEPTHOUT_M, reg_bitdepthout(0xa));
}

static hi_void ctrl_hw_set_dither_12b_8b(struct hisilicon_hdmi *hdmi)
{
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_DEMO_CTRL, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_THR, 0x3ff0000);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_TAPEMODE, 0x33333333);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(0), 0x7fffffff);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(0), 0x70103153);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(0), 0x7f010305);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(0), 0x7ff00031);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_INK_CTRL, 0x2c);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_INK_POS, 0xd000c);

    if (hdmi->is_hdmi21) {
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(1), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(1), 0x7fffffff);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(1), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(1), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(2), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(2), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(2), 0x7fffffff);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(2), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(3), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(3), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(3), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(3), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(4), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(4), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(4), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(4), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(5), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(5), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(5), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(5), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(6), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(6), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(6), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(6), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(7), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(7), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(7), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(7), 0x7fff0003);

        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_DITHER_RESET_M, reg_dither_reset(0x3));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHIN_M, reg_bitdepthin(0xc));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHTEMP_M, reg_bitdepthtemp(0xc));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHOUT_M, reg_bitdepthout(0x8));
    } else {
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_DITHER_RESET_M, reg_dither_reset(0x4));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHIN_M, reg_bitdepthin(0xc));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHTEMP_M, reg_bitdepthtemp(0xa));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHOUT_M, reg_bitdepthout(0x8));
    }
}

static hi_void ctrl_hw_set_dither_10b_8b(struct hisilicon_hdmi *hdmi)
{
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_DEMO_CTRL, 0x0);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_THR, 0x3ff0000);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_TAPEMODE, 0x33333333);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(0), 0x7fffffff);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(0), 0x70103153);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(0), 0x7f010305);
    hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(0), 0x7ff00031);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_INK_CTRL, 0x2c);
    hdmi_writel(hdmi->hdmi_regs, REG_VDP_DITHER_INK_POS, 0xd000c);

    if (hdmi->is_hdmi21) {
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(1), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(1), 0x7fffffff);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(1), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(1), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(2), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(2), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(2), 0x7fffffff);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(2), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(3), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(3), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(3), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(3), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(4), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(4), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(4), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(4), 0xfff3);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(5), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(5), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(5), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(5), 0x7fffff01);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(6), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(6), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(6), 0x7fff0003);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(6), 0xfef315);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_y(7), 0x70103153);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_u(7), 0x7f010305);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_v(7), 0x7ff00031);
        hdmi_writel(hdmi->hdmi_regs, reg_vdp_dither_seed_w(7), 0x7fff0003);

        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_DITHER_RESET_M, reg_dither_reset(0x3));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHIN_M, reg_bitdepthin(0xa));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHTEMP_M, reg_bitdepthtemp(0xa));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHOUT_M, reg_bitdepthout(0x8));
    } else {
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_DITHER_RESET_M, reg_dither_reset(0x4));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHIN_M, reg_bitdepthin(0xa));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHTEMP_M, reg_bitdepthtemp(0xa));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_BITDEPTHOUT_M, reg_bitdepthout(0x8));
        hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL, REG_DITHER_CK_GT_EN_M, reg_dither_ck_gt_en(1));
    }
}

static hi_void ctrl_hw_set_dither_en(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL,
                REG_DITHER_ENABLE_M, reg_dither_enable(enable));

    hdmi_clrset(hdmi->hdmi_regs, REG_CSC_DITHER_DFIR_EN,
                REG_DITHER_CHN_EN_M, reg_dither_chn_en(enable));
}

static hi_void ctrl_hw_set_dfir(const struct hisilicon_hdmi *hdmi)
{
    if (hdmi->is_hdmi21) {
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_CMCMODEH, 0x1084376c);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_CMCMODEL, 0x54842);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF01, 0x3f00000);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF23, 0x900100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF45, 0x900000);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF6, 0x3f0);
    } else {
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF01, 0x3f00000);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF23, 0x900100);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF45, 0x900000);
        hdmi_writel(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_HFIRCOEF6, 0x3f0);
    }
}

static hi_void ctrl_hw_set_dfir_422_en(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_CSC_DITHER_DFIR_EN,
                REG_DFIR_422_EN_M, reg_dfir_422_en(enable));
}

static hi_void ctrl_hw_set_dfir_420_en(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_CSC_DITHER_DFIR_EN,
                REG_DFIR_420_EN_M, reg_dfir_420_en(enable));
}

static hi_void ctrl_hw_set_yuv_mode_en(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_CSC_DITHER_DFIR_EN,
                REG_VDP2HDMI_YUV_MODE_M, reg_vdp2hdmi_yuv_mode(enable));
}

static hi_void ctrl_hw_set_422_mode_en(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_VID_ADAPTER,
                REG_VID_IN_422_MODE_M, reg_vid_in_422_mode(enable));
}

static hi_void ctrl_hw_set_audio_intf(struct hisilicon_hdmi *hdmi, hi_u32 type)
{
    hi_u8 i2s_en = 0;
    hi_bool spdif_en = false;
    hi_bool hbr_en = false;

    switch (type) {
        case HDMI_AUDIO_INPUT_TYPE_I2S:
            i2s_en = 0xf;
            spdif_en = false;
            hbr_en = false;
            break;

        case HDMI_AUDIO_INPUT_TYPE_SPDIF:
            i2s_en = 0;
            spdif_en = true;
            hbr_en = false;
            break;

        case HDMI_AUDIO_INPUT_TYPE_HBR:
            i2s_en = 0xf;
            spdif_en = false;
            hbr_en = true;
            break;

        default:
            break;
    }
    hdmi_clrset(hdmi->hdmi_regs, REG_TX_AUDIO_CTRL,
                REG_AUD_I2S_EN_M | REG_AUD_SPDIF_EN_M,
                reg_aud_i2s_en(i2s_en) | reg_aud_spdif_en(spdif_en));

    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_HBRA_ON_M, reg_i2s_hbra_on(hbr_en));
}

static hi_void ctrl_hw_set_audio_mute(struct hisilicon_hdmi *hdmi,
                                      hi_u32 type, hi_bool mute)
{
    switch (type) {
        /* Mute audio, don't send audio packet */
        case HDMI_AUDIO_MUTE_TYPE_IN:
            hdmi_clrset(hdmi->hdmi_regs, REG_TX_AUDIO_CTRL,
                        REG_AUD_MUTE_EN_M, reg_aud_mute_en(mute));
            break;
        /* Mute audio, send audio packet, data is zero */
        case HDMI_AUDIO_MUTE_TYPE_DATA:
            hdmi_clrset(hdmi->hdmi_regs, REG_TX_AUDIO_CTRL,
                        REG_CFG_AUDIO_DATA_MUTE_EN_M, reg_cfg_audio_data_mute_en(mute));
            break;
        /* Mute audio, send audio packet, the sf is 1 */
        case HDMI_AUDIO_MUTE_TYPE_SF:
            hdmi_clrset(hdmi->hdmi_regs, REG_TX_AUDIO_CTRL,
                        REG_CFG_AUDIO_SF_EN_M, reg_cfg_audio_sf_en(mute));
            break;
        default:
            break;
    }
}

static hi_void ctrl_hw_set_audio_layout(struct hisilicon_hdmi *hdmi, hi_bool layout)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_TX_AUDIO_CTRL,
                REG_AUD_LAYOUT_M, reg_aud_layout(layout));
}

static hi_void ctrl_hw_set_i2s_ch_swap(struct hisilicon_hdmi *hdmi, hi_u32 swap)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_CH_SWAP_M, reg_i2s_ch_swap(swap));
}

static hi_void ctrl_hw_set_i2s_length(struct hisilicon_hdmi *hdmi, hi_u32 length)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_LENGTH_M, reg_i2s_length(length));
}

static hi_void ctrl_hw_set_i2s_vbit(struct hisilicon_hdmi *hdmi,
                                    hi_bool compress)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_VBIT_M, reg_i2s_vbit(compress));
}

static hi_void ctrl_hw_set_i2s_data_dir(struct hisilicon_hdmi *hdmi,
                                        hi_bool msb_first)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_DATA_DIR_M, reg_i2s_data_dir(msb_first));
}

static hi_void ctrl_hw_set_i2s_justify(struct hisilicon_hdmi *hdmi,
                                       hi_bool ws_right)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_JUSTIFY_M, reg_i2s_justify(ws_right));
}

static hi_void ctrl_hw_set_i2s_ws_polarity(struct hisilicon_hdmi *hdmi,
                                           hi_bool msb_left)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_WS_POLARITY_M, reg_i2s_ws_polarity(msb_left));
}

static hi_void ctrl_hw_set_i2s_1st_shift(struct hisilicon_hdmi *hdmi,
                                         hi_bool shift)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_I2S_CTRL,
                REG_I2S_1ST_SHIFT_M, reg_i2s_1st_shift(shift));
}

static hi_void ctrl_hw_set_i2s_chn_status(struct hisilicon_hdmi *hdmi,
                                          struct ctrl_i2s_status *status)
{
    hdmi_clrset(hdmi->hdmi_regs, AUD_CHST_CFG0,
                REG_CHST_BYTE0_A_M, reg_chst_byte0_a(status->professional_app));

    hdmi_clrset(hdmi->hdmi_regs, AUD_CHST_CFG0,
                REG_CHST_BYTE0_B_M, reg_chst_byte0_b(status->compress));

    hdmi_clrset(hdmi->hdmi_regs, AUD_CHST_CFG0,
                REG_CHST_BYTE3_FS_M, reg_chst_byte3_fs(status->sample_rate));

    hdmi_clrset(hdmi->hdmi_regs, AUD_CHST_CFG0,
                REG_CHST_BYTE3_CLK_ACCURACY_M,
                reg_chst_byte3_clk_accuracy(status->clk_accuracy));

    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_CHST_CFG1,
                REG_CHST_BYTE4_ORG_FS_M, reg_chst_byte4_org_fs(status->org_rate));

    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_CHST_CFG1,
                REG_CHST_BYTE4_LENGTH_M, reg_chst_byte4_length(status->out_bit_length));
}

static hi_void ctrl_hw_set_audio_cts(struct hisilicon_hdmi *hdmi,
                                     hi_bool sw_sel, hi_u32 val)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_ACR_CTRL,
                REG_ACR_CTS_HW_SW_SEL_M, reg_acr_cts_hw_sw_sel(sw_sel));

    hdmi_clrset(hdmi->hdmi_regs, REG_ACR_CTS_VAL_SW,
                REG_ACR_CTS_VAL_M, reg_acr_cts_val(val));
}

static hi_void ctrl_hw_enable_sw_cts(const struct hisilicon_hdmi *hdmi,
                                     hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AUD_ACR_CTRL,
                REG_ACR_CTS_HW_SW_SEL_M, reg_acr_cts_hw_sw_sel(enable));
}

static hi_u32 ctrl_hw_get_sw_cts(const struct hisilicon_hdmi *hdmi)
{
    return hdmi_readl(hdmi->hdmi_regs, REG_ACR_CTS_VAL_SW) & REG_ACR_CTS_VAL_M;
}

static hi_u32 ctrl_hw_get_hw_cts(const struct hisilicon_hdmi *hdmi)
{
    return hdmi_readl(hdmi->hdmi_regs, REG_ACR_CTS_VAL_HW) & REG_ACR_CTS_VAL_M;
}

static hi_void ctrl_hw_set_audio_n(struct hisilicon_hdmi *hdmi, hi_u32 val)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_ACR_N_VAL_SW,
                REG_ACR_N_VAL_M, reg_acr_n_val(val));
}

static hi_void ctrl_hw_pwd_reset(struct hisilicon_hdmi *hdmi)
{
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_PWD_SRST_REQ_M);
    osal_udelay(10); /* need delay 10us. */
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_PWD_SRST_REQ_M);
}

static hi_void ctrl_hw_fast_reset(struct hisilicon_hdmi *hdmi)
{
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_VID_SRST_REQ_M);
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDMI_SRST_REQ_M);
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDCP1X_SRST_REQ_M);
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_ACR_SRST_REQ_M);
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_AFIFO_SRST_REQ_M);
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDCP2X_SRST_REQ_M);
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_FRL_FAST_ARST_REQ_M);
    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_CSC_DITHER_SRST_REQ_M);
    osal_udelay(10); /* need delay 10us. */
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_CSC_DITHER_SRST_REQ_M);
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_FRL_FAST_ARST_REQ_M);
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDCP2X_SRST_REQ_M);
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_AFIFO_SRST_REQ_M);
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_ACR_SRST_REQ_M);
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDCP1X_SRST_REQ_M);
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDMI_SRST_REQ_M);
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_VID_SRST_REQ_M);
}

static hi_void ctrl_hw_csc_reset(const struct hisilicon_hdmi *hdmi)
{
    hi_u32 hactive;
    hi_u32 vactive;

    hactive = hdmi->ctrl->mode.timing_data.in.detail.hactive;
    vactive = hdmi->ctrl->mode.timing_data.in.detail.vactive;

    hdmi_set(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_CSC_DITHER_SRST_REQ_M);
    osal_udelay(10); /* need delay 10us. */
    hdmi_clr(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_CSC_DITHER_SRST_REQ_M);

    /*
     * Software workaround. Erratic display occurs when the output
     * is in some timings (hactive or hblank is not 8-aligned).
     */
    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_RESO,
                REG_FRM_HEIGHT_M | REG_FRM_WIDTH_M,
                reg_frm_height(0) | reg_frm_width(0));
    hdmi_clrset(hdmi->hdmi_regs, REG_VDP_VDP2HDMI_RESO,
                REG_FRM_HEIGHT_M | REG_FRM_WIDTH_M,
                reg_frm_height(vactive - 1) | reg_frm_width(hactive - 1));
}

static hi_void ctrl_hw_set_video_black(struct hisilicon_hdmi *hdmi,
                                       hi_u32 color_space, hi_bool enable)
{
    hi_u32 solid_patten_r_cr;
    hi_u32 solid_patten_g_y;
    hi_u32 solid_patten_b_cb;

    solid_patten_r_cr = (color_space == HDMI_COLOR_FORMAT_RGB) ?
        VIDEO_BLACK_DATA_RGB_R : VIDEO_BLACK_DATA_YUV_CR;
    solid_patten_g_y = (color_space == HDMI_COLOR_FORMAT_RGB) ?
        VIDEO_BLACK_DATA_RGB_G : VIDEO_BLACK_DATA_YUV_Y;
    solid_patten_b_cb = (color_space == HDMI_COLOR_FORMAT_RGB) ?
        VIDEO_BLACK_DATA_RGB_B : VIDEO_BLACK_DATA_YUV_CB;

    hdmi_clrset(hdmi->hdmi_regs, REG_SOLID_PATTERN_CONFIG,
                REG_SOLID_PATTERN_CR_M | REG_SOLID_PATTERN_Y_M | REG_SOLID_PATTERN_CB_M,
                reg_solid_pattern_cr(solid_patten_r_cr) |
                reg_solid_pattern_y(solid_patten_g_y) |
                reg_solid_pattern_cb(solid_patten_b_cb));

    hdmi_clrset(hdmi->hdmi_regs, REG_PATTERN_GEN_CTRL,
                REG_TPG_ENABLE_M | REG_SOLID_PATTERN_EN_M,
                reg_tpg_enable(enable) | reg_solid_pattern_en(enable));
}

static hi_bool ctrl_hw_tmds_clk_is_stable(struct hisilicon_hdmi *hdmi)
{
    hi_u32 data;
    hi_bool stable;

    data = hdmi_readl(hdmi->hdmi_regs, REG_TX_PACK_FIFO_ST);
    stable = (data & REG_PCLK2TCLK_STABLE) ? true : false;

    return stable;
}

static hi_s32 ctrl_set_avi_infoframe(struct hdmi_controller *ctrl)
{
    struct hdmi_avi_infoframe frame;
    hi_u8 buffer[HDMI_PACKET_SIZE];
    ssize_t err;

    if (ctrl == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    err = hdmi_avi_infoframe_init(&frame);
    if (err < 0) {
        HDMI_ERR("Failed to setup avi infoframe: %zd\n", err);
        return err;
    }

    ctrl_avi_infoframe_config(ctrl, &frame);

    err = hdmi_avi_infoframe_pack(&frame, buffer, sizeof(buffer));
    if (err < 0) {
        HDMI_ERR("Failed to pack AVI infoframe: %zd\n", err);
        return err;
    }

    hal_ctrl_hw_send_info_frame(ctrl->parent, buffer, sizeof(buffer));

    return 0;
}

static hi_s32 ctrl_set_aud_infoframe(struct hdmi_controller *ctrl)
{
    struct hdmi_audio_infoframe frame;
    hi_u8 buffer[HDMI_PACKET_SIZE];
    ssize_t err;

    err = hdmi_audio_infoframe_init(&frame);
    if (err < 0) {
        HDMI_ERR("Failed to setup audio infoframe: %zd\n", err);
        return err;
    }

    frame.coding_type = HDMI_AUDIO_CODING_TYPE_STREAM;
    frame.sample_frequency = HDMI_AUDIO_SAMPLE_FREQUENCY_STREAM;
    frame.sample_size = HDMI_AUDIO_SAMPLE_SIZE_STREAM;
    frame.channels = ctrl->attr.aud_channels - 1; /* audio channel:2~8, but protocol standard is 1~7 */

    switch (ctrl->attr.aud_channels) { /* HDMI channel map */
        case HDMI_AUDIO_CHANNEL_3CH:
            frame.channel_allocation = 0x01;
            break;
        case HDMI_AUDIO_CHANNEL_6CH:
            frame.channel_allocation = 0x0b;
            break;
        case HDMI_AUDIO_CHANNEL_8CH:
            frame.channel_allocation = 0x13;
            break;
        default:
            frame.channel_allocation = 0x00;
            break;
    }

    err = hdmi_audio_infoframe_pack(&frame, buffer, sizeof(buffer));
    if (err < 0) {
        HDMI_ERR("Failed to pack audio infoframe: %zd\n", err);
        return err;
    }

    hal_ctrl_hw_send_info_frame(ctrl->parent, buffer, sizeof(buffer));
    return 0;
}

static hi_s32 ctrl_set_vendor_specific_infoframe(struct hdmi_controller *ctrl)
{
    struct hdmi_vendor_infoframe frame;
    struct hdmi_in_data *in = NULL;
    hi_u8 buffer[HDMI_PACKET_SIZE];
    ssize_t err;

    if (ctrl == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    in = &ctrl->mode.timing_data.in;

    err = hdmi_vendor_infoframe_init(&frame);
    if (err < 0) {
        HDMI_ERR("Failed to setup vendor infoframe: %zd\n", err);
        return err;
    }

    if (in->mode_3d == HDMI_3D_NONE) {
        if (in->vic == VIC_3840X2160P30_16_9) {
            frame.vic = 1; /* hdmi vic is 1 */
        } else if (in->vic == VIC_3840X2160P25_16_9) {
            frame.vic = 2; /* hdmi vic is 2 */
        } else if (in->vic == VIC_3840X2160P24_16_9) {
            frame.vic = 3; /* hdmi vic is 3 */
        } else if (in->vic == VIC_4096X2160P24_256_135) {
            frame.vic = 4; /* hdmi vic is 4 */
        }
    }

    frame.s3d_struct = (in->mode_3d == HDMI_3D_NONE) ?
        HDMI_3D_STRUCTURE_INVALID : in->mode_3d;

    if (frame.vic) {
        frame.length = 5; /* hdmi vic is not zero, the length must be 5. */
    } else if (frame.s3d_struct != HDMI_3D_STRUCTURE_INVALID) {
        frame.length = 7; /* 3d struct is not none, the length must be 7. */
    }

    err = hdmi_vendor_infoframe_pack(&frame, buffer, sizeof(buffer));
    if (err < 0) {
        HDMI_ERR("Failed to pack vendor infoframe: %zd\n", err);
        return err;
    }

    hal_ctrl_hw_send_info_frame(ctrl->parent, buffer, sizeof(buffer));
    return 0;
}

struct hdmi_controller *hal_ctrl_controller_init(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = HI_NULL;

    if (hdmi == HI_NULL) {
        HDMI_ERR("ptr is null.\n");
        return NULL;
    }

    controller = osal_kmalloc(HI_ID_HDMITX, sizeof(struct hdmi_controller), OSAL_GFP_KERNEL);
    if (controller == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_NULL;
    }

    if (memset_s(controller, sizeof(struct hdmi_controller), 0, sizeof(struct hdmi_controller))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, controller);
        return HI_NULL;
    }

    controller->parent = hdmi;
    /* version */
    controller->version = 1;
    controller->work_mode_prior = HDMI_PRIOR_AUTIO;
    controller->tmds_prior = HI_TRUE;
    controller->min_rate_prior = HI_TRUE;
    controller->tpll_enable = HI_FALSE;
    controller->ppll_enable = HI_FALSE;
    controller->force_output = HI_FALSE;
    controller->audio_mute_type = HDMI_AUDIO_MUTE_TYPE_DATA;

    /* hi_display_mode init */
    hal_ctrl_hw_get_display_mode(controller);

    /* hw_config init */
    controller->pre_hw_config.work_mode = HDMI_WORK_MODE_TMDS;
    controller->pre_hw_config.tmds_scr_en = hal_ctrl_hw_get_source_scramble(hdmi);
    controller->pre_hw_config.tmds_clock = hdmi_mode_clock_pixel2tmds(
        controller->mode.timing_data.in.pixel_clock,
        controller->mode.timing_data.out.color_depth,
        controller->mode.timing_data.out.color.color_format);
    controller->pre_hw_config.dvi_mode = !hal_ctrl_hw_get_hdmi_mode(hdmi);
    controller->pre_hw_config.dsc_enable = false;

    controller->cur_hw_config = controller->pre_hw_config;

    controller->dsc = hdmi_dsc_init(controller);
    if (!controller->dsc) {
        osal_kfree(HI_ID_HDMITX, controller);
        return HI_NULL;
    }

    controller->frl = hitxv300_frl_init(controller, hdmi->hdmi_regs);
    if (!controller->frl) {
        osal_kfree(HI_ID_HDMITX, controller);
        return HI_NULL;
    }

    return controller;
}

hi_void hal_ctrl_set_video_black(struct hdmi_controller *ctrl, hi_bool enable)
{
    struct hdmi_out_data *out = &ctrl->mode.timing_data.out;
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    ctrl_hw_set_video_black(hdmi, out->color.color_format, enable);
}

static hi_s32 ctrl_set_color_format(struct hisilicon_hdmi *hdmi,
    hi_u32 in_color_format, hi_u32 out_color_format)
{
    hi_bool yuv_mode_en = false;
    hi_bool dfir_422_en = false;
    hi_bool dfir_420_en = false;
    hi_bool csc_en = false;
    hi_bool yuv422_mode_en = false;

    if (in_color_format == HDMI_COLOR_FORMAT_RGB) {
        switch (out_color_format) {
            case HDMI_COLOR_FORMAT_RGB:
                yuv_mode_en = false;
                dfir_422_en = false;
                dfir_420_en = false;
                csc_en = false;
                yuv422_mode_en = false;
                break;
            case HDMI_COLOR_FORMAT_YCBCR444:
                yuv_mode_en = true;
                dfir_422_en = false;
                dfir_420_en = false;
                csc_en = true;
                yuv422_mode_en = false;
                break;
            case HDMI_COLOR_FORMAT_YCBCR422:
                yuv_mode_en = true;
                dfir_422_en = true;
                dfir_420_en = false;
                csc_en = true;
                yuv422_mode_en = hdmi->is_hdmi21 ? true : false; /* dsc must be 0 */
                break;
            case HDMI_COLOR_FORMAT_YCBCR420:
                yuv_mode_en = true;
                dfir_422_en = true;
                dfir_420_en = true;
                csc_en = true;
                yuv422_mode_en = false;
                break;
            default:
                break;
        }
        ctrl_hw_set_csc_rgb_yuv(hdmi);
        ctrl_hw_set_dfir(hdmi); /* ??? */
    } else if (in_color_format == HDMI_COLOR_FORMAT_YCBCR444) {
        switch (out_color_format) {
            case HDMI_COLOR_FORMAT_RGB:
                yuv_mode_en = false;
                dfir_422_en = false;
                dfir_420_en = false;
                csc_en = true;
                yuv422_mode_en = false;
                break;
            case HDMI_COLOR_FORMAT_YCBCR444:
                yuv_mode_en = true;
                dfir_422_en = false;
                dfir_420_en = false;
                csc_en = false;
                yuv422_mode_en = false;
                break;
            case HDMI_COLOR_FORMAT_YCBCR422:
                yuv_mode_en = true;
                dfir_422_en = true;
                dfir_420_en = false;
                csc_en = false;
                yuv422_mode_en = hdmi->is_hdmi21 ? true : false; /* dsc must be 0; */
                break;
            case HDMI_COLOR_FORMAT_YCBCR420:
                yuv_mode_en = true;
                dfir_422_en = true;
                dfir_420_en = true;
                csc_en = false;
                yuv422_mode_en = false;
                break;
            default:
                break;
        }
        ctrl_hw_set_csc_yuv_rgb(hdmi);
        ctrl_hw_set_dfir(hdmi); /* ??? */
    } else {
        HDMI_ERR("Not support this input color format: %d!\n", in_color_format);
        return -EINVAL;
    }

    ctrl_hw_set_yuv_mode_en(hdmi, yuv_mode_en);
    ctrl_hw_set_csc_en(hdmi, csc_en);
    ctrl_hw_set_dfir_422_en(hdmi, dfir_422_en);
    ctrl_hw_set_dfir_420_en(hdmi, dfir_420_en);
    ctrl_hw_set_422_mode_en(hdmi, yuv422_mode_en);

    return 0;
}

static hi_s32 ctrl_set_color_depth(struct hisilicon_hdmi *hdmi,
    hi_u32 in_color_depth, hi_u32 out_color_depth)
{
    hi_bool dither_en = false;
    hi_u8 pack_mode = 0;
    hi_bool deep_color_en = false;

    if (in_color_depth == HDMI_BPC_24) {
        switch (out_color_depth) {
            case HDMI_BPC_24:
                pack_mode = 0;
                dither_en = false;
                deep_color_en = false;
                break;
            case HDMI_BPC_30:
                pack_mode = 1;
                dither_en = false;
                deep_color_en = true;
                break;
            case HDMI_BPC_36:
                pack_mode = 2;
                dither_en = false;
                deep_color_en = true;
                break;
            default:
                break;
        }
    } else if (in_color_depth == HDMI_BPC_30) {
        switch (out_color_depth) {
            case HDMI_BPC_24:
                pack_mode = 0;
                dither_en = true;
                deep_color_en = false;
                ctrl_hw_set_dither_10b_8b(hdmi);
                break;
            case HDMI_BPC_30:
                pack_mode = 1;
                dither_en = false;
                deep_color_en = true;
                break;
            case HDMI_BPC_36:
                pack_mode = 2;
                dither_en = false;
                deep_color_en = true;
                break;
            default:
                break;
        }
    } else if (in_color_depth == HDMI_BPC_36) {
        switch (out_color_depth) {
            case HDMI_BPC_24:
                pack_mode = 0;
                dither_en = true;
                deep_color_en = false;
                ctrl_hw_set_dither_12b_8b(hdmi);
                break;
            case HDMI_BPC_30:
                pack_mode = 1;
                dither_en = true;
                deep_color_en = true;
                ctrl_hw_set_dither_12b_10b(hdmi);
                break;
            case HDMI_BPC_36:
                pack_mode = 2;
                dither_en = false;
                deep_color_en = true;
                break;
            default:
                break;
        }
    } else {
        HDMI_ERR("Not support this input color depth: %d!\n", in_color_depth);
        return -EINVAL;
    }

    ctrl_hw_set_deepcolor(hdmi, pack_mode, deep_color_en);
    ctrl_hw_set_dither_en(hdmi, dither_en);

    return 0;
}

hi_s32 hal_ctrl_set_video_path(struct hdmi_controller *ctrl)
{
    hi_s32 ret;
    hi_u32 in_color_format =
        ctrl->mode.timing_data.in.color.color_format;
    hi_u32 out_color_format =
        ctrl->mode.timing_data.out.color.color_format;
    hi_u32 in_color_depth =
        ctrl->mode.timing_data.in.color_depth;
    hi_u32 out_color_depth =
        ctrl->mode.timing_data.out.color_depth;
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    ret = ctrl_set_color_format(hdmi, in_color_format, out_color_format);
    if (ret) {
        return -EINVAL;
    }

    ret = ctrl_set_color_depth(hdmi, in_color_depth, out_color_depth);
    if (ret) {
        return -EINVAL;
    }

    ctrl_hw_csc_reset(hdmi);
    ctrl_hw_set_vdp_packet(hdmi);

    return ret;
}

hi_s32 hal_ctrl_disable_deepcolor_for_dsc(struct hdmi_controller *ctrl)
{
    hi_u8 pack_mode = 0;
    struct hisilicon_hdmi *hdmi = NULL;

    if (!ctrl) {
        HDMI_ERR("ctrl pointer is null!\n");
        return -EINVAL;
    }

    hdmi = ctrl->parent;
    if (!hdmi) {
        HDMI_ERR("hdmi pointer is null!\n");
        return -EINVAL;
    }

    ctrl_hw_set_deepcolor(hdmi, pack_mode, false);

    return 0;
}

static hi_s32 ctrl_hw_get_audio_ncts(struct hisilicon_hdmi *hdmi, ncts_val *ncts)
{
    hi_u32 data;

    data = hdmi_readl(hdmi->hdmi_regs, REG_ACR_CTS_VAL_SW);
    ncts->cts_value = data & REG_ACR_CTS_VAL_M;

    data = hdmi_readl(hdmi->hdmi_regs, REG_ACR_N_VAL_SW);
    ncts->n_value = data & REG_ACR_N_VAL_M;

    return 0;
}

static hi_s32 ctrl_hw_get_audio_layout(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = NULL;
    struct ao_attr *attr = NULL;
    hi_u32 data;
    hi_u32 layout;
    hi_s32 chn_num;

    if (hdmi == NULL) {
        HDMI_ERR("ptr null err.\n");
        return -EINVAL;
    }

    ctrl = hdmi->ctrl;
    attr = &ctrl->attr;
    data = hdmi_readl(hdmi->hdmi_regs, REG_TX_AUDIO_CTRL);
    layout = (data & REG_AUD_LAYOUT_M) >> 2; /* shift 2 bit */

    if ((attr->aud_input_type != HDMI_AUDIO_INPUT_TYPE_SPDIF) && (layout == 1)) {
        chn_num = HDMI_AUDIO_CHANNEL_8CH;
    } else if ((attr->aud_input_type != HDMI_AUDIO_INPUT_TYPE_SPDIF) && (layout == 0)) {
        chn_num = HDMI_AUDIO_CHANNEL_2CH;
    } else {
        chn_num = -EINVAL;
    }

    return chn_num;
}

hi_s32 get_sample_rate_val(hi_u32 org_reg_val, hi_u32 rate_reg_val)
{
    hi_s32 i;

    for (i = 0; i < ARRAY_SIZE(g_i2s_samp_rate); i++) {
        if ((g_i2s_samp_rate[i].rate_org_reg == org_reg_val) &&
            (g_i2s_samp_rate[i].rate_reg == rate_reg_val)) {
            return i;
        }
    }

    return -EINVAL;
}

static hi_s32 ctrl_hw_get_i2s_chn_status(struct hisilicon_hdmi *hdmi, struct ctrl_i2s_status *status)
{
    hi_u32 data;

    data = hdmi_readl(hdmi->hdmi_regs, AUD_CHST_CFG0);
    status->sample_rate = (data & REG_CHST_BYTE3_FS_M) >> 24; /* shift 24 bit */

    data = hdmi_readl(hdmi->hdmi_regs, REG_AUD_CHST_CFG1);
    status->org_rate = (data & REG_CHST_BYTE4_ORG_FS_M) >> 4; /* shift 4 bit */

    data = hdmi_readl(hdmi->hdmi_regs, REG_AUD_CHST_CFG1);
    status->out_bit_length = data & REG_CHST_BYTE4_LENGTH_M;

    return 0;
}

hi_u32 ctrl_hw_get_i2s_vbit(struct hisilicon_hdmi *hdmi)
{
    hi_u32 data;

    if (hdmi == NULL) {
        HDMI_ERR("ptr null err.\n");
        return -EINVAL;
    }

    data = hdmi_readl(hdmi->hdmi_regs, REG_AUD_I2S_CTRL);

    return (data & REG_I2S_VBIT_M) >> 5; /* shift 5 bit */
}

hi_u32 ctrl_hw_get_i2s_length(struct hisilicon_hdmi *hdmi)
{
    hi_u32 data;

    if (hdmi == NULL) {
        HDMI_ERR("ptr null err.\n");
        return -EINVAL;
    }

    data = hdmi_readl(hdmi->hdmi_regs, REG_AUD_I2S_CTRL);
    return (data & REG_I2S_LENGTH_M) >> 8; /* shift 8 bit */
}

hi_u32 ctrl_hw_get_audio_intf(struct hisilicon_hdmi *hdmi)
{
    hi_u32 data;
    hi_u32 type = 0;
    hi_u8 i2s_en;
    hi_u8 spdif_en;
    hi_u8 hbr_en;

    data = hdmi_readl(hdmi->hdmi_regs, REG_TX_AUDIO_CTRL);
    i2s_en = (data & REG_AUD_I2S_EN_M) >> 4; /* shift 4 bit */
    spdif_en = (data & REG_AUD_SPDIF_EN_M) >> 8; /* shift 8 bit */

    data = hdmi_readl(hdmi->hdmi_regs, REG_AUD_I2S_CTRL);
    hbr_en = data & REG_I2S_HBRA_ON_M;

    if ((i2s_en == 0xf) && (spdif_en == 0) && (hbr_en == 0)) {
        type = HDMI_AUDIO_INPUT_TYPE_I2S;
    } else if ((i2s_en == 0) && (spdif_en == 1) && (hbr_en = 0)) {
        type =  HDMI_AUDIO_INPUT_TYPE_SPDIF;
    } else if ((i2s_en == 0xf) && (spdif_en == 0) && (hbr_en == 1)) {
        type = HDMI_AUDIO_INPUT_TYPE_HBR;
    }

    return 0;
}

hi_s32 hal_ctrl_set_audio_path(struct hdmi_controller *ctrl)
{
    hi_bool hbr_on;
    hi_bool layout;
    hi_bool compress;
    hi_u32 org_sample_rate;
    struct ao_attr *attr = &ctrl->attr;
    struct hisilicon_hdmi *hdmi = ctrl->parent;
    struct ctrl_i2s_status status;

    ctrl_hw_set_audio_intf(hdmi, attr->aud_input_type);

    org_sample_rate = attr->aud_sample_rate;

    if (attr->aud_input_type == HDMI_AUDIO_INPUT_TYPE_HBR) {
        hbr_on = true;
        org_sample_rate = HDMI_AUDIO_SAMPLE_RATE_768K;
    }

    if (attr->aud_input_type != HDMI_AUDIO_INPUT_TYPE_SPDIF) {
        /* I2S layout set */
        if (hbr_on || attr->aud_channels == HDMI_AUDIO_CHANNEL_2CH) {
            layout = false;
        } else if (attr->aud_channels <= HDMI_AUDIO_CHANNEL_8CH) {
            layout = true;
        } else {
            HDMI_ERR("error audio channel number:%u\n", attr->aud_channels);
            layout = false;
        }
        ctrl_hw_set_audio_layout(hdmi, layout);

        compress = (attr->aud_codec == HDMI_AUDIO_CODE_TYPE_PCM) ? false : true;

        /* I2S ctrl set */
        ctrl_hw_set_i2s_1st_shift(hdmi, false);
        ctrl_hw_set_i2s_ch_swap(hdmi, 0);
        ctrl_hw_set_i2s_data_dir(hdmi, false);
        ctrl_hw_set_i2s_justify(hdmi, false);
        ctrl_hw_set_i2s_length(hdmi, ctrl_get_audio_bit_reg(attr->aud_sample_size));
        ctrl_hw_set_i2s_vbit(hdmi, compress);
        ctrl_hw_set_i2s_ws_polarity(hdmi, false);

        /* I2S channel status set */
        status.clk_accuracy = AUDIO_CLK_ACCURACY_LEVEL2;
        status.compress = compress;
        status.org_rate = ctrl_get_i2s_rate_org_reg(attr->aud_sample_rate);
        status.out_bit_length = ctrl_get_audio_bit_reg(attr->aud_sample_size);
        status.professional_app = false; /* Consumer use */
        status.sample_rate = ctrl_get_i2s_rate_reg(org_sample_rate);
        ctrl_hw_set_i2s_chn_status(hdmi, &status);
    }

    hal_ctrl_set_audio_ncts(ctrl);
    ctrl_set_aud_infoframe(ctrl);

    return 0;
}

hi_s32 hal_ctrl_set_audio_ncts(struct hdmi_controller *ctrl)
{
    hi_u32 n_value = 0;
    hi_u32 cts_value = 0;
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    if (ctrl->cur_hw_config.work_mode == HDMI_WORK_MODE_TMDS) {
        n_value = ctrl_tmds_recommended_n(ctrl->attr.aud_sample_rate,
                                          ctrl->cur_hw_config.tmds_clock);
        cts_value = ctrl_tmds_recommended_cts(ctrl->attr.aud_sample_rate,
                                              ctrl->cur_hw_config.tmds_clock);
    } else if (ctrl->cur_hw_config.work_mode == HDMI_WORK_MODE_FRL) {
        n_value = ctrl_frl_recommended_n(ctrl->attr.aud_sample_rate,
                                         ctrl->cur_hw_config.cur_frl_rate);
        cts_value = ctrl_frl_recommended_cts(ctrl->attr.aud_sample_rate,
                                             ctrl->cur_hw_config.cur_frl_rate);
    }

    ctrl_hw_set_audio_cts(hdmi, false, cts_value);
    ctrl_hw_set_audio_n(hdmi, n_value);

    return 0;
}

hi_void hal_ctrl_audio_enable(struct hdmi_controller *ctrl, hi_bool enable)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_u32 tmp;
    hi_u32 hw_value;
    hi_u32 sw_value;
    hi_u32 i = 0;

    if (!ctrl) {
        HDMI_ERR("ctrl pointer is null!\n");
        return;
    }

    hdmi = ctrl->parent;
    if (!hdmi) {
        HDMI_ERR("hdmi pointer is null!\n");
        return;
    }

    /*
     * Fix two problems.
     * 1. No audio after wakeup.
     * 2. When audio switch pcm to non pcm, tv has crackle.
     */
    if (!enable) {
        do {
            hw_value = ctrl_hw_get_hw_cts(hdmi);
            sw_value = ctrl_hw_get_sw_cts(hdmi);

            tmp = osal_abs(hw_value - sw_value);
            if (tmp <= 30) { /* Max difference is 30 */
                break;
            }

            osal_msleep(1); /* Sleep 1ms every time */
        } while (i++ < 20); /* Time out is 20 ms */

        HDMI_INFO("i = %d.\n", i);
        ctrl_hw_enable_sw_cts(hdmi, false);
    } else {
        ctrl_hw_enable_sw_cts(hdmi, true);
    }

    if (!enable) {
        ctrl_hw_set_audio_mute(hdmi, ctrl->audio_mute_type, enable);
    }
}

hi_s32 hal_ctrl_set_infoframe(struct hdmi_controller *ctrl)
{
    hi_s32 ret;

    if (ctrl == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    /*
     * Currently int the ES IC, the HDMI2.1 port avi infoframe is sent by the VDP,
     * and the HDMI2.0 port avi infoframe is sent by the HDMI. Subsequent
     * the avi infoframes of all hdmi ports can be sent by the vdp.
     */
    if (!ctrl->parent->is_hdmi21) {
        ret = ctrl_set_avi_infoframe(ctrl);
        if (ret) {
            HDMI_ERR("Send avi infoframe fail(%d).\n", ret);
            return ret;
        }
    }

    ret = ctrl_set_vendor_specific_infoframe(ctrl);
    if (ret) {
        HDMI_ERR("Send vendor specific infoframe fail(%d).\n", ret);
        return ret;
    }

    return ret;
}

hi_s32 hal_ctrl_set_mode(struct hdmi_controller *ctrl)
{
    hi_bool dvi_mode;
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    dvi_mode = ctrl->cur_hw_config.dvi_mode;
    ctrl_hw_set_hdmi_mode(hdmi, !dvi_mode);

    return 0;
}

hi_s32 hal_ctrl_get_dsc_emp(struct hisilicon_hdmi *hdmi, em_data_info *em)
{
    union hdmi_hdmi_extended_metadata em_data;

    if (hdmi == NULL || em == NULL) {
        HDMI_ERR("ptr null err.\n");
        return -EINVAL;
    }

    if (memset_s(&em_data, sizeof(union hdmi_hdmi_extended_metadata),
                 0, sizeof(union hdmi_hdmi_extended_metadata))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    ctrl_hw_read_dsc_emp(hdmi, em->raw_data, sizeof(em->raw_data));
    hdmi_hdmi_any_ex_metadata_unpack(&em_data, em->raw_data, sizeof(em->raw_data));
    em->em_data = em_data.dsc;

    return 0;
}

hi_s32 hal_ctrl_set_dsc_emp(struct hdmi_controller *ctrl, hi_bool enable)
{
    struct hisilicon_hdmi *hdmi = ctrl->parent;
    hi_u8 buffer[192];
    hi_s32 i;
    struct hdmi_hdmi_dsc_ext_metadata em_data;
    struct dsc_cfg_s *dsc_cfg = &ctrl->dsc->cfg_s;
    struct dsc_timing_info *timing_info = &ctrl->dsc->timing_info;
    ssize_t err;

    err = hdmi_hdmi_dsc_ext_metadata_init(&em_data);
    if (err < 0) {
        HDMI_ERR("Failed to setup dsc extended metadata: %zd\n", err);
        return err;
    }

    em_data.pkt_new = enable ? 1 : 0;
    em_data.pkt_end = enable ? 0 : 1;
    em_data.dsc_version_major = 1;
    em_data.dsc_version_minor = dsc_cfg->dsc_version_minor;
    em_data.pps_identifier = dsc_cfg->pps_identifier;
    em_data.bits_per_component = dsc_cfg->bits_per_component;
    em_data.linebuf_depth = dsc_cfg->linebuf_depth;
    em_data.block_pred_enable = dsc_cfg->block_pred_enable;
    em_data.convert_rgb = dsc_cfg->convert_rgb;
    em_data.simple_422 = dsc_cfg->simple_422;
    em_data.vbr_enable = dsc_cfg->vbr_enable;
    em_data.bits_per_pixel = dsc_cfg->bits_per_pixel;
    em_data.pic_height = dsc_cfg->pic_height;
    em_data.pic_width = dsc_cfg->pic_width;
    em_data.slice_height = dsc_cfg->slice_height;
    em_data.slice_width = dsc_cfg->slice_width;
    em_data.chunk_size = dsc_cfg->chunk_size;
    em_data.initial_xmit_delay = dsc_cfg->initial_xmit_delay;
    em_data.initial_dec_delay = dsc_cfg->initial_dec_delay;
    em_data.initial_scale_value = dsc_cfg->initial_scale_value;
    em_data.scale_increment_interval = dsc_cfg->scale_increment_interval;
    em_data.scale_decrement_interval = dsc_cfg->scale_decrement_interval;
    em_data.first_line_bpg_offset = dsc_cfg->first_line_bpg_ofs;
    em_data.nfl_bpg_offset = dsc_cfg->nfl_bpg_offset;
    em_data.slice_bpg_offset = dsc_cfg->slice_bpg_offset;
    em_data.inital_offset = dsc_cfg->initial_offset;
    em_data.final_offset = dsc_cfg->final_offset;
    em_data.flatness_min_qp = dsc_cfg->flatness_min_qp;
    em_data.flatness_max_qp = dsc_cfg->flatness_max_qp;
    em_data.rc_model_size = dsc_cfg->rc_model_size;
    em_data.rc_edge_factor = dsc_cfg->rc_edge_factor;
    em_data.rc_quant_incr_limit0 = dsc_cfg->rc_quant_incr_limit0;
    em_data.rc_quant_incr_limit1 = dsc_cfg->rc_quant_incr_limit1;
    em_data.rc_tgt_offset_hi = dsc_cfg->rc_tgt_offset_hi;
    em_data.rc_tgt_offset_lo = dsc_cfg->rc_tgt_offset_lo;
    for (i = 0; i < NUM_BUF_RANGES - 1; i++) {
        em_data.rc_buf_thresh[i] = dsc_cfg->rc_buf_thresh[i];
    }

    for (i = 0; i < NUM_BUF_RANGES; i++) {
        em_data.rc_parameters[i].range_min_qp =
            dsc_cfg->rc_range_parameters[i].range_min_qp;
        em_data.rc_parameters[i].range_max_qp =
            dsc_cfg->rc_range_parameters[i].range_max_qp;
        em_data.rc_parameters[i].range_bpg_offset =
            dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    }
    em_data.native_420 = dsc_cfg->native_420;
    em_data.native_422 = dsc_cfg->native_422;
    em_data.second_line_bpg_offset = dsc_cfg->second_line_bpg_ofs;
    em_data.nsl_bpg_offset = dsc_cfg->nsl_bpg_offset;
    em_data.second_line_offset_adj = dsc_cfg->second_line_ofs_adj;

    em_data.hfront = ctrl->mode.timing_data.in.detail.hfront;
    em_data.hsync = ctrl->mode.timing_data.in.detail.hsync;
    em_data.hback = ctrl->mode.timing_data.in.detail.hback;
    em_data.hcactive = timing_info->hcactive * 3;

    err = hdmi_hdmi_dsc_ext_metadata_pack(&em_data, buffer, sizeof(buffer));
    if (err < 0) {
        HDMI_ERR("Failed to pack dsc extended metadata: %zd\n", err);
        return err;
    }

    ctrl_hw_send_dsc_emp(hdmi, buffer, sizeof(buffer), enable);

    return 0;
}

hi_void hal_ctrl_set_avmute(struct hdmi_controller *ctrl, hi_bool enable)
{
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    if (enable) {
        ctrl_hw_send_av_mute(hdmi);
    } else {
        ctrl_hw_send_av_unmute(hdmi);
    }

    return;
}

static hi_void ctrl_set_avmute_wait_time(struct hisilicon_hdmi *hdmi, struct debug_avmute *avmute_param)
{
    struct hdmi_debug_info *debug_info = hdmi->debug_info;

    debug_info->d_avmute.wait_bef_stop = avmute_param->wait_bef_stop;
    debug_info->d_avmute.wait_aft_start = avmute_param->wait_aft_start;
}

hi_void hal_ctrl_pwd_soft_reset(struct hdmi_controller *ctrl)
{
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    ctrl_hw_pwd_reset(hdmi);
}

hi_void hal_ctrl_fast_reset(struct hdmi_controller *ctrl)
{
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    ctrl_hw_fast_reset(hdmi);
}

hi_s32 hal_ctrl_tmds_set_scramble(struct hdmi_controller *ctrl)
{
    struct hisilicon_hdmi *hdmi = HI_NULL;
    struct hdmi_connector *connector = HI_NULL;

    if (ctrl == HI_NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return HI_FAILURE;
    }

    hdmi = ctrl->parent;
    connector = hdmi->connector;

    /*
     * When the timing is switched, the scrambling is disabled firstly.
     * For example, the 4K TV is played, and the HDMI cable is removed
     * and then inserted into a non-4K TV. As a result, the scrambling
     * is not disabled, and no output is displayed on the TV. So must
     * be disable scramble firstly.
     */
    ctrl_hw_enable_scramble(hdmi, false);

    /*
     * When you operate SCDC, check whether the RXs support. Avoid the
     * crash caused by the SCDC that some RXs does not support the SCDC
     * operation. Force output just for debug.
     */
    if (!connector->scdc.present && !ctrl->force_output) {
        return HI_FAILURE;
    }

    if (ctrl->cur_hw_config.tmds_scr_en &&
        ctrl->cur_hw_config.work_mode == HDMI_WORK_MODE_TMDS) {
        hi_hdmi_scdc_set_high_tmds_clock_ratio(hdmi->ddc, HI_TRUE);
        hi_hdmi_scdc_set_scrambling(hdmi->ddc, HI_TRUE);
        ctrl_hw_enable_scramble(hdmi, true);
    } else {
        hi_hdmi_scdc_set_high_tmds_clock_ratio(hdmi->ddc, HI_FALSE);
        hi_hdmi_scdc_set_scrambling(hdmi->ddc, HI_FALSE);
    }
    HDMI_INFO("sink scramble status:%d\n", hi_hdmi_scdc_get_scrambling_status(hdmi->ddc));

    return HI_SUCCESS;
}

hi_bool hal_ctrl_tmds_clk_is_stable(struct hdmi_controller *ctrl)
{
    struct hisilicon_hdmi *hdmi = ctrl->parent;

    return ctrl_hw_tmds_clk_is_stable(hdmi);
}

hi_void hal_ctrl_controller_deinit(struct hdmi_controller *ctrl)
{
    if (ctrl) {
        if (ctrl->dsc) {
            hi_hdmi_dsc_deinit(ctrl->dsc);
        }

        if (ctrl->frl) {
            hitxv300_frl_exit(ctrl->frl);
        }

        osal_kfree(HI_ID_HDMITX, ctrl);
    }
}

static hi_u32 ctrl_hw_get_out_color_depth(struct hisilicon_hdmi *hdmi)
{
    hi_u32 data;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return 0;
    }

    data = hdmi_readl(hdmi->hdmi_regs, REG_AVMIXER_CONFIG);
    if (data & REG_DC_PKT_EN_M) {
        data = hdmi_readl(hdmi->hdmi_regs, REG_TX_PACK_FIFO_CTRL);
        data &= REG_TMDS_PACK_MODE_M;
        if (data == 3) { /* 3 is 48 bit */
            HDMI_WARN("logic support bpc48?\n");
        }
    } else {
        data = HDMI_BPC_24;
    }

    return data;
}

static hi_bool ctrl_hw_get_dither_en(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return false;
    }

    return !!(hdmi_readl(hdmi->hdmi_regs, REG_VDP_DITHER_CTRL) & REG_DITHER_ENABLE_M);
}

static hi_u32 ctrl_hw_get_in_color_depth(struct hisilicon_hdmi *hdmi)
{
    hi_bool dither_en;
    hi_u32 out_depth;
    hi_u32 in_depth;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return false;
    }

    dither_en = ctrl_hw_get_dither_en(hdmi);
    out_depth = ctrl_hw_get_out_color_depth(hdmi);
    if (out_depth == HDMI_BPC_36) {
        in_depth = HDMI_BPC_36;
    } else if (out_depth == HDMI_BPC_30) {
        in_depth = dither_en ? HDMI_BPC_36 : HDMI_BPC_30;
    } else if (out_depth == HDMI_BPC_24) { /* ingore 12 to 8 dither */
        in_depth = dither_en ? HDMI_BPC_30 : HDMI_BPC_24;
    } else {
        in_depth = HDMI_BPC_30;
    }
    return in_depth;
}

hi_bool hal_ctrl_hw_get_source_scramble(struct hisilicon_hdmi *hdmi)
{
    hi_u32 data;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return false;
    }

    data = hdmi_readl(hdmi->hdmi_regs, REG_HDMI_ENC_CTRL);
    return !!((data & REG_ENC_HDMI2_ON_M) && (data & REG_ENC_SCR_ON_M));
}

hi_void ctrl_hw_set_vic(struct hisilicon_hdmi *hdmi, hi_u32 vic)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_writel(hdmi->hdmi_regs, REG_VIC, vic);
}

hi_u32 hal_ctrl_hw_get_vic(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return 0;
    }

    return hdmi_readl(hdmi->hdmi_regs, REG_VIC);
}

hi_u32 hal_ctrl_hw_get_color_format(struct hisilicon_hdmi *hdmi)
{
    hi_u32 data;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return HDMI_COLOR_FORMAT_YCBCR444;
    }

    data = hdmi_readl(hdmi->hdmi_regs, REG_CSC_DITHER_DFIR_EN);
    if (data & REG_DFIR_420_EN_M) {
        data = HDMI_COLOR_FORMAT_YCBCR420;
    } else if (data & REG_DFIR_422_EN_M) {
        data = HDMI_COLOR_FORMAT_YCBCR422;
    } else if (data & REG_VDP2HDMI_YUV_MODE_M) {
        data = HDMI_COLOR_FORMAT_YCBCR444;
    } else {
        data = HDMI_COLOR_FORMAT_RGB;
    }

    return data;
}

hi_bool hal_ctrl_hw_get_hdmi_mode(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return HI_TRUE;
    }

    return !!(REG_HDMI_MODE_M & hdmi_readl(hdmi->hdmi_regs, REG_AVMIXER_CONFIG));
}

static hi_void ctrl_send_null_packet_en(struct hisilicon_hdmi *hdmi, hi_u8 bit_en)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_AVMIXER_CONFIG, REG_NULL_PKT_EN_M, reg_null_pkt_en(bit_en));
}

hi_void hal_ctrl_hw_get_info_frame(struct hisilicon_hdmi *hdmi,
    hi_s32 type, hi_u8 *buffer, hi_u8 buffer_len)
{
    hi_u32 if_head, data, i, j;

    if (buffer == NULL || buffer_len < HDMI_INFOFRAME_BUF_SIZE) {
        HDMI_ERR("if type=%d,buffer=%p,buffer_len=%d\n", type, buffer, buffer_len);
        return ;
    }

    switch (type) {
        case HDMI_INFOFRAME_TYPE_AVI:
            if_head = REG_AVI_PKT_HEADER;
            break;

        case HDMI_INFOFRAME_TYPE_AUDIO:
            if_head = REG_AIF_PKT_HEADER;
            break;
        case HDMI_INFOFRAME_TYPE_VENDOR:
            if_head = REG_VSIF_PKT_HEADER;
            break;

        case HDMI_INFOFRAME_TYPE_DRM:
        case HDMI_INFOFRAME_TYPE_SPD:
            HDMI_INFO("to do support\n");
            /* fall-through */
        default:
            HDMI_INFO("err type=%d\n", type);
            return ;
    }

    /* head pkt, i is buffer offset */
    i = 0;
    data = hdmi_readl(hdmi->hdmi_regs, if_head);
    buffer[i++] = (data >> 0) & 0xff; /* shift 0 bit, 0xff is byte mask */
    buffer[i++] = (data >> 8) & 0xff; /* shift 8 bit, 0xff is byte mask */
    buffer[i++] = (data >> 16) & 0xff; /* shift 16 bit, 0xff is byte mask */

    /* sub pkt */
    for (j = 0; j < 8 && i < HDMI_INFOFRAME_BUF_SIZE; j++) { /* [0,8) is sub_pkt number */
        data = hdmi_readl(hdmi->hdmi_regs, if_head + (j + 1) * 4); /* offset (1+j)*4 */
        buffer[i++] = (data >> 0) & 0xff;      /* shift 0 bit, 0xff is byte mask */
        buffer[i++] = (data >> 8) & 0xff;      /* shift 8 bit, 0xff is byte mask */
        buffer[i++] = (data >> 16) & 0xff;     /* shift 16 bit, 0xff is byte mask */
        if (((j + 1) % 2) != 0) {             /* (j+1 %)==2 is reserved */
            buffer[i++] = (data >> 24) & 0xff; /* shift 24 bit, 0xff is byte mask */
        }
    }
}

hi_void hal_ctrl_hw_get_detect_timing(struct hisilicon_hdmi *hdmi, struct hw_timing *timing)
{
    hi_u32 data;

    if (hdmi == NULL || timing == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    data = hdmi_readl(hdmi->hdmi_regs, REG_VIDEO_DMUX_CTRL);
    timing->de_polarity = !!(data & REG_DE_M);

    data = hdmi_readl(hdmi->hdmi_regs, REG_FDET_STATUS);
    timing->vsync_polarity = !!(data & REG_VSYNC_POLARITY);
    timing->hsync_polarity = !!(data & REG_HSYNC_POLARITY);
    timing->interlaced = !!(data & REG_INTERLACED);

    data = hdmi_readl(hdmi->hdmi_regs, REG_FDET_HORI_RES);
    timing->hsync_total = (data & REG_HSYNC_TOTAL_CNT_M) >> 16; /* shift 16 bit */
    timing->hsync_active = data & REG_HSYNC_ACTIVE_CNT_M;

    data = hdmi_readl(hdmi->hdmi_regs, REG_FDET_VERT_RES);
    timing->vsync_total = (data & REG_VSYNC_TOTAL_CNT_M) >> 16; /* shift 16 bit */
    timing->vsync_active = data & REG_VSYNC_ACTIVE_CNT_M;
}

static hi_void ctrl_hw_get_detail_timing(struct hdmi_controller *controller)
{
    struct hw_timing timing;
    struct hisilicon_hdmi *hdmi = NULL;
    struct hdmi_timing_mode *timing_mode = NULL;
    struct hdmi_timing_data *timing_data = NULL;

    if (controller == NULL || controller->parent == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi = controller->parent;
    timing_data = &controller->mode.timing_data;

    timing_data->in.color_depth = ctrl_hw_get_in_color_depth(hdmi);
    timing_data->out.color_depth = ctrl_hw_get_out_color_depth(hdmi);
    timing_data->out.color.color_format = hal_ctrl_hw_get_color_format(hdmi);

    hal_ctrl_hw_get_detect_timing(hdmi, &timing);
    timing_data->in.v_sync_pol = timing.vsync_polarity;
    timing_data->in.h_sync_pol = timing.hsync_polarity;
    timing_data->in.de_pol = timing.de_polarity;

    timing_data->in.vic = hal_ctrl_hw_get_vic(hdmi);
    timing_data->in.detail.hactive = timing.hsync_active;
    timing_data->in.detail.htotal = timing.hsync_total;
    timing_data->in.detail.vtotal = timing.vsync_total;
    timing_data->in.detail.vactive = timing.vsync_active;

    timing_mode = vic2timing_mode(timing_data->in.vic);
    if (timing_mode == NULL) {
        HDMI_ERR("err vic=%d\n", timing_data->in.vic);
        return ;
    }
    timing_data->in.pixel_clock = timing_mode->pixel_clock;
    timing_data->in.detail.hback = timing_mode->h_back;
    timing_data->in.detail.hsync = timing_mode->h_sync;
    timing_data->in.detail.hfront = timing_mode->h_front;
    timing_data->in.detail.vback = timing_mode->v_back;
    timing_data->in.detail.vsync = timing_mode->v_sync;
    timing_data->in.detail.vfront = timing_mode->v_front;
    timing_data->in.detail.refresh_rate = timing_mode->field_rate;
}

hi_void hal_ctrl_hw_get_display_mode(struct hdmi_controller *controller)
{
    struct hdmi_timing_data *timing_data = NULL;
    struct hisilicon_hdmi *hdmi = NULL;
    union hdmi_infoframe frame;
    hi_u8 buffer[HDMI_INFOFRAME_BUF_SIZE];
    hi_s32 ret;

    if (controller == NULL || controller->parent == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi = controller->parent;
    timing_data = &controller->mode.timing_data;

    hal_ctrl_hw_get_info_frame(hdmi, HDMI_INFOFRAME_TYPE_AVI, buffer, sizeof(buffer));
    ret = hdmi_infoframe_unpack(&frame, buffer, sizeof(buffer));
    if (ret != 0) {
        HDMI_ERR("unpack avi fail!\n");
        return ;
    }

    ctrl_hw_get_detail_timing(controller);
    timing_data->out.color.colorimetry = frame.avi.colorimetry;
    if (frame.avi.colorimetry == HDMI_COLORIMETRY_EXTENDED) {
        timing_data->out.color.colorimetry =
            frame.avi.extended_colorimetry  | HDMI_COLORIMETRY_EXTENDED;
    }
    timing_data->out.color.rgb_quantization = frame.avi.quantization_range;
    timing_data->out.color.ycc_quantization = frame.avi.ycc_quantization_range;

    timing_data->in.scan_info = frame.avi.scan_mode;
    timing_data->in.active_aspect_present = true; /* software force */
    timing_data->in.bar_present = true; /* software force */
    timing_data->in.picture_aspect_ratio = frame.avi.picture_aspect;
    timing_data->in.active_aspect_ratio = frame.avi.active_aspect;
    timing_data->in.picture_scal = frame.avi.nups;
    timing_data->in.it_content_valid = frame.avi.itc;
    timing_data->in.it_content_type = frame.avi.content_type;
    timing_data->in.pixel_repeat = frame.avi.pixel_repeat;
    timing_data->in.top_bar = frame.avi.top_bar;
    timing_data->in.bottom_bar = frame.avi.bottom_bar;
    timing_data->in.left_bar = frame.avi.left_bar;
    timing_data->in.right_bar = frame.avi.right_bar;

    timing_data->in.mode_3d = HDMI_3D_NONE;
    hal_ctrl_hw_get_info_frame(hdmi, HDMI_INFOFRAME_TYPE_VENDOR, buffer, sizeof(buffer));
    ret = hdmi_infoframe_unpack(&frame, buffer, sizeof(buffer));
    if (ret == 0 && frame.vendor.hdmi_forum.oui == HDMI_IEEE_OUI &&
        frame.vendor.hdmi.s3d_struct != HDMI_3D_STRUCTURE_INVALID) {
        timing_data->in.mode_3d = frame.vendor.hdmi.s3d_struct;
    }
}

static hi_void clr_bar_set(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_PATTERN_GEN_CTRL,
                REG_COLORBAR_EN_M, reg_colorbar_en(enable));

    hdmi_clrset(hdmi->hdmi_regs, REG_PATTERN_GEN_CTRL,
                REG_TPG_ENABLE_M, reg_tpg_enable(enable));
}

hi_s32 hal_ctrl_get_audio_path(struct hisilicon_hdmi *hdmi, audio_path *audio_path_info)
{
    ncts_val ncts;
    struct ctrl_i2s_status status;

    if (hdmi == NULL || audio_path_info == NULL) {
        HDMI_ERR("ptr null err.\n");
        return -EINVAL;
    }

    ctrl_hw_get_audio_ncts(hdmi, &ncts);
    ctrl_hw_get_i2s_chn_status(hdmi, &status);

    audio_path_info->input_type = ctrl_hw_get_audio_intf(hdmi);
    audio_path_info->input_len = ctrl_hw_get_i2s_length(hdmi);
    audio_path_info->is_pcm_compress = ctrl_hw_get_i2s_vbit(hdmi);
    audio_path_info->out_bit_len = status.out_bit_length;
    audio_path_info->sample_rate_num = get_sample_rate_val(status.org_rate, status.sample_rate);
    audio_path_info->chn_num = ctrl_hw_get_audio_layout(hdmi);
    audio_path_info->reg_n = ncts.n_value;
    audio_path_info->reg_cts = ncts.cts_value;

    return 0;
}

hi_s32 hal_ctrl_get_video_path(struct hisilicon_hdmi *hdmi, video_path *video_path_info)
{
    if (hdmi == NULL || video_path_info == NULL) {
        HDMI_ERR("ptr null err.\n");
        return -EINVAL;
    }

    video_path_info->out_color_fmt = hal_ctrl_hw_get_color_format(hdmi);
    video_path_info->in_color_depth = ctrl_hw_get_in_color_depth(hdmi);
    video_path_info->out_color_depth = ctrl_hw_get_out_color_depth(hdmi);

    return 0;
}

static hi_void ctrl_set_de(struct hisilicon_hdmi *hdmi, hi_u8 de_en, hi_u8 period)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_PKT_WIN_DE_CFG, REG_CFG_DE_POST_CNT_M, reg_cfg_de_post_cnt(period));
    hdmi_clrset(hdmi->hdmi_regs, REG_PKT_WIN_DE_CFG, REG_CFG_DE_POST_OVR_M, reg_cfg_de_post_ovr(de_en));
}

static hi_void ctrl_set_rc(struct hisilicon_hdmi *hdmi, hi_u8 rc_val)
{
    hdmi_clrset(hdmi->hdmi_regs, REG_CTL_TYPE_CONFIG, REG_TYPE_CONFIG_M, reg_type_config(rc_val));
}
static hi_void ctrl_debug_set_avmute(struct hisilicon_hdmi *hdmi, hi_u32 sub_cmd, struct debug_avmute *avmute_param)
{
    switch (sub_cmd) {
        case CMD_NUM_0:
            hal_ctrl_set_avmute(hdmi->ctrl, HI_FALSE);
            break;
        case CMD_NUM_1:
            hal_ctrl_set_avmute(hdmi->ctrl, HI_TRUE);
            break;
        case CMD_NUM_2:
        case CMD_NUM_3:
            ctrl_set_avmute_wait_time(hdmi, avmute_param);
            break;
        default:
            HDMI_ERR("ctrl cmd invalid!\n");
    }
}

hi_s32 hal_ctrl_debug(struct hisilicon_hdmi *hdmi, enum debug_ext_cmd_list cmd, struct hdmitx_debug_msg msg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 sub_cmd = msg.sub_cmd;
    void *data = msg.data;

    if (hdmi == HI_NULL || data == HI_NULL) {
        HDMI_ERR("The poniter is NULL!\n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case DEBUG_CMD_AVMUTE:
            ctrl_debug_set_avmute(hdmi, sub_cmd, data);
            break;
        case DEBUG_CMD_CBAR:
            clr_bar_set(hdmi, *(hi_bool *)data);
            break;
        case DEBUG_CMD_SCDC:
            if (msg.sub_cmd == CMD_NUM_0 || msg.sub_cmd == CMD_NUM_1) {
                hal_ctrl_tmds_set_scramble((struct hdmi_controller *)data);
            } else if (msg.sub_cmd == CMD_NUM_2) {
                return hal_ctrl_hw_get_source_scramble(hdmi);
            } else {
                return HI_FAILURE;
            }
            break;
        case DEBUG_CMD_HDMI_MODE:
            ctrl_hw_set_hdmi_mode(hdmi, *((hi_bool *)data));
            break;
        case DEBUG_CMD_RC:
            ctrl_set_rc(hdmi, *(hi_u8 *)data);
            break;
        case DEBUG_CMD_DE:
            ctrl_set_de(hdmi, sub_cmd, *(hi_u8 *)data);
            break;
        case DEBUG_CMD_NULL_PACKET:
            ctrl_send_null_packet_en(hdmi, *(hi_u8 *)data);
            break;
        default :
            HDMI_ERR("ctrl cmd invalid!\n");
    }

    return ret;
}
