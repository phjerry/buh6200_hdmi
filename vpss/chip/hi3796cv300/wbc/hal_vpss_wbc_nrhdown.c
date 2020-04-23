/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc_nrhdown.c source file vpss wbc
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_wbc_nrhdown.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 nr_hdown_calc_wbc_frame_phy_addr(vpss_wbc_nrhdown_calc *attr, hi_u64 phy_addr, hi_u32 stride,
    hi_drv_vpss_vid_frame_addr *buf_addr)
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

hi_void nr_hdown_get_wbc_frame_pix_fmt(hi_drv_pixel_format attr_pixel_format, hi_drv_pixel_format *pe_wbc_pixel_format)
{
    /* wbc don't support tile,use linear */
    if ((attr_pixel_format == HI_DRV_PIXEL_FMT_NV12) ||
        (attr_pixel_format == HI_DRV_PIXEL_FMT_NV12)) {
        *pe_wbc_pixel_format = HI_DRV_PIXEL_FMT_NV08;
    } else {
        *pe_wbc_pixel_format = attr_pixel_format;
    }

    return;
}

hi_u32 nr_hdown_get_total_buf_size(vpss_wbc_nrhdown_attr *attr, hi_u32 node_buf_size)
{
    hi_u32 total_buf_size = 0;

    switch (attr->mode) {
        case VPSS_WBC_NRHDOWN_MODE_NORMAL: {
            total_buf_size = node_buf_size * 2; /* 2:para */
            break;
        }
        case VPSS_WBC_NRHDOWN_MODE_3FIELD: {
            total_buf_size = node_buf_size * 3; /* 3:para */
            break;
        }
        case VPSS_WBC_NRHDOWN_MODE_4FIELD: {
            total_buf_size = node_buf_size * 2; /* 2:para */
            break;
        }
        case VPSS_WBC_NRHDOWN_MODE_5FIELD: {
            total_buf_size = node_buf_size * 3; /* 3:para */
            break;
        }
        default: {
            vpss_error("Vpss wbc GetTotalBufSize error(WbcMode = %d).\n", attr->mode);
            return 0;
        }
    }

    return total_buf_size;
}

hi_s32 nr_hdown_cal_buf_size(hi_u32 *size, hi_u32 *stride, vpss_wbc_nrhdown_calc *attr)
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
                /* because of RGME ME don't care 422 or 420 only deal with 8bit & Y ,so BufSize  should cal 8bit & Y */
                ret_size = height * ret_stride;
            } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == bit_width) {
                ret_stride = vpss_align_10bit_comp_ystride(width);
                ret_size = height * ret_stride;
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
                ret_size = height * ret_stride;
            } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == bit_width) {
                ret_stride = vpss_align_10bit_comp_ystride(width);
                ret_size = height * ret_stride;
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

hi_void nr_hdown_get_wbc_frame_info(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_nrhdown_calc *attr, hi_u32 stride)
{
    wbc_frame->comm_frame.width = attr->width;
    wbc_frame->comm_frame.height = attr->height;
    wbc_frame->comm_frame.bit_width = attr->bit_width;
    wbc_frame->comm_frame.pixel_format = attr->pixel_format;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_y = stride;
    wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_c = stride;
    return;
}

hi_s32 nr_hdown_init_frame(hi_drv_vpss_video_frame *wbc_frame, vpss_wbc_nrhdown_calc *attr, hi_u64 phy_addr)
{
    hi_s32 ret;
    hi_u32 stride = 0;
    hi_u32 buff_size = 0;
    ret = nr_hdown_cal_buf_size(&buff_size, &stride, attr);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    nr_hdown_get_wbc_frame_info(wbc_frame, attr, stride);
    nr_hdown_get_wbc_frame_pix_fmt(attr->pixel_format, &(wbc_frame->comm_frame.pixel_format));
    ret = nr_hdown_calc_wbc_frame_phy_addr(attr, phy_addr, stride, &(wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT]));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    memcpy(&wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT], &wbc_frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT],
           sizeof(hi_drv_vpss_vid_frame_addr));
    return HI_SUCCESS;
}

