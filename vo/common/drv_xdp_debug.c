/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: vdp
* Create: 2019-08-12
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "drv_xdp_osal.h"


hi_u32 g_debug_mask = 0;
hi_void disp_set_debug_flag(hi_u32 u32mask, hi_bool enable)
{
    if (enable == HI_TRUE) {
        g_debug_mask |= u32mask;
    } else {
        g_debug_mask &= ~u32mask;
    }

    return;
}
