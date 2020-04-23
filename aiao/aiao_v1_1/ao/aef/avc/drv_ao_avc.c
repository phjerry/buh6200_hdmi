/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao_v1_1 drv avc implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "drv_ao_avc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_void ao_get_def_avc_attr(hi_ao_avc_attr *avc_attr)
{
    hi_s32 ret;
    hi_ao_avc_attr df_avc_attr = { -32, 5, -10, 50, 100 };

    ret = memcpy_s(avc_attr, sizeof(*avc_attr), &df_avc_attr, sizeof(df_avc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
    }
}

static hi_s32 ao_check_avc_attr(hi_ao_avc_attr *avc_attr)
{
    if ((avc_attr->attack_time < 20) ||
        (avc_attr->release_time < 20) ||
        (avc_attr->attack_time > 2000) ||
        (avc_attr->release_time > 2000) ||
        (avc_attr->threshold < -39) ||
        (avc_attr->threshold > -17) ||
        (avc_attr->gain < 0) ||
        (avc_attr->gain > 8) ||
        (avc_attr->limit < -16) ||
        (avc_attr->limit > -6)) {
        return HI_ERR_AO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_snd_set_avc_attr(ao_snd_id sound, hi_ao_avc_attr *avc_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);
    snd_engine_state *engine = HI_NULL;

    CHECK_AO_NULL_PTR(avc_attr);
    CHECK_AO_NULL_PTR(card);

    ret = ao_check_avc_attr(avc_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_check_avc_attr, ret);
        return ret;
    }

    osal_list_for_each_entry(engine, &card->engine, node) {
        if (engine->engine_type != PCM) {
            continue;
        }
        ret = hal_aoe_engine_set_avc_attr(engine->engine, avc_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_avc_attr, ret);
            return ret;
        }
    }

    ret = memcpy_s(&card->avc_attr, sizeof(card->avc_attr), avc_attr, sizeof(*avc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_snd_get_avc_attr(ao_snd_id sound, hi_ao_avc_attr *avc_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(avc_attr);
    CHECK_AO_NULL_PTR(card);

    ret = memcpy_s(avc_attr, sizeof(*avc_attr), &card->avc_attr, sizeof(card->avc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_snd_set_avc_enable(ao_snd_id sound, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_engine_state *engine = HI_NULL;

    CHECK_AO_NULL_PTR(card);

    osal_list_for_each_entry(engine, &card->engine, node) {
        hi_s32 ret;
        if (engine->engine_type != PCM) {
            continue;
        }

        ret = hal_aoe_engine_set_avc_enable(engine->engine, enable);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_avc_enable, ret);
            return ret;
        }
    }

    card->avc_enable = enable;
    return HI_SUCCESS;
}

static hi_s32 ao_snd_get_avc_enable(ao_snd_id sound, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    *enable = card->avc_enable;

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_snd_set_avc_attr(hi_void *file, hi_void *arg)
{
    ao_snd_avc_param_p avc_attr = (ao_snd_avc_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(avc_attr->sound);
    return ao_snd_set_avc_attr(avc_attr->sound, &avc_attr->avc_attr);
}

hi_s32 ao_ioctl_snd_get_avc_attr(hi_void *file, hi_void *arg)
{
    ao_snd_avc_param_p avc_attr = (ao_snd_avc_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(avc_attr->sound);
    return ao_snd_get_avc_attr(avc_attr->sound, &avc_attr->avc_attr);
}

hi_s32 ao_ioctl_snd_set_avc_enable(hi_void *file, hi_void *arg)
{
    ao_snd_avc_enable_p avc_eanble = (ao_snd_avc_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(avc_eanble->sound);
    return ao_snd_set_avc_enable(avc_eanble->sound, avc_eanble->avc_enable);
}

hi_s32 ao_ioctl_snd_get_avc_enable(hi_void *file, hi_void *arg)
{
    ao_snd_avc_enable_p avc_eanble = (ao_snd_avc_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(avc_eanble->sound);
    return ao_snd_get_avc_enable(avc_eanble->sound, &avc_eanble->avc_enable);
}

#ifdef HI_PROC_SUPPORT
static hi_s32 avc_write_proc_set_attr(ao_snd_id sound, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_char *cmd = argv[2];
    const hi_char *pc_attack_time = "attacktime";
    const hi_char *pc_release_time = "releasetime";
    const hi_char *pc_thrhd = "thrhd";
    const hi_char *pc_limit = "limit";
    const hi_char *pgain = "gain";
    hi_ao_avc_attr avc_attr = { 0 };

    ao_snd_get_avc_attr(sound, &avc_attr);

    if (argv[1] == strstr(argv[1], pc_attack_time)) {
        avc_attr.attack_time = (hi_u32)simple_strtoul(cmd, &cmd, 10);
    } else if (argv[1] == strstr(argv[1], pc_release_time)) {
        avc_attr.release_time = (hi_u32)simple_strtoul(cmd, &cmd, 10);
    } else if (argv[1] == strstr(argv[1], pc_thrhd)) {
        avc_attr.threshold = simple_strtol(cmd, &cmd, 10);
    } else if (argv[1] == strstr(argv[1], pc_limit)) {
        avc_attr.limit = simple_strtol(cmd, &cmd, 10);
    } else if (argv[1] == strstr(argv[1], pgain)) {
        avc_attr.gain = simple_strtoul(cmd, &cmd, 10);
    } else {
        return HI_FAILURE;
    }

    if (ao_check_avc_attr(&avc_attr) != HI_SUCCESS) {
        osal_printk("invalid value!please view help to get more information\n");
        return HI_FAILURE;
    }
    if (ao_snd_set_avc_attr(sound, &avc_attr) == HI_SUCCESS) {
        osal_printk("avc attributes set success\n");
        return HI_SUCCESS;
    } else {
        osal_printk("avc attributes set failure\n");
        return HI_FAILURE;
    }
}

hi_s32 ao_write_proc_set_avc(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    const hi_char *pc_on_cmd = "on";
    const hi_char *pc_off_cmd = "off";
    const hi_char *pc_one_cmd = "1";
    const hi_char *pc_zero_cmd = "0";
    hi_bool avc_enable = HI_FALSE;
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if ((argv[1] == strstr(argv[1], pc_on_cmd)) || (argv[1] == strstr(argv[1], pc_one_cmd))) {
        avc_enable = card->avc_enable;
        ret = ao_snd_set_avc_enable(card->sound, HI_TRUE);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_snd_set_avc_enable, ret);
        } else {
            osal_printk("set AVC success: %s -> 1(ON)\n", avc_enable ? "1(ON)" : "0(OFF)");
        }

        return ret;
    } else if ((argv[1] == strstr(argv[1], pc_off_cmd)) || (argv[1] == strstr(argv[1], pc_zero_cmd))) {
        avc_enable = card->avc_enable;
        ret = ao_snd_set_avc_enable(card->sound, HI_FALSE);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_snd_set_avc_enable, ret);
        } else {
            osal_printk("set AVC success: %s -> 0(OFF)\n", avc_enable ? "1(ON)" : "0(OFF)");
        }

        return ret;
    } else {
        return avc_write_proc_set_attr(card->sound, argv);
    }
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
