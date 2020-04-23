 /*
  * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
  * Description: supply the api for userspace application
  */
#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include "hi_drv_ir.h"
#include "drv_ir_ioctl.h"
#include "drv_ir_protocol.h"
#include "drv_ir_utils.h"
#include "drv_ir_register.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_stat.h"
#include "hi_drv_sys.h"
#include "hi_drv_osal.h"
#include "hi_drv_dev.h"
#include "hi_errno.h"
#define IR_DEVICE_NAME HI_DEV_IR_NAME
#define READER_MIN     3
#define WRITER_MAX     497
#define READER_WRITER_MINGAP 5
#define SYMBOL_WIDTH_VAL 0x2701
/* public parameters */
/* chip work mode, key or symbol. */
static hi_u32 g_ir_work_mode = IR_MODE_SYMBOL; /* default mode symbol. */

static hi_u32 g_ir_frequence = DFT_FREQ;
module_param_named(freq, g_ir_frequence, uint, S_IRUGO);

static hi_u32 g_ir_keyhold_timeout = 300;  /* ms */
module_param_named(key_hold_timeout, g_ir_keyhold_timeout, uint, S_IRUGO);

/* ************************* symbol mode parameters *************** */
/* reserve the parameter names. */
static hi_u32 g_ir_symbol_width = SYMBOL_WIDTH_VAL;
module_param_named(symbol_width, g_ir_symbol_width, uint, S_IRUGO);

static hi_u32 g_ir_symbol_format = DFT_SYMBOL_FMT;
module_param_named(symbol_format, g_ir_symbol_format, uint, S_IRUGO);

static hi_u32 g_ir_int_level = 0;
module_param_named(int_level, g_ir_int_level, uint, S_IRUGO);

/* for raw symbol read, specify this parameter. */
static hi_u32 g_ir_key_fetch_raw = 0;
module_param_named(key_fetch, g_ir_key_fetch_raw, uint, S_IRUGO);

/* ************************************************************************ */
ir_priv      g_ir_local = {0};
key_attr     *g_last_key_state = HI_NULL;
static bool  g_is_matched = 0;
hi_u32       g_time_out_flag = HI_FALSE;

static inline hi_void ir_enable(ir_priv *ir)
{
    writel(0x101, ir->base + IR_EN);
}

static inline hi_void ir_disable(ir_priv *ir)
{
    writel(0, ir->base + IR_EN);
}

static inline hi_void ir_start(ir_priv *ir)
{
    writel(1, ir->base + IR_START);
}

static hi_void ir_set_clk(bool flag)
{
    hi_u32 *pir_rest = HI_NULL_PTR;
    hi_u32 reg_val = 0;

    pir_rest = ioremap_nocache(0x00840060, 0x4);
    if (pir_rest == HI_NULL_PTR) {
        hiir_error("ioremap_nocache IR reset err! \n");
        return;
    }

    reg_val = readl(pir_rest);

    if (flag == HI_TRUE) {
        /* open clk */
        reg_val |= 0x10;
        reg_val &= 0xffffffdf;
    } else {
        /* close clk */
        reg_val |= 0x20;
        reg_val &= 0xffffffef;
    }

    writel(reg_val, pir_rest);
    iounmap(pir_rest);
    return;
}

static hi_void ir_clear_fifo(ir_priv *ir)
{
    hi_u32 cnt;
    hi_u32 v;
    hi_u32 i;

    cnt = readl(ir->base + IR_DATAH);
    for (i = 0; i < cnt; i++) {
        v = readl(ir->base + IR_DATAL);
    }
}

static hi_s32 ir_config(ir_priv *ir)
{
    hi_u32 val = 0;

    ir_enable(ir);
    while (((hi_u32)(readl(ir->base + IR_BUSY)) & 0x1) && val++ < 1000) {
        (hi_void)schedule_timeout(10);
    }

    if (val >= 1000) {
        hiir_info("ir module is busy!\n");
        return -EBUSY;
    }

    if (ir->chip_param.work_mode == IR_MODE_KEY) {
        return -EINVAL;
    } else {
        val = ((ir->chip_param.ir_max_symbol << 16) |
               (ir->chip_param.ir_format << 14) |
               (ir->chip_param.ir_max_bits << 8) |
               ((hi_u32)ir->chip_param.work_mode << 7) |
               (ir->chip_param.ir_freq));
    }

    hiir_debug("IR work at %s mode, config value: 0x%08x\n",
               ir->chip_param.work_mode == IR_MODE_KEY ?
               "key" : "symbol",
               val);

    writel(val, ir->base + IR_CFG);
    g_is_matched = 0;
    return 0;
}

