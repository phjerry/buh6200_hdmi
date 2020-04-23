/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp command fifo implementation header file
 * Author: Audio
 * Create: 2019-11-11
 * Notes: NA
 * History: 2019-11-11 first version for tplay
 */

#ifndef __DRV_ADSP_CMD_H__
#define __DRV_ADSP_CMD_H__

#include "hi_type.h"
#include "hi_audsp_aoe.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 adsp_write_cmd(adsp_cmd_arg *info);
hi_s32 adsp_cmd_fifo_init(aoe_regs_type *aoe_reg, hi_bool sw_aoe);
hi_void adsp_cmd_fifo_deinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_ADSP_CMD_H__ */
