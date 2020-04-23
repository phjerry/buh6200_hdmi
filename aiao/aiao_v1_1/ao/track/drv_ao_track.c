/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: implement of ao track driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "hi_osal.h"
#include "drv_ao_track.h"

#if defined(HI_AUDIO_AI_SUPPORT)
#include "drv_ai_private.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

snd_track_state *ao_track_find_by_handle(hi_handle h_track)
{
    ao_snd_id snd;
    snd_card_state *card = HI_NULL;
    hi_u32 track_id = h_track & AO_TRACK_CHNID_MASK;

    ao_mgmt *mgmt = ao_get_mgmt();

    for (snd = AO_SND_0; snd < AO_SND_MAX; snd++) {
        card = mgmt->snd_entity[snd].card;
        if (card == HI_NULL) {
            continue;
        }

        if (!(card->snd_track_init_flag & (1L << track_id))) {
            continue;
        }

        return track_find_by_handle(card, h_track);
    }

    return HI_NULL;
}

hi_s32 ao_snd_free_track(hi_void *filp)
{
    hi_s32 ret;
    hi_u32 track_id;
    ao_mgmt *mgmt = ao_get_mgmt();

    for (track_id = 0; track_id < AO_MAX_TOTAL_TRACK_NUM; track_id++) {
        if (mgmt->track_entity[track_id].file != filp) {
            continue;
        }

        if (osal_atomic_read(&mgmt->track_entity[track_id].atm_use_cnt) == 0) {
            continue;
        }

        ret = ao_track_destory(track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_track_destory, ret);
            HI_ERR_PRINT_U32(track_id);
            return ret;
        }

        ao_track_free_handle_by_id(track_id);
    }

    return HI_SUCCESS;
}

hi_s32 ao_track_get_settings(hi_handle h_track, snd_track_settings *snd_settings)
{
    snd_card_state *card = HI_NULL;

    h_track &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(h_track);
    if (card != HI_NULL) {
        return track_get_setting(card, h_track, snd_settings);
    } else {
        HI_ERR_AO("track(%d) don't attach card!\n", h_track);
        return HI_FAILURE;
    }
}

hi_s32 ao_track_restore_settings(hi_handle h_track, snd_track_settings *snd_settings)
{
    snd_card_state *card = HI_NULL;

    h_track &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(h_track);
    if (card != HI_NULL) {
        return track_restore_setting(card, h_track, snd_settings);
    } else {
        HI_ERR_AO("track(%d) don't attach card!\n", h_track);
        return HI_FAILURE;
    }
}

hi_s32 ao_snd_get_alsa_track_delay_ms(ao_snd_id sound, hi_u32 *delay_ms)
{
    snd_track_state *track = HI_NULL;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(delay_ms);
    CHECK_AO_NULL_PTR(card);

    *delay_ms = 0;

    osal_list_for_each_entry(track, &card->track, node) {
        if (track->alsa_track == HI_TRUE) {
            return track_get_delay_ms(card, track->track_id, delay_ms);
        }
    }

    return HI_FAILURE;
}

hi_s32 ao_snd_set_all_track_mute(ao_snd_id sound, hi_bool mute)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    ret = track_set_all_mute(card, mute);
    if (ret != HI_SUCCESS) {
    }

    card->all_track_mute = mute;
    HI_INFO_PRINT_BOOL(card->all_track_mute);

    return ret;
}

hi_s32 ao_snd_get_all_track_mute(ao_snd_id sound, hi_bool *mute)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(mute);
    CHECK_AO_NULL_PTR(card);

    *mute = card->all_track_mute;
    HI_INFO_PRINT_BOOL(card->all_track_mute);

    return HI_SUCCESS;
}

