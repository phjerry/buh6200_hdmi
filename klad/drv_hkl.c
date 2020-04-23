/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hisilicon keyladder driver.
* Author : Hisilicon security team
* Created : 2019-06-25
*/
#include "drv_hkl.h"

#include <linux/delay.h>

#include "drv_klad_hw_define.h"
#ifdef HI_PROC_SUPPORT
#include "drv_klad_proc.h"
#endif
#include "hal_klad.h"
#include "drv_klad_timestamp_logic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct engine_2_dsc_mode {
    hi_crypto_engine_alg engine;
    hi_u8 reg_dsc_mode;
};

hi_void dump_strct_com_klad(common_klad *com_klad)
{
    hi_u32 i;

    for (i = 0; i < HKL_LEVEL; i++) {
        print_dbg_hex(com_klad->klad_alg[i]);
        hi_klad_dump_hex("session_key", (hi_u8 *)&com_klad->session_key[i], HKL_KEY_LEN);
    }
    print_dbg_hex(com_klad->key_abort);
    hi_klad_dump_hex("com_klad->cfg", (hi_u8 *)&com_klad->cfg, sizeof(klad_cfg));
}

static hi_s32 __hkl_check_rst(hi_void)
{
    hi_s32 ret;
#ifdef HI_KLAD_PERF_SUPPORT
    struct klad_timestamp_logic t;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    timestamp(&t.time_wait_b);
#endif

    ret = hkl_wait_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_wait_idle, ret);
        goto out;
    }

#ifdef HI_KLAD_PERF_SUPPORT
    timestamp(&t.time_wait_e);
    t.tag = ALG_IP;

    t.time_ctl_b.tv_sec = mgmt->time_ctl_b.tv_sec;
    t.time_ctl_b.tv_nsec = mgmt->time_ctl_b.tv_nsec;

    t.time_ctl_e.tv_sec = mgmt->time_ctl_e.tv_sec;
    t.time_ctl_e.tv_nsec = mgmt->time_ctl_e.tv_nsec;

    t.time_int_b.tv_sec = mgmt->time_int_b.tv_sec;
    t.time_int_b.tv_nsec = mgmt->time_int_b.tv_nsec;

    t.time_int_e.tv_sec = mgmt->time_int_e.tv_sec;
    t.time_int_e.tv_nsec = mgmt->time_int_e.tv_nsec;

    klad_timestamp_logic_queue(&t);
#endif

    ret = hkl_check_err();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_check_err, ret);
        goto out;
    }
out:
    return ret;
}

static hi_s32 __hkl_func_session_start(common_klad *com_klad, hi_u32 level, hi_u32 com_kl_num,
                                       common_klad_crc *com_klad_crc)
{
    hi_s32 ret;

    if (level >= HKL_LEVEL || com_klad == NULL) {
        return HI_FAILURE;
    }

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    hkl_set_data_in(com_klad->session_key[level], HKL_KEY_LEN);
    hkl_set_com_ctrl(com_kl_num, level, com_klad->klad_alg[level], HI_FALSE, HI_FALSE);

    com_klad_crc->key_crc[level] = hkl_get_crc();
    com_klad_crc->level = level;

    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
        goto out;
    }
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 __hkl_func_content_start(common_klad *com_klad, hi_u32 level, hi_u32 com_kl_num,
                                       common_klad_crc *com_klad_crc)
{
    hi_s32 ret;

    if (level >= HKL_LEVEL || com_klad == NULL) {
        return HI_FAILURE;
    }

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    hkl_set_addr(com_klad->cfg.key_slot_num, com_klad->cfg.is_odd);
    hkl_set_cfg(com_klad->cfg.tgt_cfg.key_dec_support, com_klad->cfg.tgt_cfg.key_enc_support,
                com_klad->cfg.tgt_cfg.port_sel, com_klad->cfg.tgt_cfg.dsc_mode);
    hkl_set_sec_cfg(com_klad->cfg.tgt_sec_cfg.dest_sec, com_klad->cfg.tgt_sec_cfg.dest_nsec,
                    com_klad->cfg.tgt_sec_cfg.src_sec, com_klad->cfg.tgt_sec_cfg.src_nsec,
                    com_klad->cfg.tgt_sec_cfg.key_sec);

    ret = hkl_set_data_in(com_klad->session_key[level], HKL_KEY_LEN);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_set_data_in, ret);
        goto out;
    }

    hkl_set_com_ctrl(com_kl_num, level, com_klad->klad_alg[level], HI_FALSE, com_klad->key_abort);

    com_klad_crc->key_crc[level] = hkl_get_crc();
    com_klad_crc->level = level;

    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
        goto out;
    }
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 __hkl_func_start(common_klad *com_klad, hi_s32 level, hi_u32 com_kl_num, common_klad_crc *com_klad_crc)
{
    hi_s32 ret;
    hi_s32 i = 0;

    if (level >= HKL_LEVEL || com_klad == NULL || com_klad_crc == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    dump_strct_com_klad(com_klad);

    for (i = 0; i < level; i++) {
        ret = __hkl_func_session_start(com_klad, i, com_kl_num, com_klad_crc);
        if (ret != HI_SUCCESS) {
            print_err_func(__hkl_func_session_start, ret);
            goto out;
        }
    }

    ret = __hkl_func_content_start(com_klad, level, com_kl_num, com_klad_crc);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_func_content_start, ret);
    }

out:
    return ret;
}

