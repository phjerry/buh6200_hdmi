/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal vi
 * Author: sdk
 * Create: 2019-12-14
 */

#include <linux/types.h>
#include "hi_osal.h"

#include "vi_csc.h"
#include "hal_vicap.h"
#include "hal_vi.h"

#define VI_TMP 0

/* 144 auto gen testpattern */
static vicap_hal_pt_gendata_cfg g_gen_data_cfg[VI_HOST_GEN_DATA_TYPE_MAX] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* 0: off */
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, /* 1: gradation */
    {1, 3, 0, 0, 0, 0, 0, 0, 0, 0}, /* 2: move */
    {1, 2, 0, 0, 0, 0, 0, 0, 0, 0}, /* 3: red */
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0}, /* 4: green */
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0}, /* 5: blue */
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0}, /* 6: wihte */
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0}, /* 7: black */
};

/* 148 */
static vicap_hal_pt_gendata_coef g_gen_data_coef[VI_HOST_GEN_DATA_TYPE_MAX] = {
    {0, 0, 0, 0}, /* 0: off */
    {0, 0, 0, 0}, /* 1: */
    {0, 0, 0, 0}, /* 2: */
    {0, 0, 0, 0}, /* 3: */
    {0, 0, 0, 0}, /* 4: */
    {0, 0, 0, 0}, /* 5: */
    {0, 0, 0, 0}, /* 6: */
    {0, 0, 0, 0}, /* 7: */
};

/* 14c */
static vicap_hal_pt_gendata_init g_gen_data_init[VI_HOST_GEN_DATA_TYPE_MAX] = {
    {0x0, 0x0, 0x0},    /* 0: off */
    {0x0, 0x0, 0x0},    /* 1: */
    {0x0, 0x0, 0x0},    /* 2: */
    {0x0, 0x0, 0x0},    /* 3: */
    {0x80, 0x0, 0x0},   /* 4: green */
    {0x11, 0xff, 0x55}, /* 5: blue */
    {0xeb, 0x80, 0x80}, /* 6: wihte */
    {0x10, 0x80, 0x80}, /* 7: black */
};

typedef struct {
    vi_host_instance instance[VI_MAX_IP_NUM];
} vi_host_ctx;

VI_STATIC vi_host_ctx g_vi_host_ctx = {0};

VI_STATIC hi_void hal_vi_init_ctx(hi_void)
{
    vi_host_instance *host_instance_p = HI_NULL;

    /* 第一个IP的支持能力 */
    host_instance_p = &g_vi_host_ctx.instance[0];
    host_instance_p->vicap_id = 0;
    host_instance_p->irq_num = (137 + 32); /* 137 32 is irq num */
    host_instance_p->irq_name = "int_vicap0_mmu_ns";
    host_instance_p->work_clk = 648000000; /* 648000000 is ppc clk */
    host_instance_p->mix_mode = HI_FALSE;

    host_instance_p->host_caps.cap_in_max_width = VI_MAX_UHD_WIDTH;
    host_instance_p->host_caps.cap_in_max_height = VI_MAX_UHD_HEIGHT;
    host_instance_p->host_caps.cap_smmu_type = VI_HOST_CAP_SMMU_ALONE;
    host_instance_p->host_caps.cap_in_max_pix_clk = VI_MAX_IN_PIX_CLK;
    host_instance_p->host_caps.cap_in_hdr_type = VI_HOST_CAP_HDR_TYPE_SDR;
    host_instance_p->host_caps.cap_out_full_type = VI_HOST_CAP_FULL_TYPE_ALL;
    host_instance_p->host_caps.cap_check_hdr_type = VI_HOST_CAP_CHECK_HDR_TYPE_NO;
    host_instance_p->host_caps.cap_low_delay_mode = VI_HOST_CAP_LOW_DELAY_MODE_TUNNEL;

    /* 第二个IP的支持能力 */
    host_instance_p = &g_vi_host_ctx.instance[1];
    host_instance_p->vicap_id = 1;
    host_instance_p->irq_num = (138 + 32); /* 138 32 is irq num */
    host_instance_p->irq_name = "int_vicap1_mmu_ns";
    host_instance_p->work_clk = 648000000; /* 648000000 is ppc clk */
    host_instance_p->mix_mode = HI_FALSE;

    host_instance_p->host_caps.cap_in_max_width = VI_MAX_UHD_WIDTH_8K;
    host_instance_p->host_caps.cap_in_max_height = VI_MAX_UHD_HEIGHT_8K;
    host_instance_p->host_caps.cap_smmu_type = VI_HOST_CAP_SMMU_ALONE;
    host_instance_p->host_caps.cap_in_max_pix_clk = VI_MAX_IN_PIX_CLK;
    host_instance_p->host_caps.cap_in_hdr_type = VI_HOST_CAP_HDR_TYPE_ALL;
    host_instance_p->host_caps.cap_out_full_type = VI_HOST_CAP_FULL_TYPE_ALL;
    host_instance_p->host_caps.cap_check_hdr_type = VI_HOST_CAP_CHECK_HDR_TYPE_ALL;
    host_instance_p->host_caps.cap_low_delay_mode = VI_HOST_CAP_LOW_DELAY_ALL;
}

VI_STATIC hi_void vicap_drv_comm_set_hal_clk_gate_en(hi_bool enable)
{
    vicap_hal_crg_set_bus_clk_en(enable);
    vicap_hal_crg_set_ppc_clk_en(enable);
}

VI_STATIC hi_void vicap_drv_comm_set_hal_reset_en(hi_bool enable)
{
    vicap_hal_crg_set_bus_reset(enable);
}

VI_STATIC hi_void vicap_drv_ctrl_set_hal_work_clk(hi_u32 vicap_id)
{
    if (vicap_id == HI_DRV_VICAP_PORT0) {
#if VI_TMP
        vicap_hal_crg_set_hdmirx2_vi_clk(VICAP_HAL_CHN_HDMIRX_WORKHZ_SPECIAL); /* HDMI 非 420 */
#endif
        vicap_hal_crg_set_hdmirx2_vi_clk(VICAP_HAL_CHN_HDMIRX_WORKHZ_NORMAL); /* HDMI 420 */
    }
}

VI_STATIC hi_void vicap_drv_ctrl_set_hal_mix_mode(hi_u32 vicap_id)
{
#if VI_TMP
    vicap_hal_global_set_mix_mode(vicap_id, HI_TRUE, VICAP_HAL_MIX_MODE_MODE0); /* HDMI 444 */
    vicap_hal_global_set_mix_mode(vicap_id, HI_TRUE, VICAP_HAL_MIX_MODE_MODE1); /* HDMI 422 */
#endif
    vicap_hal_global_set_mix_mode(vicap_id, HI_FALSE, VICAP_HAL_MIX_MODE_MODE0); /* HDMI 420 其他source */
}

VI_STATIC hi_void vicap_global_cfg(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;

    /* 开时钟 */
    vicap_drv_comm_set_hal_clk_gate_en(HI_TRUE);
    /* 关复位 */
    vicap_drv_comm_set_hal_reset_en(HI_FALSE);

    vicap_hal_crg_set_ppc_clk(VICAP_HAL_CHN_WORKHZ_648HZ);

    /* 设置工作模式 [普通/低功耗] */
    vicap_hal_global_set_power_mode(vicap_id, HI_TRUE);
    vicap_hal_global_set_axi_cfg(vicap_id, VICAP_HAL_OUTSTANDING_EIGHT);
    vicap_hal_global_set_mac_cfg(vicap_id, HI_TRUE);

    /* 设置工作时钟 */
    vicap_drv_ctrl_set_hal_work_clk(vicap_id);

    /* 设置MixMode */
    vicap_drv_ctrl_set_hal_mix_mode(vicap_id);
}

VI_STATIC hi_void vicap_drv_ctrl_set_port_yuv444(vi_host_instance *host_instance_p)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    hi_u32 vicap_id = host_instance_p->vicap_id;
    hi_bool to_yuv444 = HI_FALSE;

    /* 444 和 420 不需要转到 444 */
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV42 ||
        vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB ||
        vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        to_yuv444 = HI_FALSE;
    } else if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        to_yuv444 = HI_TRUE;
    }

    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_yuv444_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT0, to_yuv444);
        vicap_hal_pt_set_yuv444_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT1, to_yuv444); //
    } else {
        hi_u32 i;

        for (i = 0; i < VICAP_HAL_PT_INDEX_MAX; i++) {
            vicap_hal_pt_set_yuv444_cfg(vicap_id, i, to_yuv444);
        }
    }
}

VI_STATIC hi_void vicap_drv_ctrl_set_port_intf_mode(hi_u32 vicap_id)
{
    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_intf_mod(vicap_id, VICAP_HAL_PT_INDEX_PT0, VICAP_HAL_PT_INTF_MODE_FVHDE);
        vicap_hal_pt_set_intf_mod(vicap_id, VICAP_HAL_PT_INDEX_PT1, VICAP_HAL_PT_INTF_MODE_FVHDE); //
    } else {
        hi_u32 i;

        for (i = 0; i < HI_DRV_VICAP_PORT_MAX; i++) {
            vicap_hal_pt_set_intf_mod(vicap_id, i, VICAP_HAL_PT_INTF_MODE_FVHDE);
        }
    }
}

