/************************************************************************************************
*
* Copyright (C) 2018 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
*************************************************************************************************
File Name        : hi_gfx_sys_k.h
Version          : version 1.0
Author           :
Created          : 2018/01/01
Description      : define system function
Function List    :

History          :
Date               Author                Modification
2018/01/01          sdk
***********************************************************************************************/
#ifndef _HI_GFX_SYS_K_H_
#define _HI_GFX_SYS_K_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef CONFIG_USE_SDK_LOG
#include "hi_debug.h"
#endif
/* **************************** Macro Definition ********************************************** */
#define FUNC_TAG __func__
#define LINE_TAG __LINE__
#define FORMAT_TAG "format"
#define FAILURE_TAG HI_FAILURE
/* *============================================================================================
 **BEG DFX
 **===========================================================================================* */
#ifdef GRAPHIC_COMM_PRINT
#undef GRAPHIC_COMM_PRINT
#endif

#if defined(HI_LOG_SUPPORT) && (0 == HI_LOG_SUPPORT)
#define GRAPHIC_COMM_PRINT(fmt, args...)  // {do{}while(0);}
#else
#define GRAPHIC_COMM_PRINT osal_printk
#endif

#ifdef CONFIG_GFX_DFX_MOD_JPGE
#define GraphicLogOut jpge_out_msg
#define HI_GFX_SetLogFunc jpge_set_log_func
#define HI_GFX_SetLogLevel jpge_set_log_level
#define HI_GFX_SetLogSave jpge_set_log_save
#define HI_GFX_ProcMsg jpge_out_set_log_info
#elif defined(CONFIG_GFX_DFX_MOD_JPEG)
#define GraphicLogOut DRV_JPEG_OutputMessage
#define HI_GFX_SetLogFunc DRV_JPEG_SetLogFunc
#define HI_GFX_SetLogLevel DRV_JPEG_SetLogLevel
#define HI_GFX_SetLogSave DRV_JPEG_SetLogSave
#define HI_GFX_ProcMsg DRV_JPEG_ProcMsg
#elif defined(CONFIG_GFX_DFX_MOD_PNG)
#define GraphicLogOut png_out_msg
#define HI_GFX_SetLogFunc png_set_log_func
#define HI_GFX_SetLogLevel png_set_log_level
#define HI_GFX_SetLogSave png_set_log_save
#define HI_GFX_ProcMsg png_out_set_log_info
#elif defined(CONFIG_GFX_DFX_MOD_TDE)
#define GraphicLogOut tde_out_msg
#define HI_GFX_SetLogFunc tde_set_log_func
#define HI_GFX_SetLogLevel tde_set_log_level
#define HI_GFX_SetLogSave tde_set_log_save
#define HI_GFX_ProcMsg tde_out_set_log_info
#elif defined(CONFIG_GFX_DFX_MOD_GFX2D)
#define GraphicLogOut DRV_GFX2D_OutputMessage
#define HI_GFX_SetLogFunc DRV_GFX2D_SetLogFunc
#define HI_GFX_SetLogLevel DRV_GFX2D_SetLogLevel
#define HI_GFX_SetLogSave DRV_GFX2D_SetLogSave
#define HI_GFX_ProcMsg DRV_GFX2D_ProcMsg
#elif defined(CONFIG_GFX_DFX_MOD_FB)
#define GraphicLogOut DRV_HIFB_OutputMessage
#define HI_GFX_SetLogFunc DRV_HIFB_SetLogFunc
#define HI_GFX_SetLogLevel DRV_HIFB_SetLogLevel
#define HI_GFX_SetLogSave DRV_HIFB_SetLogSave
#define HI_GFX_ProcMsg DRV_HIFB_ProcMsg
#endif

#ifndef CONFIG_USE_SDK_LOG
#ifdef CONFIG_GFX_DFX_DEBUG
#define GRAPHIC_DFX_DEBUG_FUNC(MsgType) GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 0)
#else
#define GRAPHIC_DFX_DEBUG_FUNC(fmt, args...)
#endif

#ifdef CONFIG_GFX_DFX_INFO
#define GRAPHIC_DFX_DEBUG_INFO(MsgType, Value) GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 1, Value)
#define GRAPHIC_DFX_DEBUG_VALUE(MsgType, Value) GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 0, #Value, Value)
#else
#define GRAPHIC_DFX_DEBUG_INFO(fmt, args...)
#define GRAPHIC_DFX_DEBUG_VALUE(fmt, args...)
#endif

