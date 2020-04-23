/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver dsc controller header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_DSC_H__
#define __HAL_HDMITX_DSC_H__

#include "drv_hdmitx_dsc.h"

/* DSC capability */
struct dsc_caps {
    hi_bool dsc_1p2;                /* DSC support native 420 */
    hi_bool dsc_native420;          /* DSC support native 420 */
    hi_bool dsc_10bpc;              /* DSC support 10bpc */
    hi_bool dsc_12bpc;              /* DSC support 12bpc */
    hi_bool dsc_16bpc;              /* DSC support 16bpc */
    hi_bool dsc_all_bpp;            /* DSC support all bpp */
    hi_u32 dsc_max_frl_rate;        /* DSC support up to max frl rate */
    hi_u8 max_silce_cout;           /* DSC support up to max slices */
    hi_u32 max_pixel_clk_per_slice; /* DSC support up to max pixel clock per slice */
    hi_u8 max_chunk_byte;
};

/* DSC current status */
struct dsc_status {
    hi_bool enable;
    hi_bool o_intf_low_bw;
    hi_bool isync_fifo_full; /* DSC encoder's input fifo is full */
    hi_u8 rc_error_vld;      /* DSC encoder's code status */
};

/* timing information after DSC */
struct dsc_timing_info {
    hi_u16 hcactive; /* hcactive */
    hi_u16 hcblank;  /* hcblank */
    hi_u16 hfront;   /* hfront after dsc */
    hi_u16 hback;    /* hback after dsc */
    hi_u16 hblank;

    hi_u16 vfront;  /* vfront after dsc */
    hi_u16 vsync;   /* vsync after dsc */
    hi_u16 vback;   /* vback after dsc */
    hi_u16 vactive; /* vactive after dsc */

    hi_u8 de_polarity; /* De polarity */
    hi_u8 h_polarity;  /* H polarity */
    hi_u8 v_polarity;  /* V polarity */
};

struct dsc_ctrl {
    void __iomem *regs;
    void __iomem *ctrl_crg_regs;
    hi_u8 version;
    struct dsc_caps caps;
    struct hdmi_controller *parent;
    struct dsc_status status;
    struct dsc_cfg_s cfg_s;
    struct para_input_s inpare;
    struct dsc_timing_info timing_info;
};

struct dsc_ctrl *hdmi_dsc_init(struct hdmi_controller *controller);
hi_s32 hi_hdmi_dsc_enable(struct dsc_ctrl *ctrl);
hi_s32 hi_hdmi_dsc_disable(struct dsc_ctrl *ctrl);
hi_s32 hi_hdmi_dsc_config(struct dsc_ctrl *ctrl);
hi_s32 hi_hdmi_dsc_get_status(struct dsc_ctrl *ctrl);
void hi_hdmi_dsc_deinit(struct dsc_ctrl *ctrl);

#endif

