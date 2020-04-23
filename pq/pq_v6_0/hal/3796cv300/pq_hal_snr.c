/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal snr api
 * Author: pq
 * Create: 2016-01-1
 */

#include "drv_pq_table.h"
#include "pq_hal_snr.h"
#include "pq_hal_comm.h"

hi_s32 pq_hal_enable_snr(hi_u32 handle_no, hi_bool snr_on_off)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    vpss_reg->vpss_hipp_snr_reg0.bits.snr_en_3 = snr_on_off;

    return HI_SUCCESS;
}

hi_s32 pq_hal_enable_snr_demo(hi_u32 handle_no, hi_bool snr_demo_en)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_en = snr_demo_en;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_snr_demo_mode(hi_u32 handle_no, snr_demo_mode mode)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_mode = mode;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_snr_demo_mode_coor(hi_u32 handle_no, hi_u32 x_pos)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg1.bits.demo_wx = x_pos;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_snr_str(hi_u32 handle_no, hi_u32 snr_str)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);

    vpss_reg->vpss_hipp_snr_reg123.bits.vsnr_str = snr_str;
    vpss_reg->vpss_hipp_snr_reg123.bits.y_vsnr_str = snr_str;
    vpss_reg->vpss_hipp_snr_reg126.bits.hsnr_str = snr_str;
    vpss_reg->vpss_hipp_snr_reg126.bits.y_hsnr_str = snr_str;

    return HI_SUCCESS;
}

/**************************************************************************/
static hi_s32 pq_hal_get_snr_input_reg(drv_pq_snr_input_reg *snr_input_reg)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(snr_input_reg);

    snr_input_reg->ar_motion_alpha_max = 32; /* 32: motion alpha max */
    snr_input_reg->ar_sdi_str = 16; /* 16: ar_sdi_str */

    return HI_SUCCESS;
}

hi_s32 pq_hal_update_snr_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(nr_api_input);

    ret = pq_hal_get_snr_input_reg(&(nr_api_input->snr_input_reg));

    return ret;
}

hi_s32 pq_hal_update_snr_cfg(hi_u32 handle, hi_drv_pq_nr_api_output_reg *nr_out_reg)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(nr_out_reg);

    vpss_reg->vpss_hipp_snr_reg126.bits.motion_alpha = nr_out_reg->snr_out_reg.motion_alpha;
    vpss_reg->vpss_hipp_snr_reg307.bits.scene_change_info = nr_out_reg->snr_out_reg.scene_change_info;
    vpss_reg->vpss_hipp_snr_reg308.bits.sdi_str = nr_out_reg->snr_out_reg.sdi_str;

    return HI_SUCCESS;
}
