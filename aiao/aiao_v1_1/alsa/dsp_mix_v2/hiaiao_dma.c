/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: alsa hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"

#include <sound/pcm_params.h>

#include "drv_ao_func.h"
#include "alsa_aiao_proc_func.h"
#include "alsa_aiao_comm.h"
#include "hi_drv_ao.h"
#include "drv_ao_ext.h"

#ifdef HI_ALSA_AI_SUPPORT
#include "hi_drv_ai.h"
#include "drv_ai_ioctl.h"
#include "drv_ai_func.h"
#include "hi_drv_dev.h"
#include "drv_ai_private.h"
#endif
#ifndef HI_ALSA_HRTIMER_SUPPORT
#include "drv_timer_private.h"
#endif

#define DMA_BUFFER_SIZE (64 * 1024)
#define INITIAL_VALUE 0xffffffff

#ifndef HI_ALSA_HRTIMER_SUPPORT
#define I2SDMA_IRQ_NUM 0xffffffff
#endif

#ifdef MUTE_FRAME_OUTPUT
#define MUTE_FRAME_TIME 15
#define PORT_POLL_TIME 5
hi_s32 dump_buf[MUTE_FRAME_TIME * 192000 / 1000];
#endif

/* #define DEBUG_V */
/* #define DEBUG_VV */
#ifdef DEBUG_V
#define ATRC print
#define ATRP(fmt, ...) print(KERN_ALERT "\nfunc:%s line:%d \n", __func__, __LINE__)
#else
#define ATRC(fmt, ...)
#define ATRP(fmt, ...)
#endif
#ifdef DEBUG_VV
#define ATRCC print
#define ATRPP(fmt, ...) print(KERN_ALERT "\nfunc:%s line:%d \n", __func__, __LINE__)
#else
#define ATRCC(fmt, ...)
#define ATRPP(fmt, ...)
#endif

#define PROC_AO_NAME "ao"
#ifdef HI_ALSA_AI_SUPPORT
#define ALSA_PROC_AI_NAME "hi_ai_data"
#endif

static const struct snd_pcm_hardware ao_hardware = {
    .info = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED |
            SNDRV_PCM_INFO_BLOCK_TRANSFER | SNDRV_PCM_INFO_RESUME,
    .formats = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE,
    .channels_min = 1,
    .channels_max = 2,
    .period_bytes_min = 0x200,
    .period_bytes_max = 0x2000,
    .periods_min = 2,
    .periods_max = 16,
    .buffer_bytes_max = DMA_BUFFER_SIZE,
};

static const struct snd_pcm_hardware ai_hardware = {
    .info = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED |
            SNDRV_PCM_INFO_BLOCK_TRANSFER | SNDRV_PCM_INFO_RESUME,
    .formats = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE,
    .channels_min = 2,
    .channels_max = 2,
    .period_bytes_min = 0x800,
    .period_bytes_max = 0x800,
    .periods_min = 2,
    .periods_max = 8,
    .buffer_bytes_max = 0xf000,
};

#ifdef HI_ALSA_AI_SUPPORT
#ifdef MUTE_FRAME_INPUT
static void ai_hrtimer_pcm_elapsed(unsigned long priv)
{
    struct hiaudio_data *had = (struct hiaudio_data *)priv;
    if (osal_atomic_read(&had->atm_record_state)) {
        snd_pcm_period_elapsed(had->aisubstream);
    }
}

static enum hrtimer_restart snd_ai_peroid_callback(struct hrtimer *hrt)
{
    struct hiaudio_data *had = osal_container_of(hrt, struct hiaudio_data, aihrt);

    ATRC("cur_ns: %lldms \n", ktime_to_ms(hrtimer_cb_get_time(&had->aihrt)));
    if (!osal_atomic_read(&had->atm_record_state)) {
        ATRC("cur_ns: %lldms , timer disable !\n", ktime_to_ms(hrtimer_cb_get_time(&had->aihrt)));
        return HRTIMER_NORESTART;
    }
    ATRP();

    had->isr_total_cnt_c++;
    had->current_c_pos = had->isr_total_cnt_c % had->sthwparam_ai.periods;

    had->ai_writepos = had->current_c_pos * had->sthwparam_ai.period_bytes;

    tasklet_schedule(&had->aitasklet);
    ATRP();
    hrtimer_forward_now(hrt, ns_to_ktime(had->ai_poll_time_ns));
    return HRTIMER_RESTART;
}