hi_s32 ao_track_alloc_handle(ao_snd_id sound, hi_ao_track_type track_type, hi_void *file, hi_handle *ph_handle)
{
    hi_u32 i;
    ao_mgmt *mgmt = ao_get_mgmt();

    CHECK_AO_NULL_PTR(ph_handle);
    CHECK_AO_SNDCARD_OPEN(sound);

    if (mgmt->ready != HI_TRUE) {
        HI_LOG_ERR("need open first!\n");
        return HI_ERR_AO_DEV_NOT_OPEN;
    }

    if (mgmt->track_num >= AO_MAX_TOTAL_TRACK_NUM) {
        HI_LOG_ERR("too many track\n");
        HI_ERR_PRINT_U32(mgmt->track_num);
        return HI_ERR_AO_NOTSUPPORT;
    }

    for (i = 0; i < AO_MAX_TOTAL_TRACK_NUM; i++) {
        if (osal_atomic_read(&mgmt->track_entity[i].atm_use_cnt) == 0) {
            break;
        }
    }

    if (i >= AO_MAX_TOTAL_TRACK_NUM) {
        HI_LOG_ERR("too many track!\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    if (track_type == HI_AO_TRACK_TYPE_LOW_LATENCY) {
        if (mgmt->low_latency_created == HI_TRUE) {
            HI_LOG_ERR("too many low_latency track!\n");
            return HI_ERR_AO_NOTSUPPORT;
        }

        if (osal_atomic_read(&mgmt->snd_dma_entity[sound].atm_use_cnt) != 0) {
            HI_LOG_ERR("cannot create low_latency track in SND DMA mode!\n");
            return HI_ERR_AO_NOTSUPPORT;
        }
    }

    mgmt->track_entity[i].file = file;
    mgmt->track_num++;
    osal_atomic_inc_return(&mgmt->track_entity[i].atm_use_cnt);

    /*
     * define of track handle :
     * bit31                                                       bit0
     * |<----   16bit --------->|<---   8bit    --->|<---  8bit   --->|
     * |--------------------------------------------------------------|
     * |        hi_mod_id       |  sub_mod defined  |     chn_id      |
     * |--------------------------------------------------------------|
     */
    if (track_type == HI_AO_TRACK_TYPE_LOW_LATENCY) {
        *ph_handle = (HI_ID_AO << 16) | (HI_ID_LOWLATENCY_TRACK << 8) | i;
        mgmt->low_latency_created = HI_TRUE;
    } else {
        *ph_handle = (HI_ID_AO << 16) | (HI_ID_MASTER_SLAVE_TRACK << 8) | i;
    }

    return HI_SUCCESS;
}

hi_void ao_track_free_handle_by_id(hi_u32 track_id)
{
    ao_mgmt *mgmt = ao_get_mgmt();
    mgmt->low_latency_created = HI_FALSE;

    if (osal_atomic_read(&mgmt->track_entity[track_id].atm_use_cnt) == 0) {
        return;
    }

    mgmt->track_entity[track_id].file = HI_NULL;
    mgmt->track_num--;
    osal_atomic_set(&mgmt->track_entity[track_id].atm_use_cnt, 0);
}

hi_void ao_track_free_handle(hi_handle track)
{
    ao_mgmt *mgmt = ao_get_mgmt();
    if ((track & 0xff00) == (HI_ID_LOWLATENCY_TRACK << 8)) {
        mgmt->low_latency_created = HI_FALSE;
    }

    track &= AO_TRACK_CHNID_MASK;
    if (osal_atomic_read(&mgmt->track_entity[track].atm_use_cnt) == 0) {
        return;
    }
    mgmt->track_entity[track].file = HI_NULL;
    mgmt->track_num--;
    osal_atomic_set(&mgmt->track_entity[track].atm_use_cnt, 0);
}

hi_void ao_track_save_suspend_attr(hi_handle h_handle, ao_track_create_param_p track)
{
    ao_mgmt *mgmt = ao_get_mgmt();
    h_handle &= AO_TRACK_CHNID_MASK;
    mgmt->track_entity[h_handle].suspend_attr.sound = track->sound;
    mgmt->track_entity[h_handle].suspend_attr.alsa_track = track->alsa_track;
}

hi_s32 ao_track_get_def_attr(hi_ao_track_attr *attr)
{
    return track_get_def_attr(attr);
}

hi_s32 ao_track_get_attr(hi_u32 track_id, hi_ao_track_attr *track_attr)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_attr(card, track_id, track_attr);
}

hi_s32 ao_track_set_attr(hi_u32 track_id, hi_ao_track_attr *track_attr)
{
    hi_s32 ret;
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    ret = track_set_attr(card, track_id, track_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_attr, ret);
    }

    return ret;
}

hi_s32 ao_track_create(ao_snd_id sound, hi_ao_track_attr *attr,
                       hi_bool alsa_track, hi_handle h_track)
{
    hi_s32 ret;
    snd_card_state *card = HI_NULL;
    h_track &= AO_TRACK_CHNID_MASK;

    card = snd_card_get_card(sound);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    ret = track_create(card, attr, alsa_track, h_track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_create, ret);
    }

    return ret;
}

static hi_s32 ao_call_track_fun(hi_u32 track_id,
    hi_s32 (*track_fun)(snd_card_state *card, hi_u32 track_id))
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_fun(card, track_id);
}

hi_s32 ao_track_destory(hi_u32 track_id)
{
    return ao_call_track_fun(track_id, track_destroy);
}

