/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "usdpool_log.h"
#include "usdpool.h"
#include "vfmw_proc.h"
#include "dbg.h"

typedef struct {
    hi_bool print_block;
} usdpool_log_entry;

static usdpool_log_entry g_usdpool_log_entry = {0};
static hi_char *g_block_status[3] = {"write", "free", "read"}; /* 3 is state number */

static usdpool_log_entry *usdpool_log_get_entry(hi_void)
{
    return &g_usdpool_log_entry;
}

static hi_void usdpool_log_set_print_block(cmd_str_ptr buffer, hi_u32 count)
{
    hi_s32 print_block = 0;
    usdpool_log_entry *entry = usdpool_log_get_entry();

    str_to_val((*buffer)[1], &print_block);
    entry->print_block = (hi_bool)print_block;
    dprint(PRN_ALWS, "set usdpool print block %d\n", entry->print_block);
}

static hi_void usdpool_log_print_block(hi_void *buf, hi_s32 len, hi_u32 *used, usrdat_pool *pool)
{
    usdpool_log_entry *entry = usdpool_log_get_entry();
    usrdat_block *block = HI_NULL;

    if (entry->print_block) {
        OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_HEAD, "usdpool_block");
        OS_LIST_FOR_EACH_ENTRY(block, &pool->list, node) {
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_S, "offset", block->usr_data.usd_offset, "state",
                         g_block_status[block->state]);
        }
    }
}

hi_s32 usdpool_log_read(hi_void *buf, hi_s32 len, hi_u32 *used)
{
    hi_s32 chan_id = 0;
    hi_s32 i = 0;
    usdpool_log_entry *entry = usdpool_log_get_entry();
    usrdat_chan *chan = HI_NULL;
    usrdat_pool *pool = HI_NULL;

    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_HEAD, "usdpool");
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D, "pr_block", entry->print_block);
    OS_DUMP_PROC(buf, len, used, 0, "\n");

    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        chan = usrdat_get_chan(chan_id);
        if (chan == HI_NULL) {
            continue;
        }

        OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "chan_id", chan_id, "usd_num", chan->total_usd_num);
        OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "try_acq", chan->try_acq_cnt, "acq_ok", chan->acq_ok_cnt);
        OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "try_rls", chan->try_rls_cnt, "rls_ok", chan->rls_ok_cnt);
        OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "try_chk", chan->try_chk_cnt, "chk_ok", chan->chk_ok_cnt);
        OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "write_index", chan->check_usr.write_index,
                     "read_index", chan->check_usr.read_index);
        for (i = 0; i < USRDAT_TYPE_NUM; i++) {
            pool = &chan->pool[i];
            if (pool->start_phy_addr != 0) {
                OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "type", pool->type, "usr_num", pool->usrdata_num);
                OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_LX_LX, "phy_addr", pool->start_phy_addr, "kern_addr",
                             pool->start_vir_addr);
                OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "write_offst", pool->write_offset, "read_offst",
                             pool->read_offset);
                OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "free_len", pool->free_len, "length", pool->length);
                OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X, "freeze", pool->freeze_len);

                usdpool_log_print_block(buf, len, used, pool);
            }
        }
    }

    OS_DUMP_PROC(buf, len, used, 0, "\n");

    return HI_SUCCESS;
}

hi_s32 usdpool_log_write(cmd_str_ptr buffer, hi_u32 count)
{
    hi_s32 i = 0;
    hi_s8 *cmd_id = ((*buffer)[0]);
    cmd_handler handler = HI_NULL;

    static const log_cmd com_cmd[] = {
        { LOG_CMD_PRT_USD_BLK, usdpool_log_set_print_block },
    };

    for (i = 0; i < sizeof(com_cmd) / sizeof(log_cmd); i++) {
        if (!OS_STRNCMP(cmd_id, com_cmd[i].cmd_name, OS_STRLEN(cmd_id))) {
            handler = com_cmd[i].handler;
            break;
        }
    }

    if (handler == HI_NULL) {
        return HI_FAILURE;
    }

    handler(buffer, count);

    return HI_SUCCESS;
}
