/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HI_HAL_AIAO_COMMON_H__
#define __HI_HAL_AIAO_COMMON_H__

#include "hi_drv_audio.h"

#include "circ_buf.h"

#define AIAO_ASSERT(x, param1, param2)
#define AIAO_ASSERT_NULL(ptr)

#ifdef AIAO_SW_SIMULAUTE
#define AIAO_IRQ_NUM 79
#else
#define ENA_AIAO_ISR
#ifndef HI_FPGA
#define AIAO_IRQ_NUM (84 + 32) /* s5 asic */
#else
#define AIAO_IRQ_NUM (18 + 17 + 32) /* s5 fpga */
#endif
#endif

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#ifdef HI_FPGA_SUPPORT
#define AIAO_PLL_12MHZ
#else
#define AIAO_PLL_786MHZ
#endif
#else
#error "YOU MUST DEFINE CHIP_TYPE!"
#endif

#define AIAO_SYSCRG_REGOFFSET  0x0118

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define AIAO_CBB_REGBASE  0xea0000
#else
#define AIAO_CBB_REGBASE  0xf8cd0000
#endif

#define AIAO_COM_OFFSET 0x0000
#define AIAO_CRG_OFFSET 0x0100
#define AIAO_RX_OFFSET 0x1000
#define AIAO_TX_OFFSET 0x2000
#define AIAO_TXSDPIF_OFFSET 0x3000
#define AIAO_SDPIFER_OFFSET 0x4000

#define AIAO_CBB_REGBANDSIZE 0x10000
#define AIAO_COMREG_BANDSIZE 0x200
#define AIAO_RX_REG_BANDSIZE 0x100
#define AIAO_TX_REG_BANDSIZE 0x100
#define AIAO_TXSPDIF_REG_BANDSIZE 0x100
#define AIAO_SPDIFER_REG_BANDSIZE 0x1000 /* special */

/* hardware board i2s port */
#define AIAO_MAX_EXT_I2S_NUMBER  2  /* rx/tx */
#define AIAO_MAX_INT_I2S_NUMBER  3  /* rx/tx */

#define AIAO_MAX_PORT_NUMBER  8

/* AIAO TX/RX port */
#define AIAO_MAX_RX_PORT_NUMBER  8
#define AIAO_MAX_TX_PORT_NUMBER  8
#define AIAO_MAX_TXSPDIF_PORT_NUMBER  4

/* AIAO TX/RX direction */
#define AIAO_VOL_6DB  0x7f
#define AIAO_VOL_0DB  0x79
#define AIAO_VOL_INF_DB  0x28
#define AIAO_VOL_MAX_DB (AIAO_VOL_6DB)
#define AIAO_VOL_MIN_DB (AIAO_VOL_INF_DB)

/* AIAO TX/RX direction */
#define AIAO_DIR_RX  0
#define AIAO_DIR_TX  1

/* CRG TX/RX direction */
#define CRG_DIR_RX  0
#define CRG_DIR_TX  1

/* AIAO TX TYPE */
#define AIAO_TX_I2S  0
#define AIAO_TX_SPDIF  1

/* AIAO TX/RX stop/start */
#define AIAO_STOP  0
#define AIAO_START 1

/* AIAO TX/RX unit */
#define AIAO_FIFO_UNITBYTES (256 / 8) /* aiao min process unit */

/* AIAO TX/RX buffer */
#define AIAO_BUFFER_ADDR_ALIGN  128
#define AIAO_BUFFER_SIZE_ALIGN  128
#define AIAO_BUFFER_SIZE_MAX ((0x1L << 24) - AIAO_BUFFER_ADDR_ALIGN)
#define AIAO_BUFFER_SIZE_MIN (AIAO_BUFFER_SIZE_ALIGN)
#define AIAO_BUFFER_ALEMPTY_SIZE_ALIGN (AIAO_BUFFER_SIZE_ALIGN)
#define AIAO_BUFFER_ALEMPTY_SIZE_MIN (AIAO_BUFFER_SIZE_ALIGN)
#define AIAO_BUFFER_ALFULL_SIZE_ALIGN (AIAO_BUFFER_SIZE_ALIGN)
#define AIAO_BUFFER_ALFULL_SIZE_MIN (AIAO_BUFFER_SIZE_ALIGN)

#define AIAO_BUFFER_PERIODNUM_MIN  2
#define AIAO_BUFFER_PERIODNUM_MAX  512
#define AIAO_BUFFER_PERIODSIZE_MIN (AIAO_BUFFER_SIZE_ALIGN)
#define AIAO_BUFFER_PERIODSIZE_MAX (AIAO_BUFFER_SIZE_MAX / AIAO_BUFFER_PERIODNUM_MAX)

