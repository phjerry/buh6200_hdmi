/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: implement of ao cast driver
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "hi_osal.h"
#include "hi_drv_sys.h"

#include "audio_util.h"
#include "drv_ao_engine.h"
#include "cast_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* always use this to get cast from card */
#define CAST_FIND_BY_HANDLE(card, h_cast, cast)           \
    do {                                                  \
        (cast) = __cast_find_by_handle((card), (h_cast)); \
        if ((cast) == HI_NULL) {                          \
            HI_LOG_ERR("__cast_find_by_handle failed\n");        \
            HI_ERR_PRINT_H32((h_cast));                   \
            return HI_ERR_AO_INVALID_PARA;                \
        }                                                 \
    } while (0)

/* always use this to get cast from card */
static snd_cast_state *__cast_find_by_handle(snd_card_state *card, hi_handle h_cast)
{
    snd_cast_state *cast = HI_NULL;

    osal_list_for_each_entry(cast, &card->cast, node) {
        if ((cast->h_cast & AO_CAST_CHNID_MASK) == (h_cast & AO_CAST_CHNID_MASK)) {
            return cast;
        }
    }

    return HI_NULL;
}

static hi_s32 cast_enable(snd_card_state *card, snd_cast_state *cast, hi_bool enable)
{
    aoe_aop_id aop = cast->cast_op.aop[cast->cast_op.active_id];
    aoe_engine_id engine;

    engine = ao_engine_get_id_by_type(card, PCM);
    if (engine == AOE_ENGINE_MAX) {
        cast->status = SND_CAST_STATUS_STOP;
        HI_WARN_PRINT_H32(cast->h_cast);
        HI_WARN_PRINT_BOOL(enable);
        return HI_SUCCESS;
    }

    if (enable == HI_TRUE) {
        hal_aoe_engine_attach_aop(engine, aop);
        cast_op_start(&cast->cast_op);
        cast->status = SND_CAST_STATUS_START;
    } else {
        cast_op_stop(&cast->cast_op);
        hal_aoe_engine_detach_aop(engine, aop);
        cast->status = SND_CAST_STATUS_STOP;
    }

    return HI_SUCCESS;
}

hi_s32 cast_set_all_mute(snd_card_state *card, hi_bool mute)
{
    hi_s32 ret;
    snd_cast_state *cast = HI_NULL;

    osal_list_for_each_entry(cast, &card->cast, node) {
        ret = hal_aoe_aop_set_mute(cast->cast_op.aop[cast->cast_op.active_id],
            !((cast->mute == HI_FALSE) && (mute == HI_FALSE)));
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_mute, ret);
            HI_ERR_PRINT_H32(cast->h_cast);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 cast_set_mute(snd_card_state *card, hi_handle h_cast, hi_bool mute)
{
    hi_s32 ret;
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    ret = hal_aoe_aop_set_mute(cast->cast_op.aop[cast->cast_op.active_id], mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_mute, ret);
        return ret;
    }

    cast->mute = mute;

    return HI_SUCCESS;
}

hi_s32 cast_get_mute(snd_card_state *card, hi_handle h_cast, hi_bool *mute)
{
    snd_cast_state *cast = HI_NULL;

    CHECK_AO_NULL_PTR(mute);
    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    *mute = cast->mute;
    return HI_SUCCESS;
}

hi_s32 cast_set_abs_gain(snd_card_state *card, hi_handle h_cast, hi_ao_abs_gain *cast_abs_gain)
{
    hi_s32 ret;
    snd_cast_state *cast = HI_NULL;
    hi_u32 l_volume_db_reg;
    hi_u32 r_volume_db_reg;

    CHECK_AO_NULL_PTR(cast_abs_gain);

    if (cast_abs_gain->linear_mode == HI_TRUE) {
        CHECK_AO_LINEARVOLUME(cast_abs_gain->gain_l);
        CHECK_AO_LINEARVOLUME(cast_abs_gain->gain_r);

        l_volume_db_reg = autil_volume_linear_to_reg_db((hi_u32)cast_abs_gain->gain_l);
        r_volume_db_reg = autil_volume_linear_to_reg_db((hi_u32)cast_abs_gain->gain_r);
    } else {
        CHECK_AO_ABSLUTEVOLUMEEXT(cast_abs_gain->gain_l);
        CHECK_AO_ABSLUTEVOLUMEEXT(cast_abs_gain->gain_r);

        l_volume_db_reg = autil_volume_db_to_reg_db(cast_abs_gain->gain_l);
        r_volume_db_reg = autil_volume_db_to_reg_db(cast_abs_gain->gain_r);
    }

    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    ret = hal_aoe_aop_set_volume(cast->cast_op.aop[cast->cast_op.active_id], l_volume_db_reg, r_volume_db_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_volume, ret);
        return ret;
    }

    cast->cast_abs_gain.linear_mode = cast_abs_gain->linear_mode;
    cast->cast_abs_gain.gain_l = cast_abs_gain->gain_l;
    cast->cast_abs_gain.gain_r = cast_abs_gain->gain_r;

    return HI_SUCCESS;
}

