/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file for ir.c
 */

#ifndef __IR_H__
#define __IR_H__

#include "hi_type.h"

#define IR_EN 0x0
#define IR_CFG 0x04
#define IR_LEADS 0x08
#define IR_LEADE 0x0c
#define IR_SLEADE 0x10
#define IR_B0 0x14
#define IR_B1 0x18
#define IR_BUSY 0x1c
#define IR_DATAH 0x20
#define IR_DATAL 0x24
#define IR_INT_MASK 0x28
#define IR_INT_STATUS 0x2c
#define IR_INT_CLR 0x30
#define IR_START 0x34

#define IR_PMOC_NUM 0x6

extern hi_u8 g_ir_type;
extern hi_u8 g_ir_pmocnum;
extern __idata hi_u32_data g_ir_high_value[IR_PMOC_NUM];
extern __idata hi_u32_data g_ir_low_value[IR_PMOC_NUM];

void ir_start(void);
void ir_disable(void);
void ir_init(void);
void ir_isr(void);
#endif

