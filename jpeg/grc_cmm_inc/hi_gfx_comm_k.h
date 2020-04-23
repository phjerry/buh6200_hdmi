/************************************************************************************************
*
* Copyright (C) 2018 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
*************************************************************************************************
File Name        : hi_gfx_comm_k.h
Version          : version 2.0
Author           :
Created          : 2018/01/01
Description      : Describes adp file. CNcomment:驱动跨平台适配 CNend\n
Function List    :

History          :
Date                 Author                Modification
2018/01/01           sdk
*************************************************************************************************/
#ifndef _HI_GFX_COMM_K_H_
#define _HI_GFX_COMM_K_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* **************************** SDK Version Macro Definition ************************************ */

/* ********************************add include here********************************************** */
#include "hi_type.h"
#include "hi_osal.h"

#if defined(CONFIG_GFX_STB_SDK) || defined(CONFIG_GFX_ANDROID_SDK) || defined(CONFIG_GFX_TV_SDK)
#include "hi_drv_module.h"

#if defined(CHIP_TYPE_hi3712) || defined(CHIP_TYPE_hi3716m) || defined(CHIP_TYPE_hi3716mv310) || \
    defined(CHIP_TYPE_hi3110ev500) || defined(CHIP_TYPE_hi3716mv320) || defined(CHIP_TYPE_hi3716mv330)
#include "drv_dev_ext.h"
#else
#include "hi_drv_dev.h"
#endif

#include "hi_drv_mem.h"
#include "hi_drv_sys.h"
#include "hi_gfx_smmu.h"
#endif

/* **************************** Macro Definition ************************************************ */
#define HI_GFX_GET_HIGH_PART(addr) (hi_u32)(((addr)&0xffffffff00000000) >> 32) /* high part 32 bits */
#define HI_GFX_GET_LOW_PART(addr) (hi_u32)((addr)&0x00000000ffffffff)

#define HI_GFX_REG_MAP(base, size) osal_ioremap_nocache((base), (size))
#define HI_GFX_REG_UNMAP(base) osal_iounmap((hi_void *)(base))

/* * 定义到Makefile和Android Makefile中 * */
#ifdef CONFIG_GFX_256BYTE_ALIGN
#define GFX_MMZ_ALIGN_BYTES 256
#else
#define GFX_MMZ_ALIGN_BYTES 16
#endif

#define CONFIG_GFX_MAX_MEM_SIZE (500 * 1024 * 1024)
#define CONFIG_GFX_MIN_MEM_SIZE 1

#define ConvertID(module_id) (module_id + HI_ID_TDE - HIGFX_TDE_ID)

typedef struct {
    hi_u32 size;
    hi_void *dmabuf;
    unsigned long phy_addr;
    unsigned long mmz_addr;
    unsigned long smmu_addr;
    hi_void *virtual_addr;
    hi_bool is_smmu;
} drv_gfx_mem_info;

/* ************************** Enum Definition *************************************************** */
typedef enum tagHIGFX_CHIP_TYPE_E {
    HIGFX_CHIP_TYPE_HI3716MV100 = 0, /**< HI3716MV100  */
    HIGFX_CHIP_TYPE_HI3716MV200,     /**< HI3716MV200  */
    HIGFX_CHIP_TYPE_HI3716MV300,     /**< HI3716MV300  */
    HIGFX_CHIP_TYPE_HI3716H,         /**< HI3716H      */
    HIGFX_CHIP_TYPE_HI3716CV100,     /**< HI3716C      */

    HIGFX_CHIP_TYPE_HI3720,     /**< HI3720       */
    HIGFX_CHIP_TYPE_HI3712V300, /**< X6V300       */
    HIGFX_CHIP_TYPE_HI3715,     /**< HI3715       */

    HIGFX_CHIP_TYPE_HI3716CV200ES, /**< S40V200      */
    HIGFX_CHIP_TYPE_HI3716CV200,   /**< HI3716CV200  */
    HIGFX_CHIP_TYPE_HI3719MV100,   /**< HI3719MV100  */
    HIGFX_CHIP_TYPE_HI3718CV100,   /**< HI3718CV100  */
    HIGFX_CHIP_TYPE_HI3719CV100,   /**< HI3719CV100  */
    HIGFX_CHIP_TYPE_HI3719MV100_A, /**< HI3719MV100_A*/
    HIGFX_CHIP_TYPE_HI3716MV400,   /* <  HI3716MV400  */

    HIGFX_CHIP_TYPE_HI3531 = 100, /**< HI3531       */
    HIGFX_CHIP_TYPE_HI3521,       /**< HI3521       */
    HIGFX_CHIP_TYPE_HI3518,       /**< HI3518       */
    HIGFX_CHIP_TYPE_HI3520A,      /**< HI3520A      */
    HIGFX_CHIP_TYPE_HI3520D,      /**< HI3520D      */
    HIGFX_CHIP_TYPE_HI3535,       /**< HI3535       */

    HIGFX_CHIP_TYPE_BUTT = 400 /**< Invalid Chip */

} HIGFX_CHIP_TYPE_E;