hi_s32 cast_get_abs_gain(snd_card_state *card, hi_handle h_cast, hi_ao_abs_gain *cast_abs_gain)
{
    snd_cast_state *cast = HI_NULL;

    CHECK_AO_NULL_PTR(cast_abs_gain);
    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    cast_abs_gain->linear_mode = cast->cast_abs_gain.linear_mode;
    cast_abs_gain->gain_l = cast->cast_abs_gain.gain_l;
    cast_abs_gain->gain_r = cast->cast_abs_gain.gain_r;
    return HI_SUCCESS;
}

hi_s32 cast_set_aef_bypass(snd_card_state *card, hi_handle h_cast, hi_bool bypass)
{
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    return hal_aoe_aop_set_aef_bypass(cast->cast_op.aop[cast->cast_op.active_id], bypass);
}

#ifdef HI_PROC_SUPPORT
static const hi_char *cast_status_to_name(snd_cast_status status)
{
    const hi_char *name[] = {
        [SND_CAST_STATUS_STOP]  = "stop",
        [SND_CAST_STATUS_START] = "start",
        [SND_CAST_STATUS_PAUSE] = "pause",
    };

    if (status < SND_CAST_STATUS_MAX) {
        return name[status];
    }

    return "invalid";
}

hi_void cast_read_proc(hi_void *p, snd_card_state *card)
{
    snd_cast_state *cast = HI_NULL;

    osal_list_for_each_entry(cast, &card->cast, node) {
        osal_proc_print(p, "cast(%d): *aop(0x%x), status(%s), user_enable(%s), weight(%.3d/%.3d%s), mute(%s)",
            cast->h_cast & AO_CAST_CHNID_MASK, snd_get_op_aop_id(&cast->cast_op), cast_status_to_name(cast->status),
            TRUE_ON_FALSE_OFF(cast->user_enable_setting), cast->cast_abs_gain.gain_l, cast->cast_abs_gain.gain_r,
            (cast->cast_abs_gain.linear_mode == HI_TRUE) ? "" : "dB", TRUE_ON_FALSE_OFF(cast->mute));
        if (card->cast_simulate_op == HI_TRUE) {
            osal_proc_print(p, ", *cast_simulated_op(%s)\n", autil_port_to_name(card->cast_simulate_port));
        } else {
            osal_proc_print(p, "\n");
        }
        osal_proc_print(p, "         sample_rate(%.6d), channel(%.2d), bit_width(%2d)\n", cast->ref_frame.sample_rate,
            cast->ref_frame.channels, cast->ref_frame.bit_depth);
        osal_proc_print(p, "         max_frame_num(%.2d), sample_per_frame(%.5d)\n",
            cast->user_cast_attr.pcm_frame_max_num, cast->user_cast_attr.pcm_samples);
    }
}
#endif

hi_s32 cast_get_def_attr(hi_ao_cast_attr *attr)
{
    attr->add_mute = HI_FALSE;
    attr->pcm_frame_max_num = AO_CAST_DEFATTR_FRAMEMAXNUM;
    attr->pcm_samples = AO_CAST_DEFATTR_SAMPLESPERFRAME;
    attr->sample_rate = HI_SAMPLE_RATE_48K;
    attr->bit_depth = HI_BIT_DEPTH_16;
    attr->channels = HI_AUDIO_CH_STEREO;
    return HI_SUCCESS;
}

static inline hi_void cast_build_reference_frame(hi_ao_cast_attr *cast_attr, ao_frame *frame)
{
    frame->bit_depth = cast_attr->bit_depth;
    frame->interleaved = HI_TRUE;
    frame->sample_rate = cast_attr->sample_rate;
    frame->channels = cast_attr->channels;
    frame->pcm_samples = cast_attr->pcm_samples;
}

