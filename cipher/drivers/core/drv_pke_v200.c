/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_pke_v200
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_pke_v200.h"
#include "drv_pke.h"
#include "drv_trng.h"

#ifdef CHIP_PKE_VER_V200

/*************************** Structure Definition ****************************/
/** \addtogroup     pke */
/** @{ */ /** <!-- [pke] */

/* ! Define the time out */
#define PKE_TIME_OUT      6000

/* ! Define the start code */
#define PKE_START_CODE 0x05

/* ! Define the key type */
#define PKE_KEY_CFG_PRIVATE 0x05
#define PKE_KEY_CFG_PUBLIC  0x0a

/* ! Define the int code */
#define PKE_INT_NONE 0x0a
#define PKE_INT_DONE 0x05

#define PKE_MUL_DOT_SIZE_MIN 0x20

/* crc 16 */
#define CRC16_TABLE_SIZE 256
#define U16_MSB          0x8000
#define CRC16_POLYNOMIAL 0x1021
#define BYTE_BITS        0x08
#define BYTE_MASK        0xFF

#define PKE_RAM_SECTION_SIZE_IN_BYTE (0x50)
#define PKE_RAM_RANG_SIZE            (0x400)
#define PKE_MUL_RESULT_LEVEL         (0x100)

#define CHECK_HIGH_TWO_BIT (0xC0)

const char *err_string[PKE_FAIL_FLAG_CNT] = {
    "The mod number is even when mod-inverse operation",
    "Alloc random number failed",
    "The result of mod add/multiply is infinity"
    "Point multiplication or point addition result is infinity point",
    "The input point is not on the elliptic curve",
    "Attacked by DFX",
    "Attacked by DFA",
    "Calling unit module failed",
};

/* ! Define the context of pke */
typedef struct {
    hi_u32 done;             /* !<  calculation finish flag */
    hi_u32 pke_sub_ver;      /* !<  pke support mask? */
    crypto_queue_head queue; /* !<  quene list */
    hi_u32 attacked;         /* !<  if be attacked, don't compute any more */
} pke_hard_context;

static pke_hard_context pke_info;

/* ! pke already initialize or not */
static hi_u32 pke_initialize = HI_FALSE;

/* current cpu */
#define PKE_CPU_CUR (crypto_is_sec_cpu() ? PKE_LOCK_TEE : PKE_LOCK_REE)

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      pke */
/** @{ */ /** <!--[pke] */

static uint16_t crc_table(uint8_t byte)
{
    uint16_t nRemainder;
    uint16_t m = 0;

    nRemainder = (uint16_t)byte << BYTE_BITS;
    for (m = BYTE_BITS; m > 0; m--) {
        if (nRemainder & U16_MSB) {
            nRemainder = (nRemainder << 1) ^ CRC16_POLYNOMIAL;
        } else {
            nRemainder = (nRemainder << 1);
        }
    }
    return nRemainder;
}

static uint16_t drv_pke_crc16_key(uint16_t initial, uint8_t *key, uint32_t klen)
{
    uint32_t i, j;
    uint16_t crc16;
    uint8_t *p = NULL;

    crc16 = initial;

    for (i = klen; i >= PKE_LEN_BLOCK_IN_BYTE; i -= PKE_LEN_BLOCK_IN_BYTE) {
        p = &key[i - PKE_LEN_BLOCK_IN_BYTE];
        for (j = 0; j < PKE_LEN_BLOCK_IN_BYTE; j++) {
            crc16 = (crc16 << BYTE_BITS) ^ crc_table(((crc16 >> BYTE_BITS) ^ p[j]) & BYTE_MASK);
        }
    }

    return crc16;
}

/* *** crc = CRC16(crc, randnum) *** */
static uint16_t drv_pke_crc16_random(uint16_t initial, pke_block *random)
{
    uint32_t i;
    uint16_t crc16 = initial;

    for (i = 0; i < PKE_LEN_BLOCK_IN_BYTE; i++) {
        crc16 = (crc16 << BYTE_BITS) ^ crc_table(((crc16 >> BYTE_BITS) ^ random->byte[i]) & BYTE_MASK);
    }

    return crc16;
}

/* *** crc = CRC16(crc, 0) *** */
static uint16_t drv_pke_crc16_padding(uint16_t initial, pke_block *padding, uint32_t len)
{
    uint32_t i;
    uint16_t crc16;
    uint8_t byte = 0;

    crc16 = initial;

    for (i = 0; i < len; i++) {
        byte = padding->byte[i & PKE_LEN_BLOCK_MASK];
        crc16 = (crc16 << BYTE_BITS) ^ crc_table(((crc16 >> BYTE_BITS) ^ byte) & BYTE_MASK);
    }

    return crc16;
}

