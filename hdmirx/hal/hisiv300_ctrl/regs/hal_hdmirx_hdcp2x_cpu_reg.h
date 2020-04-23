/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of hdcp2X cpu module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_HDCP2X_CPU_REG_H__
#define __HAL_HDMIRX_HDCP2X_CPU_REG_H__

#define REG_HDCP2X_KEY_BIST_CTRL 0x0008
#define CPU_LOAD_RCV_ID_EN   0x1 /* mask bit 0 */
#define CPU_KEY_RAM_CRC_EN   0x2 /* mask bit 1 */
#define CPU_PRIVKEY_CKSUM_EN 0x4 /* mask bit 2 */

#define REG_HDCP2X_KEY_BIST_STA 0x000C
#define KEY_RAM_BIST_BUSY   0x1  /* mask bit 0 */
#define LOAD_RCV_ID_READY   0x2  /* mask bit 1 */
#define KEY_RAM_CRC_READY   0x4  /* mask bit 2 */
#define KEY_RAM_CRC_PASS    0x8  /* mask bit 3 */
#define PRIVKEY_CKSUM_READY 0x10 /* mask bit 4 */
#define PRIVKEY_CKSUM_PASS  0x20 /* mask bit 5 */

#define REG_HDCP2X_RCV_ID_LSB 0x0018
#define HDCP2X_RCV_ID_LSB 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDCP2X_RCV_ID_MSB 0x001C
#define HDCP2X_RCV_ID_MSB 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_CTRL_INT 0x0024
#define HDCP2X_ECC_INT 0x1 /* mask bit 0 */

#define REG_HDCP2X_CTRL_INT_MASK 0x0028
#define HDCP2X_ECC_INT_MASK 0x1 /* mask bit 0 */

#define REG_HDCP2X_DDC_MSG_CTRL 0x0040
#define CFG_HDCP2X_VERSION 0xFF   /* mask bit 7:0 */
#define CFG_RX_STATUS_RSV  0xF00  /* mask bit 11:8 */
#define CFG_KSV_FIFO_RDY   0x1000 /* mask bit 12 */

#define REG_HDCP2X_DDC_DBG_CTRL 0x0048
#define CFG_REAUTH_REQ_CLR_MODE 0x3  /* mask bit 1:0 */
#define CFG_SND_FIFO_CLR_MODE   0xC  /* mask bit 3:2 */
#define CFG_REAUTH_REQ_DIS      0x10 /* mask bit 4 */
#define CFG_MSG_PROT_EN         0x20 /* mask bit 5 */

#define REG_HDCP2X_REAUTH_CTRL 0x0050
#define CPU_REAUTH_REQ_CLR 0x1 /* mask bit 0 */
#define CPU_REAUTH_REQ_SET 0x2 /* mask bit 1 */

#define REG_HDCP2X_DDC_RXSTATUS 0x005C
#define HDCP2X_RX_STATUS_LSB 0xFF   /* mask bit 7:0 */
#define HDCP2X_RX_STATUS_MSB 0xFF00 /* mask bit 15:8 */

#define REG_HDCP2X_MSG_CNT_B0 0x0070
#define AKE_INIT_MSG_CNT    0xFF       /* mask bit 7:0 */
#define NO_STORE_KM_MSG_CNT 0xFF00     /* mask bit 15:8 */
#define STORE_KM_MSG_CNT    0xFF0000   /* mask bit 23:16 */
#define LC_INIT_MSG_CNT     0xFF000000 /* mask bit 31:24 */

#define REG_HDCP2X_MSG_CNT_B1 0x0074
#define SEND_SKS_MSG_CNT 0xFF     /* mask bit 7:0 */
#define MSG_RD_ERR_CNT   0xFF00   /* mask bit 15:8 */
#define MSG_WR_ERR_CNT   0xFF0000 /* mask bit 23:16 */

#define REG_HDCP2X_MSG_CNT_B2 0x0078
#define AKE_CERT_MSG_CNT     0xFF       /* mask bit 7:0 */
#define SEND_H_MSG_CNT       0xFF00     /* mask bit 15:8 */
#define SEND_PAIRING_MSG_CNT 0xFF0000   /* mask bit 23:16 */
#define SEND_L_MSG_CNT       0xFF000000 /* mask bit 31:24 */

#define REG_HDCP2X_MSG_ERR_B0 0x007C
#define AKE_INIT_ERROR_CNT    0xFF       /* mask bit 7:0 */
#define NO_STORE_KM_ERROR_CNT 0xFF00     /* mask bit 15:8 */
#define STORE_KM_ERROR_CNT    0xFF0000   /* mask bit 23:16 */
#define LC_INIT_ERROR_CNT     0xFF000000 /* mask bit 31:24 */

#define REG_HDCP2X_MSG_ERR_B1 0x0080
#define SEND_SKS_ERROR_CNT 0xFF       /* mask bit 7:0 */
#define SND_MSG_UNREC_CNT  0xFF00     /* mask bit 15:8 */
#define RCV_MSG_UNREC_CNT  0xFF0000   /* mask bit 23:16 */
#define RCV_MSG_OVR_CNT    0xFF000000 /* mask bit 31:24 */

#define REG_HDCP2X_MSG_ERR_B2 0x0084
#define AKE_CERT_ERROR_CNT     0xFF       /* mask bit 7:0 */
#define SEND_PAIRING_ERROR_CNT 0xFF00     /* mask bit 15:8 */
#define SEND_H_ERROR_CNT       0xFF0000   /* mask bit 23:16 */
#define SEND_L_ERROR_CNT       0xFF000000 /* mask bit 31:24 */

