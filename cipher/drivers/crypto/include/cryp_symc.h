/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_symc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef CRYP_SYMC_H_
#define CRYP_SYMC_H_

#include "drv_osal_lib.h"
#include "drv_symc.h"

/******************************* API Declaration *****************************/
/** \addtogroup      aes */
/** @{ */ /** <!--[aes] */

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

/**
* \brief          Initialize crypto of symc *
 */
hi_s32 cryp_symc_init(void);

/**
* \brief          Deinitialize crypto of symc *
 */
void cryp_symc_deinit(void);

/**
 * \brief          Create symc handle
 *
 * \param handle   symc handle to be initialized
 * \param chn      symc channel
 */
typedef void *(*func_symc_create)(hi_u32 hard_chn);

/**
 * \brief          Clear symc context
 *
 * \param handle      symc handle to be destory
 */
typedef hi_s32 (*func_symc_destroy)(const void *ctx);

/**
 * \brief          symc key schedule
 *
 * \param handle   SYMC handle
 * \param[in]  fkey first  key buffer, defualt
 * \param[in]  skey second key buffer, expand
 * \param hisi_klen input key type, output key length in bytes
 *
 * \return         0 if successful, or HI_SYMC_ERR_KEY_LEN_INVALID
 */
typedef hi_s32 (*func_symc_setkey)(void *ctx, hi_u32 *hisi_klen);

/**
 * \brief          Symc iv schedule
 *
 * \param handle   symc handle
 * \param IV       Symc IV
 * \param ivlen    length of iv
 *
 * \return         0 if successful.
 */
typedef hi_s32 (*func_symc_setiv)(void *ctx, const hi_u8 *iv, hi_u32 ivlen, hi_u32 usage);

/**
 * \brief          Symc iv schedule
 *
 * \param handle   symc handle
 * \param IV       Symc IV
 * \param ivlen    must be 128, 192 or 256
 *
 * \return         0 if successful.
 */
typedef hi_s32 (*func_symc_getiv)(void *ctx, hi_u8 *iv, hi_u32 iv_buf_len, hi_u32 *ivlen);

/**
 * \brief          SYMC alg and mode
 *
 * \param handle   SYMC handle
 * \param alg      Symmetric cipher alg
 * \param mode     Symmetric cipher mode
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful.
 */
typedef void (*func_symc_setmode)(void *ctx, symc_alg alg, symc_mode mode, symc_width width);

/**
 * \brief          SYMC wait done
 *
 * \param ctx      SYMC handle
 * \return         0 if successful.
 */
typedef hi_s32 (*func_symc_wait_done)(void *ctx, hi_u32 timeout);

/**
 * \brief          SYMC alg and mode
 *
 * \param handle   SYMC handle
 * \param round    SM1 round number
 *
 * \return         0 if successful.
 */
typedef hi_s32 (*func_symc_sm1_setround)(void *ctx, hi_u32 round);

/**
 * \brief          symc  buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param handle   symc handle
 * \param operation  decrypt or encrypt
 * \param mode     mode
 * \param length   length of the input data
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \param usage_list usage of buffer
 * \param pkg_num  numbers of buffer
 * \param last     last or not
 *
 * \return         0 if successful
 */
typedef hi_s32 (*func_symc_crypto)(void *ctx, hi_u32 operation,
                                   compat_addr input[],
                                   compat_addr output[],
                                   hi_u32 length[],
                                   symc_node_usage usage_list[],
                                   hi_u32 pkg_num,
                                   hi_u32 wait);

/**
 * \brief          CCM/GCM set Associated Data
 *
 * \param ctx      SYMC handle
 * \param aad      Associated Data
 * \param alen     Associated Data Length
 * \param tlen     Tag length
 *
 * \return         0 if successful.
 */
typedef hi_s32 (*func_aead_set_aad)(void *ctx, compat_addr aad, hi_u32 alen, hi_u32 tlen);

/**
 * \brief          SYMC multiple buffer encryption/decryption.
 * \param[in]  id The channel number.
 * \param[in]  tag tag data of CCM/GCM
 * \param uuid uuid The user identification.
 *
 * \return         0 if successful
 */
typedef hi_s32 (*func_aead_get_tag)(void *ctx, hi_u32 tag[AEAD_TAG_SIZE_IN_WORD], hi_u32 *taglen);

/* ! \struct of Symmetric cipher function template */
typedef struct {
    hi_u32 valid;                    /* !<  vliad or not */
    symc_alg alg;                    /* !<  Alg of Symmetric cipher */
    symc_mode mode;                  /* !<  Mode of Symmetric cipher */
    func_symc_setmode setmode;       /* !<  Set mode function */
    func_symc_sm1_setround setround; /* !<  SM1 set round function */
    func_symc_create create;         /* !<  Create function */
    func_symc_destroy destroy;       /* !<  destroy function */
    func_symc_setkey setkey;         /* !<  setkey function */
    func_symc_setiv setiv;           /* !<  setiv function */
    func_symc_getiv getiv;           /* !<  getiv function */
    func_aead_set_aad setadd;        /* !<  setadd function */
    func_aead_get_tag gettag;        /* !<  get tag function */
    func_symc_crypto crypto;         /* !<  crypto function */
    func_symc_wait_done waitdone;    /* !<  wait done */
} symc_func;

/**
\brief  symc alloc channel.
\param[out]  hard_chn symc channel.
\retval     On success, func is returned.  On error, HI_NULL is returned.
 */
hi_s32 cryp_symc_alloc_chn(hi_u32 *hard_chn, hi_cipher_type type);

/**
\brief  symc free channel.
\param[in]  hard_chn symc channel.
\retval     On success, func is returned.  On error, HI_NULL is returned.
 */
void cryp_symc_free_chn(hi_u32 hard_chn);

/**
\brief  set the key source to the symc module.
\param[in]  ctx SYMC handle.
\param[in]  src The key source.
\retval     NA.
 */
void cryp_symc_set_key_source(void *ctx, symc_key_source src);

/**
\brief  Clone the function from template of aes engine.
\param[in]  alg The alg of Symmetric cipher.
\param[in]  mode The work mode.
\retval     On success, func is returned.  On error, HI_NULL is returned.
 */
symc_func *cryp_get_symc_op(hi_cipher_alg alg, hi_cipher_work_mode mode);

/**
\brief  get the alg and mode that support by symc.
\param[out]  alg The alg of Symmetric cipher.
\param[out]  mode The work mode.
\param[out]  count The count.
\retval     NA.
 */
void cryp_get_symc_alg(hi_u8 *alg, hi_u8 *mode, hi_u32 *count);

hi_u32 cyp_aead_gcm_clen(hi_u8 *buf, hi_u32 alen, hi_u32 enclen);

/** @} */ /** <!-- ==== API declaration end ==== */

#endif
