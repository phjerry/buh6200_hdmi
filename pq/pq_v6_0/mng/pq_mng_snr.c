/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq mng snr api
 * Author: pq
 * Create: 2016-01-1
 */

#include "hi_drv_mem.h"
#include "drv_pq_table.h"
#include "pq_hal_comm.h"
#include "pq_mng_snr.h"

static alg_snr g_sg_st_pqsnr_info;

hi_s32 pq_mng_init_snr(pq_bin_param *pq_param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    hi_u32 handle_no = 0;

    if (g_sg_st_pqsnr_info.init == HI_TRUE) {
        return HI_SUCCESS;
    }

    g_sg_st_pqsnr_info.enable = HI_TRUE;
    g_sg_st_pqsnr_info.strength = PQ_ALG_DEFAULT_VALUE;
    g_sg_st_pqsnr_info.demo_enable = HI_FALSE;
    g_sg_st_pqsnr_info.demo_mode = PQ_DEMO_ENABLE_R;

    ret = pq_table_init_phy_list(handle_no, HI_PQ_MODULE_SNR, PQ_SOURCE_MODE_NO, PQ_OUTPUT_MODE_NO);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("SNR init_phy_list error\n");
        g_sg_st_pqsnr_info.init = HI_FALSE;

        return HI_FAILURE;
    }

    g_sg_st_pqsnr_info.init = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_deinit_snr(hi_void)
{
    if (g_sg_st_pqsnr_info.init == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_sg_st_pqsnr_info.init = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_snr_en(hi_bool on_off)
{
    hi_u32 handle_no;
    hi_u32 vid = PQ_VDP_LAYER_VID0;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqsnr_info.init);

    if ((source_mode > PQ_SOURCE_MODE_FHD)
        || (HI_FALSE == pq_comm_get_moudle_ctrl(HI_PQ_MODULE_SNR, source_mode, output_mode))) {
        on_off = HI_FALSE;
    }

    g_sg_st_pqsnr_info.enable = on_off;
    if (REG_TYPE_VPSS == pq_comm_get_alg_type_id(HI_PQ_MODULE_SNR)) {
        for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
            if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
                pq_hal_enable_snr(handle_no, on_off);
            }
        }
    } else {
        pq_hal_enable_snr(vid, on_off);
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_snr_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_sg_st_pqsnr_info.enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_snr_demo_en(hi_bool on_off)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;
    hi_u32 vid = PQ_VDP_LAYER_VID0;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqsnr_info.init);

    g_sg_st_pqsnr_info.demo_enable = on_off;

    if (REG_TYPE_VPSS == pq_comm_get_alg_type_id(HI_PQ_MODULE_SNR)) {
        for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
            if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
                ret = pq_hal_enable_snr_demo(handle_no, on_off);
            }
        }
    } else {
        ret = pq_hal_enable_snr_demo(vid, on_off);
    }

    return ret;
}

hi_s32 pq_mng_get_snr_demo_en(hi_bool *on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    *on_off = g_sg_st_pqsnr_info.demo_enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_snr_demo_mode(pq_demo_mode mode)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;
    hi_u32 vid = PQ_VDP_LAYER_VID0;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqsnr_info.init);

    g_sg_st_pqsnr_info.demo_mode = mode;

    if (REG_TYPE_VPSS == pq_comm_get_alg_type_id(HI_PQ_MODULE_SNR)) {
        for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
            if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
                ret = pq_hal_set_snr_demo_mode(handle_no, (snr_demo_mode)mode);
            }
        }
    } else {
        ret = pq_hal_set_snr_demo_mode(vid, (snr_demo_mode)mode);
    }

    return ret;
}

hi_s32 pq_mng_get_snr_demo_mode(pq_demo_mode *pen_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(pen_mode);

    *pen_mode = g_sg_st_pqsnr_info.demo_mode;

    return HI_SUCCESS;
}

/* set snr strengh; snr_str  snr strengh[0,100] */
hi_s32 pq_mng_set_snr_strength(hi_u32 snr_str)
{
    hi_u32 handle_no;
    hi_s32 ret = HI_SUCCESS;
    hi_u32 vid = PQ_VDP_LAYER_VID0;

    if (snr_str > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("the value[%u] of setting SNR strength is out of range!\n", snr_str);
        return HI_FAILURE;
    }

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqsnr_info.init);

    g_sg_st_pqsnr_info.strength = snr_str;

    if (REG_TYPE_VPSS == pq_comm_get_alg_type_id(HI_PQ_MODULE_SNR)) {
        for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
            if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
                ret = pq_hal_set_snr_str(handle_no, snr_str);
            }
        }
    } else {
        ret = pq_hal_set_snr_str(vid, snr_str);
    }

    return ret;
}

/* get snr strengh ; snr_str strengh[0,100] */
hi_s32 pq_mng_get_snr_strength(hi_u32 *snr_str)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(snr_str);

    *snr_str = g_sg_st_pqsnr_info.strength;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_snr_demo_mode_coor(hi_u32 x_pos)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 handle_no;
    hi_u32 vid = PQ_VDP_LAYER_VID0;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqsnr_info.init);

    if (REG_TYPE_VPSS == pq_comm_get_alg_type_id(HI_PQ_MODULE_SNR)) {
        for (handle_no = 0; handle_no < VPSS_HANDLE_NUM; handle_no++) {
            if (HI_SUCCESS == pq_hal_check_vpss_valid(handle_no)) {
                ret = pq_hal_set_snr_demo_mode_coor(handle_no, x_pos);
            }
        }
    } else {
        ret = pq_hal_set_snr_demo_mode_coor(vid, x_pos);
    }

    return ret;
}

hi_s32 pq_mng_update_snr_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqsnr_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);

    ret = pq_hal_update_snr_stt_info(vpss_stt_info, nr_api_input);

    return ret;
}

hi_s32 pq_mng_update_snr_cfg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_api_output_reg *nr_output_reg)
{
    hi_s32 ret;

    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pqsnr_info.init);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_stt_info);

    ret = pq_hal_update_snr_cfg(vpss_stt_info->handle_id, nr_output_reg);

    return ret;
}

static pq_alg_funcs g_snr_funcs = {
    .init = pq_mng_init_snr,
    .deinit = pq_mng_deinit_snr,
    .set_enable = pq_mng_set_snr_en,
    .get_enable = pq_mng_get_snr_en,
    .set_demo = pq_mng_set_snr_demo_en,
    .get_demo = pq_mng_get_snr_demo_en,
    .set_demo_mode = pq_mng_set_snr_demo_mode,
    .get_demo_mode = pq_mng_get_snr_demo_mode,
    .set_strength = pq_mng_set_snr_strength,
    .get_strength = pq_mng_get_snr_strength,
    .set_demo_mode_coor = pq_mng_set_snr_demo_mode_coor,
    .update_snr_stt_info = pq_mng_update_snr_stt_info,
    .update_snr_cfg = pq_mng_update_snr_cfg,
};

hi_s32 pq_mng_register_snr(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_SNR, type, PQ_BIN_ADAPT_MULTIPLE, "snr", &g_snr_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_snr()
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_SNR);

    return ret;
}

