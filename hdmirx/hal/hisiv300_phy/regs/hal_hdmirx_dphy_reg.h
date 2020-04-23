/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of dphy module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_DPHY_REG_H__
#define __HAL_HDMIRX_DPHY_REG_H__

#define REG_CFG_DPHY_INT 0x0000
#define INTR_CED_SERIOUS_ERR      0x1   /* mask bit 0 */
#define INTR_DESKEW_LOCK_CHANGE   0x2   /* mask bit 1 */
#define INTR_CH0_CHAR_SYNC_CHANGE 0x4   /* mask bit 2 */
#define INTR_CH1_CHAR_SYNC_CHANGE 0x8   /* mask bit 3 */
#define INTR_CH2_CHAR_SYNC_CHANGE 0x10  /* mask bit 4 */
#define INTR_CH3_CHAR_SYNC_CHANGE 0x20  /* mask bit 5 */
#define INTR_LESS8_SSCP           0x40  /* mask bit 6 */
#define INTR_SCDT_CHANGE          0x80  /* mask bit 7 */
#define INTR_SCRAMBLE_CHANGE      0x100 /* mask bit 8 */

#define DPHY_CFG_INT_MASK 0x0004
#define INTR_CED_SERIOUS_ERR_MASK      0x1   /* mask bit 0 */
#define INTR_DESKEW_LOCK_CHANGE_MASK   0x2   /* mask bit 1 */
#define INTR_CH0_CHAR_SYNC_CHANGE_MASK 0x4   /* mask bit 2 */
#define INTR_CH1_CHAR_SYNC_CHANGE_MASK 0x8   /* mask bit 3 */
#define INTR_CH2_CHAR_SYNC_CHANGE_MASK 0x10  /* mask bit 4 */
#define INTR_CH3_CHAR_SYNC_CHANGE_MASK 0x20  /* mask bit 5 */
#define INTR_LESS8_SSCP_MASK           0x40  /* mask bit 6 */
#define INTR_SCDT_CHANGE_MASK          0x80  /* mask bit 7 */
#define INTR_SCRAMBLE_CHANGE_MASK      0x100 /* mask bit 8 */

#define REG_DOMAIN_SYNC_CFG 0x0008
#define CFG_TMDS_ENDIAN_CHANGE_EN 0x1      /* mask bit 0 */
#define CFG_FRL_ENDIAN_CHANGE_EN  0x2      /* mask bit 1 */
#define CFG_DOMAIN_FIFO_REQ_EN    0x4      /* mask bit 2 */
#define CFG_MODE_OVERWRITE_EN     0x8      /* mask bit 3 */
#define CFG_MODE_OVERWRITE_VALUE  0x70     /* mask bit 6:4 */
#define CFG_LTP_REQ_VALUE         0x7FFF80 /* mask bit 22:7 */

#define REG_DOMAIN_FULL_CLR 0x000C
#define CFG_DOMAIN_FULL_CLR 0x1 /* mask bit 0 */

#define REG_DOMAIN_STATE 0x0010
#define DOMAIN_FIFO_FULL 0xF /* mask bit 3:0 */

#define REG_CHAR_SYNC_CFG 0x002C
#define CFG_MASK_CHARSCV_EN     0x1     /* mask bit 0 */
#define CFG_FRL_CHAR_LOCK_THD   0x1E    /* mask bit 4:1 */
#define CFG_FRL_CHAR_UNLOCK_THD 0x1E0   /* mask bit 8:5 */
#define CFG_FRL_OK_THRESH       0x1E00  /* mask bit 12:9 */
#define CFG_FRL_FAIL_THRESH     0x1E000 /* mask bit 16:13 */

#define REG_CHAR_SYNC_STATE 0x0030
#define CH0_CHAR_SYNC_OK 0x1 /* mask bit 0 */
#define CH1_CHAR_SYNC_OK 0x2 /* mask bit 1 */
#define CH2_CHAR_SYNC_OK 0x4 /* mask bit 2 */
#define CH3_CHAR_SYNC_OK 0x8 /* mask bit 3 */

#define REG_LOCK_STATE 0x0034
#define LOCK_STATE 0xF /* mask bit 3:0 */

#define REG_CHANNEL_FULL_CLR 0x0048
#define CFG_CHANNEL_FULL_CLR 0x1 /* mask bit 0 */

#define REG_CH_DESKEW_CFG 0x004C
#define CFG_CHANNEL_FIFO_REQ_EN 0x1  /* mask bit 0 */
#define CFG_DESKEW_FAIL_CNT     0x1E /* mask bit 4:1 */
#define CFG_MASK_CHNLSCV_EN     0x20 /* mask bit 5 */
#define CFG_LTP_DESKEW_EN       0xC0 /* mask bit 7:6 */

#define REG_CH_DESKEW_STATE 0x0050
#define DESKEW_LOCK      0x1  /* mask bit 0 */
#define DESKEW_FIFO_FULL 0x1E /* mask bit 4:1 */