/* because of RGME ME only deal with 8bit & Y ,so NrHdown_Init  should introduced into 8bit */
hi_s32 vpss_wbc_nr_hdown_init(vpss_wbc_nrhdown *wbc_nr_hdown, vpss_wbc_nrhdown_attr *in_attr)
{
    list *list_head = HI_NULL;
    hi_u32 stride = 0;
    hi_u32 node_buff_size = 0;
    hi_u32 total_buff_size;
    hi_u64 phy_addr;
    hi_u8 *vir_addr;
    hi_s32 ret;
    hi_u32 i = 0;
    hi_u32 node_count;
    vpss_wbc_nrhdown_calc calc;
    drv_vpss_mem_attr mem_attr = { 0 };
    hi_drv_ssm_buf_attach_info attach_info;
    hi_u64 sec_info_addr;

    if ((wbc_nr_hdown == HI_NULL) || (in_attr == HI_NULL)) {
        vpss_error("Vpss nrhdown wbc Init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (in_attr->mem_name == HI_NULL) {
        vpss_error("nrhdown pstAttr->pu8MemName not init !!!!!!!!.\n");
        return HI_FAILURE;
    }

    if (wbc_nr_hdown->init == HI_TRUE) {
        (hi_void) vpss_wbc_nr_hdown_deinit(wbc_nr_hdown);
    }

    if ((in_attr->ref_mode < VPSS_WBC_NRHDOWN_REF_MODE_INIT) || (in_attr->ref_mode >= VPSS_WBC_NRHDOWN_REF_MODE_MAX)) {
        vpss_error("Vpss nrhdown wbc Init error(RefMode = %d).\n", in_attr->ref_mode);
        return HI_FAILURE;
    }

    if ((HI_DRV_PIXEL_BITWIDTH_8BIT != in_attr->bit_width) && (HI_DRV_PIXEL_BITWIDTH_10BIT != in_attr->bit_width)) {
        vpss_error("Vpss nrhdown wbc Init error(BitWidth = %d).\n", in_attr->bit_width);
        return HI_FAILURE;
    }

    if (in_attr->mode >= VPSS_WBC_NRHDOWN_MODE_MAX) {
        vpss_error("Vpss nrhdown wbc Init error(enMode = %d).\n", in_attr->mode);
        return HI_FAILURE;
    }

    memset(wbc_nr_hdown, 0, sizeof(vpss_wbc_nrhdown));
    memcpy(&wbc_nr_hdown->wbc_attr, in_attr, sizeof(vpss_wbc_nrhdown_attr));

    if (wbc_nr_hdown->wbc_attr.width > 960) { /* 960 : > 960 width /2 */
        calc.width = vpss_upround_div2align(wbc_nr_hdown->wbc_attr.width);
    } else {
        calc.width = wbc_nr_hdown->wbc_attr.width;
    }

    calc.height = wbc_nr_hdown->wbc_attr.height;

    if (wbc_nr_hdown->wbc_attr.interlace == HI_TRUE) {
        calc.height /= 2; /* 2:para */
    }

    calc.bit_width = wbc_nr_hdown->wbc_attr.bit_width;
    calc.pixel_format = wbc_nr_hdown->wbc_attr.pixel_format;
    ret = nr_hdown_cal_buf_size(&node_buff_size, &stride, &calc);
    if (ret == HI_FAILURE) {
        return HI_FAILURE;
    }

    total_buff_size = nr_hdown_get_total_buf_size(&wbc_nr_hdown->wbc_attr, node_buff_size);
    mem_attr.name = wbc_nr_hdown->wbc_attr.mem_name;
    mem_attr.size = total_buff_size;
    mem_attr.mode = (wbc_nr_hdown->wbc_attr.secure == HI_TRUE) ? OSAL_SECSMMU_TYPE : OSAL_NSSMMU_TYPE;
    mem_attr.is_cache = HI_FALSE;
#ifdef HI_VPSS_DRV_USE_GOLDEN
    mem_attr.is_map_viraddr = HI_TRUE;
#else
    mem_attr.is_map_viraddr = HI_FALSE;
#endif
    if (wbc_nr_hdown->wbc_attr.secure == HI_TRUE) {
        mem_attr.is_map_viraddr = HI_FALSE;
    }

    ret = vpss_comm_mem_alloc(&mem_attr, &(wbc_nr_hdown->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS nrhdown WBC Alloc memory failed.\n");
        return HI_FAILURE;
    }

    attach_info.session_handle = 0x0FFFFFFF; /* 0x0FFFFFFF: unvalid */
    attach_info.dma_buf_addr = wbc_nr_hdown->vpss_mem.dma_handle;
    attach_info.buf_id = HI_SSM_BUFFER_ID_INTERNAL_BUF_VPSS;
    attach_info.module_handle = 0x5CFFFFFF; /* 0x5CFFFFFF: vpss handle, 0x5C valid */
    vpss_comm_attach_buf(wbc_nr_hdown->wbc_attr.secure, attach_info, &sec_info_addr);

    phy_addr = wbc_nr_hdown->vpss_mem.phy_addr;
    vir_addr = wbc_nr_hdown->vpss_mem.vir_addr;
    node_count = (hi_u32)wbc_nr_hdown->wbc_attr.mode;
    list_head = &(wbc_nr_hdown->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    wbc_nr_hdown->first_ref = list_head;

    for (i = 1; i < node_count; i++) {
        osal_list_add_tail(&(wbc_nr_hdown->data_list[i].node), list_head);
    }

    switch (wbc_nr_hdown->wbc_attr.mode) {
        case VPSS_WBC_NRHDOWN_MODE_NORMAL: {
            nr_hdown_init_frame(&(wbc_nr_hdown->data_list[0].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc_nr_hdown->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc_nr_hdown->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = (hi_u64)vir_addr;
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nr_hdown_init_frame(&(wbc_nr_hdown->data_list[1].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc_nr_hdown->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc_nr_hdown->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = (hi_u64)vir_addr;
#endif
            break;
        }
        case VPSS_WBC_NRHDOWN_MODE_3FIELD: {
            nr_hdown_init_frame(&(wbc_nr_hdown->data_list[0].wbc_frame), &calc, phy_addr);
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc_nr_hdown->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc_nr_hdown->data_list[0].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = (hi_u64)vir_addr;
#endif
            phy_addr = phy_addr + node_buff_size;
            vir_addr = vir_addr + node_buff_size;
            nr_hdown_init_frame(&(wbc_nr_hdown->data_list[1].wbc_frame), &calc, phy_addr);
            phy_addr = phy_addr + node_buff_size;
#ifdef HI_VPSS_DRV_USE_GOLDEN
            wbc_nr_hdown->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            wbc_nr_hdown->data_list[1].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = (hi_u64)vir_addr;
#endif
            vir_addr = vir_addr + node_buff_size;
            nr_hdown_init_frame(&(wbc_nr_hdown->data_list[2].wbc_frame), &calc, phy_addr); /* 2:para */
#ifdef HI_VPSS_DRV_USE_GOLDEN
            /* 2:para */
            wbc_nr_hdown->data_list[2].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_y = (hi_u64)vir_addr;
            /* 2:para */
            wbc_nr_hdown->data_list[2].wbc_frame.vpss_buf_addr[HI_DRV_3D_EYE_LEFT].vir_addr_c = (hi_u64)vir_addr;
#endif
            break;
        }
        default: {
            vpss_error("Vpss nrhdown wbc Init error(WbcMode = %d).\n", wbc_nr_hdown->wbc_attr.mode);
            return HI_FAILURE;
        }
    }

    wbc_nr_hdown->init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nr_hdown_deinit(vpss_wbc_nrhdown *wbc)
{
    if (wbc == HI_NULL) {
        vpss_error("Vpss nrhdown wbc DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(wbc->vpss_mem));
    }

    if (wbc->init == HI_FALSE) {
        vpss_warn("Vpss nrhdown wbc DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(wbc, 0, sizeof(vpss_wbc_nrhdown));
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nr_hdown_get_ref_info(vpss_wbc_nrhdown *wbc, hi_drv_vpss_video_frame **data)
{
    vpss_wbc_nrhdown_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((wbc == HI_NULL) || (data == HI_NULL)) {
        vpss_error("Vpss nrhdown wbc GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_error("Vpss nrhdown wbc GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = wbc->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_wbc_nrhdown_data, node);
    *data = &(ref_node_data->wbc_frame);
    return HI_SUCCESS;
}
hi_s32 vpss_wbc_nr_hdown_get_wbc_info(vpss_wbc_nrhdown *wbc, hi_drv_vpss_video_frame **data)
{
    vpss_wbc_nrhdown_data *wbc_node_data = HI_NULL;
    list *wbc_node = HI_NULL;

    if ((wbc == HI_NULL) || (data == HI_NULL)) {
        vpss_error("Vpss nrhdown wbc GetWbcInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_error("Vpss nrhdown wbc GetWbcInfo error(not init).\n");
        return HI_FAILURE;
    }

    wbc_node = wbc->first_ref->prev;
    wbc_node_data = osal_list_entry(wbc_node, vpss_wbc_nrhdown_data, node);
    *data = &(wbc_node_data->wbc_frame);
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nr_hdown_complete(vpss_wbc_nrhdown *wbc)
{
    if (wbc == HI_NULL) {
        vpss_error("Vpss nrhdown wbc complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_error("Vpss nrhdown wbc complete error(not init).\n");
        return HI_FAILURE;
    }

    if ((wbc->complete_count + 1) == 0) {
        wbc->complete_count = VPSS_WBC_NRHDOWN_MAX_NODE;
    }

    wbc->complete_count++;
    wbc->first_ref = wbc->first_ref->next;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_nr_hdown_reset(vpss_wbc_nrhdown *wbc)
{
    if (wbc == HI_NULL) {
        vpss_warn("Vpss nrhdown wbc reset error(null pointer).\n");
        return HI_FAILURE;
    }

    if (wbc->init == HI_FALSE) {
        vpss_warn("Vpss nrhdown wbc reset error(not init).\n");
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


