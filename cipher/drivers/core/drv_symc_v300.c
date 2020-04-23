/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_symc_v300
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_symc_v300.h"
#include "drv_symc.h"

#ifdef CHIP_SYMC_VER_V300

/*************************** Internal Structure Definition ****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

/* ! \Length of pading buffer */
#define SYMC_PAD_BUFFER_LEN (128)

/* ! \Length of aes ccm/gcm key */
#define AES_CCM_GCM_KEY_LEN (16)

/* ! \Total Length of pading buffer */
#define SYMC_PAD_BUFFER_TOTAL_LEN (SYMC_PAD_BUFFER_LEN * CIPHER_HARD_CHANNEL_CNT)

#define SYMC_ERR_CODE_WITHOUT_KEY_ERR 0x0000000E
#define SYMC_ERR_CODE                 0x00000FFF

#define SYMC_ALL_CHN_TEE  0xAAAAAAAA
#define SYMC_ALL_CHN_REE  0x55555555
#define SYMC_ALL_CHN_HPP  0xFFFFFFFF
#define SYMC_ALL_CHN_IDLE 0x00000000

#define SYMC_CFG_SECURE     0x05
#define SYMC_CFG_NON_SECURE 0x0a

#define SMMU_PAGE_BIT       12
#define VALID_SIGNAL        1

/* current cpu */
#define SPACC_CPU_CUR (crypto_is_sec_cpu() ? SPACC_CPU_TEE : SPACC_CPU_REE)

/* ! spacc symc int entry struct which is defined by hardware, you can't change it */
typedef struct {
    hi_u32 sym_first_node : 1;         /* !<  Indicates whether the node is the first node */
    hi_u32 sym_last_node : 1;          /* !<  Indicates whether the node is the last node */
    hi_u32 rev1 : 7;                   /* !<  reserve */
    hi_u32 odd_even : 1;               /* !<  Indicates whether the key is an odd key or even key */
    hi_u32 rev2 : 6;                   /* !<  reserve */
    hi_u32 sym_gcm_first : 1;          /* !<  Indicates whether the node is the first node of first list,only for GCM */
    hi_u32 sym_ccm_last : 1;           /* !<  Indicates whether the node is the last node of last list ,only for CCM */
    hi_u32 aes_ccm_gcm_in_flag : 2;    /* !<  Indicates GCM/CCM flag: CCM[2'b0:N, 2'b1:A, 2'b10:P] ;
                                              GCM[2'b0:A, 2'b1:P, 2'b10:LEN] */
    hi_u32 rev3 : 4;                   /* !<  reserve */
    hi_u32 sym_gcm_and_ctr_iv_len : 5; /* !<  GCM: Indicates iv length; CTR:Bit width of the current counter,
                                              The length of the IV is 128 bits. */
    hi_u32 rev4 : 3;                   /* !<  reserve */
    hi_u32 sym_alg_length;             /* !<  syma data length */
    hi_u32 sym_start_addr;             /* !<  syma start low addr */
    hi_u32 sym_start_high;             /* !<  syma start high addr */
    hi_u32 IV[AES_IV_SIZE / WORD_WIDTH]; /* !<  symc IV */
    hi_u32 cenc_enable : 1;            /* !<  cenc enable */
    hi_u32 rev5 : 7;                   /* !<  reserve */
    hi_u32 cenc_head : 1;              /* !<  Indicates whether the head data */
    hi_u32 rev6 : 23;                  /* !<  reserve */
    hi_u32 e_num : 8;                  /* !<  Number of blocks in ciphertext in a pattern. */
    hi_u32 rev7 : 24;                  /* !<  reserve */
    hi_u32 c_num : 8;                  /* !<  Number of blocks in plain text in a pattern. */
    hi_u32 rev8 : 24;                  /* !<  reserve */
    hi_u32 Cenc_offset : 13;           /* !<  Length of the non-complete loop in the head of node */
    hi_u32 rev9 : 19;                  /* !<  reserve */
    hi_u32 rev10;                      /* !<  reserve */
    hi_u32 rev11;                      /* !<  reserve */
    hi_u32 rev12;                      /* !<  reserve */
    hi_u32 rev13;                      /* !<  reserve */
} symc_entry_in;

/* ! spacc symc out entry struct which is defined by hardware, you can't change it */
typedef struct {
    hi_u32 rev1;           /* !<  reserve */
    hi_u32 sym_alg_length; /* !<  syma data length */
    hi_u32 sym_start_addr; /* !<  syma start low addr */
    hi_u32 sym_start_high; /* !<  syma start high addr */
} symc_entry_out;

/* ! Define the context of cipher */
typedef struct {
    hi_u32 open;                         /* !<  open or close */
    hi_u32 who_used;                     /* !<  TEE(sec) or REE(nosec) or HPP used */
    symc_entry_in *entry_in;             /* !<  in node list */
    symc_entry_out *entry_out;           /* !<  out node list */
    compat_addr dma_entry;               /* !<  dma addr of node */
    compat_addr dma_pad;                 /* !<  dma addr of padding buffer, for CCM/GCM */
    hi_u8 *via_pad;                      /* !<  via addr of padding buffer, for CCM/GCM */
    hi_u32 offset_pad;                   /* !<  offset of padding buffer, for CCM/GCM */
    hi_u32 iv[SYMC_IV_MAX_SIZE_IN_WORD]; /* !<  iv data from user */

    /* iv usage flag, should be CIPHER_IV_CHANGE_ONE_PKG
     * or CIPHER_IV_CHANGE_ALL_PKG.
 */
    hi_u32 iv_flag;                /* !<  iv flag */
    hi_u32 iv_len;                 /* !<  iv length */
    symc_alg alg;                  /* !<  The alg of Symmetric cipher */
    symc_mode mode;                /* !<  mode */
    symc_key_source key_src;       /* !<  key source */
    hi_u32 id_in;                  /* !<  current in nodes index */
    hi_u32 id_out;                 /* !<  current out nodes index */
    hi_u32 cnt_in;                 /* !<  total count in nodes to be computed */
    hi_u32 cnt_out;                /* !<  total count out nodes to be computed */
    hi_u32 done;                   /* !<  calculation finish flag */
    crypto_queue_head queue;       /* !<  quene list */
    callback_symc_isr callback;    /* !<  isr callback functon */
    callback_symc_destory destory; /* !<  destory callback functon */
    void *ctx;                     /* !<  params for isr callback functon */
} symc_hard_context;

/* ! spacc symc_chn_who_used struct which is defined by hardware, you can't change it */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int non_sec_chn_who_used : 16; /* [15..0] */
        unsigned int sec_chn1_who_used : 2;     /* [17..16] */
        unsigned int reserved : 22;             /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} symc_chn_who_used;

/* ! Channel of cipher */
static symc_hard_context *hard_context = HI_NULL;

/* ! dma memory of cipher node list */
static crypto_mem symc_dma;

/* dma padding memory of cipher */
static crypto_mem padding_dma;

/* ! symc already initialize or not */
static hi_u32 symc_initialize = HI_FALSE;

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

#ifdef CRYPTO_OS_INT_SUPPORT
static hi_u32 drv_symc_done_notify(void)
{
    out_sym_chan_int last_status;
    out_sym_chan_raw_int last_raw;
    out_sym_chan_raw_level_int level_status;
    out_sym_chan_raw_level_int level_raw;
    sym_chann_int tag_status;
    sym_chann_raw_int tag_raw;
    spacc_is spacc_is;
    hi_u32 mask = 0;

    spacc_is.u32 = symc_read(SPACC_IS);
    hi_log_debug("SPACC_IS[0x%x]: 0x%x\n", SPACC_IS, spacc_is.u32);
    if (SPACC_CPU_CUR == SPACC_CPU_TEE) {
        mask = spacc_is.bits.int_out_ctrl_tee | spacc_is.bits.int_cal_ctrl_tee;
    } else {
        mask = spacc_is.bits.int_out_ctrl_ree | spacc_is.bits.int_cal_ctrl_ree;
    }
    if (mask == 0) {
        return 0; /* there is no last int or level int */
    }

    /* read last int for the valid channel then clear interception */
    last_raw.u32 = 0;
    last_status.u32 = symc_read(OUT_SYM_CHAN_LAST_NODE_INT);
    last_status.bits.out_sym_chan_int &= CIPHER_HARD_CHANNEL_MASK;
    last_raw.bits.out_sym_chan_raw_int = last_status.bits.out_sym_chan_int;
    symc_write(OUT_SYM_CHAN_RAW_LAST_NODE_INT, last_raw.u32);
    hi_log_debug("OUT_SYM_CHAN_LAST_NODE_INT[0x%x]: 0x%x\n", OUT_SYM_CHAN_LAST_NODE_INT, last_status.u32);

    /* read level int for the valid channel then clear interception */
    level_raw.u32 = 0;
    level_status.u32 = symc_read(OUT_SYM_CHAN_LEVEL_INT);
    level_status.bits.out_sym_level_chan_raw_int &= CIPHER_HARD_CHANNEL_MASK;
    level_raw.bits.out_sym_level_chan_raw_int = level_status.bits.out_sym_level_chan_raw_int;
    symc_write(OUT_SYM_CHAN_RAW_LEVEL_INT, level_raw.u32);
    hi_log_debug("OUT_SYM_CHAN_LEVEL_INT[0x%x]: 0x%x\n", OUT_SYM_CHAN_LEVEL_INT, level_status.u32);

    /* read tag int for the valid channel then clear interception */
    tag_raw.u32 = 0;
    tag_status.u32 = symc_read(SYM_CHANN_INT);
    tag_status.bits.sym_chann_int &= CIPHER_HARD_CHANNEL_MASK;
    tag_raw.bits.sym_chann_raw_int = tag_status.bits.sym_chann_int;
    symc_write(SYM_CHANN_RAW_INT, tag_raw.u32);
    hi_log_debug("SYM_CHANN_RAW_INT[0x%x]: 0x%x for ccm/gcm tag\n", SYM_CHANN_RAW_INT, tag_status.u32);

    /* it's needn't to distinguish last and level int,
     * the isr function will distinguish them by the left of nodes.
     */
    mask = last_status.bits.out_sym_chan_int
           | level_status.bits.out_sym_level_chan_raw_int
           | tag_status.bits.sym_chann_int;

    return mask;
}

static hi_u32 drv_hash_done_test(void)
{
    hash_chann_int interruption;
    ista_in_hash_node_cnt status;
    hi_u32 chn_mask;

    interruption.u32 = symc_read(HASH_CHANN_INT);
    status.u32 = symc_read(ISTA_IN_HASH_NODE_CNT);

    /* just process the valid channel */
    interruption.bits.hash_chann_int &= HASH_HARD_CHANNEL_MASK;
    status.bits.ista_in_hash_node_cnt &= HASH_HARD_CHANNEL_MASK;
    chn_mask = interruption.bits.hash_chann_int | status.bits.ista_in_hash_node_cnt;

    return chn_mask;
}

/* ! symc interrupt process function */
static irqreturn_t drv_symc_interrupt_isr(s32 irq, void *devId)
{
    hi_u32 mask;
    hi_u32 i;
    hi_bool finish;
    symc_hard_context *ctx = HI_NULL;
    irqreturn_t ret = IRQ_HANDLED;

    /* get channel context */
    mask = drv_symc_done_notify();

    /* without channel 0 */
    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {

        if (((mask >> i) & 0x01) == 0) {
            continue;
        }

        /* process irq for channel i */
        ctx = &hard_context[i];
        if (ctx->callback != HI_NULL) {
            finish = ctx->callback(ctx->ctx);
            if (finish == HI_FALSE) {
                hi_log_debug("contiue to compute chn %d\n", i);
                continue;
            }
        }
        if (ctx->done == HI_FALSE) {
            ctx->done = HI_TRUE;
            hi_log_debug("chn %d wake up\n", i);
            crypto_queue_wake_up(&ctx->queue);
            continue;
        }
    }

    /* symc and hash use the sample interrupt number
     * so if hash has occur interrupt, we should return IRQ_NONE
     * to tell system continue to process the hash interrupt.
     */
    if (drv_hash_done_test() != 0) {
        ret = IRQ_NONE;
    }

    return ret;
}

/* ! symc register interrupt process function */
static hi_s32 drv_symc_register_interrupt(void)
{
    hi_s32 ret;
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    hi_u32 i;
    const char *name;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_SYMC, &int_valid, &int_num, &name);

    if (int_valid == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* request irq */
    hi_log_debug("symc request irq, num %d, name %s\n", int_num, name);
    ret = crypto_request_irq(int_num, drv_symc_interrupt_isr, name);
    if (ret != HI_SUCCESS) {
        hi_log_error("Irq request failure, irq = %d", int_num);
        hi_log_print_err_code(HI_ERR_CIPHER_REGISTER_IRQ);
        return ret;
    }

    /* initialize queue list */
    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        crypto_queue_init(&hard_context[i].queue);
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* ! symc unregister interrupt process function */
static void drv_symc_unregister_interrupt(void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    const char *name;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_SYMC, &int_valid, &int_num, &name);

    if (int_valid == HI_FALSE) {
        return;
    }

    /* free irq */
    hi_log_debug("symc free irq, num %d, name %s\n", int_num, name);
    crypto_free_irq(int_num, name);

    hi_log_func_exit();

    return;
}

