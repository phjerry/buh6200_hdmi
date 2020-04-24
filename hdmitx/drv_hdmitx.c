/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver main source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/of.h>
#include <linux/hisilicon/securec.h>

#include "hi_drv_module.h"
#include "drv_hdmitx_ioctl.h"
#include "drv_disp_ext.h"
#include "drv_hdmi_module_ext.h"

#include "drv_hdmitx.h"
#include "drv_hdmitx_modes.h"
#include "drv_hdmitx_proc.h"
#include "drv_hdmitx_debug.h"
#include "drv_hdmitx_hdcp.h"
#include "drv_hdmitx_connector.h"
#include "drv_hdmitx_edid.h"
#include "hal_hdmitx_io.h"
#include "drv_hdmitx_infoframe.h"
#include "drv_hdmitx_dsc.h"
#include "drv_hdmitx_ddc.h"
#include "hal_hdmitx_dsc.h"
#include "hal_hdmitx_ddc.h"
#include "hal_hdmitx_crg.h"
#include "hal_hdmitx_ctrl.h"
#include "hal_hdmitx_phy.h"
#include "hal_hdmitx_frl.h"
#include "drv_hdmitx_sysfs.h"
#include "drv_hdmitx_cec.h"

/* PERI_HDMITX_CTRL define */
#define PERI_HDMITX_CTRL_ADDR               0xa10d80
#define PERI_HDMITX_CTRL2P0_TO_PHY2P0_MASK  (0x1 << 0)

#define HDMITX_CEC_REGS_OFFSET         0xc00
/* ctrl and phy map_mode define */
#define CRLT_PHY_MAP_MODE_NO_CROSS     0  /* no cross */
#define CRLT_PHY_MAP_MODE_CROSS        1  /* cross */

#define HDMITX_PHY0_REG_BASE 0x0103f000
#define HDMITX_PHY0_TPLL_REG_BASE 0x00a000c0
#define HDMITX_PHY1_REG_BASE 0x0107f000
#define HDMITX_PHY1_TPLL_REG_BASE 0x00a000cc
#define HDMITX_PHY_REG_SIZE 0x1000
#define HDMITX_PHY_TPLL_REG_SIZE 0xc

/* Aspect ratio width&height define */
#define WIDTH_4      4
#define HEIGHT_3     3
#define WIDTH_16     16
#define HEIGHT_9     9
#define WIDTH_64     64
#define HEIGHT_27    27
#define WIDTH_256    256
#define HEIGHT_135   135

/* Index define */
#define INDEX_0      0
#define INDEX_1      1
#define INDEX_2      2

/*
 * when we have more than one hdmi interface, vo/ao will use the id to index the
 * specified hdmi interface, so there is an table to maintain the mapping.
 *
 */
#define MAX_HDMI_ID 0x8

#define HDMI0_MAX_TMDS_CLOCK_RATE 600000
struct hdmi_table {
    hi_s32 hdmi_id;
    struct hisilicon_hdmi *hdmi;
};

static hi_s32 g_hdmi_cnt;
static struct hdmi_table g_mapping[MAX_HDMI_ID];

#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX "hdmi."

static void hdmi_hw_ctrl_phy_mapping_set(hi_u32 map_mode)
{
    void *crg_reg = NULL;
    hi_u32 crg_addr;

    crg_addr = PERI_HDMITX_CTRL_ADDR;

    crg_reg = osal_ioremap_nocache(crg_addr, 4); /* the addr size is 4. */
    if (IS_ERR(crg_reg)) {
        HDMI_ERR("Failed to get memory resource\n");
        return;
    }

    if (map_mode == CRLT_PHY_MAP_MODE_NO_CROSS) {
        hdmi_set(crg_reg, 0,
            PERI_HDMITX_CTRL2P0_TO_PHY2P0_MASK);
    } else {
        hdmi_clr(crg_reg, 0,
            PERI_HDMITX_CTRL2P0_TO_PHY2P0_MASK);
    }

    osal_iounmap(crg_reg);
}

/* define notifier_call_chain */
hi_void hdmi_ao_notifiers(struct hisilicon_hdmi *hdmi, hi_u32 val)
{
    hi_s32 ret;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    ret = raw_notifier_call_chain(&hdmi->notifier_list, val, NULL);
    if (ret) {
        HDMI_ERR("Ao notify fail.\n");
        return;
    }

    return;
}

static hi_void hdmi_clk_enable(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hdmi_crg_clk_enable(hdmi->crg, enable);

    return;
}

hi_void hdmi_phy_on(struct hisilicon_hdmi *hdmi)
{
#ifndef HI_FPGA_SUPPORT
    hal_phy_on(hdmi->phy);
#endif
}

hi_void hdmi_phy_off(struct hisilicon_hdmi *hdmi)
{
#ifndef HI_FPGA_SUPPORT
    hal_phy_off(hdmi->phy);
#endif
}

hi_void hdmi_crg_set(struct hisilicon_hdmi *hdmi)
{
#ifndef HI_FPGA_SUPPORT
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hi_display_mode *mode = &ctrl->mode;
    struct hdmi_hw_config *config = &ctrl->cur_hw_config;
    hi_u32 color_format = mode->timing_data.out.color.color_format;
    hi_u32 color_depth = mode->timing_data.out.color_depth;
    hi_u32 pixel_clk = mode->timing_data.in.pixel_clock;
    struct crg_config_data cfg;

    if (memset_s(&cfg, sizeof(struct crg_config_data), 0x0, sizeof(struct crg_config_data))) {
        HDMI_ERR("memset_s fail.\n");
        return;
    }

    switch (config->work_mode) {
        case HDMI_WORK_MODE_TMDS:
            cfg.encoder_mode = 0; /* 0: tmds mode. */
            break;
        case HDMI_WORK_MODE_FRL:
            cfg.encoder_mode = 1; /* 1: frl mode. */
            break;
        default:
            break;
    }

    if (color_depth == HDMI_BPC_24) {
        cfg.color_depth = 8; /* color depth is 8bit. */
    } else if (color_depth == HDMI_BPC_30) {
        cfg.color_depth = 10; /* color depth is 10bit. */
    } else if (color_depth == HDMI_BPC_36) {
        cfg.color_depth = 12; /* color depth is 12bit. */
    } else {
        cfg.color_depth = 8; /* the default color depth is 8bit. */
    }

    cfg.dsc_enable = config->dsc_enable;
    cfg.hctotal = config->hcactive + config->hcblank;
    cfg.htotal = mode->timing_data.in.detail.htotal;
    cfg.pixel_rate = pixel_clk;
    cfg.prefer_ppll = ctrl->ppll_enable;
    cfg.yuv_420 = (color_format == HDMI_COLOR_FORMAT_YCBCR420) ?
        true : false;
    hdmi_crg_config(hdmi->crg, &cfg);
#endif
}

static hi_void hdmi_phy_set(struct hisilicon_hdmi *hdmi)
{
#ifndef HI_FPGA_SUPPORT
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hi_display_mode *mode = &ctrl->mode;
    struct hdmi_hw_config *config = &ctrl->cur_hw_config;
    hi_u32 color_format = mode->timing_data.out.color.color_format;
    hi_u32 color_depth = mode->timing_data.out.color_depth;
    hi_u32 pixel_clk = mode->timing_data.in.pixel_clock;
    struct phy_tmds tmds;
    struct phy_frl frl;

    switch (config->work_mode) {
        case HDMI_WORK_MODE_TMDS:
            if (color_format == HDMI_COLOR_FORMAT_YCBCR420) {
                tmds.pixel_clk = pixel_clk / 2; /* pixel clock need divid by 2. */
                tmds.yuv_420 = true;
            } else {
                tmds.pixel_clk = pixel_clk;
                tmds.yuv_420 = false;
            }

            tmds.tmds_clk = config->tmds_clock;
            tmds.color_depth = color_depth;
            tmds.pcb_len = PCB_LEN_1;
            tmds.tpll_enable = ctrl->tpll_enable;
            hal_phy_configure_tmds(hdmi->phy, &tmds);
            break;
        case HDMI_WORK_MODE_FRL:
            frl.frl_rate = config->cur_frl_rate;
            frl.pcb_len = PCB_LEN_1;
            frl.tpll_enable = ctrl->tpll_enable;
            hal_phy_configure_frl(hdmi->phy, &frl);
            break;
        default:
            break;
    }
#endif
    return;
}

hi_s32 hdmi_phy_fcg_set(struct hisilicon_hdmi *hdmi)
{
#ifndef HI_FPGA_SUPPORT
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hi_display_mode *mode = &ctrl->mode;
    struct hdmi_hw_config *config = &ctrl->cur_hw_config;
    hi_u32 color_format = mode->timing_data.out.color.color_format;
    hi_u32 color_depth = mode->timing_data.out.color_depth;
    hi_u32 pixel_clk = mode->timing_data.in.pixel_clock;
    struct phy_frl_tmds_clk frl_tmds_clk;

    if (color_format == HDMI_COLOR_FORMAT_YCBCR420) {
        frl_tmds_clk.pixel_clk =  pixel_clk / 2; /* pixel clock need divid by 2. */
        frl_tmds_clk.yuv_420 = true;
    } else {
        frl_tmds_clk.pixel_clk =  pixel_clk;
        frl_tmds_clk.yuv_420 = false;
    }

    frl_tmds_clk.dsc_enable = config->dsc_enable;
    frl_tmds_clk.hctotal = config->hcactive + config->hcblank;
    frl_tmds_clk.htotal = mode->timing_data.in.detail.htotal;
    frl_tmds_clk.rate = config->cur_frl_rate;
    frl_tmds_clk.color_depth = color_depth;

    hal_phy_configure_frl_tmds_clk(hdmi->phy, &frl_tmds_clk);
#endif
    return 0;
}

static void hdmi_prior_work_mode_select(struct hisilicon_hdmi *hdmi, hi_u32 vic)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;

    /**
     * VIC[108,219] is new define in CEA-861-G
     * which is apply to HDMI2.1, prior frl work mode.
     */
    switch (ctrl->work_mode_prior) {
        case HDMI_PRIOR_FORCE_FRL:
            ctrl->tmds_prior = false;
            break;
        case HDMI_PRIOR_FORCE_TMDS:
            ctrl->tmds_prior = true;
            break;
        case HDMI_PRIOR_AUTIO:
        default:
            ctrl->tmds_prior = (vic > VIC_3840X2160P60_64_27 && vic < 0xff) ?
                false : true;
            break;
    }
}

static hi_void hdmitx_get_target_hw_config(struct hdmi_controller *ctrl,
    struct hdmi_mode_config *mode_cfg, struct hdmi_hw_config *hw_cfg)
{
    if (mode_cfg->tmds_encode && ctrl->tmds_prior) {
        hw_cfg->work_mode = HDMI_WORK_MODE_TMDS;
    } else if (mode_cfg->frl.frl_uncompress || mode_cfg->dsc.frl_compress) {
        hw_cfg->work_mode = HDMI_WORK_MODE_FRL;
    } else {
        HDMI_WARN("no tmds & flr cap,set default tmds mode.\n");
        hw_cfg->work_mode = HDMI_WORK_MODE_TMDS;
    }

    hw_cfg->tmds_scr_en = mode_cfg->tmds_scdc_en;
    hw_cfg->dsc_enable = mode_cfg->dsc.frl_compress &&
                                  (!mode_cfg->frl.frl_uncompress ||
                                  ctrl->dsc_force);

    hw_cfg->dvi_mode =
        (mode_cfg->hdmi_mode == HDMI_MODE_DVI) ? true : false;

    if (mode_cfg->frl.frl_uncompress &&
        mode_cfg->dsc.frl_compress) {
        hw_cfg->cur_frl_rate = mode_cfg->frl.min_frl_rate;
        hw_cfg->max_frl_rate = mode_cfg->frl.max_frl_rate;
        hw_cfg->min_frl_rate = mode_cfg->frl.min_frl_rate;
        hw_cfg->min_dsc_frl_rate = mode_cfg->dsc.min_dsc_frl_rate;
    } else if (!mode_cfg->frl.frl_uncompress &&
               mode_cfg->dsc.frl_compress) {
        hw_cfg->cur_frl_rate = mode_cfg->dsc.min_dsc_frl_rate;
        hw_cfg->max_frl_rate = mode_cfg->dsc.max_dsc_frl_rate;
        hw_cfg->min_frl_rate = mode_cfg->dsc.min_dsc_frl_rate;
        hw_cfg->min_dsc_frl_rate = mode_cfg->dsc.min_dsc_frl_rate;
    } else if (mode_cfg->frl.frl_uncompress &&
               !mode_cfg->dsc.frl_compress) {
        hw_cfg->cur_frl_rate = mode_cfg->frl.min_frl_rate;
        hw_cfg->max_frl_rate = mode_cfg->frl.max_frl_rate;
        hw_cfg->min_frl_rate = mode_cfg->frl.min_frl_rate;
        hw_cfg->min_dsc_frl_rate = mode_cfg->frl.min_frl_rate;
    }

    if (ctrl->force_frl_rate) {
        hw_cfg->cur_frl_rate = ctrl->force_frl_rate;
        hw_cfg->max_frl_rate = ctrl->force_frl_rate;
        hw_cfg->min_frl_rate = ctrl->force_frl_rate;
        hw_cfg->min_dsc_frl_rate = ctrl->force_frl_rate;
    }

    hw_cfg->tmds_clock = mode_cfg->tmds_clock;
    hw_cfg->bpp_target = mode_cfg->dsc.bpp_target;
    hw_cfg->slice_width = mode_cfg->dsc.slice_width;
    hw_cfg->hcactive = mode_cfg->dsc.hcactive;
    hw_cfg->hcblank = mode_cfg->dsc.hcblank;
}

/**
 * When the upper layer wants to set a specified video format. There are usually
 * more than one hw configurations which can support that video format.
 * For example:
 * 4K P30 444 ---> Both TMDS and FRL can be used.
 * 8K P60 420 ---> Both FRL and FRL + DSC can be used.
 *      FRL rate 4*10G 4*12G can be used.
 *
 * So we need to convert the video format to an applicable hw config.
 * The following decisions are made:
 * (1) FRL/TMDS mode.
 * (2) Available FRL rate range.(DSC or noDSC)
 *    based on the FRL rate returned from the FRL training, we can decide DSC or
 *    noDSC.
 *
 */
