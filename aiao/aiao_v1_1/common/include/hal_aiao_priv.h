/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HAL_AIAO_PRIV_H__
#define __HAL_AIAO_PRIV_H__

#include "hi_drv_ao.h"
#include "hi_drv_ai.h"
#include "hal_aiao_common.h"
#include "circ_buf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HW_CHN_PTR_BUG
#define AIAO_PROC_NAME_LENGTH 16
#define AIAO_BIT_SHIFT_2      2
#define AIAO_BIT_SHIFT_3      3
#define AIAO_BIT_SHIFT_4      4
#define AIAO_BIT_SHIFT_6      6
#define AIAO_BIT_SHIFT_8      8
#define AIAO_CLK_TIMEOUT      10

typedef struct {
    aiao_port_user_cfg user_congfig;
    aiao_port_id port_id;

    /* internal state */
    aiao_port_status port_status;
    aiao_proc_stauts proc_status;
    hi_audio_buffer mmz;
    circ_buf cb;
    aiao_buf_info buf;
    hi_s32 stop_fade_out_mute;
} aiao_port;

#ifdef HI_AIAO_TIMER_SUPPORT
typedef struct {
    aiao_timer_id timer_id;
    aiao_timer_create timer_param;
    aiao_timer_status timer_status;
    aiao_timer_attr timer_attr;
} aiao_timer_ctx, *aiao_timer;
#endif

/* define the union U_AIAO_INT_ENA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_ch0_int_ena : 1;       /* [0] */
        unsigned int rx_ch1_int_ena : 1;       /* [1] */
        unsigned int rx_ch2_int_ena : 1;       /* [2] */
        unsigned int rx_ch3_int_ena : 1;       /* [3] */
        unsigned int rx_ch4_int_ena : 1;       /* [4] */
        unsigned int rx_ch5_int_ena : 1;       /* [5] */
        unsigned int rx_ch6_int_ena : 1;       /* [6] */
        unsigned int rx_ch7_int_ena : 1;       /* [7] */
        unsigned int reserved_1 : 8;           /* [15..8] */
        unsigned int tx_ch0_int_ena : 1;       /* [16] */
        unsigned int tx_ch1_int_ena : 1;       /* [17] */
        unsigned int tx_ch2_int_ena : 1;       /* [18] */
        unsigned int tx_ch3_int_ena : 1;       /* [19] */
        unsigned int tx_ch4_int_ena : 1;       /* [20] */
        unsigned int tx_ch5_int_ena : 1;       /* [21] */
        unsigned int tx_ch6_int_ena : 1;       /* [22] */
        unsigned int tx_ch7_int_ena : 1;       /* [23] */
        unsigned int spdiftx_ch0_int_ena : 1;  /* [24] */
        unsigned int spdiftx_ch1_int_ena : 1;  /* [25] */
        unsigned int spdiftx_ch2_int_ena : 1;  /* [26] */
#ifdef HI_AIAO_TIMER_SUPPORT
        unsigned int timer_ch0_int_ena : 1;  /* [27]  its name in AIAO register is: timer0_int_ena */
        unsigned int timer_ch1_int_ena : 1;  /* [28]  its name in AIAO register is: timer1_int_ena */
        unsigned int reserved_0 : 3;         /* [31..29] */
#else
        unsigned int spdiftx_ch3_int_ena : 1;     /* [27] */
        unsigned int reserved_0 : 4;              /* [31..28] */
#endif
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_ENA;

/* define the union U_AIAO_INT_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_ch0_int_status : 1;       /* [0] */
        unsigned int rx_ch1_int_status : 1;       /* [1] */
        unsigned int rx_ch2_int_status : 1;       /* [2] */
        unsigned int rx_ch3_int_status : 1;       /* [3] */
        unsigned int rx_ch4_int_status : 1;       /* [4] */
        unsigned int rx_ch5_int_status : 1;       /* [5] */
        unsigned int rx_ch6_int_status : 1;       /* [6] */
        unsigned int rx_ch7_int_status : 1;       /* [7] */
        unsigned int reserved_1 : 8;              /* [15..8] */
        unsigned int tx_ch0_int_status : 1;       /* [16] */
        unsigned int tx_ch1_int_status : 1;       /* [17] */
        unsigned int tx_ch2_int_status : 1;       /* [18] */
        unsigned int tx_ch3_int_status : 1;       /* [19] */
        unsigned int tx_ch4_int_status : 1;       /* [20] */
        unsigned int tx_ch5_int_status : 1;       /* [21] */
        unsigned int tx_ch6_int_status : 1;       /* [22] */
        unsigned int tx_ch7_int_status : 1;       /* [23] */
        unsigned int spdiftx_ch0_int_status : 1;  /* [24] */
        unsigned int spdiftx_ch1_int_status : 1;  /* [25] */
        unsigned int spdiftx_ch2_int_status : 1;  /* [26] */
#ifdef HI_AIAO_TIMER_SUPPORT
        unsigned int timer_ch0_int_status : 1;  /* [27] */
        unsigned int timer_ch1_int_status : 1;  /* [28] */
        unsigned int reserved_0 : 3;            /* [31..29] */
#else
        unsigned int spdiftx_ch3_int_status : 1;  /* [27] */
        unsigned int reserved_0 : 4;              /* [31..28] */
#endif
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_STATUS;

/* define the union U_AIAO_INT_RAW */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_ch0_int_raw : 1;       /* [0] */
        unsigned int rx_ch1_int_raw : 1;       /* [1] */
        unsigned int rx_ch2_int_raw : 1;       /* [2] */
        unsigned int rx_ch3_int_raw : 1;       /* [3] */
        unsigned int rx_ch4_int_raw : 1;       /* [4] */
        unsigned int rx_ch5_int_raw : 1;       /* [5] */
        unsigned int rx_ch6_int_raw : 1;       /* [6] */
        unsigned int rx_ch7_int_raw : 1;       /* [7] */
        unsigned int reserved_1 : 8;           /* [15..8] */
        unsigned int tx_ch0_int_raw : 1;       /* [16] */
        unsigned int tx_ch1_int_raw : 1;       /* [17] */
        unsigned int tx_ch2_int_raw : 1;       /* [18] */
        unsigned int tx_ch3_int_raw : 1;       /* [19] */
        unsigned int tx_ch4_int_raw : 1;       /* [20] */
        unsigned int tx_ch5_int_raw : 1;       /* [21] */
        unsigned int tx_ch6_int_raw : 1;       /* [22] */
        unsigned int tx_ch7_int_raw : 1;       /* [23] */
        unsigned int spdiftx_ch0_int_raw : 1;  /* [24] */
        unsigned int spdiftx_ch1_int_raw : 1;  /* [25] */
        unsigned int spdiftx_ch2_int_raw : 1;  /* [26] */
