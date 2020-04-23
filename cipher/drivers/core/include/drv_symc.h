/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_symc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __DRV_CIPHER_H__
#define __DRV_CIPHER_H__

#include "hi_drv_cipher.h"
/*************************** Structure Definition ****************************/
/** \addtogroup     symmetric cipher */
/** @{ */ /** <!-- [symc] */

/* ! \symmetric cipher max key size in words */
#define SYMC_KEY_MAX_SIZE_IN_WORD (8)

/* ! \symmetric cipher max iv size in word */
#define SYMC_IV_MAX_SIZE_IN_WORD (4)

/* ! \symmetric sm1 sk size in words */
#define SYMC_SM1_SK_SIZE_IN_WORD (4)

/* ! \DES BLOCK size */
#define DES_BLOCK_SIZE (8)

/* ! \Numbers of nodes list */
#define SYMC_MAX_LIST_NUM (255)

/* ! \Length of CCM N */
#define SYMC_CCM_N_LEN (16)

/* ! \Length of CCM A head */
#define SYMC_CCM_A_HEAD_LEN (16)

/* ! \Length of GCM CLEN */
#define SYMC_GCM_CLEN_LEN (16)

/* ! \Small length of CCM A */
#define SYMC_CCM_A_SMALL_LEN (0x10000 - 0x100)

/* ! \AES KEY size 128bit */
#define AES_KEY_128BIT (16)

/* ! \AES KEY size 192bit */
#define AES_KEY_192BIT (24)

/* ! \AES KEY size 256bit */
#define AES_KEY_256BIT (32)

/* ! \DES KEY size 128bit */
#define DES_KEY_SIZE (8)

/* ! \TDES KEY size 128bit */
#define TDES_KEY_128BIT (16)

/* ! \TDES KEY size 192bit */
#define TDES_KEY_192BIT (24)

/* ! \DES block size */
#define DES_BLOCK_SIZE (8)

/* ! \SM1 KEY size */
#define SM1_AK_EK_SIZE (32)
#define SM1_SK_SIZE    (16)

/* ! \SM4 KEY size */
#define SM4_KEY_SIZE (16)

/* ! \symmetric cipher key source */
typedef enum {
    SYMC_KEY_SRC_SLOT0 = 0,
    SYMC_KEY_SRC_SLOT1,
    SYMC_KEY_SRC_SLOT2,
    SYMC_KEY_SRC_SLOT3,
    SYMC_KEY_SRC_SLOT4,
    SYMC_KEY_SRC_SLOT5,
    SYMC_KEY_SRC_SLOT6,
    SYMC_KEY_SRC_SLOT7,
    SYMC_KEY_SRC_SLOT8,
    SYMC_KEY_SRC_SLOT9,
    SYMC_KEY_SRC_SLOT10,
    SYMC_KEY_SRC_SLOT11,
    SYMC_KEY_SRC_SLOT12,
    SYMC_KEY_SRC_SLOT13,
    SYMC_KEY_SRC_SLOT14,
    SYMC_KEY_SRC_SLOT15,
    SYMC_KEY_SRC_COUNT,
} symc_key_source;

/* ! \symmetric cipher width */
typedef enum {
    SYMC_DAT_WIDTH_64 = 0,
    SYMC_DAT_WIDTH_128 = 0,
    SYMC_DAT_WIDTH_8,
    SYMC_DAT_WIDTH_1,
    SYMC_DAT_WIDTH_COUNT,
} symc_width;

/* ! \symmetric cipher algorithm */
typedef enum {
    SYMC_ALG_DES,         /* !<  Data Encryption Standard */
    SYMC_ALG_TDES,        /* !<  Triple Data Encryption Standard */
    SYMC_ALG_AES,         /* !<  Advanced Encryption Standard */
    SYMC_ALG_SM4,         /* !<  SM4 Algorithm */
    SYMC_ALG_SM1,         /* !<  SM1 Algorithm */
    SYMC_ALG_NULL_CIPHER, /* !<  null cipher, dma copy */
    SYMC_ALG_COUNT,
} symc_alg;