hi_s32 ao_track_start(hi_u32 track_id)
{
    return ao_call_track_fun(track_id, track_start);
}

hi_s32 ao_track_stop(hi_u32 track_id)
{
    return ao_call_track_fun(track_id, track_stop);
}

hi_s32 ao_track_pause(hi_u32 track_id)
{
    return ao_call_track_fun(track_id, track_pause);
}

hi_s32 ao_track_flush(hi_u32 track_id)
{
    return ao_call_track_fun(track_id, track_flush);
}

hi_s32 ao_track_send_data(hi_u32 track_id, ao_frame *frame)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        if (card->dma_mode == HI_TRUE) {
            return HI_SUCCESS;
        } else {
            return track_send_data(card, track_id, frame);
        }
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

hi_s32 ao_track_set_weight(hi_u32 track_id, hi_ao_gain *gain)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        return track_set_weight(card, track_id, gain);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

hi_s32 ao_track_get_weight(hi_u32 track_id, hi_ao_gain *gain)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_weight(card, track_id, gain);
}

hi_s32 ao_track_set_abs_gain(hi_u32 track_id, hi_ao_abs_gain *abs_gain)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_set_abs_gain(card, track_id, abs_gain);
}

hi_s32 ao_track_get_abs_gain(hi_u32 track_id, hi_ao_abs_gain *abs_gain)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_abs_gain(card, track_id, abs_gain);
}

hi_s32 ao_track_set_prescale(hi_u32 track_id, hi_ao_preci_gain *preci_gain)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_set_prescale(card, track_id, preci_gain);
}

hi_s32 ao_track_get_prescale(hi_u32 track_id, hi_ao_preci_gain *preci_gain)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_prescale(card, track_id, preci_gain);
}

hi_s32 ao_track_set_mute(hi_u32 track_id, hi_bool mute)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        return track_set_mute(card, track_id, mute);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

hi_s32 ao_track_get_mute(hi_u32 track_id, hi_bool *mute)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_mute(card, track_id, mute);
}

hi_s32 ao_track_set_channel_mode(hi_u32 track_id, hi_track_mode mode)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_set_channel_mode(card, track_id, mode);
}

hi_s32 ao_track_get_channel_mode(hi_u32 track_id, hi_track_mode *mode)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_channel_mode(card, track_id, mode);
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 ao_track_set_aip_fifo_bypass(hi_u32 track_id, hi_bool enable)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        return track_set_aip_fifo_bypass(card, track_id, enable);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

hi_s32 ao_track_mmap(hi_u32 track_id, ao_track_mmap_param *param)
{
    hi_s32 ret;
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    ret = track_mmap(card, track_id, param);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_mmap, ret);
    }

    return ret;
}

hi_s32 ao_track_set_priority(hi_u32 track_id, hi_bool enable)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        return track_set_priority(card, track_id, enable);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

hi_s32 ao_track_get_priority(hi_u32 track_id, hi_bool *enable)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        return track_get_priority(card, track_id, enable);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

hi_s32 ao_track_set_fifo_latency(hi_u32 track_id, hi_u32 latency_ms)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        return track_set_fifo_latency(card, track_id, latency_ms);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

static hi_s32 ao_track_get_fifo_latency(hi_handle h_track, hi_u32 *latency)
{
    snd_card_state *card = HI_NULL;

    h_track &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(h_track);
    if (card != HI_NULL) {
        return track_get_fifo_latency(card, h_track, latency);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}
#endif

hi_s32 ao_track_set_speed_adjust(hi_u32 track_id, hi_ao_speed *speed)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        return track_set_speed_adjust(card, track_id, speed);
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }
}

hi_s32 ao_track_get_delay_ms(hi_u32 track_id, hi_u32 *delay_ms)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_delay_ms(card, track_id, delay_ms);
}

hi_s32 ao_track_get_ext_delay_ms(hi_u32 track_id, hi_u32 *delay_ms)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    *delay_ms = card->ext_delay_ms;
    return HI_SUCCESS;
}

hi_s32 ao_track_is_buf_empty(hi_u32 track_id, hi_bool *empty)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_is_buf_empty(card, track_id, empty);
}

hi_s32 ao_track_set_eos_flag(hi_u32 track_id, hi_bool eos)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_set_eos(card, track_id, eos);
}

