/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver hdmi trainning header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMI_FRL_H__
#define __HAL_HDMI_FRL_H__

#include "hi_osal.h"
#include <linux/workqueue.h>
#include "hal_hdmitx_ctrl.h"

/* TRAIN_time in ms */
#define TRAIN_TFLT                    300
#define TRAIN_READY_TIMEOUT           200
#define TRAIN_TFLT_MARGIN             100
#define TRAIN_UPDATE_TIMEOUT_MAGIN    100
#define TRAIN_LTSP_POLL_INTERVAL      1000

/* Link Training Event */
#define TRAIN_EVENT_DISABLE       0
#define TRAIN_EVENT_SUCCESS       1
#define TRAIN_EVENT_SINK_NO_SCDC  2
#define TRAIN_EVENT_READY_TIMEOUT 3
#define TRAIN_EVENT_TFLT_TIMEOUT  4
#define TRAIN_EVENT_LAST_RATE     5
#define TRAIN_EVENT_LTSP_TIMEOUT  6
#define TRAIN_EVENT_DDC_ERR       7

/* see SPEC  Table 10-18: SCDCS - Update Flags */
#define TRAIN_FLT_START_MASK  (0x1 << 4)
#define TRAIN_FLT_UPDATE_MASK (0x1 << 5)

/* see SPEC  Table 10-21: SCDCS - Sink Configuration */
#define TRAIN_FFE_LEVELS_MASK (0xf << 4)
#define TRAIN_FRL_RATE_MASK   (0xf << 0)

/* see SPEC  Table 10-22: SCDCS - Source Test Configuration */
#define TRAIN_FLT_NO_TIMEOUT_MASK (0x1 << 5)
#define TRAIN_FRL_MAX_MASK        (0x1 << 7)

/* see SPEC  Table 10-23: SCDCS - Status Flags */
#define TRAIN_FLT_READY_MASK   (0x1 << 6)
#define TRAIN_LN3_LTP_REQ_MASK (0xf << 4)
#define TRAIN_LN2_LTP_REQ_MASK (0xf << 0)
#define TRAIN_LN1_LTP_REQ_MASK (0xf << 4)
#define TRAIN_LN0_LTP_REQ_MASK (0xf << 0)

#define TIMER_DEFAULT_2MS 0xBB80

struct frl_config {
    hi_bool max_rate_proir;    /* max_frl_rate traning prior */
    hi_u32 ready_timeout;      /* FLT_Ready timeout in LTS 2,default 200ms */
    hi_u32 tflt_margin;        /* tFLT(200ms) margin in LTS 3£¬default 100ms */
    hi_u32 update_flag_magin;  /* FLT_update & FRL_Start timeout margin in LTS P £¬default 100ms */
    hi_u32 ltsp_poll_interval; /* FLT_update polling interval time in LTS P£¬default 150ms */

    /* frl max rate,the lesser of the Max_FRL_Rate from the Sink¡¯s HF-VSDB and
     *  the maximum FRL_Rate the source support
     */
    hi_u32 frl_max_rate;
    hi_u32 frl_min_rate;       /* frl min rate for the transmit mode (vic & color depth & color format) */
    hi_u32 dsc_frl_min_rate;   /* dsc frl min rate for the transmit mode (vic & color depth & color format) */
    hi_u32 sink_version;       /* sink version */
    hi_bool scdc_present;      /* SCDC_Present in HF-VSDB */
    hi_u8 src_ffe_levels;      /* ffe_levels */
};

struct frl_scdc {
    hi_u8 frl_rate; /* FRL_Rate. */
    /*
     * FFE_Levels.The Source shall set this field to indicate the maximum TxFFE level
     *  supported for the current FRL Rate.Values greater than 3 are reserved.
     */
    hi_u8 ffe_levels;
    hi_bool flt_no_timeout; /* FLT_no_timeout */
    hi_bool frl_max;        /* FRL_max. */
    hi_bool flt_update;     /* FLT_update. */
    hi_bool flt_start;      /* FLT_start.Link Training is successful and the Sink is ready to receive video */
    hi_bool flt_ready;      /* FLT_ready.The Sink shall set (=1) FLT_ready when the Sink is ready for Link Training */
    hi_u8 ln_1_0_ltp_req;   /* Ln1_LTP_req & Ln0_LTP_req.Link Training Pattern requested by the Sink for Lane 1 & 0 */
    hi_u8 ln_3_2_ltp_req;   /* Ln3_LTP_req & Ln2_LTP_req.Link Training Pattern requested by the Sink for Lane 3 & 2 */
};

struct frl_stat {
    hi_bool ready_timeout;   /* true-LTS 2 FLT_ready timeout occur;false- timeout didn't occur */
    hi_bool tflt_timeout;    /* true-LTS 3 tFLT timeout occur;false- timeout didn't occur */
    hi_bool ltsp_timeout;    /* true-LTS P polling FRL_Update;false-disable */
    hi_bool ltsp_poll;       /* true-LTS P polling FRL_Update;false-disable */
    hi_bool phy_output;      /* true-phy output signal;false-disable */
    hi_bool video_transifer; /* true-transmiting active normal video etc(WorkEn);false-disable */

    /* true-FRL transmission start, including Gap Characters etc;false-disalbe
     * FRL transmission,select pattern channel.
     */
    hi_bool frl_start;
    hi_u8 ffe_levels[4];     /* FFE level in each lane.[0] for ln0;[1] for ln1;[2] for ln2;[3] for ln3. */
    hi_bool work_3lane;      /* if true,in 3 lane mode;if false,in 4 lane mode */
    hi_u32 frl_state;        /* see Link Training State */
    hi_u32 event;            /* see Link Training Event */
};

struct frl {
    /**
     * @timer
     * use for tFLT\flt_update\flt_ready timeout etc.
     */
    osal_timer timer;

    /**
     * @parent
     * delayed workqueue,use for LTS_P polling FRL_update flag
     */
    struct delayed_work dl_work;

    struct hisilicon_hdmi *hdmi;

    struct hdmi_ddc *ddc;

    /**
     * @base_addr
     * base register address for frl frl
     */
    void *base_addr;

    /**
     * @config
     * link frling config
     */
    struct frl_config config;

    /**
     * @scdc
     * link frling scdc status
     */
    struct frl_scdc scdc;

    /**
     * @status
     * link frling status
     */
    struct frl_stat stat;
};

hi_s32 hitxv300_frl_config_set(struct frl *frl, struct frl_config *config);
hi_s32 hitxv300_frl_config_get(struct frl *frl, struct frl_config *config);
hi_s32 hitxv300_frl_start(struct frl *frl);
void hitxv300_frl_stop(struct frl *frl);
struct frl *hitxv300_frl_init(struct hdmi_controller *parent, void *reg_base);
void hitxv300_frl_exit(struct frl *frl);
hi_s32 hitxv300_frl_scdc_get(struct frl *frl, struct frl_scdc *scdc);
hi_s32 hitxv300_frl_stat_get(struct frl *frl, struct frl_stat *stat);
void hitxv300_frl_worken_set(struct frl *frl, hi_bool enable, hi_bool fast_mode);
hi_s32 hal_frl_debug(struct hisilicon_hdmi *hdmi, enum debug_ext_cmd_list cmd, struct hdmitx_debug_msg msg);
#endif /* __HAL_HDMI_FRL_H__ */
