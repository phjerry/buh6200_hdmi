/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: delivered by other modules header
 * Author: sdk
 * Create: 2018-12-10
 */

#ifndef __DRV_HIFB_EXT_H__
#define __DRV_HIFB_EXT_H__

/* ********************************add include here*********************************************** */
#include "hi_type.h"
/*************************************************************************************************/

/*************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* ************************** Structure Definition *********************************************** */
typedef struct {
    hi_u32 timeline_value;
    hi_u32 fence_value;
    hi_u32 fence_refresh_cnt;
    hi_u32 reg_update_cnt;
    hi_u32 decompress_state;
    hi_u32 will_work_addr;
    hi_u32 work_addr;
    hi_u32 layer_stride;
    hi_u32 buffer_size;
} drv_hifb_settings_info;

/* **************************** Macro Definition ************************************************* */
typedef hi_s32 (*hifb_mod_init)(hi_void);
typedef hi_void (*hifb_mod_exit)(hi_void);
typedef hi_void (*hifb_set_logo_layer_enable)(hi_bool);
typedef hi_void (*hifb_get_settings_info)(hi_u32 layer_id, drv_hifb_settings_info *settings_info);

#define hifb_init_module_k DRV_HIFB_ModInit_K
#define hifb_cleanup_module_k DRV_HIFB_ModExit_K

/* ************************** Structure Definition *********************************************** */
typedef struct {
    hifb_mod_init drv_hifb_mod_init;
    hifb_mod_exit drv_hifb_mod_exit;
    hifb_get_settings_info drv_hifb_get_settings_info;
    hifb_set_logo_layer_enable drv_hifb_set_logo_layer_enable;
} hfib_export_func;

/* **************************  The enum of Jpeg image format  ************************************ */

/* ********************* Global Variable declaration ********************************************* */

/* ****************************** API declaration ************************************************ */
hi_s32 HIFB_DRV_ModInit(hi_void);
hi_void HIFB_DRV_ModExit(hi_void);

hi_s32 DRV_HIFB_ModInit_K(hi_void);
hi_void DRV_HIFB_ModExit_K(hi_void);

hi_void HIFB_DRV_GetSettingInfo(hi_u32 layer_id, drv_hifb_settings_info *settings_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