static hi_s32 ir_symbol_proc(ir_priv *ir)
{
    ir_protocol      *ip = NULL;
    key_attr         *key = NULL;
    ir_match_result  r;
    hi_s32           matchs;
    hi_s32           ret;
    hi_ulong         flag;

    /*
     * 1. find out which is a infrared code match the head symbol.
     * 2. if no, parse from this head symbol.
     * 3. if yes, we should check the whole frame to get a
     * accurate match infrared code for this key.
     */
    key = ir_first_read_key(ir->symbol_buf);
    while (key && (!key->upper||!key->lower)) {
        if (!key->upper && !key->lower) {
            if (((ir->symbol_buf->reader > ir->symbol_buf->writer) &&
                (ir->symbol_buf->reader - ir->symbol_buf->writer < READER_WRITER_MINGAP)) ||
                ((ir->symbol_buf->reader < READER_MIN) &&
                (ir->symbol_buf->writer > WRITER_MAX))) {
                hiir_error("first break w = %d r = %d l=%lld,up=%lld\n",
                           ir->symbol_buf->writer, ir->symbol_buf->reader,key->lower, key->upper);
                break;
            }
        }
        hiir_error("first w = %d r = %d l=%lld,up=%lld\n",
                   ir->symbol_buf->writer, ir->symbol_buf->reader,key->lower, key->upper);
        ir_next_reader_clr_inc(ir->symbol_buf);
        key = ir_first_read_key(ir->symbol_buf);
    }

    while (key && key->upper && key->lower) {
        hiir_debug("header key at(%d): key->lower:%lld,"
                   " key->upper:%lld",
                   ir->symbol_buf->reader,
                   key->lower, key->upper);

        spin_lock_irqsave(&irlock, flag);
        ip = ir_prot_first();
        matchs = 0;
        while (ir_prot_valid(ip)) {
            if (!ip->match || ip->disabled) {
                ip = ir_prot_next(ip);
                continue;
            }

            /* try find header first! */
            r = ip->match(IR_MTT_HEADER, ir->symbol_buf, ip);
            if (r != IR_MATCH_MATCH) {
                if (r == IR_MATCH_NEED_MORE_DATA) {
                    matchs++;
                    ip = ir_prot_next(ip);
                    continue;
                }

                if ((hi_u32)ip->flag & INFR_HAS_ERR_HANDLE) {
                    r = ip->match_error(IR_MTT_HEADER,
                                        ir->symbol_buf, ip);
                    if (r != IR_MATCH_MATCH) {
                        if (r == IR_MATCH_NEED_MORE_DATA) {
                            matchs++;
                        }

                        ip = ir_prot_next(ip);
                        continue;
                    }
                } else {
                    ip = ir_prot_next(ip);
                    continue;
                }
            }

            /* find a match. */
            r = ip->match(IR_MTT_FRAME, ir->symbol_buf, ip);
            if ((r != IR_MATCH_MATCH) &&
               ((hi_u32)ip->flag & INFR_HAS_ERR_HANDLE)) {
                r = ip->match_error(IR_MTT_FRAME,
                                    ir->symbol_buf, ip);
            }

            if (r == IR_MATCH_MATCH) {
                g_is_matched = 1;
                mod_timer(&ir->symbol_timer, jiffies
                          + msecs_to_jiffies(1));
                /* if match, parse this frame */
                ret = ip->handle(ir, ip, ir->symbol_buf,
                                 ir->key_buf);
                if (ret == 0) {
                    wake_up_interruptible(&ir->read_wait);
                }

                if ((ret != 0) && ((hi_u32)ip->flag & INFR_HAS_ERR_HANDLE)) {
                    ip->parse_error(ir, ip,
                                    ir->symbol_buf, ir->key_buf);
                }
                spin_unlock_irqrestore(&irlock, flag);
                goto NEXT_FRAME;
            } else if (r == IR_MATCH_NEED_MORE_DATA) {
                matchs++;
            }

            ip = ir_prot_next(ip);
        }
        spin_unlock_irqrestore(&irlock, flag);

        /* if no match, discard this symbol. */
        if (matchs == 0) {
            key = ir_next_reader_clr_inc(ir->symbol_buf);
            continue;
        }

        /*
         * if some infrared code match, do not discard,
         * wait timer expires and try again.
         */
        if (ir_prot_valid(ip) == 0) {
            return -1;
        }

NEXT_FRAME:
        key = ir_first_read_key(ir->symbol_buf);
    }

    return 0;
}

static hi_void ir_symbol_timer_proc(hi_ulong data)
{
    ir_priv            *ir = &g_ir_local;
    key_attr           *key = NULL;
    ir_protocol        *ip = NULL;
    ir_match_result    r;
    static hi_ulong    deadline;
    hi_ulong           flag;

    if (g_is_matched) {
        g_is_matched = 0;
        return;
    }

    if (ir_symbol_proc(ir) == 0) {
        goto OUT;
    }

    hiir_debug("symbols cannot be parsed." \
               "abandon them till frame header appear" \
               " or buffer empty!\n");

    /*
     * case symbol cannot be parsed, ignore symbols until
     * new frame start symbol appears or buffer empty.
     */
    key = ir_first_read_key(ir->symbol_buf);
    while (key && (!key->upper||!key->lower)) {
        if (!key->upper && !key->lower) {
            if (((ir->symbol_buf->reader > ir->symbol_buf->writer) &&
                (ir->symbol_buf->reader - ir->symbol_buf->writer < READER_WRITER_MINGAP)) ||
                ((ir->symbol_buf->reader < READER_MIN) && (ir->symbol_buf->writer > WRITER_MAX))) {
                hiir_error("first break w = %d r = %d l=%lld,up=%lld\n",
                           ir->symbol_buf->writer, ir->symbol_buf->reader,key->lower, key->upper);
                break;
            }
        }
        hiir_error("first w = %d r = %d l=%lld,up=%lld\n",
                   ir->symbol_buf->writer, ir->symbol_buf->reader,key->lower, key->upper);
        ir_next_reader_clr_inc(ir->symbol_buf);
        key = ir_first_read_key(ir->symbol_buf);
    }

    while (key && key->upper && key->lower) {
        spin_lock_irqsave(&irlock, flag);
        ip = ir_prot_first();
        while (ir_prot_valid(ip)) {
            if (!ip->match || ip->disabled) {
                ip = ir_prot_next(ip);
                continue;
            }

            r = ip->match(IR_MTT_HEADER, ir->symbol_buf, ip);
            if (r == IR_MATCH_MATCH) {
                r = ip->match(IR_MTT_FRAME,
                              ir->symbol_buf, ip);
                if ((r != IR_MATCH_MATCH) &&
                   ((hi_u32)ip->flag & INFR_HAS_ERR_HANDLE)) {
                    r = ip->match_error(IR_MTT_FRAME,
                                        ir->symbol_buf, ip);
                }

                if (r == IR_MATCH_MATCH) {
                    spin_unlock_irqrestore(&irlock, flag);
                    (hi_void)ir_symbol_proc(ir);
                    deadline = 0;
                    return;
                }

                if (deadline && time_after_eq(jiffies, deadline)) {
                    ip = ir_prot_next(ip);
                    continue;
                } else {
                    deadline = jiffies + msecs_to_jiffies(
                                   SYMBOL_PARSE_TIMEOUT);
                }
                /* A new frame start. abort */
                ir->symbol_timer.expires = jiffies
                                           + msecs_to_jiffies(
                                               SYMBOL_PARSE_TIMEOUT);
                mod_timer(&ir->symbol_timer, ir->symbol_timer.expires);
                spin_unlock_irqrestore(&irlock, flag);
                goto OUT;
            } else if (r == IR_MATCH_NEED_MORE_DATA) {
                /*
                 * - For infrared code rc5, which header phase
                 * may constain two symbols,
                 * and the 2th is not appear yet,
                 * we will check after 200ms to see
                 * whether the 2th header key received
                 * or not. If still not received, discard
                 * the 1st key.
                 * - While a frame received not integrated,
                 * we need to conditional wait for a while,
                 * after timeout, and the frame still cannot
                 * be parsed, delete the frame.
                 *
                 * Another case is there are more infra code
                 * in protocol array, and some of them have
                 * more than one header bits.
                 * When the first loop of this timer returned
                 * needs more data, but take into account of
                 * 200ms after the first check,
                 * the follow symbol will be received,
                 * if not, we do discard the symbols
                 * for we have escaped 200ms!
                 */
                if (deadline && time_after_eq(jiffies, deadline)) {
                    ip = ir_prot_next(ip);
                    continue;
                } else {
                    deadline = jiffies +
                               msecs_to_jiffies(
                                   SYMBOL_PARSE_TIMEOUT);
                    ir->symbol_timer.expires = jiffies
                                               + msecs_to_jiffies(
                                                   SYMBOL_PARSE_TIMEOUT);
                    mod_timer(&ir->symbol_timer, ir->symbol_timer.expires);
                    spin_unlock_irqrestore(&irlock, flag);

                    goto OUT;
                }
            }

            ip = ir_prot_next(ip);
        }
        spin_unlock_irqrestore(&irlock, flag);

        /*
         * clear the header symbol first!
         * think about there are symbols sequence like this:
         * header,data,but no burst.
         * we should destroy header first to avoid dead loop!
         */
        if (memset_s(key, sizeof(key_attr), 0, sizeof(*key))) {
            hiir_error("memset_s key failed\n");
            return;
        }

        ir->symbol_buf->reader++;
        if (ir->symbol_buf->reader == MAX_SYMBOL_NUM) {
            ir->symbol_buf->reader = 0;
        }

        /* for we increased 'reader' cursor, so get next '0' */
        key = ir_get_read_key_n(ir->symbol_buf, 0);
    }
    /*
     * all symbols are discarded, clear deadline,
     * no need to restart this timer.
     */
    deadline = 0;
OUT:
    return;
}

