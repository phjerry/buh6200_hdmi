/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao cast hal header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HAL_CAST_H__
#define __HAL_CAST_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AIAO_CAST_BUFSIZE_MIN 2048

typedef enum {
    AIAO_CAST_0 = 0x00,
    AIAO_CAST_1 = 0x01,
    AIAO_CAST_2 = 0x02,
    AIAO_CAST_3 = 0x03,

    AIAO_CAST_MAX = 0x4,
} aiao_cast_id;

typedef enum {
    AIAO_CAST_STATUS_STOP = 0,
    AIAO_CAST_STATUS_START,
    AIAO_CAST_STATUS_CAST_MAX,
} aiao_cast_status;

/* cast mmz buffer */
typedef struct {
    hi_u32 buf_size;
    hi_u64 buf_phy_addr;
    hi_u64 buf_vir_addr;
    hi_u64 wptr_addr;
    hi_u64 rptr_addr;
} aiao_cast_mem_attr;

typedef struct {
    aiao_cast_mem_attr ext_dma_mem;
    hi_u32 buf_bit_per_sample;      /* bit per sampling */ /* CNcomment: 采样位宽 */
    hi_u32 buf_channels;            /* number of channels */ /* CNcomment: 声道数 */
    hi_u32 buf_sample_rate;         /* sampling rate */ /* CNcomment: 采样频率 */
    hi_u32 buf_data_format;         /* 0, linear pcm; 1, iec61937 */
    hi_u32 buf_latency_thd_ms;      /* 40 ~ 1000 ms */

    hi_bool add_mute;
} aiao_cast_attr;

/* global function */
hi_s32 hal_cast_init(hi_void);
hi_void hal_cast_deinit(hi_void);

/* port function */
hi_s32 hal_cast_create(aiao_cast_id *id, aiao_cast_attr *attr);
hi_void hal_cast_destroy(aiao_cast_id id);
hi_s32 hal_cast_set_attr(aiao_cast_id id, aiao_cast_attr *attr);
hi_s32 hal_cast_get_attr(aiao_cast_id id, aiao_cast_attr *attr);
hi_s32 hal_cast_start(aiao_cast_id id);
hi_s32 hal_cast_stop(aiao_cast_id id);
/* port buffer function */
hi_u32 hal_cast_read_data(aiao_cast_id id, hi_u32 *offset, hi_u32 size);
hi_u32 hal_cast_release_data(aiao_cast_id id, hi_u32 size);
hi_u32 hal_cast_query_buf_data(aiao_cast_id id);
hi_u32 hal_cast_query_buf_free(aiao_cast_id id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HAL_CAST_H__ */
