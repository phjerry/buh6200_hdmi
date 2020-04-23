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
#include <linux/cdev.h>
#include "hi_drv_sys.h"
#include "hi_drv_ir.h"
#include "drv_ir_ioctl.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_stat.h"
#include "hi_drv_module.h"
#include "hi_drv_osal.h"
#include "hi_errno.h"
#include "drv_ir_codedef.c"
#include "linux/hisilicon/securec.h"

#define  IR_ENABLE      0x00
#define  IR_CONFIG      0x04
#define  CNT_LEADS      0x08
#define  CNT_LEADE      0x0c
#define  CNT_SLEADE     0x10
#define  CNT0_B         0x14
#define  CNT1_B         0x18
#define  IR_BUSY        0x1c
#define  IR_DATAH       0x20
#define  IR_DATAL       0x24
#define  IR_INTM        0x28
#define  IR_INTS        0x2c
#define  IR_INTC        0x30
#define  IR_START       0x34
#define  IR_CLK_REG     0x00840060

#define  IR_INT_RCV         (1L << 16)
#define  IR_INT_FRAMERR     (1L << 17)
#define  IR_INT_OVERFLOW    (1L << 18)
#define  IR_INT_RELEASE     (1L << 19)
#define  IR_INTC_SYMB_RCV   (1L << 16)
#define  IR_INTC_TIME_OUT   (1L << 17)
#define  IR_INTC_OVERRUN    (1L << 18)

#define  WRITE_REG(addr, value) ((*(volatile hi_u32 *)((uintptr_t)(addr))) = (value))
#define  READ_REG(addr) (*(volatile hi_u32 *)((uintptr_t)(addr)))

#define  IR_DELAY_TIME  200
#define  IR_MAX_BUF     100
#define  DEF_BLOCK_TIME 200

#define  IR_BUF_HEAD g_ir_attr_data.ir_key_buf[g_ir_attr_data.head]
#define  IR_BUF_TAIL g_ir_attr_data.ir_key_buf[g_ir_attr_data.tail]
#define  IR_BUF_LAST (g_ir_attr_data.ir_key_buf[(g_ir_attr_data.head == 0) ?\
                     (g_ir_attr_data.ir_key_buf_len - 1) : (g_ir_attr_data.head - 1)])
#define  INC_BUF(x, len) (((x)+1) % (len))

typedef struct {
    hi_unf_ir_code    ir_code;
    ir_dev_para       ir_dev_para;
    hi_u32            head;
    hi_u32            tail;
    ir_key            ir_key_buf[IR_MAX_BUF];
    hi_u32            ir_key_buf_len;

    hi_bool           is_enable;
    hi_bool           enable_key_up;
    hi_bool           enable_rep_key;
    hi_u32            rep_key_delay_time;
    hi_u32            ir_block_time;

    hi_bool           check_rep_key;
    hi_bool           get_release_int;
    wait_queue_head_t ir_key_wait_queue;
} ir_attr;

static atomic_t       g_ir_count = ATOMIC_INIT(0);
DEFINE_SEMAPHORE(ir_mutex);
static hi_handle      g_hi_ir_irq;
static ir_attr        g_ir_attr_data;
static hi_u8          *g_ir_reg_addr;

hi_void rep_key_time_out_proc(hi_length_t timer_para);
DEFINE_TIMER(rep_key_time_out_timer, rep_key_time_out_proc, 0, 0);

hi_void rep_key_time_out_proc(hi_length_t timer_para)
{
    g_ir_attr_data.check_rep_key = HI_FALSE;
    return;
}

static hi_void ir_set_clk(hi_bool enable)
{
    hi_u32 *pir_clk_reg = HI_NULL_PTR;
    hi_u32 reg_val = 0;

    pir_clk_reg = ioremap_nocache(IR_CLK_REG, 0x4);
    if (pir_clk_reg == HI_NULL_PTR) {
        HI_ERR_IR("ioremap_nocache IR CLK err! \n");
        return;
    }

    reg_val = READ_REG(pir_clk_reg);

    if (enable == HI_TRUE) {
        reg_val |= 0x10;
        reg_val &= 0xffffffdf;
    } else {
        reg_val |= 0x20;
        reg_val &= 0xffffffef;
    }

    WRITE_REG(pir_clk_reg, reg_val);
    iounmap(pir_clk_reg);

    return;
}

