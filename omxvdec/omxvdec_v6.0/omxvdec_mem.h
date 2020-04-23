/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */


#ifndef __OMXVDEC_MEM_H__
#define __OMXVDEC_MEM_H__

#define MAX_OMXVDEC_BUFFER (100 * 1024 * 1024)

typedef enum {
    OMX_MEM_CMA_CMA = 0,
    OMX_MEM_CMA_SEC,
    OMX_MEM_MMU_MMU,
    OMX_MEM_MMU_SEC,
} omxvdec_mem_mode;

typedef struct {
    hi_s32 fd;
    hi_u64 dma_buf;
    hi_u8  *virt_addr;
    UADDR  phys_addr;    /* PhyAddr or SmmuAddr */
    hi_u32 size;
    omxvdec_mem_mode mode;
    hi_handle ssm_handle;
    hi_handle vdec_handle;
} omxvdec_buffer;

typedef enum {
    OMX_MEM_FREE_TYPE_NORMAL = 0,   /*  omxvdec_ioctl_chan_release_buffer free */
    OMX_MEM_FREE_TYPE_ABNORMAL,     /*  omxvdec_release free */
} omx_mem_free_type;

hi_s32 hi_drv_omxvdec_alloc_and_map(const char *buf_name, hi_u32 cache, hi_u32 len, hi_u32 align,
    omxvdec_buffer *omx_buf);
hi_s32 hi_drv_omxvdec_unmap_and_free(omxvdec_buffer *omx_buf, omx_mem_free_type free_type);
hi_s32 hi_drv_omxvdec_alloc(const char *buf_name, hi_u32 len, hi_u32 align, omxvdec_buffer *omx_buf);
hi_void hi_drv_omxvdec_free(omxvdec_buffer *ps_m_buf, omx_mem_free_type free_type);
hi_s32 hi_drv_omxvdec_map(omxvdec_buffer *omx_buf, hi_u32 cache);
hi_void hi_drv_omxvdec_unmap(omxvdec_buffer *ps_m_buf);

#endif