#ifdef HI_AIAO_TIMER_SUPPORT
        unsigned int timer_ch0_int_raw : 1;  /* [27] */
        unsigned int timer_ch1_int_raw : 1;  /* [28] */
        unsigned int reserved_0 : 3;         /* [31..29] */
#else
        unsigned int spdiftx_ch3_int_raw : 1;     /* [27] */
        unsigned int reserved_0 : 4;              /* [31..28] */
#endif
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_RAW;

/* define the union U_HW_CAPABILITY */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_ch0_cap : 1;       /* [0] */
        unsigned int rx_ch1_cap : 1;       /* [1] */
        unsigned int rx_ch2_cap : 1;       /* [2] */
        unsigned int rx_ch3_cap : 1;       /* [3] */
        unsigned int rx_ch4_cap : 1;       /* [4] */
        unsigned int rx_ch5_cap : 1;       /* [5] */
        unsigned int rx_ch6_cap : 1;       /* [6] */
        unsigned int rx_ch7_cap : 1;       /* [7] */
        unsigned int reserved_1 : 8;       /* [15..8] */
        unsigned int tx_ch0_cap : 1;       /* [16] */
        unsigned int tx_ch1_cap : 1;       /* [17] */
        unsigned int tx_ch2_cap : 1;       /* [18] */
        unsigned int tx_ch3_cap : 1;       /* [19] */
        unsigned int tx_ch4_cap : 1;       /* [20] */
        unsigned int tx_ch5_cap : 1;       /* [21] */
        unsigned int tx_ch6_cap : 1;       /* [22] */
        unsigned int tx_ch7_cap : 1;       /* [23] */
        unsigned int spdiftx_ch0_cap : 1;  /* [24] */
        unsigned int spdiftx_ch1_cap : 1;  /* [25] */
        unsigned int spdiftx_ch2_cap : 1;  /* [26] */
#ifdef HI_AIAO_TIMER_SUPPORT
        unsigned int timer_ch0_cap : 1;  /* [27] */
        unsigned int timer_ch1_cap : 1;  /* [28] */
        unsigned int reserved_0 : 3;     /* [31..29] */
#else
        unsigned int spdiftx_ch3_cap : 1;         /* [27] */
        unsigned int reserved_0 : 4;              /* [31..28] */
#endif
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_HW_CAPABILITY;

/* define the union U_SPDIF_TX_MUX */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int spdif_tx_0_port_sel : 3;  /* [2..0] */
        unsigned int spdif_tx_0_port_en : 1;   /* [3] */
        unsigned int reserved_3 : 4;           /* [7..4] */
        unsigned int spdif_tx_1_port_sel : 3;  /* [10..8] */
        unsigned int spdif_tx_1_port_en : 1;   /* [11] */
        unsigned int reserved_2 : 4;           /* [15..12] */
        unsigned int spdif_tx_2_port_sel : 3;  /* [18..16] */
        unsigned int spdif_tx_2_port_en : 1;   /* [19] */
        unsigned int reserved_1 : 4;           /* [23..20] */
        unsigned int spdif_tx_3_port_sel : 3;  /* [26..24] */
        unsigned int spdif_tx_3_port_en : 1;   /* [27] */
        unsigned int reserved_0 : 4;           /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_TX_MUX;

/* define the union U_AIAO_SWITCH_RX_BCLK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int inner_bclk_ws_sel_rx_00 : 4; /* [3..0] */
        unsigned int inner_bclk_ws_sel_rx_01 : 4; /* [7..4] */
        unsigned int inner_bclk_ws_sel_rx_02 : 4; /* [11..8] */
        unsigned int inner_bclk_ws_sel_rx_03 : 4; /* [15..12] */
        unsigned int inner_bclk_ws_sel_rx_04 : 4; /* [19..16] */
        unsigned int inner_bclk_ws_sel_rx_05 : 4; /* [23..20] */
        unsigned int inner_bclk_ws_sel_rx_06 : 4; /* [27..24] */
        unsigned int inner_bclk_ws_sel_rx_07 : 4; /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_SWITCH_RX_BCLK;

/* define the union U_AIAO_SWITCH_TX_BCLK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int inner_bclk_ws_sel_tx_00 : 4; /* [3..0] */
        unsigned int inner_bclk_ws_sel_tx_01 : 4; /* [7..4] */
        unsigned int inner_bclk_ws_sel_tx_02 : 4; /* [11..8] */
        unsigned int inner_bclk_ws_sel_tx_03 : 4; /* [15..12] */
        unsigned int inner_bclk_ws_sel_tx_04 : 4; /* [19..16] */
        unsigned int inner_bclk_ws_sel_tx_05 : 4; /* [23..20] */
        unsigned int inner_bclk_ws_sel_tx_06 : 4; /* [27..24] */
        unsigned int inner_bclk_ws_sel_tx_07 : 4; /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_SWITCH_TX_BCLK;

/* define the union U_AIAO_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int srst_rdy : 1;    /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_STATUS;

/* define the union U_VHB_OUTSTANDING */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int vhb_outst_num : 3; /* [2..0] */
        unsigned int reserved_14 : 29;  /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_VHB_OUTSTANDING;

/* define the union U_CRG_RST_EN_N */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aiao_crg_rst_en_n : 1; /* [0] */
        unsigned int reserved_0 : 31;       /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_CRG_RST_EN_N;

/* define the union U_I2S_CRG_CFG0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aiao_mclk_div : 27;  /* [26..0] */
        unsigned int int_div_mode : 1;    /* [27] */
        unsigned int reserved_0 : 4;      /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_I2S_CRG_CFG0;

