/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of aon module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_AON_REG_H__
#define __HAL_HDMIRX_AON_REG_H__

#define REG_RX_HW_VER 0x0
#define RX_RTL_VER 0xFFFF /* mask bit 15:0 */

#define REG_RX_SW_VER 0x4
#define RX_REG_VER 0xFFFF     /* mask bit 15:0 */
#define RX_DRV_VER 0xFFFF0000 /* mask bit 31:16 */

#define REG_RX_HW_INF 0x8
#define RX_PORT0_ON 0x1  /* mask bit 0 */
#define RX_PORT1_ON 0x2  /* mask bit 1 */
#define RX_PORT2_ON 0x4  /* mask bit 2 */
#define RX_PORT3_ON 0x8  /* mask bit 3 */
#define RX_WAKUP_ON 0x10 /* mask bit 4 */
#define RX_CEC_ON   0x20 /* mask bit 5 */
#define RX_CBUS_ON  0x40 /* mask bit 6 */

#define REG_RX_ISO_EN 0x20
#define PWD_ISO_EN 0x1 /* mask bit 0 */

#define REG_RX_PORT_SEL 0x30
#define RX_PORT_SEL 0x3 /* mask bit 1:0 */

#define REG_RX_AON_RST 0x40
#define RX_AON_ARST_REQ 0x1 /* mask bit 0 */
#define RX_DDC_ARST_REQ 0x2 /* mask bit 1 */
#define RX_CEC_ARST_REQ 0x4 /* mask bit 2 */
#define RX_DET_ARST_REQ 0x8 /* mask bit 3 */

#define REG_RX_PWR_ST 0x60
#define PWR5V_DET_ST 0xF /* mask bit 3:0 */

#define REG_RX_AON_INTR_ST 0x70
#define CEC_INT_ST     0x1  /* mask bit 0 */
#define PWR_DET_INT_ST 0x2  /* mask bit 1 */
#define P0_DET_INT_ST  0x4  /* mask bit 2 */
#define P1_DET_INT_ST  0x8  /* mask bit 3 */
#define P2_DET_INT_ST  0x10 /* mask bit 4 */
#define P3_DET_INT_ST  0x20 /* mask bit 5 */

#define REG_PWR_DET_INT 0x80
#define P0_PWR5V_INT 0x1 /* mask bit 0 */
#define P1_PWR5V_INT 0x2 /* mask bit 1 */
#define P2_PWR5V_INT 0x4 /* mask bit 2 */
#define P3_PWR5V_INT 0x8 /* mask bit 3 */

#define REG_PWR_INT_MASK 0x84
#define P0_PWR5V_INT_MASK 0x1 /* mask bit 0 */
#define P1_PWR5V_INT_MASK 0x2 /* mask bit 1 */
#define P2_PWR5V_INT_MASK 0x4 /* mask bit 2 */
#define P3_PWR5V_INT_MASK 0x8 /* mask bit 3 */

#define REG_MEM_CTRL_B0 0xA0
#define MEM_CTL_LSB 0xFFFFFFFF /* mask bit 31:0 */

#define REG_MEM_CTRL_B1 0xA4
#define MEM_CTL_MSB 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AON_WR_ECO 0xC0
#define AON_ECO_CFG 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AON_RD_ECO 0xC4
#define AON_ECO_STA 0xFFFFFFFF /* mask bit 31:0 */
#endif /* __HAL_HDMIRX_AON_REG_H__ */