#ifdef HI_PROC_SUPPORT
hi_s32 ir_proc_read(struct seq_file *p, hi_void *v)
{
    hi_u8 ir_code_string[32] = {0};

    if (p == HI_NULL) {
        HI_ERR_IR("null pointer!\n");
        return HI_FAILURE;
    }

    if (g_ir_attr_data.ir_code == HI_UNF_IR_CODE_NEC_SIMPLE) {
        if (strncpy_s(ir_code_string, sizeof(ir_code_string), "IR_CODE_NEC_SIMPLE", strlen("IR_CODE_NEC_SIMPLE"))) {
            HI_ERR_IR("strncpy_s to ir_code_string failed\n");
            return HI_FAILURE;
        }
    } else if (g_ir_attr_data.ir_code == HI_UNF_IR_CODE_TC9012) {
        if (strncpy_s(ir_code_string, sizeof(ir_code_string), "IR_CODE_TC9012", strlen("IR_CODE_TC9012"))) {
            HI_ERR_IR("strncpy_s to ir_code_string failed\n");
            return HI_FAILURE;
        }
    } else if (g_ir_attr_data.ir_code == HI_UNF_IR_CODE_NEC_FULL) {
        if (strncpy_s(ir_code_string, sizeof(ir_code_string), "IR_CODE_NEC_FULL", strlen("IR_CODE_NEC_FULL"))) {
            HI_ERR_IR("strncpy_s to ir_code_string failed\n");
            return HI_FAILURE;
        }
    } else if (g_ir_attr_data.ir_code == HI_UNF_IR_CODE_SONY_12BIT) {
        if (strncpy_s(ir_code_string, sizeof(ir_code_string), "IR_CODE_SONY_12BIT", strlen("IR_CODE_SONY_12BIT"))) {
            HI_ERR_IR("strncpy_s to ir_code_string failed\n");
            return HI_FAILURE;
        }
    }

    HI_PROC_PRINT(p, "---------hisilicon IR info---------\n");
    HI_PROC_PRINT(p,
                  "IR   enable             \t :%d\n"
                  "IR   head               \t :%d\n"
                  "IR   tail               \t :%d\n"
                  "IR   code               \t :%s\n"
                  "IR   enable_key_up             \t :%d\n"
                  "IR   enable_rep_key            \t :%d\n"
                  "IR   rep_key_delay_time    \t :%d\n"
                  "IR   ir_block_time        \t :%x\n",

                  g_ir_attr_data.is_enable,
                  g_ir_attr_data.head,
                  g_ir_attr_data.tail,
                  ir_code_string,
                  g_ir_attr_data.enable_key_up,
                  g_ir_attr_data.enable_rep_key,
                  g_ir_attr_data.rep_key_delay_time,
                  g_ir_attr_data.ir_block_time);

    return HI_SUCCESS;
}

hi_s32 ir_proc_write(struct file *file,
                     const char __user *buf, size_t count, loff_t *ppos)
{
    hi_char proc_para[64] = {0};

    if (count >= sizeof(proc_para)) {
        HI_ERR_IR("commond line is too long, please try it in %lu bytes\n", sizeof(proc_para));
        return HI_FAILURE;
    }

    if (buf == HI_NULL) {
        HI_ERR_IR("buf is null pointer\n");
        return HI_FAILURE;
    }

    if (copy_from_user(proc_para, buf, count)) {
        return -EFAULT;
    }

    return count;
}
#endif

