/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:  hitxv300 debug module main source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-10-29
 */

#include <linux/hisilicon/securec.h>
#include "hi_drv_proc.h"

#include "hal_hdmitx_phy.h"
#include "drv_hdmitx_proc.h"
#include "drv_hdmitx_debug.h"
#include "drv_hdmitx.h"
#include "drv_hdmitx_connector.h"
#include "drv_hdmitx_edid.h"
#include "hal_hdmitx_ctrl.h"
#include "drv_hdmitx_cec.h"
#include "hal_hdmitx_frl.h"

#define HDMI_DBG_PRINT hi_drv_proc_echo_helper
#define HDMI_DBG_ERR   \
    HDMI_DBG_PRINT("[HDMI DBG ERROR]: %s [%d],", __func__, __LINE__); \
    HDMI_DBG_PRINT

#define STR_SUCCESS "success"
#define STR_FAIL "fail"

#define HDMI_DEBUG_EDID_SIZE 512
#define AVMUTE_MAX_WAIT_TIME 10000
#define MAX_ALLOW_EMI_TMDS_CLK 300000

#define DBG_CEC_CMD_HEADER_LEN 2
#define DBG_CEC_CMD_DATA_OFFSET 2

typedef struct {
    hi_u32 min;
    hi_u32 max;
} hdmi_clk_range;

typedef struct {
    hi_u32 amptd;
    hi_u32 freq;
} phy_ssc_cfg;

typedef struct {
    hdmi_clk_range range;
    phy_ssc_cfg cfg;
} phy_ssc;

static phy_ssc g_phy_ssc_cfg_tab[] = {
    {{ 25000,  70000  }, { 200, 45 }},
    {{ 70001,  90000  }, { 200, 45 }},
    {{ 90001,  110000 }, { 200, 45 }},
    {{ 110001, 145000 }, { 200, 45 }},
    {{ 145001, 180000 }, { 100, 45 }},
    {{ 180001, 220000 }, { 80,  45 }},
    {{ 220001, 250000 }, { 80,  45 }},
    {{ 250001, 300000 }, { 40,  45 }},
    {{ 300001, 600000 }, { 0,   0  }},
};

static const hi_u8 *g_event_str_table[] = {
    "HDMITX_EVENT_HOTPLUG_IN",       /* HDMITX cable plug in */
    "HDMITX_EVENT_HOTPLUG_OUT",      /* HDMITX cable plug out */
    "HDMITX_EVENT_HDCP_CAP_CHANGE",  /* HDCP capability change */
    "HDMITX_EVENT_HDCP_FAIL",        /* HDCP authentication fail */
    "HDMITX_EVENT_HDCP_SUCCESS",     /* HDCP authentication success */
    "HDMITX_EVENT_RSEN_CONNECT",     /* HDMITX TMDS link connect */
    "HDMITX_EVENT_RSEN_DISCONNECT",  /* HDMITX TMDS link is disconnect */
    "HDMITX_EVENT_LINK_TRAIN_FAIL",  /* HDMITX FRL link training fail */
};

static const hi_u8 *g_color_fmt_str[] = {
    "RGB444",
    "YCBCR422",
    "YCBCR444",
    "YCBCR420",
};

static const hi_u8 *g_color_deepth_str[] = {
    "24bit",
    "30bit",
    "36bit",
    "48bit",
};

static const hi_u8 *g_frl_fifo_type[] = {
    "up",
    "down",
};

static hi_u8 g_edid_debug[HDMI_DEBUG_EDID_SIZE];

static hi_s32 debug_oe(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 argv1;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc != 2) { /* argc != 2 is error */
        HDMI_DBG_ERR("error argcs!\n");
        goto usage_error;
    }

    argv1 = simple_strtol(argv[1], NULL, STRING_TO_DEC);
    if (argv1 == CMD_NUM_0) {
        hdmi_phy_off(hdmi);
        HDMI_DBG_PRINT("oe set off %s!\n", STR_SUCCESS);
    } else if (argv1 == CMD_NUM_1) {
        hdmi_phy_on(hdmi);
        HDMI_DBG_PRINT("oe set on %s!\n", STR_SUCCESS);
    } else {
        HDMI_DBG_ERR("error argvs!\n");
        goto usage_error;
    }

    return HI_SUCCESS;

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo oe argv1 > /proc/msp/hdmi0.\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--oe off.\n"
                   "           1\t--oe on.\n");
    HDMI_DBG_PRINT("[example]: echo oe 1 > /proc/msp/hdmi0.\n");

    return -EFAULT;
}

static hi_s32 debug_edid(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 argv1;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;
    struct hdmi_connector *connector = hdmi->connector;

    if (connector == NULL) {
        HDMI_DBG_ERR("The pointer is NULL!\n");
        return -EINVAL;
    }

    if (argc != 2) { /* argc != 2 is error */
        HDMI_DBG_ERR("error argcs!\n");
        goto usage_error;
    }

    argv1 = simple_strtol(argv[1], NULL, STRING_TO_DEC);
    if (argv1 == CMD_NUM_0) {
        osal_mutex_lock(&connector->mutex);
        connector->edid_src_type = EDID_EMPTY;
        osal_mutex_unlock(&connector->mutex);
        hdmi_connector_enable_hpd(hdmi, connector);
    } else if (argv1 == CMD_NUM_1) {
        hdmi_connector_set_debug_edid(connector, NULL, 0);
    } else if (argv1 == CMD_NUM_2) {
        hdmi_connector_set_debug_edid(connector, g_edid_debug, sizeof(g_edid_debug));
        goto usage_error;
    } else {
        goto usage_error;
    }

    return 0;

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo edid argv1 [argv2] > /proc/msp/hdmi0.\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--read edid from sink.\n"
                   "           1\t--set edid for max 8k120 dsc.\n");
    HDMI_DBG_PRINT("[example]: echo edid 1 > /proc/msp/hdmi0.\n");

    return -EFAULT;
}

