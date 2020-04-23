/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal zme api
 * Author: pq
 * Create: 2016-01-1
 */

#include <linux/fs.h>
#include "pq_hal_zme.h"
#include "drv_pq.h"
#include "pq_hal_comm.h"

#define ZME_MIN(a, b) (((a) > (b)) ? (b) : (a))

static hi_bool g_hal_vdp_zme_fir_en = HI_TRUE;
static hi_bool g_hal_vdp_zme_med_en = HI_FALSE;
static hi_bool g_sg_b_vdp_zme_default = HI_FALSE;

static hi_pq_proc_vdpzme_strategy g_sg_st_set_pq_proc_vdp_zme_strategy = { 0 };
static hi_pq_proc_get_vdpzme_strategy g_sg_st_get_pq_proc_vdp_zme_strategy = { 0 };
static hi_pq_proc_vdpzme_reso g_sg_st_proc_vdp_zme_reso = { { 0 } };

pq_common_status g_zme_reso_status = { 0 };
extern pq_zme_coef_addr g_pq_vdp_zme_coef_buf[PQ_HAL_ZME_COEF_RATIO_MAX];

hi_s32 pq_hal_get_hzme_ratio(hi_drv_pq_zme_common_out *zme_out, pq_hal_zme_coef_ratio *ratio)
{
    hi_u32 w_in, w_out;
    w_in = zme_out->zme_w_in;
    w_out = zme_out->zme_w_out;

    if (w_in < w_out) {
        *ratio = PQ_HAL_ZME_COEF_1;
    } else if (w_in == w_out) {
        *ratio = PQ_HAL_ZME_COEF_E1;
    } else if (w_out >= w_in * 75 / 100) { /* 75/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_075;
    } else if (w_out >= w_in * 50 / 100) { /* 50/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_05;
    } else if (w_out >= w_in * 33 / 100) { /* 33/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_033;
    } else if (w_out >= w_in * 25 / 100) { /* 25/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_025;
    } else {
        *ratio = PQ_HAL_ZME_COEF_0;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vzme_ratio(hi_drv_pq_zme_common_out *zme_out, pq_hal_zme_coef_ratio *ratio)
{
    hi_u32 h_in, h_out;
    h_in = zme_out->zme_h_in;
    h_out = zme_out->zme_h_out;

    if (h_in < h_out) {
        *ratio = PQ_HAL_ZME_COEF_1;
    } else if (h_in == h_out) {
        *ratio = PQ_HAL_ZME_COEF_E1;
    } else if (h_out >= h_in * 75 / 100) { /* 75/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_075;
    } else if (h_out >= h_in * 50 / 100) { /* 50/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_05;
    } else if (h_out >= h_in * 33 / 100) { /* 33/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_033;
    } else if (h_out >= h_in * 25 / 100) { /* 25/100: ratio */
        *ratio = PQ_HAL_ZME_COEF_025;
    } else {
        *ratio = PQ_HAL_ZME_COEF_0;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_vdp_zme_default(hi_bool on_off)
{
    g_sg_b_vdp_zme_default = on_off;

    return HI_SUCCESS;
}

static hi_void pq_hal_adjust_zme_fir_en_cfg(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                            hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 zme_node = HI_DRV_PQ_4KZME;
    if ((g_sg_b_vdp_zme_default == HI_TRUE) ||
        ((zme_in->is_dolby_path == HI_TRUE) &&
         (HI_DRV_PQ_FRM_FRAME == g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].frame_fmt) &&
         (zme_in->zme_common_info.zme_w_in == zme_in->zme_common_info.zme_w_out) &&
         (zme_in->zme_common_info.zme_h_in == zme_in->zme_common_info.zme_h_out))) {
        for (zme_node = HI_DRV_PQ_4KZME; zme_node < HI_DRV_PQ_NODE_MAX; zme_node++) {
            zme_out->zme_fmt[zme_node].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_COPY;
            zme_out->zme_fmt[zme_node].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_COPY;
            zme_out->zme_fmt[zme_node].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_COPY;
            zme_out->zme_fmt[zme_node].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_COPY;

            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_h = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_hl = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_hc = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_v = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_vl = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_vc = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_v_tp = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_v_btm = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_vl_btm = 0;
            zme_out->zme_fmt[zme_node].zme_offset.zme_offset_vc_btm = 0;
        }
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_8k120_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h * 2 <= video_h_out * 3) { /* 2/3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 2) { /* 2: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 9) { /* 9: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_8k60_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 9) { /* 9: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_4k120_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (disp_h <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 2) { /* 2: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 9) { /* 9: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 18) { /* 18: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_4k60_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h > video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 18) { /* 18: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_fhd_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 11) { /* 11: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if ((timming == PQ_OUTPUT_MODE_SD) || (timming == PQ_OUTPUT_MODE_FHD)) { /* fhd 及以下制式 */
        pq_hal_v0_equal_ratio_fhd_vpre_zme_config(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate <= REFRESHRATE_60)) { /* 4K60 制式 */
        pq_hal_v0_equal_ratio_4k60_vpre_zme_config(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate > REFRESHRATE_60)) { /* 4K120制式 */
        pq_hal_v0_equal_ratio_4k120_vpre_zme_config(zme_in, zme_out);
    } else if (timming == PQ_OUTPUT_MODE_8K60) { /* 8K60 制式 */
        pq_hal_v0_equal_ratio_8k60_vpre_zme_config(zme_in, zme_out);
    } else if (timming == PQ_OUTPUT_MODE_8K120) { /* 8k120制式 */
        pq_hal_v0_equal_ratio_8k120_vpre_zme_config(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_v0_hpre_zme_ajust(hi_drv_pq_hd_zme_strategy_in* zme_in,
                                        hi_drv_pq_hd_zme_strategy_out* zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);

    if ((timming == PQ_OUTPUT_MODE_8K120) || (timming == PQ_OUTPUT_MODE_8K60)
        || ((timming == PQ_OUTPUT_MODE_4K) &&
            (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate > REFRESHRATE_60))) {
        while (zme_out->hor_draw_mul > HI_DRV_PQ_PREZME_HOR_DISABLE) {
            if (zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul < PQ_WIDTH_256) {
                zme_out->hor_draw_mul = zme_out->hor_draw_mul / 2; /* 2: num */
            } else {
                break;
            }
        }
    }  else {
        while (zme_out->hor_draw_mul > HI_DRV_PQ_PREZME_HOR_DISABLE) {
            if (zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul <
                zme_in->zme_common_info.zme_w_out * 2) { /* 2: num */
                zme_out->hor_draw_mul = zme_out->hor_draw_mul / 2; /* 2: num */
            } else {
                break;
            }
        }
    }
}

static hi_void pq_hal_v0_vpre_zme_ajust(hi_drv_pq_hd_zme_strategy_in* zme_in,
                                        hi_drv_pq_hd_zme_strategy_out* zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    if ((timming == PQ_OUTPUT_MODE_8K120) || (timming == PQ_OUTPUT_MODE_8K60)
        || ((timming == PQ_OUTPUT_MODE_4K) &&
            (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate > REFRESHRATE_60))) {
        while (zme_out->ver_draw_mul > HI_DRV_PQ_PREZME_VER_DISABLE) {
            if (zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul < PQ_HEIGHT_256) {
                zme_out->ver_draw_mul = zme_out->ver_draw_mul / 2; /* 2: num */
            } else {
                break;
            }
        }
    }  else {
        while (zme_out->ver_draw_mul > HI_DRV_PQ_PREZME_VER_DISABLE) {
            if (zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul <
                zme_in->zme_common_info.zme_h_out) {
                zme_out->ver_draw_mul = zme_out->ver_draw_mul / 2; /* 2: num */
            } else {
                break;
            }
        }
    }
}

static hi_void pq_hal_v0_unequal_ratio_8k120_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h * 2 <= video_h_out * 3) { /* 2/3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 2) { /* 2: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_unequal_ratio_8k60_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_unequal_ratio_4k120_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_32;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_unequal_ratio_4k60_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else if (disp_h > video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 18) { /* 18: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_unequal_ratio_fhd_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_ver_draw_mul ver_draw_tmp1, ver_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 3) { /* 3: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_2;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_4;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_8;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp1 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 12) { /* 12: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_8;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_16;
    } else {
        ver_draw_tmp2 = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = MAX2(ver_draw_tmp1, ver_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_unequal_ratio_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if ((timming == PQ_OUTPUT_MODE_SD) || (timming == PQ_OUTPUT_MODE_FHD)) { /* fhd 及以下制式 */
        pq_hal_v0_unequal_ratio_fhd_vpre_zme_config(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate <= REFRESHRATE_60)) { /* 4K60 制式 */
        pq_hal_v0_unequal_ratio_4k60_vpre_zme_config(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate > REFRESHRATE_60)) { /* 4K120制式 */
        pq_hal_v0_unequal_ratio_4k120_vpre_zme_config(zme_in, zme_out);
    } else if (timming == PQ_OUTPUT_MODE_8K60) { /* 8K60 制式 */
        pq_hal_v0_unequal_ratio_8k60_vpre_zme_config(zme_in, zme_out);
    } else if (timming == PQ_OUTPUT_MODE_8K120) { /* 8k120制式 */
        pq_hal_v0_unequal_ratio_8k120_vpre_zme_config(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_8k120_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_hor_draw_mul hor_draw_tmp1, hor_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_8;
    } else {
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (video_h_in <= video_h_out * 9) { /* 9: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_8;
    } else {
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = MAX2(hor_draw_tmp1, hor_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_8k60_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_hor_draw_mul hor_draw_tmp1, hor_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    }else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_8;
    } else {
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (video_h_in <= video_h_out * 9) { /* 9: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_8;
    } else {
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = MAX2(hor_draw_tmp1, hor_draw_tmp2);
    }
}

static hi_void pq_hal_v0_equal_ratio_4k120_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_hor_draw_mul hor_draw_tmp1, hor_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 4) { /* 4: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else {
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (video_h_in <= video_h_out * 9) { /* 9: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= video_h_out * 18) { /* 18: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else {
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = MAX2(hor_draw_tmp1, hor_draw_tmp2);
    }
}

static hi_void pq_hal_v0_equal_ratio_4k60_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_hor_draw_mul hor_draw_tmp1, hor_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h > video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else {
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out * 12) { /* 12: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (video_h_in <= video_h_out * 18) { /* 18: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else {
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = MAX2(hor_draw_tmp1, hor_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_fhd_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_u32 disp_h;
    hi_drv_pq_hor_draw_mul hor_draw_tmp1, hor_draw_tmp2;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    if (disp_h <= video_h_out * 6) { /* 6: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (disp_h <= video_h_out * 12) { /* 12: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (disp_h <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (disp_h <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_8;
    } else if (disp_h > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_8;
    } else {
        hor_draw_tmp1 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out * 6) { /* 6: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (video_h_in <= video_h_out * 11) { /* 11: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= video_h_out * 24) { /* 24: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_4;
    } else if (video_h_in <= video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_8;
    } else if (video_h_in > video_h_out * 48) { /* 48: draw multi */
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_8;
    } else {
        hor_draw_tmp2 = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = MAX2(hor_draw_tmp1, hor_draw_tmp2);
    }

    return;
}

static hi_void pq_hal_v0_equal_ratio_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if ((timming == PQ_OUTPUT_MODE_SD) || (timming == PQ_OUTPUT_MODE_FHD)) { /* fhd 及以下制式 */
        pq_hal_v0_equal_ratio_fhd_hpre_zme_config(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate <= REFRESHRATE_60)) { /* 4K60 制式 */
        pq_hal_v0_equal_ratio_4k60_hpre_zme_config(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate > REFRESHRATE_60)) { /* 4K120 制式 */
        pq_hal_v0_equal_ratio_4k120_hpre_zme_config(zme_in, zme_out);
    } else if (timming == PQ_OUTPUT_MODE_8K60) { /* 8K60 制式 */
        pq_hal_v0_equal_ratio_8k60_hpre_zme_config(zme_in, zme_out);
    } else if (timming == PQ_OUTPUT_MODE_8K120) { /* 8k120制式 */
        pq_hal_v0_equal_ratio_8k120_hpre_zme_config(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_v0_unequal_ratio_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 w_in = zme_in->zme_common_info.zme_w_in;
    hi_u32 w_out = zme_in->zme_common_info.zme_w_out;
    hi_u32 ratio = w_in / (w_out * 4); /* 4:draw multi */

    if (ratio < 2) { /* 2:draw multi */
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (ratio < 4) { /* 4:draw multi */
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_2;
    } else if (ratio < 8) { /* 8:draw multi */
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_4;
    } else if (ratio < 16) { /* 16:draw multi */
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_8;
    } else if (ratio < 32) { /* 32:draw multi */
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_16;
    } else if (ratio >= 32) { /* 32:draw multi */
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_32;
    } else {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }
}

static hi_void pq_hal_set_v0_8kzme_offset(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_h = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_hl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_hc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_v = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_vl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_vc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_v_tp = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_v_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_vl_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_offset.zme_offset_vc_btm = 0;

    return;
}

static hi_void pq_hal_set_v0_4kzme_offset(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_h = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_hl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_hc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_v = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_vl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_vc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_v_tp = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_v_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_vl_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_offset.zme_offset_vc_btm = 0;

    return;
}

static hi_void pq_hal_set_v0_4ksr_offset(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_h = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_hl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_hc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_v = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_vl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_vc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_v_tp = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_v_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_vl_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_offset.zme_offset_vc_btm = 0;

    return;
}

static hi_void pq_hal_set_v0_8ksr_offset(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_h = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_hl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_hc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_v = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_vl = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_vc = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_v_tp = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_v_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_vl_btm = 0;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_offset.zme_offset_vc_btm = 0;

    return;
}

static hi_void pq_hal_set_v0_all_zme_offset(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                            hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable == HI_TRUE) {
        pq_hal_set_v0_8kzme_offset(zme_out);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable == HI_TRUE) {
        pq_hal_set_v0_4kzme_offset(zme_out);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable == HI_TRUE) {
        pq_hal_set_v0_4ksr_offset(zme_out);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable == HI_TRUE) {
        pq_hal_set_v0_8ksr_offset(zme_out);
    }

    return;
}

hi_void pq_mng_v0_zme_reso_check(hi_drv_pq_hd_zme_strategy_in *zme_in)
{
    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_in->zme_common_info.zme_w_in, PQ_WIDTH_8192);
    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_in->zme_common_info.zme_h_in, PQ_HEIGHT_4352);
    PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_in->zme_common_info.zme_w_in, PQ_WIDTH_64);
    PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_in->zme_common_info.zme_h_in, PQ_HEIGHT_64);

    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_in->zme_common_info.zme_w_out, PQ_WIDTH_7680);
    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_in->zme_common_info.zme_h_out, PQ_HEIGHT_4320);
    PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_in->zme_common_info.zme_w_out, PQ_WIDTH_64);
    PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_in->zme_common_info.zme_h_out, PQ_HEIGHT_64);

    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_in->zme_common_info.zme_w_out,
                                    g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_w);
    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_in->zme_common_info.zme_h_out,
                                    g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h);

    return;
}

