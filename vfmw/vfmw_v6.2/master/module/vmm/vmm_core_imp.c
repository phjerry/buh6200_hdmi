/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vmm_ext.h"
#include "vmm_dbg.h"
#include "vmm_core_imp.h"
#include "vmm_memlist.h"
#include "vmm_msglist.h"
#include "vmm_ops.h"
#include "linux_proc.h"

#ifdef VFMW_PROC_SUPPORT
#ifndef VFMW_KO2_SUPPORT
#define VMM_PROC_NAME "vfmw_vmm"
#else
#define VMM_PROC_NAME "vfmw2_vmm"
#endif
#define VMM_CMD_NUM (2)
#define VMM_CMD_LEN (50)
typedef hi_s8 (*cmd_str_ptr)[VMM_CMD_NUM][VMM_CMD_LEN];
#endif

#define VMM_DEFAULT_WAITTIME     (5)
#define VMM_MAX_RLSNUM_BY_CMD    (5)
#define VMM_MAX_RLSNUM_BY_THREAD (20)

#define VMM_MAX_STATISTIC_NUM (30)

#define VMM_START_TICK          (0)
#define VMM_ADDMSG_END_TICK     (1)
#define VMM_THREAD_RESPOND_TICK (2)
#define VMM_PROCESS_END_TICK    (3)
#define VMM_CALLBACK_END_TICK   (4)
#define VMM_TICK_NUM            (VMM_CALLBACK_END_TICK + 1)

typedef enum {
    THREAD_STATUS_PREPARE = 0,
    THREAD_STATUS_STOP,
    THREAD_STATUS_RUNNING,
    THREAD_STATUS_EXIT,
    THREAD_STATUS_SLEEP,
    THREAD_STATUS_BUTT,
} vmm_thread_status;

typedef struct {
    hi_u32 prior_flag;
    hi_u32 time_tick[VMM_TICK_NUM];
    hi_u32 time_consume[VMM_TICK_NUM];
    vmm_msg_data msg_info;
} vmm_statistic;

typedef struct {
    hi_u32 wrtie_pointer;
    hi_u32 msg_count;
    hi_u32 max_time_consume[VMM_TICK_NUM];
    hi_u32 last_prior_msg_time_consume[VMM_TICK_NUM];
    vmm_statistic time_stat_in_us[VMM_MAX_STATISTIC_NUM];
} vmm_dbg;

typedef struct {
    hi_u32 is_init;
    hi_s32 stop_thread;
    hi_void *thread_handle;
    OS_SEMA mutex;
    OS_LOCK lock;
    hi_ulong lock_flags;
    OS_EVENT event;
    vmm_thread_status status;
    hi_u32 wait_time_in_ms;
    fn_vmm_callback callback;
    vmm_dbg *dbg_info;
} vmm_global_ctx;

static hi_s32 vmm_core_alloc_from_thread(hi_void *param_in);

hi_u32 g_vmm_print_enable = VMM_DEFAULT_PRINT_ENABLE;

static vmm_global_ctx g_vmm_ctx = {
    .is_init = 0,
    .stop_thread = 0,
    .callback = HI_NULL,
    .thread_handle = HI_NULL,
    .status = THREAD_STATUS_BUTT,
    .wait_time_in_ms = VMM_DEFAULT_WAITTIME,
};

static const vmm_cmd_node g_vmm_blk_cmd_table[] = {
    { VMM_CMD_BLK_SET_LOCATION,        vmm_core_set_location },
    { VMM_CMD_BLK_ALLOC,               vmm_core_alloc },
    { VMM_CMD_BLK_RELEASE,             vmm_core_release },
    { VMM_CMD_BLK_CONFIG_MEM_NODE,     vmm_core_config_mem_node },
    { VMM_CMD_BLK_MAP,                 vmm_core_map },
    { VMM_CMD_BLK_UNMAP,               vmm_core_unmap },
    { VMM_CMD_BLK_CONFIG_MEM_POOL,     vmm_core_config_mem_block },
    { VMM_CMD_BLK_RELEASE_BURST,       vmm_core_release_burst },
    { VMM_CMD_BLK_SET_PRIORITY_POLICY, vmm_core_set_priority_policy },
    { VMM_CMD_BLK_GET_MEM_NODE_INFO,   vmm_core_get_mem_node_info },
    { VMM_CMD_BLK_CANCLE_CMD,          vmm_core_cancle_cmd },
    { VMM_CMD_BLK_TYPE_MAX,            HI_NULL }, /* terminal element */
};

static const vmm_cmd_node g_vmm_cmd_table[] = {
    { VMM_CMD_ALLOC,         vmm_core_alloc_from_thread },
    { VMM_CMD_RELEASE,       vmm_core_release },
    { VMM_CMD_RELEASE_BURST, vmm_core_release_burst },
    { VMM_CMD_BLK_TYPE_MAX,  HI_NULL }, /* terminal element */
};

