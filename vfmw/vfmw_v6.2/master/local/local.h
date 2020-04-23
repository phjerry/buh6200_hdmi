/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __LOCAL__H__
#define __LOCAL__H__

#include "vfmw.h"

hi_s32 local_open(hi_void);
hi_s32 local_close(hi_void);
hi_s32 local_init(hi_void *, hi_u32);
hi_s32 local_exit(hi_void *);
hi_s32 local_suspend(hi_void *);
hi_s32 local_resume(hi_void *);
hi_s32 local_get_image(hi_s32, hi_void *);
hi_s32 local_release_image(hi_s32, const hi_void *);
hi_s32 local_check_image(hi_s32, hi_void *);
hi_s32 local_control(hi_s32, hi_s32, hi_void *, hi_u32);

#endif