/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: disp timing
* Create: 2019-04-12
 */

#ifndef __DRV_DISP_TIMING_H__
#define __DRV_DISP_TIMING_H__

#include "hi_drv_disp.h"
#include "hal_disp_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 drv_disp_timing_get_timing_info(hi_drv_disp_fmt format,
                                       hi_drv_disp_static_timing *disp_timing,
                                       disp_timing_info *timing_info);

hi_s32 drv_disp_timing_config(hi_drv_display disp, disp_timing_info *disp_timing_info);
hi_s32 drv_disp_timing_get_config(hi_drv_display disp, hi_drv_disp_timing_cfg *disp_timing);

hi_s32 drv_disp_timing_set_vrr(hi_drv_display disp,  hi_drv_disp_vrr vrr_cfg);
hi_s32 drv_disp_timing_get_vrr(hi_drv_display disp,  hi_drv_disp_vrr *vrr_cfg);
hi_s32 drv_disp_timing_set_qms(hi_drv_display disp,  hi_drv_disp_qms qms_cfg);
hi_s32 drv_disp_timing_get_qms(hi_drv_display disp,  hi_drv_disp_qms *qms_cfg);
hi_s32 drv_disp_timing_set_allm(hi_drv_display disp,  hi_bool enable);
hi_s32 drv_disp_timing_get_allm(hi_drv_display disp,  hi_bool *enable);
hi_s32 drv_disp_timing_set_qft(hi_drv_display disp,  hi_drv_disp_qft qft_cfg);
hi_s32 drv_disp_timing_get_qft(hi_drv_display disp,  hi_drv_disp_qft *qft_cfg);
hi_s32 drv_disp_timing_get_fmt_from_vic(hi_u32 vic, hi_drv_disp_fmt *format);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*  __DRV_DISP_TIMING_H__  */