#define aiao_df_period_buf_size 2048

#define aiao_df_fade_in_rate AIAO_FADE_RATE_16
#define aiao_df_fade_out_rate AIAO_FADE_RATE_8

#define AIAO_MIN_DATA_FORDMA_INBYTES 32

/* AIAO MAX TX/RX port definition */
typedef enum {
    AIAO_PORT_RX0 = 0x00,
    AIAO_PORT_RX1 = 0x01,
    AIAO_PORT_RX2 = 0x02,
    AIAO_PORT_RX3 = 0x03,
    AIAO_PORT_RX4 = 0x04,
    AIAO_PORT_RX5 = 0x05,
    AIAO_PORT_RX6 = 0x06,
    AIAO_PORT_RX7 = 0x07,

    AIAO_PORT_TX0 = 0x10,
    AIAO_PORT_TX1 = 0x11,
    AIAO_PORT_TX2 = 0x12,
    AIAO_PORT_TX3 = 0x13,
    AIAO_PORT_TX4 = 0x14,
    AIAO_PORT_TX5 = 0x15,
    AIAO_PORT_TX6 = 0x16,
    AIAO_PORT_TX7 = 0x17,

    AIAO_PORT_SPDIF_TX0 = 0x20,
    AIAO_PORT_SPDIF_TX1 = 0x21,
    AIAO_PORT_SPDIF_TX2 = 0x22,
    AIAO_PORT_SPDIF_TX3 = 0x23,

    AIAO_PORT_MAX = 0xff,
} aiao_port_id;

typedef enum {
    AIAO_MODE_RXI2S = 0,
    AIAO_MODE_TXI2S,
    AIAO_MODE_TXSPDIF,
    AIAO_MODE_PORT_MAX,
} aiao_port_mode;

typedef enum {
    AIAO_SPDIFPORT_SOURCE_TX0 = 0,
    AIAO_SPDIFPORT_SOURCE_TX1 = 1,
    AIAO_SPDIFPORT_SOURCE_TX2 = 2,
    AIAO_SPDIFPORT_SOURCE_TX3 = 3,
    AIAO_SPDIFPORT_SOURCE_RX0 = 4,
    AIAO_SPDIFPORT_SOURCE_RX1 = 5,
} aiao_spdifport_source;

typedef enum {
    AIAO_RX_CRG0 = 0,
    AIAO_RX_CRG1,
    AIAO_RX_CRG2,
    AIAO_RX_CRG3,
    AIAO_RX_CRG4,
    AIAO_RX_CRG5,
    AIAO_RX_CRG6,
    AIAO_RX_CRG7,
    AIAO_TX_CRG0,
    AIAO_TX_CRG1,
    AIAO_TX_CRG2,
    AIAO_TX_CRG3,
    AIAO_TX_CRG4,
    AIAO_TX_CRG5,
    AIAO_TX_CRG6,
    AIAO_TX_CRG7,
    AIAO_CRG_MAX = 0xffff,
} aiao_crg_source;

typedef enum {
    AIAO_CRG_MODE_MASTER = 0,
    AIAO_CRG_MODE_SLAVE,
    AIAO_CRG_MODE_DUPLICATE,
} aiao_crg_mode;

typedef enum {
    AIAO_TX0 = 0,
    AIAO_TX1,
    AIAO_TX2,
    AIAO_TX3,
    AIAO_TX4,
    AIAO_TX5,
    AIAO_TX6,
    AIAO_TX7,
    AIAO_RX0,
    AIAO_RX1,
    AIAO_RX2,
    AIAO_RX3,
    AIAO_RX4,
    AIAO_RX5,
    AIAO_RX6,
    AIAO_RX7,
} aiao_i2s_source;

typedef enum {
    AIAO_I2S_SD0 = 0,
    AIAO_I2S_SD1,
    AIAO_I2S_SD2,
    AIAO_I2S_SD3,
} aiao_i2s_sd;

typedef enum {
    AIAO_BIT_DEPTH_8 = 8,
    AIAO_BIT_DEPTH_16 = 16,
    AIAO_BIT_DEPTH_24 = 24,
} aiao_bit_depth;

typedef enum {
    AIAO_I2S_CHNUM_1 = 1,
    AIAO_I2S_CHNUM_2 = 2,
    AIAO_I2S_CHNUM_4 = 4,
    AIAO_I2S_CHNUM_8 = 8,
    AIAO_I2S_CHNUM_16 = 16,
} aiao_i2s_chnum;

