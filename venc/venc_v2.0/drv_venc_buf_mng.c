/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:  encoder
 * Author: sdk
 * Create: 2019-07-18
 */
#include "drv_venc_buf_mng.h"
#include <linux/fs.h>
#include "linux/hisilicon/securec.h"
#include "drv_venc_osal_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void venc_drv_bs_open_bit_stream(tbit_stream *bs, hi_u32 *buffer, hi_u32 len)
{
    hi_u32 test = 1;

    if (bs == NULL || buffer == NULL) {
        return;
    }

    VENC_CHECK_NEQ_VOID(memset_s(bs, sizeof(tbit_stream), 0, sizeof(tbit_stream)), HI_SUCCESS);
    bs->buff = buffer;

    bs->tu32_a = 0;
    bs->tu32_b = 0;
    bs->bits = 0;

    bs->total_bits  = 0;
    bs->big_endian = !*(hi_u8 *)&test;
}

hi_void venc_drv_bs_put_bits31(tbit_stream *bs, hi_u32 code, hi_u32 bits)
{
    if (bs == NULL) {
        return;
    }

    bs->total_bits += bits;

    if (bs->buff != NULL) {
        code <<= (32 - bits);   /* 32 is 4 bytes length */
        bs->tu32_b |= (code >> bs->bits);
        bs->bits += bits;

        if (bs->bits >= 32) {   /* 32 is 4 bytes length */
            bs->bits -= 32;     /* 32 is 4 bytes length */
            bits -= bs->bits;
            *bs->buff++ = (bs->big_endian ? bs->tu32_b : REV32(bs->tu32_b));
            bs->tu32_b = (code << bits);
        }
    }
}

hi_void venc_drv_bs_put_bits32(tbit_stream *bs, hi_u32 code, hi_u32 bits)
{
    if (bs == NULL) {
        return;
    }

    bs->total_bits += bits;

    if (bs->buff != NULL) {
        code <<= (32 - bits);   /* 32 is 4 bytes length */
        bs->tu32_b |= (code >> bs->bits);
        bs->bits += bits;

        if (bs->bits >= 32) {   /* 32 is 4 bytes length */
            bs->bits -= 32;     /* 32 is 4 bytes length */
            bits -= bs->bits;
            *bs->buff++ = (bs->big_endian ? bs->tu32_b : REV32(bs->tu32_b));
            bs->tu32_b = ((code << (bits - 1)) << 1);
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