VI_STATIC hi_void vicap_drv_ctrl_set_port_data_comp_mode(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    vicap_hal_pt_comp_mode comp_mode = VICAP_HAL_PT_COMP_MODE_DOUBLE;

    /* 分量设置 */
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV42 ||
        vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB ||
        vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        comp_mode = VICAP_HAL_PT_COMP_MODE_THREE;
    } else if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        comp_mode = VICAP_HAL_PT_COMP_MODE_DOUBLE;
    }

    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_unitf_data_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT0, comp_mode);
        vicap_hal_pt_set_unitf_data_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT1, comp_mode); //
    } else {
        hi_u32 i;

        for (i = 0; i < VICAP_HAL_PT_INDEX_MAX; i++) {
            vicap_hal_pt_set_unitf_data_cfg(vicap_id, i, comp_mode);
        }
    }
}

VI_STATIC hi_void vicap_drv_ctrl_set_port_data_comp_mask(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    hi_u32 para[][3] = /* has 3 group value */
        {{0, 12, 24}, {12, 0, 24}, {24, 0, 12}, {0, 24, 12}, {24, 12, 0}, {12, 24, 0}};
    hi_u32 yuv420_data_order = 0;
    hi_u32 offset[3] = /* has 3 group value */
        {0, 12, 24};
    hi_u32 mask = 0;
    hi_u32 comp_mask[3] = {0}; /* has 3 group value */

    if (vi_instance_p->attr_in.source_type == HI_DRV_SOURCE_HDMI) {
        if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
            offset[0] = para[yuv420_data_order][0]; /* index 0 */
            offset[1] = para[yuv420_data_order][1]; /* index 1 */
            offset[2] = para[yuv420_data_order][2]; /* index 2 */
        }
    }

    /* 分量掩码 */
    if (vi_instance_p->attr_in.bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
        mask = vicap_drv_chn_data_mask(8); /* index 8 */
    } else if (vi_instance_p->attr_in.bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
        mask = vicap_drv_chn_data_mask(10); /* index 10 */
    } else if (vi_instance_p->attr_in.bit_width == HI_DRV_PIXEL_BITWIDTH_12BIT) {
        mask = vicap_drv_chn_data_mask(12); /* index 12 */
    } else {
        HI_ASSERT(0);
    }

    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB ||
        vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV42 ||
        vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        comp_mask[0] = mask; /* index 0 */
        comp_mask[1] = mask; /* index 1 */
        comp_mask[2] = mask; /* index 2 */
    } else if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        comp_mask[0] = mask; /* index 0 */
        comp_mask[1] = mask; /* index 1 */
    }

    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_offset(vicap_id, VICAP_HAL_PT_INDEX_PT0, offset, comp_mask, 3); /* has 3 group value */
        vicap_hal_pt_set_offset(vicap_id, VICAP_HAL_PT_INDEX_PT1, offset, comp_mask, 3); /* has 3 group value */
    } else {
        hi_u32 i;

        for (i = 0; i < VICAP_HAL_PT_INDEX_MAX; i++) {
            vicap_hal_pt_set_offset(vicap_id, i, offset, comp_mask, 3); /* has 3 group value */
        }
    }
}

VI_STATIC hi_void vicap_drv_ctrl_set_port_sync_cfg(hi_u32 vicap_id)
{
    vicap_hal_pt_sync_cfg sync_cfg;

    /* fvhde外同步时序 */
    sync_cfg.field_inv = HI_FALSE;

    sync_cfg.field_sel = VICAP_HAL_PT_FIELD_SEL_FIELD;
    sync_cfg.vsync_mode = VICAP_HAL_PT_VSYNC_MODE_SINGLE_UP;
    sync_cfg.vsync_inv = HI_FALSE;
    sync_cfg.vsync_sel = VICAP_HAL_PT_VSYNC_SEL_VSYNC;
    sync_cfg.hsync_mode = VICAP_HAL_PT_HSYNC_MODE_SINGLE_UP;
    sync_cfg.hsync_and = VICAP_HAL_PT_HSYNC_AND_OFF;
    sync_cfg.hsync_inv = HI_FALSE;
    sync_cfg.hsync_sel = VICAP_HAL_PT_HSYNC_SEL_HSYNC;
    sync_cfg.de_inv = HI_FALSE;
    sync_cfg.de_sel = VICAP_HAL_PT_DE_SEL_DE;

    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_unitf_timing_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT0, &sync_cfg);
        vicap_hal_pt_set_unitf_timing_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT1, &sync_cfg); //
    } else {
        hi_u32 i;

        for (i = 0; i < VICAP_HAL_PT_INDEX_MAX; i++) {
            vicap_hal_pt_set_unitf_timing_cfg(vicap_id, i, &sync_cfg);
        }
    }
}

VI_STATIC hi_void vicap_drv_ctrl_set_port_skip(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    vicap_drv_pt_skip_type pt_skip_cfg = VICAP_DRV_PT_SKIP_TYPE_OFF;

    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        if (vi_instance_p->attr_in.over_sample == HI_VI_OVERSAMPLE_4X) {
            pt_skip_cfg = VICAP_DRV_PT_SKIP_TYPE_4X;
        } else if (vi_instance_p->attr_in.over_sample == HI_VI_OVERSAMPLE_2X) {
            pt_skip_cfg = VICAP_DRV_PT_SKIP_TYPE_2X;
        } else {
            pt_skip_cfg = VICAP_DRV_PT_SKIP_TYPE_OFF;
        }
    }

    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_hor_skip(vicap_id, VICAP_HAL_PT_INDEX_PT0, pt_skip_cfg);
        vicap_hal_pt_set_hor_skip(vicap_id, VICAP_HAL_PT_INDEX_PT1, pt_skip_cfg); //
    } else {
        hi_u32 i;

        for (i = 0; i < VICAP_HAL_PT_INDEX_MAX; i++) {
            vicap_hal_pt_set_hor_skip(vicap_id, i, pt_skip_cfg);
        }
    }
}

VI_STATIC hi_void vicap_global_port_cfg(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;

    /* 设置port上采样到444 */
    vicap_drv_ctrl_set_port_yuv444(host_instance_p);

    /* 设置port的接口模式 */
    vicap_drv_ctrl_set_port_intf_mode(vicap_id);

    /* 设置port的数据分量模式 */
    vicap_drv_ctrl_set_port_data_comp_mode(host_instance_p);

    /* 设置port的数据分量的掩码和offset */
    vicap_drv_ctrl_set_port_data_comp_mask(host_instance_p);

    /* 设置信号配置 */
    vicap_drv_ctrl_set_port_sync_cfg(vicap_id);

    /* 设置port的skip */
    vicap_drv_ctrl_set_port_skip(host_instance_p);
}

VI_STATIC hi_void vicap_drv_ctrl_set_chn_dither(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    vicap_hal_chn_dither_cfg dither_cfg = VICAP_HAL_CHN_DITHER_10BIT;

    /* dither */
    switch (vi_instance_p->attr_out.bit_width) {
        case HI_DRV_PIXEL_BITWIDTH_12BIT:
            dither_cfg = VICAP_HAL_CHN_DITHER_10BIT;
            break;

        case HI_DRV_PIXEL_BITWIDTH_10BIT:
            dither_cfg = VICAP_HAL_CHN_DITHER_10BIT;
            break;

        case HI_DRV_PIXEL_BITWIDTH_8BIT:
            dither_cfg = VICAP_HAL_CHN_DITHER_8BIT;
            break;

        default:
            dither_cfg = VICAP_HAL_CHN_DITHER_8BIT;
            break;
    }

    vicap_hal_chn_set_dither_cfg(vicap_id, dither_cfg);
}

VI_STATIC hi_void vicap_drv_ctrl_set_chn_dly_fstart(hi_u32 vicap_id)
{
    hi_u32 chn_v_dly = 24;

    vicap_hal_chn_set_vdly_cfg(vicap_id, chn_v_dly);
}

VI_STATIC hi_void vicap_drv_ctrl_set_chn_skip(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    hi_u32 skip_y_cfg;
    hi_u32 skip_c_cfg;

    /* 亮度skip属性参数 */
    if (vi_instance_p->attr_in.over_sample == HI_VI_OVERSAMPLE_2X) {
        skip_y_cfg = VICAP_DRV_CHN_SKIP_CFG_2X;
    } else if (vi_instance_p->attr_in.over_sample == HI_VI_OVERSAMPLE_4X) {
        skip_y_cfg = VICAP_DRV_CHN_SKIP_CFG_4X;
    } else {
        skip_y_cfg = VICAP_DRV_CHN_SKIP_CFG_OFF;
    }

    /* YUV422时亮度skip在端口中完成 */
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        skip_y_cfg = VICAP_DRV_CHN_SKIP_CFG_OFF;
    }

    /* 默认色度与亮度的skip相同 */
    skip_c_cfg = skip_y_cfg;

    /* 源为422时，端口做色度上采样时，色度被填充为YUV444，故这里色度skip为亮度skip的两倍 */
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        skip_c_cfg = VICAP_DRV_CHN_SKIP_CFG_2X;
    }

    /* 亮度Skip和色度Skip，32bit组成4X8的矩阵确定保留的行和列，
     * 整个图像依照4X8的规律保留行和列。应用于过采样场景 */
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        skip_c_cfg = VICAP_DRV_CHN_SKIP_CFG_YUV420;
    }

    vicap_hal_chn_set_skip_ycfg(vicap_id, skip_y_cfg);
    vicap_hal_chn_set_skip_ccfg(vicap_id, skip_c_cfg);
}

