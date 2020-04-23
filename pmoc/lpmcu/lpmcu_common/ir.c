/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: ir api functions
 */

#include "ir.h"
#include "base.h"
#include "lpmcu_ram_config.h"

#define IR_STD_NUM 4

typedef struct {
    hi_u32 ir_cfg_val;
    hi_u32 ir_leads_val;
    hi_u32 ir_leade_val;
    hi_u32 ir_sleade_val;
    hi_u32 ir_b0_val;
    hi_u32 ir_b1_val;
} ir_reg_setting;

__code const ir_reg_setting g_ir_reg_setting[IR_STD_NUM] = {
    {0x00001f01, 0x033c03cc, 0x019e01e6, 0x00b4010e, 0x00270054, 0x008700cb}, /* nec simple */
    {0x00005f01, 0x019e01e6, 0x019e01e6, 0x00000000, 0x002d0043, 0x008700cb}, /* tc9012 */
    {0x00009f01, 0x012a01b5, 0x009300d8, 0x00000000, 0x0014003c, 0x006400b4}, /* nec full */
    {0x0000cb01, 0x00c00120, 0x00300048, 0x00000000, 0x00300048, 0x00600090}  /* sony 12bit */
};

/* valiable */
hi_u8 g_ir_type;
hi_u8 g_ir_pmocnum;
__idata hi_u32_data g_ir_high_value[IR_PMOC_NUM];
__idata hi_u32_data g_ir_low_value[IR_PMOC_NUM];

#define XMP_NIBBLE_PREFIX 760
hi_u16 g_xmp_code[40] = {0}; /* array length 40 */
hi_u8 g_ir_count = 0;

/* raw module */
typedef struct {
    hi_u16 lead_s_up;
    hi_u16 lead_s_low;
    hi_u16 lead_e_up;
    hi_u16 lead_e_low;
    hi_u16 lead2_s_up;
    hi_u16 lead2_s_low;
    hi_u16 lead2_e_up;
    hi_u16 lead2_e_low;
    hi_u8  bit0_s_up;
    hi_u8  bit0_s_low;
    hi_u8  bit0_e_up;
    hi_u8  bit0_e_low;
    hi_u8  bit1_s_up;
    hi_u8  bit1_s_low;
    hi_u8  bit1_e_up;
    hi_u8  bit1_e_low;
    hi_u8  bits_num;
} ir_nec_fmt;

typedef struct {
    hi_u16 lead_s_up;      /* max mark time of the lead pulse    */
    hi_u16 lead_s_low;     /* min mark time of the lead pulse    */
    hi_u16 lead_e_up;      /* max space time of the lead pulse   */
    hi_u16 lead_e_low;     /* min space time of the lead pulse   */
    hi_u8  bit_up;         /* max mark/space time of normal bit  */
    hi_u8  bit_low;        /* min mark/space time of normal bit  */
    hi_u8  bit_wth;        /* mark/space time of normal bit      */
    hi_u8  tr_up;          /* max mark/space time of trailer bit */
    hi_u8  tr_low;         /* min mark/space time of trailer bit */
    hi_u8  tr_wth;         /* mark/space time of trailer bit     */
    hi_u8  bits_num;       /* need valid bits number             */
    hi_u8  bits_rvs;       /* trailer bit position               */
} ir_rc6_fmt;

typedef struct {
    hi_u8 bit_up;          /* max mark/space time of bit */
    hi_u8 bit_low;         /* min mark/space time of bit */
    hi_u8 bit_wth;         /* mark/space time of bit     */
    hi_u8 bits_num;        /* need valid bits number     */
    hi_u8 bits_rvs;        /* toggle bit                 */
} ir_rc5_fmt;

typedef struct {
    hi_u16 lead_s_up;      /* max mark time of the lead pulse   */
    hi_u16 lead_s_low;     /* min mark time of the lead pulse   */
    hi_u16 lead_e_up;      /* max space time of the lead pulse  */
    hi_u16 lead_e_low;     /* min space time of the lead pulse  */
    hi_u8  bit_s_up;
    hi_u8  bit_s_low;
    hi_u8  bits_num;
}ir_xmp_fmt;

