/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description:
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
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/i2c.h>
#include <linux/compat.h>

#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "drv_i2c.h"
#include "drv_i2c_ioctl.h"
#include "drv_i2c_ext.h"
#include "hi_reg_common.h"
#include "hi_drv_i2c.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"

#define SONY_I2C_START_EN          (0x01)  /* Output START (S) */
#define SONY_I2C_STOP_EN           (0x02)  /* Output STOP  (P) */
#define I2C_WAIT_TIME_OUT          0x1000
#define I2C_ADDR_SIZE              0x1000
#define HI_MAX_I2C_NUM             8

/* I2C extend flags */
#define HII2C_M_NOSTOP             0x0100
#define HII2C_M_NOSTART            0x0200
#define HII2C_M_NOSTART_NOADDR     0x0400

typedef enum {
    HII2C_STAT_RW_ERR = -1,
    HII2C_STAT_INIT,
    HII2C_STAT_RW,
    HII2C_STAT_SND_STOP,
    HII2C_STAT_RW_SUCCESS,
    HII2C_STAT_MAX
}hi_i2c_state;

typedef struct {
    struct i2c_client *client;
    struct semaphore *sem;
    hi_u32 num;
}hi_i2c_dev;

typedef struct {
    struct device *dev;
    void __iomem *regs;
    struct clk *clk;
    hi_s32 irq;

    struct i2c_adapter adapter;
    struct completion completion;

    struct i2c_msg *msg;
    hi_u32 pos;
    hi_u32 msg_len;

    hi_s32 stop;
    hi_s32 err;
    hi_u32 freq;
    hi_i2c_state state;
}hi_i2c_host;

hi_i2c_dev g_hi_i2c_dummy = {0};
hi_u32 g_hi_i2c_valid_num = 0;
hi_u32 g_i2c_rate_value[HI_MAX_I2C_NUM] = {0};
static atomic_t g_i2c_init_counter = ATOMIC_INIT(0);
static hi_u32 g_i2c_invaild_adap_num;

static i2c_ext_func g_st_i2c_ext_funcs = {
    .pfn_i2c_write_config   = hi_drv_i2c_write_config,
    .pfn_i2c_write          = hi_drv_i2c_write,
    .pfn_i2c_read           = hi_drv_i2c_read,
    .pfn_i2c_write_nostop   = hi_drv_i2c_write_nostop,
    .pfn_i2c_read_directly  = hi_drv_i2c_read_directly,
    .pfn_i2c_set_rate       = hi_drv_i2c_set_rate,
};

hi_s32 i2c_drv_set_rate(hi_u32 channel, hi_u32 rate_value)
{
    hi_i2c_host *i2c_dev = NULL;

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c channel %d exceed max channel num %d\n",
                   channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    g_i2c_rate_value[channel] = rate_value;

    i2c_dev = i2c_get_adapdata(g_hi_i2c_dummy.client[channel].adapter);
    i2c_dev->freq = rate_value;

    return HI_SUCCESS;
}

/*
 * add by Jiang Lei 2010-08-24
 * I2C write finished acknowledgement function
 * it use to e2prom device ,make sure it finished write operation.
 * i2c master start next write operation must waiting when it acknowledge
 * e2prom write cycle finished.
 */
hi_s32 i2c_drv_write_config(hi_u32 channel, hi_u8 dev_addr)
{
    return 0;
}

