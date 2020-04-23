/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: DFX for klad
 * Author: Hisilicon hisecurity team
 * Create: 2019-08-07
 */

#ifndef __DRV_KLAD_TIMESTAMP_H_
#define __DRV_KLAD_TIMESTAMP_H_

#include "drv_klad_com.h"
#include "drv_klad_hw_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef HI_KLAD_PERF_SUPPORT
#define QUEUE_MAX_SIZE 500

typedef struct {
    hi_u32 klad_hw_id;
    hi_u32 klad_handle;
    struct klad_timestamp timestamp;
} queue_elem_type;
#endif

hi_u32 klad_timestamp_queue_init(hi_void);
hi_u32 klad_timestamp_queue_destory(hi_void);
hi_u32 klad_timestamp_queue_clean(hi_void);
hi_u32 klad_timestamp_dump(struct seq_file *sf);
hi_void klad_timestamp_queue(hi_u32 klad_hw_id, hi_u32 klad_handle, struct klad_r_base *base);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __DRV_KLAD_DFX_H_ */

