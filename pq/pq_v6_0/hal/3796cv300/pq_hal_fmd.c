/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal fmd api define
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_hal_fmd.h"

hi_s32 pq_hal_set_ifmd_vir_reg(hi_void)
{
    hi_u32 i = 0;
    hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(vir_soft_reg);

    vir_soft_reg->ifmd_reg.mode32_en = 1;
    vir_soft_reg->ifmd_reg.mode2332_en = 1;
    vir_soft_reg->ifmd_reg.mode32322_en = 1;
    vir_soft_reg->ifmd_reg.mode11_2_3_en = 1;
    vir_soft_reg->ifmd_reg.mode22_en = 1;
    vir_soft_reg->ifmd_reg.mode55_64_87_en = 1;

    vir_soft_reg->ifmd_reg.ifmd_debug_en = 0;
    for (i = 0; i < 3; i++) { /* 3: ifmd debug num */
        vir_soft_reg->ifmd_reg.ifmd_debug32_en[i] = 0;
        vir_soft_reg->ifmd_reg.ifmd_debug2332_en[i] = 0;
        vir_soft_reg->ifmd_reg.ifmd_debug32322_en[i] = 0;
        vir_soft_reg->ifmd_reg.ifmd_debug11_2_3_en[i] = 0;
        vir_soft_reg->ifmd_reg.ifmd_debug22_en[i] = 0;
        vir_soft_reg->ifmd_reg.ifmd_debug55_64_87_en[i] = 0;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_ifmd_debug_en(hi_bool enable)
{
    hi_u32 i = 0;
    hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(vir_soft_reg);

    vir_soft_reg->ifmd_reg.ifmd_debug_en = enable;
    for (i = 0; i < 3; i++) { /* 3: ifmd debug num */
        vir_soft_reg->ifmd_reg.ifmd_debug32_en[i] = enable;
        vir_soft_reg->ifmd_reg.ifmd_debug2332_en[i] = enable;
        vir_soft_reg->ifmd_reg.ifmd_debug32322_en[i] = enable;
        vir_soft_reg->ifmd_reg.ifmd_debug11_2_3_en[i] = enable;
        vir_soft_reg->ifmd_reg.ifmd_debug22_en[i] = enable;
        vir_soft_reg->ifmd_reg.ifmd_debug55_64_87_en[i] = enable;
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_enable_ifmd(hi_bool enable)
{
    hi_drv_pq_dbg_vpss_vir_soft *vir_soft_reg = pq_hal_get_vpss_vir_soft_reg();

    vir_soft_reg->ifmd_reg.mode32_en = enable;
    vir_soft_reg->ifmd_reg.mode2332_en = enable;
    vir_soft_reg->ifmd_reg.mode32322_en = enable;
    vir_soft_reg->ifmd_reg.mode11_2_3_en = enable;
    vir_soft_reg->ifmd_reg.mode22_en = enable;
    vir_soft_reg->ifmd_reg.mode55_64_87_en = enable;

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_bt_mode(hi_u32 handle_no, hi_s32 fieldorder)
{
    vpss_reg_type *vpss_reg = pq_hal_get_vpss_reg(handle_no);

    vpss_reg->vpss_hipp_pdctrl.bits.bt_mode = fieldorder;

    return HI_SUCCESS;
}

hi_s32 pq_hal_update_ifmd_api_reg(hi_u32 handle_no, hi_drv_pq_ifmd_playback *sp_ifmd_result,
                                  hi_drv_pq_vpss_stt_info *info_in)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(sp_ifmd_result);

    vpss_reg->vpss_hipp_dei_ctrl.bits.dei_out_sel_c = sp_ifmd_result->die_out_sel;
    vpss_reg->vpss_hipp_dei_ctrl.bits.dei_out_sel_l = sp_ifmd_result->die_out_sel;

    vpss_reg->vpss_hipp_dei_ctrl.bits.mchdir_c = sp_ifmd_result->dir_mch;
    vpss_reg->vpss_hipp_dei_ctrl.bits.mchdir_l = sp_ifmd_result->dir_mch;
    if (info_in->width > PQ_WIDTH_1280)
        vpss_reg->vpss_hipp_mc_film4.bits.err_line_th = 8; /* 8: err line */

    return HI_SUCCESS;
}

hi_s32 pq_hal_ifmd_white_block_ctrl(hi_u32 handle_no, hi_drv_pq_ifmd_playback *sp_ifmd_result)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    vpss_reg = pq_hal_get_vpss_reg(handle_no);

    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(sp_ifmd_result);

    if (sp_ifmd_result->die_out_sel == 1) {
        if (sp_ifmd_result->film_mode > 800) { /* 800: film_mode */
            vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 0;
        } else if (sp_ifmd_result->film_mode < 10) { /* 10: film_mode */
            vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 1;
        } else if (sp_ifmd_result->film_mode >= 100 && sp_ifmd_result->film_mode < 200) { /* 200/100: film_mode */
            vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 2; /* 2: film_mode */
        } else if (sp_ifmd_result->film_mode >= 200 && sp_ifmd_result->film_mode < 300) { /* 300/200: film_mode */
            vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 3; /* 3: film_mode */
        } else if (sp_ifmd_result->film_mode >= 300 && sp_ifmd_result->film_mode < 400) { /* 400/300: film_mode */
            vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 4; /* 4: film_mode */
        } else if (sp_ifmd_result->film_mode >= 500 && sp_ifmd_result->film_mode < 600) { /* 600/500: film_mode */
            vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 5; /* 5: film_mode */
        } else {
            vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 9; /* 9: film_mode */
        }
    } else {
        vpss_reg->vpss_hipp_dei_ctrl.bits.film_mode = 9; /* 9: film_mode */
    }

    return HI_SUCCESS;
}
