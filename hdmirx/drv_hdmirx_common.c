/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Implementation of common functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/18
 */
#include "drv_hdmirx_common.h"
#include <securec.h>

hi_slong hdmirx_time_diff_us(osal_timeval cur_time, osal_timeval last_time)
{
    hi_slong ret;
    ret = abs(((cur_time.tv_sec - last_time.tv_sec) * 1000000) + /* 1000000: 1000000 us per second */
        (cur_time.tv_usec - last_time.tv_usec));
    return ret;
}

hi_slong hdmirx_time_diff_ms(osal_timeval cur_time, osal_timeval last_time)
{
    hi_slong ret;
    ret = abs(((cur_time.tv_sec - last_time.tv_sec) * 1000) + /* 1000: 1000 us per second */
        (cur_time.tv_usec - last_time.tv_usec));
    return ret;
}