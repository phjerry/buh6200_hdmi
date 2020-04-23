/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vmm_dbg.h"
#include "vmm_ops.h"
#include "vmm_dbg.h"
#include "vfmw_osal.h"

hi_s32 vmm_ops_alloc(vmm_buffer *mem_info)
{
    hi_s32 ret = MEM_OPS_SUCCESS;
    mem_record mem_rec = {0};

    D_VMM_CHECK_PTR_RET(mem_info, MEM_OPS_NULL_PTR);

    if (mem_info->sec_flag) {
        mem_rec.mode = MEM_MMU_SEC;
        mem_rec.vdec_handle = mem_info->vdec_handle;
        mem_rec.ssm_handle = mem_info->ssm_handle;
        ret = OS_ALLOC_MEM(mem_info->buf_name, mem_info->size, &mem_rec);
        mem_info->dma_buf = mem_rec.dma_buf;
        mem_info->start_phy_addr = mem_rec.phy_addr;
        mem_info->start_vir_addr = (hi_ulong)mem_rec.vir_addr;
        mem_info->size = mem_rec.length;
        if (ret != OSAL_OK) {
            PRINT(PRN_ERROR, "%s,%d, ERROR: Alloc sec mem %s size %d failed.\n",
                  __func__, __LINE__, mem_info->buf_name, mem_info->size);
        }
    } else {
        ret = OS_ALLOC_MEM(mem_info->buf_name, mem_info->size, &mem_rec);
        mem_info->dma_buf = mem_rec.dma_buf;
        mem_info->start_phy_addr = mem_rec.phy_addr;
        mem_info->start_vir_addr = (hi_ulong)mem_rec.vir_addr;
        mem_info->size = mem_rec.length;
        if (ret != OSAL_OK) {
            PRINT(PRN_ERROR, "%s,%d, ERROR: Alloc mem %s size %d failed.\n",
                  __func__, __LINE__, mem_info->buf_name, mem_info->size);
            return MEM_OPS_FAILURE;
        }

        if (mem_info->map) {
            mem_rec.is_cached = (hi_u8)mem_info->cache;
            mem_rec.vir_addr = OS_MAP_MEM(&mem_rec);
            mem_info->start_vir_addr = (hi_ulong)mem_rec.vir_addr;
            if (mem_rec.vir_addr == HI_NULL) {
                PRINT(PRN_ERROR, "%s,%d, ERROR: Map mem %s size %d failed.\n",
                      __func__, __LINE__, mem_info->buf_name, mem_info->size);
                return MEM_OPS_FAILURE;
            }
        }
    }

    return ret;
}

hi_s32 vmm_ops_release(vmm_buffer *mem_info)
{
    mem_record mem_rec = {0};

    D_VMM_CHECK_PTR_RET(mem_info, MEM_OPS_NULL_PTR);

    mem_rec.dma_buf = mem_info->dma_buf;
    mem_rec.phy_addr = mem_info->start_phy_addr;
    mem_rec.vir_addr = UINT64_PTR(mem_info->start_vir_addr);
    mem_rec.length = mem_info->size;

    if (mem_info->sec_flag) {
        mem_rec.mode = MEM_MMU_SEC;
        OS_FREE_MEM(&mem_rec);
    } else {
        if (mem_info->map) {
            OS_UNMAP_MEM(&mem_rec);
        }

        OS_FREE_MEM(&mem_rec);
    }

    return MEM_OPS_SUCCESS;
}

hi_s32 vmm_ops_map(vmm_buffer *mem_info)
{
    hi_s32 ret = MEM_OPS_SUCCESS;
    mem_record mem_rec = {0};

    D_VMM_CHECK_PTR_RET(mem_info, MEM_OPS_NULL_PTR);

    VMM_ASSERT(mem_info->map == 0);

    if (mem_info->sec_flag) {
        PRINT(PRN_ERROR, "%s,%d, WARNING:SEC MEMORY NOT SUPPORT MAP vir_addr.\n", __func__, __LINE__);
        return MEM_OPS_NOT_SUPPORT;
    }

    if (mem_info->start_vir_addr != 0) {
        PRINT(PRN_ERROR, "%s,%d, WARNING:buf_name = %s, start_vir_addr = 0x%llx already map!\n", __func__, __LINE__,
              mem_info->buf_name, mem_info->start_vir_addr);
        return MEM_OPS_MAP_FAILED;
    }

    mem_rec.dma_buf = mem_info->dma_buf;
    mem_rec.phy_addr = mem_info->start_phy_addr;
    mem_rec.length = mem_info->size;
    mem_rec.vir_addr = OS_MAP_MEM(&mem_rec);
    mem_info->start_vir_addr = PTR_UINT64(mem_rec.vir_addr);
    if (mem_rec.vir_addr == HI_NULL) {
        return MEM_OPS_MAP_FAILED;
    }

    mem_info->map = 1;

    return ret;
}

hi_s32 vmm_ops_unmap(vmm_buffer *mem_info)
{
    mem_record mem_rec = {0};

    D_VMM_CHECK_PTR_RET(mem_info, MEM_OPS_NULL_PTR);

    VMM_ASSERT(mem_info->map == 1);

    if (mem_info->sec_flag) {
        PRINT(PRN_ERROR, "%s,%d, WARNING:SEC MEMORY NOT SUPPORT MAP vir_addr.\n", __func__, __LINE__);
        return MEM_OPS_NOT_SUPPORT;
    }

    mem_rec.dma_buf = mem_info->dma_buf;
    mem_rec.phy_addr = mem_info->start_phy_addr;
    mem_rec.vir_addr = UINT64_PTR(mem_info->start_vir_addr);
    mem_rec.length = mem_info->size;
    mem_rec.is_cached = mem_info->cache;
    OS_UNMAP_MEM(&mem_rec);

    mem_info->map = 0;
    mem_info->cache = 0;
    mem_info->start_vir_addr = 0;

    return MEM_OPS_SUCCESS;
}

hi_s32 vmm_ops_config_mem_block(vmm_buffer *mem_block) /* preAllocUse */
{
    D_VMM_CHECK_PTR_RET(mem_block, MEM_OPS_NULL_PTR);

    return MEM_OPS_NOT_SUPPORT;
}




