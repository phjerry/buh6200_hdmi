/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv cast implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "audio_util.h"

#include "drv_ao_cast.h"
#include "cast_func.h"
#include "track_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CHECK_AO_CAST_OPEN(cast) \
    do { \
        ao_mgmt *_mgmt = ao_get_mgmt(); \
        CHECK_AO_CAST((cast)); \
        if (osal_atomic_read(&_mgmt->cast_entity[(cast) & AO_CAST_CHNID_MASK].atm_use_cnt) == 0) { \
            HI_LOG_WARN("invalid cast id\n"); \
            HI_WARN_PRINT_H32((cast)); \
            return HI_ERR_AO_INVALID_PARA; \
        } \
    } while (0)

hi_s32 ao_snd_set_all_cast_mute(ao_snd_id sound, hi_bool mute)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);
    CHECK_AO_NULL_PTR(card);

    ret = cast_set_all_mute(card, mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_set_all_mute, ret);
        return ret;
    }

    card->all_cast_mute = mute;
    HI_INFO_PRINT_U32(card->all_cast_mute);

    return ret;
}

hi_s32 ao_snd_get_all_cast_mute(ao_snd_id sound, hi_bool *mute)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(mute);
    CHECK_AO_NULL_PTR(card);

    *mute = card->all_cast_mute;
    return HI_SUCCESS;
}

hi_s32 ao_cast_alloc_handle(hi_handle *ph_handle, hi_void *file, hi_ao_cast_attr *user_cast_attr)
{
    hi_u32 i;
    hi_s32 ret;
    hi_u32 frame_size;
    hi_u32 buf_size;
    hi_audio_buffer rbf_mmz;
    ao_mgmt *mgmt = ao_get_mgmt();

    CHECK_AO_NULL_PTR(ph_handle);

    /* check ready flag */
    if (mgmt->ready != HI_TRUE) {
        HI_LOG_ERR("need open first!\n");
        return HI_ERR_AO_DEV_NOT_OPEN;
    }

    /* allocate new channel */
    for (i = 0; i < AO_MAX_CAST_NUM; i++) {
        if (osal_atomic_read(&mgmt->cast_entity[i].atm_use_cnt) == 0) {
            mgmt->cast_entity[i].file = HI_NULL;
            break;
        }
    }

    if (i >= AO_MAX_CAST_NUM) {
        HI_LOG_ERR("too many cast chans!\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    /* 48kHz 2ch 16bit */
    frame_size = autil_calc_frame_size(AO_TRACK_NORMAL_CHANNELNUM, AO_TRACK_BITDEPTH_LOW);
    buf_size = user_cast_attr->pcm_frame_max_num * user_cast_attr->pcm_samples * frame_size;
    if (buf_size > AO_CAST_MMZSIZE_MAX) {
        HI_LOG_ERR("invalid cast frame_max_num(%d), pcm_samples_per_frame(%d)!\n",
            user_cast_attr->pcm_frame_max_num, user_cast_attr->pcm_samples);
        return HI_ERR_AO_INVALID_PARA;
    }

    /* allocate cast mmz resource */
    ret = hi_drv_audio_mmz_alloc("ao_cast", AO_CAST_MMZSIZE_MAX, HI_FALSE, &rbf_mmz);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_audio_mmz_alloc, ret);
        return HI_ERR_AO_MALLOC_FAILED;
    }

    mgmt->cast_entity[i].rbf_mmz = rbf_mmz;

    /* allocate resource */
    mgmt->cast_entity[i].file = file;
    osal_atomic_inc_return(&mgmt->cast_entity[i].atm_use_cnt);
    *ph_handle = (HI_ID_AO << CAST_BIT_SHIFT_16) | (HI_ID_CAST << CAST_BIT_SHIFT_8) | i;

    return HI_SUCCESS;
}