hi_s32 i2c_drv_write(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                     hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len, hi_bool with_stop)
{
    hi_s32 ret;
    hi_char *msg_buf = NULL;
    struct i2c_msg msg;
    hi_char reg[4] = {0};

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c channel %d exceed max channel num %d\n",
                   channel, g_hi_i2c_dummy.num);
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (buf == HI_NULL) {
        HI_ERR_I2C("buf null\n");
        return HI_ERR_I2C_NULL_PTR;
    }

    if (reg_addr_offset > sizeof(hi_u32)) {
        HI_ERR_I2C("regAddrsize exceed sizeof hi_u32\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (memset_s(&msg, sizeof(struct i2c_msg), 0, sizeof(struct i2c_msg))) {
        HI_ERR_I2C("memset_s msg failed\n");
        return HI_FAILURE;
    }

    if ((buf_len > HI_I2C_MAX_LENGTH) || (buf_len == 0)) {
        HI_ERR_I2C("para buf_len is invalid\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    msg_buf = kzalloc(reg_addr_offset + buf_len, GFP_KERNEL);
    if (!msg_buf) {
        HI_ERR_I2C("regAddrsize exceed sizeof hi_u32\n");
        return HI_ERR_I2C_MALLOC_ERR;
    }

    msg.addr = dev_addr >> 1;
    if (!with_stop) {
        msg.flags |= HII2C_M_NOSTOP;
    }

    msg.len = reg_addr_offset + buf_len;

    reg[0] = (reg_addr >> 24) & 0xFF;
    reg[1] = (reg_addr >> 16) & 0xFF;
    reg[2] = (reg_addr >> 8)  & 0xFF;
    reg[3] = (reg_addr >> 0)  & 0xFF;

    if (memset_s(msg_buf, reg_addr_offset + buf_len, 0, reg_addr_offset + buf_len)) {
        HI_ERR_I2C("memset_s msg_buf failed\n");
        kfree(msg_buf);
        msg_buf = HI_NULL;
        return HI_FAILURE;
    }

    if (reg_addr_offset != 0) {
        if (memcpy_s(msg_buf, reg_addr_offset, &(reg[sizeof(reg) - reg_addr_offset]), reg_addr_offset)) {
            HI_ERR_I2C("memcpy_s to msg_buf failed\n");
            kfree(msg_buf);
            msg_buf = HI_NULL;
            return HI_FAILURE;
        }
    }

    if (memcpy_s(msg_buf + reg_addr_offset, buf_len, (hi_void *)buf, buf_len)) {
        HI_ERR_I2C("memcpy_s to msg_buf + reg_addr_offset failed\n");
        kfree(msg_buf);
        msg_buf = HI_NULL;
        return HI_FAILURE;
    }
    msg.buf = msg_buf;

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg, 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        ret = HI_ERR_I2C_WRITE_TIMEOUT;
    } else {
        ret = HI_SUCCESS;
    }

    kfree(msg_buf);
    msg_buf = NULL;
    return ret;
}

hi_s32 i2c_drv_read(hi_u32 channel, hi_u8 dev_addr, hi_bool send_slave,
                 hi_u32 reg_addr, hi_u32 reg_addr_offset,
                 hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;
    struct i2c_msg msg[2];
    hi_char *msg_buf = NULL;
    hi_char reg[4] = {0};

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c channel %d exceed max channel num %d\n",
                   channel, g_hi_i2c_dummy.num);
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (buf == HI_NULL) {
        HI_ERR_I2C("buf null\n");
        return HI_ERR_I2C_NULL_PTR;
    }

    if (reg_addr_offset > sizeof(hi_u32)) {
        HI_ERR_I2C("regAddrsize exceed sizeof hi_u32\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (memset_s(msg, sizeof(msg), 0, sizeof(msg))) {
        HI_ERR_I2C("memset_s msg failed\n");
        return HI_FAILURE;
    }

    if (reg_addr_offset != 0) {
        msg_buf = kzalloc(reg_addr_offset, GFP_KERNEL);
        if (!msg_buf) {
            HI_ERR_I2C("i2c_drv_read kzalloc failed\n");
            return HI_ERR_I2C_MALLOC_ERR;
        }

        reg[0] = (reg_addr >> 24) & 0xFF;
        reg[1] = (reg_addr >> 16) & 0xFF;
        reg[2] = (reg_addr >> 8)  & 0xFF;
        reg[3] = (reg_addr >> 0)  & 0xFF;

        if (memset_s(msg_buf, reg_addr_offset, 0, reg_addr_offset)) {
            HI_ERR_I2C("memset_s msg_buf failed\n");
            kfree(msg_buf);
            msg_buf = NULL;
            return HI_FAILURE;
        }

        if (reg_addr_offset != 0) {
            if (memcpy_s(msg_buf, reg_addr_offset, &(reg[sizeof(reg) - reg_addr_offset]), reg_addr_offset)) {
                HI_ERR_I2C("memcpy_s to msg_buf failed\n");
                kfree(msg_buf);
                msg_buf = NULL;
                return HI_FAILURE;
            }
        }
    }

    msg[0].addr = dev_addr >> 1;
    msg[0].len = reg_addr_offset;
    msg[0].buf = msg_buf;
    msg[0].flags = 0;
    if (!send_slave) {
        msg[0].flags |= HII2C_M_NOSTART_NOADDR;
    }

    msg[1].addr = dev_addr >> 1;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = (u16)buf_len;
    msg[1].buf = buf;

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, msg, 2);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        ret = HI_ERR_I2C_WRITE_TIMEOUT;
    } else {
        ret = HI_SUCCESS;
    }

    kfree(msg_buf);
    msg_buf = NULL;
    return ret;

}

hi_s32 i2c_drv_read_si_labs(hi_u32 channel, hi_u8 dev_addr, hi_bool send_slave,
                         hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;
    struct i2c_msg msg[2];

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c channel %d exceed max channel num %d\n",
                   channel, g_hi_i2c_dummy.num);
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (buf == HI_NULL) {
        HI_ERR_I2C("buf null\n");
        return HI_ERR_I2C_NULL_PTR;
    }

    if (memset_s(msg, sizeof(msg), 0, sizeof(msg))) {
        HI_ERR_I2C("memset_s msg failed\n");
        return HI_FAILURE;
    }

    msg[0].addr = dev_addr >> 1;
    msg[0].len = 1;
    msg[0].buf = buf;
    msg[0].flags |= I2C_M_RD;

    msg[1].addr = dev_addr >> 1;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = buf_len;
    msg[1].buf = buf;

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg[0], 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        return HI_ERR_I2C_WRITE_TIMEOUT;
    }

    udelay(5);

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg[1], 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        ret = HI_ERR_I2C_WRITE_TIMEOUT;
    } else {
        ret = HI_SUCCESS;
    }

    return ret;
}

