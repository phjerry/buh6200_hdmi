/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq hal acm
 * Author: pq
 * Create: 2019-10-26
 */

#include <linux/string.h>

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_mem.h"
#include "pq_hal_comm.h"
#include "pq_hal_acm.h"

static vdp_regs_type *g_vdp_acm_reg = HI_NULL;
static pq_acm_coef_addr g_acm_hal_coef_buf = { 0 };

#define ACM_HAL_SPLIT_SHIFT 128
#define YHS_GAIN_SAT_SHIFT  9
#define YHS_GAIN_HUE_SHIFT  18
#define ACM_HAL_LUT_EVEN    2

hi_s32 pq_hal_acm_reset_addr(hi_void)
{
    g_vdp_acm_reg = HI_NULL;
    memset(&g_acm_hal_coef_buf, 0x0, sizeof(pq_acm_coef_addr));
    return HI_SUCCESS;
}

hi_s32 pq_hal_set_acm_link(vdp_regs_type *vdp_reg, pq_acm_link bind)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = pq_new_regread((uintptr_t)&(vdp_reg->v0_ctrl.u32));
    v0_ctrl.bits.acm_link = bind;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);
    return HI_SUCCESS;
}

hi_s32 pq_hal_set_acm_enable(vdp_regs_type *vdp_reg, hi_bool on_off)
{
    u_vp0_acm_ctrl acm_ctrl;

    acm_ctrl.u32 = pq_new_regread((uintptr_t)&(vdp_reg->vp0_acm_ctrl.u32));
    acm_ctrl.bits.acm_en = on_off;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vp0_acm_ctrl.u32)), acm_ctrl.u32);
    return HI_SUCCESS;
}

hi_s32 pq_hal_set_acm_cbcr_threshold(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_vp0_acm_ctrl acm_ctrl;

    acm_ctrl.u32 = pq_new_regread((uintptr_t)&(vdp_reg->vp0_acm_ctrl.u32));
    acm_ctrl.bits.acm_cbcrthr = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vp0_acm_ctrl.u32)), acm_ctrl.u32);

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_acm_stretch(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_vp0_acm_ctrl acm_ctrl;

    acm_ctrl.u32 = pq_new_regread((uintptr_t)&(vdp_reg->vp0_acm_ctrl.u32));
    acm_ctrl.bits.acm_stretch = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vp0_acm_ctrl.u32)), acm_ctrl.u32);

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_acm_clip_range(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_vp0_acm_ctrl acm_ctrl;

    acm_ctrl.u32 = pq_new_regread((uintptr_t)&(vdp_reg->vp0_acm_ctrl.u32));
    acm_ctrl.bits.acm_cliprange = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vp0_acm_ctrl.u32)), acm_ctrl.u32);

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_acm_gain(vdp_regs_type *vdp_reg, hi_u32 luma, hi_u32 hue, hi_u32 sat)
{
    u_vp0_acm_adj  acm_adj;

    PQ_CHECK_UPPER_LIMIT(luma, MAX_ACM_GAIN);
    PQ_CHECK_UPPER_LIMIT(hue, MAX_ACM_GAIN);
    PQ_CHECK_UPPER_LIMIT(sat, MAX_ACM_GAIN);

    acm_adj.u32 = pq_new_regread((uintptr_t)&(vdp_reg->vp0_acm_adj.u32));
    acm_adj.bits.acm_gain0 = luma;
    acm_adj.bits.acm_gain1 = hue;
    acm_adj.bits.acm_gain2 = sat;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vp0_acm_adj.u32)), acm_adj.u32);

    return HI_SUCCESS;
}

hi_s32 pq_hal_acm_set_demo_en(hi_bool on_off)
{
    u_vp0_acm_ctrl acm_ctrl;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_acm_reg);

    acm_ctrl.u32 = pq_new_regread((uintptr_t)&(g_vdp_acm_reg->vp0_acm_ctrl.u32));
    acm_ctrl.bits.acm_dbg_en = on_off;
    pq_new_regwrite((uintptr_t)(&(g_vdp_acm_reg->vp0_acm_ctrl.u32)), acm_ctrl.u32);

    return HI_SUCCESS;
}

hi_s32 pq_hal_acm_set_demo_mode(pq_demo_mode mode)
{
    u_vp0_acm_ctrl acm_ctrl;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_acm_reg);

    acm_ctrl.u32 = pq_new_regread((uintptr_t)&(g_vdp_acm_reg->vp0_acm_ctrl.u32));
    acm_ctrl.bits.acm_dbg_mode = mode;
    pq_new_regwrite((uintptr_t)(&(g_vdp_acm_reg->vp0_acm_ctrl.u32)), acm_ctrl.u32);

    return HI_SUCCESS;
}

hi_s32 pq_hal_acm_set_demo_pos(hi_u32 x_pos)
{

    u_vp0_acm_ctrl acm_ctrl;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_acm_reg);

    acm_ctrl.u32 = pq_new_regread((uintptr_t)&(g_vdp_acm_reg->vp0_acm_ctrl.u32));
    acm_ctrl.bits.acm_dbg_pos = x_pos;
    pq_new_regwrite((uintptr_t)(&(g_vdp_acm_reg->vp0_acm_ctrl.u32)), acm_ctrl.u32);

    return HI_SUCCESS;
}

