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
#include "hdmi_ch_status.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 hdmi_osal_get_eld(hdmi_osal_context *ctx)
{
    hi_s32 ret;

    if (ctx == HI_NULL || ctx->hdmi_func == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (ctx->hdmi_func->get_eld == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    osal_mutex_lock(&ctx->lock);
    ret = (ctx->hdmi_func->get_eld)(ctx->hdmi_id, HI_NULL, (hi_u8 *)&ctx->eld, sizeof(ctx->eld));
    osal_mutex_unlock(&ctx->lock);

    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->get_eld, ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void hdmi_osal_set_mute(hdmi_osal_context *ctx)
{
    hi_s32 ret;

    if (ctx == HI_NULL || ctx->hdmi_func == HI_NULL) {
        return;
    }

    if (ctx->hdmi_func->digital_mute == HI_NULL) {
        return;
    }

    ret = (ctx->hdmi_func->digital_mute)(ctx->hdmi_id, HI_NULL, HI_TRUE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->digital_mute, ret);
        return;
    }
}

hi_void hdmi_osal_set_unmute(hdmi_osal_context *ctx)
{
    hi_s32 ret;

    if (ctx == HI_NULL || ctx->hdmi_func == HI_NULL) {
        return;
    }

    if (ctx->hdmi_func->digital_mute == HI_NULL) {
        return;
    }

    ret = (ctx->hdmi_func->digital_mute)(ctx->hdmi_id, HI_NULL, HI_FALSE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->digital_mute, ret);
        return;
    }
}

static hi_void channel_status_convert(hdmi_ch_status *ch_status, struct ao_attr *ao_attr)
{
    enum {
        HDMI_CH_STATUS_0,
        HDMI_CH_STATUS_1,
        HDMI_CH_STATUS_2,
        HDMI_CH_STATUS_3,
        HDMI_CH_STATUS_4,
        HDMI_CH_STATUS_5,
    };

    /* we only set the lower 8 bit in right value */
    ao_attr->channel_status[HDMI_CH_STATUS_0] = ch_status->ch_status_0.u32;
    ao_attr->channel_status[HDMI_CH_STATUS_1] = ch_status->ch_status_1.u32;
    ao_attr->channel_status[HDMI_CH_STATUS_2] = ch_status->ch_status_2.u32;
    ao_attr->channel_status[HDMI_CH_STATUS_3] = ch_status->ch_status_3.u32;
    ao_attr->channel_status[HDMI_CH_STATUS_4] = ch_status->ch_status_4.u32;
    ao_attr->channel_status[HDMI_CH_STATUS_5] = ch_status->ch_status_5.u32;
}

hi_s32 hdmi_osal_hw_params_validate(hdmi_osal_context *ctx, hdmi_ao_attr *attr)
{
    hi_s32 ret;
    hdmi_ch_status ch_status;

    struct ao_attr ao_attr = {
        .aud_codec = attr->audio_format,
        .aud_sample_size = attr->bit_depth,
        .aud_input_type = attr->interface + 1,
        .aud_sample_rate = attr->sample_rate,
        .aud_channels = attr->channels,
        .channel_status = { 0 },
    };

    if (ctx == HI_NULL || ctx->hdmi_func == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (ctx->hdmi_func->hw_params_validate == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    hdmi_ch_status_init(attr, &ch_status);
    channel_status_convert(&ch_status, &ao_attr);

    ret = (ctx->hdmi_func->hw_params_validate)(ctx->hdmi_id, &ao_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->hw_params_validate, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hdmi_osal_set_hw_params(hdmi_osal_context *ctx, hdmi_ao_attr *attr)
{
    hi_s32 ret;
    hdmi_ch_status ch_status;

    struct ao_attr ao_attr = {
        .aud_codec = attr->audio_format,
        .aud_sample_size = attr->bit_depth,
        .aud_input_type = attr->interface + 1,
        .aud_sample_rate = attr->sample_rate,
        .aud_channels = attr->channels,
        .channel_status = { 0 },
    };

    if (ctx == HI_NULL || ctx->hdmi_func == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (ctx->hdmi_func->hw_params == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    hdmi_ch_status_init(attr, &ch_status);
    channel_status_convert(&ch_status, &ao_attr);

    ret = (ctx->hdmi_func->hw_params)(ctx->hdmi_id, &ao_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->hw_params, ret);
        return ret;
    }

    ret = memcpy_s(&ctx->ao_attr, sizeof(hdmi_ao_attr), attr, sizeof(hdmi_ao_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hdmi_osal_set_sample_rate(hdmi_osal_context *ctx, hi_u32 sample_rate)
{
    return HI_SUCCESS;
}

static hi_u32 hdmi_audio_format(hi_u32 format)
{
    switch (format) {
        case IEC61937_DATATYPE_DOLBY_DIGITAL:
            return HI_AUDIO_FORMAT_AC3;

        case IEC61937_DATATYPE_DTS_TYPE_I:
        case IEC61937_DATATYPE_DTS_TYPE_II:
        case IEC61937_DATATYPE_DTS_TYPE_III:
        case IEC61937_DATATYPE_DTSCD:
            return HI_AUDIO_FORMAT_DTS;

        case IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS:
            return HI_AUDIO_FORMAT_EAC3;

        case IEC61937_DATATYPE_DTS_TYPE_IV:
            return HI_AUDIO_FORMAT_DTS_HD;

        case IEC61937_DATATYPE_DOLBY_TRUE_HD:
            return HI_AUDIO_FORMAT_MAT;

        case IEC61937_DATATYPE_71_LPCM:
            return HI_AUDIO_FORMAT_PCM;

        default:
            return HI_AUDIO_FORMAT_CXT;
    }
}

hi_bool hdmi_osal_check_audio_support(hdmi_osal_context *ctx, hi_u32 format)
{
    hi_u32 sad_count;
    hi_bool support = HI_FALSE;

    if (ctx == HI_NULL || ctx->hdmi_plug_in == HI_FALSE) {
        return HI_FALSE;
    }

    format = hdmi_audio_format(format);
    if (format == HI_AUDIO_FORMAT_CXT) {
        return HI_FALSE;
    }

    osal_mutex_lock(&ctx->lock);

    for (sad_count = 0; sad_count < ctx->eld.sad_count; sad_count++) {
        if (ctx->eld.sad[sad_count].fmt_code != format) {
            continue;
        }

        if (format != HI_AUDIO_FORMAT_PCM) {
            support = HI_TRUE;
            break;
        }

        if (ctx->eld.sad[sad_count].max_channel > HI_AUDIO_CH_STEREO) {
            support = HI_TRUE;
            break;
        }
    }

    osal_mutex_unlock(&ctx->lock);

    return support;
}

static hi_s32 hpd_detect(hdmi_osal_context *ctx, hi_u32 *status)
{
    hi_s32 ret;
    hi_u32 hdmi_status = 0;

    *status = 0;

    if (ctx->hdmi_func->hpd_detect == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    ret = (ctx->hdmi_func->hpd_detect)(ctx->hdmi_id, &hdmi_status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->hpd_detect, ret);
        return HI_FAILURE;
    }

    *status = hdmi_status;

    return HI_SUCCESS;
}

hi_s32 register_notifier(hdmi_osal_context *ctx)
{
    hi_s32 ret;

    if (ctx->hdmi_func->register_notifier == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    ret = (ctx->hdmi_func->register_notifier)(ctx->hdmi_id, &ctx->nb);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->register_notifier, ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void unregister_notifier(hdmi_osal_context *ctx)
{
    hi_s32 ret;

    if (ctx->hdmi_func == HI_NULL) {
        return;
    }

    if (ctx->hdmi_func->unregister_notifier == HI_NULL) {
        return;
    }

    ret = (ctx->hdmi_func->unregister_notifier)(ctx->hdmi_id, &ctx->nb);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ctx->hdmi_func->unregister_notifier, ret);
        return;
    }
}

hi_s32 hdmi_osal_init(hi_ao_port hdmi_port, hdmi_osal_context *ctx)
{
    hi_s32 ret;
    hi_u32 status;
    struct hi_hdmi_ext_intf *hdmi_func = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_HDMITX, (hi_void **)&hdmi_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_get, ret);
        return HI_FAILURE;
    }

    if ((hdmi_func == HI_NULL) || (hdmi_func->ao_data == HI_NULL)) {
        HI_LOG_ERR("hdmi_func is null\n");
        return HI_ERR_AO_NULL_PTR;
    }

    if (hdmi_port == HI_AO_PORT_HDMI0) {
        ctx->hdmi_id = AO_INTF_HDMI0;
    } else if (hdmi_port == HI_AO_PORT_HDMI1) {
        ctx->hdmi_id = AO_INTF_HDMI1;
    } else {
        return HI_ERR_AO_INVALID_PARA;
    }

    ctx->hdmi_func = (struct hi_ao_intf_module_ops *)(hdmi_func->ao_data);
    osal_mutex_init(&ctx->lock);

    ret = hpd_detect(ctx, &status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hpd_detect, ret);
    }

    if (status == AO_HDMI_HPD_PLUGIN) {
        hdmi_osal_get_eld(ctx);
    }

    ctx->hdmi_plug_in = (status == AO_HDMI_HPD_PLUGIN);

    ret = register_notifier(ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(register_notifier, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void hdmi_osal_deinit(hdmi_osal_context *ctx)
{
    if (ctx == HI_NULL) {
        return;
    }

    unregister_notifier(ctx);
    osal_mutex_destory(&ctx->lock);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */
