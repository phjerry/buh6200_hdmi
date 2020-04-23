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

#include "audio_util.h"
#include "drv_ao_engine.h"
#include "track_func.h"
#include "track_router.h"

#if defined(HI_AUDIO_AI_SUPPORT)
#include "drv_ai_private.h"
#endif

#define MAX_SAVE_ALSA_SIZE (1024 << 10)
#define TRACK_SAVE_TMP_SIZE (8 << 10) /* 8k tmp buffer */

/* always use this to get track from card */
#define TRACK_FIND_BY_ID(card, track_id, track)           \
    do {                                                  \
        (track) = __track_find_by_id((card), (track_id)); \
        if ((track) == HI_NULL) {                         \
            HI_LOG_ERR("track state is NULL\n");                 \
            HI_ERR_PRINT_H32((track_id));                 \
            return HI_ERR_AO_NULL_PTR;                    \
        }                                                 \
    } while (0)

static snd_track_state *__track_find_by_id(snd_card_state *card, hi_u32 track_id)
{
    snd_track_state *track = HI_NULL;

    osal_list_for_each_entry(track, &card->track, node) {
        if (track->track_id == track_id) {
            return track;
        }
    }

    return HI_NULL;
}

snd_track_state *track_find_by_handle(snd_card_state *card, hi_handle h_track)
{
    snd_track_state *track = HI_NULL;

    osal_list_for_each_entry(track, &card->track, node) {
        if (track->h_track == h_track) {
            return track;
        }
    }

    return HI_NULL;
}

static hi_s32 master_track_malloc_aip_buf(snd_card_state *card, snd_track_state *state)
{
    hi_s32 ret;

#ifdef HI_SND_DSP_SUPPORT
    hi_bool cache = HI_FALSE;
#else
    hi_bool cache = HI_TRUE;
#endif

    ret = hi_drv_audio_smmu_alloc("ao_m_aip_pcm", AO_TRACK_PCM_BUFSIZE_BYTE_MAX, cache, &state->aip_rbf_mmz[PCM]);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_audio_smmu_alloc, ret);
        return ret;
    }

    if (card->lbr_device == HI_TRUE) {
        ret = hi_drv_audio_smmu_alloc("ao_m_aip_lbr", AO_TRACK_LBR_BUFSIZE_BYTE_MAX, cache, &state->aip_rbf_mmz[LBR]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hi_drv_audio_smmu_alloc, ret);
            goto out0;
        }
    }

    if (card->hbr_device == HI_TRUE) {
        ret = hi_drv_audio_smmu_alloc("ao_m_aip_hbr", AO_TRACK_HBR_BUFSIZE_BYTE_MAX, cache, &state->aip_rbf_mmz[HBR]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hi_drv_audio_smmu_alloc, ret);
            goto out1;
        }
    }

    return HI_SUCCESS;

out1:
    if (card->lbr_device == HI_TRUE) {
        hi_drv_audio_smmu_release(&state->aip_rbf_mmz[LBR]);
    }

out0:
    hi_drv_audio_smmu_release(&state->aip_rbf_mmz[PCM]);
    return ret;
}

static hi_void master_track_free_aip_buf(snd_card_state *card, snd_track_state *state)
{
    hi_drv_audio_smmu_release(&state->aip_rbf_mmz[PCM]);

    if (card->lbr_device == HI_TRUE) {
        hi_drv_audio_smmu_release(&state->aip_rbf_mmz[LBR]);
    }

    if (card->hbr_device == HI_TRUE) {
        hi_drv_audio_smmu_release(&state->aip_rbf_mmz[HBR]);
    }
}

static hi_u32 track_get_multi_pcm_channels(ao_frame *frame)
{
    hi_u32 mul_channel;
    mul_channel = frame->channels & 0xff;

    if (mul_channel > AO_TRACK_NORMAL_CHANNELNUM) {
        /* compatible the former action */
        return (mul_channel - AO_TRACK_NORMAL_CHANNELNUM);
    } else {
        /* AOEIMP: TODO, add channel distribution diagram */
        /* follow the new action(such as the channels of ms12) */
        mul_channel = (frame->channels & 0xff00) >> 8;

        if (mul_channel > AO_TRACK_NORMAL_CHANNELNUM) {
            return mul_channel - AO_TRACK_NORMAL_CHANNELNUM;
        }

        return 0;
    }
}

static inline hi_bool track_is_pcm_multi_channels(ao_frame *frame)
{
    return (track_get_multi_pcm_channels(frame) > 0);
}

#ifdef HI_PROC_SUPPORT
static hi_void track_proc_save_data_stop(snd_track_state *track);

static hi_s32 save_alsa_thread(hi_void *args)
{
    hi_u8 *save_buf = HI_NULL;
    hi_u32 dest_size = 0;
    hi_u32 bytes = 0;
    snd_track_state *track = (snd_track_state *)args;
    if (track == HI_NULL) {
        HI_LOG_ERR("track state is NULL!\n");
        return HI_FAILURE;
    }

    if (track->file_handle == HI_NULL) {
        HI_LOG_ERR("track->file_handle is NULL!\n");
        return HI_FAILURE;
    }

    save_buf = osal_vmalloc(HI_ID_AO, MAX_SAVE_ALSA_SIZE);
    if (save_buf == HI_NULL) {
        HI_LOG_ERR("malloc save buffer failed\n");
        return HI_FAILURE;
    }

    /* warning: do not return or break from the while() */
    while (!osal_kthread_should_stop()) {
        dest_size = circ_buf_query_busy(&track->save_alsa.cb);
        if (dest_size) {
            bytes = circ_buf_read(&track->save_alsa.cb, save_buf, dest_size);
            if (bytes != dest_size) {
                HI_ERR_PRINT_U32(bytes);
                HI_ERR_PRINT_U32(dest_size);
            }
            osal_klib_fwrite(save_buf, bytes, track->file_handle);
        } else {
            osal_msleep(5);
        }
    }

    osal_vfree(HI_ID_AO, save_buf);

    return HI_SUCCESS;
}
#endif

static hi_u32 track_get_dmx_pcm_size(ao_frame *frame)
{
    hi_u32 channels = frame->channels;

    if (track_is_pcm_multi_channels(frame)) {
        channels = AO_TRACK_NORMAL_CHANNELNUM;
    } else {
        channels = channels & 0xff;
    }

    return frame->pcm_samples * autil_calc_frame_size(channels, frame->bit_depth);
}

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
static hi_u32 track_get_multi_pcm_size(ao_frame *frame)
{
    if (track_is_pcm_multi_channels(frame)) {
        /* always 8 ch */
        return frame->pcm_samples * autil_calc_frame_size(AO_TRACK_MUTILPCM_CHANNELNUM, frame->bit_depth);
    } else {
        return 0;
    }
}
#endif

static inline hi_u32 track_get_lbr_size(ao_frame *frame)
{
    return (frame->bits_bytes & 0x7fff);
}

static hi_u32 track_get_hbr_size(ao_frame *frame)
{
    hi_u32 hbr_raw_bytes = 0;

    if (frame->bits_bytes & 0xffff0000) {
        hbr_raw_bytes = (frame->bits_bytes >> 16);
    } else {
        /* HBR is truehd */
        hbr_raw_bytes = (frame->bits_bytes & 0xffff);
    }

    return hbr_raw_bytes;
}

static hi_u32 track_get_dmx_pcm_channels(ao_frame *frame)
{
    hi_u32 dmx_pcm_ch = frame->channels;
    dmx_pcm_ch = dmx_pcm_ch & 0xff;

    if ((dmx_pcm_ch == AO_TRACK_NORMAL_CHANNELNUM) || (dmx_pcm_ch == 1)) {
        return dmx_pcm_ch;
    } else if (dmx_pcm_ch > AO_TRACK_NORMAL_CHANNELNUM) {
        return AO_TRACK_NORMAL_CHANNELNUM;
    } else {
        return 0;
    }
}

