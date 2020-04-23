/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hal_vdp_chip_config.h"
#include "vdp_chip_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static disp_capacity g_disp_capability[HI_DRV_DISPLAY_BUTT];
static vdp_capacity g_vdp_capability;

static hi_drv_pixel_format g_layer0_format[] = {
    HI_DRV_PIXEL_FMT_NV16_2X1,
    HI_DRV_PIXEL_FMT_NV61_2X1,
    HI_DRV_PIXEL_FMT_NV21,
    HI_DRV_PIXEL_FMT_NV12,
    HI_DRV_PIXEL_FMT_NV42,
    HI_DRV_PIXEL_FMT_RGB24,
    HI_DRV_PIXEL_FMT_ARGB8888,

};

static hi_drv_pixel_format g_layer1_format[] = {
    HI_DRV_PIXEL_FMT_NV16_2X1,
    HI_DRV_PIXEL_FMT_NV61_2X1,
    HI_DRV_PIXEL_FMT_NV21,
    HI_DRV_PIXEL_FMT_NV12,

};

static hi_drv_pixel_format g_layer3_format[] = {
    HI_DRV_PIXEL_FMT_NV16_2X1,
    HI_DRV_PIXEL_FMT_NV61_2X1,
    HI_DRV_PIXEL_FMT_NV21,
    HI_DRV_PIXEL_FMT_NV12,

};

// :TODO: add mipi
static hi_drv_disp_intf_id g_disp_intf[] = {
    HI_DRV_DISP_INTF_HDMI0,
    HI_DRV_DISP_INTF_HDMI1,
    HI_DRV_DISP_INTF_CVBS0,
};

static hi_drv_disp_out_type g_disp_type[] = {
    HI_DRV_DISP_TYPE_NORMAL,
    HI_DRV_DISP_TYPE_SDR_CERT,
    HI_DRV_DISP_TYPE_DOLBY,
    HI_DRV_DISP_TYPE_HDR10,
    HI_DRV_DISP_TYPE_HDR10_CERT,
    HI_DRV_DISP_TYPE_HLG,
    HI_DRV_DISP_TYPE_DOLBY_LL,
};

// :TODO: need update for 8k
static fmt_section g_disp0_fmt_section[] = {
    { HI_DRV_DISP_FMT_1080P_60, HI_DRV_DISP_FMT_CUSTOM },
};

static fmt_section g_disp1_fmt_section[] = {
    { HI_DRV_DISP_FMT_1080P_60, HI_DRV_DISP_FMT_CUSTOM },
};

static hi_drv_disp_output_color_space g_disp_csc[] = {
    HI_DRV_DISP_COLOR_SPACE_BT709,
    HI_DRV_DISP_COLOR_SPACE_BT601,
    HI_DRV_DISP_COLOR_SPACE_BT2020,
};

static layer_capacity g_layter_capability[LAYER_ID_BUTT] = {
    {   HI_TRUE, HI_FALSE, 0, HI_TRUE, HI_TRUE, HI_TRUE, HI_TRUE,
        HI_FALSE, { VDP_MAX_HEIGHT_8K, VDP_MAX_HEIGHT_8K },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,    VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,    VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        HI_DRV_PIXEL_BITWIDTH_12BIT,
        sizeof(g_layer0_format) / sizeof(hi_drv_pixel_format), g_layer0_format
    },

    {   HI_TRUE, HI_TRUE, FDR_REGION_NUM_MAX, HI_FALSE, HI_TRUE, HI_FALSE, HI_FALSE,
        HI_FALSE, { VDP_TYPE_WIDTH_8K, VDP_TYPE_HEIGHT_4K },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,     VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,     VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        HI_DRV_PIXEL_BITWIDTH_10BIT,
        sizeof(g_layer1_format) / sizeof(hi_drv_pixel_format), g_layer1_format
    },

    {   HI_TRUE, HI_FALSE, 0, HI_TRUE, HI_TRUE, HI_TRUE, HI_TRUE,
        HI_FALSE, { VDP_MAX_HEIGHT_8K, VDP_MAX_HEIGHT_8K },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,    VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,    VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        HI_DRV_PIXEL_BITWIDTH_12BIT,
        sizeof(g_layer0_format) / sizeof(hi_drv_pixel_format), g_layer0_format
    },

    {   HI_TRUE, HI_FALSE, 0, HI_FALSE, HI_TRUE, HI_FALSE, HI_FALSE,
        HI_FALSE, { VDP_TYPE_WIDTH_4K, VDP_TYPE_HEIGHT_2K },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,     VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        { VDP_LAYER_ALIGN,   VDP_LAYER_ALIGN,     VDP_LAYER_ALIGN, VDP_LAYER_ALIGN },
        HI_DRV_PIXEL_BITWIDTH_10BIT,
        sizeof(g_layer3_format) / sizeof(hi_drv_pixel_format), g_layer3_format
    },
};

// :TODO: need update by chip 0626
#define VDP_CHIP_LOWPOWER_VALUE    0x00005b2b
#define VDP_CHIP_VERSION_PART_LOW  0x76756F76UL
#define VDP_CHIP_VERSION_PART_HIGH 0x30313134UL