static hi_s32 debug_tmds_prior(struct hdmi_controller *controller, hi_u32 argv2)
{
    if (argv2 == CMD_NUM_0) {
        controller->work_mode_prior = HDMI_PRIOR_AUTIO;
        HDMI_DBG_PRINT("prior set auto\n");
    } else if (argv2 == CMD_NUM_1) {
        controller->work_mode_prior = HDMI_PRIOR_FORCE_TMDS;
        HDMI_DBG_PRINT("prior set force tmds\n");
    } else if (argv2 == CMD_NUM_2) {
        controller->work_mode_prior = HDMI_PRIOR_FORCE_FRL;
        HDMI_DBG_PRINT("prior set force frl\n");
    } else {
        HDMI_DBG_ERR("error argvs!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void debug_prior_help(hi_void)
{
    HDMI_DBG_PRINT("[Usage  ]: echo prior argv1 [argv2] > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[argv1  ]: prior debug command.\n");
    HDMI_DBG_PRINT("           0\t--tmds prior(argv2 = 0-Auto, 1-TMDS, 2-FRL).\n"
                   "           1\t--min rate prior(argv2 = 0-false, 1-true).\n"
                   "           2\t--tpll enable/disable(argv2 = 0-disable, 1-enable).\n"
                   "           3\t--ppll enable/disable(argv2 = 0-disable, 1-enable).\n"
                   "           4\t--force DSC true/false(argv2 = 0-false, 1-true).\n"
                   "           5\t--set FRL rate(argv2 = 0-no FRL, 1-3L3G, 2-3L6G, 3-4L6G, 4-4L8G, 5-4L10, 6-4L12G).\n"
                   "           6\t--force output true/false(argv2 = 0-false, 1-true).\n");
    HDMI_DBG_PRINT("[argv2  ]: Command param.\n");
    HDMI_DBG_PRINT("[example]: echo prior 0 1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[example]: echo prior 1 1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[example]: echo prior 2 1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[example]: echo prior 3 1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[example]: echo prior 4 1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[example]: echo prior 5 1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[example]: echo prior 6 1 > /proc/msp/hdmi0 \n");
}

static hi_s32 debug_prior(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 argv1, argv2;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;
    struct hdmi_controller *controller = hdmi->ctrl;

    if (controller == NULL) {
        HDMI_DBG_ERR("The pointer is NULL!\n");
        return -EINVAL;
    }

    if (argc != 3) { /* argc != 3 error */
        HDMI_DBG_ERR("error argcs!\n");
        goto usage_error;
    }

    argv1 = simple_strtol(argv[1], NULL, STRING_TO_DEC);
    argv2 = simple_strtol(argv[2], NULL, STRING_TO_DEC); /* 2:cmd second param */

    if (argv1 == CMD_NUM_0) {
        if (debug_tmds_prior(controller, argv2) < 0) {
            goto usage_error;
        }
    } else if (argv1 == CMD_NUM_1) {
        controller->min_rate_prior = argv2 ? true : false;
        HDMI_DBG_PRINT("min rate prior set %s!\n", argv2 ? "true" : "false");
    } else if (argv1 == CMD_NUM_2) {
        controller->tpll_enable = argv2 ? true : false;
        HDMI_DBG_PRINT("tpll %s!\n", argv2 ? "enable" : "disable");
    } else if (argv1 == CMD_NUM_3) {
        controller->ppll_enable = argv2 ? true : false;
        HDMI_DBG_PRINT("ppll %s!\n", argv2 ? "enable" : "disable");
    } else if (argv1 == CMD_NUM_4) {
        controller->dsc_force = argv2 ? true : false;
        HDMI_DBG_PRINT("force dsc = %d\n", !!argv2);
    } else if (argv1 == CMD_NUM_5) {
        argv2 %= 7; /* 7 use for calculation frl rate */
        controller->force_frl_rate = argv2;
        HDMI_DBG_PRINT("force frl rate = %d\n", argv2);
    } else if (argv1 == CMD_NUM_6) {
        controller->force_output = argv2 ? true : false;
        HDMI_DBG_PRINT("force output = %s\n", argv2 ? "true" : "false");
    } else {
        goto usage_error;
    }
    return HI_SUCCESS;

usage_error:
    debug_prior_help();
    return -EFAULT;
}

static hi_void debug_avmute_help(hi_void)
{
    HDMI_DBG_PRINT("[Usage  ]: echo avmute argv1 [argv2] > /proc/msp/hdmi0.\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t-- disable avmute.\n");
    HDMI_DBG_PRINT("           1\t-- enable avmute.\n");
    HDMI_DBG_PRINT("           2\t-- set avmute wait before stop, argv2 means wait time(ms).\n");
    HDMI_DBG_PRINT("           3\t-- set avmute wait after start, argv2 means wait time(ms).\n");
    HDMI_DBG_PRINT("[argv2  ]: time value,value range[0:10000].\n");
    HDMI_DBG_PRINT("[example]: echo avmute 1 > /proc/msp/hdmi0.\n");
    HDMI_DBG_PRINT("           echo avmute 2 200 > /proc/msp/hdmi0.\n");
}

static hi_s32 debug_avmute(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 argv1;
    hi_u32 argv2 = 0;
    hi_s32 ret;
    struct hdmitx_debug_msg msg;
    struct debug_avmute avmute_param;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc < 2) { /* 2: need input 1 param at least */
        goto usage_error;
    }

    argv1 = osal_strtol(argv[1], HI_NULL, STRING_TO_DEC);
    if (argv1 > CMD_NUM_3) {
        goto usage_error;
    }

    if (memset_s(&avmute_param, sizeof(struct debug_avmute), 0, sizeof(struct debug_avmute))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    if (argv1 > CMD_NUM_1) {
        if (argv[2] == HI_NULL) { /* 2: cmd second param */
            goto usage_error;
        } else {
            argv2 = (osal_strtol(argv[2], NULL, STRING_TO_DEC) > AVMUTE_MAX_WAIT_TIME) ? /* 2:cmd second param */
                AVMUTE_MAX_WAIT_TIME : argv2;
        }

        if (argv1 == CMD_NUM_2) {
            avmute_param.wait_bef_stop = argv2;
            HDMI_DBG_PRINT("wait_bef_stop  = %d ms\n", argv2);
        } else {
            avmute_param.wait_aft_start = argv2;
            HDMI_DBG_PRINT("wait_aft_start = %d ms\n", argv2);
        }
    }

    msg.sub_cmd = argv1;
    msg.data = &avmute_param;
    ret = hal_ctrl_debug(hdmi, DEBUG_CMD_AVMUTE, msg);
    if (ret < 0) {
        HDMI_DBG_PRINT("avmute set fail.\n");
        return HI_FAILURE;
    } else {
        HDMI_DBG_PRINT("avmute set succ.\n");
        return HI_SUCCESS;
    }

usage_error:
    debug_avmute_help();

    return HI_FAILURE;
}

static phy_ssc_cfg *debug_phy_ssc_data_get(hi_u32 tmds_clk)
{
    hi_u32 i;
    phy_ssc *ssc = NULL;
    hi_u32 array_size;

    ssc = &g_phy_ssc_cfg_tab[0];
    array_size = ARRAY_SIZE(g_phy_ssc_cfg_tab);
    for (i = 0; (ssc && (i < array_size)); ssc++, i++) {
        if ((tmds_clk >= ssc->range.min) && (tmds_clk <= ssc->range.max)) {
            return (&ssc->cfg);
        }
    }

    return NULL;
}

static hi_s32 debug_emi_set(struct hisilicon_hdmi *hdmi, bool emi_en)
{
    phy_ssc_cfg *ssc = NULL;
    struct phy_ssc phy_ssc;
    struct hdmi_controller *controller = hdmi->ctrl;
    struct hdmi_timing_data *timing_data = NULL;
    hi_u32 clk;
    hi_u32 pixel_clk;

    if (controller == NULL) {
        HDMI_DBG_ERR("The pointer is NULL!\n");
        return -EINVAL;
    }

    timing_data = &controller->mode.timing_data;
    if (timing_data == NULL) {
        HDMI_DBG_ERR("The pointer is NULL!\n");
        return -EINVAL;
    }

    clk = controller->cur_hw_config.tmds_clock;
    if (clk > MAX_ALLOW_EMI_TMDS_CLK) {
        emi_en = false;
    }

    ssc = debug_phy_ssc_data_get(hdmi->ctrl->cur_hw_config.tmds_clock);
    if (ssc == NULL) {
        HDMI_DBG_ERR("enable emi fail,get ssc data fail!\n");
        return -EFAULT;
    }

    pixel_clk = timing_data->in.pixel_clock;
    if (timing_data->out.color.color_format == HDMI_COLOR_FORMAT_YCBCR420) {
        pixel_clk = pixel_clk >> 1;
    }

    phy_ssc.enable = emi_en;
    phy_ssc.pixel_clk = timing_data->in.pixel_clock;
    phy_ssc.color_depth = timing_data->out.color_depth;
    phy_ssc.tmds_clk = clk;
    phy_ssc.ssc_freq = ssc->freq; /* kHz */
    phy_ssc.ssc_amp = ssc->amptd;

    hal_phy_configure_ssc(hdmi->phy, &phy_ssc);

    return HI_SUCCESS;
}

static hi_s32 debug_emi(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 argv1;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;
    hi_s32 ret;

    if (argc != 2) { /* argc != 2 error */
        HDMI_DBG_ERR("error argcs!\n");
        goto usage_error;
    }

    argv1 = simple_strtol(argv[1], NULL, STRING_TO_DEC);
    switch (argv1) {
        case CMD_NUM_0:
            ret = debug_emi_set(hdmi, false);
            HDMI_DBG_PRINT("disable emi succ.\n");
            break;
        case CMD_NUM_1:
            ret = debug_emi_set(hdmi, true);
            HDMI_DBG_PRINT("enable emi succ.\n");
            break;
        default:
            goto usage_error;
    }

    return ret;

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo emi argv1 > /proc/msp/hdmi0\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--emi off.\n"
                   "           1\t--emi on.\n");
    HDMI_DBG_PRINT("[example]: echo emi 1 > /proc/msp/hdmi0\n");
    return -EFAULT;
}

static hi_s32 debug_cec_set_device_type(struct hisilicon_hdmi *hdmi,
    hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_u8 val;

    if (argc != (DBG_CEC_CMD_HEADER_LEN + 1)) { /* argc except the command header is 1 */
        goto err_usage;
    }
    val = osal_strtol(argv[DBG_CEC_CMD_DATA_OFFSET], HI_NULL, OSAL_BASE_DEC);
    return drv_hdmitx_cec_dbg(hdmi->cec, CEC_DBG_CMD_SET_DEVICE_TYPE, &val);

err_usage:
    HDMI_DBG_PRINT("[Usage  ]: echo cec 2 [3-tuner|4-playback] > /proc/msp/hdmitx \n");
    HDMI_DBG_PRINT("[example]: echo cec 2 4 > /proc/msp/hdmitx(playback) \n");
    return HI_FAILURE;
}

static hi_s32 debug_cec_send_msg(struct hisilicon_hdmi *hdmi,
    hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    cec_msg msg = {};
    hi_u32 i;

    if ((argc < (DBG_CEC_CMD_HEADER_LEN + CEC_MIN_MSG_SIZE)) ||
        (argc > (DBG_CEC_CMD_HEADER_LEN + CEC_MAX_MSG_SIZE))) {
        goto err_usage;
    }

    msg.len = argc - DBG_CEC_CMD_HEADER_LEN;
    for (i = 0; i < msg.len; i++) {
        msg.data[i] = osal_strtol(argv[i + DBG_CEC_CMD_HEADER_LEN], HI_NULL, OSAL_BASE_HEX);
    }

    return drv_hdmitx_cec_dbg(hdmi->cec, CEC_DBG_CMD_SEND_MSGS, &msg);

err_usage:
    HDMI_DBG_PRINT("[Usage  ]: echo cec 3 data0~data15 > /proc/msp/hdmitx \n");
    HDMI_DBG_PRINT("[example]: echo cec 3 4f 84 10 00 04 > /proc/msp/hdmitx(report physical address) \n");
    return HI_FAILURE;
}

static hi_s32 arg2cec_event_data(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], cec_events *event)
{
    hi_u32 i;
    hi_slong val;

    if (argc < (DBG_CEC_CMD_HEADER_LEN + CMD_NUM_2)) {
        return HI_FAILURE;
    }

    val = osal_strtol(argv[DBG_CEC_CMD_DATA_OFFSET], HI_NULL, OSAL_BASE_DEC);
    if (val == 0) { /* 0 means event.status_change */
        if (argc != DBG_CEC_CMD_HEADER_LEN + CMD_NUM_4) {
            return HI_FAILURE;
        }

        event->data.status.logical_addr = osal_strtol(argv[CMD_NUM_3], HI_NULL, OSAL_BASE_HEX);
        event->data.status.physical_addr = osal_strtol(argv[CMD_NUM_4], HI_NULL, OSAL_BASE_HEX);
        event->data.status.logic_addr_status = osal_strtol(argv[CMD_NUM_5], HI_NULL, OSAL_BASE_DEC);
        event->type = CEC_EVENT_STATUS_CHANGE;
    } else if (val == 1) { /* 1 means event.status_change */
        if ((argc < (DBG_CEC_CMD_HEADER_LEN + CMD_NUM_1 + CEC_MIN_MSG_SIZE)) ||
            (argc > (DBG_CEC_CMD_HEADER_LEN + CMD_NUM_1 + CEC_MAX_MSG_SIZE))) {
            return HI_FAILURE;
        }

        event->data.rx_msg.len = argc - DBG_CEC_CMD_HEADER_LEN - CMD_NUM_1;
        for (i = 0; i < event->data.rx_msg.len; i++) {
            event->data.rx_msg.data[i] =
                osal_strtol(argv[i + DBG_CEC_CMD_HEADER_LEN + CMD_NUM_1], HI_NULL, OSAL_BASE_HEX);
        }
        event->type = CEC_EVENT_RECEIVED;
    }
    return HI_SUCCESS;
}
static hi_s32 debug_cec_generate_event(struct hisilicon_hdmi *hdmi,
    hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    cec_events event = {};

    if (arg2cec_event_data(argc, argv, &event)) {
        goto err_usage;
    }
    return drv_hdmitx_cec_dbg(hdmi->cec, CEC_DBG_CMD_GENERATE_EVENTS, &event);

err_usage:
    HDMI_DBG_PRINT("[Usage  ]: echo cec 6 0 logic_addr physic_addr addr_status > /proc/msp/hdmitx \n");
    HDMI_DBG_PRINT("[Usage  ]: echo cec 6 1 message.data0~data15 > /proc/msp/hdmitx \n");
    HDMI_DBG_PRINT("[example]: echo cec 6 0 4 ffff 0 > /proc/msp/hdmitx \n");
    HDMI_DBG_PRINT("[example]: echo cec 6 1 04 83 > /proc/msp/hdmitx \n");
    return HI_FAILURE;
}

static hi_s32 debug_cec_wakeup_audio_system(struct hisilicon_hdmi *hdmi,
    hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_bool enable;

    if (argc != (DBG_CEC_CMD_HEADER_LEN + 1)) { /* argc except the command header is 1 */
        goto err_usage;
    }
    enable = osal_strtol(argv[DBG_CEC_CMD_DATA_OFFSET], HI_NULL, OSAL_BASE_DEC);
    return drv_hdmitx_cec_dbg(hdmi->cec, CEC_DBG_CMD_WAKEUP_AUDIO_SYSTEM, &enable);

err_usage:
    HDMI_DBG_PRINT("[Usage  ]: echo cec 7 [0-disable|1-enbale] > /proc/msp/hdmitx \n");
    HDMI_DBG_PRINT("[example]: echo cec 7 1 > /proc/msp/hdmitx\n");
    return HI_FAILURE;
}

static hi_s32 debug_cec(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    enum cec_debug_cmd cmd;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if ((hdmi == HI_NULL) || (hdmi->cec == HI_NULL)) {
        HDMI_ERR("null ptr!\n");
        return HI_FAILURE;
    }

    if (argc < DBG_CEC_CMD_HEADER_LEN) {
        goto err_usage;
    }

    cmd = osal_strtol(argv[CMD_NUM_1], HI_NULL, OSAL_BASE_DEC);
    switch (cmd) {
        case CEC_DBG_CMD_OPEN:
        case CEC_DBG_CMD_CLOSE:
        case CEC_DBG_CMD_SHOW_UNHANDLE_MSGS:
        case CEC_DBG_CMD_SHOW_HISTORY:
            ret = drv_hdmitx_cec_dbg(hdmi->cec, cmd, HI_NULL);
            break;
        case CEC_DBG_CMD_SET_DEVICE_TYPE:
            ret = debug_cec_set_device_type(hdmi, argc, argv);
            break;
        case CEC_DBG_CMD_SEND_MSGS:
            ret = debug_cec_send_msg(hdmi, argc, argv);
            break;
        case CEC_DBG_CMD_GENERATE_EVENTS:
            ret = debug_cec_generate_event(hdmi, argc, argv);
            break;
        case CEC_DBG_CMD_WAKEUP_AUDIO_SYSTEM:
            ret = debug_cec_wakeup_audio_system(hdmi, argc, argv);
            break;
        default:
            goto err_usage;
            break;
    }

    return HI_SUCCESS;

err_usage:
    HDMI_DBG_PRINT("[Usage  ]: echo cec argv1 [argv...]> /proc/msp/hdmitx \n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--open.\n"
                  "           1\t--close.\n"
                  "           2\t--set device type.\n"
                  "           3\t--send message\n"
                  "           4\t--show unhandle messages.\n"
                  "           5\t--show message history.\n"
                  "           6\t--generate events.\n"
                  "           7\t--wakeup audio system in MCU.\n");
    HDMI_DBG_PRINT("[example]: echo cec 0 > /proc/msp/hdmitx \n");
    return HI_SUCCESS;
}

static hi_void debug_mask_hpd(struct hdmi_connector *connector, hi_u8 mask_en)
{
    if (mask_en) {
        connector->poll_hpd = HI_FALSE;
        HDMI_DBG_PRINT("mask hpd event succ.\n");
    } else {
        connector->poll_hpd = HI_TRUE;
        HDMI_DBG_PRINT("unmask hpd event succ.\n");
    }
}

static hi_s32 debug_mask(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u8 sub_cmd, mask_en;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;
    struct hdmi_connector *connector = hdmi->connector;

    if (connector == NULL) {
        HDMI_DBG_ERR("The pointer is NULL!\n");
        return -EINVAL;
    }

    if (argv[2] == NULL) { /* 2:cmd second param */
        HDMI_DBG_ERR("error argcs!\n");
        goto usage_error;
    }

    sub_cmd = simple_strtol(argv[1], NULL, STRING_TO_DEC);
    mask_en = simple_strtol(argv[2], NULL, STRING_TO_DEC); /* 2:cmd second param */
    if (mask_en > 1) {
        HDMI_DBG_ERR("error argcs!\n");
        goto usage_error;
    }

    switch (sub_cmd) {
        case CMD_NUM_0:
            debug_mask_hpd(connector, mask_en);
            break;
        case CMD_NUM_1:
            debug_mask_hpd(connector, mask_en);
            break;
        case CMD_NUM_2:
            break;
        case CMD_NUM_3:
            break;
        case CMD_NUM_4:
            break;
        default:
            goto usage_error;
    }

    return HI_SUCCESS;

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo mask argv1 argv2 > /proc/msp/hdmitx0 \n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--mask all event.\n");
    HDMI_DBG_PRINT("           1\t--mask hpd event.\n");
    HDMI_DBG_PRINT("           2\t--mask hdcp event.\n");
    HDMI_DBG_PRINT("           3\t--mask rsen event.\n");
    HDMI_DBG_PRINT("           4\t--mask train event.\n");
    HDMI_DBG_PRINT("[argv2  ]: 0\t--unmask hpd event.\n");
    HDMI_DBG_PRINT("           1\t--mask hdcp event.\n");
    HDMI_DBG_PRINT("[example]: echo mask 1 0 > /proc/msp/hdmitx0 \n");
    return HI_FAILURE;
}

static hi_s32 debug_cbar(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u8 argv1;
    hi_s32 ret;
    struct hdmitx_debug_msg msg;
    hi_bool cbar_en = HI_FALSE;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc != 2) { /* 2: need input 1 param */
        HDMI_DBG_ERR("error argcs!\n");
        goto usage_error;
    }

    if (memset_s(&msg, sizeof(struct hdmitx_debug_msg), 0, sizeof(struct hdmitx_debug_msg))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    argv1 = osal_strtol(argv[1], NULL, STRING_TO_DEC);
    if (argv1 > CMD_NUM_1) {
        goto usage_error;
    }

    if (argv1 == CMD_NUM_0) {
        cbar_en = HI_FALSE;
    } else {
        cbar_en = HI_TRUE;
    }

    msg.sub_cmd = argv1;
    msg.data = &cbar_en;
    ret = hal_ctrl_debug(hdmi, DEBUG_CMD_CBAR, msg);
    if (ret < 0) {
        HDMI_DBG_PRINT("cbar set fail.\n");
        return HI_FAILURE;
    } else {
        HDMI_DBG_PRINT("cbar set succ.\n");
        return HI_SUCCESS;
    }

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo cbar argv1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--colorbar off.\n");
    HDMI_DBG_PRINT("           1\t--colorbar on.\n");
    HDMI_DBG_PRINT("[example]: echo cbar 1 > /proc/msp/hdmi0 \n");

    return HI_FAILURE;
}

static hi_void scdc_ret_info(hi_u8 argv1, hi_s32 ret)
{
    if (argv1 == CMD_NUM_2) {
        if (ret > 0) {
            HDMI_DBG_PRINT("sdcd is on.\n");
        } else if (ret == 0) {
            HDMI_DBG_PRINT("sdcd is off.\n");
        } else {
            HDMI_DBG_PRINT("scdc get fail.\n");
        }
    } else {
        if (ret < 0) {
            HDMI_DBG_PRINT("sdcd set fail.\n");
        } else {
            HDMI_DBG_PRINT("sdcd set succ.\n");
        }
    }
}

static hi_s32 debug_scdc(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u8 argv1;
    hi_s32 ret;
    struct hdmitx_debug_msg msg;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;
    struct hdmi_controller *ctrl = hdmi->ctrl;

    if (ctrl == HI_NULL) {
        HDMI_DBG_ERR("The pointer is NULL!\n");
        return HI_FAILURE;
    }

    if (argc != 2) { /* 2: need input 1 param */
        goto usage_error;
    }

    argv1 = osal_strtol(argv[1], NULL, STRING_TO_DEC);
    switch (argv1) {
        case CMD_NUM_0:
            ctrl->cur_hw_config.tmds_scr_en = HI_FALSE;
            msg.data = ctrl;
            break;
        case CMD_NUM_1:
            if (ctrl->cur_hw_config.work_mode != HDMI_WORK_MODE_TMDS) {
                HDMI_DBG_PRINT("work mode not TMDS, cmd not support.");
                return HI_FAILURE;
            } else {
                ctrl->cur_hw_config.tmds_scr_en = HI_TRUE;
                msg.data = ctrl;
            }
            break;
        case CMD_NUM_2:
            msg.data = &argv1;
            break;
        default :
            goto usage_error;
    }

    msg.sub_cmd = argv1;
    ret = hal_ctrl_debug(hdmi, DEBUG_CMD_SCDC, msg);
    scdc_ret_info(argv1, ret);
    if (ret < 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo scdc argv1 > /proc/msp/hdmi0 \n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--scdc off.\n");
    HDMI_DBG_PRINT("           1\t--scdc on.\n");
    HDMI_DBG_PRINT("           2\t--scdc status get.\n");
    HDMI_DBG_PRINT("[example]: echo scdc 1 > /proc/msp/hdmi0 \n");
    return HI_FAILURE;
}

static hi_s32 debug_hdmi_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u8 argv1;
    hi_s32 ret;
    struct hdmitx_debug_msg msg;
    hi_bool is_hdmi_mode = HI_FALSE;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc != 2) { /* 2: need input 1 param */
        goto usage_error;
    }

    argv1 = simple_strtol(argv[1], NULL, STRING_TO_DEC);
    switch (argv1) {
        case CMD_NUM_0:
            is_hdmi_mode = HI_FALSE;
            break;
        case CMD_NUM_1:
            is_hdmi_mode = HI_TRUE;
            break;
        default :
            goto usage_error;
    }

    msg.sub_cmd = argv1;
    msg.data = &is_hdmi_mode;
    ret = hal_ctrl_debug(hdmi, DEBUG_CMD_HDMI_MODE, msg);
    if (ret < 0) {
        HDMI_DBG_PRINT("hdmi mode set fail.\n");
        return HI_FAILURE;
    } else {
        HDMI_DBG_PRINT("hdmi mode set succ.\n");
        return HI_SUCCESS;
    }

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo hdmimode argv1 > /proc/msp/hdmitx0 \n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--DVI.\n");
    HDMI_DBG_PRINT("           1\t--HDMI.\n");
    HDMI_DBG_PRINT("[example]: echo hdmimode 1 > /proc/msp/hdmitx0 \n");

    return HI_FAILURE;
}