static hi_s32 hr_ai_timer_creat(struct hiaudio_data *had)
{
    ATRP();
    hrtimer_init(&had->aihrt, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    had->aihrt.function = snd_ai_peroid_callback;
    return HI_SUCCESS;
}
static hi_s32 hr_ai_timer_start(struct hiaudio_data *had)
{
    ATRP();
    hrtimer_start(&had->aihrt, had->aihrtperoid, HRTIMER_MODE_REL);
    ATRC("\n_start cur_ns: %lld  \n", ktime_to_ms(hrtimer_cb_get_time(&had->aihrt)));
    return HI_SUCCESS;
}
static hi_s32 hr_ai_timer_destory(struct hiaudio_data *had)
{
    ATRP();
    hrtimer_cancel(&had->aihrt);
    ATRP();
    return HI_SUCCESS;
}

#else

static ai_alsa_param ai_alsa_attr;
static irqreturn_t isr_ai_func(aiao_port_id port_id, hi_u32 int_raw_status, hi_void *dev_id)
{
    struct snd_pcm_substream *substream = dev_id;
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);

    unsigned int readpos = had->sthwparam_ai.period_size * had->sthwparam_ai.frame_size;

    had->isr_total_cnt_c++;
    hi_ai_alsa_query_writepos(had->ai_handle, &(had->ai_writepos));
    hi_ai_alsa_query_readpos(had->ai_handle, &(had->ai_readpos));

    if (had->ai_readpos > had->ai_writepos) {
        readpos = had->sthwparam_ai.buffer_size - had->ai_readpos + had->ai_writepos;
    } else {
        readpos = had->ai_writepos - had->ai_readpos;
    }

    hi_ai_alsa_update_readptr(had->ai_handle, &(readpos));

#ifdef AIAO_ALSA_DEBUG
    if (had->isr_total_cnt_c <= 8) {
        ATRC(KERN_ALERT " get write pos is %d,and get readpos is %d,update read ptr is %d\n ",
            had->ai_writepos, had->ai_readpos, readpos);
    }
#endif
    snd_pcm_period_elapsed(substream);

    if (had->isr_proc != HI_NULL) {
        had->isr_proc(port_id, int_raw_status, NULL);
    }

    return IRQ_HANDLED;
}
#endif
#endif

#ifdef HI_ALSA_HRTIMER_SUPPORT
static enum hrtimer_restart snd_peroid_callback(struct hrtimer *hrt)
{
    struct hiaudio_data *had = osal_container_of(hrt, struct hiaudio_data, hrt);
    struct snd_pcm_substream *substream = had->substream;
    ATRPP();

    if (!osal_atomic_read(&had->atm_track_state)) {
        ATRC("\n_cur_ns: %lldms , timer disable !\n", ktime_to_ms(hrtimer_cb_get_time(&had->hrt)));
        return HRTIMER_NORESTART;
    }

    snd_pcm_period_elapsed(substream);

    /* case 1 get real read pos to calc offset */
    /* choose --->  case 2 every timer cnt mean isr */
    had->send_try_cnt++;
#ifdef DUMMY_OUTPUT
    had->had_sent++;
    had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
#else
    if (had->trigger_start_ok == HI_FALSE) {
        had->had_sent++;
        had->discard_sent++;
        had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
    } else {
        hi_s32 ret = HI_SUCCESS;
        had->ao_frame.pcm_buffer =
            ((hi_u8 *)had->vir_base_addr - (hi_u8 *)HI_NULL) + had->hw_pointer * had->ao_hw_param.period_bytes;
        ret = hi_drv_ao_track_atomic_send_data(had->track_id, &had->ao_frame);
        if (ret == HI_SUCCESS) {
            had->had_sent++;
            had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
            ATRCC("new hw_pointer= %d", had->hw_pointer);
        } else {
            had->send_fail_cnt++;
            ATRC("ALSA timer callback hi_drv_ao_track_atomic_send_data failed 0x%x\n", ret);
        }
    }
#endif
    hrtimer_forward_now(hrt, ns_to_ktime(had->poll_time_ns));

    return HRTIMER_RESTART;
}

static hi_s32 hr_timer_start(struct hiaudio_data *had)
{
    ATRP();
    hrtimer_start(&had->hrt, had->hrtperoid, HRTIMER_MODE_REL);
    ATRC("start cur_ns: %lld \n", ktime_to_ms(hrtimer_cb_get_time(&had->hrt)));
    return HI_SUCCESS;
}

static hi_s32 hr_timer_creat(struct hiaudio_data *had)
{
    ATRP();
    hrtimer_init(&had->hrt, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    had->hrt.function = snd_peroid_callback;
    return HI_SUCCESS;
}

static hi_s32 hr_timer_destory(struct hiaudio_data *had)
{
    ATRP();
    hrtimer_cancel(&had->hrt);
    return HI_SUCCESS;
}
#else
static irqreturn_t dma_isr(aiao_timer_id timer_id, hi_void *dev_id)
{
    struct snd_pcm_substream *substream = dev_id;
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);
    ATRPP();

    if (unlikely(substream == HI_NULL)) {
        pr_err("%s: null substream\n", __func__);
        return IRQ_HANDLED;
    }
    if (!osal_atomic_read(&had->atm_track_state)) {
        ATRC("trackstate disable!\n");
        return IRQ_HANDLED;
    }
    snd_pcm_period_elapsed(substream);

    had->send_try_cnt++;
#ifdef DUMMY_OUTPUT
    had->had_sent++;
    had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
    /* ATRC("\n had->hw_pointer=%d, had->had_sent=%d  \n", had->hw_pointer, had->had_sent); */
#else
    if (had->trigger_start_ok == HI_FALSE) {
        had->had_sent++;
        had->discard_sent++;
        had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
    } else {
        hi_s32 ret = HI_SUCCESS;
        had->ao_frame.t_pcm_buffer = had->vir_base_addr + had->hw_pointer * had->ao_hw_param.period_bytes;
        ret = hi_drv_ao_track_atomic_send_data(had->track_id, &had->ao_frame);
        if (ret == HI_SUCCESS) {
            had->had_sent++;
            had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
            ATRCC("new hw_pointer= %d", had->hw_pointer);
        } else {
            ATRC("ALSA isr hi_drv_ao_track_atomic_send_data failed 0x%x\n", ret);
        }
    }
#endif
    return IRQ_HANDLED;
}
#endif

