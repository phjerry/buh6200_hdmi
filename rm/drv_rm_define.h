/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm_define
 * Author: sm_rm
 * Create: 2015/11/25
 */

#ifndef __DRV_RM_DEFINE_H__
#define __DRV_RM_DEFINE_H__

#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RM_DRV_CHN_MAX_NUM           20      /* RM的最大通路个数 */
#define RM_DRV_WIN_MAX_COUNT         1
#define RM_DRV_LIST_NODE_NUM         20      /* 节点数 */
#define RM_DRV_IDLE_MAX_TIME_US      2000000 /* 2s */
#define RM_DRV_IDLE_MAX_USED_NODE    2       /* 2 nodes */
#define RM_DRV_PROC_LEN              12
#define RM_DRV_PROC_PARA_LEN         64

/* function trace log, print the called function name when function is error */
#define hi_err_print_call_fun_err(func, err_code) HI_LOG_ERR("Call %s Failed, Error Code: [0x%08X]\n", #func, err_code)

#define hi_rm_err_print_call_func(func, err_code)  hi_err_print_call_fun_err(func, err_code)
#define hi_rm_warn_print_call_func(func, err_code)                                           \
    HI_LOG_WARN("call \33[34m %s \33[0m failed, error code: [0x%08X]\n", #func, err_code)
#define hi_rm_dbg_print_call_func(func, err_code)                                            \
    HI_LOG_DBG("call \33[34m %s \33[0m failed, error code: [0x%08X]\n", #func, err_code)

#define HI_LOG_ERR(fmt...)         HI_ERR_PRINT(HI_ID_RM, fmt)
#define HI_LOG_WARN(fmt...)        HI_WARN_PRINT(HI_ID_RM, fmt)
#define HI_LOG_INFO(fmt...)        HI_INFO_PRINT(HI_ID_RM, fmt)
#define HI_LOG_DBG(fmt...)         HI_DBG_PRINT(HI_ID_RM, fmt)
#define hi_info_func_enter()       HI_INFO_PRINT(HI_ID_RM, " ===>[Enter]\n")
#define hi_info_func_exit()        HI_INFO_PRINT(HI_ID_RM, " <===[Exit]\n")
#define hi_dbg_func_enter()        HI_DBG_PRINT(HI_ID_RM, " ===>[Enter]\n")
#define hi_dbg_func_exit()         HI_DBG_PRINT(HI_ID_RM, " <===[Exit]\n")

/** used for displaying more detailed error information */
#define hi_err_print_s32(val)      HI_LOG_ERR("%s = %d\n",         #val, val)
#define hi_err_print_u32(val)      HI_LOG_ERR("%s = %u\n",         #val, val)
#define hi_err_print_s64(val)      HI_LOG_ERR("%s = %lld\n",       #val, val)
#define hi_err_print_u64(val)      HI_LOG_ERR("%s = %llu\n",       #val, val)
#define hi_err_print_h32(val)      HI_LOG_ERR("%s = 0x%08X\n",     #val, val)
#define hi_err_print_h64(val)      HI_LOG_ERR("%s = 0x%016ll_x\n",  #val, val)
#define hi_err_print_str(val)      HI_LOG_ERR("%s = %s\n",         #val, val)
#define hi_err_print_void(val)     HI_LOG_ERR("%s = %p\n",         #val, val)
#define hi_err_print_float(val)    HI_LOG_ERR("%s = %f\n",         #val, val)
#define hi_err_print_info(val)     HI_LOG_ERR("<%s>\n", val)

/** used for displaying more detailed warning information */
#define hi_warn_print_s32(val)     HI_LOG_WARN("%s = %d\n",        #val, val)
#define hi_warn_print_u32(val)     HI_LOG_WARN("%s = %u\n",        #val, val)
#define hi_warn_print_s64(val)     HI_LOG_WARN("%s = %lld\n",      #val, val)
#define hi_warn_print_u64(val)     HI_LOG_WARN("%s = %llu\n",      #val, val)
#define hi_warn_print_h32(val)     HI_LOG_WARN("%s = 0x%08X\n",    #val, val)
#define hi_warn_print_h64(val)     HI_LOG_WARN("%s = 0x%016ll_x\n", #val, val)
#define hi_warn_print_str(val)     HI_LOG_WARN("%s = %s\n",        #val, val)
#define hi_warn_print_void(val)    HI_LOG_WARN("%s = %p\n",        #val, val)
#define hi_warn_print_float(val)   HI_LOG_WARN("%s = %f\n",        #val, val)
#define hi_warn_print_info(val)    HI_LOG_WARN("<%s>\n", val)
#define hi_warn_print_bool(val)    HI_LOG_WARN("%s = %s\n", #val, val ? "True" : "False")

/** used for displaying more detailed info information */
#define hi_info_print_s32(val)     HI_LOG_INFO("%s = %d\n",        #val, val)
#define hi_info_print_u32(val)     HI_LOG_INFO("%s = %u\n",        #val, val)
#define hi_info_print_s64(val)     HI_LOG_INFO("%s = %lld\n",      #val, val)
#define hi_info_print_u64(val)     HI_LOG_INFO("%s = %llu\n",      #val, val)
#define hi_info_print_h32(val)     HI_LOG_INFO("%s = 0x%08X\n",    #val, val)
#define hi_info_print_h64(val)     HI_LOG_INFO("%s = 0x%016ll_x\n", #val, val)
#define hi_info_print_str(val)     HI_LOG_INFO("%s = %s\n",        #val, val)
#define hi_info_print_void(val)    HI_LOG_INFO("%s = %p\n",        #val, val)
#define hi_info_print_float(val)   HI_LOG_INFO("%s = %f\n",        #val, val)
#define hi_info_print_info(val)    HI_LOG_INFO("<%s>\n", val)

/** only used for self debug, can be expanded as needed */
#define hi_dbg_print_s32(val)      HI_LOG_DBG("%s = %d\n",         #val, val)
#define hi_dbg_print_u32(val)      HI_LOG_DBG("%s = %u\n",         #val, val)
#define hi_dbg_print_s64(val)      HI_LOG_DBG("%s = %lld\n",       #val, val)
#define hi_dbg_print_u64(val)      HI_LOG_DBG("%s = %llu\n",       #val, val)
#define hi_dbg_print_h32(val)      HI_LOG_DBG("%s = 0x%08X\n",     #val, val)
#define hi_dbg_print_h64(val)      HI_LOG_DBG("%s = 0x%016ll_x\n", #val, val)
#define hi_dbg_print_str(val)      HI_LOG_DBG("%s = %s\n",         #val, val)
#define hi_dbg_print_void(val)     HI_LOG_DBG("%s = %p\n",         #val, val)
#define hi_dbg_print_float(val)    HI_LOG_DBG("%s = %f\n",         #val, val)
#define hi_dbg_print_info(val)     HI_LOG_DBG("<%s>\n", val)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_RM_DEFINE_H__ */

