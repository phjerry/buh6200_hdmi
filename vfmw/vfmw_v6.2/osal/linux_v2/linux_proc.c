/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vfmw_osal.h"
#include "linux_proc.h"
#include "linux_osal.h"
#include "dbg.h"

#define PROC_BUFFER_SIZE (512)

hi_s32 linux_proc_create(hi_u8 *proc_name, proc_read_fn read, hi_void *cmd_list, hi_u32 cmd_cnt)
{
    osal_proc_entry *entry = HI_NULL;
    OS_PROC_CMD *cmd = (OS_PROC_CMD *)cmd_list;

    if (proc_name == HI_NULL) {
        return -1;
    }

    entry = osal_proc_add(proc_name, strlen(proc_name));
    if (entry == HI_NULL) {
        OS_PRINT("Create %s proc entry fail!\n", proc_name);
        return -1;
    }

    /* Set functions */
    entry->read = read;
    entry->cmd_list = cmd;
    entry->cmd_cnt = cmd_cnt;

    return 0;
}

hi_void linux_proc_destroy(hi_u8 *proc_name)
{
    if (proc_name == HI_NULL) {
        return;
    }

    osal_proc_remove(proc_name, strlen(proc_name));
}

hi_void linux_proc_dump(hi_void *page, hi_s32 page_count, hi_s32 *used_bytes, hi_s8 from_shr,
                        const hi_s8 *format, ...)
{
    OS_VA_LIST arg_list;
    hi_s32 total_char;
    hi_s8 str[PROC_BUFFER_SIZE];

    if (from_shr != 0) {
        osal_proc_print(page, format);
        return;
    }

    OS_VA_START(arg_list, format);
    total_char = vsnprintf_s(str, PROC_BUFFER_SIZE, PROC_BUFFER_SIZE - 1, format, arg_list);
    if (total_char < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    OS_VA_END(arg_list);

    osal_proc_print(page, "%s", str);

    return;
}

hi_s32 linux_proc_init(hi_void)
{
#ifdef VFMW_PROC_SUPPORT
/* g_pProcEntry = proc_mkdir("vfmw", HI_NULL); */
#endif

    return 0;
}

hi_void linux_proc_exit(hi_void)
{
#ifdef VFMW_PROC_SUPPORT
/* remove_proc_entry("vfmw", HI_NULL); */
#endif
}