static hi_void ir_bh_symbol(hi_ulong data)
{
    ir_priv *ir = &g_ir_local;
    /*
     * if frame cannot be parsed this time,
     * we will try again after symbol_timer expires.
     */
    if (ir_symbol_proc(&g_ir_local)) {
        del_timer(&ir->symbol_timer);
        ir->symbol_timer.expires = jiffies
                                   + msecs_to_jiffies(SYMBOL_PARSE_TIMEOUT);
        add_timer(&ir->symbol_timer);
    }

    return;
}

static irqreturn_t ir_isr_symbol(hi_s32 irq, hi_void *dev_id)
{
    hi_u32        stat;
    hi_u32        stat1;
    hi_u32        i;
    hi_u32        cnt;
    hi_u32        v;
    ir_priv       *ir  = &g_ir_local;
    key_attr      key = {0};

    stat = readl(ir->base + IR_INT_STATUS);
    hiir_debug("irq stat:0x%x\n", stat);
    stat1 = (stat & IR_KEY_STAT_MASK);
    if (stat1) {
        hiir_debug("unkown interrupt 0x%x\n", stat1);

        if (stat1 & IR_INTMS_RCV) {
            v = readl(ir->base + IR_DATAL);
            writel(IR_INTC_RCV, ir->base + IR_INT_CLR);
        }

        if (stat1 & IR_INTMS_FRAMERR) {
            writel(IR_INTC_FRAMERR, ir->base + IR_INT_CLR);
        }

        if (stat1 & IR_INTMS_OVERFLOW) {
            writel(IR_INTC_OVERFLOW, ir->base + IR_INT_CLR);
        }

        if (stat1 & IR_INTMS_RELEASE) {
            writel(IR_INTC_RELEASE, ir->base + IR_INT_CLR);
        }
    }

    g_time_out_flag = HI_FALSE;
    stat &= IR_SYMBOL_STAT_MASK;
    if (stat & (IR_INTMS_OVERRUN)) {
        ir_clear_fifo(ir);
        stat &= ~(IR_INTMS_OVERRUN);
        writel(IR_INTC_OVERRUN, ir->base + IR_INT_CLR);

        if ((ir->chip_param.ir_max_bits >= 1) &&
            (ir->chip_param.ir_max_bits < 0x1f)) {
             ir->chip_param.ir_max_bits--;
            v  = readl(ir->base + IR_CFG);
            v &= 0xFFFFC0FF;
            v |= (ir->chip_param.ir_max_bits << 8);
            writel(v, ir->base + IR_CFG);
        }

        hiir_debug("int overrun!\n");
    }

    if (stat & (IR_INTMS_SYMB_RCV | IR_INTMS_TIME_OUT)) {
        cnt = readl(ir->base + IR_DATAH);
        for (i = 0; i < cnt; i++) {
            v = readl(ir->base + IR_DATAL);
            key.upper = (hi_u64)
                        ((v & 0xffff0000) >> 16) * 10;
            key.lower = (hi_u64)(v & 0xffff) * 10;
            if (ir->key_fetch == KEY_FETCH_RAW) {
                ir_insert_key_tail(ir->key_buf, &key);
            } else {
                ir_insert_key_tail(ir->symbol_buf, &key);
            }
        }

        if (stat & IR_INTMS_SYMB_RCV) {
            writel(IR_INTC_SYMB_RCV, ir->base + IR_INT_CLR);
        } else {
            writel(IR_INTC_TIME_OUT, ir->base + IR_INT_CLR);
            g_time_out_flag = HI_TRUE;
            if ((g_last_key_state != HI_NULL) &&
                (g_last_key_state->lower || g_last_key_state->upper) &&
                (g_last_key_state->key_stat != KEY_STAT_UP) &&
                (ir->key_fetch == KEY_FETCH_PARSED)) {
                g_last_key_state->key_stat = KEY_STAT_UP;
                g_redundant_frame_flag = 0;
                if (ir->key_up_event) {
                    ir_insert_key_tail(ir->key_buf, g_last_key_state);
                    wake_up_interruptible(&ir->read_wait);
                }
                g_time_out_flag = HI_FALSE;
                g_last_key_state = HI_NULL;
                return IRQ_HANDLED;
            }
        }

        stat &= ~(IR_INTMS_SYMB_RCV | IR_INTMS_TIME_OUT);
        if (ir->key_fetch == KEY_FETCH_PARSED) {
            tasklet_schedule(&ir->ir_bh);
        } else {
            wake_up_interruptible(&ir->read_wait);
        }
    }

    if (stat) {
        hiir_error("unkown interrupt 0x%x\n", stat);
    }

    return IRQ_HANDLED;
}

