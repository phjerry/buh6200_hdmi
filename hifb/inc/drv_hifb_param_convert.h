/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb param convert header
 * Create: 2019-11-25
 */

#ifndef __DRV_HIFB_PARAM_CONVERT_H__
#define __DRV_HIFB_PARAM_CONVERT_H__

#include "hifb.h"
#include "drv_hifb_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

DRV_HIFB_COLOR_FMT_E hifb_convert_fmt_unf_to_drv(hifb_color_fmt fmt);
hi_void hifb_convert_low_power_info_unf_to_drv(hifb_lowpower_info *unf_info, HI_FB_LOWPOWER_INFO_S *drv_info);
drv_hifb_xdr_mode hifb_convert_xdr_unf_to_drv(hifb_xdr_mode xdr);
drv_hifb_color_space hifb_convert_color_space_unf_to_drv(hifb_color_space color_space);
drv_hifb_compress_mode hifb_convert_compress_unf_to_drv(hifb_compress_mode compress_mode);
hi_void hifb_convert_rect_unf_to_drv(hifb_rect *unf_rect, HIFB_RECT *drv_rect);
hi_void hifb_convert_sync_info_unf_to_drv(hifb_sync_info *unf_info, hifb_hwc_layer_private_data *drv_info);
hi_void hifb_convert_layer_info_unf_to_drv(hifb_layerinfo *unf_info, HIFB_HWC_LAYERINFO_S *drv_info);

hifb_buffer_mode hifb_convert_buf_mode_drv_to_unf(HIFB_LAYER_BUF_E mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
