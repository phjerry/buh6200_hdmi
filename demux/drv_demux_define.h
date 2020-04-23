/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Register define for HiPVRV200
 * Author: sdk
 * Create: 2009-09-27
 */
#ifndef __DRV_DEMUX_DEFINE_H__
#define __DRV_DEMUX_DEFINE_H__

#include <linux/list.h>
#include <linux/wait.h>
#include "linux/mutex.h"
#include "linux/workqueue.h"

#include "hi_osal.h"
#include "hi_type.h"
#include "hi_drv_demux.h"
#include "drv_demux_config.h"
#include "drv_demux_utils.h"
#include "drv_demux_tee.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEMUX_NAME                      "HI_DEMUX"
#define BUF_AP_THRESHOLD                (16 * 1024)
#define PCBUF_AP_THRESHOLD              (4 * 1024)
#define TS_SYNC_BYTE                     0x47
#define TTS_SYNC_BYTE_OFFSET             4
#define MS_2_US                          1000

struct dmx_session;
struct dmx_slot;
struct dmx_r_base;
struct dmx_mgmt;

struct dmx_r_ram_port;
struct dmx_r_rmx_pump;
struct dmx_r_rmx_fct;
struct dmx_r_band;
struct dmx_r_pid_ch;
struct dmx_r_raw_pid_ch;
struct dmx_r_play_fct;
struct dmx_r_rec_fct;
struct dmx_r_dsc_fct;
struct dmx_r_pcr_fct;
struct dmx_r_buf;
struct dmx_r_flt;
struct dmx_r_teec;

#define DMX_STR_LEN_32                  32
#define DMX_STR_LEN_16                  16
#define DMX_INVALID_PTS                 0xFFFFFFFFFFFFFFFF
#define DMX_INVALID_PORT_ID             0xFFFFFFFFU
#define DMX_INVALID_PID                 0x1FFFU
#define DMX_INVALID_FILTER_ID           0xFFFFU
#define DMX_MAX_TIME_OUT                10000 /* ms */
#define DMX_PVR_DISP_CTRL               (-2)  /* special error code */

#define DMX_REC_SCD_RANGE_FILTER_CNT    40
#define DMX_DEFAULT_PID_COPY_BUF_LEN    (1 * 1024 * 1024)     /* 1 MB */

#define DMX_VID_SCD_BUF_SIZE            (56 * 1024)
#define DMX_AUD_SCD_BUF_SIZE            (28 * 1024)
#define DMX_SCD_INDEX_SIZE              32  /* 8word( one word has 4 bytes)*4 */
#define DMX_SCD_INDEX_THRESHOLD         384  /* 8word(one word has 4 bytes)*4*12(one ts has max 12 index) */
#define DMX_MAX_SCD_CNT                 16

#define DMX_MAX_LOST_TH                 0x3
#define DMX_MAX_LOCK_TH                 0x7

#define DMX_DEFAULT_TAG_LENGTH          4

#ifdef HI_DEMUX_PROC_SUPPORT
#define DMX_FILE_NAME_LEN               256
#endif

/* port type releated */
enum dmx_port_type_e {
    DMX_PORT_TSI_TYPE = 1,
    DMX_PORT_RAM_TYPE = 2,
    DMX_PORT_TAG_TYPE = 3,
    DMX_PORT_RMX_TYPE = 4,
    DMX_PORT_TSIO_TYPE = 5,
    DMX_PORT_IF_TYPE = 6,

    DMX_PORT_MAX_TYPE
};

/* buf releated */
struct dmx_buf_attrs {
    dmx_secure_mode   secure_mode;
    hi_s64            buf_handle;
    hi_u32            buf_size;
#if (DMX_REC_BUF_GAP_EXIST == 1)
    hi_u32            pkt_size;
#endif
    hi_bool           pes_ext_flag;
};

enum  dmx_mgmt_state {
    DMX_MGMT_CLOSED = 0x0,
    DMX_MGMT_OPENED,
};

struct dmx_mgmt;