typedef enum tagHIGFX_MODE_ID_E {
    HIGFX_TDE_ID = 0, /**< TDE ID          */
    HIGFX_JPGDEC_ID,  /**< JPEG DECODE ID  */
    HIGFX_JPGENC_ID,  /**< JPEG_ENCODE ID  */
    HIGFX_FB_ID,      /**<  FRAMEBUFFER ID */
    HIGFX_PNG_ID,     /**< PNG ID          */
    HIGFX_HIGO_ID,
    HIGFX_GFX2D_ID,
    HIGFX_BUTT_ID,
} HIGFX_MODE_ID_E;

/* ************************** Structure Definition ********************************************** */
typedef struct {
    hi_s32 (*proc_read)(struct seq_file *, hi_void *);
    hi_s32 (*proc_write)(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
    hi_s32 (*proc_ioctl)(struct seq_file *, hi_u32 cmd, hi_u32 arg);
} hi_gfx_proc_item;

/* ********************* Global Variable declaration ******************************************** */

extern unsigned long long sched_clock(void);

/* ****************************** API declaration *********************************************** */
/***************************************************************************
* func           : HI_GFX_SetIrq
* description    : set irq to cpu
                   CNcomment: 将中断号绑定到对应得CPU上 CNend\n
* param[in]      : u32ModId   CNcomment: 模块ID CNend\n
* param[in]      : u32IrqNum  CNcomment: 中断号 CNend\n
* retval         : HI_SUCCESS 成功
* retval         : HI_FAILURE 失败
* others:        : NA
****************************************************************************/
static inline hi_s32 HI_GFX_SetIrq(hi_u32 u32ModId, hi_u32 u32IrqNum, const hi_char *pIrqName)
{
#ifdef CONFIG_GFX_STB_SDK
    hi_s32 s32Ret = HI_SUCCESS;

    if (NULL == pIrqName) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    s32Ret = hi_drv_sys_set_irq_affinity(ConvertID(u32ModId), u32IrqNum, pIrqName);
    if (HI_SUCCESS != s32Ret) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
#else
#endif
    return HI_SUCCESS;
}

/***************************************************************************
* func          : HI_GFX_GetTimeStamp
* description   : get time function.
                        CNcomment:获取时间函数 CNend\n
* param[out]    : pu32TimeMs  CNcomment: 获取ms CNend\n
* param[out]    : pu32TimeUs  CNcomment: 获取us CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 HI_GFX_GetTimeStamp(hi_u32 *pu32TimeMs, hi_u32 *pu32TimeUs)
{
    hi_u64 u64TimeNow = 0;

    if (HI_NULL == pu32TimeMs) {
        GRAPHIC_COMM_PRINT("[module-gfx][err] : %s %d failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    u64TimeNow = osal_sched_clock();

    do_div(u64TimeNow, (hi_u64)1000000);

    if (u64TimeNow > UINT_MAX) {
        *pu32TimeMs = UINT_MAX;
    } else {
        *pu32TimeMs = (hi_u32)u64TimeNow;
    }

    return HI_SUCCESS;
}

/***************************************************************************
 * func          : HI_GFX_KZALLOC
 * description   : kzalloc buffer
 * retval        : mem address
 * others:       : NA
 ****************************************************************************/
#ifdef CONFIG_GFX_STB_SDK
static inline hi_void *HI_GFX_KZALLOC(hi_u32 u32ModuleId, hi_u32 u32Size, hi_s32 s32Flags)
{
    hi_void *pBuf = NULL;

    if ((u32Size > CONFIG_GFX_MIN_MEM_SIZE) && (u32Size < CONFIG_GFX_MAX_MEM_SIZE)) {
        pBuf = HI_KZALLOC(ConvertID(u32ModuleId), u32Size, s32Flags);
    }

    return pBuf;
}
#endif

#if defined(CONFIG_GFX_STB_SDK) || defined(CONFIG_GFX_ANDROID_SDK) || defined(CONFIG_GFX_TV_SDK)
/***************************************************************************
 * func          : HI_GFX_KMALLOC
 * description   : kmalloc buffer
 * retval        : mem address
 * others:       : NA
 ****************************************************************************/
static inline hi_void *HI_GFX_KMALLOC(hi_u32 u32ModuleId, hi_u32 u32Size, hi_s32 s32Flags)
{
    hi_void *pBuf = NULL;

    if ((u32Size > CONFIG_GFX_MIN_MEM_SIZE) && (u32Size < CONFIG_GFX_MAX_MEM_SIZE)) {
        pBuf = osal_kmalloc(ConvertID(u32ModuleId), u32Size, s32Flags);
    }

    return pBuf;
}

/***************************************************************************
 * func          : HI_GFX_VMALLOC
 * description   : vmalloc buffer
 * retval        : mem address
 * others:       : NA
 ****************************************************************************/
static inline hi_void *HI_GFX_VMALLOC(hi_u32 u32ModuleId, hi_u32 u32Size)
{
    hi_void *pBuf = NULL;

    if ((u32Size > CONFIG_GFX_MIN_MEM_SIZE) && (u32Size < CONFIG_GFX_MAX_MEM_SIZE)) {
        pBuf = osal_vmalloc(ConvertID(u32ModuleId), u32Size);
    }

    return pBuf;
}

/***************************************************************************
 * func          : HI_GFX_KFREE
 * description   : free buffer
 * retval        : NA
 * others:       : NA
 ****************************************************************************/
static inline hi_void HI_GFX_KFREE(hi_u32 u32ModuleId, hi_void *pBuf)
{
    if (pBuf != HI_NULL) {
        osal_kfree(ConvertID(u32ModuleId), pBuf);
        pBuf = HI_NULL;
    }

    return;
}

/***************************************************************************
 * func          : HI_GFX_VFREE
 * description   : vfree buffer
 * retval        : NA
 * others:       : NA
 ****************************************************************************/
static inline hi_void HI_GFX_VFREE(hi_u32 u32ModuleId, hi_void *pBuf)
{
    if (pBuf != HI_NULL) {
        osal_vfree(ConvertID(u32ModuleId), pBuf);
        pBuf = HI_NULL;
    }

    return;
}

/***************************************************************************
* func          : drv_gfx_mem_alloc
* description   : alloc the mem that need
                  CNcomment: 分配需要的内存 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 drv_gfx_mem_alloc(drv_gfx_mem_info *mem_info, const hi_char *name, hi_bool is_smmu, hi_bool is_cache,
                                       hi_u32 size)
{
    hi_void *dmabuf = HI_NULL;
    unsigned long addr;
    osal_mem_type mem_type = OSAL_MMZ_TYPE;

    if (mem_info == HI_NULL || size < CONFIG_GFX_MIN_MEM_SIZE || size > CONFIG_GFX_MAX_MEM_SIZE) {
        return HI_FAILURE;
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    mem_type = (is_smmu == HI_TRUE) ? OSAL_NSSMMU_TYPE : OSAL_MMZ_TYPE;
#endif

    dmabuf = osal_mem_alloc(name, size, mem_type, HI_NULL, 0);
#ifdef CONFIG_GFX_MMU_SUPPORT
    if (is_smmu) {
        if (dmabuf != HI_NULL) {
            addr = osal_mem_nssmmu_map(dmabuf, 0);
            mem_info->dmabuf = dmabuf;
            mem_info->smmu_addr = addr;
            mem_info->phy_addr = addr;
            mem_info->is_smmu = HI_TRUE;
        }
    } else
#endif
    {
        if (dmabuf != HI_NULL) {
            addr = osal_mem_phys(dmabuf);
            mem_info->dmabuf = dmabuf;
            mem_info->mmz_addr = addr;
            mem_info->phy_addr = addr;
            mem_info->is_smmu = HI_FALSE;
        }
    }

    if (mem_info->phy_addr == 0) {
        return HI_FAILURE;
    }
    osal_mem_flush(mem_info->dmabuf);
    return HI_SUCCESS;
}

/***************************************************************************
* func          : drv_gfx_mem_map_to_smmu
* description   :
                  CNcomment: map mmz to smmu CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_u32 drv_gfx_mem_map_to_smmu(drv_gfx_mem_info *mem_info)
{
    if (mem_info == HI_NULL || mem_info->dmabuf == HI_NULL) {
        return HI_FAILURE;
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    mem_info->smmu_addr = osal_mem_nssmmu_map(mem_info->dmabuf, 0);
    if (mem_info->smmu_addr != 0) {
        return HI_SUCCESS;
    }

    return HI_FAILURE;
#else
    mem_info->smmu_addr = mem_info->mmz_addr;
    return HI_SUCCESS;
#endif
}

/***************************************************************************
* func          : drv_gfx_mem_unmap_from_smmu
* description   :
                  CNcomment: un map mmz to smmu CNend\n
* retval        : smmu mem
* others:       : NA
****************************************************************************/
static inline hi_s32 drv_gfx_mem_unmap_from_smmu(drv_gfx_mem_info *mem_info)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_s32 ret;
    if (mem_info == HI_NULL || mem_info->dmabuf == HI_NULL) {
        return HI_FAILURE;
    }
    if (mem_info->smmu_addr != 0) {
        ret = osal_mem_nssmmu_unmap(mem_info->dmabuf, mem_info->smmu_addr, 0);
        mem_info->smmu_addr = 0;
    }
#endif
    return HI_SUCCESS;
}