hi_s32 ao_track_attach_ai(hi_u32 track_id, hi_handle h_ai)
{
#if defined(HI_AUDIO_AI_SUPPORT)
    hi_s32 ret;
    snd_card_state *card = HI_NULL;
    hi_handle h_track;

    h_track = track_id & AO_TRACK_CHNID_MASK;
    card = track_card_get_card(h_track);
    if (card != HI_NULL) {
        ret = track_set_pcm_attr(card, h_track, h_ai);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_set_pcm_attr, ret);
            return ret;
        }

        ret = ai_attach_track(h_ai, h_track);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ai_attach_track, ret);
            return ret;
        }

        ret = track_attach_ai(card, h_track, h_ai);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_attach_ai, ret);
            return ret;
        }
        return ret;
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

#else
    return HI_FAILURE;
#endif
}

hi_s32 ao_track_detach_ai(hi_u32 track_id, hi_handle h_ai)
{
#if defined(HI_AUDIO_AI_SUPPORT)
    hi_s32 ret;
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card != HI_NULL) {
        ret = track_detach_ai(card, track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_detach_ai, ret);
            return ret;
        }

        ret = ai_detach_track(h_ai, track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ai_detach_track, ret);
            return ret;
        }
        return ret;
    } else {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

#else
    return HI_FAILURE;
#endif
}

static hi_s32 ao_track_master_to_slave(snd_card_state *card, hi_u32 m_track_id)
{
    hi_s32 ret;
    hi_ao_track_attr *track_attr = HI_NULL;
    snd_track_settings track_settings;

    ret = track_get_setting(card, m_track_id, &track_settings);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_get_setting, ret);
        return ret;
    }

    /* master track is NOT STOP, not support exchange */
    if (track_settings.curn_status != SND_TRACK_STATUS_STOP) {
        HI_LOG_FATAL("exist master track is not stop!\n");
        HI_FATAL_PRINT_U32(m_track_id);
        return HI_ERR_AO_NOTSUPPORT;
    }

    /* destory track */
    ret = track_destroy(card, m_track_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_destroy, ret);
        return ret;
    }

    track_attr = &track_settings.track_attr;
    track_attr->track_type = HI_AO_TRACK_TYPE_SLAVE;

    /* recreate slave track */
    ret = track_create(card, track_attr, HI_FALSE, m_track_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_create, ret);
        return ret;
    }

    /* restore track setting */
    ret = track_restore_setting(card, m_track_id, &track_settings);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_restore_setting, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_track_slave_to_master(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    hi_ao_track_attr *track_attr = HI_NULL;
    snd_track_settings track_settings;

    ret = track_get_setting(card, track_id, &track_settings);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_get_setting, ret);
        return ret;
    }

    /* destory track */
    ret = track_destroy(card, track_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_destroy, ret);
        return ret;
    }

    track_attr = &track_settings.track_attr;
    track_attr->track_type = HI_AO_TRACK_TYPE_MASTER;

    /* recreate master track */
    ret = track_create(card, track_attr, HI_FALSE, track_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_create, ret);
        return ret;
    }

    ret = track_restore_setting(card, track_id, &track_settings);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_restore_setting, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_track_master_slave_exchange(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    hi_u32 m_track_id;

    m_track_id = track_get_master_id(card);
    if (m_track_id != AO_MAX_TOTAL_TRACK_NUM) {
        /* master -> slave */
        ret = ao_track_master_to_slave(card, m_track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_track_master_to_slave, ret);
            return ret;
        }
    }

    if (m_track_id == track_id) {
        /* if input track id is master, just return here */
        return HI_SUCCESS;
    } else {
        /* slave -> master */
        ret = ao_track_slave_to_master(card, track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_track_slave_to_master, ret);
            return ret;
        }

        return HI_SUCCESS;
    }
}

static hi_s32 ao_track_pre_create(ao_snd_id sound, hi_ao_track_attr *attr,
    hi_bool alsa_track, hi_handle h_track)
{
    hi_u32 track_id;
    hi_s32 ret;
    snd_card_state *card = HI_NULL;

    h_track &= AO_TRACK_CHNID_MASK;
    card = snd_card_get_card(sound);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    ret = track_check_attr(attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_check_attr, ret);
        return ret;
    }

    if (attr->track_type == HI_AO_TRACK_TYPE_MASTER) {
        track_id = track_get_master_id(card);
        if (track_id != AO_MAX_TOTAL_TRACK_NUM) {
            /*
             * check if master track exist
             * force master to slave
             */
            ret = ao_track_master_slave_exchange(card, track_id);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(ao_track_master_slave_exchange, ret);
                return ret;
            }
        }
    }

    return track_create(card, attr, alsa_track, h_track);
}

