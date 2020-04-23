/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hitxv300 ddc module main source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>
#include "drv_hdmitx.h"
#include "hal_hdmitx_io.h"
#include "hal_hdmitx_ddc.h"
#include "hal_hdmitx_ctrl_reg.h"
#include "hal_hdmitx_aon_reg.h"

#define TRANSFER_START       0
#define TRANSFER_SUCCESS     1
#define WAIT_BUS_IDLE_FAILED 2
#define WAIT_SCL_HIGH_FAILED 3
#define WAIT_SDA_HIGH_FAILED 4
#define WAIT_RFIFO_FAILED    5
#define WAIT_WFIFO_FAILED    6
#define LOCK_HW_BUS_FAILED   7
#define UNLOCK_HW_BUS_FAILED 8
#define BUS_LOW_ERROR        9
#define SLAVE_NO_ACK         10
#define WAIT_PROG_TIMEOUT    11
#define TRANSFER_FAILED      12

static const char *const g_event_msg[] = {
    "start transfer",
    "transfer success",
    "wait bus idle failed",
    "wait scl high failed",
    "wait sda high failed",
    "wait rfifo ready failed",
    "wait wfifo ready failed",
    "lock hw bus timeout",
    "unlock hw bus timeout",
    "bus low error",
    "slave no ack",
    "wait prog finished timeout",
    "transfer failed"
};

static inline u32 ddc_fifo_count(struct hisilicon_hdmi *hdmi)
{
    u32 value;

    value = hdmi_readl(hdmi->hdmi_regs, REG_PWD_DATA_CNT);
    value = (value & REG_PWD_FIFO_DATA_CNT_M);

    return value;
}

static bool ddc_rfifo_is_empty(struct hisilicon_hdmi *hdmi)
{
    u32 value;
    /*
     * Check the fifo empty status bit first, and than check the fifo
     * count as a double check.
     */
    value = hdmi_readl(hdmi->hdmi_regs, REG_PWD_MST_STATE);
    value = (value & REG_PWD_FIFO_EMPTY_M) >> 5; /* bit5: pwd_fifo_empty */
    if (value == 0x1) {
        return true;
    }

    value = ddc_fifo_count(hdmi);
    if (value == 0) {
        return true;
    }

    return false;
}

static bool ddc_fifo_is_full(struct hisilicon_hdmi *hdmi)
{
    u32 value;

    value = ddc_fifo_count(hdmi);

    return value >= DDC_MAX_FIFO_SIZE;
}

static bool ddc_bus_is_err(struct hisilicon_hdmi *hdmi)
{
    u32 value;

    /*
     * If the slave response with no ack, or bus is busy
     * ,then we abort the transfer.
     * for our usecase, when we are connected to a tv and the bus is busy
     * indicates an abnornal case.
     */
    value = hdmi_readl(hdmi->hdmi_aon_regs, REG_DDC_MST_STATE);
    value &= REG_DDC_I2C_NO_ACK_M;
    if (value == 0x1) {
        goto err_clear;
    }

    value = hdmi_readl(hdmi->hdmi_aon_regs, REG_DDC_MST_STATE);
    value = (value & REG_DDC_I2C_BUS_LOW_M) >> 1;
    if (value == 0x1) {
        goto err_clear;
    }

    return false;
err_clear:
    hdmi_clrset(hdmi->hdmi_regs, REG_PWD_MST_CMD,
                REG_PWD_MST_CMD_M, reg_pwd_mst_cmd_f(CMD_MASTER_ABORT));
    return true;
}

static inline u32 ddc_sda_level_get(struct hisilicon_hdmi *hdmi)
{
    u32 value;

    value = hdmi_readl(hdmi->hdmi_aon_regs, REG_DDC_MAN_CTRL);
    value = (value & REG_DDC_SDA_ST_M) >> 1;
    return value;
}

static inline u32 ddc_scl_level_get(struct hisilicon_hdmi *hdmi)
{
    u32 value;

    value = hdmi_readl(hdmi->hdmi_aon_regs, REG_DDC_MAN_CTRL);
    value = (value & REG_DDC_SCL_ST_M);

    return value;
}

/*
 * scl should not be high in general, it's very abnornal case,
 * we can only wait some timeout for scl high.
 * just return an error if scl is not high after a wait.
 */
