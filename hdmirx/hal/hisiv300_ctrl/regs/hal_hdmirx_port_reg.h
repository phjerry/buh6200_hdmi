/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of aon port module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_PORT_REG_H__
#define __HAL_HDMIRX_PORT_REG_H__

#define REG_PWR_DET_SRC 0x1010
#define PWR_DET_SRC 0x1 /* mask bit 0 */

#define REG_PWR_DET_PCTL 0x1014
#define PWR_DET_PCTRL 0x1 /* mask bit 0 */

#define REG_PWR_FILT_CTRL 0x1018
#define PWR_FILT_EN 0x1 /* mask bit 0 */

#define REG_PWR_FILT_RESH 0x101C
#define PWR_FILT_POS_RES 0x1FFF     /* mask bit 12:0 */
#define PWR_FILT_NEG_RES 0x1FFF0000 /* mask bit 28:16 */

#define REG_PWR_DET_ST 0x1020
#define PWR_DET_STAT 0x1 /* mask bit 0 */

#define REG_PWR_SOFT_CTRL 0x1024
#define PWR_SOFT_EN  0x1 /* mask bit 0 */
#define PWR_SOFT_CFG 0x2 /* mask bit 1 */

#define REG_SCL_DET_PCTL 0x1040
#define SCL_DET_PCTRL 0x1 /* mask bit 0 */

#define REG_SCL_FILT_CTRL 0x1044
#define SCL_FILT_EN 0x1 /* mask bit 0 */

#define REG_SCL_FILT_RESH 0x1048
#define SCL_FILT_POS_RES 0x1FFF     /* mask bit 12:0 */
#define SCL_FILT_NEG_RES 0x1FFF0000 /* mask bit 28:16 */

#define REG_SCL_DET_ST 0x104C
#define SCL_DET_STAT 0x1 /* mask bit 0 */

#define REG_SCL_SOFT_CTRL 0x1050
#define SCL_SOFT_EN  0x1 /* mask bit 0 */
#define SCL_SOFT_CFG 0x2 /* mask bit 1 */

#define REG_PWR5V_DET_SEL 0x1060
#define PWR5V_DET_SEL 0x1 /* mask bit 0 */

#define REG_PWR5V_SOFT_CTRL 0x1064
#define PWR5V_SOFT_EN  0x1 /* mask bit 0 */
#define PWR5V_SOFT_CFG 0x2 /* mask bit 1 */

#define REG_CLK_DET_ST 0x1070
#define CLK_DET_STAT 0x1 /* mask bit 0 */

#define REG_CLK_DET_FREQ 0x1074
#define CLK_DET_FREQ 0xFFFF /* mask bit 15:0 */

#define REG_RX_HPD_CFG 0x1090
#define RX_HPD_OEN  0x1 /* mask bit 0 */
#define RX_HPD_DATA 0x2 /* mask bit 1 */

#define REG_RX_HPD_CTRL 0x1094
#define HPD_LOW_RESH 0x1FF  /* mask bit 8:0 */
#define HPD_POL_CTL  0x1000 /* mask bit 12 */
#define HPD_DET_MODE 0xE000 /* mask bit 15:13 */

#define REG_DDC_SLV_CTL 0x10A0
#define DDC_SLV_EN 0x1 /* mask bit 0 */

#define REG_DDC_DEV_CTL 0x10A4
#define DDC_EDID_EN 0x1 /* mask bit 0 */
#define DDC_HDCP_EN 0x2 /* mask bit 1 */
#define DDC_SCDC_EN 0x4 /* mask bit 2 */

#define REG_DDC_FILT_CTL 0x10A8
#define DDC_FILT_SEL 0x3 /* mask bit 1:0 */

#define REG_DDC_DLY_CTL 0x10AC
#define SDA_IN_DLY_CNT  0xF  /* mask bit 3:0 */
#define SDA_OUT_DLY_CNT 0xF0 /* mask bit 7:4 */

#define REG_DDC_SLV_RST 0x10B0
#define DDC_HPD_RST_EN  0x1 /* mask bit 0 */
#define DDC_PWR_RST_EN  0x2 /* mask bit 1 */
#define DDC_TOUT_RST_EN 0x4 /* mask bit 2 */

#define REG_DDC_TOUT_RES 0x10B4
#define DDC_TOUT_RESHOLD 0xFF /* mask bit 7:0 */

#define REG_DDC_STRETCH_CTL 0x10B8
#define SCL_HOLD_RESH 0xFF /* mask bit 7:0 */