static hi_void ir_init_prot(hi_void)
{
#ifdef NEC_SUPPORT
    nec_init();
#endif

#ifdef RC6_SUPPORT
    rc6_init();
#endif

#ifdef RC5_SUPPORT
    rc5_init();
#endif

#ifdef RSTEP_SUPPORT
    rstep_init();
#endif

#ifdef SONY_SUPPORT
    sony_init();
#endif

#ifdef TC9012_SUPPORT
    tc9012_init();
#endif

#ifdef CREDIT_SUPPORT
    credit_init();
#endif

#ifdef XMP_SUPPORT
    xmp_init();
#endif
}

static hi_void ir_deinit_prot(hi_void)
{
#ifdef NEC_SUPPORT
    nec_exit();
#endif

#ifdef RC6_SUPPORT
    rc6_exit();
#endif

#ifdef RC5_SUPPORT
    rc5_exit();
#endif

#ifdef RSTEP_SUPPORT
    rstep_exit();
#endif

#ifdef SONY_SUPPORT
    sony_exit();
#endif

#ifdef TC9012_SUPPORT
    tc9012_exit();
#endif

#ifdef CREDIT_SUPPORT
    credit_exit();
#endif

#ifdef XMP_SUPPORT
    xmp_exit();
#endif
}

static hi_s32 ir_open(struct inode *inode, struct file *filp)
{
    hi_s32    ret;
    ir_priv   *ir = &g_ir_local;

    if (atomic_inc_return(&ir->refcnt) == 1) {
        ir_set_clk(HI_TRUE);
        ir_init_prot();
        ret = ir_config(ir);
        if (ret) {
            return ret;
        }

        ir_clear_fifo(ir);
        writel((hi_u32) ~0, ir->base + IR_INT_CLR);
        writel(IR_INT_MASK_SYMBOL, ir->base + IR_INT_MASK);
        init_waitqueue_head(&ir->read_wait);
        ir_start(ir);
    }

    return 0;
}

static hi_s32 ir_close(struct inode *inode, struct file *filp)
{
    ir_priv *ir = &g_ir_local;

    if (down_interruptible(&ir->sem)) {
        hiir_info("fail to hold read semaphore!\n");
    }

    if (atomic_dec_and_test(&ir->refcnt)) {
        ir_clear_fifo(ir);
        writel((hi_u32) ~0, ir->base + IR_INT_CLR);
        writel((hi_u32) ~0, ir->base + IR_INT_MASK);
        ir_disable(ir);

        if (memset_s(ir->key_buf, sizeof(ir_buffer), 0, sizeof(ir_buffer))) {
            hiir_error("memset_s ir->key_buf failed\n");
            up(&ir->sem);
            return -EFAULT;
        }
        if (ir->chip_param.work_mode == IR_MODE_SYMBOL) {
            if (memset_s(ir->symbol_buf, sizeof(ir_buffer), 0,
                         sizeof(ir_buffer))) {
                hiir_error("memset_s key failed\n");
                up(&ir->sem);
                return -EFAULT;
            }
        }
        ir_deinit_prot();
        ir_set_clk(HI_FALSE);
    }

    up(&ir->sem);
    return 0;
}

static hi_void ir_reset(ir_priv *ir)
{
    ir_enable(ir);

    writel(0, ir->base + IR_INT_MASK);
    if (memset_s(ir->key_buf, sizeof(ir_buffer), 0, sizeof(ir_buffer))) {
        hiir_error("memset_s ir->key_buf failed\n");
        return;
    }

    if (ir->chip_param.work_mode == IR_MODE_SYMBOL) {
        if (memset_s(ir->symbol_buf, sizeof(ir_buffer), 0, sizeof(ir_buffer))) {
            hiir_error("memset_s ir->symbol_buf failed\n");
            return;
        }
    }

    ir_clear_fifo(ir);
    ir_disable(ir);
}

static hi_void ir_setwork(hi_ulong arg, ir_priv *ir)
{
    hi_s32 ret;
    /*
     * config ir_cfg register and set enable,
     * enable interrupt.
     */
    if (!arg) {
        ir_reset(ir);
        ir_deinit_prot();
        ir_set_clk(HI_FALSE);
        return;
    }

    ir_set_clk(HI_TRUE);
    ir_init_prot();
    ret = ir_config(ir);
    if (ret) {
        return;
    }

    ir_clear_fifo(ir);
    if (ir->chip_param.work_mode == IR_MODE_SYMBOL) {
        writel(IR_INT_MASK_SYMBOL, ir->base + IR_INT_MASK);
    }

    ir_start(ir);
    return;

}

