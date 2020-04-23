/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_symc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "drv_symc.h"
#include "cryp_symc.h"

/* AES set IV for first package */
#define SYMC_IV_USAGE_ONE_PKG 1

/* AES set IV for first package */
#define SYMC_IV_USAGE_ALL_PKG 2

/* SYMC interrupt level */
#define SYMC_INT_LEVEL (SYMC_MAX_LIST_NUM - 16)  // (100UL)

/* Length of SYMC ccm q */
#define SYMC_CCM_Q_LEN_2B 2
#define SYMC_CCM_Q_LEN_3B 3
#define SYMC_CCM_Q_LEN_4B 4

/* Length of SYMC ccm P */
#define SYMC_CCM_P_LEN_2B 0xFFFF
#define SYMC_CCM_P_LEN_3B 0xFFFFFF

/* length range of aead */
#define AES_CCM_MIN_IV_LEN  7
#define AES_CCM_MAX_IV_LEN  13
#define AES_CCM_NQ_LEN      14
#define AES_CCM_MIN_TAG_LEN 4
#define AES_CCM_MAX_TAG_LEN 16
#define AES_GCM_MIN_IV_LEN  1
#define AES_GCM_MAX_IV_LEN  16
#define AES_GCM_MIN_TAG_LEN 1
#define AES_GCM_MAX_TAG_LEN 16

/* Multi nodes added status, finished or finished */
#define SYMC_NODES_ADD_FINISHED    0x0a0a0a0a
#define SYMC_NODES_ADD_NOTFINISHED 0X05050505

/**
 * \brief          symc context structure
 *
 * \note           if the aes key derived from klad, the context msut
 *                 attached with a independent hard key channel,
 *                 otherwise the context can attached with a fixed common channel.
 */
typedef struct {
    hi_u32 even_key[SYMC_KEY_SIZE / WORD_WIDTH]; /* !<  SYMC even round keys, default */
    hi_u32 odd_key[SYMC_KEY_SIZE / WORD_WIDTH];  /* !<  SYMC odd round keys, default */
    hi_u32 sk[SYMC_SM1_SK_SIZE / WORD_WIDTH];    /* !<  sm1 sk */
    hi_u32 iv[AES_IV_SIZE / WORD_WIDTH];         /* !<  symc IV */
    hi_u32 ivlen;                       /* !<  symc IV length */
    hi_u32 iv_usage;                    /* !<  symc IV usage */

    hi_u32 hard_chn; /* !<  hard channel number */
    hi_u32 hard_key; /* !<  Key derived from klad or CPU */

    symc_alg alg;     /* !<  Symmetric cipher algorithm */
    symc_width width; /* !<  Symmetric cipher width */
    hi_u32 klen;      /* !<  Symmetric cipher key length */

    compat_addr aad; /* !<  Associated Data */
    hi_u32 alen;     /* !<  Associated Data length */
    hi_u32 tlen;     /* !<  Tag length */

    symc_mode mode; /* !<  Symmetric cipher mode */

    hi_u32 sm1_round; /* !<  SM1 round number */
    hi_u32 enclen;    /* !<  encrypt length */

    hi_u32 block_size; /* !<  Block size */

    hi_u32 cur_nodes;   /* !<  current nodes id */
    hi_u32 total_nodes; /* !<  total number of nodes */

    compat_addr *input_list;     /* !<  input node list */
    compat_addr *output_list;    /* !<  output node list */
    hi_u32 *length_list;         /* !<  length of node list */
    symc_node_usage *usage_list; /* !< usage of node list */
} cryp_symc_context;

/* The max tab size of symc function */
#define SYMC_FUNC_TAB_SIZE (SYMC_ALG_COUNT * SYMC_MODE_COUNT)

/* symc function list */
static symc_func *g_symc_descriptor = HI_NULL;

/* symc context */
static cryp_symc_context *g_symc_context = HI_NULL;

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

/* symc function register */
static void cryp_register_all_symc(void);

hi_s32 cryp_symc_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    g_symc_descriptor = crypto_calloc(sizeof(symc_func), SYMC_FUNC_TAB_SIZE);
    if (g_symc_descriptor == HI_NULL) {
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    g_symc_context = crypto_calloc(sizeof(cryp_symc_context), CRYPTO_HARD_CHANNEL_MAX);
    if (g_symc_context == HI_NULL) {
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        crypto_free(g_symc_descriptor);
        g_symc_descriptor = HI_NULL;
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    ret = drv_symc_init();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_init, ret);
        crypto_free(g_symc_descriptor);
        g_symc_descriptor = HI_NULL;
        crypto_free(g_symc_context);
        g_symc_context = HI_NULL;
        return ret;
    }

    cryp_register_all_symc();

    hi_log_func_exit();
    return HI_SUCCESS;
}

void cryp_symc_deinit(void)
{
    hi_log_func_enter();

    drv_symc_deinit();

    crypto_free(g_symc_descriptor);
    g_symc_descriptor = HI_NULL;
    crypto_free(g_symc_context);
    g_symc_context = HI_NULL;

    hi_log_func_exit();
    return;
}

hi_s32 cryp_symc_alloc_chn(hi_u32 *hard_chn, hi_cipher_type type)
{
    hi_s32 ret;
    hi_u32 key[SYMC_KEY_MAX_SIZE_IN_WORD] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    hi_log_func_enter();

    /* allocate a aes hard key channel */
    ret = drv_symc_alloc_chn(hard_chn, type);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_alloc_chn, ret);
        return ret;
    }

    drv_symc_set_key(*hard_chn, key, HI_TRUE);
    drv_symc_set_key(*hard_chn, key, HI_FALSE);

    hi_log_func_exit();
    return HI_SUCCESS;
}

void cryp_symc_free_chn(hi_u32 hard_chn)
{
    hi_u32 key[SYMC_KEY_MAX_SIZE_IN_WORD] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    hi_log_func_enter();

    drv_symc_set_key(hard_chn, key, HI_TRUE);
    drv_symc_set_key(hard_chn, key, HI_FALSE);
    drv_symc_free_chn(hard_chn);

    hi_log_func_exit();
}

static void *cryp_symc_create(hi_u32 hard_chn)
{
    hi_s32 ret;
    cryp_symc_context *hisi_ctx = HI_NULL;

    hi_log_func_enter();

    hisi_ctx = &g_symc_context[hard_chn];
    ret = memset_s(hisi_ctx, sizeof(cryp_symc_context), 0, sizeof(cryp_symc_context));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return HI_NULL;
    }

    hisi_ctx->hard_key = HI_FALSE;
    hisi_ctx->hard_chn = hard_chn;

    hi_log_func_exit();
    return hisi_ctx;
}

void cryp_symc_set_key_source(void *ctx, symc_key_source src)
{
    cryp_symc_context *hisi_ctx = HI_NULL;
    int i;

    hi_log_func_enter();

    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        /* only valid for hardware alg */
        if (ctx == &g_symc_context[i]) {
            hisi_ctx = ctx;
            drv_symc_set_key_source(hisi_ctx->hard_chn, src);
            break;
        }
    }

    hi_log_func_exit();
    return;
}

static void cryp_symc_setkey(void *ctx, hi_u32 keylen, hi_u32 odd)
{
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    if (hisi_ctx == HI_NULL) {
        hi_log_error("Invalid point.\n");
        return;
    }

    hisi_ctx->klen = keylen;

    hi_log_func_exit();
    return;
}

