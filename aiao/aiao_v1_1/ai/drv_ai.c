/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio input (AI) driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"

#include "hi_drv_dev.h"
#include "hi_drv_ai.h"

#include "hal_aiao.h"
#include "audio_util.h"

#include "drv_ai_private.h"
#include "drv_ai_ioctl.h"

#include "drv_ao_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HI_AI_DRV_SUSPEND_SUPPORT

static osal_semaphore g_ai_mutex = { HI_NULL };

#ifdef HI_PROC_SUPPORT
static hi_void ai_reg_proc(ai_chn_state *state);
static hi_void ai_unreg_proc(ai_chn_state *state);
static hi_void ai_proc_save_data_stop(ai_chn_state *state);
#endif

extern hi_s32 ao_track_get_delay_ms(hi_u32 track_id, hi_u32 *delay_ms);

static ai_global_param g_ai_drv = {
    .ext_func = {
        .ai_drv_resume = ai_drv_resume,
        .ai_drv_suspend = ai_drv_suspend,
    },
};

#ifdef HI_ALSA_AI_SUPPORT
#include <sound/pcm.h>
static ai_alsa_param g_alsa_attr;
#endif

static hi_bool ai_check_port_valid(hi_ai_port ai_port)
{
    hi_u32 i;
    hi_ai_port support_port[] = { HI_AI_I2S0, HI_AI_I2S1, HI_AI_ADC0, HI_AI_HDMI0, HI_AI_HDMI1 };

    for (i = 0; i < ARRAY_SIZE(support_port); i++) {
        if (ai_port == support_port[i]) {
            return HI_TRUE;
        }
    }

    HI_ERR_AI("just support I2S0/I2S1, ADC0, HDMI0/HDMI1 port!\n");
    return HI_FALSE;
}

static hi_bool ai_check_port_used(hi_ai_port ai_port)
{
    hi_u32 i;

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        if (g_ai_drv.ap_ai_state[i] == HI_NULL) {
            continue;
        }

        if (g_ai_drv.ap_ai_state[i]->ai_port == ai_port) {
            HI_ERR_AI("this port has been occupied!\n");
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

static hi_void ai_init_chn_state(ai_chn_state *state)
{
    hi_ai_attr *ai_attr = &state->snd_port_attr;
    switch (state->ai_port) {
        case HI_AI_I2S0:
        case HI_AI_I2S1:
            state->channels = ai_attr->un_attr.i2s_attr.attr.channel;
            state->bit_per_sample = ai_attr->un_attr.i2s_attr.attr.bit_depth;
            return;

        case HI_AI_HDMI0:
        case HI_AI_HDMI1:
        case HI_AI_HDMI2:
        case HI_AI_HDMI3:
            state->channels = ai_attr->un_attr.hdmi_attr.channel;
            state->bit_per_sample = ai_attr->un_attr.hdmi_attr.bit_depth;
            if ((ai_attr->un_attr.hdmi_attr.hdmi_audio_data_format == HI_AI_HDMI_FORMAT_LBR) ||
                (ai_attr->un_attr.hdmi_attr.hdmi_audio_data_format == HI_AI_HDMI_FORMAT_HBR)) {
                ai_attr->pcm_samples_per_frame = AI_SAMPLE_PERFRAME_DF * 20;
            }
            return;

        case HI_AI_ADC0:
        case HI_AI_ADC1:
        case HI_AI_ADC2:
        case HI_AI_ADC3:
        case HI_AI_ADC4:
        case HI_AI_SIF0:
            state->channels = HI_I2S_CH_2;
            state->bit_per_sample = HI_I2S_BIT_DEPTH_16;
            return;
        default:
            state->channels = HI_I2S_CH_2;
            state->bit_per_sample = HI_I2S_BIT_DEPTH_16;
            return;
    }
}

static hi_s32 ai_get_default_attr(hi_ai_port ai_port, hi_ai_attr *ai_attr)
{
    if (ai_check_port_valid(ai_port) == HI_FALSE) {
        return HI_FAILURE;
    }

    ai_attr->sample_rate = HI_SAMPLE_RATE_48K;
    ai_attr->pcm_frame_max_num = AI_BUFF_FRAME_NUM_DF;
    ai_attr->pcm_samples_per_frame = AI_SAMPLE_PERFRAME_DF;

    if ((ai_port == HI_AI_I2S0) || (ai_port == HI_AI_I2S1)) {
        ai_attr->un_attr.i2s_attr.attr.master = HI_TRUE;
        ai_attr->un_attr.i2s_attr.attr.i2s_mode = HI_I2S_STD_MODE;
        ai_attr->un_attr.i2s_attr.attr.mclk = HI_I2S_MCLK_256_FS;
        ai_attr->un_attr.i2s_attr.attr.bclk = HI_I2S_BCLK_4_DIV;
        ai_attr->un_attr.i2s_attr.attr.channel = HI_I2S_CH_2;
        ai_attr->un_attr.i2s_attr.attr.bit_depth = HI_I2S_BIT_DEPTH_16;
        ai_attr->un_attr.i2s_attr.attr.pcm_sample_rise_edge = HI_TRUE;
        ai_attr->un_attr.i2s_attr.attr.pcm_delay_cycle = HI_I2S_PCM_0_DELAY;
    } else if ((ai_port == HI_AI_ADC0) || (ai_port == HI_AI_ADC1) || (ai_port == HI_AI_ADC2) ||
        (ai_port == HI_AI_ADC3) || (ai_port == HI_AI_ADC4)) {
        ai_attr->un_attr.adc_attr.by_pass = HI_FALSE;
    } else if ((ai_port == HI_AI_HDMI0) || (ai_port == HI_AI_HDMI1) || (ai_port == HI_AI_HDMI2) ||
        (ai_port == HI_AI_HDMI3)) {
        ai_attr->un_attr.hdmi_attr.sample_rate = HI_SAMPLE_RATE_48K;
        ai_attr->un_attr.hdmi_attr.bit_depth = HI_I2S_BIT_DEPTH_16;
        ai_attr->un_attr.hdmi_attr.channel = HI_I2S_CH_2;
        ai_attr->un_attr.hdmi_attr.hdmi_audio_data_format = HI_AI_HDMI_FORMAT_LPCM;
    }

    return HI_SUCCESS;
}

#ifdef HI_ALSA_AI_SUPPORT
hi_s32 ai_get_isr_func(aiao_isr_func **func)
{
    aiao_port_user_cfg attr;

    hal_aiao_get_tx_i2s_df_attr(AIAO_PORT_TX0, &attr);

    *func = attr.isr_func;

    return HI_SUCCESS;
}

hi_s32 ai_get_aiao_port_id(hi_handle h_ai, aiao_port_id *port)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    *port = state->port;

    return HI_SUCCESS;
}

static hi_s32 ai_alsa_set_param(hi_handle h_ai, hi_void *alsa_para)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    state->alsa = HI_TRUE;
    state->alsa_para = alsa_para;

    return HI_SUCCESS;
}
#endif

