/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of kapi_hash
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_hash.h"
#include "ext_alg.h"

#define HASH_SOFT_CHANNEL_MAX   0x0F
#define HASH_SOFT_CHANNEL_MASK  0x7FFF

/* ! /hmac ipad byte */
#define HMAC_IPAD_BYTE  0x36

/* ! /hmac opad byte */
#define HMAC_OPAD_BYTE  0x5C

#define HMAC_HASH       0x01
#define HMAC_AESCBC     0x02

typedef struct {
    hash_func *func; /* !<  HASH function */
    void *cryp_ctx;  /* !<  Context of cryp instance */
    hi_u32 hmac;     /* !<  HMAC or not */
    hi_u32 mac_id;   /* !<  CMAC handle */
#ifdef HI_PRODUCT_HMAC_SUPPORT
    hi_u8 hmac_ipad[HASH_BLOCK_SIZE_128]; /* !<  hmac ipad */
    hi_u8 hmac_opad[HASH_BLOCK_SIZE_128]; /* !<  hmac opad */
#endif
    crypto_owner owner; /* !<  user ID */
} kapi_hash_ctx;

/* ! Context of cipher */
static channel_context hash_ctx[HASH_SOFT_CHANNEL_MAX];

/* ! hash mutex */
static crypto_mutex hash_mutex;

#define kapi_check_hash_handle(handle)                                                        \
    do {                                                                                      \
        if ((HI_HANDLE_GET_MODULE_ID((handle) != HI_ID_CIPHER)) \
           || (HI_HANDLE_GET_PRIVATE_DATA((handle) != 0))) {                                  \
            hi_log_error("invalid handle 0x%x!\n", handle);                                             \
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);                                  \
            return HI_ERR_CIPHER_INVALID_HANDLE;                                              \
        }                                                                                     \
        if (HI_HANDLE_GET_CHAN_ID(handle) >= HASH_SOFT_CHANNEL_MAX) {                           \
            hi_log_error("chan %d is too large, max: %d\n", HI_HANDLE_GET_CHAN_ID(handle), HASH_SOFT_CHANNEL_MAX); \
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);                                  \
            return HI_ERR_CIPHER_INVALID_HANDLE;                                              \
        }                                                                                     \
        if (hash_ctx[HI_HANDLE_GET_CHAN_ID((handle))].open == HI_FALSE) {                         \
            hi_log_error("chan %d is not open\n", HI_HANDLE_GET_CHAN_ID(handle));                        \
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);                                  \
            return HI_ERR_CIPHER_INVALID_HANDLE;                                              \
        }                                                                                     \
    } while (0)

#define KAPI_HASH_LOCK()                         \
    do { \
        ret = crypto_mutex_lock(&hash_mutex);        \
        if (ret != HI_SUCCESS) {                     \
            hi_log_error("error, hash lock failed\n");            \
            hi_log_print_err_code(HI_ERR_CIPHER_BUSY); \
            return HI_ERR_CIPHER_BUSY;               \
        } \
    } while (0)

#define KAPI_HASH_UNLOCK() crypto_mutex_unlock(&hash_mutex)

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      hash */
/** @{ */ /** <!-- [kapi] */

