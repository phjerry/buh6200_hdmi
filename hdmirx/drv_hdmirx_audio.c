/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of audio functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "drv_hdmirx_audio.h"
#include "drv_hdmirx_video.h"
#include "hal_hdmirx_depack_reg.h"
#include "hal_hdmirx_audio.h"
#include "hal_hdmirx_ctrl.h"
#include <securec.h>

static hdmirx_audio_context g_hdmirx_audio_ctx_v2[HI_DRV_HDMIRX_PORT_MAX];
static hi_bool g_audio_ready[HI_DRV_HDMIRX_PORT_MAX] = {HI_FALSE, HI_FALSE, HI_FALSE, HI_FALSE};

static const audio_fs_search g_audio_fs_list[AUDIO_FS_LIST_LENGTH + 1] = {
    { AUDIO_CHST4_FS_22,      220,  200,  230 },
    { AUDIO_CHST4_FS_24,      240,  230,  280 },
    { AUDIO_CHST4_FS_32,      320,  280,  380 },
    { AUDIO_CHST4_FS_44,      441,  380,  460 },
    { AUDIO_CHST4_FS_48,      480,  460,  540 },
    { AUDIO_CHST4_FS_88,      882,  820,  921 },
    { AUDIO_CHST4_FS_96,      960,  921,  1100 },
    { AUDIO_CHST4_FS_176,     1764, 1600, 1792 },
    { AUDIO_CHST4_FS_192,     1920, 1792, 2500 },
    { AUDIO_CHST4_FS_UNKNOWN, 0,    0,    0 }
};

static const hi_char *g_hdmirx_audio_coding_type_string[] = {
    "unknown",
    "PCM",
    "AC3",
    "MPEG1",
    "MP3",
    "MPEG2",
    "AAC",
    "DTS",
    "ATRAC",
    "one bit audio",
    "dolby digital +",
    "DTS HD",
    "MAT(MLP)",
    "DST",
    "WMA",
    "audio_type_butt"
};

static const hi_u32 g_audio_channel_mask[AUDIO_CHANNEL_MASK_TABLE_LENGTH] = {
    0x10, 0x30, 0x30, 0x30, 0x70, 0x70, 0x70, 0x70,
    0x70, 0x70, 0x70, 0x70, 0xF0, 0xF0, 0xF0, 0xF0,
    0xF0, 0xF0, 0xF0, 0xF0, 0xB0, 0xB0, 0xB0, 0xB0,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
};

static hi_void hdmirx_audio_show_audio_info(hi_drv_hdmirx_port port)
{
    hi_drv_ai_hdmirx_attr audio_info;

    hdmirxv2_audio_get_info(port, &audio_info);

    hi_dbg_hdmirx_print_u32("show audio info!");
    hi_dbg_hdmirx_print_u32(audio_info.channel);
    hi_dbg_hdmirx_print_u32(audio_info.bit_depth);
    hi_dbg_hdmirx_print_u32(audio_info.sample_rate);
    hi_dbg_hdmirx_print_u32(audio_info.hdmi_audio_data_format);
    hi_dbg_hdmirx_print_u32(audio_info.hdmi_audio_data_type);
}

static hi_void hdmirx_audio_set_aac_done_int_en(hi_drv_hdmirx_port port, hi_bool en)
{
}
static hi_void hdmirx_audio_refresh(hi_drv_hdmirx_port port)
{
    hi_u32 stat;

    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);

    stat = hal_audio_get_audp_state(port);
    /* 0x90: bit7 and bit1 is never set to 1; 0x60: bit5 and bit6 is never set to 1 at same time */
    if ((stat & 0x90) || ((stat & 0x60) == 0x60)) {
        /* it happens when a source sends encrypted data before authentication */
        /* (or forgets disabling HDCP encryption after a break in video clock). */
        /* no reason to continue- audio data is broken */
    } else {
        /* RX_A__AUDP_STAT looks correct */
        if (stat & HDMI_HBRA_ON) {
            /* HBR audio */
            audio_ctx->hbr_mode = HI_TRUE;
        } else {
            audio_ctx->hbr_mode = HI_FALSE;
        }
    }
}

