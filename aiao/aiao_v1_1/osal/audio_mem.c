/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio memory interface
 * Author: audio
 * Create: 2019-07-05
 * Notes: NA
 * History: 2019-07-05 Initial version
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"
#include "audio_mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef O_CLOEXEC
#define O_CLOEXEC    02000000    /* set close_on_exec */
#endif

static hi_ulong drv_audio_buf_mmap_phys_addr(hi_void *dma_buf, hi_u32 heap_id)
{
    switch (heap_id) {
        case OSAL_MMZ_TYPE:
            return osal_mem_phys(dma_buf);

        case OSAL_NSSMMU_TYPE:
            return osal_mem_nssmmu_map(dma_buf, 0);

        case OSAL_SECSMMU_TYPE:
            return osal_mem_secsmmu_map(dma_buf, 0);
    }

    return 0;
}

static hi_s32 drv_audio_buf_munmap_phys_addr(const hi_audio_buffer *buf, hi_u32 heap_id)
{
    switch (heap_id) {
        case OSAL_NSSMMU_TYPE:
            return osal_mem_nssmmu_unmap(buf->dma_buf, buf->phys_addr, 0);

        case OSAL_SECSMMU_TYPE:
            return osal_mem_secsmmu_unmap(buf->dma_buf, buf->phys_addr, 0);
    }

    return HI_SUCCESS;
}

static hi_s32 drv_audio_get_buf_info(hi_void *dma_buf, hi_u32 heap_id, hi_audio_buffer *buf)
{
    hi_s32 fd;
    hi_u8 *virt_addr = HI_NULL;
    hi_ulong phys_addr;

    virt_addr = (hi_u8 *)osal_mem_kmap(dma_buf, 0, buf->cache);
    if (virt_addr == HI_NULL) {
        HI_LOG_ERR("call osal_mem_kmap failed!\n");
        return HI_FAILURE;
    }

    phys_addr = drv_audio_buf_mmap_phys_addr(dma_buf, heap_id);
    if (phys_addr == 0) {
        HI_LOG_ERR("call drv_audio_buf_mmap_phys_addr failed!\n");
        goto out;
    }

    fd = osal_mem_create_fd(dma_buf, O_CLOEXEC);
    if (fd < 0) {
        HI_LOG_ERR("call osal_mem_create_fd failed!\n");
        goto out;
    }

    osal_mem_flush(dma_buf);

    buf->virt_addr = virt_addr;
    buf->phys_addr = (hi_u64)phys_addr;
    buf->fd = fd;

    return HI_SUCCESS;

out:
    osal_mem_kunmap(dma_buf, virt_addr, 0);
    return HI_FAILURE;
}

static hi_s32 drv_audio_buf_alloc(hi_u32 size, hi_u32 heap_id,
    hi_bool cache, const hi_char *name, hi_audio_buffer *buf)
{
    hi_s32 ret;
    hi_void *dma_buf = HI_NULL;

    if ((name == HI_NULL) || (buf == HI_NULL)) {
        HI_LOG_ERR("invalid param\n");
        return HI_FAILURE;
    }

    dma_buf = osal_mem_alloc(name, size, heap_id, HI_NULL, 0);
    if (dma_buf == HI_NULL) {
        HI_LOG_ERR("call hi_ion_alloc failed!\n");
        return HI_FAILURE;
    }

    buf->dma_buf = (hi_void *)dma_buf;
    buf->size = size;
    buf->cache = cache;

    ret = drv_audio_get_buf_info(dma_buf, heap_id, buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_audio_get_buf_info, ret);
        goto out;
    }

    return HI_SUCCESS;

out:
    osal_mem_free(dma_buf);

    buf->dma_buf = HI_NULL;
    buf->virt_addr = HI_NULL;
    buf->phys_addr = 0;
    buf->size = 0;
    buf->cache = HI_FALSE;
    buf->fd = -1;

    return HI_FAILURE;
}

static hi_void drv_audio_buf_release(const hi_audio_buffer *buf, hi_u32 heap_id)
{
    hi_s32 ret;

    if ((buf == HI_NULL) ||
        (buf->dma_buf == HI_NULL) ||
        (buf->virt_addr == HI_NULL)) {
        HI_LOG_ERR("invalid param\n");
        return;
    }

    ret = drv_audio_buf_munmap_phys_addr(buf, heap_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_audio_buf_munmap_phys_addr, ret);
    }

    if (current->files != HI_NULL) {
        ret = osal_mem_close_fd(buf->fd);
        if ((ret != HI_SUCCESS) && (ret != -EBADF)) {
            HI_ERR_PRINT_FUNC_RES(osal_mem_close_fd, ret);
        }
    }

    osal_mem_kunmap(buf->dma_buf, buf->virt_addr, 0);
    osal_mem_free(buf->dma_buf);
}

hi_s32  hi_drv_audio_mmz_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *mmz_buf)
{
    return drv_audio_buf_alloc(size, OSAL_MMZ_TYPE, cache, name, mmz_buf);
}

hi_void hi_drv_audio_mmz_release(const hi_audio_buffer *mmz_buf)
{
    drv_audio_buf_release(mmz_buf, OSAL_MMZ_TYPE);
}

hi_s32  hi_drv_audio_sec_mmz_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *mmz_buf)
{
    return drv_audio_buf_alloc(size, OSAL_MMZ_TYPE, cache, name, mmz_buf);
}

hi_void hi_drv_audio_sec_mmz_release(const hi_audio_buffer *mmz_buf)
{
    drv_audio_buf_release(mmz_buf, OSAL_MMZ_TYPE);
}

#ifndef HI_SND_DSP_SUPPORT
hi_s32  hi_drv_audio_smmu_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *smmu_buf)
{
    return drv_audio_buf_alloc(size, OSAL_NSSMMU_TYPE, cache, name, smmu_buf);
}

hi_void hi_drv_audio_smmu_release(const hi_audio_buffer *smmu_buf)
{
    drv_audio_buf_release(smmu_buf, OSAL_NSSMMU_TYPE);
}

hi_s32  hi_drv_audio_sec_smmu_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *smmu_buf)
{
    return drv_audio_buf_alloc(size, OSAL_SECSMMU_TYPE, cache, name, smmu_buf);
}

hi_void hi_drv_audio_sec_smmu_release(const hi_audio_buffer *smmu_buf)
{
    drv_audio_buf_release(smmu_buf, OSAL_SECSMMU_TYPE);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