static hi_s32 vmm_proc_write(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static OS_PROC_CMD g_vmm_cmd_list[] = {
    { "print", vmm_proc_write },
    { "help", vmm_proc_write }
};

static hi_u32 vmm_proc_argv_to_cmd_str(hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_s32 argc, cmd_str_ptr cmd_str)
{
    hi_u32 index;
    hi_s32 ret;

    if (argc > PROC_CMD_SINGEL_LENGTH_MAX) {
        return 0;
    }

    if (argc > VMM_CMD_NUM) {
        PRINT(PRN_ERROR, "argc:%d > max vmm cnt %d\n", argc, VMM_CMD_NUM);
        argc = VMM_CMD_NUM;
    }

    for (index = 0; index < argc; index++) {
        ret = snprintf_s((*cmd_str)[index], VMM_CMD_LEN, PROC_CMD_SINGEL_LENGTH_MAX - 1, *(argv + index));
        if (ret < 0) {
            PRINT(PRN_ERROR, "snprintf_s failed ret = %d\n", ret);
            return index;
        }
    }

    return index;
}


static hi_s32 str_to_val(hi_char *str, hi_u32 *data)
{
    hi_u32 i, d, dat, weight;

    dat = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        i = 2; /* 0x start need skip 2 chars */
        weight = 16; /* 0x start is hex so weight is 16 */
    } else {
        i = 0;
        weight = 10; /* none start is decimal so weight is 10 */
    }

    for (; i < 10; i++) { /* up to max bit hex has 10 chars a hexnum total */
        if (str[i] < 0x20) { /* 0x20 is uplimit of invalid char */
            break;
        } else if (weight == 16 && str[i] >= 'a' && str[i] <= 'f') { /* 16 is hex */
            d = str[i] - 'a' + 10; /* 'a' means 10 */
        } else if (weight == 16 && str[i] >= 'A' && str[i] <= 'F') { /* 16 is hex */
            d = str[i] - 'A' + 10; /* 'A' means 10 */
        } else if (str[i] >= '0' && str[i] <= '9') {
            d = str[i] - '0';
        } else {
            return -1; /* invalid num return -1 means Fail */
        }

        dat = dat * weight + d;
    }

    *data = dat;

    return 0;
}

static fn_vmm_cmd_handler vmm_core_get_handler(hi_u32 cmd_id, hi_u32 blk_flag)
{
    hi_u32 index = 0;
    const vmm_cmd_node *cmd_table = HI_NULL;
    hi_s32 loop_time = 0;
    fn_vmm_cmd_handler target_handler = HI_NULL;

    if (blk_flag) {
        cmd_table = g_vmm_blk_cmd_table;
        loop_time = sizeof(g_vmm_blk_cmd_table) / sizeof(vmm_cmd_node);
    } else {
        cmd_table = g_vmm_cmd_table;
        loop_time = sizeof(g_vmm_cmd_table) / sizeof(vmm_cmd_node);
    }

    for (index = 0; index < loop_time; index++) {
        if (cmd_table[index].handler == HI_NULL) {
            break;
        }

        if (cmd_id == cmd_table[index].cmd_id) {
            target_handler = cmd_table[index].handler;
            break;
        }
    }

    return target_handler;
}

static hi_s32 vmm_convert_data(vmm_cmd_type cmd_id, hi_void *args, vmm_msg_data *msg_data)
{
    hi_s32 ret = VMM_SUCCESS;

    switch (cmd_id) {
        case VMM_CMD_ALLOC:
        case VMM_CMD_RELEASE: {
            vmm_buffer *buf_rec = (vmm_buffer *)args;

            msg_data->priv_id = buf_rec->priv_id;
            msg_data->cmd_id = cmd_id;
            VFMW_CHECK_SEC_FUNC(memcpy_s(&msg_data->param.buf_rec,
                                         sizeof(vmm_buffer), buf_rec, sizeof(vmm_buffer)));
        }
        break;

        case VMM_CMD_RELEASE_BURST: {
            vmm_rls_burst *rls_burst = (vmm_rls_burst *)args;

            msg_data->priv_id = rls_burst->priv_id;
            msg_data->cmd_id = cmd_id;
            VFMW_CHECK_SEC_FUNC(memcpy_s(&msg_data->param.rls_burst,
                                         sizeof(vmm_rls_burst), rls_burst, sizeof(vmm_rls_burst)));
        }
        break;

        default: {
            ret = VMM_FAILURE;
            PRINT(PRN_ERROR, "%s,%d, not support cmd_id = %d\n", __func__, __LINE__, cmd_id);
        }
        break;
    }

    return ret;
}

static hi_void vmm_convert_mem_data_first(vmm_buffer *buf_info, hi_u32 time_consume, mem_data_info *mem_data)
{
    VFMW_CHECK_SEC_FUNC(memset_s(mem_data, sizeof(mem_data_info), 0, sizeof(mem_data_info)));

    mem_data->status = NODE_STATUS_INUSE;
    mem_data->location = VMM_LOCATION_INSIDE;

    VFMW_CHECK_SEC_FUNC(memcpy_s(&mem_data->buf_info, sizeof(vmm_buffer), buf_info, sizeof(vmm_buffer)));

    mem_data->dbg_info.alloc_time_consume_in_us = time_consume;

    return;
}

static hi_void vmm_wakeup_thread(hi_void)
{
    VMM_EVENT_GIVE(&g_vmm_ctx.event);
}

static hi_void vmm_wait_event(hi_void)
{
    VMM_EVENT_WAIT(&g_vmm_ctx.event, g_vmm_ctx.wait_time_in_ms);
}

static hi_s32 vmm_protected_with_lock_blk_cmd(vmm_cmd_blk_type cmd)
{
    hi_s32 ret = VMM_FALSE;

    if (cmd == VMM_CMD_BLK_SET_LOCATION || cmd == VMM_CMD_BLK_CANCLE_CMD) {
        ret = VMM_TRUE;
    }

    return ret;
}