#define IR_TYPE_NEC 0x0
#define IR_TYPE_RC6 0x1
#define IR_TYPE_RC5 0x2
#define IR_TYPE_XMP 0x3
#define PARSE_LAST_SYMBOL 0x3
#define PARSE_CUR_SYMBOL 0x2
#define PARSE_NEXT_SYMBOL 0x1

/* symbol parsed  */
hi_u8 g_ir_rsv_stat = PARSE_CUR_SYMBOL;

/* raw ir protocol type */
hi_u8 g_ir_fmt = IR_TYPE_NEC;

/* sub protocol index */
hi_u8 g_ir_num = 0;

/* received bit number */
hi_u8 g_ir_rcv_num = 0;

/* calculated symbol value */
hi_u8 g_ir_rcx_val0  = 0;
hi_u8 g_ir_rcx_val1  = 0;

/* current bit val: 0 or 1 */
hi_u8 g_ir_bit_val = 0;

/* received symbol value */
hi_u8 g_ir_reg_val_low = 0;
hi_u8 g_ir_reg_val_high = 0;
hi_u16 g_reg_val_low = 0;
hi_u16 g_reg_val_high = 0;
hi_u32_data g_raw_reg_data;

#define IR_NEC_NUM 0x4
#define IR_RC6_NUM 0x1
#define IR_RC5_NUM 0x1
#define IR_XMP_NUM 0x1
#define key_match(max, min, val) (((max) >= (val)) && ((val) >= (min)))

hi_u32_data g_ir_raw_key_low;
hi_u32_data g_ir_raw_key_high;
hi_u32_data g_ir_rsv_key_low;
hi_u32_data g_ir_rsv_key_high;
__code const ir_nec_fmt g_ir_nec_fmt[IR_NEC_NUM] = {
    {650, 350, 650, 350, 0, 0, 0, 0, 33, 17, 67, 35, 65, 35, 132, 70, 12},
    {1080, 720, 540, 360, 0, 0, 0, 0, 84, 39, 84, 39, 84, 39, 220, 118, 32}, /* nec simple */
    /* {437, 298, 216, 147, 437, 298, 216, 147, 73, 39, 73, 39, 73, 39, 146, 78, 40}, nec simple 2header */
    {437, 298, 216, 147, 0, 0, 0, 0, 60, 20, 60, 20, 60, 20, 180, 100, 48}, /* necfull */
    {660, 540, 220, 180, 400, 327, 400, 327, 80, 30, 80, 30, 80, 30, 146, 78, 40}
};

/* default rc6 32bit, can change it to rc6 16 bit: */
__code const ir_rc6_fmt g_ir_rc6_fmt[IR_RC6_NUM] = { { 320, 213, 107, 71, 61, 33, 47, 115, 62, 89, 37, 15} };
__code const ir_rc5_fmt g_ir_rc5_fmt[IR_RC5_NUM] = { { 116, 62, 89, 14, 11}};
__code const ir_xmp_fmt g_ir_xmp_fmt[IR_XMP_NUM] = { {36, 8, 100, 70, 36, 8, 32} };

hi_void ir_disable(hi_void)
{
    hi_u32_data reg_data;

    reg_set(REG_AON_IR_BASE + IR_EN, 0);

    /* Reset IR and close IR clock */
    reg_data.val32 = reg_get(CFG_BASE_ADDR + MCU_SRST_CTRL);
    reg_data.val8[0] |= 0x20;
    reg_data.val8[0] &= 0xef;
    reg_set(CFG_BASE_ADDR + MCU_SRST_CTRL, reg_data.val32);

    return;
}

hi_void ir_start(hi_void)
{
    reg_set(REG_AON_IR_BASE + IR_START, 0x01);

    return;
}

hi_void ir_raw_reset(hi_void)
{
    g_ir_fmt = 0xff;
    g_ir_num = 0xff;
    g_ir_rcv_num = 0;
    g_ir_raw_key_low.val32 = 0;
    g_ir_raw_key_high.val32 = 0;

    g_ir_reg_val_low = 0;
    g_ir_reg_val_high = 0;

    g_ir_rcx_val0 = 0;
    g_ir_rcx_val1 = 0;
    g_ir_bit_val    = 1;
    g_ir_rsv_key_low.val32 = 0xff;
    g_ir_rsv_key_high.val32 = 0xff;

    return;
}

