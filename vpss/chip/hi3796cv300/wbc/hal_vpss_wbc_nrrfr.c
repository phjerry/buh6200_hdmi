/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc_nrrfr.c source file vpss wbc
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_wbc_nrrfr.h"
#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 nrrfr_calc_wbc_filed_phy_addr(vpss_wbc_nrrfr_calc *attr, hi_u64 phy_addr, hi_u32 stride,
    hi_drv_vpss_vid_frame_addr *buf_addr)
{
    /* wbc don't support tile,use linear */
    switch (attr->pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21:
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1: {
            buf_addr->phy_addr_y = phy_addr;
            buf_addr->phy_addr_c = phy_addr + (attr->height / 2) * stride; /* 2:para */
            break;
        }
        default: {
            vpss_error("Unsupport PixFormat %d.\n", attr->pixel_format);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 nrrfr_calc_wbc_frame_phy_addr(vpss_wbc_nrrfr_calc *attr, hi_u64 phy_addr, hi_u32 stride,
    hi_drv_vpss_vid_frame_addr *buf_addr)
{
    /* wbc don't support tile,use linear */
    switch (attr->pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21:
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1:
        case HI_DRV_PIXEL_FMT_NV16:
        case HI_DRV_PIXEL_FMT_NV61: {
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

hi_void nrrfr_get_wbc_frame_pix_fmt(hi_drv_pixel_format attr_pixel_format, hi_drv_pixel_format *pe_wbc_pixel_format)
{
    /* wbc don't support tile,use linear */
    *pe_wbc_pixel_format = attr_pixel_format;
    return;
}

hi_u32 nrrfr_get_total_buf_size(vpss_wbc_nrrfr_attr *attr, hi_u32 node_buf_size)
{
    hi_u32 total_buf_size = 0;

    switch (attr->mode) {
        case VPSS_WBC_NRRFR_MODE_NORMAL: {
            total_buf_size = node_buf_size * 2; /* 2:para */
            break;
        }
        case VPSS_WBC_NRRFR_MODE_3FIELD: {
            total_buf_size = node_buf_size * 3; /* 3:para */
            break;
        }
        case VPSS_WBC_NRRFR_MODE_4FIELD: {
            total_buf_size = node_buf_size * 4; /* 4:para */
            break;
        }
        case VPSS_WBC_NRRFR_MODE_5FIELD: {
            total_buf_size = node_buf_size * 5; /* 5:para */
            break;
        }
        default: {
            vpss_error("Vpss wbc GetTotalBufSize error(WbcMode = %d).\n", attr->mode);
            return 0;
        }
    }

    return total_buf_size;
}

hi_s32 nrrfr_cal_buf_size(hi_u32 *size, hi_u32 *stride, vpss_wbc_nrrfr_calc *attr)
{
    hi_u32 ret_size = 0;
    vpss_buffer_attr buffer_attr = { 0 };
    hi_u32 ret_stride = 0;
    hi_u32 height = attr->height;
    hi_u32 width = attr->width;
    hi_drv_pixel_format pixel_format = attr->pixel_format;
    hi_drv_pixel_bitwidth bit_width = attr->bit_width;
    hi_drv_compress_info cmp_info = attr->cmp_info;
    buffer_attr.width = width;
    buffer_attr.height = height;

    switch (pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21:
            if (cmp_info.cmp_mode != HI_DRV_COMPRESS_MODE_OFF) {
                if (cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_FRM_LOSS) {
                    vpss_comm_cal_buf_size(&buffer_attr, pixel_format, cmp_info, bit_width,
                        VPSS_ISLOSS_CMP, VPSS_SUPPORT_CMP_NR_RATE);
                    ret_size = buffer_attr.buf_size;
                    ret_stride = buffer_attr.stride_y;
                } else {
                    vpss_error("nr cannot support cmp_mode %d\n", cmp_info.cmp_mode);
                    return HI_FAILURE;
                }
            } else if (bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
                ret_stride = vpss_align_8bit_ystride(width);
                ret_size = height * ret_stride * 3 / 2; /* 3,2:para */
            } else if (bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
                ret_stride = vpss_align_10bit_comp_ystride(width);
                ret_size = height * ret_stride * 3 / 2; /* 3,2:para */
            } else {
                vpss_error("Unsupport BitWidth %d.\n", bit_width);
                return HI_FAILURE;
            }

            break;
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1: {
            /* todo : check support cmp mode */
            if (cmp_info.cmp_mode != HI_DRV_COMPRESS_MODE_OFF) {
                if (cmp_info.cmp_mode == HI_DRV_COMPRESS_MODE_FRM_LOSS) {
                    vpss_comm_cal_buf_size(&buffer_attr, pixel_format, cmp_info, bit_width,
                        VPSS_ISLOSS_CMP, VPSS_SUPPORT_CMP_NR_RATE);
                    ret_size = buffer_attr.buf_size;
                    ret_stride = buffer_attr.stride_y;
                } else {
                    vpss_error("nr cannot support cmp_mode %d\n", cmp_info.cmp_mode);
                    return HI_FAILURE;
                }
            } else if (HI_DRV_PIXEL_BITWIDTH_8BIT == bit_width) {
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
    /* todo */
    *stride = ret_stride;
    return HI_SUCCESS;
}

hi_void nrrfr_get_wbc_frame_info(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_nrrfr_calc *attr, hi_u32 stride)
{
    wbc_frame->comm_frame.width = attr->width;
    wbc_frame->comm_frame.height = attr->height;
    wbc_frame->comm_frame.bit_width = attr->bit_width;
    wbc_frame->comm_frame.pixel_format = attr->pixel_format;
    wbc_frame->comm_frame.cmp_info = attr->cmp_info;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_y = stride;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_c = stride;
    wbc_frame->comm_frame.frame_index = 0;
    wbc_frame->comm_frame.pts = 0;
    return;
}

hi_s32 nrrfr_init_blend_frame(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_nrrfr_calc *attr, hi_u64 phy_addr)
{
    hi_s32 ret;
    hi_u32 stride = 0;
    hi_u32 buff_size;
    ret = nrrfr_cal_buf_size(&buff_size, &stride, attr);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    nrrfr_get_wbc_frame_info(wbc_frame, attr, stride);
    nrrfr_get_wbc_frame_pix_fmt(attr->pixel_format, &(wbc_frame->comm_frame.pixel_format));
    ret = nrrfr_calc_wbc_filed_phy_addr(attr, phy_addr, stride, &(wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT]));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    memcpy(&wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT], &wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT],
           sizeof(hi_drv_vpss_vid_frame_addr));
    return HI_SUCCESS;
}

hi_s32 nrrfr_init_frame(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_nrrfr_calc *attr, hi_u64 phy_addr)
{
    hi_s32 ret;
    hi_u32 stride = 0;
    hi_u32 buff_size;
    ret = nrrfr_cal_buf_size(&buff_size, &stride, attr);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    nrrfr_get_wbc_frame_info(wbc_frame, attr, stride);
    nrrfr_get_wbc_frame_pix_fmt(attr->pixel_format, &(wbc_frame->comm_frame.pixel_format));
    ret = nrrfr_calc_wbc_frame_phy_addr(attr, phy_addr, stride, &(wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT]));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    memcpy(&wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT], &wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT],
           sizeof(hi_drv_vpss_vid_frame_addr));
    return HI_SUCCESS;
}
hi_s32 vpss_wbc_nrrfr_init(vpss_wbc_nrrfr *wbc, vpss_wbc_nrrfr_attr *in_attr)
{
    list *list_head = HI_NULL;
    hi_u32 stride = 0;
    hi_u32 node_buff_size;
    hi_u32 total_buff_size;
    hi_u64 phy_addr;
    hi_u8 *vir_addr;
    hi_s32 ret;
    hi_u32 i = 0;
    hi_u32 node_count;
    vpss_wbc_nrrfr_calc calc;
    drv_vpss_mem_attr mem_attr = { 0 };
    hi_drv_ssm_buf_attach_info attach_info;
    hi_u64 sec_info_addr;

    if ((wbc == HI_NULL) || (in_attr == HI_NULL)) {
        vpss_error("Vpss nr wbc Init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (in_attr->mem_name == HI_NULL) {
        vpss_error("nr pstAttr->pu8MemName not init !!!!!!!!.\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_TRUE) {
        (hi_void) vpss_wbc_nrrfr_deinit(wbc);
    }

    if ((in_attr->ref_mode < VPSS_WBC_NRRFR_REF_MODE_INIT) || (in_attr->ref_mode >= VPSS_WBC_NRRFR_REF_MODE_MAX)) {
        vpss_error("Vpss nr wbc Init error(RefMode = %d).\n", in_attr->ref_mode);
        return HI_FAILURE;
    }

    if ((HI_DRV_PIXEL_BITWIDTH_8BIT != in_attr->bit_width) && (HI_DRV_PIXEL_BITWIDTH_10BIT != in_attr->bit_width)) {
        vpss_error("Vpss nr wbc Init error(BitWidth = %d).\n", in_attr->bit_width);
        return HI_FAILURE;
    }

    if (in_attr->mode >= VPSS_WBC_NRRFR_MODE_MAX) {
        vpss_error("Vpss nr wbc Init error(enMode = %d).\n", wbc->wbc_attr.mode);
        return HI_FAILURE;
    }

    memset(wbc, 0, sizeof(vpss_wbc_nrrfr));
    memcpy(&wbc->wbc_attr, in_attr, sizeof(vpss_wbc_nrrfr_attr));

    calc.width = wbc->wbc_attr.width;
    calc.height = wbc->wbc_attr.height;

    if (wbc->wbc_attr.interlace == HI_TRUE) {
        calc.height /= 2; /* 2:para */
    }

    calc.bit_width = wbc->wbc_attr.bit_width;
    calc.pixel_format = wbc->wbc_attr.pixel_format;
    calc.cmp_info = wbc->wbc_attr.cmp_info;
    ret = nrrfr_cal_buf_size(&node_buff_size, &stride, &calc);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }

    total_buff_size = nrrfr_get_total_buf_size(&wbc->wbc_attr, node_buff_size);
    mem_attr.name = wbc->wbc_attr.mem_name;
    mem_attr.size = total_buff_size;
    mem_attr.mode = (wbc->wbc_attr.secure == HI_TRUE) ? OSAL_SECSMMU_TYPE : OSAL_NSSMMU_TYPE;
    mem_attr.is_cache = HI_FALSE;
#ifdef HI_VPSS_DRV_USE_GOLDEN
    mem_attr.is_map_viraddr = HI_TRUE;
#else
    mem_attr.is_map_viraddr = HI_FALSE;
#endif
    if (wbc->wbc_attr.secure == HI_TRUE) {
        mem_attr.is_map_viraddr = HI_FALSE;
    }

    ret = vpss_comm_mem_alloc(&mem_attr, &(wbc->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS nr WBC Alloc memory failed.\n");
        return HI_FAILURE;
    }

    attach_info.session_handle = 0x0FFFFFFF; /* 0x0FFFFFFF: unvalid */
    attach_info.dma_buf_addr = wbc->vpss_mem.dma_handle;
    attach_info.buf_id = HI_SSM_BUFFER_ID_INTERNAL_BUF_VPSS;
    attach_info.module_handle = 0x5CFFFFFF; /* 0x5CFFFFFF: vpss handle, 0x5C valid */
    vpss_comm_attach_buf(wbc->wbc_attr.secure, attach_info, &sec_info_addr);

    phy_addr = wbc->vpss_mem.phy_addr;
    vir_addr = wbc->vpss_mem.vir_addr;
    node_count = (hi_u32)wbc->wbc_attr.mode;
    list_head = &(wbc->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    wbc->first_ref = list_head;

    for (i = 1; i < node_count; i++) {
        osal_list_add_tail(&(wbc->data_list[i].node), list_head);
    }

    switch (wbc->wbc_attr.mode) {
        case VPSS_WBC_NRRFR_MODE_NORMAL: {
            nrrfr_init_frame(&(wbc->data_list[0].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c =
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[1].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c =
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            break;
        }
        /* used only for cls!!!!!!! */
        case VPSS_WBC_NRRFR_MODE_3FIELD: {
            nrrfr_init_frame(&(wbc->data_list[0].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c =
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[1].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c =
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[2].wbc_frame), &calc, phy_addr); /* 2:para */
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[2].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr; /* 2:para */
            wbc->data_list[2].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = /* 2,3:para */
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            break;
        }
        case VPSS_WBC_NRRFR_MODE_4FIELD: {
            nrrfr_init_frame(&(wbc->data_list[0].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c =
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[1].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c =
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[2].wbc_frame), &calc, phy_addr); /* 2:para */
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[2].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr; /* 2:para */
            wbc->data_list[2].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = /* 2,3:para */
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[3].wbc_frame), &calc, phy_addr); /* 3:para */
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc->data_list[3].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr; /* 3:para */
            wbc->data_list[3].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = /* 3,3:para */
                (hi_u64)(vir_addr + stride * calc.height);
#endif
            break;
        }
        case VPSS_WBC_NRRFR_MODE_5FIELD: {
            nrrfr_init_frame(&(wbc->data_list[0].wbc_frame), &calc, phy_addr);
            phy_addr = phy_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[1].wbc_frame), &calc, phy_addr);
            phy_addr = phy_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[2].wbc_frame), &calc, phy_addr); /* 2:para */
            phy_addr = phy_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[3].wbc_frame), &calc, phy_addr); /* 3:para */
            phy_addr = phy_addr + node_buff_size;
            nrrfr_init_frame(&(wbc->data_list[4].wbc_frame), &calc, phy_addr); /* 4:para */
            break;
        }
        default: {
            vpss_error("Vpss nr wbc Init error(WbcMode = %d).\n", wbc->wbc_attr.mode);
            return HI_FAILURE;
        }
    }

    wbc->init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nrrfr_deinit(vpss_wbc_nrrfr *wbc)
{
    if (wbc == HI_NULL) {
        vpss_error("Vpss nr wbc DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(wbc->vpss_mem));
    }

    if (wbc->init == HI_FALSE) {
        vpss_warn("Vpss nr wbc DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(wbc, 0, sizeof(vpss_wbc_nrrfr));
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nrrfr_get_ref_info(vpss_wbc_nrrfr *wbc, hi_drv_vpss_video_frame **data_list)
{
    hi_u32 i;
    vpss_wbc_nrrfr_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;
    vpss_wbc_nrrfr_ref_mode ref_mode;
    hi_u32 node_count;

    if ((wbc == HI_NULL) || (data_list == HI_NULL)) {
        vpss_error("Vpss nr wbc GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_error("Vpss nr wbc GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_mode = wbc->wbc_attr.ref_mode;
    node_count = (hi_u32)wbc->wbc_attr.mode - 1;
    ref_node = wbc->first_ref->prev->prev;

    for (i = 0; i < node_count; i++) {
        ref_node_data = osal_list_entry(ref_node, vpss_wbc_nrrfr_data, node);
        data_list[node_count - i - 1] = &(ref_node_data->wbc_frame);
        ref_node = ref_node->prev;
    }

    return HI_SUCCESS;
}
hi_s32 vpss_wbc_nrrfr_get_wbc_info(vpss_wbc_nrrfr *wbc, hi_drv_vpss_video_frame **data)
{
    vpss_wbc_nrrfr_data *wbc_node_data = HI_NULL;
    list *wbc_node = HI_NULL;

    if ((wbc == HI_NULL) || (data == HI_NULL)) {
        vpss_error("Vpss nr wbc GetWbcInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_error("Vpss nr wbc GetWbcInfo error(not init).\n");
        return HI_FAILURE;
    }

    wbc_node = wbc->first_ref->prev;
    wbc_node_data = osal_list_entry(wbc_node, vpss_wbc_nrrfr_data, node);
    *data = &(wbc_node_data->wbc_frame);
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nrrfr_complete(vpss_wbc_nrrfr *wbc)
{
    if (wbc == HI_NULL) {
        vpss_error("Vpss nr wbc complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_error("Vpss nr wbc complete error(not init).\n");
        return HI_FAILURE;
    }

    if ((wbc->complete_count + 1) == 0) {
        wbc->complete_count = VPSS_WBC_NRRFR_MAX_NODE;
    }

    wbc->complete_count++;
    wbc->first_ref = wbc->first_ref->next;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nrrfr_reset(vpss_wbc_nrrfr *wbc)
{
    if (wbc == HI_NULL) {
        vpss_warn("Vpss nr wbc reset error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_warn("Vpss nr wbc reset error(not init).\n");
        return HI_FAILURE;
    }

    wbc->complete_count = 0;
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

