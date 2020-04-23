/*
 * * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * * Description: header file of win_frc Component
 * * Author: image team
 * * Create: 2019-07-2
 * *
 */
#ifndef  __DRV_WIN_FRC__
#define  __DRV_WIN_FRC__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32 speed_integer;
    hi_u32 speed_decimal;
    hi_u32 in_rate;
    hi_u32 out_rate;
} win_frc_input_attr;

typedef struct {
    hi_u32 need_play_cnt;
} win_frc_output_result;

hi_s32 win_frc_init(void);

hi_s32 win_frc_deinit(void);

hi_handle win_frc_create(void);

hi_s32  win_frc_destory(hi_handle h_frc);

hi_s32 win_frc_calcute(hi_handle h_frc, win_frc_input_attr *in_attr, win_frc_output_result *result);

hi_void win_frc_proc_info(hi_handle h_frc, struct seq_file *p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