void ir_xmp_reset(void)
{
    hi_u8 i;
    for (i = 0; i < 40; ++i) { /* array length 40 */
        g_xmp_code[i] = 0;
    }
    g_ir_count = 0;
    g_ir_rcv_num = 0;
    g_ir_fmt = 0xff;

    return;
}

hi_void set_data_bit(hi_u8 pos)
{
    /* should use this param, announce it to u32  */
    /* can't directly use digit '1' to left shift  */
    hi_u32 tmp = 1;

    if (pos < 32) { /* 32 bit */
        g_ir_raw_key_low.val32 |= (tmp << pos);
    } else if (pos < 64) { /* 64bit */
        g_ir_raw_key_high.val32 |= (tmp << (pos - 32)); /* 32bit */
    }

    return;
}

#define rec_data_bit(pos, val) do { \
    if (val) { \
        set_data_bit(pos); \
    } \
} while (0)

static hi_void ir_xmp_get_check_value(hi_u8 start, hi_u8 *sum1, hi_u8 *sum2)
{
    *sum1 = (15 + g_xmp_code[start] + g_xmp_code[start + 1] +
            g_xmp_code[start + 2] + g_xmp_code[start + 3] +
            g_xmp_code[start + 4] + g_xmp_code[start + 5] +
            g_xmp_code[start + 6] + g_xmp_code[start + 7]) % 16; /* 16 xmp protocol para */
    *sum2 = (15 + g_xmp_code[start + 8] + g_xmp_code[start + 9] +
            g_xmp_code[start + 10] + g_xmp_code[start + 11] +
            g_xmp_code[start + 12] + g_xmp_code[start + 13] +
            g_xmp_code[start + 14] + g_xmp_code[start + 15]) % 16; /* 16 xmp protocol para */

    return;
}

hi_s8 ir_xmp_analyse(hi_u8 start, hi_u32 divider)
{
    hi_u8 i;
    hi_u8 addr;
    hi_u8 subaddr;
    hi_u8 subaddr2;
    hi_u8 toggle;
    hi_u8 oem;
    hi_u8 obc1;
    hi_u8 obc2;
    hi_u8 sum1;
    hi_u8 sum2;

    if(start > 16 || divider < 50) { /* 16 & 50 xmp protocol para */
        return -1;
    }
    for (i = start; i < start + 16; ++i) { /* 16 xmp protocol para */
        if (g_xmp_code[i] < 890) { /* 890 xmp protocol para */
            g_xmp_code[i] = 0;
            continue;
        }
        if(((g_xmp_code[i] - XMP_NIBBLE_PREFIX) % divider) > (divider * 78 /100)) { /* 78 /100 threshold */
            g_xmp_code[i] = (g_xmp_code[i] - XMP_NIBBLE_PREFIX) / divider + 1;
        } else {
            g_xmp_code[i] = (g_xmp_code[i] - XMP_NIBBLE_PREFIX) / divider;
        }
    }
    ir_xmp_get_check_value(start, &sum1, &sum2);
    if (sum1 != 15 || sum2 != 15) { /* Check value 15 */
        ir_xmp_reset();
        return -1;
    }
    subaddr = (g_xmp_code[start] << 4) | g_xmp_code[start + 2];
    subaddr2 = (g_xmp_code[start + 8] << 4) | g_xmp_code[start + 11];
    oem = (g_xmp_code[start + 4] << 4) | g_xmp_code[start + 5];
    addr = (g_xmp_code[start + 6] << 4) | g_xmp_code[start + 7];
    toggle = g_xmp_code[start + 10];
    obc1 = (g_xmp_code[start + 12] << 4) | g_xmp_code[start + 13];
    obc2 = (g_xmp_code[start + 14] << 4) | g_xmp_code[start + 15];
    if (subaddr != subaddr2) {
        ir_xmp_reset();
        return -1;
    }
    g_ir_raw_key_high.val32 = 0;
    g_ir_raw_key_low.val8[3] = addr;
    g_ir_raw_key_low.val8[2] = subaddr;
    g_ir_raw_key_low.val8[1] = obc1;
    g_ir_raw_key_low.val8[0] = obc2;

    return 0;
}

