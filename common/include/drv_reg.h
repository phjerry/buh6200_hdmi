/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:
 * Author: xuming
 * Create: 2019-5-16
 */

#ifndef __DRV_REG_H__
#define __DRV_REG_H__

#include "hi_type.h"

#include "hi_reg_common.h"

hi_s32  drv_reg_init(hi_void);
hi_void drv_reg_exit(hi_void);

volatile hi_reg_sys_ctrl *drv_sys_get_ctrl_reg_ptr(hi_void);
volatile hi_reg_peri     *drv_sys_get_peri_reg_ptr(hi_void);
volatile hi_reg_io       *drv_sys_get_io_reg_ptr(hi_void);
volatile hi_reg_crg      *drv_sys_get_crg_reg_ptr(hi_void);
hi_u8 *drv_sys_get_otp_base_vir_addr(hi_void);

#endif  /* __DRV_REG_H__ */

