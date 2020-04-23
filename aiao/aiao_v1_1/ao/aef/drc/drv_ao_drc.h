/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv drc include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_DRC_H__
#define __DRV_AO_DRC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 ao_ioctl_snd_set_drc_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_drc_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_drc_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_drc_attr(hi_void *file, hi_void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_DRC_H__ */
