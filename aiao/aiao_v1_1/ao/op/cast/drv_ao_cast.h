/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv cast driver header
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_CAST_H__
#define __DRV_AO_CAST_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    CAST_BIT_SHIFT_8 = 8,
    CAST_BIT_SHIFT_16 = 16,
} cast_bit_shift;

hi_s32 ao_cast_alloc_handle(hi_handle *ph_handle, hi_void *file, hi_ao_cast_attr *user_cast_attr);
hi_void ao_cast_free_handle(hi_handle cast);
hi_s32 ao_cast_get_def_attr(hi_ao_cast_attr *def_attr);
hi_s32 ao_cast_create(ao_snd_id sound, hi_ao_cast_attr *cast_attr, hi_audio_buffer *mmz, hi_handle h_cast);
hi_s32 ao_cast_destory(hi_handle h_cast);
hi_s32 ao_cast_set_info(hi_handle h_cast, hi_u64 user_virt_addr);
hi_s32 ao_cast_get_info(hi_handle h_cast, ao_cast_info_param *info);
hi_s32 ao_cast_set_enable(hi_handle h_cast, hi_bool enable);
hi_s32 ao_cast_get_enable(hi_handle h_cast, hi_bool *enable);
hi_s32 ao_snd_free_cast(hi_void *filp);
hi_s32 ao_cast_get_settings(hi_handle h_cast, snd_cast_settings *cast_settings);
hi_s32 ao_cast_restore_settings(hi_handle h_cast, snd_cast_settings *cast_settings);

/* aiao drv cast ioctl */
hi_s32 ao_ioctl_cast_get_def_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_create(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_destory(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_set_info(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_get_info(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_set_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_get_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_acquire_frame(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_release_frame(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_set_abs_gain(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_get_abs_gain(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_set_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_cast_get_mute(hi_void *file, hi_void *arg);

hi_s32 ao_snd_set_all_cast_mute(ao_snd_id sound, hi_bool mute);
hi_s32 ao_snd_get_all_cast_mute(ao_snd_id sound, hi_bool *mute);
hi_s32 ao_cast_set_aef_bypass(hi_u32 cast_id, hi_bool bypass);

/* aiao drv cast ioctl for DPT */
hi_s32 ao_ioctl_snd_set_all_cast_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_all_cast_mute(hi_void *file, hi_void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_CAST_H__ */