static int ddc_scl_wait_high(struct hisilicon_hdmi *hdmi)
{
    int ret = 0;
    u32 temp_time = 0;
    struct hdmi_ddc *ddc = hdmi->ddc;

    while (!ddc_scl_level_get(hdmi)) {
        osal_msleep(1); /* need sleep 1ms. */
        temp_time += 1;
        if (temp_time > ddc->timeout.scl_timeout) {
            ret = HI_FAILURE;
            break;
        }
    }

    return ret;
}

/*
 * If the sda is low when we are expecting idle, it's mostly possible
 * deadlock due to master/slave reset during i2c transfer.
 * And we need to recover from this case(refer to i2c deadlock for more)
 * Simulate i2c transfer is used.
 */
static int ddc_try_resolve_deadlock(struct hisilicon_hdmi *hdmi)
{
    int ret = 0;
    u32 temp_time = 0;
    struct hdmi_ddc *ddc = hdmi->ddc;

    hdmi_clrset(hdmi->hdmi_aon_regs, REG_DDC_MST_CTRL, REG_DCC_MAN_EN_M,
                reg_dcc_man_en(1));
    /* Generate scl clock util sda is high or timeout */
    while ((!ddc_sda_level_get(hdmi)) && (temp_time++ < ddc->timeout.sda_timeout)) {
        /* pull scl high */
        hdmi_clrset(hdmi->hdmi_aon_regs, REG_DDC_MAN_CTRL, REG_DDC_SCL_OEN_M, reg_ddc_scl_oen(1));
        osal_udelay(8); /* delay 8 microsecond */
        /* pull scl low */
        hdmi_clrset(hdmi->hdmi_aon_regs, REG_DDC_MAN_CTRL, REG_DDC_SCL_OEN_M, reg_ddc_scl_oen(0));
        osal_udelay(8); /* delay 8 microsecond */
    }
    /* STOP contition */
    if (temp_time < ddc->timeout.sda_timeout && ddc_sda_level_get(hdmi)) {
        /* pull sda low */
        hdmi_clrset(hdmi->hdmi_aon_regs, REG_DDC_MAN_CTRL, REG_DDC_SDA_OEN_M, reg_ddc_sda_oen(0));
        osal_udelay(8); /* delay 8 microsecond */
        /* pull scl high */
        hdmi_clrset(hdmi->hdmi_aon_regs, REG_DDC_MAN_CTRL, REG_DDC_SCL_OEN_M, reg_ddc_scl_oen(1));
        osal_udelay(8); /* delay 8 microsecond */
        /* pull sda high */
        hdmi_clrset(hdmi->hdmi_aon_regs, REG_DDC_MAN_CTRL, REG_DDC_SDA_OEN_M, reg_ddc_sda_oen(1));
        osal_udelay(8); /* delay 8 microsecond */

        HDMI_DBG("ddc deadlock clear success\n");
        ret = 0;
    } else {
        HDMI_DBG("ddc deadlock clear  fail\n");
        ret = -ETIMEDOUT;
    }
    hdmi_clrset(hdmi->hdmi_aon_regs, REG_DDC_MST_CTRL, REG_DCC_MAN_EN_M, reg_dcc_man_en(0));

    return ret;
}

static int ddc_sda_wait_high(struct hisilicon_hdmi *hdmi)
{
    if (ddc_sda_level_get(hdmi)) {
        return 0;
    }

    return ddc_try_resolve_deadlock(hdmi);
}

static int ddc_wait_for_bus_idle(struct hisilicon_hdmi *hdmi)
{
    int ret;

    ret = ddc_scl_wait_high(hdmi);
    if (ret < 0) {
        return ret;
    }

    ret = ddc_sda_wait_high(hdmi);
    if (ret < 0) {
        return ret;
    }

    return ret;
}

static int ddc_wait_for_rfifo_ready(struct hisilicon_hdmi *hdmi)
{
    u32 rd_tmo;
    int i;
    struct hdmi_ddc *ddc = hdmi->ddc;

    rd_tmo = ddc->timeout.access_timeout;

    for (i = 0; i < rd_tmo; i++) {
        if (ddc_rfifo_is_empty(hdmi)) {
            osal_msleep(1); /* need sleep 1ms. */
            if (ddc_bus_is_err(hdmi)) {
                return HI_FAILURE;
            }
        } else {
            break;
        }
    }

    if (i >= rd_tmo) {
        HDMI_DBG("ddc read fifo timeout=%u!\n", rd_tmo);
        return HI_FAILURE;
    }

    return 0;
}

