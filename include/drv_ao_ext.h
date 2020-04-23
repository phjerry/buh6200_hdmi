/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: ao drv api for other modules
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_EXT_H__
#define __DRV_AO_EXT_H__

#include "hi_drv_ao.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    hi_s32 (*ao_drv_resume)(hi_void);
    hi_s32 (*ao_drv_suspend)(hi_void);

    hi_s32 (*ao_drv_get_pll_cfg)(hi_u32 *, hi_u32 *);

    hi_s32 (*ao_track_get_def_attr)(hi_ao_track_attr *);
    hi_s32 (*ao_track_alloc_handle)(ao_snd_id, hi_ao_track_type, hi_void *, hi_handle *);
    hi_void (*ao_track_free_handle)(hi_handle);
    hi_s32 (*ao_track_create)(ao_snd_id, hi_ao_track_attr *, hi_bool, ao_buf_attr *, hi_handle);
    hi_s32 (*ao_track_destory)(hi_u32);
    hi_s32 (*ao_track_start)(hi_u32);
    hi_s32 (*ao_track_stop)(hi_u32);
    hi_s32 (*ao_track_send_data)(hi_u32, ao_frame *);
} aiao_export_func;

typedef struct {
    hi_s32  (*mmz_alloc)(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *mmz_buf);
    hi_void (*mmz_release)(const hi_audio_buffer *mmz_buf);
    hi_s32  (*sec_mmz_alloc)(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *mmz_buf);
    hi_void (*sec_mmz_release)(const hi_audio_buffer *mmz_buf);

    hi_s32  (*smmu_alloc)(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *smmu_buf);
    hi_void (*smmu_release)(const hi_audio_buffer *smmu_buf);
    hi_s32  (*sec_smmu_alloc)(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *smmu_buf);
    hi_void (*sec_smmu_release)(const hi_audio_buffer *smmu_buf);
} audio_export_func;

hi_s32  ao_drv_mod_init(hi_void);
hi_void ao_drv_mod_exit(hi_void);

hi_s32  aiao_drv_mod_init(hi_void);
hi_void aiao_drv_mod_exit(hi_void);

hi_s32 ao_drv_resume(hi_void);
hi_s32 ao_drv_suspend(hi_void);
hi_s32 ao_drv_get_pll_cfg(hi_u32 *frac_cfg, hi_u32 *int_cfg);

hi_s32 hi_drv_ao_init(hi_void);
hi_s32 hi_drv_ao_ext_init(hi_void);
hi_void hi_drv_ao_deinit(hi_void);
hi_s32 hi_drv_ao_snd_init(hi_void *file);
hi_s32 hi_drv_ao_snd_deinit(hi_void *file);
hi_s32 hi_drv_ao_snd_get_default_open_attr(ao_snd_id sound, hi_ao_attr *attr);
hi_s32 hi_drv_ao_snd_open(ao_snd_id sound, hi_ao_attr *attr, hi_void *file);
hi_s32 hi_drv_ao_snd_close(ao_snd_id sound, hi_void *file);
hi_s32 hi_drv_ao_snd_set_volume(ao_snd_id sound, hi_ao_port ao_port, hi_ao_gain *gain);
hi_s32 hi_drv_ao_snd_set_mute(ao_snd_id sound, hi_ao_port ao_port, hi_bool mute);
hi_s32 hi_drv_ao_snd_get_mute(ao_snd_id sound, hi_ao_port ao_port, hi_bool *mute);
hi_s32 hi_drv_ao_track_get_default_open_attr(hi_ao_track_type track_type, hi_ao_track_attr *attr);
hi_s32 hi_drv_ao_track_create(ao_snd_id sound, hi_ao_track_attr *attr, hi_bool alsa_track, hi_void *file, hi_handle *track);
hi_s32 hi_drv_ao_track_destroy(hi_handle track);
hi_s32 hi_drv_ao_track_flush(hi_handle track);
hi_s32 hi_drv_ao_track_start(hi_handle track);
hi_s32 hi_drv_ao_track_stop(hi_handle track);
hi_s32 hi_drv_ao_track_get_delay(hi_handle track, hi_u32 *delay);
hi_s32 hi_drv_ao_snd_get_alsa_track_delay(ao_snd_id sound, hi_u32 *delay);
hi_s32 hi_drv_ao_track_send_data(hi_handle track, ao_frame *frame);
hi_s32 hi_drv_ao_track_atomic_send_data(hi_handle h_track, ao_frame *frame);
hi_s32 hi_drv_ao_track_attach_ai(hi_handle track, hi_handle ai);
hi_s32 hi_drv_ao_track_detach_ai(hi_handle track, hi_handle ai);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __DRV_AO_EXT_H__ */
