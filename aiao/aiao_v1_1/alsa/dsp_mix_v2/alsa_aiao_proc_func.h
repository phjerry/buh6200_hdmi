/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao alsa driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#ifndef __DRV_AIAO_ALSA_PROC_H__
#define __DRV_AIAO_ALSA_PROC_H__

#include <sound/core.h>
#include "alsa_aiao_comm.h"

hi_s32 hiaudio_ao_proc_init(hi_void *card, const hi_char *name, struct hiaudio_data *had);
hi_s32 hiaudio_ai_proc_init(hi_void *card, const hi_char *name, struct hiaudio_data *had);

hi_void hiaudio_proc_cleanup(hi_void);

#endif
