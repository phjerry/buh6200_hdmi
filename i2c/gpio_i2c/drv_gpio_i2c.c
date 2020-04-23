/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description:
 */
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/unistd.h>
#include <linux/compat.h>

#include "drv_i2c_ioctl.h"
#include "drv_gpio_ioctl.h"
#include "drv_gpioi2c_ext.h"
#include "drv_gpioi2c.h"
#include "hi_drv_gpioi2c.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_debug.h"
#include "hi_drv_mem.h"
#include "hi_drv_sys.h"
#include "hi_drv_gpio.h"
#include "hi_errno.h"
#include "hi_drv_osal.h"

static hi_u32 gpio_i2c_phy_addr[HI_GPIO_GROUP_NUM] = {HI_GPIO_0_ADDR, HI_GPIO_1_ADDR, HI_GPIO_2_ADDR, HI_GPIO_3_ADDR, HI_GPIO_4_ADDR, \
                                                          HI_GPIO_5_ADDR, HI_GPIO_6_ADDR, HI_GPIO_7_ADDR, HI_GPIO_8_ADDR, HI_GPIO_9_ADDR, \
                                                          HI_GPIO_10_ADDR, HI_GPIO_11_ADDR, HI_GPIO_12_ADDR, HI_GPIO_13_ADDR
                                                         };

static DEFINE_SPINLOCK(g_gpio_i2c_lock);

static hi_u8 *g_gpio_reg_addrs[HI_GPIO_GROUP_NUM];

i2c_gpio g_gpio_i2c[HI_I2C_MAX_NUM];

typedef struct {
    hi_u32             i2c_channel;
    hi_u32             gpio_i2c_scl; /* which GPIO will be defined I2CSCL */
    hi_u32             gpio_i2c_sda; /* which GPIO will be defined I2CSDA */
    hi_u8              *gpio_dir_clk; /* I2CSCL direction register */
    hi_u8              *gpio_dir_data; /* I2CSDA direction register */
    hi_u8              *gpio_i2c_sda_reg; /* SDA GPIO  relation data buffer register,indicate high or low level about input or output(gpio_dir_data confirm the direction) */
    hi_u8              *gpio_i2c_scl_reg; /* SCL GPIO indicate high or low level about input or output(gpio_dir_data confirm the direction) */
} i2c_data_gpio;

static i2c_data_gpio g_gpio_i2c_datas[HI_I2C_MAX_NUM];

int g_i2c_mode;

/*
 * 1: read, S:DevAddr:A:RegAddr:A:S:DevAddr:RecData:Stop
 *     write, S:DevAddr:A:RegAddr:A:WriteData:Stop
 * 2: read, S:DevAddr:A:RegAddr:A:Dealy:Stop:S:DevAddr:A:RecData:NoA:S
 *     write, S:DevAddr:A:RegAddr:A:WriteData:S
 */

#define HI_FATAL_GPIOI2C(fmt...) \
    HI_FATAL_PRINT(HI_ID_GPIO_I2C, fmt)

#define HI_ERR_GPIOI2C(fmt...) \
    HI_ERR_PRINT(HI_ID_GPIO_I2C, fmt)

#define HI_WARN_GPIOI2C(fmt...) \
    HI_WARN_PRINT(HI_ID_GPIO_I2C, fmt)

#define HI_INFO_GPIOI2C(fmt...) \
    HI_INFO_PRINT(HI_ID_GPIO_I2C, fmt)

#define HW_REG(reg) * ((volatile unsigned int *)(reg))
#define DELAY(i2c_num, us) time_delay_us(i2c_num, us)

struct semaphore g_gpio_i2c_sem;

static gpio_i2c_ext_func gpio_i2c_ext_funcs = {
    .pfn_gpio_i2c_ioctl             = hi_drv_gpio_i2c_ioctl,
#ifdef CONFIG_COMPAT
    .pfn_gpio_i2c_compat_ioctl      = hi_drv_gpio_i2c_compat_ioctl,
#endif
    .pfn_gpio_i2c_write             = hi_drv_gpio_i2c_write,
    .pfn_gpio_i2c_read              = hi_drv_gpio_i2c_read,
    .pfn_gpio_i2c_write_ext         = hi_drv_gpio_i2c_write_ext,
    .pfn_gpio_i2c_read_ext          = hi_drv_gpio_i2c_read_ext,
    .pfn_gpio_i2c_read_ext_directly = hi_drv_gpio_i2c_read_ext_directly,
    .pfn_gpio_i2c_write_ext_nostop  = hi_drv_gpio_write_ext_nostop,
    .pfn_gpio_i2c_sccb_read         = hi_drv_gpio_i2c_sccb_read,
    .pfn_gpio_i2c_create_channel    = hi_drv_gpio_i2c_create_gpio_i2c,
    .pfn_gpio_i2c_destroy_channel   = hi_drv_gpio_i2c_destroy_gpio_i2c,
    .pfn_gpio_i2c_is_used           = drv_gpio_i2c_is_used,
};

/*
 * I2C by GPIO simulated  clear 0 routine.
 *
 * @param which_line: GPIO control line
 *
 */
static hi_void i2c_clr(hi_u32 i2c_num, hi_u32 gpio_num)
{
    hi_u8 reg_value;

    if (gpio_num == g_gpio_i2c[i2c_num].scl_gpio_num) {
        reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_clk);
        reg_value |= g_gpio_i2c_datas[i2c_num].gpio_i2c_scl;
        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_clk) = reg_value;

        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_i2c_scl_reg) = 0;
        return;
    } else if (gpio_num == g_gpio_i2c[i2c_num].sda_gpio_num) {
        reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data);
        reg_value |= g_gpio_i2c_datas[i2c_num].gpio_i2c_sda;
        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data) = reg_value;

        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_i2c_sda_reg) = 0;
        return;
    } else {
        HI_ERR_GPIOI2C("i2c_clr Error input.\n");
        return;
    }
}