hi_s32 i2c_drv_write_sony(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                          hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len, hi_u8 mode)
{
    hi_s32 ret;
    struct i2c_msg msg;
    hi_char *msg_buf = NULL;
    hi_char reg[4] = {0};

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c channel %d exceed max channel num %d\n",
                   channel, g_hi_i2c_dummy.num);
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (!buf) {
        HI_ERR_I2C("buf null\n");
        return HI_ERR_I2C_NULL_PTR;
    }

    if (reg_addr_offset > sizeof(hi_u32)) {
        HI_ERR_I2C("regAddrsize exceed sizeof hi_u32\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    if ((buf_len > HI_I2C_MAX_LENGTH) || (buf_len == 0)) {
        HI_ERR_I2C("buf_len is invalid!\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (memset_s(&msg, sizeof(struct i2c_msg), 0, sizeof(struct i2c_msg))) {
        HI_ERR_I2C("memset_s msg failed\n");
        return HI_FAILURE;
    }
    msg_buf = kzalloc(reg_addr_offset + buf_len, GFP_KERNEL);
    if (!msg_buf) {
        HI_ERR_I2C("i2c_drv_read kzalloc failed\n");
        return HI_ERR_I2C_MALLOC_ERR;
    }
    msg.addr = dev_addr >> 1;
    if (!(mode & SONY_I2C_START_EN)) {
        msg.flags |= HII2C_M_NOSTART;
    }

    if (!(mode & SONY_I2C_STOP_EN)) {
        msg.flags |= HII2C_M_NOSTOP;
    }

    msg.len = reg_addr_offset + buf_len;

    reg[0] = (reg_addr >> 24) & 0xFF;
    reg[1] = (reg_addr >> 16) & 0xFF;
    reg[2] = (reg_addr >> 8)  & 0xFF;
    reg[3] = (reg_addr >> 0)  & 0xFF;

    if (memset_s(msg_buf, reg_addr_offset + buf_len, 0, reg_addr_offset + buf_len)) {
        HI_ERR_I2C("memset_s msg_buf failed\n");
        kfree(msg_buf);
        msg_buf = NULL;
        return HI_FAILURE;
    }

    if (reg_addr_offset != 0) {
        if (memcpy_s(msg_buf, reg_addr_offset, &(reg[sizeof(reg) - reg_addr_offset]), reg_addr_offset)) {
            HI_ERR_I2C("memcpy_s to msg_buf failed\n");
            kfree(msg_buf);
            msg_buf = NULL;
            return HI_FAILURE;
        }
    }

    if (memcpy_s(msg_buf + reg_addr_offset, buf_len, buf, buf_len)) {
        HI_ERR_I2C("memcpy_s msg_buf + reg_addr_offset failed\n");
        kfree(msg_buf);
        msg_buf = NULL;
        return HI_FAILURE;
    }
    msg.buf = msg_buf;

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg, 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        ret = HI_ERR_I2C_WRITE_TIMEOUT;
    } else {
        ret = HI_SUCCESS;
    }

    kfree(msg_buf);
    msg_buf = NULL;
    return ret;
}

int i2c_drv_read_sony(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                      hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len, hi_u8 mode)
{
    hi_s32 ret;
    struct i2c_msg msg[2];
    hi_char *msg_buf = NULL;
    hi_char reg[4] = {0};

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c channel %d exceed max channel num %d\n",
                   channel, g_hi_i2c_dummy.num);
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (!buf) {
        HI_ERR_I2C("buf null\n");
        return HI_ERR_I2C_NULL_PTR;
    }

    if (reg_addr_offset > sizeof(hi_u32)) {
        HI_ERR_I2C("regAddrsize exceed sizeof hi_u32\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (memset_s(&msg, sizeof(msg), 0, sizeof(msg))) {
        HI_ERR_I2C("memset_s msg failed\n");
        return HI_FAILURE;
    }
    msg_buf = kzalloc(reg_addr_offset, GFP_KERNEL);
    if (!msg_buf) {
        HI_ERR_I2C("i2c_drv_read kzalloc failed\n");
        return HI_ERR_I2C_MALLOC_ERR;
    }

    reg[0] = (reg_addr >> 24) & 0xFF;
    reg[1] = (reg_addr >> 16) & 0xFF;
    reg[2] = (reg_addr >> 8)  & 0xFF;
    reg[3] = (reg_addr >> 0)  & 0xFF;

    if (reg_addr_offset != 0) {
        if (memcpy_s(msg_buf, reg_addr_offset, &(reg[sizeof(reg) - reg_addr_offset]), reg_addr_offset)) {
            HI_ERR_I2C("memcpy_s to msg_buf failed\n");
            kfree(msg_buf);
            msg_buf = NULL;
            return HI_FAILURE;
        }
    }

    msg[0].addr = dev_addr >> 1;
    msg[0].flags = 0;
    msg[0].len = reg_addr_offset;
    msg[0].buf = msg_buf;

    msg[1].addr = dev_addr >> 1;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = buf_len;
    msg[1].buf = buf;

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg[0], 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        return HI_ERR_I2C_WRITE_TIMEOUT;
    }

    udelay(5);

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg[1], 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        ret = HI_ERR_I2C_WRITE_TIMEOUT;
    } else {
        ret = HI_SUCCESS;
    }

    kfree(msg_buf);
    msg_buf = NULL;
    return ret;
}


