/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of ext_cmac
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_symc.h"

#ifdef HI_PRODUCT_CBC_MAC_SUPPORT

/* the max extend data length for encryption / decryption is  8k one time */
#define MAX_EXT_DATA_LEN (0x100000)

#define BYTE_MSB 0x80

/* For CMAC Calculation */

static hi_u8 g_const_Rb[AES_BLOCK_SIZE_IN_BYTE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
};

static hi_s32 ext_aes_cmac_chunk_copy(void *dma, hi_u32 max_len, const void *chunk, hi_u32 len, hi_u32 from_user)
{
    hi_s32 ret;

    hi_log_func_enter();

    if (from_user == HI_TRUE) {
        ret = crypto_copy_from_user(dma, max_len, chunk, len);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(crypto_copy_from_user, ret);
            return ret;
        }
    } else {
        ret = memcpy_s(dma, max_len, chunk, len);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* only output last block */
static hi_s32 ext_aes_cmac_block(hi_u32 id, hi_u8 *input, hi_u32 inlen, hi_u32 from_user, hi_u8 *output)
{
    hi_s32 ret;
    hi_u32 enc_len = 0;
    hi_u32 left_len;
    hi_u32 i = 0;
    hi_u32 blocknum;
    crypto_mem mem;
    compat_addr phy;
    hi_u8 *dma_virt = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(input == HI_NULL);
    hi_log_check_param(output == HI_NULL);
    hi_log_check_param((inlen % AES_BLOCK_SIZE_IN_BYTE != 0x00));

    ret = memset_s(&mem, sizeof(mem), 0, sizeof(mem));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }
    ret = memset_s(&phy, sizeof(phy), 0, sizeof(phy));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    /* malloc dma buffer */
#ifdef HI_SMMU_SUPPORT
    ret = crypto_mem_try_create_max("CIPHER_AES_EXT", MEM_TYPE_SMMU, cipher_min(MAX_EXT_DATA_LEN, inlen), &mem);
#else
    ret = crypto_mem_try_create_max("CIPHER_AES_EXT", MEM_TYPE_MMZ, cipher_min(MAX_EXT_DATA_LEN, inlen), &mem);
#endif
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_mem_try_create_max, ret);
        return HI_FAILURE;
    }
    hi_log_debug("cmac mmz size 0x%x, inlen 0x%x\n", mem.dma_size, inlen);

    crypto_mem_phys(&mem, &phy);
    dma_virt = crypto_mem_virt(&mem);

    /* split data to decryption or enctryption */
    left_len = inlen;
    blocknum = (inlen + mem.dma_size - 1) / mem.dma_size;
    for (i = 0; i < blocknum; i++) {
        /* new data length */
        enc_len = left_len >= mem.dma_size ? mem.dma_size : left_len;
        left_len -= enc_len;

        /* copy data to dma buffer */
        ret = ext_aes_cmac_chunk_copy(dma_virt, mem.dma_size,
            (const void *)(input + i * mem.dma_size), enc_len, from_user);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(ext_aes_cmac_chunk_copy, ret);
            goto exit__;
        }

        check_exit(kapi_symc_crypto(id, phy, phy, enc_len, HI_FALSE, HI_FALSE, HI_FALSE));

        /* only output last block */
        ret = memcpy_s(output, AES_BLOCK_SIZE_IN_BYTE,
                       dma_virt + enc_len - AES_BLOCK_SIZE_IN_BYTE, AES_BLOCK_SIZE_IN_BYTE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(crypto_copy_from_user, ret);
            goto exit__;
        }
    }

exit__:
    crypto_mem_destory(&mem);
    hi_log_func_exit();

    return ret;
}

/* Basic Functions */
static hi_s32 xor_128(hi_u8 *a, hi_u8 *b, hi_u8 *out, hi_u32 length)
{
    hi_u32 i;

    hi_log_check_param(a == HI_NULL);
    hi_log_check_param(b == HI_NULL);
    hi_log_check_param(out == HI_NULL);
    hi_log_check_param(length != AES_BLOCK_SIZE_IN_BYTE);

    for (i = 0; i < AES_BLOCK_SIZE_IN_BYTE; i++) {
        out[i] = a[i] ^ b[i];
    }

    return HI_SUCCESS;
}

