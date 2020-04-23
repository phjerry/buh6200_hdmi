/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of CEC framework.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-10-29
 */

#ifndef __DRV_HDMITX_CEC_H__
#define __DRV_HDMITX_CEC_H__

#include "hi_osal.h"
#include "hi_debug.h"
#include "hal_hdmitx_cec.h"
#include "drv_hdmitx_ioctl.h"
#include "drv_hdmitx_edid.h"

#define cec_dbg(fmt...)   HI_DBG_PRINT(HI_ID_HDMITX, fmt)
#define cec_info(fmt...)  HI_INFO_PRINT(HI_ID_HDMITX, fmt)
#define cec_warn(fmt...)  HI_WARN_PRINT(HI_ID_HDMITX, fmt)
#define cec_err(fmt...)   HI_ERR_PRINT(HI_ID_HDMITX, fmt)
#define cec_fatal(fmt...) HI_FATAL_PRINT(HI_ID_HDMITX, fmt)

#define CEC_DEBUG_MAX_ARGV_NUM 10
#define CEC_EVENT_QUEUE_STATUS_CHANGE_MAX_SZ 1
#define CEC_EVENT_QUEUE_RECEIVED_MAX_SZ 48
#define CEC_HISTORY_QUEUE_MAX_SZ 64
#define CEC_DEFAULT_RETRY_TIMES 4
#define CEC_POLLING_RETRY_TIMES 1

#define cec_retry_times_to_time(times) (400*(times) + 100)

#define CEC_LOGIC_ADDR_TV            0
#define CEC_LOGIC_ADDR_RECORD_1      1
#define CEC_LOGIC_ADDR_RECORD_2      2
#define CEC_LOGIC_ADDR_TUNER_1       3
#define CEC_LOGIC_ADDR_PLAYBACK_1    4
#define CEC_LOGIC_ADDR_AUDIOSYSTEM   5
#define CEC_LOGIC_ADDR_TUNER_2       6
#define CEC_LOGIC_ADDR_TUNER_3       7
#define CEC_LOGIC_ADDR_PLAYBACK_2    8
#define CEC_LOGIC_ADDR_RECORD_3      9
#define CEC_LOGIC_ADDR_TUNER_4       10
#define CEC_LOGIC_ADDR_PLAYBACK_3    11
#define CEC_LOGIC_ADDR_RESERVED_1    12
#define CEC_LOGIC_ADDR_RESERVED_2    13
#define CEC_LOGIC_ADDR_SPECIFIC      14
#define CEC_LOGIC_ADDR_UNREGISTERED  15 /* as initiator address */
#define CEC_LOGIC_ADDR_BROADCAST     15 /* as destination address */

enum {
    CEC_OPCODE_SET_OSD_NAME     = 0x47,
    CEC_OPCODE_REPORT_PHYADDR   = 0x84,
    CEC_OPCODE_DEVICE_VENDOR_ID = 0x87,
};

enum cec_send_state {
    CEC_SEND_STATE_IDLE,
    CEC_SEND_STATE_NEED_SEND,
    CEC_SEND_STATE_WAIT_END,
    CEC_SEND_STATE_END,
};

struct cec_event_received_entry {
    struct osal_list_head list;
    cec_msg rx_msg;
};

struct cec_event_manage {
    osal_wait wait;
    cec_status status_change;
    hi_u32 status_change_cnt;
    struct osal_list_head received;
    hi_u32 received_cnt;
};

struct cec_transmit_manage {
    osal_mutex lock;
    osal_mutex lock_multi_process;
    osal_wait wait;
    struct delayed_work work;
    hi_u32 retry_times;
    cec_msg msg;
    cec_transmit_result result;
    enum cec_send_state state;
};

struct cec_ping_manage {
    osal_task *thread;
    hi_bool cancel;
};

struct cec_msg_element {
    hi_bool recevie;
    hi_u64  timestamp_start;
    hi_u64  timestamp_finish;
    cec_transmit_result tx_result;
    cec_msg msg;
};

struct cec_msg_history_entry {
    struct osal_list_head list;
    struct cec_msg_element element;
};

struct cec_msg_history {
    struct osal_list_head head;
    hi_u32 msg_cnt;
};

struct drv_hdmitx_cec_device {
    hi_bool open;
    cec_status status;
    cec_device_type device_type;
    osal_mutex lock;
    osal_task *thread;
    pid_t tgid;

    struct cec_event_manage event_manage;
    struct cec_transmit_manage tx_manage;
    struct cec_ping_manage ping_manage;

    hi_u32 id;
    struct cec_hal hal;
    hi_bool wake_audio_system;
    hi_u8 vendor_id[CEC_VENDOR_ID_SIZE];
    hi_u8 osd_name[CEC_OSD_NAME_SIZE];

    struct cec_msg_history history;
};

enum cec_debug_cmd {
    CEC_DBG_CMD_OPEN,
    CEC_DBG_CMD_CLOSE,
    CEC_DBG_CMD_SET_DEVICE_TYPE,
    CEC_DBG_CMD_SEND_MSGS,
    CEC_DBG_CMD_SHOW_UNHANDLE_MSGS,
    CEC_DBG_CMD_SHOW_HISTORY,
    CEC_DBG_CMD_GENERATE_EVENTS,
    CEC_DBG_CMD_WAKEUP_AUDIO_SYSTEM,
    CEC_DBG_CMD_MAX,
};

struct cec_debug_cmd_arg {
    hi_s8 *argv[CEC_DEBUG_MAX_ARGV_NUM];
    hi_u32 argc;
};

struct cec_resource {
    void __iomem *regs_base;
    void __iomem *crg_regs_base;
    void __iomem *sysctrl_regs_base;
    hi_u32 id;
};

struct drv_hdmitx_cec_device *drv_hdmitx_cec_init(const struct cec_resource *resource);
hi_void drv_hdmitx_cec_deinit(struct drv_hdmitx_cec_device *cec);
hi_s32 drv_hdmitx_cec_suspend(struct drv_hdmitx_cec_device *cec);
hi_s32 drv_hdmitx_cec_resume(struct drv_hdmitx_cec_device *cec);
hi_s32 drv_hdmitx_cec_open(struct drv_hdmitx_cec_device *cec);
hi_void drv_hdmitx_cec_close(struct drv_hdmitx_cec_device *cec);
hi_s32 drv_hdmitx_cec_read_events(struct drv_hdmitx_cec_device *cec, cec_events *events);
hi_s32 drv_hdmitx_cec_get_status(struct drv_hdmitx_cec_device *cec, cec_status *status);
hi_s32 drv_hdmitx_cec_set_device_type(struct drv_hdmitx_cec_device *cec, hi_u8 device_type);
hi_s32 drv_hdmitx_cec_transmit(struct drv_hdmitx_cec_device *cec, const cec_msg *msg);
hi_void drv_hdmitx_cec_release(struct drv_hdmitx_cec_device *cec);
hi_void drv_hdmitx_cec_s_phys_addr_from_edid(
    struct drv_hdmitx_cec_device *cec, const struct edid *edid);
hi_s32 drv_hdmitx_cec_dbg(struct drv_hdmitx_cec_device *cec, enum cec_debug_cmd cmd, const hi_void *arg);
#endif