static hi_void cmd_io_task(struct work_struct *work)
{
    struct hiaudio_data *had = osal_container_of(work, struct hiaudio_data, work);
    hi_s32 ret = HI_SUCCESS;
    ATRP();

    if (had->track_id == INITIAL_VALUE) {
        ATRP();
        return;
    }
    if (had->cmd == CMD_START) {
        ATRC("cmd_io_task start track\n");
        had->trigger_start_ok = HI_TRUE;
        ret = hi_drv_ao_track_start(had->track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA trigger hi_drv_ao_track_start failed 0x%x\n", ret);
            had->trigger_start_ok = HI_FALSE;
#ifdef HI_ALSA_HRTIMER_SUPPORT
            hr_timer_start(had);
#else
            hi_drv_timer_set_enable(had->h_timer_handle, HI_TRUE);
#endif
            return;
        }

        ret = hi_drv_ao_track_flush(had->track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA trigger hi_drv_ao_track_flush failed 0x%x\n", ret);
        }

#ifdef MUTE_FRAME_OUTPUT
        ret = hi_drv_ao_track_get_delay(had->track_id, &had->port_delay_ms);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA trigger hi_drv_ao_track_get_delayms failed 0x%x\n", ret);
            had->port_delay_ms = 0;
        }
        if (had->port_delay_ms <= MUTE_FRAME_TIME) {
            if (had->port_delay_ms <= PORT_POLL_TIME) { /* means empty */
                had->mute_frame_time = MUTE_FRAME_TIME;
            } else {
                had->mute_frame_time = MUTE_FRAME_TIME - had->port_delay_ms + PORT_POLL_TIME;
            }
        } else if (had->port_delay_ms > MUTE_FRAME_TIME + PORT_POLL_TIME) {
            had->mute_frame_time = 0;
        } else {
            had->mute_frame_time = MUTE_FRAME_TIME + PORT_POLL_TIME - had->port_delay_ms;
        }

        had->ao_frame.pcm_samples = had->mute_frame_time * had->ao_hw_param.rate / 1000;

        had->ao_frame.pcm_buffer = (hi_u8 *)dump_buf - (hi_u8 *)HI_NULL;

        if (had->ao_frame.pcm_samples != 0) {
            ret = hi_drv_ao_track_send_data(had->track_id, &had->ao_frame);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("ALSA trigger MUTE_FRAME_OUTPUT failed 0x%x\n", ret);
            }
        }

        had->ao_frame.pcm_samples = had->ao_hw_param.period_size;
#endif

        had->send_try_cnt++;
#ifdef DUMMY_OUTPUT
        had->had_sent++;
        had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
#else
        had->ao_frame.pcm_buffer = (hi_u8 *)had->vir_base_addr - (hi_u8 *)HI_NULL;
        ret = hi_drv_ao_track_send_data(had->track_id, &had->ao_frame);
        if (ret == HI_SUCCESS) {
            had->had_sent++;
            had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
            osal_atomic_set(&had->atm_track_state, 1);
#ifdef HI_ALSA_HRTIMER_SUPPORT
            hr_timer_start(had);
#else
            hi_drv_timer_set_enable(had->h_timer_handle, HI_TRUE);
#endif
        } else if (ret == HI_ERR_AO_OUT_BUF_FULL) {
            had->had_sent++;
            had->hw_pointer = had->had_sent % had->ao_hw_param.periods;
            osal_atomic_set(&had->atm_track_state, 1);
#ifdef HI_ALSA_HRTIMER_SUPPORT
            hr_timer_start(had);
#else
            hi_drv_timer_set_enable(had->h_timer_handle, HI_TRUE);
#endif
            HI_ERR_AO("ALSA trigger hi_drv_ao_track_send_data full %d\n", ret);
        } else {
            HI_ERR_AO("ALSA trigger hi_drv_ao_track_send_data failed 0x%x\n", ret);
        }
#endif
    } else if (had->cmd == CMD_STOP) {
        ATRC("cmd_io_task stop track\n");
#ifdef HI_ALSA_HRTIMER_SUPPORT
        hr_timer_destory(had);
#endif
        ret = hi_drv_ao_track_stop(had->track_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA trigger hi_drv_ao_track_stop failed 0x%x\n", ret);
        }
    } else {
        HI_ERR_AO("ALSA trigger in_vaild cmd: 0x%x\n", had->cmd);
    }
}

static int hi_dma_prepare(struct snd_pcm_substream *substream)
{
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);
    hi_s32 ret = HI_SUCCESS;
    ATRP();

#ifdef HI_ALSA_AI_SUPPORT
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
        had->last_c_pos = 0;
        had->current_c_pos = 0;
        had->ai_writepos = 0;
        had->ai_readpos = 0;
        had->isr_total_cnt_c = 0;
        had->ack_c_cnt = 0;
#ifdef MUTE_FRAME_INPUT
        osal_atomic_set(&had->atm_record_state, 0);
        tasklet_kill(&had->aitasklet);
#else
        return hi_ai_alsa_flush_buffer(had->ai_handle);
#endif
    } else
