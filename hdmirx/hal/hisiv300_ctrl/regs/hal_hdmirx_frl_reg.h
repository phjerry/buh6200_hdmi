/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of frl module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_FRL_H__
#define __HAL_HDMIRX_FRL_H__

#define REG_FRL_INTSTA0 0xB000
#define SOURCE_FLT_INT          0x1  /* mask bit 0 */
#define SINK_FLT_INT            0x2  /* mask bit 1 */
#define TMDS_OK_INT             0x4  /* mask bit 2 */
#define FRL_OK_INT              0x8  /* mask bit 3 */
#define FLT_TIMEOUT_INT         0x10 /* mask bit 4 */
#define FLT_READY_ERR_INT       0x20 /* mask bit 5 */
#define SOURCE_POLL_TIMEOUT_INT 0x40 /* mask bit 6 */
#define SOURCE_CLR_TIMEOUT_INT  0x80 /* mask bit 7 */

#define REG_FRL_INTSTA1 0xB004
#define RSCC_INT           0x2  /* mask bit 1 */
#define FFE_LEVEL_INT      0x8  /* mask bit 3 */
#define FLT_NO_RETRAIN_INT 0x10 /* mask bit 4 */
#define RSED_UPDATE_INT    0x20 /* mask bit 5 */
#define FLT_UPDATE_INT     0x40 /* mask bit 6 */
#define FRL_START_INT      0x80 /* mask bit 7 */

#define REG_FRL_INTSTA2 0xB008
#define STATUS_UPDATE_INT   0x1  /* mask bit 0 */
#define FRL_RATE_INT        0x2  /* mask bit 1 */
#define SFIFO_OVER_FLOW_INT 0x4  /* mask bit 2 */
#define RS_ENCORR_OVER_INT  0x8  /* mask bit 3 */
#define RS_UNCORR_OVER_INT  0x10 /* mask bit 4 */

#define REG_FRL_INTSTA3 0xB00C
#define LN3_NORM_DATA_ERR_EXC_INT 0x1  /* mask bit 0 */
#define LN3_LTP_DATA_ERR_EXC_INT  0x2  /* mask bit 1 */
#define LN2_NORM_DATA_ERR_EXC_INT 0x4  /* mask bit 2 */
#define LN2_LTP_DATA_ERR_EXC_INT  0x8  /* mask bit 3 */
#define LN1_NORM_DATA_ERR_EXC_INT 0x10 /* mask bit 4 */
#define LN1_LTP_DATA_ERR_EXC_INT  0x20 /* mask bit 5 */
#define LN0_NORM_DATA_ERR_EXC_INT 0x40 /* mask bit 6 */
#define LN0_LTP_DATA_ERR_EXC_INT  0x80 /* mask bit 7 */

#define REG_FRL_INTSTA4 0xB010
#define BK_PREAMBLE_ERR_INT 0x1  /* mask bit 0 */
#define AV_PREAMBLE_ERR_INT 0x2  /* mask bit 1 */
#define DATA_ISLAND_ERR_INT 0x4  /* mask bit 2 */
#define AV_GB_ERR_INT       0x8  /* mask bit 3 */
#define BK_GB_ERR_INT       0x10 /* mask bit 4 */
#define CTRL_PERIOD_ERR_INT 0x20 /* mask bit 5 */
#define RC_ERR_INT          0x40 /* mask bit 6 */

#define REG_FRL_INTMASK0 0xB014
#define SOURCE_FLT_INTMASK          0x1  /* mask bit 0 */
#define SINK_FLT_INTMASK            0x2  /* mask bit 1 */
#define TMDS_OK_INTMASK             0x4  /* mask bit 2 */
#define FRL_OK_INTMASK              0x8  /* mask bit 3 */
#define FLT_TIMEOUT_INTMASK         0x10 /* mask bit 4 */
#define  FLT_READY_INTMASK0         0x20 /* mask bit 5 */
#define SOURCE_POLL_TIMEOUT_INTMASK 0x40 /* mask bit 6 */
#define SOURCE_CLR_TIMEOUT_INTMASK  0x80 /* mask bit 7 */

