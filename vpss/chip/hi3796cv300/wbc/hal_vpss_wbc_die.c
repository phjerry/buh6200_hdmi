/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc_die.c source file vpss wbc
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_wbc_die.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 deirfr_cal_buf_size(hi_u32 *size, hi_u32 *stride, vpss_wbc_deirfr_calc *attr)
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
                ret_size = height * ret_stride * 3 / 2; /* 3,2:para */
            } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == bit_width) {
                ret_stride = vpss_align_10bit_comp_ystride(width);
                ret_size = height * ret_stride * 3 / 2; /* 3,2:para */
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

// CodingPartner Format Code Section End Will Del Finally
hi_void deirfr_get_wbc_frame_info(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_deirfr_calc *attr, hi_u32 stride)
{
    wbc_frame->comm_frame.width = attr->width;
    wbc_frame->comm_frame.height = attr->height;
    wbc_frame->comm_frame.bit_width = attr->bit_width;
    wbc_frame->comm_frame.pixel_format = attr->pixel_format;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_y = stride;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_c = stride;
    return;
}

hi_void deirfr_get_wbc_frame_pix_fmt(hi_drv_pixel_format attr_pixel_format, hi_drv_pixel_format *pe_wbc_pixel_format)
{
    /* wbc don't support tile,use linear ; todo : whether need cmp */
    *pe_wbc_pixel_format = attr_pixel_format;
#ifdef TOBEMODIFY /* todo */
    switch (attr_pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
            *pe_wbc_pixel_format = HI_DRV_PIXEL_FMT_NV12;
            break;

        case HI_DRV_PIXEL_FMT_NV21:
            *pe_wbc_pixel_format = HI_DRV_PIXEL_FMT_NV21;
            break;

        default: {
            *pe_wbc_pixel_format = attr_pixel_format;
        }
    }
#endif
    return;
}

hi_s32 deirfr_calc_wbc_frame_phy_addr(vpss_wbc_deirfr_calc *attr,
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

hi_void deirfr_init_frame(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_deirfr_calc *attr, hi_u64 phy_addr)
{
    hi_s32 ret;
    hi_u32 stride = 0;
    hi_u32 buff_size = 0;
    ret = deirfr_cal_buf_size(&buff_size, &stride, attr);
    if (ret != HI_SUCCESS) {
        return;
    }

    deirfr_get_wbc_frame_info(wbc_frame, attr, stride);
    deirfr_get_wbc_frame_pix_fmt(attr->pixel_format, &(wbc_frame->comm_frame.pixel_format));
    ret = deirfr_calc_wbc_frame_phy_addr(attr, phy_addr, stride, &(wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT]));
    if (ret != HI_SUCCESS) {
        return;
    }

    memcpy(&wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT], &wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT],
           sizeof(hi_drv_vpss_vid_frame_addr));
    return;
}