static hi_s32 hdmi_cur_hw_config_decision(struct hisilicon_hdmi *hdmi)
{
    hi_u32 vic;
    hi_u32 color_format;
    hi_u32 color_depth;
    struct hdmi_hw_config target_hw_config;
    struct hdmi_mode_config mode_config;
    struct hdmi_controller *controller = hdmi->ctrl;
    struct hdmi_hw_config *cur_config = &controller->cur_hw_config;
    struct hdmi_hw_config *pre_config = &controller->pre_hw_config;
    struct hdmi_connector *connector = hdmi->connector;
    struct hdmi_timing_data *timing_data = &controller->mode.timing_data;

    if (memset_s(&mode_config, sizeof(struct hdmi_mode_config), 0x0, sizeof(struct hdmi_mode_config))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }
    if (memset_s(&target_hw_config, sizeof(struct hdmi_hw_config), 0x0, sizeof(struct hdmi_hw_config))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    vic = timing_data->in.vic;
    color_format = timing_data->out.color.color_format;
    color_depth = timing_data->out.color_depth;

    mode_config.band.vic = vic;
    mode_config.band.color_format = color_format;
    mode_config.band.color_depth = color_depth;
    if (!hdmi_connector_search_mode(connector, &mode_config)) {
        HDMI_ERR("hdmi_id = %d, mode(vic=%u,c_fmt=%d,c_depth=%d) isn't avail, please set an avail-mode\n",
                 hdmi->id, mode_config.band.vic, mode_config.band.color_format, mode_config.band.color_depth);
        if (controller->force_output) {
            /* force output mode create. */
            drv_hdmitx_connector_create_force_mode(connector, &mode_config);
        } else {
            return HDMI_ERR_MODE_NOT_AVAIL;
        }
    }

    hdmi_prior_work_mode_select(hdmi, vic);
    HDMI_ERR("tmds_prior=%d,tmds=%d,frl_u=%d,dsc=%d\n", controller->tmds_prior, mode_config.tmds_encode,
        mode_config.frl.frl_uncompress, mode_config.dsc.frl_compress);
    hdmitx_get_target_hw_config(controller, &mode_config, &target_hw_config);

    if (memcpy_s(pre_config, sizeof(struct hdmi_hw_config), cur_config, sizeof(struct hdmi_hw_config))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    if (memcpy_s(cur_config, sizeof(struct hdmi_hw_config), &target_hw_config, sizeof(struct hdmi_hw_config))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    return 0;
}

static hi_u32 hdmi_get_switch_mode(struct hisilicon_hdmi *hdmi)
{
    hi_u32 switch_mode = HDMI_SWITCH_MODE_TMDS_2_TMDS;
    struct hdmi_controller *controller = hdmi->ctrl;
    struct hdmi_hw_config *cur_config = &controller->cur_hw_config;
    struct hdmi_hw_config *pre_config = &controller->pre_hw_config;

    if (cur_config->work_mode == HDMI_WORK_MODE_TMDS &&
        pre_config->work_mode == HDMI_WORK_MODE_TMDS) {
        switch_mode = HDMI_SWITCH_MODE_TMDS_2_TMDS;
    } else if (cur_config->work_mode == HDMI_WORK_MODE_FRL &&
               pre_config->work_mode == HDMI_WORK_MODE_TMDS) {
        switch_mode = HDMI_SWITCH_MODE_TMDS_2_FRL;
    } else if (cur_config->work_mode == HDMI_WORK_MODE_TMDS &&
               pre_config->work_mode == HDMI_WORK_MODE_FRL) {
        switch_mode = HDMI_SWITCH_MODE_FRL_2_TMDS;
    } else if (cur_config->work_mode == HDMI_WORK_MODE_FRL &&
               pre_config->work_mode == HDMI_WORK_MODE_FRL) {
        switch_mode = HDMI_SWITCH_MODE_FRL_2_FRL;
    } else {
        /* illegal */
        HDMI_ERR("hdmi id = %d, work_mode(switch_mode:%d->%d) is invalid, please check param input\n",
                 hdmi->id, pre_config->work_mode, cur_config->work_mode);
    }
    HDMI_ERR("switch_mode:%d->%d.\n", pre_config->work_mode, cur_config->work_mode);
    return switch_mode;
}

static hi_bool hdmi_need_frl_link_training(struct hisilicon_hdmi *hdmi)
{
    hi_bool ret = false;
    struct hdmi_controller *controller = NULL;
    struct frl *frl = NULL;
    struct frl_stat stat;
    struct frl_scdc scdc;
    struct hdmi_hw_config *pre_config = NULL;
    struct hdmi_hw_config *cur_config = NULL;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    controller = hdmi->ctrl;
    frl = controller->frl;
    pre_config = &controller->pre_hw_config;
    cur_config = &controller->cur_hw_config;

    if (memset_s(&stat, sizeof(struct frl_stat), 0, sizeof(struct frl_stat))) {
        HDMI_ERR("memset_s fail.\n");
        return true;
    }
    if (memset_s(&scdc, sizeof(struct frl_scdc), 0, sizeof(struct frl_scdc))) {
        HDMI_ERR("memset_s fail.\n");
        return true;
    }
    hitxv300_frl_stat_get(frl, &stat);
    hitxv300_frl_scdc_get(frl, &scdc);
    /*
     *  First sw version, not need fast switch
     *  need verify by test guys.
     */
    if (scdc.frl_rate >= cur_config->min_frl_rate &&
        stat.event == TRAIN_EVENT_SUCCESS &&
        cur_config->work_mode == HDMI_WORK_MODE_FRL &&
        pre_config->work_mode == HDMI_WORK_MODE_FRL) {
        ret = true;
    } else {
        ret = true;
    }

    return ret;
}

static hi_s32 hdmi_video_config(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = hdmi->ctrl;

    /**
     *  Some of the infoframes should be set by vo, for example:
     *  hdr/vrr/avi/vsif. we need to carefully check with vdp guys.
     */
    hal_ctrl_set_infoframe(controller);
    hal_ctrl_set_video_path(controller);
    hal_ctrl_set_mode(controller);
    hal_ctrl_tmds_set_scramble(controller);

    return 0;
}

hi_s32 hdmi_soft_reset(struct hisilicon_hdmi *hdmi, hi_bool fast)
{
    hi_u32 i = 0;
    struct hdmi_controller *controller = hdmi->ctrl;

    if (fast) {
        hal_ctrl_fast_reset(controller);
    } else {
        hal_ctrl_pwd_soft_reset(controller);
    }

    do {
        if (hal_ctrl_tmds_clk_is_stable(controller)) {
            break;
        }
        osal_msleep(1); /* check whether the clock is stable per 1ms */
        i++;
    } while (i < 20); /* need wait 20 times. */

    HDMI_INFO("wait tmds clk stable %u ms(max 20)\n", i);

    return 0;
}

static hi_s32 hdmi_get_dsc_config(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = hdmi->ctrl;
    struct dsc_ctrl *dsc = controller->dsc;
    struct dsc_cfg_s *cfg = &dsc->cfg_s;
    struct dsc_timing_info *dsc_timing = &dsc->timing_info;
    struct para_input_s input;
    struct hdmi_timing_data *timing = &controller->mode.timing_data;
    struct hdmi_hw_config *cur_config = &controller->cur_hw_config;
    hi_u32 color_format = timing->out.color.color_format;
    hi_u32 color_depth = timing->out.color_depth;

    input.convert_rgb =
        (color_format == HDMI_COLOR_FORMAT_RGB) ? true : false;
    input.native_420 =
        (color_format == HDMI_COLOR_FORMAT_YCBCR420) ? true : false;
    input.native_422 =
        (color_format == HDMI_COLOR_FORMAT_YCBCR422) ? true : false;
    input.native_444 =
        (color_format == HDMI_COLOR_FORMAT_YCBCR444) ? true : false;

    if (color_depth == HDMI_BPC_24) {
        input.bits_per_component = 8; /* bits_per_component is 8bits */
    } else if (color_depth == HDMI_BPC_30) {
        input.bits_per_component = 10; /* bits_per_component is 10bits */
    } else if (color_depth == HDMI_BPC_36) {
        input.bits_per_component = 12; /* bits_per_component is 12bits */
    } else {
        HDMI_ERR("not support color depth\n");
    }

    input.bits_per_pixel = cur_config->bpp_target;

    input.pic_width = timing->in.detail.hactive;
    input.pic_height = timing->in.detail.vactive;

    dsc_timing->hblank = timing->in.detail.htotal - timing->in.detail.hactive;
    dsc_timing->vactive = timing->in.detail.vactive;
    dsc_timing->vback = timing->in.detail.vback;
    dsc_timing->vfront = timing->in.detail.vfront;
    dsc_timing->vsync = timing->in.detail.vsync;

    dsc_timing->hcactive = cur_config->hcactive;
    dsc_timing->hfront = 44; /* hfront must be 44. */
    dsc_timing->hcblank = cur_config->hcblank;
    dsc_timing->hback = (cur_config->hcblank - 44) / 2; /* hback must be (hcblank - 44)/2. */
    dsc_timing->de_polarity = true;
    dsc_timing->h_polarity = true;
    dsc_timing->v_polarity = true;

    input.slice_width = cur_config->slice_width;
    input.slice_height = 0; /* slice_height default value is 0. */

    hi_dsc_cfg_compute(&input, cfg);

    return 0;
}

hi_s32 hdmi_frl_work_en(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = hdmi->ctrl;
    struct frl *frl = controller->frl;
    struct hdmi_hw_config *cur_config = &controller->cur_hw_config;
    struct frl_scdc scdc;

    hitxv300_frl_scdc_get(frl, &scdc);
    cur_config->cur_frl_rate = scdc.frl_rate;
    if (scdc.frl_rate < cur_config->min_frl_rate &&
        scdc.frl_rate >= cur_config->min_dsc_frl_rate) {
        cur_config->dsc_enable = true;
    }

    if (cur_config->dsc_enable) {
        hdmi_get_dsc_config(hdmi);
        hi_hdmi_dsc_config(controller->dsc);
        hi_hdmi_dsc_enable(controller->dsc);
        hal_ctrl_set_audio_ncts(controller);
        /* worken enable */
        hitxv300_frl_worken_set(frl, true, false);
        hal_ctrl_set_dsc_emp(controller, true);
    } else {
        HDMI_ERR("Do not enable DSC\n");
        hal_ctrl_set_audio_ncts(controller);
        /* worken enable */
        hitxv300_frl_worken_set(frl, true, false);
    }

    return 0;
}

static hi_bool hdmi_frl_start(struct hisilicon_hdmi *hdmi)
{
    hi_bool ret = false;
    struct hdmi_controller *controller = hdmi->ctrl;
    struct frl *frl = controller->frl;
    struct frl_config config;
    struct hdmi_hw_config *cur_config = &controller->cur_hw_config;
	//struct hisilicon_hdmi *hdmi = frl->hdmi;
	//struct hdmi_connector *connector = hdmi->connector;
	
    if (memset_s(&config, sizeof(struct frl_config), 0, sizeof(struct frl_config))) {
        HDMI_ERR("memset_s fail.\n");
        return false;
    }

	//hdmi_connector_get_edid(connector, hdmi->ddc);
	
	
    hitxv300_frl_config_get(frl, &config);
	
	config.sink_version = sink_get_flt_sinkversion(frl);
	
	HDMI_ERR("read sink_version =%d\n",config.sink_version);
    config.frl_min_rate = cur_config->min_frl_rate;
    config.frl_max_rate = cur_config->max_frl_rate;
    config.dsc_frl_min_rate = cur_config->min_dsc_frl_rate;
    config.max_rate_proir = !controller->min_rate_prior;
    hitxv300_frl_config_set(frl, &config);
	HDMI_ERR("frl_max_rate set\n");
    if (hitxv300_frl_start(frl) == TRAIN_EVENT_SUCCESS) {
        cur_config->frl_train_success = true;
        ret = true;
    } else {
        cur_config->frl_train_success = false;
        ret = false;
    }

    return ret;
}

static hi_s32 hdmi_frl_mode_fast_switch(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = NULL;
    struct frl *frl = NULL;
    struct hdmi_hw_config *cur_config = NULL;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    controller = hdmi->ctrl;
    frl = controller->frl;
    cur_config = &controller->cur_hw_config;

    hdmi_crg_set(hdmi);
    if (!controller->ppll_enable) {
        hdmi_phy_fcg_set(hdmi);
    }
    /*
     *  Audio path is not configured here, it's expected to be done
     *  in the ao backup(oe off callback).
     */
    hdmi_video_config(hdmi);
    /* Deepcolor need disable when dsc enable in the hdmi2.1 spec. */
    if (cur_config->dsc_enable) {
        hal_ctrl_disable_deepcolor_for_dsc(controller);
    }
    hdmi_soft_reset(hdmi, true);

    if (cur_config->dsc_enable) {
        hdmi_get_dsc_config(hdmi);
        hi_hdmi_dsc_config(controller->dsc);
        hi_hdmi_dsc_enable(controller->dsc);
        /* worken enable */
        hitxv300_frl_worken_set(frl, true, true);
        hal_ctrl_set_dsc_emp(controller, true);
    } else {
        HDMI_ERR("Do not enable DSC\n");
        /* worken enable */
        hitxv300_frl_worken_set(frl, true, true);
    }

    return 0;
}

static hi_void drv_hdmi_compa_strategy_avmute(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    struct hdmi_controller *controller = NULL;
    struct hdmi_debug_info *debug_info = NULL;
    hi_u32 wait_time;

    controller = hdmi->ctrl;
    debug_info = hdmi->debug_info;

    wait_time = debug_info->d_avmute.wait_bef_stop;
    if ((enable == HI_TRUE) && (wait_time > 0)) {
        hal_ctrl_set_avmute(controller, HI_TRUE);
        osal_msleep(wait_time);
        HDMI_ERR("set avmute wait %dms before stop\n");
    }

   /* If wait_bef_stop > 0, means avmute(set) will send, and then avmute(clr) must send after start */
    wait_time = debug_info->d_avmute.wait_aft_start;
    if ((enable == HI_FALSE) && ((wait_time > 0) || (debug_info->d_avmute.wait_bef_stop > 0))) {
        osal_msleep(wait_time);
        hal_ctrl_set_avmute(controller, HI_FALSE);
        HDMI_ERR("wait %dms after start,then begin to clr avmute\n");
    }

    return;
}

static hi_s32 hdmi_tmds_mode_stop(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = NULL;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    controller = hdmi->ctrl;

    hdmi_ao_notifiers(hdmi, HDMI_BEFORE_OE_DISABLE);
    hal_ctrl_set_video_black(controller, true);
    drv_hdmi_compa_strategy_avmute(hdmi, HI_TRUE);
    hdmi_phy_off(hdmi);
    drv_hdmitx_hdcp_off(hdmi->hdcp);
    /* Close hdmi ouput, need to wait for one filed, maxtime = 1/24~50ms */
    osal_msleep(50);

    return 0;
}

static hi_s32 hdmi_frl_mode_stop(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = NULL;
    struct frl *frl = NULL;
    struct hdmi_hw_config *pre_config = NULL;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    controller = hdmi->ctrl;
    frl = controller->frl;
    pre_config = &controller->pre_hw_config;

    hdmi_ao_notifiers(hdmi, HDMI_BEFORE_OE_DISABLE);

    hal_ctrl_set_video_black(controller, true);
    drv_hdmi_compa_strategy_avmute(hdmi, HI_TRUE);

    if (pre_config->dsc_enable) {
        hal_ctrl_set_dsc_emp(controller, false);
    }

    if (!hdmi_need_frl_link_training(hdmi)) {
        /* worken disable */
        hitxv300_frl_worken_set(frl, false, false);
    } else {
        hdmi_phy_off(hdmi);
        osal_msleep(80); /* Need sleep 80ms after phy off */
        hitxv300_frl_stop(frl);
    }

    if (pre_config->dsc_enable) {
        hi_hdmi_dsc_disable(controller->dsc);
    }

    drv_hdmitx_hdcp_off(hdmi->hdcp);

    /* Close hdmi ouput, need to wait for one filed, maxtime = 1/24~50ms */
    osal_msleep(50);

    return 0;
}

static hi_s32 hdmi_tmds_mode_start(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = NULL;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    controller = hdmi->ctrl;
    /*
     *  Audio path expected to be done in the
     *  ao backup(oe off callback).
     */
    hdmi_ao_notifiers(hdmi, HDMI_AFTER_OE_DISABLE);

    hdmi_crg_set(hdmi);
    hdmi_phy_set(hdmi);
    /*
     *  Audio path is not configured here, it's expected to be done
     *  in the ao backup(oe off callback).
     */
    hdmi_video_config(hdmi);
    hdmi_soft_reset(hdmi, false);
    hdmi_phy_on(hdmi);

    osal_msleep(200); /* Enable HDCP auth after wait 200ms. */
    drv_hdmitx_hdcp_on(hdmi->hdcp);
    hal_ctrl_set_video_black(controller, false);
    hdmi_ao_notifiers(hdmi, HDMI_OE_ENABLE);
    drv_hdmi_compa_strategy_avmute(hdmi, HI_FALSE);

    return 0;
}

static hi_s32 hdmi_frl_mode_start(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *controller = NULL;
    struct frl *frl = NULL;
    struct hdmi_hw_config *cur_config = NULL;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    controller = hdmi->ctrl;
    frl = controller->frl;
    cur_config = &controller->cur_hw_config;
    /*
     *  Audio path expected to be done in the
     *  ao backup(oe off callback).
     */
    hdmi_ao_notifiers(hdmi, HDMI_AFTER_OE_DISABLE);

    if (hdmi_need_frl_link_training(hdmi)) {
        /*
         *  Must ensure the clock is ready before set
         *  video path&hdmi top. It's say by HW guys.
         */
        hdmi_crg_set(hdmi);
        hdmi_phy_set(hdmi);
        if (!controller->ppll_enable) {
            hdmi_phy_fcg_set(hdmi);
        }
        /*
         *  Audio path is not configured here, it's expected to be done
         *  in the ao backup(oe off callback).
         */
        hdmi_video_config(hdmi);
		HDMI_ERR("ph test1.\n");
        if (!hdmi_frl_start(hdmi)) {
            HDMI_ERR("frl training fail ph.\n");
            return -EPERM;
        }
    } else { /* FRL fast switch need verify by test guys */
        if (!hdmi_frl_mode_fast_switch(hdmi)) {
            HDMI_ERR("frl fast switch fail.\n");
            return -EPERM;
        }
    }

    osal_msleep(200); /* Enable HDCP auth after wait 200ms. */
    drv_hdmitx_hdcp_on(hdmi->hdcp);
    hal_ctrl_set_video_black(controller, false);
    hdmi_ao_notifiers(hdmi, HDMI_OE_ENABLE);
    drv_hdmi_compa_strategy_avmute(hdmi, HI_FALSE);

    return 0;
}

static hi_s32 hdmi_vo_timing_prepare(struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_u32 switch_mode;

    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    ret = hdmi_cur_hw_config_decision(hdmi);
    if (ret) {
        HDMI_ERR("This mode is not in the list!\n");
        return ret;
    }

    switch_mode = hdmi_get_switch_mode(hdmi);
    switch (switch_mode) {
        case HDMI_SWITCH_MODE_TMDS_2_TMDS:
        case HDMI_SWITCH_MODE_TMDS_2_FRL:
            ret = hdmi_tmds_mode_stop(hdmi);
            break;
        case HDMI_SWITCH_MODE_FRL_2_TMDS:
        case HDMI_SWITCH_MODE_FRL_2_FRL:
            ret = hdmi_frl_mode_stop(hdmi);
            break;
        default:
            HDMI_ERR("This switch mode is illegality!\n");
            ret = HDMI_ERR_INPUT_PARAM_INVALID;
            break;
    }

    return ret;
}

static hi_s32 hdmitx_regs_parse(struct hisilicon_hdmi *hdmi, struct platform_device *pdev)
{
    struct resource *mem;
    struct device *dev = &pdev->dev;

    /* parse hdmi regs */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0); /* register index is 0. */
    hdmi->hdmi_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->hdmi_regs)) {
        return PTR_ERR(hdmi->hdmi_regs);
    }

    /* parse hdmi aon regs */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 1); /* register index is 1. */
    hdmi->hdmi_aon_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->hdmi_aon_regs)) {
        return PTR_ERR(hdmi->hdmi_aon_regs);
    }

    /* parse aon crg regs */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 2); /* register index is 2. */
    hdmi->aon_crg_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->aon_crg_regs)) {
        return PTR_ERR(hdmi->aon_crg_regs);
    }

    /* parse ctrl crg regs */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 3); /* register index is 3. */
    hdmi->ctrl_crg_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->ctrl_crg_regs)) {
        return PTR_ERR(hdmi->ctrl_crg_regs);
    }

    /* parse phy crg regs */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 4); /* register index is 4. */
    hdmi->phy_crg_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->phy_crg_regs)) {
        return PTR_ERR(hdmi->phy_crg_regs);
    }

    /* parse ppll crg regs */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 5); /* register index is 5. */
    hdmi->ppll_crg_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->ppll_crg_regs)) {
        return PTR_ERR(hdmi->ppll_crg_regs);
    }

    mem = platform_get_resource(pdev, IORESOURCE_MEM, 6); /* register index is 6. */
    hdmi->ppll_state_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->ppll_state_regs)) {
        return PTR_ERR(hdmi->ppll_state_regs);
    }

    /* sysctrl regs */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 7); /* register index is 7. */
    hdmi->sysctrl_regs = devm_ioremap_resource(dev, mem);
    if (IS_ERR(hdmi->sysctrl_regs)) {
        return PTR_ERR(hdmi->sysctrl_regs);
    }
    return 0;
}