static hi_void pq_hal_reset_zme_reso(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                     hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point == HI_DRV_PQ_8KZME_POS0) {
        if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable == HI_FALSE) {
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
                zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
                zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in;
        } else {
            zme_out->zme_num++;
        }
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable == HI_FALSE) {
        if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point == HI_DRV_PQ_8KZME_POS0) {
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;
        } else {
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in =
                zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in =
                zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;
        }
    } else {
        zme_out->zme_num++;
    }

    return;
}

static hi_void pq_hal_reset_sr_reso(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    if (zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable == HI_FALSE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in;
    } else {
        zme_out->zme_num++;
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable == HI_FALSE) {
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in;
    } else {
        zme_out->zme_num++;
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point == HI_DRV_PQ_8KZME_POS1) {
        if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable == HI_FALSE) {
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in;
        } else {
            zme_out->zme_num++;
        }
    }

    return;
}

static hi_void pq_hal_reset_all_zme_reso(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                         hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_num = 0;
    pq_hal_reset_zme_reso(zme_in, zme_out);
    pq_hal_reset_sr_reso(zme_in, zme_out);

    return;
}

static hi_void pq_hal_8kfmt_equal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                                    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out, disp_h;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */

    /* 8K120或8K60缩小场景时只开8KZME */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;

    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;

    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;

    return;
}

