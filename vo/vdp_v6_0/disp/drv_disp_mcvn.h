/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp macrvision
* Create: 2016-04-12
 */
#ifndef __DRV_MCVN_H__
#define __DRV_MCVN_H__

#include "hi_drv_disp.h"
#include "drv_disp_hal.h"

typedef struct  hi_drv_mcvn_
{
	HI_DRV_DISP_MACROVISION_E mcvn;
}drv_mcvn;

hi_s32 drv_mcvn_init(HI_DRV_DISPLAY_E disp,HI_DRV_DISP_FMT_E fmt);

hi_s32 drv_mcvn_de_init(HI_DRV_DISPLAY_E disp);

hi_s32 drv_mcvn_set_macrovision(HI_DRV_DISP_MACROVISION_E mode, HI_DRV_DISP_FMT_E fmt);
hi_s32 drv_mcvn_set_date_mcvn(HI_DRV_DISP_FMT_E fmt, HI_DRV_DISP_MACROVISION_E mcvn);
hi_s32 drv_mcvn_get_date_mcvn(HI_DRV_DISP_MACROVISION_E* mcvn);

hi_s32 drv_mcvn_get_macrovision(HI_DRV_DISP_MACROVISION_E *pen_mode);

#endif