#define REG_FRL_INTMASK1 0xB018
#define RS_C_VALID_INTMASK     0x1  /* mask bit 0 */
#define RSCC_INTMASK           0x2  /* mask bit 1 */
#define FLT_READY_INTMASK1     0x4  /* mask bit 2 */
#define FFE_LEVEL_INTMASK      0x8  /* mask bit 3 */
#define FLT_NO_RETRAIN_INTMASK 0x10 /* mask bit 4 */
#define RSED_UPDATE_INTMASK    0x20 /* mask bit 5 */
#define FLT_UPDATE_INTMASK     0x40 /* mask bit 6 */
#define FRL_START_INTMASK      0x80 /* mask bit 7 */

#define REG_FRL_INTMASK2 0xB01C
#define STATUS_UPDATE_INTMASK   0x1  /* mask bit 0 */
#define FRL_RATE_INTMASK        0x2  /* mask bit 1 */
#define SFIFO_OVER_FLOW_INTMASK 0x4  /* mask bit 2 */
#define RS_ENCORR_OVER_INTMASK  0x8  /* mask bit 3 */
#define RS_UNCORR_OVER_INTMASK  0x10 /* mask bit 4 */
#define RSVD_BIT2               0xE0 /* mask bit 7:5 */

#define REG_FRL_INTMASK3 0xB020
#define LN3_NORM_DATA_ERR_EXC_INTMASK 0x1  /* mask bit 0 */
#define LN3_LTP_DATA_ERR_EXC_INTMASK  0x2  /* mask bit 1 */
#define LN2_NORM_DATA_ERR_EXC_INTMASK 0x4  /* mask bit 2 */
#define LN2_LTP_DATA_ERR_EXC_INTMASK  0x8  /* mask bit 3 */
#define LN1_NORM_DATA_ERR_EXC_INTMASK 0x10 /* mask bit 4 */
#define LN1_LTP_DATA_ERR_EXC_INTMASK  0x20 /* mask bit 5 */
#define LN0_NORM_DATA_ERR_EXC_INTMASK 0x40 /* mask bit 6 */
#define LN0_LTP_DATA_ERR_EXC_INTMASK  0x80 /* mask bit 7 */

#define REG_FRL_INTMASK4 0xB024
#define BK_PREAMBLE_ERR_INTMASK 0x1  /* mask bit 0 */
#define AV_PREAMBLE_ERR_INTMASK 0x2  /* mask bit 1 */
#define DATA_ISLAND_ERR_INTMASK 0x4  /* mask bit 2 */
#define AV_GB_ERR_INTMASK       0x8  /* mask bit 3 */
#define BK_GB_ERR_INTMASK       0x10 /* mask bit 4 */
#define CTRL_PERIOD_ERR_INTMASK 0x20 /* mask bit 5 */
#define RC_ERR_INTMASK          0x40 /* mask bit 6 */
#define RSVD_BIT4               0x80 /* mask bit 7 */

#define REG_FRL_CTRL 0xB028
#define FLT_HW_BYPASS  0x1  /* mask bit 0 */
#define FRL_OK         0x2  /* mask bit 1 */
#define TMDS_OK        0x4  /* mask bit 2 */
#define CPU_RETRAIN    0x8  /* mask bit 3 */
#define SOFT_AV_MUTE   0x10 /* mask bit 4 */
#define SOFT_FRL_START 0x20 /* mask bit 5 */
#define RS_ERR_MASK    0x40 /* mask bit 6 */
#define FLT_HW_INIT_EN 0x80 /* mask bit 7 */

