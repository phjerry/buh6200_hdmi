/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_csc.h"

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_hipp_csc.h"

#ifdef VDP_SDK_PQ_SUPPORT
#include "vdp_ext_func.h"
#endif

#ifdef VDP_CBB_TEST_SUPPORT
#include "hal_vdp_test_csc.h"
#endif

static hi_u32 g_csc_offset_reg[VDP_CSC_ID_BUTT] = {
    (0x03200 - 0x03200) / 4, /* vid1 */ /* 4 is number */
    (0x05200 - 0x03200) / 4, /* vid3 */ /* 4 is number */
    (0x32200 - 0x03200) / 4, /* vid0 */ /* 4 is number */
    (0x94480 - 0x03200) / 4, /* dhd0-sr0 */ /* 4 is number */
    (0x94580 - 0x03200) / 4, /* dhd0-srs1 */ /* 4 is number */
    (0x91760 - 0x03200) / 4, /* mipi */ /* 4 is number */
};

hi_bool vdp_ip_csc_check_is_rgb_type(hi_drv_color_descript cs)
{
    if (cs.color_space == HI_DRV_COLOR_CS_RGB) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

#ifdef VDP_SDK_PQ_SUPPORT

#define COEF_PQ_10_TO_12BIT 2
#define DEFAULT_10BIT_MAX   1024
hi_void vdp_csc_set_coef(hi_u32 layer, hi_u32 offset, hi_drv_color_descript i_mode,
                         hi_drv_color_descript o_mode)
{
    hi_drv_pq_csc_layer layer_id;
    hi_drv_pq_csc_info csc_info;
    hi_drv_pq_csc_coef csc_coef;
    hi_u32 bit_convert, max_value;

    csc_info.csc_en = HI_TRUE;
    csc_info.csc_in = i_mode;
    csc_info.csc_out = o_mode;

    if (layer == VDP_CSC_ID_VID0) {
        layer_id = HI_DRV_PQ_CSC_LAYER_V0;
        csc_info.csc_pecision = 12; /* 12 is a pecison */
        csc_info.csc_type = HI_PQ_CSC_TUNING_V0;
        bit_convert = COEF_PQ_10_TO_12BIT;
        max_value = 4095; /* 4095 is a reg value */
    } else if (layer == VDP_CSC_ID_VID3) {
        layer_id = HI_DRV_PQ_CSC_LAYER_V3;
        csc_info.csc_pecision = 10; /* 10 is a pecison */
        csc_info.csc_type = HI_PQ_CSC_TUNING_V3;
        bit_convert = 0;
        max_value = 1023; /* 1023 is a reg value */
    } else {
        VDP_PRINT("not support csc %d now\n", layer);
        return;
    }

    vdp_ext_pq_get_csc_coef(layer_id, csc_info, &csc_coef);

    vdp_hipp_csc_sethippcscdemoen(g_vdp_reg, offset, HI_FALSE);
    vdp_hipp_csc_sethippcsccoef00(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef00);
    vdp_hipp_csc_sethippcsccoef01(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef01);
    vdp_hipp_csc_sethippcsccoef02(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef02);
    vdp_hipp_csc_sethippcsccoef10(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef10);
    vdp_hipp_csc_sethippcsccoef11(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef11);
    vdp_hipp_csc_sethippcsccoef12(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef12);
    vdp_hipp_csc_sethippcsccoef20(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef20);
    vdp_hipp_csc_sethippcsccoef21(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef21);
    vdp_hipp_csc_sethippcsccoef22(g_vdp_reg, offset, csc_coef.csc_ac_coef.csc_coef22);
    vdp_hipp_csc_sethippcscscale(g_vdp_reg, offset, 10); /* 10 is a reg value */

    vdp_hipp_csc_sethippcscidc0(g_vdp_reg, offset, csc_coef.csc_dc_coef.csc_in_dc0 << bit_convert);
    vdp_hipp_csc_sethippcscidc1(g_vdp_reg, offset, csc_coef.csc_dc_coef.csc_in_dc1 << bit_convert);
    vdp_hipp_csc_sethippcscidc2(g_vdp_reg, offset, csc_coef.csc_dc_coef.csc_in_dc2 << bit_convert);
    vdp_hipp_csc_sethippcscodc0(g_vdp_reg, offset, csc_coef.csc_dc_coef.csc_out_dc0 << bit_convert);
    vdp_hipp_csc_sethippcscodc1(g_vdp_reg, offset, csc_coef.csc_dc_coef.csc_out_dc1 << bit_convert);
    vdp_hipp_csc_sethippcscodc2(g_vdp_reg, offset, csc_coef.csc_dc_coef.csc_out_dc2 << bit_convert);

    vdp_hipp_csc_sethippcscminy(g_vdp_reg, offset, 0);
    vdp_hipp_csc_sethippcscminc(g_vdp_reg, offset, 0);
    vdp_hipp_csc_sethippcscmaxy(g_vdp_reg, offset, max_value);
    vdp_hipp_csc_sethippcscmaxc(g_vdp_reg, offset, max_value);

    return;
}
#endif

hi_void vdp_ip_csc_set(vdp_csc_id csc_id, hi_drv_color_descript i_mode, hi_drv_color_descript o_mode)
{
    hi_u32 offset = g_csc_offset_reg[csc_id];

#ifdef VDP_SDK_PQ_SUPPORT
    vdp_hipp_csc_sethippcscckgten(g_vdp_reg, offset, HI_TRUE);
    vdp_hipp_csc_sethippcscen(g_vdp_reg, offset, HI_TRUE);
    vdp_csc_set_coef(csc_id, offset, i_mode, o_mode);
#else
    if (vdp_ip_csc_check_is_rgb_type(i_mode) ==
        vdp_ip_csc_check_is_rgb_type(o_mode)) {
        vdp_hipp_csc_sethippcscen(g_vdp_reg, offset, HI_FALSE);
    } else {
        vdp_hipp_csc_sethippcscckgten(g_vdp_reg, offset, HI_TRUE);
        vdp_hipp_csc_sethippcscen(g_vdp_reg, offset, HI_TRUE);

#ifdef VDP_CBB_TEST_SUPPORT
        vdp_csc_test(csc_id, offset, vdp_ip_csc_check_is_rgb_type(o_mode) ? VDP_CSC_RGB_MODE : VDP_CSC_YUV_MODE);
#endif
    }
#endif

    return;
}

hi_void vdp_ip_csc_disable(vdp_csc_id csc_id)
{
    hi_u32 offset = g_csc_offset_reg[csc_id];

    vdp_hipp_csc_sethippcscen(g_vdp_reg, offset, HI_FALSE);
}

hi_s32 vdp_ip_csc_set_ink(hi_u32 layer, vdp_csc_ink_info *ink_info)
{
    hi_u32 offset = g_csc_offset_reg[layer];

    if (layer == VDP_CSC_ID_INTF_MIPI) {
        return HI_FAILURE;
    }

    vdp_hipp_csc_setcolormode(g_vdp_reg, offset, ink_info->color_mode);
    vdp_hipp_csc_setcrossenable(g_vdp_reg, offset, ink_info->cross_en);
    vdp_hipp_csc_setdatafmt(g_vdp_reg, offset, ink_info->data_fmt);
    vdp_hipp_csc_setinksel(g_vdp_reg, offset, ink_info->ink_sel);
    vdp_hipp_csc_setinken(g_vdp_reg, offset, ink_info->ink_en);
    vdp_hipp_csc_setypos(g_vdp_reg, offset, ink_info->y);
    vdp_hipp_csc_setxpos(g_vdp_reg, offset, ink_info->x);

    return HI_SUCCESS;
}

hi_s32 vdp_ip_csc_get_ink(hi_u32 layer, vdp_bkg *bkg)
{
    hi_u32 offset = g_csc_offset_reg[layer];

    hi_u32 data;
    hi_u32 data2;

    if (layer >= VDP_CSC_ID_INTF_MIPI) {
        return HI_FAILURE;
    }

    data = vdp_hipp_csc_getinkdata(g_vdp_reg, offset);
    data2 = vdp_hipp_csc_getinkdata2(g_vdp_reg, offset);

    bkg->y = data & 0xffff0000;
    bkg->u = data & 0x0000ffff;
    bkg->v = data2 & 0xffff0000;
    bkg->a = data2 & 0x0000ffff;

    return HI_SUCCESS;
}

hi_void vdp_ip_csc_data_transfer(hi_drv_disp_color *rgb,
                                 hi_drv_color_descript cs, vdp_bkg *yuv)
{
    hi_s32 red, y, green, u, blue, v;

    if (vdp_ip_csc_check_is_rgb_type(cs) == HI_TRUE) {
        yuv->y = (hi_u32)rgb->red << 2;   /* 2 is number */
        yuv->u = (hi_u32)rgb->green << 2; /* 2 is number */
        yuv->v = (hi_u32)rgb->blue << 2;  /* 2 is number */
        return;
    }

    red = (hi_s32)rgb->red;
    green = (hi_s32)rgb->green;
    blue = (hi_s32)rgb->blue;

    /* change rgb to yuv. */
    y = (257 * red + 504 * green + 98 * blue) / 1000 + 16;    /* 257, 504, 98, 1000, 16 is number */
    u = (-148 * red - 291 * green + 439 * blue) / 1000 + 128; /* 148, 291, 439, 1000, 128 is number */
    v = (439 * red - 368 * green - 71 * blue) / 1000 + 128;   /* 439, 368, 71, 1000, 128 is number */

    if (y < 16) {         /* 16 is number */
        y = 16;           /* 16 is number */
    } else if (y > 235) { /* 235 is number */
        y = 235;          /* 235 is number */
    }

    if (u < 16) {         /* 16 is number */
        u = 16;           /* 16 is number */
    } else if (u > 240) { /* 240 is number */
        u = 240;          /* 240 is number */
    }

    if (v < 16) {         /* 16 is number */
        v = 16;           /* 16 is number */
    } else if (v > 240) { /* 240 is number */
        v = 240;          /* 240 is number */
    }

    yuv->y = (hi_u32)y << 2; /* 2 is number */
    yuv->u = (hi_u32)u << 2; /* 2 is number */
    yuv->v = (hi_u32)v << 2; /* 2 is number */

    return;
}