struct dmx_mgmt_ops {
    hi_s32(*init)(struct dmx_mgmt *mgmt);
    hi_s32(*exit)(struct dmx_mgmt *mgmt);

    hi_s32(*get_cap)(struct dmx_mgmt *mgmt, dmx_capability *cap);

    hi_s32(*suspend)(struct dmx_mgmt *mgmt);
    hi_s32(*resume)(struct dmx_mgmt *mgmt);

    hi_s32(*create_ram_port)(struct dmx_mgmt *mgmt, dmx_port port, const dmx_ram_port_attr *attrs,
        struct dmx_r_ram_port **ram_port);
    hi_s32(*destroy_ram_port)(struct dmx_mgmt *mgmt, struct dmx_r_ram_port *ram_port);

    hi_s32(*create_rmx_pump)(struct dmx_mgmt *mgmt, const dmx_rmx_pump_attrs *attrs, struct dmx_r_rmx_pump **rrmx_pump);
    hi_s32(*destroy_rmx_pump)(struct dmx_mgmt *mgmt, struct dmx_r_rmx_pump *rrmx_pump);

    hi_s32(*create_rmx_fct)(struct dmx_mgmt *mgmt, const dmx_rmx_attrs *attrs, struct dmx_r_rmx_fct **rrmx_fct);
    hi_s32(*destroy_rmx_fct)(struct dmx_mgmt *mgmt, struct dmx_r_rmx_fct *rrmx_fct);

    hi_s32(*create_band)(struct dmx_mgmt *mgmt, dmx_band band, const dmx_band_attr *attrs, struct dmx_r_band **rband);
    hi_s32(*destroy_band)(struct dmx_mgmt *mgmt, struct dmx_r_band *rband);

    hi_s32(*create_pid_ch)(struct dmx_mgmt *mgmt, struct dmx_r_band *rband, const hi_u32 pid,
        struct dmx_r_pid_ch **rpid_ch);
    hi_s32(*destroy_pid_ch)(struct dmx_mgmt *mgmt, struct dmx_r_pid_ch *rpid_ch);

    hi_s32(*create_raw_pid_ch)(struct dmx_mgmt *mgmt, struct dmx_r_raw_pid_ch **rraw_pid_ch);
    hi_s32(*destroy_raw_pid_ch)(struct dmx_mgmt *mgmt, struct dmx_r_raw_pid_ch *rraw_pid_ch);

    hi_s32(*create_play_fct)(struct dmx_mgmt *mgmt, const dmx_play_attrs *attrs, struct dmx_r_play_fct **rplay_fct);
    hi_s32(*destroy_play_fct)(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct);

    hi_s32(*create_rec_fct)(struct dmx_mgmt *mgmt, const dmx_rec_attrs *attrs, struct dmx_r_rec_fct **rrec_fct);
    hi_s32(*destroy_rec_fct)(struct dmx_mgmt *mgmt, struct dmx_r_rec_fct *rrec_fct);

    hi_s32(*create_dsc_fct)(struct dmx_mgmt *mgmt, const dmx_dsc_attrs *attrs, struct dmx_r_dsc_fct **rdsc_fct);
    hi_s32(*destroy_dsc_fct)(struct dmx_mgmt *mgmt, struct dmx_r_dsc_fct *rdsc_fct);

    hi_s32(*create_pcr_fct)(struct dmx_mgmt *mgmt, struct dmx_r_band *rband, hi_u32 pid,
        struct dmx_r_pcr_fct **rpcr_fct);
    hi_s32(*destroy_pcr_fct)(struct dmx_mgmt *mgmt, struct dmx_r_pcr_fct *rpcf_fct);

    hi_s32(*create_buf)(struct dmx_mgmt *mgmt, struct dmx_buf_attrs *attrs, struct dmx_r_buf **rbuf);
    hi_s32(*destroy_buf)(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf);

    hi_s32(*create_flt)(struct dmx_mgmt *mgmt, const dmx_filter_attrs *attrs, struct dmx_r_flt **rflt);
    hi_s32(*destroy_flt)(struct dmx_mgmt *mgmt, struct dmx_r_flt *rflt);