#define REG_FLT_TIMEOUT0_THD 0xB030
#define FLT_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_FLT_TIMEOUT1_THD 0xB034
#define FLT_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_FLT_TIMEOUT2_THD 0xB038
#define FLT_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_FLT_TIMEOUT3_THD 0xB03C
#define FLT_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_CLR_TIMEOUT0_THD 0xB040
#define CLR_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_CLR_TIMEOUT1_THD 0xB044
#define CLR_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_CLR_TIMEOUT2_THD 0xB048
#define CLR_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_CLR_TIMEOUT3_THD 0xB04C
#define CLR_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_POLL_TIMEOUT0_THD 0xB050
#define POLL_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_POLL_TIMEOUT1_THD 0xB054
#define POLL_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_POLL_TIMEOUT2_THD 0xB058
#define POLL_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_POLL_TIMEOUT3_THD 0xB05C
#define POLL_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_PTT0_CHK_TIMEOUT0_THD 0xB060
#define PTT0_CHK_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_PTT0_CHK_TIMEOUT1_THD 0xB064
#define PTT0_CHK_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_PTT0_CHK_TIMEOUT2_THD 0xB068
#define PTT0_CHK_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_PTT0_CHK_TIMEOUT3_THD 0xB06C
#define PTT0_CHK_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_PTT1_CHK_TIMEOUT0_THD 0xB070
#define PTT1_CHK_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_PTT1_CHK_TIMEOUT1_THD 0xB074
#define PTT1_CHK_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_PTT1_CHK_TIMEOUT2_THD 0xB078
#define PTT1_CHK_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_PTT1_CHK_TIMEOUT3_THD 0xB07C
#define PTT1_CHK_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_PTT2_CHK_TIMEOUT0_THD 0xB080
#define PTT2_CHK_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_PTT2_CHK_TIMEOUT1_THD 0xB084
#define PTT2_CHK_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_PTT2_CHK_TIMEOUT2_THD 0xB088
#define PTT2_CHK_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_PTT2_CHK_TIMEOUT3_THD 0xB08C
#define PTT2_CHK_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_PTT3_CHK_TIMEOUT0_THD 0xB090
#define PTT3_CHK_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_PTT3_CHK_TIMEOUT1_THD 0xB094
#define PTT3_CHK_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_PTT3_CHK_TIMEOUT2_THD 0xB098
#define PTT3_CHK_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_PTT3_CHK_TIMEOUT3_THD 0xB09C
#define PTT3_CHK_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_GAP_CHK_TIMEOUT0_THD 0xB0A0
#define GAP_CHK_TIMEOUT_THD0 0xFF /* mask bit 7:0 */

#define REG_GAP_CHK_TIMEOUT1_THD 0xB0A4
#define GAP_CHK_TIMEOUT_THD1 0xFF /* mask bit 7:0 */

#define REG_GAP_CHK_TIMEOUT2_THD 0xB0A8
#define GAP_CHK_TIMEOUT_THD2 0xFF /* mask bit 7:0 */

#define REG_GAP_CHK_TIMEOUT3_THD 0xB0AC
#define GAP_CHK_TIMEOUT_THD3 0xFF /* mask bit 7:0 */

#define REG_LANE_INIT_LTP0 0xB0B0
#define LANE0_INIT_LTP 0xF  /* mask bit 3:0 */
#define LANE1_INIT_LTP 0xF0 /* mask bit 7:4 */

#define REG_LANE_INIT_LTP1 0xB0B4
#define LANE2_INIT_LTP 0xF  /* mask bit 3:0 */
#define LANE3_INIT_LTP 0xF0 /* mask bit 7:4 */

#define REG_FLT_FSM_STATE 0xB0C0
#define FSM_STATE 0x7 /* mask bit 2:0 */

#define REG_PTT_CHK_STATE 0xB0C4
#define LANE0_PTT_CHK 0x3  /* mask bit 1:0 */
#define LANE1_PTT_CHK 0xC  /* mask bit 3:2 */
#define LANE2_PTT_CHK 0x30 /* mask bit 5:4 */
#define LANE3_PTT_CHK 0xC0 /* mask bit 7:6 */

#define REG_GAP_CHK_STATE 0xB0C8
#define GAP_CHK 0x3 /* mask bit 1:0 */

#define REG_TRAIN_DEBUG0 0xB0D0
#define TRAIN_DEBUG0 0xFF /* mask bit 7:0 */

#define REG_TRAIN_DEBUG1 0xB0D4
#define TRAIN_DEBUG1 0xFF /* mask bit 7:0 */

#define REG_MCU_UART_CTRL 0xB0D8
#define MCU_UART_BAUD_UNIT      0xFFF  /* mask bit 11:0 */
#define FRL_MCU_UART_RATE_SEL   0x3000 /* mask bit 13:12 */
#define FRL_MCU_UART_STOP_SEL   0x4000 /* mask bit 14 */

#define REG_FRL_18B16B_10B_LUT 0xB100
#define LUT_9B_DATA_B0 0x1FF      /* mask bit 8:0 */
#define LUT_9B_VLD_B0  0x8000     /* mask bit 15 */
#define LUT_9B_DATA_B1 0x1FF0000  /* mask bit 24:16 */
#define LUT_9B_VLD_B1  0x80000000 /* mask bit 31 */