static hi_s32 vmm_get_dbg_info_id(hi_u32 msg_tick, hi_u32 *target_idx)
{
    hi_u32 i;
    vmm_dbg *dbg = g_vmm_ctx.dbg_info;
    hi_u32 index = dbg->wrtie_pointer;

    for (i = 0; i < VMM_MAX_STATISTIC_NUM; i++) {
        if (dbg->time_stat_in_us[index].msg_info.tick == msg_tick) {
            *target_idx = index;
            return VMM_SUCCESS;
        }

        index = (index == (VMM_MAX_STATISTIC_NUM - 1)) ? (0) : (index + 1);
    }

    return VMM_FAILURE;
}

static hi_s32 vmm_dbg_init(hi_void)
{
    g_vmm_ctx.dbg_info = (vmm_dbg *)OS_ALLOC_VIR("VMM_Dbg", sizeof(vmm_dbg));

    if (g_vmm_ctx.dbg_info == HI_NULL) {
        PRINT(PRN_FATAL, "%s,%d, alloc Debug mem failed! size = %d\n", __func__, __LINE__, sizeof(vmm_dbg));
        return VMM_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(g_vmm_ctx.dbg_info, sizeof(vmm_dbg), 0, sizeof(vmm_dbg)));
    g_vmm_ctx.dbg_info->msg_count = 1;

    return VMM_SUCCESS;
}

static hi_void vmm_dbg_deinit(hi_void)
{
    if (g_vmm_ctx.dbg_info != HI_NULL) {
        OS_FREE_VIR(g_vmm_ctx.dbg_info);
        g_vmm_ctx.dbg_info = HI_NULL;
    }
}

static hi_void vmm_dbg_calculate_time(hi_u32 index)
{
    hi_u32 i;
    vmm_dbg *dbg = g_vmm_ctx.dbg_info;
    hi_u32 *time_tick = &dbg->time_stat_in_us[index].time_tick[0];
    hi_u32 *time_consume = &dbg->time_stat_in_us[index].time_consume[0];
    hi_u32 *max_time_consume = &dbg->max_time_consume[0];

    for (i = 0; i < (VMM_TICK_NUM - 1); i++) {
        time_consume[i] = time_tick[i + 1] - time_tick[i];
    }

    time_consume[VMM_TICK_NUM - 1] = time_tick[VMM_TICK_NUM - 1] - time_tick[0];

    for (i = 0; i < VMM_TICK_NUM; i++) {
        if (max_time_consume[i] < time_consume[i]) {
            max_time_consume[i] = time_consume[i];
        }
    }

    if (dbg->time_stat_in_us[index].prior_flag || dbg->time_stat_in_us[index].msg_info.tick == 1) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(dbg->last_prior_msg_time_consume,
            sizeof(dbg->last_prior_msg_time_consume), time_consume, sizeof(hi_u32) * VMM_TICK_NUM));
    }

    return;
}

