/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __MDC_MAILBOX_CLIENT__
#define __MDC_MAILBOX_CLIENT__

#include "vfmw.h"
#include "mdc_private.h"

hi_s32 mdc_clt_mailbox_init(hi_void);
hi_s32 mdc_clt_mailbox_deinit(hi_void);
hi_s32 mdc_clt_mailbox_send(hi_void *data, hi_u32 length);
hi_s32 mdc_clt_mailbox_read(hi_void *data, hi_u32 length);

hi_s32 mdc_clt_mailbox_set_notify_cb(notify_cb func);

#endif