#define REG_FRL_18B16B_8B_LUT 0xB900
#define LUT_7B_DATA_B0 0x7F       /* mask bit 6:0 */
#define LUT_7B_VLD_B0  0x8000     /* mask bit 15 */
#define LUT_7B_DATA_B1 0x7F0000   /* mask bit 22:16 */
#define LUT_7B_VLD_B1  0x80000000 /* mask bit 31 */

#define REG_FRL_18B16B_CTRL 0xBB00
#define FRL_CFG_LUT_EN 0xF  /* mask bit 3:0 */
#define FRL_DEC_GT_EN  0x10 /* mask bit 4 */
#define LUT_RD_MEM_SEL 0x60 /* mask bit 6:5 */

#define REG_FRL_18B16B_LTP_ERR_TH0 0xBB04
#define DEC_LTP_ERR_NUM_TH_H 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LTP_ERR_TH1 0xBB08
#define DEC_LTP_ERR_NUM_TH_L 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_NORM_ERR_TH0 0xBB0C
#define DEC_NORM_ERR_NUM_TH_H 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_NORM_ERR_TH1 0xBB10
#define DEC_NORM_ERR_NUM_TH_L 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_10B_CHK_EN 0xBB14
#define LUT_10B9B_CHKSUM_EN 0x1 /* mask bit 0 */

#define REG_FRL_18B16B_10B_CHKSUM 0xBB18
#define LUT_10B9B_CHKSUM 0xFFFFF /* mask bit 19:0 */

#define REG_FRL_18B16B_8B_CHK_EN 0xBB1C
#define LUT_8B7B_CHKSUM_EN 0x1 /* mask bit 0 */

#define REG_FRL_18B16B_8B_CHKSUM 0xBB20
#define LUT_8B7B_CHKSUM 0xFFFF /* mask bit 15:0 */

#define REG_FRL_18B16B_NUM_CLR 0xBB24
#define LN0_DATA_ERR_NUM_CLR 0x1 /* mask bit 0 */
#define LN1_DATA_ERR_NUM_CLR 0x2 /* mask bit 1 */
#define LN2_DATA_ERR_NUM_CLR 0x4 /* mask bit 2 */
#define LN3_DATA_ERR_NUM_CLR 0x8 /* mask bit 3 */

#define REG_FRL_18B16B_LN0_ERR_NUM0 0xBB28
#define LN0_DATA_ERR_NUM_H 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LN0_ERR_NUM1 0xBB2C
#define LN0_DATA_ERR_NUM_L 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LN1_ERR_NUM0 0xBB30
#define LN1_DATA_ERR_NUM_H 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LN1_ERR_NUM1 0xBB34
#define LN1_DATA_ERR_NUM_L 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LN2_ERR_NUM0 0xBB38
#define LN2_DATA_ERR_NUM_H 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LN2_ERR_NUM1 0xBB3C
#define LN2_DATA_ERR_NUM_L 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LN3_ERR_NUM0 0xBB40
#define LN3_DATA_ERR_NUM_H 0xFF /* mask bit 7:0 */

#define REG_FRL_18B16B_LN3_ERR_NUM1 0xBB44
#define LN3_DATA_ERR_NUM_L 0xFF /* mask bit 7:0 */

#define REG_FRL_SRSB_CHK_CTRL 0xBB48
#define CFG_SRSB_GEN_BYP 0x1 /* mask bit 0 */

#define REG_FRL_SRSB_CHK_THRD 0xBB4C
#define CFG_SRSB_CHK_OK_THD   0xF  /* mask bit 3:0 */
#define CFG_SRSB_CHK_FAIL_THD 0xF0 /* mask bit 7:4 */

#define REG_FRL_SRSB_CHK_STA 0xBB50
#define CFG_SBSR_POS_ERR_INDEX     0x1 /* mask bit 0 */
#define CFG_SBSR_LOCK_OK           0x2 /* mask bit 1 */
#define CFG_SBSR_CURRENT_LANE_MODE 0x4 /* mask bit 2 */
#define CFG_SBSR_CURRENT_WORK_MODE 0x8 /* mask bit 3 */