static hi_s32 debug_rc(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_s32 ret;
    hi_u32 rc_val;
    struct hdmitx_debug_msg msg;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc != 2) { /* argc != 2 is error */
        goto usage_error;
    }

    if (memset_s(&msg, sizeof(struct hdmitx_debug_msg), 0, sizeof(struct hdmitx_debug_msg))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    rc_val = osal_strtol(argv[1], HI_NULL, STRING_TO_DEC);
    if (rc_val > CMD_NUM_3) {
        goto usage_error;
    }

    msg.data = &rc_val;
    ret = hal_ctrl_debug(hdmi, DEBUG_CMD_RC, msg);
    if (ret < 0) {
        HDMI_DBG_PRINT("rc set %d fail!\n", rc_val);
        return HI_FAILURE;
    } else {
        HDMI_DBG_PRINT("rc set %d success!\n", rc_val);
        return HI_SUCCESS;
    }

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo rc argv1 > /proc/msp/hdmitx0\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--rc compression, not support hdcp.\n"
                   "           1\t--Logic control code pattern.\n"
                   "           2\t--Logic control code pattern.\n"
                   "           3\t--rc uncompression.\n");
    HDMI_DBG_PRINT("[example]: echo rc 3 > /proc/msp/hdmitx0\n");

    return HI_FAILURE;
}