#define REG_HDCP2X_TRNG_DBG_CNT 0x0090
#define TRNG_DATA_REQ_CNT 0xFF       /* mask bit 7:0 */
#define TRNG_CRC4_ERR_CNT 0xFF00     /* mask bit 15:8 */
#define TRNG_TOUT_ERR_CNT 0xFF0000   /* mask bit 23:16 */
#define TRNG_SEC_REQ_CNT  0xFF000000 /* mask bit 31:24 */

#define REG_HDCP2X_DBG_CNT_CLR 0x00A0
#define CFG_DDC_REC_CLR   0x1 /* mask bit 0 */
#define CFG_TRNG_CNT_CLR  0x2 /* mask bit 1 */
#define CFG_LIMIT_CNT_CLR 0x4 /* mask bit 2 */
#define CFG_MCU_CNT_CLR   0x8 /* mask bit 3 */

#define REG_HDCP2X_MCU_DBG_CFG 0x00A4
#define CFG_MCU_SPEC_PC_ADDR  0xFFFF     /* mask bit 15:0 */
#define CFG_MCU_SPEC_EXT_ADDR 0xFFFF0000 /* mask bit 31:16 */

#define REG_HDCP2X_UART_STOP_SEL 0x00B0
#define HDCP_MCU_UART_STOP_SEL 0x1 /* mask bit 0 */

#define REG_HDCP2P2_UART_BAUD_UNIT 0x00B4
#define MCU_UART_BAUD_UNIT 0xFFF /* mask bit 11:0 */

#define REG_HDCP2P2_UART_RATE_SEL 0x00B8
#define HDCP_MCU_UART_RATE_SEL 0x3 /* mask bit 1:0 */

#define REG_HDCP2X_UNLAW_CNT 0x00C4
#define MCU_UNLAW_WERR_CNT 0xFF       /* mask bit 7:0 */
#define MCU_UNLAW_RERR_CNT 0xFF00     /* mask bit 15:8 */
#define DMA_UNLAW_WERR_CNT 0xFF0000   /* mask bit 23:16 */
#define DMA_UNLAW_RERR_CNT 0xFF000000 /* mask bit 31:24 */

#define REG_HDCP2X_MCU_DBG_CNT 0x00C8
#define MCU_SPEC_ADR_CNT 0xFF     /* mask bit 7:0 */
#define MCU_EXT_ADR_WCNT 0xFF00   /* mask bit 15:8 */
#define MCU_EXT_ADR_RCNT 0xFF0000 /* mask bit 23:16 */

#define REG_HDCP2X_MCU_PC_ADDR 0x00CC
#define MCU_CURR_PC_ADDR 0xFFFF /* mask bit 15:0 */

#define REG_HDCP2X_CIPHER_CTRL 0x00E0
#define CFG_AUTH_DONE_SYN_EN 0x1  /* mask bit 0 */
#define CFG_HDCP_MODE_SYN_EN 0x2  /* mask bit 1 */
#define CFG_DIS_CIPH_MODE    0x1C /* mask bit 4:2 */

#define REG_HDCP2X_CIPHER_STATE 0x00E4
#define HDCP2X_DEC_FIFO_FULL     0x1       /* mask bit 0 */
#define HDCP2X_DEC_FIFO_EMPTY    0x2       /* mask bit 1 */
#define HDCP2X_DEC_FIFO_CNT      0x7C      /* mask bit 6:2 */
#define HDCP2X_DIS_DEC_ST        0x80      /* mask bit 7 */
#define HDCP2X_DEC_FIFO_RERR_CNT 0xFF00    /* mask bit 15:8 */
#define HDCP2X_DEC_FIFO_WERR_CNT 0xFF0000  /* mask bit 23:16 */
#define AKE_PXL_CNT_MAX          0x1000000 /* mask bit 24 */
#define AKE_FRM_CNT_MAX          0x2000000 /* mask bit 25 */

#define REG_HDCP2X_FRM_CNT_LSB 0x00E8
#define HDCP2X_FRM_CNT_LSB 0xFFFFFFFF /* mask bit 31:0 */

#define REG_HDCP2X_FRM_CNT_MSB 0x00EC
#define HDCP2X_FRM_CNT_MSB 0xFF /* mask bit 7:0 */

#define REG_ECC_CHECK_MODE 0x0100
#define ECC_CHECK_MODE       0x7 /* mask bit 2:0 */
#define ECC_CHECK_FRAME_MODE 0x8 /* mask bit 3 */

#define REG_ECC_SKIP_FRAME_NUM 0x0104
#define RI_CNT2CHK_ECC 0x1FF /* mask bit 8:0 */

#define REG_ECC_MODE0_ERR_THRES 0x0108
#define RI_ACCM_ERR_THR 0x1FFFFF /* mask bit 20:0 */

#define REG_ECC_MODE1_ERR_THRES 0x010C
#define RI_FRAME_ECC_ERR_THR 0xFFFF /* mask bit 15:0 */

#define REG_ECC_MODE1_FRAME_NUM 0x0110
#define RI_CONS_ECC_ERR_THR 0xFF /* mask bit 7:0 */

#define REG_ECC_MODE2_FRAME_NUM 0x0114
#define RI_NO_ECC_THR 0xFF /* mask bit 7:0 */

#define REG_ECC_MODE3_ERR_THRES 0x0118
#define RI_GIVEN_FRAME_ERR_THR 0x1FFFFF /* mask bit 20:0 */

#define REG_ECC_MODE3_FRAME_NUM 0x011C
#define RI_GIVEN_FRAME 0xFF /* mask bit 7:0 */

#define REG_ECC_MODE4_THRES 0x0120
#define RI_MODE4_THR 0xFF /* mask bit 7:0 */

#endif /* __HAL_HDMIRX_HDCP2X_CPU_REG_H__ */
