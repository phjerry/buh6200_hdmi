/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv arc implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "arc_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_s32 ao_snd_set_arc_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(card);

    if (arc_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return arc_op_set_enable(card, snd_op, enable);
}

static hi_s32 ao_snd_get_arc_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(enable);
    CHECK_AO_NULL_PTR(card);

    if (arc_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return arc_op_get_enable(card, snd_op, enable);
}

static hi_s32 ao_snd_set_arc_cap(ao_snd_id sound, hi_ao_port out_port, hi_ao_arc_audio_cap *cap)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(cap);
    CHECK_AO_NULL_PTR(card);

    if (arc_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return arc_op_set_cap(card, snd_op, cap);
}

static hi_s32 ao_snd_get_arc_cap(ao_snd_id sound, hi_ao_port out_port, hi_ao_arc_audio_cap *cap)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(cap);
    CHECK_AO_NULL_PTR(card);

    if (arc_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return arc_op_get_cap(card, snd_op, cap);
}

hi_s32 ao_ioctl_snd_set_arc_enable(hi_void *file, hi_void *arg)
{
    ao_snd_arc_enable_param_p arc_enable = (ao_snd_arc_enable_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(arc_enable->sound);
    return ao_snd_set_arc_enable(arc_enable->sound, arc_enable->out_port, arc_enable->enable);
}

hi_s32 ao_ioctl_snd_get_arc_enable(hi_void *file, hi_void *arg)
{
    ao_snd_arc_enable_param_p arc_enable = (ao_snd_arc_enable_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(arc_enable->sound);
    return ao_snd_get_arc_enable(arc_enable->sound, arc_enable->out_port, &arc_enable->enable);
}

hi_s32 ao_ioctl_snd_set_arc_cap(hi_void *file, hi_void *arg)
{
    ao_snd_arc_cap_param_p arc_cap = (ao_snd_arc_cap_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(arc_cap->sound);
    return ao_snd_set_arc_cap(arc_cap->sound, arc_cap->out_port, &arc_cap->cap);
}

hi_s32 ao_ioctl_snd_get_arc_cap(hi_void *file, hi_void *arg)
{
    ao_snd_arc_cap_param_p arc_cap = (ao_snd_arc_cap_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(arc_cap->sound);
    return ao_snd_get_arc_cap(arc_cap->sound, arc_cap->out_port, &arc_cap->cap);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
