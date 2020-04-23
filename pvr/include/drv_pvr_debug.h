/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: PVR debug definition.
 * Author: sdk
 * Create: 2019-08-19
 */

#ifndef __DRV_PVR_DEBUG_H__
#define __DRV_PVR_DEBUG_H__

#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_PVR_TRACE_MUTEX_ON       1

#define hi_fatal_pvr(fmt...)              HI_FATAL_PRINT(HI_ID_PVR, fmt)
#define hi_err_pvr(fmt...)                HI_ERR_PRINT(HI_ID_PVR, fmt)
#define hi_warn_pvr(fmt...)               HI_WARN_PRINT(HI_ID_PVR, fmt)
#define hi_info_pvr(fmt...)               HI_INFO_PRINT(HI_ID_PVR, fmt)
#define hi_debug_pvr(fmt...)              HI_DBG_PRINT(HI_ID_PVR, fmt)
#define hi_print_pvr(fmt, args...)        ({HI_PRINT("[%s:%d]:" fmt "", __FUNCTION__, __LINE__, ## args);})

#define hi_pvr_err_enter()                hi_err_pvr("\t >>>> Enter Func:%s Line:%d\n", __FUNCTION__, __LINE__)
#define hi_pvr_err_exit()                 hi_err_pvr("\t <<<< Exit Func:%s Line:%d\n", __FUNCTION__, __LINE__)
#define hi_pvr_err_pos()                  hi_err_pvr("\t .... pos Func:%s Line:%d\n", __FUNCTION__, __LINE__)

#define hi_pvr_log_enter()                hi_warn_pvr("\t >>>> Enter Func:%s Line:%d\n", __FUNCTION__, __LINE__)
#define hi_pvr_log_exit()                 hi_warn_pvr("\t <<<< Exit Func:%s Line:%d\n", __FUNCTION__, __LINE__)
#define hi_pvr_log_pos()                  hi_warn_pvr("\t .... pos Func:%s Line:%d\n", __FUNCTION__, __LINE__)

#define hi_pvr_debug_enter()              hi_debug_pvr("\t >>>> Enter Func:%s Line:%d\n", __FUNCTION__, __LINE__)
#define hi_pvr_debug_exit()               hi_debug_pvr("\t <<<< Exit Func:%s Line:%d\n", __FUNCTION__, __LINE__)
#define hi_pvr_debug_pos()                hi_debug_pvr("\t .... pos Func:%s Line:%d\n", __FUNCTION__, __LINE__)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_PVR_DEBUG_H__ */