static hi_void vmm_dbg_record_time(vmm_msg_data *msg_data, hi_u32 pos, hi_u32 prior_flag)
{
    vmm_dbg *dbg = g_vmm_ctx.dbg_info;
    hi_u32 cur_time = OS_GET_TIME_US();
    hi_u32 index = dbg->wrtie_pointer;

    if (pos >= VMM_TICK_NUM) {
        PRINT(PRN_ERROR, "pos(%d) not support !\n", pos);
    }

    if (pos == VMM_START_TICK) {
        VFMW_CHECK_SEC_FUNC(memset_s(&dbg->time_stat_in_us[index].time_tick[0],
                                     sizeof(hi_u32) * VMM_TICK_NUM, 0, sizeof(hi_u32) * VMM_TICK_NUM));
        msg_data->tick = dbg->msg_count;
        dbg->time_stat_in_us[index].time_tick[VMM_START_TICK] = cur_time;
    } else if (pos == VMM_ADDMSG_END_TICK) {
        dbg->time_stat_in_us[index].time_tick[VMM_ADDMSG_END_TICK] = cur_time;
        VFMW_CHECK_SEC_FUNC(memcpy_s(&dbg->time_stat_in_us[index].msg_info,
                                     sizeof(vmm_msg_data), msg_data, sizeof(msg_data)));
        dbg->time_stat_in_us[index].prior_flag = prior_flag;

        dbg->wrtie_pointer = (dbg->wrtie_pointer == (VMM_MAX_STATISTIC_NUM - 1)) ? (0) : (dbg->wrtie_pointer + 1);
        dbg->msg_count++;
    } else if (vmm_get_dbg_info_id(msg_data->tick, &index) == VMM_SUCCESS) {
        if (pos >= 5) { /* 5 :a number */
            PRINT(PRN_FATAL, "line %d func %s pos = %d\n", __LINE__, __func__, pos);
            return;
        }

        dbg->time_stat_in_us[index].time_tick[pos] = cur_time;

        if (pos == VMM_CALLBACK_END_TICK) {
            vmm_dbg_calculate_time(index);
        }
    }

    return;
}
#ifdef VFMW_PROC_SUPPORT
static hi_s32 vmm_dbg_read_proc(PROC_FILE *p)
{
    hi_u32 i;
    hi_u32 index;
    vmm_dbg *dbg = g_vmm_ctx.dbg_info;
    vmm_statistic *statistic = dbg->time_stat_in_us;
    /* 16 :a number */
    hi_s8 cmd_id_tab[VMM_CMD_TYPE_MAX + 1][16] = { { "Alloc" }, { "Release" }, { "ReleaseBurst" }, { "Unknown" } };
    hi_u32 show_num = 10; /* VMM_MAX_STATISTIC_NUM */
    hi_s32 used_size = 0;
    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);

    /* statistics info */
    OS_DUMP_PROC(p, 0, &used_size, 0,
        "--------------------------------------- Msg History ------------------------------------------------- \n");

    OS_DUMP_PROC(p, 0, &used_size, 0,
        "  NO.\t        PrivID\t              cmd_id    |   AddMsg\t Thread\t\t Process\t  CallBack\t    Total\n");
    OS_DUMP_PROC(p, 0, &used_size, 0, "\t\t\t                       |  %-7d\t %-7d\t %-7d\t %-7d\t  %-7d\t\n",
                 dbg->last_prior_msg_time_consume[0], dbg->last_prior_msg_time_consume[1],
                 dbg->last_prior_msg_time_consume[2], /* 2 :a number */
                 /* 3 :a number 4 :a number */
                 dbg->last_prior_msg_time_consume[3], dbg->last_prior_msg_time_consume[4]);
    OS_DUMP_PROC(p, 0, &used_size, 0, "\t\t\t                       |  %-7d\t %-7d\t %-7d\t %-7d\t  %-7d\t\n",
                 dbg->max_time_consume[0], dbg->max_time_consume[1], dbg->max_time_consume[2], /* 2 :a number */
                 dbg->max_time_consume[3], dbg->max_time_consume[4]); /* 3 :a number 4 :a number */
    OS_DUMP_PROC(p, 0, &used_size, 0,
        "--------------------------------------|--------------------------------------------------------------- \n");

    index = (dbg->wrtie_pointer == 0) ? (VMM_MAX_STATISTIC_NUM - 1) : (dbg->wrtie_pointer - 1);

    for (i = 0; i < show_num; i++) {
        OS_DUMP_PROC(p, 0, &used_size, 0, "%-4d\t 0x%-16llx\t %-12s  |  %-7d\t %-7d\t %-7d\t %-7d\t  %-7d\t\n",
                     statistic[index].msg_info.tick, statistic[index].msg_info.priv_id,
                     cmd_id_tab[statistic[index].msg_info.cmd_id], statistic[index].time_consume[0],
                     statistic[index].time_consume[1], statistic[index].time_consume[2], /* 2 :a number */
                     statistic[index].time_consume[3], statistic[index].time_consume[4]); /* 3 :a number 4 :a number */
        index = (index == 0) ? (VMM_MAX_STATISTIC_NUM - 1) : (index - 1);
    }

    return VMM_SUCCESS;
}

static hi_s32 vmm_proc_read(PROC_FILE *p, hi_void *v)
{
    hi_s32 used_size = 0;

    vmm_msg_list_read_proc(p);

    vmm_dbg_read_proc(p);

    OS_DUMP_PROC(p, 0, &used_size, 0, "\n");

    vmm_mem_list_read_proc(p);

    OS_DUMP_PROC(p, 0, &used_size, 0, "\n\n");

    return VMM_SUCCESS;
}

static hi_void vmm_proc_help(hi_void)
{
    PRINT(PRN_ALWS, "------ VMM Proc Help ------\n");
    PRINT(PRN_ALWS, "USAGE:echo [cmd] [para1] [para2] > /proc/vfmw/vmm\n");
    PRINT(PRN_ALWS, "cmd = PrintEn,     para1 = print_enable_word  \n");
    PRINT(PRN_ALWS, "--------------------------- \n");
    PRINT(PRN_ALWS, "'print_enable_word' definition, from bit31 to bit0: \n");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "--", "--", "--", "--");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "--", "--", "--", "--");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "--", "--", "--", "--");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "--", "--", "--", "--");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "--", "--", "--", "--");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "--", "--", "--", "--");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "--", "DBG", "INFO", "TimeDly");
    PRINT(PRN_ALWS, "%8s  %8s  %8s   %8s\n", "IMP", "WARN", "ERROR", "FATAL");
}

static hi_void vmm_proc_command_handler(hi_s8 cmd_str[VMM_CMD_NUM][VMM_CMD_LEN])
{
    hi_u32 value = 0;
    hi_u32 cmd_num = 0;

    if (!OS_STRNCMP(cmd_str[cmd_num], "print", OS_STRLEN("print"))) {
        if (str_to_val(cmd_str[cmd_num + 1], &value) != 0) {
            PRINT(PRN_ALWS, "error string to value '%s'!\n", cmd_str[cmd_num + 1]);
            return;
        }

        g_vmm_print_enable = value;
        PRINT(PRN_ALWS, "Set g_vmm_print_enable %d\n", g_vmm_print_enable);
    } else {
        vmm_proc_help();
    }

    return;
}

static hi_s32 vmm_proc_write(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s8 cmd_str[VMM_CMD_NUM][VMM_CMD_LEN];
    hi_u32 count;

    count = vmm_proc_argv_to_cmd_str(argv, argc, &cmd_str);
    if (count == 0) {
        PRINT(PRN_ALWS, "argv_to_cmd_str failed\n");
        return 0;
    }

    vmm_proc_command_handler(cmd_str);

    return HI_SUCCESS;
}
#endif