hi_s32 kapi_hash_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_info("HASH init\n");

    crypto_mutex_init(&hash_mutex);

    ret = cryp_hash_init();
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_hash_init failed\n");
        hi_log_print_func_err(cryp_hash_init, ret);
        return ret;
    }

    /* Initialize soft channel list */
    ret = crypto_channel_init(hash_ctx, HASH_SOFT_CHANNEL_MAX, sizeof(kapi_hash_ctx));
    if (ret != HI_SUCCESS) {
        hi_log_error("error, hash channel list init failed\n");
        hi_log_print_func_err(crypto_channel_init, ret);
        cryp_hash_deinit();
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_hash_deinit(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_info("HASH deinit\n");

    ret = crypto_channel_deinit(hash_ctx, HASH_SOFT_CHANNEL_MAX);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, hash channel list deinit failed\n");
        hi_log_print_func_err(crypto_channel_deinit, ret);
        return ret;
    }

    cryp_hash_deinit();

    crypto_mutex_destroy(&hash_mutex);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static kapi_hash_ctx *kapi_hash_get_ctx(hi_u32 id)
{
    return crypto_channel_get_context(hash_ctx, HASH_SOFT_CHANNEL_MAX, id);
}

static hi_s32 kapi_hash_create(hi_u32 *id)
{
    hi_s32 ret;
    hi_u32 chn = 0;

    hi_log_func_enter();

    /* allocate a hash channel */
    ret = crypto_channel_alloc(hash_ctx, HASH_SOFT_CHANNEL_MAX, HASH_SOFT_CHANNEL_MASK, &chn);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, allocate hash channel failed\n");
        hi_log_print_func_err(crypto_channel_alloc, ret);
        return ret;
    }

    *id = chn;

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 kapi_hash_destroy(hi_u32 id)
{
    hi_log_func_enter();

    hi_log_check_param(id >= HASH_SOFT_CHANNEL_MAX);

    /* Free soft channel */
    crypto_channel_free(hash_ctx, HASH_SOFT_CHANNEL_MAX, id);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 kapi_hash_mode_transform(hi_cipher_hash_type type,
                                       hash_mode *mode, hi_u32 *hmac)
{
    *hmac = HI_FALSE;

    /* transform hash mode */
    switch (type) {
        case HI_CIPHER_HASH_TYPE_HMAC_SHA1: {
            *hmac = HMAC_HASH;
            *mode = HASH_MODE_SHA1;
            break;
        }
        case HI_CIPHER_HASH_TYPE_SHA1: {
            *mode = HASH_MODE_SHA1;
            break;
        }
        case HI_CIPHER_HASH_TYPE_HMAC_SHA224: {
            *hmac = HMAC_HASH;
            *mode = HASH_MODE_SHA224;
            break;
        }
        case HI_CIPHER_HASH_TYPE_SHA224: {
            *mode = HASH_MODE_SHA224;
            break;
        }
        case HI_CIPHER_HASH_TYPE_HMAC_SHA256: {
            *hmac = HMAC_HASH;
            *mode = HASH_MODE_SHA256;
            break;
        }
        case HI_CIPHER_HASH_TYPE_SHA256: {
            *mode = HASH_MODE_SHA256;
            break;
        }
#ifdef HI_PRODUCT_SHA512_SUPPORT
        case HI_CIPHER_HASH_TYPE_HMAC_SHA384: {
            *hmac = HMAC_HASH;
            *mode = HASH_MODE_SHA384;
            break;
        }
        case HI_CIPHER_HASH_TYPE_SHA384: {
            *mode = HASH_MODE_SHA384;
            break;
        }
        case HI_CIPHER_HASH_TYPE_HMAC_SHA512: {
            *hmac = HMAC_HASH;
            *mode = HASH_MODE_SHA512;
            break;
        }
        case HI_CIPHER_HASH_TYPE_SHA512: {
            *mode = HASH_MODE_SHA512;
            break;
        }
        case HI_CIPHER_HASH_TYPE_SM3: {
            *mode = HASH_MODE_SM3;
            break;
        }
#endif
#if defined(HASH_CMAC_SUPPORT)
        case HI_CIPHER_HASH_TYPE_CBCMAC: {
            *hmac = HMAC_AESCBC;
            *mode = HASH_MODE_COUNT;
            break;
        }
#endif
        default: {
            hi_log_error("error, nonsupport hash type %d\n", type);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    return HI_SUCCESS;
}

#ifdef HI_PRODUCT_HMAC_SUPPORT
static hi_s32 kapi_hmac_start(kapi_hash_ctx *ctx, hi_u8 *key, hi_u32 keylen)
{
    hi_s32 ret;
    hi_u8 sum[HASH_RESULT_MAX_SIZE] = { 0 };
    hi_u32 len = 0;
    hi_u32 i;

    hi_log_func_enter();

    hi_log_check_param(key == HI_NULL);

    /* clean ipad and opad */
    ret = memset_s(ctx->hmac_ipad, HASH_BLOCK_SIZE_128, 0x00, ctx->func->block_size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }
    ret = memset_s(ctx->hmac_opad, HASH_BLOCK_SIZE_128, 0x00, ctx->func->block_size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    /* compute K0 */
    if (keylen <= ctx->func->block_size) {
        /* If the length of K = B: set K0 = K.
         *
         * If the length of K > B: hash K to obtain an L byte string,
         * then append (B-L) zeros to create a B-byte
         * string K0 (i.e., K0 = H(K) || 00...00).
 */
        ret = memcpy_s(ctx->hmac_ipad, HASH_BLOCK_SIZE_128, key, keylen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
        ret = memcpy_s(ctx->hmac_opad, HASH_BLOCK_SIZE_128, key, keylen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
    } else {
        /* If the length of K > B: hash K to obtain an L byte string,
         * then append (B-L) zeros to create a B-byte
         * string K0 (i.e., K0 = H(K) || 00...00).
         */
        ctx->cryp_ctx = ctx->func->create(ctx->func->mode);
        if (ctx->cryp_ctx == HI_NULL) {
            hi_log_print_func_err(ctx->func->create, 0);
            return HI_ERR_CIPHER_BUSY;
        }

        /* update key */
        check_exit(ctx->func->update(ctx->cryp_ctx, key,
                                     keylen, HASH_CHUNCK_SRC_LOCAL));

        /* sum */
        check_exit(ctx->func->finish(ctx->cryp_ctx, sum, sizeof(sum), &len));
        ctx->func->destroy((const void *)ctx->cryp_ctx);
        ctx->cryp_ctx = HI_NULL;

        /* *** K0 = H(K) || 00...00 *** */
        check_exit(memcpy_s(ctx->hmac_ipad, HASH_BLOCK_SIZE_128, sum, len));
        check_exit(memcpy_s(ctx->hmac_opad, HASH_BLOCK_SIZE_128, sum, len));
    }

    /* Exclusive-Or K0 with ipad/opad byte to produce K0 ^ ipad and K0 ^ opad */
    for (i = 0; i < ctx->func->block_size; i++) {
        ctx->hmac_ipad[i] ^= HMAC_IPAD_BYTE;
        ctx->hmac_opad[i] ^= HMAC_OPAD_BYTE;
    }

    /* H(K0 ^ ipad) */
    ctx->cryp_ctx = ctx->func->create(ctx->func->mode);
    if (ctx->cryp_ctx == HI_NULL) {
        hi_log_print_func_err(ctx->func->create, 0);
        return HI_ERR_CIPHER_BUSY;
    }
    check_exit(ctx->func->update(ctx->cryp_ctx, ctx->hmac_ipad,
                                 ctx->func->block_size, HASH_CHUNCK_SRC_LOCAL));

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    ctx->func->destroy((const void *)ctx->cryp_ctx);
    ctx->cryp_ctx = HI_NULL;

    return ret;
}

static hi_s32 kapi_hmac_finish(kapi_hash_ctx *ctx, hi_u8 *hash, hi_u32 hash_buf_len, hi_u32 *hashlen)
{
    hi_s32 ret;
    hi_u8 sum[HASH_RESULT_MAX_SIZE] = { 0 };

    hi_log_func_enter();

    /* *** sum = H((K0 ^ ipad) || text). *** */
    ret = ctx->func->finish(ctx->cryp_ctx, sum, sizeof(sum), hashlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ctx->func->finish, ret);
        return ret;
    }

    ret = ctx->func->destroy((const void *)ctx->cryp_ctx);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ctx->func->destroy, ret);
        return ret;
    }

    ctx->cryp_ctx = HI_NULL;

    /* H((K0 ^ opad)|| sum). */
    ctx->cryp_ctx = ctx->func->create(ctx->func->mode);
    if (ctx->cryp_ctx == HI_NULL) {
        hi_log_print_func_err(ctx->func->create, 0);
        return HI_ERR_CIPHER_BUSY;
    }

    /* update(K0 ^ opad) */
    ret = ctx->func->update(ctx->cryp_ctx, ctx->hmac_opad,
                            ctx->func->block_size, HASH_CHUNCK_SRC_LOCAL);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ctx->func->update, ret);
        return ret;
    }

    /* update(sum) */
    ret = ctx->func->update(ctx->cryp_ctx, sum,
                            ctx->func->size, HASH_CHUNCK_SRC_LOCAL);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ctx->func->update, ret);
        return ret;
    }

    /* H */
    ret = ctx->func->finish(ctx->cryp_ctx, hash, hash_buf_len, hashlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ctx->func->finish, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

#if defined(HASH_CMAC_SUPPORT)
static hi_s32 kapi_hash_cbcmac_start(hi_u32 *id)
{
    hi_s32 ret;
    hi_u8 key[SYMC_KEY_SIZE] = { 0 };
    hi_u8 iv[AES_IV_SIZE] = { 0 };

    hi_log_func_enter();

    /* allocate a aes channel */
    ret = kapi_symc_create(id, HI_CIPHER_TYPE_NORMAL);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_create, ret);
        return ret;
    }

    ret = kapi_symc_config(*id, (HI_CIPHER_CA_TYPE_HCA << BITS_IN_BYTE) | HI_TRUE,
                           HI_CIPHER_ALG_AES,
                           HI_CIPHER_WORK_MODE_CBC,
                           HI_CIPHER_BIT_WIDTH_128BIT,
                           HI_CIPHER_KEY_AES_128BIT,
                           0, key, HI_NULL,
                           iv, AES_IV_SIZE,
                           CIPHER_IV_CHANGE_ONE_PKG,
                           ADDR_NULL, 0, 0);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_config, ret);
        kapi_symc_destroy(*id);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 kapi_hash_cbcmac_update(hi_u32 id, hi_u8 *in, hi_u32 inlen)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = ext_aes_cmac_update(id, in, inlen, HI_TRUE);
    if (ret != HI_SUCCESS) {
        kapi_symc_destroy(id);
    }

    hi_log_func_exit();
    return ret;
}