/* define the union U_I2S_CRG_CFG1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aiao_bclk_div : 4;       /* [3..0] */
        unsigned int aiao_fsclk_div : 3;      /* [6..4] */
        unsigned int reserved_1 : 1;          /* [7] */
        unsigned int aiao_cken : 1;           /* [8] */
        unsigned int aiao_srst_req : 1;       /* [9] */
        unsigned int aiao_bclk_oen : 1;       /* [10] */
        unsigned int aiao_bclk_sel : 1;       /* [11] */
        unsigned int aiao_bclkin_pctrl : 1;   /* [12] */
        unsigned int aiao_bclkout_pctrl : 1;  /* [13] */
        unsigned int reserved_0 : 18;         /* [31..14] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_I2S_CRG_CFG1;

/* define the union U_SPDIF_CRG_CFG0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aiao_mclk_div_spdif : 27;  /* [26..0] */
        unsigned int int_div_mode : 1;          /* [27] */
        unsigned int reserved_0 : 4;            /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CRG_CFG0;

/* define the union U_SPDIF_CRG_CFG1_03 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aiao_bclk_div_spdif : 4;   /* [3..0] */
        unsigned int aiao_fsclk_div_spdif : 3;  /* [6..4] */
        unsigned int reserved_1 : 1;            /* [7] */
        unsigned int aiao_cken_spdif : 1;       /* [8] */
        unsigned int aiao_srst_req_spdif : 1;   /* [9] */
        unsigned int reserved_0 : 22;           /* [31..10] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CRG_CFG1;

/* define the union U_RX_IF_ATTRI */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_mode : 2;           /* [1..0] */
        unsigned int rx_i2s_precision : 2;  /* [3..2] */
        unsigned int rx_ch_num : 2;         /* [5..4] */
        unsigned int reserved_1 : 1;        /* [6] */
        unsigned int rx_multislot_en : 1;   /* [7] */
        unsigned int rx_sd_offset : 8;      /* [15..8] */
        unsigned int rx_trackmode : 3;      /* [18..16] */
        unsigned int reserved_0 : 1;        /* [19] */
        unsigned int rx_sd_source_sel : 4;  /* [23..20] */
        unsigned int rx_sd0_sel : 2;        /* [25..24] */
        unsigned int rx_sd1_sel : 2;        /* [27..26] */
        unsigned int rx_sd2_sel : 2;        /* [29..28] */
        unsigned int rx_sd3_sel : 2;        /* [31..30] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_IF_ATTRI;

/* define the union U_RX_DSP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mute_en : 1;          /* [0] */
        unsigned int mute_fade_en : 1;     /* [1] */
        unsigned int pause_en : 1;         /* [2] */
        unsigned int pause_fade_en : 1;    /* [3] */
        unsigned int reserved_3 : 4;       /* [7..4] */
        unsigned int volume : 7;           /* [14..8] */
        unsigned int reserved_2 : 1;       /* [15] */
        unsigned int fade_in_rate : 4;     /* [19..16] */
        unsigned int fade_out_rate : 4;    /* [23..20] */
        unsigned int reserved_1 : 3;       /* [26..24] */
        unsigned int bypass_en : 1;        /* [27] */
        unsigned int rx_enable : 1;        /* [28] */
        unsigned int rx_disable_done : 1;  /* [29] */
        unsigned int reserved_0 : 2;       /* [31..30] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_DSP_CTRL;

/* define the union U_RX_WS_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ws_count : 24;   /* [23..0] */
        unsigned int reserved_0 : 8;  /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_WS_CNT;

/* define the union U_RX_BCLK_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int bclk_count : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;   /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_BCLK_CNT;

/* define the union U_RX_BUFF_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_buff_size : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_BUFF_SIZE;

/* define the union U_RX_BUFF_WPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_buff_wptr : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_BUFF_WPTR;

/* define the union U_RX_BUFF_RPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_buff_rptr : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_BUFF_RPTR;

/* define the union U_RX_BUFF_ALFULL_TH */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_buff_alfull_th : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;          /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_BUFF_ALFULL_TH;

