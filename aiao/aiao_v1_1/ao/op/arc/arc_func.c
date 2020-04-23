/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port arc function.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"
#include "audio_util.h"

#include "arc_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_ao_ouput_mode user_arc_mode;

    hi_bool user_arc_enable;
    hi_ao_arc_audio_cap user_arc_cap;

    hi_u32 arc_passthrough;   /* 0(no arc), 1(pcm), 2(lbr), 3(hbr/7.1 lpcm) */
    hi_u32 arc_data_format;

    snd_op_state op;
} arc_device;

static inline arc_device *get_arc_device(snd_op_state *snd_op)
{
    return osal_container_of(snd_op, arc_device, op);
}

static hi_void arc_op_init(arc_device *device)
{
    device->arc_data_format = 0;
    device->arc_passthrough = PCM;
    device->user_arc_mode = HI_AO_OUTPUT_MODE_LPCM;
    device->user_arc_enable = HI_FALSE;

    device->op.out_type = SND_OUTPUT_TYPE_SPDIF;
    snd_op_init(&device->op);
}

static hi_void arc_op_destroy(snd_op_state *snd_op, hi_bool suspend)
{
    arc_device *device = get_arc_device(snd_op);

    snd_op_destroy(snd_op, suspend);
    osal_kfree(HI_ID_AO, device);
}

static hi_s32 arc_op_create(snd_card_state *card, hi_ao_port_attr *attr, hi_bool resume)
{
    hi_s32 ret;
    arc_device *device = HI_NULL;

    device = (arc_device *)osal_kmalloc(HI_ID_AO, sizeof(arc_device), OSAL_GFP_KERNEL);
    if (device == HI_NULL) {
        HI_LOG_ERR("osal_kmalloc arc_device failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(device, sizeof(*device), 0, sizeof(*device));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out;
    }

    arc_op_init(device);

    ret = snd_op_create_aop(&device->op, attr, SND_AOP_TYPE_SPDIF, card->user_sample_rate);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_create_aop, ret);
        goto out;
    }

    card->arc_passthrough = PCM;
    card->user_arc_enable = HI_FALSE;
    card->user_arc_mode = HI_AO_OUTPUT_MODE_LPCM;
    card->lbr_device = HI_TRUE;
    card->hbr_device = HI_TRUE;

    osal_list_add_tail(&device->op.node, &card->op);

    return ret;

out:
    osal_kfree(HI_ID_AO, device);
    return ret;
}

hi_s32 arc_op_set_enable(snd_card_state *card, snd_op_state *snd_op, hi_bool enable)
{
    arc_device *device = get_arc_device(snd_op);

    card->user_arc_enable = enable;
    device->user_arc_enable = enable;

    hal_aiao_set_spdif_port_en(AIAO_PORT_SPDIF_TX2, enable);
    hal_aiao_set_spdif_port_en(AIAO_PORT_SPDIF_TX1, !enable);

    return HI_SUCCESS;
}

hi_s32 arc_op_get_enable(snd_card_state *card, snd_op_state *snd_op, hi_bool *enable)
{
    arc_device *device = get_arc_device(snd_op);

    *enable = device->user_arc_enable;
    return HI_SUCCESS;
}

static hi_s32 arc_op_set_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode mode)
{
    arc_device *device = get_arc_device(snd_op);

    card->user_arc_mode = mode;
    device->user_arc_mode = mode;
    return HI_SUCCESS;
}

static hi_s32 arc_op_get_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode *mode)
{
    arc_device *device = get_arc_device(snd_op);

    *mode = device->user_arc_mode;
    return HI_SUCCESS;
}

hi_s32 arc_op_set_cap(snd_card_state *card, snd_op_state *snd_op, hi_ao_arc_audio_cap *cap)
{
    hi_s32 ret;
    arc_device *device = get_arc_device(snd_op);

    ret = memcpy_s(&device->user_arc_cap, sizeof(device->user_arc_cap), cap, sizeof(*cap));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 arc_op_get_cap(snd_card_state *card, snd_op_state *snd_op, hi_ao_arc_audio_cap *cap)
{
    hi_s32 ret;
    arc_device *device = get_arc_device(snd_op);

    ret = memcpy_s(cap, sizeof(*cap), &device->user_arc_cap, sizeof(device->user_arc_cap));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_bool arc_op_match(hi_ao_port ao_port)
{
    hi_u32 i;
    const hi_ao_port arc_op_device[] = {
        HI_AO_PORT_ARC0, HI_AO_PORT_ARC1
    };

    for (i = 0; i < ARRAY_SIZE(arc_op_device); i++) {
        if (arc_op_device[i] == ao_port) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static snd_output_port_driver g_arc_op_driver = {
    .match    = arc_op_match,
    .create   = arc_op_create,
    .destroy  = arc_op_destroy,
    .set_output_mode = arc_op_set_mode,
    .get_output_mode = arc_op_get_mode,
};

hi_void arc_op_register_driver(struct osal_list_head *head)
{
    osal_list_add_tail(&g_arc_op_driver.node, head);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