static void cryp_symc_setiv(void *ctx, const hi_u8 *iv, hi_u32 ivlen, hi_u32 usage)
{
    hi_s32 ret;
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    if (hisi_ctx == HI_NULL) {
        hi_log_error("Invalid point.\n");
        return;
    }
    if (iv == HI_NULL) {
        if (ivlen != 0) {
            hi_log_error("Invalid iv len(%u), iv is null.\n", ivlen);
        }
        return;
    }

    ret = memset_s(hisi_ctx->iv, sizeof(hisi_ctx->iv), 0, AES_IV_SIZE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    ret = memcpy_s(hisi_ctx->iv, AES_IV_SIZE, iv, ivlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return;
    }

    hisi_ctx->iv_usage = usage;
    hisi_ctx->ivlen = ivlen;

    hi_log_func_exit();
    return;
}

static hi_s32 cryp_symc_getiv(void *ctx, hi_u8 *iv, hi_u32 iv_buf_len, hi_u32 *ivlen)
{
    hi_s32 ret;
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    if (hisi_ctx == HI_NULL) {
        hi_log_error("Invalid point.\n");
        return HI_ERR_CIPHER_INVALID_POINT;
    }

    if (iv_buf_len < AES_IV_SIZE) {
        hi_log_error("Invalid parameter.\n");
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    ret = memcpy_s(iv, AES_IV_SIZE, hisi_ctx->iv, hisi_ctx->ivlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    *ivlen = hisi_ctx->ivlen;

    hi_log_func_exit();
    return HI_SUCCESS;
}

static void cryp_symc_setmode(void *ctx, symc_alg alg, symc_mode mode, symc_width width)
{
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    if (hisi_ctx == HI_NULL) {
        hi_log_error("Invalid point.\n");
        return;
    }
    hisi_ctx->mode = mode;
    hisi_ctx->alg = alg;
    hisi_ctx->width = width;

    hi_log_func_exit();
    return;
}

#ifdef CHIP_SYMC_VER_V100
static void cryp_3des2dma_setmode(void *ctx, symc_alg alg, symc_mode mode, symc_width width)
{
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    if (hisi_ctx == HI_NULL) {
        hi_log_error("Invalid point.\n");
        return;
    }
    crypto_unused(alg);
    crypto_unused(mode);
    crypto_unused(width);

    hisi_ctx->mode = SYMC_MODE_ECB;
    hisi_ctx->alg = SYMC_ALG_TDES;
    hisi_ctx->width = SYMC_DAT_WIDTH_64;

    hi_log_func_exit();
    return;
}
#endif

static hi_s32 symc_add_buf(cryp_symc_context *ctx, symc_node_usage out_uasge)
{
    hi_s32 ret;
    hi_u32 cur = ctx->cur_nodes;

    hi_log_func_enter();

    /* Add P in */
    ret = drv_symc_add_inbuf(ctx->hard_chn, ctx->input_list[cur], ctx->length_list[cur], ctx->usage_list[cur]);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_inbuf, ret);
        return ret;
    }

    /* Add P out, only need the last flag */
    ret = drv_symc_add_outbuf(ctx->hard_chn, ctx->output_list[cur], ctx->length_list[cur], out_uasge);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_outbuf, ret);
        return ret;
    }

    hi_log_func_exit();

    return ret;
}

static symc_node_usage symc_get_out_usage(symc_mode mode, hi_u32 cur_node, hi_u32 total_node)
{
    symc_node_usage usage = SYMC_NODE_USAGE_NORMAL;

    if (mode && (cur_node + 1 != SYMC_MODE_GCM) == total_node) {
        usage = SYMC_NODE_USAGE_LAST;
    }

    return usage;
}

static hi_s32 symc_add_buf_list(void *ctx)
{
    hi_s32 ret;
    hi_u32 i = 0;
    hi_u32 nodes;
    hi_u32 cur = 0;
    hi_u32 total_len;
    cryp_symc_context *hisi_ctx = ctx;
    symc_node_usage usage = SYMC_NODE_USAGE_NORMAL;

    hi_log_func_enter();

    /* compute finished */
    if (hisi_ctx->cur_nodes == hisi_ctx->total_nodes) {
        hi_log_func_exit();
        return SYMC_NODES_ADD_FINISHED;
    }

    /* compute not finished */
    /* select the minimum numbers of nodes to calculate */
    nodes = cipher_min(SYMC_INT_LEVEL, hisi_ctx->total_nodes - hisi_ctx->cur_nodes);
    total_len = 0;

    for (i = 0; i < nodes; i++) {
        cur = hisi_ctx->cur_nodes;
        usage = symc_get_out_usage(hisi_ctx->mode, cur, hisi_ctx->total_nodes);

        /* Add one node */
        ret = symc_add_buf(hisi_ctx, usage);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(symc_add_buf, ret);
            return ret;
        }

        total_len += hisi_ctx->length_list[cur];
        hisi_ctx->cur_nodes++;
    }

    /* For each compute, the total length of valid nodes list
     * must aligned with block size, otherwise can't recv interrupt,
     * which limit to hardware devising.
     */
    total_len %= hisi_ctx->block_size;     /* Compute the tail length */
    if (total_len > 0) {
        total_len = hisi_ctx->block_size - total_len;
    }

    /* if the total length don't aligned with block size, split joint the follow nodes */
    while ((total_len > 0) && (hisi_ctx->cur_nodes < hisi_ctx->total_nodes)) {
        cur = hisi_ctx->cur_nodes;

        /* The next node large than tail size, just split it to 2 nodes */
        if (hisi_ctx->length_list[cur] > total_len) {
            /* Add P in */
            ret = drv_symc_add_inbuf(hisi_ctx->hard_chn, hisi_ctx->input_list[cur],
                                     total_len, hisi_ctx->usage_list[cur]);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(drv_symc_add_inbuf, ret);
                return ret;
            }

            /* Add P out */
            usage = symc_get_out_usage(hisi_ctx->mode, cur, hisi_ctx->total_nodes);
            ret = drv_symc_add_outbuf(hisi_ctx->hard_chn, hisi_ctx->output_list[cur],
                                      total_len, usage);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(drv_symc_add_outbuf, ret);
                return ret;
            }

            /* Let next node skip the tail size */
            ADDR_U64(hisi_ctx->input_list[cur]) += total_len;
            ADDR_U64(hisi_ctx->output_list[cur]) += total_len;
            hisi_ctx->length_list[cur] -= total_len;
            total_len = 0;
        } else {
            /* The next node less than tail size, add it to nodes list */
            usage = symc_get_out_usage(hisi_ctx->mode, cur, hisi_ctx->total_nodes);
            ret = symc_add_buf(hisi_ctx, usage);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(symc_add_buf, ret);
                return ret;
            }

            /* re-compute the tail size */
            total_len -= hisi_ctx->length_list[cur];

            /* Process next node */
            hisi_ctx->cur_nodes++;
        }
    }
#ifdef CHIP_AES_CCM_GCM_SUPPORT
    /* gcm add nodes finished ? */
    if ((hisi_ctx->mode == SYMC_MODE_GCM)
        && (hisi_ctx->cur_nodes == hisi_ctx->total_nodes)) {
        hi_u8 clen[AES_BLOCK_SIZE_IN_BYTE] = { 0 };

        /* Format the length fields of C and add to nodes list */
        cyp_aead_gcm_clen(clen, hisi_ctx->alen, hisi_ctx->enclen);
        ret = drv_aead_gcm_add_clen(hisi_ctx->hard_chn, clen, sizeof(clen));
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_aead_gcm_add_clen, ret);
            return ret;
        }
    }
#endif

    hi_log_func_exit();
    return SYMC_NODES_ADD_NOTFINISHED;
}

