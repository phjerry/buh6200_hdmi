/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: mem manger
 * Author: sdk
 * Create: 2019-03-18
 */
#include "tde_define.h"
#include "wmalloc.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_gfx_comm.h"
#include "hi_gfx_debug.h"
#include "hi_gfx_type.h"
#else
#include "tde_hal.h"
#include "tde_debug.h"
#include <linux/hisilicon/securec.h>
#endif

/* ************************** Structure Definition ***************************************** */
typedef struct {
    hi_u32 nSize;
    hi_u16 nFree;
    hi_u16 nFirst;
    hi_u16 nUnitSize;
#if HI_TDE_MEMCOUNT_SUPPORT
    hi_u16 nMaxUsed; /* Max used unit number */
    hi_u16 nMaxNum;  /* Max unit number */
#endif
    hi_void *pStartAddr;
    struct _MemoryBlock *pNext;
} MemoryBlock;

typedef enum {
    UNIT_SIZE_CMD = 0,
    UNIT_SIZE_JOB,
    UNIT_SIZE_FILTER,
    UNIT_SIZE_NODE,
    UNIT_SIZE_BUTT
} UNIT_SIZE_E;

/* ********************* Global Variable declaration *************************************** */

STATIC MemoryBlock g_struMemBlock[UNIT_SIZE_BUTT];

#ifndef HI_BUILD_IN_BOOT
STATIC osal_spinlock g_MemLock;
#endif

/***************************************************************************************
* func          : MemoryBlockInit
* description   : mem block init
                  CNcomment: CNend\n
* param[in]     :
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
***************************************************************************************/
static hi_s32 MemoryBlockInit(UNIT_SIZE_E eUnitSize, hi_u32 nUnitNum, hi_void *pAddr)
{
    hi_u16 i = 0;
    hi_void *pData = pAddr;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pAddr, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, nUnitNum, HI_FAILURE);

    if (eUnitSize == UNIT_SIZE_CMD) {
        g_struMemBlock[eUnitSize].nUnitSize = CMD_SIZE;
    } else if (eUnitSize == UNIT_SIZE_JOB) {
        g_struMemBlock[eUnitSize].nUnitSize = JOB_SIZE;
    } else if (eUnitSize == UNIT_SIZE_FILTER) {
        g_struMemBlock[eUnitSize].nUnitSize = FILTER_SIZE;
    } else if (eUnitSize == UNIT_SIZE_NODE) {
        g_struMemBlock[eUnitSize].nUnitSize = NODE_SIZE;
    } else {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, UNIT_SIZE_BUTT);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, eUnitSize);
        return HI_FAILURE;
    }

    for (i = 1; i < nUnitNum; i++) {
        /* Don't flag for last unit,for last unit is ready for assigned,
         * which is say no next unit can be assigned
         */
        *(hi_u16 *)pData = i;

        pData += g_struMemBlock[eUnitSize].nUnitSize;
    }

    g_struMemBlock[eUnitSize].nFirst = 0;
    g_struMemBlock[eUnitSize].nFree = nUnitNum;
    g_struMemBlock[eUnitSize].nSize = nUnitNum * g_struMemBlock[eUnitSize].nUnitSize;
    g_struMemBlock[eUnitSize].pNext = HI_NULL;
    g_struMemBlock[eUnitSize].pStartAddr = pAddr;

#if HI_TDE_MEMCOUNT_SUPPORT
    g_struMemBlock[eUnitSize].nMaxNum = nUnitNum;
    g_struMemBlock[eUnitSize].nMaxUsed = 0;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
* func          : mallocUnit
* description   : alloc from mem unit
                  CNcomment: CNend\n