static hi_u32 drv_pke_done_try(void)
{
    pke_int_nomask_status int_raw;
    hi_u32 mask;

    int_raw.u32 = pke_read(PKE_INT_NOMASK_STATUS);
    hi_log_debug("pke int_raw 0x%x\n", int_raw.u32);

    mask = (int_raw.bits.alarm_int_nomask == PKE_INT_DONE)
           || (int_raw.bits.finish_int_nomask == PKE_INT_DONE);

    /* Clean raw int */
    if (mask) {
        int_raw.bits.finish_int_nomask = 1;
        int_raw.bits.alarm_int_nomask = 1;
        pke_write(PKE_INT_NOMASK_STATUS, int_raw.u32);
    }

    return mask;
}

#ifdef CRYPTO_OS_INT_SUPPORT
static hi_u32 drv_pke_done_notify(void)
{
    pke_int_status int_st;
    pke_int_nomask_status int_raw;

    int_st.u32 = pke_read(PKE_INT_STATUS);
    int_raw.u32 = 0x00;

    int_raw.bits.finish_int_nomask = 1;
    int_raw.bits.alarm_int_nomask = 1;

    /* Clean raw int */
    pke_write(PKE_INT_NOMASK_STATUS, int_raw.u32);

    return (int_st.bits.alarm_int == PKE_INT_DONE)
           || (int_st.bits.finish_int == PKE_INT_DONE);
}

/* ! pke interrupt process function */
static irqreturn_t pke_interrupt_isr(hi_s32 irq, void *devId)
{
    hi_u32 mask;

    crypto_unused(irq);

    mask = drv_pke_done_notify();

    if (mask != 0x00) {
        pke_info.done = HI_TRUE;
        hi_log_debug("pke wake up\n");
        crypto_queue_wake_up(&pke_info.queue);
    }

    return IRQ_HANDLED;
}

/* ! pke register interrupt process function */
static hi_s32 drv_pke_register_interrupt(void)
{
    hi_s32 ret;
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    const char *name;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_PKE, &int_valid, &int_num, &name);
    if (int_valid == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* request irq */
    ret = crypto_request_irq(int_num, pke_interrupt_isr, name);
    if (ret != HI_SUCCESS) {
        hi_log_error("Irq request failure, name = %s", name);
        hi_log_print_err_code(HI_ERR_CIPHER_REGISTER_IRQ);
        return ret;
    }

    /* initialize queue list */
    crypto_queue_init(&pke_info.queue);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* ! pke unregister interrupt process function */
static void drv_pke_unregister_interrupt(void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    const char *name = HI_NULL;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_PKE, &int_valid, &int_num, &name);
    if (int_valid == HI_FALSE) {
        return;
    }

    /* free irq */
    hi_log_debug("pke free irq, num %d, name %s\n", int_num, name);
    crypto_free_irq(int_num, name);

    hi_log_func_exit();

    return;
}
#endif

/* ! set interrupt */
static void pke_set_interrupt(void)
{
    pke_int_enable pke_int_en;

    pke_int_en.u32 = pke_read(PKE_INT_ENABLE);

    pke_int_en.bits.int_enable = 0;
    pke_int_en.bits.finish_int_enable = 0;
    pke_int_en.bits.alarm_int_enable = 0;

#ifdef CRYPTO_OS_INT_SUPPORT
    {
        hi_u32 int_valid = 0;
        hi_u32 int_num = 0;

        (void)module_get_attr(CRYPTO_MODULE_ID_PKE, &int_valid, &int_num, HI_NULL);
        if (int_valid == HI_TRUE) {
            pke_int_en.bits.int_enable = 1;
            pke_int_en.bits.finish_int_enable = 1;
            pke_int_en.bits.alarm_int_enable = 1;
        }
    }
#endif

    pke_write(PKE_INT_ENABLE, pke_int_en.u32);
    hi_log_debug("PKE_INT_EN: 0x%x\n", pke_int_en.u32);

    return;
}

