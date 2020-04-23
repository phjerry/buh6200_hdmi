/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of controller functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "drv_hdmirx_ctrl.h"
#include "hal_hdmirx_ctrl.h"
#include "hal_hdmirx_phy.h"
#include "hal_hdmirx_hdcp.h"
#include "hal_hdmirx_video.h"
#include "drv_hdmirx_video.h"
#include "drv_hdmirx_audio.h"
#include "drv_hdmirx_packet.h"
#include "drv_hdmirx_hdcp.h"
#include "drv_hdmirx_edid.h"
#include "drv_hdmirx_phy.h"
#include "hal_hdmirx_depack_reg.h"
#include "hal_hdmirx_pwd_reg.h"
#include "hal_hdmirx_video_reg.h"
#include "hal_hdmirx_product.h"

#if SUPPORT_CEC
#include "drv_hdmirx_cec.h"
#endif
#include <securec.h>

#ifdef CUSTOM_LANGGUO_OPS_4K_SLOW
hi_bool g_bforce20 = HI_FALSE;
hi_bool g_bonly = HI_TRUE;
#define STATE_OFF_TIME 100
#endif

#define HDMIRX_HDCP_ERR_CNT_THR  6 /* 80 */
#define HDMIRX_HDCP_M_RST_WAITMS 400
hdmirx_ctrl_edid g_edid[HI_DRV_HDMIRX_PORT_MAX];
hdmirx_ctrl_context_info g_hdmirx_ctrl_ctx_info;
static hi_u8 g_edid_fresh_thread_param = 0;
hi_drv_sig_status g_over_vide_sig_sta[HI_DRV_HDMIRX_PORT_MAX] = {
    HI_DRV_SIG_MAX,
    HI_DRV_SIG_MAX,
    HI_DRV_SIG_MAX,
    HI_DRV_SIG_MAX
};

const hi_char *g_state_string[] = {
    "no signal",
    "signal stable",
    "signal unstable",
    "BUTT"
};

HDMIRX_CALL_FUNC g_hdmirx_call_func1 = {
    hdmirxv2_drv_ctrl_connect,
    hdmirxv2_drv_ctrl_disconnect,
    hdmirxv2_drv_ctrl_get_sig_status,
    hdmirxv2_drv_ctrl_get_audio_status,
    hdmirxv2_drv_ctrl_get_timing,
    hdmirxv2_drv_ctrl_get_audio_info,
    hdmirxv2_drv_ctrl_get_offline_det_status,
    hdmirxv2_drv_ctrl_load_hdcp,
    hdmirxv2_drv_ctrl_update_edid,
    hdmirxv2_drv_ctrl_set_hpd_value,
    hdmirxv2_drv_ctrl_init_edid,
    hdmirxv2_drv_ctrl_set_edid_mode,
#if SUPPORT_CEC
    hdmirxv2_drv_ctrl_cec_enable,
    hdmirxv2_drv_ctrl_cec_set_command,
    hdmirxv2_drv_ctrl_cec_get_command,
    hdmirxv2_drv_ctrl_cec_get_cur_state,
    hdmirxv2_drv_ctrl_cec_standby_enable,
    hdmirxv2_drv_ctrl_cec_get_msg_cnt,
#endif
};

#if SUPPORT_CEC
static hi_void hdmirxv2_drv_ctrl_show_cec_data(hi_drv_hdmirx_cec_cmd *cec_cmd)
{
    hi_u8 cnt;
    hi_dbg_hdmirx_print_u32(cec_cmd->src_addr);
    hi_dbg_hdmirx_print_u32(cec_cmd->dst_addr);
    hi_dbg_hdmirx_print_u32(cec_cmd->opcode);
    hi_dbg_hdmirx_print_u32(cec_cmd->operand.arg_count);
    if (cec_cmd->operand.arg_count < 16) { /* 16: print only front 16 data */
        for (cnt = 0; cnt < cec_cmd->operand.arg_count; cnt++) {
            hi_dbg_hdmirx_print_u32(cec_cmd->operand.args[cnt]);
        }
    }
}
#endif
hi_void hdmirxv2_ctrl_set_input_type(hi_drv_hdmirx_port port, hdmirx_input_type input_type)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("input port ERROR!!");
        return;
    }

    ctrl_ctx->input_type = input_type;
}

hdmirx_input_type hdmirxv2_ctrl_get_input_type(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("cur_port ERROR!!");
        return HDMIRX_INPUT_MAX;
    }

    return ctrl_ctx->input_type;
}

hi_void hdmirxv2_ctrl_phy_set(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hdmirx_input_type type;
    hi_bool scrbl_en = HI_FALSE;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    if (hal_ctrl_get_tmds_ratio(port) == HI_FALSE) {
        scrbl_en = hal_ctrl_get_scrbl_en(port);
        hi_info_hdmirx("scramble %s!\n", scrbl_en ? "enable" : "disable");
    }
    scrbl_en = hal_ctrl_get_scrbl_en(port);
    if ((hal_ctrl_get_tmds_ratio(port) == HI_TRUE) || (scrbl_en == HI_TRUE)) {
        type = HDMIRX_INPUT_20;
    } else {
        type = HDMIRX_INPUT_14;
    }
    drv_hdmirx_phy_set_en(port, type, HI_TRUE);
}

hi_bool hdmirxv2_ctrl_power_set(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hdmirx_input_type type;
    hi_bool chg = HI_FALSE;
    hi_bool scrbl_en = HI_FALSE;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    if (hal_ctrl_get_tmds_ratio(port) == HI_FALSE) {
        if (hal_ctrl_get_scrbl_en(port) == HI_TRUE) {
        } else {
        }
    }
    scrbl_en = hal_ctrl_get_scrbl_en(port);

    type = ((hal_ctrl_get_tmds_ratio(port) == HI_TRUE) || (scrbl_en == HI_TRUE)) ? HDMIRX_INPUT_20 : HDMIRX_INPUT_14;
    if (type != hdmirxv2_ctrl_get_input_type(port)) {
        hal_phy_set_power(port, HDMIRX_INPUT_OFF);
        osal_msleep(1);
        hal_phy_set_power(port, type);
        osal_msleep(1);
        drv_hdmirx_phy_set_en(port, type, HI_TRUE);
        hdmirxv2_ctrl_set_input_type(port, type);
        chg = HI_TRUE;
    }
    return chg;
}


static hi_bool hdmirx_ctrl_hdcp_err_check(hi_drv_hdmirx_port port, hi_u32 errcnt)
{
    hdmirx_hdcp_err_cnt hdcp_err;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    hdcp_err = hal_hdcp_get_bch_err_cnt(port);
    hi_warn_hdmirx("___hdcp_err.t4_err_cnt=%d,hdcp_err.bch_err_cnt=%d,hdcp_err.pkt_cnt=%d\n", hdcp_err.t4_err_cnt,
                   hdcp_err.bch_err_cnt, hdcp_err.pkt_cnt);
    if (hdcp_err.t4_err_cnt == 0) {
        if ((hdcp_err.bch_err_cnt == hdcp_err.pkt_cnt) && (hdcp_err.pkt_cnt > 0)) {
            hi_warn_hdmirx("HDCP BCH error:0x%d\n!", hdcp_err.bch_err_cnt);
            ctrl_ctx->hdcp_err_cnt++;
            if (ctrl_ctx->hdcp_err_cnt > errcnt) {
                hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
                ctrl_ctx->hdcp_err_cnt = 0;
                ctrl_ctx->port_status.hdcp_stable_cnt = 0;
                ctrl_ctx->port_status.wait_cnt = 0;
                {
                    hi_dbg_hdmirx("------> reset HDMI HDCP...... \n");
                    hal_ctrl_set_hpd_level(port, HI_FALSE);
                    osal_msleep(HDMIRX_HDCP_M_RST_WAITMS);
                    hal_ctrl_set_hpd_level(port, HI_TRUE);
                }
                hdmirxv2_ctrl_mode_change(port);
                hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_VIDEO_OFF);

                return HI_TRUE;
            }
        } else {
            hdcp_err.bch_err_cnt = 0;
        }
    }
    return HI_FALSE;
}

hi_bool hdmirxv2_ctrl_is_need2be_done(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_OFF) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_bool hdmirxv2_ctrl_is_video_on_state(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return HI_FALSE;
    }
    if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_ON) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_void hdmirxv2_ctrl_change_state(hi_drv_hdmirx_port port, hdmirx_ctrl_state new_state)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (ctrl_ctx->port_status.state != new_state) {
        if (new_state == HDMIRX_STATE_VIDEO_OFF) {
            hdmirxv2_ctrl_mode_change(port);
        } else if (new_state == HDMIRX_STATE_VIDEO_ON) {
            if (hdmirxv2_packet_vsif_is_got3d(port) == HI_FALSE) {
            }
        }
    }

    ctrl_ctx->port_status.state = new_state;
}

hi_void hdmirxv2_ctrl_mode_change(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    ctrl_ctx->mode_change = HI_TRUE;
    ctrl_ctx->port_status.wait_cnt = 0;
    ctrl_ctx->unstable_cnt = 0;
    ctrl_ctx->port_status.hdcp_stable_cnt = 0;

    if (hal_ctrl_get_scdt(port) == HI_FALSE) {
        hdmirxv2_video_set_video_idx(port, TIMING_NOSIGNAL);
    }
    hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
    if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_ON) {
        if (hdmirxv2_packet_vsif_is_got3d(port) == HI_TRUE) {
            hdmirxv2_packet_vsif_reset_data(port);
        }
        if (hdmirxv2_packet_hdr10_is_got(port) == HI_TRUE) {
            hdmirxv2_packet_unrec_reset_data(port);
        }
        if (hdmirxv2_packet_spd_is_got(port) == HI_TRUE) {
            hdmirxv2_packet_spd_reset_data(port);
        }
        hdmirxv2_packet_initial(port);
        hdmirxv2_video_timing_data_init(port);
    }
    hdmirxv2_audio_initial(port);
    ctrl_ctx->no_sync_cnt = 0;
}

static hi_bool hdmirx_ctrl_ckdt_proc(hi_drv_hdmirx_port port)
{
    hi_bool is_ckdt;

    is_ckdt = hal_ctrl_get_ckdt(port);
    if (is_ckdt == HI_TRUE) {
        /* reset dc fifo */
        hal_ctrl_set_pwd_reset(port, HDMIRX_PWD_VID_FIFO_SRST);
        /* mute the video */
        hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
        /* clear scdt int */
        hal_ctrl_clear_interrupt(port, HDMIRX_INT_SCDT);

        hdmirxv2_video_rst_timing_data(port);

        return HI_TRUE;
    }
    return HI_FALSE;
}

static hi_void hdmirx_ctrl_interrupt_initial(hi_void)
{
}

static hi_void hdmirx_ctrl_resume(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    hal_ctrl_set_mute_en(port, HDMIRX_MUTE_ALL, HI_TRUE);

    hdmirxv2_packet_initial(port);
    hdmirx_ctrl_interrupt_initial();
    hdmirxv2_audio_initial(port);
    hdmirxv2_video_timing_data_init(port);
    ctrl_ctx->no_sync_cnt = 0;
}