hi_s32 i2c_drv_read_2stop(hi_u32 channel, hi_u8 dev_addr, hi_bool send_slave,
                       hi_u32 reg_addr, hi_u32 reg_addr_offset,
                       hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;
    struct i2c_msg msg[2];
    hi_char *msg_buf = NULL;
    hi_char reg[4] = {0};

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c channel %d exceed max channel num %d\n",
                   channel, g_hi_i2c_dummy.num);
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (!buf) {
        HI_ERR_I2C("buf null\n");
        return HI_ERR_I2C_NULL_PTR;
    }

    if (reg_addr_offset > sizeof(hi_u32)) {
        HI_ERR_I2C("regAddrsize exceed sizeof hi_u32\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    if (memset_s(msg, sizeof(msg), 0, sizeof(msg))) {
        HI_ERR_I2C("memset_s msg failed\n");
        return HI_FAILURE;
    }

    msg_buf = kzalloc(reg_addr_offset, GFP_KERNEL);
    if (!msg_buf) {
        HI_ERR_I2C("i2c_drv_read kzalloc failed\n");
        return HI_ERR_I2C_MALLOC_ERR;
    }

    reg[0] = (reg_addr >> 24) & 0xFF;
    reg[1] = (reg_addr >> 16) & 0xFF;
    reg[2] = (reg_addr >> 8)  & 0xFF;
    reg[3] = (reg_addr >> 0)  & 0xFF;

    if (reg_addr_offset != 0) {
        if (memcpy_s(msg_buf, reg_addr_offset, &(reg[sizeof(reg) - reg_addr_offset]), reg_addr_offset)) {
            HI_ERR_I2C("memcpy_s to msg_buf failed\n");
            kfree(msg_buf);
            msg_buf = NULL;
            return HI_FAILURE;
        }
    }

    msg[0].addr = dev_addr >> 1;
    msg[0].flags = 0;
    msg[0].len = reg_addr_offset;
    msg[0].buf = msg_buf;

    msg[1].addr = dev_addr >> 1;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = buf_len;
    msg[1].buf = buf;

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg[0], 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        return HI_ERR_I2C_WRITE_TIMEOUT;
    }

    udelay(5);

    ret = i2c_transfer(g_hi_i2c_dummy.client[channel].adapter, &msg[1], 1);
    if (ret < 0) {
        HI_ERR_I2C("i2c_transfer failed:%d\n", ret);
        ret = HI_ERR_I2C_WRITE_TIMEOUT;
    } else {
        ret = HI_SUCCESS;
    }

    kfree(msg_buf);
    msg_buf = NULL;
    return ret;
}

hi_s32 i2c_get_data_from_user(hi_size_t arg, i2c_data *data,
                              hi_u8 **buf, hi_bool copy_data)
{
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if (!data || !buf || !argp) {
        return HI_ERR_I2C_NULL_PTR;
    }

    if (copy_from_user(data, argp, sizeof(i2c_data))) {
        HI_INFO_I2C("copy data from user fail\n");
        return HI_ERR_I2C_COPY_DATA_ERR;
    }

    if ((data->buf_len > HI_I2C_MAX_LENGTH) ||
            (data->buf_len == 0) ||
            (data->i2c_num >= g_hi_i2c_dummy.num)  ||
            (data->i2c_reg_count > 4) ||
            (data->buf == NULL)) {
        HI_INFO_I2C("invalid para\n");
        return HI_ERR_I2C_INVALID_PARA;
    }

    *buf = HI_KMALLOC(HI_ID_I2C, data->buf_len, GFP_KERNEL);
    if (!(*buf)) {
        HI_ERR_I2C("i2c kmalloc fail\n");
        return HI_ERR_I2C_MALLOC_ERR;
    }

    if (memset_s(*buf, data->buf_len, 0,  data->buf_len)) {
        HI_ERR_I2C("memset_s buf failed\n");
        HI_KFREE(HI_ID_I2C, *buf);
        *buf = NULL;
        return HI_FAILURE;
    }
    if (copy_data) {
        if (copy_from_user(*buf, data->buf, data->buf_len)) {
            HI_INFO_I2C("copy data from user fail\n");
            HI_KFREE(HI_ID_I2C, *buf);
            *buf = NULL;
            return HI_ERR_I2C_COPY_DATA_ERR;
        }
    }

    return HI_SUCCESS;
}