#define REG_FRL_SRSB_CHK_ERR 0xBB54
#define CFG_SRSB_POS_ERR_CNT 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_CTRL 0xBC00
#define FRL_DSCRMB_EN         0x1  /* mask bit 0 */
#define FRL_DSCRMB_GT_EN      0x2  /* mask bit 1 */
#define GAP_CHECK_EN          0x4  /* mask bit 2 */
#define TRAIN_LFSR_BIG_ENDIAN 0x10 /* mask bit 4 */
#define TRAIN_DATA_BIG_ENDIAN 0x20 /* mask bit 5 */
#define NORM_LFSR_BIG_ENDIAN  0x40 /* mask bit 6 */
#define NORM_DATA_BIG_ENDIAN  0x80 /* mask bit 7 */

#define REG_FRL_DSCRMB_LTP_ERR_TH 0xBC04
#define LTP_DSCRMB_ERR_TH 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_LTP_SP_NUM0 0xBC08
#define LTP_DSCRMB_SP_NUM_H 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_LTP_SP_NUM1 0xBC0C
#define LTP_DSCRMB_SP_NUM_L 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_GAP_CNT0 0xBC10
#define GAP_CHECK_CNT_H 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_GAP_CNT1 0xBC14
#define GAP_CHECK_CNT_M 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_GAP_CNT2 0xBC18
#define GAP_CHECK_CNT_L 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_GAP_ERR_TH 0xBC1C
#define LTP_GAP_ERR_TH 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_GAP_SP_NUM0 0xBC20
#define LTP_GAP_SP_NUM_H 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_GAP_SP_NUM1 0xBC24
#define LTP_GAP_SP_NUM_M 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_GAP_SP_NUM2 0xBC28
#define LTP_GAP_SP_NUM_L 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SEED_0 0xBC2C
#define LN0_LFSR_NORM_SEED 0xFFFF /* mask bit 15:0 */

#define REG_FRL_DSCRMB_SEED_1 0xBC30
#define LN0_LFSR_LTP5_SEED 0xFFFF     /* mask bit 15:0 */
#define LN0_LFSR_LTP6_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SEED_2 0xBC34
#define LN0_LFSR_LTP7_SEED 0xFFFF     /* mask bit 15:0 */
#define LN0_LFSR_LTP8_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SEED_3 0xBC38
#define LN1_LFSR_NORM_SEED 0xFFFF /* mask bit 15:0 */

#define REG_FRL_DSCRMB_SEED_4 0xBC3C
#define LN1_LFSR_LTP5_SEED 0xFFFF     /* mask bit 15:0 */
#define LN1_LFSR_LTP6_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SEED_5 0xBC40
#define LN1_LFSR_LTP7_SEED 0xFFFF     /* mask bit 15:0 */
#define LN1_LFSR_LTP8_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SEED_6 0xBC44
#define LN2_LFSR_NORM_SEED 0xFFFF /* mask bit 15:0 */

#define REG_FRL_DSCRMB_SEED_7 0xBC48
#define LN2_LFSR_LTP5_SEED 0xFFFF     /* mask bit 15:0 */
#define LN2_LFSR_LTP6_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SEED_8 0xBC4C
#define LN2_LFSR_LTP7_SEED 0xFFFF     /* mask bit 15:0 */
#define LN2_LFSR_LTP8_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SEED_9 0xBC50
#define LN3_LFSR_NORM_SEED 0xFFFF /* mask bit 15:0 */

#define REG_FRL_DSCRMB_SEED_10 0xBC54
#define LN3_LFSR_LTP5_SEED 0xFFFF     /* mask bit 15:0 */
#define LN3_LFSR_LTP6_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SEED_11 0xBC58
#define LN3_LFSR_LTP7_SEED 0xFFFF     /* mask bit 15:0 */
#define LN3_LFSR_LTP8_SEED 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_SOFT_LTP_TH0 0xBC5C
#define LN0_SOFT_LTP_ERR_TH 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_TH1 0xBC60
#define LN1_SOFT_LTP_ERR_TH 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_TH2 0xBC64
#define LN2_SOFT_LTP_ERR_TH 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_TH3 0xBC68
#define LN3_SOFT_LTP_ERR_TH 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_CNT0 0xBC6C
#define LN0_SOFT_LTP_ERR_CNT 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_CNT1 0xBC70
#define LN1_SOFT_LTP_ERR_CNT 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_CNT2 0xBC74
#define LN2_SOFT_LTP_ERR_CNT 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_CNT3 0xBC78
#define LN3_SOFT_LTP_ERR_CNT 0xFF /* mask bit 7:0 */