hi_void hdmirx_ctrl_signal_support(hi_drv_hdmirx_port port)
{
    hdmirx_color_space color_space;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    color_space = hdmirxv2_packet_avi_get_color_space(port);
    ctrl_ctx->dphy_inter_align_cnt = 0;
    hdmirxv2_video_set_video_path(port);
    hdmirxv2_video_set_deep_color_mode(port);
    if (color_space != HDMIRX_COLOR_SPACE_MAX) {
        hal_video_set_mute_value(port, color_space);
    }
    hal_ctrl_set_sys_mute(port, HI_FALSE);
    hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_FALSE);
    if (hdmirxv2_video_get_hdmi_mode(port) == HI_TRUE) {
        if (hdmirxv2_audio_is_request(port) == HI_FALSE) {
            hdmirxv2_audio_restart(port);
        }
    } else {
        hdmirxv2_audio_stop(port);
    }
}

hi_s32 hdmirx_ctrl_proc_signal_stable(hi_drv_hdmirx_port port)
{
    hi_bool support;
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (ctrl_ctx->port_status.wait_cnt == HDMIRX_WAIT_TIMEOUT) {
        hi_dbg_hdmirx("waitcnt timeout!\n");
        hdmirxv2_ctrl_mode_change(port);
        ctrl_ctx->port_status.wait_cnt = 0;
        return HI_FAILURE;
    }
    hdmirxv2_video_mode_det(port);
    support = hdmirxv2_video_is_support(port);
    if (support == HI_TRUE) {
        hdmirx_ctrl_signal_support(port);
    } else {
        hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
        hdmirxv2_audio_stop(port);
    }

    if ((hdmirxv2_packet_avi_is_data_valid(port) == HI_FALSE) &&
        (hdmirxv2_video_get_hdmi_mode(port) == HI_TRUE)) {
        (ctrl_ctx->no_avi_cnt)++;
        if (ctrl_ctx->no_avi_cnt > HDMIRX_CHECK_AVI_VALID_THR) {
            hi_warn_hdmirx("[HDMI]can not get AVI, wait 500ms...\n");
            ctrl_ctx->no_avi_cnt = 0;
            hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_VIDEO_ON);
            hdmirxv2_video_show_video_timing(port);
        }
    } else {
        ctrl_ctx->no_avi_cnt = 0;
        hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_VIDEO_ON);
        hdmirxv2_video_show_video_timing(port);
    }
    return HI_SUCCESS;
}

hi_void hdmirx_ctrl_check_hdcp_stable(hi_drv_hdmirx_port port)
{
    hi_bool is_hdcp1x_stable = HI_FALSE;
    hi_bool is_hdcp2x_stable = HI_FALSE;
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    /* use hdcp to check stable --> */
    is_hdcp1x_stable = hal_hdcp_get_done(port, HI_DRV_HDMIRX_HDCPTYPE_14);
    is_hdcp2x_stable = hal_hdcp_get_done(port, HI_DRV_HDMIRX_HDCPTYPE_22);
    if ((is_hdcp1x_stable == HI_TRUE) || (is_hdcp2x_stable == HI_TRUE)) {
        ctrl_ctx->port_status.hdcp_stable_cnt++;
        if (ctrl_ctx->port_status.hdcp_stable_cnt >= HDMIRX_HDCP_STABLE_THR) {
            ctrl_ctx->port_status.hdcp_stable_cnt = HDMIRX_HDCP_STABLE_THR;
        }
    } else {
        ctrl_ctx->port_status.hdcp_stable_cnt = 0;
    }
}

hi_s32 hdmirx_ctrl_proc_hdcp_stable(hi_drv_hdmirx_port port)
{
    hi_bool b_stable;
    hi_s32 ret = HI_FAILURE;

    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    /* hdcp stable case --> */
    if ((ctrl_ctx->port_status.hdcp_stable_cnt == HDMIRX_HDCP_STABLE_THR) ||
        (ctrl_ctx->port_status.wait_cnt > HDMIRX_CHECK_STABLE_THR)) {
        b_stable = hdmirxv2_video_check_stable(port);
        if (b_stable == HI_TRUE) {
            ret = hdmirx_ctrl_proc_signal_stable(port);
            if (ret != HI_SUCCESS) { /* waitcnt timeout */
                return HI_FAILURE;
            }
        }
    }
    return HI_SUCCESS;
}

hi_void hdmirx_ctrl_wait_stable_proc_hdcp(hi_drv_hdmirx_port port)
{
    hi_s32 ret;

    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (ctrl_ctx->hdcp_check_en == HI_TRUE) {
        if (hdmirx_ctrl_hdcp_err_check(port, HDMIRX_HDCP_ERR_CNT_THR) == HI_TRUE) {
            hi_warn_hdmirx("[HDMI]HDCP ERR in wait!\n");
            return;
        }
    }

#if 0
    if (ctrl_ctx->hdcp_check_en == HI_TRUE && (hdmirx_ctrl_hdcp_err_check(port, HDMIRX_HDCP_ERR_CNT_THR) == HI_TRUE)) {
        hi_warn_hdmirx("[HDMI]HDCP ERR in wait!\n");
        return;
    }
#endif

    ctrl_ctx->no_sync_cnt = 0;

    hdmirxv2_video_hdmi_dvi_trans(port);
    if (hdmirxv2_video_get_hdmi_mode(port) == HI_FALSE) {
        hdmirxv2_audio_set_exceptions_en(port, HI_FALSE);
    }
    hdmirxv2_video_set_deep_color_mode(port);
    hdmirx_ctrl_check_hdcp_stable(port);
    ret = hdmirx_ctrl_proc_hdcp_stable(port);
}

hi_void hdmirx_ctrl_wait_stable_proc_scdt(hi_drv_hdmirx_port port)
{
    hi_bool is_scdt;
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    /* check no signal --> */
    is_scdt = hal_ctrl_get_scdt(port);
    if (is_scdt == HI_FALSE) {
        ctrl_ctx->no_sync_cnt++;
        ctrl_ctx->port_status.wait_cnt = 0;
        if (ctrl_ctx->no_sync_cnt == HDMIRX_NO_SIGNAL_THR) {
            ctrl_ctx->no_sync_cnt = 0;
            ctrl_ctx->dphy_inter_align_cnt = 0;
            hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_VIDEO_OFF);
        }
    } else {
        hdmirx_ctrl_wait_stable_proc_hdcp(port);
    }
}

/*
* the process of wait video signal to stable state .
* input the HDMI port number and return null .
*/
static hi_void hdmirx_ctrl_wait_stable_proc(hi_drv_hdmirx_port port)
{
    hi_bool pwr5v;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return;
    }
    ctrl_ctx->port_status.wait_cnt++;
    pwr5v = hal_ctrl_get_5v_status(port);
    if (pwr5v == HI_FALSE) {
        ctrl_ctx->no_sync_cnt = 0;
        ctrl_ctx->pwr5v = 0;
        ctrl_ctx->dphy_inter_align_cnt = 0;
        hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_VIDEO_OFF);
        return;
    }
    if (hal_dphy_get_inter_align(port) == HI_FALSE) {
        /* 5: not align for more than 5 times, reset fifo */
        if (ctrl_ctx->dphy_inter_align_cnt < HDMIRX_CHECK_DPHY_ALIGN) {
            (ctrl_ctx->dphy_inter_align_cnt)++;
            hi_warn_hdmirx("dphy can not get inter_align,reset dphy fifo!\n");
            hal_dphy_set_in_fifo_rst(port);
        }
    }
    if (ctrl_ctx->port_status.wait_cnt >= HDMIRX_WAIT_TIMEOUT) {
        ctrl_ctx->port_status.wait_cnt = HDMIRX_WAIT_TIMEOUT;
        ctrl_ctx->mode_change = HI_FALSE;
    }
    hdmirx_ctrl_wait_stable_proc_scdt(port);
}

static hi_void hdmirx_ctrl_no_signal_proc(hi_drv_hdmirx_port port)
{
    static hi_bool is_ckdt = HI_FALSE;
    hi_bool is_scdt = HI_FALSE;
    hi_bool pwr5v = HI_FALSE;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    pwr5v = hal_ctrl_get_5v_status(port);
    if (pwr5v == HI_FALSE) {
        ctrl_ctx->pwr5v_cnt = 0;
        ctrl_ctx->pwr5v = 0;
        return;
    }

    is_scdt = hal_ctrl_get_scdt(port);

    if (is_ckdt == HI_FALSE) {
        is_ckdt = hdmirx_ctrl_ckdt_proc(port);
    }
    if ((is_ckdt == HI_TRUE) && (is_scdt == HI_TRUE)) {
        ctrl_ctx->no_sync_cnt++;
        if (ctrl_ctx->no_sync_cnt > NO_SIGNAL_STABLE_CNT) {
            is_ckdt = HI_FALSE;
            ctrl_ctx->no_sync_cnt = 0;
            hdmirx_ctrl_resume(port);
            hdmirxv2_ctrl_mode_change(port);
            hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_WAIT);
        }
    } else {
        ctrl_ctx->no_sync_cnt = 0;
    }
}

static hi_void hdmirx_ctrl_video_unstable_cnt(hi_drv_hdmirx_port port, hi_u32 mode_chg_type)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if ((mode_chg_type & MODE_CHG_HVRES) || (mode_chg_type & MODE_CHG_PIXCLK)) {
        ctrl_ctx->unstable_cnt++;
    }
}

hi_void hdmirx_ctrl_video_on_proc_scdt(hi_drv_hdmirx_port port)
{
    hi_u32 mode_chg_type;
    hi_bool is_scdt;

    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    is_scdt = hal_ctrl_get_scdt(port);
    if (is_scdt == HI_FALSE) {
        ctrl_ctx->no_sync_cnt++;
        if (ctrl_ctx->no_sync_cnt >= NO_SIGNAL_THR_IN_VIDEO_ON) {
            ctrl_ctx->no_sync_cnt = 0;
            hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
            hdmirxv2_ctrl_mode_change(port);
            hi_warn_hdmirx("[HDMI]2.0 ctrl state on :cannot get scdt\n");
            hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_WAIT);
            return;
        }
    } else {
        ctrl_ctx->no_sync_cnt = 0;
        /* check mode change */
        mode_chg_type = hdmirxv2_video_get_mode_chg_type(port);
        hdmirx_ctrl_video_unstable_cnt(port, mode_chg_type);
        if (hdmirxv2_video_is_need_mode_change(port, mode_chg_type) == HI_TRUE) {
            hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
            hi_warn_hdmirx("[HDMI] 2.0 ctrl state on :get mode_change\n");
            hdmirxv2_ctrl_mode_change(port);
            hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_WAIT);
        }
    }
}

static hi_void hdmirx_ctrl_video_on_proc(hi_drv_hdmirx_port port)
{
    hi_bool pwr5v;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    pwr5v = hal_ctrl_get_5v_status(port);
    if (pwr5v == HI_FALSE) {
        ctrl_ctx->no_sync_cnt = 0;
        hdmirxv2_ctrl_mode_change(port);
        hi_warn_hdmirx("[HDMI]2.0 ctrl state on :cannot get pwr5v\n");
        hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_WAIT);
        return;
    }

    if (ctrl_ctx->hdcp_check_en == HI_TRUE) {
        if (hdmirx_ctrl_hdcp_err_check(port, HDMIRX_HDCP_ERR_CNT_THR) == HI_TRUE) {
            hi_err_hdmirx("[HDMI]HDCP check ERRO!\n");
            return;
        }
    }

    hdmirx_ctrl_video_on_proc_scdt(port);
}

