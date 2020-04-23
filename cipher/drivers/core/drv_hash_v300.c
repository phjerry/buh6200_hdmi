/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_hash_v300
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_hash_v300.h"
#include "drv_hash.h"

#ifdef CHIP_HASH_VER_V300

/*************************** Structure Definition ****************************/
/** \addtogroup     hash */
/** @{ */ /** <!-- [hash] */
/* ! hash node dpth */
#define HASH_MAX_DEPTH (255)

/* ! hash in entry list size */
#define HASH_NODE_SIZE (sizeof(struct hash_entry_in) * HASH_MAX_DEPTH)

/* ! hash in entry list size */
#define HASH_NODE_LIST_SIZE (HASH_NODE_SIZE * HASH_HARD_CHANNEL_CNT)

#define SPACC_CPU_CUR          (crypto_is_sec_cpu() ? SPACC_CPU_TEE : SPACC_CPU_REE)
#define SPACC_LOCK_CHN(chn)    (SPACC_CPU_CUR << chn)
#define SPACC_RELEASE_CHN(chn) (SPACC_CPU_CUR << chn)

#define SMMU_PAGE_BIT          12

//#define HASH_ALL_CHN_TEE  0xAAAAAAAA
//#define HASH_ALL_CHN_REE  0x55555555
//#define HASH_ALL_CHN_HPP  0xFFFFFFFF
//#define HASH_ALL_CHN_IDLE 0x00000000

/* ! Define the flag of node */
typedef enum {
    HASH_CTRL_NONE = 0x00,          /* !<  middle node */
    HASH_CTRL_HASH_IN_FIRST = 0x01, /* !<  first node */
    HASH_CTRL_HASH_IN_LAST = 0x02,  /* !<  last node */
    HASH_CTRL_COUNT,
} HASH_CTRL_EN;

/* ! spacc digest in entry struct which is defined by hardware, you can't change it */
struct hash_entry_in {
    hi_u32 spacc_cmd : 2;       /* !<  reserve */
    hi_u32 rev1 : 14;           /* !<  reserve */
    hi_u32 hmac_last_block : 1; /* !<  hash control flag */
    hi_u32 rev2 : 15;           /* !<  reserve */
    hi_u32 hash_alg_length;     /* !<  hash message length */
    hi_u32 hash_start_addr_l;   /* !<  hash message address */
    hi_u32 hash_start_addr_h;   /* !<  hash message address */
};

/* ! Define the context of hash */
typedef struct {
    hash_mode hash_alg;             /* !<  hash mode */
    struct hash_entry_in *entry_in; /* ! spacc digest in entry struct */
    hi_u32 id_in;                   /* !< current hash nodes to be used */
    hi_u32 id_cnt;                  /* !< current hash nodes to be used */
    hi_u32 done;                    /* !<  calculation finish flag */
    crypto_queue_head queue;        /* !<  quene list */

} hash_hard_context;

/* ! hash already initialize or not */
static hi_u32 hash_initialize = HI_FALSE;

/* ! dma memory of hash node list */
static crypto_mem hash_dma;

/* ! Channel of hash */
static channel_context hash_hard_channel[CRYPTO_HARD_CHANNEL_MAX];

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      hash */
/** @{ */ /** <!--[hash] */

#ifdef CRYPTO_OS_INT_SUPPORT
static hi_u32 hash_done_notify(void)
{
    hash_chann_int last_status;
    hash_chann_raw_int last_raw;
    ista_in_hash_node_cnt level_status;
    iraw_in_hash_node_cnt level_raw;
    spacc_is spacc_is;
    hi_u32 mask = 0;

    spacc_is.u32 = symc_read(SPACC_IS);
    if (SPACC_CPU_CUR == SPACC_CPU_TEE) {
        mask = spacc_is.bits.int_cal_ctrl_tee;
    } else {
        mask = spacc_is.bits.int_cal_ctrl_ree;
    }
    if (mask == 0) {
        return 0; /* there is no last int or level int */
    }

    /* read last int for the valid channel then clear interception */
    last_status.u32 = 0;
    last_status.u32 = hash_read(HASH_CHANN_INT);
    last_status.bits.hash_chann_int &= HASH_HARD_CHANNEL_MASK;
    hash_write(HASH_CHANN_RAW_INT, last_status.u32);

    /* read level int for the valid channel then clear interception */
    level_status.u32 = 0;
    level_status.u32 = hash_read(ISTA_IN_HASH_NODE_CNT);
    level_status.bits.ista_in_hash_node_cnt &= HASH_HARD_CHANNEL_MASK;
    hash_write(IRAW_IN_HASH_NODE_CNT, level_status.u32);

    level_raw.u32 = hash_read(IRAW_IN_HASH_NODE_CNT);
    last_raw.u32 = hash_read(HASH_CHANN_RAW_INT);

    /* it's needn't to distinguish last and level int,
     * the isr function will distinguish them by the left of nodes.
     */
    mask = last_status.bits.hash_chann_int | level_status.bits.ista_in_hash_node_cnt;

    return mask; /* mask */
}