VI_STATIC hi_void vicap_drv_ctrl_set_chn_remix420(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    vicap_hal_frame_size frame_size = {0};
    vicap_hal_chn_420_cfg st420_cfg = {0};

    st420_cfg.enable = HI_TRUE;
    st420_cfg.c_seq = VICAP_HAL_CHN_CSEQ_CBCR;

    /* YUV420的时序，实际的宽度只有一半 */
    frame_size.width = vi_instance_p->attr_in.width / 2 - 1; /* index 2 and 1 */
    frame_size.height = vi_instance_p->attr_in.height - 1;   /* index 1 */

    /* YUV420输入需特殊处理 */
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        vicap_hal_chn_set_remix420_cfg(vicap_id, &st420_cfg);
        vicap_hal_chn_set_remix420_size(vicap_id, &frame_size);
    }
}

VI_STATIC hi_void vicap_global_chn_cfg(vi_host_instance *host_instance_p)
{
    vicap_drv_ctrl_set_chn_dly_fstart(host_instance_p->vicap_id);
    vicap_drv_ctrl_set_chn_skip(host_instance_p);
    vicap_drv_ctrl_set_chn_dither(host_instance_p);
    vicap_drv_ctrl_set_chn_remix420(host_instance_p);
}

VI_STATIC hi_void vicap_port_enable(hi_u32 vicap_id)
{
    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_global_set_pt_sel(vicap_id);

        vicap_hal_global_set_pt_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT0, HI_FALSE);
        vicap_hal_global_set_pt_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT1, HI_FALSE);

        vicap_hal_pt_set_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT0, 0);
        vicap_hal_pt_set_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT1, 0);

        vicap_hal_pt_set_en(vicap_id, VICAP_HAL_PT_INDEX_PT0, HI_TRUE);
        vicap_hal_pt_set_en(vicap_id, VICAP_HAL_PT_INDEX_PT1, HI_TRUE); //
    } else {
        hi_u32 i;

        vicap_hal_global_set_pt_sel(vicap_id);

        for (i = 0; i < VICAP_HAL_PT_INDEX_MAX; i++) {
            vicap_hal_global_set_pt_int_mask(vicap_id, i, HI_FALSE);
            vicap_hal_pt_set_int_mask(vicap_id, i, 0);
            vicap_hal_pt_set_en(vicap_id, i, HI_TRUE);
        }
    }
}

VI_STATIC hi_void vicap_port_disable(hi_u32 vicap_id)
{
    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_en(vicap_id, VICAP_HAL_PT_INDEX_PT0, HI_FALSE);
        vicap_hal_pt_set_en(vicap_id, VICAP_HAL_PT_INDEX_PT1, HI_FALSE);

        vicap_hal_pt_set_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT0, 0);
        vicap_hal_pt_set_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT1, 0); //

        vicap_hal_global_set_pt_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT0, HI_FALSE);
        vicap_hal_global_set_pt_int_mask(vicap_id, VICAP_HAL_PT_INDEX_PT1, HI_FALSE);
    } else {
        hi_u32 i;

        for (i = 0; i < HI_DRV_VICAP_PORT_MAX; i++) {
            vicap_hal_pt_set_en(vicap_id, i, HI_FALSE);
            vicap_hal_pt_set_int_mask(vicap_id, i, 0);
            vicap_hal_global_set_pt_int_mask(vicap_id, i, HI_FALSE);
        }
    }
}

VI_STATIC hi_void vicap_chn_enable(hi_u32 vicap_id)
{
    vicap_hal_chn_set_clear_int(vicap_id, 0xffffffff);
    vicap_hal_chn_set_en(vicap_id, HI_TRUE);
}

VI_STATIC hi_void vicap_chn_enable_mask(hi_u32 vicap_id, hi_u32 int_mask)
{
    vicap_hal_global_set_chn_int_mask(vicap_id, HI_TRUE);
    vicap_hal_chn_set_int_mask(vicap_id, int_mask);
}

VI_STATIC hi_void vicap_chn_disable(hi_u32 vicap_id)
{
    vicap_hal_chn_set_en(vicap_id, HI_FALSE);
}

VI_STATIC hi_void vicap_chn_disable_mask(hi_u32 vicap_id)
{
    vicap_hal_global_set_chn_int_mask(vicap_id, HI_FALSE);
    vicap_hal_chn_set_int_mask(vicap_id, 0);
}

#if defined(HI_SMMU_SUPPORT)
VI_STATIC hi_void vicap_smmu_start(hi_u32 vicap_id)
{
    hi_ulong ptaddr;
    hi_ulong err_rdaddr;
    hi_ulong err_wraddr;

    vicap_hal_smmu_set_int_en(vicap_id, HI_TRUE);
    vicap_hal_smmu_set_global_bypass(vicap_id, HI_FALSE);
    vicap_hal_smmu_set_clk_gate_en(vicap_id, HI_TRUE);
    vicap_hal_smmu_set_int_mask(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE, VICAP_HAL_SMMU_INT_MASK);

    osal_mem_get_nssmmu_pgtinfo(&ptaddr, &err_rdaddr, &err_wraddr);

    vicap_hal_smmu_set_table_base_addr(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE, (hi_u32)ptaddr);
    vicap_hal_smmu_set_err_read_addr(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE, (hi_u32)err_rdaddr);
    vicap_hal_smmu_set_err_write_addr(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE, (hi_u32)err_wraddr);

    vicap_hal_smmu_set_table_base_addr(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_SAFE, (hi_u32)ptaddr);
    vicap_hal_smmu_set_err_read_addr(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_SAFE, (hi_u32)err_rdaddr);
    vicap_hal_smmu_set_err_write_addr(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_SAFE, (hi_u32)err_wraddr);
}

VI_STATIC hi_void vicap_smmu_stop(hi_u32 vicap_id)
{
    vicap_hal_smmu_set_int_en(vicap_id, HI_FALSE);
    vicap_hal_smmu_set_global_bypass(vicap_id, HI_TRUE);
    vicap_hal_smmu_set_clk_gate_en(vicap_id, HI_FALSE);
}
VI_STATIC hi_void vicap_drv_comm_set_hal_smmu_clk_en(hi_bool enable)
{
    vicap_hal_crg_set_smmu_clk_en(enable);
}

VI_STATIC hi_void vicap_drv_comm_set_hal_smmu_reset(hi_bool enable)
{
    vicap_hal_crg_set_smmu_reset(enable);
}

VI_STATIC hi_void hal_vi_update_smmu_int_status(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    hi_u32 chn0_smmu_status = 0;

    /* SMMU WARNING */
    vicap_hal_smmu_get_int_status(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE, &chn0_smmu_status);
    if (chn0_smmu_status) {
#if VI_TMP
        hi_dbg_func_trace_low_freq_cnt(vi_instance_p->vi_in_attr.rate * 2);   /* 2 is empirical value */
        hi_func_trace_low_freq_cnt_begin(vi_instance_p->vi_in_attr.rate * 2); /* 2 is empirical value */
        HI_LOG_ERR("SMMU error!\n");
        hi_err_print_u32(chn0_smmu_status);
        hi_func_trace_low_freq_cnt_end();
#endif
        host_instance_p->host_dbg_info.int_smmu_err_cnt++;
    }

    host_instance_p->mmu_int = chn0_smmu_status;
    vicap_hal_smmu_set_int_clr(vicap_id, VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE, chn0_smmu_status);
}
#endif

VI_STATIC hi_void hal_vi_int_process(vi_host_instance *host_instance_p)
{
#if VI_TMP
    hi_u32 vicap_id = host_instance_p->vicap_id;

    if (vi_instance_p->vi_in_attr.hdr_type == HI_DRV_VI_SOURCE_INPUT_TYPE_DOLBY) {
        /* 校验成功，按输入整形 */
        if (vi_instance_p->vi_in_attr.pix_fmt == HI_DRV_PIX_FMT_NV42_RGB) {
        } else if (vi_instance_p->vi_in_attr.pix_fmt == HI_DRV_PIX_FMT_NV61_2X1) {
        }

        /* 设置通道位宽 */
        hal_vi_set_chn_bitwidth(vi_id, vi_instance_p->vi_out_attr.bit_width);

        /* 设置输出skip 0xffffffff->0xaaaaaaaa */
        /* 输入源为rgb444 8bit，vicap内部整形为yuv422 12bit */
        if (vi_instance_p->vi_in_attr.pix_fmt == HI_DRV_PIX_FMT_NV42_RGB) {
            vicap_hal_chn_set_skip_c_cfg(vi_id, VICAP_DRV_CHN_SKIP_CFG_2X);
        }

        /* 关闭算法 */
        vicap_hal_chn_set_csc_cfg(vi_id, HI_FALSE);
        vicap_hal_chn_set_h_scale_y_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_h_scale_c_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_v_scale_y_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_v_scale_c_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_dither_en(vi_id, HI_FALSE);
    } else if (vi_instance_p->vi_in_attr.hdr_type == HI_DRV_VI_SOURCE_INPUT_TYPE_DOLBY_LATENCY) {
        /* 设置通道位宽 */
        hal_vi_set_chn_bitwidth(vi_id, vi_instance_p->vi_out_attr.bit_width);

        /* 关闭算法 */
        vicap_hal_chn_set_csc_cfg(vi_id, HI_FALSE);
        vicap_hal_chn_set_h_scale_y_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_h_scale_c_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_v_scale_y_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_v_scale_c_en(vi_id, HI_FALSE);
        vicap_hal_chn_set_dither_en(vi_id, HI_FALSE);
    }

    if (vi_instance_p->vi_in_attr.hdr_type == HI_DRV_VI_SOURCE_INPUT_TYPE_SDR) {
        if (vi_instance_p->vi_in_attr.pix_fmt == HI_DRV_PIX_FMT_NV42_RGB) {
        }

        /* set chn bit */
        hal_vi_set_chn_bitwidth(vi_id, vi_instance_p->vi_out_attr.bit_width);
    }
#endif
}