static hi_void hdmirx_ctrl_state_det(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (ctrl_ctx->port_status.last_state != ctrl_ctx->port_status.state) {
        ctrl_ctx->port_status.last_state = ctrl_ctx->port_status.state;
        hi_warn_hdmirx("[HDMI]==HDMI 2.0 port%d state:%s\n", port, g_state_string[ctrl_ctx->port_status.state]);
    }
    switch (ctrl_ctx->port_status.state) {
        case HDMIRX_STATE_WAIT:
            hdmirx_ctrl_wait_stable_proc(port);
            break;

        case HDMIRX_STATE_VIDEO_OFF:
            hdmirx_ctrl_no_signal_proc(port);
            break;

        case HDMIRX_STATE_VIDEO_ON:
            hdmirx_ctrl_video_on_proc(port);
            break;

        default:
            break;
    }
}

hi_void hdmirx_ctrl_get_inter_no_vsi(hi_drv_hdmirx_port port, hi_u32 *interrupts)
{
    if (interrupts[HDMIRX_INTR3] & INTR_CEA_NO_VSI) {
        hdmirxv2_packet_vsif_reset_data(port);
        hdmirxv2_video_set_stream_data(port);
    }
}

hi_void hdmirx_ctrl_proc_intr_by_type(hi_drv_hdmirx_port port, hi_u32 *interrupts)
{
    hi_bool avi_offset = HDMIRX_INTR1;
    hi_bool spd_offset = HDMIRX_INTR1;
    hi_bool vsif_offset = HDMIRX_INTR1;
    hi_bool unrec_offset = HDMIRX_INTR1;

    if (hdmirxv2_packet_get_int_new_update(port, HDMIRX_PACKET_AVI) == HI_TRUE) {
        avi_offset = HDMIRX_INTR2;
    }
    if (hdmirxv2_packet_get_int_new_update(port, HDMIRX_PACKET_SPD) == HI_TRUE) {
        spd_offset = HDMIRX_INTR2;
    }
    if (hdmirxv2_packet_get_int_new_update(port, HDMIRX_PACKET_VSI) == HI_TRUE) {
        vsif_offset = HDMIRX_INTR2;
    }
    if (hdmirxv2_packet_get_int_new_update(port, HDMIRX_PACKET_UNREC) == HI_TRUE) {
        unrec_offset = HDMIRX_INTR2;
    }

    if (interrupts[unrec_offset] & INTR_CEA_UPDATE_HDR) {
        hi_info_hdmirx("get unrec intr!!!\n");
        hdmirxv2_packet_interrupt_handler(port, INTR_CEA_UPDATE_HDR);
    } else if (hdmirxv2_packet_hdr10_is_got(port) == HI_TRUE) {
        if (interrupts[HDMIRX_INTR3] & INTR_CEA_NO_UNREC) {
            hdmirxv2_packet_unrec_reset_data(port);
            hdmirxv2_video_set_stream_data(port);
        }
    }
    if (hdmirxv2_packet_vsif_is_got_dolby(port) == HI_TRUE) {
        hdmirx_ctrl_get_inter_no_vsi(port, interrupts);
    }
    if (hdmirxv2_packet_vsif_is_got_hdr10_plus(port) == HI_TRUE) {
        hdmirx_ctrl_get_inter_no_vsi(port, interrupts);
    }
    if (interrupts[HDMIRX_INTR1] & INTR_SERVING_PACKETS_MASK) {
        hdmirxv2_packet_interrupt_handler(port, (interrupts[HDMIRX_INTR1] & INTR_SERVING_PACKETS_MASK));
    }
    if (interrupts[avi_offset] & INTR_CEA_UPDATE_AVI) {
        hdmirxv2_packet_interrupt_handler(port, INTR_CEA_UPDATE_AVI);
    }
    if (interrupts[vsif_offset] & INTR_CEA_UPDATE_VSI) {
        hdmirxv2_packet_vsif_int_handler(port);
    }
    if (interrupts[spd_offset] & INTR_CEA_UPDATE_SPD) {
        hdmirxv2_packet_interrupt_handler(port, INTR_CEA_UPDATE_SPD);
    }
    if (interrupts[HDMIRX_INTR2] & INTR_CEA_UPDATE_ACP) {
        hdmirxv2_packet_acp_int_handler(port);
    }
}

hi_void hdmirx_ctrl_av_mute(hi_drv_hdmirx_port port)
{
#ifdef CUSTOM_HDMIRX_AV_MUTE_SUPPORT
    static hi_bool mute = HI_FALSE;
#endif
#ifdef CUSTOM_HDMIRX_AV_MUTE_SUPPORT
    if (CUSTOM_HDMIRX_AV_MUTE_SUPPORT) {
        if (mute != hal_ctrl_get_av_mute(port)) {
            mute = hal_ctrl_get_av_mute(port);
            if (mute) {
                hal_ctrl_set_mute_en(port, HDMIRX_MUTE_ALL, HI_TRUE);
            } else {
                hal_ctrl_set_mute_en(port, HDMIRX_MUTE_ALL, HI_FALSE);
            }
        }
    }
#endif
}

hi_void hdmirx_ctrl_proc_depack_intr(hi_drv_hdmirx_port port, hi_u32 *interrupts, hi_u32 len)
{
    hi_bool no_avi;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    no_avi = (interrupts[HDMIRX_INTR3] & INTR_CEA_NO_AVI) ? HI_TRUE : HI_FALSE;
    if (no_avi) {
        if (interrupts[HDMIRX_INTR1] & INTR_CEA_UPDATE_AVI) {
            /* controversial info: both AVI and NO AVI bits are set, try to resolve the conflict */
            if (hal_ctrl_get_packet_type(port, REG_AVIRX_WORD0)) { /* check if AVI header is valid */
                no_avi = HI_FALSE; /* decision: AVI bit is correct */
            } else {
                interrupts[HDMIRX_INTR1] &= ~INTR_CEA_UPDATE_AVI; /* decision: NO AVI bit is correct */
            }
        }
    }
    if (no_avi) {
        hdmirxv2_packet_avi_set_no_avi_int_en(port, HI_FALSE);
        hdmirxv2_packet_avi_no_avi_handler(port);
        ctrl_ctx->no_avi = HI_TRUE;
    } else {
        hal_ctrl_clear_interrupt(port, INTR_CEA_NO_AVI);
        ctrl_ctx->no_avi = HI_FALSE;
    }

    hdmirx_ctrl_proc_intr_by_type(port, interrupts);
    hdmirx_ctrl_av_mute(port);
}

hi_void hdmirx_ctrl_proc_video_intr(hi_drv_hdmirx_port port, hi_u32 *interrupts, hi_u32 len)
{
    if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_HSYNC_POLARITY_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VSYNC_POLARITY_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_INTERLACE_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_HACTIVE_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_HSYNC_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_HTOTAL_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_HFRONT_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_HBACK_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VACTIVE_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VSYNC_EVEN_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VSYNC_ODD_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VTOTAL_EVEN_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VTOTAL_ODD_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VFRONT_EVEN_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VFRONT_ODD_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VBACK_EVEN_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_VBACK_ODD_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_FRAME_RATE_CHANGED) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_FIFO_FULL) {
    } else if (interrupts[HDMIRX_INTR1] & CFG_ORIGINAL_STATUS_FIFO_EMPTY) {
    }
}

static hi_void hdmirx_ctrl_isr_handler(hi_drv_hdmirx_port port)
{
    hi_u32 intr = 0;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_u32 intr_status[HDMIRX_INTR_MAX] = {0};
    hi_u32 len = 0;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return;
    }

    hal_ctrl_get_pwd_interrupt(port, &intr, HDMIRX_PWD_INTR_MAX);
    if (intr > 0) {
        if (intr & HDMIRX_PWD_INT_DEPACK) {
            len = hal_ctrl_get_pwd_intr_status(port, HDMIRX_PWD_INTR_DEPACK, intr_status, HDMIRX_INTR_MAX);
            hal_ctrl_clear_pwd_interrupt(port, HDMIRX_PWD_INTR_DEPACK, intr_status, len);
            if (hal_video_get_hdmi_mode(port) == HI_TRUE) {
                hdmirx_ctrl_proc_depack_intr(port, intr_status, len);
            }
        } else if (intr & HDMIRX_PWD_INT_VIDEO) {
            len = hal_ctrl_get_pwd_intr_status(port, HDMIRX_PWD_INTR_VIDEO, intr_status, HDMIRX_INTR_MAX);
            hal_ctrl_clear_pwd_interrupt(port, HDMIRX_PWD_INTR_VIDEO, intr_status, len);
            hdmirx_ctrl_proc_video_intr(port, intr_status, len);
        } else if (intr & HDMIRX_PWD_INT_AUDIO) {
        } else if (intr & HDMIRX_PWD_INT_FRL) {
        } else if (intr & HDMIRX_PWD_INT_HDCP2X) {
        } else if (intr & HDMIRX_PWD_INT_HDCP1X) {
        } else if (intr & HDMIRX_PWD_INT_ARC) {
        } else if (intr & HDMIRX_PWD_INT_EMP) {
        } else if (intr & HDMIRX_PWD_INT_TMDS) {
        } else if (intr & HDMIRX_PWD_INT_DPHY) {
        } else if (intr & HDMIRX_PWD_INT_DAMIX) {
        } else if (intr & HDMIRX_PWD_INT_DET0) {
            hi_warn_hdmirx("clk0 stat changed");
        } else if (intr & HDMIRX_PWD_INT_DET1) {
            hi_warn_hdmirx("clk1 stat changed");
        } else if (intr & HDMIRX_PWD_INT_DET2) {
            hi_warn_hdmirx("clk2 stat changed");
        } else if (intr & HDMIRX_PWD_INT_DET3) {
            hi_warn_hdmirx("clk3 stat changed");
        } else if (intr & HDMIRX_PWD_INT_DET4) {
            hi_warn_hdmirx("pclk ratio stat changed");
        } else if (intr & HDMIRX_PWD_INT_DET5) {
            hi_warn_hdmirx("hdcp mode change");
        } else if (intr & HDMIRX_PWD_INT_DET6) {
            hi_warn_hdmirx("frl rate change");
        }
    }
}

hi_void hdmirx_ctrl_set_hpd_level(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    hi_warn_hdmirx("[HDMI]HDMI port%d signal %s!\n", port,
                   hal_ctrl_get_5v_status(port) ? "connect" : "disconnect");
    {
        if (hal_ctrl_get_5v_status(port) == HI_FALSE) {
            hal_phy_set_power(port, HDMIRX_INPUT_OFF);
            hdmirxv2_ctrl_set_input_type(port, HDMIRX_INPUT_OFF);
            hal_damix_set_term_mode(port, HDMIRX_TERM_SEL_OFF);
            ctrl_ctx->pwrstatus = HI_FALSE;
            ctrl_ctx->hpd_low_ctrl = HI_TRUE;
            osal_get_timeofday(&ctrl_ctx->low_start_time);
        } else {
            ctrl_ctx->pwrstatus = HI_TRUE;
            hal_damix_set_term_mode(port, HDMIRX_TERM_SEL_HDMI);
            ctrl_ctx->hpd_high_ctrl = HI_TRUE;
            osal_get_timeofday(&ctrl_ctx->high_start_time);
        }
    }
}
static hi_void hdmirx_ctrl_port5v_det_ctrl_hpd(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    if (ctrl_ctx->hpd_low_ctrl == HI_TRUE) {
        osal_timeval cur_low_time;
        osal_get_timeofday(&cur_low_time);
        if (hdmirx_time_diff_ms(cur_low_time, ctrl_ctx->low_start_time) >= 100) { /* 100: time diff ms */
            hal_ctrl_set_hpd_level(port, HI_FALSE);
            ctrl_ctx->hpd_low_ctrl = HI_FALSE;
            ctrl_ctx->pwrstatus = HI_FALSE;
        }
    }
    if (ctrl_ctx->hpd_high_ctrl == HI_TRUE) {
        osal_timeval cur_high_time;
        osal_get_timeofday(&cur_high_time);
        if (hdmirx_time_diff_ms(cur_high_time, ctrl_ctx->high_start_time) >= 100) { /* 100: time diff ms */
            hal_ctrl_set_hpd_level(port, HI_TRUE);
            ctrl_ctx->hpd_high_ctrl = HI_FALSE;
            ctrl_ctx->pwrstatus = HI_TRUE;
        }
    }
    if (ctrl_ctx->hdcp_edid_ready == HI_FALSE) {
        if (ctrl_ctx->pwrstatus == HI_TRUE) {
            ctrl_ctx->pwrstatus = HI_FALSE;
            hal_phy_set_power(port, HDMIRX_INPUT_OFF);
            hdmirxv2_ctrl_set_input_type(port, HDMIRX_INPUT_OFF);
            hal_damix_set_term_mode(port, HDMIRX_TERM_SEL_OFF);
            ctrl_ctx->hpd_low_ctrl = HI_TRUE;
            osal_get_timeofday(&ctrl_ctx->low_start_time);
        }
        return;
    }

    if (hal_ctrl_get_5v_status(port) != ctrl_ctx->pwrstatus) {
        hdmirx_ctrl_set_hpd_level(port);
    }
}

