/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Chip-related definitions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-3
 */
#ifndef __HAL_HDMIRX_PRODUCT_H__
#define __HAL_HDMIRX_PRODUCT_H__

#define HDMIRX_CTRL_CNT            2
#define HDMIRX_PHY_CNT             2

#define HDMIRX_AON_REG_P0_PAD_ADDR 0x010FC000
#define HDMIRX_AON_REG_P1_PAD_ADDR 0x008D4000

#define HDMIRX_PWD_REG_P0_PAD_ADDR 0x01100000
#define HDMIRX_PWD_REG_P1_PAD_ADDR 0x01140000

#define HDMIRX_HDCP2X_REG_P0_PAD_ADDR 0x01118000
#define HDMIRX_HDCP2X_REG_P1_PAD_ADDR 0x01158000

#define HDMIRX_HDCP1X_REG_P0_PAD_ADDR 0x0111A000
#define HDMIRX_HDCP1X_REG_P1_PAD_ADDR 0x0115A000

#define HDMIRX_DPHY_REG_P0_PAD_ADDR 0x0111B000
#define HDMIRX_DPHY_REG_P1_PAD_ADDR 0x0115B000

#define HDMIRX_DAMIX_REG_P0_PAD_ADDR 0x0111C000
#define HDMIRX_DAMIX_REG_P1_PAD_ADDR 0x0115C000

#define HDMIRX_FRL_RAM_P0_PAD_ADDR 0x01120000
#define HDMIRX_FRL_RAM_P1_PAD_ADDR 0x01160000

#define HDMIRX_HDCP_RAM_P0_PAD_ADDR 0x01130000
#define HDMIRX_HDCP_RAM_P1_PAD_ADDR 0x01170000

#define HDMIRX_AON_REGS_SIZE    0x8000
#define HDMIRX_PWD_REGS_SIZE    0x10000
#define HDMIRX_HDCP2X_REGS_SIZE 0x1000
#define HDMIRX_HDCP1X_REGS_SIZE 0x1000
#define HDMIRX_DPHY_REGS_SIZE   0x1000
#define HDMIRX_DAMIX_REGS_SIZE  0x1000
#define HDMIRX_HDCP_RAM_SIZE    0x10000
#define HDMIRX_FRL_RAM_SIZE     0x10000

#define HI_BOARD_HDMIRX_PORT0_HPD_INVERT  1
#define HI_BOARD_HDMIRX_PORT1_HPD_INVERT  1

#endif /* __HAL_HDMIRX_PRODUCT_H__ */