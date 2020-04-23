/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:
*/

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>
#include <linux/amba/pl022.h>
#include <linux/gpio.h>
#include <linux/atomic.h>
#include <linux/compat.h>
#include <linux/uaccess.h>

#include "hi_drv_spi.h"
#include "drv_spi_ioctl.h"
#include "hi_drv_mem.h"
#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_reg_common.h"
#include "linux/hisilicon/securec.h"

#define SPI0_BASE   0xF8B1A000
#define SPI1_BASE   0xF8B1B000

/* SPI register definition .*/
#define SPI0_SR               (SPI0_BASE + 0x0C)
#define SPI0_TXFIFOCR         (SPI0_BASE + 0x28)
#define SPI0_RXFIFOCR         (SPI0_BASE + 0x2C)
#define SPI1_SR               (SPI1_BASE + 0x0C)
#define SPI1_TXFIFOCR         (SPI1_BASE + 0x28)
#define SPI1_RXFIFOCR         (SPI1_BASE + 0x2C)

#define SPI0_CSGPIO_NO      (1 * 8 + 4)
#define SPI1_CSGPIO_NO      (-1)

#define MAX_SPI_TRANSFER_SIZE  (4 * 1024 * 1024)

struct hi_spi_dev {
    struct semaphore   lock;
    struct spi_device *spi_dev;
    struct spi_board_info chip;
    struct pl022_config_chip pl022_config_info;
    u8 bits_per_word;
};

static struct hi_spi_dev spi_dummy[MAX_SPI_CHANNEL];
static hi_u32 spi_gpiocs;
static atomic_t spi_init_counter = ATOMIC_INIT(0);

static hi_s32 spi_double_cs(hi_void) {
    return 0;
}

hi_void SPI_WRITE_REG(hi_u32 RegAddr, hi_s32 value)
{
    void * __iomem ioaddr = ioremap_nocache(RegAddr, 0x4);
    if (ioaddr == HI_NULL) {
        HI_ERR_SPI("spi w ioremap failed.\n");
        return;
    }

    writel(value, ioaddr);
    iounmap(ioaddr);
}

hi_void SPI_READ_REG(hi_u32 RegAddr, hi_s32 *value)
{
    void * __iomem ioaddr;

    if (value == HI_NULL) {
        HI_ERR_SPI("null pointer\n");
        return;
    }

    ioaddr = ioremap_nocache(RegAddr, 0x4);
    if(ioaddr == HI_NULL) {
        HI_ERR_SPI("spi r ioremap failed.\n");
        return;
    }

    *value = readl(ioaddr);
    iounmap(ioaddr);
}

hi_u8 hi_drv_spi_get_cs_config(hi_void)
{
    return spi_gpiocs ? 1 : 0;
}

hi_void hi_drv_spi_set_cs_config(hi_u8 gpio_cs)
{
    spi_gpiocs = gpio_cs ? 1 : 0;
}

hi_void hi_drv_spi_set_cs_gpio(hi_u8 id)
{
    hi_s32 error;
    hi_s32 gpio_no;
    struct spi_device *spi_dev = NULL;

    if (!spi_gpiocs) {
        HI_ERR_SPI("hi_drv_spi_set_cs_config(1) firstly\n");
        return;
    }

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return;
    }

    spi_dev = spi_dummy[id].spi_dev;
    if (spi_dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return;
    }

    gpio_no = (id == 0) ? SPI0_CSGPIO_NO : SPI1_CSGPIO_NO;

    if (gpio_no < 0) {
        HI_ERR_SPI("don't support double chipselect, id:%d, gpio:%d\n", id, gpio_no);
        return;
    }

    error = devm_gpio_request(&(spi_dev->dev), gpio_no, NULL);
    if (error < 0)
        HI_ERR_SPI("gpio%d request failed:%d\n", gpio_no, error);

    /* set gpio direction output and high level to disable chipselect */
    error = gpio_direction_output(gpio_no, 1);
    if (error) {
        HI_ERR_SPI("Set gpio%d direction output failed:%d\n", gpio_no, error);
    }

    return;
}

hi_void hi_drv_spi_set_cs_level(hi_u8 id,hi_u32 level)
{
    hi_s32 gpio_no;

    if (!spi_gpiocs) {
        return;
    }

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return;
    }

    gpio_no = (id == 0) ? SPI0_CSGPIO_NO : SPI1_CSGPIO_NO;

    if (gpio_no < 0) {
        HI_ERR_SPI("don't support double chipselect, id:%d\n", id);
        return;
    }

    gpio_set_value(gpio_no, level ? 1 : 0);

}

/* Loop Model  for test set = 0 :normal set = 1:loopMod */
hi_void HI_DRV_SPI_SetLoop(hi_u8 id,hi_u8 set)
{
    hi_s32 ret;
    struct spi_device *dev = NULL;
    struct spi_board_info *chip = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return;
    }

    chip = &spi_dummy[id].chip;
    if (chip == HI_NULL) {
        HI_ERR_SPI("spi chip not initialized\n", id);
        return;
    }

    if (set) {
        dev->mode |= SPI_LOOP;
        chip->mode |= SPI_LOOP;
    } else {
        dev->mode &= ~SPI_LOOP;
        chip->mode &= ~SPI_LOOP;
    }

    ret = spi_setup(dev);
    if (ret < 0) {
        HI_ERR_SPI("spi_setup %d error:%d\n", id, ret);
   }
}

hi_void HI_DRV_SPI_SetFIFO(hi_u8 id,hi_u8 hi_s32size)
{
    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return;
    }

    if (id == 0){
        SPI_WRITE_REG(SPI0_RXFIFOCR, hi_s32size<<3);
    } else if (id == 1) {
        if (spi_double_cs())
            SPI_WRITE_REG(SPI1_RXFIFOCR, hi_s32size<<3);
    }
}

/*
 * set SPI frame form routine.
 *
 * @param framemode: see enum ssp_hi_s32erface
 *
 * @param sphvalue: SPICLKOUT phase (0/1)
 * @param sp0: SSPCLKOUT voltage level (0/1)
 * @param datavalue: data bit
 * 0000: reserved    0001: reserved    0010: reserved    0011: 4bit data
 * 0100: 5bit data   0101: 6bit data   0110:7bit data    0111: 8bit data
 * 1000: 9bit data   1001: 10bit data  1010:11bit data   1011: 12bit data
 * 1100: 13bit data  1101: 14bit data  1110:15bit data   1111: 16bit data
 *
 * @return value: 0--success; -1--error.
 *
 */