/* define the union U_RX_TRANS_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_trans_size : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;      /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_TRANS_SIZE;

/* define the union U_RX_WPTR_TMP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_wptr_tmp : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;    /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_WPTR_TMP;

/* define the union U_RX_INT_ENA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_trans_int_ena : 1;       /* [0] */
        unsigned int rx_full_int_ena : 1;        /* [1] */
        unsigned int rx_alfull_int_ena : 1;      /* [2] */
        unsigned int rx_bfifo_full_int_ena : 1;  /* [3] */
        unsigned int rx_ififo_full_int_ena : 1;  /* [4] */
        unsigned int rx_stop_int_ena : 1;        /* [5] */
        unsigned int reserved_0 : 26;            /* [31..6] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_INT_ENA;

/* define the union U_RX_INT_RAW */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_trans_int_raw : 1;       /* [0] */
        unsigned int rx_full_int_raw : 1;        /* [1] */
        unsigned int rx_alfull_int_raw : 1;      /* [2] */
        unsigned int rx_bfifo_full_int_raw : 1;  /* [3] */
        unsigned int rx_ififo_full_int_raw : 1;  /* [4] */
        unsigned int rx_stop_int_raw : 1;        /* [5] */
        unsigned int reserved_0 : 26;            /* [31..6] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_INT_RAW;

/* define the union U_RX_INT_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_trans_int_status : 1;       /* [0] */
        unsigned int rx_full_int_status : 1;        /* [1] */
        unsigned int rx_alfull_int_status : 1;      /* [2] */
        unsigned int rx_bfifo_full_int_status : 1;  /* [3] */
        unsigned int rx_ififo_full_int_status : 1;  /* [4] */
        unsigned int rx_stop_int_status : 1;        /* [5] */
        unsigned int reserved_0 : 26;               /* [31..6] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_INT_STATUS;

/* define the union U_RX_INT_CLR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int rx_trans_int_clear : 1;       /* [0] */
        unsigned int rx_full_int_clear : 1;        /* [1] */
        unsigned int rx_alfull_int_clear : 1;      /* [2] */
        unsigned int rx_bfifo_full_int_clear : 1;  /* [3] */
        unsigned int rx_ififo_full_int_clear : 1;  /* [4] */
        unsigned int rx_stop_int_clear : 1;        /* [5] */
        unsigned int reserved_0 : 26;              /* [31..6] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_RX_INT_CLR;

/* define the union U_TX_IF_ATTRI */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_mode : 2;           /* [1..0] */
        unsigned int tx_i2s_precision : 2;  /* [3..2] */
        unsigned int tx_ch_num : 2;         /* [5..4] */
        unsigned int reserved_2 : 1;        /* [6] */
        unsigned int reserved_1 : 1;        /* [7] */
        unsigned int tx_sd_offset : 8;      /* [15..8] */
        unsigned int tx_trackmode : 3;      /* [18..16] */
        unsigned int tx_spd_i2s_sel : 1;    /* [19] */
        unsigned int tx_sd_source_sel : 4;  /* [23..20] */
        unsigned int tx_sd0_sel : 2;        /* [25..24] */
        unsigned int tx_sd1_sel : 2;        /* [27..26] */
        unsigned int tx_sd2_sel : 2;        /* [29..28] */
        unsigned int tx_sd3_sel : 2;        /* [31..30] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_IF_ATTRI;

/* define the union U_TX_DSP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mute_en : 1;          /* [0] */
        unsigned int mute_fade_en : 1;     /* [1] */
        unsigned int reserved_3 : 6;       /* [7..2] */
        unsigned int volume : 7;           /* [14..8] */
        unsigned int reserved_2 : 1;       /* [15] */
        unsigned int fade_in_rate : 4;     /* [19..16] */
        unsigned int fade_out_rate : 4;    /* [23..20] */
        unsigned int reserved_1 : 3;       /* [26..24] */
        unsigned int bypass_en : 1;        /* [27] */
        unsigned int tx_enable : 1;        /* [28] */
        unsigned int tx_disable_done : 1;  /* [29] */
        unsigned int reserved_0 : 2;       /* [31..30] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_DSP_CTRL;

/* define the union U_TX_DSP_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mute_fade_vol : 7;  /* [6..0] */
        unsigned int reserved_0 : 25;    /* [31..7] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_DSP_STATUS;

/* define the union U_TX_WS_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ws_count : 24;   /* [23..0] */
        unsigned int reserved_0 : 8;  /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_WS_CNT;

/* define the union U_TX_BCLK_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int bclk_count : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;   /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_BCLK_CNT;

/* define the union U_TX_BUFF_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_size : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_BUFF_SIZE;

/* define the union U_TX_BUFF_WPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_wptr : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_BUFF_WPTR;

/* define the union U_TX_BUFF_RPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_rptr : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_BUFF_RPTR;

/* define the union U_TX_BUFF_ALEMPTY_TH */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_alempty_th : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;           /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_BUFF_ALEMPTY_TH;

/* define the union U_TX_TRANS_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_size : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;      /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_TRANS_SIZE;

/* define the union U_TX_RPTR_TMP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_rptr_tmp : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;    /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_RPTR_TMP;

/* define the union U_TX_INT_ENA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_ena : 1;        /* [0] */
        unsigned int tx_empty_int_ena : 1;        /* [1] */
        unsigned int tx_alempty_int_ena : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_ena : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_ena : 1;  /* [4] */
        unsigned int tx_stop_int_ena : 1;         /* [5] */
        unsigned int tx_mfade_int_ena : 1;        /* [6] */
        unsigned int tx_dat_break_int_ena : 1;    /* [7] */
        unsigned int reserved_0 : 24;             /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_INT_ENA;

/* define the union U_TX_INT_RAW */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_raw : 1;        /* [0] */
        unsigned int tx_empty_int_raw : 1;        /* [1] */
        unsigned int tx_alempty_int_raw : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_raw : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_raw : 1;  /* [4] */
        unsigned int tx_stop_int_raw : 1;         /* [5] */
        unsigned int tx_mfade_int_raw : 1;        /* [6] */
        unsigned int tx_dat_break_int_raw : 1;    /* [7] */
        unsigned int reserved_0 : 24;             /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_INT_RAW;

/* define the union U_TX_INT_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_status : 1;        /* [0] */
        unsigned int tx_empty_int_status : 1;        /* [1] */
        unsigned int tx_alempty_int_status : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_status : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_status : 1;  /* [4] */
        unsigned int tx_stop_int_status : 1;         /* [5] */
        unsigned int tx_mfade_int_status : 1;        /* [6] */
        unsigned int tx_dat_break_int_status : 1;    /* [7] */
        unsigned int reserved_0 : 24;                /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_INT_STATUS;

/* define the union U_TX_INT_CLR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_clear : 1;        /* [0] */
        unsigned int tx_empty_int_clear : 1;        /* [1] */
        unsigned int tx_alempty_int_clear : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_clear : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_clear : 1;  /* [4] */
        unsigned int tx_stop_int_clear : 1;         /* [5] */
        unsigned int tx_mfade_int_clear : 1;        /* [6] */
        unsigned int tx_dat_break_int_clear : 1;    /* [7] */
        unsigned int reserved_0 : 24;               /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_TX_INT_CLR;

/* define the union U_SPDIFTX_IF_ATTRI */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_2 : 2;        /* [1..0] */
        unsigned int tx_i2s_precision : 2;  /* [3..2] */
        unsigned int tx_ch_num : 2;         /* [5..4] */
        unsigned int reserved_1 : 10;       /* [15..6] */
        unsigned int tx_trackmode : 3;      /* [18..16] */
        unsigned int tx_spd_i2s_sel : 1;    /* [19] */
        unsigned int reserved_0 : 12;       /* [31..20] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_IF_ATTRI;

/* define the union U_SPDIFTX_DSP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mute_en : 1;          /* [0] */
        unsigned int mute_fade_en : 1;     /* [1] */
        unsigned int reserved_3 : 6;       /* [7..2] */
        unsigned int volume : 7;           /* [14..8] */
        unsigned int reserved_2 : 1;       /* [15] */
        unsigned int fade_in_rate : 4;     /* [19..16] */
        unsigned int fade_out_rate : 4;    /* [23..20] */
        unsigned int reserved_1 : 3;       /* [26..24] */
        unsigned int bypass_en : 1;        /* [27] */
        unsigned int tx_enable : 1;        /* [28] */
        unsigned int tx_disable_done : 1;  /* [29] */
        unsigned int reserved_0 : 2;       /* [31..30] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_DSP_CTRL;

/* define the union U_SPDIFTX_DSP_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int mute_fade_vol : 7;  /* [6..0] */
        unsigned int reserved_0 : 25;    /* [31..7] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_DSP_STATUS;

/* define the union U_SPDIFTX_WS_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int ws_count : 24;   /* [23..0] */
        unsigned int reserved_0 : 8;  /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_WS_CNT;

