/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao track func header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __TRACK_ROUTER_H__
#define __TRACK_ROUTER_H__

#include "track_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 track_get_aip_delay_ms(snd_card_state *card, snd_track_state *track, hi_u32 *delay_ms);

hi_void detect_stream_mode_change(snd_card_state *card, snd_track_state *track,
    snd_track_stream_attr *attr, stream_mode_change_attr *change);

hi_void snd_proc_pcm_route(snd_card_state *card, snd_track_state *track,
    stream_mode_change mode, snd_track_stream_attr *attr);

#if defined(HI_SND_HDMI_SUPPORT)
hi_void snd_proc_hdmi_route(snd_card_state *card, snd_track_state *track,
    stream_mode_change mode_change, snd_track_stream_attr *track_stream);
#endif

#if defined(HI_SND_SPDIF_SUPPORT)
hi_void snd_proc_spidf_route(snd_card_state *card, snd_track_state *track,
    stream_mode_change mode_change, snd_track_stream_attr *track_stream);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __TRACK_ROUTER_H__ */

