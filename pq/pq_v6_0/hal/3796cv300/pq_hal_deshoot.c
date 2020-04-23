/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: deshoot
 * Author: pq
 * Create: 2019-11-4
 */

#include "pq_hal_comm.h"
#include "pq_hal_deshoot.h"
#include "pq_mng_deshoot.h"
#include "drv_pq_table.h"

hi_s32 pq_hal_enable_deshoot(hi_u32 handle_no, hi_bool ds_on_off)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg0.bits.ds_en = ds_on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_deshoot_set_strength(hi_s32 handle_no, hi_u32 blendgainov, hi_u32 blendgainud)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg68.bits.ds_blendgainov = blendgainov;
    vpss_reg->vpss_hipp_snr_reg72.bits.ds_blendgainud = blendgainud;

    return HI_SUCCESS;
}

hi_s32 pq_hal_enable_deshoot_demo(hi_u32 handle_no, hi_bool ds_demo_en)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_en = ds_demo_en;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_deshoot_demo_mode(hi_u32 handle_no, pq_demo_mode mode)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_mode = mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_deshoot_demo_coor(hi_u32 handle_no, hi_u32 x_pos)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_wx = x_pos;

    return HI_SUCCESS;
}