hi_s32 i2c_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_s32  ret;
    hi_u8  *buf = NULL;
    i2c_data data = {0};
    i2c_rate rate = {0};
    hi_void __user *tmp = NULL;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (file == NULL) {
        HI_ERR_I2C("null pointer \n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case CMD_I2C_WRITE:
            ret = i2c_get_data_from_user(arg, &data, &buf, HI_TRUE);
            if (ret != HI_SUCCESS) {
                if (buf) {
                    HI_KFREE(HI_ID_I2C, buf);
                    buf = NULL;
                }
                break;
            }

            if (data.i2c_num >= g_hi_i2c_dummy.num) {
                HI_ERR_I2C("channel %d exceed max i2c num %d\n", data.i2c_num, g_hi_i2c_dummy.num);
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                return HI_ERR_I2C_INVALID_PARA;
            }

            ret = down_interruptible(&g_hi_i2c_dummy.sem[data.i2c_num]);
            if (ret) {
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                HI_ERR_I2C("i2c channel %d in use, lock failed\n", data.i2c_num);
                return HI_FAILURE;
            }

            ret = i2c_drv_write(data.i2c_num, data.i2c_dev_addr,
                                data.i2c_reg_addr, data.i2c_reg_count, buf,
                                data.buf_len, HI_TRUE);

            HI_KFREE(HI_ID_I2C, buf);
            buf = NULL;
            up(&g_hi_i2c_dummy.sem[data.i2c_num]);

            break;

        case CMD_I2C_READ:
            ret = i2c_get_data_from_user(arg, &data, &buf, HI_FALSE);
            if (ret != HI_SUCCESS) {
                if (buf) {
                    HI_KFREE(HI_ID_I2C, buf);
                    buf = NULL;
                }
                HI_ERR_I2C("CMD_I2C_READ get data from user failed: 0x%x\n", ret);
                break;
            }

            if (data.i2c_num >= g_hi_i2c_dummy.num) {
                HI_ERR_I2C("channel %d exceed max i2c num %d\n", data.i2c_num, g_hi_i2c_dummy.num);
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                return HI_ERR_I2C_INVALID_PARA;
            }

            if (data.buf == NULL) {
                HI_ERR_I2C("data.buf is null pointer\n");
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                return HI_ERR_I2C_INVALID_PARA;
            }

            ret = down_interruptible(&g_hi_i2c_dummy.sem[data.i2c_num]);
            if (ret) {
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                HI_ERR_I2C("i2c channel %d in use, lock failed\n", data.i2c_num);
                return HI_FAILURE;
            }

            ret = i2c_drv_read(data.i2c_num, data.i2c_dev_addr, HI_TRUE,
                               data.i2c_reg_addr, data.i2c_reg_count, buf, data.buf_len);
            if (ret == HI_SUCCESS) {
                if (copy_to_user(data.buf, buf, data.buf_len)) {
                    HI_ERR_I2C("copy data to user fail\n");
                    ret = HI_ERR_I2C_COPY_DATA_ERR;
                }
            }

            HI_KFREE(HI_ID_I2C, buf);
            buf = NULL;
            up(&g_hi_i2c_dummy.sem[data.i2c_num]);

            break;

        case CMD_I2C_SET_RATE:
            tmp = (hi_void __user *)(uintptr_t)arg;
            if (tmp == NULL) {
                HI_ERR_I2C("arg pointed to null!\n");
                ret = HI_FAILURE;
                break;
            }
            if (copy_from_user(&rate, tmp, sizeof(i2c_rate))) {
                HI_ERR_I2C("copy data from user fail\n");
                ret = HI_FAILURE;
                break;
            }

            if (rate.i2c_num >= g_hi_i2c_dummy.num) {
                HI_ERR_I2C("channel %d exceed max i2c num %d\n", rate.i2c_num, g_hi_i2c_dummy.num);
                return HI_ERR_I2C_INVALID_PARA;
            }

            ret = down_interruptible(&g_hi_i2c_dummy.sem[rate.i2c_num]);
            if (ret) {
                HI_ERR_I2C("i2c channel %d in use, lock failed\n", rate.i2c_num);
                return HI_FAILURE;
            }

            ret = i2c_drv_set_rate(rate.i2c_num, rate.rate_value);

            up(&g_hi_i2c_dummy.sem[rate.i2c_num]);
            break;

        default:
            return -ENOIOCTLCMD;

    }

    return ret;
}

#ifdef CONFIG_COMPAT
hi_s32 i2c_get_data_from_user_compat(hi_size_t arg, i2c_data_compat *data,
                                     hi_u8 **buf, hi_bool copy_data)
{
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if (argp == HI_NULL) {
        HI_ERR_I2C("invalid para\n");
        return HI_ERR_I2C_NULL_PTR;
    }

    if (!data || !buf) {
        return HI_ERR_I2C_NULL_PTR;
    }

    if (copy_from_user(data, argp, sizeof(i2c_data_compat))) {
        HI_ERR_I2C("copy data from user fail\n");
        return HI_ERR_I2C_COPY_DATA_ERR;
    }

    if ((data->buf_len > HI_I2C_MAX_LENGTH) || (data->buf_len == 0) ||
            (data->i2c_num >= g_hi_i2c_dummy.num)) {
        return HI_ERR_I2C_INVALID_PARA;
    }

    *buf = HI_KMALLOC(HI_ID_I2C, data->buf_len, GFP_KERNEL);
    if (!(*buf)) {
        HI_ERR_I2C("i2c kmalloc fail\n");
        return HI_ERR_I2C_MALLOC_ERR;
    }

    if (memset_s(*buf, data->buf_len, 0, data->buf_len)) {
        HI_ERR_I2C("memset_s buf failed\n");
        HI_KFREE(HI_ID_I2C, *buf);
        *buf = NULL;
        return HI_FAILURE;
    }
    if (copy_data) {
        if (copy_from_user(*buf, (hi_u8 *)compat_ptr(data->buf_addr), data->buf_len)) {
            HI_ERR_I2C("copy data from user fail\n");
            HI_KFREE(HI_ID_I2C, *buf);
            *buf = NULL;
            return HI_ERR_I2C_COPY_DATA_ERR;
        }
    }

    return HI_SUCCESS;
}


