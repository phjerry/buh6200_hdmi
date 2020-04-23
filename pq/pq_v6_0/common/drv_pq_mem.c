/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include <linux/ion.h>
#include "drv_pq_mem.h"
#include <linux/dma-buf.h>
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEF_MEM_PAGE_SIZE 4096
#define DRV_PQ_MEM_ALIGN(size) ((((size) + (DEF_MEM_PAGE_SIZE - 1)) / DEF_MEM_PAGE_SIZE) * DEF_MEM_PAGE_SIZE);


static dma_addr_t drv_pq_dmabuf_map(osal_mem_type mode, struct dma_buf *dma_buf)
{
    dma_addr_t temp_addr = 0x0;

    if ((mode == OSAL_MMZ_TYPE) || (mode == OSAL_SECMMZ_TYPE)) {
        temp_addr = osal_mem_phys(dma_buf);
    } else if (mode == OSAL_NSSMMU_TYPE) {
        temp_addr = osal_mem_nssmmu_map(dma_buf, 0);
    } else if (mode == OSAL_SECSMMU_TYPE) {
        temp_addr = osal_mem_secsmmu_map(dma_buf, 0);
    }

    if (temp_addr == 0x0) {
        HI_PRINT("map to phy failed, mode = %d\n", mode);
        return 0;
    }

    return temp_addr;
}

static void drv_pq_dmabuf_unmap(osal_mem_type mode, struct dma_buf *dma_buf, dma_addr_t addr)
{
    hi_s32 ret = 0;

    if (mode == OSAL_NSSMMU_TYPE) {
        ret = osal_mem_nssmmu_unmap(dma_buf, addr, 0);
    }

    if (mode == OSAL_SECSMMU_TYPE) {
        ret = osal_mem_secsmmu_unmap(dma_buf, addr, 0);
    }

    if (ret != 0) {
        HI_PRINT(" ummap dmabuf failed ,mode = %d\n", mode);
    }

    return;
}

