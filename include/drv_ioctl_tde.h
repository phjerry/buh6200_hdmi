/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: ioctl
 * Author: sdk
 * Create: 2018-12-10
 */

#ifndef __SOURCE_MSP_DRV_INCLUDE_TDE_IOCTL__
#define __SOURCE_MSP_DRV_INCLUDE_TDE_IOCTL__

#include <linux/ioctl.h>
#include "drv_tde_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TDE_IOC_MAGIC 't'

typedef struct {
    hi_s32 handle;                             /* TDE handle                           */
    hi_tde_surface back_ground_surface; /* background surface                   */
    hi_tde_rect back_ground_rect;              /* background surface operating rect    */
    hi_tde_surface fore_ground_surface; /* foreground surface                   */
    hi_tde_rect fore_ground_rect;              /* foreground surface operating rect    */
    hi_tde_surface dst_surface;         /* target surface                       */
    hi_tde_rect dst_rect;                      /* target surface operating rect        */
    hi_tde_opt option;                         /* operating option                     */
    hi_u32 null_indicator;                     /* flag of mask , 1: valid ,0: invalid  */
} drv_tde_bitblit_cmd;

typedef struct {
    hi_s32 handle;                             /* TDE handle         */
    hi_tde_surface fore_ground_surface; /* background surface */
    hi_tde_rect fore_ground_rect;              /* background surface operating rect */
    hi_tde_surface dst_surface;         /* foreground surface */
    hi_tde_rect dst_rect;                      /* foreground surface operating rect */
    hi_tde_fill_color fill_color;              /* fill color       */
    hi_tde_opt option;                         /* operating option */
    hi_u32 null_indicator;                     /* flag of mask , 1: valid ,0: invalid */
} drv_tde_solid_draw_cmd;

typedef struct {
    hi_s32 handle;                     /* TDE handle */
    hi_tde_surface src_surface; /* src surface */
    hi_tde_rect src_rect;              /* src surface rect */
    hi_tde_surface dst_surface; /* target surface */
    hi_tde_rect dst_rect;              /* target rect */
} drv_tde_quick_copy_cmd;

typedef struct {
    hi_s32 handle;                     /* TDE handle */
    hi_tde_surface dst_surface; /* target surface */
    hi_tde_rect dst_rect;              /* target surface rect */
    hi_u32 fill_data;                  /* fill color */
} drv_tde_quick_fill_cmd;

typedef struct {
    hi_s32 handle;                     /* TDE handle */
    hi_tde_surface src_surface; /* src surface */
    hi_tde_rect src_rect;              /* src rect */
    hi_tde_surface dst_surface; /* target surface */
    hi_tde_rect dst_rect;              /* target rect */
} drv_tde_quick_deflicker_cmd;

typedef struct {
    hi_s32 handle;                     /* TDE handle     */
    hi_tde_surface src_surface; /* src surface    */
    hi_tde_rect src_rect;              /* src rect       */
    hi_tde_surface dst_surface; /* target surface */
    hi_tde_rect dst_rect;              /* target rect    */
} drv_tde_quick_resize_cmd;

typedef struct {
    hi_s32 handle;                     /* TDE handle */
    hi_tde_mb_surface mb_src_surface;  /* src surface */
    hi_tde_rect mb_src_rect;           /* src rect */
    hi_tde_surface dst_surface; /* target surface */
    hi_tde_rect dst_rect;              /* target rect */
    hi_tde_mb_opt mb_option;           /* operating option */
} drv_tde_mb_blit_cmd;

typedef struct {
    hi_s32 handle;    /* TDE handle */
    hi_bool is_sync;  /* weather sync */
    hi_bool is_block; /* weather block */
    hi_u32 time_out;  /* time out(ms) */
} drv_tde_end_job_cmd;

typedef struct {
    hi_s32 handle;                             /* TDE handle */
    hi_tde_surface back_ground_surface; /* bk surface */
    hi_tde_rect back_ground_rect;              /* bk rect */
    hi_tde_surface fore_ground_surface; /* fore surface */
    hi_tde_rect fore_ground_rect;              /* fore rect */
    hi_tde_surface mask_surface;        /* mask surface  */
    hi_tde_rect mask_rect;                     /* mask surface rect */
    hi_tde_surface dst_surface;         /* target surface */
    hi_tde_rect dst_rect;                      /* target rect */
    hi_tde_rop_mode rop_color;                 /* RGB rop type */
    hi_tde_rop_mode rop_alpha;                 /* alpha rop type */
} drv_tde_bitmap_maskrop_cmd;

