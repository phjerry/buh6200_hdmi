/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: extended interface based on hifb.h
 * Create: 2019-06-29
 */

#ifndef __HIFB_LITEOS_H__
#define __HIFB_LITEOS_H__

#include "fb.h"
#include "hifb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* * \addtogroup  HIFB LITEOS      */
/* * @{ */ /* * <!-- [HIFB LITEOS] */

#define HIFBIOGET_SCREENINFO _IOR(HIFB_IOC_TYPE, 110, hifb_screeninfo)
#define HIFBIOPUT_SCREENINFO _IOW(HIFB_IOC_TYPE, 111, hifb_screeninfo)

/* * @} */ /* * <!-- ==== Macro Definition end ==== */

/* * \addtogroup     HIFB LITEOS   */
/* * @{ */ /* * <!-- [HIFB LITEOS] */

typedef struct {
    struct fb_vtable_s vtable;
    struct fb_videoinfo_s vinfo;
    struct fb_overlayinfo_s oinfo;
#ifdef CONFIG_FB_CMAP
    struct fb_cmap_s cmap;
#endif
    hi_s32 activate;
    hi_void *par;
} hifb_screeninfo;

/* * @} */ /* * <!-- ==== Structure Definition end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HIFB_LITEOS_H__ */
