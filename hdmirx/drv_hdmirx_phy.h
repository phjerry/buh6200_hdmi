/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of phy functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_PHY_H__
#define __DRV_HDMIRX_PHY_H__

#include "hal_hdmirx_comm.h"

typedef enum {
    PHY_STATE_OFF,
    PHY_STATE_HANDLE_EQ,
    PHY_STATE_AUTO_EQ,
    PHY_STATE_DFE,
    PHY_STATE_DONE,
    PHY_STATE_MAX
} phy_state;

typedef struct {
    hi_bool run;
    phy_state state;
    hdmirx_input_type type;
    hdmirx_dfe_mode dfe_mode;
    hi_u8 repeat_cnt;
    hi_u32 wait_cnt;
} hdmirx_phy_ctx;

#define SLEEP_TIME_CNT 10
#define PHY_GET_CTX(port) (&g_hdmirx_phy_ctx[port])
#define PHY_STATE_OFF_TIME_CNT 300
#define PHY_STATE_HANDLE_EQ_TIME_CNT 300
#define PHY_STATE_HANDLE_EQ_REPEAT_CNT 3
#define PHY_STATE_20_AUTO_EQ_TIME_CNT 300
#define PHY_STATE_20_AUTO_EQ_REPEAT_CNT 2
#define PHY_STATE_21_AUTO_EQ_TIME_CNT 60
#define PHY_STATE_21_AUTO_EQ_REPEAT_CNT 1
#define PHY_STATE_21_AUTO_DFE_TIME_CNT 15

hi_void drv_hdmirx_phy_init(hi_drv_hdmirx_port port);

hi_void drv_hdmirx_phy_set_en(hi_drv_hdmirx_port port, hdmirx_input_type type, hi_bool en);

hi_void drv_hdmirx_phy_proc(hi_drv_hdmirx_port port);

#endif