#endif
    {
        had->send_try_cnt = 0;
        had->had_sent = 0;
        had->discard_sent = 0;
        had->trigger_start_ok = HI_TRUE;
        had->hw_pointer = 0;
        had->port_delay_ms = 0;
        osal_atomic_set(&had->atm_track_state, 0);

#ifdef CONFIG_PM
        had->suspend_state = 0;
#endif

#ifdef MUTE_FRAME_OUTPUT
        had->mute_frame_time = 0;
#else
        had->mute_frame_time = 0xffffffff;
#endif
    }
    return ret;
}

static int hi_dma_trigger(struct snd_pcm_substream *substream, int cmd)
{
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);
    hi_s32 ret = HI_SUCCESS;
    ATRP();

    switch (cmd) {
        case SNDRV_PCM_TRIGGER_RESUME:
        case SNDRV_PCM_TRIGGER_START:
        case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
            if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
                ATRP();
                had->cmd = CMD_START;
                queue_work(had->workq, &had->work);
            }
#ifdef HI_ALSA_AI_SUPPORT
            if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
#ifdef MUTE_FRAME_INPUT
                osal_atomic_set(&had->atm_record_state, 1);
                hr_ai_timer_start(had);
#else
                ret = hi_ai_alsa_set_enable(had->ai_handle, HI_TRUE);
                if (ret) {
                    ATRC("AI ALSA start dma fail \n");
                }
#endif
            }
#endif
            break;
        case SNDRV_PCM_TRIGGER_SUSPEND:
        case SNDRV_PCM_TRIGGER_STOP:
        case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
            if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
                ATRP();
                osal_atomic_set(&had->atm_track_state, 0);
#ifndef HI_ALSA_HRTIMER_SUPPORT
                hi_drv_timer_set_enable(had->h_timer_handle, HI_FALSE);
#endif
                had->cmd = CMD_STOP;
                queue_work(had->workq, &had->work);
            }
#ifdef HI_ALSA_AI_SUPPORT
            if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
#ifdef MUTE_FRAME_INPUT
                osal_atomic_set(&had->atm_record_state, 0);
                hr_ai_timer_destory(had);
#else
                ret = hi_ai_alsa_set_enable(had->ai_handle, HI_FALSE);
                if (ret) {
                    ATRC("AI ALSA stop dma fail \n");
                }
#endif
            }
#endif
            break;

        default:
            ret = -EINVAL;
            break;
    }
    return ret;
}

static int hi_dma_mmap(struct snd_pcm_substream *substream, struct vm_area_struct *vma)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    int ret;
    unsigned int size;
    ATRP();

    vma->vm_flags |= VM_IO;
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    size = vma->vm_end - vma->vm_start;

    /* just for kernel ddr linear area */
    ret = io_remap_pfn_range(vma, vma->vm_start, runtime->dma_addr >> PAGE_SHIFT, size, vma->vm_page_prot);
    if (ret) {
        return -EAGAIN;
    }

    return 0;
}

static snd_pcm_uframes_t hi_dma_pointer(struct snd_pcm_substream *substream)
{
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);
    snd_pcm_uframes_t frame_offset;
    unsigned int bytes_offset = 0;

#ifdef HI_ALSA_AI_SUPPORT
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
        bytes_offset = had->ai_writepos;
        if (bytes_offset >= snd_pcm_lib_buffer_bytes(substream)) {
            bytes_offset = 0;
        }
    } else
#endif
    {
        bytes_offset = had->hw_pointer * had->ao_hw_param.period_bytes;
    }
    frame_offset = bytes_to_frames(runtime, bytes_offset);

    return frame_offset;
}

#ifdef HI_ALSA_AI_SUPPORT
#ifndef MUTE_FRAME_INPUT
static hi_void hi_ai_set_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params,
    struct snd_pcm_runtime *runtime)
{
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
        ai_alsa_attr.isr_func = (aiao_isr_func *)isr_ai_func;
        ai_alsa_attr.substream = (hi_void *)substream;
        ai_alsa_attr.buf.buf_phy_addr = runtime->dma_addr; /* for dma buffer */
        ai_alsa_attr.buf.buf_vir_addr = (hi_u8 *)runtime->dma_area - (hi_u8 *)HI_NULL;
        ai_alsa_attr.buf.buf_size = runtime->dma_bytes;
        ai_alsa_attr.buf.period_byte_size = params_buffer_bytes(params) / params_periods(params);
        ai_alsa_attr.buf.periods = params_periods(params);
    }
}
#endif
#endif

static int hi_dma_hwparams(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
    unsigned int buffer_bytes;
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);
    hi_s32 ret = HI_SUCCESS;

#ifdef HI_ALSA_AI_SUPPORT
#ifndef MUTE_FRAME_INPUT
    hi_ai_attr ai_attr;
    ai_drv_create_param ai_param;
#endif
#endif

    ATRP();

    buffer_bytes = params_buffer_bytes(params);
    if (snd_pcm_lib_malloc_pages(substream, buffer_bytes) < 0) {
        return -ENOMEM;
    }

