/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dering
 * Author: pq
 * Create: 2019-11-7
 */

#include "pq_hal_comm.h"
#include "pq_hal_dering.h"
#include "drv_pq_table.h"

hi_s32 pq_hal_set_dering_enable(hi_u32 handle_no, hi_bool enable)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg0.bits.dr_en = enable;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dering_demo_enable(hi_u32 handle_no, hi_bool demo_en)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_en = demo_en;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dering_demo_mode(hi_u32 handle_no, pq_demo_mode mode)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_mode = mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_dering_demo_coor(hi_u32 handle_no, hi_u32 x_pos)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_wx = x_pos;

    return HI_SUCCESS;
}