hi_s32 hi_drv_spi_set_from(hi_u8 id,hi_u8 framemode,
    hi_u8 spo,hi_u8 sph,hi_u8 bits)
{
    hi_s32 ret;
    struct pl022_config_chip *pl022 = NULL;
    struct spi_device *dev = NULL;
    struct spi_board_info *chip = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return -1;
    }

    chip = &spi_dummy[id].chip;
    pl022 = &spi_dummy[id].pl022_config_info;

    if (!chip || !pl022) {
        HI_ERR_SPI("spi chip or pl022_config_info not initialized\n", id);
        return -1;
    }

    pl022->iface = framemode;

    if (spo) {
        dev->mode |= SPI_CPOL;
        chip->mode |= SPI_CPOL;
    } else {
        dev->mode &= ~SPI_CPOL;
        chip->mode &= ~SPI_CPOL;
    }

    if (sph) {
        dev->mode |= SPI_CPHA;
        chip->mode |= SPI_CPHA;
    } else {
        dev->mode &= ~SPI_CPHA;
        chip->mode &= ~SPI_CPHA;
    }

    dev->bits_per_word = bits;
    spi_dummy[id].bits_per_word = bits;

    ret = spi_setup(dev);
    if (ret < 0) {
        HI_ERR_SPI("spi_setup error:%d, id:%d, sph:%d, spo:%d, bits:%d, framemode:%d\n",
            ret, id, sph, spo, bits, framemode);
       return ret;
   }

    return HI_SUCCESS;
}

hi_s32 hi_drv_spi_get_from(hi_u8 id,hi_u8 *framemode,hi_u8 *spo,
    hi_u8 *sph,hi_u8 *datawidth)
{
    struct spi_device *dev = NULL;
    struct pl022_config_chip *pl022 = NULL;
    struct spi_board_info *chip = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    if (!framemode || !spo || !sph || !datawidth) {
        HI_ERR_SPI("null pointers\n");
        return -1;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return -1;
    }

    chip = &spi_dummy[id].chip;
    pl022 = &spi_dummy[id].pl022_config_info;

    if (!chip || !pl022) {
        HI_ERR_SPI("spi chip or pl022_config_info not initialized\n", id);
        return -1;
    }

    *framemode = pl022->iface;
    *spo = (chip->mode & SPI_CPOL)?1:0;
    *sph = (chip->mode & SPI_CPHA)?1:0;
    *datawidth = dev->bits_per_word;

    HI_INFO_SPI("framemode=%d spo=%d sph=%d datawidth=%d\n",
        *framemode,*spo,*sph,*datawidth);

    return HI_SUCCESS;
}

/*
 * set SPI serial clock rate routine.
 *
 * @param scr: scr value.(0-255,usually it is 0)
 * @param cpsdvsr: Clock prescale divisor.(2-254 even)
 *
 * @return value: 0--success; -1--error.
 *
 */
hi_s32 hi_drv_spi_set_clk(hi_u8 id,hi_u8 scr,hi_u8 cpsdvsr)
{
    hi_s32 ret;
    struct pl022_config_chip *pl022 = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    pl022 = &spi_dummy[id].pl022_config_info;
    if (pl022 == HI_NULL) {
        HI_ERR_SPI("spi pl022_config_info not initialized\n", id);
        return -1;
    }

    if (spi_dummy[id].spi_dev->controller_data != pl022) {
        HI_ERR_SPI("controller_data error\n");
        return -1;
    }

    pl022->clk_freq.cpsdvsr = cpsdvsr;
    pl022->clk_freq.scr = scr;

    ret = spi_setup(spi_dummy[id].spi_dev);
    if (ret < 0) {
        HI_ERR_SPI("channel %d spi_setup error:%d\n", id, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void hi_drv_spi_set_blend(hi_u8 id, hi_bool bBigEnd)
{
    hi_s32 ret;
    struct spi_device *dev = NULL;
    struct spi_board_info *chip = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return;
    }

    chip = &spi_dummy[id].chip;
    if (chip == HI_NULL) {
        HI_ERR_SPI("spi chip not initialized\n", id);
        return;
    }

    if (!bBigEnd) {
        dev->mode |= SPI_LSB_FIRST;
        chip->mode |= SPI_LSB_FIRST;
    } else {
        dev->mode &= ~SPI_LSB_FIRST;
        chip->mode &= ~SPI_LSB_FIRST;
    }

    ret = spi_setup(spi_dummy[id].spi_dev);
    if (ret < 0)
        HI_ERR_SPI("spi_setup %d error:%d\n", id, ret);
}

hi_bool hi_drv_spi_get_blend(hi_u8 id)
{
    struct spi_device *dev = NULL;
    struct spi_board_info *chip = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return -1;
    }

    chip = &spi_dummy[id].chip;
    return (chip && (chip->mode & SPI_LSB_FIRST)) ? 0 : 1;
}

hi_u32 hi_drv_spi_fifo_empty(hi_u8 id, hi_s32 send)
{
    hi_u32 ret = 0;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    if (id == 0) {
        SPI_READ_REG(SPI0_SR,&ret);
    } else if (id == 1) {
        if (spi_double_cs()) {
            SPI_READ_REG(SPI1_SR,&ret);
        } else {
            return 2;
        }
    } else {
        HI_ERR_SPI("para1 error!\n");
    }

    if (send) {
        return (ret & 0x1)?1:0; /* send fifo */
    } else {
        return (ret & 0x4)?0:1; /* receive fifo */
    }
}

/*
 *  write SPI_DR register rountine.
 *  @param  sdata: data of SSP_DR register
 */
hi_s32 hi_drv_spi_write_query(hi_u8 id, hi_u8 *send, hi_u32 send_sz)
{
    hi_s32 error = 0;
    struct spi_message   msg;
    struct spi_transfer  xfer;
    struct spi_device *dev = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return -1;
    }

    spi_message_init(&msg);
    if (memset_s(&xfer, sizeof(struct spi_transfer), 0, sizeof(struct spi_transfer))) {
        HI_ERR_SPI("memset_s failed! \n");
        return -1;
    }

    xfer.tx_buf    = send;
    xfer.rx_buf    = NULL;
    xfer.len       = send_sz;
    //xfer.cs_change = 0;
    spi_message_add_tail(&xfer, &msg);
    hi_drv_spi_set_cs_level(id, 0);

    error = spi_sync(dev, &msg);
    if (error) {
        HI_ERR_SPI("spi_sync failed:%d\n", error);
    }
    hi_drv_spi_set_cs_level(id, 1);

    return error;
}

hi_s32 hi_drv_spi_write_isr(hi_u8 devId,hi_u8 *Send, hi_u32 SendCnt)
{
    return hi_drv_spi_write_query(devId, Send, SendCnt);
}