static hi_s32 vmm_alloc_real(vmm_buffer *mem_info)
{
    hi_s32 ret;
    mem_data_info mem_data;
    hi_u32 start_time;
    hi_u32 stop_time;

    start_time = OS_GET_TIME_US();
    ret = vmm_ops_alloc(mem_info);
    stop_time = OS_GET_TIME_US();

    if (ret == VMM_SUCCESS) {
        vmm_convert_mem_data_first(mem_info, stop_time - start_time, &mem_data);

        ret = vmm_mem_list_add_node(&mem_data);
        if (ret != VMM_SUCCESS) {
            PRINT(PRN_ERROR, "%s,%d,add Memlist failed!, Name = '%s', size = %d\n",
                  __func__, __LINE__, mem_info->buf_name, mem_info->size);

            if (vmm_ops_release(mem_info) != VMM_SUCCESS) {
                PRINT(PRN_ERROR, "%s,%d, Memlist AddNode failed!, BufferName = '%s', BuferAddr = 0x%x, size = %d\n",
                      __func__, __LINE__, mem_info->buf_name, mem_info->start_phy_addr, mem_info->size);
            }
        }
    }

    return ret;
}

static hi_s32 vmm_release_real(vmm_buffer *mem_info)
{
    hi_s32 ret;

    ret = vmm_ops_release(mem_info);
    if (ret == VMM_SUCCESS) {
        if (vmm_mem_list_delete_node_by_phyaddr(mem_info->start_phy_addr) != VMM_SUCCESS) {
            PRINT(PRN_ERROR, "%s,%d,Delete Memlist failed!, BuferAddr = 0x%x, size = %d\n",
                  __func__, __LINE__, mem_info->start_phy_addr, mem_info->size);
        }
    }

    return ret;
}

static hi_void vmm_release_expect_len(hi_u32 expect_len, hi_u32 max_rls_num, hi_u32 lock_en)
{
    hi_s32 ret = VMM_SUCCESS;
    hi_u32 loop_cnt_max = 0;
    hi_u32 rls_len = 0;
    mem_data_info mem_data;

    while ((!g_vmm_ctx.stop_thread) && (ret == VMM_SUCCESS) && (rls_len < expect_len) &&
           (loop_cnt_max < max_rls_num)) {
        VFMW_CHECK_SEC_FUNC(memset_s(&mem_data, sizeof(mem_data_info), 0, sizeof(mem_data_info)));

        if (lock_en) {
            VMM_MUTEX_LOCK(&g_vmm_ctx.mutex);
        }

        ret = vmm_mem_list_find_one_node_can_be_rls(&mem_data);
        if (ret == VMM_SUCCESS) {
            if (vmm_release_real(&mem_data.buf_info) != VMM_SUCCESS) {
                PRINT(PRN_ERROR, "%s,%d, Release buffer failed\n", __func__, __LINE__);
            } else {
                rls_len += mem_data.buf_info.size;
            }

            loop_cnt_max++;
        }

        if (lock_en) {
            VMM_MUTEX_UNLOCK(&g_vmm_ctx.mutex);
        }
    }

    if (loop_cnt_max >= max_rls_num) {
        PRINT(PRN_ERROR, "%s,%d, continuity release Memory %d time(expect_len = %d),don't go to release this time.\n",
              __func__, __LINE__, loop_cnt_max, expect_len);
    }

    return;
}

static hi_s32 vmm_core_alloc_from_thread(hi_void *param_in)
{
    hi_s32 ret;
    vmm_buffer *mem_info = (vmm_buffer *)param_in;

    D_VMM_CHECK_PTR_RET(mem_info, ERR_VMM_NULL_PTR);
    vmm_release_expect_len(mem_info->size, VMM_MAX_RLSNUM_BY_CMD, 0);

    ret = vmm_alloc_real(mem_info);

    return ret;
}

static hi_void vmm_callback_exception_handle(vmm_cmd_type cmd_id, hi_void *param_out, hi_s32 ret_val)
{
    vmm_buffer *mem_info = (vmm_buffer *)param_out;

    if (ret_val == VMM_SUCCESS) {
        if (cmd_id == VMM_CMD_ALLOC) {
            if (vmm_release_real(mem_info) != VMM_SUCCESS) {
                PRINT(PRN_ERROR, "%s,%d, Release remain Memory failed! buf_name = %s, Phyaddr = 0x%x, size = %d\n",
                      __func__, __LINE__, mem_info->buf_name, mem_info->start_phy_addr, mem_info->size);
            }
        }
    }

    return;
}

static hi_void vmm_callback_return_result(vmm_cmd_type cmd_id, hi_void *param_out, hi_s32 ret_val)
{
    hi_s32 ret;

    ret = g_vmm_ctx.callback(cmd_id, param_out, ret_val);
    if (ret != VMM_SUCCESS) {
        vmm_callback_exception_handle(cmd_id, param_out, ret_val);
    }

    return;
}

static hi_s32 vmm_cmd_process_one_case(vmm_msg_data *msg_data)
{
    hi_s32 ret;
    fn_vmm_cmd_handler cmd_handler;
    hi_void *param_in = &msg_data->param;

    vmm_dbg_record_time(msg_data, VMM_THREAD_RESPOND_TICK, 0);

    cmd_handler = vmm_core_get_handler(msg_data->cmd_id, 0);

    D_VMM_CHECK_PTR_RET(cmd_handler, ERR_VMM_NOT_SUPPORT);

    ret = cmd_handler(param_in);

    vmm_dbg_record_time(msg_data, VMM_PROCESS_END_TICK, 0);

    vmm_callback_return_result(msg_data->cmd_id, param_in, ret);

    vmm_dbg_record_time(msg_data, VMM_CALLBACK_END_TICK, 0);

    return ret;
}