static hi_s32 __rkp_func_start(rkp_deob_kdf *deob_kdf, klad_hw_id hw_id, rkp_deob_kdf_rst *deob_kdf_rst)
{
    hi_s32 ret;

    if (deob_kdf == NULL || deob_kdf_rst == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    deob_kdf->klad.klad_sel = (hi_rkp_klad_sel)hw_id;

    ret = rkp_eff_rk_start(deob_kdf);
    if (ret != HI_SUCCESS) {
        print_err_func(rkp_eff_rk_start, ret);
        goto out;
    }

    ret = dbg_rkp_eff_rk_crc(deob_kdf_rst);
    if (ret != HI_SUCCESS) {
        print_err_func(dbg_rkp_eff_rk_crc, ret);
        goto out;
    }
out:
    return ret;
}


static hi_s32 __hkl_fp_start(fp_klad *fpk_klad, hi_u32 com_kl_num, hi_u8 *fpk_crc)
{
    hi_s32 ret;

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    hkl_set_addr(fpk_klad->com_klad.cfg.key_slot_num, fpk_klad->com_klad.cfg.is_odd);
    hkl_set_cfg(fpk_klad->com_klad.cfg.tgt_cfg.key_dec_support, fpk_klad->com_klad.cfg.tgt_cfg.key_enc_support,
                fpk_klad->com_klad.cfg.tgt_cfg.port_sel, fpk_klad->com_klad.cfg.tgt_cfg.dsc_mode);
    hkl_set_sec_cfg(fpk_klad->com_klad.cfg.tgt_sec_cfg.dest_sec, fpk_klad->com_klad.cfg.tgt_sec_cfg.dest_nsec,
                    fpk_klad->com_klad.cfg.tgt_sec_cfg.src_sec, fpk_klad->com_klad.cfg.tgt_sec_cfg.src_nsec,
                    fpk_klad->com_klad.cfg.tgt_sec_cfg.key_sec);
    hkl_set_data_in(fpk_klad->fpk_ext_klad.session_key, HKL_KEY_LEN);
    hkl_set_fp_lev1(fpk_klad->fpk_ext_klad.is_dec, com_kl_num);

    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
        goto out;
    }
    *fpk_crc = hkl_get_crc();
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_void __dbg_klad_fp_crc(fp_klad_crc *fpk_klad_crc)
{
    hi_u32 i = 0;

    for (i = 0; i <= fpk_klad_crc->com_klad_crc.level; i++) {
        HI_ERR_KLAD("klad_crc L=%d: 0x%02x\n", i, fpk_klad_crc->com_klad_crc.key_crc[i]);
    }
    HI_ERR_KLAD("klad_crc L=%d: 0x%02x\n", i, fpk_klad_crc->fpk_crc);
}

static hi_s32 __fp_func_start(fp_klad *fpk_klad, hi_u32 com_kl_num, hi_s32 level, fp_klad_crc *fpk_klad_crc)
{
    hi_s32 ret;
    hi_s32 i = 0;

    if (level != HI_KLAD_LEVEL4 || fpk_klad == NULL || fpk_klad_crc == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    for (i = 0; i < level - 1; i++) {
        ret = __hkl_func_session_start(&fpk_klad->com_klad, i, com_kl_num, &fpk_klad_crc->com_klad_crc);
        if (ret != HI_SUCCESS) {
            print_err_func(__hkl_func_session_start, ret);
            goto out;
        }
    }

    ret = __hkl_fp_start(fpk_klad, com_kl_num, &fpk_klad_crc->fpk_crc);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_fp_start, ret);
        goto out;
    }

    __dbg_klad_fp_crc(fpk_klad_crc);

out:
    return ret;
}

static hi_s32 __fp_func_send(void)
{
    hi_s32 ret;

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    hkl_set_fp_lev2();

    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
    }
out:
    return ret;
}

static hi_s32 __fp_func_generate(fp_klad_reg *klad_clear)
{
    hi_s32 ret;

    if (klad_clear == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    ret = hkl_generate_fp_dec_key(&klad_clear->clear_key[0], &klad_clear->clear_key[0x1],
                                  &klad_clear->clear_key[0x2], &klad_clear->clear_key[0x3]);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_generate_fp_dec_key, ret);
    }
out:
    return ret;
}


static hi_s32 __fp_func_read_disable(hi_void)
{
    hi_s32 ret;

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    ret = hkl_set_fp_rd_disable();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_set_fp_rd_disable, ret);
    }
out:
    return ret;
}

/*
 * if clear key is 192 bit and algrithm is aes, it has a special combination.
 */
static hi_void __hkl_update_key(common_klad *com_klad)
{
    char *tmp = (char *)com_klad->session_key;
    int i;

    for (i = 31; i >= 8; i--) { /* move bit 0~ bit 24 to bit 8 ~bit 31 */
        tmp[i] = tmp[i - 8]; /* right shift 8 bytes. */
    }
    for (i = 0; i <= 7; i++) { /* set 0 within bit 0 ~ bit 7 */
        tmp[i] = 0;
    }
    return;
}

static hi_s32 __hkl_func_iv_128_start(clear_iv *clr_iv)
{
    hi_s32 ret;

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    hkl_set_data_in(clr_iv->iv, HKL_KEY_LEN);
    hkl_set_addr(clr_iv->key_slot_num, clr_iv->is_odd);
    /* set iv not care enc/dec/dsc_code attribute. */
    hkl_set_cfg(0, 0, clr_iv->port_sel, 0);
#ifndef HI_TEE_SUPPORT
    /*
    * If hostcpu is TEE CPU,need set key and buffer secure attribute.
    * otherwise kc_err will report a buffer security error. because logic did not care the different
    * between set key and set iv.
    */
    hkl_set_sec_cfg(1, 1, 1, 1, 1);
#endif
    ret = hkl_set_clr_ctrl(KLAD_CLR_128, HI_TRUE);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_set_clr_ctrl, ret);
    }

    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
        goto out;
    }
out:
    return ret;
}

static hi_s32 __hkl_func_clr_64_128_start(common_klad *com_klad, klad_clr_key key_size)
{
    hi_s32 ret;

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    hkl_set_cfg(com_klad->cfg.tgt_cfg.key_dec_support, com_klad->cfg.tgt_cfg.key_enc_support,
                com_klad->cfg.tgt_cfg.port_sel, com_klad->cfg.tgt_cfg.dsc_mode);
    hkl_set_sec_cfg(com_klad->cfg.tgt_sec_cfg.dest_sec, com_klad->cfg.tgt_sec_cfg.dest_nsec,
                    com_klad->cfg.tgt_sec_cfg.src_sec, com_klad->cfg.tgt_sec_cfg.src_nsec,
                    com_klad->cfg.tgt_sec_cfg.key_sec);
    hkl_set_data_in(com_klad->session_key[0], HKL_KEY_LEN);
    hkl_set_addr(com_klad->cfg.key_slot_num, com_klad->cfg.is_odd);

    ret = hkl_set_clr_ctrl(key_size, HI_FALSE);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_set_clr_ctrl, ret); /* can not go to this branch */
    }

    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
        goto out;
    }