/* ! \symmetric cipher key length */
typedef enum {
    SYMC_KEY_DEFAULT = 0, /* !<  Default, aes-128, 3des-192, sm1-256, sm4-128 */
    SYMC_KEY_AES_192BIT,  /* !<  AES 192 bit key */
    SYMC_KEY_AES_256BIT,  /* !<  AES 256 bit key */
    SYMC_KEY_TDES_2KEY,   /* !<  3DES 128 bit key */
    SYMC_KEY_LEN_COUNT,
} symc_klen;

/* ! \symmetric cipher mode */
typedef enum {
    SYMC_MODE_ECB = 0, /* !<  Electronic Codebook Mode */
    SYMC_MODE_CBC,     /* !<  Cipher Block Chaining */
    SYMC_MODE_CFB,     /* !<  Cipher Feedback Mode */
    SYMC_MODE_OFB,     /* !<  Output Feedback Mode */
    SYMC_MODE_CTR,     /* !<  Counter Mode */
    SYMC_MODE_CCM,     /* !<  Counter with Cipher Block Chaining-Message Authentication Code */
    SYMC_MODE_GCM,     /* !<  Galois/Counter Mode */
    SYMC_MODE_CTS,     /* !<  CTS Mode */
    SYMC_MODE_COUNT,
} symc_mode;

/* ! \locational of buffer under symmetric cipher */
typedef enum {
    SYMC_NODE_USAGE_NORMAL = 0x00,       /* !<  The normal buffer, don't update the iv */
    SYMC_NODE_USAGE_FIRST = 0x01,        /* !<  The first buffer, the usage of iv is expired */
    SYMC_NODE_USAGE_LAST = 0x02,         /* !<  The last buffer, must update the iv */
    SYMC_NODE_USAGE_ODD_KEY = 0x40,      /* !<  Use the odd key to encrypt/decrypt this buffer */
    SYMC_NODE_USAGE_EVEN_KEY = 0x00,     /* !<  Use the even key to encrypt/decrypt this buffer */
    SYMC_NODE_USAGE_IN_GCM_A = 0x00,     /* !<  The buffer of GCM A */
    SYMC_NODE_USAGE_IN_GCM_P = 0x08,     /* !<  The buffer of GCM P */
    SYMC_NODE_USAGE_IN_GCM_LEN = 0x10,   /* !<  The buffer of GCM LEN */
    SYMC_NODE_USAGE_IN_CCM_N = 0x00,     /* !<  The buffer of CCM N */
    SYMC_NODE_USAGE_IN_CCM_A = 0x08,     /* !<  The buffer of CCM A */
    SYMC_NODE_USAGE_IN_CCM_P = 0x10,     /* !<  The buffer of CCM P */
    SYMC_NODE_USAGE_CCM_LAST = 0x20,     /* !<  The buffer of CCM LAST */
    SYMC_NODE_USAGE_GCM_FIRST = 0x10000, /* !<  The buffer of GCM FIRST */
} symc_node_usage;

typedef struct {
    hi_u32 id;                     /* channel number */
    hi_u8 open;                    /* Open or closed */
    hi_u8 alg;                     /* algorithm */
    hi_u8 mode;                    /* work mode */
    hi_u8 klen;                    /* Length of key */
    hi_u8 ksrc;                    /* Hard or soft key */
    hi_u8 decrypt;                 /* Decrypt or Encrypt */
    hi_u32 inaddr;                 /* In DDR address */
    hi_u32 outaddr;                /* Out DDR address */
    hi_u8 int_status;              /* Status interrupt */
    hi_u8 int_en;                  /* Enable interrupt */
    hi_u8 int_raw;                 /* Raw interrupt */
    hi_u8 secure;                  /* Secure channel or not */
    hi_u8 smmu_enable;             /* SMMU Enable */
    hi_u32 in_node_head;           /* In node list head */
    hi_u8 in_node_depth;           /* In node depth */
    hi_u8 in_node_rptr;            /* In node list read index */
    hi_u8 in_node_wptr;            /* In node list write index */
    hi_u32 out_node_head;          /* Out node list head */
    hi_u8 out_node_depth;          /* Out node depth */
    hi_u8 out_node_rptr;           /* Out node list read index */
    hi_u8 out_node_wptr;           /* Out node list write index */
    hi_u8 in_last_block;           /* first word of last input block */
    crypto_owner owner;            /* Process PID of owner */
    hi_u8 iv[AES_IV_SIZE * 2 + 1]; /* Out IV */
} symc_chn_status;

