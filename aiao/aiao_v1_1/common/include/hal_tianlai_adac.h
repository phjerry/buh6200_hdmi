/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: tianlai adac header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#ifndef __HAL_TIANLAI_ADAC_H__
#define __HAL_TIANLAI_ADAC_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum adac_version {
    ADAC_TIANLAI_NULL,
    ADAC_TIANLAI_V500,
    ADAC_TIANLAI_V600,
    ADAC_TIANLAI_V700,
    ADAC_TIANLAI_V730,
    ADAC_TIANLAI_SV100,
};

typedef struct {
    hi_u32 version;
    struct osal_list_head node;
    hi_void (*fast_power_enable)(hi_bool enable);
    hi_void (*init)(hi_u32 sample_rate, hi_bool resume);
    hi_void (*deinit)(hi_bool suspend);
} adac_platform_driver;

#ifdef HI_SND_MUTECTL_SUPPORT
hi_void adac_tianlai_fast_power_enable(hi_bool enable);
#endif

hi_void adac_tianlai_init(hi_u32 sampel_rate, hi_bool resume);
hi_void adac_tianlai_deinit(hi_bool suspend);

hi_void adac_tianlai_flush_work(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HAL_TIANLAI_ADAC_H__ */
