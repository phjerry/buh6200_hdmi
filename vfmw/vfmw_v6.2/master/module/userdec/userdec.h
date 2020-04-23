/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_USER_H__
#define __VFMW_USER_H__

#include "vfmw.h"
#include "vfmw_proc.h"

hi_s32 user_open(hi_void);
hi_s32 user_close(hi_void);
hi_s32 user_init(hi_void *args, hi_u32 len);
hi_s32 user_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 param_length);
hi_s32 user_get_image(hi_s32 chan_id, hi_void *image);
hi_s32 user_release_image(hi_s32 chan_id, const hi_void *image);
hi_s32 user_info_read(hi_void *buf, hi_s32 chan_id);
hi_s32 user_info_write(cmd_str_ptr buffer, hi_u32 count);

#endif
