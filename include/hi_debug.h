/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: internal debug define.
 */

#ifndef __HI_DEBUG_H__
#define __HI_DEBUG_H__

#include "hi_type.h"
#include "hi_module.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HI_LOG_SUPPORT
#define HI_DEBUG

#ifdef __KERNEL__
    #define HI_PRINT printk
    #define HI_PANIC printk
#else
#include <stdio.h>
    #define HI_PRINT printf
    #define HI_PANIC printf
#endif
#else
#undef HI_DEBUG

#define HI_PANIC(fmt...) do { \
    } while (0)

#define HI_PRINT(fmt...) do { \
    } while (0)
#endif

/*
 * Default level of the output debugging information.
 * CNcomment: 默认的调试信息输出级别.
 */
#define HI_LOG_LEVEL_DEFAULT HI_TRACE_LEVEL_ERROR

/*
 * SDK internal Use only.
 * Allow modules to modify, If the related module does not define it, no information output.
 */
#ifndef LOG_FUNC_TRACE
#define LOG_FUNC_TRACE 0
#endif

/*
 * SDK internal Use Only.
 * Allow modules to modify, If the related module does not define it, no information output.
 */
#ifndef LOG_UNF_TRACE
#define LOG_UNF_TRACE  0
#endif

/*
 * SDK internal use only.
 * Allow modules to modify, default value is HI_ID_SYS.
 */
#ifndef LOG_MODULE_ID
#define LOG_MODULE_ID HI_ID_SYS
#endif

#define LOG_BLOCK_PERLINE (16)
#define LOG_BLOCK_BUFSIZE (LOG_BLOCK_PERLINE * 3 + 1)

/*
 * Level of the output debugging information.
 * CNcomment: 调试信息输出级别.
 */
typedef enum {
    HI_LOG_LEVEL_FATAL   = 0, /* Fatal error. It indicates that a critical problem occurs in the system. Therefore, you must pay attention to it. */
                              /* CNcomment: 致命错误, 此类错误需要特别关注，一般出现此类错误代表系统出现了重大问题 */
    HI_LOG_LEVEL_ERROR   = 1, /* Major error. It indicates that a major problem occurs in the system and the system cannot run. */
                              /* CNcomment: 一般错误, 一般出现此类错误代表系统出现了比较大的问题，不能再正常运行 */
    HI_LOG_LEVEL_WARNING = 2, /* Warning. It indicates that a minor problem occurs in the system, but the system still can run properly. */
                              /* CNcomment: 告警信息, 一般出现此类信息代表系统可能出现问题，但是还能继续运行 */
    HI_LOG_LEVEL_INFO    = 3, /* Message. It is used to prompt users. Users can open the message when locating problems. It is recommended to disable this message in general. */
                              /* CNcomment: 提示信息, 一般是为提醒用户而输出，在定位问题的时候可以打开，一般情况下建议关闭 */
    HI_LOG_LEVEL_DBG     = 4, /* Debug. It is used to prompt developers. Developers can open the message when locating problems. It is recommended to disable this message in general. */
                              /* CNcomment: 提示信息, 一般是为开发人员调试问题而设定的打印级别，一般情况下建议关闭 */
    HI_LOG_LEVEL_TRACE   = 5, /* Trace. It is used to track the entry and exit of function when the interface is called. */
                              /* CNcomment: 提示信息，一般用于跟踪接口调用时函数的进入与退出 */
    HI_LOG_LEVEL_MAX
} HI_LOG_LEVEL_E, hi_log_level;