    hi_s32(*get_free_flt_cnt)(struct dmx_mgmt *mgmt, hi_u32 *free_cnt);
    hi_s32(*get_free_ch_cnt)(struct dmx_mgmt *mgmt, hi_u32 *free_cnt);

    /* ree tee communication */
    hi_s32(*send_cmd_to_ta)(hi_u32 cmd_id, TEEC_Operation *operation, hi_u32 *ret_origin);

    /* debug helper */
    hi_void(*show_info)(struct dmx_mgmt *mgmt);
};

typedef struct {
    osal_mutex        lock;
    dmx_tsi_port_mode port_mod;
    dmx_tsi_port_type  port_type;
    hi_u32            sync_lock_th;
    hi_u32            sync_lost_th;
    /*
    * whether Tuner input clock inverting or not.
    * 0: in-phase(default)
    * 1: inverting
    */
    hi_u32  tuner_in_clk;
    /* port-line sequence select:
    * parallel:
    *    0: mean cdata[7] is the significant bit(default)
    *    1: mean cdata[0] is the significant bit
    * serial:
    *    0: mean cdata[0] is the data line (default)
    *    1: mean cdata[7] is the data line
    */
    hi_u32  bit_selector;
    dmx_port serial_port_share_clk; /* share clock port */
    hi_u32  tuner_clk_mode; /* 0: single edge-triggered, 1: double edge-triggered */
} dmx_tsi_port_info, dmx_if_port_info;

typedef struct {
    osal_mutex        lock;
    dmx_tso_port_attr attrs;
} dmx_tso_port_info;

typedef struct {
    osal_mutex        lock;
    dmx_tag_port_attr attrs;
} dmx_tag_port_info;

struct dmx_data_sel {
    osal_mutex           data_sel_lock;
    hi_u32               condition;
    wait_queue_head_t    wait_queue;
};

struct dmx_mgmt {
    osal_mutex            lock;
    enum  dmx_mgmt_state  state;
    osal_atomic           ref_count;
    struct dmx_mgmt_ops   *ops;

    struct workqueue_struct *dmx_queue;

    hi_void               *io_base;
    hi_void               *mdsc_base;
    hi_u32                cb_ttbr;

    hi_u32                if_port_cnt;
    osal_mutex            if_port_list_lock;
    dmx_if_port_info      if_port_info[DMX_IF_PORT_CNT];
    DECLARE_BITMAP(if_port_bitmap, DMX_IF_PORT_CNT);

    hi_u32                tsi_port_cnt;
    hi_u32                tsi_port_policy_cnt;
    osal_mutex            tsi_port_list_lock;
    dmx_tsi_port_info     tsi_port_info[DMX_TSI_PORT_CNT];
    DECLARE_BITMAP(tsi_port_bitmap, DMX_TSI_PORT_CNT);

    hi_u32                tso_port_cnt;
    osal_mutex            tso_port_list_lock;
    dmx_tso_port_info     tso_port_info[DMX_TSO_PORT_CNT];
    DECLARE_BITMAP(tso_port_bitmap, DMX_TSO_PORT_CNT);

    hi_u32                tag_port_cnt;
    osal_mutex            tag_port_list_lock;
    dmx_tag_port_info     tag_port_info[DMX_TAG_PORT_CNT];
    DECLARE_BITMAP(tag_port_bitmap, DMX_TAG_PORT_CNT);

    hi_u32                tsio_port_cnt;

    hi_u32                rmx_port_cnt;

    hi_u32                ram_port_cnt;
    osal_mutex            ram_port_list_lock;
    osal_spinlock         ram_port_list_lock2;
    struct list_head      ram_port_head;
    DECLARE_BITMAP(ram_port_bitmap, DMX_RAM_PORT_CNT);

    /* total rmx pump list */
    hi_u32                pump_total_cnt;
    osal_mutex            pump_total_list_lock;
    struct list_head      pump_total_head;
    DECLARE_BITMAP(pump_total_bitmap, DMX_RMXPUMP_TOTALCNT);