/* ! set interrupt */
static void drv_symc_set_interrupt(hi_u32 chn_num)
{
    out_sym_chan_raw_level_int_en out_level_int_en;
    out_sym_chan_raw_level_int out_level_int_raw;
    out_sym_chan_raw_int_en out_sym_int_en;
    out_sym_chan_raw_int out_sym_raw;
    sym_chann_raw_int_en tag_int_en;
    sym_chann_raw_int tag_raw;
    hi_u32 mask;

    hi_log_func_enter();

    mask = 1 << chn_num;

    /* clear interception
     * the history of interception may trigge the system to
     * call the irq function before initialization
     * when register interrupt, this will cause a system abort.
     */
    out_level_int_raw.u32 = symc_read(OUT_SYM_CHAN_RAW_LEVEL_INT);
    out_level_int_raw.bits.out_sym_level_chan_raw_int &= mask;
    symc_write(OUT_SYM_CHAN_RAW_LEVEL_INT, out_level_int_raw.u32);

    out_sym_raw.u32 = symc_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT);
    out_sym_raw.bits.out_sym_chan_raw_int &= mask;
    symc_write(OUT_SYM_CHAN_RAW_LAST_NODE_INT, out_sym_raw.u32);

    tag_raw.u32 = symc_read(SYM_CHANN_RAW_INT);
    tag_raw.bits.sym_chann_raw_int &= mask;
    symc_write(SYM_CHANN_RAW_INT, tag_raw.u32);

    /* enable interrupt */
    out_sym_int_en.u32 = symc_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN);
    out_sym_int_en.bits.out_sym_chan_int_en |= mask;
    symc_write(OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN, out_sym_int_en.u32);
    hi_log_debug("OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN[0x%x]: 0x%x\n",
        OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN, out_sym_int_en.u32);

    /* now don't use level interrupt, may be use it in the future. */
    out_level_int_en.u32 = symc_read(OUT_SYM_CHAN_RAW_LEVEL_INT_EN);
    out_level_int_en.bits.out_sym_level_chan_int_en = 0;
    symc_write(OUT_SYM_CHAN_RAW_LEVEL_INT_EN, out_level_int_en.u32);
    hi_log_debug("OUT_SYM_CHAN_RAW_LEVEL_INT_EN[0x%x]: 0x%x\n",
        OUT_SYM_CHAN_RAW_LEVEL_INT_EN,out_level_int_en.u32);

    tag_int_en.u32 = symc_read(SYM_CHANN_RAW_INT_EN);
    tag_int_en.bits.sym_chann_int_en |= mask;
    symc_write(SYM_CHANN_RAW_INT_EN, tag_int_en.u32);
    hi_log_debug("SYM_CHANN_RAW_INT_EN[0x%x]: 0x%x\n", SYM_CHANN_RAW_INT_EN, tag_int_en.u32);

    hi_log_func_exit();

    return;
}

/* ! set interrupt */
static void drv_symc_interrupt_init(hi_void)
{
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    spacc_ie cipher_int_en;

    hi_log_func_enter();

    module_get_attr(CRYPTO_MODULE_ID_SYMC, &int_valid, &int_num, HI_NULL);

    if (int_valid) {
        /* Enable interrupt */
        cipher_int_en.u32 = symc_read(SPACC_IE);
        cipher_int_en.bits.spacc_ie_ree = 1;
        cipher_int_en.bits.spacc_ie_tee = 1;
        symc_write(SPACC_IE, cipher_int_en.u32);
        hi_log_debug("SPACC_IE[0x%x]: 0x%x\n", SPACC_IE, cipher_int_en.u32);
    } else {
        /* Disable interrupt */
        cipher_int_en.u32 = symc_read(SPACC_IE);
        cipher_int_en.bits.spacc_ie_tee = 0;
        cipher_int_en.bits.spacc_ie_ree = 0;
        cipher_int_en.bits.spacc_ie_hpp = 0;
        symc_write(SPACC_IE, cipher_int_en.u32);
        hi_log_debug("SPACC_IE[0x%x]: 0x%x\n", SPACC_IE, cipher_int_en.u32);
    }

    hi_log_func_exit();

    return;
}
#endif

static hi_s32 drv_symc_done_try(hi_u32 chn_num)
{
    out_sym_chan_raw_int last_raw;
    out_sym_chan_raw_level_int level_raw;
    sym_chann_raw_int tag_raw;
    hi_u32 mask;

    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    last_raw.u32 = symc_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT);
    last_raw.bits.out_sym_chan_raw_int &= 0x01 << (chn_num); /* check interception */
    symc_write(OUT_SYM_CHAN_RAW_LAST_NODE_INT, last_raw.u32);

    level_raw.u32 = symc_read(OUT_SYM_CHAN_RAW_LEVEL_INT);
    level_raw.bits.out_sym_level_chan_raw_int &= 0x01 << (chn_num); /* check interception */
    symc_write(OUT_SYM_CHAN_RAW_LEVEL_INT, level_raw.u32);

    tag_raw.u32 = symc_read(SYM_CHANN_RAW_INT);
    tag_raw.bits.sym_chann_raw_int &= 0x01 << (chn_num);
    symc_write(SYM_CHANN_RAW_INT, tag_raw.u32);

    mask = last_raw.bits.out_sym_chan_raw_int
           | level_raw.bits.out_sym_level_chan_raw_int
           | tag_raw.bits.sym_chann_raw_int;
    if (mask == 0) {
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    return HI_SUCCESS;
}

void drv_symc_set_dma(hi_u32 chn_num, hi_u32 dma_enable)
{
    in_sym_out_ctrl cipher_dma_ctrl;

    cipher_dma_ctrl.u32 = symc_read(IN_SYM_OUT_CTRL(chn_num));
    cipher_dma_ctrl.bits.sym_dma_copy = dma_enable;
    symc_write(IN_SYM_OUT_CTRL(chn_num), cipher_dma_ctrl.u32);
    hi_log_debug("IN_SYM_OUT_CTRL[0x%x]  :0x%x\n", IN_SYM_OUT_CTRL(chn_num), cipher_dma_ctrl.u32);

    return;
}

/* ! set smmu */
void drv_symc_smmu_bypass(hi_u32 chn)
{
    in_sym_chn_ctrl in_ctrl;
    out_sym_chn_ctrl out_ctrl;

    hi_log_func_enter();

    in_ctrl.u32 = symc_read(IN_SYM_CHN_CTRL(chn));
    out_ctrl.u32 = symc_read(OUT_SYM_CHN_CTRL(chn));

#ifdef CRYPTO_SMMU_SUPPORT
    in_ctrl.bits.sym_chn_mmu_en      = 0x01;
    out_ctrl.bits.out_sym_chn_mmu_en = 0x01;
#else
    in_ctrl.bits.sym_chn_mmu_en      = 0x00;
    out_ctrl.bits.out_sym_chn_mmu_en = 0x00;
#endif

    symc_write(IN_SYM_CHN_CTRL(chn), in_ctrl.u32);
    hi_log_debug("IN_SYM_CHN_CTRL[0x%x]  :0x%x\n", IN_SYM_CHN_CTRL(chn), in_ctrl.u32);

    symc_write(OUT_SYM_CHN_CTRL(chn), out_ctrl.u32);
    hi_log_debug("OUT_SYM_CHN_CTRL[0x%x] :0x%x\n", OUT_SYM_CHN_CTRL(chn), out_ctrl.u32);

    hi_log_func_exit();
}

/* ! smmu set base address */
static void drv_symc_smmu_base_addr(void)
{
#ifdef CRYPTO_SMMU_SUPPORT
    hi_ulong err_raddr = 0;
    hi_ulong err_waddr = 0;
    hi_ulong table_addr = 0;

    hi_log_func_enter();

    /* get table base addr from system api */
    smmu_get_table_addr(&err_raddr, &err_waddr, &table_addr);
    symc_write(SPACC_MMU_NOSEC_TLB, (hi_u32)table_addr >> 4);                /* bit[35:4]: mmu base address */
    symc_write (SPACC_MMU_IN_SYM_NOSEC_EADDR_H, (hi_u32)(err_raddr >> 32));  /* high 32 bit of addr */
    symc_write (SPACC_MMU_IN_SYM_NOSEC_EADDR_L, (hi_u32)(err_raddr));        /* low 32 bit of addr */
    symc_write (SPACC_MMU_OUT_SYM_NOSEC_EADDR_H, (hi_u32)(err_waddr >> 32)); /* high 32 bit of addr */
    symc_write (SPACC_MMU_OUT_SYM_NOSEC_EADDR_L, (hi_u32)(err_waddr));       /* low 32 bit of addr */
    hi_log_debug("SPACC_MMU_NOSEC_TLB[0x%x]  : 0x%llx\n", SPACC_MMU_NOSEC_TLB, table_addr);
#endif

    hi_log_func_exit();
    return;
}