static hi_s32 hisilicon_hdmi_dt_parse(struct hisilicon_hdmi *hdmi,
                                      struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    hi_s32 ret;
    struct device *dev = &pdev->dev;

    ret = hdmitx_regs_parse(hdmi, pdev);
    if (ret) {
        HDMI_ERR("hdmitx[%d] regs parse fail(ret:%d).\n", hdmi->id, ret);
        return ret;
    }

    /* parse hdmi irq */
    hdmi->irq = platform_get_irq(pdev, 0); /* irq index is 0. */
    if (hdmi->irq < 0) {
        dev_err(dev, "Failed to get hdmi irq: %d\n", hdmi->irq);
        return hdmi->irq;
    }

    hdmi->pwd_irq = platform_get_irq(pdev, 1);
    if (hdmi->pwd_irq < 0) {
        dev_err(dev, "Failed to get pwd irq: %d\n", hdmi->pwd_irq);
        return hdmi->pwd_irq;
    }

    ret = of_property_read_u32(np, "hdmi_id", &hdmi->id);
    if (ret) {
        dev_err(dev, "Failed to get hdmi_id: %d\n", ret);
        return ret;
    }

    ret = of_property_read_u32(np, "ctrl_version", &hdmi->version);
    if (ret) {
        dev_err(dev, "Failed to get hdmi controller version: %d\n", ret);
        return ret;
    }

    ret = of_property_read_u32(np, "caps", &hdmi->caps);
    if (ret) {
        dev_err(dev, "Failed to get hdmi controller caps: %d\n", ret);
        return ret;
    }

    ret = of_property_read_u32(np, "caps2", &hdmi->caps2);
    if (ret) {
        dev_err(dev, "Failed to get hdmi controller caps2: %d\n", ret);
        return ret;
    }

    return ret;
}

void hdmi_sysfs_event(struct hisilicon_hdmi *hdmi,
    char *event, hi_u32 size)
{
    char *event_string = event;
    char *envp[] = { event_string, NULL };

    if (size == 0) {
        HDMI_ERR("size is zero.\n");
        return;
    }

    if (hdmi == NULL || event == NULL) {
        HDMI_ERR("null pointer.\n");
        return;
    }

    HDMI_INFO("event %s\n\n", event_string);

    kobject_uevent_env(&hdmi->dev->kobj, KOBJ_CHANGE, envp);
}

static hi_bool is_mode_exceed_port20_cap(struct hisilicon_hdmi *hdmi, struct hdmi_timing_data *timing)
{
    hi_u32 tmds_clk;

    tmds_clk = hdmi_mode_clock_pixel2tmds(timing->in.pixel_clock,
        timing->out.color_depth,
        timing->out.color.color_format);
    if ((!hdmi->is_hdmi21) && (tmds_clk > HDMI0_MAX_TMDS_CLOCK_RATE * 1000)) { /* 1000 for unit conversion */
        return true;
    }

    return false;
}

static void hdmi_vo_suspend(struct hisilicon_hdmi *hdmi, void *data)
{
    hi_u32 work_mode;

    work_mode = hdmi->ctrl->cur_hw_config.work_mode;

    /*
     * Suspend function need do follows steps to get better compatibility
     * step 1: Mask HPD interrupt
     */
    hdmi_set_hpd_irq_mask(hdmi, HI_TRUE);

    /*
     * step 2: Set black frame data
     * step 3: Set avmute packet and wait for three video field
     * step 4: Close hdmi output and wait for one video field
     */
    if (work_mode == HDMI_WORK_MODE_TMDS) {
        hdmi_tmds_mode_stop(hdmi);
    } else {
        hdmi_frl_mode_stop(hdmi);
    }

    /* step 5: Disable hdmi clock */
    hdmi_clk_enable(hdmi, HI_FALSE);

    drv_hdmitx_cec_suspend(hdmi->cec);

    HI_PRINT("hdmi%d suspend ok.\n", hdmi->id);
    return;
}

static hi_s32 hdmi_vo_resume(struct hisilicon_hdmi *hdmi, void *data)
{
    hi_s32 ret;
    hi_u32 work_mode;
    struct hdmi_connector *connector = NULL;
    struct hdmi_controller *controller = NULL;
    struct hdmi_timing_data *timing = NULL;

    work_mode  = hdmi->ctrl->cur_hw_config.work_mode;
    connector  = hdmi->connector;
    controller = hdmi->ctrl;
    timing     = &(controller->mode.timing_data);

    drv_hdmitx_cec_resume(hdmi->cec);

#if defined(CHIP_TYPE_HI3796CV300)
    hdmi_hw_ctrl_phy_mapping_set(CRLT_PHY_MAP_MODE_CROSS);
#elif defined(CHIP_TYPE_HI3751V900)
    hdmi_hw_ctrl_phy_mapping_set(CRLT_PHY_MAP_MODE_NO_CROSS);
#endif
    /*
     * Rsesume function need do follows steps to get better compatibility
     * step 1: Enable hdmi clock
     */
    hdmi_clk_enable(hdmi, HI_TRUE);

    /* step 2: Need wait more than 100ms to Read HPD status,and read edid */
    hdmi_connector_enable_hpd(hdmi, connector);

    /* step 3: Check video mode valid */
    HDMI_INFO("vic=%u,c_fmt=%d,c_depth=%d\n", timing->in.vic,
        timing->out.color.color_format, timing->out.color_depth);

    if (is_mode_exceed_port20_cap(hdmi, timing) == HI_TRUE) {
        HDMI_WARN("hdmi_id = %d not support 8k, pls try another one", hdmi->id);
    }
    ret = hdmi_connector_mode_validate(connector, timing);
    if (ret == false) {
        HDMI_ERR("hdmi_id = %d, mode(vic=%u,c_fmt=%d,c_depth=%d) isn't avail\n",
            hdmi->id, timing->in.vic, timing->out.color.color_format,
            timing->out.color_depth);
    }

    /*
     * step 4: Video mode config
     * step 5: Clear black data frame
     * step 6: Set clear avmute, clear avmute packet will be sent always
     */
    if (work_mode == HDMI_WORK_MODE_TMDS) {
        hdmi_tmds_mode_start(hdmi);
    } else {
        hdmi_frl_mode_start(hdmi);
    }

    HI_PRINT("hdmi%d resume ok.\n", hdmi->id);

    return HI_SUCCESS;
}

static hi_s32 hdmi_vo_attach(struct hisilicon_hdmi *hdmi, void *data)
{
    return 0;
}