hi_s32 ao_track_pre_set_attr(hi_u32 track_id, hi_ao_track_attr *track_attr)
{
    hi_s32 ret;
    snd_card_state *card = HI_NULL;
    snd_track_attr_setting attr_setting = SND_TRACK_ATTR_MAX;
    hi_ao_track_attr track_tmp_attr = { 0 };

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    ret = track_detect_attr(card, track_id, track_attr, &attr_setting);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_detect_attr, ret);
        return ret;
    }

    switch (attr_setting) {
        case SND_TRACK_ATTR_MODIFY: {
            ret = ao_track_set_attr(track_id, track_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(ao_track_set_attr, ret);
            }
            return ret;
        }

        case SND_TRACK_ATTR_MASTER2SLAVE:
        case SND_TRACK_ATTR_SLAVE2MASTER: {
            ret = ao_track_get_attr(track_id, &track_tmp_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(ao_track_get_attr, ret);
                return ret;
            }

            ret = ao_track_set_attr(track_id, track_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(ao_track_set_attr, ret);
                return ret;
            }

            ret = ao_track_master_slave_exchange(card, track_id);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(ao_track_master_slave_exchange, ret);
                /* exchange failed, restore track attr */
                ret = ao_track_set_attr(track_id, &track_tmp_attr);
                if (ret != HI_SUCCESS) {
                    HI_ERR_PRINT_FUNC_RES(ao_track_set_attr, ret);
                    return ret;
                }

                return HI_FAILURE;
            }
            return ret;
        }

        default: {
            return HI_SUCCESS;
        }
    }
}

hi_s32 ao_track_set_used_by_kernel(hi_u32 track_id)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_set_used_by_kernel(card, track_id);
}

#ifdef HI_SND_ADVANCED_SUPPORT
static hi_s32 ao_track_set_resume_threshold_ms(hi_u32 track_id, hi_u32 threshold_ms)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_set_resume_threshold_ms(card, track_id, threshold_ms);
}

static hi_s32 ao_track_get_resume_threshold_ms(hi_u32 track_id, hi_u32 *threshold_ms)
{
    snd_card_state *card = HI_NULL;

    track_id &= AO_TRACK_CHNID_MASK;
    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_resume_threshold_ms(card, track_id, threshold_ms);
}

static hi_s32 ao_track_set_fade_attr(hi_u32 track_id, ao_track_fade *track_fade)
{
    hi_s32 ret;
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    ret = track_set_fade_attr(card, track_id, track_fade);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_fade_attr, ret);
    }

    return ret;
}

static hi_s32 ao_track_set_info(hi_u32 track_id, hi_u64 t_vir_addr)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_set_info(card, track_id, t_vir_addr);
}

static hi_s32 ao_track_get_info(hi_u32 track_id, hi_u64 *vir_addr, hi_u64 *phy_addr)
{
    snd_card_state *card = HI_NULL;
    track_id &= AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    return track_get_info(card, track_id, vir_addr, phy_addr);
}
#endif

hi_s32 ao_snd_set_all_track_prescale(ao_snd_id sound, hi_ao_preci_gain *preci_gain)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(preci_gain);

    ret = track_set_all_prescale(card, preci_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_all_prescale, ret);
        return ret;
    }

    card->all_track_preci_gain.integer = preci_gain->integer;
    card->all_track_preci_gain.decimal = preci_gain->decimal;

    return ret;
}

hi_s32 ao_snd_get_all_track_prescale(ao_snd_id sound, hi_ao_preci_gain *preci_gain)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    preci_gain->integer = card->all_track_preci_gain.integer;
    preci_gain->decimal = card->all_track_preci_gain.decimal;

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_snd_set_all_track_mute(hi_void *file, hi_void *arg)
{
    ao_snd_all_track_mute_param_p all_mute = (ao_snd_all_track_mute_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(all_mute->sound);
    return ao_snd_set_all_track_mute(all_mute->sound, all_mute->mute);
}

hi_s32 ao_ioctl_snd_get_all_track_mute(hi_void *file, hi_void *arg)
{
    ao_snd_all_track_mute_param_p all_mute = (ao_snd_all_track_mute_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(all_mute->sound);
    return ao_snd_get_all_track_mute(all_mute->sound, &all_mute->mute);
}

hi_s32 ao_ioctl_track_get_def_attr(hi_void *file, hi_void *arg)
{
    return ao_track_get_def_attr((hi_ao_track_attr *)arg);
}

hi_s32 ao_ioctl_track_create(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    hi_handle h_track;
    ao_track_create_param_p track = (ao_track_create_param_p)arg;

    ret = ao_track_alloc_handle(track->sound, track->attr.track_type, file, &h_track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_track_alloc_handle, ret);
        return ret;
    }

    ret = ao_track_pre_create(track->sound, &track->attr, track->alsa_track,
                              h_track);
    if (ret != HI_SUCCESS) {
        ao_track_free_handle(h_track);
        return ret;
    }
    ao_track_save_suspend_attr(h_track, track);
    track->h_track = h_track;

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_track_destory(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    hi_handle h_track = *(hi_handle *)arg;

    CHECK_AO_TRACK_OPEN(h_track);
    ret = ao_track_destory(h_track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_track_destory, ret);
        return ret;
    }

    ao_track_free_handle(h_track);

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_track_start(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    hi_handle h_track = *(hi_handle *)arg;

    CHECK_AO_TRACK_OPEN(h_track);

    ret = ao_track_start(h_track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_track_start, ret);
    }

    return ret;
}

hi_s32 ao_ioctl_track_stop(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    hi_handle h_track = *(hi_handle *)arg;

    CHECK_AO_TRACK_OPEN(h_track);

    ret = ao_track_stop(h_track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_track_stop, ret);
    }

    return ret;
}