typedef struct {
    hi_u8 sec_cpu;     /* secure CPU or not */
    hi_u8 allow_reset; /* allow reset CPU or not */
    hi_u32 int_num;    /* interrupt number */
    const char *name;  /* interrupt name */
    hi_u8 int_en;      /* interrupt enable */
    hi_u32 smmu_base;  /* smmu base address */
    hi_u32 err_code;   /* error code */
} symc_module_status;

/* ! \symc capacity, 0-nonsupport, 1-support */
typedef struct {
    hi_u32 aes_ecb : 1;  /* !<  Support AES ECB */
    hi_u32 aes_cbc : 1;  /* !<  Support AES CBC */
    hi_u32 aes_cfb : 1;  /* !<  Support AES CFB */
    hi_u32 aes_ofb : 1;  /* !<  Support AES OFB */
    hi_u32 aes_ctr : 1;  /* !<  Support AES CTR */
    hi_u32 aes_ccm : 1;  /* !<  Support AES CCM */
    hi_u32 aes_gcm : 1;  /* !<  Support AES GCM */
    hi_u32 aes_cts : 1;  /* !<  Support AES CTS */
    hi_u32 tdes_ecb : 1; /* !<  Support TDES ECB */
    hi_u32 tdes_cbc : 1; /* !<  Support TDES CBC */
    hi_u32 tdes_cfb : 1; /* !<  Support TDES CFB */
    hi_u32 tdes_ofb : 1; /* !<  Support TDES OFB */
    hi_u32 tdes_ctr : 1; /* !<  Support TDES CTR */
    hi_u32 des_ecb : 1;  /* !<  Support DES ECB */
    hi_u32 des_cbc : 1;  /* !<  Support DES CBC */
    hi_u32 des_cfb : 1;  /* !<  Support DES CFB */
    hi_u32 des_ofb : 1;  /* !<  Support DES OFB */
    hi_u32 des_ctr : 1;  /* !<  Support DES CTR */
    hi_u32 sm1_ecb : 1;  /* !<  Support SM1 ECB */
    hi_u32 sm1_cbc : 1;  /* !<  Support SM1 CBC */
    hi_u32 sm1_cfb : 1;  /* !<  Support SM1 CFB */
    hi_u32 sm1_ofb : 1;  /* !<  Support SM1 OFB */
    hi_u32 sm1_ctr : 1;  /* !<  Support SM1 CTR */
    hi_u32 sm4_ecb : 1;  /* !<  Support SM4 ECB */
    hi_u32 sm4_cbc : 1;  /* !<  Support SM4 CBC */
    hi_u32 sm4_cfb : 1;  /* !<  Support SM4 CFB */
    hi_u32 sm4_ofb : 1;  /* !<  Support SM4 OFB */
    hi_u32 sm4_ctr : 1;  /* !<  Support SM4 CTR */
    hi_u32 dma : 1;      /* !<  Support DMA */
} symc_capacity;

typedef hi_s32 (*callback_symc_isr)(void *ctx);
typedef void (*callback_symc_destory)(void);

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      symc */
/** @{ */ /** <!--[symc] */