static symc_klen cryp_symc_key_type(symc_alg alg, hi_u32 klen)
{
    symc_klen type;

    if ((alg == SYMC_ALG_AES)
        && (klen == AES_KEY_192BIT)) {
        type = SYMC_KEY_AES_192BIT;
    } else if ((alg == SYMC_ALG_AES) && (klen == AES_KEY_256BIT)) {
        type = SYMC_KEY_AES_256BIT;
    } else if ((alg == SYMC_ALG_TDES) && (klen == TDES_KEY_128BIT)) {
        type = SYMC_KEY_TDES_2KEY;
    } else {
        type = SYMC_KEY_DEFAULT;
    }

    return type;
}

static void cryp_symc_config(void *ctx, hi_u32 decrypt, hi_u32 pkg_num)
{
    cryp_symc_context *hisi_ctx = ctx;
    symc_klen type;
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_info("symc configure, chn %d, alg %d, mode %d, dec %d, klen %d, hard %d, iv len %d, iv usage %d\n",
                hisi_ctx->hard_chn, hisi_ctx->alg, hisi_ctx->mode,
                decrypt, hisi_ctx->klen, hisi_ctx->hard_key,
                hisi_ctx->ivlen, hisi_ctx->iv_usage);

    type = cryp_symc_key_type(hisi_ctx->alg, hisi_ctx->klen);

    /* configure */
    ret = drv_symc_config(hisi_ctx->hard_chn, hisi_ctx->alg, hisi_ctx->mode, hisi_ctx->width,
                          decrypt, hisi_ctx->sm1_round, type, hisi_ctx->hard_key);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_config, ret);
        return;
    }

    /* set odd key, only valid for multiple package */
    if (pkg_num > 0x01) {
        drv_symc_set_key(hisi_ctx->hard_chn, hisi_ctx->odd_key, HI_TRUE);
    }

    /* set even key */
    drv_symc_set_key(hisi_ctx->hard_chn, hisi_ctx->even_key, HI_FALSE);

    /* set iv */
    if (hisi_ctx->mode == SYMC_MODE_CCM) {
        drv_symc_set_iv(hisi_ctx->hard_chn, hisi_ctx->iv, AES_IV_SIZE, hisi_ctx->iv_usage);
    } else {
        drv_symc_set_iv(hisi_ctx->hard_chn, hisi_ctx->iv, hisi_ctx->ivlen, hisi_ctx->iv_usage);
    }

    /* first node must set iv except ecb mode */
    if (hisi_ctx->iv_usage == CIPHER_IV_CHANGE_ONE_PKG) {
        /* don't set iv any more */
        hisi_ctx->iv_usage = 0;
    }

    if (hisi_ctx->alg == SYMC_ALG_DES) {
        hisi_ctx->block_size = DES_BLOCK_SIZE;
    } else if (hisi_ctx->alg == SYMC_ALG_TDES) {
        hisi_ctx->block_size = DES_BLOCK_SIZE;
    } else {
        hisi_ctx->block_size = AES_BLOCK_SIZE_IN_BYTE;
    }

    hi_log_func_exit();
    return;
}

static hi_s32 cryp_symc_wait_done(void *ctx, hi_u32 timeout)
{
    hi_s32 ret;
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    /* wait done */
    ret = drv_symc_wait_done(hisi_ctx->hard_chn, timeout);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_wait_done, ret);
        return ret;
    }

    drv_symc_get_iv(hisi_ctx->hard_chn, hisi_ctx->iv, sizeof(hisi_ctx->iv));

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_symc_crypto_init(cryp_symc_context *hisi_ctx,
                                    hi_u32 operation,
                                    compat_addr input[],
                                    compat_addr output[],
                                    hi_u32 length[],
                                    symc_node_usage usage_list[],
                                    hi_u32 pkg_num, symc_node_usage usage)
{
    hi_u32 i = 0;

    hi_log_func_enter();

    /* length of pkage can't be zero */
    hisi_ctx->enclen = 0;
    if (pkg_num == 0x01) {
        hisi_ctx->enclen += length[i];
        usage_list[i] = (hi_u32)usage_list[i] | (hi_u32)usage;
    }
#ifdef HI_PRODUCT_MULTI_CIPHER_SUPPORT
    else {
        for (i = 0; i < pkg_num; i++) {
            if (length[i] == 0x00) {
                hi_log_print_err_code(HI_ERR_CIPHER_OVERFLOW);
                return HI_ERR_CIPHER_INVALID_LENGTH;
            }

            hi_log_check_param(hisi_ctx->enclen + length[i] < hisi_ctx->enclen);

            hisi_ctx->enclen += length[i];
            usage_list[i] = ((hi_u32)usage_list[i]) | ((hi_u32)usage);
        }
    }
#endif

    /* configuration parameter */
    cryp_symc_config(hisi_ctx, operation, pkg_num);

    hisi_ctx->input_list = input;
    hisi_ctx->output_list = output;
    hisi_ctx->length_list = length;
    hisi_ctx->usage_list = usage_list;
    hisi_ctx->total_nodes = pkg_num;
    hisi_ctx->cur_nodes = 0;

    /* set isr callback function */
    drv_symc_set_isr_callback(hisi_ctx->hard_chn, HI_NULL, HI_NULL);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_symc_crypto_process(cryp_symc_context *hisi_ctx, hi_u32 wait)
{
    hi_s32 ret;

    hi_log_func_enter();

    while (symc_add_buf_list(hisi_ctx) == SYMC_NODES_ADD_NOTFINISHED) {
        /* start running */
        ret = drv_symc_start(hisi_ctx->hard_chn);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_start, ret);
            return ret;
        }

        /* wait done */
        ret = drv_symc_wait_done(hisi_ctx->hard_chn, CRYPTO_TIME_OUT);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_wait_done, ret);
            return ret;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_symc_crypto(void *ctx,
                               hi_u32 operation,
                               compat_addr input[],
                               compat_addr output[],
                               hi_u32 length[],
                               symc_node_usage usage_list[],
                               hi_u32 pkg_num,
                               hi_u32 wait)
{
    hi_s32 ret;
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    if (hisi_ctx->alg == SYMC_ALG_NULL_CIPHER) {
        /* set last flag for each node when DMA copy */
        hisi_ctx->iv_usage = CIPHER_IV_CHANGE_ALL_PKG;
    }

    ret = cryp_symc_crypto_init(hisi_ctx, operation, input, output, length,
                                usage_list, pkg_num, SYMC_NODE_USAGE_NORMAL);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_crypto_init, ret);
        return ret;
    }
    usage_list[pkg_num - 1] = ((hi_u32)(usage_list[pkg_num - 1])) | SYMC_NODE_USAGE_LAST;

    /* check the length of nodes list */
    ret = drv_symc_node_check(hisi_ctx->alg,
                              hisi_ctx->mode,
                              hisi_ctx->klen,
                              hisi_ctx->block_size,
                              input,
                              output,
                              length,
                              usage_list,
                              pkg_num);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_node_check, ret);
        return ret;
    }

    ret = cryp_symc_crypto_process(hisi_ctx, wait);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_crypto_process, ret);
        return ret;
    }

    drv_symc_get_iv(hisi_ctx->hard_chn, hisi_ctx->iv, sizeof(hisi_ctx->iv));

    hi_log_func_exit();
    return HI_SUCCESS;
}