out:
    return ret;
}

static hi_s32 __hkl_func_clr_192_256_start(common_klad *com_klad, klad_clr_key key_size)
{
    hi_s32 ret;

    /* if clear key is 192 bit and algrithm is aes(0x2x), it has a special combination */
    if ((key_size == KLAD_CLR_192) && ((com_klad->cfg.tgt_cfg.dsc_mode & 0xf0) == 0x20)) {
        __hkl_update_key(com_klad);
    }

    ret = hkl_is_idle();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_is_idle, ret);
        goto out;
    }

    hkl_set_cfg(com_klad->cfg.tgt_cfg.key_dec_support, com_klad->cfg.tgt_cfg.key_enc_support,
                com_klad->cfg.tgt_cfg.port_sel, com_klad->cfg.tgt_cfg.dsc_mode);
    hkl_set_sec_cfg(com_klad->cfg.tgt_sec_cfg.dest_sec, com_klad->cfg.tgt_sec_cfg.dest_nsec,
                    com_klad->cfg.tgt_sec_cfg.src_sec, com_klad->cfg.tgt_sec_cfg.src_nsec,
                    com_klad->cfg.tgt_sec_cfg.key_sec);

    /* if clear key is not 128 bit, use odd key register + even key register sent the whole key. */
    hkl_set_data_in(com_klad->session_key[0], HKL_KEY_LEN);
    hkl_set_addr(com_klad->cfg.key_slot_num, 0);
    ret = hkl_set_clr_ctrl(key_size, HI_FALSE);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_set_clr_ctrl, ret); /* can not go to this branch */
    }
    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
        goto out;
    }

    hkl_set_data_in(com_klad->session_key[1], HKL_KEY_LEN);
    hkl_set_addr(com_klad->cfg.key_slot_num, 1);
    ret = hkl_set_clr_ctrl(key_size, HI_FALSE);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_set_clr_ctrl, ret); /* can not go to this branch */
    }
    ret = __hkl_check_rst();
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_check_rst, ret);
        goto out;
    }
out:
    return ret;
}

static hi_s32 __clr_func_start(common_klad *com_klad, klad_clr_key key_size)
{
    hi_s32 ret;

    if (com_klad == NULL || key_size >= KLAD_CLR_MAX) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    if (key_size <= KLAD_CLR_128) {
        ret = __hkl_func_clr_64_128_start(com_klad, key_size);
        if (ret != HI_SUCCESS) {
            print_err_func(__hkl_func_clr_64_128_start, ret);
        }
    } else {
        ret = __hkl_func_clr_192_256_start(com_klad, key_size);
        if (ret != HI_SUCCESS) {
            print_err_func(__hkl_func_clr_192_256_start, ret);
        }
    }
    return ret;
}

static hi_s32 __clr_iv_func_start(clear_iv *clr_iv)
{
    hi_s32 ret;

    if (clr_iv == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    ret = __hkl_func_iv_128_start(clr_iv);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_func_iv_128_start, ret);
    }
    return ret;
}

