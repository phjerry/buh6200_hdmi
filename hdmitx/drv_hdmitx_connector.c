/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver connector source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */
#include <linux/hisilicon/securec.h>
#include "drv_hdmitx.h"
#include "hal_hdmitx_io.h"
#include "drv_hdmitx_connector.h"
#include "drv_hdmitx_edid.h"
#include "drv_hdmitx_ddc.h"
#include "drv_hdmi_module_ext.h"
#include "drv_hdmitx_cec.h"
#include "drv_hdmitx_hdcp.h"
#include "hal_hdmitx_aon_reg.h"
#include "hal_hdmitx_ctrl.h"

#define HDMI_MAX_READ_SINK_CNT   2
#define HDMI_HPD_HIGH_RESHOLD    100
#define HDMI_HPD_LOW_RESHOLD     50
#define HDMI_HPD_MAX_LOW_RESHOLD 100
#define HDMI_EVENT_LEN           20
#define PIXELCLK_TOLERANCE       5 /* 0.5% * 1000 */
#define AUDIOCLK_TOLERANCE       1000
#define INCREASE_VALUE           1000000000000
#define DSC_MAX_PIXEL_CLOCK      2720000
#define DSC_MAX_PIXEL_340KHZ     340000
#define DSC_MAX_PIXEL_400KHZ     400000
#define DSC_MAX_PIXEL_340MHZ     340
#define DSC_YUV444_MIN_BPP       8
#define DSC_YUV422_MIN_BPP       7
#define DSC_YUV420_MIN_BPP       6
#define DSC_BPP_FACTOR_16        16
#define DSC_BPP_FACTOR_32        32
#define DSC_CHUNK_SIZE_FACTOR    1024
#define DSC_MAX_SLICE_WIDTH      8
#define HDMITX_CEC_DBG_PHY_ADDR  0x2000

/* test 8kp120_4l10g_dsc edid */
static hi_u8 g_edid_debug_8k120_dsc[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x52, 0x74, 0xd7, 0x0f, 0x00, 0x0e, 0x00, 0x01,
    0x01, 0x1d, 0x01, 0x03, 0x80, 0xa5, 0x5d, 0x78, 0x0a, 0xa8, 0x33, 0xab, 0x50, 0x45, 0xa5, 0x27,
    0x0d, 0x48, 0x48, 0xbd, 0xef, 0x80, 0x71, 0x4f, 0x81, 0xc0, 0x81, 0x00, 0x81, 0x80, 0x95, 0x00,
    0xa9, 0xc0, 0xb3, 0x00, 0xd1, 0xc0, 0x08, 0xe8, 0x00, 0x30, 0xf2, 0x70, 0x5a, 0x80, 0xb0, 0x58,
    0x8a, 0x00, 0x50, 0x1d, 0x74, 0x00, 0x00, 0x1e, 0x56, 0x5e, 0x00, 0xa0, 0xa0, 0xa0, 0x29, 0x50,
    0x30, 0x20, 0x35, 0x00, 0x50, 0x1d, 0x74, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x18,
    0x78, 0x0f, 0xff, 0x77, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfc,
    0x00, 0x53, 0x41, 0x4d, 0x53, 0x55, 0x4e, 0x47, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x95,
    0x02, 0x03, 0x68, 0xf0, 0x5f, 0x61, 0x10, 0x25, 0x04, 0x13, 0x05, 0x15, 0x20, 0x22, 0x5d, 0x5e,
    0x5f, 0x60, 0x65, 0x66, 0x62, 0x64, 0x07, 0x16, 0x03, 0x12, 0x3f, 0x40, 0x75, 0x76, 0xda, 0xdb,
    0xc4, 0xc7, 0xc8, 0xc9, 0x29, 0x09, 0x7f, 0x07, 0x15, 0x17, 0x50, 0x57, 0x17, 0x00, 0x83, 0x01,
    0x00, 0x00, 0xe2, 0x00, 0x4f, 0xe3, 0x05, 0xc3, 0x01, 0x6e, 0x03, 0x0c, 0x00, 0x40, 0x00, 0xb8,
    0x3c, 0x28, 0x00, 0x80, 0x01, 0x02, 0x03, 0x04, 0x6d, 0xd8, 0x5d, 0xc4, 0x01, 0x78, 0x80, 0x53,
    0x02, 0x00, 0x00, 0xc3, 0x55, 0x0b, 0xe3, 0x06, 0x0d, 0x01, 0xe5, 0x0f, 0x01, 0xe0, 0x00, 0x7f,
    0xe1, 0x0e, 0xe5, 0x01, 0x8b, 0x84, 0x90, 0x01, 0x6f, 0xc2, 0x00, 0xa0, 0xa0, 0xa0, 0x55, 0x50,
    0x30, 0x20, 0x35, 0x00, 0x50, 0x1d, 0x74, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90
};

/* SAMSUNG Q900 */
static hi_u8 g_edid_default_hdmi[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x4c, 0x2d, 0xd7, 0x0f, 0x00, 0x0e, 0x00, 0x01,
    0x01, 0x1d, 0x01, 0x03, 0x80, 0x66, 0x39, 0x78, 0x0a, 0xa8, 0x33, 0xab, 0x50, 0x45, 0xa5, 0x27,
    0x0d, 0x48, 0x48, 0xbd, 0xef, 0x80, 0x71, 0x4f, 0x81, 0xc0, 0x81, 0x00, 0x81, 0x80, 0x95, 0x00,
    0xa9, 0xc0, 0xb3, 0x00, 0xd1, 0xc0, 0x08, 0xe8, 0x00, 0x30, 0xf2, 0x70, 0x5a, 0x80, 0xb0, 0x58,
    0x8a, 0x00, 0x50, 0x1d, 0x74, 0x00, 0x00, 0x1e, 0x56, 0x5e, 0x00, 0xa0, 0xa0, 0xa0, 0x29, 0x50,
    0x30, 0x20, 0x35, 0x00, 0x50, 0x1d, 0x74, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x18,
    0x78, 0x0f, 0xff, 0x77, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfc,
    0x00, 0x53, 0x41, 0x4d, 0x53, 0x55, 0x4e, 0x47, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x45,
    0x02, 0x03, 0x66, 0xf0, 0x5d, 0x61, 0x10, 0x1f, 0x04, 0x13, 0x05, 0x14, 0x20, 0x21, 0x22, 0x5d,
    0x5e, 0x5f, 0x60, 0x65, 0x66, 0x62, 0x64, 0x07, 0x16, 0x03, 0x12, 0x3f, 0x40, 0x75, 0x76, 0xda,
    0xdb, 0xc4, 0x2c, 0x09, 0x07, 0x07, 0x15, 0x07, 0x50, 0x57, 0x07, 0x00, 0x00, 0x00, 0x00, 0x83,
    0x01, 0x00, 0x00, 0xe2, 0x00, 0x4f, 0xe3, 0x05, 0xc3, 0x01, 0x6e, 0x03, 0x0c, 0x00, 0x40, 0x00,
    0xb8, 0x3c, 0x28, 0x00, 0x80, 0x01, 0x02, 0x03, 0x04, 0x68, 0xd8, 0x5d, 0xc4, 0x01, 0x78, 0x80,
    0x53, 0x02, 0xe3, 0x06, 0x0d, 0x01, 0xe5, 0x0f, 0x01, 0xe0, 0x00, 0x1f, 0xe3, 0x0e, 0xc6, 0xc7,
    0xe5, 0x01, 0x8b, 0x84, 0x90, 0x01, 0x6f, 0xc2, 0x00, 0xa0, 0xa0, 0xa0, 0x55, 0x50, 0x30, 0x20,
    0x35, 0x00, 0x50, 0x1d, 0x74, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60
};

/* default edid 720P60 & 1024x768P60 */
static hi_u8 g_edid_default_dvi[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x10, 0xa6, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x30, 0x16, 0x01, 0x03, 0x81, 0x00, 0x00, 0x78, 0x0a, 0xee, 0x9d, 0xa3, 0x54, 0x4c, 0x99, 0x26,
    0x0f, 0x47, 0x4a, 0x21, 0x08, 0x00, 0x41, 0x40, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28,
    0x55, 0x00, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e, 0x01, 0x1d, 0x00, 0xbc, 0x52, 0xd0, 0x1e, 0x20,
    0xb8, 0x28, 0x55, 0x40, 0x20, 0xc2, 0x31, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e
};

static hi_s32 hdmi_connector_add_validmodes(struct hdmi_connector *connector);
static hi_s32 hdmi_connector_destroy_modes(struct hdmi_connector *connector);

static hi_u32 hdmitx_get_tmds_clock(hi_u32 pixel_clk, hi_u32 color_depth, hi_u32 color_format)
{
    hi_u32 tmds_clock;

    tmds_clock = (color_format == HDMI_COLOR_FORMAT_YCBCR420) ? (pixel_clk >> 1) : pixel_clk;

    if (color_format != HDMI_COLOR_FORMAT_YCBCR422) {
        switch (color_depth) {
            case HDMI_BPC_30:
                /* tmds_clock = (pixel_clk * 5) >> 2, when deep color is 10bit. */
                tmds_clock = (tmds_clock * 5) >> 2;
                break;
            case HDMI_BPC_36:
                /* tmds_clock = (pixel_clk * 3) >> 1, when deep color is 12bit. */
                tmds_clock = (tmds_clock * 3) >> 1;
                break;
            default:
                break;
        }
    }

    return tmds_clock;
}

static void hdmi_source_tmds_capability_init(struct src_hdmi *src, hi_u32 caps)
{
    hi_bool tmds_clock_600m;

    /* tmds */
    src->dvi_support = !!(caps & DVI_SUPPORT_MASK);
    src->hdmi_support = !!(caps & HDMI_SUPPORT_MASK);
    tmds_clock_600m = !!(caps & MAX_TMDS_CLOCK_MASK);
    if (tmds_clock_600m) {
        src->max_tmds_clock = HDMI20_MAX_TMDS_CLK;
    } else {
        src->max_tmds_clock = HDMI14_MAX_TMDS_CLK;
    }

    /* scdc */
    src->scdc_present = !!(caps & SCDC_PRESENT_MASK);
    src->scdc_lte_340mcsc = !!(caps & SCDC_LTE_340MCSC_MASK);
}