static hi_void pq_hal_4k120_equal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                                    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out, disp_h;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    /* 只开启8KZME */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;

    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;

    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;

    return;
}

static hi_void pq_hal_check_smaller_256(hi_drv_pq_hd_zme_strategy_in* zme_in,
                                        hi_drv_pq_hd_zme_strategy_out* zme_out, hi_bool* is_smaller_256)
{
    hi_u32 draw_h, draw_w;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    draw_h = zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul;
    draw_w = zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul;

    if ((draw_h < PQ_HEIGHT_256) || (draw_w < PQ_WIDTH_256) ||
        (zme_in->zme_common_info.zme_w_out < PQ_WIDTH_256) ||
        (zme_in->zme_common_info.zme_h_out < PQ_HEIGHT_256)) {
        *is_smaller_256 = HI_TRUE;
    } else {
        *is_smaller_256 = HI_FALSE;
    }

    if (*is_smaller_256 == HI_TRUE) {
        while (zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul > PQ_WIDTH_4096) {
            zme_out->hor_draw_mul = zme_out->hor_draw_mul * 2; /* 2:draw multi */
        }

        while (zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul > PQ_HEIGHT_2160) {
            zme_out->ver_draw_mul = zme_out->ver_draw_mul * 2; /* 2:draw multi */
        }
    }
}

static hi_void pq_hal_4k60_equal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                                   hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out, disp_h;
    hi_bool is_smaller_256 = HI_FALSE;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    pq_hal_check_smaller_256(zme_in, zme_out, &is_smaller_256);
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */

    if (is_smaller_256 == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        if ((disp_h <= video_h_out * 3) && (video_h_in <= video_h_out * 6)) { /* 3/6: multi */
            /* 只开启8KZME */
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;

            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;

            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;
        } else {
            /* 同时开启8KZME 和4KZME, 其中4KZME固定2倍缩小，剩余由8KZME完成 */
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = MAX2(PQ_WIDTH_256, MIN3(PQ_WIDTH_4096,
                zme_in->zme_common_info.zme_w_out * 2, /* 2: multi */
                zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2)); /* 2: multi */
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = MAX2(PQ_HEIGHT_256, MIN3(PQ_HEIGHT_2160,
                zme_in->zme_common_info.zme_h_out * 2, /* 2: multi */
                zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2)); /* 2: multi */

            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
        }
    }

    return;
}

static hi_void pq_hal_fhdfmt_equal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                                     hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out, disp_h;
    hi_bool is_smaller_256 = HI_FALSE;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    pq_hal_check_smaller_256(zme_in, zme_out, &is_smaller_256);
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */

    if (is_smaller_256 == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        if ((disp_h <= video_h_out * 3) && (video_h_in <= video_h_out * 4)) { /* 3/4: multi */
            /* 只开启8KZME */
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;

            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;

            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;
        } else {
            /* 同时开启8KZME 和4KZME, 其中4KZME固定2倍缩小，剩余由8KZME完成 */
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = MAX2(PQ_WIDTH_256, MIN3(PQ_WIDTH_4096,
                zme_in->zme_common_info.zme_w_out * 2, /* 2: multi */
                zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2)); /* 2: multi */
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = MAX2(PQ_HEIGHT_256, MIN3(PQ_HEIGHT_2160,
                zme_in->zme_common_info.zme_h_out * 2,  /* 2: multi */
                zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2)); /* 2: multi */

            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
        }
    }

    return;
}

static hi_void pq_hal_v0_zme_equal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                                     hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    if ((timming == PQ_OUTPUT_MODE_8K120) || (timming == PQ_OUTPUT_MODE_8K60)) {
        pq_hal_8kfmt_equal_ratio_scaler_down(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate > REFRESHRATE_60)) {
        pq_hal_4k120_equal_ratio_scaler_down(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate <= REFRESHRATE_60)) {
        pq_hal_4k60_equal_ratio_scaler_down(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_FHD) || (timming == PQ_OUTPUT_MODE_SD)) {
        pq_hal_fhdfmt_equal_ratio_scaler_down(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_8k120fmt_fullscreen_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 25) && (ratio_max <= 100)) { /* 100/25: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in * 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in * 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    }

    return;
}

static hi_void pq_hal_8k120fmt_nofullscreen_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out, disp_w;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    disp_w = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_w;

    if ((ratio_max > 50) && (ratio_max <= 100)) { /* 100/50: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else if ((ratio_max > 25) && (ratio_max <= 50) && (disp_w < video_w_out * 2)) { /* 25/50/2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else if ((ratio_max <= 25) || /* 25: multi */
               (((ratio_max > 25) && (ratio_max <= 50) && (disp_w >= video_w_out * 2)))) { /* 25/50/2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in * 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in * 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    }

    return;
}

static hi_void pq_hal_8k60fmt_fullscreen_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 50) && (ratio_max <= 100)) { /* 100/50: multi */
        /* 只开启8KZME */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else if ((ratio_max > 25) && (ratio_max <= 50)) { /* 25/50: multi */
        /* 同时开启8KZME 和4KZME, 其中4KZME固定2倍缩小，剩余由8KZME完成 */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in * 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in * 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        /* 同时开启8KZME 和4KZME, 其中4KZME固定4倍缩小，剩余由8KZME完成 */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in;
    }

    return;
}

