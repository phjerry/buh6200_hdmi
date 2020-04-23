/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao alsa driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#ifndef __ALSA_AIAO_H__
#define __ALSA_AIAO_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_s32  ao_alsa_mod_init(hi_void);
hi_void ao_alsa_mod_exit(hi_void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
