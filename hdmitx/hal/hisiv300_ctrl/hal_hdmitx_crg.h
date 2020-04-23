/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of driver hdmitx crg
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_CRG_H__
#define __HAL_HDMITX_CRG_H__

struct hisilicon_hdmi;

struct hdmi_crg {
    struct hisilicon_hdmi *parent;
    void *hdmi_regs;
    void *ctrl_crg_regs;
    void *phy_crg_regs;
    void *ppll_crg_regs;
    void *ppll_state_regs;
    int dsc_enable;
};

struct crg_config_data {
    int encoder_mode;
    int pixel_rate; /* in khz */
    bool yuv_420;
    int color_depth;
    bool dsc_enable;
    int hctotal;
    int htotal;
    /* Use phy tmds clock for dsc and hdmi top clock as default */
    bool prefer_ppll;
};

/*
 * hdmi_crg_init - create an crg structure based on the hdmi and init it
 * @hdmi: The base hdmi structure
 * Based on the hdmi structure, after device tree parsing, we get the
 * configuration for the crg. then we can create a new crg or just
 * reference a crg, after that, init the crg hardware if nessary.
 * Return: The targe crg or null if the required crg is not specified.
 */
struct hdmi_crg *hdmi_crg_init(struct hisilicon_hdmi *hdmi);
void hdmi_crg_deinit(struct hdmi_crg *crg);
void hdmi_crg_clk_enable(struct hdmi_crg *crg, hi_bool enable);
void hdmi_crg_config(struct hdmi_crg *crg, struct crg_config_data *cfg);

#endif