hi_s32 ao_ioctl_track_pause(hi_void *file, hi_void *arg)
{
    hi_handle h_track = *(hi_handle *)arg;
    CHECK_AO_TRACK_OPEN(h_track);
    return ao_track_pause(h_track);
}

hi_s32 ao_ioctl_track_flush(hi_void *file, hi_void *arg)
{
    hi_handle h_track = *(hi_handle *)arg;
    CHECK_AO_TRACK_OPEN(h_track);
    return ao_track_flush(h_track);
}

hi_s32 ao_ioctl_track_send_data(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    ao_track_send_data_param_p data = (ao_track_send_data_param_p)arg;

    CHECK_AO_TRACK_OPEN(data->h_track);

    ret = ao_track_send_data(data->h_track, &data->ao_frame);
    if (ret != HI_SUCCESS && ret != HI_ERR_AO_OUT_BUF_FULL) {
        HI_ERR_PRINT_FUNC_RES(ao_track_send_data, ret);
    }

    return ret;
}

hi_s32 ao_ioctl_track_set_weight(hi_void *file, hi_void *arg)
{
    ao_track_weight_param_p weight = (ao_track_weight_param_p)arg;
    CHECK_AO_TRACK_OPEN(weight->h_track);
    return ao_track_set_weight(weight->h_track, &weight->track_gain);
}

hi_s32 ao_ioctl_track_get_weight(hi_void *file, hi_void *arg)
{
    ao_track_weight_param_p weight = (ao_track_weight_param_p)arg;
    CHECK_AO_TRACK_OPEN(weight->h_track);
    return ao_track_get_weight(weight->h_track, &weight->track_gain);
}

hi_s32 ao_ioctl_track_set_speed_adjust(hi_void *file, hi_void *arg)
{
    ao_track_speed_adjust_param_p speed = (ao_track_speed_adjust_param_p)arg;
    CHECK_AO_TRACK_OPEN(speed->h_track);
    return ao_track_set_speed_adjust(speed->h_track, &speed->speed);
}

hi_s32 ao_ioctl_track_get_delay_ms(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    hi_u32 track_delay_ms = 0;
    hi_u32 ext_delay_ms = 0;
    ao_track_delay_ms_param_p delay_ms = (ao_track_delay_ms_param_p)arg;

    CHECK_AO_TRACK_OPEN(delay_ms->h_track);

    ret = ao_track_get_delay_ms(delay_ms->h_track, &track_delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_track_get_delay_ms, ret);
        return ret;
    }

    ret = ao_track_get_ext_delay_ms(delay_ms->h_track, &ext_delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_track_get_ext_delay_ms, ret);
        return ret;
    }

    delay_ms->delay_ms = track_delay_ms + ext_delay_ms;

    return ret;
}

hi_s32 ao_ioctl_track_is_buf_empty(hi_void *file, hi_void *arg)
{
    ao_track_buf_empty_param_p buf_empty = (ao_track_buf_empty_param_p)arg;
    CHECK_AO_TRACK_OPEN(buf_empty->h_track);
    return ao_track_is_buf_empty(buf_empty->h_track, &buf_empty->empty);
}

hi_s32 ao_ioctl_track_set_eos_flag(hi_void *file, hi_void *arg)
{
    ao_track_eos_flag_param_p eos_flag = (ao_track_eos_flag_param_p)arg;
    CHECK_AO_TRACK_OPEN(eos_flag->h_track);
    return ao_track_set_eos_flag(eos_flag->h_track, eos_flag->eos_flag);
}