#define REG_FRL_DSCRMB_SOFT_LTP_FLAG 0xBC7C
#define LN0_SOFT_LTP_ERR_HIGH_FLAG 0x1 /* mask bit 0 */
#define LN1_SOFT_LTP_ERR_HIGH_FLAG 0x2 /* mask bit 1 */
#define LN2_SOFT_LTP_ERR_HIGH_FLAG 0x4 /* mask bit 2 */
#define LN3_SOFT_LTP_ERR_HIGH_FLAG 0x8 /* mask bit 3 */

#define REG_FRL_DSCRMB_LTP2COARSE_TH 0xBC80
#define CFG_LTP2COARSE_SP_CNT_TH   0xFFFF    /* mask bit 15:0 */
#define CFG_LTP2COARSE_CHAR_ERR_TH 0xFFF0000 /* mask bit 27:16 */

#define REG_FRL_DSCRMB_LTP2SWEEP_TH 0xBC84
#define CFG_LTP2SWEEP_SP_CNT_TH   0xFFFF    /* mask bit 15:0 */
#define CFG_LTP2SWEEP_CHAR_ERR_TH 0xFFF0000 /* mask bit 27:16 */

#define REG_FRL_DSCRMB_LTP2COARSE_LANE0_CHAR_SET 0xBC90
#define CFG_LTP2COARSE_LN0_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2COARSE_LN0_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_LTP2COARSE_LANE1_CHAR_SET 0xBC94
#define CFG_LTP2COARSE_LN1_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2COARSE_LN1_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_LTP2COARSE_LANE2_CHAR_SET 0xBC98
#define CFG_LTP2COARSE_LN2_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2COARSE_LN2_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_LTP2COARSE_LANE3_CHAR_SET 0xBC9C
#define CFG_LTP2COARSE_LN3_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2COARSE_LN3_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_LTP2SWEEP_LANE0_CHAR_SET 0xBCA0
#define CFG_LTP2SWEEP_LN0_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2SWEEP_LN0_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_LTP2SWEEP_LANE1_CHAR_SET 0xBCA4
#define CFG_LTP2SWEEP_LN1_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2SWEEP_LN1_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_LTP2SWEEP_LANE2_CHAR_SET 0xBCA8
#define CFG_LTP2SWEEP_LN2_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2SWEEP_LN2_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_DSCRMB_LTP2SWEEP_LANE3_CHAR_SET 0xBCAC
#define CFG_LTP2SWEEP_LN3_CHK_LAST  0xFFFF     /* mask bit 15:0 */
#define CFG_LTP2SWEEP_LN3_CHK_FIRST 0xFFFF0000 /* mask bit 31:16 */

#define REG_RS_DECODE_CTRL 0xBD00
#define RS_DECODE_EN      0x1 /* mask bit 0 */
#define RS_DECODE_GT_EN   0x2 /* mask bit 1 */
#define RS_DECODE_CORR_EN 0x4 /* mask bit 2 */

#define REG_RS_DECODE_ERR_THD 0xBD04
#define RS_DECODE_ENERR_THD 0xFFFF     /* mask bit 15:0 */
#define RS_DECODE_UNERR_THD 0xFFFF0000 /* mask bit 31:16 */

#define REG_RS_DECODE_NUM_CLR 0xBD08
#define RS0_ENERR_NUM_CLR 0x1  /* mask bit 0 */
#define RS1_ENERR_NUM_CLR 0x2  /* mask bit 1 */
#define RS2_ENERR_NUM_CLR 0x4  /* mask bit 2 */
#define RS3_ENERR_NUM_CLR 0x8  /* mask bit 3 */
#define RS0_UNERR_NUM_CLR 0x10 /* mask bit 4 */
#define RS1_UNERR_NUM_CLR 0x20 /* mask bit 5 */
#define RS2_UNERR_NUM_CLR 0x40 /* mask bit 6 */
#define RS3_UNERR_NUM_CLR 0x80 /* mask bit 7 */

