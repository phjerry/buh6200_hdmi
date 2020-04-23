/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_hdcp_v300
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_hdcp_v300.h"
#include "drv_symc_v300.h"
#include "cryp_symc.h"
#include "drv_symc.h"
#include "drv_hdcp.h"

#ifdef CHIP_HDCP_VER_V300

/*************************** Internal Structure Definition ****************************/
/** \addtogroup      hdcp drivers */
/** @{ */ /** <!-- [hdcp] */

/* ! Define the time out */
#define HDCP_TIME_OUT 5000

/* ! \HDCP ROOT KEY size 128bit */
#define HDCP_ROOT_KEY_SIZE (16)

typedef enum {
    HDMI_WRITE_TX_HDCP_14 = 0x00,
    HDMI_WRITE_RX_HDCP_14 = 0x01,
    HDMI_WRITE_RX_HDCP_22 = 0x02,
} hdmi_ram_write_sel;

#define HDCP_RX_SEL_22      1
#define HDCP_RX_SEL_14      0
#define HDCP_MODE_SEL_TX_14 0
#define HDCP_MODE_SEL_RX    1

#define SPACC_CPU_CUR (crypto_is_sec_cpu() ? SPACC_CPU_TEE : SPACC_CPU_REE)

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      hdcp */
/** @{ */ /** <!--[hdcp] */

