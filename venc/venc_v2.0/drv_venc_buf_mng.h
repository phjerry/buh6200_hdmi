/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */

#ifndef __DRV_VENC_BUF_MNG_H__
#define __DRV_VENC_BUF_MNG_H__

#include "hi_type.h"
#include "hi_venc_type.h"

/************************cabac/cavlc coding************************/
static const hi_u8 g_ms_zeros_table[] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define MS_ZEROS(zeros, c) do { \
        hi_u32 __code__ = (c); \
        (zeros) = g_ms_zeros_table[__code__ >> 24]; \
        if (8 == (zeros)) \
        { (zeros) += g_ms_zeros_table[0xFF & (__code__ >> 16)]; \
            if (16 == (zeros)) \
            { (zeros) += g_ms_zeros_table[0xFF & (__code__ >> 8)]; \
                if (24 == (zeros)) \
                { (zeros) += g_ms_zeros_table[0xFF & __code__ ]; } } } \
    } while (0)

#define UE_VLC(bits, code, val) do { \
        hi_u32 zeros;             \
        code = (val) + 1;          \
        MS_ZEROS(zeros, code);       \
        bits = 63 - (zeros << 1);    \
    } while (0)

#define SE_VLC(bits, code, val) do { \
        hi_u32 zeros;             \
        (code) = ((hi_u32)((val) < 0 ? - (val) : (val)) << 1) | (hi_u32)((val) <= 0); \
        MS_ZEROS(zeros, (code));       \
        (bits) = 63 - (zeros << 1);    \
    } while (0)

#define REV32(x) ((((hi_u32)(x)) >> 24) + ((((hi_u32)(x)) >> 8) & (hi_u32)0xFF00) + \
                  (((hi_u32)(x)) << 24) + ((((hi_u32)(x)) << 8) & (hi_u32)0xFF0000))

/************************************************
Processor-related definitions

 Notes:
 1)All length units are addressing step of processor, and ARM is the "bytes";
 2)All W/R operations carried out in accordance with word, so need:
   buffer length of the packet should be in word-alignment;
   elements of the queue should be in word-alignment;
 *************************************************/
#define WORD_ALIGN 0x04       /* Bytes included in one word */

typedef struct {
    hi_u32  tu32_a; /* only used in ShowBits32 and FlushBits31/32 */
    hi_u32  tu32_b;
    hi_u32* buff;
    hi_u32  bits;
    hi_u32  total_bits;
    hi_u32  big_endian;
} tbit_stream;

typedef struct {
    hi_u32* base;  /* base addr */
    hi_u32  len;    /* max length */
    hi_u32  head;   /* head pointer */
    hi_u32  sum;
    hi_u32  mean;
} valg_fifo;

/******************************************************************************
Function   :
Description:
Calls      :
Input      :
Output     :
Return     :
Others     :
******************************************************************************/
#define FRM_RATE_CNT_NUM 65

__inline static hi_void venc_drv_fifo_info(valg_fifo* fifo, hi_void* base, hi_u32 buf_num, hi_u32 len, hi_u32 mean)
{
    int i;

    if (fifo == NULL || base == NULL || buf_num > FRM_RATE_CNT_NUM) {
        HI_ERR_VENC("Fifo Init failed,wrong para:fifo: %p, base: %p, buf_num: %u\n", fifo, base, buf_num);
        return;
    }

    fifo->base = (hi_u32*)base;
    if (len >= FRM_RATE_CNT_NUM) {
        len = FRM_RATE_CNT_NUM;
    }

    fifo->len  = len;
    fifo->head = 0;
    fifo->sum  = 0;
    fifo->mean = mean;

    for (i = 0; i < len && i < buf_num; i++) {
        fifo->base[i] = 0;
    }
}

__inline static hi_void venc_drv_fifo_write_init(valg_fifo* fifo, hi_u32 val)
{
    if (fifo == NULL || fifo->base == NULL) {
        return;
    }

    fifo->sum -= fifo->base[fifo->head];
    fifo->sum += val;

    fifo->base[fifo->head] = val;

    fifo->head++;

    if (fifo->head >= fifo->len) {
        fifo->head -= fifo->len;
    }
}

__inline static hi_void venc_drv_fifo_write(valg_fifo* fifo, hi_u32 val)
{
    if (fifo == NULL || fifo->base == NULL) {
        return;
    }

    if (fifo->sum > fifo->mean) {
        fifo->sum -= fifo->base[fifo->head];
    } else {
        fifo->sum = 0;
    }

    fifo->sum += val;

    fifo->base[fifo->head] = val;

    fifo->head++;

    if (fifo->head >= fifo->len) {
        fifo->head -= fifo->len;
    }
}

hi_void venc_drv_bs_open_bit_stream(tbit_stream* bs, hi_u32* buffer, hi_u32 len);
hi_void venc_drv_bs_put_bits31(tbit_stream* bs, hi_u32 code, hi_u32 bits);
hi_void venc_drv_bs_put_bits32(tbit_stream* bs, hi_u32 code, hi_u32 bits);

#endif