hi_void ir_config(hi_void)
{
    hi_u32 value = 0;

    while (READ_REG(g_ir_reg_addr + IR_BUSY)) {
        HI_WARN_IR("ir is busy, waiting.\n");
    }

    value  = (g_ir_attr_data.ir_dev_para.code_type << 14);
    value |= (g_ir_attr_data.ir_dev_para.code_len - 1) << 8;
    value |= DFT_FREQ;
    WRITE_REG(g_ir_reg_addr + IR_CONFIG, value);

    value  = g_ir_attr_data.ir_dev_para.leads_min << 16;
    value |= g_ir_attr_data.ir_dev_para.leads_max;
    WRITE_REG(g_ir_reg_addr + CNT_LEADS, value);

    value  = g_ir_attr_data.ir_dev_para.leade_min << 16;
    value |= g_ir_attr_data.ir_dev_para.seade_max;
    WRITE_REG(g_ir_reg_addr + CNT_LEADE, value);

    value  = g_ir_attr_data.ir_dev_para.sleade_min << 16;
    value |= g_ir_attr_data.ir_dev_para.sleade_max;
    WRITE_REG(g_ir_reg_addr + CNT_SLEADE, value);

    value  = g_ir_attr_data.ir_dev_para.bit0_min << 16;
    value |= g_ir_attr_data.ir_dev_para.bit0_max;
    WRITE_REG(g_ir_reg_addr + CNT0_B, value);

    value  = g_ir_attr_data.ir_dev_para.bit1_min << 16;
    value |= g_ir_attr_data.ir_dev_para.bit1_max;
    WRITE_REG(g_ir_reg_addr + CNT1_B, value);

    WRITE_REG(g_ir_reg_addr + IR_INTM, 0x70000);
    WRITE_REG(g_ir_reg_addr + IR_START, 0x00);

    return;
}

hi_void ir_clr_int(hi_void)
{
    WRITE_REG(g_ir_reg_addr + IR_INTC, IR_INTC_SYMB_RCV);
    WRITE_REG(g_ir_reg_addr + IR_INTC, IR_INTC_TIME_OUT);
    WRITE_REG(g_ir_reg_addr + IR_INTC, IR_INTC_OVERRUN);

    return;
}

