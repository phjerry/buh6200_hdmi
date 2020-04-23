/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of pwd module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_PWD_REG_H__
#define __HAL_HDMIRX_PWD_REG_H__

#define REG_VERSION_INFO 0x0
#define HDMIRX_REVISION       0xFFFF     /* mask bit 15:0 */
#define HDMIRX_IDENTIFICATION 0xFFFF0000 /* mask bit 31:16 */

#define REG_FEATURE_INFO 0x4
#define HDMIRX_BASIC_FEATURE 0xFFF      /* mask bit 11:0 */
#define HDMIRX_DPCLR_FEATURE 0xF000     /* mask bit 15:12 */
#define HDMIRX_AUDIO_FEATURE 0xF0000    /* mask bit 19:16 */
#define HDMIRX_HDCP_FEATURE  0xFF00000  /* mask bit 27:20 */
#define HDMIRX_DPK_FEATURE   0xF0000000 /* mask bit 31:28 */

#define REG_INTR_HDMIRX_PWD 0x10
#define HDMIRX_PWD_INTR_DET0 0x10000  /* mask bit 16 */
#define HDMIRX_PWD_INTR_DET1 0x20000  /* mask bit 17 */
#define HDMIRX_PWD_INTR_DET2 0x40000  /* mask bit 18 */
#define HDMIRX_PWD_INTR_DET3 0x80000  /* mask bit 19 */
#define HDMIRX_PWD_INTR_DET4 0x100000 /* mask bit 20 */
#define HDMIRX_PWD_INTR_DET5 0x200000 /* mask bit 21 */
#define HDMIRX_PWD_INTR_DET6 0x400000 /* mask bit 22 */

#define REG_RX_PWD_INTR_MASK 0x14
#define HDMIRX_PWD_INTR_DET0_MASK 0x10000  /* mask bit 16 */
#define HDMIRX_PWD_INTR_DET1_MASK 0x20000  /* mask bit 17 */
#define HDMIRX_PWD_INTR_DET2_MASK 0x40000  /* mask bit 18 */
#define HDMIRX_PWD_INTR_DET3_MASK 0x80000  /* mask bit 19 */
#define HDMIRX_PWD_INTR_DET4_MASK 0x100000 /* mask bit 20 */
#define HDMIRX_PWD_INTR_DET5_MASK 0x200000 /* mask bit 21 */
#define HDMIRX_PWD_INTR_DET6_MASK 0x400000 /* mask bit 22 */

#define REG_RX_PWD_INTR 0x18
#define HDMIRX_PWD_INT_DEPACK 0x1      /* mask bit 0 */
#define HDMIRX_PWD_INT_VIDEO  0x2      /* mask bit 1 */
#define HDMIRX_PWD_INT_AUDIO  0x4      /* mask bit 2 */
#define HDMIRX_PWD_INT_FRL    0x8      /* mask bit 3 */
#define HDMIRX_PWD_INT_HDCP2X 0x10     /* mask bit 4 */
#define HDMIRX_PWD_INT_HDCP1X 0x20     /* mask bit 5 */
#define HDMIRX_PWD_INT_ARC    0x40     /* mask bit 6 */
#define HDMIRX_PWD_INT_EMP    0x80     /* mask bit 7 */
#define HDMIRX_PWD_INT_TMDS   0x100    /* mask bit 8 */
#define HDMIRX_PWD_INT_DPHY   0x200    /* mask bit 9 */
#define HDMIRX_PWD_INT_DAMIX  0x400    /* mask bit 10 */
#define HDMIRX_PWD_INT_DET0   0x10000  /* mask bit 16 */
#define HDMIRX_PWD_INT_DET1   0x20000  /* mask bit 17 */
#define HDMIRX_PWD_INT_DET2   0x40000  /* mask bit 18 */
#define HDMIRX_PWD_INT_DET3   0x80000  /* mask bit 19 */
#define HDMIRX_PWD_INT_DET4   0x100000 /* mask bit 20 */
#define HDMIRX_PWD_INT_DET5   0x200000 /* mask bit 21 */
#define HDMIRX_PWD_INT_DET6   0x400000 /* mask bit 22 */