hi_s32 ao_ioctl_track_set_attr(hi_void *file, hi_void *arg)
{
    ao_track_attr_param_p track_attr = (ao_track_attr_param_p)arg;
    CHECK_AO_TRACK_OPEN(track_attr->h_track);
    return ao_track_pre_set_attr(track_attr->h_track, &track_attr->attr);
}

hi_s32 ao_ioctl_track_get_attr(hi_void *file, hi_void *arg)
{
    ao_track_attr_param_p track_attr = (ao_track_attr_param_p)arg;
    CHECK_AO_TRACK_OPEN(track_attr->h_track);
    return ao_track_get_attr(track_attr->h_track, &track_attr->attr);
}

#ifdef HI_AUDIO_AI_SUPPORT
hi_s32 ao_ioctl_track_attach_ai(hi_void *file, hi_void *arg)
{
    ao_track_att_ai_param_p track_att_ai = (ao_track_att_ai_param_p)arg;
    CHECK_AI_ID(track_att_ai->h_ai);
    CHECK_AO_TRACK_OPEN(track_att_ai->h_track);
    return ao_track_attach_ai(track_att_ai->h_track, track_att_ai->h_ai);
}

hi_s32 ao_ioctl_track_detach_ai(hi_void *file, hi_void *arg)
{
    ao_track_att_ai_param_p track_att_ai = (ao_track_att_ai_param_p)arg;
    CHECK_AI_ID(track_att_ai->h_ai);
    CHECK_AO_TRACK_OPEN(track_att_ai->h_track);
    return ao_track_detach_ai(track_att_ai->h_track, track_att_ai->h_ai);
}
#endif

hi_s32 ao_ioctl_track_set_abs_gain(hi_void *file, hi_void *arg)
{
    ao_track_abs_gain_param_p abs_gain = (ao_track_abs_gain_param_p)arg;
    CHECK_AO_TRACK_OPEN(abs_gain->h_track);
    return ao_track_set_abs_gain(abs_gain->h_track, &abs_gain->track_abs_gain);
}

hi_s32 ao_ioctl_track_get_abs_gain(hi_void *file, hi_void *arg)
{
    ao_track_abs_gain_param_p abs_gain = (ao_track_abs_gain_param_p)arg;
    CHECK_AO_TRACK_OPEN(abs_gain->h_track);
    return ao_track_get_abs_gain(abs_gain->h_track, &abs_gain->track_abs_gain);
}

hi_s32 ao_ioctl_track_set_mute(hi_void *file, hi_void *arg)
{
    ao_track_mute_param_p mute = (ao_track_mute_param_p)arg;
    CHECK_AO_TRACK_OPEN(mute->h_track);
    return ao_track_set_mute(mute->h_track, mute->mute);
}

hi_s32 ao_ioctl_track_get_mute(hi_void *file, hi_void *arg)
{
    ao_track_mute_param_p mute = (ao_track_mute_param_p)arg;
    CHECK_AO_TRACK_OPEN(mute->h_track);
    return ao_track_get_mute(mute->h_track, &mute->mute);
}

hi_s32 ao_ioctl_track_set_channel_mode(hi_void *file, hi_void *arg)
{
    ao_track_channel_mode_param_p channel_mode = (ao_track_channel_mode_param_p)arg;
    CHECK_AO_TRACK_OPEN(channel_mode->h_track);
    return ao_track_set_channel_mode(channel_mode->h_track, channel_mode->mode);
}

hi_s32 ao_ioctl_track_get_channel_mode(hi_void *file, hi_void *arg)
{
    ao_track_channel_mode_param_p channel_mode = (ao_track_channel_mode_param_p)arg;
    CHECK_AO_TRACK_OPEN(channel_mode->h_track);
    return ao_track_get_channel_mode(channel_mode->h_track, &channel_mode->mode);
}

hi_s32 ao_ioctl_track_set_prescale(hi_void *file, hi_void *arg)
{
    ao_track_prescale_param_p prescale = (ao_track_prescale_param_p)arg;
    CHECK_AO_TRACK_OPEN(prescale->h_track);
    return ao_track_set_prescale(prescale->h_track, &prescale->preci_gain);
}

