/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port device function.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"
#include "hdmi_osal.h"
#include "hdmi_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_ao_ouput_mode user_hdmi_mode;
    hi_u32 hdmi_passthrough;  /* 3(no hdmi),  0(pcm), 1(lbr), 2(hbr/7.1 lpcm) */
    hi_u32 hdmi_data_format;
    hi_bool hdmi_debug;
    hi_bool hdmi_mute;

    snd_card_state *card;

    snd_op_state op;

    hdmi_osal_context hdmi_osal_ctx;
} hdmi_device;

static inline hdmi_device *get_hdmi_device(snd_op_state *snd_op)
{
    return osal_container_of(snd_op, hdmi_device, op);
}

static hi_void hdmi_op_init(hdmi_device *device)
{
    device->hdmi_data_format = 0;
    device->hdmi_debug = HI_FALSE;
    device->hdmi_mute = HI_FALSE;
    device->hdmi_passthrough = PCM;
    device->user_hdmi_mode = HI_AO_OUTPUT_MODE_LPCM;

    device->op.out_type = SND_OUTPUT_TYPE_HDMI;
    snd_op_init(&device->op);
}

static hi_s32 hdmi_notifier_event(struct notifier_block *nb, hi_ulong event, hi_void *arg)
{
    hi_s32 ret;
    hdmi_osal_context *ctx = osal_container_of(nb, hdmi_osal_context, nb);
    hdmi_device *device = osal_container_of(ctx, hdmi_device, hdmi_osal_ctx);

    switch (event) {
        case AO_HDMI_BEFORE_OE_DISABLE:
            hdmi_op_set_mute(&device->op);
            break;

        case AO_HDMI_AFTER_OE_DISABLE:
            ret = hdmi_osal_hw_params_validate(ctx, &ctx->ao_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hdmi_osal_hw_params_validate, ret);
                return ret;
            }

            ret = hdmi_osal_set_hw_params(ctx, &ctx->ao_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hdmi_osal_set_hw_params, ret);
                return ret;
            }
            break;

        case AO_HDMI_OE_ENABLE:
            hdmi_op_set_unmute(&device->op);
            break;

        case AO_HDMI_HOT_PLUG_IN:
            ret = hdmi_osal_get_eld(ctx);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hdmi_osal_get_eld, ret);
                return ret;
            }

            ctx->hdmi_plug_in = HI_TRUE;
            break;

        case AO_HDMI_HOT_PLUG_OUT:
            ctx->hdmi_plug_in = HI_FALSE;
            break;

        default:
            return HI_SUCCESS;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmi_op_osal_init(snd_card_state *card, hdmi_device *device)
{
    hi_s32 ret;
    hdmi_ao_attr ao_attr = {
        .audio_format = HI_AUDIO_FORMAT_PCM,
        .interface = HDMI_AO_INTERFACE_I2S,
        .channels = HI_AUDIO_CH_STEREO,
        .sample_rate = card->user_sample_rate,
        .bit_depth = HI_BIT_DEPTH_16,
    };

    device->hdmi_osal_ctx.nb.notifier_call = hdmi_notifier_event;

    ret = hdmi_osal_init(card->hdmi_port, &device->hdmi_osal_ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hdmi_osal_init, ret);
        return ret;
    }

    ret = hdmi_osal_set_hw_params(&device->hdmi_osal_ctx, &ao_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hdmi_osal_set_hw_params, ret);
        return ret;
    }

    hdmi_osal_set_unmute(&device->hdmi_osal_ctx);

    return HI_SUCCESS;
}

static hi_void hdmi_op_destroy(snd_op_state *snd_op, hi_bool suspend)
{
    hdmi_device *device = get_hdmi_device(snd_op);

    hdmi_osal_set_mute(&device->hdmi_osal_ctx);
    snd_op_destroy(snd_op, suspend);
    hdmi_osal_deinit(&device->hdmi_osal_ctx);

    osal_kfree(HI_ID_AO, device);
}