static hi_s32 drv_symc_alloc_chn0(hi_void)
{
    hi_u32 used;
    hi_u32 chnn_who_used = 0;
    hi_u32 my_cpu = SPACC_CPU_REE;
    hi_s32 ret;
    hi_u32 i = 0;

    hi_log_func_enter();

    if (crypto_is_sec_cpu() == HI_TRUE) {
        my_cpu = SPACC_CPU_TEE;
    }

    for (i = 0; i < CRYPTO_TIME_OUT; i++) {

        used = symc_read(SPACC_SYM_CHN_LOCK);
        chnn_who_used = CHN_WHO_USED_GET(used, 0);
        if (chnn_who_used != SPACC_CPU_IDLE) {
            continue;
        }

        /* try to use this channel */
        CHN_WHO_USED_SET(used, 0, my_cpu);
        symc_write(SPACC_SYM_CHN_LOCK, used);

        /* check if the channal aleardy be useded by other cpu
         * if other cpu break-in when write the CIPHER_NON_SEC_CHN_WHO_USED
         * the value of CIPHER_NON_SEC_CHN_WHO_USED will be channged
         */
        used = symc_read(SPACC_SYM_CHN_LOCK);
        chnn_who_used = CHN_WHO_USED_GET(used, 0);
        if (chnn_who_used != my_cpu) {
            /* chn aleardy be used by tee */
            continue;
        }

        break;
    }

    if (i >= CRYPTO_TIME_OUT) {
        hi_log_error("symc alloc channel0 failed\n");
        hi_log_print_err_code(HI_ERR_CIPHER_BUSY);
        return HI_ERR_CIPHER_BUSY;
    }

    ret = drv_symc_clear_entry(0);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_clear_entry, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static void drv_symc_free_chn0(hi_void)
{
    hi_u32 used;

    hi_log_func_enter();

    /* set channel to idle */
    used = symc_read(SPACC_SYM_CHN_LOCK);
    CHN_WHO_USED_CLR(used, 0);
    symc_write(SPACC_SYM_CHN_LOCK, used);

    hi_log_func_exit();
    return;
}

static hi_s32 drv_symc_chn0_done_try(void)
{
    sym_chan0_finish_raw_int chn0_status;

    chn0_status.u32 = symc_read(SYM_CHAN0_FINISH_RAW_INT);
    chn0_status.bits.sym_chan0_finish_raw_int &= 0x01; /* check interception */

    /* clear interception */
    symc_write(SYM_CHAN0_FINISH_RAW_INT, chn0_status.u32);
    if (chn0_status.bits.sym_chan0_finish_raw_int == 0) {
        return HI_ERR_CIPHER_BUSY;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_symc_config_chn0(hi_bool decrypt, hi_u32 keysot)
{
    in_sym_chn0_key_ctrl cipher_chn0_key_ctrl;

    hi_log_func_enter();

    /* aes gcm */
    cipher_chn0_key_ctrl.u32 = symc_read(IN_SYM_CHN0_KEY_CTRL);
    cipher_chn0_key_ctrl.bits.sym_chn0_alg_key_len = 0x01;    /* 0x01:  128 bit */
    cipher_chn0_key_ctrl.bits.sym_chn0_alg_data_width = 0x00; /* 0x00:  128 bit */
    cipher_chn0_key_ctrl.bits.sym_chn0_alg_decrypt = (decrypt == HI_TRUE ? 1 : 0);
    cipher_chn0_key_ctrl.bits.sym_chn0_alg_sel = 0x02;  /* 0x02:  aes */
    cipher_chn0_key_ctrl.bits.sym_chn0_alg_mode = 0x0a; /* 0x0a:  gcm */
    cipher_chn0_key_ctrl.bits.sym_chn0_key_chn_id = keysot & 0xFF;
    symc_write(IN_SYM_CHN0_KEY_CTRL, cipher_chn0_key_ctrl.u32);
    hi_log_info("IN_SYM_CHN0_KEY_CTRL[0x%x]: 0x%x\n", IN_SYM_CHN0_KEY_CTRL, cipher_chn0_key_ctrl.u32);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_start_chn0(hi_void)
{
    in_sym_chn0_ctrl cipher_chn0_ctrl;

    hi_log_func_enter();

    /* configure out nodes */
    cipher_chn0_ctrl.u32 = symc_read(IN_SYM_CHN0_CTRL);
    cipher_chn0_ctrl.bits.sym_chn0_req = 1;
    symc_write(IN_SYM_CHN0_CTRL, cipher_chn0_ctrl.u32);
    hi_log_info("IN_SYM_CHN0_CTRL[0x%x]: 0x%x.\n", IN_SYM_CHN0_CTRL, cipher_chn0_ctrl.u32);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_set_chn0_din(const hi_u8 *input_addr, hi_u32 byte_num, symc_node_usage usage)
{
    in_sym_chn0_iv_data_ctrl cipher_chn0_iv_data_ctrl;
    in_sym_chn0_ccm_gcm cipher_chn0_ccm_gcm;
    hi_u32 data[AES_BLOCK_SIZE_IN_WORD] = {0};
    hi_u32 word_cnt;

    hi_log_func_enter();

    hi_log_check_param(byte_num > AES_BLOCK_SIZE_IN_BYTE);

    memcpy(data, input_addr, byte_num);
    for (word_cnt = 0; word_cnt < 4; word_cnt++) {
        symc_write(IN_SYM_CHN0_DATA(word_cnt), data[word_cnt]);
        hi_log_debug("IN_SYM_CHN0_DATA[0x%x]: 0x%x\n", IN_SYM_CHN0_DATA(word_cnt), data[word_cnt]);
    }

    cipher_chn0_iv_data_ctrl.u32 = symc_read(IN_SYM_CHN0_IV_DATA_CTRL);
    cipher_chn0_iv_data_ctrl.bits.sym_chn0_block_len = byte_num;
    cipher_chn0_iv_data_ctrl.bits.sym_chn0_last_block = (usage & SYMC_NODE_USAGE_LAST) >> 1;
    cipher_chn0_iv_data_ctrl.bits.sym_chn0_first_block = usage & SYMC_NODE_USAGE_FIRST;
    symc_write(IN_SYM_CHN0_IV_DATA_CTRL, cipher_chn0_iv_data_ctrl.u32);
    hi_log_info("IN_SYM_CHN0_IV_DATA_CTRL[0x%x]: 0x%x\n", IN_SYM_CHN0_IV_DATA_CTRL, cipher_chn0_iv_data_ctrl.u32);

    cipher_chn0_ccm_gcm.u32 = symc_read(IN_SYM_CHN0_CCM_GCM);
    cipher_chn0_ccm_gcm.bits.sym_chn0_gcm_first = (usage & SYMC_NODE_USAGE_GCM_FIRST) >> 16; /* bit16: gcm first flag */
    cipher_chn0_ccm_gcm.bits.sym_chn0_ccm_gcm_in_flag = (usage & (SYMC_NODE_USAGE_IN_GCM_P |
                                                                  SYMC_NODE_USAGE_IN_GCM_LEN |
                                                                  SYMC_NODE_USAGE_IN_CCM_N |
                                                                  SYMC_NODE_USAGE_IN_CCM_A)) >> 0x3; /* bit3: gcm flag */
    symc_write(IN_SYM_CHN0_CCM_GCM, cipher_chn0_ccm_gcm.u32);
    hi_log_info("IN_SYM_CHN0_CCM_GCM[0x%x] 0x%x.\n", IN_SYM_CHN0_CCM_GCM, cipher_chn0_ccm_gcm.u32);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_wait_chn0_done(hi_void)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 i;
    hi_u32 code = 0;

    hi_log_func_enter();

    for (i = 0; i < CRYPTO_TIME_OUT; i++) {
        ret = drv_symc_chn0_done_try();
        if (HI_SUCCESS == ret) {
            break;
        }
        if (MS_TO_US >= i) {
            crypto_udelay(1); /* short waitting for 1000 us */
        } else {
            crypto_msleep(1); /* long waitting for 5000 ms */
        }
    }

    /* read error code */
    code = symc_read(SYM_CALC_CTRL_CHECK_ERR(SPACC_CPU_CUR));
    if (code != 0x00) {
        hi_log_error("symc error code: 0x%x!!!\n", code);
        return HI_ERR_CIPHER_FAILED_ENCRYPT;
    }

    if (i >= CRYPTO_TIME_OUT) {
        hi_log_error("symc wait done timeout\n");
        hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
        return HI_ERR_CIPHER_TIMEOUT;
    }

    hi_log_info("symc done\n");

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_wait_chn0_idle(hi_void)
{
    hi_u32 i;
    in_sym_chn0_ctrl chn0_ctrl;

    hi_log_func_enter();

    for (i = 0; i < CRYPTO_TIME_OUT; i++) {
        chn0_ctrl.u32 = symc_read(IN_SYM_CHN0_CTRL);
        if (chn0_ctrl.bits.sym_chn0_req == 0) {
            break;
        }
        if (MS_TO_US >= i) {
            crypto_udelay(1); /* short waitting for 1000 us */
        } else {
            crypto_msleep(1); /* long waitting for 5000 ms */
        }
    }
    if (i >= CRYPTO_TIME_OUT) {
        hi_log_error("symc wait done timeout\n");
        hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
        return HI_ERR_CIPHER_TIMEOUT;
    }

    hi_log_info("symc done\n");

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_set_chn0_iv(hi_u8 iv[AES_IV_SIZE])
{
    hi_u32 i;
    hi_u32 buffer[AES_IV_LEN_IN_WORD];
    in_sym_chn0_ccm_gcm cipher_chn0_ccm_gcm;

    hi_log_func_enter();

    /* copy iv data into channel context */
    memcpy(buffer, iv, AES_IV_SIZE);
    for (i = 0; i < AES_IV_SIZE / WORD_WIDTH; i++) {
        symc_write(IN_SYM_CHN0_IV(i), buffer[i]);
        hi_log_info("IN_SYM_CHN0_IV[%d]: 0x%x.\n", i, buffer[i]);
    }

    cipher_chn0_ccm_gcm.u32 = symc_read(IN_SYM_CHN0_CCM_GCM);
    cipher_chn0_ccm_gcm.bits.sym_chn0_gcm_ctr_iv_len = AES_IV_SIZE;
    symc_write(IN_SYM_CHN0_CCM_GCM, cipher_chn0_ccm_gcm.u32);
    hi_log_info("IN_SYM_CHN0_CCM_GCM[0x%x]: 0x%x.\n", IN_SYM_CHN0_CCM_GCM, cipher_chn0_ccm_gcm.u32);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_set_hdcp(hdmi_ram_sel   ram_sel,
                              hi_u32           ram_num,
                              hdcp_rootkey_sel key_sel,
                              hi_u32           key_slot)
{
    hdcp_mode_ctrl hdcp_ctrl;

    hi_log_func_enter();

    hdcp_ctrl.u32 = 0x00;
    symc_write(HDCP_MODE_CTRL, hdcp_ctrl.u32);

    switch (ram_sel) {
        case HDMI_RAM_SEL_TX_14:
            hdcp_ctrl.bits.hdcp_mode_sel = HDCP_MODE_SEL_TX_14;
            hdcp_ctrl.bits.hdcp_tx_mode  = (ram_num == 1) ? 0 : 1; /* range: 0,1 */
            break;
        case HDMI_RAM_SEL_RX_14:
            hdcp_ctrl.bits.hdcp_mode_sel = HDCP_MODE_SEL_RX;
            hdcp_ctrl.bits.hdcp_rx_mode  = HDCP_RX_SEL_14;
            hdcp_ctrl.bits.hdcp_rx_mode |= ram_num << 1;
            break;
        case HDMI_RAM_SEL_RX_22:
            hdcp_ctrl.bits.hdcp_mode_sel = HDCP_MODE_SEL_RX;
            hdcp_ctrl.bits.hdcp_rx_mode  = HDCP_RX_SEL_22;
            hdcp_ctrl.bits.hdcp_rx_mode |= ram_num << 1;
            break;
        default:
            hi_log_error("Error, non-support hdcp ram %d\n", ram_sel);
            return HI_ERR_CIPHER_INVALID_PARA;
    }

    hdcp_ctrl.bits.hdcp_rootkey_sel = key_sel;
    hdcp_ctrl.bits.hdcp_mode_en = 1;
    symc_write(HDCP_MODE_CTRL, hdcp_ctrl.u32);
    hi_log_info("HDCP_MODE_CTRL[0x%x]: 0x%x.\n", HDCP_MODE_CTRL, hdcp_ctrl.u32);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_get_chn0_dout(hi_u8 *output_addr, hi_u32 byte_num)
{
    hi_u32 data[AES_BLOCK_SIZE_IN_WORD];
    hi_u32 word_cnt;

    hi_log_func_enter();

    hi_log_check_param(byte_num < AES_BLOCK_SIZE_IN_BYTE);

    for (word_cnt = 0; word_cnt < AES_BLOCK_SIZE_IN_WORD; word_cnt++) {
        data[word_cnt] = symc_read(CHAN0_CIPHER_DOUT(word_cnt));
        hi_log_debug("CHAN0_CIPHER_DOUT[0x%x]: 0x%x\n", CHAN0_CIPHER_DOUT(word_cnt), data[word_cnt]);
    }
    memcpy(output_addr, data, AES_BLOCK_SIZE_IN_BYTE);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_hdcp_init(hi_cipher_hdcp_attr *attr, hi_u32 decrypt)
{
    hi_s32 ret;

    ret = drv_symc_set_hdcp(attr->ram_sel, attr->ram_num, attr->key_sel, attr->key_slot);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_set_hdcp, ret);
        return ret;
    }

    /* hdcp mode configuration */
    ret = drv_symc_config_chn0(decrypt, attr->key_slot);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_config_chn0, ret);
        return ret;
    }

    ret = drv_symc_set_chn0_iv(attr->iv);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_set_chn0_iv, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_hdcp_compute_a(hi_cipher_hdcp_attr *attr)
{
    hi_s32 ret;
    symc_node_usage usage;

    usage = SYMC_NODE_USAGE_FIRST | SYMC_NODE_USAGE_LAST
        | SYMC_NODE_USAGE_GCM_FIRST | SYMC_NODE_USAGE_IN_GCM_A;
    ret = drv_symc_set_chn0_din(attr->aad, HDCP_AES_GCM_AAD_LEN, usage);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_set_chn0_din, ret);
        return ret;
    }

    ret = drv_symc_start_chn0();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_start_chn0, ret);
        return ret;
    }

    ret = drv_symc_wait_chn0_idle();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_wait_chn0_idle, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_hdcp_compute_p(const hi_u8 *in, hi_u8 *out, hi_u32 len)
{
    hi_s32 ret;
    hi_u32 count;
    symc_node_usage usage;
    hi_u32 i;

    /* compute P */
    count = len / AES_BLOCK_SIZE_IN_BYTE;
    for (i = 0; i < count; i++) {

        /* write input data */
        usage = SYMC_NODE_USAGE_IN_GCM_P;
        if (i == 0) {
            usage |= SYMC_NODE_USAGE_FIRST;
        }
        if (i == count - 1){
            usage |= SYMC_NODE_USAGE_LAST;
        }
        ret = drv_symc_set_chn0_din(in + i * AES_BLOCK_SIZE_IN_BYTE, AES_BLOCK_SIZE_IN_BYTE, usage);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_set_chn0_din, ret);
            return ret;
        }

        ret = drv_symc_start_chn0();
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_start_chn0, ret);
            return ret;
        }

        ret = drv_symc_wait_chn0_idle();
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_wait_chn0_idle, ret);
            return ret;
        }

        if (out == HI_NULL) {
            continue;
        }

        ret = drv_symc_get_chn0_dout(out + i * AES_BLOCK_SIZE_IN_BYTE, AES_BLOCK_SIZE_IN_BYTE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_get_chn0_dout, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 drv_hdcp_compute_clen(hi_cipher_hdcp_attr *attr, hi_u32 len)
{
    hi_s32 ret;
    hi_u32 count;
    hi_u8 buffer[AES_BLOCK_SIZE_IN_BYTE] = {0};
    symc_node_usage usage;

    count = cyp_aead_gcm_clen(buffer, HDCP_AES_GCM_AAD_LEN, len);
    usage = SYMC_NODE_USAGE_FIRST | SYMC_NODE_USAGE_LAST | SYMC_NODE_USAGE_IN_GCM_LEN;
    ret = drv_symc_set_chn0_din(buffer, count, usage);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_set_chn0_din, ret);
        return ret;
    }

    ret = drv_symc_start_chn0();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_start_chn0, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_hdcp_encrypt(hi_cipher_hdcp_attr *attr, const hi_u8 *in, hi_u8 *out, hi_u32 len, hi_u32 decrypt)
{
    hi_u32 buffer[AES_BLOCK_SIZE_IN_WORD] = {0};
    hdcp_debug crc4;
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_param(len % AES_BLOCK_SIZE_IN_BYTE != 0x00);
    hi_log_check_param(attr->alg  != HI_CIPHER_ALG_AES);
    hi_log_check_param(attr->mode != HI_CIPHER_WORK_MODE_GCM);

    hi_log_info("keysel 0x%x, ramsel 0x%x, decrypt 0x%x\n", attr->key_sel, attr->ram_sel, decrypt);

    ret = drv_symc_alloc_chn0();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_alloc_chn0, ret);
        return ret;
    }

    ret = drv_hdcp_init(attr, decrypt);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_hdcp_init, ret);
        drv_symc_free_chn0();
        return ret;
    }

    /* compute A */
    ret = drv_hdcp_compute_a(attr);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_hdcp_compute_a, ret);
        drv_symc_free_chn0();
        return ret;
    }

    /* compute P */
    ret = drv_hdcp_compute_p(in, out, len);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_hdcp_compute_p, ret);
        drv_symc_free_chn0();
        return ret;
    }

    /* compute C_LEN */
    ret = drv_hdcp_compute_clen(attr, len);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_hdcp_compute_p, ret);
        drv_symc_free_chn0();
        return ret;
    }

    ret = drv_symc_wait_chn0_done();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_wait_chn0_done, ret);
        drv_symc_free_chn0();
        return ret;
    }

    /* check tag */
    ret = drv_aead_get_tag(0, buffer);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_aead_get_tag, ret);
        drv_symc_free_chn0();
        return ret;
    }

    if (decrypt == HI_FALSE) {
        memcpy(attr->tag, buffer, AEAD_TAG_SIZE);
    } else {
        if (osal_memncmp(buffer, AEAD_TAG_SIZE, attr->tag, AEAD_TAG_SIZE) != 0) {
                hi_log_error("hdcp decrypt tag check failed\n");
                drv_symc_free_chn0();
                return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    crc4.u32 = symc_read(HDCP_DEBUG);
    hi_log_info("CRC4: 0x%x, u32 0x%x\n", crc4.bits.hdcp_crc4, crc4.u32);

    drv_symc_free_chn0();

    hi_log_func_exit();
    return HI_SUCCESS;
}

/******* proc function begin ********/
#ifdef HI_PROC_SUPPORT
void drv_hdcp_proc_status(hdcp_module_status *module)
{
    hdcp_mode_ctrl ctrl;
    hdcp_debug crc4;

    ctrl.u32 = symc_read(HDCP_MODE_CTRL);

    module->rx_read = ctrl.bits.hdcp_rx_mode;
    module->tx_read = ctrl.bits.hdcp_tx_mode;
    module->key_sel = ctrl.bits.hdcp_rootkey_sel;

    /* CRC4       : x4 + 1
     * polynomial : 0x01
     * init       : 0x00
     * out-xor    : 0x05
     * .e.g: crc4(d4 3f b6 98) = 0x0C
     */
    crc4.u32 = symc_read(HDCP_DEBUG);
    module->crc4 = crc4.bits.hdcp_crc4;

}
#endif
/******* proc function end ********/

/** @} */ /** <!-- ==== API declaration end ==== */

#endif  // End of CHIP_HDCP_VER_V300