#ifdef CONFIG_GFX_DFX_ERR
#define GRAPHIC_DFX_ERR_FUNC(MsgType, CallFunc, Value) \
    GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 3, #CallFunc, #Value, Value)
#define GRAPHIC_DFX_ERR_INFO(MsgType, Value) GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 1, Value)
#define GRAPHIC_DFX_ERR_VALUE(MsgType, Value) GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 2, #Value, Value)
#else
#define GRAPHIC_DFX_ERR_FUNC(fmt, args...)
#define GRAPHIC_DFX_ERR_INFO(fmt, args...)
#define GRAPHIC_DFX_ERR_VALUE(fmt, args...)
#endif

#if defined(CONFIG_GFX_DFX_MINI) && defined(CONFIG_GFX_DFX_ERR)
#define GRAPHIC_DFX_MINI_ERR_FUNC(MsgType, CallFunc, Value) \
    GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 3, #CallFunc, #Value, Value)
#define GRAPHIC_DFX_MINI_ERR_INFO(MsgType, Value) GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 1, Value)
#define GRAPHIC_DFX_MINI_ERR_VALUE(MsgType, Value) GraphicLogOut(FUNC_TAG, LINE_TAG, MsgType, 2, #Value, Value)
#else
#define GRAPHIC_DFX_MINI_ERR_FUNC(fmt, args...)
#define GRAPHIC_DFX_MINI_ERR_INFO(fmt, args...)
#define GRAPHIC_DFX_MINI_ERR_VALUE(fmt, args...)
#endif
#else
#if 0
#ifdef CONFIG_GFX_DFX_DEBUG
#define GRAPHIC_DFX_DEBUG_FUNC(MsgType)                   \
    {                                                     \
        if (GRAPHIC_DFX_UNF_ENTER_FUNC == MsgType)        \
            HI_UNF_FuncEnter();                           \
        else if (GRAPHIC_DFX_UNF_EXIT_FUNC == MsgType)    \
            HI_UNF_FuncExit();                            \
        else if (GRAPHIC_DFX_INNER_ENTER_FUNC == MsgType) \
            HI_FuncEnter();                               \
        else                                              \
            HI_FuncExit();                                \
    }
#else
#define GRAPHIC_DFX_DEBUG_FUNC(fmt, args...)
#endif

#ifdef CONFIG_GFX_DFX_INFO
#define GRAPHIC_DFX_DEBUG_INFO(MsgType, Value)       \
    {                                                \
        if (GRAPHIC_DFX_DEBUG_INFOMATION == MsgType) \
            HI_DBG_PrintInfo(Value);                 \
    }
#define GRAPHIC_DFX_DEBUG_VALUE(MsgType, Value)      \
    {                                                \
        if (GRAPHIC_DFX_DEBUG_INT == MsgType)        \
            HI_DBG_PrintS32(Value);                  \
        else if (GRAPHIC_DFX_DEBUG_UINT == MsgType)  \
            HI_DBG_PrintU32(Value);                  \
        else if (GRAPHIC_DFX_DEBUG_XINT == MsgType)  \
            HI_DBG_PrintH32(Value);                  \
        else if (GRAPHIC_DFX_DEBUG_LONG == MsgType)  \
            HI_DBG_PrintU64(Value);                  \
        else if (GRAPHIC_DFX_DEBUG_ULONG == MsgType) \
            HI_DBG_PrintU64(Value);                  \
        else if (GRAPHIC_DFX_DEBUG_FLOAT == MsgType) \
            HI_DBG_PrintFloat(Value);                \
        else if (GRAPHIC_DFX_DEBUG_VOID == MsgType)  \
            HI_DBG_PrintVoid(Value);                 \
        else if (GRAPHIC_DFX_DEBUG_STR == MsgType)   \
            HI_DBG_PrintStr(Value);                  \
        else                                         \
            HI_DBG_PrintS32(Value);                  \
    }
#else
#define GRAPHIC_DFX_DEBUG_INFO(fmt, args...)
#define GRAPHIC_DFX_DEBUG_VALUE(fmt, args...)
#endif

#ifdef CONFIG_GFX_DFX_ERR
#define GRAPHIC_DFX_ERR_FUNC(MsgType, CallFunc, Value) \
    {                                                  \
        if (GRAPHIC_DFX_ERR_FUNCTION == MsgType)       \
            HI_ERR_PrintFuncResult(CallFunc, Value);   \
    }
#define GRAPHIC_DFX_ERR_INFO(MsgType, Value)       \
    {                                              \
        if (GRAPHIC_DFX_ERR_INFOMATION == MsgType) \
            HI_ERR_PrintInfo(Value);               \
    }
