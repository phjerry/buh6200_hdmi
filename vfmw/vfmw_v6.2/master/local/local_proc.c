/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "local_proc.h"
#include "vctrl_log.h"

hi_s32 local_proc_read(hi_s32 module, hi_void *p, hi_s32 chan_id)
{
    return vctrl_log_read(module, chan_id, p, 0);
}

hi_s32 local_proc_write(hi_s32 module, cmd_str_ptr cmd, hi_s32 count)
{
    return vctrl_log_write(module, cmd, count);
}
