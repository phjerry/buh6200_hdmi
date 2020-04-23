#ifndef __REG51_H__
#define __REG51_H__
#include "hi_type.h"

/* Special register define */
/* CNcomment: 特殊功能寄存器定义 */
__sfr __no_init volatile unsigned char CMD_STATUS_REG @ 0x91;

/*
 * [0]: Cmd Valid
 * [1]: read/write flag
 *      1:write,
 *      0: read
 * [2]: access type
 *      1: access isntruction ram,
 *      0: access ahb bus
 * [7:3]: reserved
 */
__sfr __no_init volatile unsigned char CMD_REG @ 0x92;
__sfr __no_init volatile unsigned char ADDR_REG_0 @ 0x93;

__sfr __no_init volatile unsigned char ADDR_REG_1 @ 0x94;

__sfr __no_init volatile unsigned char ADDR_REG_2 @ 0x95;

__sfr __no_init volatile unsigned char ADDR_REG_3 @ 0x96;

__sfr __no_init volatile unsigned char INT_STATUS_ARM @ 0xB9; /* intr status reg from arm */

__sfr __no_init volatile unsigned char INT_STATUS_IP0 @ 0xBA; /* intr status reg from arm */

__sfr __no_init volatile unsigned char INT_STATUS_IP1 @ 0xBB; /* intr status reg from arm */

__sfr __no_init volatile unsigned char INT_STATUS_IP2 @ 0xBC; /* intr status reg from arm */

__sfr __no_init volatile unsigned char INT_STATUS_IP3 @ 0xBD; /* intr status reg from arm */

__sfr __no_init volatile unsigned char MCU_TO_ARM_INT @ 0xB1; /* 8051 mcu intr to arm */

__sfr __no_init volatile unsigned char ARM_INTR_MASK @ 0xB2;

__sfr __no_init volatile unsigned char INT_MASK_0 @ 0xB3;

__sfr __no_init volatile unsigned char INT_MASK_1 @ 0xB4;

__sfr __no_init volatile unsigned char INT_MASK_2 @ 0xB5;

__sfr __no_init volatile unsigned char INT_MASK_3 @ 0xB6;

__sfr __no_init volatile unsigned char ARM_INT_CLEAR @ 0xC4;  /* clear intr from arm */

__sfr __no_init volatile unsigned char WR_DATA_REG_0 @ 0x9A;

__sfr __no_init volatile unsigned char WR_DATA_REG_1 @ 0x9B;

__sfr __no_init volatile unsigned char WR_DATA_REG_2 @ 0x9C;

__sfr __no_init volatile unsigned char WR_DATA_REG_3 @ 0x9D;

__sfr __no_init volatile unsigned char RD_DATA_REG_0 @ 0xA1;

__sfr __no_init volatile unsigned char RD_DATA_REG_1 @ 0xA2;

__sfr __no_init volatile unsigned char RD_DATA_REG_2 @ 0xA3;

__sfr __no_init volatile unsigned char RD_DATA_REG_3 @ 0xA4;

#endif