hi_s32 i2c_compat_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_s32  ret;
    hi_u8  *buf = NULL;
    i2c_data_compat data = {0};
    i2c_rate rate = {0};
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (file == NULL) {
        HI_ERR_I2C("null pointer \n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case CMD_I2C_WRITE:
            ret = i2c_get_data_from_user_compat(arg, &data, &buf, HI_TRUE);
            if (ret != HI_SUCCESS) {
                if (buf) {
                    HI_KFREE(HI_ID_I2C, buf);
                    buf = NULL;
                }
                break;
            }

            if (data.i2c_num >= g_hi_i2c_dummy.num) {
                HI_ERR_I2C("channel %d exceed max i2c num %d\n", data.i2c_num, g_hi_i2c_dummy.num);
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                return HI_ERR_I2C_INVALID_PARA;
            }

            ret = down_interruptible(&g_hi_i2c_dummy.sem[data.i2c_num]);
            if (ret) {
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                HI_ERR_I2C("i2c channel %d in use, lock failed\n", data.i2c_num);
                return HI_FAILURE;
            }

            ret = i2c_drv_write(data.i2c_num, data.i2c_dev_addr, data.i2c_reg_addr,
                                data.i2c_reg_count, buf, data.buf_len, HI_TRUE);

            HI_KFREE(HI_ID_I2C, buf);
            buf = NULL;
            up(&g_hi_i2c_dummy.sem[data.i2c_num]);

            break;

        case CMD_I2C_READ:
            ret = i2c_get_data_from_user_compat(arg, &data, &buf, HI_FALSE);
            if (ret != HI_SUCCESS) {
                if (buf) {
                    HI_KFREE(HI_ID_I2C, buf);
                    buf = NULL;
                }
                break;
            }

            if (data.i2c_num >= g_hi_i2c_dummy.num) {
                HI_ERR_I2C("channel %d exceed max i2c num %d\n", data.i2c_num, g_hi_i2c_dummy.num);
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                return HI_ERR_I2C_INVALID_PARA;
            }

            ret = down_interruptible(&g_hi_i2c_dummy.sem[data.i2c_num]);
            if (ret) {
                HI_KFREE(HI_ID_I2C, buf);
                buf = NULL;
                HI_ERR_I2C("i2c channel %d in use, lock failed\n", data.i2c_num);
                return HI_FAILURE;
            }

            ret = i2c_drv_read(data.i2c_num, data.i2c_dev_addr, HI_TRUE,
                               data.i2c_reg_addr, data.i2c_reg_count, buf, data.buf_len);
            if (ret == HI_SUCCESS) {
                if (copy_to_user((hi_u8 *)compat_ptr(data.buf_addr),
                                 buf, data.buf_len)) {
                    HI_ERR_I2C("copy data to user fail!\n");
                    ret = HI_ERR_I2C_COPY_DATA_ERR;
                }
            }

            HI_KFREE(HI_ID_I2C, buf);
            buf = NULL;
            up(&g_hi_i2c_dummy.sem[data.i2c_num]);

            break;

        case  CMD_I2C_SET_RATE:
            if (argp == HI_NULL) {
                HI_ERR_I2C("invalid para!\n");
                ret = HI_FAILURE;
                break;
            }

            if (copy_from_user(&rate, argp, sizeof(i2c_rate))) {
                HI_ERR_I2C("copy data from user fail\n");
                ret = HI_FAILURE;
                break;
            }

            if (rate.i2c_num >= g_hi_i2c_dummy.num) {
                HI_ERR_I2C("channel %d exceed max i2c num %d\n", rate.i2c_num, g_hi_i2c_dummy.num);
                return HI_ERR_I2C_INVALID_PARA;
            }

            ret = down_interruptible(&g_hi_i2c_dummy.sem[rate.i2c_num]);
            if (ret) {
                HI_ERR_I2C("i2c channel %d in use, lock failed\n", rate.i2c_num);
                return HI_FAILURE;
            }

            ret = i2c_drv_set_rate(rate.i2c_num, rate.rate_value);

            up(&g_hi_i2c_dummy.sem[rate.i2c_num]);
            break;

        default:
            return -ENOIOCTLCMD;
    }

    return ret;
}
#endif

static hi_s32 i2c_drv_get_adapters_num(struct device *dev, hi_void *data)
{
    hi_s32 id = 0;
    if (dev->type != &i2c_adapter_type) {
        return 0;
    }
    if (dev->of_node) {
        id = of_alias_get_id(dev->of_node, "i2c");
        if (id > *((hi_slong *)data)) {
            *((hi_slong *)data) = id;
        }
    } else {
        HI_INFO_I2C("i2c not configure in dts \n");
        return -1;
    }

    return 0;
}

