/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ai alsa
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AI_ALSA_FUNC_H__
#define __DRV_AI_ALSA_FUNC_H__

#include "hi_drv_ai.h"

int hi_ai_alsa_open(ai_drv_create_param *ai_param, hi_void *file);
int hi_ai_alsa_update_readptr(int handle, unsigned int *write_pos);
void hi_ai_alsa_query_readpos(int handle, unsigned int *pos);
int hi_ai_alsa_flush_buffer(int handle);
void hi_ai_alsa_query_writepos(int handle, hi_u32 *pos);
int hi_ai_alsa_close(int handle, hi_void *file);
int hi_ai_alsa_set_enable(int handle, hi_bool enable);
int hi_ai_alsa_get_attr(hi_ai_port ai_port, hi_ai_attr *ai_attr);
int hi_ai_alsa_get_proc_func(aiao_isr_func **func);

#endif
