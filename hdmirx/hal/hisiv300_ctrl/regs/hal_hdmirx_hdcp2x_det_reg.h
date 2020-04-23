/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of hdcp2x detection module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_HDCP2X_DET_H__
#define __HAL_HDMIRX_HDCP2X_DET_H__

#define REG_FILTER_CTL 0x4000
#define HDCP2X_CFG_VS_FILTER_SEL       0x3    /* mask bit 1:0 */
#define HDCP2X_CFG_HS_FILTER_SEL       0xC    /* mask bit 3:2 */
#define HDCP2X_CFG_CTL_FILTER_SEL      0x30   /* mask bit 5:4 */
#define HDCP2X_CFG_DIDE_FILTER_SEL     0xC0   /* mask bit 7:6 */
#define HDCP2X_CFG_VIDDE_FILTER_SEL    0x300  /* mask bit 9:8 */
#define HDCP2X_CFG_VS_FILTER_BYPASS    0x400  /* mask bit 10 */
#define HDCP2X_CFG_HS_FILTER_BYPASS    0x800  /* mask bit 11 */
#define HDCP2X_CFG_CTL_FILTER_BYPASS   0x1000 /* mask bit 12 */
#define HDCP2X_CFG_DIDE_FILTER_BYPASS  0x2000 /* mask bit 13 */
#define HDCP2X_CFG_VIDDE_FILTER_BYPASS 0x4000 /* mask bit 14 */

#define REG_HDCP_DET_CFG 0x4004
#define CFG_HDCP2X_WIN_START 0x3FF     /* mask bit 9:0 */
#define CFG_HDCP2X_WIN_END   0xFFC00   /* mask bit 19:10 */
#define CFG_HDCP2X_CTL_CNT   0x1F00000 /* mask bit 24:20 */
#define CFG_HDCP2X_CTL_MODE  0x6000000 /* mask bit 26:25 */

#define REG_HDCP_DECRYPT_CFG 0x4008
#define CFG_DISABLE_GCP_MUTE         0x1  /* mask bit 0 */
#define CFG_HDCP2X_ADVANCE_CIPHER_EN 0x2  /* mask bit 1 */
#define CFG_HDCP2X_FRAME_CLR         0x4  /* mask bit 2 */
#define CFG_HDCP2X_ERROR_CLR         0x8  /* mask bit 3 */
#define CFG_CTL3_MODE_SEL            0x10 /* mask bit 4 */

#define REG_HDCP_VS_GLITCH_CFG 0x400C
#define CFG_VS_GLITCH_CNT   0xFF  /* mask bit 7:0 */
#define CFG_VS_ERROR_ENABLE 0x100 /* mask bit 8 */

#define REG_HDCP_FRAME_CNT0 0x4010
#define HDCP2X_ENCRYPT_MUTE_FRAMES 0xFFFF /* mask bit 15:0 */

#define REG_HDCP_FRAME_CNT1 0x4014
#define HDCP2X_UNENCRYPT_FRAMES 0xFFFF     /* mask bit 15:0 */
#define HDCP2X_ENCRYPT_FRAMES   0xFFFF0000 /* mask bit 31:16 */

#define REG_HDCP2X_DET_ERR0 0x4018
#define HDCP2X_VID_ERR_CNT 0xFFFF     /* mask bit 15:0 */
#define HDCP2X_DI_ERR_CNT  0xFFFF0000 /* mask bit 31:16 */

#define REG_HDCP2X_DET_ERR1 0x401C
#define HDCP2X_CTL_COMPRESS_ERR_CNT 0xFFFF     /* mask bit 15:0 */
#define HDCP2X_KEEPOUT_DE_ERR_CNT   0xFFFF0000 /* mask bit 31:16 */

#define REG_HDCP2X_DET_ERR2 0x4020
#define HDCP2X_CTL3_ERR_CNT    0xFFFF     /* mask bit 15:0 */
#define HDCP2X_ENCRYPT_ERR_CNT 0xFFFF0000 /* mask bit 31:16 */

#define REG_HDCP_VS_POL_CFG 0x4024
#define CFG_HDCP2X_VS_POL_SW  0x1 /* mask bit 0 */
#define CFG_HDCP2X_VS_POL_OVR 0x2 /* mask bit 1 */

#define REG_HDCP_DET_HW 0x4028
#define HDCP2X_VS_POL     0x1 /* mask bit 0 */
#define HDCP2X_DECRYPT_ST 0x2 /* mask bit 1 */

#define REG_KEEPOUT_WIN_CFG 0x402C
#define CFG_KEEPOUT_WIN_START 0x3FF   /* mask bit 9:0 */
#define CFG_KEEPOUT_WIN_END   0xFFC00 /* mask bit 19:10 */

#endif /* __HAL_HDMIRX_HDCP2X_DET_H__ */