#define REG_RS_DECODE_ENERR_NUM0 0xBD0C
#define RS0_ENERR_NUM 0xFFFF     /* mask bit 15:0 */
#define RS1_ENERR_NUM 0xFFFF0000 /* mask bit 31:16 */

#define REG_RS_DECODE_ENERR_NUM1 0xBD10
#define RS2_ENERR_NUM 0xFFFF     /* mask bit 15:0 */
#define RS3_ENERR_NUM 0xFFFF0000 /* mask bit 31:16 */

#define REG_RS_DECODE_UNERR_NUM0 0xBD14
#define RS0_UNERR_NUM 0xFFFF     /* mask bit 15:0 */
#define RS1_UNERR_NUM 0xFFFF0000 /* mask bit 31:16 */

#define REG_RS_DECODE_UNERR_NUM1 0xBD18
#define RS2_UNERR_NUM 0xFFFF     /* mask bit 15:0 */
#define RS3_UNERR_NUM 0xFFFF0000 /* mask bit 31:16 */

#define REG_FRL_L2T_CTRL 0xBE00
#define FRL_L2T_GT_EN 0x1 /* mask bit 0 */

#define REG_FRL_L2T_DFX_CNT_CLR 0xBE04
#define PKG_BLACK_ZERO2_CNT_CLR   0x1  /* mask bit 0 */
#define PKG_SPAN_SUPBLOCK_CNT_CLR 0x2  /* mask bit 1 */
#define PKG_GAPTYPE_ERR_CNT_CLR   0x4  /* mask bit 2 */
#define PKG_GAPLEN_ERR_CNT_CLR    0x8  /* mask bit 3 */
#define PKG_TYPE_USE_RSVD_CNT_CLR 0x10 /* mask bit 4 */

#define REG_FRL_L2T_BLACK_ZERO2_CNT 0xBE08
#define PKG_BLACK_ZERO2_CNT 0xFFFF /* mask bit 15:0 */

#define REG_FRL_L2T_SPAN_SPBLOCK_CNT 0xBE0C
#define PKG_SPAN_SUPBLOCK_CNT 0xFFFF /* mask bit 15:0 */

#define REG_FRL_L2T_GAPTYPE_ERR_CNT 0xBE10
#define PKG_GAPTYPE_ERR_CNT 0xFFFF /* mask bit 15:0 */

#define REG_FRL_L2T_GAPLEN_ERR_CNT 0xBE14
#define PKG_GAPLEN_ERR_CNT 0xFFFF /* mask bit 15:0 */

#define REG_FRL_L2T_TYPE_RSVD_CNT 0xBE18
#define PKG_TYPE_USE_RSVD_CNT 0xFFFF /* mask bit 15:0 */

#define REG_FRL_DFM_CTRL 0xBF00
#define SFIFO_START_THD       0x3FF      /* mask bit 9:0 */
#define DFM_CK_GT_EN          0x400      /* mask bit 10 */
#define BLANK_CORR_MODE       0x800      /* mask bit 11 */
#define HSYNC_FILTER_PERIOD   0xF000     /* mask bit 15:12 */
#define VSYNC_FILTER_PERIOD   0xF0000    /* mask bit 19:16 */
#define SFIFO_ALMOST_FULL_THD 0x3FF00000 /* mask bit 29:20 */

#define REG_FRL_DFM_RADDR 0xBF10
#define FRL_SFIFO_RADDR 0xFF /* mask bit 7:0 */

#define REG_FRL_DFM_WADDR 0xBF14
#define FRL_SFIFO_WADDR 0xFF /* mask bit 7:0 */

#define REG_FRL_DFM_VALID_DATA 0xBF18
#define VALID_DATA_0 0x1FF /* mask bit 8:0 */

#define REG_FRL_DFM_OVFL_CNT 0xBF1C
#define OVER_FLOW_CNT 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FRL_DFM_OVFL_CNT_CLR 0xBF20
#define OVER_FLOW_CNT_CLR 0x1 /* mask bit 0 */

#define REG_FLT_MCU_PRAM 0x20000
#define PRAM_DATA 0xFFFFFFFF /* mask bit 31:0 */

#define REG_FLT_MCU_DRAM 0x22000
#define DRAM_DATA 0xFF /* mask bit 7:0 */

#endif /* __HAL_HDMIRX_FRL_H__ */