hi_s32 hi_drv_spi_read_query(hi_u8 id, hi_u8 *read, hi_u32 read_sz)
{
    hi_s32 error = 0;
    struct spi_message   msg;
    struct spi_transfer  xfer;
    struct spi_device *dev = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return -1;
    }

    spi_message_init(&msg);
    if (memset_s(&xfer, sizeof(struct spi_transfer), 0, sizeof(struct spi_transfer))) {
        HI_ERR_SPI("memset_s failed! \n");
        return -1;
    }

    /* Read zero byte and event code */
    xfer.tx_buf    = NULL;
    xfer.rx_buf    = read;
    xfer.len       = read_sz;
    spi_message_add_tail(&xfer, &msg);

    hi_drv_spi_set_cs_level(id, 0);
    error = spi_sync(dev, &msg);
    if (error) {
        HI_ERR_SPI("spi_sync failed:%d\n", error);
    }
    hi_drv_spi_set_cs_level(id, 1);
    return error;
}

hi_s32 hi_drv_spi_read_isr(hi_u8 devId, hi_u8 *Read, hi_u32 ReadCnt)
{
    return hi_drv_spi_read_query(devId, Read, ReadCnt);
}

/*
 *  read SPI_DR register rountine.
 *
 *  @return value: data from SSP_DR register readed
 *
 */
hi_s32 hi_drv_spi_read_ex(hi_u8 id, hi_u8 *send, hi_u32 send_sz,
    hi_u8 *read, hi_u32 read_sz)
{
    hi_s32 error = 0;
    struct spi_message   msg;
    struct spi_transfer  xfer[2];
    struct spi_transfer *p = HI_NULL;
    struct spi_transfer *q = HI_NULL;
    struct spi_device *dev = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return -1;
    }

    spi_message_init(&msg);
    if (memset_s(xfer, sizeof(xfer), 0, sizeof(xfer))) {
        HI_ERR_SPI("memset_s failed! \n");
        return -1;
    }

    xfer[0].tx_buf    = send;
    xfer[0].rx_buf    = NULL;
    xfer[0].len       = send_sz;
    p = &xfer[0];
    spi_message_add_tail(p, &msg);

    /* Read zero byte and event code */
    xfer[1].tx_buf    = NULL;
    xfer[1].rx_buf    = read;
    xfer[1].len       = read_sz;

    q = &xfer[1];
    spi_message_add_tail(q, &msg);

    hi_drv_spi_set_cs_level(id, 0);
    error = spi_sync(dev, &msg);
    if (error) {
        HI_ERR_SPI("spi_sync failed:%d\n", error);
    }
    hi_drv_spi_set_cs_level(id, 1);
    return error;
}

hi_s32 hi_drv_spi_read_ex_isr(hi_u8 devId, hi_u8 *Send, hi_u32 SendCnt,
    hi_u8 *Read, hi_u32 ReadCnt)
{
    return hi_drv_spi_read_ex(devId, Send, SendCnt, Read, ReadCnt);
}

hi_s32 HI_DRV_SPI_RW_LOOP(hi_u8 id, hi_u8 *send, hi_u32 send_sz,
    hi_u8 *read, hi_u32 read_sz)
{
    hi_s32 error = 0;
    struct spi_message   msg;
    struct spi_transfer  xfer;
    struct spi_device *dev = NULL;

    if (id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", id);
        return -1;
    }

    dev = spi_dummy[id].spi_dev;
    if (dev == HI_NULL) {
        HI_ERR_SPI("open spi channel %d firstly\n", id);
        return -1;
    }

    if (send_sz > read_sz) {
        HI_ERR_SPI("invalid args, send_sz=%d, read_sz=%d\n", send_sz, read_sz);
        return -1;
    }

    spi_message_init(&msg);
    if (memset_s(&xfer, sizeof(xfer), 0, sizeof(xfer))) {
        HI_ERR_SPI("memset_s failed! \n");
        return -1;
    }

    xfer.tx_buf    = send;
    xfer.rx_buf    = read;
    xfer.len       = send_sz;
    spi_message_add_tail(&xfer, &msg);

    hi_drv_spi_set_cs_level(id, 0);
    error = spi_sync(dev, &msg);
    if (error) {
        HI_ERR_SPI("spi_sync failed:%d\n", error);
    }
    hi_drv_spi_set_cs_level(id, 1);
    return error;
}


