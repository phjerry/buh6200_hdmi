/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm_proc
 * Author: sm_rm
 * Create: 2015/11/25
 */

#include "securec.h"
#include "hi_osal.h"

#include "hi_drv_win.h"
#include "hi_drv_rm.h"
#include "drv_rm.h"
#include "drv_rm_define.h"
#include "drv_rm_proc.h"
#include "drv_rm_version.h"

/********************************************************************************/
/* Array                                                                        */
/********************************************************************************/
hi_char *g_wind_owner[] = {
    "DTV",
    "ATV",
    "MEDIA",
    "BUTT",
};

hi_char *g_wind_event[] = {
    "LACK",
    "CREATED",
    "DESTROYED",
    "BUTT",
};

/********************************************************************************/
/* Private API                                                                  */
/********************************************************************************/
static hi_void rm_drv_proc_help(hi_void)
{
    osal_printk("----------------------rm debug option--------------------\n");
}

hi_s32 rm_drv_proc_parse_para(hi_char *proc_para_p, size_t dest_max, hi_char **pp_arg1, hi_char **pp_arg2)
{
    hi_char *char_p = HI_NULL;

    if (strlen(proc_para_p) == 0) {
        /* not fined arg1 and arg2, return failed */
        *pp_arg1 = HI_NULL;
        *pp_arg2 = HI_NULL;
        return HI_FAILURE;
    }

    if (strlen(proc_para_p) > dest_max) {
        /* invalid input */
        *pp_arg1 = HI_NULL;
        *pp_arg2 = HI_NULL;
        HI_LOG_ERR("proc_para_p is invalid\n");
        return HI_FAILURE;
    }

    /* find arg1 */
    char_p = proc_para_p;
    while ((*char_p == ' ') && (*char_p != '\0')) {
        char_p++;
    }

    if (*char_p != '\0') {
        *pp_arg1 = char_p;
    } else {
        *pp_arg1 = HI_NULL;
        return HI_FAILURE;
    }

    /* ignor arg1 */
    while ((*char_p != ' ') && (*char_p != '\0')) {
        char_p++;
    }

    /* not find arg2, return */
    if (*char_p == '\0') {
        *pp_arg2 = HI_NULL;
        return HI_SUCCESS;
    }

    /* add '\0' for arg1 */
    *char_p = '\0';

    /* start to find arg2 */
    char_p = char_p + 1;
    while ((*char_p == ' ') && (*char_p != '\0')) {
        char_p++;
    }

    if (*char_p != '\0') {
        *pp_arg2 = char_p;
    } else {
        *pp_arg2 = HI_NULL;
    }

    return HI_SUCCESS;
}