/* Just only for fatal level print. */ /* CNcomment: 为了打印致命信息而制定的宏打印级别 */
#define HI_TRACE_LEVEL_FATAL    (0)
/* Just only for error level print. */ /* CNcomment: 为了打印错误信息而制定的宏打印级别 */
#define HI_TRACE_LEVEL_ERROR    (1)
/* Just only for warning level print. */ /* CNcomment: 为了打印警告信息而制定的宏打印级别 */
#define HI_TRACE_LEVEL_WARN     (2)
/* Just only for info level print. */ /* CNcomment: 为了打印信息级别而制定的宏打印级别 */
#define HI_TRACE_LEVEL_INFO     (3)
/* Just only for debug level print. */ /* CNcomment: 为了打印调试信息而制定的宏打印级别 */
#define HI_TRACE_LEVEL_DBG      (4)
/* Just only for trace level print. */ /* CNcomment: 为了打印接口跟踪信息而制定的宏打印级别 */
#define HI_TRACE_LEVEL_TRACE    (5)

#ifndef HI_LOG_LEVEL
#define HI_LOG_LEVEL (HI_TRACE_LEVEL_INFO)
#endif

/* Just only debug output, MUST BE NOT calling it. */
/* CNcomment: 调试输出信息接口，不推荐直接调用此接口 */
hi_void hi_log_print(hi_u32 level, hi_u32 module_id, hi_u8 *fn_name, hi_u32 line_num, const hi_char *format, ...);

#ifdef HI_DEBUG

#define HI_TRACE(level, module_id, fmt...) do {                               \
    hi_log_print(level, (hi_u32)module_id, (hi_u8*)__FUNCTION__, __LINE__, fmt); \
} while (0)

#define HI_ASSERT(expr) do {                                                                                      \
    if (!(expr)) {                                                                                                \
        HI_PANIC("\nASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
                    __FILE__,__FUNCTION__, __LINE__, #expr);                                                              \
    }                                                                                                             \
} while (0)

#define HI_ASSERT_RET(expr) do {                           \
    if (!(expr)) {                                         \
        HI_PRINT("\n<%s %d>: ASSERT Failure{" #expr "}\n", \
                    __FUNCTION__, __LINE__);               \
        return (-1);                                 \
    }                                                      \
} while (0)

#define LOG_PRINT_BLOCK(level, block, size) do {                                 \
    hi_u32 i_ = 0;                                                               \
    hi_char str_out_[LOG_BLOCK_BUFSIZE] = {0};                                   \
    hi_char *buf_ = (hi_char *)(block);                                          \
    HI_TRACE(level, LOG_MODULE_ID, " %s\n", #block);                             \
    for (i_ = 0; i_ < (size); i_++) {                                            \
        snprintf(&str_out_[(i_ % LOG_BLOCK_PERLINE) * 3], 4, " %02X", buf_[i_]); \
        if(((i_ + 1) % LOG_BLOCK_PERLINE) == 0) {                                \
            HI_TRACE(level, LOG_MODULE_ID, " %s\n", str_out_);                   \
            memset(str_out_, 0x00, sizeof(str_out_));                            \
        }                                                                        \
    }                                                                            \
    if(((i_ % LOG_BLOCK_PERLINE) != 0) && (i_ != 0)) {                           \
        HI_TRACE(level, LOG_MODULE_ID, " %s\n", str_out_);                       \
    }                                                                            \
} while (0)

/* Supported for debug output to serial/network/u-disk. */
/* CNcomment: 各个模块需要调用以下宏进行输出调试信息、可输出到串口、网口、U盘存储等 */

/* Just only reserve the fatal level output. */
/* CNcomment: 仅仅保留致命的调试信息 */
#if (HI_LOG_LEVEL == HI_TRACE_LEVEL_FATAL)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL, module_id, fmt)
#define HI_ERR_PRINT(module_id, fmt...)
#define HI_WARN_PRINT(module_id, fmt...)
#define HI_INFO_PRINT(module_id, fmt...)
#define HI_DBG_PRINT(module_id, fmt...)
#define HI_TRACE_PRINT(module_id, fmt...)

#define LOG_FATAL_PRINT(fmt...)             HI_TRACE(HI_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define LOG_ERR_PRINT(fmt...)
#define LOG_WARN_PRINT(fmt...)
#define LOG_INFO_PRINT(fmt...)
#define LOG_DBG_PRINT(fmt...)

