/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb config header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_CONFIG_H__
#define __DRV_HIFB_CONFIG_H__

/* ********************************add include here*********************************************** */

#include "hi_type.h"

/*************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************* */
#define CONFIG_HIFB_LAYER_0BUFFER 0
#define CONFIG_HIFB_LAYER_1BUFFER 1
#define CONFIG_HIFB_LAYER_2BUFFER 2
#define CONFIG_HIFB_LAYER_3BUFFER 3

#define CONFIG_HIFB_STEREO_BUFFER_MAX_NUM 2
#if defined(CFG_HIFB_ANDROID_SUPPORT) || defined(CONFIG_HIFB_DISP_WINDOW_SYNC_SUPPORT)
#define CONFIG_HIFB_LAYER_BUFFER_MAX_NUM 3
#else
#define CONFIG_HIFB_LAYER_BUFFER_MAX_NUM 3
#endif

#define CONFIG_HIFB_CHN_OFFSET 0x400
#define CONFIG_HIFB_WBC_OFFSET 0x400

#define CONFIG_HIFB_CSC_OFFSET (0x90E00)

#define CONFIG_HIFB_G3_CSC_OFFSET (0x5700)

#define CONFIG_HIFB_CTL_OFFSET 0x800
#define CONFIG_HIFB_GFX_OFFSET 0x200
#define CONFIG_HIFB_GP_OFFSET 0x100
#define CONFIG_HIFB_GP_ZME_OFFSET (0x8F900)
#define CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET 0x100

#define CONFIG_VDP_SIZE 0xf000

#ifdef CONFIG_HIFB_VERSION_4_0
#define CONFIG_VDP_REG_BASEADDR (0x00f00000)
#else
#define CONFIG_VDP_REG_BASEADDR (0xf8cc0000)
#endif

#if defined(CHIP_TYPE_hi3798cv200) || defined(CHIP_TYPE_hi3798mv200) || defined(CHIP_TYPE_hi3796mv200) || \
    defined(CHIP_TYPE_hi3716mv450) || defined(CHIP_TYPE_hi3798mv300)
#define CONFIG_HIFB_HD0_REG_BASEADDR (0xf8cc7000)
#define CONFIG_HIFB_GP0_REG_BASEADDR (0xf8cc8000)
#define CONFIG_HIFB_SD_LOGO_REG_BASEADDR (0xf8cc7800)
#define CONFIG_HIFB_WBC_SLAYER_REG_BASEADDR (0xf8cc7800)
#define CONFIG_HIFB_WBC_GP0_REG_BASEADDR (0xf8cc9400)
#elif defined(CHIP_TYPE_hi3798mv310) || defined(CHIP_TYPE_hi3716mv430)
#define CONFIG_HIFB_HD0_REG_BASEADDR (0xf8cc5000)
#define CONFIG_HIFB_GP0_REG_BASEADDR (0xf8cc6800)
#define CONFIG_HIFB_SD_LOGO_REG_BASEADDR (0xf8cc5800)
#define CONFIG_HIFB_WBC_SLAYER_REG_BASEADDR (0xf8cc5800)
#define CONFIG_HIFB_WBC_GP0_REG_BASEADDR (0xf8cc7800)
#elif defined(CHIP_TYPE_hi3716mv410) || defined(CHIP_TYPE_hi3716mv420)
#define CONFIG_HIFB_HD0_REG_BASEADDR (0xf8cc6000)
#define CONFIG_HIFB_GP0_REG_BASEADDR (0xf8cc9000)
#define CONFIG_HIFB_SD_LOGO_REG_BASEADDR (0xf8cc8000)
#define CONFIG_HIFB_WBC_SLAYER_REG_BASEADDR (0xf8cc8000)
#define CONFIG_HIFB_WBC_GP0_REG_BASEADDR (0xf8cca800)
#else
#define CONFIG_HIFB_HD0_REG_BASEADDR (0xf8cc6000)
#define CONFIG_HIFB_GP0_REG_BASEADDR (0xf8cc9000)
#define CONFIG_HIFB_SD_LOGO_REG_BASEADDR (0xf8cc7800)
#define CONFIG_HIFB_WBC_SLAYER_REG_BASEADDR (0xf8cc8000)
#define CONFIG_HIFB_WBC_GP0_REG_BASEADDR (0xf8cca800)
#endif

#if defined(CHIP_TYPE_hi3798mv100) || defined(CHIP_TYPE_hi3796mv100) || defined(CHIP_TYPE_hi3716dv100) || \
    defined(CHIP_TYPE_hi3716mv410) || defined(CHIP_TYPE_hi3716mv420)