static void hdmi_source_frl_dsc_capability_init(struct src_hdmi *src,
                                                hi_u32 caps)
{
    hi_u32 max_slice;
    hi_u32 max_pixel_clk_per_line;

    /* frl */
    src->max_frl_rate = (caps & MAX_FRL_RATE_MASK) >> MAX_FRL_RATE_SHIFT;
    src->ffe_levels = (caps & FFE_LEVELS_MASK) >> FFE_LEVELS_SHIFT;

    /* dsc */
    src->dsc_support = !!(caps & DSC_SUPPORT_MASK);
    src->native_y420 = !!(caps & NATIVE_Y420_MASK);
    src->dsc_10bpc = !!(caps & DSC_10BPC_MASK);
    src->dsc_12bpc = !!(caps & DSC_12BPC_MASK);
    max_slice = (caps & MAX_SLICE_COUNT_MASK) >> MAX_SLICE_COUNT_SHIFT;
    switch (max_slice) {
        case DSC_CAP_MAX_SLICE_CNT_0:
            break;
        case DSC_CAP_MAX_SLICE_CNT_1:
            src->max_silce_count = 1; /* 1 slices */
            break;
        case DSC_CAP_MAX_SLICE_CNT_2:
            src->max_silce_count = 2; /* 2 slices */
            break;
        case DSC_CAP_MAX_SLICE_CNT_3:
            src->max_silce_count = 4; /* 4 slices */
            break;
        case DSC_CAP_MAX_SLICE_CNT_4:
            src->max_silce_count = 8; /* 8 slices */
            break;
        case DSC_CAP_MAX_SLICE_CNT_5:
            src->max_silce_count = 12; /* 12 slices */
            break;
        case DSC_CAP_MAX_SLICE_CNT_6:
            src->max_silce_count = 16; /* 16 slices */
            break;
        default:
            break;
    }

    src->max_dsc_frl_rate = src->max_frl_rate;

    max_pixel_clk_per_line = (caps & MAX_PIXEL_CLK_PER_SLICE_MASK) >>
        MAX_PIXEL_CLK_PER_SLICE_SHIFT;

    if (max_pixel_clk_per_line == 0) {
        src->max_pixel_clk_per_slice = 340; /* 340Mhz */
    } else if (max_pixel_clk_per_line == 1) {
        src->max_pixel_clk_per_slice = 400; /* 400Mhz */
    }
}

static void hdmi_source_csc_capability_init(struct src_hdmi *src,
                                            hi_u32 caps, hi_u32 caps2)
{
    /* color depth */
    src->bpc_30 = !!(caps & BPC_30_MASK);
    src->bpc_36 = !!(caps & BPC_36_MASK);
    src->bpc_48 = !!(caps & BPC_48_MASK);

    /* csc */
    src->rgb2yuv = !!(caps & RGB2YUV_MASK);
    src->ycbcr444 = !!(caps & YCBCR444_MASK);
    src->ycbcr422 = !!(caps & YCBCR422_MASK);
    src->ycbcr420 = !!(caps & YCBCR420_MASK);
    src->yuv2rgb = !!(caps2 & YUV2RGB_MASK);
    src->dither_support = !!(caps2 & DITHER_SUPPORT_MASK);
}

static void hdmi_source_capability_init(struct hdmi_connector *connector)
{
    struct source_capability *src = NULL;
    struct hisilicon_hdmi *hdmi = connector->hdmi_dev;
    hi_u32 caps;
    hi_u32 caps2;

    caps = hdmi->caps;
    caps2 = hdmi->caps2;

    src = &connector->src_cap;

    src->cec_support = !!(caps & CEC_SUPPORT_MASK);
    src->hdcp14_support = !!(caps & HDCP14_SUPPORT_MASK);
    src->hdcp2x_support = !!(caps & HDCP2x_SUPPORT_MASK);

    hdmi_source_tmds_capability_init(&src->hdmi, caps);
    hdmi_source_frl_dsc_capability_init(&src->hdmi, caps);
    hdmi_source_csc_capability_init(&src->hdmi, caps, caps2);

    src->hpd.fillter_en = true;
    src->hpd.high_reshold = HDMI_HPD_HIGH_RESHOLD;
    src->hpd.low_reshold = HDMI_HPD_LOW_RESHOLD;
}

static void hdmi_hpd_config(struct hisilicon_hdmi *hdmi,
                            struct hdmi_hpd_cfg *cfg)
{
    if (hdmi == NULL || cfg == NULL) {
        HDMI_INFO("null pointer!\n");
        return;
    }

    if (cfg->low_reshold == 0 || cfg->high_reshold == 0) {
        return;
    }

    hdmi_clrset(hdmi->hdmi_aon_regs, REG_HOTPLUG_FILLTER_CFG,
                REG_HPD_LOW_RESHOLD_M | REG_HPD_HIGH_RESHOLD_M,
                reg_hpd_low_reshold(cfg->low_reshold - 1) |
                reg_hpd_high_reshold(cfg->high_reshold - 1));

    hdmi_clrset(hdmi->hdmi_aon_regs, REG_HOTPLUG_ST_CFG,
                REG_HPD_FILLTER_EN_M, reg_hpd_fillter_en(1));
}

void hdmi_set_hpd_irq_mask(struct hisilicon_hdmi *hdmi, hi_bool mask)
{
    if (!hdmi) {
        return;
    }
    hdmi_clrset(hdmi->hdmi_aon_regs, REG_TX_AON_INTR_MASK,
                REG_AON_INTR_MASK0_M, reg_aon_intr_mask0(!mask));
}

static void hdmi_clear_hpd_irq(struct hisilicon_hdmi *hdmi)
{
    if (!hdmi) {
        return;
    }
    hdmi_clrset(hdmi->hdmi_aon_regs, REG_TX_AON_INTR_STATE,
                REG_AON_INTR_STAT0_M, reg_aon_intr_stat0(1));
}

enum hdmi_rsen_status hdmi_connector_get_rsen(struct hisilicon_hdmi *hdmi)
{
    hi_u32 rsen;

    if (hdmi == NULL) {
        HDMI_INFO("null pointer!\n");
        return RSEN_DET_FAIL;
    }

    rsen = hdmi_readl(hdmi->hdmi_aon_regs, REG_TX_AON_STATE) & REG_PHY_RX_SENSE_M;

    return (enum hdmi_rsen_status)(!!rsen);
}

static enum hdmi_connector_status hdmi_get_hpd_status(struct hisilicon_hdmi *hdmi)
{
    hi_bool hpd_pol = HI_FALSE;
    hi_bool hpd_state = HI_FALSE;

    if (hdmi == NULL) {
        HDMI_INFO("null pointer!\n");
        return HPD_DET_FAIL;
    }

    hpd_pol = hdmi_readl(hdmi->hdmi_aon_regs, REG_HOTPLUG_ST_CFG) &
              REG_HPD_POLARITY_CTL_M;
    hpd_state = hdmi_readl(hdmi->hdmi_aon_regs, REG_TX_AON_STATE) &
                REG_HOTPLUG_STATE_M;
    if (hpd_pol) {
        return hpd_state ? HPD_PLUGOUT : HPD_PLUGIN;
    }

    return hpd_state ? HPD_PLUGIN : HPD_PLUGOUT;
}

static hi_s32 hdmi_hpd_irq_server(hi_s32 irq, void *data)
{
    struct hdmi_connector *connector = data;
    struct hisilicon_hdmi *hdmi = HI_NULL;

    if (!connector || !connector->hdmi_dev) {
        HDMI_ERR("null!\n");
        return OSAL_IRQ_HANDLED;
    }

    hdmi = connector->hdmi_dev;

    hdmi_set_hpd_irq_mask(hdmi, true);

    connector->status = hdmi_get_hpd_status(hdmi);

    if (connector->status == HPD_PLUGOUT) {
        drv_hdmitx_hdcp_plugout_handle(hdmi->hdcp);
    }

    hdmi_clear_hpd_irq(hdmi);

    if (connector->poll_hpd) {
        schedule_delayed_work(&hdmi->hpd_poll_work, 0);
    } else {
        hdmi_set_hpd_irq_mask(hdmi, false);
    }

    return OSAL_IRQ_HANDLED;
}

void hdmi_connector_set_debug_edid(struct hdmi_connector *connector, hi_u8 *edid, hi_s32 size)
{
    hi_s32 ret;

    if (connector == HI_NULL) {
        HDMI_INFO("null!\n");
        return;
    }

    ret = hdmi_connector_edid_reset(connector);
    if (ret != 0) {
        HDMI_ERR("edid reset fail.\n");
    }

    osal_mutex_lock(&connector->mutex);

    if (edid == HI_NULL || size == HI_NULL) {
        connector->edid_raw = (struct edid *)g_edid_debug_8k120_dsc;
        connector->edid_size = sizeof(g_edid_debug_8k120_dsc);
        HDMI_INFO("set g_edid_debug_8k120_dsc succes\n");
    } else {
        connector->edid_raw = (struct edid *)edid;
        connector->edid_size = size;
        HDMI_INFO("set success,size=%d\n", size);
    }
    connector->edid_src_type = EDID_DEBUG;

    /* add modes */
    ret = hdmi_add_edid_modes(connector, connector->edid_raw);
    if (!ret) {
        HDMI_ERR("no mode!\n");
    }
    ret = hdmi_connector_add_validmodes(connector);
    if (ret != 0) {
        HDMI_ERR("add validmode fail!\n");
    }

    osal_mutex_unlock(&connector->mutex);
}

