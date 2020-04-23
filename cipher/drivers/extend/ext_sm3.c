/*****************************************************************************

    Copyright (C), 2017, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name     : ext_sm3.c
  Version       : Initial Draft
  Created       : 2017
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/
#include "drv_osal_lib.h"
#include "cryp_hash.h"

#ifdef SOFT_SM3_SUPPORT

/*************************** Internal Structure Definition ****************************/
/* \addtogroup      sm3 */
/* @{ */  /* <!-- [sm3] */

#define ROTATE_DIVIDER 32

hi_u32 ROTATELEFT(hi_u32 x, hi_u32 n)
{
    n %= ROTATE_DIVIDER;

    if (n == 0) {
        return x;
    }

    return (x << n) | (x >> (ROTATE_DIVIDER - n));
}

#define P0(x) ((x) ^  ROTATELEFT((x),9)  ^ ROTATELEFT((x),17))
#define P1(x) ((x) ^  ROTATELEFT((x),15) ^ ROTATELEFT((x),23))

#define FF0(x,y,z) ((x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))

#define GG0(x,y,z) ((x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ((~(x)) & (z)))

#define SM3_BLOCK_SIZE            64
#define SM3_W_SIZE                (SM3_BLOCK_SIZE + WORD_WIDTH)
#define SM3_RESULT_SIZE_IN_WORD   (SM3_RESULT_SIZE / WORD_WIDTH)
#define SM3_PAD_MIN_SIZE          9
#define SM3_PAD_LEN_SIZE          8
#define SM3_BYTE_MSB              0x80

/* SM3, the initial hash value, H(0) */
#define SM3_H0    0x7380166F
#define SM3_H1    0x4914B2B9
#define SM3_H2    0x172442D7
#define SM3_H3    0xDA8A0600
#define SM3_H4    0xA96F30BC
#define SM3_H5    0x163138AA
#define SM3_H6    0xE38DEE4D
#define SM3_H7    0xB0FB0E4E

#define HASH_MAX_BUFFER_SIZE    0x10000 /* 64K */

/**
 * \brief          aes ccm context structure
 */
typedef struct {
    hi_u32 state[SM3_RESULT_SIZE_IN_WORD];
    hi_u8  tail[SM3_BLOCK_SIZE];
    hi_u32 tail_len;
    hi_u32 total;
}
ext_sm3_context;

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      sm3 */
/** @{ */  /** <!-- [sm3] */

hi_s32 sm3_compress(hi_u32 digest[SM3_RESULT_SIZE_IN_WORD], const hi_u8 block[SM3_BLOCK_SIZE])
{
    hi_s32 j;
    hi_s32 k = 0;
    hi_u32 *W = HI_NULL;
    hi_u32 *W1 = HI_NULL;
    hi_u32 *T = HI_NULL;
    const hi_u32 *pblock = (const hi_u32 *)block;
    hi_u32 A = digest[k++];
    hi_u32 B = digest[k++];
    hi_u32 C = digest[k++];
    hi_u32 D = digest[k++];
    hi_u32 E = digest[k++];
    hi_u32 F = digest[k++];
    hi_u32 G = digest[k++];
    hi_u32 H = digest[k++];
    hi_u32 SS1, SS2, TT1, TT2;
    hi_u32 *buffer = HI_NULL;
    hi_u32 buffer_size;
    hi_s32 ret;

    buffer_size = sizeof(hi_u32) * (SM3_W_SIZE + SM3_BLOCK_SIZE + SM3_BLOCK_SIZE);
    buffer = (hi_u32 *)crypto_malloc(buffer_size);
    if (buffer == HI_NULL) {
        hi_log_error("sm3 compress crypto malloc buff failed!\n");
        return HI_ERR_CIPHER_FAILED_MEM;
    }
    ret = memset_s(buffer, buffer_size, 0, buffer_size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        crypto_free(buffer);
        buffer = HI_NULL;
        return ret;
    }
    W = buffer;
    W1 = buffer + SM3_W_SIZE;
    T = buffer + SM3_W_SIZE + SM3_BLOCK_SIZE;

    for (j = 0; j < 16; j++) { /* 16 */
        W[j] = cipher_cpu_to_be32(pblock[j]);
    }
    for (j = 16; j < SM3_W_SIZE; j++) { /* 16 */
        W[j] = P1(W[j - 16] ^ W[j - 9] ^ ROTATELEFT(W[j - 3], 15)) /* 16 9 3 15 */
            ^ ROTATELEFT(W[j - 13], 7) ^ W[j - 6]; /* 13 7 6 */
    }
    for (j = 0; j < SM3_BLOCK_SIZE; j++) {
        W1[j] = W[j] ^ W[j + 4]; /* 4 */
    }

    for (j = 0; j < 16; j++) { /* 16 */

        T[j] = 0x79CC4519;
        SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(T[j], j)), 7); /* 12 7 */
        SS2 = SS1 ^ ROTATELEFT(A, 12); /* 12 */
        TT1 = FF0(A, B, C) + D + SS2 + W1[j];
        TT2 = GG0(E, F, G) + H + SS1 + W[j];
        D = C;
        C = ROTATELEFT(B, 9); /* 9 */
        B = A;
        A = TT1;
        H = G;
        G = ROTATELEFT(F, 19); /* 19 */
        F = E;
        E = P0(TT2);
    }

    for (j = 16; j < SM3_BLOCK_SIZE; j++) { /* 16 */

        T[j] = 0x7A879D8A;
        SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(T[j], j)), 7); /* 12 7 */
        SS2 = SS1 ^ ROTATELEFT(A, 12); /* 12 */
        TT1 = FF1(A, B, C) + D + SS2 + W1[j];
        TT2 = GG1(E, F, G) + H + SS1 + W[j];
        D = C;
        C = ROTATELEFT(B, 9); /* 9 */
        B = A;
        A = TT1;
        H = G;
        G = ROTATELEFT(F, 19); /* 19 */
        F = E;
        E = P0(TT2);
    }

    k = 0;
    digest[k++] ^= A;
    digest[k++] ^= B;
    digest[k++] ^= C;
    digest[k++] ^= D;
    digest[k++] ^= E;
    digest[k++] ^= F;
    digest[k++] ^= G;
    digest[k++] ^= H;

    if (buffer != HI_NULL) {
        crypto_free(buffer);
        buffer = HI_NULL;
    }

    return HI_SUCCESS;
}

