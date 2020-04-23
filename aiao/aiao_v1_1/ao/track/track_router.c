/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: implement of ao track driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"
#include "drv_ao_engine.h"

#include "audio_util.h"

#include "track_router.h"
#include "drv_ao_hdmi.h"

typedef struct {
    hi_u32 bit_depth;
    hi_u32 channels;
    hi_u32 sample_rate;
    hi_u32 format;
} router_ref_frame;

typedef struct {
    snd_card_state *card;
    snd_track_state *track;
    snd_track_stream_attr *track_stream;
    stream_mode_change mode_change;

    router_ref_frame ref_frame;

    aoe_engine_id engine_old;
    aoe_engine_id engine_new;

    aoe_aip_id aip_old;
    aoe_aip_type aip_type_old;
    aoe_aip_id aip_new;
    aoe_aip_type aip_type_new;
    aoe_aip_status aip_status;
    hi_bool resume_aip_delay;

    hi_ao_port out_port;
    snd_op_state *snd_op;
    hi_u32 op_target_delay;
} router_context;

#if defined(HI_SND_HDMI_SUPPORT)
static hi_u32 track_get_hdmi_auto_mode(snd_card_state *card, snd_track_stream_attr *attr)
{
    hi_bool support;

    if (autil_is_hbr_format(attr->hbr_format)) {
        support = ao_snd_hdmi_check_format_support(card, card->hdmi_port,
            autil_get_passth_format(attr->hbr_format));
        if (support == HI_TRUE) {
            return HBR;
        }
    }

    if (autil_is_lbr_format(attr->lbr_format)) {
        support = ao_snd_hdmi_check_format_support(card, card->hdmi_port,
            autil_get_passth_format(attr->lbr_format));
        if (support == HI_TRUE) {
            return LBR;
        }
    }

    return PCM;
}

static hi_void track_get_hdmi_mode(snd_card_state *card, snd_track_stream_attr *attr,
    hi_u32 *mode)
{
    hi_bool lbr_format;
    hi_bool hbr_format;

    lbr_format = autil_is_lbr_format(attr->lbr_format);
    hbr_format = autil_is_hbr_format(attr->hbr_format);
    /* no raw data in stream */
    if ((lbr_format == HI_FALSE) && (hbr_format == HI_FALSE)) {
        *mode = PCM;
        return;
    }

    if (card->user_hdmi_mode == HI_AO_OUTPUT_MODE_LPCM) {
        *mode = PCM;
    } else if (card->user_hdmi_mode == HI_AO_OUTPUT_MODE_RAW) {
        *mode = (hbr_format == HI_TRUE) ? HBR : LBR;
    } else if (card->user_hdmi_mode == HI_AO_OUTPUT_MODE_HBR2LBR) {
        *mode = (lbr_format == HI_TRUE) ? LBR : PCM;
    } else {
        *mode = track_get_hdmi_auto_mode(card, attr);
    }
}
#endif

#if defined(HI_SND_HDMI_SUPPORT) || defined(HI_SND_SPDIF_SUPPORT)
static stream_mode_change track_get_pcm_change(hi_u32 passthrough)
{
    stream_mode_change change = STREAM_CHANGE_NONE;

    if (passthrough == LBR) {
        change = STREAM_CHANGE_LBR2PCM;
    } else if (passthrough == HBR) {
        change = STREAM_CHANGE_HBR2PCM;
    }

    return change;
}

static stream_mode_change track_get_lbr_change(hi_u32 passthrough,
    snd_track_stream_attr *last_attr, snd_track_stream_attr *attr)
{
    stream_mode_change change = STREAM_CHANGE_NONE;

    if (passthrough == PCM) {
        change = STREAM_CHANGE_PCM2LBR;
    } else if (passthrough == LBR) {
        if ((last_attr->lbr_sample_rate != attr->lbr_sample_rate) ||
            (last_attr->lbr_format != attr->lbr_format)) {
            change = STREAM_CHANGE_LBR2LBR;
        }
    } else {
        change = STREAM_CHANGE_HBR2LBR;
    }

    return change;
}

static stream_mode_change track_get_hbr_change(hi_u32 passthrough,
    snd_track_stream_attr *last_attr, snd_track_stream_attr *attr)
{
    stream_mode_change change = STREAM_CHANGE_NONE;

    if (passthrough == PCM) {
        change = STREAM_CHANGE_PCM2HBR;
    } else if (passthrough == LBR) {
        change = STREAM_CHANGE_LBR2HBR;
    } else {
        if ((last_attr->hbr_sample_rate != attr->hbr_sample_rate) ||
            (last_attr->hbr_format != attr->hbr_format)) {
            change = STREAM_CHANGE_HBR2HBR;
        }
    }

    return change;
}
#endif

