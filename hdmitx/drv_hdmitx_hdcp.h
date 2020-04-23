/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver hdcp head file.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-15
 */

#ifndef __DRV_HDMITX_HDCP_H__
#define __DRV_HDMITX_HDCP_H__

#include "hi_type.h"
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include "hal_hdmitx_hdcp2x.h"
#include "hal_hdmitx_hdcp1x.h"
#include "drv_hdmitx_ioctl.h"
#include "hi_hdmi_types.h"

struct hisilicon_hdmi;

#define HDCP_NAME_SIZE                 20

/* HDCP1.4 offset define (slave=0x74), see HDCP14 spec page 29 */
#define HDCP1X_OFFSET_BKSV                   0x00
#define HDCP1X_OFFSET_BRI                    0x08
#define HDCP1X_OFFSET_AKSV                   0x10
#define HDCP1X_OFFSET_AN                     0x18
#define HDCP1X_OFFSET_BVH0                   0x20
#define HDCP1X_OFFSET_BCAPS                  0x40
#define HDCP1X_OFFSET_BSTATUS                0x41
#define HDCP1X_OFFSET_KSV_FIFO               0x43

/* Bcaps ,Rd,OFFSET 0x40,see <HDCP1.4> table 2-2 */
union hdcp1x_bcaps {
    struct {
        hi_u8 fast_reauthentication : 1;
        hi_u8 features1p1           : 1;
        hi_u8 rsvd                  : 2;
        hi_u8 fast                  : 1;
        hi_u8 ksv_fifo_ready        : 1;
        hi_u8 repeater              : 1;
        hi_u8 hdmi_rsvd             : 1;
    } u8;
    hi_u8 byte;
};

/* Bstatus ,Rd,OFFSET 0x41,see <HDCP1.4> table 2-4 */
union hdcp1x_bstatus {
    struct {
        hi_u16 device_cnt           : 7;
        hi_u16 max_devs_exceeded    : 1;
        hi_u16 depth                : 3;
        hi_u16 max_cascade_exceeded : 1;
        hi_u16 hdmi_mode            : 1;
        hi_u16 hdmi_rsvd            : 1;
        hi_u16 rsvd                 : 2;
    } u16;
    hi_u16 word;
};

struct hdcp1x_sink_status {
    union hdcp1x_bcaps bcaps_p1;
    hi_u8 b_ksv[HDCP1X_SIZE_5BYTES_KSV];
    hi_u8 b_r0[HDCP1X_SIZE_2BYTES_RI];
    union hdcp1x_bstatus bstatus;
    hi_u32 bstatus_cnt;
    union hdcp1x_bcaps bcaps_p2;
    hi_u8 bksv_list_data[HDCP1X_SIZE_MAX_BKSV_LIST];
    hi_u32 bksv_list_size;
    hi_u8 b_vi[HDCP1X_SIZE_20BYTES_VI];
};
struct hdcp2x_src_status {
    struct hdcp2x_stream_manage str_msg;
};

struct hdmi_hdcp {
    struct hisilicon_hdmi *hdmi;
    struct mutex mutex;
    struct hdcp_usr_status status;
    hi_u32 usr_mode;              /* see HDCP_MODE_XXX */
    hi_u32 usr_start;
    hi_u32 usr_reauth_times;
    struct hdcp_cap cap_src;
    struct hdcp_cap cap_sink;
    struct hdcp1x_sink_status sink_st_1x;
    struct hdcp1x_src_status src_st_1x;
    struct hdcp2x_sink_status sink_st_2x;
    struct hdcp2x_src_status src_st_2x;
    hi_bool load_1x_key;
    hi_bool hdcp1x_ri_err_cnt;
    hi_bool load_mcu_code2x;
    hi_u32 start_delay_time;     /* different sink may need a different delay time */
    hi_u32 cur_delay_time;
    hi_u32 work_mode;            /* see HDCP_MODE_XXX */
    hi_char *name;
    struct delayed_work start_work;
    struct delayed_work irq_handle;
    struct hdcp1x_hal_ops *hal_1x_ops;
    struct hdcp2x_hal_ops *hal_2x_ops;
};