hi_s32 drv_pke_init(void)
{
    pke_power_on power;
    pke_noise_en noise;

    hi_log_func_enter();

    hi_log_debug("enable pke\n");

    if (pke_initialize) {
        return HI_SUCCESS;
    }

    module_enable(CRYPTO_MODULE_ID_PKE);

#ifdef CRYPTO_OS_INT_SUPPORT
    {
        hi_s32 ret;

        hi_log_debug("pke register interrupt function\n");
        ret = drv_pke_register_interrupt();
        if (ret != HI_SUCCESS) {
            hi_log_error("error, register interrupt failed\n");
            hi_log_print_func_err(drv_pke_register_interrupt, ret);
            module_disable(CRYPTO_MODULE_ID_PKE);
            return ret;
        }
    }
#endif

    pke_initialize = HI_TRUE;
    pke_info.done = HI_TRUE;
    pke_info.attacked = HI_FALSE;

    power.u32 = pke_read(PKE_POWER_EN);
    power.bits.power_en = PKE_SECURE_TRUE;
    pke_write(PKE_POWER_EN, power.u32);

    noise.u32 = pke_read(PKE_NOISE_EN);
    noise.bits.noise_en = PKE_SECURE_TRUE;
    pke_write(PKE_NOISE_EN, noise.u32);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_pke_deinit(void)
{
    hi_log_debug("disable pke\n");

    if (pke_initialize == HI_FALSE) {
        return HI_SUCCESS;
    }

#ifdef CRYPTO_OS_INT_SUPPORT
    drv_pke_unregister_interrupt();
#endif

    pke_initialize = HI_FALSE;

    return HI_SUCCESS;
}

 hi_s32 drv_pke_lock(void)
{
    pke_lock_ctrl lock_ctrl;
    pke_lock_status lock_status;
    hi_u32 lock_code = PKE_LOCK_TEE;
    uint32_t i;

    if (crypto_is_sec_cpu() == HI_FALSE) {
        lock_code = PKE_LOCK_REE;
    }

    /* lock pke */
    for (i = 0; i < PKE_TIME_OUT; i++) {
        /* pke unlock ? */
        lock_status.u32 = pke_read(PKE_LOCK_STATUS);
        hi_log_debug("-PKE_LOCK_STATUS[0x%x] 0x%x\n", PKE_LOCK_STATUS, lock_status.u32);
        if (lock_status.bits.pke_lock_stat != PKE_LOCK_UNLOCK) {
            crypto_msleep(1);
            continue;
        }

        /* try to lock pke */
        lock_ctrl.u32 = pke_read(PKE_LOCK_CTRL);
        lock_ctrl.bits.pke_lock_type = PKE_LOCK_TYPE_LOCK;
        lock_ctrl.bits.pke_lock = 1;
        pke_write(PKE_LOCK_CTRL, lock_ctrl.u32);

        /* check lock result */
        lock_status.u32 = pke_read(PKE_LOCK_STATUS);
        hi_log_debug("+PKE_LOCK_STATUS[0x%x] 0x%x\n", PKE_LOCK_STATUS, lock_status.u32);
        if (lock_status.bits.pke_lock_stat == lock_code) {
            break;
        }
    }

    if (i >= PKE_TIME_OUT) {
        hi_log_error("pke lock timeout\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

 hi_s32 drv_pke_unlock(void)
{
    pke_lock_ctrl lock_ctrl;
    pke_lock_status lock_status;

    /* try to unlock pke */
    lock_ctrl.u32 = pke_read(PKE_LOCK_CTRL);
    lock_ctrl.bits.pke_lock_type = PKE_LOCK_TYPE_UNLOCK;
    lock_ctrl.bits.pke_lock = 1;
    pke_write(PKE_LOCK_CTRL, lock_ctrl.u32);

    /* check unlock result */
    lock_status.u32 = pke_read(PKE_LOCK_STATUS);
    if ((lock_status.bits.pke_lock_stat != PKE_LOCK_UNLOCK) ||
        (lock_status.bits.pke_unlock_fail == 1)) {
        hi_log_error("pke unlock timeout\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


hi_s32 drv_pke_resume(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = drv_pke_lock();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_lock, ret);
        return ret;
    }

    hi_log_debug("pke interrupt configure\n");
    pke_set_interrupt();

    hi_log_func_exit();
    return HI_SUCCESS;
}

void drv_pke_suspend(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = drv_pke_unlock();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_unlock, ret);
        return;
    }

    hi_log_func_exit();
    return;
}

static void drv_pke_set_ram(pke_ram_type type, hi_u32 section,
                            const hi_u8 *ram, hi_u32 klen,
                            pke_block *padding)
{
    hi_u32 start;
    hi_u32 end;
    hi_u32 val;
    hi_u32 i;

    start = PKE_MRAM + PKE_RAM_RANG_SIZE * type
            + section * PKE_RAM_SECTION_SIZE_IN_BYTE;
    end = start + PKE_RAM_SECTION_SIZE_IN_BYTE;

    for (i = klen; i >= WORD_WIDTH; i -= WORD_WIDTH) {
        val  = ram[i - 4] << 24 ;  /* offset 4: bit 31~24 */
        val |= ram[i - 3] << 16 ;  /* offset 3: bit 23~16 */
        val |= ram[i - 2] << 8 ;   /* offset 2: bit 15~8  */
        val |= ram[i - 1];         /* offset 1: bit 7~0   */
        pke_write(start, val);
        start += WORD_WIDTH;
    }

    /* padding */
    if (padding != HI_NULL) {
        i = 1;
        while (start < end) {
            val = cipher_cpu_to_be32(padding->word[i]);
            pke_write(start, val);
            start += WORD_WIDTH;
            i ^= 0x01;
        }
    }

    return;
}

static void drv_pke_set_block(pke_ram_type type, hi_u32 section,
                              const pke_block *block, hi_u32 power, hi_u32 klen)
{
    hi_u32 i;
    hi_u32 start;

    start = PKE_MRAM + PKE_RAM_RANG_SIZE * type
            + section * PKE_RAM_SECTION_SIZE_IN_BYTE;

    for (i = 0; i < klen; i += WORD_WIDTH) {
        pke_write(start + i, 0x00);
    }

    pke_write(start + power, block->word[1]);
    pke_write(start + power + WORD_WIDTH, block->word[0]);

    return;
}

static void drv_pke_set_crt_p(hi_u8 *p, hi_u32 klen)
{
    hi_u32 i;
    hi_u32 val;
    hi_u32 start = PKE_PRAM;

    for (i = klen; i >= WORD_WIDTH; i -= WORD_WIDTH) {
        val  = p[i - 4] << 24 ;  /* offset 4: bit 31~24 */
        val |= p[i - 3] << 16 ;  /* offset 3: bit 23~16 */
        val |= p[i - 2] << 8 ;   /* offset 2: bit 15~8  */
        val |= p[i - 1];         /* offset 1: bit 7~0   */
        pke_write(start, val);
        start += WORD_WIDTH;
    }

    return;
}

static hi_s32 drv_pke_check_pub_e(hi_u32 e, hi_u32 klen)
{
    hi_u32 val;
    hi_u32 i;

    /* first, get e to ram[0], ram[0] is least significant byte */
    val = pke_read(PKE_NRAM);
    if (val != e) {
        hi_log_error("The e 0x%x is error during rsa key generation\n", e);
        return HI_FAILURE;
    }

    /* second, get 0x00 to left ram */
    for (i = WORD_WIDTH; i < klen; i += WORD_WIDTH) {
        val = pke_read(PKE_NRAM + i);
        if (val != 0) {
            hi_log_error("The e is error during rsa key generation, addr 0x%x, val 0x%x\n", PKE_NRAM + i, val);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static void drv_pke_get_ram(pke_ram_type type, hi_u32 section, hi_u8 *ram, hi_u32 klen)
{
    hi_u32 addr;
    hi_u32 val = 0;
    hi_u32 len = klen;
    hi_u32 i;

    addr = PKE_MRAM + PKE_RAM_RANG_SIZE * type + section * PKE_RAM_SECTION_SIZE_IN_BYTE;

    for (i = len; i >= WORD_WIDTH; i -= WORD_WIDTH) {
        val = pke_read(addr + i - WORD_WIDTH);
        ram[len - i + 0] = (val >> 24) & 0xFF; /* byte 0: bit 31~24 */
        ram[len - i + 1] = (val >> 16) & 0xFF; /* byte 1: bit 23~16 */
        ram[len - i + 2] = (val >> 8) & 0xFF;  /* byte 2: bit 15~8  */
        ram[len - i + 3] = (val) & 0xFF;       /* byte 3: bit 7~0   */
    }
    return;
}

static void drv_pke_set_mode(pke_mode mode, hi_u32 len)
{
    pke_work_mode work;

    work.u32 = 0x00;
    work.bits.mode_sel = mode;

    if (mode == PKE_MODE_CLR_RAM) {
        work.bits.data_len = PKE_DATA_LEN_BYTE_512;
        work.bits.ram_sel = PKE_CLR_RAM_SEL_ALL;
    } else {
        work.bits.ram_sel = PKE_CLR_RAM_SEL_NONE;
        work.bits.data_len = len / PKE_LEN_BLOCK_IN_BYTE;
    }

    pke_write(PKE_WORK_MODE, work.u32);
    hi_log_debug("PKE_WORK_MODE 0x%x\n", work.u32);

    /* default: public key */
    pke_write(PKE_CFG_KEY_FLAG, PKE_KEY_CFG_PUBLIC);

    return;
}

static void drv_pke_start(void)
{
    pke_start start;

    hi_log_func_enter();

    pke_write(PKE_CFG_KEY_FLAG, PKE_KEY_CFG_PUBLIC);

    start.u32 = 0x00;
    start.bits.pke_start = PKE_START_CODE;
    pke_write(PKE_START, start.u32);
    hi_log_debug("PKE_START 0x%x\n", start.u32);

    hi_log_func_exit();

    return;
}

static hi_s32 drv_pke_wait_free(void)
{
    hi_u32 i = 0;
    pke_busy busy;

    hi_log_func_enter();

    /* wait ready */
    for (i = 0; i < PKE_TIME_OUT; i++) {
        busy.u32 = pke_read(PKE_BUSY);
        if (!busy.bits.pke_busy) {
            break;
        }
        crypto_msleep(1);
    }

    if (i >= PKE_TIME_OUT) {
        hi_log_error("error, pke wait free timeout\n");
        hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
        return HI_ERR_CIPHER_TIMEOUT;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_pke_error_code(void)
{
    pke_failure_flag code;
    pke_result_flag result;
    hi_u32 i;

    hi_log_func_enter();

    result.u32 = pke_read(PKE_RESULT_FLAG);
    if (result.bits.pke_result_flag != PKE_RESULT_FLAG_OK) {
        code.u32 = pke_read(PKE_FAILURE_FLAG);
        for (i = 0; i < PKE_FAIL_FLAG_CNT; i++) {
            if (code.bits.pke_failure_flag & (1 << i)) {
                hi_log_error("PKE operation failed: %s\n", err_string[i]);
            }
        }
        hi_log_error("Hardware Error Code: 0x%x\n", code.u32);
        pke_info.attacked = HI_TRUE;
        hi_log_print_err_code(HI_ERR_CIPHER_HARD_STATUS);
        return HI_ERR_CIPHER_HARD_STATUS;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* wait pke done */
static hi_s32 drv_pke_wait_done(void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    hi_u32 i;
    hi_s32 ret;
    const char *name = HI_NULL;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_PKE, &int_valid, &int_num, &name);

#ifdef CRYPTO_OS_INT_SUPPORT
    /* interrupt support, wait irq */
    if (int_valid) {
        /* wait interrupt */
        ret = crypto_queue_wait_timeout(&pke_info.queue, crypto_queue_callback_func, &pke_info.done, PKE_TIME_OUT);
        if (ret == 0x00) {
            hi_log_error("wait done timeout\n");
            hi_log_print_func_err(crypto_queue_wait_timeout, HI_ERR_CIPHER_TIMEOUT);
            hi_log_error("PKE_INT_ENABLE[0x%x]            0x%x\n", PKE_INT_ENABLE, pke_read(PKE_INT_ENABLE));
            hi_log_error("PKE_INT_STATUS[0x%x]            0x%x\n", PKE_INT_STATUS, pke_read(PKE_INT_STATUS));
            hi_log_error("pke PKE_INT_NOMASK_STATUS[0x%x] 0x%x\n", PKE_INT_NOMASK_STATUS,
                pke_read(PKE_INT_NOMASK_STATUS));

            return HI_ERR_CIPHER_TIMEOUT;
        }
    } else /* interrupt unsupport, query the raw interrupt flag */
#endif
    {
        for (i = 0; i < PKE_TIME_OUT; i++) {
            if (drv_pke_done_try()) {
                break;
            }
            if (i <= MS_TO_US) {
                crypto_udelay(1); /* short waitting for 1000 us */
            } else {
                crypto_msleep(1); /* long waitting for 5000 ms */
            }
        }

        if (i >= PKE_TIME_OUT) {
            hi_log_error("pke wait done timeout\n");
            hi_log_print_func_err(crypto_queue_wait_timeout, HI_ERR_CIPHER_TIMEOUT);
            return HI_ERR_CIPHER_TIMEOUT;
        }
    }

    ret = drv_pke_error_code();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_error_code, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static void drv_pke_set_key(const hi_u8 *inkey, hi_u8 *outkey, hi_u32 klen,
                            pke_block *random, hi_u32 pad_len)
{
    hi_u32 i;
    hi_u16 crc16 = 0;

    if (random != HI_NULL) { /* private key */

        for (i = 0; i < klen; i++) {
            outkey[i] = inkey[i] ^ random->byte[i & PKE_LEN_BLOCK_MASK];
        }

        crc16 = drv_pke_crc16_random(0x0000, random);
        crc16 = drv_pke_crc16_key(crc16, outkey, klen);
        crc16 = drv_pke_crc16_padding(crc16, random, pad_len);

        hi_log_debug("PKE_KEY_RANDOM_1 0x%x\n", random->word[1]);
        hi_log_debug("PKE_KEY_RANDOM_2 0x%x\n", random->word[0]);
        hi_log_debug("CRC16 0x%x\n", crc16);

        /* set Random */
        pke_write(PKE_KEY_RANDOM_1, cipher_cpu_to_be32(random->word[1]));
        pke_write(PKE_KEY_RANDOM_2, cipher_cpu_to_be32(random->word[0]));

        /* set src16 */
        pke_write(PKE_KEY_CRC, crc16);

        /* private key */
        pke_write(PKE_CFG_KEY_FLAG, PKE_KEY_CFG_PRIVATE);
    } else { /* not private key */
        pke_write(PKE_CFG_KEY_FLAG, PKE_KEY_CFG_PUBLIC);
        (void)memcpy_s(outkey, klen, inkey, klen);
    }

    /* select cpu key */
    pke_write(PKE_OTP_KEY_SEL, PKE_SECURE_FALSE);

    return;
}

/* c = a + b mod p */
hi_s32 drv_pke_add_mod(const hi_u8 *a, const hi_u8 *b, const hi_u8 *p, hi_u8 *c, hi_u32 klen)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    crypto_assert(a != HI_NULL);
    crypto_assert(b != HI_NULL);
    crypto_assert(p != HI_NULL);
    crypto_assert(c != HI_NULL);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    drv_pke_set_mode(PKE_MODE_ADD_MOD, klen);

    drv_pke_set_ram(PKE_RAM_TYPE_NRAM, 0, a, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, b, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_RRAM, 0, p, klen, HI_NULL);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mod add failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_MRAM, 0, c, klen);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* c = a - b mod p */
hi_s32 drv_pke_sub_mod(const hi_u8 *a, const hi_u8 *b, const hi_u8 *p, hi_u8 *c, hi_u32 klen)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    crypto_assert(a);
    crypto_assert(b != HI_NULL);
    crypto_assert(p != HI_NULL);
    crypto_assert(c != HI_NULL);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    drv_pke_set_mode(PKE_MODE_SUB_MOD, klen);

    drv_pke_set_ram(PKE_RAM_TYPE_NRAM, 0, a, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, b, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_RRAM, 0, p, klen, HI_NULL);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mod sub failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_MRAM, 0, c, klen);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* c = a * b mod p */
hi_s32 drv_pke_mul_mod(const hi_u8 *a, const hi_u8 *b, const hi_u8 *p, hi_u8 *c, hi_u32 klen, hi_u32 private)
{
    hi_s32 ret;
    hi_u8 *key = HI_NULL;
    pke_block random;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    crypto_assert(a);
    crypto_assert(b != HI_NULL);
    crypto_assert(c != HI_NULL);

    key = crypto_calloc(1, klen);
    if (key == HI_NULL) {
        hi_log_print_func_err(crypto_calloc, HI_LOG_ERR_MEM);
        return HI_LOG_ERR_MEM;
    }

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        crypto_free(key);
        return ret;
    }

    drv_pke_set_mode(PKE_MODE_MUL_MOD, klen);

    if (private == HI_TRUE) {
        random.word[0] = get_rand();
        random.word[1] = get_rand();
        drv_pke_set_key(b, key, klen, &random, 0x00);
    } else {
        random.word[0] = 0x00000000;
        random.word[1] = 0x00000000;
        drv_pke_set_key(b, key, klen, HI_NULL, 0x00);
    }

    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, 0, a, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_NRAM, 0, p, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, key, klen, HI_NULL);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mod mul failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        crypto_free(key);
        return ret;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, c, klen);

    crypto_free(key);
    key = HI_NULL;

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* c = a^-1 mod p */
hi_s32 drv_pke_inv_mod(const hi_u8 *a, const hi_u8 *p, hi_u8 *c, hi_u32 klen)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    drv_pke_set_mode(PKE_MODE_MINV_MOD, klen);

    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, 0, a, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, p, klen, HI_NULL);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mod inv failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, c, klen);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* c = a mod p */
hi_s32 drv_pke_mod(const hi_u8 *a, const hi_u8 *p, hi_u8 *c, hi_u32 klen)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    crypto_assert(a);
    crypto_assert(p != HI_NULL);
    crypto_assert(c != HI_NULL);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    drv_pke_set_mode(PKE_MODE_MOD, klen);

    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, 0, a, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, p, klen, HI_NULL);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mod failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, c, klen);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* c = a ^ mod p */
hi_s32 drv_pke_mod_block(const pke_block *a, hi_u32 power, const hi_u8 *p, hi_u32 plen, hi_u8 *c)
{
    hi_s32 ret;
    hi_u32 klen;
    pke_block padding;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    crypto_assert(a);
    crypto_assert(p != HI_NULL);
    crypto_assert(c != HI_NULL);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    padding.word[0] = 0x00;
    padding.word[1] = 0x00;
    klen = cipher_max(power + PKE_LEN_BLOCK_IN_BYTE, plen);

    drv_pke_set_mode(PKE_MODE_MOD, klen);
    drv_pke_set_block(PKE_RAM_TYPE_MRAM, 0, a, power, klen);
    drv_pke_set_block(PKE_RAM_TYPE_KRAM, 0, &padding, 0, klen); /* clear KRAM */
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, p, plen, HI_NULL);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mod failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, c, plen);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* *** c = a * b *** */
hi_s32 drv_pke_mul(const hi_u8 *a, const hi_u8 *b, hi_u8 *c, hi_u32 klen)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    crypto_assert(a);
    crypto_assert(b != HI_NULL);
    crypto_assert(c != HI_NULL);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    drv_pke_set_mode(PKE_MODE_MUL, klen);

    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, a, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_RRAM, 0, b, klen, HI_NULL);
    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mul failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    if (klen <= PKE_MUL_RESULT_LEVEL) {
        drv_pke_get_ram(PKE_RAM_TYPE_MRAM, 0, c, klen * 2); /* 2 klen for mul result */
    } else {
        drv_pke_get_ram(PKE_RAM_TYPE_MRAM, 0, c, PKE_MUL_RESULT_LEVEL);
        drv_pke_get_ram(PKE_RAM_TYPE_NRAM, 0, c + PKE_MUL_RESULT_LEVEL,
                        klen - PKE_MUL_RESULT_LEVEL);
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* *** R = k . P(x,y) *** */
hi_s32 drv_pke_mul_dot(const hi_u8 *k,
                       const hi_u8 *px,
                       const hi_u8 *py,
                       hi_u8 *rx,
                       hi_u8 *ry,
                       hi_u32 klen,
                       hi_u32 private,
                       ecc_param_t *ecp_id)
{
    hi_s32 ret;
    hi_u8 *key = HI_NULL;
    hi_u32 pad_len;
    hi_u32 id;
    pke_block random;
    pke_block zero;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);

    key = crypto_calloc(1, klen);
    if (key == HI_NULL) {
        hi_log_print_func_err(crypto_calloc, HI_LOG_ERR_MEM);
        return HI_LOG_ERR_MEM;
    }

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        crypto_free(key);
        key = HI_NULL;
        return ret;
    }

    zero.word[0] = 0x00000000;
    zero.word[1] = 0x00000000;
    pad_len = PKE_RAM_SECTION_SIZE_IN_BYTE - klen;

    drv_pke_set_mode(PKE_MODE_MUL_DOT, klen);

    if (private == HI_TRUE) {
        random.word[0] = get_rand();
        random.word[1] = get_rand();
        drv_pke_set_key(k, key, klen, &random, pad_len);
    } else {
        random.word[0] = 0x00000000;
        random.word[1] = 0x00000000;
        drv_pke_set_key(k, key, klen, HI_NULL, pad_len);
    }

    id = 0;
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id++, px, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id++, py, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id++, ecp_id->gx, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id++, ecp_id->gy, klen, &zero);

    id = 0;
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, id++, key, klen, &random);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, id++, ecp_id->n, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, id++, ecp_id->b, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, id++, ecp_id->a, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, id++, ecp_id->p, klen, &zero);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 mul dot failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        crypto_free(key);
        return ret;
    }

    if (rx != HI_NULL) {
        drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, rx, klen);
    }
    if (ry != HI_NULL) {
        drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 1, ry, klen);
    }

    crypto_free(key);
    key = HI_NULL;
    hi_log_func_exit();
    return HI_SUCCESS;
}