static hi_s32 hdmi_vo_detach(struct hisilicon_hdmi *hdmi, void *data)
{
    return 0;
}

static hi_s32 hdmi_vo_mode_validate(struct hisilicon_hdmi *hdmi, hi_s32 mode,
                                    struct hi_display_mode *display_mode)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hdmi_connector *connector = hdmi->connector;
    struct hdmi_timing_data *timing = NULL;
	HDMI_ERR("enter hdmi_vo_mode_validate.\n");
    if (connector->hotplug == HPD_PLUGOUT) {
        HDMI_ERR("hdmi_id = %d, hotplug is 0, please check cable whether is connected, and TV whether is powered on\n",
                 hdmi->id);
        return ctrl->force_output ? DISPLAY_MODE_OK : HDMI_ERR_CABLE_PLUG_OUT; /* force output. */
    } else if (connector->hotplug == HPD_DET_FAIL) {
        HDMI_WARN("hdmi_id = %d, hotplug detect fail, hdmi port or cable may damaged\n", hdmi->id);
    }

    if (mode & TIMING_SET_MODE) {
        timing = &display_mode->timing_data;
        HDMI_INFO("vic=%u,c_fmt=%d,c_depth=%d\n", timing->in.vic,
            timing->out.color.color_format, timing->out.color_depth);

        if (is_mode_exceed_port20_cap(hdmi, timing)) {
            HDMI_ERR("hdmi_id = %d, current port hdmitx0 not support 8k, please try hdmitx1\n");
            return ctrl->force_output ? DISPLAY_MODE_OK : HDMI_ERR_MODE_EXCEED_PORT20_CAP; /* force output. */
        }

        if (!hdmi_connector_mode_validate(connector, timing)) {
            HDMI_ERR("hdmi_id = %d, mode(vic=%u,c_fmt=%d,c_depth=%d) isn't avail, please set an avail-mode\n",
                     hdmi->id, timing->in.vic, timing->out.color.color_format, timing->out.color_depth);
            return ctrl->force_output ? DISPLAY_MODE_OK : HDMI_ERR_MODE_NOT_AVAIL; /* force output. */
        }
    }

    return DISPLAY_MODE_OK;
}

static hi_s32 hdmi_vo_prepare(struct hisilicon_hdmi *hdmi, hi_s32 mode,
                              struct hi_display_mode *display_mode)
{
    struct hdmi_controller *controller = NULL;
    hi_s32 ret = 0;
	//HDMI_ERR("enter hdmi_vo_prepare\n");
    if (hdmi == NULL || display_mode == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    controller = hdmi->ctrl;

    if (mode & TIMING_SET_MODE) {
        if (memcpy_s(&controller->mode.timing_data,
            sizeof(struct hdmi_timing_data),
            &display_mode->timing_data,
            sizeof(struct hdmi_timing_data))) {
            HDMI_ERR("memset_s fail.\n");
            return HI_FAILURE;
        }
        ret = hdmi_vo_timing_prepare(hdmi);
        if (ret) {
            HDMI_ERR("vo timing set prepare fail.\n");
            return ret;
        }
    }

    if (mode & HDR_SET_MODE) {
        if (memcpy_s(&controller->mode.hdr_data, sizeof(struct hdmi_hdr_data),
            &display_mode->hdr_data, sizeof(struct hdmi_hdr_data))) {
            HDMI_ERR("memset_s fail.\n");
            return HI_FAILURE;
        }
    }

    if (mode & VRR_SET_MODE) {
        if (memcpy_s(&controller->mode.vrr_data, sizeof(struct hdmi_vrr_data),
            &display_mode->vrr_data, sizeof(struct hdmi_vrr_data))) {
            HDMI_ERR("memset_s fail.\n");
            return HI_FAILURE;
        }
    }

    return ret;
}

static hi_s32 hdmi_vo_mode_set(struct hisilicon_hdmi *hdmi, hi_s32 mode,
                               struct hi_display_mode *display_mode)
{
    hi_s32 ret = 0;
    hi_u32 switch_mode = HDMI_SWITCH_MODE_TMDS_2_TMDS;
	//HDMI_ERR("enter hdmi_vo_mode_set.\n");
    if (hdmi == NULL || display_mode == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    if (mode & TIMING_SET_MODE) {
        switch_mode = hdmi_get_switch_mode(hdmi);
        switch (switch_mode) {
            case HDMI_SWITCH_MODE_TMDS_2_TMDS:
            case HDMI_SWITCH_MODE_FRL_2_TMDS:
                ret = hdmi_tmds_mode_start(hdmi);
                break;
            case HDMI_SWITCH_MODE_TMDS_2_FRL:
            case HDMI_SWITCH_MODE_FRL_2_FRL:
                ret = hdmi_frl_mode_start(hdmi);
                break;
            default:
                ret = HDMI_ERR_INPUT_PARAM_INVALID;
                break;
        }
    }

    return ret;
}

static hi_s32 hdmi_vo_atomic_mode_set(struct hisilicon_hdmi *hdmi, hi_s32 mode,
                                      struct hi_display_mode *display_mode)
{
    return 0;
}

static hi_s32 hdmi_vo_display_on(struct hisilicon_hdmi *hdmi)
{
    return 0;
}

static hi_s32 hdmi_vo_display_off(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    hdmi_phy_off(hdmi);

    return 0;
}

static struct hdmi_vo_ops g_hdmi_vo_ops = {
    .suspend = hdmi_vo_suspend,
    .resume = hdmi_vo_resume,
    .attach = hdmi_vo_attach,
    .detach = hdmi_vo_detach,
    .prepare = hdmi_vo_prepare,
    .mode_validate = hdmi_vo_mode_validate,
    .mode_set = hdmi_vo_mode_set,
    .atomic_mode_set = hdmi_vo_atomic_mode_set,
    .display_on = hdmi_vo_display_on,
    .display_off = hdmi_vo_display_off,
};

static hi_s32 hdmi_ao_get_eld(struct hisilicon_hdmi *hdmi, void *data, hi_u8 *buf, size_t len)
{
    len = osal_min(sizeof(struct audio_property), len);
    if (memcpy_s(buf, len, &hdmi->connector->audio, len)) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }
    return 0;
}

static hi_s32 hdmi_ao_hw_params(struct hisilicon_hdmi *hdmi,
                                struct ao_attr *attr)
{
    struct hdmi_controller *controller = hdmi->ctrl;

    if (memcpy_s(&controller->attr, sizeof(struct ao_attr), attr, sizeof(struct ao_attr))) {
        HDMI_ERR("memset_s fail.\n");
        return HI_FAILURE;
    }

    hal_ctrl_set_audio_path(controller);