typedef enum {
    AIAO_MODE_I2S = 0,
    AIAO_MODE_PCM = 1,

    AIAO_MODE_MAX
} aiao_i2s_mode;

typedef enum {
    AIAO_MODE_EDGE_RISE = 1,
    AIAO_MODE_EDGE_FALL = 0,
} aiao_i2s_edge;

typedef enum {
    AIAO_TRACK_MODE_STEREO = 0,
    AIAO_TRACK_MODE_DOUBLE_LEFT,
    AIAO_TRACK_MODE_DOUBLE_RIGHT,
    AIAO_TRACK_MODE_EXCHANGE,
    AIAO_TRACK_MODE_DOUBLE_MONO,
    AIAO_TRACK_MODE_ONLY_RIGHT,
    AIAO_TRACK_MODE_ONLY_LEFT,
    AIAO_TRACK_MODE_MUTED,

    AIAO_TRACK_MODE_MAX
} aiao_track_mode;
typedef enum {
    AIAO_FADE_RATE_1 = 0,
    AIAO_FADE_RATE_2 = 1,
    AIAO_FADE_RATE_4 = 2,
    AIAO_FADE_RATE_8 = 3,
    AIAO_FADE_RATE_16 = 4,
    AIAO_FADE_RATE_32 = 5,
    AIAO_FADE_RATE_64 = 6,
    AIAO_FADE_RATE_128 = 7,
} aiao_fade_rate;

typedef enum {
    AIAO_RX_INT0 = 0,
    AIAO_RX_INT1,
    AIAO_RX_INT2,
    AIAO_RX_INT3,
    AIAO_RX_INT4,
    AIAO_RX_INT5,
    AIAO_RX_INT6,
    AIAO_RX_INT7,

    AIAO_TX_INT0 = 16,
    AIAO_TX_INT1,
    AIAO_TX_INT2,
    AIAO_TX_INT3,
    AIAO_TX_INT4,
    AIAO_TX_INT5,
    AIAO_TX_INT6,
    AIAO_TX_INT7,

    AIAO_SPDIFTX_INT0 = 24,
    AIAO_SPDIFTX_INT1,
    AIAO_SPDIFTX_INT2,
#ifdef HI_AIAO_TIMER_SUPPORT
    AIAO_TIMER_INT0,
    AIAO_TIMER_INT1,
#else
    AIAO_SPDIFTX_INT3,
#endif
    AIAO_INT_MAX = 32
} aiao_top_int;

typedef enum {
    AIAO_TXINT0_BUF_TRANSFINISH = 0,
    AIAO_TXINT1_BUF_EMPTY = 1,
    AIAO_TXINT2_BUF_AEMPTY = 2,
    AIAO_TXINT3_BUF_FIFOEMPTY = 3,
    AIAO_TXINT4_IF_FIFOEMPTY = 4,
    AIAO_TXINT5_STOP_DONE = 5,
    AIAO_TXINT6_MUTEFADE_DONE = 6,
    AIAO_TXINT7_DATA_BROKEN = 7,
    AIAO_TXINT_ALL = 0xff,
} aiao_tx_intmode;

#define AIAO_TXINT0_BUF_TRANSFINISH_MASK (1 << ((hi_u32)AIAO_TXINT0_BUF_TRANSFINISH))
#define AIAO_TXINT1_BUF_EMPTY_MASK (1 << ((hi_u32)AIAO_TXINT1_BUF_EMPTY))
#define AIAO_TXINT2_BUF_AEMPTY_MASK (1 << ((hi_u32)AIAO_TXINT2_BUF_AEMPTY))
#define AIAO_TXINT3_BUF_FIFOEMPTY_MASK (1 << ((hi_u32)AIAO_TXINT3_BUF_FIFOEMPTY))
#define AIAO_TXINT4_IF_FIFOEMPTY_MASK (1 << ((hi_u32)AIAO_TXINT4_IF_FIFOEMPTY))
#define AIAO_TXINT5_STOP_DONE_MASK (1 << ((hi_u32)AIAO_TXINT5_STOP_DONE))
#define AIAO_TXINT6_MUTEFADE_DONE_MASK (1 << ((hi_u32)AIAO_TXINT6_MUTEFADE_DONE))
#define AIAO_TXINT7_DATA_BROKEN_MASK (1 << ((hi_u32)AIAO_TXINT7_DATA_BROKEN))

