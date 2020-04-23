/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of hdmitx dphy hal layer
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_PHY_H__
#define __HAL_HDMITX_PHY_H__

#include "hi_type.h"

/* frl rate define */
#define FRL_RATE_3L_3G      1
#define FRL_RATE_3L_6G      2
#define FRL_RATE_4L_6G      3
#define FRL_RATE_4L_8G      4
#define FRL_RATE_4L_10G     5
#define FRL_RATE_4L_12G     6

/* color depth define */
#define DEEP_COLOR_24BIT   0
#define DEEP_COLOR_30BIT   1
#define DEEP_COLOR_36BIT   2

/* phy port id define */
#define PHY_ID_0     0
#define PHY_ID_1     1

/* pcb_len define */
#define PCB_LEN_1               1
#define PCB_LEN_2               2
#define PCB_LEN_3               3
#define PCB_LEN_4               4
#define PCB_LEN_5               5

/* mode define */
#define PHY_SET_MODE_TMDS   0
#define PHY_SET_MODE_FRL    1

struct phy_tmds {
    hi_u32 pixel_clk;
    hi_u32 tmds_clk; /* TMDS colck,in kHz */
    hi_u32 color_depth; /* Deep color(color depth) */
    hi_u8  pcb_len;
    hi_bool yuv_420;
    hi_bool tpll_enable;
};

struct phy_frl {
    hi_u32 frl_rate; /* Lane and Rate */
    hi_u8 pcb_len;
    hi_bool tpll_enable;
};

struct phy_frl_tmds_clk {
    hi_u32 pixel_clk;
    hi_bool dsc_enable;
    hi_u32 color_depth;
    hi_bool yuv_420;
    hi_u32 hctotal;
    hi_u32 htotal;
    hi_u32 rate;
};

struct phy_ssc {
    hi_bool enable;
    hi_u32 pixel_clk;
    hi_u32 color_depth;
    hi_u32 tmds_clk;
    hi_u32 ssc_freq; /* kHz */
    hi_u32 ssc_amp;
};

struct phy_spec {
};

struct hdmitx_phy_resuorce {
    hi_void *regs;
    hi_void *tpll_regs;
    hi_u32  phy_id;
    hi_u32  ctrl_id;
};

struct hdmitx_phy {
    hi_void *regs;
    hi_void *tpll_regs;
    hi_u32  phy_id;
    hi_u32  ctrl_id;
    hi_bool on;
    hi_u32  mode;
};

struct hdmitx_phy *hal_phy_init(const struct hdmitx_phy_resuorce *res);
hi_void hal_phy_deinit(struct hdmitx_phy *phy);
hi_void hal_phy_on(struct hdmitx_phy *phy);
hi_void hal_phy_off(struct hdmitx_phy *phy);
hi_bool hal_phy_is_on(struct hdmitx_phy *phy);
hi_void hal_phy_configure_tmds(struct hdmitx_phy *phy, const struct phy_tmds *tmds);
hi_void hal_phy_configure_frl(struct hdmitx_phy *phy, const struct phy_frl *frl);
hi_void hal_phy_configure_frl_tmds_clk(struct hdmitx_phy *phy,
    const struct phy_frl_tmds_clk *frl_tmds_clk);
hi_void hal_phy_configure_ssc(struct hdmitx_phy *phy, const struct phy_ssc *ssc);
hi_void hal_phy_configure_spec(struct hdmitx_phy *phy, const struct phy_spec *spec);

#endif /* __HAL_HDMITX_PHY_H__ */