    /* rmx_fct list */
    hi_u32                rmx_fct_cnt;
    osal_mutex            rmx_fct_list_lock;
    struct list_head      rmx_fct_head;
    DECLARE_BITMAP(rmx_fct_bitmap, DMX_RMXFCT_CNT);

    hi_u32                band_cnt;
    hi_u32                band_total_cnt;
    osal_mutex            band_list_lock;
    struct list_head      band_head;
    DECLARE_BITMAP(band_bitmap, DMX_BAND_TOTAL_CNT);

    /* virtual pid_channel list */
    hi_u32                pid_channel_cnt;
    osal_mutex            pid_channel_list_lock;
    osal_spinlock         pid_channel_list_lock2;
    struct list_head      pid_channel_head;
    DECLARE_BITMAP(pid_channel_bitmap, DMX_PID_CHANNEL_CNT);

    /* total raw pid_channel list */
    hi_u32                raw_pid_channel_cnt;
    osal_mutex            raw_pid_ch_total_list_lock;
    struct list_head      raw_pid_ch_total_head;
    DECLARE_BITMAP(raw_pid_channel_bitmap, DMX_PID_CHANNEL_CNT);

    /* play_fct list */
    hi_u32                play_fct_cnt;
    osal_mutex            play_fct_list_lock;
    struct list_head      play_fct_head;
    DECLARE_BITMAP(play_fct_bitmap, DMX_PLAY_CNT);

    /* dsc_fct list */
    hi_u32                dsc_fct_cnt;
    osal_mutex            dsc_fct_list_lock;
    struct list_head      dsc_fct_head;
    DECLARE_BITMAP(dsc_fct_bitmap, DMX_DSC_CNT);

    /* pcrt_fct list */
    hi_u32                pcr_fct_cnt;
    osal_mutex            pcr_fct_list_lock;
    osal_spinlock         pcr_fct_list_lock2;
    struct list_head      pcr_fct_head;
    DECLARE_BITMAP(pcr_fct_bitmap, DMX_PCR_CNT);

    /* buffer list */
    hi_u32                buf_cnt;
    osal_mutex            buf_list_lock;
    osal_spinlock         buf_list_lock2;
    struct list_head      buf_head;
    DECLARE_BITMAP(buf_bitmap, DMX_BUF_CNT);

    /* filter list */
    hi_u32                flt_cnt;
    osal_mutex            flt_list_lock;
    struct list_head      flt_head;
    DECLARE_BITMAP(flt_bitmap, DMX_FLT_CNT);

    /* whole TS channel bitmap */
    hi_u32                ts_chan_cnt;
    osal_mutex            ts_chan_lock;
    DECLARE_BITMAP(ts_bitmap, DMX_WHOLE_TS_CHAN_CNT);

    /* pes section channel bitmap */
    hi_u32                pes_sec_chan_cnt;
    osal_mutex            pes_sec_chan_lock;
    DECLARE_BITMAP(pes_sec_bitmap, DMX_PES_SEC_CHAN_CNT);

    /* av_pes/record channel bitmap */
    hi_u32                av_pes_chan_cnt;
    hi_u32                rec_chan_cnt;
    hi_u32                av_pes_used_cnt;
    hi_u32                rec_chan_used_cnt;
    osal_mutex            avr_chan_lock;
    struct list_head      rec_fct_head;
    DECLARE_BITMAP(avr_bitmap, DMX_AVR_CNT);

    /* pid_copy channel bitmap */
    hi_u32                pid_copy_chan_cnt;
    osal_mutex            pid_copy_chan_lock;
    DECLARE_BITMAP(pid_copy_bitmap, DMX_PIDCOPY_CHAN_CNT);

    /* scd channel bitmap */
    hi_u32                scd_chan_cnt;
    osal_mutex            scd_chan_lock;
    DECLARE_BITMAP(scd_bitmap, DMX_SCD_CHAN_CNT);

    osal_task             *monitor;

    /* tee */
    struct dmx_r_teec     teec;

    struct dmx_data_sel   select_wait_queue;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_DEFINE_H__