#ifdef HI_ALSA_AI_SUPPORT
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
#ifndef MUTE_FRAME_INPUT
        memset(&ai_attr, 0, sizeof(hi_ai_attr));
        memset(&ai_param, 0, sizeof(ai_drv_create_param));

        ret = hi_ai_alsa_get_attr(HI_AI_I2S0, &ai_attr);
        if (ret != HI_SUCCESS) {
            ret = -EINVAL;
            goto err_allocate_dma;
        }
        ai_attr.sample_rate = params_rate(params);
        ai_attr.un_attr.i2s_attr.attr.channel = params_channels(params);
        switch (params_format(params)) {
            case SNDRV_PCM_FORMAT_S16_LE:
                ai_attr.un_attr.i2s_attr.attr.bit_depth = HI_I2S_BIT_DEPTH_16;
                break;
            case SNDRV_PCM_FMTBIT_S24_LE:
                ai_attr.un_attr.i2s_attr.attr.bit_depth = HI_I2S_BIT_DEPTH_24;
                break;
            default:
                break;
        }
        ATRC("rate : %d\n", ai_attr.sample_rate);
        ATRC("channel : %d\n", ai_attr.un_attr.i2s_attr.attr.channel);
        ATRC("bitdepth : %d\n", ai_attr.un_attr.i2s_attr.attr.bit_depth);
        ret = hi_ai_alsa_get_proc_func(&had->isr_proc);
        if (ret != HI_SUCCESS) {
            ret = -EINVAL;
            goto err_allocate_dma;
        }

        hi_ai_set_params(substream, params, runtime);

        memcpy(&ai_param.attr, &ai_attr, sizeof(hi_ai_attr));
        ai_param.ai_port = HI_AI_I2S0;
        ai_param.alsa = HI_TRUE;
        ai_param.alsa_para = (hi_void *)(&ai_alsa_attr);

        ret = hi_ai_alsa_open(&ai_param, &had->cfile);
        if (ret != HI_SUCCESS) {
            ret = -EINVAL;
            goto err_allocate_dma;
        }
        had->ai_handle = ai_param.ai;
        ATRC("\nbuffer_bytes : 0x%x \n", buffer_bytes);
        ATRC("\n ai_attr.sample_rate : 0x%d \n", (int)ai_attr.sample_rate);
        ATRC("\n ai_attr.pcm_frame_max_num : 0x%d \n", ai_attr.pcm_frame_max_num);
        ATRC("\n ai_attr.pcm_samples_per_frame : 0x%d \n", ai_attr.pcm_samples_per_frame);
        ATRC("\n ai_attr.alsa_use : 0x%d \n", ai_param.alsa);
        ATRC("\nruntime->dma_addr : %d \n", runtime->dma_addr);
        ATRC("\(int)runtime->dma_area : %d \n", (int)runtime->dma_area);
        ATRC("\nruntime->dma_bytes : %d \n", runtime->dma_bytes);
        ATRC("\nhad->ai_handle is %d\n", had->ai_handle);
#endif
        had->sthwparam_ai.channels = params_channels(params);
        had->sthwparam_ai.rate = params_rate(params);
        had->sthwparam_ai.format = params_format(params);
        had->sthwparam_ai.periods = params_periods(params);
        had->sthwparam_ai.period_size = params_period_size(params);
        had->sthwparam_ai.buffer_size = params_buffer_size(params);
        had->sthwparam_ai.buffer_bytes = params_buffer_bytes(params);
        had->sthwparam_ai.period_bytes = params_period_bytes(params);

        switch (had->sthwparam_ai.format) {
            case SNDRV_PCM_FORMAT_S16_LE:
                had->sthwparam_ai.frame_size = 2 * had->sthwparam_ai.channels;
                break;
            case SNDRV_PCM_FMTBIT_S24_LE:
                had->sthwparam_ai.frame_size = 3 * had->sthwparam_ai.channels;
                break;
            default:
                break;
        }

        ATRC("\nhad->sthwparam_ai.channels : 0x%x", had->sthwparam_ai.channels);
        ATRC("\nhad->sthwparam_ai.rate : 0x%x", had->sthwparam_ai.rate);
        ATRC("\nhad->sthwparam_ai.periods : 0x%x", had->sthwparam_ai.periods);
        ATRC("\nhad->sthwparam_ai.period_size : 0x%x", (int)had->sthwparam_ai.period_size);
        ATRC("\nhad->sthwparam_ai.buffer_size : 0x%x", (int)had->sthwparam_ai.buffer_size);
        ATRC("\nhad->sthwparam_ai.frame_size : 0x%x", had->sthwparam_ai.frame_size);
#ifdef MUTE_FRAME_INPUT
        had->ai_poll_time_ns = 1000000000 / params_rate(params) * params_period_size(params);
        had->aihrtperoid = ns_to_ktime(had->ai_poll_time_ns);
        ATRC("\nhad->ai_poll_time_ns %d", had->ai_poll_time_ns);
#endif
    } else