/* *** C = S(x,y) + R(x,y) *** */
hi_s32 drv_pke_add_dot(const hi_u8 *sx,
                       const hi_u8 *sy,
                       const hi_u8 *rx,
                       const hi_u8 *ry,
                       hi_u8 *cx,
                       hi_u8 *cy,
                       hi_u32 klen,
                       ecc_param_t *ecp_id)
{
    hi_s32 ret;
    hi_u32 id;
    pke_block zero;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    crypto_assert(sx);
    crypto_assert(sy);
    crypto_assert(rx);
    crypto_assert(ry);
    crypto_assert(cx);
    crypto_assert(cy);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    zero.word[0] = 0x00000000;
    zero.word[1] = 0x00000000;

    drv_pke_set_mode(PKE_MODE_ADD_DOT, klen);

    id = 0;
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id++, sx, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id++, sy, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id++, rx, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, id, ry, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, id++, ecp_id->a, klen, &zero);
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, id++, ecp_id->p, klen, &zero);

    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 dot add failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, cx, klen);
    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 1, cy, klen);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_pke_clean_ram(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);

    pke_info.done = HI_FALSE;

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 wait ready failed!\n");
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    drv_pke_set_mode(PKE_MODE_CLR_RAM, 0);
    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("SM2 clean ram failed\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_u32 drv_rsa_get_klen(rsa_key_width width)
{
    hi_u32 klen = 0x00;

    switch (width) {
        case RSA_KEY_WIDTH_1024: {
            klen = RSA_KEY_LEN_1024;
            break;
        }
        case RSA_KEY_WIDTH_2048: {
            klen = RSA_KEY_LEN_2048;
            break;
        }
        case RSA_KEY_WIDTH_3072: {
            klen = RSA_KEY_LEN_3072;
            break;
        }
        case RSA_KEY_WIDTH_4096: {
            klen = RSA_KEY_LEN_4096;
            break;
        }
        default: {
            hi_log_error("error, nonsupport RSA width %d\n", width);
            klen = 0;
            break;
        }
    }

    return klen;
}

/* c=m^e mod n */
hi_s32 drv_ifep_rsa_exp_mod(hi_u8 *n, hi_u8 *k, hi_u8 *in, hi_u8 *out, rsa_key_width width)
{
    hi_s32 ret;
    hi_s32 ret_exit;
    hi_u8 *key = HI_NULL;
    pke_block random;
    hi_u32 klen;

    hi_log_func_enter();

    crypto_assert(n);
    crypto_assert(k);
    crypto_assert(in);
    crypto_assert(out);

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    pke_info.done = HI_FALSE;

    klen = drv_rsa_get_klen(width);
    if (klen == 0) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    key = crypto_calloc(1, klen * 2); /* 2 buffer for n and k */
    if (key == HI_NULL) {
        hi_log_print_func_err(crypto_calloc, HI_LOG_ERR_MEM);
        return HI_LOG_ERR_MEM;
    }

    ret = drv_pke_resume();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_resume, ret);
        crypto_free(key);
        key = HI_NULL;
        return ret;
    }

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_wait_free, ret);
        goto _exit;
    }

    random.word[0] = get_rand();
    random.word[1] = get_rand();

    drv_pke_set_mode(PKE_MODE_EXP_MOD, klen);

    hi_log_info("[RSA] compute exp-mod, klen 0x%x\n", klen);

    /* key xor random */
    memcpy(key, k, klen);
    memcpy(key + klen, n, klen);
    drv_pke_set_key(key, key, klen + klen, &random, 0);

    /* set data */
    drv_pke_set_ram(PKE_RAM_TYPE_MRAM, 0, in, klen, HI_NULL);
    drv_pke_set_ram(PKE_RAM_TYPE_NRAM, 0, key + klen, klen, HI_NULL); /* n */
    drv_pke_set_ram(PKE_RAM_TYPE_KRAM, 0, key, klen, HI_NULL);        /* k */

    /* start */
    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("rsa exp mod failed.\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        goto _exit;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, out, klen);
_exit:
    ret_exit = drv_pke_clean_ram();
    if (ret_exit != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_clean_ram, ret_exit);
    }

    drv_pke_suspend();
    crypto_free(key);
    key = HI_NULL;

    hi_log_func_exit();
    return ret;
}

