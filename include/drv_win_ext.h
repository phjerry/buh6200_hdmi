/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: define for external use
* Author: vdp
* Create: 2019-06-28
 */

#ifndef __DRV_WIN_EXT_H__
#define __DRV_WIN_EXT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "hi_type.h"
#include "hi_drv_video.h"
#include "hi_drv_disp.h"
#include "hi_drv_win.h"
#include "hi_drv_dev.h"

typedef hi_s32 (*fn_win_init)(hi_void);
typedef hi_s32 (*fn_win_deinit)(hi_void);
typedef hi_s32 (*fn_win_create)(hi_drv_win_attr *winattr, hi_handle *win);
typedef hi_s32 (*fn_win_destroy)(hi_handle win);
typedef hi_s32 (*fn_win_setsource)(hi_handle window, hi_drv_win_src_handle *src_info);
typedef hi_s32 (*fn_win_setenable)(hi_handle win, hi_bool enable);
typedef hi_s32 (*fn_win_queueframe)(hi_handle win, hi_drv_video_frame *frameinfo);
typedef hi_s32 (*fn_win_dequeueframe)(hi_handle win, hi_drv_video_frame *frameinfo);
typedef hi_s32 (*fn_win_getplayinfo)(hi_handle win, hi_drv_win_play_info *info);
typedef hi_s32 (*fn_win_reset)(hi_handle win, hi_drv_win_reset_mode reset_mode);

typedef hi_s32 (*fn_win_suspend)(hi_void *dev, hi_void *state);
typedef hi_s32 (*fn_win_resume)(hi_void *dev);
typedef hi_s32 (*fn_win_set_vir_attr)(hi_handle win, hi_u32 width, hi_u32 height, hi_u32 frmRate);
typedef hi_s32 (*fn_win_releaseframe)(hi_handle win, hi_drv_video_frame *frameinfo);

typedef struct
{
    fn_win_init pfnwin_init;               /* mce */
    fn_win_deinit pfnwin_deinit;           /* mce */
    fn_win_create pfnwin_create;           /* mce */
    fn_win_destroy pfnwin_destory;         /* mce */
    fn_win_setsource pfnwin_setsrc;        /* mce */
    fn_win_setenable pfnwin_setenable;     /* mce */
    fn_win_queueframe pfnwin_queuefrm;     /* mce */
    fn_win_dequeueframe pfnwin_dequeuefrm; /* mce */
    fn_win_getplayinfo pfnwin_getplayinfo; /* mce */
    fn_win_reset pfnwin_reset;             /* mce */

    fn_win_set_vir_attr pfnwin_set_vir_attr;  /* VENC */
    fn_win_releaseframe pfnwin_release_frame; /* VENC */

    fn_win_resume pfnwin_resume;   /* pm */
    fn_win_suspend pfnwin_suspend; /* pm */
} win_export_func_s;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __VO_EXT_H__ */

