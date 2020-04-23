/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv common header config
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_H__
#define __DRV_AO_H__

#include "hi_drv_audio.h"

hi_s32 ao_get_open_attr_from_bootargs(ao_snd_id sound, hi_ao_attr *attr);
hi_s32 ao_get_open_attr_from_pdm(ao_snd_id sound, hi_ao_attr *attr);

hi_s32 ao_ioctl_get_snd_def_open_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_open(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_close(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_volume(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_volume(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_output_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_output_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_preci_volume(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_preci_volume(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_alsa_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_balance(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_balance(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_x_run_count(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_delay_compensation(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_delay_compensation(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_low_latency(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_low_latency(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_ext_delay_ms(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_dma_create(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_dma_destory(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_dma_mode_set_sample_rate(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_ad_output_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_ad_output_enable(hi_void *file, hi_void *arg);

hi_s32 ao_ioctl_snd_set_track_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_track_mode(hi_void *file, hi_void *arg);

hi_void ao_osal_init(hi_void);
hi_void ao_osal_deinit(hi_void);

#endif

