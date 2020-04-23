/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio encoder driver ext header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AENC_EXT_H__
#define __DRV_AENC_EXT_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_s32  aenc_drv_mod_init(hi_void);
hi_void aenc_drv_mod_exit(hi_void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_AENC_EXT_H__ */
