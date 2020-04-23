/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: register definition of audio module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_AUDIO_REG_H__
#define __HAL_HDMIRX_AUDIO_REG_H__

#define REG_CFG_AUD_INT 0x3000
#define INTR_AAC_AUDIO_MUTE   0x1     /* mask bit 0 */
#define INTR_AAC_AUDIO_UNMUTE 0x2     /* mask bit 1 */
#define INTR_HDMI_ERR         0x4     /* mask bit 2 */
#define INTR_ACR_PKT_ERR      0x8     /* mask bit 3 */
#define INTR_ACR_N_CHANGE     0x10    /* mask bit 4 */
#define INTR_ACR_CTS_CHANGE   0x20    /* mask bit 5 */
#define INTR_CTS_REUSED_ERR   0x40    /* mask bit 6 */
#define INTR_CTS_DROPPED_ERR  0x80    /* mask bit 7 */
#define INTR_AUDIO_FLAT       0x100   /* mask bit 8 */
#define INTR_AUD_CHST_READY   0x200   /* mask bit 9 */
#define INTR_AUD_CHST_CHANGE  0x400   /* mask bit 10 */
#define INTR_LR_MATCH         0x800   /* mask bit 11 */
#define INTR_FIFO_OVERUN_AF   0x1000  /* mask bit 12 */
#define INTR_FIFO_OVERUN_BF   0x2000  /* mask bit 13 */
#define INTR_FIFO_UNDERUN_AF  0x4000  /* mask bit 14 */
#define INTR_FIFO_UNDERUN_BF  0x8000  /* mask bit 15 */
#define INTR_P_ERR            0x10000 /* mask bit 16 */
#define INTR_CTS_ADJ_ERR      0x20000 /* mask bit 17 */
#define INTR_PRE_FS_CHANGE    0x40000 /* mask bit 18 */

#define AUDIO_CFG_INT_MASK 0x3004
#define INTR_AAC_AUDIO_MUTE_MASK   0x1     /* mask bit 0 */
#define INTR_AAC_AUDIO_UNMUTE_MASK 0x2     /* mask bit 1 */
#define INTR_HDMI_ERR_MASK         0x4     /* mask bit 2 */
#define INTR_ACR_PKT_ERR_MASK      0x8     /* mask bit 3 */
#define INTR_ACR_N_CHANGE_MASK     0x10    /* mask bit 4 */
#define INTR_ACR_CTS_CHANGE_MASK   0x20    /* mask bit 5 */
#define INTR_CTS_REUSED_ERR_MASK   0x40    /* mask bit 6 */
#define INTR_CTS_DROPPED_ERR_MASK  0x80    /* mask bit 7 */
#define INTR_AUDIO_FLAT_MASK       0x100   /* mask bit 8 */
#define INTR_AUD_CHST_READY_MASK   0x200   /* mask bit 9 */
#define INTR_AUD_CHST_CHANGE_MASK  0x400   /* mask bit 10 */
#define INTR_LR_MATCH_MASK         0x800   /* mask bit 11 */
#define INTR_FIFO_OVERUN_AF_MASK   0x1000  /* mask bit 12 */
#define INTR_FIFO_OVERUN_BF_MASK   0x2000  /* mask bit 13 */
#define INTR_FIFO_UNDERUN_AF_MASK  0x4000  /* mask bit 14 */
#define INTR_FIFO_UNDERUN_BF_MASK  0x8000  /* mask bit 15 */
#define INTR_P_ERR_MASK            0x10000 /* mask bit 16 */
#define INTR_CTS_ADJ_ERR_MASK      0x20000 /* mask bit 17 */
#define INTR_PRE_FS_CHANGE_MASK    0x40000 /* mask bit 18 */

#define REG_AUD_INT_STAT 0x3008
#define HDMI_ERR_STAT           0x1  /* mask bit 0 */
#define LR_MATCH_STA            0x2  /* mask bit 1 */
#define AUDIO_FLAT_STAT         0x4  /* mask bit 2 */
#define FIFO_UNDERUN_THRES_STAT 0x8  /* mask bit 3 */
#define FIFO_OVERUN_THRES_STAT  0x10 /* mask bit 4 */