#define HI_FATAL_PRINT_BLOCK(block, size)   LOG_PRINT_BLOCK(HI_TRACE_LEVEL_FATAL, block, size)
#define HI_ERR_PRINT_BLOCK(block, size)
#define HI_WARN_PRINT_BLOCK(block, size)
#define HI_INFO_PRINT_BLOCK(block, size)
#define HI_DBG_PRINT_BLOCK(block, size)
/* Just only reserve the fatal/error level output. */
/* CNcomment: 仅仅保留致命的和错误级别的调试信息 */
#elif (HI_LOG_LEVEL == HI_TRACE_LEVEL_ERROR)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL, module_id, fmt)
#define HI_ERR_PRINT(module_id, fmt...)     HI_TRACE(HI_TRACE_LEVEL_ERROR, module_id, fmt)
#define HI_WARN_PRINT(module_id, fmt...)
#define HI_INFO_PRINT(module_id, fmt...)
#define HI_DBG_PRINT(module_id, fmt...)
#define HI_TRACE_PRINT(module_id, fmt...)

#define LOG_FATAL_PRINT(fmt...)             HI_TRACE(HI_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define LOG_ERR_PRINT(fmt...)               HI_TRACE(HI_TRACE_LEVEL_ERROR, LOG_MODULE_ID, fmt)
#define LOG_WARN_PRINT(fmt...)
#define LOG_INFO_PRINT(fmt...)
#define LOG_DBG_PRINT(fmt...)

#define HI_FATAL_PRINT_BLOCK(block, size)   LOG_PRINT_BLOCK(HI_TRACE_LEVEL_FATAL, block, size)
#define HI_ERR_PRINT_BLOCK(block, size)     LOG_PRINT_BLOCK(HI_TRACE_LEVEL_ERROR, block, size)
#define HI_WARN_PRINT_BLOCK(block, size)
#define HI_INFO_PRINT_BLOCK(block, size)
#define HI_DBG_PRINT_BLOCK(block, size)
/* Just only reserve the fatal/error/warning level output. */
/* CNcomment: 仅仅保留致命的、错误的、警告级别的调试信息 */
#elif (HI_LOG_LEVEL == HI_TRACE_LEVEL_WARN)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL, module_id, fmt)
#define HI_ERR_PRINT(module_id, fmt...)     HI_TRACE(HI_TRACE_LEVEL_ERROR, module_id, fmt)
#define HI_WARN_PRINT(module_id, fmt...)    HI_TRACE(HI_TRACE_LEVEL_WARN,  module_id, fmt)
#define HI_INFO_PRINT(module_id, fmt...)
#define HI_DBG_PRINT(module_id, fmt...)
#define HI_TRACE_PRINT(module_id, fmt...)

#define LOG_FATAL_PRINT(fmt...)             HI_TRACE(HI_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define LOG_ERR_PRINT(fmt...)               HI_TRACE(HI_TRACE_LEVEL_ERROR, LOG_MODULE_ID, fmt)
#define LOG_WARN_PRINT(fmt...)              HI_TRACE(HI_TRACE_LEVEL_WARN,  LOG_MODULE_ID, fmt)
#define LOG_INFO_PRINT(fmt...)
#define LOG_DBG_PRINT(fmt...)