/* define the union U_SPDIFTX_BCLK_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int bclk_count : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;   /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_BCLK_CNT;

/* define the union U_SPDIFTX_BUFF_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_size : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_BUFF_SIZE;

/* define the union U_SPDIFTX_BUFF_WPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_wptr : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_BUFF_WPTR;

/* define the union U_SPDIFTX_BUFF_RPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_rptr : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_BUFF_RPTR;

/* define the union U_SPDIFTX_BUFF_ALEMPTY_TH */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_buff_alempty_th : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;           /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_BUFF_ALEMPTY_TH;

/* define the union U_SPDIFTX_TRANS_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_size : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;      /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_TRANS_SIZE;

/* define the union U_SPDIFTX_RPTR_TMP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_rptr_tmp : 24;  /* [23..0] */
        unsigned int reserved_0 : 8;    /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_RPTR_TMP;

/* define the union U_SPDIFTX_INT_ENA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_ena : 1;        /* [0] */
        unsigned int tx_empty_int_ena : 1;        /* [1] */
        unsigned int tx_alempty_int_ena : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_ena : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_ena : 1;  /* [4] */
        unsigned int tx_stop_int_ena : 1;         /* [5] */
        unsigned int tx_mfade_int_ena : 1;        /* [6] */
        unsigned int tx_dat_break_int_ena : 1;    /* [7] */
        unsigned int reserved_0 : 24;             /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_INT_ENA;

/* define the union U_SPDIFTX_INT_RAW */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_raw : 1;        /* [0] */
        unsigned int tx_empty_int_raw : 1;        /* [1] */
        unsigned int tx_alempty_int_raw : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_raw : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_raw : 1;  /* [4] */
        unsigned int tx_stop_int_raw : 1;         /* [5] */
        unsigned int tx_mfade_int_raw : 1;        /* [6] */
        unsigned int tx_dat_break_int_raw : 1;    /* [7] */
        unsigned int reserved_0 : 24;             /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_INT_RAW;

/* define the union U_SPDIFTX_INT_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_status : 1;        /* [0] */
        unsigned int tx_empty_int_status : 1;        /* [1] */
        unsigned int tx_alempty_int_status : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_status : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_status : 1;  /* [4] */
        unsigned int tx_stop_int_status : 1;         /* [5] */
        unsigned int tx_mfade_int_status : 1;        /* [6] */
        unsigned int tx_dat_break_int_status : 1;    /* [7] */
        unsigned int reserved_0 : 24;                /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_INT_STATUS;

/* define the union U_SPDIFTX_INT_CLR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_int_clear : 1;        /* [0] */
        unsigned int tx_empty_int_clear : 1;        /* [1] */
        unsigned int tx_alempty_int_clear : 1;      /* [2] */
        unsigned int tx_bfifo_empty_int_clear : 1;  /* [3] */
        unsigned int tx_ififo_empty_int_clear : 1;  /* [4] */
        unsigned int tx_stop_int_clear : 1;         /* [5] */
        unsigned int tx_mfade_int_clear : 1;        /* [6] */
        unsigned int tx_dat_break_int_clear : 1;    /* [7] */
        unsigned int reserved_0 : 24;               /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIFTX_INT_CLR;

/* define the union U_SPDIF_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int spdif_en : 1;     /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CTRL;

/* define the union U_SPDIF_CONFIG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int txdata_en : 1;    /* [0] */
        unsigned int mode : 2;         /* [2..1] */
        unsigned int reserved_0 : 29;  /* [31..3] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CONFIG;

/* define the union U_SPDIF_INTRUPT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lfifo_empty_intr : 1;   /* [0] */
        unsigned int rfifo_empty_intr : 1;   /* [1] */
        unsigned int lfifo_single_intr : 1;  /* [2] */
        unsigned int rfifo_single_intr : 1;  /* [3] */
        unsigned int reserved_0 : 28;        /* [31..4] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_INTRUPT;

/* define the union U_SPDIF_INTR_MASK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lfifo_empty_mask : 1;    /* [0] */
        unsigned int rfifo_empty__mask : 1;   /* [1] */
        unsigned int lfifo_single__mask : 1;  /* [2] */
        unsigned int rfifo_single__mask : 1;  /* [3] */
        unsigned int reserved_0 : 28;         /* [31..4] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_INTR_MASK;

/* define the union U_SPDIF_INTR_STATE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int lfifo_empty_state : 1;   /* [0] */
        unsigned int rfifo_empty_state : 1;   /* [1] */
        unsigned int lfifo_single_state : 1;  /* [2] */
        unsigned int rfifo_single_state : 1;  /* [3] */
        unsigned int reserved_0 : 28;         /* [31..4] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_INTR_STATE;

/* define the union U_SPDIF_CH_STATUS1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int l_use_a : 1;      /* [0] */
        unsigned int l_use_b : 1;      /* [1] */
        unsigned int l_use_c : 1;      /* [2] */
        unsigned int l_use_d : 3;      /* [5..3] */
        unsigned int l_use_mode : 2;   /* [7..6] */
        unsigned int r_use_a : 1;      /* [8] */
        unsigned int r_use_b : 1;      /* [9] */
        unsigned int r_use_c : 1;      /* [10] */
        unsigned int r_use_d : 3;      /* [13..11] */
        unsigned int r_use_mode : 2;   /* [15..14] */
        unsigned int reserved_0 : 16;  /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CH_STATUS1;