* param[in]     :
* retval        : ddr
* others:       : NA
***************************************************************************************/
static hi_void *mallocUnit(UNIT_SIZE_E eUnitSize)
{
#ifndef HI_BUILD_IN_BOOT
    hi_size_t lockflags = 0;
#endif
    MemoryBlock *pBlock = HI_NULL;
    hi_u8 *pFree = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    tde_lock(&g_MemLock, lockflags);

    pBlock = &g_struMemBlock[eUnitSize];

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pBlock->nUnitSize);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pBlock->nFree);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pBlock->nFirst);

    if (!pBlock->nFree) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pBlock->nFree);
        tde_unlock(&g_MemLock, lockflags);
        return HI_NULL;
    }

    pFree = pBlock->pStartAddr + pBlock->nFirst * pBlock->nUnitSize;
    pBlock->nFirst = *(hi_u16 *)pFree;
    pBlock->nFree--;

#if HI_TDE_MEMCOUNT_SUPPORT
    if ((g_struMemBlock[eUnitSize].nMaxNum - pBlock->nFree) > g_struMemBlock[eUnitSize].nMaxUsed) {
        g_struMemBlock[eUnitSize].nMaxUsed = g_struMemBlock[eUnitSize].nMaxNum - pBlock->nFree;
    }
#endif

    tde_unlock(&g_MemLock, lockflags);

    memset(pFree, 0, pBlock->nUnitSize);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return pFree;
}

/***************************************************************************************
* func          : wmalloc
* description   : alloc from mempool
                  CNcomment: CNend\n
* param[in]     :
* retval        : ddr
* others:       : NA
***************************************************************************************/
static hi_void *wmalloc(hi_size_t size)
{
    UNIT_SIZE_E i = 0;
    hi_void *pMalloc = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, size, HI_NULL);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(size, NODE_SIZE, HI_NULL);

    if (size <= CMD_SIZE) {
        for (i = UNIT_SIZE_CMD; i < UNIT_SIZE_BUTT; i++) {
            pMalloc = mallocUnit(i);
            if (pMalloc != HI_NULL) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return pMalloc;
            }
        }
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CMD_SIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, size);
        return HI_NULL;
    } else if (size <= JOB_SIZE) {
        for (i = UNIT_SIZE_JOB; i < UNIT_SIZE_BUTT; i++) {
            pMalloc = mallocUnit(i);
            if (pMalloc != HI_NULL) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return pMalloc;
            }
        }
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, JOB_SIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, size);
        return HI_NULL;
    } else if (size <= UNIT_SIZE_FILTER) {
        for (i = UNIT_SIZE_FILTER; i < UNIT_SIZE_BUTT; i++) {
            pMalloc = mallocUnit(i);
            if (pMalloc != HI_NULL) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return pMalloc;
            }
        }
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, NODE_SIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, size);
        return HI_NULL;
    } else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return mallocUnit(UNIT_SIZE_NODE);
    }
}

static hi_s32 freeUnit(UNIT_SIZE_E eUnitSize, hi_void *ptr)
{
#ifndef HI_BUILD_IN_BOOT
    hi_size_t lockflags = 0;
#endif
    MemoryBlock *pBlock = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    tde_lock(&g_MemLock, lockflags);

    pBlock = &g_struMemBlock[eUnitSize];

    if ((ptr < pBlock->pStartAddr) || (ptr >= (pBlock->pStartAddr + pBlock->nSize))) {
        tde_unlock(&g_MemLock, lockflags);
        return HI_FAILURE;
    }

    pBlock->nFree++;
    *(hi_u16 *)ptr = pBlock->nFirst; /* point to next unit can be assigned */

    if (pBlock->nUnitSize == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pBlock->nUnitSize);
        tde_unlock(&g_MemLock, lockflags);
        return HI_FAILURE;
    }

    pBlock->nFirst = (ptr - pBlock->pStartAddr) / pBlock->nUnitSize;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pBlock->nFree);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pBlock->nFree);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pBlock->nFree);

    tde_unlock(&g_MemLock, lockflags);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 wfree(hi_void *ptr)
{
    UNIT_SIZE_E i = UNIT_SIZE_CMD;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    for (i = UNIT_SIZE_CMD; i < UNIT_SIZE_BUTT; i++) {
        if (HI_SUCCESS == freeUnit(i, ptr)) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_SUCCESS;
        }
    }

    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, ptr);
    GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, freeUnit, FAILURE_TAG);
    return HI_FAILURE;
}