static hi_void vmm_cmd_process_all(hi_void)
{
    vmm_msg_data msg_data;
    hi_s32 ret = VMM_OSAL_SUCCESS;

    while ((!g_vmm_ctx.stop_thread) && (vmm_msg_list_is_empty() != VMM_TRUE)) {
        VFMW_CHECK_SEC_FUNC(memset_s(&msg_data, sizeof(vmm_msg_data), 0, sizeof(vmm_msg_data)));

        VMM_MUTEX_LOCK(&g_vmm_ctx.mutex);

        ret = vmm_msg_list_take_out_node(&msg_data);
        if (ret == VMM_OSAL_SUCCESS) {
            vmm_cmd_process_one_case(&msg_data);
        } else {
            PRINT(PRN_ERROR, "%s,%d,MsgList isn't Empty, but TakeOut Node Failed!\n", __func__, __LINE__);
        }

        VMM_MUTEX_UNLOCK(&g_vmm_ctx.mutex);
    }

    return;
}

static hi_void vmm_try_release_all(hi_void)
{
    vmm_release_expect_len(-1, VMM_MAX_RLSNUM_BY_THREAD, 1);
}

static hi_void vmm_thread_process(hi_void)
{
    g_vmm_ctx.status = THREAD_STATUS_PREPARE;

    while (!g_vmm_ctx.stop_thread) {
        g_vmm_ctx.status = THREAD_STATUS_RUNNING;

        vmm_cmd_process_all();
        vmm_try_release_all();

        if ((!g_vmm_ctx.stop_thread) && vmm_msg_list_is_empty()) {
            g_vmm_ctx.status = THREAD_STATUS_SLEEP;
            vmm_wait_event();
        }
    }

    g_vmm_ctx.status = THREAD_STATUS_EXIT;

    return;
}

hi_s32 vmm_core_alloc(hi_void *param_in)
{
    hi_s32 ret;
    vmm_buffer *mem_info = (vmm_buffer *)param_in;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(mem_info, ERR_VMM_NULL_PTR);

    ret = vmm_alloc_real(mem_info);

    return ret;
}

hi_s32 vmm_core_release(hi_void *param_in)
{
    hi_s32 ret;
    vmm_buffer *mem_info = (vmm_buffer *)param_in;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(mem_info, ERR_VMM_NULL_PTR);

    ret = vmm_mem_list_mark_status(mem_info->start_phy_addr, NODE_STATUS_NOUSE);
    if (ret != VMM_SUCCESS) {
        PRINT(PRN_ERROR, "%s mark mem %s status failed!\n", __func__, mem_info->buf_name);
        return ret;
    }

    if (vmm_mem_list_is_one_node_can_be_rls(mem_info->start_phy_addr) == VMM_SUCCESS) {
        ret = vmm_release_real(mem_info);
    }

    return ret;
}

hi_s32 vmm_core_map(hi_void *param_in)
{
    hi_s32 ret;
    vmm_buffer *mem_info = (vmm_buffer *)param_in;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(mem_info, ERR_VMM_NULL_PTR);

    ret = vmm_ops_map(mem_info);
    if (ret == VMM_SUCCESS) {
        VMM_ASSERT(vmm_mem_list_update_buf_rec(mem_info) == VMM_SUCCESS);
    }

    return ret;
}

hi_s32 vmm_core_unmap(hi_void *param_in)
{
    hi_s32 ret;
    vmm_buffer *mem_info = (vmm_buffer *)param_in;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(param_in, ERR_VMM_NULL_PTR);

    ret = vmm_ops_unmap(mem_info);
    if (ret == VMM_SUCCESS) {
        VMM_ASSERT(vmm_mem_list_update_buf_rec(mem_info) == VMM_SUCCESS);
    }

    return ret;
}

hi_s32 vmm_core_config_mem_block(hi_void *param_in)
{
    hi_s32 ret;
    vmm_buffer *mem_info = (vmm_buffer *)param_in;
    hi_u32 start_time;
    hi_u32 stop_time;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(mem_info, ERR_VMM_NULL_PTR);

    start_time = OS_GET_TIME_US();
    ret = vmm_ops_config_mem_block(mem_info);
    stop_time = OS_GET_TIME_US();

    if (ret == VMM_SUCCESS) {
        mem_data_info mem_data;

        vmm_convert_mem_data_first(mem_info, stop_time - start_time, &mem_data);

        ret = vmm_mem_list_add_node(&mem_data);
        if (ret != VMM_SUCCESS) {
            PRINT(PRN_ERROR, "%s,%d,add Memlist failed!, BuferAddr = 0x%x, size = %d\n", __func__, __LINE__,
                  mem_info->start_phy_addr, mem_info->size);
        }
    }

    return ret;
}

hi_s32 vmm_core_config_mem_node(hi_void *param_in)
{
    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);

    return ERR_VMM_NOT_SUPPORT;
}

hi_s32 vmm_core_set_location(hi_void *param_in)
{
    hi_s32 ret;
    vmm_attr *attr = (vmm_attr *)param_in;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(attr, ERR_VMM_NULL_PTR);

    ret = vmm_mem_list_mark_location(attr->phy_addr, attr->location);

    return ret;
}