typedef enum {
    AIAO_RXINT0_BUF_TRANSFINISH = 0,
    AIAO_RXINT1_BUF_FULL = 1,
    AIAO_RXINT2_BUF_AFULL = 2,
    AIAO_RXINT3_BUF_FIFOFULL = 3,
    AIAO_RXINT4_IF_FIFOFULL = 4,
    AIAO_RXINT5_STOP_DONE = 5,
    AIAO_RXINT_ALL = 0x3f,
} aiao_rx_intmode;

#define AIAO_RXINT0_BUF_TRANSFINISH_MASK (1 << ((hi_u32)AIAO_RXINT0_BUF_TRANSFINISH))
#define AIAO_RXINT1_BUF_FULL_MASK (1 << ((hi_u32)AIAO_RXINT1_BUF_FULL))
#define AIAO_RXINT2_BUF_AFULL_MASK (1 << ((hi_u32)AIAO_RXINT2_BUF_AFULL))
#define AIAO_RXINT3_BUF_FIFOFULL_MASK (1 << ((hi_u32)AIAO_RXINT3_BUF_FIFOFULL))
#define AIAO_RXINT4_IF_FIFOFULL_MASK (1 << ((hi_u32)AIAO_RXINT4_IF_FIFOFULL))
#define AIAO_RXINT5_STOP_DONE_MASK (1 << ((hi_u32)AIAO_RXINT5_STOP_DONE))

typedef enum {
    AIAO_STOP_IMMEDIATE = 0,
    AIAO_STOP_FADEOUT = 1,
} aiao_port_stopmode;

typedef enum {
    AIAO_SAMPLE_RATE_UNKNOWN = 0,
    AIAO_SAMPLE_RATE_8K = 8000,
    AIAO_SAMPLE_RATE_11K = 11025,
    AIAO_SAMPLE_RATE_12K = 12000,
    AIAO_SAMPLE_RATE_16K = 16000,
    AIAO_SAMPLE_RATE_22K = 22050,
    AIAO_SAMPLE_RATE_24K = 24000,
    AIAO_SAMPLE_RATE_32K = 32000,
    AIAO_SAMPLE_RATE_44K = 44100,
    AIAO_SAMPLE_RATE_48K = 48000,
    AIAO_SAMPLE_RATE_88K = 88200,
    AIAO_SAMPLE_RATE_96K = 96000,
    AIAO_SAMPLE_RATE_176K = 176400,
    AIAO_SAMPLE_RATE_192K = 192000,
    AIAO_SAMPLE_RATE_288K = 288000,

    AIAO_SAMPLE_RATE_MAX
} aiao_sample_rate;

/* SPDIF output mode */
typedef enum {
    AIAO_SPDIF_MODE_PCM = 0,    /* linear pcm */
    AIAO_SPDIF_MODE_COMPRESSED, /* non linear pcm(bit stream) */
    AIAO_SPDIF_MODE_MAX
} aiao_spdif_mode;

/* SPDIF SCMS mode setting, use in spdif_set_scms */
typedef enum {
    AIAO_SPDIF_SCMS_COPYALLOW,  /* SPDIF SCMS mode is copy allow */
    AIAO_SPDIF_SCMS_COPYONCE,   /* SPDIF SCMS mode is copy once */
    AIAO_SPDIF_SCMS_COPYNOMORE, /* SPDIF SCMS mode is copy no more */
    AIAO_SPDIF_SCMS_COPYDEFY,   /* SPDIF SCMS mode is copy prohibited */
    AIAO_SPDIF_SCMS_MAX
} aiao_spdif_scms_mode;

typedef enum {
    AIAO_SPDIF_CATEGORY_GENERAL = 0x00,       /* general */
    AIAO_SPDIF_CATEGORY_BROADCAST_JP = 0x10,  /* japan */
    AIAO_SPDIF_CATEGORY_BROADCAST_USA,        /* united states */
    AIAO_SPDIF_CATEGORY_BROADCAST_EU,         /* europe */
    AIAO_SPDIF_CATEGORY_PCM_CODEC = 0x20,     /* PCM encoder/decoder */
    AIAO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER,   /* digital sound sampler */
    AIAO_SPDIF_CATEGORY_DIGITAL_MIXER,        /* digital signal mixer */
    AIAO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR, /* digital sound processor */
    AIAO_SPDIF_CATEGORY_SRC,                  /* sample rate converter */
    AIAO_SPDIF_CATEGORY_MD = 0x30,            /* mini_disc */
    AIAO_SPDIF_CATEGORY_DVD,                  /* digital versatile disc */
    AIAO_SPDIF_CATEGORY_SYNTHESISER = 0x40,   /* synthesiser */
    AIAO_SPDIF_CATEGORY_MIC,                  /* microphone */
    AIAO_SPDIF_CATEGORY_DAT = 0x50,           /* digital audio tape */
    AIAO_SPDIF_CATEGORY_DCC,                  /* digital compact cassette */
    AIAO_SPDIF_CATEGORY_VCR,                  /* video cassette recorder */
    AIAO_SPDIF_CATEGORY_MAX
} aiao_spdif_categorycode;
typedef hi_void aiao_isr_func(aiao_port_id port_id, hi_u32 int_raw_status, hi_void *);