static hi_s32 rm_drv_proc_read(hi_void *s, hi_void *data_p)
{
    hi_u32 rm_path_id;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;

    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);

    osal_proc_print(s, "\n--------------------------RM INFO(version:2019-08-27)---------------------------\n");
    osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "rm_path_count", ":", rm_drv_golbal_ctx_p->rm_path_count, " ", "|");
    osal_proc_print(s, "%-15s%-2s%-13s\n", "wake_up_flags", ":", rm_drv_golbal_ctx_p->wake_up_flags ? "TRUE" : "FALSE");
    osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "win_count", ":",
                    rm_drv_golbal_ctx_p->rm_available_win_count, " ", "|");
    osal_proc_print(s, "%-15s%-2s%-13s\n", "waiting_event", ":", rm_drv_golbal_ctx_p->waiting_event ? "TRUE" : "FALSE");
    osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "wake_up_cnt", ":", rm_drv_golbal_ctx_p->wake_up_cnt, " ", "|");
    osal_proc_print(s, "%-15s%-2s%-13s\n", "wind_owner", ":", g_wind_owner[rm_drv_golbal_ctx_p->wind_owner]);
    osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "wake_up_time", ":", rm_drv_golbal_ctx_p->wake_up_time, " ", "|");
    osal_proc_print(s, "%-15s%-2s%-13d\n", "normal_cnt", ":", rm_drv_golbal_ctx_p->normal_cnt);
    osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "recycle_cnt", ":", rm_drv_golbal_ctx_p->path_recycle_cnt, " ", "|");
    osal_proc_print(s, "%-15s%-2s%-13d\n", "owner_id", ":", rm_drv_golbal_ctx_p->win_owner_id);

    for (rm_path_id = 0; rm_path_id < RM_DRV_CHN_MAX_NUM; rm_path_id++) {
        rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);

        if (HI_INVALID_HANDLE != rm_drv_path_ctx_p->rm_handle) {
            osal_proc_print(s, "\n----------------------------------RM PATH[%x] ATTR--------------------------------\n",
                            rm_path_id);
            osal_proc_print(s, "%-15s%-2s0x%-11x%-6s%-2s", "hpath_handle", ":", rm_drv_path_ctx_p->rm_handle, " ", "|");
            osal_proc_print(s, "%-15s%-2s%-13s\n", "chn_enable", ":", rm_drv_path_ctx_p->chn_enable ? "TRUE" : "FALSE");
            osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "cur_index", ":", rm_drv_path_ctx_p->cur_index, " ", "|");
            osal_proc_print(s, "%-15s%-2s%-13s\n", "cur_event", ":", g_wind_event[rm_drv_path_ctx_p->cur_event]);
            osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "used_node", ":", rm_drv_path_ctx_p->used_node, " ", "|");
            osal_proc_print(s, "%-15s%-2s%-13d\n", "free_node", ":", rm_drv_path_ctx_p->free_node);
            osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "path_owner_id", ":", rm_drv_path_ctx_p->cur_pid, " ", "|");
            osal_proc_print(s, "%-15s%-2s0x%-11p\n", "event_handle", ":", rm_drv_path_ctx_p->event_handle_p);
            osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "create_cnt", ":",
                            rm_drv_golbal_ctx_p->chn_create_cnt[rm_path_id], " ", "|");
            osal_proc_print(s, "%-15s%-2s%-13d\n", "destroy_cnt", ":",
                            rm_drv_golbal_ctx_p->chn_destroy_cnt[rm_path_id]);
            osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "queue_ok", ":", rm_drv_path_ctx_p->queue_cnt, " ", "|");
            osal_proc_print(s, "%-15s%-2s%-13d\n", "queue_try", ":", rm_drv_path_ctx_p->try_queue_cnt);
            osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "cur_query_time", ":", rm_drv_path_ctx_p->cur_query_time,
                            " ", "|");
            osal_proc_print(s, "%-15s%-2s%-13d\n", "lst_query_time", ":", rm_drv_path_ctx_p->lst_query_time);
            osal_proc_print(s, "%-15s%-2s%-13d%-6s%-2s", "query_diff(us)", ":", rm_drv_path_ctx_p->query_time_diff,
                            " ", "|");
            osal_proc_print(s, "%-15s%-2s%-13s\n", "idle", ":", rm_drv_path_ctx_p->idle ? "true" : "false");
        }
    }

    osal_proc_print(s, "-------------------------RM ver_info:%-22s----------------------\n", g_version_info);
    osal_proc_print(s, "\n");
    return HI_SUCCESS;
}

static int drv_proc_cmd_help__(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    printk("argc = %d, argv=%s, private =%p, *private = %s\n", argc, argv[0], private, (char *)private);

    rm_drv_proc_help();
    return HI_SUCCESS;
}

static osal_proc_cmd g_rm_proc_cmd[] = {
    { "help", drv_proc_cmd_help__ }
};

/********************************************************************************/
/* Public API                                                                   */
/********************************************************************************/
hi_s32 rm_drv_proc_add(hi_void)
{
    hi_s32 len;
    osal_proc_entry *rm_proc_entry = HI_NULL;
    hi_char proc_name[RM_DRV_PROC_LEN];

    len = snprintf_s(proc_name, sizeof(proc_name), strlen("rm"), "rm");
    if (len < 0) {
        HI_LOG_ERR("snprintf failed!\n");
        return HI_FAILURE;
    }

    rm_proc_entry = osal_proc_add(proc_name, RM_DRV_PROC_LEN);
    if (rm_proc_entry == HI_NULL) {
        HI_LOG_ERR("RM add proc failed!\n");
        return HI_FAILURE;
    }

    rm_proc_entry->read = rm_drv_proc_read;
    rm_proc_entry->cmd_cnt = sizeof(g_rm_proc_cmd) / sizeof(osal_proc_cmd);
    rm_proc_entry->cmd_list = g_rm_proc_cmd;
    rm_proc_entry->private = rm_proc_entry->name;

    return HI_SUCCESS;
}

hi_s32 rm_drv_proc_del(hi_void)
{
    hi_s32 len;
    hi_char proc_name[RM_DRV_PROC_LEN];

    len = snprintf_s(proc_name, sizeof(proc_name), strlen("rm"), "rm");
    if (len < 0) {
        return HI_FAILURE;
    }

    osal_proc_remove(proc_name, RM_DRV_PROC_LEN);
    return HI_SUCCESS;
}