#define HI_FATAL_PRINT_BLOCK(block, size)   LOG_PRINT_BLOCK(HI_TRACE_LEVEL_FATAL, block, size)
#define HI_ERR_PRINT_BLOCK(block, size)     LOG_PRINT_BLOCK(HI_TRACE_LEVEL_ERROR, block, size)
#define HI_WARN_PRINT_BLOCK(block, size)    LOG_PRINT_BLOCK(HI_TRACE_LEVEL_WARN, block, size)
#define HI_INFO_PRINT_BLOCK(block, size)
#define HI_DBG_PRINT_BLOCK(block, size)
/* Just only reserve the fatal/error/warning/info level output. */
/* CNcomment: 仅仅保留致命的、错误的、警告和信息级别的调试信息 */
#elif (HI_LOG_LEVEL == HI_TRACE_LEVEL_INFO)
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL, module_id, fmt)
#define HI_ERR_PRINT(module_id, fmt...)     HI_TRACE(HI_TRACE_LEVEL_ERROR, module_id, fmt)
#define HI_WARN_PRINT(module_id, fmt...)    HI_TRACE(HI_TRACE_LEVEL_WARN,  module_id, fmt)
#define HI_INFO_PRINT(module_id, fmt...)    HI_TRACE(HI_TRACE_LEVEL_INFO,  module_id, fmt)
#define HI_DBG_PRINT(module_id, fmt...)
#define HI_TRACE_PRINT(module_id, fmt...)

#define LOG_FATAL_PRINT(fmt...)             HI_TRACE(HI_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define LOG_ERR_PRINT(fmt...)               HI_TRACE(HI_TRACE_LEVEL_ERROR, LOG_MODULE_ID, fmt)
#define LOG_WARN_PRINT(fmt...)              HI_TRACE(HI_TRACE_LEVEL_WARN,  LOG_MODULE_ID, fmt)
#define LOG_INFO_PRINT(fmt...)              HI_TRACE(HI_TRACE_LEVEL_INFO,  LOG_MODULE_ID, fmt)
#define LOG_DBG_PRINT(fmt...)

#define HI_FATAL_PRINT_BLOCK(block, size)   LOG_PRINT_BLOCK(HI_TRACE_LEVEL_FATAL, block, size)
#define HI_ERR_PRINT_BLOCK(block, size)     LOG_PRINT_BLOCK(HI_TRACE_LEVEL_ERROR, block, size)
#define HI_WARN_PRINT_BLOCK(block, size)    LOG_PRINT_BLOCK(HI_TRACE_LEVEL_WARN, block, size)
#define HI_INFO_PRINT_BLOCK(block, size)    LOG_PRINT_BLOCK(HI_TRACE_LEVEL_INFO, block, size)
#define HI_DBG_PRINT_BLOCK(block, size)
#else
/* Reserve all the levels output. */
/* CNcomment: 保留所有级别调试信息 */
#define HI_FATAL_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_FATAL, module_id, fmt)
#define HI_ERR_PRINT(module_id, fmt...)     HI_TRACE(HI_TRACE_LEVEL_ERROR, module_id, fmt)
#define HI_WARN_PRINT(module_id, fmt...)    HI_TRACE(HI_TRACE_LEVEL_WARN,  module_id, fmt)
#define HI_INFO_PRINT(module_id, fmt...)    HI_TRACE(HI_TRACE_LEVEL_INFO,  module_id, fmt)
#define HI_DBG_PRINT(module_id, fmt...)     HI_TRACE(HI_TRACE_LEVEL_DBG,   module_id, fmt)
#define HI_TRACE_PRINT(module_id, fmt...)   HI_TRACE(HI_TRACE_LEVEL_TRACE, module_id, fmt)

#define LOG_FATAL_PRINT(fmt...)             HI_TRACE(HI_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define LOG_ERR_PRINT(fmt...)               HI_TRACE(HI_TRACE_LEVEL_ERROR, LOG_MODULE_ID, fmt)
#define LOG_WARN_PRINT(fmt...)              HI_TRACE(HI_TRACE_LEVEL_WARN,  LOG_MODULE_ID, fmt)
#define LOG_INFO_PRINT(fmt...)              HI_TRACE(HI_TRACE_LEVEL_INFO,  LOG_MODULE_ID, fmt)
#define LOG_DBG_PRINT(fmt...)               HI_TRACE(HI_TRACE_LEVEL_DBG,   LOG_MODULE_ID, fmt)