#define CONFIG_HIFB_GP0_SUPPORT_GFX_COUNT 2 /* * (G0 G1) * */
#define CONFIG_HIFB_GP1_SUPPORT_GFX_COUNT 1 /* * (G4)    * */
#elif defined(CHIP_TYPE_hi3798cv200)
#define CONFIG_HIFB_GP0_SUPPORT_GFX_COUNT 3 /* * (G0 G1 G3) * */
#define CONFIG_HIFB_GP1_SUPPORT_GFX_COUNT 1 /* * (G4) * */
#elif defined(CHIP_TYPE_hi3798mv200) || defined(CHIP_TYPE_hi3798mv300) || defined(CHIP_TYPE_hi3798mv310) || \
    defined(CHIP_TYPE_hi3796mv200) || defined(CHIP_TYPE_hi3716mv450)
#define CONFIG_HIFB_GP0_SUPPORT_GFX_COUNT 2 /* * (G0 G1) * */
#define CONFIG_HIFB_GP1_SUPPORT_GFX_COUNT 1 /* * (G4) * */
#elif defined(CHIP_TYPE_hi3716mv430)
#define CONFIG_HIFB_GP0_SUPPORT_GFX_COUNT 2 /* * (G0 G3) * */
#define CONFIG_HIFB_GP1_SUPPORT_GFX_COUNT 1 /* * (G1) * */
#else
#define CONFIG_HIFB_GP0_SUPPORT_GFX_COUNT 4 /* * (G0 G1 G2)  * */
#define CONFIG_HIFB_GP1_SUPPORT_GFX_COUNT 2 /* * (G3(±äG5) G4 G5) * */
#endif

#ifndef HI_BUILD_IN_BOOT
#if defined(CHIP_TYPE_hi3716mv410) || defined(CHIP_TYPE_hi3716mv420)
#define CONFIG_HIFB_HD1_LAYER_16BIT_FMT 1
#define CONFIG_HIFB_HD1_LAYER_24BIT_FMT 0
#define CONFIG_HIFB_HD1_LAYER_32BIT_FMT 0
#define CONFIG_HIFB_HD1_LAYER_CLUT1_FMT 0
#define CONFIG_HIFB_HD1_LAYER_CLUT2_FMT 0
#define CONFIG_HIFB_HD1_LAYER_CLUT4_FMT 0
#define CONFIG_HIFB_HD1_LAYER_CLUT8_FMT 0
#else
#define CONFIG_HIFB_HD1_LAYER_16BIT_FMT 1
#define CONFIG_HIFB_HD1_LAYER_24BIT_FMT 1
#define CONFIG_HIFB_HD1_LAYER_32BIT_FMT 1
#define CONFIG_HIFB_HD1_LAYER_CLUT1_FMT 1
#define CONFIG_HIFB_HD1_LAYER_CLUT2_FMT 1
#define CONFIG_HIFB_HD1_LAYER_CLUT4_FMT 1
#define CONFIG_HIFB_HD1_LAYER_CLUT8_FMT 1
#endif
#else
#define CONFIG_HIFB_HD1_LAYER_16BIT_FMT 1
#define CONFIG_HIFB_HD1_LAYER_24BIT_FMT 1
#define CONFIG_HIFB_HD1_LAYER_32BIT_FMT 1
#define CONFIG_HIFB_HD1_LAYER_CLUT1_FMT 0
#define CONFIG_HIFB_HD1_LAYER_CLUT2_FMT 0
#define CONFIG_HIFB_HD1_LAYER_CLUT4_FMT 0
#define CONFIG_HIFB_HD1_LAYER_CLUT8_FMT 0
#endif

#define CONFIG_HIFB_LAYER_MAXWIDTH 3840
#define CONFIG_HIFB_LAYER_MAXHEIGHT 2160

#define CONFIG_HIFB_DEFAULT_DEPTH 32
#define CONFIG_HIFB_STRIDE_ALIGN 64
#define CONFIG_HIFB_LAYER_MINWIDTH 32
#define CONFIG_HIFB_LAYER_MINHEIGHT 32
#define CONFIG_HIFB_LAYER_MAXSTRIDE ((CONFIG_HIFB_LAYER_MAXWIDTH * 4) * 2) /* * need > w * 4 * 1.3 * */
#define CONFIG_HIFB_LAYER_MINSTRIDE 32
#define CONFIG_HIFB_LAYER_MINSIZE 32
#define CONFIG_HIFB_LAYER_BITSPERPIX 32

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define CONFIG_HIFB_MMU_SUPPORT
#endif

#define CONFIG_HIFB_CMP_SUPPORT

#ifdef CHIP_TYPE_hi3798mv310
#define CONFIG_HIFB_WBC_UNSUPPORT
#endif

#define CONFIG_HIFB_GP1_SUPPORT
#define CONFIG_HIFB_GP1_INRECT_OFFSET_SUPPORT
#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define CONFIG_HIFB_DECOMPRESS_SEGMENT
#endif
/* ************************** Structure Definition *********************************************** */

/* ********************* Global Variable declaration ********************************************* */

/* ****************************** API declaration ************************************************ */

#ifdef __cplusplus

#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_HIFB_CONFIG_H__ */
