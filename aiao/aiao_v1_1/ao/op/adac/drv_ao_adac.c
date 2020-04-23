/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao adac implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "drv_ao_adac.h"
#include "adac_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 ao_snd_set_adac_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool enable)
{
    snd_op_state *snd_op = HI_NULL;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    if (adac_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    snd_set_adac_enable(snd_op, enable);
    card->adac_enable = enable;

    return HI_SUCCESS;
}

hi_s32 ao_snd_get_adac_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool *enable)
{
    hi_s32 ret;
    snd_op_state *snd_op = HI_NULL;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    if (adac_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    ret = snd_get_adac_enable(snd_op, enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_get_adac_enable, ret);
    }

    return ret;
}

hi_s32 ao_ioctl_snd_set_adac_enable(hi_void *file, hi_void *arg)
{
    ao_snd_adac_enable_param_p adac_enable = (ao_snd_adac_enable_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(adac_enable->sound);
    return ao_snd_set_adac_enable(adac_enable->sound, HI_AO_PORT_DAC0, adac_enable->enable);
}

hi_s32 ao_ioctl_snd_get_adac_enable(hi_void *file, hi_void *arg)
{
    ao_snd_adac_enable_param_p adac_enable = (ao_snd_adac_enable_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(adac_enable->sound);
    return ao_snd_get_adac_enable(adac_enable->sound, HI_AO_PORT_DAC0, &adac_enable->enable);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