    return 0;
}

static hi_s32 hdmi_ao_hw_params_validate(struct hisilicon_hdmi *hdmi,
                                         struct ao_attr *attr)
{
    return 0;
}

static hi_s32 hdmi_ao_hpd_detect(struct hisilicon_hdmi *hdmi, hi_u32 *status)
{
    struct hdmi_connector *connector = NULL;

    if (hdmi == NULL || status == NULL) {
        HDMI_ERR("input params is NULL pointer.\n");
        return -EINVAL;;
    }

    connector = hdmi->connector;

    if (connector->status == HPD_PLUGIN ||
        connector->status == HPD_DET_FAIL) {
        *status = HDMI_HPD_PLUGIN;
    } else if (connector->status == HPD_PLUGOUT) {
        *status = HDMI_HPD_PLUGOUT;
    }

    return 0;
}

static hi_s32 hdmi_ao_digital_mute(struct hisilicon_hdmi *hdmi, void *data,
                                   hi_bool enable)
{
    struct hdmi_controller *controller = hdmi->ctrl;

    hal_ctrl_audio_enable(controller, enable);

    return 0;
}

static hi_s32 hdmi_ao_register_notifier(struct hisilicon_hdmi *hdmi,
                                        struct notifier_block *nb)
{
    return raw_notifier_chain_register(&hdmi->notifier_list, nb);
}

static hi_s32 hdmi_ao_unregister_notifier(struct hisilicon_hdmi *hdmi,
                                          struct notifier_block *nb)
{
    return raw_notifier_chain_unregister(&hdmi->notifier_list, nb);
}

struct hdmi_ao_ops g_hdmi_ao_ops = {
    .get_eld = hdmi_ao_get_eld,
    .hw_params_validate = hdmi_ao_hw_params_validate,
    .hw_params = hdmi_ao_hw_params,
    .digital_mute = hdmi_ao_digital_mute,
    .hpd_detect = hdmi_ao_hpd_detect,
    .register_notifier = hdmi_ao_register_notifier,
    .unregister_notifier = hdmi_ao_unregister_notifier,
};

static hi_s32 hisilicon_hdmi_phy_init(struct hisilicon_hdmi *hdmi)
{
    hi_u32 reg_base;
    hi_u32 tpll_reg_base;
    struct hdmitx_phy_resuorce res;

    res.phy_id = hdmi->id;
    res.ctrl_id = hdmi->id;
#if defined(CHIP_TYPE_HI3796CV300)
    if (res.phy_id == PHY_ID_0) {
        reg_base = HDMITX_PHY0_REG_BASE;
        tpll_reg_base = HDMITX_PHY0_TPLL_REG_BASE;
    } else {
        reg_base = HDMITX_PHY1_REG_BASE;
        tpll_reg_base = HDMITX_PHY1_TPLL_REG_BASE;
    }
#else
    if (res.phy_id == PHY_ID_0) {
        reg_base = HDMITX_PHY1_REG_BASE;
        tpll_reg_base = HDMITX_PHY1_TPLL_REG_BASE;
    } else {
        reg_base = HDMITX_PHY0_REG_BASE;
        tpll_reg_base = HDMITX_PHY0_TPLL_REG_BASE;
    }
#endif

    res.regs = osal_ioremap_nocache(reg_base, HDMITX_PHY_REG_SIZE);
    if (IS_ERR(res.regs)) {
        HDMI_ERR("osal_ioremap_nocache failed!\n");
        return HI_FAILURE;
    }
    res.tpll_regs = osal_ioremap_nocache(tpll_reg_base, HDMITX_PHY_REG_SIZE);
    if (IS_ERR(res.tpll_regs)) {
        osal_iounmap(res.regs);
        HDMI_ERR("osal_ioremap_nocache failed!\n");
        return HI_FAILURE;
    }

    hdmi->phy = hal_phy_init(&res);
    if (hdmi->phy == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void hisilicon_hdmi_phy_exit(struct hisilicon_hdmi *hdmi)
{
    struct hdmitx_phy_resuorce res;

    if (hdmi->phy == HI_NULL) {
        return;
    }

    res.regs = hdmi->phy->regs;
    res.tpll_regs = hdmi->phy->tpll_regs;
    hal_phy_deinit(hdmi->phy);
    hdmi->phy = HI_NULL;
    osal_iounmap(res.regs);
    osal_iounmap(res.tpll_regs);
}

static hi_s32 hisilicon_hdmi_init(struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    struct cec_resource cec_res;

    hdmi->vo_ops = &g_hdmi_vo_ops;

    hdmi->ao_ops = &g_hdmi_ao_ops;

    hdmi->is_hdmi21 = hdmi->id ? false : true;

    ret = hal_ddc_init(hdmi);
    if (ret) {
        HDMI_ERR("ddc init failed\n");
        goto err1;
    }

    hdmi->connector = hdmi_connector_init(hdmi);
    if (!hdmi->connector) {
        HDMI_ERR("connector init failed\n");
        ret = -ENODEV;
        goto err2;
    }

    hdmi->ctrl = hal_ctrl_controller_init(hdmi);
    if (!hdmi->ctrl) {
        HDMI_ERR("controller init failed\n");
        ret = -ENODEV;
        goto err3;
    }

    hdmi->crg = hdmi_crg_init(hdmi);
    if (!hdmi->crg) {
        HDMI_ERR("connector init failed\n");
        ret = -ENODEV;
        goto err4;
    }

    ret = hisilicon_hdmi_phy_init(hdmi);
    if (ret) {
        HDMI_ERR("phy init failed\n");
        ret = -ENODEV;
        goto err5;
    }
    RAW_INIT_NOTIFIER_HEAD(&hdmi->notifier_list);

    cec_res.id = hdmi->id;
    cec_res.regs_base = hdmi->hdmi_aon_regs + HDMITX_CEC_REGS_OFFSET;
    cec_res.crg_regs_base = hdmi->aon_crg_regs;
    cec_res.sysctrl_regs_base = hdmi->sysctrl_regs;
    hdmi->cec = drv_hdmitx_cec_init(&cec_res);
    if (hdmi->cec == HI_NULL) {
        HDMI_ERR("cec init failed\n");
        ret = -ENODEV;
        goto err6;
    }

    ret = drv_hdmitx_hdcp_init(hdmi);
    if (ret != HI_SUCCESS) {
        HDMI_ERR("hdcp fail\n");
        ret = -ENODEV;
        goto err7;
    }
    return 0;
err7:
    drv_hdmitx_cec_deinit(hdmi->cec);
err6:
    hisilicon_hdmi_phy_exit(hdmi);
err5:
    hdmi_crg_deinit(hdmi->crg);
err4:
    hal_ctrl_controller_deinit(hdmi->ctrl);
err3:
    hdmi_connector_deinit(hdmi);
err2:
    hal_ddc_deinit(hdmi);
err1:
    return ret;
}

static void hisilicon_hdmi_deinit(struct hisilicon_hdmi *hdmi)
{
    drv_hdmitx_hdcp_deinit(hdmi);
    drv_hdmitx_cec_deinit(hdmi->cec);
    hisilicon_hdmi_phy_exit(hdmi);
    hdmi_crg_deinit(hdmi->crg);
    hal_ctrl_controller_deinit(hdmi->ctrl);
    hdmi_connector_deinit(hdmi);
    hal_ddc_deinit(hdmi);
}

static hi_s32 hisilicon_hdmi_add_mapping(struct hisilicon_hdmi *hdmi)
{
    if (g_hdmi_cnt >= MAX_HDMI_ID) {
        return -ENOMEM;
    }

    g_mapping[g_hdmi_cnt].hdmi_id = hdmi->id;
    g_mapping[g_hdmi_cnt].hdmi = hdmi;
    g_hdmi_cnt++;

    return 0;
}

static void hisilicon_hdmi_delect_mapping(void)
{
    if (g_hdmi_cnt >= MAX_HDMI_ID || g_hdmi_cnt <= 0) {
        return;
    }

    g_hdmi_cnt--;
    g_mapping[g_hdmi_cnt].hdmi_id = -1;
    g_mapping[g_hdmi_cnt].hdmi = NULL;
}

static hi_s32 hdmi_cdev_open(struct inode *inode, struct file *filp)
{
    struct hisilicon_hdmi *hdmi = osal_container_of(filp->private_data,
                                               struct hisilicon_hdmi, miscdev);

    filp->private_data = hdmi;

    return 0;
}

static hi_s32 hdmitx_cdev_close(struct inode *inode, struct file *filp)
{
    struct hisilicon_hdmi *hdmi = filp->private_data;
    drv_hdmitx_cec_release(hdmi->cec);
    drv_hdmitx_hdcp_stop_auth(hdmi->hdcp);
    return HI_SUCCESS;
}

static long ioc_read_edid(struct hisilicon_hdmi *hdmi, unsigned long arg)
{
    hdmitx_ioctl_getedid data;
    struct hdmi_connector *connector = NULL;

    if (!hdmi || !arg) {
        return -EINVAL;
    }

    connector = hdmi->connector;

    if (osal_copy_from_user(&data, (hi_void *)arg, sizeof(hdmitx_ioctl_getedid))) {
        return HI_FAILURE;
    }

    if (data.force) {
        hdmi_connector_edid_reset(connector);
        connector->edid_raw = hdmi_connector_get_edid(connector, hdmi->ddc);
    }

    if (!connector->edid_raw || !data.edid_ptr ||
        !connector->edid_size || !data.length) {
        HDMI_INFO("data.edid null!\n");
        return -ENOMEM;
    }

    if (data.length > (u32)connector->edid_size) {
        data.length = (u32)connector->edid_size;
    }

    if (osal_copy_to_user((hi_void *)data.edid_ptr, hdmi->connector->edid_raw, data.length)) {
        return HI_FAILURE;
    }

    if (osal_copy_to_user((hi_void *)arg, &data, sizeof(hdmitx_ioctl_getedid))) {
        return HI_FAILURE;
    }

    return 0;
}

static long ioc_get_sinkinfo(struct hisilicon_hdmi *hdmi, unsigned long arg)
{
    long ret = 0;
    hdmitx_ioctl_getsinkinfo *data = NULL;
    struct hdmi_connector *connector = NULL;

    if (!hdmi || !arg) {
        return -EINVAL;
    }

    connector = hdmi->connector;

    data = (hdmitx_ioctl_getsinkinfo *)osal_kmalloc(HI_ID_HDMITX, sizeof(hdmitx_ioctl_getsinkinfo), OSAL_GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }
    if (memset_s(data, sizeof(hdmitx_ioctl_getsinkinfo), 0, sizeof(hdmitx_ioctl_getsinkinfo))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, data);
        return HI_FAILURE;
    }

    if (sizeof(hdmi_audio_info) != sizeof(struct audio_property)) {
        ret = HI_FAILURE;
        goto error_out;
    }
    if (memcpy_s(&data->audio, sizeof(struct audio_property),
        &connector->audio, sizeof(struct audio_property))) {
        HDMI_ERR("memset_s fail.\n");
        ret = HI_FAILURE;
        goto error_out;
    }
    if (sizeof(hdmi_colorimetry) != sizeof(struct colorimetry)) {
        ret = HI_FAILURE;
        goto error_out;
    }
    if (memcpy_s(&data->colorimetry, sizeof(struct colorimetry),
        &connector->color.colorimetry, sizeof(struct colorimetry))) {
        HDMI_ERR("memset_s fail.\n");
        ret = HI_FAILURE;
        goto error_out;
    }
    data->disp_para.max_image_height = connector->display_info.height_cm;
    data->disp_para.max_image_width = connector->display_info.width_cm;
    if (sizeof(hdmi_dolby_info) != sizeof(struct dolby_property)) {
        ret = HI_FAILURE;
        goto error_out;
    }
    if (memcpy_s(&data->dolby, sizeof(struct dolby_property),
        &connector->dolby, sizeof(struct dolby_property))) {
        HDMI_ERR("memset_s fail.\n");
        ret = HI_FAILURE;
        goto error_out;
    }
    data->edid_version.revision = connector->base.revision;
    data->edid_version.version = connector->base.version;
    if (sizeof(hdmi_hdr_info) != sizeof(struct hdr_property)) {
        ret = HI_FAILURE;
        goto error_out;
    }

    if (memcpy_s(&data->hdr, sizeof(struct hdr_property),
        &connector->hdr, sizeof(struct hdr_property))) {
        HDMI_ERR("memset_s fail.\n");
        ret = HI_FAILURE;
        goto error_out;
    }
    if (sizeof(hdmi_latency) != sizeof(struct latency_property)) {
        ret = HI_FAILURE;
        goto error_out;
    }
    if (memcpy_s(&data->latency, sizeof(struct latency_property),
        &connector->latency, sizeof(struct latency_property))) {
        HDMI_ERR("memset_s fail.\n");
        ret = HI_FAILURE;
        goto error_out;
    }
    if (memcpy_s(data->manufacture.mfrs_name, ARRAY_SIZE(data->manufacture.mfrs_name),
        connector->base.vendor.mfc_name, ARRAY_SIZE(data->manufacture.mfrs_name))) {
        HDMI_ERR("memset_s fail.\n");
        ret = HI_FAILURE;
        goto error_out;
    }
    data->manufacture.product_code = connector->base.vendor.product_code;
    data->manufacture.serial_number = connector->base.vendor.serial_num;
    data->manufacture.week = connector->base.vendor.mfc_week;
    data->manufacture.year = connector->base.vendor.mfc_year;
    /* data->speaker */
    if (sizeof(hdmi_vrr_info) != sizeof(struct vrr_property)) {
        ret = HI_FAILURE;
        goto error_out;
    }
    if (memcpy_s(&data->vrr, sizeof(struct vrr_property),
        &connector->vrr, sizeof(struct vrr_property))) {
        HDMI_ERR("memset_s fail.\n");
        ret = HI_FAILURE;
        goto error_out;
    }

    if (osal_copy_to_user((hi_void *)arg, data, sizeof(hdmitx_ioctl_getsinkinfo))) {
        ret = HI_FAILURE;
        goto error_out;
    }

    if (data) {
        osal_kfree(HI_ID_HDMITX, data);
    }

    return ret;

error_out:
    if (data) {
        osal_kfree(HI_ID_HDMITX, data);
    }
    return ret;
}

static long ioc_get_status(struct hisilicon_hdmi *hdmi, unsigned long arg)
{
    hdmitx_ioctl_getstatus data;
    struct hdmi_connector *connector = NULL;

    if (hdmi == NULL || !arg) {
        return -EINVAL;
    }

    connector = hdmi->connector;

    data.hotplug = (hi_s32)connector->hotplug;
    data.rsen = (hi_s32)hdmi_connector_get_rsen(hdmi);
    data.output_enable = hal_phy_is_on(hdmi->phy);

    if (osal_copy_to_user((hi_void *)arg, &data, sizeof(hdmitx_ioctl_getstatus))) {
        return HI_FAILURE;
    }

    return 0;
}

static long ioc_get_availcapbility(struct hisilicon_hdmi *hdmi, unsigned long arg)
{
    hi_s32 ret;
    hdmitx_ioctl_getcapbility *data = NULL;
    struct hdmi_connector *connector = NULL;

    if (!hdmi || !arg) {
        return -EINVAL;
    }

    connector = hdmi->connector;

    data = (hdmitx_ioctl_getcapbility *)osal_kmalloc(HI_ID_HDMITX, sizeof(hdmitx_ioctl_getcapbility), OSAL_GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }
    if (memset_s(data, sizeof(hdmitx_ioctl_getcapbility), 0, sizeof(hdmitx_ioctl_getcapbility))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, data);
        return HI_FAILURE;
    }

    data->mode_count = connector->probed_mode_cnt;
    data->dvi_only = !(connector->display_info.has_hdmi_infoframe && connector->src_cap.hdmi.hdmi_support);
    data->max_tmds_clock = osal_min(connector->display_info.max_tmds_clock, connector->src_cap.hdmi.max_tmds_clock);
    ret = hdmi_connector_get_native_mode(connector, &data->native_mode);
    ret |= hdmi_connector_get_max_mode(connector, &data->max_mode);

    if (osal_copy_to_user((hi_void *)arg, data, sizeof(hdmitx_ioctl_getcapbility))) {
        ret = HI_FAILURE;
    }

    if (data) {
        osal_kfree(HI_ID_HDMITX, data);
    }

    return ret;
}

static long ioc_get_availmodes(struct hisilicon_hdmi *hdmi,
                               unsigned long arg)
{
    long ret;
    hdmitx_ioctl_getmodes data;
    hdmi_avail_mode *mode_buf = NULL;
    struct hdmi_connector *connector = NULL;

    if (!hdmi || !arg) {
        return -EINVAL;
    }

    connector = hdmi->connector;

    if (!connector->probed_mode_cnt) {
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&data, (hi_void *)arg, sizeof(hdmitx_ioctl_getmodes))) {
        return HI_FAILURE;
    }

    if (data.mode_count > connector->probed_mode_cnt) {
        data.mode_count = connector->probed_mode_cnt;
    }

    mode_buf = osal_kmalloc(HI_ID_HDMITX, data.mode_count * sizeof(hdmi_avail_mode), OSAL_GFP_KERNEL);
    if (!mode_buf) {
        return -ENOMEM;
    }
    if (memset_s(mode_buf, sizeof(hdmi_avail_mode), 0, sizeof(hdmi_avail_mode))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, mode_buf);
        return HI_FAILURE;
    }

    ret = hdmi_connector_get_availmode(connector, mode_buf, data.mode_count);

    if (osal_copy_to_user((hi_void *)data.mode_buf_ptr, mode_buf, data.mode_count * sizeof(hdmi_avail_mode))) {
        ret = HI_FAILURE;
        goto error_out;
    }

    if (osal_copy_to_user((hi_void *)arg, &data, sizeof(hdmitx_ioctl_getmodes))) {
        ret = HI_FAILURE;
        goto error_out;
    }

    if (mode_buf) {
        osal_kfree(HI_ID_HDMITX, mode_buf);
    }

    return ret;

error_out:
    if (mode_buf) {
        osal_kfree(HI_ID_HDMITX, mode_buf);
    }
    return ret;
}

static long ioc_set_force_output(struct hisilicon_hdmi *hdmi, unsigned long arg)
{
    hi_bool enable = HI_FALSE;

    if (osal_copy_from_user(&enable, (hi_void *)arg, sizeof(hi_bool))) {
        HDMI_ERR("hdmi%d-osal_copy_from_user failed!\n", hdmi->id);
        return HI_FAILURE;
    }
    HDMI_INFO("[hdmi%d]set force output:%s\n", hdmi->id, enable ? "TRUE" : "FALSE");
    hdmi->ctrl->force_output = enable;
    return HI_SUCCESS;
}

void ioc_handle_hdcp_cmd(struct hisilicon_hdmi *hdmi, unsigned long arg, unsigned int cmd, long *ret)
{
    hi_u32 tmp;
    struct hdcp_cap hdcp_cap;
    struct hdcp_usr_status hdcp_status;

    switch (cmd) {
        case HDMITX_IOC_HDCP_LOADKEY:
            if (osal_copy_from_user(&tmp, (hi_void *)arg, sizeof(hi_u32))) {
                HDMI_ERR("copy_from_user HDCP_LOADKEY fail!\n");
                *ret = HI_FAILURE;
                break;
            }
            *ret = drv_hdmitx_hdcp_loadkey(hdmi->hdcp, tmp);
            break;

        case HDMITX_IOC_HDCP_GETCAPABILITY:
            *ret = drv_hdmitx_hdcp_get_cap(hdmi->hdcp, &hdcp_cap);
            if (osal_copy_to_user((hi_void *)arg, &hdcp_cap, sizeof(struct hdcp_cap))) {
                HDMI_ERR("copy_to_user HDCP_GETCAPABILITY fail!\n");
                *ret = HI_FAILURE;
            }
            break;

        case HDMITX_IOC_HDCP_SET_REAUTH:
            if (osal_copy_from_user(&tmp, (hi_void *)arg, sizeof(hi_u32))) {
                HDMI_ERR("copy_from_user HDCP_GETSTATUS fail!\n");
                *ret = HI_FAILURE;
            }
            drv_hdmitx_hdcp_set_reauth_times(hdmi->hdcp, tmp);
            break;

        case HDMITX_IOC_HDCP_START:
            if (osal_copy_from_user(&tmp, (hi_void *)arg, sizeof(hi_u32))) {
                HDMI_ERR("copy_from_user HDCP_START fail!\n");
                *ret = HI_FAILURE;
            }
            drv_hdmitx_hdcp_start_auth(hdmi->hdcp, tmp);
            break;

        case HDMITX_IOC_HDCP_STOP:
            drv_hdmitx_hdcp_stop_auth(hdmi->hdcp);
            break;

        case HDMITX_IOC_HDCP_GETSTATUS:
            *ret = drv_hdmitx_hdcp_get_user_status(hdmi->hdcp, &hdcp_status);
            if (osal_copy_to_user((hi_void *)arg, &hdcp_status, sizeof(struct hdcp_usr_status))) {
                HDMI_ERR("copy_to_user HDCP_GETSTATUS fail!\n");
                *ret = HI_FAILURE;
            }
            break;
        default :
            break;
    }
}

