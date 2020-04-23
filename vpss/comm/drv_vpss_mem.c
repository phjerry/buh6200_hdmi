/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_osal.h"

#include "hi_debug.h"
#include "hi_errno.h"
#include <linux/ion.h>
#include "drv_vpss_mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_MEM_PAGE_SIZE  128
#define vpss_mem_align(size) ((((size) + (VPSS_MEM_PAGE_SIZE - 1)) / VPSS_MEM_PAGE_SIZE) * VPSS_MEM_PAGE_SIZE);

static dma_addr_t vpss_map_dmabuf_to_phys(osal_mem_type mode, struct dma_buf *dma_handle)
{
    dma_addr_t temp_addr = 0x0;

    if ((mode == OSAL_MMZ_TYPE) || (mode == OSAL_SECMMZ_TYPE)) {
        temp_addr = osal_mem_phys(dma_handle);
    } else if (mode == OSAL_NSSMMU_TYPE) {
        temp_addr = osal_mem_nssmmu_map(dma_handle, 0);
    } else if (mode == OSAL_SECSMMU_TYPE) {
        temp_addr = osal_mem_secsmmu_map(dma_handle, 0);
    }

    if (temp_addr == 0x0) {
        osal_printk("[%s] dmabuf_map failed ,mode = %d\n", __FUNCTION__, mode);
        return 0;
    }

    return temp_addr;
}

static void vpss_unmap_dmabuf_from_phys(osal_mem_type mode, struct dma_buf *dma_handle, dma_addr_t addr)
{
    hi_s32 ret = 0;

    if (mode == OSAL_NSSMMU_TYPE) {
        ret = osal_mem_nssmmu_unmap(dma_handle, addr, 0);
    }

    if (mode == OSAL_SECSMMU_TYPE) {
        ret = osal_mem_secsmmu_unmap(dma_handle, addr, 0);
    }

    if (ret != 0) {
        osal_printk("[%s] dmabuf_unmap failed ,mode = %d\n", __FUNCTION__, mode);
    }

    return;
}

hi_s32 drv_vpss_mem_alloc(const drv_vpss_mem_attr *mem_attr, drv_vpss_mem_info *mem_info)
{
    drv_vpss_mem_info tmp_mem = { 0 };

    if ((mem_attr == HI_NULL) || (mem_info == HI_NULL)) {
        osal_printk("null pointer\n");
        return HI_FAILURE;
    }

    if (mem_attr->mode >= OSAL_ERROR_TYPE) {
        osal_printk(" ummap dmabuf failed ,mode = %d\n",  mem_attr->mode);
        return HI_FAILURE;
    }

    if (mem_attr->size == 0) {
        osal_printk("invalid param, mem size = %d\n", mem_attr->size);
        return HI_FAILURE;
    }

    tmp_mem.mode = mem_attr->mode;
    tmp_mem.size = vpss_mem_align(mem_attr->size);
    tmp_mem.dma_handle = osal_mem_alloc(mem_attr->name, tmp_mem.size, mem_attr->mode, HI_NULL, 0);
    if (tmp_mem.dma_handle == HI_NULL) {
        osal_printk("alloc failed, size = %d, mode = %d, cache = %d\n",
                    tmp_mem.size, mem_attr->mode, mem_attr->is_cache);

        return HI_FAILURE;
    }

    tmp_mem.phy_addr = vpss_map_dmabuf_to_phys(mem_attr->mode, tmp_mem.dma_handle);
    if (tmp_mem.phy_addr == 0) {
        osal_mem_free(tmp_mem.dma_handle);
        osal_printk("map phys failed, size = %d\n", tmp_mem.size);
        return HI_FAILURE;
    }

    if (mem_attr->is_map_viraddr == HI_TRUE) {
        tmp_mem.vir_addr = osal_mem_kmap(tmp_mem.dma_handle, 0, HI_FALSE);
        if (tmp_mem.vir_addr == HI_NULL) {
            vpss_unmap_dmabuf_from_phys(mem_attr->mode, tmp_mem.dma_handle, tmp_mem.phy_addr);
            osal_mem_free(tmp_mem.dma_handle);
            osal_printk("map virt failed, mode = %d\n", mem_attr->mode);
            return HI_FAILURE;
        }
    }

    osal_mem_flush(tmp_mem.dma_handle);
    *mem_info = tmp_mem;

    return HI_SUCCESS;
}