static hi_s32 debug_de(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_s32 ret;
    hi_u32 sub_cmd;
    hi_u8 period = 0;
    struct hdmitx_debug_msg msg;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc < CMD_NUM_2) {
        goto usage_error;
    }

    sub_cmd = osal_strtol(argv[1], NULL, STRING_TO_DEC);
    if (sub_cmd > CMD_NUM_1) {
        goto usage_error;
    }

    if (sub_cmd == CMD_NUM_1) {
        if (argc != CMD_NUM_3) {
            goto usage_error;
        } else {
            period = osal_strtol(argv[2], NULL, STRING_TO_DEC); /* 2:cmd second param */
        }
    }

    msg.sub_cmd = sub_cmd;
    msg.data = &period;
    ret = hal_ctrl_debug(hdmi, DEBUG_CMD_DE, msg);
    if (ret < 0) {
        HDMI_DBG_PRINT("de set fail!\n");
        return HI_FAILURE;
    } else {
        if (sub_cmd == CMD_NUM_0) {
            HDMI_DBG_PRINT("de set 0 success!\n");
        } else {
            HDMI_DBG_PRINT("de set 1 succ, %d periods to be undeliverable!\n", period);
        }
        return HI_SUCCESS;
    }

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo de argv1 [argv2] > /proc/msp/hdmitx0\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--de disable.\n"
                   "           1\t--de enable.\n");
    HDMI_DBG_PRINT("[argv2  ]: [0-59]\t--the number of undeliverable periods.\n");
    HDMI_DBG_PRINT("[example]: echo de 0 > /proc/msp/hdmitx0 \n");
    HDMI_DBG_PRINT("           echo de 1 59 > /proc/msp/hdmitx0\n");

    return HI_FAILURE;
}