#define GRAPHIC_DFX_ERR_VALUE(MsgType, Value)      \
    {                                              \
        if (GRAPHIC_DFX_ERR_INT == MsgType)        \
            HI_ERR_PrintS32(Value);                \
        else if (GRAPHIC_DFX_ERR_UINT == MsgType)  \
            HI_ERR_PrintU32(Value);                \
        else if (GRAPHIC_DFX_ERR_XINT == MsgType)  \
            HI_ERR_PrintH32(Value);                \
        else if (GRAPHIC_DFX_ERR_LONG == MsgType)  \
            HI_ERR_PrintU64(Value);                \
        else if (GRAPHIC_DFX_ERR_ULONG == MsgType) \
            HI_ERR_PrintU64(Value);                \
        else if (GRAPHIC_DFX_ERR_FLOAT == MsgType) \
            HI_ERR_PrintFloat(Value);              \
        else if (GRAPHIC_DFX_ERR_VOID == MsgType)  \
            HI_ERR_PrintVoid(Value);               \
        else if (GRAPHIC_DFX_ERR_STR == MsgType)   \
            HI_ERR_PrintStr(Value);                \
        else                                       \
            HI_ERR_PrintS32(Value);                \
    }
#else
#define GRAPHIC_DFX_ERR_FUNC(fmt, args...)
#define GRAPHIC_DFX_ERR_INFO(fmt, args...)
#define GRAPHIC_DFX_ERR_VALUE(fmt, args...)
#endif

#if defined(CONFIG_GFX_DFX_MINI) && defined(CONFIG_GFX_DFX_ERR)
#define GRAPHIC_DFX_MINI_ERR_FUNC(MsgType, CallFunc, Value) \
    {                                                       \
        if (GRAPHIC_DFX_ERR_FUNCTION == MsgType)            \
            HI_ERR_PrintFuncResult(CallFunc, Value);        \
    }
#define GRAPHIC_DFX_MINI_ERR_INFO(MsgType, Value)  \
    {                                              \
        if (GRAPHIC_DFX_ERR_INFOMATION == MsgType) \
            HI_ERR_PrintInfo(Value);               \
    }
#define GRAPHIC_DFX_MINI_ERR_VALUE(MsgType, Value) \
    {                                              \
        if (GRAPHIC_DFX_ERR_INT == MsgType)        \
            HI_ERR_PrintS32(Value);                \
        else if (GRAPHIC_DFX_ERR_UINT == MsgType)  \
            HI_ERR_PrintU32(Value);                \
        else if (GRAPHIC_DFX_ERR_XINT == MsgType)  \
            HI_ERR_PrintH32(Value);                \
        else if (GRAPHIC_DFX_ERR_LONG == MsgType)  \
            HI_ERR_PrintU64(Value);                \
        else if (GRAPHIC_DFX_ERR_ULONG == MsgType) \
            HI_ERR_PrintU64(Value);                \
        else if (GRAPHIC_DFX_ERR_FLOAT == MsgType) \
            HI_ERR_PrintFloat(Value);              \
        else if (GRAPHIC_DFX_ERR_VOID == MsgType)  \
            HI_ERR_PrintVoid(Value);               \
        else if (GRAPHIC_DFX_ERR_STR == MsgType)   \
            HI_ERR_PrintStr(Value);                \
        else                                       \
            HI_ERR_PrintS32(Value);                \
    }
#else
#define GRAPHIC_DFX_MINI_ERR_FUNC(fmt, args...)
#define GRAPHIC_DFX_MINI_ERR_INFO(fmt, args...)
#define GRAPHIC_DFX_MINI_ERR_VALUE(fmt, args...)
#endif
#endif
#endif
/* *============================================================================================
 **END DFX
 **===========================================================================================* */
/* *===============================================================================================
 ** 确保运算不出现反转
 **==============================================================================================* */
#define GRAPHIC_CHECK_U64_ADDITION_REVERSAL_UNRETURN(left_value, right_value) do { \
    if ((0 == (left_value)) || ((left_value) > (ULONG_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);         \
        return;                                                                \
    }                                                                          \
} while (0)

#define GRAPHIC_CHECK_U64_ADDITION_REVERSAL_RETURN(left_value, right_value, ret) do { \
    if ((0 == (left_value)) || ((left_value) > (ULONG_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);         \
        return ret;                                                            \
    }                                                                          \
} while (0)

