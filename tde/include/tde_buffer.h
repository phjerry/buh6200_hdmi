/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: buffer manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_BUFFER__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_BUFFER__

#include "tde_define.h"
#ifndef HI_BUILD_IN_BOOT
#include <linux/hisilicon/securec.h>
#else
#include "hi_gfx_type.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

STATIC drv_gfx_mem_info g_tde_mem_info = {0};
STATIC hi_u32 g_tde_ref_buffer = 0;

#ifndef HI_BUILD_IN_BOOT
static osal_spinlock g_tde_buf_lock;

STATIC hi_u32 tde_alloc_physic_buffer(hi_u32 cbcr_offset, hi_u32 buffer_size)
{
    drv_gfx_mem_info mem_info = {0};
    hi_u64 phy_addr = 0;
    hi_s32 ret;
    hi_size_t lock_flags = 0;

#ifdef CFG_HI_TDE_CSCTMPBUFFER_SIZE
    hi_u32 csc_buffer_size = CFG_HI_TDE_CSCTMPBUFFER_SIZE;
#elif defined(HI_BUILD_IN_BOOT)
    hi_u32 csc_buffer_size = 0;
#else
    hi_u32 csc_buffer_size = g_tde_tmp_buffer_size;
#endif

    tde_lock(&g_tde_buf_lock, lock_flags);

    if ((cbcr_offset + buffer_size) >= csc_buffer_size) {
        tde_unlock(&g_tde_buf_lock, lock_flags);
        return 0;
    }

    if (g_tde_mem_info.phy_addr == 0) {
        g_tde_ref_buffer = 0;
        tde_unlock(&g_tde_buf_lock, lock_flags);

#ifndef HI_BUILD_IN_BOOT
        ret = drv_gfx_mem_alloc(&mem_info, "TDE_TEMP_BUFFER", HI_TRUE, HI_FALSE, csc_buffer_size);
        if (ret != HI_SUCCESS) {
            return 0;
        }
#else
        phy_addr = (hi_u32)hi_gfx_phy_mem_malloc(csc_buffer_size, "TDE_TEMP_BUFFER");
        mem_info.phy_addr = phy_addr;
#endif
        if (phy_addr == 0) {
            return 0;
        }

        tde_lock(&g_tde_buf_lock, lock_flags);
    }

    g_tde_ref_buffer++;
    tde_unlock(&g_tde_buf_lock, lock_flags);

    return mem_info.phy_addr + cbcr_offset;
}
#endif

STATIC hi_void tde_free_physic_buffer(hi_void)
{
#ifndef HI_BUILD_IN_BOOT
    hi_size_t lock_flags = 0;
    drv_gfx_mem_info tmp_tde_mem_info = {0};
#else
#endif

    tde_lock(&g_tde_buf_lock, lock_flags);
    if (g_tde_ref_buffer == 0) {
        tde_unlock(&g_tde_buf_lock, lock_flags);
        return;
    }

    g_tde_ref_buffer--;
    if (g_tde_ref_buffer == 0) {
#ifndef HI_BUILD_IN_BOOT
        if (memcpy_s(&tmp_tde_mem_info, sizeof(drv_gfx_mem_info), &g_tde_mem_info, sizeof(drv_gfx_mem_info)) != EOK) {
            tde_unlock(&g_tde_buf_lock, lock_flags);
            return;
        }
#endif
        memset(&g_tde_mem_info, 0, sizeof(drv_gfx_mem_info));
        tde_unlock(&g_tde_buf_lock, lock_flags);

#ifndef HI_BUILD_IN_BOOT
        drv_gfx_mem_free(&tmp_tde_mem_info);
#endif
        return;
    }

    tde_unlock(&g_tde_buf_lock, lock_flags);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INCLUDE_BUFFER__ */
