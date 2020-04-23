/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq tool define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __DRV_PQ_TOOLS_H__
#define __DRV_PQ_TOOLS_H__

#include "hi_drv_pq.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 pq_tools_set_register(hi_pq_register *attr);
hi_s32 pq_tools_get_register(hi_pq_register *attr);
hi_s32 pq_tools_get_bin_phy_addr(hi_u32 *addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