hi_void ao_cast_free_handle(hi_handle cast)
{
    ao_mgmt *mgmt = ao_get_mgmt();

    cast &= AO_CAST_CHNID_MASK;

    if (cast >= AO_MAX_CAST_NUM) {
        return;
    }

    if (osal_atomic_read(&mgmt->cast_entity[cast].atm_use_cnt) == 1) {
        /* free cast mmz resource */
        hi_drv_audio_mmz_release(&mgmt->cast_entity[cast].rbf_mmz);

        mgmt->cast_entity[cast].file = HI_NULL;
        osal_atomic_set(&mgmt->cast_entity[cast].atm_use_cnt, 0);
    }
}

hi_s32 ao_snd_free_cast(hi_void *filp)
{
    hi_s32 ret;
    hi_u32 cast_id;
    ao_mgmt *mgmt = ao_get_mgmt();

    for (cast_id = 0; cast_id < AO_MAX_CAST_NUM; cast_id++) {
        if (mgmt->cast_entity[cast_id].file != filp) {
            continue;
        }

        if (!osal_atomic_read(&mgmt->cast_entity[cast_id].atm_use_cnt)) {
            continue;
        }

        ret = ao_cast_destory(cast_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_cast_destory, ret);
            HI_ERR_PRINT_U32(cast_id);
            return ret;
        }

        ao_cast_free_handle(cast_id);
    }

    return HI_SUCCESS;
}

static snd_card_state *cast_card_get_card(hi_u32 id)
{
    ao_snd_id snd;
    snd_card_state *card = HI_NULL;

    if (id >= AO_MAX_CAST_NUM) {
        HI_LOG_WARN("invalid cast id\n");
        HI_WARN_PRINT_U32(id);
        return HI_NULL;
    }

    for (snd = AO_SND_0; snd < AO_SND_MAX; snd++) {
        card = snd_card_get_card(snd);
        if (card != HI_NULL) {
            if (card->snd_cast_init_flag & (1L << id)) {
                return card;
            }
        }
    }

    return HI_NULL;
}

static hi_s32 ao_cast_set_mute(hi_handle h_cast, hi_bool mute)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_set_mute(card, h_cast, mute);
}

static hi_s32 ao_cast_get_mute(hi_handle h_cast, hi_bool *mute)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_get_mute(card, h_cast, mute);
}

static hi_s32 ao_cast_set_abs_gain(hi_handle h_cast, hi_ao_abs_gain *cast_abs_gain)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_set_abs_gain(card, h_cast, cast_abs_gain);
}

static hi_s32 ao_cast_get_abs_gain(hi_handle h_cast, hi_ao_abs_gain *cast_abs_gain)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_get_abs_gain(card, h_cast, cast_abs_gain);
}

hi_s32 ao_cast_set_aef_bypass(hi_handle h_cast, hi_bool bypass)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_set_aef_bypass(card, h_cast, bypass);
}

hi_s32 ao_cast_get_def_attr(hi_ao_cast_attr *def_attr)
{
    return cast_get_def_attr(def_attr);
}

static hi_void ao_cast_save_suspend_attr(ao_snd_id sound, hi_handle h_handle, hi_ao_cast_attr *cast_attr)
{
    ao_mgmt *mgmt = ao_get_mgmt();

    h_handle &= AO_TRACK_CHNID_MASK;
    mgmt->cast_entity[h_handle].suspend_attr.sound = sound;
    mgmt->cast_entity[h_handle].suspend_attr.cast_attr = *cast_attr;
}

hi_s32 ao_cast_create(ao_snd_id sound, hi_ao_cast_attr *cast_attr, hi_audio_buffer *mmz, hi_handle h_cast)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    ret = cast_create_chn(card, cast_attr, mmz, h_cast);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_create_chn, ret);
        return ret;
    }

    card->snd_cast_init_flag |= ((hi_u32)1L << (h_cast & AO_CAST_CHNID_MASK));

    return HI_SUCCESS;
}

