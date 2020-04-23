/**************************************************************************************************
*
* Copyright (C) 2018 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
***************************************************************************************************
File Name       : hi_gfx_debug.h
Version         : Initial Draft
Author          :
Created         : 2018/01/01
Description     : this file only use by drv_mod_debug.c
Function List   :


History         :
Date                    Author                Modification
2018/01/01              sdk                   Created file
***************************************************************************************************/
#ifndef __HI_GFX_DEBUG_H__
#define __HI_GFX_DEBUG_H__

#include <linux/hisilicon/securec.h>
#include "hi_type.h"

/**************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************** */
/* ************************** Structure Definition ************************************************ */

/* ********************* Global Variable declaration ********************************************** */

/* ****************************** API declaration ************************************************* */
#ifndef CONFIG_USE_SDK_LOG
typedef struct {
    hi_char *mod;
    const hi_char *func;
    hi_u32 line;
    GRAPHIC_DFX_TYPE_E msg_type;
    hi_char *format;
    hi_bool *log_err;
    hi_bool *log_func;
    hi_bool *log_value;
} drv_gfx_format_msg;

#define drv_gfx_fill_format_param(format_msg, model, fun, lines, dfx_type, form, form_len, err, log_fun, log_val) do {\
    (format_msg).mod = model; \
    (format_msg).func = fun;\
    (format_msg).line = lines; \
    (format_msg).msg_type = dfx_type; \
    (format_msg).format = form; \
    (format_msg).log_err = err; \
    (format_msg).log_func = log_fun; \
    (format_msg).log_value = log_val; \
} while (0)

static hi_void OutputMessageToFile(const hi_char* pMsg)
{
    return;
}

static hi_void drv_gfx_output_message(const hi_char *pMsg, GRAPHIC_DFX_TYPE_E eMsgType)
{
    switch (eMsgType) {
        case GRAPHIC_DFX_ERR_FUNCTION:
        case GRAPHIC_DFX_ERR_INT:
        case GRAPHIC_DFX_ERR_UINT:
        case GRAPHIC_DFX_ERR_XINT:
        case GRAPHIC_DFX_ERR_LONG:
        case GRAPHIC_DFX_ERR_ULONG:
        case GRAPHIC_DFX_ERR_FLOAT:
        case GRAPHIC_DFX_ERR_VOID:
        case GRAPHIC_DFX_ERR_STR:
        case GRAPHIC_DFX_ERR_INFOMATION:
            if ((1 == g_log_level) || (4 == g_log_level)) {
                if (0 == g_log_save) {
                    GRAPHIC_COMM_PRINT("%s", pMsg);
                }
                if (1 == g_log_save) {
                    OutputMessageToFile(pMsg);
                }
            }
            break;
        case GRAPHIC_DFX_BEG:
        case GRAPHIC_DFX_END:
            if ((g_log_level > 1) && (g_log_level < 5)) {
                if (0 == g_log_save) {
                    GRAPHIC_COMM_PRINT("%s", pMsg);
                }
                if (1 == g_log_save) {
                    OutputMessageToFile(pMsg);
                }
            }
            break;
        case GRAPHIC_DFX_UNF_ENTER_FUNC:
        case GRAPHIC_DFX_UNF_EXIT_FUNC:
        case GRAPHIC_DFX_INNER_ENTER_FUNC:
        case GRAPHIC_DFX_INNER_EXIT_FUNC:
            if ((2 == g_log_level) || (4 == g_log_level)) {
                if (0 == g_log_save) {
                    GRAPHIC_COMM_PRINT("%s", pMsg);
                }
                if (1 == g_log_save) {
                    OutputMessageToFile(pMsg);
                }
            }
            break;
        case GRAPHIC_DFX_DEBUG_INT:
        case GRAPHIC_DFX_DEBUG_UINT:
        case GRAPHIC_DFX_DEBUG_XINT:
        case GRAPHIC_DFX_DEBUG_LONG:
        case GRAPHIC_DFX_DEBUG_ULONG:
        case GRAPHIC_DFX_DEBUG_FLOAT:
        case GRAPHIC_DFX_DEBUG_VOID:
        case GRAPHIC_DFX_DEBUG_STR:
        case GRAPHIC_DFX_DEBUG_INFOMATION:
            if ((3 == g_log_level) || (4 == g_log_level)) {
                if (0 == g_log_save) {
                    GRAPHIC_COMM_PRINT("%s", pMsg);
                }
                if (1 == g_log_save) {
                    OutputMessageToFile(pMsg);
                }
            }
            break;
        default:
            break;
    }

    return;
}

