/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */
#include "omxvdec.h"
#include "omxvdec_mem.h"
#include "linux/hisilicon/securec.h"
#ifdef HI_OMX_TEE_SUPPORT
#include "hi_drv_ssm.h"
#endif

#define OMXVDEC_MEM_ASSERT_RET(cond, ret)                            \
    do {                                                           \
        if (!(cond)) {                                             \
            omx_print(OMX_ERR, "Assert Warning: condition %s not match.\n", #cond); \
            return ret;                                            \
        }                                                          \
    } while (0)

#define OMXVDEC_MEM_ASSERT(cond)                            \
    do {                                                           \
        if (!(cond)) {                                             \
            omx_print(OMX_ERR, "Assert Warning: condition %s not match.\n", #cond); \
            return;                                            \
        }                                                          \
    } while (0)

#ifdef HI_SMMU_SUPPORT
osal_mem_type omxvdec_get_mem_type(omxvdec_mem_mode mode)
{
    osal_mem_type type;

    switch (mode) {
        case OMX_MEM_CMA_CMA:
        case OMX_MEM_MMU_MMU:
            type = OSAL_NSSMMU_TYPE;
            break;
        case OMX_MEM_CMA_SEC:
        case OMX_MEM_MMU_SEC:
            type = OSAL_SECSMMU_TYPE;
            break;
        default:
            type = OSAL_ERROR_TYPE;
            break;
    }

    return type;
}
#else
osal_mem_type omxvdec_get_mem_type(omxvdec_mem_mode mode)
{
    osal_mem_type type;

    switch (mode) {
        case OMX_MEM_CMA_CMA:
        case OMX_MEM_MMU_MMU:
            type = OSAL_MMZ_TYPE;
            break;
        case OMX_MEM_CMA_SEC:
        case OMX_MEM_MMU_SEC:
            type = OSAL_SECMMZ_TYPE;
            break;
        default:
            type = OSAL_ERROR_TYPE;
            break;
    }

    return type;
}
#endif

/* ============== EXPORT FUNCTION ============= */
hi_s32 hi_drv_omxvdec_alloc(const char *buf_name, hi_u32 len, hi_u32 align, omxvdec_buffer *omx_buf)
{
    osal_mem_type type;
    hi_u32 sec = 0;
    hi_s32 fd;
    hi_void *priv_data = HI_NULL;
    size_t priv_len = 0;
    hi_void *buf = HI_NULL;
    UADDR phy_addr;
#ifdef HI_OMX_TEE_SUPPORT
    hi_tee_ssm_buf_attach_pre_params attach_param = {0};
#endif

    OMXVDEC_MEM_ASSERT_RET(len != 0, HI_FAILURE);
    OMXVDEC_MEM_ASSERT_RET(omx_buf != HI_NULL, HI_FAILURE);

    type = omxvdec_get_mem_type(omx_buf->mode);

#ifdef HI_OMX_TEE_SUPPORT
    if (omx_buf->mode == OMX_MEM_CMA_SEC ||
        omx_buf->mode == OMX_MEM_MMU_SEC) {
        sec = 1;
        priv_data = &attach_param;
        priv_len = sizeof(attach_param);
    }

    if (sec == 1) {
        attach_param.buf_id = HI_SSM_BUFFER_ID_INTERNAL_BUF_VDEC;
    }
#endif

    /* ion_alloc() will align page size */
    buf = osal_mem_alloc(buf_name, len, type, priv_data, priv_len);
    if (buf == HI_NULL) {
        omx_print(OMX_ERR, "hi_ion_alloc fail\n");
        return HI_FAILURE;
    }

#ifdef HI_SMMU_SUPPORT
    if (sec == 0) {
        phy_addr = (UADDR)osal_mem_nssmmu_map(buf, 0);
    } else {
        phy_addr = (UADDR)osal_mem_secsmmu_map(buf, 0);
    }
#else
    if (sec == 0) {
        phy_addr = (UADDR)osal_mem_phys(buf);
    }
#endif

    osal_mem_flush(buf);

    omx_buf->phys_addr = (UADDR)phy_addr;
    omx_buf->size = len;
    omx_buf->dma_buf = PTR_UINT64(buf);
    fd = osal_mem_create_fd(buf, OSAL_O_CLOEXEC);
    if (fd < 0) {
        omx_print(OMX_ERR, "get dma buf fd fail\n");
    }

    omx_buf->fd = fd;

    return HI_SUCCESS;
}


hi_void hi_drv_omxvdec_free(omxvdec_buffer *omx_buf, omx_mem_free_type free_type)
{
    hi_void *buf = HI_NULL;
    hi_u32 smmu = 0;
    hi_u32 sec = 0;

    OMXVDEC_MEM_ASSERT(omx_buf != HI_NULL);
    OMXVDEC_MEM_ASSERT(omx_buf->fd != 0);

    buf = (hi_void *)UINT64_PTR(omx_buf->dma_buf);

    if (omx_buf->virt_addr != HI_NULL) {
        osal_mem_kunmap(buf, omx_buf->virt_addr, 0);
    }

#ifdef HI_SMMU_SUPPORT
    smmu = 1;
#endif

#ifdef HI_OMX_TEE_SUPPORT
    if (omx_buf->mode == OMX_MEM_CMA_SEC ||
        omx_buf->mode == OMX_MEM_MMU_SEC) {
        sec = 1;
    }
#endif

    if (smmu == 1 && sec == 0) {
        osal_mem_nssmmu_unmap(buf, 0, omx_buf->phys_addr);
    } else if (smmu == 1 && sec == 1) {
        osal_mem_secsmmu_unmap(buf, 0, omx_buf->phys_addr);
    }

    if (free_type == OMX_MEM_FREE_TYPE_NORMAL) {
        (hi_void)osal_mem_close_fd(omx_buf->fd);
    }

    osal_mem_free(buf);
}

hi_s32 hi_drv_omxvdec_map(omxvdec_buffer *omx_buf, hi_u32 cache)
{
    hi_void *vir_addr = HI_NULL;
    hi_void *buf = HI_NULL;

    OMXVDEC_MEM_ASSERT_RET(omx_buf != HI_NULL, HI_FAILURE);
    OMXVDEC_MEM_ASSERT_RET(omx_buf->dma_buf != 0, HI_FAILURE);

    buf = (hi_void *)UINT64_PTR(omx_buf->dma_buf);
    vir_addr = osal_mem_kmap(buf, 0, cache);

    omx_buf->virt_addr = (hi_u8 *)vir_addr;

    return HI_SUCCESS;
}

hi_void hi_drv_omxvdec_unmap(omxvdec_buffer *omx_buf)
{
    hi_void *buf = HI_NULL;

    OMXVDEC_MEM_ASSERT(omx_buf != HI_NULL);
    OMXVDEC_MEM_ASSERT(omx_buf->dma_buf != 0);

    buf = (hi_void *)PTR_UINT64(omx_buf->dma_buf);
    osal_mem_kunmap(buf, omx_buf->virt_addr, 0);
}

hi_s32 hi_drv_omxvdec_alloc_and_map(const char *buf_name, hi_u32 cache, hi_u32 len, hi_u32 align,
    omxvdec_buffer *omx_buf)
{
    hi_s32 ret;

    ret = hi_drv_omxvdec_alloc(buf_name, len, align, omx_buf);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_ERR, "hi_drv_omxvdec_alloc fail ret = %d\n", ret);
        return ret;
    }

    ret = hi_drv_omxvdec_map(omx_buf, cache);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_ERR, "hi_drv_omxvdec_map fail ret = %d\n", ret);
        hi_drv_omxvdec_free(omx_buf, OMX_MEM_FREE_TYPE_NORMAL);
    }

    return ret;
}

hi_s32 hi_drv_omxvdec_unmap_and_free(omxvdec_buffer *omx_buf, omx_mem_free_type free_type)
{
    OMXVDEC_MEM_ASSERT_RET(omx_buf != HI_NULL, HI_FAILURE);
    OMXVDEC_MEM_ASSERT_RET(omx_buf->fd != 0, HI_FAILURE);

    hi_drv_omxvdec_unmap(omx_buf);

    hi_drv_omxvdec_free(omx_buf, free_type);

    return HI_SUCCESS;
}
