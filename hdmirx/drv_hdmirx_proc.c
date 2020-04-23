/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of proc functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "drv_hdmirx_proc.h"
#include "drv_hdmirx_ctrl.h"
#include "drv_hdmirx_video.h"
#include "drv_hdmirx_audio.h"
#include "drv_hdmirx_edid.h"
#include "hi_osal.h"
#include "version.h"
#include "drv_hdmirx_ctrl.h"
#include "hal_hdmirx_video.h"
#include "hal_hdmirx_ctrl.h"
#include "hal_hdmirx_phy.h"
#include "hal_hdmirx_hdcp.h"
#include "hal_hdmirx_audio.h"
#include "hal_hdmirx_comm.h"
#include <securec.h>
#include <linux/seq_file.h>
#include "drv_hdmirx_phy.h"

static osal_proc_cmd g_hdmirx20_proc_cmd_array[] = {
    { "help",         hdmirxv2_drv_proc_cmd_help },
    { "thread",       hdmirxv2_drv_proc_thread_state },
    { "bypass",       hdmirxv2_drv_proc_video_bypass },
    { "edidinfo",     hdmirxv2_drv_proc_edid_info },
    { "updateedid",   hdmirxv2_drv_proc_update_edid },
    { "selectport",   hdmirxv2_drv_proc_select_port },
    { "ddcen",        hdmirxv2_drv_proc_set_ddc_en },
    { "sethpd",       hdmirxv2_drv_proc_set_hpd },
    { "softrst",      hdmirxv2_drv_proc_soft_ware_rst },
    { "rstaudiofifo", hdmirxv2_drv_proc_rst_audio_fifo },
    { "termmode",     hdmirxv2_drv_proc_set_trem_mode },
    { "audstate",     hdmirxv2_drv_proc_set_audio_state },
    { "vidstate",     hdmirxv2_drv_proc_set_video_state },
    { "svstate",      hdmirxv2_drv_proc_over_write_v_state },
    { "getedid",      hdmirxv2_drv_proc_get_edid },
    { "hdcperrch",    hdmirxv2_drv_proc_set_hdcp_err_chk },
    { "mute",         hdmirxv2_drv_proc_set_mute_state },
    { "phypwr",       hdmirxv2_drv_proc_set_phy_power },
    { "phyinit",      hdmirxv2_drv_proc_phy_init },
    { "seteq",        hdmirxv2_drv_proc_set_eq_code },
    { "autoeq",       hdmirxv2_drv_proc_set_auto_eq_en },
    { "setch0eq",     hdmirxv2_drv_proc_set_lane0_eq },
    { "setch1eq",     hdmirxv2_drv_proc_set_lane1_eq },
    { "setch2eq",     hdmirxv2_drv_proc_set_lane2_eq },
    { "null",         NULL },
};

static hi_void hdmirxv2_ctrl_edid_chg(hdmirx_edid_type edid_type)
{
}

static hi_u32 hdmirx_drv_ctrl_trans_hex_str_to_dec(const char *src)
{
    hi_u32 sum = 0;
    int i = 0;
    int len;

    if (src == NULL) {
        return -1;
    }

    len = strlen(src);
    if (len == 0) {
        return -1;
    }

    if (src[len - 1] == '\n') {
        len--;
    }

    if ((len >= 2) && src[i] == '0' && src[i + 1] == 'x') { /* 2: skip "0x" 2 words */
        i += 2; /* 2: move ahead 2 bytes */
    }

    if (len > 10) { /* 10: at most 10 words, like: 0xffffffff */
        len = 10; /* 10: at most handle front 19 words */
    }

    for (; i < len; ++i) {
        if (src[i] >= '0' && src[i] <= '9') {
            sum = sum * 10 + (src[i] - '0');        /* 10: times 10, means carry forword */
        } else if (src[i] >= 'a' && src[i] <= 'f') {
            sum = sum * 0x10 + (src[i] - 'a' + 10); /* 10: 'a' value equals 10 */
        } else if (src[i] >= 'A' && src[i] <= 'F') {
            sum = sum * 0x10 + (src[i] - 'A' + 10); /* 10: 'A' regard as 'a' */
        } else {
            return -1;
        }
    }

    return sum;
}