/* ! set symc entry */
hi_s32 drv_symc_clear_entry(hi_u32 chn_num)
{
    hi_u32 i;
    hi_u32 timeout = CRYPTO_TIME_OUT;
    hi_u32 mask;
    spacc_sym_chn_clear_req symc_chn_clear;
    spacc_int_raw_sym_clear_finish symc_chn_clear_finish;
    spacc_calc_inc_cfg calc_inc_cfg;

    hi_log_func_enter();

    mask = 0x1 << chn_num;
    symc_chn_clear.u32 = symc_read(SPACC_SYM_CHN_CLEAR_REQ);
    symc_chn_clear.bits.sym_chn_clear_req |= mask;
    symc_write(SPACC_SYM_CHN_CLEAR_REQ, symc_chn_clear.u32);

    calc_inc_cfg.u32 = symc_read(SPACC_CALC_INC_CFG);
    calc_inc_cfg.bits.spacc_gcm_incr_disable = 1;
    calc_inc_cfg.bits.spacc_ccm_incr_disable = 1;
    symc_write(SPACC_CALC_INC_CFG, calc_inc_cfg.u32);

    for (i = 0; i < timeout; i++) {
        symc_chn_clear_finish.u32 = symc_read(SPACC_INT_RAW_SYM_CLEAR_FINISH);
        symc_chn_clear_finish.bits.int_raw_sym_clear_finish &= mask;
        if (symc_chn_clear_finish.bits.int_raw_sym_clear_finish != 0) {
            symc_write(SPACC_INT_RAW_SYM_CLEAR_FINISH, symc_chn_clear.u32);
            break;
        }
        if (i <= MS_TO_US) {
            crypto_udelay(1); /* short waitting for 1000 us */
        } else {
            crypto_msleep(1); /* long waitting for 5000 ms */
        }
    }
    if (timeout <= i) {
        hi_log_error("symc wait clear channel timeout, chn=%d\n", chn_num);
        hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
        return HI_ERR_CIPHER_TIMEOUT;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* ! set symc entry */
static hi_s32 drv_symc_recover_entry(hi_u32 chn)
{
    in_sym_chn_node_wr_point cipher_in_cfg;
    out_sym_chn_node_wr_point cipher_out_cfg;
    in_sym_chn_node_length cipher_in_depth;
    out_sym_chn_node_length cipher_out_depth;
    in_sym_chn_key_ctrl cipher_key_ctrl;
    symc_hard_context *ctx = &hard_context[chn];
    compat_addr out_addr;
    hi_u32 entry;
    hi_s32 ret;

    hi_log_func_enter();

    /* Disable DMA_COPY */
    drv_symc_set_dma(chn, 0x0);
    ret = drv_symc_clear_entry(chn);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_clear_entry, ret);
        return ret;
    }

    /* set total num and start addr for cipher in node
     * On ree, the chn may be seized by tee,
     * so we must check it, that is check we can write the reg of chn or not.
     */
    symc_write(IN_SYM_CHN_NODE_START_ADDR_L(chn), ADDR_L32(ctx->dma_entry));
    symc_write(IN_SYM_CHN_NODE_START_ADDR_H(chn), ADDR_H32(ctx->dma_entry));
    entry = symc_read(IN_SYM_CHN_NODE_START_ADDR_L(chn));
    if (entry != ADDR_L32(ctx->dma_entry)) {
        hi_log_error("the ree chn %d be seized by tee -- entry : 0x%x, dma_entry : 0x%x.\n",
                     chn, entry, ADDR_L32(ctx->dma_entry));
        hi_log_print_err_code(HI_ERR_CIPHER_UNAVAILABLE);
        return HI_ERR_CIPHER_UNAVAILABLE;
    }
    cipher_in_cfg.u32 = symc_read(IN_SYM_CHN_NODE_WR_POINT(chn));
    ctx->id_in = cipher_in_cfg.bits.sym_chn_node_wr_point;
    hi_log_info("symc chn %d recover, id in 0x%x, IN_NODE_START_ADDR 0x%x%08x, VIA %p\n",
                chn, ctx->id_in, ADDR_H32(ctx->dma_entry), ADDR_L32(ctx->dma_entry), ctx->entry_in);

    /* set total num and start addr for cipher out node */
    cipher_out_cfg.u32 = symc_read(OUT_SYM_CHN_NODE_WR_POINT(chn));
    ADDR_U64(out_addr) = ADDR_U64(ctx->dma_entry) + SYMC_IN_NODE_SIZE;
    symc_write(OUT_SYM_CHN_NODE_START_ADDR_L(chn), ADDR_L32(out_addr));
    symc_write(OUT_SYM_CHN_NODE_START_ADDR_H(chn), ADDR_H32(out_addr));
    ctx->id_out = cipher_out_cfg.bits.out_sym_chn_node_wr_point;
    hi_log_info("symc chn %d recover, id out 0x%x, OUT_NODE_START_ADDR 0x%x%08x, VIA %p\n",
                chn, ctx->id_out, ADDR_H32(out_addr), ADDR_L32(out_addr), ctx->entry_out);

    cipher_in_depth.u32 = symc_read(IN_SYM_CHN_NODE_LENGTH(chn));
    cipher_in_depth.bits.sym_chn_node_length = SYMC_MAX_LIST_NUM;
    symc_write(IN_SYM_CHN_NODE_LENGTH(chn), cipher_in_depth.u32);
    hi_log_info("ctx: %p, CHAN[%d]IN_SYM_CHN_NODE_LENGTH[0x%x]: \t0x%x\n",
                ctx, chn, IN_SYM_CHN_NODE_LENGTH(chn), cipher_in_depth.u32);

    cipher_out_depth.u32 = symc_read(OUT_SYM_CHN_NODE_LENGTH(chn));
    cipher_out_depth.bits.out_sym_chn_node_length = SYMC_MAX_LIST_NUM;
    symc_write(OUT_SYM_CHN_NODE_LENGTH(chn), cipher_out_depth.u32);
    hi_log_info("ctx: %p, CHAN[%d]IN_SYM_CHN_NODE_LENGTH[0x%x]: \t0x%x\n",
                ctx, chn, IN_SYM_CHN_NODE_LENGTH(chn), cipher_in_depth.u32);

    /* default key seclect */
    cipher_key_ctrl.u32 = symc_read(IN_SYM_CHN_KEY_CTRL(chn));
    cipher_key_ctrl.bits.sym_key_chn_id = chn;
    symc_write(IN_SYM_CHN_KEY_CTRL(chn), cipher_key_ctrl.u32);

    drv_symc_smmu_bypass(chn);

#ifdef CRYPTO_OS_INT_SUPPORT
    drv_symc_set_interrupt(chn);
#endif

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* ! set symc entry */
static void drv_symc_set_entry(hi_u32 chn, compat_addr dma_addr, void *cpu_addr)
{
    symc_hard_context *ctx = &hard_context[chn];

    hi_log_func_enter();

    /* set total num and start addr for cipher in node */
    hi_log_info("CHAN[%-2d] IN Node: PHY 0x%x, VIA %p\n", chn, ADDR_L32(dma_addr), cpu_addr);
    ctx->entry_in = (symc_entry_in *)cpu_addr;
    ctx->cnt_in = 0;

    ADDR_U64(dma_addr) += SYMC_IN_NODE_SIZE;
    cpu_addr = (u8 *)cpu_addr + SYMC_IN_NODE_SIZE;
    /* set total num and start addr for cipher out node */
    hi_log_info("CHAN[%-2d] OUT Node: PHY 0x%x, VIA %p\n", chn, ADDR_L32(dma_addr), cpu_addr);
    ctx->entry_out = (symc_entry_out *)cpu_addr;
    ctx->cnt_out = 0;

    hi_log_func_exit();
}

/* ! set symc pad buffer */
static void drv_symc_set_pad_buffer(hi_u32 chn, compat_addr dma_addr, void *cpu_addr)
{
    symc_hard_context *ctx = &hard_context[chn];

    ADDR_U64(ctx->dma_pad) = ADDR_U64(dma_addr);
    ctx->via_pad = cpu_addr;
    ctx->offset_pad = 0x00;
    hi_log_info("CHAN[%-2d] PAD: PHY 0x%x, VIA %p\n", chn, ADDR_L32(dma_addr), cpu_addr);
}

/* ! set secure channel,
 *  non-secure CPU can't change the value of SEC_CHN_CFG,
 *  so non-secure CPU call this function will do nothing.
 */
static void drv_symc_enable_secure(hi_u32 chn, hi_u32 ss, hi_u32 ds)
{
    in_sym_chn_ctrl chn_ctrl;

    /* key seclect */
    chn_ctrl.u32 = symc_read(IN_SYM_CHN_CTRL(chn));
    chn_ctrl.bits.sym_chn_ss = ss;
    chn_ctrl.bits.sym_chn_ds = ds;
    symc_write(IN_SYM_CHN_CTRL(chn), chn_ctrl.u32);
    hi_log_debug("IN_SYM_CHN_CTRL[0x%x]  :0x%x\n", IN_SYM_CHN_CTRL(chn), chn_ctrl.u32);

    return;
}

static void drv_symc_print_last_node(hi_u32 chn_num)
{
    symc_entry_in *in = HI_NULL;
    symc_entry_out *out = HI_NULL;
    symc_hard_context *ctx = HI_NULL;

    hi_log_func_enter();

    ctx = &hard_context[chn_num];

    /* get last in node info */
    if (ctx->id_in == 0x00) {
        in = &ctx->entry_in[SYMC_MAX_LIST_NUM];
    } else {
        in = &ctx->entry_in[ctx->id_in - 1];
    }

    /* get last out node info */
    if (ctx->id_out == 0x00) {
        out = &ctx->entry_out[SYMC_MAX_LIST_NUM];
    } else {
        out = &ctx->entry_out[ctx->id_out - 1];
    }

    hi_log_error("chn %d, src ctrl 0x%x, addr 0x%x, size 0x%x, dest addr 0x%x, size 0x%x\n",
                 chn_num, *(hi_u32 *)in, in->sym_start_addr, in->sym_alg_length,
                 out->sym_start_addr, out->sym_alg_length);
    crypto_unused(in);
    crypto_unused(out);

    hi_log_func_exit();
    return;
}

static void drv_symc_print_status(hi_u32 chn_num)
{
    out_sym_chan_raw_int out_chan_raw_int;
    out_sym_chan_raw_int_en out_chan_raw_int_en;
    out_sym_chan_int out_chan_int;
    in_sym_chn_ctrl in_chn_ctrl;
    spacc_is status;
    hi_u32 chn_lock;

    hi_log_func_enter();

    hi_log_error("IN_SYM_CHN_NODE_START_ADDR_L  : 0x%x\n", symc_read(IN_SYM_CHN_NODE_START_ADDR_L(chn_num)));
    hi_log_error("IN_SYM_CHN_NODE_START_ADDR_H  : 0x%x\n", symc_read(IN_SYM_CHN_NODE_START_ADDR_H(chn_num)));
    hi_log_error("IN_SYM_CHN_NODE_LENGTH        : 0x%x\n", symc_read(IN_SYM_CHN_NODE_LENGTH(chn_num)));
    hi_log_error("IN_SYM_CHN_NODE_WR_POINT      : 0x%x\n", symc_read(IN_SYM_CHN_NODE_WR_POINT(chn_num)));
    hi_log_error("IN_SYM_CHN_NODE_RD_POINT      : 0x%x\n", symc_read(IN_SYM_CHN_NODE_RD_POINT(chn_num)));
    hi_log_error("IN_SYM_CHN_RD_ADDR_H          : 0x%x\n", symc_read(DBG_IN_SYM_CHN_RD_ADDR_H(chn_num)));
    hi_log_error("IN_SYM_CHN_RD_ADDR_L          : 0x%x\n", symc_read(DBG_IN_SYM_CHN_RD_ADDR_L(chn_num)));
    hi_log_error("IN_SYM_CHN_DATA_LEN           : 0x%x\n", symc_read(DBG_IN_SYM_CHN_DATA_LEN(chn_num)));
    hi_log_error("IN_SYM_CHN_KEY_CTRL           : 0x%x\n", symc_read(IN_SYM_CHN_KEY_CTRL(chn_num)));
    hi_log_error("sym_dma_copy                  : 0x%x\n", symc_read(IN_SYM_OUT_CTRL(chn_num)));

    hi_log_error("OUT_SYM_CHN_NODE_START_ADDR_L : 0x%x\n", symc_read(OUT_SYM_CHN_NODE_START_ADDR_L(chn_num)));
    hi_log_error("OUT_SYM_CHN_NODE_START_ADDR_H : 0x%x\n", symc_read(OUT_SYM_CHN_NODE_START_ADDR_H(chn_num)));
    hi_log_error("OUT_SYM_CHN_NODE_LENGTH       : 0x%x\n", symc_read(OUT_SYM_CHN_NODE_LENGTH(chn_num)));
    hi_log_error("OUT_SYM_CHN_NODE_WR_POINT     : 0x%x\n", symc_read(IN_SYM_CHN_NODE_WR_POINT(chn_num)));
    hi_log_error("OUT_SYM_CHN_NODE_RD_POINT     : 0x%x\n", symc_read(IN_SYM_CHN_NODE_RD_POINT(chn_num)));
    hi_log_error("OUT_SYM_CHN_NODE_CTRL         : 0x%x\n", symc_read(OUT_SYM_CHN_NODE_CTRL(chn_num)));
    hi_log_error("DBG_OUT_SYM_CHN_RD_ADDR_L     : 0x%x\n", symc_read(DBG_OUT_SYM_CHN_RD_ADDR_L(chn_num)));
    hi_log_error("DBG_OUT_SYM_CHN_RD_ADDR_H     : 0x%x\n", symc_read(DBG_OUT_SYM_CHN_RD_ADDR_H(chn_num)));
    hi_log_error("DBG_OUT_SYM_CHN_NODE_LEFT_BUF_LEN : 0x%x\n", symc_read(DBG_OUT_SYM_CHN_NODE_LEFT_BUF_LEN(chn_num)));

    in_chn_ctrl.u32 = symc_read(IN_SYM_CHN_CTRL(chn_num));
    hi_log_error("chn enable %d, mmu enable %d, ss 0x%x, ds 0x%x\n", in_chn_ctrl.bits.sym_chn_en,
        in_chn_ctrl.bits.sym_chn_mmu_en, in_chn_ctrl.bits.sym_chn_ss, in_chn_ctrl.bits.sym_chn_ds);

    status.u32 = symc_read(SPACC_IS);
    out_chan_raw_int.u32 = symc_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT);
    out_chan_int.u32 = symc_read(OUT_SYM_CHAN_LAST_NODE_INT);
    out_chan_raw_int_en.u32 = symc_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN);
    chn_lock = symc_read(SPACC_SYM_CHN_LOCK);
    hi_log_error("\nsym_chn_lock 0x%x, chn %d, spacc_int_status 0x%x, chn_obuf_en 0x%x, status 0x%x, raw 0x%x\n",
                 (chn_lock >> (chn_num * 0x2)) & 0x03,
                 chn_num, status.u32,
                 (out_chan_raw_int_en.bits.out_sym_chan_int_en >> (chn_num > 0 ? chn_num - 1 : chn_num)) & 0x01,
                 (out_chan_int.bits.out_sym_chan_int >> (chn_num > 0 ? chn_num - 1 : chn_num)) & 0x01,
                 (out_chan_raw_int.bits.out_sym_chan_raw_int >> (chn_num > 0 ? chn_num - 1 : chn_num)) & 0x01);

    hi_log_error("\nThe cause of time out may be:\n"
                 "\t1. SMMU address invalid\n"
                 "\t2. interrupt number or name incorrect\n"
                 "\t3. CPU type mismatching, request CPU and channel: %s\n",
                 crypto_is_sec_cpu() ? "secure" : "non-secure");

    /* avoid compile error when hi_log_error be defined to empty */
    crypto_unused(status);
    crypto_unused(chn_lock);
    crypto_unused(out_chan_raw_int);
    crypto_unused(out_chan_raw_int_en);
    crypto_unused(out_chan_int);

    hi_log_func_exit();

    return;
}