static hi_u32 symc_done_test(void)
{
    out_sym_chan_int interruption;
    ista_in_sym_node_cnt status;

    interruption.u32 = hash_read(OUT_SYM_CHAN_LAST_NODE_INT);
    status.u32 = hash_read(ISTA_IN_SYM_NODE_CNT);

    /* just process the valid channel */
    interruption.bits.out_sym_chan_int &= CIPHER_HARD_CHANNEL_MASK;
    status.bits.ista_in_sym_node_cnt &= CIPHER_HARD_CHANNEL_MASK;

    return (interruption.bits.out_sym_chan_int | status.bits.ista_in_sym_node_cnt); /* mask */
}

/* ! hash interrupt process function */
static irqreturn_t hash_interrupt_isr(hi_s32 irq, void *devId)
{
    hi_u32 mask;
    hi_u32 i;
    hash_hard_context *ctx = HI_NULL;
    irqreturn_t ret = IRQ_HANDLED;

    crypto_unused(irq);

    mask = hash_done_notify();

    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if ((mask >> i) & 0x01) {
            ctx = (hash_hard_context *)hash_hard_channel[i].ctx;
            ctx->done = HI_TRUE;
            hi_log_debug("chn %d wake up\n", i);
            crypto_queue_wake_up(&ctx->queue);
        }
    }

    /* symc and hash use the sample interrupt number
     * so if symc has occur interrupt, we should return IRQ_NONE
     * to tell system continue to process the symc interrupt.
 */
    if (symc_done_test() != 0) {
        ret = IRQ_NONE;
    }

    return ret;
}

/* ! hash register interrupt process function */
static hi_s32 drv_hash_register_interrupt(void)
{
    hi_s32 ret;
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    hi_u32 i;
    const char *name;
    hash_hard_context *ctx = HI_NULL;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_HASH, &int_valid, &int_num, &name);

    if (int_valid == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* request irq */
    ret = crypto_request_irq(int_num, hash_interrupt_isr, name);
    if (ret != HI_SUCCESS) {
        hi_log_error("Irq request failure, irq = %d", int_num);
        hi_log_print_err_code(HI_ERR_CIPHER_REGISTER_IRQ);
        return HI_ERR_CIPHER_REGISTER_IRQ;
    }

    /* initialize queue list */
    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        ctx = hash_hard_channel[i].ctx;
        crypto_queue_init(&ctx->queue);
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* ! hash unregister interrupt process function */
static void drv_hash_unregister_interrupt(void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    const char *name;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_HASH, &int_valid, &int_num, &name);

    if (int_valid == HI_FALSE) {
        return;
    }

    /* free irq */
    hi_log_debug("hash free irq, num %d, name %s\n", int_num, name);
    crypto_free_irq(int_num, name);

    hi_log_func_exit();

    return;
}

/* ! set interrupt */
static void hash_set_interrupt(void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    const char *name;
    hash_chann_raw_int_en hash_fin_int_en;
    hash_chann_raw_int hash_fin_int_raw;
    iena_in_hash_node_cnt hash_level_int_en;
    iraw_in_hash_node_cnt hash_level_int_raw;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_HASH, &int_valid, &int_num, &name);

    if (int_valid == HI_FALSE) {
        hash_fin_int_en.u32 = hash_read(HASH_CHANN_RAW_INT_EN);
        hash_fin_int_en.bits.hash_chann_int_en &= (~HASH_HARD_CHANNEL_MASK);
        hash_write(HASH_CHANN_RAW_INT_EN, hash_fin_int_en.u32);
        hi_log_debug("HASH_CHANN_RAW_INT_EN[0x%x]: 0x%x\n", HASH_CHANN_RAW_INT_EN, hash_fin_int_en.u32);

        hash_level_int_en.u32 = hash_read(IENA_IN_HASH_NODE_CNT);
        hash_level_int_en.bits.iena_in_hash_node_cnt &= (~HASH_HARD_CHANNEL_MASK);
        hash_write(IENA_IN_HASH_NODE_CNT, hash_level_int_en.u32);
        hi_log_debug("IENA_IN_HASH_NODE_CNT[0x%x]: 0x%x\n", IENA_IN_HASH_NODE_CNT, hash_level_int_en.u32);
    } else {
        /* clear interception
         * the history of interception may trigge the system to
         * call the irq function before initialization
         * when register interrupt, this will cause a system abort.
         */
        hash_fin_int_raw.u32 = hash_read(HASH_CHANN_RAW_INT);
        hash_fin_int_raw.bits.hash_chann_raw_int &= HASH_HARD_CHANNEL_MASK; /* clear valid channel */
        hash_write(HASH_CHANN_RAW_INT, hash_fin_int_raw.u32);
        hi_log_debug("HASH_CHANN_RAW_INT[0x%x]: 0x%x\n", HASH_CHANN_RAW_INT, hash_fin_int_raw.u32);

        hash_level_int_raw.u32 = hash_read(IRAW_IN_HASH_NODE_CNT);
        hash_level_int_raw.bits.iraw_in_hash_node_cnt &= HASH_HARD_CHANNEL_MASK; /* clear valid channel */
        hash_write(IRAW_IN_HASH_NODE_CNT, hash_level_int_raw.u32);
        hi_log_debug("IRAW_IN_HASH_NODE_CNT[0x%x]: 0x%x\n", IRAW_IN_HASH_NODE_CNT, hash_level_int_raw.u32);

        hash_fin_int_en.u32 = hash_read(HASH_CHANN_RAW_INT_EN);
        hash_fin_int_en.bits.hash_chann_int_en |= HASH_HARD_CHANNEL_MASK;
        hash_write(HASH_CHANN_RAW_INT_EN, hash_fin_int_en.u32);
        hi_log_debug("HASH_INT_EN[0x%x]: 0x%x\n", HASH_CHANN_RAW_INT_EN, hash_fin_int_en.u32);

        hash_level_int_en.u32 = hash_read(IENA_IN_HASH_NODE_CNT);
        hash_level_int_en.bits.iena_in_hash_node_cnt |= HASH_HARD_CHANNEL_MASK;
        hash_write(IENA_IN_HASH_NODE_CNT, hash_level_int_en.u32);
        hi_log_debug("HASH_CHANN_RAW_INT_EN[0x%x]: 0x%x\n", IENA_IN_HASH_NODE_CNT, hash_level_int_en.u32);
    }

    hi_log_func_exit();

    return;
}
#endif

