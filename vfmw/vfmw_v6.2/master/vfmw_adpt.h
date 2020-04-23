/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_ADPT_H__
#define __VFMW_ADPT_H__

#include "vfmw.h"
#include "vfmw_osal.h"

typedef enum {
    VFMW_STATE_NULL = 0,
    VFMW_STATE_OPEN,
    VFMW_STATE_BUTT
} vfmw_state;

typedef enum {
    VFMW_CHAN_STATE_NULL = 0,
    VFMW_CHAN_STATE_WAIT,
    VFMW_CHAN_STATE_CREATE,
    VFMW_CHAN_STATE_BUTT
} vfmw_chan_state;

typedef enum {
    VFMW_TYPE_LOCAL,
    VFMW_TYPE_TEE,
    VFMW_TYPE_MCU,
    VFMW_TYPE_USER,
    VFMW_TYPE_BUFF
} vfmw_type;

typedef struct {
    hi_u32 begin;
    hi_u32 end;
    hi_u32 cost;
} vfmw_intf_cost;

typedef struct {
    vfmw_intf_cost create;
    vfmw_intf_cost config;
    vfmw_intf_cost start;
    vfmw_intf_cost questm;
    vfmw_intf_cost getfrm;
    vfmw_intf_cost stop;
    vfmw_intf_cost reset;
    vfmw_intf_cost destroy;
} vfmw_kpi;

typedef struct {
    hi_u32 save_begin;
    hi_u32 save_end;
} vfmw_dflt;

typedef struct {
    hi_u32 id;
    hi_u32 frm_cnt;
    OS_SEMA sema;
    vfmw_chan_state state;
    vfmw_frm_alloc_type alloc_type;
    vfmw_vpp_work_mode work_mode;
    vfmw_type type;
    hi_bool is_sec;
    vfmw_kpi kpi;
    OS_FILE *yuv;
    OS_FILE *mtdt;
} vfmw_chan;

typedef struct {
    hi_u32 reserve1;

    OS_SEMA sema;
    hi_s32 ref_cnt;
    hi_s32 chan_cnt;
    vfmw_state state;
    vfmw_chan chan[VFMW_CHAN_NUM];
    vfmw_dflt dflt;

    vfmw_intf_cost init_kpi;
    vfmw_intf_cost exit_kpi;

    fun_vfmw_event_report event_report_vdec;
    fun_vfmw_event_report event_report_omxvdec;

    hi_u32 reserve2;
} vfmw_entry;

hi_s32 vadpt_open(hi_void);
hi_s32 vadpt_close(hi_void);
hi_s32 vadpt_init(hi_void *);
hi_s32 vadpt_exit(hi_void *);
hi_s32 vadpt_suspend(hi_void *);
hi_s32 vadpt_resume(hi_void *);
hi_s32 vadpt_get_frame(hi_s32, hi_void *, hi_void *);
hi_s32 vadpt_release_frame(hi_s32, const hi_void *);
hi_s32 vadpt_control(hi_s32, hi_s32, hi_void *, hi_u32);

vfmw_entry *vadpt_entry(hi_void);
vfmw_chan *vadpt_chan(hi_s32);
vfmw_dflt *vadpt_get_dflt(hi_void);

#endif /* __VFMW_ADPT_H__ */
