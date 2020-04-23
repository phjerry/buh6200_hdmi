/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv hdmi include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_HDMI_H__
#define __DRV_AO_HDMI_H__

#include "audio_util.h"
#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_bool ao_snd_hdmi_check_format_support(snd_card_state *card, hi_ao_port out_port, hi_u32 format);
hi_void ao_snd_hdmi_set_mute(snd_card_state *card, hi_ao_port out_port, hi_bool mute);
hi_void ao_snd_hdmi_set_ao_attr(snd_card_state *card, hi_ao_port out_port, hdmi_ao_attr *ao_attr);

#ifdef HI_PROC_SUPPORT
hi_s32 ao_write_proc_set_hdmi(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_HDMI_H__ */

