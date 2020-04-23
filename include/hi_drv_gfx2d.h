/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: delivered by other modules header
 * Author: sdk
 * Create: 2018-12-10
 */

#ifndef  __HI_DRV_GFX2D_H__
#define  __HI_DRV_GFX2D_H__

#include "hi_type.h"
#include "hi_drv_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hi_s32 (*gfx2d_module_init)(hi_void);
typedef hi_void (*gfx2d_module_exit)(hi_void);
/* typedef hi_s32 (*gfx2d_module_suspend)(PM_BASEDEV_S *, pm_message_t); */
/* typedef hi_s32 (*gfx2d_module_resume)(PM_BASEDEV_S *); */

typedef struct {
    gfx2d_module_init    drv_gfx2d_module_init;
    gfx2d_module_exit    drv_gfx2d_module_exit;
    /* gfx2d_module_suspend drv_gfx2d_module_suspend; */
    /* gfx2d_module_resume  drv_gfx2d_module_resume; */
} hi_gfx2d_export_func;

hi_s32 hi_drv_gfx2d_module_init(hi_void);
hi_void hi_drv_gfx2d_module_exit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HI_DRV_GFX2D_H__ */