hi_u32 hdmirxv2_audio_get_tmds_clk_10k(hi_drv_hdmirx_port port)
{
    hi_u32 tmds_clk_10k_hz;
    tmds_clk_10k_hz = hdmirxv2_video_get_pixel_clk(port);
    /* this is pix clock (not TMDS clock at that moment) in 100 hz units */
    if (hdmirxv2_packet_avi_get_color_space(port) == HDMIRX_COLOR_SPACE_YCBCR422) {
        return tmds_clk_10k_hz;
    } else {
        /* convert pixel clock into TMDS clock */
        switch (hdmirxv2_video_get_input_width(port)) {
            case HDMIRX_INPUT_WIDTH_30:                    /* DC 30 bit */
                tmds_clk_10k_hz = tmds_clk_10k_hz * 5 / 4; /* 5: times 5, 4: div 4, about equals *1.25 */
                break;
            case HDMIRX_INPUT_WIDTH_36:                    /* DC 36 bit */
                tmds_clk_10k_hz = tmds_clk_10k_hz * 3 / 2; /* 3: times 3, 2: div 2, about equals *1.5 */
                break;
            case HDMIRX_INPUT_WIDTH_48: /* DC 48 bit (reserved for the future) */
                tmds_clk_10k_hz *= 2;   /* *2 */
                break;
            default:
                break;
        }
        if (hdmirxv2_packet_avi_get_color_space(port) == HDMIRX_COLOR_SPACE_YCBCR420) {
            tmds_clk_10k_hz = tmds_clk_10k_hz / 2; /* 420 format per pixel has 2 clock */
        }

        return tmds_clk_10k_hz;
    }
}

hi_void hdmirxv2_audio_set_audio_ready(hi_drv_hdmirx_port port, hi_bool en)
{
    hi_dbg_hdmirx("[HDMI]set 2.0 ctrl audioready ");
    hi_dbg_hdmirx_print_u32(en);

    g_audio_ready[port] = en;
}

hi_bool hdmirxv2_audio_get_audio_ready(hi_drv_hdmirx_port port)
{
    return g_audio_ready[port];
}

static hi_bool hdmirx_audio_is_cts_error(hi_drv_hdmirx_port port)
{
    hi_bool err = HI_FALSE;

    if (hal_audio_is_audio_packet_got(port) == HI_FALSE) {
        err = HI_TRUE;
    } else if (hal_audio_is_cts_got(port) == HI_FALSE) {
        err = HI_TRUE;
    } else if (hal_audio_is_cts_stable(port) == HI_FALSE) {
        err = HI_TRUE;
    }

    return err;
}

/* wait until FIFO underrun and overrun are gone */
static hi_bool hdmirx_audio_is_fifo_stable(hi_drv_hdmirx_port port)
{
    hi_u32 timeout = AUDIO_CHECK_STABLE_THR;
    osal_msleep(1);
    do {
        /* reset audio FIFO */
        hal_ctrl_set_pwd_reset(port, HDMIRX_PWD_AUD_FIFO_SRST);

        /* clear FIFO underrun and overrun interrupts */
        hal_audio_set_intr(port, AUDIO_FIFO_UNDERRUN | AUDIO_FIFO_OVERRUN);
        osal_msleep(1);

        /* check whether the interrupt has raised */
        if (hal_audio_is_fifo_stable(port) == HI_TRUE) {
            break;  /* success */
        }
    } while (--timeout);

    if (0 != timeout) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

static hi_bool hdmirx_audio_is_status_changed(hi_drv_hdmirx_port port)
{
    hi_u32 channel_status_tmp[5]; /* 5: channel_statue has 20 bytes */
    hdmirx_audio_context *audio_ctx = HI_NULL;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    hal_audio_get_channel_state(port, channel_status_tmp, sizeof(channel_status_tmp) / sizeof(channel_status_tmp[0]));

    channel_status_tmp[3] &= ~CHST4_SAMPLE_F; /* 3: clear sample rate */

    if (AUDIO_CHST4_FS_UNKNOWN != audio_ctx->measured_fs) {
        /* replace with FW measured values */
        channel_status_tmp[3] |= audio_ctx->measured_fs; /* 3: set sample rate to measured value */
    } else {
        /* replace with HW measured values */
        channel_status_tmp[3] |= hal_audio_get_hw_sample_rate(port); /* 3: set sample rate to hw-values */
    }
    if (osal_memncmp(channel_status_tmp, 20, audio_ctx->channel_sta, 20) == 0) { /* 20: compare front 20 bytes */
        return HI_FALSE;
    }
    return HI_TRUE;
}

static hi_void hdmirx_audio_start(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    audio_ctx->start_req = HI_TRUE;
}

hi_void hdmirxv2_audio_set_exceptions_en(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    hal_audio_set_aac_mute_en(port, en);
    audio_ctx->exceptions_en = en;
}
static hi_void hdmirx_audio_on_off(hi_drv_hdmirx_port port, hi_bool on_off)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (on_off) {
        if (audio_ctx->audio_is_on == HI_FALSE) { /* reduce extra log prints */
            audio_ctx->audio_is_on = HI_TRUE;
        }
        hdmirxv2_audio_set_exceptions_en(port, HI_TRUE);
        hal_ctrl_set_mute_en(port, HDMIRX_MUTE_AUD, HI_FALSE);
    } else {
        if (audio_ctx->audio_is_on == HI_TRUE) { /* reduce extra log prints */
            audio_ctx->audio_is_on = HI_FALSE;
        }

        hal_ctrl_set_mute_en(port, HDMIRX_MUTE_AUD, HI_TRUE);
    }
    hdmirx_audio_set_aac_done_int_en(port, on_off);
}