static void drv_symc_print_smmu_error(hi_u32 chn_num)
{
#ifdef CRYPTO_SMMU_SUPPORT
    spacc_axi_iraw smmu_raw_int;
    spacc_axi_iraw smmu_raw_set;

    hi_log_func_enter();

    smmu_raw_int.u32 = symc_read(SPACC_AXI_IRAW);
    smmu_raw_set.u32 = 0;

    if (smmu_raw_int.bits.spacc_mmu_in_sym_sec_err_iraw) {
        smmu_raw_set.bits.spacc_mmu_in_sym_sec_err_iraw = 1;
        hi_log_error("symc error: spacc_mmu_in_sym_sec_err_iraw, chn %d !!!\n", chn_num);
    }
    if (smmu_raw_int.bits.spacc_mmu_out_sym_sec_err_iraw) {
        smmu_raw_set.bits.spacc_mmu_out_sym_sec_err_iraw = 1;
        hi_log_error("symc error: spacc_mmu_out_sym_sec_err_iraw, chn %d !!!\n", chn_num);
    }
    if (smmu_raw_int.bits.spacc_mmu_in_sym_nosec_err_iraw) {
        smmu_raw_set.bits.spacc_mmu_in_sym_nosec_err_iraw = 1;
        hi_log_error("symc error: spacc_mmu_in_sym_nosec_err_iraw, chn %d !!!\n", chn_num);
    }
    if (smmu_raw_int.bits.spacc_mmu_out_sym_sec_err_iraw) {
        smmu_raw_set.bits.spacc_mmu_out_sym_sec_err_iraw = 1;
        hi_log_error("symc error: spacc_mmu_out_sym_nosec_err_iraw, chn %d !!!\n", chn_num);
    }

    /* clear error int */
    symc_write(SPACC_AXI_IRAW, smmu_raw_set.u32);

    hi_log_func_exit();
#endif

    return;
}

static hi_s32 drv_symc_get_err_code(hi_u32 chn_num)
{
    sym_calc_ctrl_check_err code;
    sym_calc_ctrl_check_err_status status;

    hi_log_func_enter();

    /* read error code */
    code.u32 = symc_read(SYM_CALC_CTRL_CHECK_ERR(SPACC_CPU_CUR));
    status.u32 = symc_read(SYM_CALC_CTRL_CHECK_ERR_STATUS(SPACC_CPU_CUR));

    if (code.bits.calc_ctrl_check_ds_error) {
        hi_log_error("symc error: in ctrl input ds doesn't match key info ds/dns.\n");
    }
    if (code.bits.calc_ctrl_check_ss_error) {
        hi_log_error("symc error: in ctrl input ss doesn't match key info ss/sns.\n");
    }
    if (code.bits.calc_ctrl_check_alg_error) {
        hi_log_error("symc error: in ctr input algorithm doesn't match key info status.\n");
    }
    if (code.bits.calc_ctrl_check_alg_invld_error) {
        hi_log_error("symc error: in ctrl input algorithm mode is invaild.\n");
    }
    if (code.bits.calc_ctrl_check_dec_error) {
        hi_log_error("symc error: in ctrl input dec doesn't match key info status.\n");
    }
    if (code.bits.calc_ctrl_check_error) {
        hi_log_error("symc error: key info is hpp dedicated, but channel lock doesn't hpp.\n");
    }
    if (code.bits.calc_ctrl_check_sc_error) {
        hi_log_error("symc error: key info is secure, but channel lock is ree.\n");
    }
    if (code.bits.calc_ctrl_check_key_size_error) {
        hi_log_error("symc error: in ctrl input key size doesn't match key info key size.\n");
    }
    if (code.bits.calc_ctrl_check_hdcp_error) {
        hi_log_error("symc error: When HDCP calculation, hw key isn't HDCP key.\n");
    }
    if (code.bits.calc_ctrl_check_cenc_dec_error) {
        hi_log_error("symc error: When cenc enable ,input data is used for encrypt.\n");
    }
    if (code.bits.calc_ctrl_check_cenc_alg_error) {
        hi_log_error("symc error: When cenc enable, input algorithm isn't AES/SM4 CBC/CTR.\n");
    }
    if (code.bits.calc_ctrl_check_gcm_ccm_error) {
        hi_log_error("symc error: When algorithm mode is AES GCM/CCM ,sym_ccm_gcm_in_fla is illegal.\n");
    }
    if (code.bits.calc_ctrl_check_iv_length_error) {
        hi_log_error("symc error: When mode is GCM/CTR, in ctrl iv length beyond 16byte or 0 byte is vaild.\n");
    }
    if (code.bits.calc_ctrl_check_data_length_error) {
        hi_log_error("symc error: IN CTRL input data length beyond 16 byte or 0 byte is vaild.\n");
    }
    if (code.bits.calc_ctrl_big_key_info_diff_error) {
        hi_log_error("symc error: When key size is 192/256bit, but key info doesn't match.\n");
    }
    if (code.bits.calc_ctrl_hdcp_key_size_error) {
        hi_log_error("symc error: when channel 0 HDCP is enable ,Hdcp key is static key but key size isn't 128bit.\n");
    }
    if (code.bits.calc_ctrl_hdcp_sm4_error) {
        hi_log_error("symc error: when channel 0 HDCP is enable ,SW select SM4 algorithm but OTP sm4_disable is 1.\n");
    }
    if (code.bits.calc_ctrl_des_keysame_error) {
        hi_log_error("symc error: des core's tdes key same error.\n");
    }

    if (code.u32) {
        hi_log_error("symc error chn %d !!!\n", status.bits.hpp_error_sym_chan_id);
        hi_log_error("SYM_CALC_CTRL_CHECK_ERR[0x%x]: 0x%x\n",
            SYM_CALC_CTRL_CHECK_ERR(SPACC_CPU_CUR), code.u32);

        /* clear error code */
        status.bits.hpp_sym_error_code_clr = 1;
        symc_write(SYM_CALC_CTRL_CHECK_ERR_STATUS(SPACC_CPU_CUR), status.u32);

        /* print the inout buffer address */
        drv_symc_print_smmu_error(chn_num);
        drv_symc_print_last_node(chn_num);
        drv_symc_print_status(chn_num);

        return HI_ERR_CIPHER_HARD_STATUS;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_check_mmu(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    spacc_mmu_in_sym_nosec_unvld_va_tab in_va_tab;
    spacc_mmu_in_sym_nosec_unvld_pa_tab in_pa_tab;
    spacc_mmu_out_sym_nosec_unvld_va_tab out_va_tab;
    spacc_mmu_out_sym_nosec_unvld_pa_tab out_pa_tab;

    /* read error code */
    in_va_tab.u32 = symc_read(SPACC_MMU_IN_SYM_NOSEC_UNVLD_VA_TAB);
    in_pa_tab.u32 = symc_read(SPACC_MMU_IN_SYM_NOSEC_UNVLD_PA_TAB);
    if (in_va_tab.bits.mmu_in_sym_nosec_unvld_va_vld == VALID_SIGNAL) {
        hi_log_error("SPACC_MMU_IN_SYM_NOSEC_UNVLD_VA_TAB[0x%x]: 0x%x\n",
            SPACC_MMU_IN_SYM_NOSEC_UNVLD_PA_TAB, in_va_tab.u32);
        hi_log_error("mmu_in_sym_nosec_unvld_chn_id %d !!!\n", in_va_tab.bits.mmu_in_sym_nosec_unvld_chn_id);
        hi_log_error("mmu_in_sym_nosec_unvld_va_tab 0x%x !!!\n",
            in_va_tab.bits.mmu_in_sym_nosec_unvld_va_tab << SMMU_PAGE_BIT);
        ret = HI_ERR_CIPHER_INVALID_ADDR;
    }
    if (in_pa_tab.bits.mmu_in_sym_nosec_unvld_pa_vld == VALID_SIGNAL) {
        hi_log_error("SPACC_MMU_IN_SYM_NOSEC_UNVLD_PA_TAB[0x%x]: 0x%x\n",
            SPACC_MMU_IN_SYM_NOSEC_UNVLD_PA_TAB, in_pa_tab.u32);
        hi_log_error("mmu_in_sym_nosec_unvld_vld_err %d !!!\n", in_pa_tab.bits.mmu_in_sym_nosec_unvld_vld_err);
        hi_log_error("mmu_in_sym_nosec_unvld_pa_tab 0x%x !!!\n",
            in_pa_tab.bits.mmu_in_sym_nosec_unvld_pa_tab << SMMU_PAGE_BIT);
        ret = HI_ERR_CIPHER_INVALID_ADDR;
    }

    out_va_tab.u32 = symc_read(SPACC_MMU_OUT_SYM_NOSEC_UNVLD_VA_TAB);
    out_pa_tab.u32 = symc_read(SPACC_MMU_OUT_SYM_NOSEC_UNVLD_PA_TAB);
    if (out_va_tab.bits.mmu_out_sym_nosec_unvld_va_vld == VALID_SIGNAL) {
        hi_log_error("SPACC_MMU_OUT_SYM_NOSEC_UNVLD_VA_TAB[0x%x]: 0x%x\n",
            SPACC_MMU_OUT_SYM_NOSEC_UNVLD_VA_TAB, out_va_tab.u32);
        hi_log_error("mmu_out_sym_nosec_unvld_chn_id %d !!!\n", out_va_tab.bits.mmu_out_sym_nosec_unvld_chn_id);
        hi_log_error("mmu_out_sym_nosec_unvld_va_tab 0x%x !!!\n",
            out_va_tab.bits.mmu_out_sym_nosec_unvld_va_tab << SMMU_PAGE_BIT);
        ret = HI_ERR_CIPHER_INVALID_ADDR;
    }
    if (out_pa_tab.bits.mmu_out_sym_nosec_unvld_pa_vld == VALID_SIGNAL) {
        hi_log_error("SPACC_MMU_OUT_SYM_NOSEC_UNVLD_PA_TAB[0x%x]: 0x%x\n",
            SPACC_MMU_OUT_SYM_NOSEC_UNVLD_PA_TAB, out_pa_tab.u32);
        hi_log_error("mmu_in_sym_nosec_unvld_vld_err %d !!!\n", out_pa_tab.bits.mmu_out_sym_nosec_unvld_vld_err);
        hi_log_error("mmu_in_sym_nosec_unvld_pa_tab 0x%x !!!\n",
            out_pa_tab.bits.mmu_out_sym_nosec_unvld_pa_tab << SMMU_PAGE_BIT);
        ret = HI_ERR_CIPHER_INVALID_ADDR;
    }

    /* clear error code */
    symc_write(SPACC_MMU_IN_SYM_NOSEC_UNVLD_VA_TAB,  in_va_tab.u32);
    symc_write(SPACC_MMU_IN_SYM_NOSEC_UNVLD_PA_TAB,  in_pa_tab.u32);
    symc_write(SPACC_MMU_OUT_SYM_NOSEC_UNVLD_VA_TAB, out_va_tab.u32);
    symc_write(SPACC_MMU_OUT_SYM_NOSEC_UNVLD_PA_TAB, out_pa_tab.u32);

    return ret;
}

static void drv_symc_entry_init(crypto_mem mem, crypto_mem padding)
{
    hi_u32 i = 0;
    void *cpu_addr = HI_NULL;
    compat_addr dma_pad;
    hi_u8 *via_pad = HI_NULL;

    hi_log_func_enter();

    /* set in node and out node dma buffer */
    hi_log_debug("symc entry list configure\n");
    cpu_addr = mem.dma_virt;

    /* skip the in node and out node dma buffer */
    ADDR_U64(dma_pad) = ADDR_U64(padding.dma_addr);
    via_pad = (hi_u8 *)padding.dma_virt;

    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if ((CIPHER_HARD_CHANNEL_MASK >> i) & 0x01) { /* valid channel */
            /* in node and out node */
            drv_symc_set_entry(i, mem.dma_addr, cpu_addr);
            ADDR_U64(hard_context[i].dma_entry) = ADDR_U64(mem.dma_addr);

            ADDR_U64(mem.dma_addr) += SYMC_NODE_SIZE;      /* move to next channel */
            cpu_addr = (hi_u8 *)cpu_addr + SYMC_NODE_SIZE; /* move to next channel */

            /* padding */
            drv_symc_set_pad_buffer(i, dma_pad, via_pad);
            ADDR_U64(dma_pad) += SYMC_PAD_BUFFER_LEN;
            via_pad += SYMC_PAD_BUFFER_LEN;
        }
    }

    hi_log_func_exit();
    return;
}

static hi_s32 drv_symc_mem_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    /** dma buffer struct
      * ((in_node || out_node) * chn_num) || (pad_buffer * chn_num)
      */
    hi_log_debug("alloc memory for nodes list\n");
    ret = crypto_mem_create(&symc_dma, MEM_TYPE_MMZ, "CIPHER_NODE", SYMC_NODE_LIST_SIZE);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, malloc ddr for symc nodes list failed\n");
        hi_log_print_func_err(crypto_mem_create, ret);
        return ret;
    }

    hi_log_debug("alloc memory for padding buffer\n");
#ifdef HI_SMMU_SUPPORT
    ret = crypto_mem_create(&padding_dma, MEM_TYPE_SMMU, "CIPHER_PADDING", SYMC_PAD_BUFFER_TOTAL_LEN);
#else
    ret = crypto_mem_create(&padding_dma, MEM_TYPE_MMZ, "CIPHER_PADDING", SYMC_PAD_BUFFER_TOTAL_LEN);
