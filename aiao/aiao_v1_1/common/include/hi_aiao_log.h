/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio log function
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HI_AIAO_LOG_H__
#define __HI_AIAO_LOG_H__

#ifdef LOG_MODULE_ID
#error "audio debug header files include error!"
#endif

#define LOG_MODULE_ID  HI_ID_AO
#define LOG_FUNC_TRACE 1

#include "hi_debug.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HI_LOG_FATAL LOG_FATAL_PRINT
#define HI_LOG_ERR   LOG_ERR_PRINT
#define HI_LOG_WARN  LOG_WARN_PRINT
#define HI_LOG_INFO  LOG_INFO_PRINT
#define HI_LOG_DBG   LOG_DBG_PRINT

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

