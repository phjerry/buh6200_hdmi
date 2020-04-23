/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of phy functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "drv_hdmirx_phy.h"
#include "hal_hdmirx_phy.h"
#include "hi_drv_hdmirx.h"
#include <securec.h>

static hdmirx_phy_ctx g_hdmirx_phy_ctx[HI_DRV_HDMIRX_PORT_MAX];

static hi_void phy_clear_init(hi_drv_hdmirx_port port)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;

    phy_ctx = PHY_GET_CTX(port);
    phy_ctx->state = PHY_STATE_OFF;
    phy_ctx->run = HI_FALSE;
    phy_ctx->wait_cnt = 0;
    phy_ctx->repeat_cnt = 0;
    phy_ctx->dfe_mode = HDMIRX_DFE_MODE_MAX;
    phy_ctx->type = HDMIRX_INPUT_MAX;
}

static hi_u32 phy_get_sleep_cnt(void)
{
    return SLEEP_TIME_CNT;
}

const hi_char *g_phy_state_string[] = {
    "off",
    "handle eq",
    "auto eq",
    "dfe",
    "done",
    "BUTT"
};

static hi_void phy_set_state(hi_drv_hdmirx_port port, phy_state state)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;

    phy_ctx = PHY_GET_CTX(port);
    hi_info_hdmirx("[HDMI]== port %d phy from %s to %s ,%d",
        port, g_phy_state_string[phy_ctx->state], g_phy_state_string[state], state);
    phy_ctx->state = state;
    phy_ctx->wait_cnt = 0;
    phy_ctx->repeat_cnt = 0;
}

static hi_void phy_state_set_done(hi_drv_hdmirx_port port)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;

    phy_ctx = PHY_GET_CTX(port);
    hi_info_hdmirx("[HDMI]== port %d phy state done!", port);
    phy_set_state(port, PHY_STATE_DONE);
    phy_ctx->run = HI_FALSE;
    hi_info_hdmirx("[HDMI]== port %d phy state done! end", port);
}

static hi_void phy_state_off_proc(hi_drv_hdmirx_port port)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;
    hi_u32 delay_time;
    hdmirx_input_type type;
    hdmirx_damix_status clk_stable, pll_stable;
    hi_bool auto_eq_en = HI_FALSE;

    phy_ctx = PHY_GET_CTX(port);
    type = phy_ctx->type;
    clk_stable = hal_damix_get_status(port, type, HDMIRX_DAMIX_CLK);
    pll_stable = hal_damix_get_status(port, type, HDMIRX_DAMIX_PLL);
    delay_time = phy_get_sleep_cnt();
    if ((clk_stable == HDMIRX_DAMIX_STATUS_DONE) && (pll_stable == HDMIRX_DAMIX_STATUS_DONE)) {
        auto_eq_en = hal_damix_need_autoeq_mode(port, type);
        phy_ctx->wait_cnt = 0;
        if (auto_eq_en == HI_FALSE) {
            phy_set_state(port, PHY_STATE_HANDLE_EQ);
            hal_damix_fix_eq_init(port, type);
            hal_damix_set_fix_eq_en(port, type, HI_TRUE);
        } else {
            phy_set_state(port, PHY_STATE_AUTO_EQ);
            hal_damix_auto_eq_init(port, type);
            hal_damix_set_auto_eq_en(port, type, HI_TRUE);
        }
    } else {
        phy_ctx->wait_cnt += delay_time;
        if (phy_ctx->wait_cnt > PHY_STATE_OFF_TIME_CNT) {
            hi_info_hdmirx("[HDMI]== port %d phy off timeout", port);
            phy_state_set_done(port);
        }
    }
}

static hi_void phy_state_handle_eq_proc(hi_drv_hdmirx_port port)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;
    hi_u32 delay_time ;
    hi_bool result = HI_FALSE;
    hdmirx_input_type type;

    delay_time = phy_get_sleep_cnt();
    phy_ctx = PHY_GET_CTX(port);
    type = phy_ctx->type;
    if (phy_ctx->wait_cnt < PHY_STATE_HANDLE_EQ_TIME_CNT) {
        phy_ctx->wait_cnt += delay_time;
        result = hal_damix_get_fix_eq_result(port, type);
        if (result == HI_TRUE) {
            phy_state_set_done(port);
        }
        return;
    }
    phy_ctx->repeat_cnt++;
    if (phy_ctx->repeat_cnt < PHY_STATE_HANDLE_EQ_REPEAT_CNT) {
        hi_dbg_hdmirx("time %d set handle eq fail,repeate agin!total:%d\n",
            phy_ctx->repeat_cnt, PHY_STATE_HANDLE_EQ_REPEAT_CNT);
        hal_damix_set_fix_eq_en(port, type, HI_TRUE);
        phy_ctx->wait_cnt = 0;
        return;
    }
    hi_info_hdmirx("[HDMI]== port %d phy handle eq fail,set fixed eq!", port);
    hal_damix_set_force_eq(port, type);
    phy_state_set_done(port);
}

