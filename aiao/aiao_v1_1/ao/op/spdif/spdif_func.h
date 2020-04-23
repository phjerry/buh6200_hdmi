/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port spdif include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __SPDIF_FUNC_H__
#define __SPDIF_FUNC_H__

#include "hi_drv_proc.h"
#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_bool spdif_op_match(hi_ao_port ao_port);
hi_s32  spdif_op_set_category_code(snd_op_state *snd_op, hi_ao_spdif_category_code category_code);
hi_s32  spdif_op_get_category_code(snd_op_state *snd_op, hi_ao_spdif_category_code *category_code);
hi_s32  spdif_op_set_scms_mode(snd_op_state *snd_op, hi_ao_spdif_scms_mode scms_mode);
hi_s32  spdif_op_get_scms_mode(snd_op_state *snd_op, hi_ao_spdif_scms_mode *scms_mode);
hi_s32  spdif_op_set_output_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode mode);
hi_s32  spdif_op_get_output_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode *mode);

hi_void spdif_op_read_proc(hi_void *file, snd_op_state *snd_op);

#if defined(HI_SND_SPDIF_SUPPORT)
hi_void spdif_op_register_driver(struct osal_list_head *head);
#else
#define spdif_op_register_driver(head)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __SPDIF_FUNC_H__ */