static hi_void pq_hal_8k60fmt_nofullscreen_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 55) && (ratio_max <= 100)) { /* 55/100: multi */
        /* 只开启8KZME */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else if ((ratio_max > 30) && (ratio_max <= 55)) { /* 30/55: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        if (zme_in->zme_common_info.zme_w_in > 3840) { /* 3840: multi */
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;

            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
        } else {
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;

            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_in->zme_common_info.zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_in->zme_common_info.zme_h_in;
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in * 2; /* 2: multi */
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in * 2; /* 2: multi */

            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
        }
    } else if ((ratio_max > 25) && (ratio_max <= 30)) { /* 25/30: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        if (zme_in->zme_common_info.zme_w_in > PQ_WIDTH_1920) {
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;

            if (zme_in->zme_common_info.zme_w_in > PQ_WIDTH_3840) {
                zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
            } else {
                zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;

                zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_in->zme_common_info.zme_w_in;
                zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_in->zme_common_info.zme_h_in;
                zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in * 2; /* 2: */
                zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in * 2; /* 2: */

                zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
                zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
                zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
                zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
            }
        } else {
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_TRUE;
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;

            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in = zme_in->zme_common_info.zme_w_in;
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in = zme_in->zme_common_info.zme_h_in;
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in * 2; /* 2: multi */
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in * 2; /* 2: multi */

            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in * 2; /* 2: multi */
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in * 2; /* 2: multi */

            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
        }
    } else if (ratio_max <= 25) { /* 25: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in;
    }

    return;
}

static hi_void pq_hal_4k120fmt_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);

    if (zme_in->zme_common_info.zme_w_out > PQ_WIDTH_3840) { /* 50/100: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in;
    } else if ((ratio_max > 50) && (ratio_max <= 100)) { /* 50/100: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in =
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in =
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in;
    }

    return;
}

static hi_void pq_hal_4k60fmt4096_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);

    if (zme_in->zme_common_info.zme_w_out > PQ_WIDTH_3840) { /* 50/100: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in;
    } else if ((ratio_max > 50) && (ratio_max <= 100)) { /* 50/100: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    }else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in =
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in =
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in;
    }

    return;
}

static hi_void pq_hal_4k60fmt_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);

    if (zme_in->zme_common_info.zme_w_out > PQ_WIDTH_3840) { /* 50/100: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in;
    } else if ((ratio_max > 50) && (ratio_max <= 100)) { /* 50/100: multi */
        /* INFO: CS may be open 4kzme 1Dscaler */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out = zme_in->zme_common_info.zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in =
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out / 2; /* 2: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in =
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out / 2; /* 2: multi */

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in;
    }

    return;
}


static hi_void pq_hal_8k120fmt_fullscreen_close_sr_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 25) && (ratio_max <= 100)) { /* 100/25: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS1;
    }

    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    return;
}

static hi_void pq_hal_8k120fmt_nofullscreen_close_sr_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out, disp_w;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    disp_w = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_w;

    if ((ratio_max > 50) && (ratio_max <= 100)) { /* 100/50: multi */
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS1;
    }

    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    return;
}

static hi_void pq_hal_8k60fmt_fullscreen_close_sr_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 50) && (ratio_max <= 100)) { /* 100/50: multi */
        /* 只开启8KZME */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else if ((ratio_max >= 25) && (ratio_max <= 50)) { /* 25/50: multi */
        /*  只开4KZME */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS1;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        /*  只开4KZME */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    }

    return;
}

static hi_void pq_hal_8k60fmt_nofullscreen_close_sr_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 55) && (ratio_max <= 100)) { /* 55/100: multi */
        /* 只开启8KZME */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else if ((ratio_max > 25) && (ratio_max <= 55)) { /* 25/55: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS1;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    }

    return;
}

static hi_void pq_hal_4k120fmt_close_sr_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 50) && (ratio_max <= 100)) { /* 50/100: multi */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out / 2; /* 2: num */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out / 2; /* 2: num */

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    }

    return;
}

static hi_void pq_hal_4k60fmt_close_sr_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_w_in, video_h_out, video_w_out;
    hi_u32 ratio_h, ratio_w, ratio_max;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_in = zme_in->zme_common_info.zme_w_in;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    ratio_h = video_h_in * 100 / video_h_out; /* 100: multi */
    ratio_w = video_w_in * 100 / video_w_out; /* 100: multi */
    ratio_max = MAX2(ratio_h, ratio_w);
    if ((ratio_max > 50) && (ratio_max <= 100)) { /* 50/100: multi */
        /* INFO: CS may be open 4kzme 1Dscaler */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_in->zme_common_info.zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_in->zme_common_info.zme_h_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out / 2; /* 2: num */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out / 2; /* 2: num */

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    }

    return;
}
static hi_void pq_hal_v0_zme_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_out, video_w_out, disp_h, disp_w, refresh_rate;
    hi_bool is_full_screen = HI_TRUE;
    pq_output_mode timming = pq_comm_get_output_mode();

    refresh_rate = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;
    disp_w = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_w;

    if ((disp_w == video_w_out) && (disp_h == video_h_out)) {
        is_full_screen = HI_TRUE;
    } else {
        is_full_screen = HI_FALSE;
    }

    if ((timming == PQ_OUTPUT_MODE_8K120) && (is_full_screen == HI_TRUE)) {
        pq_hal_8k120fmt_fullscreen_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_8K120) && (is_full_screen != HI_TRUE)) {
        pq_hal_8k120fmt_nofullscreen_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_8K60) && (is_full_screen == HI_TRUE)) {
        pq_hal_8k60fmt_fullscreen_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_8K60) && (is_full_screen != HI_TRUE)) {
        pq_hal_8k60fmt_nofullscreen_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) && (refresh_rate > REFRESHRATE_60)) {
        pq_hal_4k120fmt_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) && (refresh_rate <= REFRESHRATE_60) &&
    (disp_w == PQ_WIDTH_4096) && (disp_h == PQ_HEIGHT_2160)) {
        pq_hal_4k60fmt4096_scaler_up(zme_in, zme_out);
    } else {
        pq_hal_4k60fmt_scaler_up(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_4k60_unequal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out, disp_h;
    hi_bool is_smaller_256 = HI_FALSE;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    pq_hal_check_smaller_256(zme_in, zme_out, &is_smaller_256);
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */

    if (is_smaller_256 == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        /* 只开启8KZME */
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;
    }

    return;
}

static hi_void pq_hal_fhdfmt_unequal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out, disp_h;
    hi_bool is_smaller_256 = HI_FALSE;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;

    pq_hal_check_smaller_256(zme_in, zme_out, &is_smaller_256);
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */

    if (is_smaller_256 == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
    } else {
        /* 只开启8KZME */
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_FALSE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in;
    }

    return;
}

