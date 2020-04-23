/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb debug
 * Author: sdk
 * Create: 2016-01-01
 */

#include "hi_osal.h"
#include "hi_type.h"

#define MSG_LENGTH_MAX 256

/* ************************** Structure Definition ************************************************ */

/* ********************* Global Variable declaration ********************************************** */
static hi_char g_log_func[MSG_LENGTH_MAX] = "all";
static hi_u32 g_log_level = 1;
static hi_u32 g_log_save = 0;

#include "drv_hifb_debug.h"
#include "hi_gfx_debug.h"
/* ****************************** API declaration ************************************************* */

/* ****************************** API relese ****************************************************** */
#ifndef CONFIG_USE_SDK_LOG
hi_void DRV_HIFB_OutputMessage(const hi_char *pFunc, const hi_u32 Line, const GRAPHIC_DFX_TYPE_E eMsgType,
                               const hi_u32 Format, ...)
{
    va_list args = {0};
    hi_u32 MsgLen = 0;
    hi_bool LogErr = HI_FALSE;
    hi_bool LogFunc = HI_FALSE;
    hi_bool LogValue = HI_FALSE;
    hi_char LogStr[MSG_LENGTH_MAX + 1] = {'\0'};
    hi_char TmpFormat[MSG_LENGTH_MAX + 1] = {'\0'};
    drv_gfx_format_msg format_msg;

    if (NULL == pFunc) {
        GRAPHIC_COMM_PRINT("[module-gfx-hifb][err] : input func pointer %p\n", pFunc);
        return;
    }

    drv_gfx_fill_format_param(format_msg, "hifb", pFunc, Line, eMsgType, TmpFormat, MSG_LENGTH_MAX + 1,
        &LogErr, &LogFunc, &LogValue);
    if ((GRAPHIC_DFX_BEG == eMsgType) || (GRAPHIC_DFX_END == eMsgType)) {
        drv_gfx_format_message(&format_msg);
        return;
    }

    if (0 == osal_strncmp(g_log_func, 3, "all", 3)) {
        drv_gfx_format_message(&format_msg);
        if ((GRAPHIC_DFX_UNF_ENTER_FUNC == eMsgType) || (GRAPHIC_DFX_UNF_EXIT_FUNC == eMsgType) ||
            (GRAPHIC_DFX_INNER_ENTER_FUNC == eMsgType) || (GRAPHIC_DFX_INNER_EXIT_FUNC == eMsgType)) {
            return;
        }
        goto OUT_MSG;
    }

    if (0 == osal_strncmp(g_log_func, strlen(pFunc), pFunc, strlen(pFunc))) {
        drv_gfx_format_message(&format_msg);
        if ((GRAPHIC_DFX_UNF_ENTER_FUNC == eMsgType) || (GRAPHIC_DFX_UNF_EXIT_FUNC == eMsgType) ||
            (GRAPHIC_DFX_INNER_ENTER_FUNC == eMsgType) || (GRAPHIC_DFX_INNER_EXIT_FUNC == eMsgType)) {
            return;
        }
        goto OUT_MSG;
    }

    return;

OUT_MSG:

    /* * the follows debug dispose should care performance, if not open debug level, not need format strings * */
    /* * 底下debug处理需要注意对性能的影响，级别没有打开不需要处理，这里不单单考虑宏开关控制，这样对性能影响最小* */
    if (0 == g_log_level) {
        return;
    }

    if ((1 == g_log_level) && (HI_FALSE == LogErr)) {
        return;
    }

    if ((2 == g_log_level) && (HI_FALSE == LogFunc)) {
        return;
    }

    if ((3 == g_log_level) && (HI_FALSE == LogValue)) {
        return;
    }

    TmpFormat[MSG_LENGTH_MAX] = '\0';

    if (Format <= 3) {
        va_start(args, Format); /* *获取format之后第一个变参的首地址* */
        MsgLen = vsnprintf(LogStr, MSG_LENGTH_MAX, TmpFormat, args);
        va_end(args);

        LogStr[MSG_LENGTH_MAX] = '\0';
        drv_gfx_output_message(LogStr, eMsgType);
    }

    return;
}
#endif

hi_void DRV_HIFB_SetLogFunc(hi_char *pFunc)
{
    if (pFunc != NULL) {
        strncpy(g_log_func, pFunc + 1, sizeof(g_log_func) - 1);
        g_log_func[sizeof(g_log_func) - 1] = '\0';
    }
}

hi_void DRV_HIFB_SetLogLevel(hi_u32 LogLevel)
{
    g_log_level = LogLevel;
}

hi_void DRV_HIFB_SetLogSave(hi_u32 LogSave)
{
    g_log_save = LogSave;
}

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void DRV_HIFB_ProcMsg(hi_void *p)
{
    if (p != NULL) {
        osal_proc_print(p, "++++++++++++++++++++++++++++ debug hifb msg +++++++++++++++++++++++\n");
        osal_proc_print(p, "hifb log level\t: %u\n",   g_log_level);
        osal_proc_print(p, "hifb log save\t: %u\n",    g_log_save);
        osal_proc_print(p, "hifb log func\t: %s\n",    g_log_func);
        osal_proc_print(p, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
}
#endif