hi_s32 ao_cast_destory(hi_handle h_cast)
{
    hi_s32 ret;
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);

    CHECK_AO_NULL_PTR(card);

    ret = cast_destroy_chn(card, h_cast);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_destroy_chn, ret);
        return ret;
    }

    card->snd_cast_init_flag &= ~((hi_u32)1L << (h_cast & AO_CAST_CHNID_MASK));

    return HI_SUCCESS;
}

hi_s32 ao_cast_set_info(hi_handle h_cast, hi_u64 user_virt_addr)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_set_info(card, h_cast, user_virt_addr);
}

hi_s32 ao_cast_get_info(hi_handle h_cast, ao_cast_info_param *info)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_get_info(card, h_cast, info);
}

hi_s32 ao_cast_set_enable(hi_handle h_cast, hi_bool enable)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_set_enable(card, h_cast, enable);
}

hi_s32 ao_cast_get_enable(hi_handle h_cast, hi_bool *enable)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_get_enable(card, h_cast, enable);
}

static hi_s32 ao_cast_read_data(hi_handle h_cast, ao_frame *frame)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_read_data(card, h_cast, frame);
}

static hi_s32 ao_cast_relese_data(hi_handle h_cast, ao_frame *frame)
{
    snd_card_state *card = cast_card_get_card(h_cast & AO_CAST_CHNID_MASK);
    CHECK_AO_NULL_PTR(card);

    return cast_release_data(card, h_cast);
}

hi_s32 ao_ioctl_snd_set_all_cast_mute(hi_void *file, hi_void *arg)
{
    ao_snd_all_cast_mute_param_p all_cast_mute = (ao_snd_all_cast_mute_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(all_cast_mute->sound);
    return ao_snd_set_all_cast_mute(all_cast_mute->sound, all_cast_mute->mute);
}

hi_s32 ao_ioctl_snd_get_all_cast_mute(hi_void *file, hi_void *arg)
{
    ao_snd_all_cast_mute_param_p all_cast_mute = (ao_snd_all_cast_mute_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(all_cast_mute->sound);
    return ao_snd_get_all_cast_mute(all_cast_mute->sound, &all_cast_mute->mute);
}

hi_s32 ao_ioctl_cast_get_def_attr(hi_void *file, hi_void *arg)
{
    return ao_cast_get_def_attr((hi_ao_cast_attr *)arg);
}

hi_s32 ao_ioctl_cast_create(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    hi_handle h_cast;
    ao_cast_create_param_p cast_attr = (ao_cast_create_param_p)arg;
    ao_mgmt *mgmt = ao_get_mgmt();

    CHECK_AO_SNDCARD_OPEN(cast_attr->sound);

    ret = ao_cast_alloc_handle(&h_cast, file, &cast_attr->cast_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_cast_alloc_handle, ret);
        return ret;
    }

    ret = ao_cast_create(cast_attr->sound,
                         &cast_attr->cast_attr,
                         &mgmt->cast_entity[h_cast & AO_CAST_CHNID_MASK].rbf_mmz,
                         h_cast);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_cast_create, ret);
        ao_cast_free_handle(h_cast);
        return ret;
    }

    ao_cast_save_suspend_attr(cast_attr->sound, h_cast, &cast_attr->cast_attr);
    cast_attr->h_cast = h_cast;

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_cast_destory(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    hi_handle h_cast = *(hi_handle *)arg;

    CHECK_AO_CAST_OPEN(h_cast);

    ret = ao_cast_destory(h_cast);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_cast_destory, ret);
        return ret;
    }

    ao_cast_free_handle(h_cast);

    return ret;
}

hi_s32 ao_ioctl_cast_set_info(hi_void *file, hi_void *arg)
{
    ao_cast_info_param_p info = (ao_cast_info_param_p)arg;
    CHECK_AO_CAST_OPEN(info->h_cast);
    return ao_cast_set_info(info->h_cast, info->user_virt_addr);
}

