/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_cenc_v300
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "drv_cenc.h"
#include "drv_symc.h"

#if defined(HI_PRODUCT_CENC_SUPPORT)
#define CENC_FIRSTENCRYPTOFFSET_MAX_LEN (AES_BLOCK_SIZE_IN_BYTE - 1)
#define CENC_SUBSAMPLE_MAX_NUM          (100)
#define CENC_MAX_DATALEN                (0X2000000)
#define CENC_MAX_PATTERN_LEN            (255 * 16)
#define CENC_IV_VALID_BYTE              8

static hi_s32 cenc_config(hi_handle handle, hi_cipher_work_mode *word_mode)
{
    hi_s32 ret;
    symc_mode mode = SYMC_MODE_CBC;
    symc_alg alg = SYMC_ALG_AES;
    hi_cipher_ctrl ctrl;
    hi_u32 chn;

    chn = HI_HANDLE_GET_CHAN_ID(handle);

    ret = kapi_symc_unlock_get_config(handle, &ctrl);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_unlock_get_config, ret);
        return ret;
    }

    if (ctrl.work_mode == HI_CIPHER_WORK_MODE_CTR) {
        mode = SYMC_MODE_CTR;
    }

    if (ctrl.alg == HI_CIPHER_ALG_SM4) {
        alg = SYMC_ALG_SM4;
    }

    ret = drv_symc_config(chn, alg, mode, SYMC_DAT_WIDTH_128,
                          HI_TRUE, 0, SYMC_KEY_DEFAULT, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_config, ret);
        return ret;
    }

    *word_mode = ctrl.work_mode;

    return HI_SUCCESS;
}

static hi_s32 cenc_node_finish(hi_u32 chn_num)
{
    hi_s32 ret;

    hi_log_func_enter();

    drv_symc_add_buf_usage(chn_num, HI_TRUE, SYMC_NODE_USAGE_LAST);

    /* start decrypt */
    ret = drv_symc_start(chn_num);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_start, ret);
        return ret;
    }

    /* waitting decrypt finished */
    ret = drv_symc_wait_done(chn_num, CRYPTO_TIME_OUT);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_wait_done, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cenc_add_node(hi_u32 chn_num,
                            hi_bool odd_key,
                            hi_u32 is_clear_head,
                            compat_addr input,
                            compat_addr output,
                            hi_u32 pattern_clean_len,
                            hi_u32 pattern_enc_len,
                            hi_u32 offset_len,
                            hi_u32 len)
{
    hi_s32 ret;
    symc_node_usage usage = SYMC_NODE_USAGE_NORMAL;

    hi_log_func_enter();

    if (len == 0) {
        return HI_SUCCESS;
    }

    if (odd_key == HI_TRUE) {
        usage = ((hi_u32)usage) | SYMC_NODE_USAGE_ODD_KEY;
    }

    ret = drv_symc_add_inbuf(chn_num, input, len, usage);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_inbuf, ret);
        return ret;
    }

    ret = drv_symc_add_outbuf(chn_num, output, len, SYMC_NODE_USAGE_NORMAL);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_outbuf, ret);
        return ret;
    }

    /* head data ,don't care e num and c num */
    if ((pattern_clean_len == 0) && (pattern_enc_len == 0)) {
        pattern_enc_len = AES_BLOCK_SIZE_IN_BYTE;
    }
    drv_symc_enable_cenc_node(chn_num, is_clear_head, pattern_clean_len, pattern_enc_len, offset_len);

    return HI_SUCCESS;
}

static hi_u32 cenc_calc_offset(hi_cipher_work_mode word_mode, hi_u32 first_offset, hi_cipher_subsample *subsample)
{
    hi_u32 pattern_len;
    hi_u32 current_offset = 0;
    hi_u32 last_len;
    static hi_u32 last_pattern_len = 0;

    pattern_len = subsample->payload_pattern_encrypt_len + subsample->payload_pattern_clear_len;
    if (pattern_len == 0) {
        /* non-pattern mode, all blocks in payload are encrypted */
        pattern_len = AES_BLOCK_SIZE_IN_BYTE;
    }

    if (word_mode == HI_CIPHER_WORK_MODE_CBC) {
        return pattern_len - subsample->payload_pattern_offset_len;
    }

    /* CTR mode */
    if (subsample->iv_change == HI_TRUE) {
        last_pattern_len = 0;
    }

    if (first_offset != 0) { /* update offset len for first node */
        last_pattern_len = first_offset;
    }

    /* the head of payload may be part of last pattern in last subsample,
     * encrypt data stored in front of clear data
     *  H | Pattern_offset(processed blocks)| First_offset(processed part of last block)
          | Part_last_Pattern_Blocks | E | C | E | C
     */

    last_len = subsample->payload_pattern_offset_len + last_pattern_len;
    if (last_len != 0) {
        current_offset = pattern_len - last_len;
    }
    last_pattern_len = (last_pattern_len + subsample->payload_len) % AES_BLOCK_SIZE_IN_BYTE;
    hi_log_info("pattern_len 0x%x, first_offset 0x%x, current offset 0x%x, last_pattern_len 0x%x\n",
        pattern_len, first_offset, current_offset, last_pattern_len);

    return current_offset;
}