static hi_s32 __hkl_clr_cw_length(hi_u32 key_size, klad_clr_key *key_len)
{
    if (key_size == 8) { /* 8 byte clear key, sent to tscipher, csa2/des/multi2 */
        *key_len = KLAD_CLR_64;
    } else if (key_size == 16) { /* 16 byte clear key, sent to cipher and tscipher */
        *key_len = KLAD_CLR_128;
    } else if (key_size == 24) { /* 24 byte clear key, only sent to cipher */
        *key_len = KLAD_CLR_192;
    } else if (key_size == 32) { /* 32 byte clear key, only sent to cipher */
        *key_len = KLAD_CLR_256;
    } else {
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    return HI_SUCCESS;
}

static struct engine_2_dsc_mode g_dsc_mode_reg_map[] = {
    {HI_CRYPTO_ENGINE_ALG_CSA2,             0x00},
    {HI_CRYPTO_ENGINE_ALG_CSA3,             0x10},
    {HI_CRYPTO_ENGINE_ALG_ASA,              0x91},

    {HI_CRYPTO_ENGINE_ALG_AES_ECB_T,        0x21},
    {HI_CRYPTO_ENGINE_ALG_AES_ECB_L,        0x24},

    {HI_CRYPTO_ENGINE_ALG_AES_CBC_T,        0x20},
    {HI_CRYPTO_ENGINE_ALG_AES_CISSA,        0x22},
    {HI_CRYPTO_ENGINE_ALG_AES_CBC_L,        0x25},

    {HI_CRYPTO_ENGINE_ALG_AES_CBC_IDSA,     0x20},
    {HI_CRYPTO_ENGINE_ALG_AES_IPTV,         0x20},
    {HI_CRYPTO_ENGINE_ALG_AES_CTR,          0x26},

    {HI_CRYPTO_ENGINE_ALG_DES_CI,           0x33},
    {HI_CRYPTO_ENGINE_ALG_DES_CBC,          0x33},
    {HI_CRYPTO_ENGINE_ALG_DES_CBC_IDSA,     0x30},

    {HI_CRYPTO_ENGINE_ALG_SMS4_ECB,         0x51},
    {HI_CRYPTO_ENGINE_ALG_SMS4_CBC,         0x53},
    {HI_CRYPTO_ENGINE_ALG_SMS4_CBC_IDSA,    0x50},

    {HI_CRYPTO_ENGINE_ALG_TDES_ECB,         0x71},
    {HI_CRYPTO_ENGINE_ALG_TDES_CBC,         0x73},
    {HI_CRYPTO_ENGINE_ALG_TDES_CBC_IDSA,    0x70},

    {HI_CRYPTO_ENGINE_ALG_MULTI2_ECB,       0x81},
    {HI_CRYPTO_ENGINE_ALG_MULTI2_CBC,       0x83},
    {HI_CRYPTO_ENGINE_ALG_MULTI2_CBC_IDSA,  0x80},

    {HI_CRYPTO_ENGINE_ALG_RAW_AES,          0x20},
    {HI_CRYPTO_ENGINE_ALG_RAW_DES,          0x30},
    {HI_CRYPTO_ENGINE_ALG_RAW_SM4,          0x50},
    {HI_CRYPTO_ENGINE_ALG_RAW_TDES,         0x70},
    {HI_CRYPTO_ENGINE_ALG_RAW_HMAC_SHA1,    0xa0},
    {HI_CRYPTO_ENGINE_ALG_RAW_HMAC_SHA2,    0xa1},
    {HI_CRYPTO_ENGINE_ALG_RAW_HMAC_SM3,     0xa2},
    {HI_CRYPTO_ENGINE_ALG_RAW_HDCP,         0xf0},
    {HI_CRYPTO_ENGINE_ALG_MAX,              0xff}
};

static hi_s32 __dsc_mode_gen(hi_handle target, hi_crypto_engine_alg engine_alg,
                             klad_port_sel *engine, hi_u32 *dsc_mode)
{
    hi_u8 i;

    if (engine_alg >= HI_CRYPTO_ENGINE_ALG_MAX) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    if (is_tscipher_ks_handle(target)) {
        *engine = PORT_SEL_TSCIPHER;
    } else {
        *engine = PORT_SEL_MCIPHER;
    }
    for (i = 0; i < sizeof(g_dsc_mode_reg_map) / sizeof(struct engine_2_dsc_mode); i++) {
        if (engine_alg == g_dsc_mode_reg_map[i].engine) {
            *dsc_mode = g_dsc_mode_reg_map[i].reg_dsc_mode;
            return HI_SUCCESS;
        }
    }
    return HI_ERR_KLAD_INVALID_ENGINE;
}

static hi_s32 __hkl_klad_cfg(const hi_klad_attr *attr, hi_handle target, hi_bool odd, common_klad *com_klad)
{
    hi_s32 ret;

    com_klad->cfg.is_odd = odd;
    com_klad->cfg.key_slot_num = HI_HANDLE_GET_CHAN_ID(target);
    com_klad->cfg.tgt_cfg.key_dec_support = attr->key_cfg.decrypt_support;
    com_klad->cfg.tgt_cfg.key_enc_support = attr->key_cfg.encrypt_support;
    ret = __dsc_mode_gen(target,
                         attr->key_cfg.engine,
                         &com_klad->cfg.tgt_cfg.port_sel,
                         &com_klad->cfg.tgt_cfg.dsc_mode);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    /* Key is always non secure, in linux, but it is set by caller in secure os system. */
    com_klad->cfg.tgt_sec_cfg.key_sec = HI_FALSE;

    /*
    * buffer is ignored if key secure set false.
    * Logic will update the buffer attribures according to fuse_info.disable_dest_source_ctrl(OTP) and
    * cfg.KS(CPU config key security)
    * 1):OTP disabled, security key sent to security buffer. non security kery sent to non security buffer.
    *     key_info.DS is cfg.KS;
    *     key_info.DNS is ~cfg.KS;
    *     key_info.SS is cfg.KS;
    *     key_info.SNS is ~cfg.KS;
    * 2):OTP enabled and cfg.KS is 0, Only sent to non security buffer.
    *     key_info.DS is 0;
    *     key_info.DNS is 1;
    *     key_info.SS is 0;
    *     key_info.SNS is 1;
    * 3):OTP enabled and cfg.KS is 1, buffer security properties can be configured via CPU.
    *    key_info.DS is cfg.DS;
    *    key_info.DNS is cfg.DNS;
    *    key_info.SS is cfg.SS;
    *    key_info.SNS is cfg.SNS;
    */
    com_klad->cfg.tgt_sec_cfg.dest_nsec = 1;
    com_klad->cfg.tgt_sec_cfg.dest_sec = 0;
    com_klad->cfg.tgt_sec_cfg.src_nsec = 1;
    com_klad->cfg.tgt_sec_cfg.src_sec = 0;

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_rkp_slot_choose __hkl_get_rk_slot(hi_rootkey_select root_slot)
{
    switch (root_slot) {
        case HI_CAS_ROOTKEY_SLOT0:
            return HI_RKP_SLOT_CHOOSE_SLOT0;
        case HI_CAS_ROOTKEY_SLOT1:
            return HI_RKP_SLOT_CHOOSE_SLOT1;
        case HI_CAS_ROOTKEY_SLOT2:
            return HI_RKP_SLOT_CHOOSE_SLOT2;
        case HI_CAS_ROOTKEY_SLOT3:
            return HI_RKP_SLOT_CHOOSE_SLOT3;
        case HI_CAS_ROOTKEY_SLOT4:
            return HI_RKP_SLOT_CHOOSE_SLOT4;
        case HI_CAS_ROOTKEY_SLOT5:
            return HI_RKP_SLOT_CHOOSE_SLOT5;
        case HI_CAS_ROOTKEY_SLOT6:
            return HI_RKP_SLOT_CHOOSE_SLOT6;
        case HI_CAS_ROOTKEY_SLOT7:
            return HI_RKP_SLOT_CHOOSE_SLOT7;
        case HI_BOOT_ROOTKEY_SLOT:
            return HI_RKP_SLOT_CHOOSE_BOOT;
        case HI_HISI_ROOTKEY_SLOT:
            return HI_RKP_SLOT_CHOOSE_HISI;
        case HI_OEM_ROOTKEY_SLOT:
            return HI_RKP_SLOT_CHOOSE_OEM;
        default:
            return HI_RKP_SLOT_CHOOSE_MAX;
    }
}

static hi_s32 __hkl_rkp_cfg(struct klad_r_base_attr *base_attr, rkp_deob_kdf *deob_kdf)
{
    deob_kdf->deob.rootkey_slot = __hkl_get_rk_slot(base_attr->root_slot);
    deob_kdf->kdf.rootkey_slot = deob_kdf->deob.rootkey_slot;

    /* vendor id is set by caller. */
    deob_kdf->kdf.vendor_id = base_attr->vendor_id;

    /*
    *  Note: kdf.cas_kdf_static = REE means generate key using REE static values, regardless of the host CPU,
    *  1) soc_tee(OTP) set( TEE enable), linux system runs in REE register(in logic)
    *  2) soc_tee(OTP) not set( TEE disable), linux system runs in TEE register(in logic)
    *  If kdf.cas_kdf_static set REE, we can generate a same rootkey for keyladder when module id and vendor id
    *  is fixed, no matter TEE enabled or not.
    */
    deob_kdf->kdf.cas_kdf_static = HI_RKP_CAS_KDF_STATIC_REE;

    /*
    * set 1 if want to generate unique rootkey per chipset, otherwise set 0.
    */
    deob_kdf->kdf.unique_type = base_attr->unique;

    deob_kdf->klad.klad_type_sel = HI_RKP_KLAD_TYPE_SEL_HKL;

    return HI_SUCCESS;
}

static hi_s32 __hkl_rkp_module_id(hi_u32 *module_id, hi_u32 len, rkp_deob_kdf *deob_kdf)
{
    if (len != 0x4) {
        print_err_hex(len);
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    deob_kdf->kdf.module_id0.u32 = module_id[0x0];
    deob_kdf->kdf.module_id1.u32 = module_id[0x1];
    deob_kdf->kdf.module_id2.u32 = module_id[0x2];
    deob_kdf->kdf.module_id3.u32 = module_id[0x3];

    return HI_SUCCESS;
}

static klad_alg_sel __hkl_alg(hi_klad_alg_type alg)
{
    if (alg == HI_KLAD_ALG_TYPE_DEFAULT) {
        return KLAD_ALG_RSD;
    } else if (alg == HI_KLAD_ALG_TYPE_TDES) {
        return KLAD_ALG_TDES;
    } else if (alg == HI_KLAD_ALG_TYPE_AES) {
        return KLAD_ALG_AES;
    } else if (alg == HI_KLAD_ALG_TYPE_SM4) {
        return KLAD_ALG_SM4;
    }
    return KLAD_ALG_RSD;
}

static hi_s32 __hkl_klad_com_key(const hi_klad_com_entry *instance, common_klad *com_klad)
{
    hi_s32 ret;
    hi_u32 i;

    for (i = 0; i < HI_KLAD_LEVEL_MAX; i++) {
        if (instance->session_cnt[i] == 0) {
            break;
        }
        if (instance->session_key[i].level != i || instance->session_key[i].key_size != HKL_KEY_LEN) {
            print_err_code(HI_ERR_KLAD_INVALID_PARAM);
            return HI_ERR_KLAD_INVALID_PARAM;
        }
        com_klad->klad_alg[i] = __hkl_alg(instance->session_key[i].alg);
        ret = memcpy_s(&com_klad->session_key[i], HKL_KEY_LEN,
                       &instance->session_key[i].key, HKL_KEY_LEN);
        if (ret != EOK) {
            print_err_func(memcpy_s, ret);
            return ret;
        }
    }
    if ((instance->content_cnt == 0) || (instance->content_key.key_size != HKL_KEY_LEN)) {
        print_err_code(HI_ERR_KLAD_INVALID_PARAM);
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    com_klad->klad_alg[i] = __hkl_alg(instance->content_key.alg);
    ret = memcpy_s(&com_klad->session_key[i], HKL_KEY_LEN,
                   &instance->content_key.key, HKL_KEY_LEN);
    if (ret != EOK) {
        print_err_func(memcpy_s, ret);
        return ret;
    }
    return ret;
}

#ifdef HI_INT_SUPPORT
static hi_void __hkl_isr_core(struct hkl_mgmt *mgmt)
{
    hal_klad_int_clr();

    mgmt->kl_alg_ip.wait_cond = 0x01;
    osal_wait_wakeup(&mgmt->kl_alg_ip.wait_queue);

    return;
}

static hi_void __hkl_isr_com_lock_impl(struct hkl_mgmt *mgmt)
{
    hal_klad_com_lock_int_clr();

    mgmt->kl_com_lock.lock_info = hkl_read_reg(HKL_COM_LOCK_INFO);
    mgmt->kl_com_lock.wait_cond  = 0x01;
    osal_wait_wakeup(&mgmt->kl_com_lock.wait_queue);
    return;
}

static hi_void __hkl_isr_fp_lock_impl(struct hkl_mgmt *mgmt)
{
    hal_klad_fp_lock_int_clr();

    mgmt->kl_fp_lock.lock_info = hkl_read_reg(HKL_FP_LOCK_INFO);
    mgmt->kl_fp_lock.wait_cond  = 0x01;
    osal_wait_wakeup(&mgmt->kl_fp_lock.wait_queue);

    return;
}

static hi_void __hkl_isr_nonce_lock_impl(struct hkl_mgmt *mgmt)
{
    hal_klad_nonce_lock_int_clr();

    mgmt->kl_nonce_lock.lock_info = hkl_read_reg(HKL_NONCE_LOCK_INFO);
    mgmt->kl_nonce_lock.wait_cond  = 0x01;
    osal_wait_wakeup(&mgmt->kl_nonce_lock.wait_queue);

    return;
}

static hi_void __hkl_isr_ta_lock_impl(struct hkl_mgmt *mgmt)
{
    hal_klad_ta_lock_int_clr();

    mgmt->kl_ta_lock.lock_info = hkl_read_reg(HKL_TA_LOCK_INFO);
    mgmt->kl_ta_lock.wait_cond  = 0x01;
    osal_wait_wakeup(&mgmt->kl_ta_lock.wait_queue);

    return;
}

static hi_void __hkl_isr_clr_route_lock_impl(struct hkl_mgmt *mgmt)
{
    hal_klad_clr_lock_int_clr();

    mgmt->kl_clr_lock.lock_info = hkl_read_reg(HKL_CLR_LOCK_INFO);
    mgmt->kl_clr_lock.wait_cond  = 0x01;
    osal_wait_wakeup(&mgmt->kl_clr_lock.wait_queue);

    return;
}

static hi_void _hkl_isr_impl(struct hkl_mgmt *mgmt)
{
    hkl_int_raw raw;

    timestamp(&mgmt->time_int_b);

    raw.u32 = hkl_read_reg(HKL_INT_RAW);
    if (raw.bits.kl_int_raw) {
        __hkl_isr_core(mgmt);
        timestamp(&mgmt->time_int_e);
    }

    if (raw.bits.kl_com_lock_int_raw) {
        __hkl_isr_com_lock_impl(mgmt);
    }

    if (raw.bits.kl_nonce_lock_int_raw) {
        __hkl_isr_nonce_lock_impl(mgmt);
    }

    if (raw.bits.kl_clr_lock_int_raw) {
        __hkl_isr_clr_route_lock_impl(mgmt);
    }

    if (raw.bits.kl_fp_lock_int_raw) {
        __hkl_isr_fp_lock_impl(mgmt);
    }

    if (raw.bits.kl_ta_lock_int_raw) {
        __hkl_isr_ta_lock_impl(mgmt);
    }
    timestamp(&mgmt->time_int_e);
    return;
}

static irqreturn_t _hkl_isr(hi_s32 irq, hi_void *priv)
{
    if (priv == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    if (hal_klad_get_int_stat() & 0x1) {
        _hkl_isr_impl((struct hkl_mgmt *)priv);
    }
    return IRQ_HANDLED;
}
#endif


static struct hkl_mgmt g_hkl_mgmt = {
    .lock      = {0},
    .state     = HKL_MGMT_CLOSED,
};

struct hkl_mgmt *__get_hkl_mgmt(hi_void)
{
    return &g_hkl_mgmt;
}

#ifdef HI_INT_SUPPORT
static hi_void __hkl_mgmt_irq_init(struct hkl_mgmt *mgmt)
{
    osal_mutex_init(&mgmt->kl_alg_ip.lock);
    osal_wait_init(&mgmt->kl_alg_ip.wait_queue);
    mgmt->kl_alg_ip.wait_cond = 0;

    osal_mutex_init(&mgmt->kl_csgk2_lock.lock);
    osal_wait_init(&mgmt->kl_csgk2_lock.wait_queue);
    mgmt->kl_csgk2_lock.wait_cond = 0;

    osal_mutex_init(&mgmt->kl_ta_lock.lock);
    osal_wait_init(&mgmt->kl_ta_lock.wait_queue);
    mgmt->kl_ta_lock.wait_cond = 0;

    osal_mutex_init(&mgmt->kl_fp_lock.lock);
    osal_wait_init(&mgmt->kl_fp_lock.wait_queue);
    mgmt->kl_fp_lock.wait_cond = 0;

    osal_mutex_init(&mgmt->kl_clr_lock.lock);
    osal_wait_init(&mgmt->kl_clr_lock.wait_queue);
    mgmt->kl_clr_lock.wait_cond = 0;

    osal_mutex_init(&mgmt->kl_nonce_lock.lock);
    osal_wait_init(&mgmt->kl_nonce_lock.wait_queue);
    mgmt->kl_nonce_lock.wait_cond = 0;

    osal_mutex_init(&mgmt->kl_com_lock.lock);
    osal_wait_init(&mgmt->kl_com_lock.wait_queue);
    mgmt->kl_com_lock.wait_cond = 0;
}

static hi_void __hkl_mgmt_irq_deinit(struct hkl_mgmt *mgmt)
{
    osal_mutex_destory(&mgmt->kl_alg_ip.lock);
    osal_wait_destroy(&mgmt->kl_alg_ip.wait_queue);
    mgmt->kl_alg_ip.wait_cond = 0;

    osal_mutex_destory(&mgmt->kl_csgk2_lock.lock);
    osal_wait_destroy(&mgmt->kl_csgk2_lock.wait_queue);
    mgmt->kl_csgk2_lock.wait_cond = 0;

    osal_mutex_destory(&mgmt->kl_ta_lock.lock);
    osal_wait_destroy(&mgmt->kl_ta_lock.wait_queue);
    mgmt->kl_ta_lock.wait_cond = 0;

    osal_mutex_destory(&mgmt->kl_fp_lock.lock);
    osal_wait_destroy(&mgmt->kl_fp_lock.wait_queue);
    mgmt->kl_fp_lock.wait_cond = 0;

    osal_mutex_destory(&mgmt->kl_clr_lock.lock);
    osal_wait_destroy(&mgmt->kl_clr_lock.wait_queue);
    mgmt->kl_clr_lock.wait_cond = 0;

    osal_mutex_destory(&mgmt->kl_nonce_lock.lock);
    osal_wait_destroy(&mgmt->kl_nonce_lock.wait_queue);
    mgmt->kl_nonce_lock.wait_cond = 0;

    osal_mutex_destory(&mgmt->kl_com_lock.lock);
    osal_wait_destroy(&mgmt->kl_com_lock.wait_queue);
    mgmt->kl_com_lock.wait_cond = 0;
}
#endif

#ifndef HI_TEE_SUPPORT
hi_void hkl_perm_multi2_cfg(hi_void)
{
    hi_u8 *addr = HI_NULL;

    addr = (hi_u8 *)osal_ioremap_nocache(PERM_MULTI2_CFG, 0x10);
    if (addr == HI_NULL) {
        print_err_val(PERM_MULTI2_CFG);
        return;
    }
    reg_write(addr, 0x20); /* set fixed value, 0x20, come from logic engineer. */
    HI_INFO_KLAD("klad w 0x%08x 0x%08x\n", PERM_MULTI2_CFG, 0x20);
    osal_iounmap(addr);
}
#endif

#if HI_INT_SUPPORT
hi_void hkl_int_deinit(hi_void)
{
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    __mutex_lock(&mgmt->lock);

    hal_klad_int_clr();
    hal_klad_lock_int_clr();

    hal_klad_int_dis();
    hal_klad_lock_int_dis();

    __mutex_unlock(&mgmt->lock);
}

hi_void hkl_int_init(hi_void)
{
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    __mutex_lock(&mgmt->lock);

    hal_klad_int_en();
    hal_klad_lock_int_en();

    hal_klad_int_clr();
    hal_klad_lock_int_clr();

    __mutex_unlock(&mgmt->lock);
}
#endif

hi_s32 hkl_mgmt_init(hi_void)
{
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    mgmt->io_base = osal_ioremap_nocache(KLAD_REG_BASE, KLAD_REG_RANGE);
    if (mgmt->io_base == HI_NULL) {
        ret = HI_ERR_KLAD_MEM_MAP;
        goto out;
    }

    ret = osal_mutex_init(&mgmt->lock);
    if (ret != HI_SUCCESS) {
        goto out;
    }
#ifdef HI_INT_SUPPORT
    __hkl_mgmt_irq_init(mgmt);

    if (hi_drv_osal_request_irq(HKL_IRQ_NUMBER, (irq_handler_t)_hkl_isr, IRQF_SHARED,
                                HKL_IRQ_NAME, (hi_void *)mgmt) != 0) {
        HI_ERR_KLAD("osal_request irq(%d) failed.\n", HKL_IRQ_NUMBER);
        ret = HI_FAILURE;
        goto out;
    }
    if (hi_drv_sys_set_irq_affinity(HI_ID_KLAD, HKL_IRQ_NUMBER, HKL_IRQ_NAME) != HI_SUCCESS) {
        HI_WARN_KLAD("set klad irq affinity failed.\n");
    }
    hkl_int_init();
#endif
#ifndef HI_TEE_SUPPORT
    hkl_perm_multi2_cfg();
#endif
    mgmt->state = HKL_MGMT_OPENED;
    return  HI_SUCCESS;
out:
    mgmt->state = HKL_MGMT_CLOSED;
    return ret;
}

hi_void hkl_mgmt_exit(hi_void)
{
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

#ifdef HI_INT_SUPPORT
    hkl_int_deinit();
    hi_drv_osal_free_irq(HKL_IRQ_NUMBER, HKL_IRQ_NAME, (hi_void *)mgmt);
    __hkl_mgmt_irq_deinit(mgmt);
#endif
    mgmt->state = HKL_MGMT_CLOSED;
    osal_mutex_destory(&mgmt->lock);

    osal_iounmap(mgmt->io_base);
}


hi_s32 hi_klad_com_start(rkp_deob_kdf *deob_kdf, common_klad *com_klad, hi_u32 com_kl_num,
                         common_klad_crc *com_klad_crc, rkp_deob_kdf_rst *deob_kdf_rst)
{
    hi_s32 ret;
    hi_s32 level;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    if (deob_kdf == NULL || com_klad == NULL || deob_kdf_rst == NULL || com_klad_crc == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    osal_mutex_lock(&mgmt->lock);

    ret = __rkp_func_start(deob_kdf, com_kl_num, deob_kdf_rst);
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_func_start, ret);
        goto out;
    }

    level = rkp_get_klad_level(deob_kdf->kdf.module_id0.u32);

    ret = __hkl_func_start(com_klad, level, com_kl_num, com_klad_crc);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_func_start, ret);
        goto out;
    }
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 hi_klad_fp_start(rkp_deob_kdf *deob_kdf, fp_klad *fpk_klad, hi_u32 com_kl_num,
                        fp_klad_crc *fpk_klad_crc, rkp_deob_kdf_rst *deob_kdf_rst)
{
    hi_s32 ret;
    hi_u32 level;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    if (deob_kdf == NULL || fpk_klad_crc == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = __rkp_func_start(deob_kdf, com_kl_num, deob_kdf_rst);
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_func_start, ret);
        goto out;
    }

    level = rkp_get_klad_level(deob_kdf->kdf.module_id0.u32);
    ret = __fp_func_start(fpk_klad, com_kl_num, level + 1, fpk_klad_crc);
    if (ret != HI_SUCCESS) {
        print_err_func(__fp_func_start, ret);
    }
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 hi_klad_fp_generate(fp_klad_reg *klad_clear)
{
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    if (klad_clear == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = hi_klad_fp_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_fp_lock, ret);
        goto out;
    }
    ret = __fp_func_generate(klad_clear);
    if (ret != HI_SUCCESS) {
        print_err_func(__fp_func_generate, ret);
    }

    hi_klad_fp_unlock();
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 hi_klad_fp_send(void)
{
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->lock);

    ret = hi_klad_fp_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_fp_lock, ret);
        goto out;
    }
    ret = __fp_func_send();
    if (ret != HI_SUCCESS) {
        print_err_func(__fp_func_send, ret);
    }

    hi_klad_fp_unlock();
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 hi_klad_fp_rd_disable(void)
{
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->lock);

    ret = hi_klad_fp_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_fp_lock, ret);
        goto out;
    }
    ret = __fp_func_read_disable();
    if (ret != HI_SUCCESS) {
        print_err_func(__fp_func_read_disable, ret);
    }

    hi_klad_fp_unlock();
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 hi_klad_clrcw_process(common_klad *com_klad, klad_clr_key key_len)
{
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    if (com_klad == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    osal_mutex_lock(&mgmt->lock);

    ret = hi_klad_clrcw_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_clrcw_lock, ret);
        goto out;
    }

    ret = __clr_func_start(com_klad, key_len);
    if (ret != HI_SUCCESS) {
        print_err_func(__clr_func_start, ret);
        goto unlock;
    }