static hi_s32 hdmi_op_create(snd_card_state *card, hi_ao_port_attr *attr, hi_bool resume)
{
    hi_s32 ret;
    hdmi_device *device = HI_NULL;
    snd_aop_type aop_type;

    device = (hdmi_device *)osal_kmalloc(HI_ID_AO, sizeof(hdmi_device), OSAL_GFP_KERNEL);
    if (device == HI_NULL) {
        HI_FATAL_AO("MALLOC hdmi_device failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(device, sizeof(*device), 0, sizeof(*device));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out;
    }

    card->hdmi_port = attr->port;

    ret = hdmi_op_osal_init(card, device);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hdmi_op_osal_init, ret);
        goto out;
    }

    hdmi_op_init(device);

    for (aop_type = SND_AOP_TYPE_I2S; aop_type < SND_AOP_TYPE_CAST; aop_type++) {
#ifdef HI_SND_HDMI_PASSTHROUGH_USE_I2S_ONLY
        if (aop_type == SND_AOP_TYPE_SPDIF) {
            continue;
        }
#endif
        ret = snd_op_create_aop(&device->op, attr, aop_type, card->user_sample_rate);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_create_aop, ret);
            goto out;
        }
    }

    card->hdmi_passthrough = PCM;
    card->user_hdmi_mode = HI_AO_OUTPUT_MODE_LPCM;
    card->hdmi_data_format = 0;
    card->lbr_device = HI_TRUE;
    card->hbr_device = HI_TRUE;
    device->card = card;

    osal_list_add_tail(&device->op.node, &card->op);

    return ret;

out:
    osal_kfree(HI_ID_AO, device);
    return ret;
}

hi_s32 hdmi_op_set_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode mode)
{
    hdmi_device *device = get_hdmi_device(snd_op);
    card->user_hdmi_mode = mode;
    device->user_hdmi_mode = mode;

    return HI_SUCCESS;
}

hi_s32 hdmi_op_get_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode *mode)
{
    hdmi_device *device = get_hdmi_device(snd_op);
    *mode = device->user_hdmi_mode;

    return HI_SUCCESS;
}