static snd_cast_state *cast_malloc_chn(hi_handle handle)
{
    hi_s32 ret;
    snd_cast_state *cast = HI_NULL;

    cast = (snd_cast_state *)osal_kmalloc(HI_ID_AO, sizeof(snd_cast_state), OSAL_GFP_KERNEL);
    if (cast == HI_NULL) {
        HI_LOG_FATAL("malloc snd_cast_state failed\n");
        return HI_NULL;
    }

    ret = memset_s(cast, sizeof(*cast), 0, sizeof(snd_cast_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        osal_kfree(HI_ID_AO, cast);
        return HI_NULL;
    }

    cast->h_cast = handle;

    return cast;
}

static hi_s32 cast_init_attr(snd_cast_state *cast, hi_ao_cast_attr *cast_attr, hi_audio_buffer *buf)
{
    hi_s32 ret;

    ret = memcpy_s(&cast->user_cast_attr, sizeof(cast->user_cast_attr), cast_attr, sizeof(*cast_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    cast_build_reference_frame(cast_attr, &cast->ref_frame);

    /*
     * when create cast in kernel, we pointer cast reference ao frame to
     * the start of cast aop buffer offset as default
     */
    cast->ref_frame.pcm_buffer = buf->virt_addr - (hi_u8 *)HI_NULL;
    ret = memcpy_s(&cast->map_buffer, sizeof(cast->map_buffer), buf, sizeof(*buf));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    cast->frame_bytes =
        cast->ref_frame.pcm_samples * autil_calc_frame_size(cast->ref_frame.channels, cast->ref_frame.bit_depth);

    cast->status = SND_CAST_STATUS_STOP;
    cast->cast_abs_gain.linear_mode = HI_TRUE;
    cast->cast_abs_gain.gain_l = AO_MAX_LINEARVOLUME;
    cast->cast_abs_gain.gain_r = AO_MAX_LINEARVOLUME;

    return HI_SUCCESS;
}

hi_s32 cast_create_chn(snd_card_state *card, hi_ao_cast_attr *cast_attr, hi_audio_buffer *mmz, hi_handle h_cast)
{
    hi_s32 ret;
    snd_cast_state *cast = cast_malloc_chn(h_cast);
    if (cast == HI_NULL) {
        HI_LOG_FATAL("call cast_malloc_chn failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = cast_init_attr(cast, cast_attr, mmz);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_init_attr, ret);
        goto out;
    }

    ret = cast_op_create(card, cast_attr, mmz, &cast->cast_op);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_op_create, ret);
        goto out;
    }

    osal_list_add(&cast->node, &card->cast);

    return HI_SUCCESS;

out:
    osal_kfree(HI_ID_AO, cast);
    return ret;
}

hi_s32 cast_destroy_chn(snd_card_state *card, hi_handle h_cast)
{
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    if (cast->status != SND_CAST_STATUS_STOP) {
        cast_enable(card, cast, HI_FALSE);
    }

    cast_op_destroy(&cast->cast_op, HI_FALSE);

    osal_list_del(&cast->node);
    osal_kfree(HI_ID_AO, cast);

    return HI_SUCCESS;
}

hi_s32 cast_set_info(snd_card_state *card, hi_handle h_cast, hi_u64 user_virt_addr)
{
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);
    cast->user_virt_addr = user_virt_addr;

    /*
     * when create cast in user space, we pointer cast reference ao frame to
     * the start of cast aop buffer mapped user virtual address
     */
    cast->ref_frame.pcm_buffer = user_virt_addr;

    return HI_SUCCESS;
}

hi_s32 cast_get_info(snd_card_state *card, hi_handle h_cast, ao_cast_info_param *info)
{
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    info->user_virt_addr = cast->user_virt_addr;
    info->kernel_virt_addr = cast->map_buffer.virt_addr - (hi_u8 *)HI_NULL;
    info->phy_addr = cast->map_buffer.phys_addr;
    info->buf_size = cast->map_buffer.size;
    info->map_fd = cast->map_buffer.fd;
    info->frame_bytes = cast->frame_bytes;

    info->frame_samples = cast->ref_frame.pcm_samples;
    info->channels = cast->ref_frame.channels;
    info->bit_per_sample = cast->ref_frame.bit_depth;

    return HI_SUCCESS;
}

hi_void cast_get_settings(snd_card_state *card, hi_u32 cast_id, snd_cast_settings *cast_settings)
{
    hi_s32 ret;
    snd_cast_state *cast = __cast_find_by_handle(card, cast_id);
    if (cast == HI_NULL) {
        HI_LOG_ERR("__cast_find_by_handle failed\n");
        HI_ERR_PRINT_H32(cast_id);
        return;
    }

    ret = memcpy_s(&cast_settings->cast_abs_gain, sizeof(cast_settings->cast_abs_gain), &cast->cast_abs_gain,
        sizeof(cast->cast_abs_gain));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return;
    }

    cast_settings->mute = cast->mute;
    cast_settings->user_virt_addr = cast->user_virt_addr;
    cast_settings->user_enable_setting = cast->user_enable_setting;
}