#endif
    if (ret != HI_SUCCESS) {
        hi_log_error("error, malloc ddr for symc padding buffer failed\n");
        hi_log_print_func_err(crypto_mem_create, ret);
        (hi_void)crypto_mem_destory(&symc_dma);
        return ret;
    }
    hi_log_debug("SYMC DMA buffer, MMZ 0x%x, VIA %p, size 0x%x\n",
                 ADDR_L32(symc_dma.dma_addr),
                 symc_dma.dma_virt, symc_dma.dma_size);

    drv_symc_entry_init(symc_dma, padding_dma);

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 drv_symc_mem_deinit(void)
{
    hi_s32 ret;

    ret = crypto_mem_destory(&symc_dma);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_mem_destory, ret);
        return ret;
    }
    ret = crypto_mem_destory(&padding_dma);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_mem_destory, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_symc_chn_resume(hi_u32 chn_num)
{
    hi_s32 ret;
    hi_u32 base;

    hi_log_func_enter();

#ifdef CRYPTO_SMMU_SUPPORT
    if (crypto_is_sec_cpu()) {
        base = symc_read(SPACC_MMU_SEC_TLB);
    } else {
        base = symc_read(SPACC_MMU_NOSEC_TLB);
    }
#else
    base = symc_read(IN_SYM_CHN_NODE_START_ADDR_L(chn_num));
#endif

    if (base == 0) {
        /* smmu base address is zero means hardware be unexpected reset */
        hi_log_warn("cipher module is not ready, try to resume it now...\n");
        ret = drv_symc_resume();
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_resume, ret);
            return ret;
        }
    }

    crypto_unused(chn_num);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_symc_alloc_chn(hi_u32 *chn_num, hi_cipher_type type)
{
    hi_s32 ret;
    hi_u32 used;
    hi_u32 chnn_who_used = 0;
    hi_u32 my_cpu = SPACC_CPU_REE;
    hi_u32 i = 0;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_unused(type);

    if (crypto_is_sec_cpu() == HI_TRUE) {
        my_cpu = SPACC_CPU_TEE;
    }

    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if (hard_context[i].open == HI_TRUE) {
            continue;
        }

        if (((0x1 << i) & CIPHER_HARD_CHANNEL_MASK) == 0x00) {
            continue;
        }

        used = symc_read(SPACC_SYM_CHN_LOCK);
        chnn_who_used = CHN_WHO_USED_GET(used, i);

        if (chnn_who_used == SPACC_CPU_IDLE) {
            /* try to use this channel */
            CHN_WHO_USED_SET(used, i, my_cpu);
            symc_write(SPACC_SYM_CHN_LOCK, used);

            /* check if the channal aleardy be useded by other cpu
             * if other cpu break-in when write the CIPHER_NON_SEC_CHN_WHO_USED
             * the value of CIPHER_NON_SEC_CHN_WHO_USED will be channged
             */
            used = symc_read(SPACC_SYM_CHN_LOCK);
            chnn_who_used = CHN_WHO_USED_GET(used, i);
            if (chnn_who_used != my_cpu) {
                /* chn aleardy be used by tee */
                continue;
            }

            /* alloc channel */
            hard_context[i].open = HI_TRUE;
            *chn_num = i;
            hi_log_debug("+++ alloc symc chn %d +++\n", i);
            break;
        }
    }

    if (i >= CRYPTO_HARD_CHANNEL_MAX) {
        hi_log_error("symc alloc channel failed\n");
        hi_log_print_err_code(HI_ERR_CIPHER_BUSY);
        return HI_ERR_CIPHER_BUSY;
    }

    ret = drv_symc_recover_entry(i);
    if (ret != HI_SUCCESS) {
        CHN_WHO_USED_CLR(used, i);
        symc_write(SPACC_SYM_CHN_LOCK, used);
        hi_log_print_func_err(drv_symc_recover_entry, ret);
        return ret;
    }

    /* hardware may be unexpected reset by other module or platform,
     * such as unexpected reset by fastboot after load tee image,
     * in this case, the hardware configuration will be reset,
     * here try to re-config the hardware.
     */
    ret = drv_symc_chn_resume(*chn_num);
    if (ret != HI_SUCCESS) {
        CHN_WHO_USED_CLR(used, i);
        symc_write(SPACC_SYM_CHN_LOCK, used);
        hi_log_print_func_err(drv_symc_chn_resume, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

void drv_symc_free_chn(hi_u32 chn_num)
{
    hi_u32 used;

    hi_log_func_enter();

    if (hard_context[chn_num].open == HI_FALSE) {
        hi_log_func_exit();
        return;
    }

    /* set channel to idle */
    used = symc_read(SPACC_SYM_CHN_LOCK);
    CHN_WHO_USED_CLR(used, chn_num);
    symc_write(SPACC_SYM_CHN_LOCK, used);

    drv_symc_enable_secure(chn_num, SYMC_CFG_NON_SECURE, SYMC_CFG_NON_SECURE);

    if (hard_context[chn_num].destory != HI_NULL) {
        hard_context[chn_num].destory();
        hard_context[chn_num].destory = HI_NULL;
    }

    /* free channel */
    hard_context[chn_num].open = HI_FALSE;

    hi_log_debug("--- free symc chn %d ---\n", chn_num);

    hi_log_func_exit();
    return;
}

#ifdef CRYPTO_SWITCH_CPU
hi_u32 drv_symc_is_secure(void)
{
    hi_u32 used;
    hi_u32 secure = HI_FALSE;

    hi_log_func_enter();

    hi_log_debug("Change the secure type of the chn0 to get cpu type\n");
    module_enable(CRYPTO_MODULE_ID_SYMC);

    used = symc_read(SPACC_SYM_CHN_LOCK);

    /* change the secure type of chn0 */
    CHN_WHO_USED_SET(used, 0, SPACC_CPU_TEE);
    symc_write(SPACC_SYM_CHN_LOCK, used);

    /* read the secure type of chn0 */
    used = symc_read(SPACC_SYM_CHN_LOCK);

    if (CHN_WHO_USED_GET(used, 0) == SPACC_CPU_TEE) {
        /* The REG_SEC_CHN_CFG only can be set by secure CPU
         * can write the cfg, must be secure CPU
         */
        secure = HI_TRUE;

        /* recovery the secure type of chn0 */
        CHN_WHO_USED_CLR(used, 0);
        symc_write(SPACC_SYM_CHN_LOCK, used);
    }

    hi_log_info("CPU secure type: 0x%x\n", secure);

    hi_log_func_exit();
    return secure;
}
#endif

static hi_void drv_symc_low_power(hi_void)
{
    spacc_calc_crg_cfg cfg;

    cfg.u32 = symc_read(SPACC_CALC_CRG_CFG);
    cfg.bits.spacc_aes_low_power_enable  = SYMC_LOW_POWER_ENABLE;
    cfg.bits.spacc_tdes_low_power_enable = SYMC_LOW_POWER_ENABLE;
    cfg.bits.spacc_sm4_low_power_enable  = SYMC_LOW_POWER_ENABLE;
    symc_write(SPACC_CALC_CRG_CFG, cfg.u32);
}

hi_s32 drv_symc_init(void)
{
    hi_s32 ret;
    hi_u32 i = 0;

    hi_log_func_enter();

    if (symc_initialize == HI_TRUE) {
        hi_log_func_exit();
        return HI_SUCCESS;
    }

    hard_context = crypto_calloc(sizeof(symc_hard_context), CRYPTO_HARD_CHANNEL_MAX);
    if (hard_context == HI_NULL) {
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    ret = memset_s(&symc_dma, sizeof(symc_dma), 0, sizeof(symc_dma));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    hi_log_debug("enable symc\n");
    module_disable(CRYPTO_MODULE_ID_SYMC);
    module_enable(CRYPTO_MODULE_ID_SYMC);

    ret = drv_symc_mem_init();
    if (ret != HI_SUCCESS) {
        goto __error;
    }

    drv_symc_smmu_base_addr();
    drv_symc_low_power();

#ifdef CRYPTO_OS_INT_SUPPORT
    hi_log_debug("symc interrupt configure\n");
    drv_symc_interrupt_init();

    hi_log_debug("symc register interrupt function\n");
    ret = drv_symc_register_interrupt();
    if (ret != HI_SUCCESS) {
        hi_log_error("error, register interrupt failed\n");
        hi_log_print_func_err(drv_symc_register_interrupt, ret);
        drv_symc_mem_deinit();
        goto __error;
    }
#endif

    /* set all channel as non-secure channel,
     *  may be set it to secure channel when alloc handle.
     */
    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        /* check the valid channel */
        if (CIPHER_HARD_CHANNEL_MASK & (0x1 << i)) {
            drv_symc_enable_secure(i, SYMC_CFG_NON_SECURE, SYMC_CFG_NON_SECURE);
        }
    }

    symc_initialize = HI_TRUE;
    symc_write(SPACC_SYM_CHN_LOCK, 0x00);

    hi_log_func_exit();
    return HI_SUCCESS;

__error:
    module_disable(CRYPTO_MODULE_ID_SYMC);
    crypto_free(hard_context);
    hard_context = HI_NULL;

    hi_log_func_exit();
    return ret;
}

hi_s32 drv_symc_resume(void)
{
    hi_u32 i;
    hi_s32 ret;
    hi_u32 ss = SYMC_CFG_NON_SECURE;
    hi_u32 ds = SYMC_CFG_NON_SECURE;
    hi_u32 my_cpu = SPACC_CPU_REE;
    hi_u32 used;

    hi_log_func_enter();

    hi_log_debug("enable symc\n");
    module_enable(CRYPTO_MODULE_ID_SYMC);
    module_disable(CRYPTO_MODULE_ID_SM4);

    if (crypto_is_sec_cpu() == HI_TRUE) {
        ss = SYMC_CFG_SECURE;
        ds = SYMC_CFG_SECURE;
        my_cpu = SPACC_CPU_TEE;
    }

    for (i = 1; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if (hard_context[i].open) {
            used = symc_read(SPACC_SYM_CHN_LOCK);
            drv_symc_enable_secure(i, ss, ds);
            CHN_WHO_USED_SET(used, i, my_cpu);
            symc_write(SPACC_SYM_CHN_LOCK, used);
            ret = drv_symc_recover_entry(i);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(drv_symc_recover_entry, ret);
                return ret;
            }
        }
    }

#ifdef CRYPTO_OS_INT_SUPPORT
    drv_symc_interrupt_init();
#endif

    drv_symc_entry_init(symc_dma, padding_dma);
    drv_symc_smmu_base_addr();

    hi_log_func_exit();
    return HI_SUCCESS;
}

void drv_symc_suspend(void)
{
    return;
}

hi_s32 drv_symc_deinit(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);

    ret = drv_symc_mem_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_mem_deinit, ret);
        return ret;
    }
    module_disable(CRYPTO_MODULE_ID_SYMC);

#ifdef CRYPTO_OS_INT_SUPPORT
    drv_symc_unregister_interrupt();
#endif

    crypto_free(hard_context);
    hard_context = HI_NULL;
    symc_initialize = HI_FALSE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

void drv_symc_add_buf_usage(hi_u32 chn_num, hi_u32 in, symc_node_usage usage)
{
    symc_hard_context *ctx = HI_NULL;
    hi_u32 id;

    ctx = &hard_context[chn_num];

    if (in == HI_FALSE) {
        /* out node info doesn't need last node flag */
        return;
    }

    /* get last node */
    id = (ctx->id_in == 0) ? SYMC_MAX_LIST_NUM - 1 : ctx->id_in - 1;
    if (usage == SYMC_NODE_USAGE_LAST) {
        ctx->entry_in[id].sym_last_node = 0x1;
    }

    ctx->entry_in[id].aes_ccm_gcm_in_flag |=
        (usage & (SYMC_NODE_USAGE_IN_CCM_A | SYMC_NODE_USAGE_IN_CCM_P)) >> 3; /* 3: bit 1-0 */

    if (usage == SYMC_NODE_USAGE_CCM_LAST) {
        ctx->entry_in[id].sym_ccm_last = 0x1;
    }

    if (usage == SYMC_NODE_USAGE_GCM_FIRST) {
        ctx->entry_in[id].sym_gcm_first = 0x1;
    }

    hi_log_debug("chn %d, add symc in ctrl: id %d, sym_last_node 0x%x\n", chn_num, id, ctx->entry_in[id].sym_last_node);

    return;
}

void drv_symc_set_iv(hi_u32 chn_num, hi_u32 iv[SYMC_IV_MAX_SIZE_IN_WORD], hi_u32 ivlen, hi_u32 flag)
{
    hi_u32 i;
    symc_hard_context *ctx = HI_NULL;

    hi_log_func_enter();

    ctx = &hard_context[chn_num];

    /* copy iv data into channel context */
    for (i = 0; i < SYMC_IV_MAX_SIZE_IN_WORD; i++) {
        ctx->iv[i] = iv[i];
    }
    ctx->iv_flag = flag;
    ctx->iv_len = ivlen;

    hi_log_func_exit();
    return;
}

