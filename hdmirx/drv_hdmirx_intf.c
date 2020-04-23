/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of ioctl functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
*/
#include "drv_hdmirx_ioctl.h"
#include "drv_hdmirx_common.h"
#include "hal_hdmirx_ctrl.h"
#include "drv_hdmirx_ctrl.h"
#include "drv_hdmirx_proc.h"
#include "hal_hdmirx_comm.h"
#include "hi_drv_dev.h"
#include "hi_module.h"
#include <securec.h>

#define HDMIRX_CTRL_NUMBER 2

osal_task *g_process_task_p = NULL;
osal_task *g_pro_cec_task_p = NULL;
static drv_hdmirx_data_route g_ctrl_sec = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;
static HDMIRX_CTRL_FUNC g_hdmirx_fun_ctrl[HDMIRX_CTRL_NUMBER];
static hi_bool g_init = HI_FALSE;
static hdmirx_intf_context g_hdmirx_intf_ctx;
#define HDMIRXV2_INTF_GET_CTX()  (&g_hdmirx_intf_ctx)

static hi_s32 hdmirx_drv_intf_connect(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_dis_connect(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_get_sig_status(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_get_audio_status(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_get_timing_info(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_get_audio_info(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_get_off_line_det_status(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_update_hdcp(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_update_edid(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_set_hpd_value(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_init_edid(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_set_edid_mode(unsigned int cmd, void *arg, void *private_data);
#if SUPPORT_CEC
static hi_s32 hdmirx_drv_intf_cec_enable(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_cec_set_command(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_cec_get_command(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_cec_get_cur_cmd_state(unsigned int cmd, void *arg, void *private_data);
static hi_s32 hdmirx_drv_intf_cec_standby_enable(unsigned int cmd, void *arg, void *private_data);
#endif

static osal_ioctl_cmd g_hdmirx_io_ctrl_func[] = { /* init to max 26 */
    {HIIOC_HDMIRX_CONNECT,                 hdmirx_drv_intf_connect},
    {HIIOC_HDMIRX_DISCONNECT,              hdmirx_drv_intf_dis_connect},
    {HIIOC_HDMIRX_G_STATUS,                hdmirx_drv_intf_get_sig_status},
    {HIIOC_HDMIRX_G_AUDIO_STATUS,          hdmirx_drv_intf_get_audio_status},
    {HIIOC_HDMIRX_G_TIMING_INFO,           hdmirx_drv_intf_get_timing_info},
    {HIIOC_HDMIRX_G_AUDIO_INFO,            hdmirx_drv_intf_get_audio_info},
    {HIIOC_HDMIRX_G_OFF_LINE_DET_STA,      hdmirx_drv_intf_get_off_line_det_status},
    {HIIOC_HDMIRX_S_LOAD_HDCP,             hdmirx_drv_intf_update_hdcp},
    {HIIOC_HDMIRX_S_UPDATE_EDID,           hdmirx_drv_intf_update_edid},
    {HIIOC_HDMIRX_S_INIT_EDID,             hdmirx_drv_intf_init_edid},
    {HIIOC_HDMIRX_S_EDID_MODE,             hdmirx_drv_intf_set_edid_mode},
    {HIIOC_HDMIRX_S_HPD_VALUE,             hdmirx_drv_intf_set_hpd_value},
#if SUPPORT_CEC
    {HIIOC_HDMIRX_S_CEC_ENABLE,            hdmirx_drv_intf_cec_enable},
    {HIIOC_HDMIRX_S_CEC_CMD,               hdmirx_drv_intf_cec_set_command},
    {HIIOC_HDMIRX_G_CEC_CMD,               hdmirx_drv_intf_cec_get_command},
    {HIIOC_HDMIRX_G_CEC_CMDSTATE,          hdmirx_drv_intf_cec_get_cur_cmd_state},
    {HIIOC_HDMIRX_S_CEC_STANDBY_ENABLE,    hdmirx_drv_intf_cec_standby_enable},
#endif
};

osal_semaphore g_hdmirx_intf_ctx_lock;
osal_semaphore g_cec_intf_ctx_lock;

inline hi_void hdmirx_drv_intf_ctx_lock(hi_void)
{
    osal_sem_down(&g_hdmirx_intf_ctx_lock);
}

inline hi_void hdmirx_drv_intf_ctx_unlock(hi_void)
{
    osal_sem_up(&g_hdmirx_intf_ctx_lock);
}

inline hi_void cec_drv_intf_ctx_lock(hi_void)
{
    osal_sem_down(&g_cec_intf_ctx_lock);
}

inline hi_void cec_drv_intf_ctx_un_lock(hi_void)
{
    osal_sem_up(&g_cec_intf_ctx_lock);
}

static hi_s32 hdmirx_drv_open(void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 hdmirx_drv_close(void *private_data)
{
    return HI_SUCCESS;
}

hi_void hdmirx_register_fun(hi_u32 ctrl, HDMIRX_CALL_FUNC *fun)
{
    if (ctrl < HDMIRX_CTRL_NUMBER) {
        g_hdmirx_fun_ctrl[ctrl].used = HI_TRUE;
        g_hdmirx_fun_ctrl[ctrl].fun = fun;
    }
}

/* port convert for 810 */
static hi_void hdmirx_intf_port_convert(hi_drv_hdmirx_port inputprot,
    hi_drv_hdmirx_port *outputport, drv_hdmirx_data_route *out_ctrl)
{
    if (inputprot >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("input port error!!\n");
        return;
    }
    if ((outputport == HI_NULL) || (out_ctrl == HI_NULL)) {
        hi_err_hdmirx("input error!!\n");
        return;
    }

    *out_ctrl = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;
    *outputport = inputprot;

    return;
}

hi_s32 hdmirx_drv_intf_connect(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_hdmirx_port outputport = HI_DRV_HDMIRX_PORT_MAX;
    hi_drv_hdmirx_port port_idx;
    hdmirx_intf_context *intf_ctx = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    intf_ctx = HDMIRXV2_INTF_GET_CTX();
    if (intf_ctx->edid_state == HI_FALSE) {
        hi_err_hdmirx("[HDMI]please load HDCP key before connect port!\n");
    }
    port_idx = *((hi_drv_hdmirx_port *)arg);
    hdmirx_intf_port_convert(port_idx, &outputport, &g_ctrl_sec);
    hi_info_hdmirx("[HDMI]get connect port%d ,\n", port_idx);
    hi_info_hdmirx("ctrl:%s\n", g_ctrl_sec ? "1.4" : "2.0\n");
    if (g_ctrl_sec < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[g_ctrl_sec].fun->hdmirx_drv_ctrl_connect(outputport);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_dis_connect(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_hdmirx_port outputport = HI_DRV_HDMIRX_PORT_MAX;
    hi_drv_hdmirx_port port_idx;

    hdmirx_drv_intf_ctx_lock();
    port_idx = *((hi_drv_hdmirx_port *)arg);
    hdmirx_intf_port_convert(port_idx, &outputport, &g_ctrl_sec);
    if (g_ctrl_sec < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[g_ctrl_sec].fun->hdmirx_drv_ctrl_disconnect(outputport);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_get_sig_status(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_sig_info *sig_stat = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    sig_stat = (hi_drv_sig_info *)arg;
    if (g_ctrl_sec < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[g_ctrl_sec].fun->hdmirx_drv_ctrl_get_sig_status(sig_stat);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_get_audio_status(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_sig_info *sig_stat = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    sig_stat = (hi_drv_sig_info *)arg;
    if (g_ctrl_sec < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[g_ctrl_sec].fun->hdmirx_drv_ctrl_get_audio_status(sig_stat);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_get_timing_info(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_hdmirx_timing *timing_info = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    timing_info = (hi_drv_hdmirx_timing *)arg;
    if (g_ctrl_sec < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[g_ctrl_sec].fun->hdmirx_drv_ctrl_get_timing(timing_info);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_get_audio_info(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_hdmirx_aud_info *audio_info = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    audio_info = (hi_drv_hdmirx_aud_info *)arg;
    if (g_ctrl_sec < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[g_ctrl_sec].fun->hdmirx_drv_ctrl_get_audio_info(audio_info);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_get_off_line_det_status(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_hdmirx_offline_status *offline_sta = HI_NULL;
    drv_hdmirx_data_route out_ctrl = HI_DRV_HDMIRX_DATA_ROUTE_BUTT;

    hdmirx_drv_intf_ctx_lock();
    offline_sta = (hi_drv_hdmirx_offline_status *)arg;

    hdmirx_intf_port_convert(offline_sta->port, &(offline_sta->port), &out_ctrl);
    if (out_ctrl < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[out_ctrl].fun->hdmirx_drv_ctrl_get_offline_det_status(offline_sta);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_update_hdcp(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_drv_hdmirx_hdcp_info *encrypt_hdcp = HI_NULL;
    hdmirx_intf_context *intf_ctx = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    intf_ctx = HDMIRXV2_INTF_GET_CTX();

    if (intf_ctx->edid_state == HI_FALSE) {
        hi_err_hdmirx("please update EDID before LOAD HDCP!\n");
    }

    encrypt_hdcp = (hi_drv_hdmirx_hdcp_info *)arg;
    ret = g_hdmirx_fun_ctrl[HI_DRV_HDMIRX_DATA_ROUTE_CTRL0].fun->hdmirx_drv_ctrl_load_hdcp(encrypt_hdcp);
    intf_ctx->edid_state = HI_TRUE;
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_update_edid(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_hdmirx_edid_info *edid = HI_NULL;
    drv_hdmirx_data_route out_ctrl = HI_DRV_HDMIRX_DATA_ROUTE_BUTT;
    hdmirx_intf_context *intf_ctx = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    intf_ctx = HDMIRXV2_INTF_GET_CTX();

    edid = (hi_drv_hdmirx_edid_info *)arg;

    hdmirx_intf_port_convert(edid->port, &(edid->port), &out_ctrl);
    if (out_ctrl < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[out_ctrl].fun->hdmirx_drv_ctrl_update_edid(edid);
    }
    intf_ctx->edid_state = HI_TRUE;

    hi_warn_hdmirx("[HDMI]UPDATE EDID for port%d!\n", edid->port);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_init_edid(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_hdmirx_edid_init_info *init_edid = HI_NULL;
    drv_hdmirx_data_route out_ctrl = HI_DRV_HDMIRX_DATA_ROUTE_BUTT;
    hdmirx_intf_context *intf_ctx = HI_NULL;

    hdmirx_drv_intf_ctx_lock();
    intf_ctx = HDMIRXV2_INTF_GET_CTX();

    init_edid = (hi_drv_hdmirx_edid_init_info *)arg;

    hdmirx_intf_port_convert(init_edid->port, &(init_edid->port), &out_ctrl);
    if (out_ctrl < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[out_ctrl].fun->hdmirx_drv_ctrl_init_edid(init_edid);
    }
    intf_ctx->edid_state = HI_TRUE;

    hi_warn_hdmirx("[HDMI]INIT EDID for port%d!\n", init_edid->port);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_set_edid_mode(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_drv_hdmirx_edid_mode_info *edid_mode = HI_NULL;
    drv_hdmirx_data_route mhl_ctrl_sel = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;
    hdmirx_drv_intf_ctx_lock();
    edid_mode = (hi_drv_hdmirx_edid_mode_info *)arg;
    ret = g_hdmirx_fun_ctrl[mhl_ctrl_sel].fun->hdmirx_drv_ctrl_set_edid_mode(edid_mode);

    hi_warn_hdmirx("[HDMI]edid_mode: %d!\n", *edid_mode);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_set_hpd_value(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret = HI_FAILURE;
    hi_drv_hdmirx_hpd *hpd = HI_NULL;
    drv_hdmirx_data_route out_ctrl = HI_DRV_HDMIRX_DATA_ROUTE_BUTT;
    hdmirx_drv_intf_ctx_lock();
    hpd = (hi_drv_hdmirx_hpd *)arg;
    hdmirx_intf_port_convert(hpd->port, &(hpd->port), &out_ctrl);
    if (out_ctrl < HI_DRV_HDMIRX_DATA_ROUTE_BUTT) {
        ret = g_hdmirx_fun_ctrl[out_ctrl].fun->hdmirx_drv_ctrl_set_hpd_value(hpd);
    }
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

#if SUPPORT_CEC
hi_s32 hdmirx_drv_intf_cec_enable(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_bool *enable = HI_NULL;
    drv_hdmirx_data_route cec_ctrl_sel = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;

    hdmirx_drv_intf_ctx_lock();
    enable = (hi_bool *)arg;
    ret = g_hdmirx_fun_ctrl[cec_ctrl_sel].fun->hdmirx_drv_ctrl_cec_enable(enable);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_cec_set_command(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_drv_hdmirx_cec_cmd *cec_cmd = HI_NULL;
    drv_hdmirx_data_route cec_ctrl_sel = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;

    hdmirx_drv_intf_ctx_lock();
    cec_cmd = (hi_drv_hdmirx_cec_cmd *)arg;

    ret = g_hdmirx_fun_ctrl[cec_ctrl_sel].fun->hdmirx_drv_ctrl_cec_set_command(cec_cmd);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_cec_get_command(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_drv_hdmirx_cec_cmd *cec_cmd = HI_NULL;
    drv_hdmirx_data_route cec_ctrl_sel = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;

    hdmirx_drv_intf_ctx_lock();
    cec_cmd = (hi_drv_hdmirx_cec_cmd *)arg;
    ret = g_hdmirx_fun_ctrl[cec_ctrl_sel].fun->hdmirx_drv_ctrl_cec_get_command(cec_cmd);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_cec_get_cur_cmd_state(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_drv_hdmirx_cec_cmd_state_data *cmd_state = HI_NULL;
    drv_hdmirx_data_route cec_ctrl_sel = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;

    hdmirx_drv_intf_ctx_lock();
    cmd_state = (hi_drv_hdmirx_cec_cmd_state_data *)arg;
    ret = g_hdmirx_fun_ctrl[cec_ctrl_sel].fun->hdmirx_drv_ctrl_cec_get_cur_state(cmd_state);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}

hi_s32 hdmirx_drv_intf_cec_standby_enable(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_bool enable;
    drv_hdmirx_data_route cec_ctrl_sel = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;

    hdmirx_drv_intf_ctx_lock();
    enable = *(hi_bool *)arg;
    ret = g_hdmirx_fun_ctrl[cec_ctrl_sel].fun->hdmirx_drv_ctrl_cec_standby_enable(enable);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}
hi_s32 hdmirx_drv_intf_cec_get_cmd_cnt(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    hi_u8 *cmd_cnt = HI_NULL;
    drv_hdmirx_data_route cec_ctrl_sel = HI_DRV_HDMIRX_DATA_ROUTE_CTRL0;

    hdmirx_drv_intf_ctx_lock();
    cmd_cnt = (hi_u8 *)arg;
    ret = g_hdmirx_fun_ctrl[cec_ctrl_sel].fun->hdmirx_drv_ctrl_cec_get_msg_cnt(cmd_cnt);
    hdmirx_drv_intf_ctx_unlock();
    return ret;
}
#endif

hi_void hdmirx_intf_peri_set_hpd_value(hi_u32 *hpd_value)
{
#ifdef HI_BOARD_HDMIRX_PORT3_HPD_INVERT
    if (HI_BOARD_HDMIRX_PORT3_HPD_INVERT == HI_TRUE) {
        *hpd_value |= 0x8;
    }
#endif

#ifdef HI_BOARD_HDMIRX_PORT2_HPD_INVERT
    if (HI_BOARD_HDMIRX_PORT2_HPD_INVERT == HI_TRUE) {
        *hpd_value |= 0x4;
    }
#endif

#ifdef HI_BOARD_HDMIRX_PORT1_HPD_INVERT
    if (HI_BOARD_HDMIRX_PORT1_HPD_INVERT == HI_TRUE) {
        *hpd_value |= 0x2;
    }
#endif

#ifdef HI_BOARD_HDMIRX_PORT0_HPD_INVERT
    if (HI_BOARD_HDMIRX_PORT0_HPD_INVERT == HI_TRUE) {
        *hpd_value |= 0x1;
    }
#endif
    hi_warn_hdmirx_print_u32(*hpd_value);
}

/* set hpd/pwr polarity */
hi_void hdmirx_intf_peri_init(hi_void)
{
    hi_u32 hpd_value = 0;
    hi_u32 pwr_value = 0;

    hdmirx_intf_peri_set_hpd_value(&hpd_value);
#ifdef HI_BOARD_HDMIRX_PORT3_PWR_INVERT
    if (HI_BOARD_HDMIRX_PORT3_PWR_INVERT == HI_TRUE) {
        pwr_value |= 0x8;
    }
#endif

#ifdef HI_BOARD_HDMIRX_PORT2_PWR_INVERT
    if (HI_BOARD_HDMIRX_PORT2_PWR_INVERT == HI_TRUE) {
        pwr_value |= 0x4;
    }
#endif

#ifdef HI_BOARD_HDMIRX_PORT1_PWR_INVERT
    if (HI_BOARD_HDMIRX_PORT1_PWR_INVERT == HI_TRUE) {
        pwr_value |= 0x2;
    }
#endif

#ifdef HI_BOARD_HDMIRX_PORT0_PWR_INVERT
    if (HI_BOARD_HDMIRX_PORT0_PWR_INVERT == HI_TRUE) {
        pwr_value |= 0x1;
    }
#endif
    hi_warn_hdmirx_print_u32(pwr_value);
}

#ifndef HI_FPGA
hi_void hdmirx_intf_crg_init(hi_void)
{
    hdmirx_reg_write_map(0x00a0047c, 0x0003D37C); /* CRG init, enable clk and hw reset */
    hdmirx_reg_write_map(0x00a00480, 0x0000037C); /* CRG init, enable clk and hw reset */

    /* io mux */
    hdmirx_reg_write_map(0x00848054, 0x201);  /* HDMIRX1 5V det */
    hdmirx_reg_write_map(0x00a110bc, 0x1201); /* HDMIRX0 5V det */

    /* overwrite port0 5V det */
    hdmirx_reg_write_map(0x10fd064, 0x3); /* set 5v always high */
}
#endif

hi_s32 hdmirx_drv_suspend(void *private_data)
{
    hi_s32 ret;

    hi_warn_hdmirx_func_enter();
    ret = hdmirxv2_drv_ctrl_suspend();
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("HDMIRX hal init fail!\n");
        hi_warn_hdmirx_func_exit();
        return ret;
    }
#ifdef MODULE
    HI_PRINT("reusme hi_hdmirx.ko success.\t\t(%s)\n", VERSION_STRING);
#endif
    hi_warn_hdmirx("HDMIRX suspend -->");
    hi_warn_hdmirx_func_exit();
    return HI_SUCCESS;
}

hi_s32 hdmirx_drv_resume(void *private_data)
{
    hi_s32 ret;

    hi_warn_hdmirx_func_enter();
#ifndef HI_FPGA
    hdmirx_intf_crg_init();
#endif
    hdmirx_intf_peri_init();
    /* pull down the HPD immediately after resetting the HDMI module, avoid tx reading edid. */
    hal_ctrl_set_hpd_level(HI_DRV_HDMIRX_PORT0, HI_FALSE);

    ret = hdmirxv2_drv_ctrl_resume();
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("HDMIRX hal init fail!\n");
        return ret;
    }
    HI_PRINT("reusme hi_hdmirx.ko success.\t\t(%s)\n", VERSION_STRING);
    hi_warn_hdmirx("HDMIRX resume -->");
    hi_warn_hdmirx_func_exit();
    return HI_SUCCESS;
}
/* only for vicap */
hi_u32 hdmirx_drv_intf_get_data_route(hi_void)
{
    return g_ctrl_sec;
}

hi_s32 hdmirx_drv_intf_pm_lowpower_enter(void *private_data)
{
    hdmirx_ctrl_context_info *ctrl_ctx_info;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();

    hi_warn_hdmirx("pm lowpower enter.\n");
    hdmirx_drv_intf_ctx_lock();
    ctrl_ctx_info->run = HI_FALSE;
    hdmirx_drv_intf_ctx_unlock();
    return HI_SUCCESS;
}

hi_s32 hdmirx_drv_intf_pm_lowpower_exit(void *private_data)
{
    hdmirx_ctrl_context_info *ctrl_ctx_info;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    hdmirx_drv_intf_ctx_lock();
    ctrl_ctx_info->run = HI_TRUE;
    hdmirx_drv_intf_ctx_unlock();
    hi_warn_hdmirx("pm lowpower exit.\n");
    return HI_SUCCESS;
}

static osal_fileops g_st_hdmirx_file_ops = {
    .open = hdmirx_drv_open,
    .release = hdmirx_drv_close,
    .cmd_list = g_hdmirx_io_ctrl_func,
    .cmd_cnt = 0,
};

static osal_pmops g_st_hdmirx_drv_ops = {
    .pm_suspend = hdmirx_drv_suspend,
    .pm_resume = hdmirx_drv_resume,
    .pm_lowpower_enter = hdmirx_drv_intf_pm_lowpower_enter,
    .pm_lowpower_exit = hdmirx_drv_intf_pm_lowpower_exit,
    .pm_poweroff = NULL,
    .private_data = NULL,
};

static osal_dev g_st_hdmirx_dev = {
    .name  = "hi_hdmirx",
    .minor = HI_DEV_HDMIRX_MINOR,
    .fops = &g_st_hdmirx_file_ops,
    .pmops = &g_st_hdmirx_drv_ops,
};

static const hdmirx_export_func g_hdmirx_ext_funcs = {
    .hdmirx_get_data_route = hdmirx_drv_intf_get_data_route,
};

static hi_s32 hdmirx_intf_main_task(hi_void *data)
{
    while (1) {
        if (osal_kthread_should_stop()) {
            hdmirx_drv_intf_ctx_unlock();
            break;
        }
        hdmirx_drv_intf_ctx_lock();
        hdmirxv2_ctrl_main_task();
        hdmirx_drv_intf_ctx_unlock();
        osal_msleep(10); /* 10: sleep 10 ms every time */
    }
    return 0;
}

#if SUPPORT_CEC
static hi_s32 cec_intf_main_task(hi_void *data)
{
    while (1) {
        if (osal_kthread_should_stop()) {
            cec_drv_intf_ctx_un_lock();
            break;
        }
        cec_drv_intf_ctx_lock();

        hdmirxv2_ctrl_cec_main_task();

        cec_drv_intf_ctx_un_lock();
        osal_msleep(10); /* 10: sleep 10 ms every time */
    }
    return 0;
}
#endif

hi_s32 hdmirx_drv_regist(hi_void)
{
    hi_s32 ret;
    hi_drv_hdmirx_port port;

    ret = osal_dev_register(&g_st_hdmirx_dev);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("HDMIRX device register fail!\n");
        return ret;
    }

    ret = osal_exportfunc_register(HI_ID_HDMIRX, "HI_HDMIRX", (hi_void *)&g_hdmirx_ext_funcs);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("HDMIRX module register fail!\n");
        osal_dev_unregister(&g_st_hdmirx_dev);
        return ret;
    }
    for (port = HI_DRV_HDMIRX_PORT0; port < HI_DRV_HDMIRX_PORT2; port++) {
        ret = hdmirxv2_drv_proc_add(port);
        if (ret != HI_SUCCESS) {
            hi_err_hdmirx("hdmirxv2_drv_proc_add fail!");
            osal_exportfunc_unregister(HI_ID_HDMIRX);
            osal_dev_unregister(&g_st_hdmirx_dev);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 hdmirx_drv_module_init(hi_void)
{
    hi_s32 ret;
    hi_u32 stack_size = 12288; /* pre-set stack size is 12288 */

    osal_sem_init(&g_hdmirx_intf_ctx_lock, 1);
    osal_sem_init(&g_cec_intf_ctx_lock, 1);
    hi_warn_hdmirx_func_enter();
    g_st_hdmirx_file_ops.cmd_cnt = sizeof(g_hdmirx_io_ctrl_func) / sizeof(g_hdmirx_io_ctrl_func[0]);
    ret = snprintf_s(g_st_hdmirx_dev.name, sizeof(g_st_hdmirx_dev.name),
        sizeof(g_st_hdmirx_dev.name) - 1, "%s", HI_DEV_HDMIRX_NAME);
    if (ret < 0) {
        hi_err_hdmirx("call secure func snprintf_s failed\n");
        return HI_FAILURE;
    }

    ret = hdmirx_drv_regist();
    if (ret != HI_SUCCESS) {
        return ret;
    }
#ifndef HI_FPGA
    hdmirx_intf_crg_init();
#endif
    hdmirx_intf_peri_init();
    ret = hdmirxv2_drv_ctrl_init();
    if (ret != HI_SUCCESS) {
        osal_exportfunc_unregister(HI_ID_HDMIRX);
        osal_dev_unregister(&g_st_hdmirx_dev);
        hdmirxv2_drv_proc_del();
        hi_err_hdmirx("HDMIRXV2 hal init fail!\n");
        hi_warn_hdmirx_func_exit();

        return ret;
    }
    g_process_task_p = osal_kthread_create(hdmirx_intf_main_task, NULL, "HDMIRX main task", stack_size);
#if SUPPORT_CEC
    g_pro_cec_task_p = osal_kthread_create(cec_intf_main_task, NULL, "CEC main task", 0);
#endif
    g_init = HI_TRUE;
#ifdef MODULE
    HI_PRINT("load hi_hdmirx.ko success.\t(%s)\n", VERSION_STRING);
#endif
    hi_warn_hdmirx_func_exit();
    return HI_SUCCESS;
}

hi_void hdmirx_drv_module_exit(hi_void)
{
    hi_warn_hdmirx_func_enter();
    g_init = HI_FALSE;

    hdmirxv2_drv_ctrl_de_init();
    osal_kthread_destroy(g_process_task_p, HI_TRUE);
#if SUPPORT_CEC
    osal_kthread_destroy(g_pro_cec_task_p, HI_TRUE);
#endif
    osal_sem_destory(&g_hdmirx_intf_ctx_lock);
    osal_sem_destory(&g_cec_intf_ctx_lock);
    osal_exportfunc_unregister(HI_ID_HDMIRX);
    hdmirxv2_drv_proc_del();
    osal_dev_unregister(&g_st_hdmirx_dev);

    HI_PRINT("remove hi_hdmirx.ko success.\n");
    hi_warn_hdmirx_func_exit();
}

#ifndef __LITEOS__
MODULE_LICENSE("GPL");
#endif

#ifdef MODULE
module_init(hdmirx_drv_module_init);
module_exit(hdmirx_drv_module_exit);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