#ifdef HI_AIAO_TIMER_SUPPORT
typedef enum {
    AIAO_TIMER_0 = 0,
    AIAO_TIMER_1,
    AIAO_TIMER_MAX,
} aiao_timer_id;

typedef hi_void aiao_timer_isr_func(aiao_timer_id timer_id, hi_void *);
#endif

typedef struct {
    hi_u32 period_buf_size;
    hi_u32 period_number; /* 2/4/8 */
} aiao_buf_attr;

typedef struct {
    hi_u32 buff_saddr;
    hi_u32 buff_size;
    hi_u32 buff_wptr;
    hi_u32 buff_rptr;
    hi_u32 period_buf_size;
    hi_u32 threshold_size;
} aiao_buf_info;

typedef struct {
    /* common */
    aiao_crg_mode crg_mode;
    aiao_i2s_chnum ch_num;
    aiao_bit_depth bit_depth;

    /* i2s AIAO_CRG_MODE_MASTER & AIAO_CRG_MODE_SLAVE only */
    aiao_i2s_edge rise_edge;

    /* i2s/spdif AIAO_CRG_MODE_MASTER only */
    aiao_sample_rate rate;
    hi_u32 fclk_div;
    hi_u32 bclk_div;

    /* i2s AIAO_CRG_MODE_DUPLICATE only */
    aiao_crg_source crg_source;

    /* i2s rx/tx only */
    hi_u32 pcm_delay_cycles;
    aiao_i2s_mode i2s_mode;
    aiao_i2s_source source;
    aiao_i2s_sd sd0;
    aiao_i2s_sd sd1;
    aiao_i2s_sd sd2;
    aiao_i2s_sd sd3;

    /* i2s rx only */
    hi_s32 multislot;
} aiao_if_attr;

/* void internal mmz */
typedef struct {
    hi_u64 buf_phy_addr;
    hi_u64 buf_vir_addr;
    hi_u32 buf_size;
} aiao_mem_attr;

typedef enum {
    SND_OP_TYPE_SPDIF = 0, /* spdif */
    SND_OP_TYPE_I2S = 1,   /* tx */
    SND_OP_TYPE_MAX
} aiao_op_type;

typedef struct {
    aiao_if_attr if_attr;
    aiao_buf_attr buf_config;
    aiao_track_mode track_mode;
    aiao_fade_rate fade_in_rate;  /* tx only */
    aiao_fade_rate fade_out_rate; /* tx only */
    hi_s32 mute;
    hi_s32 mute_fade; /* tx only */
    hi_u32 volume_db;
    hi_s32 by_bass;
    hi_bool ext_dma_mem;
    aiao_mem_attr ext_mem;

    aiao_isr_func *isr_func;
    hi_void *substream; /* only for alsa isr HI_ALSA_AI_SUPPORT */
    aiao_spdif_scms_mode spdifscms_mode;
    aiao_spdif_categorycode spdif_category_code;
    aiao_op_type op_type;
} aiao_port_user_cfg;

typedef struct {
    aiao_if_attr if_attr;
    aiao_buf_attr buf_config;
} aiao_port_attr;

typedef struct {
    hi_u64 buf_phy_addr;
    hi_u64 buf_phy_wptr;
    hi_u64 buf_phy_rptr;
    hi_u64 buf_vir_addr;
    hi_u64 buf_vir_wptr;
    hi_u64 buf_vir_rptr;
    hi_u32 buf_size;
} aiao_rbuf_attr;

typedef struct {
    /* tx/RX */
    hi_u32 dma_cnt;
    hi_u32 bus_time_out_cnt; /* update at isr */

    /* rx only */
    hi_u32 total_byte_read;
    hi_u32 try_read_cnt;
    hi_u32 buf_full_cnt;         /* update at rx isr */
    hi_u32 buf_full_warning_cnt; /* update at rx isr */
    hi_u32 bus_fifo_full_cnt;    /* update at rx isr */
    hi_u32 inf_fifo_full_cnt;    /* update at rx isr */

    /* tx only */
    hi_u32 total_byte_write;
    hi_u32 try_write_cnt;
    hi_u32 buf_empty_cnt;         /* update at tx isr */
    hi_u32 buf_empty_warning_cnt; /* update at tx isr */
    hi_u32 bus_fifo_empty_cnt;    /* update at tx isr */
    hi_u32 inf_fifo_empty_cnt;    /* update at tx isr */
    hi_u32 inf_empty_cnt_real;    /* update at tx isr */
} aiao_proc_stauts;