hi_void hdmirxv2_audio_enable(hi_drv_hdmirx_port port, hi_bool on_off)
{
    hdmirx_audio_on_off(port, on_off);
}

static hi_void hdmirx_audio_change_state(hi_drv_hdmirx_port port, hdmix_audio_state new_state, hi_u32 delay)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    audio_ctx->audio_state = new_state;
    audio_ctx->time_out = delay;
}

hi_void hdmirxv2_audio_change_state(hi_drv_hdmirx_port port, hdmix_audio_state new_state, hi_u32 delay)
{
    hdmirx_audio_change_state(port, new_state, delay);
}

static hi_bool hdmirx_audio_is_spdif_out_enable(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (audio_ctx->protected || audio_ctx->hbr_mode || ((!audio_ctx->enc) && audio_ctx->ca)) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

hi_void hdmirxv2_audio_update_on_acp(hi_drv_hdmirx_port port, hdmirx_acp_type type)
{
    hi_bool proctect = HI_FALSE;
    hi_bool out_en;
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (type != ACP_GENERAL_AUDIO) {
        proctect = HI_TRUE;
    }
    if (audio_ctx->protected != proctect) {
        audio_ctx->protected = proctect;
        if ((audio_ctx->audio_state == AUDIO_STATE_ON) ||
            (audio_ctx->audio_state == AUDIO_STATE_READY)) {
            out_en = hdmirx_audio_is_spdif_out_enable(port);
        }
    }
}

static hi_void hdmirx_audio_set_mclk(hi_drv_hdmirx_port port)
{
    hi_u32 fs_code;
    hi_u32 cts;
    hi_u32 n;
    hi_u32 tmds_clk;
    hi_u32 fs_100_hz;
    hi_u32 i;
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    audio_ctx->measured_fs = AUDIO_CHST4_FS_UNKNOWN;
    fs_code = AUDIO_CHST4_FS_UNKNOWN;
    {
        n = hal_audio_get_n(port);
        cts = hal_audio_get_cts(port);
        tmds_clk = hdmirxv2_audio_get_tmds_clk_10k(port);
        if (tmds_clk && cts) {
            fs_100_hz = (tmds_clk * n / cts) * 100 / 128; /* 100, 128: 10k units to 100, times 100 */
            for (i = 0; i < AUDIO_FS_LIST_LENGTH; i++) {
                if ((fs_100_hz <= g_audio_fs_list[i].max_fs)
                    && (fs_100_hz > g_audio_fs_list[i].min_fs)) {
                    /* search if calculated fs close to the fs in the table */
                    break;
                }
            }
            fs_code = g_audio_fs_list[i].code;
        }
        if (audio_ctx->hbr_mode) {
            if (AUDIO_CHST4_FS_UNKNOWN == fs_code) {
                fs_code = AUDIO_CHST4_FS_192;
            }
        }
        if (fs_code != AUDIO_CHST4_FS_UNKNOWN) {
            audio_ctx->measured_fs = fs_code;
        } else {
            audio_ctx->measured_fs = hal_audio_get_hw_sample_rate(port);
        }
        if (audio_ctx->measured_fs == AUDIO_CHST4_FS_UNKNOWN) {
            audio_ctx->measured_fs = AUDIO_CHST4_FS_48;
        }
    }
}

static hi_void hdmirx_audio_set_out(hi_drv_hdmirx_port port)
{
}

hdmix_audio_state hdmirxv2_audio_getstatus(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);

    return audio_ctx->audio_state;
}

hi_void hdmirxv2_audio_initial(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;
    errno_t err_ret;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    err_ret = memset_s(audio_ctx, sizeof(hdmirx_audio_context), 0, sizeof(hdmirx_audio_context));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    hdmirx_audio_change_state(port, AUDIO_STATE_OFF, 0);

    /* means "not indicated", default values for 48k_hz audio will be used. */
    audio_ctx->fs = AUDIO_CHST4_FS_UNKNOWN;
    audio_ctx->soft_mute = HI_TRUE;

    /* default audio settings (set to I2S) */
    /* allow SPDIF clocking even if audio is not coming out */
    hdmirxv2_packet_reset_aud_info_frame_data(port);
    hdmirx_audio_on_off(port, HI_FALSE);
}

