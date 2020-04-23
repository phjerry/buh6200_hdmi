/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: register definition of cec module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_CEC_REG_H__
#define __HAL_HDMIRX_CEC_REG_H__

#define REG_CEC_IRQ_SRC_PND 0x2000
#define RX_FRAME_RDY_SRC        0x1   /* mask bit 0 */
#define RX_BUFFER_OV_SRC        0x2   /* mask bit 1 */
#define FA_REQ_SRC              0x4   /* mask bit 2 */
#define TX0_FRAME_RDY_SRC       0x8   /* mask bit 3 */
#define TX1_FRAME_RDY_SRC       0x10  /* mask bit 4 */
#define TX0_FRAME_RETRY_OUT_SRC 0x20  /* mask bit 5 */
#define TX1_FRAME_RETRY_OUT_SRC 0x40  /* mask bit 6 */
#define FA_RETRY_OUT_SRC        0x80  /* mask bit 7 */
#define AUTO_PING_RDY_SRC       0x100 /* mask bit 8 */
#define START_ABNM_SRC          0x200 /* mask bit 9 */
#define LOGIC_ABNM_SRC          0x400 /* mask bit 10 */
#define VALUE_ABNM_SRC          0x800 /* mask bit 11 */

#define REG_CEC_IRQ_EN_SET 0x2004
#define RX_FRAME_RDY_EN        0x1   /* mask bit 0 */
#define RX_BUFFER_OV_EN        0x2   /* mask bit 1 */
#define FA_REQ_EN              0x4   /* mask bit 2 */
#define TX0_FRAME_RDY_EN       0x8   /* mask bit 3 */
#define TX1_FRAME_RDY_EN       0x10  /* mask bit 4 */
#define TX0_FRAME_RETRY_OUT_EN 0x20  /* mask bit 5 */
#define TX1_FRAME_RETRY_OUT_EN 0x40  /* mask bit 6 */
#define FA_RETRY_OUT_EN        0x80  /* mask bit 7 */
#define AUTO_PING_RDY_EN       0x100 /* mask bit 8 */
#define START_ABNM_EN          0x200 /* mask bit 9 */
#define LOGIC_ABNM_EN          0x400 /* mask bit 10 */
#define VALUE_ABNM_EN          0x800 /* mask bit 11 */

#define REG_CEC_CTRL 0x2008
#define CEC_RCV_EN         0x1  /* mask bit 0 */
#define CEC_TRAN_EN        0x2  /* mask bit 1 */
#define SNOOP_EN           0x4  /* mask bit 2 */
#define FEATURE_ABORT_EN   0x8  /* mask bit 3 */
#define BRCST_INIT_RCV_EN  0x10 /* mask bit 4 */
#define RCV_NACK_HDR       0x20 /* mask bit 5 */
#define DEGLITCH_BYPASS    0x40 /* mask bit 6 */
#define RX_FULL_NACK_BRCST 0x80 /* mask bit 7 */

#define REG_CEC_PULSE 0x200C
#define TX0_FRAME_EN        0x1   /* mask bit 0 */
#define TX0_POLL_MESSAGE_EN 0x2   /* mask bit 1 */
#define TX1_FRAME_EN        0x4   /* mask bit 2 */
#define TX1_POLL_MESSAGE_EN 0x8   /* mask bit 3 */
#define RX_FRAME_CLR        0x10  /* mask bit 4 */
#define AUTO_PING_EN        0x40  /* mask bit 6 */
#define TX0_BUFFER_FLUSH    0x80  /* mask bit 7 */
#define TX1_BUFFER_FLUSH    0x100 /* mask bit 8 */
#define TX_STATE_CLR        0x200 /* mask bit 9 */
#define RX_STATE_CLR        0x400 /* mask bit 10 */
#define RX_FRAME_CLR_ALL    0x800 /* mask bit 11 */

#define REG_CEC_RETRY_NUM 0x2010
#define RETRY_NUM 0x7 /* mask bit 2:0 */

#define REG_CEC_CAPTURE_ID_FLAG 0x2014
#define CAPTURE_ID_FLAG 0xFFFF /* mask bit 15:0 */

#define REG_CEC_SNOOP_ID_FLAG 0x2018
#define SNOOP_ID_FLAG 0xFFFF /* mask bit 15:0 */

#define REG_CEC_CDC 0x201C
#define CEC_CDC_OPCODE  0xFF   /* mask bit 7:0 */
#define CEC_CDC_ARB_CNT 0x1F00 /* mask bit 12:8 */
#define CEC_CDC_ARB_EN  0x2000 /* mask bit 13 */

#define REG_CEC_AUTO_PING_MAP_RSLT 0x2020
#define AUTO_PING_MAP 0x7FFF /* mask bit 14:0 */

#define REG_CEC_AUTO_PING_MAP_EN 0x2024
#define AUTO_PING_MAP_EN 0x7FFF /* mask bit 14:0 */