static hi_void debug_event_help(hi_void)
{
    hi_u8 i;

    HDMI_DBG_PRINT("[Usage  ]: echo event argv1 > /proc/msp/hdmitx0\n");
    HDMI_DBG_PRINT("[argv1  ]: event type as follow\n");
    for (i = 0; i < CMD_NUM_7; i++) {
        HDMI_DBG_PRINT("\t%4d -- %s\n", i, g_event_str_table[i]);
    }
    HDMI_DBG_PRINT("[example]: echo event 1 > /proc/msp/hdmitx0\n");
}

static hi_s32 debug_event(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u8 sub_cmd;
    hi_char event_str[20] = {0}; /* 20: hdmi event len */
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc != 2) { /* 2: need input 1 param */
        goto usage_error;
    }

    sub_cmd = osal_strtol(argv[1], NULL, STRING_TO_DEC);
    if (sub_cmd > CMD_NUM_7) {
        goto usage_error;
    }

    /* Set event */
    switch (sub_cmd) {
        case CMD_NUM_0:
            if (snprintf_s(event_str, sizeof(event_str), sizeof(event_str) - 1, "HOTPLUG=1 HDMI=%d",
                           hdmi->id % (DISP_INFT_HDMI1 + 1)) == HI_FAILURE) {
                HDMI_ERR("snprintf_s fail.\n");
                return HI_FAILURE;
            }
            break;
        case CMD_NUM_1:
            if (snprintf_s(event_str, sizeof(event_str), sizeof(event_str) - 1, "HOTPLUG=2 HDMI=%d",
                           hdmi->id % (DISP_INFT_HDMI1 + 1)) == HI_FAILURE) {
                HDMI_ERR("snprintf_s fail.\n");
                return HI_FAILURE;
            }
            break;
        case CMD_NUM_2:
        case CMD_NUM_3:
        case CMD_NUM_4:
        case CMD_NUM_5:
        case CMD_NUM_6:
        case CMD_NUM_7:
            break;
        default:
            goto usage_error;
    }

    if (strlen(event_str) != 0) {
        hdmi_sysfs_event(hdmi, event_str, 20); /* 20: hdmi event len */
    } else {
        HDMI_DBG_PRINT("event not support now!\n");
    }

    return HI_SUCCESS;