/***************************************************************************************
* func          : tde_malloc
* description   : alloc from mempool
                  CNcomment: CNend\n
* param[in]     :
* retval        : ddr
* others:       : NA
***************************************************************************************/
hi_void *tde_malloc(hi_u32 size)
{
    hi_void *ptr = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    ptr = (hi_void *)wmalloc(size);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return ptr;
}

hi_void tde_free(hi_void *ptr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (ptr != HI_NULL) {
        wfree(ptr);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/* all before is unrelated with tde, can be transplant to other modules.The follow is related with tde encapsulation */
#define TDE_MIN_BUFFER ((FILTER_SIZE)*HI_TDE_FILTER_NUM + ((CMD_SIZE) + (NODE_SIZE) + (JOB_SIZE)) * 2)
#define TDE_MAX_BUFFER (1024 * 1024)

STATIC drv_gfx_mem_info g_mem_pool_info = {0};
STATIC hi_u32 g_u32TdeBuf = 0;

#ifdef HI_BUILD_IN_BOOT
/* * boot下内存优化 * */
#define HI_TDE_CMD_NUM 20
#define HI_TDE_JOB_NUM 1
#define HI_TDE_NODE_NUM 20
#else
#define HI_TDE_CMD_NUM (((g_u32TdeBuf) - (FILTER_SIZE)*HI_TDE_FILTER_NUM) / ((CMD_SIZE) + (NODE_SIZE) + (JOB_SIZE)))
#define HI_TDE_JOB_NUM HI_TDE_CMD_NUM
#define HI_TDE_NODE_NUM HI_TDE_CMD_NUM
#endif

#define TDE_CMD_OFFSET 0
#define TDE_JOB_OFFSET ((HI_TDE_CMD_NUM)*CMD_SIZE)
#define TDE_NODE_OFFSET (TDE_JOB_OFFSET + ((HI_TDE_JOB_NUM)*JOB_SIZE))
#define TDE_FILTER_OFFSET (TDE_NODE_OFFSET + ((HI_TDE_NODE_NUM)*NODE_SIZE))
#define TDE_MEMPOOL_SIZE                                                                   \
    ((HI_TDE_CMD_NUM)*CMD_SIZE + (HI_TDE_JOB_NUM)*JOB_SIZE + (HI_TDE_NODE_NUM)*NODE_SIZE + \
     (HI_TDE_FILTER_NUM)*FILTER_SIZE)

#ifdef HI_BUILD_IN_BOOT
#if !defined(HI_TDE_BUFFER)
#define HI_TDE_BUFFER 0x20000
#endif
#endif

/***************************************************************************************
* func          : wmeminit
* description   : mem init
                  CNcomment: CNend\n
* param[in]     :
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
***************************************************************************************/
hi_s32 wmeminit(void)
{
#ifndef HI_BUILD_IN_BOOT
    hi_s32 ret;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (HI_TDE_BUFFER > TDE_MAX_BUFFER) {
        g_u32TdeBuf = TDE_MAX_BUFFER;
    } else if (HI_TDE_BUFFER < TDE_MIN_BUFFER) {
        g_u32TdeBuf = TDE_MIN_BUFFER;
    } else {
        g_u32TdeBuf = HI_TDE_BUFFER;
    }

#ifdef HI_BUILD_IN_BOOT
    g_mem_pool_info.phy_addr = (hi_u32)(uintptr_t)hi_gfx_phy_mem_malloc(TDE_MEMPOOL_SIZE, "TDE_MemPool");
    g_mem_pool_info.smmu_addr = g_mem_pool_info.phy_addr;
    g_mem_pool_info.virtual_addr = (hi_void *)(uintptr_t)g_mem_pool_info.phy_addr;
#else
#ifdef TDE_CACH_STRATEGY
    ret = drv_gfx_mem_alloc(&g_mem_pool_info, "TDE_MemPool", HI_FALSE, HI_TRUE, TDE_MEMPOOL_SIZE);
#else
    ret = drv_gfx_mem_alloc(&g_mem_pool_info, "TDE_MemPool", HI_FALSE, HI_FALSE, TDE_MEMPOOL_SIZE);
#endif
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
        return HI_FAILURE;
    }
    ret = drv_gfx_mem_map(&g_mem_pool_info);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
        drv_gfx_mem_free(&g_mem_pool_info);
        memset(&g_mem_pool_info, 0, sizeof(drv_gfx_mem_info));
        return HI_FAILURE;
    }

    ret = osal_spin_lock_init(&g_MemLock);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
        drv_gfx_mem_unmap(&g_mem_pool_info);
        drv_gfx_mem_free(&g_mem_pool_info);
        memset(&g_mem_pool_info, 0, sizeof(drv_gfx_mem_info));
        return HI_FAILURE;
    }