#define REG_CEC_VERSION 0x2028
#define CEC_RTL_VERSION 0xFF     /* mask bit 7:0 */
#define CEC_REG_VERSION 0xFF00   /* mask bit 15:8 */
#define CEC_DRV_VERSION 0xFF0000 /* mask bit 23:16 */

#define REG_CEC_COMPENSATE 0x202C
#define CEC_COMPENSATE 0x1FF /* mask bit 8:0 */

#define REG_CEC_FA_DEBUG 0x2030
#define FA_DEST_ADDR 0xF    /* mask bit 3:0 */
#define FA_INIT_ADDR 0xF0   /* mask bit 7:4 */
#define FA_OPCODE    0xFF00 /* mask bit 15:8 */

#define REG_CEC_FA_OPCODE0 0x2034
#define FA_OPCODE_FLAG0 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_FA_OPCODE1 0x2038
#define FA_OPCODE_FLAG1 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_FA_OPCODE2 0x203C
#define FA_OPCODE_FLAG2 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_FA_OPCODE3 0x2040
#define FA_OPCODE_FLAG3 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_FA_OPCODE4 0x2044
#define FA_OPCODE_FLAG4 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_FA_OPCODE5 0x2048
#define FA_OPCODE_FLAG5 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_FA_OPCODE6 0x204C
#define FA_OPCODE_FLAG6 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_FA_OPCODE7 0x2050
#define FA_OPCODE_FLAG7 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_UMONITOR0 0x2054
#define TX0_BUFFER_ST 0x1      /* mask bit 0 */
#define TX1_BUFFER_ST 0x2      /* mask bit 1 */
#define RX_BUFFER0_ST 0x4      /* mask bit 2 */
#define RX_BUFFER1_ST 0x8      /* mask bit 3 */
#define RX_BUFFER2_ST 0x10     /* mask bit 4 */
#define RX_BUFFER3_ST 0x20     /* mask bit 5 */
#define RETRY_CNT     0x1C0    /* mask bit 8:6 */
#define CEC_TX_STATE  0x3E00   /* mask bit 13:9 */
#define CEC_RX_STATE  0x3C000  /* mask bit 17:14 */
#define CTRL_STATE    0x380000 /* mask bit 21:19 */

#define REG_CEC_UMONITOR1 0x2058
#define START_LOW_LONG   0x1     /* mask bit 0 */
#define START_LOW_SHORT  0x2     /* mask bit 1 */
#define START_BIT_LONG   0x4     /* mask bit 2 */
#define START_BIT_SHORT  0x8     /* mask bit 3 */
#define LOGIC0_LOW_LONG  0x10    /* mask bit 4 */
#define LOGIC_LOW_MIDDLE 0x20    /* mask bit 5 */
#define LOGIC1_LOW_SHORT 0x40    /* mask bit 6 */
#define LOGIC_BIT_LONG   0x80    /* mask bit 7 */
#define LOGIC_BIT_SHORT  0x100   /* mask bit 8 */
#define LOGIC_ERROR      0x200   /* mask bit 9 */
#define ARBI_FAIL        0x400   /* mask bit 10 */
#define TX0_NO_ACK       0x800   /* mask bit 11 */
#define TX1_NO_ACK       0x1000  /* mask bit 12 */
#define TX0_POLL_NO_ACK  0x2000  /* mask bit 13 */
#define TX1_POLL_NO_ACK  0x4000  /* mask bit 14 */
#define FA_NO_ACK        0x8000  /* mask bit 15 */
#define AP_NO_ACK        0x10000 /* mask bit 16 */

#define REG_CEC_RESERVED 0x2080
#define CEC_RESERVED 0xFFFFFFFF /* mask bit 31:0 */

#define REG_CEC_TX0_HEAD_ADDR 0x2100
#define TX0_DEST_ADDR 0xF  /* mask bit 3:0 */
#define TX0_INIT_ADDR 0xF0 /* mask bit 7:4 */

#define REG_CEC_TX0_FRAME_LENGTH 0x2104
#define TX0_FRAME_LENGTH 0xF /* mask bit 3:0 */

#define REG_CEC_TX0_FRAME_DATA0 0x2108
#define TX0_FRAME_DATA_BYTE0 0xFF       /* mask bit 7:0 */
#define TX0_FRAME_DATA_BYTE1 0xFF00     /* mask bit 15:8 */
#define TX0_FRAME_DATA_BYTE2 0xFF0000   /* mask bit 23:16 */
#define TX0_FRAME_DATA_BYTE3 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_TX0_FRAME_DATA1 0x210C
#define TX0_FRAME_DATA_BYTE4 0xFF       /* mask bit 7:0 */
#define TX0_FRAME_DATA_BYTE5 0xFF00     /* mask bit 15:8 */
#define TX0_FRAME_DATA_BYTE6 0xFF0000   /* mask bit 23:16 */
#define TX0_FRAME_DATA_BYTE7 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_TX0_FRAME_DATA2 0x2110
#define TX0_FRAME_DATA_BYTE8  0xFF       /* mask bit 7:0 */
#define TX0_FRAME_DATA_BYTE9  0xFF00     /* mask bit 15:8 */
#define TX0_FRAME_DATA_BYTE10 0xFF0000   /* mask bit 23:16 */
#define TX0_FRAME_DATA_BYTE11 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_TX0_FRAME_DATA3 0x2114
#define TX0_FRAME_DATA_BYTE12 0xFF     /* mask bit 7:0 */
#define TX0_FRAME_DATA_BYTE13 0xFF00   /* mask bit 15:8 */
#define TX0_FRAME_DATA_BYTE14 0xFF0000 /* mask bit 23:16 */