static hi_s32 kapi_hash_cbcmac_finish(hi_u32 id, hi_u8 *mac, hi_u32 mac_buf_len, hi_u32 *maclen)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = kapi_symc_getiv(id, mac, mac_buf_len, maclen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_getiv, ret);
        kapi_symc_destroy(id);
        return ret;
    }

    kapi_symc_destroy(id);

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

hi_s32 kapi_hash_start(hi_u32 *id, hi_cipher_hash_type type,
                       hi_u8 *key, hi_u32 keylen)
{
    hi_s32 ret;
    kapi_hash_ctx *ctx = HI_NULL;
    hash_mode mode = 0x00;
    hi_u32 hmac = 0;
    hi_u32 softHashId = 0;

    hi_log_func_enter();

    hi_log_check_param(id == HI_NULL);

    /* transform hash mode */
    ret = kapi_hash_mode_transform(type, &mode, &hmac);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_hash_mode_transform, ret);
        return ret;
    }

    KAPI_HASH_LOCK();

    /* Create hash channel */
    ret = kapi_hash_create(&softHashId);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, kapi_hash_create failed\n");
        hi_log_print_func_err(kapi_hash_create, ret);
        KAPI_HASH_UNLOCK();
        return ret;
    }

    ctx = kapi_hash_get_ctx(softHashId);
    if (ctx == HI_NULL) {
        hi_log_error("error, kapi_hash_get_ctx failed\n");
        hi_log_print_func_err(kapi_hash_get_ctx, 0);
        ret = HI_ERR_CIPHER_BUSY;
        goto error1;
    }

    ret = memset_s(ctx, sizeof(kapi_hash_ctx), 0, sizeof(kapi_hash_ctx));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, 0);
        ret = HI_ERR_CIPHER_BUSY;
        goto error1;
    }
    /* record owner */
    crypto_get_owner(&ctx->owner);
    ctx->hmac = hmac;