unlock:
    hi_klad_clrcw_unlock();
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 hi_klad_clriv_process(clear_iv *clr_iv)
{
    hi_s32 ret;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    if (clr_iv == NULL) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    osal_mutex_lock(&mgmt->lock);
    ret = hi_klad_clrcw_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_clrcw_lock, ret);
        goto out;
    }

    ret = __clr_iv_func_start(clr_iv);
    if (ret != HI_SUCCESS) {
        print_err_func(__clr_iv_func_start, ret);
        goto unlock;
    }
unlock:
    hi_klad_clrcw_unlock();
out:
    osal_mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 hi_klad_clr_entry_start(struct klad_r_clr_route *clr_route, const hi_klad_clr_entry *instance)
{
    hi_s32 ret;
    common_klad com_klad = {{0}};
    klad_clr_key key_len;

    dump_hkl_clr_route(instance);

    unused(clr_route);

    ret = __hkl_clr_cw_length(instance->clr_key.key_size, &key_len);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_clr_cw_length, ret);
        goto out;
    }

    ret = memcpy_s(com_klad.session_key, sizeof(com_klad.session_key), instance->clr_key.key, HI_KLAD_MAX_KEY_LEN);
    if (ret != HI_SUCCESS) {
        print_err_func(memcpy_s, ret);
        goto out;
    }

    ret = __hkl_klad_cfg(&instance->attr, instance->target_handle, instance->clr_key.odd, &com_klad);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_klad_cfg, ret);
        goto out;
    }
    /*
    * Clear keyladder is different other keyladder, lock and unlock logic resource immediately.
    * Beacuse clear keyladder need not to derive rootkey from RKP.
    */
    ret = hi_klad_clrcw_process(&com_klad, key_len);