hi_s32 ir_isr(int irq, void *dev_id, struct pt_regs *regs)
{
    if (READ_REG(g_ir_reg_addr + IR_INTS) & IR_INT_FRAMERR) {
        WRITE_REG(g_ir_reg_addr + IR_INTC, 0x01 << 1);

        g_ir_attr_data.check_rep_key = 0;
        if ((g_ir_attr_data.enable_key_up) &&
            (IR_BUF_LAST.ir_key_state != HI_UNF_KEY_STATUS_UP) &&
            (IR_BUF_LAST.ir_key_state != HI_UNF_KEY_STATUS_MAX)) {
            IR_BUF_HEAD = IR_BUF_LAST;
            IR_BUF_HEAD.ir_key_state = HI_UNF_KEY_STATUS_UP;
            IR_BUF_HEAD.ir_protocol = (hi_unf_ir_protocol)g_ir_attr_data.ir_code;
            g_ir_attr_data.head = INC_BUF(g_ir_attr_data.head, g_ir_attr_data.ir_key_buf_len);
            wake_up_interruptible(&(g_ir_attr_data.ir_key_wait_queue));
        }
    } else if (READ_REG(g_ir_reg_addr + IR_INTS) & IR_INT_OVERFLOW) {
        HI_ERR_IR("ir fifo overflow.\n");
        WRITE_REG(g_ir_reg_addr + IR_INTC, 0x01 << 2);
    } else if (READ_REG(g_ir_reg_addr + IR_INTS) & IR_INT_RELEASE) {
        WRITE_REG(g_ir_reg_addr + IR_INTC, 0x01 << 3);

        g_ir_attr_data.check_rep_key = 0;
        if ((g_ir_attr_data.enable_key_up) &&
            (IR_BUF_LAST.ir_key_state != HI_UNF_KEY_STATUS_UP) &&
            (IR_BUF_LAST.ir_key_state != HI_UNF_KEY_STATUS_MAX)) {
            IR_BUF_HEAD = IR_BUF_LAST;
            IR_BUF_HEAD.ir_key_state = HI_UNF_KEY_STATUS_UP;
            IR_BUF_HEAD.ir_protocol = (hi_unf_ir_protocol)g_ir_attr_data.ir_code;
            g_ir_attr_data.head = INC_BUF(g_ir_attr_data.head, g_ir_attr_data.ir_key_buf_len);
            wake_up_interruptible(&(g_ir_attr_data.ir_key_wait_queue));
        }
        g_ir_attr_data.get_release_int = HI_TRUE;
    } else if (READ_REG(g_ir_reg_addr + IR_INTS) & IR_INT_RCV) {
        WRITE_REG(g_ir_reg_addr + IR_INTC, 0x01 << 0);

        if (g_ir_attr_data.enable_rep_key) {
            if ((g_ir_attr_data.check_rep_key) &&
                (IR_BUF_LAST.ir_key_data_h == READ_REG(g_ir_reg_addr + IR_DATAH)) &&
                (IR_BUF_LAST.ir_key_data_l == READ_REG(g_ir_reg_addr + IR_DATAL))) {
                HI_WARN_IR("repeart key [0x%.8x]-[0x%.8x] detective\n", READ_REG(g_ir_reg_addr + IR_DATAH),
                           READ_REG(g_ir_reg_addr + IR_DATAL));
            } else {
                /* repeat key check */
                rep_key_time_out_timer.expires = jiffies + g_ir_attr_data.rep_key_delay_time * HZ / 1000;
                mod_timer(&rep_key_time_out_timer, rep_key_time_out_timer.expires);

                g_ir_attr_data.check_rep_key = HI_TRUE;
                if ((IR_BUF_LAST.ir_key_data_h == READ_REG(g_ir_reg_addr + IR_DATAH)) &&
                    (IR_BUF_LAST.ir_key_data_l == READ_REG(g_ir_reg_addr + IR_DATAL)) &&
                    (IR_BUF_LAST.ir_key_state != HI_UNF_KEY_STATUS_UP) &&
                    (!g_ir_attr_data.get_release_int)) {
                    IR_BUF_HEAD.ir_key_data_h = READ_REG(g_ir_reg_addr + IR_DATAH);
                    IR_BUF_HEAD.ir_key_data_l = READ_REG(g_ir_reg_addr + IR_DATAL);
                    IR_BUF_HEAD.ir_key_state = HI_UNF_KEY_STATUS_HOLD;
                } else {
                    g_ir_attr_data.get_release_int = HI_FALSE;
                    IR_BUF_HEAD.ir_key_data_h = READ_REG(g_ir_reg_addr + IR_DATAH);
                    IR_BUF_HEAD.ir_key_data_l = READ_REG(g_ir_reg_addr + IR_DATAL);
                    IR_BUF_HEAD.ir_key_state = HI_UNF_KEY_STATUS_DOWN;
                    hi_drv_stat_event(HI_STAT_EVENT_KEY_IN, IR_BUF_HEAD.ir_key_data_l);
                }

                IR_BUF_HEAD.ir_protocol = (hi_unf_ir_protocol)g_ir_attr_data.ir_code;
                g_ir_attr_data.head = INC_BUF(g_ir_attr_data.head, g_ir_attr_data.ir_key_buf_len);

                wake_up_interruptible(&(g_ir_attr_data.ir_key_wait_queue));
            }
        } else {
            IR_BUF_HEAD.ir_key_data_h = READ_REG(g_ir_reg_addr + IR_DATAH);
            IR_BUF_HEAD.ir_key_data_l = READ_REG(g_ir_reg_addr + IR_DATAL);

            if (g_ir_attr_data.get_release_int) {
                g_ir_attr_data.get_release_int = HI_FALSE;
                IR_BUF_HEAD.ir_key_state = HI_UNF_KEY_STATUS_DOWN;
                IR_BUF_HEAD.ir_protocol = (hi_unf_ir_protocol)g_ir_attr_data.ir_code;
                hi_drv_stat_event(HI_STAT_EVENT_KEY_IN, IR_BUF_HEAD.ir_key_data_l);
                g_ir_attr_data.head = INC_BUF(g_ir_attr_data.head, g_ir_attr_data.ir_key_buf_len);
                wake_up_interruptible(&(g_ir_attr_data.ir_key_wait_queue));
            }
        }
    } else {
        HI_ERR_IR("ir logic error, then clear interrupts: int_mask=0x%.8x, int_state=0x%.8x.\n",
                  READ_REG(g_ir_reg_addr + IR_INTM), READ_REG(g_ir_reg_addr + IR_INTS));
        ir_clr_int();
    }

    return IRQ_HANDLED;
}

