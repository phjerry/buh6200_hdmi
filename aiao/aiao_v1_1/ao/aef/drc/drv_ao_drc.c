/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao_v1_1 drv drc implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

/* drv headers */
#include "drv_ao_drc.h"
#include "drc_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_s32 ao_snd_set_drc_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    return snd_set_op_drc_enable(card, out_port, enable);
}

static hi_s32 ao_snd_get_drc_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    return snd_get_op_drc_enable(card, out_port, enable);
}

static hi_s32 ao_snd_set_drc_attr(ao_snd_id sound, hi_ao_port out_port, hi_ao_drc_attr *drc_attr)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(drc_attr);
    CHECK_AO_NULL_PTR(card);
    CHECK_AO_DRCATTR(drc_attr);

    return snd_set_op_drc_attr(card, out_port, drc_attr);
}

static hi_s32 ao_snd_get_drc_attr(ao_snd_id sound, hi_ao_port out_port, hi_ao_drc_attr *drc_attr)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(drc_attr);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_drc_attr(card, out_port, drc_attr);
}

hi_s32 ao_ioctl_snd_set_drc_enable(hi_void *file, hi_void *arg)
{
    ao_snd_drc_enable_p drc_eanble = (ao_snd_drc_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(drc_eanble->sound);
    return ao_snd_set_drc_enable(drc_eanble->sound, drc_eanble->out_port, drc_eanble->drc_enable);
}

hi_s32 ao_ioctl_snd_get_drc_enable(hi_void *file, hi_void *arg)
{
    ao_snd_drc_enable_p drc_eanble = (ao_snd_drc_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(drc_eanble->sound);
    return ao_snd_get_drc_enable(drc_eanble->sound, drc_eanble->out_port, &drc_eanble->drc_enable);
}

hi_s32 ao_ioctl_snd_set_drc_attr(hi_void *file, hi_void *arg)
{
    ao_snd_drc_param_p drc_attr = (ao_snd_drc_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(drc_attr->sound);
    return ao_snd_set_drc_attr(drc_attr->sound, drc_attr->out_port, &drc_attr->drc_attr);
}

hi_s32 ao_ioctl_snd_get_drc_attr(hi_void *file, hi_void *arg)
{
    ao_snd_drc_param_p drc_attr = (ao_snd_drc_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(drc_attr->sound);
    return ao_snd_get_drc_attr(drc_attr->sound, drc_attr->out_port, &drc_attr->drc_attr);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