hi_void  ir_xmp_parsekey(hi_void)
{
    if (g_ir_count < g_ir_xmp_fmt[0].bits_num && g_reg_val_high < 0x464) {
        g_xmp_code[g_ir_count++] = (g_reg_val_low + g_reg_val_high - 21) * 10; /* 21 & 10 calculation factor */
        if (g_ir_count < g_ir_xmp_fmt[0].bits_num) {
        return;
        }
    }

    if (g_ir_count == g_ir_xmp_fmt[0].bits_num) {
        hi_u32 divider;
        hi_s8 ret;

        divider = (g_xmp_code[3] - XMP_NIBBLE_PREFIX) / 15 - 2; /* 15 & 2 protocol para */
        if (divider < 50) { /* divider value 50 */
again:
            divider = (g_xmp_code[19] - XMP_NIBBLE_PREFIX) / 15 - 2; /* 15 & 2 protocol para */
            (void)ir_xmp_analyse(16, divider); /* 16 xmp protocol para */
            return;
        } else {
            ret = ir_xmp_analyse(0, divider);
            if (ret) {
                goto again;
            }
        }
    } else if (g_ir_count > g_ir_xmp_fmt[0].bits_num) {
        ir_xmp_reset();
    }

    return;
}
void ir_rc_parse_symbol(hi_u8 bit_up, hi_u8 bit_low, hi_u8 bit_wth, hi_u8 bits_num)
{
    if (!key_match(bit_up, bit_low, g_ir_rcx_val1)) {
        /* g_ir_rcx_val1 is illegal, reset all parsed parameter */
        /* CNcomment: g_ir_rcx_val1 值非法，所有解析参数复位 */
        if (g_ir_rcx_val1 < bit_up) {
            ir_raw_reset();
            return;
        }

        /* g_ir_rcx_val1 is bigger than width, use the width part and g_ir_rcx_val0 to build one bit */
        /* save the left value to g_ir_rcx_val0, wait for next cycle to handle */
        /* CNcomment: rcxval1值超过 width, 将其中的width和rcxval0组成一个bit，剩下的值放到rcxval0中，等待下一次处理 */
        g_ir_rcx_val0 = g_ir_rcx_val1 - bit_wth;
        g_ir_rcx_val1 = 0;
        g_ir_rsv_stat--;
    } else {
        /* g_ir_rcx_val1 is in the range of width, use it and g_ir_rcx_val0 to build one bit */
        /* CNcomment:  g_ir_rcx_val1 在 width范围内，和rcxval0组成了一个bit */
        g_ir_rsv_stat = g_ir_rsv_stat - 2; /* 2 bit */
        g_ir_rcx_val0 = 0;
        g_ir_rcx_val1 = 0;
    }

    /* put the parsed bit to the parameter 'rawkey', from high to low bit */
    /* CNcomment:  将解析出的bit值放到rawkey中，从高位到低位存放 */
    rec_data_bit(bits_num - g_ir_rcv_num - 1, g_ir_bit_val);

    /* if g_ir_rcx_val0 is not zero, it means that rxcval1 is bigger than width */
    /* so the value of next bit will overturn (from 1 to 0, or from 0 to 1) */
    /* CNcomment:  如果rcxval0不为0，说明rcxval1值超过 width，那么下一个bit值会反转(从1变0或者从0变1) */
    if (g_ir_rcx_val0) {
        g_ir_bit_val = 1 - g_ir_bit_val;
    }

    g_ir_rcv_num++;
}

