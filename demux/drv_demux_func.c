/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-05-31
 */
#include <linux/kernel.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/printk.h"
#include "linux/ratelimit.h"
#include "linux/kthread.h"
#include "linux/hisilicon/securec.h"
#include "linux/hisilicon/hi_license.h"

#include "hi_type.h"
#include "hi_osal.h"
#include "hi_drv_sys.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_drv_file.h"
#include "hi_drv_osal.h"
#include "hi_drv_keyslot.h"

#include "drv_demux_func.h"
#include "drv_demux_rmxfct.h"
#include "drv_demux_plyfct.h"
#include "drv_demux_recfct.h"
#include "drv_demux_dscfct.h"
#include "drv_demux_index.h"
#include "drv_demux_scd.h"
#include "drv_demux_tee.h"
#include "drv_demux_define.h"
#include "drv_demux_utils.h"
#include "hi_drv_demux.h"
#include "hal_demux.h"

hi_void dmx_cls_buf_ap_status(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    hi_u32 hw_write = 0;
    hi_u32 queue_len = 0;
    hi_u32 port_offset = 0;

    if (dmx_hal_buf_get_raw_ap_int_status(mgmt, rbuf->base.id)) {
        if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
            port_offset = rbuf->port - DMX_RAM_PORT_0;
        } else if (rbuf->port >= DMX_TSIO_PORT_0 && rbuf->port < DMX_TSIO_PORT_MAX) {
            port_offset = rbuf->port - DMX_TSIO_PORT_0;
        } else {
            HI_WARN_DEMUX("this kind port[%#x] don't support cls ap status!\n", rbuf->port);
            return;
        }

        hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
        queue_len = dmx_get_queue_lenth(rbuf->sw_read, hw_write, rbuf->buf_size);
        if (queue_len + BUF_AP_THRESHOLD + DMX_TS_PKT_SIZE < rbuf->buf_size) {
            dmx_hal_buf_cls_ap_int_status(mgmt, rbuf->base.id);
            if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
                dmx_hal_ram_port_cls_ap_status(mgmt, port_offset);
            } else {
                dmx_hal_buf_clr_tsio_ap(mgmt, port_offset);
            }
        }
    }

    return;
}

hi_void dmx_cls_pcbuf_ap_status(struct dmx_mgmt *mgmt, struct dmx_r_pid_ch *rpid_ch)
{
    hi_u32 hw_write = 0;
    hi_u32 hw_read = 0;
    hi_u32 queue_len = 0;
    hi_u32 port_offset = 0;
    struct dmx_r_band *rband = HI_NULL;

    DMX_NULL_POINTER_VOID(rpid_ch);
    rband = rpid_ch->rband;
    DMX_NULL_POINTER_VOID(rband);

    osal_mutex_lock(&rpid_ch->lock);
    if (rpid_ch->pid_copy_en_status != HI_TRUE) {
        osal_mutex_unlock(&rpid_ch->lock);
        HI_WARN_DEMUX("pid[%#x] channel doesn't enable pid copy!\n", rpid_ch->pid);
        return;
    }

    if (rband->port >= DMX_TSIO_PORT_0 && rband->port < DMX_TSIO_PORT_MAX) {
        port_offset = rband->port - DMX_TSIO_PORT_0;
        if (dmx_hal_buf_get_pc_tsio_ap_status(mgmt, port_offset) != 0) {
            hw_write = dmx_hal_pid_copy_get_cur_write_idx(mgmt, rpid_ch->pid_copy_chan_id);
            hw_read = dmx_hal_pid_copy_get_cur_write_idx(mgmt, rpid_ch->pid_copy_chan_id);
            queue_len = dmx_get_queue_lenth(hw_read, hw_write, rpid_ch->pid_copy_buf_size);
            if (queue_len + PCBUF_AP_THRESHOLD + DMX_TS_PKT_SIZE < rpid_ch->pid_copy_buf_size) {
                dmx_hal_pid_copy_cls_ap_status(mgmt, rpid_ch->pid_copy_chan_id);
                dmx_hal_buf_clr_tsio_ap(mgmt, port_offset);
            }
        }
        osal_mutex_unlock(&rpid_ch->lock);
        return;
    }

    if (rband->port >= DMX_RAM_PORT_0 && rband->port < DMX_ANY_RAM_PORT) {
        port_offset = rband->port - DMX_RAM_PORT_0;
    } else {
        osal_mutex_unlock(&rpid_ch->lock);
        HI_WARN_DEMUX("this kind port[%#x] don't support cls ap status!\n", rband->port);
        return;
    }

    if (dmx_hal_pid_copy_get_raw_ap_ram_int_status(mgmt, port_offset)) {
        hw_write = dmx_hal_pid_copy_get_cur_write_idx(mgmt, rpid_ch->pid_copy_chan_id);
        hw_read = dmx_hal_pid_copy_get_cur_write_idx(mgmt, rpid_ch->pid_copy_chan_id);
        queue_len = dmx_get_queue_lenth(hw_read, hw_write, rpid_ch->pid_copy_buf_size);
        if (queue_len + PCBUF_AP_THRESHOLD + DMX_TS_PKT_SIZE < rpid_ch->pid_copy_buf_size) {
            dmx_hal_pid_copy_cls_ap_ram_int_status(mgmt, port_offset);
            dmx_hal_pid_copy_cls_ap_status(mgmt, rpid_ch->pid_copy_chan_id);
            dmx_hal_ram_port_cls_ap_status(mgmt, port_offset);
        }
    }
    osal_mutex_unlock(&rpid_ch->lock);

    return;
}

static hi_void dmx_hal_process_par_int(struct dmx_mgmt *mgmt)
{
    hi_u32 flag;
    hi_u32 status;
    hi_size_t lock_flag;
    struct list_head *node = HI_NULL;

    HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
    flag = dmx_hal_get_par_int_type(mgmt);
    if (flag & DMX_PAR_TEI_INT_MASK) {
        status = dmx_hal_par_get_tei_int_status(mgmt);
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_par_cls_tei_int_status(mgmt, status);

        osal_spin_lock_irqsave(&mgmt->pid_channel_list_lock2, &lock_flag);
        osal_spin_unlock_irqrestore(&mgmt->pid_channel_list_lock2, &lock_flag);
    }

    if (flag & DMX_PAR_CC_INT_MASK) {
        status = dmx_hal_par_get_cc_int_status(mgmt);
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_par_cls_cc_int_status(mgmt, status);

        osal_spin_lock_irqsave(&mgmt->pid_channel_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->pid_channel_head) {
            struct dmx_r_pid_ch *pid_ch = list_entry(node, struct dmx_r_pid_ch, node0);

            if (status & (1 << pid_ch->base.id)) {
                pid_ch->cc_disc_err++;
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->pid_channel_list_lock2, &lock_flag);
    }
}

static hi_void dmx_hal_process_dav_int(struct dmx_mgmt *mgmt)
{
    hi_u32 flag;
    hi_u32 status;
    hi_size_t lock_flag;
    struct list_head *node;

    /* mask all dav interrupt */
    dmx_hal_mask_all_dav_int(mgmt);
    HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
    flag = dmx_hal_get_dav_int_flag(mgmt);
    /* buffer pes head or section interrupt */
    if (flag & DMX_DAV_SEOP_INT_MASK) {
        dmx_hal_clr_dav_seop_int_flag(mgmt);
        osal_spin_lock_irqsave(&mgmt->buf_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->buf_head) {
            struct dmx_r_buf *rbuf = list_entry(node, struct dmx_r_buf, node);

            if (dmx_hal_buf_get_seop_int_status(mgmt, rbuf->base.id)) {
                dmx_hal_buf_cls_seop_int_status(mgmt, rbuf->base.id);
                queue_work(mgmt->dmx_queue, &rbuf->buf_worker);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->buf_list_lock2, &lock_flag);
    }

    /* ram port desc read finish interrupt */
    if (flag & DMX_DAV_FQ_RD_INT_MASK) {
        status = dmx_hal_ram_port_get_dsc_rd_int_status(mgmt);
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_ram_port_cls_dsc_rd_int_status(mgmt, status);

        osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->ram_port_head) {
            struct dmx_r_ram_port *ram_port = list_entry(node, struct dmx_r_ram_port, node);

            if ((1 << TO_RAMPORT_HW_ID(ram_port->base.id)) & status) {
                queue_work(mgmt->dmx_queue, &ram_port->dsc_end_worker);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);
    }

    /* ram port desc check code interrupt */
    if (flag & DMX_DAV_FQ_CHECK_FAILED_INT_MASK) {
        status = dmx_hal_ram_port_get_dsc_tread_int_status(mgmt);

        dmx_hal_ram_port_cls_dsc_tread_int_status(mgmt, status);
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
        /* to do something */
        osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);
    }

    /* buffer timeout interrupt */
    if (flag & DMX_DAV_TIMEOUT_INT_MASK) {
        dmx_hal_clr_dav_timeout_int_flag(mgmt);
        osal_spin_lock_irqsave(&mgmt->buf_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->buf_head) {
            struct dmx_r_buf *rbuf = list_entry(node, struct dmx_r_buf, node);

            if (dmx_hal_buf_get_timeout_int_status(mgmt, rbuf->base.id)) {
                dmx_hal_buf_cls_timeout_int_status(mgmt, rbuf->base.id);
                queue_work(mgmt->dmx_queue, &rbuf->buf_worker);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->buf_list_lock2, &lock_flag);
    }

    /* buffer fix ts cnt interrupt */
    if (flag & DMX_DAV_TS_BUF_INT_MASK) {
        dmx_hal_clr_dav_ts_int_flag(mgmt);
        osal_spin_lock_irqsave(&mgmt->buf_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->buf_head) {
            struct dmx_r_buf *rbuf = list_entry(node, struct dmx_r_buf, node);

            if (dmx_hal_buf_get_ts_cnt_int_status(mgmt, rbuf->base.id)) {
                dmx_hal_buf_cls_ts_cnt_int_status(mgmt, rbuf->base.id);
                queue_work(mgmt->dmx_queue, &rbuf->buf_worker);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->buf_list_lock2, &lock_flag);
    }

    if (flag & DMX_DAV_PRS_OVFL_INT_MASK) {
        dmx_hal_clr_dav_prs_ovfl_int_flag(mgmt);
        osal_spin_lock_irqsave(&mgmt->buf_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->buf_head) {
            struct dmx_r_buf *rbuf = list_entry(node, struct dmx_r_buf, node);

            if (dmx_hal_buf_get_ovfl_int_status(mgmt, rbuf->base.id)) {
                dmx_hal_buf_cls_ovfl_int_status(mgmt, rbuf->base.id);
                queue_work(mgmt->dmx_queue, &rbuf->buf_ovfl_worker);
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->buf_list_lock2, &lock_flag);
    }

    /* ram port ip loss interrupt */
    if (flag & DMX_DAV_IP_LOSS_INT_FLG_MASK) {
        status = dmx_hal_ram_port_get_ip_loss_int_status(mgmt);
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_ram_port_cls_ip_loss_int_status(mgmt, status);

        osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->ram_port_head) {
            struct dmx_r_ram_port *ram_port = list_entry(node, struct dmx_r_ram_port, node);
            if (status & (1 << ram_port->base.id)) {
                ram_port->sync_lost_cnt++;
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);
    }

    /* unmask all dav interrupt */
    dmx_hal_un_mask_all_dav_int(mgmt);
}

static hi_void dmx_hal_process_swh_int(struct dmx_mgmt *mgmt)
{
    hi_u32 status;
    hi_size_t lock_flag;
    struct list_head *node;

    HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);

    /* pcr interrupt */
    if (dmx_hal_pcr_get_int_status(mgmt)) {
        status = dmx_hal_pcr_get_int_status(mgmt);
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_pcr_cls_int_status(mgmt, status);

        osal_spin_lock_irqsave(&mgmt->pcr_fct_list_lock2, &lock_flag);
        list_for_each(node, &mgmt->pcr_fct_head) {
            struct dmx_r_pcr_fct *rpcr_fct = list_entry(node, struct dmx_r_pcr_fct, node);

            if ((1U << rpcr_fct->base.id) & status) {
                dmx_hal_pcr_get_pcr_value(mgmt, rpcr_fct->base.id, &rpcr_fct->pcr_val);
                dmx_hal_pcr_get_scr_value(mgmt, rpcr_fct->base.id, &rpcr_fct->scr_val);
                if (rpcr_fct->callback.cb != HI_NULL) {
                    dmx_sync_pcr sync_info = {0};
                    sync_info.handle = rpcr_fct->callback.user_handle;
                    sync_info.pcr_value = rpcr_fct->pcr_val;
                    sync_info.scr_value = rpcr_fct->scr_val;
                    rpcr_fct->callback.cb(rpcr_fct->callback.user_handle, HI_DMX_EVT_PCR,
                                          (const void *)&sync_info, sizeof(sync_info));
                }
            }
        }
        osal_spin_unlock_irqrestore(&mgmt->pcr_fct_list_lock2, &lock_flag);
    }
}

static hi_s32 dmx_isr(int irq, hi_void *priv)
{
    struct dmx_mgmt *mgmt = (struct dmx_mgmt *)priv;
    hi_u32 flag;
    /* disable all demux interrupt */
    dmx_hal_dis_all_int(mgmt);
    HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);

    flag = dmx_hal_get_int_flag(mgmt);
    if (flag & DMX_PAR2CPU_INT_MASK) {
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_process_par_int(mgmt);
    }

    if (flag & DMX_DAV2CPU_INT_MASK) {
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_process_dav_int(mgmt);
    }

    if (flag & DMX_SWH2CPU_INT_MASK) {
        HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
        dmx_hal_process_swh_int(mgmt);
    }

    /* enable all demux interrupt */
    dmx_hal_en_all_int(mgmt);

    return IRQ_HANDLED;
}

/* 3 * 30 < 100ms */
#define DMX_EOS_MAX_CNT 3

static hi_void dmx_report_eos_event(struct dmx_mgmt *mgmt)
{
    struct list_head *temp = HI_NULL;
    struct dmx_r_play_fct *play_fct = HI_NULL;
    struct dmx_r_buf *buf = HI_NULL;
    struct dmx_r_avpes_buf *avpes_buf = HI_NULL;
    hi_u32 mq_cur_write_idx;

    osal_mutex_lock(&mgmt->play_fct_list_lock);

    list_for_each(temp, &mgmt->play_fct_head) {
        play_fct = list_entry(temp, struct dmx_r_play_fct, node);
        if ((play_fct->play_fct_type != DMX_PLAY_TYPE_AUD) && (play_fct->play_fct_type != DMX_PLAY_TYPE_VID)) {
            continue;
        }

        osal_mutex_lock(&play_fct->lock);

        if (play_fct->staled == HI_TRUE) {
            HI_WARN_DEMUX("play fct is staled.\n");
            goto unlock_play;
        }

        buf = play_fct->rbuf;
        if ((buf == HI_NULL) || (buf->staled == HI_TRUE)) {
            HI_ERR_DEMUX("buf is staled,%p.\n", buf);
            goto unlock_play;
        }

        avpes_buf = (struct dmx_r_avpes_buf *)buf;

        osal_mutex_lock(&buf->lock);
        osal_mutex_lock(&buf->interrupt_lock);

        mq_cur_write_idx = dmx_hal_mq_get_cur_write_idx(mgmt, buf->base.id);
        if ((buf->eos_flag != HI_TRUE) || (buf->eos_record_idx != mq_cur_write_idx)
            || (mq_cur_write_idx != avpes_buf->buf_avpes.last_mq_dsc_read_idx)) {
            buf->eos_cnt = 0;
            buf->eos_record_idx = mq_cur_write_idx;
            goto unlock_buf;
        }

        if ((buf->eos_cnt >= DMX_EOS_MAX_CNT) && (buf->callback.cb != HI_NULL)) {
            buf->eos_flag = HI_FALSE;
            buf->eos_cnt = 0;
            buf->eos_record_idx = mq_cur_write_idx;
            buf->callback.cb(buf->callback.user_handle, HI_DMX_EVT_EOS, HI_NULL, 0);
        } else {
            buf->eos_cnt++;
        }

unlock_buf:
        osal_mutex_unlock(&buf->interrupt_lock);
        osal_mutex_unlock(&buf->lock);

unlock_play:
        osal_mutex_unlock(&play_fct->lock);
    }

    osal_mutex_unlock(&mgmt->play_fct_list_lock);
}

static hi_s32 dmx_monitor(hi_void *ptr)
{
    struct dmx_mgmt *mgmt = (struct dmx_mgmt *)ptr;

    while (!kthread_should_stop()) {
        /* 30ms per cycle */
        msleep(30);
        dmx_report_eos_event(mgmt);
    }

    DMX_UNUSED(mgmt);
    return HI_SUCCESS;
}

static struct dmx_r_ram_port_ops g_dmx_ram_port_ops;

#define DMX_R_RAM_PORT_GET(handle, ram_port) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base**)&ram_port); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_ram_port_ops != ram_port->ops) { \
            dmx_r_put((struct dmx_r_base*)ram_port);\
            HI_ERR_DEMUX("handle is in active, but not a valid ram_port handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

/*************************dmx_mgmt_xxxx*************************************************/
static hi_s32 trim_ram_port_attrs(const dmx_ram_port_attr *in, dmx_ram_port_attr *out)
{
    hi_s32 ret = HI_ERR_DMX_INVALID_PARA;

    if (in->buffer_size > MAX_RAM_BUFFER_SIZE || in->buffer_size < MIN_RAM_BUFFER_SIZE) {
        HI_ERR_DEMUX("invalid ram buffer size(0x%x)\n", in->buffer_size);
        goto out;
    }

    /* auto data rate with flow control if max_data_rate 125000 kbps. */
    if (in->max_data_rate && (in->max_data_rate > MAX_RAM_DATA_RATE || in->max_data_rate < MIN_RAM_DATE_RATE)) {
        HI_ERR_DEMUX("invalid ram data rate(%d kbps)\n", in->max_data_rate);
        goto out;
    }

    memcpy(out, in, sizeof(dmx_ram_port_attr));

    ret = HI_SUCCESS;

out:
    return ret;
}

hi_s32 dmx_mgmt_create_ram_port(dmx_port port, const dmx_ram_port_attr *attrs, struct dmx_r_ram_port **ram_port)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    dmx_ram_port_attr valid_attr = {0};

    if (!(port >= DMX_RAM_PORT_0 && port <= DMX_ANY_RAM_PORT)) {
        HI_ERR_DEMUX("invalid port_id(%d).\n", port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = trim_ram_port_attrs(attrs, &valid_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("trim_ram_port_attrs failed, ret(0x%x).\n", ret);
        goto out;
    }

    ret = mgmt->ops->create_ram_port(mgmt, port, &valid_attr, ram_port);

out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_ram_port(struct dmx_r_base *obj)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    return mgmt->ops->destroy_ram_port(mgmt, (struct dmx_r_ram_port *)obj);
}
/*************************dmx_mgmt_xxxx*************************************************/
static hi_void _dmx_dvb_port_get_paras(const dmx_port_attr *port_attr, hi_bool *is_serial, hi_bool *force_valid,
    dmx_sync_mode *sync_mode, dmx_serial_bit_mode *ser_bit_mode)
{
    if (port_attr->port_type == DMX_PORT_TYPE_PARALLEL_BURST ||
        port_attr->port_type == DMX_PORT_TYPE_PARALLEL_VALID ||
        port_attr->port_type == DMX_PORT_TYPE_PARALLEL_VALID_DUMMY_SYNC ||
        port_attr->port_type == DMX_PORT_TYPE_PARALLEL_NOSYNC_188 ||
        port_attr->port_type == DMX_PORT_TYPE_PARALLEL_NOSYNC_204 ||
        port_attr->port_type == DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204) {
        *is_serial = HI_FALSE;
        *sync_mode = DMX_SYNC_MODE_SYNC;
        if (port_attr->port_type == DMX_PORT_TYPE_PARALLEL_NOSYNC_188) {
            *sync_mode = DMX_SYNC_MODE_NOSYNC_188;
        } else if (port_attr->port_type == DMX_PORT_TYPE_PARALLEL_NOSYNC_204) {
            *sync_mode = DMX_SYNC_MODE_NOSYNC_204;
        } else if (port_attr->port_type == DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204) {
            *sync_mode = DMX_SYNC_MODE_NOSYNC_188_204;
        }
    }

    if (port_attr->port_type == DMX_PORT_TYPE_SERIAL ||
        port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT ||
        port_attr->port_type == DMX_PORT_TYPE_SERIAL_NOSYNC ||
        port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT_NOSYNC ||
        port_attr->port_type == DMX_PORT_TYPE_SERIAL_NOSYNC_NOVALID ||
        port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID) {
        *is_serial = HI_TRUE;
        *sync_mode = DMX_SYNC_MODE_SYNC;
        *ser_bit_mode = DMX_SERIAL_1BIT;

        if (port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT ||
            port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT_NOSYNC ||
            port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID) {
            *ser_bit_mode = DMX_SERIAL_2BIT;
        }

        if (port_attr->port_type == DMX_PORT_TYPE_SERIAL_NOSYNC ||
            port_attr->port_type == DMX_PORT_TYPE_SERIAL_NOSYNC_NOVALID ||
            port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID) {
            *sync_mode = DMX_SYNC_MODE_NOSYNC_188;
        }

        if (port_attr->port_type == DMX_PORT_TYPE_SERIAL_NOSYNC_NOVALID ||
            port_attr->port_type == DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID) {
            *force_valid = HI_TRUE;
        }
    }

    return;
}

static hi_s32 _dmx_tsi_port_set_attrs(struct dmx_mgmt *mgmt, const hi_u32 raw_port_id, const dmx_port_attr *port_attr)
{
    hi_bool is_serial = HI_FALSE;
    hi_bool force_valid = HI_FALSE;
    dmx_sync_mode sync_mode = DMX_SYNC_MODE_MAX;
    dmx_serial_bit_mode ser_bit_mode = DMX_SERIAL_1BIT;
    dmx_tsi_port_info *tsi_port_info = &mgmt->tsi_port_info[raw_port_id];

    /* get the paraments of sync serial valid */
    _dmx_dvb_port_get_paras(port_attr, &is_serial, &force_valid, &sync_mode, &ser_bit_mode);

    /* set share clock port configure, becase RawPortId=PortIndex - HI_UNF_DMX_PORT_TSI_0+GetDmxCluster()->Ops->
     GetIFPortNum() so here we need substract the IFPortNum */
    dmx_hal_dvb_port_set_share_clk(mgmt, raw_port_id, port_attr->serial_port_share_clk);

    /* configure the dvb port, configure must be done when the port is closed */
    dmx_hal_dvb_port_close(mgmt, raw_port_id);

    /* config the inf ctrol */
    dmx_hal_dvb_port_set_inf_ctrl(mgmt, raw_port_id, is_serial, sync_mode, force_valid, port_attr->tuner_clk_mode);

    /* set inf sub ctrl, 3 means 4 pulse read one time */
    dmx_hal_dvb_port_set_inf_sub_ctrl(mgmt, raw_port_id, 3, tsi_port_info->sync_lock_th, tsi_port_info->sync_lost_th);

    dmx_hal_dvb_port_set_serial_ctrl(mgmt, raw_port_id, ser_bit_mode, tsi_port_info->bit_selector);

    dmx_hal_dvb_port_set_parallel_ctrl(mgmt, raw_port_id, tsi_port_info->bit_selector);

    /* tag sync configure */
    dmx_hal_dvb_port_set_sync_len(mgmt, raw_port_id, 0);

    dmx_hal_dvb_port_open(mgmt, raw_port_id);

    /* open the ts cnt statics feature */
    dmx_hal_dvb_set_ts_cnt_ctrl(mgmt, raw_port_id, 1); /* 1 means enable */

    /* set the tuner_inverse clock */
    dmx_hal_dvb_port_set_clk_in_pol(mgmt, raw_port_id, tsi_port_info->tuner_in_clk);

    return HI_SUCCESS;
}

static hi_s32 _dmx_dvb_port_check_attrs(dmx_port port, const dmx_port_attr *attrs)
{
    switch (attrs->port_type) {
        case DMX_PORT_TYPE_PARALLEL_BURST:
        case DMX_PORT_TYPE_PARALLEL_VALID:
        case DMX_PORT_TYPE_PARALLEL_VALID_DUMMY_SYNC:
        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188:
        case DMX_PORT_TYPE_PARALLEL_NOSYNC_204:
        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204:
        case DMX_PORT_TYPE_SERIAL: {
            break;
        }

        case DMX_PORT_TYPE_SERIAL2BIT:
        case DMX_PORT_TYPE_SERIAL_NOSYNC:
        case DMX_PORT_TYPE_SERIAL2BIT_NOSYNC:
        case DMX_PORT_TYPE_SERIAL_NOSYNC_NOVALID:
        case DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID: {
            break;
        }

        case DMX_PORT_TYPE_USER_DEFINED:
        case DMX_PORT_TYPE_AUTO: {
            return HI_ERR_DMX_NOT_SUPPORT;
        }

        default : {
            HI_ERR_DEMUX("Port %u set invalid port type %d\n", port, attrs->port_type);
            return HI_ERR_DMX_INVALID_PARA;
        }
    }

    if (attrs->tuner_in_clk > 1) {
        HI_ERR_DEMUX("Port %u set invalid tunner in clock %d\n", port, attrs->tuner_in_clk);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (attrs->serial_bit_select > 1) {
        HI_ERR_DEMUX("Port %u set invalid Serial Bit Selector %d\n", port, attrs->serial_bit_select);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (attrs->tuner_clk_mode > 1) {
        HI_ERR_DEMUX("Port %u set invalid tuner_clk_mode %d\n", port, attrs->tuner_clk_mode);
        return HI_ERR_DMX_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 drv_dmx_tsi_get_port_attrs(dmx_port port, dmx_port_attr *attrs)
{
    dmx_tsi_port_info *tsi_port_info = HI_NULL;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_TSI_PORT_0;

    if (index >= mgmt->tsi_port_cnt) {
        HI_ERR_DEMUX("invalid tsi port(0x%x index(%u).\n", port, index);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (index >= mgmt->tsi_port_policy_cnt) {
        HI_ERR_DEMUX("tsi policy support [%#x] tsi port.\n", mgmt->tsi_port_policy_cnt);
        return HI_ERR_DMX_INVALID_PARA;
    }

    tsi_port_info = &mgmt->tsi_port_info[index];

    osal_mutex_lock(&tsi_port_info->lock);
    attrs->port_mod = DMX_PORT_MODE_EXTERNAL;
    attrs->port_type = tsi_port_info->port_type;
    attrs->sync_lost_threshold = tsi_port_info->sync_lost_th;
    attrs->sync_lock_threshold = tsi_port_info->sync_lock_th;
    attrs->tuner_in_clk = tsi_port_info->tuner_in_clk;
    attrs->serial_bit_select = tsi_port_info->bit_selector;
    attrs->tuner_err_mod = 0;
    attrs->user_define_len1 = 0;
    attrs->user_define_len2 = 0;

    attrs->serial_port_share_clk = tsi_port_info->serial_port_share_clk;
    attrs->tuner_clk_mode = tsi_port_info->tuner_clk_mode;

    osal_mutex_unlock(&tsi_port_info->lock);

    return HI_SUCCESS;
}

hi_s32 drv_dmx_tsi_set_port_attrs(dmx_port port, const dmx_port_attr *attrs)
{
    hi_s32 ret;

    dmx_tsi_port_info *tsi_port_info = HI_NULL;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_TSI_PORT_0;

    if (index >= mgmt->tsi_port_cnt) {
        HI_ERR_DEMUX("invalid tsi port(0x%x index(%u).\n", port, index);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (index >= mgmt->tsi_port_policy_cnt) {
        HI_ERR_DEMUX("tsi policy support [%#x] tsi port.\n", mgmt->tsi_port_policy_cnt);
        return HI_ERR_DMX_INVALID_PARA;
    }

    ret = _dmx_dvb_port_check_attrs(port, attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid if port attrs. ret = %#x\n", ret);
        return ret;
    }

    tsi_port_info = &mgmt->tsi_port_info[index];

    osal_mutex_lock(&tsi_port_info->lock);

    tsi_port_info->port_type = attrs->port_type;
    tsi_port_info->sync_lock_th = attrs->sync_lock_threshold > DMX_MAX_LOCK_TH ?
                                    DMX_MAX_LOCK_TH : attrs->sync_lock_threshold;
    tsi_port_info->sync_lost_th = attrs->sync_lost_threshold > DMX_MAX_LOST_TH ?
                                    DMX_MAX_LOST_TH : attrs->sync_lost_threshold;;
    tsi_port_info->tuner_in_clk = attrs->tuner_in_clk;
    tsi_port_info->bit_selector = attrs->serial_bit_select;
    tsi_port_info->serial_port_share_clk = attrs->serial_port_share_clk;
    tsi_port_info->tuner_in_clk = attrs->tuner_in_clk;
    tsi_port_info->tuner_clk_mode = attrs->tuner_clk_mode;

    ret = _dmx_tsi_port_set_attrs(mgmt, index, attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dvb port set attr failed!\n");
        goto out;
    }

out:
    osal_mutex_unlock(&tsi_port_info->lock);

    return ret;
}

hi_s32 drv_dmx_if_get_port_attrs(dmx_port port, dmx_port_attr *attrs)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_IF_PORT_0;

    if (index >= mgmt->if_port_cnt) {
        HI_ERR_DEMUX("invalid if port(0x%x index(%u).\n", port, index);

        return HI_ERR_DMX_INVALID_PARA;
    }

    osal_mutex_lock(&mgmt->if_port_info[index].lock);

    attrs->port_mod = DMX_PORT_MODE_INTERNAL;
    attrs->port_type = mgmt->if_port_info[index].port_type;
    attrs->sync_lost_threshold = mgmt->if_port_info[index].sync_lost_th;
    attrs->sync_lock_threshold = mgmt->if_port_info[index].sync_lock_th;
    attrs->tuner_in_clk = mgmt->if_port_info[index].tuner_in_clk;
    attrs->tuner_clk_mode = mgmt->if_port_info[index].tuner_clk_mode;
    attrs->serial_bit_select = mgmt->if_port_info[index].bit_selector;
    attrs->tuner_err_mod = 0;
    attrs->user_define_len1 = 0;
    attrs->user_define_len2 = 0;

    attrs->serial_port_share_clk = port;

    osal_mutex_unlock(&mgmt->if_port_info[index].lock);

    return HI_SUCCESS;
}

hi_s32 drv_dmx_if_set_port_attrs(dmx_port port, const dmx_port_attr *attrs)
{
    hi_s32 ret;

    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_IF_PORT_0;
    dmx_if_port_info *if_port_info = &mgmt->if_port_info[index];

    if (index >= mgmt->if_port_cnt) {
        HI_ERR_DEMUX("invalid if port(0x%x index(%u).\n", port, index);
        return HI_ERR_DMX_INVALID_PARA;
    }

    ret = _dmx_dvb_port_check_attrs(port, attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid if port attrs.\n");
        return ret;
    }

    osal_mutex_lock(&if_port_info->lock);
    if_port_info->port_mod = attrs->port_mod;
    if_port_info->port_type = attrs->port_type;
    if_port_info->sync_lock_th =
        attrs->sync_lock_threshold > DMX_MAX_LOCK_TH ? DMX_MAX_LOCK_TH : attrs->sync_lock_threshold;
    if_port_info->sync_lost_th =
        attrs->sync_lost_threshold > DMX_MAX_LOST_TH ? DMX_MAX_LOST_TH : attrs->sync_lost_threshold;
    if_port_info->tuner_in_clk = attrs->tuner_in_clk;
    if_port_info->tuner_clk_mode = attrs->tuner_clk_mode;
    if_port_info->bit_selector = attrs->serial_bit_select;

    osal_mutex_unlock(&if_port_info->lock);

    return ret;
}

hi_s32 drv_dmx_tso_get_port_attrs(dmx_port port, dmx_tso_port_attr *attrs)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_TSO_PORT_0;

    if (index >= mgmt->tso_port_cnt) {
        HI_ERR_DEMUX("invalid tso port(0x%x index(%u).\n", port, index);

        return HI_ERR_DMX_INVALID_PARA;
    }

    osal_mutex_lock(&mgmt->tso_port_info[index].lock);

    *attrs = mgmt->tso_port_info[index].attrs;

    osal_mutex_unlock(&mgmt->tso_port_info[index].lock);

    return HI_SUCCESS;
}

hi_s32 drv_dmx_tso_set_port_attrs(dmx_port port, const dmx_tso_port_attr *attrs)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_TSO_PORT_0;

    if (index >= mgmt->tso_port_cnt) {
        HI_ERR_DEMUX("invalid tso port(0x%x index(%u).\n", port, index);

        return HI_ERR_DMX_INVALID_PARA;
    }

    if ((attrs->clk_mode >=  DMX_TSO_CLK_MODE_MAX) ||
            (attrs->valid_mode >=  DMX_TSO_VALID_ACTIVE_MAX) ||
            (attrs->en_clk >=  DMX_TSO_CLK_MAX)) {
        HI_ERR_DEMUX("enClkMode or enValidMode or enClk is invalid\n");
        return HI_ERR_DMX_INVALID_PARA;
    }

    if ((attrs->clk_div < 2) || (attrs->clk_div > 32) || (attrs->clk_div % 2 != 0)) { /* clk div 2&32 */
        HI_ERR_DEMUX("u32ClkDiv = %d is invalid\n", attrs->clk_div);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if ((attrs->clk_mode == DMX_TSO_CLK_MODE_NORMAL) && (attrs->valid_mode == DMX_TSO_VALID_ACTIVE_HIGH)) {
        HI_ERR_DEMUX("invalid tso port attr\n");
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (attrs->ts_source < DMX_TSI_PORT_0) {
        if (mgmt->if_port_cnt == 0) {
            HI_ERR_DEMUX("No IF port\n");
            return HI_ERR_DMX_INVALID_PARA;
        } else if (attrs->ts_source - DMX_IF_PORT_0 >= mgmt->if_port_cnt) {
            HI_ERR_DEMUX("IFport[0x%x] too large\n", attrs->ts_source);
            return HI_ERR_DMX_INVALID_PARA;
        }
    } else if ((attrs->ts_source >= DMX_TSI_PORT_0) && (attrs->ts_source < DMX_TSI_PORT_MAX)) {
        if (attrs->ts_source - DMX_TSI_PORT_0 >= mgmt->tsi_port_cnt) {
            HI_ERR_DEMUX("TSIport[0x%x] too large\n", attrs->ts_source);
            return HI_ERR_DMX_INVALID_PARA;
        }
    } else if ((attrs->ts_source >= DMX_RAM_PORT_0) && (attrs->ts_source < DMX_ANY_RAM_PORT)) {
        if (attrs->ts_source - DMX_RAM_PORT_0 >= mgmt->ram_port_cnt) {
            HI_ERR_DEMUX("TSIport[0x%x] too large\n", attrs->ts_source);
            return HI_ERR_DMX_INVALID_PARA;
        }
    } else {
        HI_ERR_DEMUX("enTSSource invalid\n");
        return HI_ERR_DMX_INVALID_PARA;
    }

    osal_mutex_lock(&mgmt->tso_port_info[index].lock);

    mgmt->tso_port_info[index].attrs = *attrs;

    osal_mutex_unlock(&mgmt->tso_port_info[index].lock);

    return HI_SUCCESS;
}

static hi_s32 _dmx_tag_port_check_attrs(const dmx_tag_port_attr *attrs)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (attrs->ts_src < DMX_TSI_PORT_0 || attrs->ts_src >= mgmt->tsi_port_cnt + DMX_TSI_PORT_0) {
        HI_ERR_DEMUX("Invalid tag source port[%#x]\n", attrs->ts_src);
        return HI_ERR_DMX_INVALID_PARA;
    }

    /* tag_len only can be 4, 8 ,12 */
    if ((attrs->tag_len != DMX_MAX_TAG_LENGTH) || (attrs->tag_len != 8) || (attrs->tag_len != 4)) {
        HI_ERR_DEMUX("Invalid tag len[%#x], tag len only can be 4,8,12\n", attrs->tag_len);
        return HI_ERR_DMX_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_tag_set_sync_len(struct dmx_mgmt *mgmt, hi_u32 tsi_raw_id, hi_u32 tag_len)
{
    dmx_tsi_port_info *tsi_port_info = HI_NULL;

    if (tsi_raw_id >= mgmt->tsi_port_cnt) {
        HI_ERR_DEMUX("invalid tsi raw id{0x%x].\n", tsi_raw_id);

        return HI_ERR_DMX_INVALID_PARA;
    }

    tsi_port_info = &mgmt->tsi_port_info[tsi_raw_id];

    switch (tsi_port_info->port_type) {
        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188:
        case DMX_PORT_TYPE_PARALLEL_NOSYNC_204:
        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204:
        case DMX_PORT_TYPE_SERIAL_NOSYNC:
        case DMX_PORT_TYPE_SERIAL2BIT_NOSYNC:
        case DMX_PORT_TYPE_SERIAL_NOSYNC_NOVALID:
        case DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID: {
            dmx_hal_dvb_port_set_sync_len(mgmt, tsi_raw_id, tag_len);
            break;
        }

        case DMX_PORT_TYPE_PARALLEL_BURST:
        case DMX_PORT_TYPE_PARALLEL_VALID:
        case DMX_PORT_TYPE_SERIAL:
        case DMX_PORT_TYPE_SERIAL2BIT:
        case DMX_PORT_TYPE_AUTO:
        default: {
            dmx_hal_dvb_port_set_sync_len(mgmt, tsi_raw_id, 0);
            break;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 _dmx_tag_port_set_attrs(struct dmx_mgmt *mgmt, const hi_u32 tag_raw_id,
    const dmx_tag_port_attr *port_attr)
{
    hi_s32 ret;

    hi_u32 tsi_raw_id;
    hi_u32 tag_dual_id;
    hi_u32 tag_idx;
    dmx_tag_value tag_value = {0};
    dmx_tag_dual_ctrl tag_dual_ctrl = {0};

    tag_dual_id = tag_raw_id / DMX_TAG_OUTPUT_CNT;
    tag_idx = tag_raw_id % DMX_TAG_OUTPUT_CNT;
    tsi_raw_id = port_attr->ts_src - DMX_TSI_PORT_0;

    tag_value.low = *((hi_u32 *)port_attr->tag); /* low 4 bytes */
    tag_value.mid = *((hi_u32 *)(port_attr->tag + 4)); /* middle 4 bytes */
    tag_value.high = *((hi_u32 *)(port_attr->tag + 8)); /* high 4 bytes, plus 8 */

    dmx_hal_tag_port_set_attr(mgmt, tag_dual_id, tag_idx, &tag_value);

    tag_dual_ctrl.enable = port_attr->enable;
    tag_dual_ctrl.tsi_raw_id = tsi_raw_id;
    tag_dual_ctrl.sync_mode = port_attr->sync_mod;
    tag_dual_ctrl.tag_len = port_attr->tag_len;

    dmx_hal_tag_port_set_ctrl(mgmt, tag_dual_id, &tag_dual_ctrl);

    /* disable the tag clk gt */
    dmx_hal_tag_port_set_clk_gt(mgmt, tag_dual_id, HI_FALSE);

    ret = _dmx_tag_set_sync_len(mgmt, tsi_raw_id, port_attr->tag_len);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("set source port sync len failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_dmx_tag_get_port_attrs(dmx_port port, dmx_tag_port_attr *attrs)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_TAG_PORT_0;

    if (index >= mgmt->tag_port_cnt) {
        HI_ERR_DEMUX("invalid tag port(0x%x index(%u).\n", port, index);

        return HI_ERR_DMX_INVALID_PARA;
    }

    osal_mutex_lock(&mgmt->tag_port_info[index].lock);

    *attrs = mgmt->tag_port_info[index].attrs;

    osal_mutex_unlock(&mgmt->tag_port_info[index].lock);

    return HI_SUCCESS;
}

hi_s32 drv_dmx_tag_set_port_attrs(dmx_port port, const dmx_tag_port_attr *attrs)
{
    hi_s32 ret;

    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 index = port - DMX_TAG_PORT_0;
    dmx_tag_port_info *tag_port_info = &mgmt->tag_port_info[index];

    if (index >= mgmt->tag_port_cnt) {
        HI_ERR_DEMUX("invalid tag port(0x%x index(%u).\n", port, index);

        return HI_ERR_DMX_INVALID_PARA;
    }

    ret = _dmx_tag_port_check_attrs(attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("tag attr check failed.\n");

        return HI_ERR_DMX_INVALID_PARA;
    }

    osal_mutex_lock(&tag_port_info->lock);

    tag_port_info->attrs = *attrs;

    ret = _dmx_tag_port_set_attrs(mgmt, index, attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dvb port set attr failed!\n");
        goto out;
    }

out:
    osal_mutex_unlock(&tag_port_info->lock);

    return ret;
}

hi_s32 dmx_ram_open_port(dmx_port port, const dmx_ram_port_attr *attrs, hi_handle *handle,
    struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = dmx_mgmt_create_ram_port(port, attrs, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    dmx_r_get_raw((struct dmx_r_base *)ram_port);

    ret = ram_port->ops->open(ram_port);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)ram_port, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = dmx_ram_close_port;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;
    ram_port->handle = slot->handle;

    dmx_r_put((struct dmx_r_base *)ram_port);

    DMX_RAM_TRACE_EXIT();
    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    ram_port->ops->close(ram_port);
out1:
    dmx_r_put((struct dmx_r_base *)ram_port);

    dmx_r_put((struct dmx_r_base *)ram_port);
out0:
    return ret;
}

hi_s32 dmx_ram_get_port_attrs(hi_handle handle, dmx_port_attr *attrs)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->get_attrs(ram_port, attrs);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_ram_set_port_attrs(hi_handle handle, const dmx_port_attr *attrs)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->set_attrs(ram_port, attrs);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_ram_get_port_status(hi_handle handle, dmx_ram_port_status *status)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->get_status(ram_port, status);

    dmx_r_put((struct dmx_r_base *)ram_port);
out:
    return ret;
}

hi_s32 dmx_ram_get_buffer(hi_handle handle, hi_u32 req_len, dmx_ram_buffer *buf, hi_u32 time_out)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->get_buffer(ram_port, req_len, buf, time_out);

    dmx_r_put((struct dmx_r_base *)ram_port);
out:
    return ret;
}

hi_s32 dmx_ram_push_buffer(hi_handle handle, dmx_ram_buffer *buf)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->push_buffer(ram_port, buf);

    dmx_r_put((struct dmx_r_base *)ram_port);
out:
    return ret;
}

hi_s32 dmx_ram_put_buffer(hi_handle handle, hi_u32 valid_datalen, hi_u32 start_pos)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->put_buffer(ram_port, valid_datalen, start_pos);

    dmx_r_put((struct dmx_r_base *)ram_port);
out:
    return ret;
}

hi_s32 dmx_ram_release_buffer(hi_handle handle, dmx_ram_buffer *buf)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->release_buffer(ram_port, buf);

    dmx_r_put((struct dmx_r_base *)ram_port);
out:
    return ret;
}

hi_s32 dmx_ram_flush_buffer(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->flush_buffer(ram_port);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_ram_reset_buffer(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->reset_buffer(ram_port);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_ram_close_port(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);
    ram_port->handle = HI_INVALID_HANDLE;
    ret = ram_port->ops->close(ram_port);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out1:
    dmx_r_put((struct dmx_r_base *)ram_port);
out0:
    return ret;
}

hi_s32 dmx_ram_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->pre_mmap(ram_port, buf_handle, buf_size, buf_usr_addr);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->pst_mmap(ram_port, buf_usr_addr);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_ram_get_port_id(hi_handle handle, dmx_port *port)
{
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    *port = ram_port->base.id + DMX_RAM_PORT_0;

    dmx_r_put((struct dmx_r_base *)ram_port);
out:
    return ret;
}

static hi_bool dmx_ram_is_port_created(dmx_port port)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 id = port - DMX_RAM_PORT_0;
    unsigned long mask;
    unsigned long *p = HI_NULL;
    hi_bool ret = HI_FALSE;

    osal_mutex_lock(&mgmt->ram_port_list_lock);
    osal_mutex_lock(&mgmt->lock);

    mask = BIT_MASK(id);
    p = ((unsigned long *)mgmt->ram_port_bitmap) + BIT_WORD(id);
    if (*p & mask) {
        ret = HI_TRUE;
    }

    osal_mutex_unlock(&mgmt->lock);
    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    return ret;
}

static hi_s32 dmx_ram_get_handle_by_id(dmx_port port, hi_handle *handle)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    hi_u32 id = port - DMX_RAM_PORT_0;
    struct list_head *node = HI_NULL;
    hi_size_t lock_flag;
    hi_s32 ret = HI_FAILURE;

    osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
    list_for_each(node, &mgmt->ram_port_head) {
        struct dmx_r_ram_port *ram_port = list_entry(node, struct dmx_r_ram_port, node);
        if (ram_port->base.id == id) {
            ret = HI_SUCCESS;
            *handle = ram_port->handle;
            break;
        }
    }
    osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);

    return ret;
}

hi_s32 dmx_ram_get_port_handle(dmx_port port, hi_handle *handle)
{
    hi_bool is_created = dmx_ram_is_port_created(port);
    hi_u32 ret;

    if (is_created == HI_FALSE) {
        HI_ERR_DEMUX("port(%u) not created.\n", port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_get_handle_by_id(port, handle);
out:
    return ret;
}

hi_s32 dmx_ram_get_bufhandle(hi_handle handle,
    hi_mem_handle_t *buf_handle, hi_mem_handle_t *dsc_buf_handle, hi_mem_handle_t *flush_buf_handle)
{
#ifdef DMX_TEE_SUPPORT
    hi_s32 ret;
    struct dmx_r_ram_port *ram_port = HI_NULL;

    DMX_RAM_TRACE_ENTER();
    ret = DMX_R_RAM_PORT_GET(handle, ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->get_buf_handle(ram_port, buf_handle, dsc_buf_handle, flush_buf_handle);

    dmx_r_put((struct dmx_r_base *)ram_port);
    DMX_RAM_TRACE_EXIT();
out:
    return ret;
#else
    return HI_ERR_DMX_NOT_SUPPORT;
#endif
}

/*************************dmx_r_ram_port_xxx_impl*************************************************/
static inline hi_bool ram_port_staled(struct dmx_r_ram_port *ram_port)
{
    if (unlikely(ram_port->staled == HI_TRUE)) {
        HI_ERR_DEMUX("ram port is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_void flow_control_handler(struct work_struct *work)
{
    return;
}

static hi_void dsc_end_irq_handler(struct work_struct *work)
{
    struct dmx_r_ram_port *ram_port =  container_of(work, struct dmx_r_ram_port, dsc_end_worker);
    hi_u32 cur_sw_read_idx, cur_hw_read_idx;
    hi_u32 del_desc_cnt = 0;
    osal_mutex_lock(&ram_port->blk_map_lock);

    /*
     * dsc end handler running in wq context.
     * ram port hw maybe invalid after disable ram port by _DMX_R_RAMPort_Close_Impl.
     */
    if (dmx_hal_ram_port_enabled(ram_port->base.mgmt, ram_port->base.id) != HI_TRUE) {
        goto out;
    }

    cur_sw_read_idx = ram_port->last_dsc_read_idx;
    cur_hw_read_idx = dmx_hal_ram_port_get_cur_read_idx(ram_port->base.mgmt, ram_port->base.id);

#ifdef HI_DMX_TSBUF_MULTI_THREAD_SUPPORT
    /* assert */
    del_desc_cnt = (cur_hw_read_idx >= cur_sw_read_idx) ? (cur_hw_read_idx - cur_sw_read_idx) :
                   (ram_port->dsc_depth + cur_hw_read_idx - cur_sw_read_idx);

    WARN_ON(del_desc_cnt > ram_port->total_blk_nr);

    /* [cur_sw_read_idx, cur_hw_read_idx) */
    while (cur_sw_read_idx != cur_hw_read_idx) {
        hi_u32 *dsc_addr = HI_NULL;
        hi_u64 buf_phy_addr;
        U_RAM_DSC_WORD_1 word1;

        dsc_addr = (hi_u32 *)(ram_port->dsc_buf_ker_addr + cur_sw_read_idx * DEFAULT_RAM_DSC_SIZE);
        word1.u32 = dsc_addr[1];
        buf_phy_addr = *(dsc_addr) | (word1.bits.session << 32U);

        HI_DBG_DEMUX("dsc_addr[0] is [0x%x], dsc_addr[1] is [0x%x]\n", dsc_addr[0], dsc_addr[1]);
        /* flush dsc, word0 0 and word1[bit19..0] 0 setted by flush_ts_buffer */
        if (unlikely(dsc_addr[0] == 0x0 && (dsc_addr[1] & 0xFFFFF) == 0x0)) {
            WARN_ON(word1.bits.flush != 0x1);

            HI_ERR_DEMUX("flush in %s %d\n", __FUNCTION__, __LINE__);
            ram_port->wait_flush = 1;
            wake_up_interruptible(&ram_port->wait_queue);
        } else {
            hi_u32 blk_idx = (buf_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;

            WARN_ON(buf_phy_addr < ram_port->buf_phy_addr);
            WARN_ON(buf_phy_addr >= ram_port->buf_phy_addr + ram_port->buf_size);

            switch (ram_port->blk_map[blk_idx].flag) {
                case BLK_ADD_INT_FLAG: {
                        ram_port->blk_map[blk_idx].ref--;
                        break;
                    }

                case BLK_ADD_RELCAIM_INT_FLAG: {
                        if (--ram_port->blk_map[blk_idx].ref == 0) {
                            ram_port->blk_map[blk_idx].flag = BLK_FREE_FLAG;
                            ram_port->blk_map[blk_idx].head_blk_idx = BLK_INVALID_HEAD_IDX;
                            ram_port->blk_map[blk_idx].req_len = 0;

                            ram_port->free_blk_nr++;
                        }
                        break;
                    }

                default:
                    WARN(1, "Invalid blk_map flag[%#x]\n", ram_port->blk_map[blk_idx].flag);
                    break;
            }

            WARN_ON(ram_port->free_blk_nr > ram_port->total_blk_nr);
        }

        /* roll loop */
        cur_sw_read_idx = (cur_sw_read_idx + 1 >= ram_port->dsc_depth) ? 0 : cur_sw_read_idx + 1;
    }
#else
    DMX_UNUSED(del_desc_cnt);
    while (cur_sw_read_idx != cur_hw_read_idx) {
        hi_u32 *dsc_addr = (hi_u32 *)(ram_port->dsc_buf_ker_addr + cur_sw_read_idx * DEFAULT_RAM_DSC_SIZE);
        hi_u32 read_addr = *(dsc_addr);
        U_RAM_DSC_WORD_1 word1;

        word1.u32 = dsc_addr[1];

        HI_DBG_DEMUX("dsc_addr[0] is [0x%x], dsc_addr[1] is [0x%x]\n", dsc_addr[0], dsc_addr[1]);
        /* flush dsc, word0 0 and word1[bit19..0] 0 setted by flush_ts_buffer */
        if (unlikely(dsc_addr[0] == 0x0 && (dsc_addr[1] & 0xFFFFF) == 0x0)) {
            WARN_ON(word1.bits.flush != 0x1);

            HI_ERR_DEMUX("flush in %s %d\n", __FUNCTION__, __LINE__);
            ram_port->wait_flush = 1;
            wake_up_interruptible(&ram_port->wait_queue);
        } else {
            if (0 != ram_port->write) {
                /* PortInfo->Read is the ts buffer read offset */
                ram_port->read = read_addr - ram_port->buf_phy_addr + word1.bits.iplength;
                if (ram_port->read == ram_port->write) {
                    ram_port->read  = 0;
                    ram_port->write = 0;
                }
            }
        }

        /* roll loop */
        cur_sw_read_idx = (cur_sw_read_idx + 1 >= ram_port->dsc_depth) ? 0 : cur_sw_read_idx + 1;
    }
#endif
    ram_port->last_dsc_read_idx = cur_hw_read_idx;
    HI_DBG_DEMUX("in %s %d\n", __FUNCTION__, __LINE__);
    ram_port->wait_cond = 1;
    wake_up_interruptible(&ram_port->wait_queue);

out:
    osal_mutex_unlock(&ram_port->blk_map_lock);
    return;
}

static hi_s32 wait_dsc_sync(struct dmx_r_ram_port *ram_port, hi_u32 count)
{
    hi_s32 ret = HI_SUCCESS;

    if (unlikely(count == 0)) {
        goto out;
    }

    while (!(ram_port->dsc_depth - dmx_get_queue_lenth(ram_port->last_dsc_read_idx, ram_port->last_dsc_write_idx,
                                                       ram_port->dsc_depth) > count)) {
        ram_port->wait_cond = 0;
        ret = wait_event_interruptible(ram_port->wait_queue, ram_port->wait_cond);
        if (ret < 0) {
            /* exit after get signal. */
            ret = HI_ERR_DMX_INTR;
            goto out;
        }
    }

out:
    return ret;
}

static hi_s32 clean_ram_port(struct dmx_r_ram_port *ram_port)
{
    /* do something such as set the releated band port invalid */
    dmx_hal_ram_port_clr_port(ram_port->base.mgmt, ram_port->base.id);

    return HI_SUCCESS;
}

static hi_s32 __flush_ts_buffer_first_step(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;
    hi_u32 *cur_dsc_addr = HI_NULL;
    struct dmx_ram_port_dsc dsc = {0};

    cur_dsc_addr = (hi_u32 *)(ram_port->dsc_buf_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
    DMX_NULL_POINTER_RETURN(cur_dsc_addr);

    dsc.buf_phy_addr = ram_port->flush_buf_phy_addr;
    dsc.buf_len = ram_port->flush_buf_size;
    dsc.desep = HI_TRUE;
    dsc.flush_flag = HI_FALSE;
    dsc.sync_data_flag = HI_TRUE;
    dsc.cur_dsc_addr = cur_dsc_addr;
    dmx_hal_ram_port_set_dsc(ram_port->base.mgmt, ram_port->base.id, &dsc);

    /* copy the dsc content to tee secure dsc buffer */
    if (ram_port->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_ramport_dsc tee_ramport_dsc = {0};
        tee_ramport_dsc.buf_phy_addr = dsc.buf_len;
        tee_ramport_dsc.buf_len = dsc.buf_len;
        tee_ramport_dsc.desep = dsc.desep;
        tee_ramport_dsc.flush_flag = dsc.flush_flag;
        tee_ramport_dsc.sync_data_flag = dsc.sync_data_flag;
        tee_ramport_dsc.write_index = ram_port->last_dsc_write_idx;

        ret = dmx_tee_set_ramport_dsc(ram_port->base.id, &tee_ramport_dsc);
        if (ret != HI_SUCCESS) {
            osal_mutex_unlock(&ram_port->blk_map_lock);
            HI_ERR_DEMUX("set tee ramport dsc faild!\n");
            return ret;
        }
    }

    ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ? 0 :
                                    ram_port->last_dsc_write_idx + 1;
    dmx_hal_ram_port_add_dsc(ram_port->base.mgmt, ram_port->base.id, 1);

    return HI_SUCCESS;
}

static hi_s32 __flush_ts_buffer_second_step(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;
    hi_u32 *cur_dsc_addr = HI_NULL;
    struct dmx_ram_port_dsc dsc = {0};

    cur_dsc_addr = (hi_u32 *)(ram_port->dsc_buf_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
    DMX_NULL_POINTER_RETURN(cur_dsc_addr);

    dsc.buf_phy_addr = 0x0;
    dsc.buf_len = 0x0;
    dsc.desep = HI_TRUE;
    dsc.flush_flag = HI_TRUE;
    dsc.sync_data_flag = HI_TRUE;
    dsc.cur_dsc_addr = cur_dsc_addr;

    dmx_hal_ram_port_set_dsc(ram_port->base.mgmt, ram_port->base.id, &dsc);

    /* copy the dsc content to tee secure dsc buffer */
    if (ram_port->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_ramport_dsc tee_ramport_dsc = {0};
        tee_ramport_dsc.buf_phy_addr = dsc.buf_len;
        tee_ramport_dsc.buf_len = dsc.buf_len;
        tee_ramport_dsc.desep = dsc.desep;
        tee_ramport_dsc.flush_flag = dsc.flush_flag;
        tee_ramport_dsc.sync_data_flag = dsc.sync_data_flag;
        tee_ramport_dsc.write_index = ram_port->last_dsc_write_idx;

        ret = dmx_tee_set_ramport_dsc(ram_port->base.id, &tee_ramport_dsc);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set tee ramport dsc faild!\n");
            return ret;
        }
    }

    ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ? 0 :
                                    ram_port->last_dsc_write_idx + 1;
    dmx_hal_ram_port_add_dsc(ram_port->base.mgmt, ram_port->base.id, 1);

    return HI_SUCCESS;
}

static hi_s32 _flush_ts_buffer(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    /* wait dsc ready indefinitely until signal. */
    ret = wait_dsc_sync(ram_port, 1);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&ram_port->blk_map_lock);

    /* first: send the sync data(16 ts package, all filled with 0x47) */
    ret = __flush_ts_buffer_first_step(ram_port);
    if (ret != HI_SUCCESS) {
        osal_mutex_unlock(&ram_port->blk_map_lock);
        HI_ERR_DEMUX("flush ts buffer first step faild!\n");
        goto out;
    }

    /* second: send the flush flag desc */
    ret = __flush_ts_buffer_second_step(ram_port);
    if (ret != HI_SUCCESS) {
        osal_mutex_unlock(&ram_port->blk_map_lock);
        HI_ERR_DEMUX("flush ts buffer second step faild!\n");
        goto out;
    }

    osal_mutex_unlock(&ram_port->blk_map_lock);

    /* wait flush dsc  reach  signal. */
    ret = wait_event_interruptible(ram_port->wait_queue, ram_port->wait_flush);
    if (ret < 0) {
        ret = HI_ERR_DMX_INTR; /* exit after get signal. */
        goto out;
    }

    ram_port->wait_flush = 0;

out:
    return ret;
}

static hi_s32 _dmx_r_ram_port_close_impl(struct dmx_r_ram_port *ram_port)
{
    osal_mutex_lock(&ram_port->blk_map_lock);

    /* disable ramport. */
    dmx_hal_ram_port_de_config(ram_port->base.mgmt, ram_port->base.id);

    /* clear the  ramport ap status */
    dmx_hal_ram_port_cls_ap_status(ram_port->base.mgmt, ram_port->base.id);

    /* clean the ramport */
    dmx_dofunc_no_return(clean_ram_port(ram_port));

    ram_port->last_dsc_read_idx   = 0;
    ram_port->last_dsc_write_idx  = 0;
    osal_mutex_unlock(&ram_port->blk_map_lock);

    cancel_work_sync(&ram_port->flow_control_worker);
    cancel_work_sync(&ram_port->dsc_end_worker);

    return HI_SUCCESS;
}

static hi_s32 _dmx_alloc_ts_buf_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;
    hi_char buf_name[DMX_STR_LEN_32] = {0};
    dmx_mem_info ts_buf = {0};

    /* alloc buf */
    ret = snprintf_s(buf_name, sizeof(buf_name), sizeof(buf_name) - 1, "dmx_tsbuf[%d]", ram_port->base.id);
    if (ret == -1) {
        HI_ERR_DEMUX("snprintf_s failed!\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = dmx_alloc_and_map_buf(buf_name, ram_port->buf_size, 0, &ts_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("mmz malloc 0x%x failed\n", ram_port->buf_size);
        goto out;
    }

    WARN_ON(ram_port->buf_size != ts_buf.buf_size);

    ram_port->buf_handle   = ts_buf.handle;
    ram_port->buf_obj      = ts_buf.dmx_buf_obj;
    ram_port->buf_usr_addr = HI_NULL;
    ram_port->buf_ker_addr = ts_buf.buf_vir_addr;
    ram_port->buf_phy_addr = ts_buf.buf_phy_addr;

out:
    return ret;
}

static hi_void _dmx_free_ts_buf_impl(struct dmx_r_ram_port *ram_port)
{
    dmx_mem_info ts_buf = {0};

    /* release ts buffer */
    ts_buf.handle        = ram_port->buf_handle;
    ts_buf.dmx_buf_obj   = ram_port->buf_obj;
    ts_buf.buf_vir_addr  = ram_port->buf_ker_addr;
    ts_buf.buf_phy_addr  = ram_port->buf_phy_addr;
    ts_buf.buf_size      = ram_port->buf_size;
    ts_buf.user_map_flag = (ram_port->buf_usr_addr != HI_NULL) ? HI_TRUE : HI_FALSE;
    dmx_unmap_and_release_buf(&ts_buf);
}

static hi_s32 _dmx_alloc_flush_buf_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;
    hi_char buf_name[DMX_STR_LEN_32] = {0};
    dmx_mem_info flush_buf = {0};

    /* alloc buf */
    ret = snprintf_s(buf_name, sizeof(buf_name), sizeof(buf_name) - 1, "dmx_flushbuf[%d]", ram_port->base.id);
    if (ret == -1) {
        HI_ERR_DEMUX("snprintf_s failed!\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ram_port->flush_buf_size = DMX_TS_PKT_SIZE * DMX_RAM_FLUSH_TS_CNT;
    ret = dmx_alloc_and_map_buf(buf_name, ram_port->flush_buf_size, 0, &flush_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("flush buffer malloc 0x%x failed\n", ram_port->flush_buf_size);
        goto out;
    }

    WARN_ON(ram_port->flush_buf_size != flush_buf.buf_size);

    ram_port->flush_buf_handle   = flush_buf.handle;
    ram_port->flush_buf_obj      = flush_buf.dmx_buf_obj;
    ram_port->flush_buf_ker_addr = flush_buf.buf_vir_addr;
    ram_port->flush_buf_phy_addr = flush_buf.buf_phy_addr;

    /* filled the flush buffer with all 0x47 */
    memset(ram_port->flush_buf_ker_addr, 0x47, ram_port->flush_buf_size);

out:
    return ret;
}

static hi_void _dmx_free_flush_buf_impl(struct dmx_r_ram_port *ram_port)
{
    dmx_mem_info flush_buf = {0};

    /* release flush buffer */
    flush_buf.handle        = ram_port->flush_buf_handle;
    flush_buf.dmx_buf_obj   = ram_port->flush_buf_obj;
    flush_buf.buf_vir_addr  = ram_port->flush_buf_ker_addr;
    flush_buf.buf_phy_addr  = ram_port->flush_buf_phy_addr;
    flush_buf.buf_size      = ram_port->flush_buf_size;
    flush_buf.user_map_flag = HI_FALSE;
    dmx_unmap_and_release_buf(&flush_buf);
}

static hi_s32 _dmx_alloc_dsc_buf_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;
    hi_char buf_name[DMX_STR_LEN_32] = {0};
    dmx_mem_info dsc_buf = {0};
    hi_u32 dsc_buf_size = DEFAULT_RAM_DSC_DEPTH * DEFAULT_RAM_DSC_SIZE;

    /* alloc buf */
    ret = snprintf_s(buf_name, sizeof(buf_name), sizeof(buf_name) - 1, "dmx_dscbuf[%d]", ram_port->base.id);
    if (ret == -1) {
        HI_ERR_DEMUX("snprintf_s failed!\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = dmx_alloc_and_map_buf(buf_name, dsc_buf_size, 0, &dsc_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dsc buffer malloc 0x%x failed\n", dsc_buf_size);
        goto out;
    }

    WARN_ON(dsc_buf.buf_phy_addr % RAM_DSC_ADDR_ALIGN);
    WARN_ON(dsc_buf_size != dsc_buf.buf_size);

    ram_port->dsc_buf_handle   = dsc_buf.handle;
    ram_port->dsc_buf_obj      = dsc_buf.dmx_buf_obj;
    ram_port->dsc_buf_ker_addr = dsc_buf.buf_vir_addr;
    ram_port->dsc_buf_phy_addr = dsc_buf.buf_phy_addr;
    ram_port->dsc_depth = DEFAULT_RAM_DSC_DEPTH;
    memset(ram_port->dsc_buf_ker_addr, 0, dsc_buf_size);

out:
    return ret;
}

static hi_void _dmx_free_dsc_buf_impl(struct dmx_r_ram_port *ram_port)
{
    dmx_mem_info dsc_buf = {0};

    /* release flush buffer */
    dsc_buf.handle        = ram_port->dsc_buf_handle;
    dsc_buf.dmx_buf_obj   = ram_port->dsc_buf_obj;
    dsc_buf.buf_vir_addr  = ram_port->dsc_buf_ker_addr;
    dsc_buf.buf_phy_addr  = ram_port->dsc_buf_phy_addr;
    dsc_buf.buf_size      = ram_port->dsc_depth * DEFAULT_RAM_DSC_SIZE;
    dsc_buf.user_map_flag = HI_FALSE;
    dmx_unmap_and_release_buf(&dsc_buf);
}

#ifdef HI_DMX_TSBUF_MULTI_THREAD_SUPPORT
static hi_s32 _dmx_r_ram_port_open_impl(struct dmx_r_ram_port *ram_port)
{
    hi_u32 index = 0;

    osal_mutex_lock(&ram_port->blk_map_lock);

    ram_port->free_blk_nr = ram_port->total_blk_nr;
    ram_port->iter_blk_map_helper = 0;

    memset(ram_port->blk_map, 0, sizeof(*ram_port->blk_map) * ram_port->total_blk_nr);

    for (index = 0; index < ram_port->total_blk_nr; index++) {
        ram_port->blk_map[index].head_blk_idx = BLK_INVALID_HEAD_IDX;
    }

    /* clear ram mmu cache */
    dmx_hal_ram_clr_mmu_cache(ram_port->base.mgmt, ram_port->base.id);

    /* CRC check of input ts packets. */
    if (ram_port->pkt_size == DMX_TS_PKT_SIZE &&
        (ram_port->crc_scb == DMX_TS_CRC_EN_SCB_DIS || ram_port->crc_scb == DMX_TS_CRC_EN_SCB_EN)) {
        dmx_hal_ram_port_ip_crc_en(ram_port->base.mgmt, ram_port->base.id);
    }

    dmx_hal_ram_port_config(ram_port->base.mgmt, ram_port->base.id, ram_port->dsc_buf_phy_addr, ram_port->dsc_depth,
                            ram_port->max_data_rate);

    ram_port->last_dsc_read_idx = 0;
    ram_port->last_dsc_write_idx = 0;
    osal_mutex_unlock(&ram_port->blk_map_lock);
    ram_port->all_pkt_cnt = 0;

    ram_port->get_count = 0;
    ram_port->get_valid_count = 0;
    ram_port->push_count = 0;
    ram_port->put_count = 0;
    ram_port->flush_count = 0;

    ram_port->min_len = 188; /* mini ts packet length is 188 Bytes */
    ram_port->max_len = 204; /* max ts packet length is 204 Bytes */
    ram_port->sync_lock_th = 7; /* default sync_lock_th as 7 */
    ram_port->sync_lost_th = 0; /* default sync_lost_th as 0 */
    ram_port->port_type = DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204;

    ram_port->sync_lost_cnt = 0;

    return HI_SUCCESS;
}

static hi_s32 dmx_r_ram_port_open_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    WARN_ON(ram_port->staled == HI_TRUE);

    ret = _dmx_alloc_ts_buf_impl(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("tsbuf malloc 0x%x failed\n", ram_port->buf_size);
        goto unlock;
    }

    ret = _dmx_alloc_flush_buf_impl(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("flush buf malloc 0x%x failed\n", ram_port->flush_buf_size);
        goto free_tsbuf;
    }

    ret = _dmx_alloc_dsc_buf_impl(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dsc buf malloc 0x%x failed\n", ram_port->flush_buf_size);
        goto free_flush;
    }

    ram_port->max_data_rate = DMX_DEFAULT_RAMPORT_RATE;

    /* alloc map blk */
    ram_port->total_blk_nr = ram_port->buf_size / ram_port->blk_size;

    ram_port->blk_map = HI_KZALLOC(HI_ID_DEMUX, sizeof(*ram_port->blk_map) * ram_port->total_blk_nr, GFP_KERNEL);
    if (!ram_port->blk_map) {
        HI_ERR_DEMUX("malloc 0x%x failed\n", sizeof(*ram_port->blk_map) * ram_port->total_blk_nr);
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto free_dsc;
    }

    ret = _dmx_r_ram_port_open_impl(ram_port);
    if (ret != HI_SUCCESS) {
        goto free_blk;
    }

    osal_mutex_unlock(&ram_port->lock);

    return HI_SUCCESS;

free_blk:
    HI_KFREE(HI_ID_DEMUX, ram_port->blk_map);
free_dsc:
    _dmx_free_dsc_buf_impl(ram_port);
free_flush:
    _dmx_free_flush_buf_impl(ram_port);
free_tsbuf:
    _dmx_free_ts_buf_impl(&ram_port);
unlock:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 ram_port_get_buf_used(struct dmx_r_ram_port *ram_port)
{
    hi_u32 cur_cont_blk_cnt = 0, max_cont_blk_cnt = 0;
    unsigned long start, end;
    hi_u32 index;

    osal_mutex_lock(&ram_port->blk_map_lock);

    /* refer to dmx_port_proc_read. external checking without hold ramport->lock. */
    if (!ram_port->blk_map) {
        osal_mutex_unlock(&ram_port->blk_map_lock);
        return HI_ERR_DMX_STALED;
    }

    start = osal_get_jiffies;
    end   = start + osal_msecs_to_jiffies(10); /* 10 ms */

    /* locate max free continue space */
    for (index = 0; index < ram_port->total_blk_nr; index++) {
        if (BLK_FREE_FLAG == ram_port->blk_map[index].flag) {
            cur_cont_blk_cnt++;

            max_cont_blk_cnt = cur_cont_blk_cnt > max_cont_blk_cnt ? cur_cont_blk_cnt : max_cont_blk_cnt;
        } else {
            max_cont_blk_cnt = cur_cont_blk_cnt > max_cont_blk_cnt ? cur_cont_blk_cnt : max_cont_blk_cnt;

            cur_cont_blk_cnt = 0;
        }
    }

    WARN_ON(max_cont_blk_cnt > ram_port->total_blk_nr);
    ram_port->buf_used = ram_port->buf_size - max_cont_blk_cnt * ram_port->blk_size;

    WARN(!time_in_range(osal_get_jiffies, start, end), "get buffer status latency is too much(%d ms)!",
         jiffies_to_msecs(osal_get_jiffies - start));

    osal_mutex_unlock(&ram_port->blk_map_lock);

    return HI_SUCCESS;
}

static hi_s32 get_buffer(struct dmx_r_ram_port *ram_port, hi_u32 req_blk_nr, hi_u32 *buf_phy_addr)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 cur_cont_blk_idx = 0, cur_cont_blk_cnt = 0;
    unsigned long start = osal_get_jiffies, end = start + osal_msecs_to_jiffies(10); /* 10 ms */
    hi_u32 index;

    for (index = ram_port->iter_blk_map_helper; index < ram_port->total_blk_nr; index++) {
        if (ram_port->blk_map[index].flag == BLK_FREE_FLAG) {
            cur_cont_blk_idx = (cur_cont_blk_cnt == 0) ? index : cur_cont_blk_idx;

            cur_cont_blk_cnt++;

            if (cur_cont_blk_cnt >= req_blk_nr) {
                *buf_phy_addr = ram_port->buf_phy_addr + cur_cont_blk_idx * ram_port->blk_size;

                ram_port->iter_blk_map_helper = cur_cont_blk_idx + req_blk_nr;

                ret = HI_SUCCESS;

                break;
            }
        } else {
            cur_cont_blk_cnt = 0;
        }
    }

    WARN(!time_in_range(osal_get_jiffies, start, end), "alloc buffer latency is too much(%d ms)!",
         jiffies_to_msecs(osal_get_jiffies - start));

    return ret;
}

static hi_s32 check_buffer(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    hi_s32 ret = HI_ERR_DMX_INVALID_PARA;
    hi_u32 start_blk_idx, blk_cnt, head_blk_idx;
    hi_u32 index;
    hi_u64 phy_addr = ram_port->buf_phy_addr + buf->offset;

    /* hw support access non-align byte address. */
    if (buf->data == HI_NULL || buf->length == 0) {
        HI_ERR_DEMUX("invalid buffer[0x%x:%d] range.\n", buf->data, buf->length);
        goto out;
    }

    if (phy_addr < ram_port->buf_phy_addr ||
            phy_addr + buf->length  > ram_port->buf_phy_addr + ram_port->buf_size) {
        HI_ERR_DEMUX("invalid buffer[0x%x:%d] range.\n", phy_addr, buf->length);
        goto out;
    }

    start_blk_idx = (phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    blk_cnt      = (buf->length + ram_port->blk_size - 1) / ram_port->blk_size;

    /* all blk have same headblkidx which means they belong to one buffer */
    head_blk_idx = ram_port->blk_map[start_blk_idx].head_blk_idx;

    for (index = start_blk_idx; index < start_blk_idx + blk_cnt && index < ram_port->total_blk_nr; index++) {
        /* head blk idx is wrong */
        if (head_blk_idx != ram_port->blk_map[index].head_blk_idx) {
            goto out;
        }

        /* the blk tag must be BLK_ALLOC_FLAG or BLK_ADD_INT_TAG */
        if (ram_port->blk_map[index].flag != BLK_ALLOC_FLAG && ram_port->blk_map[index].flag != BLK_ADD_INT_FLAG) {
            HI_ERR_DEMUX("blk(%d) tag(0x%x) is wrong.\n", index, ram_port->blk_map[index].flag);
            goto out;
        }
    }

    /* push buffer range is subset of request range. */
    if (phy_addr < head_blk_idx * ram_port->blk_size + ram_port->buf_phy_addr ||
            phy_addr + buf->length > head_blk_idx * ram_port->blk_size + ram_port->buf_phy_addr +
            ram_port->blk_map[head_blk_idx].req_len) {
        HI_ERR_DEMUX("invalid buffer[0x%x:%d] range.\n", phy_addr, buf->length);
        goto out;
    }

    ret = HI_SUCCESS;

out:
    return ret;
}

static inline hi_s32 check_push_buffer(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->blk_map_lock);

    ret = check_buffer(ram_port, buf);

    osal_mutex_unlock(&ram_port->blk_map_lock);

    return ret;
}

static hi_s32 push_ts_buffer(struct dmx_r_ram_port *ram_port, hi_u64 push_phy_addr, hi_u32 push_len)
{
    hi_s32 ret;
    hi_u32 blk_idx;
    hi_u32 *cur_dsc_addr = HI_NULL;
    struct dmx_ram_port_dsc dsc = {0};

    /* wait dsc ready indefinitely until signal. */
    ret = wait_dsc_sync(ram_port, 1);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&ram_port->blk_map_lock);

    blk_idx = (push_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    ram_port->blk_map[blk_idx].flag = BLK_ADD_INT_FLAG;
    ram_port->blk_map[blk_idx].ref++;

    cur_dsc_addr = (hi_u32 *)(ram_port->dsc_buf_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
    DMX_NULL_POINTER_GOTO(cur_dsc_addr, out1);

    dsc.buf_phy_addr = push_phy_addr;
    dsc.buf_len = push_len;
    dsc.desep = HI_TRUE;
    dsc.flush_flag = HI_FALSE;
    dsc.sync_data_flag = HI_FALSE;
    dsc.cur_dsc_addr = cur_dsc_addr;

    dmx_hal_ram_port_set_dsc(ram_port->base.mgmt, ram_port->base.id, &dsc);

    /* copy the dsc content to tee secure dsc buffer */
    if (ram_port->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_ramport_dsc tee_ramport_dsc = {0};
        tee_ramport_dsc.buf_phy_addr = dsc.buf_len;
        tee_ramport_dsc.buf_len = dsc.buf_len;
        tee_ramport_dsc.desep = dsc.desep;
        tee_ramport_dsc.flush_flag = dsc.flush_flag;
        tee_ramport_dsc.sync_data_flag = dsc.sync_data_flag;
        tee_ramport_dsc.write_index = ram_port->last_dsc_write_idx;

        ret = dmx_tee_set_ramport_dsc(ram_port->base.id, &tee_ramport_dsc);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set tee ramport dsc faild!\n");
            goto out1;
        }
    }

    ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ? 0 :
                                    ram_port->last_dsc_write_idx + 1;

    dmx_hal_ram_port_add_dsc(ram_port->base.mgmt, ram_port->base.id, 1);

out1:
    osal_mutex_unlock(&ram_port->blk_map_lock);
out0:
    return ret;
}

static hi_s32 check_release_buffer(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    hi_s32 ret;
    hi_u32 start_blk_idx, blk_cnt, head_blk_idx;
    hi_u32 index;

    osal_mutex_lock(&ram_port->blk_map_lock);

    ret = check_buffer(ram_port, buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    start_blk_idx = (buf->phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;

    /* all blk have same headblkidx which means they belong to one buffer */
    head_blk_idx = ram_port->blk_map[start_blk_idx].head_blk_idx;

    if (head_blk_idx != ram_port->blk_map[head_blk_idx].head_blk_idx ||
            buf->phy_addr != head_blk_idx * ram_port->blk_size + ram_port->buf_phy_addr) {
        HI_ERR_DEMUX("invalid buffer[0x%x:%d] range.\n", buf->phy_addr, buf->length);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    /*
     * HI_MPI_DMX_PutBuffer will complete push data and put buffer in one call.
     * for example:
     * |***************|&&&&&&&|
     * <-        buffer       ->
     * <-   push      ->
     * <-         put         ->
     * the input pstBuf only indicate the scope of push(head of buffer), but put need release entire buffer.
     */
    buf->length = ram_port->blk_map[head_blk_idx].req_len;

    /* next blk should not belong to this buffer. */
    blk_cnt = (buf->length + ram_port->blk_size - 1) / ram_port->blk_size;
    index = head_blk_idx + blk_cnt;
    if (unlikely(index < ram_port->total_blk_nr &&
                 ram_port->blk_map[index].flag != BLK_FREE_FLAG &&
                 head_blk_idx == ram_port->blk_map[index].head_blk_idx)) {
        WARN(1, "index[%#x], total_blk_nr[%#x], blk_map flag[%#x], head_blk_idx[%#x], blk_map head_blk_idx[%#x]\n",
             index, ram_port->total_blk_nr, ram_port->blk_map[blk_idx].flag,
             head_blk_idx, ram_port->blk_map[index].head_blk_idx);
    }

out:
    osal_mutex_unlock(&ram_port->blk_map_lock);
    return ret;
}

static hi_s32 _dmx_r_ram_port_get_buffer_impl(struct dmx_r_ram_port *ram_port, hi_u32 req_len, hi_u64 *buf_phy_addr)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 req_blk_nr = (req_len  + ram_port->blk_size - 1) / ram_port->blk_size;
    hi_u32 head_blk_idx = 0;
    hi_u32 index;

    DMX_FATAL_CON_RETURN(req_len == 0x0, HI_ERR_DMX_INVALID_PARA);
    DMX_FATAL_CON_RETURN(req_len > ram_port->buf_size, HI_ERR_DMX_INVALID_PARA);

    osal_mutex_lock(&ram_port->blk_map_lock);

    if (ram_port->free_blk_nr < req_blk_nr) {
        ret = HI_ERR_DMX_NOAVAILABLE_BUF;
        goto out;
    }

    /* fast path */
    ret = get_buffer(ram_port, req_blk_nr, buf_phy_addr);
    if (ret != HI_SUCCESS) {
        cond_resched();

        /* try slow path again */
        ram_port->iter_blk_map_helper = 0;
        ret = get_buffer(ram_port, req_blk_nr, buf_phy_addr);
        if (ret != HI_SUCCESS) {
            ret = HI_ERR_DMX_NOAVAILABLE_BUF;
            goto out;
        }
    }

    head_blk_idx = (*buf_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    for (index = head_blk_idx; index < head_blk_idx + req_blk_nr; index++) {
        ram_port->blk_map[index].flag       = BLK_ALLOC_FLAG;
        ram_port->blk_map[index].ref        = 1;
        ram_port->blk_map[index].head_blk_idx = head_blk_idx;
        ram_port->blk_map[index].req_len     = req_len;

        ram_port->free_blk_nr--;
    }

out:
    osal_mutex_unlock(&ram_port->blk_map_lock);
    return ret;
}

static hi_s32 dmx_r_ram_port_get_buffer_impl(struct dmx_r_ram_port *ram_port, hi_u32 req_len, dmx_ram_buffer *buf,
        hi_u32 time_out)
{
    hi_s32 ret = HI_FAILURE;
    hi_u64 buf_phy_addr  = 0;

    osal_mutex_lock(&ram_port->lock);

    ram_port->get_count++;

    do {
        if (ram_port_staled(ram_port)) {
            ret = HI_ERR_DMX_STALED;
            goto out;
        }

        /* req buffer size must align with package size. */
        if (unlikely(req_len == 0 || req_len > ram_port->buf_size || req_len % ram_port->pkt_size)) {
            HI_ERR_DEMUX("invalid request len(%d).\n", req_len);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        ret = _dmx_r_ram_port_get_buffer_impl(ram_port, req_len, &buf_phy_addr);
        if (ret != HI_SUCCESS) {
            if (time_out == 0) {
                ret = HI_ERR_DMX_NOAVAILABLE_BUF;
                goto out;
            }

            osal_mutex_unlock(&ram_port->lock);

            ram_port->wait_cond = 0;
            ret = wait_event_interruptible_timeout(ram_port->wait_queue, ram_port->wait_cond,
                                                   osal_msecs_to_jiffies(time_out));

            osal_mutex_lock(&ram_port->lock);

            if (ret == 0) {
                ret = HI_ERR_DMX_TIMEOUT;
                goto out;
            } else if (ret < 0) { /* signal */
                ret = HI_ERR_DMX_INTR;
                goto out;
            }

            time_out = jiffies_to_msecs(ret);
        } else {
            ram_port->get_valid_count++;
            break;
        }
    } while (1);

    buf->data = ram_port->buf_usr_addr ?
                buf_phy_addr - ram_port->buf_phy_addr + ram_port->buf_usr_addr : buf_phy_addr - ram_port->buf_phy_addr +
                ram_port->buf_ker_addr;

    buf->phy_addr = buf_phy_addr;
    buf->length  = req_len;
    buf->buf_handle = ram_port->buf_handle;
    buf->offset = buf_phy_addr - ram_port->buf_phy_addr;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 _dmx_r_ram_port_push_buffer_impl(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 push_phy_addr = buf->phy_addr;
    hi_u32 push_len = buf->length;

    do {
        hi_u32 len = ram_port->blk_size - (push_phy_addr - ram_port->buf_phy_addr) % ram_port->blk_size;
        len = len < push_len ? len : push_len;

        WARN_ON(len % ram_port->pkt_size);

        ret = push_ts_buffer(ram_port, push_phy_addr, len);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        push_phy_addr += len;
        push_len -= len;
    } while (push_len);

out:
    return ret;
}

static hi_s32 dmx_r_ram_port_push_buffer_impl(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&ram_port->lock);

    ram_port->push_count++;

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    /* buf->phy_addr will be update after function call. */
    ret = check_push_buffer(ram_port, buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = _dmx_r_ram_port_push_buffer_impl(ram_port, buf);
    if (ret == HI_SUCCESS) {
        ram_port->all_pkt_cnt += buf->length / ram_port->pkt_size;
    }

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 _dmx_r_ram_port_release_buffer_impl(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    hi_u32 start_blk_idx, blk_cnt;
    hi_u32 index;
    hi_u64 phy_addr = ram_port->buf_phy_addr + buf->offset;

    start_blk_idx = (phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    blk_cnt      = (buf->length + ram_port->blk_size - 1) / ram_port->blk_size;

    osal_mutex_lock(&ram_port->blk_map_lock);

    for (index = start_blk_idx; index < start_blk_idx + blk_cnt; index++) {
        switch (ram_port->blk_map[index].flag) {
            case BLK_ALLOC_FLAG: {
                WARN_ON(ram_port->blk_map[index].ref != 0x1);

                ram_port->blk_map[index].ref--;

                ram_port->blk_map[index].flag        = BLK_FREE_FLAG;
                ram_port->blk_map[index].head_blk_idx  = BLK_INVALID_HEAD_IDX;
                ram_port->blk_map[index].req_len      = 0;

                ram_port->free_blk_nr++;

                break;
            }

            case BLK_ADD_INT_FLAG: {
                WARN_ON(ram_port->blk_map[index].ref == 0x0);

                if (ram_port->blk_map[index].ref == 1) { /* blk can be safe release now */
                    ram_port->blk_map[index].flag       = BLK_FREE_FLAG;
                    ram_port->blk_map[index].head_blk_idx = BLK_INVALID_HEAD_IDX;
                    ram_port->blk_map[index].ref        = 0;
                    ram_port->blk_map[index].req_len     = 0;

                    ram_port->free_blk_nr++;
                } else if (ram_port->blk_map[index].ref > 1) { /* blk will be release after int processed */
                    ram_port->blk_map[index].flag = BLK_ADD_RELCAIM_INT_FLAG;
                    ram_port->blk_map[index].ref--;
                }

                break;
            }

            default: {
                WARN(1, "Invalid blk_map flag[%#x]\n", ram_port->blk_map[index].flag);
                break;
            }
        }

        WARN_ON(ram_port->free_blk_nr > ram_port->total_blk_nr);
    }

    ram_port->wait_cond = 1;
    wake_up_interruptible(&ram_port->wait_queue);

    osal_mutex_unlock(&ram_port->blk_map_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_ram_port_release_buffer_impl(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    /* buf->phy_addr and buf->length will be update after function call. */
    ret = check_release_buffer(ram_port, buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = _dmx_r_ram_port_release_buffer_impl(ram_port, buf);

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_put_buffer_impl(struct dmx_r_ram_port *ram_port, hi_u32 valid_datalen, hi_u32 start_pos)
{
    DMX_UNUSED(ram_port);
    DMX_UNUSED(valid_datalen);
    DMX_UNUSED(start_pos);

    HI_ERR_DEMUX("not support PutTSBuffer, please disable CFG_HI_DMX_TSBUF_MULTI_THREAD_SUPPORT\n");
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_ram_port_close_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    dmx_mem_info ram_buf = {0};
    dmx_mem_info ram_flush_buf = {0};
    dmx_mem_info ram_desc_buf = {0};

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_r_ram_port_close_impl(ram_port);
    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_DEMUX, ram_port->blk_map);
        ram_port->blk_map = HI_NULL;

        ram_desc_buf.handle       = ram_port->dsc_buf_handle;
        ram_desc_buf.dmx_buf_obj  = ram_port->dsc_buf_obj;
        ram_desc_buf.buf_vir_addr = ram_port->dsc_buf_ker_addr;
        ram_desc_buf.buf_phy_addr = ram_port->dsc_buf_phy_addr;
        ram_desc_buf.buf_size     = ram_port->dsc_depth * DEFAULT_RAM_DSC_SIZE;
        ram_desc_buf.user_map_flag = HI_FALSE;
        dmx_unmap_and_release_buf(&ram_desc_buf);

        /* release ramport mmz buffer */
        ram_buf.handle        = ram_port->buf_handle;
        ram_buf.dmx_buf_obj   = ram_port->buf_obj;
        ram_buf.buf_vir_addr  = ram_port->buf_ker_addr;
        ram_buf.buf_phy_addr  = ram_port->buf_phy_addr;
        ram_buf.buf_size      = ram_port->buf_size;
        ram_buf.user_map_flag = (ram_port->buf_usr_addr != HI_NULL) ? HI_TRUE : HI_FALSE;
        dmx_unmap_and_release_buf(&ram_buf);

        /* release ramport flush mmz buffer */
        ram_flush_buf.handle        = ram_port->flush_buf_handle;
        ram_flush_buf.dmx_buf_obj   = ram_port->flush_buf_obj;
        ram_flush_buf.buf_vir_addr  = ram_port->flush_buf_ker_addr;
        ram_flush_buf.buf_phy_addr  = ram_port->flush_buf_phy_addr;
        ram_flush_buf.buf_size      = ram_port->flush_buf_size;
        ram_flush_buf.user_map_flag = HI_FALSE;
        dmx_unmap_and_release_buf(&ram_flush_buf);

        ram_port->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}
#else
static hi_s32 _dmx_r_ram_port_open_impl(struct dmx_r_ram_port *ram_port)
{
    osal_mutex_lock(&ram_port->blk_map_lock);
    ram_port->read = 0;
    ram_port->write = 0;
    ram_port->req_addr = 0;
    ram_port->req_len = 0;

    /* clear ram mmu cache */
    dmx_hal_ram_clr_mmu_cache(ram_port->base.mgmt, ram_port->base.id);

    if (ram_port->secure_mode == DMX_SECURE_TEE) {
        dmx_hal_ram_port_config(ram_port->base.mgmt, ram_port->base.id, ram_port->dsc_sec_buf_phy_addr,
                                ram_port->dsc_depth, ram_port->max_data_rate);
    } else {
        dmx_hal_ram_port_config(ram_port->base.mgmt, ram_port->base.id, ram_port->dsc_buf_phy_addr, ram_port->dsc_depth,
                                ram_port->max_data_rate);
    }

    ram_port->last_dsc_read_idx   = 0;
    ram_port->last_dsc_write_idx  = 0;
    osal_mutex_unlock(&ram_port->blk_map_lock);
    ram_port->all_pkt_cnt     = 0;

    ram_port->get_count      = 0;
    ram_port->get_valid_count = 0;
    ram_port->push_count     = 0;
    ram_port->put_count      = 0;
    ram_port->flush_count    = 0;

    ram_port->min_len = DMX_TS_PKT_SIZE;
    ram_port->max_len = DMX_TS_FEC_PKT_SIZE;
    ram_port->sync_lock_th = 7; /* default sync_lock_th as 7 */
    ram_port->sync_lost_th = 0; /* default sync_lost_th as 0 */
    ram_port->port_type = DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204;

    return HI_SUCCESS;
}

static hi_s32 _dmx_ram_create_non_tee_buf_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    ret = _dmx_alloc_ts_buf_impl(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("tsbuf malloc 0x%x failed\n", ram_port->buf_size);
        goto out;
    }

    ret = _dmx_alloc_flush_buf_impl(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("flush buf malloc 0x%x failed\n", ram_port->flush_buf_size);
        goto free_tsbuf;
    }

    return ret;

free_tsbuf:
    _dmx_free_ts_buf_impl(ram_port);
out:
    return ret;
}

static hi_void _dmx_ram_destroy_non_tee_buf_impl(struct dmx_r_ram_port *ram_port)
{
    _dmx_free_ts_buf_impl(ram_port);

    _dmx_free_flush_buf_impl(ram_port);
}

static hi_s32 _dmx_ram_create_tee_buf_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;
    hi_u32 dsc_buf_size = DEFAULT_RAM_DSC_DEPTH * DEFAULT_RAM_DSC_SIZE;
    dmx_tee_ramport_info tee_ramport = {0};

    /* tee secure buffer alloc */
    ram_port->flush_buf_size = DMX_TS_PKT_SIZE * DMX_RAM_FLUSH_TS_CNT;
    ret = dmx_tee_create_ramport(ram_port->base.id, ram_port->buf_size, ram_port->flush_buf_size, dsc_buf_size,
                                 &tee_ramport);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create tee ramport failed!\n");
        goto out;
    }
    ram_port->buf_phy_addr = tee_ramport.buf_phy_addr;
    ram_port->buf_size = tee_ramport.buf_size;
    ret = dmx_get_buf_fd_by_phyaddr(ram_port->buf_phy_addr, ram_port->buf_size, &ram_port->buf_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get tee tsbuf handle failed!\n");
        goto destroy_tee_ram;
    }

    ram_port->flush_buf_phy_addr = tee_ramport.flush_buf_phy_addr;
    ram_port->flush_buf_size = tee_ramport.flush_buf_size;
    ret = dmx_get_buf_fd_by_phyaddr(ram_port->flush_buf_phy_addr, ram_port->flush_buf_size,
        &ram_port->flush_buf_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get tee flush buf handle failed!\n");
        goto put_tsbuf_id;
    }

    ram_port->dsc_sec_buf_phy_addr = tee_ramport.dsc_buf_phy_addr;
    ret = dmx_get_buf_fd_by_phyaddr(ram_port->dsc_sec_buf_phy_addr, dsc_buf_size, &ram_port->dsc_buf_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get tee des buf handle failed!\n");
        goto put_flush_buf_id;
    }

    return ret;

put_flush_buf_id:
    dmx_dofunc_no_return(dmx_put_buf_fd(ram_port->flush_buf_handle));
put_tsbuf_id:
    dmx_dofunc_no_return(dmx_put_buf_fd(ram_port->buf_handle));

destroy_tee_ram:
    dmx_tee_destroy_ramport(ram_port->base.id, &tee_ramport);

out:
    return ret;
}

static hi_void _dmx_ram_destroy_tee_buf_impl(struct dmx_r_ram_port *ram_port)
{
    dmx_tee_ramport_info tee_ramport = {0};
    tee_ramport.buf_handle = ram_port->buf_handle;
    tee_ramport.buf_phy_addr = ram_port->buf_phy_addr;
    tee_ramport.buf_size = ram_port->buf_size;
    dmx_dofunc_no_return(dmx_put_buf_fd(ram_port->buf_handle));

    tee_ramport.flush_buf_phy_addr = ram_port->flush_buf_phy_addr;
    tee_ramport.flush_buf_size = ram_port->flush_buf_size;
    dmx_dofunc_no_return(dmx_put_buf_fd(ram_port->flush_buf_handle));

    tee_ramport.dsc_buf_phy_addr = ram_port->dsc_sec_buf_phy_addr;
    tee_ramport.dsc_buf_size = ram_port->dsc_depth * DEFAULT_RAM_DSC_SIZE;
    dmx_dofunc_no_return(dmx_put_buf_fd(ram_port->dsc_buf_handle));

    dmx_tee_destroy_ramport(ram_port->base.id, &tee_ramport);
}

static hi_s32 dmx_r_ram_port_open_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    WARN_ON(ram_port->staled == HI_TRUE);

    /* alloc buf */
    if (ram_port->secure_mode != DMX_SECURE_TEE) {
        ret = _dmx_ram_create_non_tee_buf_impl(ram_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("create non tee buf failed!\n");
            goto unlock;
        }
    } else {
        ret = _dmx_ram_create_tee_buf_impl(ram_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("create non tee buf failed!\n");
            goto unlock;
        }
    }

    /* alloc desc, hw restrict that it must align with 4K bytes. */
    ret = _dmx_alloc_dsc_buf_impl(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dsc buf malloc 0x%x failed\n", ram_port->dsc_depth * DEFAULT_RAM_DSC_SIZE);
        goto destroy_buf;
    }

    ram_port->max_data_rate = DMX_DEFAULT_RAMPORT_RATE;

    ret = _dmx_r_ram_port_open_impl(ram_port);
    if (ret != HI_SUCCESS) {
        goto free_dsc;
    }

    osal_mutex_unlock(&ram_port->lock);

    return HI_SUCCESS;

free_dsc:
    _dmx_free_dsc_buf_impl(ram_port);

destroy_buf:
    if (ram_port->secure_mode != DMX_SECURE_TEE) {
        _dmx_ram_destroy_non_tee_buf_impl(ram_port);
    } else {
        _dmx_ram_destroy_tee_buf_impl(ram_port);
    }
unlock:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 ram_port_get_buf_used(struct dmx_r_ram_port *ram_port)
{
    unsigned long start, end;

    osal_mutex_lock(&ram_port->blk_map_lock);

    ram_port->buf_used = dmx_get_queue_lenth(ram_port->read, ram_port->write, ram_port->buf_size);

    if (ram_port->read < ram_port->write) {
        start = ram_port->read;
        end = ram_port->buf_size - ram_port->write;

        start = (start <= DMX_TS_BUFFER_GAP) ? 0 : (start - DMX_TS_BUFFER_GAP);
        end = (end <= DMX_TS_BUFFER_GAP) ? 0 : (end - DMX_TS_BUFFER_GAP);

        if (start < end) {
            ram_port->buf_used = ram_port->buf_size - end;
        } else {
            ram_port->buf_used = ram_port->buf_size - start;
        }
    } else if (ram_port->read > ram_port->write) {
        start = ram_port->read - ram_port->write;
        if (start > DMX_TS_BUFFER_GAP) {
            ram_port->buf_used = DMX_TS_BUFFER_GAP + ram_port->buf_size - start;
        }
    } else {
        if (ram_port->read == 0) {
            ram_port->buf_used = DMX_TS_BUFFER_GAP;
        }
    }

    osal_mutex_unlock(&ram_port->blk_map_lock);

    return HI_SUCCESS;
}

/*
1. check the avaliable space in ts buffer head part (Head) and tail part (Tail)
2. if Tail is enough for ReqLen, return Tail sapce, otherwise ,retuen Head space.
3. if both not enough ,return
PortInfo->ReqAddr   = 0;
PortInfo->ReqLen    = 0;
*/
static hi_void get_ip_buf_len(struct dmx_r_ram_port *ram_port, const hi_u32 req_len)
{
    hi_u32              head; /* the avaliable space in ts buffer head part */
    hi_u32              tail; /* the avaliable space in ts buffer tail part */

    ram_port->req_addr   = 0;
    ram_port->req_len    = 0;

    if (ram_port->read < ram_port->write) {
        head = ram_port->read;
        tail = ram_port->buf_size - ram_port->write;

        head = (head <= DMX_TS_BUFFER_GAP) ? 0 : (head - DMX_TS_BUFFER_GAP);
        tail = (tail <= DMX_TS_BUFFER_GAP) ? 0 : (tail - DMX_TS_BUFFER_GAP);

        if (tail >= req_len) {
            ram_port->req_addr = ram_port->buf_phy_addr + ram_port->write;
            /* 4 bytes address align for tsr2rcipher */
            ram_port->req_addr = (ram_port->req_addr + 0x3ULL) & (~0x3ULL);
        } else if (head >= req_len) {
            ram_port->req_addr = ram_port->buf_phy_addr;
            /* 4 bytes address align for tsr2rcipher */
            ram_port->req_addr = (ram_port->req_addr + 0x3ULL) & (~0x3ULL);
        }
    } else if (ram_port->read > ram_port->write) {
        head = ram_port->read - ram_port->write;
        if (head > DMX_TS_BUFFER_GAP) {
            head -= DMX_TS_BUFFER_GAP;
            if (head >= req_len) {
                ram_port->req_addr = ram_port->buf_phy_addr + ram_port->write;
                /* 4 bytes address align for tsr2rcipher */
                ram_port->req_addr = (ram_port->req_addr + 0x3ULL) & (~0x3ULL);
            }
        }
    } else {
        if (ram_port->read == 0) {
            ram_port->req_addr = ram_port->buf_phy_addr;
            /* 4 bytes address align for tsr2rcipher */
            ram_port->req_addr = (ram_port->req_addr + 0x3ULL) & (~0x3ULL);
        }
    }

    if (ram_port->req_addr) {
        ram_port->req_len = req_len;
    }
}

 /*
 1. check wether Desc queue have least 1 desc
 2. check wether TS buffer have enough space for user to get
 */
static hi_bool check_ip_buf(struct dmx_r_ram_port *ram_port, const hi_u32 req_len)
{
    /* at least one desc should add into the desc queue */
    if (ram_port->dsc_depth - dmx_get_queue_lenth(ram_port->last_dsc_read_idx,
                                                  ram_port->last_dsc_write_idx, ram_port->dsc_depth) < 1) {
        HI_WARN_DEMUX("ip desc is empty, read desc=0x%x, write desc=0x%x, desc depth=0x%x\n",
                      ram_port->last_dsc_read_idx, ram_port->last_dsc_write_idx, ram_port->dsc_depth);

        return HI_TRUE;
    }
    /*
    call get_ip_buf_len
    1. check the avaliable space in ts buffer head part (Head) and tail part (Tail)
    2. if Tail is enough for ReqLen, return Tail sapce, otherwise ,retuen Head space.
    3. if both not enough ,return
    PortInfo->ReqAddr   = 0;
    PortInfo->ReqLen    = 0;
    */
    get_ip_buf_len(ram_port, req_len);

    if (ram_port->req_addr == 0x0) {
        HI_WARN_DEMUX("buffer not enough, r=0x%x, w=0x%x, size=0x%x\n",
                      ram_port->read, ram_port->write, ram_port->buf_size);

        return HI_TRUE;
    }

    return HI_FALSE;
}
static hi_s32 dmx_r_ram_port_get_buffer_impl(struct dmx_r_ram_port *ram_port, hi_u32 req_len, dmx_ram_buffer *buf,
        hi_u32 time_out)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&ram_port->lock);
    osal_mutex_lock(&ram_port->blk_map_lock);

    ram_port->get_count++;

    do {
        if (ram_port_staled(ram_port)) {
            ret = HI_ERR_DMX_STALED;
            goto out;
        }

        /* req buffer size must align with package size. */
        if (unlikely(req_len == 0 || req_len > ram_port->buf_size)) {
            HI_ERR_DEMUX("invalid request len(%d).\n", req_len);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        if (check_ip_buf(ram_port, req_len)) {
            if (time_out == 0) {
                ret = HI_ERR_DMX_NOAVAILABLE_BUF;
                goto out;
            }
            osal_mutex_unlock(&ram_port->blk_map_lock);
            osal_mutex_unlock(&ram_port->lock);

            ram_port->wait_cond = 0;
            ret = wait_event_interruptible_timeout(ram_port->wait_queue, ram_port->wait_cond,
                                                   osal_msecs_to_jiffies(time_out));

            osal_mutex_lock(&ram_port->lock);
            osal_mutex_lock(&ram_port->blk_map_lock);
            if (ret == 0) {
                ret = HI_ERR_DMX_TIMEOUT;
                goto out;
            } else if (ret < 0) { /* signal */
                ret = HI_ERR_DMX_INTR;
                goto out;
            }

            time_out = jiffies_to_msecs(ret);
        } else {
            ram_port->get_valid_count++;
            ret = HI_SUCCESS;
            break;
        }
    } while (1);

    buf->data = ram_port->buf_usr_addr ? ram_port->req_addr - ram_port->buf_phy_addr + ram_port->buf_usr_addr :
                ram_port->req_addr - ram_port->buf_phy_addr + ram_port->buf_ker_addr;

    buf->phy_addr = ram_port->req_addr;
    buf->length  = req_len;
    buf->buf_handle = ram_port->buf_handle;
    buf->offset = ram_port->req_addr - ram_port->buf_phy_addr;

out:
    osal_mutex_unlock(&ram_port->blk_map_lock);
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_push_buffer_impl(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    DMX_UNUSED(ram_port);
    DMX_UNUSED(buf);

    HI_ERR_DEMUX("not support PushTSBuffer, please enable CFG_HI_DMX_TSBUF_MULTI_THREAD_SUPPORT\n");
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_ram_port_release_buffer_impl(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf)
{
    DMX_UNUSED(ram_port);
    DMX_UNUSED(buf);

    HI_ERR_DEMUX("not support ReleaseTSBuffer, please enable CFG_HI_DMX_TSBUF_MULTI_THREAD_SUPPORT\n");
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_ram_port_put_buffer_impl(struct dmx_r_ram_port *ram_port, hi_u32 valid_datalen, hi_u32 start_pos)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 buffer_addr;
    hi_u32 buffer_len;
    hi_u32 offset;
    hi_u32 *cur_dsc_addr = HI_NULL;
    struct dmx_ram_port_dsc dsc = {0};

    /* lock */
    osal_mutex_lock(&ram_port->lock);
    osal_mutex_lock(&ram_port->blk_map_lock);

    if ((ram_port->req_len == 0) || (valid_datalen == 0)) {
        ret = HI_SUCCESS;
        goto out;
    }

    if (ram_port->req_len < start_pos + valid_datalen) {
        HI_ERR_DEMUX("port %u put buf len %u, request len %u\n", ram_port->base.id, valid_datalen, ram_port->req_len);

        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    offset = ram_port->req_addr - ram_port->buf_phy_addr;

    ram_port->write = offset + valid_datalen + start_pos;

    buffer_addr  = ram_port->req_addr + start_pos;
    buffer_len   = valid_datalen;

    /*****************************put begin******************************************/
    /* check the buffer to be putted valid */
    if ((buffer_len > ram_port->buf_size) || (buffer_addr < ram_port->buf_phy_addr) ||
        (buffer_addr + buffer_len < buffer_addr) ||
        (buffer_addr + buffer_len > ram_port->buf_phy_addr + ram_port->buf_size)) {
        HI_FATAL_DEMUX("port %u put buf invalid, putaddr[0x%x], putlen[0x%x], TsBuf addr[0x%x], Butlen[0x%x]\n",
                       ram_port->base.id, buffer_addr, buffer_len, ram_port->buf_phy_addr, ram_port->buf_size);

        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }
#ifdef HI_DEMUX_PROC_SUPPORT
    dmx_ram_port_save_ip_ts(ram_port, ram_port->buf_ker_addr + offset + start_pos, valid_datalen, ram_port->base.id);
#endif
    osal_mutex_unlock(&ram_port->blk_map_lock);
    osal_mutex_unlock(&ram_port->lock);
    ret = wait_dsc_sync(ram_port, 1);
    osal_mutex_lock(&ram_port->lock);
    osal_mutex_lock(&ram_port->blk_map_lock);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    cur_dsc_addr = (hi_u32 *)(ram_port->dsc_buf_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
    DMX_NULL_POINTER_GOTO(cur_dsc_addr, out);

    dsc.buf_phy_addr = buffer_addr;
    dsc.buf_len = buffer_len;
    dsc.desep = HI_TRUE;
    dsc.flush_flag = HI_FALSE;
    dsc.sync_data_flag = HI_FALSE;
    dsc.cur_dsc_addr = cur_dsc_addr;
    dmx_hal_ram_port_set_dsc(ram_port->base.mgmt, ram_port->base.id, &dsc);

    /* copy the dsc content to tee secure dsc buffer */
    if (ram_port->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_ramport_dsc tee_ramport_dsc = {0};
        tee_ramport_dsc.buf_phy_addr = dsc.buf_phy_addr;
        tee_ramport_dsc.buf_len = dsc.buf_len;
        tee_ramport_dsc.desep = dsc.desep;
        tee_ramport_dsc.flush_flag = dsc.flush_flag;
        tee_ramport_dsc.sync_data_flag = dsc.sync_data_flag;
        tee_ramport_dsc.write_index = ram_port->last_dsc_write_idx;

        ret = dmx_tee_set_ramport_dsc(ram_port->base.id, &tee_ramport_dsc);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set tee ramport dsc faild!\n");
            goto out;
        }
    }

    ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ? 0 :
                                    ram_port->last_dsc_write_idx + 1;

    dmx_hal_ram_port_add_dsc(ram_port->base.mgmt, ram_port->base.id, 1);
    /*****************************put end********************************************/
    /* calculate the ts pkg number */
    ram_port->all_pkt_cnt += valid_datalen / ram_port->pkt_size;

    ram_port->put_count++;

    ret = HI_SUCCESS;
out:
    /* unlock */
    osal_mutex_unlock(&ram_port->blk_map_lock);
    osal_mutex_unlock(&ram_port->lock);
    return ret;
}

static hi_s32 dmx_r_ram_port_close_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_r_ram_port_close_impl(ram_port);
    if (ret == HI_SUCCESS) {
        /* release ramport dsc buffer */
        _dmx_free_dsc_buf_impl(ram_port);

        if (ram_port->secure_mode != DMX_SECURE_TEE) {
            _dmx_ram_destroy_non_tee_buf_impl(ram_port);
        } else {
            _dmx_ram_destroy_tee_buf_impl(ram_port);
        }
        ram_port->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}
#endif

static hi_s32 _dmx_r_ram_port_get_status_impl(struct dmx_r_ram_port *ram_port, dmx_ram_port_status *status)
{
    hi_s32 ret;

    if (ram_port_staled(ram_port)) {
        return HI_ERR_DMX_STALED;
    }

    ret = ram_port_get_buf_used(ram_port);
    if (ret == HI_SUCCESS) {
        status->port_id = ram_port->base.id + DMX_RAM_PORT_0;
        status->buffer_used = ram_port->buf_used;
        status->buffer_size = ram_port->buf_size;
        status->pkt_num = ram_port->all_pkt_cnt;
        status->buffer_blk_size = ram_port->blk_size;
        status->secure_mode = ram_port->secure_mode;
    }

    return ret;
}

static hi_s32 dmx_r_ram_port_get_status_impl(struct dmx_r_ram_port *ram_port, dmx_ram_port_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    ret = _dmx_r_ram_port_get_status_impl(ram_port, status);

    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_get_attrs_impl(struct dmx_r_ram_port *ram_port, dmx_port_attr *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    attrs->port_mod = DMX_PORT_MODE_RAM;
    attrs->port_type = ram_port->port_type;
    attrs->sync_lost_threshold = ram_port->sync_lost_th;
    attrs->sync_lock_threshold = ram_port->sync_lock_th;
    attrs->tuner_in_clk = 0;
    attrs->tuner_clk_mode = 0; /* only for tuner, ramport set 0 as default */
    attrs->serial_bit_select = 0;
    attrs->tuner_err_mod = 0;
    attrs->user_define_len1 = ram_port->min_len;
    attrs->user_define_len2 = ram_port->max_len;
    /* for ram port the share clk is useless,so we give it DMX_TSI_PORT_0 to avoid error report. */
    attrs->serial_port_share_clk = DMX_TSI_PORT_0;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_set_attrs_impl(struct dmx_r_ram_port *ram_port, const dmx_port_attr *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ram_port->port_type = attrs->port_type;
    ram_port->sync_lock_th = attrs->sync_lock_threshold > DMX_MAX_LOCK_TH ? DMX_MAX_LOCK_TH :
                                attrs->sync_lock_threshold;
    ram_port->sync_lost_th = attrs->sync_lost_threshold > DMX_MAX_LOST_TH ? DMX_MAX_LOST_TH :
                                attrs->sync_lost_threshold;

    switch (ram_port->port_type) {
        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188: {
            dmx_hal_ram_port_set_ipcfg(ram_port->base.mgmt, ram_port->base.id, 0, /* 0: nosync188 */
                ram_port->sync_lock_th, ram_port->sync_lost_th, HI_FALSE, HI_FALSE);
            break;
        }

        case DMX_PORT_TYPE_PARALLEL_NOSYNC_204: {
            dmx_hal_ram_port_set_ipcfg(ram_port->base.mgmt, ram_port->base.id, 1, /* 1: nosync204 */
                ram_port->sync_lock_th, ram_port->sync_lost_th, HI_FALSE, HI_FALSE);
            break;
        }

        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204: {
            dmx_hal_ram_port_set_ipcfg(ram_port->base.mgmt, ram_port->base.id, 2, /* 2: nosync188_204 */
                ram_port->sync_lock_th, ram_port->sync_lost_th, HI_FALSE, HI_FALSE);
            break;
        }

        case DMX_PORT_TYPE_USER_DEFINED: {
            if ((attrs->user_define_len1 < DMX_RAM_MIN_LEN) ||
                (attrs->user_define_len1 > DMX_RAM_MAX_LEN) ||
                (attrs->user_define_len2 < DMX_RAM_MIN_LEN) ||
                (attrs->user_define_len2 > DMX_RAM_MAX_LEN)) {
                HI_ERR_DEMUX("set len error. len1=%u, len2=%u\n", attrs->user_define_len1, attrs->user_define_len2);

                ret = HI_ERR_DMX_INVALID_PARA;
                goto out;
            }
            ram_port->min_len = attrs->user_define_len1;
            ram_port->max_len = attrs->user_define_len2;
            dmx_hal_ram_port_set_sync_len(ram_port->base.mgmt, ram_port->base.id, ram_port->min_len, ram_port->max_len);
            break;
        }

        case DMX_PORT_TYPE_PARALLEL_BURST:
        case DMX_PORT_TYPE_PARALLEL_VALID:
        case DMX_PORT_TYPE_SERIAL:
        case DMX_PORT_TYPE_SERIAL2BIT:
        case DMX_PORT_TYPE_SERIAL_NOSYNC:
        case DMX_PORT_TYPE_SERIAL2BIT_NOSYNC: { /* abnormal branch */
            ret = HI_ERR_DMX_NOT_SUPPORT;
            goto out;
        }

        case DMX_PORT_TYPE_MAX: { /* abnormal branch */
            HI_ERR_DEMUX("Invalid type %u\n", ram_port->port_type);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        default : {
            dmx_hal_ram_port_set_ipcfg(ram_port->base.mgmt, ram_port->base.id, 2, /* 2: nosync188_204 */
                ram_port->sync_lock_th, ram_port->sync_lost_th, HI_FALSE, HI_FALSE);
            break;
        }
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 _dmx_r_ram_port_flush_buffer_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    ret = _flush_ts_buffer(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("flush ram port[%d] failed", ram_port->base.id);
        goto out;
    }

out:
    return ret;
}

static hi_s32 dmx_r_ram_port_flush_buffer_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    ram_port->flush_count++;

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_r_ram_port_flush_buffer_impl(ram_port);
    if (ret != HI_SUCCESS) {
        /* fixme */
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_reset_buffer_impl(struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    dmx_dofunc_no_return(_dmx_r_ram_port_close_impl(ram_port));
    dmx_dofunc_no_return(_dmx_r_ram_port_open_impl(ram_port));

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_pre_mmap_impl(struct dmx_r_ram_port *ram_port, hi_s64 *buf_handle, hi_u32 *buf_size,
    hi_void **buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    *buf_handle = ram_port->buf_handle;
    *buf_size = ram_port->buf_size;
    *buf_usr_addr = ram_port->buf_usr_addr;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_pst_mmap_impl(struct dmx_r_ram_port *ram_port, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    WARN_ON(ram_port->buf_usr_addr != HI_NULL);

    ram_port->buf_usr_addr = buf_usr_addr;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 dmx_r_ram_port_suspend_impl(struct dmx_r_ram_port *ram_port)
{
    osal_mutex_lock(&ram_port->lock);

    WARN_ON(ram_port->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_ram_port_close_impl(ram_port));

    osal_mutex_unlock(&ram_port->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_ram_port_resume_impl(struct dmx_r_ram_port *ram_port)
{
    osal_mutex_lock(&ram_port->lock);

    WARN_ON(ram_port->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_ram_port_open_impl(ram_port));

    osal_mutex_unlock(&ram_port->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_ram_port_get_buf_handle_impl(struct dmx_r_ram_port *ram_port,
    hi_mem_handle_t *buf_handle, hi_mem_handle_t *dsc_buf_handle, hi_mem_handle_t *flush_buf_handle)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port->secure_mode != DMX_SECURE_TEE) {
        HI_ERR_DEMUX("none secure buf can't get buf handle.\n");
        ret = HI_ERR_DMX_NOT_SUPPORT;
        goto out;
    }

    *buf_handle = ram_port->buf_handle;
    *flush_buf_handle = ram_port->flush_buf_handle;
    *dsc_buf_handle = ram_port->dsc_buf_handle;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static struct dmx_r_ram_port_ops g_dmx_ram_port_ops = {
    .open           = dmx_r_ram_port_open_impl,
    .get_attrs      = dmx_r_ram_port_get_attrs_impl,
    .set_attrs      = dmx_r_ram_port_set_attrs_impl,
    .get_status     = dmx_r_ram_port_get_status_impl,
    .get_buffer     = dmx_r_ram_port_get_buffer_impl,
    .push_buffer    = dmx_r_ram_port_push_buffer_impl,
    .put_buffer     = dmx_r_ram_port_put_buffer_impl,
    .release_buffer = dmx_r_ram_port_release_buffer_impl,
    .flush_buffer   = dmx_r_ram_port_flush_buffer_impl,
    .reset_buffer   = dmx_r_ram_port_reset_buffer_impl,
    .close          = dmx_r_ram_port_close_impl,
    .get_buf_handle = dmx_r_ram_port_get_buf_handle_impl,

    .pre_mmap       = dmx_r_ram_port_pre_mmap_impl,
    .pst_mmap       = dmx_r_ram_port_pst_mmap_impl,

    .suspend        = dmx_r_ram_port_suspend_impl,
    .resume         = dmx_r_ram_port_resume_impl,
};

static hi_s32 dmx_ram_get_packet_num(hi_u32 port_id, dmx_port_packet_num *port_packet)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *node = HI_NULL;

    port_packet->err_ts_packet_cnt = 0;
    port_packet->ts_sync_loss_cnt = 0;
    port_packet->packet_dis_cc_cnt = 0;

    osal_mutex_lock(&mgmt->pid_channel_list_lock);
    list_for_each(node, &mgmt->pid_channel_head) {
        struct dmx_r_pid_ch *pid_ch = list_entry(node, struct dmx_r_pid_ch, node0);
        if (!pid_ch->rband) {
            HI_ERR_DEMUX("pid channel not attach band!\n");
            ret = HI_FAILURE;
            goto exit_error;
        }
        if (pid_ch->rband->port - DMX_RAM_PORT_0 == port_id) {
            port_packet->ts_sync_loss_cnt = pid_ch->sync_lost_cnt;
            port_packet->packet_dis_cc_cnt = pid_ch->cc_disc_err;
        }
    }
    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    port_packet->ts_sync_byte_err_cnt = port_packet->ts_sync_loss_cnt;
    port_packet->ts_packet_cnt = dmx_hal_ram_port_get_ts_packets_cnt(mgmt, port_id);
    port_packet->err_ts_packet_cnt = port_packet->packet_dis_cc_cnt;

    return HI_SUCCESS;

exit_error:
    osal_mutex_unlock(&mgmt->pid_channel_list_lock);
    return ret;
}

static hi_s32 dmx_tuner_get_packet_num(hi_u32 port_id, dmx_port_packet_num *port_packet)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *node = HI_NULL;

    port_packet->ts_sync_byte_err_cnt = dmx_hal_dvb_get_sync_err_cnt(mgmt, port_id);
    port_packet->ts_sync_loss_cnt = port_packet->ts_sync_byte_err_cnt;
    port_packet->packet_dis_cc_cnt = 0;

    osal_mutex_lock(&mgmt->ram_port_list_lock);
    list_for_each(node, &mgmt->ram_port_head) {
        struct dmx_r_ram_port *ram_port = list_entry(node, struct dmx_r_ram_port, node);
        port_packet->ts_sync_loss_cnt = ram_port->sync_lost_cnt;
    }
    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    port_packet->ts_packet_cnt = dmx_hal_dvb_get_ts_cnt(mgmt, port_id);
    port_packet->err_ts_packet_cnt = dmx_hal_dvb_get_err_ts_cnt(mgmt, port_id);

    return HI_SUCCESS;
}

hi_s32 dmx_port_get_packet_num(dmx_port port, dmx_port_packet_num *port_packet)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 port_id;

    if (port >= DMX_IF_PORT_0 && port < DMX_IF_PORT_MAX) {
        port_id = port - DMX_IF_PORT_0;
        ret = dmx_tuner_get_packet_num(port_id, port_packet);
    } else if (port >= DMX_TSI_PORT_0 && port < DMX_TSI_PORT_MAX) {
        port_id = port - DMX_TSI_PORT_0;
        ret = dmx_tuner_get_packet_num(port_id, port_packet);
    } else if(port >= DMX_RAM_PORT_0 && port < DMX_RAM_PORT_MAX) {
        port_id = port - DMX_RAM_PORT_0;
        ret = dmx_ram_get_packet_num(port_id, port_packet);
    } else {
        HI_ERR_DEMUX("invalid port(%d).\n", port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    port_packet->packet_dis_cc_cnt_ca = port_packet->packet_dis_cc_cnt;
    if (port_packet->ts_packet_cnt > port_packet->err_ts_packet_cnt) {
        port_packet->ts_packet_drained_cnt = port_packet->ts_packet_cnt - port_packet->err_ts_packet_cnt;
    } else {
        port_packet->ts_packet_drained_cnt = 0;
    }

    if ((port_packet->err_ts_packet_cnt > port_packet->ts_packet_cnt) ||
        (port_packet->packet_dis_cc_cnt > port_packet->ts_packet_cnt) ||
        (port_packet->ts_packet_drained_cnt > port_packet->ts_packet_cnt)) {
        ret = HI_FAILURE;
    }

out:
    return ret;
}

/*************************dmx_mgmt_band*************************************************/
hi_s32 dmx_mgmt_create_band(dmx_band band, const dmx_band_attr *attrs, struct dmx_r_band **rband)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (!(band >= DMX_BAND_0 && band < DMX_BAND_MAX)) {
        HI_ERR_DEMUX("invalid band_id(%d).\n", band);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_band(mgmt, band, attrs, rband);

out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_band(struct dmx_r_base *obj)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    return mgmt->ops->destroy_band(mgmt, (struct dmx_r_band *)obj);
}

hi_s32 dmx_band_open(dmx_band band, const dmx_band_attr *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();
    ret = dmx_mgmt_create_band(band, attrs, &rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    dmx_r_get_raw((struct dmx_r_base *)rband);

    ret = rband->ops->open(rband);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rband, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = dmx_band_close;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    /* save the bandhandle */
    rband->band_handle = slot->handle;

    dmx_r_put((struct dmx_r_base *)rband);

    DMX_BAND_TRACE_EXIT();
    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    rband->ops->close(rband);
out1:
    dmx_r_put((struct dmx_r_base *)rband);

    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_band_ref_inc(dmx_band band, const dmx_band_attr *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_band *rband = HI_NULL;
    struct list_head *node = HI_NULL;
    struct dmx_mgmt *rmgmt = HI_NULL;

    DMX_BAND_TRACE_ENTER();

    rmgmt = get_dmx_mgmt();
    DMX_NULL_POINTER_RETURN(rmgmt);

    osal_mutex_lock(&rmgmt->band_list_lock);
    list_for_each(node, &rmgmt->band_head) {
        struct dmx_r_band *tmp_rband = list_entry(node, struct dmx_r_band, node);

        /* compare the object */
        if (band == tmp_rband->base.id + DMX_BAND_0) {
            *handle = tmp_rband->band_handle;
            if (osal_atomic_inc_return(&tmp_rband->band_handle_ref_cnt) != 0) {
                ret = HI_SUCCESS;
            } else {
                HI_ERR_DEMUX("osal_atomic_inc_return failed!\n");
                ret = HI_FAILURE;
            }
            osal_mutex_unlock(&rmgmt->band_list_lock);
            goto out0;
        }
    }
    osal_mutex_unlock(&rmgmt->band_list_lock);

    ret = dmx_mgmt_create_band(band, attrs, &rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    dmx_r_get_raw((struct dmx_r_base *)rband);

    ret = rband->ops->open(rband);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rband, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = dmx_band_close;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    /* save the bandhandle */
    rband->band_handle = slot->handle;

    dmx_r_put((struct dmx_r_base *)rband);

    DMX_BAND_TRACE_EXIT();
    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    rband->ops->close(rband);
out1:
    dmx_r_put((struct dmx_r_base *)rband);

    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_band_attach_port(hi_handle handle, dmx_port port)
{
    hi_s32 ret;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();
    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rband->ops->attach_port(rband, port);

    dmx_r_put((struct dmx_r_base *)rband);
    DMX_BAND_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_band_detach_port(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();
    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (DMX_PORT_MAX == rband->port) {
        HI_ERR_DEMUX("this band has not attached any port!\n");
        ret = HI_ERR_DMX_NOATTACH_PORT;
        goto out1;
    }

    ret = rband->ops->detach_port(rband);

    DMX_BAND_TRACE_EXIT();
out1:
    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_band_get_attrs(hi_handle handle, dmx_band_attr *attrs)
{
    hi_s32 ret;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();
    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rband->ops->get_attrs(rband, attrs);

    dmx_r_put((struct dmx_r_base *)rband);

    DMX_BAND_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_band_set_attrs(hi_handle handle, const dmx_band_attr *attrs)
{
    hi_s32 ret;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();
    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rband->ops->set_attrs(rband, attrs);

    dmx_r_put((struct dmx_r_base *)rband);
    DMX_BAND_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_band_get_status(hi_handle handle, dmx_band_status *status)
{
    hi_s32 ret;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();
    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rband->ops->get_status(rband, status);

    dmx_r_put((struct dmx_r_base *)rband);

    DMX_BAND_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_band_close(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();
    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rband->ops->close(rband);

    dmx_r_put((struct dmx_r_base *)rband);

    DMX_BAND_TRACE_EXIT();
out1:
    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_band_ref_dec(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_band *rband = HI_NULL;

    DMX_BAND_TRACE_ENTER();

    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    /* if band_handle_ref_cnt is not zero, then don't release the pidCh handle really */
    if (osal_atomic_dec_return(&rband->band_handle_ref_cnt) != 0) {
        ret = HI_SUCCESS;
        goto out1;
    }

    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rband->ops->close(rband);

    dmx_r_put((struct dmx_r_base *)rband);

    DMX_BAND_TRACE_EXIT();
out1:
    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_band_tei_set(hi_handle handle, hi_bool tei)
{
    hi_s32 ret;
    struct dmx_r_band *rband = HI_NULL;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    DMX_BAND_TRACE_ENTER();
    ret = DMX_R_BAND_GET(handle, rband);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&rband->lock);
    dmx_hal_set_ts_tei_ctl(mgmt, rband->base.id, tei);
    osal_mutex_unlock(&rband->lock);

    dmx_r_put((struct dmx_r_base *)rband);
    DMX_BAND_TRACE_EXIT();
out:
    return ret;
}

/*************************dmx_r_band_xxx_impl*************************************************/
static inline hi_bool band_staled(struct dmx_r_band *rband)
{
    if (unlikely(rband->staled == HI_TRUE)) {
        HI_ERR_DEMUX("band is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _dmx_r_band_open_impl(struct dmx_r_band *rband)
{
    /* do something about hardware */
    dmx_hal_band_dis_all_tsrec(rband->base.mgmt, rband->base.id);

    /* enable pcr scr count */
    dmx_hal_band_set_pcr_scr(rband->base.mgmt, HI_TRUE);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_band_open_impl(struct dmx_r_band *rband)
{
    hi_s32 ret;

    osal_mutex_lock(&rband->lock);

    WARN_ON(rband->staled == HI_TRUE);

    ret = _dmx_r_band_open_impl(rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    /* set the band_handle_ref_cnt as 1 in the first time */
    osal_atomic_set(&rband->band_handle_ref_cnt, 1);

    ret = HI_SUCCESS;

out0:
    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 _dmx_r_band_attach_port_impl(struct dmx_r_band *rband, dmx_port port)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 port_index = 0;

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    if (port >= DMX_IF_PORT_0 && port < DMX_IF_PORT_0 + rband->base.mgmt->if_port_cnt) {
        rband->port_type = DMX_PORT_IF_TYPE;
        port_index = port - DMX_IF_PORT_0;
        osal_mutex_lock(&rband->base.mgmt->if_port_list_lock);
        set_bit(port_index, rband->base.mgmt->if_port_bitmap);
        osal_mutex_unlock(&rband->base.mgmt->if_port_list_lock);
    } else if (port >= DMX_TSI_PORT_0 && port < DMX_TSI_PORT_0 + rband->base.mgmt->tsi_port_cnt) {
        rband->port_type = DMX_PORT_TSI_TYPE;
        port_index = port - DMX_TSI_PORT_0;
        osal_mutex_lock(&rband->base.mgmt->tsi_port_list_lock);
        set_bit(port_index, rband->base.mgmt->tsi_port_bitmap);
        osal_mutex_unlock(&rband->base.mgmt->tsi_port_list_lock);
    } else if (port >= DMX_RAM_PORT_0 && port < DMX_RAM_PORT_0 + rband->base.mgmt->ram_port_cnt) {
        /* ram_port_bitmap has been set in the create process */
        rband->port_type = DMX_PORT_RAM_TYPE;
        port_index = port - DMX_RAM_PORT_0;
    } else if (port >= DMX_TAG_PORT_0 && port < DMX_TAG_PORT_0 + rband->base.mgmt->tag_port_cnt) {
        rband->port_type = DMX_PORT_TAG_TYPE;
        port_index = port - DMX_TAG_PORT_0;
        osal_mutex_lock(&rband->base.mgmt->tag_port_list_lock);
        set_bit(port_index, rband->base.mgmt->tag_port_bitmap);
        osal_mutex_unlock(&rband->base.mgmt->tag_port_list_lock);
    } else if (port >= DMX_RMX_PORT_0 && port < DMX_RMX_PORT_0 + rband->base.mgmt->rmx_port_cnt) {
        /* rmx _bitmap has been set in the create process */
        rband->port_type = DMX_PORT_RMX_TYPE;
        port_index = port - DMX_RMX_PORT_0;
    } else if (port >= DMX_TSIO_PORT_0 && port < DMX_TSIO_PORT_0 + rband->base.mgmt->tsio_port_cnt) {
        rband->port_type = DMX_PORT_TSIO_TYPE;
        port_index = port - DMX_TSIO_PORT_0;
    } else {
        HI_ERR_DEMUX("invalid port id[0x%x].\n", port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    dmx_hal_band_attach_port(rband->base.mgmt, rband->base.id, rband->port_type, port_index);

    ret = HI_SUCCESS;

out:
    return ret;
}


static hi_s32 dmx_r_band_attach_port_impl(struct dmx_r_band *rband, dmx_port port)
{
    hi_s32 ret;

    osal_mutex_lock(&rband->lock);

    ret = _dmx_r_band_attach_port_impl(rband, port);
    if (ret == HI_SUCCESS) {
        /* record the port */
        rband->port = port;
    }

    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 _dmx_r_band_detach_port_impl(struct dmx_r_band *rband)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 port_index = 0;

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    if (rband->port >= DMX_IF_PORT_0 && rband->port < DMX_IF_PORT_0 + rband->base.mgmt->if_port_cnt) {
        port_index = rband->port - DMX_IF_PORT_0;
        osal_mutex_lock(&rband->base.mgmt->if_port_list_lock);
        clear_bit(port_index, rband->base.mgmt->if_port_bitmap);
        osal_mutex_unlock(&rband->base.mgmt->if_port_list_lock);
    }

    if (rband->port >= DMX_TSI_PORT_0 && rband->port < DMX_TSI_PORT_0 + rband->base.mgmt->tsi_port_cnt) {
        port_index = rband->port - DMX_TSI_PORT_0;
        osal_mutex_lock(&rband->base.mgmt->tsi_port_list_lock);
        clear_bit(port_index, rband->base.mgmt->tsi_port_bitmap);
        osal_mutex_unlock(&rband->base.mgmt->tsi_port_list_lock);
    }

    if (rband->port >= DMX_TAG_PORT_0 && rband->port < DMX_TAG_PORT_0 + rband->base.mgmt->tag_port_cnt) {
        port_index = rband->port - DMX_TAG_PORT_0;
        osal_mutex_lock(&rband->base.mgmt->tag_port_list_lock);
        clear_bit(port_index, rband->base.mgmt->tag_port_bitmap);
        osal_mutex_unlock(&rband->base.mgmt->tag_port_list_lock);
    }

    rband->port = DMX_PORT_MAX;

    dmx_hal_band_detach_port(rband->base.mgmt, rband->base.id);

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_r_band_detach_port_impl(struct dmx_r_band *rband)
{
    hi_s32 ret;

    osal_mutex_lock(&rband->lock);

    ret = _dmx_r_band_detach_port_impl(rband);
    if (ret == HI_SUCCESS) {
        /* clear the port */
        rband->port = DMX_PORT_MAX;
    }

    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 dmx_r_band_get_attrs_impl(struct dmx_r_band *rband, dmx_band_attr *attrs)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rband->lock);

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    attrs->band_attr = rband->band_attr;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 dmx_r_band_set_attrs_impl(struct dmx_r_band *rband, const dmx_band_attr *attrs)
{
    hi_s32 ret = HI_ERR_DMX_NOT_SUPPORT;

    osal_mutex_lock(&rband->lock);

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    HI_ERR_DEMUX("not support set band attr yet!\n");

out:
    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 _dmx_r_band_get_status_impl(struct dmx_r_band *rband, dmx_band_status *status)
{
    hi_s32 ret = HI_FAILURE;

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    status->port_id = rband->port;
    status->band = TO_BAND_HW_ID(rband->base.id);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_band_get_status_impl(struct dmx_r_band *rband, dmx_band_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rband->lock);

    ret = _dmx_r_band_get_status_impl(rband, status);

    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 _dmx_r_band_attach_rec_impl(struct dmx_r_band *rband, struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret = HI_FAILURE;

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    /* enable all ts record */
    dmx_hal_band_en_all_tsrec(rband->base.mgmt, rband->base.id);
    /* full ts to dav */
    dmx_hal_set_full_tsout_flt(rband->base.mgmt, DMX_FULL_TS_OUT_TYPE_DAV);
    /* set the buffer id and other table */
    dmx_hal_set_full_tsctl_tab(rband->base.mgmt, rband->base.id, rrec_fct->rbuf->base.id, HI_TRUE, HI_FALSE);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_band_attach_rec_impl(struct dmx_r_band *rband, struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rband->lock);

    ret = _dmx_r_band_attach_rec_impl(rband, rrec_fct);

    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 _dmx_r_band_detach_rec_impl(struct dmx_r_band *rband, struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret = HI_FAILURE;

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    /* disable all ts record */
    dmx_hal_band_dis_all_tsrec(rband->base.mgmt, rband->base.id);
    /* unset the buffer id and other table */
    dmx_hal_set_full_tsctl_tab(rband->base.mgmt, rband->base.id, 0, HI_FALSE, HI_FALSE);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_band_detach_rec_impl(struct dmx_r_band *rband, struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rband->lock);

    ret = _dmx_r_band_detach_rec_impl(rband, rrec_fct);

    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 _dmx_r_band_close_impl(struct dmx_r_band *rband)
{
    /* disable ts record */
    dmx_hal_band_dis_all_tsrec(rband->base.mgmt, rband->base.id);

    /* disable pcr scr count */
    dmx_hal_band_set_pcr_scr(rband->base.mgmt, HI_FALSE);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_band_close_impl(struct dmx_r_band *rband)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rband->lock);

    if (band_staled(rband)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_r_band_close_impl(rband);
    if (ret == HI_SUCCESS) {
        /* set the band_handle_ref_cnt as 0 in the last time */
        osal_atomic_set(&rband->band_handle_ref_cnt, 0);
        rband->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&rband->lock);

    return ret;
}

static hi_s32 dmx_r_band_suspend_impl(struct dmx_r_band *rband)
{
    dmx_port tmp_port;
    osal_mutex_lock(&rband->lock);

    WARN_ON(rband->staled == HI_TRUE);

    tmp_port = rband->port;

    dmx_dofunc_no_return(_dmx_r_band_close_impl(rband));
    dmx_dofunc_no_return(_dmx_r_band_detach_port_impl(rband));

    rband->port = tmp_port;

    osal_mutex_unlock(&rband->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_band_resume_impl(struct dmx_r_band *rband)
{
    osal_mutex_lock(&rband->lock);

    WARN_ON(rband->staled == HI_TRUE);

    /* if the band did not attached before, so it did not need attach when resume. */
    if (rband->port != DMX_PORT_MAX) {
        dmx_dofunc_no_return(_dmx_r_band_attach_port_impl(rband, rband->port));
    }
    dmx_dofunc_no_return(_dmx_r_band_open_impl(rband));

    osal_mutex_unlock(&rband->lock);

    return HI_SUCCESS;
}

/*************************dmx_r_band_xxx_impl*************************************************/
struct dmx_r_band_ops g_dmx_band_ops = {
    .open          = dmx_r_band_open_impl,
    .attach_port   = dmx_r_band_attach_port_impl,
    .detach_port   = dmx_r_band_detach_port_impl,
    .get_attrs     = dmx_r_band_get_attrs_impl,
    .set_attrs     = dmx_r_band_set_attrs_impl,
    .get_status    = dmx_r_band_get_status_impl,
    .attach_rec    = dmx_r_band_attach_rec_impl,
    .detach_rec    = dmx_r_band_detach_rec_impl,
    .close         = dmx_r_band_close_impl,

    .suspend       = dmx_r_band_suspend_impl,
    .resume        = dmx_r_band_resume_impl,
};

/*
 * obj is staled after Close, it should discard all possible call request after that.
 */
static hi_s32 dmx_mgmt_create_raw_pid_ch(struct dmx_r_raw_pid_ch **rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (!rraw_pid_ch) {
        HI_ERR_DEMUX("invalid rband(%#x) or pid[%#x].\n", rraw_pid_ch);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_raw_pid_ch(mgmt, rraw_pid_ch);
out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_raw_pid_ch(struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_raw_pid_ch(mgmt, (struct dmx_r_raw_pid_ch *)obj);

    return ret;
}

/*************************dmx_r_raw_pid_channel_xxx_impl*************************************************/
static inline hi_bool raw_pid_ch_staled(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    if (unlikely(rraw_pid_ch->staled == HI_TRUE)) {
        HI_ERR_DEMUX("raw pid channel is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _dmx_r_raw_pid_ch_open_impl(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    /* no need to do something currently */
    return HI_SUCCESS;
}

static hi_s32 dmx_r_raw_pid_ch_open_impl(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret;

    osal_mutex_lock(&rraw_pid_ch->lock);

    WARN_ON(rraw_pid_ch->staled == HI_TRUE);

    ret = _dmx_r_raw_pid_ch_open_impl(rraw_pid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rraw_pid_ch->lock);

    return ret;
}

static hi_s32 _dmx_r_raw_pid_ch_close_impl(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    /* fixme :to do something */
    return HI_SUCCESS;
}


static hi_s32 dmx_r_raw_pid_ch_close_impl(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rraw_pid_ch->lock);

    if (raw_pid_ch_staled(rraw_pid_ch)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_r_raw_pid_ch_close_impl(rraw_pid_ch);
    if (ret == HI_SUCCESS) {
        rraw_pid_ch->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&rraw_pid_ch->lock);

    return ret;
}


static hi_s32 dmx_r_raw_pid_ch_suspend_impl(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    osal_mutex_lock(&rraw_pid_ch->lock);

    WARN_ON(rraw_pid_ch->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_raw_pid_ch_close_impl(rraw_pid_ch));

    osal_mutex_unlock(&rraw_pid_ch->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_raw_pid_ch_resume_impl(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    osal_mutex_lock(&rraw_pid_ch->lock);

    WARN_ON(rraw_pid_ch->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_raw_pid_ch_open_impl(rraw_pid_ch));

    osal_mutex_unlock(&rraw_pid_ch->lock);

    return HI_SUCCESS;
}

/*************************dmx_r_raw_pid_channel_xxx_impl*************************************************/
static struct dmx_r_raw_pid_ch_ops g_dmx_raw_pid_ch_ops = {
    .open           = dmx_r_raw_pid_ch_open_impl,
    .close          = dmx_r_raw_pid_ch_close_impl,

    .suspend        = dmx_r_raw_pid_ch_suspend_impl,
    .resume         = dmx_r_raw_pid_ch_resume_impl,
};

/*************************dmx_mgmt_pid_ch*************************************************/
static hi_s32 _dmx_r_pid_ch_create_pcch(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = HI_NULL;
    hi_u32 pid_copy_chan_id;

    hi_char buf_name[32] = {0}; /* buf of name, size 32 */

    dmx_mem_info pid_copy_buf = {0};

    DMX_NULL_POINTER_RETURN(rpid_ch);
    mgmt = rpid_ch->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    /* create an pid copy channel */
    osal_mutex_lock(&mgmt->pid_copy_chan_lock);
    pid_copy_chan_id = find_first_zero_bit(mgmt->pid_copy_bitmap, mgmt->pid_copy_chan_cnt);
    if (!(pid_copy_chan_id < mgmt->pid_copy_chan_cnt)) {
        osal_mutex_unlock(&mgmt->pid_copy_chan_lock);
        HI_ERR_DEMUX("there is no available pidcopy channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }
    set_bit(pid_copy_chan_id, mgmt->pid_copy_bitmap);
    osal_mutex_unlock(&mgmt->pid_copy_chan_lock);

    rpid_ch->pid_copy_chan_id = pid_copy_chan_id;

    /* alloc buf */
    snprintf(buf_name, sizeof(buf_name), "dmx_pcbuf");
    ret = dmx_alloc_and_map_buf(buf_name, DMX_DEFAULT_PID_COPY_BUF_LEN, 0, &pid_copy_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("mmz malloc 0x%x failed\n", DMX_DEFAULT_PID_COPY_BUF_LEN);
        goto out;
    }

    WARN_ON(pid_copy_buf.buf_size != DMX_DEFAULT_PID_COPY_BUF_LEN);
    rpid_ch->pid_copy_buf_handle     = pid_copy_buf.handle;
    rpid_ch->pid_copy_buf_obj        = pid_copy_buf.dmx_buf_obj;
    rpid_ch->pid_copy_buf_vir_addr   = pid_copy_buf.buf_vir_addr;
    rpid_ch->pid_copy_buf_phy_addr   = pid_copy_buf.buf_phy_addr;
    rpid_ch->pid_copy_buf_size       = pid_copy_buf.buf_size;
    HI_DBG_DEMUX("pid_copy buf startaddr[%#x], size[%#x]\n",
                 rpid_ch->pid_copy_buf_phy_addr, rpid_ch->pid_copy_buf_size);

    dmx_hal_pid_copy_config(mgmt, rpid_ch->pid_copy_chan_id, rpid_ch->pid_copy_buf_phy_addr,
                            rpid_ch->pid_copy_buf_size);
out:
    return ret;
}

static hi_s32 _dmx_r_pid_ch_destroy_pcch(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt       *mgmt  = HI_NULL;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    dmx_mem_info pid_copy_buf = {0};

    DMX_NULL_POINTER_RETURN(rpid_ch);
    mgmt = rpid_ch->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    dmx_hal_pid_copy_de_config(mgmt, rpid_ch->pid_copy_chan_id);
    /* release buf */
    pid_copy_buf.handle       = rpid_ch->pid_copy_buf_handle;
    pid_copy_buf.dmx_buf_obj  = rpid_ch->pid_copy_buf_obj;
    pid_copy_buf.buf_vir_addr = rpid_ch->pid_copy_buf_vir_addr;
    pid_copy_buf.buf_phy_addr = rpid_ch->pid_copy_buf_phy_addr;
    pid_copy_buf.buf_size     = rpid_ch->pid_copy_buf_size;
    pid_copy_buf.user_map_flag = HI_FALSE;

    HI_DBG_DEMUX("***release pid_copy buf startaddr[%#x], size[%#x]\n",
                 rpid_ch->pid_copy_buf_phy_addr, rpid_ch->pid_copy_buf_size);
    dmx_unmap_and_release_buf(&pid_copy_buf);

    rpid_ch->pid_copy_buf_vir_addr = 0;
    rpid_ch->pid_copy_buf_phy_addr = 0;
    rpid_ch->pid_copy_buf_size = 0;

    /* clear the pidcopy channel */
    mask = BIT_MASK(rpid_ch->pid_copy_chan_id);
    osal_mutex_lock(&mgmt->pid_copy_chan_lock);
    p = ((unsigned long *)mgmt->pid_copy_bitmap) + BIT_WORD(rpid_ch->pid_copy_chan_id);
    if (!(*p & mask)) {
        osal_mutex_unlock(&mgmt->pid_copy_chan_lock);
        HI_ERR_DEMUX("pid_copy channel(%d) is invalid.\n", rpid_ch->pid_copy_chan_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rpid_ch->pid_copy_chan_id, mgmt->pid_copy_bitmap);
    osal_mutex_unlock(&mgmt->pid_copy_chan_lock);

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 create_raw_pid_ch(struct dmx_r_pid_ch *rpid_ch, enum dmx_rawpidch_type raw_pid_ch_type,
                                struct dmx_r_raw_pid_ch **rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id  = 0;
    struct dmx_r_raw_pid_ch *tmp_raw_pid_ch;

    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    if (raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE) {
        unsigned long mask, *p;

        id = 0;
        mask = BIT_MASK(id);
        p = ((unsigned long *)rpid_ch->raw_pid_ch_bitmap) + BIT_WORD(id);
        if (*p & mask) {
            HI_ERR_DEMUX("the master raw_pid_ch(%d) is busy now!.\n", id);
            ret = HI_ERR_DMX_BUSY;
            goto out;
        }
    } else {
        id = find_first_zero_bit(rpid_ch->raw_pid_ch_bitmap, rpid_ch->raw_pid_ch_cnt);
        if (!(id < rpid_ch->raw_pid_ch_cnt)) {
            HI_ERR_DEMUX("there is no available raw pidch now!\n");
            ret = HI_ERR_DMX_NO_RESOURCE;
            goto out;
        }
    }

    ret = dmx_mgmt_create_raw_pid_ch(&tmp_raw_pid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    tmp_raw_pid_ch->base.id      = id;

    /* set the pidchannel object */
    tmp_raw_pid_ch->rpid_ch        = rpid_ch;

    /* set the type flag */
    if (raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE) {
        tmp_raw_pid_ch->raw_pid_ch_type = DMX_RAWPIDCH_MASTER_TYPE;
    } else {
        tmp_raw_pid_ch->raw_pid_ch_type = DMX_RAWPIDCH_SLAVE_TYPE;
    }

    list_add_tail(&tmp_raw_pid_ch->node1, &rpid_ch->raw_pid_ch_head);

    *rraw_pid_ch = tmp_raw_pid_ch;

    HI_DBG_DEMUX("#    rraw_pid_ch obj(0x%x) created.\n", tmp_raw_pid_ch);

    set_bit(id, rpid_ch->raw_pid_ch_bitmap);

out:
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
    return ret;
}

static hi_s32 destroy_raw_pid_ch(struct dmx_r_pid_ch *rpid_ch, enum dmx_rawpidch_type raw_pid_ch_type,
                                 struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask = 0;
    unsigned long *p = HI_NULL;

    struct dmx_mgmt *mgmt = rpid_ch->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    if (raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE) {
        rraw_pid_ch = list_first_entry(&rpid_ch->raw_pid_ch_head, struct dmx_r_raw_pid_ch, node1);
        DMX_NULL_POINTER_GOTO(rraw_pid_ch, out);
        DMX_FATAL_CON_GOTO(rraw_pid_ch->base.id != 0x0, HI_ERR_DMX_FATAL, out);

        if (unlikely(!list_is_last(&rraw_pid_ch->node1, &rpid_ch->raw_pid_ch_head))) {
            HI_ERR_DEMUX("slave raw pid_ch list is not empty\n");
            ret = HI_ERR_DMX_BUSY_CHAN;
            goto out;
        }
    }

    mask = BIT_MASK(rraw_pid_ch->base.id);
    p = ((unsigned long *)rpid_ch->raw_pid_ch_bitmap) + BIT_WORD(rraw_pid_ch->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("raw_pid_ch(%d) is invalid.\n", rraw_pid_ch->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    osal_mutex_lock(&rraw_pid_ch->lock);
    /* if the rawpid channle is busy, left destroy it in the _dmx_r_pid_ch_detach_raw_pid_ch_impl */
    if (osal_atomic_read(&rraw_pid_ch->ref_play_cnt) != 0x0 || osal_atomic_read(&rraw_pid_ch->ref_rec_cnt) != 0x0) {
        HI_DBG_DEMUX("raw_pid_ch(%d) is busy, ref_play_cnt[%d], ref_rec_cnt[%d]\n", rraw_pid_ch->base.id,
                     osal_atomic_read(&rraw_pid_ch->ref_play_cnt), osal_atomic_read(&rraw_pid_ch->ref_rec_cnt));

        osal_mutex_unlock(&rraw_pid_ch->lock);
        ret = HI_SUCCESS;
        goto out;
    }
    osal_mutex_unlock(&rraw_pid_ch->lock);

    /* try to destroy the raw pidch */
    clear_bit(rraw_pid_ch->base.id, rpid_ch->raw_pid_ch_bitmap);

    list_del(&rraw_pid_ch->node1); /* del the node form pidch list */

    ret = dmx_mgmt_destroy_raw_pid_ch((struct dmx_r_base *)rraw_pid_ch);

out:
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
    return ret;
}

static hi_s32 destroy_all_raw_pid_ch(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask;
    unsigned long *p = HI_NULL;
    struct dmx_r_raw_pid_ch  *rraw_pid_ch = HI_NULL;
    struct dmx_r_raw_pid_ch  *tmp_rraw_pid_ch = HI_NULL;
    struct dmx_mgmt        *mgmt = rpid_ch->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    list_for_each_entry_safe(rraw_pid_ch, tmp_rraw_pid_ch, &rpid_ch->raw_pid_ch_head, node1) {
        mask = BIT_MASK(rraw_pid_ch->base.id);
        p = ((unsigned long *)rpid_ch->raw_pid_ch_bitmap) + BIT_WORD(rraw_pid_ch->base.id);
        if (!(*p & mask)) {
            HI_ERR_DEMUX("raw_pid_ch(%d) is invalid.\n", rraw_pid_ch->base.id);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        osal_mutex_lock(&rraw_pid_ch->lock);
        /* if the rawpid channle is busy, left destroy it in the _dmx_r_pid_ch_detach_raw_pid_ch_impl */
        if (osal_atomic_read(&rraw_pid_ch->ref_play_cnt) != 0x0 || osal_atomic_read(&rraw_pid_ch->ref_rec_cnt) != 0x0) {
            HI_DBG_DEMUX("raw_pid_ch(%d) is busy, ref_play_cnt[%d], ref_rec_cnt[%d]\n", rraw_pid_ch->base.id,
                         osal_atomic_read(&rraw_pid_ch->ref_play_cnt), osal_atomic_read(&rraw_pid_ch->ref_rec_cnt));
            osal_mutex_unlock(&rraw_pid_ch->lock);
            continue;
        }
        osal_mutex_unlock(&rraw_pid_ch->lock);

        /* try to destroy the raw pidch */
        clear_bit(rraw_pid_ch->base.id, rpid_ch->raw_pid_ch_bitmap);

        list_del(&rraw_pid_ch->node1); /* del node from pidch list */

        /* clear all the raw pid channel */
        dmx_hal_pid_clear_ram_state(mgmt, rraw_pid_ch->raw_pid_chan_id);

        /* destroy all the raw pid channel */
        ret = dmx_mgmt_destroy_raw_pid_ch((struct dmx_r_base *)rraw_pid_ch);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_mgmt_destroy_raw_pid_ch failed!\n");
            osal_mutex_unlock(&rraw_pid_ch->lock);
            goto out;
        }
    }

    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
    return ret;
}

hi_s32 dmx_mgmt_create_pid_ch(struct dmx_r_band *rband, const hi_u32 pid, struct dmx_r_pid_ch **rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    DMX_NULL_POINTER_RETURN(rband);
    DMX_NULL_POINTER_RETURN(rpid_ch);

    if (pid > DMX_MAX_PID) {
        HI_ERR_DEMUX("invalid pid[%#x].\n", pid);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_pid_ch(mgmt, rband, pid, rpid_ch);
    if (ret == HI_SUCCESS) {
        /* get the band obj to avoid band been destroyed unexpectly */
        dmx_r_get_raw((struct dmx_r_base *)rband);
    }

out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_pid_ch(struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_r_pid_ch *rpid_ch = (struct dmx_r_pid_ch *)obj;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    DMX_NULL_POINTER_RETURN(rpid_ch);

    /* put the band obj */
    DMX_NULL_POINTER_RETURN(rpid_ch->rband);
    dmx_r_put((struct dmx_r_base *)rpid_ch->rband);

    ret = mgmt->ops->destroy_pid_ch(mgmt, rpid_ch);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy_pid_ch failed!\n");
    }

    return ret;
}

struct dmx_r_raw_pid_ch *_get_raw_pid_obj_for_play(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_play_fct *rplayfct)
{
    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;
    struct dmx_r_raw_pid_ch *tmp_rraw_pid_ch = HI_NULL;
    DMX_NULL_POINTER_RETURN_NULL(rpid_ch);
    DMX_NULL_POINTER_RETURN_NULL(rplayfct);

    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    list_for_each_entry_safe(rraw_pid_ch, tmp_rraw_pid_ch, &rpid_ch->raw_pid_ch_head, node1) {
        /* rraw_pid_ch->base.id == 0 means master rawpid, it's no need to open pid copy function */
        osal_mutex_lock(&rraw_pid_ch->lock);
        if (rplayfct == rraw_pid_ch->rplay_fct) {
            osal_mutex_unlock(&rraw_pid_ch->lock);
            osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);

            return rraw_pid_ch;
        }
        osal_mutex_unlock(&rraw_pid_ch->lock);
    }
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);

    return HI_NULL;
}

struct dmx_r_raw_pid_ch *_get_raw_pid_obj_for_rec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_rec_fct *rrecfct)
{
    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;
    struct dmx_r_raw_pid_ch *tmp_rraw_pid_ch = HI_NULL;
    DMX_NULL_POINTER_RETURN_NULL(rpid_ch);
    DMX_NULL_POINTER_RETURN_NULL(rrecfct);

    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    list_for_each_entry_safe(rraw_pid_ch, tmp_rraw_pid_ch, &rpid_ch->raw_pid_ch_head, node1) {
        osal_mutex_lock(&rraw_pid_ch->lock);
        if (rrecfct == rraw_pid_ch->rrec_fct) {
            osal_mutex_unlock(&rraw_pid_ch->lock);
            osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
            return rraw_pid_ch;
        }
        osal_mutex_unlock(&rraw_pid_ch->lock);
    }
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);

    return HI_NULL;
}

/*************************dmx_mgmt_pid_ch*************************************************/
hi_s32 dmx_pid_ch_create(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;
    struct dmx_r_band  *rband = HI_NULL;

    DMX_PID_TRACE_ENTER();
    /* get the band obj */
    ret = DMX_R_BAND_GET(band_handle, rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_mgmt_create_pid_ch(rband, pid, &rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* get the pid_ch obj */
    dmx_r_get_raw((struct dmx_r_base *)rpid_ch);

    ret = rpid_ch->ops->open(rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rpid_ch, &slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    /* associate an slot to the session. */
    slot->release = dmx_pid_ch_destroy;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out4;
    }

    *handle = slot->handle;

    rpid_ch->pid_ch_handle = slot->handle;

    /* release the pid_ch obj */
    dmx_r_put((struct dmx_r_base *)rpid_ch);

    /* release the band obj */
    dmx_r_put((struct dmx_r_base *)rband);

    DMX_PID_TRACE_EXIT();
    return HI_SUCCESS;

out4:
    dmx_slot_destroy(slot);
out3:
    rpid_ch->ops->close(rpid_ch);
out2:
    dmx_r_put((struct dmx_r_base *)rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);
out1:
    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_pid_ch_ref_inc(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;
    struct dmx_r_band  *rband = HI_NULL;
    struct list_head *node = HI_NULL;
    struct dmx_mgmt *rmgmt = HI_NULL;

    DMX_PID_TRACE_ENTER();

    rmgmt = get_dmx_mgmt();
    DMX_NULL_POINTER_RETURN(rmgmt);

    /* get the Band obj */
    ret = DMX_R_BAND_GET(band_handle, rband);
    if (HI_SUCCESS != ret) {
        goto out0;
    }

    osal_mutex_lock(&rmgmt->pid_channel_list_lock);
    list_for_each(node, &rmgmt->pid_channel_head) {
        struct dmx_r_pid_ch *tmp_pid_ch = list_entry(node, struct dmx_r_pid_ch, node0);
        /* compare the object */
        if (rband == tmp_pid_ch->rband && pid == tmp_pid_ch->pid && tmp_pid_ch->pid_ch_handle != HI_INVALID_HANDLE) {
            *handle = tmp_pid_ch->pid_ch_handle;

            if (osal_atomic_inc_return(&tmp_pid_ch->pid_ch_handle_ref_cnt) != 0) {
                ret = HI_SUCCESS;
            } else {
                HI_ERR_DEMUX("osal_atomic_inc_return failed!\n");
                ret = HI_FAILURE;
            }
            osal_mutex_unlock(&rmgmt->pid_channel_list_lock);
            goto out1;
        }
    }
    osal_mutex_unlock(&rmgmt->pid_channel_list_lock);

    ret = dmx_mgmt_create_pid_ch(rband, pid, &rpid_ch);
    if (HI_SUCCESS != ret) {
        goto out1;
    }

    /* get the PidCh obj */
    dmx_r_get_raw((struct dmx_r_base *)rpid_ch);

    ret = rpid_ch->ops->open(rpid_ch);
    if (HI_SUCCESS != ret) {
        goto out2;
    }

    /* Associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rpid_ch, &slot);
    if (HI_SUCCESS != ret) {
        goto out3;
    }

    /* Associate an slot to the session. */
    slot->release = dmx_pid_ch_destroy;
    ret = dmx_session_add_slot(session, slot);
    if (HI_SUCCESS != ret) {
        goto out4;
    }

    *handle = slot->handle;

    rpid_ch->pid_ch_handle = slot->handle;

    /* release the PidCh obj */
    dmx_r_put((struct dmx_r_base *)rpid_ch);

    /* release the Band obj */
    dmx_r_put((struct dmx_r_base *)rband);

    DMX_PID_TRACE_EXIT();

    return HI_SUCCESS;

out4:
    dmx_slot_destroy(slot);
out3:
    rpid_ch->ops->close(rpid_ch);
out2:
    dmx_r_put((struct dmx_r_base *)rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);
out1:
    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_pid_ch_get_handle(hi_handle band_handle, hi_u32 pid, hi_handle *handle)
{
    hi_s32 ret;
    struct dmx_r_band  *rband = HI_NULL;
    struct list_head *node = NULL;

    DMX_PID_TRACE_ENTER();
    /* get the band obj */
    ret = DMX_R_BAND_GET(band_handle, rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&rband->base.mgmt->pid_channel_list_lock);
    list_for_each(node, &rband->base.mgmt->pid_channel_head) {
        struct dmx_r_pid_ch *rpid_ch = list_entry(node, struct dmx_r_pid_ch, node0);

        /* if (band_handle == rpid_ch->rband->band_handle && pid == rpid_ch->pid) */
        /* compare the object */
        if (rband == rpid_ch->rband && pid == rpid_ch->pid) {
            *handle = rpid_ch->pid_ch_handle;
            ret = HI_SUCCESS;
            goto out1;
        }
    }

    HI_WARN_DEMUX("not find the pid_ch by band_handle[%#x],pid[%#x]\n", band_handle, pid);
    ret = HI_ERR_DMX_UNMATCH_CHAN;

out1:
    DMX_PID_TRACE_EXIT();
    osal_mutex_unlock(&rband->base.mgmt->pid_channel_list_lock);
    dmx_r_put((struct dmx_r_base *) rband);
out0:
    return ret;
}

hi_s32 dmx_pid_ch_get_status(hi_handle handle, dmx_pidch_status *status)
{
    hi_s32 ret;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;

    DMX_PID_TRACE_ENTER();
    ret = DMX_R_PID_CH_GET(handle, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rpid_ch->ops->get_status(rpid_ch, status);

    dmx_r_put((struct dmx_r_base *)rpid_ch);
    DMX_PID_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_pid_ch_lock_out(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;

    DMX_PID_TRACE_ENTER();
    ret = DMX_R_PID_CH_GET(handle, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rpid_ch->ops->lock_out(rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);

    DMX_PID_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_pid_ch_un_lock_out(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;

    DMX_PID_TRACE_ENTER();
    ret = DMX_R_PID_CH_GET(handle, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rpid_ch->ops->un_lock_out(rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);

    DMX_PID_TRACE_EXIT();
out:
    return ret;
}

hi_s32 dmx_pid_ch_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;

    DMX_PID_TRACE_ENTER();
    ret = DMX_R_PID_CH_GET(handle, rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out0;
    }
    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rpid_ch->ops->close(rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);

    DMX_PID_TRACE_EXIT();
out1:
    dmx_r_put((struct dmx_r_base *)rpid_ch);
out0:
    return ret;
}

hi_s32 dmx_pid_ch_ref_dec(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;

    DMX_PID_TRACE_ENTER();
    ret = DMX_R_PID_CH_GET(handle, rpid_ch);
    if (HI_SUCCESS != ret) {
        goto out0;
    }

    /* if pid_ch_handle_ref_count is not zero, then don't release the pidCh handle really */
    if (osal_atomic_dec_return(&rpid_ch->pid_ch_handle_ref_cnt) != 0) {
        ret = HI_SUCCESS;
        goto out1;
    }

    ret = dmx_slot_find(handle, &slot);
    if (HI_SUCCESS != ret) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rpid_ch->ops->close(rpid_ch);

    dmx_r_put((struct dmx_r_base *)rpid_ch);

    DMX_PID_TRACE_EXIT();
out1:
    dmx_r_put((struct dmx_r_base *)rpid_ch);
out0:
    return ret;
}

hi_s32 dmx_pid_ch_get_free_cnt(hi_u32 *free_cnt)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (free_cnt == HI_NULL) {
        HI_ERR_DEMUX("Null poiner.\n");
        return HI_FAILURE;
    }

    return mgmt->ops->get_free_ch_cnt(mgmt, free_cnt);
}

/*************************dmx_r_pid_channel_xxx_impl*************************************************/
static inline hi_bool pid_ch_staled(struct dmx_r_pid_ch *rpid_ch)
{
    if (unlikely(HI_TRUE == rpid_ch->staled)) {
        HI_ERR_DEMUX("pid channel is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _dmx_r_pid_ch_open_impl(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;

    /* create the mast raw pid channel */
    ret = create_raw_pid_ch(rpid_ch, DMX_RAWPIDCH_MASTER_TYPE, &rraw_pid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    rpid_ch->master_raw_pid_ch_id = rraw_pid_ch->raw_pid_chan_id;

    /* make sure the right object */
    WARN_ON(rraw_pid_ch->base.id != 0x0);
    WARN_ON(rraw_pid_ch->raw_pid_ch_type != DMX_RAWPIDCH_MASTER_TYPE);

    /* configure the pid channel */
    dmx_hal_pid_ch_config(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id, rpid_ch->rband->base.id, rpid_ch->pid, 0);

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_r_pid_ch_open_impl(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret;

    osal_mutex_lock(&rpid_ch->lock);

    WARN_ON(rpid_ch->staled == HI_TRUE);

    ret = _dmx_r_pid_ch_open_impl(rpid_ch);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* set the pid_ch_handle_ref_cnt as 1 in the first time */
    osal_atomic_set(&rpid_ch->pid_ch_handle_ref_cnt, 1);

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rpid_ch->lock);

    return ret;
}

static hi_s32 _dmx_r_pid_ch_get_status_impl(struct dmx_r_pid_ch *rpid_ch, dmx_pidch_status *status)
{
    hi_s32 ret = HI_FAILURE;

    if (pid_ch_staled(rpid_ch)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    if (unlikely(rpid_ch->rdsc_fct)) { /* not attach dscfct yet */
        status->dsc_handle = rpid_ch->rdsc_fct->dsc_fct_handle;
    } else {
        status->dsc_handle = HI_INVALID_HANDLE;
    }

    DMX_NULL_POINTER_GOTO(rpid_ch->rband, out);
    status->band_handle   = rpid_ch->rband->band_handle;
    status->pid           = rpid_ch->pid;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_pid_ch_get_status_impl(struct dmx_r_pid_ch *rpid_ch, dmx_pidch_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rpid_ch->lock);

    ret = _dmx_r_pid_ch_get_status_impl(rpid_ch, status);

    osal_mutex_unlock(&rpid_ch->lock);

    return ret;
}

static hi_s32 _raw_pid_ch_attach_pid_copy(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
        struct dmx_r_raw_pid_ch **rnew_raw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;

    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;
    struct dmx_r_raw_pid_ch *tmp_rraw_pid_ch = HI_NULL;

    /* tmp raw pid channel state */
    rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_MAX_STATE;

    /* if raw_pid_ch_head empty init the pidch again */
    if (unlikely(list_empty(&rpid_ch->raw_pid_ch_head))) {
        ret = _dmx_r_pid_ch_open_impl(rpid_ch);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("raw_pid_ch_head init again failed!\n");
            goto out;
        }
    }

    /* for pidcopy check */
    if (IS_PLAYFCT(obj)) {
        struct dmx_r_play_fct *rplay_fct = (struct dmx_r_play_fct *)obj;
        osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
        list_for_each_entry(rraw_pid_ch, &rpid_ch->raw_pid_ch_head, node1) {
            /* DMX_RAWPIDCH_MASTER_TYPE means master rawpid, it's no need to open pid copy function */
            osal_mutex_lock(&rraw_pid_ch->lock);
            if (osal_atomic_read(&rraw_pid_ch->ref_play_cnt) == 0x0 &&
                rraw_pid_ch->raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE) {
                HI_DBG_DEMUX("find the master play raw_pid_ch obj to be attached\n");
                osal_atomic_set(&rraw_pid_ch->ref_play_cnt, 1);
                rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_MASTER_STATE;
                osal_mutex_unlock(&rraw_pid_ch->lock);
                break;
            } else if (osal_atomic_read(&rraw_pid_ch->ref_play_cnt) == 0x0 &&
                rraw_pid_ch->raw_pid_ch_type != DMX_RAWPIDCH_MASTER_TYPE) {
                HI_DBG_DEMUX("find the exist slave play raw_pid_ch obj to be attached\n");
                osal_atomic_set(&rraw_pid_ch->ref_play_cnt, 1);
                rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_EXIST_SLAVE_STATE;
                osal_mutex_unlock(&rraw_pid_ch->lock);
                break;
            } else {
                HI_DBG_DEMUX("left empty\n");
            }
            osal_mutex_unlock(&rraw_pid_ch->lock);
        }
        osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);

        /* if not liveplay, open the pidcopy default, this occation it will be destroyed in
         * dmx_pid_ch_destroy:rpid->ops->close.
         */
        if (rpid_ch->tmp_raw_pid_ch_state == DMX_RAWPIDCH_MASTER_STATE && rplay_fct->live_play == HI_FALSE) {
            HI_DBG_DEMUX("unlive type need create the slave rawpidch, left the master rawpid chan empty\n");

            osal_mutex_lock(&rraw_pid_ch->lock);
            osal_atomic_set(&rraw_pid_ch->ref_play_cnt, 1);
            rraw_pid_ch->force_unlive = HI_TRUE;
            osal_mutex_unlock(&rraw_pid_ch->lock);
            rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_CREATE_SLAVE_STATE;
        }
    } else if (IS_RECFCT(obj)) {
        osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
        list_for_each_entry(rraw_pid_ch, &rpid_ch->raw_pid_ch_head, node1) {
            /* DMX_RAWPIDCH_MASTER_TYPE means master rawpid, it's no need to open pid copy function */
            osal_mutex_lock(&rraw_pid_ch->lock);
            if (osal_atomic_read(&rraw_pid_ch->ref_rec_cnt) == 0x0 &&
                rraw_pid_ch->raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE) {
                HI_DBG_DEMUX("find the master rec raw_pid_ch obj to be attached\n");
                osal_atomic_set(&rraw_pid_ch->ref_rec_cnt, 1);
                rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_MASTER_STATE;
                osal_mutex_unlock(&rraw_pid_ch->lock);
                break;
            } else if (osal_atomic_read(&rraw_pid_ch->ref_rec_cnt) == 0x0 &&
                rraw_pid_ch->raw_pid_ch_type != DMX_RAWPIDCH_MASTER_TYPE) {
                HI_DBG_DEMUX("find the exist slave rec raw_pid_ch obj to be attached\n");
                osal_atomic_set(&rraw_pid_ch->ref_rec_cnt, 1);
                rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_EXIST_SLAVE_STATE;
                osal_mutex_unlock(&rraw_pid_ch->lock);
                break;
            } else {
                HI_DBG_DEMUX("left empty\n");
            }
            osal_mutex_unlock(&rraw_pid_ch->lock);
        }
        osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
    }else {
        /* dscfct no need ram_pid_ch object */
        ret = HI_SUCCESS;
        goto out;
    }

    /* create the slave raw pid channel */
    if (rpid_ch->tmp_raw_pid_ch_state == DMX_RAWPIDCH_MAX_STATE ||
        rpid_ch->tmp_raw_pid_ch_state == DMX_RAWPIDCH_CREATE_SLAVE_STATE) {
        ret = create_raw_pid_ch(rpid_ch, DMX_RAWPIDCH_SLAVE_TYPE, &tmp_rraw_pid_ch);
        if (ret != HI_SUCCESS) {
            goto out;
        }
        HI_DBG_DEMUX("crate slave play raw pidch***\n");
        rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_CREATE_SLAVE_STATE;
    } else {
        /* use the master or exist raw_pidch */
        tmp_rraw_pid_ch = rraw_pid_ch;
    }

    DMX_NULL_POINTER_RETURN(tmp_rraw_pid_ch);

    /* enable the pidcopy feature */
    if (rpid_ch->tmp_raw_pid_ch_state == DMX_RAWPIDCH_EXIST_SLAVE_STATE ||
        rpid_ch->tmp_raw_pid_ch_state == DMX_RAWPIDCH_CREATE_SLAVE_STATE) {
        /* some hardwre configure */
        if (rpid_ch->pid_copy_en_status == HI_FALSE) {
            /* clear pid copy mmu cache */
            dmx_hal_pid_copy_clr_mmu_cache(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id);
            /* enable the pid copy configure */
            dmx_hal_pid_tab_ctl_en_set(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id, DMX_PID_CHN_PIDCOPY_FLAG);

            /* configure pid copy channel, and buffer id */
            dmx_hal_pid_tab_set_pcid(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id, rpid_ch->pid_copy_chan_id);

            ret = _dmx_r_pid_ch_create_pcch(rpid_ch);
            if (ret != HI_SUCCESS) {
                goto out;
            }

            /* set the pid copy bufid as pid copy chan id */
            dmx_hal_pid_set_pcbuf_id(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id, rpid_ch->pid_copy_chan_id);

            /* set the tsio  ap enable */
            if ((rpid_ch->rband->port >= DMX_TSIO_PORT_0) && (rpid_ch->rband->port < DMX_TSIO_PORT_MAX)) {
                dmx_hal_buf_pc_set_tsio_ap(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id,
                    HI_TRUE, rpid_ch->rband->port - DMX_TSIO_PORT_0);
            }

            rpid_ch->pid_copy_en_status = HI_TRUE;
        }

        /* configure the pid and band id and markid of slave raw pid channel */
        dmx_hal_pid_ch_config(rpid_ch->base.mgmt, tmp_rraw_pid_ch->raw_pid_chan_id, rpid_ch->rband->base.id,
                              rpid_ch->pid, tmp_rraw_pid_ch->base.id);

        /* set the valid bit of slave raw pid channel */
        dmx_hal_pid_en_pcmark_valid(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id, tmp_rraw_pid_ch->base.id);
    }

    *rnew_raw_pid_ch = tmp_rraw_pid_ch;

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 __raw_pid_ch_attach_ts(struct dmx_mgmt *mgmt, hi_u32 chan_id,
    hi_u32 buf_id, hi_u32 raw_pidch_id, hi_u32 master_raw_pidch_id)
{
    /* enable pid channel for whole ts */
    dmx_hal_pid_tab_ctl_en_set(mgmt, raw_pidch_id, DMX_PID_CHN_WHOLE_TS_FLAG);

    /* set the whole ts channel id register */
    dmx_hal_pid_tab_set_sub_play_chan_id(mgmt, raw_pidch_id, chan_id);

    /* set the whole ts tab register */
    dmx_hal_pid_set_whole_tstab(mgmt, chan_id, buf_id, HI_TRUE);

    /* attention: do this in the last step, enable the rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, raw_pidch_id);

    /* two step about master rawpidch */
    /* enable the master rawpid_ch as whole ts */
    dmx_hal_pid_tab_ctl_en_set(mgmt, master_raw_pidch_id, DMX_PID_CHN_WHOLE_TS_FLAG);

    /* attention: must do it in the last step, even repeatly. enable the master rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, master_raw_pidch_id);

    return HI_SUCCESS;
}

static hi_u32 _raw_pid_ch_attach_ts(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret;

    struct dmx_r_playfct_ts *rsub_play_fct = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    rsub_play_fct = (struct dmx_r_playfct_ts *)obj;
    rplay_fct = (struct dmx_r_play_fct *)obj;

    if (rplay_fct->secure_mode != DMX_SECURE_TEE) {
        ret = __raw_pid_ch_attach_ts(rpid_ch->base.mgmt, rsub_play_fct->play_ts.ts_chan_id,
                                     rplay_fct->rbuf->base.id, rraw_pid_ch->raw_pid_chan_id,
                                     rpid_ch->master_raw_pid_ch_id);
    } else {
        ret = dmx_tee_attach_play_chan(rsub_play_fct->play_ts.ts_chan_id, DMX_PLAY_TYPE_TS,
                                       rraw_pid_ch->raw_pid_chan_id, rpid_ch->master_raw_pid_ch_id);
    }

    return ret;
}

static hi_s32 __raw_pid_ch_attach_pes(struct dmx_mgmt *mgmt, hi_u32 chan_id,
    hi_u32 raw_pidch_id, hi_u32 master_raw_pidch_id, hi_u32 chan_buf_id)
{
    /* enable pid channel for pes section */
    dmx_hal_pid_tab_ctl_en_set(mgmt, raw_pidch_id, DMX_PID_CHN_PES_SEC_FLAG);

    /* set the pes section channel id register */
    dmx_hal_pid_tab_set_sub_play_chan_id(mgmt, raw_pidch_id, chan_id);

    /* set the data type as pes, enable the pes head len check */
    dmx_hal_pid_set_pes_sec_tab(mgmt, chan_id, DMX_PID_TYPE_PES, HI_FALSE, HI_TRUE);

    dmx_hal_flt_set_pes_default_attr(mgmt, chan_id, chan_buf_id);

    /* attention: do this in the last step, enable the rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, raw_pidch_id);

    /* two step about master rawpidch */
    /* enable the master rawpid_ch as pes_sec */
    dmx_hal_pid_tab_ctl_en_set(mgmt, master_raw_pidch_id, DMX_PID_CHN_PES_SEC_FLAG);

    /* attention: must do it in the last step, even repeatly. enable the master rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, master_raw_pidch_id);

    return HI_SUCCESS;
}

static hi_s32 _raw_pid_ch_attach_pes(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret;

    struct dmx_r_playfct_pes_sec *rsub_play_fct = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    rsub_play_fct = (struct dmx_r_playfct_pes_sec *)obj;
    rplay_fct = (struct dmx_r_play_fct *)obj;

    if (rplay_fct->secure_mode != DMX_SECURE_TEE) {
        ret = __raw_pid_ch_attach_pes(rpid_ch->base.mgmt,
                                      rsub_play_fct->play_pes_sec.pes_sec_chan_id,
                                      rraw_pid_ch->raw_pid_chan_id, rpid_ch->master_raw_pid_ch_id,
                                      rplay_fct->rbuf->base.id);
    } else {
        ret = dmx_tee_attach_play_chan(rsub_play_fct->play_pes_sec.pes_sec_chan_id,
                                       DMX_PLAY_TYPE_PES, rraw_pid_ch->raw_pid_chan_id, rpid_ch->master_raw_pid_ch_id);
    }

    return ret;
}

static hi_s32 __raw_pid_ch_attach_sec(struct dmx_mgmt *mgmt, hi_u32 chan_id,
    hi_u32 raw_pidch_id, hi_u32 master_raw_pidch_id, hi_u32 chan_buf_id)
{
    /* enable pid channel for pes section */
    dmx_hal_pid_tab_ctl_en_set(mgmt, raw_pidch_id, DMX_PID_CHN_PES_SEC_FLAG);

    /* set the pes section channel id register */
    dmx_hal_pid_tab_set_sub_play_chan_id(mgmt, raw_pidch_id, chan_id);

    /* set the data type as section, enable the pes head len check */
    dmx_hal_pid_set_pes_sec_tab(mgmt, chan_id, DMX_PID_TYPE_SECTION, HI_FALSE, HI_TRUE);

    dmx_hal_flt_set_sec_default_attr(mgmt, chan_id, chan_buf_id);

    /* attention: do this in the last step, enable the rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, raw_pidch_id);

    /* two step about master rawpidch */
    /* enable the master rawpid_ch as pes_sec */
    dmx_hal_pid_tab_ctl_en_set(mgmt, master_raw_pidch_id, DMX_PID_CHN_PES_SEC_FLAG);

    /* attention: must do it in the last step, even repeatly. enable the master rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, master_raw_pidch_id);

    return HI_SUCCESS;
}

static hi_s32 _raw_pid_ch_attach_sec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret;

    struct dmx_r_playfct_pes_sec *rsub_play_fct = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    rsub_play_fct = (struct dmx_r_playfct_pes_sec *)obj;
    rplay_fct = (struct dmx_r_play_fct *)obj;

    if (rplay_fct->secure_mode != DMX_SECURE_TEE) {
        ret = __raw_pid_ch_attach_sec(rpid_ch->base.mgmt,
                                      rsub_play_fct->play_pes_sec.pes_sec_chan_id,
                                      rraw_pid_ch->raw_pid_chan_id, rpid_ch->master_raw_pid_ch_id,
                                      rplay_fct->rbuf->base.id);
    } else {
        ret = dmx_tee_attach_play_chan(rsub_play_fct->play_pes_sec.pes_sec_chan_id,
                                       DMX_PLAY_TYPE_SEC, rraw_pid_ch->raw_pid_chan_id, rpid_ch->master_raw_pid_ch_id);
    }

    return ret;
}

static hi_s32 __raw_pid_ch_attach_av_pes(struct dmx_mgmt *mgmt, hi_u32 chan_id,
    hi_u32 buf_id, hi_u32 raw_pidch_id, hi_u32 master_raw_pidch_id)
{
    /* enable pid channel for avpes */
    dmx_hal_pid_tab_ctl_en_set(mgmt, raw_pidch_id, DMX_PID_CHN_AVPES_FLAG);

    /* set the avpes channel id register */
    dmx_hal_pid_tab_set_sub_play_chan_id(mgmt, raw_pidch_id, chan_id);

    /* set the avpes tab register, disable drop cc not continue until pusi, disable the pes head len check */
    dmx_hal_pid_set_av_pes_tab(mgmt, chan_id, buf_id, DMX_PID_TYPE_REC_SCD, HI_FALSE, HI_FALSE);
    /* set the rec scd enable and work on av pes mode */
    dmx_hal_scd_set_av_pes_cfg(mgmt, chan_id, 1, 1, 0x200); /* 0x200 means rec_bufid bit[25] set to 1, enable the mq */

    /* attention: do this in the last step, enable the rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, raw_pidch_id);

    /* two step about master rawpidch */
    /* enable the master rawpid_ch as avpes */
    dmx_hal_pid_tab_ctl_en_set(mgmt, master_raw_pidch_id, DMX_PID_CHN_AVPES_FLAG);

    /* attention: must do it in the last step, even repeatly. enable the master rawpid channle */
    dmx_hal_pid_tab_flt_en(mgmt, master_raw_pidch_id);

    return HI_SUCCESS;
}

static hi_s32 _raw_pid_ch_attach_av_pes(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret;

    struct dmx_r_playfct_avpes *rsub_play_fct = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;

    rsub_play_fct = (struct dmx_r_playfct_avpes *)obj;
    rplay_fct = (struct dmx_r_play_fct *)obj;

    if (rplay_fct->secure_mode != DMX_SECURE_TEE) {
        ret = __raw_pid_ch_attach_av_pes(rpid_ch->base.mgmt,
                                         rsub_play_fct->play_avpes.av_pes_chan_id,
                                         rplay_fct->rbuf->base.id, rraw_pid_ch->raw_pid_chan_id,
                                         rpid_ch->master_raw_pid_ch_id);
    } else {
        ret = dmx_tee_attach_play_chan(rsub_play_fct->play_avpes.av_pes_chan_id,
                                       rplay_fct->play_fct_type, rraw_pid_ch->raw_pid_chan_id,
                                       rpid_ch->master_raw_pid_ch_id);
    }

    return ret;
}

static hi_s32 __raw_pidch_attach_rec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_rec_fct *rrec_fct,
                                      struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    /* enable pid channel for record */
    dmx_hal_pid_tab_ctl_en_set(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, DMX_PID_CHN_REC_FLAG);
    dmx_hal_pid_set_rec_tab(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, rrec_fct->base.id);

    if (rrec_fct->descramed == HI_TRUE) {
        dmx_hal_pid_set_rec_dsc_mode(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, HI_TRUE);
    } else {
        dmx_hal_pid_set_rec_dsc_mode(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, HI_FALSE);
    }

    /* enable the ts index */
    if (rrec_fct->index_type != DMX_REC_INDEX_TYPE_NONE && rpid_ch->pid == rrec_fct->index_src_pid) {
        dmx_hal_pid_tab_ctl_en_set(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, DMX_PID_CHN_TS_SCD_FLAG);
        dmx_hal_pid_set_scd_tab(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, rrec_fct->scd_id, 0);

        /* get the scd_buf obj */
        dmx_r_get_raw((struct dmx_r_base *)rrec_fct->scd_rbuf);
        rrec_fct->scd_rbuf->ops->attach(rrec_fct->scd_rbuf, rpid_ch->rband->port);
        dmx_r_put((struct dmx_r_base *)rrec_fct->scd_rbuf);
    }

    /* attention: do this in the last step, enable the rawpid channle */
    dmx_hal_pid_tab_flt_en(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id);

    /* two step about master rawpidch */
    /* enable the master rawpid_ch as rec ts */
    dmx_hal_pid_tab_ctl_en_set(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id, DMX_PID_CHN_REC_FLAG);

    /* attention: must do it in the last step, even repeatly. enable the master rawpid channle */
    dmx_hal_pid_tab_flt_en(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id);

    return HI_SUCCESS;
}

static hi_s32 _raw_pid_ch_attach_rec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret;

    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    rrec_fct = (struct dmx_r_rec_fct *)obj;

    if (rrec_fct->secure_mode != DMX_SECURE_TEE) {
        ret = __raw_pidch_attach_rec(rpid_ch,  rrec_fct, rraw_pid_ch);
    } else {
        dmx_tee_rec_attach_info rec_attach_info = {0};
        rec_attach_info.is_descram = HI_FALSE;
        rec_attach_info.is_video_index = HI_FALSE;
        rec_attach_info.chan_id = rrec_fct->base.id;
        rec_attach_info.raw_pidch_id = rraw_pid_ch->raw_pid_chan_id;
        rec_attach_info.master_raw_pidch_id = rpid_ch->master_raw_pid_ch_id;

        if (rrec_fct->descramed == HI_TRUE) {
            rec_attach_info.is_descram = HI_TRUE;
        }

        /* enable the ts index */
        if (rrec_fct->index_type != DMX_REC_INDEX_TYPE_VIDEO && rpid_ch->pid == rrec_fct->index_src_pid) {
            rec_attach_info.is_video_index = HI_TRUE;
            rec_attach_info.index_src_pid = rpid_ch->pid;
            /* get the scd_buf obj */
            dmx_r_get_raw((struct dmx_r_base *)rrec_fct->scd_rbuf);
            rrec_fct->scd_rbuf->ops->attach(rrec_fct->scd_rbuf, rpid_ch->rband->port);
            dmx_r_put((struct dmx_r_base *)rrec_fct->scd_rbuf);
        }
        ret = dmx_tee_attach_rec_chan(&rec_attach_info);
    }

    return ret;
}

static hi_s32 _raw_pid_ch_attach_dsc(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj)
{
    struct list_head *node = HI_NULL;
    struct dmx_r_dsc_fct    *rdsc_fct = HI_NULL;

    HI_DBG_DEMUX("***begin descrambler here!\n");
    rdsc_fct = (struct dmx_r_dsc_fct *)obj;

    if (rpid_ch->rdsc_fct != HI_NULL && rpid_ch->rdsc_fct->dsc_fct_handle != HI_INVALID_HANDLE) {
        HI_ERR_DEMUX("pid channel has already attach descrambler! pid_handle=%#x\n", rpid_ch->pid_ch_handle);
        return HI_ERR_DMX_ATTACHED_KEY;
    }
    /*
     * if keyslot_attached == HI_FALSE,means keyslot created by keyslot unf fucntion,
     * so we should let dsc attach keyslot at first.
     */
    if (rdsc_fct->keyslot_attached == HI_FALSE) {
        HI_WARN_DEMUX("Descrambler will attach pid channel at a later time!\n");
        return HI_ERR_DMX_NOATTACH_KEY;
    }

    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    list_for_each(node, &rpid_ch->raw_pid_ch_head) {
        struct dmx_r_raw_pid_ch *rraw_pid_ch = list_entry(node, struct dmx_r_raw_pid_ch, node1);
        /* enable the descrambler */
        dmx_hal_pid_cw_en_set(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, HI_TRUE);
        dmx_hal_pid_set_cw_id(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, rdsc_fct->base.id);
        /* enable ts descrambler support */
        dmx_hal_pid_set_dsc_type(rpid_ch->base.mgmt, rdsc_fct->base.id, HI_TRUE, HI_FALSE);
    }
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_pid_ch_attach_raw_pid_ch_impl(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_r_raw_pid_ch  *rraw_pid_ch = HI_NULL;

    DMX_NULL_POINTER_RETURN(rpid_ch);
    DMX_NULL_POINTER_RETURN(obj);

    ret = _raw_pid_ch_attach_pid_copy(rpid_ch, obj, &rraw_pid_ch);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("_raw_pid_ch_attach_pid_copy failed, ret[0x%x]!\n", ret);
        return ret;
    }

    if (IS_PLAYFCT(obj)) {
        DMX_NULL_POINTER_RETURN(rraw_pid_ch);
        if (IS_PLAYFCT_TS(obj)) {
            ret = _raw_pid_ch_attach_ts(rpid_ch, obj, rraw_pid_ch);
        } else if (IS_PLAYFCT_PES(obj)) {
            ret = _raw_pid_ch_attach_pes(rpid_ch, obj, rraw_pid_ch);
        } else if (IS_PLAYFCT_SEC(obj)) {
            ret = _raw_pid_ch_attach_sec(rpid_ch, obj, rraw_pid_ch);
        } else if (IS_PLAYFCT_AVPES(obj)) {
            ret = _raw_pid_ch_attach_av_pes(rpid_ch, obj, rraw_pid_ch);
        } else {
            HI_ERR_DEMUX("invalid fct object type!\n");
            ret = HI_ERR_DMX_INVALID_PARA;
            return ret;
        }

        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("play attach raw pid failed, ret[0x%x]!\n", ret);
            return ret;
        }

        osal_mutex_lock(&rraw_pid_ch->lock);
        osal_atomic_set(&rraw_pid_ch->ref_play_cnt, 1);
        osal_mutex_unlock(&rraw_pid_ch->lock);
        /* save the playfct object to the rawpid channel object for detach */
        rraw_pid_ch->rplay_fct = (struct dmx_r_play_fct *)obj;
    } else if (IS_RECFCT(obj)) {
        DMX_NULL_POINTER_RETURN(rraw_pid_ch);
        ret = _raw_pid_ch_attach_rec(rpid_ch, obj, rraw_pid_ch);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("rec attach raw pid failed, ret[0x%x]!\n", ret);
            return ret;
        }

        osal_mutex_lock(&rraw_pid_ch->lock);
        osal_atomic_set(&rraw_pid_ch->ref_rec_cnt, 1);
        osal_mutex_unlock(&rraw_pid_ch->lock);
        /* save the recfct object to the rawpid channel object for detach */
        rraw_pid_ch->rrec_fct = (struct dmx_r_rec_fct *)obj;
    } else if (IS_DSCFCT(obj)) {
        ret = _raw_pid_ch_attach_dsc(rpid_ch, obj);
        if (ret != HI_SUCCESS && ret != HI_ERR_DMX_NOATTACH_KEY) {
            HI_ERR_DEMUX("_raw_pid_ch_attach_dsc failed!\n");
            return ret;
        }
        rpid_ch->rdsc_fct = (struct dmx_r_dsc_fct *)obj;
    } else {
        HI_ERR_DEMUX("invalid fct object type!\n");
        ret = HI_ERR_DMX_INVALID_PARA;
        return ret;
    }

    /* increase the pidch ref, sync with the pid object ref */
    if (osal_atomic_inc_return(&rpid_ch->pid_ch_handle_ref_cnt) == 0) {
        HI_ERR_DEMUX("inc pid_ch_handle_ref_cnt err.\n");
    }

    return HI_SUCCESS;
}

/* private interface, no need to check staled */
static hi_s32 dmx_r_pid_ch_attach_raw_pid_ch_impl(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rpid_ch->lock);

    ret = _dmx_r_pid_ch_attach_raw_pid_ch_impl(rpid_ch, obj);

    osal_mutex_unlock(&rpid_ch->lock);

    return ret;
}

static hi_void disable_pc_tsio_ap(struct dmx_r_pid_ch *rpid_ch)
{
    if ((rpid_ch->rband->port >= DMX_TSIO_PORT_0) && (rpid_ch->rband->port < DMX_TSIO_PORT_MAX)) {
        dmx_hal_buf_pc_set_tsio_ap(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id,
            HI_FALSE, rpid_ch->rband->port - DMX_TSIO_PORT_0);
    }

    return;
}

static hi_s32 _raw_pid_ch_detach_pid_copy(struct dmx_r_pid_ch *rpid_ch, dmx_secure_mode secure_mode,
    struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;

    /* tmp raw pid channel state */
    rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_MAX_STATE;

    /* check whether slave rawpid_ch destroy */
    osal_mutex_lock(&rraw_pid_ch->lock);
    if (osal_atomic_read(&rraw_pid_ch->ref_play_cnt) == 0x0 &&
            osal_atomic_read(&rraw_pid_ch->ref_rec_cnt) == 0x0 &&
            rraw_pid_ch->raw_pid_ch_type == DMX_RAWPIDCH_SLAVE_TYPE) {
        HI_DBG_DEMUX("slave raw_pid_ch obj(0x%x) is empty, can be destroy\n", rraw_pid_ch);
        rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_DESTROY_SLAVE_STATE;
    }
    osal_mutex_unlock(&rraw_pid_ch->lock);

    /* destroy the slave raw pid channel */
    if (rpid_ch->tmp_raw_pid_ch_state == DMX_RAWPIDCH_DESTROY_SLAVE_STATE) {
        /* deconfigure slave raw pid channel */
        dmx_hal_pid_ch_de_config(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id);
        /* clear the valid bit of  slave raw pid channel */
        dmx_hal_pid_dis_pcmark_valid(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id, rraw_pid_ch->base.id);
        /* clear the slave pid channel */
        dmx_hal_pid_clear_ram_state(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id);

        /* try to destroy the slave raw pidch */
        ret = destroy_raw_pid_ch(rpid_ch, DMX_RAWPIDCH_SLAVE_TYPE, rraw_pid_ch);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        /* after destroy the slave raw pid channel, get the master raw pid channel and check
         * it whether is the last master raw pid channel
         */
        rraw_pid_ch = list_first_entry(&rpid_ch->raw_pid_ch_head, struct dmx_r_raw_pid_ch, node1);
        DMX_NULL_POINTER_GOTO(rraw_pid_ch, out);
    }

    /* clear force_unlive master rawpid_ch if master rawpidchannel is laster */
    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    osal_mutex_lock(&rraw_pid_ch->lock);
    if (unlikely(list_is_last(&rraw_pid_ch->node1, &rpid_ch->raw_pid_ch_head)) &&
                rraw_pid_ch->raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE &&
                osal_atomic_read(&rraw_pid_ch->ref_play_cnt) == 0x1 &&
                rraw_pid_ch->force_unlive == HI_TRUE) {
        HI_DBG_DEMUX("find the empty force_live master rawpidch, need to be destroyed!\n");
        osal_atomic_set(&rraw_pid_ch->ref_play_cnt, 0x0);
        rraw_pid_ch->force_unlive = HI_FALSE;
    }
    osal_mutex_unlock(&rraw_pid_ch->lock);
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);

    /* check whether master rawpid_ch destroy */
    osal_mutex_lock(&rraw_pid_ch->lock);
    if (unlikely(list_is_last(&rraw_pid_ch->node1, &rpid_ch->raw_pid_ch_head)) &&
                osal_atomic_read(&rraw_pid_ch->ref_play_cnt) == 0x0 &&
                osal_atomic_read(&rraw_pid_ch->ref_rec_cnt) == 0x0 &&
                rraw_pid_ch->raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE) {
        HI_DBG_DEMUX("master raw_pid_ch obj(0x%x) is empty, can be destroy\n", rraw_pid_ch);
        rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_DESTROY_MASTER_STATE;
    }
    osal_mutex_unlock(&rraw_pid_ch->lock);

    /* destroy the master raw pid channel at last */
    if (rpid_ch->tmp_raw_pid_ch_state == DMX_RAWPIDCH_DESTROY_MASTER_STATE) {
        /* detach the tee master rawpidch */
        if (secure_mode == DMX_SECURE_TEE) {
            dmx_tee_detach_raw_pidch(rpid_ch->master_raw_pid_ch_id);
        }

        /* check wherther pidcopy should disable or not */
        if (rpid_ch->pid_copy_en_status == HI_TRUE) {
            rraw_pid_ch = list_first_entry(&rpid_ch->raw_pid_ch_head, struct dmx_r_raw_pid_ch, node1);
            DMX_NULL_POINTER_GOTO(rraw_pid_ch, out);

            osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
            if (unlikely(list_is_last(&rraw_pid_ch->node1, &rpid_ch->raw_pid_ch_head) &&
                rraw_pid_ch->raw_pid_ch_type == DMX_RAWPIDCH_MASTER_TYPE)) {
                /* diable the pid copy configure */
                dmx_hal_pid_tab_ctl_dis_set(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id,
                                            DMX_PID_CHN_PIDCOPY_FLAG);
                /* set the pid copy id of master pid channel as 0 */
                dmx_hal_pid_tab_set_pcid(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id, 0);
                /* set the copy bufid of slave raw pid channel as 0 */
                dmx_hal_pid_set_pcbuf_id(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id, 0);

                /* clear the pid copy channel */
                dmx_hal_pid_copy_clear_chan(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id);

                /* disable tsio ap */
                disable_pc_tsio_ap(rpid_ch);

                ret = _dmx_r_pid_ch_destroy_pcch(rpid_ch);
                if (ret != HI_SUCCESS) {
                    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
                    goto out;
                }
                rpid_ch->pid_copy_en_status = HI_FALSE;
            }
            osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);
        }

        /* disable the rawpid channle */
        dmx_hal_pid_tab_flt_dis(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id);
        /* disable pid channel of  all data type */
        dmx_hal_pid_tab_ctl_dis_set(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, DMX_PID_CHN_DATA_MASK);
        /* deconfigure master raw pid channel and destroy it */
        dmx_hal_pid_ch_de_config(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id);
        /* clear the master pid channel */
        dmx_hal_pid_clear_ram_state(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id);
        /* try to destroy the slave raw pidch */
        ret = destroy_raw_pid_ch(rpid_ch, DMX_RAWPIDCH_MASTER_TYPE, rraw_pid_ch);
        if (ret != HI_SUCCESS) {
            goto out;
        }
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_void __raw_pid_ch_detach_ts(struct dmx_mgmt *mgmt, hi_u32 chan_id, hi_u32 raw_pidch_id)
{
    /* disable the rawpid channle */
    dmx_hal_pid_tab_flt_dis(mgmt, raw_pidch_id);

    /* disable pid channel of whole ts */
    dmx_hal_pid_tab_ctl_dis_set(mgmt, raw_pidch_id, DMX_PID_CHN_WHOLE_TS_FLAG);
    /* clear whole ts buf id register */
    dmx_hal_pid_set_whole_tstab(mgmt, chan_id, 0, HI_FALSE);

    return;
}

static hi_void _raw_pid_ch_detach_ts(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rawpid_ch)
{
    struct dmx_r_playfct_ts *rsub_play_fct = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    rsub_play_fct = (struct dmx_r_playfct_ts *)obj;
    rplay_fct = (struct dmx_r_play_fct *)obj;

    if (rplay_fct->secure_mode != DMX_SECURE_TEE) {
        __raw_pid_ch_detach_ts(rpid_ch->base.mgmt, rsub_play_fct->play_ts.ts_chan_id,
                               rawpid_ch->raw_pid_chan_id);
    } else {
        dmx_tee_detach_play_chan(rsub_play_fct->play_ts.ts_chan_id, DMX_PLAY_TYPE_TS,
                                 rawpid_ch->raw_pid_chan_id);
    }

    return;
}

static hi_void __raw_pid_ch_detach_pes_sec(struct dmx_mgmt *mgmt, hi_u32 chan_id, hi_u32 raw_pidch_id)
{
    /* disable the rawpid channle */
    dmx_hal_pid_tab_flt_dis(mgmt, raw_pidch_id);

    /* disable pid channel of  pes section */
    dmx_hal_pid_tab_ctl_dis_set(mgmt, raw_pidch_id, DMX_PID_CHN_PES_SEC_FLAG);

    return;
}

static hi_void _raw_pid_ch_detach_pes_sec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rawpid_ch)
{
    struct dmx_r_playfct_pes_sec *rsub_play_fct = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    rsub_play_fct = (struct dmx_r_playfct_pes_sec *)obj;
    rplay_fct = (struct dmx_r_play_fct *)obj;

    /* master raw pidch disable just destroy it in _raw_pid_ch_detach_pid_copy */
    if ((rawpid_ch->raw_pid_ch_type != DMX_RAWPIDCH_MASTER_TYPE) && (rplay_fct->secure_mode != DMX_SECURE_TEE)) {
        __raw_pid_ch_detach_pes_sec(rpid_ch->base.mgmt,
                                    rsub_play_fct->play_pes_sec.pes_sec_chan_id,
                                    rawpid_ch->raw_pid_chan_id);
    } else if (rplay_fct->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_detach_play_chan(rsub_play_fct->play_pes_sec.pes_sec_chan_id,
                                 rplay_fct->play_fct_type, rawpid_ch->raw_pid_chan_id);
    }

    return;
}

static hi_void __raw_pid_ch_detach_av_pes(struct dmx_mgmt *mgmt, struct dmx_r_raw_pid_ch *rawpid_ch,
    hi_u32 chan_id, hi_u32 raw_pidch_id)
{
    /* play alone needs to disable the pid_tab, but play&record don't need to */
    osal_mutex_lock(&rawpid_ch->lock);
    if ((osal_atomic_read(&rawpid_ch->ref_play_cnt) == 0x1) && osal_atomic_read(&rawpid_ch->ref_rec_cnt) == 0x0) {
        /* disable the rawpid channle */
        dmx_hal_pid_tab_flt_dis(mgmt, rawpid_ch->raw_pid_chan_id);
    }
    osal_mutex_unlock(&rawpid_ch->lock);

    /* disable pid channel of avpes */
    dmx_hal_pid_tab_ctl_dis_set(mgmt, raw_pidch_id, DMX_PID_CHN_AVPES_FLAG);

    return;
}

static hi_void _raw_pid_ch_detach_av_pes(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
                                                            struct dmx_r_raw_pid_ch *rawpid_ch)
{
    struct dmx_r_playfct_avpes *rsub_play_fct = HI_NULL;
    struct dmx_r_play_fct *rplay_fct = HI_NULL;
    rsub_play_fct = (struct dmx_r_playfct_avpes *)obj;
    rplay_fct = (struct dmx_r_play_fct *)obj;

    /* master raw pidch disable just destroy it in _raw_pid_ch_detach_pid_copy */
    if ((rawpid_ch->raw_pid_ch_type != DMX_RAWPIDCH_MASTER_TYPE) && (rplay_fct->secure_mode != DMX_SECURE_TEE)) {
            __raw_pid_ch_detach_av_pes(rpid_ch->base.mgmt, rawpid_ch,
                                       rsub_play_fct->play_avpes.av_pes_chan_id,
                                       rawpid_ch->raw_pid_chan_id);
    } else if (rplay_fct->secure_mode == DMX_SECURE_TEE) {
        dmx_play_type play_type = rplay_fct->play_fct_type;

        /* play alone needs to disable the pid_tab, but play&record don't need to */
        osal_mutex_lock(&rawpid_ch->lock);
        if ((osal_atomic_read(&rawpid_ch->ref_play_cnt) == 0x1) &&
             osal_atomic_read(&rawpid_ch->ref_rec_cnt) == 0x0) {
            play_type |= DMX_PLAY_TYPE_REC; /* need to disable pid_tab */
        }
        osal_mutex_unlock(&rawpid_ch->lock);

        dmx_tee_detach_play_chan(rsub_play_fct->play_avpes.av_pes_chan_id,
                                 play_type, rawpid_ch->raw_pid_chan_id);
    }

    /* clear the avpes tab register, disable drop cc not continue until pusi, disable the pes head len check */
    dmx_hal_pid_set_av_pes_tab(rpid_ch->base.mgmt, rsub_play_fct->play_avpes.av_pes_chan_id, 0,
                               DMX_PID_TYPE_REC_SCD, HI_FALSE, HI_FALSE);

    /* disable rec scd */
    dmx_hal_scd_set_av_pes_cfg(rpid_ch->base.mgmt, rsub_play_fct->play_avpes.av_pes_chan_id, 0x0, 0x0, 0x0);

    return;
}

static hi_void __raw_pidch_detach_rec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_rec_fct *rrec_fct,
    struct dmx_r_raw_pid_ch *rawpid_ch)
{
    if (unlikely(rawpid_ch->raw_pid_ch_type != DMX_RAWPIDCH_MASTER_TYPE)) {
        /* record alone needs to disable the pid_tab, but play&record don't need to */
        osal_mutex_lock(&rawpid_ch->lock);
        if ((osal_atomic_read(&rawpid_ch->ref_play_cnt) == 0x0) && osal_atomic_read(&rawpid_ch->ref_rec_cnt) == 0x1) {
            /* disable the rawpid channle */
            dmx_hal_pid_tab_flt_dis(rpid_ch->base.mgmt, rawpid_ch->raw_pid_chan_id);
        }
        osal_mutex_unlock(&rawpid_ch->lock);

        /* disable pid channel of ts record */
        dmx_hal_pid_tab_ctl_dis_set(rpid_ch->base.mgmt, rawpid_ch->raw_pid_chan_id, DMX_PID_CHN_REC_FLAG);
    }
    /* disable the ts index */
    if (rrec_fct->index_type != DMX_REC_INDEX_TYPE_NONE && rpid_ch->pid == rrec_fct->index_src_pid) {
        HI_DBG_DEMUX("***begin disable record index here!\n");
        dmx_hal_pid_tab_ctl_dis_set(rpid_ch->base.mgmt, rawpid_ch->raw_pid_chan_id, DMX_PID_CHN_TS_SCD_FLAG);
    }

    return;
}

static hi_void _raw_pid_ch_detach_rec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj,
    struct dmx_r_raw_pid_ch *rawpid_ch)
{
    struct dmx_r_rec_fct    *rrec_fct = HI_NULL;

    rrec_fct = (struct dmx_r_rec_fct *)obj;

    if (rrec_fct->secure_mode != DMX_SECURE_TEE) {
        __raw_pidch_detach_rec(rpid_ch, rrec_fct, rawpid_ch);
    } else {
        dmx_tee_rec_detach_info rec_detach_info = {0};
        rec_detach_info.is_descram = HI_FALSE;
        rec_detach_info.is_video_index = HI_FALSE;
        rec_detach_info.is_rec_only = HI_FALSE;
        rec_detach_info.chan_id = rrec_fct->base.id;
        rec_detach_info.raw_pidch_id = rawpid_ch->raw_pid_chan_id;
        if (unlikely(rawpid_ch->raw_pid_ch_type != DMX_RAWPIDCH_MASTER_TYPE)) {
            /* record alone needs to disable the pid_tab, but play&record don't need to */
            osal_mutex_lock(&rawpid_ch->lock);
            if ((osal_atomic_read(&rawpid_ch->ref_play_cnt) == 0x0) &&
                osal_atomic_read(&rawpid_ch->ref_rec_cnt) == 0x1) {
                rec_detach_info.is_rec_only = HI_TRUE;
            }
            osal_mutex_unlock(&rawpid_ch->lock);
        }
        /* disable the ts index */
        if (rrec_fct->index_type == DMX_REC_INDEX_TYPE_VIDEO && rpid_ch->pid == rrec_fct->index_src_pid) {
            rec_detach_info.is_video_index = HI_TRUE;
        }

        dmx_tee_detach_rec_chan(&rec_detach_info);
    }

    return;
}

static hi_s32 _raw_pid_ch_detach_dsc(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj)
{
    struct list_head *node = HI_NULL;
    struct dmx_r_dsc_fct    *rdsc_fct = HI_NULL;

    HI_DBG_DEMUX("***stop descrambler here!\n");
    rdsc_fct = (struct dmx_r_dsc_fct *)obj;

    if (rpid_ch->rdsc_fct == HI_NULL || rpid_ch->rdsc_fct->dsc_fct_handle == HI_INVALID_HANDLE) {
        HI_ERR_DEMUX("pid channel and descrambler not attach!\n");
        return HI_ERR_DMX_NOATTACH_KEY;
    }

    if (rpid_ch->rdsc_fct != HI_NULL && rpid_ch->rdsc_fct->dsc_fct_handle != rdsc_fct->dsc_fct_handle) {
        HI_ERR_DEMUX("pid channel and descrambler is unmatch! pid_handle=%#x, dsc_handle=%#x\n",
            rpid_ch->pid_ch_handle, rdsc_fct->dsc_fct_handle);
        return HI_ERR_DMX_UNMATCH_KEY;
    }

    osal_mutex_lock(&rpid_ch->raw_pid_ch_list_lock);
    list_for_each(node, &rpid_ch->raw_pid_ch_head) {
        struct dmx_r_raw_pid_ch *rraw_pid_ch = list_entry(node, struct dmx_r_raw_pid_ch, node1);
        /* disable the descrambler */
        dmx_hal_pid_cw_en_set(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, HI_FALSE);
        dmx_hal_pid_set_cw_id(rpid_ch->base.mgmt, rraw_pid_ch->raw_pid_chan_id, 0);
        /* disable ts descrambler support */
        dmx_hal_pid_set_dsc_type(rpid_ch->base.mgmt, rdsc_fct->base.id, HI_FALSE, HI_FALSE);
    }
    osal_mutex_unlock(&rpid_ch->raw_pid_ch_list_lock);

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_pid_ch_detach_raw_pid_ch_impl(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;
    dmx_secure_mode secure_mode = DMX_SECURE_NONE;

    DMX_NULL_POINTER_RETURN(rpid_ch);
    DMX_NULL_POINTER_RETURN(obj);

    if (IS_PLAYFCT(obj)) {
        struct dmx_r_play_fct *rplayfct = (struct dmx_r_play_fct *)obj;
        rraw_pid_ch = _get_raw_pid_obj_for_play(rpid_ch, rplayfct);
        DMX_NULL_POINTER_RETURN(rraw_pid_ch);

        if (IS_PLAYFCT_TS(obj)) {
            _raw_pid_ch_detach_ts(rpid_ch, obj, rraw_pid_ch);
        } else if (IS_PLAYFCT_PES(obj) || IS_PLAYFCT_SEC(obj)) {
            _raw_pid_ch_detach_pes_sec(rpid_ch, obj, rraw_pid_ch);
        } else if (IS_PLAYFCT_AVPES(obj)) {
            _raw_pid_ch_detach_av_pes(rpid_ch, obj, rraw_pid_ch);
        } else {
            HI_ERR_DEMUX("invalid fct object type!\n");
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        /* clear the play refer count */
        osal_mutex_lock(&rraw_pid_ch->lock);
        if (osal_atomic_read(&rraw_pid_ch->ref_play_cnt) == 0x1) {
            osal_atomic_set(&rraw_pid_ch->ref_play_cnt, 0);
            HI_DBG_DEMUX("find the attached play raw_pid_ch obj, detach it\n");
        }
        osal_mutex_unlock(&rraw_pid_ch->lock);

        secure_mode = rplayfct->secure_mode;
    } else if (IS_RECFCT(obj)) {
        struct dmx_r_rec_fct *rrecfct = (struct dmx_r_rec_fct *)obj;

        rraw_pid_ch = _get_raw_pid_obj_for_rec(rpid_ch, rrecfct);
        DMX_NULL_POINTER_RETURN(rraw_pid_ch);
        _raw_pid_ch_detach_rec(rpid_ch, obj, rraw_pid_ch);
        /* clear the rec refer count */
        osal_mutex_lock(&rraw_pid_ch->lock);
        if (osal_atomic_read(&rraw_pid_ch->ref_rec_cnt) == 0x1) {
            osal_atomic_set(&rraw_pid_ch->ref_rec_cnt, 0);
            HI_DBG_DEMUX("find the attached rec raw_pid_ch obj, detach it\n");
        }
        osal_mutex_unlock(&rraw_pid_ch->lock);

        secure_mode = rrecfct->secure_mode;
    } else if (IS_DSCFCT(obj)) {
        ret = _raw_pid_ch_detach_dsc(rpid_ch, obj);
        if (ret != HI_SUCCESS) {
            goto out;
        }
        rpid_ch->rdsc_fct = HI_NULL;
        /* decrease the pidch ref, sync with the pid object ref */
        if (osal_atomic_dec_return(&rpid_ch->pid_ch_handle_ref_cnt) != 0) {
            HI_DBG_DEMUX("pid_ch_handle_ref_cnt is not 0.\n");
        }
        ret = HI_SUCCESS;
        goto out;
    } else {
        HI_ERR_DEMUX("invalid fct object type!\n");
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    /* play and rec channel need to check pid copy and try destroy it */
    ret = _raw_pid_ch_detach_pid_copy(rpid_ch, secure_mode, rraw_pid_ch);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("_raw_pid_ch_detach_pid_copy failed!\n");
        goto out;
    }

    /* decrease the pidch ref, sync with the pid object ref */
    if (osal_atomic_dec_return(&rpid_ch->pid_ch_handle_ref_cnt) != 0) {
        HI_DBG_DEMUX("pid_ch_handle_ref_cnt is not 0.\n");
    }
    ret = HI_SUCCESS;
out:
    return ret;
}

/* private interface, no need to check staled */
static hi_s32 dmx_r_pid_ch_detach_raw_pid_ch_impl(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rpid_ch->lock);

    ret = _dmx_r_pid_ch_detach_raw_pid_ch_impl(rpid_ch, obj);

    osal_mutex_unlock(&rpid_ch->lock);

    return ret;
}

static hi_s32 dmx_r_pid_ch_lock_out_impl(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;
    struct dmx_mgmt  *mgmt = rpid_ch->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    osal_mutex_lock(&rpid_ch->lock);

    if (pid_ch_staled(rpid_ch)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    rraw_pid_ch = list_first_entry(&rpid_ch->raw_pid_ch_head, struct dmx_r_raw_pid_ch, node1);
    DMX_NULL_POINTER_GOTO(rraw_pid_ch, out);

    if (rpid_ch->pid_copy_en_status == HI_TRUE && rraw_pid_ch->rplay_fct->live_play == HI_FALSE) {
        osal_mutex_lock(&mgmt->pid_copy_chan_lock);
        /* disable the rx pid copy featuer */
        dmx_hal_pid_copy_rx_en_set(mgmt, rpid_ch->pid_copy_chan_id, HI_FALSE);
        osal_mutex_unlock(&mgmt->pid_copy_chan_lock);
    } else {
        HI_ERR_DEMUX("pidcopy not enable, don't support lock.\n");
        ret = HI_ERR_DMX_NOT_SUPPORT;
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rpid_ch->lock);

    return ret;
}

static hi_s32 dmx_r_pid_ch_un_lock_out_impl(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    struct dmx_r_raw_pid_ch *rraw_pid_ch = HI_NULL;
    struct dmx_mgmt  *mgmt = rpid_ch->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    osal_mutex_lock(&rpid_ch->lock);
    if (pid_ch_staled(rpid_ch)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    rraw_pid_ch = list_first_entry(&rpid_ch->raw_pid_ch_head, struct dmx_r_raw_pid_ch, node1);
    DMX_NULL_POINTER_GOTO(rraw_pid_ch, out);

    if (rpid_ch->pid_copy_en_status == HI_TRUE && rraw_pid_ch->rplay_fct->live_play == HI_FALSE) {
        osal_mutex_lock(&mgmt->pid_copy_chan_lock);
        /* enable the rx pid copy featuer */
        dmx_hal_pid_copy_rx_en_set(mgmt, rpid_ch->pid_copy_chan_id, HI_TRUE);
        osal_mutex_unlock(&mgmt->pid_copy_chan_lock);
    } else {
        HI_ERR_DEMUX("pidcopy not enable, don't support unlock.\n");
        ret = HI_ERR_DMX_NOT_SUPPORT;
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rpid_ch->lock);

    return ret;
}

static hi_s32 _dmx_r_pid_ch_close_impl(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    /* deconfig the pid channel */
    dmx_hal_pid_ch_de_config(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id);

    /* post close begin */
    /* if the pid copy channnel is enable, disable it when close pid channel */
    if (rpid_ch->pid_copy_en_status == HI_TRUE) {
        /* diable the pid copy configure */
        dmx_hal_pid_tab_ctl_dis_set(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id, DMX_PID_CHN_PIDCOPY_FLAG);
        /* set the pid copy id of master pid channel as 0 */
        dmx_hal_pid_tab_set_pcid(rpid_ch->base.mgmt, rpid_ch->master_raw_pid_ch_id, 0);
        /* clear the pid copy channel */
        dmx_hal_pid_copy_clear_chan(rpid_ch->base.mgmt, rpid_ch->pid_copy_chan_id);
        ret = _dmx_r_pid_ch_destroy_pcch(rpid_ch);
        if (ret != HI_SUCCESS) {
            goto out;
        }
        rpid_ch->pid_copy_en_status = HI_FALSE;
    }

    /* try to destroy all raw pidch with the same pid */
    ret = destroy_all_raw_pid_ch(rpid_ch);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy_all_raw_pid_ch failed!\n");
        goto out;
    }
    /* post close end */
    ret = HI_SUCCESS;
out:
    return ret;
}


static hi_s32 dmx_r_pid_ch_close_impl(struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rpid_ch->lock);

    if (pid_ch_staled(rpid_ch)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_r_pid_ch_close_impl(rpid_ch);
    if (ret == HI_SUCCESS) {
        rpid_ch->staled = HI_TRUE;
    }

    /* set the pid_ch_handle_ref_cnt as 0 in the laster time */
    osal_atomic_set(&rpid_ch->pid_ch_handle_ref_cnt, 0);

out:
    osal_mutex_unlock(&rpid_ch->lock);

    return ret;
}


static hi_s32 dmx_r_pid_ch_suspend_impl(struct dmx_r_pid_ch *rpid_ch)
{
    osal_mutex_lock(&rpid_ch->lock);

    WARN_ON(rpid_ch->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_pid_ch_close_impl(rpid_ch));

    osal_mutex_unlock(&rpid_ch->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_pid_ch_resume_impl(struct dmx_r_pid_ch *rpid_ch)
{
    osal_mutex_lock(&rpid_ch->lock);

    WARN_ON(rpid_ch->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_pid_ch_open_impl(rpid_ch));

    osal_mutex_unlock(&rpid_ch->lock);

    return HI_SUCCESS;
}

/*************************dmx_r_pid_channel_xxx_impl*************************************************/
struct dmx_r_pid_ch_ops g_dmx_pid_ch_ops = {
    .open            = dmx_r_pid_ch_open_impl,
    .get_status      = dmx_r_pid_ch_get_status_impl,
    .lock_out        = dmx_r_pid_ch_lock_out_impl,
    .un_lock_out     = dmx_r_pid_ch_un_lock_out_impl,
    .close           = dmx_r_pid_ch_close_impl,
    /* private interface, no need to check staled */
    .attach          = dmx_r_pid_ch_attach_raw_pid_ch_impl,
    .detach          = dmx_r_pid_ch_detach_raw_pid_ch_impl,

    .suspend         = dmx_r_pid_ch_suspend_impl,
    .resume          = dmx_r_pid_ch_resume_impl,
};

/*************************dmx_mgmt_pcr_fct*************************************************/
hi_s32 dmx_mgmt_create_pcr_fct(struct dmx_r_band *rband, hi_u32 pid, struct dmx_r_pcr_fct **rpcr_fct)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (rband->port == DMX_PORT_MAX) {
        HI_ERR_DEMUX("band has not attach port yet(%d).\n", rband->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_pcr_fct(mgmt, rband, pid, rpcr_fct);

out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_pcr_fct(struct dmx_r_base *obj)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    return mgmt->ops->destroy_pcr_fct(mgmt, (struct dmx_r_pcr_fct *)obj);
}

hi_s32 dmx_pcr_fct_create(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_pcr_fct *rpcr_fct = HI_NULL;
    struct dmx_r_band  *rband = HI_NULL;

    /* get the band obj */
    ret = DMX_R_BAND_GET(band_handle, rband);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_mgmt_create_pcr_fct(rband, pid, &rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_r_get_raw((struct dmx_r_base *)rpcr_fct);

    ret = rpcr_fct->ops->create(rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rpcr_fct, &slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    /* associate an slot to the session. */
    slot->release = dmx_pcr_fct_destroy;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out4;
    }

    *handle = slot->handle;

    /* save the pcr handle */
    rpcr_fct->handle = slot->handle;

    dmx_r_put((struct dmx_r_base *)rpcr_fct);

    dmx_r_put((struct dmx_r_base *)rband);

    return HI_SUCCESS;

out4:
    dmx_slot_destroy(slot);
out3:
    rpcr_fct->ops->destroy(rpcr_fct);
out2:
    dmx_r_put((struct dmx_r_base *)rpcr_fct);

    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rband);
out0:
    return ret;
}

hi_s32 dmx_pcr_fct_attach(hi_handle handle, hi_handle sync_handle)
{
    hi_s32 ret;
    struct dmx_r_pcr_fct *rpcr_fct = HI_NULL;

    ret = DMX_R_PCR_FCT_GET(handle, rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rpcr_fct->ops->attach(rpcr_fct, sync_handle);

    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out:
    return ret;
}

hi_s32 dmx_pcr_fct_detach(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_pcr_fct *rpcr_fct = HI_NULL;

    ret = DMX_R_PCR_FCT_GET(handle, rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rpcr_fct->ops->detach(rpcr_fct);

    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out:
    return ret;
}

hi_s32 dmx_pcr_fct_get_pcr_scr(hi_handle handle, hi_u64 *pcr_ms, hi_u64 *scr_ms)
{
    hi_s32 ret;
    struct dmx_r_pcr_fct *rpcr_fct = HI_NULL;

    ret = DMX_R_PCR_FCT_GET(handle, rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rpcr_fct->ops->get_pcr_scr(rpcr_fct, pcr_ms, scr_ms);

    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out:
    return ret;
}

hi_s32 dmx_pcr_fct_get_status(hi_handle handle, dmx_pcr_status *status)
{
    hi_s32 ret;
    struct dmx_r_pcr_fct *rpcr_fct = HI_NULL;

    ret = DMX_R_PCR_FCT_GET(handle, rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rpcr_fct->ops->get_status(rpcr_fct, status);

    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out:
    return ret;
}

hi_s32 dmx_pcr_fct_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_pcr_fct *rpcr_fct = HI_NULL;

    ret = DMX_R_PCR_FCT_GET(handle, rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rpcr_fct->ops->destroy(rpcr_fct);

    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out0:
    return ret;
}

hi_s32 dmx_pcr_fct_set_callback(hi_handle handle, hi_handle user_handle, drv_dmx_callback cb)
{
    hi_s32 ret;
    struct dmx_r_pcr_fct *rpcr_fct = HI_NULL;

    ret = DMX_R_PCR_FCT_GET(handle, rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&rpcr_fct->lock);

    rpcr_fct->callback.user_handle = user_handle;
    rpcr_fct->callback.cb = cb;

    osal_mutex_unlock(&rpcr_fct->lock);

    dmx_r_put((struct dmx_r_base *)rpcr_fct);
out:
    return ret;
}

/*************************dmx_r_pcr_fct_xxx_impl*************************************************/
static inline hi_bool pcr_fct_staled(struct dmx_r_pcr_fct *rpcr_fct)
{
    if (unlikely(rpcr_fct->staled == HI_TRUE)) {
        HI_ERR_DEMUX("pcr_fct is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _dmx_r_pcr_fct_create_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    hi_s32 ret;
    hi_u32 raw_port_id = 0;
    enum dmx_port_type_e port_mode = DMX_PORT_MAX_TYPE;
    struct dmx_mgmt  *mgmt = rpcr_fct->base.mgmt;
    struct dmx_hal_pcr_info pcr_info;

    DMX_NULL_POINTER_RETURN(mgmt);

    ret = dmx_get_port_type_and_id(rpcr_fct->port, &port_mode, &raw_port_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid en_port_id[0x%x]\n", rpcr_fct->port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    pcr_info.mgmt = mgmt;
    pcr_info.pcr_id = rpcr_fct->base.id;
    pcr_info.pcr_pid = rpcr_fct->pid;
    pcr_info.pcr_src = DMX_PCR_SRC_TYPE_BAND;
    pcr_info.port_type = port_mode;
    pcr_info.raw_port_id = raw_port_id;
    pcr_info.band = rpcr_fct->rband->base.id;
    /* enable the pcr configure */
    dmx_hal_pcr_en(&pcr_info);

    /* eanble the pcr interrupt */
    dmx_hal_pcr_int_en(mgmt, rpcr_fct->base.id);
out:
    return ret;
}

static hi_s32 dmx_r_pcr_fct_create_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rpcr_fct->lock);

    WARN_ON(rpcr_fct->staled == HI_TRUE);

    ret = _dmx_r_pcr_fct_create_impl(rpcr_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = HI_SUCCESS;

out0:
    osal_mutex_unlock(&rpcr_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_pcr_fct_attach_impl(struct dmx_r_pcr_fct *rpcr_fct, hi_handle sync_handle)
{
    hi_s32 ret;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_pcr_fct_attach_impl(struct dmx_r_pcr_fct *rpcr_fct, hi_handle sync_handle)
{
    hi_s32 ret;

    osal_mutex_lock(&rpcr_fct->lock);

    ret = _dmx_r_pcr_fct_attach_impl(rpcr_fct, sync_handle);
    if (ret == HI_SUCCESS) {
        /* record the port */
        rpcr_fct->sync_handle = sync_handle;
    }

    osal_mutex_unlock(&rpcr_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_pcr_fct_detach_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    hi_s32 ret = HI_FAILURE;

    if (pcr_fct_staled(rpcr_fct)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_r_pcr_fct_detach_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rpcr_fct->lock);

    ret = _dmx_r_pcr_fct_detach_impl(rpcr_fct);

    osal_mutex_unlock(&rpcr_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_pcr_fct_get_status_impl(struct dmx_r_pcr_fct *rpcr_fct, dmx_pcr_status *status)
{
    hi_s32 ret = HI_FAILURE;

    if (pcr_fct_staled(rpcr_fct)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    status->band          = rpcr_fct->rband->base.id;
    status->pid           = rpcr_fct->pid;
    status->port          = rpcr_fct->port;
    status->sync_handle   = rpcr_fct->sync_handle;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_pcr_fct_get_status_impl(struct dmx_r_pcr_fct *rpcr_fct, dmx_pcr_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rpcr_fct->lock);

    ret = _dmx_r_pcr_fct_get_status_impl(rpcr_fct, status);

    osal_mutex_unlock(&rpcr_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_pcr_fct_get_pcr_scr_impl(struct dmx_r_pcr_fct *rpcr_fct, hi_u64 *pcr_ms, hi_u64 *scr_ms)
{
    hi_s32 ret = HI_FAILURE;

    if (pcr_fct_staled(rpcr_fct)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    *pcr_ms = rpcr_fct->pcr_val;
    *scr_ms = rpcr_fct->scr_val;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_pcr_fct_get_pcr_scr_impl(struct dmx_r_pcr_fct *rpcr_fct, hi_u64 *pcr_ms, hi_u64 *scr_ms)
{
    hi_s32 ret;

    osal_mutex_lock(&rpcr_fct->lock);

    ret = _dmx_r_pcr_fct_get_pcr_scr_impl(rpcr_fct, pcr_ms, scr_ms);

    osal_mutex_unlock(&rpcr_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_pcr_fct_destroy_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    struct dmx_mgmt  *mgmt = rpcr_fct->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    /* diable the pcr configure */
    dmx_hal_pcr_dis(mgmt, rpcr_fct->base.id);

    /* disable the pcr interrupt */
    dmx_hal_pcr_int_dis(mgmt, rpcr_fct->base.id);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_pcr_fct_destroy_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&rpcr_fct->lock);

    if (pcr_fct_staled(rpcr_fct)) {
        ret = HI_ERR_DMX_STALED;
        goto out;
    }
    ret = _dmx_r_pcr_fct_destroy_impl(rpcr_fct);
    if (ret == HI_SUCCESS) {
        rpcr_fct->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&rpcr_fct->lock);

    return ret;
}

static hi_s32 dmx_r_pcr_fct_suspend_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    osal_mutex_lock(&rpcr_fct->lock);

    WARN_ON(rpcr_fct->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_pcr_fct_destroy_impl(rpcr_fct));

    osal_mutex_unlock(&rpcr_fct->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_pcr_fct_resume_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    osal_mutex_lock(&rpcr_fct->lock);

    WARN_ON(rpcr_fct->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_pcr_fct_create_impl(rpcr_fct));

    osal_mutex_unlock(&rpcr_fct->lock);

    return HI_SUCCESS;
}

/*************************dmx_r_pcr_fct_xxx_impl*************************************************/
struct dmx_r_pcr_fct_ops g_dmx_pcr_fct_ops = {
    .create        = dmx_r_pcr_fct_create_impl,
    .attach        = dmx_r_pcr_fct_attach_impl,
    .detach        = dmx_r_pcr_fct_detach_impl,
    .get_status    = dmx_r_pcr_fct_get_status_impl,
    .get_pcr_scr   = dmx_r_pcr_fct_get_pcr_scr_impl,
    .destroy       = dmx_r_pcr_fct_destroy_impl,

    .suspend       = dmx_r_pcr_fct_suspend_impl,
    .resume        = dmx_r_pcr_fct_resume_impl,
};

/*************************dmx_r_buf_xxx_impl*************************************************/
static hi_void buf_irq_handler(struct work_struct *work)
{
    struct dmx_data_sel *wait_mgr = HI_NULL;
    struct dmx_r_buf *rbuf = container_of(work, struct dmx_r_buf, buf_worker);

    osal_mutex_lock(&rbuf->interrupt_lock);

    rbuf->wait_cond = HI_TRUE;
    wake_up_interruptible(&rbuf->wait_queue);

    /* notify avplay when avpes type, only avpes open mq_use_flag */
    if (rbuf->buf_type == DMX_BUF_TYPE_AVPES && rbuf->callback.cb != HI_NULL) {
        rbuf->callback.cb(rbuf->callback.user_handle, HI_DMX_EVT_NEW_PES, HI_NULL, 0);
    }

    if (rbuf->select_wait_queue_flag == HI_TRUE) {
        wait_mgr = &(rbuf->base.mgmt->select_wait_queue);
        wait_mgr->condition = 1;
        wake_up_interruptible(&wait_mgr->wait_queue);
    }

    osal_mutex_unlock(&rbuf->interrupt_lock);
}

static hi_s32 _dmx_buf_sec_overflow_close_impl(struct dmx_r_buf *rbuf);
static hi_s32 _dmx_r_buf_open_impl(struct dmx_r_buf *rbuf);

static hi_void buf_ovfl_irq_handler(struct work_struct *work)
{
    struct dmx_mgmt *mgmt = HI_NULL;
    struct list_head *temp = HI_NULL;
    struct dmx_r_play_fct *play_fct = HI_NULL;
    struct dmx_r_buf *rbuf =  container_of(work, struct dmx_r_buf, buf_ovfl_worker);

    osal_mutex_lock(&rbuf->interrupt_lock);

    rbuf->ovfl_flag = HI_TRUE;
    rbuf->ovfl_cnt++;

    if (rbuf->callback.cb != HI_NULL) {
        rbuf->callback.cb(rbuf->callback.user_handle, HI_DMX_EVT_OVFL, HI_NULL, 0);
    }

    if (rbuf->buf_type != DMX_BUF_TYPE_SEC) {
        osal_mutex_unlock(&rbuf->interrupt_lock);
        return;
    }

    mgmt = rbuf->base.mgmt;
    if (mgmt == HI_NULL) {
        osal_mutex_unlock(&rbuf->interrupt_lock);
        return;
    }
    osal_mutex_unlock(&rbuf->interrupt_lock);

    osal_mutex_lock(&mgmt->play_fct_list_lock);
    list_for_each(temp, &mgmt->play_fct_head) {
        play_fct = list_entry(temp, struct dmx_r_play_fct, node);
        osal_mutex_lock(&play_fct->lock);
        if (play_fct->rbuf == rbuf) {
            struct dmx_r_raw_pid_ch *raw_pid_ch = _get_raw_pid_obj_for_play(play_fct->rpid_ch, play_fct);
            if (raw_pid_ch) {
                dmx_hal_pid_tab_flt_dis(mgmt, raw_pid_ch->raw_pid_chan_id);

                dmx_hal_band_clear_chan(mgmt, play_fct->rpid_ch->rband->base.id);

                dmx_hal_dav_tx_clear_chan(mgmt, rbuf->base.id);

                _dmx_buf_sec_overflow_close_impl(rbuf);

                _dmx_r_buf_open_impl(rbuf);

                dmx_hal_pid_tab_flt_en(mgmt, raw_pid_ch->raw_pid_chan_id);
            } else {
                HI_ERR_DEMUX("null pointer\n");
            }
            osal_mutex_unlock(&play_fct->lock);
            break;
        }
        osal_mutex_unlock(&play_fct->lock);
    }
    osal_mutex_unlock(&mgmt->play_fct_list_lock);

    HI_WARN_DEMUX("overflow interrupt happened, buf_id[%#x]\n", rbuf->base.id);
}

static inline hi_bool buf_staled(struct dmx_r_buf *rbuf)
{
    if (unlikely(rbuf->staled == HI_TRUE)) {
        HI_ERR_DEMUX("buf is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_s32 dmx_mgmt_create_buf(struct dmx_buf_attrs *attrs, struct dmx_r_buf **rbuf)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (!attrs) {
        HI_ERR_DEMUX("invalid attrs(%#x).\n", attrs);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_buf(mgmt, attrs, rbuf);
out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_buf(struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_buf(mgmt, (struct dmx_r_buf *)obj);

    return ret;
}

static hi_s32 _dmx_r_buf_open_impl(struct dmx_r_buf *rbuf)
{
    hi_bool secure;
    hi_u32 port_offset = 0;

    secure = (rbuf->secure_mode == DMX_SECURE_TEE) ? HI_TRUE : HI_FALSE;

    rbuf->acquire_cnt = 0;
    rbuf->acquire_ok_cnt = 0;
    rbuf->release_cnt = 0;

    /* general */
    rbuf->sw_read               = 0;
    rbuf->last_addr             = rbuf->buf_phy_addr;
    rbuf->last_read             = 0;
    rbuf->last_end              = 0;
    rbuf->rool_flag             = HI_FALSE;
    rbuf->eos_flag              = HI_FALSE;
    rbuf->ovfl_flag             = HI_FALSE;
    rbuf->eos_cnt               = 0;
    rbuf->eos_record_idx        = 0;
    rbuf->ovfl_cnt              = 0;

    /* force clear the output buffer ap status */
    if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
        port_offset = rbuf->port - DMX_RAM_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_ram_port_cls_ap_status(rbuf->base.mgmt, port_offset);
    } else if (rbuf->port >= DMX_TSIO_PORT_0 && rbuf->port < DMX_TSIO_PORT_MAX) {
        port_offset = rbuf->port - DMX_TSIO_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_buf_clr_tsio_ap(rbuf->base.mgmt, port_offset);
    }

    /* clear buf mmu cache */
    dmx_hal_buf_clr_mmu_cache(rbuf->base.mgmt, rbuf->base.id);

    /* init and open the buffer configure */
    dmx_hal_buf_config(rbuf->base.mgmt, rbuf->base.id, rbuf->buf_phy_addr, rbuf->buf_size, secure);
    dmx_hal_buf_open(rbuf->base.mgmt, rbuf->base.id, rbuf->time_int_th, rbuf->ts_cnt_int_th, rbuf->seop_int_th);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_open_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_open_impl(rbuf);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = HI_SUCCESS;

out0:
    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_get_attrs_impl(struct dmx_r_buf *rbuf, struct dmx_buf_attrs *attr)
{
    hi_s32 ret;

    attr->buf_handle  = rbuf->buf_handle;
    attr->buf_size    = rbuf->buf_size;
    attr->secure_mode = rbuf->secure_mode;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_buf_get_attrs_impl(struct dmx_r_buf *rbuf, struct dmx_buf_attrs *attr)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_get_attrs_impl(rbuf, attr);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_get_status_impl(struct dmx_r_buf *rbuf, struct dmx_buf_status *status)
{
    hi_s32 ret;

    struct dmx_mgmt        *mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    status->buf_ker_addr = rbuf->buf_ker_addr;
    status->buf_phy_addr = rbuf->buf_phy_addr;
    status->buf_size    = rbuf->buf_size;
    status->buf_handle  = rbuf->buf_handle;
    status->offset      = 0;

    status->hw_read  = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);
    /* attention: before read 'write ponint, must read 'end'  point first */
    status->hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
    status->hw_end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);
    status->buf_used_size = dmx_get_queue_lenth(status->hw_read, status->hw_write, status->buf_size);

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_buf_get_status_impl(struct dmx_r_buf *rbuf, struct dmx_buf_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_get_status_impl(rbuf, status);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_attach_impl(struct dmx_r_buf *rbuf, dmx_port port)
{
    hi_s32 ret;

    /* enable tsio anti_pressure */
    if (port >= DMX_TSIO_PORT_0 && port < DMX_TSIO_PORT_MAX) {
        ret = dmx_hal_buf_set_tsio_ap(rbuf->base.mgmt, rbuf->base.id, HI_TRUE, port - DMX_TSIO_PORT_0);
    } else {
        ret = HI_SUCCESS;
    }

    if (ret == HI_SUCCESS) {
        rbuf->port = port;
    }

    return ret;
}

static hi_s32 dmx_r_buf_attach_impl(struct dmx_r_buf *rbuf, dmx_port port)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_attach_impl(rbuf, port);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_detach_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    /* disable tsio anti_pressure */
    if ((rbuf->port >= DMX_TSIO_PORT_0) && (rbuf->port < DMX_TSIO_PORT_MAX)) {
        ret = dmx_hal_buf_set_tsio_ap(rbuf->base.mgmt, rbuf->base.id, HI_FALSE, rbuf->port - DMX_TSIO_PORT_0);
    } else {
        ret = HI_SUCCESS;
    }

    if (ret == HI_SUCCESS) {
        rbuf->port = DMX_RAM_PORT_MAX;
    }

    return ret;
}

static hi_s32 dmx_r_buf_detach_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_detach_impl(rbuf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_pre_mmap_impl(struct dmx_r_buf *rbuf, hi_s64 *buf_handle, hi_u32 *buf_size,
                                       hi_void **buf_usr_addr)
{
    hi_s32 ret;

    *buf_handle = rbuf->buf_handle;
    /* for pes channel, rbuf->real_buf_size == rbuf->buf_size + 64K */
    *buf_size = rbuf->real_buf_size;
    *buf_usr_addr = rbuf->buf_usr_addr;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_buf_pre_mmap_impl(struct dmx_r_buf *rbuf, hi_s64 *buf_handle, hi_u32 *buf_size,
                                      hi_void **buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_pre_mmap_impl(rbuf, buf_handle, buf_size, buf_usr_addr);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_pst_mmap_impl(struct dmx_r_buf *rbuf, hi_void *user_addr)
{
    hi_s32 ret;

    WARN_ON(rbuf->buf_usr_addr != HI_NULL);

    rbuf->buf_usr_addr = user_addr;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_buf_pst_mmap_impl(struct dmx_r_buf *rbuf, hi_void *user_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_pst_mmap_impl(rbuf, user_addr);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 dmx_r_buf_acquire_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
    dmx_buffer *play_fct_buf)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_buf_release_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    HI_ERR_DEMUX("some error may happened, not support!\n");
    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 _dmx_r_buf_acquire_ts_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
    dmx_buffer *play_fct_buf)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 end_addr;
    hi_u32 hw_write;
    hi_u32 hw_read;

    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    /* acquire cnt++ */
    rbuf->acquire_cnt++;

    hw_read  = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);
    /* attention: before read 'write ponint, must read 'end'  point first */
    hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
    end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);

    /* get the data */
    WARN_ON(hw_read != rbuf->sw_read);
    WARN_ON(rbuf->last_read > rbuf->buf_size);

    if (hw_write == rbuf->last_read) {
        *acqed_num = 0;
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    play_fct_buf->buf_handle = rbuf->buf_handle;
    play_fct_buf->phy_addr = rbuf->buf_phy_addr + rbuf->last_read;
    play_fct_buf->offset = play_fct_buf->phy_addr - rbuf->buf_phy_addr;
    play_fct_buf->data = rbuf->buf_usr_addr + rbuf->last_read;

    if (likely(hw_read < hw_write)) { /* not rool back */
        play_fct_buf->length = hw_write - rbuf->last_read;
        rbuf->last_read = hw_write;
    } else if (hw_read > hw_write) { /* rool back */
        if (rbuf->rool_flag == HI_FALSE) {
            play_fct_buf->length = end_addr - rbuf->last_read;
            rbuf->last_read = 0;
            rbuf->rool_flag = HI_TRUE;
            rbuf->last_end   = end_addr;
        } else {
            play_fct_buf->length = hw_write - rbuf->last_read;
            rbuf->last_read = hw_write;
        }
    } else {
        *acqed_num = 0;
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    *acqed_num = 1;

    /* acquire ok cnt++ */
    rbuf->acquire_ok_cnt++;

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_buf_acquire_ts_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
    dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_acquire_ts_impl(rbuf, acq_num, time_out, acqed_num, play_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_release_ts_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret = HI_FAILURE;

    hi_u32 rel_len = play_fct_buf->length;
    hi_u8  *usr_addr = (hi_u8 *)((hi_ulong)play_fct_buf->data);

    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    if ((rel_len > rbuf->buf_size) ||
        (usr_addr < rbuf->buf_usr_addr || usr_addr >= rbuf->buf_usr_addr + rbuf->buf_size) ||
        (usr_addr + rel_len < usr_addr) || (usr_addr + rel_len > rbuf->buf_usr_addr + rbuf->buf_size)) {
        HI_ERR_DEMUX("bufid[%d], phy_addr[0x%x], rel_len[0x%x], buf_phy[0x%x], buf_size[0x%x]!\n",
                     rbuf->base.id, usr_addr, rel_len, rbuf->buf_usr_addr, rbuf->buf_size);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    rbuf->sw_read = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);

    rbuf->sw_read += rel_len;

    /* if the release the data block is the last block of the buffer */
    if (unlikely(rbuf->sw_read == rbuf->last_end && rbuf->rool_flag == HI_TRUE)) {
        rbuf->sw_read = 0;
        rbuf->last_end = 0;
        rbuf->rool_flag = HI_FALSE;
    }

    /* update the read idx */
    if (rbuf->secure_mode != DMX_SECURE_TEE) {
        dmx_hal_buf_set_read_idx(mgmt, rbuf->base.id, rbuf->sw_read);
    } else {
        ret = dmx_tee_update_play_buf_read_idx(rbuf->base.id, DMX_PLAY_TYPE_TS, rbuf->sw_read);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("update secure read idx failed, ret[0x%x]!\n", ret);
            goto out;
        }
    }

    /* check the buffer ap status */
    dmx_cls_buf_ap_status(mgmt, rbuf);

    /* release cnt++ */
    rbuf->release_cnt++;

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_buf_release_ts_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_release_ts_impl(rbuf, rel_num, play_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 dmx_parse_sec_header(hi_u8 *parser_addr, hi_u32 parser_len, hi_u32 *table_id, hi_u32 *section_len)
{
    if ((parser_addr == HI_NULL) || (table_id == HI_NULL) || (section_len == HI_NULL) || (parser_len < 0x3)) {
        HI_ERR_DEMUX("para invalid ! \n");
        return HI_FAILURE;
    }

    /* the total section length need to be added 3 bytes forward  */
    *table_id = parser_addr[0];
    *section_len = (((parser_addr[1] & 0x0F) << 8) | parser_addr[0x2]) + 0x3; /* 8 bits */

    return HI_SUCCESS;
}

static hi_s32 save_sec_block_to_list(struct dmx_r_sec_buf *sec_buf, dmx_buffer *play_buf)
{
    struct sec_block_info_node *sec_block_info_ptr = HI_NULL;

    /* add the es block nod into the es block list */
    sec_block_info_ptr = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct sec_block_info_node), GFP_KERNEL);
    DMX_NULL_POINTER_RETURN(sec_block_info_ptr);

    sec_block_info_ptr->buf_usr_addr = play_buf->data;
    sec_block_info_ptr->sec_data_leng = play_buf->length;
    list_add_tail(&sec_block_info_ptr->node, &sec_buf->buf_sec.sec_block_info_head);

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_buf_acquire_sec_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
    dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    hi_u32 hw_end_addr, hw_write, hw_read, hw_seop_addr;
    hi_u32 offset = 0;
    hi_u32 sec_table_id = 0;
    hi_u32 section_pkt_len   = 0;

    hi_u32 i = 0;

    struct dmx_r_sec_buf *sec_buf = (struct dmx_r_sec_buf *)rbuf;
    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    /* acquire cnt++ */
    rbuf->acquire_cnt++;

    ret = wait_event_interruptible_timeout(rbuf->wait_queue, rbuf->wait_cond, osal_msecs_to_jiffies(time_out));
    hw_read  = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);
    /* attention: before read 'write' ponint, must read 'end'  point first */
    hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
    hw_end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);
    hw_seop_addr = dmx_hal_buf_get_seop_addr(mgmt, rbuf->base.id);

    HI_DBG_DEMUX("hw_read[0x%x], hw_write[0x%x], hw_end_addr[0x%x], hw_seop_addr[0x%x]\n",
                 hw_read, hw_write, hw_end_addr, hw_seop_addr);

    if ((ret == 0) && unlikely(hw_read == hw_write)) {
        HI_WARN_DEMUX("sec playfct acquire buffer timeout!\n");
        ret = HI_ERR_DMX_TIMEOUT;
        goto out;
    }

    /* clear the buf condition */
    osal_mutex_lock(&rbuf->interrupt_lock);
    rbuf->wait_cond = HI_FALSE;
    osal_mutex_unlock(&rbuf->interrupt_lock);

    if (likely(hw_read < hw_seop_addr + 1)) { /* not rool back */
        offset = hw_read;

        do {
            ret = dmx_parse_sec_header(rbuf->buf_ker_addr + offset, DMX_SEC_HEAD_FIX_LEN,
                                       &sec_table_id, &section_pkt_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("sec head parse failed!\n");
                ret = HI_ERR_DMX_NOAVAILABLE_DATA;
                goto out;
            }

            play_fct_buf[i].buf_handle = rbuf->buf_handle;
            play_fct_buf[i].phy_addr = rbuf->buf_phy_addr + offset;
            play_fct_buf[i].offset = offset;
            play_fct_buf[i].data = rbuf->buf_usr_addr + offset;
            play_fct_buf[i].length = section_pkt_len;
            play_fct_buf[i].data_type = DMX_DATA_WHOLE;
            save_sec_block_to_list(sec_buf, &play_fct_buf[i]);

            offset += section_pkt_len;
            i++;
        } while ((offset < hw_seop_addr + 1) && (i < acq_num) && (hw_seop_addr + 1 - offset >= DMX_SEC_HEAD_FIX_LEN));
    } else if (hw_read > hw_seop_addr + 1) { /* rool back */
        WARN_ON(hw_read > rbuf->buf_size);
        offset = hw_read;

        do {
            if (hw_read >= hw_end_addr) { /* need to reset the read */
                HI_WARN_DEMUX("sec rool loop in acquire_buf!\n");
                hw_read = 0;
                dmx_hal_buf_set_read_idx(mgmt, rbuf->base.id, hw_read);
                break;
            }

            ret = dmx_parse_sec_header(rbuf->buf_ker_addr + offset, DMX_SEC_HEAD_FIX_LEN,
                                       &sec_table_id, &section_pkt_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("sec head parse failed!\n");
                ret = HI_ERR_DMX_NOAVAILABLE_DATA;
                goto out;
            }

            play_fct_buf[i].buf_handle = rbuf->buf_handle;
            play_fct_buf[i].phy_addr = rbuf->buf_phy_addr + offset;
            play_fct_buf[i].offset = offset;
            play_fct_buf[i].data = rbuf->buf_usr_addr + offset;
            play_fct_buf[i].length = section_pkt_len;
            play_fct_buf[i].data_type = DMX_DATA_WHOLE;
            save_sec_block_to_list(sec_buf, &play_fct_buf[i]);

            offset += section_pkt_len;
            if (offset == hw_end_addr) {
                HI_DBG_DEMUX("@@rool loop flag\n");
                play_fct_buf[i].data_type = DMX_DATA_TAIL;
            }
            i++;
        } while ((offset < hw_end_addr) && (i < acq_num) && (hw_end_addr - offset >= DMX_SEC_HEAD_FIX_LEN));

        if (i == acq_num) {
            *acqed_num = acq_num;
            rbuf->acquire_ok_cnt++;
            ret = HI_SUCCESS;
            goto out;
        }

        offset = 0;
        do {
            ret = dmx_parse_sec_header(rbuf->buf_ker_addr + offset, DMX_SEC_HEAD_FIX_LEN,
                                       &sec_table_id, &section_pkt_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("sec head parse failed!\n");
                ret = HI_ERR_DMX_NOAVAILABLE_DATA;
                goto out;
            }

            play_fct_buf[i].buf_handle = rbuf->buf_handle;
            play_fct_buf[i].phy_addr  = rbuf->buf_phy_addr + offset;
            play_fct_buf[i].offset = offset;
            play_fct_buf[i].data     = rbuf->buf_usr_addr + offset;
            play_fct_buf[i].length   = section_pkt_len;
            play_fct_buf[i].data_type = DMX_DATA_WHOLE;
            save_sec_block_to_list(sec_buf, &play_fct_buf[i]);

            offset += section_pkt_len;
            i++;
        } while ((offset < hw_seop_addr + 1) && (i < acq_num) && (hw_seop_addr + 1 - offset >= DMX_SEC_HEAD_FIX_LEN));
    } else {
        *acqed_num = 0;
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    *acqed_num = i;

    /* acquire ok cnt++ */
    rbuf->acquire_ok_cnt++;

    ret = HI_SUCCESS;

out:

    return ret;
}

static hi_s32 dmx_r_buf_acquire_sec_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_acquire_sec_impl(rbuf, acq_num, time_out, acqed_num, play_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_release_sec_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret = HI_FAILURE;

    hi_s32 i = 0;

    hi_u32 rel_len = 0;
    hi_u32 hw_end_addr = 0;
    hi_u8  *rel_usr_addr = HI_NULL;

    struct dmx_r_sec_buf *sec_buf = (struct dmx_r_sec_buf *)rbuf;
    struct sec_block_info_node *sec_block_p = HI_NULL;
    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    hw_end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);

    for (i = 0; i < rel_num; i++) {
        rel_usr_addr = (hi_u8 *)((hi_ulong)play_fct_buf[i].data);
        rel_len = play_fct_buf[i].length;

        /* get pes head len from local list */
        sec_block_p = list_first_entry(&sec_buf->buf_sec.sec_block_info_head, struct sec_block_info_node, node);
        DMX_NULL_POINTER_RETURN(sec_block_p);

        if (unlikely(rel_usr_addr != sec_block_p->buf_usr_addr ||
                     rel_len != sec_block_p->sec_data_leng)) {
            HI_ERR_DEMUX("bufid[%d], rel_usr_addr[0x%x], buf_usr_addr[0x%x], swread[0x%x], len[0x%x],"
                         " local addr[0x%x], local len[0x%x]!\n",
                         rbuf->base.id, rel_usr_addr, rbuf->buf_usr_addr, rbuf->sw_read,
                         rel_len, sec_block_p->buf_usr_addr, sec_block_p->sec_data_leng);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        rbuf->sw_read = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);

        rbuf->sw_read += rel_len;

        /* sec rool loop */
        if ((play_fct_buf[i].data_type == DMX_DATA_TAIL) && (rbuf->sw_read >= hw_end_addr)) {
            HI_WARN_DEMUX("**section rool loop rel_len[%#x]\n", rel_len);
            rbuf->sw_read = 0;
        }

        /* release the local list entry */
        list_del(&sec_block_p->node);
        HI_KFREE(HI_ID_DEMUX, sec_block_p);
        sec_block_p = HI_NULL;

        /* update the read idx */
        if (rbuf->secure_mode != DMX_SECURE_TEE) {
            dmx_hal_buf_set_read_idx(mgmt, rbuf->base.id, rbuf->sw_read);
        } else {
            ret = dmx_tee_update_play_buf_read_idx(rbuf->base.id, DMX_PLAY_TYPE_SEC, rbuf->sw_read);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("update secure read idx failed, ret[0x%x]!\n", ret);
                goto out;
            }
        }
    }

    /* check the buffer ap status */
    dmx_cls_buf_ap_status(mgmt, rbuf);

    /* release cnt++ */
    rbuf->release_cnt++;

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_buf_release_sec_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_release_sec_impl(rbuf, rel_num, play_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

/* PES head parse */
static hi_s32 dmx_parse_pes_header(hi_u8 *parser_addr, hi_u32 parser_len, hi_u32 *pes_header_len,
                                   hi_u32 *pes_pay_load_len,
                                   hi_u32 *pes_len)
{
    hi_s32 ret = HI_FAILURE;

    DMX_NULL_POINTER_RETURN(parser_addr);
    DMX_NULL_POINTER_RETURN(pes_header_len);
    DMX_NULL_POINTER_RETURN(pes_pay_load_len);
    DMX_NULL_POINTER_RETURN(pes_len);

    if ((parser_addr[0] != 0x00) || (parser_addr[1] != 0x00) || (parser_addr[0x2] != 0x01)) {
        HI_WARN_DEMUX("pes start byte = %x %x %x\n", parser_addr[0], parser_addr[1], parser_addr[0x2]);
        ret = HI_FAILURE;
        goto out;
    } else {
        HI_DBG_DEMUX("success pes start byte = %x %x %x\n", parser_addr[0], parser_addr[1], parser_addr[0x2]);
    }

    if (parser_len >= 0x6) {
        /* 3(packet_start_code_prefix) + 1(stream_id) +2(pes_packet_length) */
        *pes_header_len = 0x6;
        /* the value of pes_packet_length */
        *pes_pay_load_len = ((parser_addr[0x4] << 8) | parser_addr[0x5]); /* 8 bits */

        *pes_len = *pes_header_len + *pes_pay_load_len;
        ret = HI_SUCCESS;
    } else {
        HI_ERR_DEMUX("invalid pes len[0x%x]\n", parser_len);
        ret = HI_FAILURE;
    }
out:
    return ret;
}

static hi_s32 _dmx_r_buf_acquire_pes_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    hi_u32 hw_end_addr;
    hi_u32 hw_write;
    hi_u32 hw_read;
    hi_u32 hw_seop_addr;
    hi_u32 offset;
    hi_u32 pes_head_len;
    hi_u32 pes_pload_len;
    hi_u32 pes_len;

    hi_s32 i = 0;

    hi_u8 pes_head_arry[DMX_PES_HEADER_LENGTH] = { 0 };

    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    /* acquire cnt++ */
    rbuf->acquire_cnt++;

    ret = wait_event_interruptible_timeout(rbuf->wait_queue, rbuf->wait_cond, osal_msecs_to_jiffies(time_out));
    hw_read  = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);
    /* attention: before read 'write' ponint, must read 'end'  point first */
    hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
    hw_end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);
    hw_seop_addr = dmx_hal_buf_get_seop_addr(mgmt, rbuf->base.id);

    if ((ret == 0) && unlikely(hw_read == hw_write)) {
        HI_WARN_DEMUX("pes playfct acquire buffer timeout!\n");
        ret = HI_ERR_DMX_TIMEOUT;
        goto out;
    }

    /* clear the buf condition */
    osal_mutex_lock(&rbuf->interrupt_lock);
    rbuf->wait_cond = HI_FALSE;
    osal_mutex_unlock(&rbuf->interrupt_lock);

    if (likely(hw_read < hw_seop_addr)) { /* not rool back */
        offset = hw_read;
        do {
            ret = dmx_parse_pes_header(rbuf->buf_ker_addr + offset, DMX_PES_HEADER_LENGTH,
                                       &pes_head_len, &pes_pload_len, &pes_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("pes head parse failed!\n");
                ret = HI_ERR_DMX_NOAVAILABLE_DATA;
                goto out;
            }

            HI_DBG_DEMUX("pes_len[%#x], pes_pload_len[%#x], pes_head_len[%#x]\n", pes_len, pes_pload_len, pes_head_len);
            play_fct_buf[i].buf_handle  = rbuf->buf_handle;
            play_fct_buf[i].phy_addr  = rbuf->buf_phy_addr + offset;
            play_fct_buf[i].offset  = offset;
            play_fct_buf[i].data = rbuf->buf_usr_addr + offset;
            play_fct_buf[i].length  = pes_len;
            play_fct_buf[i].data_type = DMX_DATA_WHOLE;

            i++;
            offset += pes_len;
        } while ((offset < hw_seop_addr) && (i < acq_num) && (hw_seop_addr - offset >= DMX_PES_HEADER_LENGTH));
    } else if (hw_read > hw_seop_addr) { /* rool back */
        WARN_ON(hw_read > rbuf->buf_size);
        offset = hw_read;
        do {
            if (rbuf->buf_size - offset < DMX_PES_HEADER_LENGTH) {
                rbuf->rool_flag = HI_TRUE;
                memcpy(pes_head_arry, rbuf->buf_ker_addr + offset, rbuf->buf_size - offset);
                memcpy(pes_head_arry + rbuf->buf_size - offset, rbuf->buf_ker_addr,
                       DMX_PES_HEADER_LENGTH + offset - rbuf->buf_size);
            } else {
                rbuf->rool_flag = HI_FALSE;
                memcpy(pes_head_arry, rbuf->buf_ker_addr + offset, DMX_PES_HEADER_LENGTH);
            }

            ret = dmx_parse_pes_header(pes_head_arry, DMX_PES_HEADER_LENGTH,
                                       &pes_head_len, &pes_pload_len, &pes_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("pes head parse failed!\n");
                ret = HI_ERR_DMX_NOAVAILABLE_DATA;
                goto out;
            }

            HI_DBG_DEMUX("pes_len[%#x], pes_pload_len[%#x], pes_head_len[%#x]\n", pes_len, pes_pload_len, pes_head_len);
            if (offset + pes_len > rbuf->buf_size) {
                rbuf->rool_flag = HI_TRUE;
            }

            if (rbuf->rool_flag == HI_TRUE) {
                HI_DBG_DEMUX("rbuf->last_addr[%#llx], play_fct_buf->phy_addr[%#llx]\n", rbuf->last_addr,
                             rbuf->buf_phy_addr);
                if (memcpy_s(rbuf->buf_ker_addr + rbuf->buf_size, offset + pes_len - rbuf->buf_size,
                             rbuf->buf_ker_addr, offset + pes_len - rbuf->buf_size)) {
                    HI_ERR_DEMUX("memcpy_s failed!\n");
                    ret = HI_FAILURE;
                    goto out;
                }

                play_fct_buf[i].buf_handle = rbuf->buf_handle;
                play_fct_buf[i].phy_addr = rbuf->buf_phy_addr + offset;
                play_fct_buf[i].offset = offset;
                play_fct_buf[i].data = rbuf->buf_usr_addr + offset;
                play_fct_buf[i].length = pes_len;
                play_fct_buf[i].data_type = DMX_DATA_TAIL;
                rbuf->rool_flag = HI_FALSE;
                i++;
                offset = (offset + pes_len) % rbuf->buf_size;
                HI_DBG_DEMUX("***pes_len[%#x], offset[%#x]\n", pes_len, offset);
                break;
            } else {
                play_fct_buf[i].buf_handle = rbuf->buf_handle;
                play_fct_buf[i].phy_addr = rbuf->buf_phy_addr + offset;
                play_fct_buf[i].offset = offset;
                play_fct_buf[i].data = rbuf->buf_usr_addr + offset;
                play_fct_buf[i].length = pes_len;
                play_fct_buf[i].data_type = DMX_DATA_WHOLE;
                i++;
                offset += pes_len;
            }
        } while ((offset < rbuf->buf_size) && (i < acq_num) && (rbuf->buf_size - offset >= DMX_PES_HEADER_LENGTH));

        if (i == acq_num || offset >= hw_seop_addr) {
            *acqed_num = i;
            rbuf->acquire_ok_cnt++;
            ret = HI_SUCCESS;
            HI_DBG_DEMUX("***reach the sop address, pes_len[%#x], offset[%#x], hw_seop_addr[%#x]\n",
                         pes_len, offset, hw_seop_addr);
            goto out;
        }

        do {
            ret = dmx_parse_pes_header(rbuf->buf_ker_addr + offset, DMX_PES_HEADER_LENGTH,
                                       &pes_head_len, &pes_pload_len, &pes_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("pes head parse failed!\n");
                ret = HI_ERR_DMX_NOAVAILABLE_DATA;
                goto out;
            }

            HI_DBG_DEMUX("pes_len[%#x], pes_pload_len[%#x], pes_head_len[%#x]\n", pes_len, pes_pload_len, pes_head_len);
            play_fct_buf[i].buf_handle = rbuf->buf_handle;
            play_fct_buf[i].phy_addr = rbuf->buf_phy_addr + offset;
            play_fct_buf[i].offset = offset;
            play_fct_buf[i].data = rbuf->buf_usr_addr + offset;
            play_fct_buf[i].length = pes_len;
            play_fct_buf[i].data_type = DMX_DATA_WHOLE;

            i++;
            offset += pes_len;
        } while ((offset < hw_seop_addr) && (i < acq_num) && (hw_seop_addr - offset >= DMX_PES_HEADER_LENGTH));
    } else {
        *acqed_num = 0;
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    *acqed_num = i;
    HI_DBG_DEMUX("hw_read[0x%x], hw_write[0x%x], hw_end_addr[0x%x], hw_seop_addr[0x%x]\n",
                 hw_read, hw_write, hw_end_addr, hw_seop_addr);

    /* acquire ok cnt++ */
    rbuf->acquire_ok_cnt++;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_buf_acquire_pes_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
    dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_acquire_pes_impl(rbuf, acq_num, time_out, acqed_num, play_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_release_pes_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;
    hi_s32 i = 0;
    hi_u32 rel_len = 0;
    hi_u8  *usr_addr = HI_NULL;

    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    for (i = 0; i < rel_num; i++) {
        usr_addr = (hi_u8 *)((hi_ulong)play_fct_buf[i].data);
        rel_len = play_fct_buf[i].length;

        if ((rel_len > rbuf->buf_size) || (usr_addr < rbuf->buf_usr_addr || \
             usr_addr >= rbuf->buf_usr_addr + rbuf->buf_size) || (usr_addr + rel_len < usr_addr) || \
            (usr_addr + rel_len > rbuf->buf_usr_addr + rbuf->buf_size + DMX_PES_PACKAGE_MAX_LEN)) {
            HI_ERR_DEMUX("bufid[%d], usr_addr[0x%x], rel_len[0x%x], buf_usr_phy[0x%x], buf_size[0x%x]!\n",\
                         rbuf->base.id, usr_addr, rel_len, rbuf->buf_usr_addr, rbuf->buf_size);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        rbuf->sw_read = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);
        rbuf->sw_read += rel_len;

        if (rbuf->sw_read == rbuf->buf_size) { /* pes body rool loop */
            rbuf->sw_read = 0;
            HI_DBG_DEMUX("rool loop1 release!\n");
        } else if (play_fct_buf[i].data_type == DMX_DATA_TAIL && rbuf->sw_read > rbuf->buf_size \
                   && rbuf->sw_read <= rbuf->buf_size + DMX_PES_PACKAGE_MAX_LEN) { /* pes head rool loop */
            rbuf->sw_read = rbuf->sw_read - rbuf->buf_size;
            HI_DBG_DEMUX("rool loop2 release!\n");
        }

        if (rbuf->sw_read >= rbuf->buf_size) {
            HI_ERR_DEMUX("rbuf->sw_read[%#x], rbuf->buf_size[%#x], data_type[%#x], rel_len[%#x]!\n",\
                         rbuf->sw_read, rbuf->buf_size, play_fct_buf[i].data_type, rel_len);
            WARN_ON(rbuf->sw_read >= rbuf->buf_size);
        }

        /* update the read idx */
        if (rbuf->secure_mode != DMX_SECURE_TEE) {
            dmx_hal_buf_set_read_idx(mgmt, rbuf->base.id, rbuf->sw_read);
        } else {
            ret = dmx_tee_update_play_buf_read_idx(rbuf->base.id, DMX_PLAY_TYPE_PES, rbuf->sw_read);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("update secure read idx failed, ret[0x%x]!\n", ret);
                goto out;
            }
        }
    }

    /* check the buffer ap status */
    dmx_cls_buf_ap_status(mgmt, rbuf);

    /* release cnt++ */
    rbuf->release_cnt++;

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_buf_release_pes_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_release_pes_impl(rbuf, rel_num, play_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

/* mq buffer len is fixed 64 bytes */
static hi_s32 dmx_parse_mq_buf(hi_u8 *mq_buf, hi_u32 mq_len, hi_u64 *buf_addr, hi_bool *pes_flag, hi_u32 *pes_head_len)
{
    if (mq_buf == HI_NULL || buf_addr == HI_NULL || pes_flag == HI_NULL ||
        pes_head_len == HI_NULL || mq_len > MQ_DEFAULT_RAM_DSC_SIZE) {
        HI_ERR_DEMUX("invalid parameter mq_buf[%#x], buf_addr[%#x], pes_flag[%#x], pes_head_len[%#x], mq_len[%#]\n",
                     mq_buf, buf_addr, pes_flag, pes_head_len, mq_len);
        return HI_ERR_DMX_INVALID_PARA;
    }
    /* 36 bits pes address, shift 8, 16, 24 bits */
    *buf_addr = mq_buf[0] | (mq_buf[1] << 8) | (mq_buf[0x2] << 16) | (mq_buf[0x3] << 24) | (mq_buf[0x4] & 0xF);
    *pes_flag = mq_buf[0x7] & 0x20;  /* bit 61 */
    *pes_head_len = mq_buf[0x8];

    return HI_SUCCESS;
}

/* av_pes head parse */
static hi_s32 dmx_parse_av_pes_header(
    hi_u8          *parser_addr,
    hi_u32          parser_len,
    hi_u32         *pes_header_len,
    hi_u32         *pes_pay_load_len,
    hi_s64         *last_pts,
    struct dmx_disp_control *p_disp_controller
)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 stream_id = 0;
    hi_u32 ptsflag = 0;
    hi_s64 tmp_pts = 0;

    /* the correct first three bytes of pes head is 00 00 01 */
    if ((parser_addr[0] != 0x00) || (parser_addr[1] != 0x00) || (parser_addr[0x2] != 0x01)) {
        HI_WARN_DEMUX("error pes start code = %x %x %x\n", parser_addr[0], parser_addr[1], parser_addr[0x2]);
        ret = HI_FAILURE;
        goto out;
    }

    /* get stream_id */
    stream_id = parser_addr[0x3];

    /* del pes header according to 13818-1 */
    if ((stream_id != 0xbc) /* 1011 1100  1   program_stream_map */
            && (stream_id != 0xbe) /* 1011 1110       padding_stream */
            && (stream_id != 0xbf) /* 1011 1111   3   private_stream_2 */
            && (stream_id != 0xf0) /* 1111 0000   3   ecm_stream */
            && (stream_id != 0xf1) /* 1111 0001   3   emm_stream */
            && (stream_id != 0xff) /* 1111 1111   4   program_stream_directory */
            && (stream_id != 0xf2) /*
                                    * 1111 0010   5   ITU-T rec. H.222.0 | ISO/IEC 13818-1 annex A or
                                    * ISO/IEC 13818-6_dsmcc_stream
                                    */
            && (stream_id != 0xf8)) { /* 1111 1000  6   ITU-T rec. H.222.1 type E */
        /*      if (PTS_DTS_flags =='10' ) {
            '0010'  4   bslbf
            PTS [32..30]    3   bslbf
            marker_bit  1   bslbf
            PTS [29..15]    15  bslbf
            marker_bit  1   bslbf
            PTS [14..0] 15  bslbf
            marker_bit  1   bslbf
        }
                if (PTS_DTS_flags =='11' )  {
            '0011'
            PTS [32..30]
            marker_bit
            PTS [29..15]
            marker_bit
            PTS [14..0]
            marker_bit
            '0001'
            DTS [32..30]
            marker_bit
            DTS [29..15]
            marker_bit
            DTS [14..0]
            marker_bit
        }
         */
        if (parser_len < DMX_PES_HEADER_LENGTH) {
            HI_ERR_DEMUX("invalid pes len[0x%x]\n", parser_len);
            ret = HI_FAILURE;
        }

        ptsflag = (parser_addr[7] & 0x80) >> 7; /* index 7, 7 bits */

        *pes_header_len = DMX_PES_HEADER_LENGTH + parser_addr[0x8];

        *pes_pay_load_len = ((parser_addr[0x4] << 8) | parser_addr[0x5]) - *pes_header_len + 0x6; /* 8 bits */

        if (ptsflag && (parser_len > *pes_header_len)) {
            hi_u64 ptslow = (((parser_addr[9] & 0x06) >> 1) << 30) /* index 9 , 30 bits */
                            | ((parser_addr[10]) << 22) /* index 10 , 22 bits */
                            | (((parser_addr[11] & 0xfe) >> 1) << 15) /* index 11 , 15 bits */
                            | ((parser_addr[12]) << 7) /* index 12 , 7 bits */
                            | (((parser_addr[13] & 0xfe)) >> 1); /* index 13 */

            /* set the high 32 bits zero,to prevent data errors caused by sign bit extension. */
            ptslow &= 0xffffffff;

            if (parser_addr[9] & 0x08) { /* index 9 */
                tmp_pts = (ptslow + (1ULL << 32)) * MS_2_US; /* 32 bits */
                do_div(tmp_pts, 90); /* trans 90khz count to us. */
            } else {
                ptslow = ptslow * MS_2_US;
                tmp_pts = (ptslow / 90); /* 90 khz */
            }

            *last_pts = tmp_pts;
        } else {
            *last_pts = DMX_INVALID_PTS;
        }
        /* for dispcontrol */
        if ((stream_id == 0xee) && (parser_len > 24)) { /* min pes len 24 */
            if ((parser_addr[13] == 0x70) && (parser_addr[14] == 0x76) && /* index 13&14 */
                (parser_addr[15] == 0x72) && (parser_addr[16] == 0x63)) { /* index 15&16 */
                if ((parser_addr[17] == 0x75) && (parser_addr[18] == 0x72) && /* index 17&18 */
                    (parser_addr[19] == 0x74) && (parser_addr[20] == 0x6d)) { /* index 19&20 */
                    p_disp_controller->disp_time = *(hi_u32 *)&parser_addr[21]; /* index 21 */
                    p_disp_controller->disp_enable_flag = *(hi_u32 *)&parser_addr[25]; /* index 25 */
                    p_disp_controller->disp_frame_distance = *(hi_u32 *)&parser_addr[29]; /* index 29 */
                    p_disp_controller->distance_before_first_frame = *(hi_u32 *)&parser_addr[33]; /* index 33 */
                    p_disp_controller->gop_num = *(hi_u32 *)&parser_addr[37]; /* index 37 */
                    *pes_header_len = 184; /* pes len 184 */
                    return DMX_PVR_DISP_CTRL;
                }
            }
        }

        ret = HI_SUCCESS;
    } else { /* normal pes data for pes channel accdg to 13818 -1 */
        if (parser_len >= 6) { /* pes len 6 */
            /* 3(packet_start_code_prefix) + 1(stream_id) +2(pes_packet_length) */
            *pes_header_len = 6; /* pes len 6 */
            /* the value of pes_packet_length */
            *pes_pay_load_len = ((parser_addr[4] << 8) | parser_addr[5]); /* index 4&5, 8 bits */
            HI_WARN_DEMUX("stream_id:[0x%x] only for the pes channel!\n", stream_id);
            ret = HI_SUCCESS;
        } else {
            HI_ERR_DEMUX("invalid pes len[0x%x]\n", parser_len);
            ret = HI_FAILURE;
        }
    }
out:
    return ret;
}

static hi_s32 __dmx_r_buf_parse_avpes_buf(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf,
    hi_s64 *parse_pts, hi_u32 *parse_pes_head_len, hi_u64 *buf_addr)
{
    hi_s32 ret;
    hi_u32 mq_cur_write_idx;
    hi_u32 mq_cur_read_idx;
    hi_u32 pes_flag               = 0;
    hi_u32 pes_pkt_len            = 0;

    struct dmx_disp_control disp_controller;
    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;

    mq_cur_read_idx = avpes_buf->buf_avpes.last_mq_dsc_read_idx;
    mq_cur_write_idx = dmx_hal_mq_get_cur_write_idx(mgmt, rbuf->base.id);

    ret = dmx_parse_mq_buf(avpes_buf->buf_avpes.mq_buf_ker_addr + mq_cur_read_idx * MQ_DEFAULT_RAM_DSC_SIZE,
                           MQ_DEFAULT_RAM_DSC_SIZE, buf_addr, &pes_flag, parse_pes_head_len);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("parse mq failed, ret[%#x]!\n", ret);
        return HI_ERR_DMX_INVALID_PARA;
    }

    /* begin, we will parse the pes head */
    if (pes_flag) {
        ret = dmx_parse_av_pes_header(avpes_buf->buf_avpes.mq_buf_ker_addr +
                                      mq_cur_read_idx * MQ_DEFAULT_RAM_DSC_SIZE + PES_OFFSET_IN_MQ,
                                      MQ_DEFAULT_RAM_DSC_SIZE - PES_OFFSET_IN_MQ, parse_pes_head_len,
                                      &pes_pkt_len, parse_pts, &disp_controller);
        if (ret == DMX_PVR_DISP_CTRL) {
            memcpy(&avpes_buf->buf_avpes.disp_contrl, &disp_controller, sizeof(struct dmx_disp_control));
        }

        if (!avpes_buf->buf_avpes.first_pes_flag) {
            avpes_buf->buf_avpes.last_pts          = *parse_pts;
            avpes_buf->buf_avpes.last_pes_head_len = *parse_pes_head_len;
            avpes_buf->buf_avpes.first_pes_flag    = HI_TRUE;
        }
    } else {
        HI_DBG_DEMUX("pes flag invalid, means not whole pes, parse_pes_head_len[%#x]\n", parse_pes_head_len);
    }

    if (*buf_addr == rbuf->last_addr ||
        *buf_addr < rbuf->buf_phy_addr ||
        *buf_addr > rbuf->buf_phy_addr + rbuf->buf_size) {
        HI_DBG_DEMUX("maybe the beginning or two same mq descript.buf_addr[%#x], rbuf->last_addr[%#x]!\n",
                     *buf_addr, rbuf->last_addr);
        /* roll loop */
        mq_cur_read_idx = (mq_cur_read_idx + 1 >= avpes_buf->buf_avpes.mq_dsc_depth) ? 0 : mq_cur_read_idx + 1;
        avpes_buf->buf_avpes.last_mq_dsc_read_idx = mq_cur_read_idx;
        dmx_hal_mq_set_read_idx(mgmt, rbuf->base.id, mq_cur_read_idx);
        return HI_ERR_DMX_NOAVAILABLE_DATA;
    }

    return HI_SUCCESS;
}

static hi_s32 save_es_block_to_list(struct dmx_r_avpes_buf *avpes_buf, dmx_buffer *play_buf, hi_u32 pes_head_len)
{
    struct es_block_info_node *es_block_info_ptr = HI_NULL;

    /* add the es block nod into the es block list */
    es_block_info_ptr = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct es_block_info_node), GFP_KERNEL);
    DMX_NULL_POINTER_RETURN(es_block_info_ptr);

    es_block_info_ptr->buf_phy_addr = play_buf[0].phy_addr;
    es_block_info_ptr->es_data_leng = play_buf[0].length;
    es_block_info_ptr->pes_head_len = pes_head_len;
    list_add_tail(&es_block_info_ptr->node, &avpes_buf->buf_avpes.es_block_info_head);

    return HI_SUCCESS;
}

static hi_s32 __dmx_r_buf_calc_play_buf(hi_u64 buf_addr, struct dmx_r_buf *rbuf,
    hi_u32 *acqed_num, dmx_buffer *play_buf)
{
    hi_s32 ret;
    hi_u32 pes_head_len = 0;
    hi_u32 top_pes_head_len;
    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;

    if (rbuf->rool_flag) {
        play_buf[0].phy_addr = rbuf->buf_phy_addr;
        play_buf[0].length   = rbuf->last_addr - rbuf->buf_phy_addr;
        play_buf[0].pts = DMX_INVALID_PTS;
        play_buf[0].data_type = DMX_DATA_TAIL;

        rbuf->rool_flag = HI_FALSE;
    } else {
        if (likely(buf_addr > rbuf->last_addr)) {
            play_buf[0].phy_addr = rbuf->last_addr + avpes_buf->buf_avpes.last_pes_head_len;
            play_buf[0].length   = buf_addr - play_buf[0].phy_addr;
            play_buf[0].data_type = DMX_DATA_WHOLE;
        } else {
            if (rbuf->last_addr + avpes_buf->buf_avpes.last_pes_head_len >= rbuf->buf_phy_addr + rbuf->buf_size) {
                /* top_pes_head_len here only used to calc the data offset */
                top_pes_head_len = rbuf->last_addr + avpes_buf->buf_avpes.last_pes_head_len -
                    rbuf->buf_phy_addr - rbuf->buf_size;
                play_buf[0].length  = buf_addr - rbuf->buf_phy_addr - top_pes_head_len;
                play_buf[0].phy_addr = rbuf->buf_phy_addr + top_pes_head_len;
                play_buf[0].data_type = DMX_DATA_WHOLE;
            } else {
                play_buf[0].phy_addr = rbuf->last_addr + avpes_buf->buf_avpes.last_pes_head_len;
                play_buf[0].length   = rbuf->buf_phy_addr + rbuf->buf_size - play_buf[0].phy_addr;
                play_buf[0].data_type = DMX_DATA_HEAD;

                rbuf->rool_flag = HI_TRUE;
            }
        }
        play_buf[0].pts = avpes_buf->buf_avpes.last_pts;
        pes_head_len = avpes_buf->buf_avpes.last_pes_head_len;
    }

    play_buf[0].buf_handle = rbuf->buf_handle;
    play_buf[0].offset   = play_buf[0].phy_addr - rbuf->buf_phy_addr;
    play_buf[0].data = rbuf->buf_usr_addr + play_buf[0].offset;
    play_buf[0].ker_vir_addr = (hi_u64)(rbuf->buf_ker_addr + play_buf[0].offset);

    play_buf[0].disp_time                   = avpes_buf->buf_avpes.disp_contrl.disp_time;
    play_buf[0].disp_enable_flag            = avpes_buf->buf_avpes.disp_contrl.disp_enable_flag;
    play_buf[0].disp_frame_distance         = avpes_buf->buf_avpes.disp_contrl.disp_frame_distance;
    play_buf[0].distance_before_first_frame = avpes_buf->buf_avpes.disp_contrl.distance_before_first_frame;
    play_buf[0].gop_num                     = avpes_buf->buf_avpes.disp_contrl.gop_num;

    ret = save_es_block_to_list(avpes_buf, play_buf, pes_head_len);
    if (ret == HI_SUCCESS) {
        *acqed_num = 1;
    }

    return ret;
}

static hi_s32 _dmx_r_buf_acquire_av_pes_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_buf)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 queue_len;
    hi_u32 mq_cur_write_idx;
    hi_u32 mq_cur_read_idx;
    hi_u64 buf_addr               = 0;
    hi_u32 parse_pes_head_len     = 0;
    hi_s64 parse_pts              = DMX_INVALID_PTS;

    struct dmx_mgmt *mgmt = rbuf->base.mgmt;
    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;

    DMX_NULL_POINTER_RETURN(mgmt);

    /* acquire cnt++ */
    rbuf->acquire_cnt++;

    if (rbuf->rool_flag) {
        ret = __dmx_r_buf_calc_play_buf(0, rbuf, acqed_num, play_buf);
        if (ret != HI_SUCCESS) {
            goto out;
        }
        goto acquire_success;
    }

    mq_cur_read_idx = avpes_buf->buf_avpes.last_mq_dsc_read_idx;
    mq_cur_write_idx = dmx_hal_mq_get_cur_write_idx(mgmt, rbuf->base.id);
    queue_len = dmx_get_queue_lenth(mq_cur_read_idx, mq_cur_write_idx, avpes_buf->buf_avpes.mq_dsc_depth);
    if (queue_len >= 1) {
        ret = __dmx_r_buf_parse_avpes_buf(mgmt, rbuf, &parse_pts, &parse_pes_head_len, &buf_addr);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        ret = __dmx_r_buf_calc_play_buf(buf_addr, rbuf, acqed_num, play_buf);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        /* mq roll loop */
        mq_cur_read_idx = (mq_cur_read_idx + 1 >= avpes_buf->buf_avpes.mq_dsc_depth) ? 0 : mq_cur_read_idx + 1;
        avpes_buf->buf_avpes.last_mq_dsc_read_idx = mq_cur_read_idx;
        dmx_hal_mq_set_read_idx(mgmt, rbuf->base.id, mq_cur_read_idx);

        /* record last pes information */
        rbuf->last_addr = buf_addr;
        avpes_buf->buf_avpes.last_pts = parse_pts;
        avpes_buf->buf_avpes.last_pes_head_len = parse_pes_head_len;
    } else {
        HI_WARN_DEMUX("there is no available data!\n");
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }
acquire_success:
    /* acquire ok cnt++ */
    rbuf->acquire_ok_cnt++;
out:
    return ret;
}

static hi_s32 dmx_r_buf_acquire_av_pes_impl(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_acquire_av_pes_impl(rbuf, acq_num, time_out, acqed_num, play_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_release_av_pes_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_buf)
{
    hi_s32 ret = HI_FAILURE;

    hi_s32 i = 0;

    hi_u32 es_len = 0;
    hi_u32 real_rel_len = 0;
    hi_u64 phy_addr = 0;

    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;
    struct es_block_info_node *es_block_p = HI_NULL;
    struct dmx_mgmt *mgmt = HI_NULL;

    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    for (i = 0; i < rel_num; i++) {
        phy_addr = play_buf[i].offset + rbuf->buf_phy_addr;
        es_len = play_buf[i].length;

        /* get pes head len from local list */
        es_block_p = list_first_entry(&avpes_buf->buf_avpes.es_block_info_head, struct es_block_info_node, node);
        DMX_NULL_POINTER_RETURN(es_block_p);

        if (unlikely(phy_addr != es_block_p->buf_phy_addr ||
                     es_len != es_block_p->es_data_leng ||
                     phy_addr != rbuf->buf_phy_addr + (rbuf->sw_read + es_block_p->pes_head_len) % rbuf->buf_size)) {
            HI_ERR_DEMUX("bufid[%d], phy_addr[0x%llx], len[0x%x], buf_phy_addr[0x%llx], swread[0x%x]"
                         "local addr[0x%llx], pes_head_len[0x%x], local len[0x%x]!\n",
                         rbuf->base.id, phy_addr, es_len, rbuf->buf_phy_addr, rbuf->sw_read,
                         es_block_p->buf_phy_addr, es_block_p->pes_head_len, es_block_p->es_data_leng);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        /* begin update the read pointer */
        rbuf->sw_read = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);

        /* actual release len need add the pes head len */
        real_rel_len = es_len + es_block_p->pes_head_len;

        rbuf->sw_read += real_rel_len;

        /* include the buffer rewind */
        rbuf->sw_read = rbuf->sw_read % rbuf->buf_size;

        /* release the local list entry */
        list_del(&es_block_p->node);
        HI_KFREE(HI_ID_DEMUX, es_block_p);
        es_block_p = HI_NULL;

        if (rbuf->secure_mode != DMX_SECURE_TEE) {
            dmx_hal_buf_set_read_idx(mgmt, rbuf->base.id, rbuf->sw_read);
        } else {
            ret = dmx_tee_update_play_buf_read_idx(rbuf->base.id, DMX_PLAY_TYPE_VID, rbuf->sw_read);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("update secure read idx failed, ret[0x%x]!\n", ret);
                goto out;
            }
        }
    }

    /* check the buffer ap status */
    dmx_cls_buf_ap_status(mgmt, rbuf);

    /* release cnt++ */
    rbuf->release_cnt++;

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_buf_release_av_pes_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *play_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_release_av_pes_impl(rbuf, rel_num, play_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

#if (DMX_REC_BUF_GAP_EXIST == 1)

/* create a empty ts packet with pid 0x1fff */
static void create_ts_packet(hi_u8 *buf)
{
    memset(buf, 0x0, DMX_TS_PKT_SIZE);
    *(buf++) = TS_SYNC_BYTE; /* Set ts sync byte to 0x47 */
    *(buf++) = 0x1f; /* Set the upper five digits of the PID to 0x1f */
    *(buf++) = 0xff; /* Set the lower eight bits of the PID to 0xff */
}

/* create a tts packet with a timestamp of 0 and a pid of 0x1fff */
static void create_tts_packet(hi_u8 *buf)
{
    memset(buf, 0x0, DMX_TTS_PKT_SIZE);
    buf += TTS_SYNC_BYTE_OFFSET;
    *(buf++) = TS_SYNC_BYTE; /* Set ts sync byte to 0x47 */
    *(buf++) = 0x1f; /* Set the upper five digits of the PID to 0x1f */
    *(buf++) = 0xff; /* Set the lower eight bits of the PID to 0xff */
}

static hi_void _dmx_r_buf_fix_gap(struct dmx_r_buf *rbuf, hi_u32 *end_addr)
{
    /* Since there is a gap at the end of the recording buffer, when the buffer wrap is detected,
       fill in one or two(ts:2, tts:1) empty packets with pid 0x1fff at the end of the buffer,
       and the 47K alignment is sufficient. */
    if (rbuf->pkt_size == DMX_TS_PKT_SIZE) {
        create_ts_packet(rbuf->buf_ker_addr + *end_addr);
        *end_addr += DMX_TS_PKT_SIZE;
        create_ts_packet(rbuf->buf_ker_addr + *end_addr);
        *end_addr += DMX_TS_PKT_SIZE;
    } else if (rbuf->pkt_size == DMX_TTS_PKT_SIZE) {
        create_tts_packet(rbuf->buf_ker_addr + *end_addr);
        *end_addr += DMX_TTS_PKT_SIZE;
    } else {
        /* do nothing */
    }
}
#endif

static hi_s32 _dmx_r_buf_acquire_rec_impl(struct dmx_r_buf *rbuf, hi_u32 req_len, hi_u32 time_out,
    hi_u32 *reqd_len, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 end_addr;
    hi_u32 hw_write;
    hi_u32 hw_read;
    hi_s32 left_time = time_out;
    hi_u32 ready_len;
    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    rbuf->acquire_cnt++;

    rec_fct_buf->buf_handle = rbuf->buf_handle;
    rec_fct_buf->phy_addr = rbuf->buf_phy_addr + rbuf->last_read;
    rec_fct_buf->ker_vir_addr = (hi_u64)(rbuf->buf_ker_addr + rbuf->last_read);
    rec_fct_buf->offset = rec_fct_buf->phy_addr - rbuf->buf_phy_addr;
    rec_fct_buf->data   = rbuf->buf_usr_addr + rbuf->last_read;

    do {
        hw_read  = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);
        /* attention: before read 'write ponint, must read 'end'  point first */
        hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
        end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);

        /* get the data */
        WARN_ON(hw_read != rbuf->sw_read);
        WARN_ON(rbuf->last_read > rbuf->buf_size);

        if (likely(hw_read < hw_write)) { /* not rool back */
            ready_len = hw_write - rbuf->last_read;
            if (ready_len >= req_len) {
                rec_fct_buf->length = req_len;
                rbuf->last_read += rec_fct_buf->length;
                ret = HI_SUCCESS;
                break;
            }
        } else if (hw_read > hw_write) { /* rool back */
            if (rbuf->rool_flag == HI_FALSE) {
#if (DMX_REC_BUF_GAP_EXIST == 1)
                _dmx_r_buf_fix_gap(rbuf, &end_addr);
#endif
                rec_fct_buf->length = end_addr - rbuf->last_read;
                rbuf->last_read = 0;
                rbuf->rool_flag = HI_TRUE;
                rbuf->last_end   = end_addr;
                HI_DBG_DEMUX(" ******set the end addr[0x%x], length[0x%x]\n",
                             rbuf->last_end, rec_fct_buf->length);
                ret = HI_SUCCESS;
                break;
            } else {
                ready_len = hw_write - rbuf->last_read;
                if (ready_len >= req_len) {
                    rec_fct_buf->length = req_len;
                    rbuf->last_read += rec_fct_buf->length;
                    HI_DBG_DEMUX("don't come here[0x%x], length[0x%x]\n",
                                 rbuf->last_end, rec_fct_buf->length);
                    ret = HI_SUCCESS;
                    break;
                }
            }
        }

        if (left_time <= 0) {
            ret = (time_out == 0) ? HI_ERR_DMX_NOAVAILABLE_DATA : HI_ERR_DMX_TIMEOUT;
            break;
        }
        /* clear the buf condition */
        osal_mutex_lock(&rbuf->interrupt_lock);
        rbuf->wait_cond = HI_FALSE;
        osal_mutex_unlock(&rbuf->interrupt_lock);

        left_time = wait_event_interruptible_timeout(rbuf->wait_queue, rbuf->wait_cond,
            osal_msecs_to_jiffies(left_time));
    } while (1);

    if (ret == HI_SUCCESS) {
        rbuf->acquire_ok_cnt++;
    }

    return ret;
}

static hi_s32 dmx_r_buf_acquire_rec_impl(struct dmx_r_buf *rbuf, hi_u32 req_len, hi_u32 time_out,
    hi_u32 *reqd_len, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_acquire_rec_impl(rbuf, req_len, time_out, reqd_len, rec_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_release_rec_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret = HI_FAILURE;

    hi_u32 rel_len = rec_fct_buf->length;
    hi_u64 phy_addr = rec_fct_buf->offset + rbuf->buf_phy_addr;
    struct dmx_mgmt *mgmt = HI_NULL;

    DMX_NULL_POINTER_RETURN(rbuf);
    mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    if ((rel_len > rbuf->buf_size) ||
            (phy_addr < rbuf->buf_phy_addr || phy_addr >= rbuf->buf_phy_addr + rbuf->buf_size) ||
            (phy_addr + rel_len < phy_addr) || (phy_addr + rel_len > rbuf->buf_phy_addr + rbuf->buf_size)) {
        HI_ERR_DEMUX("invalid parameter, phy_addr[0x%x], rel_len[0x%x], buf_phy[0x%x], buf_size[0x%x]!\n",
                     phy_addr, rel_len, rbuf->buf_phy_addr, rbuf->buf_size);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    rbuf->sw_read = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);

    rbuf->sw_read += rel_len;

    /* if the release the data block is the last block of the buffer */
    if (unlikely(rbuf->sw_read == rbuf->last_end && rbuf->rool_flag == HI_TRUE)) {
        rbuf->sw_read = 0;
        rbuf->last_end = 0;
        rbuf->rool_flag = HI_FALSE;
    }

    dmx_hal_buf_set_read_idx(mgmt, rbuf->base.id, rbuf->sw_read);

    /* check the buffer ap status */
    dmx_cls_buf_ap_status(mgmt, rbuf);

    /* release cnt++ */
    rbuf->release_cnt++;

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_buf_release_rec_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);
    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_release_rec_impl(rbuf, rel_num, rec_fct_buf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_acquire_scd_impl(struct dmx_r_buf *rbuf, hi_u32 req_num, hi_u32 time_out,
    hi_u32 *reqed_num, dmx_index_scd **scd_index)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 end_addr;
    hi_u32 hw_write;
    hi_u32 hw_read;
    hi_u32 index_num = 0;

    struct dmx_mgmt *mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    rbuf->acquire_cnt++;
    hw_read  = dmx_hal_buf_get_cur_read_idx(mgmt, rbuf->base.id);
    /* attention: before read 'write ponint, must read 'end'  point first */
    hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
    end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);

    /* calculate the index count */
    if (likely(hw_read < hw_write)) { /* not rool back */
        if ((hw_write - hw_read) > req_num * sizeof(dmx_index_scd)) {
            index_num = req_num;
        } else if ((hw_write - hw_read) > sizeof(dmx_index_scd)) {
            index_num = (hw_write - hw_read) / sizeof(dmx_index_scd);
        } else {
            index_num = 0;
            ret = HI_ERR_DMX_NOAVAILABLE_DATA;
            goto out;
        }
    } else if (hw_read > hw_write) { /* rool back */
        if ((end_addr - hw_read) > req_num * sizeof(dmx_index_scd)) {
            index_num = req_num;
            /* update the sw_read */
        } else { /* if (end_addr - hw_read<dmx_rec_scd_index_size), we just reset the hw_read and sw_read */
            index_num = (end_addr - hw_read) / sizeof(dmx_index_scd);
        }
        /* set the roolback flag */
        rbuf->rool_flag = HI_TRUE;
    } else {
        HI_WARN_DEMUX("no available index data, buf_id[%#x] hw_read[0x%x], hw_write[0x%x]," \
                      "end_addr[0x%x], sw_read[%#x], index_num[%#x],\n",
                      rbuf->base.id, hw_read, hw_write, end_addr, rbuf->sw_read, index_num);
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    *scd_index = (dmx_index_scd *)(rbuf->buf_ker_addr + hw_read);

    *reqed_num = index_num;

    rbuf->acquire_ok_cnt++;

    ret = HI_SUCCESS;
out:

    return ret;
}

static hi_s32 dmx_r_buf_acquire_scd_impl(struct dmx_r_buf *rbuf, hi_u32 req_num, hi_u32 time_out,
    hi_u32 *reqed_num, dmx_index_scd **scd_data)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_acquire_scd_impl(rbuf, req_num, time_out, reqed_num, scd_data);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_release_scd_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, const dmx_index_scd *scd_data)
{
    hi_u32 end_addr;
    struct dmx_mgmt *mgmt = rbuf->base.mgmt;

    DMX_NULL_POINTER_RETURN(mgmt);

    end_addr = dmx_hal_buf_get_cur_end_idx(mgmt, rbuf->base.id);

    rbuf->sw_read += rel_num * sizeof(dmx_index_scd);

    if (rbuf->rool_flag == HI_TRUE) {
        if (rbuf->sw_read >= end_addr) {
            rbuf->sw_read = 0;
            rbuf->rool_flag = HI_FALSE;
        }
    }
    /* update the hw_read */
    dmx_hal_buf_set_read_idx(mgmt, rbuf->base.id, rbuf->sw_read);

    /* check the buffer ap status */
    dmx_cls_buf_ap_status(mgmt, rbuf);

    rbuf->release_cnt++;

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_release_scd_impl(struct dmx_r_buf *rbuf, hi_u32 rel_num, const dmx_index_scd *scd_data)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_release_scd_impl(rbuf, rel_num, scd_data);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_close_impl(struct dmx_r_buf *rbuf)
{
    hi_u32 port_offset = 0;
    hi_bool secure;

    /* close the buffer configure */
    dmx_hal_buf_close(rbuf->base.mgmt, rbuf->base.id);

    /* force clear the output buffer ap status */
    if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
        port_offset = rbuf->port - DMX_RAM_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_ram_port_cls_ap_status(rbuf->base.mgmt, port_offset);
    } else if (rbuf->port >= DMX_TSIO_PORT_0 && rbuf->port < DMX_TSIO_PORT_MAX) {
        port_offset = rbuf->port - DMX_TSIO_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_buf_clr_tsio_ap(rbuf->base.mgmt, port_offset);
    }

    secure = (rbuf->secure_mode == DMX_SECURE_TEE) ? HI_TRUE : HI_FALSE;
    dmx_hal_buf_de_config(rbuf->base.mgmt, rbuf->base.id, 0, 0, secure);

    /* general */
    rbuf->sw_read               = 0;
    rbuf->last_addr             = rbuf->buf_phy_addr;
    rbuf->last_read             = 0;
    rbuf->last_end              = 0;
    rbuf->rool_flag             = HI_FALSE;
    rbuf->eos_flag              = HI_FALSE;
    rbuf->ovfl_flag             = HI_FALSE;
    rbuf->eos_cnt               = 0;
    rbuf->eos_record_idx        = 0;
    rbuf->ovfl_cnt              = 0;

    rbuf->acquire_cnt = 0;
    rbuf->acquire_ok_cnt = 0;
    rbuf->release_cnt = 0;

    /* clear the output buffer */
    dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

    /* clear the mmu cache */
    dmx_hal_buf_clr_mmu_cache(rbuf->base.mgmt, rbuf->base.id);

    cancel_work_sync(&rbuf->buf_worker);
    cancel_work_sync(&rbuf->buf_ovfl_worker);
    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_close_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_close_impl(rbuf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    struct dmx_r_raw_pid_ch *raw_pid_ch = HI_NULL;
    struct dmx_r_play_fct *play_fct = (struct dmx_r_play_fct *)obj;

    rbuf->sw_read = 0;
    raw_pid_ch = _get_raw_pid_obj_for_play(play_fct->rpid_ch, play_fct);
    if (raw_pid_ch) {
        dmx_hal_pid_tab_flt_dis(rbuf->base.mgmt, raw_pid_ch->raw_pid_chan_id);

        dmx_hal_band_clear_chan(rbuf->base.mgmt, play_fct->rpid_ch->rband->base.id);

        dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

        dmx_dofunc_no_return(_dmx_r_buf_close_impl(rbuf));

        dmx_dofunc_no_return(_dmx_r_buf_open_impl(rbuf));

        dmx_hal_pid_tab_flt_en(rbuf->base.mgmt, raw_pid_ch->raw_pid_chan_id);
    } else {
        HI_ERR_DEMUX("null pointer\n");
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_reset_impl(rbuf, obj);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

/* avpes sub buf ops */
static hi_s32 _dmx_r_buf_avpes_open_impl(struct dmx_r_buf *rbuf)
{
    hi_bool secure;
    hi_u32 port_offset;
    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;

    secure = (rbuf->secure_mode == DMX_SECURE_TEE) ? HI_TRUE : HI_FALSE;

    rbuf->acquire_cnt = 0;
    rbuf->acquire_ok_cnt = 0;
    rbuf->release_cnt = 0;

    /* general */
    rbuf->sw_read               = 0;
    rbuf->last_addr             = rbuf->buf_phy_addr;
    rbuf->last_read             = 0;
    rbuf->last_end              = 0;
    rbuf->rool_flag             = HI_FALSE;
    rbuf->eos_flag              = HI_FALSE;
    rbuf->ovfl_flag             = HI_FALSE;
    rbuf->eos_cnt               = 0;
    rbuf->eos_record_idx        = 0;
    rbuf->ovfl_cnt              = 0;

    /* force clear the output buffer ap status */
    if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
        port_offset = rbuf->port - DMX_RAM_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_ram_port_cls_ap_status(rbuf->base.mgmt, port_offset);
    }

    /* clear buf mmu cache */
    dmx_hal_buf_clr_mmu_cache(rbuf->base.mgmt, rbuf->base.id);

    if (avpes_buf->buf_avpes.mq_use_flag) {
        dmx_hal_mq_config(rbuf->base.mgmt, rbuf->base.id, avpes_buf->buf_avpes.mq_buf_phy_addr,
                          avpes_buf->buf_avpes.mq_dsc_depth);
        dmx_hal_buf_en_mq_pes(rbuf->base.mgmt, rbuf->base.id);

        /* for avpes playfct */
        avpes_buf->buf_avpes.last_pts  = DMX_INVALID_PTS;
        avpes_buf->buf_avpes.last_pes_head_len = 0;
        avpes_buf->buf_avpes.first_pes_flag    = HI_FALSE;
        avpes_buf->buf_avpes.last_mq_dsc_read_idx  = 0x0;
        avpes_buf->buf_avpes.last_mq_dsc_write_idx = 0x0;
    }

    /* init and open the buffer configure */
    dmx_hal_buf_config(rbuf->base.mgmt, rbuf->base.id, rbuf->buf_phy_addr, rbuf->buf_size, secure);
    dmx_hal_buf_open(rbuf->base.mgmt, rbuf->base.id, rbuf->time_int_th, rbuf->ts_cnt_int_th, rbuf->seop_int_th);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_avpes_open_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_avpes_open_impl(rbuf);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = HI_SUCCESS;

out0:
    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_avpes_close_impl(struct dmx_r_buf *rbuf)
{
    hi_u32 port_offset;
    hi_bool secure;
    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;

    /* close the buffer configure */
    dmx_hal_buf_close(rbuf->base.mgmt, rbuf->base.id);

    /* force clear the output buffer ap status */
    if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
        port_offset = rbuf->port - DMX_RAM_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_ram_port_cls_ap_status(rbuf->base.mgmt, port_offset);
    }

    secure = (rbuf->secure_mode == DMX_SECURE_TEE) ? HI_TRUE : HI_FALSE;
    dmx_hal_buf_de_config(rbuf->base.mgmt, rbuf->base.id, 0, 0, secure);

    if (avpes_buf->buf_avpes.mq_use_flag) {
        struct list_head *node_pointer = HI_NULL;
        struct list_head *tmp_node_pointer = HI_NULL;

        /* mq disable */
        dmx_hal_buf_dis_mq_pes(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_mq_de_config(rbuf->base.mgmt, rbuf->base.id);

        /* free the left es block entry */
        list_for_each_safe(node_pointer, tmp_node_pointer, &avpes_buf->buf_avpes.es_block_info_head) {
            struct es_block_info_node *es_block_info_ptr = list_entry(node_pointer, struct es_block_info_node, node);

            DMX_NULL_POINTER_RETURN(es_block_info_ptr);
            list_del(&es_block_info_ptr->node);

            HI_KFREE(HI_ID_DEMUX, es_block_info_ptr);
            es_block_info_ptr = HI_NULL;
        }

        /* for avpes playfct */
        avpes_buf->buf_avpes.last_pts          = DMX_INVALID_PTS;
        avpes_buf->buf_avpes.last_pes_head_len = 0;
        avpes_buf->buf_avpes.first_pes_flag    = HI_FALSE;
        avpes_buf->buf_avpes.last_mq_dsc_read_idx  = 0x0;
        avpes_buf->buf_avpes.last_mq_dsc_write_idx = 0x0;
    }

    /* general */
    rbuf->sw_read               = 0;
    rbuf->last_addr             = rbuf->buf_phy_addr;
    rbuf->last_read             = 0;
    rbuf->last_end              = 0;
    rbuf->rool_flag             = HI_FALSE;
    rbuf->eos_flag              = HI_FALSE;
    rbuf->ovfl_flag             = HI_FALSE;
    rbuf->eos_cnt               = 0;
    rbuf->eos_record_idx        = 0;
    rbuf->ovfl_cnt              = 0;

    rbuf->acquire_cnt = 0;
    rbuf->acquire_ok_cnt = 0;
    rbuf->release_cnt = 0;

    /* clear the output buffer */
    dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

    /* clear the mmu cache */
    dmx_hal_buf_clr_mmu_cache(rbuf->base.mgmt, rbuf->base.id);

    cancel_work_sync(&rbuf->buf_worker);
    cancel_work_sync(&rbuf->buf_ovfl_worker);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_avpes_close_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_avpes_close_impl(rbuf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_avpes_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_r_raw_pid_ch *raw_pid_ch = HI_NULL;
    struct dmx_r_play_fct *play_fct = (struct dmx_r_play_fct *)obj;
    struct dmx_r_playfct_avpes *play_avpes = (struct dmx_r_playfct_avpes *)obj;

    rbuf->sw_read = 0;
    raw_pid_ch = _get_raw_pid_obj_for_play(play_fct->rpid_ch, play_fct);
    if (raw_pid_ch) {
        dmx_hal_pid_tab_flt_dis(rbuf->base.mgmt, raw_pid_ch->raw_pid_chan_id);

        dmx_hal_band_clear_chan(rbuf->base.mgmt, play_fct->rpid_ch->rband->base.id);

        dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

        /* deconfig the secure buffer */
        if (play_fct->secure_mode == DMX_SECURE_TEE) {
            dmx_tee_deconfig_buf(play_avpes->play_avpes.av_pes_chan_id, play_fct->play_fct_type);
        }

        dmx_dofunc_no_return(_dmx_r_buf_avpes_close_impl(rbuf));

        /* config the secure buffer */
        if (play_fct->secure_mode == DMX_SECURE_TEE) {
            ret = dmx_tee_config_buf(play_avpes->play_avpes.av_pes_chan_id, play_fct->play_fct_type);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("configure secure buf failed!\n");
            }
        }

        dmx_dofunc_no_return(_dmx_r_buf_avpes_open_impl(rbuf));

        dmx_hal_pid_tab_flt_en(rbuf->base.mgmt, raw_pid_ch->raw_pid_chan_id);
    } else {
        HI_ERR_DEMUX("null pointer\n");
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_avpes_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_avpes_reset_impl(rbuf, obj);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

struct dmx_r_buf_ops g_dmx_buf_avpes_ops = {
    .open               = dmx_r_buf_avpes_open_impl,
    .get_attrs          = dmx_r_buf_get_attrs_impl,
    .get_status         = dmx_r_buf_get_status_impl,
    .attach             = dmx_r_buf_attach_impl,
    .detach             = dmx_r_buf_detach_impl,
    .pre_mmap           = dmx_r_buf_pre_mmap_impl,
    .pst_mmap           = dmx_r_buf_pst_mmap_impl,
    .acquire_buf        = dmx_r_buf_acquire_av_pes_impl,
    .release_buf        = dmx_r_buf_release_av_pes_impl,
    .reset              = dmx_r_buf_avpes_reset_impl,
    .close              = dmx_r_buf_avpes_close_impl,
};

static hi_s32 _dmx_buf_init_avpes(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    hi_s32 ret;
    dmx_mem_info mq_dsc_buf = {0};
    hi_char buf_name[DMX_STR_LEN_32] = {0};
    hi_u32 mq_dsc_buf_size;

    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;

    avpes_buf->avpes_ops = &g_dmx_buf_avpes_ops;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rbuf->ops->open = avpes_buf->avpes_ops->open;
    rbuf->ops->acquire_buf = avpes_buf->avpes_ops->acquire_buf;
    rbuf->ops->release_buf = avpes_buf->avpes_ops->release_buf;
    rbuf->ops->reset = avpes_buf->avpes_ops->reset;
    rbuf->ops->close = avpes_buf->avpes_ops->close;

    /* alloc mq desc, hw restrict that it must align with 4K bytes. */
    mq_dsc_buf_size = MQ_DEFAULT_RAM_DSC_DEPTH * MQ_DEFAULT_RAM_DSC_SIZE;
    snprintf(buf_name, sizeof(buf_name), "dmx_mq_buf[%d]", rbuf->base.id);
    ret = dmx_alloc_and_map_mmz(buf_name, mq_dsc_buf_size, 0x0, &mq_dsc_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("mmz malloc 0x%x failed\n", mq_dsc_buf_size);
        goto out;
    }

    WARN_ON(mq_dsc_buf.buf_phy_addr % RAM_DSC_ADDR_ALIGN || mq_dsc_buf_size != mq_dsc_buf.buf_size);
    avpes_buf->buf_avpes.mq_use_flag     = HI_TRUE;
    avpes_buf->buf_avpes.mq_buf_handle   = mq_dsc_buf.handle;
    avpes_buf->buf_avpes.mq_buf_obj      = mq_dsc_buf.dmx_buf_obj;
    avpes_buf->buf_avpes.mq_buf_ker_addr = mq_dsc_buf.buf_vir_addr;
    avpes_buf->buf_avpes.mq_buf_phy_addr = mq_dsc_buf.buf_phy_addr;
    avpes_buf->buf_avpes.mq_dsc_depth    = MQ_DEFAULT_RAM_DSC_DEPTH;
    memset(avpes_buf->buf_avpes.mq_buf_ker_addr, 0, mq_dsc_buf_size);

    /* init avpes member */
    avpes_buf->buf_avpes.last_pts          = DMX_INVALID_PTS;
    avpes_buf->buf_avpes.last_pes_head_len = 0;
    avpes_buf->buf_avpes.first_pes_flag    = HI_FALSE;
    INIT_LIST_HEAD(&avpes_buf->buf_avpes.es_block_info_head);

    rbuf->time_int_th       = 0;  /* do not open time out interrupt */
    rbuf->ts_cnt_int_th     = 0;  /* do not open ts cnt interrupt */
    rbuf->seop_int_th       = 1;  /* every seop has a interrupot, so every mq include whole pes */

    /* init the mq configure */
    dmx_hal_mq_config(rbuf->base.mgmt, rbuf->base.id, avpes_buf->buf_avpes.mq_buf_phy_addr,
                      avpes_buf->buf_avpes.mq_dsc_depth);

out:
    return ret;
}

static hi_s32 _dmx_r_buf_rec_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    hi_u32 index;
    struct dmx_r_raw_pid_ch *raw_pid_ch[REC_MAX_PID_CNT] = {HI_NULL};
    struct dmx_r_rec_fct *rec_fct = (struct dmx_r_rec_fct *)obj;

    rbuf->sw_read = 0;
    for_each_set_bit(index, rec_fct->pid_bit_index, REC_MAX_PID_CNT) {
        raw_pid_ch[index] = _get_raw_pid_obj_for_rec(rec_fct->pid_chan_array[index], rec_fct);
        if (raw_pid_ch[index] != HI_NULL) {
            dmx_hal_pid_tab_flt_dis(rbuf->base.mgmt, raw_pid_ch[index]->raw_pid_chan_id);
        } else {
            HI_ERR_DEMUX("null pointer\n");
        }
    }

    for_each_set_bit(index, rec_fct->pid_bit_index, REC_MAX_PID_CNT) {
        if (raw_pid_ch[index] != HI_NULL) {
            dmx_hal_band_clear_chan(rbuf->base.mgmt, rec_fct->pid_chan_array[index]->rband->base.id);
            break;
        }
    }

    dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

    dmx_dofunc_no_return(_dmx_r_buf_close_impl(rbuf));

    for_each_set_bit(index, rec_fct->pid_bit_index, REC_MAX_PID_CNT) {
        dmx_hal_pid_tab_flt_en(rbuf->base.mgmt, raw_pid_ch[index]->raw_pid_chan_id);
    }

    dmx_dofunc_no_return(_dmx_r_buf_open_impl(rbuf));

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_rec_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_rec_reset_impl(rbuf, obj);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

struct dmx_r_buf_ops g_dmx_buf_rec_ops = {
    .open               = dmx_r_buf_open_impl,
    .get_attrs          = dmx_r_buf_get_attrs_impl,
    .get_status         = dmx_r_buf_get_status_impl,
    .attach             = dmx_r_buf_attach_impl,
    .detach             = dmx_r_buf_detach_impl,
    .pre_mmap           = dmx_r_buf_pre_mmap_impl,
    .pst_mmap           = dmx_r_buf_pst_mmap_impl,
    .acquire_buf        = dmx_r_buf_acquire_rec_impl,
    .release_buf        = dmx_r_buf_release_rec_impl,
    .reset              = dmx_r_buf_rec_reset_impl,
    .close              = dmx_r_buf_close_impl,
};

static hi_s32 _dmx_buf_init_rec(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    struct dmx_r_rec_buf *rec_buf = (struct dmx_r_rec_buf *)rbuf;

    rec_buf->rec_ops = &g_dmx_buf_rec_ops;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rbuf->ops->acquire_buf = rec_buf->rec_ops->acquire_buf;
    rbuf->ops->release_buf = rec_buf->rec_ops->release_buf;
    rbuf->ops->reset       = rec_buf->rec_ops->reset;

    /* set the base ts_cnt_int_th as 2(2 * ts_buf_int_set(128) * 188 = 47KB) */
    rbuf->ts_cnt_int_th = 0x2;

    return HI_SUCCESS;
}

struct dmx_r_buf_ops g_dmx_buf_scd_ops = {
    .open               = dmx_r_buf_open_impl,
    .get_attrs          = dmx_r_buf_get_attrs_impl,
    .get_status         = dmx_r_buf_get_status_impl,
    .attach             = dmx_r_buf_attach_impl,
    .detach             = dmx_r_buf_detach_impl,
    .pre_mmap           = dmx_r_buf_pre_mmap_impl,
    .pst_mmap           = dmx_r_buf_pst_mmap_impl,
    .acquire_scd        = dmx_r_buf_acquire_scd_impl,
    .release_scd        = dmx_r_buf_release_scd_impl,
    .close              = dmx_r_buf_close_impl,
};

static hi_s32 _dmx_buf_init_scd(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    struct dmx_r_scd_buf *scd_buf = (struct dmx_r_scd_buf *)rbuf;

    scd_buf->scd_ops = &g_dmx_buf_scd_ops;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rbuf->ops->acquire_scd = scd_buf->scd_ops->acquire_scd;
    rbuf->ops->release_scd = scd_buf->scd_ops->release_scd;

    /* set ts cnt interrupt as 1 to support index interrupt */
    rbuf->ts_cnt_int_th = 1;

    return HI_SUCCESS;
}

struct dmx_r_buf_ops g_dmx_buf_pes_ops = {
    .open               = dmx_r_buf_open_impl,
    .get_attrs          = dmx_r_buf_get_attrs_impl,
    .get_status         = dmx_r_buf_get_status_impl,
    .attach             = dmx_r_buf_attach_impl,
    .detach             = dmx_r_buf_detach_impl,
    .pre_mmap           = dmx_r_buf_pre_mmap_impl,
    .pst_mmap           = dmx_r_buf_pst_mmap_impl,
    .acquire_buf        = dmx_r_buf_acquire_pes_impl,
    .release_buf        = dmx_r_buf_release_pes_impl,
    .reset              = dmx_r_buf_reset_impl,
    .close              = dmx_r_buf_close_impl,
};

static hi_s32 _dmx_buf_init_pes(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    struct dmx_r_pes_buf *pes_buf = (struct dmx_r_pes_buf *)rbuf;

    pes_buf->pes_ops = &g_dmx_buf_pes_ops;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rbuf->ops->acquire_buf = pes_buf->pes_ops->acquire_buf;
    rbuf->ops->release_buf = pes_buf->pes_ops->release_buf;

    /* set the seop_in_th: 1 seop submit 1 interrupt */
    rbuf->seop_int_th = 1;

    return HI_SUCCESS;
}

/* for section buffer overflow process,
 * not process cancel_work_sync
 */
static hi_s32 _dmx_buf_sec_overflow_close_impl(struct dmx_r_buf *rbuf)
{
    hi_u32 port_offset = 0;
    hi_bool secure;

    struct list_head *node_pointer = HI_NULL;
    struct list_head *tmp_node_pointer = HI_NULL;
    struct dmx_r_sec_buf *sec_buf = (struct dmx_r_sec_buf *)rbuf;

    /* close the buffer configure */
    dmx_hal_buf_close(rbuf->base.mgmt, rbuf->base.id);

    /* force clear the output buffer ap status */
    if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
        port_offset = rbuf->port - DMX_RAM_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_ram_port_cls_ap_status(rbuf->base.mgmt, port_offset);
    } else if (rbuf->port >= DMX_TSIO_PORT_0 && rbuf->port < DMX_TSIO_PORT_MAX) {
        port_offset = rbuf->port - DMX_TSIO_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_buf_clr_tsio_ap(rbuf->base.mgmt, port_offset);
    }

    secure = (rbuf->secure_mode == DMX_SECURE_TEE) ? HI_TRUE : HI_FALSE;
    dmx_hal_buf_de_config(rbuf->base.mgmt, rbuf->base.id, 0, 0, secure);

    /* free the left section block entry */
    list_for_each_safe(node_pointer, tmp_node_pointer, &sec_buf->buf_sec.sec_block_info_head) {
        struct sec_block_info_node *sec_block_info_ptr = list_entry(node_pointer, struct sec_block_info_node, node);

        DMX_NULL_POINTER_RETURN(sec_block_info_ptr);
        list_del(&sec_block_info_ptr->node);

        HI_KFREE(HI_ID_DEMUX, sec_block_info_ptr);
        sec_block_info_ptr = HI_NULL;
    }

    /* general */
    rbuf->sw_read               = 0;
    rbuf->last_addr             = rbuf->buf_phy_addr;
    rbuf->last_read             = 0;
    rbuf->last_end              = 0;
    rbuf->rool_flag             = HI_FALSE;
    rbuf->eos_flag              = HI_FALSE;
    rbuf->ovfl_flag             = HI_FALSE;
    rbuf->eos_cnt               = 0;
    rbuf->eos_record_idx        = 0;
    rbuf->ovfl_cnt              = 0;

    rbuf->acquire_cnt = 0;
    rbuf->acquire_ok_cnt = 0;
    rbuf->release_cnt = 0;

    /* clear the output buffer */
    dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

    /* clear the mmu cache */
    dmx_hal_buf_clr_mmu_cache(rbuf->base.mgmt, rbuf->base.id);

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_buf_sec_close_impl(struct dmx_r_buf *rbuf)
{
    hi_u32 port_offset = 0;
    hi_bool secure;

    struct list_head *node_pointer = HI_NULL;
    struct list_head *tmp_node_pointer = HI_NULL;
    struct dmx_r_sec_buf *sec_buf = (struct dmx_r_sec_buf *)rbuf;

    /* close the buffer configure */
    dmx_hal_buf_close(rbuf->base.mgmt, rbuf->base.id);

    /* force clear the output buffer ap status */
    if (rbuf->port >= DMX_RAM_PORT_0 && rbuf->port < DMX_ANY_RAM_PORT) {
        port_offset = rbuf->port - DMX_RAM_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_ram_port_cls_ap_status(rbuf->base.mgmt, port_offset);
    } else if (rbuf->port >= DMX_TSIO_PORT_0 && rbuf->port < DMX_TSIO_PORT_MAX) {
        port_offset = rbuf->port - DMX_TSIO_PORT_0;
        dmx_hal_buf_cls_ap_int_status(rbuf->base.mgmt, rbuf->base.id);
        dmx_hal_buf_clr_tsio_ap(rbuf->base.mgmt, port_offset);
    }

    secure = (rbuf->secure_mode == DMX_SECURE_TEE) ? HI_TRUE : HI_FALSE;
    dmx_hal_buf_de_config(rbuf->base.mgmt, rbuf->base.id, 0, 0, secure);

    /* free the left section block entry */
    list_for_each_safe(node_pointer, tmp_node_pointer, &sec_buf->buf_sec.sec_block_info_head) {
        struct sec_block_info_node *sec_block_info_ptr = list_entry(node_pointer, struct sec_block_info_node, node);

        DMX_NULL_POINTER_RETURN(sec_block_info_ptr);
        list_del(&sec_block_info_ptr->node);

        HI_KFREE(HI_ID_DEMUX, sec_block_info_ptr);
        sec_block_info_ptr = HI_NULL;
    }

    /* general */
    rbuf->sw_read               = 0;
    rbuf->last_addr             = rbuf->buf_phy_addr;
    rbuf->last_read             = 0;
    rbuf->last_end              = 0;
    rbuf->rool_flag             = HI_FALSE;
    rbuf->eos_flag              = HI_FALSE;
    rbuf->ovfl_flag             = HI_FALSE;
    rbuf->eos_cnt               = 0;
    rbuf->eos_record_idx        = 0;
    rbuf->ovfl_cnt              = 0;

    rbuf->acquire_cnt = 0;
    rbuf->acquire_ok_cnt = 0;
    rbuf->release_cnt = 0;

    /* clear the output buffer */
    dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

    /* clear the mmu cache */
    dmx_hal_buf_clr_mmu_cache(rbuf->base.mgmt, rbuf->base.id);

    cancel_work_sync(&rbuf->buf_worker);
    cancel_work_sync(&rbuf->buf_ovfl_worker);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_sec_close_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_sec_close_impl(rbuf);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_r_buf_sec_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    struct dmx_r_raw_pid_ch *raw_pid_ch = HI_NULL;
    struct dmx_r_play_fct *play_fct = (struct dmx_r_play_fct *)obj;

    rbuf->sw_read = 0;
    raw_pid_ch = _get_raw_pid_obj_for_play(play_fct->rpid_ch, play_fct);
    if (raw_pid_ch) {
        dmx_hal_pid_tab_flt_dis(rbuf->base.mgmt, raw_pid_ch->raw_pid_chan_id);

        dmx_hal_band_clear_chan(rbuf->base.mgmt, play_fct->rpid_ch->rband->base.id);

        dmx_hal_dav_tx_clear_chan(rbuf->base.mgmt, rbuf->base.id);

        dmx_dofunc_no_return(_dmx_r_buf_sec_close_impl(rbuf));

        dmx_dofunc_no_return(_dmx_r_buf_open_impl(rbuf));

        dmx_hal_pid_tab_flt_en(rbuf->base.mgmt, raw_pid_ch->raw_pid_chan_id);
    } else {
        HI_ERR_DEMUX("null pointer\n");
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_sec_reset_impl(struct dmx_r_buf *rbuf, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    ret = _dmx_r_buf_sec_reset_impl(rbuf, obj);

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

struct dmx_r_buf_ops g_dmx_buf_sec_ops = {
    .open               = dmx_r_buf_open_impl,
    .get_attrs          = dmx_r_buf_get_attrs_impl,
    .get_status         = dmx_r_buf_get_status_impl,
    .attach             = dmx_r_buf_attach_impl,
    .detach             = dmx_r_buf_detach_impl,
    .pre_mmap           = dmx_r_buf_pre_mmap_impl,
    .pst_mmap           = dmx_r_buf_pst_mmap_impl,
    .acquire_buf        = dmx_r_buf_acquire_sec_impl,
    .release_buf        = dmx_r_buf_release_sec_impl,
    .reset              = dmx_r_buf_sec_reset_impl,
    .close              = dmx_r_buf_sec_close_impl,
};

static hi_s32 _dmx_buf_init_sec(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    struct dmx_r_sec_buf *sec_buf = (struct dmx_r_sec_buf *)rbuf;

    sec_buf->sec_ops = &g_dmx_buf_sec_ops;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rbuf->ops->acquire_buf = sec_buf->sec_ops->acquire_buf;
    rbuf->ops->release_buf = sec_buf->sec_ops->release_buf;
    rbuf->ops->reset       = sec_buf->sec_ops->reset;
    rbuf->ops->close       = sec_buf->sec_ops->close;

    INIT_LIST_HEAD(&sec_buf->buf_sec.sec_block_info_head);
    /* set the seop_in_th: 1 seop submit 1 interrupt */
    rbuf->seop_int_th = 1;

    return HI_SUCCESS;
}

struct dmx_r_buf_ops g_dmx_buf_ts_ops = {
    .open               = dmx_r_buf_open_impl,
    .get_attrs          = dmx_r_buf_get_attrs_impl,
    .get_status         = dmx_r_buf_get_status_impl,
    .attach             = dmx_r_buf_attach_impl,
    .detach             = dmx_r_buf_detach_impl,
    .pre_mmap           = dmx_r_buf_pre_mmap_impl,
    .pst_mmap           = dmx_r_buf_pst_mmap_impl,
    .acquire_buf        = dmx_r_buf_acquire_ts_impl,
    .release_buf        = dmx_r_buf_release_ts_impl,
    .reset              = dmx_r_buf_reset_impl,
    .close              = dmx_r_buf_close_impl,
};

static hi_s32 _dmx_buf_init_ts(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    struct dmx_r_ts_buf *ts_buf = (struct dmx_r_ts_buf *)rbuf;

    ts_buf->ts_ops = &g_dmx_buf_ts_ops;

    /* attention: here some of the base ops function will be replaced by sub ops function  */
    rbuf->ops->acquire_buf = ts_buf->ts_ops->acquire_buf;
    rbuf->ops->release_buf = ts_buf->ts_ops->release_buf;

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_buf_init_impl(struct dmx_r_buf *rbuf, dmx_buf_type buf_type)
{
    hi_s32 ret;
    hi_bool secure;
    struct dmx_mgmt *mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    rbuf->buf_type = buf_type;

    switch (rbuf->buf_type) {
        case DMX_BUF_TYPE_AVPES: {
            ret = _dmx_buf_init_avpes(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_create_avpes failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_REC: {
            ret = _dmx_buf_init_rec(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_create_rec failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_SCD: {
            ret = _dmx_buf_init_scd(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_create_scd failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_PES: {
            ret = _dmx_buf_init_pes(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_create_pes failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_SEC: {
            ret = _dmx_buf_init_sec(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_create_sec failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_TS: {
            ret = _dmx_buf_init_ts(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_create_ts failed!\n");
                goto out;
            }
            break;
        }

        default: {
            HI_ERR_DEMUX("invalid play type[0x%x]!\n", buf_type);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }
    }

    /* init the buffer configure */
    secure = (rbuf->secure_mode == DMX_SECURE_TEE) ? HI_TRUE : HI_FALSE;
    dmx_hal_buf_config(rbuf->base.mgmt, rbuf->base.id, rbuf->buf_phy_addr, rbuf->buf_size, secure);

out:
    return ret;
}

static hi_s32 dmx_r_buf_init_impl(struct dmx_r_buf *rbuf, dmx_buf_type buf_type, const dmx_tee_mem_info *tee_mem_info)
{
    hi_s32 ret;
    hi_mem_handle_t buf_handle;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        rbuf->base.id = tee_mem_info->buf_id;
        rbuf->buf_phy_addr = tee_mem_info->buf_phy_addr;

        ret = dmx_get_buf_fd_by_phyaddr(tee_mem_info->buf_phy_addr, tee_mem_info->buf_size, &buf_handle);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("get buf fd failed.\n");
            goto out0;
        }
        rbuf->buf_handle = buf_handle;
    }

    ret = _dmx_r_buf_init_impl(rbuf, buf_type);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = HI_SUCCESS;

out0:
    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 _dmx_buf_deinit_avpes(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    dmx_mem_info mq_desc_buf = {0};
    struct dmx_r_avpes_buf *avpes_buf = (struct dmx_r_avpes_buf *)rbuf;

    /* post desroy begin */
    dmx_dofunc_no_return(_dmx_r_buf_avpes_close_impl(rbuf));
    /* post desroy end */
    if (avpes_buf->buf_avpes.mq_use_flag) {
        /* deinit the mq configure */
        dmx_hal_mq_de_config(rbuf->base.mgmt, rbuf->base.id);
        if (!list_empty(&avpes_buf->buf_avpes.es_block_info_head)) {
            WARN(1, "es_block_info_head of rbuf is not empty.\n");
            ret = HI_ERR_DMX_BUSY;
            goto out;
        }
    }

    /* release the mq buffer */
    mq_desc_buf.handle       = avpes_buf->buf_avpes.mq_buf_handle;
    mq_desc_buf.dmx_buf_obj  = avpes_buf->buf_avpes.mq_buf_obj;
    mq_desc_buf.buf_vir_addr = avpes_buf->buf_avpes.mq_buf_ker_addr;
    mq_desc_buf.buf_phy_addr = avpes_buf->buf_avpes.mq_buf_phy_addr;
    mq_desc_buf.buf_size     = avpes_buf->buf_avpes.mq_dsc_depth * MQ_DEFAULT_RAM_DSC_SIZE;
    mq_desc_buf.user_map_flag = HI_FALSE;
    dmx_unmap_and_release_mmz(&mq_desc_buf);
    avpes_buf->buf_avpes.mq_buf_ker_addr = HI_NULL;

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 _dmx_buf_deinit_rec(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    return HI_SUCCESS;
}

static hi_s32 _dmx_buf_deinit_scd(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    return HI_SUCCESS;
}

static hi_s32 _dmx_buf_deinit_pes(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    return HI_SUCCESS;
}

static hi_s32 _dmx_buf_deinit_sec(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    hi_s32 ret;
    struct dmx_r_sec_buf *sec_buf = (struct dmx_r_sec_buf *)rbuf;

    ret = _dmx_r_buf_sec_close_impl(rbuf);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (!list_empty(&sec_buf->buf_sec.sec_block_info_head)) {
        WARN(1, "sec_block_info_head of rbuf is not empty.\n");
        return HI_ERR_DMX_BUSY;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_buf_deinit_ts(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    return HI_SUCCESS;
}

static hi_s32 _dmx_r_buf_deinit_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = rbuf->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    /* post desroy begin */
    dmx_dofunc_no_return(_dmx_r_buf_close_impl(rbuf));
    /* post desroy end */
    switch (rbuf->buf_type) {
        case DMX_BUF_TYPE_AVPES: {
            ret = _dmx_buf_deinit_avpes(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_deinit_avpes failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_REC: {
            ret = _dmx_buf_deinit_rec(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_deinit_rec failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_SCD: {
            ret = _dmx_buf_deinit_scd(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_deinit_scd failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_PES: {
            ret = _dmx_buf_deinit_pes(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_deinit_pes failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_SEC: {
            ret = _dmx_buf_deinit_sec(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_deinit_sec failed!\n");
                goto out;
            }
            break;
        }
        case DMX_BUF_TYPE_TS: {
            ret = _dmx_buf_deinit_ts(mgmt, rbuf);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("_dmx_buf_deinit_ts failed!\n");
                goto out;
            }
            break;
        }

        default: {
            HI_ERR_DEMUX("invalid play type[0x%x]!\n", rbuf->buf_type);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }
    }
    /* deinit the buffer configure */
    dmx_hal_buf_de_config(rbuf->base.mgmt, rbuf->base.id, 0, 0, HI_FALSE);

    cancel_work_sync(&rbuf->buf_worker);
    cancel_work_sync(&rbuf->buf_ovfl_worker);
out:
    return ret;
}

static hi_s32 dmx_r_buf_deinit_impl(struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    if (rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_dofunc_no_return(dmx_put_buf_fd(rbuf->buf_handle));
    }

    ret = _dmx_r_buf_deinit_impl(rbuf);
    if (ret == HI_SUCCESS) {
        rbuf->staled = HI_TRUE;
    }

    osal_mutex_unlock(&rbuf->lock);

    return ret;
}

static hi_s32 dmx_r_buf_suspend_impl(struct dmx_r_buf *rbuf)
{
    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_buf_close_impl(rbuf));

    osal_mutex_unlock(&rbuf->lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_r_buf_resume_impl(struct dmx_r_buf *rbuf)
{
    osal_mutex_lock(&rbuf->lock);

    WARN_ON(rbuf->staled == HI_TRUE);

    dmx_dofunc_no_return(_dmx_r_buf_open_impl(rbuf));

    osal_mutex_unlock(&rbuf->lock);

    return HI_SUCCESS;
}

struct dmx_r_buf_ops g_dmx_buf_ops = {
    .init               = dmx_r_buf_init_impl,
    .open               = dmx_r_buf_open_impl,
    .get_attrs          = dmx_r_buf_get_attrs_impl,
    .get_status         = dmx_r_buf_get_status_impl,
    .attach             = dmx_r_buf_attach_impl,
    .detach             = dmx_r_buf_detach_impl,
    .pre_mmap           = dmx_r_buf_pre_mmap_impl,
    .pst_mmap           = dmx_r_buf_pst_mmap_impl,
    .acquire_buf        = dmx_r_buf_acquire_impl,
    .release_buf        = dmx_r_buf_release_impl,
    .reset              = dmx_r_buf_reset_impl,
    .acquire_scd        = dmx_r_buf_acquire_scd_impl,
    .release_scd        = dmx_r_buf_release_scd_impl,
    .close              = dmx_r_buf_close_impl,
    .deinit             = dmx_r_buf_deinit_impl,

    .suspend            = dmx_r_buf_suspend_impl,
    .resume             = dmx_r_buf_resume_impl,
};

/*************************dmx_r_buf_xxx_impl*************************************************/
/*
 * demux mgmt helper functions.
 */
hi_s32 dmx_mgmt_init(hi_void)
{
    struct dmx_mgmt *mgmt = _get_dmx_mgmt();

    return mgmt->ops->init(mgmt);
}

hi_void dmx_mgmt_exit(hi_void)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    if (HI_SUCCESS != mgmt->ops->exit(mgmt)) {
        mgmt->ops->show_info(mgmt);

        /*
         * session must release all resource.
         */
        WARN(1, "List resource not release completely!\n");
    }
}

hi_s32 dmx_mgmt_get_cap(dmx_capability *cap)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    return mgmt->ops->get_cap(mgmt, cap);
}

hi_s32 dmx_mgmt_get_free_flt_cnt(hi_u32 *free_cnt)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    return mgmt->ops->get_free_flt_cnt(mgmt, free_cnt);
}

hi_s32 dmx_mgmt_suspend(hi_void)
{
    struct dmx_mgmt *mgmt = _get_dmx_mgmt();

    return mgmt->ops->suspend(mgmt);
}

hi_s32 dmx_mgmt_resume(hi_void)
{
    struct dmx_mgmt *mgmt = _get_dmx_mgmt();

    return mgmt->ops->resume(mgmt);
}

#ifdef HI_DEMUX_PROC_SUPPORT
static hi_void port_type_to_string(hi_char *str, hi_u32 len, hi_u32 type)
{
    switch (type) {
        case DMX_PORT_TYPE_PARALLEL_BURST:
            if (strncpy_s(str, len, "PARALLEL_BURST", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_PARALLEL_VALID:
            if (strncpy_s(str, len, "PARALLEL_VALID", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_PARALLEL_NOSYNC_204:
            if (strncpy_s(str, len, "PARALLEL_NOSYNC_204", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204:
            if (strncpy_s(str, len, "PARALLEL_NOSYNC_188_204", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_SERIAL:
            if (strncpy_s(str, len, "SERIAL", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_SERIAL2BIT:
            if (strncpy_s(str, len, "SERIAL2BIT", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_SERIAL_NOSYNC:
            if (strncpy_s(str, len, "SERIAL_NOSYNC", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_SERIAL2BIT_NOSYNC:
            if (strncpy_s(str, len, "SERIAL2BIT_NOSYNC", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_SERIAL_NOSYNC_NOVALID:
            if (strncpy_s(str, len, "SERIAL_NOSYNC_NOVALID", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID:
            if (strncpy_s(str, len, "SERIAL2BIT_NOSYNC_NOVALID", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_USER_DEFINED:
            if (strncpy_s(str, len, "USER_DEFINED", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_AUTO:
            if (strncpy_s(str, len, "AUTO", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_PARALLEL_VALID_DUMMY_SYNC:
            if (strncpy_s(str, len, "PARALLEL_VALID_DUMMY_SYNC", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
            break;

        case DMX_PORT_TYPE_PARALLEL_NOSYNC_188:
        default:
            if (strncpy_s(str, len, "PARALLEL_NOSYNC_188", len - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
    }
}

static hi_s32 dmx_proc_main_read(hi_void *p, hi_void *v)
{
    struct   dmx_mgmt *mgmt;
    struct   list_head *pos;

    osal_proc_print(p, "DmxId\tBandHandle\tPortId\tTEICnt\tCCDiscCnt\n");

    mgmt = get_dmx_mgmt();
    DMX_NULL_POINTER_RETURN(mgmt);

    osal_mutex_lock(&mgmt->band_list_lock);

    list_for_each(pos, &mgmt->band_head) {
        hi_char  port_str[0x8] = "--";
        struct dmx_r_band *rband = list_entry(pos, struct dmx_r_band, node);

        osal_mutex_lock(&rband->lock);
        if (rband->port < DMX_PORT_MAX) {
            snprintf(port_str, 0x8, "%u", rband->port);
        }
        osal_proc_print(p, "  %u\t%8x\t%s\t%u\t%u\n", rband->base.id, rband->band_handle, port_str, 0, 0);
        osal_mutex_unlock(&rband->lock);
    }

    osal_mutex_unlock(&mgmt->band_list_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_port_read(hi_void *p, hi_void *v)
{
    hi_s32 ret;
    hi_size_t lock_flag;
    struct list_head *pos;
    hi_char str[32] = "";      /* max character num is 32 */
    hi_char bit_sel[16]  = ""; /* max character num is 16 */
    hi_u32 i;

    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    osal_proc_print(p, " --------------------------------IF port--------------------------\n");
    osal_proc_print(p, " If port cnt:%u\n", mgmt->if_port_cnt);
    osal_proc_print(p, " Id   AllTsCnt  ErrTsCnt  Lock/lost  ClkMode  ClkReverse    BitSel   Type\n");
    for (i = 0; i < mgmt->if_port_cnt; i++) {
        hi_u32 allts_cnt;
        hi_u32 errts_cnt;
        dmx_port_attr if_port_attr = {0};

        ret = hi_drv_dmx_if_get_port_attrs(i + DMX_IF_PORT_0, &if_port_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("get if port attr failed!\n");
        }
        allts_cnt = dmx_hal_band_get_qam_ts_cnt(mgmt, i);
        errts_cnt = 0;

        port_type_to_string(str, sizeof(str), if_port_attr.port_type);
        if (if_port_attr.serial_bit_select == 0x0) {
            if (strncpy_s(bit_sel, sizeof(bit_sel), "D7", sizeof(bit_sel) - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
        } else {
            if (strncpy_s(bit_sel, sizeof(bit_sel), "D0", sizeof(bit_sel) - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
        }

        osal_proc_print(p, "%3u   0x%-8x  0x%-4x     %u/%u        %d        %d          %s       %s\n",
            i + DMX_IF_PORT_0, allts_cnt, errts_cnt, if_port_attr.sync_lock_threshold,
            if_port_attr.sync_lost_threshold, if_port_attr.tuner_clk_mode, if_port_attr.tuner_in_clk,
            bit_sel, str);
    }

    osal_proc_print(p, "\n-------------------------------TSI port-----------------------------------\n");
    osal_proc_print(p, " Tsi port cnt:%u\n", mgmt->tsi_port_cnt);
    osal_proc_print(p, " Id   AllTsCnt  ErrTsCnt  Lock/lost  ClkMode  ClkReverse    BitSel   Type\n");

    for (i = 0; i < mgmt->tsi_port_cnt; i++) {
        hi_u32 allts_cnt;
        hi_u32 errts_cnt;
        dmx_port_attr tsi_port_attr = {0};

        ret = hi_drv_dmx_tsi_get_port_attrs(i + DMX_TSI_PORT_0, &tsi_port_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("get tsi port attr failed!\n");
        }
        allts_cnt = dmx_hal_dvb_get_ts_cnt(mgmt, i);
        errts_cnt = dmx_hal_dvb_get_err_ts_cnt(mgmt, i);

        port_type_to_string(str, sizeof(str), tsi_port_attr.port_type);

        if (tsi_port_attr.serial_bit_select == 0x0) {
            if (strncpy_s(bit_sel, sizeof(bit_sel), "D7", sizeof(bit_sel) - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
        } else {
            if (strncpy_s(bit_sel, sizeof(bit_sel), "D0", sizeof(bit_sel) - 1)) {
                HI_ERR_DEMUX("strncpy_s failed!\n");
            }
        }

        osal_proc_print(p, "%3u   0x%-8x  0x%-4x     %u/%u        %d        %d          %s       %s\n",
            i + DMX_TSI_PORT_0, allts_cnt, errts_cnt, tsi_port_attr.sync_lock_threshold,
            tsi_port_attr.sync_lost_threshold, tsi_port_attr.tuner_clk_mode, tsi_port_attr.tuner_in_clk,
            bit_sel, str);
    }

    osal_proc_print(p, "\n----------------------------------------------------------------------RAM port---------------"
        "----------------------------------------------------------\n");
    osal_proc_print(p, " Ram port cnt:%u\n", mgmt->ram_port_cnt);
#ifdef HI_DMX_TSBUF_MULTI_THREAD_SUPPORT
    osal_proc_print(p, " Id\tAllTsCnt\t   BufAddr\tBufSize\tTotalBlk\tFreeBlk\tBlkSize\t      Get(Try/Ok)\t      Put\t"
        "     Push\tsecureMode\tmaxDataRate\tpktSize\n");
    osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
    list_for_each(pos, &mgmt->ram_port_head) {
        struct dmx_r_ram_port *ram_port = list_entry(pos, struct dmx_r_ram_port, node);

        osal_mutex_lock(&ram_port->lock);
        osal_proc_print(p, "%3u\t%8u\t0x%08x\t%7u\t%8u\t%7u\t%7u\t%9u\t%9u\t%9u\t%10u\t%11u\t%7u\n",
            ram_port->base.id + DMX_RAM_PORT_0,
            ram_port->all_pkt_cnt,
            ram_port->buf_phy_addr,
            ram_port->buf_size,
            ram_port->total_blk_nr,
            ram_port->free_blk_nr,
            ram_port->blk_size,
            ram_port->get_count,
            ram_port->get_valid_count,
            ram_port->put_count,
            ram_port->push_count,
            ram_port->secure_mode,
            ram_port->max_data_rate,
            ram_port->pkt_size);
        osal_mutex_unlock(&ram_port->lock);
    }
    osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);
#else
    osal_proc_print(p, " RamId  RamHandle  AllTsCnt   BufAddr     BufSize  TBufUsed   Read      Write     ReqAddr"
        "    ReqLen     BlkSize       Get(Try/Ok)"
        "    Put    SecureMode  MaxDataRate  PktSize\n");
    osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
    list_for_each(pos, &mgmt->ram_port_head) {
        struct dmx_r_ram_port *ram_port = list_entry(pos, struct dmx_r_ram_port, node);

        ret = ram_port_get_buf_used(ram_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("ram_port_get_buf_used failed with ret:0x%x\n", ret);
        }
        osal_mutex_lock(&ram_port->lock);
        /* Convert the unit to kbyte before calculating the percentage, 1024 byte per KB , multi 100 for show */
        snprintf(str, sizeof(str), "(%u%%)", (ram_port->buf_used / 1024 * 100) / (ram_port->buf_size / 1024));
        osal_proc_print(p, " %4u    %-8x  %-#8x  0x%-8llx   0x%-8x %-3s   0x%-8x 0x%-8x 0x%-8llx  %-#8x"
            "  %-#8x%10u/%-10u%-10u   %-5u   %-8u    %3u\n",
            ram_port->base.id + DMX_RAM_PORT_0, ram_port->handle, ram_port->all_pkt_cnt,
            ram_port->buf_phy_addr, ram_port->buf_size, str, ram_port->read, ram_port->write,
            ram_port->req_addr, ram_port->req_len, ram_port->blk_size, ram_port->get_count,
            ram_port->get_valid_count, ram_port->put_count, ram_port->secure_mode, ram_port->max_data_rate,
            ram_port->pkt_size);
        osal_mutex_unlock(&ram_port->lock);
    }
    osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);
#endif

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_pidch_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;

    osal_proc_print(p, "pid channel cnt:%u\n", mgmt->pid_channel_cnt);
    osal_proc_print(p, "PidChId\tPidChHandle\t Pid\tStaled\tPidCopyEn\tPidCopyStatus\tPidCopyChId\tPidCopyBufPhyAddr"
        "\tPidCopyBufSize\tRawPidCnt\tBandId\tBandHandle\tPortId\tKeyId\n");
    osal_mutex_lock(&mgmt->pid_channel_list_lock);
    list_for_each(pos, &mgmt->pid_channel_head) {
        struct dmx_r_pid_ch *rpid_ch = list_entry(pos, struct dmx_r_pid_ch, node0);

        osal_mutex_lock(&rpid_ch->lock);
        osal_proc_print(p, "%6u\t%10x\t%#4x\t%6d\t%9d\t%13d\t%11u\t       0x%010llx\t%12u\t%9u", rpid_ch->base.id,
            rpid_ch->pid_ch_handle, rpid_ch->pid, rpid_ch->staled,
            rpid_ch->pid_copy_en, rpid_ch->pid_copy_en_status, rpid_ch->pid_copy_chan_id,
            rpid_ch->pid_copy_buf_phy_addr, rpid_ch->pid_copy_buf_size, rpid_ch->raw_pid_ch_cnt);
        if (rpid_ch->rband != NULL) {
            struct dmx_r_band *rband = rpid_ch->rband;
            osal_proc_print(p, "\t%6u\t%10x\t%5u", rband->base.id + DMX_BAND_0, rband->band_handle, rband->port);
        } else {
            osal_proc_print(p, "\t------\t----------\t--------");
        }

        if (rpid_ch->rdsc_fct != HI_NULL) {
            struct dmx_r_dsc_fct *rdsc_fct = rpid_ch->rdsc_fct;
            osal_proc_print(p, "\t%3u\n", rdsc_fct->base.id);
        } else {
            osal_proc_print(p, "\t---\n");
        }
        osal_mutex_unlock(&rpid_ch->lock);
    }

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_raw_pidch_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;

    osal_proc_print(p, "raw ch cnt:%u\n", mgmt->raw_pid_channel_cnt);
    osal_proc_print(p, "RawPidChId   Staled\t PidChId\t Pid\tPlayFctId\tRecFctId\tRefPlayCnt\tRefRecCnt\tRawPidType\n");
    osal_mutex_lock(&mgmt->raw_pid_ch_total_list_lock);
    list_for_each(pos, &mgmt->raw_pid_ch_total_head) {
        struct dmx_r_raw_pid_ch *rrawpid_ch = list_entry(pos, struct dmx_r_raw_pid_ch, node0);

        osal_mutex_lock(&rrawpid_ch->lock);
        osal_proc_print(p, "%7u\t%8u", rrawpid_ch->raw_pid_chan_id, rrawpid_ch->staled);
        if (rrawpid_ch->rpid_ch != NULL) {
            osal_proc_print(p, "\t%8u", rrawpid_ch->rpid_ch->base.id);
            osal_proc_print(p, "\t%#4x", rrawpid_ch->rpid_ch->pid);
        } else {
            osal_proc_print(p, "\t-------");
        }
        if (rrawpid_ch->rplay_fct != NULL) {
            osal_proc_print(p, "\t%9u", rrawpid_ch->rplay_fct->base.id);
        } else {
            osal_proc_print(p, "\t--------");
        }
        if (rrawpid_ch->rrec_fct != NULL) {
            osal_proc_print(p, "\t%8u", rrawpid_ch->rrec_fct->base.id);
        } else {
            osal_proc_print(p, "\t--------");
        }
        osal_proc_print(p, "\t%10u\t%9u\t%10u\n", osal_atomic_read(&rrawpid_ch->ref_play_cnt),
            osal_atomic_read(&rrawpid_ch->ref_rec_cnt), rrawpid_ch->raw_pid_ch_type);
        osal_mutex_unlock(&rrawpid_ch->lock);
    }
    osal_mutex_unlock(&mgmt->raw_pid_ch_total_list_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_play_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;
    hi_u32 buf_used;
    hi_u32 hw_write;
    hi_char tmp_buf[10] = {0}; /* temp buf size is 10 */

    osal_proc_print(p, "play channel cnt:%u\n", mgmt->play_fct_cnt);
    osal_proc_print(p, "PlayId\tPlayHandle  Opened  PlayType  CrcMode  LiveMode  PortId  Secure"
        "  BufId\t    BufAddr\t BufSize\tPBufUsed   Acquire(Try/Ok)   Release\t Read\t  Write\t   Overflow"
        "\tPidChanId   PidHandle\tPid\n");
    osal_mutex_lock(&mgmt->play_fct_list_lock);
    list_for_each(pos, &mgmt->play_fct_head) {
        struct dmx_r_play_fct *rplay_fct = list_entry(pos, struct dmx_r_play_fct, node);

        osal_mutex_lock(&rplay_fct->lock);
        osal_proc_print(p, "%4u\t%8x\t%1u\t%1u\t%1u\t%1u", rplay_fct->base.id, rplay_fct->play_fct_handle,
            rplay_fct->is_opened, rplay_fct->play_fct_type, rplay_fct->crc_mode, rplay_fct->live_play);

        if (rplay_fct->rbuf != NULL) {
            struct dmx_r_buf *rbuf = rplay_fct->rbuf;
            hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
            buf_used = dmx_get_queue_lenth(rbuf->sw_read, hw_write, rbuf->buf_size);
            /* 1024 byte per KB , multi 100 for show */
            snprintf(tmp_buf, sizeof(tmp_buf), "(%u%%)", (buf_used / 1024 * 100) / (rbuf->buf_size / 1024));
            osal_proc_print(p, "\t%4u\t%4u\t%4u\t   %#10llx\t%#8x\t%6s %10u/%-10u %-10u%#8x  %#8x %6u",
                rbuf->port, rbuf->secure_mode, rbuf->base.id, rbuf->buf_phy_addr,
                rbuf->buf_size, tmp_buf, rbuf->acquire_cnt, rbuf->acquire_ok_cnt, rbuf->release_cnt,
                rbuf->sw_read, hw_write, rbuf->ovfl_cnt);
        } else {
            osal_proc_print(p, "\t--------\t----------\t----------\t-------");
        }

        if (rplay_fct->rpid_ch != NULL) {
            osal_proc_print(p, "\t%4u\t   %8x\t%#4x\n", rplay_fct->rpid_ch->base.id, rplay_fct->rpid_ch->pid_ch_handle,
                rplay_fct->rpid_ch->pid);
        } else {
            osal_proc_print(p, "\t------\t----------\t----\n");
        }
        osal_mutex_unlock(&rplay_fct->lock);
    }
    osal_mutex_unlock(&mgmt->play_fct_list_lock);

    return HI_SUCCESS;
}
static hi_s32 dmx_proc_filter_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;
    hi_s32 i;

    osal_proc_print(p, "flt cnt:%u\n", mgmt->flt_cnt);
    osal_proc_print(p, "FltId\tFltHandle   PlayId   PesSecId   BufId\tCrcMode\t Depth"
        "\t              Match              \t              Mask              \t             "
        "Negate             \n");
    osal_mutex_lock(&mgmt->flt_list_lock);
    list_for_each(pos, &mgmt->flt_head) {
        struct dmx_r_flt *flt = list_entry(pos, struct dmx_r_flt, node0);

        osal_mutex_lock(&flt->lock);
        osal_proc_print(p, "%3u\t%8x", flt->base.id, flt->flt_handle);

        if (flt->rplay_fct != NULL) {
            osal_proc_print(p, "%8u", flt->rplay_fct->base.id);
        } else {
            osal_proc_print(p, "\t--------");
        }

        osal_proc_print(p, "%8u\t%4u\t%5u", flt->pes_sec_id, flt->buf_id, flt->crc_mode);

        osal_proc_print(p, "\t%5u\t", flt->depth);
        for (i = 0; i < DMX_FILTER_MAX_DEPTH; i++) {
            osal_proc_print(p, "%02x", flt->match[i]);
        }
        osal_proc_print(p, "\t");
        for (i = 0; i < DMX_FILTER_MAX_DEPTH; i++) {
            osal_proc_print(p, "%02x", flt->mask[i]);
        }
        osal_proc_print(p, "\t");
        for (i = 0; i < DMX_FILTER_MAX_DEPTH; i++) {
            osal_proc_print(p, "%02x", flt->negate[i]);
        }
        osal_mutex_unlock(&flt->lock);
        osal_proc_print(p, "\n");
    }
    osal_mutex_unlock(&mgmt->flt_list_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_key_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;
    struct list_head *ch_node;
    hi_s32 i;

    osal_proc_print(p, "dsc cnt:%u\n", mgmt->dsc_fct_cnt);
    osal_proc_print(p, "DscId\tDscHandle\tStaled\tCaType\tEntropy\tAlgType\tIvType\tIvLen"
        "\t               IV               \tKeyType\tKeyLen\t              Key               \tChannelIds\n");
    osal_mutex_lock(&mgmt->dsc_fct_list_lock);
    list_for_each(pos, &mgmt->dsc_fct_head) {
        struct dmx_r_dsc_fct *dsc_fct = list_entry(pos, struct dmx_r_dsc_fct, node);
        osal_proc_print(p, "%5u\t%9x\t%6d\t%6u\t%7u\t%7u\t%6u\t%5u\t", dsc_fct->base.id, dsc_fct->dsc_fct_handle,
            dsc_fct->staled, dsc_fct->ca_type, dsc_fct->entropy_reduction,
            dsc_fct->alg, dsc_fct->ivtype, dsc_fct->ivlen);
        for (i = 0; i < DMX_MAX_KEY_LEN; i++) {
            osal_proc_print(p, "%02x", dsc_fct->IV[i]);
        }
        osal_proc_print(p, "\t%7u\t%6u\t", dsc_fct->key_type, dsc_fct->key_len);
        for (i = 0; i < DMX_MAX_KEY_LEN; i++) {
            osal_proc_print(p, "%02x", dsc_fct->key[i]);
        }
        osal_proc_print(p, "\t");

        osal_mutex_lock(&dsc_fct->pid_ch_list_lock);
        list_for_each(ch_node, &dsc_fct->pid_ch_head) {
            struct dmx_r_pid_ch *pid_ch = list_entry(ch_node, struct dmx_r_pid_ch, node2);
            osal_proc_print(p, "%u ", pid_ch->base.id);
        }
        osal_mutex_unlock(&dsc_fct->pid_ch_list_lock);

        osal_proc_print(p, "\n");
    }
    osal_mutex_unlock(&mgmt->dsc_fct_list_lock);
    return HI_SUCCESS;
}

static hi_s32 dmx_proc_pcr_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;
    hi_size_t lock_flag;

    osal_proc_print(p, "pcr fct cnt:%u\n", mgmt->pcr_fct_cnt);
    osal_proc_print(p,
        "PcrFctId\tPcrFctHandle\tStaled\tPort\t Pid\t       PcrVal       \t       ScrVal       \tSyncHandle\tBandId\n");
    osal_spin_lock_irqsave(&mgmt->pcr_fct_list_lock2, &lock_flag);
    list_for_each(pos, &mgmt->pcr_fct_head) {
        struct dmx_r_pcr_fct *pcr_fct = list_entry(pos, struct dmx_r_pcr_fct, node);
        osal_proc_print(p, "%8u\t%12u\t%6u\t%4u\t%4u\t%20llu\t%20llu\t%10u", pcr_fct->base.id, pcr_fct->handle,
            pcr_fct->staled, pcr_fct->port, pcr_fct->pid, pcr_fct->pcr_val, pcr_fct->scr_val,
            pcr_fct->sync_handle);
        if (pcr_fct->rband != NULL) {
            osal_proc_print(p, "\t%6u\n", pcr_fct->rband->base.id + DMX_BAND_0);
        } else {
            osal_proc_print(p, "\t------\n");
        }
    }
    osal_spin_unlock_irqrestore(&mgmt->pcr_fct_list_lock2, &lock_flag);

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_rec_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;
    hi_u32 i;
    hi_u32 buf_used;
    hi_u32 hw_write;
    hi_char tmp_buf[10] = {0};

    osal_proc_print(p, "record channel cnt:%u\n", mgmt->rec_chan_cnt);
    osal_proc_print(p, "RecId\tRecHandle  RecType  Descramed  Secure  IdxType  IdxSrcPid  PortId"
        "  BufId   BufAddr     BufSize    RBufUsed   Acquire(Try/Ok)   Release\t   Read\t    Write"
        "   Overflow   ScdId    BandId   PidChns\n");
    osal_mutex_lock(&mgmt->avr_chan_lock);
    list_for_each(pos, &mgmt->rec_fct_head) {
        struct dmx_r_rec_fct *rec_fct = list_entry(pos, struct dmx_r_rec_fct, node);
        osal_mutex_lock(&rec_fct->lock);
        osal_proc_print(p, "%2u\t%8x\t%1u\t%1u\t%1u\t%1u\t%#6x", rec_fct->base.id, rec_fct->rec_fct_handle,
            rec_fct->rec_type, rec_fct->descramed, rec_fct->secure_mode,
            rec_fct->index_type, rec_fct->index_src_pid);
        if (rec_fct->rbuf != NULL) {
            struct dmx_r_buf *rbuf = rec_fct->rbuf;
            hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
            buf_used = dmx_get_queue_lenth(rbuf->sw_read, hw_write, rbuf->buf_size);
            /* buf used ratio, KB, 1024 byte per KB, multi 100 for show */
            snprintf(tmp_buf, sizeof(tmp_buf), "(%u%%)", (buf_used / 1024 * 100) / (rbuf->buf_size / 1024));
            osal_proc_print(p, "    %5u\t%4u\t  %#10llx  %#7x    %4s %10u/%-10u %-10u%#8x  %#8x\t%u",
                rbuf->port, rbuf->base.id, rbuf->buf_phy_addr,
                rbuf->buf_size, tmp_buf, rbuf->acquire_cnt, rbuf->acquire_ok_cnt, rbuf->release_cnt,
                rbuf->sw_read, hw_write, rbuf->ovfl_cnt);
        } else {
            osal_proc_print(p, "\t--------\t----------\t----------\t-------");
        }

        osal_proc_print(p, "\t%4u", rec_fct->scd_id);
        if (rec_fct->rband != NULL) {
            osal_proc_print(p, "\t%8u", rec_fct->rband->base.id + DMX_BAND_0);
        } else {
            osal_proc_print(p, "\t------");
        }
        osal_proc_print(p, "\t");
        osal_mutex_lock(&rec_fct->pid_ch_list_lock);
        for_each_set_bit(i, rec_fct->pid_bit_index, REC_MAX_PID_CNT) {
            struct dmx_r_pid_ch *rpid_ch = (struct dmx_r_pid_ch *)rec_fct->pid_chan_array[i];
            osal_proc_print(p, "%u ", rpid_ch->base.id);
        }
        osal_mutex_unlock(&rec_fct->pid_ch_list_lock);
        osal_proc_print(p, "\n");

        osal_mutex_unlock(&rec_fct->lock);
    }
    osal_mutex_unlock(&mgmt->avr_chan_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_rec_index_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;
    hi_u32 buf_used;
    hi_u32 hw_write;
    hi_char tmp_buf[DMX_STR_LEN_16] = {0};

    osal_proc_print(p, "ScdId\tBufId\t BufAddr\tBufSize\t BufUsed\tAcquire(Try/Ok)\tRelease"
        "\t   Read\t    Write\tOverflow\n");
    osal_mutex_lock(&mgmt->avr_chan_lock);
    list_for_each(pos, &mgmt->rec_fct_head) {
        struct dmx_r_rec_fct *rec_fct = list_entry(pos, struct dmx_r_rec_fct, node);
        osal_mutex_lock(&rec_fct->lock);
        osal_proc_print(p, "%2u", rec_fct->scd_id);
        if (rec_fct->scd_rbuf != NULL) {
            struct dmx_r_buf *rbuf = rec_fct->scd_rbuf;
            hw_write = dmx_hal_buf_get_cur_write_idx(mgmt, rbuf->base.id);
            buf_used = dmx_get_queue_lenth(rbuf->sw_read, hw_write, rbuf->buf_size);
            /* 1024 byte per KB, multi 100 for show */
            snprintf(tmp_buf, sizeof(tmp_buf), "(%u%%)", (buf_used / 1024 * 100) / (rbuf->buf_size / 1024));
            osal_proc_print(p, "\t%4u\t%#10llx\t%#4x\t%6s\t    %10u/%-10u%-10u%#6x   %#6x\t%u",
                rbuf->base.id, rbuf->buf_phy_addr,
                rbuf->buf_size, tmp_buf, rbuf->acquire_cnt, rbuf->acquire_ok_cnt, rbuf->release_cnt,
                rbuf->sw_read, hw_write, rbuf->ovfl_cnt);
        } else {
            osal_proc_print(p, "\t--------\t----------\t----------\t-------");
        }
        osal_mutex_unlock(&rec_fct->lock);
        osal_proc_print(p, "\n");
    }
    osal_mutex_unlock(&mgmt->avr_chan_lock);

    return HI_SUCCESS;
}

static hi_s32 dmx_proc_rmx_read(hi_void *p, hi_void *v)
{
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    struct list_head *pos;

    osal_proc_print(p, "rmx fct cnt:%u\n", mgmt->rmx_fct_cnt);
    osal_proc_print(p, "RmxFctId\tRmxFctHandle\tStaled\tOutPortId\tOutPortCnt\tSrcPortCnt"
        "\t       RmxPortIds      \t          Refs         \t       OverFlowCnt     \n");
    osal_mutex_lock(&mgmt->rmx_fct_list_lock);
    list_for_each(pos, &mgmt->rmx_fct_head) {
        struct dmx_r_rmx_fct *rmx_fct = list_entry(pos, struct dmx_r_rmx_fct, node);
        osal_proc_print(p, "%8u\t%12u\t%6d\t%9u\t%10u\t%10u\t%5u\t%5u\t%5u\n",
            rmx_fct->base.id, rmx_fct->rmx_fct_handle, rmx_fct->staled,
            rmx_fct->out_port_id, rmx_fct->out_port_cnt, rmx_fct->src_port_cnt,
            rmx_fct->port_info[0].port_id,
            rmx_fct->port_info[0].ref,
            rmx_fct->port_info[0].overflow_count);
    }
    osal_mutex_unlock(&mgmt->rmx_fct_list_lock);

    return HI_SUCCESS;
}

static hi_void drv_dmx_add_proc(hi_void)
{
    osal_proc_entry *item;

    item = osal_proc_add("demux_main", strlen("demux_main"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_main failed\n");
    } else {
        item->read = dmx_proc_main_read;
    }

    item = osal_proc_add("demux_port", strlen("demux_port"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_port failed\n");
    } else {
        item->read = dmx_proc_port_read;
    }

    item = osal_proc_add("demux_pidch", strlen("demux_pidch"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_pidch failed\n");
    } else {
        item->read = dmx_proc_pidch_read;
    }

    item = osal_proc_add("demux_raw_pidch", strlen("demux_raw_pidch"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_raw_pidch failed\n");
    } else {
        item->read = dmx_proc_raw_pidch_read;
    }

    item = osal_proc_add("demux_play", strlen("demux_play"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_chanbuf failed\n");
    } else {
        item->read = dmx_proc_play_read;
    }

    item = osal_proc_add("demux_filter", strlen("demux_filter"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_filter failed\n");
    } else {
        item->read = dmx_proc_filter_read;
    }

    item = osal_proc_add("demux_key", strlen("demux_key"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_key failed\n");
    } else {
        item->read = dmx_proc_key_read;
    }

    item = osal_proc_add("demux_pcr", strlen("demux_pcr"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_pcr failed\n");
    } else {
        item->read = dmx_proc_pcr_read;
    }

    item = osal_proc_add("demux_rec", strlen("demux_pcr"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_rec failed\n");
    } else {
        item->read = dmx_proc_rec_read;
    }

    item = osal_proc_add("demux_rec_index", strlen("demux_rec_index"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_rec_index failed\n");
    } else {
        item->read = dmx_proc_rec_index_read;
    }

    item = osal_proc_add("demux_rmx", strlen("demux_rmx"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_rmx failed\n");
    } else {
        item->read = dmx_proc_rmx_read;
    }

    return;
}

static hi_void drv_dmx_remove_proc(hi_void)
{
    osal_proc_remove("demux_main", strlen("demux_main"));
    osal_proc_remove("demux_port", strlen("demux_port"));
    osal_proc_remove("demux_pidch", strlen("demux_pidch"));
    osal_proc_remove("demux_raw_pidch", strlen("demux_raw_pidch"));
    osal_proc_remove("demux_play", strlen("demux_play"));
    osal_proc_remove("demux_filter", strlen("demux_filter"));
    osal_proc_remove("demux_key", strlen("demux_key"));
    osal_proc_remove("demux_pcr", strlen("demux_pcr"));
    osal_proc_remove("demux_rec", strlen("demux_rec"));
    osal_proc_remove("demux_rec_index", strlen("demux_rec_index"));
    osal_proc_remove("demux_rmx", strlen("demux_rmx"));

    return;
}
#endif

static hi_s32 __dmx_mgmt_lock_init_impl(struct dmx_mgmt *mgmt)
{
    /* mutex init */
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->if_port_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->tsi_port_list_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->tso_port_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->tag_port_list_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->ram_port_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->rmx_fct_list_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->pump_total_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->band_list_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->pid_channel_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->raw_pid_ch_total_list_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->play_fct_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->dsc_fct_list_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->pcr_fct_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->buf_list_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->flt_list_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->ts_chan_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->pes_sec_chan_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->avr_chan_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->pid_copy_chan_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->scd_chan_lock));

    dmx_dofunc_no_return(osal_mutex_init(&mgmt->select_wait_queue.data_sel_lock));
    dmx_dofunc_no_return(osal_mutex_init(&mgmt->teec.lock));

    /* spinlock init */
    dmx_dofunc_no_return(osal_spin_lock_init(&mgmt->ram_port_list_lock2));
    dmx_dofunc_no_return(osal_spin_lock_init(&mgmt->pid_channel_list_lock2));

    dmx_dofunc_no_return(osal_spin_lock_init(&mgmt->pcr_fct_list_lock2));
    dmx_dofunc_no_return(osal_spin_lock_init(&mgmt->buf_list_lock2));

    return HI_SUCCESS;
}

static hi_s32 __dmx_mgmt_init_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;
    hi_u32 tsi_policy;

    /* static resource init */
    ret = __dmx_mgmt_lock_init_impl(mgmt);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("mgmt mutex init failed.\n");
        return ret;
    }

    /* IF port */
    mgmt->if_port_cnt   = DMX_IF_PORT_CNT;
    bitmap_zero(mgmt->if_port_bitmap, mgmt->if_port_cnt);

    /* TSI port */
    ret = hi_drv_get_license_support(HI_LICENSE_TSI_EN, &tsi_policy);
    if (ret == HI_SUCCESS && (tsi_policy > 0)) {
        hi_u32 tsi_num = 1U << (tsi_policy + 1); /* tsi_policy:1, 4 tsi; tsi_policy:2, 8 tsi; tsi_policy:3, 16 tsi */
        mgmt->tsi_port_policy_cnt = tsi_num;
    } else {
        mgmt->tsi_port_policy_cnt = 0;
    }
    mgmt->tsi_port_cnt  = DMX_TSI_PORT_CNT;
    bitmap_zero(mgmt->tsi_port_bitmap, mgmt->tsi_port_cnt);

    /* TSO port */
    mgmt->tso_port_cnt  = DMX_TSO_PORT_CNT;
    bitmap_zero(mgmt->tso_port_bitmap, mgmt->tso_port_cnt);

    /* TAG port */
    mgmt->tag_port_cnt  = DMX_TAG_PORT_CNT;
    bitmap_zero(mgmt->tag_port_bitmap, mgmt->tag_port_cnt);

    /* TSIO port */
    mgmt->tsio_port_cnt = DMX_TSIO_PORT_CNT;

    /* RMX port */
    mgmt->rmx_port_cnt  = DMX_RMX_PORT_CNT;

    /* object resource init */
    /* RAM port */
    mgmt->ram_port_cnt = DMX_RAM_PORT_CNT;
    INIT_LIST_HEAD(&mgmt->ram_port_head);
    bitmap_zero(mgmt->ram_port_bitmap, mgmt->ram_port_cnt);

    /* rmx_fct */
    mgmt->rmx_fct_cnt = DMX_RMXFCT_CNT;
    INIT_LIST_HEAD(&mgmt->rmx_fct_head);
    bitmap_zero(mgmt->rmx_fct_bitmap, mgmt->rmx_fct_cnt);

    /* total rmx pump */
    mgmt->pump_total_cnt = DMX_RMXPUMP_TOTALCNT;
    INIT_LIST_HEAD(&mgmt->pump_total_head);
    bitmap_zero(mgmt->pump_total_bitmap, mgmt->pump_total_cnt);

    /* band */
    mgmt->band_cnt = DMX_BAND_CNT;
    mgmt->band_total_cnt = DMX_BAND_TOTAL_CNT;
    INIT_LIST_HEAD(&mgmt->band_head);
    bitmap_zero(mgmt->band_bitmap, mgmt->band_total_cnt);

    /* PID channel */
    mgmt->pid_channel_cnt = DMX_PID_CHANNEL_CNT;
    INIT_LIST_HEAD(&mgmt->pid_channel_head);
    bitmap_zero(mgmt->pid_channel_bitmap, mgmt->pid_channel_cnt);

    /* raw pid channel */
    mgmt->raw_pid_channel_cnt = DMX_PID_CHANNEL_CNT;
    INIT_LIST_HEAD(&mgmt->raw_pid_ch_total_head);
    bitmap_zero(mgmt->raw_pid_channel_bitmap, mgmt->raw_pid_channel_cnt);

    /* play channel */
    mgmt->play_fct_cnt = DMX_PLAY_CNT;
    INIT_LIST_HEAD(&mgmt->play_fct_head);
    bitmap_zero(mgmt->play_fct_bitmap, mgmt->play_fct_cnt);

    /* dsc_fct */
    mgmt->dsc_fct_cnt = DMX_DSC_CNT;
    INIT_LIST_HEAD(&mgmt->dsc_fct_head);
    bitmap_zero(mgmt->dsc_fct_bitmap, mgmt->dsc_fct_cnt);

    /* pcr_fct */
    mgmt->pcr_fct_cnt = DMX_PCR_CNT;
    INIT_LIST_HEAD(&mgmt->pcr_fct_head);
    bitmap_zero(mgmt->pcr_fct_bitmap, mgmt->pcr_fct_cnt);

    /* buf manage */
    mgmt->buf_cnt = DMX_BUF_CNT;
    INIT_LIST_HEAD(&mgmt->buf_head);
    bitmap_zero(mgmt->buf_bitmap, mgmt->buf_cnt);

    /* flt manage */
    mgmt->flt_cnt = DMX_FLT_CNT;
    INIT_LIST_HEAD(&mgmt->flt_head);
    bitmap_zero(mgmt->flt_bitmap, mgmt->flt_cnt);

    /* whole ts channel manage */
    mgmt->ts_chan_cnt = DMX_WHOLE_TS_CHAN_CNT;
    bitmap_zero(mgmt->ts_bitmap, mgmt->ts_chan_cnt);

    /* pes section channel manage */
    mgmt->pes_sec_chan_cnt = DMX_PES_SEC_CHAN_CNT;
    bitmap_zero(mgmt->pes_sec_bitmap, mgmt->pes_sec_chan_cnt);

    /* avpes and record share the channel */
    mgmt->av_pes_chan_cnt = DMX_AVPES_CHAN_CNT;
    mgmt->av_pes_used_cnt = 0;
    mgmt->rec_chan_cnt = DMX_REC_CNT;
    mgmt->rec_chan_used_cnt = 0;
    INIT_LIST_HEAD(&mgmt->rec_fct_head);
    bitmap_zero(mgmt->avr_bitmap, mgmt->av_pes_chan_cnt + mgmt->rec_chan_cnt);

    /* pid_copy channel manage */
    mgmt->pid_copy_chan_cnt = DMX_PIDCOPY_CHAN_CNT;
    bitmap_zero(mgmt->pid_copy_bitmap, mgmt->pid_copy_chan_cnt);

    /* scd channel manage */
    mgmt->scd_chan_cnt = DMX_SCD_CHAN_CNT;
    bitmap_zero(mgmt->scd_bitmap, mgmt->scd_chan_cnt);

    fidx_init();

    mgmt->select_wait_queue.condition = 0;
    init_waitqueue_head(&mgmt->select_wait_queue.wait_queue);

    /* init the ree&tee communication environment */
    ret = dmx_teec_init(&mgmt->teec);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dmx_teec_init failed, ret[%#x]\n", ret);
        return ret;
    }

#ifdef HI_DEMUX_PROC_SUPPORT
    drv_dmx_add_proc();
#endif

    return ret;
}

static hi_void __dmx_mgmt_lock_destory_impl(struct dmx_mgmt *mgmt)
{
    hi_u32 i;

    /* mutex deinit */
    osal_mutex_destory(&mgmt->if_port_list_lock);
    osal_mutex_destory(&mgmt->tsi_port_list_lock);
    osal_mutex_destory(&mgmt->tso_port_list_lock);
    osal_mutex_destory(&mgmt->tag_port_list_lock);
    osal_mutex_destory(&mgmt->ram_port_list_lock);
    osal_mutex_destory(&mgmt->rmx_fct_list_lock);
    osal_mutex_destory(&mgmt->pump_total_list_lock);
    osal_mutex_destory(&mgmt->band_list_lock);
    osal_mutex_destory(&mgmt->pid_channel_list_lock);
    osal_mutex_destory(&mgmt->raw_pid_ch_total_list_lock);
    osal_mutex_destory(&mgmt->play_fct_list_lock);
    osal_mutex_destory(&mgmt->dsc_fct_list_lock);
    osal_mutex_destory(&mgmt->pcr_fct_list_lock);
    osal_mutex_destory(&mgmt->buf_list_lock);
    osal_mutex_destory(&mgmt->flt_list_lock);
    osal_mutex_destory(&mgmt->ts_chan_lock);
    osal_mutex_destory(&mgmt->pes_sec_chan_lock);
    osal_mutex_destory(&mgmt->avr_chan_lock);
    osal_mutex_destory(&mgmt->pid_copy_chan_lock);
    osal_mutex_destory(&mgmt->scd_chan_lock);
    osal_mutex_destory(&mgmt->select_wait_queue.data_sel_lock);
    osal_mutex_destory(&mgmt->teec.lock);

    /* destory if lock */
    for (i = 0; i < DMX_IF_PORT_CNT; i++) {
        osal_mutex_destory(&mgmt->if_port_info[i].lock);
    }

    /* destory tsi lock */
    for (i = 0; i < DMX_TSI_PORT_CNT; i++) {
        osal_mutex_destory(&mgmt->tsi_port_info[i].lock);
    }

    /* destory tso lock */
    for (i = 0; i < DMX_TSO_PORT_CNT; i++) {
        osal_mutex_destory(&mgmt->tso_port_info[i].lock);
    }

    /* destory tag lock */
    for (i = 0; i < DMX_TAG_PORT_CNT; i++) {
        osal_mutex_destory(&mgmt->tag_port_info[i].lock);
    }

    /* spinlock deinit */
    osal_spin_lock_destory(&mgmt->ram_port_list_lock2);
    osal_spin_lock_destory(&mgmt->pid_channel_list_lock2);

    osal_spin_lock_destory(&mgmt->pcr_fct_list_lock2);;
    osal_spin_lock_destory(&mgmt->buf_list_lock2);
}

static inline hi_void __dmx_mgmt_exit_impl(struct dmx_mgmt *mgmt)
{
#ifdef HI_DEMUX_PROC_SUPPORT
    drv_dmx_remove_proc();
#endif

    fidx_de_init();

    /* tee deinit */
    dmx_teec_deinit(&mgmt->teec);

    /* lock resource destroy */
    __dmx_mgmt_lock_destory_impl(mgmt);
}

static void dmx_if_port_default_config(struct dmx_mgmt *mgmt)
{
    hi_s32 i;

    /* if port */
    for (i = 0; i < DMX_IF_PORT_CNT; i++) {
        if (osal_mutex_init(&mgmt->if_port_info[i].lock) != 0) {
            HI_ERR_DEMUX("if port(%d) mutex init err.\n", i);
        }
        mgmt->if_port_info[i].port_mod = DMX_PORT_MODE_INTERNAL;
        mgmt->if_port_info[i].port_type = DMX_PORT_TYPE_PARALLEL_NOSYNC_188;
        mgmt->if_port_info[i].bit_selector = 0;
        mgmt->if_port_info[i].sync_lock_th = 5;  /* default sync_lock_th as 5 */
        mgmt->if_port_info[i].sync_lost_th = 1;  /* default sync_lost_th as 1 */
        mgmt->if_port_info[i].tuner_in_clk = 0;
        mgmt->if_port_info[i].tuner_clk_mode = 0;
    }

    return;
}

static void dmx_tsi_port_default_config(struct dmx_mgmt *mgmt)
{
    hi_s32 i;

    /* tsi port */
    for (i = 0; i < DMX_TSI_PORT_CNT; i++) {
        if (osal_mutex_init(&mgmt->tsi_port_info[i].lock) != 0) {
            HI_ERR_DEMUX("tsi port(%d) mutex init err.\n", i);
        }
        mgmt->tsi_port_info[i].port_mod = DMX_PORT_MODE_EXTERNAL;
        mgmt->tsi_port_info[i].port_type = DMX_PORT_TYPE_PARALLEL_NOSYNC_188;
        mgmt->tsi_port_info[i].bit_selector = 0;
        mgmt->tsi_port_info[i].sync_lock_th = 5; /* default sync_lock_th as 5 */
        mgmt->tsi_port_info[i].sync_lost_th = 1; /* default sync_lost_th as 1 */
        mgmt->tsi_port_info[i].tuner_in_clk = 0;
        mgmt->tsi_port_info[i].tuner_clk_mode = 0;
    }

    return;
}

static void dmx_tso_port_default_config(struct dmx_mgmt *mgmt)
{
    hi_s32 i;

    /* tso port */
    for (i = 0; i < DMX_TSO_PORT_CNT; i++) {
        if (osal_mutex_init(&mgmt->tso_port_info[i].lock) != 0) {
            HI_ERR_DEMUX("tso port(%d) mutex init err.\n", i);
        }
        mgmt->tso_port_info[i].attrs.enable      = HI_TRUE;
        mgmt->tso_port_info[i].attrs.clk_reverse = HI_TRUE;
        mgmt->tso_port_info[i].attrs.ts_source      = DMX_TSI_PORT_0;
        mgmt->tso_port_info[i].attrs.clk_mode    = DMX_TSO_CLK_MODE_NORMAL;
        mgmt->tso_port_info[i].attrs.valid_mode  = DMX_TSO_VALID_ACTIVE_OUTPUT;
        mgmt->tso_port_info[i].attrs.bit_sync    = HI_TRUE;
        mgmt->tso_port_info[i].attrs.port_type   = DMX_PORT_TYPE_SERIAL;
        mgmt->tso_port_info[i].attrs.bit_selector  = DMX_TSO_SERIAL_BIT_0;
        mgmt->tso_port_info[i].attrs.out_mode    = HI_FALSE;
    }

    return;
}

static void dmx_tag_port_default_config(struct dmx_mgmt *mgmt)
{
    hi_s32 i;

    for (i = 0; i < DMX_TAG_PORT_CNT; i++) {
        if (osal_mutex_init(&mgmt->tag_port_info[i].lock) != 0) {
            HI_ERR_DEMUX("tag port(%d) mutex init err.\n", i);
        }
        mgmt->tag_port_info[i].attrs.sync_mod = DMX_TAG_HEAD_SYNC;
        mgmt->tag_port_info[i].attrs.tag_len  = DMX_DEFAULT_TAG_LENGTH; /* bytes */
        mgmt->tag_port_info[i].attrs.ts_src   = DMX_PORT_MAX;
        mgmt->tag_port_info[i].attrs.enable = HI_FALSE;

        memset(mgmt->tag_port_info[i].attrs.tag, 0, sizeof(mgmt->tag_port_info[i].attrs.tag));
    }

    return;
}

static void dmx_port_default_config(struct dmx_mgmt *mgmt)
{
    /* if port */
    dmx_if_port_default_config(mgmt);

    /* tsi port */
    dmx_tsi_port_default_config(mgmt);

    /* tso port */
    dmx_tso_port_default_config(mgmt);

    /* tag port */
    dmx_tag_port_default_config(mgmt);

    return;
}

static hi_s32 _dmx_mgmt_init_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;

    ret = osal_atomic_init(&mgmt->ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("init ref count err.\n");
        goto out0;
    }

    ret = __dmx_mgmt_init_impl(mgmt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    mgmt->io_base = osal_ioremap_nocache(DMX_REGS_BASE, 0x30000);
    DMX_NULL_POINTER_GOTO(mgmt->io_base, out1);

    mgmt->mdsc_base = osal_ioremap_nocache(DMX_REGS_MDSC_BASE, 0x2000);
    DMX_NULL_POINTER_GOTO(mgmt->mdsc_base, out1);

    /* set the check code for all ram port */
    dmx_hal_ram_port_set_check_code(mgmt);

    /* enable mmu */
    dmx_hal_en_mmu(mgmt);

    if (hi_drv_osal_request_irq(DMX_IRQ_NUM, (irq_handler_t)dmx_isr, IRQF_SHARED, "int_dmx", (hi_void *)mgmt) != 0) {
        HI_ERR_DEMUX("request_irq irq(%d) failed.\n", DMX_IRQ_NUM);
        ret = HI_FAILURE;
        goto out1;
    }

    /* enable the interrupt */
    /* par */
    dmx_hal_par_set_cc_int(mgmt, HI_TRUE);

    /* dav */
    dmx_hal_un_mask_all_dav_int(mgmt);

    /* total */
    dmx_hal_en_all_int(mgmt);

    /* mq config, the minimum of not ap threshold is 88, here set 100 */
    dmx_hal_mq_set_ap_th(mgmt, 100); /* set ap threshold as 100 */

    /* port default config */
    dmx_port_default_config(mgmt);

    hi_drv_sys_set_irq_affinity(HI_ID_DEMUX, DMX_IRQ_NUM, "dmx");

    /* create dmx private workqueue */
    mgmt->dmx_queue = create_workqueue("dmx_queue");
    DMX_NULL_POINTER_GOTO(mgmt->dmx_queue, out1);

    /* exception monitor */
    mgmt->monitor = osal_kthread_create(dmx_monitor, mgmt, "dmx_monitor", 0);
    WARN_ON(IS_ERR(mgmt->monitor));

    mgmt->state = DMX_MGMT_OPENED;

    return HI_SUCCESS;

out1:

    osal_iounmap(mgmt->io_base);

    __dmx_mgmt_exit_impl(mgmt);
out0:
    return ret;
}

static hi_s32 dmx_mgmt_init_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    if (osal_mutex_init(&mgmt->lock) != 0) {
        HI_ERR_DEMUX("mgmt init err.\n");
        return ret;
    }

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state == DMX_MGMT_CLOSED) {
        ret = _dmx_mgmt_init_impl(mgmt);
        if (ret == HI_SUCCESS) {
            if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
                HI_ERR_DEMUX("inc mgmt ref count err.\n");
            }
        }
    } else if (mgmt->state == DMX_MGMT_OPENED) {
        WARN_ON(osal_atomic_read(&mgmt->ref_count) == 0);

        if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
            HI_ERR_DEMUX("inc mgmt ref count err.\n");
            return HI_FAILURE;
        }

        ret = HI_SUCCESS;
    } else {
        WARN(1, "Invalid mgmt state[%#x]!\n", mgmt->state);
        ret = HI_ERR_DMX_STALED;
    }

    osal_mutex_unlock(&mgmt->lock);

    return ret;
}

static hi_s32 _dmx_mgmt_exit_impl(struct dmx_mgmt *mgmt)
{
    WARN_ON(osal_atomic_read(&mgmt->ref_count) != 0x1);
    WARN_ON(mgmt->state != DMX_MGMT_OPENED);

    mgmt->state = DMX_MGMT_CLOSED;

    /* disable the interrupt */
    /* par */
    dmx_hal_par_set_cc_int(mgmt, HI_FALSE);

    /* dav */
    dmx_hal_mask_all_dav_int(mgmt);
    /* total */
    dmx_hal_dis_all_int(mgmt);

    /* destroy dmx private workqueue */
    if (mgmt->dmx_queue) {
        destroy_workqueue(mgmt->dmx_queue);
    }

    /* ctrl */
    osal_kthread_destroy(mgmt->monitor, HI_TRUE);

    hi_drv_osal_free_irq(DMX_IRQ_NUM, "int_dmx", (hi_void *)mgmt);

    /* disable mmu */
    dmx_hal_dis_mmu(mgmt);

    osal_iounmap(mgmt->mdsc_base);
    osal_iounmap(mgmt->io_base);

    __dmx_mgmt_exit_impl(mgmt);

    return HI_SUCCESS;
}
static hi_s32 dmx_mgmt_rel_and_exit(struct dmx_mgmt *mgmt)
{
    if (list_empty(&mgmt->ram_port_head) &&
        list_empty(&mgmt->pump_total_head) &&
        list_empty(&mgmt->rmx_fct_head) &&
        list_empty(&mgmt->band_head) &&
        list_empty(&mgmt->pid_channel_head) &&
        list_empty(&mgmt->play_fct_head) &&
        list_empty(&mgmt->rec_fct_head) &&
        list_empty(&mgmt->dsc_fct_head) &&
        list_empty(&mgmt->pcr_fct_head) &&
        list_empty(&mgmt->raw_pid_ch_total_head) &&
        list_empty(&mgmt->buf_head) &&
        list_empty(&mgmt->flt_head)) {
        return _dmx_mgmt_exit_impl(mgmt);
    } else {
        HI_ERR_DEMUX("attention: some list may not empty!\n");
    }

    return HI_FAILURE;
}

static hi_s32 dmx_mgmt_exit_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long start = jiffies, end = start + HZ; /* 1s */

    do {
        osal_mutex_lock(&mgmt->lock);

        if (mgmt->state == DMX_MGMT_OPENED) {
            WARN_ON(osal_atomic_read(&mgmt->ref_count) == 0);

            if (osal_atomic_read(&mgmt->ref_count) == 1) {
                ret = dmx_mgmt_rel_and_exit(mgmt);
                if (ret == HI_SUCCESS) {
                    if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
                        HI_DBG_DEMUX("mgmt ref_count is not 0.\n");
                    }
                }
            } else {
                if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
                    HI_DBG_DEMUX("mgmt ref_count is not 0.\n");
                }

                ret = HI_SUCCESS;
            }
        } else if (mgmt->state == DMX_MGMT_CLOSED) {
            WARN_ON(osal_atomic_read(&mgmt->ref_count));

            ret = HI_SUCCESS;
        } else {
            WARN(1, "Invalid mgmt state[%#x]\n", mgmt->state);
            ret = HI_ERR_DMX_INVALID_PARA;
        }

        osal_mutex_unlock(&mgmt->lock);

        msleep(10); /* 10 ms */
    } while (ret != HI_SUCCESS && time_in_range(jiffies, start, end));

    osal_atomic_destory(&mgmt->ref_count);
    osal_mutex_destory(&mgmt->lock);

    return ret;
}

static hi_s32 dmx_mgmt_get_cap_impl(struct dmx_mgmt *mgmt, dmx_capability *cap)
{
    cap->if_port_num    = mgmt->if_port_cnt;
    cap->tsi_port_num   = mgmt->tsi_port_cnt;
    cap->tso_port_num   = mgmt->tso_port_cnt;
    cap->tsio_port_num  = mgmt->tsio_port_cnt;
    cap->ram_port_num   = mgmt->ram_port_cnt;
    cap->rmx_port_num   = mgmt->rmx_fct_cnt;
    cap->tag_port_num   = mgmt->tag_port_cnt;
    cap->dmx_num        = mgmt->band_cnt;
    cap->channel_num    = mgmt->play_fct_cnt;
    cap->av_channel_num = mgmt->av_pes_chan_cnt;
    cap->filter_num     = mgmt->flt_cnt;
    cap->key_num        = mgmt->dsc_fct_cnt;
    cap->rec_chn_num    = mgmt->rec_chan_cnt;

    return HI_SUCCESS;
}

static hi_void __dmx_mgmt_suspend_input_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    /* ram port */
    osal_mutex_lock(&mgmt->ram_port_list_lock);
    list_for_each(node, &mgmt->ram_port_head) {
        struct dmx_r_ram_port *rram_port = list_entry(node, struct dmx_r_ram_port, node);
        ret = rram_port->ops->suspend(rram_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend ram port(%d) failed.\n", rram_port->base.id);
        }
        cancel_work_sync(&rram_port->dsc_end_worker);
        cancel_work_sync(&rram_port->flow_control_worker);
    }
    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    /* band */
    osal_mutex_lock(&mgmt->band_list_lock);
    list_for_each(node, &mgmt->band_head) {
        struct dmx_r_band *rband = list_entry(node, struct dmx_r_band, node);
        ret = rband->ops->suspend(rband);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend band(%d) failed.\n", rband->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->band_list_lock);
}

static hi_void __dmx_mgmt_suspend_route_config_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    /* pidch */
    osal_mutex_lock(&mgmt->pid_channel_list_lock);
    list_for_each(node, &mgmt->pid_channel_head) {
        struct dmx_r_pid_ch *rpid_ch = list_entry(node, struct dmx_r_pid_ch, node0);
        ret = rpid_ch->ops->suspend(rpid_ch);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend raw channel(%d) failed.\n", rpid_ch->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    /* rmx pump */
    osal_mutex_lock(&mgmt->pump_total_list_lock);
    list_for_each(node, &mgmt->pump_total_head) {
        struct dmx_r_rmx_pump *rrmx_pump = list_entry(node, struct dmx_r_rmx_pump, node0);
        ret = rrmx_pump->ops->suspend(rrmx_pump);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend rrmx_pump(%d) failed.\n", rrmx_pump->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->pump_total_list_lock);

    /* rmx_fct */
    osal_mutex_lock(&mgmt->rmx_fct_list_lock);
    list_for_each(node, &mgmt->rmx_fct_head) {
        struct dmx_r_rmx_fct *rrmx = list_entry(node, struct dmx_r_rmx_fct, node);
        ret = rrmx->ops->suspend(rrmx);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend rrmx(%d) failed.\n", rrmx->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->rmx_fct_list_lock);

    /* descrambler */
    osal_mutex_lock(&mgmt->dsc_fct_list_lock);
    list_for_each(node, &mgmt->dsc_fct_head) {
        struct dmx_r_dsc_fct *rdsc = list_entry(node, struct dmx_r_dsc_fct, node);
        ret = rdsc->ops->suspend(rdsc);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend descrambler(%d) failed.\n", rdsc->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->dsc_fct_list_lock);

    /* flt */
    osal_mutex_lock(&mgmt->flt_list_lock);
    list_for_each(node, &mgmt->flt_head) {
        struct dmx_r_flt *rflt = list_entry(node, struct dmx_r_flt, node0);
        ret = rflt->ops->suspend(rflt);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend rflt(%d) failed.\n", rflt->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->flt_list_lock);
}

static hi_void __dmx_mgmt_suspend_output_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    /* pcr */
    osal_mutex_lock(&mgmt->pcr_fct_list_lock);
    list_for_each(node, &mgmt->pcr_fct_head) {
        struct dmx_r_pcr_fct *rpcr = list_entry(node, struct dmx_r_pcr_fct, node);

        ret = rpcr->ops->suspend(rpcr);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend rpcr(%d) failed.\n", rpcr->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->pcr_fct_list_lock);

    /* play */
    osal_mutex_lock(&mgmt->play_fct_list_lock);
    list_for_each(node, &mgmt->play_fct_head) {
        struct dmx_r_play_fct *rplay = list_entry(node, struct dmx_r_play_fct, node);

        ret = rplay->ops->suspend(rplay);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend play(%d) failed.\n", rplay->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->play_fct_list_lock);

    /* record */
    osal_mutex_lock(&mgmt->avr_chan_lock);
    list_for_each(node, &mgmt->rec_fct_head) {
        struct dmx_r_rec_fct *rrecord = list_entry(node, struct dmx_r_rec_fct, node);

        ret = rrecord->ops->suspend(rrecord);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend rrecord(%d) failed.\n", rrecord->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->avr_chan_lock);

    /* buffer */
    osal_mutex_lock(&mgmt->buf_list_lock);
    list_for_each(node, &mgmt->buf_head) {
        struct dmx_r_buf *rbuf = list_entry(node, struct dmx_r_buf, node);

        ret = rbuf->ops->suspend(rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("suspend rbuf(%d) failed.\n", rbuf->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->buf_list_lock);
}

static hi_void __dmx_mgmt_suspend_reg_impl(struct dmx_mgmt *mgmt)
{
    /* disable the interrupt */
    /* par */
    dmx_hal_par_set_cc_int(mgmt, HI_FALSE);

    /* dav */
    dmx_hal_mask_all_dav_int(mgmt);

    /* total */
    dmx_hal_dis_all_int(mgmt);

    /* disable mmu */
    dmx_hal_dis_mmu(mgmt);

    /* deconfigure the hardware clock */
    dmx_hal_deinit_hw();
}

static hi_void _dmx_mgmt_suspend_impl(struct dmx_mgmt *mgmt)
{
    /* input suspend */
    __dmx_mgmt_suspend_input_impl(mgmt);

    /* output suspend */
    __dmx_mgmt_suspend_output_impl(mgmt);

    /* route configure suspend */
    __dmx_mgmt_suspend_route_config_impl(mgmt);

    /* general register suspend */
    __dmx_mgmt_suspend_reg_impl(mgmt);
}

static hi_s32 dmx_mgmt_suspend_impl(struct dmx_mgmt *mgmt)
{
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        osal_mutex_unlock(&mgmt->lock);
        HI_WARN_DEMUX("mgmt has not opened.\n");
        return HI_SUCCESS;
    }
    osal_mutex_unlock(&mgmt->lock);

    /* suspend implement */
    if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
        HI_ERR_DEMUX("mgmt ref count is not 0.\n");
        return HI_FAILURE;
    }
    _dmx_mgmt_suspend_impl(mgmt);
    if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
        HI_DBG_DEMUX("mgmt ref count is not 0.\n");
    }

    return HI_SUCCESS;
}

static hi_void __dmx_mgmt_resume_reg_impl(struct dmx_mgmt *mgmt)
{
    /* configure the hardware clock */
    dmx_hal_init_hw();

    /* set the check code for all ram port */
    dmx_hal_ram_port_set_check_code(mgmt);

    /* enable mmu */
    dmx_hal_en_mmu(mgmt);

    /* enable the interrupt */
    /* par */
    dmx_hal_par_set_cc_int(mgmt, HI_TRUE);

    /* dav */
    dmx_hal_un_mask_all_dav_int(mgmt);

    /* total */
    dmx_hal_en_all_int(mgmt);

    /* mq config, the minimum of not ap threshold is 88, here set 100 */
    dmx_hal_mq_set_ap_th(mgmt, 100); /* set the ap threshold as 100 */
}

static hi_void __dmx_mgmt_resume_output_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;
    hi_u32 port_index;
    struct list_head *node = HI_NULL;

    /* buffer */
    osal_mutex_lock(&mgmt->buf_list_lock);
    list_for_each(node, &mgmt->buf_head) {
        struct dmx_r_buf *rbuf = list_entry(node, struct dmx_r_buf, node);

        ret = rbuf->ops->resume(rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume rbuf(%d) failed.\n", rbuf->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->buf_list_lock);

    /* play */
    osal_mutex_lock(&mgmt->play_fct_list_lock);
    list_for_each(node, &mgmt->play_fct_head) {
        struct dmx_r_play_fct *rplay = list_entry(node, struct dmx_r_play_fct, node);

        ret = rplay->ops->resume(rplay);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume play(%d) failed.\n", rplay->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->play_fct_list_lock);

    /* record */
    osal_mutex_lock(&mgmt->avr_chan_lock);
    list_for_each(node, &mgmt->rec_fct_head) {
        struct dmx_r_rec_fct *rrecord = list_entry(node, struct dmx_r_rec_fct, node);

        ret = rrecord->ops->resume(rrecord);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume rrecord(%d) failed.\n", rrecord->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->avr_chan_lock);

    /* pcr */
    osal_mutex_lock(&mgmt->pcr_fct_list_lock);
    list_for_each(node, &mgmt->pcr_fct_head) {
        struct dmx_r_pcr_fct *rpcr = list_entry(node, struct dmx_r_pcr_fct, node);

        ret = rpcr->ops->resume(rpcr);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume rpcr(%d) failed.\n", rpcr->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->pcr_fct_list_lock);

    /* tso port */
    osal_mutex_lock(&mgmt->tso_port_list_lock);
    for_each_set_bit(port_index, mgmt->tso_port_bitmap, mgmt->tso_port_cnt) {
        dmx_tso_port_attr resume_atrrs;

        dmx_dofunc_no_return(drv_dmx_tso_get_port_attrs(port_index + DMX_TSO_PORT_0, &resume_atrrs));
        dmx_dofunc_no_return(drv_dmx_tso_set_port_attrs(port_index + DMX_TSO_PORT_0, &resume_atrrs));
    }
    osal_mutex_unlock(&mgmt->tso_port_list_lock);
}

static hi_void __dmx_mgmt_resume_route_config_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    /* flt */
    osal_mutex_lock(&mgmt->flt_list_lock);
    list_for_each(node, &mgmt->flt_head) {
        struct dmx_r_flt *rflt = list_entry(node, struct dmx_r_flt, node0);
        ret = rflt->ops->resume(rflt);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume rflt(%d) failed.\n", rflt->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->flt_list_lock);

    /* rmx pump */
    osal_mutex_lock(&mgmt->pump_total_list_lock);
    list_for_each(node, &mgmt->pump_total_head) {
        struct dmx_r_rmx_pump *rrmx_pump = list_entry(node, struct dmx_r_rmx_pump, node0);
        ret = rrmx_pump->ops->resume(rrmx_pump);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume rrmx_pump(%d) failed.\n", rrmx_pump->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->pump_total_list_lock);

    /* rmx_fct */
    osal_mutex_lock(&mgmt->rmx_fct_list_lock);
    list_for_each(node, &mgmt->rmx_fct_head) {
        struct dmx_r_rmx_fct *rrmx = list_entry(node, struct dmx_r_rmx_fct, node);
        ret = rrmx->ops->resume(rrmx);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume rrmx(%d) failed.\n", rrmx->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->rmx_fct_list_lock);

    /* descrambler */
    osal_mutex_lock(&mgmt->dsc_fct_list_lock);
    list_for_each(node, &mgmt->dsc_fct_head) {
        struct dmx_r_dsc_fct *rdsc = list_entry(node, struct dmx_r_dsc_fct, node);
        ret = rdsc->ops->resume(rdsc);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume descrambler(%d) failed.\n", rdsc->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->dsc_fct_list_lock);

    /* pidch */
    osal_mutex_lock(&mgmt->pid_channel_list_lock);
    list_for_each(node, &mgmt->pid_channel_head) {
        struct dmx_r_pid_ch *rpid_ch = list_entry(node, struct dmx_r_pid_ch, node0);
        ret = rpid_ch->ops->resume(rpid_ch);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume raw channel(%d) failed.\n", rpid_ch->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->pid_channel_list_lock);
}

static hi_void __dmx_mgmt_resume_input_impl(struct dmx_mgmt *mgmt)
{
    hi_s32 ret;
    hi_u32 port_index;
    struct list_head *node = HI_NULL;

    /* band */
    osal_mutex_lock(&mgmt->band_list_lock);
    list_for_each(node, &mgmt->band_head) {
        struct dmx_r_band *rband = list_entry(node, struct dmx_r_band, node);
        ret = rband->ops->resume(rband);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume band(%d) failed.\n", rband->base.id);
        }
    }
    osal_mutex_unlock(&mgmt->band_list_lock);

    /* ram port */
    osal_mutex_lock(&mgmt->ram_port_list_lock);
    list_for_each(node, &mgmt->ram_port_head) {
        struct dmx_r_ram_port *rram_port = list_entry(node, struct dmx_r_ram_port, node);
        ret = rram_port->ops->resume(rram_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("resume ram port(%d) failed.\n", rram_port->base.id);
        }
        cancel_work_sync(&rram_port->dsc_end_worker);
        cancel_work_sync(&rram_port->flow_control_worker);
    }
    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    /* if port */
    osal_mutex_lock(&mgmt->if_port_list_lock);
    for_each_set_bit(port_index, mgmt->if_port_bitmap, mgmt->if_port_cnt) {
        dmx_port_attr resume_atrrs;

        dmx_dofunc_no_return(drv_dmx_if_get_port_attrs(port_index + DMX_IF_PORT_0, &resume_atrrs));
        dmx_dofunc_no_return(drv_dmx_if_set_port_attrs(port_index + DMX_IF_PORT_0, &resume_atrrs));
    }
    osal_mutex_unlock(&mgmt->if_port_list_lock);

    /* tsi port */
    osal_mutex_lock(&mgmt->tsi_port_list_lock);
    for_each_set_bit(port_index, mgmt->tsi_port_bitmap, mgmt->tsi_port_cnt) {
        dmx_port_attr resume_atrrs;

        dmx_dofunc_no_return(drv_dmx_tsi_get_port_attrs(port_index + DMX_TSI_PORT_0, &resume_atrrs));
        dmx_dofunc_no_return(drv_dmx_tsi_set_port_attrs(port_index + DMX_TSI_PORT_0, &resume_atrrs));
    }
    osal_mutex_unlock(&mgmt->tsi_port_list_lock);

    /* tag port */
    osal_mutex_lock(&mgmt->tag_port_list_lock);
    for_each_set_bit(port_index, mgmt->tag_port_bitmap, mgmt->tag_port_cnt) {
        dmx_tag_port_attr resume_atrrs;

        dmx_dofunc_no_return(drv_dmx_tag_get_port_attrs(port_index + DMX_TAG_PORT_0, &resume_atrrs));
        dmx_dofunc_no_return(drv_dmx_tag_set_port_attrs(port_index + DMX_TAG_PORT_0, &resume_atrrs));
    }
    osal_mutex_unlock(&mgmt->tag_port_list_lock);
}

static hi_void _dmx_mgmt_resume_impl(struct dmx_mgmt *mgmt)
{
    /* general register resume */
    __dmx_mgmt_resume_reg_impl(mgmt);

    /* route configure resume */
    __dmx_mgmt_resume_route_config_impl(mgmt);

    /* output resume */
    __dmx_mgmt_resume_output_impl(mgmt);

    /* input resume */
    __dmx_mgmt_resume_input_impl(mgmt);
}

static hi_s32 dmx_mgmt_resume_impl(struct dmx_mgmt *mgmt)
{
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        osal_mutex_unlock(&mgmt->lock);
        HI_WARN_DEMUX("mgmt has not opened.\n");
        return HI_SUCCESS;
    }
    osal_mutex_unlock(&mgmt->lock);

    /* resume implement */
    if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
        HI_DBG_DEMUX("inc mgmt ref err.\n");
        return HI_FAILURE;
    }
    _dmx_mgmt_resume_impl(mgmt);
    if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
        HI_DBG_DEMUX("mgmt ref count is not 0.\n");
    }

    return HI_SUCCESS;
}

/*
 *  Ram port Mgmt interface
 */
static hi_s32 _dmx_mgmt_init_ram_port_lock_impl(struct dmx_r_ram_port *new_ram_port)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_ram_port->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_ram_port ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_ram_port->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_ram_port lock init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_ram_port->blk_map_lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_ram_port blk_map_lock init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_ram_port_impl(struct dmx_mgmt *mgmt, dmx_port port,
        const dmx_ram_port_attr *attrs, struct dmx_r_ram_port **ram_port)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id;
    struct dmx_r_ram_port *new_ram_port = HI_NULL;
    hi_size_t lock_flag;

    if (port == DMX_ANY_RAM_PORT) { /* choose any available */
        id = find_first_zero_bit(mgmt->ram_port_bitmap, mgmt->ram_port_cnt);
        if (!(id < mgmt->ram_port_cnt)) {
            HI_ERR_DEMUX("there is no available ram port now!\n");
            ret = HI_ERR_DMX_NO_RESOURCE;
            goto out;
        }
    } else if (port >= DMX_RAM_PORT_0 && port < DMX_ANY_RAM_PORT) {
        unsigned long mask, *p;

        id = port - DMX_RAM_PORT_0;

        mask = BIT_MASK(id);
        p = ((unsigned long *)mgmt->ram_port_bitmap) + BIT_WORD(id);
        if (*p & mask) {
            HI_ERR_DEMUX("the RAM port(%d) is busy now!.\n", port);
            ret = HI_ERR_DMX_BUSY;
            goto out;
        }
    } else {
        WARN(1, "Invalid port[%#x]!\n", port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    new_ram_port = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_ram_port), GFP_KERNEL);
    if (!new_ram_port) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = _dmx_mgmt_init_ram_port_lock_impl(new_ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_ram_port lock init err.\n");
        goto out1;
    }
    osal_atomic_set(&new_ram_port->base.ref_count, 1);
    new_ram_port->base.ops     = get_dmx_rbase_ops();
    new_ram_port->base.release = dmx_mgmt_destroy_ram_port;
    new_ram_port->base.id      = id;
    new_ram_port->base.mgmt    = mgmt;

    new_ram_port->ops = &g_dmx_ram_port_ops;
    new_ram_port->handle = HI_INVALID_HANDLE;
    new_ram_port->staled = HI_FALSE;

    osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
    list_add_tail(&new_ram_port->node, &mgmt->ram_port_head);
    osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);

    new_ram_port->secure_mode = attrs->secure_mode;
    new_ram_port->max_data_rate  = attrs->max_data_rate;

    new_ram_port->buf_handle   = HI_INVALID_HANDLE;
    new_ram_port->buf_obj      = HI_NULL;
    new_ram_port->buf_ker_addr = HI_NULL;
    new_ram_port->buf_usr_addr = HI_NULL;
    new_ram_port->buf_phy_addr = 0;
    new_ram_port->pkt_size     = DMX_TS_PKT_SIZE;
    new_ram_port->blk_size     = new_ram_port->pkt_size * DMX_BLK_DEFAULT_PKT_NUM;
    new_ram_port->buf_size     = attrs->buffer_size;
    new_ram_port->crc_scb      = attrs->crc_scb;

    init_waitqueue_head(&new_ram_port->wait_queue);
    new_ram_port->wait_cond = 0;
    new_ram_port->wait_flush = 0;

    INIT_WORK(&new_ram_port->flow_control_worker, flow_control_handler);
    INIT_WORK(&new_ram_port->dsc_end_worker, dsc_end_irq_handler);

    *ram_port = new_ram_port;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_ram_port);

    set_bit(id, mgmt->ram_port_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_ram_port);
    new_ram_port = HI_NULL;

out:
    return ret;
}

static hi_s32 dmx_mgmt_create_ram_port_impl(struct dmx_mgmt *mgmt, dmx_port port, const dmx_ram_port_attr *attrs,
        struct dmx_r_ram_port **ram_port)
{
    hi_s32 ret = HI_FAILURE;
    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->ram_port_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_ram_port_impl(mgmt, port, attrs, ram_port);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->ram_port_list_lock);
    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_ram_port_lock_impl(struct dmx_r_ram_port *ram_port)
{
    osal_atomic_destory(&ram_port->base.ref_count);

    osal_mutex_destory(&ram_port->lock);
    osal_mutex_destory(&ram_port->blk_map_lock);
}

static hi_s32 _dmx_mgmt_destroy_ram_port_impl(struct dmx_mgmt *mgmt, struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;
    hi_size_t lock_flag;

    mask = BIT_MASK(ram_port->base.id);
    p = ((unsigned long *)mgmt->ram_port_bitmap) + BIT_WORD(ram_port->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("RAM port(%d) is invalid.\n", ram_port->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(ram_port->base.id, mgmt->ram_port_bitmap);
    _dmx_mgmt_destroy_ram_port_lock_impl(ram_port);
    osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
    list_del(&ram_port->node);
    osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_ram_port_impl(struct dmx_mgmt *mgmt, struct dmx_r_ram_port *ram_port)
{
    hi_s32 ret = HI_FAILURE;
    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->ram_port_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_ram_port_impl(mgmt, ram_port);
    if (ret == HI_SUCCESS) {
        /* clear and free the raw_pid_ch object */
        memset(ram_port, 0x0, sizeof(struct dmx_r_ram_port));
        HI_KFREE(HI_ID_DEMUX, ram_port);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  Rmx Pump interface
 */
static hi_s32 _dmx_mgmt_init_rmx_pump_lock_impl(struct dmx_r_rmx_pump *new_rrmx_pump)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rrmx_pump->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("rrmx_pump ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rrmx_pump->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("rrmx_pump lock init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_rmx_pump_impl(struct dmx_mgmt *mgmt, const dmx_rmx_pump_attrs *attrs,
        struct dmx_r_rmx_pump **rrmx_pump)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 pump_total_id;
    struct dmx_r_rmx_pump *new_rrmx_pump = HI_NULL;

    pump_total_id = find_first_zero_bit(mgmt->pump_total_bitmap, mgmt->pump_total_cnt);
    if (!(pump_total_id < mgmt->pump_total_cnt)) {
        HI_ERR_DEMUX("there is no available pump now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    new_rrmx_pump = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_rmx_pump), GFP_KERNEL);
    if (!new_rrmx_pump) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = _dmx_mgmt_init_rmx_pump_lock_impl(new_rrmx_pump);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("_dmx_mgmt_init_rmx_pump_lock_impl failed!\n");
        goto out1;
    }

    osal_atomic_set(&new_rrmx_pump->base.ref_count, 1);
    new_rrmx_pump->base.ops     = get_dmx_rbase_ops();
    new_rrmx_pump->base.release = dmx_mgmt_destroy_rmx_pump;
    new_rrmx_pump->base.mgmt    = mgmt;

    new_rrmx_pump->ops = &g_dmx_rmx_pump_ops;

    new_rrmx_pump->staled      = HI_FALSE;
    new_rrmx_pump->pump_total_id = pump_total_id;

    /* init the attribute of rmx_pump */
    new_rrmx_pump->pump_type = attrs->pump_type;
    new_rrmx_pump->in_port_id = attrs->in_port_id;
    new_rrmx_pump->pid      = attrs->pid;
    new_rrmx_pump->remap_pid = attrs->remap_pid;

    /* add the rmxpump node from mgmt */
    list_add_tail(&new_rrmx_pump->node0, &mgmt->pump_total_head);

    *rrmx_pump = new_rrmx_pump;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rrmx_pump);

    set_bit(pump_total_id, mgmt->pump_total_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rrmx_pump);
    new_rrmx_pump = HI_NULL;
out:
    return ret;
}

static hi_s32 dmx_mgmt_create_rmx_pump_impl(struct dmx_mgmt *mgmt, const dmx_rmx_pump_attrs *attrs,
        struct dmx_r_rmx_pump **rrmx_pump)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->pump_total_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_rmx_pump_impl(mgmt, attrs, rrmx_pump);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pump_total_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_rmx_pump_lock_impl(struct dmx_r_rmx_pump *rrmx_pump)
{
    osal_atomic_destory(&rrmx_pump->base.ref_count);

    osal_mutex_destory(&rrmx_pump->lock);
}

static hi_s32 _dmx_mgmt_destroy_rmx_pump_impl(struct dmx_mgmt *mgmt, struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;

    mask = BIT_MASK(rrmx_pump->pump_total_id);
    p = ((unsigned long *)mgmt->pump_total_bitmap) + BIT_WORD(rrmx_pump->pump_total_id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("pump_total_id(%d) is invalid.\n", rrmx_pump->pump_total_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    /* delete the rmxpump node from mgmt */
    clear_bit(rrmx_pump->pump_total_id, mgmt->pump_total_bitmap);
    _dmx_mgmt_destroy_rmx_pump_lock_impl(rrmx_pump);
    list_del(&rrmx_pump->node0);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_rmx_pump_impl(struct dmx_mgmt *mgmt, struct dmx_r_rmx_pump *rrmx_pump)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->pump_total_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_rmx_pump_impl(mgmt, rrmx_pump);
    if (ret == HI_SUCCESS) {
        /* clear and free the raw_pid_ch object */
        memset(rrmx_pump, 0x0, sizeof(struct dmx_r_rmx_pump));
        HI_KFREE(HI_ID_DEMUX, rrmx_pump);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pump_total_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  RmxFct interface
 */
static hi_s32 _dmx_mgmt_init_rmx_fct_lock_impl(struct dmx_r_rmx_fct *new_rrmx_fct)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rrmx_fct->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("rrmx_fct ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rrmx_fct->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("rrmx_fct lock init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rrmx_fct->pump_bitmap_lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("rrmx_fct pump_bitmap_lock init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_rmx_fct_impl(struct dmx_mgmt *mgmt, const dmx_rmx_attrs *attrs,
        struct dmx_r_rmx_fct **rrmx_fct)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id;
    struct dmx_r_rmx_fct *new_rrmx_fct = HI_NULL;

    id = find_first_zero_bit(mgmt->rmx_fct_bitmap, mgmt->rmx_fct_cnt);
    if (!(id < mgmt->rmx_fct_cnt)) {
        HI_ERR_DEMUX("there is no available rmxfct now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    /* new rmxfct */
    new_rrmx_fct = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_rmx_fct), GFP_KERNEL);
    if (!new_rrmx_fct) {
        HI_ERR_DEMUX("malloc recfct error!\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = _dmx_mgmt_init_rmx_fct_lock_impl(new_rrmx_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rrmx_fct lock init err.\n");
        goto out1;
    }

    /* init the base recfct */
    osal_atomic_set(&new_rrmx_fct->base.ref_count, 1);
    new_rrmx_fct->base.ops     = get_dmx_rbase_ops();
    new_rrmx_fct->base.release = dmx_mgmt_destroy_rmx_fct;
    new_rrmx_fct->base.id      = id;
    new_rrmx_fct->base.mgmt    = mgmt;

    new_rrmx_fct->ops        = &g_dmx_rmx_fct_ops;

    new_rrmx_fct->staled     = HI_FALSE;

    /* init the portinfo array */
    new_rrmx_fct->src_port_cnt = DMX_RMX_PORT_CNT;
    memset(new_rrmx_fct->port_info, 0x0, sizeof(new_rrmx_fct->port_info));
    bitmap_zero(new_rrmx_fct->port_bitmap, new_rrmx_fct->src_port_cnt);

    /* rmx pump object */
    new_rrmx_fct->pump_cnt = DMX_RMXPUMP_CNT;

    INIT_LIST_HEAD(&new_rrmx_fct->pump_head);
    bitmap_zero(new_rrmx_fct->pump_bitmap, new_rrmx_fct->pump_cnt);

    /* set the rmxfct attribute */
    new_rrmx_fct->out_port_id = attrs->out_port_id;
    new_rrmx_fct->out_port_cnt = mgmt->rmx_fct_cnt;

    list_add_tail(&new_rrmx_fct->node, &mgmt->rmx_fct_head);

    *rrmx_fct = new_rrmx_fct;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rrmx_fct);

    set_bit(id, mgmt->rmx_fct_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rrmx_fct);
    new_rrmx_fct = HI_NULL;
out:
    return ret;
}

static hi_s32 dmx_mgmt_create_rmx_fct_impl(struct dmx_mgmt *mgmt, const dmx_rmx_attrs *attrs,
        struct dmx_r_rmx_fct **rrmx_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->rmx_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_rmx_fct_impl(mgmt, attrs, rrmx_fct);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->rmx_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_rmx_fct_lock_impl(struct dmx_r_rmx_fct *rrmx_fct)
{
    osal_atomic_destory(&rrmx_fct->base.ref_count);

    osal_mutex_destory(&rrmx_fct->lock);
    osal_mutex_destory(&rrmx_fct->pump_bitmap_lock);
}

static hi_s32 _dmx_mgmt_destroy_rmx_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;

    mask = BIT_MASK(rrmx_fct->base.id);
    p = ((unsigned long *)mgmt->rmx_fct_bitmap) + BIT_WORD(rrmx_fct->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("rmx_fct(%d) is invalid.\n", rrmx_fct->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rrmx_fct->base.id, mgmt->rmx_fct_bitmap);
    _dmx_mgmt_destroy_rmx_fct_lock_impl(rrmx_fct);
    list_del(&rrmx_fct->node);

    if (!list_empty(&rrmx_fct->pump_head)) {
        WARN(1, "pump_head of rrmx_fct is not empty.\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_rmx_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_rmx_fct *rrmx_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->rmx_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_rmx_fct_impl(mgmt, rrmx_fct);
    if (ret == HI_SUCCESS) {
        /* free the recfct object */
        memset(rrmx_fct, 0x0, sizeof(struct dmx_r_rmx_fct));
        HI_KFREE(HI_ID_DEMUX, (hi_void *)rrmx_fct);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->rmx_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  Band Mgmt interface
 */
static hi_s32 _dmx_mgmt_init_band_lock_impl(struct dmx_r_band *new_rband)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rband->base.ref_count);
    if (ret != 0) {
        HI_ERR_DEMUX("ref_count init err.\n");
        return ret;
    }

    ret = osal_atomic_init(&new_rband->band_handle_ref_cnt);
    if (ret != 0) {
        HI_ERR_DEMUX("band_handle_ref_cnt init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rband->lock);
    if (ret != 0) {
        HI_ERR_DEMUX("new_rband mutex init err.\n");
        return ret;
    }

    return ret;
}

static hi_s32 _dmx_mgmt_create_band_impl(struct dmx_mgmt *mgmt, dmx_band band, const dmx_band_attr *attrs,
        struct dmx_r_band **rband)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id;
    struct dmx_r_band *new_rband = HI_NULL;

    if (band == DMX_ANY_BAND) { /* choose any available */
        id = find_first_zero_bit(mgmt->band_bitmap, mgmt->band_total_cnt);
        if (!(id < mgmt->band_total_cnt)) {
            HI_ERR_DEMUX("there is no available band now!\n");
            ret = HI_ERR_DMX_NO_RESOURCE;
            goto out;
        }
    } else if (band >= DMX_BAND_0 && band < DMX_ANY_BAND) {
        unsigned long mask, *p;

        id = band - DMX_BAND_0;

        mask = BIT_MASK(id);
        p = ((unsigned long *)mgmt->band_bitmap) + BIT_WORD(id);
        if (*p & mask) {
            HI_ERR_DEMUX("the band(%d) is busy now!.\n", band);
            ret = HI_ERR_DMX_BUSY;
            goto out;
        }
    } else {
        HI_ERR_DEMUX("invalid bandid[%#x]!\n", band);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    new_rband = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_band), GFP_KERNEL);
    if (!new_rband) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    if (_dmx_mgmt_init_band_lock_impl(new_rband) != HI_SUCCESS) {
        HI_ERR_DEMUX("_dmx_mgmt_init_band_lock_impl error!\n");
        goto out1;
    }

    osal_atomic_set(&new_rband->base.ref_count, 1);
    new_rband->base.ops     = get_dmx_rbase_ops();
    new_rband->base.release = dmx_mgmt_destroy_band;
    new_rband->base.id      = id;
    new_rband->base.mgmt    = mgmt;

    new_rband->ops = &g_dmx_band_ops;

    new_rband->staled = HI_FALSE;
    new_rband->port   = DMX_PORT_MAX;

    osal_atomic_set(&new_rband->band_handle_ref_cnt, 0);
    new_rband->band_handle = HI_INVALID_HANDLE;

    list_add_tail(&new_rband->node, &mgmt->band_head);

    *rband = new_rband;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rband);

    set_bit(id, mgmt->band_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rband);
    new_rband = HI_NULL;

out:
    return ret;
}

static hi_s32 dmx_mgmt_create_band_impl(struct dmx_mgmt *mgmt, dmx_band band, const dmx_band_attr *attrs,
                                        struct dmx_r_band **rband)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->band_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_band_impl(mgmt, band, attrs, rband);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->band_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_band_lock_impl(struct dmx_r_band *rband)
{
    osal_atomic_destory(&rband->base.ref_count);
    osal_atomic_destory(&rband->band_handle_ref_cnt);

    osal_mutex_destory(&rband->lock);
}

static hi_s32 _dmx_mgmt_destroy_band_impl(struct dmx_mgmt *mgmt, struct dmx_r_band *rband)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;

    mask = BIT_MASK(rband->base.id);
    p = ((unsigned long *)mgmt->band_bitmap) + BIT_WORD(rband->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("band(%d) is invalid.\n", rband->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rband->base.id, mgmt->band_bitmap);
    _dmx_mgmt_destroy_band_lock_impl(rband);
    list_del(&rband->node);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_band_impl(struct dmx_mgmt *mgmt, struct dmx_r_band *rband)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->band_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_band_impl(mgmt, rband);
    if (ret == HI_SUCCESS) {
        /* free the band object */
        memset(rband, 0x0, sizeof(struct dmx_r_band));
        HI_KFREE(HI_ID_DEMUX, rband);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->band_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  Pid Channel interface
 */
static hi_s32 _dmx_mgmt_init_pid_ch_lock_impl(struct dmx_r_pid_ch *new_rpid_ch)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rpid_ch->base.ref_count);
    if (ret != 0) {
        HI_ERR_DEMUX("base ref_count init err.\n");
        return ret;
    }

    ret = osal_atomic_init(&new_rpid_ch->pid_ch_handle_ref_cnt);
    if (ret != 0) {
        HI_ERR_DEMUX("pid_ch_handle_ref_cnt init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rpid_ch->lock);
    if (ret != 0) {
        HI_ERR_DEMUX("new_rpid_ch mutex init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rpid_ch->raw_pid_ch_list_lock);
    if (ret != 0) {
        HI_ERR_DEMUX("new_rpid_ch mutex init err.\n");
        return ret;
    }

    return ret;
}

static hi_s32 _dmx_mgmt_create_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_band *rband, const hi_u32 pid,
        struct dmx_r_pid_ch **rpid_ch)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id;
    hi_u32 raw_id;
    struct dmx_r_pid_ch *new_rpid_ch = HI_NULL;

    id = find_first_zero_bit(mgmt->pid_channel_bitmap, mgmt->pid_channel_cnt);
    if (!(id < mgmt->pid_channel_cnt)) {
        HI_ERR_DEMUX("there is no available pid channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    raw_id = find_first_zero_bit(mgmt->raw_pid_channel_bitmap, mgmt->raw_pid_channel_cnt);
    if (!(raw_id < mgmt->raw_pid_channel_cnt)) {
        HI_ERR_DEMUX("there is no available raw pid channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    new_rpid_ch = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_pid_ch), GFP_KERNEL);
    if (!new_rpid_ch) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    if (_dmx_mgmt_init_pid_ch_lock_impl(new_rpid_ch) != 0) {
        HI_ERR_DEMUX("_dmx_mgmt_init_pid_ch_lock_impl failed!\n");
        goto out1;
    }

    osal_atomic_set(&new_rpid_ch->base.ref_count, 1);
    new_rpid_ch->base.ops     = get_dmx_rbase_ops();
    new_rpid_ch->base.release = dmx_mgmt_destroy_pid_ch;
    new_rpid_ch->base.id      = id;
    new_rpid_ch->base.mgmt    = mgmt;
    new_rpid_ch->rband        = rband;
    new_rpid_ch->pid          = pid;

    new_rpid_ch->pid_copy_en       = HI_FALSE; /* init the pid_copy_en as HI_FALSE */
    new_rpid_ch->pid_copy_en_status = HI_FALSE; /* init the pid_copy_en_status as HI_FALSE */
    new_rpid_ch->master_raw_pid_ch_id = 0; /* init the master_raw_pid_ch as 0 */
    /* tmp raw pid channel state used in attach rawpid channel */
    new_rpid_ch->tmp_raw_pid_ch_state = DMX_RAWPIDCH_MAX_STATE;
    new_rpid_ch->pid_ch_handle = HI_INVALID_HANDLE;
    osal_atomic_set(&new_rpid_ch->pid_ch_handle_ref_cnt, 0);

    new_rpid_ch->ops = &g_dmx_pid_ch_ops;

    new_rpid_ch->staled = HI_FALSE;

    /* init the raw_pid_ch resource */
    new_rpid_ch->raw_pid_ch_cnt = DMX_RAW_PID_CHANNEL_CNT;

    INIT_LIST_HEAD(&new_rpid_ch->raw_pid_ch_head);
    bitmap_zero(new_rpid_ch->raw_pid_ch_bitmap, new_rpid_ch->raw_pid_ch_cnt);

    list_add_tail(&new_rpid_ch->node0, &mgmt->pid_channel_head);

    *rpid_ch = new_rpid_ch;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rpid_ch);

    set_bit(id, mgmt->pid_channel_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rpid_ch);
    new_rpid_ch = HI_NULL;

out:
    return ret;
}

static hi_s32 dmx_mgmt_create_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_band *rband, const hi_u32 pid,
        struct dmx_r_pid_ch **rpid_ch)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = NULL;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->pid_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    /* check the same pid on the same band */
    list_for_each(node, &mgmt->pid_channel_head) {
        struct dmx_r_pid_ch *rpid_ch_ptr = list_entry(node, struct dmx_r_pid_ch, node0);

        /* if (band_handle == rpid_ch->rband->band_handle && pid == rpid_ch->pid) */
        /* compare the object */
        if (rband == rpid_ch_ptr->rband && pid == rpid_ch_ptr->pid) {
            HI_ERR_DEMUX("don't allow to create same pid channle on the same band!\n");
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }
    }

    /* create the pidchannel object */
    ret = _dmx_mgmt_create_pid_ch_impl(mgmt, rband, pid, rpid_ch);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_pid_ch_lock_impl(struct dmx_r_pid_ch *rpid_ch)
{
    osal_atomic_init(&rpid_ch->pid_ch_handle_ref_cnt);
    osal_atomic_init(&rpid_ch->base.ref_count);

    osal_mutex_destory(&rpid_ch->lock);
    osal_mutex_destory(&rpid_ch->raw_pid_ch_list_lock);
}

static hi_s32 _dmx_mgmt_destroy_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;

    mask = BIT_MASK(rpid_ch->base.id);
    p = ((unsigned long *)mgmt->pid_channel_bitmap) + BIT_WORD(rpid_ch->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("pid_ch(%d) is invalid.\n", rpid_ch->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rpid_ch->base.id, mgmt->pid_channel_bitmap);
    _dmx_mgmt_destroy_pid_ch_lock_impl(rpid_ch);
    list_del(&rpid_ch->node0);

    ret = HI_SUCCESS;

out:

    return ret;
}

static hi_s32 dmx_mgmt_destroy_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_pid_ch *rpid_ch)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->pid_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_pid_ch_impl(mgmt, rpid_ch);
    if (ret == HI_SUCCESS) {
        /* free the pidch object */
        memset(rpid_ch, 0x0, sizeof(struct dmx_r_pid_ch));
        HI_KFREE(HI_ID_DEMUX, rpid_ch);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}


/*
 *  RawPid Channel interface
 */
static hi_s32 _dmx_mgmt_init_raw_pid_ch_lock_impl(struct dmx_r_raw_pid_ch *new_rraw_pid_ch)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rraw_pid_ch->base.ref_count);
    if (ret != 0) {
        HI_ERR_DEMUX("base ref_count init err.\n");
        return ret;
    }

    ret = osal_atomic_init(&new_rraw_pid_ch->ref_play_cnt);
    if (ret != 0) {
        HI_ERR_DEMUX("ref_play_cnt init err.\n");
        return ret;
    }

    ret = osal_atomic_init(&new_rraw_pid_ch->ref_rec_cnt);
    if (ret != 0) {
        HI_ERR_DEMUX("ref_rec_cnt init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rraw_pid_ch->lock);
    if (ret != 0) {
        HI_ERR_DEMUX("new_rraw_pid_ch mutex init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_raw_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_raw_pid_ch **rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 raw_pid_ch_id;

    struct dmx_r_raw_pid_ch *new_rraw_pid_ch = HI_NULL;

    /* alloc the actual raw pid channel id */
    raw_pid_ch_id = find_first_zero_bit(mgmt->raw_pid_channel_bitmap, mgmt->raw_pid_channel_cnt);
    if (!(raw_pid_ch_id < mgmt->raw_pid_channel_cnt)) {
        HI_ERR_DEMUX("there is no available raw pid channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    new_rraw_pid_ch = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_pid_ch), GFP_KERNEL);
    if (!new_rraw_pid_ch) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    if (_dmx_mgmt_init_raw_pid_ch_lock_impl(new_rraw_pid_ch) != HI_SUCCESS) {
        HI_ERR_DEMUX("_dmx_mgmt_init_raw_pid_ch_lock_impl failed!\n");
        goto out1;
    }

    osal_atomic_set(&new_rraw_pid_ch->base.ref_count, 1);
    new_rraw_pid_ch->base.ops     = get_dmx_rbase_ops();
    new_rraw_pid_ch->base.release = dmx_mgmt_destroy_raw_pid_ch;
    new_rraw_pid_ch->base.mgmt    = mgmt;

    new_rraw_pid_ch->raw_pid_chan_id = raw_pid_ch_id;
    new_rraw_pid_ch->rplay_fct      = HI_NULL;
    new_rraw_pid_ch->rrec_fct      = HI_NULL;
    osal_atomic_set(&new_rraw_pid_ch->ref_play_cnt, 0);
    osal_atomic_set(&new_rraw_pid_ch->ref_rec_cnt, 0);

    new_rraw_pid_ch->ops = &g_dmx_raw_pid_ch_ops;

    new_rraw_pid_ch->staled = HI_FALSE;
    new_rraw_pid_ch->force_unlive = HI_FALSE;

    list_add_tail(&new_rraw_pid_ch->node0, &mgmt->raw_pid_ch_total_head); /* add the node to mgmt rawpid list */

    *rraw_pid_ch = new_rraw_pid_ch;

    HI_DBG_DEMUX("#    rraw_pid_ch obj(0x%x) created.\n", new_rraw_pid_ch);

    /* set the actual raw pid channel id */
    set_bit(raw_pid_ch_id, mgmt->raw_pid_channel_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rraw_pid_ch);
    new_rraw_pid_ch = HI_NULL;

out:
    return ret;
}

static hi_s32 dmx_mgmt_create_raw_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_raw_pid_ch **rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->raw_pid_ch_total_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_raw_pid_ch_impl(mgmt, rraw_pid_ch);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->raw_pid_ch_total_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static inline hi_void _dmx_mgmt_destory_raw_pid_ch_lock_impl(struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    osal_atomic_destory(&rraw_pid_ch->ref_rec_cnt);
    osal_atomic_destory(&rraw_pid_ch->ref_play_cnt);
    osal_atomic_destory(&rraw_pid_ch->base.ref_count);

    osal_mutex_destory(&rraw_pid_ch->lock);
}

static hi_s32 _dmx_mgmt_destroy_raw_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;
    DMX_NULL_POINTER_GOTO(rraw_pid_ch, out);

    mask = BIT_MASK(rraw_pid_ch->raw_pid_chan_id);
    p = ((unsigned long *)mgmt->raw_pid_channel_bitmap) + BIT_WORD(rraw_pid_ch->raw_pid_chan_id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("raw_pid_ch(%d) is invalid.\n", rraw_pid_ch->raw_pid_chan_id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rraw_pid_ch->raw_pid_chan_id, mgmt->raw_pid_channel_bitmap);
    _dmx_mgmt_destory_raw_pid_ch_lock_impl(rraw_pid_ch);
    list_del(&rraw_pid_ch->node0); /* del the node from mgmt rawpid list */

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_raw_pid_ch_impl(struct dmx_mgmt *mgmt, struct dmx_r_raw_pid_ch *rraw_pid_ch)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();

    osal_mutex_lock(&mgmt->raw_pid_ch_total_list_lock);
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_raw_pid_ch_impl(mgmt, rraw_pid_ch);
    if (ret == HI_SUCCESS) {
        /* clear and free the raw_pid_ch object */
        memset(rraw_pid_ch, 0x0, sizeof(struct dmx_r_raw_pid_ch));
        HI_KFREE(HI_ID_DEMUX, rraw_pid_ch);
    }

out:
    osal_mutex_unlock(&mgmt->lock);
    osal_mutex_unlock(&mgmt->raw_pid_ch_total_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  playFct interface
 */
static hi_s32 _dmx_mgmt_init_play_fct_lock_impl(struct dmx_r_play_fct *new_rplay_fct)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rplay_fct->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rplay_fct ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rplay_fct->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rplay_fct mutex init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_play_fct_impl(struct dmx_mgmt *mgmt, const dmx_play_attrs *attrs,
    struct dmx_r_play_fct **rplay_fct)
{
    hi_s32 ret;
    hi_u32 id;
    struct dmx_r_play_fct *new_rplay_fct = HI_NULL;
    struct dmx_r_sub_play_fct *new_rsub_play_fct = HI_NULL;

    id = find_first_zero_bit(mgmt->play_fct_bitmap, mgmt->play_fct_cnt);
    if (!(id < mgmt->play_fct_cnt)) {
        HI_ERR_DEMUX("there is no available playfct now!\n");
        return HI_ERR_DMX_NO_RESOURCE;
    }

    /* new subplayfct */
    new_rsub_play_fct = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_sub_play_fct), GFP_KERNEL);
    if (!new_rsub_play_fct) {
        HI_ERR_DEMUX("malloc playfct error!\n");
        return HI_ERR_DMX_ALLOC_MEM_FAILED;
    }
    new_rplay_fct = &new_rsub_play_fct->play_fct_base;

    ret = _dmx_mgmt_init_play_fct_lock_impl(new_rplay_fct);
    if (ret != HI_SUCCESS) {
        HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rsub_play_fct);
        new_rsub_play_fct = HI_NULL;
        return ret;
    }

    /* init the base playfct */
    osal_atomic_set(&new_rplay_fct->base.ref_count, 1);
    new_rplay_fct->base.ops     = get_dmx_rbase_ops();
    new_rplay_fct->base.release = dmx_mgmt_destroy_play_fct;
    new_rplay_fct->base.id      = id;
    new_rplay_fct->base.mgmt    = mgmt;

    /* to protect the g_dmx_play_fct_ops not to be replace */
    memcpy(&new_rplay_fct->inner_ops, &g_dmx_play_fct_ops, sizeof(struct dmx_r_play_fct_ops));
    new_rplay_fct->ops = &new_rplay_fct->inner_ops;

    new_rplay_fct->staled               = HI_FALSE;
    new_rplay_fct->play_fct_type        = attrs->type;
    new_rplay_fct->crc_mode             = attrs->crc_mode;
    new_rplay_fct->live_play            = attrs->live_play;
    new_rplay_fct->data_mode            = attrs->data_mode;
    new_rplay_fct->vcodec_type          = attrs->vcodec_type;
    new_rplay_fct->secure_mode          = attrs->secure_mode;
    new_rplay_fct->rpid_ch              = HI_NULL;
    new_rplay_fct->callback.user_handle = HI_INVALID_HANDLE;
    new_rplay_fct->callback.cb          = HI_NULL;

    list_add_tail(&new_rplay_fct->node, &mgmt->play_fct_head);

    *rplay_fct = new_rplay_fct;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rplay_fct);

    set_bit(id, mgmt->play_fct_bitmap);

    return HI_SUCCESS;
}

static hi_s32 dmx_mgmt_create_play_fct_impl(struct dmx_mgmt *mgmt, const dmx_play_attrs *attrs,
        struct dmx_r_play_fct **rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->play_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_play_fct_impl(mgmt, attrs, rplay_fct);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->play_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_play_fct_lock_impl(struct dmx_r_play_fct *rplay_fct)
{
    osal_atomic_destory(&rplay_fct->base.ref_count);

    osal_mutex_destory(&rplay_fct->lock);
}

static hi_s32 _dmx_mgmt_destroy_play_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;

    mask = BIT_MASK(rplay_fct->base.id);
    p = ((unsigned long *)mgmt->play_fct_bitmap) + BIT_WORD(rplay_fct->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("pid_ch(%d) is invalid.\n", rplay_fct->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rplay_fct->base.id, mgmt->play_fct_bitmap);
    _dmx_mgmt_destroy_play_fct_lock_impl(rplay_fct);
    list_del(&rplay_fct->node);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_play_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_play_fct *rplay_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->play_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_play_fct_impl(mgmt, rplay_fct);
    if (ret == HI_SUCCESS) {
        /* free the subplayfct object, keep sync with _dmx_mgmt_create_play_fct_impl */
        memset(rplay_fct, 0x0, sizeof(struct dmx_r_sub_play_fct));
        HI_KFREE(HI_ID_DEMUX, (hi_void *)rplay_fct);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->play_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  RecFct interface
 */
static hi_s32 _dmx_mgmt_init_rec_fct_lock_impl(struct dmx_r_rec_fct *new_rrec_fct)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rrec_fct->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rrec_fct ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rrec_fct->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rrec_fct mutex init err.\n");
        return ret;
    }

    /* for select pid channel record */
    ret = osal_mutex_init(&new_rrec_fct->pid_ch_list_lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("pid_ch_list_lock mutex init err.\n");
        return ret;
    }

    /* for video index list */
    ret = osal_mutex_init(&new_rrec_fct->index_list_lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("index_list_lock mutex init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_rec_fct_impl(struct dmx_mgmt *mgmt, const dmx_rec_attrs *attrs,
        struct dmx_r_rec_fct **rrec_fct)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id;
    struct dmx_r_rec_fct *new_rrec_fct = HI_NULL;
    hi_u32 i;

    if (mgmt->rec_chan_used_cnt >= DMX_REC_CNT) {
        HI_ERR_DEMUX("there is no available recfct now, used(%u).\n", mgmt->rec_chan_used_cnt);
        ret = HI_ERR_DMX_NOFREE_CHAN;
        goto out;
    }

    id = find_first_zero_bit(mgmt->avr_bitmap, mgmt->av_pes_chan_cnt + mgmt->rec_chan_cnt);
    if (!(id < (mgmt->av_pes_chan_cnt + mgmt->rec_chan_cnt))) {
        HI_ERR_DEMUX("there is no available recfct now!\n");
        ret = HI_ERR_DMX_NOFREE_CHAN;
        goto out;
    }

    /* new recfct */
    new_rrec_fct = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_rec_fct), GFP_KERNEL);
    if (!new_rrec_fct) {
        HI_ERR_DEMUX("malloc recfct error!\n");
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = _dmx_mgmt_init_rec_fct_lock_impl(new_rrec_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rrec_fct ref init err.\n");
        goto out1;
    }

    /* init the base recfct */
    osal_atomic_set(&new_rrec_fct->base.ref_count, 1);
    new_rrec_fct->base.ops     = get_dmx_rbase_ops();
    new_rrec_fct->base.release = dmx_mgmt_destroy_rec_fct;
    new_rrec_fct->base.id      = id;
    new_rrec_fct->base.mgmt    = mgmt;

    new_rrec_fct->ops        = &g_dmx_rec_fct_ops;

    new_rrec_fct->staled     = HI_FALSE;
    /* for all pid record */
    new_rrec_fct->rband = HI_NULL;

    INIT_LIST_HEAD(&new_rrec_fct->index_list_head);
    new_rrec_fct->rec_index_thread = HI_NULL;
    new_rrec_fct->index_cnt = 0;
#if (DMX_REC_BUF_GAP_EXIST == 1)
    new_rrec_fct->last_scd_offset = 0;
    new_rrec_fct->ts_cnt_compensate = 0;
#endif
    /* set the recfct attribute */
    new_rrec_fct->descramed     = attrs->descramed;
    new_rrec_fct->index_src_pid = attrs->index_src_pid;
    new_rrec_fct->rec_type      = attrs->rec_type;
    new_rrec_fct->index_type    = attrs->index_type;
    new_rrec_fct->vcodec_type   = attrs->vcodec_type;
    new_rrec_fct->secure_mode   = attrs->secure_mode;
    new_rrec_fct->crc_scb       = attrs->crc_scb;

    new_rrec_fct->prev_scr_clk    = 0;
    new_rrec_fct->first_frame_ms  = 0;
    new_rrec_fct->prev_frame_ms   = 0;
    new_rrec_fct->cnt32to63helper = 0;
    if (attrs->ts_packet_type == DMX_TS_PACKET_192) {
        new_rrec_fct->rec_time_stamp  = DMX_REC_TIMESTAMP_LOW32BIT_SCR;
    } else {
        new_rrec_fct->rec_time_stamp  = DMX_REC_TIMESTAMP_NONE;;
    }
    bitmap_zero(new_rrec_fct->pid_bit_index, REC_MAX_PID_CNT);
    for (i = 0; i < REC_MAX_PID_CNT; i++) {
        new_rrec_fct->pid_chan_array[i] = HI_NULL;
    }
    memset(&new_rrec_fct->last_frame_info, 0x0, sizeof(dmx_index_data));

    list_add_tail(&new_rrec_fct->node, &mgmt->rec_fct_head);

    *rrec_fct = new_rrec_fct;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rrec_fct);

    set_bit(id, mgmt->avr_bitmap);
    mgmt->rec_chan_used_cnt++;

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rrec_fct);
    new_rrec_fct = HI_NULL;

out:
    return ret;
}

static hi_s32 dmx_mgmt_create_rec_fct_impl(struct dmx_mgmt *mgmt, const dmx_rec_attrs *attrs,
        struct dmx_r_rec_fct **rrec_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->avr_chan_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_rec_fct_impl(mgmt, attrs, rrec_fct);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->avr_chan_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_rec_fct_lock_impl(struct dmx_r_rec_fct *rrec_fct)
{
    osal_atomic_destory(&rrec_fct->base.ref_count);

    osal_mutex_destory(&rrec_fct->lock);
    osal_mutex_destory(&rrec_fct->pid_ch_list_lock);
    osal_mutex_destory(&rrec_fct->index_list_lock);
}

static hi_s32 _dmx_mgmt_destroy_rec_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;

    mask = BIT_MASK(rrec_fct->base.id);
    p = ((unsigned long *)mgmt->avr_bitmap) + BIT_WORD(rrec_fct->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("rec_fct(%d) is invalid.\n", rrec_fct->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rrec_fct->base.id, mgmt->avr_bitmap);
    mgmt->rec_chan_used_cnt--;
    _dmx_mgmt_destroy_rec_fct_lock_impl(rrec_fct);
    list_del(&rrec_fct->node);

    if (find_first_bit(rrec_fct->pid_bit_index, REC_MAX_PID_CNT) < REC_MAX_PID_CNT) {
        WARN(1, "pid_ch_array of rec_fct is not empty.\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_rec_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->avr_chan_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_rec_fct_impl(mgmt, rrec_fct);
    if (ret == HI_SUCCESS) {
        /* free the recfct object */
        memset(rrec_fct, 0x0, sizeof(struct dmx_r_rec_fct));
        HI_KFREE(HI_ID_DEMUX, (hi_void *)rrec_fct);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->avr_chan_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  DscFct mgmt interface
 */
static hi_s32 _dmx_mgmt_init_dsc_fct_lock_impl(struct dmx_r_dsc_fct *new_rdsc_fct)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rdsc_fct->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rdsc_fct ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rdsc_fct->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rdsc_fct lock init err.\n");
        return ret;
    }

    /* for attached pid channel */
    ret = osal_mutex_init(&new_rdsc_fct->pid_ch_list_lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rdsc_fct pid_ch_list_lock init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_dsc_fct_impl(struct dmx_mgmt *mgmt, const dmx_dsc_attrs *attrs,
        struct dmx_r_dsc_fct **rdsc_fct)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id = 0;
    struct dmx_r_dsc_fct *new_rdsc_fct = HI_NULL;
    ks_export_func *keyslot_func = HI_NULL;
    hi_handle keyslot_handle = HI_INVALID_HANDLE;

    new_rdsc_fct = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_dsc_fct), GFP_KERNEL);
    if (!new_rdsc_fct) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = _dmx_mgmt_init_dsc_fct_lock_impl(new_rdsc_fct);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* create key slot */
    if (attrs->keyslot_create_en == HI_TRUE) {
        ret = hi_drv_module_get_func(HI_ID_KEYSLOT, (hi_void **)&keyslot_func);
        if ((ret != HI_SUCCESS) || (keyslot_func == HI_NULL)) {
            HI_ERR_DEMUX("Get key slot export function failed. ret = %#x\n", ret);
            goto out1;
        }

        ret = keyslot_func->ext_ks_create(HI_KEYSLOT_TYPE_TSCIPHER, &keyslot_handle);
        if (ret != HI_SUCCESS) {
            /* HI_ERR_KS_AUTO_LOCK_FAILED means no more keyslot can be created */
            if (ret == HI_ERR_KS_AUTO_LOCK_FAILED) {
                ret = HI_ERR_DMX_NOFREE_KEY;
            }
            goto out1;
        }
        new_rdsc_fct->keyslot_handle  = keyslot_handle;
        /*
         * let keyslot_attached = HI_TRUE, keyslot can attach pid channel automatic,
         * when call _raw_pid_ch_attach_dsc.
         */
        new_rdsc_fct->keyslot_attached = HI_TRUE;
        id = KS_HANDLE_2_ID(keyslot_handle);
    } else {
        new_rdsc_fct->keyslot_handle  = HI_INVALID_HANDLE;
        new_rdsc_fct->keyslot_attached = HI_FALSE;

        /* if do not create keyslot,we also set dsc bitmap in case of the number of created dsc overflow */
        id = find_first_zero_bit(mgmt->dsc_fct_bitmap, mgmt->dsc_fct_cnt);
        if (!(id < mgmt->dsc_fct_cnt)) {
            HI_ERR_DEMUX("there is no available descrambler now!\n");
            ret = HI_ERR_DMX_NOFREE_KEY;
            goto out1;
        }
    }

    set_bit(id, mgmt->dsc_fct_bitmap);

    osal_atomic_set(&new_rdsc_fct->base.ref_count, 1);
    new_rdsc_fct->base.ops     = get_dmx_rbase_ops();
    new_rdsc_fct->base.release = dmx_mgmt_destroy_dsc_fct;
    new_rdsc_fct->base.id      = id;
    new_rdsc_fct->base.mgmt    = mgmt;

    new_rdsc_fct->ops = &g_dmx_dsc_fct_ops;

    new_rdsc_fct->staled     = HI_FALSE;

    INIT_LIST_HEAD(&new_rdsc_fct->pid_ch_head);

    /* dsc_fct attris */
    new_rdsc_fct->ca_type           = attrs->ca_type;
    new_rdsc_fct->entropy_reduction = attrs->entropy_reduction;
    new_rdsc_fct->alg               = attrs->alg;
    new_rdsc_fct->keyslot_create_en = attrs->keyslot_create_en;

    list_add_tail(&new_rdsc_fct->node, &mgmt->dsc_fct_head);

    *rdsc_fct = new_rdsc_fct;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rdsc_fct);

    return HI_SUCCESS;

out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rdsc_fct);
    new_rdsc_fct = HI_NULL;
out:
    return ret;
}

static hi_s32 dmx_mgmt_create_dsc_fct_impl(struct dmx_mgmt *mgmt, const dmx_dsc_attrs *attrs,
        struct dmx_r_dsc_fct **rdsc_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(attrs);
    DMX_NULL_POINTER_RETURN(rdsc_fct);

    osal_mutex_lock(&mgmt->dsc_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_dsc_fct_impl(mgmt, attrs, rdsc_fct);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->dsc_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_dsc_fct_lock_impl(struct dmx_r_dsc_fct *rdsc_fct)
{
    osal_atomic_destory(&rdsc_fct->base.ref_count);

    osal_mutex_destory(&rdsc_fct->lock);
    osal_mutex_destory(&rdsc_fct->pid_ch_list_lock);
}

static hi_s32 _dmx_mgmt_destroy_dsc_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_dsc_fct *rdsc_fct)
{
    hi_s32 ret = HI_FAILURE;
    ks_export_func *keyslot_func = HI_NULL;

    list_del(&rdsc_fct->node);

    if (!list_empty(&rdsc_fct->pid_ch_head)) {
        WARN(1, "pid_ch_head of rdsc_fct is not empty.\n");
        ret = HI_ERR_DMX_BUSY;
        goto out;
    }

    /* destroy key slot */
    if (rdsc_fct->keyslot_create_en == HI_TRUE) {
        if (rdsc_fct->keyslot_handle != HI_INVALID_HANDLE) {
            ret = hi_drv_module_get_func(HI_ID_KEYSLOT, (hi_void **)&keyslot_func);
            if ((ret != HI_SUCCESS) || (keyslot_func == HI_NULL)) {
                HI_ERR_DEMUX("Get key slot export function failed. ret = %#x\n", ret);
                goto out;
            }

            ret = keyslot_func->ext_ks_destory(HI_KEYSLOT_TYPE_TSCIPHER, rdsc_fct->keyslot_handle);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("keyslot destroy failed!.ret = %#x\n", ret);
                goto out;
            }

            rdsc_fct->keyslot_attached = HI_FALSE;
            rdsc_fct->keyslot_handle = HI_INVALID_HANDLE;
            rdsc_fct->keyslot_create_en = HI_FALSE;
        }
    }

    clear_bit(rdsc_fct->base.id, mgmt->dsc_fct_bitmap);
    rdsc_fct->base.id = 0;
    _dmx_mgmt_destroy_dsc_fct_lock_impl(rdsc_fct);
    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_dsc_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_dsc_fct *rdsc_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rdsc_fct);

    osal_mutex_lock(&mgmt->dsc_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_dsc_fct_impl(mgmt, rdsc_fct);
    if (ret == HI_SUCCESS) {
        memset(rdsc_fct, 0x0, sizeof(struct dmx_r_dsc_fct));
        HI_KFREE(HI_ID_DEMUX, (hi_void *)rdsc_fct);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->dsc_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  PcrFct mgmt interface
 */
static hi_s32 _dmx_mgmt_init_pcr_fct_lock_impl(struct dmx_r_pcr_fct *new_rpcr_fct)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rpcr_fct->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rpcr_fct ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rpcr_fct->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rpcr_fct lock init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_pcr_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_band *rband, hi_u32 pid,
        struct dmx_r_pcr_fct **rpcr_fct)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id;
    hi_size_t lock_flag;
    struct dmx_r_pcr_fct *new_rpcr_fct = HI_NULL;

    id = find_first_zero_bit(mgmt->pcr_fct_bitmap, mgmt->pcr_fct_cnt);
    if (!(id < mgmt->pcr_fct_cnt)) {
        HI_ERR_DEMUX("there is no available filter now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out;
    }

    new_rpcr_fct = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_pcr_fct), GFP_KERNEL);
    if (!new_rpcr_fct) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = _dmx_mgmt_init_pcr_fct_lock_impl(new_rpcr_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rpcr_fct ref init err.\n");
        goto out1;
    }
    osal_atomic_set(&new_rpcr_fct->base.ref_count, 1);
    new_rpcr_fct->base.ops              = get_dmx_rbase_ops();
    new_rpcr_fct->base.release          = dmx_mgmt_destroy_pcr_fct;
    new_rpcr_fct->base.id               = id;
    new_rpcr_fct->base.mgmt             = mgmt;

    new_rpcr_fct->ops                   = &g_dmx_pcr_fct_ops;

    new_rpcr_fct->staled                = HI_FALSE;

    new_rpcr_fct->rband                 = rband;
    new_rpcr_fct->port                  = rband->port;
    new_rpcr_fct->pid                   = pid;
    new_rpcr_fct->callback.user_handle  = HI_INVALID_HANDLE;
    new_rpcr_fct->callback.cb           = HI_NULL;

    osal_spin_lock_irqsave(&mgmt->pcr_fct_list_lock2, &lock_flag);
    list_add_tail(&new_rpcr_fct->node, &mgmt->pcr_fct_head);
    osal_spin_unlock_irqrestore(&mgmt->pcr_fct_list_lock2, &lock_flag);

    *rpcr_fct = new_rpcr_fct;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rpcr_fct);

    set_bit(id, mgmt->pcr_fct_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rpcr_fct);
    new_rpcr_fct = HI_NULL;
out:
    return ret;
}

static hi_s32 dmx_mgmt_create_pcr_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_band *rband, hi_u32 pid,
        struct dmx_r_pcr_fct **rpcr_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rpcr_fct);

    osal_mutex_lock(&mgmt->pcr_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_create_pcr_fct_impl(mgmt, rband, pid, rpcr_fct);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pcr_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_pcr_fct_lock_impl(struct dmx_r_pcr_fct *rpcr_fct)
{
    osal_atomic_destory(&rpcr_fct->base.ref_count);

    osal_mutex_destory(&rpcr_fct->lock);
}

static hi_s32 _dmx_mgmt_destroy_pcr_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_pcr_fct *rpcr_fct)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long mask, *p;
    hi_size_t lock_flag;

    mask = BIT_MASK(rpcr_fct->base.id);
    p = ((unsigned long *)mgmt->pcr_fct_bitmap) + BIT_WORD(rpcr_fct->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("rdsc(%d) is invalid.\n", rpcr_fct->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rpcr_fct->base.id, mgmt->pcr_fct_bitmap);
    _dmx_mgmt_destroy_pcr_fct_lock_impl(rpcr_fct);
    osal_spin_lock_irqsave(&mgmt->pcr_fct_list_lock2, &lock_flag);
    list_del(&rpcr_fct->node);
    osal_spin_unlock_irqrestore(&mgmt->pcr_fct_list_lock2, &lock_flag);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_pcr_fct_impl(struct dmx_mgmt *mgmt, struct dmx_r_pcr_fct *rpcr_fct)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rpcr_fct);

    osal_mutex_lock(&mgmt->pcr_fct_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    ret = _dmx_mgmt_destroy_pcr_fct_impl(mgmt, rpcr_fct);
    if (ret == HI_SUCCESS) {
        memset(rpcr_fct, 0x0, sizeof(struct dmx_r_pcr_fct));
        HI_KFREE(HI_ID_DEMUX, (hi_void *)rpcr_fct);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pcr_fct_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  Buf interface
 */
static hi_s32 _dmx_mgmt_init_buf_lock_impl(struct dmx_r_buf *new_rbuf)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rbuf->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rbuf ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rbuf->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rbuf lock init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rbuf->interrupt_lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rbuf interrupt_lock init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_buf_impl(struct dmx_mgmt *mgmt, struct dmx_buf_attrs *attrs,
    struct dmx_r_buf **rbuf)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id;
    hi_size_t lock_flag;
    struct dmx_r_buf *new_rbuf = HI_NULL;
    struct dmx_r_sub_buf *new_rsub_buf = HI_NULL;

#ifndef DMX_TEE_SUPPORT
    id = find_first_zero_bit(mgmt->buf_bitmap, mgmt->buf_cnt);
    if (!(id < mgmt->buf_cnt)) {
        HI_ERR_DEMUX("there is no available pid channel now!\n");
        ret = HI_ERR_DMX_NO_RESOURCE;
        goto out0;
    }
#else
    if (attrs->secure_mode != DMX_SECURE_TEE) {
        ret = dmx_tee_acquire_bufid(&id);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("acquire bufid failed!\n");
            goto out0;
        }
    }
#endif
    new_rsub_buf = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_sub_buf), GFP_KERNEL);
    if (new_rsub_buf == HI_NULL) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out1;
    }
    new_rbuf = &new_rsub_buf->buf_base;

    ret = _dmx_mgmt_init_buf_lock_impl(new_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rbuf ref init err.\n");
        goto out2;
    }
    /* init the base of buf */
    osal_atomic_set(&new_rbuf->base.ref_count, 1);
    new_rbuf->base.ops              = get_dmx_rbase_ops();
    new_rbuf->base.release          = dmx_mgmt_destroy_buf;
    new_rbuf->base.mgmt             = mgmt;

#ifndef DMX_TEE_SUPPORT
    new_rbuf->base.id               = id;
#else
    if (attrs->secure_mode != DMX_SECURE_TEE) {
        new_rbuf->base.id           = id;
    }
#endif

    /* to protect the g_dmx_buf_ops not to be replace */
    memcpy(&new_rbuf->inner_ops, &g_dmx_buf_ops, sizeof(struct dmx_r_buf_ops));
    new_rbuf->ops                   = &new_rbuf->inner_ops;

    new_rbuf->staled                = HI_FALSE;

    new_rbuf->buf_handle            = HI_INVALID_HANDLE;
    new_rbuf->buf_obj               = HI_NULL;
    new_rbuf->buf_ker_addr          = HI_NULL;
    new_rbuf->buf_usr_addr          = 0;
    new_rbuf->buf_phy_addr          = 0;
    /* buf size must be 4K align */
    new_rbuf->buf_size              = attrs->buf_size & ~0xFFFU;
    new_rbuf->real_buf_size         = new_rbuf->buf_size;
#if (DMX_REC_BUF_GAP_EXIST == 1)
    new_rbuf->pkt_size              = attrs->pkt_size;
#endif

    if (attrs->pes_ext_flag) {
        /* pes channel need to extend 64K buffer in the tail */
        new_rbuf->real_buf_size = new_rbuf->buf_size + DMX_PES_PACKAGE_MAX_LEN;
    }

    new_rbuf->secure_mode           = attrs->secure_mode;

    /* init waite queue */
    init_waitqueue_head(&new_rbuf->wait_queue);
    new_rbuf->wait_cond             = HI_FALSE;

    /* init work queue */
    INIT_WORK(&new_rbuf->buf_worker, buf_irq_handler);
    INIT_WORK(&new_rbuf->buf_ovfl_worker, buf_ovfl_irq_handler);
    new_rbuf->select_wait_queue_flag = HI_FALSE;

    /* interrupt th */
    new_rbuf->time_int_th           = 0;
    new_rbuf->ts_cnt_int_th         = 0;
    new_rbuf->seop_int_th           = 0;

    /* general */
    new_rbuf->sw_read               = 0;
    new_rbuf->last_addr             = 0;
    new_rbuf->last_read             = 0;
    new_rbuf->last_end              = 0;
    new_rbuf->rool_flag             = HI_FALSE;
    new_rbuf->eos_flag              = HI_FALSE;
    new_rbuf->eos_cnt               = 0;
    new_rbuf->eos_record_idx        = 0;
    new_rbuf->callback.user_handle  = HI_INVALID_HANDLE;
    new_rbuf->callback.cb           = HI_NULL;

    /* procfs debug info */
    new_rbuf->acquire_cnt           = 0;
    new_rbuf->acquire_ok_cnt        = 0;
    new_rbuf->release_cnt           = 0;

    osal_spin_lock_irqsave(&mgmt->buf_list_lock2, &lock_flag);
    list_add_tail(&new_rbuf->node, &mgmt->buf_head);
    osal_spin_unlock_irqrestore(&mgmt->buf_list_lock2, &lock_flag);

    *rbuf = new_rbuf;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rbuf);

#ifndef DMX_TEE_SUPPORT
    set_bit(id, mgmt->buf_bitmap);
#else
    if (attrs->secure_mode != DMX_SECURE_TEE) {
        set_bit(id, mgmt->buf_bitmap);
    }
#endif

    return HI_SUCCESS;
out2:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rsub_buf);
    new_rsub_buf = HI_NULL;

out1:
#ifdef DMX_TEE_SUPPORT
    if (attrs->secure_mode != DMX_SECURE_TEE) {
        dmx_tee_release_bufid(id);
    }
#endif

out0:
    return ret;
}

static hi_void _dmx_mgmt_destroy_buf_lock_impl(struct dmx_r_buf *rbuf)
{
    osal_atomic_destory(&rbuf->base.ref_count);

    osal_mutex_destory(&rbuf->lock);
    osal_mutex_destory(&rbuf->interrupt_lock);
}

static hi_s32 _dmx_mgmt_destroy_buf_impl(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    hi_size_t lock_flag;

#ifndef DMX_TEE_SUPPORT
    unsigned long mask;
    unsigned long *p = HI_NULL;

    mask = BIT_MASK(rbuf->base.id);
    p = ((unsigned long *)mgmt->buf_bitmap) + BIT_WORD(rbuf->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("buf(%d) is invalid.\n", rbuf->base.id);
        return HI_ERR_DMX_INVALID_PARA;
    }

    clear_bit(rbuf->base.id, mgmt->buf_bitmap);
#else
    if (rbuf->secure_mode != DMX_SECURE_TEE) {
        dmx_tee_release_bufid(rbuf->base.id);
    }
#endif
    _dmx_mgmt_destroy_buf_lock_impl(rbuf);

    osal_spin_lock_irqsave(&mgmt->buf_list_lock2, &lock_flag);
    list_del(&rbuf->node);
    osal_spin_unlock_irqrestore(&mgmt->buf_list_lock2, &lock_flag);

    return HI_SUCCESS;
}

static hi_s32 dmx_mgmt_create_buf_impl(struct dmx_mgmt *mgmt, struct dmx_buf_attrs *attrs, struct dmx_r_buf **rbuf)
{
    hi_s32 ret;
    hi_char buf_name[DMX_STR_LEN_32] = {0};
    struct dmx_r_buf *new_rbuf = HI_NULL;

    DMX_MGMT_TRACE_ENTER();

    osal_mutex_lock(&mgmt->buf_list_lock);
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_mgmt_create_buf_impl(mgmt, attrs, rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("_dmx_mgmt_create_buf_impl failed.\n");
        goto out;
    }

    new_rbuf = *rbuf;
    /* alloc buf */
    snprintf(buf_name, sizeof(buf_name), "dmx_ply_rec[%d]", new_rbuf->base.id);
    /* left the tee buffer alloc in playfct create */
    if (attrs->secure_mode != DMX_SECURE_TEE) { /* non secure buffer */
        dmx_mem_info play_rec_buf = {0};
        ret = dmx_alloc_and_map_buf(buf_name, new_rbuf->real_buf_size, 0x0, &play_rec_buf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("buf malloc 0x%x failed\n", new_rbuf->real_buf_size);
            if (_dmx_mgmt_destroy_buf_impl(mgmt, new_rbuf) == HI_SUCCESS) {
                /* free the subbuf object, keep sync with _dmx_mgmt_create_buf_impl */
                if (memset_s(new_rbuf, sizeof(struct dmx_r_sub_buf), 0x0, sizeof(struct dmx_r_sub_buf))) {
                    HI_ERR_DEMUX("memset_s buf failed!\n");
                }
                HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rbuf);
                new_rbuf = HI_NULL;
            }
            goto out;
        }
        WARN_ON(new_rbuf->real_buf_size != play_rec_buf.buf_size);

        new_rbuf->buf_handle   = play_rec_buf.handle;
        new_rbuf->buf_obj      = play_rec_buf.dmx_buf_obj;
        new_rbuf->buf_usr_addr = HI_NULL;
        new_rbuf->buf_ker_addr = play_rec_buf.buf_vir_addr;
        new_rbuf->buf_phy_addr = play_rec_buf.buf_phy_addr;
    }

out:
    osal_mutex_unlock(&mgmt->lock);
    osal_mutex_unlock(&mgmt->buf_list_lock);

    DMX_MGMT_TRACE_EXIT();

    return ret;
}

static hi_s32 dmx_mgmt_destroy_buf_impl(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf)
{
    hi_s32 ret;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->buf_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        ret = HI_ERR_DMX_STALED;
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    /* left tee buffer destroy to playfct destroy */
    if (rbuf->secure_mode != DMX_SECURE_TEE) { /* non secure buffer */
        dmx_mem_info play_rec_buf = {0};
        play_rec_buf.handle        = rbuf->buf_handle;
        play_rec_buf.dmx_buf_obj   = rbuf->buf_obj;
        play_rec_buf.buf_vir_addr  = rbuf->buf_ker_addr;
        play_rec_buf.buf_phy_addr  = rbuf->buf_phy_addr;
        play_rec_buf.buf_size      = rbuf->real_buf_size;
        play_rec_buf.user_map_flag = (rbuf->buf_usr_addr != HI_NULL) ? HI_TRUE : HI_FALSE;
        dmx_unmap_and_release_buf(&play_rec_buf);
    }

    ret = _dmx_mgmt_destroy_buf_impl(mgmt, rbuf);
    if (ret == HI_SUCCESS) {
        /* release the rbuf object */
        if (memset_s(rbuf, sizeof(struct dmx_r_sub_buf), 0x0, sizeof(struct dmx_r_sub_buf))) {
            HI_ERR_DEMUX("memset_s buf failed!\n");
        }
        HI_KFREE(HI_ID_DEMUX, (hi_void *)rbuf);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->buf_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

/*
 *  Flt mgmt interface
 */
static hi_s32 _dmx_mgmt_init_flt_lock_impl(struct dmx_r_flt *new_rflt)
{
    hi_s32 ret;

    ret = osal_atomic_init(&new_rflt->base.ref_count);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rflt ref init err.\n");
        return ret;
    }

    ret = osal_mutex_init(&new_rflt->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rflt lock init err.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 _dmx_mgmt_create_flt_impl(struct dmx_mgmt *mgmt, const dmx_filter_attrs *attrs,
    struct dmx_r_flt **rflt)
{
    hi_s32 ret;
    hi_u32 id;
    struct dmx_r_flt *new_rflt = HI_NULL;

    id = find_first_zero_bit(mgmt->flt_bitmap, mgmt->flt_cnt);
    if (!(id < mgmt->flt_cnt)) {
        HI_ERR_DEMUX("there is no available filter now!\n");
        ret = HI_ERR_DMX_NOFREE_FILTER;
        goto out;
    }

    new_rflt = HI_KZALLOC(HI_ID_DEMUX, sizeof(struct dmx_r_flt), GFP_KERNEL);
    if (!new_rflt) {
        ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
        goto out;
    }

    ret = _dmx_mgmt_init_flt_lock_impl(new_rflt);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("new_rflt ref init err.\n");
        goto out1;
    }
    osal_atomic_set(&new_rflt->base.ref_count, 1);
    new_rflt->base.ops     = get_dmx_rbase_ops();
    new_rflt->base.release = dmx_mgmt_destroy_flt;
    new_rflt->base.id      = id;
    new_rflt->base.mgmt    = mgmt;

    new_rflt->ops = &g_dmx_flt_ops;

    new_rflt->staled = HI_FALSE;
    new_rflt->rplay_fct = HI_NULL;
    new_rflt->flt_handle = HI_INVALID_HANDLE;

    new_rflt->pes_sec_id = DMX_PES_SEC_CHAN_CNT;
    new_rflt->attr_status = DMX_FLT_ATTR_MAX;
    new_rflt->crc_mode = DMX_FLT_CRC_MODE_MAX;
    new_rflt->depth = attrs->depth;
    memcpy(new_rflt->match, attrs->match, sizeof(new_rflt->match));
    memcpy(new_rflt->mask, attrs->mask, sizeof(new_rflt->mask));
    memcpy(new_rflt->negate, attrs->negate, sizeof(new_rflt->negate));

    list_add_tail(&new_rflt->node0, &mgmt->flt_head);

    *rflt = new_rflt;

    HI_DBG_DEMUX("#    robj(0x%x) created.\n", new_rflt);

    set_bit(id, mgmt->flt_bitmap);

    return HI_SUCCESS;
out1:
    HI_KFREE(HI_ID_DEMUX, (hi_void *)new_rflt);
    new_rflt = HI_NULL;

out:
    return ret;
}

static hi_s32 dmx_mgmt_create_flt_impl(struct dmx_mgmt *mgmt, const dmx_filter_attrs *attrs, struct dmx_r_flt **rflt)
{
    hi_s32 ret;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->flt_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_mgmt_create_flt_impl(mgmt, attrs, rflt);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->flt_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_destroy_flt_lock_impl(struct dmx_r_flt *rflt)
{
    osal_atomic_destory(&rflt->base.ref_count);

    osal_mutex_destory(&rflt->lock);
}

static hi_s32 _dmx_mgmt_destroy_flt_impl(struct dmx_mgmt *mgmt, struct dmx_r_flt *rflt)
{
    hi_s32 ret;
    unsigned long mask;
    unsigned long *p = HI_NULL;

    mask = BIT_MASK(rflt->base.id);
    p = ((unsigned long *)mgmt->flt_bitmap) + BIT_WORD(rflt->base.id);
    if (!(*p & mask)) {
        HI_ERR_DEMUX("rflt(%d) is invalid.\n", rflt->base.id);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    clear_bit(rflt->base.id, mgmt->flt_bitmap);
    _dmx_mgmt_destroy_flt_lock_impl(rflt);
    list_del(&rflt->node0);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_mgmt_destroy_flt_impl(struct dmx_mgmt *mgmt, struct dmx_r_flt *rflt)
{
    hi_s32 ret;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->flt_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        ret = HI_ERR_DMX_STALED;
        goto out;
    }

    ret = _dmx_mgmt_destroy_flt_impl(mgmt, rflt);
    if (ret == HI_SUCCESS) {
        /* release the rflt object */
        memset(rflt, 0x0, sizeof(struct dmx_r_flt));
        HI_KFREE(HI_ID_DEMUX, (hi_void *)rflt);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->flt_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_get_free_flt_cnt_impl(struct dmx_mgmt *mgmt, hi_u32 *free_cnt)
{
    struct list_head *node = HI_NULL;
    hi_u32 used_cnt = 0;

    list_for_each(node, &(mgmt->flt_head)) {
        used_cnt++;
    }

    *free_cnt = mgmt->flt_cnt - used_cnt;

    return;
}

static hi_s32 dmx_mgmt_get_free_flt_cnt_impl(struct dmx_mgmt *mgmt, hi_u32 *free_cnt)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();
    osal_mutex_lock(&mgmt->flt_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }

    _dmx_mgmt_get_free_flt_cnt_impl(mgmt, free_cnt);
    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->flt_list_lock);

    DMX_MGMT_TRACE_EXIT();
    return ret;
}

static hi_void _dmx_mgmt_get_free_rawch_cnt_impl(struct dmx_mgmt *mgmt, hi_u32 *free_cnt)
{
    struct list_head *node = HI_NULL;
    hi_u32 used_cnt = 0;

    list_for_each(node, &(mgmt->raw_pid_ch_total_head)) {
        used_cnt++;
    }

    *free_cnt = mgmt->raw_pid_channel_cnt - used_cnt;

    return;
}

static hi_s32 dmx_mgmt_get_free_ch_cnt_impl(struct dmx_mgmt *mgmt, hi_u32 *free_cnt)
{
    hi_s32 ret = HI_FAILURE;

    DMX_MGMT_TRACE_ENTER();

    osal_mutex_lock(&mgmt->raw_pid_ch_total_list_lock);
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");

        osal_mutex_unlock(&mgmt->lock);
        osal_mutex_unlock(&mgmt->raw_pid_ch_total_list_lock);

        return ret;
    }

    _dmx_mgmt_get_free_rawch_cnt_impl(mgmt, free_cnt);

    osal_mutex_unlock(&mgmt->lock);
    osal_mutex_unlock(&mgmt->raw_pid_ch_total_list_lock);

    ret = HI_SUCCESS;

    DMX_MGMT_TRACE_EXIT();

    return ret;
}

static hi_void dmx_mgmt_show_info_impl(struct dmx_mgmt *mgmt)
{
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("mgmt has not opened.\n");
        goto out;
    }
    if (!list_empty(&mgmt->ram_port_head)) {
        HI_ERR_DEMUX("ram_port_head is not empty.\n");
    }
    if (!list_empty(&mgmt->rmx_fct_head)) {
        HI_ERR_DEMUX("rmx_fct_head is not empty.\n");
    }
    if (!list_empty(&mgmt->pump_total_head)) {
        HI_ERR_DEMUX("pump_total_head is not empty.\n");
    }
    if (!list_empty(&mgmt->band_head)) {
        HI_ERR_DEMUX("band_head is not empty.\n");
    }
    if (!list_empty(&mgmt->pid_channel_head)) {
        HI_ERR_DEMUX("pid_channel_head is not empty.\n");
    }
    if (!list_empty(&mgmt->play_fct_head)) {
        HI_ERR_DEMUX("play_fct_head is not empty.\n");
    }
    if (!list_empty(&mgmt->rec_fct_head)) {
        HI_ERR_DEMUX("rec_fct_head is not empty.\n");
    }
    if (!list_empty(&mgmt->dsc_fct_head)) {
        HI_ERR_DEMUX("dsc_fct_head is not empty.\n");
    }
    if (!list_empty(&mgmt->pcr_fct_head)) {
        HI_ERR_DEMUX("pcr_fct_head is not empty.\n");
    }
    if (!list_empty(&mgmt->raw_pid_ch_total_head)) {
        HI_ERR_DEMUX("raw_pid_ch_total_head is not empty.\n");
    }
    if (!list_empty(&mgmt->buf_head)) {
        HI_ERR_DEMUX("buf_head is not empty.\n");
    }

    if (!list_empty(&mgmt->flt_head)) {
        HI_ERR_DEMUX("flt_head is not empty.\n");
    }

out:
    osal_mutex_unlock(&mgmt->lock);
}

static struct dmx_mgmt_ops g_dmx_mgmt_ops = {
    .init                = dmx_mgmt_init_impl,
    .exit                = dmx_mgmt_exit_impl,

    .get_cap             = dmx_mgmt_get_cap_impl,

    .suspend             = dmx_mgmt_suspend_impl,
    .resume              = dmx_mgmt_resume_impl,

    .create_ram_port     = dmx_mgmt_create_ram_port_impl,
    .destroy_ram_port    = dmx_mgmt_destroy_ram_port_impl,

    .create_rmx_pump     = dmx_mgmt_create_rmx_pump_impl,
    .destroy_rmx_pump    = dmx_mgmt_destroy_rmx_pump_impl,

    .create_rmx_fct      = dmx_mgmt_create_rmx_fct_impl,
    .destroy_rmx_fct     = dmx_mgmt_destroy_rmx_fct_impl,

    .create_band         = dmx_mgmt_create_band_impl,
    .destroy_band        = dmx_mgmt_destroy_band_impl,

    .create_pid_ch       = dmx_mgmt_create_pid_ch_impl,
    .destroy_pid_ch      = dmx_mgmt_destroy_pid_ch_impl,

    .create_raw_pid_ch   = dmx_mgmt_create_raw_pid_ch_impl,
    .destroy_raw_pid_ch  = dmx_mgmt_destroy_raw_pid_ch_impl,

    .create_play_fct     = dmx_mgmt_create_play_fct_impl,
    .destroy_play_fct    = dmx_mgmt_destroy_play_fct_impl,

    .create_rec_fct      = dmx_mgmt_create_rec_fct_impl,
    .destroy_rec_fct     = dmx_mgmt_destroy_rec_fct_impl,

    .create_dsc_fct      = dmx_mgmt_create_dsc_fct_impl,
    .destroy_dsc_fct     = dmx_mgmt_destroy_dsc_fct_impl,

    .create_pcr_fct      = dmx_mgmt_create_pcr_fct_impl,
    .destroy_pcr_fct     = dmx_mgmt_destroy_pcr_fct_impl,

    .create_buf          = dmx_mgmt_create_buf_impl,
    .destroy_buf         = dmx_mgmt_destroy_buf_impl,

    .create_flt          = dmx_mgmt_create_flt_impl,
    .destroy_flt         = dmx_mgmt_destroy_flt_impl,

    .get_free_flt_cnt    = dmx_mgmt_get_free_flt_cnt_impl,
    .get_free_ch_cnt     = dmx_mgmt_get_free_ch_cnt_impl,

    /* tee releated */
    .send_cmd_to_ta      = dmx_mgmt_send_cmd_to_ta,

    .show_info           = dmx_mgmt_show_info_impl,
};

static struct dmx_mgmt g_dmx_mgmt = {
    .state     = DMX_MGMT_CLOSED,
    .ops       = &g_dmx_mgmt_ops,
};

struct dmx_mgmt *_get_dmx_mgmt(hi_void)
{
    return &g_dmx_mgmt;
}

/*
 * it needs to be guaranteed that DmxMgmt has inited here.
 */
struct dmx_mgmt *get_dmx_mgmt(hi_void)
{
    struct dmx_mgmt *mgmt = _get_dmx_mgmt();

    WARN_ON(osal_atomic_read(&mgmt->ref_count) == 0);

    return mgmt;
}

/* some general code for dmx   */
hi_s32 dmx_get_port_type_and_id(const dmx_port port, enum dmx_port_type_e *port_mode, hi_u32 *port_id)
{
    hi_s32 ret;
    dmx_capability cap_info;

    ret = hi_drv_dmx_get_capability(&cap_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get demux capability info failed.\n");
        return ret;
    }

    if ((port >= DMX_TSI_PORT_0) && (port <= DMX_TSI_PORT_7)) {
        hi_u32 id = (hi_u32)port - (hi_u32)DMX_TSI_PORT_0;

        if (id < cap_info.tsi_port_num) {
            *port_mode   = DMX_PORT_TSI_TYPE;
            *port_id     = id;

            return HI_SUCCESS;
        }
    } else if ((port >= DMX_RAM_PORT_0) && (port <= DMX_RAM_PORT_15)) {
        hi_u32 id = (hi_u32)port - (hi_u32)DMX_RAM_PORT_0;

        if (id < cap_info.ram_port_num) {
            *port_mode   = DMX_PORT_RAM_TYPE;
            *port_id     = id;

            return HI_SUCCESS;
        }
    } else if ((port >= DMX_TAG_PORT_0) && (port <= DMX_TAG_PORT_7)) {
        hi_u32 id = (hi_u32)port - (hi_u32)DMX_TAG_PORT_0;

        if (id < cap_info.tag_port_num) {
            *port_mode   = DMX_PORT_TAG_TYPE;
            *port_id     = id;

            return HI_SUCCESS;
        }
    } else if ((port >= DMX_RMX_PORT_0) && (port <= DMX_RMX_PORT_15)) {
        hi_u32 id = (hi_u32)port - (hi_u32)DMX_RMX_PORT_0;

        if (id < cap_info.tag_port_num) {
            *port_mode   = DMX_PORT_RMX_TYPE;
            *port_id     = id;

            return HI_SUCCESS;
        }
    } else if ((port >= DMX_TSIO_PORT_0) && (port <= DMX_TSIO_PORT_1)) {
        hi_u32 id = (hi_u32)port - (hi_u32)DMX_TSIO_PORT_0;

        if (id < cap_info.tsio_port_num) {
            *port_mode   = DMX_PORT_TSIO_TYPE;
            *port_id     = id;

            return HI_SUCCESS;
        }
    } else if ((port >= DMX_IF_PORT_0) && (port <= DMX_IF_PORT_1)) {
        hi_u32 id = (hi_u32)port - (hi_u32)DMX_IF_PORT_0;

        if (id < cap_info.if_port_num) {
            *port_mode   = DMX_PORT_IF_TYPE;
            *port_id     = id;

            return HI_SUCCESS;
        }
    }

    HI_ERR_DEMUX("invalid unknown port 0x%x\n", port);

    return HI_ERR_DMX_INVALID_PARA;
}

#ifdef HI_DEMUX_PROC_SUPPORT

static struct file *g_dmx_ram_port_ts_handle = HI_NULL;
static hi_u32 g_dmx_ram_port_id = 0;

hi_void dmx_ram_port_save_ip_ts(struct dmx_r_ram_port *ram_port, hi_u8 *buf, hi_u32 len, hi_u32 port_id)
{
    if (g_dmx_ram_port_ts_handle && (g_dmx_ram_port_id - DMX_RAM_PORT_0) == port_id) {
        if (ram_port->secure_mode == DMX_SECURE_TEE) {
            HI_ERR_DEMUX("not support saving ts data from secure buffer.\n");
        } else {
            hi_drv_file_write(g_dmx_ram_port_ts_handle, buf, len);
        }
    }
}

hi_s32 dmx_ram_port_start_save_ip_ts(hi_u32 port)
{
    hi_s32 ret;
    hi_s8 str[DMX_FILE_NAME_LEN];
    struct dmx_mgmt *rmgmt = HI_NULL;

    if (g_dmx_ram_port_ts_handle != HI_NULL) {
        HI_ERR_DEMUX("already started\n");
        return HI_FAILURE;
    }

    rmgmt = _get_dmx_mgmt();
    if (port < DMX_RAM_PORT_0 || port > DMX_RAM_PORT_0 + rmgmt->ram_port_cnt) {
        HI_ERR_DEMUX("invalid port id:%d\n", port);
        return HI_FAILURE;
    }

    if (hi_drv_file_get_store_path(str, DMX_FILE_NAME_LEN) != HI_SUCCESS) {
        HI_ERR_DEMUX("get path failed\n");
        return HI_FAILURE;
    }

    g_dmx_ram_port_id = port;

    if (g_dmx_ram_port_ts_handle == HI_NULL) {
        struct tm now;
        time_to_tm(get_seconds(), 0, &now);

        ret = snprintf_s(str, sizeof(str), sizeof(str) - 1, "%s/dmx_ram_%u-%02u_%02u_%02u.ts",
                         str, port, now.tm_hour, now.tm_min, now.tm_sec);
        if (ret < 0) {
            HI_ERR_DEMUX("snprintf_s failed with ret:0x%x\n", ret);
            return HI_FAILURE;
        }

        g_dmx_ram_port_ts_handle = hi_drv_file_open(str, 1);
        if (!g_dmx_ram_port_ts_handle) {
            HI_ERR_DEMUX("open %s error\n", str);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_void dmx_ram_port_stop_save_ip_ts(hi_void)
{
    if (g_dmx_ram_port_ts_handle) {
        hi_drv_file_close(g_dmx_ram_port_ts_handle);
        g_dmx_ram_port_ts_handle = HI_NULL;
        g_dmx_ram_port_id = 0;
    }
}
#endif /* HI_DEMUX_PROC_SUPPORT */