static hi_void pq_hal_set_acm_addr(vdp_regs_type* vdp_reg, hi_u32 coef_addr)
{
    u_para_addr_v0_chn26 para_addr_v0_chn26;

    para_addr_v0_chn26.u32 = pq_new_regread((uintptr_t)&(vdp_reg->para_addr_v0_chn26.u32));
    para_addr_v0_chn26.bits.para_addr_v0_chn26 = coef_addr;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn26.u32)), para_addr_v0_chn26.u32);

    return;
}

static hi_void pq_hal_update_acm_coef(vdp_regs_type* vdp_reg, hi_bool enable)
{
    u_para_up_v0 para_up_v0;

    para_up_v0.u32 = pq_new_regread((uintptr_t)&(vdp_reg->para_up_v0.u32));
    para_up_v0.bits.para_up_v0_chn26 = enable;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_up_v0.u32)), para_up_v0.u32);

    return;
}

static hi_void pq_hal_acm_split_lut(acm_effect_param *acm_2d_lut, acm_split_lut *acm_split_lut)
{
    hi_u32 i, j;

    /* 拼入主表到S附表 */
    for (i = 0; i < COLOR_H_NUM; i++) {
        for (j = 0; j < COLOR_S_NUM; j++) {
            acm_split_lut->yhs_by_hs[PQ_COLOR_YHS_Y][i][j] =
                (acm_2d_lut->yhs_by_hs_gain[PQ_COLOR_YHS_Y][i][j] *
                acm_2d_lut->yhs_by_h[PQ_COLOR_YHS_Y][i]) /
                ACM_HAL_SPLIT_SHIFT;

            acm_split_lut->yhs_by_hs[PQ_COLOR_YHS_H][i][j] =
                (acm_2d_lut->yhs_by_hs_gain[PQ_COLOR_YHS_H][i][j] *
                acm_2d_lut->yhs_by_h[PQ_COLOR_YHS_H][i]) /
                ACM_HAL_SPLIT_SHIFT;
            acm_split_lut->yhs_by_hs[PQ_COLOR_YHS_S][i][j] =
                (acm_2d_lut->yhs_by_hs_gain[PQ_COLOR_YHS_S][i][j] *
                acm_2d_lut->yhs_by_h[PQ_COLOR_YHS_S][i]) /
                ACM_HAL_SPLIT_SHIFT;
        }
    }

    /* 赋值Y附表 */
    for (i = 0; i < COLOR_H_NUM; i++) {
        for (j = 0; j < COLOR_Y_NUM; j++) {
            acm_split_lut->yhs_by_hy[PQ_COLOR_YHS_Y][i][j] =
                acm_2d_lut->yhs_by_hy_gain[PQ_COLOR_YHS_Y][i][j];
            acm_split_lut->yhs_by_hy[PQ_COLOR_YHS_H][i][j] =
                acm_2d_lut->yhs_by_hy_gain[PQ_COLOR_YHS_H][i][j];
            acm_split_lut->yhs_by_hy[PQ_COLOR_YHS_S][i][j] =
                acm_2d_lut->yhs_by_hy_gain[PQ_COLOR_YHS_S][i][j];
        }
    }

    return;
}

