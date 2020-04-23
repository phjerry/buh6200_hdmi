/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adp manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_ADP__
#define __SOURCE_MSP_DRV_TDE_ADP__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TDE_REG_BASEADDR 0x01400000
#define TDE_INTNUM (116 + 32)
#define TDE_MMU_REG_BASEADDR (TDE_REG_BASEADDR + 0xF000)

#define SIZE_256BYTE_ALIGN

#ifndef HI_BUILD_IN_BOOT
#define TDE_HWC_COOPERATE
#endif

#ifndef SIZE_256BYTE_ALIGN
#define CMD_SIZE 64
#define JOB_SIZE 96
#define NODE_SIZE 208
#define FILTER_SIZE 960
#else
#define CMD_SIZE 64
#define JOB_SIZE 96
#define FILTER_SIZE 1792
#define NODE_SIZE 7712
#endif

#define HI_TDE_FILTER_NUM 3

#define CONFIG_TDE_TDE_EXPORT_FUNC
// #define CONFIG_TDE_PM_ENABLE
#define DESCRIPTION "Hisilicon TDE Device driver"
#define AUTHOR "Digital Media Team, Hisilicon crop."
#define TDE_VERSION "V1.0.0.0"

#define TDE_NO_SCALE_VSTEP 0x1000
#define TDE_NO_SCALE_HSTEP 0x100000
#define TDE_FLOAT_BITLEN 12
#define TDE_HAL_HSTEP_FLOATLEN 20
#define TDE_HAL_VSTEP_FLOATLEN 12
#define TDE_MAX_SLICE_WIDTH 512
#define TDE_MAX_SLICE_NUM 35

#ifdef HI_BUILD_IN_BOOT
#define TDE_MAX_RECT_WIDTH 8192
#else
#define TDE_MAX_RECT_WIDTH 8192
#endif

#define TDE_MAX_RECT_HEIGHT 8192
#define TDE_MAX_SLICE_RECT_WIDTH 0xffe
#define TDE_MAX_SLICE_RECT_HEIGHT 0xffe

#define TDE_MAX_SURFACE_PITCH 0x1FFFFF
#define TDE_MAX_ZOOM_OUT_STEP 8
#define TDE_MAX_RECT_WIDTH_EX 0x2000
#define TDE_MAX_RECT_HEIGHT_EX 0x2000

#define TDE_MAX_MINIFICATION_H 255
#define TDE_MAX_MINIFICATION_V 255

#define ROP 0x1                /* Rop        */
#define ALPHABLEND (0x1 << 1)  /* AlphaBlend */
#define COLORIZE (0x1 << 2)    /* Colorize   */
#define CLUT (0x1 << 3)        /* Clut       */
#define COLORKEY (0x1 << 4)    /* ColorKey   */
#define CLIP (0x1 << 5)        /* Clip       */
#define DEFLICKER (0x1 << 6)   /* Deflicker  */
#define RESIZE (0x1 << 7)      /* Resize     */
#define MIRROR (0x1 << 8)      /* Mirror     */
#define CSCCOVERT (0x1 << 9)   /* CSC        */
#define QUICKCOPY (0x1 << 10)  /* 快速拷贝   */
#define QUICKFILL (0x1 << 11)  /* 快速填充   */
#define PATTERFILL (0x1 << 12) /* 模式填充   */
#define MASKROP (0x1 << 13)    /* MaskRop    */
#define MASKBLEND (0x1 << 14)  /* MaskBlend  */

hi_void tde_get_capability(hi_u32 *pst_capability);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_ADP__ */