/* rsa key generation with public key */
hi_s32 drv_rsa_pub_key(hi_u32 e, hi_u32 klen, cryp_rsa_key *key)
{
    hi_s32 ret_exit;
    hi_s32 ret;
    pke_block block;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    pke_info.done = HI_FALSE;

    if ((e & 0x00000001) == 0) {
        hi_log_error("Invalid input public key, e should be an odd number.\n");
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    ret = drv_pke_resume();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_resume, ret);
        return ret;
    }

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_wait_free, ret);
        drv_pke_suspend();
        return ret;
    }

    block.word[0] = 0x00;
    block.word[1] = e;

    drv_pke_set_mode(PKE_MODE_KGEN_WITH_E, key->klen);
    drv_pke_set_block(PKE_RAM_TYPE_KRAM, 0, &block, 0, key->klen);

    /* start */
    drv_pke_start();

    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("rsa exp mod failed.\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        goto _exit;
    }

    ret = drv_pke_check_pub_e(e, klen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_check_pub_e, ret);
        goto _exit;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_MRAM, 0, key->n, klen);
    if ((key->n[0] & CHECK_HIGH_TWO_BIT) == 0) {
        hi_log_error("Invalid n key with two high bit 0.\n");
        ret = HI_ERR_CIPHER_ILLEGAL_KEY;
        hi_log_print_err_code(ret);
        goto _exit;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, key->d, klen);

    /* nonsupport generation p,q,dp,dq.qp */
    memset_s(key->p, klen, 0, klen);
    memset_s(key->q, klen, 0, klen);
    memset_s(key->dp, klen, 0, klen);
    memset_s(key->dq, klen, 0, klen);
    memset_s(key->qp, klen, 0, klen);

    key->klen = klen;
    key->e = e;