hi_void  ir_rc_parsekey(hi_void)
{
    hi_u8 ret = 0;
    if (g_ir_rcx_val0) { /* it comes from last symbol, should be one width */
        g_ir_rsv_stat = PARSE_LAST_SYMBOL;
    } else { /* it starts from this symbol   */
        g_ir_rsv_stat = PARSE_CUR_SYMBOL;
    }
    while (g_ir_rsv_stat) {
        if (g_ir_rcx_val0) {
            if (g_ir_rsv_stat == PARSE_LAST_SYMBOL) { /* read the low part of this symbol */
                g_ir_rcx_val1 = g_ir_reg_val_low;
            } else if (g_ir_rsv_stat == PARSE_CUR_SYMBOL) {
                g_ir_rcx_val1 = g_ir_reg_val_high;
            } else if (g_ir_rsv_stat == PARSE_NEXT_SYMBOL) {
                break;
            }
        } else {
            if (g_ir_rsv_stat == PARSE_CUR_SYMBOL) { /* it starts from this symbol   */
                g_ir_rcx_val0 = g_ir_reg_val_low;
                g_ir_rcx_val1 = g_ir_reg_val_high;
            } else if (g_ir_rsv_stat == PARSE_NEXT_SYMBOL) {
                g_ir_rcx_val0 = g_ir_reg_val_high;
                break;
            }
        }
        if (g_ir_fmt == IR_TYPE_RC6) {
            if (key_match(g_ir_rc6_fmt[g_ir_num].bit_up, g_ir_rc6_fmt[g_ir_num].bit_low, g_ir_rcx_val0)) {
                ir_rc_parse_symbol(g_ir_rc6_fmt[g_ir_num].bit_up, g_ir_rc6_fmt[g_ir_num].bit_low,
                                   g_ir_rc6_fmt[g_ir_num].bit_wth, g_ir_rc6_fmt[g_ir_num].bits_num);
            } else if (key_match(g_ir_rc6_fmt[g_ir_num].tr_up, g_ir_rc6_fmt[g_ir_num].tr_low, g_ir_rcx_val0)) {
                ir_rc_parse_symbol(g_ir_rc6_fmt[g_ir_num].tr_up, g_ir_rc6_fmt[g_ir_num].tr_low,
                                   g_ir_rc6_fmt[g_ir_num].tr_wth, g_ir_rc6_fmt[g_ir_num].bits_num);
            } else { /* g_ir_rcx_val0 is illegal, reset all parsed parameter */
                ir_raw_reset();
                return;
            }
        } else if (g_ir_fmt == IR_TYPE_RC5) {
            if (key_match(g_ir_rc5_fmt[g_ir_num].bit_up, g_ir_rc5_fmt[g_ir_num].bit_low, g_ir_rcx_val0)) {
                /* if it is normal bit */
                ir_rc_parse_symbol(g_ir_rc5_fmt[g_ir_num].bit_up, g_ir_rc5_fmt[g_ir_num].bit_low,
                                   g_ir_rc5_fmt[g_ir_num].bit_wth, g_ir_rc5_fmt[g_ir_num].bits_num);
            } else { /* g_ir_rcx_val0 is illegal, reset all parsed parameter */
                ir_raw_reset();
                return;
            }
        } else { /* should not enter here */
            break;
        }
    }
    return;
}

void ir_nec_parsekey(void)
{
    if (key_match(g_ir_nec_fmt[g_ir_num].bit0_s_up, g_ir_nec_fmt[g_ir_num].bit0_s_low, g_raw_reg_data.val8[0]) &&
        key_match(g_ir_nec_fmt[g_ir_num].bit0_e_up, g_ir_nec_fmt[g_ir_num].bit0_e_low, g_raw_reg_data.val8[2])) {
        /* symbol matches to 0 */
        rec_data_bit(g_ir_rcv_num, 0);
        g_ir_rcv_num++;
    } else if (key_match(g_ir_nec_fmt[g_ir_num].bit1_s_up, g_ir_nec_fmt[g_ir_num].bit1_s_low, g_raw_reg_data.val8[0]) &&
               key_match(g_ir_nec_fmt[g_ir_num].bit1_e_up, g_ir_nec_fmt[g_ir_num].bit1_e_low, g_raw_reg_data.val8[2])) {
        /* symbol matches to 1 */
        rec_data_bit(g_ir_rcv_num, 1);
        g_ir_rcv_num++;
    } else if (key_match(g_ir_nec_fmt[g_ir_num].lead2_s_up, g_ir_nec_fmt[g_ir_num].lead2_s_low, g_raw_reg_data.val16[0]) &&
               key_match(g_ir_nec_fmt[g_ir_num].lead2_e_up, g_ir_nec_fmt[g_ir_num].lead2_e_low, g_raw_reg_data.val16[1])) {
        /* symbol matches to second head */
        ;
    } else {
        /* not match, reset all data */
        ir_raw_reset();
        return;
    }

    return;
}

