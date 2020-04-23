/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: geq implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "drv_ao_geq.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AO_EQ_BAND_NUM_MIN 5
#define AO_EQ_BAND_NUM_MAX 10

hi_s32 ao_get_def_geq_attr(hi_ao_geq_attr *geq_attr)
{
    hi_s32 ret;
    hi_ao_geq_attr df_geq_attr = {
        5,
        {
            { 120,   0 },
            { 500,   0 },
            { 1500,  0 },
            { 5000,  0 },
            { 10000, 0 },
            { 0, 0 },
            { 0, 0 },
            { 0, 0 },
            { 0, 0 },
            { 0, 0 }
        }
    };

    ret = memcpy_s(geq_attr, sizeof(*geq_attr), &df_geq_attr, sizeof(df_geq_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 ao_check_geq_attr(hi_ao_geq_attr *geq_attr)
{
    hi_u32 i;

    if ((geq_attr->band_num < AO_EQ_BAND_NUM_MIN) ||
        (geq_attr->band_num > AO_EQ_BAND_NUM_MAX)) {
        HI_LOG_ERR("invalid geq band_num!\n");
        HI_ERR_PRINT_U32(geq_attr->band_num);
        return HI_ERR_AO_INVALID_PARA;
    }

    for (i = 0; i < geq_attr->band_num; i++) {
        if ((geq_attr->param[i].gain < -15000) ||
            (geq_attr->param[i].gain > 15000) ||
            (geq_attr->param[i].gain % 125)) {
            HI_LOG_ERR("invalid geq gain value!\n");
            return HI_ERR_AO_INVALID_PARA;
        }

        if ((geq_attr->param[i].freq < 20) || (geq_attr->param[i].freq > 20000)) {
            HI_LOG_ERR("invalid geq gain value!\n");
            return HI_ERR_AO_INVALID_PARA;
        }
    }

    return HI_SUCCESS;
}

hi_s32 ao_snd_set_geq_attr(ao_snd_id sound, hi_ao_geq_attr *geq_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);
    snd_engine_state *engine = HI_NULL;

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(geq_attr);

    ret = ao_check_geq_attr(geq_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_check_geq_attr, ret);
        return ret;
    }

    osal_list_for_each_entry(engine, &card->engine, node) {
        if (engine->engine_type != PCM) {
            continue;
        }

        ret = hal_aoe_engine_set_geq_attr(engine->engine, geq_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_geq_attr, ret);
            return ret;
        }
    }

    ret = memcpy_s(&card->geq_attr, sizeof(card->geq_attr), geq_attr, sizeof(*geq_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 ao_snd_get_geq_attr(ao_snd_id sound, hi_ao_geq_attr *geq_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(geq_attr);
    CHECK_AO_NULL_PTR(card);

    ret = memcpy_s(geq_attr, sizeof(*geq_attr), &card->geq_attr, sizeof(hi_ao_geq_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 ao_snd_set_geq_enable(ao_snd_id sound, hi_bool enable)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);
    snd_engine_state *engine = HI_NULL;

    CHECK_AO_NULL_PTR(card);

    osal_list_for_each_entry(engine, &card->engine, node) {
        if (engine->engine_type != PCM) {
            continue;
        }

        ret = hal_aoe_engine_set_geq_enable(engine->engine, enable);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_geq_enable, ret);
            return ret;
        }
    }

    card->geq_enable = enable;
    return HI_SUCCESS;
}

hi_s32 ao_snd_get_geq_enable(ao_snd_id sound, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(enable);

    *enable = card->geq_enable;
    return HI_SUCCESS;
}

hi_s32 ao_snd_set_geq_gain(ao_snd_id sound, hi_u32 band, hi_s32 gain)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_engine_state *engine = HI_NULL;

    CHECK_AO_NULL_PTR(card);

    if (band >= HI_AO_EQ_BAND_NUM_MAX) {
        return HI_ERR_AO_INVALID_PARA;
    }

    if ((gain < -15000) || (gain > 15000) || (gain % 125)) {
        HI_LOG_ERR("invalid geq gain!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    osal_list_for_each_entry(engine, &card->engine, node) {
        hi_s32 ret;
        if (engine->engine_type != PCM) {
            continue;
        }

        ret = hal_aoe_engine_set_geq_gain(engine->engine, band, gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_geq_gain, ret);
            return ret;
        }
    }

    card->geq_attr.param[band].gain = gain;
    return HI_SUCCESS;
}

hi_s32 ao_snd_get_geq_gain(ao_snd_id sound, hi_u32 band, hi_s32 *gain)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(gain);

    if (band >= HI_AO_EQ_BAND_NUM_MAX) {
        return HI_ERR_AO_INVALID_PARA;
    }

    *gain = card->geq_attr.param[band].gain;

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_snd_set_geq_attr(hi_void *file, hi_void *arg)
{
    ao_snd_geq_param_p geq_attr = (ao_snd_geq_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(geq_attr->sound);
    return ao_snd_set_geq_attr(geq_attr->sound, &geq_attr->eq_attr);
}

hi_s32 ao_ioctl_snd_get_geq_attr(hi_void *file, hi_void *arg)
{
    ao_snd_geq_param_p geq_attr = (ao_snd_geq_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(geq_attr->sound);
    return ao_snd_get_geq_attr(geq_attr->sound, &geq_attr->eq_attr);
}

hi_s32 ao_ioctl_snd_set_geq_enable(hi_void *file, hi_void *arg)
{
    ao_snd_eq_enable_p geq_eanble = (ao_snd_eq_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(geq_eanble->sound);
    return ao_snd_set_geq_enable(geq_eanble->sound, geq_eanble->eq_enable);
}

hi_s32 ao_ioctl_snd_get_geq_enable(hi_void *file, hi_void *arg)
{
    ao_snd_eq_enable_p geq_eanble = (ao_snd_eq_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(geq_eanble->sound);
    return ao_snd_get_geq_enable(geq_eanble->sound, &geq_eanble->eq_enable);
}

hi_s32 ao_ioctl_snd_set_geq_gain(hi_void *file, hi_void *arg)
{
    ao_snd_geq_gain_p geq_gain = (ao_snd_geq_gain_p)arg;
    CHECK_AO_SNDCARD_OPEN(geq_gain->sound);
    return ao_snd_set_geq_gain(geq_gain->sound, geq_gain->band, geq_gain->gain);
}

hi_s32 ao_ioctl_snd_get_geq_gain(hi_void *file, hi_void *arg)
{
    ao_snd_geq_gain_p geq_gain = (ao_snd_geq_gain_p)arg;
    CHECK_AO_SNDCARD_OPEN(geq_gain->sound);
    return ao_snd_get_geq_gain(geq_gain->sound, geq_gain->band, &geq_gain->gain);
}

hi_s32 ao_write_proc_geq(ao_snd_id sound, snd_card_state *card, hi_char *pc_buf)
{
    const hi_char *pc_on_cmd = "on";
    const hi_char *pc_off_cmd = "off";
    const hi_char *pc_one_cmd = "1";
    const hi_char *pc_zero_cmd = "0";
    const hi_char *pc_band = "band";
    const hi_char *pc_gain = "gain";
    hi_u32 band_id;
    hi_u32 val1, val2;
    hi_s32 ret;
    hi_bool nagetive = HI_FALSE;
    hi_bool geq_enable = HI_FALSE;
    hi_ao_geq_attr geq_attr = { 0 };

    if ((pc_buf == strstr(pc_buf, pc_on_cmd)) || (pc_buf == strstr(pc_buf, pc_one_cmd))) {
        ret = ao_snd_get_geq_enable(sound, &geq_enable);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_snd_get_geq_enable, ret);
            return ret;
        }

        ret = ao_snd_set_geq_enable(sound, HI_TRUE);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_snd_set_geq_enable, ret);
            return ret;
        } else {
            osal_printk("set GEQ success: %s -> 1(ON)\n", geq_enable ? "1(ON)" : "0(OFF)");
        }

        return ret;
    } else if ((pc_buf == strstr(pc_buf, pc_off_cmd)) || (pc_buf == strstr(pc_buf, pc_zero_cmd))) {
        ret = ao_snd_get_geq_enable(sound, &geq_enable);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_snd_get_geq_enable, ret);
            return ret;
        }

        ret = ao_snd_set_geq_enable(sound, HI_FALSE);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_snd_set_geq_enable, ret);
            return ret;
        } else {
            osal_printk("set GEQ success: %s -> 0(OFF)\n", geq_enable ? "1(ON)" : "0(OFF)");
        }

        return ret;
    } else if (pc_buf == strstr(pc_buf, pc_band)) {
        ao_snd_get_geq_attr(sound, &geq_attr);

        pc_buf += strlen(pc_band);
        band_id = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
        if (band_id >= geq_attr.band_num) {
            return HI_FAILURE;
        }
        AO_STRING_SKIP_BLANK(pc_buf);
        if (pc_buf == strstr(pc_buf, pc_gain)) {
            pc_buf += strlen(pc_gain);
            AO_STRING_SKIP_BLANK(pc_buf);
            if (pc_buf[0] == '-') {
                nagetive = HI_TRUE;
                pc_buf++;
            }
            val1 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            if (pc_buf[0] == '.') {
                pc_buf++;
                val2 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            } else {
                val2 = 0;
            }
            geq_attr.param[band_id].gain = (nagetive == HI_TRUE) ? (-(val1 * 1000 + val2)) : (val1 * 1000 + val2);
            return ao_snd_set_geq_attr(sound, &geq_attr);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end__cplusplus */