static hi_void pq_hal_v0_zme_unequal_ratio_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if ((timming == PQ_OUTPUT_MODE_8K120) || (timming == PQ_OUTPUT_MODE_8K60)) {
        pq_hal_8kfmt_equal_ratio_scaler_down(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate > REFRESHRATE_60)) {
        pq_hal_4k120_equal_ratio_scaler_down(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) &&
               (g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate <= REFRESHRATE_60)) {
        pq_hal_4k60_unequal_ratio_scaler_down(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_FHD) || (timming == PQ_OUTPUT_MODE_SD)) {
        pq_hal_fhdfmt_unequal_ratio_scaler_down(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_set_all_zme_fmt(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                      hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    if ((zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point == HI_DRV_PQ_8KZME_POS0) &&
        (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable == HI_TRUE)) {
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_in = zme_in->zme_common_info.zme_fmt_in;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_out = zme_in->zme_common_info.zme_fmt_out;

        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_in =
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_out; /* zme input:  0-422; 1-420; 2-444 */
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_out =
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_in; /* zme output: 0-422; 1-420; 2-444 */
    } else if ((zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point == HI_DRV_PQ_8KZME_POS0) &&
        (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable == HI_FALSE)) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_in = zme_in->zme_common_info.zme_fmt_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_out = zme_in->zme_common_info.zme_fmt_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        /* zme input:  0-422; 1-420; 2-444 */
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_in = zme_in->zme_common_info.zme_fmt_in;
        /* zme output: 0-422; 1-420; 2-444 */
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_out = zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_in;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_in = zme_in->zme_common_info.zme_fmt_in;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_out = zme_in->zme_common_info.zme_fmt_out;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;

        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_in =
            zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_out; /* zme input:  0-422; 1-420; 2-444 */
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_out =
            zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fmt_in; /* zme output: 0-422; 1-420; 2-444 */
    }

    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fmt_in =
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fmt_out; /* zme input:  0-422; 1-420; 2-444 */
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fmt_out =
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fmt_in; /* zme output: 0-422; 1-420; 2-444 */
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fmt_in =
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fmt_out; /* zme input:  0-422; 1-420; 2-444 */
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fmt_out =
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fmt_in; /* zme output: 0-422; 1-420; 2-444 */

    return;
}

static hi_void pq_hal_set_all_zme_frame_fmt(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                            hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_drv_pq_v0_zme_node zme_node;
    /* ES片缩放不支持逐行转隔行，CS要完善 */
    if (zme_in->zme_common_info.zme_frame_in != zme_in->zme_common_info.zme_frame_out) {
        HI_ERR_PQ("zme frame fmt error, in is not the same out!\n");
        return;
    } else {
        for (zme_node = HI_DRV_PQ_4KZME; zme_node < HI_DRV_PQ_NODE_MAX; zme_node++) {
            zme_out->zme_fmt[zme_node].frame_fmt_in = zme_in->zme_common_info.zme_frame_in;
            zme_out->zme_fmt[zme_node].frame_fmt_out = zme_out->zme_fmt[zme_node].frame_fmt_in;
        }
    }

    return;
}

static hi_void pq_hal_src_lessthan_fhd_8kfmt_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_drv_pq_ver_draw_mul ver_draw_mul;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= video_h_out * 2) { /* 2: draw multi */
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= video_h_out * 8) { /* 8: draw multi */
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in > video_h_out * 8) { /* 8: draw multi */
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_8;
    }else {
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = ver_draw_mul;
    }

    return;
}

static hi_void pq_hal_src_lessthan_fhd_4kfmt_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_drv_pq_ver_draw_mul ver_draw_mul;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= video_h_out * 8) { /* 8: draw multi */
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in > video_h_out * 8) { /* 8: draw multi */
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_4;
    }else {
        ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = ver_draw_mul;
    }

    return;
}

static hi_void pq_hal_src_lessthan_fhd_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if ((timming == PQ_OUTPUT_MODE_8K120) || (timming == PQ_OUTPUT_MODE_8K60)) {
        pq_hal_src_lessthan_fhd_8kfmt_vpre_zme_config(zme_in, zme_out);
    } else {
        pq_hal_src_lessthan_fhd_4kfmt_vpre_zme_config(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_src_lessthan_fhd_8kfmt_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_drv_pq_hor_draw_mul hor_draw_tmp;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= video_h_out * 2) { /* 2: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= video_h_out * 8) { /* 8: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_4;
    } else if (video_h_in > video_h_out * 8) { /* 8: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_8;
    } else {
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = hor_draw_tmp;
    }

    return;
}

static hi_void pq_hal_src_lessthan_fhd_4kfmt_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_in, video_h_out;
    hi_drv_pq_hor_draw_mul hor_draw_tmp;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= video_h_out * 4) { /* 4: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else if (video_h_in <= video_h_out * 8) { /* 8: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in > video_h_out * 8) { /* 8: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_4;
    } else {
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_DISABLE;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = hor_draw_tmp;
    }

    return;
}

static hi_void pq_hal_src_lessthan_fhd_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if ((timming == PQ_OUTPUT_MODE_8K120) || (timming == PQ_OUTPUT_MODE_8K60)) {
        pq_hal_src_lessthan_fhd_8kfmt_hpre_zme_config(zme_in, zme_out);
    } else {
        pq_hal_src_lessthan_fhd_4kfmt_hpre_zme_config(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_zme_ajust_lessthan_fhd_scaler_down(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;

    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);
    pq_hal_src_lessthan_fhd_vpre_zme_config(zme_in, zme_out);
    pq_hal_src_lessthan_fhd_hpre_zme_config(zme_in, zme_out);

    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;

    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;

    return;
}

static hi_void pq_hal_v0_hdcp_8k120_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 hdcp_h_in, video_h_in, video_h_out;
    hi_drv_pq_ver_draw_mul ver_draw_tmp;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    hdcp_h_in = zme_in->hdcp_factor.hdcp_height;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= hdcp_h_in * 8) { /* 8: draw multi */
        ver_draw_tmp = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= hdcp_h_in * 16) { /* 16: draw multi */
        ver_draw_tmp = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in <= hdcp_h_in * 32) { /* 32: draw multi */
        ver_draw_tmp = HI_DRV_PQ_PREZME_VER_4;
    } else if (video_h_in > hdcp_h_in * 32) { /* 32: draw multi */
        ver_draw_tmp = HI_DRV_PQ_PREZME_VER_4;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = ver_draw_tmp;
    }

    return;
}