/* check lead code to judge ir protocol type */
void ir_raw_getfmtval(void)
{
    hi_u8 i;
    for (i = 0; i < IR_NEC_NUM; i++) {
        if (key_match(g_ir_nec_fmt[i].lead_s_up, g_ir_nec_fmt[i].lead_s_low, g_raw_reg_data.val16[0]) &&
            key_match(g_ir_nec_fmt[i].lead_e_up, g_ir_nec_fmt[i].lead_e_low, g_raw_reg_data.val16[1])) {
            g_ir_rcv_num = 0;
            g_ir_fmt = IR_TYPE_NEC;
            g_ir_num = i;
            return;
        }
    }

    for (i = 0; i < IR_RC6_NUM; i++) {
        if (key_match(g_ir_rc6_fmt[i].lead_s_up, g_ir_rc6_fmt[i].lead_s_low, g_raw_reg_data.val16[0]) &&
            key_match(g_ir_rc6_fmt[i].lead_e_up, g_ir_rc6_fmt[i].lead_e_low, g_raw_reg_data.val16[1])) {
            g_ir_rcv_num = 0;
            g_ir_fmt = IR_TYPE_RC6;
            g_ir_num = i;
            g_ir_rcx_val0 = 0;
            g_ir_rcx_val1 = 0;
            g_ir_bit_val    = 1;
            return;
        }
    }

    for (i = 0; i < IR_RC5_NUM; i++) {
        if (key_match(g_ir_rc5_fmt[i].bit_up, g_ir_rc5_fmt[i].bit_low, g_raw_reg_data.val8[0]) &&
            key_match(g_ir_rc5_fmt[i].bit_up, g_ir_rc5_fmt[i].bit_low, g_raw_reg_data.val8[2])) {
            g_ir_rcv_num = 0;
            g_ir_fmt = IR_TYPE_RC5;
            g_ir_num = i;

            /* RC5 has no lead code. The first half symbol of rc5 is space. */
            /* And we can't recognize it, so we add it manually. */
            g_ir_rcx_val0 = g_ir_rc5_fmt[i].bit_wth;
            g_ir_rcx_val1 = 0;
            g_ir_bit_val = 1;
            ir_rc_parsekey();
            return;
        }
    }

    for (i = 0; i < IR_XMP_NUM; i++) {
        if (key_match(g_ir_xmp_fmt[i].lead_s_up, g_ir_xmp_fmt[i].lead_s_low, g_reg_val_low) &&
            key_match(g_ir_xmp_fmt[i].lead_e_up, g_ir_xmp_fmt[i].lead_e_low, g_reg_val_high)) {
            g_ir_fmt = IR_TYPE_XMP;
            g_xmp_code[g_ir_count++] = (g_reg_val_low + g_reg_val_high - 21) * 10; /* 21 & 10 calculation factor */
            return;
        }
    }
    return;
}

void ir_rcx_posthandle(hi_u8 pos)
{
    hi_u32 tmp = 1;

    g_ir_rsv_key_low.val32 = g_ir_raw_key_low.val32;
    g_ir_rsv_key_high.val32 = g_ir_raw_key_high.val32;

    if (pos < 32) { /* 32 bit */
        if (g_ir_raw_key_low.val32 & (tmp << pos)) {
            g_ir_rsv_key_low.val32 &= ~(tmp << pos);
        } else {
            g_ir_rsv_key_low.val32 |= (tmp << pos);
        }
    } else if (pos < 64) { /* 64 bit */
        pos = pos - 32; /* 32 bit */
        if (g_ir_raw_key_high.val32 & (tmp << pos)) {
            g_ir_rsv_key_high.val32 &= ~(tmp << pos);
        } else {
            g_ir_rsv_key_high.val32 |= (tmp << pos);
        }
    }

    return;
}