/*
 * I2C by GPIO simulated  set 1 routine.
 *
 * @param which_line: GPIO control line
 *
 */
static hi_void i2c_set(hi_u32 i2c_num, hi_u32 gpio_num)
{
    hi_u8 reg_value;

    if (gpio_num == g_gpio_i2c[i2c_num].scl_gpio_num) {
        reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_clk);
        reg_value |= g_gpio_i2c_datas[i2c_num].gpio_i2c_scl;
        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_clk) = reg_value;

        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_i2c_scl_reg) = g_gpio_i2c_datas[i2c_num].gpio_i2c_scl;
        return;
    } else if (gpio_num == g_gpio_i2c[i2c_num].sda_gpio_num) {
        reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data);
        reg_value |= g_gpio_i2c_datas[i2c_num].gpio_i2c_sda;
        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data) = reg_value;

        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_i2c_sda_reg) = g_gpio_i2c_datas[i2c_num].gpio_i2c_sda;
        return;
    } else {
        HI_ERR_GPIOI2C("i2c_set Error input.\n");
        return;
    }
}

/*
 *  delays for a specified number of micro seconds rountine.
 *
 *  @param usec: number of micro seconds to pause for
 *
 */
static hi_void time_delay_us(hi_u32 i2c_num, hi_u32 usec)
{
    hi_u32 i;

    for (i = 0; i < usec; i++) {
        udelay(1);
    }

    return;
}

/*
 * I2C by GPIO simulated  read data routine.
 *
 * @return value: a bit for read
 *
 */

static hi_u8 i2c_data_read(hi_u32 i2c_num)
{
    hi_u8 reg_value;

    reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data);
    reg_value &= (~g_gpio_i2c_datas[i2c_num].gpio_i2c_sda);
    HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data) = reg_value;

    reg_value = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_i2c_sda_reg);
    if ((reg_value & g_gpio_i2c_datas[i2c_num].gpio_i2c_sda) != 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * sends a start bit via I2C rountine.
 *
 */
static hi_void i2c_start_bit(hi_u32 i2c_num)
{
    DELAY(i2c_num, 1);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
    DELAY(i2c_num, 2);

    return;
}

/*
 * sends a stop bit via I2C rountine.
 *
 */
static hi_void i2c_stop_bit(hi_u32 i2c_num)
{
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
    DELAY(i2c_num, 1);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
    DELAY(i2c_num, 1);

    return;
}

/*
 * sends a character over I2C rountine.
 *
 * @param  c: character to send
 *
 */
static hi_void i2c_send_byte(hi_u32 i2c_num, hi_u8 c)
{
    hi_u32 i;
    hi_size_t irq_flags;

    spin_lock_irqsave(&g_gpio_i2c_lock, irq_flags);
    for (i = 0; i < 8; i++) {
        i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
        DELAY(i2c_num, 1);

        if (c & (1 << (7 - i))) {
            i2c_set(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
        } else {
            i2c_clr(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
        }

        DELAY(i2c_num, 1);
        i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
        DELAY(i2c_num, 1);
        DELAY(i2c_num, 1);
        i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    }

    DELAY(i2c_num, 1);
    spin_unlock_irqrestore(&g_gpio_i2c_lock, irq_flags);

    return;
}

/*  receives a character from I2C rountine.
 *
 *  @return value: character received
 *
 */
static hi_u8 i2c_receive_byte(hi_u32 i2c_num)
{
    hi_s32 j = 0;
    hi_u32 i;
    hi_u8 reg_value;
    hi_size_t irq_flags;

    spin_lock_irqsave(&g_gpio_i2c_lock, irq_flags);
    for (i = 0; i < 8; i++) {
        DELAY(i2c_num, 1);
        i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
        DELAY(i2c_num, 2);
        i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);

        reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data);
        reg_value &= (~g_gpio_i2c_datas[i2c_num].gpio_i2c_sda);
        HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data) = reg_value;
        DELAY(i2c_num, 1);

        if (i2c_data_read(i2c_num)) {
            j += (1 << (7 - i));
        }

        DELAY(i2c_num, 1);
        i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    }

    spin_unlock_irqrestore(&g_gpio_i2c_lock, irq_flags);

    DELAY(i2c_num, 1);

    return j;
}

/*  receives an acknowledge from I2C rountine.
 *
 *  @return value: 0--Ack received; 1--Nack received
 *
 */
static hi_s32 i2c_receive_ack(hi_u32 i2c_num)
{
    hi_s32 nack;
    hi_u8 reg_value;

    DELAY(i2c_num, 1);

    reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data);
    reg_value &= (~g_gpio_i2c_datas[i2c_num].gpio_i2c_sda);
    HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data) = reg_value;

    DELAY(i2c_num, 1);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);

    nack = i2c_data_read(i2c_num);

    DELAY(i2c_num, 1);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 2);

    if (nack == 0) {
        return HI_SUCCESS;
    } else {
        HI_ERR_GPIOI2C("receive Err ack = 1 \n");
        return HI_FAILURE;
    }
}

/*
 * sends an acknowledge over I2C rountine.
 *
 */
static hi_void i2c_send_ack(hi_u32 i2c_num)
{
    hi_u8 reg_value;

    DELAY(i2c_num, 1);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
    DELAY(i2c_num, 1);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);

    reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data);
    reg_value &= (~g_gpio_i2c_datas[i2c_num].gpio_i2c_sda);
    HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data) = reg_value;

    DELAY(i2c_num, 2);

    return;
}

static hi_void i2c_send_noack(hi_u32 i2c_num)
{
    hi_u8 reg_value;

    DELAY(i2c_num, 1);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
    DELAY(i2c_num, 1);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 2);
    i2c_clr(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    DELAY(i2c_num, 1);

    reg_value  = HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data);
    reg_value &= (~g_gpio_i2c_datas[i2c_num].gpio_i2c_sda);
    HW_REG(g_gpio_i2c_datas[i2c_num].gpio_dir_data) = reg_value;

    DELAY(i2c_num, 1);
}

