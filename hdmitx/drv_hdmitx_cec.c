/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: CEC framework.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-10-29
 */

#include "drv_hdmitx_cec.h"
#include <linux/hisilicon/securec.h>
#include "hi_errno.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"

#define TIME_BASE_NS 1000000
#define TIME_BASE_MS 1000
#define TIME_BASE_S  60
#define TIME_BASE_M  60
#define TIME_BASE_H  24
#define TIME_STR_MAX_LEN 15

#define cec_dbg_print(fmt...) hi_drv_proc_echo_helper(fmt)
#define cec_dbg_err(fmt...) cec_dbg_print("[CEC DBG ERROR]: %s [%d],", __func__, __LINE__); cec_dbg_print(fmt)

static const hi_u8 g_playback_logic_addrs[] = {
    CEC_LOGIC_ADDR_PLAYBACK_1,
    CEC_LOGIC_ADDR_PLAYBACK_2,
    CEC_LOGIC_ADDR_PLAYBACK_3,
    CEC_INVALID_LOGICAL_ADDR,
};

static const hi_u8 g_tuner_logic_addrs[] = {
    CEC_LOGIC_ADDR_TUNER_1,
    CEC_LOGIC_ADDR_TUNER_2,
    CEC_LOGIC_ADDR_TUNER_3,
    CEC_LOGIC_ADDR_TUNER_4,
    CEC_INVALID_LOGICAL_ADDR,
};

static const hi_char *g_logic_addr_status_name[] = {
    "valid",
    "invalid",
    "not config",
    "configuring",
};

static const hi_char *g_device_type_name[] = {
    "tv",
    "record",
    "reserve",
    "tuner",
    "playback",
    "audio system",
    "switch",
    "processor"
};

static const hi_u8 g_default_vendor_id[CEC_VENDOR_ID_SIZE] = { 0x04, 0x75, 0x03 };

static hi_char *reuslt2name(hi_s32 result)
{
    hi_char *name = "error";

    name = (result == HI_SUCCESS) ? "success" :
        (result == HI_ERR_HDMITX_MSG_NACK) ? "nack" : "failed";
    return name;
}

static hi_char *get_time_str(hi_u64 timestamp)
{
    static hi_char str[TIME_STR_MAX_LEN];
    hi_u64 val;
    hi_u32 h, m, s, ms;

    ms = timestamp % TIME_BASE_MS;

    val = timestamp / TIME_BASE_MS;
    s = val % TIME_BASE_S;

    val /= TIME_BASE_S;
    m = val % TIME_BASE_M;

    val /= TIME_BASE_M;
    h = val % TIME_BASE_H;

    if (memset_s(str, sizeof(str), 0, sizeof(str))) {
        return "error";
    }
    if (snprintf_s(str, sizeof(str), TIME_STR_MAX_LEN, "%02d.%02d.%02d.%03d", h, m, s, ms) == HI_FAILURE) {
        return "error";
    }
    return str;
}

static cec_device_type get_device_type(hi_u8 logic_addr)
{
    int i;

    if (logic_addr == CEC_INVALID_LOGICAL_ADDR) {
        return CEC_DEVICE_TYPE_MAX;
    }

    for (i = 0; g_playback_logic_addrs[i] != CEC_INVALID_LOGICAL_ADDR; i++) {
        if (logic_addr == g_playback_logic_addrs[i]) {
            return CEC_DEVICE_TYPE_PLAYBACK;
        }
    }

    for (i = 0; g_tuner_logic_addrs[i] != CEC_INVALID_LOGICAL_ADDR; i++) {
        if (logic_addr == g_tuner_logic_addrs[i]) {
            return CEC_DEVICE_TYPE_TUNER;
        }
    }

    return CEC_DEVICE_TYPE_MAX;
}

static hi_u8 *get_addr_set(cec_device_type device_type)
{
    hi_u8 *addr_set = HI_NULL;

    switch (device_type) {
        case CEC_DEVICE_TYPE_PLAYBACK:
            addr_set = (hi_u8 *)g_playback_logic_addrs;
            break;
        case CEC_DEVICE_TYPE_TUNER:
            addr_set = (hi_u8 *)g_tuner_logic_addrs;
            break;
        default :
            break;
    }
    return addr_set;
}

static hi_u64 get_time_ms(hi_void)
{
    hi_u64 sys_time;

    sys_time = osal_sched_clock();

    return osal_div_u64(sys_time, TIME_BASE_NS);
}

static hi_void save_vendor_id(struct drv_hdmitx_cec_device *cec, const cec_msg *msg)
{
    /* data[4:2] is vendor id */
    if (memcpy_s(cec->vendor_id, sizeof(cec->vendor_id), &msg->data[2], sizeof(cec->vendor_id))) {
        cec_err("cec%d-memcpy_s failed\n", cec->id);
    }
}

static hi_void save_osd_name(struct drv_hdmitx_cec_device *cec, const cec_msg *msg)
{
    if (memset_s(cec->osd_name, sizeof(cec->osd_name), 0, sizeof(cec->osd_name))) {
        cec_err("cec%d-memset_s failed\n", cec->id);
        return;
    }
    if (memcpy_s(cec->osd_name, sizeof(cec->osd_name), &msg->data[2], msg->len - 2)) { /* data[15:2] is osd name */
        cec_err("cec%d-memcpy_s failed\n", cec->id);
    }
}

