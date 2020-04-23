/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: gfx2d config
 * Author: sdk
 * Create: 2018-01-01
 */

/* ********************************add include here*********************************************** */
#include "drv_gfx2d_config.h"
#include "hi_gfx_comm_k.h"
#include "drv_gfx2d_debug.h"

/* **************************** Macro Definition ************************************************* */
/* * the node size is equal to register size that from 0x0 to TDE_INTMASK * */
#if (HWC_VERSION) >= 0x500
#define GFX2D_CONFIG_NODE_SIZE 2896
#else
#define GFX2D_CONFIG_NODE_SIZE 1120
#endif

#define GFX2D_NODE_NUM 1000

#define GFX2D_CONFIG_MAX_MEMSIZE 8036000

#define GFX2D_CONFIG_DEFAULT_MEMSIZE 8026000

/* ********************* Global Variable declaration ********************************************* */
static hi_u32 g_mem_size = (GFX2D_CONFIG_DEFAULT_MEMSIZE);

#ifdef CONFIG_GFX_PROC_SUPPORT
static hi_u32 g_start_time_ms = 0;
static hi_u32 g_start_time_us = 0;
static hi_u32 g_end_time_ms = 0;
static hi_u32 g_end_time_us = 0;
#endif

/* ****************************** API realization ************************************************ */
/*****************************************************************************
 * func         : GFX2D_CONFIG_SetMemSize
 * description  : calc node memsize
                  1. first, use insmod memsize
                  2. second, use make menuconfig memsize
                  3. third, use default size.
                  CNcomment: 计算节点内存大小 CNend\n
 * param[in]    : u32MemSize  CNcomment: 内存大小 CNend\n
                  Mem size(KByte)
 * retval       : NA
 * others:      : NA
 *****************************************************************************/
hi_s32 GFX2D_CONFIG_SetMemSize(hi_u32 u32MemSize)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (u32MemSize == 0) {
        g_mem_size = (GFX2D_CONFIG_DEFAULT_MEMSIZE);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_mem_size);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (u32MemSize < (GFX2D_CONFIG_NODE_SIZE)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, GFX2D_CONFIG_NODE_SIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32MemSize);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Mem not enough to work");
        return HI_FAILURE;
    } else if (u32MemSize > (GFX2D_CONFIG_MAX_MEMSIZE)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, GFX2D_CONFIG_MAX_MEMSIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32MemSize);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "No enough mem!");
        return HI_FAILURE;
    } else {
        g_mem_size = u32MemSize;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_mem_size);
    }

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32MemSize);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_u32 GFX2D_CONFIG_GetMemSize(hi_void)
{
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_mem_size);
    return g_mem_size;
}

hi_u32 GFX2D_CONFIG_GetNodeNum(hi_void)
{
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_mem_size);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, GFX2D_CONFIG_NODE_SIZE);
    return GFX2D_NODE_NUM;
}

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_CONFIG_StartTime(hi_void)
{
    HI_GFX_GetTimeStamp(&g_start_time_ms, &g_start_time_us);
}

hi_u32 GFX2D_CONFIG_EndTime(hi_void)
{
    hi_u32 Times;

    HI_GFX_GetTimeStamp(&g_end_time_ms, &g_end_time_us);
    if (g_end_time_ms < g_start_time_ms) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }
    Times = (g_end_time_ms - g_start_time_ms) * 1000 + (g_end_time_us - g_start_time_us); /* 1ms is 1000us */

    return Times;
}
#endif
