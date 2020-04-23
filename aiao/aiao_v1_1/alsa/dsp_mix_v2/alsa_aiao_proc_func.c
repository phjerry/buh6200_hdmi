/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao alsa driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "alsa_aiao_proc_func.h"

static hi_void hiaudio_ao_proc_read(struct snd_info_entry *entry,
    struct snd_info_buffer *buffer)
{
    struct hiaudio_data *had = entry->private_data;

    snd_iprintf(buffer,
                "timer_handle=0x%x, snd_open=0x%x  track_id=0x%x\n",
                had->h_timer_handle, had->snd_open, had->track_id);
    snd_iprintf(buffer,
                "track_state=0x%x, send_track(try/ok)=0x%x/0x%x,discard(0x%x),fail(0x%x)\n",
                osal_atomic_read(&had->atm_track_state), had->send_try_cnt,
                had->had_sent, had->discard_sent, had->send_fail_cnt);
    snd_iprintf(buffer,
                "periods=0x%x, peroid_time=%dns, mute_frame_time/port_delay=%d/%dms, hw_pointer=0x%x\n",
                had->ao_hw_param.periods, had->poll_time_ns, had->mute_frame_time, had->port_delay_ms, had->hw_pointer);
    snd_iprintf(buffer,
                "vir_base_addr=0x%p, phy_base_addr=0x%p, samples_per_frame(peroid)=0x%x\n",
                had->vir_base_addr, had->phy_base_addr, had->ao_frame.pcm_samples);
#ifdef CONFIG_PM
    snd_iprintf(buffer, " -----pm-----\n");
    snd_iprintf(buffer,
                " suspend_state=0x%x\n",
                had->suspend_state);
#endif
}

hi_s32 hiaudio_ao_proc_init(hi_void *card, const hi_char *name, struct hiaudio_data *had)
{
    hi_s32 ret;
    if ((card == HI_NULL) || (name == HI_NULL) || (had == HI_NULL)) {
        return -EINVAL;
    }
    ret = snd_card_proc_new((struct snd_card *)card, name, &had->entry);
    if (ret) {
    }
    snd_info_set_text_ops(had->entry, had, hiaudio_ao_proc_read);

    return 0;
}

#ifdef HI_ALSA_AI_SUPPORT
static hi_void hiaudio_ai_proc_read(struct snd_info_entry *entry,
    struct snd_info_buffer *buffer)
{
    struct hiaudio_data *had = entry->private_data;
    snd_iprintf(buffer,
                "ai_handle=%d,dma isr cnt =%d\n",
                had->ai_handle, had->isr_total_cnt_c);
    snd_iprintf(buffer,
                "  dma writepos=%d,dma read pos=%d\n",
                had->ai_writepos, had->ai_readpos);
#ifdef MUTE_FRAME_INPUT
    snd_iprintf(buffer,
                "  periods:%d, peroidtime_ns=%d,current_c_pos=%d\n",
                had->sthwparam_ai.periods, had->ai_poll_time_ns, had->current_c_pos);
#endif
}

hi_s32 hiaudio_ai_proc_init(hi_void *card, const hi_char *name, struct hiaudio_data *had)
{
    hi_s32 ret;
    if ((card == HI_NULL) || (name == HI_NULL) || (had == HI_NULL)) {
        return -EINVAL;
    }
    ret = snd_card_proc_new((struct snd_card *)card, name, &had->entry);
    if (ret) {
    }
    snd_info_set_text_ops(had->entry, had, hiaudio_ai_proc_read);
    return 0;
}
#endif

hi_void hiaudio_proc_cleanup(hi_void)
{
}