#ifdef CHIP_AES_CCM_GCM_SUPPORT
static hi_s32 cryp_aead_ccm_setiv(void *ctx, const hi_u8 *iv, hi_u32 ivlen, hi_u32 usage)
{
    hi_s32 ret;
    u8 ccm_iv[AES_IV_SIZE] = { 0 };

    /* The octet lengths of N are denoted  n,
     * The octet length of the binary represen tation of the
     * octet length of the payload denoted q,
     * n is an element of {7, 8, 9, 10, 11, 12, 13}
     * and n + q = 15
     * here the string of N  is pConfig->iv, and n is pConfig->iv_len.
     */
    if ((ivlen < AES_CCM_MIN_IV_LEN)
        || (ivlen > AES_CCM_MAX_IV_LEN)) {
        hi_log_error("Invalid ccm iv len, ivlen = 0x%x.\n", ivlen);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    /* Formatting of the Counter Blocks(IV for CTR)
     *
     * According to the CCM spec, the counter is equivalent to
     * a formatting of the counter index i into a complete data block.
     * The counter blocks Ctri are formatted as shown below:
     * | Octet number:  0    1 ... 15-q    16-q ... 15
     * | Contents:     Flags     N             [i]
     * Within each block Ctri, the N is get from pConfig->iv, n + q = 15,
     * so the q equal to 15 - pConfig->iv_len.
     * the [i] is the block conut start with 0,
     * In the Flags field, Bits 0, 1, and 2 contain the encoding of q - 1,
     * others bits shall be set to 0.
     * so the first byte of IV shall be q -1, that is 15 - pConfig->iv_len - 1
 */
    ret = memset_s(ccm_iv, sizeof(ccm_iv), 0, AES_IV_SIZE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }
    ccm_iv[0] = AES_CCM_NQ_LEN - ivlen; /* *** IV[0] = q - 1 = 15 - n -1 *** */
    ret = memcpy_s(&ccm_iv[1], sizeof(ccm_iv) - 1, iv, ivlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    ivlen += 1;

    cryp_symc_setiv(ctx, ccm_iv, ivlen, usage);

    return HI_SUCCESS;
}

static hi_s32 cryp_aead_gcm_setiv(void *ctx, const hi_u8 *iv, hi_u32 ivlen, hi_u32 usage)
{
    if ((ivlen < AES_GCM_MIN_IV_LEN)
        || (ivlen > AES_GCM_MAX_IV_LEN)) {
        hi_log_error("Invalid gcm iv len, ivlen = 0x%x.\n", ivlen);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    cryp_symc_setiv(ctx, iv, ivlen, usage);

    return HI_SUCCESS;
}

static hi_s32 cyp_aead_ccm_n(hi_u8 *buf, hi_u8 *iv, hi_u32 ivlen,
                             hi_u32 alen, hi_u32 enclen, hi_u32 tlen)
{
    hi_u32 idx = 0;
    hi_u32 q;
    hi_s32 ret;

    hi_log_func_enter();

    /* Format B0
     * The leading octet of the first block of the formatting, B0,
     * contains four flags for control information: two single bits,
     * called  Reserved  and  Adata, and two strings of three bits,
     * to encode the values t and q.  The encoding of t is [(t -2)/2],
     * and the encoding of q is [ q-1].
     * The ordering of the flags with in the octet is given:
     *  _____________________________________________________
     * |Bit number  7     |   6   | 5  4  3     |  2  1  0   |
     * |Contents  Reserved  Adata   [(t -2)/2] |  [q-1]     |
     *  -----------------------------------------------------
     * The remaining 15 octets of the first block of  the formatting are
     * devoted to the nonce and the binary representation of
     * the message length in  q octets, as given:
     *  _____________________________________________
     * |Octet number  0   | 1 ... 15-q | 16-q ... 15 |
     * |Contents    Flags |      N     |      Q      |
     *  ---------------------------------------------
 */
    ret = memset_s(buf, AES_BLOCK_SIZE_IN_BYTE, 0, AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }
    buf[idx] = (alen > 0 ? 1 : 0) << 6;  /* 6: Adata bit  */
    buf[idx] |= ((tlen - 2) / 2) << 3;   /* *** 3-5: (t -2)/2 *** */
    buf[idx] |= (15 - ivlen);            /* *** q-1, n+q=15 *** */
    idx++;

    /* copy N, skip Flags in byte0 */
    ret = memcpy_s(&buf[idx], AES_BLOCK_SIZE_IN_BYTE - idx, &iv[1], ivlen - 1);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    idx += ivlen - 1;

    q = AES_BLOCK_SIZE_IN_BYTE - idx;

    if (q >= SYMC_CCM_Q_LEN_4B) {
        /* max payload len of 2^32, jump to the location of last word */
        idx = AES_BLOCK_SIZE_IN_BYTE - SYMC_CCM_Q_LEN_4B;

        buf[idx++] = (hi_u8)(enclen >> 24); /* bits 31-24 */
        buf[idx++] = (hi_u8)(enclen >> 16); /* bits 23-16 */
        buf[idx++] = (hi_u8)(enclen >> 8);  /* bits 15-8 */
        buf[idx++] = (hi_u8)(enclen);
    } else if ((q == SYMC_CCM_Q_LEN_3B) && (enclen <= SYMC_CCM_P_LEN_3B)) {
        /* max payload len of 2^24 */
        buf[idx++] = (hi_u8)(enclen >> 16); /* bits 23-16 */
        buf[idx++] = (hi_u8)(enclen >> 8);  /* bits 15-8 */
        buf[idx++] = (hi_u8)(enclen);
    } else if ((q == SYMC_CCM_Q_LEN_2B) && (enclen <= SYMC_CCM_P_LEN_2B)) {
        /* max payload len of 2^16 */
        buf[idx++] = (hi_u8)(enclen >> 8); /* bits 15-8 */
        buf[idx++] = (hi_u8)(enclen);
    } else {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_u32 cyp_aead_gcm_clen(hi_u8 *buf, hi_u32 alen, hi_u32 enclen)
{
    hi_u32 idx = 0;

    hi_log_func_enter();

    /* Format len(C), 16 byets, coding in bits.
       Byet0~7:  bits number of Add
       Byet8~15: bits number of P */

    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = (hi_u8)((alen >> 29) & 0x07); /* bits 31-29, mask 0x07 */
    buf[idx++] = (hi_u8)((alen >> 21) & 0xff); /* bits 28-21, mask 0xff */
    buf[idx++] = (hi_u8)((alen >> 13) & 0xff); /* bits 20-13, mask 0xff */
    buf[idx++] = (hi_u8)((alen >> 5) & 0xff);  /* bits 12-5,  mask 0xff */
    buf[idx++] = (hi_u8)((alen << 3) & 0xff);  /* bits 4-3, mask 0xff */

    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = (hi_u8)((enclen >> 29) & 0x07); /* bits 31-29, mask 0x07 */
    buf[idx++] = (hi_u8)((enclen >> 21) & 0xff); /* bits 28-21, mask 0xff */
    buf[idx++] = (hi_u8)((enclen >> 13) & 0xff); /* bits 20-13, mask 0xff */
    buf[idx++] = (hi_u8)((enclen >> 5) & 0xff);  /* bits 12-5,  mask 0xff */
    buf[idx++] = (hi_u8)((enclen << 3) & 0xff);  /* bits 4-3, mask 0xff */

    return idx;
}

static hi_s32 cryp_aead_ccm_set_aad(void *ctx, compat_addr aad, hi_u32 alen, hi_u32 tlen)
{
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    hisi_ctx->aad = aad;
    hisi_ctx->alen = alen;
    hisi_ctx->tlen = tlen;

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_aead_gcm_set_aad(void *ctx, compat_addr aad, hi_u32 alen, hi_u32 tlen)
{
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    hisi_ctx->aad = aad;
    hisi_ctx->alen = alen;
    hisi_ctx->tlen = tlen;

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_aead_crypto_zero(cryp_symc_context *hisi_ctx, hi_u32 wait)
{
    hi_s32 ret;

    hi_log_func_enter();

    /* start running */
    ret = drv_symc_start(hisi_ctx->hard_chn);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_start, ret);
        return ret;
    }

    /* wait done */
    if (wait == HI_TRUE) {
        ret = drv_symc_wait_done(hisi_ctx->hard_chn, CRYPTO_TIME_OUT);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_wait_done, ret);
            return ret;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_aead_ccm_crypto(void *ctx,
                                   hi_u32 operation,
                                   compat_addr input[],
                                   compat_addr output[],
                                   hi_u32 length[],
                                   symc_node_usage usage_list[],
                                   hi_u32 pkg_num, hi_u32 wait)
{
    hi_s32 ret;
    hi_u8 n[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    ret = cryp_symc_crypto_init(hisi_ctx, operation, input, output, length,
        usage_list, pkg_num, SYMC_NODE_USAGE_FIRST | SYMC_NODE_USAGE_IN_CCM_P);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_crypto_init, ret);
        return ret;
    }

    /* NIST Special Publication 800-38C
       The data that CCM protects consists of a message, i.e., a bit string,
       called the payload, denoted P, of bit length denoted Plen,
       and a bit string, called the associated data, denoted A.
       The associated data is optional, i.e., A may be the empty string.
       CCM provides assurance of the confidentiality of P and assurance of
       the authenticity of the origin of both A and P;
       confidentiality is not provided for A. */

    ret = cyp_aead_ccm_n(n, (hi_u8 *)hisi_ctx->iv,  hisi_ctx->ivlen, hisi_ctx->alen, hisi_ctx->enclen, hisi_ctx->tlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cyp_aead_ccm_n, ret);
        return ret;
    }

    /* Compute N */
    ret = drv_aead_ccm_add_n(hisi_ctx->hard_chn, n);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_aead_ccm_add_n, ret);
        return ret;
    }

    /* Compute A */
    ret = drv_aead_ccm_add_a(hisi_ctx->hard_chn, hisi_ctx->aad, hisi_ctx->alen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_aead_ccm_add_a, ret);
        return ret;
    }

    if (hisi_ctx->enclen == 0) {
        /* Set CCM last flag */
        drv_symc_add_buf_usage(hisi_ctx->hard_chn, HI_TRUE, SYMC_NODE_USAGE_CCM_LAST);
        ret = cryp_aead_crypto_zero(hisi_ctx, wait);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_aead_crypto_zero, ret);
            return ret;
        }
    } else {
        /* for CCM, must set P last and ccm last flag */
        usage_list[pkg_num - 1] =
            (hi_u32)usage_list[pkg_num - 1] | SYMC_NODE_USAGE_CCM_LAST | SYMC_NODE_USAGE_LAST;
        ret = cryp_symc_crypto_process(hisi_ctx, wait);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_symc_crypto_len, ret);
            return ret;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_aead_gcm_crypto(void *ctx, hi_u32 operation, compat_addr input[], compat_addr output[],
                                   hi_u32 length[], symc_node_usage usage_list[], hi_u32 pkg_num, hi_u32 wait)
{
    hi_s32 ret;
    cryp_symc_context *hisi_ctx = ctx;
    hi_u8 clen[AES_BLOCK_SIZE_IN_BYTE] = { 0 };

    hi_log_func_enter();

    ret = cryp_symc_crypto_init(hisi_ctx, operation, input, output, length,
        usage_list, pkg_num, SYMC_NODE_USAGE_IN_GCM_P);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_crypto_init, ret);
        return ret;
    }

    ret = drv_aead_gcm_add_a(hisi_ctx->hard_chn, hisi_ctx->aad, hisi_ctx->alen); /* Compute A */
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_aead_gcm_add_a, ret);
        return ret;
    }

    if (hisi_ctx->enclen == 0) {
        cyp_aead_gcm_clen(clen, hisi_ctx->alen, 0x00);
        ret = drv_aead_gcm_add_clen(hisi_ctx->hard_chn, clen, sizeof(clen));
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_aead_gcm_add_clen, ret);
            return ret;
        }
        if (hisi_ctx->alen == 0) {
            drv_symc_add_buf_usage(hisi_ctx->hard_chn, HI_TRUE, SYMC_NODE_USAGE_GCM_FIRST);
        }
        ret = cryp_aead_crypto_zero(hisi_ctx, wait);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_aead_crypto_zero, ret);
            return ret;
        }
    } else {
        if (hisi_ctx->alen == 0) {
            usage_list[0] |= SYMC_NODE_USAGE_GCM_FIRST;
        }
        usage_list[0] |= SYMC_NODE_USAGE_FIRST;
        usage_list[pkg_num - 1] |= SYMC_NODE_USAGE_LAST;
        ret = cryp_symc_crypto_process(hisi_ctx, wait);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_symc_crypto_process, ret);
            return ret;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_aead_get_tag(void *ctx, hi_u32 tag[AEAD_TAG_SIZE_IN_WORD], hi_u32 *taglen)
{
    hi_s32 ret;
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();

    hi_log_check_param(*taglen < hisi_ctx->tlen);

    hi_log_debug("tag buffer len %d, tag len %d\n", *taglen, hisi_ctx->tlen);

    *taglen = hisi_ctx->tlen;

    ret = drv_aead_get_tag(hisi_ctx->hard_chn, tag);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_aead_get_tag, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

#ifdef SOFT_AES_CTS_SUPPORT
static hi_s32 cryp_aes_cbc_cts_encrypt(void *ctx, crypto_mem *src, crypto_mem *dest, hi_u32 length)
{
    hi_s32 ret;
    hi_u32 tail;
    hi_u32 total;
    hi_u8 cn[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    compat_addr input, output;
    symc_node_usage usage = SYMC_NODE_USAGE_NORMAL;

    hi_log_func_enter();

    tail = length % AES_BLOCK_SIZE_IN_BYTE;
    total = length + AES_BLOCK_SIZE_IN_BYTE - tail;

    crypto_mem_phys(src, &input);
    crypto_mem_phys(dest, &output);

    if (length < AES_BLOCK_SIZE_IN_BYTE) {
        /* just copy without encrypt */
        ret = memcpy_s(crypto_mem_virt(src), src->dma_size, crypto_mem_virt(dest), length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
        hi_log_func_exit();
        return HI_SUCCESS;
    }

    cryp_symc_setmode(ctx, SYMC_ALG_AES, SYMC_MODE_CBC, SYMC_DAT_WIDTH_128);

    if (tail == 0) {
        ret = cryp_symc_crypto(ctx,
                               SYMC_OPERATION_ENCRYPT,
                               &input,
                               &output,
                               &length,
                               &usage,
                               1,
                               HI_TRUE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_symc_crypto, ret);
            return ret;
        }

        hi_log_func_exit();
        return HI_SUCCESS;
    }

    ret = memset_s(crypto_mem_virt(src) + length, AES_BLOCK_SIZE_IN_BYTE, 0, AES_BLOCK_SIZE_IN_BYTE - tail);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    ret = cryp_symc_crypto(ctx,
                           SYMC_OPERATION_ENCRYPT,
                           &input,
                           &output,
                           &total,
                           &usage,
                           1,
                           HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_crypto, ret);
        return ret;
    }

    /* Swap the last two ciphertext blocks
     * 1. save the penult block as backup */
    ret = memcpy_s(cn,
                   AES_BLOCK_SIZE_IN_BYTE,
                   crypto_mem_virt(dest) + total - AES_BLOCK_SIZE_IN_BYTE * 2, /* max 2 block */
                   AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    /* 2. copy the last block to the penult block */
    ret = memcpy_s(crypto_mem_virt(dest) + total - AES_BLOCK_SIZE_IN_BYTE * 2, /* max 2 block */
                   AES_BLOCK_SIZE_IN_BYTE,
                   crypto_mem_virt(dest) + total - AES_BLOCK_SIZE_IN_BYTE,
                   AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    /* 3. copy the backup block to the last block */
    ret = memcpy_s(crypto_mem_virt(dest) + total - AES_BLOCK_SIZE_IN_BYTE,
                   AES_BLOCK_SIZE_IN_BYTE,
                   cn,
                   AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* ciphertext stealing(CTS) decryption using a standard CBC interface
 * 1.Dn = Decrypt (K, Cn_1). Decrypt the second to last ciphertext block.
 * 2.Cn = Cn || Tail (Dn, B-M).
     Pad the ciphertext to the nearest multiple of the block size
     using the last B-M bits of block cipher decryption of the second-to-last ciphertext block.
 * 3.Swap the last two ciphertext blocks.
 * 4.Decrypt the (modified) ciphertext using the standard CBC mode.
 * 5.Truncate the plaintext to the length of the original ciphertext.
 */
hi_s32 cryp_aes_cbc_cts_decrypt(void *ctx, crypto_mem *src, crypto_mem *dest, hi_u32 length)
{
    hi_s32 ret;
    hi_u32 tail;
    hi_u32 total;
    hi_u32 size;
    hi_u32 ivlen = 0;
    hi_u8 iv[AES_IV_SIZE] = { 0 };
    hi_u8 cn[AES_BLOCK_SIZE_IN_BYTE] = { 0 };
    compat_addr input, output;
    symc_node_usage usage = SYMC_NODE_USAGE_NORMAL;

    hi_log_func_enter();

    crypto_mem_phys(src, &input);
    crypto_mem_phys(dest, &output);

    if (length < AES_BLOCK_SIZE_IN_BYTE) {
        /* just copy without decrypt */
        ret = memcpy_s(crypto_mem_virt(dest), AES_BLOCK_SIZE_IN_BYTE, crypto_mem_virt(src), length);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
        hi_log_func_exit();
        return HI_SUCCESS;
    }

    cryp_symc_setmode(ctx, SYMC_ALG_AES, SYMC_MODE_CBC, SYMC_DAT_WIDTH_128);

    tail = length % AES_BLOCK_SIZE_IN_BYTE;
    total = length + AES_BLOCK_SIZE_IN_BYTE - tail;
    if (tail == 0) {
        return cryp_symc_crypto(ctx, SYMC_OPERATION_DECRYPT, &input, &output,
                                &total, &usage, 1, HI_TRUE);
    }

    /*******************************************
     * Gen the second-to-last ciphertext block
     * 1. Decrypt the last ciphertext block using ECB mode
     * 2. Copy the top of last ciphertext block to cn
     * 3. Copy the tail of last clean text block to cn
     * Cipher can't write data to non-secure ddr
     */

    ret = cryp_symc_getiv(ctx, iv, sizeof(iv), &ivlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_getiv, ret);
        return ret;
    }

    /* reset IV to zero */
    ret = memset_s(iv, sizeof(iv), 0, AES_IV_SIZE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    cryp_symc_setiv(ctx, cn, AES_IV_SIZE, CIPHER_IV_CHANGE_ONE_PKG);

    ADDR_L32(input) += total - AES_BLOCK_SIZE_IN_BYTE * 2; /* max 2 block */
    size = AES_BLOCK_SIZE_IN_BYTE;
    ret = cryp_symc_crypto(ctx, SYMC_OPERATION_DECRYPT, &input, &output, &size, &usage, 1, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_crypto, ret);
        return ret;
    }
    ADDR_L32(input) -= total - AES_BLOCK_SIZE_IN_BYTE * 2; /* max 2 block */
    ret = memcpy_s(cn, AES_BLOCK_SIZE_IN_BYTE, crypto_mem_virt(src) + total - AES_BLOCK_SIZE_IN_BYTE, tail);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }
    ret = memcpy_s(cn + tail,
                   AES_BLOCK_SIZE_IN_BYTE - tail,
                   crypto_mem_virt(dest) + tail,
                   AES_BLOCK_SIZE_IN_BYTE - tail);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    /**************************************
     * Swap the last 2 block
     * second-to-last block -> last block
     * cn -> second-to-last block
     */
    ret = memcpy_s(crypto_mem_virt(src) + total - AES_BLOCK_SIZE_IN_BYTE, AES_BLOCK_SIZE_IN_BYTE,
                   crypto_mem_virt(src) + total - AES_BLOCK_SIZE_IN_BYTE * 2, /* max 2 block */
                   AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }
    ret = memcpy_s(crypto_mem_virt(src) + total - AES_BLOCK_SIZE_IN_BYTE * 2, /* max 2 block */
        AES_BLOCK_SIZE_IN_BYTE, cn, AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    /* Decrypt the ciphertext using CBC mode . */
    /* recovery IV */
    cryp_symc_setiv(ctx, iv, AES_IV_SIZE, CIPHER_IV_CHANGE_ONE_PKG);

    ret = cryp_symc_crypto(ctx, SYMC_OPERATION_DECRYPT, &input, &output, &total, &usage, 1, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_symc_crypto, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 cryp_aes_cbc_cts_crypto(void *ctx,
                               hi_u32 operation,
                               compat_addr input[],
                               compat_addr output[],
                               hi_u32 length[],
                               symc_node_usage usage_list[],
                               hi_u32 pkg_num,
                               hi_u32 last)
{
    crypto_mem src;
    crypto_mem dest;
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_param(pkg_num != 0x01);

    if (ADDR_L32(input[0]) == ADDR_L32(output[0])) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_ADDR);
        return HI_ERR_CIPHER_INVALID_ADDR;
    }

    ret = crypto_mem_open(&src, input[0], (length[0] + AES_BLOCK_SIZE_IN_BYTE - 1) & (~0x0F));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_mem_open, ret);
        goto exit0;
    }

    ret = crypto_mem_open(&dest, output[0], (length[0] + AES_BLOCK_SIZE_IN_BYTE - 1) & (~0x0F));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_mem_open, ret);
        goto exit1;
    }

    if (operation == SYMC_OPERATION_DECRYPT) {
        ret = cryp_aes_cbc_cts_decrypt(ctx, &src, &dest, length[0]);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_aes_cbc_cts_decrypt, ret);
        }
    } else {
        ret = cryp_aes_cbc_cts_encrypt(ctx, &src, &dest, length[0]);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_aes_cbc_cts_encrypt, ret);
        }
    }

    crypto_mem_close(&src);