static hi_bool track_is_pcm_hdmi_ouput_sample_rate(hi_u32 sample_rate)
{
    hi_u32 i;
    const hi_u32 sample_rate_table[] = {
        HI_SAMPLE_RATE_32K,
        HI_SAMPLE_RATE_44K,
        HI_SAMPLE_RATE_48K,
        HI_SAMPLE_RATE_88K,
        HI_SAMPLE_RATE_96K,
        HI_SAMPLE_RATE_176K,
        HI_SAMPLE_RATE_192K,
    };

    for (i = 0; i < ARRAY_SIZE(sample_rate_table); i++) {
        if (sample_rate_table[i] == sample_rate) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static inline hi_void *track_get_pcm_buf_addr(ao_frame *frame)
{
    return (hi_void *)((hi_u8 *)HI_NULL + frame->pcm_buffer);
}

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
/*
 * |----interleaved dmx 2.0 frame----|--interleaved multi 7.1 frame--|----interleaved assoc 2.0 frame----|
 * |----interleaved 7.1-----------------------------|
 * |----interleaved 5.1----------------- padding 0/0|
 */
static hi_void *track_get_multi_pcm_addr(ao_frame *frame)
{
    hi_void *base = (hi_void *)((hi_u8 *)HI_NULL + frame->pcm_buffer);

    if (track_is_pcm_multi_channels(frame) == HI_FALSE) {
        return HI_NULL;
    } else {
        /* dmx always 2 ch */
        return base + frame->pcm_samples * autil_calc_frame_size(AO_TRACK_NORMAL_CHANNELNUM, frame->bit_depth);
    }
}
#endif

hi_void *track_get_lbr_buf_addr(ao_frame *frame)
{
    if (frame->bits_bytes & 0x7fff) {
        return (hi_void *)((hi_u8 *)HI_NULL + frame->bits_buffer);
    }

    return HI_NULL;
}

hi_void *track_get_hbr_buf_addr(ao_frame *frame)
{
    hi_void *addr = (hi_void *)((hi_u8 *)HI_NULL + frame->bits_buffer);

    if (frame->bits_bytes & 0xffff0000) {
        addr += (frame->bits_bytes & 0x7fff);
    }

    return addr;
}

/* for both passthrough-only(no pcm output) and simul mode */
hi_void track_build_pcm_attr(ao_frame *frame, snd_track_stream_attr *stream_attr)
{
    if (frame->pcm_samples) {
        stream_attr->pcm_sample_rate = frame->sample_rate;
        stream_attr->pcm_bit_depth = frame->bit_depth;
        stream_attr->pcm_samples = frame->pcm_samples;
        stream_attr->pcm_bytes = track_get_dmx_pcm_size(frame);
        stream_attr->pcm_channels = track_get_dmx_pcm_channels(frame);
        stream_attr->pcm_data_buf = track_get_pcm_buf_addr(frame);
    } else {
        hi_u32 bit_width = sizeof(hi_u16);

        if (frame->bit_depth != HI_BIT_DEPTH_16) {
            bit_width = sizeof(hi_u32);
        }

        if (stream_attr->lbr_data_buf != HI_NULL) {
            stream_attr->pcm_sample_rate = frame->sample_rate;
            stream_attr->pcm_bit_depth = frame->bit_depth;
            stream_attr->pcm_bytes = stream_attr->lbr_bytes;
            stream_attr->pcm_channels = AO_TRACK_NORMAL_CHANNELNUM;
            if (stream_attr->pcm_channels == 0) {
                stream_attr->pcm_samples = 0;
            } else {
                stream_attr->pcm_samples = stream_attr->pcm_bytes / stream_attr->pcm_channels / bit_width;
            }
            stream_attr->pcm_data_buf = (hi_void *)HI_NULL;
        } else if (stream_attr->hbr_data_buf != HI_NULL) {
            hi_u32 hbr_samples_per_frame = 0;

            if (stream_attr->hbr_channels != 0) {
                hbr_samples_per_frame = stream_attr->hbr_bytes / stream_attr->hbr_channels / bit_width;
            }

            stream_attr->pcm_samples = hbr_samples_per_frame >> 2;
            stream_attr->pcm_sample_rate = frame->sample_rate;
            stream_attr->pcm_bit_depth = frame->bit_depth;
            stream_attr->pcm_channels = AO_TRACK_NORMAL_CHANNELNUM;
            stream_attr->pcm_bytes = stream_attr->pcm_samples * stream_attr->pcm_channels * bit_width;
            stream_attr->pcm_data_buf = (hi_void *)HI_NULL;
        } else {
            stream_attr->pcm_sample_rate = HI_SAMPLE_RATE_48K;
            stream_attr->pcm_bit_depth = AO_TRACK_BITDEPTH_LOW;
            stream_attr->pcm_channels = AO_TRACK_NORMAL_CHANNELNUM;
            stream_attr->pcm_bytes = 0;
            stream_attr->pcm_samples = 0;
            stream_attr->pcm_data_buf = (hi_void *)HI_NULL;
        }
    }
}

static hi_void track_build_lbr_attr(ao_frame *frame, snd_track_stream_attr *stream_attr)
{
    hi_u32 iec_data_type;
    hi_u32 lbr_size;
    hi_void *lbr_addr = HI_NULL;

    if (autil_is_truehd(frame->iec_data_type)) {
        return;
    }

    lbr_size = track_get_lbr_size(frame);
    lbr_addr = track_get_lbr_buf_addr(frame);
    iec_data_type = autil_iec61937_data_type((hi_u16 *)lbr_addr, lbr_size);
    if (autil_is_iec61937_lbr(iec_data_type, frame->sample_rate) == HI_FALSE) {
        return;
    }

    stream_attr->lbr_bit_depth = AO_TRACK_BITDEPTH_LOW;
    stream_attr->lbr_channels = AO_TRACK_NORMAL_CHANNELNUM;
    stream_attr->lbr_sample_rate = frame->sample_rate;
    stream_attr->lbr_data_buf = lbr_addr;
    stream_attr->lbr_bytes = track_get_lbr_size(frame);
    stream_attr->lbr_format = autil_get_iec_data_type(iec_data_type, frame->bits_bytes);
}

static hi_u32 track_get_hbr_samplerate(hi_u32 sample_rate)
{
    hi_u32 hbr_sample_rate;

    if (sample_rate <= HI_SAMPLE_RATE_48K) {
        hbr_sample_rate = sample_rate * 4; /* hbr 4*samplerate */
    } else if ((sample_rate == HI_SAMPLE_RATE_88K) || (sample_rate == HI_SAMPLE_RATE_96K)) {
        hbr_sample_rate = sample_rate * 2;
    } else {
        hbr_sample_rate = sample_rate; /* hbr samplerate */
    }

    return hbr_sample_rate;
}

static hi_u32 track_get_hbr_chn(hi_u32 iec_data_type)
{
    if (iec_data_type == IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS) {
        return AO_TRACK_NORMAL_CHANNELNUM;
    }

    return AO_TRACK_MAX_CHANNELNUM;
}

static hi_void track_build_passth_pcm_attr(ao_frame *frame, snd_track_stream_attr *stream_attr)
{
    if ((stream_attr->lbr_format != IEC61937_DATATYPE_NULL) || (stream_attr->hbr_format != IEC61937_DATATYPE_NULL)) {
        return;
    }

    if (track_is_pcm_hdmi_ouput_sample_rate(frame->sample_rate) == HI_FALSE) {
        return;
    }

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
    if (track_is_pcm_multi_channels(frame) == HI_TRUE) {
        stream_attr->hbr_bit_depth = frame->bit_depth;
        stream_attr->hbr_sample_rate = frame->sample_rate;
        stream_attr->hbr_format = IEC61937_DATATYPE_71_LPCM;
        stream_attr->hbr_channels = AO_TRACK_MUTILPCM_CHANNELNUM;
        stream_attr->org_multi_pcm_channels = track_get_multi_pcm_channels(frame);
        stream_attr->hbr_bytes = track_get_multi_pcm_size(frame);
        stream_attr->hbr_data_buf = track_get_multi_pcm_addr(frame);
        return;
    }
#endif

    if (frame->sample_rate > HI_SAMPLE_RATE_48K) {
        hi_u32 channels = (frame->channels & 0xff);
        channels = min_t(hi_u32, channels, AO_TRACK_NORMAL_CHANNELNUM);

        stream_attr->lbr_bit_depth = frame->bit_depth;
        stream_attr->lbr_sample_rate = frame->sample_rate;
        stream_attr->lbr_format = IEC61937_DATATYPE_20_LPCM;
        stream_attr->lbr_data_buf = (hi_void *)((hi_u8 *)HI_NULL + frame->pcm_buffer);
        stream_attr->org_multi_pcm_channels = channels;
        stream_attr->lbr_channels = channels;
        stream_attr->lbr_bytes = frame->pcm_samples * autil_calc_frame_size(channels, frame->bit_depth);
    }
}

static hi_void track_build_hbr_attr(ao_frame *frame, snd_track_stream_attr *stream_attr)
{
    hi_u32 iec_data_type = 0;
    hi_void *hbr_addr = HI_NULL;

    hbr_addr = track_get_hbr_buf_addr(frame);

    if (autil_is_truehd(frame->iec_data_type)) {
#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
        iec_data_type = autil_get_truehd_data_type(frame->iec_data_type);
#endif
    } else {
        iec_data_type = autil_iec61937_data_type((hi_u16 *)hbr_addr, track_get_hbr_size(frame));
    }

    if (autil_is_iec61937_hbr(iec_data_type, frame->sample_rate) == HI_TRUE) {
        stream_attr->hbr_bit_depth = AO_TRACK_BITDEPTH_LOW;
        stream_attr->hbr_channels = track_get_hbr_chn(iec_data_type);
        stream_attr->hbr_format = autil_get_iec_data_type(iec_data_type, frame->bit_depth);
        stream_attr->hbr_sample_rate = track_get_hbr_samplerate(frame->sample_rate);
        stream_attr->hbr_bytes = track_get_hbr_size(frame);
        stream_attr->hbr_data_buf = hbr_addr;
    }
}

static hi_void track_build_stream_attr(ao_frame *frame, snd_track_stream_attr *stream_attr)
{
    stream_attr->lbr_format = IEC61937_DATATYPE_NULL;
    stream_attr->hbr_format = IEC61937_DATATYPE_NULL;

    track_build_lbr_attr(frame, stream_attr);

    track_build_hbr_attr(frame, stream_attr);

    track_build_passth_pcm_attr(frame, stream_attr);

    /*
     * pcm attr must build after LBR and HBR
     * ac3passthrough needs to calculate pcm frame length based on raw data
     * !!warning: salve track must build raw data to calculate pcm frame length in passthrough only lib condition
     */
    track_build_pcm_attr(frame, stream_attr);
}

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
static hi_void detect_true_hd_mode_change(snd_card_state *card, snd_track_stream_attr *attr)
{
    hi_u32 pcm_bit_width;
    hi_u32 sample_rate_width;
    hi_u32 hbr_bit_width;
    hi_u32 hbr_samples_per_frame;

    if ((attr->hbr_format & 0xff) != IEC61937_DATATYPE_DOLBY_TRUE_HD) {
        return;
    }

    if (card->hdmi_passthrough != HBR) {
        return;
    }

    pcm_bit_width = ((attr->pcm_bit_depth == HI_BIT_DEPTH_16) ? sizeof(hi_u16) : sizeof(hi_u32));
    hbr_bit_width = ((attr->hbr_bit_depth == HI_BIT_DEPTH_16) ? sizeof(hi_u16) : sizeof(hi_u32));

    if (attr->pcm_sample_rate <= HI_SAMPLE_RATE_48K) {
        sample_rate_width = 2;
    } else if ((attr->pcm_sample_rate == HI_SAMPLE_RATE_88K) || (attr->pcm_sample_rate == HI_SAMPLE_RATE_96K)) {
        sample_rate_width = 1;
    } else {
        sample_rate_width = 0;
    }

    hbr_samples_per_frame = attr->hbr_bytes / attr->hbr_channels / hbr_bit_width;
    attr->pcm_samples = hbr_samples_per_frame >> sample_rate_width;
    attr->pcm_channels = AO_TRACK_NORMAL_CHANNELNUM;
    attr->pcm_bytes = attr->pcm_samples * attr->pcm_channels * pcm_bit_width;
    attr->pcm_data_buf = HI_NULL;
}
#endif

static hi_bool track_check_lbr_aip_used(snd_card_state *card, snd_track_stream_attr *track_stream)
{
    snd_op_state *snd_op = HI_NULL;

    if (track_stream->lbr_format == IEC61937_DATATYPE_NULL) {
        return HI_FALSE;
    }

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if (snd_op->snd_port_attr.data_format == track_stream->lbr_format) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static hi_bool track_check_hbr_aip_used(snd_card_state *card, snd_track_stream_attr *track_stream)
{
    snd_op_state *snd_op = HI_NULL;

    if (track_stream->hbr_format == IEC61937_DATATYPE_NULL) {
        return HI_FALSE;
    }

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if (snd_op->snd_port_attr.data_format == track_stream->hbr_format) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static hi_bool track_is_buf_free(snd_card_state *card, snd_track_state *track, snd_track_stream_attr *stream_attr)
{
    hi_u32 free;
    hi_u32 delay_ms;
    hi_u32 frame_size;
    hi_u32 frame_ms;

    if (ao_snd_check_output(card, PCM) == HI_TRUE) {
        free = hal_aoe_aip_query_buf_free(track->aip[PCM]);
        if (free <= stream_attr->pcm_bytes) {
            return HI_FALSE;
        }

        hal_aoe_aip_get_buf_delay_ms(track->aip[PCM], &delay_ms);
        frame_size = autil_calc_frame_size(stream_attr->pcm_channels, stream_attr->pcm_bit_depth);
        frame_ms = autil_byte_size_to_latency_ms(stream_attr->pcm_bytes, frame_size, stream_attr->pcm_sample_rate);
        if (delay_ms + frame_ms >= track->user_track_attr.buf_level) {
            return HI_FALSE;
        }
    }

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
        if (track_check_lbr_aip_used(card, stream_attr)) {
            free = hal_aoe_aip_query_buf_free(track->aip[LBR]);
            if (free <= stream_attr->lbr_bytes) {
                return HI_FALSE;
            }

            hal_aoe_aip_get_buf_delay_ms(track->aip[LBR], &delay_ms);
            frame_size = autil_calc_frame_size(stream_attr->lbr_channels, stream_attr->lbr_bit_depth);
            frame_ms = autil_byte_size_to_latency_ms(stream_attr->lbr_bytes, frame_size, stream_attr->lbr_sample_rate);
            if (delay_ms + frame_ms >= track->user_track_attr.buf_level) {
                return HI_FALSE;
            }
        }

        if (track_check_hbr_aip_used(card, stream_attr)) {
            free = hal_aoe_aip_query_buf_free(track->aip[HBR]);
            if (free <= stream_attr->hbr_bytes) {
                return HI_FALSE;
            }

            hal_aoe_aip_get_buf_delay_ms(track->aip[HBR], &delay_ms);
            frame_size = autil_calc_frame_size(stream_attr->hbr_channels, stream_attr->hbr_bit_depth);
            frame_ms = autil_byte_size_to_latency_ms(stream_attr->hbr_bytes, frame_size, stream_attr->hbr_sample_rate);
            if (delay_ms + frame_ms >= track->user_track_attr.buf_level) {
                return HI_FALSE;
            }
        }
    }

    return HI_TRUE;
}

static hi_s32 track_aip_func(snd_track_state *track, hi_u32 status_mask, hi_s32 (*hal_aoe_aip_func)(hi_u32 mask))
{
    hi_s32 ret;
    hi_u32 mask = 0;
    hi_u32 type;
    aoe_aip_id aip_id;
    aoe_aip_status status;

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        hal_aoe_aip_get_status(aip_id, &status);

        if (status & status_mask) {
            mask |= (1 << aip_id);
        }
    }

    if (mask == 0) {
        return HI_SUCCESS;
    }

    ret = hal_aoe_aip_func(mask);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_func, ret);
        HI_ERR_PRINT_H32(mask);
    }

    return ret;
}

static inline hi_s32 track_pause_aip(snd_track_state *track)
{
    /* send pause command if aip is start */
    return track_aip_func(track, AOE_AIP_STATUS_START, hal_aoe_aip_group_pause);
}

static inline hi_s32 track_stop_aip(snd_track_state *track)
{
    /* send stop command if aip is start or pause */
    return track_aip_func(track, AOE_AIP_STATUS_START | AOE_AIP_STATUS_PAUSE, hal_aoe_aip_group_stop);
}

static inline hi_s32 track_start_aip(snd_track_state *track)
{
    /* send start command if aip is stop or pause */
    return track_aip_func(track, AOE_AIP_STATUS_STOP | AOE_AIP_STATUS_PAUSE, hal_aoe_aip_group_start);
}

static hi_void track_try_start_aip(snd_track_state *track)
{
    hi_s32 ret;

    if (track->curn_status == SND_TRACK_STATUS_START) {
        return;
    }

    ret = track_start_aip(track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_start_aip, ret);
        return;
    }
}

static hi_void track_set_eos_to_aip(snd_card_state *card, snd_track_state *track)
{
    if (track->eos_flag == HI_FALSE) {
        return;
    }

    if (ao_snd_check_output(card, PCM) == HI_TRUE) {
        hal_aoe_aip_set_eos(track->aip[PCM], HI_TRUE);
    }

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
        if (ao_snd_check_output(card, LBR) == HI_TRUE) {
            hal_aoe_aip_set_eos(track->aip[LBR], HI_TRUE);
        }

        if (ao_snd_check_output(card, HBR) == HI_TRUE) {
            hal_aoe_aip_set_eos(track->aip[HBR], HI_TRUE);
        }
    }
}

static hi_bool track_in_tplay_mode(snd_track_state *track)
{
    if (track->speed.type != HI_AO_SPEED_TPLAY) {
        return HI_FALSE;
    }

    if (track->speed.speed_int == 1 && track->speed.speed_dec == 0) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

static hi_void track_route_master(snd_card_state *card, snd_track_state *track, snd_track_stream_attr *stream_attr,
    stream_mode_change_attr *change)
{
    hi_s32 ret;

    if ((change->pcm_change == STREAM_CHANGE_NONE) && (change->spdif_change == STREAM_CHANGE_NONE) &&
        (change->hdmi_change == STREAM_CHANGE_NONE)) {
        return;
    }

    if (change->pcm_change != STREAM_CHANGE_NONE) {
        snd_proc_pcm_route(card, track, change->pcm_change, stream_attr);
    }

#if defined(HI_SND_SPDIF_SUPPORT)
    if (change->spdif_change != STREAM_CHANGE_NONE) {
        snd_proc_spidf_route(card, track, change->spdif_change, stream_attr);
    }
#endif

#if defined(HI_SND_HDMI_SUPPORT)
    if (change->hdmi_change != STREAM_CHANGE_NONE) {
        snd_proc_hdmi_route(card, track, change->hdmi_change, stream_attr);
    }
#endif

    track_set_eos_to_aip(card, track);

    if (track_in_tplay_mode(track) == HI_TRUE) {
        ret = track_set_speed_adjust(card, track->track_id, &track->speed);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_set_speed_adjust, ret);
            return;
        }
    }

    ret = memcpy_s(&track->stream_attr, sizeof(snd_track_stream_attr), stream_attr, sizeof(snd_track_stream_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return;
    }
}

