/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: register definition of decoder module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_DECODER_REG_H__
#define __HAL_HDMIRX_DECODER_REG_H__

#define REG_HDMI_MODE_CTL 0x6000
#define CFG_HDMI_MODE_OVERWRITE     0x1     /* mask bit 0 */
#define CFG_HDMI_MODE_SW_VALUE      0x2     /* mask bit 1 */
#define CFG_HDMI_MODE_CLR_EN        0x4     /* mask bit 2 */
#define CFG_HDMI_LOST_CNT           0x1F8   /* mask bit 8:3 */
#define CFG_HDMI_TERC4_CNT          0x7E00  /* mask bit 14:9 */
#define DECODER_CFG_HDMI_MODE_SEL   0x8000  /* mask bit 15 */
#define CFG_TERC_DET_SEL            0x10000 /* mask bit 16 */
#define CFG_DIGB_SEL                0x60000 /* mask bit 18:17 */

#define REG_PREAMBLE_CNT_CFG 0x6004
#define CFG_DIPREAMBLE_CNT  0xF  /* mask bit 3:0 */
#define CFG_VIDPREAMBLE_CNT 0xF0 /* mask bit 7:4 */

#define REG_AV_SPLIT_BYP 0x6008
#define CFG_DIPREAMBLE_BYP  0x1 /* mask bit 0 */
#define CFG_DIGB_BYP        0x2 /* mask bit 1 */
#define CFG_VIDPREAMBLE_BYP 0x4 /* mask bit 2 */
#define CFG_VIDGB_BYP       0x8 /* mask bit 3 */

#define REG_TERC_THRESHOLD_CFG 0x600C
#define CFG_T4_UNCORR_THRESHOLD 0x7F   /* mask bit 6:0 */
#define CFG_T4_CORR_THRESHOLD   0x7F00 /* mask bit 14:8 */

#define REG_HDMI_MODE_HW 0x6010
#define CFG_HDMI_MODE_GB   0x1 /* mask bit 0 */
#define CFG_HDMI_MODE_TERC 0x2 /* mask bit 1 */
#define CFG_HDMI_MODE_ST   0x4 /* mask bit 2 */

#define REG_CFG_CTL_DEL_FILTER 0x6014
#define CFG_CTLDET_FILTER_SEL      0x1  /* mask bit 0 */
#define CFG_CTLDET_FILTER_BYPASS   0x2  /* mask bit 1 */
#define CFG_SSCP_DET_FILTER_SEL    0x4  /* mask bit 2 */
#define CFG_SSCP_DET_FILTER_BYPASS 0x8  /* mask bit 3 */
#define CFG_SYNC_DET_SEL           0x30 /* mask bit 5:4 */
#define CFG_SYNC_DET_BYPASS        0x40 /* mask bit 6 */

#define REG_CFG_DEC_IRQ_MASK 0x6018
#define CFG_ORIGINAL_DECODER_INTR_STAT0_MASK 0x1  /* mask bit 0 */
#define CFG_ORIGINAL_DECODER_INTR_STAT1_MASK 0x2  /* mask bit 1 */
#define CFG_ORIGINAL_DECODER_INTR_STAT2_MASK 0x4  /* mask bit 2 */
#define CFG_ORIGINAL_DECODER_INTR_STAT3_MASK 0x8  /* mask bit 3 */
#define CFG_ORIGINAL_DECODER_INTR_STAT4_MASK 0x10 /* mask bit 4 */
#define CFG_ORIGINAL_DECODER_INTR_STAT5_MASK 0x20 /* mask bit 5 */
#define CFG_ORIGINAL_DECODER_INTR_STAT6_MASK 0x40 /* mask bit 6 */
#define CFG_ORIGINAL_DECODER_INTR_STAT7_MASK 0x80 /* mask bit 7 */

#define REG_CFG_DEC_ORIGINAL_IRQ_STATUS 0x601C
#define CFG_ORIGINAL_DECODER_INTR_STAT0 0x1  /* mask bit 0 */
#define CFG_ORIGINAL_DECODER_INTR_STAT1 0x2  /* mask bit 1 */
#define CFG_ORIGINAL_DECODER_INTR_STAT2 0x4  /* mask bit 2 */
#define CFG_ORIGINAL_DECODER_INTR_STAT3 0x8  /* mask bit 3 */
#define CFG_ORIGINAL_DECODER_INTR_STAT4 0x10 /* mask bit 4 */
#define CFG_ORIGINAL_DECODER_INTR_STAT5 0x20 /* mask bit 5 */
#define CFG_ORIGINAL_DECODER_INTR_STAT6 0x40 /* mask bit 6 */
#define CFG_ORIGINAL_DECODER_INTR_STAT7 0x80 /* mask bit 7 */

#define REG_EXCEP_CONFIG_TIME 0x6020
#define CFG_EXCEP_CHECK_TIME 0xFFFFFFFF /* mask bit 31:0 */

#define REG_EXCEP_CONFIG_CNT 0x6024
#define CFG_CONTI_EXCEPT_DUTY_CNT 0xFF   /* mask bit 7:0 */
#define CFG_CONTI_NORMAL_DUTY_CNT 0xFF00 /* mask bit 15:8 */

#define REG_TMDS_CLK_GEN_FIFO_CLR 0x6028
#define FIFO_WR_ERR_CLR 0x1 /* mask bit 0 */
#define FIFO_RD_ERR_CLR 0x2 /* mask bit 1 */

#define REG_TMDS_CLK_GEN_FIFO_ERR_CNT 0x602C
#define FIFO_WR_ERR_CNT 0xFFFF     /* mask bit 15:0 */
#define FIFO_RD_ERR_CNT 0xFFFF0000 /* mask bit 31:16 */

#define REG_DEC_TEST_SEL_CFG 0x6030
#define CFG_DEC_TEST_SEL0 0x1F    /* mask bit 4:0 */
#define CFG_DEC_TEST_SEL1 0x3E0   /* mask bit 9:5 */
#define CFG_DEC_TEST_SEL2 0x7C00  /* mask bit 14:10 */
#define CFG_DEC_TEST_SEL3 0xF8000 /* mask bit 19:15 */

#define REG_DEC_INTERALIGN_CFG 0x6034
#define CFG_DESKEW_EN 0x1 /* mask bit 0 */

#define REG_DE_CHECK_CFG0 0x6050
#define CFG_VID_DE_ERROR_THRESH   0xFF    /* mask bit 7:0 */
#define CFG_DI_DE_ERROR_THRESH    0xFF00  /* mask bit 15:8 */
#define CFG_VID_DE_ERROR_CHECK_EN 0x10000 /* mask bit 16 */
#define CFG_DI_DE_ERROR_CHECK_EN  0x20000 /* mask bit 17 */
#define CFG_DE_ERROR_COMBINE      0x40000 /* mask bit 18 */

#define REG_DE_CHECK_CFG1 0x6054
#define CFG_HACTIVE_MIN      0xFF   /* mask bit 7:0 */
#define CFG_DE_STABLE_THRESH 0xFF00 /* mask bit 15:8 */

#define REG_DE_STABLE_HW 0x6058
#define DVI_DE_STABLE 0x1 /* mask bit 0 */
#endif /* __HAL_HDMIRX_DECODER_REG_H__ */
