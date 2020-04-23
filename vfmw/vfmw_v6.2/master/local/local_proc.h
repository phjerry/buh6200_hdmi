/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __LOCAL__PROC__
#define __LOCAL__PROC__

#include "vfmw.h"
#include "vfmw_proc.h"

hi_s32 local_proc_read(hi_s32 module, hi_void *p, hi_s32 chan_id);
hi_s32 local_proc_write(hi_s32 module, cmd_str_ptr cmd, hi_s32 count);

#endif