#endif
    {
        had->ao_hw_param.channels = params_channels(params);
        had->ao_hw_param.rate = params_rate(params);
        had->ao_hw_param.format = params_format(params);
        had->ao_hw_param.periods = params_periods(params);
        had->ao_hw_param.period_size = params_period_size(params);
        had->ao_hw_param.period_bytes = params_period_bytes(params);

        switch (had->ao_hw_param.format) {
            case SNDRV_PCM_FORMAT_S16_LE:
                had->ao_hw_param.frame_size = 2 * had->ao_hw_param.channels;
                had->ao_frame.bit_depth = 16;
                break;
            case SNDRV_PCM_FMTBIT_S24_LE:
                had->ao_hw_param.frame_size = 3 * had->ao_hw_param.channels;
                had->ao_frame.bit_depth = 24;
                break;
            default:
                break;
        }

        had->poll_time_ns = 1000000000 / params_rate(params) * params_period_size(params);
#ifdef HI_ALSA_HRTIMER_SUPPORT
        had->hrtperoid = ns_to_ktime(had->poll_time_ns);
#else
        had->isr_config = params_period_size(params); /* rate not 48K */
        ATRC("\n%d -> config %d rate: 0x%x\n", (int)had->h_timer_handle, had->isr_config, params_rate(params));
        hi_drv_timer_set_attr(had->h_timer_handle, had->isr_config, params_rate(params));
#endif
        ATRC("\nhad->sthwparam.channels : 0x%x", had->ao_hw_param.channels);
        ATRC("\nhad->sthwparam.rate : 0x%x", had->ao_hw_param.rate);
        ATRC("\nhad->sthwparam.periods : 0x%x", had->ao_hw_param.periods);
        ATRC("\nhad->sthwparam.buffer_size : 0x%x", (int)had->ao_hw_param.buffer_size);
        ATRC("\nhad->sthwparam.frame_size : 0x%x", had->ao_hw_param.frame_size);
        ATRC("\nhad->sthwparam.period_bytes : 0x%x", had->ao_hw_param.period_bytes);
        ATRC("\nhad->sthwparam.period_size : 0x%x", (int)had->ao_hw_param.period_size);
        had->ao_frame.sample_rate = had->ao_hw_param.rate;
        had->ao_frame.channels = had->ao_hw_param.channels;
        had->ao_frame.pcm_buffer = (hi_u8 *)runtime->dma_area - (hi_u8 *)HI_NULL;
        had->ao_frame.pcm_samples = had->ao_hw_param.period_size;
        had->phy_base_addr = (hi_void *)((hi_u8 *)HI_NULL + runtime->dma_addr);
        had->vir_base_addr = (hi_void *)runtime->dma_area;
    }

    return ret;

#ifdef HI_ALSA_AI_SUPPORT
#ifndef MUTE_FRAME_INPUT
err_allocate_dma:
    ret = snd_pcm_lib_free_pages(substream);
#endif
#endif

    return HI_FAILURE;
}

static int hi_dma_hwfree(struct snd_pcm_substream *substream)
{
    ATRP();
    snd_pcm_lib_free_pages(substream);

    return HI_SUCCESS;
}
static int hi_dma_open(struct snd_pcm_substream *substream)
{
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);
    hi_s32 ret = HI_SUCCESS;

    ATRP();
    if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
        ret = snd_soc_set_runtime_hwparams(substream, &ao_hardware);
    } else {
        ret = snd_soc_set_runtime_hwparams(substream, &ai_hardware);
    }
    if (ret) {
        return ret;
    }

    /* interrupt by step */
    ret = snd_pcm_hw_constraint_integer(substream->runtime, SNDRV_PCM_HW_PARAM_PERIODS);
    if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
        hi_ao_attr snd_attr;
        hi_ao_track_attr track_attr;
        hi_handle h_track = HI_INVALID_HANDLE;

        memset(&had->ao_hw_param, 0, sizeof(struct aiao_hwparams));
        had->vir_base_addr = HI_NULL;
        had->phy_base_addr = HI_NULL;

        had->snd_open = 0;
        osal_atomic_set(&had->atm_track_state, 0);

        had->send_try_cnt = 0;
        had->had_sent = 0;
        had->discard_sent = 0;
        had->hw_pointer = 0;
        had->poll_time_ns = 0;
        had->cmd = 0;
        had->mute_frame_time = 0;
        had->port_delay_ms = 0;
        had->trigger_start_ok = HI_TRUE;
#ifdef CONFIG_PM
        had->suspend_state = 0;
#endif
        had->track_id = INITIAL_VALUE;
        had->h_timer_handle = INITIAL_VALUE;
#ifdef HI_ALSA_HRTIMER_SUPPORT
        had->substream = substream;
#endif
        ret = hi_drv_ao_snd_init(&had->file);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA hi_drv_ao_snd_init failed 0x%x\n", ret);
            return ret;
        }
        ret = hi_drv_ao_snd_get_default_open_attr(AO_SND_0, &snd_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA hi_drv_ao_snd_get_default_open_attr failed 0x%x\n", ret);
            goto ERR_SND_INIT_EXT;
        }
        ret = hi_drv_ao_snd_open(AO_SND_0, &snd_attr, &had->file);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA hi_drv_ao_snd_open failed 0x%x\n", ret);
            goto ERR_SND_INIT_EXT;
        }
        had->snd_open++;

        ret = hi_drv_ao_track_get_default_open_attr(HI_AO_TRACK_TYPE_SLAVE, &track_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA hi_drv_ao_track_get_default_open_attr failed 0x%x\n", ret);
            goto ERR_SND_OPEN_EXT;
        }
        ret = hi_drv_ao_track_create(AO_SND_0, &track_attr, HI_TRUE, &had->file, &h_track);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ALSA hi_drv_ao_track_create failed 0x%x\n", ret);
            ret = -ENOMEM;
            goto ERR_SND_OPEN_EXT;
        }

        had->track_id = h_track;

        memset(&had->ao_frame, 0, sizeof(ao_frame));
        had->ao_frame.bit_depth = 16;
        had->ao_frame.interleaved = HI_TRUE;
        had->ao_frame.sample_rate = 48000;
        had->ao_frame.channels = 2;

        had->workq = create_singlethread_workqueue("hisi-alsa-ao");
        if (had->workq == NULL) {
            HI_ERR_AO("workqueue create failed!");
            goto ERR_TRACK_CREAT_EXT;
        }
        INIT_WORK(&had->work, cmd_io_task);