/*
 *  read data from the I2C bus by GPIO simulated of a device rountine.
 *
 *  @param  dev_addr:  address of the device
 *  @param  reg_addr: address of register within device
 *
 *  @return value: data from the device readed
 *
 */

hi_s32 hi_drv_gpio_i2c_read(hi_u32 i2c_num, hi_u8 dev_addr, hi_u8 reg_addr, hi_u8 *buf)
{
    if (down_interruptible(&g_gpio_i2c_sem)) {
        HI_ERR_GPIOI2C("Semaphore lock is  error. \n");
        return HI_FAILURE;;
    }

    if ((i2c_num >= HI_I2C_MAX_NUM) || (buf == HI_NULL) ||
            (g_gpio_i2c[i2c_num].is_used == HI_FALSE)) {
        up(&g_gpio_i2c_sem);
        return HI_FAILURE;
    }

    i2c_start_bit(i2c_num);

    i2c_send_byte(i2c_num, (unsigned char)(dev_addr));
    i2c_receive_ack(i2c_num);

    i2c_send_byte(i2c_num, reg_addr);
    i2c_receive_ack(i2c_num);

    if (g_i2c_mode == 2) {
        i2c_stop_bit(i2c_num);
        DELAY(i2c_num, 5);
    }

    i2c_start_bit(i2c_num);

    i2c_send_byte(i2c_num, (unsigned char)(dev_addr) | 1);
    i2c_receive_ack(i2c_num);

    *buf = i2c_receive_byte(i2c_num);
    if (g_i2c_mode == 2) {
        i2c_send_noack(i2c_num);
    }

    i2c_stop_bit(i2c_num);

    up(&g_gpio_i2c_sem);
    HI_INFO_GPIOI2C("\t i2c_num=%d, DevAddr=0x%x, RegAddr=0x%x, data0=0x%x\n",
                    i2c_num, dev_addr, reg_addr, *buf);

    return HI_SUCCESS;
}

