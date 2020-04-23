/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aoe hal common header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HAL_AOE_COMMON_H__
#define __HAL_AOE_COMMON_H__

#include "hi_drv_audio.h"
#include "hi_audsp_aoe.h"

/* AOE AIP definition */
typedef enum {
    AOE_AIP0 = 0x00,
    AOE_AIP1 = 0x01,
    AOE_AIP2 = 0x02,
    AOE_AIP3 = 0x03,
    AOE_AIP4 = 0x04,
    AOE_AIP5 = 0x05,
    AOE_AIP6 = 0x06,
    AOE_AIP7 = 0x07,

    AOE_AIP_MAX = AOE_MAX_AIP_NUM,
} aoe_aip_id;

/* AOE AOP definition */
typedef enum {
    AOE_AOP0 = 0x00,
    AOE_AOP1 = 0x01,
    AOE_AOP2 = 0x02,
    AOE_AOP3 = 0x03,
    AOE_AOP4 = 0x04,
    AOE_AOP5 = 0x05,
    AOE_AOP6 = 0x06,
    AOE_AOP7 = 0x07,

    AOE_AOP_MAX = AOE_MAX_AOP_NUM,
} aoe_aop_id;

/* AOE ENGINE definition */
typedef enum {
    AOE_ENGINE0 = 0x00,
    AOE_ENGINE1 = 0x01,
    AOE_ENGINE2 = 0x02,
    AOE_ENGINE3 = 0x03,
    AOE_ENGINE4 = 0x04,
    AOE_ENGINE5 = 0x05,

    AOE_ENGINE_MAX = AOE_MAX_ENGINE_NUM,
} aoe_engine_id;

/* defines the status of AIP */
typedef enum {
    /* stop */ /* CNcomment: 停止 */
    AOE_AIP_STATUS_STOP = 0x1,

    /* start */ /* CNcomment: 运行 */
    AOE_AIP_STATUS_START = 0x2,
    AOE_AIP_STATUS_PAUSE = 0x4,

    AIP_STATUS_MAX = 0x8,
} aoe_aip_status;

/* defines the  status of an AOP. */
typedef enum {
    /* stop */ /* CNcomment: 停止 */
    AOE_AOP_STATUS_STOP = 0,

    /* start */ /* CNcomment: 运行 */
    AOE_AOP_STATUS_START,

    AOE_AOP_STATUS_MAX
} aoe_aop_status;

/* defines the status of engine */
typedef enum {
    /* <stop */ /* <CNcomment: 停止 */
    AOE_ENGINE_STATUS_STOP = 0,

    /* <start */ /* <CNcomment: 运行 */
    AOE_ENGINE_STATUS_START,

    AOE_ENGINE_STATUS_MAX
} aoe_engine_status;

typedef struct {
    hi_u64 buf_phy_addr;
    hi_u64 buf_phy_wptr;
    hi_u64 buf_phy_rptr;
    hi_u64 buf_vir_addr;
    hi_u64 buf_vir_wptr;
    hi_u64 buf_vir_rptr;
    hi_u32 buf_wptr_rptr_flag; /* 0: buf_wptr & buf_rptr located at AIP reg */
    hi_u32 buf_size;
} aoe_rbuf_attr;

typedef struct {
    aoe_rbuf_attr rbf_attr;
    hi_u32 buf_bit_per_sample; /* bit per sample */ /* CNcomment: 位宽 */
    hi_u32 buf_channels;       /* number of channels */ /* CNcomment: 声道数 */
    hi_u32 buf_sample_rate;    /* sampling rate */ /* CNcomment: 采样频率 */
    hi_u32 buf_data_format;    /* 0, linear pcm, 1, iec61937 */
    hi_u32 buf_latency_thd_ms; /* 40 ~ 1000 ms */
    hi_u32 start_threshold_ms;
    hi_u32 resume_threshold_ms;
    hi_u32 fade_in_ms;
    hi_u32 fade_out_ms;
    hi_bool fade_enable;
    aoe_aip_type aip_type;
    hi_bool mix_priority; /* TRUE: high priority */
} aoe_aip_in_buf_attr;

typedef struct {
    hi_u32 fifo_bit_per_sample; /* bit per sample */ /* CNcomment: 位宽 */
    hi_u32 fifo_channels;       /* number of channels */ /* CNcomment: 声道数 */
    hi_u32 fifo_sample_rate;    /* sampling rate */ /* CNcomment: 采样频率 */
    hi_u32 fifo_data_format;    /* 0, linear pcm, 1, iec61937 */
    hi_u32 fifo_latency_thd_ms; /* 6 ~ 40 ms */
} aoe_aip_out_fifo_attr;

typedef struct {
    aoe_aip_in_buf_attr buf_in_attr;
    aoe_aip_out_fifo_attr fifo_out_attr;
    hi_bool fifo_bypass;
} aoe_aip_chn_attr;

typedef struct {
    aoe_rbuf_attr rbf_attr;
    hi_u32 buf_bit_per_sample; /* bit per sample */ /* CNcomment: 位宽 */
    hi_u32 buf_channels;       /* number of channels */ /* CNcomment: 声道数 */
    hi_u32 buf_sample_rate;    /* sampling rate */ /* CNcomment: 采样频率 */
    hi_u32 buf_data_format;    /* 0, linear pcm, 1, iec61937 */
    hi_u32 buf_latency_thd_ms; /* 10 ~ 40 ms */
} aoe_aop_out_buf_attr;

typedef struct {
    aoe_aop_out_buf_attr rbf_out_attr;
    hi_bool rbf_hw_priority; /* TRUE: high priority */

    hi_bool is_cast;  /* HI_TRUE:aop is cast ; HI_FALSE:aop is normal aop */
    hi_bool add_mute; /* just for cast aop */
} aoe_aop_chn_attr;

typedef struct {
    hi_u32 bit_per_sample; /* bit per sample */ /* CNcomment: 位宽 */
    hi_u32 channels;       /* number of channels */ /* CNcomment: 声道数 */
    hi_u32 sample_rate;    /* sampling rate */ /* CNcomment: 采样频率 */
    hi_u32 data_format;    /* 0, linear pcm, 1, iec61937 */
} aoe_engine_attr;

#endif  /* __HAL_AOE_COMMON_H__ */