void ioc_handle_cec_cmd(struct hisilicon_hdmi *hdmi, unsigned long arg,
    unsigned int cmd, long *ret)
{
    cec_events event = {};
    cec_status status = {};
    hi_u8 device_type;
    cec_msg msg;

    switch (cmd) {
        case HDMITX_IOC_CEC_OPEN:
            *ret = drv_hdmitx_cec_open(hdmi->cec);
            break;

        case HDMITX_IOC_CEC_CLOSE:
            drv_hdmitx_cec_close(hdmi->cec);
            *ret = HI_SUCCESS;
            break;

        case HDMITX_IOC_CEC_READ_EVENTS:
            *ret = drv_hdmitx_cec_read_events(hdmi->cec, &event);
            if (osal_copy_to_user((hi_void *)arg, &event, sizeof(event))) {
                HDMI_ERR("osal_copy_to_user failed!\n");
                *ret = HI_FAILURE;
            }
            break;

        case HDMITX_IOC_CEC_GET_STATUS:
            *ret = drv_hdmitx_cec_get_status(hdmi->cec, &status);
            if (osal_copy_to_user((hi_void *)arg, &status, sizeof(status))) {
                HDMI_ERR("osal_copy_to_user failed!\n");
                *ret = HI_FAILURE;
            }
            break;

        case HDMITX_IOC_CEC_SET_DEVICE_TYPE:
            if (osal_copy_from_user(&device_type, (hi_void *)arg, sizeof(device_type))) {
                HDMI_ERR("osal_copy_from_user failed!\n");
                *ret = HI_FAILURE;
                break;
            }
            *ret = drv_hdmitx_cec_set_device_type(hdmi->cec, device_type);
            break;

        case HDMITX_IOC_CEC_TRANSMIT:
            if (osal_copy_from_user(&msg, (hi_void *)arg, sizeof(msg))) {
                HDMI_ERR("osal_copy_from_user failed!\n");
                *ret = HI_FAILURE;
            }

            *ret = drv_hdmitx_cec_transmit(hdmi->cec, &msg);
            break;

        default:
            break;
    }
}

/* hdcp && edid ioctl functions for userspace */
long hdmi_cdev_ioctl(struct file *filp,
                     unsigned int cmd, unsigned long arg)
{
    long ret = 0;
    struct hisilicon_hdmi *hdmi = NULL;

    if (filp == NULL || filp->private_data == NULL) {
        HDMI_ERR("null ptr or arg is 0\n");
        return HI_FAILURE;
    }
    hdmi = filp->private_data;
    switch (cmd) {
        case HDMITX_IOC_READ_EDID:
            ret = ioc_read_edid(hdmi, arg);
            break;

        case HDMITX_IOC_GET_SINKINFO:
            ret = ioc_get_sinkinfo(hdmi, arg);
            break;

        case HDMITX_IOC_GET_STATUS:
            ret = ioc_get_status(hdmi, arg);
            break;

        case HDMITX_IOC_GET_AVAILCAPBILITY:
            ret = ioc_get_availcapbility(hdmi, arg);
            break;

        case HDMITX_IOC_GET_AVAILMODES:
            ret = ioc_get_availmodes(hdmi, arg);
            break;

        case HDMITX_IOC_SET_FORCEOUTPUT:
            ret = ioc_set_force_output(hdmi, arg);
            break;

        case HDMITX_IOC_HDCP_SETSRM:
            break;

        default:
            break;
    }

    ioc_handle_hdcp_cmd(hdmi, arg, cmd, &ret);

    ioc_handle_cec_cmd(hdmi, arg, cmd, &ret);

    return ret;
}

const struct file_operations g_hdmi_fops = {
    .owner = THIS_MODULE,
    .open = hdmi_cdev_open,
    .unlocked_ioctl = hdmi_cdev_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = hdmi_cdev_ioctl,
#endif
    .release = hdmitx_cdev_close,
};

static hi_s32 hisilicon_hdmi_probe(struct platform_device *pdev)
{
    struct hisilicon_hdmi *hdmi = NULL;
    struct device *dev = &pdev->dev;
    hi_s32 ret;
	 HDMI_ERR("enter hisilicon_hdmi_probe.\n");
    hdmi = osal_kmalloc(HI_ID_HDMITX, sizeof(*hdmi), OSAL_GFP_KERNEL);
    if (!hdmi) {
        ret = HI_FAILURE;
        goto err1;
    }
    if (memset_s(hdmi, sizeof(struct hisilicon_hdmi), 0, sizeof(struct hisilicon_hdmi))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, hdmi);
        return HI_FAILURE;
    }
    hdmi->dev = dev;

    /* Parse dt config */
    ret = hisilicon_hdmi_dt_parse(hdmi, pdev);
    if (ret) {
        dev_err(dev, "hdmi dt parse failed: %d\n", ret);
        goto err1;
    }

    /* Setup misc device */
    ret = snprintf_s(hdmi->name, sizeof(hdmi->name), sizeof(hdmi->name), "hdmitx%d", hdmi->id);
    if (ret < 0) {
        dev_err(dev, "snprintf_s failed!\n");
        goto err1;
    }
    hdmi->miscdev.minor = MISC_DYNAMIC_MINOR;
    hdmi->miscdev.fops = &g_hdmi_fops;
    hdmi->miscdev.name = hdmi->name;

    ret = misc_register(&hdmi->miscdev);
    if (ret) {
        dev_err(dev, "misc_register failed: %d\n", ret);
        goto err1;
    }

    ret = hisilicon_hdmi_init(hdmi);
    if (ret) {
        dev_err(dev, "hdmi_init failed: %d\n", ret);
        goto err2;
    }

    ret = hisilicon_hdmi_add_mapping(hdmi);
    if (ret) {
        dev_err(dev, "add hdmi mapping failed: %d\n", ret);
        goto err3;
    }

    ret = drv_hdmitx_proc_init(hdmi);
    if (ret < 0) {
        dev_err(dev, "add hdmi proc failed: %d\n", ret);
        goto err4;
    }

    ret = drv_hdmitx_debug_init(hdmi);
    if (ret != HI_SUCCESS) {
        dev_err(dev, "add hdmi debug failed: %d\n", ret);
        goto err5;
    }

    ret = drv_hdmitx_sysfs_init(hdmi);
    if (ret) {
        dev_err(dev, "add hdmi sysfs failed: %d\n", ret);
        goto err6;
    }

    platform_set_drvdata(pdev, hdmi);
    /**
     *  Enalble IRQ & workqueue to handle hotplug event .
     *  This operation should do after all internal modules have initial,
     *  or it may occor some error.
     */
    hdmi_connector_enable_hpd(hdmi, hdmi->connector);
    return ret;

err6:
    drv_hdmitx_debug_deinit(hdmi);
err5:
    drv_hdmitx_proc_deinit(hdmi);
err4:
    hisilicon_hdmi_delect_mapping();
err3:
    hisilicon_hdmi_deinit(hdmi);
err2:
    misc_deregister(&hdmi->miscdev);
err1:
    if (hdmi != HI_NULL) {
        osal_kfree(HI_ID_HDMITX, hdmi);
    }
    return ret;
}

static hi_s32 hisilicon_hdmi_remove(struct platform_device *pdev)
{
    struct hisilicon_hdmi *hdmi = platform_get_drvdata(pdev);

    drv_hdmitx_sysfs_exit(hdmi);
    drv_hdmitx_debug_deinit(hdmi);
    drv_hdmitx_proc_deinit(hdmi);
    hisilicon_hdmi_delect_mapping();
    hisilicon_hdmi_deinit(hdmi);
    misc_deregister(&hdmi->miscdev);
    osal_kfree(HI_ID_HDMITX, hdmi);

    return 0;
}

static const struct of_device_id g_hisilicon_hdmi_dt_ids[] = {
    { .compatible = "hisilicon, hdmi-tx" },
    {},
};
MODULE_DEVICE_TABLE(of, g_hisilicon_hdmi_dt_ids);

struct platform_driver g_hisilicon_hdmi_driver = {
    .probe = hisilicon_hdmi_probe,
    .remove = hisilicon_hdmi_remove,
    .driver = {
        .name = "hisilicon-hdmi",
        .of_match_table = g_hisilicon_hdmi_dt_ids,
    },
};

static struct hisilicon_hdmi *hdmi_id_to_hisilicon_hdmi(hi_s32 hdmi_id)
{
    hi_s32 i;

    for (i = 0; i < g_hdmi_cnt; i++) {
        if (g_mapping[i].hdmi_id == hdmi_id) {
            break;
        }
    }
    if (i == g_hdmi_cnt) {
        return NULL;
    }

    return g_mapping[i].hdmi;
}

/*
 * vo intf and ao intf are defined dependently with hdmi
 * so and convertion is needed here.
 */
static hi_s32 intf_id_to_hdmi_id(hi_s32 id)
{
    if (id == AO_INTF_HDMI0 || id == DISP_INFT_HDMI0) {
        return 0;
    }

    if (id == AO_INTF_HDMI1 || id == DISP_INFT_HDMI1) {
        return 1;
    }

    return MAX_HDMI_ID;
}

static hi_s32 hdmi_module_vo_suspend_func(hi_s32 vo_intf_id, void *data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (hdmi == NULL) {
        return -EINVAL;
    }

    if (!hdmi->attached) {
        return -ENODEV;
    }

    hdmi->vo_ops->suspend(hdmi, data);

    return 0;
}

static hi_s32 hdmi_module_vo_resume_func(hi_s32 vo_intf_id, void *data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (hdmi == NULL) {
        return -EINVAL;
    }

    if (!hdmi->attached) {
        return -ENODEV;
    }

    return hdmi->vo_ops->resume(hdmi, data);
}

static hi_s32 hdmi_module_vo_attach_func(hi_s32 vo_intf_id, void *data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi_id = %d, id is invalid, please check param input\n", hdmi_id);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        HDMI_ERR("hdmi uninit, please check hi_hdmi.ko loaded\n");
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi->attached = true;

    return hdmi->vo_ops->attach(hdmi, data);
}

static hi_s32 hdmi_module_vo_detach_func(hi_s32 vo_intf_id, void *data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi_id = %d, id is invalid, please check param input\n", hdmi_id);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        HDMI_ERR("hdmi uninit, please check hi_hdmi.ko loaded\n");
        return HDMI_ERR_MODULE_UNINIT;
    }

    hdmi->attached = false;

    return hdmi->vo_ops->detach(hdmi, data);
}

static hi_s32 hdmi_module_vo_mode_validate_func(hi_s32 vo_intf_id, hi_s32 mode, struct hi_display_mode *mode_data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi_id = %d, id is invalid, please check param input\n", hdmi_id);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        HDMI_ERR("hdmi uninit, please check hi_hdmi.ko loaded\n");
        return HDMI_ERR_MODULE_UNINIT;
    }

    return hdmi->vo_ops->mode_validate(hdmi, mode, mode_data);
}

static hi_s32 hdmi_module_vo_prepare_func(hi_s32 vo_intf_id, hi_s32 mode,
                                          struct hi_display_mode *mode_data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi_id = %d, id is invalid, please check param input\n", hdmi_id);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        HDMI_ERR("hdmi uninit, please check hi_hdmi.ko loaded\n");
        return HDMI_ERR_MODULE_UNINIT;
    }

    return hdmi->vo_ops->prepare(hdmi, mode, mode_data);
}

static hi_s32 hdmi_module_vo_mode_set_func(hi_s32 vo_intf_id, hi_s32 mode,
                                           struct hi_display_mode *mode_data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi_id = %d, id is invalid, please check param input\n", hdmi_id);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        HDMI_ERR("hdmi uninit, please check hi_hdmi.ko loaded\n");
        return HDMI_ERR_MODULE_UNINIT;
    }

    if (!hdmi->attached) {
        return -ENODEV;
    }

    return hdmi->vo_ops->mode_set(hdmi, mode, mode_data);
}

static hi_s32 hdmi_module_vo_atomic_mode_set_func(hi_s32 vo_intf_id, hi_s32 mode,
                                                  struct hi_display_mode *mode_data)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi_id = %d, id is invalid, please check param input\n", hdmi_id);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        HDMI_ERR("hdmi uninit, please check hi_hdmi.ko loaded\n");
        return HDMI_ERR_MODULE_UNINIT;
    }

    if (!hdmi->attached) {
        return -ENODEV;
    }

    return hdmi->vo_ops->atomic_mode_set(hdmi, mode, mode_data);
}

static hi_s32 hdmi_module_vo_display_on_func(hi_s32 vo_intf_id)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi_id = %d, id is invalid, please check param input\n", hdmi_id);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        HDMI_ERR("hdmi uninit, please check hi_hdmi.ko loaded\n");
        return HDMI_ERR_MODULE_UNINIT;
    }

    if (!hdmi->attached) {
        return -ENODEV;
    }

    return hdmi->vo_ops->display_on(hdmi);
}

static hi_s32 hdmi_module_vo_display_off_func(hi_s32 vo_intf_id)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(vo_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        return -EINVAL;
    }

    if (!hdmi->attached) {
        return -ENODEV;
    }

    return hdmi->vo_ops->display_off(hdmi);
}

static void color_space_conv(hi_drv_color_descript color_descript,
    struct color_info *color)
{
    switch (color_descript.color_primary) {
        case HI_DRV_COLOR_PRIMARY_BT601_525:
        case HI_DRV_COLOR_PRIMARY_BT601_625:
            color->colorimetry = HDMI_COLORIMETRY_ITU601;
            break;
        case HI_DRV_COLOR_PRIMARY_BT709:
            color->colorimetry = HDMI_COLORIMETRY_ITU709;
            break;
        case HI_DRV_COLOR_PRIMARY_BT2020:
            color->colorimetry = HDMI_COLORIMETRY_2020_NON_CONST_LUMINOUS;
            break;
        default:
            color->colorimetry = HDMI_COLORIMETRY_ITU709;
            break;
    }

    switch (color_descript.quantify_range) {
        case HI_DRV_COLOR_LIMITED_RANGE:
            color->rgb_quantization = HDMI_RGB_QUANTIZEION_LIMITED;
            color->ycc_quantization = HDMI_YCC_QUANTIZEION_LIMITED;
            break;
        case HI_DRV_COLOR_FULL_RANGE:
            color->rgb_quantization = HDMI_RGB_QUANTIZEION_FULL;
            color->ycc_quantization = HDMI_YCC_QUANTIZEION_FULL;
            break;
        default:
            color->rgb_quantization = HDMI_RGB_QUANTIZEION_LIMITED;
            color->ycc_quantization = HDMI_YCC_QUANTIZEION_LIMITED;
            break;
    }
}