static hi_void pq_hal_v0_hdcp_8k60_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 hdcp_h_in, video_h_in, video_h_out;
    hi_drv_pq_ver_draw_mul ver_draw_tmp;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    hdcp_h_in = zme_in->hdcp_factor.hdcp_height;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= hdcp_h_in * 8) { /* 8: draw multi */
        ver_draw_tmp = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else if (video_h_in <= hdcp_h_in * 16) { /* 16: draw multi */
        ver_draw_tmp = HI_DRV_PQ_PREZME_VER_2;
    } else if (video_h_in > hdcp_h_in * 16) { /* 16: draw multi */
        ver_draw_tmp = HI_DRV_PQ_PREZME_VER_2;
    }

    if (video_h_in <= video_h_out) {
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
    } else {
        zme_out->ver_draw_mul = ver_draw_tmp;
    }

    return;
}


static hi_void pq_hal_v0_hdcp_vpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if (timming == PQ_OUTPUT_MODE_8K120) { /* 8K60 制式 */
        pq_hal_v0_hdcp_8k120_vpre_zme_config(zme_in, zme_out);
    } else { /* 8k120制式 */
        pq_hal_v0_hdcp_8k60_vpre_zme_config(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_v0_hdcp_8k120_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 hdcp_h_in, video_h_in, video_h_out;
    hi_drv_pq_hor_draw_mul hor_draw_tmp;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    hdcp_h_in = zme_in->hdcp_factor.hdcp_height;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= hdcp_h_in * 8) { /* 8: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= hdcp_h_in * 16) { /* 16: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_4;
    } else if (video_h_in <= hdcp_h_in * 32) { /* 32: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_8;
    } else if (video_h_in > hdcp_h_in * 32) { /* 32: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_8;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = hor_draw_tmp;
    }
}

static hi_void pq_hal_v0_hdcp_8k60_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 hdcp_h_in, video_h_in, video_h_out;
    hi_drv_pq_hor_draw_mul hor_draw_tmp;

    video_h_in = zme_in->zme_common_info.zme_h_in;
    hdcp_h_in = zme_in->hdcp_factor.hdcp_height;
    video_h_out = zme_in->zme_common_info.zme_h_out;

    if (video_h_in <= hdcp_h_in * 8) { /* 8: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= hdcp_h_in * 16) { /* 16: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_2;
    } else if (video_h_in <= hdcp_h_in * 32) { /* 32: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_4;
    } else if (video_h_in > hdcp_h_in * 32) { /* 32: draw multi */
        hor_draw_tmp = HI_DRV_PQ_PREZME_HOR_4;
    }

    if (video_h_in <= video_h_out) {
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
    } else {
        zme_out->hor_draw_mul = hor_draw_tmp;
    }
}

static hi_void pq_hal_v0_hdcp_hpre_zme_config(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_output_mode timming = pq_comm_get_output_mode();
    if (timming == PQ_OUTPUT_MODE_8K120) { /* 8K60 制式 */
        pq_hal_v0_hdcp_8k120_hpre_zme_config(zme_in, zme_out);
    } else if (timming == PQ_OUTPUT_MODE_8K60) { /* 8k120制式 */
        pq_hal_v0_hdcp_8k60_hpre_zme_config(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_v0_zme_hdcp_closr_sr_scaler(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_hal_v0_hdcp_vpre_zme_config(zme_in, zme_out);
    pq_hal_v0_hdcp_hpre_zme_config(zme_in, zme_out);

    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;

    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in =
        zme_in->zme_common_info.zme_w_in / zme_out->hor_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in =
        zme_in->zme_common_info.zme_h_in / zme_out->ver_draw_mul / 2 * 2; /* 2: align */
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out = zme_in->hdcp_factor.hdcp_width;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out = zme_in->hdcp_factor.hdcp_height;

    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in =  zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in = zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out = zme_in->zme_common_info.zme_w_out;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out = zme_in->zme_common_info.zme_h_out;
}

static hi_void pq_hal_v0_zme_close_sr_scaler_up(hi_drv_pq_hd_zme_strategy_in *zme_in,
    hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_u32 video_h_out, video_w_out, disp_h, disp_w, refresh_rate;
    hi_bool is_full_screen = HI_TRUE;
    pq_output_mode timming = pq_comm_get_output_mode();

    refresh_rate = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].refresh_rate;
    video_h_out = zme_in->zme_common_info.zme_h_out;
    video_w_out = zme_in->zme_common_info.zme_w_out;
    disp_h = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h;
    disp_w = g_zme_reso_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_w;

    if ((disp_w == video_w_out) && (disp_h == video_h_out)) {
        is_full_screen = HI_TRUE;
    } else {
        is_full_screen = HI_FALSE;
    }

    if ((timming == PQ_OUTPUT_MODE_8K120) && (is_full_screen == HI_TRUE)) {
        pq_hal_8k120fmt_fullscreen_close_sr_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_8K120) && (is_full_screen != HI_TRUE)) {
        pq_hal_8k120fmt_nofullscreen_close_sr_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_8K60) && (is_full_screen == HI_TRUE)) {
        pq_hal_8k60fmt_fullscreen_close_sr_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_8K60) && (is_full_screen != HI_TRUE)) {
        pq_hal_8k60fmt_nofullscreen_close_sr_scaler_up(zme_in, zme_out);
    } else if ((timming == PQ_OUTPUT_MODE_4K) && (refresh_rate > REFRESHRATE_60)) {
        pq_hal_4k120fmt_close_sr_scaler_up(zme_in, zme_out);
    } else {
        pq_hal_4k60fmt_close_sr_scaler_up(zme_in, zme_out);
    }

    return;
}

static hi_void pq_hal_check_equal_ratio(hi_u32 w_in, hi_u32 h_in,
                                        hi_u32 w_out, hi_u32 h_out, hi_bool *is_equal_ratio)
{
    /*  等比判断条件：
    a: (宽度缩小倍数* 1.6 > 高度缩小倍数) 或
    (高度缩小倍数* 2.0 > 宽度缩小倍数) */
    if ((w_in * h_out * 8 > h_in * w_out * 5) || (h_in * w_out * 2 > w_in * h_out)) { /* 2/5/8:multi */
        /* 缩小 */
        *is_equal_ratio = HI_TRUE;
    } else {
        *is_equal_ratio = HI_FALSE;
    }

    return;
}

static hi_void pq_hal_check_zme_out_reso(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                         hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    PQ_CHECK_ZERO_RE_NULL(zme_out->hor_draw_mul);
    PQ_CHECK_ZERO_RE_NULL(zme_out->ver_draw_mul);

   /* if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable == HI_TRUE) {
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_in, PQ_WIDTH_256);
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_w_out, PQ_WIDTH_256);
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_in, PQ_HEIGHT_256);
        PQ_CHECK_NUM_LOW_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_h_out, PQ_HEIGHT_256);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable == HI_TRUE) {
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_in, PQ_WIDTH_4096);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_w_out, PQ_WIDTH_4096);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_in, PQ_HEIGHT_2160);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_h_out, PQ_HEIGHT_2160);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable == HI_TRUE) {
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_in, PQ_WIDTH_1920);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_w_out, PQ_WIDTH_3840);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_in, PQ_HEIGHT_1080);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_h_out, PQ_HEIGHT_2160);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable == HI_TRUE) {
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_in, PQ_WIDTH_3840);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_w_out, PQ_WIDTH_7680);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_in, PQ_HEIGHT_2160);
        PQ_CHECK_NUM_OVER_RANGE_RE_NULL(zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_h_out, PQ_HEIGHT_4320);
    }*/
}