exit1:
    crypto_mem_close(&dest);
exit0:

    if (ret == HI_SUCCESS) {
        hi_log_func_exit();
    }

    return ret;
}
#endif

static hi_s32 cryp_register_symc(symc_func *func)
{
    hi_s32 ret;
    hi_u32 i = 0;

    hi_log_func_enter();

    /* check availability */
    if ((func->create == HI_NULL)
        || (func->crypto == HI_NULL)) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    /* is it already registered? */
    for (i = 0; i < SYMC_FUNC_TAB_SIZE; i++) {
        if (g_symc_descriptor[i].valid
            && g_symc_descriptor[i].alg == func->alg
            && g_symc_descriptor[i].mode == func->mode) {
            hi_log_func_exit();
            return HI_SUCCESS;
        }
    }

    /* find a blank spot */
    for (i = 0; i < SYMC_FUNC_TAB_SIZE; i++) {
        if (!g_symc_descriptor[i].valid) {
            ret = memcpy_s(&g_symc_descriptor[i], sizeof(symc_func), func, sizeof(symc_func));
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(memcpy_s, ret);
                return ret;
            }
            g_symc_descriptor[i].valid = HI_TRUE;
            hi_log_debug("g_symc_descriptor[%d], alg %d, mode %d\n", i,
                         g_symc_descriptor[i].alg, g_symc_descriptor[i].mode);

            hi_log_func_exit();
            return HI_SUCCESS;
        }
    }

    /* Can't find a blank spot */
    hi_log_print_err_code(HI_ERR_CIPHER_OVERFLOW);
    return HI_ERR_CIPHER_OVERFLOW;
}