static int ddc_wait_for_wfifo_ready(struct hisilicon_hdmi *hdmi)
{
    u32 wr_tmo;
    int i;
    struct hdmi_ddc *ddc = hdmi->ddc;

    wr_tmo = ddc->timeout.access_timeout;

    for (i = 0; i < wr_tmo; i++) {
        if (ddc_fifo_is_full(hdmi)) {
            osal_msleep(1); /* need sleep 1ms. */
            if (ddc_bus_is_err(hdmi)) {
                return -ENXIO;
            }
        } else {
            break;
        }
    }

    if (i >= wr_tmo) {
        HDMI_DBG("ddc write fifo timeout = %u!\n", wr_tmo);
        return -ETIMEDOUT;
    }

    return 0;
}

/*
 *  Software can only see the read/write fifo of the ddc controller,
 *  but sometimes it's nessary to make sure the bus transaction is finished.
 *  especially when the ddc bus share by mutiple masters.
 *  with ddc_wait_for_prog_complete, we can make sure that the bus transaction
 *  is finished.
 */
static int ddc_wait_for_prog_complete(struct hisilicon_hdmi *hdmi)
{
    u32 prog_tmo, value;
    int i = 0;
    struct hdmi_ddc *ddc = hdmi->ddc;

    prog_tmo = ddc->timeout.in_prog_timeout;

    while (i < prog_tmo) {
        value = hdmi_readl(hdmi->hdmi_regs, REG_PWD_MST_STATE);
        if (!(value & REG_PWD_I2C_IN_PROG_M)) {
            break;
        }
        i++;
        osal_msleep(1); /* need sleep 1ms. */
    }

    if (i >= prog_tmo) {
        HDMI_DBG("wait prog finish timeout = %u!\n", prog_tmo);
        return -ETIMEDOUT;
    }

    return 0;
}

static int ddc_lock_unlock_helper(struct hisilicon_hdmi *hdmi, bool lock)
{
    u32 status;
    u32 count = 0;
    u32 op = lock ? DDC_MASTER_ENABLE : DDC_MASTER_DISABLE;
    u32 tmo = hdmi->ddc->timeout.access_timeout;

    hdmi_clrset(hdmi->hdmi_regs, REG_DDC_MST_ARB_REQ, REG_CPU_DDC_REQ_M,
                reg_cpu_ddc_req(op));
    while (1) {
        status = hdmi_readl(hdmi->hdmi_regs, REG_DDC_MST_ARB_ACK);
        if (lock && (status & REG_CPU_DDC_REQ_ACK_M)) {
            break;
        } else if (!lock && !(status & REG_CPU_DDC_REQ_ACK_M)) {
            break;
        }

        osal_msleep(1); /* need sleep 1ms. */
        count++;
        if (count > tmo) {
            return -ETIMEDOUT;
        }
    }

    return 0;
}

static int ddc_lock_internel_hw_bus(struct hisilicon_hdmi *hdmi)
{
    return ddc_lock_unlock_helper(hdmi, true);
}

static int ddc_unlock_internal_hw_bus(struct hisilicon_hdmi *hdmi)
{
    return ddc_lock_unlock_helper(hdmi, false);
}

static void ddc_transfer_configure(struct hisilicon_hdmi *hdmi, u16 addr,
                                   u8 slave_reg, bool segment,
                                   u16 len, bool read)
{
    int mode;

    if (read) {
        mode = segment ? MODE_READ_SEGMENT_NO_ACK : MODE_READ_MUTIL_NO_ACK;
    } else {
        mode = MODE_WRITE_MUTIL_NO_ACK;
    }

    /* Clear DDC FIFO, it's recommanded */
    hdmi_clrset(hdmi->hdmi_regs, REG_PWD_MST_CMD, REG_PWD_MST_CMD_M,
                reg_pwd_mst_cmd_f(CMD_FIFO_CLR));
    /* Set Slave Address */
    hdmi_clrset(hdmi->hdmi_regs, REG_PWD_SLAVE_CFG, REG_PWD_SLAVE_ADDR_M,
                reg_pwd_slave_addr(addr));
    /* Set Slave Offset */
    hdmi_clrset(hdmi->hdmi_regs, REG_PWD_SLAVE_CFG, REG_PWD_SLAVE_OFFSET_M,
                reg_pwd_slave_offset(slave_reg));
    /* Set Slave Segment */
    hdmi_clrset(hdmi->hdmi_regs, REG_PWD_SLAVE_CFG, REG_PWD_SLAVE_SEG_M,
                reg_pwd_slave_seg(segment));
    /* Set DDC FIFO Data Cnt */
    hdmi_clrset(hdmi->hdmi_regs, REG_PWD_DATA_CNT, REG_PWD_DATA_OUT_CNT_M,
                reg_pwd_data_out_cnt(len));
    /* Set DDC Master register read mode */
    hdmi_clrset(hdmi->hdmi_regs, REG_PWD_MST_CMD, REG_PWD_MST_CMD_M,
                reg_pwd_mst_cmd_f(mode));
}

