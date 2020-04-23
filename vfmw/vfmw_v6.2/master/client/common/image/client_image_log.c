/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "client_image_log.h"
#include "client_image.h"
#include "vfmw_osal.h"

hi_s32 clt_img_log_read(hi_void *buffer, hi_s32 chan_id)
{
    hi_s32 used_size = 0;
    hi_s32 i;
    clt_img_chan *chan = clt_img_get_chan_entry(chan_id);

    OS_DUMP_PROC(buffer, 0, &used_size, 0, "  ----------------------SHR IMG--------------------------\n");

    if (chan->list != HI_NULL) {
        OS_DUMP_PROC(buffer, 0, &used_size, 0, "  head_c/head_s     :(%d/%d)\n",
                     chan->list->head_c, chan->list->head_s);

        OS_DUMP_PROC(buffer, 0, &used_size, 0, "  Frm rate     :%d\n", chan->frm_rate);

        OS_DUMP_PROC(buffer, 0, &used_size, 0, "  ------------------VALID image----------------------\n");

        OS_DUMP_PROC(buffer, 0, &used_size, 0, "  %-6s %-6s %-6s %-6s  %-10s \n",
                     "Pos", "Top", "Bot", "FldVld", "DecAddr");

        for (i = chan->list->head_c; i < chan->list->head_c + SHR_IMG_QUEUE_SIZE; i++) {
            OS_DUMP_PROC(buffer, 0, &used_size, 0, "  %-6d %-6d %-6d %-6d 0x%-10x \n",
                         i % SHR_IMG_QUEUE_SIZE,
                         chan->list->img_que[i % SHR_IMG_QUEUE_SIZE].img.dec_over.bits.top_over,
                         chan->list->img_que[i % SHR_IMG_QUEUE_SIZE].img.dec_over.bits.bot_over,
                         chan->list->img_que[i % SHR_IMG_QUEUE_SIZE].img.format.field_valid_flag,
                         chan->list->img_que[i % SHR_IMG_QUEUE_SIZE].img.disp_info.luma_phy_addr);
            if ((i % SHR_IMG_QUEUE_SIZE) == chan->list->head_s) {
                break;
            }
        }
    }
    OS_DUMP_PROC(buffer, 0, &used_size, 0, "\n");

    return HI_SUCCESS;
}