#if defined(HI_SND_HDMI_SUPPORT)
static stream_mode_change track_get_hdmi_change(snd_card_state *card,
    snd_track_stream_attr *last_attr, snd_track_stream_attr *attr, hi_u32 mode)
{
    stream_mode_change change = STREAM_CHANGE_NONE;

    if (mode == PCM) {
        change = track_get_pcm_change(card->hdmi_passthrough);
    } else if (mode == LBR) {
        change = track_get_lbr_change(card->hdmi_passthrough, last_attr, attr);
    } else if (mode == HBR) {
        change = track_get_hbr_change(card->hdmi_passthrough, last_attr, attr);
    }

    return change;
}

static inline stream_mode_change track_get_hdmi_change_mode(snd_card_state *card,
    snd_track_stream_attr *attr_old, snd_track_stream_attr *attr)
{
    hi_u32 mode = TYPE_MAX;

    track_get_hdmi_mode(card, attr, &mode);

    return track_get_hdmi_change(card, attr_old, attr, mode);
}
#endif

#if defined(HI_SND_SPDIF_SUPPORT)
static stream_mode_change track_get_spdif_change(snd_card_state *card,
    snd_track_stream_attr *last_attr, snd_track_stream_attr *attr, hi_u32 mode)
{
    stream_mode_change change = STREAM_CHANGE_NONE;

    if (mode == PCM) {
        change = track_get_pcm_change(card->spdif_passthrough);
    } else if (mode == LBR) {
        change = track_get_lbr_change(card->spdif_passthrough, last_attr, attr);
    } else if (mode == HBR) {
        change = track_get_hbr_change(card->spdif_passthrough, last_attr, attr);
    }

    return change;
}
#endif

#if defined(HI_SND_ARC_SUPPORT)
static hi_u32 track_arc_edid_change(hi_ao_arc_audio_cap *arc_cap, hi_u32 format)
{
    switch (format) {
        case IEC61937_DATATYPE_NULL:
            return PCM;

        case IEC61937_DATATYPE_DOLBY_DIGITAL:
            if (arc_cap->audio_fmt_supported[HI_AUDIO_FORMAT_AC3] == HI_TRUE) {
                return LBR;
            } else {
                return PCM;
            }

        case IEC61937_DATATYPE_DTS_TYPE_I:
        case IEC61937_DATATYPE_DTS_TYPE_II:
        case IEC61937_DATATYPE_DTS_TYPE_III:
        case IEC61937_DATATYPE_DTSCD:
            if (arc_cap->audio_fmt_supported[HI_AUDIO_FORMAT_DTS] == HI_TRUE) {
                return LBR;
            } else {
                return PCM;
            }

        case IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS:
            if (arc_cap->audio_fmt_supported[HI_AUDIO_FORMAT_EAC3] == HI_TRUE) {
                return HBR;
            } else {
                return LBR;
            }

        case IEC61937_DATATYPE_DTS_TYPE_IV:  /* spdif can't transmit dtshd */
            return LBR;

        case IEC61937_DATATYPE_DOLBY_TRUE_HD:  /* spdif can't transmit truehd */
            return LBR;

        case IEC61937_DATATYPE_71_LPCM:  /* spdif can't transmit 7.1 pcm */
            return LBR;

        default:
            HI_LOG_WARN("failed to judge edid cabability of format %d\n", format);
            return PCM;
    }
}

hi_u32 track_get_spdif_auto_mode(snd_track_stream_attr *attr, hi_ao_arc_audio_cap *user_arc_cap)
{
    hi_u32 spdif_mode = TYPE_MAX;

    if (autil_is_hbr_format(attr->hbr_format)) {
        spdif_mode = track_arc_edid_change(user_arc_cap, autil_get_passth_format(attr->hbr_format));
        if (spdif_mode == HBR) {
            return spdif_mode;
        }
    }

    if (autil_is_lbr_format(attr->lbr_format)) {
        spdif_mode = track_arc_edid_change(user_arc_cap, autil_get_passth_format(attr->lbr_format));
    }

    return spdif_mode;
}