static hi_void hdmirxv2_ctrl_port_5v_det(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_port5v_det_ctrl_hpd(port);
}

static hi_void hdmirxv2_ctrl_phy_handler(hi_drv_hdmirx_port port)
{
    static hi_bool is_ckdt[4] = {HI_FALSE}; /* 4: array index */
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return;
    }

    if ((ctrl_ctx->pwrstatus == HI_TRUE) && (hal_ctrl_get_5v_status(port) == HI_TRUE)) {
        hdmirxv2_ctrl_power_set(port);
    }

    if (hal_ctrl_get_ckdt(port) != is_ckdt[port]) {
        hi_dbg_hdmirx("___is_ckdt=%d,ckdt=%d\n", is_ckdt[port], hal_ctrl_get_ckdt(port));
        if (hal_ctrl_get_ckdt(port) == HI_TRUE) {
            osal_msleep(50); /* 50: delay time */
            hdmirxv2_ctrl_phy_set(port);
        }
        is_ckdt[port] = hal_ctrl_get_ckdt(port);
    }
}

hi_void hdmirxv2_ctrl_update_edid_type(hi_drv_hdmirx_port port, hi_bool *update, hi_u32* dvi_cnt)
{
    static hi_u32 consecutive_cnt = 0;
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    if ((hal_ctrl_get_5v_status(port) == HI_FALSE) && (*update == HI_FALSE)) {
        *update = HI_TRUE;
        *dvi_cnt = 0;
        if (ctrl_ctx->cur_edid_type == HDMIRX_EDID_14) {
            ctrl_ctx->cur_edid_type = HDMIRX_EDID_20;
            hi_warn_hdmirx("HDMI mode ,change EDID to 2.0\n");
            hdmirxv2_drv_ctrl_edid_fresh(port);
        }
    }
    if ((*update == HI_TRUE) && (HI_TRUE == hal_ctrl_get_ckdt(port))) {
        if (*dvi_cnt < 10) { /* 10: count value */
            (*dvi_cnt)++;
            return;
        }
        if (hal_video_get_hdmi_mode(port) != HI_FALSE) {
            consecutive_cnt = 0;
            return;
        }
        consecutive_cnt++;
        /* 有些tx开始发dvi, */
        /* 后面发hdmi,但实际上就是要发hdmi */
        /* 为了兼容稍微延缓一下 */
        if (consecutive_cnt > 3) { /* 3是经验值 */
            *update = HI_FALSE;
            if (ctrl_ctx->cur_edid_type == HDMIRX_EDID_20) {
                ctrl_ctx->cur_edid_type = HDMIRX_EDID_14;
                hi_warn_hdmirx("HDMI mode ,change EDID to 1.4\n");
                hdmirxv2_drv_ctrl_edid_fresh(port);
            }
        }
    }
}

hi_void hdmirxv2_drv_ctrl_remove_task(hi_void)
{
    hi_u32 index;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    for (index = 0; index < HDMRIX_EDID_TASK_NUM; index++) {
        if (ctrl_ctx_info->edid_fresh_task[index].is_free == HI_FALSE) {
            if (ctrl_ctx_info->edid_fresh_task[index].can_free == HI_TRUE) {
                osal_kfree(HI_ID_HDMIRX, (const hi_void *)ctrl_ctx_info->edid_fresh_task[index].task);
                hi_dbg_hdmirx("remove %d task from deque.\n", index);
                ctrl_ctx_info->edid_fresh_task[index].is_free = HI_TRUE;
                ctrl_ctx_info->edid_fresh_task[index].can_free = HI_FALSE;
            }
        }
    }
}

hi_void hdmirxv2_custom_edid_method(hi_drv_hdmirx_port port, hi_bool b_reset)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    static hi_drv_hdmirx_port enold_port = HI_DRV_HDMIRX_PORT_MAX;
    static hi_u32 dvi_cnt = 0;
    static hi_bool update = HI_TRUE;
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    if (b_reset == HI_TRUE) {
        dvi_cnt = 0;
        update = HI_TRUE;
        return;
    }
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return;
    }

    if (enold_port != port) {
        dvi_cnt = 0;
        update = HI_TRUE;
        enold_port = port;
    }
    hdmirxv2_drv_ctrl_remove_task();
    /* only work in EDID mode is auto */
    if (ctrl_ctx->edid_mode == HI_DRV_HDMIRX_EDID_MODE_AUTO) {
        if (ctrl_ctx->hdcp_edid_ready == HI_TRUE) {
            hdmirxv2_ctrl_update_edid_type(port, &update, &dvi_cnt);
        }
    } else {
        dvi_cnt = 0;
        update = HI_TRUE;
    }
}

hi_s32 hdmirxv2_ctrl_main_task(hi_void)
{
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;
    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    if (ctrl_ctx_info->run != HI_TRUE) {
        return 0;
    }

    for (ctrl_ctx_info->loop_port = ctrl_ctx_info->start_port;
        ctrl_ctx_info->loop_port <= ctrl_ctx_info->end_port; ctrl_ctx_info->loop_port++) {
        hdmirx_ctrl_state_det(ctrl_ctx_info->loop_port);
        hdmirx_ctrl_isr_handler(ctrl_ctx_info->loop_port);
        hdmirxv2_audio_main_loop(ctrl_ctx_info->loop_port);
        hdmirxv2_custom_edid_method(ctrl_ctx_info->loop_port, HI_FALSE);
        hdmirxv2_ctrl_port_5v_det(ctrl_ctx_info->loop_port);
        hdmirxv2_ctrl_phy_handler(ctrl_ctx_info->loop_port);
        drv_hdmirx_phy_proc(ctrl_ctx_info->loop_port);
    }

    return 0;
}

#if SUPPORT_CEC
hi_s32 hdmirxv2_ctrl_cec_main_task(hi_void)
{
    hdmirxv2_cec_main_loop(HI_DRV_HDMIRX_PORT1);

    return 0;
}
#endif

static hi_void hdmirx_ctrl_ctx_init(hdmirx_ctrl_context *ctrl_ctx)
{
    ctrl_ctx->port_status.state = HDMIRX_STATE_VIDEO_OFF;
    ctrl_ctx->port_status.last_state = HDMIRX_STATE_BUTT;
    ctrl_ctx->mode_change = HI_FALSE;
    ctrl_ctx->unstable_cnt = 0;
    ctrl_ctx->input_type = HDMIRX_INPUT_OFF;
    ctrl_ctx->hdcp_edid_ready = HI_FALSE;
    ctrl_ctx->hdcp_check_en = HI_FALSE;
    ctrl_ctx->dphy_inter_align_cnt = 0;
    ctrl_ctx->no_avi_cnt = 0;
    ctrl_ctx->hpd_low_ctrl = HI_FALSE;
    ctrl_ctx->hpd_high_ctrl = HI_FALSE;
    ctrl_ctx->low_start_time.tv_sec = 0;
    ctrl_ctx->low_start_time.tv_usec = 0;
    ctrl_ctx->high_start_time.tv_sec = 0;
    ctrl_ctx->high_start_time.tv_usec = 0;
}


hi_drv_sig_status hdmirxv2_ctrl_get_port_status(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_sig_status en_sig_sta;
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (port > HI_DRV_HDMIRX_PORT3) {
        return HI_DRV_SIG_NO_SIGNAL;
    }
    if (ctrl_ctx->port_status.state == HDMIRX_STATE_WAIT) {
        en_sig_sta = HI_DRV_SIG_UNSTABLE;
    } else if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_ON) {
        en_sig_sta = HI_DRV_SIG_SUPPORT;
    } else {
        en_sig_sta = HI_DRV_SIG_NO_SIGNAL;
    }
    return en_sig_sta;
}

#ifdef CUSTOM_SPD_VENDOR_NAME
static hi_bool hdmirx_drv_ctrl_compare(hi_u32 *dest, hi_char *src)
{
    int i;
    if ((dest == HI_NULL) || (src == HI_NULL)) {
        return HI_FALSE;
    }
    for (i = 0; (dest[i] != 0) && (src[i] != '\0'); i++) {
        if (src[i] != (hi_u8)dest[i]) {
            return HI_FALSE;
        }
    }
    if ((dest[i] == 0) && (src[i] == '\0')) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}
#endif

static hi_bool hdmirx_drv_ctrl_is_spd_name_in_list(hi_void)
{
#ifdef CUSTOM_SPD_VENDOR_NAME
    {
        hi_u32 data[HDMIRX_SPD_VENDOR_NAME_LENGTH] = { 0 };
        hi_bool flag;
        hdmirxv2_packet_spd_get_vendor_name(data);
        if (strlen(CUSTOM_SPD_VENDOR_NAME) >= HDMIRX_SPD_VENDOR_NAME_LENGTH) {
            return HI_FALSE;
        }
        flag = hdmirx_drv_ctrl_compare(data, CUSTOM_SPD_VENDOR_NAME);
        if (flag == HI_FALSE) {
            return HI_FALSE;
        }
        errno_t err_ret = memset_s(data, sizeof(data), 0, HDMIRX_SPD_VENDOR_NAME_LENGTH * sizeof(hi_u32));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return err_ret;
        }
        hdmirxv2_packet_spd_get_product_des(data);
        flag = hdmirx_drv_ctrl_compare(data, CUSTOM_SPD_PRODUCT_DES);
        return flag;
    }
#else
    return HI_TRUE;
#endif
}

hi_void hdmirxv2_drv_ctrl_get_video_on_status(hdmirx_ctrl_context *ctrl_ctx, hi_drv_sig_info *sig_info)
{
    if (ctrl_ctx->mode_change == HI_TRUE) {
        ctrl_ctx->mode_change = HI_FALSE;
        sig_info->status = HI_DRV_SIG_UNSTABLE;
    } else if (hdmirxv2_video_is_timing_active(sig_info->port) == HI_TRUE) {
        sig_info->status = HI_DRV_SIG_SUPPORT;
        if (hdmirxv2_video_is_dolby_vision(sig_info->port) == HI_TRUE) {
            sig_info->status = HI_DRV_SIG_NOT_SUPPORT;
        }
    } else {
        sig_info->status = HI_DRV_SIG_NOT_SUPPORT;
    }
}

hi_s32 hdmirxv2_drv_ctrl_get_sig_status(hi_drv_sig_info *sig_info)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    errno_t err_ret;
    hi_s32 ret = HI_SUCCESS;
    HDMIRX_CHECK_NULL_PTR(sig_info, ret);
    if (sig_info->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(sig_info->port);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    err_ret = memset_s(&(sig_info->status), sizeof(hi_drv_sig_status), 0, sizeof(hi_drv_sig_status));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    if (g_over_vide_sig_sta[sig_info->port] != HI_DRV_SIG_MAX) {
        sig_info->status = g_over_vide_sig_sta[sig_info->port];
        return HI_SUCCESS;
    }
    if (sig_info->port >= HI_DRV_HDMIRX_PORT_MAX) {
        sig_info->status = HI_DRV_SIG_NO_SIGNAL;
        return HI_FAILURE;
    }
    if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_OFF) {
        sig_info->status = HI_DRV_SIG_NO_SIGNAL;
    } else if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_ON) {
        hdmirxv2_drv_ctrl_get_video_on_status(ctrl_ctx, sig_info);
    } else if (ctrl_ctx->port_status.state == HDMIRX_STATE_WAIT) {
        if (ctrl_ctx->port_status.wait_cnt == HDMIRX_WAIT_TIMEOUT) {
            sig_info->status = HI_DRV_SIG_NOT_SUPPORT;
        } else {
            sig_info->status = HI_DRV_SIG_UNSTABLE;
        }
    }
    if (sig_info->status == HI_DRV_SIG_SUPPORT) {
        if (hdmirx_drv_ctrl_is_spd_name_in_list() == HI_FALSE) {
            sig_info->status = HI_DRV_SIG_NOT_SUPPORT;
        }
    }
    return HI_SUCCESS;
}

hdmirx_ctrl_state hdmirxv2_drv_ctrl_get_state(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    return ctrl_ctx->port_status.state;
}