hi_s32 vmm_core_release_burst(hi_void *param_in)
{
    hi_s32 ret = VMM_SUCCESS;
    hi_s32 ret_tmp = VMM_SUCCESS;
    hi_s32 rls_cnt = 0;
    vmm_rls_burst *rls_burst = (vmm_rls_burst *)param_in;
    vmm_msg_delete_info msg_delete;
    mem_data_info mem_data;
    vmm_buffer *buf_info = &mem_data.buf_info;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(param_in, ERR_VMM_NULL_PTR);

    VFMW_CHECK_SEC_FUNC(memset_s(&msg_delete, sizeof(vmm_msg_delete_info), 0, sizeof(vmm_msg_delete_info)));
    msg_delete.priv_id_valid = 1;
    msg_delete.priv_id = rls_burst->priv_id;
    msg_delete.priv_mask = rls_burst->priv_mask;
    ret |= vmm_msg_list_delete_specified_node(&msg_delete);

    ret |= vmm_mem_list_set_specified_node_no_use(rls_burst);

    while ((ret_tmp == VMM_SUCCESS) && (rls_cnt < 64)) { /* 64 :a number */
        VFMW_CHECK_SEC_FUNC(memset_s(&mem_data, sizeof(mem_data_info), 0, sizeof(mem_data_info)));

        ret_tmp = vmm_mem_list_takeout_one_node_can_be_rls(rls_burst, &mem_data);
        if (ret_tmp == VMM_SUCCESS) {
            rls_cnt++;

            if (vmm_ops_release(buf_info) != VMM_SUCCESS) {
                PRINT(PRN_ERROR, "%s,%d, buf_name = %s,Phyaddr = 0x%x,size = %d can be released,but release failed!\n",
                      __func__, __LINE__, buf_info->buf_name, buf_info->start_phy_addr, buf_info->size);
            }
        }
    }

    return ret;
}

hi_s32 vmm_core_set_priority_policy(hi_void *param_in)
{
    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);

    return ERR_VMM_NOT_SUPPORT;
}

hi_s32 vmm_core_get_mem_node_info(hi_void *param_in)
{
    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);

    return ERR_VMM_NOT_SUPPORT;
}

hi_s32 vmm_core_cancle_cmd(hi_void *param_in)
{
    hi_s32 ret;
    vmm_cancel_cmd *cancle_cmd = (vmm_cancel_cmd *)param_in;
    vmm_msg_delete_info msg_delete;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(param_in, ERR_VMM_NULL_PTR);

    VFMW_CHECK_SEC_FUNC(memset_s(&msg_delete, sizeof(vmm_msg_delete_info), 0, sizeof(vmm_msg_delete_info)));

    msg_delete.cmd_valid = 1;
    msg_delete.cmd_id = cancle_cmd->cmd_id;
    msg_delete.priv_id_valid = 1;
    msg_delete.priv_id = cancle_cmd->priv_id;
    msg_delete.priv_mask = cancle_cmd->priv_mask;

    ret = vmm_msg_list_delete_specified_node(&msg_delete);

    return ret;
}

hi_s32 vmm_core_add_msg(const vmm_cmd_type cmd_id, vmm_cmd_prio proir, hi_void *args)
{
    hi_s32 ret = VMM_SUCCESS;
    vmm_msg_data msg_data;

    D_VMM_ASSERT_RET((g_vmm_ctx.is_init == 1), ERR_VMM_NOT_INIT);
    D_VMM_CHECK_PTR_RET(args, ERR_VMM_NULL_PTR);

    VFMW_CHECK_SEC_FUNC(memset_s(&msg_data, sizeof(vmm_msg_data), 0, sizeof(vmm_msg_data)));

    vmm_dbg_record_time(&msg_data, VMM_START_TICK, 0);

    if (vmm_convert_data(cmd_id, args, &msg_data) != VMM_SUCCESS) {
        PRINT(PRN_ERROR, "%s command %d vmm_convert_data failed.\n", __func__, cmd_id);
        return ERR_VMM_INVALID_PARA;
    }

    ret = vmm_msg_list_add_node(&msg_data, proir);

    vmm_dbg_record_time(&msg_data, VMM_ADDMSG_END_TICK, proir);

    vmm_wakeup_thread();

    return ret;
}

hi_s32 vmm_core_send_cmd_block(vmm_cmd_blk_type cmd_id, hi_void *param_in_out)
{
    hi_s32 ret = VMM_SUCCESS;
    fn_vmm_cmd_handler cmd_handler = HI_NULL;

    D_VMM_CHECK_PTR_RET(param_in_out, ERR_VMM_NULL_PTR);

    if (cmd_id >= VMM_CMD_BLK_TYPE_MAX) {
        PRINT(PRN_ERROR, "%s invalid command %d\n", __func__, cmd_id);
        return ERR_VMM_INVALID_PARA;
    }

    cmd_handler = vmm_core_get_handler(cmd_id, 1);

    D_VMM_CHECK_PTR_RET(cmd_handler, ERR_VMM_NOT_SUPPORT);

    if (vmm_protected_with_lock_blk_cmd(cmd_id) == VMM_TRUE) {
        VMM_SPINLOCK_LOCK(&g_vmm_ctx.lock, &g_vmm_ctx.lock_flags);
        ret = cmd_handler(param_in_out);
        VMM_SPINLOCK_UNLOCK(&g_vmm_ctx.lock, &g_vmm_ctx.lock_flags);
    } else {
        VMM_MUTEX_LOCK(&g_vmm_ctx.mutex);
        ret = cmd_handler(param_in_out);
        VMM_MUTEX_UNLOCK(&g_vmm_ctx.mutex);
    }

    return ret;
}