static hi_void hdmi_set_default_edid(struct hdmi_connector *connector)
{
    hi_s32 ret;

    if (connector == NULL) {
        HDMI_ERR("null pointer.\n");
        return;
    }

    osal_mutex_lock(&connector->mutex);
    if ((connector->src_cap.hdmi.hdmi_support) == HI_NULL || (connector->user_dvi_mode) != HI_NULL) {
        HDMI_INFO("force dvi mode edid.user_dvi_mode=%d.\n", connector->user_dvi_mode);
        connector->edid_raw = (struct edid *)g_edid_default_dvi;
        connector->edid_size = sizeof(g_edid_default_dvi);
        connector->edid_src_type = EDID_DEFAULT_DVI;
    } else {
        HDMI_INFO("force hdmi mode edid.user_dvi_mode=%d.\n", connector->user_dvi_mode);
        connector->edid_raw = (struct edid *)g_edid_default_hdmi;
        connector->edid_size = sizeof(g_edid_default_hdmi);
        connector->edid_src_type = EDID_DEFAULT_HDMI;
    }

    /* add modes */
    ret = hdmi_add_edid_modes(connector, connector->edid_raw);
    HDMI_INFO("add modes %s!\n", ret ? "fail" : "OK");

    ret = hdmi_connector_add_validmodes(connector);
    if (ret) {
        HDMI_ERR("add valid modes fail.\n");
    }
    osal_mutex_unlock(&connector->mutex);

    return;
}

static hi_void hdmi_hpd_process(struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_s32 rd_sink_cnt = 0;
    char event[HDMI_EVENT_LEN] = {0};
    struct hdmi_connector *connector = hdmi->connector;

    if (connector == NULL) {
        HDMI_ERR("null pointer.\n");
        return;
    }

    /* Reset edid */
    if (connector->edid_src_type != EDID_DEBUG) {
        ret = hdmi_connector_edid_reset(hdmi->connector);
        HDMI_INFO("reset edid %s!\n", ret ? "fail" : "OK");
        drv_hdmitx_hdcp_clear_cap(hdmi->hdcp);
        drv_hdmitx_hdcp_off(hdmi->hdcp);
    }

    /* Read sink edid */
    if (connector->edid_src_type != EDID_DEBUG &&
        connector->status != HPD_PLUGOUT) {
        do {
            connector->edid_raw = hdmi_connector_get_edid(connector, hdmi->ddc);
            rd_sink_cnt++;
            HDMI_INFO("read sink edid %d time %s!\n", rd_sink_cnt,
                connector->edid_raw ? "OK" : "fail");
        }while (!connector->edid_raw && rd_sink_cnt < HDMI_MAX_READ_SINK_CNT);

        drv_hdmitx_hdcp_get_hw_cap(hdmi->hdcp);
    }

    /* Default edid */
    if (connector->edid_src_type != EDID_DEBUG &&
        connector->status != HPD_PLUGOUT &&
        connector->edid_raw == NULL) {
        hdmi_set_default_edid(connector);
    }

    drv_hdmitx_cec_s_phys_addr_from_edid(hdmi->cec, connector->edid_raw);

    /* Update hotplug status, it must be set after reading edid. */
    connector->hotplug = connector->status;

    /* Set event */
    if (connector->status == HPD_PLUGIN ||
        connector->status == HPD_PLUGOUT) {
        ret = snprintf_s(event, sizeof(event), HDMI_EVENT_LEN, "HOTPLUG=%d HDMI=%d",
            connector->status % HPD_DET_FAIL, hdmi->id % (DISP_INFT_HDMI1 + 1));
        if (ret == HI_FAILURE) {
            HDMI_ERR("snprintf_s failed!\n");
            return;
        }
    }
    hdmi_sysfs_event(hdmi, event, HDMI_EVENT_LEN);
    /* Hot plug event notify for ao. */
    if (connector->status == HPD_PLUGIN ||
        connector->status == HPD_DET_FAIL) {
        hdmi_ao_notifiers(hdmi, HDMI_HOT_PLUG_IN);
    } else if (connector->status == HPD_PLUGOUT) {
        hdmi_ao_notifiers(hdmi, HDMI_HOT_PLUG_OUT);
    }

    hdmi_set_hpd_irq_mask(hdmi, false);
}

void hdmi_connector_enable_hpd(struct hisilicon_hdmi *hdmi, struct hdmi_connector *connector)
{
    hi_u32 hpd_detect_time;
    struct hdmi_hpd_cfg *hpd_cfg = NULL;
    hi_s32 ret;
    hi_u8 edid_byte;

    if (hdmi == NULL || connector == NULL) {
        HDMI_ERR("null pointer.\n");
        return;
    }

    connector->status = HPD_DETECTING;
    connector->hotplug = connector->status;

    hpd_cfg = &connector->src_cap.hpd;
    if (hpd_cfg->low_reshold > HDMI_HPD_MAX_LOW_RESHOLD) {
        HDMI_WARN("low_reshold force %d to %d\n", hpd_cfg->low_reshold,
                  HDMI_HPD_MAX_LOW_RESHOLD);
        hpd_cfg->low_reshold = HDMI_HPD_MAX_LOW_RESHOLD;
    }

    hdmi_hpd_config(hdmi, hpd_cfg);

    /* hardware hpd status */
    hpd_detect_time = hpd_cfg->high_reshold + 1; /* Need add 1ms margin. */
    do {
        connector->status = hdmi_get_hpd_status(hdmi);
        if (connector->status == HPD_PLUGIN) {
            break;
        }
        osal_msleep(1); /* 1ms each time */
    } while (hpd_detect_time--);

    HDMI_INFO("status=%d,hpd reshold H=%d,L=%d,wait %d ms\n",
              connector->status,
              hpd_cfg->high_reshold, hpd_cfg->low_reshold,
              hpd_cfg->high_reshold - hpd_detect_time);

    if (connector->status == HPD_PLUGOUT) {
        /* read 1 byte to detect connect */
        ret = hi_hdmi_edid_read(hdmi->ddc, &edid_byte, 0, 1);
        if (ret == HI_SUCCESS) {
            HDMI_WARN("hotplug low,try read 1 bytes success!\n");
            connector->status = HPD_DET_FAIL;
        }
    }

    if (connector->poll_hpd &&
        (connector->status == HPD_DET_FAIL ||
         connector->status == HPD_PLUGIN)) {
       /*
        * Hotplug irq is disalbe by connector_init,
        * it will be enable in hdmi->hpd_poll_work.
        * We need hdmi_hpd_process to read edid once for module insmod.
        * We should clear hpd_irq once since it will trigger
        * schedule_delayed_work 2 times from power on reset,
        * irq occur in boot but not clear.
        */
        hdmi_clear_hpd_irq(hdmi);
        hdmi_hpd_process(hdmi);
    } else {
        connector->hotplug = connector->status;
        /* Need reset edid info when hdmitx resume. */
        hdmi_connector_edid_reset(connector);
        /* Enable hotplug irq */
        hdmi_set_hpd_irq_mask(hdmi, false);
    }
}

static void hdmi_hpd_poll_func(struct work_struct *work)
{
    struct hisilicon_hdmi *hdmi = NULL;

    if (work == NULL) {
        HDMI_ERR("null pointer.\n");
        return;
    }

    hdmi = osal_container_of(work, struct hisilicon_hdmi, hpd_poll_work.work);
    if (hdmi == NULL) {
        HDMI_ERR("null pointer.\n");
        return;
    }

    hdmi_hpd_process(hdmi);
}

/*
 * hdmi_connector_init - connector init
 * @hdmi: hdmi we're probing,which is the parent of the new hdmi connector instance.
 * Return: Pointer to valid hdmi_connector or NULL if we couldn't kzmalloc any.
 */
struct hdmi_connector *hdmi_connector_init(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = HI_NULL;

    if (hdmi == HI_NULL) {
        HDMI_ERR("ptr is null.\n");
        return HI_NULL;
    }

    connector = osal_kmalloc(HI_ID_HDMITX, sizeof(struct hdmi_connector), OSAL_GFP_KERNEL);
    if (connector == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_NULL;
    }

    if (memset_s(connector, sizeof(struct hdmi_connector), 0, sizeof(struct hdmi_connector))) {
        HDMI_ERR("memset_s fail.\n");
        goto err0;
    }

    connector->hdmi_dev = hdmi;
    connector->priv = (void *)hdmi;
    connector->detail_vic_base = VIC_DETAIL_TIMING_BASE;
    connector->irq_name = osal_kmalloc(HI_ID_HDMITX, INT_AON_NAME_LEN, OSAL_GFP_KERNEL);
    if (connector->irq_name == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        goto err0;
    }

    if (memset_s(connector->irq_name, INT_AON_NAME_LEN, 0, INT_AON_NAME_LEN)) {
        HDMI_ERR("memset_s fail.\n");
        goto err1;
    }

    if (snprintf_s(connector->irq_name, INT_AON_NAME_LEN, INT_AON_NAME_LEN - 1,
        "int_hdmitx%d_aon", hdmi->is_hdmi21 ? 1 : 0) == HI_FAILURE) {
        HDMI_ERR("snprintf_s failed!\n");
        goto err1;
    }

    osal_mutex_init(&connector->mutex);
    OSAL_INIT_LIST_HEAD(&connector->valid_modes);
    OSAL_INIT_LIST_HEAD(&connector->probed_modes);
    hdmi_source_capability_init(connector);
    connector->poll_hpd = true;

    INIT_DELAYED_WORK(&hdmi->hpd_poll_work, hdmi_hpd_poll_func);
    /* static inline hi_s32 __must_check */
    hdmi_set_hpd_irq_mask(hdmi, true);
    if (osal_irq_request(hdmi->irq, hdmi_hpd_irq_server, HI_NULL, connector->irq_name, connector)) {
        HDMI_ERR("hdmi%d request_irq fail!\n", hdmi->id);
    }

    return connector;

err1:
    osal_kfree(HI_ID_HDMITX, connector->irq_name);
err0:
    osal_kfree(HI_ID_HDMITX, connector);
    return HI_NULL;
}

void hdmi_connector_deinit(struct hisilicon_hdmi *hdmi)
{
    if ((hdmi != HI_NULL) && (hdmi->connector != HI_NULL)) {
        osal_irq_free(hdmi->irq, hdmi->connector);
        cancel_delayed_work_sync(&hdmi->hpd_poll_work);
        hdmi_connector_edid_reset(hdmi->connector);
        osal_kfree(HI_ID_HDMITX, hdmi->connector->irq_name);
        osal_kfree(HI_ID_HDMITX, hdmi->connector);
    }
}