void ir_raw_cmpval(void)
{
    hi_u8 i;
    hi_u32_data reg_data;

    /* add to avoid error when g_ir_fmt is 0xff */
    if (g_ir_rcv_num == 0) {
        return;
    }

    if (g_ir_fmt == IR_TYPE_NEC) {
        if (g_ir_nec_fmt[g_ir_num].bits_num != g_ir_rcv_num) {
            return;
        }
    }

    if (g_ir_fmt == IR_TYPE_RC6) {
        if (g_ir_rc6_fmt[g_ir_num].bits_num != g_ir_rcv_num) {
            return;
        }

        if (g_ir_rc6_fmt[g_ir_num].bits_rvs != 0xff) {
            /* handle the toggle bit */
            /* CNcomment: 处理码值中的翻转bit */
            ir_rcx_posthandle(g_ir_rc6_fmt[g_ir_num].bits_rvs);
        }
    }

    if (g_ir_fmt == IR_TYPE_RC5) {
        if (g_ir_rc5_fmt[g_ir_num].bits_num != g_ir_rcv_num) {
            return;
        }

        if (g_ir_rc5_fmt[g_ir_num].bits_rvs != 0xff) {
            /* mask the high 3bit: 2 start and 1 toggle bit */
            g_ir_raw_key_low.val8[1] &= 0x07;
        }
    }

    for (i = 0; i < g_ir_pmocnum; i++) {
        if (((g_ir_low_value[i].val32 == g_ir_raw_key_low.val32) &&
             (g_ir_high_value[i].val32 == g_ir_raw_key_high.val32)) ||
            ((g_ir_low_value[i].val32 == g_ir_rsv_key_low.val32) &&
             (g_ir_high_value[i].val32 == g_ir_rsv_key_high.val32))) {
            /* save power key to ram */
            reg_data.val32 = g_ir_low_value[i].val32;
            ram_set(LPMCU_WAKEUP_IR_KEYLOW, reg_data.val32);

            reg_data.val32 = g_ir_high_value[i].val32;
            ram_set(LPMCU_WAKEUP_IR_KEYHIGH, reg_data.val32);

            g_wakeup_type = HI_PMOC_WAKEUP_TYPE_IR;
            g_resume_flag = HI_TRUE;
            break;
        }
    }

    if (i >= g_ir_pmocnum) {
        ir_raw_reset();
    }

    return;
}

void ir_raw_isr()
{
    hi_u8 cnt;
    hi_u8 isrflg;

    g_raw_reg_data.val32 = reg_get(REG_AON_IR_BASE + IR_INT_STATUS);
    isrflg = g_raw_reg_data.val8[3];
    if ((isrflg & 0x07) == 0) {
        return;
    }
    g_raw_reg_data.val32 = reg_get(REG_AON_IR_BASE + IR_DATAH); /* get symbol count */
    cnt = g_raw_reg_data.val8[0] & 0x7f;
    /* overflow interrupt */
    if (isrflg & 0x04) {
        /* clear symbol fifo */
        while (cnt > 0) {
            g_raw_reg_data.val32 = reg_get(REG_AON_IR_BASE + IR_DATAL);
            cnt--;
        }
    } else { /* symbol or timeout interrupt */
        /* get symbol from fifo */
        while (cnt-- > 0) {
            g_raw_reg_data.val32 = reg_get(REG_AON_IR_BASE + IR_DATAL);
            g_ir_reg_val_low = g_raw_reg_data.val8[0]; /* save the symbol data to global variables */
            g_ir_reg_val_high = g_raw_reg_data.val8[2];
            g_reg_val_low = g_raw_reg_data.val16[0];
            g_reg_val_high = g_raw_reg_data.val16[1];
            if (g_ir_fmt == 0xff) {
                /* get the type of ir protocol */
                ir_raw_getfmtval();
                continue;
            }
            if (g_ir_fmt == IR_TYPE_NEC) {
                ir_nec_parsekey();
            }
            if ((g_ir_fmt == IR_TYPE_RC5) || (g_ir_fmt == IR_TYPE_RC6)) {
                ir_rc_parsekey();
                }
            if (g_ir_fmt == IR_TYPE_XMP) {
                    g_ir_rcv_num++;
                    ir_xmp_parsekey();
                    if (g_ir_count < g_ir_xmp_fmt[0].bits_num) {
                        continue;
                    }
            }
            /* compare received key to wake up key */
            ir_raw_cmpval();
            if (g_resume_flag) {
                break;
            }
        }
        /* overtime int, the last symbol. After handle it, reset all parameter */
        if (isrflg & 0x2) {
            ir_raw_reset();
        }
    }
    /* clear interrupt */
    reg_set(REG_AON_IR_BASE + IR_INT_CLR, 0x00070000);

    return;
}