void drv_symc_get_iv(hi_u32 chn_num, hi_u32 iv[SYMC_IV_MAX_SIZE_IN_WORD], hi_u32 iv_len)
{
    hi_u32 i;

    hi_log_func_enter();

    if ((iv_len / sizeof(hi_u32)) < SYMC_IV_MAX_SIZE_IN_WORD) {
        hi_log_error("error, parameter iv_len invalid.\n");
        return;
    }

    for (i = 0; i < SYMC_IV_MAX_SIZE_IN_WORD; i++) {
        iv[i] = symc_read(CHANn_CIPHER_IVOUT(chn_num) + i * WORD_WIDTH);
        hi_log_info("OUTIV[0x%x]: 0x%x\n", CHANn_CIPHER_IVOUT(chn_num) + i * WORD_WIDTH, iv[i]);
    }

    hi_log_func_exit();
    return;
}

void drv_symc_set_key_source(hi_u32 chn_num, symc_key_source src)
{
    in_sym_chn_key_ctrl cipher_key_ctrl;

    hi_log_func_enter();

    /* key seclect */
    cipher_key_ctrl.u32 = symc_read(IN_SYM_CHN_KEY_CTRL(chn_num));
    cipher_key_ctrl.bits.sym_key_chn_id = src;
    symc_write(IN_SYM_CHN_KEY_CTRL(chn_num), cipher_key_ctrl.u32);

    hi_log_func_exit();
    return;
}

void drv_symc_set_key(hi_u32 chn_num, hi_u32 key[SYMC_KEY_MAX_SIZE_IN_WORD], hi_u32 odd)
{
    /* all of key is come from klad, need not to config */
    crypto_unused(chn_num);
    crypto_unused(key);
    crypto_unused(odd);

    return;
}