static void sm3_init(ext_sm3_context *ctx)
{
    int i = 0;
    hi_log_func_enter();

    ctx->state[i++] = SM3_H0;
    ctx->state[i++] = SM3_H1;
    ctx->state[i++] = SM3_H2;
    ctx->state[i++] = SM3_H3;
    ctx->state[i++] = SM3_H4;
    ctx->state[i++] = SM3_H5;
    ctx->state[i++] = SM3_H6;
    ctx->state[i++] = SM3_H7;

    hi_log_func_exit();

    return;
}

static hi_s32 sm3_update(ext_sm3_context *ctx, const hi_u8 *data, hi_u32 data_len)
{
    hi_u32 left = 0;
    hi_s32 ret = HI_FAILURE;

    hi_log_func_enter();

    ctx->total += data_len;

    if (ctx->tail_len) {
        left = SM3_BLOCK_SIZE - ctx->tail_len;
        if (data_len < left) {
            ret = memcpy_s(ctx->tail + ctx->tail_len, left, data, data_len);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(memcpy_s, ret);
                return ret;
            }
            ctx->tail_len += data_len;
            return HI_SUCCESS;
        } else {
            ret = memcpy_s(ctx->tail + ctx->tail_len, left, data, left);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(memcpy_s, ret);
                return ret;
            }
            ret = sm3_compress(ctx->state, ctx->tail);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(sm3_compress, ret);
                return ret;
            }

            data += left;
            data_len -= left;
        }
    }

    while (data_len >= SM3_BLOCK_SIZE) {
        ret = sm3_compress(ctx->state, data);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(sm3_compress, ret);
            return ret;
        }

        data += SM3_BLOCK_SIZE;
        data_len -= SM3_BLOCK_SIZE;
    }

    ctx->tail_len = data_len;
    if (data_len) {
        ret = memcpy_s(ctx->tail, SM3_BLOCK_SIZE, data, data_len);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
    }

    hi_log_func_exit();

    return HI_SUCCESS;
}