static hi_void pq_hal_v0_norm_zme_reso_calc(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                            hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    hi_bool equal_ratio = HI_TRUE;
    hi_bool scaler_down = HI_TRUE;

    hi_u32 w_in = zme_in->zme_common_info.zme_w_in;
    hi_u32 w_out = zme_in->zme_common_info.zme_w_out;
    hi_u32 h_in = zme_in->zme_common_info.zme_h_in;
    hi_u32 h_out = zme_in->zme_common_info.zme_h_out;

    pq_hal_check_equal_ratio(w_in, h_in, w_out, h_out, &equal_ratio);
    /* 视频输入高度比输出高度大（VIDEO_IH/VIDEO_OH > 1）时，为缩小场景 */
    if (h_in > h_out) {
        scaler_down = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS0;
    } else {
        scaler_down = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_point = HI_DRV_PQ_8KZME_POS1;
    }

    if (scaler_down == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable = HI_TRUE;
        /* 缩小场景下SR不开启 */
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
        if (equal_ratio == HI_TRUE) {
            /* 抽行、抽点 */
            pq_hal_v0_equal_ratio_vpre_zme_config(zme_in, zme_out);
            pq_hal_v0_equal_ratio_hpre_zme_config(zme_in, zme_out);
            pq_hal_v0_vpre_zme_ajust(zme_in, zme_out);
            pq_hal_v0_hpre_zme_ajust(zme_in, zme_out);
            pq_hal_v0_zme_equal_ratio_scaler_down(zme_in, zme_out);
        } else {
            /* 抽行、抽点 */
            pq_hal_v0_unequal_ratio_vpre_zme_config(zme_in, zme_out);
            pq_hal_v0_unequal_ratio_hpre_zme_config(zme_in, zme_out);
            pq_hal_v0_vpre_zme_ajust(zme_in, zme_out);
            pq_hal_v0_hpre_zme_ajust(zme_in, zme_out);
            pq_hal_v0_zme_unequal_ratio_scaler_down(zme_in, zme_out);
        }
    } else {
        /* 放大场景下默认开启SR，在下层策略中会进行校正 */
        zme_out->hor_draw_mul = HI_DRV_PQ_PREZME_HOR_DISABLE;
        zme_out->ver_draw_mul = HI_DRV_PQ_PREZME_VER_DISABLE;
        if (g_pq_sr_enable == HI_TRUE) {
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_TRUE;
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_TRUE;
            pq_hal_v0_zme_scaler_up(zme_in, zme_out);
        } else {
            zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable = HI_FALSE;
            zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable = HI_FALSE;
            pq_hal_v0_zme_close_sr_scaler_up(zme_in, zme_out);
        }
    }

    if ((scaler_down == HI_TRUE) && (w_in < PQ_WIDTH_1920) && (h_in < PQ_HEIGHT_1080)) {
        /* 缩小场景下SR不开启 */
        pq_hal_zme_ajust_lessthan_fhd_scaler_down(zme_in, zme_out);
    }

    g_sg_st_get_pq_proc_vdp_zme_strategy.is_equal_ratio = equal_ratio;

    return;
}

hi_void pq_hal_set_vdp_zme_fir_en(hi_bool zme_fir_en)
{
    g_hal_vdp_zme_fir_en = zme_fir_en;

    return;
}

hi_void pq_hal_set_vdp_zme_med_en(hi_bool zme_mod_en)
{
    g_hal_vdp_zme_med_en = zme_mod_en;

    return;
}

static hi_void pq_hal_set_v0_8kzme_fir_enable(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_hor_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_ver_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_en.zme_h_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_en.zme_hl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_en.zme_hc_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_en.zme_v_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_en.zme_vl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_en.zme_vc_en = HI_TRUE;

    if (g_hal_vdp_zme_fir_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_FIR;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_COPY;
    }

    if (g_hal_vdp_zme_med_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_med_fir_en.zme_med_h_en = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_med_fir_en.zme_med_v_en = HI_TRUE;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_med_fir_en.zme_med_h_en = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_med_fir_en.zme_med_v_en = HI_FALSE;
    }

    return;
}

static hi_void pq_hal_set_v0_4kzme_fir_enable(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_hor_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_ver_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_en.zme_h_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_en.zme_hl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_en.zme_hc_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_en.zme_v_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_en.zme_vl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_en.zme_vc_en = HI_TRUE;

    if (g_hal_vdp_zme_fir_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_FIR;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_COPY;
    }

    if (g_hal_vdp_zme_med_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_med_fir_en.zme_med_h_en = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_med_fir_en.zme_med_v_en = HI_TRUE;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_med_fir_en.zme_med_h_en = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_med_fir_en.zme_med_v_en = HI_FALSE;
    }

    return;
}

static hi_void pq_hal_set_v0_4ksr_fir_enable(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_hor_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_ver_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_en.zme_h_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_en.zme_hl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_en.zme_hc_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_en.zme_v_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_en.zme_vl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_en.zme_vc_en = HI_TRUE;

    if (g_hal_vdp_zme_fir_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_FIR;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_COPY;
    }

    if (g_hal_vdp_zme_med_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_med_fir_en.zme_med_h_en = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_med_fir_en.zme_med_v_en = HI_TRUE;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_med_fir_en.zme_med_h_en = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_med_fir_en.zme_med_v_en = HI_FALSE;
    }

    return;
}

static hi_void pq_hal_set_v0_8ksr_fir_enable(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_hor_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_ver_enable = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_en.zme_h_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_en.zme_hl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_en.zme_hc_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_en.zme_v_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_en.zme_vl_en = HI_TRUE;
    zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_en.zme_vc_en = HI_TRUE;

    if (g_hal_vdp_zme_fir_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_FIR;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_FIR;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_hl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_vl = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_hc = HI_DRV_PQ_ZME_COPY;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_fir_mode.zme_fir_vc = HI_DRV_PQ_ZME_COPY;
    }

    if (g_hal_vdp_zme_med_en == HI_TRUE) {
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_med_fir_en.zme_med_h_en = HI_TRUE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_med_fir_en.zme_med_v_en = HI_TRUE;
    } else {
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_med_fir_en.zme_med_h_en = HI_FALSE;
        zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_med_fir_en.zme_med_v_en = HI_FALSE;
    }

    return;
}

static hi_void pq_hal_v0_all_zme_mode_config(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    if (zme_out->zme_fmt[HI_DRV_PQ_8KZME].zme_enable == HI_TRUE) {
        pq_hal_set_v0_8kzme_fir_enable(zme_out);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_4KZME].zme_enable == HI_TRUE) {
        pq_hal_set_v0_4kzme_fir_enable(zme_out);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_4KSR].zme_enable == HI_TRUE) {
        pq_hal_set_v0_4ksr_fir_enable(zme_out);
    }

    if (zme_out->zme_fmt[HI_DRV_PQ_8KSR].zme_enable == HI_TRUE) {
        pq_hal_set_v0_8ksr_fir_enable(zme_out);
    }

    return;
}