static hi_s32 ir_set_protocol_enable(hi_ulong arg)
{
    ir_protocol    *ip = NULL;
    hi_char        name[PROTOCOL_NAME_SZ] = {0};
    hi_void   	   *tmp = NULL;

    tmp = (hi_void *)(uintptr_t)arg;
    if (tmp == NULL) {
        hiir_info("invalid parameter\n");
        return -EINVAL;
    }

    if (copy_from_user((hi_void *)name, tmp, PROTOCOL_NAME_SZ - 1)) {
        return -EFAULT;
    }
    name[PROTOCOL_NAME_SZ - 1] = '\0';

    ip = ir_prot_first();
    while (ip && ip->ir_code_name && ip->idx != IR_PROT_MAX) {
        if (!strncmp(ip->ir_code_name, name, strlen(ip->ir_code_name))) {
            ip->disabled = 0;
            return 0;
        }

        hiir_info("\n ip->ir_code_name = %s, name = %s  \n", ip->ir_code_name, name);
        ip = ir_prot_next(ip);
    }

    return -EINVAL;
}

static hi_s32 ir_set_protocol_disable(hi_ulong arg)
{
    ir_protocol    *ip = NULL;
    hi_char        name[PROTOCOL_NAME_SZ] = {0};
    hi_void        *tmp = NULL;

    tmp = (hi_void *)(uintptr_t)arg;
    if (tmp == NULL) {
        hiir_info("invalid parameter\n");
        return -EINVAL;
    }

    if (copy_from_user((hi_void *)name, tmp, PROTOCOL_NAME_SZ - 1)) {
        return -EFAULT;
    }
    name[PROTOCOL_NAME_SZ - 1] = '\0';

    ip = ir_prot_first();
    while (ip && ip->ir_code_name && ip->idx != IR_PROT_MAX) {
        if (!strncmp(ip->ir_code_name, name, strlen(ip->ir_code_name))) {
            ip->disabled = 1;
            return 0;
        }

        ip = ir_prot_next(ip);
    }

    return -EINVAL;
}

static hi_s32 ir_get_protocol_status(hi_ulong arg)
{
    ir_protocol    *ip = NULL;
    hi_char        name[PROTOCOL_NAME_SZ] = {0};
    hi_void        *tmp = NULL;

    tmp = (hi_void *)(uintptr_t)arg;
    if (tmp == NULL) {
        hiir_info("invalid parameter\n");
        return -EINVAL;
    }

    if (copy_from_user((hi_void *)name, tmp, PROTOCOL_NAME_SZ - 1)) {
        return -EFAULT;
    }
    name[PROTOCOL_NAME_SZ - 1] = '\0';

    ip = ir_prot_first();
    while (ip && ip->ir_code_name && ip->idx != IR_PROT_MAX) {
        if (!strncmp(ip->ir_code_name, name, strlen(ip->ir_code_name))) {
            if (copy_to_user(tmp, &ip->disabled, sizeof(hi_s32))) {
                return -EFAULT;
            }

            return 0;
        }

        ip = ir_prot_next(ip);
    }

    return -EINVAL;
}

