/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port hdmi include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HDMI_FUNC_H__
#define __HDMI_FUNC_H__

#include "drv_ao_op.h"
#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32  hdmi_op_set_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode mode);
hi_s32  hdmi_op_get_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode *mode);

#if defined(HI_SND_HDMI_SUPPORT)
hi_void hdmi_op_register_driver(struct osal_list_head *head);
#else
#define hdmi_op_register_driver(head)
#endif

hi_s32  hdmi_op_set_sample_rate(snd_op_state *snd_op, hi_u32 sample_rate);
hi_bool hdmi_op_check_format_support(snd_op_state *snd_op, hi_u32 format);
hi_void hdmi_op_set_mute(snd_op_state *snd_op);
hi_void hdmi_op_set_unmute(snd_op_state *snd_op);
hi_void hdmi_op_set_ao_attr(snd_op_state *snd_op, hdmi_ao_attr *ao_attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end__cplusplus */

#endif  /* __HDMI_FUNC_H__ */