static hi_u32 hash_done_try(hi_u32 chn_num)
{
    hash_chann_raw_int hash_fin_int_raw;
    iraw_in_hash_node_cnt level_raw;
    hi_u32 chn_mask;

    hash_fin_int_raw.u32 = hash_read(HASH_CHANN_RAW_INT);
    hash_fin_int_raw.bits.hash_chann_raw_int &= 0x01 << chn_num;
    hash_write(HASH_CHANN_RAW_INT, hash_fin_int_raw.u32);

    level_raw.u32 = hash_read(IRAW_IN_HASH_NODE_CNT);
    level_raw.bits.iraw_in_hash_node_cnt &= 0x01 << chn_num;
    hash_write(IRAW_IN_HASH_NODE_CNT, level_raw.u32);

    chn_mask = hash_fin_int_raw.bits.hash_chann_raw_int | level_raw.bits.iraw_in_hash_node_cnt;

    return chn_mask;
}

hi_s32 hash_clear_channel(hi_u32 chn_num)
{
    hi_u32 j;
    hi_u32 clear_int;

    hi_log_debug("hash_clear_channel, chn %d!\n", chn_num);

    hash_write(SPACC_HASH_CHN_CLEAR_REQ, 0x01 << chn_num);
    for (j = 0; j < CRYPTO_TIME_OUT; j++) {
        clear_int = hash_read(SPACC_INT_RAW_HASH_CLEAR_FINISH);
        if ((0x01 << chn_num) & clear_int) {
            hash_write(SPACC_INT_RAW_HASH_CLEAR_FINISH, 0x01 << chn_num);
            break;
        }

        if (j <= MS_TO_US) {
            crypto_udelay(1); /* short waitting for 1000 us */
        } else {
            crypto_msleep(1); /* long waitting for 5000 ms */
        }
    }

    if (j >= CRYPTO_TIME_OUT) {
        hi_log_error("clear channel[%d] failed!\n", chn_num);
        return HI_ERR_CIPHER_TIMEOUT;
    }

    return HI_SUCCESS;
}

/* ! set hash entry */
static void hash_set_entry(hi_u32 chn, compat_addr dma_addr, void *cpu_addr)
{
    hash_hard_context *ctx = (hash_hard_context *)hash_hard_channel[chn].ctx;
    in_hash_chn_node_length hash_node_len;
    in_hash_chn_node_wr_point hash_wr_ptr;
    in_hash_chn_ctrl hash_chn_ctrl;

    /* set total num and start addr for hash in node */
    hash_node_len.u32 = hash_read(IN_HASH_CHN_NODE_LENGTH(chn));
    hash_node_len.bits.hash_chn_node_length = HASH_MAX_DEPTH;
    hash_write(IN_HASH_CHN_NODE_LENGTH(chn), hash_node_len.u32);
    hash_write(IN_HASH_CHN_NODE_START_ADDR_L(chn), ADDR_L32(dma_addr));
    hash_write(IN_HASH_CHN_NODE_START_ADDR_H(chn), ADDR_H32(dma_addr));
    hi_log_debug("IN_HASH_CHN_NODE_LENGTH[0x%x]: 0x%x\n", IN_HASH_CHN_NODE_LENGTH(chn), hash_node_len.u32);
    hi_log_debug("IN_HASH_CHN_NODE_START_ADDR_L[0x%x]: 0x%x\n", IN_HASH_CHN_NODE_START_ADDR_L(chn), ADDR_L32(dma_addr));
    hi_log_debug("IN_HASH_CHN_NODE_START_ADDR_H[0x%x]: 0x%x\n", IN_HASH_CHN_NODE_START_ADDR_H(chn), ADDR_H32(dma_addr));

    hash_chn_ctrl.u32 = hash_read(IN_HASH_CHN_CTRL(chn));
    hash_chn_ctrl.bits.hash_chn_en = 0x01; /* enable channel */
    hash_chn_ctrl.bits.hash_chn_ss = 0x00; /* use nosec table */
    hash_write(IN_HASH_CHN_CTRL(chn), hash_chn_ctrl.u32);
    hi_log_debug("IN_HASH_CHN_CTRL[0x%x]: 0x%x\n", IN_HASH_CHN_CTRL(chn), hash_chn_ctrl.u32);

    hash_wr_ptr.u32 = hash_read(IN_HASH_CHN_NODE_WR_POINT(chn));
    ctx->entry_in = (struct hash_entry_in *)cpu_addr;
    ctx->id_in = hash_wr_ptr.bits.hash_chn_node_wr_point;
    hi_log_debug("IN_HASH_CHN_NODE_WR_POINT[0x%x]: 0x%x\n", IN_HASH_CHN_NODE_WR_POINT(chn), hash_wr_ptr.u32);

    return;
}