static hi_s32 drv_gpio_i2c_read_data(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset,
                                     hi_bool enable_send_reg_addr, hi_u8 *buf, hi_u32 buf_len)
{
    hi_u32 i = 0;
    hi_u32 ret; /* TC MODIFY */
    hi_u8 reval = 0;
    hi_u8 tmp_reg_addr;

    if (buf == HI_NULL) {
        HI_ERR_GPIOI2C("null pointer\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&g_gpio_i2c_sem)) {
        HI_ERR_GPIOI2C("Semaphore lock is   error. \n");
        return HI_FAILURE;
    }

    if ((i2c_num >= HI_I2C_MAX_NUM) || (g_gpio_i2c[i2c_num].is_used == HI_FALSE)) {
        HI_ERR_GPIOI2C("the i2c_num haven't requested ,can't read operation.\n");
        up(&g_gpio_i2c_sem);
        return HI_FAILURE;
    }

    if (enable_send_reg_addr) {
        /* send start condition */
        i2c_start_bit(i2c_num);

        /* send slave device address */
        i2c_send_byte(i2c_num, (hi_u8)(dev_addr));

        /* TC MODIFY START */
        ret = i2c_receive_ack(i2c_num);
        if (ret != HI_SUCCESS) {
            HI_ERR_GPIOI2C("i2c not receive ack!\n");
            up(&g_gpio_i2c_sem);
            return HI_FAILURE;
        }

        for (i = 0; i < (reg_addr_offset); i++) {
            tmp_reg_addr = reg_addr >> ((reg_addr_offset - i - 1) * 8);

            i2c_send_byte(i2c_num, tmp_reg_addr);
            i2c_receive_ack(i2c_num);
        }
    }

    /* send start condition */
    i2c_start_bit(i2c_num);

    /* send slave device address */
    i2c_send_byte(i2c_num, (hi_u8)(dev_addr) | 1);
    i2c_receive_ack(i2c_num);

    /* receive data */
    for (i = 0; i < buf_len; i++) {
        reval = i2c_receive_byte(i2c_num);

        *(buf + i) = reval;

        if (i == buf_len - 1) {
            i2c_send_noack(i2c_num);
        } else {
            i2c_send_ack(i2c_num);
        }
    }

    /* send stop condition */
    i2c_stop_bit(i2c_num);
    up(&g_gpio_i2c_sem);

    HI_INFO_GPIOI2C("\t i2c_num=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, Len=%d, data0=0x%x\n", i2c_num, dev_addr,
                    reg_addr, reg_addr_offset, buf_len, *buf);

    return HI_SUCCESS;
}

static hi_s32 drv_gpio_i2c_read_data_si_labs(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 seg_addr_offset,
        hi_bool will_send_reg_addr, hi_u8 *buf, hi_u32 buf_len)
{
    hi_u32 i = 0;
    hi_u8 reval = 0;

    if (buf == HI_NULL) {
        HI_ERR_GPIOI2C("null pointer\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&g_gpio_i2c_sem)) {
        HI_ERR_GPIOI2C("Semaphore lock is   error. \n");
        return HI_FAILURE;
    }

    if ((i2c_num >= HI_I2C_MAX_NUM) || (g_gpio_i2c[i2c_num].is_used == HI_FALSE)) {
        HI_ERR_GPIOI2C("the i2c_num haven't requested ,can't read operation.\n");
        up(&g_gpio_i2c_sem);
        return HI_FAILURE;
    }

    if (will_send_reg_addr) {
        /* send start condition */
        i2c_start_bit(i2c_num);
        /* send slave device address */
        i2c_send_byte(i2c_num, (hi_u8)(dev_addr) | 1);
        i2c_receive_ack(i2c_num);

        /* receive data */
        for (i = 0; i < 1; i++) {
            reval = i2c_receive_byte(i2c_num);

            *(buf + i) = reval;

            if (i == buf_len - 1) {
                i2c_send_noack(i2c_num);
            } else {
                i2c_send_ack(i2c_num);
            }
        }

    }

    /* send stop condition */
    i2c_stop_bit(i2c_num);

    /* send start condition */
    i2c_start_bit(i2c_num);

    /* send slave device address */
    i2c_send_byte(i2c_num, (hi_u8)(dev_addr) | 1);
    i2c_receive_ack(i2c_num);

    /* receive data */
    for (i = 0; i < buf_len; i++) {
        reval = i2c_receive_byte(i2c_num);

        *(buf + i) = reval;

        if (i == buf_len - 1) {
            i2c_send_noack(i2c_num);
        } else {
            i2c_send_ack(i2c_num);
        }
    }

    /* send stop condition */
    i2c_stop_bit(i2c_num);
    up(&g_gpio_i2c_sem);

    HI_INFO_GPIOI2C("\t i2c_num=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, Len=%d, data0=0x%x\n", i2c_num, dev_addr,
                    reg_addr, seg_addr_offset, buf_len, *buf);

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_i2c_read_ext(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                                hi_u32 buf_len)
{
    if ((reg_addr_offset > 4) || (buf_len > HI_I2C_MAX_LENGTH) || (buf == HI_NULL)) {
        HI_ERR_GPIOI2C("incalid para\n");
        return HI_FAILURE;
    }

    return drv_gpio_i2c_read_data(i2c_num, dev_addr, reg_addr, reg_addr_offset, HI_TRUE, buf, buf_len);
}

hi_s32 hi_drv_gpio_i2c_read_si_labs(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                                    hi_u32 buf_len)
{
    if ((reg_addr_offset > 4) || (buf_len > HI_I2C_MAX_LENGTH) || (HI_NULL == buf)) {
        HI_ERR_GPIOI2C("incalid para\n");
        return HI_FAILURE;
    }

    return drv_gpio_i2c_read_data_si_labs(i2c_num, dev_addr, reg_addr, reg_addr_offset, HI_TRUE, buf, buf_len);
}

hi_s32 hi_drv_gpio_i2c_read_ext_directly(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
        hi_u32 buf_len)
{
    if ((reg_addr_offset > 4) || (buf_len > HI_I2C_MAX_LENGTH) || (buf == HI_NULL)) {
        HI_ERR_GPIOI2C("incalid para\n");
        return HI_FAILURE;
    }

    return drv_gpio_i2c_read_data(i2c_num, dev_addr, reg_addr, reg_addr_offset, HI_FALSE, buf, buf_len);
}

/*
 *  writes data to a device on the I2C bus rountine.
 *
 *  @param  dev_addr:  address of the device
 *  @param  address: address of register within device
 *  @param  data:   data for write to device
 *
 */

hi_s32 hi_drv_gpio_i2c_write(hi_u32 i2c_num, hi_u8 dev_addr, hi_u8 reg_addr, hi_u8 data)
{
    if (down_interruptible(&g_gpio_i2c_sem)) {
        HI_ERR_GPIOI2C("Semaphore lock is  error. \n");
        return HI_FAILURE;;
    }

    if ((i2c_num >= HI_I2C_MAX_NUM) || (g_gpio_i2c[i2c_num].is_used == HI_FALSE)) {
        up(&g_gpio_i2c_sem);
        return HI_FAILURE;
    }

    HI_INFO_GPIOI2C("\t i2c_num=%d, DevAddr=0x%x, RegAddr=0x%x, data0=0x%x\n",
                    i2c_num, dev_addr, reg_addr, data);

    i2c_start_bit(i2c_num);

    i2c_send_byte(i2c_num, (hi_u8)(dev_addr));
    i2c_receive_ack(i2c_num);

    i2c_send_byte(i2c_num, reg_addr);
    i2c_receive_ack(i2c_num);

    i2c_send_byte(i2c_num, data);
    if (g_i2c_mode == 2) {
        i2c_receive_ack(i2c_num);
        DELAY(i2c_num, 10);
    }

    i2c_stop_bit(i2c_num);
    up(&g_gpio_i2c_sem);

    return HI_SUCCESS;
}

static hi_s32  drv_gpio_i2c_write_data(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                                       hi_u32 buf_len, hi_bool enable_send_stop)
{
    hi_u32 i;
    hi_u8 tmp_reg_addr;

    if (buf == HI_NULL) {
        HI_ERR_GPIOI2C("null pointer\n");
        return HI_FAILURE;
    }

    if (down_interruptible(&g_gpio_i2c_sem)) {
        HI_ERR_GPIOI2C("Semaphore lock is  error. \n");
        return HI_FAILURE;
    }

    if ((i2c_num >= HI_I2C_MAX_NUM) || (g_gpio_i2c[i2c_num].is_used == HI_FALSE)) {
        HI_ERR_GPIOI2C("the i2c_num haven't requested ,can't write operation.\n");
        up(&g_gpio_i2c_sem);
        return HI_FAILURE;
    }

    HI_INFO_GPIOI2C("i2c_num=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, Len=%d, data0=0x%x\n", i2c_num, dev_addr, reg_addr,
                    reg_addr_offset, buf_len, buf[0]);

    i2c_start_bit(i2c_num);

    i2c_send_byte(i2c_num, (hi_u8)(dev_addr));
    i2c_receive_ack(i2c_num);

    for (i = 0; i < (reg_addr_offset); i++) {
        tmp_reg_addr = reg_addr >> ((reg_addr_offset - i - 1) * 8);

        i2c_send_byte(i2c_num, (hi_u8)tmp_reg_addr);
        i2c_receive_ack(i2c_num);
    }

    for (i = 0; i < buf_len; i++) {
        i2c_send_byte(i2c_num, buf[i]);
        i2c_receive_ack(i2c_num);
    }

    if (enable_send_stop) {
        i2c_stop_bit(i2c_num);
    }

    up(&g_gpio_i2c_sem);

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_i2c_write_ext(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                                 hi_u32 buf_len)
{
    return drv_gpio_i2c_write_data(i2c_num, dev_addr, reg_addr, reg_addr_offset, buf, buf_len, HI_TRUE);
}

hi_s32 hi_drv_gpio_write_ext_nostop(hi_u32 i2c_num, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                                    hi_u32 buf_len)
{
    return drv_gpio_i2c_write_data(i2c_num, dev_addr, reg_addr, reg_addr_offset, buf, buf_len, HI_FALSE);
}

/*
 *  read data from the I2C bus by GPIO simulated of a digital camera device rountine.
 *
 *  @param  dev_addr:  address of the device
 *  @param  reg_addr: address of register within device
 *
 */

hi_s32 hi_drv_gpio_i2c_sccb_read(hi_u32 i2c_num, hi_u8 dev_addr, hi_u8 reg_addr, hi_u8 *buf)
{
    if (down_interruptible(&g_gpio_i2c_sem)) {
        HI_ERR_GPIOI2C("Semaphore lock is  error. \n");
        return HI_FAILURE;;
    }

    if ((i2c_num >= HI_I2C_MAX_NUM) || (buf == HI_NULL) ||
            (g_gpio_i2c[i2c_num].is_used == HI_FALSE)) {
        up(&g_gpio_i2c_sem);
        return HI_FAILURE;
    }

    i2c_start_bit(i2c_num);
    i2c_send_byte(i2c_num, (hi_u8)(dev_addr));
    i2c_receive_ack(i2c_num);
    i2c_send_byte(i2c_num, reg_addr);
    i2c_receive_ack(i2c_num);
    i2c_stop_bit(i2c_num);
    i2c_start_bit(i2c_num);
    i2c_send_byte(i2c_num, (hi_u8)(dev_addr) | 1);
    i2c_receive_ack(i2c_num);
    *buf = i2c_receive_byte(i2c_num);
    i2c_send_ack(i2c_num);
    i2c_stop_bit(i2c_num);
    up(&g_gpio_i2c_sem);

    return HI_SUCCESS;
}

hi_s32 drv_gpio_i2c_is_used(hi_u32 i2c_num, hi_bool *is_used)
{
    if ((i2c_num > HI_I2C_MAX_NUM_USER) || (is_used == HI_NULL)) {
        return HI_FAILURE;
    }

    *is_used = g_gpio_i2c[i2c_num].is_used;
    return HI_SUCCESS;
}

hi_s32 drv_gpio_i2c_config(hi_u32 i2c_num, hi_u32 scl_gpio_num, hi_u32 sda_gpio_num, hi_u32 clock_bit, hi_u32 data_bit)
{
    hi_u8 *gpio_base_clock;
    hi_u8 *gpio_base_data;

    gpio_base_clock = g_gpio_reg_addrs[scl_gpio_num];
    gpio_base_data = g_gpio_reg_addrs[sda_gpio_num];

    g_gpio_i2c_datas[i2c_num].i2c_channel = i2c_num;
    g_gpio_i2c_datas[i2c_num].gpio_i2c_scl = (1 << clock_bit);
    g_gpio_i2c_datas[i2c_num].gpio_i2c_sda = (1 << data_bit);

    g_gpio_i2c_datas[i2c_num].gpio_dir_data = gpio_base_data + HI_GPIO_DIR_REG;
    g_gpio_i2c_datas[i2c_num].gpio_dir_clk = gpio_base_clock + HI_GPIO_DIR_REG;
    g_gpio_i2c_datas[i2c_num].gpio_i2c_sda_reg = (gpio_base_data + ((g_gpio_i2c_datas[i2c_num].gpio_i2c_sda) << 2));
    g_gpio_i2c_datas[i2c_num].gpio_i2c_scl_reg = (gpio_base_clock + ((g_gpio_i2c_datas[i2c_num].gpio_i2c_scl) << 2));

    HI_INFO_GPIOI2C("drv_gpio_i2c_config OK(i2c_num=%d, SCL=gpio%d_%d, SDA=gpio%d_%d, g_i2c_mode=%d).\n", i2c_num,
                    scl_gpio_num, clock_bit, sda_gpio_num, data_bit, g_i2c_mode);

    g_gpio_i2c[i2c_num].i2c_num = i2c_num;
    g_gpio_i2c[i2c_num].scl_gpio_num = scl_gpio_num * HI_GPIO_BIT_NUM + clock_bit;
    g_gpio_i2c[i2c_num].sda_gpio_num = sda_gpio_num * HI_GPIO_BIT_NUM + data_bit;
    g_gpio_i2c[i2c_num].is_used = HI_TRUE;
    g_gpio_i2c[i2c_num].count = 1;

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_i2c_create_gpio_i2c(hi_u32 *i2c_num, hi_u32 scl_gpio_num, hi_u32 sda_gpio_num)
{
    hi_s32 ret;
    hi_u32 i, valid_i2c_num;
    hi_u32 tmp_gpio_clk_num;
    hi_u32 tmp_gpio_sda_num;
    hi_u32 which_gpio_clk;
    hi_u32 which_gpio_data;
    hi_u32 clk_bit;
    hi_u32 data_bit;

    if (i2c_num == HI_NULL) {
        HI_ERR_GPIOI2C("null pointer\n");
        return HI_FAILURE;
    }

    which_gpio_clk = scl_gpio_num / HI_GPIO_BIT_NUM;
    clk_bit = scl_gpio_num % HI_GPIO_BIT_NUM;
    which_gpio_data = sda_gpio_num / HI_GPIO_BIT_NUM;
    data_bit = sda_gpio_num % HI_GPIO_BIT_NUM;

    if ((which_gpio_clk >= HI_GPIO_GROUP_NUM)
            || (which_gpio_data >= HI_GPIO_GROUP_NUM)
            || (scl_gpio_num == sda_gpio_num)) {
        HI_ERR_GPIOI2C("error: GPIO NO. %d and NO. %d is invalid!\n", scl_gpio_num, sda_gpio_num);
        return HI_ERR_GPIO_INVALID_PARA;
    }

    /* whether the gpio have used in other i2c_num */
    for (i = HI_STD_I2C_NUM; i < HI_I2C_MAX_NUM; i++) {
        if (g_gpio_i2c[i].is_used) {
            tmp_gpio_clk_num  = g_gpio_i2c[i].scl_gpio_num;
            tmp_gpio_sda_num = g_gpio_i2c[i].sda_gpio_num;

            if ((tmp_gpio_clk_num == scl_gpio_num) && (tmp_gpio_sda_num == sda_gpio_num)) {
                g_gpio_i2c[i].count++;
                *i2c_num = g_gpio_i2c[i].i2c_num;
                HI_INFO_GPIOI2C("SCLGpioNo.%d and SDAGpioNo.%d have been created for I2c channel[%d] !\n",
                                scl_gpio_num, sda_gpio_num, *i2c_num);
                return HI_SUCCESS;
            }

            if ((scl_gpio_num == tmp_gpio_clk_num) || (sda_gpio_num == tmp_gpio_clk_num)) {
                HI_ERR_GPIOI2C("GPIO NO. %d is used to GpioClock!\n", tmp_gpio_clk_num);
                return HI_ERR_GPIO_INVALID_PARA;
            }

            if ((scl_gpio_num == tmp_gpio_sda_num) || (sda_gpio_num == tmp_gpio_sda_num)) {
                HI_ERR_GPIOI2C("GPIO NO. %d is used to GpioData!\n", tmp_gpio_sda_num);
                return HI_ERR_GPIO_INVALID_PARA;
            }
        }
    }

    /* find a idle i2c channel */
    for (i = HI_STD_I2C_NUM; i < HI_I2C_MAX_NUM; i++) {
        if (g_gpio_i2c[i].is_used == HI_FALSE) {
            valid_i2c_num = i;
            break;
        }
    }

    if (i >= HI_I2C_MAX_NUM) {
        HI_ERR_GPIOI2C("i2c channel all  have used ,request i2c channel fail  !\n");
        return HI_FAILURE;
    }

    /* the gpio can be use */
    ret = drv_gpio_i2c_config(valid_i2c_num, which_gpio_clk, which_gpio_data, clk_bit, data_bit);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIOI2C("config gpio i2c failure !\n");
        return HI_FAILURE;
    }

    *i2c_num = valid_i2c_num;

    return HI_SUCCESS;
}

hi_s32 hi_drv_gpio_i2c_destroy_gpio_i2c(hi_u32 i2c_num)
{
    if (i2c_num < HI_STD_I2C_NUM || i2c_num >= HI_I2C_MAX_NUM) {
        return HI_ERR_I2C_MALLOC_ERR;
    }

    if (g_gpio_i2c[i2c_num].is_used == HI_FALSE) {
        HI_ERR_GPIOI2C("i2c_num = %d is not used!\n", i2c_num);
        return HI_FAILURE;
    }

    if ((--g_gpio_i2c[i2c_num].count) == 0) {
        g_gpio_i2c[i2c_num].is_used = HI_FALSE;
    }

    HI_INFO_GPIOI2C("gpio_i2c_destroy OK(i2c_num=%d, SCL=gpio No.%d, SDA=gpio No.%d).\n", i2c_num,
                    g_gpio_i2c[i2c_num].scl_gpio_num, g_gpio_i2c[i2c_num].sda_gpio_num);

    return HI_SUCCESS;
}

/*************************************************************************/
DEFINE_SEMAPHORE(g_gpio_i2c_mutex);

hi_slong hi_drv_gpio_i2c_ioctl(hi_void *file, hi_u32 cmd, hi_size_t arg)
{
    hi_slong    ret;
    hi_u8  *buf = HI_NULL;
    i2c_data data = {0};
    i2c_gpio g_gpio_i2c = {0};
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if (argp == HI_NULL) {
        HI_ERR_GPIOI2C("invalid para\n");
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_gpio_i2c_mutex);
    if (ret) {
        HI_ERR_GPIOI2C("lock g_I2cMutex error.\n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case CMD_I2C_WRITE:
            if (copy_from_user(&data, argp, sizeof(i2c_data))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            if (data.buf_len > HI_I2C_MAX_LENGTH  || data.buf_len == 0 || data.i2c_reg_count > 4) {
                HI_ERR_GPIOI2C("invalid para\n");
                ret = HI_ERR_I2C_INVALID_PARA;
                break;
            }

            if (data.buf == HI_NULL) {
                HI_ERR_GPIOI2C("data.buf is null pointer\n");
                ret = HI_ERR_I2C_INVALID_PARA;
                break;
            }

            buf = HI_KMALLOC(HI_ID_GPIO_I2C, data.buf_len, GFP_KERNEL);
            if (!buf) {
                HI_ERR_GPIOI2C("i2c kmalloc fail!\n");
                ret = HI_ERR_I2C_MALLOC_ERR;
                break;
            }

            if (memset_s(buf, data.buf_len, 0, data.buf_len)) {
                HI_ERR_GPIOI2C("memset_s buf failed\n");
                HI_KFREE(HI_ID_GPIO_I2C, buf);
                buf = HI_NULL;
                ret = HI_FAILURE;
                break;
            }

            if (copy_from_user(buf, data.buf, data.buf_len)) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                HI_KFREE(HI_ID_GPIO_I2C, buf);
                buf = HI_NULL;
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            ret = hi_drv_gpio_i2c_write_ext(data.i2c_num, data.i2c_dev_addr, data.i2c_reg_addr, data.i2c_reg_count,
                                            buf, data.buf_len);
            HI_KFREE(HI_ID_GPIO_I2C, buf);
            buf = HI_NULL;
            break;

        case CMD_I2C_READ:
            if (copy_from_user(&data, argp, sizeof(i2c_data))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            if (data.buf_len > HI_I2C_MAX_LENGTH || data.buf_len == 0 || data.i2c_reg_count > 4) {
                HI_ERR_GPIOI2C("invalid para\n");
                ret = HI_ERR_I2C_INVALID_PARA;
                break;
            }

            if (data.buf == HI_NULL) {
                HI_ERR_GPIOI2C("data.buf is null pointer\n");
                ret = HI_ERR_I2C_INVALID_PARA;
                break;
            }

            buf = HI_KMALLOC(HI_ID_GPIO_I2C, data.buf_len, GFP_KERNEL);
            if (!buf) {
                HI_ERR_GPIOI2C("i2c kmalloc fail!\n");
                ret = HI_ERR_I2C_MALLOC_ERR;
                break;
            }

            if (memset_s(buf, data.buf_len, 0, data.buf_len)) {
                HI_ERR_GPIOI2C("memset_s buf failed\n");
                HI_KFREE(HI_ID_GPIO_I2C, buf);
                ret = HI_FAILURE;
                break;
            }

            ret = hi_drv_gpio_i2c_read_ext(data.i2c_num, data.i2c_dev_addr, data.i2c_reg_addr, data.i2c_reg_count, buf,
                                           data.buf_len);
            if (ret == HI_SUCCESS) {
                if (copy_to_user(data.buf, buf, data.buf_len)) {
                    HI_ERR_GPIOI2C("copy data to user fail!\n");
                    ret = HI_ERR_I2C_COPY_DATA_ERR;
                }
            }

            HI_KFREE(HI_ID_GPIO_I2C, buf);
            buf = HI_NULL;
            break;

        case CMD_I2C_CONFIG:
            if (copy_from_user(&g_gpio_i2c, argp, sizeof(i2c_gpio))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_FAILURE;
                break;
            }

            ret = hi_drv_gpio_i2c_create_gpio_i2c(&(g_gpio_i2c.i2c_num), g_gpio_i2c.scl_gpio_num, g_gpio_i2c.sda_gpio_num);
            if (ret == HI_SUCCESS) {
                if (copy_to_user(argp, (hi_void *)&g_gpio_i2c, sizeof(i2c_gpio))) {
                    HI_ERR_GPIOI2C("copy data to user fail!\n");
                    ret = HI_ERR_I2C_COPY_DATA_ERR;
                }
            }

            break;

        case CMD_I2C_DESTROY:
            if (copy_from_user(&g_gpio_i2c, argp, sizeof(i2c_gpio))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_FAILURE;
                break;
            }

            ret = hi_drv_gpio_i2c_destroy_gpio_i2c(g_gpio_i2c.i2c_num);

            break;

        default:
            up(&g_gpio_i2c_mutex);
            return -ENOIOCTLCMD;
    }

    up(&g_gpio_i2c_mutex);
    return ret;
}

#ifdef CONFIG_COMPAT
hi_slong hi_drv_gpio_i2c_compat_ioctl(hi_void *file, hi_u32 cmd, hi_size_t arg)
{
    hi_slong    ret;
    hi_u8  *buf = HI_NULL;
    i2c_data_compat data = {0};
    i2c_gpio g_gpio_i2c = {0};
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if (argp == HI_NULL) {
        HI_ERR_GPIOI2C("invalid para\n");
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_gpio_i2c_mutex);
    if (ret) {
        HI_ERR_GPIOI2C("lock g_I2cMutex error.\n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case CMD_I2C_WRITE:
            if (copy_from_user(&data, argp, sizeof(i2c_data_compat))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            if (data.buf_len > HI_I2C_MAX_LENGTH || data.buf_len == 0 || data.i2c_reg_count > 4) {
                HI_ERR_GPIOI2C("invalid para\n");
                ret = HI_ERR_I2C_INVALID_PARA;
                break;
            }

            buf = HI_KMALLOC(HI_ID_GPIO_I2C, data.buf_len, GFP_KERNEL);
            if (!buf) {
                HI_ERR_GPIOI2C("i2c kmalloc fail!\n");
                ret = HI_ERR_I2C_MALLOC_ERR;
                break;
            }

            if (memset_s(buf, data.buf_len, 0, data.buf_len)) {
                HI_ERR_GPIOI2C("memset_s buf failed\n");
                HI_KFREE(HI_ID_GPIO_I2C, buf);
                buf = HI_NULL;
                ret = HI_FAILURE;
                break;
            }

            if (copy_from_user(buf, (hi_u8 *)compat_ptr(data.buf_addr), data.buf_len)) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                HI_KFREE(HI_ID_GPIO_I2C, buf);
                buf = HI_NULL;
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            ret = hi_drv_gpio_i2c_write_ext(data.i2c_num, data.i2c_dev_addr, data.i2c_reg_addr, data.i2c_reg_count,
                                            buf, data.buf_len);
            HI_KFREE(HI_ID_GPIO_I2C, buf);
            buf = HI_NULL;
            break;

        case CMD_I2C_READ:
            if (copy_from_user(&data, argp, sizeof(i2c_data_compat))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                break;
            }

            if (data.buf_len > HI_I2C_MAX_LENGTH || data.buf_len == 0 || data.i2c_reg_count > 4) {
                HI_ERR_GPIOI2C("invalid para\n");
                ret = HI_ERR_I2C_INVALID_PARA;
                break;
            }

            buf = HI_KMALLOC(HI_ID_GPIO_I2C, data.buf_len, GFP_KERNEL);
            if (!buf) {
                HI_ERR_GPIOI2C("i2c kmalloc fail!\n");
                ret = HI_ERR_I2C_MALLOC_ERR;
                break;
            }

            if (memset_s(buf, data.buf_len, 0, data.buf_len)) {
                HI_ERR_GPIOI2C("memset_s buf failed\n");
                HI_KFREE(HI_ID_GPIO_I2C, buf);
                buf = HI_NULL;
                ret = HI_FAILURE;
                break;
            }
            ret = hi_drv_gpio_i2c_read_ext(data.i2c_num, data.i2c_dev_addr, data.i2c_reg_addr, data.i2c_reg_count, buf,
                                           data.buf_len);
            if (ret == HI_SUCCESS) {
                if (copy_to_user((hi_u8 *)compat_ptr(data.buf_addr), buf, data.buf_len)) {
                    HI_ERR_GPIOI2C("copy data to user fail!\n");
                    ret = HI_ERR_I2C_COPY_DATA_ERR;
                }
            }

            HI_KFREE(HI_ID_GPIO_I2C, buf);
            buf = HI_NULL;
            break;

        case CMD_I2C_CONFIG:
            if (copy_from_user(&g_gpio_i2c, argp, sizeof(i2c_gpio))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_FAILURE;
                break;
            }

            ret = hi_drv_gpio_i2c_create_gpio_i2c(&(g_gpio_i2c.i2c_num), g_gpio_i2c.scl_gpio_num, g_gpio_i2c.sda_gpio_num);
            if (ret == HI_SUCCESS) {
                if (copy_to_user(argp, (hi_void *)&g_gpio_i2c, sizeof(i2c_gpio))) {
                    HI_ERR_GPIOI2C("copy data to user fail!\n");
                    ret = HI_ERR_I2C_COPY_DATA_ERR;
                }
            }

            break;

        case CMD_I2C_DESTROY:
            if (copy_from_user(&g_gpio_i2c, argp, sizeof(i2c_gpio))) {
                HI_ERR_GPIOI2C("copy data from user fail!\n");
                ret = HI_FAILURE;
                break;
            }

            ret = hi_drv_gpio_i2c_destroy_gpio_i2c(g_gpio_i2c.i2c_num);

            break;

        default:
            up(&g_gpio_i2c_mutex);
            return -ENOIOCTLCMD;
    }

    up(&g_gpio_i2c_mutex);
    return ret;
}
#endif

#ifdef GPIO_I2C_PM
hi_s32 gpio_i2c_suspend(struct device *dev)
{
    HI_PRINT("GPIO_I2C suspend OK\n");
    return HI_SUCCESS;
}

hi_s32 gpio_i2c_resume(struct device *dev)
{
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].scl_gpio_num);
    i2c_set(i2c_num, g_gpio_i2c[i2c_num].sda_gpio_num);
    HI_PRINT("GPIO_I2C resume OK\n");
    return HI_SUCCESS;
}

#endif

static hi_u32 g_gpio_initialized = 0;

/*
 * initializes I2C interface routine.
 *
 * @return value:0--success; 1--error.
 *
 */
hi_s32 hi_drv_gpio_i2c_init(hi_void)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;

    ret = hi_drv_module_register(HI_ID_GPIO_I2C, "HI_GPIO_I2C", (hi_void *)&gpio_i2c_ext_funcs,HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_FATAL_GPIOI2C(" GPIO_I2C Module register failed 0x%x.\n", ret);
    }

    if (g_gpio_initialized == 0) {
        HI_OSAL_INIT_MUTEX(&g_gpio_i2c_sem);
        g_gpio_initialized = 1;

        for (i = 0; i < HI_GPIO_GROUP_NUM; i++) {
            g_gpio_reg_addrs[i] = (hi_u8 *)ioremap_nocache(gpio_i2c_phy_addr[i], HI_GPIO_ADDR_SIZE);
            if (g_gpio_reg_addrs[i] == HI_NULL) {
                HI_WARN_GPIO("ioremap_nocache err! \n");
                return HI_FAILURE;
            }
        }

        for (i = HI_STD_I2C_NUM; i < HI_I2C_MAX_NUM; i++) {
            g_gpio_i2c[i].is_used = HI_FALSE;
            g_gpio_i2c[i].count = 0;
        }
    }

    return HI_SUCCESS;
}

hi_void hi_drv_gpio_i2c_deinit(hi_void)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;

    ret = hi_drv_module_unregister(HI_ID_GPIO_I2C);
    if (ret != HI_SUCCESS) {
        HI_FATAL_GPIOI2C(" GPIO_I2C Module unregister failed 0x%x.\n", ret);
    }

    g_gpio_initialized = 0;

    for (i = 0; i < HI_GPIO_GROUP_NUM; i++) {
        iounmap(g_gpio_reg_addrs[i]);
    }

    for (i = HI_STD_I2C_NUM; i < HI_I2C_MAX_NUM; i++) {
        g_gpio_i2c[i].is_used = HI_FALSE;
        g_gpio_i2c[i].count = 0;
    }
}

MODULE_LICENSE("GPL");

EXPORT_SYMBOL(hi_drv_gpio_i2c_init);
EXPORT_SYMBOL(hi_drv_gpio_i2c_deinit);
#if 1
EXPORT_SYMBOL(hi_drv_gpio_i2c_write);
EXPORT_SYMBOL(hi_drv_gpio_i2c_read);
EXPORT_SYMBOL(hi_drv_gpio_i2c_write_ext);
EXPORT_SYMBOL(hi_drv_gpio_i2c_read_ext);
EXPORT_SYMBOL(hi_drv_gpio_i2c_read_si_labs);
EXPORT_SYMBOL(hi_drv_gpio_i2c_read_ext_directly);
EXPORT_SYMBOL(hi_drv_gpio_write_ext_nostop);

EXPORT_SYMBOL(hi_drv_gpio_i2c_sccb_read);
EXPORT_SYMBOL(hi_drv_gpio_i2c_create_gpio_i2c);
EXPORT_SYMBOL(hi_drv_gpio_i2c_destroy_gpio_i2c);
#endif
EXPORT_SYMBOL(g_i2c_mode);
EXPORT_SYMBOL(g_gpio_i2c);