#define REG_CED_CFG 0x0064
#define CFG_CED_VALID_CLR 0x1 /* mask bit 0 */
#define CFG_NO_TMDS_DATA  0x2 /* mask bit 1 */
#define CFG_NO_FRL_DATA   0x4 /* mask bit 2 */

#define REG_TIME_1024US_CFG 0x0068
#define CFG_1024US_THRESH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_TIME_1MS_CFG 0x006C
#define CFG_1MS_THRESH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SHADOW_CED_CLR 0x0070
#define CFG_CNT_SW_CLR 0x1 /* mask bit 0 */

#define REG_SHADOW_CED_CFG 0x0074
#define CFG_CNT_SW_EN 0x1 /* mask bit 0 */

#define REG_SHADOW_CED_CNT0 0x0078
#define CED_CNT_SW0 0xFFFF     /* mask bit 15:0 */
#define CED_CNT_SW1 0xFFFF0000 /* mask bit 31:16 */

#define REG_SHADOW_CED_CNT1 0x007C
#define CED_CNT_SW2 0xFFFF     /* mask bit 15:0 */
#define CED_CNT_SW3 0xFFFF0000 /* mask bit 31:16 */

#define REG_SHADOW_CED_CNT2 0x0080
#define CED_DATA_CNT_SW0    0xFFFF     /* mask bit 15:0 */
#define CED_CONTROL_CNT_SW0 0xFFFF0000 /* mask bit 31:16 */

#define REG_SHADOW_CED_CNT3 0x0084
#define CED_DATA_CNT_SW1    0xFFFF     /* mask bit 15:0 */
#define CED_CONTROL_CNT_SW1 0xFFFF0000 /* mask bit 31:16 */

#define REG_SHADOW_CED_CNT4 0x0088
#define CED_DATA_CNT_SW2    0xFFFF     /* mask bit 15:0 */
#define CED_CONTROL_CNT_SW2 0xFFFF0000 /* mask bit 31:16 */

#define REG_AUTOEQ_CED_OVER_THRESH 0x008C
#define CFG_AUTOEQ_ERR_THRESH 0xFF  /* mask bit 7:0 */
#define CFG_AUTOEQ_FLAG_SEL   0x100 /* mask bit 8 */

#define REG_CED_LOCK_STATE 0x0090
#define TMDS_SYMBOL_LOCK 0x7   /* mask bit 2:0 */
#define TMDS_CED_LOCK    0x38  /* mask bit 5:3 */
#define LANE_CED_LOCK    0x3C0 /* mask bit 9:6 */

#define REG_LTP_CFG 0x0094
#define CFG_LTPEN_OVERWRITE_EN    0x1       /* mask bit 0 */
#define CFG_LTPEN_OVERWRITE_VALUE 0x2       /* mask bit 1 */
#define CFG_LTP58_FAIL_THRESH     0x3C      /* mask bit 5:2 */
#define CFG_LTP58_OK_THRESH       0x3C0     /* mask bit 9:6 */
#define CFG_LTP_ERR_THRESH        0x3FFFC00 /* mask bit 25:10 */

#define REG_LTP_STATE 0x009C
#define LTP_OK          0xF  /* mask bit 3:0 */
#define LTP5_8REPEAT_OK 0xF0 /* mask bit 7:4 */

#define REG_SCDT_CFG0 0x00B4
#define CFG_SCDT_CONTROL_THRESH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SCDT_CFG1 0x00B8
#define CFG_SCDT_DATA_THRESH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_SCDT_STATE 0x00BC
#define CH_SCDT 0x7 /* mask bit 2:0 */
#define SCDT    0x8 /* mask bit 3 */

#define REG_SCRAMBLE_DET_CFG 0x00C0
#define CFG_SCRAMBLE_OVERWRITE_EN        0x1        /* mask bit 0 */
#define CFG_SCRAMBLE_OVERWRITE_VALUE     0x2        /* mask bit 1 */
#define CFG_SCRAMBLE_DET_OVERWRITE_EN    0x4        /* mask bit 2 */
#define CFG_SCRAMBLE_DET_OVERWRITE_VALUE 0x8        /* mask bit 3 */
#define CFG_SCRAMBLE_STATUS_SEL          0x10       /* mask bit 4 */
#define CFG_TIMEOUT_THRESH               0x1FFFE0   /* mask bit 20:5 */
#define CFG_SSCP_SCV_CNT                 0x1E00000  /* mask bit 24:21 */
#define CFG_SSCP_USCV_CNT                0x1E000000 /* mask bit 28:25 */

#define REG_SCRAMBLE_THRESH 0x00D4
#define CFG_DEC_SCRAMBLE_THRESH 0xFFFF /* mask bit 15:0 */

#define REG_SCRAMBLE_STATUS 0x00D8
#define DEC_SCRAMBLE_STATUS  0x7  /* mask bit 2:0 */
#define SCDC_SCRAMBLE_STATUS 0x38 /* mask bit 5:3 */
#define SCRAMBLE_STATUS      0x40 /* mask bit 6 */

#endif /* __HAL_HDMIRX_DPHY_REG_H__ */