hi_u32 track_get_non_arc_spdif_mode(snd_track_stream_attr *attr, hi_ao_ouput_mode user_spdif_mode)
{
    if ((user_spdif_mode == HI_AO_OUTPUT_MODE_LPCM) ||
        (autil_is_lbr_format(attr->lbr_format) == HI_FALSE) ||
        (autil_get_passth_format(attr->lbr_format) == IEC61937_DATATYPE_20_LPCM)) {
        return PCM;
    } else {
        return LBR;
    }
}

hi_u32 track_get_arc_spdif_mode(snd_track_stream_attr *attr, hi_ao_arc_audio_cap *user_arc_cap,
    hi_ao_ouput_mode user_arc_mode)
{
    hi_bool lbr_format;
    hi_bool hbr_format;
    hi_u32 tmp_mode = PCM;

    lbr_format = autil_is_lbr_format(attr->lbr_format);
    hbr_format = autil_is_hbr_format(attr->hbr_format);
    /* no raw data at stream */
    if ((lbr_format == HI_FALSE) && (hbr_format == HI_FALSE)) {
        tmp_mode = PCM;
        return tmp_mode;
    }

    if (user_arc_mode == HI_AO_OUTPUT_MODE_LPCM) {
        tmp_mode = PCM;
    } else if (user_arc_mode == HI_AO_OUTPUT_MODE_RAW) {
        tmp_mode = (hbr_format == HI_TRUE) ? HBR : LBR;
    } else if (user_arc_mode == HI_AO_OUTPUT_MODE_HBR2LBR) {
        tmp_mode = (lbr_format == HI_TRUE) ? LBR : PCM;
    } else {
        tmp_mode = track_get_spdif_auto_mode(attr, user_arc_cap);
    }

    /* spdif can't transmit DTSHD/TRUEHD/7.1 pcm */
    if ((tmp_mode == HBR) && (autil_is_arc_support_hbr(attr->hbr_format) == HI_FALSE)) {
        tmp_mode = (lbr_format == HI_TRUE) ? LBR : PCM;
    }

    return tmp_mode;
}

static hi_void track_get_spdif_mode(snd_card_state *card,
    snd_track_stream_attr *attr, hi_u32 *mode)
{
    if (card->user_arc_enable == HI_FALSE) {
        *mode = track_get_non_arc_spdif_mode(attr, card->user_spdif_mode);
    } else {
        *mode = track_get_arc_spdif_mode(attr, &card->user_arc_cap, card->user_arc_mode);
    }
}

#elif defined(HI_SND_SPDIF_SUPPORT)

static hi_void track_get_spdif_mode(snd_card_state *card, snd_track_stream_attr *attr,
    hi_u32 *mode)
{
    if ((card->user_spdif_mode == HI_AO_OUTPUT_MODE_LPCM) ||
        (autil_is_lbr_format(attr->lbr_format) == HI_FALSE) ||
        (autil_get_passth_format(attr->lbr_format) == IEC61937_DATATYPE_20_LPCM)) {
        *mode = PCM;
    } else {
        *mode = LBR;
    }
}
#endif

#if defined(HI_SND_SPDIF_SUPPORT)
static inline stream_mode_change track_get_spdif_change_mode(snd_card_state *card,
    snd_track_stream_attr *attr_old, snd_track_stream_attr *attr)
{
    hi_u32 mode = TYPE_MAX;

    track_get_spdif_mode(card, attr, &mode);

    return track_get_spdif_change(card, attr_old, attr, mode);
}
#endif

#if defined(HI_SND_HDMI_SUPPORT) || defined(HI_SND_SPDIF_SUPPORT)
static hi_void track_router_build_ref_frame(router_context *router)
{
    snd_track_stream_attr *track_stream = router->track_stream;

    switch (STREAM_CHANGE_TO(router->mode_change)) {
        case LBR:
            router->ref_frame.bit_depth = track_stream->lbr_bit_depth;
            router->ref_frame.channels = track_stream->lbr_channels;
            router->ref_frame.sample_rate = track_stream->lbr_sample_rate;
            router->ref_frame.format = track_stream->lbr_format;
            break;

        case HBR:
            router->ref_frame.bit_depth = track_stream->hbr_bit_depth;
            router->ref_frame.channels = track_stream->hbr_channels;
            router->ref_frame.sample_rate = track_stream->hbr_sample_rate;
            router->ref_frame.format = track_stream->hbr_format;
            break;

        default:
            router->ref_frame.bit_depth = track_stream->pcm_bit_depth;
            router->ref_frame.channels = track_stream->pcm_channels;
            router->ref_frame.sample_rate = track_stream->pcm_sample_rate;
            router->ref_frame.format = 0;
            break;
    }
}