static hi_s32 ai_check_i2s_attr(hi_ai_i2s_attr *i2s_attr)
{
    CHECK_AI_MCLKDIV(i2s_attr->attr.mclk);
    CHECK_AI_BCLKDIV(i2s_attr->attr.bclk);
    CHECK_AI_CHN(i2s_attr->attr.channel);
    CHECK_AI_BITDEPTH(i2s_attr->attr.bit_depth);
    CHECK_AI_PCMDELAY(i2s_attr->attr.pcm_delay_cycle);

    if (HI_I2S_MODE_MAX <= i2s_attr->attr.i2s_mode) {
        HI_ERR_AI("dont support i2s_mode(%d)\n", i2s_attr->attr.i2s_mode);
        return HI_ERR_AI_INVALID_PARA;
    }

    if (HI_I2S_MCLK_MAX <= i2s_attr->attr.mclk) {
        HI_ERR_AI("dont support I2S mclk_sel(%d)\n", i2s_attr->attr.bclk);
        return HI_ERR_AI_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_s32 ai_check_hdmi_attr(hi_ai_hdmi_attr *hdmi_attr)
{
    CHECK_AI_CHN(hdmi_attr->channel);
    CHECK_AI_BITDEPTH(hdmi_attr->bit_depth);
    check_ai_hdmi_data_format(hdmi_attr->hdmi_audio_data_format);

    return HI_SUCCESS;
}

static hi_s32 ai_check_adc_attr(hi_ai_attr *ai_attr)
{
    if (ai_attr->sample_rate != HI_SAMPLE_RATE_48K) {
        HI_ERR_AI("ADC port only support 48k samplerate!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 ai_check_attr(hi_ai_port ai_port, hi_ai_attr *ai_attr)
{
    hi_s32 ret = HI_SUCCESS;
    CHECK_AI_SAMPLERATE(ai_attr->sample_rate);

    if ((ai_attr->pcm_frame_max_num <= 0) || (ai_attr->pcm_samples_per_frame <= 0)) {
        HI_ERR_AI("pcm_frame_max_num(%d) is invalid!\n", ai_attr->pcm_frame_max_num);
        return HI_ERR_AI_INVALID_PARA;
    }

    switch (ai_port) {
        case HI_AI_I2S0:
        case HI_AI_I2S1:
            ret = ai_check_i2s_attr(&ai_attr->un_attr.i2s_attr);
            break;

        case HI_AI_HDMI0:
        case HI_AI_HDMI1:
        case HI_AI_HDMI2:
        case HI_AI_HDMI3:
            ret = ai_check_hdmi_attr(&ai_attr->un_attr.hdmi_attr);
            break;

        case HI_AI_ADC0:
        case HI_AI_ADC1:
        case HI_AI_ADC2:
        case HI_AI_ADC3:
        case HI_AI_ADC4:
            ret = ai_check_adc_attr(ai_attr);
            break;

        case HI_AI_SIF0:
            break;

        default:
            break;
    }

    return ret;
}

static hi_void ai_set_port_if_attr(hi_ai_port ai_port, hi_ai_attr *ai_attr, aiao_if_attr *if_attr)
{
    if_attr->rate = (aiao_sample_rate)(ai_attr->sample_rate);

    if ((ai_port == HI_AI_I2S0) || (ai_port == HI_AI_I2S1)) {
        hi_i2s_attr *i2s_attr = &ai_attr->un_attr.i2s_attr.attr;
        if (i2s_attr->master == HI_TRUE) {
            if_attr->crg_mode = AIAO_CRG_MODE_DUPLICATE;
            if (ai_port == HI_AI_I2S0) {
                if_attr->crg_source = AIAO_TX_CRG0;
            }

            if (ai_port == HI_AI_I2S1) {
                if_attr->crg_source = AIAO_TX_CRG1;
            }

            if_attr->bclk_div = i2s_attr->bclk;
            if_attr->fclk_div = autil_bclk_fclk_div(i2s_attr->mclk, i2s_attr->bclk);
        } else {
            if_attr->crg_mode = AIAO_CRG_MODE_SLAVE;
        }
        if_attr->i2s_mode = autil_i2s_mode_unf_to_aiao(i2s_attr->i2s_mode);
        if_attr->ch_num = autil_ch_num_unf_to_aiao(i2s_attr->channel);
        if_attr->bit_depth = autil_bit_depth_unf_to_aiao((hi_u32)i2s_attr->bit_depth);
        if_attr->pcm_delay_cycles = i2s_attr->pcm_delay_cycle;
        if (i2s_attr->pcm_sample_rise_edge == HI_TRUE) {
            if_attr->rise_edge = AIAO_MODE_EDGE_RISE;
        } else {
            if_attr->rise_edge = AIAO_MODE_EDGE_FALL;
        }
    } else if ((ai_port == HI_AI_HDMI0) || (ai_port == HI_AI_HDMI1) || (ai_port == HI_AI_HDMI2) ||
        (ai_port == HI_AI_HDMI3)) {
        if_attr->ch_num = autil_ch_num_unf_to_aiao(ai_attr->un_attr.hdmi_attr.channel);
        if_attr->bit_depth = autil_bit_depth_unf_to_aiao((hi_u32)ai_attr->un_attr.hdmi_attr.bit_depth);
    } else if ((ai_port == HI_AI_ADC0) || (ai_port == HI_AI_ADC1) || (ai_port == HI_AI_ADC2) ||
        (ai_port == HI_AI_ADC3) || (ai_port == HI_AI_ADC4)) {
    }
}

static void port2_tianlai_sel(hi_ai_port ai_port, hi_bool by_pass)
{
#ifdef HI_TIANLAI_V510
    s5_tianlai_linein_sel sel = S5_TIANLAI_LINEIN_SEL_MAX;
    switch (ai_port) {
        case HI_AI_ADC0:
            sel = S5_TIANLAI_LINEIN_SEL_MIC;
            break;
        case HI_AI_ADC1:
            sel = S5_TIANLAI_LINEIN_SEL_L1;
            break;
        case HI_AI_ADC2:
            sel = S5_TIANLAI_LINEIN_SEL_L2;
            break;
        case HI_AI_ADC3:
            sel = S5_TIANLAI_LINEIN_SEL_L3;
            break;
        case HI_AI_ADC4:
            sel = S5_TIANLAI_LINEIN_SEL_L4;
            break;
        default:
            break;
    }

    hal_tianlai_v510_set_line_in_suorce(sel, by_pass, HI_TRUE);
#endif
}

static hi_void ai_free_buf(ai_chn_state *state)
{
    hi_drv_audio_mmz_release(&state->rbf_mmz);
    hi_drv_audio_mmz_release(&state->frame_mmz);
}

static hi_s32 ai_alloc_buf(ai_chn_state *state)
{
    hi_s32 ret;
    hi_char sz_name[16] = { 0 };
    hi_u32 ai_frame_size;
    hi_u32 buf_size;
    hi_ai_attr *ai_attr = &state->snd_port_attr;
    hi_handle h_ai = ((state->h_ai) & AI_CHNID_MASK);

    ai_frame_size = ai_attr->pcm_samples_per_frame * autil_calc_frame_size(state->channels, state->bit_per_sample);

    /* frame buffer for AI acquire frame */
    ret = snprintf(sz_name, sizeof(sz_name), "ai_chn_buf%d", h_ai);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return ret;
    }

    ret = hi_drv_audio_mmz_alloc(sz_name, ai_frame_size, HI_FALSE, &state->frame_mmz);
    if (ret != HI_SUCCESS) {
        HI_FATAL_AI("HI_MMZ AI_BUF failed, alloc_size(%d)\n", ai_frame_size);
        return ret;
    }
    state->ai_buf.phy_base_addr = state->frame_mmz.phys_addr;
    state->ai_buf.size = state->frame_mmz.size;
    state->ai_buf.kernel_vir_addr = state->frame_mmz.virt_addr - (hi_u8 *)HI_NULL;
    state->ai_buf.map_fd = state->frame_mmz.fd;
    state->ai_buf.user_vir_addr = 0;
    state->ai_buf.read = 0;
    state->ai_buf.write = 0;

    /* AI port mmz */
    buf_size = ai_frame_size * ai_attr->pcm_frame_max_num;

    ret = snprintf(sz_name, sizeof(sz_name), "ai_i2s_buf%d", h_ai);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        hi_drv_audio_mmz_release(&state->frame_mmz);
        return ret;
    }

    ret = hi_drv_audio_mmz_alloc(sz_name, buf_size, HI_FALSE, &state->rbf_mmz);
    if (ret != HI_SUCCESS) {
        HI_FATAL_AI("mmz_alloc AI_PORT_BUF failed, alloc_size(%d)\n", buf_size);
        hi_drv_audio_mmz_release(&state->frame_mmz);
    }

    return ret;
}

static hi_void ai_create_set_flag(ai_chn_state *state)
{
    switch (state->ai_port) {
        case HI_AI_I2S0:
            g_ai_drv.ai_use_flag |= (hi_u32)1 << AI_I2S0_MSK;
            break;

        case HI_AI_I2S1:
            g_ai_drv.ai_use_flag |= (hi_u32)1 << AI_I2S1_MSK;
            break;

        case HI_AI_ADC0:
        case HI_AI_ADC1:
        case HI_AI_ADC2:
        case HI_AI_ADC3:
        case HI_AI_ADC4:
            g_ai_drv.ai_use_flag |= (hi_u32)1 << AI_ADAC_MSK;
            break;

        case HI_AI_SIF0:
            g_ai_drv.ai_use_flag |= (hi_u32)1 << AI_I2S1_MSK;
            break;

        case HI_AI_HDMI0:
        case HI_AI_HDMI1:
        case HI_AI_HDMI2:
        case HI_AI_HDMI3:
            g_ai_drv.ai_use_flag |= (hi_u32)1 << AI_HDMI_MSK;
            break;

        default:
            HI_ERR_AI("aiport is invalid!\n");
            break;
    }
}

static hi_void ai_destory_set_flag(ai_chn_state *state)
{
    switch (state->ai_port) {
        case HI_AI_I2S0:
            g_ai_drv.ai_use_flag &= (hi_u32)(~(1 << AI_I2S0_MSK));
            break;

        case HI_AI_I2S1:
            g_ai_drv.ai_use_flag &= (hi_u32)(~(1 << AI_I2S1_MSK));
            break;

        case HI_AI_ADC0:
        case HI_AI_ADC1:
        case HI_AI_ADC2:
        case HI_AI_ADC3:
        case HI_AI_ADC4:
            g_ai_drv.ai_use_flag &= (hi_u32)(~(1 << AI_ADAC_MSK));
            break;

        case HI_AI_HDMI0:
        case HI_AI_HDMI1:
        case HI_AI_HDMI2:
        case HI_AI_HDMI3:
            g_ai_drv.ai_use_flag &= (hi_u32)(~(1 << AI_HDMI_MSK));
            break;

        case HI_AI_SIF0:
            g_ai_drv.ai_use_flag &= (hi_u32)(~(1 << AI_I2S1_MSK));
            break;

        default:
            HI_ERR_AI("aiport is invalid!\n");
            break;
    }
}

static hi_s32 ai_alloc_handle(hi_handle *ph_handle, hi_void *file, hi_ai_port ai_port, hi_ai_attr *ai_attr)
{
    hi_u32 i;
    hi_handle h_ai;
    hi_s32 ret;
    ai_chn_state *state = HI_NULL;

    if (ai_check_port_valid(ai_port) == HI_FALSE) {
        return HI_ERR_AI_INVALID_PARA;
    }

    if (ai_check_port_used(ai_port) == HI_FALSE) {
        return HI_ERR_AI_INVALID_PARA;
    }

    ret = ai_check_attr(ai_port, ai_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ai_check_attr, ret);
        return ret;
    }

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        if (g_ai_drv.ap_ai_state[i] == HI_NULL) {
            break;
        }
    }

    if (i >= AI_MAX_TOTAL_NUM) {
        HI_ERR_AI("too many ai channel!\n");
        return HI_ERR_AI_NOTSUPPORT;
    }

    state = (ai_chn_state *)osal_kmalloc(HI_ID_AI, sizeof(ai_chn_state), OSAL_GFP_KERNEL);
    if (state == HI_NULL) {
        HI_ERR_AI("call osal_kmalloc failed!\n");
        return HI_ERR_AI_NULL_PTR;
    }

    ret = memset_s(state, sizeof(ai_chn_state), 0, sizeof(ai_chn_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out;
    }

    state->ai_port = ai_port;
    ret = memcpy_s(&state->snd_port_attr, sizeof(hi_ai_attr), ai_attr, sizeof(hi_ai_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        goto out;
    }

    ai_init_chn_state(state);

    state->file = file;
    g_ai_drv.ap_ai_state[i] = state;

    h_ai = ((HI_ID_AI << 16) | i);

    state->h_ai = h_ai;

    ret = ai_alloc_buf(state);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ai_alloc_buf, ret);
        goto out;
    }

    *ph_handle = h_ai;

    return HI_SUCCESS;

out:
    osal_kfree(HI_ID_AI, state);
    return ret;
}

static hi_void ai_free_handle(hi_handle h_ai)
{
    ai_chn_state *state = HI_NULL;

    h_ai &= AI_CHNID_MASK;
    if (h_ai >= AI_MAX_TOTAL_NUM) {
        return;
    }

    state = g_ai_drv.ap_ai_state[h_ai];
    if (state != HI_NULL) {
        ai_free_buf(state);
        osal_kfree(HI_ID_AI, state);
    }

    g_ai_drv.ap_ai_state[h_ai] = HI_NULL;
}

static hi_s32 ai_create_chn(ai_chn_state *state)
{
    hi_s32 ret;
    aiao_port_user_cfg hw_port_attr;
    hi_ai_attr *attr = &state->snd_port_attr;

    aiao_port_id port = AIAO_PORT_MAX;

    switch (state->ai_port) {
        case HI_AI_I2S0:
            hal_aiao_get_board_rx_i2s_df_attr(0x0, &port, &hw_port_attr);
            break;

        case HI_AI_I2S1:
            hal_aiao_get_board_rx_i2s_df_attr(0x1, &port, &hw_port_attr);
            break;

        case HI_AI_ADC0:
        case HI_AI_ADC1:
        case HI_AI_ADC2:
        case HI_AI_ADC3:
        case HI_AI_ADC4:
            port2_tianlai_sel(state->ai_port, attr->un_attr.adc_attr.by_pass);
            hal_aiao_get_rx_adc_df_attr(port, &hw_port_attr);
            break;

        case HI_AI_HDMI0:
        case HI_AI_HDMI1:
        case HI_AI_HDMI2:
        case HI_AI_HDMI3:
            hal_aiao_get_rx_hdmi_df_attr(port, &hw_port_attr);
            break;

        case HI_AI_SIF0:
            hal_aiao_get_rx_sif_df_attr(port, &hw_port_attr);
            break;

        default:
            HI_ERR_AI("ai_port is invalid!\n");
            return HI_ERR_AI_INVALID_PARA;
    }

    hw_port_attr.buf_config.period_number = state->rbf_mmz.size / hw_port_attr.buf_config.period_buf_size;

    if (state->alsa == HI_TRUE) {
        ai_alsa_param *alsa_attr = (ai_alsa_param *)state->alsa_para;
        if (alsa_attr == HI_NULL) {
            HI_ERR_AI("alsa_para is null!\n");
            return HI_ERR_AI_INVALID_PARA;
        }

        hw_port_attr.ext_dma_mem = HI_TRUE;
        hw_port_attr.ext_mem.buf_phy_addr = alsa_attr->buf.buf_phy_addr;
        hw_port_attr.ext_mem.buf_vir_addr = alsa_attr->buf.buf_vir_addr;
        hw_port_attr.ext_mem.buf_size = alsa_attr->buf.buf_size;
        hw_port_attr.isr_func = alsa_attr->isr_func;
        hw_port_attr.substream = alsa_attr->substream;
        hw_port_attr.buf_config.period_buf_size = alsa_attr->buf.period_byte_size;
        hw_port_attr.buf_config.period_number = alsa_attr->buf.periods;
    } else {
        hw_port_attr.ext_dma_mem = HI_TRUE;
        hw_port_attr.ext_mem.buf_phy_addr = state->rbf_mmz.phys_addr;
        hw_port_attr.ext_mem.buf_vir_addr = state->rbf_mmz.virt_addr - (hi_u8 *)HI_NULL;
        hw_port_attr.ext_mem.buf_size = state->rbf_mmz.size;
    }

    ai_set_port_if_attr(state->ai_port, attr, &hw_port_attr.if_attr);
    ret = hal_aiao_open(port, &hw_port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("hal_aiao_open failed\n");
        return HI_FAILURE;
    }

    state->curn_status = AI_CHN_STATUS_STOP;
    state->port = port;

    return HI_SUCCESS;
}

static hi_void ai_destory_chn(ai_chn_state *state)
{
    hal_aiao_close(state->port);
}

static hi_s32 ai_create(hi_handle h_ai)
{
    hi_s32 ret;

    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    ret = ai_create_chn(state);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("ai_create_chn failed(0x%x)!\n", ret);
        return ret;
    }

    state->attach = HI_FALSE;

#if defined(HI_ALSA_AI_SUPPORT)
    if (state->alsa == HI_TRUE && state->alsa_para != HI_NULL) {
        ret = memcpy_s(&g_alsa_attr, sizeof(ai_alsa_param), state->alsa_para, sizeof(ai_alsa_param));
        if (ret != EOK) {
            HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        }
    }
#endif

    ai_create_set_flag(state);

#ifdef HI_PROC_SUPPORT
    ai_reg_proc(state);
#endif

    return ret;
}