usage_error:
    debug_event_help();

    return HI_FAILURE;
}

static hi_s32 deug_valid_mode_show(struct hdmi_valid_mode *valid_mode, struct hdmi_timing_mode *timing)
{
    static hi_u16 pre_vic = -1;
    static hi_u8 pre_fmt = -1;
    hi_char timing_info[25] = {0}; /* timing format max len is 25 */

    if (valid_mode->band.vic != pre_vic) {
        HDMI_DBG_PRINT("\n");
        if (snprintf_s(timing_info, sizeof(timing_info), sizeof(timing_info) - 1, "[%03u]%uX%u%s_%u",
                       timing->vic, timing->h_active, timing->v_active,
                       timing->progressive ? "p" : "i", timing->field_rate) == HI_FAILURE) {
            HDMI_ERR("snprintf_s fail.\n");
            return HI_FAILURE;
        }
        HDMI_DBG_PRINT("%-20s ", timing_info);
        pre_fmt = -1;
    }

    if (valid_mode->band.color_format != pre_fmt) {
        if (snprintf_s(timing_info, sizeof(timing_info), sizeof(timing_info) - 1, "   %s/%s",
                       g_color_fmt_str[valid_mode->band.color_format],
                       g_color_deepth_str[valid_mode->band.color_depth]) == HI_FAILURE) {
            HDMI_ERR("snprintf_s fail.\n");
            return HI_FAILURE;
        }
    } else {
        if (snprintf_s(timing_info, sizeof(timing_info), sizeof(timing_info) - 1, "/%s",
                       g_color_deepth_str[valid_mode->band.color_depth]) == HI_FAILURE) {
            HDMI_ERR("snprintf_s fail.\n");
            return HI_FAILURE;
        }
    }
    HDMI_DBG_PRINT("%s", timing_info);
    pre_fmt = valid_mode->band.color_format;
    pre_vic = valid_mode->band.vic;

    return HI_SUCCESS;
}