static hi_void track_route_slave(snd_card_state *card, snd_track_state *track, snd_track_stream_attr *stream_attr,
    stream_mode_change_attr *change)
{
    hi_s32 ret;

    if (change->pcm_change == STREAM_CHANGE_NONE) {
        return;
    }

    snd_proc_pcm_route(card, track, change->pcm_change, stream_attr);
    track_set_eos_to_aip(card, track);

    if (track_in_tplay_mode(track) == HI_TRUE) {
        ret = track_set_speed_adjust(card, track->track_id, &track->speed);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_set_speed_adjust, ret);
            return;
        }
    }

    ret = memcpy_s(&track->stream_attr, sizeof(snd_track_stream_attr), stream_attr, sizeof(snd_track_stream_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
    }
}

static hi_void track_route(snd_card_state *card, snd_track_state *track, snd_track_stream_attr *stream_attr)
{
    stream_mode_change_attr change;

    /* close passthrough in tplay mode */
    if (track_in_tplay_mode(track) == HI_TRUE) {
        stream_attr->lbr_format = IEC61937_DATATYPE_NULL;
        stream_attr->hbr_format = IEC61937_DATATYPE_NULL;
    }

    detect_stream_mode_change(card, track, stream_attr, &change);

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
        track_route_master(card, track, stream_attr, &change);
#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
        detect_true_hd_mode_change(card, stream_attr);
#endif
    } else if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_SLAVE) {
        track_route_slave(card, track, stream_attr, &change);
    }
}

#ifdef HI_PROC_SUPPORT
static hi_void track_save_pcm_data(snd_track_state *track, snd_track_stream_attr *stream_attr)
{
    hi_s32 len;
    hi_u32 frame_size;
    hi_u32 write_size;
    hi_u32 data_size;
    hi_void *pcm_data_buf = stream_attr->pcm_data_buf;

    if (track->save_state != SND_DEBUG_CMD_CTRL_START) {
        return;
    }

    if (stream_attr->pcm_data_buf == HI_NULL) {
        /* only support saving pcm data */
        return;
    }

    if (track->file_handle == HI_NULL) {
        return;
    }

    if ((track->alsa_track == HI_FALSE) && (track->tmp_buf == HI_NULL)) {
        /* tmp_buf is not initialised */
        return;
    }

    frame_size = autil_calc_frame_size(stream_attr->pcm_channels, stream_attr->pcm_bit_depth);
    data_size = stream_attr->pcm_samples * frame_size;

    if (track->alsa_track == HI_TRUE) {
        /* save alsa pcm data */
        write_size = circ_buf_write(&track->save_alsa.cb, stream_attr->pcm_data_buf, data_size, HI_TRUE);
        if (write_size != data_size) {
            HI_LOG_ERR("circ_buf_write alsa track error\n");
            HI_ERR_PRINT_H32(write_size);
            HI_ERR_PRINT_H32(data_size);
        }

        return;
    }

    while (1) {
        if (data_size == 0) {
            /* finished saving data */
            break;
        }

        write_size = min_t(hi_u32, data_size, TRACK_SAVE_TMP_SIZE);
        /* copy write_size from user buffer every time */
        if (osal_copy_from_user(track->tmp_buf, pcm_data_buf, write_size)) {
            HI_LOG_ERR("osal_copy_from_user failed!\n");
            break;
        }

        len = osal_klib_fwrite(track->tmp_buf, write_size, track->file_handle);
        if (len != write_size) {
            HI_LOG_ERR("osal_klib_fwrite failed!\n");
            track->save_state = SND_DEBUG_CMD_CTRL_STOP;
            osal_klib_fclose(track->file_handle);
            track->file_handle = HI_NULL;
            break;
        }

        data_size -= write_size;
        /* move user buffer pointer */
        pcm_data_buf += write_size;
    }
}
#endif

static hi_void track_write_frame(snd_card_state *card, snd_track_state *track, snd_track_stream_attr *stream_attr)
{
    hi_u32 write;

#ifdef HI_PROC_SUPPORT
    track_save_pcm_data(track, stream_attr);
#endif

    if (ao_snd_check_output(card, PCM) == HI_TRUE) {
        write = hal_aoe_aip_write_buf_data(track->aip[PCM], (hi_u8 *)stream_attr->pcm_data_buf, stream_attr->pcm_bytes);
        if (write != stream_attr->pcm_bytes) {
            HI_LOG_ERR("hal_aoe_aip_write_buf_data failed\n");
            HI_ERR_PRINT_H32(write);
            HI_ERR_PRINT_H32(stream_attr->pcm_bytes);
        }
    }

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
        if (track_check_lbr_aip_used(card, stream_attr) == HI_TRUE) {
            write =
                hal_aoe_aip_write_buf_data(track->aip[LBR], (hi_u8 *)stream_attr->lbr_data_buf, stream_attr->lbr_bytes);
            if (write != stream_attr->lbr_bytes) {
                HI_LOG_ERR("hal_aoe_aip_write_buf_data failed\n");
                HI_ERR_PRINT_H32(write);
                HI_ERR_PRINT_H32(stream_attr->lbr_bytes);
            }
        }

        if (track_check_hbr_aip_used(card, stream_attr) == HI_TRUE) {
            write =
                hal_aoe_aip_write_buf_data(track->aip[HBR], (hi_u8 *)stream_attr->hbr_data_buf, stream_attr->hbr_bytes);
            if (write != stream_attr->hbr_bytes) {
                HI_LOG_ERR("hal_aoe_aip_write_buf_data failed\n");
                HI_ERR_PRINT_H32(write);
                HI_ERR_PRINT_H32(stream_attr->hbr_bytes);
            }
        }
    }
}

static hi_void track_aip_write_mute_data(aoe_aip_id aip, hi_u32 mute_size)
{
    hi_u32 free_size;

    if (mute_size == 0) {
        return;
    }

    free_size = hal_aoe_aip_query_buf_free(aip);
    if (mute_size >= free_size) {
        return;
    }

    hal_aoe_aip_write_buf_data(aip, HI_NULL, mute_size);
}

static hi_u32 track_cal_raw_aip_add_mute_ms(aoe_aip_id aip_pcm, aoe_aip_id aip_raw)
{
    hi_u32 pcm_delay_ms = 0;
    hi_u32 raw_delay_ms = 0;

    hal_aoe_aip_get_buf_delay_ms(aip_pcm, &pcm_delay_ms);
    hal_aoe_aip_get_buf_delay_ms(aip_raw, &raw_delay_ms);
    if (pcm_delay_ms <= raw_delay_ms) {
        return 0;
    } else {
        return (pcm_delay_ms - raw_delay_ms);
    }
}

static hi_void track_lbr_aip_write_mute(snd_card_state *card, snd_track_state *track,
    snd_track_stream_attr *stream_attr)
{
    hi_u32 busy_size;
    hi_u32 add_mute_size;
    hi_u32 bytes_per_frame = stream_attr->lbr_bytes;

    aoe_aip_id aip_pcm = track->aip[PCM];
    aoe_aip_id aip_raw = track->aip[LBR];

    /* when the data of raw AIP less than two frame, send mute frame */
    busy_size = hal_aoe_aip_query_buf_data(aip_raw);
    if (busy_size >= AO_TRACK_NEED_ADD_MUTE_AIP_FRAMES_LIMITS * bytes_per_frame) {
        return;
    }

    if (ao_snd_check_output(card, PCM) == HI_TRUE) {
        hi_u32 add_mute_ms = track_cal_raw_aip_add_mute_ms(aip_pcm, aip_raw);
        hi_u32 frame_size = autil_calc_frame_size(stream_attr->lbr_channels, stream_attr->lbr_bit_depth);
        add_mute_size = autil_latency_ms_to_byte_size(add_mute_ms, frame_size, stream_attr->lbr_sample_rate);
        if (add_mute_size == 0) {
            return;
        } else if (add_mute_size < bytes_per_frame) {
            add_mute_size = bytes_per_frame;
        }
    } else {
        add_mute_size = AO_TRACK_RAW_OUTPUT_ADD_MUTE_FRAMES * bytes_per_frame;
    }

    track_aip_write_mute_data(aip_raw, add_mute_size);
}

static hi_void track_hbr_aip_write_mute(snd_card_state *card, snd_track_state *track,
    snd_track_stream_attr *stream_attr)
{
    hi_u32 busy_size;
    hi_u32 add_mute_size = 0;
    hi_u32 bytes_per_frame = stream_attr->hbr_bytes;

    aoe_aip_id aip_pcm = track->aip[PCM];
    aoe_aip_id aip_raw = track->aip[HBR];

    /* when the data of raw AIP less than two frame, send mute frame */
    busy_size = hal_aoe_aip_query_buf_data(aip_raw);
    if (busy_size >= AO_TRACK_NEED_ADD_MUTE_AIP_FRAMES_LIMITS * bytes_per_frame) {
        return;
    }

    if (ao_snd_check_output(card, PCM) == HI_TRUE) {
        hi_u32 add_mute_ms = track_cal_raw_aip_add_mute_ms(aip_pcm, aip_raw);
        hi_u32 frame_size = autil_calc_frame_size(stream_attr->hbr_channels, stream_attr->hbr_bit_depth);
        add_mute_size = autil_latency_ms_to_byte_size(add_mute_ms, frame_size, stream_attr->hbr_sample_rate);
        if (add_mute_size == 0) {
            return;
        } else if (add_mute_size < bytes_per_frame) {
            add_mute_size = bytes_per_frame;
        }
    } else {
        add_mute_size = AO_TRACK_RAW_OUTPUT_ADD_MUTE_FRAMES * bytes_per_frame;
    }

    track_aip_write_mute_data(aip_raw, add_mute_size);
}

static hi_void track_write_mute_frame(snd_card_state *card, snd_track_state *track, snd_track_stream_attr *stream_attr)
{
    if (track->user_track_attr.track_type != HI_AO_TRACK_TYPE_MASTER) {
        return;
    }

    if (track->speed.type != HI_AO_SPEED_SRC) {
        return;
    }

    /* we can only make passthrough output play slow */
    if (track->speed.speed_int >= 0) {
        return;
    }

    if (ao_snd_check_output(card, LBR) == HI_TRUE) {
        track_lbr_aip_write_mute(card, track, stream_attr);
    }

    if (ao_snd_check_output(card, HBR) == HI_TRUE) {
        track_hbr_aip_write_mute(card, track, stream_attr);
    }

    track->add_mute_frame_num++;
}

static hi_void track_set_aip_rbf_attr(aoe_rbuf_attr *rbf_attr, hi_audio_buffer *rbf_mmz)
{
    rbf_attr->buf_phy_addr = rbf_mmz->phys_addr;
    rbf_attr->buf_vir_addr = rbf_mmz->virt_addr - (hi_u8 *)HI_NULL;
    rbf_attr->buf_size = rbf_mmz->size;
    rbf_attr->buf_wptr_rptr_flag = 0; /* cpu write */
}

static hi_void track_get_aip_pcm_df_attr(aoe_aip_chn_attr *aip_attr, hi_audio_buffer *rbf_mmz,
    hi_ao_track_attr *unf_attr)
{
    track_set_aip_rbf_attr(&aip_attr->buf_in_attr.rbf_attr, rbf_mmz);
    aip_attr->buf_in_attr.buf_bit_per_sample = AO_TRACK_BITDEPTH_LOW;
    aip_attr->buf_in_attr.buf_sample_rate = HI_SAMPLE_RATE_48K;
    aip_attr->buf_in_attr.buf_channels = AO_TRACK_NORMAL_CHANNELNUM;
    aip_attr->buf_in_attr.buf_data_format = 0;
    aip_attr->buf_in_attr.buf_latency_thd_ms = unf_attr->buf_level;
    aip_attr->buf_in_attr.start_threshold_ms = unf_attr->start_threshold;
    aip_attr->buf_in_attr.resume_threshold_ms = AO_TRACK_DEFATTR_RESUME_THRESHOLDMS;
    aip_attr->buf_in_attr.fade_in_ms = unf_attr->fade_in;
    aip_attr->buf_in_attr.fade_out_ms = unf_attr->fade_out;
    aip_attr->buf_in_attr.fade_enable = HI_FALSE;
    aip_attr->buf_in_attr.aip_type = AOE_AIP_TYPE_PCM_MAX;
    aip_attr->buf_in_attr.mix_priority = HI_FALSE;

    if (unf_attr->fade_in | unf_attr->fade_out) {
        aip_attr->buf_in_attr.fade_enable = HI_TRUE;
    }

    aip_attr->fifo_out_attr.fifo_bit_per_sample = AO_TRACK_BITDEPTH_LOW;
    aip_attr->fifo_out_attr.fifo_sample_rate = HI_SAMPLE_RATE_48K;
    aip_attr->fifo_out_attr.fifo_channels = AO_TRACK_NORMAL_CHANNELNUM;
    aip_attr->fifo_out_attr.fifo_data_format = 0;
    aip_attr->fifo_out_attr.fifo_latency_thd_ms = AIP_FIFO_LATENCYMS_DEFAULT;
}

