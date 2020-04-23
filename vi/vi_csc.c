/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi csc
 * Author: sdk
 * Create: 2019-12-14
 */

#include "hi_type.h"
#include "hi_debug.h"

#include "vi_csc.h"

VI_STATIC hi_s32 vi_csc_get_rgb2rbg_csc_info(vi_instance *vi_instance_p, hi_drv_pq_csc_info *csc_info_p)
{
    hi_drv_color_descript color_space_in = vi_instance_p->attr_in.color_descript;
    hi_drv_color_descript color_space_out = vi_instance_p->attr_out.color_descript;

    if (color_space_in.color_space == color_space_out.color_space &&
        color_space_in.color_primary == color_space_out.color_primary &&
        color_space_in.quantify_range == color_space_out.quantify_range &&
        color_space_in.transfer_type == color_space_out.transfer_type &&
        color_space_in.matrix_coef == color_space_out.matrix_coef) {
        csc_info_p->csc_en = HI_FALSE;
        return HI_SUCCESS;
    }

    csc_info_p->csc_en = HI_TRUE;
    csc_info_p->csc_pecision = 12; /* Current Csc Pecision: 12 */
    csc_info_p->csc_type = HI_PQ_CSC_NORMAL_SETTING;
    csc_info_p->csc_in = color_space_in;
    csc_info_p->csc_out = color_space_out;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_csc_get_rgb2yuv_csc_info(vi_instance *vi_instance_p, hi_drv_pq_csc_info *csc_info_p)
{
    hi_drv_color_descript color_space_in = vi_instance_p->attr_in.color_descript;
    hi_drv_color_descript color_space_out = vi_instance_p->attr_out.color_descript;

    if (color_space_in.color_space == color_space_out.color_space &&
        color_space_in.color_primary == color_space_out.color_primary &&
        color_space_in.quantify_range == color_space_out.quantify_range &&
        color_space_in.transfer_type == color_space_out.transfer_type &&
        color_space_in.matrix_coef == color_space_out.matrix_coef) {
        csc_info_p->csc_en = HI_FALSE;
        return HI_SUCCESS;
    }

    csc_info_p->csc_en = HI_TRUE;
    csc_info_p->csc_pecision = 12; /* Current Csc Pecision: 12 */
    csc_info_p->csc_type = HI_PQ_CSC_NORMAL_SETTING;
    csc_info_p->csc_in = color_space_in;
    csc_info_p->csc_out = color_space_out;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_csc_get_yuv2yuv_csc_info(vi_instance *vi_instance_p, hi_drv_pq_csc_info *csc_info_p)
{
    hi_drv_color_descript color_space_in = vi_instance_p->attr_in.color_descript;
    hi_drv_color_descript color_space_out = vi_instance_p->attr_out.color_descript;

    if (color_space_in.color_space == color_space_out.color_space &&
        color_space_in.color_primary == color_space_out.color_primary &&
        color_space_in.quantify_range == color_space_out.quantify_range &&
        color_space_in.transfer_type == color_space_out.transfer_type &&
        color_space_in.matrix_coef == color_space_out.matrix_coef) {
        csc_info_p->csc_en = HI_FALSE;
        return HI_SUCCESS;
    }

    csc_info_p->csc_en = HI_TRUE;
    csc_info_p->csc_pecision = 12; /* Current Csc Pecision: 12 */
    csc_info_p->csc_type = HI_PQ_CSC_NORMAL_SETTING;
    csc_info_p->csc_in = color_space_in;
    csc_info_p->csc_out = color_space_out;

    return HI_SUCCESS;
}

hi_s32 vi_csc_get_csc_info(vi_instance *vi_instance_p, hi_drv_pq_csc_info *csc_info_p)
{
    vi_in_attr *vi_in_attr_p = HI_NULL;
    vi_out_attr *vi_out_attr_p = HI_NULL;

    vi_in_attr_p = &vi_instance_p->attr_in;
    vi_out_attr_p = &vi_instance_p->attr_out;

    if (vi_in_attr_p->pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB) {
        if (vi_out_attr_p->pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB) {
            vi_csc_get_rgb2rbg_csc_info(vi_instance_p, csc_info_p);
        } else {
            vi_csc_get_rgb2yuv_csc_info(vi_instance_p, csc_info_p);
        }
    } else {
        vi_csc_get_yuv2yuv_csc_info(vi_instance_p, csc_info_p);
    }

    return HI_SUCCESS;
}

hi_s32 vi_csc_get_csc_para(hi_drv_pq_csc_info csc_info, hi_drv_pq_csc_coef *pq_csc_coef_p)
{
    hi_s32 ret;

    if (csc_info.csc_en == HI_TRUE) {
        ret = vi_comm_pq_get_csc_coef(csc_info, pq_csc_coef_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("get csc coef faild!\n");
            return HI_FAILURE;
        }
    }

#ifdef CUSTOM_TVD_CSC_OFFSET
    if (vi_instance_p->vi_in_attr.access == HI_DRV_VI_ACCESS_TVD) {
        pq_csc_dc_coef_p->csc_in_dc0 += 3; /* dc0 + 3 */
        pq_csc_dc_coef_p->csc_in_dc1 += 3; /* dc1 + 3 */
        pq_csc_dc_coef_p->csc_in_dc2 += 3; /* dc2 + 3 */
    }
#endif

    return HI_SUCCESS;
}
