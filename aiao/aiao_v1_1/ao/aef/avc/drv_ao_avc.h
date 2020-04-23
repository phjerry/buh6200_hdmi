/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv avc include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_AVC_H__
#define __DRV_AO_AVC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* ao cast base functions */
hi_void ao_get_def_avc_attr(hi_ao_avc_attr *avc_attr);

/* ao cast drv ioctl */
hi_s32 ao_ioctl_snd_set_avc_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_avc_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_avc_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_avc_enable(hi_void *file, hi_void *arg);

/* ao cast proc functions */
#ifdef HI_PROC_SUPPORT
hi_s32 ao_write_proc_set_avc(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end__cplusplus */

#endif  /* __HI_AO_TRACK_H__ */
