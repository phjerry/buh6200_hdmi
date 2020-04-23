/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv aef functions include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __AEF_FUNC_H__
#define __AEF_FUNC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifdef HI_SND_AEF_SUPPORT
hi_s32 aef_attach_snd(snd_card_state *card, hi_u32 aef_id, ao_aef_attr *aef_attr, hi_u32 *aef_proc_addr);
hi_s32 aef_detach_snd(snd_card_state *card, hi_u32 aef_id, ao_aef_attr *aef_attr);
hi_s32 aef_get_buf_attr(snd_card_state *card, ao_aef_buf_attr *aef_buf);
hi_s32 aef_get_debug_addr(snd_card_state *card, aef_debug_attr *debug_attr);

hi_s32 snd_set_op_aef_bypass(snd_card_state *card, hi_ao_port out_port, hi_bool bypass);
hi_s32 snd_get_op_aef_bypass(snd_card_state *card, hi_ao_port out_port, hi_bool *bypass);
hi_s32 snd_op_set_aef_bypass(snd_op_state *snd_op, hi_bool bypass);
hi_void snd_op_set_def_aef_attr(snd_op_state *snd_op, aoe_aop_id aop);

hi_s32 snd_write_op_proc_aef_bypass(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf);
#else
#define snd_op_set_def_aef_attr(snd_op, aop)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __AEF_FUNC_H__ */