static hi_s32 debug_valid_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    struct osal_list_head *next = HI_NULL;
    struct osal_list_head *pos = HI_NULL;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;
    struct hdmi_connector *connector = hdmi->connector;
    struct hdmi_valid_mode *valid_mode = HI_NULL;
    struct hdmi_timing_mode *timing = HI_NULL;

    if (argc != 1) { /* 1: don't need any param */
        goto usage_error;
    }

    if (!osal_list_empty(&connector->valid_modes)) {
        osal_list_for_each_safe(pos, next, &connector->valid_modes) {
            valid_mode = osal_list_entry(pos, struct hdmi_valid_mode, head);
            if (valid_mode == HI_NULL) {
                continue;
            }

            timing = vic2timing_mode(valid_mode->band.vic);
            if (timing == NULL) {
                continue;
            }

            if (deug_valid_mode_show(valid_mode, timing) == HI_FAILURE) {
                return HI_FAILURE;
            }
        }
    }
    HDMI_DBG_PRINT("\n");
    return HI_SUCCESS;

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo validmode > /proc/msp/hdmitx0\n");
    HDMI_DBG_PRINT("[argv1  ]: none.\n");
    HDMI_DBG_PRINT("[example]: echo validmode > /proc/msp/hdmitx0\n");

    return HI_FAILURE;
}

static hi_s32 debug_null_packet(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u8 null_packet_en;
    hi_s32 ret;
    struct hdmitx_debug_msg msg;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc != 2) { /* 2: need input 1 param */
        goto usage_error;
    }

    if (memset_s(&msg, sizeof(struct hdmitx_debug_msg), 0, sizeof(struct hdmitx_debug_msg))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    null_packet_en = osal_strtol(argv[1], NULL, STRING_TO_DEC);
    if (null_packet_en > CMD_NUM_1) {
        goto usage_error;
    }

    msg.data = &null_packet_en;
    ret = hal_ctrl_debug(hdmi, DEBUG_CMD_NULL_PACKET, msg);
    if (ret < 0) {
        HDMI_DBG_PRINT("null packet set fail.\n");
        return HI_FAILURE;
    } else {
        HDMI_DBG_PRINT("null packet set succ.\n");
        return HI_SUCCESS;
    }

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo nullpacket argv1 > /proc/msp/hdmitx0\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--disable null packet send.\n");
    HDMI_DBG_PRINT("           1\t--enable null packet send.\n");
    HDMI_DBG_PRINT("[example]: echo nullpacket 0 > /proc/msp/hdmitx0\n");

    return HI_FAILURE;
}

