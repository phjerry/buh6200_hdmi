/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __STREAM_LOG_H__
#define __STREAM_LOG_H__

#include "vfmw.h"
#include "vfmw_proc.h"

hi_s32 stream_log_read(hi_void *buf, hi_s32 len, hi_u32 *used);
hi_s32 stream_log_write(cmd_str_ptr buffer, hi_u32 count);

#endif