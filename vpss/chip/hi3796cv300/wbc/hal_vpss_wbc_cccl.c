/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc_cccl.c source file vpss wbc
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_wbc_cccl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 ccclrfr_calc_wbc_frame_phy_addr(vpss_wbc_ccclrfr_calc *attr,
    hi_u64 phy_addr, hi_u32 stride, hi_drv_vpss_vid_frame_addr *buf_addr)
{
    /* wbc don't support tile,use linear */
    switch (attr->pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21:
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1: {
            buf_addr->phy_addr_y = phy_addr;
            buf_addr->phy_addr_c = phy_addr + stride * attr->height;
            break;
        }
        default: {
            vpss_error("Unsupport PixFormat %d.\n", attr->pixel_format);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_void ccclrfr_get_wbc_frame_pix_fmt(hi_drv_pixel_format attr_pixel_format, hi_drv_pixel_format *pe_wbc_pixel_format)
{
    /* wbc don't support tile,use linear */
    *pe_wbc_pixel_format = attr_pixel_format;
#ifdef TOBEMODIFY
    switch (attr_pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12_TILE:  // wbc don't support tile,use linear
            *pe_wbc_pixel_format = HI_DRV_PIXEL_FMT_NV12;
            break;
        case HI_DRV_PIXEL_FMT_NV21_TILE:  // wbc don't support tile,use linear
            *pe_wbc_pixel_format = HI_DRV_PIXEL_FMT_NV21;
            break;
        case HI_DRV_PIXEL_FMT_NV12_TILE_CMP:  // wbc don't support tile,use linear
            *pe_wbc_pixel_format = HI_DRV_PIXEL_FMT_NV12;
            break;
        case HI_DRV_PIXEL_FMT_NV21_TILE_CMP:  // wbc don't support tile,use linear
            *pe_wbc_pixel_format = HI_DRV_PIXEL_FMT_NV21;
            break;
        default: {
            *pe_wbc_pixel_format = attr_pixel_format;
        }
    }
#endif
    return;
}

hi_s32 ccclrfr_cal_buf_size(hi_u32 *size, hi_u32 *stride, vpss_wbc_ccclrfr_calc *attr)
{
    hi_u32 ret_size = 0;
    hi_u32 ret_stride = 0;
    hi_u32 height = attr->height;
    hi_u32 width = attr->width;
    hi_drv_pixel_format pixel_format = attr->pixel_format;
    hi_drv_pixel_bitwidth bit_width = attr->bit_width;

    /* wbc don't support tile,use linear */
    switch (pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21: {
            if (HI_DRV_PIXEL_BITWIDTH_8BIT == bit_width) {
                ret_stride = vpss_align_8bit_ystride(width);
                ret_size = height * ret_stride * 2; /* 2:para */
            } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == bit_width) {
                ret_stride = vpss_align_10bit_comp_ystride(width);
                ret_size = height * ret_stride * 2; /* 2:para */
            } else {
                vpss_error("Unsupport BitWidth %d.\n", bit_width);
                return HI_FAILURE;
            }

            break;
        }
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1: {
            if (HI_DRV_PIXEL_BITWIDTH_8BIT == bit_width) {
                ret_stride = vpss_align_8bit_ystride(width);
                ret_size = height * ret_stride * 2; /* 2:para */
            } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == bit_width) {
                ret_stride = vpss_align_10bit_comp_ystride(width);
                ret_size = height * ret_stride * 2; /* 2:para */
            } else {
                vpss_error("Unsupport BitWidth %d.\n", bit_width);
                return HI_FAILURE;
            }

            break;
        }
        default: {
            vpss_error("Unsupport PixFormat %d.\n", pixel_format);
            return HI_FAILURE;
        }
    }

    *size = ret_size;
    *stride = ret_stride;
    return HI_SUCCESS;
}

hi_void ccclrfr_get_wbc_frame_info(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_ccclrfr_calc *attr, hi_u32 stride)
{
    wbc_frame->comm_frame.width = attr->width;
    wbc_frame->comm_frame.height = attr->height;
    wbc_frame->comm_frame.bit_width = attr->bit_width;
    wbc_frame->comm_frame.pixel_format = attr->pixel_format;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_y = stride;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_c = stride;
    return;
}

hi_s32 vpss_wbc_ccclrfr_init_frame(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_ccclrfr_calc *attr, hi_u64 phy_addr)
{
    hi_s32 ret;
    hi_u32 stride = 0;
    hi_u32 buff_size = 0;
    ret = ccclrfr_cal_buf_size(&buff_size, &stride, attr);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ccclrfr_get_wbc_frame_info(wbc_frame, attr, stride);
    ccclrfr_get_wbc_frame_pix_fmt(attr->pixel_format, &(wbc_frame->comm_frame.pixel_format));
    /* todo : tile cmp */
    ret = ccclrfr_calc_wbc_frame_phy_addr(attr, phy_addr, stride, &(wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT]));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    memcpy(&wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT], &wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT],
           sizeof(hi_drv_vpss_vid_frame_addr));
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_ccclrfr_init(vpss_wbc_ccclrfr *cccl_wbc, vpss_wbc_ccclrfr_attr *cccl_attr)
{
    list *list_head = HI_NULL;
    hi_u32 stride = 0;
    hi_u32 node_buff_size = 0;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    hi_u32 i = 0;
    vpss_wbc_ccclrfr_calc calc;
    drv_vpss_mem_attr mem_attr = { 0 };
    hi_drv_ssm_buf_attach_info attach_info;
    hi_u64 sec_info_addr;

    if ((cccl_wbc == HI_NULL) || (cccl_attr == HI_NULL)) {
        vpss_error("Vpss cccl wbc Init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_attr->mem_name == HI_NULL) {
        vpss_error("cccl pstAttr->pu8MemName not init !!!!!!!!.\n");
        return HI_FAILURE;
    }

    if (cccl_wbc->init == HI_TRUE) {
        (hi_void) vpss_wbc_ccclrfr_deinit(cccl_wbc);
    }

    if ((cccl_attr->ref_mode < VPSS_WBC_CCCLRFR_REF_MODE_INIT) ||
        (cccl_attr->ref_mode >= VPSS_WBC_CCCLRFR_REF_MODE_MAX)) {
        vpss_error("Vpss cccl wbc Init error(RefMode = %d).\n", cccl_attr->ref_mode);
        return HI_FAILURE;
    }

    if ((HI_DRV_PIXEL_BITWIDTH_8BIT != cccl_attr->bit_width) &&
        (HI_DRV_PIXEL_BITWIDTH_10BIT != cccl_attr->bit_width)) {
        vpss_error("Vpss cccl wbc Init error(BitWidth = %d).\n", cccl_attr->bit_width);
        return HI_FAILURE;
    }

    memset(cccl_wbc, 0, sizeof(vpss_wbc_ccclrfr));
    memcpy(&cccl_wbc->wbc_attr, cccl_attr, sizeof(vpss_wbc_ccclrfr_attr));
    calc.width = cccl_attr->width;
    calc.height = cccl_attr->height;

    if (cccl_attr->interlace == HI_TRUE) {
        calc.height /= 2; /* 2:para */
    }

    calc.bit_width = cccl_attr->bit_width;
    calc.pixel_format = cccl_attr->pixel_format;
    ret = ccclrfr_cal_buf_size(&node_buff_size, &stride, &calc);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }

    mem_attr.name = cccl_attr->mem_name;
    mem_attr.size = node_buff_size * VPSS_WBC_CCCLRFR_MAX_NODE;
    mem_attr.mode = (cccl_attr->secure == HI_TRUE) ? OSAL_SECSMMU_TYPE : OSAL_NSSMMU_TYPE;
    mem_attr.is_cache = HI_FALSE;
#ifdef HI_VPSS_DRV_USE_GOLDEN
    mem_attr.is_map_viraddr = HI_TRUE;
#else
    mem_attr.is_map_viraddr = HI_FALSE;
#endif

    if (cccl_attr->secure == HI_TRUE) {
        mem_attr.is_map_viraddr = HI_FALSE;
    }

    ret = vpss_comm_mem_alloc(&mem_attr, &(cccl_wbc->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS cccl WBC Alloc memory failed.\n");
        return HI_FAILURE;
    }

    attach_info.session_handle = 0x0FFFFFFF; /* 0x0FFFFFFF: unvalid */
    attach_info.dma_buf_addr = cccl_wbc->vpss_mem.dma_handle;
    attach_info.buf_id = HI_SSM_BUFFER_ID_INTERNAL_BUF_VPSS;
    attach_info.module_handle = 0x5CFFFFFF; /* 0x5CFFFFFF: vpss handle, 0x5C valid */
    vpss_comm_attach_buf(cccl_attr->secure, attach_info, &sec_info_addr);

    phy_addr = cccl_wbc->vpss_mem.phy_addr;
    vir_addr = cccl_wbc->vpss_mem.vir_addr;
    list_head = &(cccl_wbc->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    cccl_wbc->first_ref = list_head;

    for (i = 1; i < VPSS_WBC_CCCLRFR_MAX_NODE; i++) {
        osal_list_add_tail(&(cccl_wbc->data_list[i].node), list_head);
    }

    for (i = 0; i < VPSS_WBC_CCCLRFR_MAX_NODE; i++) {
        vpss_wbc_ccclrfr_init_frame(&(cccl_wbc->data_list[i].wbc_frame), &calc, phy_addr);
        phy_addr = phy_addr + node_buff_size;
#ifdef HI_VPSS_DRV_USE_GOLDEN
        cccl_wbc->data_list[i].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
        cccl_wbc->data_list[i].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c =
            (hi_u64)(vir_addr + stride * calc.height);
        vir_addr = vir_addr + node_buff_size;
#endif
    }

    cccl_wbc->init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_ccclrfr_deinit(vpss_wbc_ccclrfr *cccl_wbc)
{
    if (cccl_wbc == HI_NULL) {
        vpss_error("Vpss cccl wbc DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_wbc->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(cccl_wbc->vpss_mem));
    }

    if (cccl_wbc->init == HI_FALSE) {
        vpss_warn("Vpss cccl wbc DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(cccl_wbc, 0, sizeof(vpss_wbc_ccclrfr));
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_ccclrfr_get_pre_info(vpss_wbc_ccclrfr *cccl_wbc,
                                     hi_drv_vpss_video_frame **data_pre2,
                                     hi_drv_vpss_video_frame **data_pre4,
                                     hi_drv_vpss_video_frame **data_pre8)
{
    hi_u32 i;
    vpss_wbc_ccclrfr_data *ref_node_data = HI_NULL;
    hi_drv_vpss_video_frame *data_list[VPSS_WBC_CCCLRFR_MAX_NODE];
    list *ref_node = HI_NULL;

    if (cccl_wbc == HI_NULL) {
        vpss_error("Vpss cccl wbc GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_wbc->init == HI_FALSE) {
        vpss_error("Vpss cccl wbc GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = cccl_wbc->first_ref->prev;

    for (i = 0; i < VPSS_WBC_CCCLRFR_MAX_NODE; i++) {
        ref_node_data = osal_list_entry(ref_node, vpss_wbc_ccclrfr_data, node);
        data_list[VPSS_WBC_CCCLRFR_MAX_NODE - 1 - i] = &(ref_node_data->wbc_frame);
        ref_node = ref_node->prev;
    }

    *data_pre2 = data_list[VPSS_WBC_CCCLRFR_MAX_NODE - 1 - 2]; /* 2,1,2:para */
    *data_pre4 = data_list[VPSS_WBC_CCCLRFR_MAX_NODE - 1 - 4]; /* 4,1,4:para */
    *data_pre8 = data_list[VPSS_WBC_CCCLRFR_MAX_NODE - 1];
    return HI_SUCCESS;
}
hi_s32 vpss_wbc_ccclrfr_get_wbc_info(vpss_wbc_ccclrfr *cccl_wbc, hi_drv_vpss_video_frame **data)
{
    vpss_wbc_ccclrfr_data *wbc_node_data = HI_NULL;
    list *wbc_node = HI_NULL;

    if ((cccl_wbc == HI_NULL) || (data == HI_NULL)) {
        vpss_error("Vpss cccl wbc GetWbcInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_wbc->init == HI_FALSE) {
        vpss_error("Vpss cccl wbc GetWbcInfo error(not init).\n");
        return HI_FAILURE;
    }

    wbc_node = cccl_wbc->first_ref->prev;
    wbc_node_data = osal_list_entry(wbc_node, vpss_wbc_ccclrfr_data, node);
    *data = &(wbc_node_data->wbc_frame);
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_ccclrfr_complete(vpss_wbc_ccclrfr *cccl_wbc)
{
    if (cccl_wbc == HI_NULL) {
        vpss_error("Vpss cccl wbc complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_wbc->init == HI_FALSE) {
        vpss_error("Vpss cccl wbc complete error(not init).\n");
        return HI_FAILURE;
    }

    if ((cccl_wbc->complete_count + 1) == 0) {
        cccl_wbc->complete_count = VPSS_WBC_CCCLRFR_MAX_NODE;
    }

    cccl_wbc->complete_count++;
    cccl_wbc->first_ref = cccl_wbc->first_ref->next;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_ccclrfr_reset(vpss_wbc_ccclrfr *cccl_wbc)
{
    if (cccl_wbc == HI_NULL) {
        vpss_warn("Vpss cccl wbc reset error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_wbc->init == HI_FALSE) {
        vpss_warn("Vpss cccl wbc reset error(not init).\n");
        return HI_FAILURE;
    }

    cccl_wbc->complete_count = 0;
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