#define HI_FATAL_PRINT_BLOCK(block, size)   LOG_PRINT_BLOCK(HI_TRACE_LEVEL_FATAL, block, size)
#define HI_ERR_PRINT_BLOCK(block, size)     LOG_PRINT_BLOCK(HI_TRACE_LEVEL_ERROR, block, size)
#define HI_WARN_PRINT_BLOCK(block, size)    LOG_PRINT_BLOCK(HI_TRACE_LEVEL_WARN, block, size)
#define HI_INFO_PRINT_BLOCK(block, size)    LOG_PRINT_BLOCK(HI_TRACE_LEVEL_INFO, block, size)
#define HI_DBG_PRINT_BLOCK(block, size)     LOG_PRINT_BLOCK(HI_TRACE_LEVEL_DBG, block, size)
#endif

#else
#define HI_TRACE(level, module_id, fmt...)
#define HI_ASSERT(expr)
#define HI_ASSERT_RET(expr)
#define LOG_PRINT_BLOCK(level, block, size)

#define HI_FATAL_PRINT(module_id, fmt...)
#define HI_ERR_PRINT(module_id, fmt...)
#define HI_WARN_PRINT(module_id, fmt...)
#define HI_INFO_PRINT(module_id, fmt...)
#define HI_DBG_PRINT(module_id, fmt...)
#define HI_TRACE_PRINT(module_id, fmt...)

#define LOG_FATAL_PRINT(fmt...)
#define LOG_ERR_PRINT(fmt...)
#define LOG_WARN_PRINT(fmt...)
#define LOG_INFO_PRINT(fmt...)
#define LOG_DBG_PRINT(fmt...)

#define HI_FATAL_PRINT_BLOCK(block, size)
#define HI_ERR_PRINT_BLOCK(block, size)
#define HI_WARN_PRINT_BLOCK(block, size)
#define HI_INFO_PRINT_BLOCK(block, size)
#define HI_DBG_PRINT_BLOCK(block, size)
#endif /* endif HI_DEBUG */

/* Function trace log, strictly prohibited to expand */
#define HI_FATAL_PRINT_FUNC_RES(func, err_code)  LOG_FATAL_PRINT("Call %s return [0x%08X]\n", #func, err_code);
#define HI_FATAL_PRINT_ERR_CODE(err_code)        LOG_FATAL_PRINT("Error Code: [0x%08X]\n", err_code);

#define HI_ERR_PRINT_FUNC_RES(func, err_code)    LOG_ERR_PRINT("Call %s return [0x%08X]\n", #func, err_code);
#define HI_ERR_PRINT_ERR_CODE(err_code)          LOG_ERR_PRINT("Error Code: [0x%08X]\n", err_code);

#define HI_WARN_PRINT_FUNC_RES(func, err_code)   LOG_WARN_PRINT("Call %s return [0x%08X]\n", #func, err_code);
#define HI_WARN_PRINT_ERR_CODE(err_code)         LOG_WARN_PRINT("Error Code: [0x%08X]\n", err_code);

/* Used for displaying more detailed fatal information */
#define HI_FATAL_PRINT_S32(val)   LOG_FATAL_PRINT("%s = %d\n",        #val, val)
#define HI_FATAL_PRINT_U32(val)   LOG_FATAL_PRINT("%s = %u\n",        #val, val)
#define HI_FATAL_PRINT_S64(val)   LOG_FATAL_PRINT("%s = %lld\n",      #val, val)
#define HI_FATAL_PRINT_U64(val)   LOG_FATAL_PRINT("%s = %llu\n",      #val, val)
#define HI_FATAL_PRINT_H32(val)   LOG_FATAL_PRINT("%s = 0x%08X\n",    #val, val)
#define HI_FATAL_PRINT_H64(val)   LOG_FATAL_PRINT("%s = 0x%016llX\n", #val, val)
#define HI_FATAL_PRINT_STR(val)   LOG_FATAL_PRINT("%s = %s\n",        #val, val)
#define HI_FATAL_PRINT_VOID(val)  LOG_FATAL_PRINT("%s = %p\n",        #val, val)
#define HI_FATAL_PRINT_FLOAT(val) LOG_FATAL_PRINT("%s = %f\n",        #val, val)
#define HI_FATAL_PRINT_BOOL(val)  LOG_FATAL_PRINT("%s = %s\n",        #val, val ? "True" : "False")
#define HI_FATAL_PRINT_INFO(val)  LOG_FATAL_PRINT("<%s>\n",            val)

