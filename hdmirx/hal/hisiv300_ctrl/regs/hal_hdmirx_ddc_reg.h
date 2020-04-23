/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of ddc module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_DDC_H__
#define __HAL_HDMIRX_DDC_H__

#define REG_SINK_VERSION 0x8404
#define REG_SINK_VER 0xFF /* mask bit 7:0 */

#define REG_SOURCE_VERSION 0x8408
#define REG_SRC_VER 0xFF /* mask bit 7:0 */

#define REG_UPDATE_FLAGS_B0 0x8440
#define STATUS_UPDATE      0x1  /* mask bit 0 */
#define CED_UPDATE         0x2  /* mask bit 1 */
#define RR_TEST            0x4  /* mask bit 2 */
#define SOURCE_TEST_UPDATE 0x8  /* mask bit 3 */
#define FRL_START          0x10 /* mask bit 4 */
#define FLT_UPDATE         0x20 /* mask bit 5 */
#define RSED_UPDATE        0x40 /* mask bit 6 */

#define REG_UPDATE_FLAGS_B1 0x8444


#define REG_TMDS_CONFIGURATION 0x8480
#define SCRAMBLING_ENABLE    0x1 /* mask bit 0 */
#define TMDS_BIT_CLOCK_RATIO 0x2 /* mask bit 1 */

#define REG_TMDS_SCRAMBLER_STATUS 0x8484
#define SCRAMBLER_STATUS 0x1 /* mask bit 0 */

#define REG_SINK_CONFIGURATION_B0 0x84C0
#define RR_ENABLE      0x1 /* mask bit 0 */
#define FLT_NO_RETRAIN 0x2 /* mask bit 1 */

#define REG_SINK_CONFIGURATION_B1 0x84C4
#define FRL_RATE   0xF  /* mask bit 3:0 */
#define FFE_LEVELS 0xF0 /* mask bit 7:4 */

#define REG_SOURCE_TEST_CONFIGURATION 0x84D4
#define TXFFE_PRE_SHOOT_ONLY   0x2  /* mask bit 1 */
#define TXFFE_DE_EMPHASIS_ONLY 0x4  /* mask bit 2 */
#define TXFFE_NO_FFE           0x8  /* mask bit 3 */
#define FLT_NO_TIMEOUT         0x20 /* mask bit 5 */
#define DSC_FRL_MAX            0x40 /* mask bit 6 */
#define FRL_MAX                0x80 /* mask bit 7 */

#define REG_STATUS_FLAGS_B0 0x8500
#define CLOCK_DETECTED 0x1  /* mask bit 0 */
#define CH0_LN0_LOCKED 0x2  /* mask bit 1 */
#define CH1_LN1_LOCKED 0x4  /* mask bit 2 */
#define CH2_LN2_LOCKED 0x8  /* mask bit 3 */
#define LANE3_LOCKED   0x10 /* mask bit 4 */
#define FLT_READY      0x40 /* mask bit 6 */
#define DSC_DECODEFAIL 0x80 /* mask bit 7 */

#define REG_STATUS_FLAGS_B1 0x8504
#define LN0_LTP_REQ 0xF  /* mask bit 3:0 */
#define LN1_LTP_REQ 0xF0 /* mask bit 7:4 */

#define REG_STATUS_FLAGS_B2 0x8508
#define LN2_LTP_REQ 0xF  /* mask bit 3:0 */
#define LN3_LTP_REQ 0xF0 /* mask bit 7:4 */

#define REG_CH0_ERR_CNT_LSB 0x8540
#define CH0_CED_LSB 0xFF /* mask bit 7:0 */

#define REG_CH0_ERR_CNT_MSB 0x8544
#define CH0_CED_MSB   0x7F /* mask bit 6:0 */
#define CH0_LN0_VALID 0x80 /* mask bit 7 */

#define REG_CH1_ERR_CNT_LSB 0x8548
#define CH1_CED_LSB 0xFF /* mask bit 7:0 */

#define REG_CH1_ERR_CNT_MSB 0x854C
#define CH1_CED_MSB   0x7F /* mask bit 6:0 */
#define CH1_LN1_VALID 0x80 /* mask bit 7 */

#define REG_CH2_ERR_CNT_LSB 0x8550
#define CH2_CED_LSB 0xFF /* mask bit 7:0 */

#define REG_CH2_ERR_CNT_MSB 0x8554
#define CH2_CED_MSB   0x7F /* mask bit 6:0 */
#define CH2_LN2_VALID 0x80 /* mask bit 7 */

#define REG_CHKSUM_ERR_CNT 0x8558
#define CED_CHKSUM 0xFF /* mask bit 7:0 */