/*
 * hdmi_connector_get_edid - get E DID data, if available
 * @connector: hdmi connector we're probing
 * @ddc: ddc operation struct
 *
 * Poke the given I2C channel to grab EDID data if possible.  If found,
 * attach it to the connector.
 *
 * Return: Pointer to valid EDID or NULL if we couldn't find any.
 */
struct edid *hdmi_connector_get_edid(struct hdmi_connector *connector, struct hdmi_ddc *ddc)
{
    hi_s32 i = 0;
    hi_s32 j = 0;
    hi_s32 size = 0;
    hi_s32 valid_extensions;
    hi_u8 *edid = HI_NULL;
    hi_u8 *new = HI_NULL;

    if (connector == HI_NULL || ddc == HI_NULL) {
        HDMI_ERR("ptr is null!\n");
        return NULL;
    }

    edid = osal_kmalloc(HI_ID_HDMITX, EDID_LENGTH, OSAL_GFP_KERNEL);
    if (edid == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_NULL;
    }

    if (memset_s(edid, EDID_LENGTH, 0, EDID_LENGTH)) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, edid);
        return HI_NULL;
    }

    /* base block fetch */
    for (i = 0; i < 4; i++) {
        if (hi_hdmi_edid_read(ddc, edid, 0, EDID_LENGTH) != HI_SUCCESS) {
            goto out;
        }
        /* &connector->edid_corrupt */
        if (hdmi_edid_block_valid(edid, 0, HI_FALSE, HI_NULL)) {
            break;
        }
        /* connector->null_edid_counter++; */
        if (i == 0 && hdmi_edid_is_zero(edid, EDID_LENGTH)) {
            goto carp;
        }
    }

    if (i == 4) {
        goto carp;
    }

    /* if there's no extensions, we're done */
    valid_extensions = edid[0x7e];
    size = EDID_LENGTH;
    if (valid_extensions == 0) {
        return (struct edid *)edid;
    }

    new = osal_kmalloc(HI_ID_HDMITX, (valid_extensions + 1) * EDID_LENGTH, OSAL_GFP_KERNEL);
    if (new == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        goto out;
    }

    if (memset_s(new, (valid_extensions + 1) * EDID_LENGTH, 0, (valid_extensions + 1) * EDID_LENGTH)) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, new);
        goto out;
    }

    if (memcpy_s(new, (valid_extensions + 1) * EDID_LENGTH, edid, EDID_LENGTH)) {
        osal_kfree(HI_ID_HDMITX, new);
        HDMI_ERR("memcpy_s failed!\n");
        goto out;
    }
    osal_kfree(HI_ID_HDMITX, edid);
    edid = new;

    size = (valid_extensions + 1) * EDID_LENGTH;
    for (j = 1; j <= edid[0x7e]; j++) {
        hi_u8 *block = edid + j * EDID_LENGTH;

        for (i = 0; i < 4; i++) {
            if (hi_hdmi_edid_read(ddc, block, j, EDID_LENGTH) != HI_SUCCESS) {
                goto out;
            }

            if (hdmi_edid_block_valid(block, j, false, NULL)) {
                break;
            }
        }

        if (i == 4) {
            valid_extensions--;
        }
    }

    if (valid_extensions != edid[0x7e]) {
        hi_u8 *base = HI_NULL;

        hdmi_bad_edid(edid, edid[0x7e] + 1);

        edid[EDID_LENGTH - 1] += edid[0x7e] - valid_extensions;
        edid[0x7e] = valid_extensions;

        new = osal_kmalloc(HI_ID_HDMITX, (valid_extensions + 1) * EDID_LENGTH, OSAL_GFP_KERNEL);
        if (new == HI_NULL) {
            HDMI_ERR("osal_kmalloc fail.\n");
            goto out;
        }

        if (memset_s(new, (valid_extensions + 1) * EDID_LENGTH, 0, (valid_extensions + 1) * EDID_LENGTH)) {
            HDMI_ERR("memset_s fail.\n");
            osal_kfree(HI_ID_HDMITX, new);
            goto out;
        }

        base = new;
        for (i = 0; i <= edid[0x7e]; i++) {
            hi_u8 *block = edid + i * EDID_LENGTH;

            if (!hdmi_edid_block_valid(block, i, false, NULL)) {
                continue;
            }

            if (memcpy_s(base, EDID_LENGTH, block, EDID_LENGTH)) {
                osal_kfree(HI_ID_HDMITX, new);
                HDMI_ERR("memcpy_s failed!\n");
                goto out;
            }
            base += EDID_LENGTH;
        }

        osal_kfree(HI_ID_HDMITX, edid);
        edid = new;
    }

    osal_mutex_lock(&connector->mutex);
    connector->edid_size = size;
    connector->edid_raw = (struct edid *)edid;
    connector->edid_src_type = EDID_FROM_SINK;
    /* add modes */
    if (!hdmi_add_edid_modes(connector, connector->edid_raw)) {
        HDMI_ERR("no mode!\n");
    }

    hdmi_connector_add_validmodes(connector);
    osal_mutex_unlock(&connector->mutex);

    return (struct edid *)edid;

carp:
    hdmi_bad_edid(edid, 1);
out:
    osal_kfree(HI_ID_HDMITX, edid);
    osal_mutex_lock(&connector->mutex);
    connector->edid_size = 0;
    osal_mutex_unlock(&connector->mutex);
    return NULL;
}

/*
 * hdmi_connector_edid_reset - reset edid ,clear all valid data
 * @connector: hdmi connector we're probing
 *
 *
 * Return: 0 for success,negetive value for fail.
 */
hi_s32 hdmi_connector_edid_reset(struct hdmi_connector *connector)
{
    if (!connector) {
        return -EINVAL;
    }
    osal_mutex_lock(&connector->mutex);
    if (connector->edid_src_type == EDID_FROM_SINK &&
        connector->edid_raw) {
        osal_kfree(HI_ID_HDMITX, connector->edid_raw);
    }

    connector->edid_raw = NULL;
    connector->edid_size = 0;

    if (memset_s(&connector->audio, sizeof(connector->audio), 0, sizeof(connector->audio))) {
        goto err_memset;
    }
    if (memset_s(&connector->base, sizeof(connector->base), 0, sizeof(connector->base))) {
        goto err_memset;
    }
    if (memset_s(&connector->color, sizeof(connector->color), 0, sizeof(connector->color))) {
        goto err_memset;
    }
    if (memset_s(&connector->dolby, sizeof(connector->dolby), 0, sizeof(connector->dolby))) {
        goto err_memset;
    }
    if (memset_s(&connector->dsc, sizeof(connector->dsc), 0, sizeof(connector->dsc))) {
        goto err_memset;
    }
    if (memset_s(&connector->hdr, sizeof(connector->hdr), 0, sizeof(connector->hdr))) {
        goto err_memset;
    }
    if (memset_s(&connector->latency, sizeof(connector->latency), 0, sizeof(connector->latency))) {
        goto err_memset;
    }
    if (memset_s(&connector->quick_cfg, sizeof(connector->quick_cfg), 0, sizeof(connector->quick_cfg))) {
        goto err_memset;
    }
    if (memset_s(&connector->quirk, sizeof(connector->quirk), 0, sizeof(connector->quirk))) {
        goto err_memset;
    }
    if (memset_s(&connector->scdc, sizeof(connector->scdc), 0, sizeof(connector->scdc))) {
        goto err_memset;
    }
    if (memset_s(&connector->timing, sizeof(connector->timing), 0, sizeof(connector->timing))) {
        goto err_memset;
    }
    if (memset_s(&connector->vrr, sizeof(connector->vrr), 0, sizeof(connector->vrr))) {
        goto err_memset;
    }
    if (memset_s(&connector->display_info, sizeof(connector->display_info), 0, sizeof(connector->display_info))) {
        goto err_memset;
    }

    connector->edid_src_type = EDID_EMPTY;

    if (hdmi_connector_destroy_modes(connector)) {
        HDMI_ERR("destroy mode fail!\n");
    }
    osal_mutex_unlock(&connector->mutex);
    return HI_SUCCESS;

err_memset:
    HDMI_ERR("memset failed!\n");
    osal_mutex_unlock(&connector->mutex);
    return HI_FAILURE;
}

/*
 * hdmi_connector_mode_validate - validate the specified display mode
 * @connector: connector we're probing
 * @mode: display mode to be validated
 *
 * Return: true if the specified mode is supported or otherwise false.
 */
hi_bool hdmi_connector_mode_validate(struct hdmi_connector *connector,
                                     struct hdmi_timing_data *timing)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *p = HI_NULL;
    struct hdmi_valid_mode *valid_mode = HI_NULL;
    hi_bool ret = HI_FALSE;
    hi_u32 vic = timing->in.vic;
    struct hdmi_out_data *out = &timing->out;

    if ((connector == HI_NULL) || (timing == HI_NULL)) {
        return false;
    }

    osal_mutex_lock(&connector->mutex);

    if (!osal_list_empty(&connector->valid_modes)) {
        osal_list_for_each_safe(p, n, &connector->valid_modes) {
            valid_mode = osal_list_entry(p, struct hdmi_valid_mode, head);
            if (valid_mode->band.vic == vic &&
                valid_mode->band.color_format == out->color.color_format &&
                valid_mode->band.color_depth == out->color_depth &&
                valid_mode->valid) {
                ret = true;
                break;
            }
        }
    }

    osal_mutex_unlock(&connector->mutex);

    return ret;
}

void hdmi_valid_mode_copy(struct hdmi_valid_mode *dst,
                          struct hdmi_valid_mode *src)
{
    struct osal_list_head head = dst->head;

    *dst = *src;
    dst->head = head;
}

static hi_s8 *hdmi_get_color_format_name(hi_u32 color_format)
{
    switch (color_format) {
        case RGB444:
            return "RGB444";
        case YCBCR422:
            return "YCBCR422";
        case YCBCR444:
            return "YCBCR444";
        case YCBCR420:
            return "YCBCR420";
        default:
            break;
    }

    return "UNKNOWN COLOR FORMAT";
}