/* Used for displaying more detailed error information */
#define HI_ERR_PRINT_S32(val)     LOG_ERR_PRINT("%s = %d\n",        #val, val)
#define HI_ERR_PRINT_U32(val)     LOG_ERR_PRINT("%s = %u\n",        #val, val)
#define HI_ERR_PRINT_S64(val)     LOG_ERR_PRINT("%s = %lld\n",      #val, val)
#define HI_ERR_PRINT_U64(val)     LOG_ERR_PRINT("%s = %llu\n",      #val, val)
#define HI_ERR_PRINT_H32(val)     LOG_ERR_PRINT("%s = 0x%08X\n",    #val, val)
#define HI_ERR_PRINT_H64(val)     LOG_ERR_PRINT("%s = 0x%016llX\n", #val, val)
#define HI_ERR_PRINT_STR(val)     LOG_ERR_PRINT("%s = %s\n",        #val, val)
#define HI_ERR_PRINT_VOID(val)    LOG_ERR_PRINT("%s = %p\n",        #val, val)
#define HI_ERR_PRINT_FLOAT(val)   LOG_ERR_PRINT("%s = %f\n",        #val, val)
#define HI_ERR_PRINT_BOOL(val)    LOG_ERR_PRINT("%s = %s\n",        #val, val ? "True" : "False")
#define HI_ERR_PRINT_INFO(val)    LOG_ERR_PRINT("<%s>\n",            val)

/* Used for displaying more detailed warning information */
#define HI_WARN_PRINT_S32(val)    LOG_WARN_PRINT("%s = %d\n",        #val, val)
#define HI_WARN_PRINT_U32(val)    LOG_WARN_PRINT("%s = %u\n",        #val, val)
#define HI_WARN_PRINT_S64(val)    LOG_WARN_PRINT("%s = %lld\n",      #val, val)
#define HI_WARN_PRINT_U64(val)    LOG_WARN_PRINT("%s = %llu\n",      #val, val)
#define HI_WARN_PRINT_H32(val)    LOG_WARN_PRINT("%s = 0x%08X\n",    #val, val)
#define HI_WARN_PRINT_H64(val)    LOG_WARN_PRINT("%s = 0x%016llX\n", #val, val)
#define HI_WARN_PRINT_STR(val)    LOG_WARN_PRINT("%s = %s\n",        #val, val)
#define HI_WARN_PRINT_VOID(val)   LOG_WARN_PRINT("%s = %p\n",        #val, val)
#define HI_WARN_PRINT_FLOAT(val)  LOG_WARN_PRINT("%s = %f\n",        #val, val)
#define HI_WARN_PRINT_BOOL(val)   LOG_WARN_PRINT("%s = %s\n",        #val, val ? "True" : "False")
#define HI_WARN_PRINT_INFO(val)   LOG_WARN_PRINT("<%s>\n",            val)

/* Only used for key info, Can be expanded as needed */
#define HI_INFO_PRINT_S32(val)    LOG_INFO_PRINT("%s = %d\n",        #val, val)
#define HI_INFO_PRINT_U32(val)    LOG_INFO_PRINT("%s = %u\n",        #val, val)
#define HI_INFO_PRINT_S64(val)    LOG_INFO_PRINT("%s = %lld\n",      #val, val)
#define HI_INFO_PRINT_U64(val)    LOG_INFO_PRINT("%s = %llu\n",      #val, val)
#define HI_INFO_PRINT_H32(val)    LOG_INFO_PRINT("%s = 0x%08X\n",    #val, val)
#define HI_INFO_PRINT_H64(val)    LOG_INFO_PRINT("%s = 0x%016llX\n", #val, val)
#define HI_INFO_PRINT_STR(val)    LOG_INFO_PRINT("%s = %s\n",        #val, val)
#define HI_INFO_PRINT_VOID(val)   LOG_INFO_PRINT("%s = %p\n",        #val, val)
#define HI_INFO_PRINT_FLOAT(val)  LOG_INFO_PRINT("%s = %f\n",        #val, val)
#define HI_INFO_PRINT_BOOL(val)   LOG_INFO_PRINT("%s = %s\n",        #val, val ? "True" : "False")
#define HI_INFO_PRINT_INFO(val)   LOG_INFO_PRINT("<%s>\n",            val)

