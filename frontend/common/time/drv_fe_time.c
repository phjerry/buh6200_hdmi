/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: delay function
 * Author: SDK
 * Created: 2017-06-30
 */
#include "drv_fe_time.h"
#include "hi_osal.h"

hi_void tuner_mdelay(hi_u32 ms)
{
    osal_mdelay(ms);
}

hi_void tuner_udelay(hi_u32 us)
{
    osal_udelay(us);
}