hi_void hdmirxv2_audio_set_ca(hi_drv_hdmirx_port port, hi_u32 ca)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    audio_ctx->ca = ca;
}

hi_void hdmirxv2_audio_set_coding_type(hi_drv_hdmirx_port port, hi_u32 coding_type)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (coding_type < RX_AUDIO_CODE_BUTT) {
        audio_ctx->audio_cod_type = (hdmirx_audio_code_type)coding_type;
    } else {
        audio_ctx->audio_cod_type = RX_AUDIO_CODE_UNKNOWN;
    }
}

hdmirx_audio_code_type hdmirxv2_audio_get_coding_type(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    return audio_ctx->audio_cod_type;
}

hi_void hdmirxv2_audio_update(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if ((audio_ctx->audio_state == AUDIO_STATE_ON) || (audio_ctx->audio_state == AUDIO_STATE_READY)) {
        hdmirx_audio_set_out(port);
        hdmirx_audio_set_mclk(port); /* couldn't it make audio restart if MCLK changed? */
    }
}

hi_void hdmirxv2_audio_aac_done(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (audio_ctx->exceptions_en) {
        hdmirxv2_packet_reset_aud_info_frame_data(port);
        hdmirxv2_audio_set_exceptions_en(port, HI_FALSE);
    }

    hdmirx_audio_on_off(port, HI_FALSE);

    hdmirx_audio_change_state(port, AUDIO_STATE_OFF, 0);
}

hi_void hdmirxv2_audio_stop(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    {
        hdmirxv2_audio_aac_done(port);
    }
    audio_ctx->start_req = HI_FALSE;
}

hi_bool hdmirxv2_audio_is_request(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);

    return audio_ctx->start_req;
}

hi_void hdmirxv2_audio_restart(hi_drv_hdmirx_port port)
{
    if (hdmirxv2_audio_is_request(port)) {
        hdmirxv2_audio_stop(port);
    }
    hdmirx_audio_start(port);
}

hi_void hdmirxv2_audio_stream_type_changed(hi_drv_hdmirx_port port)
{
    hdmirxv2_audio_stop(port);
    hdmirx_audio_start(port);
}

hi_void hdmirxv2_audio_on_channel_status_chg(hi_drv_hdmirx_port port)
{
    hdmirx_audio_context *audio_ctx = HDMIRX_AUDIO_GET_CTX(port);

    hal_audio_get_channel_state(port, audio_ctx->channel_sta,
        sizeof(audio_ctx->channel_sta) / sizeof(audio_ctx->channel_sta[0]));
    {
        if (audio_ctx->hbr_mode == HI_TRUE) {
            audio_ctx->enc = HI_TRUE;
        } else if ((audio_ctx->channel_sta[0] & AUDIO_CHST1_ENCODED) != 0) {
            audio_ctx->enc = HI_TRUE;
        } else {
            audio_ctx->enc = HI_FALSE;
        }
    }

    /*
     * fs data read from RX_CHST4 may be not the actual value coming
     * from HDMI input, but the fs written into RX_FREQ_SVAL.
     * to have real fs value, get it from RX_TCLK_FS register
     * and replace in the audio status channel byte 4.
     */
    audio_ctx->channel_sta[3] &= ~CHST4_SAMPLE_F;       /* 3: directing audio sample rate data */
    audio_ctx->fs = hal_audio_get_hw_sample_rate(port); /* HW measured fs */

    if (AUDIO_CHST4_FS_UNKNOWN != audio_ctx->measured_fs) {
        /* replace with FW measured values */
        audio_ctx->channel_sta[3] |= audio_ctx->measured_fs; /* 3: directing audio sample rate data */
    } else {
        /* replace with HW measured values */
        audio_ctx->channel_sta[3] |= audio_ctx->fs; /* 3: directing audio sample rate data */
    }

    /* note: DSD does not have audio status channel, so all bytes in the audio
        status channel registers are zeros.
        that should not cause problems because the DSD format is fixed. */
    audio_ctx->blayout1 = (hi_bool)hal_audio_get_layout(port);

    audio_ctx->status_received = HI_TRUE;
    hdmirxv2_audio_update(port);
}