VI_STATIC hi_void hal_vi_update_int_err(vi_host_instance *host_instance_p, hi_u32 chn0_int_status)
{
    /* 场数据丢失中断 */
    if (chn0_int_status & VICAP_HAL_CHN_INT_FIELDTHROW) {
        host_instance_p->host_status.is_video_err = HI_TRUE;
        host_instance_p->host_dbg_info.filed_lost_cnt++;
    }

    /* 内部FIFO溢出错误中断状态 */
    if (chn0_int_status & VICAP_HAL_CHN_INT_BUFOVF) {
        host_instance_p->host_status.is_video_err = HI_TRUE;
        host_instance_p->host_dbg_info.buf_overflow_cnt++;
    }

    if (chn0_int_status & VICAP_HAL_CHN_INT_FRAMESTART) {
        host_instance_p->host_dbg_info.int_f_start_cnt++;
    }
    if (chn0_int_status & VICAP_HAL_CHN_INT_FSTARTDELAY) {
        host_instance_p->host_dbg_info.int_f_delay_cnt++;
    }

    if (chn0_int_status & (VICAP_HAL_CHN_INT_CC)) {
        host_instance_p->host_dbg_info.cc_int_cnt++;
    } else {
        host_instance_p->host_dbg_info.no_cc_int_cnt++;
    }

    if (chn0_int_status & (VICAP_HAL_CHN_INT_UPDATECFG)) {
        host_instance_p->host_dbg_info.update_cfg_cnt++;
    } else {
        host_instance_p->host_dbg_info.no_update_cfg_cnt++;
    }
}

VI_STATIC hi_void hal_vi_update_pt_state(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vicap_hal_frame_size frame_size = {0};

#if VI_TMP
    if ((vi_instance_p->vi_in_attr.interlace == HI_TRUE) && (vi_host_ctx_p->gen_timing_enable == HI_TRUE)) {
        vi_host_p->vi_host_statistics.cur_filed_type = !(vi_host_p->vi_host_statistics.cur_filed_type);
    } else {
        vi_host_p->vi_host_statistics.cur_filed_type = vicap_hal_pt_get_field(vi_id);
    }

    vi_host_ctx_p->mute_status = vi_cap_hal_pt_get_mut_status(VICAP_HAL_PT_INDEX_PT0);
    if (vi_host_ctx_p->mute_status) {
        vi_host_ctx_p->vi_host_dbg_info.mute_cnt++;
    }
#else
    host_instance_p->host_statistics.cur_filed_type = vicap_hal_pt_get_field(vicap_id, VICAP_HAL_PT_INDEX_PT0);
#endif

    host_instance_p->host_statistics.y_checksum = vicap_hal_chn_get_ycheck_sum(vicap_id);
    host_instance_p->host_statistics.c_checksum = vicap_hal_chn_get_ccheck_sum(vicap_id);

    vicap_hal_pt_get_frame_size(vicap_id, VICAP_HAL_PT_INDEX_PT0, &frame_size);
    host_instance_p->host_statistics.in_width[VICAP_HAL_PT_INDEX_PT0] = frame_size.width;
    host_instance_p->host_statistics.in_height[VICAP_HAL_PT_INDEX_PT0] = frame_size.height;

    vicap_hal_pt_get_frame_size(vicap_id, VICAP_HAL_PT_INDEX_PT1, &frame_size);
    host_instance_p->host_statistics.in_width[VICAP_HAL_PT_INDEX_PT1] = frame_size.width;
    host_instance_p->host_statistics.in_height[VICAP_HAL_PT_INDEX_PT1] = frame_size.height;

    host_instance_p->host_statistics.int_write_line_num = vicap_hal_chn_get_line_num(vicap_id);
}

VI_STATIC hi_void hal_vi_int_statistic(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    hi_u32 chn0_int_status;

    host_instance_p->host_status.is_invalid = HI_FALSE;
    host_instance_p->host_status.is_video_err = HI_FALSE;

    /* get int status */
    chn0_int_status = vicap_hal_chn_get_int_status(vicap_id);
    if (!(chn0_int_status & VICAP_HAL_CHN_INT_FSTARTDELAY) && !(chn0_int_status & VICAP_HAL_CHN_INT_MASK_ERR)) {
        vi_drv_log_err("no done! chn0_int_status=0x%x\n", chn0_int_status);
        host_instance_p->host_status.is_invalid = HI_TRUE;
        return;
    }

    host_instance_p->chn_int = chn0_int_status;
    vicap_hal_chn_set_clear_int(vicap_id, chn0_int_status);

#if defined(HI_SMMU_SUPPORT)
    hal_vi_update_smmu_int_status(host_instance_p);
#endif

    hal_vi_update_int_err(host_instance_p, chn0_int_status);

    hal_vi_update_pt_state(host_instance_p);
}

VI_STATIC hi_s32 hal_vi_isr(hi_s32 irq, hi_void *arg)
{
    vi_host_instance *host_instance_p = (vi_host_instance *)arg;

    if (host_instance_p == HI_NULL) {
        goto out;
    }

    hal_vi_int_statistic(host_instance_p);

    hal_vi_int_process(host_instance_p);

    if (host_instance_p->host_status.is_invalid) {
        goto out;
    }

    if (host_instance_p->call_back != HI_NULL) {
        host_instance_p->call_back(host_instance_p->vi_instance_p);
    }

out:
    return OSAL_IRQ_HANDLED;
}

VI_STATIC hi_s32 hal_vi_set_chn_crop(vi_host_instance *host_instance_p)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    hi_u32 vicap_id = host_instance_p->vicap_id;
    hi_u32 src_width = vi_instance_p->attr_in.width;
    hi_u32 src_height = vi_instance_p->attr_in.height;
    hi_drv_rect crop_rect = {0};
    vicap_hal_chn_crop_cfg chn_crop_cfg = {0};

    memcpy_s(&crop_rect, sizeof(hi_drv_rect), &vi_instance_p->attr_ctrls.crop_rect, sizeof(hi_drv_rect));

    /* 根据过采样模式，处理相应的源宽度与裁剪宽度 */
    if (HI_DRV_PIXEL_FMT_NV61_2X1 != vi_instance_p->attr_in.pix_fmt) {
        switch (vi_instance_p->attr_in.over_sample) {
            case HI_VI_OVERSAMPLE_4X:
                crop_rect.rect_x *= 4; /* need * 4 */
                crop_rect.rect_w *= 4; /* need * 4 */
                src_width *= 4;        /* need * 4 */
                break;

            case HI_VI_OVERSAMPLE_2X:
                crop_rect.rect_x *= 2; /* need * 2 */
                crop_rect.rect_w *= 2; /* need * 2 */
                src_width *= 2;        /* need * 2 */
                break;

            default:
                break;
        }
    }

    /* 隔行信号 */
    if (vi_instance_p->attr_in.interlace) {
        /* 如果输入为场, 则硬件限制针对一场, 故crop的h/2 */
        crop_rect.rect_y /= P_I_MULTIPLE;
        crop_rect.rect_h /= P_I_MULTIPLE;
        src_height /= P_I_MULTIPLE;
    }

    /* 2D裁剪 */
    if (vi_instance_p->attr_in.video_3d_type == HI_DRV_3D_NONE) {
        chn_crop_cfg.crop_rect[VICAP_DRV_CHN_CROP_CROP0].rect_x = crop_rect.rect_x;
        chn_crop_cfg.crop_rect[VICAP_DRV_CHN_CROP_CROP0].rect_y = crop_rect.rect_y;
        chn_crop_cfg.crop_rect[VICAP_DRV_CHN_CROP_CROP0].rect_w = crop_rect.rect_w;
        chn_crop_cfg.crop_rect[VICAP_DRV_CHN_CROP_CROP0].rect_h = crop_rect.rect_h;

        chn_crop_cfg.crop_en[VICAP_DRV_CHN_CROP_CROP0] = HI_TRUE;
        chn_crop_cfg.crop_en[VICAP_DRV_CHN_CROP_CROP1] = HI_FALSE;

        /* 设置通道裁剪区域 */
        vicap_hal_chn_set_crop(vicap_id, &chn_crop_cfg);
    } else { /* 3D裁剪 */
#if defined(VI_3D_SUPPORT)
        hal_vi_set_chn_crop_3d(vicap_id, vi_instance_p);
#endif
    }

    return HI_SUCCESS;
}