typedef struct {
    hi_s32 handle;                             /* TDE handle */
    hi_tde_surface back_ground_surface; /* bk surface */
    hi_tde_rect back_ground_rect;              /* bk rect */
    hi_tde_surface fore_ground_surface; /* fore surface */
    hi_tde_rect fore_ground_rect;              /* fore rect */
    hi_tde_surface mask_surface;        /* mask surface  */
    hi_tde_rect mask_rect;                     /* mask surface rect */
    hi_tde_surface dst_surface;         /* target surface */
    hi_tde_rect dst_rect;                      /* target rect */
    hi_u8 alpha;                               /* global alpha */
    hi_u8 reserved0;
    hi_u8 reserved1;
    hi_u8 reserved2;
    hi_tde_alpha_blending blend_mode; /* blend opt */
} drv_tde_bitmap_maskblend_cmd;

typedef struct {
    hi_s32 handle;                             /* TDE handle */
    hi_tde_surface back_ground_surface; /* bk surface */
    hi_tde_rect back_ground_rect;              /* bk rect */
    hi_tde_surface fore_ground_surface; /* fore surface */
    hi_tde_rect fore_ground_rect;              /* fore rect */
    hi_tde_surface dst_surface;         /* target surface */
    hi_tde_rect dst_rect;                      /* target rect */
    hi_tde_pattern_fill_opt option;            /* option */
    hi_u32 null_indicator;                     /* flag of mask , 1: valid ,0: invalid */
} drv_tde_pattern_fill_cmd;

#define TDE_BEGIN_JOB                _IOW(TDE_IOC_MAGIC, 1, hi_s32)
#define TDE_BIT_BLIT                 _IOW(TDE_IOC_MAGIC, 2, drv_tde_bitblit_cmd)
#define TDE_SOLID_DRAW               _IOW(TDE_IOC_MAGIC, 3, drv_tde_solid_draw_cmd)
#define TDE_QUICK_COPY               _IOW(TDE_IOC_MAGIC, 4, drv_tde_quick_copy_cmd)
#define TDE_QUICK_RESIZE             _IOW(TDE_IOC_MAGIC, 5, drv_tde_quick_resize_cmd)
#define TDE_QUICK_FILL               _IOW(TDE_IOC_MAGIC, 6, drv_tde_quick_fill_cmd)
#define TDE_QUICK_DEFLICKER          _IOW(TDE_IOC_MAGIC, 7, drv_tde_quick_deflicker_cmd)
#define TDE_MB_BITBLT                _IOW(TDE_IOC_MAGIC, 8, drv_tde_mb_blit_cmd)
#define TDE_END_JOB                  _IOW(TDE_IOC_MAGIC, 9, drv_tde_end_job_cmd)
#define TDE_WAITFORDONE              _IOW(TDE_IOC_MAGIC, 10, hi_s32)
#define TDE_CANCEL_JOB               _IOW(TDE_IOC_MAGIC, 11, hi_s32)
#define TDE_BITMAP_MASKROP           _IOW(TDE_IOC_MAGIC, 12, drv_tde_bitmap_maskrop_cmd)
#define TDE_BITMAP_MASKBLEND         _IOW(TDE_IOC_MAGIC, 13, drv_tde_bitmap_maskblend_cmd)
#define TDE_WAITALLDONE              _IO(TDE_IOC_MAGIC, 14)
#define TDE_RESET                    _IO(TDE_IOC_MAGIC, 15)
#define TDE_TRIGGER_SEL              _IOW(TDE_IOC_MAGIC, 16, hi_s32)
#define TDE_SET_DEFLICKERLEVEL       _IOW(TDE_IOC_MAGIC, 17, hi_tde_deflicker_level)
#define TDE_GET_DEFLICKERLEVEL       _IOR(TDE_IOC_MAGIC, 18, hi_tde_deflicker_level)
#define TDE_SET_ALPHATHRESHOLD_VALUE _IOW(TDE_IOC_MAGIC, 19, hi_u8)
#define TDE_GET_ALPHATHRESHOLD_VALUE _IOR(TDE_IOC_MAGIC, 20, hi_u8)
#define TDE_SET_ALPHATHRESHOLD_STATE _IOW(TDE_IOC_MAGIC, 21, hi_bool)
#define TDE_GET_ALPHATHRESHOLD_STATE _IOW(TDE_IOC_MAGIC, 22, hi_bool)
#define TDE_PATTERN_FILL             _IOW(TDE_IOC_MAGIC, 23, drv_tde_pattern_fill_cmd)
#define TDE_ENABLE_REGIONDEFLICKER   _IOW(TDE_IOC_MAGIC, 24, hi_bool)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOURCE_MSP_DRV_INCLUDE_TDE_IOCTL__ */