static void disp_stereo_mode_conv(hi_drv_disp_stereo_mode stereo_mode,
                                  hi_u8 *mode_3d)
{
    switch (stereo_mode) {
        case HI_DRV_DISP_STEREO_NONE:
            *mode_3d = HDMI_3D_NONE;
            break;
        case HI_DRV_DISP_STEREO_FRAME_PACKING:
            *mode_3d = HDMI_3D_FRAME_PACKETING;
            break;
        case HI_DRV_DISP_STEREO_SBS_HALF:
            *mode_3d = HDMI_3D_SIDE_BY_SIDE_HALF;
            break;
        case HI_DRV_DISP_STEREO_TAB:
            *mode_3d = HDMI_3D_TOP_AND_BOTTOM;
            break;
        case HI_DRV_DISP_STEREO_FIELD_ALT:
            *mode_3d = HDMI_3D_FIELD_ALTERNATIVE;
            break;
        case HI_DRV_DISP_STEREO_LINE_ALT:
            *mode_3d = HDMI_3D_LINE_ALTERNATIVE;
            break;
        case HI_DRV_DISP_STEREO_SBS_FULL:
            *mode_3d = HDMI_3D_SIDE_BY_SIDE_FULL;
            break;
        case HI_DRV_DISP_STEREO_L_DEPTH:
            *mode_3d = HDMI_3D_L_DEPTH;
            break;
        case HI_DRV_DISP_STEREO_L_DEPTH_G_DEPTH:
            *mode_3d = HDMI_3D_L_DEPTH_GRAPHICS_DEPTH;
            break;
        default:
            *mode_3d = HDMI_3D_NONE;
            break;
    }
}

static void pixel_bitwidth_conv(hi_drv_pixel_bitwidth bit_width,
                                hi_u32 *color_depth)
{
    switch (bit_width) {
        case HI_DRV_PIXEL_BITWIDTH_8BIT:
            *color_depth = HDMI_BPC_24;
            break;
        case HI_DRV_PIXEL_BITWIDTH_10BIT:
            *color_depth = HDMI_BPC_30;
            break;
        case HI_DRV_PIXEL_BITWIDTH_12BIT:
            *color_depth = HDMI_BPC_36;
            break;
        default:
            *color_depth = HDMI_BPC_24;
            break;
    }
}

static void disp_out_type_conv(hi_drv_disp_out_type type,
                               hi_u32 *hdr_type)
{
    switch (type) {
        case HI_DRV_DISP_TYPE_NORMAL:
            *hdr_type = HDMI_HDR_MODE_SDR;
            break;
        case HI_DRV_DISP_TYPE_SDR_CERT:
            *hdr_type = HDMI_HDR_MODE_STATIC_TRD_SDR;
            break;
        case HI_DRV_DISP_TYPE_DOLBY:
            *hdr_type = HDMI_HDR_MODE_DOLBY_V1;
            break;
        case HI_DRV_DISP_TYPE_HDR10:
            *hdr_type = HDMI_HDR_MODE_STATIC_ST2084;
            break;
        case HI_DRV_DISP_TYPE_HDR10_CERT:
            *hdr_type = HDMI_HDR_MODE_STATIC_TRD_HDR;
            break;
        case HI_DRV_DISP_TYPE_HLG:
            *hdr_type = HDMI_HDR_MODE_STATIC_HLG;
            break;
        case HI_DRV_DISP_TYPE_DOLBY_LL:
            *hdr_type = HDMI_HDR_MODE_DOLBY_V2;
            break;
        default:
            break;
    }
}

static void disp_timing_to_detail_info(hi_drv_disp_timing *src,
                                       struct timing_detail_info *dst)
{
    dst->htotal = src->hact + src->hbb + src->hfb;
    dst->hactive = src->hact;
    dst->hfront = src->hfb;
    dst->hsync = src->hpw;
    dst->hback = src->hbb - src->hpw;
    dst->vtotal = src->vact + src->vbb + src->vfb;
    dst->vactive = src->vact;
    dst->vfront = src->vfb;
    dst->vsync = src->vpw;
    dst->vback = src->vbb - src->vpw;
    dst->refresh_rate = src->refresh_rate;
}

static void disp_intf_info2hi_display_mode(hi_drv_disp_intf_info *src,
                                           struct hi_display_mode *dst)
{
    hi_u32 vic = src->disp_timing.static_timing.vic_num;
    struct hdmi_timing_mode *timing_mode = NULL;
    struct hdmi_in_data *dst_in = &dst->timing_data.in;
    struct hdmi_out_data *dst_out = &dst->timing_data.out;
    struct hdmi_hdr_data *dst_hdr = &dst->hdr_data;
    struct hdmi_vrr_data *dst_vrr = &dst->vrr_data;
    struct color_info color;
    hi_drv_disp_timing *disp_timing =
        &src->disp_timing.static_timing.timing;

    dst_in->active_aspect_present = false;
    dst_in->active_aspect_ratio = HDMI_ACT_ASP_RATIO_ATSC_SAME_PIC;
    dst_in->bar_present = HDMI_BAR_DATA_NO_PRESENT;
    dst_in->bottom_bar = 0;
    dst_in->top_bar = 0;
    dst_in->right_bar = 0;
    dst_in->left_bar = 0;
    dst_in->it_content_type = HDMI_IT_CONTENT_GRAPHYICS;
    dst_in->it_content_valid = false;
    dst_in->picture_scal = 0;
    dst_in->scan_info = HDMI_SCAN_OVER_SCAN;

    disp_out_type_conv(src->out_type, &dst_hdr->hdr_mode_type);

    if (src->disp_timing.dynamic_timing.vrr_cfg.enable) {
        dst_vrr->vrr_mode_type = HDMI_VRR_MODE_VRR;
    } else if (src->disp_timing.dynamic_timing.qft_cfg.enable) {
        dst_vrr->vrr_mode_type = HDMI_VRR_MODE_QFT;
    } else if (src->disp_timing.dynamic_timing.qms_cfg.enable) {
        dst_vrr->vrr_mode_type = HDMI_VRR_MODE_QMS;
    } else {
        dst_vrr->vrr_mode_type = HDMI_VRR_MODE_ALLM;
    }

    dst_in->de_pol = src->disp_timing.static_timing.timing.idv;
    dst_in->h_sync_pol = src->disp_timing.static_timing.timing.ihs;
    dst_in->v_sync_pol = src->disp_timing.static_timing.timing.ivs;

    disp_timing_to_detail_info(disp_timing, &dst_in->detail);

    dst_in->pixel_clock = src->disp_timing.static_timing.timing.pix_freq;
    dst_in->pixel_repeat = src->disp_timing.static_timing.pix_repeat - 1;
    disp_stereo_mode_conv(src->disp_timing.static_timing.disp_3d_mode,
        &dst_in->mode_3d);

    timing_mode = vic2timing_mode(vic);
    dst_in->picture_aspect_ratio = (timing_mode == NULL) ?
        HDMI_PIC_ASPECT_RATIO_16_9 : timing_mode->pic_aspect;

    color_space_conv(src->in_info.color_space, &color);

    dst_in->vic = vic;
    pixel_bitwidth_conv(src->in_info.data_width, &dst_in->color_depth);
    dst_in->color.color_format = src->in_info.pixel_format;
    dst_in->color.colorimetry = color.colorimetry;
    dst_in->color.ycc_quantization = HDMI_YCC_QUANTIZEION_LIMITED;
    dst_in->color.rgb_quantization = HDMI_RGB_QUANTIZEION_DEFAULT;
    pixel_bitwidth_conv(src->out_info.data_width, &dst_out->color_depth);
    dst_out->color.color_format = src->out_info.pixel_format;
}