VI_STATIC hi_void hal_vi_set_chn_scaleattr_param(hi_u32 vi_id, hi_u32 yh_step, hi_u32 ch_step, hi_u32 yv_step,
                                                 hi_u32 cv_step)
{
    vicap_hal_chn_hscale_cfg chn_hscale_cfg = {0};
    vicap_hal_chn_vscale_cfg chn_vscale_cfg = {0};

    /* 配置水平y缩放相关 */
    if (yh_step != VICAP_HAL_CHN_HFIR_RATIO_NO) {
        vicap_hal_chn_set_coef_yupdate(vi_id, HI_TRUE);
        chn_hscale_cfg.ratio = yh_step;
        chn_hscale_cfg.chn_hscale_mode = VICAP_HAL_CHN_HSCALE_MODE_FILT;
        chn_hscale_cfg.mid_en = HI_TRUE;
        chn_hscale_cfg.scale_en = HI_TRUE;
        vicap_hal_chn_set_hscale_ycfg(vi_id, &chn_hscale_cfg);
    } else {
        chn_hscale_cfg.scale_en = HI_FALSE;
        vicap_hal_chn_set_hscale_ycfg(vi_id, &chn_hscale_cfg);
    }

    /* 配置垂直y缩放相关 */
    if (yv_step != VICAP_HAL_CHN_VFIR_RATIO_NO) {
        chn_vscale_cfg.ratio = yv_step;
        chn_vscale_cfg.scale_en = HI_TRUE;
        vicap_hal_chn_set_vscale_ycfg(vi_id, &chn_vscale_cfg);
    } else {
        chn_vscale_cfg.scale_en = HI_FALSE;
        vicap_hal_chn_set_vscale_ycfg(vi_id, &chn_vscale_cfg);
    }

    /* 配置水平c缩放相关 */
    if (ch_step != VICAP_HAL_CHN_HFIR_RATIO_NO) {
        vicap_hal_chn_set_coef_cupdate(vi_id, HI_TRUE);
        chn_hscale_cfg.ratio = ch_step;
        chn_hscale_cfg.chn_hscale_mode = VICAP_HAL_CHN_HSCALE_MODE_FILT;
        chn_hscale_cfg.mid_en = HI_TRUE;
        chn_hscale_cfg.scale_en = HI_TRUE;
        vicap_hal_chn_set_hscale_ccfg(vi_id, &chn_hscale_cfg);
    } else {
        chn_hscale_cfg.scale_en = HI_FALSE;
        vicap_hal_chn_set_hscale_ccfg(vi_id, &chn_hscale_cfg);
    }

    /* 配置垂直c缩放相关 */
    if (cv_step != VICAP_HAL_CHN_VFIR_RATIO_NO) {
        chn_vscale_cfg.ratio = cv_step;
        chn_vscale_cfg.scale_en = HI_TRUE;
        vicap_hal_chn_set_vscale_ccfg(vi_id, &chn_vscale_cfg);
    } else {
        chn_vscale_cfg.scale_en = HI_FALSE;
        vicap_hal_chn_set_vscale_ccfg(vi_id, &chn_vscale_cfg);
    }
}

vicap_hal_chn_scale_coef hal_vi_select_coef(hi_u32 step_handle)
{
    vicap_hal_chn_scale_coef coef = VICAP_HAL_CHN_SCALE_COEF_MAX;

    if (step_handle == VICAP_HAL_CHN_HFIR_RATIO_NO) { /* 无缩小 */
        coef = VICAP_HAL_CHN_SCALE_COEF_NO;
    } else if ((step_handle > VICAP_HAL_CHN_HFIR_RATIO_NO) &&
               (step_handle <= VICAP_HAL_CHN_HFIR_RATIO_2X)) { /* 2倍缩小 */
        coef = VICAP_HAL_CHN_SCALE_COEF_2X;
    } else if ((step_handle > VICAP_HAL_CHN_HFIR_RATIO_2X) &&
               (step_handle <= VICAP_HAL_CHN_HFIR_RATIO_3X)) { /* 3倍缩小 */
        coef = VICAP_HAL_CHN_SCALE_COEF_3X;
    } else if ((step_handle > VICAP_HAL_CHN_HFIR_RATIO_3X) &&
               (step_handle <= VICAP_HAL_CHN_HFIR_RATIO_4X)) { /* 4倍缩小 */
        coef = VICAP_HAL_CHN_SCALE_COEF_4X;
    } else { /* 其它倍缩小 */
        coef = VICAP_HAL_CHN_SCALE_COEF_OTHER;
    }

    return coef;
}

VI_STATIC hi_void hal_vi_set_chn_scale_attr(hi_u32 vi_id, vicap_hal_chn_update_scale_type scaler_type,
                                            vicap_hal_chn_scaler_attr *scale_attr_p)
{
    hi_u32 wi = scale_attr_p->in_img_width;
    hi_u32 wo = scale_attr_p->out_img_width;
    hi_u32 hi = scale_attr_p->in_img_height;
    hi_u32 ho = scale_attr_p->out_img_height;
    hi_u32 yh_step = 0;
    hi_u32 ch_step = 0;
    hi_u32 yv_step;
    hi_u32 cv_step;

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~step1: 计算缩放配置~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    /* 444->422下采样，色度水平方向缩小是亮度的两倍, */
    if (((HI_DRV_PIXEL_FMT_NV42 == scale_attr_p->pixel_format) ||
         (HI_DRV_PIXEL_FMT_NV42_RGB == scale_attr_p->pixel_format)) &&
        (HI_DRV_PIXEL_FMT_NV61_2X1 == scale_attr_p->video_fmt)) {
        /* 4096左移20位，32位溢出 */
        yh_step = (((wi << 19) / wo) * 2); /* 2^20=1048576, need << 19bit, and need * 2 */
        ch_step = yh_step << 1;            /* 色度为亮度的2倍 */
    } else {
        yh_step = (((wi << 19) / wo) * 2); /* 2^20=1048576, need << 19bit, and need * 2 */
        ch_step = yh_step;
    }

    /* 垂直缩放倍数, 向上取整 */
    yv_step = (hi + (ho - 1)) / ho;
    cv_step = yv_step;

    /* ~~~~~~~~~~~~~~~~~~~~~~~~step2:配置缩放参数到硬件~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    /* 设置水平缩放系数 */
    if (scaler_type == VICAP_HAL_CHN_UPDATE_SCALE_TYPE_COEF) {
#if VI_TMP
        vicap_hal_chn_scale_coef y_coef = hal_vi_select_coef(yh_step);
        vicap_hal_chn_scale_coef c_coef = hal_vi_select_coef(ch_step);
#endif
        if (yh_step != VICAP_HAL_CHN_HFIR_RATIO_NO) {
#if VI_TMP
            vicap_hal_chn_set_h_scale_coef8_phase(vi_id, VICAP_HAL_CHN_SCALE_OBJ_YH, y_coef);
#endif
            vicap_hal_chn_set_coef_yresl(vi_id, VICAP_HAL_CHN_SCALE_COEF_TYPE_USING);
        }

        if (ch_step != VICAP_HAL_CHN_HFIR_RATIO_NO) {
#if VI_TMP
            vicap_hal_chn_set_h_scale_coef8_phase(vi_id, VICAP_HAL_CHN_SCALE_OBJ_CH, c_coef);
#endif
            vicap_hal_chn_set_coef_cresl(vi_id, VICAP_HAL_CHN_SCALE_COEF_TYPE_USING);
        }
    }

    /* 设置除水平缩放系数外的其它相关缩放配置 */
    if (scaler_type == VICAP_HAL_CHN_UPDATE_SCALE_TYPE_PARAM) {
        hal_vi_set_chn_scaleattr_param(vi_id, yh_step, ch_step, yv_step, cv_step);
    }
}

VI_STATIC hi_void hal_vi_set_chn_scaler_reg(hi_u32 vi_id, vi_instance *vi_instance_p, hi_u32 out_width,
                                            hi_u32 out_height, hi_drv_rect crop_rect)
{
    vicap_hal_chn_scaler_attr scale_attr = {0};
    vicap_hal_chn_scale_in_size scale_in_size = {0};
    vicap_hal_chn_scale_out_size scale_out_size = {0};

    /* 缩放属性参数 */
    scale_in_size.y_in_pixel = crop_rect.rect_w;
    scale_in_size.y_in_line = crop_rect.rect_h;
    scale_in_size.c_in_pixel = crop_rect.rect_w;
    scale_in_size.c_in_line = crop_rect.rect_h;
    scale_out_size.y_out_pixel = out_width;
    scale_out_size.y_out_line = out_height;
    scale_out_size.c_out_pixel = out_width;
    scale_out_size.c_out_line = out_height;

    scale_attr.in_img_width = crop_rect.rect_w;               /* 输入图像宽度 */
    scale_attr.in_img_height = crop_rect.rect_h;              /* 输入图像高度 */
    scale_attr.out_img_width = out_width;                     /* 输出图像宽度 */
    scale_attr.out_img_height = out_height;                   /* 输出图像高度 */
    scale_attr.pixel_format = vi_instance_p->attr_in.pix_fmt; /* 输入像素格式 */
    scale_attr.video_fmt = vi_instance_p->attr_out.pix_fmt;   /* 输出存储格式 */

    /* YUV420, 色度的w and h各减为原来的一半 */
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        scale_in_size.c_in_pixel = scale_in_size.c_in_pixel / 2; /* width / 2 */
        scale_in_size.c_in_line = scale_in_size.c_in_line / 2;   /* height / 2 */
    }
    if (vi_instance_p->attr_in.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        scale_in_size.c_in_pixel = scale_in_size.c_in_pixel / 2; /* width / 2 */
    }

    /* YUV422, 色度的w减为原来的一半 */
    if (vi_instance_p->attr_out.pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        scale_out_size.c_out_pixel = scale_out_size.c_out_pixel / 2; /* width / 2 */
        scale_out_size.c_out_line = scale_out_size.c_out_line / 2;   /* height / 2 */
    }
    if (vi_instance_p->attr_out.pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        scale_out_size.c_out_pixel = scale_out_size.c_out_pixel / 2; /* width / 2 */
    }

    /* 设置缩放相关配置 */
    hal_vi_set_chn_scale_attr(vi_id, VICAP_HAL_CHN_UPDATE_SCALE_TYPE_COEF, &scale_attr);
    hal_vi_set_chn_scale_attr(vi_id, VICAP_HAL_CHN_UPDATE_SCALE_TYPE_PARAM, &scale_attr);

    vicap_hal_chn_set_scale_hfir_in_size(vi_id, &scale_in_size);
    vicap_hal_chn_set_scale_hfir_out_size(vi_id, &scale_out_size);
}

