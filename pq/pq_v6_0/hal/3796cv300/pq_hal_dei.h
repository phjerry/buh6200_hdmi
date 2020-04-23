/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal dei api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_DEI_H__
#define __PQ_HAL_DEI_H__

#include "hi_type.h"
#include "pq_hal_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define s7_to_s32(x)  (((x) > 63) ? ((x) | 0xFFFFFFC0) : (x))
#define s8_to_s32(x)  (((x) > 127) ? ((x) | 0xFFFFFF80) : (x))

extern hi_u32 g_dei_mean_value_array[VPSS_HANDLE_NUM];
/* dei demo mode */
typedef enum {
    DEI_DEMO_ENABLE_L = 0,
    DEI_DEMO_ENABLE_R,

    DEI_DEMO_ENABLE_MAX
} pq_dei_demo_mode;

/* dei demo mode */
typedef enum {
    PQ_MCDI_DISABLE = 0,
    PQ_MAONLY_OUTPUT,
    PQ_MCDI_OUTPUT,

    MC_DEMO_MODE_MAX,
} pq_dei_mc_demo_mode;

hi_s32 pq_hal_enable_dei_demo(hi_u32 handle_no, hi_bool on_off);
hi_s32 pq_hal_set_dei_demo_mode(hi_u32 handle_no, pq_dei_demo_mode mode);
hi_s32 pq_hal_set_dei_demo_mode_coor(hi_u32 handle_no, hi_u32 x_pos);
hi_s32 pq_hal_set_mc_only_enable(hi_u32 handle_no, hi_bool on_off);
hi_s32 pq_hal_get_ma_only_enable(hi_u32 handle_no, hi_bool *on_off);
hi_s32 pq_hal_update_field_flag(hi_u32 handle_no, hi_u32 frm_num);
hi_s32 pq_hal_update_dei_rc_info(hi_u32 handle_no, hi_u32 width, hi_u32 height);
hi_s32 pq_hal_get_dei_stt_info(hi_drv_pq_vpss_stt_info *info_in, drv_pq_dei_input_info *dei_api_input);
hi_s32 pq_hal_update_dei_cfg(hi_u32 handle_no, hi_drv_pq_dei_api_output_reg *dei_api_out);
hi_s32 pq_hal_get_move_scene_wt(hi_s32 *ps32_h_move_scene_wt, hi_s32 *ps32_v_move_scene_wt);
hi_void pq_hal_set_sigma(hi_u32 sigma);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


