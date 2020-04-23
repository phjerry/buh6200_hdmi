/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao engine api
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"

#include "drv_ao_track.h"

static hi_void engine_attach_op(snd_card_state *card, snd_engine_state *state)
{
    snd_op_state *op = HI_NULL;

    osal_list_for_each_entry(op, &card->op, node) {
        if (op->engine_type[op->active_id] == state->engine_type) {
            hal_aoe_engine_attach_aop(state->engine, op->aop[op->active_id]);
        }
    }
}

static hi_void engine_get_create_attr(snd_card_state *card, hi_u32 engine_type,
    aoe_engine_attr *engine_attr)
{
    switch (engine_type) {
        case PCM: {
            engine_attr->bit_per_sample = AO_TRACK_BITDEPTH_LOW;
            engine_attr->channels = AO_TRACK_NORMAL_CHANNELNUM;
            /* pcm engine must follow SND's samplerate */
            engine_attr->sample_rate = card->user_sample_rate;
            engine_attr->data_format = 0;
            break;
        }

        case LBR: {
            engine_attr->bit_per_sample = AO_TRACK_BITDEPTH_LOW;
            engine_attr->channels = AO_TRACK_NORMAL_CHANNELNUM;
            engine_attr->sample_rate = HI_SAMPLE_RATE_48K;
            engine_attr->data_format = 1;
            break;
        }

        case HBR: {
            engine_attr->bit_per_sample = AO_TRACK_BITDEPTH_LOW;
            engine_attr->channels = AO_TRACK_MUTILPCM_CHANNELNUM;
            engine_attr->sample_rate = HI_SAMPLE_RATE_192K;
            engine_attr->data_format = 1;
            break;
        }

        default:
        {
            HI_LOG_ERR("invalid engine type\n");
            break;
        }
    }
}

static hi_void engine_set_attr(snd_card_state *card, aoe_engine_id engine)
{
#if defined(HI_SND_AVC_SUPPORT)
    /* we need to set avc default attr to avoid avc corupt in AOE */
    hal_aoe_engine_set_avc_attr(engine, &card->avc_attr);
    hal_aoe_engine_set_avc_enable(engine, card->avc_enable);
#endif

#if defined(HI_SND_AR_SUPPORT)
    hal_aoe_engine_set_con_output_enable(engine, card->con_output_enable);
    hal_aoe_engine_set_output_atmos_enable(engine, card->output_atmos_enable);
#endif
}

static hi_s32 engine_create_by_type(snd_card_state *card, hi_u32 engine_type)
{
    hi_s32 ret;
    aoe_engine_attr engine_attr = { 0 };
    snd_engine_state *engine = HI_NULL;

    engine = (snd_engine_state *)osal_kmalloc(HI_ID_AO, sizeof(snd_engine_state), OSAL_GFP_KERNEL);
    if (engine == HI_NULL) {
        HI_LOG_ERR("malloc snd_engine_state failed\n");
        return HI_FAILURE;
    }

    ret = memset_s(engine, sizeof(snd_engine_state), 0, sizeof(snd_engine_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        osal_kfree(HI_ID_AO, engine);
        return ret;
    }

    engine_get_create_attr(card, engine_type, &engine_attr);
    ret = hal_aoe_engine_create(&engine->engine, &engine_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_create, ret);
        osal_kfree(HI_ID_AO, engine);
        return ret;
    }

    engine->engine_type = engine_type;
    engine_set_attr(card, engine->engine);

    HI_LOG_DBG("malloc engine node\n");
    HI_DBG_PRINT_VOID(engine);
    HI_DBG_PRINT_U32(engine_type);

    hal_aoe_engine_start(engine->engine);
    engine_attach_op(card, engine);

    osal_list_add(&engine->node, &card->engine);

    return ret;
}

aoe_engine_id ao_engine_get_id_by_type(snd_card_state *card, hi_u32 data_type)
{
    snd_engine_state *engine = HI_NULL;

    osal_list_for_each_entry(engine, &card->engine, node) {
        if (engine->engine_type == data_type) {
            return engine->engine;
        }
    }

    return AOE_ENGINE_MAX;
}

hi_void ao_engine_deinit(snd_card_state *card)
{
    snd_engine_state *engine = HI_NULL;
    snd_engine_state *tmp = HI_NULL;

    osal_list_for_each_entry_safe(engine, tmp, &card->engine, node) {
        hal_aoe_engine_stop(engine->engine);
        hal_aoe_engine_destroy(engine->engine);

        osal_list_del(&engine->node);
        osal_kfree(HI_ID_AO, engine);
    }

    HI_LOG_INFO("destory pcm engine and raw engine.\n");
}

hi_s32 ao_engine_init(snd_card_state *card)
{
    hi_s32 ret;

    /* always create pcm engine */
    ret = engine_create_by_type(card, PCM);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(engine_create_by_type, ret);
        return ret;
    }

    if (card->lbr_device == HI_TRUE) {
        ret = engine_create_by_type(card, LBR);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(engine_create_by_type, ret);
            goto ERR_EXIT;
        }
    }

    if (card->hbr_device == HI_TRUE) {
        ret = engine_create_by_type(card, HBR);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(engine_create_by_type, ret);
            goto ERR_EXIT;
        }
    }

    HI_LOG_INFO("create pcm engine and raw engine success.\n");

    return ret;

ERR_EXIT:
    ao_engine_deinit(card);
    return ret;
}