/* define the union U_SPDIF_CH_STATUS2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int l_category_code : 8;  /* [7..0] */
        unsigned int r_category_code : 8;  /* [15..8] */
        unsigned int reserved_0 : 16;      /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CH_STATUS2;

/* define the union U_SPDIF_CH_STATUS3 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int l_src_nb : 4;     /* [3..0] */
        unsigned int l_ch_nb : 4;      /* [7..4] */
        unsigned int r_src_nb : 4;     /* [11..8] */
        unsigned int r_ch_nb : 4;      /* [15..12] */
        unsigned int reserved_0 : 16;  /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CH_STATUS3;

/* define the union U_SPDIF_CH_STATUS4 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int l_freq : 4;          /* [3..0] */
        unsigned int l_clk_accuracy : 2;  /* [5..4] */
        unsigned int reserved_1 : 2;      /* [7..6] */
        unsigned int r_freq : 4;          /* [11..8] */
        unsigned int r_clk_accuracy : 2;  /* [13..12] */
        unsigned int reserved_0 : 18;     /* [31..14] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CH_STATUS4;

/* define the union U_SPDIF_CH_STATUS5 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int l_bit_max : 1;    /* [0] */
        unsigned int l_bit_width : 3;  /* [3..1] */
        unsigned int l_org_freq : 4;   /* [7..4] */
        unsigned int r_bit_max : 1;    /* [8] */
        unsigned int r_bit_width : 3;  /* [11..9] */
        unsigned int r_org_freq : 4;   /* [15..12] */
        unsigned int reserved_0 : 16;  /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_SPDIF_CH_STATUS5;

typedef struct {
    volatile U_I2S_CRG_CFG0 I2S_CRG_CFG0;
    volatile U_I2S_CRG_CFG1 I2S_CRG_CFG1;
} S_AIAO_I2S_CRG_CFG_REGS_TYPE;

typedef struct {
    volatile U_SPDIF_CRG_CFG0 SPDIF_CRG_CFG0;
    volatile U_SPDIF_CRG_CFG1 SPDIF_CRG_CFG1;
} S_AIAO_SPDIF_CRG_CFG_REGS_TYPE;

#ifdef HI_AIAO_TIMER_SUPPORT
typedef union {
    struct {
        unsigned int aiao_mclk_div : 27;  /* [26..0] */
        unsigned int reserved_0 : 5;      /* [31..27] */
    } bits;
    unsigned int u32;
} U_TIMER_CRG_CFG0;

typedef union {
    struct {
        unsigned int aiao_bclk_div : 4;       /* [3..0] */
        unsigned int aiao_fsclk_div : 3;      /* [6..4] */
        unsigned int reserved_1 : 1;          /* [7] */
        unsigned int aiao_cken : 1;           /* [8] */
        unsigned int aiao_srst_req : 1;       /* [9] */
        unsigned int aiao_bclk_oen : 1;       /* [10] */
        unsigned int aiao_bclk_sel : 1;       /* [11] */
        unsigned int aiao_bclkin_pctrl : 1;   /* [12] */
        unsigned int aiao_bclkout_pctrl : 1;  /* [13] */
        unsigned int reserved_0 : 18;         /* [31..14] */
    } bits;
    unsigned int u32;
} U_TIMER_CRG_CFG1;

typedef union {
    struct {
        unsigned int timer_ch0_clr : 1;  /* [0] */
        unsigned int timer_ch1_clr : 1;  /* [1] */
        unsigned int reserved_0 : 30;    /* [31..2] */
    } bits;
    unsigned int u32;
} U_TIMER_CH_CLEAR;

typedef union {
    struct {
        unsigned int timer_config : 13;  /* [12..0] */
        unsigned int reserved_0 : 19;    /* [31..13] */
    } bits;
    unsigned int u32;
} U_TIMER_CH_CONFIG;

typedef struct {
    volatile U_TIMER_CH_CONFIG TIMER_CONFIG;
} S_AIAO_TIER_CONFIG;

typedef struct {
    volatile U_TIMER_CH_CLEAR TIMER_INT_CLEAR;
} S_AIAO_TIER_CLEAR;

typedef struct {
    volatile U_TIMER_CRG_CFG0 TIMER_CRG_CFG0;
    volatile U_TIMER_CRG_CFG1 TIMER_CRG_CFG1;
} S_AIAO_TIER_CRG_CFG_REGS_TYPE;
#endif

/* ============================================================================== */
/* define the global struct */
typedef struct {
    volatile U_AIAO_INT_ENA AIAO_INT_ENA;
    volatile U_AIAO_INT_STATUS AIAO_INT_STATUS;
    volatile U_AIAO_INT_RAW AIAO_INT_RAW;
    volatile unsigned int HW_CAPABILITY;
    volatile unsigned int SW_KEY0;
    volatile unsigned int SW_KEY1;
    volatile unsigned int SW_KEY2;
    volatile unsigned int SW_KEY3;
    volatile U_SPDIF_TX_MUX SPDIF_TX_MUX;
    volatile unsigned int HW_VERSION;
    volatile U_AIAO_SWITCH_RX_BCLK AIAO_SWITCH_RX_BCLK;
    volatile U_AIAO_SWITCH_TX_BCLK AIAO_SWITCH_TX_BCLK;
    volatile U_AIAO_STATUS AIAO_STATUS;
    volatile unsigned int reserved0[((0x34 - 0x030) >> 2) - 1];
    volatile U_VHB_OUTSTANDING OUT_STANDING;
#ifdef HI_AIAO_TIMER_SUPPORT
    volatile unsigned int reserved1[((0x40 - 0x034) >> 2) - 1];
    volatile S_AIAO_TIER_CONFIG TIMER_CONFIG[2];
    volatile S_AIAO_TIER_CLEAR TIMER_CLEAR;
    volatile unsigned int reserved2[((0x100 - 0x048) >> 2) - 1];
    volatile S_AIAO_I2S_CRG_CFG_REGS_TYPE I2S_RX_CRG[6];
    volatile S_AIAO_TIER_CRG_CFG_REGS_TYPE TIMER_CRG[2];
    volatile S_AIAO_I2S_CRG_CFG_REGS_TYPE I2S_TX_CRG[8];
    volatile S_AIAO_SPDIF_CRG_CFG_REGS_TYPE SPDIF_CRG[3];
#else
    volatile unsigned int reserved1[((0x100 - 0x034) >> 2) - 1];
    volatile S_AIAO_I2S_CRG_CFG_REGS_TYPE I2S_RX_CRG[8];
    volatile S_AIAO_I2S_CRG_CFG_REGS_TYPE I2S_TX_CRG[8];
    volatile S_AIAO_SPDIF_CRG_CFG_REGS_TYPE SPDIF_CRG[4];
#endif
} S_AIAO_COM_REGS_TYPE;