static hi_bool phy_check_start_dfe_en(hi_drv_hdmirx_port port, hdmirx_input_type type, hdmirx_dfe_mode mode)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;

    phy_ctx = PHY_GET_CTX(port);
    if (hal_damix_need_dfe(port, type) == HI_TRUE) {
        hal_damix_dfe_init(port, type);
        hal_damix_set_dfe_mode_en(port, mode, HI_TRUE);
        phy_ctx->dfe_mode = mode;
        return HI_TRUE;
    }
    phy_ctx->dfe_mode = HDMIRX_DFE_MODE_MAX;
    return HI_FALSE;
}

static hi_void phy_get_time_cnt_info(hdmirx_input_type type, hi_u32 *wait_time, hi_u32 *repeat_cnt)
{
    if (type <= HDMIRX_INPUT_20) {
        *wait_time = PHY_STATE_20_AUTO_EQ_TIME_CNT;
        *repeat_cnt = PHY_STATE_20_AUTO_EQ_REPEAT_CNT;
    } else {
        *wait_time = PHY_STATE_21_AUTO_EQ_TIME_CNT;
        *repeat_cnt = PHY_STATE_21_AUTO_EQ_REPEAT_CNT;
    }
}

static hi_void phy_state_auto_eq_proc(hi_drv_hdmirx_port port)
{
    hi_u32 wait_time = 0;
    hi_u32 repeat_cnt = 0;
    hdmirx_phy_ctx *phy_ctx = HI_NULL;
    hi_u32 delay_time;
    hi_bool result = HI_FALSE;
    hdmirx_input_type type;

    phy_ctx = PHY_GET_CTX(port);
    type = phy_ctx->type;
    phy_get_time_cnt_info(type, &wait_time, &repeat_cnt);

    delay_time = phy_get_sleep_cnt();
    phy_ctx = PHY_GET_CTX(port);
    if (phy_ctx->wait_cnt < wait_time) {
        phy_ctx->wait_cnt += delay_time;
        result = hal_damix_get_auto_eq_result(port, type);
        if (result == HI_TRUE) {
            if (hal_damix_need_dfe(port, type) == HI_TRUE) {
                phy_set_state(port, PHY_STATE_DFE);
                phy_check_start_dfe_en(port, type, HDMIRX_DFE_MODE_HARDWARE);
            } else {
                phy_state_set_done(port);
            }
        }
        return;
    }
    phy_ctx->repeat_cnt++;
    if (phy_ctx->repeat_cnt < repeat_cnt) {
        hi_dbg_hdmirx("time %d set auto eq fail,repeate agin!total:%d\n",
            phy_ctx->repeat_cnt, repeat_cnt);
        hal_damix_set_auto_eq_en(port, type, HI_TRUE);
        phy_ctx->wait_cnt = 0;
        return;
    }
    hi_info_hdmirx("[HDMI]== port %d phy auto eq fail,set fixed eq!", port);
    hal_damix_set_force_eq(port, type);
    if (hal_damix_need_dfe(port, type) == HI_TRUE) {
        phy_set_state(port, PHY_STATE_DFE);
        phy_check_start_dfe_en(port, type, HDMIRX_DFE_MODE_SOFTWARE);
    } else {
        phy_state_set_done(port);
    }
}

static hi_void phy_state_dfe_proc(hi_drv_hdmirx_port port)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;
    hi_u32 delay_time;
    hi_bool result = HI_FALSE;

    delay_time = phy_get_sleep_cnt();
    phy_ctx = PHY_GET_CTX(port);
    if (phy_ctx->wait_cnt < PHY_STATE_21_AUTO_DFE_TIME_CNT) {
        phy_ctx->wait_cnt += delay_time;
        result = hal_damix_get_dfe_result(port);
        if (result == HI_TRUE) {
            hi_info_hdmirx("[HDMI]== port %d dfe done", port);
            phy_state_set_done(port);
        }
        return;
    }
    hi_info_hdmirx("[HDMI]== port %d dfe fail", port);
    phy_state_set_done(port);
}

hi_void drv_hdmirx_phy_init(hi_drv_hdmirx_port port)
{
    phy_clear_init(port);
}

hi_void drv_hdmirx_phy_set_en(hi_drv_hdmirx_port port, hdmirx_input_type type, hi_bool en)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;

    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return;
    }
    hi_info_hdmirx("[HDMI] == port %d set %d %d", port, type, en);
    phy_ctx = PHY_GET_CTX(port);
    phy_clear_init(port);
    phy_ctx->run = en;
    phy_ctx->type = type;
    hal_damix_pre_set(port, type);
}

hi_void drv_hdmirx_phy_proc(hi_drv_hdmirx_port port)
{
    hdmirx_phy_ctx *phy_ctx = HI_NULL;

    phy_ctx = PHY_GET_CTX(port);
    if ((phy_ctx->run == HI_FALSE) || (port >= HI_DRV_HDMIRX_PORT_MAX)) {
        return;
    }
    switch (phy_ctx->state) {
        case PHY_STATE_OFF:
            phy_state_off_proc(port);
            break;
        case PHY_STATE_HANDLE_EQ:
            phy_state_handle_eq_proc(port);
            break;
        case PHY_STATE_AUTO_EQ:
            phy_state_auto_eq_proc(port);
            break;
        case PHY_STATE_DFE:
            phy_state_dfe_proc(port);
            break;
        default:
            break;
    }
}