hi_void ir_reset(hi_void)
{
    WRITE_REG(g_ir_reg_addr + IR_INTM, 0x7000f);

    if (memset_s(g_ir_attr_data.ir_key_buf, sizeof(g_ir_attr_data.ir_key_buf), 0, sizeof(g_ir_attr_data.ir_key_buf))) {
        HI_ERR_IR("memset_s g_ir_attr_data.ir_key_buf failed\n");
        return;
    }

    g_ir_attr_data.head = 0;
    g_ir_attr_data.tail = 0;
    g_ir_attr_data.check_rep_key = 0;

    WRITE_REG(g_ir_reg_addr + IR_INTM, 0x70000);
    del_timer_sync(&rep_key_time_out_timer);
    return;
}

hi_void ir_set_enable(hi_u32 is_enable)
{
    if (is_enable == HI_TRUE) {
        WRITE_REG(g_ir_reg_addr + IR_ENABLE, HI_TRUE);
        ir_set_clk(HI_TRUE);
    } else {
        ir_reset();
        WRITE_REG(g_ir_reg_addr + IR_ENABLE, HI_FALSE);
        ir_set_clk(HI_FALSE);
    }

    return;
}

static hi_slong ir_ioctl(struct file *filp, hi_u32 cmd, hi_ulong arg)
{
    hi_slong ret = 0;

    ret = down_interruptible(&ir_mutex);
    if (ret) {
        HI_FATAL_IR("semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case CMD_IR_ENABLE_KEYUP:
            g_ir_attr_data.enable_key_up = (hi_bool)arg;
            break;

        case CMD_IR_ENABLE_REPKEY:
            g_ir_attr_data.enable_rep_key = (hi_bool)arg;
            break;

        case CMD_IR_SET_REPKEY_TIMEOUT:
            g_ir_attr_data.rep_key_delay_time = arg;
            break;

        case CMD_IR_SET_ENABLE:
            g_ir_attr_data.is_enable = (hi_bool)arg;
            if (g_ir_attr_data.is_enable) {
                ir_set_enable(arg);
                ir_config();
            } else {
                ir_set_enable(HI_FALSE);
            }

            break;

        case CMD_IR_RESET:
            ir_reset();
            break;

        case CMD_IR_SET_BLOCKTIME:
            g_ir_attr_data.ir_block_time = arg;
            break;

        case CMD_IR_SET_FORMAT:
            g_ir_attr_data.ir_code = (hi_unf_ir_code)arg;

            if (g_ir_attr_data.ir_code == HI_UNF_IR_CODE_NEC_SIMPLE) {
                g_ir_attr_data.ir_dev_para = ir_dev_paras[0];
            } else if (g_ir_attr_data.ir_code == HI_UNF_IR_CODE_TC9012) {
                g_ir_attr_data.ir_dev_para = ir_dev_paras[4];
            } else if (g_ir_attr_data.ir_code == HI_UNF_IR_CODE_NEC_FULL) {
                g_ir_attr_data.ir_dev_para = ir_dev_paras[5];
            } else {
                g_ir_attr_data.ir_dev_para = ir_dev_paras[12];
            }

            if (g_ir_attr_data.is_enable) {
                ir_config();
            }

            break;

        case CMD_IR_SET_FETCH_METHOD:
        case CMD_IR_SET_PROT_ENABLE:
        case CMD_IR_SET_PROT_DISABLE:
        case CMD_IR_GET_PROT_ENABLED:
            up(&ir_mutex);
            return HI_SUCCESS;

        default:
            HI_ERR_IR("error: inappropriate ioctl for device. cmd=%d\n", cmd);
            up(&ir_mutex);
            return -ENOTTY;
    }

    up(&ir_mutex);
    return HI_SUCCESS;
}

static ssize_t ir_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ir_key read_ir_key;
    hi_u32 read_len = 0;
    hi_s32 ret = 0;

    if (atomic_read(&g_ir_count) == 0) {
        HI_FATAL_IR("not open!\n");
        return -1;
    }

    ret = down_interruptible(&ir_mutex);
    if (ret) {
        HI_FATAL_IR("semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    while ((g_ir_attr_data.head) == (g_ir_attr_data.tail)) {
        if (((filp->f_flags & O_NONBLOCK) == O_NONBLOCK) || (g_ir_attr_data.ir_block_time == 0)) {
            up(&ir_mutex);
            HI_WARN_IR("the data buf is null.\n");
            return -EAGAIN;
        }

        if (g_ir_attr_data.ir_block_time == 0xffffffff) {
            ret = wait_event_interruptible(g_ir_attr_data.ir_key_wait_queue,
                                           (g_ir_attr_data.head != g_ir_attr_data.tail));
            if (ret < 0) {
                up(&ir_mutex);
                HI_ERR_IR("wait data err.\n");
                return -ERESTARTSYS;
            }
        } else {
            ret = wait_event_interruptible_timeout(g_ir_attr_data.ir_key_wait_queue,
                                                   (g_ir_attr_data.head != g_ir_attr_data.tail),
                                                   (hi_slong)(g_ir_attr_data.ir_block_time * HZ / 1000));
            if (ret < 0) {
                up(&ir_mutex);
                HI_ERR_IR("wait data err.\n");
                return -ERESTARTSYS;
            } else if (ret == 0) {
                up(&ir_mutex);
                HI_WARN_IR("wait data timeout.\n");
                return HI_ERR_IR_READ_FAILED;
            }
        }
    }

    while (((g_ir_attr_data.head) != (g_ir_attr_data.tail)) && ((read_len + sizeof(ir_key)) <= count)) {
        read_ir_key = IR_BUF_TAIL;
        g_ir_attr_data.tail = INC_BUF(g_ir_attr_data.tail, g_ir_attr_data.ir_key_buf_len);
        if (copy_to_user((buf + read_len), &read_ir_key, sizeof(ir_key))) {
            HI_FATAL_IR("copy data to user failed.\n");
            up(&ir_mutex);
            return HI_FAILURE;
        }

        read_len += sizeof(ir_key);
        if (read_ir_key.ir_key_state == HI_UNF_KEY_STATUS_DOWN) {
            hi_drv_stat_event(HI_STAT_EVENT_KEY_OUT, read_ir_key.ir_key_data_l);
        }
    }

    up(&ir_mutex);
    return read_len;
}

hi_u32 ir_select(struct file *filp, struct poll_table_struct *wait)
{
    hi_s32 ret;

    ret = down_interruptible(&ir_mutex);
    if (ret) {
        HI_FATAL_IR("semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    if ((g_ir_attr_data.head) != (g_ir_attr_data.tail)) {
        up(&ir_mutex);
        return POLLIN | POLLRDNORM;
    }

    poll_wait(filp, &(g_ir_attr_data.ir_key_wait_queue), wait);

    up(&ir_mutex);
    return 0;
}

hi_s32 ir_open(struct inode *inode, struct file *filp)
{
    hi_s32 ret;

    ret = down_interruptible(&ir_mutex);
    if (ret) {
        HI_FATAL_IR("semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    if (atomic_inc_return(&g_ir_count) == 1) {
        ir_set_clk(HI_TRUE);
        del_timer_sync(&rep_key_time_out_timer);

        g_ir_attr_data.ir_code = HI_UNF_IR_CODE_NEC_SIMPLE;
        g_ir_attr_data.ir_dev_para = ir_dev_paras[0];
        g_ir_attr_data.head = 0;
        g_ir_attr_data.tail = 0;
        g_ir_attr_data.ir_key_buf_len = IR_MAX_BUF;

        g_ir_attr_data.enable_key_up  = HI_TRUE;
        g_ir_attr_data.enable_rep_key = HI_TRUE;
        g_ir_attr_data.rep_key_delay_time = IR_DELAY_TIME;
        g_ir_attr_data.ir_block_time = DEF_BLOCK_TIME;

        g_ir_attr_data.check_rep_key = HI_FALSE;
        g_ir_attr_data.get_release_int = HI_TRUE;
        init_waitqueue_head(&g_ir_attr_data.ir_key_wait_queue);

        WRITE_REG(g_ir_reg_addr + IR_ENABLE, 0x101);
        ir_config();

        g_ir_attr_data.is_enable = HI_TRUE;

        ret = hi_drv_osal_request_irq(IR_IRQ_NO, (irq_handler_t)ir_isr, IRQF_SHARED, "ir_std", (hi_void *)&g_hi_ir_irq);
        if (ret != HI_SUCCESS) {
            HI_FATAL_IR("register IR INT failed 0x%x.\n", ret);
            atomic_dec(&g_ir_count);
            up(&ir_mutex);
            return HI_FAILURE;
        } else {
            if (hi_drv_sys_set_irq_affinity(HI_ID_IR, IR_IRQ_NO, "ir_std") != HI_SUCCESS) {
                HI_FATAL_IR("hi_drv_sys_set_irq_affinity failed.\n");
            }
        }
    }

    up(&ir_mutex);
    return 0;
}

hi_s32 ir_close(struct inode *inode, struct file *filp)
{
    hi_s32 ret;

    ret = down_interruptible(&ir_mutex);
    if (ret) {
        HI_FATAL_IR("semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    if (atomic_dec_and_test(&g_ir_count)) {
        hi_drv_osal_free_irq(IR_IRQ_NO, "ir_std", (hi_void *)&g_hi_ir_irq);
        del_timer_sync(&rep_key_time_out_timer);

        WRITE_REG(g_ir_reg_addr + IR_ENABLE, 0x00);
        ir_set_clk(HI_FALSE);
    }

    up(&ir_mutex);

    return 0;
}

static hi_s32 ir_pm_suspend(struct device *dev)
{
    /* stop ir */
    WRITE_REG(g_ir_reg_addr + IR_ENABLE, 0x00);

    /* clear key buffer */
    WRITE_REG(g_ir_reg_addr + IR_INTM, 0x7000f);
    g_ir_attr_data.head = 0;
    g_ir_attr_data.tail = 0;
    g_ir_attr_data.check_rep_key = 0;
    WRITE_REG(g_ir_reg_addr + IR_INTM, 0x70000);
    ir_set_clk(HI_FALSE);
    del_timer_sync(&rep_key_time_out_timer);

    HI_PRINT("IR suspend OK\n");
    return 0;
}

static hi_s32 ir_pm_resume(struct device *dev)
{
    if (atomic_read(&g_ir_count) != 0) {
        ir_set_clk(HI_TRUE);
        WRITE_REG(g_ir_reg_addr + IR_ENABLE, 0x01);
        ir_config();
    }

    HI_PRINT("IR resume OK\n");
    return 0;
}


static struct dev_pm_ops g_ir_pm_ops = {
    .suspend        = ir_pm_suspend,
    .suspend_late   = NULL,
    .resume_early   = NULL,
    .resume         = ir_pm_resume,
};

/*****************************************************************************
                                    power down
*****************************************************************************/

static struct file_operations g_ir_fops = {
    owner   : THIS_MODULE,
    open    : ir_open,
    unlocked_ioctl   : ir_ioctl,
#ifdef CONFIG_COMPAT
    compat_ioctl : ir_ioctl,
#endif
    poll    : ir_select,
    read    : ir_read,
    release : ir_close,
};

static struct class *g_ir_class = HI_NULL;
static dev_t g_ir_devno;
static struct cdev *g_ir_cdev = HI_NULL;
static struct device *g_ir_dev = HI_NULL;

static hi_s32 ir_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_ir_devno, 0, 1, "ir");
    if (ret != HI_SUCCESS) {
        HI_FATAL_IR("ir alloc chrdev region failed\n");

        return HI_FAILURE;
    }

    g_ir_cdev = cdev_alloc();
    if (IS_ERR(g_ir_cdev)) {
        HI_FATAL_IR("IR alloc cdev failed! \n");

        ret = HI_FAILURE;
        goto out0;
    }

    cdev_init(g_ir_cdev, &g_ir_fops);
    g_ir_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_ir_cdev, g_ir_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_FATAL_IR("IR add cdev failed, ret(%d).\n", ret);
        ret = HI_FAILURE;
        goto out1;
    }

    g_ir_class = class_create(THIS_MODULE, "ir_class");
    if (IS_ERR(g_ir_class)) {
        HI_FATAL_IR("ir create dev class failed! \n");

        ret = HI_FAILURE;
        goto out2;
    }

    g_ir_class->pm = &g_ir_pm_ops;

    g_ir_dev = device_create(g_ir_class, HI_NULL, g_ir_devno, HI_NULL, HI_DEV_IR_NAME);
    if (IS_ERR(g_ir_dev)) {
        HI_FATAL_IR("IR create dev failed! \n");
        ret = HI_FAILURE;

        goto out3;
    }

    return HI_SUCCESS;

out3:
    class_destroy(g_ir_class);
    g_ir_class = HI_NULL;
out2:
    cdev_del(g_ir_cdev);
out1:
    kfree(g_ir_cdev);
    g_ir_cdev = HI_NULL;
out0:
    unregister_chrdev_region(g_ir_devno, 1);

    return ret;
}

static hi_void ir_unregister_dev(hi_void)
{
    device_destroy(g_ir_class, g_ir_devno);
    g_ir_dev = HI_NULL;
    class_destroy(g_ir_class);
    g_ir_class = HI_NULL;
    cdev_del(g_ir_cdev);
    kfree(g_ir_cdev);
    g_ir_cdev = HI_NULL;
    unregister_chrdev_region(g_ir_devno, 1);

    return;
}

hi_s32 ir_drv_module_init(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_proc_item *proc_item = NULL;
#endif
    (hi_void)hi_drv_module_register(HI_ID_IR, "HI_IR", HI_NULL, HI_NULL);

    if (ir_register_dev() < 0) {
        HI_FATAL_IR("register IR failed.\n");
        return HI_FAILURE;
    }

#ifdef HI_PROC_SUPPORT
    proc_item = hi_drv_proc_add_module(HI_MOD_IR_NAME, HI_NULL, HI_NULL);
    if (!proc_item) {
        HI_FATAL_IR("add IR proc failed.\n");
        ir_unregister_dev();
        return HI_FAILURE;
    }

    proc_item->read  = ir_proc_read;
    proc_item->write = ir_proc_write;
#endif

    g_ir_reg_addr = (hi_u8 *)ioremap_nocache(IR_IO_BASE, IR_ADDR_SIZE);
    if (g_ir_reg_addr == HI_NULL) {
        HI_ERR_IR("ir std ioremap_nocache err! \n");
        return HI_FAILURE;
    }

#ifdef MODULE
    HI_PRINT("load hi_ir.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return 0;
}

hi_void ir_drv_module_exit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_drv_proc_remove_module(HI_MOD_IR_NAME);
#endif
    ir_unregister_dev();
    hi_drv_module_unregister(HI_ID_IR);

    iounmap(g_ir_reg_addr);
    return;
}

#ifdef MODULE
module_init(ir_drv_module_init);
module_exit(ir_drv_module_exit);
#endif
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");