typedef struct {
    volatile U_RX_IF_ATTRI RX_IF_ATTRI;
    volatile U_RX_DSP_CTRL RX_DSP_CTRL;
    volatile unsigned int reserved0[((0x1020 - 0x1004) >> 2) - 1];
    volatile U_RX_WS_CNT RX_WS_CNT;
    volatile U_RX_BCLK_CNT RX_BCLK_CNT;
    volatile unsigned int reserved1[((0x1080 - 0x1024) >> 2) - 1];
    volatile unsigned int RX_BUFF_SADDR;
    volatile U_RX_BUFF_SIZE RX_BUFF_SIZE;
    volatile U_RX_BUFF_WPTR RX_BUFF_WPTR;
    volatile U_RX_BUFF_RPTR RX_BUFF_RPTR;
    volatile U_RX_BUFF_ALFULL_TH RX_BUFF_ALFULL_TH;
    volatile U_RX_TRANS_SIZE RX_TRANS_SIZE;
    volatile U_RX_WPTR_TMP RX_WPTR_TMP;
    volatile unsigned int reserved2;
    volatile U_RX_INT_ENA RX_INT_ENA;
    volatile U_RX_INT_RAW RX_INT_RAW;
    volatile U_RX_INT_STATUS RX_INT_STATUS;
    volatile U_RX_INT_CLR RX_INT_CLR;
} S_AIAO_RX_REGS_TYPE;

typedef struct {
    volatile U_TX_IF_ATTRI TX_IF_ATTRI;
    volatile U_TX_DSP_CTRL TX_DSP_CTRL;
    volatile U_TX_DSP_STATUS TX_DSP_STATUS;
    volatile unsigned int reserved0[((0x2020 - 0x2008) >> 2) - 1];
    volatile U_TX_WS_CNT TX_WS_CNT;
    volatile U_TX_BCLK_CNT TX_BCLK_CNT;
    volatile unsigned int reserved1[((0x2080 - 0x2024) >> 2) - 1];
    volatile unsigned int TX_BUFF_SADDR;
    volatile U_TX_BUFF_SIZE TX_BUFF_SIZE;
    volatile U_TX_BUFF_WPTR TX_BUFF_WPTR;
    volatile U_TX_BUFF_RPTR TX_BUFF_RPTR;
    volatile U_TX_BUFF_ALEMPTY_TH TX_BUFF_ALEMPTY_TH;
    volatile U_TX_TRANS_SIZE TX_TRANS_SIZE;
    volatile U_TX_RPTR_TMP TX_RPTR_TMP;
    volatile unsigned int reserved2;
    volatile U_TX_INT_ENA TX_INT_ENA;
    volatile U_TX_INT_RAW TX_INT_RAW;
    volatile U_TX_INT_STATUS TX_INT_STATUS;
    volatile U_TX_INT_CLR TX_INT_CLR;
} S_AIAO_TX_REGS_TYPE;

typedef struct {
    volatile U_SPDIFTX_IF_ATTRI SPDIFTX_IF_ATTRI;
    volatile U_SPDIFTX_DSP_CTRL SPDIFTX_DSP_CTRL;
    volatile U_SPDIFTX_DSP_STATUS SPDIFTX_DSP_STATUS;
    volatile unsigned int reserved0[((0x3020 - 0x3008) >> 2) - 1];
    volatile U_SPDIFTX_WS_CNT SPDIFTX_WS_CNT;
    volatile U_SPDIFTX_BCLK_CNT SPDIFTX_BCLK_CNT;
    volatile unsigned int reserved1[((0x3080 - 0x3024) >> 2) - 1];
    volatile unsigned int SPDIFTX_BUFF_SADDR;
    volatile U_SPDIFTX_BUFF_SIZE SPDIFTX_BUFF_SIZE;
    volatile U_SPDIFTX_BUFF_WPTR SPDIFTX_BUFF_WPTR;
    volatile U_SPDIFTX_BUFF_RPTR SPDIFTX_BUFF_RPTR;
    volatile U_SPDIFTX_BUFF_ALEMPTY_TH SPDIFTX_BUFF_ALEMPTY_TH;
    volatile U_SPDIFTX_TRANS_SIZE SPDIFTX_TRANS_SIZE;
    volatile U_SPDIFTX_RPTR_TMP SPDIFTX_RPTR_TMP;
    volatile unsigned int reserved2;
    volatile U_SPDIFTX_INT_ENA SPDIFTX_INT_ENA;
    volatile U_SPDIFTX_INT_RAW SPDIFTX_INT_RAW;
    volatile U_SPDIFTX_INT_STATUS SPDIFTX_INT_STATUS;
    volatile U_SPDIFTX_INT_CLR SPDIFTX_INT_CLR;
} S_AIAO_TXSPDIF_REGS_TYPE;