#define REG_DDC_SEG_LIMIT 0x10BC
#define EDID_SEG_LIMIT 0xFF /* mask bit 7:0 */

#define REG_DDC_HDCP_CTL 0x10C0
#define HDCP_SOFT_EN    0x1 /* mask bit 0 */
#define HDCP_SOFT_CFG   0x2 /* mask bit 1 */
#define HDCP_RFIFO_SKIP 0x4 /* mask bit 2 */
#define HDCP_WFIFO_SKIP 0x8 /* mask bit 3 */

#define REG_DDC_MAN_CTL 0x10E0
#define SCL_MANU_EN 0x1 /* mask bit 0 */
#define SDA_MANU_EN 0x2 /* mask bit 1 */

#define REG_DDC_MAN_CFG 0x10E4
#define SCL_DRV_EN 0x1 /* mask bit 0 */
#define SDA_DRV_EN 0x2 /* mask bit 1 */

#define REG_DDC_MAN_ST 0x10E8
#define DDC_SCL_STAT 0x1 /* mask bit 0 */
#define DDC_SDA_STAT 0x2 /* mask bit 1 */

#define REG_EDID_MOD_B0_CTL 0x1100
#define EDID_MOD_B0_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B0_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B0_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_MOD_B1_CTL 0x1104
#define EDID_MOD_B1_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B1_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B1_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_MOD_B2_CTL 0x1108
#define EDID_MOD_B2_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B2_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B2_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_MOD_B3_CTL 0x110C
#define EDID_MOD_B3_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B3_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B3_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_MOD_B4_CTL 0x1110
#define EDID_MOD_B4_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B4_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B4_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_MOD_B5_CTL 0x1114
#define EDID_MOD_B5_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B5_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B5_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_MOD_B6_CTL 0x1118
#define EDID_MOD_B6_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B6_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B6_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_MOD_B7_CTL 0x111C
#define EDID_MOD_B7_EN   0x1       /* mask bit 0 */
#define EDID_MOD_B7_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_MOD_B7_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_EDID_LIMIT_CTL 0x1130
#define EDID_LIMIT_EN   0x1       /* mask bit 0 */
#define EDID_LIMIT_MODE 0x2       /* mask bit 1 */
#define EDID_TYPE_LIMIT 0xC       /* mask bit 3:2 */
#define EDID_LIMIT_DATA 0xFF00    /* mask bit 15:8 */
#define EDID_LIMIT_ADDR 0x3FF0000 /* mask bit 25:16 */

#define REG_DDC_OPER_CNT 0x1140
#define DDC_REC_CNT 0x7 /* mask bit 2:0 */

#define REG_DDC_OPER_CTL 0x1144
#define DDC_REC_RD  0x1 /* mask bit 0 */
#define DDC_REC_CLR 0x2 /* mask bit 1 */

#define REG_DDC_OPER_REC 0x1148
#define DDC_REC_LEN 0x7FF      /* mask bit 10:0 */
#define DDC_REC_RWN 0x800      /* mask bit 11 */
#define DDC_REC_OFF 0xFF000    /* mask bit 19:12 */
#define DDC_REC_DEV 0xFF00000  /* mask bit 27:20 */
#define DDC_REC_SEG 0xF0000000 /* mask bit 31:28 */

#define REG_DDC_UNEXP_REC 0x114C
#define REC_AUTO_CLR_CNT    0xF    /* mask bit 3:0 */
#define REC_UNEXP_START_CNT 0xF0   /* mask bit 7:4 */
#define REC_UNEXP_STOP_CNT  0xF00  /* mask bit 11:8 */
#define REC_UNKNOW_DEV_CNT  0xF000 /* mask bit 15:12 */

#define REG_RX_DET_INT 0x1160
#define SCL_DET_INT  0x1  /* mask bit 0 */
#define PWR_DET_INT  0x2  /* mask bit 1 */
#define CLK_DET_INT  0x4  /* mask bit 2 */
#define DDC_SCDC_INT 0x8  /* mask bit 3 */
#define DDC_FRL_INT  0x10 /* mask bit 4 */

#define REG_RX_DET_INT_MASK 0x1164
#define SCL_DET_INT_MASK  0x1  /* mask bit 0 */
#define PWR_DET_INT_MASK  0x2  /* mask bit 1 */
#define CLK_DET_INT_MASK  0x4  /* mask bit 2 */
#define DDC_SCDC_INT_MASK 0x8  /* mask bit 3 */
#define DDC_FRL_INT_MASK  0x10 /* mask bit 4 */
#endif /* __HAL_HDMIRX_PORT_REG_H__ */