#ifdef HI_HDMITX_HDCP_SUPPORT
hi_s32 drv_hdmitx_hdcp_init(struct hisilicon_hdmi *hdmi);
void drv_hdmitx_hdcp_deinit(struct hisilicon_hdmi *hdmi);
void drv_hdmitx_hdcp_on(struct hdmi_hdcp *hdcp);
void drv_hdmitx_hdcp_off(struct hdmi_hdcp *hdcp);
void drv_hdmitx_hdcp_start_auth(struct hdmi_hdcp *hdcp, hi_u32 mode);
void drv_hdmitx_hdcp_stop_auth(struct hdmi_hdcp *hdcp);
void drv_hdmitx_hdcp_clear_cap(struct hdmi_hdcp *hdcp);
hi_s32 drv_hdmitx_hdcp_get_cap(struct hdmi_hdcp *hdcp, struct hdcp_cap *cap);
void drv_hdmitx_hdcp_set_reauth_times(struct hdmi_hdcp *hdcp, hi_u32 reauth_times);
hi_s32 drv_hdmitx_hdcp_get_user_status(struct hdmi_hdcp *hdcp, struct hdcp_usr_status *status);
hi_s32 drv_hdmitx_hdcp_get_hw_status(struct hdmi_hdcp *hdcp);
hi_s32 drv_hdmitx_hdcp_get_hw_cap(struct hdmi_hdcp *hdcp);
hi_s32 drv_hdmitx_hdcp_loadkey(struct hdmi_hdcp *hdcp, hi_u32 key_version);
hi_void drv_hdmitx_hdcp_plugout_handle(struct hdmi_hdcp *hdcp);
#else
static inline hi_s32 drv_hdmitx_hdcp_init(struct hisilicon_hdmi *hdmi)
{
    return HI_SUCCESS;
}

static inline void drv_hdmitx_hdcp_deinit(struct hisilicon_hdmi *hdmi)
{
}

static inline void drv_hdmitx_hdcp_on(struct hdmi_hdcp *hdcp)
{
}

static inline void drv_hdmitx_hdcp_off(struct hdmi_hdcp *hdcp)
{
}

static inline void drv_hdmitx_hdcp_start_auth(struct hdmi_hdcp *hdcp, hi_u32 mode)
{
}

static inline void drv_hdmitx_hdcp_stop_auth(struct hdmi_hdcp *hdcp)
{
}

static inline void drv_hdmitx_hdcp_clear_cap(struct hdmi_hdcp *hdcp)
{
}

static inline hi_s32 drv_hdmitx_hdcp_get_cap(struct hdmi_hdcp *hdcp, struct hdcp_cap *cap)
{
    return HI_SUCCESS;
}

static inline void drv_hdmitx_hdcp_set_reauth_times(struct hdmi_hdcp *hdcp, hi_u32 reauth_times)
{
}

static inline hi_s32 drv_hdmitx_hdcp_get_user_status(struct hdmi_hdcp *hdcp, struct hdcp_usr_status *status)
{
    return HI_SUCCESS;
}

static inline hi_s32 drv_hdmitx_hdcp_get_hw_status(struct hdmi_hdcp *hdcp)
{
    return HI_SUCCESS;
}

static inline hi_s32 drv_hdmitx_hdcp_get_hw_cap(struct hdmi_hdcp *hdcp)
{
    return HI_SUCCESS;
}

static inline hi_s32 drv_hdmitx_hdcp_loadkey(struct hdmi_hdcp *hdcp, hi_u32 key_version)
{
    return HI_SUCCESS;
}

static inline hi_void drv_hdmitx_hdcp_plugout_handle(struct hdmi_hdcp *hdcp)
{
}

#endif /* HI_HDMITX_HDCP_SUPPORT */

#endif /* __DRV_HDMITX_HDCP_H__ */