static hi_s8 *hdmi_get_color_depth_name(hi_u32 color_depth)
{
    switch (color_depth) {
        case CD_24:
            return "CD_24";
        case CD_30:
            return "CD_30";
        case CD_36:
            return "CD_36";
        case CD_48:
            return "CD_48";
        default:
            break;
    }

    return "UNKNOWN COLOR DEPTH";
}

static void hdmi_dump_valid_mode(struct hdmi_valid_mode *mode)
{
    struct hdmi_timing_mode *timing_mode = NULL;
    hi_s8 *color_format = NULL;
    hi_s8 *color_depth = NULL;

    if (mode == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    timing_mode = vic2timing_mode(mode->band.vic);

    color_format = hdmi_get_color_format_name(mode->band.color_format);
    color_depth = hdmi_get_color_depth_name(mode->band.color_depth);

    HDMI_DBG("[%d] %s %s %s", mode->band.vic,
             (timing_mode != NULL) ? timing_mode->name : "DETAIL TIMING",
             color_format, color_depth);
    return;
}

hi_bool hdmi_create_valid_mode(struct hdmi_connector *connector,
                               struct hdmi_valid_mode *valid_mode)
{
    struct hdmi_valid_mode *nmode = HI_NULL;

    nmode = osal_kmalloc(HI_ID_HDMITX, sizeof(struct hdmi_valid_mode), OSAL_GFP_KERNEL);
    if (nmode == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_NULL;
    }

    if (memset_s(nmode, sizeof(struct hdmi_valid_mode), 0, sizeof(struct hdmi_valid_mode))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, nmode);
        return HI_NULL;
    }

    valid_mode->valid = true;
    hdmi_valid_mode_copy(nmode, valid_mode);
    hdmi_dump_valid_mode(valid_mode);

    osal_list_add_tail(&nmode->head, &connector->valid_modes);

    return true;
}

void hdmi_destroy_valid_mode(struct hdmi_connector *connector,
                             struct hdmi_valid_mode *valid_mode)
{
    if (valid_mode != HI_NULL) {
        osal_list_del(&valid_mode->head);
        osal_kfree(HI_ID_HDMITX, valid_mode);
    }
}

static hi_u32 hdmi_get_audio_ap(struct audio_in *pst_audio_in)
{
    hi_u32 audio_ap = 0;

    if (pst_audio_in == NULL) {
        return 0;
    }

    if (pst_audio_in->packet_type == 0x2 || pst_audio_in->packet_type == 0x7) {
        if (pst_audio_in->layout == 0) {
            audio_ap = 25; /* 25: ap * 100, according to the hdmi agreement, ap = 0.25 */
        } else {
            audio_ap = 100; /* 100: ap * 100, according to the hdmi agreement, ap = 1 */
        }
    } else if (pst_audio_in->packet_type == 0x8) {
        audio_ap = 100; /* 100: ap * 100, according to the hdmi agreement, ap = 1 */
    } else if (pst_audio_in->packet_type == 0x9 ||
               pst_audio_in->packet_type == 0xE ||
               pst_audio_in->packet_type == 0xF) {
        audio_ap = 100; /* 100: ap * 100, according to the hdmi agreement, ap = 1 */
    } else if (pst_audio_in->packet_type == 0xB ||
               pst_audio_in->packet_type == 0xC) {
        if (pst_audio_in->acat == 1) { /* 3D_CA field for 10.2 Channels (ACAT = 1) */
            audio_ap = 200; /* 200: ap * 100, according to the hdmi agreement, ap = 2 */
        } else if (pst_audio_in->acat == 2) { /* 3D_CA field for 22.2 Channels (ACAT = 2) */
            audio_ap = 300; /* 300: ap * 100, according to the hdmi agreement, ap = 3 */
        } else if (pst_audio_in->acat == 3) { /* 3D_CA field for 30.2 Channels (ACAT = 3) */
            audio_ap = 400; /* 400: ap * 100, according to the hdmi agreement, ap = 4 */
        }
    }

    return audio_ap;
}

hi_bool hdmi_audio_caps(struct audio_in *pst_audio_in)
{
    hi_u32 kcd = 0;
    hi_u32 k420 = 0;
    hi_u32 time_line;
    hi_u32 audio_ap;
    hi_u64 audio_rap;
    hi_u64 avg_audio_packets;
    hi_u32 blank_audio_min;
    hi_u32 blank_tb_per_line;

    struct hdmi_timing_mode *mode = &pst_audio_in->mode;

    if (pst_audio_in->pixel_format == YCBCR420) {
        k420 = 2;
    } else {
        k420 = 1;
    }

    if (pst_audio_in->pixel_format == YCBCR422) {
        kcd = 1;
    } else {
        kcd = pst_audio_in->bpc / 8;
    }

    pst_audio_in->mode.pixel_clock = pst_audio_in->mode.pixel_clock *
                                     (1000 + PIXELCLK_TOLERANCE); /* 1000: unit conversion,KHZ */
    /* time_line * 1000000000000 */
    time_line = osal_div_s64((pst_audio_in->mode.h_active + pst_audio_in->mode.h_blank) * INCREASE_VALUE,
                             pst_audio_in->mode.pixel_clock);

    audio_ap = hdmi_get_audio_ap(pst_audio_in); /* ap * 100 */
    audio_rap = osal_div64_u64(pst_audio_in->audio_rate * (1000000 + AUDIOCLK_TOLERANCE) * audio_ap, 1000000 * 100);
    avg_audio_packets = osal_div64_u64(audio_rap * time_line, INCREASE_VALUE);
    avg_audio_packets++;
    blank_audio_min = 64 + 32 * avg_audio_packets;

    blank_tb_per_line = DIV_ROUND_UP(mode->h_blank * kcd, k420);
    if (blank_audio_min > blank_tb_per_line) {
        return false;
    }

    return true;
}

static hi_bool dsc_caps_decision(struct hdmi_connector *connector,
                                 struct band_mode *mode)
{
    struct dsc_property *cap = &connector->dsc;
    struct src_hdmi *src = &connector->src_cap.hdmi;

    if (!cap->dsc_1p2) {
        HDMI_DBG("Enable DSC failure.\n");
        return HI_FALSE;
    }

    if (!(cap->dsc_max_rate > 0 &&
          cap->dsc_max_rate <= 6)) { /* frl rate is not in 0~6 */
        HDMI_DBG("Enable DSC failure.\n");
        return HI_FALSE;
    }

    if (!src->dsc_support) {
        HDMI_DBG("Enable DSC failure.\n");
        return HI_FALSE;
    }

    if (mode->color_format == YCBCR420) {
        if (!(cap->y420 && src->native_y420)) {
            HDMI_DBG("Enable DSC failure.\n");
            return HI_FALSE;
        }
    }

    if (mode->color_depth == CD_30) {
        if (!(cap->dsc_10bpc && src->dsc_10bpc)) {
            HDMI_DBG("Enable DSC failure.\n");
            return HI_FALSE;
        }
    }