#define REG_CFG_ACR_CTRL 0x301C
#define REG_ACR_INIT            0x1 /* mask bit 0 */
#define REG_CTS_REUSED_AUTO_EN  0x2 /* mask bit 1 */
#define REG_CTS_DROPPED_AUTO_EN 0x4 /* mask bit 2 */

#define REG_CFG_MCLK_SEL 0x3020
#define REG_MCLK4HBRA 0x3 /* mask bit 1:0 */
#define REG_MCLK4DAC  0xC /* mask bit 3:2 */

#define REG_CFG_N_SVAL 0x3028
#define REG_N_VAL_SW    0xFFFFF  /* mask bit 19:0 */
#define REG_N_HW_SW_SEL 0x100000 /* mask bit 20 */

#define REG_CFG_N_HVAL 0x302C
#define N_VAL_HW 0xFFFFF /* mask bit 19:0 */

#define REG_CFG_CTS_SVAL 0x3030
#define REG_CTS_VAL_SW    0xFFFFF  /* mask bit 19:0 */
#define REG_CTS_HW_SW_SEL 0x100000 /* mask bit 20 */

#define REG_CFG_CTS_HVAL 0x3034
#define CTS_VAL_HW 0xFFFFF /* mask bit 19:0 */

#define REG_CFG_CTS_THRE 0x3038
#define REG_CTS_THRESH 0xF /* mask bit 3:0 */

#define REG_CFG_AUD_EN 0x305C
#define CFG_MUTE_AF_AAC 0xF   /* mask bit 3:0 */
#define REG_SD0_EN      0x10  /* mask bit 4 */
#define REG_SD1_EN      0x20  /* mask bit 5 */
#define REG_SD2_EN      0x40  /* mask bit 6 */
#define REG_SD3_EN      0x80  /* mask bit 7 */
#define REG_I2S_MODE    0x100 /* mask bit 8 */

#define REG_AAC_STATE 0x3060
#define AAC_MUTE_STAT         0x1 /* mask bit 0 */
#define AAC_FSM_CURRENT_STATE 0xE /* mask bit 3:1 */

#define REG_CFG_AAC 0x3064
#define CFG_AIAO_MUTESTAT_EN 0x1   /* mask bit 0 */
#define CFG_AIAO_MUTESTAT    0x2   /* mask bit 1 */
#define CFG_AAC_AUTO_MUTE_EN 0x4   /* mask bit 2 */
#define CFG_AUDIO_AAC_MUTE   0x8   /* mask bit 3 */
#define CFG_VSYNC_CNT        0x3F0 /* mask bit 9:4 */
#define CFG_AUDIO_AAC_UNMUTE 0x400 /* mask bit 10 */

#define REG_CFG_AUD_HOLD 0x3068
#define REG_CH0_MUTE   0x1  /* mask bit 0 */
#define REG_CH1_MUTE   0x2  /* mask bit 1 */
#define REG_CH2_MUTE   0x4  /* mask bit 2 */
#define REG_CH3_MUTE   0x8  /* mask bit 3 */
#define REG_AUDIO_MUTE 0x10 /* mask bit 4 */

#define REG_CFG_EXCEP_CHECK_TIME 0x306C
#define CFG_EXCEP_CHECK_TIME 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CFG_EXCEP_DUTY_CNT 0x3070
#define CFG_CONTI_EXCEPT_DUTY_CNT 0xFF   /* mask bit 7:0 */
#define CFG_CONTI_NORMAL_DUTY_CNT 0xFF00 /* mask bit 15:8 */

#define REG_CFG_I2S_CTRL1 0x308C
#define REG_1ST_BIT           0x1      /* mask bit 0 */
#define REG_DATA_DIR          0x2      /* mask bit 1 */
#define REG_JUSTIFY           0x4      /* mask bit 2 */
#define REG_WS                0x8      /* mask bit 3 */
#define REG_MSB               0x10     /* mask bit 4 */
#define REG_SIZE              0x20     /* mask bit 5 */
#define REG_CLK_EDGE          0x40     /* mask bit 6 */
#define REG_INVALID_EN        0x80     /* mask bit 7 */
#define REG_I2S_LENGTH_EN     0x100    /* mask bit 8 */
#define REG_I2S_LENGTH        0x1E00   /* mask bit 12:9 */
#define REG_SWAP              0x1E000  /* mask bit 16:13 */
#define REG_SPDIF4I2S_EN      0x20000  /* mask bit 17 */
#define REG_AUDIO_TOGGLE_MODE 0x40000  /* mask bit 18 */
#define REG_PCM               0x80000  /* mask bit 19 */
#define REG_VUCP              0x100000 /* mask bit 20 */
#define CFG_STOP_SCLK_EN      0x200000 /* mask bit 21 */
#define CFG_STOP_WS_EN        0x400000 /* mask bit 22 */