#define REG_RX_PWD_STATUS 0x1C
#define PWD_STAT_MUTED            0x1     /* mask bit 0 */
#define PWD_STAT_DPHY_SCDT        0x10    /* mask bit 4 */
#define PWD_STAT_HDCP_DET_STAT    0x20    /* mask bit 5 */
#define PWD_STAT_AON_PWR5V        0x100   /* mask bit 8 */
#define PWD_STAT_AON_HPD          0x200   /* mask bit 9 */
#define PWD_STAT_CLK_DET_TMDS     0x1000  /* mask bit 12 */
#define PWD_STAT_CLK_DET_TMDS_D4  0x2000  /* mask bit 13 */
#define PWD_STAT_CLK_DET_AUD_DACR 0x4000  /* mask bit 14 */
#define PWD_STAT_FRL_RATE         0xF0000 /* mask bit 19:16 */

#define REG_RX_PWD_SRST 0x20
#define HDMIRX_PWD_SRST_REQ            0x1        /* mask bit 0 */
#define CFG_DPHY_CHANNEL_FIFO_SRST_REQ 0x10       /* mask bit 4 */
#define CFG_DPHY_DOMAIN_FIFO_SRST_REQ  0x20       /* mask bit 5 */
#define CFG_DAMIX_SRST_REQ             0x40       /* mask bit 6 */
#define CFG_DPHY_SRST_REQ              0x100      /* mask bit 8 */
#define CFG_TMDS_SRST_REQ              0x200      /* mask bit 9 */
#define CFG_FRL_SRST_REQ               0x400      /* mask bit 10 */
#define CFG_FRL_MCU_SRST_REQ           0x800      /* mask bit 11 */
#define CFG_SCDC_SRST_REQ              0x1000     /* mask bit 12 */
#define CFG_VID_FIFO_SRST_REQ          0x10000    /* mask bit 16 */
#define CFG_VID_SRST_REQ               0x20000    /* mask bit 17 */
#define CFG_AUD_FIFO_SRST_REQ          0x40000    /* mask bit 18 */
#define CFG_AUD_SRST_REQ               0x80000    /* mask bit 19 */
#define CFG_DPK_SRST_REQ               0x100000   /* mask bit 20 */
#define CFG_BIST_SRST_REQ              0x200000   /* mask bit 21 */
#define CFG_HDCPD_SRST_REQ             0x1000000  /* mask bit 24 */
#define CFG_HDCP1X_CIPHER_SRST_REQ     0x2000000  /* mask bit 25 */
#define CFG_HDCP1X_SRST_REQ            0x4000000  /* mask bit 26 */
#define CFG_HDCP1X_KEYOP_SRST_REQ      0x8000000  /* mask bit 27 */
#define CFG_HDCP2X_SRST_REQ            0x10000000 /* mask bit 28 */
#define CFG_HDCP2X_AUTH_SRST_REQ       0x20000000 /* mask bit 29 */
#define CFG_HDCP2X_CYP_SRST_REQ        0x40000000 /* mask bit 30 */
#define CFG_HDCP2X_MCU_SRST_REQ        0x80000000 /* mask bit 31 */

#define REG_RX_PWD_CKEN 0x24
#define CFG_CKEN_DPK            0x100     /* mask bit 8 */
#define CFG_CKEN_DPK_EMP        0x200     /* mask bit 9 */
#define CFG_CKEN_TMDS_DEC       0x1000    /* mask bit 12 */
#define CFG_CKEN_FRL            0x2000    /* mask bit 13 */
#define CFG_CKEN_AUD_AAC        0x100000  /* mask bit 20 */
#define CFG_CKEN_AUD_DACR_RFCLK 0x200000  /* mask bit 21 */
#define CFG_CKEN_HDCP1X         0x1000000 /* mask bit 24 */
#define CFG_CKEN_HDCP2X         0x2000000 /* mask bit 25 */

#define REG_RX_PWD_SEL 0x28
#define CFG_FRL_MODE_EN                   0x1      /* mask bit 0 */
#define CFG_FRL_DYN_CKGT_EN               0x2      /* mask bit 1 */
#define CFG_FRL_4LN_EN                    0x4      /* mask bit 2 */
#define CFG_SCDC_FRL_EN                   0x8      /* mask bit 3 */
#define CFG_SCDC_CED_CLR_SEL              0x10     /* mask bit 4 */
#define CKSEL_AUD_MCLK                    0x100    /* mask bit 8 */
#define CFG_HDCP_MODE_SW_EN               0x1000   /* mask bit 12 */
#define CFG_HDCP_MODE_SW_VAL              0x2000   /* mask bit 13 */
#define CFG_SYS_MUTE_SW_EN                0x4000   /* mask bit 14 */
#define CFG_SYS_MUTE_SW_VAL               0x8000   /* mask bit 15 */
#define CFG_DPHY_CHANNEL_FIFO_RST_LANE_EN 0x10000  /* mask bit 16 */
#define CFG_DMIX_PLL_UNLOCK_RST_DPHY_EN   0x20000  /* mask bit 17 */
#define CFG_FRL_RATE_SW_EN                0x40000  /* mask bit 18 */
#define CFG_FRL_RATE_SW_VAL               0x780000 /* mask bit 22:19 */