static hi_void track_router_get_engine_info(router_context *router)
{
    hi_u32 data_type = STREAM_CHANGE_FROM(router->mode_change);
    router->engine_old = ao_engine_get_id_by_type(router->card, data_type);

    data_type = STREAM_CHANGE_TO(router->mode_change);
    router->engine_new = ao_engine_get_id_by_type(router->card, data_type);
}

static hi_void track_router_get_aip_info(router_context *router)
{
    hi_u32 data_type;

    data_type = STREAM_CHANGE_FROM(router->mode_change);
    router->aip_old = router->track->aip[data_type];

    data_type = STREAM_CHANGE_TO(router->mode_change);
    router->aip_new = router->track->aip[data_type];

    hal_aoe_aip_get_status(router->aip_old, &router->aip_status);

    switch (data_type) {
        case LBR:
            router->aip_type_new = AOE_AIP_TYPE_LBR;
            break;

        case HBR:
            router->aip_type_new = AOE_AIP_TYPE_HBR;
            break;

        default:
            router->aip_type_new = AOE_AIP_TYPE_PCM_DMX;
            break;
    }
}

static hi_s32 track_router_get_op_info(router_context *router)
{
    router->snd_op = snd_get_op_handle_by_out_port(router->card, router->out_port);
    if (router->snd_op == HI_NULL) {
        HI_LOG_ERR("snd_get_op_handle_by_out_port return null\n");
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    snd_get_delay_ms(router->card, &router->op_target_delay);

    return HI_SUCCESS;
}

static hi_void track_router_update_op_attr(router_context *router, snd_op_attr *op_attr)
{
    op_attr->data_type = STREAM_CHANGE_TO(router->mode_change);

    if (op_attr->data_type == PCM) {
        /* in pcm mode, op attr is fixed */
        op_attr->bit_per_sample = AO_TRACK_BITDEPTH_LOW;
        op_attr->sample_rate = router->card->user_sample_rate;
        op_attr->channels = AO_TRACK_NORMAL_CHANNELNUM;
        op_attr->data_format = PCM;
    } else {
        /* in passthrough mode, op attr follows track stream attr */
        op_attr->bit_per_sample = router->ref_frame.bit_depth;
        op_attr->sample_rate = router->ref_frame.sample_rate;
        op_attr->channels = router->ref_frame.channels;
        op_attr->data_format = router->ref_frame.format;
    }
}

static hi_void track_router_proccess_op(router_context *router)
{
    hi_s32 ret;
    aoe_aop_id aop_id;
    snd_op_attr op_attr;

    aop_id = snd_get_op_aop_id(router->snd_op);

    /*
     * detach aop to make us safe
     * aop will attach to new engine after new engine is ready
     * see track_router_finish
     */
    hal_aoe_engine_detach_aop(router->engine_old, aop_id);

    ret = snd_op_stop(router->snd_op);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_stop, ret);
        return;
    }

    ret = snd_op_get_attr(router->snd_op, &op_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_get_attr, ret);
        return;
    }

    track_router_update_op_attr(router, &op_attr);

    ret = snd_op_set_attr(router->snd_op, &op_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_attr, ret);
        return;
    }

    ret = snd_op_start(router->snd_op);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_start, ret);
        return;
    }

#if defined (HI_SOUND_PORT_DELAY_SUPPORT)
    ret = snd_set_delay_compensation(router->card, router->snd_op->out_port, router->snd_op->delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_set_delay_compensation, ret);
        return;
    }
#endif
}

static hi_bool track_router_check_aip(router_context *router, aoe_aip_chn_attr *aip_attr)
{
    return (
        aip_attr->buf_in_attr.buf_bit_per_sample == router->ref_frame.bit_depth &&
        aip_attr->buf_in_attr.buf_sample_rate == router->ref_frame.sample_rate &&
        aip_attr->buf_in_attr.buf_channels == router->ref_frame.channels &&
        aip_attr->buf_in_attr.buf_data_format == router->ref_frame.format
        );
}