#endif

    MemoryBlockInit(UNIT_SIZE_CMD, HI_TDE_CMD_NUM, (g_mem_pool_info.virtual_addr));
    MemoryBlockInit(UNIT_SIZE_JOB, HI_TDE_JOB_NUM, (g_mem_pool_info.virtual_addr + TDE_JOB_OFFSET));
    MemoryBlockInit(UNIT_SIZE_NODE, HI_TDE_NODE_NUM, (g_mem_pool_info.virtual_addr + TDE_NODE_OFFSET));
    MemoryBlockInit(UNIT_SIZE_FILTER, HI_TDE_FILTER_NUM, (g_mem_pool_info.virtual_addr + TDE_FILTER_OFFSET));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void wmemterm(void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifndef HI_BUILD_IN_BOOT
    if (g_mem_pool_info.virtual_addr != HI_NULL) {
        drv_gfx_mem_free(&g_mem_pool_info);
    }
#endif

    memset(&g_mem_pool_info, 0, sizeof(drv_gfx_mem_info));
    osal_spin_lock_destory(&g_MemLock);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void wmemflush(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifndef HI_BUILD_IN_BOOT
    drv_gfx_mem_flush(&g_mem_pool_info, HI_TRUE);
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_u64 wgetphy(hi_void *ptr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if ((ptr == HI_NULL) || (ptr < g_mem_pool_info.virtual_addr) ||
        (ptr >= (g_mem_pool_info.virtual_addr + TDE_MEMPOOL_SIZE))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, ptr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, TDE_MEMPOOL_SIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_mem_pool_info.virtual_addr);
        return 0;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return (g_mem_pool_info.phy_addr + (ptr - g_mem_pool_info.virtual_addr));
}

hi_void *wgetvrt(hi_u64 phyaddr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if ((phyaddr < g_mem_pool_info.phy_addr) || (phyaddr >= (g_mem_pool_info.phy_addr + TDE_MEMPOOL_SIZE))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, TDE_MEMPOOL_SIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, phyaddr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_mem_pool_info.phy_addr);
        return HI_NULL;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return (hi_void *)(g_mem_pool_info.virtual_addr + (phyaddr - g_mem_pool_info.phy_addr));
}

hi_u32 wgetfreenum(hi_void)
{
    UNIT_SIZE_E eUnitSize = 0;
    hi_u32 u32FreeUnitNum = g_struMemBlock[eUnitSize].nFree;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    for (eUnitSize = UNIT_SIZE_CMD; eUnitSize < UNIT_SIZE_FILTER; eUnitSize++) {
        u32FreeUnitNum = (u32FreeUnitNum > g_struMemBlock[eUnitSize].nFree) ? g_struMemBlock[eUnitSize].nFree
                                                                            : u32FreeUnitNum;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u32FreeUnitNum;
}

#ifndef HI_BUILD_IN_BOOT

#ifdef CONFIG_GFX_PROC_SUPPORT
struct seq_file *wprintinfo(struct seq_file *page)
{
#if HI_TDE_MEMCOUNT_SUPPORT
    hi_u32 u32MaxUsedCmd = g_struMemBlock[UNIT_SIZE_CMD].nMaxUsed;
    hi_u32 u32MaxUsedJob = g_struMemBlock[UNIT_SIZE_JOB].nMaxUsed;
    hi_u32 u32MaxUsedNode = g_struMemBlock[UNIT_SIZE_NODE].nMaxUsed;
    hi_u32 u32MaxUsedFilter = g_struMemBlock[UNIT_SIZE_FILTER].nMaxUsed;
#else
    hi_u32 u32FreeCmd = g_struMemBlock[UNIT_SIZE_CMD].nFree;
    hi_u32 u32FreeJob = g_struMemBlock[UNIT_SIZE_JOB].nFree;
    hi_u32 u32FreeNode = g_struMemBlock[UNIT_SIZE_NODE].nFree;
    hi_u32 u32FreeFilter = g_struMemBlock[UNIT_SIZE_FILTER].nFree;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#ifndef CONFIG_TDE_STR_DISABLE
    if (page != HI_NULL) {
        osal_proc_print(page, "------ Hisilicon TDE Memory Pool Info------ \n");
#if HI_TDE_MEMCOUNT_SUPPORT
        osal_proc_print(page, "     Type         Total       MaxUsed\n");
        osal_proc_print(page, "[Unit %d   ]   %8u  %8u\n", CMD_SIZE, HI_TDE_CMD_NUM, u32MaxUsedCmd); /* width 8 for
                                                                                                        print info */
        osal_proc_print(page, "[Unit %d   ]   %8u  %8u\n", JOB_SIZE, HI_TDE_JOB_NUM, u32MaxUsedJob); /* width 8 for
                                                                                                        print info */
        osal_proc_print(page, "[Unit %d ]  %8u  %8u\n", NODE_SIZE, HI_TDE_NODE_NUM, u32MaxUsedJob); /* width 8 for print
                                                                                                       info */
        osal_proc_print(page, "[Unit %d ]  %8u  %8u\n", FILTER_SIZE, HI_TDE_FILTER_NUM,
                        u32MaxUsedFilter);                                           /* width 8 for print info */
        osal_proc_print(page, "[Total     ]   %8uK %8uK\n", TDE_MEMPOOL_SIZE / 1024, /* width 8 for print info, div by
                                                                                        1024 to get kb */
                        (CMD_SIZE * u32MaxUsedCmd + JOB_SIZE * u32MaxUsedJob + NODE_SIZE * u32MaxUsedNode +
                         FILTER_SIZE * u32MaxUsedFilter) /
                            1024); /* div by 1024 to get kb */
#else
        osal_proc_print(page, "     Type         Total       Used\n");
        osal_proc_print(page, "[Unit %d   ]   %8u  %8u\n", CMD_SIZE, HI_TDE_CMD_NUM,
                        HI_TDE_CMD_NUM - u32FreeCmd); /* width 8 for print info */
        osal_proc_print(page, "[Unit %d   ]   %8u  %8u\n", JOB_SIZE, HI_TDE_JOB_NUM,
                        HI_TDE_JOB_NUM - u32FreeJob); /* width 8 for print info */
        osal_proc_print(page, "[Unit %d ]   %8u  %8u\n", NODE_SIZE, HI_TDE_NODE_NUM,
                        HI_TDE_NODE_NUM - u32FreeNode); /* width 8 for print info */
        osal_proc_print(page, "[Unit %d ]   %8u  %8u\n", FILTER_SIZE, HI_TDE_FILTER_NUM,
                        HI_TDE_FILTER_NUM - u32FreeFilter);                          /* width 8 for print info */
        osal_proc_print(page, "[Total     ]   %8uK %8uK\n", TDE_MEMPOOL_SIZE / 1024, /* width 8 for print info, div by
                                                                                        1024 to get kb */
                        (TDE_MEMPOOL_SIZE - (CMD_SIZE * u32FreeCmd + JOB_SIZE * u32FreeJob + NODE_SIZE * u32FreeNode +
                                             FILTER_SIZE * u32FreeFilter)) /
                            1024); /* div by 1024 to get kb */
#endif
    }
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return page;
}
#endif

#endif
