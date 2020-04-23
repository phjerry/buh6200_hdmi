/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */


#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "omxvdec.h"

#define QUEUE_DEFAULT_DEPTH 350

typedef struct msg_data {
    struct list_head list;
    /* msg payload */
    OMXVDEC_MSG_INFO msg_info;
} msg_data_s;

typedef struct msg_queue {
    osal_spinlock lock;
    osal_wait wait;

    hi_u8 quit;
    hi_void *alloc;
    hi_u32 msg_num;
    hi_u32 stop : 1;
    struct list_head head;
    struct list_head free;
} msg_queue_s;

hi_s32 message_queue(msg_queue_s *queue, hi_u32 msgcode, hi_u32 status, const hi_void *priv);

hi_s32 message_dequeue(msg_queue_s *queue, OMXVDEC_MSG_INFO *pmsg_info);

msg_queue_s *message_queue_init(hi_u32 max_msg_num);

hi_void message_queue_deinit(msg_queue_s *queue);

#endif