static hi_void track_router_proccess_aip(router_context *router)
{
    hi_s32 ret;
    aoe_aip_chn_attr aip_attr;

    ret = hal_aoe_aip_get_attr(router->aip_new, &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_get_attr, ret);
        HI_ERR_PRINT_H32(router->aip_new);
        return;
    }

    /* someone is already using this aip and aip attr do not need to change */
    if (hal_aoe_engine_get_aop_attach_mask(router->engine_new) != 0 &&
        track_router_check_aip(router, &aip_attr) == HI_TRUE) {
        router->resume_aip_delay = HI_FALSE;
        return;
    }

    aip_attr.buf_in_attr.buf_bit_per_sample = router->ref_frame.bit_depth;
    aip_attr.buf_in_attr.buf_sample_rate = router->ref_frame.sample_rate;
    aip_attr.buf_in_attr.buf_channels = router->ref_frame.channels;
    aip_attr.buf_in_attr.buf_data_format = router->ref_frame.format;
    aip_attr.buf_in_attr.aip_type = router->aip_type_new;

    aip_attr.fifo_out_attr.fifo_bit_per_sample = router->ref_frame.bit_depth;
    aip_attr.fifo_out_attr.fifo_sample_rate = router->ref_frame.sample_rate;
    aip_attr.fifo_out_attr.fifo_channels = router->ref_frame.channels;
    aip_attr.fifo_out_attr.fifo_data_format = router->ref_frame.format;

    /*
     * stop aip to make us safe
     * aip will start after new engine is ready
     * see track_router_finish
     */
    ret = hal_aoe_aip_stop(router->aip_new);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_stop, ret);
        HI_ERR_PRINT_H32(router->aip_new);
        return;
    }

    ret = hal_aoe_aip_set_attr(router->aip_new, &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_attr, ret);
        HI_ERR_PRINT_H32(router->aip_new);
        return;
    }

    router->resume_aip_delay = (router->aip_status != AOE_AIP_STATUS_STOP);
}

static hi_bool track_router_check_engine(router_context *router, aoe_engine_attr *engine_attr)
{
    return (
        engine_attr->bit_per_sample == router->ref_frame.bit_depth &&
        engine_attr->channels == router->ref_frame.channels &&
        engine_attr->sample_rate == router->ref_frame.sample_rate &&
        engine_attr->data_format == router->ref_frame.format
        );
}

static hi_void track_router_proccess_engine(router_context *router)
{
    hi_s32 ret;
    aoe_engine_attr engine_attr;

    /* pcm engine attr is fixed at engine create */
    if (STREAM_CHANGE_TO(router->mode_change) == PCM) {
        return;
    }

    ret = hal_aoe_engine_get_attr(router->engine_new, &engine_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_attr, ret);
        HI_ERR_PRINT_H32(router->engine_new);
        return;
    }

    if (track_router_check_engine(router, &engine_attr) == HI_TRUE) {
        /* so lucky that engine attr do not need to change */
        return;
    }

    ret = hal_aoe_engine_stop(router->engine_new);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_stop, ret);
        HI_ERR_PRINT_H32(router->engine_new);
        return;
    }

    engine_attr.bit_per_sample = router->ref_frame.bit_depth;
    engine_attr.channels = router->ref_frame.channels;
    engine_attr.sample_rate = router->ref_frame.sample_rate;
    engine_attr.data_format = router->ref_frame.format;

    ret = hal_aoe_engine_set_attr(router->engine_new, &engine_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_attr, ret);
        HI_ERR_PRINT_H32(router->engine_new);
        return;
    }

    ret = hal_aoe_engine_start(router->engine_new);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_start, ret);
        HI_ERR_PRINT_H32(router->engine_new);
        return;
    }
}

static inline hi_void track_router_init(router_context *router)
{
    hi_s32 ret;

    track_router_build_ref_frame(router);
    track_router_get_engine_info(router);
    track_router_get_aip_info(router);

    ret = track_router_get_op_info(router);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_router_get_op_info, ret);
    }
}