hi_s32 SPI_Write(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    SPI_DATA_S SSPData;
    SPI_DATAEX_S __user *pSSPData = (SPI_DATAEX_S __user *)(hi_size_t)arg;

    if (HI_NULL == pSSPData) {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }


    if (copy_from_user(&SSPData, pSSPData, sizeof(SPI_DATA_S))) {
        HI_INFO_SPI("ssp read copy data from user fail!\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (SSPData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if(0 == SSPData.sDataCnt)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI, SSPData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp write kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }

    if (memset_s(pData, SSPData.sDataCnt, 0, SSPData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_FAILURE;

    }

    if (HI_NULL == SSPData.sData)
    {
        HI_ERR_SPI("pointer SSPData.sData is NULL! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_NULL_PTR;
    }

    if (copy_from_user(pData, SSPData.sData, SSPData.sDataCnt)) {
        HI_ERR_SPI("ssp write copy data from user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (hi_drv_spi_write_query(SSPData.devId, pData, SSPData.sDataCnt)) {
        HI_ERR_SPI("ssp write timeout\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_WRITE_TIMEOUT;
    }

    HI_KFREE(HI_ID_SPI,pData);

    return HI_SUCCESS;
}

hi_s32 SPI_Read(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    SPI_DATA_S SSPData;

    SPI_DATA_S __user *pSSPData = (SPI_DATA_S __user *)(hi_size_t)arg;
    if (HI_NULL == pSSPData) {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (memset_s(&SSPData, sizeof(SSPData), 0, sizeof(SSPData))) {
        HI_ERR_SPI("memset_s failed! \n");
        return HI_FAILURE;
    }
    if (copy_from_user(&SSPData, pSSPData, sizeof(SPI_DATA_S))) {
        HI_INFO_SPI("ssp read copy data from user fail!\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (SSPData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if(0 == SSPData.sDataCnt)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI,SSPData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp read ssp kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }
    if (memset_s(pData, SSPData.sDataCnt, 0, SSPData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_FAILURE;
    }

    if (hi_drv_spi_read_query(SSPData.devId, pData, SSPData.sDataCnt)) {
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_READ_TIMEOUT;
    }

    if (HI_NULL == SSPData.sData)
    {
        HI_ERR_SPI("SSPData.sData is null pointer!\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_NULL_PTR;
    }

    if (copy_to_user(SSPData.sData, pData, SSPData.sDataCnt)) {
        HI_ERR_SPI("ssp read copy data to user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    HI_KFREE(HI_ID_SPI, pData);

    return HI_SUCCESS;
}

hi_s32 SPI_ReadEx(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    hi_u8  *rData = NULL;
    SPI_DATAEX_S EXData;

    SPI_DATAEX_S __user *pEXData = (SPI_DATAEX_S __user *)(hi_size_t)arg;
    if(HI_NULL == pEXData) {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (memset_s(&EXData, sizeof(EXData), 0, sizeof(EXData))) {
        HI_ERR_SPI("memset_s failed! \n");
        return HI_FAILURE;
    }

    if (copy_from_user(&EXData, pEXData, sizeof(SPI_DATAEX_S))) {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if ((EXData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) ||
        (EXData.rDataCnt >= MAX_SPI_TRANSFER_SIZE)) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if ((0 == EXData.sDataCnt) || 0 == EXData.rDataCnt)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI,EXData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp pData kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }

    if (memset_s(pData, EXData.sDataCnt, 0, EXData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_FAILURE;
    }

    rData = HI_KMALLOC(HI_ID_SPI,EXData.rDataCnt, GFP_KERNEL);
    if (!rData) {
        HI_ERR_SPI("ssp rData kmalloc fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_MALLOC_ERR;
    }

    if (memset_s(rData, EXData.rDataCnt, 0, EXData.rDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_FAILURE;
    }

    if (HI_NULL == EXData.sData)
    {
        HI_ERR_SPI("pointer EXData.sData is NULL! \n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_NULL_PTR;
    }

    if (copy_from_user(pData, EXData.sData, EXData.sDataCnt)) {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (hi_drv_spi_read_ex(EXData.devId,pData,EXData.sDataCnt,
        rData,EXData.rDataCnt)) {
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_READ_TIMEOUT;
    }

    if (EXData.rData == HI_NULL) {
        HI_ERR_SPI("pointer EXData.rData is NULL! \n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_NULL_PTR;
    }

    if (copy_to_user(EXData.rData, rData, EXData.rDataCnt)) {
        HI_ERR_SPI("ssp readex copy data to user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    HI_KFREE(HI_ID_SPI, pData);
    HI_KFREE(HI_ID_SPI, rData);

    return HI_SUCCESS;
}

hi_s32 SPI_RW_Loop(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    hi_u8  *rData = NULL;
    SPI_DATAEX_S EXData;
    SPI_DATAEX_S __user *pEXData = (SPI_DATAEX_S __user *)(hi_size_t)arg;

    if (HI_NULL == pEXData)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (memset_s(&EXData, sizeof(EXData), 0, sizeof(EXData)))
    {
        HI_ERR_SPI("memset_s failed! \n");
        return HI_FAILURE;
    }

    if (copy_from_user(&EXData, pEXData, sizeof(SPI_DATAEX_S))) {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if ((EXData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) ||
        (EXData.rDataCnt >= MAX_SPI_TRANSFER_SIZE)) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if ((0 == EXData.sDataCnt) || (0 == EXData.rDataCnt))
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI,EXData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp pData kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }

    if (memset_s(pData, EXData.sDataCnt, 0, EXData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_FAILURE;
    }

    rData = HI_KMALLOC(HI_ID_SPI,EXData.rDataCnt, GFP_KERNEL);
    if (rData == HI_NULL) {
        HI_ERR_SPI("ssp rData kmalloc fail!\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_MALLOC_ERR;
    }

    if (memset_s(rData, EXData.rDataCnt, 0, EXData.rDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
    }

    if (HI_NULL == EXData.sData)
    {
        HI_ERR_SPI("EXData.sData is NULL! \n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_NULL_PTR;
    }

    if (copy_from_user(pData,EXData.sData, EXData.sDataCnt)) {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if(HI_DRV_SPI_RW_LOOP(EXData.devId,pData,EXData.sDataCnt,
        rData,EXData.rDataCnt)) {
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_READ_TIMEOUT;
    }

    if (EXData.rData == HI_NULL) {
        HI_ERR_SPI("EXData.rData is NULL! \n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_NULL_PTR;
    }

    if (copy_to_user(EXData.rData, rData, EXData.rDataCnt)) {
        HI_ERR_SPI("ssp readex copy data to user fail!\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    HI_KFREE(HI_ID_SPI, pData);
    HI_KFREE(HI_ID_SPI, rData);

    return HI_SUCCESS;
}

#ifdef CONFIG_COMPAT
hi_s32 SPI_Compat_Write(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    SPI_DATA_COMPAT_S SSPData;

    SPI_DATA_COMPAT_S __user *pSSPData = compat_ptr(arg);

    if (pSSPData == HI_NULL)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (copy_from_user(&SSPData, pSSPData, sizeof(SPI_DATA_COMPAT_S))) {
        HI_INFO_SPI("ssp read copy data from user fail!\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (HI_NULL == SSPData.sData)
    {
        HI_ERR_SPI("EXData.sData is NULL! \n");
        return HI_ERR_SPI_NULL_PTR;
    }

    if (SSPData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if(0 == SSPData.sDataCnt)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI, SSPData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp write kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }

    if (memset_s(pData, SSPData.sDataCnt, 0, SSPData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_FAILURE;
    }

    if (copy_from_user(pData, (hi_u8 *)compat_ptr(SSPData.sData), SSPData.sData))
    {
        HI_ERR_SPI("ssp write copy data from user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if(hi_drv_spi_write_query(SSPData.devId,pData,SSPData.sDataCnt)) {
        HI_ERR_SPI("ssp write timeout\n");
        HI_KFREE(HI_ID_SPI,pData);
        return HI_ERR_SPI_WRITE_TIMEOUT;
    }

    HI_KFREE(HI_ID_SPI,pData);

    return HI_SUCCESS;
}

hi_s32 SPI_Compat_Read(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    SPI_DATA_COMPAT_S SSPData;
    SPI_DATA_COMPAT_S __user *pSSPData = compat_ptr(arg);

    if (HI_NULL == pSSPData) {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (memset_s(&SSPData, sizeof(SSPData), 0, sizeof(SSPData))) {
        HI_ERR_SPI("memset_s failed! \n");
        return HI_FAILURE;
    }

    if (copy_from_user(&SSPData, pSSPData, sizeof(SPI_DATA_COMPAT_S))) {
        HI_INFO_SPI("ssp read copy data from user fail!\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (HI_NULL == SSPData.sData)
    {
        HI_ERR_SPI("EXData.sData is NULL! \n");
        return HI_ERR_SPI_NULL_PTR;
    }

    if (SSPData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (0 == SSPData.sDataCnt)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI, SSPData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp read ssp kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }
    if (memset_s(pData, SSPData.sDataCnt, 0, SSPData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_FAILURE;
    }

    if (hi_drv_spi_read_query(SSPData.devId, pData, SSPData.sDataCnt)) {
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_READ_TIMEOUT;
    }

    if (copy_to_user((hi_u8 *)compat_ptr(SSPData.sData), pData, SSPData.sDataCnt))
    {
        HI_ERR_SPI("ssp read copy data to user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    HI_KFREE(HI_ID_SPI, pData);

    return HI_SUCCESS;
}

hi_s32 SPI_Compat_ReadEx(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    hi_u8  *rData = NULL;
    SPI_DATAEX_COMPAT_S EXData;

    SPI_DATAEX_COMPAT_S __user *pSSPDataEx = compat_ptr(arg);

    if (HI_NULL == pSSPDataEx)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (memset_s(&EXData, sizeof(EXData), 0, sizeof(EXData))) {
        HI_ERR_SPI("memset_s failed! \n");
        return HI_FAILURE;
    }

    if (copy_from_user(&EXData, pSSPDataEx, sizeof(SPI_DATAEX_COMPAT_S))) {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (HI_NULL == EXData.sData)
    {
        HI_ERR_SPI("EXData.sData is NULL! \n");
        return HI_ERR_SPI_NULL_PTR;
    }

    if ((EXData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) ||
        (EXData.rDataCnt >= MAX_SPI_TRANSFER_SIZE)) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (0 == EXData.sDataCnt || 0 == EXData.rDataCnt)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI,EXData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp pData kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }

    if (memset_s(pData, EXData.sDataCnt, 0, EXData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_FAILURE;
    }

    rData = HI_KMALLOC(HI_ID_SPI,EXData.rDataCnt, GFP_KERNEL);
    if (rData == HI_NULL) {
        HI_ERR_SPI("ssp rData kmalloc fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_MALLOC_ERR;
    }


    if (memset_s(rData, EXData.rDataCnt, 0, EXData.rDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
    }

    if (copy_from_user(pData, (hi_u8 *)compat_ptr(EXData.sData), EXData.sDataCnt))
    {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (hi_drv_spi_read_ex(EXData.devId,pData,EXData.sDataCnt,
        rData,EXData.rDataCnt)) {
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_READ_TIMEOUT;
    }

    if (copy_to_user((hi_u8 *)compat_ptr(EXData.rData), rData, EXData.rDataCnt))
    {
        HI_ERR_SPI("ssp readex copy data to user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    HI_KFREE(HI_ID_SPI, pData);
    HI_KFREE(HI_ID_SPI, rData);

    return HI_SUCCESS;
}

hi_s32 SPI_Compat_RW_Loop(hi_size_t arg)
{
    hi_u8  *pData = NULL;
    hi_u8  *rData = NULL;
    SPI_DATAEX_COMPAT_S EXData;

    SPI_DATAEX_COMPAT_S __user *pEXData =  compat_ptr(arg);
    if (HI_NULL == pEXData)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (memset_s(&EXData, sizeof(EXData), 0, sizeof(EXData))) {
        HI_ERR_SPI("memset_s failed! \n");
    }

    if (copy_from_user(&EXData, pEXData, sizeof(SPI_DATAEX_COMPAT_S))) {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (HI_NULL == EXData.sData)
    {
        HI_ERR_SPI("EXData.sData is NULL! \n");
        return HI_ERR_SPI_NULL_PTR;
    }

    if ((EXData.sDataCnt >= MAX_SPI_TRANSFER_SIZE) ||
        (EXData.rDataCnt >= MAX_SPI_TRANSFER_SIZE)) {
        HI_ERR_SPI("ssp transfer msg too large\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (0 == EXData.sDataCnt || 0 == EXData.rDataCnt)
    {
        HI_ERR_SPI("invalid para\n");
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    pData = HI_KMALLOC(HI_ID_SPI,EXData.sDataCnt, GFP_KERNEL);
    if (pData == HI_NULL) {
        HI_ERR_SPI("ssp pData kmalloc fail\n");
        return HI_ERR_SPI_MALLOC_ERR;
    }
    if (memset_s(pData, EXData.sDataCnt, 0, EXData.sDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
    }

    rData = HI_KMALLOC(HI_ID_SPI,EXData.rDataCnt, GFP_KERNEL);
    if (rData == HI_NULL) {
        HI_ERR_SPI("ssp rData kmalloc fail!\n");
        HI_KFREE(HI_ID_SPI, pData);
        return HI_ERR_SPI_MALLOC_ERR;
    }
    if (memset_s(rData, EXData.rDataCnt, 0, EXData.rDataCnt)) {
        HI_ERR_SPI("memset_s failed! \n");
    }

    if (copy_from_user(pData,(hi_u8 *)compat_ptr(EXData.sData), EXData.sDataCnt))
    {
        HI_ERR_SPI("ssp readex copy data from user fail\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    if (HI_DRV_SPI_RW_LOOP(EXData.devId, pData, EXData.sDataCnt, rData, EXData.rDataCnt)) {
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_READ_TIMEOUT;
    }

    if (copy_to_user((hi_u8 *)compat_ptr(EXData.rData), rData, EXData.rDataCnt))
    {
        HI_ERR_SPI("ssp readex copy data to user fail!\n");
        HI_KFREE(HI_ID_SPI, pData);
        HI_KFREE(HI_ID_SPI, rData);
        return HI_ERR_SPI_COPY_DATA_ERR;
    }

    HI_KFREE(HI_ID_SPI, pData);
    HI_KFREE(HI_ID_SPI, rData);

    return HI_SUCCESS;
}
#endif


hi_s32 HI_DRV_SPI_Ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_s32 error = 0;
    SPI_FFORM_S SSPFForm = {0};
    SPI_BLEND_S SSPBLEND = {0};
    SPI_LOOP_S  SSPLOOP = {0};
    hi_size_t *pDevId = HI_NULL;

    if (file == HI_NULL || file->private_data == HI_NULL) {
        HI_ERR_SPI("pointer file is NULL! \n");
        return  HI_FAILURE;
    }

    switch (cmd) {
        case CMD_SPI_OPEN: {
            error = hi_drv_spi_open(arg);
            if (error) {
                HI_ERR_SPI("spi open failed:%d\n", error);
            } else {
                pDevId = file->private_data;
                if ((pDevId != HI_NULL) && (arg < MAX_SPI_CHANNEL)) {
                    pDevId[arg] = 1;
                }
            }
            break;
        }
        case CMD_SPI_CLOSE: {
            error = hi_drv_spi_close(arg);
            if (error) {
                HI_ERR_SPI("spi close failed:%d\n", error);
            } else {
                pDevId = file->private_data;
                if ((pDevId != HI_NULL) && (arg < MAX_SPI_CHANNEL)) {
                    pDevId[arg] = 0;
                }
            }
            break;
        }
        case CMD_SPI_WRITE: {
            error = SPI_Write(arg);
                break;
        }
        case CMD_SPI_READ: {
            error = SPI_Read(arg);
            break;
        }
        case CMD_SPI_READEX: {
            error = SPI_ReadEx(arg);
            break;
        }
        case CMD_SPI_SET_ATTR: {
            if ((SPI_FFORM_S __user *)(hi_size_t)arg == HI_NULL) {
                HI_ERR_SPI("Pointer is NULL ! \n");
                error = HI_FAILURE;
                break;
            }
            if (copy_from_user(&SSPFForm, (SPI_FFORM_S __user *)(hi_size_t)arg, sizeof(SPI_FFORM_S))) {
                HI_ERR_SPI("ssp setattr copy data from user fail\n");
                error = HI_FAILURE;
                break;
            }
            if (SSPFForm.cscfg) {
                hi_drv_spi_set_cs_config(1);
            } else {
                hi_drv_spi_set_cs_config(0);
            }

            hi_drv_spi_set_cs_gpio(SSPFForm.devId);
            error = hi_drv_spi_set_from(SSPFForm.devId, SSPFForm.mode,SSPFForm.spo,
                SSPFForm.sph,SSPFForm.dss);
            break;
        }
        case CMD_SPI_GET_ATTR: {
            if ((SPI_FFORM_S __user *)(hi_size_t)arg == HI_NULL) {
                HI_ERR_SPI("pointer is NULL ! \n");
                error = HI_FAILURE;
                break;
            }
            if (copy_from_user(&SSPFForm, (SPI_FFORM_S __user *)(hi_size_t)arg, sizeof(SPI_FFORM_S))) {
                HI_ERR_SPI("ssp getattr copy data from user fail\n");
                error = HI_FAILURE;
                break;
            }

            SSPFForm.cscfg = hi_drv_spi_get_cs_config();
            error = hi_drv_spi_get_from(SSPFForm.devId,&(SSPFForm.mode),&(SSPFForm.spo),
                                       &(SSPFForm.sph),&(SSPFForm.dss));
            if (error == HI_SUCCESS) {
                if (copy_to_user((SPI_FFORM_S __user *)(hi_size_t)arg, &SSPFForm,sizeof(SPI_FFORM_S))) {
                    HI_ERR_SPI("ssp getattr copy data to user fail\n");
                    error = HI_ERR_SPI_COPY_DATA_ERR;
                }
            } else {
                HI_ERR_SPI("ssp hi_drv_spi_get_from fail\n");
            }

            break;
        }
        case CMD_SPI_SET_BLEND: {
            if ((SPI_BLEND_S __user *)(hi_size_t)arg == HI_NULL) {
                HI_ERR_SPI("pointer is NULL ! \n");
                error = HI_FAILURE;
                break;
            }

            if (copy_from_user(&SSPBLEND, (SPI_BLEND_S __user *)(hi_size_t)arg, sizeof(SPI_BLEND_S))) {
                HI_ERR_SPI("ssp set blend copy data from user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
                break;
            }

            hi_drv_spi_set_blend(SSPBLEND.devId,SSPBLEND.set_bend);
            break;
        }
        case CMD_SPI_GET_BLEND: {
            if ((SPI_BLEND_S __user *)(hi_size_t)arg == HI_NULL) {
                HI_ERR_SPI("pointer is NULL ! \n");
                error = HI_FAILURE;
                break;
            }

            if (copy_from_user(&SSPBLEND, (SPI_BLEND_S __user *)(hi_size_t)arg, sizeof(SPI_BLEND_S))) {
                HI_ERR_SPI("ssp get blend copy data from user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
                break;
            }

            SSPBLEND.set_bend = hi_drv_spi_get_blend(SSPBLEND.devId);
            if (copy_to_user((SPI_BLEND_S __user *)(hi_size_t)arg, &SSPBLEND,sizeof(SPI_BLEND_S))) {
                HI_ERR_SPI("ssp get blend copy data to user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
            }
            break;
        }

        case CMD_SPI_SET_LOOP: {
            if ((SPI_LOOP_S __user *)(hi_size_t)arg == HI_NULL) {
                HI_ERR_SPI("pointer is NULL ! \n");
                error = HI_FAILURE;
                break;
            }

            if (copy_from_user(&SSPLOOP, (SPI_LOOP_S __user *)(hi_size_t)arg, sizeof(SPI_LOOP_S))) {
                HI_ERR_SPI("ssp get loop from user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
                break;
            }

            HI_DRV_SPI_SetLoop(SSPLOOP.devId, SSPLOOP.setLoop);
            break;
        }

        case CMD_SPI_RW_LOOP: {
            error = SPI_RW_Loop(arg);
            break;
        }

        default:
            return -ENOIOCTLCMD;
    }

    return error;
}

#ifdef CONFIG_COMPAT
hi_s32 HI_DRV_SPI_Compat_Ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_s32 error = 0;
    SPI_FFORM_S SSPFForm = {0};
    SPI_BLEND_S SSPBLEND = {0};
    SPI_LOOP_S  SSPLOOP = {0};
    hi_size_t *pDevId = HI_NULL;

    if (file == HI_NULL || file->private_data == HI_NULL) {
        HI_ERR_SPI("pointer file is NULL! \n");
        return  HI_FAILURE;
    }

    switch (cmd) {
        case CMD_SPI_OPEN: {
            error = hi_drv_spi_open(arg);
            if (error) {
                HI_ERR_SPI("spi open failed:%d\n", error);
            } else {
                pDevId = file->private_data;
                if ((pDevId != HI_NULL) && (arg < MAX_SPI_CHANNEL)) {
                    pDevId[arg] = 1;
                }
            }
            break;
        }
        case CMD_SPI_CLOSE: {
            error = hi_drv_spi_close(arg);
            if (error) {
                HI_ERR_SPI("spi close failed:%d\n", error);
            } else {
                pDevId = file->private_data;
                if ((pDevId != HI_NULL) && (arg < MAX_SPI_CHANNEL)) {
                    pDevId[arg] = 0;
                }
            }
            break;
        }
        case CMD_SPI_WRITE: {
            error = SPI_Compat_Write(arg);
            break;
        }
        case CMD_SPI_READ: {
            error = SPI_Compat_Read(arg);
            break;
        }
        case CMD_SPI_READEX: {
            error = SPI_Compat_ReadEx(arg);
            break;
        }
        case  CMD_SPI_SET_ATTR: {
           SPI_FFORM_S __user *pSSPFForm =  compat_ptr(arg);
           if(pSSPFForm == HI_NULL) {
                HI_ERR_SPI("invalid para\n");
                error = HI_FAILURE;
                break;
            }
            if (copy_from_user(&SSPFForm, pSSPFForm, sizeof(SPI_FFORM_S))) {
                HI_ERR_SPI("ssp setattr copy data from user fail\n");
                error = HI_FAILURE;
                break;
            }
            if (SSPFForm.cscfg) {
                hi_drv_spi_set_cs_config(1);
            } else {
                hi_drv_spi_set_cs_config(0);
            }

            hi_drv_spi_set_cs_gpio(SSPFForm.devId);
            error = hi_drv_spi_set_from(SSPFForm.devId, SSPFForm.mode,SSPFForm.spo,
                                        SSPFForm.sph,SSPFForm.dss);
            break;
        }
        case  CMD_SPI_GET_ATTR: {
            SPI_FFORM_S __user *pSSPFForm =  compat_ptr(arg);
            if(pSSPFForm == HI_NULL) {
                HI_ERR_SPI("invalid para\n");
                error = HI_FAILURE;
                break;
            }
            if (copy_from_user(&SSPFForm, pSSPFForm, sizeof(SPI_FFORM_S))) {
                HI_ERR_SPI("ssp getattr copy data from user fail\n");
                error = HI_FAILURE;
                break;
            }

            SSPFForm.cscfg = hi_drv_spi_get_cs_config();
            error = hi_drv_spi_get_from(SSPFForm.devId,&(SSPFForm.mode),&(SSPFForm.spo),
                                       &(SSPFForm.sph),&(SSPFForm.dss));
            if (HI_SUCCESS == error) {
                if (copy_to_user(pSSPFForm, &SSPFForm,sizeof(SPI_FFORM_S))) {
                    HI_ERR_SPI("ssp getattr copy data to user fail\n");
                    error = HI_ERR_SPI_COPY_DATA_ERR;
                }
            } else {
                HI_ERR_SPI("ssp hi_drv_spi_get_from fail\n");
            }
            break;
        }
        case CMD_SPI_SET_BLEND: {
            SPI_BLEND_S __user *pSSPBLEND =  compat_ptr(arg);
            if(pSSPBLEND == HI_NULL) {
                HI_ERR_SPI("invalid para\n");
                error = HI_FAILURE;
                break;
            }
            if (copy_from_user(&SSPBLEND, pSSPBLEND, sizeof(SPI_BLEND_S))) {
                HI_ERR_SPI("ssp set blend copy data from user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
                break;
            }
            hi_drv_spi_set_blend(SSPBLEND.devId,SSPBLEND.set_bend);
            break;
        }
        case CMD_SPI_GET_BLEND: {
            SPI_BLEND_S __user *pSSPBLEND =  compat_ptr(arg);
            if(pSSPBLEND == HI_NULL) {
                HI_ERR_SPI("invalid para\n");
                error = HI_FAILURE;
                break;
            }
            if (copy_from_user(&SSPBLEND, pSSPBLEND, sizeof(SPI_BLEND_S))) {
                HI_ERR_SPI("ssp get blend copy data from user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
                break;
            }
            SSPBLEND.set_bend = hi_drv_spi_get_blend(SSPBLEND.devId);
            if (copy_to_user(pSSPBLEND, &SSPBLEND,sizeof(SPI_BLEND_S))) {
                HI_ERR_SPI("ssp get blend copy data to user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
            }
            break;
        }
        case CMD_SPI_SET_LOOP: {
            SPI_LOOP_S __user *pSSPLOOP =  compat_ptr(arg);
            if(pSSPLOOP == HI_NULL) {
                HI_ERR_SPI("invalid para\n");
                error = HI_FAILURE;
                break;
            }
            if (copy_from_user(&SSPLOOP, pSSPLOOP, sizeof(SPI_LOOP_S))) {
                HI_ERR_SPI("ssp get loop from user fail\n");
                error = HI_ERR_SPI_COPY_DATA_ERR;
                break;
            }
            HI_DRV_SPI_SetLoop(SSPLOOP.devId, SSPLOOP.setLoop);
            break;
        }
        case CMD_SPI_RW_LOOP: {
            error = SPI_Compat_RW_Loop(arg);
            break;
        }
        default:
            return -ENOIOCTLCMD;
    }
        return error;
}
#endif

static hi_u32 get_spi_dev_id(hi_char *devname)
{
    hi_u32 id = 0;

    if (devname == HI_NULL) {
        HI_ERR_SPI("get_spi_dev_id error:%s\n", devname);
        return 0;
    }

    if (sscanf_s(devname, "hi_spi_%u", &id) != 1) {
        HI_ERR_SPI("get_spi_dev_id sscanf error:%s\n", devname);
        return 0;
    }

    return id;
}

static hi_s32 hi_spi_probe(struct spi_device *spi)
{
    hi_u32 id = 0;

    if (spi == HI_NULL || spi->modalias == HI_NULL) {
        HI_ERR_SPI("pointer spi is NULL! \n");
        return -1;
    }

    id = get_spi_dev_id(spi->modalias);

    if (id >= MAX_SPI_CHANNEL || spi == HI_NULL) {
        HI_ERR_SPI("hi_spi_probe failed, id:%d\n", id);
        return -1;
    }

    if (down_interruptible(&spi_dummy[id].lock)){
        HI_ERR_SPI("hi_spi_probe lock failed\n");
        return -ERESTARTSYS;
    }

    spi_dummy[id].spi_dev= spi;
    up(&spi_dummy[id].lock);

    return 0;
}

static hi_s32 hi_spi_remove(struct spi_device *spi)
{
    hi_u32 id = 0;

    if (spi == HI_NULL) {
        HI_ERR_SPI("pointer spi is NULL! \n");
        return -1;
    }
    id = get_spi_dev_id(spi->modalias);

    if (id >= MAX_SPI_CHANNEL || spi == HI_NULL) {
        HI_ERR_SPI("hi_spi_remove failed, id:%d\n", id);
        return -1;
    }

    if (down_interruptible(&spi_dummy[id].lock)) {
        HI_ERR_SPI("hi_spi_remove lock failed\n");
        return -ERESTARTSYS;
    }

    spi_dummy[id].spi_dev= NULL;
    up(&spi_dummy[id].lock);

    return 0;
}

static struct spi_device_id hi_spi_drv_ids[] = {
    { "hi_spi_0",   0 },
    { "hi_spi_1",   0 },
    {},
};

static struct spi_driver hi_spi_driver = {
    .driver        = {
        .name    = "hi_spi",
        .owner    = THIS_MODULE,
    },
    .id_table     = hi_spi_drv_ids,
    .probe        = hi_spi_probe,
    .remove       = hi_spi_remove,
};

hi_s32 hi_drv_spi_open(hi_u8 dev_id)
{
    struct spi_master *spi_master = NULL;
    struct spi_device *spi_device = NULL;
    struct spi_board_info *chip = NULL;
    hi_s32 error = 0;

    if (dev_id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", dev_id);
        return -1;
    }

    spi_master = spi_busnum_to_master(dev_id);
    if (spi_master == HI_NULL){
        HI_ERR_SPI("spi_busnum_to_master(%d) returned NULL\n", dev_id);
        return -ENODEV;
    }

    spi_master->rt = true;
    spi_device = spi_alloc_device(spi_master);
    if (spi_device == HI_NULL) {
        HI_ERR_SPI("spi_alloc_device returned NULL\n");
        error = -ENOMEM;
        goto spi_master_fail;
    }

    chip = &spi_dummy[dev_id].chip;
    spi_device->controller_data  = &spi_dummy[dev_id].pl022_config_info;
    spi_device->max_speed_hz     = chip->max_speed_hz;
    spi_device->chip_select      = chip->chip_select;
    spi_device->mode             = chip->mode;
    spi_device->irq              = chip->irq;
    spi_device->bits_per_word    = spi_dummy[dev_id].bits_per_word;
    spi_device->controller_state = NULL;

    snprintf_s(spi_device->modalias, SPI_NAME_SIZE, SPI_NAME_SIZE, "hi_spi_%d", dev_id);

    error = spi_add_device(spi_device);
    if (error < 0) {
        HI_ERR_SPI("spi_add_device returned %d\n", error);
        goto spi_dev_fail;
    }
    spi_master_put(spi_master);

    return 0;

spi_dev_fail:
    spi_dev_put(spi_device);

spi_master_fail:
    spi_master_put(spi_master);

    return error;
}

hi_s32 hi_drv_spi_close(hi_u8 dev_id)
{
    if (dev_id >= MAX_SPI_CHANNEL) {
        HI_ERR_SPI("spi channel %d not support\n", dev_id);
        return -1;
    }

    if (spi_dummy[dev_id].spi_dev)
        spi_unregister_device(spi_dummy[dev_id].spi_dev);
    return HI_SUCCESS;
}

hi_s32 hi_drv_spi_init(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    hi_s32 i = 0;
    hi_s32 error = 0;
    struct pl022_config_chip *pl022 = NULL;
    struct spi_board_info *chip = NULL;

    if (atomic_inc_return(&spi_init_counter) != 1) {
        HI_WARN_SPI("HI_DRV_SPI already registered:%d\n",
                     atomic_read(&spi_init_counter));
        return HI_SUCCESS;
    }

    ret = hi_drv_module_register(HI_ID_SPI, "HI_SPI", HI_NULL, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_SPI("register failed 0x%x.\n", ret);
        return HI_FAILURE;
    }

    error = spi_register_driver(&hi_spi_driver);
    if (error < 0) {
        HI_ERR_SPI("spi_register_driver returned %d\n", error);
        return error;
    }

    for (i=0; i < MAX_SPI_CHANNEL; i++) {
        sema_init(&spi_dummy[i].lock, 1);
        pl022 = &spi_dummy[i].pl022_config_info;
        pl022->com_mode = POLLING_TRANSFER;
        pl022->iface = SSP_INTERFACE_MOTOROLA_SPI;
        pl022->hierarchy = SSP_MASTER;
        pl022->slave_tx_disable = 0,
        pl022->rx_lev_trig = SSP_RX_1_OR_MORE_ELEM,
        pl022->tx_lev_trig = SSP_TX_1_OR_MORE_EMPTY_LOC,
        pl022->ctrl_len = SSP_BITS_8,
        pl022->wait_state = SSP_MWIRE_WAIT_ZERO,
        pl022->duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
        pl022->clk_freq.cpsdvsr = 0x4;
        pl022->clk_freq.scr = 0x1;
        chip = &spi_dummy[i].chip;
        chip->max_speed_hz = 16000000;
        chip->chip_select = i;
        chip->mode = SPI_MODE_0;
        chip->irq = -1;
        spi_dummy[i].bits_per_word = 8;
    }
    pr_info("hi_drv_spi_init success\n");

    return ret;
}

hi_s32 hi_drv_spi_deinit(hi_void)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;

    if (!atomic_dec_and_test(&spi_init_counter)) {
        HI_WARN_SPI("hi_drv_spi_deinit counter:%d\n",
                     atomic_read(&spi_init_counter));
        return -1;
    }

    for (i=0; i < MAX_SPI_CHANNEL; i++) {
        if (spi_dummy[i].spi_dev)
            spi_unregister_device(spi_dummy[i].spi_dev);
    }

    spi_unregister_driver(&hi_spi_driver);

    ret = hi_drv_module_unregister(HI_ID_SPI);
    if (HI_SUCCESS != ret) {
        HI_ERR_SPI("SPI Module unregister failed 0x%x.\n", ret);
        return HI_FAILURE;
    }

    pr_info("hi_drv_spi_deinit success\n");
    return ret;
}

EXPORT_SYMBOL(hi_drv_spi_init);
EXPORT_SYMBOL(hi_drv_spi_deinit);
EXPORT_SYMBOL(hi_drv_spi_open);
EXPORT_SYMBOL(hi_drv_spi_close);
EXPORT_SYMBOL(hi_drv_spi_set_blend);
EXPORT_SYMBOL(hi_drv_spi_get_cs_config);
EXPORT_SYMBOL(hi_drv_spi_set_cs_config);
EXPORT_SYMBOL(hi_drv_spi_set_cs_gpio);
EXPORT_SYMBOL(hi_drv_spi_set_cs_level);
EXPORT_SYMBOL(hi_drv_spi_read_ex);
EXPORT_SYMBOL(hi_drv_spi_read_ex_isr);
EXPORT_SYMBOL(hi_drv_spi_write_isr);
EXPORT_SYMBOL(hi_drv_spi_write_query);
EXPORT_SYMBOL(hi_drv_spi_read_query);
EXPORT_SYMBOL(hi_drv_spi_read_isr);
EXPORT_SYMBOL(hi_drv_spi_set_from);
EXPORT_SYMBOL(hi_drv_spi_set_clk);