hi_void cast_restore_settings(snd_card_state *card, hi_u32 cast_id, snd_cast_settings *cast_settings)
{
    hi_s32 ret;
    snd_cast_state *cast = __cast_find_by_handle(card, cast_id);
    if (cast == HI_NULL) {
        HI_LOG_ERR("__cast_find_by_handle failed\n");
        HI_ERR_PRINT_H32(cast_id);
        return;
    }

    cast->user_virt_addr = cast_settings->user_virt_addr;

    /* fource discard release_cast_frame after resume */
    cast->acquire_cast_frame_flag = HI_FALSE;

    if (cast_settings->user_enable_setting == cast->user_enable_setting) {
        return;
    }

    ret = cast_set_enable(card, cast_id, cast_settings->user_enable_setting);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_set_enable, ret);
        return;
    }

    ret = cast_set_abs_gain(card, cast_id, &cast_settings->cast_abs_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_set_abs_gain, ret);
        return;
    }

    ret = cast_set_mute(card, cast_id, cast_settings->mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_set_mute, ret);
        return;
    }
}

hi_s32 cast_set_enable(snd_card_state *card, hi_handle h_cast, hi_bool enable)
{
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);
    cast->user_enable_setting = enable;

    return cast_enable(card, cast, enable);
}

hi_s32 cast_get_enable(snd_card_state *card, hi_handle h_cast, hi_bool *enable)
{
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);
    *enable = cast->user_enable_setting;
    return HI_SUCCESS;
}

hi_s32 cast_read_data(snd_card_state *card, hi_handle h_cast, ao_frame *frame)
{
    hi_s32 ret;
    hi_u32 read_bytes;
    snd_cast_state *cast = HI_NULL;
    hi_u32 offset = 0;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    if (cast->user_enable_setting == HI_FALSE) {
        return HI_ERR_AO_NOTSUPPORT;
    }

    if (cast->status == SND_CAST_STATUS_STOP) {
        HI_LOG_WARN("cast is stop, try to enable cast\n");
        HI_WARN_PRINT_H32(h_cast);
        ret = cast_enable(card, cast, HI_TRUE);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(cast_enable, ret);
            return ret;
        }
    }

    if (cast->status == SND_CAST_STATUS_STOP) {
        return HI_SUCCESS;
    }

    ret = memcpy_s(frame, sizeof(*frame), &cast->ref_frame, sizeof(ao_frame));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    read_bytes = hal_cast_read_data(cast->cast_op.cast_id, &offset, cast->frame_bytes);
    if (read_bytes == 0) {
        frame->pcm_samples = 0;
        return HI_SUCCESS;
    }

    /*
     * @pcm_buffer pointer to current cast frame now,
     * Be careful with the value stored in frame->pcm_buffer,
     *
     * Before cast_set_info is called frame->pcm_buffer is a kernel virtual address,
     * After cast_set_info is called frame->pcm_buffer is a user virtual address.
     */
    frame->pcm_buffer += offset;
    frame->pts = hi_drv_sys_get_time_stamp_ms();
    cast->acquire_cast_frame_flag = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 cast_release_data(snd_card_state *card, hi_handle h_cast)
{
    snd_cast_state *cast = HI_NULL;

    CAST_FIND_BY_HANDLE(card, h_cast, cast);

    /* discard release_cast_frame before call acquire_cast_frame */
    if (cast->acquire_cast_frame_flag == HI_FALSE) {
        return HI_SUCCESS;
    }

    if (hal_cast_release_data(cast->cast_op.cast_id, cast->frame_bytes) == 0) {
        return HI_FAILURE;
    }

    cast->acquire_cast_frame_flag = HI_FALSE;

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end__cplusplus */
