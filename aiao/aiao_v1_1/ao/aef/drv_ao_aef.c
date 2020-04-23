/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv aef implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

/* drv headers */
#include "drv_ao_aef.h"
#include "aef_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_s32 ao_snd_attach_aef(ao_snd_id sound, hi_u32 aef_id, ao_aef_attr *aef_attr, hi_u32 *aef_proc_addr)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(aef_proc_addr);
    CHECK_AO_NULL_PTR(card);

    return aef_attach_snd(card, aef_id, aef_attr, aef_proc_addr);
}

static hi_s32 ao_snd_detach_aef(ao_snd_id sound, hi_u32 aef_id, ao_aef_attr *aef_attr)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    return aef_detach_snd(card, aef_id, aef_attr);
}

static hi_s32 ao_snd_set_aef_bypass(ao_snd_id sound, hi_ao_port out_port, hi_bool bypass)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    return snd_set_op_aef_bypass(card, out_port, bypass);
}

static hi_s32 ao_snd_get_aef_bypass(ao_snd_id sound, hi_ao_port out_port, hi_bool *bypass)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(bypass);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_aef_bypass(card, out_port, bypass);
}

static hi_s32 ao_snd_get_aef_buf_attr(ao_snd_id sound, ao_aef_buf_attr *aef_buf)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(aef_buf);
    CHECK_AO_NULL_PTR(card);

    return aef_get_buf_attr(card, aef_buf);
}

static hi_s32 ao_snd_get_debug_attr(ao_snd_id sound, ao_debug_attr *debug_attr)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(debug_attr);
    CHECK_AO_NULL_PTR(card);

    switch (debug_attr->debug_type) {
        case AO_SND_DEBUG_TYPE_AEF:
            aef_get_debug_addr(card, &(debug_attr->un_debug_attr.debug_attr));
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_snd_attach_aef(hi_void *file, hi_void *arg)
{
    ao_snd_att_aef_param_p snd_att_aef = (ao_snd_att_aef_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(snd_att_aef->sound);
    return ao_snd_attach_aef(snd_att_aef->sound, snd_att_aef->aef_id,
        &snd_att_aef->aef_attr, &snd_att_aef->aef_proc_addr);
}

hi_s32 ao_ioctl_snd_detach_aef(hi_void *file, hi_void *arg)
{
    ao_snd_att_aef_param_p snd_att_aef = (ao_snd_att_aef_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(snd_att_aef->sound);
    return ao_snd_detach_aef(snd_att_aef->sound, snd_att_aef->aef_id, &snd_att_aef->aef_attr);
}

hi_s32 ao_ioctl_snd_set_aef_bypass(hi_void *file, hi_void *arg)
{
    ao_snd_aef_bypass_param_p aef_bypass = (ao_snd_aef_bypass_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(aef_bypass->sound);
    return ao_snd_set_aef_bypass(aef_bypass->sound, aef_bypass->out_port, aef_bypass->bypass);
}

hi_s32 ao_ioctl_snd_get_aef_bypass(hi_void *file, hi_void *arg)
{
    ao_snd_aef_bypass_param_p aef_bypass = (ao_snd_aef_bypass_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(aef_bypass->sound);
    return ao_snd_get_aef_bypass(aef_bypass->sound, aef_bypass->out_port, &aef_bypass->bypass);
}

hi_s32 ao_ioctl_snd_get_aef_buf_attr(hi_void *file, hi_void *arg)
{
    ao_snd_aef_buf_param_p aef_buf = (ao_snd_aef_buf_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(aef_buf->sound);
    return ao_snd_get_aef_buf_attr(aef_buf->sound, &aef_buf->aef_buf);
}

hi_s32 ao_ioctl_snd_get_debug_attr(hi_void *file, hi_void *arg)
{
    ao_snd_debug_param_p debug = (ao_snd_debug_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(debug->sound);
    return ao_snd_get_debug_attr(debug->sound, &debug->debug_attr);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