static inline hi_void track_router_finish(router_context *router)
{
    hi_s32 ret;
    hi_u32 aip_target_delay = 0;
    aoe_aop_id aop_id = snd_get_op_aop_id(router->snd_op);

    /* resume aip delay */
    if (router->resume_aip_delay == HI_TRUE) {
        ret = track_get_aip_delay_ms(router->card, router->track, &aip_target_delay);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_get_aip_delay_ms, ret);
            aip_target_delay = 0;
        }
        hal_aoe_aip_add_mute_at_read(router->aip_new, aip_target_delay);
    }

    /* resume aip status after route */
    if (router->aip_status == AOE_AIP_STATUS_START) {
        ret = hal_aoe_aip_start(router->aip_new);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_start, ret);
            HI_ERR_PRINT_H32(router->aip_new);
            return;
        }
    }

    /* resume aop delay */
    snd_op_add_mute(router->snd_op, router->op_target_delay);
    hal_aoe_engine_attach_aop(router->engine_new, aop_id);

    /* aip_old and engine_old have no output port after route process, we need to flush aip */
    if (hal_aoe_engine_get_aop_attach_mask(router->engine_old) == 0) {
        ret = hal_aoe_aip_flush(router->aip_old);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_flush, ret);
            HI_ERR_PRINT_H32(router->aip_old);
            return;
        }
    }
}

static inline hi_void track_router_process(router_context *router)
{
    track_router_proccess_op(router);
    track_router_proccess_aip(router);
    track_router_proccess_engine(router);
    track_router_finish(router);
}
#endif

hi_void detect_stream_mode_change(snd_card_state *card, snd_track_state *track,
    snd_track_stream_attr *attr, stream_mode_change_attr *change)
{
    snd_track_stream_attr *attr_old = &track->stream_attr;

    change->pcm_change = STREAM_CHANGE_NONE;
    change->spdif_change = STREAM_CHANGE_NONE;
    change->hdmi_change = STREAM_CHANGE_NONE;

    /* pcm stream attr(image that pcm_bit_depth & pcm_sample_rate is the same for dmx & mc & assoc) */
    if ((attr_old->pcm_bit_depth != attr->pcm_bit_depth) ||
        (attr_old->pcm_sample_rate != attr->pcm_sample_rate) ||
        (attr_old->pcm_channels != attr->pcm_channels)) {
        change->pcm_change = STREAM_CHANGE_PCM2PCM;
    }

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
#if defined(HI_SND_SPDIF_SUPPORT)
        if (card->spdif_passthrough != TYPE_MAX) {
            change->spdif_change = track_get_spdif_change_mode(card, attr_old, attr);
        }
#endif

#if defined(HI_SND_HDMI_SUPPORT)
        if (card->hdmi_passthrough != TYPE_MAX) {
            change->hdmi_change = track_get_hdmi_change_mode(card, attr_old, attr);
        }
#endif
    }
}

hi_void snd_proc_pcm_route(snd_card_state *card, snd_track_state *track,
    stream_mode_change mode, snd_track_stream_attr *attr)
{
    hi_s32 ret;
    aoe_aip_chn_attr aip_attr;

    /*
     * only start status aip triger pcm route
     * we can simply stop aip, change aip attr and then start aip here
     */
    ret = hal_aoe_aip_stop(track->aip[PCM]);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_stop, ret);
        return;
    }

    ret = hal_aoe_aip_get_attr(track->aip[PCM], &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_get_attr, ret);
        return;
    }

    aip_attr.buf_in_attr.buf_bit_per_sample = attr->pcm_bit_depth;
    aip_attr.buf_in_attr.buf_sample_rate = attr->pcm_sample_rate;
    aip_attr.buf_in_attr.buf_channels = attr->pcm_channels;
    aip_attr.buf_in_attr.buf_data_format = 0;

    ret = hal_aoe_aip_set_attr(track->aip[PCM], &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_get_attr, ret);
        return;
    }

    ret = hal_aoe_aip_start(track->aip[PCM]);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_start, ret);
        return;
    }
}