/* AES-CMAC Generation Function */
static hi_s32 leftshift_onebit(hi_u8 *input, hi_u8 *output, hi_u32 length)
{
    hi_s32 i;
    hi_u8 overflow = 0;

    if (input == NULL || output == NULL) {
        hi_log_error("Invalid parameter!\n");
        return HI_ERR_CIPHER_INVALID_POINT;
    }

    if (length != AES_BLOCK_SIZE_IN_BYTE) {
        hi_log_error("Invalid length!\n");
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    for (i = length - 1; i >= 0; i--) {
        output[i] = input[i] << 1;
        output[i] |= overflow;
        overflow = (input[i] & BYTE_MSB) ? 1 : 0;
    }

    return HI_SUCCESS;
}

static hi_s32 generate_subkey(hi_u32 id, hi_u8 *K1, hi_u8 *K2, hi_u32 length)
{
    hi_u8 L[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_u8 Z[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_u8 tmp[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_s32 ret;

    if (length != AES_BLOCK_SIZE_IN_BYTE) {
        hi_log_error("Invalid length.\n");
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    ret = ext_aes_cmac_block(id, Z, length, HI_FALSE, L);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ext_aes_cmac_block, ret);
        return ret;
    }

    if ((L[0] & BYTE_MSB) == 0) { /* If MSB(L) = 0, then K1 = L << 1 */
        ret = leftshift_onebit(L, K1, length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(leftshift_onebit, ret);
            return ret;
        }
    } else { /* Else K1 = (L << 1) (+) Rb */
        ret = leftshift_onebit(L, tmp, length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(leftshift_onebit, ret);
            return ret;
        }

        ret = xor_128(tmp, g_const_Rb, K1, length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(xor_128, ret);
            return ret;
        }
    }

    if ((K1[0] & BYTE_MSB) == 0) {
        ret = leftshift_onebit(K1, K2, length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(leftshift_onebit, ret);
            return ret;
        }
    } else {
        ret = leftshift_onebit(K1, tmp, length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(leftshift_onebit, ret);
            return ret;
        }

        ret = xor_128(tmp, g_const_Rb, K2, length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(xor_128, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

static void padding(hi_u8 *lastb, hi_u8 *pad, hi_u32 length)
{
    hi_u32 j;

    /* original last block */
    for (j = 0; j < AES_BLOCK_SIZE_IN_BYTE; j++) {
        if (j < length) {
            pad[j] = lastb[j];
        } else if (j == length) {
            pad[j] = BYTE_MSB;
        } else {
            pad[j] = 0x00;
        }
    }
}

/**********************************************************
*                        AES_CBCMAC                       *

http:// nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38b.pdf

Subkey Generation:
Steps:
1. Let L = CIPHK(0b).
2. If MSB1(L) = 0, then K1 = L << 1;
   Else K1 = (L << 1)  Rb; see Sec. 5.3 for the definition of Rb.
3. If MSB1(K1) = 0, then K2 = K1 << 1; Else K2 = (K1 << 1)  Rb.
4. Return K1, K2.

MAC Generation:
Steps:
1. Apply the subkey generation process in Sec. 6.1 to K to produce K1 and K2.
2. If Mlen = 0, let n = 1; else, let n = [Mlen/b]
3. Let M1, M2, ... , Mn-1, Mn* denote the unique sequence of bit strings such that
   M = M1 || M2 || ... || Mn-1 || Mn*, where M1, M2,..., Mn-1 are complete blocks.2
4. If Mn* is a complete block, let Mn = K1 ^ Mn*;
   else, let Mn = K2 ^ (Mn*||10j), where j = nb-Mlen-1.
5. Let C0 = 0b .
6. For i = 1 to n, let Ci = CIPHK(Ci-1 . Mi).
7. Let T = MSBTlen(Cn).
8. Return T.
***********************************************************/
hi_s32 ext_aes_cmac_update(hi_u32 id, hi_u8 *input, hi_u32 inlen, hi_u32 from_user)
{
    hi_s32 ret;
    hi_u8 iv[AES_BLOCK_SIZE_IN_BYTE] = { 0 };

    hi_log_func_enter();

    hi_log_debug("id %d, inlen %d\n", id, inlen);

    hi_log_check_param(input == HI_NULL);
    hi_log_check_param(inlen == 0);
    hi_log_check_param((inlen % AES_BLOCK_SIZE_IN_BYTE != 0));

    /* X := AES-128(KEY, Y); */
    ret = ext_aes_cmac_block(id, input, inlen, from_user, iv);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ext_aes_cmac_block, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 ext_aes_cmac_finish(hi_u32 id, hi_u8 *input, hi_u32 inlen, hi_u32 from_user, hi_u8 *mac)
{
    hi_u8 M_last[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_u8 padded[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_u8 K1[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_u8 K2[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_u8 IV[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    hi_u32 tail;
    hi_u32 ivlen = 0;
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_debug("id %d, inlen %d\n", id, inlen);

    hi_log_check_param(mac == HI_NULL);
    hi_log_check_param(input == HI_NULL);

    /* tail length */
    tail = inlen % AES_BLOCK_SIZE_IN_BYTE;
    if ((inlen > 0) && (tail == 0)) {
        tail = AES_BLOCK_SIZE_IN_BYTE;
    }
    inlen -= tail;

    /* Copy tail data then padding with zero */
    ret = ext_aes_cmac_chunk_copy(M_last, AES_BLOCK_SIZE_IN_BYTE,
        (const void *)(input + inlen), tail, from_user);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ext_aes_cmac_chunk_copy, ret);
        return ret;
    }

    /* compute part of complete block */
    if (inlen > 0) {
        ret = ext_aes_cmac_block(id, input, inlen, from_user, IV);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(ext_aes_cmac_block, ret);
            return ret;
        }
    }

    /* save last IV */
    ret = kapi_symc_getiv(id, IV, sizeof(IV), &ivlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_getiv, ret);
        return ret;
    }

    /* reset IV to zero */
    ret = memset_s(padded, sizeof(padded), 0, sizeof(padded));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }
    ret = kapi_symc_setiv(id, padded, AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_setiv, ret);
        return ret;
    }

    /* generate K1 and K2 */
    ret = generate_subkey(id, K1, K2, AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(generate_subkey, ret);
        return ret;
    }

    /* last block is complete block ? */
    if (tail == AES_BLOCK_SIZE_IN_BYTE) {
        ret = xor_128(M_last, K1, M_last, AES_BLOCK_SIZE_IN_BYTE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(xor_128, ret);
            return ret;
        }
    } else {
        padding(M_last, M_last, tail);
        ret = xor_128(M_last, K2, M_last, AES_BLOCK_SIZE_IN_BYTE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(xor_128, ret);
            return ret;
        }
    }

    /* recovery the last IV */
    ret = kapi_symc_setiv(id, IV, AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_setiv, ret);
        return ret;
    }

    /* continue to compute last block */
    ret = ext_aes_cmac_block(id, M_last, AES_BLOCK_SIZE_IN_BYTE, HI_FALSE, mac);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_setiv, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

