/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: JPGD reg define
 */

#ifndef __DRV_JPGD_INCLUDE_REG__
#define __DRV_JPGD_INCLUDE_REG__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define JPGD_REG_BASEADDR 0x1510000
#define JPGD_IRQ_NUM (111 + 32)
#define JPGD_MMU_REG_BASEADDR (0x1510000 + 0xF000)

#define JPGD_CLOCK_SELECT 0x000
#define JPGD_CLOCK_ON 0x1
#define JPGD_CLOCK_OFF 0xFFFFFFFE
#define JPGD_RESET_REG_VALUE 0x10
#define JPGD_UNRESET_REG_VALUE 0xFFFFFFEF
#define JPGD_REG_LENGTH 0x6F0
#define JPGD_CRG_REG_LENGTH 0x4
#define JPGD_REG_START 0x0
#define JPGD_REG_RESUME 0x4
#define JPGD_REG_PICVLDNUM 0x8
#define JPGD_REG_STRIDE 0xC
#define JPGD_REG_PICSIZE 0x10
#define JPGD_REG_PICTYPE 0x14
#define JPGD_REG_TIME 0x18
#define JPGD_REG_STADDR 0x20
#define JPGD_REG_ENDADDR 0x24
#define JPGD_REG_STADD 0x28
#define JPGD_REG_ENDADD 0x2C
#define JPGD_REG_YSTADDR 0x30
#define JPGD_REG_UVSTADDR 0x34
#define JPGD_REG_SCALE 0x40
/* ARGB8888 YUV420SP */
#define JPGD_REG_OUTTYPE 0x44
/* ARGB8888 */
#define JPGD_REG_ARGBOUTSTRIDE 0x1C
#define JPGD_REG_ALPHA 0x48
#define JPGD_REG_OUTSTARTPOS 0xd8
#define JPGD_REG_OUTENDPOS 0xdc
#define JPGD_REG_MINADDR 0xc8
#define JPGD_REG_MINADDR1 0xcc
#define JPGD_REG_HORCOEF00_01 0x4C
#define JPGD_REG_HORCOEF02_03 0x50
#define JPGD_REG_HORCOEF04_05 0x54
#define JPGD_REG_HORCOEF06_07 0x58
#define JPGD_REG_HORCOEF20_21 0x6C
#define JPGD_REG_HORCOEF22_23 0x70
#define JPGD_REG_HORCOEF24_25 0x74
#define JPGD_REG_HORCOEF26_27 0x78
#define JPGD_REG_VERCOEF00_01 0x8C
#define JPGD_REG_VERCOEF02_03 0x90
#define JPGD_REG_VERCOEF10_11 0x94
#define JPGD_REG_VERCOEF12_13 0x98
#define JPGD_REG_VERCOEF20_21 0x9C
#define JPGD_REG_VERCOEF22_23 0xA0
#define JPGD_REG_VERCOEF30_31 0xA4
#define JPGD_REG_VERCOEF32_33 0xA8
#define JPGD_REG_CSC00_01 0xB4
#define JPGD_REG_CSC02_10 0xB8
#define JPGD_REG_CSC11_12 0xBC
#define JPGD_REG_CSC20_21 0xC0
#define JPGD_REG_CSC22 0xC4
#define JPGD_REG_INT 0x100
#define JPGD_REG_INTMASK 0x104
#define JPGD_REG_INTMASK_VALUE 0x20
#define JPGD_REG_DEBUG 0x108
#define JPGD_REG_LPIXSUM0 0x114
#define JPGD_REG_LPIXSUM1 0x118

#define JPGD_REG_AXI4KCNT 0x11C
#define JPGD_REG_AXI16MCNT 0x120
#define JPGD_REG_SAMPLINGFACTOR 0x124
#define JPGD_REG_DRI 0x128
#define JPGD_REG_DRI_CNT_CFG 0x12C
#define JPGD_REG_DRI_CNT 0x130
#define JPGD_REG_MMU_BYPASS 0x134
#define JPGD_REG_LINE_NUM 0x138
#define JPGD_REG_LINE_ADDR 0x13C
#define JPGD_REG_ERR_STREAM_EN 0x154
#define JPGD_REG_OUTPUT_HEIGHT 0x168
#define JPGD_REG_QUANT 0x200
#define JPGD_REG_HDCTABLE 0x300
#define JPGD_REG_HACMINTABLE 0x340
#define JPGD_REG_HACBASETABLE 0x360
#define JPGD_REG_HACSYMTABLE 0x400

#define JPGD_REG_PD_CBCR_CFG 0x38
#define JPGD_REG_PD_Y_CFG 0x3c
#define JPGD_REG_MCUY_CFG 0xd4
#define JPGD_REG_MCUY 0xe0
#define JPGD_REG_BSRES 0xe4
#define JPGD_REG_BSRES_DATA0 0xe8
#define JPGD_REG_BSRES_DATA1 0xec
#define JPGD_REG_BSRES_BIT 0xf0
#define JPGD_REG_BSRES_DATA0_CFG 0xf4
#define JPGD_REG_BSRES_DATA1_CFG 0xf8
#define JPGD_REG_BSRES_BIT_CFG 0xfc
#define JPGD_REG_PD_Y 0x10c
#define JPGD_REG_PD_CBCR 0x110

hi_u32 jpeg_reg_read(hi_u32 offset);
hi_void jpeg_reg_write(hi_u32 offset, hi_u64 data);
hi_void jpeg_reg_cpy_data(const hi_char *data_buf, hi_u32 data_size, hi_u32 offset);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
