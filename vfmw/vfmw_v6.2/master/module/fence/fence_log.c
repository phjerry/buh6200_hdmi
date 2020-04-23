/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "dbg.h"
#include "fence.h"
#include "fence_log.h"
#include "hi_osal.h"

static hi_s32 fence_handle_cmd(cmd_str_ptr p_cmd_str, hi_u32 cmd_len)
{
    hi_u32 value = 0;
    hi_s8 *p_cmd_id = ((*p_cmd_str)[0]);
    fence_entry *entry = fence_get_entry();

    VFMW_ASSERT_RET(cmd_len >= 2, HI_FAILURE); /* 2 :cmd length */

    if (!OS_STRNCMP(p_cmd_id, LOG_CMD_FRM_FENCE, OS_STRLEN(p_cmd_id))) {
        if (str_to_val((*p_cmd_str)[1], &value) != 0) {
            dprint(PRN_ALWS, "error string to value '%s'!\n", (*p_cmd_str)[1]);
            return HI_FAILURE;
        }
        entry->fence_frm_en = value;
        dprint(PRN_ALWS, "set frm fence enable %d\n", entry->fence_frm_en);
    } else if (!OS_STRNCMP(p_cmd_id, LOG_CMD_FRM_SRC_TIM, OS_STRLEN(p_cmd_id))) {
        if (str_to_val((*p_cmd_str)[1], &value) != 0) {
            dprint(PRN_ALWS, "error string to value '%s'!\n", (*p_cmd_str)[1]);
            return HI_FAILURE;
        }
        entry->src_wait_time = value;
        dprint(PRN_ALWS, "set frm fence src time %d ms\n", entry->src_wait_time);
    } else if (!OS_STRNCMP(p_cmd_id, LOG_CMD_FRM_SINK_TIM, OS_STRLEN(p_cmd_id))) {
        if (str_to_val((*p_cmd_str)[1], &value) != 0) {
            dprint(PRN_ALWS, "error string to value '%s'!\n", (*p_cmd_str)[1]);
            return HI_FAILURE;
        }
        entry->sink_wait_time = value;
        dprint(PRN_ALWS, "set frm fence sink time %d ms\n", entry->sink_wait_time);
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 fence_log_read(hi_void *buf, hi_s32 len, hi_u32 *used)
{
    hi_s32 i = 0;
    fence_entry *entry = fence_get_entry();
    fence_chan *chan = HI_NULL;

    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_HEAD, "fence");
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "init", entry->inited, "frm_enable", entry->fence_frm_en);
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D, "fence_num", osal_fence_get_num());
    OS_DUMP_PROC(buf, len, used, 0, "\n");

    for (i = 0 ; i < VFMW_CHAN_NUM; i++) {
        chan = fence_get_chan(i);
        if (chan != HI_NULL) {
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D,
                         "fence_creat", chan->fence_create, "fence_sign", chan->fence_signal);
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D,
                         "fence_wait", chan->fence_wait, "fence_free", chan->fence_free);
        }
    }

    OS_DUMP_PROC(buf, len, used, 0, "\n");

    return HI_SUCCESS;
}

hi_s32 fence_log_write(cmd_str_ptr buffer, hi_u32 count)
{
    return fence_handle_cmd(buffer, count);
}
