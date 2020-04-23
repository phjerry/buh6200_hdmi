/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal tnr define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_TNR_H__
#define __PQ_HAL_TNR_H__

#include "pq_hal_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_bool dci_en;
    hi_pq_dci_histgram dci_histgram;
} pq_tnr_dci_status;

extern pq_tnr_dci_status g_tnr_dci_info[VPSS_HANDLE_NUM];

/* tnr demo mode */
typedef enum {
    TNR_DEMO_ENABLE_L = 0,
    TNR_DEMO_ENABLE_R
} tnr_demo_mode;

/* reginal motion vector define */
typedef struct {
    hi_s32       mv_x;        /* ???????¡¥¡ê?¡¤??¡ì?a[7:0] */
    hi_u32       sad;          /* [9:0] */
    hi_u32       mag;        /* [9:0] */
    hi_s32       mv_y;      /* ¡ä1?¡À???¡¥¡ê????¨ª?a[6:0] */
}pq_nr_rgmv_info;

hi_s32 pq_hal_set_nr_vir_reg(hi_void);

hi_s32 pq_hal_set_tnr_str(hi_u32 handle_no, hi_u32 tnr_str);

hi_s32 pq_hal_enable_tnr(hi_u32 handle_no, hi_bool nr_on_off);

hi_s32 pq_hal_enable_tnr_demo(hi_u32 handle_no, hi_bool tnr_demo_en);

hi_s32 pq_hal_set_tnr_demo_mode(hi_u32 handle_no, tnr_demo_mode mode);

hi_s32 pq_hal_set_tnr_demo_mode_coor(hi_u32 handle_no, hi_u32 x_pos);

hi_s32 pq_hal_update_tnr_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input);

hi_s32 pq_hal_update_tnr_cfg(hi_u32 handle, hi_drv_pq_nr_api_output_reg *nr_out_reg);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