/***************************************************************************
* func          : drv_gfx_mem_map
* description   :
                  CNcomment: 映射不带cache的虚拟内存 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 drv_gfx_mem_map(drv_gfx_mem_info *mem_info)
{
    if (mem_info == HI_NULL || mem_info->dmabuf == HI_NULL) {
        return HI_FAILURE;
    }

    mem_info->virtual_addr = osal_mem_kmap(mem_info->dmabuf, 0, 0);

    if (mem_info->virtual_addr != HI_NULL) {
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

/***************************************************************************
* func            : HI_GFX_Flush
* description     :
                    CNcomment: 地址内容刷新 CNend\n
* retval          : HI_SUCCESS 成功
* retval          : HI_FAILURE 失败
* others:         : NA
****************************************************************************/
static inline hi_void drv_gfx_mem_flush(drv_gfx_mem_info *mem_info, hi_bool cpu_to_phy)
{
    if (mem_info == HI_NULL || mem_info->dmabuf == HI_NULL) {
        return;
    }

    if (cpu_to_phy == HI_TRUE) {
        dma_buf_end_cpu_access(mem_info->dmabuf, DMA_TO_DEVICE);
    } else {
        osal_mem_flush(mem_info->dmabuf);
    }

    return;
}

/***************************************************************************
* func             : drv_gfx_mem_unmap
* description      : un map phy ddr
                     CNcomment: 逆映射物理地址 CNend\n
* retval           : HI_SUCCESS 成功
* retval           : HI_FAILURE 失败
* others:         : NA
****************************************************************************/
static inline hi_s32 drv_gfx_mem_unmap(drv_gfx_mem_info *mem_info)
{
    if (mem_info == HI_NULL || mem_info->dmabuf == HI_NULL) {
        return HI_FAILURE;
    }

    if (mem_info->virtual_addr != HI_NULL) {
        osal_mem_kunmap(mem_info->dmabuf, mem_info->virtual_addr, 0);
        mem_info->virtual_addr = HI_NULL;
    }

    return HI_SUCCESS;
}

