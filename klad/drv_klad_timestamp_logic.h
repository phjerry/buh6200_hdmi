/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: DFX for klad
 * Author: Hisilicon hisecurity team
 * Create: 2019-08-07
 */

#ifndef __DRV_KLAD_TIMESTAMP_LOGIC_H_
#define __DRV_KLAD_TIMESTAMP_LOGIC_H_

#include "drv_klad_com.h"
#include "drv_klad_hw_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef HI_KLAD_PERF_SUPPORT
#define QUEUE_MAX_SIZE 500

typedef enum {
    LOCK         = 0x00,
    COM_LOCK,
    CLR_LOCK,
    TA_LOCK,
    NONCE_LOCK,
    FP_LOCK,
    RKP          = 0x10,
    ALG_IP       = 0x20,
    COM_ALG_IP,
    TA_ALG_IP,
    NONCE_ALG_IP,
    FP_ALG_IP,
} logic_stamp_tag;

struct klad_timestamp_logic {
    logic_stamp_tag tag;
    struct time_ns time_ctl_b;
    struct time_ns time_ctl_e;
    struct time_ns time_int_b;
    struct time_ns time_int_e;
    struct time_ns time_wait_b;
    struct time_ns time_wait_e;
};
#endif

hi_u32 klad_timestamp_logic_queue_init(hi_void);
hi_u32 klad_timestamp_logic_queue_destory(hi_void);
hi_u32 klad_timestamp_logic_queue_clean(hi_void);
hi_u32 klad_timestamp_logic_dump(struct seq_file *sf);
hi_void klad_timestamp_logic_queue(struct klad_timestamp_logic *timestamp);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __DRV_KLAD_TIMESTAMP_LOGIC_H_ */

