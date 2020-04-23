/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv ar include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AR_H__
#define __DRV_AR_H__

#include "hi_ao_type.h"

hi_s32 ao_snd_set_con_output_enable(ao_snd_id sound, hi_bool enable);
hi_s32 ao_snd_set_output_atmos_enable(ao_snd_id sound, hi_bool enable);
hi_void ao_snd_update_render_raw_info(ao_snd_id sound, hi_ao_ouput_mode mode);

/* aiao drv ar ioctl */
hi_s32 ao_ioctl_snd_create_sb(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_destroy_sb(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_render_param(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_render_param(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_output_atmos_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_output_atmos_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_set_con_output_status(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_get_con_output_status(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_set_output_latency_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_get_output_latency_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_continue_output(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_continue_output(hi_void *file, hi_void *arg);

#endif  /* __DRV_AR_H__ */