hi_s32 ao_ioctl_track_get_prescale(hi_void *file, hi_void *arg)
{
    ao_track_prescale_param_p prescale = (ao_track_prescale_param_p)arg;
    CHECK_AO_TRACK_OPEN(prescale->h_track);
    return ao_track_get_prescale(prescale->h_track, &prescale->preci_gain);
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 ao_ioctl_track_set_fifo_latency(hi_void *file, hi_void *arg)
{
    ao_track_fifo_latency_p fifo_latency = (ao_track_fifo_latency_p)arg;
    CHECK_AO_TRACK_OPEN(fifo_latency->h_track);
    return ao_track_set_fifo_latency(fifo_latency->h_track, fifo_latency->latency_ms);
}

hi_s32 ao_ioctl_track_get_fifo_latency(hi_void *file, hi_void *arg)
{
    ao_track_fifo_latency_p fifo_latency = (ao_track_fifo_latency_p)arg;
    CHECK_AO_TRACK_OPEN(fifo_latency->h_track);
    return ao_track_get_fifo_latency(fifo_latency->h_track, &fifo_latency->latency_ms);
}

hi_s32 ao_ioctl_track_set_fifo_bypass(hi_void *file, hi_void *arg)
{
    ao_track_fifo_bypass_param_p param = (ao_track_fifo_bypass_param_p)arg;
    CHECK_AO_TRACK_OPEN(param->h_track);
    return ao_track_set_aip_fifo_bypass(param->h_track, param->enable);
}

hi_s32 ao_ioctl_track_mmap(hi_void *file, hi_void *arg)
{
    ao_track_mmap_param_p param = (ao_track_mmap_param_p)arg;
    CHECK_AO_TRACK_OPEN(param->h_track);
    return ao_track_mmap(param->h_track, param);
}

hi_s32 ao_ioctl_track_set_priority(hi_void *file, hi_void *arg)
{
    ao_track_priority_param_p param = (ao_track_priority_param_p)arg;
    CHECK_AO_TRACK_OPEN(param->h_track);
    return ao_track_set_priority(param->h_track, param->enable);
}

hi_s32 ao_ioctl_track_get_priority(hi_void *file, hi_void *arg)
{
    ao_track_priority_param_p param = (ao_track_priority_param_p)arg;
    CHECK_AO_TRACK_OPEN(param->h_track);
    return ao_track_get_priority(param->h_track, &param->enable);
}

hi_s32 ao_ioctl_track_set_resume_threshold_ms(hi_void *file, hi_void *arg)
{
    ao_track_resume_threshold_p param = (ao_track_resume_threshold_p)arg;
    CHECK_AO_TRACK_OPEN(param->h_track);
    return ao_track_set_resume_threshold_ms(param->h_track, param->threshold_ms);
}

hi_s32 ao_ioctl_track_get_resume_threshold_ms(hi_void *file, hi_void *arg)
{
    ao_track_resume_threshold_p param = (ao_track_resume_threshold_p)arg;
    CHECK_AO_TRACK_OPEN(param->h_track);
    return ao_track_get_resume_threshold_ms(param->h_track, &param->threshold_ms);
}

hi_s32 ao_ioctl_track_set_fade_attr(hi_void *file, hi_void *arg)
{
    ao_track_fade_param_p track_fade = (ao_track_fade_param_p)arg;
    CHECK_AO_TRACK_OPEN(track_fade->h_track);
    return ao_track_set_fade_attr(track_fade->h_track, &track_fade->fade);
}

hi_s32 ao_ioctl_track_set_info(hi_void *file, hi_void *arg)
{
    ao_track_info_param_p track_info = (ao_track_info_param_p)arg;
    CHECK_AO_TRACK_OPEN(track_info->h_track);
    return ao_track_set_info(track_info->h_track, track_info->user_virt_addr);
}

hi_s32 ao_ioctl_track_get_info(hi_void *file, hi_void *arg)
{
    ao_track_info_param_p track_info = (ao_track_info_param_p)arg;
    CHECK_AO_TRACK_OPEN(track_info->h_track);
    return ao_track_get_info(track_info->h_track, &track_info->user_virt_addr, &track_info->phy_addr);
}
#endif

hi_s32 ao_ioctl_snd_set_all_track_prescale(hi_void *file, hi_void *arg)
{
    ao_snd_all_track_prescale_param_p all_track_prescale = (ao_snd_all_track_prescale_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(all_track_prescale->sound);
    return ao_snd_set_all_track_prescale(all_track_prescale->sound, &all_track_prescale->all_track_prescale);
}

hi_s32 ao_ioctl_snd_get_all_track_prescale(hi_void *file, hi_void *arg)
{
    ao_snd_all_track_prescale_param_p all_track_prescale = (ao_snd_all_track_prescale_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(all_track_prescale->sound);
    return ao_snd_get_all_track_prescale(all_track_prescale->sound, &all_track_prescale->all_track_prescale);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