hi_s32 vmm_core_init(fn_vmm_callback callback)
{
    hi_s32 ret = VMM_SUCCESS;

    D_VMM_CHECK_PTR_RET(callback, ERR_VMM_NULL_PTR);

    if (g_vmm_ctx.is_init == 0) {
        VMM_EVENT_INIT(&g_vmm_ctx.event, 0);
        VMM_MUTEX_INIT(&g_vmm_ctx.mutex);
        VMM_SPINLOCK_INIT(&g_vmm_ctx.lock);

        ret = vmm_dbg_init();
        if (ret != VMM_SUCCESS) {
            goto Error;
        }

        ret = vmm_msg_list_init();
        if (ret != VMM_SUCCESS) {
            PRINT(PRN_FATAL, "%s,%d, MsgList Init Failed!\n", __func__, __LINE__);
            goto Error_0;
        }

        ret = vmm_mem_list_init();
        if (ret != VMM_SUCCESS) {
            PRINT(PRN_FATAL, "%s,%d, MsgList Init Failed!\n", __func__, __LINE__);
            goto Error_1;
        }

        g_vmm_ctx.stop_thread = 0;

#ifndef VFMW_KO2_SUPPORT
        g_vmm_ctx.thread_handle = OS_CREATE_THREAD(vmm_thread_process, HI_NULL, "HI_VFMW_VMM");
#else
        g_vmm_ctx.thread_handle = OS_CREATE_THREAD(vmm_thread_process, HI_NULL, "HI_VFMW_VMM2");
#endif
        if (g_vmm_ctx.thread_handle == HI_NULL) {
            PRINT(PRN_FATAL, "%s create task %s failed!\n", __func__, "VMM_PROCESS");
            goto Error_2;
        }

        VMM_MUTEX_LOCK(&g_vmm_ctx.mutex);

        g_vmm_ctx.is_init = 1;
        g_vmm_ctx.callback = callback;

        VMM_MUTEX_UNLOCK(&g_vmm_ctx.mutex);
#ifdef VFMW_PROC_SUPPORT
        if (OS_PROC_CREATE(VMM_PROC_NAME, vmm_proc_read,
            g_vmm_cmd_list, (sizeof(g_vmm_cmd_list) / sizeof(OS_PROC_CMD))) != VMM_SUCCESS) {
            PRINT(PRN_FATAL, "%s create proc %s failed!\n", __func__, VMM_PROC_NAME);
            goto Error_3;
        }
#endif
    }

    return ret;

#ifdef VFMW_PROC_SUPPORT
Error_3:
#endif
    g_vmm_ctx.stop_thread = 1;

Error_2:
    vmm_mem_list_deinit();

Error_1:
    vmm_msg_list_deinit();

Error_0:
    if (g_vmm_ctx.dbg_info) {
        OS_FREE_VIR(g_vmm_ctx.dbg_info);
        g_vmm_ctx.dbg_info = HI_NULL;
    }

Error:
    g_vmm_ctx.is_init = 0;

    return ret;
}

hi_s32 vmm_core_deinit(hi_void)
{
    hi_s32 ret = VMM_OSAL_SUCCESS;
    mem_data_info mem_data;
    hi_s32 recycle_cnt = 0;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_data, sizeof(mem_data_info), 0, sizeof(mem_data_info)));

    if (g_vmm_ctx.is_init) {
        g_vmm_ctx.stop_thread = 1;

#ifdef VFMW_PROC_SUPPORT
        OS_PROC_DESTROY(VMM_PROC_NAME);
#endif
        while ((g_vmm_ctx.status != THREAD_STATUS_EXIT) && (recycle_cnt < 50)) { /* 50 :a number */
            recycle_cnt++;
            OS_MSLEEP(1);
        }

        if (recycle_cnt == 50) { /* 50 :a number */
            PRINT(PRN_ERROR, "%s,%d,Task stop timeout!! status = %d\n", __func__, __LINE__, g_vmm_ctx.status);
        }

        vmm_msg_list_deinit();

        do {
            ret = vmm_mem_list_takeout_node(&mem_data);
            if (ret == VMM_OSAL_SUCCESS) {
                if (vmm_ops_release(&mem_data.buf_info) != MEM_OPS_SUCCESS) {
                    PRINT(PRN_ERROR,
                          "%s,%d, want to release Mem(Addr = 0x%x, size = %d, Name = '%s') failed!\n",
                          __func__, __LINE__,
                          mem_data.buf_info.start_phy_addr, mem_data.buf_info.size, mem_data.buf_info.buf_name);
                }
            }
        } while (ret == VMM_OSAL_SUCCESS);

        vmm_mem_list_deinit();

        vmm_dbg_deinit();

        VMM_EVENT_EXIT(&g_vmm_ctx.event);
        VMM_MUTEX_EXIT(&g_vmm_ctx.mutex);
        VMM_SPINLOCK_EXIT(&g_vmm_ctx.lock);

        g_vmm_ctx.callback = HI_NULL;
        g_vmm_ctx.is_init = 0;
    }

    return VMM_SUCCESS;
}

hi_void *vmm_core_get_callback_func(hi_void)
{
    hi_void *func = HI_NULL;

    if (g_vmm_ctx.is_init) {
        func = (hi_void *)g_vmm_ctx.callback;
    }

    return func;
}


