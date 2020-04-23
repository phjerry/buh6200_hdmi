/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv drc functions include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRC_FUNC_H__
#define __DRC_FUNC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 snd_op_set_drc_enable(snd_op_state *state, hi_bool enable);
hi_s32 snd_op_set_drc_attr(snd_op_state *state, hi_ao_drc_attr *drc_attr);
hi_s32 snd_set_op_drc_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_drc_attr *drc_attr);
hi_s32 snd_get_op_drc_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_drc_attr *drc_attr);
hi_s32 snd_set_op_drc_enable(snd_card_state *card, hi_ao_port out_port, hi_bool enable);
hi_s32 snd_get_op_drc_enable(snd_card_state *card, hi_ao_port out_port, hi_bool *enable);
hi_void snd_op_get_def_drc_attr(snd_op_state *state);

hi_s32 snd_write_op_proc_drc(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif  /* __DRC_FUNC_H__ */