/* ! set smmu */
static void hash_smmu_bypass(hi_u32 chn, hi_u32 enable)
{
#ifdef CRYPTO_SMMU_SUPPORT
    in_hash_chn_ctrl in_ctrl;

    hi_log_func_enter();

    in_ctrl.u32 = hash_read(IN_HASH_CHN_CTRL(chn));
    in_ctrl.bits.hash_chn_mmu_en = enable ? 1 : 0;
    hash_write(IN_HASH_CHN_CTRL(chn), in_ctrl.u32);

    hi_log_debug("IN_HASH_CHN_CTRL[0x%x]  :0x%x\n", IN_HASH_CHN_CTRL(chn), in_ctrl.u32);

    hi_log_func_exit();
#endif
}

/* ! smmu set base address */
static void drv_hash_smmu_base_addr(void)
{
#ifdef CRYPTO_SMMU_SUPPORT
    hi_ulong err_raddr = 0;
    hi_ulong err_waddr = 0;
    hi_ulong table_addr = 0;

    /* get table base addr from system api */
    smmu_get_table_addr(&err_raddr, &err_waddr, &table_addr);

    /* without tee, ree can use sec or nsec smmu table
       depend on IN_HASH_CHN_CTRL.hash_chn_ss.
       use sec table when IN_HASH_CHN_CTRL.hash_chn_ss = 1,
       use nsec table when IN_HASH_CHN_CTRL.hash_chn_ss = 01,
       there ree fix use nosec table
    */
    symc_write(SPACC_MMU_NOSEC_TLB, (hi_u32)table_addr >> 4); /* bit[35:4]: mmu base address, bit[3:0] fixed 0 */
    symc_write (SPACC_MMU_IN_HASH_NOSEC_EADDR_H, (hi_u32)(err_raddr >> 32)); /* high 32 bit of addr */
    symc_write (SPACC_MMU_IN_HASH_NOSEC_EADDR_L, (hi_u32)(err_raddr));       /* low 32 bit of addr */
    hi_log_debug("SPACC_MMU_NOSEC_TLB[0x%x]  : 0x%x\n", SPACC_MMU_NOSEC_TLB, (hi_u32)table_addr);
#endif

    return;
}

/* ! set secure channel,
 *  non-secure CPU can't change the value of SEC_CHN_CFG,
 *  so non-secure CPU call this function will do nothing.
 */
