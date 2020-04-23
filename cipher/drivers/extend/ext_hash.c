/*****************************************************************************

    Copyright (C), 2017, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name     : ext_hash.c
  Version       : Initial Draft
  Created       : 2017
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/
#include "drv_osal_lib.h"
#include "cryp_hash.h"
#include "mbedtls/md.h"

#define HASH_MAX_BUFFER_SIZE    0x10000 /* 64K */

#if defined(SOFT_SHA1_SUPPORT) \
    || defined(SOFT_SHA256_SUPPORT) \
    || defined(SOFT_SHA512_SUPPORT)

void *mbedtls_hash_create(hash_mode mode)
{
    hi_s32 ret;
    mbedtls_md_type_t md_type;
    const mbedtls_md_info_t *info = HI_NULL;
    mbedtls_md_context_t *ctx = HI_NULL;

    hi_log_func_enter();

    /* convert to mebdtls type */
    md_type = MBEDTLS_MD_SHA1 + (mode - HASH_MODE_SHA1);

    info = mbedtls_md_info_from_type(md_type);
    if (info == HI_NULL) {
        hi_log_error("error, invalid hash mode %d\n", mode);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_NULL;
    }

    ctx = crypto_malloc(sizeof(mbedtls_md_context_t));
    if (ctx == HI_NULL) {
        hi_log_error("malloc hash context buffer failed!");
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_NULL;
    }

    ret = memset_s(ctx, sizeof(mbedtls_md_context_t), 0, sizeof(mbedtls_md_context_t));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return HI_NULL;
    }

    mbedtls_md_init(ctx);
    ret = mbedtls_md_setup(ctx, info, HI_FALSE);
    if (ret != HI_SUCCESS) {
        crypto_free(ctx);
        ctx = HI_NULL;
        hi_log_print_func_err(mbedtls_md_setup, ret);
        return HI_NULL;
    }

    ret = mbedtls_md_starts(ctx);
    if (ret != HI_SUCCESS) {
        mbedtls_md_free(ctx);
        crypto_free(ctx);
        ctx = HI_NULL;
        hi_log_print_func_err(mbedtls_md_starts, ret);
        return HI_NULL;
    }

    hi_log_func_exit();

    return ctx;
}

hi_s32 mbedtls_hash_update(void *ctx, hi_u8 *chunk, hi_u32 chunkLen, hash_chunk_src src)
{
    hi_u8 *ptr = HI_NULL;
    hi_s32 ret;
    mbedtls_md_context_t *md = ctx;
    hi_u32 offset = 0;
    hi_u32 length = 0;

    hi_log_func_enter();

    hi_log_check_param(ctx == HI_NULL);

    if (chunkLen == 0x00) {
        return HI_SUCCESS;
    }

    if (src == HASH_CHUNCK_SRC_USER) {
        ptr = crypto_malloc(HASH_MAX_BUFFER_SIZE);
        if (HI_NULL == ptr) {
            hi_log_error("malloc hash chunk buffer failed, chunkLen 0x%x\n!", chunkLen);
            hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
            return HI_ERR_CIPHER_FAILED_MEM;
        }

        while (offset < chunkLen) {
            length = chunkLen - offset;
            if (length > HASH_MAX_BUFFER_SIZE) {
                length = HASH_MAX_BUFFER_SIZE;
            }
            if (crypto_copy_from_user(ptr, HASH_MAX_BUFFER_SIZE, chunk + offset, length)) {
                hi_log_error("copy hash chunk from user failed!");
                hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
                ret = HI_ERR_CIPHER_FAILED_MEM;
                goto exit;
            }
            ret = mbedtls_md_update(md, ptr, length);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(mbedtls_md_update, ret);
                break;
            }
            crypto_msleep(1);
            offset   += length;
        }
    } else {
        ret = mbedtls_md_update(md, chunk, chunkLen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(mbedtls_md_update, ret);
        }
    }

exit:
    if (ptr != HI_NULL) {
        crypto_free(ptr);
        ptr = HI_NULL;
    }

    hi_log_func_exit();

    return ret;
}

hi_s32 mbedtls_hash_finish(void *ctx,  void *hash, hi_u32 hash_buf_len, hi_u32 *hashlen)
{
    hi_s32 ret;
    mbedtls_md_context_t *md = ctx;

    hi_log_func_enter();

    hi_log_check_param(ctx == HI_NULL);

    ret = mbedtls_md_finish(md, hash);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(mbedtls_md_finish, ret);
        return HI_NULL;
    }

    *hashlen = mbedtls_md_get_size(md->md_info);
    hi_log_check_length(*hashlen > hash_buf_len);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 mbedtls_hash_destory(const void *ctx)
{
    mbedtls_md_context_t *md = (mbedtls_md_context_t *)ctx;

    hi_log_func_enter();

    hi_log_check_param(ctx == HI_NULL);

    mbedtls_md_free(md);
    crypto_free(md);
    md = HI_NULL;

    hi_log_func_exit();
    return HI_SUCCESS;
}

#endif // End of SOFT_AES_CCM_GCM_SUPPORT