/**
\brief  Initialize the symc module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_init(void);

/**
\brief  Deinitialize the symc module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_deinit(void);

/**
\brief  suspend the symc module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
void drv_symc_suspend(void);

/**
\brief  resume the symc module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_resume(void);

/**
\brief  allocate a hard symc channel.
\param[out]  chn_num The channel number.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_alloc_chn(hi_u32 *chn_num, hi_cipher_type type);

/**
\brief  clear a hard symc channel.
\param[in]  chn_num The channel number.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_clear_entry(hi_u32 chn_num);

/**
\brief  free a hard symc channel.
\param[in]  chn_num The channel number.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
void drv_symc_free_chn(hi_u32 chn_num);

/**
\brief  set the iv to the symc module.
\param[in]  chn_num The channel number.
\retval     NA.
 */
hi_s32 drv_symc_reset(hi_u32 chn_num);

/**
\brief  check the length of nodes list.
\param[in]  alg The symmetric cipher algorithm.
\param[in]  mode The symmetric cipher mode.
\param[in]  block_size The block size.
\param[in]  input The MMZ/SMMU address of in buffer.
\param[in]  output The MMZ/SMMU address of out buffer.
\param[in]  length The MMZ/SMMU siae of in buffer.
\param[in]  klen The key length.
\param[in]  usage_list The usage of node.
\param[in]  pkg_num The numbers of node.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_node_check(symc_alg alg, symc_mode mode,
                           symc_klen klen, hi_u32 block_size,
                           compat_addr input[],
                           compat_addr output[],
                           hi_u32 length[],
                           symc_node_usage usage_list[],
                           hi_u32 pkg_num);

/**
\brief  set work params.
\param[in]  chn_num The channel number.
\param[in]  alg The symmetric cipher algorithm.
\param[in]  mode The symmetric cipher mode.
\param[in]  decrypt Decrypt or encrypt.
\param[in]  sm1_round_num The round number of sm1.
\param[in]  klen The key length.
\param[in]  hard_key whether use the hard key or not.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_config(hi_u32 chn_num, symc_alg alg, symc_mode mode, symc_width width, hi_u32 decrypt,
                       hi_u32 sm1_round_num, symc_klen klen, hi_u32 hard_key);

/**
\brief  set the iv to the symc module.
\param[in]  chn_num The channel number.
\param[in]  iv The IV data, hardware use the valid bytes according to the alg.
\param[in]  flag The IV flag, should be CIPHER_IV_CHANGE_ONE_PKG or CIPHER_IV_CHANGE_ALL_PKG.
\retval     NA.
 */
void drv_symc_set_iv(hi_u32 chn_num, hi_u32 iv[SYMC_IV_MAX_SIZE_IN_WORD], hi_u32 ivlen, hi_u32 flag);

/**
\brief  get the iv to the symc module.
\param[in]  chn_num The channel number.
\param[out] iv The IV data, the length is 16.
\retval     NA.
 */
void drv_symc_get_iv(hi_u32 chn_num, hi_u32 iv[SYMC_IV_MAX_SIZE_IN_WORD], hi_u32 iv_len);

/**
\brief  set the key to the symc module.
\param[in]  chn_num The channel number.
\param[in]  key The key data, hardware use the valid bytes according to the alg.
\param[in]  odd This id odd key or not .
\retval     NA.
 */
void drv_symc_set_key(hi_u32 chn_num, hi_u32 key[SYMC_KEY_MAX_SIZE_IN_WORD], hi_u32 odd);

/**
\brief  set the key source to the symc module.
\param[in]  chn_num The channel number.
\param[in]  src The key source.
\retval     NA.
 */
void drv_symc_set_key_source(hi_u32 chn_num, symc_key_source src);