static hi_void hdmirx_audio_acr_process(hi_drv_hdmirx_port port)
{
    hi_bool cts_error;

    hal_audio_clear_audio_int(port);
    hal_ctrl_clear_t4_error(port);

    osal_udelay(64); /* 64: delay 64 us */
    cts_error = hdmirx_audio_is_cts_error(port);
    hal_audio_clear_audio_int(port);
    if (cts_error) {
        hal_ctrl_set_pwd_reset(port, HDMIRX_PWD_AUD_SRST);
        /* schedule next check */
        hdmirx_audio_change_state(port, AUDIO_STATE_REQ, 30000); /* 30000: delay time, us */
    } else {
        /* no errors, prepare for the next step, the bit will be cleared automatically */
        hal_audio_set_acr_start(port);
        hdmirx_audio_refresh(port);
        hdmirx_audio_set_mclk(port);
        hdmirx_audio_change_state(port, AUDIO_STATE_READY, 30000); /* 30000: delay time, us */
    }
    hal_audio_clear_audio_packet(port);
}
static hi_bool hdmirx_audio_is_time_out(hi_drv_hdmirx_port port)
{
    static osal_timeval audio_time[HI_DRV_HDMIRX_PORT_MAX];
    osal_timeval cur_time;
    hdmirx_audio_context *audio_ctx;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (audio_ctx->time_out == 0) {
        return HI_TRUE;
    }
    osal_get_timeofday(&cur_time);
    if (audio_ctx->time_out <= hdmirx_time_diff_us(cur_time, audio_time[port])) {
        osal_get_timeofday(&audio_time[port]);
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_void hdmirxv2_audio_get_sample_rate(hi_drv_hdmirx_port port, hi_drv_ai_hdmirx_attr *audio_info)
{
    hdmirx_audio_context *audio_ctx = HI_NULL;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (audio_ctx == HI_NULL || audio_info == HI_NULL) {
        hi_err_hdmirx("input ptr is null!\n");
        return;
    }
    switch (audio_ctx->measured_fs) {
        case AUDIO_CHST4_FS_44:
            audio_info->sample_rate = HI_SAMPLE_RATE_44K;
            break;
        case AUDIO_CHST4_FS_48:
            audio_info->sample_rate = HI_SAMPLE_RATE_48K;
            break;
        case AUDIO_CHST4_FS_32:
            audio_info->sample_rate = HI_SAMPLE_RATE_32K;
            break;
        case AUDIO_CHST4_FS_22:
            audio_info->sample_rate = HI_SAMPLE_RATE_22K;
            break;
        case AUDIO_CHST4_FS_24:
            audio_info->sample_rate = HI_SAMPLE_RATE_24K;
            break;
        case AUDIO_CHST4_FS_88:
            audio_info->sample_rate = HI_SAMPLE_RATE_88K;
            break;
        case AUDIO_CHST4_FS_96:
            audio_info->sample_rate = HI_SAMPLE_RATE_96K;
            break;
        case AUDIO_CHST4_FS_176:
            audio_info->sample_rate = HI_SAMPLE_RATE_176K;
            break;
        case AUDIO_CHST4_FS_192:
            audio_info->sample_rate = HI_SAMPLE_RATE_192K;
            break;
        default:
            audio_info->sample_rate = HI_SAMPLE_RATE_UNKNOWN;
            break;
    }
}

hi_void hdmirxv2_audio_get_data_type(hi_drv_hdmirx_port port, hi_drv_ai_hdmirx_attr *audio_info)
{
    hdmirx_audio_context *audio_ctx = HI_NULL;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (audio_ctx == HI_NULL || audio_info == HI_NULL) {
        hi_err_hdmirx("input ptr is null!\n");
        return;
    }
    switch (audio_ctx->audio_cod_type) {
        case RX_AUDIO_CODE_AC3:
            audio_info->hdmi_audio_data_type = HI_DRV_AI_HDMI_DATA_DD;
            break;
        case RX_AUDIO_CODE_DOLBY:
            audio_info->hdmi_audio_data_type = HI_DRV_AI_HDMI_DATA_DDP;
            break;
        case RX_AUDIO_CODE_DTS:
            audio_info->hdmi_audio_data_type = HI_DRV_AI_HDMI_DATA_DTS;
            break;
        case RX_AUDIO_CODE_DTSHD:
            audio_info->hdmi_audio_data_type = HI_DRV_AI_HDMI_DATA_DTSHD;
            break;
        case RX_AUDIO_CODE_MAT:
            audio_info->hdmi_audio_data_type = HI_DRV_AI_HDMI_DATA_TRUEHD;
            break;
        default:
            audio_info->hdmi_audio_data_type = HI_DRV_AI_HDMI_DATA_MAX;
            break;
    }
}

hi_void hdmirxv2_audio_get_info(hi_drv_hdmirx_port port, hi_drv_ai_hdmirx_attr *audio_info)
{
    hdmirx_audio_context *audio_ctx = HI_NULL;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    audio_info->channel = HI_AUDIO_CH_STEREO;
    if (audio_ctx->hbr_mode == HI_TRUE) {
        audio_info->channel = HI_AUDIO_CH_8;
    } else {
        if (audio_ctx->blayout1 == HI_FALSE) {
            audio_info->channel = HI_AUDIO_CH_STEREO;
        } else {
            audio_info->channel = HI_AUDIO_CH_8;
        }
    }
    audio_info->bit_depth = HI_BIT_DEPTH_16;
    audio_info->hdmi_audio_data_format = HI_DRV_AI_HDMI_FORMAT_LPCM;
    if (audio_ctx->hbr_mode == HI_TRUE) {
        audio_info->hdmi_audio_data_format = HI_DRV_AI_HDMI_FORMAT_HBR;
    } else {
        if (audio_ctx->enc == HI_FALSE) {
            audio_info->hdmi_audio_data_format = HI_DRV_AI_HDMI_FORMAT_LPCM;
        } else {
            audio_info->hdmi_audio_data_format = HI_DRV_AI_HDMI_FORMAT_LBR;
        }
    }
    hdmirxv2_audio_get_sample_rate(port, audio_info);

    hdmirxv2_audio_get_data_type(port, audio_info);
    if (audio_info->hdmi_audio_data_format == HI_DRV_AI_HDMI_FORMAT_LPCM) {
        audio_info->hdmi_audio_data_type = HI_DRV_AI_HDMI_DATA_MAX;
    }
    return;
}
hi_void hdmirxv2_audio_get_audio_info(hi_drv_hdmirx_port port, hdmirx_audio_context *audio_ctx)
{
    hdmirx_audio_context *audio_ctx_get = HI_NULL;
    errno_t err_ret;

    audio_ctx_get = HDMIRX_AUDIO_GET_CTX(port);

    err_ret = memcpy_s(audio_ctx, sizeof(hdmirx_audio_context), audio_ctx_get, sizeof(hdmirx_audio_context));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
}

hi_void HDMIRXV2_AUDIO_STATE_READY(hi_drv_hdmirx_port port,
    hi_u32 *audio_stable_cnt, hdmirx_audio_context *audio_ctx)
{
    hi_bool fifo_stable;
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("enport is invalid!\n");
        return;
    }
    if (audio_ctx == HI_NULL) {
        hi_err_hdmirx("input ptr is NULL!\n");
        return;
    }

    fifo_stable = hdmirx_audio_is_fifo_stable(port);
    if (fifo_stable == HI_FALSE) {
        hdmirx_audio_change_state(port, AUDIO_STATE_REQ, 0);
        *audio_stable_cnt = 0;
    } else {
        (*audio_stable_cnt)++;
        if (*audio_stable_cnt >= AUDIO_CHECK_STABLE_THR) {
            audio_ctx->channel_sta[0] = 0x03;
            hdmirx_audio_on_off(port, HI_TRUE);
            audio_ctx->fifo_err_cnt = 0;
            *audio_stable_cnt = 0;
            hdmirxv2_audio_on_channel_status_chg(port);
            hi_dbg_hdmirx_print_u32(audio_ctx->enc);
            hdmirx_audio_show_audio_info(port);
            if (hal_audio_get_audio_fifo_diff(port) == 0) {
                hi_dbg_hdmirx("hdmi receive invalid sound packet, can not go to on\n");
            } else {
                hi_dbg_hdmirx("go to on\n");
                hal_audio_set_aac_unmute(port);
                hdmirx_audio_change_state(port, AUDIO_STATE_ON, 0);
            }
        }
    }
}

hi_void hdmirxv2_audio_state_on(hi_drv_hdmirx_port port, hi_u32 *audio_check)
{
    hi_bool b_channel_status_chg;
    hi_u32 audio_intr;
    b_channel_status_chg = hdmirx_audio_is_status_changed(port);
    audio_intr = hal_audio_get_intr(port);
    if (b_channel_status_chg == HI_TRUE) {
        hdmirxv2_audio_on_channel_status_chg(port);
    }
    if (((audio_intr & CHN_STATUS_READY) > 0) ||
        (audio_intr & AUDIO_FS_CHANGE) > 0) {
        hal_audio_set_intr(port, CHN_STATUS_READY | AUDIO_FS_CHANGE);
        hdmirxv2_audio_on_channel_status_chg(port);
    }
    if (hal_audio_get_hbra_intr(port) == HI_TRUE) {
        hal_audio_clear_hbra_intr(port);
        hi_dbg_hdmirx("get audio hbra!\n");
        hdmirxv2_audio_stream_type_changed(port);
        return;
    }

    if (hal_audio_is_fifo_stable(port) == HI_FALSE) {
        hi_dbg_hdmirx("get audio fifo unstable!\n");
#if 0 /* CUSTOME_PCM_SWITCH_DD_OPTIMIZE */
        hdmirxv2_hal_set_mute_en(port, HDMIRX_MUTE_AUD, HI_TRUE);
        hdmirx_audio_change_state(AUDIO_STATE_OFF, 0);
#endif
        hdmirxv2_audio_set_exceptions_en(port, HI_FALSE);

        hal_ctrl_set_pwd_reset(port, HDMIRX_PWD_AUD_FIFO_SRST);

        /* clear FIFO underrun and overrun interrupts_p */
        hal_audio_set_intr(port, AUDIO_FIFO_UNDERRUN | AUDIO_FIFO_OVERRUN);
        (*audio_check)++;
        if (*audio_check > 2) { /* unstable for 2 times, switch to off */
            *audio_check = 0;
            hdmirx_audio_change_state(port, AUDIO_STATE_OFF, 20000); /* 20000: dalay 20000 us */
        }
    } else {
        *audio_check = 0;
        hdmirx_audio_change_state(port, AUDIO_STATE_ON, 0);
#if 0 /* CUSTOME_PCM_SWITCH_DD_OPTIMIZE */
        hdmirxv2_hal_set_mute_en(port, HDMIRX_MUTE_AUD, HI_FALSE);
#endif
    }
}


hi_void HDMIRXV2_AUDIO_STATE_OFF(hi_drv_hdmirx_port port, hdmirx_audio_context *audio_ctx)
{
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        hi_err_hdmirx("enport is invalid!\n");
        return;
    }
    if (audio_ctx == HI_NULL) {
        hi_err_hdmirx("input ptr is NULL!\n");
        return;
    }
    audio_ctx->status_received = HI_FALSE;
    if (audio_ctx->exceptions_en == HI_TRUE) {
        hdmirxv2_audio_aac_done(port);
    }
    if (audio_ctx->start_req == HI_TRUE && g_audio_ready[port] == HI_TRUE) {
        hal_ctrl_set_pwd_reset(port, HDMIRX_PWD_AUD_SRST);
        hdmirx_audio_change_state(port, AUDIO_STATE_REQ, 0);
        hi_dbg_hdmirx("##### go to REQ\n");
        hdmirxv2_packet_reset_aud_info_frame_data(port);
        audio_ctx->measured_fs = AUDIO_CHST4_FS_UNKNOWN;
    } else {
        hdmirx_audio_change_state(port, AUDIO_STATE_OFF, 30000); /* 30000: delay 30000 us */
    }
}