/*
 * In some cases, there are something error happens.
 * we need to do cleanup to clear the error status.
 * currently, the following 3 cleanups are done:
 * 1. wait for i2c program finish.
 * 2. wait for the bus idle and resolve the i2c deadlock if it's needed
 * and possible.
 */
static void ddc_errs_cleanup(struct hisilicon_hdmi *hdmi)
{
    /*
     * No more error handling is needed since we are already done
     * everything, we have to ignore.
     */
    (void)ddc_wait_for_prog_complete(hdmi);
    (void)ddc_wait_for_bus_idle(hdmi);
}

static void ddc_timeout_init_default(struct hdmi_ddc *ddc)
{
    ddc->timeout.access_timeout = DDC_DEFAULT_TIMEOUT_ACCESS;
    ddc->timeout.hpd_timeout = DDC_DEFAULT_TIMEOUT_HPD;
    ddc->timeout.in_prog_timeout = DDC_DEFAULT_TIMEOUT_IN_PROG;
    ddc->timeout.scl_timeout = DDC_DEFAULT_TIMEOUT_SCL;
    ddc->timeout.sda_timeout = DDC_DEFAULT_TIMEOUT_SDA;
    ddc->timeout.issue_timeout = DDC_DEFAULT_TIMEOUT_ISSUE;
}

static int ddc_data_read(struct hisilicon_hdmi *hdmi, struct ddc_msg *msg)
{
    int i, ret;
    struct hdmi_ddc *ddc = hdmi->ddc;
    hi_u16 length = msg->len;
    hi_u8 *buf = msg->buf;
    hi_u16 addr = msg->addr;

    /* Default regaddr is 0 */
    if (ddc->is_regaddr == HI_NULL) {
        ddc->slave_reg = 0x00;
        ddc->is_regaddr = true;
    }

    if (ddc->is_segment) {
        ddc->slave_reg = 0;
    }

    ddc_transfer_configure(hdmi, addr, ddc->slave_reg, ddc->is_segment,
                           length, true);
    /* DDC read fifo data */
    for (i = 0; i < length; i++, buf++) {
        ret = ddc_wait_for_rfifo_ready(hdmi);
        if (ret) {
            return ret;
        }

        *buf = hdmi_readl(hdmi->hdmi_regs, REG_PWD_FIFO_RDATA) & REG_PWD_FIFO_DATA_OUT_M;
        /*
         * The hw fifo status is not updated in time after a fifo
         * read/write, so we have to wait 5us to let the hw refresh
         * for the next fifo status
         */
        osal_udelay(5); /* delay 5 microsecond */
    }

    ddc->is_segment = false;
    /*
     *  It's not that nessary to wait for a read complete, since there is
     *  no data on the way when the read fifo finished.
     *  but we still double check it for harden.
     */
    ret = ddc_wait_for_prog_complete(hdmi);

    return ret;
}

/*
 * DDC Write data
 * @hdmi: hisilicon hdmi private structure
 * @msgs: i2c msg
 */
static int ddc_data_write(struct hisilicon_hdmi *hdmi, struct ddc_msg *msgs)
{
    int i, ret;
    u16 length;
    u8 *buf = HI_NULL;
    u16 addr;
    struct hdmi_ddc *ddc = hdmi->ddc;

    length = msgs->len;
    buf = msgs->buf;
    addr = msgs->addr;

    if (!ddc->is_regaddr) {
        /* Use the first write byte as offset */
        ddc->slave_reg = buf[0];
        length--;
        buf++;
        ddc->is_regaddr = true;
    }

    if (length == 0) {
        return 0;
    }

    ddc_transfer_configure(hdmi, addr, ddc->slave_reg, ddc->is_segment,
                           length, false);

    /* DDC fifo write data */
    for (i = 0; i < length; i++, buf++) {
        ret = ddc_wait_for_wfifo_ready(hdmi);
        if (ret) {
            return ret;
        }

        hdmi_clrset(hdmi->hdmi_regs, REG_PWD_FIFO_WDATA, REG_PWD_FIFO_DATA_IN_M,
                    reg_pwd_fifo_data_in(*buf));
        /*
         * The hw fifo status is not updated in time after a fifo
         * read/write, so we have to wait 5us to let the hw refresh
         * for the next fifo status
         */
        osal_udelay(5); /* delay 5 microsecond */
    }

    /*
     * It's mandantory to wait for a write complete, since there maybe
     * still some data on the way when the write fifo is finished.
     * if we return earlier, the fifo could be wrongly cleared by the next
     * transfer.
     */
    ret = ddc_wait_for_prog_complete(hdmi);

    return ret;
}