static hi_s32 deirfr_buf_attach(hi_bool secure, struct dma_buf *dma_handle)
{
    hi_s32 ret;
    hi_drv_ssm_buf_attach_info attach_info;
    hi_u64 sec_info_addr;

    attach_info.session_handle = 0x0FFFFFFF; /* 0x0FFFFFFF: unvalid */
    attach_info.dma_buf_addr = dma_handle;
    attach_info.buf_id = HI_SSM_BUFFER_ID_INTERNAL_BUF_VPSS;
    attach_info.module_handle = 0x5CFFFFFF; /* 0x5CFFFFFF: vpss handle, 0x5C valid */
    ret = vpss_comm_attach_buf(secure, attach_info, &sec_info_addr);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 deirfr_mem_alloc(drv_vpss_mem_info *mem_info, vpss_wbc_deirfr_attr *in_attr,
    hi_u32 *stride, vpss_wbc_deirfr_calc *calc)
{
    hi_s32 ret;
    hi_u32 node_buff_size = 0;
    drv_vpss_mem_attr mem_attr = { 0 };

    ret = deirfr_cal_buf_size(&node_buff_size, stride, calc);
    if (ret != HI_SUCCESS) {
        vpss_error("wbc_deirfr_cal_buf failed.\n");
        return HI_FAILURE;
    }
    mem_attr.name = "vpss_deiWbcBuf";
    mem_attr.size = node_buff_size;
    mem_attr.mode = (in_attr->secure == HI_TRUE) ? OSAL_SECSMMU_TYPE : OSAL_NSSMMU_TYPE;
    mem_attr.is_cache = HI_FALSE;
#ifdef HI_VPSS_DRV_USE_GOLDEN
    mem_attr.is_map_viraddr = (in_attr->secure == HI_TRUE) ? HI_FALSE : HI_TRUE;
#else
    mem_attr.is_map_viraddr = HI_FALSE;
#endif

    ret = vpss_comm_mem_alloc(&mem_attr, mem_info);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss dei blend wbc alloc memory failed.\n");
        return HI_FAILURE;
    }

    ret = deirfr_buf_attach(in_attr->secure, mem_info->dma_handle);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss dei blend wbc alloc memory failed.\n");
        if (mem_info->phy_addr != 0) {
            vpss_comm_mem_free(mem_info);
        }
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_wbc_deirfr_init(vpss_wbc_deirfr *dei_wbc, vpss_wbc_deirfr_attr *in_attr)
{
    hi_s32 ret;
    hi_u32 stride = 0;
    hi_u64 phy_addr;
    vpss_wbc_deirfr_calc calc = { 0 };

    if ((dei_wbc == HI_NULL) || (in_attr == HI_NULL)) {
        vpss_error("Vpss die blend wbc Init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_wbc->init == HI_TRUE) {
        (hi_void) vpss_wbc_deirfr_deinit(dei_wbc);
    }

    memset(dei_wbc, 0, sizeof(vpss_wbc_deirfr));
    memcpy(&dei_wbc->dei_wbc_attr, in_attr, sizeof(vpss_wbc_deirfr_attr));

    calc.width = in_attr->width;
    calc.height = in_attr->height / 2; /* 2:para */
    calc.bit_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
    calc.pixel_format = in_attr->pixel_format;
    ret = deirfr_mem_alloc(&(dei_wbc->vpss_mem), in_attr, &stride, &calc);
    if (ret != HI_SUCCESS) {
        vpss_error("deirfr_mem_alloc failed.\n");
        return HI_FAILURE;
    }

    phy_addr = dei_wbc->vpss_mem.phy_addr;
    deirfr_init_frame(&(dei_wbc->wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
    {
        hi_u8 *vir_addr = HI_NULL;
        vir_addr = dei_wbc->vpss_mem.vir_addr;
        dei_wbc->wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
        dei_wbc->wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = (hi_u64)(vir_addr + stride * calc.height);
    }
#endif
    dei_wbc->init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_deirfr_deinit(vpss_wbc_deirfr *dei_wbc)
{
    if (dei_wbc == HI_NULL) {
        vpss_error("Vpss blend wbc DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_wbc->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(dei_wbc->vpss_mem));
    }

    if (dei_wbc->init == HI_FALSE) {
        vpss_warn("Vpss blend wbc DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(dei_wbc, 0, sizeof(vpss_wbc_deirfr));
    return HI_SUCCESS;
}

// 获取回写队列中的参考帧
hi_s32 vpss_wbc_deirfr_get_ref_info(vpss_wbc_deirfr *dei_wbc, hi_drv_vpss_video_frame **ref_frame)
{
    if ((dei_wbc == HI_NULL) || (ref_frame == HI_NULL)) {
        vpss_error("Vpss blend wbc GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_wbc->init == HI_FALSE) {
        vpss_error("Vpss blend wbc GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    *ref_frame = &dei_wbc->wbc_frame;
    return HI_SUCCESS;
}

// 获取回写队列中的回写帧
hi_s32 vpss_wbc_deirfr_get_wbc_info(vpss_wbc_deirfr *dei_wbc, hi_drv_vpss_video_frame **wbc_frame)
{
    if ((dei_wbc == HI_NULL) || (wbc_frame == HI_NULL)) {
        vpss_error("Vpss blend wbc GetWbcInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_wbc->init == HI_FALSE) {
        vpss_error("Vpss blend wbc GetWbcInfo error(not init).\n");
        return HI_FAILURE;
    }

    *wbc_frame = &dei_wbc->wbc_frame;
    return HI_SUCCESS;
}
// 设置回写队列轮转
hi_s32 vpss_wbc_deirfr_complete_image(vpss_wbc_deirfr *dei_wbc)
{
    if (dei_wbc == HI_NULL) {
        vpss_error("Vpss blend wbc complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_wbc->init == HI_FALSE) {
        vpss_error("Vpss blend wbc complete error(not init).\n");
        return HI_FAILURE;
    }

    dei_wbc->complete_count++;
    return HI_SUCCESS;
}
hi_s32 vpss_wbc_deirfr_reset(vpss_wbc_deirfr *dei_wbc)
{
    if (dei_wbc == HI_NULL) {
        vpss_warn("Vpss blend wbc reset error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_wbc->init == HI_FALSE) {
        vpss_warn("Vpss blend wbc reset error(not init).\n");
        return HI_FAILURE;
    }

    dei_wbc->complete_count = 0;
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