VI_STATIC hi_s32 hal_vi_set_chn_scaler(vi_host_instance *host_instance_p)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_out_attr *vi_out_attr_p = &vi_instance_p->attr_out;
    hi_u32 out_width = vi_instance_p->attr_out.width;
    hi_u32 out_height = vi_instance_p->attr_out.height;
    hi_drv_rect crop_rect = vi_instance_p->attr_ctrls.crop_rect;

    if (vi_instance_p->attr_out.video_3d_type == HI_DRV_3D_NONE) {
        if (vi_instance_p->attr_out.interlace) {
            crop_rect.rect_h = crop_rect.rect_h / P_I_MULTIPLE;
            out_height = vi_out_attr_p->height / P_I_MULTIPLE;
        } else {
            out_height = vi_out_attr_p->height;
        }
    }

#if defined(VI_3D_SUPPORT)
    if (vi_instance_p->vi_out_attr.video_3d_type == HI_DRV_VIDEO_3D_TYPE_SBS) {
        crop_rect.width = crop_rect.width * 2; /* sbs crop width need * 2 */
        out_width = vi_out_attr_p->width * 2;  /* sbs out width need * 2 */
        if (vi_instance_p->vi_out_attr.interlace) {
            crop_rect.height = crop_rect.height / P_I_MULTIPLE;
            out_height = vi_out_attr_p->height / P_I_MULTIPLE;
        } else {
            out_height = vi_out_attr_p->height;
        }
    } else if (vi_instance_p->vi_out_attr.video_3d_type == HI_DRV_VIDEO_3D_TYPE_TAB) {
        if (vi_instance_p->vi_out_attr.interlace) {
            out_height = vi_out_attr_p->height;
        } else {
            crop_rect.height = crop_rect.height * 2; /* tab crop height need * 2 */
            out_height = vi_out_attr_p->height * 2;  /* tab out height need * 2 */
        }
    } else if (vi_instance_p->vi_out_attr.video_3d_type == HI_DRV_VIDEO_3D_TYPE_FPK) {
        crop_rect.height = crop_rect.height * 2; /* fpk out height need * 2 */
        out_height = vi_out_attr_p->height * 2;  /* fpk out height need * 2 */
    } else {
        if (vi_instance_p->vi_out_attr.interlace) {
            crop_rect.height = crop_rect.height / P_I_MULTIPLE;
            out_height = vi_out_attr_p->height / P_I_MULTIPLE;
        } else {
            out_height = vi_out_attr_p->height;
        }
    }
#endif

    hal_vi_set_chn_scaler_reg(vicap_id, vi_instance_p, out_width, out_height, crop_rect);

    return HI_SUCCESS;
}

VI_STATIC hi_s32 hal_vi_set_chn_csc(vi_host_instance *host_instance_p)
{
    hi_s32 ret;
    hi_u32 vicap_id = host_instance_p->vicap_id;

    memset_s(&host_instance_p->csc_info, sizeof(hi_drv_pq_csc_info), 0, sizeof(hi_drv_pq_csc_info));
    memset_s(&host_instance_p->pq_csc_coef, sizeof(hi_drv_pq_csc_coef), 0, sizeof(hi_drv_pq_csc_coef));

    ret = vi_csc_get_csc_info(host_instance_p->vi_instance_p, &host_instance_p->csc_info);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = vi_csc_get_csc_para(host_instance_p->csc_info, &host_instance_p->pq_csc_coef);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* 配置CSC寄存器 */
    vicap_hal_chn_set_csc_cfg(vicap_id, host_instance_p->csc_info.csc_en);

    if (host_instance_p->csc_info.csc_en) {
        vicap_hal_chn_set_csc_param(vicap_id, (vicap_hal_chn_csc_coef *)&host_instance_p->pq_csc_coef.csc_ac_coef,
                                    (vicap_hal_chn_csc_dccoef *)&host_instance_p->pq_csc_coef.csc_dc_coef);
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 hal_vi_set_chn_w_cfg(vi_host_instance *host_instance_p, vi_buf_node *node_set_p)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vicap_hal_chn_wch_faddr wch_addr[VICAP_DRV_FRMTYPE_EYE_MAX] = {0};
    vicap_hal_chn_wch_cfg wch_cfg = {0};
    hi_drv_video_frame *v_frame_p = HI_NULL;
    hi_u32 base_addr;

    v_frame_p = &node_set_p->frame_info;
    base_addr = node_set_p->frame_addr.video_buf_addr.start_phy_addr;

    wch_addr[VICAP_DRV_FRMTYPE_EYE_LEFT].y_addr = base_addr + v_frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].y_offset;
    wch_addr[VICAP_DRV_FRMTYPE_EYE_LEFT].c_addr = base_addr + v_frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].c_offset;

    wch_cfg.interleave = HI_FALSE;
    wch_cfg.dst_bit_width = vi_instance_p->attr_out.bit_width;
    wch_cfg.field = (vicap_hal_frame_filed)v_frame_p->field_mode;

    if (vi_instance_p->attr_in.video_3d_type == HI_DRV_3D_FPK) {
#if defined(VI_3D_SUPPORT)
        if (vi_instance_p->vi_in_attr.interlace) {
            wch_cfg.fpk_interlace_mode = HI_TRUE;
        } else {
            wch_cfg.fpk_interlace_mode = HI_FALSE;
        }
#endif
    } else {
        wch_cfg.fpk_interlace_mode = HI_FALSE;
    }

#if defined(HI_SMMU_SUPPORT)
    wch_cfg.mmu_bypass = HI_FALSE;
#else
    wch_cfg.mmu_bypass = HI_TRUE;
#endif
    wch_cfg.enable = HI_TRUE;

    vicap_hal_chn_set_wch_ycfg(vicap_id, &wch_cfg);
    vicap_hal_chn_set_wch_ccfg(vicap_id, &wch_cfg);
    vicap_hal_chn_set_wch_faddr(vicap_id, &wch_addr[VICAP_DRV_FRMTYPE_EYE_LEFT]);

    return HI_SUCCESS;
}

VI_STATIC hi_s32 hal_vi_set_chn_wch_stride(vi_host_instance *host_instance_p)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    hi_u32 vicap_id = host_instance_p->vicap_id;
    hi_u32 y_stride = vi_instance_p->attr_out.y_stride;
    hi_u32 c_stride = vi_instance_p->attr_out.c_stride;

    vicap_hal_chn_set_wch_ystride(vicap_id, y_stride);
    vicap_hal_chn_set_wch_cstride(vicap_id, c_stride);

    return HI_SUCCESS;
}

VI_STATIC hi_void hal_vi_calc_chn_w_y_size(vi_host_instance *host_instance_p, vicap_hal_chn_dstsize *y_w_size_p)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    vi_out_attr *vi_out_attr_p = &vi_instance_p->attr_out;
    hi_u32 w_width = 0;
    hi_u32 w_height = 0;

    if (vi_out_attr_p->video_3d_type == HI_DRV_3D_NONE) {
        w_width = vi_out_attr_p->width;
        if (vi_out_attr_p->interlace) {
            w_height = vi_out_attr_p->height / P_I_MULTIPLE;
        } else {
            w_height = vi_out_attr_p->height;
        }
    }
#if defined(VI_3D_SUPPORT)
    if (vi_out_attr_p->video_3d_type == HI_DRV_VIDEO_3D_TYPE_SBS) {
        w_width = vi_out_attr_p->width * 2; /* sbs width need * 2 */
        if (vi_out_attr_p->interlace) {
            w_height = vi_out_attr_p->height / P_I_MULTIPLE;
        } else {
            w_height = vi_out_attr_p->height;
        }
    } else if (vi_out_attr_p->video_3d_type == HI_DRV_VIDEO_3D_TYPE_TAB) {
        w_width = vi_out_attr_p->width;
        if (vi_out_attr_p->interlace) {
            w_height = vi_out_attr_p->height;
        } else {
            w_height = vi_out_attr_p->height * 2; /* tab height need * 2 */
        }
    } else if (vi_out_attr_p->video_3d_type == HI_DRV_VIDEO_3D_TYPE_FPK) {
        w_width = vi_out_attr_p->width;
        w_height = vi_out_attr_p->height * 2; /* fpk height need * 2 */
    } else {
        w_width = vi_out_attr_p->width;
        if (vi_out_attr_p->interlace) {
            w_height = vi_out_attr_p->height / P_I_MULTIPLE;
        } else {
            w_height = vi_out_attr_p->height;
        }
    }
#endif

    y_w_size_p->width = w_width;
    y_w_size_p->height = w_height;
}

VI_STATIC hi_void hal_vi_calc_chn_w_c_size(vi_host_instance *host_instance_p, vicap_hal_chn_dstsize *y_w_size_p,
                                           vicap_hal_chn_dstsize *c_w_size_p)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    vi_out_attr *vi_out_attr_p = &vi_instance_p->attr_out;

    c_w_size_p->width = y_w_size_p->width * 2; /* yuv444 need * 2 */
    c_w_size_p->height = y_w_size_p->height;

    /* YUV422, 色度的w减为原来的一半 */
    if (vi_out_attr_p->pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        c_w_size_p->width = c_w_size_p->width / 2; /* yuv422 c width need / 2 */
    }

    /* YUV420, 色度的w and h各减为原来的一半 */
    if (vi_out_attr_p->pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        c_w_size_p->width = c_w_size_p->width / 2;   /* yuv420 c width need / 2 */
        c_w_size_p->height = c_w_size_p->height / 2; /* yuv420 c height need / 2 */
    }
}

