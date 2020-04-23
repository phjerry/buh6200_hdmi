/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hi_debug.h"
#include <linux/ion.h>
#include "drv_xdp_ion.h"
#include "drv_xdp_osal.h"
#include <linux/dma-buf.h>
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEF_MEM_PAGE_SIZE      4096
#define HI_DRV_MEM_ALIGN(size) ((((size) + (DEF_MEM_PAGE_SIZE - 1)) / DEF_MEM_PAGE_SIZE) * DEF_MEM_PAGE_SIZE);

hi_u32 g_ref_increase_cnt = 0;
hi_u32 g_ref_decrease_cnt = 0;

static dma_addr_t map_dmabuf_to_phys(osal_mem_type mode, struct dma_buf *dma_buf)
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
        DISP_ERROR("map to phy failed, mode = %d\n",mode);
        return 0;
    }

    return temp_addr;
}

static  void unmap_dmabuf_from_phys(osal_mem_type mode, struct dma_buf *dma_buf, dma_addr_t addr)
{
    hi_s32 ret = 0;

    if (mode == OSAL_NSSMMU_TYPE) {
        ret = osal_mem_nssmmu_unmap(dma_buf, addr, 0);
    }

    if (mode == OSAL_SECSMMU_TYPE) {
        ret = osal_mem_secsmmu_unmap(dma_buf, addr, 0);
    }

    if (ret != 0) {
        DISP_ERROR(" ummap dmabuf failed ,mode = %d\n",  mode);
    }

    return;
}

hi_s32 drv_xdp_mem_alloc(const drv_xdp_mem_attr *mem_attr, drv_xdp_mem_info *mem_info)
{
    drv_xdp_mem_info temp_mem_info = { 0 };

    if ((mem_attr == HI_NULL) || (mem_info == HI_NULL)) {
        DISP_ERROR("null pointer\n");
        return HI_FAILURE;
    }

    if (mem_attr->mode >= OSAL_ERROR_TYPE) {
        DISP_ERROR(" ummap dmabuf failed ,mode = %d\n",  mem_attr->mode);
        return HI_FAILURE;
    }

    if (mem_attr->size == 0) {
        DISP_ERROR("invalid param, mem size = 0\n", mem_attr->size);
        return HI_FAILURE;
    }

    temp_mem_info.mode = mem_attr->mode;
    temp_mem_info.size = HI_DRV_MEM_ALIGN(mem_attr->size);

    temp_mem_info.dma_buf = osal_mem_alloc(mem_attr->name, temp_mem_info.size,
                                           mem_attr->mode, HI_NULL, 0);
    if (temp_mem_info.dma_buf == HI_NULL) {
        DISP_ERROR("alloc failed, size = %d, mode = %d, cache = %d\n",
                   temp_mem_info.size, mem_attr->mode, mem_attr->is_cache);

        return HI_FAILURE;
    }

    temp_mem_info.phy_addr = map_dmabuf_to_phys(mem_attr->mode, temp_mem_info.dma_buf);
    if (temp_mem_info.phy_addr == 0) {
        osal_mem_free(temp_mem_info.dma_buf);
        DISP_ERROR("map phys failed, size = %d\n", temp_mem_info.size);
        return HI_FAILURE;
    }

    temp_mem_info.vir_addr = osal_mem_kmap(temp_mem_info.dma_buf, 0, HI_FALSE);
    if (temp_mem_info.vir_addr == HI_NULL) {
        unmap_dmabuf_from_phys(mem_attr->mode, temp_mem_info.dma_buf, temp_mem_info.phy_addr);
        osal_mem_free(temp_mem_info.dma_buf);
        DISP_ERROR("map virt failed, mode = %d\n", mem_attr->mode);
        return HI_FAILURE;
    }

    osal_mem_flush(temp_mem_info.dma_buf);
    *mem_info = temp_mem_info;
    return HI_SUCCESS;
}