static hi_bool hdmi_op_match(hi_ao_port ao_port)
{
    hi_u32 i;
    const hi_ao_port hdmi_op_device[] = {
        HI_AO_PORT_HDMI0, HI_AO_PORT_HDMI1,
    };

    for (i = 0; i < ARRAY_SIZE(hdmi_op_device); i++) {
        if (hdmi_op_device[i] == ao_port) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static const struct {
    hi_ao_port   ao_port;
    snd_aop_type aop_type;
    aiao_port_id aiao_port;
    hi_char      *buf_name;
    hi_u32       buf_size;
} g_hdmi_hw_param[] = {
    { HI_AO_PORT_HDMI0, SND_AOP_TYPE_I2S, AIAO_PORT_TX3, "ao_hdmi0_i2s", AO_HDMI_MMZSIZE_MAX},
    { HI_AO_PORT_HDMI0, SND_AOP_TYPE_SPDIF, AIAO_PORT_SPDIF_TX0, "ao_hdmi0_spidf", AO_SPDIF_MMZSIZE_MAX},
    { HI_AO_PORT_HDMI1, SND_AOP_TYPE_I2S, AIAO_PORT_TX0, "ao_hdmi1_i2s", AO_HDMI_MMZSIZE_MAX},
    { HI_AO_PORT_HDMI1, SND_AOP_TYPE_SPDIF, AIAO_PORT_MAX, "ao_hdmi1_spidf", AO_SPDIF_MMZSIZE_MAX},
};

static hi_s32 hdmi_op_get_hw_param(hi_ao_port_attr *attr, snd_aop_type aop_type,
    snd_op_create_param *hw_param)
{
    hi_u32 i;

    /* 1.hdmi get port id */
    hw_param->aiao_port = AIAO_PORT_MAX;

    /* 2.find hdmi hw_param and save */
    for (i = 0; i < ARRAY_SIZE(g_hdmi_hw_param); i++) {
        if (g_hdmi_hw_param[i].ao_port == attr->port &&
            g_hdmi_hw_param[i].aop_type == aop_type) {
            hw_param->buf_name = g_hdmi_hw_param[i].buf_name;
            hw_param->buf_size = g_hdmi_hw_param[i].buf_size;
            hw_param->aiao_port = g_hdmi_hw_param[i].aiao_port;
            break;
        }
    }

    if (hw_param->aiao_port == AIAO_PORT_MAX) {
        return HI_ERR_AO_INVALID_PARA;
    }

    /* 3.hdmi get hw cfg */
    if (aop_type == SND_AOP_TYPE_I2S) {
        hal_aiao_get_hdmi_i2s_df_attr(hw_param->aiao_port, hw_param->port_cfg);
    } else {
        hal_aiao_get_tx_spd_df_attr(hw_param->aiao_port, hw_param->port_cfg);
    }

    return HI_SUCCESS;
}

static snd_output_port_driver g_hdmi_op_driver = {
    .match    = hdmi_op_match,
    .create   = hdmi_op_create,
    .destroy  = hdmi_op_destroy,
    .set_output_mode = hdmi_op_set_mode,
    .get_output_mode = hdmi_op_get_mode,
    .get_hw_param = hdmi_op_get_hw_param,
};

hi_void hdmi_op_register_driver(struct osal_list_head *head)
{
    osal_list_add_tail(&g_hdmi_op_driver.node, head);
}

hi_s32 hdmi_op_set_sample_rate(snd_op_state *snd_op, hi_u32 sample_rate)
{
    hdmi_device *device = get_hdmi_device(snd_op);
    return hdmi_osal_set_sample_rate(&device->hdmi_osal_ctx, sample_rate);
}

hi_bool hdmi_op_check_format_support(snd_op_state *snd_op, hi_u32 format)
{
    hdmi_device *device = get_hdmi_device(snd_op);
    return hdmi_osal_check_audio_support(&device->hdmi_osal_ctx, format);
}

hi_void hdmi_op_set_mute(snd_op_state *snd_op)
{
    hi_s32 ret;
    hi_u32 id;
    hdmi_device *device = get_hdmi_device(snd_op);

    if (device->hdmi_mute == HI_TRUE) {
        return;
    }

    if (snd_op->user_mute == 0) {
        for (id = 0; id < AO_SNDOP_MAX_AOP_NUM; id++) {
            if (snd_op->port_id[id] == AIAO_PORT_MAX) {
                continue;
            }

            ret = hal_aiao_mute(snd_op->port_id[id], HI_TRUE);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aiao_mute, ret);
                HI_ERR_PRINT_H32(snd_op->port_id[id]);
                return;
            }
        }
    }

    hdmi_osal_set_mute(&device->hdmi_osal_ctx);

    device->hdmi_mute = HI_TRUE;
}

hi_void hdmi_op_set_unmute(snd_op_state *snd_op)
{
    hi_s32 ret;
    hi_u32 id;
    hdmi_device *device = get_hdmi_device(snd_op);

    if (device->hdmi_mute == HI_FALSE) {
        return;
    }

    hdmi_osal_set_unmute(&device->hdmi_osal_ctx);

    if (snd_op->user_mute == 0) {
        for (id = 0; id < AO_SNDOP_MAX_AOP_NUM; id++) {
            if (snd_op->port_id[id] == AIAO_PORT_MAX) {
                continue;
            }

            ret = hal_aiao_mute(snd_op->port_id[id], HI_FALSE);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aiao_mute, ret);
                HI_ERR_PRINT_H32(snd_op->port_id[id]);
                return;
            }
        }
    }

    device->hdmi_mute = HI_FALSE;
}

hi_void hdmi_op_set_ao_attr(snd_op_state *snd_op, hdmi_ao_attr *ao_attr)
{
    hi_s32 ret;
    hdmi_device *device = get_hdmi_device(snd_op);

    ret = hdmi_osal_set_hw_params(&device->hdmi_osal_ctx, ao_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hdmi_osal_set_hw_params, ret);
        return;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