hi_s32 ao_ioctl_cast_get_info(hi_void *file, hi_void *arg)
{
    ao_cast_info_param_p info = (ao_cast_info_param_p)arg;
    CHECK_AO_CAST_OPEN(info->h_cast);
    return ao_cast_get_info(info->h_cast, info);
}

hi_s32 ao_ioctl_cast_set_enable(hi_void *file, hi_void *arg)
{
    ao_cast_enable_param_p enable = (ao_cast_enable_param_p)arg;
    CHECK_AO_CAST_OPEN(enable->h_cast);
    return ao_cast_set_enable(enable->h_cast, enable->cast_enable);
}

hi_s32 ao_ioctl_cast_get_enable(hi_void *file, hi_void *arg)
{
    ao_cast_enable_param_p enable = (ao_cast_enable_param_p)arg;
    CHECK_AO_CAST_OPEN(enable->h_cast);
    return ao_cast_get_enable(enable->h_cast, &enable->cast_enable);
}

hi_s32 ao_ioctl_cast_acquire_frame(hi_void *file, hi_void *arg)
{
    ao_cast_data_param_p cast_data = (ao_cast_data_param_p)arg;
    CHECK_AO_CAST_OPEN(cast_data->h_cast);
    return ao_cast_read_data(cast_data->h_cast, &cast_data->ao_frame);
}

hi_s32 ao_ioctl_cast_release_frame(hi_void *file, hi_void *arg)
{
    ao_cast_data_param_p cast_data = (ao_cast_data_param_p)arg;
    CHECK_AO_CAST_OPEN(cast_data->h_cast);
    return ao_cast_relese_data(cast_data->h_cast, &cast_data->ao_frame);
}

hi_s32 ao_ioctl_cast_set_abs_gain(hi_void *file, hi_void *arg)
{
    ao_cast_abs_gain_param_p abs_gain = (ao_cast_abs_gain_param_p)arg;
    CHECK_AO_CAST_OPEN(abs_gain->h_cast);
    return ao_cast_set_abs_gain(abs_gain->h_cast, &abs_gain->cast_abs_gain);
}

hi_s32 ao_ioctl_cast_get_abs_gain(hi_void *file, hi_void *arg)
{
    ao_cast_abs_gain_param_p abs_gain = (ao_cast_abs_gain_param_p)arg;
    CHECK_AO_CAST_OPEN(abs_gain->h_cast);
    return ao_cast_get_abs_gain(abs_gain->h_cast, &abs_gain->cast_abs_gain);
}

hi_s32 ao_ioctl_cast_set_mute(hi_void *file, hi_void *arg)
{
    ao_cast_mute_param_p mute = (ao_cast_mute_param_p)arg;
    CHECK_AO_CAST_OPEN(mute->h_cast);
    return ao_cast_set_mute(mute->h_cast, mute->mute);
}

hi_s32 ao_ioctl_cast_get_mute(hi_void *file, hi_void *arg)
{
    ao_cast_mute_param_p mute = (ao_cast_mute_param_p)arg;
    CHECK_AO_CAST_OPEN(mute->h_cast);
    return ao_cast_get_mute(mute->h_cast, &mute->mute);
}

hi_s32 ao_cast_get_settings(hi_handle h_cast, snd_cast_settings *cast_settings)
{
    snd_card_state *card = HI_NULL;

    h_cast &= AO_CAST_CHNID_MASK;
    card = cast_card_get_card(h_cast);
    CHECK_AO_NULL_PTR(card);

    cast_get_settings(card, h_cast, cast_settings);
    return HI_SUCCESS;
}

hi_s32 ao_cast_restore_settings(hi_handle h_cast, snd_cast_settings *cast_settings)
{
    snd_card_state *card = HI_NULL;

    h_cast &= AO_CAST_CHNID_MASK;
    card = cast_card_get_card(h_cast);
    CHECK_AO_NULL_PTR(card);

    cast_restore_settings(card, h_cast, cast_settings);
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

