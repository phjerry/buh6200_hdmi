/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __TEE_CLT_COMM_H__
#define __TEE_CLT_COMM_H__

#include "vfmw.h"

hi_s32 tc_comm_init(hi_void);
hi_void tc_comm_deinit(hi_void);
hi_s32 tc_comm_send_command(hi_u32 cmd_id, hi_void *param, hi_u32 *ret_value);

#endif