hi_void init_vdp_capaciblity(hi_void)
{
    g_vdp_capability.lowpower_value = VDP_CHIP_LOWPOWER_VALUE;
    g_vdp_capability.version_part_low = VDP_CHIP_VERSION_PART_LOW;
    g_vdp_capability.version_part_high = VDP_CHIP_VERSION_PART_HIGH;
}

hi_void init_layer_capaciblity(hi_void)
{
    return;
}
// :TODO: need update by otp 0626
hi_void init_disp_capaciblity_by_otp(disp_capacity *disp_capability)
{
    /* hdr support */
    disp_capability->support_hdr_hdr10 = HI_TRUE;
    disp_capability->support_hdr_hlg = HI_TRUE;
    disp_capability->support_hdr_slf = HI_TRUE;
    disp_capability->support_hdr_dolby = HI_TRUE;
    disp_capability->support_hdr_jtp = HI_TRUE;
    disp_capability->support_microvison = HI_TRUE;
}

hi_void init_disp_capaciblity(hi_void)
{
    g_disp_capability[HI_DRV_DISPLAY_1].support = HI_TRUE;
    g_disp_capability[HI_DRV_DISPLAY_1].support_wbc = HI_TRUE;
    g_disp_capability[HI_DRV_DISPLAY_1].support_attach_disp = HI_DRV_DISPLAY_0;
    g_disp_capability[HI_DRV_DISPLAY_1].support_intf_num =
        sizeof(g_disp_intf) / sizeof(hi_drv_disp_intf_id);
    g_disp_capability[HI_DRV_DISPLAY_1].support_intf = g_disp_intf;
    g_disp_capability[HI_DRV_DISPLAY_1].support_fmt_section_num =
        sizeof(g_disp1_fmt_section) / sizeof(fmt_section);
    g_disp_capability[HI_DRV_DISPLAY_1].support_fmt_section = g_disp1_fmt_section;
    g_disp_capability[HI_DRV_DISPLAY_1].support_csc_num =
        sizeof(g_disp_csc) / sizeof(hi_drv_disp_output_color_space);
    g_disp_capability[HI_DRV_DISPLAY_1].support_csc = g_disp_csc;
    g_disp_capability[HI_DRV_DISPLAY_1].support_type_num =
        sizeof(g_disp_type) / sizeof(hi_drv_disp_out_type);
    g_disp_capability[HI_DRV_DISPLAY_1].support_type = g_disp_type;
    g_disp_capability[HI_DRV_DISPLAY_1].support_interlaced = HI_FALSE;
    init_disp_capaciblity_by_otp(&g_disp_capability[HI_DRV_DISPLAY_1]);

    g_disp_capability[HI_DRV_DISPLAY_0].support = HI_TRUE;
    g_disp_capability[HI_DRV_DISPLAY_0].support_wbc = HI_TRUE;
    g_disp_capability[HI_DRV_DISPLAY_0].support_attach_disp = HI_DRV_DISPLAY_1;
    g_disp_capability[HI_DRV_DISPLAY_0].support_intf_num =
        sizeof(g_disp_intf) / sizeof(hi_drv_disp_intf_id);
    g_disp_capability[HI_DRV_DISPLAY_0].support_intf = g_disp_intf;
    g_disp_capability[HI_DRV_DISPLAY_0].support_fmt_section_num =
        sizeof(g_disp0_fmt_section) / sizeof(fmt_section);
    g_disp_capability[HI_DRV_DISPLAY_0].support_fmt_section = g_disp0_fmt_section;
    g_disp_capability[HI_DRV_DISPLAY_0].support_csc_num =
        sizeof(g_disp_csc) / sizeof(hi_drv_disp_output_color_space);
    g_disp_capability[HI_DRV_DISPLAY_0].support_csc = g_disp_csc;
    g_disp_capability[HI_DRV_DISPLAY_0].support_type_num =
        sizeof(g_disp_type) / sizeof(hi_drv_disp_out_type);
    g_disp_capability[HI_DRV_DISPLAY_0].support_type = g_disp_type;
    g_disp_capability[HI_DRV_DISPLAY_0].support_interlaced = HI_FALSE;
    init_disp_capaciblity_by_otp(&g_disp_capability[HI_DRV_DISPLAY_0]);
}

hi_void vdp_chip_capaciblity_init(hi_void)
{
    init_vdp_capaciblity();
    init_layer_capaciblity();
    init_disp_capaciblity();
}

vdp_capacity *hal_vdp_get_chip_capacity(void)
{
    return &g_vdp_capability;
}

layer_capacity *hal_layer_get_chip_capacity(hi_u32 layer)
{
    if (layer >= LAYER_ID_BUTT) {
        VDP_ERROR("error layer = %d max=%d\n", layer, LAYER_ID_BUTT - 1);
        return &g_layter_capability[LAYER_ID_VID0];
    }
    return &g_layter_capability[layer];
}

disp_capacity *hal_disp_get_chip_capacity(hi_drv_display chn)
{
    if (chn >= HI_DRV_DISPLAY_BUTT) {
        VDP_ERROR("error chn = %d max=%d\n", chn, HI_DRV_DISPLAY_BUTT - 1);
        return &g_disp_capability[HI_DRV_DISPLAY_0];
    }
    return &g_disp_capability[chn];
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