/**
\brief  add a in buffer to the nodes list.
\param[in]  chn_num The channel number.
\param[in]  buf_phy The MMZ/SMMU address of in buffer.
\param[in]  buf_size The MMZ/SMMU siae of in buffer.
\param[in]  local The locational of in buffer under a symmetric cipher.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_add_inbuf(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size, symc_node_usage usage);

/**
\brief  add a out buffer to the nodes list.
\param[in]  chn_num The channel number.
\param[in]  buf_phy The MMZ/SMMU address of out buffer.
\param[in]  buf_size The MMZ/SMMU siae of out buffer.
\param[in]  local The locational of in buffer under a symmetric cipher.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_add_outbuf(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size, symc_node_usage usage);

/**
\brief  add a buffer usage to the nodes list.
\param[in]  chn_num The channel number.
\param[in]  in in or out.
\param[in]  usage uasge.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
void drv_symc_add_buf_usage(hi_u32 chn_num, hi_u32 in, symc_node_usage usage);

/**
\brief  add N of CCM to the nodes list.
\param[in]  chn_num The channel number.
\param[in]  n The buffer of n, the size is 16.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_ccm_add_n(hi_u32 chn_num, hi_u8 *n);

/**
\brief  add A of CCM to the nodes list.
\param[in]  chn_num The channel number.
\param[in]  buf_phy The MMZ/SMMU address of A.
\param[in]  buf_size The MMZ/SMMU size of A.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_ccm_add_a(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size);

/**
\brief  add A of GCM to the nodes list.
\param[in]  chn_num The channel number.
\param[in]  buf_phy The MMZ/SMMU address of A.
\param[in]  buf_size The MMZ/SMMU size of A.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_gcm_add_a(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size);

/**
\brief  add length field of GCM to the nodes list.
\param[in]  chn_num The channel number.
\param[in]  buf_phy The MMZ/SMMU address of length field, the size is 16.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_gcm_add_clen(hi_u32 chn_num, hi_u8 *clen, hi_u32 clen_buf_len);

/**
\brief  get ccm/gcm tag.
\param[in]   chn_num The channel number.
\param[out]  tag The tag value.
\retval     On received interception, HI_TRUE is returned  otherwise HI_FALSE is returned.
 */
hi_s32 drv_aead_get_tag(hi_u32 chn_num, hi_u32 *tag);

/**
\brief  start symmetric cipher calculation.
\param[in]  chn_num The channel number.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_start(hi_u32 chn_num);

/**
\brief  wait running finished.
\param[in]  chn_num The channel number.
\retval     On received interception, HI_TRUE is returned  otherwise HI_FALSE is returned.
 */
hi_s32 drv_symc_wait_done(hi_u32 chn_num, hi_u32 timeout);

/**
\brief  set isr callback function.
\param[in]  chn_num The channel number.
\retval     On finished, HI_TRUE is returned otherwise HI_FALSE is returned.
 */
hi_s32 drv_symc_set_isr_callback(hi_u32 chn_num, callback_symc_isr callback, void *ctx);

/**
\brief  set destory callback function.
\param[in]  chn_num The channel number.
\retval     On finished, HI_TRUE is returned otherwise HI_FALSE is returned.
 */
void drv_symc_set_destory_callbcak(hi_u32 chn_num, callback_symc_destory destory);

/**
\brief  set destory callback function.
\param[in]  chn_num The channel number.
\param[in]  is_head Indicates whether the head data.
\param[in]  c_num Number of blocks in plain text in a pattern.
\param[in]  e_num Number of blocks in ciphertext in a pattern.
\param[in]  offset_len Length of the non-complete pattern in the head of node.
\retval     NA.
 */
void drv_symc_enable_cenc_node(u32 chn_num, u32 is_head, u32 c_num, u32 e_num, u32 offset_len);

/**
\brief  proc status.
\param[in]  status The  proc status.
\retval     On received interception, HI_TRUE is returned  otherwise HI_FALSE is returned.
 */
void drv_symc_proc_status(symc_chn_status *status, hi_u32 status_buf_len, symc_module_status *module);

/**
\brief  get the secure type of cpu .
\retval     secure cpu, HI_TRUE is returned.  On other, HI_FLASE is returned.
 */
hi_u32 drv_symc_is_secure(void);

/**
\brief  get the symc capacity.
\param[out] capacity The symc capacity.
\retval     NA.
 */
void drv_symc_get_capacity(symc_capacity *capacity);

/** @} */ /** <!-- ==== API declaration end ==== */

#endif