/* Only used for self debug, Can be expanded as needed */
#define HI_DBG_PRINT_S32(val)     LOG_DBG_PRINT("%s = %d\n",        #val, val)
#define HI_DBG_PRINT_U32(val)     LOG_DBG_PRINT("%s = %u\n",        #val, val)
#define HI_DBG_PRINT_S64(val)     LOG_DBG_PRINT("%s = %lld\n",      #val, val)
#define HI_DBG_PRINT_U64(val)     LOG_DBG_PRINT("%s = %llu\n",      #val, val)
#define HI_DBG_PRINT_H32(val)     LOG_DBG_PRINT("%s = 0x%08X\n",    #val, val)
#define HI_DBG_PRINT_H64(val)     LOG_DBG_PRINT("%s = 0x%016llX\n", #val, val)
#define HI_DBG_PRINT_STR(val)     LOG_DBG_PRINT("%s = %s\n",        #val, val)
#define HI_DBG_PRINT_VOID(val)    LOG_DBG_PRINT("%s = %p\n",        #val, val)
#define HI_DBG_PRINT_FLOAT(val)   LOG_DBG_PRINT("%s = %f\n",        #val, val)
#define HI_DBG_PRINT_BOOL(val)    LOG_DBG_PRINT("%s = %s\n",        #val, val ? "True" : "False")
#define HI_DBG_PRINT_INFO(val)    LOG_DBG_PRINT("<%s>\n",            val)

#if (LOG_FUNC_TRACE == 1) || (LOG_UNF_TRACE == 1)
#define HI_UNF_FUNC_ENTER() LOG_DBG_PRINT(" >>>>>>[Enter]\n") /* only used for unf interface */
#define HI_UNF_FUNC_EXIT()  LOG_DBG_PRINT(" <<<<<<[Exit]\n")  /* only used for unf interface */
#else
#define HI_UNF_FUNC_ENTER()
#define HI_UNF_FUNC_EXIT()
#endif

#if LOG_FUNC_TRACE
#define HI_FUNC_ENTER() LOG_DBG_PRINT(" =====>[Enter]\n")
#define HI_FUNC_EXIT()  LOG_DBG_PRINT(" =====>[Exit]\n")
#else
#define HI_FUNC_ENTER()
#define HI_FUNC_EXIT()
#endif

#define HI_CHECK(func) do {                       \
    hi_s32 err_code_ = func;                      \
    if (err_code_ != HI_SUCCESS) {                \
        HI_WARN_PRINT_FUNC_RES(#func, err_code_); \
    }                                             \
} while (0)

#define HI_CHECK_PARAM(val, err_code_print, err_code_ret) do { \
    if (val) {                                                 \
        HI_ERR_PRINT_ERR_CODE(err_code_print);                 \
        return err_code_ret;                                   \
    }                                                          \
} while (0)

#define MKSTR(exp) # exp
#define MKMARCOTOSTR(exp) MKSTR(exp)
#define VERSION_STRING ("SDK_VERSION: [" MKMARCOTOSTR(SDK_VERSION) "] Build Time: [" __DATE__ ", " __TIME__ "]")
#define USER_VERSION_STRING ("SDK_VERSION: [" MKMARCOTOSTR(SDK_VERSION) "]")

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_DEBUG_H__ */

