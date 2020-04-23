/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: debug manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_DEBUG__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_DEBUG__

#include "hi_osal.h"
#include "hi_gfx_sys_k.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void tde_out_msg(const hi_char *func, const hi_u32 line, const GRAPHIC_DFX_TYPE_E msg_type, const hi_u32 format,
                    ...);

hi_void tde_set_log_func(hi_char *func);
hi_void tde_set_log_level(hi_u32 log_level);
hi_void tde_set_log_save(hi_u32 log_save);
hi_void tde_out_set_log_info(struct seq_file *p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* *__SOURCE_MSP_DRV_TDE_INCLUDE_DEBUG__* */
