/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#ifndef __VPP_LOG_H__
#define __VPP_LOG_H__

#include "vfmw_proc.h"

hi_s32 vpp_log_read(hi_void *buf, hi_s32 len, hi_u32 *used);
hi_s32 vpp_log_write(cmd_str_ptr buffer, hi_u32 count);

#endif