static hi_s32 drv_hash_enable_secure(void)
{
    hi_s32 ret;
    hi_u32 used;
    hi_u32 chnn_who_used = 0;
    hi_u32 mask;

    hi_log_func_enter();

    mask = crypto_is_sec_cpu() ? SPACC_CPU_TEE : SPACC_CPU_REE;
    used = hash_read(SPACC_HASH_CHN_LOCK);

    /* try to use this channel */
    chnn_who_used = CHN_WHO_USED_GET(used, HASH_HARD_CHANNEL);
    if (chnn_who_used != SPACC_CPU_IDLE) {
        hi_log_error("chn[%d] aleardy be used by cpu 0x%x.\n", HASH_HARD_CHANNEL, chnn_who_used);
        return HI_ERR_CIPHER_BUSY;
    }

    CHN_WHO_USED_SET(used, HASH_HARD_CHANNEL, mask);
    hash_write(SPACC_HASH_CHN_LOCK, used);

    /* check if the channal aleardy be useded by other cpu
     * if other cpu break-in when write the CIPHER_NON_SEC_CHN_WHO_USED
     * the value of CIPHER_NON_SEC_CHN_WHO_USED will be channged
     */
    used = hash_read(SPACC_HASH_CHN_LOCK);
    chnn_who_used = CHN_WHO_USED_GET(used, HASH_HARD_CHANNEL);
    hi_log_debug("SPACC_HASH_CHN_LOCK[0x%x] 0x%x\n", SPACC_HASH_CHN_LOCK, used);
    if (chnn_who_used != mask) {
        hi_log_error("chn[%d] aleardy be used by cpu 0x%x.\n", HASH_HARD_CHANNEL, chnn_who_used);
        return HI_ERR_CIPHER_BUSY;
    }

    ret = hash_clear_channel(HASH_HARD_CHANNEL);
    if (HI_SUCCESS != ret) {
        hi_log_print_func_err(hash_clear_channel, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static void hash_print_last_node(hi_u32 chn_num)
{
    struct hash_entry_in *in = HI_NULL;
    hash_hard_context *ctx = HI_NULL;

    ctx = (hash_hard_context *)hash_hard_channel[chn_num].ctx;

    /* get last in node info */
    if (ctx->id_in == 0x00) {
        in = &ctx->entry_in[HASH_MAX_DEPTH];
    } else {
        in = &ctx->entry_in[ctx->id_in - 1];
    }

    hi_log_info("chn %d, src addr 0x%x%x, size 0x%x\n",
                chn_num, in->hash_start_addr_h, in->hash_start_addr_l, in->hash_alg_length);

    crypto_unused(in);
}

static hi_s32 drv_hash_get_err_code(hi_u32 chn_num)
{
    tee_hash_calc_ctrl_check_err code;
    tee_hash_calc_ctrl_check_err_status status;

    /* read error code */
    code.u32 = hash_read(HASH_CALC_CTRL_CHECK_ERR(SPACC_CPU_CUR));
    status.u32 = hash_read(HASH_CALC_CTRL_CHECK_ERR_STATUS(SPACC_CPU_CUR));

    if (code.bits.tee_hash_check_alg_error) {
        hi_log_error("hash error: hash_check_alg_error, chn %d !!!\n", chn_num);
    }
    if (code.bits.tee_hash_check_alg_invld_error) {
        hi_log_error("hash error: hash_check_alg_invld_error, chn %d !!!\n", chn_num);
    }
    if (code.bits.tee_hash_check_tee_error) {
        hi_log_error("hash error: hash_check_tee_error, chn %d !!!\n", chn_num);
    }
    if (code.bits.tee_hash_check_sc_error) {
        hi_log_error("symc error: hash_check_sc_error, chn %d !!!\n", chn_num);
    }
    if (code.bits.tee_hash_check_hmac_lock_error) {
        hi_log_error("symc error: hash_check_hmac_lock_error, chn %d !!!\n", chn_num);
    }

    /* print the inout buffer address */
    if (code.u32) {
        hash_print_last_node(chn_num);

        /* clear error code */
        status.bits.tee_hash_error_code_clr = 1;
        hash_write(HASH_CALC_CTRL_CHECK_ERR_STATUS(SPACC_CPU_CUR), status.u32);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_hash_check_mmu(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    spacc_mmu_in_hash_nosec_unvld_va_tab va_tab;
    spacc_mmu_in_hash_nosec_unvld_pa_tab pa_tab;

    /* read error code */
    va_tab.u32 = hash_read(SPACC_MMU_IN_HASH_NOSEC_UNVLD_VA_TAB);
    pa_tab.u32 = hash_read(SPACC_MMU_IN_HASH_NOSEC_UNVLD_PA_TAB);

    if (va_tab.bits.mmu_in_hash_nosec_unvld_va_vld == 1) {
        hi_log_error("mmu_in_hash_nosec_unvld_va_vld:\n");
        hi_log_error("mmu_in_hash_nosec_unvld_chn_id %d !!!\n", va_tab.bits.mmu_in_hash_nosec_unvld_chn_id);
        hi_log_error("mmu_in_hash_nosec_unvld_va_tab 0x%x !!!\n",
            va_tab.bits.mmu_in_hash_nosec_unvld_va_tab << SMMU_PAGE_BIT);
        hi_log_error("SPACC_MMU_NOSEC_TLB[0x%x] 0x%x\n", SPACC_MMU_NOSEC_TLB, symc_read(SPACC_MMU_NOSEC_TLB));
        ret = HI_ERR_CIPHER_INVALID_ADDR;
    }
    if (pa_tab.bits.mmu_in_hash_nosec_unvld_pa_vld == 1) {
        hi_log_error("mmu_in_hash_nosec_unvld_pa_vld:\n");
        hi_log_error("mmu_in_hash_nosec_unvld_vld_err %d !!!\n", pa_tab.bits.mmu_in_hash_nosec_unvld_vld_err);
        hi_log_error("mmu_in_hash_nosec_unvld_pa_tab 0x%x !!!\n",
            pa_tab.bits.mmu_in_hash_nosec_unvld_pa_tab << SMMU_PAGE_BIT);
        ret = HI_ERR_CIPHER_INVALID_ADDR;
    }

    /* clear error code */
    symc_write(SPACC_MMU_IN_HASH_SEC_UNVLD_VA_TAB, va_tab.u32);
    symc_write(SPACC_MMU_IN_HASH_SEC_UNVLD_PA_TAB, pa_tab.u32);

    return ret;
}

void hash_enrty_init(crypto_mem mem)
{
    hi_u32 i;
    compat_addr mmz_addr;
    void *cpu_addr = HI_NULL;

    ADDR_U64(mmz_addr) = ADDR_U64(mem.dma_addr);
    cpu_addr = mem.dma_virt;

    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if ((HASH_HARD_CHANNEL_MASK >> i) & 0x01) { /* valid channel */
            hash_set_entry(i, mmz_addr, cpu_addr);
            ADDR_U64(mmz_addr) += HASH_NODE_SIZE;          /* move to next channel */
            cpu_addr = (hi_u8 *)cpu_addr + HASH_NODE_SIZE; /* move to next channel */
        }
    }

    return;
}

void hash_enrty_deinit(void)
{
    hi_u32 used;

    used = hash_read(SPACC_HASH_CHN_LOCK);
    CHN_WHO_USED_CLR(used, HASH_HARD_CHANNEL);
    hash_write(SPACC_HASH_CHN_LOCK, used);

    return;
}

static hi_void drv_hash_low_power(hi_void)
{
    spacc_calc_crg_cfg cfg;

    cfg.u32 = symc_read(SPACC_CALC_CRG_CFG);
    cfg.bits.spacc_sha1_low_power_enable  = HASH_LOW_POWER_ENABLE;
    cfg.bits.spacc_sha2_low_power_enable  = HASH_LOW_POWER_ENABLE;
    cfg.bits.spacc_sm3_low_power_enable   = HASH_LOW_POWER_ENABLE;
    symc_write(SPACC_CALC_CRG_CFG, cfg.u32);
}

hi_s32 drv_hash_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    if (hash_initialize == HI_TRUE) {
        hi_log_func_exit();
        return HI_SUCCESS;
    }

    ret = crypto_channel_init(hash_hard_channel, CRYPTO_HARD_CHANNEL_MAX, sizeof(hash_hard_context));
    if (ret != HI_SUCCESS) {
        hi_log_error("error, hash channel list init failed\n");
        hi_log_print_func_err(crypto_channel_init, ret);
        return ret;
    }

    hi_log_debug("enable hash\n");
    module_enable(CRYPTO_MODULE_ID_HASH);

    hi_log_debug("alloc memory for nodes list\n");
    ret = crypto_mem_create(&hash_dma, MEM_TYPE_MMZ, "CIPHER_SHA_NODE", HASH_NODE_LIST_SIZE);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, malloc ddr for hash nodes list failed\n");
        hi_log_print_func_err(crypto_mem_create, ret);
        goto __error1;
    }
    hi_log_debug("HASH DMA buffer, MMZ 0x%x, VIA %p, size 0x%x\n",
                 ADDR_L32(hash_dma.dma_addr),
                 hash_dma.dma_virt, hash_dma.dma_size);

    hi_log_debug("hash secure channel configure\n");
    ret = drv_hash_enable_secure();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_hash_enable_secure, ret);
        crypto_mem_destory(&hash_dma);
        goto __error1;
    }

    hi_log_debug("hash entry list configure\n");
    hash_enrty_init(hash_dma);

    hi_log_debug("hash SMMU configure\n");
    hash_smmu_bypass(HASH_HARD_CHANNEL, HI_TRUE);
    drv_hash_smmu_base_addr();
    drv_hash_low_power();

#ifdef CRYPTO_OS_INT_SUPPORT
    hi_log_debug("hash interrupt configure\n");
    hash_set_interrupt();

    hi_log_debug("hash register interrupt function\n");
    ret = drv_hash_register_interrupt();
    if (ret != HI_SUCCESS) {
        hi_log_error("error, register interrupt failed\n");
        hi_log_print_func_err(drv_hash_register_interrupt, ret);
        crypto_mem_destory(&hash_dma);
        goto __error1;
    }
#endif

    hash_initialize = HI_TRUE;
    hi_log_func_exit();
    return HI_SUCCESS;

__error1:
    module_disable(CRYPTO_MODULE_ID_HASH);
    crypto_channel_deinit(hash_hard_channel, CRYPTO_HARD_CHANNEL_MAX);

    return ret;
}

