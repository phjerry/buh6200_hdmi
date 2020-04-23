/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#include "message.h"
#include "linux/hisilicon/securec.h"
#include <linux/delay.h>

static hi_void message_stop(msg_queue_s *queue)
{
    unsigned long flags;
    struct list_head *list = HI_NULL;
    struct list_head *tmp = HI_NULL;

    omx_print(OMX_TRACE, "msg prepare to stop.\n");

    /* flush msg list */
    osal_spin_lock_irqsave(&queue->lock, &flags);
    queue->stop = 1;
    if (!list_empty(&queue->head)) {
        omx_print(OMX_WARN, "msg queue not empty, flush it!\n");
        list_for_each_safe(list, tmp, &queue->head)
        {
            list_del(list);
            list_add_tail(list, &queue->free);
        }
    }
    osal_spin_unlock_irqrestore(&queue->lock, &flags);

    osal_wait_wakeup(&queue->wait);

    omx_print(OMX_TRACE, "msg stop ok.\n");
}

static hi_s32 message_pending(const void *param)
{
    hi_bool has_msg;
    hi_bool stopped;
    unsigned long flags;
    msg_queue_s *queue = (msg_queue_s *)param;

    if (queue == HI_NULL) {
        omx_print(OMX_FATAL, "queue = HI_NULL.\n");
        return -EINVAL;
    }

    osal_spin_lock_irqsave(&queue->lock, &flags);
    has_msg = !list_empty(&queue->head);
    stopped = (hi_bool)(queue->stop);
    osal_spin_unlock_irqrestore(&queue->lock, &flags);

    return (hi_s32)(has_msg || stopped);
}

hi_s32 message_queue(msg_queue_s *queue, hi_u32 msgcode, hi_u32 status, const hi_void *priv)
{
    unsigned long flags;
    msg_data_s *msg = HI_NULL;

    osal_spin_lock_irqsave(&queue->lock, &flags);
    if (queue->stop) {
        osal_spin_unlock_irqrestore(&queue->lock, &flags);
        omx_print(OMX_FATAL, "msg queue stopped.\n");
        return 0;
    }

    if (list_empty(&queue->free)) {
        osal_spin_unlock_irqrestore(&queue->lock, &flags);
        omx_print(OMX_FATAL, "no free space in list.\n");
        return -ENOMEM;
    }

    msg = list_first_entry(&queue->free, msg_data_s, list);
    list_del(&msg->list);
    msg->msg_info.status_code = status;
    msg->msg_info.msgcode = msgcode;

    if (priv != HI_NULL) {
        if (msgcode == VDEC_EVT_REPORT_IMG_SIZE_CHG) {
            HI_CHECK_SEC_FUNC(memcpy_s(&msg->msg_info.msgdata,  sizeof(msg->msg_info.msgdata),  priv,
                                       sizeof(OMXVDEC_IMG_SIZE)));
        } else if (msgcode == VDEC_EVT_REPORT_DEC_SIZE_CHG) {
            HI_CHECK_SEC_FUNC(memcpy_s(&msg->msg_info.msgdata,  sizeof(msg->msg_info.msgdata),  priv,
                                       sizeof(OMXVDEC_DEC_SIZE)));
        } else if (msgcode == VDEC_EVT_REPORT_HDR_INFO) {
            HI_CHECK_SEC_FUNC(memcpy_s(&msg->msg_info.msgdata,  sizeof(msg->msg_info.msgdata),  priv,
                                       sizeof(OMXVDEC_DEC_COLOR_ASPECTS)));
        } else {
            HI_CHECK_SEC_FUNC(memcpy_s(&msg->msg_info.msgdata,  sizeof(msg->msg_info.msgdata),  priv,
                                       sizeof(OMXVDEC_BUF_DESC)));
        }
    }

    list_add_tail(&msg->list, &queue->head);
    osal_spin_unlock_irqrestore(&queue->lock, &flags);

    osal_wait_wakeup(&queue->wait);

    return 0;
}