void ir_std_isr()
{
    hi_u32_data reg_data;

    reg_data.val32 = reg_get(REG_AON_IR_BASE + IR_INT_STATUS);

    if ((reg_data.val8[2] & 0x0f) == 0) {
        return;
    }

    /* if receive data */
    if (reg_data.val8[2] & 0x01) {
        /* read low 32 bit of ir key */
        reg_data.val32 = reg_get(REG_AON_IR_BASE + IR_DATAL);
        if (reg_data.val32 == g_ir_low_value[0].val32) {
            /* save power key to ram */
            reg_data.val32 = g_ir_low_value[0].val32;
            ram_set(LPMCU_WAKEUP_IR_KEYLOW, reg_data.val32);

            reg_data.val32 = g_ir_high_value[0].val32;
            ram_set(LPMCU_WAKEUP_IR_KEYHIGH, reg_data.val32);

            g_wakeup_type = HI_PMOC_WAKEUP_TYPE_IR;
            g_resume_flag = HI_TRUE;
        }
    }

    reg_set(REG_AON_IR_BASE + IR_INT_CLR, 0x0f);

    return;
}

hi_void ir_init(hi_void)
{
    hi_u32_data reg_data = {0};
    /* open IR clock */
    reg_data.val32 = reg_get(CFG_BASE_ADDR + MCU_SRST_CTRL);
    reg_data.val8[0] |= 0x10; /* bit 4 to 1 */
    reg_data.val8[0] &= 0xdf; /* bit 5 to 0 */
    reg_set(CFG_BASE_ADDR + MCU_SRST_CTRL, reg_data.val32);

    /* enable IR */
    reg_set(REG_AON_IR_BASE + IR_EN, 0x101);

    /* 0  busy */
    do {
        wait_minute_1(10); /* 10 minutes */
        reg_data.val32 = reg_get(REG_AON_IR_BASE + IR_BUSY);
    } while (reg_data.val8[0]);

    /* 1  cfg */
    if (g_ir_type < IR_STD_NUM) {
        reg_data.val32 = g_ir_reg_setting[g_ir_type].ir_cfg_val;
    } else {
        reg_data.val32 = 0x3e800181;
    }
    reg_set(REG_AON_IR_BASE + IR_CFG, reg_data.val32);

    /* raw type do not need set leads, leade and so on */
    if (g_ir_type >= IR_STD_NUM) {
        goto over;
    }

    /* 2 leads */
    reg_data.val32 = g_ir_reg_setting[g_ir_type].ir_leads_val;
    reg_set(REG_AON_IR_BASE + IR_LEADS, reg_data.val32);

    /* 3 leade */
    reg_data.val32 = g_ir_reg_setting[g_ir_type].ir_leade_val;
    reg_set(REG_AON_IR_BASE + IR_LEADE, reg_data.val32);

    /* 4 sleade */
    reg_data.val32 = g_ir_reg_setting[g_ir_type].ir_sleade_val;
    reg_set(REG_AON_IR_BASE + IR_SLEADE, reg_data.val32);

    /* 5 b0 */
    reg_data.val32 = g_ir_reg_setting[g_ir_type].ir_b0_val;
    reg_set(REG_AON_IR_BASE + IR_B0, reg_data.val32);

    /* 6 b1 */
    reg_data.val32 = g_ir_reg_setting[g_ir_type].ir_b1_val;
    reg_set(REG_AON_IR_BASE + IR_B1, reg_data.val32);

over:
    /* 7 int mask */
    if (g_ir_type < IR_STD_NUM) {
        reg_data.val32 = 0x0007000e;
    } else {
        reg_data.val32 = 0x1f;
    }
    reg_set(REG_AON_IR_BASE + IR_INT_MASK, reg_data.val32);

    if (g_ir_type >= IR_STD_NUM) {
        ir_raw_reset();
    }

    return;
}

hi_void ir_isr(hi_void)
{
    if (g_ir_type < IR_STD_NUM) {
        ir_std_isr();
    } else {
        ir_raw_isr();
    }

    return;
}