#define REG_CH3_ERR_CNT_LSB 0x855C
#define CED_LN3_LSB 0xFF /* mask bit 7:0 */

#define REG_CH3_ERR_CNT_MSB 0x8560
#define CED_LN3_MSB   0x7F /* mask bit 6:0 */
#define CED_LN3_VALID 0x80 /* mask bit 7 */

#define REG_RS_ERR_CNT_LSB 0x8564
#define RS_CC_LSB 0xFF /* mask bit 7:0 */

#define REG_RS_ERR_CNT_MSB 0x8568
#define RS_CC_MSB  0x7F /* mask bit 6:0 */
#define RS_C_VALID 0x80 /* mask bit 7 */

#define REG_TEST_CONFIGURATION 0x8700
#define TESTREADREQUESTDELAY 0x7F /* mask bit 6:0 */
#define TESTREADREQUEST      0x80 /* mask bit 7 */

#define REG_SCDCS_MANUFACTURERSPECIFIC_B0 0x8740
#define MANUFACTURER_OUI_1 0xFF /* mask bit 7:0 */

#define REG_SCDCS_MANUFACTURERSPECIFIC_B1 0x8744
#define MANUFACTURER_OUI_2 0xFF /* mask bit 7:0 */

#define REG_SCDCS_MANUFACTURERSPECIFIC_B2 0x8748
#define MANUFACTURER_OUI_3 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B1 0x874C
#define DEVICE_ID_STRING_1 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B2 0x8750
#define DEVICE_ID_STRING_2 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B3 0x8754
#define DEVICE_ID_STRING_3 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B4 0x8758
#define DEVICE_ID_STRING_4 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B5 0x875C
#define DEVICE_ID_STRING_5 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B6 0x8760
#define DEVICE_ID_STRING_6 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B7 0x8764
#define DEVICE_ID_STRING_7 0xFF /* mask bit 7:0 */

#define REG_SCDC_DEV_ID_B8 0x8768
#define DEVICE_ID_STRING_8 0xFF /* mask bit 7:0 */

#define REG_SCDC_HW_REV 0x876C
#define HARDWARE_MINOR_REV 0xF  /* mask bit 3:0 */
#define HARDWARE_MAJOR_REV 0xF0 /* mask bit 7:4 */

#define REG_SCDC_SW_MAJ_REV 0x8770
#define SOFTWARE_MAJOR_REV 0xFF /* mask bit 7:0 */

#define REG_SCDC_SW_MIN_REV 0x8774
#define SOFTWARE_MINOR_REV 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B0 0x8778
#define MANU_SPEC_B0 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B1 0x877C
#define MANU_SPEC_B1 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B2 0x8780
#define MANU_SPEC_B2 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B3 0x8784
#define MANU_SPEC_B3 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B4 0x8788
#define MANU_SPEC_B4 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B5 0x878C
#define MANU_SPEC_B5 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B6 0x8790
#define MANU_SPEC_B6 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B7 0x8794
#define MANU_SPEC_B7 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B8 0x8798
#define MANU_SPEC_B8 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B9 0x879C
#define MANU_SPEC_B9 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B10 0x87A0
#define MANU_SPEC_B10 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B11 0x87A4
#define MANU_SPEC_B11 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B12 0x87A8
#define MANU_SPEC_B12 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B13 0x87AC
#define MANU_SPEC_B13 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B14 0x87B0
#define MANU_SPEC_B14 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B15 0x87B4
#define MANU_SPEC_B15 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B16 0x87B8
#define MANU_SPEC_B16 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B17 0x87BC
#define MANU_SPEC_B17 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B18 0x87C0
#define MANU_SPEC_B18 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B19 0x87C4
#define MANU_SPEC_B19 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B20 0x87C8
#define MANU_SPEC_B20 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B21 0x87CC
#define MANU_SPEC_B21 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B22 0x87D0
#define MANU_SPEC_B22 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B23 0x87D4
#define MANU_SPEC_B23 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B24 0x87D8
#define MANU_SPEC_B24 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B25 0x87DC
#define MANU_SPEC_B25 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B26 0x87E0
#define MANU_SPEC_B26 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B27 0x87E4
#define MANU_SPEC_B27 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B28 0x87E8
#define MANU_SPEC_B28 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B29 0x87EC
#define MANU_SPEC_B29 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B30 0x87F0
#define MANU_SPEC_B30 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B31 0x87F4
#define MANU_SPEC_B31 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B32 0x87F8
#define MANU_SPEC_B32 0xFF /* mask bit 7:0 */

#define REG_SCDC_MANU_SPEC_B33 0x87FC
#define MANU_SPEC_B33 0xFF /* mask bit 7:0 */

#endif /* __HAL_HDMIRX_DDC_H__ */