out:
    return ret;
}

hi_s32 hi_klad_rkp_entry_start(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;
    rkp_deob_kdf deob_kdf = {{0}};
    rkp_deob_kdf_rst deob_kdf_rst = {{0}};
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    ret = __hkl_rkp_module_id(com_hkl->base_attr.module_id, MODULE_ID_CNT, &deob_kdf);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_rkp_module_id, ret);
        goto out;
    }

    ret = __hkl_rkp_cfg(&com_hkl->base_attr, &deob_kdf);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_rkp_cfg, ret);
        goto out;
    }
    osal_mutex_lock(&mgmt->lock);
    ret = __rkp_func_start(&deob_kdf, com_hkl->base.hw_id, &deob_kdf_rst);
    if (ret != HI_SUCCESS) {
        print_err_func(__rkp_func_start, ret);
    }
    osal_mutex_unlock(&mgmt->lock);
out:
    return ret;
}

hi_s32 hi_klad_com_entry_start(struct klad_r_com_hkl *com_hkl, const hi_klad_com_entry *instance)
{
    hi_s32 ret;
    common_klad com_klad = {{0}};
    common_klad_crc com_klad_crc = {0};
    hi_u32 level;
    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    dump_hkl_com(instance);

    ret = __hkl_klad_cfg(&instance->attr, instance->target_handle, instance->content_key.odd, &com_klad);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_rkp_module_id, ret);
        goto out;
    }

    level = rkp_get_klad_level(com_hkl->base_attr.module_id[0]);

    ret = __hkl_klad_com_key(instance, &com_klad);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_klad_com_key, ret);
        goto out;
    }
    osal_mutex_lock(&mgmt->lock);
    ret = __hkl_func_start(&com_klad, level, com_hkl->base.hw_id, &com_klad_crc);
    if (ret != HI_SUCCESS) {
        print_err_func(__hkl_func_start, ret);
    }
    osal_mutex_unlock(&mgmt->lock);