#if 0
        tasklet_init(&had->tasklet2, pcm_elapsed,
                     (unsigned long)had);
#endif

#ifdef HI_ALSA_HRTIMER_SUPPORT
        osal_atomic_set(&had->atm_track_state, 0);
        hr_timer_creat(had);
#else
        if (had->h_timer_handle == INITIAL_VALUE) {
            ATRP();

            ret = hi_drv_timer_create(&had->h_timer_handle, (aiao_timer_isr_func *)dma_isr, substream);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("ALSA hi_drv_timer_create failed 0x%x\n", ret);
                goto ERR_WORKQUUE_CREAT_EXT;
            }
            HI_INFO_AO("hi_drv_timer_create h_timer_handle  0x%x\n", had->h_timer_handle);
        }
#endif
    } else {
#ifdef HI_ALSA_AI_SUPPORT
#ifdef MUTE_FRAME_INPUT
        had->ai_writepos = 0;
        had->isr_total_cnt_c = 0;
        had->aisubstream = substream;
        osal_atomic_set(&had->atm_record_state, 0);
        hr_ai_timer_creat(had);
        tasklet_init(&had->aitasklet, ai_hrtimer_pcm_elapsed, (uintptr_t)had);
#endif
#endif
    }
    return HI_SUCCESS;

#ifndef HI_ALSA_HRTIMER_SUPPORT
ERR_WORKQUUE_CREAT_EXT:
    destroy_workqueue(had->workq);
    had->workq = HI_NULL;
#endif

ERR_TRACK_CREAT_EXT:
    hi_drv_ao_track_destroy(had->track_id);
ERR_SND_OPEN_EXT:
    hi_drv_ao_snd_close(AO_SND_0, &had->file);
ERR_SND_INIT_EXT:
    hi_drv_ao_snd_deinit(&had->file);

    return ret;
}

static int hi_dma_close(struct snd_pcm_substream *substream)
{
    struct snd_soc_pcm_runtime *soc_rtd = substream->private_data;
    struct snd_soc_platform *platform = soc_rtd->platform;
    struct hiaudio_data *had = snd_soc_platform_get_drvdata(platform);
    hi_s32 ret = HI_SUCCESS;
    ATRP();
#ifdef HI_ALSA_AI_SUPPORT
    if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
#ifdef MUTE_FRAME_INPUT
        tasklet_kill(&had->aitasklet);
#else
        if (had->ai_handle != INITIAL_VALUE) {
            ret = hi_ai_alsa_close(had->ai_handle, &had->cfile);
            if (ret != HI_SUCCESS) {
                ATRC("hi_ai_alsa_close failed(ai_handle = %d)!\n", had->ai_handle);
            } else {
                had->ai_handle = INITIAL_VALUE;
            }
        }
#endif
        had->isr_total_cnt_c = 0;
    } else
#endif
    {
        if (had->workq != HI_NULL) {
            destroy_workqueue(had->workq);
            had->workq = HI_NULL;
        }

#ifndef HI_ALSA_HRTIMER_SUPPORT
        if (had->h_timer_handle != INITIAL_VALUE) {
            ATRP();
            ret = hi_drv_timer_destroy(had->h_timer_handle);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("ALSA hi_drv_timer_destroy failed 0x%x\n", ret);
            }
            had->h_timer_handle = INITIAL_VALUE;
        }
#endif

        if (had->track_id != INITIAL_VALUE) {
            ret = hi_drv_ao_track_destroy(had->track_id);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("ALSA hi_drv_ao_track_destroy failed 0x%x\n", ret);
            } else {
                had->track_id = INITIAL_VALUE;
            }
        }
        if (had->snd_open) {
            ret = hi_drv_ao_snd_close(AO_SND_0, &had->file);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("ALSA hi_drv_ao_snd_close failed 0x%x\n", ret);
            } else {
                had->snd_open--;
            }
        }
        hi_drv_ao_snd_deinit(&had->file);
    }

    snd_pcm_lib_free_pages(substream);

    return HI_SUCCESS;
}

static int hi_dma_ack(struct snd_pcm_substream *substream)
{
    ATRPP();
    return HI_SUCCESS;
}

static struct snd_pcm_ops hi_dma_ops = {
    .open = hi_dma_open,
    .close = hi_dma_close,
    .ioctl = snd_pcm_lib_ioctl,
    .hw_params = hi_dma_hwparams,
    .hw_free = hi_dma_hwfree,
    .prepare = hi_dma_prepare,
    .trigger = hi_dma_trigger,
    .pointer = hi_dma_pointer,
    .mmap = hi_dma_mmap,
    .ack = hi_dma_ack,
};

