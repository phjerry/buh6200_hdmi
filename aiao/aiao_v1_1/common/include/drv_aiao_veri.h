/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao cbb verification head file
 * Author: audio
 * Create: 2019-07-30
 * Notes: NA
 * History: 2019-07-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AIAO_VERI_H__
#define __DRV_AIAO_VERI_H__

#include "hi_drv_audio.h"

hi_void aiao_veri_open(hi_void);
hi_void aiao_veri_release(hi_void);
hi_s32 aiao_veri_process_cmd(hi_void *file, hi_u32 cmd, hi_void *arg);

#endif /* __DRV_AIAO_VERI_H__ */