typedef enum {
    AIAO_PORT_STATUS_STOP = 0,
    AIAO_PORT_STATUS_START,
    AIAO_PORT_STATUS_STOP_PENDDING,
} aiao_port_status;

typedef struct {
    aiao_proc_stauts proc_status;
    aiao_port_user_cfg user_config;
    aiao_buf_info buf;
    circ_buf circ_buf;
    aiao_port_status status;
} aiao_port_stauts;

static inline hi_u32 RX_PORT2CHID(aiao_port_id port_id)
{
#ifdef HI_AIAO_TIMER_SUPPORT
    if (port_id <= AIAO_PORT_RX5) {
#else
    if (port_id <= AIAO_PORT_RX7) {
#endif
        return (port_id & 0x0f);
    } else {
        return 0;
    }
}

static inline hi_u32 TX_PORT2CHID(aiao_port_id port_id)
{
    if ((port_id <= AIAO_PORT_TX7) && (port_id >= AIAO_PORT_TX0)) {
        return (port_id & 0x0f);
    } else {
        return 0;
    }
}

static inline hi_u32 SPDIF_TX_PORT2CHID(aiao_port_id port_id)
{
#ifdef HI_AIAO_TIMER_SUPPORT
    if ((port_id <= AIAO_PORT_SPDIF_TX2) && (port_id >= AIAO_PORT_SPDIF_TX0))
#else
    if ((port_id <= AIAO_PORT_SPDIF_TX3) && (port_id >= AIAO_PORT_SPDIF_TX0))
#endif
    {
        return (port_id & 0x0f);
    } else {
        return 0;
    }
}

static inline hi_u32 PORT2CHID(aiao_port_id port_id)
{
#ifdef HI_AIAO_TIMER_SUPPORT
    if (port_id <= AIAO_PORT_RX5) {
#else
    if (port_id <= AIAO_PORT_RX7) {
#endif
        return (port_id & 0x0f);
    } else if ((port_id <= AIAO_PORT_TX7) && (port_id >= AIAO_PORT_TX0)) {
        return (port_id & 0x0f);
    }
#ifdef HI_AIAO_TIMER_SUPPORT
    else if ((port_id <= AIAO_PORT_SPDIF_TX2) && (port_id >= AIAO_PORT_SPDIF_TX0)) {
#else
    else if ((port_id <= AIAO_PORT_SPDIF_TX3) && (port_id >= AIAO_PORT_SPDIF_TX0)) {
#endif
        return (port_id & 0x0f);
    } else {
        return 0;
    }
}

static inline hi_u32 RX_CRG2ID(aiao_crg_source crg_source)
{
#ifdef HI_AIAO_TIMER_SUPPORT
    if (crg_source <= AIAO_RX_CRG5) {
#else
    if (crg_source <= AIAO_RX_CRG7) {
#endif
        return (hi_u32)crg_source;
    } else {
        return 0;
    }
}

static inline hi_u32 TX_CRG2ID(aiao_crg_source crg_source)
{
    if ((crg_source <= AIAO_TX_CRG7) && (crg_source >= AIAO_TX_CRG0)) {
        return (hi_u32)(crg_source - AIAO_TX_CRG0);
    } else {
        return 0;
    }
}

static inline hi_u32 CRG2ID(aiao_crg_source crg_source)
{
    if (crg_source <= AIAO_RX_CRG7) {
        return (hi_u32)crg_source;
    } else if ((crg_source <= AIAO_TX_CRG7) && (crg_source >= AIAO_TX_CRG0)) {
        return (hi_u32)(crg_source - AIAO_TX_CRG0);
    } else {
        return 0;
    }
}

static inline hi_s32 CRG2DIR(aiao_crg_source crg_source)
{
    if (crg_source <= AIAO_RX_CRG7) {
        return CRG_DIR_RX;
    } else {
        return CRG_DIR_TX;
    }
}

static inline hi_u32 aiao_frame_size(aiao_i2s_chnum ch, aiao_bit_depth bit_depth)
{
    hi_u32 frame_size = 0;

    switch (bit_depth) {
        case AIAO_BIT_DEPTH_8:
            frame_size = ((hi_u32)ch) * sizeof(hi_u8);
            break;
        case AIAO_BIT_DEPTH_16:
            frame_size = ((hi_u32)ch) * sizeof(hi_u16);
            break;
        case AIAO_BIT_DEPTH_24:
            frame_size = ((hi_u32)ch) * sizeof(hi_u32);
            break;
    }

    return frame_size;
}

static inline hi_u32 PORT2ID(aiao_port_id port_id)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    if (!((hi_u32)port_id & 0x30)) {
        chn_id += (hi_u32)AIAO_RX_INT0;
    } else if (((hi_u32)port_id & 0x10)) {
        chn_id += (hi_u32)AIAO_TX_INT0;
    } else if (((hi_u32)port_id & 0x20)) {
        chn_id += (hi_u32)AIAO_SPDIFTX_INT0;
    } else {
        chn_id = 0;
    }

    return chn_id; /* convert to golbal ID */
}

static inline aiao_port_id ID2PORT(hi_u32 ID)
{
    aiao_port_id port_id;

    if (ID <= AIAO_RX_INT7) {
        ID -= (hi_u32)AIAO_RX_INT0;
        port_id = (aiao_port_id)(ID);
    } else if ((ID <= AIAO_TX_INT7) && (ID >= AIAO_TX_INT0)) {
        ID -= (hi_u32)AIAO_TX_INT0;
        ID += 0x10;
        port_id = (aiao_port_id)(ID);
    }
#ifdef HI_AIAO_TIMER_SUPPORT
    else if ((ID <= AIAO_SPDIFTX_INT2) && (ID >= AIAO_SPDIFTX_INT0))
#else
    else if ((ID <= AIAO_SPDIFTX_INT3) && (ID >= AIAO_SPDIFTX_INT0))
#endif
    {
        ID -= (hi_u32)AIAO_SPDIFTX_INT0;
        ID += 0x20;
        port_id = (aiao_port_id)(ID);
    } else {
        port_id = AIAO_PORT_MAX;
    }

    return port_id;
}

static inline aiao_port_mode PORT2MODE(aiao_port_id port_id)
{
    if (port_id == AIAO_PORT_MAX) {
        return AIAO_MODE_PORT_MAX;
    } else if (!((hi_u32)port_id & 0x30)) {
        return AIAO_MODE_RXI2S;
    } else if (((hi_u32)port_id & 0x10)) {
        return AIAO_MODE_TXI2S;
    } else if (((hi_u32)port_id & 0x20)) {
        return AIAO_MODE_TXSPDIF;
    } else {
        return AIAO_MODE_PORT_MAX;
    }
}

static inline hi_s32 PORT2DIR(aiao_port_id port_id)
{
    if (((hi_u32)port_id & 0x30) == 0) {
        return AIAO_DIR_RX;
    } else {
        return AIAO_DIR_TX;
    }
}

static inline hi_s32 port2_int_status(aiao_port_id port_id, hi_u32 int_status)
{
    hi_u32 ID = PORT2ID(port_id);

    return (int_status >> ID) & 0x01;
}

static inline hi_s32 int_status(hi_u32 ID, hi_u32 int_status)
{
    return (int_status >> ID) & 0x01;
}

#ifdef HI_AIAO_TIMER_SUPPORT
static inline hi_u32 TIMER2CHID(aiao_timer_id timer_id)
{
    if (timer_id <= AIAO_TIMER_1) {
        return (timer_id & 0x0f);
    } else {
        return 0;
    }
}

static inline hi_u32 TIMER2ID(aiao_timer_id timer_id)
{
    hi_u32 chn_id = TIMER2CHID(timer_id);

    if (timer_id <= AIAO_TIMER_1) {
        chn_id += (hi_u32)AIAO_TIMER_INT0;
    } else {
        chn_id = 0;
    }
    return chn_id; /* convert to golbal ID */
}

static inline aiao_timer_id ID2TIMER(hi_u32 ID)
{
    aiao_timer_id timer_id;
    if ((ID <= AIAO_TIMER_INT1) && (ID >= AIAO_TIMER_INT0)) {
        ID -= (hi_u32)AIAO_TIMER_INT0;
        timer_id = (aiao_timer_id)(ID);
    } else {
        timer_id = 0;
    }
    return timer_id;
}

static inline hi_s32 timer2_int_status(aiao_timer_id timer_id, hi_u32 int_status)
{
    hi_u32 ID = TIMER2ID(timer_id);
    return (int_status >> ID) & 0x01;
}

typedef struct {
    aiao_timer_id timer_id;
    hi_bool enable;
} aiao_timer_enable;

typedef struct {
    aiao_timer_id timer_id;
    hi_bool enable;
    hi_u32 isr_cnt;
    aiao_sample_rate rate;
    hi_u32 config;
} aiao_timer_status;

typedef struct {
    aiao_sample_rate rate;
    hi_u32 fclk_div;
    hi_u32 bclk_div;
} aiao_if_timer_attr;

typedef struct {
    aiao_timer_id timer_id;
    hi_u32 config;
    aiao_if_timer_attr if_attr;
} aiao_timer_attr;

typedef struct {
    hi_handle handle;
    aiao_timer_id timer_id;
    aiao_timer_isr_func *timer_isr_func;
    hi_void *substream;
} aiao_timer_create;

typedef struct {
    aiao_timer_id timer_id;
} aiao_timer_destroy;
#endif

typedef struct {
    hi_u32 capability;
} aiao_port_get_capability;

typedef struct {
    aiao_port_id port_id;
    aiao_port_user_cfg config;
} aiao_port_open;

typedef struct {
    aiao_port_id port_id;
} aiao_port_close;

typedef struct {
    aiao_port_id port_id;
} aiao_port_start;

typedef struct {
    aiao_port_id port_id;
    aiao_spdifport_source src_chn_id;
} aiao_port_select_spdif_source;

typedef struct {
    aiao_port_id port_id;
    hi_s32 en;
} aiao_port_set_spdif_out_port;

typedef struct {
    aiao_port_id port_id;
    aiao_port_stopmode stop_mode;
} aiao_port_stop;

typedef struct {
    aiao_port_id port_id;
    hi_bool mute;
} aiao_port_mute;

typedef struct {
    aiao_port_id port_id;
    hi_u32 volume_db;
} aiao_port_volume;

typedef struct {
    aiao_port_id port_id;
    aiao_track_mode track_mode;
} aiao_port_track_mode;

typedef struct {
    aiao_port_id port_id;
    aiao_port_user_cfg user_config;
} aiao_port_get_user_config;

typedef struct {
    aiao_port_id port_id;
    aiao_port_stauts status;
} aiao_port_get_status;

typedef struct {
    aiao_port_id port_id;
    hi_u8 *dest;
    hi_u32 dest_size;

    hi_u32 read_bytes;
} aiao_port_read_data;

typedef struct {
    aiao_port_id port_id;
    hi_u8 *src;
    hi_u32 src_len;
    hi_u32 write_bytes;
} aiao_port_write_data;

typedef struct {
    aiao_port_id port_id;
    hi_u32 data_size;
    hi_u32 free_size;
} aiao_port_buf_status;

typedef struct {
    hi_s32 is_read;
    hi_u32 reg_addr_base;
    hi_u32 reg_addr_off_set;
    hi_u32 reg_value;
} aiao_dbg_reg;

typedef struct {
    aiao_i2s_sd sd0;
    aiao_i2s_sd sd1;
    aiao_i2s_sd sd2;
    aiao_i2s_sd sd3;
} aiao_i2s_data_sel;

typedef struct {
    aiao_port_id port_id;
    aiao_i2s_data_sel sd_sel;
} aiao_port_i2s_data_sel;

/* for ALSA */
typedef struct {
    hi_u32 channels;
    hi_u32 sampler_rate;
    hi_u32 bit_depth;
    hi_void *addr;
} aiao_alsa_dac_status;

typedef struct {
    hi_u32 init_flag; /* bit0: rx, bit1: tx */
    aiao_port_id tx_port_id;
    aiao_i2s_source tx_source;
    aiao_crg_mode tx_crg_mode;
    aiao_crg_source tx_crg_source;

    aiao_port_id rx_port_id;
    aiao_i2s_source rx_source;
    aiao_crg_mode rx_crg_mode;
    aiao_crg_source rx_crg_source;

    aiao_if_attr if_common_attr;
} aiao_i2s_board_config;

typedef struct {
    hi_u32 init_flag; /* bit0: rx, bit1: tx */
    aiao_port_id tx_port_id;
    aiao_i2s_source tx_source;
    aiao_crg_mode tx_crg_mode;
    aiao_crg_source tx_crg_source;

    aiao_port_id rx_port_id;
    aiao_i2s_source rx_source;
    aiao_crg_mode rx_crg_mode;
    aiao_crg_source rx_crg_source;

    aiao_if_attr if_common_attr;
} aiao_i2s_int_config;

#endif  /* __HAL_AIAO_COMMON_H__ */
