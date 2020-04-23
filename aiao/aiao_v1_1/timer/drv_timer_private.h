/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao timer driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_TIMER_PRIVATE_H__
#define __DRV_TIMER_PRIVATE_H__

#include "hal_aiao_common.h"

#define TIMER_MAX_TOTAL_NUM 2
#define TIMER_CHNID_MASK    0xffff

#define TIMER_INTERRUPT_PERIOD_DF 5
#define TIMER_INTERRUPT_CONFIG_DF (HI_SAMPLE_RATE_48K / 1000 * TIMER_INTERRUPT_PERIOD_DF)

typedef struct {
    hi_bool enable;
    hi_u32 config;
    aiao_if_timer_attr if_attr;

    hi_void *func;
    hi_void *substream;
} timer_channel_state;

typedef struct {
    hi_bool enable;
    hi_u32 config;
    aiao_sample_rate rate;
    hi_u32 fclk_div;
    hi_u32 bclk_div;

    hi_void *func;
    hi_void *substream;
} snd_timer_settings;

typedef struct {
    timer_channel_state *timer_attr[TIMER_MAX_TOTAL_NUM];
    snd_timer_settings suspend_attr[TIMER_MAX_TOTAL_NUM];
} timer_global_resource;

hi_s32 hi_drv_timer_create(hi_handle *ph_handle, aiao_timer_isr_func *func, hi_void *substream);
hi_s32 hi_drv_timer_destroy(hi_handle handle);
hi_s32 hi_drv_timer_set_attr(hi_handle handle, hi_u32 config, aiao_sample_rate rate);
hi_s32 hi_drv_timer_set_enable(hi_handle handle, hi_bool enable);
hi_s32 hi_drv_timer_get_enable(hi_handle handle, hi_bool *enable);
hi_s32 hi_drv_timer_suspend(hi_void);
hi_s32 hi_drv_timer_resume(hi_void);

hi_s32  timer_drv_init(hi_void);
hi_void timer_drv_deinit(hi_void);

#endif