static hi_s32 i2c_drv_dummy_attach_adapter(struct device *dev, hi_void *dummy)
{
    struct i2c_adapter *adap = NULL;

    if (dev->type != &i2c_adapter_type) {
        return 0;
    }

    adap = to_i2c_adapter(dev);
    if (!adap) {
        HI_ERR_I2C("i2c_get_adapter failed\n");
        return -ENODEV;
    }

    if (strcmp(adap->name, "hisilicon-hdmi-ddc-0") == 0 ||
        strcmp(adap->name, "hisilicon-hdmi-ddc-1") == 0) {
        HI_INFO_I2C("hdmi adapter, not processed !\n");
        g_i2c_invaild_adap_num++;
        return 0;
    }
    if ((adap->nr - g_i2c_invaild_adap_num) >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("i2c_for_each_dev error, i2c_num:%d\n", g_hi_i2c_dummy.num);
        return 0;
    }

    sema_init(&g_hi_i2c_dummy.sem[adap->nr], 1);
    g_hi_i2c_dummy.client[adap->nr].adapter = adap;
    atomic_add(1, (atomic_t *)(&g_hi_i2c_dummy.client[adap->nr].adapter->dev.kobj.kref.refcount));

    return 0;
}

static hi_void i2c_drv_dummy_detach_adapter(hi_void)
{
    hi_s32 pos;
    for (pos = 0; pos < g_hi_i2c_dummy.num; pos++) {
        if (g_hi_i2c_dummy.client[pos].adapter) {
            i2c_put_adapter(g_hi_i2c_dummy.client[pos].adapter);
        }
    }
}

hi_s32 hi_drv_i2c_init(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    hi_slong i2c_dev_num = -1;

    if (atomic_inc_return(&g_i2c_init_counter) != 1) {
        HI_WARN_I2C(" HI_DRV_I2C already registered:%d\n",
                    atomic_read(&g_i2c_init_counter));
        return HI_SUCCESS;
    }

    ret = hi_drv_module_register(HI_ID_I2C, "HI_I2C", (hi_void *)&g_st_i2c_ext_funcs, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_I2C("I2C register failed\n");
        return HI_FAILURE;
    }

    i2c_for_each_dev(&i2c_dev_num, i2c_drv_get_adapters_num);
    if (i2c_dev_num == -1) {
        HI_ERR_I2C("i2c device not found, init failed: %d\n", i2c_dev_num);
        return HI_FAILURE;
    }

    g_hi_i2c_dummy.num = i2c_dev_num + 1;
    g_hi_i2c_valid_num = g_hi_i2c_dummy.num;

    g_hi_i2c_dummy.client = kzalloc(sizeof(struct i2c_client) * g_hi_i2c_dummy.num, GFP_KERNEL);
    if (!g_hi_i2c_dummy.client) {
        HI_ERR_I2C("g_hi_i2c_dummy kzalloc i2c_client failed:%d\n", g_hi_i2c_dummy.num);
        return HI_ERR_I2C_MALLOC_ERR;
    }

    g_hi_i2c_dummy.sem = kzalloc(sizeof(struct semaphore) * g_hi_i2c_dummy.num, GFP_KERNEL);
    if (!g_hi_i2c_dummy.sem) {
        HI_ERR_I2C("g_hi_i2c_dummy kzalloc sema failed\n");
        ret = HI_ERR_I2C_MALLOC_ERR;
        goto sema_err;
    }

    ret = i2c_for_each_dev(NULL, i2c_drv_dummy_attach_adapter);
    if (ret) {
        HI_ERR_I2C("g_hi_i2c_dummy attach adapter failed\n");
        ret = HI_ERR_I2C_OPEN_ERR;
        goto attach_err;
    }

    return HI_SUCCESS;

attach_err:
    if (g_hi_i2c_dummy.sem) {
        kfree(g_hi_i2c_dummy.sem);
        g_hi_i2c_dummy.sem = NULL;
    }

sema_err:
    if (g_hi_i2c_dummy.client) {
        kfree(g_hi_i2c_dummy.client);
        g_hi_i2c_dummy.client = NULL;
    }
    return ret;
}

hi_void hi_drv_i2c_deinit(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    if (!atomic_dec_and_test(&g_i2c_init_counter)) {
        HI_WARN_I2C("hi_drv_i2c_deinit counter:%d\n",
                    atomic_read(&g_i2c_init_counter));
        return;
    }

    i2c_drv_dummy_detach_adapter();

    kfree(g_hi_i2c_dummy.client);
    kfree(g_hi_i2c_dummy.sem);
    g_hi_i2c_dummy.client = NULL;
    g_hi_i2c_dummy.sem = NULL;
    g_hi_i2c_dummy.num = 0;
    g_hi_i2c_valid_num = 0;

    ret = hi_drv_module_unregister(HI_ID_I2C);
    if (ret != HI_SUCCESS) {
        HI_ERR_I2C("I2C unregister failed\n");
    }

    pr_notice("hi_drv_i2c_deinit success\n");
    return;
}