hi_s32 drv_vpss_mem_free(drv_vpss_mem_info *mem_info)
{
    if (mem_info == HI_NULL) {
        osal_printk("null pointer\n");
        return HI_FAILURE;
    }

    if (mem_info->dma_handle == HI_NULL) {
        osal_printk("null pointer\n");
        return HI_FAILURE;
    }

    if (mem_info->mode >= OSAL_ERROR_TYPE) {
        osal_printk("invalid mode:%d \n", mem_info->mode);
        return HI_FAILURE;
    }

    if (mem_info->vir_addr != 0) {
        osal_mem_kunmap(mem_info->dma_handle, mem_info->vir_addr, 0);
        mem_info->vir_addr = HI_NULL;
    }

    if (mem_info->phy_addr != 0) {
        vpss_unmap_dmabuf_from_phys(mem_info->mode, mem_info->dma_handle, mem_info->phy_addr);
        mem_info->phy_addr = 0x0;
    }

    osal_mem_free(mem_info->dma_handle);
    mem_info->dma_handle = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 drv_vpss_mem_increase_refcnt(struct dma_buf *dma_handle)
{
    osal_mem_ref_get(dma_handle, HI_ID_VPSS);

    return HI_SUCCESS;
}

hi_s32 drv_vpss_mem_decrease_refcnt(struct dma_buf *dma_handle)
{
    osal_mem_ref_put(dma_handle, HI_ID_VPSS);

    return HI_SUCCESS;
}

/* user space must give smmu nosafe fd,can't suppot CMA, safe type */
hi_s32 drv_vpss_mem_map_dma_handle(struct dma_buf *dma_handle, drv_vpss_mem_info *mem_info)
{
    osal_mem_type mem_mode;

    if ((dma_handle == HI_NULL) || (mem_info == HI_NULL)) {
        osal_printk("null pointer\n");
        return HI_FAILURE;
    }

    mem_mode  = osal_mem_get_attr(dma_handle);
    if (mem_mode == OSAL_ERROR_TYPE) {
        osal_printk("error mode type.\n");
        return HI_FAILURE;
    }

    mem_info->phy_addr = vpss_map_dmabuf_to_phys(mem_mode, dma_handle);
    if (mem_info->phy_addr == 0) {
        osal_printk("map phy failed.\n");
        return HI_FAILURE;
    }

    if ((mem_mode == OSAL_MMZ_TYPE) || (mem_mode == OSAL_NSSMMU_TYPE)) {
        mem_info->vir_addr = osal_mem_kmap(dma_handle, 0, HI_FALSE);
        if (mem_info->vir_addr == 0) {
            vpss_unmap_dmabuf_from_phys(mem_mode, dma_handle, mem_info->phy_addr);
            osal_printk("map virt faild.\n");
            return HI_FAILURE;
        }
    }

    mem_info->size = 0;

    return HI_SUCCESS;
}

hi_s32 drv_vpss_mem_unmap_dma_handle(vpss_dma_para_umap *dma_info)
{
    osal_mem_type mem_mode;

    if ((dma_info == HI_NULL) || (dma_info->dma_handle == HI_NULL)) {
        osal_printk("null pointer\n");
        return HI_FAILURE;
    }

    mem_mode  = osal_mem_get_attr(dma_info->dma_handle);
    if (mem_mode == OSAL_ERROR_TYPE) {
        osal_printk("error mode type.\n");
        return HI_FAILURE;
    }

    if (dma_info->phy_addr != 0) {
        vpss_unmap_dmabuf_from_phys(mem_mode, dma_info->dma_handle, dma_info->phy_addr);
        dma_info->phy_addr = 0x0;
    }

    if ((mem_mode == OSAL_MMZ_TYPE) || (mem_mode == OSAL_NSSMMU_TYPE)) {
        if (dma_info->vir_addr != 0) {
            osal_mem_kunmap(dma_info->dma_handle, (hi_void *)dma_info->vir_addr, 0);
            dma_info->vir_addr = HI_NULL;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_vpss_mem_map_vir_addr(struct dma_buf *dma_handle, drv_vpss_mem_info *mem_info)
{
    osal_mem_type mem_mode;

    if ((dma_handle == HI_NULL) || (mem_info == HI_NULL)) {
        osal_printk("null pointer\n");
        return HI_FAILURE;
    }

    mem_mode  = osal_mem_get_attr(dma_handle);
    if (mem_mode == OSAL_ERROR_TYPE) {
        osal_printk("error mode type.\n");
        return HI_FAILURE;
    }

    if ((mem_mode == OSAL_MMZ_TYPE) || (mem_mode == OSAL_NSSMMU_TYPE)) {
        mem_info->vir_addr = osal_mem_kmap(dma_handle, 0, HI_FALSE);
        if (mem_info->vir_addr == 0) {
            osal_printk("map virt faild.\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_vpss_mem_unmap_vir_addr(struct dma_buf *dma_handle, hi_u8 *vir_addr)
{
    if ((dma_handle == HI_NULL) || (vir_addr == HI_NULL)) {
        osal_printk("[%s] invalid param, null pointer\n", __FUNCTION__);
        return HI_FAILURE;
    }

    if (vir_addr != 0) {
        osal_mem_kunmap(dma_handle, vir_addr, 0);
        vir_addr = HI_NULL;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