static hi_void track_get_aip_lbr_df_attr(aoe_aip_chn_attr *aip_attr, hi_audio_buffer *rbf_mmz,
    hi_ao_track_attr *unf_attr)
{
    track_set_aip_rbf_attr(&aip_attr->buf_in_attr.rbf_attr, rbf_mmz);
    aip_attr->buf_in_attr.buf_bit_per_sample = AO_TRACK_BITDEPTH_LOW;
    aip_attr->buf_in_attr.buf_sample_rate = HI_SAMPLE_RATE_48K;
    aip_attr->buf_in_attr.buf_channels = AO_TRACK_NORMAL_CHANNELNUM;
    aip_attr->buf_in_attr.buf_data_format = 1;
    aip_attr->buf_in_attr.buf_latency_thd_ms = unf_attr->buf_level;
    aip_attr->buf_in_attr.start_threshold_ms = unf_attr->start_threshold;
    aip_attr->buf_in_attr.resume_threshold_ms = AO_TRACK_DEFATTR_RESUME_THRESHOLDMS;
    aip_attr->buf_in_attr.fade_enable = HI_FALSE;
    aip_attr->buf_in_attr.aip_type = AOE_AIP_TYPE_LBR;
    aip_attr->buf_in_attr.mix_priority = HI_FALSE;

    aip_attr->fifo_out_attr.fifo_bit_per_sample = AO_TRACK_BITDEPTH_LOW;
    aip_attr->fifo_out_attr.fifo_sample_rate = HI_SAMPLE_RATE_48K;
    aip_attr->fifo_out_attr.fifo_channels = AO_TRACK_NORMAL_CHANNELNUM;
    aip_attr->fifo_out_attr.fifo_data_format = 1;
    aip_attr->fifo_out_attr.fifo_latency_thd_ms = AIP_FIFO_LATENCYMS_DEFAULT;
}

static hi_void track_get_aip_hbr_df_attr(aoe_aip_chn_attr *aip_attr, hi_audio_buffer *rbf_mmz,
    hi_ao_track_attr *unf_attr)
{
    track_set_aip_rbf_attr(&aip_attr->buf_in_attr.rbf_attr, rbf_mmz);
    aip_attr->buf_in_attr.buf_bit_per_sample = AO_TRACK_BITDEPTH_LOW;
    aip_attr->buf_in_attr.buf_sample_rate = HI_SAMPLE_RATE_192K;
    aip_attr->buf_in_attr.buf_channels = AO_TRACK_MUTILPCM_CHANNELNUM;
    aip_attr->buf_in_attr.buf_data_format = 1;
    aip_attr->buf_in_attr.buf_latency_thd_ms = unf_attr->buf_level;
    aip_attr->buf_in_attr.start_threshold_ms = unf_attr->start_threshold;
    aip_attr->buf_in_attr.resume_threshold_ms = AO_TRACK_DEFATTR_RESUME_THRESHOLDMS;
    aip_attr->buf_in_attr.fade_enable = HI_FALSE;
    aip_attr->buf_in_attr.aip_type = AOE_AIP_TYPE_HBR;
    aip_attr->buf_in_attr.mix_priority = HI_FALSE;

    aip_attr->fifo_out_attr.fifo_bit_per_sample = AO_TRACK_BITDEPTH_LOW;
    aip_attr->fifo_out_attr.fifo_sample_rate = HI_SAMPLE_RATE_192K;
    aip_attr->fifo_out_attr.fifo_channels = AO_TRACK_MUTILPCM_CHANNELNUM;
    aip_attr->fifo_out_attr.fifo_data_format = 1;
    aip_attr->fifo_out_attr.fifo_latency_thd_ms = AIP_FIFO_LATENCYMS_DEFAULT;
}

static hi_s32 track_create_master(snd_card_state *card, snd_track_state *state)
{
    hi_s32 ret;
    aoe_aip_id aip;
    aoe_aip_chn_attr aip_attr;

    ret = master_track_malloc_aip_buf(card, state);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(master_track_malloc_aip_buf, ret);
        return ret;
    }

    track_get_aip_pcm_df_attr(&aip_attr, &state->aip_rbf_mmz[PCM], &state->user_track_attr);
    aip_attr.buf_in_attr.aip_type = AOE_AIP_TYPE_PCM_DMX;

    {
        hi_u32 i;
        /* for support slic 8K 1ch, avoid default value here, update fifo samplerate and channel */
        aip_attr.fifo_out_attr.fifo_sample_rate = card->user_sample_rate;
        for (i = 0; i < card->user_open_param.port_num; i++) {
            if (card->user_open_param.outport[i].port == HI_AO_PORT_I2S0) {
                aip_attr.fifo_out_attr.fifo_channels =
                    (hi_u32)(card->user_open_param.outport[i].un_attr.i2s_attr.attr.channel);
                HI_INFO_AO("update HI_AO_PORT_I2S0 fifo_channels: %d\n", aip_attr.fifo_out_attr.fifo_channels);
            }
        }
    }

    ret = hal_aoe_aip_create(&aip, &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_create, ret);
        goto CREATE_PCM_ERR_EXIT;
    }

    state->stream_attr.pcm_bit_depth = aip_attr.buf_in_attr.buf_bit_per_sample;
    state->stream_attr.pcm_sample_rate = aip_attr.buf_in_attr.buf_sample_rate;
    state->stream_attr.pcm_channels = aip_attr.buf_in_attr.buf_channels;

    state->aip[PCM] = aip;

    hal_aoe_aip_get_status_phy(aip, &state->phy_delay_addr);

    if (card->lbr_device == HI_TRUE) {
        track_get_aip_lbr_df_attr(&aip_attr, &state->aip_rbf_mmz[LBR], &state->user_track_attr);
        ret = hal_aoe_aip_create(&aip, &aip_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_create, ret);
            goto CREATE_SPDIF_ERR_EXIT;
        }

        state->stream_attr.lbr_bit_depth = aip_attr.buf_in_attr.buf_bit_per_sample;
        state->stream_attr.lbr_sample_rate = aip_attr.buf_in_attr.buf_sample_rate;
        state->stream_attr.lbr_channels = aip_attr.buf_in_attr.buf_channels;

        state->aip[LBR] = aip;
    }

    if (card->hbr_device == HI_TRUE) {
        track_get_aip_hbr_df_attr(&aip_attr, &state->aip_rbf_mmz[HBR], &state->user_track_attr);
        ret = hal_aoe_aip_create(&aip, &aip_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_create, ret);
            goto CREATE_HDMI_ERR_EXIT;
        }

        state->stream_attr.hbr_bit_depth = aip_attr.buf_in_attr.buf_bit_per_sample;
        state->stream_attr.hbr_sample_rate = aip_attr.buf_in_attr.buf_sample_rate;
        state->stream_attr.hbr_channels = aip_attr.buf_in_attr.buf_channels;

        state->aip[HBR] = aip;
    }

    return HI_SUCCESS;

CREATE_HDMI_ERR_EXIT:
    if (card->lbr_device == HI_TRUE) {
        hal_aoe_aip_destroy(state->aip[LBR]);
    }
CREATE_SPDIF_ERR_EXIT:
    hal_aoe_aip_destroy(state->aip[PCM]);
CREATE_PCM_ERR_EXIT:
    master_track_free_aip_buf(card, state);

    return HI_FAILURE;
}

static hi_s32 track_create_slave(snd_card_state *card, snd_track_state *state)
{
    hi_s32 ret;
    aoe_aip_id aip;
    aoe_aip_chn_attr aip_attr;
    hi_audio_buffer rbf_mmz;

#ifdef HI_SND_DSP_SUPPORT
    hi_bool cache = HI_FALSE;
#else
    hi_bool cache = (state->user_track_attr.track_type == HI_AO_TRACK_TYPE_LOW_LATENCY);
#endif

    ret = hi_drv_audio_smmu_alloc("ao_s_aip_pcm", AO_TRACK_PCM_BUFSIZE_BYTE_MAX, cache, &rbf_mmz);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_audio_smmu_alloc, ret);
        return ret;
    }

    track_get_aip_pcm_df_attr(&aip_attr, &rbf_mmz, &state->user_track_attr);

    {
        hi_u32 i;
        /* for support slic 8K 1ch, avoid default value here, update fifo samplerate and channel */
        aip_attr.fifo_out_attr.fifo_sample_rate = card->user_sample_rate;
        for (i = 0; i < card->user_open_param.port_num; i++) {
            if (card->user_open_param.outport[i].port == HI_AO_PORT_I2S0) {
                aip_attr.fifo_out_attr.fifo_channels =
                    (hi_u32)(card->user_open_param.outport[i].un_attr.i2s_attr.attr.channel);
                HI_INFO_AO("update HI_AO_PORT_I2S0 fifo_channels: %d\n", aip_attr.fifo_out_attr.fifo_channels);
            }
        }
    }

    /* slave track only support alsa & slave normal pcm 2.0 */
    if (state->alsa_track == HI_TRUE) {
        aip_attr.buf_in_attr.aip_type = AOE_AIP_TYPE_PCM_ALSA;
    } else {
        aip_attr.buf_in_attr.aip_type = AOE_AIP_TYPE_PCM_SLAVE;
    }

    ret = hal_aoe_aip_create(&aip, &aip_attr);
    if (ret != HI_SUCCESS) {
        hi_drv_audio_smmu_release(&rbf_mmz);
        return HI_FAILURE;
    }

    state->stream_attr.pcm_bit_depth = aip_attr.buf_in_attr.buf_bit_per_sample;
    state->stream_attr.pcm_sample_rate = aip_attr.buf_in_attr.buf_sample_rate;
    state->stream_attr.pcm_channels = aip_attr.buf_in_attr.buf_channels;

    state->aip[PCM] = aip;
    state->aip_rbf_mmz[PCM] = rbf_mmz;
    hal_aoe_aip_get_status_phy(aip, &state->phy_delay_addr);

    return HI_SUCCESS;
}

static hi_void track_set_mute_to_aip(snd_card_state *card, snd_track_state *track, hi_bool mute)
{
    hi_s32 ret;
    hi_u32 type;
    aoe_aip_id aip_id;

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        ret = hal_aoe_aip_set_mute(aip_id, mute);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_mute, ret);
            HI_ERR_PRINT_U32(aip_id);
            return;
        }
    }
}

static hi_void track_destroy_aip(snd_track_state *track)
{
    hi_u32 type;
    aoe_aip_id aip_id;

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        hal_aoe_aip_destroy(aip_id);
        track->aip[type] = AOE_AIP_MAX;
    }
}

static hi_void track_detach_engine(snd_card_state *card, snd_track_state *track)
{
    hi_u32 type;
    aoe_aip_id aip_id;
    aoe_engine_id engine_id;

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        engine_id = ao_engine_get_id_by_type(card, type);
        if (engine_id == AOE_ENGINE_MAX) {
            continue;
        }

        hal_aoe_engine_detach_aip(engine_id, aip_id);
    }
}

static hi_void track_attach_engine(snd_card_state *card, snd_track_state *track)
{
    hi_u32 type;
    aoe_aip_id aip_id;
    aoe_engine_id engine_id;

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        engine_id = ao_engine_get_id_by_type(card, type);
        if (engine_id == AOE_ENGINE_MAX) {
            continue;
        }

        hal_aoe_engine_attach_aip(engine_id, aip_id);
    }
}

hi_u32 track_get_master_id(snd_card_state *card)
{
    snd_track_state *track = HI_NULL;

    osal_list_for_each_entry(track, &card->track, node)
    {
        if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
            return track->track_id;
        }
    }

    return AO_MAX_TOTAL_TRACK_NUM;
}

hi_s32 track_check_attr(hi_ao_track_attr *track_attr)
{
    CHECK_AO_NULL_PTR(track_attr);

    if (track_attr->track_type >= HI_AO_TRACK_TYPE_MAX) {
        HI_LOG_ERR("unsupport track type\n");
        HI_ERR_PRINT_H32(track_attr->track_type);
        return HI_ERR_AO_INVALID_PARA;
    }

    if ((track_attr->buf_level < AO_TRACK_MASTER_MIN_BUFLEVELMS) ||
        (track_attr->buf_level > AO_TRACK_MASTER_MAX_BUFLEVELMS)) {
        HI_LOG_ERR("Invalid buffer level\n");
        HI_ERR_PRINT_H32(track_attr->buf_level);
        HI_ERR_PRINT_H32(AO_TRACK_MASTER_MIN_BUFLEVELMS);
        HI_ERR_PRINT_H32(AO_TRACK_MASTER_MAX_BUFLEVELMS);
        return HI_ERR_AO_INVALID_PARA;
    }

    if ((track_attr->track_type == HI_AO_TRACK_TYPE_MASTER) && (track_attr->start_threshold > track_attr->buf_level)) {
        HI_LOG_ERR("Invalid start threshold\n");
        HI_ERR_PRINT_H32(track_attr->buf_level);
        HI_ERR_PRINT_H32(track_attr->start_threshold);
        return HI_ERR_AO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_void track_state_init(snd_track_state *state)
{
    hi_u32 type;

    state->track_abs_gain.linear_mode = HI_TRUE;
    state->track_abs_gain.gain_l = AO_MAX_LINEARVOLUME;
    state->track_abs_gain.gain_r = AO_MAX_LINEARVOLUME;
    state->mute = HI_FALSE;
    state->channel_mode = HI_TRACK_MODE_STEREO;
    state->send_try_cnt = 0;
    state->send_cnt = 0;
    state->add_mute_frame_num = 0;
    state->eos_flag = HI_FALSE;
#ifdef HI_PROC_SUPPORT
    state->save_state = SND_DEBUG_CMD_CTRL_STOP;
    state->file_handle = HI_NULL;
#endif
    state->fifo_bypass = HI_FALSE;
    state->curn_status = SND_TRACK_STATUS_STOP;

    for (type = PCM; type < TYPE_MAX; type++) {
        state->aip[type] = AOE_AIP_MAX;
    }

    state->speed.type = HI_AO_SPEED_NORMAL;
    state->speed.speed_int = 1;
    state->speed.speed_dec = 0;
}

hi_s32 track_create(snd_card_state *card, hi_ao_track_attr *attr, hi_bool alsa_track, hi_u32 track_id)
{
    hi_s32 ret;
    snd_track_state *state = HI_NULL;
    hi_s32 (*track_create_func)(snd_card_state *card, snd_track_state *state) = track_create_slave;

    ret = track_check_attr(attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_check_attr, ret);
        return ret;
    }

    if (attr->track_type == HI_AO_TRACK_TYPE_MASTER) {
        if (track_get_master_id(card) != AO_MAX_TOTAL_TRACK_NUM) {
            HI_LOG_ERR("master track exist!\n");
            return HI_ERR_AO_NOTSUPPORT;
        }

        track_create_func = track_create_master;
    }

    state = (snd_track_state *)osal_kmalloc(HI_ID_AO, sizeof(snd_track_state), OSAL_GFP_KERNEL);
    if (state == HI_NULL) {
        HI_FATAL_AO("osal_kmalloc snd_track_state failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(state, sizeof(snd_track_state), 0, sizeof(snd_track_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out;
    }

    track_state_init(state);
    state->track_id = track_id;
    state->alsa_track = alsa_track;
    ret = memcpy_s(&state->user_track_attr, sizeof(hi_ao_track_attr), attr, sizeof(hi_ao_track_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        goto out;
    }

    ret = track_create_func(card, state);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_create_func, ret);
        goto out;
    }

    osal_list_add_tail(&state->node, &card->track);
    card->snd_track_init_flag |= ((hi_u32)1L << track_id);

    track_attach_engine(card, state);

    return HI_SUCCESS;

out:
    osal_kfree(HI_ID_AO, state);
    return ret;
}

hi_s32 track_destroy(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

#ifdef HI_PROC_SUPPORT
    /* free resources */
    if (track->save_state == SND_DEBUG_CMD_CTRL_START) {
        track_proc_save_data_stop(track);
    }
#endif

    ret = track_stop(card, track_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_stop, ret);
    }

    track_detach_engine(card, track);
    track_destroy_aip(track);

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
        master_track_free_aip_buf(card, track);
    } else {
        if (track->aip_rbf_ext_dma_mem[PCM] != HI_TRUE) {
            hi_drv_audio_smmu_release(&track->aip_rbf_mmz[PCM]);
        }
    }

    card->snd_track_init_flag &= ~((hi_u32)1L << track->track_id);
    osal_list_del(&track->node);
    osal_kfree(HI_ID_AO, track);

    return HI_SUCCESS;
}

