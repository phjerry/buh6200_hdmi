/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: memory operation
 * Author: sdk
 * Create: 2019-05-17
 */

#include <linux/hisilicon/securec.h>
#include "hi_osal.h"

#include "drv_gfx2d_struct.h"
#include "hi_gfx_comm_k.h"
#include "drv_gfx2d_mem.h"
#include "hi_gfx_sys_k.h"
#include "drv_gfx2d_debug.h"
#include "drv_gfx2d_hal.h"

/* **************************** Macro Definition ************************************************* */
#define GFX2D_PROC 1
#define RegisterMaxTimes 16 /* Register Max Times */ /* CNcomment: 内存块注册最多次数 */

/* ************************** Structure Definition *********************************************** */

typedef struct {
    hi_u32 nSize; /* memblock size */                                 /* CNcoment: 内存块大小 */
    hi_u16 nFree; /* number of free memory units */                   /* CNcomment: 空闲内存单元个数 */
    hi_u16 nFirst; /* Identification of the first free memory unit */ /* CNcomment:首个空闲内存单元标识 */
    hi_u32 nUnitSize; /* memory unit size */                          /* CNcomment: 内存单元大小 */
    hi_void *pStartAddr; /* Starting address of memory block */       /* CNcomment: 内存块起始地址 */
    hi_u16 nMaxUsed; /* max used */                                   /* CNcomment: 最大使用个数 */
    hi_u16 nUnitNum; /* total number of memory units */               /* CNcomment: 内存单元总个数 */
    hi_u16 nBeUsed; /* number of history times */                     /* CNcomment: 历史申请次数 */
    osal_spinlock lock; /* spinlock identification */                 /* CNcomment: spinlock标识 */
} MemoryBlock;

/* ********************* Global Variable declaration ********************************************* */

static hi_u32 g_u32MemPoolPhyAddr = 0; /* physical address of memory pool */      /* CNcomment: 内存池物理地址 */
static hi_void *g_pMemPoolVrtAddr = HI_NULL; /* virtual address of memory pool */ /* CNcomment: 内存池虚拟地址 */
static hi_u32 g_u32MemPoolSize = 0; /* memory pool size */                        /* CNcomment: 内存池大小 */

static hi_u32 g_uRegisterCount = 0; /* number of registered memory blocks */ /* CNcomment: 内存块已注册次数 */

static MemoryBlock g_struMemBlock[RegisterMaxTimes] = {
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
};

/* ****************************** API realization ************************************************ */
void GFX2D_MEM_Lock(osal_spinlock *lock, unsigned long *lockflags)
{
    if ((lock != HI_NULL) && (lock->lock != HI_NULL) && (lockflags != HI_NULL)) {
        osal_spin_lock_irqsave(lock, lockflags);
    }
}

void GFX2D_MEM_Unlock(osal_spinlock *lock, unsigned long *lockflags)
{
    if ((lock != HI_NULL) && (lock->lock != HI_NULL) && (lockflags != HI_NULL)) {
        osal_spin_unlock_irqrestore(lock, lockflags);
    }
}

static drv_gfx_mem_info g_mem_pool_info = {0};