/* * addition by unsigned int, maybe equal to 0 * */
#define GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_UNRETURN(left_value, right_value) do { \
    if ((0 == (left_value)) || ((left_value) > (UINT_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);        \
        return;                                                               \
    }                                                                         \
} while (0)

#define GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(left_value, right_value, ret) do { \
    if ((0 == (left_value)) || ((left_value) > (UINT_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);        \
        return ret;                                                           \
    }                                                                         \
} while (0)

/* * addition by unsigned int, maybe equal to 0 * */
#define GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_UNRETURN(left_value, right_value) do { \
    unsigned long tmp_value = (unsigned long)(left_value) * (unsigned long)(right_value); \
    if ((0 == tmp_value) || (tmp_value > UINT_MAX)) {                                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_value);                      \
        return;                                                                           \
    }                                                                                     \
} while (0)

#define GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(left_value, right_value, ret) do { \
    unsigned long tmp_value = (unsigned long)(left_value) * (unsigned long)(right_value); \
    if ((0 == tmp_value) || (tmp_value > UINT_MAX)) {                                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_value);                      \
        return ret;                                                                       \
    }                                                                                     \
} while (0)

/* * addition by int, maybe equal to 0 * */
#define GRAPHIC_CHECK_INT_ADDITION_REVERSAL_UNRETURN(left_value, right_value) do { \
    if (((left_value) <= 0) || ((left_value) > (INT_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);        \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);       \
        return;                                                              \
    }                                                                        \
} while (0)

#define GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(left_value, right_value, ret) do { \
    if (((left_value) <= 0) || ((left_value) > (INT_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);        \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);       \
        return ret;                                                          \
    }                                                                        \
} while (0)

/* * addition by char, maybe equal to -128 * */
#define GRAPHIC_CHECK_CHAR_ADDITION_REVERSAL_UNRETURN(left_value, right_value) do { \
    if (((left_value) <= 0) || ((right_value) < 0) || ((left_value) > (SCHAR_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                                 \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);                                \
        return;                                                                                       \
    }                                                                                                 \
} while (0)

#define GRAPHIC_CHECK_CHAR_ADDITION_REVERSAL_RETURN(left_value, right_value, ret) do { \
    if (((left_value) <= 0) || ((right_value) < 0) || ((left_value) > (SCHAR_MAX - (right_value)))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                                 \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);                                \
        return ret;                                                                                   \
    }                                                                                                 \
} while (0)

/* *===============================================================================================
 ** 确保整型转换时不会出现截断错误
 **==============================================================================================* */
/* * long to char * */
#define GRAPHIC_CHECK_SLONG_TO_CHAR_REVERSAL_UNRETURN(right_value) do { \
    if (((right_value) < SCHAR_MIN) || ((right_value) > SCHAR_MAX)) {  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value); \
        return;                                                        \
    }                                                                  \
} while (0)

#define GRAPHIC_CHECK_SLONG_TO_CHAR_REVERSAL_RETURN(right_value, ret) do { \
    if (((right_value) < SCHAR_MIN) || ((right_value) > SCHAR_MAX)) {  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value); \
        return ret;                                                    \
    }                                                                  \
} while (0)

/* * unsigned long to unsigned int * */
#define GRAPHIC_CHECK_ULONG_TO_UINT_REVERSAL_UNRETURN(right_value) do { \
    if ((right_value) > UINT_MAX) {                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value); \
        return;                                                        \
    }                                                                  \
} while (0)

#define GRAPHIC_CHECK_ULONG_TO_UINT_REVERSAL_RETURN(right_value, ret) do { \
    if ((right_value) > UINT_MAX) {                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value); \
        return ret;                                                    \
    }                                                                  \
} while (0)

/* *===============================================================================================
** BEG PARA CHECK
**==============================================================================================* */
#define GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(CurIndex, MaxIndex, Ret) \
    if ((CurIndex) >= (MaxIndex)) {                                    \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CurIndex);         \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, MaxIndex);         \
        return Ret;                                                    \
    }

#define GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(CurIndex, MaxIndex) \
    if ((CurIndex) >= (MaxIndex)) {                                 \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CurIndex);      \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, MaxIndex);      \
        return;                                                     \
    }

#define GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(left_value, right_value, Ret) \
    if ((left_value) > (right_value)) {                                            \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                   \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);                  \
        return Ret;                                                                \
    }

#define GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_NOVALUE(left_value, right_value) \
    if ((left_value) > (right_value)) {                                         \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);               \
        return;                                                                 \
    }