out:
    return ret;
}


hi_s32 hi_klad_clrcw_lock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_clr_lock.lock);
    ret = hkl_clrcw_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_clrcw_lock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_clr_lock.lock);

    return ret;
}

hi_s32 hi_klad_clrcw_unlock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_clr_lock.lock);
    ret = hkl_clrcw_unlock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_clrcw_unlock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_clr_lock.lock);

    return ret;
}

hi_s32 hi_klad_com_lock(hi_u32 *com_kl_num)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_com_lock.lock);

    ret = hkl_com_lock(com_kl_num);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_com_lock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_com_lock.lock);

    return ret;
}

hi_s32 hi_klad_com_unlock(hi_u32 com_kl_num)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_com_lock.lock);

    ret = hkl_com_unlock(com_kl_num);
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_com_unlock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_com_lock.lock);

    return ret;
}

hi_s32 hi_klad_ta_lock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_ta_lock.lock);

    ret = hkl_ta_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_ta_lock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_ta_lock.lock);

    return ret;
}

hi_s32 hi_klad_ta_unlock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_ta_lock.lock);

    ret = hkl_ta_unlock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_ta_unlock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_ta_lock.lock);

    return ret;
}

hi_s32 hi_klad_fp_lock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_fp_lock.lock);

    ret = hkl_fp_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_fp_lock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_fp_lock.lock);

    return ret;
}

hi_s32 hi_klad_fp_unlock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_fp_lock.lock);

    ret = hkl_fp_unlock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_fp_unlock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_fp_lock.lock);

    return ret;
}

hi_s32 hi_klad_nonce_lock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_nonce_lock.lock);

    ret = hkl_nonce_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_nonce_lock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_nonce_lock.lock);

    return ret;
}

hi_s32 hi_klad_nonce_unlock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_nonce_lock.lock);

    ret = hkl_nonce_unlock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_nonce_unlock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_nonce_lock.lock);

    return ret;
}

hi_s32 hi_klad_csgk2_lock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_csgk2_lock.lock);

    ret = hkl_csgk2_lock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_csgk2_lock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_csgk2_lock.lock);

    return ret;
}

hi_s32 hi_klad_csgk2_unlock(void)
{
    hi_s32 ret;

    struct hkl_mgmt *mgmt = __get_hkl_mgmt();

    osal_mutex_lock(&mgmt->kl_csgk2_lock.lock);

    ret = hkl_csgk2_unlock();
    if (ret != HI_SUCCESS) {
        print_err_func(hkl_csgk2_unlock, ret);
    }
    osal_mutex_unlock(&mgmt->kl_csgk2_lock.lock);

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