    if (mode->color_depth == CD_36) {
        if (!(cap->dsc_12bpc && src->dsc_12bpc)) {
            HDMI_DBG("Enable DSC failure.\n");
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

static hi_bool slice_per_line(hi_u32 *slice_num)
{
    if (*slice_num <= 1) {
        *slice_num = 1;
    } else if (*slice_num <= 2) { /* slice number 2 */
        *slice_num = 2;
    } else if (*slice_num <= 4) { /* slice number 4 */
        *slice_num = 4;
    } else if (*slice_num <= 8) { /* slice number 8 */
        *slice_num = 8;
    } else if (*slice_num <= 12) { /* slice number 12 */
        *slice_num = 12;
    } else if (*slice_num <= 16) { /* slice number 16 */
        *slice_num = 16;
    } else {
        HDMI_DBG("slice target excced max.\n");
        return false;
    }

    return true;
}

static hi_bool dsc_get_slice_num(hi_u8 max_slice,
    hi_u8 *max_slice_num, hi_u32 *max_pixel_clk_per_slice)
{
    switch (max_slice) {
        case 0:
            return HI_FALSE;
        case 1:
            *max_slice_num = 1;  /* 1 slices. */
            *max_pixel_clk_per_slice = 340; /* 340Mhz. */
            break;
        case 2:
            *max_slice_num = 2; /* 2 slices. */
            *max_pixel_clk_per_slice = 340; /* 340Mhz. */
            break;
        case 3:
            *max_slice_num = 4; /* 4 slices. */
            *max_pixel_clk_per_slice = 340; /* 340Mhz. */
            break;
        case 4:
            *max_slice_num = 8; /* 8 slices. */
            *max_pixel_clk_per_slice = 340; /* 340Mhz. */
            break;
        case 5:
            *max_slice_num = 8; /* 8 slices. */
            *max_pixel_clk_per_slice = 400; /* 400Mhz. */
            break;
        case 6:
            *max_slice_num = 12; /* 12 slices. */
            *max_pixel_clk_per_slice = 400; /* 400Mhz. */
            break;
        default:
            *max_slice_num = 16; /* 16 slices. */
            *max_pixel_clk_per_slice = 400; /* 400Mhz. */
            break;
    }
    return HI_TRUE;
}

static hi_u32 dsc_get_slice_width(struct band_mode *mode,
                                  struct hdmi_detail *detail,
                                  hi_u32 max_pixel_clk_per_slice)
{
    hi_s32 k;
    hi_s32 i = 0;
    hi_s32 slice_w = 0;
    hi_s32 slice_n = 0;
    hi_s32 fpixelclock;

    /* step 1 */
    if (mode->color_format == RGB444 || mode->color_format == YCBCR444) {
        k = 2; /* The pixel clock need multiply by 2. */
    } else {
        k = 1;
    }

    fpixelclock = k * detail->pixel_clock / 2; /* The pixel unit is KHZ, need divide by 2 */

    if (fpixelclock <= DSC_MAX_PIXEL_CLOCK) {
        slice_n = DIV_ROUND_UP(fpixelclock, DSC_MAX_PIXEL_340KHZ);
    } else {
        if (max_pixel_clk_per_slice == DSC_MAX_PIXEL_340MHZ) {
            HDMI_DBG("less than 2720MHz!\n");
            return 0;
        }
        slice_n = DIV_ROUND_UP(fpixelclock, DSC_MAX_PIXEL_400KHZ);
    }

    do {
        slice_n = slice_n + i;
        if (!slice_per_line(&slice_n)) {
            return 0;
        }
        if (slice_n <= 0) {
            return 0;
        }
        slice_w = detail->h_active / slice_n;
        i++;
    } while (slice_w > 2720); /* max slice width 2720 */

    if (slice_n > max_pixel_clk_per_slice) {
        HDMI_DBG("slice num excced max.\n");
        return 0;
    }

    return slice_w;
}

static hi_bool dsc_slice_cmp(struct hdmi_connector *connector,
                             struct hdmi_detail *detail,
                             struct hdmi_valid_mode *valid_mode)
{
    hi_u8 max_slice_num;
    hi_u32 max_pixel_clk_per_slice;
    hi_u32 slice_width;
    struct dsc_property *cap = &connector->dsc;
    struct src_hdmi *src = &connector->src_cap.hdmi;
    struct band_mode *mode = &valid_mode->band;

    if (!dsc_get_slice_num(cap->max_slice, &max_slice_num, &max_pixel_clk_per_slice)) {
        HDMI_DBG("cap->max_slice is zero, sink not support dsc!\n");
        return HI_FALSE;
    }

    max_slice_num = osal_min(max_slice_num, src->max_silce_count);
    max_pixel_clk_per_slice = osal_min(max_pixel_clk_per_slice, src->max_pixel_clk_per_slice);

    slice_width = dsc_get_slice_width(mode, detail, max_pixel_clk_per_slice);
    if (slice_width) {
        valid_mode->dsc.slice_width = slice_width;
    } else {
        return HI_FALSE;
    }

    return HI_TRUE;
}

static hi_bool dsc_bpp_cmp(struct hdmi_connector *connector,
                           struct hdmi_detail *detail,
                           struct frl_requirements *frl_req,
                           struct hdmi_valid_mode *valid_mode)
{
    hi_s32 bpp_min, bpp_max, bpp_target;

    struct dsc_property *cap = &connector->dsc;
    struct band_mode *mode = &valid_mode->band;

    bpp_target = frl_req->dsc_bpp_target;

    if (mode->color_format == RGB444 ||
        mode->color_format == YCBCR444) {
        bpp_min = DSC_YUV444_MIN_BPP * DSC_BPP_FACTOR_16;
        bpp_max = DSC_CHUNK_SIZE_FACTOR * (cap->total_chunk_bytes + 1) *
                  DSC_MAX_SLICE_WIDTH * DSC_BPP_FACTOR_16 / detail->h_active;
    } else if (mode->color_format == YCBCR422) {
        bpp_min = DSC_YUV422_MIN_BPP * DSC_BPP_FACTOR_32;
        bpp_max = DSC_CHUNK_SIZE_FACTOR * (cap->total_chunk_bytes + 1) *
                  DSC_MAX_SLICE_WIDTH * DSC_BPP_FACTOR_32 / detail->h_active;
    } else if (mode->color_format == YCBCR420) {
        bpp_min = DSC_YUV420_MIN_BPP * DSC_BPP_FACTOR_32;
        bpp_max = DSC_CHUNK_SIZE_FACTOR * (cap->total_chunk_bytes + 1) *
                  DSC_MAX_SLICE_WIDTH * DSC_BPP_FACTOR_32 / detail->h_active;
    } else {
        HDMI_DBG("No such color_deepth.\n");
        return HI_FALSE;
    }

    if (bpp_max < bpp_min) {
        HDMI_DBG("No such bpp.\n");
        return HI_FALSE;
    }

    if (bpp_target > bpp_max && cap->all_bpp == 0) {
        HDMI_DBG("bpp target is not right.\n");
        return HI_FALSE;
    }

    if (bpp_target > bpp_max && cap->all_bpp == 1) {
        valid_mode->dsc.bpp_target = bpp_max;
    } else {
        valid_mode->dsc.bpp_target = bpp_target;
    }

    return true;
}

hi_bool connector_dsc_check(struct hdmi_connector *connector,
                            struct hdmi_detail *detail,
                            struct hdmi_valid_mode *valid_mode,
                            struct frl_requirements *frl_req)
{
    if (!dsc_caps_decision(connector, &valid_mode->band)) {
        return false;
    }

    if (!dsc_slice_cmp(connector, detail, valid_mode)) {
        return false;
    }

    if (!dsc_bpp_cmp(connector, detail, frl_req, valid_mode)) {
        return false;
    }

    return true;
}

hi_bool hdmi_check_valid_mode(struct hdmi_connector *connector,
                              struct hdmi_display_mode *dis_mode,
                              struct hdmi_valid_mode *valid_mode)
{
    struct frl_requirements *frl_req = NULL;
    struct src_hdmi *src = NULL;
    struct hdmi_display_info *info = NULL;
    struct dsc_property *dsc = NULL;
    /* struct dfm_mode dfm ; */
    hi_bool dsc_support = false;

    if ((connector == HI_NULL) || (valid_mode == HI_NULL)) {
        return false;
    }

    src = &connector->src_cap.hdmi;
    info = &connector->display_info;
    dsc = &connector->dsc;

    valid_mode->tmds_encode = false;
    valid_mode->frl.frl_uncompress = false;
    valid_mode->dsc.frl_compress = false;

    /* tmds */
    if (valid_mode->tmds_clock <
        osal_min(info->max_tmds_clock, src->max_tmds_clock)) {
        valid_mode->tmds_encode = true;

        if (valid_mode->tmds_clock > HDMI14_MAX_TMDS_CLK) {
            if (connector->scdc.present && src->scdc_present) {
                valid_mode->hdmi_mode = HDMI_MODE_20;
                valid_mode->tmds_scdc_en = true;
            } else {
                /* edid error */
                valid_mode->tmds_encode = false;
            }
        } else {
            valid_mode->hdmi_mode = (info->has_hdmi_infoframe &&
                                     src->hdmi_support)
                                    ? HDMI_MODE_14
                                    : HDMI_MODE_DVI;
            valid_mode->tmds_scdc_en =
                !!((valid_mode->hdmi_mode == HDMI_MODE_14) &&
                   src->scdc_lte_340mcsc &&
                   connector->scdc.lte_340mcsc);
        }
    } else {
        valid_mode->tmds_encode = false;
        valid_mode->tmds_scdc_en = false;
        valid_mode->hdmi_mode = HDMI_MODE_21;
    }

    /* frl & dsc */
    if (connector->scdc.present &&
        connector->display_info.max_frl_rate &&
        src->max_frl_rate) {
        frl_req = frl_req_search_by_band(&valid_mode->band);
        if (frl_req) {
            valid_mode->frl.max_frl_rate =
                osal_min(info->max_frl_rate, src->max_frl_rate);
            valid_mode->frl.min_frl_rate = frl_req->min_frl_rate;
            valid_mode->frl.frl_uncompress = !!(frl_req->frl_uncompress &&
                                                frl_req->min_frl_rate <= valid_mode->frl.max_frl_rate);
            valid_mode->frl.ffe_levels = src->ffe_levels;
            dsc_support = connector_dsc_check(connector, &dis_mode->detail,
                                              valid_mode, frl_req);

            valid_mode->dsc.frl_compress =
                    (frl_req->frl_compress && dsc_support);

            if (dsc_support) {
                valid_mode->dsc.max_dsc_frl_rate =
                    osal_min(dsc->dsc_max_rate, src->max_frl_rate);
                valid_mode->dsc.min_dsc_frl_rate = frl_req->min_dsc_frl_rate;
                valid_mode->dsc.hcactive = frl_req->dsc_hcactive;
                valid_mode->dsc.hcblank = frl_req->dsc_hcblank;
            }
        }
    }

    if ((!valid_mode->tmds_encode) &&
        (!valid_mode->frl.frl_uncompress) &&
        (!valid_mode->dsc.frl_compress)) {
        return false;
    }

    return hdmi_create_valid_mode(connector, valid_mode);
}

void add_420_valid_mode(struct hdmi_connector *connector,
                        struct hdmi_display_mode *dis_mode)
{
    struct hdmi_valid_mode valid_mode;
    struct color_depth *depth = NULL;
    struct src_hdmi *src = NULL;
    struct hdmi_detail *detail = HI_NULL;
    hi_bool hdmi_support = HI_FALSE;

    if (memset_s(&valid_mode, sizeof(valid_mode), 0x0, sizeof(struct hdmi_valid_mode))) {
        HDMI_ERR("memset_s failed!\n");
        return;
    }

    if ((connector == HI_NULL) || (dis_mode == HI_NULL)) {
        return;
    }

    detail = &dis_mode->detail;
    src = &connector->src_cap.hdmi;
    depth = &connector->color.depth;
    hdmi_support = connector->display_info.has_hdmi_infoframe &&
                   connector->src_cap.hdmi.hdmi_support;

    dis_mode->color_cap &= ~COLOR_Y420_24;
    if (hdmi_support && depth->y420_24 && src->ycbcr420) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR420;
        valid_mode.band.color_depth = CD_24;
        valid_mode.tmds_clock = detail->pixel_clock / 2; /* 1/2 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y420_24;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y420_30;
    if (hdmi_support && depth->y420_30 && src->ycbcr420 && src->bpc_30) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR420;
        valid_mode.band.color_depth = CD_30;
        valid_mode.tmds_clock = detail->pixel_clock * 5 / 8; /* 5/8: 0.625 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y420_30;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y420_36;
    if (hdmi_support && depth->y420_36 && src->ycbcr420 && src->bpc_36) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR420;
        valid_mode.band.color_depth = CD_36;
        valid_mode.tmds_clock = detail->pixel_clock * 3 / 4; /* 3/4: 0.75 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y420_36;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y420_48;
    if (hdmi_support && depth->y420_48 && src->ycbcr420 && src->bpc_48) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR420;
        valid_mode.band.color_depth = CD_48;
        valid_mode.tmds_clock = detail->pixel_clock;
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y420_48;
            dis_mode->status = MODE_OK;
        }
    }
}

void add_n420_valid_mode(struct hdmi_connector *connector,
                         struct hdmi_display_mode *dis_mode)
{
    struct hdmi_valid_mode valid_mode;
    struct color_depth *depth = HI_NULL;
    struct src_hdmi *src = HI_NULL;
    struct hdmi_detail *detail = HI_NULL;
    hi_bool hdmi_support = HI_FALSE;

    if (memset_s(&valid_mode, sizeof(valid_mode), 0x0, sizeof(struct hdmi_valid_mode))) {
        HDMI_ERR("memset_s failed!\n");
        return;
    }

    if ((connector == HI_NULL) || (dis_mode == HI_NULL)) {
        return;
    }

    detail = &dis_mode->detail;
    src = &connector->src_cap.hdmi;
    depth = &connector->color.depth;
    hdmi_support = connector->display_info.has_hdmi_infoframe &&
                   connector->src_cap.hdmi.hdmi_support;

    dis_mode->color_cap &= ~COLOR_RGB_24;
    if (depth->rgb_24) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = RGB444;
        valid_mode.band.color_depth = CD_24;
        valid_mode.tmds_clock = detail->pixel_clock;
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_RGB_24;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_RGB_30;
    if (hdmi_support && depth->rgb_30 && src->bpc_30) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = RGB444;
        valid_mode.band.color_depth = CD_30;
        valid_mode.tmds_clock = detail->pixel_clock * 5 / 4; /* 5/4: 1.25 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_RGB_30;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_RGB_36;
    if (hdmi_support && depth->rgb_36 && src->bpc_36) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = RGB444;
        valid_mode.band.color_depth = CD_36;
        valid_mode.tmds_clock = detail->pixel_clock * 3 / 2; /* 3/2: 1.5 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_RGB_36;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_RGB_48;
    if (hdmi_support && depth->rgb_48 && src->bpc_48) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = RGB444;
        valid_mode.band.color_depth = CD_48;
        valid_mode.tmds_clock = detail->pixel_clock * 2; /* 2 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_RGB_48;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y444_24;
    if (hdmi_support && depth->y444_24 && src->ycbcr444) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR444;
        valid_mode.band.color_depth = CD_24;
        valid_mode.tmds_clock = detail->pixel_clock;
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y444_24;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y444_30;
    if (hdmi_support && depth->y444_30 && src->bpc_30 && src->ycbcr444) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR444;
        valid_mode.band.color_depth = CD_30;
        valid_mode.tmds_clock = detail->pixel_clock * 5 / 4; /* 5/4: 1.25 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y444_30;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y444_36;
    if (hdmi_support && depth->y444_36 && src->bpc_36 && src->ycbcr444) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR444;
        valid_mode.band.color_depth = CD_36;
        valid_mode.tmds_clock = detail->pixel_clock * 3 / 2; /* 3/2: 1.5 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y444_36;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y444_48;
    if (hdmi_support && depth->y444_48 && src->bpc_48 && src->ycbcr444) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR444;
        valid_mode.band.color_depth = CD_48;
        valid_mode.tmds_clock = detail->pixel_clock * 2; /* 2 times */
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y444_48;
            dis_mode->status = MODE_OK;
        }
    }

    dis_mode->color_cap &= ~COLOR_Y422;
    if (connector->color.format.ycbcr422 && src->ycbcr422) {
        valid_mode.band.vic = dis_mode->vic;
        valid_mode.band.color_format = YCBCR422;
        valid_mode.band.color_depth = CD_24;
        valid_mode.tmds_clock = detail->pixel_clock;
        if (hdmi_check_valid_mode(connector, dis_mode, &valid_mode)) {
            dis_mode->color_cap |= COLOR_Y422;
            dis_mode->status = MODE_OK;
        }
    }
}

/*
 * hdmi_connector_add_modes - add modes from EDID & source property, if
 * available
 * @connector: connector we're probing
 * @edid: EDID data
 * Add the specified modes to the connector's mode list. Also fills out the
 * ELD in @connector with any information which can be derived from the edid.
 * Return: The number of modes added or 0 if we couldn't find any.
 */
static hi_s32 hdmi_connector_add_validmodes(struct hdmi_connector *connector)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *pos = HI_NULL;
    struct hdmi_display_mode *dis_mode = HI_NULL;

