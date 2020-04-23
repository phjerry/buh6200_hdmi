/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: osictl manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_OSICTL__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_OSICTL__

#include "drv_tde_struct.h"
#include "tde_adp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_tde_open(hi_void);
hi_s32 drv_tde_close(hi_void);
hi_s32 drv_tde_init(hi_void);
hi_s32 tde_begin_job(hi_s32 *handle, hi_void *private_data);
hi_s32 drv_tde_begin_job(hi_s32 *handle);
hi_s32 drv_tde_end_job(hi_s32 handle, hi_bool is_block, hi_u32 time_out, hi_bool is_sync,
                       drv_tde_func_callback func_complete_callback, hi_void *func_para);
hi_s32 drv_tde_cancel_job(hi_s32 handle);
hi_s32 drv_tde_wait_for_done(hi_s32 handle, hi_u32 time_out);
hi_s32 drv_tde_wait_all_done(hi_bool is_sync);
hi_s32 drv_tde_quick_copy(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                          hi_tde_surface *dst_surface, hi_tde_rect *dst_rect);
hi_s32 drv_tde_quick_fill(hi_s32 handle, hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_u32 fill_data);
hi_s32 tde_hal_quick_resize(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                            hi_tde_surface *dst_surface, hi_tde_rect *dst_rect);
hi_s32 tde_hal_quick_flicker(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                             hi_tde_surface *dst_surface, hi_tde_rect *dst_rect);
hi_s32 drv_tde_blit(hi_s32 handle, hi_tde_surface *back_ground_surface, hi_tde_rect *back_ground_rect,
                    hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect, hi_tde_surface *dst_surface,
                    hi_tde_rect *dst_rect, hi_tde_opt *opt);

hi_s32 tde_mb_blit(hi_s32 handle, hi_tde_mb_surface *mb_surface, hi_tde_rect *mb_rect, hi_tde_surface *dst_surface,
                   hi_tde_rect *dst_rect, hi_tde_mb_opt *mb_opt);

hi_s32 tde_hal_bitmap_mask_rop(hi_s32 handle, hi_tde_surface *back_ground_surface, hi_tde_rect *back_ground_rect,
                               hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                               hi_tde_surface *mask_surface, hi_tde_rect *mask_rect, hi_tde_surface *dst_surface,
                               hi_tde_rect *dst_rect, hi_tde_rop_mode rop_color, hi_tde_rop_mode rop_alpha);

hi_s32 tde_hal_bitmap_mask_blend(hi_s32 handle, hi_tde_surface *back_ground_surface, hi_tde_rect *back_ground_rect,
                                 hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                 hi_tde_surface *mask_surface, hi_tde_rect *mask_rect, hi_tde_surface *dst_surface,
                                 hi_tde_rect *dst_rect, hi_u8 alpha, hi_tde_alpha_blending blend_mode);

hi_s32 tde_hal_solid_draw(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                          hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_fill_color *fill_color,
                          hi_tde_opt *opt);

hi_s32 drv_tde_set_deflicker_level(hi_tde_deflicker_level deflicker_level);

hi_s32 TdeOsiGetDeflickerLevel(hi_tde_deflicker_level *deflicker_level);

hi_s32 tde_set_alpha_threshold_value(hi_u8 threshold_value);

hi_s32 tde_get_alpha_threshold_value(hi_u8 *threshold_value);

hi_s32 tde_set_alpha_threshold_state(hi_bool alpha_threshold_en);

hi_s32 tde_get_alpha_threshold_state(hi_bool *alpha_threshold_en);

hi_s32 tde_hal_pattern_fill(hi_s32 handle, hi_tde_surface *back_ground_surface, hi_tde_rect *back_ground_rect,
                            hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                            hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt);

hi_s32 drv_tde_enable_region_deflicker(hi_bool region_deflicker);

hi_s32 drv_tde_calc_scale_rect(const hi_tde_rect *src_rect, const hi_tde_rect *dst_rect, hi_tde_rect *rect_in_src,
                               hi_tde_rect *rect_in_dst);

hi_s32 drv_tde_quick_copyex(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                            hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_bool mmz_for_src,
                            hi_bool mmz_for_dst);
#ifdef CONFIG_TDE_BLIT_EX
hi_s32 drv_tde_single_blitex(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                             hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt, hi_bool mmz_for_src,
                             hi_bool mmz_for_dst);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INCLUDE_OSICTL__ */