static symc_func *cryp_get_symc(hi_u32 alg, hi_u32 mode)
{
    hi_u32 i = 0;
    symc_func *template = HI_NULL;

    hi_log_func_enter();

    /* find the valid function */
    for (i = 0; i < SYMC_FUNC_TAB_SIZE; i++) {
        hi_log_debug("g_symc_descriptor[%d] valid %d, alg %d, mode %d \n",
                     i, g_symc_descriptor[i].valid, g_symc_descriptor[i].alg, g_symc_descriptor[i].mode);

        if (g_symc_descriptor[i].valid) {
            if (g_symc_descriptor[i].alg == alg
                && g_symc_descriptor[i].mode == mode) {
                template = &g_symc_descriptor[i];
                break;
            }
        }
    }

    hi_log_func_exit();
    return template;
}

/* Defined the default template of Symmetric cipher function,
 * the function can be replaced by other engine
 */
static hi_s32 cryp_aes_setkey(void *ctx, hi_u32 *hisi_klen)
{
    hi_u32 klen = 0;
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();
    hi_log_check_param(ctx == HI_NULL);

    /* set the key length depend on alg
     des/3des support 2key and 3key
     aes support 128, 192, and 256
     sm1 support ak/ek/sk
     sm4 support 128 */

    switch (*hisi_klen) {
        case HI_CIPHER_KEY_AES_128BIT: {
            klen = AES_KEY_128BIT;
            break;
        }
        case HI_CIPHER_KEY_AES_192BIT: {
            klen = AES_KEY_192BIT;
            break;
        }
        case HI_CIPHER_KEY_AES_256BIT: {
            klen = AES_KEY_256BIT;
            break;
        }
        default: {
            hi_log_error("aes with invalid keylen.\n");
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }
    hi_log_debug("key len %d, type %d\n", klen, *hisi_klen);

    cryp_symc_setkey(ctx, klen, HI_FALSE);

    hisi_ctx->hard_key = HI_TRUE;
    *hisi_klen = klen;

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cryp_tdes_setkey(void *ctx, hi_u32 *hisi_klen)
{
    hi_u32 klen = 0;
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();
    hi_log_check_param(ctx == HI_NULL);

    /* set the key length depend on alg
     des/3des support 2key and 3key
     aes support 128, 192, and 256
     sm1 support ak/ek/sk
     sm4 support 128 */

    switch (*hisi_klen) {
        case HI_CIPHER_KEY_DES_2KEY: {
            klen = TDES_KEY_128BIT;
            break;
        }
        case HI_CIPHER_KEY_DES_3KEY: {
            klen = TDES_KEY_192BIT;
            break;
        }
        default: {
            hi_log_error("3des with invalid keylen, keylen = 0x%x.\n", *hisi_klen);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    cryp_symc_setkey(ctx, klen, HI_FALSE);

    hisi_ctx->hard_key = HI_TRUE;
    *hisi_klen = klen;

    hi_log_func_exit();
    return HI_SUCCESS;
}

#ifdef HI_PRODUCT_SM1_SM4_SUPPORT
static hi_s32 cryp_sm4_setkey(void *ctx, hi_u32 *hisi_klen)
{
    cryp_symc_context *hisi_ctx = ctx;

    hi_log_func_enter();
    hi_log_check_param(ctx == HI_NULL);
    hi_log_check_param(*hisi_klen != HI_CIPHER_KEY_DEFAULT);

    /* sm4 support 128 */
    cryp_symc_setkey(ctx, SM4_KEY_SIZE, HI_FALSE);

    hisi_ctx->hard_key = HI_TRUE;
    *hisi_klen = SM4_KEY_SIZE;

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

static hi_s32 cryp_symc_setiv_default(void *ctx, const hi_u8 *iv, hi_u32 ivlen, hi_u32 usage)
{
    hi_log_func_enter();

    if (iv == HI_NULL) {
        return HI_SUCCESS;
    }

    if (ivlen > AES_IV_SIZE) {
        return HI_FAILURE;
    }

    cryp_symc_setiv(ctx, iv, ivlen, usage);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* Default As AES */
static void cryp_register_symc_default(symc_func *func, symc_alg alg, symc_mode mode)
{
    hi_s32 ret;

    ret = memset_s(func, sizeof(symc_func), 0, sizeof(symc_func));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    func->mode = mode;
    func->alg = alg;
    func->create = cryp_symc_create;
    func->setiv = cryp_symc_setiv_default;
    func->getiv = cryp_symc_getiv;
    func->crypto = cryp_symc_crypto;
    func->setmode = cryp_symc_setmode;
    func->setkey = cryp_aes_setkey;
    func->waitdone = cryp_symc_wait_done;
    return;
}

static void cryp_register_symc_aes(hi_u32 capacity, symc_mode mode)
{
    symc_func func;
    hi_s32 ret;

    ret = memset_s(&func, sizeof(symc_func), 0, sizeof(symc_func));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    if (capacity == CRYPTO_CAPACITY_SUPPORT) {
        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        ret = cryp_register_symc(&func);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_register_symc, ret);
            return;
        }
    }

    return;
}

static void cryp_register_symc_dma(hi_u32 dma_capacity, hi_u32 tdes_capacity)
{
    symc_func func;
    hi_s32 ret;

    ret = memset_s(&func, sizeof(func), 0, sizeof(func));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    if (dma_capacity == CRYPTO_CAPACITY_SUPPORT) {
        func.mode = SYMC_MODE_ECB;
        func.alg = SYMC_ALG_NULL_CIPHER;
        func.create = cryp_symc_create;
        func.setmode = cryp_symc_setmode;
        func.crypto = cryp_symc_crypto;
        ret = cryp_register_symc(&func);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_register_symc, ret);
            return;
        }
    }

    return;
}

static void cryp_register_symc_aes_cts(hi_u32 capacity, symc_mode mode)
{
#ifdef SOFT_AES_CTS_SUPPORT
    hi_s32 ret;

    hi_log_debug("CTS crypto capacity %d, mode %d\n", capacity, mode);

    if (capacity != CRYPTO_CAPACITY_SUPPORT) {
        symc_func func;

        ret = memset_s(&func, sizeof(func), 0, sizeof(func));
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memset_s, ret);
            return;
        }

        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        func.crypto = cryp_aes_cbc_cts_crypto;
        func.waitdone = HI_NULL;
        hi_log_debug("CTS crypto 0x%p, mode %d\n", func.crypto, mode);
        ret = cryp_register_symc(&func);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_register_symc, ret);
            return;
        }
    }
