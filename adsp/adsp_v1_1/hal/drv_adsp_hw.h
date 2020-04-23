/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp hardware driver
 * Author: Audio
 * Create: 2019-10-30
 * Notes: NA
 * History: 2019-10-30 put sw and hw adsp driver together
 */

#ifndef __DRV_ADSP_HW_H__
#define __DRV_ADSP_HW_H__

#include "drv_adsp_hal.h"

#ifdef HI_SND_DSP_SUPPORT
adsp_driver *hw_adsp_get_driver(hi_void);
#else
#define hw_adsp_get_driver() ({adsp_driver *_ops = HI_NULL; _ops;})
#endif

#endif