hi_s32 hdmirxv2_drv_proc_thread_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_bool thread_state;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HI_NULL;

    ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();

    thread_state = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    osal_printk("thread_state :%d\n", thread_state);

    if (thread_state) {
        ctrl_ctx_info->run = HI_TRUE;
    } else {
        ctrl_ctx_info->run = HI_FALSE;
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_video_bypass(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_edid_info(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hdmirx_drv_edid_log(HDMIRX_EDID_20);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_update_edid(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_bool update14;

    update14 = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);
    if (update14) {
        hdmirxv2_ctrl_edid_chg(HDMIRX_EDID_14);
        osal_printk("update EDID 14 !\n");
    } else {
        hdmirxv2_ctrl_edid_chg(HDMIRX_EDID_20);
        osal_printk("update EDID 2.0 !\n");
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_select_port(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 port;

    port = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);
    if (port < HI_DRV_HDMIRX_PORT_MAX) {
        hdmirxv2_drv_ctrl_disconnect(port);
        hdmirxv2_drv_ctrl_connect(port);
        osal_printk("connect port %d SUCESS!\n", port);
        return HI_SUCCESS;
    } else {
        osal_printk("input port error!\n");
        return HI_FAILURE;
    }
}
hi_s32 hdmirxv2_drv_proc_set_hpd(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_bool hpd_high;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    hpd_high = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);
    hal_ctrl_set_hpd_level(port, hpd_high);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_ddc_en(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_bool ddc_en;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    ddc_en = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);
    if (ddc_en) {
        hal_ctrl_set_ddc_edid_en(port, HI_TRUE);
        osal_printk("enable DDC !\n");
    } else {
        hal_ctrl_set_ddc_edid_en(port, HI_FALSE);
        osal_printk("disable DDC !\n");
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_soft_ware_rst(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    osal_printk("cancel soft reset!\n");
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_rst_audio_fifo(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_trem_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 term_mode;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    term_mode = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    hal_damix_set_term_mode(port, term_mode);
    osal_printk("set term_mode %d!\n", term_mode);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_audio_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 state;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    state = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);
    hdmirxv2_audio_change_state(port, state, 0);
    osal_printk("set audio status to %d \n", state);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_video_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 state;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    state = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);
    hdmirxv2_ctrl_change_state(port, state);
    osal_printk("set video status to %d \n", state);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_over_write_v_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 state;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    state = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);
    if (state <= HI_DRV_SIG_MAX) {
        g_over_vide_sig_sta[port] = state;
        osal_printk("set video status to %d \n", state);
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_get_edid(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 temp = 0;
    hi_s32 ret;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    hi_char buf[HDMIRX_EDID_LENGTH] = {0};
    hi_char *tmp = NULL;

    ctrl_ctx = (hdmirx_ctrl_context *)private;
    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    tmp = buf;
    osal_printk("####EDID:");
    for (temp = 0; temp < HDMIRX_EDID_LENGTH; temp++) {
        if (temp % 16 == 0) { // per 16 bytes go to next line
            osal_printk("%s", buf);
            memset_s(buf, sizeof(buf), 0, sizeof(buf));
            tmp = buf;
        }
        ret = snprintf_s(tmp, sizeof(tmp), HDMIRX_EDID_PRINT_SIZE, "0x%02x  ,", g_edid[port].edid_data[temp]);
        if (ret < 0) {
            osal_printk("snprintf_s err\n");
            return ret;
        }
        tmp = tmp + HDMIRX_EDID_PRINT_SIZE;
    }
    osal_printk("%s", buf);
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_hdcp_err_chk(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_bool err_chk;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    err_chk = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    ctrl_ctx->hdcp_check_en = err_chk;

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_mute_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 mute_state;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    mute_state = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    switch (mute_state) {
        case 0: /* 0: case 0 */
            hal_ctrl_set_mute_en(port, HDMIRX_MUTE_ALL, HI_FALSE);
            osal_printk("mute off all\n");
            break;
        case 1: /* 1: case 1 */
            hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
            osal_printk("mute video\n");
            break;
        case 2: /* 2: case 2 */
            hal_ctrl_set_mute_en(port, HDMIRX_MUTE_AUD, HI_TRUE);
            osal_printk("mute audio\n");
            break;
        case 3: /* 3: case 3 */
            hal_ctrl_set_mute_en(port, HDMIRX_MUTE_ALL, HI_TRUE);
            osal_printk("mute all\n");
            break;
        default:
            osal_printk("mute para error\n");
            break;
    }

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_phy_power(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 phy_pwr_state;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    phy_pwr_state = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    hal_phy_set_power(port, phy_pwr_state);
    hdmirxv2_ctrl_set_input_type(port, phy_pwr_state);
    osal_printk("set phy power to %d \n", phy_pwr_state);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_phy_init(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 phy_init_state;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    phy_init_state = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    drv_hdmirx_phy_set_en(port, phy_init_state, HI_TRUE);
    osal_printk("set phy init to %d \n", phy_init_state);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_eq_code(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 eq_code;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    eq_code = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    hal_damix_set_handle_eq_code(port, eq_code);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_auto_eq_en(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_bool auto_eq;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    auto_eq = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_lane0_eq(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 eq_code;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }

    eq_code = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_lane1_eq(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 eq_code;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    eq_code = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_set_lane2_eq(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 eq_code;
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hi_drv_hdmirx_port port;
    ctrl_ctx = (hdmirx_ctrl_context *)private;

    port = ctrl_ctx->port;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_printk("CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);

    eq_code = hdmirx_drv_ctrl_trans_hex_str_to_dec(argv[1]);

    return HI_SUCCESS;
}

hi_void hdmirxv2_drv_proc_help(hi_void)
{
    osal_printk("                                                                        \n");
    osal_printk("--------------------------------- HDMIRX 2.0 -------------------------------\n");
    osal_printk("echo command      args   > /proc/msp/hdmirx | explaination           \n");
    osal_printk("------------------------------------------------------------------------\n");
    osal_printk("echo thread       [0/1]  > /proc/msp/hdmirx | disable HDMI process"
                "      [0:stop/1:resume]\n");
    osal_printk("echo bypass              > /proc/msp/hdmirx | video path is enable or bypassed  \n");
    osal_printk("echo edidinfo            > /proc/msp/hdmirx | show EDID capability \n");
    osal_printk("echo updateedid          > /proc/msp/hdmirx | change EDID to 1.4/2.0,   [0:2.0 1:1.4] \n");
    osal_printk("echo selectport   [0-2]  > /proc/msp/hdmirx | change curport to x"
                "       [0:port0/1:port1/2:port2]\n");
    osal_printk("echo ddcen        [0/1]  > /proc/msp/hdmirx |change curport DDC enable"
                "   [1:enable  0:disable]\n");
    osal_printk("echo sethpd       [0/1]  > /proc/msp/hdmirx | set the curport HPD to x"
                "  [0:low/1:high]\n");
    osal_printk("echo softrst             > /proc/msp/hdmirx | soft reset\n");
    osal_printk("echo rstaudiofifo        > /proc/msp/hdmirx | reset audio FIFO\n");
    osal_printk("echo audstate            > /proc/msp/hdmirx | set audio state"
                "           [0:OFF 1:unstable 3:ON] \n");
    osal_printk("echo vidstate            > /proc/msp/hdmirx | set video state"
                "           [0:OFF 1:ON 3:unstable] \n");
    osal_printk("echo svstate             > /proc/msp/hdmirx | set video state for MW"
                "    [0:support 1:no siganl 2: no support 3:unstable 4:no change] \n");
    osal_printk("echo getedid             > /proc/msp/hdmirx | get EDID data \n");
    osal_printk("echo termmode     [0/1]  > /proc/msp/hdmirx | set term_mode x,"
                "           [0:open 1:HDMI] \n");
    osal_printk("echo hdcperrch    [0/1]  > /proc/msp/hdmirx | HDCP err check enable"
                "     [0:disable/1:enable]\n");
    osal_printk("echo mute         [0-3]  > /proc/msp/hdmirx | mute video/audio/all"
                "      [0:off/1:video/2:audio/3:audio]\n");
    osal_printk("echo phypwr              > /proc/msp/hdmirx | set phy power to x"
                "        [0:1.4 1:2.0 2:0ff]\n");
    osal_printk("echo phyinit             > /proc/msp/hdmirx | set phy init to x"
                "         [0:1.4 1:2.0]\n");
    osal_printk("echo seteq               > /proc/msp/hdmirx | set all lane eq code\n");
    osal_printk("echo setch0eq            > /proc/msp/hdmirx | set lane 0 eq code\n");
    osal_printk("echo setch1eq            > /proc/msp/hdmirx | set lane 1 eq code\n");
    osal_printk("echo setch2eq            > /proc/msp/hdmirx | set lane 2 eq code\n");
    osal_printk("echo autoeq       [0/1]  > /proc/msp/hdmirx | set auto_eq enable"
                "         [0:disable/1:enable]\n");
    osal_printk("                                                                            \n");
    osal_printk("---------------------------------------------------------------------------\n");
}

hi_s32 hdmirxv2_drv_proc_cmd_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hdmirxv2_drv_proc_help();
    return HI_SUCCESS;
}

hi_void drv_hdmirxv2_ctrl_spd_proc_read(hi_drv_hdmirx_port port, hi_void *s)
{
    errno_t err_ret;

    if (hdmirxv2_packet_spd_is_got(port) == HI_TRUE) {
        hi_u32 data[SPD_PRODUCT_DES_LEN + 1] = { 0 };
        hi_u32 i;
        hdmirxv2_packet_spd_get_vendor_name(port, data, sizeof(data));
        osal_proc_print(s, "\nvendor name :");
        for (i = 0; i < SPD_VENDOR_NAME_LEN; i++) {
            if (data[i] == 0) {
                break;
            }
            osal_proc_print(s, "%c", data[i]);
        }
        err_ret = memset_s(data, sizeof(data), 0, (SPD_PRODUCT_DES_LEN + 1) * sizeof(hi_u32));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return;
        }
        hdmirxv2_packet_spd_get_product_des(port, data, sizeof(data));
        osal_proc_print(s, "\n_description :");
        for (i = 0; i < SPD_PRODUCT_DES_LEN; i++) {
            if (data[i] == 0) {
                break;
            }
            osal_proc_print(s, "%c", data[i]);
        }
        osal_proc_print(s, "\n");
    }
}

static hi_bool hdmirx_ctrl_ksv_check(hi_u32 *pstksv, hi_u32 len)
{
    hi_u32 i;
    hi_u32 j;
    hi_u32 one_count = 0;
    hi_u32 zero_count = 0;

    for (i = 0; i < 5; i++) { /* ksv has 5 HI_U32 */
        for (j = 0; j < 8; j++) { /* total 8 4bits, check every one */
            if ((pstksv[i] & (0x01 << j))) {
                one_count++;
            } else {
                zero_count++;
            }
        }
    }

    if (one_count == 20 || zero_count == 20) { /* check ksv has 20 '0' and 20 '1' */
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_void drv_hdmirxv2_ctrl_hdcp_info(hi_drv_hdmirx_port port, hi_void *s)
{
    hdmirx_ctrl_context *ctrl_ctx;
    hi_drv_hdmirx_hdcp_type hdcp_type;
    hi_u32 data[5] = { 0 }; /* aksv or bksv has 5 HI_U32 */
    hi_bool hdcp_status = HI_FALSE;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_proc_print(s, "CurPort is %d, ERRO!\n", port);
        return;
    }
    osal_proc_print(s, "\n---------------HDMIRX HDCP---------------\n");
    hdcp_type = hal_hdcp_check_type(port);
    if (hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_22) {
        hdcp_status = hal_hdcp_get_done(port, HI_DRV_HDMIRX_HDCPTYPE_22);
        hal_hdcp_get_bksv(port, HI_DRV_HDMIRX_HDCPTYPE_22, data, 5); /* 5: Bksv length */
        osal_proc_print(s, "HDCP Type            :   HDCP 2.2\n");
        osal_proc_print(s, "HDCP Check Status    :   %s\n", (hdcp_status ? "Pass" : "Fail"));
        osal_proc_print(s, "RX hdcp2.2 key       :   %s\n",
            (hdmirx_ctrl_ksv_check(data, sizeof(data) / sizeof(data[0])) ? "Valid" : "Invalid"));
    } else if (hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_14) {
        hdcp_status = hal_hdcp_get_done(port, HI_DRV_HDMIRX_HDCPTYPE_14);
        hal_hdcp_get_bksv(port, HI_DRV_HDMIRX_HDCPTYPE_14, data, 5); /* 5: Bksv length */
        osal_proc_print(s, "HDCP Type            :   HDCP 1.4\n");
        osal_proc_print(s, "HDCP Check Status    :   %s\n", (hdcp_status ? "Pass" : "Fail"));
        osal_proc_print(s, "RX hdcp1.4 Key       :   %s\n",
            (hdmirx_ctrl_ksv_check(data, sizeof(data) / sizeof(data[0])) ? "Valid" : "Invalid"));
        hal_hdcp_get_aksv(port, data, 5); /* 5: Bksv length */
        osal_proc_print(s, "TX hdcp1.4 key       :   %s\n",
            (hdmirx_ctrl_ksv_check(data, sizeof(data) / sizeof(data[0])) ? "Valid" : "Invalid"));
    } else {
        osal_proc_print(s, "HDCP Type    :  no hdcp\n");
    }
    osal_proc_print(s, "HdcpErrCheckEn       :   %s\n", ctrl_ctx->hdcp_check_en ? "Enable" : "Disable");
}

hi_void drv_hdmirxv2_ctrl_edid_info(hi_drv_hdmirx_port port, hi_void *s)
{
    hdmirx_ctrl_context *ctrl_ctx;

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    osal_proc_print(s, "\n---------------EDID Status---------------\n");
    osal_proc_print(s, "EDID Type            :%s   \n", ctrl_ctx->cur_edid_type == HDMIRX_EDID_14 ? "1.4" : "2.0");
    osal_proc_print(s, "Port%d EDID Mode      :%s   \n",
        port, ctrl_ctx->edid_mode == HI_DRV_HDMIRX_EDID_MODE_MENU ? "menu" : "auto");
}

hi_void drv_hdmirxv2_ctrl_color_space_info(hi_drv_hdmirx_port port, hi_void *s, hi_drv_color_descript color_space)
{
    if (color_space.color_primary == HI_DRV_COLOR_PRIMARY_BT601_525) {
        osal_proc_print(s, "color_primary        :   HI_DRV_COLOR_PRIMARY_BT601_525\n");
    } else if (color_space.color_primary == HI_DRV_COLOR_PRIMARY_BT601_625) {
        osal_proc_print(s, "color_primary        :   HI_DRV_COLOR_PRIMARY_BT601_625\n");
    } else if (color_space.color_primary == HI_DRV_COLOR_PRIMARY_BT709) {
        osal_proc_print(s, "color_primary        :   HI_DRV_COLOR_PRIMARY_BT709\n");
    } else if (color_space.color_primary == HI_DRV_COLOR_PRIMARY_BT2020) {
        osal_proc_print(s, "color_primary        :   HI_DRV_COLOR_PRIMARY_BT2020\n");
    }

    if (color_space.color_space == HI_DRV_COLOR_CS_YUV) {
        osal_proc_print(s, "ColorSpace           :   HI_DRV_COLOR_CS_YUV\n");
    } else if (color_space.color_space == HI_DRV_COLOR_CS_RGB) {
        osal_proc_print(s, "ColorSpace           :   HI_DRV_COLOR_CS_RGB\n");
    }

    if (color_space.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) {
        osal_proc_print(s, "quantify_range       :   HI_DRV_COLOR_LIMITED_RANGE");
    } else if (color_space.quantify_range == HI_DRV_COLOR_FULL_RANGE) {
        osal_proc_print(s, "quantify_range       :   HI_DRV_COLOR_FULL_RANGE");
    }

    if (hdmirxv2_packet_avi_get_color_space(port) == HDMIRX_COLOR_SPACE_RGB) {
        if (hdmirxv2_packet_avi_get_rgb_range(port) == HDMIRX_RGB_DEFAULT) {
            osal_proc_print(s, "(default)\n");
        } else {
            osal_proc_print(s, "\n");
        }
    } else {
        if (hdmirxv2_packet_avi_get_yuv_range(port) == HDMIRX_RANGE_BUTT) {
            osal_proc_print(s, "(default)\n");
        } else {
            osal_proc_print(s, "\n");
        }
    }
}

hi_void drv_hdmirxv2_ctrl_video_proc_print_timing(hi_void *s, hi_drv_hdmirx_port port,
    hi_drv_hdmirx_timing_info timing_info, hi_u32 value)
{
    osal_proc_print(s, "\n---------------HDMIRX Timing---------------\n");
    osal_proc_print(s, "TimingIndex          :   %d\n", timing_info.timing_idx);
    osal_proc_print(s, "Width                :   %d\n", timing_info.width);
    osal_proc_print(s, "Height               :   %d\n", timing_info.height);
    osal_proc_print(s, "FrameRate            :   %d\n", timing_info.frame_rate);
    osal_proc_print(s, "Interlace            :   %s\n", timing_info.interlace ? "interlace" : "progressive");
    osal_proc_print(s, "Htotal               :   %d\n",
        hal_video_get_htotal(port, HI_FALSE) * value);
    osal_proc_print(s, "Vtotal               :   %d\n",
        hal_video_get_vtotal(port, HI_FALSE));
    osal_proc_print(s, "PixelFreq            :   %d\n",
        hal_video_get_pix_clk(port) * value);
    osal_proc_print(s, "Hpol                 :   %s\n", hal_video_get_hpol(port) ? "POS" : "NEG");
    osal_proc_print(s, "Vpol                 :   %s\n", hal_video_get_vpol(port) ? "POS" : "NEG");
    if (timing_info.pc_mode) {
        osal_proc_print(s, "TimingMode           :   PcMode\n");
    } else if ((timing_info.timing_idx >= TIMING_MAX) &&
        (timing_info.timing_idx < (TIMING_MAX + NMB_OF_HDMI_VIDEO_MODES))) {
        osal_proc_print(s, "TimingMode           :   2D_4K*2K\n");
    } else {
        osal_proc_print(s, "TimingMode           :   2D_General\n");
    }

    osal_proc_print(s, "HdmiMode             :   %s\n", timing_info.hdmi_mode ? "HDMI" : "DVI");
    osal_proc_print(s, "bMHLMode             :   %s\n", timing_info.mhl ? "YES" : "NO");
}

hi_void drv_hdmirxv2_ctrl_video_on_proc_read(hi_drv_hdmirx_port port, hi_void *s)
{
    hdmirx_ctrl_context *ctrl_ctx;
    hi_drv_hdmirx_timing_info timing_info;
    hdmirx_color_space color_space_idx;
    hi_u32 value;
    static const hi_char *color_space[HDMIRX_COLOR_SPACE_MAX] = {
        "RGB444",
        "YCBCR422",
        "YCBCR444",
        "YCBCR420"
    };

    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    color_space_idx = hdmirxv2_packet_avi_get_color_space(port);

    value = (color_space_idx == HDMIRX_COLOR_SPACE_YCBCR420) ? /* 420 color_space */
        2 : 1; hdmirxv2_video_get_timing_info(port, &timing_info); /* 2 times htotal and pixel_freq */
    drv_hdmirxv2_ctrl_video_proc_print_timing(s, port, timing_info, value);

    drv_hdmirxv2_ctrl_color_space_info(port, s, timing_info.color_space);

    osal_proc_print(s, "InputPixelFmt        :   %s\n", color_space[color_space_idx]);
    if (timing_info.pixel_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        osal_proc_print(s, "OutputPixelFmt       :   YCBCR422\n");
    } else if (timing_info.pixel_fmt == HI_DRV_PIXEL_FMT_NV42) {
        osal_proc_print(s, "OutputPixelFmt       :   YCBCR444\n");
    } else if (timing_info.pixel_fmt == HI_DRV_PIXEL_FMT_NV21) {
        osal_proc_print(s, "OutputPixelFmt       :   YCBCR420\n");
    } else {
        osal_proc_print(s, "OutputPixelFmt       :   RGB444\n");
    }
    osal_proc_print(s, "BitWidth(Bit)        :   %d\n", (timing_info.bit_width * 2 + 8)); /* based on 8 bit, times 2 */
    hdmirxv2_video_proc_read(port, s);
    hdmirxv2_packet_proc_read(port, s);

    if (timing_info.hdmi_mode == HI_TRUE) {
        hdmirxv2_audio_proc_read(port, s);
    }
    drv_hdmirxv2_ctrl_spd_proc_read(port, s);
}

hi_void drv_hdmirxv2_ctrl_video_proc_print_ctrl(hi_drv_hdmirx_port port, hi_void *s)
{
    hi_drv_sig_info sig_sta;
    hi_drv_hdmirx_offline_status offline_stat;
    hdmirx_ctrl_context *ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    hdmirx_ctrl_context_info *ctrl_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    hi_u32 ctrl_base_addr[HI_DRV_HDMIRX_PORT_MAX] = {0x010fc000, 0x008d4000, 0, 0};

    osal_proc_print(s, "\n--------------- Ctrl 2.0 ---------------\n");
    osal_proc_print(s, "\n--------------- HDMIRX attr ---------------\n");
    osal_proc_print(s, "Version              :   %s\n", g_hdmirx_date);
    osal_proc_print(s, "Base Address         :   0x%x\n", ctrl_base_addr[port]);
    osal_proc_print(s, "ConnectState         :   %s\n", ctrl_info->run ? "Yes" : "No");
    osal_proc_print(s, "CurPort              :   port%d\n", ctrl_info->cur_port);
    osal_proc_print(s, "CurPortState         :   %s\n",
        g_state_string[ctrl_ctx->port_status.state]);
    osal_proc_print(s, "CurPortWaitCnt       :   %d\n", ctrl_ctx->port_status.wait_cnt);
    osal_proc_print(s, "HdcpEdidReady        :   %d\n", ctrl_ctx->hdcp_edid_ready);
    osal_proc_print(s, "InputType            :   %s\n",
        ctrl_ctx->input_type == HDMIRX_INPUT_20 ? "2.0" : "1.4");
    if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_ON) {
        osal_proc_print(s, "UnstableCnt          :   %d\n", ctrl_ctx->unstable_cnt);
    }

    osal_proc_print(s, "OffLineDetStatus:\n");
    offline_stat.port = port;
    if (hdmirxv2_drv_ctrl_get_offline_det_status(&offline_stat) == HI_SUCCESS) {
        osal_proc_print(s, "port%d:%s   ", port, offline_stat.info.connected ? "On" : "Off");
    } else {
        osal_proc_print(s, "port%d:erro ", port);
    }

    osal_proc_print(s, "\n");
    sig_sta.port = port;
    hdmirxv2_drv_ctrl_get_audio_status(&sig_sta);
    osal_proc_print(s, "Audio State : %s\n", (sig_sta.status == HI_DRV_SIG_SUPPORT) ? "support" : "no support");

    osal_proc_print(s, "\n---------------Mute Status---------------\n");
    osal_proc_print(s, "Video                :%s   \n",
        hal_ctrl_get_mute_en(port, HDMIRX_MUTE_VDO) ? "Mute" : "unMute");
    osal_proc_print(s, "Audio                :%s   \n",
        hal_ctrl_get_mute_en(port, HDMIRX_MUTE_AUD) ? "Mute" : "unMute");
    osal_proc_print(s, "HWAudio              :%s   \n",
        hal_audio_get_aac_mute_en(port) ? "Mute" : "unMute");
}


hi_s32 drv_hdmirxv2_ctrl_proc_read(hi_void *s, hi_void *data)
{
    hdmirx_ctrl_context *ctrl_ctx = HI_NULL;
    hdmirx_ctrl_context_info *ctrl_ctx_info = HDMIRXV2_CTRL_GET_CTX_INFO();
    hi_drv_hdmirx_port port;
    osal_proc_entry *entry = ((struct seq_file *)s)->private;
    hdmirx_hdcp_err_cnt hdcp_err;
    hi_u32 eq_value[3] = { 0 }; /* eq has 3 channels */
    hi_u32 ced_result[3] = { 0 }; /* 3 channels ced err */
    hi_bool eq_mode, is_ckdt, is_scdt;

    port = (hi_drv_hdmirx_port)(entry->name[6] - '0'); /* 6: entry name index */
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        osal_proc_print(s, "CurPort is %d, ERRO!\n", port);
        return HI_SUCCESS;
    }
    ctrl_ctx = HDMIRXV2_CTRL_GET_CTX(port);
    drv_hdmirxv2_ctrl_video_proc_print_ctrl(port, s);
    drv_hdmirxv2_ctrl_edid_info(port, s);
    drv_hdmirxv2_ctrl_hdcp_info(port, s);
    hdcp_err = hal_hdcp_get_bch_err_cnt(port);
    osal_proc_print(s, "\n---------------HDMIRX Phy Setting----------\n");
    is_ckdt = hal_ctrl_get_ckdt(port);
    osal_proc_print(s, "Ckdt                 :%s\n", is_ckdt ? "Yes" : "No");
    is_scdt = hal_ctrl_get_scdt(port);
    osal_proc_print(s, "Scdt                 :%s\n", is_scdt ? "Yes" : "No");
    osal_proc_print(s, "BchErrCnt            :0x%04x\n", hdcp_err.bch_err_cnt);
    eq_mode = 0; /* hdmirxv2_hal_get_eq_mode(port); */
    osal_proc_print(s, "EqMode               :%s\n", eq_mode ? "Handle" : "Auto");
    osal_proc_print(s, "-----------Ch0--------Ch1--------Ch2---------\n");
    /* 2: array index */
    osal_proc_print(s, "EQ :       %-2d         %-2d         %-2d      \n", eq_value[0], eq_value[1], eq_value[2]);
    ced_result[0] = hal_dphy_get_ced_err_cnt(HI_DRV_HDMIRX_PORT2, 0);
    ced_result[1] = hal_dphy_get_ced_err_cnt(HI_DRV_HDMIRX_PORT2, 1);
    ced_result[2] = hal_dphy_get_ced_err_cnt(HI_DRV_HDMIRX_PORT2, 2); /* 2: ced err on channel 2 */
    /* 2: ced err on channel 2 */
    osal_proc_print(s, "ced:       %-4d       %-4d       %-4d      \n", ced_result[0], ced_result[1], ced_result[2]);
    if (ctrl_ctx_info->run != HI_TRUE || ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_OFF) {
        return HI_SUCCESS;
    }
    /* HDMIRX signal timing information */
    if (ctrl_ctx->port_status.state == HDMIRX_STATE_VIDEO_ON) {
        drv_hdmirxv2_ctrl_video_on_proc_read(port, s);
    } else {
        osal_proc_print(s, "\n---------------HDMIRX timing---------------\n");
        osal_proc_print(s, "Htotal               :   %d\n", hal_video_get_htotal(port, HI_FALSE));
        osal_proc_print(s, "Vtotal               :   %d\n", hal_video_get_vtotal(port, HI_FALSE));
        osal_proc_print(s, "Interlace            :   %s\n", hal_video_get_interlance(port) ? "YES" : "NO");
        osal_proc_print(s, "PixelFreq            :   %d\n", hal_video_get_pix_clk(port));
        osal_proc_print(s, "Hpol                 :   %s\n", hal_video_get_hpol(port) ? "POS" : "NEG");
        osal_proc_print(s, "Vpol                 :   %s\n", hal_video_get_vpol(port) ? "POS" : "NEG");
    }
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_drv_proc_add(hi_drv_hdmirx_port port)
{
    hi_ulong proc_name_size = 7; /* 6: pre-set value */
    osal_proc_entry *hdmirx_proc_entry = HI_NULL;
    hi_char name[16] = {0}; /* 16: max name num */
    hi_s32 ret;

    ret = snprintf_s(name, sizeof(name), proc_name_size, "hdmirx%d", port);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return ret;
    }
    hdmirx_proc_entry = osal_proc_add(name, proc_name_size);
    if (hdmirx_proc_entry == HI_NULL) {
        hi_err_hdmirx("HDMIRX add proc fail!");
        return HI_FAILURE;
    }
    hdmirx_proc_entry->read = drv_hdmirxv2_ctrl_proc_read;
    hdmirx_proc_entry->cmd_cnt = sizeof(g_hdmirx20_proc_cmd_array) / sizeof(osal_proc_cmd);
    hdmirx_proc_entry->cmd_list = g_hdmirx20_proc_cmd_array;
    hdmirx_proc_entry->private = (hi_void *)HDMIRXV2_CTRL_GET_CTX(port);
    return HI_SUCCESS;
}

hi_void hdmirxv2_drv_proc_del(hi_void)
{
    hi_ulong proc_name_size = 6; /* 6: pre-set value */
    osal_proc_remove("hdmirx", proc_name_size);
}