hi_void hdmirxv2_audio_main_loop(hi_drv_hdmirx_port port)
{
    hi_bool time_out;
    hdmirx_audio_context *audio_ctx;
    static hi_u32 audio_stable_cnt[HI_DRV_HDMIRX_PORT_MAX] = {0};
    static hi_u32 audio_check[HI_DRV_HDMIRX_PORT_MAX] = {0};

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return;
    }
    if ((hal_audio_is_fifo_stable(port) == HI_FALSE) &&
        (audio_ctx->audio_state == AUDIO_STATE_ON)) {
        audio_ctx->fifo_err_cnt++;
    }
    time_out = hdmirx_audio_is_time_out(port);
    if (time_out == HI_FALSE) {
        return;
    }

    switch (audio_ctx->audio_state) {
        case AUDIO_STATE_OFF:
            HDMIRXV2_AUDIO_STATE_OFF(port, audio_ctx);
            break;
        case AUDIO_STATE_REQ:
            audio_ctx->status_received = HI_FALSE;
            hdmirx_audio_acr_process(port);
            break;
        case AUDIO_STATE_READY:
            HDMIRXV2_AUDIO_STATE_READY(port, &audio_stable_cnt[port], audio_ctx);
            break;
        case AUDIO_STATE_ON:
            hdmirxv2_audio_state_on(port, &audio_check[port]);
            break;
        default:
            break;
    }
}