#define REG_CFG_I2S_CTRL2 0x3090
#define REG_SD0_MAP 0x3  /* mask bit 1:0 */
#define REG_SD1_MAP 0xC  /* mask bit 3:2 */
#define REG_SD2_MAP 0x30 /* mask bit 5:4 */
#define REG_SD3_MAP 0xC0 /* mask bit 7:6 */

#define REG_RX_CHST1 0x30B4
#define CHST_BIT0         0x1        /* mask bit 0 */
#define CHST_BIT1         0x2        /* mask bit 1 */
#define CHST_BIT2         0x4        /* mask bit 2 */
#define CHST_BIT3_4_5     0x38       /* mask bit 5:3 */
#define CHST_BIT6_7       0xC0       /* mask bit 7:6 */
#define CHST_BIT15_8      0xFF00     /* mask bit 15:8 */
#define AUD_SOURCE1       0xF0000    /* mask bit 19:16 */
#define AUD_CH_NUM1       0xF00000   /* mask bit 23:20 */
#define AUD_SAMPLE_F      0xF000000  /* mask bit 27:24 */
#define AUD_ACCURACY      0x30000000 /* mask bit 29:28 */
#define AUD_SAMPLE_F_EXTN 0xC0000000 /* mask bit 31:30 */

#define REG_RX_CHST2 0x30B8
#define AUD_LENGTH_MAX     0x1      /* mask bit 0 */
#define AUD_LENGTH         0xE      /* mask bit 3:1 */
#define AUD_ORG_FS         0xF0     /* mask bit 7:4 */
#define CGMS_A             0x300    /* mask bit 9:8 */
#define CGMS_A_VALIDITY    0x400    /* mask bit 10 */
#define CHST_BIT43         0x800    /* mask bit 11 */
#define AUD_SAMPLE_F_COEFF 0xF000   /* mask bit 15:12 */
#define INFO_IN_PCM_SIGNAL 0x10000  /* mask bit 16 */
#define CHST_BIT55_49      0xFE0000 /* mask bit 23:17 */

#define REG_CFG_UNDERUN_RST_EN 0x30E4
#define CFG_UNDERUN_RST_AUTO_EN 0x1 /* mask bit 0 */

#define REG_CFG_OVERUN_RST_EN 0x30E8
#define CFG_OVERUN_RST_AUTO_EN 0x1 /* mask bit 0 */

#define REG_CFG_FIFO_WRITE 0x30EC
#define CFG_FIFO_INI_STOR_COUNT 0x3F   /* mask bit 5:0 */
#define CFG_SP_LAYOUTIS0_MASK   0x3C0  /* mask bit 9:6 */
#define CFG_SP_LAYOUTIS1_MASK   0x3C00 /* mask bit 13:10 */

#define REG_CFG_SP_LOW_SDEAL 0x30F0
#define CFG_SP_LOW_SUBPKT_SDEAL_MASK 0xF /* mask bit 3:0 */

#define REG_CFG_LAYOUT_OW 0x30F4
#define CFG_LAYOUT_OW_EN 0x1 /* mask bit 0 */
#define CFG_LAYOUT       0x6 /* mask bit 2:1 */

#define REG_CFG_FS_EXTR 0x30F8
#define CFG_FS_EXTN_MASK 0x1 /* mask bit 0 */
#define REG_PASS_AUD_ERR 0x2 /* mask bit 1 */
#define REG_FS_FILTER_EN 0x4 /* mask bit 2 */
#define REG_MUTE_FLAT    0x8 /* mask bit 3 */

#define REG_REG_FS_PRE 0x30FC
#define AUD_FS_PRE      0xF  /* mask bit 3:0 */
#define AUD_FS_PRE_EXTN 0x30 /* mask bit 5:4 */