static hi_s32 track_check_ao_frame(ao_frame *frame)
{
    CHECK_AO_NULL_PTR(frame);

    if ((frame->sample_rate > HI_SAMPLE_RATE_192K) || (frame->sample_rate < HI_SAMPLE_RATE_8K)) {
        HI_LOG_ERR("Invalid AO frame sample rate\n");
        HI_ERR_PRINT_U32(frame->sample_rate);
        return HI_ERR_AO_INVALID_PARA;
    }

    if ((frame->bit_depth != HI_BIT_DEPTH_8) && (frame->bit_depth != HI_BIT_DEPTH_16) &&
        (frame->bit_depth != HI_BIT_DEPTH_24)) {
        HI_LOG_ERR("Invalid AO frame bit depth\n");
        HI_ERR_PRINT_U32(frame->bit_depth);
        return HI_ERR_AO_INVALID_PARA;
    }

    if (frame->pcm_samples > AO_TRACK_MAX_PCM_DATA_SIZE) {
        HI_LOG_ERR("invalid AO frame pcm_samples too large\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    /* valid AO frame */
    if ((frame->pcm_buffer != HI_NULL) && (frame->pcm_samples != 0)) {
        return HI_SUCCESS;
    }

    /* valid AO frame */
    if ((frame->bits_buffer != HI_NULL) && (frame->bits_bytes != 0)) {
        return HI_SUCCESS;
    }

    HI_LOG_ERR("invalid AO frame data!\n");
    return HI_ERR_AO_INVALID_PARA;
}

static hi_s32 track_check_start(snd_track_state *track)
{
    if (track->curn_status >= SND_TRACK_STATUS_MAX) {
        HI_LOG_ERR("invalid ao track status\n");
        return HI_ERR_AO_NOTSUPPORT;
    } else if (track->curn_status == SND_TRACK_STATUS_PAUSE) {
        return HI_ERR_AO_PAUSE_STATE;
    } else if (track->curn_status == SND_TRACK_STATUS_STOP) {
        HI_LOG_ERR("ao track stop status, can't send data\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    return HI_SUCCESS;
}

hi_s32 track_send_data(snd_card_state *card, hi_u32 track_id, ao_frame *frame)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;
    snd_track_stream_attr stream_attr = { 0 };

    ret = track_check_ao_frame(frame);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    TRACK_FIND_BY_ID(card, track_id, track);

    ret = track_check_start(track);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    track->send_try_cnt++;

    track_build_stream_attr(frame, &stream_attr);

    track_route(card, track, &stream_attr);

    if (track_is_buf_free(card, track, &stream_attr) == HI_FALSE) {
        track_try_start_aip(track); /* buf is full, directly start aip */
        return HI_ERR_AO_OUT_BUF_FULL;
    }

    track_write_frame(card, track, &stream_attr);
    track_write_mute_frame(card, track, &stream_attr);

    track_try_start_aip(track);

    track->send_cnt++;

    return HI_SUCCESS;
}

hi_s32 track_atomic_send_data(snd_card_state *card, hi_u32 track_id, ao_frame *frame)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;
    snd_track_stream_attr stream_attr = { 0 };

    ret = track_check_ao_frame(frame);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->alsa_track == HI_FALSE) {
        HI_LOG_ERR("not alsa track\n");
        HI_ERR_PRINT_H32(track_id);
        return HI_ERR_AO_NOTSUPPORT;
    }

    ret = track_check_start(track);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    track->send_try_cnt++;

    /* alsa track only has pcm data */
    if (ao_snd_check_output(card, PCM) == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* alsa track only need to build pcm attr */
    track_build_pcm_attr(frame, &stream_attr);

    if (track_is_buf_free(card, track, &stream_attr) == HI_FALSE) {
        return HI_ERR_AO_OUT_BUF_FULL;
    }

    track_write_frame(card, track, &stream_attr);

    track->send_cnt++;

    return HI_SUCCESS;
}

static hi_void track_pcm_aip_resume_delay(snd_card_state *card, snd_track_state *track, hi_u32 target_delay)
{
    hi_u32 cur_delay;
    hi_u32 aip_delay = 0;
    hi_u32 aip_fifo_delay = 0;
    hi_u32 aop_delay = 0;

    if (ao_snd_check_output(card, PCM) == HI_FALSE) {
        return;
    }

    hal_aoe_aip_get_buf_delay_ms(track->aip[PCM], &aip_delay);
    hal_aoe_aip_get_fifo_delay_ms(track->aip[PCM], &aip_fifo_delay);
    snd_op_get_delay_by_output_mode(card, PCM, &aop_delay);

    cur_delay = aip_delay + aip_fifo_delay + aop_delay;

    if (target_delay <= cur_delay) {
        return;
    }

    hal_aoe_aip_add_mute_at_read(track->aip[PCM], target_delay - cur_delay);
}

static hi_void track_lbr_aip_resume_delay(snd_card_state *card, snd_track_state *track, hi_u32 target_delay)
{
    hi_u32 cur_delay;
    hi_u32 aip_delay = 0;
    hi_u32 aop_delay = 0;

    if (ao_snd_check_output(card, LBR) == HI_FALSE) {
        return;
    }

    hal_aoe_aip_get_buf_delay_ms(track->aip[LBR], &aip_delay);
    snd_op_get_delay_by_output_mode(card, LBR, &aop_delay);

    cur_delay = aip_delay + aop_delay;

    if (target_delay <= cur_delay) {
        return;
    }

    hal_aoe_aip_add_mute_at_read(track->aip[LBR], target_delay - cur_delay);
}

static hi_void track_hbr_aip_resume_delay(snd_card_state *card, snd_track_state *track, hi_u32 target_delay)
{
    hi_u32 cur_delay;
    hi_u32 aip_delay = 0;
    hi_u32 aop_delay = 0;

    if (ao_snd_check_output(card, HBR) == HI_FALSE) {
        return;
    }

    hal_aoe_aip_get_buf_delay_ms(track->aip[HBR], &aip_delay);
    snd_op_get_delay_by_output_mode(card, HBR, &aop_delay);

    cur_delay = aip_delay + aop_delay;

    if (target_delay <= cur_delay) {
        return;
    }

    hal_aoe_aip_add_mute_at_read(track->aip[HBR], target_delay - cur_delay);
}

static hi_void track_resume_delay(snd_card_state *card, snd_track_state *track, hi_u32 target_delay)
{
    track_pcm_aip_resume_delay(card, track, target_delay);
    track_lbr_aip_resume_delay(card, track, target_delay);
    track_hbr_aip_resume_delay(card, track, target_delay);
}

hi_s32 track_start(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->curn_status == SND_TRACK_STATUS_START) {
        return HI_SUCCESS;
    }

    if (track->curn_status == SND_TRACK_STATUS_PAUSE) {
        track_resume_delay(card, track, track->pause_delay_ms);
    }

    ret = track_start_aip(track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_start_aip, ret);
        return ret;
    }

    track->curn_status = SND_TRACK_STATUS_START;

    return ret;
}

hi_s32 track_stop(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->curn_status == SND_TRACK_STATUS_STOP) {
        return HI_SUCCESS;
    }

    ret = track_stop_aip(track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_stop_aip, ret);
        return ret;
    }

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
#if defined(HI_SND_SPDIF_SUPPORT)
        if (card->spdif_passthrough == LBR) {
            snd_proc_spidf_route(card, track, STREAM_CHANGE_LBR2PCM, &track->stream_attr);
        } else if (card->spdif_passthrough == HBR) {
            snd_proc_spidf_route(card, track, STREAM_CHANGE_HBR2PCM, &track->stream_attr);
        }
#endif

#if defined(HI_SND_HDMI_SUPPORT)
        if (card->hdmi_passthrough == LBR) {
            snd_proc_hdmi_route(card, track, STREAM_CHANGE_LBR2PCM, &track->stream_attr);
        } else if (card->hdmi_passthrough == HBR) {
            snd_proc_hdmi_route(card, track, STREAM_CHANGE_HBR2PCM, &track->stream_attr);
        }
#endif
    }

    track->eos_flag = HI_FALSE;
    track->curn_status = SND_TRACK_STATUS_STOP;

    return ret;
}

hi_s32 track_pause(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->curn_status == SND_TRACK_STATUS_PAUSE) {
        return HI_SUCCESS;
    }

    ret = track_get_delay_ms(card, track_id, &track->pause_delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_get_delay_ms, ret);
        return ret;
    }

    ret = track_pause_aip(track);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_pause_aip, ret);
        return ret;
    }

    track->curn_status = SND_TRACK_STATUS_PAUSE;

    return ret;
}