hi_s32 drv_hash_deinit(void)
{
    hi_log_func_enter();

    if (hash_initialize == HI_FALSE) {
        hi_log_func_exit();
        return HI_SUCCESS;
    }

#ifdef CRYPTO_OS_INT_SUPPORT
    drv_hash_unregister_interrupt();
#endif

    hash_enrty_deinit();
    crypto_mem_destory(&hash_dma);
    module_disable(CRYPTO_MODULE_ID_HASH);
    crypto_channel_deinit(hash_hard_channel, CRYPTO_HARD_CHANNEL_MAX);
    hash_initialize = HI_FALSE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

void drv_hash_resume(void)
{
    hi_u32 i;

    hi_log_func_enter();

    hi_log_debug("enable hash\n");
    module_enable(CRYPTO_MODULE_ID_HASH);

    hi_log_debug("hash entry list configure\n");
    hash_enrty_init(hash_dma);

#ifdef CRYPTO_OS_INT_SUPPORT
    hi_log_debug("hash interrupt configure\n");
    hash_set_interrupt();
#endif

    hi_log_debug("hash SMMU configure\n");
    for (i = 0; i < HASH_HARD_CHANNEL_CNT; i++) {
        /* check the valid channel */
        if (HASH_HARD_CHANNEL_MASK & (0x1 << i)) {
            hash_smmu_bypass(i, HI_TRUE);
        }
    }
    drv_hash_smmu_base_addr();

    hi_log_debug("hash secure channel configure\n");
    drv_hash_enable_secure();

    hi_log_func_exit();

    return;
}

void drv_hash_suspend(void)
{
    hi_log_func_enter();
    hi_log_func_exit();

    return;
}

/* wait hash ready */
static hi_s32 drv_hash_wait_ready(hi_u32 chn_num)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    hi_u32 errcode = 0;
    hi_s32 ret;
    hi_u32 i;

    hi_log_func_enter();

    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    module_get_attr(CRYPTO_MODULE_ID_HASH, &int_valid, &int_num, HI_NULL);

#ifdef CRYPTO_OS_INT_SUPPORT
    if (int_valid) {
        hash_hard_context *ctx = HI_NULL;

        ctx = (hash_hard_context *)hash_hard_channel[chn_num].ctx;

        /* wait interrupt */
        ret = crypto_queue_wait_timeout(&ctx->queue, crypto_queue_callback_func, &ctx->done, CRYPTO_TIME_OUT);
        if (ret == 0) {
            hi_log_error("wait done timeout, chn=%d\n", chn_num);
            drv_hash_get_err_code(chn_num);
            hi_log_print_func_err(crypto_queue_wait_timeout, ret);
            return HI_ERR_CIPHER_TIMEOUT;
        } else {
            ret = HI_SUCCESS;
        }
    } else {
#else
    {
#endif
        for (i = 0; i < CRYPTO_TIME_OUT; i++) {
            if (hash_done_try(chn_num)) {
                break;
            }

            if (i <= MS_TO_US) {
                crypto_udelay(1); /* short waitting for 1000 us */
            } else {
                crypto_msleep(1); /* long waitting for 5000 ms */
            }
        }
        if (i >= CRYPTO_TIME_OUT) {
            hi_log_error("hash wait done timeout, chn=%d\n", chn_num);
            hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
            ret = HI_ERR_CIPHER_TIMEOUT;
        } else {
            ret = HI_SUCCESS;
        }
    }
    if (ret != HI_SUCCESS) {
        errcode = drv_hash_get_err_code(chn_num);
        hi_log_error("hard error code: 0x%x\n", errcode);
        crypto_unused(errcode);
    }

    hi_log_func_exit();
    return ret;
}

static void hash_addbuf(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size)
{
    hash_hard_context *ctx = HI_NULL;
    hi_u32 id;
    hi_u32 size;
    void *addr = HI_NULL;

    ctx = (hash_hard_context *)hash_hard_channel[chn_num].ctx;

    /* clean in entry */
    id = ctx->id_in++;
    addr = &ctx->entry_in[id];
    size = sizeof(struct hash_entry_in);

    memset_s(addr, size, 0, size);

    /* set addr and length */
    ctx->entry_in[id].spacc_cmd = HASH_CTRL_HASH_IN_FIRST | HASH_CTRL_HASH_IN_LAST;
    ctx->entry_in[id].hash_start_addr_l = ADDR_L32(buf_phy);
    ctx->entry_in[id].hash_start_addr_h = ADDR_H32(buf_phy) & 0xf;
    ctx->entry_in[id].hash_alg_length = buf_size;
    // ctx->entry_in[id].hmac_last_block = HASH_CTRL_HASH_IN_LAST>>1;
    ctx->id_in %= HASH_MAX_DEPTH;
    hi_log_debug("add digest in buf: id %d, addr 0x%x, len 0x%x\n",
                 id, ADDR_L32(buf_phy), buf_size);

    return;
}

hi_s32 drv_hash_config(hi_u32 chn_num, hash_mode mode, hi_u32 state[HASH_RESULT_MAX_SIZE_IN_WORD])
{
    hash_hard_context *ctx = HI_NULL;
    in_hash_chn_key_ctrl hash_key_ctrl;
    hi_u32 hash_mode_cfg_val[] = { 0x0, 0x0, 0x1, 0x2, 0x3, 0x0 }; /* null 224 256 384 512 null */
    hi_u32 hash_alg_cfg_val[] = { 0xa, 0xb, 0xb, 0xb, 0xb, 0xc };  /* null 224 256 384 512 null */
    hi_u32 i = 0;

    crypto_assert(hash_initialize == HI_TRUE);
    crypto_assert((HASH_HARD_CHANNEL_MASK >> chn_num) & 0x01);

    ctx = (hash_hard_context *)hash_hard_channel[chn_num].ctx;
    ctx->hash_alg = mode;

    /* Control */
    hash_key_ctrl.u32 = hash_read(IN_HASH_CHN_KEY_CTRL(chn_num));
    hash_key_ctrl.bits.hash_chn_alg_mode = hash_mode_cfg_val[mode];
    hash_key_ctrl.bits.hash_chn_alg_sel = hash_alg_cfg_val[mode];
    hash_write(IN_HASH_CHN_KEY_CTRL(chn_num), hash_key_ctrl.u32);
    hi_log_debug("IN_HASH_CHN_KEY_CTRL: 0x%X\n", hash_key_ctrl.u32);

    /* Write last state */
    for (i = 0; i < HASH_RESULT_MAX_SIZE_IN_WORD; i++) {
        hash_write(CHANn_HASH_STATE_VAL_ADDR(chn_num), i);
        hash_write(CHANn_HASH_STATE_VAL(chn_num), state[i]);
    }

    return HI_SUCCESS;
}

static hi_void drv_hash_check_resume(hi_void)
{
    hi_u32 base;

    hi_log_func_enter();

    base = symc_read(IN_HASH_CHN_NODE_START_ADDR_L(HASH_HARD_CHANNEL));
    if (base == 0) {
        /* smmu base address is zero means hardware be unexpected reset */
        hi_log_warn("hash module is not ready, try to resume it now...\n");
        drv_hash_resume();

    }

    hi_log_func_exit();
    return;
}

hi_s32 drv_hash_start(hi_u32 chn_num, crypto_mem *mem, hi_u32 length)
{
    in_hash_chn_node_wr_point in_node_wr_ptr;
    spacc_mmu_in_hash_clear clear;
    hash_hard_context *ctx = HI_NULL;
    hi_u32 ptr;

    hi_log_func_enter();

    crypto_assert(hash_initialize == HI_TRUE);
    crypto_assert((HASH_HARD_CHANNEL_MASK >> chn_num) & 0x01);

    ctx = (hash_hard_context *)hash_hard_channel[chn_num].ctx;

    if (length == 0) {
        return HI_SUCCESS;
    }

    drv_hash_check_resume();

    /* channel clear mmu page (SW set this bit 1 and HW clean it to be 0) */
    clear.u32 = hash_read(SPACC_MMU_IN_HASH_CLEAR);
    clear.bits.sw_mmu_in_hash_clr = chn_num << 1;
    hash_write(SPACC_MMU_IN_HASH_CLEAR, clear.u32);

    ctx->done = HI_FALSE;
    /* set message addr and length */
    hash_addbuf(chn_num, mem->dma_addr, length);

    /* configure in-node, only compute one nodes */
    in_node_wr_ptr.u32 = hash_read(IN_HASH_CHN_NODE_WR_POINT(chn_num));
    ptr = in_node_wr_ptr.bits.hash_chn_node_wr_point + 1;
    in_node_wr_ptr.bits.hash_chn_node_wr_point = ptr % HASH_MAX_DEPTH;

    /* make sure all the above explicit memory accesses and instructions are completed
     * before start the hardware.
     */
    arm_memory_barrier();

    /* Start */
    hash_write(IN_HASH_CHN_NODE_WR_POINT(chn_num), in_node_wr_ptr.u32);
    hi_log_debug("IN_HASH_CHN_NODE_WR_POINT[0x%x]: 0x%X\n", IN_HASH_CHN_NODE_WR_POINT(chn_num), in_node_wr_ptr.u32);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_hash_wait_done(hi_u32 chn_num, hi_u32 *state, hi_u32 len_in_word)
{
    hi_u32 i = 0;
    hi_s32 ret;

    hi_log_func_enter();

    crypto_assert(hash_initialize == HI_TRUE);
    crypto_assert((HASH_HARD_CHANNEL_MASK >> chn_num) & 0x01);

    if (len_in_word < HASH_RESULT_MAX_SIZE_IN_WORD) {
        hi_log_error("error, state len err, len_in_word=%d", len_in_word);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    ret = drv_hash_wait_ready(chn_num);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_hash_wait_ready, ret);
        return ret;
    }

    ret = drv_hash_check_mmu();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_hash_check_mmu, ret);
        return ret;
    }

    /* read hash result */
    for (i = 0; i < HASH_RESULT_MAX_SIZE_IN_WORD; i++) {
        hash_write(CHANn_HASH_STATE_VAL_ADDR(chn_num), i);
        state[i] = hash_read(CHANn_HASH_STATE_VAL(chn_num));
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

void drv_hash_reset(hi_u32 chn_num)
{
    crypto_unused(chn_num);
}

void drv_hash_get_capacity(hash_capacity *capacity)
{
    memset_s(capacity, sizeof(hash_capacity), 0, sizeof(hash_capacity));

    capacity->sha1 = CRYPTO_CAPACITY_SUPPORT;
    capacity->sha224 = CRYPTO_CAPACITY_SUPPORT;
    capacity->sha256 = CRYPTO_CAPACITY_SUPPORT;
    capacity->sha384 = CRYPTO_CAPACITY_SUPPORT;
    capacity->sha512 = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm3 = CRYPTO_CAPACITY_SUPPORT;

    return;
}

/** @} */ /** <!-- ==== API declaration end ==== */

#endif  // End of CHIP_HASH_VER_V200