static hi_s32 wait_read_event(const void *param)
{
    struct drv_hdmitx_cec_device *cec = (struct drv_hdmitx_cec_device *)param;

    if ((cec->open == HI_FALSE) ||
        cec->event_manage.status_change_cnt ||
        cec->event_manage.received_cnt) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_void event_init(struct cec_event_manage *event_manage)
{
    osal_wait_init(&event_manage->wait);
    event_manage->status_change_cnt = 0;
    OSAL_INIT_LIST_HEAD(&event_manage->received);
    event_manage->received_cnt = 0;
}

static hi_void event_write(struct cec_event_manage *event_manage, const cec_events *event)
{
    struct drv_hdmitx_cec_device *cec = osal_container_of(event_manage, struct drv_hdmitx_cec_device, event_manage);
    struct cec_event_received_entry *entry = HI_NULL;
    hi_u32 id;

    id = cec->id;
    if (event->type == CEC_EVENT_STATUS_CHANGE) {
        event_manage->status_change.logical_addr = event->data.status.logical_addr;
        event_manage->status_change.physical_addr = event->data.status.physical_addr;
        event_manage->status_change.logic_addr_status = event->data.status.logic_addr_status;
        event_manage->status_change_cnt = 1;
        osal_wait_wakeup(&event_manage->wait);
    } else if (event->type == CEC_EVENT_RECEIVED) {
        entry = osal_vmalloc(HI_ID_HDMITX, sizeof(*entry));
        if (entry == HI_NULL) {
            cec_err("cec%d-vmalloc failed\n", id);
            return;
        }
        if (memcpy_s(&entry->rx_msg, sizeof(entry->rx_msg), &event->data.rx_msg, sizeof(entry->rx_msg))) {
            osal_vfree(HI_ID_HDMITX, entry);
            cec_err("cec%d-memcpy_s failed\n", id);
            return;
        }
        osal_list_add_tail(&entry->list, &event_manage->received);
        event_manage->received_cnt++;

        /* if the message queue is full, then drop the oldest one */
        if (event_manage->received_cnt > CEC_EVENT_QUEUE_RECEIVED_MAX_SZ) {
            event_manage->received_cnt = CEC_EVENT_QUEUE_RECEIVED_MAX_SZ;
            entry = osal_list_first_entry(&event_manage->received, struct cec_event_received_entry, list);
            osal_list_del(&entry->list);
            cec_warn("cec%d-queue full, drop msg: %*ph\n", id, entry->rx_msg.len, entry->rx_msg.data);
            osal_vfree(HI_ID_HDMITX, entry);
        }
        osal_wait_wakeup(&event_manage->wait);
    }
}

static hi_s32 event_read(struct cec_event_manage *event_manage, cec_events *event)
{
    struct cec_event_received_entry *entry = HI_NULL;

    if (event_manage->status_change_cnt) {
        event_manage->status_change_cnt = 0;
        event->type = CEC_EVENT_STATUS_CHANGE;
        event->data.status.logical_addr = event_manage->status_change.logical_addr;
        event->data.status.physical_addr = event_manage->status_change.physical_addr;
        event->data.status.logic_addr_status = event_manage->status_change.logic_addr_status;
        return HI_SUCCESS;
    }

    if (!osal_list_empty(&event_manage->received)) {
        event_manage->received_cnt--;
        entry = osal_list_first_entry(&event_manage->received, struct cec_event_received_entry, list);
        event->type = CEC_EVENT_RECEIVED;
        if (memcpy_s(&event->data.rx_msg, sizeof(event->data.rx_msg), &entry->rx_msg, sizeof(event->data.rx_msg))) {
            cec_err("memcpy_s failed\n");
            return HI_FAILURE;
        }
        osal_list_del(&entry->list);
        osal_vfree(HI_ID_HDMITX, entry);
        return HI_SUCCESS;
    }

    event->type = CEC_EVENT_MAX;
    return HI_FAILURE;
}

static hi_void event_release(struct cec_event_manage *event_manage)
{
    struct drv_hdmitx_cec_device *cec = osal_container_of(event_manage, struct drv_hdmitx_cec_device, event_manage);
    struct cec_event_received_entry *entry = HI_NULL;
    hi_u32 id;

    id = cec->id;
    while (!osal_list_empty(&event_manage->received)) {
        entry = osal_list_first_entry(&event_manage->received, struct cec_event_received_entry, list);
        osal_list_del(&entry->list);
        cec_info("cec%d-drop msg: %*ph\n", id, entry->rx_msg.len, entry->rx_msg.data);
        osal_vfree(HI_ID_HDMITX, entry);
    }
    event_manage->status_change_cnt = 0;
    event_manage->received_cnt = 0;
    osal_wait_wakeup(&event_manage->wait);
}

static hi_void history_init(struct cec_msg_history *history)
{
    history->msg_cnt = 0;
    OSAL_INIT_LIST_HEAD(&history->head);
}

static hi_void history_write(struct cec_msg_history *history, const struct cec_msg_element *element)
{
    struct drv_hdmitx_cec_device *cec = osal_container_of(history, struct drv_hdmitx_cec_device, history);
    struct cec_msg_history_entry *entry = HI_NULL;
    hi_u32 id;

    id = cec->id;

    if (!element->recevie && (element->tx_result != CEC_TX_RESULT_MAX)) {
        osal_list_for_each_entry_reverse(entry, &history->head, list) {
            if (!memcmp(&entry->element.msg, &element->msg, sizeof(element->msg))) {
                entry->element.timestamp_finish = element->timestamp_finish;
                entry->element.tx_result = element->tx_result;
                return;
            }
        }
    } else {
        entry = osal_vmalloc(HI_ID_HDMITX, sizeof(*entry));
        if (entry == HI_NULL) {
            cec_err("cec%d-vmalloc failed\n", id);
            return;
        }
        if (memcpy_s(&entry->element, sizeof(entry->element), element, sizeof(*element))) {
            osal_vfree(HI_ID_HDMITX, entry);
            cec_err("cec%d-memcpy_s failed\n", id);
            return;
        }
        osal_list_add_tail(&entry->list, &history->head);
        history->msg_cnt++;

        /* if the queue is full, then drop the oldest one */
        if (history->msg_cnt > CEC_HISTORY_QUEUE_MAX_SZ) {
            history->msg_cnt = CEC_HISTORY_QUEUE_MAX_SZ;
            entry = osal_list_first_entry(&history->head, struct cec_msg_history_entry, list);
            osal_list_del(&entry->list);
            osal_vfree(HI_ID_HDMITX, entry);
        }
    }
}

static hi_void history_write_alias(struct cec_msg_history *history,
    hi_bool receive, hi_u64 timestamp, cec_transmit_result tx_result, const cec_msg *msg)
{
    struct cec_msg_element element;

    element.recevie = receive;
    element.timestamp_start = timestamp;
    element.timestamp_finish = timestamp;
    element.tx_result = tx_result;
    if (memcpy_s(&element.msg, sizeof(element.msg), msg, sizeof(element.msg))) {
        cec_err("memcpy_s failed\n");
        return;
    }
    history_write(history, &element);
}

static hi_void history_release(struct cec_msg_history *history)
{
    struct cec_msg_history_entry *entry = HI_NULL;

    while (!osal_list_empty(&history->head)) {
        entry = osal_list_first_entry(&history->head, struct cec_msg_history_entry, list);
        osal_list_del(&entry->list);
        osal_vfree(HI_ID_HDMITX, entry);
    }
    history->msg_cnt = 0;
}

static hi_void change_status(struct drv_hdmitx_cec_device *cec, cec_status status)
{
    hi_bool change = HI_FALSE;
    cec_events event;
    const hi_char *old_name = HI_NULL;
    const hi_char *new_name = HI_NULL;

    old_name = cec->status.logic_addr_status > CEC_LOGIC_ADDR_CONFIGURING ?
        "error" : g_logic_addr_status_name[cec->status.logic_addr_status];
    new_name = status.logic_addr_status > CEC_LOGIC_ADDR_CONFIGURING ?
        "error" : g_logic_addr_status_name[status.logic_addr_status];

    if (cec->status.logical_addr != status.logical_addr) {
        cec_info("cec%d-logic_addr change, old(%d)->new(%d)\n",
            cec->id, cec->status.logical_addr, status.logical_addr);
        cec->status.logical_addr = status.logical_addr;
        hal_cec_set_logic_addr(&cec->hal, status.logical_addr);
        change = HI_TRUE;
    }

    if (cec->status.physical_addr != status.physical_addr) {
        cec_info("cec%d-physic_addr change, old(%x)->new(%x)\n",
            cec->id, cec->status.physical_addr, status.physical_addr);
        cec->status.physical_addr = status.physical_addr;
        change = HI_TRUE;
    }

    if (cec->status.logic_addr_status != status.logic_addr_status) {
        cec_info("cec%d-addr_status change, old(%s)->new(%s)\n", cec->id, old_name, new_name);
        cec->status.logic_addr_status = status.logic_addr_status;
        change = HI_TRUE;
    }

    if (change) {
        event.type = CEC_EVENT_STATUS_CHANGE;
        event.data.status.logical_addr = cec->status.logical_addr;
        event.data.status.physical_addr = cec->status.physical_addr;
        event.data.status.logic_addr_status = cec->status.logic_addr_status;
        event_write(&cec->event_manage, &event);
    }
}

static hi_s32 check_msg(struct cec_transmit_manage *tx_manage, const cec_msg *msg)
{
    struct drv_hdmitx_cec_device *cec = osal_container_of(tx_manage, struct drv_hdmitx_cec_device, tx_manage);

    if ((msg->len == 0) || (msg->len > CEC_MAX_MSG_SIZE)) {
        cec_err("cec%d-invalid msg len(%x)\n", cec->id, msg->len);
        return HI_FAILURE;
    }

    if (cec->status.logical_addr == CEC_INVALID_LOGICAL_ADDR) {
        if (cec_initiator(msg) != CEC_LOGIC_ADDR_UNREGISTERED) {
            cec_err("cec%d-invalid initiator(%x)\n", cec->id, cec_initiator(msg));
            return HI_FAILURE;
        }
    } else {
        if (cec_initiator(msg) != cec->status.logical_addr) {
            cec_err("cec%d-invalid initiator(%x)\n", cec->id, cec_initiator(msg));
            return HI_FAILURE;
        }
    }

    if ((msg->len == 1) && (cec_destination(msg) == CEC_LOGIC_ADDR_BROADCAST)) {
        cec_err("cec%d-invalid destination(%x)\n", cec->id, cec_destination(msg));
        return HI_FAILURE;
    }

    if (cec_destination(msg) == cec->status.logical_addr) {
        cec_err("cec%d-invalid destination(%x)\n", cec->id, cec_destination(msg));
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 wait_transmit(const hi_void *param)
{
    struct drv_hdmitx_cec_device *cec = osal_container_of(param, struct drv_hdmitx_cec_device, tx_manage);

    if ((cec->tx_manage.state == CEC_SEND_STATE_END) || (cec->open == HI_FALSE)) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_void transmit_timeout(struct work_struct *work)
{
    struct cec_transmit_manage *tx_manage = osal_container_of(work, struct cec_transmit_manage, work.work);
    struct drv_hdmitx_cec_device *cec = osal_container_of(tx_manage, struct drv_hdmitx_cec_device, tx_manage);

    osal_mutex_lock(&cec->tx_manage.lock);
    if ((cec->tx_manage.state == CEC_SEND_STATE_NEED_SEND) ||
        (cec->tx_manage.state == CEC_SEND_STATE_WAIT_END)) {
        hal_cec_stop_transmit(&cec->hal);
        cec->tx_manage.result = CEC_TX_RESULT_FAILED;
        cec->tx_manage.state = CEC_SEND_STATE_END;
        osal_wait_wakeup(&cec->tx_manage.wait);
    }
    osal_mutex_unlock(&cec->tx_manage.lock);
}

static hi_void transmit_init(struct cec_transmit_manage *tx_manage)
{
    osal_mutex_init(&tx_manage->lock);
    osal_mutex_init(&tx_manage->lock_multi_process);
    osal_wait_init(&tx_manage->wait);
    INIT_DELAYED_WORK(&tx_manage->work, transmit_timeout);
    tx_manage->retry_times = CEC_DEFAULT_RETRY_TIMES;
    tx_manage->result = CEC_TX_RESULT_MAX;
    tx_manage->state = CEC_SEND_STATE_IDLE;
}

static hi_s32 transmit_msg(struct cec_transmit_manage *tx_manage, const cec_msg *msg, hi_u32 retry_times)
{
    hi_s32 ret;
    hi_u64 timestamp;
    struct drv_hdmitx_cec_device *cec = osal_container_of(tx_manage, struct drv_hdmitx_cec_device, tx_manage);
    hi_char *name = HI_NULL;

    osal_mutex_lock(&tx_manage->lock_multi_process);
    osal_mutex_lock(&tx_manage->lock);
    if (memcpy_s(&tx_manage->msg, sizeof(tx_manage->msg), msg, sizeof(tx_manage->msg))) {
        osal_mutex_unlock(&tx_manage->lock);
        osal_mutex_unlock(&tx_manage->lock_multi_process);
        cec_err("cec%d-memcpy_s failed\n", cec->id);
        return HI_FAILURE;
    }
    tx_manage->retry_times = (retry_times > CEC_DEFAULT_RETRY_TIMES) ? CEC_DEFAULT_RETRY_TIMES : retry_times;
    tx_manage->result = CEC_TX_RESULT_MAX;
    tx_manage->state = CEC_SEND_STATE_NEED_SEND;
    osal_mutex_unlock(&tx_manage->lock);

    osal_wait_interruptible(&tx_manage->wait, wait_transmit, (hi_void *)tx_manage);

    osal_mutex_lock(&tx_manage->lock);
    switch (tx_manage->result) {
        case CEC_TX_RESULT_ACK:
            ret = HI_SUCCESS;
            break;
        case CEC_TX_RESULT_NACK:
            ret = HI_ERR_HDMITX_MSG_NACK;
            break;
        default:
            ret = HI_ERR_HDMITX_MSG_FAILED;
            break;
    }
    tx_manage->state = CEC_SEND_STATE_IDLE;
    timestamp = get_time_ms();
    history_write_alias(&cec->history, HI_FALSE, timestamp, tx_manage->result, &tx_manage->msg);
    osal_mutex_unlock(&tx_manage->lock);
    osal_mutex_unlock(&tx_manage->lock_multi_process);
    name = reuslt2name(ret);
    cec_info("cec%d-tx msg:%*ph(%s)\n", cec->id, tx_manage->msg.len, &tx_manage->msg.data[0], name);
    return ret;
}

static hi_void transmit_cancal(struct cec_transmit_manage *tx_manage)
{
    struct drv_hdmitx_cec_device *cec = osal_container_of(tx_manage, struct drv_hdmitx_cec_device, tx_manage);

    while (1) {
        if (osal_mutex_trylock(&tx_manage->lock_multi_process)) {
            osal_mutex_unlock(&tx_manage->lock_multi_process);
            break;
        }

        osal_mutex_lock(&tx_manage->lock);
        if (tx_manage->state == CEC_SEND_STATE_NEED_SEND) {
            tx_manage->result = CEC_TX_RESULT_FAILED;
            tx_manage->state = CEC_SEND_STATE_END;
        } else if (tx_manage->state == CEC_SEND_STATE_WAIT_END) {
            hal_cec_stop_transmit(&cec->hal);
            tx_manage->result = CEC_TX_RESULT_FAILED;
            tx_manage->state = CEC_SEND_STATE_END;
            osal_mutex_unlock(&tx_manage->lock);
            cancel_delayed_work_sync(&tx_manage->work);
            osal_mutex_lock(&tx_manage->lock);
        }
        osal_mutex_unlock(&tx_manage->lock);

    osal_wait_wakeup(&tx_manage->wait);
    }
}

static hi_s32 msg_polling(struct cec_transmit_manage *tx_manage, hi_u8 initiator, hi_u8 destination,
    hi_u32 retry_times)
{
    cec_msg msg = {};

    msg.len = 1; /* the length of <Polling Message> is 1 */
    msg.data[0] = cec_msg_header(initiator, destination);
    return transmit_msg(tx_manage, &msg, retry_times);
}

static hi_s32 msg_report_physical_addr(struct drv_hdmitx_cec_device *cec)
{
    cec_msg msg = {};
    hi_u8 *data = msg.data;

    *data++ = cec_msg_header(cec->status.logical_addr, CEC_LOGIC_ADDR_BROADCAST);
    *data++ = CEC_OPCODE_REPORT_PHYADDR;
    *data++ = cec_physic_addr_ab(cec->status.physical_addr);
    *data++ = cec_physic_addr_cd(cec->status.physical_addr);
    *data++ = cec->device_type;
    msg.len = 5; /* the lenght of <Report physical address> is 5 */
    return transmit_msg(&cec->tx_manage, &msg, CEC_DEFAULT_RETRY_TIMES);
}

static hi_s32 ping_thread(hi_void *data)
{
    struct cec_ping_manage *ping_manage = (struct cec_ping_manage *)data;
    struct drv_hdmitx_cec_device *cec = osal_container_of(ping_manage, struct drv_hdmitx_cec_device, ping_manage);
    hi_u8 last_logic_addr = CEC_INVALID_LOGICAL_ADDR;
    hi_u8 logic_addr = CEC_INVALID_LOGICAL_ADDR;
    cec_logic_addr_status logic_addr_status = CEC_LOGIC_ADDR_INVALID;
    hi_u8 *logic_addr_set = HI_NULL;
    cec_status status;
    hi_s32 ret;
    hi_u8 i;

    if (cec->status.physical_addr == CEC_INVALID_PHYSICAL_ADDR) {
        goto ping_end;
    }

    hal_cec_get_backup_logic_addr(&cec->hal, &last_logic_addr);
    if (get_device_type(last_logic_addr) == cec->device_type) {
        ret = msg_polling(&cec->tx_manage, last_logic_addr, last_logic_addr, CEC_DEFAULT_RETRY_TIMES);
        if (ret == HI_ERR_HDMITX_MSG_NACK) {
            logic_addr = last_logic_addr;
            logic_addr_status = CEC_LOGIC_ADDR_VALID;
            goto ping_end;
        }
    }

    logic_addr_set = get_addr_set(cec->device_type);
    for (i = 0; (logic_addr_set != HI_NULL) && (logic_addr_set[i] != CEC_INVALID_LOGICAL_ADDR); i++) {
        if (ping_manage->cancel) {
            goto cancel;
        }
        if (logic_addr_set[i] == last_logic_addr) {
            continue;
        }
        ret = msg_polling(&cec->tx_manage, logic_addr_set[i], logic_addr_set[i], CEC_DEFAULT_RETRY_TIMES);
        if (ret == HI_ERR_HDMITX_MSG_NACK) {
            logic_addr = logic_addr_set[i];
            logic_addr_status = CEC_LOGIC_ADDR_VALID;
            break;
        }
    }

ping_end:
    if (ping_manage->cancel) {
        goto cancel;
    }

    if (cec->status.physical_addr == CEC_INVALID_PHYSICAL_ADDR) {
        logic_addr = CEC_INVALID_LOGICAL_ADDR;
        logic_addr_status = CEC_LOGIC_ADDR_INVALID;
    }
    status.physical_addr = cec->status.physical_addr;
    status.logical_addr = logic_addr;
    status.logic_addr_status = logic_addr_status;
    osal_mutex_lock(&cec->lock);
    change_status(cec, status);
    osal_mutex_unlock(&cec->lock);

    if (logic_addr != CEC_INVALID_LOGICAL_ADDR) {
        hal_cec_backup_logic_addr(&cec->hal, logic_addr);
        (hi_void)msg_report_physical_addr(cec);
    }

cancel:
    osal_mutex_lock(&cec->lock);
    ping_manage->cancel = HI_FALSE;
    osal_kthread_destroy(ping_manage->thread, HI_FALSE);
    ping_manage->thread = HI_NULL;
    osal_mutex_unlock(&cec->lock);
    return HI_SUCCESS;
}

static hi_void ping_init(struct cec_ping_manage *ping_manage)
{
    ping_manage->thread = HI_NULL;
    ping_manage->cancel = HI_FALSE;
}

static hi_void ping_cancel(struct cec_ping_manage *ping_manage)
{
    struct drv_hdmitx_cec_device *cec = osal_container_of(ping_manage, struct drv_hdmitx_cec_device, ping_manage);

    if (ping_manage->thread != HI_NULL) {
        ping_manage->cancel = HI_TRUE;
        transmit_cancal(&cec->tx_manage);
        osal_mutex_unlock(&cec->lock);
        /* wait thread exit */
        while (ping_manage->thread != HI_NULL) {
            osal_msleep(1); /* check every 1ms */
        }
        osal_mutex_lock(&cec->lock);
        cec_info("cec%d-ping cancel\n", cec->id);
    }
}

static hi_s32 ping_start(struct cec_ping_manage *ping_manage)
{
    hi_char thread_name[20]; /* max size is 20bytes */
    struct drv_hdmitx_cec_device *cec = osal_container_of(ping_manage, struct drv_hdmitx_cec_device, ping_manage);

    ping_manage->cancel = HI_FALSE;
    scnprintf(thread_name, 20, "cec%d-ping-thread", cec->id); /* the array max size is 20. */
    ping_manage->thread = osal_kthread_create(ping_thread, ping_manage, thread_name, 0);
    if (IS_ERR(ping_manage->thread)) {
        ping_manage->thread = HI_NULL;
        cec_err("create cec%d-ping-thread failed!\n", cec->id);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_u32 edid_get_spa_location(const hi_u8 *edid, hi_u32 size)
{
    hi_u32 blocks = size / 128; /* block size is 128 bytes. */
    hi_u32 block;
    hi_u8 d;
    hi_u32 i;
    hi_u32 end;
    hi_u32 offset;
    hi_u8 tag;
    hi_u8 len;

    if (blocks < 2 || size % 128) { /* Sanity check: at least 2 blocks and a multiple of the block size(128) */
        return 0;
    }
    /*
     * If there are fewer extension blocks than the size, then update
     * 'blocks'. It is allowed to have more extension blocks than the size,
     * since some hardware can only read e.g. 256 bytes of the EDID, even
     * though more blocks are present. The first CEA-861 extension block
     * should normally be in block 1 anyway.
     */
    if (edid[0x7e] + 1 < blocks) {
        blocks = edid[0x7e] + 1; /* the blocks number is edid[0x7e]. */
    }

    for (block = 1; block < blocks; block++) {
        offset = block * 128; /* block size is 128 bytes. */

        /* Skip any non-CEA-861 extension blocks */
        if (edid[offset] != 0x02 || edid[offset + 1] != 0x03) {
            continue;
        }
        /* search Vendor Specific Data Block (tag 3) */
        d = edid[offset + 2] & 0x7f; /* d is the edid[offset + 2] & 0x7f). */
        /* Check if there are Data Blocks */
        if (d <= 4) { /* if d <= 4, no data blocks. */
            continue;
        }

        i = offset + 4; /* skip 4 bytes firstly. */
        end = offset + d;

        /* Note: 'end' is always < 'size' */
        do {
            tag = edid[i] >> 5; /* vsdb' tag is upper 3 bits, need right shift 5bits. */
            len = edid[i] & 0x1f; /* vsdb' length is lower 5 bits(edid[i] & 0x1f). */

            if (tag == 3 && len >= 5 && i + len <= end && /* vsdb' tag is 3, the least length is 5. */
                edid[i + 1] == 0x03 && /* oui 3th is 0x03(edid[i + 1]). */
                edid[i + 2] == 0x0c && /* oui 2th is 0x0c(edid[i + 2]). */
                edid[i + 3] == 0x00) { /* oui 1th is 0x00(edid[i + 3]). */
                return i + 4; /* physical addr offset is 4 in the Vendor Specific Data Block. */
            }
            i += len + 1;
        } while (i < end);
    }
    return 0;
}

static hi_u16 edid_get_phys_addr(const hi_u8 *edid, hi_u32 size,
    hi_u32 *offset)
{
    hi_u32 loc = edid_get_spa_location(edid, size);

    if (offset) {
        *offset = loc;
    }
    if (loc == 0) {
        return CEC_INVALID_PHYSICAL_ADDR;
    }
    return (edid[loc] << 8) | edid[loc + 1]; /* upper 8 bits. */
}

static hi_s32 set_physical_addr(struct drv_hdmitx_cec_device *cec, hi_u16 physical_addr)
{
    hi_s32 ret = HI_SUCCESS;
    cec_status status;

    osal_mutex_lock(&cec->lock);
    /* physical address not change, do nothing */
    if (cec->status.physical_addr == physical_addr) {
        osal_mutex_unlock(&cec->lock);
        return HI_SUCCESS;
    }

    status.logical_addr = cec->status.logical_addr;
    status.logic_addr_status = cec->status.logic_addr_status;
    status.physical_addr = physical_addr;
    change_status(cec, status);

    /* if the logical address has been configured, query the available logical address again. */
    if (cec->status.logic_addr_status != CEC_LOGIC_ADDR_NOT_CONFIG) {
        ping_cancel(&cec->ping_manage);
        transmit_cancal(&cec->tx_manage);
        ret = ping_start(&cec->ping_manage);
        if (ret == HI_SUCCESS) {
            status.logical_addr = CEC_INVALID_LOGICAL_ADDR;
            status.logic_addr_status = CEC_LOGIC_ADDR_CONFIGURING;
        }
    }
    change_status(cec, status);
    osal_mutex_unlock(&cec->lock);
    return ret;
}

static hi_void dbg_show_unhandle_msg(struct drv_hdmitx_cec_device *cec)
{
    hi_u32 i = 0;
    struct cec_event_received_entry *entry = HI_NULL;

    cec_dbg_print("cec%d-unhandle message count:%d\n", cec->id, cec->event_manage.received_cnt);
    cec_dbg_print("index  message\n", cec->id, cec->event_manage.received_cnt);
    osal_list_for_each_entry(entry, &cec->event_manage.received, list) {
        cec_dbg_print("%-5d  %*ph\n", i, entry->rx_msg.len, entry->rx_msg.data);
        i++;
    }
}

static hi_void dbg_show_msg_history(struct drv_hdmitx_cec_device *cec)
{
    hi_u32 i = 0;
    struct cec_msg_history_entry *entry = HI_NULL;
    hi_char *result = "sending";

    cec_dbg_print("-----------------cec%d message history-----------------\n", cec->id);
    cec_dbg_print("index  timestamp     duration(ms)  T/R  message\n");
    osal_list_for_each_entry(entry, &cec->history.head, list) {
        cec_dbg_print("%-5d  %-16s  %-8llu  %-3s  %*ph", i,
            get_time_str(entry->element.timestamp_finish),
            entry->element.timestamp_finish - entry->element.timestamp_start,
            entry->element.recevie ? "Rx" : "Tx",
            entry->element.msg.len, entry->element.msg.data);
        result = entry->element.recevie ? "" :
            (entry->element.tx_result == CEC_TX_RESULT_ACK) ? "(ack)" :
            (entry->element.tx_result == CEC_TX_RESULT_NACK) ? "(nack)" :
            (entry->element.tx_result == CEC_TX_RESULT_FAILED) ? "(failed)" : "(sending)";
        cec_dbg_print("%s\n", result);
        i++;
    }
}

static hi_s32 cec_thread(hi_void *data)
{
    hi_s32 ret;
    cec_msg rx_msg;
    cec_events event;
    cec_transmit_result result;
    hi_u64 timestamp;
    hi_bool send_finish = HI_FALSE;
    struct drv_hdmitx_cec_device *cec = (struct drv_hdmitx_cec_device *)data;

    while (cec->open) {
        ret = hal_cec_receive(&cec->hal, &rx_msg);
        if (ret == HI_SUCCESS) {
            event.type = CEC_EVENT_RECEIVED;
            if (memcpy_s(&event.data.rx_msg, sizeof(event.data.rx_msg), &rx_msg, sizeof(event.data.rx_msg))) {
                cec_err("cec%d-memcpy_s failed\n", cec->id);
                continue;
            }
            timestamp = get_time_ms();
            cec_info("cec%d-rx msg:%*ph\n", cec->id, rx_msg.len, rx_msg.data);
            osal_mutex_lock(&cec->lock);
            event_write(&cec->event_manage, &event);
            history_write_alias(&cec->history, HI_TRUE, timestamp, CEC_TX_RESULT_MAX, &rx_msg);
            osal_mutex_unlock(&cec->lock);
        }

        osal_mutex_lock(&cec->tx_manage.lock);
        if (cec->tx_manage.state == CEC_SEND_STATE_NEED_SEND) {
            hal_cec_stop_transmit(&cec->hal);
            hal_cec_transmit(&cec->hal, &cec->tx_manage.msg, cec->tx_manage.retry_times);
            /* send times = retry times + 1 */
            schedule_delayed_work(&cec->tx_manage.work,
                osal_msecs_to_jiffies(cec_retry_times_to_time(cec->tx_manage.retry_times + 1)));
            cec->tx_manage.state = CEC_SEND_STATE_WAIT_END;
            timestamp = get_time_ms();
            history_write_alias(&cec->history, HI_FALSE, timestamp, CEC_TX_RESULT_MAX, &cec->tx_manage.msg);
        } else if (cec->tx_manage.state == CEC_SEND_STATE_WAIT_END) {
            ret = hal_cec_get_transmit_result(&cec->hal, &result);
            if ((ret == HI_SUCCESS) && (result != CEC_TX_RESULT_MAX)) {
                osal_mutex_unlock(&cec->tx_manage.lock);
                cancel_delayed_work_sync(&cec->tx_manage.work);
                osal_mutex_lock(&cec->tx_manage.lock);
                cec->tx_manage.result = result;
                cec->tx_manage.state = CEC_SEND_STATE_END;
                osal_wait_wakeup(&cec->tx_manage.wait);
                send_finish = HI_TRUE;
            }
        }
        osal_mutex_unlock(&cec->tx_manage.lock);

        if (send_finish) {
            send_finish = HI_FALSE;
            /*
             * Present Initiator wants to send another frame immediately after its previous frame, should wait
             * a signal free time of 7bits(7*2.4ms = 17ms)
             */
            osal_msleep(17);
        } else {
            /* the polling time of the state machine is 10ms */
            osal_msleep(10);
        }
    };
    osal_kthread_destroy(cec->thread, HI_FALSE);
    cec->thread = HI_NULL;

    return HI_SUCCESS;
}

struct drv_hdmitx_cec_device *drv_hdmitx_cec_init(const struct cec_resource *resource)
{
    cec_status status;
    struct drv_hdmitx_cec_device *cec = HI_NULL;

    if (resource == HI_NULL) {
        cec_err("null ptr\n");
        return HI_NULL;
    }

    cec = osal_vmalloc(HI_ID_HDMITX, sizeof(*cec));
    if (cec == HI_NULL) {
        cec_err("cec%d-vmalloc failed\n", resource->id);
        return HI_NULL;
    }

    /* resource init */
    cec->id = resource->id;
    cec->hal.id = resource->id;
    cec->hal.regs_base = resource->regs_base;
    cec->hal.crg_regs_base = resource->crg_regs_base;
    cec->hal.sysctrl_regs_base = resource->sysctrl_regs_base;

    /* software init */
    cec->open = HI_FALSE;
    cec->thread = HI_NULL;
    cec->tgid = -1;
    osal_mutex_init(&cec->lock);
    event_init(&cec->event_manage);
    transmit_init(&cec->tx_manage);
    ping_init(&cec->ping_manage);
    history_init(&cec->history);
    status.logical_addr = CEC_INVALID_LOGICAL_ADDR;
    status.physical_addr = CEC_INVALID_PHYSICAL_ADDR;
    status.logic_addr_status = CEC_LOGIC_ADDR_NOT_CONFIG;
    change_status(cec, status);
    cec->device_type = CEC_DEVICE_TYPE_MAX;
    cec->wake_audio_system = HI_FALSE;
    if (memcpy_s(cec->vendor_id, sizeof(cec->vendor_id), g_default_vendor_id, sizeof(cec->vendor_id))) {
        drv_hdmitx_cec_deinit(cec);
        cec_err("cec%d-memcpy_s failed\n", cec->id);
        return HI_NULL;
    }
    if (strcpy_s(cec->osd_name, sizeof(cec->osd_name), "hisi")) {
        drv_hdmitx_cec_deinit(cec);
        cec_err("cec%d-strcpy_s failed\n", cec->id);
        return HI_NULL;
    }

    /* hardware init */
    hal_cec_reset(&cec->hal);
    hal_cec_backup_logic_addr(&cec->hal, CEC_INVALID_LOGICAL_ADDR);

    return cec;
}

hi_void drv_hdmitx_cec_deinit(struct drv_hdmitx_cec_device *cec)
{
    if (cec != HI_NULL) {
        drv_hdmitx_cec_close(cec);
        osal_vfree(HI_ID_HDMITX, cec);
    }
}

hi_s32 drv_hdmitx_cec_suspend(struct drv_hdmitx_cec_device *cec)
{
    struct cec_switch_info info;

    if (cec == HI_NULL) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    osal_mutex_lock(&cec->lock);
    ping_cancel(&cec->ping_manage);
    history_release(&cec->history);
    /* drop all transmit messages and drop all events */
    transmit_cancal(&cec->tx_manage);
    event_release(&cec->event_manage);
    osal_mutex_unlock(&cec->lock);

    /* hdmitx 1 not support CEC standby */
    if (cec->id == 1) {
        drv_hdmitx_cec_close(cec);
        return HI_SUCCESS;
    }

    if (cec->status.logical_addr == CEC_INVALID_LOGICAL_ADDR) {
        cec_warn("cec%d-logical addr is invalid!\n", cec->id);
    }

    info.enable = (cec->status.logic_addr_status == CEC_LOGIC_ADDR_NOT_CONFIG) ? HI_FALSE : cec->open;
    info.wake_audio_system = cec->wake_audio_system;
    info.device_type = cec->device_type;
    info.logical_addr = cec->status.logical_addr;
    info.physical_addr = cec->status.physical_addr;
    if (memcpy_s(info.vendor_id, sizeof(info.vendor_id), cec->vendor_id, sizeof(info.vendor_id))) {
        cec_err("cec%d-memcpy_s failed\n", cec->id);
        return HI_FAILURE;
    }
    if (memcpy_s(info.osd_name, sizeof(info.osd_name), cec->osd_name, sizeof(info.osd_name))) {
        cec_err("cec%d-memcpy_s failed\n", cec->id);
        return HI_FAILURE;
    }
    hal_cec_set_switch_info(&cec->hal, &info);

    return HI_SUCCESS;
}

hi_s32 drv_hdmitx_cec_resume(struct drv_hdmitx_cec_device *cec)
{
    hi_u8 device_type;
    pid_t tgid;

    if (cec == HI_NULL) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    if (cec->id == 1) { /* hdmitx 1 */
        device_type = cec->device_type;
        tgid = cec->tgid;
        drv_hdmitx_cec_open(cec);
        cec->tgid = tgid;
        return drv_hdmitx_cec_set_device_type(cec, device_type);
    }
    return HI_SUCCESS;
}

hi_s32 drv_hdmitx_cec_open(struct drv_hdmitx_cec_device *cec)
{
    cec_status status;
    cec_events event;
    hi_char thread_name[16]; /* max size is 16bytes */

    if (cec == HI_NULL) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    osal_mutex_lock(&cec->lock);
    /* prevent multi process open */
    if (cec->open == HI_TRUE) {
        osal_mutex_unlock(&cec->lock);
        cec_warn("cec%d already open by process(%d)\n", cec->id, cec->tgid);
        return HI_ERR_HDMITX_BUS_BUSY;
    }
    cec->open = HI_TRUE;
    cec->tgid = current->tgid;
    cec_info("cec%d open by process(%d)\n", cec->id, cec->tgid);

    /* set logical addr status to not_config */
    status.logical_addr = CEC_INVALID_LOGICAL_ADDR;
    status.physical_addr = cec->status.physical_addr;
    status.logic_addr_status = CEC_LOGIC_ADDR_NOT_CONFIG;
    change_status(cec, status);
    cec->device_type = CEC_DEVICE_TYPE_MAX;

    /* Force to generate status_change event when open cec. */
    event.type = CEC_EVENT_STATUS_CHANGE;
    event.data.status.logical_addr = cec->status.logical_addr;
    event.data.status.physical_addr = cec->status.physical_addr;
    event.data.status.logic_addr_status = cec->status.logic_addr_status;
    event_write(&cec->event_manage, &event);

    /* enable hardware and set a invalid logical address(can only receive broadcast messages) */
    hal_cec_reset(&cec->hal);
    hal_cec_enable(&cec->hal);
    hal_cec_set_logic_addr(&cec->hal, CEC_INVALID_LOGICAL_ADDR);

    /* create cec-thread */
    scnprintf(thread_name, 16, "cec%d-thread", cec->id); /* the array max size is 16. */
    cec->thread = osal_kthread_create(cec_thread, cec, thread_name, 0);
    if (IS_ERR(cec->thread)) {
        cec->thread = HI_NULL;
        cec->open = HI_FALSE;
        cec->tgid = -1;
        osal_mutex_unlock(&cec->lock);
        cec_err("create cec%d-thread failed!\n", cec->id);
        return HI_FAILURE;
    }
    osal_mutex_unlock(&cec->lock);

    return HI_SUCCESS;
}

hi_void drv_hdmitx_cec_close(struct drv_hdmitx_cec_device *cec)
{
    if (cec == HI_NULL) {
        cec_err("null ptr!\n");
        return;
    }

    osal_mutex_lock(&cec->lock);
    if (cec->open == HI_FALSE) {
        osal_mutex_unlock(&cec->lock);
        return;
    }
    cec->open = HI_FALSE;

    ping_cancel(&cec->ping_manage);
    history_release(&cec->history);
    /* drop all transmit messages and drop all events */
    transmit_cancal(&cec->tx_manage);
    event_release(&cec->event_manage);

    /* disable hardware */
    hal_cec_disable(&cec->hal);
    osal_mutex_unlock(&cec->lock);

    /* wait thread exit */
    while (cec->thread != HI_NULL) {
        osal_msleep(1); /* check every 1ms */
    }
}

hi_s32 drv_hdmitx_cec_read_events(struct drv_hdmitx_cec_device *cec, cec_events *events)
{
    hi_s32 ret;

    if ((cec == HI_NULL) || (events == HI_NULL)) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    osal_mutex_lock(&cec->lock);
    ret = event_read(&cec->event_manage, events);
    osal_mutex_unlock(&cec->lock);
    if (ret == HI_SUCCESS) {
        return HI_SUCCESS;
    }

    osal_wait_interruptible(&cec->event_manage.wait, wait_read_event, (hi_void *)cec);

    osal_mutex_lock(&cec->lock);
    ret = event_read(&cec->event_manage, events);
    osal_mutex_unlock(&cec->lock);
    if (ret != HI_SUCCESS) {
        cec_info("cec%d-no event\n", cec->id);
        events->type = CEC_EVENT_MAX;
    }

    return HI_SUCCESS;
}

hi_s32 drv_hdmitx_cec_get_status(struct drv_hdmitx_cec_device *cec, cec_status *status)
{
    if ((cec == HI_NULL) || (status == HI_NULL)) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    osal_mutex_lock(&cec->lock);
    if (memcpy_s(status, sizeof(*status), &cec->status, sizeof(*status))) {
        osal_mutex_unlock(&cec->lock);
        cec_err("cec%d-memcpy_s failed\n", cec->id);
        return HI_FAILURE;
    }
    osal_mutex_unlock(&cec->lock);
    return HI_SUCCESS;
}

hi_s32 drv_hdmitx_cec_set_device_type(struct drv_hdmitx_cec_device *cec, hi_u8 device_type)
{
    hi_s32 ret;
    cec_status status;
    const hi_char *name = HI_NULL;

    name = g_device_type_name[device_type];

    if (cec == HI_NULL) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    osal_mutex_lock(&cec->lock);
    if (cec->open == HI_FALSE) {
        osal_mutex_unlock(&cec->lock);
        cec_warn("cec%d-not open\n", cec->id);
        return HI_ERR_HDMITX_DEV_NOT_OPEN;
    }

    /* only support tuner and playback now */
    if ((device_type != CEC_DEVICE_TYPE_TUNER) &&
        (device_type != CEC_DEVICE_TYPE_PLAYBACK)) {
        osal_mutex_unlock(&cec->lock);
        cec_warn("cec%d-not support device type:%s\n", cec->id,
            device_type >= CEC_DEVICE_TYPE_MAX ? "error" : name);
        return HI_ERR_HDMITX_FEATURE_NO_SUPPORT;
    }

    if ((cec->device_type == device_type) &&
        cec->status.logical_addr != CEC_INVALID_LOGICAL_ADDR) {
        osal_mutex_unlock(&cec->lock);
        return HI_SUCCESS;
    }
    cec->device_type = device_type;

    ping_cancel(&cec->ping_manage);
    /* the device type change, drop all transmit messages and drop all events */
    transmit_cancal(&cec->tx_manage);
    event_release(&cec->event_manage);

    ret = ping_start(&cec->ping_manage);
    if (ret == HI_SUCCESS) {
        status.physical_addr = cec->status.physical_addr;
        status.logical_addr = CEC_INVALID_LOGICAL_ADDR;
        status.logic_addr_status = CEC_LOGIC_ADDR_CONFIGURING;
        change_status(cec, status);
    }

    osal_mutex_unlock(&cec->lock);
    return ret;
}

hi_s32 drv_hdmitx_cec_transmit(struct drv_hdmitx_cec_device *cec, const cec_msg *msg)
{
    hi_s32 ret;
    hi_u32 retry_times;

    if ((cec == HI_NULL) || (msg == HI_NULL)) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    osal_mutex_lock(&cec->lock);
    if (cec->open == HI_FALSE) {
        osal_mutex_unlock(&cec->lock);
        cec_warn("cec%d not open\n", cec->id);
        return HI_ERR_HDMITX_DEV_NOT_OPEN;
    }

    /* not allow to transmit message when in not_config or configuring state */
    if (cec->status.logic_addr_status == CEC_LOGIC_ADDR_NOT_CONFIG) {
        osal_mutex_unlock(&cec->lock);
        cec_warn("cec%d-logical address not config.\n", cec->id);
        return HI_ERR_HDMITX_NO_LOGIC_ADDR;
    } else if (cec->status.logic_addr_status == CEC_LOGIC_ADDR_CONFIGURING) {
        osal_mutex_unlock(&cec->lock);
        cec_warn("cec%d-logical address is quering now.\n", cec->id);
        return HI_ERR_HDMITX_NO_LOGIC_ADDR;
    }
    osal_mutex_unlock(&cec->lock);

    ret = check_msg(&cec->tx_manage, msg);
    if (ret != HI_SUCCESS) {
        cec_err("cec%d-invalid msg: %*ph\n", cec->id, msg->len, msg->data);
        return HI_ERR_HDMITX_INVALID_PARA;
    }

    if (msg->data[1] == CEC_OPCODE_DEVICE_VENDOR_ID) {
        save_vendor_id(cec, msg);
    } else if (msg->data[1] == CEC_OPCODE_SET_OSD_NAME) {
        save_osd_name(cec, msg);
    }

    /* the retry times of <polling message> is 1, the retry times of other messages is 4. */
    retry_times = (msg->len == 1) ? CEC_POLLING_RETRY_TIMES : CEC_DEFAULT_RETRY_TIMES;
    return transmit_msg(&cec->tx_manage, msg, retry_times);
}

hi_void drv_hdmitx_cec_release(struct drv_hdmitx_cec_device *cec)
{
    /* cose cec if exit abnormal */
    if ((cec != HI_NULL) && (cec->tgid == current->tgid) && (current->exit_code != 0)) {
        cec_info("cec%d-release, tgid=%d\n", cec->id, cec->tgid);
        drv_hdmitx_cec_close(cec);
    }
}

hi_void drv_hdmitx_cec_s_phys_addr_from_edid(struct drv_hdmitx_cec_device *cec,
    const struct edid *edid)
{
    hi_u16 pa = CEC_INVALID_PHYSICAL_ADDR;

    if (cec == HI_NULL) {
        cec_err("null ptr!\n");
        return;
    }

    if (edid && edid->extensions) {
        pa = edid_get_phys_addr((const hi_u8 *)edid, EDID_LENGTH * (edid->extensions + 1), HI_NULL);
    }

    if (set_physical_addr(cec, pa)) {
        cec_err("set physical addr fail.\n");
    }
}

hi_s32 drv_hdmitx_cec_dbg(struct drv_hdmitx_cec_device *cec, enum cec_debug_cmd cmd, const hi_void *arg)
{
    hi_s32 ret = HI_SUCCESS;

    if (cec == HI_NULL) {
        cec_err("null ptr!\n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case CEC_DBG_CMD_OPEN:
            ret = drv_hdmitx_cec_open(cec);
            break;
        case CEC_DBG_CMD_CLOSE:
            drv_hdmitx_cec_close(cec);
            break;
        case CEC_DBG_CMD_SET_DEVICE_TYPE:
            ret = drv_hdmitx_cec_set_device_type(cec, *(hi_u8 *)arg);
            break;
        case CEC_DBG_CMD_SEND_MSGS:
            ret = drv_hdmitx_cec_transmit(cec, (const cec_msg *)arg);
            break;
        case CEC_DBG_CMD_SHOW_UNHANDLE_MSGS:
            dbg_show_unhandle_msg(cec);
            break;
        case CEC_DBG_CMD_SHOW_HISTORY:
            dbg_show_msg_history(cec);
            break;
        case CEC_DBG_CMD_GENERATE_EVENTS:
            event_write(&cec->event_manage, (const cec_events *)arg);
            break;
        case CEC_DBG_CMD_WAKEUP_AUDIO_SYSTEM:
            cec->wake_audio_system = *(hi_bool *)arg;
            break;
        default:
            break;
    }
    return ret;
}

