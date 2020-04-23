/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao track header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_TRACK_H__
#define __DRV_AO_TRACK_H__

#include "track_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 ao_snd_free_track(hi_void *filp);
hi_s32 ao_track_get_settings(hi_handle h_track, snd_track_settings *snd_settings);
hi_s32 ao_track_restore_settings(hi_handle h_track, snd_track_settings *snd_settings);
hi_s32 ao_snd_get_alsa_track_delay_ms(ao_snd_id sound, hi_u32 *delay_ms);
hi_s32 ao_snd_set_all_track_mute(ao_snd_id sound, hi_bool mute);
hi_s32 ao_snd_get_all_track_mute(ao_snd_id sound, hi_bool *mute);
hi_s32 ao_track_alloc_handle(ao_snd_id sound, hi_ao_track_type track_type, hi_void *file, hi_handle *ph_handle);
hi_void ao_track_free_handle_by_id(hi_u32 track_id);
hi_void ao_track_free_handle(hi_handle track);
hi_void ao_track_save_suspend_attr(hi_handle h_handle, ao_track_create_param_p track);
hi_s32 ao_track_get_def_attr(hi_ao_track_attr *attr);
hi_s32 ao_track_get_attr(hi_u32 track_id, hi_ao_track_attr *track_attr);
hi_s32 ao_track_set_attr(hi_u32 track_id, hi_ao_track_attr *track_attr);
hi_s32 ao_track_create(ao_snd_id sound, hi_ao_track_attr *attr,
                       hi_bool alsa_track, hi_handle h_track);
hi_s32 ao_track_destory(hi_u32 track_id);
hi_s32 ao_track_start(hi_u32 track_id);
hi_s32 ao_track_stop(hi_u32 track_id);
hi_s32 ao_track_pause(hi_u32 track_id);
hi_s32 ao_track_flush(hi_u32 track_id);
hi_s32 ao_track_set_aip_fifo_bypass(hi_u32 track_id, hi_bool enable);
hi_s32 ao_track_send_data(hi_u32 track_id, ao_frame *frame);
hi_s32 ao_track_mmap(hi_u32 track_id, ao_track_mmap_param *param);
hi_s32 ao_track_set_weight(hi_u32 track_id, hi_ao_gain *gain);
hi_s32 ao_track_get_weight(hi_u32 track_id, hi_ao_gain *gain);
hi_s32 ao_track_set_abs_gain(hi_u32 track_id, hi_ao_abs_gain *abs_gain);
hi_s32 ao_track_get_abs_gain(hi_u32 track_id, hi_ao_abs_gain *abs_gain);
hi_s32 ao_track_set_prescale(hi_u32 track_id, hi_ao_preci_gain *preci_gain);
hi_s32 ao_track_get_prescale(hi_u32 track_id, hi_ao_preci_gain *preci_gain);
hi_s32 ao_track_set_mute(hi_u32 track_id, hi_bool mute);
hi_s32 ao_track_get_mute(hi_u32 track_id, hi_bool *mute);
hi_s32 ao_track_set_channel_mode(hi_u32 track_id, hi_track_mode mode);
hi_s32 ao_track_get_channel_mode(hi_u32 track_id, hi_track_mode *mode);
hi_s32 ao_track_set_priority(hi_u32 track_id, hi_bool enable);
hi_s32 ao_track_get_priority(hi_u32 track_id, hi_bool *enable);
hi_s32 ao_track_set_fifo_latency(hi_u32 track_id, hi_u32 latency_ms);
hi_s32 ao_track_set_speed_adjust(hi_u32 track_id, hi_ao_speed *speed);
hi_s32 ao_track_get_delay_ms(hi_u32 track_id, hi_u32 *delay_ms);
hi_s32 ao_track_get_ext_delay_ms(hi_u32 track_id, hi_u32 *delay_ms);
hi_s32 ao_track_is_buf_empty(hi_u32 track_id, hi_bool *empty);
hi_s32 ao_track_set_eos_flag(hi_u32 track_id, hi_bool eos);
hi_s32 ao_track_attach_ai(hi_u32 track_id, hi_handle h_ai);
hi_s32 ao_track_detach_ai(hi_u32 track_id, hi_handle h_ai);
hi_s32 ao_track_pre_set_attr(hi_u32 track_id, hi_ao_track_attr *track_attr);
hi_s32 ao_track_set_used_by_kernel(hi_u32 track_id);
hi_s32 ao_snd_set_all_track_prescale(ao_snd_id sound, hi_ao_preci_gain *preci_gain);
hi_s32 ao_snd_get_all_track_prescale(ao_snd_id sound, hi_ao_preci_gain *preci_gain);

/* ao track ioctl */
hi_s32 ao_ioctl_snd_set_all_track_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_all_track_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_all_track_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_all_track_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_def_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_create(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_destory(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_start(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_stop(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_pause(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_flush(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_send_data(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_weight(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_weight(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_speed_adjust(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_delay_ms(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_is_buf_empty(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_eos_flag(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_abs_gain(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_abs_gain(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_mute(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_channel_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_channel_mode(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_all_track_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_all_track_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_all_track_prescale(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_all_track_prescale(hi_void *file, hi_void *arg);

#ifdef HI_AUDIO_AI_SUPPORT
hi_s32 ao_ioctl_track_attach_ai(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_detach_ai(hi_void *file, hi_void *arg);
#endif

hi_s32 ao_ioctl_track_set_fifo_latency(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_fifo_latency(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_fifo_bypass(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_mmap(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_priority(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_priority(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_resume_threshold_ms(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_resume_threshold_ms(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_fade_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_set_info(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_track_get_info(hi_void *file, hi_void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HI_AO_TRACK_H__ */
