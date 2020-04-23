/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:keyladder session node list manager.
 * Author: Hisilicon hisecurity team
 * Create: 2019-06-25
 */
#ifndef __DRV_KLAD_SW_H__
#define __DRV_KLAD_SW_H__

#include "drv_klad_sw_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

hi_s32 fmw_klad_com_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_ta_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_fp_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_nonce_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_clr_process(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_com_create(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_ta_create(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_fp_create(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_nonce_create(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_com_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_ta_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_fp_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data);
hi_s32 fmw_klad_nonce_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif  /* __DRV_KLAD_SW_H__ */