hi_s32 GFX2D_MEM_Init(const hi_u32 u32Size)
{
    hi_s32 ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (u32Size == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Size);
        return HI_FAILURE;
    }
    //    g_u32MemPoolPhyAddr = HI_GFX_AllocMem("GFX2D_MemPool", NULL, u32Size, &bMmu);
    ret = drv_gfx_mem_alloc(&g_mem_pool_info, "TDE_MemPool", HI_FALSE, HI_FALSE, u32Size);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
        return HI_FAILURE;
    }

    ret = drv_gfx_mem_map(&g_mem_pool_info);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
        drv_gfx_mem_free(&g_mem_pool_info);
        memset_s(&g_mem_pool_info, sizeof(drv_gfx_mem_info), 0, sizeof(drv_gfx_mem_info));
        return HI_FAILURE;
    }
    g_pMemPoolVrtAddr = g_mem_pool_info.virtual_addr;
    if (g_pMemPoolVrtAddr == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Size);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, g_u32MemPoolPhyAddr);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_Map, FAILURE_TAG);
        return HI_FAILURE;
    }

    g_u32MemPoolSize = u32Size;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void GFX2D_MEM_Deinit(hi_void)
{
    hi_u16 i = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    for (i = 0; i < g_uRegisterCount; i++) {
        if (g_struMemBlock[i].nFree != g_struMemBlock[i].nUnitNum) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "some buffer in use");
        }
    }

    if (g_pMemPoolVrtAddr != NULL) {
        drv_gfx_mem_free(&g_mem_pool_info);
    }

    g_u32MemPoolPhyAddr = 0;
    g_pMemPoolVrtAddr = HI_NULL;
    g_u32MemPoolSize = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 GFX2D_MEM_Register(const hi_u32 u32BlockSize, const hi_u32 u32BlockNum)
{
    hi_u16 i = 0;
    hi_u16 j = 0;
    hi_u32 u32TotalSize = 0;
    hi_u32 u32Size = 0;
    hi_u8 *pData = NULL;
    MemoryBlock TempMemBlock = {0};
    hi_u32 u32MemSize = u32BlockSize * u32BlockNum;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (((hi_u64)u32BlockSize * u32BlockNum > 0xffffffffU) || ((hi_u64)u32BlockSize * u32BlockNum == 0)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32MemSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BlockSize);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "u32MemSize too large or too small");
        return HI_FAILURE;
    }

    if (g_u32MemPoolSize == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_u32MemPoolSize);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "The memory pool is not initial");
        return HI_FAILURE;
    }

    /* The size range of memory blocks is not between memory cells and memory pools. */
    /* CNcomment: 内存块大小范围不在内存单元与内存池之间 */
    if (u32MemSize > g_u32MemPoolSize) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32MemSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_u32MemPoolSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BlockSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BlockNum);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "u32MemSize too large or too small");
        return HI_FAILURE;
    }

    /* Determine whether the number of registrations exceeds the required number */
    /* CNcomment: 判断注册次数是否超过规定次数 */
    if (g_uRegisterCount >= RegisterMaxTimes) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, RegisterMaxTimes);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_uRegisterCount);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the times of register is more than the max times");
        return HI_FAILURE;
    }

    /* Determine whether there are memory blocks of the same memory unit */
    /* CNcomment: 判断是否存在相同内存单元的内存块 */
    for (i = 0; i < g_uRegisterCount; i++) {
        if (u32BlockSize == g_struMemBlock[i].nUnitSize) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_struMemBlock[i].nUnitSize);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BlockSize);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "already  register memory block of the same size");
            return HI_FAILURE;
        }
    }

    /* Determine whether the total memory block size exceeds the memory pool size */
    /* CNcomment: 判断内存块总大小是否超出内存池大小 */
    for (i = 0; i < g_uRegisterCount; i++) {
        u32Size += g_struMemBlock[i].nSize;
        u32TotalSize = u32Size + u32MemSize;
        if (u32TotalSize > g_u32MemPoolSize) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32TotalSize);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_u32MemPoolSize);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,
                                 "the registered memory size is larger than the memory pool size");
            return HI_FAILURE;
        }
    }

    /* Update memory management information */
    /* CNcomment: 更新内存管理信息 */
    if (u32BlockSize == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BlockSize);
        return HI_FAILURE;
    }

    g_struMemBlock[g_uRegisterCount].nFree = u32MemSize / u32BlockSize;                   /* 空闲单元个数 */
    g_struMemBlock[g_uRegisterCount].nSize = u32MemSize;                                  /* 内存块大小 */
    g_struMemBlock[g_uRegisterCount].nFirst = 0;                                          /* 初始化首个空闲单元标识 */
    g_struMemBlock[g_uRegisterCount].nUnitSize = u32BlockSize;                            /* 内存单元大小 */
    g_struMemBlock[g_uRegisterCount].nUnitNum = u32BlockNum;                              /* 内存单元个数 */
    g_struMemBlock[g_uRegisterCount].pStartAddr = (hi_u8 *)(g_pMemPoolVrtAddr + u32Size); /*  内存块起始地址 */

    pData = g_struMemBlock[g_uRegisterCount].pStartAddr;

    if (osal_spin_lock_init(&(g_struMemBlock[g_uRegisterCount].lock)) != 0) {
        return HI_FAILURE;
    }

    /* Initialize the identification of each memory unit */
    /* CNcomment: 初始化每个内存单元的标识 */
    for (i = 1; i < g_struMemBlock[g_uRegisterCount].nUnitNum; i++) {
        /* When the last unit is assigned to the last unit without marking, it means that all units have been allocated,
           so there is no next assignable unit instruction. */
        /* CNcomment: 最后一个unit不做标记，分配到最后一个unit了就说明
           所有的unit都分配光了，因而没有下个可分配unit指示 */
        *(hi_u16 *)pData = i;
        pData += g_struMemBlock[g_uRegisterCount].nUnitSize;
    }

    TempMemBlock = g_struMemBlock[g_uRegisterCount];

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_uRegisterCount);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[g_uRegisterCount].nFree);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[g_uRegisterCount].nSize);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[g_uRegisterCount].nFirst);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[g_uRegisterCount].nUnitSize);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[g_uRegisterCount].nUnitNum);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_struMemBlock[g_uRegisterCount].pStartAddr);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");

    /* incremental arrangement of registered memory blocks */
    /* CNcomment: 将注册的内存块递增排列 */
    for (i = 0; i < g_uRegisterCount; i++) {
        if (g_struMemBlock[g_uRegisterCount].nUnitSize < g_struMemBlock[i].nUnitSize) {
            for (j = g_uRegisterCount; j > i; j--) {
                g_struMemBlock[j] = g_struMemBlock[j - 1];
            }
            g_struMemBlock[j] = TempMemBlock;
        }

        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, i);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[i].nFree);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[i].nSize);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[i].nFirst);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[i].nUnitSize);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_struMemBlock[i].nUnitNum);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_struMemBlock[i].pStartAddr);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");
    }

    /* Number of registrations plus 1 */
    /* CNcomment: 注册次数加1 */
    g_uRegisterCount++;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/* release fixed size memory blocks */
