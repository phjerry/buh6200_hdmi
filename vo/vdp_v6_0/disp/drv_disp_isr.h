/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: isr
* Create: 2019-04-12
 */

#ifndef __DRV_DISP_ISR_H__
#define __DRV_DISP_ISR_H__

#include "hi_drv_disp.h"
#include "hal_disp_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define DEF_DISP_ISR_LIST_LENGTH   50
#define DEF_DISP_ISR_DELAY_TIME_MS 20

typedef enum {
    ISRFUNC_CTRL_FLAG_STOP_WHEN_LOWBAND = 0x00000001,
    ISRFUNC_CTRL_FLAG_STOP_WHEN_SMMU_ERR = 0x00000002,
    ISRFUNC_CTRL_FLAG_STOP_WHEN_MUTE = 0x00000004,
    ISRFUNC_CTRL_FLAG_BUTT = 0xffffffff,
} isr_func_ctrl_flag;

typedef struct {
    hi_u64 node_flag;
    hi_drv_disp_callback isr_node[DEF_DISP_ISR_LIST_LENGTH];
} disp_isr_c_list;

typedef struct {
    hi_drv_disp_callback_type isr_type;
    hi_u32 isr_cnt;
    hi_u64 isr_time_us;
    hi_u64 isr_period_time;
    hi_u64 last_time;
    hi_u32 vtime;
    hi_u32 into_isr_twice_cnt;
    hi_u32 time_out;
    hi_u32 lost_interrupt;
    hi_u32 current_int_cnt;
    hi_u32 pre_int_cnt;
} disp_isr_state_info;

typedef struct {
    hi_u32 isr_delay_time_ms;
    hi_u32 decmp_err;
    hi_u32 bus_err;

    hi_u32 smmu_err_status;
    hi_u32 smmu_err_addr;
    disp_isr_state_info isr_state_info[HI_DRV_DISP_C_TYPE_BUTT];
} disp_isr_debug;

typedef enum {
    DISP_CHECKSUM_TYPE_R = 0,
    DISP_CHECKSUM_TYPE_G,
    DISP_CHECKSUM_TYPE_B,
    DISP_CHECKSUM_TYPE_BUTT
} disp_checksum_type;

typedef struct {
    hi_drv_display disp;
    volatile hi_bool enable;
    volatile disp_isr_c_list isr_list[HI_DRV_DISP_C_TYPE_BUTT];
    hi_drv_disp_callback_info cb_info;
    hi_u32 under_flow;
    hi_u32 smmu_err_cnt;
    disp_isr_debug isr_debug;
    hi_u32 checksum[DISP_CHECKSUM_TYPE_BUTT];
} disp_isr_chn;

typedef struct {
    disp_isr_chn disp_chn[HI_DRV_DISPLAY_BUTT + 1];
    hi_u32 chn_number;
    hi_u32 int_mask_save_suspend;
} disp_isr_m;

hi_s32 disp_isr_init(hi_void);
hi_s32 disp_isr_deinit(hi_void);

hi_s32 disp_isr_suspend(hi_void);
hi_s32 disp_isr_resume(hi_void);

hi_s32 disp_isr_open_chn(hi_drv_display disp);
hi_s32 disp_isr_close_chn(hi_drv_display disp);

hi_s32 disp_isr_reg_callback(hi_drv_display disp, hi_drv_disp_callback_type isr_type,
                             hi_drv_disp_callback *isr_cb);
hi_s32 disp_isr_unreg_callback(hi_drv_display disp, hi_drv_disp_callback_type isr_type,
                               hi_drv_disp_callback *isr_cb);

hi_s32 disp_isr_set_event(hi_drv_display disp, hi_drv_disp_callback_event event);
hi_s32 disp_isr_set_disp_info(hi_drv_display disp, hi_disp_display_info *disp_info);

hi_s32 disp_isr_check_delay_time_ms(hi_drv_display disp, hi_u32 isr_delay_time_ms);
hi_s32 disp_isr_get_isr_state(hi_drv_display disp, hi_u32 *under_flow,
    hi_u32 *smmu_err_cnt, disp_isr_debug *isr_debug);
hi_void disp_isr_set_isr_control_flag(isr_func_ctrl_flag en_isr_control_flag);
hi_void disp_isr_clear_isr_control_flag(isr_func_ctrl_flag en_isr_control_flag);

hi_void disp_isr_free_irq(hi_bool enable);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_DISP_ISR_H__ */

