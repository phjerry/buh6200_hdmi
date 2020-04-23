/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv adsp hal header file
 * Author: Audio
 * Create: 2019-10-30
 * Notes: NA
 * History: 2019-10-30 put sw and hw adsp driver together
 */

#ifndef __DRV_ADSP_HAL_H__
#define __DRV_ADSP_HAL_H__

#include "hi_audsp_aoe.h"

typedef struct {
    hi_s32  (*load_fw)(hi_void);
    hi_void (*unload_fw)(hi_void);

    hi_s32 (*load_aoe)(hi_void);
    hi_s32 (*unload_aoe)(hi_void);
    hi_s32 (*get_aoe_info)(aoe_fw_info *info);
} adsp_driver;

hi_s32 adsp_load_aoe_firmware(hi_void);
hi_s32 adsp_unload_aoe_firmware(hi_void);
hi_s32 adsp_get_aoe_firmware_info(aoe_fw_info *info);

#endif