/* CNcomment: 注销固定大小的内存块 */
hi_void GFX2D_Mem_UnRegister(const hi_u32 u32BlockSize)
{
    hi_u16 i = 0;
    hi_u16 pos = 0;
    MemoryBlock *pBlock = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(g_uRegisterCount, RegisterMaxTimes);
    /* Find out the corresponding memory block */
    /* CNcomment: 找出对应的内存块 */
    for (i = 0; i < g_uRegisterCount; i++) {
        if (u32BlockSize == g_struMemBlock[i].nUnitSize) {
            pBlock = &g_struMemBlock[i];
            /* record array location */ /* CNcomment: 记录数组位置 */
            pos = i;
            break;
        }
    }

    /* No corresponding memory block was found. */ /* CNcomment: 没有找到对应的内存块 */
    if (pBlock == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pBlock);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "There is no such memory block");
        return;
    }

    /* there is unreleased memory */ /* CNcomment: 存在未释放的内存 */
    if (g_struMemBlock[i].nFree != g_struMemBlock[i].nUnitNum) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "some memory is not free");
        return;
    }

    /* update global memory management array information */ /* CNcomment: 更新全局内存管理数组信息 */
    for (i = pos; i < g_uRegisterCount - 1; i++) {
        g_struMemBlock[i] = g_struMemBlock[i + 1]; /* adjust array position */ /* CNcomment: 调整数组位置 */
    }

    memset_s(&g_struMemBlock[i], sizeof(MemoryBlock), 0, sizeof(MemoryBlock));

    /* reduce the number of memory blocks by one */ /* CNcomment: 内存块个数-1 */
    g_uRegisterCount--;
    osal_spin_lock_destory(&g_struMemBlock[g_uRegisterCount].lock);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/* memory alloc */ /* CNcomment: 分配内存 */