static hi_s32 pq_hal_get_v0_zme_strategy(hi_drv_pq_hd_zme_strategy_in *zme_in,
                                         hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_out);
    drv_pq_get_pq_status(&g_zme_reso_status);
    pq_mng_v0_zme_reso_check(zme_in);
    if (zme_in->hdcp_factor.is_hdcp_scence != HI_TRUE) {
        pq_hal_v0_norm_zme_reso_calc(zme_in, zme_out);
    } else {
        pq_hal_v0_zme_hdcp_closr_sr_scaler(zme_in, zme_out);
    }

    pq_hal_reset_all_zme_reso(zme_in, zme_out);
    pq_hal_check_zme_out_reso(zme_in, zme_out);
    pq_hal_set_all_zme_fmt(zme_in, zme_out);
    pq_hal_set_all_zme_frame_fmt(zme_in, zme_out);
    pq_hal_v0_all_zme_mode_config(zme_out);
    pq_hal_set_v0_all_zme_offset(zme_in, zme_out);
    pq_hal_adjust_zme_fir_en_cfg(zme_in, zme_out);

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                   hi_drv_pq_zme_strategy_out *zme_out)
{
    hi_s32 ret = HI_SUCCESS;

    if (HI_DRV_PQ_DISP_V0_LAYER_ZME == layer_id) {
        ret = pq_hal_get_v0_zme_strategy(&zme_in->hd_zme_strategy, &(zme_out->hd_zme_strategy));
    } else {
        HI_ERR_PQ("layer_id:%d, unsupport zme strategy!\n", layer_id);
    }

    g_sg_st_get_pq_proc_vdp_zme_strategy.layer_id = layer_id;
    memcpy(&g_sg_st_get_pq_proc_vdp_zme_strategy.zme_in, zme_in,
           sizeof(g_sg_st_get_pq_proc_vdp_zme_strategy.zme_in));
    memcpy(&g_sg_st_get_pq_proc_vdp_zme_strategy.zme_out, zme_out,
           sizeof(g_sg_st_get_pq_proc_vdp_zme_strategy.zme_out));

    return ret;
}

hi_s32 pq_hal_get_v0_zme_coef_addr(hi_drv_pq_hd_zme_strategy_out *zme_out)
{
    pq_hal_zme_coef_ratio zme_4k_ratio_h;
    pq_hal_zme_coef_ratio zme_4k_ratio_v;
    pq_hal_zme_coef_ratio zme_8k_ratio_h;
    pq_hal_zme_coef_ratio zme_8k_ratio_v;
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(zme_out);

    ret = pq_hal_get_hzme_ratio(&zme_out->zme_fmt[HI_DRV_PQ_4KZME],
                                &zme_4k_ratio_h);
    ret = pq_hal_get_vzme_ratio(&zme_out->zme_fmt[HI_DRV_PQ_4KZME],
                                &zme_4k_ratio_v);

    ret = pq_hal_get_hzme_ratio(&zme_out->zme_fmt[HI_DRV_PQ_8KZME],
                                &zme_8k_ratio_h);
    ret = pq_hal_get_vzme_ratio(&zme_out->zme_fmt[HI_DRV_PQ_8KZME],
                                &zme_8k_ratio_v);

    zme_out->zme_coef_addr.pq_8kzme_coef_addr_h =
        g_pq_vdp_zme_coef_buf[zme_8k_ratio_h].phy_addr[PQ_ZME_COEF_BUF_8K_HZME];
    zme_out->zme_coef_addr.pq_8kzme_coef_addr_v =
        g_pq_vdp_zme_coef_buf[zme_8k_ratio_v].phy_addr[PQ_ZME_COEF_BUF_8K_VZME];

    zme_out->zme_coef_addr.pq_4kzme_coef_addr_h =
        g_pq_vdp_zme_coef_buf[zme_4k_ratio_h].phy_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_H];
    zme_out->zme_coef_addr.pq_4kzme_coef_addr_v =
        g_pq_vdp_zme_coef_buf[zme_4k_ratio_v].phy_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_V];

    zme_out->zme_coef_addr.pq_4ksr0_coef_addr =
        g_pq_vdp_zme_coef_buf[PQ_HAL_ZME_COEF_1].phy_addr[PQ_ZME_COEF_BUF_4K_SR0];
    zme_out->zme_coef_addr.pq_8ksr0_coef_addr =
        g_pq_vdp_zme_coef_buf[PQ_HAL_ZME_COEF_1].phy_addr[PQ_ZME_COEF_BUF_8K_SR0];

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_zme_coef_addr(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                hi_drv_pq_zme_strategy_out *zme_out)
{
    hi_s32 ret = HI_SUCCESS;

    if (HI_DRV_PQ_DISP_V0_LAYER_ZME == layer_id) {
        ret = pq_hal_get_v0_zme_coef_addr(&(zme_out->hd_zme_strategy));
    } else {
        HI_ERR_PQ("layer_id:%d, unsupport zme strategy!\n", layer_id);
    }

    return ret;
}

hi_s32 pq_hal_set_vdp_hdcpen(hi_bool on_off)
{
    g_sg_st_set_pq_proc_vdp_zme_strategy.proc_hdcp_en = on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_proc_vdp_zme_strategy(hi_pq_proc_vdpzme_strategy proc_vdp_zme_strategy)
{
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(proc_vdp_zme_strategy.proc_vdp_zme_num, HI_PQ_PROC_ZME_NUM_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(proc_vdp_zme_strategy.proc_vdp_width_draw_mode, HI_PQ_PROC_PREZME_HOR_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(proc_vdp_zme_strategy.proc_vdp_height_draw_mode, HI_PQ_PROC_PREZME_VER_MAX);

    /* hpzme do not support set by proc */
    g_sg_st_set_pq_proc_vdp_zme_strategy.proc_vdp_zme_num = proc_vdp_zme_strategy.proc_vdp_zme_num;
    g_sg_st_set_pq_proc_vdp_zme_strategy.proc_vdp_width_draw_mode = proc_vdp_zme_strategy.proc_vdp_width_draw_mode;
    g_sg_st_set_pq_proc_vdp_zme_strategy.proc_vdp_height_draw_mode = proc_vdp_zme_strategy.proc_vdp_height_draw_mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_proc_vdp_zme_strategy(hi_pq_proc_get_vdpzme_strategy *proc_vdp_zme_strategy)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(proc_vdp_zme_strategy);

    PQ_SAFE_MEMCPY(proc_vdp_zme_strategy, &g_sg_st_get_pq_proc_vdp_zme_strategy,
                   sizeof(g_sg_st_get_pq_proc_vdp_zme_strategy));

    return HI_SUCCESS;
}

hi_s32 pq_hal_get_proc_vdp_zme_reso(hi_pq_proc_vdpzme_reso *proc_vdp_zme_reso)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(proc_vdp_zme_reso);

    PQ_SAFE_MEMCPY(proc_vdp_zme_reso, &g_sg_st_proc_vdp_zme_reso, sizeof(g_sg_st_proc_vdp_zme_reso));

    return HI_SUCCESS;
}