hi_s32 track_flush(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    hi_u32 type;
    aoe_aip_id aip_id;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        ret = hal_aoe_aip_flush(aip_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_flush, ret);
            HI_ERR_PRINT_U32(aip_id);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 track_detect_attr(snd_card_state *card, hi_u32 track_id, hi_ao_track_attr *track_attr,
    snd_track_attr_setting *attr_setting)
{
    snd_track_state *track = HI_NULL;

    if (track_check_attr(track_attr) != HI_SUCCESS) {
        HI_LOG_ERR("invalid parameter!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->user_track_attr.track_type == track_attr->track_type) {
        if ((track->user_track_attr.buf_level != track_attr->buf_level) ||
            (track->user_track_attr.start_threshold != track_attr->start_threshold) ||
            (track->user_track_attr.fade_in != track_attr->fade_in) ||
            (track->user_track_attr.fade_out != track_attr->fade_out) ||
            (track->user_track_attr.output_buf_size != track_attr->output_buf_size)) {
            *attr_setting = SND_TRACK_ATTR_MODIFY;
        } else {
            *attr_setting = SND_TRACK_ATTR_RETAIN;
        }
    } else if (track_attr->track_type == HI_AO_TRACK_TYPE_MASTER) {
        *attr_setting = SND_TRACK_ATTR_SLAVE2MASTER;
    } else if (track_attr->track_type == HI_AO_TRACK_TYPE_SLAVE) {
        *attr_setting = SND_TRACK_ATTR_MASTER2SLAVE;
    } else {
        HI_LOG_ERR("virtual track not support \n");
        return HI_ERR_AO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 track_set_attr(snd_card_state *card, hi_u32 track_id, hi_ao_track_attr *track_attr)
{
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(track_attr);
    TRACK_FIND_BY_ID(card, track_id, track);

    track->user_track_attr.buf_level = track_attr->buf_level;
    track->user_track_attr.start_threshold = track_attr->start_threshold;

    track->user_track_attr.fade_in = track_attr->fade_in;
    track->user_track_attr.fade_out = track_attr->fade_out;
    track->user_track_attr.output_buf_size = track_attr->output_buf_size;

    return HI_SUCCESS;
}

hi_s32 track_set_weight(snd_card_state *card, hi_u32 track_id, hi_ao_gain *track_gain)
{
    hi_u32 reg_db;
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(track_gain);

    if (track_gain->linear_mode == HI_TRUE) {
        CHECK_AO_LINEARVOLUME(track_gain->gain);
        reg_db = autil_volume_linear_to_reg_db((hi_u32)track_gain->gain);
    } else {
        CHECK_AO_ABSLUTEVOLUME(track_gain->gain);
        reg_db = autil_volume_db_to_reg_db(track_gain->gain);
    }

    TRACK_FIND_BY_ID(card, track_id, track);

    hal_aoe_aip_set_volume(track->aip[PCM], reg_db, reg_db);
    track->track_abs_gain.linear_mode = track_gain->linear_mode;
    track->track_abs_gain.gain_l = track_gain->gain;
    track->track_abs_gain.gain_r = track_gain->gain;

    return HI_SUCCESS;
}

hi_s32 track_get_weight(snd_card_state *card, hi_u32 track_id, hi_ao_gain *track_gain)
{
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(track_gain);
    TRACK_FIND_BY_ID(card, track_id, track);

    track_gain->linear_mode = track->track_abs_gain.linear_mode;

    /* only report gain in left channel */
    track_gain->gain = track->track_abs_gain.gain_l;

    return HI_SUCCESS;
}

hi_s32 track_set_abs_gain(snd_card_state *card, hi_u32 track_id, hi_ao_abs_gain *track_abs_gain)
{
    hi_u32 l_volume_db_reg;
    hi_u32 r_volume_db_reg;
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(track_abs_gain);

    if (track_abs_gain->linear_mode == HI_TRUE) {
        CHECK_AO_LINEARVOLUME(track_abs_gain->gain_l);
        CHECK_AO_LINEARVOLUME(track_abs_gain->gain_r);

        l_volume_db_reg = autil_volume_linear_to_reg_db((hi_u32)track_abs_gain->gain_l);
        r_volume_db_reg = autil_volume_linear_to_reg_db((hi_u32)track_abs_gain->gain_r);
    } else {
        CHECK_AO_ABSLUTEVOLUMEEXT(track_abs_gain->gain_l);
        CHECK_AO_ABSLUTEVOLUMEEXT(track_abs_gain->gain_r);

        l_volume_db_reg = autil_volume_db_to_reg_db(track_abs_gain->gain_l);
        r_volume_db_reg = autil_volume_db_to_reg_db(track_abs_gain->gain_r);
    }

    TRACK_FIND_BY_ID(card, track_id, track);

    hal_aoe_aip_set_volume(track->aip[PCM], l_volume_db_reg, r_volume_db_reg);
    track->track_abs_gain.linear_mode = track_abs_gain->linear_mode;
    track->track_abs_gain.gain_l = track_abs_gain->gain_l;
    track->track_abs_gain.gain_r = track_abs_gain->gain_r;

    return HI_SUCCESS;
}

hi_s32 track_get_abs_gain(snd_card_state *card, hi_u32 track_id, hi_ao_abs_gain *track_abs_gain)
{
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(track_abs_gain);
    TRACK_FIND_BY_ID(card, track_id, track);

    track_abs_gain->linear_mode = track->track_abs_gain.linear_mode;
    track_abs_gain->gain_l = track->track_abs_gain.gain_l;
    track_abs_gain->gain_r = track->track_abs_gain.gain_r;

    return HI_SUCCESS;
}

hi_s32 track_set_prescale(snd_card_state *card, hi_u32 track_id, hi_ao_preci_gain *preci_gain)
{
    hi_u32 int_db;
    hi_s32 dec_db;
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(preci_gain);
    CHECK_AO_ABSLUTEPRECIVOLUME(preci_gain->integer, preci_gain->decimal);

    int_db = autil_volume_db_to_reg_db(preci_gain->integer);
    dec_db = autil_decimal_volume_db_to_reg_db(preci_gain->decimal);

    TRACK_FIND_BY_ID(card, track_id, track);

    hal_aoe_aip_set_prescale(track->aip[PCM], int_db, dec_db);

    track->user_prescale.integer = preci_gain->integer;
    track->user_prescale.decimal = preci_gain->decimal;

    return HI_SUCCESS;
}

hi_s32 track_get_prescale(snd_card_state *card, hi_u32 track_id, hi_ao_preci_gain *preci_gain)
{
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(preci_gain);
    TRACK_FIND_BY_ID(card, track_id, track);

    preci_gain->decimal = track->user_prescale.decimal;
    preci_gain->integer = track->user_prescale.integer;

    return HI_SUCCESS;
}

hi_s32 track_set_mute(snd_card_state *card, hi_u32 track_id, hi_bool mute)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->alsa_track == HI_TRUE) {
        HI_LOG_WARN("alsa track don't support mute function!\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    if ((card->all_track_mute == HI_FALSE) && (mute == HI_FALSE)) {
        HI_LOG_INFO("track set unmute\n");
        HI_INFO_PRINT_U32(track_id);
        track_set_mute_to_aip(card, track, HI_FALSE);
    } else {
        HI_LOG_INFO("track set mute\n");
        HI_INFO_PRINT_U32(track_id);
        track_set_mute_to_aip(card, track, HI_TRUE);
    }

    track->mute = mute;

    return HI_SUCCESS;
}

hi_s32 track_get_mute(snd_card_state *card, hi_u32 track_id, hi_bool *mute)
{
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(mute);
    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->alsa_track == HI_TRUE) {
        HI_LOG_WARN("alsa track don't support mute function!\n");
        return HI_FAILURE;
    }

    *mute = track->mute;

    HI_LOG_INFO("track get mute\n");
    HI_INFO_PRINT_U32(track_id);
    HI_INFO_PRINT_BOOL(track->mute);

    return HI_SUCCESS;
}

hi_s32 track_set_all_mute(snd_card_state *card, hi_bool mute)
{
    snd_track_state *track = HI_NULL;

    osal_list_for_each_entry(track, &card->track, node) {
        if (track->alsa_track == HI_TRUE) {
            continue;
        }

        if ((track->mute == HI_FALSE) && (mute == HI_FALSE)) {
            HI_LOG_INFO("track set unmute\n");
            HI_INFO_PRINT_U32(track->track_id);
            track_set_mute_to_aip(card, track, HI_FALSE);
        } else {
            HI_LOG_INFO("track set mute\n");
            HI_INFO_PRINT_U32(track->track_id);
            track_set_mute_to_aip(card, track, HI_TRUE);
        }
    }

    return HI_SUCCESS;
}

hi_s32 track_set_channel_mode(snd_card_state *card, hi_u32 track_id, hi_track_mode mode)
{
    snd_track_state *track = HI_NULL;

    if (mode >= HI_TRACK_MODE_MAX) {
        return HI_ERR_AO_INVALID_PARA;
    }

    TRACK_FIND_BY_ID(card, track_id, track);

    hal_aoe_aip_set_channel_mode(track->aip[PCM], (hi_u32)mode);
    track->channel_mode = mode;

    return HI_SUCCESS;
}

hi_s32 track_get_channel_mode(snd_card_state *card, hi_u32 track_id, hi_track_mode *mode)
{
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(mode);
    TRACK_FIND_BY_ID(card, track_id, track);

    *mode = track->channel_mode;

    return HI_SUCCESS;
}

hi_s32 track_set_add_mute_ms(snd_card_state *card, hi_u32 track_id, hi_u32 mute_ms)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    if (ao_snd_check_output(card, PCM) == HI_TRUE) {
        hal_aoe_aip_set_add_mute_ms(track->aip[PCM], mute_ms);
    }

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
        if (ao_snd_check_output(card, LBR) == HI_TRUE) {
            hal_aoe_aip_set_add_mute_ms(track->aip[LBR], mute_ms);
        }

        if (ao_snd_check_output(card, HBR) == HI_TRUE) {
            hal_aoe_aip_set_add_mute_ms(track->aip[HBR], mute_ms);
        }
    }

    return HI_SUCCESS;
}

hi_s32 track_set_speed_adjust(snd_card_state *card, hi_u32 track_id, hi_ao_speed *speed)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    switch (speed->type) {
        case HI_AO_SPEED_SRC:
            if (ao_snd_check_output(card, PCM) == HI_TRUE) {
                CHECK_AO_SPEEDADJUST(speed->speed_int);
                ret = hal_aoe_aip_set_adjust_speed(track->aip[PCM], speed->speed_int);
                if (ret != HI_SUCCESS) {
                    HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_adjust_speed, ret);
                    return ret;
                }
            }
            break;

        case HI_AO_SPEED_MUTE:
            if (speed->speed_int <= 0) {
                return HI_ERR_AO_NOTSUPPORT;
            }

            ret = track_set_add_mute_ms(card, track_id, (hi_u32)speed->speed_int);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_speed, ret);
                return ret;
            }

            break;

        case HI_AO_SPEED_NORMAL:
        case HI_AO_SPEED_TPLAY:
            ret = hal_aoe_aip_set_speed(track->aip[PCM], speed);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_speed, ret);
                return ret;
            }

            break;

        default:
            return HI_ERR_AO_INVALID_PARA;
    }

    ret = memcpy_s(&track->speed, sizeof(track->speed), speed, sizeof(*speed));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef HI_AUDIO_AI_SUPPORT
hi_s32 track_set_pcm_attr(snd_card_state *card, hi_u32 track_id, hi_handle h_ai)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;
    aoe_aip_chn_attr aip_attr;
    aiao_rbuf_attr aiao_buf;
    aiao_port_attr port_attr;

    ret = ai_get_port_attr(h_ai, &port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ai_get_port_attr, ret);
        return ret;
    }

    ret = memset_s(&aiao_buf, sizeof(aiao_rbuf_attr), 0, sizeof(aiao_rbuf_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }
    ret = ai_get_port_buf(h_ai, &aiao_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ai_get_port_buf, ret);
        return ret;
    }

    TRACK_FIND_BY_ID(card, track_id, track);
    hal_aoe_aip_get_attr(track->aip[PCM], &aip_attr);

    aip_attr.buf_in_attr.buf_sample_rate = (hi_u32)(port_attr.if_attr.rate);
    aip_attr.buf_in_attr.buf_bit_per_sample = (hi_u32)(port_attr.if_attr.bit_depth);
    aip_attr.buf_in_attr.buf_channels = (hi_u32)(port_attr.if_attr.ch_num);

    aip_attr.buf_in_attr.rbf_attr.buf_phy_addr = aiao_buf.buf_phy_addr;
    aip_attr.buf_in_attr.rbf_attr.buf_phy_rptr = aiao_buf.buf_phy_rptr;
    aip_attr.buf_in_attr.rbf_attr.buf_phy_wptr = aiao_buf.buf_phy_wptr;
    aip_attr.buf_in_attr.rbf_attr.buf_vir_addr = aiao_buf.buf_vir_addr;
    aip_attr.buf_in_attr.rbf_attr.buf_vir_rptr = aiao_buf.buf_vir_rptr;
    aip_attr.buf_in_attr.rbf_attr.buf_vir_wptr = aiao_buf.buf_vir_wptr;
    aip_attr.buf_in_attr.rbf_attr.buf_size = aiao_buf.buf_size;
    aip_attr.buf_in_attr.rbf_attr.buf_wptr_rptr_flag = 1;
    aip_attr.buf_in_attr.mix_priority = HI_TRUE;
    hal_aoe_aip_set_attr(track->aip[PCM], &aip_attr);

    return HI_SUCCESS;
}

hi_s32 track_attach_ai(snd_card_state *card, hi_u32 track_id, hi_handle h_ai)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    track->att_ai = HI_TRUE;
    track->h_ai = h_ai;
    return HI_SUCCESS;
}

hi_s32 track_detach_ai(snd_card_state *card, hi_u32 track_id)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;
    aoe_aip_chn_attr aip_attr;
    hi_audio_buffer mmz_buf;
    TRACK_FIND_BY_ID(card, track_id, track);
    mmz_buf = track->aip_rbf_mmz[PCM];

    ret = hal_aoe_aip_get_attr(track->aip[PCM], &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_get_attr, ret);
        return ret;
    }
    aip_attr.buf_in_attr.rbf_attr.buf_phy_addr = mmz_buf.phys_addr;
    aip_attr.buf_in_attr.rbf_attr.buf_vir_addr = mmz_buf.virt_addr - (hi_u8 *)HI_NULL;
    aip_attr.buf_in_attr.rbf_attr.buf_size = mmz_buf.size;
    aip_attr.buf_in_attr.rbf_attr.buf_wptr_rptr_flag = 0;
    aip_attr.buf_in_attr.mix_priority = HI_FALSE;

    hal_aoe_aip_stop(track->aip[PCM]);
    ret = hal_aoe_aip_set_attr(track->aip[PCM], &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_attr, ret);
        return ret;
    }
    ret = hal_aoe_aip_start(track->aip[PCM]);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_start, ret);
        return ret;
    }

    track->att_ai = HI_FALSE;
    track->h_ai = HI_INVALID_HANDLE;
    return ret;
}
#endif

hi_s32 track_get_aip_delay_ms(snd_card_state *card, snd_track_state *track, hi_u32 *delay_ms)
{
    hi_u32 aip_delay = 0;
    hi_u32 aip_fifo_delay = 0;
    aoe_aip_id aip_id;

    *delay_ms = 0;

#ifdef HI_AUDIO_AI_SUPPORT
    if (track->att_ai == HI_TRUE) {
        return HI_SUCCESS;
    }
#endif

    if (track->fifo_bypass == HI_TRUE) {
        return HI_SUCCESS;
    }

    if (ao_snd_check_output(card, PCM) == HI_TRUE) {
        aip_id = track->aip[PCM];
    } else if (ao_snd_check_output(card, LBR) == HI_TRUE) {
        aip_id = track->aip[LBR];
    } else if (ao_snd_check_output(card, HBR) == HI_TRUE) {
        aip_id = track->aip[HBR];
    } else {
        return HI_SUCCESS;
    }

    hal_aoe_aip_get_buf_delay_ms(aip_id, &aip_delay);
    hal_aoe_aip_get_fifo_delay_ms(aip_id, &aip_fifo_delay);

    *delay_ms = aip_delay + aip_fifo_delay;

    return HI_SUCCESS;
}