#define REG_RX_PWD_CTRL_CMD 0x2C
#define CFG_SYS_CLR_MUTE 0x1 /* mask bit 0 */
#define CFG_SYS_SET_MUTE 0x2 /* mask bit 1 */

#define REG_RX_PWD_HW_RST_OVERRIDES 0x40
#define CFG_AFIFO_AUTO_RST_OVERRIDE     0x1 /* mask bit 0 */
#define CFG_VFIFO_AUTO_RST_OVERRIDE     0x2 /* mask bit 1 */
#define CFG_TCLK_DET_CHG_RST_OVERRIDE   0x4 /* mask bit 2 */
#define CFG_PCLK_RATIO_CHG_RST_OVERRIDE 0x8 /* mask bit 3 */

#define REG_RX_PWD_CLK_DET_TLR 0x44
#define CFG_CLK_DET_TLR0 0x300   /* mask bit 9:8 */
#define CFG_CLK_DET_TLR1 0xC00   /* mask bit 11:10 */
#define CFG_CLK_DET_TLR2 0x3000  /* mask bit 13:12 */
#define CFG_CLK_DET_TLR3 0xC000  /* mask bit 15:14 */
#define CFG_CLK_CHK_TLR  0x30000 /* mask bit 17:16 */

#define REG_HDCP1X_EVENT_MASK 0x48
#define CFG_HDCP1X_EVENT_MASK_FOR_AUDIO_UNMUTE 0xF     /* mask bit 3:0 */
#define CFG_HDCP1X_EVENT_MASK_FOR_AUDIO        0xF00   /* mask bit 11:8 */
#define CFG_HDCP1X_EVENT_MASK_FOR_VIDEO        0xF0000 /* mask bit 19:16 */

#define REG_HDCP2X_EVENT_MASK 0x4C
#define CFG_HDCP2X_EVENT_MASK_FOR_AUDIO_UNMUTE 0xFF     /* mask bit 7:0 */
#define CFG_HDCP2X_EVENT_MASK_FOR_AUDIO        0xFF00   /* mask bit 15:8 */
#define CFG_HDCP2X_EVENT_MASK_FOR_VIDEO        0xFF0000 /* mask bit 23:16 */

#define REG_PWD_EVENT_OVERRIDE 0x50
#define CFG_EVENT_OVERRIDE_TMDS   0x1  /* mask bit 0 */
#define CFG_EVENT_OVERRIDE_FRL    0x2  /* mask bit 1 */
#define CFG_EVENT_OVERRIDE_DEPACK 0x4  /* mask bit 2 */
#define CFG_EVENT_OVERRIDE_AUDIO  0x8  /* mask bit 3 */
#define CFG_EVENT_OVERRIDE_VIDEO  0x10 /* mask bit 4 */
#define CFG_EVENT_OVERRIDE_SYS    0x20 /* mask bit 5 */

#define REG_SYS_EVENT_CFG_0 0x54
#define CFG_SYS_EVENT_MASK_FOR_AUDIO_UNMUTE 0x7F     /* mask bit 6:0 */
#define CFG_SYS_EVENT_MASK_FOR_AUDIO        0x7F00   /* mask bit 14:8 */
#define CFG_SYS_EVENT_MASK_FOR_VIDEO        0x7F0000 /* mask bit 22:16 */

#define REG_SYS_EVENT_CFG_1 0x58
#define CFG_SYS_EVENT_MASK_FOR_DEPACK 0x7F   /* mask bit 6:0 */
#define CFG_SYS_EVENT_MASK_FOR_DECODE 0x7F00 /* mask bit 14:8 */

#define REG_VID_EVENT_MASK 0x60
#define CFG_VID_EVENT_MASK 0x7FFFFF /* mask bit 22:0 */

#define REG_AUD_EVENT_MASK 0x64
#define CFG_AUD_EVENT_MASK_MUTE          0x7FFFF /* mask bit 18:0 */
#define CFG_VID_RSLT_MASK_FOR_AUDIO_MUTE 0x80000 /* mask bit 19 */

