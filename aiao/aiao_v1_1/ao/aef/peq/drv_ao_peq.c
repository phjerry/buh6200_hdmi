/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao_v1_1 drv peq implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "drv_ao_peq.h"
#include "peq_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 ao_check_peq_band_num(hi_u32 band_num)
{
    if ((band_num < PEQ_BANDNUM_MIN) ||
        (band_num > PEQ_BANDNUM_MAX)) {
        HI_LOG_ERR("invalid peq band_num!\n");
        HI_ERR_PRINT_U32(band_num);
        return HI_ERR_AO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 ao_check_peq_band_attr(hi_ao_peq_band_attr *eq_param)
{
    hi_u32 qmin = 0;
    hi_u32 qmax = 0;
    hi_u32 freqmin = 0;
    hi_u32 freqmax = 0;

    switch (eq_param->type) {
        case HI_AO_PEQ_FILTER_TYPE_HP:
            qmin = PEQ_Q_HP_MIN;
            qmax = PEQ_Q_HP_MAX;
            freqmin = PEQ_FREQ_HP_MIN;
            freqmax = PEQ_FREQ_HP_MAX;
            break;

        case HI_AO_PEQ_FILTER_TYPE_LP:
            qmin = PEQ_Q_LP_MIN;
            qmax = PEQ_Q_LP_MAX;
            freqmin = PEQ_FREQ_LP_MIN;
            freqmax = PEQ_FREQ_LP_MAX;
            break;

        case HI_AO_PEQ_FILTER_TYPE_LS:
            qmin = PEQ_Q_LS_MIN;
            qmax = PEQ_Q_LS_MAX;
            freqmin = PEQ_FREQ_LS_MIN;
            freqmax = PEQ_FREQ_LS_MAX;
            break;

        case HI_AO_PEQ_FILTER_TYPE_HS:
            qmin = PEQ_Q_HS_MIN;
            qmax = PEQ_Q_HS_MAX;
            freqmin = PEQ_FREQ_HS_MIN;
            freqmax = PEQ_FREQ_HS_MAX;
            break;

        case HI_AO_PEQ_FILTER_TYPE_PK:
            qmin = PEQ_Q_PK_MIN;
            qmax = PEQ_Q_PK_MAX;
            freqmin = PEQ_FREQ_PK_MIN;
            freqmax = PEQ_FREQ_PK_MAX;
            break;

        default:
            HI_LOG_ERR("invalid filter type\n");
            HI_ERR_PRINT_U32(eq_param->type);
            return HI_ERR_AO_INVALID_PARA;
    }

    AO_CHECK_PEQQ(eq_param->q, qmin, qmax);
    AO_CHECK_PEQFREQ(eq_param->freq, freqmin, freqmax);
    AO_CHECK_PEQGAIN(eq_param->gain, PEQ_GAIN_MIN, PEQ_GAIN_MAX);

    return HI_SUCCESS;
}

hi_s32 ao_check_peq_attr(hi_ao_peq_attr *peq_attr)
{
    hi_s32 ret;
    hi_u32 band_id;

    ret = ao_check_peq_band_num(peq_attr->band_num);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_check_peq_band_num, ret);
        return ret;
    }

    for (band_id = 0; band_id < peq_attr->band_num; band_id++) {
        ret = ao_check_peq_band_attr(&peq_attr->param[band_id]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_check_peq_band_attr, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 ao_snd_set_peq_attr(ao_snd_id sound, hi_ao_port out_port, hi_ao_peq_attr *peq_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(peq_attr);
    CHECK_AO_NULL_PTR(card);

    ret = ao_check_peq_attr(peq_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_check_peq_attr, ret);
        return ret;
    }

    return snd_set_op_peq_attr(card, out_port, peq_attr);
}

hi_s32 ao_snd_get_peq_attr(ao_snd_id sound, hi_ao_port out_port, hi_ao_peq_attr *peq_attr)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(peq_attr);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_peq_attr(card, out_port, peq_attr);
}

hi_s32 ao_snd_set_peq_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    return snd_set_op_peq_enable(card, out_port, enable);
}

hi_s32 ao_snd_get_peq_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    return snd_get_op_peq_enable(card, out_port, enable);
}

hi_s32 ao_ioctl_snd_set_peq_attr(hi_void *file, hi_void *arg)
{
    ao_snd_peq_param_p peq_attr = (ao_snd_peq_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(peq_attr->sound);
    return ao_snd_set_peq_attr(peq_attr->sound, peq_attr->out_port, &peq_attr->eq_attr);
}

hi_s32 ao_ioctl_snd_get_peq_attr(hi_void *file, hi_void *arg)
{
    ao_snd_peq_param_p peq_attr = (ao_snd_peq_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(peq_attr->sound);
    return ao_snd_get_peq_attr(peq_attr->sound, peq_attr->out_port, &peq_attr->eq_attr);
}

hi_s32 ao_ioctl_snd_set_peq_enable(hi_void *file, hi_void *arg)
{
    ao_snd_eq_enable_p peq_eanble = (ao_snd_eq_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(peq_eanble->sound);
    return ao_snd_set_peq_enable(peq_eanble->sound, peq_eanble->out_port, peq_eanble->eq_enable);
}

hi_s32 ao_ioctl_snd_get_peq_enable(hi_void *file, hi_void *arg)
{
    ao_snd_eq_enable_p peq_eanble = (ao_snd_eq_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(peq_eanble->sound);
    return ao_snd_get_peq_enable(peq_eanble->sound, peq_eanble->out_port, &peq_eanble->eq_enable);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
