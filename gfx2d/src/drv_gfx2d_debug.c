/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d debug
 * Author: sdk
 * Create: 2019-05-13
 */

#include <linux/hisilicon/securec.h>
#include "hi_type.h"
#include "hi_osal.h"

/***************************** Macro Definition ***************************************************/

#define MSG_LENGTH_MAX 256

/* ********************* Global Variable declaration ********************************************** */
static hi_char g_log_func[MSG_LENGTH_MAX] = "all";
static hi_u32 g_log_level = 1;
static hi_u32 g_log_save = 0;

#include "drv_gfx2d_debug.h"
#include "hi_gfx_debug.h"

/* ****************************** API relese ****************************************************** */
#ifndef CONFIG_USE_SDK_LOG
hi_void DRV_GFX2D_OutputMessage(const hi_char *pFunc, const hi_u32 Line, const GRAPHIC_DFX_TYPE_E eMsgType,
                                const hi_u32 Format, ...)
{
    va_list args;
    hi_u32 MsgLen = 0;
    hi_bool LogErr = HI_FALSE;
    hi_bool LogFunc = HI_FALSE;
    hi_bool LogValue = HI_FALSE;
    hi_char LogStr[MSG_LENGTH_MAX + 1] = {'\0'};
    hi_char TmpFormat[MSG_LENGTH_MAX + 1] = {'\0'};
    drv_gfx_format_msg format_msg;

    if (pFunc == NULL) {
        GRAPHIC_COMM_PRINT("[module-gfx-gfx2d][err] : input func null pointer.\n");
        return;
    }

    drv_gfx_fill_format_param(format_msg, "gfx2d", pFunc, Line, eMsgType, TmpFormat, MSG_LENGTH_MAX + 1,
        &LogErr, &LogFunc, &LogValue);
    if ((eMsgType == GRAPHIC_DFX_BEG) || (eMsgType == GRAPHIC_DFX_END)) {
        drv_gfx_format_message(&format_msg);
        return;
    }

    /* 0 is that the first 3 characters of g_log_func is the same as all */
    if (osal_strncmp(g_log_func, 3, "all", 3) == 0) {
        drv_gfx_format_message(&format_msg);
        if ((eMsgType == GRAPHIC_DFX_UNF_ENTER_FUNC) || (eMsgType == GRAPHIC_DFX_UNF_EXIT_FUNC) ||
            (eMsgType == GRAPHIC_DFX_INNER_ENTER_FUNC) || (eMsgType == GRAPHIC_DFX_INNER_EXIT_FUNC)) {
            return;
        }
        goto OUT_MSG;
    }

    if (0 == osal_strncmp(g_log_func, strlen(pFunc), pFunc, strlen(pFunc))) {
        drv_gfx_format_message(&format_msg);
        if ((eMsgType == GRAPHIC_DFX_UNF_ENTER_FUNC) || (eMsgType == GRAPHIC_DFX_UNF_EXIT_FUNC) ||
            (eMsgType == GRAPHIC_DFX_INNER_ENTER_FUNC) || (eMsgType == GRAPHIC_DFX_INNER_EXIT_FUNC)) {
            return;
        }
        goto OUT_MSG;
    }

    return;

OUT_MSG:

    /* the follows debug dispose should care performance, if not open debug level, not need format strings * */
    if (g_log_level == 0) { /* 0 is level of g_log_level */
        return;
    }

    if ((g_log_level == 1) && (LogErr == HI_FALSE)) { /* 1 is level of g_log_level */
        return;
    }

    if ((g_log_level == 2) && (LogFunc == HI_FALSE)) { /* 2 is level of g_log_level */
        return;
    }

    if ((g_log_level == 3) && (LogValue == HI_FALSE)) { /* 3 is level of g_log_level */
        return;
    }

    TmpFormat[MSG_LENGTH_MAX] = '\0';

    if (Format <= 3) {          /* 3 is the max number of Variable Parameters */
        va_start(args, Format); /* The first variable's first address after getting format  */
        MsgLen = vsnprintf(LogStr, MSG_LENGTH_MAX, TmpFormat, args);
        va_end(args);

        // HI_UNUSED(MsgLen);
        LogStr[MSG_LENGTH_MAX] = '\0';
        drv_gfx_output_message(LogStr, eMsgType);
    }

    return;
}
#endif

hi_void DRV_GFX2D_SetLogFunc(hi_char *pFunc)
{
    if (pFunc != NULL) {
        if (strncpy_s(g_log_func, sizeof(g_log_func), pFunc + 1, sizeof(g_log_func) - 1) != EOK) {
            return;
        }
        g_log_func[sizeof(g_log_func) - 1] = '\0';
    }
}

hi_void DRV_GFX2D_SetLogLevel(hi_u32 LogLevel)
{
    g_log_level = LogLevel;
}

hi_void DRV_GFX2D_SetLogSave(hi_u32 LogSave)
{
    g_log_save = LogSave;
}

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void DRV_GFX2D_ProcMsg(struct seq_file *p)
{
    if (p != NULL) {
        osal_proc_print(p, "++++++++++++++++++++++++++++ debug gfx2d msg ++++++++++++++++++++++\n");
        osal_proc_print(p, "gfx2d log level\t: %u\n", g_log_level);
        osal_proc_print(p, "gfx2d log save\t: %u\n", g_log_save);
        osal_proc_print(p, "gfx2d log func\t: %s\n", g_log_func);
        osal_proc_print(p, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
}
#endif
