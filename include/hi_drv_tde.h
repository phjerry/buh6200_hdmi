/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: 和其他模块交付的接口头文件
 * Author: sdk
 * Create: 2018-12-10
 */

#ifndef __SOURCE_MSP_DRV_INCLUDE_TDE_EXT__
#define __SOURCE_MSP_DRV_INCLUDE_TDE_EXT__

#include "hi_type.h"
#include "drv_tde_struct.h"
#include "hi_drv_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hi_s32 (*tde_module_init)(hi_void);
typedef hi_void (*tde_module_exit)(hi_void);
typedef hi_s32 (*tde_module_open)(hi_void);
typedef hi_s32 (*tde_module_close)(hi_void);
typedef hi_s32 (*tde_module_begin_job)(hi_s32 *);
typedef hi_s32 (*tde_module_end_job)(hi_s32, hi_bool, hi_u32, hi_bool, drv_tde_func_callback, hi_void *);
typedef hi_s32 (*tde_module_cancel_job)(hi_s32);
typedef hi_s32 (*tde_module_wait_for_done)(hi_s32, hi_u32);
typedef hi_s32 (*tde_module_wait_all_done)(hi_bool);
typedef hi_s32 (*tde_module_quick_copy)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_tde_surface *,
               hi_tde_rect *);
typedef hi_s32 (*tde_module_quick_copyex)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_tde_surface *,
               hi_tde_rect *, hi_bool, hi_bool);
typedef hi_s32 (*tde_module_single_blitex)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_tde_surface *,
               hi_tde_rect *, hi_tde_opt *, hi_bool, hi_bool);
typedef hi_s32 (*tde_module_quick_fill)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_u32);
typedef hi_s32 (*tde_module_quick_resize)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_tde_surface *,
               hi_tde_rect *);
typedef hi_s32 (*tde_module_quick_flicker)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_tde_surface *,
               hi_tde_rect *);
typedef hi_s32 (*tde_module_blit)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_tde_surface *, hi_tde_rect *,
                                  hi_tde_surface *,
                                  hi_tde_rect *, hi_tde_opt *);
typedef hi_s32 (*tde_module_mbblit)(hi_s32, hi_tde_mb_surface *, hi_tde_rect *, hi_tde_surface *,
               hi_tde_rect *, hi_tde_mb_opt *);
typedef hi_s32 (*tde_module_solid_draw)(hi_s32, hi_tde_surface *, hi_tde_rect *, hi_tde_surface *,
               hi_tde_rect *, hi_tde_fill_color *, hi_tde_opt *);
typedef hi_s32 (*tde_module_set_deflicker_level)(hi_tde_deflicker_level);
typedef hi_s32 (*tde_module_enable_region_deflicker)(hi_bool);
typedef hi_s32 (*tde_module_set_alpha_threshold_value)(hi_u8 u8ThresholdValue);
typedef hi_s32 (*tde_module_set_alpha_threshold_state)(hi_bool bEnAlphaThreshold);
typedef hi_s32 (*tde_module_get_alpha_threshold_state)(hi_bool *pbEnAlphaThreshold);
typedef hi_s32 (*tde_module_calc_scale_rect)(const hi_tde_rect *, const hi_tde_rect *, hi_tde_rect *,
               hi_tde_rect *);
//typedef hi_s32 (*tde_module_suspend)(PM_BASEDEV_S *, pm_message_t);
//typedef hi_s32 (*tde_module_resume)(PM_BASEDEV_S *);
typedef hi_s32 (*tde_module_lock_working_flag)(hi_size_t *);
typedef hi_s32 (*tde_module_unlock_working_flag)(hi_size_t *);
typedef hi_s32 (*tde_module_get_working_flag)(hi_bool *);
typedef hi_s32 (*tde_module_set_working_flag)(hi_bool);

typedef struct {
    tde_module_init drv_tde_module_init;
    tde_module_exit drv_tde_module_exit;
    tde_module_open drv_tde_module_open;
    tde_module_close drv_tde_module_close;
    tde_module_begin_job drv_tde_module_begin_job;
    tde_module_end_job drv_tde_module_end_job;
    tde_module_cancel_job drv_tde_module_cancel_job;
    tde_module_wait_for_done drv_tde_module_wait_for_done;
    tde_module_wait_all_done drv_tde_module_wait_all_done;
    tde_module_quick_copy drv_tde_module_quick_copy;
    tde_module_quick_fill drv_tde_module_quick_fill;
    tde_module_blit drv_tde_module_blit;
    tde_module_set_deflicker_level drv_tde_module_set_deflicker_level;
    tde_module_enable_region_deflicker drv_tde_module_enable_region_deflicker;
    tde_module_calc_scale_rect drv_tde_module_calc_scale_rect;
    //tde_module_suspend drv_tde_module_suspend;
    //tde_module_resume drv_tde_module_resume;
    tde_module_quick_copyex drv_tde_module_quick_copyex;
    tde_module_single_blitex drv_tde_module_single_blitex;
    tde_module_lock_working_flag drv_tde_module_lock_working_flag;
    tde_module_unlock_working_flag drv_tde_module_unlock_working_flag;
    tde_module_get_working_flag drv_tde_module_get_working_flag;
    tde_module_set_working_flag drv_tde_module_set_working_flag;
} hi_tde_export_func;

hi_s32 hi_drv_tde_module_init(hi_void);
hi_s32 drv_tde_module_init_k(hi_void);
hi_void hi_drv_tde_module_exit(hi_void);
hi_void drv_tde_module_exit_k(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_INCLUDE_TDE_EXT__ */