    if (connector == HI_NULL) {
        return -EINVAL;
    }

    if (osal_list_empty(&connector->probed_modes)) {
        return 0;
    }

    osal_list_for_each_safe(pos, n, &connector->probed_modes) {
        dis_mode = osal_list_entry(pos, struct hdmi_display_mode, head);
        if (dis_mode && ((dis_mode->parse_type &
                          MODE_TYPE_Y420VDB) != MODE_TYPE_Y420VDB)) {
            add_n420_valid_mode(connector, dis_mode);
        }

        if ((dis_mode->parse_type & MODE_TYPE_Y420VDB) ||
            (dis_mode->parse_type & MODE_TYPE_VDB_Y420CMDB)) {
            add_420_valid_mode(connector, dis_mode);
        }
    }

    return 0;
}

/*
 * hdmi_connector_search_mode - search hdmi mode from display mode, if
 * available
 * @connector: connector we're probing
 * @mode: display mode
 * iterate the connector's mode list, and find out the specified display mode's
 * matching hdmi mode, hdmi mode is used internally in hdmi and display mode is
 * used to communicate with vo.
 * hdmi mode includes frl/dsc/scdc setting of the specified display d_mode->
 * Return: The target display mode or NULL if the display mode is invalid.
 */
hi_bool hdmi_connector_search_mode(struct hdmi_connector *connector,
                                   struct hdmi_mode_config *mode)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *p = HI_NULL;
    struct hdmi_valid_mode *tmp_mode = HI_NULL;
    hi_bool ret = HI_FALSE;

    if ((connector == HI_NULL) || (mode == HI_NULL)) {
        return false;
    }

    osal_mutex_lock(&connector->mutex);

    if (osal_list_empty(&connector->valid_modes)) {
        osal_mutex_unlock(&connector->mutex);
        return false;
    }

    osal_list_for_each_safe(p, n, &connector->valid_modes) {
        tmp_mode = osal_list_entry(p, struct hdmi_valid_mode, head);
        if (tmp_mode->band.vic == mode->band.vic &&
            tmp_mode->band.color_format == mode->band.color_format &&
            tmp_mode->band.color_depth == mode->band.color_depth &&
            tmp_mode->valid) {
            mode->band = tmp_mode->band;
            mode->dsc = tmp_mode->dsc;
            mode->frl = tmp_mode->frl;
            mode->hdmi_mode = tmp_mode->hdmi_mode;
            mode->tmds_scdc_en = tmp_mode->tmds_scdc_en;
            mode->tmds_encode = tmp_mode->tmds_encode;
            mode->tmds_clock = tmp_mode->tmds_clock;
            ret = true;
            break;
        }
    }

    osal_mutex_unlock(&connector->mutex);

    return ret;
}

hi_void hdmitx_connector_tmds_force_config(struct hdmi_mode_config *mode,
    struct hdmi_timing_data *timing_data)
{
    hi_u32 color_fmt;
    hi_u32 color_depth;
    hi_u32 pixel_clock;

    pixel_clock = timing_data->in.pixel_clock;
    color_fmt = timing_data->out.color.color_format;
    color_depth = timing_data->out.color_depth;

    mode->tmds_clock = hdmitx_get_tmds_clock(pixel_clock, color_depth, color_fmt);

    /* tmds */
    if (mode->tmds_clock < HDMI20_MAX_TMDS_CLK) {
        mode->tmds_encode = HI_TRUE;

        if (mode->tmds_clock > HDMI14_MAX_TMDS_CLK) {
                mode->hdmi_mode = HDMI_MODE_20;
                mode->tmds_scdc_en = HI_TRUE;
        } else {
            mode->hdmi_mode = HDMI_MODE_14;
            mode->tmds_scdc_en = HI_FALSE;
        }
    } else {
        mode->tmds_encode = HI_FALSE;
        mode->tmds_scdc_en = HI_FALSE;
        mode->hdmi_mode = HDMI_MODE_21;
    }
}

hi_void hdmitx_connector_frl_force_config(struct hdmi_mode_config *mode,
    struct src_hdmi *src, struct hdmi_detail *detail)
{
    struct frl_requirements *frl_req = HI_NULL;

    frl_req = frl_req_search_by_band(&mode->band);
    if (frl_req) {
        mode->frl.max_frl_rate = src->max_frl_rate;
        mode->frl.min_frl_rate = frl_req->min_frl_rate;
        mode->frl.frl_uncompress = !!(frl_req->frl_uncompress && frl_req->min_frl_rate <= mode->frl.max_frl_rate);
        mode->frl.ffe_levels = src->ffe_levels;
        mode->dsc.frl_compress = frl_req->frl_compress;
        mode->dsc.max_dsc_frl_rate = src->max_frl_rate;
        mode->dsc.min_dsc_frl_rate = frl_req->min_dsc_frl_rate;
        mode->dsc.hcactive = frl_req->dsc_hcactive;
        mode->dsc.hcblank = frl_req->dsc_hcblank;
        mode->dsc.bpp_target = frl_req->dsc_bpp_target;
        mode->dsc.slice_width = dsc_get_slice_width(&mode->band, detail, src->max_pixel_clk_per_slice);
    }
}

hi_void drv_hdmitx_connector_create_force_mode(struct hdmi_connector *connector,
    struct hdmi_mode_config *mode)
{
    struct hisilicon_hdmi *hdmi = HI_NULL;
    struct hdmi_controller *controller = HI_NULL;
    struct hdmi_timing_data *timing_data = HI_NULL;
    struct src_hdmi *src = HI_NULL;
    struct hdmi_detail detail;

    if ((connector == HI_NULL) || (mode == HI_NULL)) {
        HDMI_ERR("ptr is null.\n");
        return;
    }
    src = &connector->src_cap.hdmi;
    hdmi = connector->hdmi_dev;
    controller = hdmi->ctrl;
    timing_data = &controller->mode.timing_data;

    /* just need pixel_clock&h_active. */
    if (memset_s(&detail, sizeof(struct hdmi_detail), 0, sizeof(struct hdmi_detail))) {
        HDMI_ERR("memset_s fail.\n");
        return;
    }