static hi_s32 ai_destory(hi_handle h_ai)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

#ifdef HI_PROC_SUPPORT
    ai_unreg_proc(state);
#endif

    ai_destory_chn(state);
    ai_destory_set_flag(state);

    return HI_SUCCESS;
}

static hi_s32 ai_set_attr(hi_handle h_ai, hi_ai_attr *attr)
{
    hi_s32 ret;
    aiao_port_attr port_attr;

    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    ret = ai_check_attr(state->ai_port, attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("invalid ai attr!\n");
        return ret;
    }

    if (state->curn_status != AI_CHN_STATUS_STOP) {
        HI_ERR_AI("current state is not stop, can not set attr!\n");
        return HI_FAILURE;
    }

    ret = hal_aiao_get_attr(state->port, &port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("hal_aiao_get_attr failed(0x%x)!", ret);
        return ret;
    }

    ai_set_port_if_attr(state->ai_port, attr, &port_attr.if_attr);

    ret = hal_aiao_set_attr(state->port, &port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("hal_aiao_set_attr failed(0x%x)!", ret);
        return ret;
    }

    ret = memcpy_s(&state->snd_port_attr, sizeof(hi_ai_attr), attr, sizeof(hi_ai_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ai_get_attr(hi_handle h_ai, hi_ai_attr *ai_attr)
{
    hi_s32 ret;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    ret = memcpy_s(ai_attr, sizeof(hi_ai_attr), &state->snd_port_attr, sizeof(hi_ai_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 ai_get_port_attr(hi_handle h_ai, aiao_port_attr *port_attr)
{
    hi_s32 ret;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    ret = hal_aiao_get_attr(state->port, port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("hal_aiao_get_attr failed(0x%x)!", ret);
    }

    return ret;
}

static hi_s32 ai_check_delay_comps(ai_chn_state *state, hi_u32 compensation_ms)
{
    hi_u32 buf_delay_ms = 0;

    if (state->snd_port_attr.sample_rate) {
        buf_delay_ms = state->snd_port_attr.pcm_samples_per_frame * state->snd_port_attr.pcm_frame_max_num * 1000 /
            state->snd_port_attr.sample_rate;
    }

    if (compensation_ms > buf_delay_ms) {
        HI_ERR_AI("compensation_ms(%d) exceed buf_delay_ms(%d)!\n", compensation_ms, buf_delay_ms);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 ai_set_delay_comps(hi_handle h_ai, hi_ai_delay *delay_comps)
{
    hi_s32 ret;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    if (state->curn_status != AI_CHN_STATUS_STOP) {
        HI_ERR_AI("current state is not stop,can not set delay compensation!\n");
        return HI_FAILURE;
    }

    ret = ai_check_delay_comps(state, delay_comps->delay);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = memcpy_s(&state->delay_comps, sizeof(hi_ai_delay), delay_comps, sizeof(hi_ai_delay));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ai_get_delay_comps(hi_handle h_ai, hi_ai_delay *delay_comps)
{
    hi_s32 ret;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    ret = memcpy_s(delay_comps, sizeof(hi_ai_delay), &state->delay_comps, sizeof(hi_ai_delay));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_u32 ai_calc_acquire_size(hi_handle h_ai)
{
    hi_u32 need_bytes;
    hi_u32 frame_size;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    if (state == HI_NULL) {
        return 0;
    }

    frame_size = autil_calc_frame_size(state->channels, state->bit_per_sample);
    need_bytes = frame_size * state->snd_port_attr.pcm_samples_per_frame;

    return need_bytes;
}

static hi_s32 ai_acquire_frame(hi_handle h_ai, hi_u32 need_bytes)
{
    hi_u32 read_bytes;
    hi_u32 frame_size;
    hi_u32 avail_bytes = 0;
    hi_u32 wait_ms = 0;
    hi_u32 timeout_ms = 1; /* default wait time */
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    state->proc_attr.proc_info->aqc_try_cnt++;

    if (state->curn_status == AI_CHN_STATUS_STOP) {
        HI_ERR_AI("current state is stop, can not get frame!\n");
        return HI_FAILURE;
    }

    if (state->ai_buf.size < need_bytes) {
        HI_ERR_AI("need_bytes(%d) is too big than size(%d)!\n", need_bytes, state->ai_buf.size);
        return HI_FAILURE;
    }

    frame_size = autil_calc_frame_size(state->channels, state->bit_per_sample);
    if (state->snd_port_attr.sample_rate && frame_size) {
        timeout_ms = (need_bytes * 1000) / (state->snd_port_attr.sample_rate * frame_size);
    }

    while (1) {
        avail_bytes = hal_aiao_query_buf_data(state->port);
        if (avail_bytes >= need_bytes) {
            break;
        }

        if (wait_ms++ > timeout_ms) {
            HI_ERR_AI("query buf_data time out! need_bytes(%d), avail_bytes(%d)\n", need_bytes, avail_bytes);
            return HI_ERR_AI_NOT_ENOUGH_DATA;
        }

        osal_msleep(1);
    }

    read_bytes = hal_aiao_read_data(state->port, (hi_u8 *)HI_NULL + state->ai_buf.kernel_vir_addr, need_bytes);
    if (read_bytes != need_bytes) {
        HI_ERR_AI("hal_aiao_read_data failed! read_bytes(%d)!=need_bytes(%d)\n", read_bytes, need_bytes);
        return HI_ERR_AI_NOT_ENOUGH_DATA;
    }

    state->proc_attr.proc_info->aqc_cnt++;

    return HI_SUCCESS;
}

static hi_s32 ai_release_frame(hi_handle h_ai)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    state->proc_attr.proc_info->rel_try_cnt++;
    state->proc_attr.proc_info->rel_cnt++;
    return HI_SUCCESS;
}

static hi_s32 ai_get_ai_buf_info(hi_handle h_ai, ai_buf_attr *ai_buf)
{
    hi_s32 ret;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    ret = memcpy_s(ai_buf, sizeof(ai_buf_attr), &state->ai_buf, sizeof(ai_buf_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ai_set_ai_buf_info(hi_handle h_ai, ai_buf_attr *ai_buf)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    state->ai_buf.user_vir_addr = ai_buf->user_vir_addr;

    return HI_SUCCESS;
}

static hi_s32 ai_proc_deinit(hi_handle h_ai)
{
    hi_s32 ret;
    ai_proc_attr *proc_attr = HI_NULL;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    proc_attr = &state->proc_attr;
    if (proc_attr->used == HI_FALSE) {
        return HI_SUCCESS;
    }

    hi_drv_audio_mmz_release(&proc_attr->ai_proc_mmz);

    ret = memset_s(proc_attr, sizeof(ai_proc_attr), 0, sizeof(ai_proc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ai_proc_init(hi_handle h_ai, hi_audio_buffer *proc_buf)
{
    hi_s32 ret;
    hi_char mmz_name[16] = {0};
    ai_proc_attr *proc_attr = HI_NULL;
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    proc_attr = &state->proc_attr;
    if (proc_attr->used == HI_TRUE) {
        HI_ERR_AI("proc is already registered!");
        return HI_SUCCESS;
    }

    h_ai &= AI_CHNID_MASK;

    ret = snprintf(mmz_name, sizeof(mmz_name), "%s%02d", "ai_proc", h_ai);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return ret;
    }

    ret = hi_drv_audio_mmz_alloc(mmz_name, sizeof(ai_proc_info), HI_FALSE, &proc_attr->ai_proc_mmz);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_audio_mmz_alloc, ret);
        return ret;
    }

    proc_attr->proc_info = (ai_proc_info *)proc_attr->ai_proc_mmz.virt_addr;
    ret = memset_s(proc_attr->proc_info, sizeof(ai_proc_info), 0, sizeof(ai_proc_info));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out;
    }

    proc_attr->used = HI_TRUE;

    ret = memcpy_s(proc_buf, sizeof(hi_audio_buffer), &proc_attr->ai_proc_mmz, sizeof(hi_audio_buffer));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        goto out;
    }

    return HI_SUCCESS;

out:
    proc_attr->used = HI_FALSE;
    hi_drv_audio_mmz_release(&proc_attr->ai_proc_mmz);
    return ret;
}

hi_s32 ai_set_enable(hi_handle h_ai, hi_bool enable, hi_bool track_resume)
{
    hi_s32 ret;
    hi_u32 ai_delay_ms = 0;
    hi_u32 ao_delay_ms = 0;
    hi_u32 bytes_size = 0;
    hi_u32 frame_size = 0;
    aiao_port_attr port_attr;

    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    ret = memset_s(&port_attr, sizeof(aiao_port_attr), 0, sizeof(aiao_port_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    if (enable) {
        ret = hal_aiao_get_attr(state->port, &port_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_AI("hal_aiao_get_attr(%d) failed\n", state->port);
            return ret;
        }
        frame_size = autil_calc_frame_size(port_attr.if_attr.ch_num, port_attr.if_attr.bit_depth);

        if (track_resume) {
            bytes_size = autil_latency_ms_to_byte_size(20, frame_size, port_attr.if_attr.rate);
            hal_aiao_write_data(state->port, HI_NULL, bytes_size);
        }
        hal_aiao_get_delay_ms(state->port, &ai_delay_ms);
        if (state->attach == HI_TRUE) {
            ret = ao_track_get_delay_ms(state->h_track, &ao_delay_ms);
            if (ret != HI_SUCCESS) {
                HI_ERR_AI("ao_track_get_delay_ms(%d) failed\n", state->h_track);
                return ret;
            }
        }

        if (ai_delay_ms + ao_delay_ms < state->delay_comps.delay) {
            bytes_size = autil_latency_ms_to_byte_size(state->delay_comps.delay - ai_delay_ms - ao_delay_ms, frame_size,
                port_attr.if_attr.rate);
            hal_aiao_write_data(state->port, HI_NULL, bytes_size);
        }

        ret = hal_aiao_start(state->port);
        if (ret != HI_SUCCESS) {
            HI_ERR_AI("hal_aiao_start(%d) failed\n", state->port);
        } else {
            state->curn_status = AI_CHN_STATUS_START;
        }
    } else {
#ifdef HI_PROC_SUPPORT
        ai_proc_save_data_stop(state);
#endif
        ret = hal_aiao_stop(state->port, AIAO_STOP_IMMEDIATE);
        if (ret != HI_SUCCESS) {
            HI_ERR_AI("hal_aiao_stop(%d) failed\n", state->port);
        } else {
            state->curn_status = AI_CHN_STATUS_STOP;
        }
    }

    return ret;
}

hi_s32 ai_get_enable(hi_handle h_ai, hi_bool *enable)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    if (state->curn_status == AI_CHN_STATUS_START) {
        *enable = HI_TRUE;
    } else if (state->curn_status == AI_CHN_STATUS_STOP) {
        *enable = HI_FALSE;
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 ai_get_delay_ms(hi_handle h_ai, hi_u32 *delay_ms)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    *delay_ms = state->delay_comps.delay;

    return HI_SUCCESS;
}

hi_s32 ai_get_port_buf(hi_handle h_ai, aiao_rbuf_attr *aiao_buf)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    return hal_aiao_get_rbf_attr(state->port, aiao_buf);
}

hi_s32 ai_attach_track(hi_handle h_ai, hi_handle h_track)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    if ((state->attach == HI_TRUE) && (state->h_track != h_track)) {
        HI_ERR_AI("AI is attached, can not be attached again!\n");
        return HI_FAILURE;
    }

    state->attach = HI_TRUE;
    state->h_track = h_track;

    return HI_SUCCESS;
}

hi_s32 ai_detach_track(hi_handle h_ai, hi_handle h_track)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    CHECK_AI_CHN_STATE(h_ai, state);

    if ((state->attach != HI_TRUE) || (state->h_track != h_track)) {
        HI_ERR_AI("track(0x%x) is not attach this AI channel, can not detach!\n", h_track);
        return HI_FAILURE;
    }

    state->attach = HI_FALSE;
    state->h_track = HI_INVALID_HANDLE;

    return HI_SUCCESS;
}

hi_bool ai_check_is_hdmi_port(hi_handle h_ai)
{
    ai_chn_state *state = g_ai_drv.ap_ai_state[h_ai & AI_CHNID_MASK];
    if (state == HI_NULL) {
        HI_ERR_AI("AI chn(%d) not open!\n", h_ai);
        return HI_FALSE;
    }

    if ((state->ai_port == HI_AI_HDMI0) || (state->ai_port == HI_AI_HDMI1) || (state->ai_port == HI_AI_HDMI2) ||
        (state->ai_port == HI_AI_HDMI3)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 ai_open_dev(hi_void)
{
    hi_u32 i;

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        g_ai_drv.ap_ai_state[i] = HI_NULL;
    }

    hal_aiao_init();

    return HI_SUCCESS;
}

static hi_s32 ai_close_dev(hi_void)
{
    hi_u32 i;

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        g_ai_drv.ap_ai_state[i] = HI_NULL;
    }

    hal_aiao_de_init();

    return HI_SUCCESS;
}

static hi_s32 ai_process_cmd(hi_u32 cmd, hi_void *arg, hi_void *file)
{
    hi_s32 ret = HI_SUCCESS;

    switch (cmd) {
        case CMD_AI_GETDEFAULTATTR: {
            ai_get_df_attr_param_p param = (ai_get_df_attr_param_p)arg;
            ret = ai_get_default_attr(param->ai_port, &param->attr);
            break;
        }

        case CMD_AI_CREATE: {
            hi_handle h_ai = HI_INVALID_HANDLE;
            ai_create_param_p param = (ai_create_param_p)arg;

            ret = ai_alloc_handle(&h_ai, file, param->ai_port, &param->attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_AI("ai_alloc_handle failed(0x%x)!\n", ret);
                break;
            }

            ret = ai_create(h_ai);
            if (ret != HI_SUCCESS) {
                HI_ERR_AI("ai_create failed(0x%x)!\n", ret);
                ai_free_handle(h_ai);
                break;
            }
            param->ai = h_ai;

            break;
        }
        case CMD_AI_DESTROY: {
            hi_handle h_ai = *(hi_handle *)arg;
            CHECK_AI_ID(h_ai);
            ret = ai_destory(h_ai);
            if (ret != HI_SUCCESS) {
                break;
            }

            ai_free_handle(h_ai);
            break;
        }

        case CMD_AI_SETENABLE: {
            ai_enable_param_p param = (ai_enable_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_set_enable(param->ai, param->ai_enable, HI_FALSE);
            break;
        }

        case CMD_AI_GETENABLE: {
            ai_enable_param_p param = (ai_enable_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_get_enable(param->ai, &param->ai_enable);
            break;
        }

        case CMD_AI_ACQUIREFRAME: {
            ai_frame_param_p param = (ai_frame_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_acquire_frame(param->ai, param->need_bytes);
            break;
        }

        case CMD_AI_RELEASEFRAME: {
            hi_handle h_ai = *(hi_handle *)arg;
            CHECK_AI_ID(h_ai);
            ret = ai_release_frame(h_ai);
            break;
        }

        case CMD_AI_SETATTR: {
            ai_attr_param_p param = (ai_attr_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_set_attr(param->ai, &param->attr);
            break;
        }

        case CMD_AI_GETATTR: {
            ai_attr_param_p param = (ai_attr_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_get_attr(param->ai, &param->attr);
            break;
        }

        case CMD_AI_GETBUFINFO: {
            ai_buf_param_p param = (ai_buf_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_get_ai_buf_info(param->ai, &param->ai_buf);
            break;
        }

        case CMD_AI_SETBUFINFO: {
            ai_buf_param_p param = (ai_buf_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_set_ai_buf_info(param->ai, &param->ai_buf);
            break;
        }

        case CMD_AI_SETDELAYCOMPS: {
            ai_delay_comps_param_p param = (ai_delay_comps_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_set_delay_comps(param->ai, &param->delay_comps);
            break;
        }

        case CMD_AI_GETDELAYCOMPS: {
            ai_delay_comps_param_p param = (ai_delay_comps_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_get_delay_comps(param->ai, &param->delay_comps);
            break;
        }

        case CMD_AI_PROCINIT: {
            hi_audio_buffer proc_buf = { 0 };
            ai_proc_init_param_p param = (ai_proc_init_param_p)arg;
            CHECK_AI_ID(param->ai);
            ret = ai_proc_init(param->ai, &proc_buf);
            param->proc_phy_addr = proc_buf.phys_addr;
            param->proc_buf_size = proc_buf.size;
            param->proc_buf_map_fd = proc_buf.fd;
            break;
        }

        case CMD_AI_PROCDEINIT: {
            hi_handle h_ai = *(hi_handle *)arg;
            CHECK_AI_ID(h_ai);
            ret = ai_proc_deinit(h_ai);
            break;
        }

        default: {
            ret = HI_ERR_AI_INVALID_PARA;
            HI_WARN_AI("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

hi_s32 ai_drv_ioctl(hi_u32 cmd, hi_void *arg, hi_void *file)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_ai_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("lock g_ai_mutex failed\n");
        return ret;
    }

    ret = ai_process_cmd(cmd, arg, file);

    osal_sem_up(&g_ai_mutex);
    return ret;
}

hi_s32 ai_drv_open(hi_void *filp)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_ai_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("lock g_ai_mutex failed\n");
        return ret;
    }

    if (osal_atomic_inc_return(&g_ai_drv.atm_open_cnt) == 1) {
        ret = ai_open_dev();
        if (ret != HI_SUCCESS) {
            HI_FATAL_AI("call ai_open_dev failed(0x%x)!", ret);
        }
    }

    osal_sem_up(&g_ai_mutex);
    return ret;
}

hi_s32 ai_drv_release(hi_void *filp)
{
    hi_s32 ret, i;
    hi_handle h_ai;

    ret = osal_sem_down_interruptible(&g_ai_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("lock g_ai_mutex failed\n");
        return ret;
    }

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        if (g_ai_drv.ap_ai_state[i] == HI_NULL) {
            continue;
        }

        if (filp != g_ai_drv.ap_ai_state[i]->file) {
            continue;
        }

        h_ai = (hi_handle)i;
        ret = ai_proc_deinit(h_ai);
        if (ret != HI_SUCCESS) {
            HI_ERR_AI("ai_proc_deinit failed(h_ai = %d, ret = 0x%x)!\n", h_ai, ret);
        }

        ret = ai_destory(h_ai);
        if (ret != HI_SUCCESS) {
            HI_ERR_AI("ai_destory failed(h_ai = %d, ret = 0x%x)!\n", h_ai, ret);
        }

        ai_free_handle(h_ai);
    }

    if (osal_atomic_dec_return(&g_ai_drv.atm_open_cnt) == 0) {
        ret = ai_close_dev();
        if (ret != HI_SUCCESS) {
            HI_FATAL_AI("call ai_close_dev failed(0x%x)!", ret);
        }
    }

    osal_sem_up(&g_ai_mutex);

    return ret;
}

#ifdef HI_PROC_SUPPORT
static hi_s32 ai_save_thread(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 read_bytes;
    hi_u32 need_bytes;
    hi_u32 data_bytes;
    hi_u32 write_pos = 0;

    ai_chn_state *state = (ai_chn_state *)arg;

    if ((state->save_buf == HI_NULL) || (state->file_handle == HI_NULL)) {
        return HI_ERR_AI_NULL_PTR;
    }

    if (state->curn_status == AI_CHN_STATUS_STOP) {
        HI_ERR_AI("current state is stop, can not save frame!\n");
        return HI_ERR_AI_NOTSUPPORT;
    }

    need_bytes = ai_calc_acquire_size(state->h_ai);

    while (!osal_kthread_should_stop()) {
        data_bytes = hal_aiao_query_buf_data_provide_rptr(state->port, &state->save_read_pos);
        if (data_bytes < need_bytes) {
            osal_msleep(1);
            continue;
        }

        read_bytes = hal_aiao_read_data_not_up_rptr(state->port, (hi_u8 *)state->save_buf, need_bytes,
            &state->save_read_pos, &write_pos);
        if (read_bytes != need_bytes) {
            HI_ERR_AI("hal_aiao_read_data_not_up_rptr error!\n");
            continue;
        }
        ret = osal_klib_fwrite((hi_s8 *)state->save_buf, need_bytes, state->file_handle);
        if (ret < 0) {
            HI_ERR_AI("osal_klib_fwrite return err(0x%x)!\n", ret);
        }
    }

    return HI_SUCCESS;
}

static hi_void ai_proc_save_data_stop(ai_chn_state *state)
{
    if (state->save_thread != HI_NULL) {
        osal_kthread_destroy(state->save_thread, 1);
        state->save_thread = HI_NULL;
    }

    if (state->file_handle != HI_NULL) {
        osal_klib_fclose(state->file_handle);
        state->file_handle = HI_NULL;
    }

    if (state->save_buf != HI_NULL) {
        osal_kfree(HI_ID_AI, state->save_buf);
        state->save_buf = HI_NULL;
    }
}

static hi_s32 ai_proc_save_data_start(ai_chn_state *state)
{
    hi_s32 ret;
    hi_u32 ai = state->h_ai & AI_CHNID_MASK;
    hi_char path[AI_PATH_NAME_MAXLEN + AI_FILE_NAME_MAXLEN] = { 0 };
    osal_rtc_time now = { 0 };
    osal_timeval tv = { 0 };

    if (osal_klib_get_store_path(path, AI_PATH_NAME_MAXLEN) != HI_SUCCESS) {
        HI_ERR_AI("get store path failed\n");
        return HI_FAILURE;
    }
    osal_get_timeofday(&tv);
    osal_rtc_time_to_tm(tv.tv_sec, &now);

    ret = snprintf(path, sizeof(path), "%s/ai%d_%02u_%02u_%02u.pcm", path, ai, now.tm_hour, now.tm_min, now.tm_sec);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return ret;
    }

    state->file_handle = osal_klib_fopen(path, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, OSAL_O_ACCMODE);
    if (state->file_handle == HI_NULL) {
        HI_ERR_AI("open %s error\n", path);
        return HI_FAILURE;
    }

    state->save_buf = osal_kmalloc(HI_ID_AI, state->ai_buf.size, OSAL_GFP_KERNEL);
    if (state->save_buf == HI_NULL) {
        HI_ERR_AI("malloc save buffer failed\n");
        goto out;
    }

    state->save_thread = osal_kthread_create(ai_save_thread, state, "ai_save_data_thread", 0);
    if (state->save_thread == HI_NULL) {
        HI_ERR_AI("creat ai save thread failed\n");
        goto out;
    }

    osal_printk("started saving ai data to %s\n", path);

    state->save_cnt++;
    return HI_SUCCESS;

out:
    ai_proc_save_data_stop(state);
    return HI_FAILURE;
}

static hi_s32 ai_write_proc(ai_chn_state *state, ai_cmd_ctrl cmd)
{
    if ((cmd == AI_CMD_CTRL_START) && (state->save_state == AI_CMD_CTRL_STOP)) {
        ai_proc_save_data_start(state);
    } else if ((cmd == AI_CMD_CTRL_START) && (state->save_state == AI_CMD_CTRL_START)) {
        osal_printk("started saving ai data already!\n");
    }

    if ((cmd == AI_CMD_CTRL_STOP) && (state->save_state == AI_CMD_CTRL_START)) {
        ai_proc_save_data_stop(state);
        osal_printk("finished saving ai data.\n");
    } else if ((cmd == AI_CMD_CTRL_STOP) && (state->save_state == AI_CMD_CTRL_STOP)) {
        osal_printk("no ai data saved, please start saving.\n");
        return HI_SUCCESS;
    }

    state->save_state = cmd;

    return HI_SUCCESS;
}

static hi_void ai_show_spec_attr(hi_void *p, ai_chn_state *state)
{
    hi_ai_i2s_attr i2s_attr;
    hi_ai_hdmi_attr hdmi_attr;

    switch (state->ai_port) {
        case HI_AI_I2S0:
        case HI_AI_I2S1:
            i2s_attr = state->snd_port_attr.un_attr.i2s_attr;
            osal_proc_print(p,
                "channel                              :%d\n"
                "bit_width                            :%d\n"
                "clk_mode                             :%s\n"
                "i2s_mode                             :%s\n"
                "mclk/fs                              :%d\n"
                "mclk/bclk                            :%d\n"
                "sample_edge                          :%s\n"
                "delay_cycle                          :%d\n",
                i2s_attr.attr.channel, i2s_attr.attr.bit_depth, (i2s_attr.attr.master == HI_TRUE) ? "master" : "slave",
                (i2s_attr.attr.i2s_mode == HI_I2S_STD_MODE) ?
                "standard" : "pcm", autil_mclk_fclk_div(i2s_attr.attr.mclk), i2s_attr.attr.bclk,
                (i2s_attr.attr.pcm_sample_rise_edge == HI_TRUE) ?
                "positive" : "negative", i2s_attr.attr.pcm_delay_cycle);
            break;
        case HI_AI_HDMI0:
        case HI_AI_HDMI1:
        case HI_AI_HDMI2:
        case HI_AI_HDMI3:
            hdmi_attr = state->snd_port_attr.un_attr.hdmi_attr;
            osal_proc_print(p,
                "channel                              :%d\n"
                "bit_width                            :%d\n"
                "format                               :%s\n",
                hdmi_attr.channel, hdmi_attr.bit_depth,
                (hdmi_attr.hdmi_audio_data_format == HI_AI_HDMI_FORMAT_LPCM) ?
                "pcm" : ((hdmi_attr.hdmi_audio_data_format == HI_AI_HDMI_FORMAT_LBR) ? "LBR" : "HBR"));
            break;
        default:
            break;
    }
}

static hi_s32 ai_show_chn_proc(hi_void *p, hi_u32 chn)
{
    hi_s32 ret;
    hi_u32 buf_size_used, buf_per_cent_used;
    aiao_port_id port;
    aiao_port_stauts port_status;
    ai_chn_state *state = HI_NULL;

    state = g_ai_drv.ap_ai_state[chn];

    port = state->port;

    ret = memset_s(&port_status, sizeof(aiao_port_stauts), 0, sizeof(aiao_port_stauts));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    ret = hal_aiao_get_status(port, &port_status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_get_status, ret);
        return ret;
    }

    buf_size_used = circ_buf_query_busy(&port_status.circ_buf);

    buf_per_cent_used = buf_size_used * 100 / port_status.circ_buf.size;

    osal_proc_print(p, "\n--------------------- AI%d[%s] status ---------------------\n", chn,
        autil_ai_port_to_name(state->ai_port));
    osal_proc_print(p, "status                               :%s\n",
        (hi_char *)((AIAO_PORT_STATUS_START == port_status.status) ?
        "start" : ((AIAO_PORT_STATUS_STOP == port_status.status) ? "stop" : "stopping")));
    osal_proc_print(p, "sample_rate                          :%d\n", state->snd_port_attr.sample_rate);
    osal_proc_print(p, "pcm_frame_max_num                    :%d\n", state->snd_port_attr.pcm_frame_max_num);
    osal_proc_print(p, "pcm_samples_per_frame                :%d\n", state->snd_port_attr.pcm_samples_per_frame);

    ai_show_spec_attr(p, state);

    osal_proc_print(p, "delay_compensation                   :%dms\n", state->delay_comps.delay);
    osal_proc_print(p, "*ai_port                             :0x%.2x\n", port);
    osal_proc_print(p, "*alsa                                :%s\n", (state->alsa == HI_TRUE) ? "yes" : "no");
    osal_proc_print(p, "dma_cnt                              :%d\n", port_status.proc_status.dma_cnt);
    osal_proc_print(p, "buf_full_cnt                         :%d\n", port_status.proc_status.buf_full_cnt);
    osal_proc_print(p, "fifo_full_cnt                        :%d\n", port_status.proc_status.inf_fifo_full_cnt);
    osal_proc_print(p, "frame_buf(total/use/percent)(bytes)  :%d/%d/%d%%\n", port_status.buf.buff_size, buf_size_used,
        buf_per_cent_used);

    if (state->alsa == HI_FALSE) {
        osal_proc_print(p, "acquire_frame(try/OK)                :%d/%d\n", state->proc_attr.proc_info->aqc_try_cnt,
            state->proc_attr.proc_info->aqc_cnt);
        osal_proc_print(p, "release_frame(try/OK)                :%d/%d\n", state->proc_attr.proc_info->rel_try_cnt,
            state->proc_attr.proc_info->rel_cnt);
    }

    osal_proc_print(p, "-----------------------------------------------------------\n");

    return HI_SUCCESS;
}

static hi_s32 ai_drv_read_proc(hi_void *filp, hi_void *arg)
{
    hi_s32 i;

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        if (g_ai_drv.ap_ai_state[i] != HI_NULL) {
            ai_show_chn_proc(filp, i);
        }
    }

    return HI_SUCCESS;
}

static hi_s32 drv_ai_proc_cmd_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    ai_chn_state *state = (ai_chn_state *)private;
    if (state == HI_NULL) {
        return HI_ERR_AI_NULL_PTR;
    }

    osal_printk("---------------------------------------------AI-----------------------------------------\n");
    osal_printk("echo command      para              path            explanation\n\n");
    osal_printk("echo save        [start/stop]     > /proc/msp/ai%d  record pcm data from ai\n",
        state->h_ai & AI_CHNID_MASK);
    osal_printk("echo adc         [1/0]            > /proc/msp/ai%d  set adc power on/off\n",
        state->h_ai & AI_CHNID_MASK);
    osal_printk("echo NR          [normal/mute/off]> /proc/msp/ai%d  set AI NR mode\n",
        state->h_ai & AI_CHNID_MASK);
    osal_printk("echo set_nr_level  [-60~-100]     > /proc/msp/ai%d  set AI NR mute threshold\n",
        state->h_ai & AI_CHNID_MASK);
    osal_printk("echo delay       [0~500]          > /proc/msp/ai%d  ai set delay(step:1;unit:ms)\n",
        state->h_ai & AI_CHNID_MASK);
    osal_printk("----------------------------------------------------------------------------------------\n");

    return HI_SUCCESS;
}

static int drv_ai_proc_cmd_save(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_s32 ret;
    ai_cmd_ctrl ctrl_cmd;
    const hi_char *pc_start_cmd = "start";
    const hi_char *pc_stop_cmd = "stop";

    ai_chn_state *state = (ai_chn_state *)private;
    if (state == HI_NULL) {
        return HI_ERR_AI_NULL_PTR;
    }

    if (argc < 2) {
        return drv_ai_proc_cmd_help(argc, argv, private);
    }

    if (strstr(argv[1], pc_start_cmd)) {
        ctrl_cmd = AI_CMD_CTRL_START;
    } else if (strstr(argv[1], pc_stop_cmd)) {
        ctrl_cmd = AI_CMD_CTRL_STOP;
    } else {
        return drv_ai_proc_cmd_help(argc, argv, private);
    }

    ret = osal_sem_down_interruptible(&g_ai_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("lock g_ai_mutex failed!\n");
        return ret;
    }

    ret = ai_write_proc(state, ctrl_cmd);

    osal_sem_up(&g_ai_mutex);

    return ret;
}

static int drv_ai_proc_cmd_adc(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    return HI_SUCCESS;
}
static int drv_ai_proc_cmd_nr(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    return HI_SUCCESS;
}

static int drv_ai_proc_cmd_set_nr_level(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    return HI_SUCCESS;
}

static void ai_proc_set_delay(hi_u32 ai, ai_chn_state *state, hi_u32 ai_delay)
{
    hi_s32 ret;
    hi_u64 mid;
    hi_u32 buf_delay_ms = 0;
    hi_ai_delay get_delay_comps;
    hi_ai_delay set_delay_comps;
    ai_chn_status en_status_tmp;

    set_delay_comps.delay_auto_hold = HI_FALSE;
    set_delay_comps.delay = ai_delay;
    get_delay_comps.delay_auto_hold = HI_FALSE;
    get_delay_comps.delay = 0;

    en_status_tmp = state->curn_status;

    if (en_status_tmp == AI_CHN_STATUS_START) {
        ai_set_enable(ai, HI_FALSE, HI_FALSE);
    }

    if (state->snd_port_attr.sample_rate) {
        mid = (hi_u64)state->snd_port_attr.pcm_samples_per_frame * (hi_u64)state->snd_port_attr.pcm_frame_max_num;
        if (((mid * 1000) & 0xffffffff00000000) != 0x0) { /* 1000: change unit from s to ms */
            osal_printk("set ai%d delay faiure with wrong parameter! \n", ai);
        }
        buf_delay_ms = (hi_u32)mid * 1000 / state->snd_port_attr.sample_rate; /* 1000: change unit from s to ms */
    }

    if (set_delay_comps.delay <= buf_delay_ms) {
        ret = ai_get_delay_comps(ai, &get_delay_comps);
        if (ret != HI_SUCCESS) {
            osal_printk("ai_get_delay_comps failed!\n");
        }

        ret = ai_set_delay_comps(ai, &set_delay_comps);
        if (ret == HI_SUCCESS) {
            osal_printk("set ai%d delay success:%dms -> %dms\n", ai, get_delay_comps.delay, set_delay_comps.delay);
        } else {
            osal_printk("set ai%d delay faiure!\n", ai);
        }
    } else {
        osal_printk("set ai%d delay faiure! ai delay range: [0~%d]ms\n", ai, buf_delay_ms);
    }

    if (en_status_tmp == AI_CHN_STATUS_START) {
        ai_set_enable(ai, HI_TRUE, HI_FALSE);
    }
}

static int drv_ai_proc_cmd_delay(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_s32 ret;
    hi_u32 ai_delay;
    hi_char *pc_buf = HI_NULL;

    ai_chn_state *state = (ai_chn_state *)private;
    if (state == HI_NULL) {
        return HI_ERR_AI_NULL_PTR;
    }

    pc_buf = argv[1];
    if (pc_buf[0] < '0' || pc_buf[0] > '9') {
        return drv_ai_proc_cmd_help(argc, argv, private);
    }

    ai_delay = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);

    ret = osal_sem_down_interruptible(&g_ai_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("lock g_ai_mutex failed!\n");
        return ret;
    }

    ai_proc_set_delay(state->h_ai & AI_CHNID_MASK, state, ai_delay);

    osal_sem_up(&g_ai_mutex);

    return HI_SUCCESS;
}

static osal_proc_cmd g_ai_proc_cmd[] = {
    { "help", drv_ai_proc_cmd_help},
    { "save", drv_ai_proc_cmd_save},
    { "adc", drv_ai_proc_cmd_adc},
    { "NR", drv_ai_proc_cmd_nr},
    { "set_nr_level", drv_ai_proc_cmd_set_nr_level},
    { "delay", drv_ai_proc_cmd_delay},
};

static hi_void ai_reg_proc(ai_chn_state *state)
{
    hi_s32 ret;
    hi_char buf[16] = { 0 };
    osal_proc_entry *item = HI_NULL;

    state->save_state = AI_CMD_CTRL_STOP;

    /* proc name: ai0 or ai1 */
    ret = snprintf(buf, sizeof(buf), "ai%d", state->h_ai & AI_CHNID_MASK);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return;
    }

    /* create proc */
    item = osal_proc_add(buf, strlen(buf));
    if (item == HI_NULL) {
        HI_LOG_FATAL("create ai proc entry fail!\n");
        return;
    }

    /* set functions */
    item->read = ai_drv_read_proc;
    item->cmd_list = g_ai_proc_cmd;
    item->cmd_cnt = sizeof(g_ai_proc_cmd) / sizeof(osal_proc_cmd);
    item->private = state;

    HI_LOG_INFO("create ai proc entry OK!\n");
}

static hi_void ai_unreg_proc(ai_chn_state *state)
{
    hi_s32 ret;
    hi_char buf[16] = { 0 };

    ai_proc_save_data_stop(state);

    ret = snprintf(buf, sizeof(buf), "ai%d", state->h_ai & AI_CHNID_MASK);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return;
    }

    osal_proc_remove(buf, strlen(buf));
}
#endif

hi_s32 ai_drv_suspend(hi_void *private_data)
{
#if defined(HI_AI_DRV_SUSPEND_SUPPORT)
    hi_u32 i;
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_ai_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("lock g_ai_mutex failed\n");
        return ret;
    }

    if (osal_atomic_read(&g_ai_drv.atm_open_cnt) == 0) {
        osal_sem_up(&g_ai_mutex);
        return HI_SUCCESS;
    }

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        if (g_ai_drv.ap_ai_state[i] == HI_NULL) {
            continue;
        }

        ret = ai_destory(i);
        if (ret != HI_SUCCESS) {
            HI_FATAL_AI("AI destory fail\n");
            osal_sem_up(&g_ai_mutex);
            return HI_FAILURE;
        }
    }

    ret = hal_aiao_suspend();
    if (ret != HI_SUCCESS) {
        HI_FATAL_AI("AIAO suspend fail\n");
        osal_sem_up(&g_ai_mutex);
        return HI_FAILURE;
    }

    HI_PRINT("AI suspend OK\n");
#endif
    osal_sem_up(&g_ai_mutex);
    return HI_SUCCESS;
}

hi_s32 ai_drv_resume(hi_void *private_data)
{
#if defined(HI_AI_DRV_SUSPEND_SUPPORT)
    hi_s32 ret;
    hi_u32 i;
    hi_ai_port ai_port;
    hi_ai_attr ai_attr;
    hi_bool alsa = HI_FALSE;
    hi_void *alsa_para = HI_NULL;
    ai_chn_state *state = HI_NULL;
    ai_chn_status ai_status;

    ret = osal_sem_down_interruptible(&g_ai_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("lock g_ai_mutex failed\n");
        return ret;
    }

    if (osal_atomic_read(&g_ai_drv.atm_open_cnt) == 0) {
        osal_sem_up(&g_ai_mutex);
        return HI_SUCCESS;
    }

    ret = hal_aiao_resume();
    if (ret != HI_SUCCESS) {
        HI_FATAL_AI("call hal_aiao_resume failed(0x%x)!\n", ret);
        osal_sem_up(&g_ai_mutex);
        return ret;
    }

    for (i = 0; i < AI_MAX_TOTAL_NUM; i++) {
        if (g_ai_drv.ap_ai_state[i] == HI_NULL) {
            continue;
        }

        state = g_ai_drv.ap_ai_state[i];
        ai_port = state->ai_port;
        ai_attr = state->snd_port_attr;
        alsa = state->alsa;
        alsa_para = state->alsa_para;
        ai_status = state->curn_status;
#ifdef HI_ALSA_AI_SUPPORT
        if (alsa == HI_TRUE) {
            alsa_para = (void *)&g_alsa_attr;
        }
#endif
        ret = ai_create(i);
        if (ret != HI_SUCCESS) {
            HI_FATAL_AI("ai_create failed(0x%x)\n", ret);
            osal_sem_up(&g_ai_mutex);
            return HI_FAILURE;
        }

        if ((ai_status == AI_CHN_STATUS_START) && (!(state->attach))) {
            ret = ai_set_enable(i, HI_TRUE, HI_FALSE);
            if (ret != HI_SUCCESS) {
                HI_ERR_AI("set AI enable failed\n");
                osal_sem_up(&g_ai_mutex);
                return HI_FAILURE;
            }
        }
    }

    osal_sem_up(&g_ai_mutex);
    HI_PRINT("AI resume OK\n");
#endif
    return HI_SUCCESS;
}

hi_s32 ai_drv_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_AI, AI_NAME, (hi_void *)&g_ai_drv.ext_func);
    if (ret != HI_SUCCESS) {
        HI_FATAL_AI("osal_exportfunc_register HI_ID_AI failed(0x%x)!\n", ret);
    }

    return ret;
}

hi_void ai_drv_exit(hi_void)
{
    osal_exportfunc_unregister(HI_ID_AI);
}

hi_s32 hi_drv_ai_init(hi_void)
{
    return ai_drv_init();
}

hi_void hi_drv_ai_deinit(hi_void)
{
    ai_drv_exit();
}

hi_s32 hi_drv_ai_open(hi_void *file)
{
    return ai_drv_open(file);
}

hi_s32 hi_drv_ai_release(hi_void *file)
{
    return ai_drv_release(file);
}

hi_s32 hi_drv_ai_get_default_attr(hi_ai_port ai_port, hi_ai_attr *attr)
{
    CHECK_AI_NULL_PTR(attr);
    return ai_get_default_attr(ai_port, attr);
}

hi_s32 hi_drv_ai_create(ai_drv_create_param *ai_para, hi_void *file)
{
    hi_s32 ret;
    hi_handle h_ai = HI_INVALID_HANDLE;

    CHECK_AI_NULL_PTR(ai_para);

    ret = ai_alloc_handle(&h_ai, file, ai_para->ai_port, &ai_para->attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("ai_alloc_handle failed(0x%x)!", ret);
        return ret;
    }

#ifdef HI_ALSA_AI_SUPPORT
    if (ai_para->alsa == HI_TRUE) {
        ai_alsa_set_param(h_ai, ai_para->alsa_para);
    }
#endif

    ret = ai_create(h_ai);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("ai_create failed(0x%x)!", ret);
        ai_free_handle(h_ai);
    }

    ai_para->ai = h_ai;

    return ret;
}

hi_s32 hi_drv_ai_destroy(hi_handle h_ai)
{
    hi_s32 ret;

    ret = ai_destory(h_ai);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("ai_destory failed(0x%x)!", ret);
        return ret;
    }

    ai_free_handle(h_ai);

    return ret;
}

hi_s32 hi_drv_ai_set_enable(hi_handle h_ai, hi_bool enable)
{
    return ai_set_enable(h_ai, enable, HI_FALSE);
}

hi_s32 hi_drv_ai_get_enable(hi_handle h_ai, hi_bool *enable)
{
    CHECK_AI_NULL_PTR(enable);
    return ai_get_enable(h_ai, enable);
}

hi_s32 hi_drv_ai_get_attr(hi_handle h_ai, hi_ai_attr *attr)
{
    CHECK_AI_NULL_PTR(attr);
    return ai_get_attr(h_ai, attr);
}

hi_s32 hi_drv_ai_set_attr(hi_handle h_ai, hi_ai_attr *attr)
{
    CHECK_AI_NULL_PTR(attr);
    return ai_set_attr(h_ai, attr);
}

hi_s32 hi_drv_ai_acquire_frame(hi_handle h_ai, ao_frame *frame)
{
    hi_s32 ret;
    ai_buf_attr ai_buf = { 0 };
    CHECK_AI_NULL_PTR(frame);

    ret = ai_get_ai_buf_info(h_ai, &ai_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("ai_get_ai_buf_info failed(0x%x)!", ret);
        return ret;
    }

    ret = ai_acquire_frame(h_ai, ai_calc_acquire_size(h_ai));
    if (ret != HI_SUCCESS) {
        HI_ERR_AI("ai_acquire_frame failed(0x%x)!", ret);
        return ret;
    }

    frame->pcm_buffer = ai_buf.kernel_vir_addr;

    return HI_SUCCESS;
}

hi_s32 hi_drv_ai_release_frame(hi_handle h_ai)
{
    return ai_release_frame(h_ai);
}

hi_void ai_osal_init(hi_void)
{
    osal_sem_init(&g_ai_mutex, 1);
    osal_atomic_init(&g_ai_drv.atm_open_cnt);
}

hi_void ai_osal_deint(hi_void)
{
    osal_atomic_destory(&g_ai_drv.atm_open_cnt);
    osal_sem_destory(&g_ai_mutex);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