static hi_s32 cenc_add_subsample(hi_u32 chn_num,
                                 hi_bool odd_key,
                                 hi_cipher_work_mode word_mode,
                                 hi_u32 first_block_offset,
                                 compat_addr inputaddr,
                                 compat_addr outputaddr,
                                 hi_cipher_subsample *subsample)
{
    hi_s32 ret;
    hi_u32 current_offset;

    hi_log_func_enter();

    if (subsample->iv_change == HI_TRUE) {
        drv_symc_set_iv(chn_num, subsample->iv, CENC_IV_VALID_BYTE, CIPHER_IV_CHANGE_ONE_PKG);
    }

    /* add clear head data to node if exit */
    ret = cenc_add_node(chn_num, odd_key, HI_TRUE, inputaddr, outputaddr, subsample->payload_pattern_clear_len,
                        subsample->payload_pattern_encrypt_len, 0x00, subsample->clear_header_len);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cenc_add_node, ret);
        return ret;
    }
    ADDR_U64(inputaddr) += subsample->clear_header_len;
    ADDR_U64(outputaddr) += subsample->clear_header_len;

    /* add payload data to node if exit */
    current_offset = cenc_calc_offset(word_mode, first_block_offset, subsample);
    ret = cenc_add_node(chn_num, odd_key, HI_FALSE, inputaddr, outputaddr, subsample->payload_pattern_clear_len,
                        subsample->payload_pattern_encrypt_len, current_offset, subsample->payload_len);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cenc_add_node, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* The format of CBC subsample is: H means head, P means Payload
 * H | P | H | P | H | P
 *
 * The format of Payload is: E means encrypt data, C clear data
 * E | C | E | C | E | C
 * the CBC mode does not encrypt Blocks smaller than 16 Bytes to
 * avoid adding padding that would cahnge the file size.
 */
