/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2009-12-17
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include "hi_debug.h"
#include "hi_type.h"
#include "hi_osal.h"

#include "hi_drv_dev.h"
#include "hi_drv_log.h"
#include "drv_common_ioctl.h"
#include "hi_drv_module.h"
#include "drv_log.h"

#define MAX_MODULE_ID       256

typedef struct {
    hi_u32      id;
    hi_u32      count;
    hi_char     name[MODULE_NAME_MAX_LEN];
    hi_void     *func;
    struct file *file;
} module_detail_info;

static module_detail_info g_modules[MAX_MODULE_ID];

static DEFINE_SEMAPHORE(g_module_mutex);

#define module_lock(ret) do {                               \
    hi_s32 lock_ret = down_interruptible(&g_module_mutex);  \
    if (lock_ret != 0) {                                    \
        return ret;                                         \
    }                                                       \
} while (0)

#define module_unlock() do { \
    up(&g_module_mutex);     \
} while (0)

#ifdef HI_PROC_SUPPORT
#define MODULE_PROC_NAME    "module"
#define SPLIT_LINE          "--------------------------------------------------------\n"

static hi_s32 module_read_proc(hi_void *s, hi_void *arg)
{
    hi_u32 i;

    osal_proc_print(s, SPLIT_LINE);
    osal_proc_print(s, "| Module Name      |  ID  | Log Level |\n");
    osal_proc_print(s, SPLIT_LINE);

    module_lock(HI_FAILURE);

    for (i = 0; i < MAX_MODULE_ID; i++) {
        if (g_modules[i].count != 0) {
            osal_proc_print(s, "| %-16.16s | 0x%-2x |\n", g_modules[i].name, g_modules[i].id);
        }
    }

    module_unlock();

    osal_proc_print(s, SPLIT_LINE);

    return 0;
}

hi_s32 drv_module_add_proc(hi_void)
{
    osal_proc_entry *item = HI_NULL;

    item = osal_proc_add(MODULE_PROC_NAME, strlen(MODULE_PROC_NAME));
    if (item == HI_NULL) {
        HI_ERR_SYS("add proc failed!\n");
        return HI_FAILURE;
    }

    item->read = module_read_proc;

    return HI_SUCCESS;
}

hi_void drv_module_remove_proc(hi_void)
{
    osal_proc_remove(MODULE_PROC_NAME, strlen(MODULE_PROC_NAME));
}
#endif

hi_s32 drv_module_init(hi_void)
{
    memset(g_modules, 0, sizeof(g_modules));

    return HI_SUCCESS;
}

hi_void drv_module_exit(hi_void)
{
    memset(g_modules, 0, sizeof(g_modules));
}

hi_s32 drv_module_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case MODULE_ADD_INFO: {
            module_info *info = (module_info*)arg;

            ret = hi_drv_module_register(info->module_id, info->module_name, HI_NULL, HI_NULL);

            break;
        }

        case MODULE_DEL_INFO: {
            hi_u32 module_id = *((hi_u32*)arg);

            ret = hi_drv_module_unregister(module_id);

            break;
        }

        default: {
            HI_ERR_SYS("unknown command 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

hi_s32 hi_drv_module_register(const hi_u32 module_id, const hi_char *name, hi_void *func, struct file *file)
{
    hi_s32 ret;
    hi_u32 len;
    hi_bool is_first_register = HI_FALSE;

    if (module_id >= MAX_MODULE_ID) {
        HI_ERR_SYS("module id(0x%x) is error\n", module_id);
        return HI_FAILURE;
    }

    if (name == HI_NULL) {
        HI_ERR_SYS("name is null\n");
        return HI_FAILURE;
    }

    len = strlen(name);
    if ((len == 0) || (len >= MODULE_NAME_MAX_LEN)) {
        HI_ERR_SYS("%s length is error\n", name);
        return HI_FAILURE;
    }

    module_lock(HI_FAILURE);

    if (g_modules[module_id].count == 0) {
        g_modules[module_id].id = module_id;
        g_modules[module_id].func = func;
        g_modules[module_id].file = file;
        strncpy(g_modules[module_id].name, name, MODULE_NAME_MAX_LEN);
        is_first_register = HI_TRUE;
    }
    g_modules[module_id].count++;

    module_unlock();

    ret = HI_SUCCESS;

#ifdef HI_LOG_SUPPORT
    if (is_first_register == HI_TRUE) {
        ret = hi_drv_log_add_module(name, module_id);
    }
#endif

    return ret;
}

hi_s32 hi_drv_module_unregister(const hi_u32 module_id)
{
    hi_s32 ret = HI_FAILURE;
    hi_bool is_last_unregister = HI_FALSE;

    if (module_id >= MAX_MODULE_ID) {
        HI_ERR_SYS("module id(0x%x) is error\n", module_id);
        return HI_FAILURE;
    }

    module_lock(HI_FAILURE);

    if (g_modules[module_id].count != 0) {
        if (g_modules[module_id].count == 1) {
            memset(&g_modules[module_id], 0, sizeof(module_detail_info));
            is_last_unregister = HI_TRUE;
        } else {
            g_modules[module_id].count--;
        }
        ret = HI_SUCCESS;
    }

    module_unlock();

#ifdef HI_LOG_SUPPORT
    if (is_last_unregister == HI_TRUE) {
        hi_drv_log_remove_module(module_id);
    }
#endif

    return ret;
}

hi_s32 hi_drv_module_get_func(const hi_u32 module_id, hi_void **func)
{
    hi_s32 ret = HI_FAILURE;

    if (module_id >= MAX_MODULE_ID) {
        HI_ERR_SYS("module id(0x%x) is error\n", module_id);
        return HI_FAILURE;
    }

    if (func == HI_NULL) {
        HI_ERR_SYS("func is null\n", module_id);
        return HI_FAILURE;
    }

    module_lock(HI_FAILURE);

    if (g_modules[module_id].count != 0) {
        *func = g_modules[module_id].func;
        ret = HI_SUCCESS;
    }

    module_unlock();

    return ret;
}

EXPORT_SYMBOL(hi_drv_module_register);
EXPORT_SYMBOL(hi_drv_module_unregister);
EXPORT_SYMBOL(hi_drv_module_get_func);