static hi_s32 hal_ddc_xfer(struct hdmi_ddc *ddc, struct ddc_msg *msgs, hi_u8 num)
{
    hi_s32 i;
    hi_s32 ret;
    struct hisilicon_hdmi *hdmi = HI_NULL;

    hdmi = ddc->parent;
    if (hdmi == HI_NULL) {
        HDMI_ERR("hdmi is null.\n");
        return HI_FAILURE;
    }

    osal_mutex_lock(&ddc->lock);
    /*
     * Lock the DDC hardware bus
     * The DDC hardware bus is shared by cpu/mcu/hdcp, so it's required to lock the
     * bus before access
     */
    ret = ddc_lock_internel_hw_bus(hdmi);
    if (ret) {
        goto hw_lock_failed;
    }
    /* wait the bus idle and resolve the i2c deadlock if possible */
    ret = ddc_wait_for_bus_idle(hdmi);
    if (ret) {
        goto out;
    }

    /* Set slave device register address on transfer */
    ddc->is_regaddr = false;
    /* Set segment pointer for I2C extended read mode operation */
    ddc->is_segment = false;

    for (i = 0; i < num; i++) {
        if (msgs[i].addr == DDC_SEGMENT_ADDR && msgs[i].len == 1) {
            ddc->is_segment = true;
            continue;
        }

        if (msgs[i].flags & DDC_M_RD) {
            ret = ddc_data_read(hdmi, &msgs[i]);
        } else {
            ret = ddc_data_write(hdmi, &msgs[i]);
        }

        if (ret < 0) {
            break;
        }
    }

    if (ret) {
        ddc_errs_cleanup(hdmi);
    }

out:
    /* Just print the error but no way to handle unlock error */
    if (ddc_unlock_internal_hw_bus(hdmi)) {
        HDMI_ERR("ddc_unlock_internal_hw_bus err\n");
    }
hw_lock_failed:
    osal_mutex_unlock(&ddc->lock);

    return ret;
}

hi_s32 hal_ddc_transfer(struct hdmi_ddc *ddc, struct ddc_msg *msgs, hi_u8 num, hi_u8 retry)
{
    hi_s32 i;
    hi_s32 ret;

    if (ddc == HI_NULL || msgs == HI_NULL) {
        HDMI_ERR("ddc uninit or msg is null.\n");
        return HI_FAILURE;
    }

    for (i = 0; i < num; i++) {
        if (msgs[i].len == 0) {
            HDMI_ERR("unsupported transfer %d/%d, no data\n", i + 1, num);
            return HI_FAILURE;
        }
    }

    do {
        ret = hal_ddc_xfer(ddc, msgs, num);
    } while ((retry-- > 0) && (ret < 0));

    return ret;
}

hi_s32 hal_ddc_init(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_ddc *ddc = HI_NULL;

    if (hdmi == HI_NULL) {
        HDMI_ERR("ptr is null.\n");
        return HI_FAILURE;
    }

    ddc = osal_kmalloc(HI_ID_HDMITX, sizeof(struct hdmi_ddc), OSAL_GFP_KERNEL);
    if (ddc == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_FAILURE;
    }

    if (memset_s(ddc, sizeof(struct hdmi_ddc), 0, sizeof(struct hdmi_ddc))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, ddc);
        return HI_FAILURE;
    }

    hdmi->ddc = ddc;
    ddc->parent = hdmi;
    osal_mutex_init(&ddc->lock);

    ddc_timeout_init_default(ddc);

    return HI_SUCCESS;
}

void hal_ddc_deinit(struct hisilicon_hdmi *hdmi)
{
    if ((hdmi != HI_NULL) && (hdmi->ddc != HI_NULL)) {
        osal_kfree(HI_ID_HDMITX, hdmi->ddc);
    }

    hdmi->ddc = HI_NULL;
}