#if defined(HI_SND_HDMI_SUPPORT)
/* verify should simple, can change hdmi attr according to op attr */
static hi_void hdmi_audio_change(snd_card_state *card, stream_mode_change mode,
                                 snd_track_stream_attr *attr)
{
    hi_u32 channels;
    hi_u32 rate;

    hi_u32 hdmi_ao_intf;
    hi_u32 audio_format = HI_AUDIO_FORMAT_CXT;
    hdmi_ao_attr ao_attr;

    if (STREAM_CHANGE_TO_LBR(mode)) {
        hi_u32 lbr_format = attr->lbr_format & 0xff;
        channels = attr->lbr_channels;
        rate = attr->lbr_sample_rate;
        hdmi_ao_intf = HDMI_AO_INTERFACE_SPDIF;

        /* set hdmi_audio_interface_spdif audio codec type */
        if (lbr_format == IEC61937_DATATYPE_DOLBY_DIGITAL) {
            audio_format = HI_AUDIO_FORMAT_AC3;
        } else if ((lbr_format == IEC61937_DATATYPE_DTS_TYPE_I) ||
                   (lbr_format == IEC61937_DATATYPE_DTS_TYPE_II) ||
                   (lbr_format == IEC61937_DATATYPE_DTS_TYPE_III)) {
            audio_format = HI_AUDIO_FORMAT_DTS;
        } else if (lbr_format == IEC61937_DATATYPE_20_LPCM) {
            channels = attr->lbr_channels;
            hdmi_ao_intf = HDMI_AO_INTERFACE_I2S;
            audio_format = HI_AUDIO_FORMAT_PCM;
            rate = attr->pcm_sample_rate;
        }
    } else if (STREAM_CHANGE_TO_HBR(mode)) {
        hi_u32 hbr_format = attr->hbr_format & 0xff;
        rate = attr->hbr_sample_rate;
        channels = attr->hbr_channels;
        hdmi_ao_intf = HDMI_AO_INTERFACE_HBR;

        if (hbr_format == IEC61937_DATATYPE_71_LPCM) {
            channels = attr->org_multi_pcm_channels;
            hdmi_ao_intf = HDMI_AO_INTERFACE_I2S;
            audio_format = HI_AUDIO_FORMAT_PCM;
            rate = attr->pcm_sample_rate;
        } else if (hbr_format == IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS) {
            hdmi_ao_intf = HDMI_AO_INTERFACE_SPDIF;
            audio_format = HI_AUDIO_FORMAT_EAC3;
        } else if (hbr_format == IEC61937_DATATYPE_DTS_TYPE_IV) {
            audio_format = HI_AUDIO_FORMAT_DTS_HD;
        }
    } else if (STREAM_CHANGE_TO_PCM(mode)) {
        rate = card->user_sample_rate;
        channels = AO_TRACK_NORMAL_CHANNELNUM;
        hdmi_ao_intf = HDMI_AO_INTERFACE_I2S;
        audio_format = HI_AUDIO_FORMAT_PCM;
    } else {
        return;
    }

#ifdef HI_SND_HDMI_PASSTHROUGH_USE_I2S_ONLY
    if (hdmi_ao_intf == HDMI_AO_INTERFACE_SPDIF) {
        hdmi_ao_intf = HDMI_AO_INTERFACE_I2S;
    }
#endif

    ao_attr.audio_format = audio_format;
    ao_attr.interface = hdmi_ao_intf;
    ao_attr.channels = channels;
    ao_attr.sample_rate = rate;
    ao_attr.bit_depth = HI_I2S_BIT_DEPTH_16;

    ao_snd_hdmi_set_ao_attr(card, card->hdmi_port, &ao_attr);
}

hi_void snd_proc_hdmi_route(snd_card_state *card, snd_track_state *track,
    stream_mode_change mode_change, snd_track_stream_attr *track_stream)
{
    router_context router = {
        .card = card,
        .track = track,
        .track_stream = track_stream,
        .mode_change = mode_change,
        .aip_status = AIP_STATUS_MAX,
        .out_port = card->hdmi_port,
        .snd_op = HI_NULL,
        .op_target_delay = 0,
    };

    ao_snd_hdmi_set_mute(card, router.out_port, HI_TRUE);

    track_router_init(&router);
    track_router_process(&router);

    card->hdmi_data_format = router.ref_frame.format;
    card->hdmi_passthrough = STREAM_CHANGE_TO(mode_change);

    hdmi_audio_change(card, mode_change, track_stream);

    ao_snd_hdmi_set_mute(card, router.out_port, HI_FALSE);
}
#endif

#if defined(HI_SND_SPDIF_SUPPORT)
hi_void snd_proc_spidf_route(snd_card_state *card, snd_track_state *track,
    stream_mode_change mode_change, snd_track_stream_attr *track_stream)
{
    router_context router = {
        .card = card,
        .track = track,
        .track_stream = track_stream,
        .mode_change = mode_change,
        .aip_status = AIP_STATUS_MAX,
        .out_port = HI_AO_PORT_SPDIF0,
        .snd_op = HI_NULL,
        .op_target_delay = 0,
    };

    track_router_init(&router);
    track_router_process(&router);

    card->spdif_data_format = router.ref_frame.format;
    card->spdif_passthrough = STREAM_CHANGE_TO(mode_change);
}
#endif

