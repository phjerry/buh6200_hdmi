/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal vi
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __HAL_VI_H__
#define __HAL_VI_H__

#include "hi_type.h"
#include "hi_osal.h"

#include "vi_type.h"
#include "vi_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VI_MAX_IP_NUM 2
#define VI_MAX_PORT_NUM 2
#define VI_MAX_IN_PIX_CLK 154000000

#define vi_osal_bit(nr) (1UL << (nr)) /* BIT(nr) */

typedef enum {
    VI_HOST_CAP_FULL_TYPE_SDR_LIMIT = vi_osal_bit(0), /* bit 0 */
    VI_HOST_CAP_FULL_TYPE_SDR_FULL = vi_osal_bit(1),  /* bit 1 */
    VI_HOST_CAP_FULL_TYPE_HDR_LIMIT = vi_osal_bit(2), /* bit 2 */
    VI_HOST_CAP_FULL_TYPE_HDR_FULL = vi_osal_bit(3),  /* bit 3 */

    VI_HOST_CAP_FULL_TYPE_SDR_ALL = (vi_osal_bit(0) | vi_osal_bit(1)), /* bit 0 and 1 */
    VI_HOST_CAP_FULL_TYPE_HDR_ALL = (vi_osal_bit(2) | vi_osal_bit(3)), /* bit 2 and 3 */

    VI_HOST_CAP_FULL_TYPE_ALL = 0xffUL,
} vi_host_cap_full_type;

typedef enum {
    VI_HOST_GEN_DATA_TYPE_OFF = 0,
    VI_HOST_GEN_DATA_TYPE_ONE,
    VI_HOST_GEN_DATA_TYPE_TWO,
    VI_HOST_GEN_DATA_TYPE_TREE,
    VI_HOST_GEN_DATA_TYPE_FOUR,
    VI_HOST_GEN_DATA_TYPE_FIVE,
    VI_HOST_GEN_DATA_TYPE_SIX,
    VI_HOST_GEN_DATA_TYPE_SEVEN,
    VI_HOST_GEN_DATA_TYPE_EIGHT,
    VI_HOST_GEN_DATA_TYPE_NINE,
    VI_HOST_GEN_DATA_TYPE_TEN,

    VI_HOST_GEN_DATA_TYPE_MAX
} vi_host_gen_data_type;

typedef enum {
    VI_HOST_CAP_HDR_TYPE_SDR = vi_osal_bit(0),    /* bit 0 */
    VI_HOST_CAP_HDR_TYPE_HLG = vi_osal_bit(1),    /* bit 1 */
    VI_HOST_CAP_HDR_TYPE_HDR10 = vi_osal_bit(2),  /* bit 2 */
    VI_HOST_CAP_HDR_TYPE_HDR10P = vi_osal_bit(3), /* bit 3 */
    VI_HOST_CAP_HDR_TYPE_DOLBY = vi_osal_bit(4),  /* bit 4 */

    /* support sdr hlg hdr10 */
    VI_HOST_CAP_HDR_TYPE_SDR_HDR = (vi_osal_bit(0) | vi_osal_bit(1) | vi_osal_bit(2)), /* bit 0, 1 and 2 */
    /* support sdr hlg hdr10 hdr10plus */
    VI_HOST_CAP_HDR_TYPE_SDR_HDR_HDR10P = (vi_osal_bit(0) | vi_osal_bit(1) | vi_osal_bit(3)), /* bit 0, 1 and 3 */
    /* support sdr hlg hdr10 dolby */
    VI_HOST_CAP_HDR_TYPE_SDR_HDR_DOLBY = (vi_osal_bit(0) | vi_osal_bit(1) | vi_osal_bit(4)), /* bit 0, 1 and 4 */
    /* support sdr hlg hdr10 dolby */
    VI_HOST_CAP_HDR_TYPE_ALL = 0xffUL,
} vi_host_cap_hdr_type;