hi_void *GFX2D_MEM_Alloc(const hi_u32 u32Size)
{
    hi_u16 i = 0;
    unsigned long lockflags = 0;
    hi_u8 *pFree = NULL;
    MemoryBlock *pBlock = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* The memory size of the application is 0 */ /* CNcomment: 申请的内存大小为0 */
    if (u32Size == 0) {
        /* print warning info */ /* CNcomment 打印warning信息 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Size);
        return NULL;
    }

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(g_uRegisterCount, RegisterMaxTimes, NULL);
    /* Find the memory block where the application is located */ /* CNcomment: 找出申请的内存所在的内存块 */
    for (i = 0; i < g_uRegisterCount; i++) {
        if (u32Size <= g_struMemBlock[i].nUnitSize) {
            pBlock = &g_struMemBlock[i];
            break;
        }
    }

    /* not find memory block */ /* CNcomment: 没有找到内存块 */
    if (pBlock == NULL) {
        /* print warning info */ /* CNcomment 打印warning信息 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pBlock);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the memory block is not registered");
        return NULL;
    }

    /* obtain spin locks */ /* CNcomment: 获取自旋锁 */
    GFX2D_MEM_Lock(&g_struMemBlock[i].lock, &lockflags);

    /* No idle memory unit */ /* CNcomment: 无空闲内存单元 */
    if (!pBlock->nFree) {
        GFX2D_MEM_Unlock(&g_struMemBlock[i].lock, &lockflags);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "There is no free unit ");
        return NULL;
    }

    /* Calculate the address of the free memory unit from the identification of the first free unit */
    /* CNcomment: 由首个空闲单元的标识计算出空闲内存单元地址 */
    pFree = pBlock->pStartAddr + pBlock->nFirst * pBlock->nUnitSize;
    /* Modify the identification of the first idle cell */ /* CNcomment: 修改首个空闲单元的标识 */
    pBlock->nFirst = *(hi_u16 *)pFree;
    /* Reduce the number of free memory units by 1 */ /* CNcomment: 空闲内存单元数减1 */
    pBlock->nFree--;
    /* Memory block application times plus one */ /* CNcomment: 内存块申请次数+1 */
    pBlock->nBeUsed++;
    /* release spin lock */ /* CNcomment: 释放自旋锁 */
    GFX2D_MEM_Unlock(&g_struMemBlock[i].lock, &lockflags);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return pFree;
}

hi_s32 GFX2D_MEM_Unit_Free(hi_u32 i, hi_void *pBuf)
{
    MemoryBlock *pBlock = NULL;
    unsigned long lockflags = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(i, RegisterMaxTimes, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pBuf, HI_FAILURE);

    pBlock = &g_struMemBlock[i];
    /* obtain spin lock */ /* CNcomment: 获取自旋锁 */
    GFX2D_MEM_Lock(&g_struMemBlock[i].lock, &lockflags);
    /* Number of idle units plus one */ /* CNcomment: 空闲单元数+1 */
    pBlock->nFree++;
    /* Point the identity of the memory unit that is requested to be released to the next allocatable unit */
    /* CNcomment: 将申请释放的内存单元的标识指向下一个可以分配的单元 */
    *(hi_u16 *)pBuf = pBlock->nFirst;

    /* Make the memory unit requested to be freed the first free memory unit */
    /* CNcomment: 将申请释放的内存单元作为首个空闲内存单元 */
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, pBlock->nUnitSize, HI_FAILURE);
    pBlock->nFirst = (pBuf - pBlock->pStartAddr) / pBlock->nUnitSize;
    /* release spinlock */ /* CNcomment: 释放自旋锁 */

    pBlock->nMaxUsed++;
    GFX2D_MEM_Unlock(&g_struMemBlock[i].lock, &lockflags);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/* release memory */ /* CNcomment: 释放内存 */
hi_s32 GFX2D_MEM_Free(hi_void *pBuf)
{
    hi_u16 i = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* Null pointer detection */ /* CNcomment: 空指针检测 */
    if (pBuf == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pBuf);
        return HI_FAILURE;
    }

    if ((pBuf < g_pMemPoolVrtAddr) || (pBuf >= (g_pMemPoolVrtAddr + g_u32MemPoolSize))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pBuf);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_pMemPoolVrtAddr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_u32MemPoolSize);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the addr is out of the range of the memory pool");
        return HI_FAILURE;
    }

    /* Calculate the corresponding memory block from the address */ /* CNcomment: 由地址计算出对应的内存块 */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(g_uRegisterCount, RegisterMaxTimes, HI_FAILURE);
    for (i = 0; i < g_uRegisterCount; i++) {
        if ((pBuf < g_struMemBlock[i].pStartAddr) ||
            (pBuf >= (g_struMemBlock[i].pStartAddr + g_struMemBlock[i].nSize))) {
            continue;
        }

        if (HI_SUCCESS == GFX2D_MEM_Unit_Free(i, pBuf)) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_SUCCESS;
        }
    }

    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the addr is not the right addr of memory");

    return HI_FAILURE;
}

/* Get the physical address of memory */ /* CNcomment: 获取内存物理地址 */
hi_u64 GFX2D_MEM_GetPhyaddr(hi_void *pBuf)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* Address is not in memory pool range */ /* CNcomment: 地址不在内存池范围内 */
    if ((pBuf == NULL) || (pBuf < g_pMemPoolVrtAddr) || (pBuf >= (g_pMemPoolVrtAddr + g_u32MemPoolSize))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pBuf);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_pMemPoolVrtAddr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_u32MemPoolSize);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the addr is out of the range of the memory pool");
        return 0;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    /* Corresponding physical address */ /* CNcomment: 返回pBuf  对应的物理地址 */
    return (g_mem_pool_info.phy_addr + (pBuf - g_mem_pool_info.virtual_addr));
}

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_MEM_ReadProc(hi_void *p, hi_void *v)
{
    hi_u16 i = 0;
    // HI_UNUSED(v);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(p);

    osal_proc_print(p, "+++++++++++++++++Hisilicon TDE Memory Pool Info +++++++++++++++++++\n");
    osal_proc_print(p, " MemPoolSize   MemBlockNum       \n");
    osal_proc_print(p, " %8u %8u\n", g_u32MemPoolSize, g_uRegisterCount);
    osal_proc_print(p,
                    " BlockSize   UnitSize   UnitNum    UnitFreeNum    FirstFreePos    AllocTimes    MaxUsed       \n");

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(g_uRegisterCount, RegisterMaxTimes);
    for (i = 0; i < g_uRegisterCount; i++) {
        osal_proc_print(p, "%8u   %8u  %8u   %8u       %8u        %8u      %8u\n", g_struMemBlock[i].nSize,
                        g_struMemBlock[i].nUnitSize, g_struMemBlock[i].nUnitNum, g_struMemBlock[i].nFree,
                        g_struMemBlock[i].nFirst, g_struMemBlock[i].nBeUsed, g_struMemBlock[i].nMaxUsed);
    }

    return;
}
#endif
