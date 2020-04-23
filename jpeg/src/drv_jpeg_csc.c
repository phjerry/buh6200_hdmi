/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:jpeg csc convert define
 */

#include "hi_osal.h"
#include "drv_jpeg_csc.h"
#include "hi_drv_tde.h"
#include "hi_drv_module.h"

#ifdef CONFIG_GFX_MMU_SUPPORT
static hi_tde_export_func *g_tde_export_funcs = HI_NULL;
#endif

#define JPEG_HDEC_TRANSPARENT_ALPHA 255
#define JPEG_HDEC_JOB_EXECUTE_TIMEOUT 100

static hi_s32 drv_jpeg_execute_csc_convert(jpeg_hdec_csc_surface *csc_surface);
static hi_void drv_jpeg_config_tde_src_surface(jpeg_hdec_csc_surface *csc_surface, hi_tde_surface *src_surface);
static hi_void drv_jpeg_config_tde_dst_surface(jpeg_hdec_csc_surface *csc_surface, hi_tde_surface *dst_surface);
static hi_void drv_jpeg_config_tde_src_rect(jpeg_hdec_csc_surface *csc_surface, hi_tde_rect *src_rect);
static hi_void drv_jpeg_config_tde_dst_rect(jpeg_hdec_csc_surface *csc_surface, hi_tde_rect *src_rect);