hi_s32 hdmirxv2_drv_ctrl_get_audio_status(hi_drv_sig_info *sig_info)
{
    hi_s32 ret = HI_SUCCESS;
    hdmix_audio_state audio_state;
    errno_t err_ret;
    HDMIRX_CHECK_NULL_PTR(sig_info, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (sig_info->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }
    err_ret = memset_s(&(sig_info->status), sizeof(hi_drv_sig_status), 0, sizeof(hi_drv_sig_status));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }

    audio_state = hdmirxv2_audio_getstatus(sig_info->port);
    if (sig_info->port < HI_DRV_HDMIRX_PORT_MAX) {
        if (hdmirxv2_video_get_hdmi_mode(sig_info->port) == HI_FALSE) {
            sig_info->status = HI_DRV_SIG_NO_SIGNAL;
        } else {
            switch (audio_state) {
                case AUDIO_STATE_OFF:
                    sig_info->status = HI_DRV_SIG_NO_SIGNAL;
                    break;
                case AUDIO_STATE_ON:
                    sig_info->status = HI_DRV_SIG_SUPPORT;
                    break;
                case AUDIO_STATE_REQ:
                case AUDIO_STATE_READY:
                    sig_info->status = HI_DRV_SIG_UNSTABLE;
                    break;
                default:
                    sig_info->status = HI_DRV_SIG_NOT_SUPPORT;
                    break;
            }
        }
    } else {
        sig_info->status = HI_DRV_SIG_NO_SIGNAL;
    }
    if (sig_info->status == HI_DRV_SIG_SUPPORT) {
        if (hdmirx_drv_ctrl_is_spd_name_in_list() == HI_FALSE) {
            sig_info->status = HI_DRV_SIG_NOT_SUPPORT;
        }
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_get_timing(hi_drv_hdmirx_timing *timing)
{
    errno_t err_ret;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_s32 ret = HI_SUCCESS;

    HDMIRX_CHECK_NULL_PTR(timing, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (timing->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(timing->port);
    err_ret = memset_s(&(timing->timing_info), sizeof(hi_drv_hdmirx_timing_info), 0, sizeof(hi_drv_hdmirx_timing_info));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    if (hdmirxv2_video_is_support(timing->port) == HI_FALSE) {
        return HI_FAILURE;
    }
    hdmirxv2_video_get_timing_info(timing->port, &(timing->timing_info));
    if (ctrl_ctx->port_status.state != HDMIRX_STATE_VIDEO_ON) {
        hi_err_hdmirx("state changed, not return timing!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_get_audio_info(hi_drv_hdmirx_aud_info *audio_info)
{
    hi_s32 ret = HI_SUCCESS;
    errno_t err_ret;
    HDMIRX_CHECK_NULL_PTR(audio_info, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (audio_info->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }
    err_ret = memset_s(&(audio_info->attr), sizeof(hi_drv_ai_hdmirx_attr), 0, sizeof(hi_drv_ai_hdmirx_attr));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    if (hdmirxv2_video_is_support(audio_info->port) == HI_FALSE) {
        return HI_FAILURE;
    }
    hdmirxv2_audio_get_info(audio_info->port, &(audio_info->attr));

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_get_offline_det_status(hi_drv_hdmirx_offline_status *offline_stat)
{
    hi_bool offline_det_status;
    hi_s32 ret = HI_SUCCESS;
    HDMIRX_CHECK_NULL_PTR(offline_stat, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (offline_stat->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("get offline det_status input port erro!!!\n");
        offline_stat->info.connected = HI_FALSE;
        return HI_FAILURE;
    }
    offline_det_status = hal_ctrl_get_5v_status(offline_stat->port);
    offline_stat->info.connected = offline_det_status;
    if (offline_det_status == HI_TRUE) {
        offline_stat->info.format = HI_DRV_HDMIRX_FORMAT_HDMI;
    } else {
        offline_stat->info.format = HI_DRV_HDMIRX_FORMAT_MAX;
    }
    return HI_SUCCESS;
}

hi_void hdmirxv2_drv_ctrl_load_hdcp_done(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    osal_msleep(10); /* 10: sleep 10 ms */
    ctrl_ctx->hdcp_edid_ready = HI_TRUE;
    if (port == HI_DRV_HDMIRX_PORT_MAX) {
        hdmirxv2_audio_set_audio_ready(port, HI_FALSE);
    }

    hal_ctrl_set_ddc_slave_en(port, HI_TRUE);
    hal_ctrl_set_ddc_edid_en(port, HI_TRUE);
    hal_ctrl_set_ddc_scdc_en(port, HI_TRUE);
    hal_ctrl_set_ddc_hdcp_en(port, HI_TRUE);
    hal_ctrl_set_ddc_scdc_en(port, HI_TRUE);

    hal_ctrl_set_pwd_reset(port, HDMIRX_PWD_HDCP1X_SRST);
}

hi_s32 hdmirxv2_drv_ctrl_load_hdcp(hi_drv_hdmirx_hdcp_info *hdcp_key)
{
    hi_s32 ret = HI_SUCCESS;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    static hi_bool hdcp14 = HI_FALSE;
    static hi_bool hdcp22 = HI_FALSE;
    HDMIRX_CHECK_NULL_PTR(hdcp_key, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (hdcp_key->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(hdcp_key->port);
    if (hdcp_key->hdcp_type >= HI_DRV_HDMIRX_HDCPTYPE_MAX) {
        hi_err_hdmirx("load hdcp input port erro!!!\n");
        return HI_FAILURE;
    }
    if ((hdcp_key->hdcp.hdcp_length != HDMIRX_HDCP_1P4_KEY_LENGTH) && /* 320: hdcp length check threshold */
        (hdcp_key->hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_14)) {
        hi_err_hdmirx("load hdcp 1.4 length err for ctrl 2.0 !!\n");
        return HI_FAILURE;
    }
    if ((hdcp_key->hdcp.hdcp_length != HDMIRX_HDCP_2P2_KEY_LENGTH) && /* 864: hdcp length check threshold */
        (hdcp_key->hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_22)) {
        hi_err_hdmirx("load hdcp 2.2 length err for ctrl 2.0 !!\n");
        return HI_FAILURE;
    }

    if ((hdcp14 == HI_FALSE) && (hdcp22 == HI_FALSE)) {
    }
    if (hdcp_key->hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_14) {
        hdcp14 = HI_TRUE;
    } else {
        hdcp22 = HI_TRUE;
    }

    if ((hdcp14 == HI_TRUE) && (hdcp22 == HI_TRUE)) {
        hdmirxv2_drv_ctrl_load_hdcp_done(hdcp_key->port);
    }
    return HI_SUCCESS;
}

hi_void hdmirxv2_drv_ctrl_update_port_edid(hi_drv_hdmirx_port port,
    hi_drv_hdmirx_edid *edid)
{
    hal_ctrl_load_edid(port, edid);

    osal_kfree(HI_ID_HDMIRX, edid);
}

hi_void hdmirxv2_drv_ctrl_edid_updateby_port(hi_drv_hdmirx_port port)
{
    hi_drv_hdmirx_edid_info *edid;
    hdmirx_edid_type tmp_edid_type;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    errno_t err_ret;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    edid = (hi_drv_hdmirx_edid_info *)osal_kmalloc(HI_ID_HDMIRX, sizeof(hi_drv_hdmirx_edid_info), OSAL_GFP_KERNEL);
    if (edid == HI_NULL) {
        hi_err_hdmirx("kmalloc error!\n");
        return;
    }
    err_ret = memset_s(edid, sizeof(hi_drv_hdmirx_edid_info), 0, sizeof(hi_drv_hdmirx_edid_info));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        osal_kfree(HI_ID_HDMIRX, edid);
        return;
    }
    edid->port = port;
    edid->edid.cec_addr = ctrl_ctx->edid_info.cec_addr;
    edid->edid.edid_length = EDID_LENGTH;
    if (ctrl_ctx->edid_mode == HI_DRV_HDMIRX_EDID_MODE_AUTO) {
        /* if EDID mode is auto ,edid data come from edid_info.else come from g_edid */
        if (ctrl_ctx->cur_edid_type == HDMIRX_EDID_MAX) {
            ctrl_ctx->cur_edid_type = HDMIRX_EDID_20;
        }
        tmp_edid_type = ctrl_ctx->cur_edid_type;
        err_ret = memcpy_s(edid->edid.edid_data, sizeof(edid->edid.edid_data),
            ctrl_ctx->edid_info.edid_data[tmp_edid_type], HDMIRX_EDID_LENGTH * sizeof(hi_u8));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            osal_kfree(HI_ID_HDMIRX, edid);
            return;
        }
    } else {
        hdmirx_drv_edid_get_edid_type(g_edid[port].edid_data, &tmp_edid_type);
        err_ret = memcpy_s(edid->edid.edid_data, sizeof(edid->edid.edid_data),
            g_edid[port].edid_data, HDMIRX_EDID_LENGTH * sizeof(hi_u8));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            osal_kfree(HI_ID_HDMIRX, edid);
            return;
        }
    }
    ctrl_ctx->cur_edid_type = tmp_edid_type;

    hi_warn_hdmirx("EDID mode %s:update to %s\n",
                   (ctrl_ctx->edid_mode == HI_DRV_HDMIRX_EDID_MODE_MENU ? "menu" : "auto"),
                   (tmp_edid_type == HDMIRX_EDID_20) ? "2.0" : "1.4");
    hdmirxv2_drv_ctrl_update_port_edid(port, &(edid->edid));
}

hi_s32 hdmirxv2_drv_ctrl_edid_fresh_thread(hi_void *pData)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u8 data;
    hi_u8 task_index;
    hi_drv_hdmirx_port port;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;

    HDMIRX_CHECK_NULL_PTR(pData, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    data = *((hi_u8 *)pData);
    port = data & 0xf;
    task_index = (data >> 4) & 0xf; /* get high 4 bits */
    hi_warn_hdmirx("EDID refresh port: %d!!!!\n", port);

    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("edid fresh port error\n");
        return HI_FAILURE;
    }

    hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
    if (hal_ctrl_get_5v_status(port) == HI_TRUE) {
        hal_damix_set_term_mode(port, HDMIRX_TERM_SEL_OFF);
        hal_ctrl_set_hpd_level(port, HI_FALSE);
    }
    hdmirxv2_drv_ctrl_edid_updateby_port(port);
    if (hal_ctrl_get_5v_status(port) == HI_TRUE) {
        osal_msleep(300); /* 300: delay time */
        hal_damix_set_term_mode(port, HDMIRX_TERM_SEL_HDMI);
        osal_msleep(100); /* 100: dealy time */
        hal_ctrl_set_hpd_level(port, HI_TRUE);
    }
    if (task_index >= 0) {
        hi_dbg_hdmirx("set %d task to can_free.\n", task_index);
        ctrl_ctx_info->edid_fresh_task[task_index].can_free = HI_TRUE;
    }
    return HI_SUCCESS;
}

hi_void hdmirxv2_drv_ctrl_record_task(osal_task *task, hi_s32 index)
{
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;
    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    ctrl_ctx_info->edid_fresh_task[index].task = task; /* record task addr */
    ctrl_ctx_info->edid_fresh_task[index].is_free = HI_FALSE;
    hi_warn_hdmirx("record %d task to queue.\n", index);
}

hi_s32 hdmirxv2_drv_ctrl_get_task_index(hi_void)
{
    hi_u32 index;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;
    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    for (index = 0; index < HDMRIX_EDID_TASK_NUM; index++) {
        if (ctrl_ctx_info->edid_fresh_task[index].is_free == HI_TRUE) {
            return index;
        }
    }
    if (index >= HDMRIX_EDID_TASK_NUM) {
        hi_dbg_hdmirx("no free task index!\n");
    }
    return HI_FAILURE;
}

hi_s32 hdmirxv2_drv_ctrl_edid_fresh(hi_drv_hdmirx_port port)
{
    osal_task *task = HI_NULL;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_s8 task_index;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    hi_dbg_hdmirx("EDID fresh for port %d \n", port);
    task_index = hdmirxv2_drv_ctrl_get_task_index();
    if (task_index < 0) {
        hi_dbg_hdmirx("no available task index!\n");
        return HI_SUCCESS; /* if keep pressing switch key, return success avoid MW report error */
    }
    hdmirxv2_ctrl_mode_change(port);
    hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_VIDEO_OFF);
    g_edid_fresh_thread_param = ((hi_u8)port & 0xf) | (((hi_u8)task_index & 0xf) << 4); /* set to high 4 bits */
    task = osal_kthread_create(hdmirxv2_drv_ctrl_edid_fresh_thread,
        (hi_void*)&g_edid_fresh_thread_param, "HDMIRX EDID task", 0);
    hdmirxv2_drv_ctrl_record_task(task, task_index);
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_check_update_edid_input(hi_drv_hdmirx_edid_info *edid)
{
    hi_s32 ret = HI_SUCCESS;
    HDMIRX_CHECK_NULL_PTR(edid, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (edid->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("input EDID port error!!\n");
        hi_info_hdmirx_func_exit();
        return HI_FAILURE;
    }
    if (edid->edid.edid_length != EDID_LENGTH) {
        hi_err_hdmirx("port length is invalid!\n");
        hi_info_hdmirx_func_exit();
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_update_edid(hi_drv_hdmirx_edid_info *edid)
{
    hi_s32 ret;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    errno_t err_ret;
    hi_info_hdmirx_func_enter();
    if (edid->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }
    ret = hdmirxv2_drv_ctrl_check_update_edid_input(edid);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    hi_dbg_hdmirx_print_u32(edid->port);
    hi_dbg_hdmirx_print_u32(edid->edid.edid_length);
    hi_dbg_hdmirx_print_u32(edid->edid.cec_addr);
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(edid->port);

    hdmirx_drv_edid_get_edid_type(edid->edid.edid_data, &(g_edid[edid->port].edid_type));

    /* start for EDID refresh */
    err_ret = memcpy_s(g_edid[edid->port].edid_data, sizeof(g_edid[edid->port].edid_data),
        edid->edid.edid_data, EDID_LENGTH * sizeof(hi_u8));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    g_edid[edid->port].cec_addr = edid->edid.cec_addr;
    /* end for EDID refresh */
    hdmirx_drv_edid_register(HDMIRX_EDID_20, edid->edid.edid_data, edid->edid.edid_length);

    if (edid->port < HI_DRV_HDMIRX_PORT_MAX) {
        ctrl_ctx->edid_info.cec_addr = edid->edid.cec_addr;
    }
    ctrl_ctx->edid_mode = HI_DRV_HDMIRX_EDID_MODE_MENU;
    ctrl_ctx->cur_edid_type = g_edid[edid->port].edid_type;

    ret = hdmirxv2_drv_ctrl_edid_fresh(edid->port);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("EDID fresh func call error\n");
        return ret;
    }

    hi_info_hdmirx_func_exit();
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_check_init_edid_input(hi_drv_hdmirx_edid_init *init_edid)
{
    hi_u32 index;
    for (index = 0; index < init_edid->count; index++) {
        if (init_edid->edid_group[index].edid_length != EDID_LENGTH) {
            hi_err_hdmirx("input edid Length error!!\n");
            return HI_FAILURE;
        }

        if (init_edid->edid_group[index].edid_data == NULL) {
            hi_err_hdmirx("input edid Data error!!\n");
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_init_edid(hi_drv_hdmirx_edid_init_info *edid)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 temp1;
    hdmirx_edid_type edid_type = HDMIRX_EDID_MAX;
    hi_drv_hdmirx_port tmp_port;
    errno_t err_ret;

    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    HDMIRX_CHECK_NULL_PTR(edid, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (edid->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(edid->port);
    if (edid->init_edid.count != 2) { /* 2: edid data must contain 2 groups */
        hi_err_hdmirx("input EDID count error!!\n");
        return HI_FAILURE;
    }
    ret = hdmirxv2_drv_ctrl_check_init_edid_input(&(edid->init_edid));
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    for (temp1 = 0; temp1 < edid->init_edid.count; temp1++) {
        hdmirx_drv_edid_get_edid_type(edid->init_edid.edid_group[temp1].edid_data, &edid_type);
        tmp_port = edid->port;

        err_ret = memcpy_s(ctrl_ctx->edid_info.edid_data[edid_type], EDID_LENGTH * sizeof(hi_u8),
            edid->init_edid.edid_group[temp1].edid_data, EDID_LENGTH * sizeof(hi_u8));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return err_ret;
        }

        ctrl_ctx->edid_info.cec_addr = edid->init_edid.edid_group[temp1].cec_addr;
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_set_edid_mode(hi_drv_hdmirx_edid_mode_info *edid_mode)
{
    hi_s32 ret = HI_SUCCESS;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    HDMIRX_CHECK_NULL_PTR(edid_mode, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (edid_mode->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(edid_mode->port);

    if (edid_mode->port >= HI_DRV_HDMIRX_PORT_MAX) {
        return HI_FAILURE;
    }

    hi_warn_hdmirx("set port%d EDID mode %s\n", edid_mode->port,
        (edid_mode->mode == HI_DRV_HDMIRX_EDID_MODE_MENU ? "menu" : "auto"));
    if (ctrl_ctx->edid_mode == edid_mode->mode) {
        return HI_SUCCESS;
    }
    ctrl_ctx->edid_mode = edid_mode->mode;

    if (ctrl_ctx->edid_mode == HI_DRV_HDMIRX_EDID_MODE_AUTO) {
        ctrl_ctx->cur_edid_type = HDMIRX_EDID_20;
        if (ctrl_ctx->cur_edid_type != HDMIRX_EDID_20) {
            hdmirxv2_drv_ctrl_edid_fresh(edid_mode->port);
        }
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_set_hpd_value(hi_drv_hdmirx_hpd *hpd)
{
    hi_s32 ret = HI_SUCCESS;
    hi_info_hdmirx_func_enter();
    HDMIRX_CHECK_NULL_PTR(hpd, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    if (hpd->port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("input HDMI port error!!\n");

        return HI_FAILURE;
    }

    hi_dbg_hdmirx("set HPD value");
    hi_dbg_hdmirx_print_u32(hpd->port);
    hi_dbg_hdmirx_print_u32(hpd->value);
    hal_ctrl_set_hpd_level(hpd->port, hpd->value);
    hi_info_hdmirx_func_exit();
    return HI_SUCCESS;
}

hi_void hdmirxv2_drv_ctrl_de_init(hi_void)
{
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;
    hi_drv_hdmirx_port port;
    hi_s32 ret;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    for (port = ctrl_ctx_info->start_port; port < ctrl_ctx_info->end_port; port++) {
        hdmirx20_drv_ctrl_hdcp_de_init();
        ret = hdmirxv2_drv_ctrl_disconnect(port);
        if (ret != HI_SUCCESS) {
            hi_err_hdmirx("disconnect func call error\n");
        }
    }
    ctrl_ctx_info->run = HI_FALSE;
    hdmirx_hal_reg_deinit();
}

#ifdef HI_FPGA
gpio_i2c_ext_func *g_gpio_i2c_func_p = HI_NULL;
hi_u32 g_i2c_num;
static hi_void fpga_hdmirx_i2c_write(gpio_i2c_ext_func *g_gpio_i2c_func_p,
    hi_u32 i2c_channel, hi_u32 slave_addr, hi_u32 reg_addr, hi_u8 value)
{
    hi_s32 ret;

    ret = g_gpio_i2c_func_p->pfn_gpio_i2c_write_ext(i2c_channel, slave_addr, reg_addr, 1, &value, 1);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("###addr:0x%x, data:0x%x fail!\n", reg_addr, value);
    }
}

hi_void fpga_hdmirxi2c_write(hi_u8 i2c_addr, hi_u8 addr, hi_u16 length, hi_u8 *buf)
{
    hi_u32 num;
    for (num = 0; num < length; num++) {
        fpga_hdmirx_i2c_write(g_gpio_i2c_func_p, g_i2c_num, i2c_addr, addr + num, *(buf++));
        osal_msleep(1);
    }
    return;
}

hi_void fpga_hdmirx_apb_i2c_wr_bridge(hi_u32 small_addr, hi_u32 big_addr, hi_u32 value)
{
    hi_u32 tmp_addr;
    hi_u8 apb_reg_wr[9]; /* 9: array index max value */
    hi_u8 HDMIRX_I2C_ADDR = 0xe6;

    tmp_addr = small_addr + big_addr;

    apb_reg_wr[0] = tmp_addr & 0xff;
    apb_reg_wr[1] = (tmp_addr >> 8) & 0xff;  /* 1: array index 8: tmp_addr right shift */
    apb_reg_wr[2] = (tmp_addr >> 16) & 0xff; /* 2: array index 16: tmp_addr right shift */
    apb_reg_wr[3] = (tmp_addr >> 24) & 0xff; /* 3: array index 24: tmp_addr right shift */
    apb_reg_wr[4] = value & 0xff;            /* 4: array index */
    apb_reg_wr[5] = (value >> 8) & 0xff;     /* 5: array index 8: value right shift */
    apb_reg_wr[6] = (value >> 16) & 0xff;    /* 6: array index 16: value right shift */
    apb_reg_wr[7] = (value >> 24) & 0xff;    /* 7: array index 24: value right shift */
    apb_reg_wr[8] = 0x1;                     /* 8: array index */

    fpga_hdmirxi2c_write(HDMIRX_I2C_ADDR, 0x0, 8, apb_reg_wr);      /* 8: the length of num */
    fpga_hdmirxi2c_write(HDMIRX_I2C_ADDR, 0x11, 1, &apb_reg_wr[8]); /* 1: the length of num */
}

static hi_u8 fpga_hdmirx_i2c_read(gpio_i2c_ext_func *g_gpio_i2c_func_p,
    hi_u32 i2c_channel, hi_u32 slave_addr, hi_u32 reg_addr)
{
    hi_s32 ret;
    hi_u8 value = 0;

    ret = g_gpio_i2c_func_p->pfn_gpio_i2c_read_ext(i2c_channel, slave_addr, reg_addr, 1, &value, 1);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("hdmi_rx gpic_i2c_read failed!\n");
    }
    return value;
}

hi_void fpga_hdmirxi2c_read(hi_u8 i2c_addr, hi_u8 addr, hi_u16 length, hi_u8 *buf)
{
    hi_u32 num;
    for (num = 0; num < length; num++) {
        *(buf++) = fpga_hdmirx_i2c_read(g_gpio_i2c_func_p, g_i2c_num, i2c_addr, addr + num);
    }
}

hi_u32 fpga_hdmirx_apb_i2c_rd_bridge(hi_u32 small_addr, hi_u32 big_addr)
{
    hi_u32 tmp_addr;
    hi_u32 tmp_value;
    hi_u8 apb_reg_wr[5];         /* 5: the array apb_reg_wr max index */
    hi_u8 apb_reg_rd[4] = { 0 }; /* 4: the apb_reg_rd max index */
    hi_u8 HDMIRX_I2C_ADDR = 0xe6;

    tmp_addr = small_addr + big_addr;

    apb_reg_wr[0] = tmp_addr & 0xff;
    apb_reg_wr[1] = (tmp_addr >> 8) & 0xff;  /* 1: array index 8: tmp_addr right shift */
    apb_reg_wr[2] = (tmp_addr >> 16) & 0xff; /* 2: array index 16: tmp_addr right shift */
    apb_reg_wr[3] = (tmp_addr >> 24) & 0xff; /* 3: array index 24: tmp_addr right shift */
    apb_reg_wr[4] = 0x2;                     /* 4: array index */
    fpga_hdmirxi2c_write(HDMIRX_I2C_ADDR, 0x0, 4, apb_reg_wr);      /* 4: the length of num */
    fpga_hdmirxi2c_write(HDMIRX_I2C_ADDR, 0x11, 1, &apb_reg_wr[4]); /* 1: the length of num */
    fpga_hdmirxi2c_read(HDMIRX_I2C_ADDR, 0x8, 4, apb_reg_rd);       /* 4: the length of num */

    tmp_value = ((hi_u32)apb_reg_rd[0] & 0xff) +              /* 0: array index */
                (((hi_u32)apb_reg_rd[1] << 8) & 0xff00) +     /* 1: array index 8: the right shift */
                (((hi_u32)apb_reg_rd[2] << 16) & 0xff0000) +  /* 2: array index 16: the right shift */
                (((hi_u32)apb_reg_rd[3] << 24) & 0xff000000); /* 3: array index 24: the right shift */

    hi_warn_hdmirx("HDMI read  tmp_addr = 0x%x   value = 0x%x\n", tmp_addr, tmp_value);
    return tmp_value;
}

#endif

hi_void hdmirxv2_drv_ctrl_edid_soft_init(hi_void)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    hdmirx_ctrl_context_info *ctrl_ctx_info;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    for (port = ctrl_ctx_info->start_port; port < ctrl_ctx_info->end_port; port++) {
        ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
        ctrl_ctx->edid_mode = HI_DRV_HDMIRX_EDID_MODE_MAX;
        ctrl_ctx->cur_edid_type = HDMIRX_EDID_MAX;
    }
}

hi_void hdmirxv2_drv_ctrl_edid_soft_resume(hi_void)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    for (port = ctrl_ctx_info->start_port; port < ctrl_ctx_info->end_port; port++) {
        ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
        ctrl_ctx->cur_edid_type = HDMIRX_EDID_MAX;
    }
}

hi_s32 hdmirxv2_drv_ctrl_init_fpga(hi_void)
{
#ifdef HI_FPGA
    hi_s32 ret;
    hi_u8 tmp = 0;
    hi_u8 hdmirx1_i2c_gpio_scl = 36; /* 36: scl */
    hi_u8 hdmirx1_i2c_gpio_sda = 37; /* 37: sda */

    ret = osal_exportfunc_get(HI_ID_GPIO_I2C, (hi_void **)&g_gpio_i2c_func_p);
    if (ret == HI_FAILURE) {
        hi_err_hdmirx("GPIO I2C erro!!!\n");
        return HI_FAILURE;
    }
    HDMIRX_CHECK_NULL_PTR(g_gpio_i2c_func_p, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    g_pst_reg_peri->FPGA_CTRL1 = 3; /* 3: set FPGA_CTRL1 to 3 */

    ret = g_gpio_i2c_func_p->pfn_gpio_i2c_create_channel(&g_i2c_num, hdmirx1_i2c_gpio_scl, hdmirx1_i2c_gpio_sda);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("I2C1 init failure!\n");
        return HI_FAILURE;
    } else {
        fpga_hdmirx_apb_i2c_wr_bridge(0x001c, 0x00170000, 0xFFFFBFF);
        fpga_hdmirx_apb_i2c_wr_bridge(0x001c, 0x00170000, 0xFFBFF);
        fpga_hdmirx_apb_i2c_wr_bridge(0x03d4, 0x00110000, 0x0000000);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0024, 0x00110000, 0x000000b0);
        fpga_hdmirx_apb_i2c_wr_bridge(0x008c, 0x00140000, 0x00020820);
        fpga_hdmirx_apb_i2c_wr_bridge(0x00c8, 0x00140000, 0x00000000);
        fpga_hdmirx_apb_i2c_wr_bridge(0x00c4, 0x00140000, 0x00000070);
        fpga_hdmirx_apb_i2c_wr_bridge(0x00cc, 0x00140000, 0x8003E00);
        fpga_hdmirx_apb_i2c_wr_bridge(0x00b0, 0x00140000, 0x80000000);
        fpga_hdmirx_apb_i2c_wr_bridge(0x00b4, 0x00140000, 0x80000000);
        fpga_hdmirx_apb_i2c_wr_bridge(0x00ac, 0x00140000, 0x80000000);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0088, 0x00140000, 0x24924);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0038, 0x00100000, 0x00000500);
        fpga_hdmirx_apb_i2c_wr_bridge(0x003c, 0x00100000, 0x00000001);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0024, 0x00170000, 0x00007fca);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0024, 0x00170000, 0x00000fca);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0000, 0x001a0000, 0x81800000);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0050, 0x00100000, 0x00000001);
        fpga_hdmirx_apb_i2c_wr_bridge(0x0050, 0x00100000, 0x00000000);

        tmp = fpga_hdmirx_apb_i2c_rd_bridge(0x0030, 0x00110000);
        hi_warn_hdmirx("=====i2c read status is %x ======\n", tmp);
    }
#endif
    return HI_SUCCESS;
}

hi_bool hdmirxv2_drv_ctrl_get_invert_flag(hi_drv_hdmirx_port port)
{
    hi_bool invert = HI_FALSE;
    if (port == HI_DRV_HDMIRX_PORT0) {
#ifdef HI_BOARD_HDMIRX_PORT0_HPD_INVERT
        invert = HI_TRUE;
#endif
    } else if (port == HI_DRV_HDMIRX_PORT1) {
#ifdef HI_BOARD_HDMIRX_PORT1_HPD_INVERT
        invert = HI_TRUE;
#endif
    } else if (port == HI_DRV_HDMIRX_PORT2) {
#ifdef HI_BOARD_HDMIRX_PORT2_HPD_INVERT
        if (HI_BOARD_HDMIRX_PORT2_HPD_INVERT == HI_TRUE) {
            invert = HI_TRUE;
        }
#endif
    } else if (port == HI_DRV_HDMIRX_PORT3) {
#ifdef HI_BOARD_HDMIRX_PORT3_HPD_INVERT
        if (HI_BOARD_HDMIRX_PORT3_HPD_INVERT == HI_TRUE) {
            invert = HI_TRUE;
        }
#endif
    }
    return invert;
}

hi_s32 hdmirx_drv_fpga_edid_init(hi_void)
{
    errno_t err_ret;
    hi_s32 ret;

    ret = hdmirxv2_drv_ctrl_init_fpga();
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("init fpga func call error\n");
        return ret;
    }
    err_ret = memset_s(g_edid, sizeof(g_edid), 0,
        4 * sizeof(hdmirx_ctrl_edid)); /* 4: g_edid contains 4 hdmirx_ctrl_edid unit */
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    hdmirx_hal_reg_init();
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_init(hi_void)
{
    errno_t err_ret;
    hi_u32 index;
    hi_bool invert;
    hi_s32 ret;
    hi_drv_hdmirx_port tmp_port;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    hdmirx_ctrl_event_type event;
    err_ret = memset_s(ctrl_ctx_info, sizeof(hdmirx_ctrl_context_info), 0, sizeof(hdmirx_ctrl_context_info));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    ret = hdmirx_drv_fpga_edid_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }
    ctrl_ctx_info->start_port = HI_DRV_HDMIRX_PORT0;
    ctrl_ctx_info->end_port = HI_DRV_HDMIRX_PORT1; /* HI_DRV_HDMIRX_PORT3; */
    ctrl_ctx_info->cur_port = HI_DRV_HDMIRX_PORT_MAX;
    for (tmp_port = ctrl_ctx_info->start_port; tmp_port <= ctrl_ctx_info->end_port; tmp_port++) {
        hal_phy_init(tmp_port);
        hal_ctrl_init(tmp_port);
        invert = hdmirxv2_drv_ctrl_get_invert_flag(tmp_port);
        hal_ctrl_set_hpd_pol(tmp_port, invert);
        hal_ctrl_set_hpd_level(tmp_port, HI_FALSE);
        hdmirx_ctrl_ctx_init(&(ctrl_ctx_info->ctrl_ctx[tmp_port]));
        ctrl_ctx_info->ctrl_ctx[tmp_port].port = tmp_port;
        hdmirxv2_audio_initial(tmp_port);
        hdmirxv2_video_timing_data_init(tmp_port);
        hdmirxv2_packet_initial(tmp_port);
        event.sys_event = HDMIRX_SYS_EVENT_ALL;
        hal_ctrl_set_pwd_hw_mute(tmp_port, HDMIRX_HW_MUTE_SYS_EVENT_DEPACK, event, HI_TRUE);
        drv_hdmirx_phy_init(tmp_port);
        hal_phy_init(tmp_port);
        hal_phy_set_power(tmp_port, HDMIRX_INPUT_OFF);
    }
    for (index = 0; index < HDMRIX_EDID_TASK_NUM; index++) {
        ctrl_ctx_info->edid_fresh_task[index].is_free = HI_TRUE;
        ctrl_ctx_info->edid_fresh_task[index].can_free = HI_FALSE;
        ctrl_ctx_info->edid_fresh_task[index].task = HI_NULL;
    }
    hdmirx_register_fun(HI_DRV_HDMIRX_DATA_ROUTE_CTRL0, &g_hdmirx_call_func1);
    ctrl_ctx_info->run = HI_TRUE;
#if SUPPORT_CEC
    hdmirxv2_cec_init(HI_DRV_HDMIRX_PORT1);
#endif
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_suspend(hi_void)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;
    hi_drv_hdmirx_port port;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    ctrl_ctx_info->run = HI_FALSE;
    for (port = ctrl_ctx_info->start_port; port < ctrl_ctx_info->end_port; port++) {
        ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
        hdmirx_ctrl_port5v_det_ctrl_hpd(port);
        ctrl_ctx->hdcp_edid_ready = HI_FALSE;
        ctrl_ctx->power = 0;
        hdmirx20_drv_ctrl_hdcp_de_init();
    }
#if SUPPORT_CEC
    hdmirxv2_cec_de_init();
#endif
    osal_kthread_destroy(ctrl_ctx_info->resume_thread_task, 0);
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_hdcp_resume_thread(hi_void *pData)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;
    hi_drv_hdmirx_port port;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();

    for (port = ctrl_ctx_info->start_port; port < ctrl_ctx_info->end_port; port++) {
        ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

        hi_dbg_hdmirx("EDID rssume !!!!\n");
        hdmirxv2_drv_ctrl_edid_updateby_port(port);
        ctrl_ctx->hdcp_edid_ready = HI_TRUE;
    }
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_resume(hi_void)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    osal_task *resume_thread = HI_NULL;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;
    hi_drv_hdmirx_port port;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();

    for (port = ctrl_ctx_info->start_port; port < ctrl_ctx_info->end_port; port++) {
        ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
        hal_phy_set_power(HI_DRV_HDMIRX_PORT0, HDMIRX_INPUT_OFF);

#ifdef CUSTOM_SCL_DETECT_DELAY
        hdmirxv2_hal_ready_ctrl(port);
#endif
#if SUPPORT_CEC
        hdmirxv2_cec_init(port);
#endif

        hdmirxv2_packet_reset_aud_info_frame_data(port);
        hdmirx20_drv_ctrl_hdcp_init();

        hdmirxv2_custom_edid_method(port, HI_TRUE);

        hdmirx_ctrl_ctx_init(&(ctrl_ctx_info->ctrl_ctx[port]));
        ctrl_ctx_info->ctrl_ctx[port].port = port;
    }
    hdmirxv2_drv_ctrl_edid_soft_resume();
    resume_thread = osal_kthread_create(hdmirxv2_drv_ctrl_hdcp_resume_thread, NULL, "HDMIRX resume task", 0);
    if (resume_thread == HI_NULL) {
        hi_err_hdmirx("HDMIRX resume task create fail!\n");
    } else {
        ctrl_ctx_info->resume_thread_task = resume_thread;
    }
    ctrl_ctx_info->run = HI_TRUE;
    return HI_SUCCESS;
}

/* add for EDID refresh when hdmi port connected,don't need reset hpd */
hi_void hdmirxv2_ctrl_edid_switch(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    hi_warn_hdmirx("ctrl_ctx->cur_edid_type:%d  ", ctrl_ctx->cur_edid_type);
    hi_warn_hdmirx("ctrl_ctx->edid_mode[port] :%d \n", ctrl_ctx->edid_mode);
    /* if EDID mode is menu ,only update EDID when EDID is diff */
    /* if EDID mode is auto ,only update EDID when EDID is 1.4 */
    if ((ctrl_ctx->edid_mode != HI_DRV_HDMIRX_EDID_MODE_AUTO) &&
        (ctrl_ctx->cur_edid_type != g_edid[port].edid_type)) {
        hi_warn_hdmirx("hdmirxv2_ctrl_edid_switch to %s\n",
            (g_edid[port].edid_type == HDMIRX_EDID_14 ? "1.4" : "2.0"));
        hdmirxv2_drv_ctrl_edid_updateby_port(port);
        ctrl_ctx->cur_edid_type = g_edid[port].edid_type;
    } else if ((ctrl_ctx->edid_mode == HI_DRV_HDMIRX_EDID_MODE_AUTO) &&
               (ctrl_ctx->cur_edid_type == HDMIRX_EDID_14)) {
        ctrl_ctx->cur_edid_type = HDMIRX_EDID_20;
        hi_warn_hdmirx("IN auto mode,swicht EDID to 2.0 first!\n");
        hdmirxv2_drv_ctrl_edid_updateby_port(port);
    } else {
        hi_dbg_hdmirx("do not refresh EDID!!\n");
    }
}

/*
 * used for MW throngh UNF when select the HDMI port.
 */
hi_s32 hdmirxv2_drv_ctrl_connect(hi_drv_hdmirx_port port)
{
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }

    hdmirxv2_audio_set_audio_ready(port, HI_TRUE);
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_disconnect(hi_drv_hdmirx_port port)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;

    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("port index is invalid!\n");
        return HI_FAILURE;
    }

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    hi_info_hdmirx_func_enter();

    hdmirxv2_audio_enable(port, HI_FALSE);
    hdmirxv2_audio_change_state(port, AUDIO_STATE_OFF, 0);
    hdmirxv2_audio_set_audio_ready(port, HI_FALSE);

    hi_info_hdmirx_func_exit();
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_cec_enable(hi_bool *enable)
{
#if SUPPORT_CEC
    hi_s32 ret = HI_SUCCESS;
    hi_info_hdmirx_func_enter();
    HDMIRX_CHECK_NULL_PTR(enable, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    hdmirxv2_cec_enable(HI_DRV_HDMIRX_PORT1, *enable);
    hi_info_hdmirx_func_exit();
#endif
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_cec_set_command(hi_drv_hdmirx_cec_cmd *cec_cmd)
{
#if SUPPORT_CEC
    hi_s32 ret = HI_SUCCESS;
    errno_t err_ret;
    hdmirx_cec_message cec_msg;
    err_ret = memset_s(&cec_msg, sizeof(hdmirx_cec_message), 0, sizeof(hdmirx_cec_message));
    if (err_ret != 0) {
        hi_err_hdmirx("secure func call failed!\n");
        return HI_FAILURE;
    }
    HDMIRX_CHECK_NULL_PTR(cec_cmd, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    hi_dbg_hdmirx_func_enter();

    hi_dbg_hdmirx("set CEC command");
    hdmirxv2_drv_ctrl_show_cec_data(cec_cmd);

    cec_msg.src_dest_addr = MAKE_SRCDEST(cec_cmd->src_addr, cec_cmd->dst_addr);
    cec_msg.opcode = cec_cmd->opcode;
    cec_msg.arg_count = cec_cmd->operand.arg_count;

    if (cec_msg.arg_count > CEC_MAX_CMD_SIZE) {
        cec_msg.arg_count = CEC_MAX_CMD_SIZE;
    }

    if (cec_msg.arg_count != 0) {
        err_ret = memcpy_s(cec_msg.args, sizeof(cec_msg.args), cec_cmd->operand.args,
            (cec_msg.arg_count) * sizeof(hi_u32));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return err_ret;
        }
    }
    if (cec_msg.opcode == HI_DRV_HDMIRX_CEC_OPCODE_REPORT_ARC_INITIATED) {
        hdmirxv2_hal_cec_arc_enable(HI_DRV_HDMIRX_PORT1, HI_TRUE);
    }
    if (cec_msg.opcode == HI_DRV_HDMIRX_CEC_OPCODE_REPORT_ARC_TERMINATED) {
        hdmirxv2_hal_cec_arc_enable(HI_DRV_HDMIRX_PORT1, HI_FALSE);
    }

    hdmirxv2_cec_en_queue(&cec_msg);
    hi_dbg_hdmirx_func_exit();
#endif

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_cec_get_command(hi_drv_hdmirx_cec_cmd *cec_cmd)
{
#if SUPPORT_CEC
    hdmirx_cec_message cec_msg;
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i;
    errno_t err_ret;

    hi_u32 wake_up_by_mcu = hdmirxv2_cec_hal_pm_reg_read(0x1);
    HDMIRX_CHECK_NULL_PTR(cec_cmd, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    err_ret = memset_s(cec_cmd, sizeof(hi_drv_hdmirx_cec_cmd), 0, sizeof(hi_drv_hdmirx_cec_cmd));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    if (wake_up_by_mcu == 1) {
        cec_msg.src_dest_addr = hdmirxv2_cec_hal_pm_reg_read(0x4);

        cec_cmd->dst_addr = 0x00;
        cec_cmd->src_addr = ((cec_msg.src_dest_addr) & 0xf0) >> 4; /* 4: get high 4 bits */
        cec_cmd->operand.arg_count = 0;
        cec_cmd->opcode = HI_DRV_HDMIRX_CEC_OPCODE_IMAGE_VIEW_ON;

        wake_up_by_mcu = 0;
        hdmirxv2_cec_hal_pm_reg_write(0x1, wake_up_by_mcu);
        hi_dbg_hdmirx("get CEC command");
        hdmirxv2_drv_ctrl_show_cec_data(cec_cmd);

        return HI_SUCCESS;
    }

    ret = hdmirxv2_cec_receive_msg(HI_DRV_HDMIRX_PORT1, &cec_msg);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    } else {
        cec_cmd->dst_addr = (cec_msg.src_dest_addr) & 0x0f;
        cec_cmd->src_addr = ((cec_msg.src_dest_addr) & 0xf0) >> 4; /* 4: get high 4 bits */
        cec_cmd->operand.arg_count = cec_msg.arg_count;
        cec_cmd->opcode = cec_msg.opcode;

        if (cec_cmd->operand.arg_count > CEC_MAX_CMD_SIZE) {
            cec_cmd->operand.arg_count = CEC_MAX_CMD_SIZE;
        }

        for (i = 0; i < cec_cmd->operand.arg_count; i++) {
            cec_cmd->operand.args[i] = cec_msg.args[i];
        }
        hi_dbg_hdmirx("get CEC command");
        hdmirxv2_drv_ctrl_show_cec_data(cec_cmd);
    }
#endif
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_cec_get_cur_state(hi_drv_hdmirx_cec_cmd_state_data *cmd_state)
{
#if SUPPORT_CEC
    hdmirx_cec_context *pst_cec_ctx = hdmirxv2_cec_get_cec_ctx();
    errno_t err_ret;
    hi_s32 ret = HI_SUCCESS;
    HDMIRX_CHECK_NULL_PTR(cmd_state, ret);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }
    err_ret = memset_s(cmd_state, sizeof(hi_drv_hdmirx_cec_cmd_state_data), 0,
        sizeof(hi_drv_hdmirx_cec_cmd_state_data));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    if (pst_cec_ctx->cur_state != HDMIRX_CEC_STATE_IDLE) {
        return HI_FAILURE;
    }

    cmd_state->cur_state = (hi_drv_hdmirx_cec_cmd_state)(pst_cec_ctx->tx_state);
    cmd_state->cur_opcode = pst_cec_ctx->cur_opcode;
    pst_cec_ctx->cur_state = HDMIRX_CEC_STATE_SENDING;
#endif
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_cec_get_msg_cnt(hi_u8 *cmd_cnt)
{
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_ctrl_cec_standby_enable(hi_bool enable)
{
#if SUPPORT_CEC
    hi_info_hdmirx_func_enter();
    hi_dbg_hdmirx("set CEC standby");
    hi_dbg_hdmirx_print_u32(enable);

    hdmirxv2_cec_standby_enable(enable);
    hi_info_hdmirx_func_exit();
#endif
    return HI_SUCCESS;
}