typedef struct {
    volatile U_SPDIF_CTRL SPDIF_CTRL;
    volatile U_SPDIF_CONFIG SPDIF_CONFIG;
    volatile U_SPDIF_INTRUPT SPDIF_INTRUPT;
    volatile U_SPDIF_INTR_MASK SPDIF_INTR_MASK;
    volatile U_SPDIF_INTR_STATE SPDIF_INTR_STATE;
    volatile unsigned int reserved0[((0x4020 - 0x4010) >> 2) - 1];
    volatile U_SPDIF_CH_STATUS1 SPDIF_CH_STATUS1;
    volatile U_SPDIF_CH_STATUS2 SPDIF_CH_STATUS2;
    volatile U_SPDIF_CH_STATUS3 SPDIF_CH_STATUS3;
    volatile U_SPDIF_CH_STATUS4 SPDIF_CH_STATUS4;
    volatile U_SPDIF_CH_STATUS5 SPDIF_CH_STATUS5;
    volatile unsigned int reserved1[((0x4040 - 0x4030) >> 2) - 1];
    volatile unsigned int SPDIF_USER_DATA1;
    volatile unsigned int SPDIF_USER_DATA2;
    volatile unsigned int SPDIF_USER_DATA3;
    volatile unsigned int SPDIF_USER_DATA4;
    volatile unsigned int SPDIF_USER_DATA5;
    volatile unsigned int reserved2[((0x40f0 - 0x4050) >> 2) - 1];
    volatile unsigned int SPDIF_EDITION;
} S_AIAO_SPDIFER_REGS_TYPE;

/*****************************************************************************
 description  : AIAO golbal HAL API
*****************************************************************************/
hi_s32 aiao_hw_power_on(hi_void);
hi_void aiao_hw_power_off(hi_void);
hi_s32 aiao_hw_init(hi_void);
hi_void aiao_hw_de_init(hi_void);
hi_void aiao_hw_get_hw_capability(hi_u32 *capability);
hi_void aiao_hw_get_hw_version(hi_u32 *version);
hi_void aiao_hw_dbg_rw_reg(aiao_dbg_reg *reg);

hi_void aiao_hw_set_top_int(hi_u32 multibit);
hi_u32 aiao_hw_get_top_int_raw_status(hi_void);
hi_u32 aiao_hw_get_top_int_status(hi_void);

/*****************************************************************************
 description  : AIAO rx/tx port CRG/i2s attr HAL API
*****************************************************************************/
hi_void aiao_hw_set_if_attr(aiao_port_id port_id, aiao_if_attr *if_attr);
hi_void aiao_hw_set_spdif_port_en(aiao_port_id port_id, hi_s32 en);
hi_void aiao_hw_set_spdif_port_select(aiao_port_id port_id, aiao_spdifport_source src_chn_id);
hi_void aiao_hw_set_i2s_data_select(aiao_port_id port_id, aiao_i2s_sd org_sd, aiao_i2s_sd src_sd);

/*****************************************************************************
 description  : AIAO TX/RX port DSP control HAL API
*****************************************************************************/
hi_void aiao_hw_set_int(aiao_port_id port_id, hi_u32 multibit);
hi_void aiao_hw_clr_int(aiao_port_id port_id, hi_u32 multibit);
hi_u32 aiao_hw_get_int_status_raw(aiao_port_id port_id);
hi_u32 aiao_hw_get_int_status(aiao_port_id port_id);

/*****************************************************************************
 description  : AIAO TX/RX port DSP control HAL API
*****************************************************************************/
hi_s32 aiao_hw_set_start(aiao_port_id port_id, hi_s32 en);
hi_s32 aiao_hw_get_stop_done_status(aiao_port_id port_id);
hi_void aiao_hw_set_bypass(aiao_port_id port_id, hi_s32 en);
hi_void aiao_hw_set_mode(aiao_port_id port_id, hi_s32 en);
hi_void aiao_hw_set_volume_db(aiao_port_id port_id, hi_s32 vol_db);
hi_void aiao_hw_set_spdif_category_scms(aiao_port_id port_id, aiao_spdif_scms_mode scms_mode,
    aiao_spdif_categorycode category_code);
hi_void aiao_hw_set_fade_out_rate(aiao_port_id port_id, aiao_fade_rate fade_rate);
hi_void aiao_hw_set_fade_in_rate(aiao_port_id port_id, aiao_fade_rate fade_rate);
hi_void aiao_hw_set_mute_fade(aiao_port_id port_id, hi_s32 en);
hi_void aiao_hw_set_mute(aiao_port_id port_id, hi_s32 en);
hi_void aiao_hw_set_track_mode(aiao_port_id port_id, aiao_track_mode track_mode);
hi_void aiao_hw_set_i2s_master_clk(aiao_port_id port_id, aiao_if_attr *if_attr_master);
hi_void aiao_hw_set_i2s_slave_clk(aiao_port_id port_id, aiao_if_attr *if_attr_slave);

/*****************************************************************************
 description  : AIAO TX/RX port buffer HAL API
*****************************************************************************/
hi_void aiao_hw_get_rptr_and_wptr_reg_addr(aiao_port_id port_id, hi_u32 **wptr_reg,
                                           hi_u32 **rptr_reg);

hi_void aiao_hw_get_rptr_and_wptr_reg_phy_addr(aiao_port_id port_id, hi_u64 *wptr_reg, hi_u64 *rptr_reg);
hi_void aiao_hw_get_bufu32_wptr(aiao_port_id port_id, hi_u32 *wptr);
hi_void aiao_hw_get_bufu32_rptr(aiao_port_id port_id, hi_u32 *rptr);
hi_void aiao_hw_set_buf_addr_and_size(aiao_port_id port_id, hi_u32 start_addr, hi_u32 size);
hi_void aiao_hw_set_buf_wptr(aiao_port_id port_id, hi_u32 wptr);
hi_void aiao_hw_set_buf_rptr(aiao_port_id port_id, hi_u32 rptr);
hi_void aiao_hw_set_buf(aiao_port_id port_id, aiao_buf_info *buf);
hi_void aiao_hw_get_dbg_bclk_cnt(aiao_port_id port_id, hi_u32 *bclk_cnt);
hi_void aiao_hw_get_dbg_ws_cnt(aiao_port_id port_id, hi_u32 *ws_cnt);
hi_void aiao_hw_set_op_type(aiao_port_id port_id, aiao_op_type op_type);
#ifdef HI_AIAO_TIMER_SUPPORT
hi_void aiao_timer_set_enable(aiao_timer_id timer_id, hi_s32 en);
hi_void aiao_timer_set_attr(aiao_timer_id timer_id, const aiao_if_timer_attr *if_attr, hi_u32 config);
hi_void aiao_timer_clear_timer(aiao_timer_id timer_id);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HAL_AIAO_PRIV_H__ */