#if defined(HASH_CMAC_SUPPORT)
    if (ctx->hmac == HMAC_AESCBC) {
        KAPI_HASH_UNLOCK();

        ret = kapi_hash_cbcmac_start(&ctx->mac_id);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(kapi_hash_cbcmac_start, ret);
            kapi_hash_destroy(softHashId);
            return ret;
        }
        *id = HI_HANDLE_INIT(HI_ID_CIPHER, 0, softHashId);
        return HI_SUCCESS;
    }
#endif

    /* Clone the function from template of hash engine */
    ctx->cryp_ctx = HI_NULL;
    ctx->func = cryp_get_hash(mode);
    if (ctx->func == HI_NULL) {
        hi_log_error("error, cryp_get_hash failed\n");
        hi_log_print_func_err(cryp_get_hash, 0);
        ret = HI_ERR_CIPHER_INVALID_PARA;
        goto error1;
    }

    if ((ctx->func->create == HI_NULL)
        || (ctx->func->update == HI_NULL)
        || (ctx->func->finish == HI_NULL)
        || (ctx->func->destroy == HI_NULL)) {
        hi_log_error("error, cryp hash func is HI_NULL\n");
        hi_log_print_err_code(HI_ERR_CIPHER_UNSUPPORTED);
        ret = HI_ERR_CIPHER_UNSUPPORTED;
        goto error1;
    }

