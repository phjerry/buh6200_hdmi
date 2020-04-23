/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao cast drv include files
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __CAST_FUNC_H__
#define __CAST_FUNC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AO_CAST_DEFATTR_FRAMEMAXNUM     8
#define AO_CAST_DEFATTR_SAMPLESPERFRAME 1024

typedef enum {
    SND_CAST_STATUS_STOP = 0,
    SND_CAST_STATUS_START,
    SND_CAST_STATUS_PAUSE,
    SND_CAST_STATUS_MAX,
} snd_cast_status;

typedef struct {
    hi_ao_cast_attr user_cast_attr;

    hi_handle h_cast;
    snd_cast_status status;
    hi_u64 user_virt_addr;
    hi_bool mute;
    hi_bool user_enable_setting;
    hi_ao_abs_gain cast_abs_gain;

    struct osal_list_head node;
    snd_op_state cast_op;

    ao_frame ref_frame;   /* cast reference ao frame */
    hi_audio_buffer map_buffer; /* cast buffer information */

    hi_bool add_mute;
    hi_u32 frame_bytes;

    hi_bool acquire_cast_frame_flag;
} snd_cast_state;

hi_s32 cast_get_def_attr(hi_ao_cast_attr *def_attr);
hi_s32 cast_create_chn(snd_card_state *card, hi_ao_cast_attr *cast_attr, hi_audio_buffer *mmz, hi_handle h_cast);
hi_s32 cast_destroy_chn(snd_card_state *card, hi_handle h_cast);
hi_s32 cast_set_info(snd_card_state *card, hi_handle h_cast, hi_u64 user_virt_addr);
hi_s32 cast_get_info(snd_card_state *card, hi_handle h_cast, ao_cast_info_param *info);
hi_s32 cast_set_enable(snd_card_state *card, hi_handle h_cast, hi_bool enable);
hi_s32 cast_get_enable(snd_card_state *card, hi_handle h_cast, hi_bool *enable);
hi_s32 cast_read_data(snd_card_state *card, hi_handle h_cast, ao_frame *frame);
hi_s32 cast_release_data(snd_card_state *card, hi_handle h_cast);
hi_void cast_get_settings(snd_card_state *card, hi_handle h_cast, snd_cast_settings *cast_settings);
hi_void cast_restore_settings(snd_card_state *card, hi_handle h_cast, snd_cast_settings *cast_settings);

hi_s32 cast_get_abs_gain(snd_card_state *card, hi_handle h_cast, hi_ao_abs_gain *abs_gain);
hi_s32 cast_set_abs_gain(snd_card_state *card, hi_handle h_cast, hi_ao_abs_gain *abs_gain);
hi_s32 cast_set_mute(snd_card_state *card, hi_handle h_cast, hi_bool mute);
hi_s32 cast_get_mute(snd_card_state *card, hi_handle h_cast, hi_bool *mute);
hi_s32 cast_set_aef_bypass(snd_card_state *card, hi_handle h_cast, hi_bool bypass);
hi_s32 cast_set_all_mute(snd_card_state *card, hi_bool mute);

#ifdef HI_PROC_SUPPORT
hi_void cast_read_proc(hi_void *p, snd_card_state *card);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of__cplusplus */

#endif  /* __CAST_FUNC_H__ */

