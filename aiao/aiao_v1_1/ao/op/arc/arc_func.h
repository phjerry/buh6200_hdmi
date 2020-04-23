/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port arc include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __ARC_FUNC_H__
#define __ARC_FUNC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_bool arc_op_match(hi_ao_port ao_port);
hi_s32 arc_op_set_enable(snd_card_state *card, snd_op_state *snd_op, hi_bool enable);
hi_s32 arc_op_get_enable(snd_card_state *card, snd_op_state *snd_op, hi_bool *enable);
hi_s32 arc_op_set_cap(snd_card_state *card, snd_op_state *snd_op, hi_ao_arc_audio_cap *cap);
hi_s32 arc_op_get_cap(snd_card_state *card, snd_op_state *snd_op, hi_ao_arc_audio_cap *cap);

#if defined(HI_SND_ARC_SUPPORT)
hi_void arc_op_register_driver(struct osal_list_head *head);
#else
#define arc_op_register_driver(head)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end__cplusplus */

#endif  /* __ARC_FUNC_H__ */