#define REG_REG_FIFO_INI_FIN 0x3100
#define HDMI_FIFO_INI_CNT_ENOUGH 0x1 /* mask bit 0 */

#define REG_REG_FIFO_CNT 0x3104
#define HDMI_FIFO_DIFF 0x7F /* mask bit 6:0 */

#define REG_CFG_OBS_TIME 0x3128
#define CFG_OBS_TIME 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CFG_FIFO_ADJ 0x312C
#define CFG_FIFO_COUNT_ADJ_EN 0x1 /* mask bit 0 */
#define CFG_OBS_RP_SEL        0x2 /* mask bit 1 */

#define REG_CFG_FIFO_TIME_ADJ 0x3130
#define CFG_FIFO_CHECK_TIME 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CFG_FIFO_MAX_THRESH 0x3134
#define CFG_FIFO_MAX_COUNT_ADJ_THRE 0x7F /* mask bit 6:0 */

#define REG_CFG_FIFO_MIN_THRESH 0x3138
#define CFG_FIFO_MIN_COUNT_ADJ_THRE 0x7F /* mask bit 6:0 */

#define REG_CFG_FIFO_ADJ_DELTA0 0x313C
#define CFG_FIFO_MAX_1ST_CTS_DELTA 0xFFFF     /* mask bit 15:0 */
#define CFG_FIFO_MAX_INC_CTS_DELTA 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_FIFO_ADJ_DELTA1 0x3140
#define CFG_FIFO_MAX_UNCHANGE_CTS_DELTA 0xFFFF     /* mask bit 15:0 */
#define CFG_FIFO_MIN_1ST_CTS_DELTA      0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_FIFO_ADJ_DELTA2 0x3144
#define CFG_FIFO_MIN_UNCHANGE_CTS_DELTA 0xFFFF     /* mask bit 15:0 */
#define CFG_FIFO_MIN_DECR_CTS_DELTA     0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_FIFO_ADJ_MAX_LIMIT 0x3148
#define CFG_FIFO_MAX_CTS_DELTA 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CFG_FIFO_ADJ_MIN_LIMIT 0x314C
#define CFG_FIFO_MIN_CTS_DELTA 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CFG_FIFO_ADJ_CON0 0x3150
#define CTS_ADJ_FSM_STATE 0x3 /* mask bit 1:0 */

#define REG_CFG_FIFO_ADJ_CON1 0x3154
#define CTS_VAL_ADJ 0xFFFFF /* mask bit 19:0 */

#define REG_CFG_AUDIO_PATTERN_CHECN_EN 0x3158
#define CFG_AUDIO_PATTERN_CHECK_EN_SD0 0x1 /* mask bit 0 */
#define CFG_AUDIO_PATTERN_CHECK_EN_SD1 0x2 /* mask bit 1 */
#define CFG_AUDIO_PATTERN_CHECK_EN_SD2 0x4 /* mask bit 2 */
#define CFG_AUDIO_PATTERN_CHECK_EN_SD3 0x8 /* mask bit 3 */

#define REG_AUDIO_PATTERN_PBRS_SD0 0x315C
#define AUD_PRBS_ERR_CNT_SD0 0xFFFF  /* mask bit 15:0 */
#define AUD_PRBS_PAT_SD0     0x10000 /* mask bit 16 */

#define REG_AUDIO_PATTERN_PBRS_SD1 0x3160
#define AUD_PRBS_ERR_CNT_SD1 0xFFFF  /* mask bit 15:0 */
#define AUD_PRBS_PAT_SD1     0x10000 /* mask bit 16 */

#define REG_AUDIO_PATTERN_PBRS_SD2 0x3164
#define AUD_PRBS_ERR_CNT_SD2 0xFFFF  /* mask bit 15:0 */
#define AUD_PRBS_PAT_SD2     0x10000 /* mask bit 16 */

#define REG_AUDIO_PATTERN_PBRS_SD3 0x3168
#define AUD_PRBS_ERR_CNT_SD3 0xFFFF  /* mask bit 15:0 */
#define AUD_PRBS_PAT_SD3     0x10000 /* mask bit 16 */
#endif /* __HAL_HDMIRX_AUDIO_REG_H__ */
