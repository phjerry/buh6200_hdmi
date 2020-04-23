/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: debug manage
 * Author: sdk
 * Create: 2019-03-18
 */

#include <linux/hisilicon/securec.h>
#include "hi_type.h"
#include "hi_osal.h"

#define TDE_LOG_ERR 1
#define TDE_LOG_FUNC 2
#define TDE_LOG_VALUE 3
#define MSG_LENGTH_MAX 256

static hi_char g_log_func[MSG_LENGTH_MAX] = "all";
static hi_u32 g_log_level = 1;
static hi_u32 g_log_save = 0;

#include "tde_debug.h"
#include "hi_gfx_debug.h"

#ifndef CONFIG_USE_SDK_LOG
static hi_bool is_need_out_msg(hi_bool log_err, hi_bool log_func, hi_bool log_value)
{
    if (g_log_level == 0) {
        return HI_FALSE;
    }

    if ((g_log_level == TDE_LOG_ERR) && (log_err == HI_FALSE)) {
        return HI_FALSE;
    }

    if ((g_log_level == TDE_LOG_FUNC) && (log_func == HI_FALSE)) {
        return HI_FALSE;
    }

    if ((g_log_level == TDE_LOG_VALUE) && (log_value == HI_FALSE)) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_void tde_out_msg(const hi_char *func, const hi_u32 line, const GRAPHIC_DFX_TYPE_E msg_type, const hi_u32 format, ...)
{
    va_list args;
    hi_u32 msg_len = 0;
    hi_bool need_out;
    hi_bool log_err = HI_FALSE;
    hi_bool log_func = HI_FALSE;
    hi_bool log_value = HI_FALSE;
    hi_char log_str[MSG_LENGTH_MAX + 1] = {'\0'};
    hi_char tmp_format[MSG_LENGTH_MAX + 1] = {'\0'};
    drv_gfx_format_msg format_msg;

    if (func == NULL) {
        GRAPHIC_COMM_PRINT("[module-gfx-tde][err] : input func pointer null\n");
        return;
    }

    drv_gfx_fill_format_param(format_msg, "tde", func, line, msg_type, tmp_format, MSG_LENGTH_MAX + 1,
        &log_err, &log_func, &log_value);
    if ((msg_type == GRAPHIC_DFX_BEG) || (msg_type == GRAPHIC_DFX_END)) {
        drv_gfx_format_message(&format_msg);
        return;
    }

    if (osal_strncmp(g_log_func, strlen("all"), "all", strlen("all")) == 0) {
        drv_gfx_format_message(&format_msg);
        if ((msg_type == GRAPHIC_DFX_UNF_ENTER_FUNC) || (msg_type == GRAPHIC_DFX_UNF_EXIT_FUNC) ||
            (msg_type == GRAPHIC_DFX_INNER_ENTER_FUNC) || (msg_type == GRAPHIC_DFX_INNER_EXIT_FUNC)) {
            return;
        }
        goto OUT_MSG;
    }

    if (osal_strncmp(g_log_func, strlen(func), func, strlen(func)) == 0) {
        drv_gfx_format_message(&format_msg);
        if ((msg_type == GRAPHIC_DFX_UNF_ENTER_FUNC) || (msg_type == GRAPHIC_DFX_UNF_EXIT_FUNC) ||
            (msg_type == GRAPHIC_DFX_INNER_ENTER_FUNC) || (msg_type == GRAPHIC_DFX_INNER_EXIT_FUNC)) {
            return;
        }
        goto OUT_MSG;
    }

    return;
OUT_MSG:
    need_out = is_need_out_msg(log_err, log_func, log_value);
    if (need_out == HI_FALSE) {
        return;
    }

    tmp_format[MSG_LENGTH_MAX] = '\0';
    if (format <= TDE_LOG_VALUE) {
        va_start(args, format); /* 获取format之后第一个变参的首地址 */
        msg_len = vsnprintf(log_str, MSG_LENGTH_MAX, tmp_format, args);
        va_end(args);

        // HI_UNUSED(msg_len);
        log_str[MSG_LENGTH_MAX] = '\0';
        drv_gfx_output_message(log_str, msg_type);
    }

    return;
}
#endif

hi_void tde_set_log_func(hi_char *func)
{
    if (func != NULL) {
        if (strncpy_s(g_log_func, sizeof(g_log_func), func + 1, sizeof(g_log_func) - 1) != EOK) {
            return;
        }
        g_log_func[sizeof(g_log_func) - 1] = '\0';
    }
}

hi_void tde_set_log_level(hi_u32 log_level)
{
    g_log_level = log_level;
}

hi_void tde_set_log_save(hi_u32 log_save)
{
    g_log_save = log_save;
}

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void tde_out_set_log_info(struct seq_file *p)
{
    if (p != NULL) {
        osal_proc_print(p, "++++++++++++++++++++++++++++ debug tde msg ++++++++++++++++++++++++\n");
        osal_proc_print(p, "tde log level\t: %u\n", g_log_level);
        osal_proc_print(p, "tde log save\t: %u\n", g_log_save);
        osal_proc_print(p, "tde log func\t: %s\n", g_log_func);
        osal_proc_print(p, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
}
#endif