hi_s32 drv_xdp_mem_free(drv_xdp_mem_info *mem_info)
{
    if (mem_info == HI_NULL) {
        DISP_ERROR("null pointer\n");
        return HI_FAILURE;
    }
    if (mem_info->dma_buf == HI_NULL) {
        DISP_ERROR("null pointer\n");
        return HI_FAILURE;
    }

    if (mem_info->mode >= OSAL_ERROR_TYPE) {
        DISP_ERROR("invalid mode:%d \n",mem_info->mode);
        return HI_FAILURE;
    }

    if (mem_info->vir_addr != 0) {
        osal_mem_kunmap(mem_info->dma_buf, mem_info->vir_addr, 0);
        mem_info->vir_addr = HI_NULL;
    }

    if (mem_info->phy_addr != 0) {
        unmap_dmabuf_from_phys(mem_info->mode, mem_info->dma_buf, mem_info->phy_addr);
        mem_info->phy_addr = 0x0;
    }

    osal_mem_free(mem_info->dma_buf);
    mem_info->dma_buf = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 drv_xdp_mem_get_fd(struct dma_buf *dma_buf)
{
    hi_s32 temp_fd;

    if (dma_buf == HI_NULL) {
        DISP_ERROR("null pointer\n");
        return HI_FAILURE;
    }

    temp_fd = osal_mem_create_fd(dma_buf, O_CLOEXEC);
    if (temp_fd <= 0) {
        DISP_ERROR("create fd failed, fd = %d\n", temp_fd);
    }

    return temp_fd;
}

hi_void drv_xdp_mem_close_fd(int fd)
{
    (hi_void)osal_mem_close_fd(fd);
    return;
}

/* ref cnt will increase, should call drv_xdp_mem_decrease_refcnt. */
struct dma_buf *drv_xdp_mem_get_dmabuf(int fd)
{
    struct dma_buf *tmp_dma_buf = HI_NULL;
    g_ref_increase_cnt++;

    tmp_dma_buf = osal_mem_handle_get(fd, HI_ID_WIN);
    if (tmp_dma_buf == HI_NULL) {
        DISP_ERROR("get dmabuf failed, fd = %d\n", fd);
        return HI_NULL;
    }

    return tmp_dma_buf;
}


hi_s32 drv_xdp_mem_increase_refcnt(struct dma_buf *dma_buf)
{
    g_ref_increase_cnt++;
    osal_mem_ref_get(dma_buf, HI_ID_WIN);
    return HI_SUCCESS;
}

hi_s32 drv_xdp_mem_decrease_refcnt(struct dma_buf *dma_buf)
{
    g_ref_decrease_cnt++;
    osal_mem_ref_put(dma_buf, HI_ID_WIN);
    return HI_SUCCESS;
}

hi_s32 drv_xdp_mem_map_dma_handle(struct dma_buf *dma_buf_descp,
                                  drv_xdp_mem_info *fd_info)
{
    osal_mem_type mem_mode;

    if ((dma_buf_descp == HI_NULL) || (fd_info == HI_NULL)) {
        DISP_ERROR("null pointer\n");
        return HI_FAILURE;
    }

    mem_mode  = osal_mem_get_attr(dma_buf_descp);
    if (mem_mode == OSAL_ERROR_TYPE) {
        DISP_ERROR("error mode type.\n");
        return HI_FAILURE;
    }

    fd_info->phy_addr = map_dmabuf_to_phys(mem_mode, dma_buf_descp);
    if (fd_info->phy_addr == 0) {
        DISP_ERROR("map phy failed.\n");
        return HI_FAILURE;
    }

    if ((mem_mode == OSAL_MMZ_TYPE)
        ||(mem_mode == OSAL_NSSMMU_TYPE))
    {
        fd_info->vir_addr = osal_mem_kmap(dma_buf_descp, 0, HI_FALSE);
        if (fd_info->vir_addr == 0) {
            unmap_dmabuf_from_phys(mem_mode, dma_buf_descp, fd_info->phy_addr);
            DISP_ERROR("map virt faild.\n");
            return HI_FAILURE;
        }
    }

    fd_info->size = 0;
    return HI_SUCCESS;
}

hi_s32 drv_xdp_mem_unmap_dma_handle(struct dma_buf *dma_buf_descp,
                                    drv_xdp_mem_info *fd_info)
{
    osal_mem_type mem_mode;

    if ((dma_buf_descp == HI_NULL) || (fd_info == HI_NULL)) {
        DISP_ERROR("null pointer\n");
        return HI_FAILURE;
    }

    mem_mode  = osal_mem_get_attr(dma_buf_descp);
    if (mem_mode == OSAL_ERROR_TYPE) {
        DISP_ERROR("error mode type.\n");
        return HI_FAILURE;
    }

    if (fd_info->phy_addr != 0) {
        unmap_dmabuf_from_phys(mem_mode, dma_buf_descp, fd_info->phy_addr);
        fd_info->phy_addr = 0x0;
    }
    if ((mem_mode == OSAL_MMZ_TYPE)
        || (mem_mode == OSAL_NSSMMU_TYPE)) {

        if (fd_info->vir_addr != 0) {
            osal_mem_kunmap(dma_buf_descp, fd_info->vir_addr, 0);
            fd_info->vir_addr = HI_NULL;
        }
    }

    return HI_SUCCESS;
}

hi_void drv_xdp_mem_get_refcnt(hi_u32 *ref_increase_cnt, hi_u32 *ref_decrease_cnt)
{
    *ref_increase_cnt = g_ref_increase_cnt;
    *ref_decrease_cnt = g_ref_decrease_cnt;
    return;
}


hi_void drv_xdp_mem_reset_refcnt(hi_void)
{
    g_ref_increase_cnt = 0;
    g_ref_decrease_cnt = 0;
    return;
}

hi_void drv_xdp_mem_invalid(const drv_xdp_mem_info *mem_info, hi_bool is_cache)
{
    if (is_cache == HI_TRUE) {
        osal_mem_flush(mem_info->dma_buf);
    }

    return;
}

hi_void drv_xdp_mem_flush(const drv_xdp_mem_info *mem_info, hi_bool is_cache)
{
    int ret;

    if (is_cache != HI_TRUE) {
        return;
    }

    if (mem_info->dma_buf == HI_NULL) {
        DISP_ERROR(" null dma buf.\n");
        return;
    }

    ret = dma_buf_end_cpu_access(mem_info->dma_buf, DMA_TO_DEVICE);
    if (ret) {
        DISP_ERROR("flush failed .\n");
    }

    return;
}

EXPORT_SYMBOL(drv_xdp_mem_invalid);
EXPORT_SYMBOL(drv_xdp_mem_flush);
EXPORT_SYMBOL(drv_xdp_mem_alloc);
EXPORT_SYMBOL(drv_xdp_mem_free);
EXPORT_SYMBOL(drv_xdp_mem_get_fd);
EXPORT_SYMBOL(drv_xdp_mem_close_fd);
EXPORT_SYMBOL(drv_xdp_mem_get_dmabuf);
EXPORT_SYMBOL(drv_xdp_mem_increase_refcnt);
EXPORT_SYMBOL(drv_xdp_mem_decrease_refcnt);
EXPORT_SYMBOL(drv_xdp_mem_map_dma_handle);
EXPORT_SYMBOL(drv_xdp_mem_unmap_dma_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
