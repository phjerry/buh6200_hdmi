/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VMM_MEMORY_OPS_H__
#define __VMM_MEMORY_OPS_H__

#include "vmm_ext.h"

#define MEM_OPS_SUCCESS      (0x0)
#define MEM_OPS_FAILURE      (0xFFFFFFFF)
#define MEM_OPS_INVALID_PARA (0xFF020001)
#define MEM_OPS_ALLOC_FAILED (0xFF020002)
#define MEM_OPS_MAP_FAILED   (0xFF020003)
#define MEM_OPS_NOT_SUPPORT  (0xFF020004)
#define MEM_OPS_NULL_PTR     (0xFF020005)

#define MEM_OPS_TRUE  (1)
#define MEM_OPS_FALSE (0)

hi_s32 vmm_ops_alloc(vmm_buffer *mem_info);

hi_s32 vmm_ops_release(vmm_buffer *mem_info);

hi_s32 vmm_ops_map(vmm_buffer *mem_info);

hi_s32 vmm_ops_unmap(vmm_buffer *mem_info);

hi_s32 vmm_ops_config_mem_block(vmm_buffer *mem_block); /* preAllocUse */

#ifdef __cplusplus
}
#endif

#endif /* __VMM_MEMORY_OPS_H__ */


