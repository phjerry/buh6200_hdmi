/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: err msg
 * Create: 2019-02-15
 */
#ifndef _DRV_GFX2D_ERRCODE_H_
#define _DRV_GFX2D_ERRCODE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DRV_GFX2D_ERR_DEV_NOT_OPEN = 0x80000001, /* < device not open yet */
    DRV_GFX2D_ERR_DEV_PERMISSION,            /* < device operation failed */
    DRV_GFX2D_ERR_INVALID_DEVID,
    DRV_GFX2D_ERR_NULL_PTR,             /* < parameters contain null ptr */
    DRV_GFX2D_ERR_INVALID_COMPOSECNT,   /* < invalid composor count(>7 or =0) */
    DRV_GFX2D_ERR_INVALID_SURFACE_TYPE, /* < invalid surface info:colorfmt,phyaddr,stride,resolution... */
    DRV_GFX2D_ERR_INVALID_SURFACE_RESO,
    DRV_GFX2D_ERR_INVALID_SURFACE_FMT,
    DRV_GFX2D_ERR_INVALID_SURFACE_ADDR,
    DRV_GFX2D_ERR_INVALID_SURFACE_STRIDE,
    DRV_GFX2D_ERR_INVALID_SURFACE_CMPTYPE,
    DRV_GFX2D_ERR_INVALID_RECT,              /* < invalid opt rect:1.no intersection with surface */
    DRV_GFX2D_ERR_INVALID_RESIZE_FILTERMODE, /* < invalid resize info:1.invalid resize raito */
    DRV_GFX2D_ERR_INVALID_RESIZE_RATIO,
    DRV_GFX2D_ERR_INVALID_CLIP_MODE, /* < invalid clip info */
    DRV_GFX2D_ERR_INVALID_CLIP_RECT,
    DRV_GFX2D_ERR_UNSUPPORT, /* < unsupported operation */
    DRV_GFX2D_ERR_NO_MEM,    /* < lack of memory */
    DRV_GFX2D_ERR_TIMEOUT,   /* < sync task timeout */
    DRV_GFX2D_ERR_INTERRUPT, /* < sync task interrupted by system */
    DRV_GFX2D_ERR_SYS,
} drv_gfx2d_err_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _DRV_GFX2D_ERRCODE_H_ */
