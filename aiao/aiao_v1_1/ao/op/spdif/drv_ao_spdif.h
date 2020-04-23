/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv spdif include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_SPDIF_H__
#define __DRV_AO_SPDIF_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 ao_ioctl_snd_set_spdif_scms_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_spdif_scms_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_spdif_category_code(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_spdif_category_code(hi_void *file, hi_void *arg);

#ifdef HI_PROC_SUPPORT
hi_s32 ao_write_proc_set_spdif(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_SPDIF_H__ */