_exit:
    ret_exit = drv_pke_clean_ram();
    if (ret_exit != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_clean_ram, ret_exit);
    }

    drv_pke_suspend();

    hi_log_func_exit();

    return ret;
}

hi_s32 drv_rsa_compute_crt(hi_u8 *p, hi_u8 *q, hi_u32 e, hi_u32 klen,
                           hi_u8 *n, hi_u8 *d)
{
    hi_s32 ret;
    hi_s32 ret_exit;
    pke_block block;

    hi_log_func_enter();

    hi_log_check_inited(pke_initialize);
    hi_log_check_param(pke_info.attacked);
    pke_info.done = HI_FALSE;

    if ((e & 0x00000001) == 0) {
        hi_log_error("Invalid input public key, e should be an odd number.\n");
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    ret = drv_pke_wait_free();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_wait_free, ret);
        return ret;
    }

    block.word[0] = 0x00;
    block.word[1] = e;

    drv_pke_set_mode(PKE_MODE_KTRANS, klen);

    drv_pke_set_block(PKE_RAM_TYPE_KRAM, 0, &block, 0, klen);
    drv_pke_set_crt_p(p, klen);
    drv_pke_set_ram(PKE_RAM_TYPE_NRAM, 0, q, klen, HI_NULL);

    /* config start reg */
    drv_pke_start();
    ret = drv_pke_wait_done();
    if (ret != HI_SUCCESS) {
        hi_log_error("rsa exp mod failed.\n");
        hi_log_print_func_err(drv_pke_wait_done, ret);
        goto _exit;
    }

    ret = drv_pke_check_pub_e(e, klen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_check_pub_e, ret);
        goto _exit;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_MRAM, 0, n, klen);
    if ((n[0] & CHECK_HIGH_TWO_BIT) == 0) {
        hi_log_error("Invalid n key with two high bit 0.\n");
        ret = HI_ERR_CIPHER_ILLEGAL_KEY;
        hi_log_print_err_code(ret);
        goto _exit;
    }

    drv_pke_get_ram(PKE_RAM_TYPE_RRAM, 0, d, klen);

_exit:
    ret_exit = drv_pke_clean_ram();
    if (ret_exit != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_clean_ram, ret_exit);
    }
    hi_log_func_exit();

    return ret;
}

void drv_pke_get_capacity(pke_capacity *capacity)
{
    (void)memset_s(capacity, sizeof(pke_capacity), 0, sizeof(pke_capacity));

    capacity->sm2 = CRYPTO_CAPACITY_SUPPORT;
    capacity->ecc = CRYPTO_CAPACITY_SUPPORT;
    capacity->rsa = CRYPTO_CAPACITY_SUPPORT;

    return;
}

/** @} */ /** <!-- ==== API declaration end ==== */

#endif  // End of CHIP_PKE_VER_V100
