/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv peq functions include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __PEQ_FUNC_H__
#define __PEQ_FUNC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 snd_op_set_peq_enable(snd_op_state *snd_op, hi_bool enable);
hi_s32 snd_op_set_peq_attr(snd_op_state *snd_op, hi_ao_peq_attr *peq_attr);
hi_s32 snd_set_op_peq_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_peq_attr *peq_attr);
hi_s32 snd_get_op_peq_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_peq_attr *peq_attr);
hi_s32 snd_set_op_peq_enable(snd_card_state *card, hi_ao_port out_port, hi_bool enable);
hi_s32 snd_get_op_peq_enable(snd_card_state *card, hi_ao_port out_port, hi_bool *enable);
hi_void snd_op_get_def_peq_attr(snd_op_state *snd_op);

hi_s32 snd_write_op_proc_peq(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRC_FUNC_H__ */