    detail.pixel_clock = timing_data->in.pixel_clock;
    detail.h_active = timing_data->in.detail.hactive;

    mode->tmds_encode = HI_FALSE;
    mode->frl.frl_uncompress = HI_FALSE;
    mode->dsc.frl_compress = HI_FALSE;
    hdmitx_connector_tmds_force_config(mode, timing_data);
    hdmitx_connector_frl_force_config(mode, src, &detail);
}

void delete_color_mode(struct hdmi_valid_mode *valid_mode,
                       struct hdmi_display_mode *dis_mode)
{
    if ((valid_mode == HI_NULL) || (dis_mode == HI_NULL)) {
        return;
    }

    if (dis_mode->vic != valid_mode->band.vic) {
        return;
    }

    if (valid_mode->band.color_format == YCBCR444 &&
        valid_mode->band.color_depth == CD_24) {
        dis_mode->color_cap &= ~COLOR_Y444_24;
    } else if (valid_mode->band.color_format == YCBCR444 &&
               valid_mode->band.color_depth == CD_30) {
        dis_mode->color_cap &= ~COLOR_Y444_30;
    } else if (valid_mode->band.color_format == YCBCR444 &&
               valid_mode->band.color_depth == CD_36) {
        dis_mode->color_cap &= ~COLOR_Y444_36;
    } else if (valid_mode->band.color_format == YCBCR444 &&
               valid_mode->band.color_depth == CD_48) {
        dis_mode->color_cap &= ~COLOR_Y444_48;
    } else if (valid_mode->band.color_format == RGB444 &&
               valid_mode->band.color_depth == CD_24) {
        dis_mode->color_cap &= ~COLOR_RGB_24;
    } else if (valid_mode->band.color_format == RGB444 &&
               valid_mode->band.color_depth == CD_30) {
        dis_mode->color_cap &= ~COLOR_RGB_30;
    } else if (valid_mode->band.color_format == RGB444 &&
               valid_mode->band.color_depth == CD_36) {
        dis_mode->color_cap &= ~COLOR_RGB_36;
    } else if (valid_mode->band.color_format == RGB444 &&
               valid_mode->band.color_depth == CD_48) {
        dis_mode->color_cap &= ~COLOR_RGB_48;
    } else if (valid_mode->band.color_format == YCBCR420 &&
               valid_mode->band.color_depth == CD_24) {
        dis_mode->color_cap &= ~COLOR_Y420_24;
    } else if (valid_mode->band.color_format == YCBCR420 &&
               valid_mode->band.color_depth == CD_30) {
        dis_mode->color_cap &= ~COLOR_Y420_30;
    } else if (valid_mode->band.color_format == YCBCR420 &&
               valid_mode->band.color_depth == CD_36) {
        dis_mode->color_cap &= ~COLOR_Y420_36;
    } else if (valid_mode->band.color_format == YCBCR420 &&
               valid_mode->band.color_depth == CD_48) {
        dis_mode->color_cap &= ~COLOR_Y420_48;
    } else if (valid_mode->band.color_format == YCBCR422) {
        dis_mode->color_cap &= ~COLOR_Y422;
    } else {
        HDMI_WARN("Invalid color depth and format\n");
        /* nothing; */
    }
}

/*
 * hdmi_connector_delete_modes - delete modes which tmds clock larger than
 * the @mode
 * @connector: connector we're probing
 * @mode: delete from the mode
 */
hi_s32 hdmi_connector_delete_modes(struct hdmi_connector *connector,
                                   struct hdmi_mode_config *mode)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *p = HI_NULL;
    struct osal_list_head *nn = HI_NULL;
    struct osal_list_head *pp = HI_NULL;
    struct hdmi_valid_mode *valid_mode = NULL;
    struct hdmi_display_mode *dis_mode = NULL;

    if ((connector == HI_NULL) || (mode == HI_NULL)) {
        return -EINVAL;
    }

    osal_mutex_lock(&connector->mutex);

    if (osal_list_empty(&connector->valid_modes) ||
        osal_list_empty(&connector->probed_modes)) {
        osal_mutex_unlock(&connector->mutex);
        return -EINVAL;
    }

    osal_list_for_each_safe(p, n, &connector->valid_modes) {
        valid_mode = osal_list_entry(p, struct hdmi_valid_mode, head);
        if (valid_mode->frl.min_frl_rate >= mode->frl.min_frl_rate) {
            valid_mode->valid = false;
            osal_list_for_each_safe(pp, nn, &connector->probed_modes) {
                dis_mode = osal_list_entry(pp, struct hdmi_display_mode, head);
                if ((dis_mode != HI_NULL) && (dis_mode->vic == valid_mode->band.vic)) {
                    delete_color_mode(valid_mode, dis_mode);
                    if ((dis_mode != HI_NULL) && !dis_mode->color_cap) {
                        osal_list_del(&dis_mode->head);
                        osal_kfree(HI_ID_HDMITX, dis_mode);
                    }
                }
            }
            break;
        }
    }
    osal_mutex_unlock(&connector->mutex);

    return 0;
}

hi_s32 hdmi_connector_get_availmode(struct hdmi_connector *connector,
                                    hdmi_avail_mode *mode_buf, hi_u32 cnt)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *p = HI_NULL;
    struct hdmi_display_mode *tmp_mode = HI_NULL;
    hi_u32 i = 0;
    hi_s32 ret = 0;

    if ((connector == HI_NULL) || (mode_buf == HI_NULL) || (cnt == HI_NULL)) {
        return -EINVAL;
    }

    if (osal_list_empty(&connector->probed_modes)) {
        osal_mutex_lock(&connector->mutex);
        return -ESPIPE;
    }

    osal_list_for_each_safe(p, n, &connector->probed_modes) {
        tmp_mode = osal_list_entry(p, struct hdmi_display_mode, head);
        mode_buf[i].vic = tmp_mode->vic;
        mode_buf[i].detail_timing = tmp_mode->detail;
        mode_buf[i].color_space = tmp_mode->color_cap;
        i++;
        if (i >= cnt || i >= connector->probed_mode_cnt) {
            break;
        }
    }

    osal_mutex_unlock(&connector->mutex);

    return ret;
}

hi_s32 hdmi_connector_get_native_mode(struct hdmi_connector *connector,
                                      hdmi_avail_mode *avail_mode)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *p = HI_NULL;
    struct hdmi_display_mode *tmp_mode = HI_NULL;
    struct hdmi_display_mode *nat_mode = HI_NULL;
    hi_s32 ret = 0;

    if (connector == NULL || avail_mode == NULL) {
        HDMI_ERR("Input params is NULL pointer!\n");
        return -EINVAL;
    }

    osal_mutex_lock(&connector->mutex);
    if (osal_list_empty(&connector->probed_modes)) {
        osal_mutex_unlock(&connector->mutex);
        return -ESPIPE;
    }

    nat_mode = osal_list_entry(connector->probed_modes.next, struct hdmi_display_mode, head);

    osal_list_for_each_safe(p, n, &connector->probed_modes) {
        tmp_mode = osal_list_entry(p, struct hdmi_display_mode, head);
        if (tmp_mode->native_mode) {
            nat_mode = tmp_mode;
            break;
        }

        if (tmp_mode->first_mode) {
            nat_mode = tmp_mode;
        }
    }

    avail_mode->vic = nat_mode->vic;
    avail_mode->detail_timing = nat_mode->detail;
    avail_mode->color_space = nat_mode->color_cap;

    osal_mutex_unlock(&connector->mutex);

    return ret;
}

hi_s32 hdmi_connector_get_max_mode(struct hdmi_connector *connector,
                                   hdmi_avail_mode *avail_mode)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *p = HI_NULL;
    struct hdmi_display_mode *tmp_mode = HI_NULL;
    struct hdmi_detail *detail = HI_NULL;
    hi_u64 product;
    hi_u64 max_prod = 0;
    hi_s32 ret = 0;

    if ((connector == HI_NULL) || (avail_mode == HI_NULL)) {
        return -EINVAL;
    }

    osal_mutex_lock(&connector->mutex);
    if (osal_list_empty(&connector->probed_modes)) {
        osal_mutex_unlock(&connector->mutex);
        return -ESPIPE;
    }
    osal_list_for_each_safe(p, n, &connector->probed_modes) {
        tmp_mode = osal_list_entry(p, struct hdmi_display_mode, head);
        detail = &tmp_mode->detail;
        product = detail->field_rate * detail->h_active * detail->v_active;
        if (product > max_prod) {
            max_prod = product;
            avail_mode->vic = tmp_mode->vic;
            avail_mode->detail_timing = tmp_mode->detail;
            avail_mode->color_space = tmp_mode->color_cap;
        }
    }

    osal_mutex_unlock(&connector->mutex);

    return ret;
}

/*
 * hdmi_connector_destroy_modes - destroy modes list
 * @connector: connector we're probing
 */
static hi_s32 hdmi_connector_destroy_modes(struct hdmi_connector *connector)
{
    struct osal_list_head *n = HI_NULL;
    struct osal_list_head *p = HI_NULL;
    struct hdmi_valid_mode *valid_mode = HI_NULL;
    struct hdmi_display_mode *dis_mode = HI_NULL;

    if (connector == HI_NULL) {
        return -EINVAL;
    }

    if (!osal_list_empty(&connector->valid_modes)) {
        osal_list_for_each_safe(p, n, &connector->valid_modes) {
            valid_mode = osal_list_entry(p, struct hdmi_valid_mode, head);
            if (valid_mode != HI_NULL) {
                hdmi_destroy_valid_mode(connector, valid_mode);
            }
        }
    }

    if (!osal_list_empty(&connector->probed_modes)) {
        osal_list_for_each_safe(p, n, &connector->probed_modes) {
            dis_mode = osal_list_entry(p, struct hdmi_display_mode, head);
            if (dis_mode != HI_NULL) {
                osal_list_del(&dis_mode->head);
                hdmi_mode_destroy(connector->hdmi_dev, dis_mode);
            }
        }
    }
    connector->probed_mode_cnt = 0;

    return 0;
}