/* 拼入yhs_gain数据 */
static inline hi_void pq_hal_acm_put_yhs_gain_into_u32(hi_s32 luma, hi_s32 hue, hi_s32 sat, hi_u32 *bit)
{
    *bit = ((hi_u32)luma & 0x1FF) | (((hi_u32)sat & 0x1FF) << YHS_GAIN_SAT_SHIFT) |
           (((hi_u32)hue & 0x1FF) << YHS_GAIN_HUE_SHIFT);
}
static hi_void pq_hal_acm_convert_s_table(acm_split_lut *split_table, acm_convert_table *convert_table)
{
    hi_u32 i = 0;
    hi_u32 j = 0;
    /* 逻辑按照需求拆分8个表 */
    hi_u32 m = 0;
    hi_u32 n = 0;
    hi_u32 p = 0;
    hi_u32 q = 0;

    /* 拼入S的三个附表 */
    for (j = 0; j < COLOR_S_NUM; j++) {
        for (i = 0; i < COLOR_H_NUM; i++) {
            if ((i % ACM_HAL_LUT_EVEN == 0) && (j % ACM_HAL_LUT_EVEN == 0)) {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hs[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hs_lut_a[m]);
                m++;
            } else if ((i % ACM_HAL_LUT_EVEN != 0) && (j % ACM_HAL_LUT_EVEN == 0)) {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hs[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hs_lut_b[n]);
                n++;
            } else if ((i % ACM_HAL_LUT_EVEN == 0) && (j % ACM_HAL_LUT_EVEN != 0)) {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hs[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hs_lut_c[p]);
                p++;
            } else {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hs[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hs[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hs_lut_d[q]);
                q++;
            }
        }
    }
}

static hi_void pq_hal_acm_convert_y_table(acm_split_lut *split_table, acm_convert_table *convert_table)
{
    hi_u32 i = 0;
    hi_u32 j = 0;
    /* 逻辑按照需求拆分8个表 */
    hi_u32 m1 = 0;
    hi_u32 n1 = 0;
    hi_u32 p1 = 0;
    hi_u32 q1 = 0;

    /* 拼入Y的三个附表 */
    for (j = 0; j < COLOR_Y_NUM; j++) {
        for (i = 0; i < COLOR_H_NUM; i++) {
            if ((i % ACM_HAL_LUT_EVEN == 0) && (j % ACM_HAL_LUT_EVEN == 0)) {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hy[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hy_lut_a[m1]);
                m1++;
            } else if ((i % ACM_HAL_LUT_EVEN != 0) && (j % ACM_HAL_LUT_EVEN == 0)) {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hy[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hy_lut_b[n1]);
                n1++;
            } else if ((i % ACM_HAL_LUT_EVEN == 0) && (j % ACM_HAL_LUT_EVEN != 0)) {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hy[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hy_lut_c[p1]);
                p1++;
            } else {
                pq_hal_acm_put_yhs_gain_into_u32(split_table->yhs_by_hy[PQ_COLOR_YHS_Y][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_H][i][j],
                                                 split_table->yhs_by_hy[PQ_COLOR_YHS_S][i][j],
                                                 &convert_table->yhs_hy_lut_d[q1]);
                q1++;
            }
        }
    }

    return;
}

static hi_s32 pq_hal_acm_convert_table(acm_split_lut *split_table, acm_convert_table *convert_table)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(split_table);
    PQ_CHECK_NULL_PTR_RE_FAIL(convert_table);

    memset(convert_table, 0, sizeof(acm_convert_table));

    pq_hal_acm_convert_s_table(split_table, convert_table);
    pq_hal_acm_convert_y_table(split_table, convert_table);

    return HI_SUCCESS;
}

hi_s32 pq_hal_acm_distribute_addr(hi_u8 *vir_addr, dma_addr_t phy_addr)
{
    if (vir_addr == HI_NULL || phy_addr == HI_NULL) {
        return HI_FAILURE;
    }

    g_vdp_acm_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_acm_reg);

    g_acm_hal_coef_buf.vir_addr = vir_addr;
    g_acm_hal_coef_buf.phy_addr = phy_addr;

    return HI_SUCCESS;
}
hi_s32 pq_hal_acm_config_coef(acm_effect_param *color_param, acm_convert_table *convert_table)
{
    hi_s32 ret;
    vdp_regs_type* vdp_reg = HI_NULL;

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    if (g_acm_hal_coef_buf.vir_addr == HI_NULL) {
        HI_ERR_PQ("g_acm_hal_coef_buf.vir_addr is NULL\n");
        return HI_FAILURE;
    }

    memcpy((hi_u8*)g_acm_hal_coef_buf.vir_addr, convert_table, sizeof(acm_convert_table));
    pq_hal_set_acm_addr(vdp_reg, g_acm_hal_coef_buf.phy_addr);
    pq_hal_update_acm_coef(vdp_reg, HI_TRUE);

    ret = pq_hal_set_acm_gain(vdp_reg, color_param->luma_gain, color_param->hue_gain, color_param->sat_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_set_acm_gain failure\n");

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_acm_set_param(acm_effect_param *color_param)
{
    hi_s32 ret;

    acm_convert_table *convert_table = HI_NULL;
    acm_split_lut *split_table = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(color_param);

    convert_table = (acm_convert_table*)pq_kmalloc(sizeof(acm_convert_table));
    if (convert_table == HI_NULL) {
        HI_ERR_PQ("alloc acm convert table mem is failed!\n");
        return HI_FAILURE;
    }

    split_table = (acm_split_lut*)pq_kmalloc(sizeof(acm_split_lut));
    if (split_table == HI_NULL) {
        pq_kfree_safe(convert_table);
        HI_ERR_PQ("acm_mix_lut is invaild!\n");
        return HI_FAILURE;
    }
    memset(convert_table, 0, sizeof(acm_convert_table));
    memset(split_table, 0, sizeof(acm_split_lut));

    /* get mix table */
    pq_hal_acm_split_lut(color_param, split_table);

    /* convert */
    ret = pq_hal_acm_convert_table(split_table, convert_table);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("call pq_hal_acm_convert_table failed\n");
        goto FREE_BUFF_AND_RETURN;
    }

    ret = pq_hal_acm_config_coef(color_param, convert_table);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_acm_config_coef failure\n");
        goto FREE_BUFF_AND_RETURN;
    }
    pq_kfree_safe(convert_table);
    pq_kfree_safe(split_table);

    return HI_SUCCESS;

FREE_BUFF_AND_RETURN:
    pq_kfree_safe(convert_table);
    pq_kfree_safe(split_table);

    return HI_FAILURE;
}