#endif
    return;
}

#ifdef HI_PRODUCT_AEAD_SUPPORT
static void cryp_register_aead_ccm(hi_u32 capacity, symc_mode mode)
{
    if (capacity == CRYPTO_CAPACITY_SUPPORT) {
#ifdef CHIP_AES_CCM_GCM_SUPPORT
        symc_func func;
        hi_s32 ret;

        ret = memset_s(&func, sizeof(func), 0, sizeof(func));
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memset_s, ret);
            return;
        }

        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        func.setadd = cryp_aead_ccm_set_aad;
        func.gettag = cryp_aead_get_tag;
        func.crypto = cryp_aead_ccm_crypto;
        func.setiv = cryp_aead_ccm_setiv;
        ret = cryp_register_symc(&func);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_register_symc, ret);
            return;
        }
#endif
    }

    return;
}

static void cryp_register_aead_gcm(hi_u32 capacity, symc_mode mode)
{
    if (capacity == CRYPTO_CAPACITY_SUPPORT) {
#ifdef CHIP_AES_CCM_GCM_SUPPORT
        symc_func func;
        hi_s32 ret;

        ret = memset_s(&func, sizeof(symc_func), 0, sizeof(symc_func));
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memset_s, ret);
            return;
        }

        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        func.setadd = cryp_aead_gcm_set_aad;
        func.gettag = cryp_aead_get_tag;
        func.crypto = cryp_aead_gcm_crypto;
        func.setiv = cryp_aead_gcm_setiv;
        ret = cryp_register_symc(&func);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_register_symc, ret);
            return;
        }