static hi_void drv_gfx_format_message(drv_gfx_format_msg *format_msg)
{
    hi_char LogStr[MSG_LENGTH_MAX + 1] = {'\0'};

    if ((g_log_level == 0) || (format_msg == HI_NULL)) {
        return;
    }

    *(format_msg->log_err) = HI_TRUE;
    switch (format_msg->msg_type)
    {
        case GRAPHIC_DFX_ERR_FUNCTION:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "call %s -> return %s = [0x%08X]");
             return;
        case GRAPHIC_DFX_ERR_INT:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "%s = %d");
             return;
        case GRAPHIC_DFX_ERR_UINT:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "%s = %u");
             return;
        case GRAPHIC_DFX_ERR_XINT:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "%s = 0x%X");
             return;
        case GRAPHIC_DFX_ERR_LONG:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "%s = %lld");
             return;
        case GRAPHIC_DFX_ERR_ULONG:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "%s = %llu");
             return;
        case GRAPHIC_DFX_ERR_VOID:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "%s = %p");
             return;
        case GRAPHIC_DFX_ERR_STR:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "%s = %s");
             return;
        case GRAPHIC_DFX_ERR_INFOMATION:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][err] : %s[%d] -> %s\n", format_msg->mod,
                 format_msg->func, format_msg->line, "<%s>");
             return;
        default:
             break;
    }
    *(format_msg->log_err) = HI_FALSE;

    /* the follows debug dispose should care performance, if not open debug level, not need format strings */
    if (1 == g_log_level) {
        return;
    }

    *(format_msg->log_func) = HI_TRUE;
    switch (format_msg->msg_type)
    {
        case GRAPHIC_DFX_BEG:
             snprintf(LogStr, MSG_LENGTH_MAX, "[module-gfx-%s-beg][line] :=====================================\n",
                 format_msg->mod);
             LogStr[MSG_LENGTH_MAX] = '\0';
             return drv_gfx_output_message(LogStr, format_msg->msg_type);
        case GRAPHIC_DFX_END:
             snprintf(LogStr, MSG_LENGTH_MAX, "[module-gfx-%s-end][line] :=====================================\n",
                 format_msg->mod);
             LogStr[MSG_LENGTH_MAX] = '\0';
             return drv_gfx_output_message(LogStr, format_msg->msg_type);
        default:
             break;
    }

    switch (format_msg->msg_type)
    {
        case GRAPHIC_DFX_UNF_ENTER_FUNC:
             snprintf(LogStr, MSG_LENGTH_MAX, "[module-gfx-%s][func] : %s[%d] : >>>>>>>>>>>>>>>>[enter]\n",
                 format_msg->mod, format_msg->func, format_msg->line);
             LogStr[MSG_LENGTH_MAX] = '\0';
             return drv_gfx_output_message(LogStr, format_msg->msg_type);
        case GRAPHIC_DFX_UNF_EXIT_FUNC:
             snprintf(LogStr, MSG_LENGTH_MAX, "[module-gfx-%s][func] : %s[%d] : >>>>>>>>>>>>>>>>[exit]\n",
                 format_msg->mod, format_msg->func, format_msg->line);
             LogStr[MSG_LENGTH_MAX] = '\0';
             return drv_gfx_output_message(LogStr, format_msg->msg_type);
        case GRAPHIC_DFX_INNER_ENTER_FUNC:
             snprintf(LogStr, MSG_LENGTH_MAX, "[module-gfx-%s][func] : %s[%d] : ===============>[enter]\n",
                 format_msg->mod, format_msg->func, format_msg->line);
             LogStr[MSG_LENGTH_MAX] = '\0';
             return drv_gfx_output_message(LogStr, format_msg->msg_type);
        case GRAPHIC_DFX_INNER_EXIT_FUNC:
             snprintf(LogStr, MSG_LENGTH_MAX, "[module-gfx-%s][func] : %s[%d] : ===============>[exit]\n",
                 format_msg->mod, format_msg->func, format_msg->line);
             LogStr[MSG_LENGTH_MAX] = '\0';
             return drv_gfx_output_message(LogStr, format_msg->msg_type);
        default:
             break;
    }
    *(format_msg->log_func) = HI_FALSE;

    if (2 == g_log_level) {
        return;
    }

    *(format_msg->log_value) = HI_TRUE;
    switch (format_msg->msg_type)
    {
        case GRAPHIC_DFX_DEBUG_INT:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "%s = %d");
             return;
        case GRAPHIC_DFX_DEBUG_UINT:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "%s = %u");
             return;
        case GRAPHIC_DFX_DEBUG_XINT:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "%s = 0x%X");
             return;
        case GRAPHIC_DFX_DEBUG_LONG:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "%s = %lld");
             return;
        case GRAPHIC_DFX_DEBUG_ULONG:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "%s = %llu");
             return;
        case GRAPHIC_DFX_DEBUG_VOID:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "%s = %p");
             return;
        case GRAPHIC_DFX_DEBUG_STR:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "%s = %s");
             return;
        case GRAPHIC_DFX_DEBUG_INFOMATION:
             snprintf(format_msg->format, MSG_LENGTH_MAX, "[module-gfx-%s][debug] : %s[%d] -> %s\n",
                 format_msg->mod, format_msg->func, format_msg->line, "<%s>");
             return;
        default:
             break;
    }
    *(format_msg->log_value) = HI_FALSE;

    return;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* *__HI_GFX_DEBUG_H__* */