hi_s32 hi_drv_i2c_write_config(hi_u32 channel, hi_u8 dev_addr)
{
    hi_s32 ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }
    ret = i2c_drv_write_config(channel, dev_addr);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

hi_s32 hi_drv_i2c_write(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                        hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_write(channel, dev_addr, reg_addr, reg_addr_offset,
                        buf, buf_len, HI_TRUE);

    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, "
                "Num=%d, Len=%d, data0=0x%x\n", ret, channel, dev_addr,
                reg_addr, reg_addr_offset, buf_len, buf[0]);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

hi_s32 hi_drv_i2c_write_nostop(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                               hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_write(channel, dev_addr, reg_addr, reg_addr_offset,
                        buf, buf_len, HI_FALSE);
    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, "
                "Num=%d, Len=%d, data0=0x%x\n", ret, channel, dev_addr,
                reg_addr, reg_addr_offset, buf_len, buf[0]);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

hi_s32 hi_drv_i2c_read(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                       hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_read(channel, dev_addr, HI_TRUE, reg_addr, reg_addr_offset,
                       buf, buf_len);
    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, "
                "Num=%d, Len=%d\n", ret, channel, dev_addr, reg_addr,
                reg_addr_offset, buf_len);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

hi_s32 hi_drv_i2c_read_si_labs(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                               hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_read_si_labs(channel, dev_addr, HI_TRUE, reg_addr,
                               reg_addr_offset, buf, buf_len);
    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, "
                "Num=%d, Len=%d\n", ret, channel, dev_addr, reg_addr,
                reg_addr_offset, buf_len);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

hi_s32 hi_drv_i2c_write_sony(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                             hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len, hi_u8 mode)
{
    hi_s32  ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_write_sony(channel, dev_addr, reg_addr, reg_addr_offset,
                             buf, buf_len, mode);
    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, Num=%d, "
                "Len=%d, data0=0x%x\n", ret, channel, dev_addr, reg_addr,
                reg_addr_offset, buf_len, buf[0]);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

hi_s32 hi_drv_i2c_read_sony(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr,
                            hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len, hi_u8 mode)
{
    hi_s32  ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_read_sony(channel, dev_addr, reg_addr, reg_addr_offset,
                            buf, buf_len, mode);
    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, "
                "Num=%d, Len=%d\n", ret, channel, dev_addr, reg_addr,
                reg_addr_offset, buf_len);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

/*
 * Added begin: sdk 20120131, for avl6211 demod
 * Some I2C needn't send slave address before read
 */
hi_s32 hi_drv_i2c_read_directly(hi_u32 channel, hi_u8 dev_addr,
                                hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_read(channel, dev_addr, HI_FALSE, reg_addr,
                       reg_addr_offset, buf, buf_len);
    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, "
                "Num=%d, Len=%d\n", ret, channel, dev_addr, reg_addr,
                reg_addr_offset, buf_len);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}


hi_s32 hi_drv_i2c_read_2stop(hi_u32 channel, hi_u8 dev_addr,
                             hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;

    if (atomic_read(&g_i2c_init_counter) <= 0) {
        HI_ERR_I2C("i2c not initialized\n");
        return HI_FAILURE;
    }

    if (channel >= g_hi_i2c_dummy.num) {
        HI_ERR_I2C("channel %d exceed max i2c num %d\n", channel, g_hi_i2c_dummy.num);
        return HI_FAILURE;
    }

    ret = down_interruptible(&g_hi_i2c_dummy.sem[channel]);
    if (ret) {
        HI_INFO_I2C("i2c channel %d in use, lock failed\n", channel);
        return HI_FAILURE;
    }

    ret = i2c_drv_read_2stop(channel, dev_addr, HI_TRUE, reg_addr, reg_addr_offset,
                             buf, buf_len);
    HI_INFO_I2C("ret=0x%x, channel=%d, DevAddr=0x%x, RegAddr=0x%x, "
                "Num=%d, Len=%d\n", ret, channel, dev_addr, reg_addr,
                reg_addr_offset, buf_len);

    up(&g_hi_i2c_dummy.sem[channel]);

    return ret;
}

hi_s32 hi_drv_i2c_set_rate(hi_u32 channel, hi_u32 rate_value)
{
    return i2c_drv_set_rate(channel, rate_value);
}

#ifndef MODULE
EXPORT_SYMBOL(i2c_ioctl);
#endif
EXPORT_SYMBOL(hi_drv_i2c_init);
EXPORT_SYMBOL(hi_drv_i2c_deinit);
EXPORT_SYMBOL(hi_drv_i2c_write_config);
EXPORT_SYMBOL(hi_drv_i2c_write);
EXPORT_SYMBOL(hi_drv_i2c_read);
EXPORT_SYMBOL(hi_drv_i2c_read_si_labs);
EXPORT_SYMBOL(hi_drv_i2c_read_sony);
EXPORT_SYMBOL(hi_drv_i2c_read_2stop);
EXPORT_SYMBOL(hi_drv_i2c_write_sony);
EXPORT_SYMBOL(hi_drv_i2c_read_directly);
EXPORT_SYMBOL(hi_drv_i2c_write_nostop);
EXPORT_SYMBOL(hi_drv_i2c_set_rate);