hi_s32 track_get_delay_ms(snd_card_state *card, hi_u32 track_id, hi_u32 *delay)
{
    hi_s32 ret;
    hi_u32 aip_delay = 0;
    hi_u32 snd_op_delay = 0;
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(delay);
    TRACK_FIND_BY_ID(card, track_id, track);

    ret = track_get_aip_delay_ms(card, track, &aip_delay);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_get_aip_delay_ms, ret);
        HI_ERR_PRINT_H32(track_id);
        return ret;
    }

    snd_get_delay_ms(card, &snd_op_delay);

    *delay = aip_delay + snd_op_delay;

    return HI_SUCCESS;
}

hi_s32 track_is_buf_empty(snd_card_state *card, hi_u32 track_id, hi_bool *empty)
{
    hi_s32 ret;
    hi_u32 aip_delay = 0;
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(empty);
    TRACK_FIND_BY_ID(card, track_id, track);

    ret = track_get_aip_delay_ms(card, track, &aip_delay);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_get_aip_delay_ms, ret);
        HI_ERR_PRINT_H32(track_id);
        return ret;
    }

    *empty = (aip_delay <= AO_TRACK_BUF_EMPTY_THRESHOLD_MS);

    return HI_SUCCESS;
}

hi_s32 track_set_eos(snd_card_state *card, hi_u32 track_id, hi_bool eos)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    track->eos_flag = eos;
    track_set_eos_to_aip(card, track);

    return HI_SUCCESS;
}

hi_s32 track_get_def_attr(hi_ao_track_attr *def_attr)
{
    switch (def_attr->track_type) {
        case HI_AO_TRACK_TYPE_MASTER:
            def_attr->buf_level = AO_TRACK_MASTER_DEFATTR_BUFLEVELMS;
            def_attr->output_buf_size = AO_TRACK_MASTER_DEFATTR_BUFSIZE;
            def_attr->fade_in = AO_TRACK_MASTER_DEFATTR_FADEINMS;
            def_attr->fade_out = AO_TRACK_MASTER_DEFATTR_FADEOUTMS;
            def_attr->start_threshold = AO_TRACK_DEFATTR_START_THRESHOLDMS;
            break;

        case HI_AO_TRACK_TYPE_LOW_LATENCY:
        case HI_AO_TRACK_TYPE_SLAVE:
        case HI_AO_TRACK_TYPE_OTT_SOUND:
        case HI_AO_TRACK_TYPE_SYSTEM_AUDIO:
        case HI_AO_TRACK_TYPE_APP_AUDIO:
            def_attr->buf_level = AO_TRACK_SLAVE_DEFATTR_BUFLEVELMS;
            def_attr->output_buf_size = AO_TRACK_SLAVE_DEFATTR_BUFSIZE;
            def_attr->fade_in = AO_TRACK_SLAVE_DEFATTR_FADEINMS;
            def_attr->fade_out = AO_TRACK_SLAVE_DEFATTR_FADEOUTMS;
            def_attr->start_threshold = 0;
            break;

        case HI_AO_TRACK_TYPE_VIRTUAL:
            def_attr->output_buf_size = AO_TRACK_VIRTUAL_DEFATTR_BUFSIZE;
            break;

        default:
            HI_LOG_ERR("invalid track type\n");
            HI_ERR_PRINT_H32(def_attr->track_type);
            return HI_ERR_AO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 track_get_attr(snd_card_state *card, hi_u32 track_id, hi_ao_track_attr *track_attr)
{
    snd_track_state *track = HI_NULL;

    CHECK_AO_NULL_PTR(track_attr);

    TRACK_FIND_BY_ID(card, track_id, track);

    return memcpy_s(track_attr, sizeof(hi_ao_track_attr), &track->user_track_attr, sizeof(hi_ao_track_attr));
}

snd_card_state *track_card_get_card(hi_u32 track_id)
{
    ao_snd_id snd;
    snd_card_state *card = HI_NULL;

    if (track_id >= AO_MAX_TOTAL_TRACK_NUM) {
        return HI_NULL;
    }

    for (snd = AO_SND_0; snd < AO_SND_MAX; snd++) {
        card = snd_card_get_card(snd);
        if (card != HI_NULL) {
            if (card->snd_track_init_flag & (1L << track_id)) {
                return card;
            }
        }
    }

    return HI_NULL;
}

hi_s32 track_set_used_by_kernel(snd_card_state *card, hi_u32 track_id)
{
    hi_u32 type;
    aoe_aip_id aip_id;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        hal_aoe_aip_set_data_source(aip_id, HI_TRUE);
    }

    return HI_SUCCESS;
}

hi_s32 track_set_all_prescale(snd_card_state *card, hi_ao_preci_gain *prescale)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    osal_list_for_each_entry(track, &card->track, node) {
        if (track->alsa_track == HI_TRUE) {
            continue;
        }

        ret = track_set_prescale(card, track->track_id, prescale);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_set_prescale, ret);
            HI_ERR_PRINT_U32(track->track_id);
            return ret;
        }
    }

    return HI_SUCCESS;
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 track_set_aip_fifo_bypass(snd_card_state *card, hi_u32 track_id, hi_bool enable)
{
    hi_s32 ret;
    snd_track_state *state = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, state);

    ret = hal_aoe_aip_set_fifo_bypass(state->aip[PCM], enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_fifo_bypass, ret);
        HI_ERR_PRINT_BOOL(enable);
        return ret;
    }

    state->fifo_bypass = enable;

    return ret;
}

hi_s32 track_set_priority(snd_card_state *card, hi_u32 track_id, hi_bool enable)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->curn_status != SND_TRACK_STATUS_START) {
        HI_LOG_WARN("please make track start first !\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    ret = hal_aoe_aip_set_mix_priority(track->aip[PCM], enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_mix_priority, ret);
        HI_ERR_PRINT_BOOL(enable);
        return ret;
    }
    track->aip_priority = enable;

    return ret;
}

hi_s32 track_get_priority(snd_card_state *card, hi_u32 track_id, hi_bool *enable)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    return hal_aoe_aip_get_mix_priority(track->aip[PCM], enable);
}

hi_s32 track_set_fifo_latency(snd_card_state *card, hi_u32 track_id, hi_u32 latency_ms)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    if ((latency_ms < AOE_AIP_FIFO_LATENCYMS_MIN) || (latency_ms > AIP_FIFO_BUFFERSIZE_MS_DEFAULT)) {
        HI_LOG_ERR("invalid track fifo latency\n");
        HI_ERR_PRINT_U32(latency_ms);
        return HI_ERR_AO_INVALID_PARA;
    }

    TRACK_FIND_BY_ID(card, track_id, track);

    ret = hal_aoe_aip_set_fifo_latency(track->aip[PCM], latency_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_fifo_latency, ret);
    }

    return ret;
}

hi_s32 track_get_fifo_latency(snd_card_state *card, hi_u32 track_id, hi_u32 *latency_ms)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    return hal_aoe_aip_get_fifo_latency(track->aip[PCM], latency_ms);
}

hi_s32 track_set_fade_attr(snd_card_state *card, hi_u32 track_id, ao_track_fade *track_fade)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    ret = hal_aoe_aip_set_fade_attr(track->aip[PCM], track_fade->enable, track_fade->fade_in, track_fade->fade_out);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_fade_attr, ret);
    }

    return ret;
}

hi_s32 track_mmap(snd_card_state *card, hi_u32 track_id, ao_track_mmap_param *param)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;
    aoe_aip_chn_attr aip_attr;

    TRACK_FIND_BY_ID(card, track_id, track);

    if (track->curn_status != SND_TRACK_STATUS_STOP) {
        HI_LOG_ERR("mmap track should be ahead of track start!\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    if (track->fifo_bypass == HI_TRUE) {
        /* check pcm attr, we only support 48k/2ch/16bit */
        if (param->bit_per_sample != HI_I2S_BIT_DEPTH_16) {
            HI_LOG_ERR("fifo bypass: track bit_per_sample should be HI_I2S_BIT_DEPTH_16\n");
            return HI_ERR_AO_INVALID_PARA;
        }

        if (param->channels != HI_I2S_CH_2) {
            HI_LOG_ERR("fifo bypass: track channel should be HI_I2S_CH_2\n");
            return HI_ERR_AO_INVALID_PARA;
        }

        if (param->sample_rate != HI_SAMPLE_RATE_48K) {
            HI_LOG_ERR("fifo bypass: track sample_rate should be HI_SAMPLE_RATE_48K\n");
            return HI_ERR_AO_INVALID_PARA;
        }
    }

    param->aip_reg_addr = hal_aoe_aip_get_reg_addr(track->aip[PCM]);

    /* change AIP attr */
    ret = hal_aoe_aip_get_attr(track->aip[PCM], &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_get_attr, ret);
        return ret;
    }

    aip_attr.buf_in_attr.buf_bit_per_sample = param->bit_per_sample;
    aip_attr.buf_in_attr.buf_sample_rate = param->sample_rate;
    aip_attr.buf_in_attr.buf_channels = param->channels;
    aip_attr.buf_in_attr.buf_data_format = 0;

    ret = hal_aoe_aip_set_attr(track->aip[PCM], &aip_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_get_attr, ret);
        return ret;
    }

    /* save stream attr */
    track->stream_attr.pcm_bit_depth = param->bit_per_sample;
    track->stream_attr.pcm_sample_rate = param->sample_rate;
    track->stream_attr.pcm_channels = param->channels;

    return HI_SUCCESS;
}

hi_s32 track_set_info(snd_card_state *card, hi_u32 track_id, hi_u64 virt_addr)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    track->user_delay_virt_addr = virt_addr;
    return HI_SUCCESS;
}

hi_s32 track_get_info(snd_card_state *card, hi_u32 track_id, hi_u64 *virt_addr, hi_u64 *phys_addr)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    *virt_addr = track->user_delay_virt_addr;
    *phys_addr = track->phy_delay_addr;
    return HI_SUCCESS;
}

static hi_void track_force_to_pcm(snd_card_state *card, snd_track_state *track)
{
    hi_s32 ret;
    snd_track_stream_attr stream_attr;

    /* build stream attr as default */
    ret = memset_s(&stream_attr, sizeof(snd_track_stream_attr), 0, sizeof(snd_track_stream_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return;
    }

    stream_attr.pcm_sample_rate = HI_SAMPLE_RATE_48K;
    stream_attr.pcm_bit_depth = HI_BIT_DEPTH_16;
    stream_attr.pcm_channels = AO_TRACK_NORMAL_CHANNELNUM;

    ret = track_flush(card, track->track_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_flush, ret);
    }
    track_route(card, track, &stream_attr);
}

/* route every track to 48k/2ch/16bit */
hi_s32 track_force_all_to_pcm(snd_card_state *card)
{
    snd_track_state *track = HI_NULL;

    osal_list_for_each_entry(track, &card->track, node) {
        if (track->alsa_track == HI_TRUE) {
            /* cannot route alsa track to 48k/2ch, flush alsa track individually */
            hi_s32 ret = hal_aoe_aip_flush(track->aip[PCM]);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_flush, ret);
                return ret;
            }
        } else {
            /* route every track to 48k/2ch/16bit */
            track_force_to_pcm(card, track);
        }
    }

    return HI_SUCCESS;
}

hi_s32 track_set_resume_threshold_ms(snd_card_state *card, hi_u32 track_id, hi_u32 threshold_ms)
{
    hi_s32 ret;
    hi_u32 type;
    aoe_aip_id aip_id;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    for (type = PCM; type < TYPE_MAX; type++) {
        aip_id = track->aip[type];
        if (aip_id == AOE_AIP_MAX) {
            continue;
        }

        ret = hal_aoe_aip_set_resume_threshold_ms(aip_id, threshold_ms);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_resume_threshold_ms, ret);
            HI_ERR_PRINT_U32(aip_id);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 track_get_resume_threshold_ms(snd_card_state *card, hi_u32 track_id, hi_u32 *threshold_ms)
{
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    /* all aip(s) in one track have the same resume threshold */
    return hal_aoe_aip_get_resume_threshold_ms(track->aip[PCM], threshold_ms);
}
#endif

hi_s32 track_get_setting(snd_card_state *card, hi_u32 track_id, snd_track_settings *snd_settings)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    snd_settings->curn_status = track->curn_status;

    ret = memcpy_s(&snd_settings->speed, sizeof(hi_ao_speed), &track->speed, sizeof(hi_ao_speed));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    ret =
        memcpy_s(&snd_settings->track_abs_gain, sizeof(hi_ao_abs_gain), &track->track_abs_gain, sizeof(hi_ao_abs_gain));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    ret = memcpy_s(&snd_settings->prescale, sizeof(hi_ao_preci_gain), &track->user_prescale, sizeof(hi_ao_preci_gain));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    ret = memcpy_s(&snd_settings->track_attr, sizeof(hi_ao_track_attr), &track->user_track_attr,
        sizeof(hi_ao_track_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    snd_settings->channel_mode = track->channel_mode;
    snd_settings->mute = track->mute;
    snd_settings->priority = track->aip_priority;
    snd_settings->bypass = track->fifo_bypass;

#ifdef HI_AUDIO_AI_SUPPORT
    snd_settings->att_ai = track->att_ai;
    snd_settings->h_ai = track->h_ai;
#endif

    snd_settings->alsa_track = track->alsa_track;
    snd_settings->data_from_kernel = track->data_from_kernel;
    return HI_SUCCESS;
}

static hi_s32 track_restore_status(snd_card_state *card, hi_u32 track_id, snd_track_status track_status)
{
    hi_s32 ret;
    hi_s32 (*track_control)(snd_card_state *card, hi_u32 track_id) = HI_NULL;

    switch (track_status) {
        case SND_TRACK_STATUS_STOP:
            track_control = track_stop;
            break;

        case SND_TRACK_STATUS_START:
            track_control = track_start;
            break;

        case SND_TRACK_STATUS_PAUSE:
            track_control = track_pause;
            break;

        default:
            return HI_SUCCESS;
    }

    ret = track_control(card, track_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_control, ret);
        HI_ERR_PRINT_H32(track_id);
        HI_ERR_PRINT_H32(track_status);
    }

    return ret;
}

hi_s32 track_restore_setting(snd_card_state *card, hi_u32 track_id, snd_track_settings *snd_settings)
{
    hi_s32 ret;

    ret = track_restore_status(card, track_id, snd_settings->curn_status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_restore_status, ret);
        return ret;
    }

    ret = track_set_speed_adjust(card, track_id, &snd_settings->speed);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_speed_adjust, ret);
        return ret;
    }

    ret = track_set_mute(card, track_id, snd_settings->mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_mute, ret);
        return ret;
    }