static hi_slong ir_ioctl(struct file *filp, hi_u32 cmd, hi_ulong arg)
{
    hi_slong    ret = 0;
    ir_priv     *ir = &g_ir_local;

    switch (cmd) {
        case CMD_IR_ENABLE_KEYUP:
            ir->key_up_event = arg;
            break;

        case CMD_IR_ENABLE_REPKEY:
            ir->key_repeat_event = arg;
            if (!ir->key_repeat_event) {
                hiir_info("You have disabled key hold event," \
                          " which will cause some crazy key press" \
                          " action not recevied.!\n" \
                          " For some protocol like nec full," \
                          " press twice the same key very quickly will be" \
                          " recoginzed as a hold event.\n");
            }

            break;

        case CMD_IR_SET_REPKEY_TIMEOUT:
            ir->key_repeat_interval = arg;
            break;

        case CMD_IR_SET_ENABLE:
            ir_setwork(arg, ir);
            break;

        case CMD_IR_RESET:

            /*
             * disable interrupt,
             * clear key & symbol buf and more what?
             */
            writel(0, ir->base + IR_INT_MASK);
            if (memset_s(ir->key_buf, sizeof(ir_buffer), 0, sizeof(ir_buffer))) {
                hiir_error("memset_s ir->key_buf failed\n");
                return -EFAULT;
            }
            if (ir->chip_param.work_mode == IR_MODE_SYMBOL) {
                if (memset_s(ir->symbol_buf, sizeof(ir_buffer), 0, sizeof(ir_buffer))) {
                    hiir_error("memset_s ir->symbol_buf failed\n");
                    return -EFAULT;
                }
            }

            ir_clear_fifo(ir);
            writel(IR_INT_MASK_SYMBOL, ir->base + IR_INT_MASK);
            break;

        case CMD_IR_SET_BLOCKTIME:
            /* set read time out */
            ir->key_read_timeout = arg;
            break;

        case CMD_IR_SET_FORMAT:
            /*
             * I think this command is not necessary under both mode.
             * so, abandon it.
             */
            break;

        case CMD_IR_SET_BUF:
            /*
             * Current buffer is large enough,
             * and in this driver, buffer size is fix.
             * abandon it.
             */
            break;

        case CMD_IR_SET_FETCH_METHOD:
            ir->key_fetch = (key_fetch_mode)arg;
            break;

        case CMD_IR_SET_PROT_ENABLE:
            ret = ir_set_protocol_enable(arg);
            break;

        case CMD_IR_SET_PROT_DISABLE:
            ret = ir_set_protocol_disable(arg);
            break;

        case CMD_IR_GET_PROT_ENABLED:
            ret = ir_get_protocol_status(arg);
            break;

        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}

static hi_u32 ir_poll(struct file *filp, struct poll_table_struct *wait)
{
    ir_priv *ir = &g_ir_local;

    if (down_interruptible(&ir->sem)) {
        return -EAGAIN;
    }

    if (ir->key_buf->writer != ir->key_buf->reader) {
        up(&ir->sem);
        return POLLIN | POLLRDNORM;
    }

    poll_wait(filp, &ir->read_wait, wait);

    up(&ir->sem);
    return 0;
}

static ssize_t ir_read(struct file *filp, hi_char __user *buf,
                       size_t count, loff_t *f_pos)
{
    ir_priv     *ir = &g_ir_local;
    hi_s32      ret = 0;
    key_attr    *key = NULL;

    if (atomic_read(&ir->refcnt) == 0) {
        HI_FATAL_IR("not open!\n");
        return -1;
    }

    if (down_interruptible(&ir->sem)) {
        return -EAGAIN;
    }

    ir->key_debug_trycount++;

    /*
     * NOTE: writer may run over reader,
     * in these case should increase buffer more large.
     * because ir module may not generate so many data
     * at the same time in normal case, so here simply check cursor..
     */
    while (ir->key_buf->writer == ir->key_buf->reader) {
        if (filp->f_flags & O_NONBLOCK) {
            ret = -EAGAIN;
            goto OUT;
        }

        if (ir->key_read_timeout == (hi_u32) ~0) {
            ir->key_read_timeout = 200;
        }

        ret = wait_event_interruptible_timeout(ir->read_wait,
                                               ir->key_buf->writer != ir->key_buf->reader,
                                               msecs_to_jiffies(ir->key_read_timeout));
        if (ret < 0) {
            ret = -EAGAIN;
            goto OUT;
        } else if (ret == 0) {
            ret = -ETIME;
            goto OUT;
        } else {
            break;
        }
    }

    ret = 0;
    while (ir->key_buf->writer != ir->key_buf->reader && ret < count) {
        key = ir_reader_inc(ir->key_buf);
        if (key->key_stat == KEY_STAT_DOWN) {
            hi_drv_stat_event(HI_STAT_EVENT_KEY_OUT, (hi_u32)key->lower);
        }

        if ((buf + ret) != NULL) {
            if (copy_to_user(buf + ret, key, sizeof(*key))) {
                ret = -EFAULT;
                goto OUT;
            }
        }
        ir->key_debug_succeed_count++;
        if (memset_s(key, sizeof(key_attr), 0, sizeof(*key))) {
            hiir_error("memset_s key failed\n");
            up(&ir->sem);
            return -EFAULT;
        }
        ret += sizeof(*key);
    }

OUT:
    up(&ir->sem);
    return ret;
}

static ssize_t ir_write(struct file *f, const hi_char __user *b, size_t c, loff_t *o)
{
    return 0;
}

static struct file_operations g_ir_fops = {
    .owner           =     THIS_MODULE,
    .open            =     ir_open,
    .write           =     ir_write,
    .unlocked_ioctl  =     ir_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl    =     ir_ioctl,
#endif
    .poll            =     ir_poll,
    .read            =     ir_read,
    .release         =     ir_close
};
static hi_void ir_init_callbacks(ir_priv *ir)
{
    ir->callbacks.isr_handle  = ir_isr_symbol;
    ir->callbacks.data_handle = ir_bh_symbol;
}

static hi_s32 ir_request_resource(ir_priv *ir)
{
    hi_s32 ret = -ENOMEM;
    if (ir->chip_param.work_mode == IR_MODE_SYMBOL) {
        ir->symbol_buf =
            HI_KMALLOC(HI_ID_IR, sizeof(ir_buffer), GFP_KERNEL | __GFP_ZERO);
        if (!ir->symbol_buf) {
            goto SYMBOL_BUF;
        }

        if (memset_s(ir->symbol_buf, sizeof(ir_buffer), 0, sizeof(ir_buffer))) {
            hiir_error("memset_s ir->symbol_buf failed\n");
            goto KEY_BUF;
        }

        tasklet_init(&ir->ir_bh, ir->callbacks.data_handle, (hi_ulong)(uintptr_t)ir);
    }

    ir->key_buf = HI_KMALLOC(HI_ID_IR, sizeof(ir_buffer), GFP_KERNEL | __GFP_ZERO);
    if (!ir->key_buf) {
        goto KEY_BUF;
    }

    if (memset_s(ir->key_buf, sizeof(ir_buffer), 0, sizeof(ir_buffer))) {
        hiir_error("memset_s ir->key_buf failed\n");
        goto IRQ;
    }

    init_timer(&ir->symbol_timer);
    ir->symbol_timer.function = ir_symbol_timer_proc;
    ir->symbol_timer.data = (hi_ulong)(uintptr_t)ir;
    sema_init(&ir->sem, 1);
    ret = -EFAULT;

    if (hi_drv_osal_request_irq(ir->irq, ir->callbacks.isr_handle, IRQF_SHARED, "ir_s2", (hi_void *)ir)) {
        goto IRQ;
    } else {
        if (hi_drv_sys_set_irq_affinity(HI_ID_IR, ir->irq, "ir_s2") != HI_SUCCESS) {
            hiir_info("hi_drv_sys_set_irq_affinity failed.\n");
        }
    }

    return 0;

IRQ:
    HI_KFREE(HI_ID_IR, ir->key_buf);
KEY_BUF:
    if (ir->chip_param.work_mode == IR_MODE_SYMBOL) {
        HI_KFREE(HI_ID_IR, ir->symbol_buf);
    }
SYMBOL_BUF:
    return ret;
}

static hi_void ir_adjust_freq(hi_void)
{
    g_ir_frequence = DFT_FREQ;

    return;
}

static hi_void ir_free_resource(ir_priv *ir)
{
    del_timer_sync(&ir->symbol_timer);
    hi_drv_osal_free_irq(ir->irq, "ir_s2", (hi_void *)ir);
    HI_KFREE(HI_ID_IR, ir->symbol_buf);
    HI_KFREE(HI_ID_IR, ir->key_buf);
}

static inline hi_void parameter_helper_symbol(hi_void)
{
    hiir_info("available parameters under symbol mode are: \n" \
              "\t- freq: frequence control.\n" \
              "\t- symbol_width: max pluse + space width\n" \
              "\t- int_level: how many symbols to generate a interrupt.\n" \
              "\t- key_fetch: key fetch mode, set 1 to fetch raw symbol\n");
}

static hi_s32 ir_check_module_param(hi_void)
{
    if ((g_ir_keyhold_timeout >= 1000) || (g_ir_frequence > 0x7f)) {
        hiir_info("key hold timeout >= 0x1000"
                  " or frequence > 0x7f!\n");
        return -EINVAL;
    }

    if (g_ir_keyhold_timeout < 150) {
        hiir_info("key hold time too small," \
                  "may due to subsequence of 'DOWN,HOLD,,,UP,DOWN,UP'" \
                  "while a key being press for a while\n");
    }

    /* now we don't support key mode */
    if (g_ir_work_mode == IR_MODE_SYMBOL) {
        if ((g_ir_symbol_width > 0xffff) ||
            (g_ir_symbol_format > 1) ||
            (g_ir_int_level > 0x1f) ||
            (g_ir_frequence > 0x7f) ||
            (g_ir_key_fetch_raw > 1)) {
            parameter_helper_symbol();
            return -EINVAL;
        }

        return 0;
    }

    return -EINVAL;
}

static struct miscdevice g_ir_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = IR_DEVICE_NAME,
    .fops  = &g_ir_fops,
};

