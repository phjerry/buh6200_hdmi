/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __MDC_CLIENT_H__
#define __MDC_CLIENT_H__

#include "vfmw.h"

#include "mdc_private.h"

typedef enum {
    CHAN_INVALID = 0,
    CHAN_START,
    CHAN_STOP,
    CHAN_BUTT,
} mdc_chan_state;

typedef struct {
    OS_SEMA sema;
    OS_EVENT event;
    hi_s32 chan_id;
    hi_s32 vid_std;
    hi_s32 ack_value;
    hi_s32 is_sec;
    hi_s32 valid;
    OS_LOCK chan_lock;

    hi_void *chan_comm_share_mem;
    hi_u32 chan_comm_share_mem_size;
    vfmw_chan_option *chan_opt_external;
    mdc_create_param create_param;
    vfmw_shr_chn_mem chan_share_mem;
    vfmw_mem_desc chan_ctx_mem;
    mdc_chan_state chan_state;
    hi_handle vdec_handle;
    hi_handle ssm_handle;
} mdc_client_chan;

typedef struct {
    fun_vfmw_event_report event_report_vdec;
    fun_vfmw_event_report event_report_omxvdec;
} mdc_call_back;

typedef struct {
    OS_SEMA sema;
    hi_s32 inited;
    hi_s32 ref_cnt;
    UADDR  ree_pt_addr;
    hi_s32 seg_buf_size;
    hi_s32 dev_hal_size;
    hi_s32 chan_max_size;
    hi_s32 chan_share_mem_size;
    hi_s32 chan_share_mem_cnt;
    hi_s32 chan_ctx_mem_cnt;
    hi_s32 created_chan_num[MDC_INDEX_MAX];
    hi_s8  *proc;
    hi_void   *comm_share_mem;
    hi_u32 comm_share_mem_size;
    hi_void   *tmp_create_share_mem;
    hi_u32 tmp_create_share_mem_size;

    mdc_thread thread_ctx;
    mdc_call_back call_back;
    shr_glb_mem glb_share_mem;
    vfmw_mem_desc dev_hal_mem[MDC_INDEX_MAX];
    mdc_client_chan chan_ctx[VFMW_CHAN_NUM];

    hi_u32 wait_time;
    hi_u32 process_evt_time;
    hi_u32 get_stm_time;
    hi_u32 rls_stm_time;
    hi_u32 chk_state_time;
} mdc_ctrl_ctx_c;

mdc_ctrl_ctx_c *mdc_get_ctx(hi_void);
hi_s32 mdc_client_open(hi_void);
hi_s32 mdc_client_close(hi_void);
hi_s32 mdc_client_init(hi_void *args, hi_u32 length);
hi_s32 mdc_client_exit(hi_void *args, hi_u32 length);
hi_s32 mdc_client_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 length);
hi_s32 mdc_client_suspend(hi_void *args, hi_u32 length);
hi_s32 mdc_client_resume(hi_void *args, hi_u32 length);
hi_s32 mdc_client_get_image(hi_s32 chan_id, vfmw_image *image);
hi_s32 mdc_client_rel_image(hi_s32 chan_id, const vfmw_image *image);
hi_s32 mdc_client_check_image(hi_s32 chan_id, vfmw_image *image);

#endif