#endif
    }

    return;
}
#endif

static void cryp_register_symc_tdes(hi_u32 capacity, symc_mode mode)
{
    symc_func func;
    hi_s32 ret;

    ret = memset_s(&func, sizeof(symc_func), 0, sizeof(symc_func));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    if (capacity == CRYPTO_CAPACITY_SUPPORT) {
        cryp_register_symc_default(&func, SYMC_ALG_TDES, mode);
        func.setkey = cryp_tdes_setkey;
        ret = cryp_register_symc(&func);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_register_symc, ret);
            return;
        }
    }

    return;
}

#ifdef HI_PRODUCT_SM1_SM4_SUPPORT
static void cryp_register_symc_sm4(hi_u32 capacity, symc_mode mode)
{
    symc_func func;
    hi_s32 ret;

    ret = memset_s(&func, sizeof(symc_func), 0, sizeof(symc_func));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    if (capacity == CRYPTO_CAPACITY_SUPPORT) {
        cryp_register_symc_default(&func, SYMC_ALG_SM4, mode);
        func.setkey = cryp_sm4_setkey;
        ret = cryp_register_symc(&func);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cryp_register_symc, ret);
            return;
        }
    }

    return;
}
#endif

/* symc function register */
static void cryp_register_all_symc(void)
{
    hi_s32 ret;
    symc_capacity capacity;

    hi_log_func_enter();

    ret = memset_s(&capacity, sizeof(capacity), 0, sizeof(capacity));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    /* register the symc function if supported */
    drv_symc_get_capacity(&capacity);

    /* AES */
#ifndef HI_CIPHER_MODE_ECB_DISABLE
    cryp_register_symc_aes(capacity.aes_ecb, SYMC_MODE_ECB);
#endif
    cryp_register_symc_aes(capacity.aes_cbc, SYMC_MODE_CBC);
    cryp_register_symc_aes(capacity.aes_cfb, SYMC_MODE_CFB);
    cryp_register_symc_aes(capacity.aes_ofb, SYMC_MODE_OFB);
    cryp_register_symc_aes(capacity.aes_ctr, SYMC_MODE_CTR);
    cryp_register_symc_dma(capacity.dma, capacity.tdes_ecb);
    cryp_register_symc_aes_cts(capacity.aes_cts, SYMC_MODE_CTS);

#ifdef HI_PRODUCT_AEAD_SUPPORT
    /* AEAD */
    cryp_register_aead_ccm(capacity.aes_ccm, SYMC_MODE_CCM);
    cryp_register_aead_gcm(capacity.aes_gcm, SYMC_MODE_GCM);
#endif

    /* TDES */
#ifndef HI_CIPHER_MODE_ECB_DISABLE
    cryp_register_symc_tdes(capacity.tdes_ecb, SYMC_MODE_ECB);
#endif
    cryp_register_symc_tdes(capacity.tdes_cbc, SYMC_MODE_CBC);
    cryp_register_symc_tdes(capacity.tdes_cfb, SYMC_MODE_CFB);
    cryp_register_symc_tdes(capacity.tdes_ofb, SYMC_MODE_OFB);
    cryp_register_symc_tdes(capacity.tdes_ctr, SYMC_MODE_CTR);

#ifdef HI_PRODUCT_SM1_SM4_SUPPORT
    /* SM4 */
#ifndef HI_CIPHER_MODE_ECB_DISABLE
    cryp_register_symc_sm4(capacity.sm4_ecb, SYMC_MODE_ECB);
#endif
    cryp_register_symc_sm4(capacity.sm4_cbc, SYMC_MODE_CBC);
    cryp_register_symc_sm4(capacity.sm4_cfb, SYMC_MODE_CFB);
    cryp_register_symc_sm4(capacity.sm4_ofb, SYMC_MODE_OFB);
    cryp_register_symc_sm4(capacity.sm4_ctr, SYMC_MODE_CTR);
#endif

    hi_log_func_exit();

    return;
}

symc_func *cryp_get_symc_op(hi_cipher_alg alg, hi_cipher_work_mode mode)
{
    hi_u32 cryp_mode = 0;
    symc_func *func = HI_NULL;
    symc_alg cryp_alg;

    hi_log_func_enter();

    switch (alg) {
        case HI_CIPHER_ALG_3DES:
            cryp_alg = SYMC_ALG_TDES;
            break;
        case HI_CIPHER_ALG_AES:
            cryp_alg = SYMC_ALG_AES;
            break;
#ifdef HI_PRODUCT_SM1_SM4_SUPPORT
        case HI_CIPHER_ALG_SM4:
            cryp_alg = SYMC_ALG_SM4;
            break;
#endif
#ifdef HI_PRODUCT_DMA_SUPPORT
        case HI_CIPHER_ALG_DMA:
            cryp_alg = SYMC_ALG_NULL_CIPHER;
            mode = HI_CIPHER_WORK_MODE_ECB;
            break;
#endif
        default:
            hi_log_error("Invalid alg, alg = 0x%x.\n", alg);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_NULL;
    }

    switch (mode) {
        case HI_CIPHER_WORK_MODE_ECB:
            cryp_mode = SYMC_MODE_ECB;
            break;
        case HI_CIPHER_WORK_MODE_CBC:
            cryp_mode = SYMC_MODE_CBC;
            break;
        case HI_CIPHER_WORK_MODE_CFB:
            cryp_mode = SYMC_MODE_CFB;
            break;
        case HI_CIPHER_WORK_MODE_OFB:
            cryp_mode = SYMC_MODE_OFB;
            break;
        case HI_CIPHER_WORK_MODE_CTR:
            cryp_mode = SYMC_MODE_CTR;
            break;
#ifdef HI_PRODUCT_AEAD_SUPPORT
        case HI_CIPHER_WORK_MODE_CCM:
            cryp_mode = SYMC_MODE_CCM;
            break;
        case HI_CIPHER_WORK_MODE_GCM:
            cryp_mode = SYMC_MODE_GCM;
            break;
#endif
#ifdef SOFT_AES_CTS_SUPPORT
        case HI_CIPHER_WORK_MODE_CBC_CTS:
            cryp_mode = SYMC_MODE_CTS;
            break;
#endif
        default:
            hi_log_error("Invalid mode, mode = 0x%x.\n", mode);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_NULL;
    }

    func = cryp_get_symc(cryp_alg, cryp_mode);

    hi_log_func_exit();

    return func;
}

/* proc function begin */
#ifdef HI_PROC_SUPPORT
void cryp_get_symc_alg(hi_u8 *alg, hi_u8 *mode, hi_u32 *count)
{
    hi_u32 i = 0;
    hi_u32 id = 0;

    hi_log_func_enter();

    if (*count < SYMC_FUNC_TAB_SIZE) {
        *count = 0;
        hi_log_warn("symc-alg-mode buffer too samll!\n");
        return;
    }

    /* find the valid function */
    for (i = 0; i < SYMC_FUNC_TAB_SIZE; i++) {
        if (g_symc_descriptor[i].valid) {
            alg[id] = (hi_u8)g_symc_descriptor[i].alg;
            mode[id] = (hi_u8)g_symc_descriptor[i].mode;
            id++;
        }
    }

    *count = id;

    hi_log_func_exit();
    return;
}
#endif

