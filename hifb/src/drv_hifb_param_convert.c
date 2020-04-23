/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb param convert
 * Create: 2019-11-25
 */

#include "drv_hifb_param_convert.h"
#include "drv_hifb_mem.h"

typedef struct {
    hifb_color_fmt unf_fmt;
    DRV_HIFB_COLOR_FMT_E drv_fmt;
} hifb_unf_drv_fmt_convert;

const static hifb_unf_drv_fmt_convert g_hifb_unf_drv_fmt_convert[] = {
    { HIFB_COLOR_FMT_RGB565, DRV_HIFB_FMT_RGB565 },
    { HIFB_COLOR_FMT_RGB888, DRV_HIFB_FMT_RGB888 },
    { HIFB_COLOR_FMT_ARGB4444, DRV_HIFB_FMT_ARGB4444 },
    { HIFB_COLOR_FMT_ARGB1555, DRV_HIFB_FMT_ARGB1555 },
    { HIFB_COLOR_FMT_ARGB8888, DRV_HIFB_FMT_ARGB8888 },

    { HIFB_COLOR_FMT_RGBA4444, DRV_HIFB_FMT_RGBA4444 },
    { HIFB_COLOR_FMT_RGBA5551, DRV_HIFB_FMT_RGBA5551 },
    { HIFB_COLOR_FMT_RGBA8888, DRV_HIFB_FMT_RGBA8888 },

    { HIFB_COLOR_FMT_ABGR4444, DRV_HIFB_FMT_ABGR4444 },
    { HIFB_COLOR_FMT_ABGR1555, DRV_HIFB_FMT_ABGR1555 },
    { HIFB_COLOR_FMT_ABGR8888, DRV_HIFB_FMT_ABGR8888 },

    { HIFB_COLOR_FMT_BGR565, DRV_HIFB_FMT_BGR565 },
    { HIFB_COLOR_FMT_BGR888, DRV_HIFB_FMT_BGR888 },
    { HIFB_COLOR_FMT_BGRA4444, DRV_HIFB_FMT_BUTT },
    { HIFB_COLOR_FMT_BGRA5551, DRV_HIFB_FMT_BUTT },
    { HIFB_COLOR_FMT_BGRA8888, DRV_HIFB_FMT_BUTT },

    { HIFB_COLOR_FMT_CLUT1, DRV_HIFB_FMT_1BPP },
    { HIFB_COLOR_FMT_CLUT2, DRV_HIFB_FMT_2BPP },
    { HIFB_COLOR_FMT_CLUT4, DRV_HIFB_FMT_4BPP },
    { HIFB_COLOR_FMT_CLUT8, DRV_HIFB_FMT_8BPP },
    { HIFB_COLOR_FMT_ACLUT44, DRV_HIFB_FMT_ACLUT44 },
    { HIFB_COLOR_FMT_ACLUT88, DRV_HIFB_FMT_ACLUT88 },

    { HIFB_COLOR_FMT_FP16, DRV_HIFB_FMT_FP16 },
    { HIFB_COLOR_FMT_ARGB2101010, DRV_HIFB_FMT_ARGB2101010 },
    { HIFB_COLOR_FMT_ARGB10101010, DRV_HIFB_FMT_ARGB10101010 },
};

DRV_HIFB_COLOR_FMT_E hifb_convert_fmt_unf_to_drv(hifb_color_fmt fmt)
{
    hi_u32 i;

    for (i = 0; i < sizeof(g_hifb_unf_drv_fmt_convert) / sizeof(g_hifb_unf_drv_fmt_convert[0]); i++) {
        if (fmt == g_hifb_unf_drv_fmt_convert[i].unf_fmt) {
            return g_hifb_unf_drv_fmt_convert[i].drv_fmt;
        }
    }
    return DRV_HIFB_FMT_BUTT;
}

hi_void hifb_convert_low_power_info_unf_to_drv(hifb_lowpower_info *unf_info, HI_FB_LOWPOWER_INFO_S *drv_info)
{
    if (unf_info != HI_NULL && drv_info != HI_NULL && (sizeof(unf_info->lowpower_value) == sizeof(drv_info->LpInfo))) {
        hi_u32 i;
        drv_info->LowPowerEn = unf_info->enable;
        for (i = 0; i < CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT; i++) {
            drv_info->LpInfo[i] = unf_info->lowpower_value[i];
        }
    }
}

