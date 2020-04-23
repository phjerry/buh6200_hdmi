/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ion  adapter code
 * Author: image_libin
 * Create: 2019-03-28
 */
#ifndef __DRV_PQ_MEM_H__
#define __DRV_PQ_MEM_H__

#include <linux/dma-buf.h>
#include "hi_osal.h"

typedef enum {
    HI_DRV_MEM_CACHE_TO_DEVICE = 0,
    HI_DRV_MEM_CACHE_FROM_DEVICE,
    HI_DRV_MEM_CACHE_BIDIRECTIONAL,
    HI_DRV_MEM_CACHE_MAX,
} drv_pq_mem_cache_mode;

typedef struct {
    osal_mem_type mode;     /* 内存分配的模式 */
    hi_bool is_map_viraddr; /* 是否需要映射CPU地址 */
    hi_bool is_cache;       /* 是否需要cache */
    hi_u32 size;            /* 内存分配的大小 */
    hi_u8 *name;            /* ion接口不管理名称，杨伟在考虑如何增加这个需求************************************ */
} drv_pq_mem_attr;

typedef struct {
    osal_mem_type mode;     /* 内存分配的模式 */
    hi_bool is_map_viraddr; /* 是否映射了CPU地址 */
    hi_bool is_cache;       /* 是否做了cache */
    hi_u32 size;            /* 内存分配的大小 */
    dma_addr_t phy_addr;    /* 物理地址，32/64自适应 */
    hi_u8 *vir_addr;        /* CPU地址 */
    hi_u8 *name;            /* ion接口不管理名称，杨伟在考虑如何增加这个需求************************************ */

    struct dma_buf *dma_buf; /* fd对应的dma buf调用者不需要关注 */
} drv_pq_mem_info;

typedef struct {
    hi_u64 fd;               /* dma_buffer 的shared_fd */
    hi_bool is_map_viraddr;  /* 是否需要映射CPU地址 */
    hi_bool is_secure;       /* 是否需要映射CPU地址 */
    dma_addr_t phy_addr;     /* fd对应的物理地址，32/64自适应 */
    hi_u8 *vir_addr;         /* fd对应的CPU地址 */
    struct dma_buf *dma_buf; /* fd对应的dma buf调用者不需要关注 */
} drv_pq_mem_fd_info;

/* 申请内存接口 */
// 申请内存使用，引用计数+1，默认直接给出对应的fd与物理地址，需要虚拟地址
hi_s32 drv_pq_mem_alloc(const drv_pq_mem_attr *mem_attr, drv_pq_mem_info *mem_info);
// 释放内存使用，引用计数-1
hi_s32 drv_pq_mem_free(drv_pq_mem_info *mem_info);

hi_s32 drv_pq_mem_get_fd(struct dma_buf *dma_buf);

// 对带cash的内存做flush
hi_s32 drv_pq_mem_flush(const drv_pq_mem_info *mem_info, drv_pq_mem_cache_mode cache_mode);

/* 使用外部传递的fd，通过下面的函数接口来使用 */
// 通过fd获取物理地址或者虚拟地址，引用计数+1
hi_s32 drv_pq_mem_map_fd(struct dma_buf *dma_buf_descp, drv_pq_mem_info *fd_info);

// 解映射fd，引用计数-1
hi_s32 drv_pq_mem_unmap_fd(struct dma_buf *dma_buf_descp, drv_pq_mem_info *fd_info);

#endif