static hi_void picture_aspect_conv(hi_drv_aspect_ratio src,
    enum hdmi_picture_aspect *dst)
{
    if (dst == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    if (src.aspect_ratio_w == WIDTH_4 &&
        src.aspect_ratio_h == HEIGHT_3) {
        *dst = HDMI_PICTURE_ASPECT_4_3;
    } else if (src.aspect_ratio_w == WIDTH_16 &&
        src.aspect_ratio_h == HEIGHT_9) {
        *dst = HDMI_PICTURE_ASPECT_16_9;
    } else if (src.aspect_ratio_w == WIDTH_64 &&
        src.aspect_ratio_h == HEIGHT_27) {
        *dst = HDMI_PICTURE_ASPECT_64_27;
    } else if (src.aspect_ratio_w == WIDTH_256 &&
        src.aspect_ratio_h == HEIGHT_135) {
        *dst = HDMI_PICTURE_ASPECT_256_135;
    } else {
        *dst = HDMI_PICTURE_ASPECT_RESERVED;
    }

    return;
}

static hi_void disp_avi_infoframe2hdmi_avi_infoframe(
    hi_drv_disp_avi_infoframe *src, struct hdmi_avi_infoframe *dst)
{
    struct color_info color;
    hi_u32 colorimetry;
    hi_u32 extended_colorimetry;

    if (src == NULL || dst == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    color_space_conv(src->color_space, &color);
    colorimetry = color.colorimetry & 0xf;
    extended_colorimetry = (color.colorimetry >> 4) & 0xf; /* The upper 4 bits is extended colorimetry. */
    dst->pixel_repeat = src->pixel_repeat - 1;
    dst->video_code = (src->vic > 255) ? 0 : (src->vic & 0xff); /* Vic > 255 is not CEA timing. */

    /*
     * The video code must be 0 in the avi infoframe, when the timing
     * is HDMI_3840X2160P24_16_9(93), HDMI_3840X2160P25_16_9(94),
     * HDMI_3840X2160P30_16_9(95), HDMI_4096X2160P24_256_135(98) at 2D mode.
     */
    if ((dst->video_code == VIC_3840X2160P24_16_9 ||
        dst->video_code == VIC_3840X2160P25_16_9 ||
        dst->video_code == VIC_3840X2160P30_16_9 ||
        dst->video_code == VIC_4096X2160P24_256_135) &&
        src->disp_3d_mode == HI_DRV_DISP_STEREO_NONE) {
        dst->video_code = 0;
    }

    picture_aspect_conv(src->picture_aspect, &dst->picture_aspect);

    dst->content_type = (enum hdmi_content_type)src->content_type;
    dst->itc = false;
    dst->active_aspect = (enum hdmi_active_aspect)src->active_aspect;
    dst->scan_mode = (enum hdmi_scan_mode)src->scan_mode;
    dst->colorspace = (enum hdmi_colorspace)src->pix_format;
    dst->colorimetry = colorimetry;
    dst->extended_colorimetry = extended_colorimetry;
    dst->quantization_range = color.rgb_quantization;
    dst->ycc_quantization_range = color.ycc_quantization;
    dst->top_bar = src->top_bar;
    dst->bottom_bar = src->bottom_bar;
    dst->left_bar = src->left_bar;
    dst->right_bar = src->right_bar;
    dst->nups = (enum hdmi_nups)src->nups;

    /* Y2 = 1 or vic >= 128, version shall use 3 */
    if (dst->video_code > VIC_5120X2160P100_64_27) {
        dst->version = 3; /* Avi infoframe version need be 3, when vic > 127. */
    } else if (dst->colorimetry == HDMI_COLORIMETRY_EXTENDED &&
        dst->extended_colorimetry == HDMI_EXTENDED_COLORIMETRY_RESERVED) {
        dst->version = 4; /* Avi infoframe version need be 4, when extended colorimetry is reserved. */
    }

    return;
}

static hi_void video_hdr10_info2hdmi_drm_infoframe(
    hi_drv_hdr_static_metadata *src, struct hdmi_drm_infoframe *dst)
{
    struct hdmi_meta_descriptor_1st *type1 = NULL;
    hi_drv_hdr_mastering_display_info *mastering_info = NULL;
    hi_drv_hdr_content_light_level *content_info = NULL;

    if (src == NULL || dst == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    type1 = &dst->meta_descriptor.type1;
    mastering_info = &src->mastering_info;
    content_info = &src->content_info;

    dst->eotf_type = HDMI_EOTF_SMPTE_ST_2084;
    dst->metadata_type = HDMI_HDR_METADATA_ID_0;
    type1->primaries0_x = mastering_info->display_primaries_x[INDEX_0];
    type1->primaries0_y = mastering_info->display_primaries_y[INDEX_0];
    type1->primaries1_x = mastering_info->display_primaries_x[INDEX_1];
    type1->primaries1_y = mastering_info->display_primaries_y[INDEX_1];
    type1->primaries2_x = mastering_info->display_primaries_x[INDEX_2];
    type1->primaries2_y = mastering_info->display_primaries_y[INDEX_2];
    type1->white_point_x = mastering_info->white_point_x;
    type1->white_point_y = mastering_info->white_point_y;
    type1->max_luminance = mastering_info->max_display_mastering_luminance;
    type1->min_luminance = mastering_info->min_display_mastering_luminance;
    type1->max_light_level = content_info->max_content_light_level;
    type1->average_light_level = content_info->max_pic_average_light_level;

    return;
}

static hi_void video_hlg_info2hdmi_drm_infoframe(
    hi_drv_hdr_hlg_metadata *src, struct hdmi_drm_infoframe *dst)
{
    struct hdmi_meta_descriptor_1st *type1 = NULL;
    hi_drv_hdr_mastering_display_info *mastering_info = NULL;
    hi_drv_hdr_content_light_level *content_info = NULL;

    if (src == NULL || dst == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    type1 = &dst->meta_descriptor.type1;
    mastering_info = &src->mastering_info;
    content_info = &src->content_info;

    dst->eotf_type = HDMI_EOTF_HLG;
    dst->metadata_type = HDMI_HDR_METADATA_ID_0;
    type1->primaries0_x = mastering_info->display_primaries_x[INDEX_0];
    type1->primaries0_y = mastering_info->display_primaries_y[INDEX_0];
    type1->primaries1_x = mastering_info->display_primaries_x[INDEX_1];
    type1->primaries1_y = mastering_info->display_primaries_y[INDEX_1];
    type1->primaries2_x = mastering_info->display_primaries_x[INDEX_2];
    type1->primaries2_y = mastering_info->display_primaries_y[INDEX_2];
    type1->white_point_x = mastering_info->white_point_x;
    type1->white_point_y = mastering_info->white_point_y;
    type1->max_luminance = mastering_info->max_display_mastering_luminance;
    type1->min_luminance = mastering_info->min_display_mastering_luminance;
    type1->max_light_level = content_info->max_content_light_level;
    type1->average_light_level = content_info->max_pic_average_light_level;

    return;
}

static hi_s32 avi_infoframe_pack(hi_drv_disp_avi_infoframe *frame,
    hi_u8 *buffer, hi_u32 size)
{
    struct hdmi_avi_infoframe avi;
    ssize_t err;

    if (frame == NULL || buffer == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    err = hdmi_avi_infoframe_init(&avi);
    if (err < 0) {
        HDMI_ERR("Failed to setup avi infoframe: %zd\n", err);
        return err;
    }

    disp_avi_infoframe2hdmi_avi_infoframe(frame, &avi);

    err = hdmi_avi_infoframe_pack(&avi, buffer, size);
    if (err < 0) {
        HDMI_ERR("Failed to pack AVI infoframe: %zd\n", err);
        return err;
    }

    return 0;
}

static hi_s32 hdr10_infoframe_pack(hi_drv_hdr_static_metadata *frame,
    hi_u8 *buffer, hi_u32 size)
{
    struct hdmi_drm_infoframe drm;
    hi_s32 err;

    if (frame == NULL || buffer == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    err = hdmi_drm_infoframe_init(&drm);
    if (err < 0) {
        HDMI_ERR("Failed to setup avi infoframe: %zd\n", err);
        return err;
    }

    video_hdr10_info2hdmi_drm_infoframe(frame, &drm);

    err = hdmi_drm_infoframe_pack(&drm, buffer, size);
    if (err < 0) {
        HDMI_ERR("Failed to pack AVI infoframe: %zd\n", err);
        return err;
    }

    return 0;
}

static hi_s32 hlg_infoframe_pack(hi_drv_hdr_hlg_metadata *frame,
    hi_u8 *buffer, hi_u32 size)
{
    struct hdmi_drm_infoframe drm;
    hi_s32 err;

    if (frame == NULL || buffer == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    err = hdmi_drm_infoframe_init(&drm);
    if (err < 0) {
        HDMI_ERR("Failed to setup avi infoframe: %zd\n", err);
        return err;
    }

    video_hlg_info2hdmi_drm_infoframe(frame, &drm);

    err = hdmi_drm_infoframe_pack(&drm, buffer, size);
    if (err < 0) {
        HDMI_ERR("Failed to pack AVI infoframe: %zd\n", err);
        return err;
    }

    return 0;
}

static hi_s32 hdmi_module_disp_intf_suspend(hi_drv_disp_intf intf,
                                            hi_void *pdev, hi_void *state)
{
    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        return -EINVAL;
    }

    return hdmi_module_vo_suspend_func(intf.un_intf.hdmi, state);
}

static hi_s32 hdmi_module_disp_intf_resume(hi_drv_disp_intf intf,
                                           hi_void *pdev, hi_void *state)
{
    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        return -EINVAL;
    }

    return hdmi_module_vo_resume_func(intf.un_intf.hdmi, state);
}

static hi_s32 hdmi_module_disp_intf_check_validate(hi_drv_disp_intf intf,
                                                   hi_u32 set_mode, hi_drv_disp_intf_info *intf_info)
{
    struct hi_display_mode display_mode;

    if (intf_info == NULL) {
        HDMI_ERR("hdmi id = %d, param intf_info is NULL, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (memset_s(&display_mode, sizeof(struct hi_display_mode), 0, sizeof(struct hi_display_mode))) {
        HDMI_ERR("memset_s fail.\n");
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        HDMI_ERR("hdmi id = %d, intf_type not hdmi, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    disp_intf_info2hi_display_mode(intf_info, &display_mode);

    return hdmi_module_vo_mode_validate_func(intf.un_intf.hdmi,
                                             set_mode, &display_mode);
}

static hi_s32 hdmi_module_disp_intf_enable(hi_drv_disp_intf intf, hi_bool enable)
{
    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        HDMI_ERR("hdmi id = %d, intf_type not hdmi, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (enable) {
        return hdmi_module_vo_display_on_func(intf.un_intf.hdmi);
    } else {
        return hdmi_module_vo_display_off_func(intf.un_intf.hdmi);
    }
}

static hi_s32 hdmi_module_disp_intf_detach(hi_drv_disp_intf intf, void *data)
{
    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        HDMI_ERR("hdmi id = %d, intf_type not hdmi, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    return hdmi_module_vo_detach_func(intf.un_intf.hdmi, data);
}

static hi_s32 hdmi_module_disp_intf_attach(hi_drv_disp_intf intf, void *data)
{
    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        HDMI_ERR("hdmi id = %d, intf_type not hdmi, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    return hdmi_module_vo_attach_func(intf.un_intf.hdmi, data);
}

static hi_s32 hdmi_module_disp_intf_prepare(hi_drv_disp_intf intf,
                                            hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    struct hi_display_mode display_mode;

    if (memset_s(&display_mode, sizeof(struct hi_display_mode), 0, sizeof(struct hi_display_mode))) {
        HDMI_ERR("memset_s fail.\n");
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        HDMI_ERR("hdmi id = %d, intf_type not hdmi, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (intf_info == NULL) {
        HDMI_ERR("hdmi id = %d, param intf_info is NULL, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    disp_intf_info2hi_display_mode(intf_info, &display_mode);

    return hdmi_module_vo_prepare_func(intf.un_intf.hdmi,
                                       mode, &display_mode);
}

static hi_s32 hdmi_module_disp_intf_config(hi_drv_disp_intf intf,
                                           hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    struct hi_display_mode display_mode;

    if (memset_s(&display_mode, sizeof(struct hi_display_mode), 0, sizeof(struct hi_display_mode))) {
        HDMI_ERR("memset_s fail.\n");
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        HDMI_ERR("hdmi id = %d, intf_type not hdmi, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (intf_info == NULL) {
        HDMI_ERR("hdmi id = %d, param intf_info is NULL, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    disp_intf_info2hi_display_mode(intf_info, &display_mode);

    return hdmi_module_vo_mode_set_func(intf.un_intf.hdmi,
                                        mode, &display_mode);
}

static hi_s32 hdmi_module_disp_intf_atomic_config(hi_drv_disp_intf intf,
                                                  hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    struct hi_display_mode display_mode;

    if (memset_s(&display_mode, sizeof(struct hi_display_mode), 0, sizeof(struct hi_display_mode))) {
        HDMI_ERR("memset_s fail.\n");
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (intf.intf_type != HI_DRV_DISP_INTF_TYPE_HDMI) {
        HDMI_ERR("hdmi id = %d, intf_type not hdmi, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    if (intf_info == NULL) {
        HDMI_ERR("hdmi id = %d, param intf_info is NULL, please check param input\n", intf.un_intf.hdmi);
        return HDMI_ERR_INPUT_PARAM_INVALID;
    }

    disp_intf_info2hi_display_mode(intf_info, &display_mode);

    return hdmi_module_vo_atomic_mode_set_func(intf.un_intf.hdmi,
                                               mode, &display_mode);
}

static hi_s32 hdmi_module_disp_intf_avi_infoframe_pack(
    hi_drv_disp_avi_infoframe *frame, hi_u8 *buffer, hi_u32 size)
{
    if (frame == NULL || buffer == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    return avi_infoframe_pack(frame, buffer, size);
}

static hi_s32 hdmi_module_disp_intf_hdr10_infoframe_pack(
    hi_drv_hdr_static_metadata *frame, hi_u8 *buffer, hi_u32 size)
{
    if (frame == NULL || buffer == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    return hdr10_infoframe_pack(frame, buffer, size);
}

static hi_s32 hdmi_module_disp_intf_hlg_infoframe_pack(
    hi_drv_hdr_hlg_metadata *frame, hi_u8 *buffer, hi_u32 size)
{
    if (frame == NULL || buffer == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return -EINVAL;
    }

    return hlg_infoframe_pack(frame, buffer, size);
}

disp_intf_func g_module_vo_ops = {
    .intf_suspend = hdmi_module_disp_intf_suspend,
    .intf_resume = hdmi_module_disp_intf_resume,
    .intf_check_validate = hdmi_module_disp_intf_check_validate,
    .intf_enable = hdmi_module_disp_intf_enable,
    .intf_detach = hdmi_module_disp_intf_detach,
    .intf_attach = hdmi_module_disp_intf_attach,
    .intf_prepare = hdmi_module_disp_intf_prepare,
    .intf_config = hdmi_module_disp_intf_config,
    .intf_atomic_config = hdmi_module_disp_intf_atomic_config,
    .intf_avi_infoframe_pack = hdmi_module_disp_intf_avi_infoframe_pack,
    .intf_hdr10_infoframe_pack = hdmi_module_disp_intf_hdr10_infoframe_pack,
    .intf_hlg_infoframe_pack = hdmi_module_disp_intf_hlg_infoframe_pack,
};

static hi_s32 hdmi_module_ao_get_eld_func(hi_s32 ao_intf_id, void *data,
                                          hi_u8 *buf, size_t len)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(ao_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        return -EINVAL;
    }

    return hdmi->ao_ops->get_eld(hdmi, data, buf, len);
}

static hi_s32 hdmi_module_ao_hw_params_validate_func(hi_s32 ao_intf_id,
                                                     struct ao_attr *attr)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(ao_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        return -EINVAL;
    }

    return hdmi->ao_ops->hw_params_validate(hdmi, attr);
}

static hi_s32 hdmi_module_ao_hw_params_func(hi_s32 ao_intf_id, struct ao_attr *attr)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(ao_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        return -EINVAL;
    }

    return hdmi->ao_ops->hw_params(hdmi, attr);
}

static hi_s32 hdmi_module_ao_digital_mute_func(hi_s32 ao_intf_id, void *data,
                                               hi_bool enable)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(ao_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        return -EINVAL;
    }

    return hdmi->ao_ops->digital_mute(hdmi, data, enable);
}

static hi_s32 hdmi_module_ao_hpd_detect_func(hi_s32 ao_intf_id, hi_u32 *status)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(ao_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        HDMI_ERR("hdmi id exceed max id.\n");
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (hdmi == NULL) {
        HDMI_ERR("hdmi not init.\n");
        return -EINVAL;
    }

    if (status == NULL) {
        HDMI_ERR("input params is NULL pointer.\n");
        return -EINVAL;
    }

    return hdmi->ao_ops->hpd_detect(hdmi, status);
}

static hi_s32 hdmi_module_ao_register_notifier_func(hi_s32 ao_intf_id,
                                                    struct notifier_block *nb)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(ao_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        return -EINVAL;
    }

    return hdmi->ao_ops->register_notifier(hdmi, nb);
}

static hi_s32 hdmi_module_ao_unregister_notifier_func(hi_s32 ao_intf_id,
                                                      struct notifier_block *nb)
{
    struct hisilicon_hdmi *hdmi = NULL;
    hi_s32 hdmi_id;

    hdmi_id = intf_id_to_hdmi_id(ao_intf_id);
    if (hdmi_id == MAX_HDMI_ID) {
        return -EINVAL;
    }

    hdmi = hdmi_id_to_hisilicon_hdmi(hdmi_id);
    if (!hdmi) {
        return -EINVAL;
    }

    return hdmi->ao_ops->unregister_notifier(hdmi, nb);
}

struct hi_ao_intf_module_ops g_module_ao_ops = {
    .get_eld = hdmi_module_ao_get_eld_func,
    .hw_params_validate = hdmi_module_ao_hw_params_validate_func,
    .hw_params = hdmi_module_ao_hw_params_func,
    .digital_mute = hdmi_module_ao_digital_mute_func,
    .hpd_detect = hdmi_module_ao_hpd_detect_func,
    .register_notifier = hdmi_module_ao_register_notifier_func,
    .unregister_notifier = hdmi_module_ao_unregister_notifier_func,
};

static struct hi_hdmi_ext_intf g_hdmi_ext_data;

hi_s32 __init hdmi_module_init(void)
{
    hi_s32 i;
    hi_s32 ret;

    g_hdmi_ext_data.vo_data = (void *)&g_module_vo_ops;
    g_hdmi_ext_data.ao_data = (void *)&g_module_ao_ops;

    ret = osal_exportfunc_register(HI_ID_HDMITX, "HI_HDMITX", (void *)&g_hdmi_ext_data);
    if (ret) {
        HDMI_ERR("register module failed, ret :%d\n", ret);
        return ret;
    }

    /* Init the mapping */
    g_hdmi_cnt = 0;
    for (i = 0; i < MAX_HDMI_ID; i++) {
        g_mapping[i].hdmi_id = -1;
        g_mapping[i].hdmi = NULL;
    }

#if defined(CHIP_TYPE_HI3796CV300)
    hdmi_hw_ctrl_phy_mapping_set(CRLT_PHY_MAP_MODE_CROSS);
#elif defined(CHIP_TYPE_HI3751V900)
    hdmi_hw_ctrl_phy_mapping_set(CRLT_PHY_MAP_MODE_NO_CROSS);
#endif
    ret = platform_driver_register(&g_hisilicon_hdmi_driver);
    if (ret) {
        HDMI_ERR("driver register failed, ret :%d\n", ret);
        return ret;
    }
    HI_PRINT("Load hi_hdmitx.ko success.\t(%s)\n", VERSION_STRING);

    return ret;
}

static void __exit hdmi_module_exit(void)
{
    osal_exportfunc_unregister(HI_ID_HDMITX);
    platform_driver_unregister(&g_hisilicon_hdmi_driver);
    HI_PRINT("Unload hi_hdmitx.ko success.\t(%s)\n", VERSION_STRING);
}

#ifdef MODULE
module_init(hdmi_module_init);
module_exit(hdmi_module_exit);
#else
int drv_hdmi_mod_init(void)
{
    return hdmi_module_init();
}
EXPORT_SYMBOL(drv_hdmi_mod_init);

void drv_hdmi_mod_exit(void)
{
    hdmi_module_exit();
}
EXPORT_SYMBOL(drv_hdmi_mod_exit);

#endif

MODULE_DESCRIPTION("Hisilicon HDMI Driver");
MODULE_LICENSE("GPL v2");