#ifdef HI_SND_ADVANCED_SUPPORT
    ret = track_set_aip_fifo_bypass(card, track_id, snd_settings->bypass);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_aip_fifo_bypass, ret);
        return ret;
    }

    ret = track_set_priority(card, track_id, snd_settings->priority);
    if (ret != HI_SUCCESS) {
        HI_WARN_PRINT_FUNC_RES(track_set_priority, ret);
    }
#endif

    ret = track_set_abs_gain(card, track_id, &snd_settings->track_abs_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_abs_gain, ret);
        return ret;
    }

    ret = track_set_channel_mode(card, track_id, snd_settings->channel_mode);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_channel_mode, ret);
        return ret;
    }

    ret = track_set_prescale(card, track_id, &snd_settings->prescale);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(track_set_prescale, ret);
        return ret;
    }

    if (snd_settings->data_from_kernel == HI_TRUE) {
        ret = track_set_used_by_kernel(card, track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_set_used_by_kernel, ret);
            return ret;
        }
    }

    return ret;
}

#ifdef HI_PROC_SUPPORT
static hi_bool track_need_read_proc(snd_card_state *card, hi_u32 aip_type)
{
    if (aip_type == PCM) {
        return ao_snd_check_output(card, PCM);
    }

    if (aip_type == LBR) {
        return ao_snd_check_output(card, LBR);
    }

    if (aip_type == HBR) {
        return ao_snd_check_output(card, HBR);
    }

    return HI_FALSE;
}

hi_void track_read_chn_proc(hi_void *filp, snd_track_state *track)
{
    osal_proc_print(filp, "track(%d): ", track->track_id);

    if (track->alsa_track == HI_TRUE) {
        osal_proc_print(filp, "type(alsa)");
    } else {
        osal_proc_print(filp, "type(%s)", autil_track_to_name(track->user_track_attr.track_type));
    }

    osal_proc_print(filp, ", status(%s), weight(%.3d/%.3d%s)",
        (hi_char *)((track->curn_status == SND_TRACK_STATUS_START) ?
        "start" : ((track->curn_status == SND_TRACK_STATUS_STOP) ?
        "stop" : "pause")), track->track_abs_gain.gain_l,
        track->track_abs_gain.gain_r, (track->track_abs_gain.linear_mode == HI_TRUE) ? "" : "dB");

    osal_proc_print(filp, ", prescale(%s%d.%ddB)",
        (track->user_prescale.integer == 0 && track->user_prescale.decimal < 0) ?
        "-" : "", track->user_prescale.integer,
        (track->user_prescale.decimal < 0) ?
        (-track->user_prescale.decimal) : track->user_prescale.decimal);

    osal_proc_print(filp, ", channel_mode(%s), mute(%s)", autil_track_mode_to_name(track->channel_mode),
        TRUE_ON_FALSE_OFF(track->mute));

#ifdef HI_AUDIO_AI_SUPPORT
    if (track->att_ai == HI_TRUE) {
        osal_proc_print(filp, ", attach_ai(0x%x)", track->h_ai);
    }
#endif

    if (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER) {
        osal_proc_print(filp, ", start_thres_hold(%.3dms)", track->user_track_attr.start_threshold);
    }

    osal_proc_print(filp, "\n");

    osal_proc_print(filp, "          speed_rate(%.2d), add_mute_frames(%.4d), send_cnt(try/OK)(%.6u/%.6u)\n",
        track->speed.speed_int, track->add_mute_frame_num, track->send_try_cnt, track->send_cnt);
}

hi_void track_read_aip_proc(hi_void *filp, snd_card_state *card, snd_track_state *track)
{
    hi_u32 delay_ms;
    hi_u32 aip_type;
    aoe_aip_chn_attr aip_attr;
    hi_bool aip_fifo_bypass = HI_FALSE;

    for (aip_type = PCM; aip_type < TYPE_MAX; aip_type++) {
        if (track->aip[aip_type] == AOE_AIP_MAX) {
            continue;
        }

        if ((aip_type == PCM) || (track->user_track_attr.track_type == HI_AO_TRACK_TYPE_MASTER)) {
            if (track_need_read_proc(card, aip_type) == HI_FALSE) {
                continue;
            }

            hal_aoe_aip_get_attr(track->aip[aip_type], &aip_attr);
#ifdef HI_SND_ADVANCED_SUPPORT
            hal_aoe_aip_get_fifo_bypass(track->aip[aip_type], &aip_fifo_bypass);
#endif
            osal_proc_print(filp,
                "*AIP(%x): name(%s), sample_rate(%.6d), channel(%.2d), bit_width(%2d), data_format(%s), "
                "priority(%s), fifo_bypass(%s)\n",
                (hi_u32)track->aip[aip_type], autil_aip_to_name(aip_type), aip_attr.buf_in_attr.buf_sample_rate,
                aip_attr.buf_in_attr.buf_channels, aip_attr.buf_in_attr.buf_bit_per_sample,
                autil_format_to_name(aip_attr.buf_in_attr.buf_data_format),
                (aip_attr.buf_in_attr.mix_priority == HI_FALSE) ? "low" : "high", TRUE_ON_FALSE_OFF(aip_fifo_bypass));

            hal_aoe_aip_get_buf_delay_ms(track->aip[aip_type], &delay_ms);
            osal_proc_print(filp,
                "         empty_cnt(%.6u), empty_warning_cnt(%.6u), latency/threshold(%.3dms/%.3dms)\n", 0, 0, delay_ms,
                aip_attr.buf_in_attr.buf_latency_thd_ms);
        }
    }

    osal_proc_print(filp, "\n");
}

hi_s32 track_read_proc(hi_void *filp, snd_card_state *card)
{
    snd_track_state *track = HI_NULL;

    osal_list_for_each_entry(track, &card->track, node)
    {
        track_read_chn_proc(filp, track);
        track_read_aip_proc(filp, card, track);
    }

    return HI_SUCCESS;
}

hi_s32 ao_write_proc_set_track(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 track_id;
    const hi_char *pc_mute_cmd = "mute";
    /* const hi_char* pc_prescale_cmd = "prescale"; */
    const hi_char *pc_weight_cmd = "weight";
    const hi_char *pc_on_cmd = "on";
    const hi_char *pc_off_cmd = "off";
    const hi_char *pcd_unit = "dB";
    hi_bool nagetive = HI_FALSE;
    hi_char *cmd = argv[1];
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (cmd[0] < '0' || cmd[0] > '9') {
        return HI_FAILURE;
    }

    track_id = (hi_u32)simple_strtoul(cmd, &cmd, 10); /* base 10 transfer to decimal */
    if (track_id >= AO_MAX_TOTAL_TRACK_NUM) {
        HI_LOG_ERR("invalid track ID!\n");
        return HI_FAILURE;
    }

    if (strstr(argv[2], pc_mute_cmd)) {
        if (strstr(argv[3], pc_on_cmd)) {
            return track_set_mute(card, track_id, HI_TRUE);
        } else if (strstr(argv[3], pc_off_cmd)) {
            return track_set_mute(card, track_id, HI_FALSE);
        } else {
            HI_LOG_ERR("Invalid input\n");
            return HI_FAILURE;
        }
    } else if (strstr(argv[2], pc_weight_cmd)) {
        hi_u32 val;
        hi_ao_gain track_gain;
        cmd = argv[3]; /* 3 is argv num. */
        if (strstr(cmd, pcd_unit)) {
            if (cmd[0] == '-') {
                nagetive = HI_TRUE;
                cmd++;
            }
            val = (hi_u32)simple_strtoul(cmd, &cmd, 10);
            track_gain.linear_mode = HI_FALSE;
            track_gain.gain = (nagetive == HI_TRUE) ? (-val) : val;
            return track_set_weight(card, track_id, &track_gain);
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }
}

static hi_s32 track_proc_save_data_start(snd_track_state *track)
{
    hi_s32 ret;
    osal_task *thread = HI_NULL;

    /* for non-alsa track, only need to malloc tmp buffer */
    if (track->alsa_track != HI_TRUE) {
        track->tmp_buf = (hi_void *)osal_kmalloc(HI_ID_AO, TRACK_SAVE_TMP_SIZE, OSAL_GFP_KERNEL);
        if (track->tmp_buf == HI_NULL) {
            HI_LOG_ERR("malloc track save tmp buffer failed\n");
            return HI_FAILURE;
        }

        return HI_SUCCESS;
    }

    ret = memset_s(&track->save_alsa, sizeof(track_save_alsa_attr), 0, sizeof(track_save_alsa_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return HI_FAILURE;
    }

    ret = hi_drv_audio_smmu_alloc("save_alsa_track", MAX_SAVE_ALSA_SIZE, HI_TRUE, &track->save_alsa.rbf_mmz);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_audio_smmu_alloc, ret);
        return ret;
    }

    circ_buf_init(&track->save_alsa.cb, (hi_void *)(&track->save_alsa.buf_wptr),
        (hi_void *)(&track->save_alsa.buf_rptr), (hi_void *)(track->save_alsa.rbf_mmz.virt_addr),
        track->save_alsa.rbf_mmz.size);

    thread = osal_kthread_create(save_alsa_thread, track, "save_alsa_data_thread", 0);
    if (IS_ERR(thread)) {
        hi_drv_audio_smmu_release(&track->save_alsa.rbf_mmz);
        HI_LOG_ERR("create alsa track save thread failed\n");
        return HI_FAILURE;
    }

    track->save_alsa.thread = thread;

    return HI_SUCCESS;
}

static hi_void track_proc_save_data_stop(snd_track_state *track)
{
    if (track->save_alsa.thread != HI_NULL) {
        osal_kthread_destroy(track->save_alsa.thread, 1);
        hi_drv_audio_smmu_release(&track->save_alsa.rbf_mmz);
        track->save_alsa.thread = HI_NULL;
    }

    if (track->tmp_buf != HI_NULL) {
        osal_kfree(HI_ID_AO, track->tmp_buf);
        track->tmp_buf = HI_NULL;
    }

    if (track->file_handle != HI_NULL) {
        osal_klib_fclose(track->file_handle);
        track->file_handle = HI_NULL;
        osal_printk("finished saving track data.\n");
    }

    track->save_state = SND_DEBUG_CMD_CTRL_STOP;
}

hi_s32 track_write_proc_save_data(snd_card_state *card, hi_u32 track_id, snd_debug_cmd_ctrl cmd)
{
    hi_s32 ret;
    snd_track_state *track = HI_NULL;

    TRACK_FIND_BY_ID(card, track_id, track);

    if ((cmd == SND_DEBUG_CMD_CTRL_START) && (track->save_state == SND_DEBUG_CMD_CTRL_STOP)) {
        hi_char path[AO_TRACK_PATH_NAME_MAXLEN + AO_TRACK_FILE_NAME_MAXLEN] = { 0 };
        osal_rtc_time now = { 0 };
        osal_timeval tv = { 0 };

        if (osal_klib_get_store_path(path, AO_TRACK_PATH_NAME_MAXLEN) != HI_SUCCESS) {
            HI_LOG_ERR("get store path failed\n");
            return HI_FAILURE;
        }

        osal_get_timeofday(&tv);
        osal_rtc_time_to_tm(tv.tv_sec, &now);

        ret = snprintf(path, sizeof(path), "%s/track%d_%02u_%02u_%02u.pcm", path, track_id, now.tm_hour, now.tm_min,
            now.tm_sec);
        if (ret < 0) {
            HI_ERR_PRINT_FUNC_RES(snprintf, ret);
            return ret;
        }

        track->file_handle = osal_klib_fopen(path, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, OSAL_O_ACCMODE);
        if (track->file_handle == HI_NULL) {
            HI_LOG_ERR("open file error\n");
            HI_ERR_PRINT_STR(path);
            return HI_FAILURE;
        }

        ret = track_proc_save_data_start(track);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(track_proc_save_data_start, ret);
            HI_ERR_PRINT_U32(track_id);
            osal_klib_fclose(track->file_handle);
            track->file_handle = HI_NULL;
            return ret;
        }

        osal_printk("started saving track data to %s\n", path);
    } else if ((cmd == SND_DEBUG_CMD_CTRL_START) && (track->save_state == SND_DEBUG_CMD_CTRL_START)) {
        osal_printk("started saving track data already.\n");
    } else if ((cmd == SND_DEBUG_CMD_CTRL_STOP) && (track->save_state == SND_DEBUG_CMD_CTRL_START)) {
        track->save_state = SND_DEBUG_CMD_CTRL_STOP;
        track_proc_save_data_stop(track);
    } else if ((cmd == SND_DEBUG_CMD_CTRL_STOP) && (track->save_state == SND_DEBUG_CMD_CTRL_STOP)) {
        osal_printk("no track data saved, please start saving.\n");
    }

    track->save_state = cmd;

    return HI_SUCCESS;
}
#endif
