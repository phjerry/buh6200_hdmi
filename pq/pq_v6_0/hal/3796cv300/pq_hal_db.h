/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: db hal layer header file
 * Author: pq
 * Create:
 */

#ifndef __PQ_HAL_DB_H__
#define __PQ_HAL_DB_H__

#include "hi_drv_pq.h"
#include "drv_pq_ext.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DB_BIT_DEPTH 10


hi_s32 pq_hal_db_get_detect_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, dbd_input_info *db_detect_info);
hi_s32 pq_hal_db_get_filter_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, db_input_info *db_filter_info);
hi_s32 pq_hal_db_set_detect_api_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info, dbd_output_info *detect_output_reg);
hi_s32 pq_hal_db_set_filter_api_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info, db_output_info *filter_output_reg);

hi_s32 pq_hal_db_set_strength(hi_u32 handle_id, hi_u32 strength);
hi_s32 pq_hal_db_set_en(hi_u32 handle_id, hi_bool enable);

hi_s32 pq_hal_db_set_demo_enable(hi_u32 handle_id, hi_bool on_off);
hi_s32 pq_hal_db_set_demo_mode(hi_u32 handle_id, pq_demo_mode demo_mode);
hi_s32 pq_hal_db_set_demo_pos(hi_u32 handle_id, hi_u32 x_pos);

hi_s32 pq_hal_db_set_boarder_flag(hi_drv_pq_vpss_stt_info *vpss_stt_info);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



