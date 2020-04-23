/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of drv_pmoc
 */

#ifndef __DRV_PMOC_H__
#define __DRV_PMOC_H__

#include <linux/uaccess.h>
#include "hi_type.h"
#include "hi_drv_pmoc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32 wakeup_type;
    hi_u32 gpio_wakeup_port;
    hi_u32 ir_low_val;
    hi_u32 ir_high_val;
    hi_u32 standby_period;
} pmoc_wakeup_message;

typedef struct {
    /* TBD: */
    pmoc_wakeup_message wakeup_message;
} pmoc_proc_info;

hi_s32 pmoc_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 pmoc_tsensor_init(hi_void);
hi_s32 pmoc_tsensor_read(hi_s16 *temperature);
hi_s32 pmoc_get_proc_info(pmoc_proc_info *info);
hi_void pmoc_get_wakeup_message(hi_void);
hi_void pmoc_set_lpmcu_dbg_level(hi_u32 value);
hi_s32 pmoc_set_wakeup_type_to_hrf(hi_pmoc_wakeup_type type);
hi_s32 pmoc_load_standby_params(hi_void);
hi_s32 pmoc_load_lpmcu(hi_void);
hi_s32 pmoc_sem_init(hi_void);
hi_s32 pmoc_register_remap(hi_void);
hi_void pmoc_register_unmap(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_PMOC_H__ */
