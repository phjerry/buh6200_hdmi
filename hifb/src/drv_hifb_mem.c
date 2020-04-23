/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb memory
 * Author: sdk
 * Create: 2016-01-01
 */

#include "drv_hifb_mem.h"
#include "drv_hifb_debug.h"
#include "hi_osal.h"

/* **************************** Macro Definition ************************************************ */

/* ************************** Structure Definition ********************************************** */

/* ********************* Global Variable declaration ******************************************** */

/* ****************************** API declaration *********************************************** */
static inline hi_u32 HIFB_MEM_GetBpp32FromPixelFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);
static inline hi_u32 HIFB_MEM_GetBpp24FromPixelFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);
static inline hi_u32 HIFB_MEM_GetBpp16FromPixelFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);

/* ****************************** API release *************************************************** */

/***************************************************************************************
 * func          : drv_hifb_mem_map
 * description   : CNcomment: 内存映射 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void *drv_hifb_mem_map(hi_void *dmabuf)
{
    hi_void *vaddr = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    vaddr = osal_mem_kmap(dmabuf, 0, 0);
    return vaddr;
}

/***************************************************************************************
 * func          : drv_hifb_mem_unmap
 * description   : CNcomment: 内存逆映射 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_mem_unmap(hi_void *dmabuf, void *vaddr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return osal_mem_kunmap(dmabuf, vaddr, 0);
}

/***************************************************************************************
 * func         : drv_hifb_mem_free
 * description  : CNcomment: 释放内存 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_mem_free(hi_void *dmabuf)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    osal_mem_free(dmabuf);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/***************************************************************************************
 * func         : drv_hifb_mem_alloc
 * description  : CNcomment: 分配内存 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
unsigned long drv_hifb_mem_get_phy_addr(hi_void *dmabuf)
{
    unsigned long addr = osal_mem_phys(dmabuf);
    return addr;
}

/***************************************************************************************
 * func        : drv_hifb_mem_map_to_smmu
 * description : CNcomment: map mmz to smmu mem CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
unsigned long drv_hifb_mem_map_to_smmu(hi_void *dmabuf)
{
    unsigned long addr = osal_mem_nssmmu_map(dmabuf, 0);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return addr;
}

/***************************************************************************************
 * func         : drv_hifb_mem_unmap_from_smmu
 * description  : CNcomment: un map smmu mem CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 drv_hifb_mem_unmap_from_smmu(hi_void *dmabuf, unsigned long addr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return osal_mem_nssmmu_unmap(dmabuf, addr, 0);
}

unsigned long drv_hifb_mem_get_smmu_from_fd(hi_mem_handle mem_handle)
{
    unsigned long addr;
    hi_void *dmabuf = HI_NULL;
    if (mem_handle.mem_handle <= 0) {
        return 0;
    }

    dmabuf = osal_mem_handle_get(mem_handle.mem_handle, ConvertID(HIGFX_FB_ID));
    if (IS_ERR_OR_NULL(dmabuf)) {
        return 0;
    }

    addr = osal_mem_nssmmu_map(dmabuf, 0);

    osal_mem_ref_put(dmabuf, ConvertID(HIGFX_FB_ID));

    return (addr == 0) ? 0 : addr + mem_handle.addr_offset;
}

/**< check these two rectangle cover each other >**/
hi_bool DRV_HIFB_MEM_IsConTain(HIFB_RECT *pstParentRect, HIFB_RECT *pstChildRect)
{
    HIFB_POINT_S stPoint = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstParentRect, HI_FALSE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstChildRect, HI_FALSE);

    stPoint.s32XPos = pstChildRect->x;
    stPoint.s32YPos = pstChildRect->y;

    GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(pstParentRect->w, pstParentRect->x, HI_FALSE);
    GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(pstParentRect->h, pstParentRect->y, HI_FALSE);

    if ((stPoint.s32XPos < pstParentRect->x) || (stPoint.s32XPos > (pstParentRect->x + pstParentRect->w)) ||
        (stPoint.s32YPos < pstParentRect->y) || (stPoint.s32YPos > (pstParentRect->y + pstParentRect->h))) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stPoint.s32XPos);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stPoint.s32YPos);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->w);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->h);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_FALSE;
    }

    stPoint.s32XPos = pstChildRect->x + pstChildRect->w;
    stPoint.s32YPos = pstChildRect->y + pstChildRect->h;

    if ((stPoint.s32XPos < pstParentRect->x) || (stPoint.s32XPos > (pstParentRect->x + pstParentRect->w)) ||
        (stPoint.s32YPos < pstParentRect->y) || (stPoint.s32YPos > (pstParentRect->y + pstParentRect->h))) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stPoint.s32XPos);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stPoint.s32YPos);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->w);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstParentRect->h);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_FALSE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_TRUE;
}