hi_s32 message_dequeue(msg_queue_s *queue, OMXVDEC_MSG_INFO *pmsg_info)
{
    unsigned long flags;
    msg_data_s *msg = HI_NULL;
    hi_s32 ret;

    if (queue == HI_NULL) {
        omx_print(OMX_FATAL, "invalid param: queue=HI_NULL(0x%p)\n", queue);
        return -EINVAL;
    }

    if (pmsg_info == HI_NULL) {
        omx_print(OMX_FATAL, "invalid param: pmsg_info=HI_NULL(0x%p)\n", pmsg_info);
        goto shutdown;
    }

    ret = osal_wait_timeout_interruptible(&queue->wait, message_pending, queue, 10); /* wait 10 ms */
    if (queue->stop) {
        omx_print(OMX_WARN, "wait msg failed, msg queue stopped.\n");
        goto shutdown;
    }

    if (ret <= 0) {
        if (ret < 0) {
            omx_print(OMX_WARN, "wait msg interrupted.\n");
        } else {
        }
        return -EAGAIN;
    }

    osal_spin_lock_irqsave(&queue->lock, &flags);
    if (queue->stop) {
        osal_spin_unlock_irqrestore(&queue->lock, &flags);
        omx_print(OMX_INFO, "msg queue stopped.\n");
        goto shutdown;
    }

    if (!list_empty(&queue->head)) {
        msg = list_first_entry(&queue->head, msg_data_s, list);
        list_del(&msg->list);
        HI_CHECK_SEC_FUNC(memcpy_s(pmsg_info, sizeof(OMXVDEC_MSG_INFO), &msg->msg_info, sizeof(OMXVDEC_MSG_INFO)));
        list_add(&msg->list, &queue->free);
    }
    osal_spin_unlock_irqrestore(&queue->lock, &flags);

    return HI_SUCCESS;

shutdown:
    queue->quit = 1;

    return -ESHUTDOWN;
}

msg_queue_s *message_queue_init(hi_u32 max_msg_num)
{
    hi_u32 nqueues;
    msg_queue_s *queue = HI_NULL;
    msg_data_s *data = HI_NULL;

    omx_print(OMX_TRACE, "msg prepare to init.\n");

    queue = hi_vmalloc_omxvdec(sizeof(msg_queue_s));
    if (queue == HI_NULL) {
        omx_print(OMX_FATAL, "alloc for queue failed.\n");
        return HI_NULL;
    }

    HI_CHECK_SEC_FUNC(memset_s(queue, sizeof(msg_queue_s), 0, sizeof(msg_queue_s)));

    osal_spin_lock_init(&queue->lock);
    INIT_LIST_HEAD(&queue->head);
    INIT_LIST_HEAD(&queue->free);
    osal_wait_init(&queue->wait);
    queue->stop = 0;
    queue->quit = 0;

    /* alloc element for seg stream */
    nqueues = max_msg_num;
    data = queue->alloc = hi_vmalloc_omxvdec(sizeof(msg_data_s) * nqueues);
    if (data == HI_NULL) {
        omx_print(OMX_FATAL, "alloc for data failed.\n");
        hi_vfree_omxvdec(queue);
        return HI_NULL;
    }

    HI_CHECK_SEC_FUNC(memset_s(data, sizeof(msg_data_s) * nqueues, 0, sizeof(msg_data_s) * nqueues));

    for (; nqueues; data++, nqueues--) {
        list_add(&data->list, &queue->free);
    }

    queue->msg_num = nqueues;

    omx_print(OMX_TRACE, "msg init ok.\n");

    return queue;
}

hi_void message_queue_deinit(msg_queue_s *queue)
{
    hi_u32 slp_cnt = 0;

    omx_print(OMX_TRACE, "msg prepare to deinit.\n");

    if (queue == HI_NULL) {
        omx_print(OMX_FATAL, "queue = HI_NULL.\n");
        return;
    }

    message_stop(queue);

    while (!queue->quit && slp_cnt < 10) { /* 10 is the comparison value */
        msleep(1);
        slp_cnt++;
    }

    hi_vfree_omxvdec(queue->alloc);
    queue->alloc = HI_NULL;

    hi_vfree_omxvdec(queue);

    omx_print(OMX_TRACE, "msg deinit ok.\n");
}