static hi_s32 ir_plat_driver_probe(struct platform_device *pdev)
{
    hi_s32     ret;
    ir_priv    *ir = &g_ir_local;

    ir_set_clk(HI_FALSE);
    ir_adjust_freq();

    if (ir_check_module_param()) {
        hiir_info("invalid module parameters!\n");
        return -EINVAL;
    }

    if (memset_s(ir, sizeof(ir_priv), 0, sizeof(ir_priv))) {
        hiir_error("memset_s ir failed\n");
        return -EINVAL;
    }

    if (g_ir_work_mode == IR_MODE_SYMBOL) {
        ir->chip_param.ir_max_symbol = g_ir_symbol_width;
        ir->chip_param.ir_max_bits = g_ir_int_level;
        ir->key_fetch = g_ir_key_fetch_raw;
        ir->chip_param.ir_format = g_ir_symbol_format;
        ir->chip_param.ir_freq = g_ir_frequence;
    }

    ir->chip_param.work_mode = IR_MODE_SYMBOL;

    ret = ir_protocol_init();
    if (ret) {
        goto PROT;
    }

    ir_init_callbacks(ir);

    if (strncpy_s(ir->dev_name, IR_DEVICE_NAME_SZ, IR_DEVICE_NAME,
                  strlen(IR_DEVICE_NAME) >= (IR_DEVICE_NAME_SZ - 1) ?
                  IR_DEVICE_NAME_SZ : (strlen(IR_DEVICE_NAME) + 1))) {
        hiir_info("strncpy_s to ir->dev_name failed\n");
        return -EINVAL;
    }

    ir->irq  = IR_IRQ_NO;
    ir->base = ioremap_nocache(IR_IO_BASE, IR_ADDR_SIZE);
    if (ir->base == HI_NULL) {
        hiir_info("ir ioremap_nocache err! \n");
        return -EINVAL;
    }

    ir->key_hold_timeout_time = g_ir_keyhold_timeout;
    ir->key_read_timeout = (hi_u32) ~0;
    ir->key_up_event = 1;
    ir->key_repeat_event = 1;
    ir->key_repeat_interval = 90;
    ret = ir_request_resource(ir);
    if (ret) {
        goto RES;
    }

    ret = misc_register(&g_ir_misc);
    if (ret) {
        goto REG_DEV;
    }

    atomic_set(&ir->refcnt, 0);

#ifdef CONFIG_DEVFS_FS
    devfs_mk_cdev(MKDEV(ir->major, 0),
                  S_IFCHR | S_IRUGO | S_IWUSR, ir->dev_name);
#endif

#ifdef MODULE
    HI_PRINT("Load hi_ir.ko success.  \t(%s)\n", VERSION_STRING);
#endif
    return 0;

REG_DEV:
    ir_free_resource(ir);
RES:
PROT:
    hiir_error("Load hi_ir.ko fail(0x%x)!\n", ret);
    return ret;
}

static hi_s32 ir_plat_driver_remove(struct platform_device *pdev)
{
    ir_priv *ir = &g_ir_local;

#ifdef CONFIG_DEVFS_FS
    devfs_remove(ir->dev_name);
#endif
    misc_deregister(&g_ir_misc);
    ir_free_resource(ir);
    iounmap(ir->base);
    ir_protocol_exit();
    return 0;
}

