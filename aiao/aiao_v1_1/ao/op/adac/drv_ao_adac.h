/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv adac include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_ADAC_H__
#define __DRV_AO_ADAC_H__

#include "drv_ao_op.h"

hi_s32 ao_snd_set_adac_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool enable);
hi_s32 ao_snd_get_adac_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool *enable);

hi_s32 ao_ioctl_snd_set_adac_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_adac_enable(hi_void *file, hi_void *arg);

#endif  /* __DRV_AO_ADAC_H__ */