VI_STATIC hi_s32 hal_vi_set_chn_w_size(vi_host_instance *host_instance_p)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vicap_hal_chn_dstsize y_w_size = {0};
    vicap_hal_chn_dstsize c_w_size = {0};

    hal_vi_calc_chn_w_y_size(host_instance_p, &y_w_size);
    hal_vi_calc_chn_w_c_size(host_instance_p, &y_w_size, &c_w_size);

    vicap_hal_chn_set_wch_ysize(vicap_id, &y_w_size);
    vicap_hal_chn_set_wch_csize(vicap_id, &c_w_size);

    return HI_SUCCESS;
}

VI_STATIC hi_s32 hal_vi_cfg_set_pt(vi_host_instance *host_instance_p)
{
    return HI_SUCCESS;
}

VI_STATIC hi_s32 hal_vi_cfg_set_chn(vi_host_instance *host_instance_p, vi_buf_node *node_set_p)
{
    hi_s32 ret;
    hi_u32 vicap_id = host_instance_p->vicap_id;

    ret = hal_vi_set_chn_crop(host_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = hal_vi_set_chn_scaler(host_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = hal_vi_set_chn_csc(host_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = hal_vi_set_chn_w_cfg(host_instance_p, node_set_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = hal_vi_set_chn_wch_stride(host_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = hal_vi_set_chn_w_size(host_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

#if defined(VI_HWBUFF_SUPPORT)
    ret = hal_vi_cfg_hwbuff(vi_id, node_set_p);
    VI_CHECK_FUNC_RET(ret);
#endif

    vicap_hal_chn_set_reg_newer(vicap_id);

    return HI_SUCCESS;
}

VI_STATIC hi_void hal_vi_calc_gen_timing_area(vi_host_instance *host_instance_p, vicap_hal_pt_gentiming_info *info)
{
    vi_instance *vi_instance_p = host_instance_p->vi_instance_p;
    vi_in_attr *vi_in_attr_p = &vi_instance_p->attr_in;
    hi_u32 frm_total;
    hi_u32 frm_act;
    hi_u32 blank_total;

    frm_total = host_instance_p->work_clk / vi_in_attr_p->rate;

    if (vi_in_attr_p->interlace) {
        frm_act = (vi_in_attr_p->width << ((hi_u32)vi_in_attr_p->over_sample)) * vi_in_attr_p->height / P_I_MULTIPLE;
    } else {
        frm_act = (vi_in_attr_p->width << ((hi_u32)vi_in_attr_p->over_sample)) * vi_in_attr_p->height;
    }

    vi_drv_log_notice("work_clk=%d,frm_total=%d,frm_act=%d\n", host_instance_p->work_clk, frm_total, frm_act);

    blank_total = frm_total - frm_act;

    info->h_act = vi_in_attr_p->width;
    if (vi_in_attr_p->interlace) {
        info->v_act = vi_in_attr_p->height / P_I_MULTIPLE;
    } else {
        info->v_act = vi_in_attr_p->height;
    }

    info->hfb = 0x58;
    info->hbb = 0xc0;
    info->vfb = 0x4;
    info->vbb = (frm_total / (info->hfb + info->hbb + info->h_act)) - info->vfb - info->v_act;

    info->vbfb = 0;
    info->vbact = 0;
    info->vbbb = 0;
}

VI_STATIC hi_void hal_vi_set_gen_timing(vi_host_instance *host_instance_p, hi_u32 pattern_type)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_host_gen_data_type gen_data_type = (vi_host_gen_data_type)pattern_type;
    vicap_hal_pt_gentiming_cfg pt_gen_timing_cfg = {0};
    vicap_hal_pt_gentiming_info pt_gen_timing_info = {0};

    hal_vi_calc_gen_timing_area(host_instance_p, &pt_gen_timing_info);

    if (gen_data_type != VI_HOST_GEN_DATA_TYPE_OFF) {
        pt_gen_timing_cfg.recover_hsync = HI_TRUE;
        pt_gen_timing_cfg.recover_vsync = HI_TRUE;
        pt_gen_timing_cfg.gen_mode = VICAP_HAL_PT_GEN_MODE_AUTO;
        pt_gen_timing_cfg.enable = HI_TRUE;

        if (vicap_id == HI_DRV_VICAP_PORT0) {
            vicap_hal_pt_set_gen_intf(vicap_id, VICAP_HAL_PT_INDEX_PT0, pt_gen_timing_info); /* 180-1A0 */
            vicap_hal_pt_set_gen_timing(vicap_id, VICAP_HAL_PT_INDEX_PT0, pt_gen_timing_cfg);

            vicap_hal_pt_set_gen_intf(vicap_id, VICAP_HAL_PT_INDEX_PT1, pt_gen_timing_info); /* 180-1A0 */
            vicap_hal_pt_set_gen_timing(vicap_id, VICAP_HAL_PT_INDEX_PT1, pt_gen_timing_cfg);
        } else {
            hi_u32 i;

            for (i = 0; i < HI_DRV_VICAP_PORT_MAX; i++) {
                vicap_hal_pt_set_gen_intf(vicap_id, i, pt_gen_timing_info); /* 180-1A0 */
                vicap_hal_pt_set_gen_timing(vicap_id, i, pt_gen_timing_cfg);
            }
        }
    } else {
        pt_gen_timing_cfg.recover_hsync = HI_FALSE;
        pt_gen_timing_cfg.recover_vsync = HI_FALSE;
        pt_gen_timing_cfg.gen_mode = VICAP_HAL_PT_GEN_MODE_FVHDE;
        pt_gen_timing_cfg.enable = HI_FALSE;

        if (vicap_id == HI_DRV_VICAP_PORT0) {
            vicap_hal_pt_set_gen_timing(vicap_id, VICAP_HAL_PT_INDEX_PT0, pt_gen_timing_cfg);
            vicap_hal_pt_set_gen_timing(vicap_id, VICAP_HAL_PT_INDEX_PT1, pt_gen_timing_cfg);
        } else {
            hi_u32 i;

            for (i = 0; i < HI_DRV_VICAP_PORT_MAX; i++) {
                vicap_hal_pt_set_gen_timing(vicap_id, i, pt_gen_timing_cfg);
            }
        }
    }
}

VI_STATIC hi_void hal_vi_set_gen_date(vi_host_instance *host_instance_p, hi_u32 pattern_type)
{
    hi_u32 vicap_id = host_instance_p->vicap_id;
    vi_host_gen_data_type gen_data_type = (vi_host_gen_data_type)pattern_type;

    if (gen_data_type >= VI_HOST_GEN_DATA_TYPE_MAX) {
        gen_data_type = VI_HOST_GEN_DATA_TYPE_OFF;
    }

    if (vicap_id == HI_DRV_VICAP_PORT0) {
        vicap_hal_pt_set_gen_data_init(vicap_id, VICAP_HAL_PT_INDEX_PT0, g_gen_data_init[gen_data_type]);
        vicap_hal_pt_set_gen_data_coef(vicap_id, VICAP_HAL_PT_INDEX_PT0, g_gen_data_coef[gen_data_type]);
        vicap_hal_pt_set_gen_data_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT0, g_gen_data_cfg[gen_data_type]);

        vicap_hal_pt_set_gen_data_init(vicap_id, VICAP_HAL_PT_INDEX_PT1, g_gen_data_init[gen_data_type]);
        vicap_hal_pt_set_gen_data_coef(vicap_id, VICAP_HAL_PT_INDEX_PT1, g_gen_data_coef[gen_data_type]);
        vicap_hal_pt_set_gen_data_cfg(vicap_id, VICAP_HAL_PT_INDEX_PT1, g_gen_data_cfg[gen_data_type]);
    } else {
        hi_u32 i;

        for (i = 0; i < HI_DRV_VICAP_PORT_MAX; i++) {
            vicap_hal_pt_set_gen_data_init(vicap_id, i, g_gen_data_init[gen_data_type]);
            vicap_hal_pt_set_gen_data_coef(vicap_id, i, g_gen_data_coef[gen_data_type]);
            vicap_hal_pt_set_gen_data_cfg(vicap_id, i, g_gen_data_cfg[gen_data_type]);
        }
    }
}

hi_s32 vi_hal_creat_host(hi_vi_attr *attr, hi_void **host_handle)
{
    vi_host_instance *host_instance_p = HI_NULL;

    /* 根据attr返回相应的host实例 */
    if (attr->access_type == HI_VI_ACCESS_HDMIRX_PORT1) {
        host_instance_p = &g_vi_host_ctx.instance[1];
    } else {
        host_instance_p = &g_vi_host_ctx.instance[0];
    }

    if (host_instance_p->call_back != HI_NULL) {
        vi_drv_log_err("vicap(%d) used!!!\n", host_instance_p->vicap_id);
        return HI_FAILURE;
    }

    *host_handle = host_instance_p;

    vi_drv_log_info("wh(%d,%d) vicap(%d)\n", attr->config.width, attr->config.height, host_instance_p->vicap_id);

    return HI_SUCCESS;
}

hi_s32 vi_hal_get_vicap_id(hi_void *host_handle)
{
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    return host_instance_p->vicap_id;
}

hi_s32 vi_hal_get_cur_filed_type(hi_void *host_handle)
{
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    return host_instance_p->host_statistics.cur_filed_type;
}

hi_s32 vi_hal_set_callback(hi_void *host_handle, vi_fun fun, vi_instance *vi_instance_p)
{
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    host_instance_p = (vi_host_instance *)host_handle;
    host_instance_p->call_back = fun;
    host_instance_p->vi_instance_p = vi_instance_p;

    return HI_SUCCESS;
}

hi_s32 vi_hal_set_gen_date(hi_void *host_handle, hi_u32 pattern_type)
{
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    hal_vi_set_gen_date(host_instance_p, pattern_type);

    return HI_SUCCESS;
}

hi_s32 vi_hal_set_gen_timing_date(hi_void *host_handle, hi_u32 pattern_type)
{
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    hal_vi_set_gen_date(host_instance_p, pattern_type);
    hal_vi_set_gen_timing(host_instance_p, pattern_type);

    return HI_SUCCESS;
}

hi_s32 vi_hal_prepare(hi_void *host_handle)
{
    hi_u32 vicap_id;
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    vi_drv_func_enter();

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    vicap_id = host_instance_p->vicap_id;

    /* vicap逻辑时钟，复位，工作模式等其他全局设置 */
    vicap_global_cfg(host_instance_p);

    /* 设置Port */
    vicap_global_port_cfg(host_instance_p);

    /* 设置Chn */
    vicap_global_chn_cfg(host_instance_p);

    /* 打开Port */
    vicap_port_enable(vicap_id);

    /* 开启SMMU功能 */
#if defined(HI_SMMU_SUPPORT)
    vicap_smmu_start(vicap_id);
#endif

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_hal_unprepare(hi_void *host_handle)
{
    hi_u32 vicap_id;
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    vi_drv_func_enter();

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    vicap_id = host_instance_p->vicap_id;

    /* 停止通路的端口 */
    vicap_port_disable(vicap_id);

    /* 关闭SMMU功能 */
#if defined(HI_SMMU_SUPPORT)
    vicap_smmu_stop(vicap_id);
#endif

    /* 开复位 */
    vicap_drv_comm_set_hal_reset_en(HI_TRUE);

    /* 关时钟 */
    vicap_drv_comm_set_hal_clk_gate_en(HI_FALSE);

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_hal_start(hi_void *host_handle)
{
    hi_u32 vicap_id;
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    vi_drv_func_enter();

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    vicap_id = host_instance_p->vicap_id;

    vicap_chn_enable_mask(vicap_id, VICAP_HAL_CHN_INT_FSTARTDELAY);

    vicap_chn_enable(vicap_id);

    vicap_hal_chn_set_fstart_dly(vicap_id, 0x400);

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_hal_stop(hi_void *host_handle)
{
    hi_u32 vicap_id;
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    vi_drv_func_enter();

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    vicap_id = host_instance_p->vicap_id;

    vicap_chn_disable_mask(vicap_id);

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_hal_cfg_node(hi_void *host_handle, vi_buf_node *node_set_p)
{
    hi_s32 ret;
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;

    if (host_instance_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    ret = hal_vi_cfg_set_pt(host_instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = hal_vi_cfg_set_chn(host_instance_p, node_set_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 vi_hal_suspend(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 vi_hal_resume(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 vi_hal_deinit(hi_void)
{
    hi_u32 i;
    vi_host_instance *host_instance_p = HI_NULL;

    vi_drv_func_enter();

    for (i = 0; i < VI_MAX_IP_NUM; i++) {
        host_instance_p = &g_vi_host_ctx.instance[i];
        vi_drv_log_info("free irq(%d)\n", host_instance_p->irq_num);
        osal_irq_free(host_instance_p->irq_num, host_instance_p);
    }

    /* close all clk */
    vicap_drv_comm_set_hal_reset_en(HI_FALSE);
    vicap_drv_comm_set_hal_clk_gate_en(HI_FALSE);
    osal_msleep(VICAP_DRV_WAIT_SMMU_DELAY);
    vicap_drv_comm_set_hal_smmu_reset(HI_TRUE);
    osal_msleep(VICAP_DRV_WAIT_SMMU_DELAY);
    vicap_drv_comm_set_hal_smmu_clk_en(HI_FALSE);

    vicap_hal_de_init();

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_hal_init(hi_void)
{
    hi_s32 ret;
    hi_u32 i;
    vi_host_instance *host_instance_p = HI_NULL;

    vi_drv_func_enter();

    hal_vi_init_ctx();

    ret = vicap_hal_init();
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("check attr err:0x%x\n", ret);
        return HI_ERR_VI_INVALID_PARA;
    }

    /* open bus clk gate, and reset all */
    vicap_drv_comm_set_hal_smmu_reset(HI_TRUE);
    vicap_drv_comm_set_hal_clk_gate_en(HI_TRUE);
    vicap_drv_comm_set_hal_smmu_clk_en(HI_TRUE);
    osal_msleep(VICAP_DRV_WAIT_SMMU_DELAY);
    vicap_drv_comm_set_hal_reset_en(HI_TRUE);
    vicap_drv_comm_set_hal_smmu_reset(HI_TRUE);
    osal_msleep(VICAP_DRV_WAIT_SMMU_SLEEP);
    vicap_drv_comm_set_hal_smmu_reset(HI_FALSE);

    for (i = 0; i < VI_MAX_IP_NUM; i++) {
        host_instance_p = &g_vi_host_ctx.instance[i];
        if (osal_irq_request(host_instance_p->irq_num, hal_vi_isr, 0, host_instance_p->irq_name, host_instance_p)) {
            vi_drv_log_err("request irq(%d) fail\n", host_instance_p->irq_num);
            vi_hal_deinit();
            return HI_FAILURE;
        }
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_void vi_hal_proc_print(hi_void *host_handle, hi_void *s)
{
    vi_host_instance *host_instance_p = (vi_host_instance *)host_handle;
    vi_host_statistics *host_statistics_p = &host_instance_p->host_statistics;
    vi_host_dbg_info *vi_host_dbg_info_p = &host_instance_p->host_dbg_info;
    hi_drv_pq_csc_ac_coef *csc_ac_coef_p = &host_instance_p->pq_csc_coef.csc_ac_coef;
    hi_drv_pq_csc_dc_coef *csc_dc_coef_p = &host_instance_p->pq_csc_coef.csc_dc_coef;
    hi_drv_color_descript *csc_in_p = &host_instance_p->csc_info.csc_in;
    hi_drv_color_descript *csc_out_p = &host_instance_p->csc_info.csc_out;

    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    osal_proc_print(s, "%-40s:%10s,%10d,%10d\n", "CscInfo(En/Type)",
                    (host_instance_p->csc_info.csc_en ? "True" : "False"), host_instance_p->csc_info.csc_type,
                    host_instance_p->csc_info.csc_pecision);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d,%10d\n", "CscIn(color/range/prim/trans/matrix)",
                    csc_in_p->color_space, csc_in_p->quantify_range, csc_in_p->color_primary, csc_in_p->transfer_type,
                    csc_in_p->matrix_coef);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d,%10d\n", "CscOut(color/range/prim/trans/matrix)",
                    csc_out_p->color_space, csc_out_p->quantify_range, csc_out_p->color_primary,
                    csc_out_p->transfer_type, csc_out_p->matrix_coef);

    osal_proc_print(s, "%-40s:%X,%X,%X,%X,%X,%X,%X,%X,%X\n", "csc_ac_coef", csc_ac_coef_p->csc_coef00,
                    csc_ac_coef_p->csc_coef01, csc_ac_coef_p->csc_coef02, csc_ac_coef_p->csc_coef10,
                    csc_ac_coef_p->csc_coef11, csc_ac_coef_p->csc_coef12, csc_ac_coef_p->csc_coef20,
                    csc_ac_coef_p->csc_coef21, csc_ac_coef_p->csc_coef22);

    osal_proc_print(s, "%-40s:%X,%X,%X,%X,%X,%X\n", "csc_dc_coef", csc_dc_coef_p->csc_in_dc0, csc_dc_coef_p->csc_in_dc1,
                    csc_dc_coef_p->csc_in_dc2, csc_dc_coef_p->csc_out_dc0, csc_dc_coef_p->csc_out_dc1,
                    csc_dc_coef_p->csc_out_dc2);

    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    osal_proc_print(s, "%-40s:%10X,%10X\n", "Node(chn_int/mmu_int)", host_instance_p->chn_int,
                    host_instance_p->mmu_int);

    osal_proc_print(
        s, "%-40s:%10d,%10d,(%4d*%-4d)(%4d*%-4d)\n", "Int(Filed/LineNum/PT0(W*H)/PT1(W*H))",
        host_statistics_p->cur_filed_type, host_statistics_p->int_write_line_num,
        host_statistics_p->in_width[VICAP_HAL_PT_INDEX_PT0], host_statistics_p->in_height[VICAP_HAL_PT_INDEX_PT0],
        host_statistics_p->in_width[VICAP_HAL_PT_INDEX_PT1], host_statistics_p->in_height[VICAP_HAL_PT_INDEX_PT1]);

    osal_proc_print(s, "--------------------------------------VI HAL DBG INFO--------------------------------------\n");
    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d\n", "IntCnt(total/f/fdelay/mute)", vi_host_dbg_info_p->int_cnt_total,
                    vi_host_dbg_info_p->int_f_start_cnt, vi_host_dbg_info_p->int_f_delay_cnt,
                    vi_host_dbg_info_p->mute_cnt);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d\n", "IntCnt(cc/nocc/cfg/nocfg)", vi_host_dbg_info_p->cc_int_cnt,
                    vi_host_dbg_info_p->no_cc_int_cnt, vi_host_dbg_info_p->update_cfg_cnt,
                    vi_host_dbg_info_p->no_update_cfg_cnt);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d\n", "IntErr(lost/overflow/ll/smmu)",
                    vi_host_dbg_info_p->filed_lost_cnt, vi_host_dbg_info_p->buf_overflow_cnt,
                    vi_host_dbg_info_p->ll_err_cnt, vi_host_dbg_info_p->int_smmu_err_cnt);
}
