/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: spread private Head File
 * Author: BSP
 * Create: 2019-11-29
 */

#ifndef __HI_DRV_SPREAD_H__
#define __HI_DRV_SPREAD_H__

#include "hi_drv_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define hi_fatal_spread(fmt...)   HI_FATAL_PRINT(HI_ID_SPREAD, fmt)
#define hi_err_spread(fmt...)     HI_ERR_PRINT(HI_ID_SPREAD, fmt)
#define hi_warn_spread(fmt...)    HI_WARN_PRINT(HI_ID_SPREAD, fmt)
#define hi_info_spread(fmt...)    HI_INFO_PRINT(HI_ID_SPREAD, fmt)
#define hi_dbg_spread(fmt...)     HI_DBG_PRINT(HI_ID_SPREAD, fmt)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DRV_SPREAD_H__ */
