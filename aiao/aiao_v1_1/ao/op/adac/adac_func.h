/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv adac include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __ADAC_FUNC_H__
#define __ADAC_FUNC_H__

#include "drv_ao_op.h"

hi_bool adac_op_match(hi_ao_port ao_port);
hi_void snd_set_adac_enable(snd_op_state *snd_op, hi_bool enable);
hi_s32  snd_get_adac_enable(snd_op_state *snd_op, hi_bool *enable);

#if defined(HI_SND_ADAC_SUPPORT)
hi_void adac_op_register_driver(struct osal_list_head *head);
#else
#define adac_op_register_driver(head)
#endif

#endif  /* __ADAC_FUNC_H__ */