hi_s32 drv_symc_add_inbuf(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size, symc_node_usage usage)
{
    hi_s32 ret;
    symc_hard_context *ctx = HI_NULL;
    hi_u32 id;
    hi_u32 size;
    hi_u32 i = 0;
    void *addr = HI_NULL;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    ctx = &hard_context[chn_num];

    id = ctx->id_in;
    addr = &ctx->entry_in[id];
    size = sizeof(symc_entry_in);
    ret = memset_s(addr, size, 0, size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    ctx->entry_in[id].sym_start_addr = ADDR_L32(buf_phy);
    ctx->entry_in[id].sym_start_high = ADDR_H32(buf_phy);
    ctx->entry_in[id].sym_alg_length = buf_size;
    ctx->entry_in[id].sym_first_node = usage & SYMC_NODE_USAGE_FIRST;
    ctx->entry_in[id].sym_last_node  = (usage & SYMC_NODE_USAGE_LAST) >> 1;
    ctx->entry_in[id].odd_even       = (usage & SYMC_NODE_USAGE_ODD_KEY) >> 6; /* bit 6 -> bit 0 */
    ctx->entry_in[id].aes_ccm_gcm_in_flag = (usage & (SYMC_NODE_USAGE_IN_GCM_A |
                                                      SYMC_NODE_USAGE_IN_GCM_P |
                                                      SYMC_NODE_USAGE_IN_GCM_LEN |
                                                      SYMC_NODE_USAGE_IN_CCM_N |
                                                      SYMC_NODE_USAGE_IN_CCM_A |
                                                      SYMC_NODE_USAGE_IN_CCM_P)) >> 0x3; /* 3: bit 1..0 */
    ctx->entry_in[id].sym_ccm_last = (usage & SYMC_NODE_USAGE_CCM_LAST) >> 5;    /* bit 5 -> bit 0 */
    ctx->entry_in[id].sym_gcm_first = (usage & SYMC_NODE_USAGE_GCM_FIRST) >> 16; /* bit 16 -> bit 0 */
    ctx->entry_in[id].sym_gcm_and_ctr_iv_len = ctx->iv_len;

    /* set IV to every node, but the hardware only update the IV
     * from node when first flag is 1
     */
    for (i = 0; i < SYMC_IV_MAX_SIZE_IN_WORD; i++) {
        ctx->entry_in[id].IV[i] = ctx->iv[i];
        hi_log_debug("IV[%d]: 0x%x\n", i, ctx->iv[i]);
    }

    if (ctx->iv_flag == CIPHER_IV_CHANGE_ONE_PKG) {
        /* update iv for first node */
        ctx->iv_flag = 0x00;

        /* don't update iv any more */
        ctx->entry_in[id].sym_first_node = 1;
    } else if (ctx->iv_flag == CIPHER_IV_CHANGE_ALL_PKG) {
        /* update iv for all node */
        ctx->entry_in[id].sym_first_node = 1;
        ctx->entry_in[id].sym_last_node = 1;
    }

    /* move to next node */
    ctx->id_in++;
    ctx->id_in %= SYMC_MAX_LIST_NUM;

    /* total count of computed nodes add 1 */
    ctx->cnt_in++;

    hi_log_info("chn %d, add symc in buf[0x%llx]: id %d, cnt %d, addr 0x%llx, len 0x%x, first_node 0x%x, usage 0x%x\n",
        chn_num, ADDR_U64(ctx->dma_entry), id, ctx->cnt_in,
        ADDR_U64(buf_phy), buf_size, ctx->entry_in[id].sym_first_node, usage);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_symc_add_outbuf(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size, symc_node_usage usage)
{
    hi_s32 ret;
    symc_hard_context *ctx = HI_NULL;
    hi_u32 id;
    hi_u32 size;
    void *addr = HI_NULL;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);
    crypto_unused(usage);

    ctx = &hard_context[chn_num];

    id = ctx->id_out;
    addr = &ctx->entry_out[id];
    size = sizeof(symc_entry_out);
    ret = memset_s(addr, size, 0, size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    ctx->entry_out[id].sym_start_addr = ADDR_L32(buf_phy);
    ctx->entry_out[id].sym_start_high = ADDR_H32(buf_phy);
    ctx->entry_out[id].sym_alg_length = buf_size;

    /* move to next node */
    ctx->id_out++;
    ctx->id_out %= SYMC_MAX_LIST_NUM;

    hi_log_info("chn %d, add symc out buf[0x%llx]: id %d, addr 0x%llx, len 0x%x, ctrl 0x%x\n",
                chn_num, ADDR_U64(ctx->dma_entry) + SYMC_IN_NODE_SIZE,
                id, ADDR_U64(buf_phy), buf_size, usage);

    /* total count of computed nodes add 1 */
    ctx->cnt_out++;

    hi_log_func_exit();
    return HI_SUCCESS;
}

#ifdef HI_PRODUCT_AEAD_SUPPORT
hi_s32 drv_aead_ccm_add_n(hi_u32 chn_num, u8 *n)
{
    hi_s32 ret;
    symc_hard_context *ctx = HI_NULL;
    symc_node_usage usage;
    compat_addr dma_pad;
    hi_u8 *via_pad = HI_NULL;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    ctx = &hard_context[chn_num];

    hi_log_debug("PAD buffer, PHY: 0x%x, VIA %p\n", ADDR_L32(ctx->dma_pad), ctx->via_pad);

    ADDR_U64(dma_pad) = ADDR_U64(ctx->dma_pad) + ctx->offset_pad;
    via_pad = ctx->via_pad + ctx->offset_pad;
    usage = SYMC_NODE_USAGE_IN_CCM_N | SYMC_NODE_USAGE_LAST;
    ret = memcpy_s(via_pad, SYMC_CCM_N_LEN, n, SYMC_CCM_N_LEN);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }
    ctx->offset_pad += SYMC_CCM_N_LEN;

    ret = drv_symc_add_inbuf(chn_num, dma_pad, SYMC_CCM_N_LEN, usage);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_inbuf, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_u32 drv_aead_ccm_a_head(u8 *buf, hi_u32 alen)
{
    hi_u32 idx = 0;

    /* Formatting of the Associated Data in B1, the length of A denotes as a */
    /* The value a is encoded according to the following three cases:
    * If 0 < a < 2^16 - 2^8, then a  is encoded as a[0..15], i.e., two octets.
    * If 2^16 - 2^8 a < 2^32, then a  is encoded as 0xff || 0xfe || a[0..31], i.e., six octets.
    * If 2^32 a < 2^64, then  a is encoded as 0xff || 0xff || a[0..63], i.e., ten octets.
    * For example, if a=2^16, the encoding of a  is
    * 11111111 11111110 00000000 00000001 00000000 00000000.
    */
    if (alen < SYMC_CCM_A_SMALL_LEN) {
        buf[idx++] = (hi_u8)(alen >> 8); /* 8: byte1 */
        buf[idx++] = (hi_u8)(alen);
    } else {
        buf[idx++] = 0xFF;
        buf[idx++] = 0xFE;
        buf[idx++] = (hi_u8)(alen >> 24);  /* 24: byte3 */
        buf[idx++] = (hi_u8)(alen >> 16);  /* 16: byte2 */
        buf[idx++] = (hi_u8)(alen >> 8);   /*  8: byte1 */
        buf[idx++] = (hi_u8)alen;
    }

    return idx;
}

hi_s32 drv_aead_ccm_add_a(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size)
{
    symc_hard_context *ctx = HI_NULL;
    compat_addr dma_pad;
    hi_u8 *via_pad = HI_NULL;
    hi_s32 ret;
    hi_u32 count;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);
    ctx = &hard_context[chn_num];
    crypto_assert((ctx->offset_pad + SYMC_CCM_A_HEAD_LEN < SYMC_PAD_BUFFER_LEN));

    /* return success when alen is zero */
    if (buf_size == 0x00) {
        return HI_SUCCESS;
    }

    ADDR_U64(dma_pad) = ADDR_U64(ctx->dma_pad) + ctx->offset_pad;
    via_pad = ctx->via_pad + ctx->offset_pad;
    ret = memset_s(via_pad, AES_BLOCK_SIZE_IN_BYTE * 2, 0, AES_BLOCK_SIZE_IN_BYTE * 2); /* max 2 block */
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    /* add ccm a head */
    count = drv_aead_ccm_a_head(via_pad, buf_size);

    ret = drv_symc_add_inbuf(chn_num, dma_pad, count, SYMC_NODE_USAGE_IN_CCM_A | SYMC_NODE_USAGE_FIRST);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_inbuf, ret);
        return ret;
    }

    /* move buffer addr */
    ctx->offset_pad += count;
    ADDR_U64(dma_pad) += count;
    via_pad += count;

    /* add the phy of A into node list */
    ret = drv_symc_add_inbuf(chn_num, buf_phy, buf_size, SYMC_NODE_USAGE_IN_CCM_A);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_inbuf, ret);
        return ret;
    }

    /* if idx + Alen do not aligned with 16, padding 0 to the tail */
    count = (buf_size + count) % AES_BLOCK_SIZE_IN_BYTE;
    if (count > 0) {
        /* add the padding phy of A into node list */
        ret = drv_symc_add_inbuf(chn_num, dma_pad, AES_BLOCK_SIZE_IN_BYTE - count,
                                 SYMC_NODE_USAGE_IN_CCM_A);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_add_inbuf, ret);
            return ret;
        }
        ctx->offset_pad += AES_BLOCK_SIZE_IN_BYTE - count;
    }

    /* add ccm a last flag */
    drv_symc_add_buf_usage(chn_num, HI_TRUE, SYMC_NODE_USAGE_LAST);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_aead_gcm_add_a(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size)
{
    symc_hard_context *ctx = HI_NULL;
    compat_addr dma_pad;
    hi_u8 *via_pad = HI_NULL;
    hi_s32 ret;
    hi_u32 count = 0;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);
    ctx = &hard_context[chn_num];
    crypto_assert((ctx->offset_pad + AES_BLOCK_SIZE_IN_BYTE < SYMC_PAD_BUFFER_LEN));

    /* return success when alen is zero */
    if (buf_size == 0x00) {
        hi_log_func_exit();
        return HI_SUCCESS;
    }

    ADDR_U64(dma_pad) = ADDR_U64(ctx->dma_pad) + ctx->offset_pad;
    via_pad = ctx->via_pad + ctx->offset_pad;
    ret = memset_s(via_pad, AES_BLOCK_SIZE_IN_BYTE, 0, AES_BLOCK_SIZE_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    /* Add phy of A into node list */
    ret = drv_symc_add_inbuf(chn_num, buf_phy, buf_size,
        SYMC_NODE_USAGE_IN_GCM_A | SYMC_NODE_USAGE_FIRST | SYMC_NODE_USAGE_GCM_FIRST);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_inbuf, ret);
        return ret;
    }

    /* if Alen do not aligned with 16, padding 0 to the tail */
    count = (buf_size + count) % AES_BLOCK_SIZE_IN_BYTE;
    if (count > 0) {
        /* add the padding phy of A into node list */
        ret = drv_symc_add_inbuf(chn_num, dma_pad, AES_BLOCK_SIZE_IN_BYTE - count, SYMC_NODE_USAGE_IN_GCM_A);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_symc_add_inbuf, ret);
            return ret;
        }
        ctx->offset_pad += AES_BLOCK_SIZE_IN_BYTE - count;
    }

    /* add gcm a last flag */
    drv_symc_add_buf_usage(chn_num, HI_TRUE, SYMC_NODE_USAGE_LAST);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_aead_gcm_add_clen(hi_u32 chn_num, hi_u8 *clen, hi_u32 clen_buf_len)
{
    hi_s32 ret;
    symc_hard_context *ctx = HI_NULL;
    symc_node_usage usage;
    compat_addr dma_pad;
    hi_u8 *via_pad = HI_NULL;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);
    ctx = &hard_context[chn_num];
    crypto_assert((ctx->offset_pad + SYMC_CCM_N_LEN < SYMC_PAD_BUFFER_LEN));
    hi_log_check_param(clen_buf_len < SYMC_GCM_CLEN_LEN);

    /* add Clen */
    ADDR_U64(dma_pad) = ADDR_U64(ctx->dma_pad) + ctx->offset_pad;
    via_pad = ctx->via_pad + ctx->offset_pad;

    usage = SYMC_NODE_USAGE_IN_GCM_LEN | SYMC_NODE_USAGE_LAST | SYMC_NODE_USAGE_FIRST;

    ret = memcpy_s(via_pad, SYMC_GCM_CLEN_LEN, clen, SYMC_GCM_CLEN_LEN);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }
    ctx->offset_pad += SYMC_GCM_CLEN_LEN;

    ret = drv_symc_add_inbuf(chn_num, dma_pad, SYMC_GCM_CLEN_LEN, usage);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_add_inbuf, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_aead_get_tag(hi_u32 chn_num, hi_u32 *tag)
{
    hi_u32 i;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    for (i = 0; i < AEAD_TAG_SIZE_IN_WORD; i++) {
        tag[i] = symc_read(CHANn_CIPHER_CCM_GCM_TAG(chn_num, i));
        hi_log_debug("CHANn_CIPHER_CCM_GCM_TAG[0x%x]: 0x%x\n", CHANn_CIPHER_CCM_GCM_TAG(chn_num, i), tag[i]);
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

hi_s32 drv_symc_node_check(symc_alg alg, symc_mode mode,
                           hi_u32 klen, hi_u32 block_size,
                           compat_addr input[],
                           compat_addr output[],
                           hi_u32 length[],
                           symc_node_usage usage_list[],
                           hi_u32 pkg_num)
{
    hi_u32 i = 0;
    hi_u32 total = 0;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(block_size != 0);

    crypto_unused(input);
    crypto_unused(output);
    crypto_unused(klen);
    crypto_unused(usage_list);

    for (i = 0; i < pkg_num; i++) {
#ifdef HI_PRODUCT_MULTI_CIPHER_SUPPORT
        /* Used the odd key must accord with conditions as follows: */
        if (usage_list[i] & SYMC_NODE_USAGE_ODD_KEY) {
            /* 1. Only support aes ecb/cbc/cfb/ofb/ctr */
            if ((alg != SYMC_ALG_AES)
                || ((mode != SYMC_MODE_ECB)
                          && (mode != SYMC_MODE_CBC)
                          && (mode != SYMC_MODE_CFB)
                          && (mode != SYMC_MODE_OFB)
                          && (mode != SYMC_MODE_CTR))) {
                hi_log_error("Odd key only support aes ecb/cbc/cfb/ofb/ctr.");
                hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
                return HI_ERR_CIPHER_INVALID_PARA;
            }

            /* 2. Only support aes128 */
            if (klen != AES_CCM_GCM_KEY_LEN) {
                hi_log_error("Odd key only support aes128, klen %d\n", klen);
                hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
                return HI_ERR_CIPHER_INVALID_PARA;
            }

            /* 3. each node length must be a multiple of 64 */
            if ((length[i] % (AES_BLOCK_SIZE_IN_BYTE)) != 0) {
                hi_log_error("Odd key only supported when each node length is a multiple of 64.");
                hi_log_print_err_code(HI_ERR_CIPHER_INVALID_LENGTH);
                return HI_ERR_CIPHER_INVALID_LENGTH;
            }
        }
#endif
        /* each node length can't be zero */
        if (length[i] == 0) {
            hi_log_error("PKG len must large than 0.\n");
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_LENGTH);
            return HI_ERR_CIPHER_INVALID_LENGTH;
        }

        /* check overflow */
        if (length[i] > ADDR_L32(input[i]) + length[i]) {
            hi_log_error("PKG len overflow.\n");
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_LENGTH);
            return HI_ERR_CIPHER_INVALID_LENGTH;
        }
        total += length[i];
    }

    if ((alg != SYMC_ALG_NULL_CIPHER) &&
        ((mode == SYMC_MODE_ECB)
         || (mode == SYMC_MODE_CBC)
         || (mode == SYMC_MODE_CFB)
         || (mode == SYMC_MODE_OFB))) {
        /* The length of data depend on alg and mode, which limit to hardware
        * for ecb/cbc/ofb/cfb, the total data length must aligned with block size.
        * for ctr/ccm/gcm, support any data length.
 */
        if (total % block_size != 0) {
            hi_log_error("PKG len must align with 0x%x.\n", block_size);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_LENGTH);
            return HI_ERR_CIPHER_INVALID_LENGTH;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_symc_config(hi_u32 chn_num, symc_alg alg, symc_mode mode, symc_width width, hi_u32 decrypt,
                       hi_u32 sm1_round_num, symc_klen klen, hi_u32 hard_key)
{
    symc_hard_context *ctx = HI_NULL;
    in_sym_chn_key_ctrl cipher_ctrl;
    hi_u32 symc_alg_cfg_val[]  = {0x0, TDES, AES, SM4, 0x0, 0x0};  /* DES TDES AES SM4 SM1 DMA */
    hi_u32 symc_mode_cfg_val[] = {ECB, CBC,  CFB, OFB, CTR, CCM, GCM, 0x0};  /* ECB CBC CFB OFB CTR CCM GCM CTS */
    hi_u32 symc_klen_cfg_val[][KEY_CNT] = {
        {KEY_128BIT, KEY_192BIT, KEY_256BIT, 0},  /* 0-AES: 128, 192, 256 */
        {KEY_192BIT, 0, 0, KEY_128BIT},   /* 1-TDES: 3key 2key */
        {KEY_128BIT, 0, 0, 0}             /* 2-SM4: 128 */
    };

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    ctx = &hard_context[chn_num];

    /* record alg */
    ctx->alg = alg;
    crypto_unused(sm1_round_num);

    hi_log_debug("symc configure, chn %d, alg %d, mode %d, dec %d, hard %d\n",
                 chn_num, alg, mode, decrypt, hard_key);

    if (alg == SYMC_ALG_NULL_CIPHER) {
        drv_symc_set_dma(chn_num, 0x1);
    } else {
        cipher_ctrl.u32 = symc_read(IN_SYM_CHN_KEY_CTRL(chn_num));
        if (alg == SYMC_ALG_AES) {
            cipher_ctrl.bits.sym_alg_key_len = symc_klen_cfg_val[0][klen]; /* 0 - aes */
        } else if (alg == SYMC_ALG_TDES) {
            cipher_ctrl.bits.sym_alg_key_len = symc_klen_cfg_val[1][klen]; /* 1 - tdes */
        } else if (alg == SYMC_ALG_SM4) {
            cipher_ctrl.bits.sym_alg_key_len = symc_klen_cfg_val[2][klen]; /* 2 - sm4 */
        } else {
            cipher_ctrl.bits.sym_alg_key_len = 0x1;
        }
        cipher_ctrl.bits.sym_alg_data_width = width;
        cipher_ctrl.bits.sym_alg_decrypt = decrypt;
        cipher_ctrl.bits.sym_alg_sel = symc_alg_cfg_val[alg];
        cipher_ctrl.bits.sym_alg_mode = symc_mode_cfg_val[mode];
        ctx->mode = mode;
        symc_write(IN_SYM_CHN_KEY_CTRL(chn_num), cipher_ctrl.u32);
        hi_log_debug("IN_SYM_CHN_KEY_CTRL[0x%x]: 0x%x\n", IN_SYM_CHN_KEY_CTRL(chn_num), cipher_ctrl.u32);
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_symc_set_isr_callback(hi_u32 chn_num, callback_symc_isr callback, void *ctx)
{
    symc_hard_context *hisi_ctx = HI_NULL;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    hisi_ctx = &hard_context[chn_num];

    hisi_ctx->callback = callback;
    hisi_ctx->ctx = ctx;

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_symc_start(hi_u32 chn_num)
{
    symc_hard_context *ctx = HI_NULL;
    in_sym_chn_node_wr_point in_node_wr_ptr;
    out_sym_chn_node_wr_point out_node_wr_ptr;
    in_sym_chn_node_rd_point in_node_rd_ptr;
    out_sym_chn_node_rd_point out_node_rd_ptr;
    out_sym_chn_node_ctrl cipher_node_ctrl;
    spacc_mmu_in_sym_clear mmu_in_clear;
    spacc_mmu_out_sym_clear mmu_out_clear;
    in_sym_chn_ctrl in_ctrl;
    hi_u32 ptr;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    ctx = &hard_context[chn_num];

    hi_log_debug("symc start, chn %d\n", chn_num);

    ctx->done = HI_FALSE;

    /* enable channel */
    in_ctrl.u32 = symc_read(IN_SYM_CHN_CTRL(chn_num));
    in_ctrl.bits.sym_chn_en = 1;
    in_ctrl.bits.sym_chn_ss = SYMC_CFG_NON_SECURE;
    in_ctrl.bits.sym_chn_ds = SYMC_CFG_NON_SECURE;
    symc_write(IN_SYM_CHN_CTRL(chn_num), in_ctrl.u32);
    hi_log_debug("IN_SYM_CHN_CTRL[0x%x]  :0x%x\n", IN_SYM_CHN_CTRL(chn_num), in_ctrl.u32);

    /* channel clear in mmu page (SW set this bit 1 and HW clean it to be 0) */
    mmu_in_clear.u32 = symc_read(SPACC_MMU_IN_SYM_CLEAR);
    mmu_in_clear.bits.sw_mmu_in_sym_clr = 1 << chn_num;
    symc_write(SPACC_MMU_IN_SYM_CLEAR, mmu_in_clear.u32);
    hi_log_debug("SPACC_MMU_IN_SYM_CLEAR: 0x%x\n", mmu_in_clear.u32);

    /* channel clear out mmu page (SW set this bit 1 and HW clean it to be 0) */
    mmu_out_clear.u32 = symc_read(SPACC_MMU_OUT_SYM_CLEAR);
    mmu_out_clear.bits.sw_mmu_out_sym_clr = 1 << chn_num;
    symc_write(SPACC_MMU_OUT_SYM_CLEAR, mmu_out_clear.u32);
    hi_log_debug("SPACC_MMU_OUT_SYM_CLEAR: 0x%x\n", mmu_out_clear.u32);

    /* configure out nodes */
    out_node_wr_ptr.u32 = symc_read(OUT_SYM_CHN_NODE_WR_POINT(chn_num));
    out_node_rd_ptr.u32 = symc_read(OUT_SYM_CHN_NODE_RD_POINT(chn_num));
    if (out_node_wr_ptr.bits.out_sym_chn_node_wr_point != out_node_rd_ptr.bits.out_sym_chn_node_rd_point) {
        hi_log_error("Error, out_chn %d is busy, Wr_ptr 0x%x, Rd_ptr 0x%x.\n",
                     chn_num, out_node_wr_ptr.u32, out_node_rd_ptr.u32);
    }
    ptr = out_node_wr_ptr.bits.out_sym_chn_node_wr_point + ctx->cnt_out;
    out_node_wr_ptr.bits.out_sym_chn_node_wr_point = ptr % SYMC_MAX_LIST_NUM;

    cipher_node_ctrl.u32 = symc_read(OUT_SYM_CHN_NODE_CTRL(chn_num));
    cipher_node_ctrl.bits.out_sym_chn_node_level = ctx->cnt_out;
    symc_write(OUT_SYM_CHN_NODE_CTRL(chn_num), cipher_node_ctrl.u32);
    hi_log_info("CHAN[%d] IN_SYM_CHN_NODE_CTRL[0x%x]: \t0x%x\n",
                chn_num, OUT_SYM_CHN_NODE_CTRL(chn_num), cipher_node_ctrl.u32);

    /* *** out_node_cfg.bits.cipher_out_node_mpackage_int_level = ctx->cnt_out *** */
    symc_write(OUT_SYM_CHN_NODE_WR_POINT(chn_num), out_node_wr_ptr.u32);
    hi_log_debug("OUT_SYM_CHN_NODE_WR_POINT: 0x%x\n", out_node_wr_ptr.u32);

    /* configure in nodes */
    in_node_wr_ptr.u32 = symc_read(IN_SYM_CHN_NODE_WR_POINT(chn_num));
    in_node_rd_ptr.u32 = symc_read(IN_SYM_CHN_NODE_RD_POINT(chn_num));
    if (in_node_wr_ptr.bits.sym_chn_node_wr_point != in_node_rd_ptr.bits.sym_chn_node_rd_point) {
        hi_log_error("Error, in_chn %d is busy, Wr_ptr 0x%x, Rd_ptr 0x%x.\n",
                     chn_num, in_node_wr_ptr.u32, in_node_rd_ptr.u32);
    }

    ptr = in_node_wr_ptr.bits.sym_chn_node_wr_point + ctx->cnt_in;
    in_node_wr_ptr.bits.sym_chn_node_wr_point = ptr % SYMC_MAX_LIST_NUM;

    /* make sure all the above explicit memory accesses and instructions are completed
     * before start the hardware.
     */
    arm_memory_barrier();

    symc_write(IN_SYM_CHN_NODE_WR_POINT(chn_num), in_node_wr_ptr.u32);
    hi_log_debug("IN_SYM_CHN_NODE_WR_POINT[0x%x]: 0x%x\n",
        IN_SYM_CHN_NODE_WR_POINT(chn_num), in_node_wr_ptr.u32);

    /* all the nodes are processed, retset the cnount to zero */
    ctx->cnt_in = 0;
    ctx->cnt_out = 0;
    ctx->offset_pad = 0;

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_symc_wait_done(hi_u32 chn_num, hi_u32 timeout)
{
    hi_s32 ret;
    symc_hard_context *ctx = HI_NULL;
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;

    hi_log_func_enter();

    crypto_assert(symc_initialize == HI_TRUE);
    crypto_assert(chn_num < CRYPTO_HARD_CHANNEL_MAX);

    ctx = &hard_context[chn_num];
    module_get_attr(CRYPTO_MODULE_ID_SYMC, &int_valid, &int_num, HI_NULL);

#ifdef CRYPTO_OS_INT_SUPPORT
    if (int_valid == 0x01) {

        hi_log_info("chn %d wait interrupt\n", chn_num);
        ret = crypto_queue_wait_timeout(&ctx->queue, crypto_queue_callback_func, &ctx->done, timeout);
        if (ret == 0) {
            hi_log_error("wait done timeout, chn=%d\n", chn_num);
            hi_log_print_func_err(crypto_queue_wait_timeout, ret);
            drv_symc_get_err_code(chn_num);
            return HI_ERR_CIPHER_TIMEOUT;
        }
    } else {
#else /* !CRYPTO_OS_INT_SUPPORT */
    {
#endif
        hi_u32 i;

        for (i = 0; i < timeout; i++) {
            ret = drv_symc_done_try(chn_num);
            if (ret == HI_SUCCESS) {
                break;
            }
            if (i <= MS_TO_US) {
                crypto_udelay(1); /* short waitting for 1000 us */
            } else {
                crypto_msleep(1); /* long waitting for 5000 ms */
            }
        }

        if (timeout <= i) {
            hi_log_error("symc wait done timeout, chn=%d\n", chn_num);
            hi_log_print_err_code(HI_ERR_CIPHER_TIMEOUT);
            drv_symc_get_err_code(chn_num);
            return HI_ERR_CIPHER_TIMEOUT;
        }
    }

    ret = drv_symc_check_mmu();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_check_mmu, ret);
        drv_symc_get_err_code(chn_num);
        return ret;
    }

    ret = drv_symc_get_err_code(chn_num);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_symc_get_err_code, ret);
        return ret;
    }

    hi_log_info("symc done, chn=%d\n", chn_num);

    hi_log_func_exit();
    return HI_SUCCESS;
}

#if defined(HI_PRODUCT_CENC_SUPPORT)
void drv_symc_enable_cenc_node(hi_u32 chn_num, hi_u32 is_head, hi_u32 c_num, hi_u32 e_num, hi_u32 offset_len)
{
    symc_hard_context *ctx = HI_NULL;
    hi_u32 id;

    ctx = &hard_context[chn_num];

    /* get last node */
    id = (ctx->id_in == 0) ? SYMC_MAX_LIST_NUM - 1 : ctx->id_in - 1;

    ctx->entry_in[id].cenc_enable = HI_TRUE;
    ctx->entry_in[id].cenc_head = is_head;
    ctx->entry_in[id].e_num = e_num / AES_BLOCK_SIZE_IN_BYTE;
    ctx->entry_in[id].c_num = c_num / AES_BLOCK_SIZE_IN_BYTE;
    ctx->entry_in[id].Cenc_offset = offset_len;

    hi_log_info("chn %d, add symc in ctrl: id %d, head : %x, clear : 0x%x, enc : 0x%x, offset : 0x%x.\n",
                chn_num, id, is_head, c_num, e_num, offset_len);

    return;
}

void drv_symc_set_destory_callbcak(hi_u32 chn_num, callback_symc_destory destory)
{
    symc_hard_context *ctx = HI_NULL;

    hi_log_func_enter();

    ctx = &hard_context[chn_num];
    ctx->destory = destory;

    hi_log_func_exit();
    return;
}
#endif

/******* proc function begin ********/
#ifdef HI_PROC_SUPPORT
void drv_symc_proc_status(symc_chn_status *status, hi_u32 status_buf_len, symc_module_status *module)
{
    in_sym_chn_key_ctrl ctrl;
    spacc_ie stIntEn;

    out_sym_chan_raw_int stOutIntRaw;
    out_sym_chan_raw_int_en stOutIntEn;
    out_sym_chan_int stOutIntStatus;

    in_sym_chn_node_wr_point in_node_wr_ptr;
    in_sym_chn_node_rd_point in_node_rd_ptr;
    in_sym_chn_node_length in_node_depth;

    out_sym_chn_node_wr_point out_node_wr_ptr;
    out_sym_chn_node_rd_point out_node_rd_ptr;
    out_sym_chn_node_length out_node_depth;

    in_sym_chn_ctrl in_chn_ctrl;
    out_sym_chn_ctrl out_chn_ctrl;
    hi_u32 sec_chn_cfg;
    hi_u32 info = 0;
    hi_u32 int_valid = 0;
    hi_u32 int_num = 0;
    hi_u32 val = 0;
    const char *name = HI_NULL;
    hi_u32 i, j;

    if ((status_buf_len != sizeof(symc_chn_status) * CRYPTO_HARD_CHANNEL_MAX) || (status == HI_NULL)) {
        return;
    }

    module_get_attr(CRYPTO_MODULE_ID_SYMC, &int_valid, &int_num, &name);
    module_get_info(CRYPTO_MODULE_ID_SYMC, &info, MODULE_INFO_RESET_VALID);
    sec_chn_cfg = symc_read(SPACC_SYM_CHN_LOCK);
    stIntEn.u32 = symc_read(SPACC_IE);

    module->allow_reset = info;
    module->sec_cpu = crypto_is_sec_cpu();
    module->int_num = int_num;
    module->name = name;
    module->smmu_base = symc_read(module->sec_cpu ? SPACC_MMU_SEC_TLB : SPACC_MMU_NOSEC_TLB);
    module->err_code = symc_read(SYM_CALC_CTRL_CHECK_ERR(SPACC_CPU_CUR));
    module->int_en = stIntEn.bits.spacc_ie_tee;

    for (i = 1; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if (((0x1 << i) & CIPHER_HARD_CHANNEL_MASK) == 0x00) {
            continue;
        }

        status[i].open = hard_context[i].open;

        /* ctrl */
        ctrl.u32 = symc_read(IN_SYM_CHN_KEY_CTRL(i));
        status[i].decrypt = ctrl.bits.sym_alg_decrypt;
        status[i].alg = ctrl.bits.sym_alg_sel;
        status[i].mode = ctrl.bits.sym_alg_mode;
        status[i].klen = ctrl.bits.sym_alg_key_len;
        status[i].ksrc = ctrl.bits.sym_key_chn_id;

        /* out iv */
        for (j = 0; j < 32; j += 8) { /* iv string have 32 bytes, a word have 8bytes */
            val = symc_read(CHANn_CIPHER_IVOUT(i) + j * WORD_WIDTH);
            hex2str (status[i].iv + j, (hi_u8)(val & 0xFF));
            hex2str (status[i].iv + j + 2, (hi_u8)((val >> 8) & 0xFF));  /* byte2~3: bit 15..8  */
            hex2str (status[i].iv + j + 4, (hi_u8)((val >> 16) & 0xFF)); /* byte4~5: bit 23..16 */
            hex2str (status[i].iv + j + 6, (hi_u8)((val >> 24) & 0xFF)); /* byte6~7: bit 31..24 */
        }

        /* secure channel */
        status[i].secure = CHN_WHO_USED_GET(sec_chn_cfg, i);

        /* interrupt */
        stOutIntEn.u32 = symc_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN);
        stOutIntRaw.u32 = symc_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT);
        stOutIntStatus.u32 = symc_read(OUT_SYM_CHAN_LAST_NODE_INT);
        status[i].int_en = (stOutIntEn.bits.out_sym_chan_int_en >> (i - 1)) & 0x1;
        status[i].int_raw = (stOutIntRaw.bits.out_sym_chan_raw_int >> (i - 1)) & 0x1;
        status[i].int_status = (stOutIntStatus.bits.out_sym_chan_int >> (i - 1)) & 0x1;

        /* in node */
        in_node_wr_ptr.u32 = symc_read(IN_SYM_CHN_NODE_WR_POINT(i));
        in_node_rd_ptr.u32 = symc_read(IN_SYM_CHN_NODE_RD_POINT(i));
        in_node_depth.u32 = symc_read(IN_SYM_CHN_NODE_LENGTH(i));
        status[i].in_node_head = symc_read(IN_SYM_CHN_NODE_START_ADDR_L(i));
        status[i].in_node_depth = in_node_depth.bits.sym_chn_node_length;
        status[i].in_node_rptr = in_node_rd_ptr.bits.sym_chn_node_rd_point;
        status[i].in_node_wptr = in_node_wr_ptr.bits.sym_chn_node_wr_point;
        status[i].inaddr = symc_read(DBG_IN_SYM_CHN_RD_ADDR_L(i));

        /* out node */
        out_node_wr_ptr.u32 = symc_read(OUT_SYM_CHN_NODE_WR_POINT(i));
        out_node_rd_ptr.u32 = symc_read(OUT_SYM_CHN_NODE_RD_POINT(i));
        out_node_depth.u32 = symc_read(OUT_SYM_CHN_NODE_LENGTH(i));
        status[i].out_node_head = symc_read(OUT_SYM_CHN_NODE_START_ADDR_L(i));
        status[i].out_node_depth = out_node_depth.bits.out_sym_chn_node_length;
        status[i].out_node_rptr = out_node_rd_ptr.bits.out_sym_chn_node_rd_point;
        status[i].out_node_wptr = out_node_wr_ptr.bits.out_sym_chn_node_wr_point;
        status[i].outaddr = symc_read(DBG_OUT_SYM_CHN_RD_ADDR_L(i));

        /* SMMU Enable */
        in_chn_ctrl.u32 = symc_read(IN_SYM_CHN_CTRL(i));
        out_chn_ctrl.u32 = symc_read(OUT_SYM_CHN_CTRL(i));
        status[i].smmu_enable = in_chn_ctrl.bits.sym_chn_mmu_en;
        status[i].smmu_enable &= out_chn_ctrl.bits.out_sym_chn_mmu_en;
    }

    return;
}
#endif
/******* proc function end ********/

void drv_symc_get_capacity(symc_capacity *capacity)
{
    hi_s32 ret;

    hi_log_func_enter();

    /* the mode depend on alg, which limit to hardware
     * 3des support ecb/cbc/cfb/ofb
     * aes support ecb/cbc/cfb/ofb/ctr/ccm/gcm
     * sm4 support ecb/cbc/ctr
     */

    ret = memset_s(capacity, sizeof(symc_capacity), 0, sizeof(symc_capacity));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return;
    }

    /* AES */
    capacity->aes_ecb = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_cbc = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_ofb = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_cfb = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_ctr = CRYPTO_CAPACITY_SUPPORT;

#ifdef CHIP_AES_CCM_GCM_SUPPORT
    capacity->aes_ccm = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_gcm = CRYPTO_CAPACITY_SUPPORT;
#endif

    /* TDES */
    capacity->tdes_ecb = CRYPTO_CAPACITY_SUPPORT;
    capacity->tdes_cbc = CRYPTO_CAPACITY_SUPPORT;
    capacity->tdes_ofb = CRYPTO_CAPACITY_SUPPORT;
    capacity->tdes_cfb = CRYPTO_CAPACITY_SUPPORT;

    /* SM4 */
#ifdef CHIP_SYMC_SM4_SUPPORT
    capacity->sm4_ecb = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm4_cbc = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm4_ctr = CRYPTO_CAPACITY_SUPPORT;
#endif

    /* DMA */
    capacity->dma = CRYPTO_CAPACITY_SUPPORT;

    return;
}

/** @} */ /** <!-- ==== API Code end ==== */

#endif  // End of CHIP_SYMC_VER_V300