#ifdef HI_PROC_SUPPORT
static hi_s32 ir_proc_read(struct seq_file *p, hi_void *v)
{
    ir_priv        *ir = &g_ir_local;
    ir_protocol    *ip = NULL;
    hi_u8          i = 0;
    hi_ulong       flag;

    if (p == HI_NULL) {
        hiir_error("p is null pointer!\n");
        return HI_FAILURE;
    }

    HI_PROC_PRINT(p, "---------Hisilicon IR Info---------\n");
    HI_PROC_PRINT(p,
                  "IR   Code               \t :%s\n"
                  "IR   WorkMode           \t :%s\n"
                  "IR   FetchMode          \t :%s\n"
                  "IR   KeyUpEnable        \t :%s\n"
                  "IR   UpEventDelay       \t :%d(ms)\n"
                  "IR   RepeatkeyEnable    \t :%s\n"
                  "IR   RepkeyDelayTime    \t :%d(ms)\n"
                  "IR   ReportKeyBlockTime \t :%d(ms)\n"
                  "IR   ModuleFrequence    \t :%d(MHz)\n",

                  "IR_S2",
                  (ir->chip_param.work_mode == 0) ? "Chip Report Keys" : "Chip Report Symbols",
                  (ir->key_fetch) ? "Drive Report Raw Symbols" : "Drive Report Parsed Symbols",
                  (ir->key_up_event) ? "Enable" : "Disable",
                  ir->key_hold_timeout_time,
                  (ir->key_repeat_event) ? "Enable" : "Disable",
                  ir->key_repeat_interval,
                  ir->key_read_timeout,
                  (ir->chip_param.ir_freq + 1));

    HI_PROC_PRINT(p, "\nRegistered Protocols info:\n");

    spin_lock_irqsave(&irlock, flag);
    ip = ir_prot_first();
    while (ir_prot_valid(ip)) {
        HI_PROC_PRINT(p, "no.%d:\tStatus:%12s, \tName: \t%s \n",
                      i++, ip->disabled ? "Disabled" : "Enabled", ip->ir_code_name);
        ip = ir_prot_next(ip);
    }

    spin_unlock_irqrestore(&irlock, flag);

    HI_PROC_PRINT(p, "\nKey getting info:\n");
    HI_PROC_PRINT(p,
                  "Get(Try/OK) \t :%d/%d\n",
                  ir->key_debug_trycount, ir->key_debug_succeed_count);

    if (ir->key_buf) {
        HI_PROC_PRINT(p, "\nKey buffer info:\n");
        HI_PROC_PRINT(p, "\tBuffer size:  \t%d keys \n", MAX_SYMBOL_NUM);
        HI_PROC_PRINT(p, "\tReader at:  \t%d\n", ir->key_buf->reader);
        HI_PROC_PRINT(p, "\tWriter at:  \t%d\n", ir->key_buf->writer);
    }

    if ((ir->chip_param.work_mode == IR_MODE_SYMBOL) && ir->symbol_buf) {
        HI_PROC_PRINT(p, "\nSymbol buffer info:\n");
        HI_PROC_PRINT(p, "\tBuffer size: \t%d symbols\n", MAX_SYMBOL_NUM);
        HI_PROC_PRINT(p, "\tReader at: \t%d\n", ir->symbol_buf->reader);
        HI_PROC_PRINT(p, "\tWriter at: \t%d\n", ir->symbol_buf->writer);
    }

    return HI_SUCCESS;
}
#endif

#ifdef CONFIG_PM
static hi_s32 ir_plat_driver_suspend(struct platform_device *pdev,
                                     pm_message_t state)
{
    ir_priv *ir = &g_ir_local;

    ir_set_clk(HI_FALSE);
    ir_disable(ir);
    HI_PRINT("IR suspend OK\n");
    return 0;
}

static hi_s32 ir_plat_driver_resume(struct platform_device *pdev)
{
    ir_priv    *ir = &g_ir_local;
    hi_s32     ret;

    ir_set_clk(HI_TRUE);
    ir_reset(ir);
    if (atomic_read(&ir->refcnt) > 0) {
        ret = ir_config(ir);
        if (ret) {
            hiir_error(" Fail to set ir configuration. ret = 0x%x \n", ret);
        }

        writel(IR_INT_MASK_SYMBOL, ir->base + IR_INT_MASK);
        ir_start(ir);
    }

    HI_PRINT("IR resume OK\n");
    return 0;
}

#else
#define ir_plat_driver_suspend (NULL)
#define ir_plat_driver_resume (NULL)
#endif
static struct platform_driver g_ir_platform_driver = {
    .probe   = ir_plat_driver_probe,
    .remove  = ir_plat_driver_remove,
    .suspend = ir_plat_driver_suspend,
    .resume  = ir_plat_driver_resume,
    .driver  = {
        .owner = THIS_MODULE,
        .name  = IR_DEVICE_NAME,
        .bus   = &platform_bus_type,
    },
};
#define IR_IO_SZ (SZ_1K * 4)
static struct resource g_ir_resources[] = {
    [0] = {
        .start = IR_IO_BASE,
        .end   = IR_IO_BASE + IR_IO_SZ - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = IR_IRQ_NO,
        .end   = IR_IRQ_NO,
        .flags = IORESOURCE_IRQ,
    }
};
static hi_void ir_platform_device_release(struct device *dev)
{
}

static struct platform_device g_ir_platform_device = {
    .name                  = IR_DEVICE_NAME,
    .id  =                          0,
    .dev = {
        .platform_data     = NULL,
        .dma_mask          = (u64 *)~0,
        .coherent_dma_mask = (u64) ~0,
        .release           = ir_platform_device_release,
    },
    .num_resources         = ARRAY_SIZE(g_ir_resources),
    .resource              = g_ir_resources,
};

hi_s32 ir_drv_module_init(hi_void)
{
    hi_s32 ret = 0;
#ifdef HI_PROC_SUPPORT
    hi_proc_item *item = NULL;

    hi_proc_fn_set func_opt = {
        .read = ir_proc_read,
    };
#endif
    (hi_void)hi_drv_module_register(HI_ID_IR, "HI_IR", HI_NULL, HI_NULL);
#ifdef HI_PROC_SUPPORT
    item = hi_drv_proc_add_module(HI_MOD_IR_NAME, &func_opt, NULL);
    if (!item) {
        hiir_error("add proc ir failed\n");
        return HI_FAILURE;
    }
#endif

    ret = platform_device_register(&g_ir_platform_device);
    if (ret) {
        hiir_info("register platform device failed!");
        goto ERROR_REGISTER_DEVICE;
    }

    ret = platform_driver_register(&g_ir_platform_driver);
    if (ret) {
        hiir_info("register platform driver failed!");
        goto ERROR_REGISTER_DRIVER;
    }

    return ret;

ERROR_REGISTER_DEVICE:
    platform_device_unregister(&g_ir_platform_device);
ERROR_REGISTER_DRIVER:
    return -1;
}

hi_void ir_drv_module_exit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_drv_proc_remove_module(HI_MOD_IR_NAME);
#endif
    hi_drv_module_unregister(HI_ID_IR);
    platform_driver_unregister(&g_ir_platform_driver);
    platform_device_unregister(&g_ir_platform_device);
}

#ifdef MODULE
module_init(ir_drv_module_init);
module_exit(ir_drv_module_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HISILION");
