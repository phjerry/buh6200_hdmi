/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2007-2019. All rights reserved.
 * Description: system interface.
 * Author: guoqingbo
 * Create: 2007-1-31
 */

#ifndef __HI_REG_COMMON_H__
#define __HI_REG_COMMON_H__

#include "hi_type.h"

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#include "./hi3796cv300/hi_reg_sys.h"
#include "./hi3796cv300/hi_reg_peri.h"
#include "./hi3796cv300/hi_reg_io.h"
#include "./hi3796cv300/hi_reg_crg.h"
#endif

#define HI_REG_SYS_BASE_ADDR  0x00840000
#define HI_REG_SYS_END_ADDR   0x00841FFF

#define HI_REG_PERI_BASE_ADDR 0x00A10000
#define HI_REG_PERI_END_ADDR  0x00A10FFF

#define HI_REG_IO_BASE_ADDR   0x00A11000
#define HI_REG_IO_END_ADDR    0x00A11FFF

#define HI_REG_CRG_BASE_ADDR  0x00A00000
#define HI_REG_CRG_END_ADDR   0x00A00FFF

#define HI_REG_DDR_BASE_ADDR  0x00D00000
#define HI_REG_DDR_END_ADDR   0x00D1FFFF

#define HI_REG_OTP_BASE_ADDR  0x00B00000
#define HI_REG_OTP_END_ADDR   0x00B04FFF

typedef S_SYSCTRL_REGS_TYPE  hi_reg_sys_ctrl;
typedef S_PERICTRL_REGS_TYPE hi_reg_peri;
typedef S_IO_REGS_TYPE       hi_reg_io;
typedef S_CRG_REGS_TYPE      hi_reg_crg;

#endif /* __HI_REG_COMMON_H__ */

