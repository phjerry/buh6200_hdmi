/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description:  common list function
* Author: xdp
* Create: 2019-08-12
 */

#include "drv_xdp_osal.h"

hi_u64 hi_drv_sys_get_time_stamp_us(hi_void)
{
    hi_u64 time_now;

    time_now = osal_sched_clock();

    return osal_div_u64(time_now, ISR_SYSTEM_TIME_US);
}