static hi_s32 debug_frl_fifo(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u8 sub_cmd;
    hi_s32 ret;
    hi_u32 fifo_val;
    struct hdmitx_debug_msg msg;
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    if (argc != 3) { /* 3: need input 2 param */
        goto usage_error;
    }

    sub_cmd = osal_strtol(argv[1], HI_NULL, STRING_TO_DEC);
    if (sub_cmd > CMD_NUM_1) {
        goto usage_error;
    }

    fifo_val = osal_strtol(argv[2], HI_NULL, STRING_TO_DEC); /* 2: cmd second param */
    if (fifo_val > 65535) { /* 65535: Maximum of frl fifo threshold */
        goto usage_error;
    }

    msg.sub_cmd = sub_cmd;
    msg.data = &fifo_val;
    ret = hal_frl_debug(hdmi, DEBUG_CMD_FIFO_THRESHOLD, msg);
    if (ret < 0) {
        HDMI_DBG_PRINT("frl %s fifo threshold set fail.\n", g_frl_fifo_type[sub_cmd]);
        return HI_FAILURE;
    } else {
        HDMI_DBG_PRINT("frl %s fifo threshold set succ.\n", g_frl_fifo_type[sub_cmd]);
        return HI_SUCCESS;
    }

usage_error:
    HDMI_DBG_PRINT("[Usage  ]: echo frlfifo argv1 argv2 > /proc/msp/hdmitx0\n");
    HDMI_DBG_PRINT("[argv1  ]: 0\t--frl fifo up threshold.\n");
    HDMI_DBG_PRINT("           1\t--frl fifo down threshold.\n");
    HDMI_DBG_PRINT("[argv2  ]: val to set,[0-65535].\n");
    HDMI_DBG_PRINT("[example]: echo frlfifo 0 1 > /proc/msp/hdmitx0\n");

    return HI_FAILURE;
}

static hi_s32 debug_print_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);

static osal_proc_cmd g_cmd_table[] = {
    { "help",       debug_print_help },
    { "oe",         debug_oe },
    { "edid",       debug_edid },
    { "avmute",     debug_avmute },
    { "prior",      debug_prior },
    { "emi",        debug_emi },
    { "cec",        debug_cec },
    { "mask",       debug_mask },
    { "cbar",       debug_cbar },
    { "scdc",       debug_scdc },
    { "hdmimode",   debug_hdmi_mode },
    { "rc",         debug_rc },
    { "de",         debug_de },
    { "event",      debug_event },
    { "validmode",  debug_valid_mode },
    { "nullpacket", debug_null_packet },
    { "frlfifo",    debug_frl_fifo },
};

static cmd_msg g_cmd_list = {
    .cmd_list = g_cmd_table,
    .cmd_cnt = sizeof(g_cmd_table) / sizeof(osal_proc_cmd),
};

static const hi_u8 *g_cmd_comment_help[] = {
    "print these help info.",
    "phyoutput enable(oe) set on/off.",
    "set edid.",
    "set avmute wait time",
    "select prior and enable [tpll/ppll].",
    "emi set on/off.",
    "cec debug command.",
    "mask event reporting(hpd/hdcp/rsen/train).",
    "cbar set on/off.",
    "scdc set on/off.",
    "tmds mode set dvi/hdmi.",
    "Configuring rc compression.",
    "Configuring de.",
    "hardware event simulate",
    "get current valid mode.",
    "set null packet send.",
    "set frl fifo threshold.",
};

static hi_s32 debug_print_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 i;

    for (i = 0; i < ARRAY_SIZE(g_cmd_table); i++) {
        if (g_cmd_table[i].name != NULL) {
            HDMI_DBG_PRINT("    %02d. %-20s", i, g_cmd_table[i].name);
            HDMI_DBG_PRINT("    --%s\n", g_cmd_comment_help[i]);
        } else {
            HDMI_DBG_ERR("HDMI_DBG[%d].HDMI_DBG or name is null!\n", i);
        }
    }

    HDMI_DBG_PRINT("\n");
    HDMI_DBG_PRINT("    You can use 'echo cmd argv... > /proc/msp/hdmi0'");
    HDMI_DBG_PRINT(" execute a debug command.\n"
                   "    You can use 'echo cmd > /proc/msp/hdmi0' for more details about every command.\n"
                   "    Here,'cmd' refers to NO./complete-name/short-name above.\n");
    return HI_SUCCESS;
}

hi_s32 drv_hdmitx_debug_init(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("hdmi is null!\n");
        return HI_FAILURE;
    }

    hdmi->debug_info = osal_kmalloc(HI_ID_HDMITX, sizeof(struct hdmi_debug_info), OSAL_GFP_KERNEL);
    if (hdmi->debug_info == NULL) {
        HDMI_ERR("osal_kmalloc men for hdmi_debug_info failed!\n");
        return HI_FAILURE;
    }

    if (memset_s(hdmi->debug_info, sizeof(struct hdmi_debug_info), 0, sizeof(struct hdmi_debug_info))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, hdmi->debug_info);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void drv_hdmitx_debug_deinit(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("hdmi is null!\n");
        return;
    }

    if (hdmi->debug_info != NULL) {
        osal_kfree(HI_ID_HDMITX, hdmi->debug_info);
        hdmi->debug_info = NULL;
    }

    return;
}
cmd_msg *drv_hdmitx_debug_get_cmd_list(hi_void)
{
    return &g_cmd_list;
}
