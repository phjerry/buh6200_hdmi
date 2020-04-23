/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal snr api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_SNR_H__
#define __PQ_HAL_SNR_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    SNR_DEMO_ENABLE_L = 0,
    SNR_DEMO_ENABLE_R
} snr_demo_mode;

hi_s32 pq_hal_set_snr_str(hi_u32 data, hi_u32 snr_str);

hi_s32 pq_hal_enable_snr(hi_u32 handle_no, hi_bool snr_on_off);

hi_s32 pq_hal_enable_snr_demo(hi_u32 handle_no, hi_bool snr_demo_en);

hi_s32 pq_hal_set_snr_demo_mode(hi_u32 handle_no, snr_demo_mode mode);

hi_s32 pq_hal_set_snr_demo_mode_coor(hi_u32 handle_no, hi_u32 x_pos);

hi_s32 pq_hal_update_snr_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input);

hi_s32 pq_hal_update_snr_cfg(hi_u32 handle, hi_drv_pq_nr_api_output_reg *nr_out_reg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