/***************************************************************************
* func          : drv_gfx_mem_free
* description   : free the mem that has alloced
                  CNcomment: 释放分配过的内存 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_void drv_gfx_mem_free(drv_gfx_mem_info *mem_info)
{
    if (mem_info == HI_NULL || mem_info->dmabuf == HI_NULL) {
        return;
    }

    drv_gfx_mem_unmap(mem_info);

#ifdef CONFIG_GFX_MMU_SUPPORT
    (hi_void) drv_gfx_mem_unmap_from_smmu(mem_info);
#endif
    osal_mem_free(mem_info->dmabuf);
    mem_info->mmz_addr = 0;
    mem_info->phy_addr = 0;
    mem_info->dmabuf = 0;
}

/***************************************************************************
* func          : drv_gfx_mem_alloc_map
* description   : alloc the mem that need
                  CNcomment: 分配需要的内存 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 drv_gfx_mem_alloc_map(drv_gfx_mem_info *mem_info, const hi_char *name, hi_bool is_smmu, hi_bool is_cache,
                                           hi_u32 size)
{
    hi_s32 ret;

    ret = drv_gfx_mem_alloc(mem_info, name, is_smmu, is_cache, size);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    ret = drv_gfx_mem_map(mem_info);
    if (ret != HI_SUCCESS) {
        drv_gfx_mem_free(mem_info);
    }
    return ret;
}

static inline unsigned long drv_gfx_mem_get_addr_from_dma_buf(hi_void *dmabuf)
{
    return osal_mem_nssmmu_map(dmabuf, 0);
}

static inline unsigned long drv_gfx_mem_get_smmu_from_fd(hi_mem_handle mem_handle, hi_s32 module_id)
{
    unsigned long addr;
    hi_void *dmabuf = HI_NULL;

    if (mem_handle.mem_handle <= 0) {
        return 0;
    }
    dmabuf = osal_mem_handle_get(mem_handle.mem_handle, ConvertID(module_id));
    if (IS_ERR_OR_NULL(dmabuf)) {
        return 0;
    }

    addr = osal_mem_nssmmu_map(dmabuf, 0);
    osal_mem_ref_put(dmabuf, ConvertID(module_id));
    return (addr == 0) ? 0 : addr + mem_handle.addr_offset;
}

/***************************************************************************
 * func          : HI_GFX_MODULE_Register
 * description   : 注册模块
 * retval        : HI_SUCCESS 成功
 * retval        : HI_FAILURE 失败
 * others:       : NA
 ****************************************************************************/
static inline hi_s32 hi_gfx_module_register(hi_u32 u32ModuleID, const hi_char *name, hi_void *pData)
{
    hi_s32 Ret = HI_FAILURE;

    if ((NULL != name) && (NULL != pData)) {
        Ret = osal_exportfunc_register(ConvertID(u32ModuleID), name, pData);
    }

    return Ret;
}

/***************************************************************************
 * func          : hi_gfx_module_unregister
 * description   : 删除模块
 * retval        : HI_SUCCESS 成功
 * retval        : HI_FAILURE 失败
 * others:       : NA
 ****************************************************************************/
static inline hi_s32 hi_gfx_module_unregister(hi_u32 u32ModuleID)
{
    return osal_exportfunc_unregister(ConvertID(u32ModuleID));
}

#endif

/* * @} */ /* ! <!-- API declaration end */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* _HI_GFX_COMM_K_H_ */