hi_u32 DRV_HIFB_MEM_GetBppByFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    hi_u32 BppDepth = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    BppDepth = HIFB_MEM_GetBpp32FromPixelFmt(enDataFmt);
    if (32 == BppDepth) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return BppDepth;
    }

    BppDepth = HIFB_MEM_GetBpp24FromPixelFmt(enDataFmt);
    if (24 == BppDepth) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return BppDepth;
    }

    BppDepth = HIFB_MEM_GetBpp16FromPixelFmt(enDataFmt);
    if (16 == BppDepth) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return BppDepth;
    }

    switch (enDataFmt) {
        case DRV_HIFB_FMT_1BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 1;
        case DRV_HIFB_FMT_2BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 2;
        case DRV_HIFB_FMT_4BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 4;
        case DRV_HIFB_FMT_8BPP:
        case DRV_HIFB_FMT_ACLUT44:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 8;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
    }
}

static inline hi_u32 HIFB_MEM_GetBpp32FromPixelFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case DRV_HIFB_FMT_KRGB888:
        case DRV_HIFB_FMT_ARGB8888:
        case DRV_HIFB_FMT_RGBA8888:
        case DRV_HIFB_FMT_ABGR8888:
        case DRV_HIFB_FMT_KBGR888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 32;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
    }
}

static inline hi_u32 HIFB_MEM_GetBpp24FromPixelFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case DRV_HIFB_FMT_RGB888:
        case DRV_HIFB_FMT_ARGB8565:
        case DRV_HIFB_FMT_RGBA5658:
        case DRV_HIFB_FMT_ABGR8565:
        case DRV_HIFB_FMT_BGR888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 24;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
    }
}

static inline hi_u32 HIFB_MEM_GetBpp16FromPixelFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case DRV_HIFB_FMT_RGB565:
        case DRV_HIFB_FMT_KRGB444:
        case DRV_HIFB_FMT_KRGB555:
        case DRV_HIFB_FMT_ARGB4444:
        case DRV_HIFB_FMT_ARGB1555:
        case DRV_HIFB_FMT_RGBA4444:
        case DRV_HIFB_FMT_RGBA5551:
        case DRV_HIFB_FMT_ACLUT88:
        case DRV_HIFB_FMT_BGR565:
        case DRV_HIFB_FMT_ABGR1555:
        case DRV_HIFB_FMT_ABGR4444:
        case DRV_HIFB_FMT_KBGR444:
        case DRV_HIFB_FMT_KBGR555:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 16;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
    }
}

/***************************************************************************************
 * func         : DRV_HIFB_MEM_BitFieldCmp
 * description  : CNcomment: 判断两个像素格式是否相等 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_MEM_BitFieldCmp(struct fb_bitfield x, struct fb_bitfield y)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if ((x.offset == y.offset) && (x.length == y.length) && (x.msb_right == y.msb_right)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, x.offset);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, y.offset);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, x.length);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, y.length);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, x.msb_right);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, y.msb_right);
    return HI_FAILURE;
}

#ifndef HI_BUILD_IN_BOOT
static hi_u32 hifb_calc_deno_minators(hi_u32 bpp)
{
    hi_u32 deno_minators;

    if (bpp == 1) {
        deno_minators = 8; /* deno is 8 */
    } else if (bpp == 2) { /* bpp is 2 */
        deno_minators = 4; /* deno is 4 */
    } else if (bpp == 4) { /* bpp is 4 */
        deno_minators = 2; /* deno is 2 */
    } else {
        deno_minators = 1;
    }

    return deno_minators;
}

