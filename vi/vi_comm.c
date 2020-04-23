/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi comm
 * Author: sdk
 * Create: 2019-12-14
 */

#include <linux/types.h>
#include <linux/ion.h>
#include <linux/dma-buf.h>

#include "vi_comm.h"

hi_s32 vi_comm_pq_get_csc_coef(hi_drv_pq_csc_info csc_info, hi_drv_pq_csc_coef *csc_coef_p)
{
#ifdef VI_USE_PQ
    hi_s32 ret;
    pq_export_func *pq_ext_func_p = HI_NULL;

    if (hi_drv_module_get_func(HI_ID_PQ, (hi_void **)&(pq_ext_func_p))) {
        vi_drv_log_err("get function from pq failed.\n");
        return HI_FAILURE;
    }

    if (pq_ext_func_p == HI_NULL) {
        vi_drv_log_err("null point.\n");
        return HI_FAILURE;
    }

    ret = pq_ext_func_p->pq_get_vdp_csc_coef(HI_DRV_PQ_CSC_LAYER_VPSS, csc_info, csc_coef_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
#else
    return HI_SUCCESS;
#endif
}

hi_s32 vi_comm_get_y_stride(hi_u32 width, hi_drv_pixel_bitwidth bit_width, hi_u32 *stride_p)
{
    if (bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
        *stride_p = vi_align_stride_y_8bit(width);
    } else if (bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
        *stride_p = vi_align_stride_y_10bit(width);
    } else if (bit_width == HI_DRV_PIXEL_BITWIDTH_12BIT) {
        *stride_p = vi_align_stride_y_12bit(width);
    } else {
        vi_drv_log_err("vicap only support 8/10/12 bit!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vi_comm_get_c_stride(hi_u32 y_stride, hi_drv_pixel_format pix_fmt, hi_u32 *cstride_p)
{
    if ((pix_fmt == HI_DRV_PIXEL_FMT_NV42_RGB) || (pix_fmt == HI_DRV_PIXEL_FMT_NV42)) {
        *cstride_p = y_stride * 2; /* rbg c stride is y * 2 */
    } else if (pix_fmt == HI_DRV_PIXEL_FMT_NV61_2X1) {
        *cstride_p = y_stride;
    } else if (pix_fmt == HI_DRV_PIXEL_FMT_NV21) {
        *cstride_p = y_stride;
    } else {
        vi_drv_log_err("invalid pixel format(%d)!\n", pix_fmt);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vi_comm_alloc(const char *buf_name, vi_buffer_addr_info *buf_p)
{
    osal_mem_type mem_type;

    if (buf_name == HI_NULL || buf_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    if (buf_p->alloc_type == VI_BUFFER_ALLOC_TYPE_MMZ) {
        mem_type = OSAL_MMZ_TYPE;
    } else {
        mem_type = OSAL_NSSMMU_TYPE;
    }

    buf_p->buf_obj = osal_mem_alloc(buf_name, buf_p->size, mem_type, HI_NULL, 0);
    if (buf_p->buf_obj == HI_NULL) {
        vi_drv_log_err("osal alloc failed\n");
        goto out0;
    }

    buf_p->start_phy_addr = 0x0;
    if ((mem_type == OSAL_MMZ_TYPE) || (mem_type == OSAL_SECMMZ_TYPE)) {
        buf_p->start_phy_addr = osal_mem_phys(buf_p->buf_obj);
    } else if (mem_type == OSAL_NSSMMU_TYPE) {
        buf_p->start_phy_addr = osal_mem_nssmmu_map(buf_p->buf_obj, 0);
    } else if (mem_type == OSAL_SECSMMU_TYPE) {
        buf_p->start_phy_addr = osal_mem_secsmmu_map(buf_p->buf_obj, 0);
    }
    if (buf_p->start_phy_addr == 0x0) {
        vi_drv_log_err("phys failed\n");
        goto out1;
    }

    buf_p->buf_handle = osal_mem_create_fd(buf_p->buf_obj, O_CLOEXEC);
    if (buf_p->buf_handle <= 0) {
        vi_drv_log_err("create fd failed, fd = %d\n", buf_p->buf_handle);
        goto out2;
    }

    buf_p->start_vir_addr_p = HI_NULL;

    return HI_SUCCESS;

out2:
    if (mem_type == OSAL_NSSMMU_TYPE) {
        osal_mem_nssmmu_unmap(buf_p->buf_obj, buf_p->start_phy_addr, 0);
    } else if (mem_type == OSAL_SECSMMU_TYPE) {
        osal_mem_secsmmu_unmap(buf_p->buf_obj, buf_p->start_phy_addr, 0);
    }
out1:
    osal_mem_free(buf_p->buf_obj);
out0:
    return HI_FAILURE;
}

hi_void vi_comm_release(vi_buffer_addr_info *buf_p)
{
    hi_s32 ret = HI_SUCCESS;
    osal_mem_type mem_type;

    if (buf_p == HI_NULL || buf_p->buf_obj == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return;
    }

    if (buf_p->alloc_type == VI_BUFFER_ALLOC_TYPE_MMZ) {
        mem_type = OSAL_MMZ_TYPE;
    } else {
        mem_type = OSAL_NSSMMU_TYPE;
    }

    if (buf_p->start_vir_addr_p != HI_NULL) {
        osal_mem_kunmap(buf_p->buf_obj, buf_p->start_vir_addr_p, 0);
        buf_p->start_vir_addr_p = HI_NULL;
    }

    if (mem_type == OSAL_NSSMMU_TYPE) {
        ret = osal_mem_nssmmu_unmap(buf_p->buf_obj, buf_p->start_phy_addr, 0);
    } else if (mem_type == OSAL_SECSMMU_TYPE) {
        ret = osal_mem_secsmmu_unmap(buf_p->buf_obj, buf_p->start_phy_addr, 0);
    }
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("ummap dmabuf failed, mode = %d\n", mem_type);
    }

    if (buf_p->buf_handle > 0) {
        if (current->files != HI_NULL) {
            ret = osal_mem_close_fd(buf_p->buf_handle);
            if (ret != HI_SUCCESS && ret != -EBADF) {
                vi_drv_log_err("close fd fail\n");
            }
        } else {
            vi_drv_log_warn("fd closed\n");
        }
        buf_p->buf_handle = 0;
    }

    osal_mem_free(buf_p->buf_obj);
    buf_p->start_phy_addr = 0x0;
    buf_p->buf_obj = HI_NULL;
}

hi_s32 vi_comm_mmap(vi_buffer_addr_info *buf_p)
{
    if (buf_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    buf_p->start_vir_addr_p = osal_mem_kmap(buf_p->buf_obj, 0, HI_FALSE);
    if (buf_p->start_vir_addr_p == HI_NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void vi_comm_unmap(vi_buffer_addr_info *buf_p)
{
    if (buf_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return;
    }

    if (buf_p->start_vir_addr_p != HI_NULL) {
        osal_mem_kunmap(buf_p->buf_obj, buf_p->start_vir_addr_p, 0);
        buf_p->start_vir_addr_p = HI_NULL;
    }
}

hi_s32 vi_comm_query_phy_addr(hi_mem_handle_t phy_addr)
{
    hi_void *buf_obj = HI_NULL;

    buf_obj = osal_mem_handle_get(phy_addr, HI_ID_VI);
    if (buf_obj == HI_NULL) {
        vi_drv_log_err("not fount phy addr(%d)\n", phy_addr);
        return HI_FAILURE;
    } else {
        osal_mem_ref_put(buf_obj, HI_ID_VI);
    }

    return HI_SUCCESS;
}