#ifdef HI_PRODUCT_HMAC_SUPPORT
    if (ctx->hmac == HMAC_HASH) {
        ret = kapi_hmac_start(ctx, key, keylen);
        if (ret != HI_SUCCESS) {
            hi_log_error("error, kapi_hmac_start failed\n");
            hi_log_print_func_err(kapi_hmac_start, ret);
            goto error1;
        }
    } else
#endif
    {
        ctx->cryp_ctx = ctx->func->create(mode);
        if (ctx->cryp_ctx == HI_NULL) {
            hi_log_error("error, hash context for hash engine failed\n");
            hi_log_print_func_err(ctx->func->create, 0);
            ret = HI_ERR_CIPHER_BUSY;
            goto error1;
        }
    }

    *id = HI_HANDLE_INIT(HI_ID_CIPHER, 0, softHashId);

    hi_log_info("[HASH] create handle 0x%x, mode %d, klen %d, owner 0x%x\n",
                *id, type, keylen, ctx->owner);

    KAPI_HASH_UNLOCK();

    hi_log_func_exit();
    return HI_SUCCESS;

error1:
    ret = kapi_hash_destroy(softHashId);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_hash_destroy, ret);
    }

    KAPI_HASH_UNLOCK();

    hi_log_func_exit();

    return ret;
}

hi_s32 kapi_hash_update(hi_u32 id, hi_u8 *input, hi_u32 length,
                        hash_chunk_src src)
{
    hi_s32 ret;
    kapi_hash_ctx *ctx = HI_NULL;
    hi_u32 softHashId;

    hi_log_func_enter();

    kapi_check_hash_handle(id);
    softHashId = HI_HANDLE_GET_CHAN_ID(id);

    ctx = kapi_hash_get_ctx(softHashId);
    hi_log_check_param(ctx == HI_NULL);
    hi_log_check_param(input > input + length); /* check overflow */

    check_owner(&ctx->owner);

    hi_log_info("[HASH] handle 0x%x, upadte 0x%x\n", id, length);

#if defined(HASH_CMAC_SUPPORT)
    if (ctx->hmac == HMAC_AESCBC) {
        ret = kapi_hash_cbcmac_update(ctx->mac_id, input, length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(kapi_hash_cbcmac_update, ret);
            kapi_hash_destroy(softHashId);
            return ret;
        }
        return HI_SUCCESS;
    }
#endif

    hi_log_check_param(ctx->func == HI_NULL);
    hi_log_check_param(ctx->func->update == HI_NULL);

    KAPI_HASH_LOCK();

    ret = ctx->func->update(ctx->cryp_ctx, input, length, src);

    /* release resource */
    if (ret != HI_SUCCESS) {
        ctx->func->destroy((const void *)ctx->cryp_ctx);
        ctx->cryp_ctx = HI_NULL;
        kapi_hash_destroy(softHashId);
        hi_log_print_func_err(ctx->func->update, ret);
    }

    KAPI_HASH_UNLOCK();

    hi_log_func_exit();

    return ret;
}