hi_void hdmirxv2_audio_proc_read_state(hi_drv_hdmirx_port port, hi_void *s)
{
    hdmirx_audio_context *audio_ctx = HDMIRX_AUDIO_GET_CTX(port);
    if (audio_ctx == HI_NULL) {
        hi_err_hdmirx("input ptr is NULL!");
        return;
    }
    switch (audio_ctx->audio_state) {
        case AUDIO_STATE_OFF:
            osal_proc_print(s, "AudioState           :   OFF\n");
            break;
        case AUDIO_STATE_REQ:
            osal_proc_print(s, "AudioState           :   REQ\n");
            break;
        case AUDIO_STATE_READY:
            osal_proc_print(s, "AudioState           :   READY\n");
            break;
        case AUDIO_STATE_ON:
            osal_proc_print(s, "AudioState           :   ON\n");
            break;
        default:
            osal_proc_print(s, "AudioState           :   ERRO\n");
            break;
    }
    if (g_audio_ready[port]) {
        osal_proc_print(s, "AudioReady           :   yes\n");
    } else {
        osal_proc_print(s, "AudioReady           :   no\n");
    }
}
hi_void hdmirxv2_audio_proc_read_sample_rate(hi_void *s, hdmirx_audio_context *audio_ctx)
{
    if (audio_ctx == HI_NULL) {
        hi_err_hdmirx("input ptr is NULL!");
        return;
    }
    switch (audio_ctx->measured_fs) {
        case AUDIO_CHST4_FS_44:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "44kHz");
            break;
        case AUDIO_CHST4_FS_48:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "48kHz");
            break;
        case AUDIO_CHST4_FS_32:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "32kHz");
            break;
        case AUDIO_CHST4_FS_22:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "22kHz");
            break;
        case AUDIO_CHST4_FS_24:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "24kHz");
            break;
        case AUDIO_CHST4_FS_88:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "88kHz");
            break;
        case AUDIO_CHST4_FS_96:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "96kHz");
            break;
        case AUDIO_CHST4_FS_176:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "176kHz");
            break;
        case AUDIO_CHST4_FS_192:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "192kHz");
            break;
        case AUDIO_CHST4_FS_768:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "768kHz");
            break;
        default:
            osal_proc_print(s, "Audio sample rate    :   %s\n", "unknown");
            break;
    }
}