#ifdef CFG_HI_FB_DECOMPRESS_SUPPORT
#ifdef CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE
hi_u32 drv_hifb_mem_get_max_stride_width_line_compress(hi_u32 width, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align)
{
    hi_u32 deno_minators;
    hi_u32 pix_depth;
    hi_u32 no_cmp_stride;

    deno_minators = hifb_calc_deno_minators(bpp);
    if (deno_minators == 0) {
        deno_minators = 1;
    }
    pix_depth = (bpp * deno_minators) >> 3; /* 3 for 8bit */

    no_cmp_stride = ((width * pix_depth) / deno_minators + align - 1) & (~(align - 1));

    if (cmp_stride == HI_NULL) {
        return no_cmp_stride;
    }
    *cmp_stride = (pix_depth != 4) ? (0) : /* 4 for argb8888 */
                      (((hi_u32)(((width * pix_depth) * 13 + 9) / 10) + (align - 1)) & (~(align - 1))); /* 13 9 10 is
                                                                                                           alg num */

    return (no_cmp_stride > *cmp_stride) ? (no_cmp_stride) : (*cmp_stride);
}
#elif defined(CONFIG_HIFB_DECOMPRESS_SEGMENT)
hi_u32 drv_hifb_mem_get_max_stride_width_seg_compress(hi_u32 width, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align)
{
    hi_u32 deno_minators;
    hi_u32 pix_depth;
    hi_u32 no_cmp_stride;
    hi_u32 disp_buff_stride = 0;

    deno_minators = hifb_calc_deno_minators(bpp);
    if (deno_minators == 0) {
        deno_minators = 1;
    }
    pix_depth = (bpp * deno_minators) >> 3; /* 3 for 8bit */

    no_cmp_stride = ((width * pix_depth) / deno_minators + align - 1) & (~(align - 1));
    if (cmp_stride == NULL) {
        return no_cmp_stride;
    }

    if (pix_depth != 3 && pix_depth != 4 && pix_depth != 5) { /* 3 for RGB888, 4 for ARGB8888, 5 for ARGB10101010 */
        *cmp_stride = 0;
    } else {
        *cmp_stride = no_cmp_stride;
        disp_buff_stride = (*cmp_stride + CONFIG_HIFB_DECOMPRESS_HEADER_STRIDE + align - 1) & (~(align - 1));
    }

    return (no_cmp_stride > disp_buff_stride) ? (no_cmp_stride) : (disp_buff_stride);
}
#else
hi_u32 drv_hifb_mem_get_max_stride_width_block_compress(hi_u32 width, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align)
{
    hi_u32 deno_minators;
    hi_u32 pix_depth;
    hi_u32 no_cmp_stride;
    hi_u32 disp_buff_stride = 0;

    deno_minators = hifb_calc_deno_minators(bpp);
    if (deno_minators == 0) {
        deno_minators = 1;
    }
    pix_depth = (bpp * deno_minators) >> 3; /* 3 for 8bit */

    no_cmp_stride = ((width * pix_depth) / deno_minators + align - 1) & (~(align - 1));
    if (cmp_stride == HI_NULL) {
        return no_cmp_stride;
    }

    if (pix_depth != 4) { /* 4 for ARGB8888 */
        *cmp_stride = 0;
    } else {
        *cmp_stride = ((width * pix_depth / 2) + CONFIG_HIFB_DECOMPRESS_DATA_STRIDE_ALIGN - 1) & /* 2 for AR and GB */
                      (~(CONFIG_HIFB_DECOMPRESS_DATA_STRIDE_ALIGN - 1));
        *cmp_stride = (*cmp_stride % (CONFIG_HIFB_DECOMPRESS_DATA_STRIDE_ALIGN * 2))
                            ? /* 2 for AR and GB */
                          (*cmp_stride)
                            : (*cmp_stride + CONFIG_HIFB_DECOMPRESS_DATA_STRIDE_ALIGN);
        disp_buff_stride = ((CONFIG_HIFB_DECOMPRESS_HEADER_SIZE_ALIGN * 2 + *cmp_stride * 2) + /* 2 for AR and GB */
                            align - 1) &
                           (~(align - 1));
    }

    return (no_cmp_stride > disp_buff_stride) ? (no_cmp_stride) : (disp_buff_stride);
}
#endif

#else
hi_u32 drv_hifb_mem_get_max_stride_width_no_compress(hi_u32 width, hi_u32 bpp, hi_u32 *cmp_stride, hi_u32 align)
{
    hi_u32 deno_minators;
    hi_u32 pix_depth;

    if (cmp_stride == HI_NULL) {
        return 0;
    }

    deno_minators = hifb_calc_deno_minators(bpp);
    if (deno_minators == 0) {
        deno_minators = 1;
    }
    pix_depth = (bpp * deno_minators) >> 3; /* 3 for 8bit */

    *cmp_stride = ((width * pix_depth) / deno_minators + align - 1) & (~(align - 1));

    return *cmp_stride;
}
#endif
#endif