static hi_s32 cenc_decrypt_sample(hi_handle handle,
                                  hi_cipher_cenc *cenc,
                                  compat_addr inputaddr,
                                  compat_addr outputaddr)
{
    hi_s32 ret;
    hi_cipher_subsample *subsample;
    hi_u32 chn_num;
    hi_u32 subsample_len;
    hi_cipher_work_mode word_mode = HI_CIPHER_WORK_MODE_BUTT;
    hi_u32 i;

    hi_log_func_enter();

    chn_num = HI_HANDLE_GET_CHAN_ID(handle);

    ret = cenc_config(handle, &word_mode);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cenc_config, ret);
        return ret;
    }

    for (i = 0; i < cenc->subsample_num; i++) {
        subsample = cenc->subsample + i;

        hi_log_debug("subsample %d, input 0x%llx, output 0x%llx, iv-change %d\n", i,
                     ADDR_U64(inputaddr), ADDR_U64(outputaddr), cenc->subsample[i].iv_change);

        ret = cenc_add_subsample(chn_num, cenc->odd_key, word_mode, cenc->first_offset,
            inputaddr, outputaddr, subsample);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(cenc_add_subsample, ret);
            return ret;
        }
        cenc->first_offset = 0;

        /* move to next subsample */
        subsample_len = subsample->clear_header_len + subsample->payload_len;
        ADDR_U64(inputaddr) += subsample_len;
        ADDR_U64(outputaddr) += subsample_len;
    }

    ret = cenc_node_finish(chn_num);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cenc_node_finish, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 cenc_length_check(hi_u32 bytelength, hi_cipher_subsample *subsample, hi_u32 number)
{
    hi_u32 total = 0;
    hi_u32 pattern_len = 0;
    hi_u32 i;

    hi_log_func_enter();

    for (i = 0; i < number; i++) {
        hi_log_info("subsample %d, ClearHead 0x%x, Payload 0x%x, PatternEncryp 0x%x, "
            "PatternClear 0x%x, PatternOffset 0x%x, iv-change %d\n", i,
                    subsample[i].clear_header_len,
                    subsample[i].payload_len,
                    subsample[i].payload_pattern_encrypt_len,
                    subsample[i].payload_pattern_clear_len,
                    subsample[i].payload_pattern_offset_len,
                    subsample[i].iv_change);

        if ((subsample[i].payload_pattern_encrypt_len > CENC_MAX_PATTERN_LEN)
            || (subsample[i].payload_pattern_clear_len > CENC_MAX_PATTERN_LEN)
            || (subsample[i].payload_pattern_offset_len > CENC_MAX_PATTERN_LEN)
            || (subsample[i].clear_header_len > CENC_MAX_DATALEN)
            || (subsample[i].payload_len > CENC_MAX_DATALEN)) {
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }

        /* PatternOffsetLen means how many blocks already be processed in last Pattern,
         * so the PatternOffsetLen must less than PatternEncryptLen + PatternClearLen.
         */
        pattern_len = subsample[i].payload_pattern_encrypt_len + subsample[i].payload_pattern_clear_len;
        if (subsample[i].payload_pattern_offset_len > pattern_len) {
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
        total += subsample[i].clear_header_len + subsample[i].payload_len;
        if (total > bytelength) {
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    if (total != bytelength) {
        hi_log_error("invalid CencDecrypt total data bytelength 0x%x, input length 0x%x!\n", total, bytelength);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 cenc_decrypt(hi_u32 id,
                    hi_cipher_cenc *cenc,
                    compat_addr inputphy,
                    compat_addr outputphy,
                    hi_u32 bytelength)
{
    hi_s32 ret;
    hi_cipher_cenc local_cenc;
    hi_u32 subsample_len;

    hi_log_func_enter();

    hi_log_check_param(cenc == HI_NULL);
    hi_log_check_param(cenc->subsample == HI_NULL);
    hi_log_check_param(cenc->subsample_num == 0);
    hi_log_check_param(cenc->subsample_num > CENC_SUBSAMPLE_MAX_NUM);
    hi_log_check_param(bytelength > CENC_MAX_DATALEN);
    hi_log_check_param(cenc->first_offset > CENC_FIRSTENCRYPTOFFSET_MAX_LEN);
    hi_log_check_param(ADDR_U64(inputphy) + bytelength < bytelength);
    hi_log_check_param(ADDR_U64(outputphy) + bytelength < bytelength);

    hi_log_info("inputphy 0x%llx, outputphy 0x%llx, firstoffset 0x%x, bytelength 0x%x, number %d, oddkey 0x%x\n",
                ADDR_U64(inputphy), ADDR_U64(outputphy), cenc->first_offset,
                bytelength, cenc->subsample_num, cenc->odd_key);

    ret = memcpy_s(&local_cenc, sizeof(local_cenc), cenc, sizeof(hi_cipher_cenc));
    if (ret != HI_NULL) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }
    subsample_len = cenc->subsample_num * sizeof(hi_cipher_subsample);
    local_cenc.subsample = (hi_cipher_subsample *)crypto_malloc(subsample_len);
    if (local_cenc.subsample == HI_NULL) {
        hi_log_print_func_err(crypto_malloc, HI_FAILURE);
        return HI_FAILURE;
    }

    /* copy subsample from user buffer */
    ret = crypto_copy_from_user(local_cenc.subsample, subsample_len, cenc->subsample, subsample_len);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_copy_from_user, ret);
        goto _exit;
    }

    /* check the length avoid overflow or inversion */
    ret = cenc_length_check(bytelength, local_cenc.subsample, local_cenc.subsample_num);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cenc_length_check, ret);
        goto _exit;
    }

    ret = cenc_decrypt_sample(id, &local_cenc, inputphy, outputphy);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cenc_decrypt_process, ret);
        goto _exit;
    }

_exit:
    crypto_free(local_cenc.subsample);
    local_cenc.subsample = HI_NULL;

    return ret;
}

#else
hi_s32 cenc_decrypt(hi_u32 id,
                    hi_cipher_cenc *cenc,
                    hi_u32 inputphy,
                    hi_u32 outputphy,
                    hi_u32 bytelength)
{
    hi_log_error("error, nonsupport CENC\n");
    return HI_FAILURE;
}
#endif  // End of HI_SMMU_SUPPORT