hi_void hdmirxv2_audio_proc_read(hi_drv_hdmirx_port port, hi_void *s)
{
    hdmirx_audio_context *audio_ctx;
    hi_u32 cts;
    hi_u32 n;
    hi_u32 tmds_clk;
    hi_u32 fs_100_hz = 0;
    hdmirx_audio_code_type audio_cod_type;

    audio_ctx = HDMIRX_AUDIO_GET_CTX(port);

    osal_proc_print(s, "\n---------------HDMIRX Audio---------------\n");
    hdmirxv2_audio_proc_read_state(port, s);
    osal_proc_print(s, "Fs                   :   %d\n", audio_ctx->fs);

    n = hal_audio_get_n(port);
    cts = hal_audio_get_cts(port);
    tmds_clk = hdmirxv2_audio_get_tmds_clk_10k(port);
    if (tmds_clk && cts) {
        fs_100_hz = (tmds_clk * n / cts) * 100 / 128; /* 100, 128: 10k units to 100, times 100 */
    }
    osal_proc_print(s, "fs_100Hz             :   %d\n", fs_100_hz);
    osal_proc_print(s, "HbrMode              :   %s\n", audio_ctx->hbr_mode ? "Yes" : "No");
    osal_proc_print(s, "StartReq             :   %s\n", audio_ctx->start_req ? "Yes" : "No");
    if (audio_ctx->hbr_mode == HI_TRUE) {
        osal_proc_print(s, "AudioDataFormat      :   HBR\n");
    } else {
        if (audio_ctx->enc == HI_TRUE) {
            osal_proc_print(s, "AudioDataFormat      :   LBR\n");
        } else {
            osal_proc_print(s, "AudioDataFormat      :   LPCM\n");
        }
    }
    osal_proc_print(s, "MeasuredFs           :   %d\n", audio_ctx->measured_fs);
    hdmirxv2_audio_proc_read_sample_rate(s, audio_ctx);
    audio_cod_type = hdmirxv2_audio_get_coding_type(port);
    osal_proc_print(s, "Audio Coding Type    :   %s\n", g_hdmirx_audio_coding_type_string[audio_cod_type]);

    osal_proc_print(s, "ChannelSta           :   0x%x,0x%x,0x%x,0x%x,0x%x\n",
                    audio_ctx->channel_sta[0],  /* 0: print 1st channel state */
                    audio_ctx->channel_sta[1],  /* 1: print 2st channel state */
                    audio_ctx->channel_sta[2],  /* 2: print 3st channel state */
                    audio_ctx->channel_sta[3],  /* 3: print 4st channel state */
                    audio_ctx->channel_sta[4]); /* 4: print 5st channel state */
}