static int hi_dma_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
    struct snd_pcm *pcm = rtd->pcm;
    int ret = HI_SUCCESS;

    if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream || pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream) {
#ifdef __aarch64__
#if (KERNEL_VERSION(4, 4, 0) <= LINUX_VERSION_CODE)
        ret = snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_CONTINUOUS,
            snd_dma_continuous_data(OSAL_GFP_KERNEL), DMA_BUFFER_SIZE, DMA_BUFFER_SIZE);
#else
        struct snd_card *card = rtd->card->snd_card;
        ret =
            snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV, card->dev, DMA_BUFFER_SIZE, DMA_BUFFER_SIZE);
#endif
#else
        ret =
            snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV, NULL, DMA_BUFFER_SIZE, DMA_BUFFER_SIZE / 2);
#endif
        if (ret) {
            ATRC("dma buffer allocation fail \n");
            return ret;
        }
    }
    return ret;
}

static hi_void hi_dma_pcm_free(struct snd_pcm *pcm)
{
    snd_pcm_lib_preallocate_free_for_all(pcm);
}

static int hi_dma_probe(struct snd_soc_platform *soc_platform)
{
    int ret;

    struct hiaudio_data *had = dev_get_drvdata(soc_platform->dev);
    ret = hiaudio_ao_proc_init(soc_platform->component.card->snd_card, PROC_AO_NAME, had);
    if (ret < 0) {
        /* ATRC("had_init_debugfs fail %d", ret); */
    }
#ifdef HI_ALSA_AI_SUPPORT
    ret = hiaudio_ai_proc_init(soc_platform->component.card->snd_card, ALSA_PROC_AI_NAME, had);
    if (ret < 0) {
    }
#endif

    return ret;
}

static struct snd_soc_platform_driver aiao_soc_platform_drv = {
    .ops = &hi_dma_ops,
    .pcm_new = hi_dma_pcm_new,
    .pcm_free = hi_dma_pcm_free,
    .probe = hi_dma_probe,
};

static int soc_snd_platform_probe(struct platform_device *pdev)
{
    struct hiaudio_data *had = HI_NULL;
    int ret;

    had = osal_kmalloc(HI_ID_AO, sizeof(struct hiaudio_data), OSAL_GFP_KERNEL);
    if (had == HI_NULL) {
        return -ENOMEM;
    }

    osal_atomic_init(&had->atm_track_state);
#ifdef MUTE_FRAME_INPUT
    osal_atomic_init(&had->atm_record_state);
#endif

    dev_set_drvdata(&pdev->dev, had);

    had->h_timer_handle = INITIAL_VALUE;
    had->track_id = INITIAL_VALUE;
#ifdef HI_ALSA_AI_SUPPORT
#ifdef MUTE_FRAME_INPUT
    had->ai_writepos = 0;
    had->isr_total_cnt_c = 0;
#else
    had->ack_c_cnt = 0;
    had->last_c_pos = 0;
    had->current_c_pos = 0;
    had->ai_writepos = 0;
    had->ai_readpos = 0;
    had->ai_handle = INITIAL_VALUE;
    had->isr_total_cnt_c = 0;
#endif
#endif

#ifdef MUTE_FRAME_OUTPUT
    memset(dump_buf, 0, sizeof(hi_s32) * MUTE_FRAME_TIME * 192000 / 1000);
#endif

    ret = snd_soc_register_platform(&pdev->dev, &aiao_soc_platform_drv);
    if (ret < 0) {
        goto err;
    }

    return ret;
err:
#ifdef MUTE_FRAME_INPUT
    osal_atomic_destory(&had->atm_record_state);
#endif
    osal_atomic_destory(&had->atm_track_state);
    osal_kfree(HI_ID_AO, had);
    return ret;
}

static int __exit soc_snd_platform_remove(struct platform_device *pdev)
{
    struct hiaudio_data *had = dev_get_drvdata(&pdev->dev);
    if (had != HI_NULL) {
#ifdef MUTE_FRAME_INPUT
        osal_atomic_destory(&had->atm_record_state);
#endif
        osal_atomic_destory(&had->atm_track_state);
        osal_kfree(HI_ID_AO, had);
    }

#ifdef CONFIG_AIAO_ALSA_PROC_SUPPORT
    hiaudio_proc_cleanup();
#endif

    snd_soc_unregister_platform(&pdev->dev);

    return 0;
}

static struct platform_driver hiaudio_dma_driver = {
    .driver = {
        .name = "hisi-audio",
        .owner = THIS_MODULE,
    },
    .probe = soc_snd_platform_probe,
    .remove = __exit_p(soc_snd_platform_remove),
};

static struct platform_device *hiaudio_dma_device = HI_NULL;

int hiaudio_dma_init(hi_void)
{
    int ret;

    hiaudio_dma_device = platform_device_alloc("hisi-audio", -1);
    if (hiaudio_dma_device == HI_NULL) {
        ATRCC(KERN_ERR "hiaudio-dma so_c platform device: unable to register\n");
        return -ENOMEM;
    }

    ret = platform_device_add(hiaudio_dma_device);
    if (ret) {
        ATRCC(KERN_ERR "hiaudio-dma so_c platform device: unable to add\n");
        platform_device_put(hiaudio_dma_device);
        return ret;
    }

    return platform_driver_register(&hiaudio_dma_driver);
}

hi_void hiaudio_dma_deinit(hi_void)
{
    platform_device_unregister(hiaudio_dma_device);
    platform_driver_unregister(&hiaudio_dma_driver);
}