#define REG_AUD_EVENT_MASK_1 0x68
#define CFG_AUD_EVENT_MASK_UNMUTE          0x7FFFF /* mask bit 18:0 */
#define CFG_VID_RSLT_MASK_FOR_AUDIO_UNMUTE 0x80000 /* mask bit 19 */

#define REG_TMDS_EVENT_MASK 0x6C
#define CFG_TMDS_EVENT_MASK                  0xFF       /* mask bit 7:0 */
#define CFG_TMDS_EVENT_MASK_FOR_VIDEO        0xFF00     /* mask bit 15:8 */
#define CFG_TMDS_EVENT_MASK_FOR_AUDIO        0xFF0000   /* mask bit 23:16 */
#define CFG_TMDS_EVENT_MASK_FOR_AUDIO_UNMUTE 0xFF000000 /* mask bit 31:24 */

#define REG_FRL_EVENT_MASK 0x70
#define CFG_FRL_EVENT_MASK_FOR_AUDIO_UNMUTE 0xFF     /* mask bit 7:0 */
#define CFG_FRL_EVENT_MASK_FOR_AUDIO        0xFF00   /* mask bit 15:8 */
#define CFG_FRL_EVENT_MASK_FOR_VIDEO        0xFF0000 /* mask bit 23:16 */

#define REG_DPK_EVENT_MASK_FOR_AUDIO 0x74
#define CFG_DPK_EVENT_MASK_FOR_AUDIO 0x7FFF /* mask bit 14:0 */

#define REG_DPK_EVENT_MASK_FOR_VIDEO 0x78
#define CFG_DPK_EVENT_MASK_FOR_VIDEO 0x7FFF /* mask bit 14:0 */

#define REG_AUD_EVENT_MASK_FOR_VIDEO 0x7C
#define CFG_AUD_EVENT_MASK_FOR_VIDEO 0x7FFFF /* mask bit 18:0 */

#define REG_VID_EVENT_MASK_FOR_AUD 0x80
#define CFG_VID_EVENT_MASK_FOR_AUDIO 0x7FFFFF /* mask bit 22:0 */

#define REG_VID_EVENT_MASK_FOR_AUD_UNMUTE 0x84
#define CFG_VID_EVENT_MASK_FOR_AUDIO_UNMUTE 0x7FFFFF /* mask bit 22:0 */

#define REG_DPK_EVENT_MASK_FOR_AUD_UNMUTE 0x88
#define CFG_DPK_EVENT_MASK_FOR_AUDIO_UNMUTE 0x7FFF /* mask bit 14:0 */

#define REG_RX_PWD_CLK_DET_TMDS_STAT 0x90
#define DET_CLK0_FEAT_EXIST 0x1        /* mask bit 0 */
#define DET_CLK0_STAT       0x10       /* mask bit 4 */
#define DET_CLK0_FREQ       0xFFFFF000 /* mask bit 31:12 */

#define REG_RX_PWD_CLK_DET_TMDS_D4_STAT 0x94
#define DET_CLK1_FEAT_EXIST 0x1        /* mask bit 0 */
#define DET_CLK1_STAT       0x10       /* mask bit 4 */
#define DET_CLK1_FREQ       0xFFFFF000 /* mask bit 31:12 */

#define REG_RX_PWD_CLK_DET_AUD_DACR_STAT 0x98
#define DET_CLK2_FEAT_EXIST 0x1        /* mask bit 0 */
#define DET_CLK2_STAT       0x10       /* mask bit 4 */
#define DET_CLK2_FREQ       0xFFFFF000 /* mask bit 31:12 */

#define REG_RX_PWD_CLK_DET3_STAT 0x9C
#define DET_CLK3_FEAT_EXIST 0x1        /* mask bit 0 */
#define DET_CLK3_STAT       0x10       /* mask bit 4 */
#define DET_CLK3_FREQ       0xFFFFF000 /* mask bit 31:12 */

#define REG_RX_PWD_CLK_CHK_STAT 0xA0
#define CHK_CLK_FEAT_EXIST 0x1        /* mask bit 0 */
#define CHK_CLK_STAT       0x10       /* mask bit 4 */
#define CHK_CLK_FREQ       0xFFFFF000 /* mask bit 31:12 */

#define REG_MEM_CTRL_L 0xB0
#define MEM_DBUS_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MEM_CTRL_H 0xB4
#define MEM_DBUS_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_RX_PWD_DBG_UART_SEL 0xDC
#define CFG_RX_UART_SEL 0x1 /* mask bit 0 */

#endif /* __HAL_HDMIRX_PWD_REG_H__ */