drv_hifb_xdr_mode hifb_convert_xdr_unf_to_drv(hifb_xdr_mode xdr)
{
    switch (xdr) {
        case HIFB_HDR_SDR:
            return DRV_HIFB_HDR_MODE_SDR;
        case HIFB_HDR_HDR10:
            return DRV_HIFB_HDR_MODE_HDR10;
        case HIFB_HDR_HLG:
            return DRV_HIFB_HDR_MODE_HLG;
        default:
            return DRV_HIFB_HDR_MODE_MAX;
    }
}

drv_hifb_color_space hifb_convert_color_space_unf_to_drv(hifb_color_space color_space)
{
    switch (color_space) {
        case HIFB_COLOR_SPACE_BT709:
            return DRV_HIFB_COLOR_SPACE_BT709;
        case HIFB_COLOR_SPACE_BT2020:
            return DRV_HIFB_COLOR_SPACE_BT2020;
        default:
            return DRV_HIFB_COLOR_SPACE_MAX;
    }
}

drv_hifb_compress_mode hifb_convert_compress_unf_to_drv(hifb_compress_mode compress_mode)
{
    switch (compress_mode) {
        case HIFB_CMP_NONE:
            return DRV_HIFB_CMP_MODE_NONE;
        case HIFB_CMP_HFBC:
            return DRV_HIFB_CMP_MODE_HFBC;
        case HIFB_CMP_AFBC:
            return DRV_HIFB_CMP_MODE_AFBC;
        default:
            return DRV_HIFB_CMP_MODE_MAX;
    }
}

hi_void hifb_convert_rect_unf_to_drv(hifb_rect *unf_rect, HIFB_RECT *drv_rect)
{
    if (unf_rect != HI_NULL && drv_rect != HI_NULL) {
        drv_rect->x = unf_rect->x;
        drv_rect->y = unf_rect->y;
        drv_rect->w = unf_rect->w;
        drv_rect->h = unf_rect->h;
    }
}

hi_void hifb_convert_sync_info_unf_to_drv(hifb_sync_info *unf_info, hifb_hwc_layer_private_data *drv_info)
{
    if (unf_info != HI_NULL && drv_info != HI_NULL) {
        drv_info->buffer_addr = drv_hifb_mem_get_smmu_from_fd(unf_info->mem_handle);
        hifb_convert_rect_unf_to_drv(&unf_info->video_rect, &drv_info->video_rect);
        hifb_convert_rect_unf_to_drv(&unf_info->output_rect, &drv_info->output_rect);
        drv_info->sync_enable = unf_info->enable;
        drv_info->handle = unf_info->win_handle;
    }
}

hi_void hifb_convert_layer_info_unf_to_drv(hifb_layerinfo *unf_info, HIFB_HWC_LAYERINFO_S *drv_info)
{
    if (unf_info != HI_NULL && drv_info != HI_NULL) {
        drv_info->bPreMul = unf_info->is_premulti;
        drv_info->bStereo = unf_info->is_stereo;

        drv_info->eFmt = hifb_convert_fmt_unf_to_drv(unf_info->surface.fmt);
        drv_info->mem_handle = unf_info->surface.mem_handle;
        drv_info->stInRect.x = 0;
        drv_info->stInRect.y = 0;
        drv_info->stInRect.w = unf_info->surface.width;
        drv_info->stInRect.h = unf_info->surface.height;
        drv_info->u32Stride = unf_info->surface.stride;

        drv_info->u32Alpha = unf_info->alpha_sum;
        drv_info->s32AcquireFenceFd = unf_info->acquire_fd;

        hifb_convert_low_power_info_unf_to_drv(&unf_info->lowpower_info, &drv_info->stLowPowerInfo);
        drv_info->xdr_mode = hifb_convert_xdr_unf_to_drv(unf_info->xdr_mode);
        drv_info->color_space = hifb_convert_color_space_unf_to_drv(unf_info->color_space);
        drv_info->compress_mode = hifb_convert_compress_unf_to_drv(unf_info->compress_mode);

        hifb_convert_sync_info_unf_to_drv(&unf_info->sync_info, &drv_info->private_data);
    }
}

hifb_buffer_mode hifb_convert_buf_mode_drv_to_unf(HIFB_LAYER_BUF_E mode)
{
    switch (mode) {
        case HIFB_LAYER_BUF_DOUBLE:
            return HIFB_BUFFER_DOUBLE;
        case HIFB_LAYER_BUF_ONE:
            return HIFB_BUFFER_ONE;
        case HIFB_LAYER_BUF_NONE:
            return HIFB_BUFFER_NONE;
        default:
            return HIFB_BUFFER_MAX;
    }
}
