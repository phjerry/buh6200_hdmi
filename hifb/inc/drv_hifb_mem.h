/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb memory header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_MEM_H__
#define __DRV_HIFB_MEM_H__

/* ********************************add include here********************************************** */
#include "drv_hifb_type.h"
#include <linux/dma-buf.h>
#include "hi_osal.h"

#include "hi_gfx_comm_k.h"
#include "drv_hifb_debug.h"
#include "drv_hifb_config.h"
/************************************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************* */
#define HIFB_FILE_PATH_MAX_LEN 256
#define HIFB_FILE_NAME_MAX_LEN 32

#define HIFB_MIN(m, n) (m) > (n) ? (n) : (m)

#define CONFIG_HIFB_DECOMPRESS_HEADER_STRIDE 16

#ifdef CONFIG_HIFB_DECOMPRESS_SEGMENT
#define CONFIG_HIFB_DECOMPRESS_HEADER_SIZE_ALIGN 1
#define CONFIG_HIFB_DECOMPRESS_DATA_STRIDE_ALIGN 1
#else
#define CONFIG_HIFB_DECOMPRESS_HEADER_SIZE_ALIGN 256
#define CONFIG_HIFB_DECOMPRESS_DATA_STRIDE_ALIGN 256
#endif

/* ************************** Structure Definition ********************************************** */
typedef struct {
    hi_u32 size;
    hi_void *dmabuf;
    unsigned long mmz_addr;
    unsigned long smmu_addr;
    hi_void *virtual_addr;
} drv_hifb_mem_info;

/* ********************* Global Variable declaration ******************************************** */

/* ****************************** API declaration *********************************************** */
hi_void *drv_hifb_mem_map(hi_void *dmabuf);
hi_void drv_hifb_mem_unmap(hi_void *dmabuf, void *vaddr);

hi_void drv_hifb_mem_free(hi_void *dmabuf);

unsigned long drv_hifb_mem_get_phy_addr(hi_void *dmabuf);
unsigned long drv_hifb_mem_map_to_smmu(hi_void *dmabuf);
hi_s32 drv_hifb_mem_unmap_from_smmu(hi_void *dmabuf, unsigned long iova);
unsigned long drv_hifb_mem_get_smmu_from_fd(hi_mem_handle mem_handle);

hi_bool DRV_HIFB_MEM_IsConTain(HIFB_RECT *pstParentRect, HIFB_RECT *pstChildRect);
hi_u32 DRV_HIFB_MEM_GetBppByFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);
hi_s32 DRV_HIFB_MEM_BitFieldCmp(struct fb_bitfield x, struct fb_bitfield y);

#ifdef CFG_HI_FB_DECOMPRESS_SUPPORT
#ifdef CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE
/* decompress stride equal to (no_cmp_stride * 1.3) */
hi_u32 drv_hifb_mem_get_max_stride_with_line_compress(hi_u32 widht, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align);
#define CONIFG_HIFB_GetMaxStride drv_hifb_mem_get_max_stride_with_line_compress
#elif defined(CONFIG_HIFB_DECOMPRESS_SEGMENT)
hi_u32 drv_hifb_mem_get_max_stride_width_seg_compress(hi_u32 widht, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align);
#define CONIFG_HIFB_GetMaxStride drv_hifb_mem_get_max_stride_width_seg_compress
#else
hi_u32 drv_hifb_mem_get_max_stride_width_block_compress(hi_u32 widht, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align);
#define CONIFG_HIFB_GetMaxStride drv_hifb_mem_get_max_stride_width_block_compress
#endif
#else
hi_u32 drv_hifb_mem_get_max_stride_width_no_compress(hi_u32 widht, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align);
#define CONIFG_HIFB_GetMaxStride drv_hifb_mem_get_max_stride_width_no_compress
#endif

/* ****************************** API release **************************************************** */
#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
* func         : DRV_HIFB_MEM_GetHeadStride
* description  : CNcomment: calc head stride CNend\n
                        AR head size:  (16 * height + 256 - 1) & (~(256 - 1));
* param[in]    : u32Stride
* param[in]    : u32Height
* retval       : NA
* others:      : NA
***************************************************************************************/
static inline hi_void DRV_HIFB_MEM_GetCmpHeadInfo(hi_u32 Height, hi_u32 *pHeadSize, hi_u32 *pHeadStride)
{
    if (NULL != pHeadStride) {
        *pHeadStride = CONFIG_HIFB_DECOMPRESS_HEADER_STRIDE;
    }

    if (NULL != pHeadSize) {
        *pHeadSize = (CONFIG_HIFB_DECOMPRESS_HEADER_STRIDE * Height + CONFIG_HIFB_DECOMPRESS_HEADER_SIZE_ALIGN - 1) &
                     (~(CONFIG_HIFB_DECOMPRESS_HEADER_SIZE_ALIGN - 1));
    }

    return;
}

/***************************************************************************************
* func         : HI_HIFB_GetMemSize
* description  : CNcomment: calc memory size CNend\n
                       AR head size:  (16 * height + 256 - 1) & (~(256 - 1));
                       AR data size:  CmpStride * height;
                       GB head size:  (16 * height + 256 - 1) & (~(256 - 1));
                       GB data size:  CmpStride * height;
* param[in]    : u32Stride
* param[in]    : u32Height
* retval       : NA
* others:      : NA
***************************************************************************************/
static inline hi_u32 HI_HIFB_GetMemSize(hi_u32 u32Stride, hi_u32 u32Height)
{
    hi_u32 TotalSize = 0;

    GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(u32Stride, u32Height, 0);
    TotalSize = u32Stride * u32Height;

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(TotalSize, (PAGE_SIZE - 1), 0);

    return ((TotalSize + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1)));
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