typedef enum {
    VI_HOST_CAP_CHECK_HDR_TYPE_NO = 0UL,
    VI_HOST_CAP_CHECK_HDR_TYPE_HDR = vi_osal_bit(0),   /* bit 0 */
    VI_HOST_CAP_CHECK_HDR_TYPE_DOLBY = vi_osal_bit(1), /* bit 1 */

    VI_HOST_CAP_CHECK_HDR_TYPE_ALL = 0xffUL,
} vi_host_cap_check_hdr_type;

typedef enum {
    VI_HOST_CAP_LOW_DELAY_MODE_SOFT = vi_osal_bit(0),   /* bit 0 */
    VI_HOST_CAP_LOW_DELAY_MODE_TUNNEL = vi_osal_bit(1), /* bit 1 */

    VI_HOST_CAP_LOW_DELAY_ALL = 0xffUL,
} vi_host_cap_low_delay_mode;

typedef enum {
    VI_HOST_CAP_SMMU_SHARE = vi_osal_bit(0), /* vicap share smmu, example 350 */
    VI_HOST_CAP_SMMU_ALONE = vi_osal_bit(1), /* vicap use self smmu, example 560 */

    VI_HOST_CAP_SMMU_ALL = 0xffUL,
} vi_host_cap_smmu_type;

typedef struct {
    hi_u32 int_cnt_total; /* total count from start */

    hi_u32 int_f_start_cnt; /* vicap frame start interrupt count */
    hi_u32 int_f_delay_cnt; /* vicap frame delay interrupt count */
    hi_u32 cc_int_cnt;      /* vicap frame complete interrupt count */
    hi_u32 no_cc_int_cnt;   /* vicap frame incomplete interrupt count */
    hi_u32 update_cfg_cnt;
    hi_u32 no_update_cfg_cnt;

    hi_u32 filed_lost_cnt;
    hi_u32 buf_overflow_cnt;
    hi_u32 ll_err_cnt;

    hi_u32 int_smmu_err_cnt; /* vicap interrupt SMMU error count */

    hi_u32 mute_cnt;

    hi_u32 int_time; /* interrupt handling time */
    hi_u32 int_time_max;
    hi_u32 int_time_min;

    hi_u32 int_time_total; /* interrupt total handling time */

    hi_u32 int_time_interval; /* interrupt interval, current time - last interrupt time */
    hi_u32 int_time_interval_max;
    hi_u32 int_time_interval_min;

    osal_timeval int_time_first; /* first interrupt start time */
    osal_timeval int_time_last;  /* last interrupt start time */
} vi_host_dbg_info;

typedef struct {
    hi_u32 cap_in_max_pix_clk;
    hi_u32 cap_in_max_width;
    hi_u32 cap_in_max_height;

    vi_host_cap_smmu_type cap_smmu_type;
    vi_host_cap_hdr_type cap_in_hdr_type;
    vi_host_cap_full_type cap_out_full_type;
    vi_host_cap_check_hdr_type cap_check_hdr_type;
    vi_host_cap_low_delay_mode cap_low_delay_mode;
} vi_host_capabilities;

typedef struct {
    hi_bool is_invalid;
    hi_bool is_video_err;
    hi_bool hdr_check_err;
    hi_bool dolby_check_err;
} vi_host_status;

typedef struct {
    hi_u32 cur_filed_type;
    hi_u32 dolby_pack_num;
    hi_u32 hdr_metadata_len;
    hi_u32 in_width[VI_MAX_PORT_NUM];
    hi_u32 in_height[VI_MAX_PORT_NUM];

    hi_u32 int_write_line_num; /* write back frame line num */
    hi_u32 y_checksum;
    hi_u32 c_checksum;
} vi_host_statistics;

typedef struct {
    vi_host_capabilities host_caps;

    vi_host_status host_status;
    vi_host_statistics host_statistics;

    vi_host_dbg_info host_dbg_info;

    hi_drv_pq_csc_info csc_info;
    hi_drv_pq_csc_coef pq_csc_coef;

    hi_u32 chn_int;
    hi_u32 mmu_int;
    hi_bool mix_mode; /* 通道混合模式 */

    vi_fun call_back;
    vi_instance* vi_instance_p;

    hi_u32 vicap_id; /* vicap logic ip id */
    hi_u32 work_clk;
    hi_u32 irq_num;
    hi_char* irq_name;
} vi_host_instance;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