static hi_s32 sm3_final(ext_sm3_context *ctx, hi_u8 *digest, hi_u32 digest_buf_len)
{
    hi_s32 i = 0;
    hi_s32 ret = HI_FAILURE;
    hi_u32 hash[SM3_RESULT_SIZE_IN_WORD] = {0};

    hi_log_func_enter();

    ctx->tail[ctx->tail_len] = SM3_BYTE_MSB;

    /* a block is enough */
    if (ctx->tail_len + SM3_PAD_MIN_SIZE <= SM3_BLOCK_SIZE) {
        ret = memset_s(ctx->tail + ctx->tail_len + 1, SM3_BLOCK_SIZE - ctx->tail_len - 1, 0,
            SM3_BLOCK_SIZE - ctx->tail_len - 1);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memset_s, ret);
            return ret;
        }
    } else {
        /* 2 block is request */
        ret = memset_s(ctx->tail + ctx->tail_len + 1, SM3_BLOCK_SIZE - ctx->tail_len - 1,
            0, SM3_BLOCK_SIZE - ctx->tail_len - 1);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memset_s, ret);
            return ret;
        }

        ret = sm3_compress(ctx->state, ctx->tail);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(sm3_compress, ret);
            return ret;
        }

        ret = memset_s(ctx->tail, SM3_BLOCK_SIZE, 0, SM3_BLOCK_SIZE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memset_s, ret);
            return ret;
        }
    }

    /* write 8 bytes fix data length * 8 */
    ctx->tail[SM3_BLOCK_SIZE - 5] = (hi_u8)((ctx->total >> 29) & 0x07); /* 5 29 0x07 */
    ctx->tail[SM3_BLOCK_SIZE - 4] = (hi_u8)((ctx->total >> 21) & 0xff); /* 4 21 0xff */
    ctx->tail[SM3_BLOCK_SIZE - 3] = (hi_u8)((ctx->total >> 13) & 0xff); /* 3 13 0xff */
    ctx->tail[SM3_BLOCK_SIZE - 2] = (hi_u8)((ctx->total >> 5)  & 0xff); /* 2  5 0xff */
    ctx->tail[SM3_BLOCK_SIZE - 1] = (hi_u8)((ctx->total << 3)  & 0xff); /* 1  3 0xff */

    ret = sm3_compress(ctx->state, ctx->tail);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(sm3_compress, ret);
        return ret;
    }

    for (i = 0; i < SM3_RESULT_SIZE_IN_WORD; i++) {
        hash[i] = cipher_cpu_to_be32(ctx->state[i]);
    }

    ret = memcpy_s(digest, digest_buf_len, hash, SM3_RESULT_SIZE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    hi_log_func_exit();

    return HI_SUCCESS;
}

void *ext_sm3_create(hash_mode mode)
{
    ext_sm3_context *ctx = HI_NULL;
    hi_s32 ret;

    hi_log_func_enter();

    ctx = crypto_malloc(sizeof(ext_sm3_context));
    if (ctx == HI_NULL) {
        hi_log_error("malloc hash context buffer failed!");
        return HI_NULL;
    }

    ret = memset_s(ctx, sizeof(ext_sm3_context), 0, sizeof(ext_sm3_context));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        crypto_free(ctx);
        ctx = HI_NULL;
        return HI_NULL;
    }

    sm3_init(ctx);

    hi_log_func_exit();

    return ctx;
}

hi_s32 ext_sm3_update(void *ctx, hi_u8 *chunk, hi_u32 chunkLen, hash_chunk_src src)
{
    hi_u8 *ptr = HI_NULL;
    hi_s32 ret = HI_FAILURE;
    hi_u32 offset = 0;
    hi_u32 length = 0;

    hi_log_func_enter();

    hi_log_check_param(ctx == HI_NULL);

    if (chunkLen == 0x00) {
        return HI_SUCCESS;
    }

    if (src == HASH_CHUNCK_SRC_USER) {
        ptr = crypto_malloc(HASH_MAX_BUFFER_SIZE);
        if (ptr == HI_NULL) {
            hi_log_error("malloc hash chunk buffer failed, chunkLen 0x%x\n!", chunkLen);
            hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
            return HI_ERR_CIPHER_FAILED_MEM;
        }

        while (offset < chunkLen) {
            length = chunkLen - offset;
            if (length > HASH_MAX_BUFFER_SIZE) {
                length = HASH_MAX_BUFFER_SIZE;
            }
            ret = crypto_copy_from_user(ptr, HASH_MAX_BUFFER_SIZE, chunk + offset, length);
            if (ret != HI_SUCCESS) {
                hi_log_error("copy hash chunk from user failed!");
                hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
                goto exit;
            }
            ret = sm3_update(ctx, ptr, length);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(sm3_update, ret);
                goto exit;
            }

            crypto_msleep(1);
            offset   += length;
        }
    } else {
        if (chunk == HI_NULL) {
            hi_log_error("Invalid point,chunk is null!");
            ret = HI_ERR_CIPHER_INVALID_POINT;
            goto exit;
        }
        ret = sm3_update(ctx, chunk, chunkLen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(sm3_update, ret);
            goto exit;
        }

        ret = HI_SUCCESS;
    }

exit:
    if (ptr != HI_NULL) {
        crypto_free(ptr);
        ptr = HI_NULL;
    }

    hi_log_func_exit();

    return ret;
}

hi_s32 ext_sm3_finish(void *ctx, void *hash, hi_u32 hash_buf_len, hi_u32 *hashlen)
{
    hi_s32 ret;
    hi_log_func_enter();

    hi_log_check_param(ctx == HI_NULL);

    ret = sm3_final(ctx, hash, hash_buf_len);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(sm3_final, ret);
        return ret;
    }
    *hashlen = SM3_RESULT_SIZE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 ext_sm3_destory(const void *ctx)
{
    hi_log_func_enter();

    hi_log_check_param(ctx == HI_NULL);

    crypto_free(ctx);
    ctx  = HI_NULL;

    hi_log_func_exit();
    return HI_SUCCESS;
}

#endif
/** @} */  /** <!-- ==== API Code end ==== */