hi_s32 drv_pq_mem_alloc(const drv_pq_mem_attr *mem_attr, drv_pq_mem_info *mem_info)
{
    drv_pq_mem_info temp_mem_info = { 0 };

    if ((mem_attr == HI_NULL) || (mem_info == HI_NULL)) {
        HI_PRINT("[%s][%d] invalid param, null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (mem_attr->mode >= OSAL_ERROR_TYPE) {
        HI_PRINT("[%s][%d] invalid param, mem mode = %d\n", __FUNCTION__, __LINE__, mem_attr->mode);
        return HI_FAILURE;
    }

    if (mem_attr->size == 0) {
        HI_PRINT("[%s][%d] invalid param, mem size = 0\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    temp_mem_info.mode = mem_attr->mode;
    temp_mem_info.size = DRV_PQ_MEM_ALIGN(mem_attr->size);
    temp_mem_info.dma_buf = osal_mem_alloc(mem_attr->name, temp_mem_info.size, mem_attr->mode, HI_NULL, 0);
    if (temp_mem_info.dma_buf == HI_NULL) {
        HI_PRINT("alloc failed, size = %d, mode = %d, cache = %d\n",
                 temp_mem_info.size, mem_attr->mode, mem_attr->is_cache);

        return HI_FAILURE;
    }

    temp_mem_info.phy_addr = drv_pq_dmabuf_map(mem_attr->mode, temp_mem_info.dma_buf);

    if (temp_mem_info.phy_addr == 0) {
        osal_mem_free(temp_mem_info.dma_buf);
        return HI_FAILURE;
    }

    temp_mem_info.vir_addr = osal_mem_kmap(temp_mem_info.dma_buf, 0, HI_FALSE);

    if (temp_mem_info.vir_addr == HI_NULL) {
        drv_pq_dmabuf_unmap(mem_attr->mode, temp_mem_info.dma_buf, temp_mem_info.phy_addr);
        osal_mem_free(temp_mem_info.dma_buf);
        HI_PRINT("map virt failed, mode = %d\n", mem_attr->mode);
        return HI_FAILURE;
    }

    *mem_info = temp_mem_info;
    return HI_SUCCESS;
}

hi_s32 drv_pq_mem_free(drv_pq_mem_info *mem_info)
{
    if (mem_info == HI_NULL) {
        return HI_FAILURE;
    }

    if (mem_info->dma_buf == HI_NULL) {
        return HI_FAILURE;
    }

    if (mem_info->mode >= OSAL_ERROR_TYPE) {
        return HI_FAILURE;
    }

    if (mem_info->vir_addr != 0) {
        osal_mem_kunmap(mem_info->dma_buf, mem_info->vir_addr, 0);
        mem_info->vir_addr = HI_NULL;
    }

    if (mem_info->phy_addr != 0) {
        drv_pq_dmabuf_unmap(mem_info->mode, mem_info->dma_buf, mem_info->phy_addr);
        mem_info->phy_addr = 0x0;
    }

    osal_mem_free(mem_info->dma_buf);
    mem_info->dma_buf = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 drv_pq_mem_get_fd(struct dma_buf *dma_buf)
{
    hi_s32 temp_fd;

    if (dma_buf == HI_NULL) {
        HI_PRINT("null pointer\n");
        return HI_FAILURE;
    }

    temp_fd = osal_mem_create_fd(dma_buf, O_CLOEXEC);
    if (temp_fd <= 0) {
        HI_PRINT("create fd failed, fd = %d\n", temp_fd);
    }

    return temp_fd;
}

/* user space must give smmu nosafe fd,can't suppot CMA, safe type */
hi_s32 drv_pq_mem_map_fd(struct dma_buf *dma_buf_descp, drv_pq_mem_info *fd_info)
{
    osal_mem_type mem_mode;

    if ((dma_buf_descp == HI_NULL) || (fd_info == HI_NULL)) {
        HI_PRINT("null pointer\n");
        return HI_FAILURE;
    }

    mem_mode = osal_mem_get_attr(dma_buf_descp);
    if (mem_mode == OSAL_ERROR_TYPE) {
        HI_PRINT("error mode type.\n");
        return HI_FAILURE;
    }

    fd_info->phy_addr = drv_pq_dmabuf_map(mem_mode, dma_buf_descp);
    if (fd_info->phy_addr == 0) {
        HI_PRINT("map phy failed.\n");
        return HI_FAILURE;
    }

    if ((mem_mode == OSAL_MMZ_TYPE) || (mem_mode == OSAL_NSSMMU_TYPE)) {
        fd_info->vir_addr = osal_mem_kmap(dma_buf_descp, 0, HI_FALSE);
        if (fd_info->vir_addr == 0) {
            drv_pq_dmabuf_unmap(mem_mode, dma_buf_descp, fd_info->phy_addr);
            HI_PRINT("map virt faild.\n");
            return HI_FAILURE;
        }
    }

    fd_info->size = 0;
    return HI_SUCCESS;
}

hi_s32 drv_pq_mem_unmap_fd(struct dma_buf *dma_buf_descp, drv_pq_mem_info *fd_info)
{
    osal_mem_type mem_mode;

    if ((dma_buf_descp == HI_NULL) || (fd_info == HI_NULL)) {
        HI_PRINT("null pointer\n");
        return HI_FAILURE;
    }

    mem_mode = osal_mem_get_attr(dma_buf_descp);
    if (mem_mode == OSAL_ERROR_TYPE) {
        HI_PRINT("error mode type.\n");
        return HI_FAILURE;
    }

    if (fd_info->phy_addr != 0) {
        drv_pq_dmabuf_unmap(mem_mode, dma_buf_descp, fd_info->phy_addr);
        fd_info->phy_addr = 0x0;
    }
    if ((mem_mode == OSAL_MMZ_TYPE) || (mem_mode == OSAL_NSSMMU_TYPE)) {
        if (fd_info->vir_addr != 0) {
            osal_mem_kunmap(dma_buf_descp, fd_info->vir_addr, 0);
            fd_info->vir_addr = HI_NULL;
        }
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