#define REG_CEC_TX1_HEAD_ADDR 0x2120
#define TX1_DEST_ADDR 0xF  /* mask bit 3:0 */
#define TX1_INIT_ADDR 0xF0 /* mask bit 7:4 */

#define REG_CEC_TX1_FRAME_LENGTH 0x2124
#define TX1_FRAME_LENGTH 0xF /* mask bit 3:0 */

#define REG_CEC_TX1_FRAME_DATA0 0x2128
#define TX1_FRAME_DATA_BYTE0 0xFF       /* mask bit 7:0 */
#define TX1_FRAME_DATA_BYTE1 0xFF00     /* mask bit 15:8 */
#define TX1_FRAME_DATA_BYTE2 0xFF0000   /* mask bit 23:16 */
#define TX1_FRAME_DATA_BYTE3 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_TX1_FRAME_DATA1 0x212C
#define TX1_FRAME_DATA_BYTE4 0xFF       /* mask bit 7:0 */
#define TX1_FRAME_DATA_BYTE5 0xFF00     /* mask bit 15:8 */
#define TX1_FRAME_DATA_BYTE6 0xFF0000   /* mask bit 23:16 */
#define TX1_FRAME_DATA_BYTE7 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_TX1_FRAME_DATA2 0x2130
#define TX1_FRAME_DATA_BYTE8  0xFF       /* mask bit 7:0 */
#define TX1_FRAME_DATA_BYTE9  0xFF00     /* mask bit 15:8 */
#define TX1_FRAME_DATA_BYTE10 0xFF0000   /* mask bit 23:16 */
#define TX1_FRAME_DATA_BYTE11 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_TX1_FRAME_DATA3 0x2134
#define TX1_FRAME_DATA_BYTE12 0xFF     /* mask bit 7:0 */
#define TX1_FRAME_DATA_BYTE13 0xFF00   /* mask bit 15:8 */
#define TX1_FRAME_DATA_BYTE14 0xFF0000 /* mask bit 23:16 */

#define REG_CEC_RX_FRAME_LENGTH 0x2140
#define RX_FRAME_LENGTH 0xF /* mask bit 3:0 */

#define REG_CEC_RX_HEAD_ADDR 0x2144
#define RX_DEST_ADDR 0xF  /* mask bit 3:0 */
#define RX_INIT_ADDR 0xF0 /* mask bit 7:4 */

#define REG_CEC_RX_FRAME_DATA0 0x2148
#define RX_FRAME_DATA_BYTE0 0xFF       /* mask bit 7:0 */
#define RX_FRAME_DATA_BYTE1 0xFF00     /* mask bit 15:8 */
#define RX_FRAME_DATA_BYTE2 0xFF0000   /* mask bit 23:16 */
#define RX_FRAME_DATA_BYTE3 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_RX_FRAME_DATA1 0x214C
#define RX_FRAME_DATA_BYTE4 0xFF       /* mask bit 7:0 */
#define RX_FRAME_DATA_BYTE5 0xFF00     /* mask bit 15:8 */
#define RX_FRAME_DATA_BYTE6 0xFF0000   /* mask bit 23:16 */
#define RX_FRAME_DATA_BYTE7 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_RX_FRAME_DATA2 0x2150
#define RX_FRAME_DATA_BYTE8  0xFF       /* mask bit 7:0 */
#define RX_FRAME_DATA_BYTE9  0xFF00     /* mask bit 15:8 */
#define RX_FRAME_DATA_BYTE10 0xFF0000   /* mask bit 23:16 */
#define RX_FRAME_DATA_BYTE11 0xFF000000 /* mask bit 31:24 */

#define REG_CEC_RX_FRAME_DATA3 0x2154
#define RX_FRAME_DATA_BYTE12 0xFF     /* mask bit 7:0 */
#define RX_FRAME_DATA_BYTE13 0xFF00   /* mask bit 15:8 */
#define RX_FRAME_DATA_BYTE14 0xFF0000 /* mask bit 23:16 */

#define REG_CEC_RX_FRAME_CNT 0x2158
#define RX_FRAME_CNT 0x7 /* mask bit 2:0 */
#endif /* __HAL_HDMIRX_CEC_REG_H__ */