hi_s32 drv_jpeg_init_tde_dev(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_s32 ret;

    ret = osal_exportfunc_get(HI_ID_TDE, (hi_void **)&g_tde_export_funcs);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (g_tde_export_funcs == NULL) {
        return HI_FAILURE;
    }

    if (g_tde_export_funcs->drv_tde_module_open == NULL) {
        return HI_FAILURE;
    }

    ret = g_tde_export_funcs->drv_tde_module_open();
    if (ret < 0) {
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

hi_void drv_jpeg_deinit_tde_dev(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    if (g_tde_export_funcs == NULL) {
        return;
    }

    if (g_tde_export_funcs->drv_tde_module_close == NULL) {
        return;
    }

    g_tde_export_funcs->drv_tde_module_close();
#endif
}

static hi_s32 drv_jpeg_execute_csc_convert(jpeg_hdec_csc_surface *csc_surface)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_s32 ret;
    hi_s32 handle = 0;
    hi_tde_surface src_surface = {0};
    hi_tde_surface dst_surface = {0};
    hi_tde_rect src_rect = {0};
    hi_tde_rect dst_rect = {0};
    hi_tde_opt opt = {0};

    if (g_tde_export_funcs == NULL) {
        return HI_FAILURE;
    }

    drv_jpeg_config_tde_src_surface(csc_surface, &src_surface);
    drv_jpeg_config_tde_dst_surface(csc_surface, &dst_surface);
    drv_jpeg_config_tde_src_rect(csc_surface, &src_rect);
    drv_jpeg_config_tde_dst_rect(csc_surface, &dst_rect);

    ret = g_tde_export_funcs->drv_tde_module_begin_job(&handle);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = g_tde_export_funcs->drv_tde_module_blit(handle, NULL, NULL, &src_surface, &src_rect, &dst_surface, &dst_rect,
                                                  &opt);
    if (ret != HI_SUCCESS) {
        g_tde_export_funcs->drv_tde_module_cancel_job(handle);
        return ret;
    }

    ret = g_tde_export_funcs->drv_tde_module_end_job(handle, HI_TRUE, JPEG_HDEC_JOB_EXECUTE_TIMEOUT, HI_TRUE, HI_NULL,
                                                     HI_NULL);
    if (ret != HI_SUCCESS) {
        g_tde_export_funcs->drv_tde_module_cancel_job(handle);
        return ret;
    }
#endif
    csc_surface->is_csc_finished = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 drv_jpeg_hdec_csc_convert(jpeg_hdec_csc_surface *csc_surface)
{
    if (csc_surface->is_csc_finished == HI_TRUE) {
        return HI_SUCCESS;
    }
    return drv_jpeg_execute_csc_convert(csc_surface);
}

static hi_tde_color_fmt drv_jpeg_get_tde_color_fmt(jpeg_hdec_image_fmt fmt)
{
    switch (fmt) {
        case JPEG_FMT_YUV400:
            return HI_TDE_COLOR_FMT_JPG_YCbCr400MBP;
        case JPEG_FMT_YUV420:
            return HI_TDE_COLOR_FMT_JPG_YCbCr420MBP;
        case JPEG_FMT_YUV422_21:
            return HI_TDE_COLOR_FMT_JPG_YCbCr422MBHP;
        case JPEG_FMT_YUV422_12:
            return HI_TDE_COLOR_FMT_JPG_YCbCr422MBVP;
        default:
            return HI_TDE_COLOR_FMT_JPG_YCbCr444MBP;
    }
}

static hi_tde_color_fmt drv_jpeg_hdec_get_xrgb8888(jpeg_hdec_image_fmt fmt)
{
    switch (fmt) {
        case JPEG_FMT_RGBA_8888:
            return HI_TDE_COLOR_FMT_RGBA8888;
        case JPEG_FMT_BGRA_8888:
            return HI_TDE_COLOR_FMT_BGRA8888;
        case JPEG_FMT_ABGR_8888:
            return HI_TDE_COLOR_FMT_ABGR8888;
        case JPEG_FMT_ARGB_8888:
            return HI_TDE_COLOR_FMT_ARGB8888;
        default:
            return HI_TDE_COLOR_FMT_MAX;
    }
}

static hi_tde_color_fmt drv_jpeg_hdec_get_xrgb1555(jpeg_hdec_image_fmt fmt)
{
    switch (fmt) {
        case JPEG_FMT_BGRA_5551:
            return HI_TDE_COLOR_FMT_BGRA1555;
        case JPEG_FMT_RGBA_5551:
            return HI_TDE_COLOR_FMT_RGBA1555;
        case JPEG_FMT_ARGB_1555:
            return HI_TDE_COLOR_FMT_ARGB1555;
        case JPEG_FMT_ABGR_1555:
            return HI_TDE_COLOR_FMT_ABGR1555;
        default:
            return HI_TDE_COLOR_FMT_MAX;
    }
}

static hi_tde_color_fmt drv_jpeg_hdec_get_rgb888(jpeg_hdec_image_fmt fmt)
{
    switch (fmt) {
        case JPEG_FMT_BGR_888:
            return HI_TDE_COLOR_FMT_BGR888;
        case JPEG_FMT_RGB_888:
            return HI_TDE_COLOR_FMT_RGB888;
        default:
            return HI_TDE_COLOR_FMT_MAX;
    }
}

static inline hi_tde_color_fmt drv_jpeg_hdec_getrgb565(jpeg_hdec_image_fmt fmt)
{
    switch (fmt) {
        case JPEG_FMT_RGB_565:
            return HI_TDE_COLOR_FMT_RGB565;
        case JPEG_FMT_BGR_565:
            return HI_TDE_COLOR_FMT_BGR565;
        default:
            return HI_TDE_COLOR_FMT_MAX;
    }
}

static hi_tde_color_fmt drv_jpeg_hdec_convert_out_fmt_to_tde_fmt(jpeg_hdec_image_fmt fmt)
{
    hi_tde_color_fmt color_fmt;

    color_fmt = drv_jpeg_hdec_get_xrgb8888(fmt);
    if (color_fmt != HI_TDE_COLOR_FMT_MAX) {
        return color_fmt;
    }

    color_fmt = drv_jpeg_hdec_get_xrgb1555(fmt);
    if (color_fmt != HI_TDE_COLOR_FMT_MAX) {
        return color_fmt;
    }

    color_fmt = drv_jpeg_hdec_get_rgb888(fmt);
    if (color_fmt != HI_TDE_COLOR_FMT_MAX) {
        return color_fmt;
    }

    color_fmt = drv_jpeg_hdec_getrgb565(fmt);
    return color_fmt;
}

static hi_void drv_jpeg_config_tde_src_surface(jpeg_hdec_csc_surface *csc_surface, hi_tde_surface *src_surface)
{
    src_surface->phy_addr = csc_surface->dec_in.phy_addr;
    src_surface->color_fmt = drv_jpeg_get_tde_color_fmt(csc_surface->dec_in.jpeg_color_space);
    src_surface->width = csc_surface->dec_in.y_width;
    src_surface->height = csc_surface->dec_in.y_height;
    src_surface->stride = csc_surface->dec_in.y_stride;
    src_surface->clut_phy_addr = csc_surface->dec_in.clut_phy_addr;
    src_surface->is_ycbcr_clut = HI_FALSE;
    src_surface->alpha_max_is_255 = HI_TRUE;
    src_surface->support_alpha_ext_1555 = HI_FALSE;
    src_surface->alpha0 = 0;
    src_surface->alpha1 = 0;
    src_surface->cbcr_phy_addr = csc_surface->dec_in.cbcr_phy_addr;
    src_surface->cbcr_stride = csc_surface->dec_in.cbcr_stride;
    return;
}

static hi_void drv_jpeg_config_tde_dst_surface(jpeg_hdec_csc_surface *csc_surface, hi_tde_surface *dst_surface)
{
    dst_surface->phy_addr = csc_surface->dec_out.phy_addr;
    dst_surface->color_fmt = drv_jpeg_hdec_convert_out_fmt_to_tde_fmt(csc_surface->dec_out.output_color_space);
    dst_surface->width = csc_surface->dec_out.width;
    dst_surface->height = csc_surface->dec_out.height;
    dst_surface->stride = csc_surface->dec_out.out_stride;
    dst_surface->clut_phy_addr = csc_surface->dec_out.clut_phy_addr;
    dst_surface->is_ycbcr_clut = HI_FALSE;
    dst_surface->alpha_max_is_255 = HI_TRUE;
    dst_surface->support_alpha_ext_1555 = HI_FALSE;
    dst_surface->alpha0 = 0;
    dst_surface->alpha1 = 0;
    dst_surface->cbcr_phy_addr = 0;
    dst_surface->cbcr_stride = 0;
    return;
}

static hi_void drv_jpeg_config_tde_src_rect(jpeg_hdec_csc_surface *csc_surface, hi_tde_rect *src_rect)
{
    src_rect->pos_x = csc_surface->dec_out.crop_rect.x;
    src_rect->pos_y = csc_surface->dec_out.crop_rect.y;
    src_rect->width = csc_surface->dec_out.crop_rect.w;
    src_rect->height = csc_surface->dec_out.crop_rect.h;
    return;
}

static hi_void drv_jpeg_config_tde_dst_rect(jpeg_hdec_csc_surface *csc_surface, hi_tde_rect *dst_rect)
{
    dst_rect->pos_x = 0;
    dst_rect->pos_y = 0;
    dst_rect->width = csc_surface->dec_out.crop_rect.w;
    dst_rect->height = csc_surface->dec_out.crop_rect.h;
    return;
}
