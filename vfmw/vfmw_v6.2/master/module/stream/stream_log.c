/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "stream_log.h"
#include "stream.h"
#include "vfmw_proc.h"
#include "dbg.h"

typedef struct {
    hi_bool print_block;
} stream_log_entry;

static stream_log_entry g_stm_log_entry = {0};
static hi_char *g_stream_mode[2] = {"outer", "inner"}; /* 2 :a number */
static hi_char *g_block_status[3] = {"write", "free", "read"}; /* 3 :a number */

static stream_log_entry *stream_log_get_entry(hi_void)
{
    return &g_stm_log_entry;
}

static hi_void stream_log_set_print_block(cmd_str_ptr buffer, hi_u32 count)
{
    hi_s32 print_block = 0;
    stream_log_entry *entry = stream_log_get_entry();

    str_to_val((*buffer)[1], &print_block);
    entry->print_block = (hi_bool)print_block;
    dprint(PRN_ALWS, "set stream print block %d\n", entry->print_block);
}


hi_s32 stream_log_read(hi_void *buf, hi_s32 len, hi_u32 *used)
{
    hi_s32 chan_id = 0;
    stream_block *block = HI_NULL;
    stream_log_entry *entry = stream_log_get_entry();
    stream_chan *chan = HI_NULL;

    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_HEAD, "stream");
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D, "pr_block", entry->print_block);
    OS_DUMP_PROC(buf, len, used, 0, "\n");

    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        chan = stream_get_chan(chan_id);
        if (chan->is_used) {
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_S, "chan_id", chan_id, "mode", g_stream_mode[chan->mode]);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_LX_X, "phy_addr", chan->start_phy_addr, "length", chan->length);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_LX_LX, "kern_addr", chan->start_kern_vir_addr, "usr_addr",
                         chan->start_usr_vir_addr);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "write_offst", chan->write_offst, "read_offst",
                         chan->read_offst);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "free_len", chan->free_len, "used_len", chan->used_len);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_D, "freeze", chan->freeze_size, "sec", chan->is_sec);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "try_read", chan->try_read_cnt, "read_suc",
                         chan->read_ok_cnt);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "try_rel", chan->try_release_cnt, "rel_suc",
                         chan->release_ok_cnt);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "try_deq", chan->try_dequeue_cnt, "deq_suc",
                         chan->dequeue_ok_cnt);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "try_que", chan->try_queue_cnt, "que_suc",
                         chan->queue_ok_cnt);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "eos", chan->eos_flag, "valid_pts", chan->valid_pts_flag);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "max_raw_num", chan->max_raw_num, "raw_num", chan->raw_num);

            if (entry->print_block) {
                OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_HEAD, "stream_block");
                OS_LIST_FOR_EACH_ENTRY(block, &chan->list, node) {
                    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_S, "phy_addr", block->buf.phy_addr, "status",
                                 g_block_status[block->status]);
                }
            }
        }
    }

    OS_DUMP_PROC(buf, len, used, 0, "\n");

    return HI_SUCCESS;
}

hi_s32 stream_log_write(cmd_str_ptr buffer, hi_u32 count)
{
    hi_s32 i = 0;
    hi_s8 *cmd_id = ((*buffer)[0]);
    cmd_handler handler = HI_NULL;

    static const log_cmd com_cmd[] = {
        { LOG_CMD_PRT_STM_BLK, stream_log_set_print_block },
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