hi_s32 kapi_hash_finish(hi_u32 id, hi_u8 *hash, hi_u32 hash_buf_len, hi_u32 *hashlen)
{
    hi_s32 ret;
    kapi_hash_ctx *ctx = HI_NULL;
    hi_u32 softHashId;

    hi_log_func_enter();

    kapi_check_hash_handle(id);
    softHashId = HI_HANDLE_GET_CHAN_ID(id);

    hi_log_check_param(hash == HI_NULL);
    hi_log_check_param(hashlen == HI_NULL);

    ctx = kapi_hash_get_ctx(softHashId);
    hi_log_check_param(ctx == HI_NULL);

    check_owner(&ctx->owner);

#if defined(HASH_CMAC_SUPPORT)
    if (ctx->hmac == HMAC_AESCBC) {
        ret = kapi_hash_cbcmac_finish(ctx->mac_id, hash, hash_buf_len, hashlen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(kapi_hash_cbcmac_finish, ret);
            kapi_hash_destroy(softHashId);
            return ret;
        }
        kapi_hash_destroy(softHashId);
        return HI_SUCCESS;
    }
#endif

    hi_log_check_param(ctx->func == HI_NULL);
    hi_log_check_param(ctx->func->destroy == HI_NULL);

    KAPI_HASH_LOCK();

#ifdef HI_PRODUCT_HMAC_SUPPORT
    if (ctx->hmac == HMAC_HASH) {
        ret = kapi_hmac_finish(ctx, hash, hash_buf_len, hashlen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(kapi_hmac_finish, ret);
        }
    } else
#endif
    {
        ret = ctx->func->finish(ctx->cryp_ctx, hash, hash_buf_len, hashlen);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(ctx->func->finish, ret);
        }
    }

    /* release resource */
    ctx->func->destroy((const void *)ctx->cryp_ctx);
    ctx->cryp_ctx = HI_NULL;
    ret = kapi_hash_destroy(softHashId);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ctx->func->finish, ret);
    }
    hi_log_info("[HASH] destroy handle 0x%x, owner 0x%x\n", id, ctx->owner);

    KAPI_HASH_UNLOCK();

    hi_log_func_exit();

    return ret;
}

hi_s32 kapi_hash_release(void)
{
    hi_u32 i = 0;
    hi_s32 ret;
    kapi_hash_ctx *ctx = HI_NULL;
    hi_u32 all_closed = HI_TRUE;
    crypto_owner owner;

    hi_log_func_enter();

    KAPI_HASH_LOCK();

    crypto_get_owner(&owner);

    /* destroy the channel which are created by current user */
    for (i = 0; i < HASH_SOFT_CHANNEL_MAX; i++) {
        if (hash_ctx[i].open == HI_TRUE) {
            ctx = kapi_hash_get_ctx(i);
            if (ctx == HI_NULL) {
                hi_log_print_func_err(kapi_hash_get_ctx, HI_ERR_CIPHER_INVALID_POINT);
                KAPI_HASH_UNLOCK();
                return HI_ERR_CIPHER_INVALID_POINT;
            }
            if (osal_memncmp(&owner, sizeof(owner), &ctx->owner, sizeof(owner)) == 0) {
                hi_log_info("[HASH] release chn %d, owner 0x%x\n", i, owner);
                if ((ctx->func != HI_NULL)
                    && (ctx->func->destroy != HI_NULL)
                    && (ctx->cryp_ctx != HI_NULL)) {
                    ctx->func->destroy((const void *)ctx->cryp_ctx);
                }
                ctx->cryp_ctx = HI_NULL;
                ret = kapi_hash_destroy(i);
                if (ret != HI_SUCCESS) {
                    hi_log_print_func_err(kapi_hash_destroy, ret);
                    KAPI_HASH_UNLOCK();
                    return ret;
                }
            } else {
                /* some channel are opened by other user */
                all_closed = HI_FALSE;
            }
        }
    }

    if (all_closed == HI_TRUE) {
        /* release the hash mmz memory when all channel are closed */
        ret = cryp_hash_release();
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_hash_release, ret);
            KAPI_HASH_UNLOCK();
            return ret;
        }
    }

    KAPI_HASH_UNLOCK();

    hi_log_func_exit();
    return HI_SUCCESS;
}

/** @} */ /** <!-- ==== Structure Definition end ==== */
