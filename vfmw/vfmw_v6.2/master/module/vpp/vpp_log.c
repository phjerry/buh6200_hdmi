/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#include "vpp_log.h"
#include "vpp.h"
#include "dbg.h"

static hi_u8 *vpp_show_thread_state(vpp_thread_state type)
{
    switch (type) {
        case THREA_IDLE:
            return "IDLE";
            break;

        case THREA_RUNNING:
            return "RUNNING";
            break;

        case THREA_SLEEP:
            return "SLEEP";
            break;

        default:
            return "INVALID";
    }
}

static hi_s32 vpp_handle_cmd(cmd_str_ptr p_cmd_str, hi_u32 cmd_len)
{
    hi_u32 value = 0;
    hi_s8 *p_cmd_id = ((*p_cmd_str)[0]);
    vpp_entry *entry = vpp_get_entry();

    VFMW_ASSERT_RET(cmd_len >= 2, HI_FAILURE); /* 2 :cmd length */

    /* parse command */
    if (!OS_STRNCMP(p_cmd_id, LOG_CMD_VPP_BYPASS, OS_STRLEN(p_cmd_id))) {
        if (str_to_val((*p_cmd_str)[1], &value) != 0) {
            dprint(PRN_ALWS, "error string to value '%s'!\n", (*p_cmd_str)[1]);
            return HI_FAILURE;
        }
        entry->bypass = value;
        dprint(PRN_ALWS, "set bypass %d\n", entry->bypass);
    } else if (!OS_STRNCMP(p_cmd_id, LOG_CMD_VPP_PR_FRM, OS_STRLEN(p_cmd_id))) {
        if (str_to_val((*p_cmd_str)[1], &value) != 0) {
            dprint(PRN_ALWS, "error string to value '%s'!\n", (*p_cmd_str)[1]);
            return HI_FAILURE;
        }
        entry->print_frm = value;
        dprint(PRN_ALWS, "set print frame %d\n", entry->print_frm);
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpp_log_read(hi_void *buf, hi_s32 len, hi_u32 *used)
{
    hi_s32 i = 0;
    vpp_entry *entry = vpp_get_entry();
    vpp_chan *chan = HI_NULL;

    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_HEAD, "vpp");
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_S, "init", entry->inited, "thread_state",
                 vpp_show_thread_state(entry->thread_state));
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "bypass", entry->bypass, "print_frm", entry->print_frm);
    OS_DUMP_PROC(buf, len, used, 0, "\n");

    for (i = 0 ; i < VFMW_CHAN_NUM; i++) {
        chan = vpp_get_chan(i);
        if (chan != HI_NULL) {
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan->chan_id, "state", chan->state);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "work_mode", chan->work_mode, "source", chan->source);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "err_thr", chan->err_thr, "dec_mode", chan->dec_mode);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "pvr_tmp_frm", chan->pvr_info.tmp_list_pos,
                         "pvr_final_frm", chan->pvr_info.final_frame_num);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D, "speed", chan->pvr_info.speed);

            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "recv_try", chan->receive_try, "recv_ok", chan->receive_ok);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "rels_try", chan->release_try, "rels_ok", chan->release_ok);
        }
    }

    OS_DUMP_PROC(buf, len, used, 0, "\n");

    return HI_SUCCESS;
}

hi_s32 vpp_log_write(cmd_str_ptr buffer, hi_u32 count)
{
    return vpp_handle_cmd(buffer, count);
}