#define GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_VALUE(left_value, right_value, Ret) \
    if ((left_value) < (right_value)) {                                            \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                   \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);                  \
        return Ret;                                                                \
    }

#define GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_NOVALUE(left_value, right_value) \
    if ((left_value) < (right_value)) {                                         \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, left_value);                \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, right_value);               \
        return;                                                                 \
    }

#define GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(para1, para2) \
    if ((para1) == (para2)) {                                       \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para1);         \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para2);         \
        return;                                                     \
    }

#define GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(para1, para2, Ret) \
    if ((para1) == (para2)) {                                          \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para1);            \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para2);            \
        return Ret;                                                    \
    }

#define GRAPHIC_CHECK_LEFT_UNEQUAL_RIGHT_RETURN_NOVALUE(para1, para2) \
    if ((para1) != (para2)) {                                         \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para1);           \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para2);           \
        return;                                                       \
    }

#define GRAPHIC_CHECK_LEFT_UNEQUAL_RIGHT_RETURN_VALUE(para1, para2, Ret) \
    if ((para1) != (para2)) {                                            \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para1);              \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para2);              \
        return Ret;                                                      \
    }

#define GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Pointer, Ret)                      \
    if (NULL == (Pointer)) {                                                      \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, Pointer);                \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "pointer is null"); \
        return Ret;                                                               \
    }

#define GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(Pointer)                         \
    if (NULL == (Pointer)) {                                                      \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, Pointer);                \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "pointer is null"); \
        return;                                                                   \
    }

#define GRAPHIC_CHECK_EQUAL_ZERO_RETURN_VALUE(para, ret)   \
    if (0 == para) {                                       \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para); \
        return ret;                                        \
    }

#define GRAPHIC_CHECK_EQUAL_ZERO_RETURN_NOVALUE(para)      \
    if (0 == para) {                                       \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, para); \
        return;                                            \
    }
/* *===============================================================================================
** END PARA CHECK
**==============================================================================================* */
/* *===============================================================================================
** 数组合法性检查
**==============================================================================================* */
#define HI_GFX_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

/* *===============================================================================================
** 时间函数封装
**==============================================================================================* */
#define HI_GFX_TINIT() osal_timeval tv_cur
#define HI_GFX_TSTART(StartTimes) \
    osal_get_timeofday(&tv_cur);  \
    StartTimes = tv_cur.tv_sec * 1000 + tv_cur.tv_usec / 1000
#define HI_GFX_TEND(EndTimes)    \
    osal_get_timeofday(&tv_cur); \
    EndTimes = tv_cur.tv_sec * 1000 + tv_cur.tv_usec / 1000

/* ************************** Structure Definition ******************************************** */

/* ************************** Enum Definition ************************************************* */
typedef enum tagGfxMsgType {
    GRAPHIC_DFX_ERR_FUNCTION = 0,
    GRAPHIC_DFX_ERR_INT,
    GRAPHIC_DFX_ERR_UINT,
    GRAPHIC_DFX_ERR_XINT,
    GRAPHIC_DFX_ERR_LONG,
    GRAPHIC_DFX_ERR_ULONG,
    GRAPHIC_DFX_ERR_FLOAT,
    GRAPHIC_DFX_ERR_VOID,
    GRAPHIC_DFX_ERR_STR,
    GRAPHIC_DFX_ERR_INFOMATION,
    GRAPHIC_DFX_BEG,
    GRAPHIC_DFX_END,
    GRAPHIC_DFX_UNF_ENTER_FUNC,
    GRAPHIC_DFX_UNF_EXIT_FUNC,
    GRAPHIC_DFX_INNER_ENTER_FUNC,
    GRAPHIC_DFX_INNER_EXIT_FUNC,
    GRAPHIC_DFX_DEBUG_INT,
    GRAPHIC_DFX_DEBUG_UINT,
    GRAPHIC_DFX_DEBUG_XINT,
    GRAPHIC_DFX_DEBUG_LONG,
    GRAPHIC_DFX_DEBUG_ULONG,
    GRAPHIC_DFX_DEBUG_FLOAT,
    GRAPHIC_DFX_DEBUG_VOID,
    GRAPHIC_DFX_DEBUG_STR,
    GRAPHIC_DFX_DEBUG_INFOMATION,
    GRAPHIC_DFX_BUTT,
} GRAPHIC_DFX_TYPE_E;
/* ********************* Global Variable declaration ****************************************** */

/* ****************************** API declaration ********************************************* */

/**********************************************************************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* _HI_GFX_SYS_K_H_ */
