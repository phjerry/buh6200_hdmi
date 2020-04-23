/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio basic function impl.
* Author: guoqingbo
* Create: 2016-08-12
*/

#include "linux/hisilicon/securec.h"
#include "linux/ion.h"

#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_drv_osal.h"
#include "hi_drv_demux.h"
#include "hi_errno.h"
#include "hi_drv_tsio.h"

#include "drv_tsio_func.h"
#include "drv_tsio_utils.h"
#include "hal_tsio.h"
#include "drv_tsio_define.h"
#include "drv_tsio_reg.h"

static struct tsio_r_tsi_port_ops g_tsio_tsi_port_ops;

#define TSIO_HIGH_PHY_ADDR_LEN 4
#define ALIGN_LEN             10

static hi_s32 tsio_r_tsi_port_get(hi_handle handle, struct tsio_r_tsi_port **tsi_port)
{
    hi_s32 ret;
    ret = tsio_r_get(handle, (struct tsio_r_base **)tsi_port);
    if (ret == HI_SUCCESS) {
        if (&g_tsio_tsi_port_ops != (*tsi_port)->ops) {
            tsio_r_put((struct tsio_r_base*)(*tsi_port));
            HI_ERR_TSIO("handle is in active, but not a valid tsi_port handle(0x%x).\n", handle);
            ret = HI_ERR_TSIO_INVALID_PARA;
        }
    }
    return ret;
}

hi_s32 tsio_tsi_open_port(tsio_port port, const tsio_tsi_port_attrs *attrs, hi_handle *handle,
                          struct tsio_session *session)
{
    hi_s32 ret;
    struct tsio_slot  *slot = HI_NULL;
    struct tsio_r_tsi_port *tsi_port = HI_NULL;

    ret = tsio_mgmt_create_tsi_port(port, attrs, &tsi_port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    tsio_r_get_raw((struct tsio_r_base *)tsi_port);

    ret = tsi_port->ops->open(tsi_port);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = tsio_slot_create((struct tsio_r_base *)tsi_port, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsio_tsi_close_port;
    ret = tsio_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    tsio_r_put((struct tsio_r_base *)tsi_port);

    return HI_SUCCESS;

out3:
    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }
out2:
    tsi_port->ops->close(tsi_port);
out1:
    tsio_r_put((struct tsio_r_base *)tsi_port);

    tsio_r_put((struct tsio_r_base *)tsi_port);
out0:
    return ret;
}

hi_s32 tsio_tsi_get_port_attrs(hi_handle handle, tsio_tsi_port_attrs *attrs)
{
    hi_s32 ret;
    struct tsio_r_tsi_port *tsi_port = HI_NULL;

    ret = tsio_r_tsi_port_get(handle, &tsi_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = tsi_port->ops->get_attrs(tsi_port, attrs);

    tsio_r_put((struct tsio_r_base *)tsi_port);
out:
    return ret;
}

hi_s32 tsio_tsi_set_port_attrs(hi_handle handle, const tsio_tsi_port_attrs *attrs)
{
    hi_s32 ret;
    struct tsio_r_tsi_port *tsi_port = HI_NULL;

    ret = tsio_r_tsi_port_get(handle, &tsi_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = tsi_port->ops->set_attrs(tsi_port, attrs);

    tsio_r_put((struct tsio_r_base *)tsi_port);
out:
    return ret;
}

hi_s32 tsio_tsi_get_port_status(hi_handle handle, tsio_tsi_port_status *status)
{
    hi_s32 ret;
    struct tsio_r_tsi_port *tsi_port = HI_NULL;

    ret = tsio_r_tsi_port_get(handle, &tsi_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = tsi_port->ops->get_status(tsi_port, status);

    tsio_r_put((struct tsio_r_base *)tsi_port);
out:
    return ret;
}

hi_s32 tsio_tsi_close_port(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_tsi_port *tsi_port = HI_NULL;

    ret = tsio_r_tsi_port_get(handle, &tsi_port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_session_del_slot(slot->session, slot);

    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }

    ret = tsi_port->ops->close(tsi_port);

    tsio_r_put((struct tsio_r_base *)tsi_port);
out1:
    tsio_r_put((struct tsio_r_base *)tsi_port);
out0:
    return ret;
}

static struct tsio_r_ram_port_ops g_tsio_ram_port_ops;

static hi_s32 tsio_r_ram_port_get(hi_handle handle, struct tsio_r_ram_port **ram_port)\
{
    hi_s32 ret;
    ret = tsio_r_get(handle, (struct tsio_r_base **)ram_port);
    if (ret == HI_SUCCESS) {
        if (&g_tsio_ram_port_ops != (*ram_port)->ops) {
            tsio_r_put((struct tsio_r_base*)(*ram_port));
            HI_ERR_TSIO("handle is in active, but not a valid ram_port handle(0x%x).\n", handle);
            ret = HI_ERR_TSIO_INVALID_PARA;
        }
    }
    return ret;
}

hi_s32 tsio_ram_open_port(tsio_port port, const tsio_ram_port_attrs_ex *attrs, hi_handle *handle,
                          struct tsio_session *session)
{
    hi_s32 ret;
    struct tsio_slot  *slot = HI_NULL;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_mgmt_create_ram_port(port, attrs, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    tsio_r_get_raw((struct tsio_r_base *)ram_port);

    ret = ram_port->ops->open(ram_port);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = tsio_slot_create((struct tsio_r_base *)ram_port, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsio_ram_close_port;
    ret = tsio_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    tsio_r_put((struct tsio_r_base *)ram_port);

    return HI_SUCCESS;

out3:
    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }
out2:
    ram_port->ops->close(ram_port);
out1:
    tsio_r_put((struct tsio_r_base *)ram_port);

    tsio_r_put((struct tsio_r_base *)ram_port);
out0:
    return ret;
}

hi_s32 tsio_ram_get_port_attrs(hi_handle handle, tsio_ram_port_attrs *attrs)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->get_attrs(ram_port, attrs);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_set_port_attrs(hi_handle handle, const tsio_ram_port_attrs_ex *attrs)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->set_attrs(ram_port, attrs);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_get_port_status(hi_handle handle, tsio_ram_port_status *status)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->get_status(ram_port, status);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_begin_bulk_seg(hi_handle handle, tsio_key_type key, const hi_u8 *IV)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->begin_bulk_seg(ram_port, key, IV);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_end_bulk_seg(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->end_bulk_seg(ram_port);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_get_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->get_buffer(ram_port, req_len, buf, time_out);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_push_buffer(hi_handle handle, tsio_buffer *buf)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->push_buffer(ram_port, buf);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_put_buffer(hi_handle handle, tsio_buffer *buf)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->put_buffer(ram_port, buf);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_reset_buffer(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->reset_buffer(ram_port);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_close_port(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_session_del_slot(slot->session, slot);

    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }

    ret = ram_port->ops->close(ram_port);

    tsio_r_put((struct tsio_r_base *)ram_port);
out1:
    tsio_r_put((struct tsio_r_base *)ram_port);
out0:
    return ret;
}

hi_s32 tsio_ram_pre_mmap(hi_handle handle, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->pre_mmap(ram_port, buf_handle, buf_size, usr_addr);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

hi_s32 tsio_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = HI_NULL;

    ret = tsio_r_ram_port_get(handle, &ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = ram_port->ops->pst_mmap(ram_port, buf_usr_addr);

    tsio_r_put((struct tsio_r_base *)ram_port);
out:
    return ret;
}

static struct tsio_r_pid_channel_ops g_tsio_pid_channel_ops;

static hi_s32 tsio_r_pid_get(hi_handle handle, struct tsio_r_pid_channel **pid_channel)
{
    hi_s32 ret;
    ret = tsio_r_get(handle, (struct tsio_r_base **)pid_channel);
    if (ret == HI_SUCCESS) {
        if (&g_tsio_pid_channel_ops != (*pid_channel)->ops) {
            tsio_r_put((struct tsio_r_base *)(*pid_channel));
            HI_ERR_TSIO("handle is in active, but not a valid pid_channel handle(0x%x).\n", handle);
            ret = HI_ERR_TSIO_INVALID_PARA;
        }
    }
    return ret;
}

hi_s32 tsio_pid_create_channel(hi_handle pt_handle, hi_u32 pid, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_base *port = HI_NULL;
    struct tsio_r_pid_channel *pid_channel = HI_NULL;

    ret = tsio_r_get(pt_handle, (struct tsio_r_base **)&port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_mgmt_create_pid_channel(port, pid, &pid_channel);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_r_get_raw((struct tsio_r_base *)pid_channel);

    /* associate a slot to the object  */
    ret = tsio_slot_create((struct tsio_r_base *)pid_channel, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsio_pid_destroy_channel;
    ret = tsio_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    tsio_r_put((struct tsio_r_base *)pid_channel);

    return HI_SUCCESS;

out3:
    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }
out2:
    tsio_r_put((struct tsio_r_base *)pid_channel);

    tsio_r_put((struct tsio_r_base *)pid_channel);
out1:
    tsio_r_put(port);
out0:
    return ret;
}

hi_s32 tsio_pid_get_channel_status(hi_handle handle, tsio_pid_channel_status *status)
{
    hi_s32 ret;
    struct tsio_r_pid_channel *pid_channel = HI_NULL;

    ret = tsio_r_pid_get(handle, &pid_channel);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = pid_channel->ops->get_status(pid_channel, status);

    tsio_r_put((struct tsio_r_base *)pid_channel);
out:
    return ret;
}

hi_s32 tsio_pid_destroy_channel(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_pid_channel *pid_channel = HI_NULL;

    ret = tsio_r_pid_get(handle, &pid_channel);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_session_del_slot(slot->session, slot);

    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }

    tsio_r_put((struct tsio_r_base *)pid_channel);
out1:
    tsio_r_put((struct tsio_r_base *)pid_channel);
out0:
    return ret;
}

static struct tsio_r_raw_channel_ops g_tsio_raw_channel_ops;

static hi_s32 tsio_r_raw_get(hi_handle handle, struct tsio_r_raw_channel **raw_channel)
{
    hi_s32 ret;
    ret = tsio_r_get(handle, (struct tsio_r_base**)raw_channel);
    if (ret == HI_SUCCESS) {
        if (&g_tsio_raw_channel_ops != (*raw_channel)->ops) {
            tsio_r_put((struct tsio_r_base*)(*raw_channel));
            HI_ERR_TSIO("handle is in active, but not a valid raw_channel handle(0x%x).\n", handle);
            ret = HI_ERR_TSIO_INVALID_PARA;
        }
    }
    return ret;
}

hi_s32 tsio_raw_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_base *port = HI_NULL;
    struct tsio_r_raw_channel *raw_channel = HI_NULL;

    ret = tsio_r_get(pt_handle, (struct tsio_r_base **)&port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_mgmt_create_raw_channel(port, &raw_channel);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_r_get_raw((struct tsio_r_base *)raw_channel);

    /* associate a slot to the object  */
    ret = tsio_slot_create((struct tsio_r_base *)raw_channel, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsio_raw_destroy_channel;
    ret = tsio_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    tsio_r_put((struct tsio_r_base *)raw_channel);

    return HI_SUCCESS;

out3:
    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }
out2:
    tsio_r_put((struct tsio_r_base *)raw_channel);

    tsio_r_put((struct tsio_r_base *)raw_channel);
out1:
    tsio_r_put(port);
out0:
    return ret;
}

hi_s32 tsio_raw_destroy_channel(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_raw_channel *raw_channel = HI_NULL;

    ret = tsio_r_raw_get(handle, &raw_channel);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_session_del_slot(slot->session, slot);

    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }

    tsio_r_put((struct tsio_r_base *)raw_channel);
out1:
    tsio_r_put((struct tsio_r_base *)raw_channel);
out0:
    return ret;
}


static struct tsio_r_sp_channel_ops g_tsio_sp_channel_ops;

static hi_s32 tsio_r_sp_get(hi_handle handle, struct tsio_r_sp_channel **sp_channel)
{
    hi_s32 ret;
    ret = tsio_r_get(handle, (struct tsio_r_base **)sp_channel);
    if (ret == HI_SUCCESS) {
        if (&g_tsio_sp_channel_ops != (*sp_channel)->ops) {
            tsio_r_put((struct tsio_r_base *)(*sp_channel));
            HI_ERR_TSIO("handle is in active, but not a valid sp_channel handle(0x%x).\n", handle);
            ret = HI_ERR_TSIO_INVALID_PARA;
        }
    }
    return ret;
}

hi_s32 tsio_sp_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_base *port = HI_NULL;
    struct tsio_r_sp_channel *sp_channel = HI_NULL;

    ret = tsio_r_get(pt_handle, (struct tsio_r_base **)&port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_mgmt_create_sp_channel(port, &sp_channel);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_r_get_raw((struct tsio_r_base *)sp_channel);

    /* associate a slot to the object  */
    ret = tsio_slot_create((struct tsio_r_base *)sp_channel, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsio_sp_destroy_channel;
    ret = tsio_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    tsio_r_put((struct tsio_r_base *)sp_channel);

    return HI_SUCCESS;

out3:
    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }
out2:
    tsio_r_put((struct tsio_r_base *)sp_channel);

    tsio_r_put((struct tsio_r_base *)sp_channel);
out1:
    tsio_r_put(port);
out0:
    return ret;
}

hi_s32 tsio_sp_destroy_channel(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_sp_channel *sp_channel = HI_NULL;

    ret = tsio_r_sp_get(handle, &sp_channel);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_session_del_slot(slot->session, slot);

    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }

    tsio_r_put((struct tsio_r_base *)sp_channel);
out1:
    tsio_r_put((struct tsio_r_base *)sp_channel);
out0:
    return ret;
}

static struct tsio_r_ivr_channel_ops g_tsio_ivr_channel_ops;

static hi_s32 tsio_r_ivr_get(hi_handle handle, struct tsio_r_ivr_channel **ivr_channel)
{
    hi_s32 ret;
    ret = tsio_r_get(handle, (struct tsio_r_base**)ivr_channel);
    if (ret == HI_SUCCESS) {
        if (&g_tsio_ivr_channel_ops != (*ivr_channel)->ops) {
            tsio_r_put((struct tsio_r_base*)(*ivr_channel));
            HI_ERR_TSIO("handle is in active, but not a valid ivr_channel handle(0x%x).\n", handle);
            ret = HI_ERR_TSIO_INVALID_PARA;
        }
    }
    return ret;
}

hi_s32 tsio_ivr_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_base *port = HI_NULL;
    struct tsio_r_ivr_channel *ivr_channel = HI_NULL;

    ret = tsio_r_get(pt_handle, (struct tsio_r_base **)&port);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_mgmt_create_ivr_channel(port, &ivr_channel);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_r_get_raw((struct tsio_r_base *)ivr_channel);

    /* associate a slot to the object  */
    ret = tsio_slot_create((struct tsio_r_base *)ivr_channel, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsio_ivr_destroy_channel;
    ret = tsio_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    tsio_r_put((struct tsio_r_base *)ivr_channel);

    return HI_SUCCESS;

out3:
    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }
out2:
    tsio_r_put((struct tsio_r_base *)ivr_channel);

    tsio_r_put((struct tsio_r_base *)ivr_channel);
out1:
    tsio_r_put(port);
out0:
    return ret;
}

hi_s32 tsio_ivr_destroy_channel(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_ivr_channel *ivr_channel = HI_NULL;

    ret = tsio_r_ivr_get(handle, &ivr_channel);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_session_del_slot(slot->session, slot);

    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }

    tsio_r_put((struct tsio_r_base *)ivr_channel);
out1:
    tsio_r_put((struct tsio_r_base *)ivr_channel);
out0:
    return ret;
}

static struct tsio_r_se_ops g_tsio_se_ops;

static hi_s32 tsio_r_se_get(hi_handle handle, struct tsio_r_se **se)
{
    hi_s32 ret;
    ret = tsio_r_get(handle, (struct tsio_r_base**)se);
    if (ret == HI_SUCCESS) {
        if (&g_tsio_se_ops != (*se)->ops) {
            tsio_r_put((struct tsio_r_base*)(*se));
            HI_ERR_TSIO("handle is in active, but not a valid se handle(0x%x).\n", handle);
            ret = HI_ERR_TSIO_INVALID_PARA;
        }
    }
    return ret;
}

hi_s32 tsio_se_create(tsio_sid sid, const tsio_secure_engine_attrs *attrs, hi_handle *handle,
                      struct tsio_session *session)
{
    hi_s32 ret;
    struct tsio_slot  *slot = HI_NULL;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_mgmt_create_se(sid, attrs, &se);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    tsio_r_get_raw((struct tsio_r_base *)se);

    ret = se->ops->pre_open(se);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = tsio_slot_create((struct tsio_r_base *)se, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = tsio_se_destroy;
    ret = tsio_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;

    tsio_r_put((struct tsio_r_base *)se);

    return HI_SUCCESS;

out3:
    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }
out2:
    se->ops->pst_close(se);
out1:
    tsio_r_put((struct tsio_r_base *)se);

    tsio_r_put((struct tsio_r_base *)se);
out0:
    return ret;
}

hi_s32 tsio_se_open(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->open(se);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_get_attrs(hi_handle handle, tsio_secure_engine_attrs *attrs)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->get_attrs(se, attrs);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_set_attrs(hi_handle handle, const tsio_secure_engine_attrs *attrs)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->set_attrs(se, attrs);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_get_status(hi_handle handle, tsio_secure_engine_status *status)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->get_status(se, status);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_add_channel(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;
    struct tsio_r_base *chn = HI_NULL;

    ret = tsio_r_get(ch_handle, &chn);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    ret = se->ops->add_channel(se, chn);

    tsio_r_put((struct tsio_r_base *)se);
out1:
    tsio_r_put(chn);
out0:
    return ret;
}

hi_s32 tsio_se_del_channel(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;
    struct tsio_r_base *chn = HI_NULL;

    ret = tsio_r_get(ch_handle, &chn);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    ret = se->ops->del_channel(se, chn);

    tsio_r_put((struct tsio_r_base *)se);
out1:
    tsio_r_put(chn);
out0:
    return ret;
}

hi_s32 tsio_se_del_all_channels(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->del_all_channels(se);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_acquire_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->acquire_buffer(se, req_len, buf, time_out);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_release_buffer(hi_handle handle, const tsio_buffer *buf)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->release_buffer(se, buf);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_close(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->close(se);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct tsio_slot *slot = HI_NULL;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = tsio_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    tsio_session_del_slot(slot->session, slot);

    if (tsio_slot_destroy(slot) != HI_SUCCESS) {
        HI_ERR_TSIO("tsio slot destroy failed\n");
    }

    se->ops->close(se);

    ret = se->ops->pst_close(se);

    tsio_r_put((struct tsio_r_base *)se);
out1:
    tsio_r_put((struct tsio_r_base *)se);
out0:
    return ret;
}

hi_s32 tsio_se_pre_mmap(hi_handle handle, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->pre_mmap(se, buf_handle, buf_size, usr_addr);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_se_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;
    struct tsio_r_se *se = HI_NULL;

    ret = tsio_r_se_get(handle, &se);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = se->ops->pst_mmap(se, buf_usr_addr);

    tsio_r_put((struct tsio_r_base *)se);
out:
    return ret;
}

hi_s32 tsio_cc_send(const hi_uchar *cmd, hi_u32 cmd_len, hi_uchar *resp, hi_u32 *resp_len, hi_u32 time_out)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct tsio_r_cc *ccsingleton = mgmt->ccsingleton;
    struct tsio_r_cc_para cc_para = {
        .cmd = cmd,
        .cmd_len = cmd_len,
        .resp = resp,
        .resp_len = resp_len,
    };

    return ccsingleton->ops->send(ccsingleton, &cc_para, time_out);
}

/*
 * TSIO mgmt helper functions.
 */
hi_s32 tsio_mgmt_init(hi_void)
{
    struct tsio_mgmt *mgmt = __get_tsio_mgmt();

    return mgmt->ops->init(mgmt);
}

hi_void tsio_mgmt_exit(hi_void)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    if (mgmt->ops->exit(mgmt) != HI_SUCCESS) {
        mgmt->ops->show_info(mgmt);

        /*
         * session must release all resource.
         */
        HI_FATAL_TSIO("tsio mgmt exit with error!\n");
    }
}

hi_s32 tsio_mgmt_get_config(tsio_config *config)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->get_config(mgmt, config);
}

hi_s32 tsio_mgmt_set_config(const tsio_config *config)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    if (!(config->band_width >= TSIO_BW_50M && config->band_width < TSIO_BW_BUTT)) {
        HI_ERR_TSIO("invalid bandwidth(%d).\n", config->band_width);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (!(config->stuff_sid < TSIO_SID_BUTT)) {
        HI_ERR_TSIO("invalid stuff sid(%d).\n", config->stuff_sid);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (!(config->sync_thres >= TSIO_SYNC_THRES_3 && config->sync_thres < TSIO_SYNC_THRES_BUTT)) {
        HI_ERR_TSIO("invalid sync thres(%d).\n", config->sync_thres);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->set_config(mgmt, config);
out:
    return ret;
}

hi_s32 tsio_mgmt_get_cap(tsio_capability *cap)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->get_cap(mgmt, cap);
}

hi_s32 tsio_mgmt_get_out_of_sync_cnt(hi_u32 *cnt)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->get_out_of_sync_cnt(mgmt, cnt);
}

hi_s32 tsio_mgmt_suspend(hi_void)
{
    struct tsio_mgmt *mgmt = __get_tsio_mgmt();

    return mgmt->ops->suspend(mgmt);
}

hi_s32 tsio_mgmt_resume(hi_void)
{
    struct tsio_mgmt *mgmt = __get_tsio_mgmt();

    return mgmt->ops->resume(mgmt);
}

static hi_s32 demand_demux(dmx_capability *cap)
{
    hi_s32 ret;
    demux_func_export *demux_export_func = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_DEMUX, (hi_void **)&demux_export_func);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = demux_export_func->dmx_get_capability(cap);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    return ret;

out:
    HI_ERR_TSIO("please first call HI_UNF_DMX_Init or other function to initilize demux module.\n");
    ret = HI_ERR_TSIO_DEMUX_NOT_READY;

    return ret;
}

static hi_s32 trim_tsi_port_attrs(const tsio_tsi_port_attrs *in, tsio_tsi_port_attrs_ex *out)
{
    hi_s32 ret;
    dmx_capability cap = {0};

    ret = demand_demux(&cap);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    if (!((in->source_port < DMX_IF_PORT_0 + cap.if_port_num) ||
            (in->source_port >= DMX_TSI_PORT_0 && in->source_port < DMX_TSI_PORT_0 + cap.tsi_port_num))) {
        HI_ERR_TSIO("invalid demux source port(%d).\n", in->source_port);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    out->source_port      = in->source_port;
    out->dmx_if_port_cnt  = cap.if_port_num;
    out->dmx_tsi_port_cnt = cap.tsi_port_num;

    ret = HI_SUCCESS;

out:
    return ret;
}

hi_s32 tsio_mgmt_create_tsi_port(tsio_port port, const tsio_tsi_port_attrs *attrs, struct tsio_r_tsi_port **tsi_port)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    tsio_tsi_port_attrs_ex valid_attr = {0};

    if (!((port == TSIO_ANY_TSI_PORT) ||
            (port >= TSIO_TSI_PORT_0 && port < TSIO_TSI_PORT_BUTT))) {
        HI_ERR_TSIO("invalid port_id(%d).\n", port);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = trim_tsi_port_attrs(attrs, &valid_attr);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = mgmt->ops->create_tsi_port(mgmt, port, &valid_attr, tsi_port);
out:
    return ret;
}

hi_s32 tsio_mgmt_destroy_tsi_port(struct tsio_r_base *obj)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->destroy_tsi_port(mgmt, (struct tsio_r_tsi_port *)obj);
}

static hi_s32 trim_ram_port_attrs(const tsio_ram_port_attrs_ex *in, tsio_ram_port_attrs_ex *out)
{
    hi_s32 ret;

    if ((in->source_type != TSIO_SOURCE_TS) && (in->source_type != TSIO_SOURCE_BULK)) {
        HI_ERR_TSIO("invalid source type(%d).\n", in->source_type);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (in->buffer_size > MAX_RAM_BUFFER_SIZE || in->buffer_size < MIN_RAM_BUFFER_SIZE) {
        HI_ERR_TSIO("invalid ram buffer size(0x%x)\n", in->buffer_size);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    /* auto data rate with flow control if max_data_rate is 0. */
    if (in->max_data_rate && (in->max_data_rate > MAX_RAM_DATA_RATE || in->max_data_rate < MIN_RAM_DATE_RATE)) {
        HI_ERR_TSIO("invalid ram data rate(%d kbps)\n", in->max_data_rate);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    TSIO_FATAL_ERROR((in->max_data_rate == 0) && (in->pace != 0));

    ret = memcpy_s(out, sizeof(tsio_ram_port_attrs_ex), in, sizeof(tsio_ram_port_attrs_ex));
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("memcpy_s failed, ret(%d)\n", ret);
    }

out:
    return ret;
}

hi_s32 tsio_mgmt_create_ram_port(tsio_port port, const tsio_ram_port_attrs_ex *attrs, struct tsio_r_ram_port **ram_port)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    tsio_ram_port_attrs_ex valid_attr = {0};

    if (!((port == TSIO_ANY_RAM_PORT) ||
            (port >= TSIO_RAM_PORT_0 && port < TSIO_RAM_PORT_BUTT))) {
        HI_ERR_TSIO("invalid port_id(%d).\n", port);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = trim_ram_port_attrs(attrs, &valid_attr);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = mgmt->ops->create_ram_port(mgmt, port, &valid_attr, ram_port);

out:
    return ret;
}

hi_s32 tsio_mgmt_destroy_ram_port(struct tsio_r_base *obj)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->destroy_ram_port(mgmt, (struct tsio_r_ram_port *)obj);
}

hi_s32 tsio_mgmt_create_pid_channel(struct tsio_r_base *port, hi_u32 pid, struct tsio_r_pid_channel **pid_channel)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    if (!(IS_RAMPORT(port) || IS_TSIPORT(port))) {
        HI_ERR_TSIO("not support non ram port or tsi port as pid channel data source.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (IS_RAMPORT(port)) {
        struct tsio_r_ram_port *ram_port = (struct tsio_r_ram_port *)port;

        if (ram_port->source_type != TSIO_SOURCE_TS) {
            HI_ERR_TSIO("pid channel demand ts mode ramport as data source.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }
    }

    ret = mgmt->ops->create_pid_channel(mgmt, port, pid, pid_channel);

out:
    return ret;
}

hi_s32 tsio_mgmt_destroy_pid_channel(struct tsio_r_base *obj)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->destroy_pid_channel(mgmt, (struct tsio_r_pid_channel *)obj);
}

hi_s32 tsio_mgmt_create_raw_channel(struct tsio_r_base *port, struct tsio_r_raw_channel **raw_channel)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct tsio_r_ram_port *ram_port = HI_NULL;

    if (!IS_RAMPORT(port)) {
        HI_ERR_TSIO("not support non ram port as raw channel data source.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ram_port = (struct tsio_r_ram_port *)port;
    if (ram_port->source_type != TSIO_SOURCE_BULK) {
        HI_ERR_TSIO("raw channel demand bulk mode ramport as data source.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_raw_channel(mgmt, port, raw_channel);

out:
    return ret;
}

hi_s32 tsio_mgmt_destroy_raw_channel(struct tsio_r_base *obj)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->destroy_raw_channel(mgmt, (struct tsio_r_raw_channel *)obj);
}

hi_s32 tsio_mgmt_create_sp_channel(struct tsio_r_base *port, struct tsio_r_sp_channel **sp_channel)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct tsio_r_ram_port *ram_port = HI_NULL;

    if (!IS_RAMPORT(port)) {
        HI_ERR_TSIO("not support non ram port as sp channel data source.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ram_port = (struct tsio_r_ram_port *)port;
    if (ram_port->source_type != TSIO_SOURCE_BULK) {
        HI_ERR_TSIO("sp channel demand bulk mode ramport as data source.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_sp_channel(mgmt, port, sp_channel);

out:
    return ret;
}

hi_s32 tsio_mgmt_destroy_sp_channel(struct tsio_r_base *obj)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->destroy_sp_channel(mgmt, (struct tsio_r_sp_channel *)obj);
}

hi_s32 tsio_mgmt_create_ivr_channel(struct tsio_r_base *port, struct tsio_r_ivr_channel **ivr_channel)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct tsio_r_ram_port *ram_port = HI_NULL;

    if (!IS_RAMPORT(port)) {
        HI_ERR_TSIO("not support non ram port as ivrestart channel data source.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ram_port = (struct tsio_r_ram_port *)port;
    if (ram_port->source_type != TSIO_SOURCE_BULK) {
        HI_ERR_TSIO("ivrestart channel demand bulk mode ramport as data source.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = mgmt->ops->create_ivr_channel(mgmt, port, ivr_channel);

out:
    return ret;
}

hi_s32 tsio_mgmt_destroy_ivr_channel(struct tsio_r_base *obj)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->destroy_ivr_channel(mgmt, (struct tsio_r_ivr_channel *)obj);
}

static hi_s32 trim_se_attrs(const tsio_secure_engine_attrs *in, tsio_secure_engine_attrs *out)
{
    hi_s32 ret;

    if (in->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        if (in->dest_port < DMX_TSIO_PORT_0 || in->dest_port >= DMX_TSIO_PORT_MAX) {
            HI_ERR_TSIO("invalid demux dest port(%d).\n", in->dest_port);
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }
    } else if (in->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        if (in->buffer_size > MAX_SE_BUFFER_SIZE || in->buffer_size < MIN_SE_BUFFER_SIZE) {
            HI_ERR_TSIO("invalid se buffer size(0x%x)\n", in->buffer_size);
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }
    } else {
        HI_ERR_TSIO("invalid output mode(%d).\n", in->mod);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = memcpy_s(out, sizeof(tsio_secure_engine_attrs), in, sizeof(tsio_secure_engine_attrs));
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("memcpy_s failed, ret(%d)\n", ret);
    }

out:
    return ret;
}

hi_s32 tsio_mgmt_create_se(tsio_sid sid, const tsio_secure_engine_attrs *attrs, struct tsio_r_se **se)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    tsio_secure_engine_attrs valid_attr = {0};

    if (!(sid == TSIO_ANY_SID || sid < TSIO_SID_BUTT)) {
        HI_ERR_TSIO("invalid sid(%d).\n", sid);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = trim_se_attrs(attrs, &valid_attr);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = mgmt->ops->create_se(mgmt, sid, &valid_attr, se);

out:
    return ret;
}

hi_s32 tsio_mgmt_destroy_se(struct tsio_r_base *obj)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    return mgmt->ops->destroy_se(mgmt, (struct tsio_r_se *)obj);
}

/*
 * TSIO TSI port functions.
 */
static hi_u32 get_dvb_port_id(const struct tsio_r_tsi_port *tsi_port)
{
    hi_u32 dvb_port_id = -1;

    if (tsi_port->source_port < DMX_IF_PORT_0 + tsi_port->dmx_if_port_cnt) {
        dvb_port_id = tsi_port->source_port - DMX_IF_PORT_0 + 1;
    } else if (tsi_port->source_port >= DMX_TSI_PORT_0 &&
                tsi_port->source_port < DMX_TSI_PORT_0 + tsi_port->dmx_tsi_port_cnt) {
        dvb_port_id = tsi_port->source_port - DMX_TSI_PORT_0 + tsi_port->dmx_if_port_cnt + 1;
    } else {
        HI_FATAL_TSIO("tsi_port->source_port:%u is not match!\n", tsi_port->source_port);
    }
    return dvb_port_id;
}

static hi_s32 get_live_port_id_and_type(const struct tsio_r_tsi_port *tsi_port,
    hi_u32 *port_id, tsio_live_port_type *type)
{
    if (tsi_port->source_port < DMX_IF_PORT_0 + tsi_port->dmx_if_port_cnt) {
        *port_id = tsi_port->source_port - DMX_IF_PORT_0;
        *type = TSIO_LIVE_PORT_IF;
    } else if (tsi_port->source_port >= DMX_TSI_PORT_0 &&
                tsi_port->source_port < DMX_TSI_PORT_0 + tsi_port->dmx_tsi_port_cnt) {
        *port_id = tsi_port->source_port - DMX_TSI_PORT_0;
        *type = TSIO_LIVE_PORT_TSI;
    } else {
        HI_FATAL_TSIO("tsi_port->source_port:%u is not match!\n", tsi_port->source_port);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/*
 * obj is staled after Close, it should discard all possible call request after that.
 */
static inline hi_bool tsi_port_staled(const struct tsio_r_tsi_port *tsi_port)
{
    if (unlikely(tsi_port->staled == HI_TRUE)) {
        HI_ERR_TSIO("tsi port is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _tsio_r_tsi_port_open_impl(const struct tsio_r_tsi_port *tsi_port)
{
    hi_u32 port_id;
    tsio_live_port_type port_type;

    if (get_live_port_id_and_type(tsi_port, &port_id, &port_type) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    tsio_hal_en_tsi_port(tsi_port->base.mgmt, tsi_port->base.id, port_id, port_type);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_tsi_port_open_impl(struct tsio_r_tsi_port *tsi_port)
{
    hi_s32 ret;

    osal_mutex_lock(&tsi_port->lock);

    TSIO_FATAL_ERROR(tsi_port->staled == HI_TRUE);

    ret = _tsio_r_tsi_port_open_impl(tsi_port);

    osal_mutex_unlock(&tsi_port->lock);

    return ret;
}

static hi_s32 tsio_r_tsi_port_get_attrs_impl(struct tsio_r_tsi_port *tsi_port, tsio_tsi_port_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&tsi_port->lock);

    if (tsi_port_staled(tsi_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    attrs->source_port = tsi_port->source_port;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&tsi_port->lock);

    return ret;
}

static hi_s32 tsio_r_tsi_port_set_attrs_impl(struct tsio_r_tsi_port *tsi_port, const tsio_tsi_port_attrs *attrs)
{
    HI_ERR_TSIO("not support alter tsi port data source configuration.\n");

    return HI_ERR_TSIO_NOT_SUPPORT;
}

static hi_s32 tsio_r_tsi_port_get_status_impl(struct tsio_r_tsi_port *tsi_port, tsio_tsi_port_status *status)
{
    hi_s32 ret;
    demux_func_export *demux_export_func = HI_NULL;
    hi_u32 ts_pkt_cnt = 0;
    hi_u32 err_pkt_cnt = 0;

    ret = osal_exportfunc_get(HI_ID_DEMUX, (hi_void **)&demux_export_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("get demux export symbol failed.\n");
        goto out0;
    }

    /* demux raw port id should sub 1. */
    ret = demux_export_func->dmx_get_tuner_port_pkt_cnt(get_dvb_port_id(tsi_port) - 1, &ts_pkt_cnt, &err_pkt_cnt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&tsi_port->lock);

    if (tsi_port_staled(tsi_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out1;
    }

    status->port_id = tsi_port->base.id + TSIO_TSI_PORT_0;
    /* after demux process ,tsio will not receive err ts pkt, so we only need total pkt cnt. */
    status->pkt_num = ts_pkt_cnt;

    ret = HI_SUCCESS;

out1:
    osal_mutex_unlock(&tsi_port->lock);
out0:
    return ret;
}

static hi_s32 _tsio_r_tsi_port_close_impl(const struct tsio_r_tsi_port *tsi_port)
{
    tsio_hal_dis_tsi_port(tsi_port->base.mgmt, tsi_port->base.id);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_tsi_port_close_impl(struct tsio_r_tsi_port *tsi_port)
{
    hi_s32 ret;

    osal_mutex_lock(&tsi_port->lock);

    if (tsi_port_staled(tsi_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    ret = _tsio_r_tsi_port_close_impl(tsi_port);
    if (ret == HI_SUCCESS) {
        tsi_port->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&tsi_port->lock);

    return ret;
}

static hi_s32 tsio_r_tsi_port_suspend_impl(struct tsio_r_tsi_port *tsi_port)
{
    hi_s32 ret;
    osal_mutex_lock(&tsi_port->lock);

    TSIO_FATAL_ERROR(tsi_port->staled == HI_TRUE);

    ret = _tsio_r_tsi_port_close_impl(tsi_port);

    osal_mutex_unlock(&tsi_port->lock);

    return ret;
}

static hi_s32 tsio_r_tsi_port_resume_impl(struct tsio_r_tsi_port *tsi_port)
{
    hi_s32 ret;
    osal_mutex_lock(&tsi_port->lock);

    TSIO_FATAL_ERROR(tsi_port->staled == HI_TRUE);

    ret = _tsio_r_tsi_port_open_impl(tsi_port);

    osal_mutex_unlock(&tsi_port->lock);

    return ret;
}

static struct tsio_r_tsi_port_ops g_tsio_tsi_port_ops = {
    .open        = tsio_r_tsi_port_open_impl,
    .get_attrs   = tsio_r_tsi_port_get_attrs_impl,
    .set_attrs   = tsio_r_tsi_port_set_attrs_impl,
    .get_status  = tsio_r_tsi_port_get_status_impl,
    .close       = tsio_r_tsi_port_close_impl,

    .suspend     = tsio_r_tsi_port_suspend_impl,
    .resume      = tsio_r_tsi_port_resume_impl,
};

/*
 * obj is staled after Close, it should discard all possible call request after that.
 */
static inline hi_bool ram_port_staled(const struct tsio_r_ram_port *ram_port)
{
    if (unlikely(ram_port->staled == HI_TRUE)) {
        HI_ERR_TSIO("ram port is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_void flow_control_handler(struct work_struct *work)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port =  container_of(work, struct tsio_r_ram_port, flow_control_worker);
    struct tsio_mgmt *mgmt = ram_port->base.mgmt;
    struct list_head *se_node = HI_NULL;
    hi_bool do_reset = HI_TRUE;

    osal_mutex_lock(&mgmt->se_list_lock);

    list_for_each(se_node, &mgmt->se_head) {
        struct tsio_r_se *se = list_entry(se_node, struct tsio_r_se, node);

        ret = se->ops->handle_flow_control(se, ram_port);
        if (ret != HI_SUCCESS) {
            do_reset = HI_FALSE;

            break;
        }
    }

    osal_mutex_unlock(&mgmt->se_list_lock);

    if (do_reset == HI_TRUE) {
        tsio_hal_clr_dma_chn_pend_status(mgmt, ram_port->base.id);
    }
}

#define DSC_ADDR_OFFSET1 1
#define DSC_ADDR_OFFSET2 2
#define DSC_ADDR_OFFSET3 3

static hi_void dsc_end_irq_handler(struct work_struct *work)
{
    struct tsio_r_ram_port *ram_port =  container_of(work, struct tsio_r_ram_port, dsc_end_worker);
    hi_u32 cur_sw_read_idx, cur_hw_read_idx;

    osal_mutex_lock(&ram_port->blk_map_lock);

    /*
     * dsc end handler running in wq context.
     * ram port hw maybe invalid after disable ram port by _TSIO_R_RAMPort_Close_Impl.
     */
    if (tsio_hal_ram_port_enabled(ram_port->base.mgmt, ram_port->base.id) != HI_TRUE) {
        goto out;
    }

    cur_sw_read_idx = ram_port->last_dsc_read_idx;
    cur_hw_read_idx = tsio_hal_get_ram_port_cur_read_idx(ram_port->base.mgmt, ram_port->base.id);

    /* [cur_sw_read_idx, cur_hw_read_idx) */
    while (cur_sw_read_idx != cur_hw_read_idx) {
        hi_u32 *dsc_addr = (hi_u32 *)(ram_port->dsc_ker_addr + cur_sw_read_idx * DEFAULT_RAM_DSC_SIZE);
        /* phy addr total 36bit, low 32 bit and high 4 bit  */
        hi_u64 buf_phy_addr = *(dsc_addr + DSC_ADDR_OFFSET1) +
            (((hi_u64)(*(dsc_addr + DSC_ADDR_OFFSET3) & 0xf0000000)) << TSIO_HIGH_PHY_ADDR_LEN);
        /* skip flush dsc */
        if (unlikely((buf_phy_addr == 0) && (*(dsc_addr + DSC_ADDR_OFFSET2) == 0))) {
            U_DMA_DSC_WORD_0 word0;

            word0.u32 = *dsc_addr;

            TSIO_FATAL_ERROR(word0.bits.flush != 1);
        } else {
            hi_u32 blk_idx = (hi_u32)(buf_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;

            TSIO_FATAL_ERROR(buf_phy_addr < ram_port->buf_phy_addr ||
                             buf_phy_addr >= ram_port->buf_phy_addr + ram_port->buf_size ||
                             RAM_DSC_GUIDE_NUMBER != (*(dsc_addr + DSC_ADDR_OFFSET3) & 0xfffffff));

            switch (ram_port->blk_map[blk_idx].flag) {
                case BLK_ADD_INT_FLAG: {
                        ram_port->blk_map[blk_idx].ref--;
                        break;
                    }

                case BLK_ADD_RELCAIM_INT_FLAG: {
                        if (--ram_port->blk_map[blk_idx].ref == 0) {
                            ram_port->blk_map[blk_idx].flag         = BLK_FREE_FLAG;
                            ram_port->blk_map[blk_idx].head_blk_idx = BLK_INVALID_HEAD_IDX;
                            ram_port->blk_map[blk_idx].req_len      = 0;

                            ram_port->free_blk_nr++;
                        }
                        break;
                    }

                default:
                    HI_FATAL_TSIO("BLK flag unknowen, blk flag is:%u \n", ram_port->blk_map[blk_idx].flag);
            }

            TSIO_FATAL_ERROR(ram_port->free_blk_nr > ram_port->total_blk_nr);
        }

        /* roll loop */
        cur_sw_read_idx = (cur_sw_read_idx + 1 >= ram_port->dsc_depth) ? 0 : cur_sw_read_idx + 1;
    }

    ram_port->last_dsc_read_idx = cur_hw_read_idx;

    ram_port->wait_cond = 1;
    wake_up_interruptible(&ram_port->wait_queue);

out:
    osal_mutex_unlock(&ram_port->blk_map_lock);
}

#define TEN_MSECS 10

static hi_s32 get_buffer(struct tsio_r_ram_port *ram_port, hi_u32 req_blk_nr, hi_u64 *buf_phy_addr)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 cur_cont_blk_idx = 0;
    hi_u32 cur_cont_blk_cnt = 0;
    unsigned long start = jiffies;
    unsigned long end = start + msecs_to_jiffies(TEN_MSECS);
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

    WARN(!time_in_range(jiffies, start, end), "alloc buffer latency is too much(%d ms)!",
         jiffies_to_msecs(jiffies - start));

    return ret;
}

static hi_s32 check_buffer(const struct tsio_r_ram_port *ram_port, tsio_buffer *buf)
{
    hi_s32 ret;
    hi_u32 start_blk_idx, blk_cnt, head_blk_idx;
    hi_u32 index;
    hi_u64 phy_addr;

    /* hw support access non-align byte address. */
    if ((buf->data == HI_NULL) || (buf->length == 0)) {
        HI_ERR_TSIO("invalid buffer[0x%x:%d] range.\n", buf->data, buf->length);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    phy_addr = ram_port->buf_usr_addr ?
                    buf->data - ram_port->buf_usr_addr + ram_port->buf_phy_addr : buf->data - ram_port->buf_ker_addr +
                    ram_port->buf_phy_addr;

    if (phy_addr < ram_port->buf_phy_addr ||
            phy_addr + buf->length  > ram_port->buf_phy_addr + ram_port->buf_size) {
        HI_ERR_TSIO("invalid buffer[0x%x:%d] range.\n", phy_addr, buf->length);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    start_blk_idx = (hi_u32)(phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    blk_cnt      = (buf->length + ram_port->blk_size - 1) / ram_port->blk_size;

    /* all blk have same headblkidx which means they belong to one buffer */
    head_blk_idx = ram_port->blk_map[start_blk_idx].head_blk_idx;

    for (index = start_blk_idx; index < start_blk_idx + blk_cnt && index < ram_port->total_blk_nr; index++) {
        /* head blk idx is wrong */
        if (head_blk_idx != ram_port->blk_map[index].head_blk_idx) {
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }

        /* the blk tag must be BLK_ALLOC_FLAG or BLK_ADD_INT_TAG */
        if (ram_port->blk_map[index].flag != BLK_ALLOC_FLAG && ram_port->blk_map[index].flag != BLK_ADD_INT_FLAG) {
            HI_ERR_TSIO("blk(%d) tag(0x%x) is wrong.\n", index, ram_port->blk_map[index].flag);
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }
    }

    /* push buffer range is subset of request range. */
    if (phy_addr < head_blk_idx * ram_port->blk_size + ram_port->buf_phy_addr ||
            phy_addr + buf->length > head_blk_idx * ram_port->blk_size + ram_port->buf_phy_addr +
            ram_port->blk_map[head_blk_idx].req_len) {
        HI_ERR_TSIO("invalid buffer[0x%x:%d] range.\n", phy_addr, buf->length);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = HI_SUCCESS;

out:
    return ret;
}

static inline hi_s32 check_push_buffer(struct tsio_r_ram_port *ram_port, tsio_buffer *buf)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->blk_map_lock);

    ret = check_buffer(ram_port, buf);

    osal_mutex_unlock(&ram_port->blk_map_lock);

    return ret;
}

static hi_s32 wait_dsc_sync(struct tsio_r_ram_port *ram_port, hi_u32 count)
{
    hi_s32 ret = HI_SUCCESS;

    if (unlikely(count == 0)) {
        goto out;
    }

    while (!(ram_port->dsc_depth - tsio_get_queue_lenth(ram_port->last_dsc_read_idx,
        ram_port->last_dsc_write_idx, ram_port->dsc_depth) > count)) {
        ram_port->wait_cond = 0;
        ret = wait_event_interruptible(ram_port->wait_queue, ram_port->wait_cond);
        if (ret < 0) {
            /* exit after get signal. */
            ret = HI_ERR_TSIO_INTR;
            goto out;
        }
    }

out:
    return ret;
}

static hi_s32 wait_bulk_dsc_sync(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret = HI_SUCCESS;

    TSIO_FATAL_ERROR(!osal_mutex_is_locked(&ram_port->bulk_srv_lock));

    for (;;) {
        osal_mutex_unlock(&ram_port->bulk_srv_lock);

        ret = wait_dsc_sync(ram_port, ram_port->bulk_srv_cnt);

        osal_mutex_lock(&ram_port->bulk_srv_lock);

        /* double check again when hold locks */
        if (ret == HI_SUCCESS) {
            if (ram_port->dsc_depth - tsio_get_queue_lenth(ram_port->last_dsc_read_idx,
                ram_port->last_dsc_write_idx, ram_port->dsc_depth) > ram_port->bulk_srv_cnt) {
                break;
            } else {
                continue;
            }
        } else {
            break;
        }
    }

    return ret;
}

static hi_s32 push_ts_buffer(struct tsio_r_ram_port *ram_port, hi_u64 push_phy_addr, hi_u32 push_len)
{
    hi_s32 ret;
    hi_u32 blk_idx;
    hi_u32 *cur_dsc_addr = HI_NULL;
    struct tsio_dsc_base_info dsc_base_info = {0};

    /* wait dsc ready indefinitely until signal. */
    ret = wait_dsc_sync(ram_port, 1);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&ram_port->blk_map_lock);

    blk_idx = (push_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    ram_port->blk_map[blk_idx].flag = BLK_ADD_INT_FLAG;
    ram_port->blk_map[blk_idx].ref++;

    cur_dsc_addr = (hi_u32 *)(ram_port->dsc_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
    dsc_base_info.id = ram_port->base.id;
    dsc_base_info.buf_phy_addr = push_phy_addr;
    dsc_base_info.pkt_cnt = push_len / ram_port->pkt_size;
    tsio_hal_setup_ts_dsc(ram_port->base.mgmt, &dsc_base_info, HI_TRUE, cur_dsc_addr);

    ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ?
                                                  0 : ram_port->last_dsc_write_idx + 1;
    tsio_hal_add_dsc(ram_port->base.mgmt, ram_port->base.id, ram_port->last_dsc_write_idx);

    osal_mutex_unlock(&ram_port->blk_map_lock);

out:
    return ret;
}

static hi_s32 push_bulk_buffer(struct tsio_r_ram_port *ram_port, hi_u64 push_phy_addr, hi_u32 push_len)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;
    struct tsio_dsc_base_info dsc_base_info = {0};

    osal_mutex_lock(&ram_port->bulk_srv_lock);

    ret = wait_bulk_dsc_sync(ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    osal_mutex_lock(&ram_port->blk_map_lock);

    list_for_each(node, &ram_port->bulk_srv_list) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, bulk_srv_node);
        hi_u32 blk_idx;
        hi_u32 *cur_dsc_addr = HI_NULL;

        blk_idx = (push_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
        ram_port->blk_map[blk_idx].flag = BLK_ADD_INT_FLAG;
        ram_port->blk_map[blk_idx].ref++;

        cur_dsc_addr = (hi_u32 *)(ram_port->dsc_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
        dsc_base_info.id = ram_port->base.id;
        dsc_base_info.buf_phy_addr = push_phy_addr;
        dsc_base_info.pkt_cnt = push_len / ram_port->pkt_size;
        tsio_hal_setup_bulk_dsc(ram_port->base.mgmt, &dsc_base_info, HI_TRUE, cur_dsc_addr, se->base.id);

        ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ?
                                                      0 : ram_port->last_dsc_write_idx + 1;
        tsio_hal_add_dsc(ram_port->base.mgmt, ram_port->base.id, ram_port->last_dsc_write_idx);
    }

    osal_mutex_unlock(&ram_port->blk_map_lock);

out:
    osal_mutex_unlock(&ram_port->bulk_srv_lock);

    return ret;
}

/*
 * caller must ensure that there are enought dsc .
 */
static hi_void push_ivr_buffer(struct tsio_r_ram_port *ram_port, hi_u64 push_phy_addr, hi_u32 push_len)
{
    hi_u32 blk_idx;
    hi_u32 *cur_dsc_addr = HI_NULL;
    struct tsio_dsc_base_info dsc_base_info = {0};

    osal_mutex_lock(&ram_port->blk_map_lock);

    TSIO_FATAL_ERROR(!(ram_port->dsc_depth - tsio_get_queue_lenth(ram_port->last_dsc_read_idx,
        ram_port->last_dsc_write_idx, ram_port->dsc_depth) > 1));

    blk_idx = (push_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    ram_port->blk_map[blk_idx].flag = BLK_ADD_INT_FLAG;
    ram_port->blk_map[blk_idx].ref++;

    cur_dsc_addr = (hi_u32 *)(ram_port->dsc_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
    dsc_base_info.id = ram_port->base.id;
    dsc_base_info.buf_phy_addr = push_phy_addr;
    dsc_base_info.pkt_cnt = push_len / ram_port->pkt_size;
    tsio_hal_setup_bulk_dsc(ram_port->base.mgmt, &dsc_base_info,
        HI_TRUE, cur_dsc_addr, ram_port->ivrestart_se->base.id);

    ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ?
                                                  0 : ram_port->last_dsc_write_idx + 1;
    tsio_hal_add_dsc(ram_port->base.mgmt, ram_port->base.id, ram_port->last_dsc_write_idx);

    osal_mutex_unlock(&ram_port->blk_map_lock);
}

/*
 * caller must ensure that there are enought dsc .
 */
static hi_void push_flush_op(struct tsio_r_ram_port *ram_port, struct tsio_r_se *se)
{
    hi_u32 *cur_dsc_addr = HI_NULL;

    se->wait_flush_cond = 0;

    osal_mutex_lock(&ram_port->blk_map_lock);

    TSIO_FATAL_ERROR(!(ram_port->dsc_depth - tsio_get_queue_lenth(ram_port->last_dsc_read_idx,
        ram_port->last_dsc_write_idx, ram_port->dsc_depth) > 1));

    cur_dsc_addr = (hi_u32 *)(ram_port->dsc_ker_addr + ram_port->last_dsc_write_idx * DEFAULT_RAM_DSC_SIZE);
    tsio_hal_setup_bulk_flush_dsc(ram_port->base.mgmt, ram_port->base.id, cur_dsc_addr, se->base.id);

    ram_port->last_dsc_write_idx = (ram_port->last_dsc_write_idx + 1 >= ram_port->dsc_depth) ?
                                                  0 : ram_port->last_dsc_write_idx + 1;
    tsio_hal_add_dsc(ram_port->base.mgmt, ram_port->base.id, ram_port->last_dsc_write_idx);

    osal_mutex_unlock(&ram_port->blk_map_lock);
}

static hi_s32 check_put_buffer(struct tsio_r_ram_port *ram_port, tsio_buffer *buf)
{
    hi_s32 ret;
    hi_u32 start_blk_idx, blk_cnt, head_blk_idx;
    hi_u32 index;
    hi_u64 phy_addr;

    osal_mutex_lock(&ram_port->blk_map_lock);

    ret = check_buffer(ram_port, buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    phy_addr = ram_port->buf_usr_addr ?
                    buf->data - ram_port->buf_usr_addr + ram_port->buf_phy_addr : buf->data - ram_port->buf_ker_addr +
                    ram_port->buf_phy_addr;

    start_blk_idx = (phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;

    /* all blk have same headblkidx which means they belong to one buffer */
    head_blk_idx = ram_port->blk_map[start_blk_idx].head_blk_idx;

    if (head_blk_idx != ram_port->blk_map[head_blk_idx].head_blk_idx ||
            phy_addr != head_blk_idx * ram_port->blk_size + ram_port->buf_phy_addr) {
        HI_ERR_TSIO("invalid buffer[0x%llx:%d] range.\n", phy_addr, buf->length);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    /*
     * HI_UNF_TSIO_PutBuffer will complete push data and put buffer in one call.
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
        HI_FATAL_TSIO("check put buffer error!\n");
    }

out:
    osal_mutex_unlock(&ram_port->blk_map_lock);
    return ret;
}

static hi_void fixup_ts_stream(struct tsio_r_ram_port *ram_port, tsio_buffer *buf)
{
    hi_u8 *buf_ker_addr = buf->buf_offset + ram_port->buf_ker_addr;
    hi_bool need_fix = HI_FALSE;
    hi_u32 update_offset = 0;
    hi_u32 index;

    if (ram_port->source_type == TSIO_SOURCE_TS) {
        for (index = 0; index < buf->length; index += ram_port->pkt_size) {
            if (TS_SYNC_BYTE != *(buf_ker_addr + index)) {
                HI_ERR_TSIO("try to auto fix abnormal ts stream.\n");

                need_fix = HI_TRUE;
                break;
            }
        }

        if (need_fix == HI_TRUE) {
            for (index = 0; index < buf->length; index++) {
                if (TS_SYNC_BYTE == *(buf_ker_addr + index) &&
                        (index + ram_port->pkt_size < buf->length &&
                        TS_SYNC_BYTE == *(buf_ker_addr + index + ram_port->pkt_size))) {
                    if (memmove_s(buf_ker_addr + update_offset, ram_port->pkt_size,
                        buf_ker_addr + index, ram_port->pkt_size)) {
                        HI_ERR_TSIO("memmove_s failed\n");
                        return;
                    }

                    update_offset += ram_port->pkt_size;
                }
            }

            buf->length = update_offset ? update_offset : buf->length;
        }
    }
}

static hi_s32 _tsio_r_ram_port_open_impl(struct tsio_r_ram_port *ram_port)
{
    hi_u32 index;

    osal_mutex_lock(&ram_port->blk_map_lock);

    ram_port->free_blk_nr         = ram_port->total_blk_nr;
    ram_port->last_dsc_read_idx   = 0;
    ram_port->last_dsc_write_idx  = 0;
    ram_port->iter_blk_map_helper = 0;

    if (memset_s(ram_port->blk_map, sizeof(*ram_port->blk_map) * ram_port->total_blk_nr, 0,
                 sizeof(*ram_port->blk_map) * ram_port->total_blk_nr)) {
        osal_mutex_unlock(&ram_port->blk_map_lock);
        HI_ERR_TSIO("memset_s failed\n");
        return HI_FAILURE;
    }

    for (index = 0; index < ram_port->total_blk_nr; index++) {
        ram_port->blk_map[index].head_blk_idx = BLK_INVALID_HEAD_IDX;
    }

    tsio_hal_en_ram_port(ram_port->base.mgmt, ram_port->base.id, ram_port->dsc_phy_addr, ram_port->dsc_depth,
                         ram_port->pace);

    osal_mutex_unlock(&ram_port->blk_map_lock);

    ram_port->all_pkt_cnt     = 0;

    ram_port->get_count       = 0;
    ram_port->get_valid_count = 0;
    ram_port->push_count      = 0;
    ram_port->put_count       = 0;

    return HI_SUCCESS;
}

#define PHY_ADDR_ALIGN_BYTES 16

static hi_s32 tsio_r_ram_port_open_impl(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    hi_char buf_name[TSIO_BUF_NAME_LEN] = "tsio_ram_buf";
    hi_u32 buf_name_index = sizeof("tsio_ram_buf") - 1;
    hi_char buf_dsc_name[TSIO_BUF_NAME_LEN] = "ram_dsc_buf";
    hi_u32 buf_dsc_name_index = sizeof("ram_dsc_buf") - 1;
    hi_u32 dsc_buf_size = DEFAULT_RAM_DSC_DEPTH * DEFAULT_RAM_DSC_SIZE;
    tsio_mem_info mmu_buf = {0};
    tsio_mem_info  mmz_buf = {0};

    osal_mutex_lock(&ram_port->lock);

    TSIO_FATAL_ERROR(ram_port->staled == HI_TRUE);

    buf_name[buf_name_index] = '0' + (ram_port->base.id + TSIO_RAM_PORT_0) / ALIGN_LEN % ALIGN_LEN;
    buf_name[buf_name_index + 1] = '0' + (ram_port->base.id + TSIO_RAM_PORT_0) % ALIGN_LEN;
    ret = tsio_alloc_and_map_buf(buf_name, ram_port->buf_size, HI_FALSE, &mmu_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("mmu malloc 0x%x failed\n", ram_port->buf_size);
        goto out0;
    }

    TSIO_FATAL_ERROR(ram_port->buf_size != mmu_buf.buf_size);

    ram_port->buf_obj      = mmu_buf.tsio_buf_obj;
    ram_port->buf_handle   = mmu_buf.handle;
    ram_port->buf_usr_addr = HI_NULL;
    ram_port->buf_ker_addr = mmu_buf.buf_vir_addr;
    ram_port->buf_phy_addr = mmu_buf.buf_phy_addr;

    buf_dsc_name[buf_dsc_name_index] = '0' + (ram_port->base.id + TSIO_RAM_PORT_0) / ALIGN_LEN % ALIGN_LEN;
    buf_dsc_name[buf_dsc_name_index + 1] = '0' + (ram_port->base.id + TSIO_RAM_PORT_0) % ALIGN_LEN;
    ret = tsio_alloc_and_map_mmz(buf_dsc_name, dsc_buf_size, HI_FALSE, &mmz_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("mmz malloc 0x%x failed\n", dsc_buf_size);
        goto out1;
    }

    TSIO_FATAL_ERROR(mmz_buf.buf_phy_addr % PHY_ADDR_ALIGN_BYTES || dsc_buf_size != mmz_buf.buf_size);

    ram_port->dsc_buf_obj  = mmz_buf.tsio_buf_obj;
    ram_port->dsc_handle   = mmz_buf.handle;
    ram_port->dsc_ker_addr = mmz_buf.buf_vir_addr;
    ram_port->dsc_phy_addr = mmz_buf.buf_phy_addr;
    ram_port->dsc_depth    = DEFAULT_RAM_DSC_DEPTH;

    if (memset_s(ram_port->dsc_ker_addr, dsc_buf_size, 0, dsc_buf_size)) {
        HI_ERR_TSIO("memset_s failed\n");
        ret = HI_FAILURE;
        goto out2;
    }

    /* alloc & free blk map */
    ram_port->total_blk_nr = ram_port->buf_size / ram_port->blk_size;

    ram_port->blk_map = HI_KZALLOC(HI_ID_TSIO, sizeof(*ram_port->blk_map) * ram_port->total_blk_nr, GFP_KERNEL);
    if (ram_port->blk_map == HI_NULL) {
        HI_ERR_TSIO("malloc 0x%x failed\n", sizeof(*ram_port->blk_map) * ram_port->total_blk_nr);
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out2;
    }

    ret = _tsio_r_ram_port_open_impl(ram_port);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    osal_mutex_unlock(&ram_port->lock);

    return HI_SUCCESS;

out3:
    HI_KFREE(HI_ID_TSIO, ram_port->blk_map);
out2:
    tsio_unmap_and_release_mmz(&mmz_buf);
out1:
    tsio_unmap_and_release_buf(&mmu_buf);
out0:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_get_attrs_impl(struct tsio_r_ram_port *ram_port, tsio_ram_port_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    attrs->source_type   = ram_port->source_type;
    attrs->buffer_size   = ram_port->buf_size;
    attrs->max_data_rate = ram_port->max_data_rate;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_set_attrs_impl(struct tsio_r_ram_port *ram_port, const tsio_ram_port_attrs_ex *attrs)
{
    hi_s32 ret;
    tsio_ram_port_attrs_ex valid_attr = {0};

    ret = trim_ram_port_attrs(attrs, &valid_attr);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out1;
    }

    if (valid_attr.source_type != ram_port->source_type) {
        HI_ERR_TSIO("not support alter ram port source type.\n");
        ret = HI_ERR_TSIO_NOT_SUPPORT;
        goto out1;
    } else if (valid_attr.buffer_size - valid_attr.buffer_size % ram_port->blk_size != ram_port->buf_size) {
        HI_ERR_TSIO("not support alter ram port buffer size.\n");
        ret = HI_ERR_TSIO_NOT_SUPPORT;
        goto out1;
    } else if (valid_attr.max_data_rate != ram_port->max_data_rate) {
        ram_port->max_data_rate = valid_attr.max_data_rate;
        ram_port->pace        = valid_attr.pace;

        tsio_hal_set_ram_port_rate(ram_port->base.mgmt, ram_port->base.id, ram_port->pace);

        ret = HI_SUCCESS;
    } else { /* no parameter changed ,do nothing. */
        ret = HI_SUCCESS;
    }

out1:
    osal_mutex_unlock(&ram_port->lock);
out0:
    return ret;
}

static hi_s32 _tsio_r_ram_port_get_status_impl(struct tsio_r_ram_port *ram_port, tsio_ram_port_status *status)
{
    hi_s32 ret;
    hi_u32 cur_cont_blk_cnt = 0;
    hi_u32 max_cont_blk_cnt = 0;
    unsigned long start, end;
    hi_u32 index;

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out0;
    }

    status->port_id         = ram_port->base.id + TSIO_RAM_PORT_0;
    status->buffer_used     = 0;
    status->buffer_size     = ram_port->buf_size;
    status->buffer_blk_size = ram_port->blk_size;
    status->pkt_num         = ram_port->all_pkt_cnt;

    osal_mutex_lock(&ram_port->blk_map_lock);

    /* refer to tsio_port_proc_read. external checking without hold ramport->lock. */
    if (ram_port->blk_map == HI_NULL) {
        ret = HI_ERR_TSIO_STALED;
        goto out1;
    }

    start = jiffies;
    end   = start + msecs_to_jiffies(TEN_MSECS);

    /* locate max free continue space */
    for (index = 0; index < ram_port->total_blk_nr; index++) {
        if (ram_port->blk_map[index].flag == BLK_FREE_FLAG) {
            cur_cont_blk_cnt++;

            max_cont_blk_cnt = cur_cont_blk_cnt > max_cont_blk_cnt ? cur_cont_blk_cnt : max_cont_blk_cnt;
        } else {
            max_cont_blk_cnt = cur_cont_blk_cnt > max_cont_blk_cnt ? cur_cont_blk_cnt : max_cont_blk_cnt;

            cur_cont_blk_cnt = 0;
        }
    }

    TSIO_FATAL_ERROR(max_cont_blk_cnt > ram_port->total_blk_nr);

    status->buffer_used = ram_port->buf_size - max_cont_blk_cnt * ram_port->blk_size;
    status->buffer_size = ram_port->buf_size;

    WARN(!time_in_range(jiffies, start, end), "get buffer status latency is too much(%d ms)!",
         jiffies_to_msecs(jiffies - start));

    ret = HI_SUCCESS;

out1:
    osal_mutex_unlock(&ram_port->blk_map_lock);
out0:
    return ret;
}

static hi_s32 tsio_r_ram_port_get_status_impl(struct tsio_r_ram_port *ram_port, tsio_ram_port_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    ret = _tsio_r_ram_port_get_status_impl(ram_port, status);

    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 _tsio_r_ram_port_get_buffer_impl(struct tsio_r_ram_port *ram_port, hi_u32 req_len, hi_u64 *buf_phy_addr)
{
    hi_s32 ret;
    hi_u32 req_blk_nr = (req_len  + ram_port->blk_size - 1) / ram_port->blk_size;
    hi_u32 head_blk_idx;
    hi_u32 index;

    TSIO_FATAL_ERROR(req_len == 0 || req_len > ram_port->buf_size || req_len % ram_port->pkt_size);

    osal_mutex_lock(&ram_port->blk_map_lock);

    if (ram_port->free_blk_nr < req_blk_nr) {
        ret = HI_ERR_TSIO_NOAVAILABLE_BUF;
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
            ret = HI_ERR_TSIO_NOAVAILABLE_BUF;
            goto out;
        }
    }

    head_blk_idx = (hi_u32)(*buf_phy_addr - ram_port->buf_phy_addr) / ram_port->blk_size;
    for (index = head_blk_idx; index < head_blk_idx + req_blk_nr; index++) {
        ram_port->blk_map[index].flag         = BLK_ALLOC_FLAG;
        ram_port->blk_map[index].ref          = 1;
        ram_port->blk_map[index].head_blk_idx = head_blk_idx;
        ram_port->blk_map[index].req_len      = req_len;

        ram_port->free_blk_nr-- ;
    }

out:
    osal_mutex_unlock(&ram_port->blk_map_lock);
    return ret;
}

static hi_s32 tsio_r_ram_port_get_buffer_impl(struct tsio_r_ram_port *ram_port,
    hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out)
{
    hi_s32 ret = HI_FAILURE;
    hi_u64 buf_phy_addr  = 0;

    osal_mutex_lock(&ram_port->lock);

    ram_port->get_count++;

    do {
        if (ram_port_staled(ram_port)) {
            ret = HI_ERR_TSIO_STALED;
            goto out;
        }

        /* req buffer size must align with blk size. */
        if (unlikely(req_len == 0 || req_len > ram_port->buf_size || req_len % ram_port->pkt_size)) {
            HI_ERR_TSIO("invalid request len(%d).\n", req_len);
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }

        ret = _tsio_r_ram_port_get_buffer_impl(ram_port, req_len, &buf_phy_addr);
        if (ret != HI_SUCCESS) {
            if (time_out == 0) {
                ret = HI_ERR_TSIO_NOAVAILABLE_BUF;
                goto out;
            }

            osal_mutex_unlock(&ram_port->lock);

            ram_port->wait_cond = 0;
            ret = wait_event_interruptible_timeout(ram_port->wait_queue,
                ram_port->wait_cond, msecs_to_jiffies(time_out));

            osal_mutex_lock(&ram_port->lock);

            if (ret == 0) {
                ret = HI_ERR_TSIO_TIMEOUT;
                goto out;
            } else if (ret < 0) { /* signal */
                ret = HI_ERR_TSIO_INTR;
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

    buf->buf_handle = ram_port->buf_handle;
    buf->buf_offset = buf_phy_addr - ram_port->buf_phy_addr;
    buf->length  = req_len;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 _tsio_r_ram_port_push_buffer_impl(struct tsio_r_ram_port *ram_port, const tsio_buffer *buf)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 push_phy_addr = buf->buf_offset + ram_port->buf_phy_addr;
    hi_u32 push_len      = buf->length;

    do {
        hi_u32 len = ram_port->blk_size - (push_phy_addr - ram_port->buf_phy_addr) % ram_port->blk_size;
        len = len < push_len ? len : push_len;

        TSIO_FATAL_ERROR(len % ram_port->pkt_size);

        if (ram_port->source_type == TSIO_SOURCE_TS) {
            ret = push_ts_buffer(ram_port, push_phy_addr, len);
            if (ret != HI_SUCCESS) {
                goto out;
            }
        } else if (ram_port->source_type == TSIO_SOURCE_BULK) {
            ret = push_bulk_buffer(ram_port, push_phy_addr, len);
            if (ret != HI_SUCCESS) {
                goto out;
            }
        } else {
            HI_FATAL_TSIO("ram_port->source_type is unknown, ram_port->source_type:%u!\n", ram_port->source_type);
        }

        push_phy_addr += len;
        push_len     -= len;
    } while (push_len);

out:
    return ret;
}

static hi_s32 tsio_r_ram_port_push_buffer_impl(struct tsio_r_ram_port *ram_port, tsio_buffer *buf)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    ram_port->push_count++;

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    /* buf->phy_addr will be update after function call. */
    ret = check_push_buffer(ram_port, buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* try to auto fix some abnormal ts stream. 'buf' maybe update after function call. */
    fixup_ts_stream(ram_port, buf);

    ret = _tsio_r_ram_port_push_buffer_impl(ram_port, buf);
    if (ret == HI_SUCCESS) {
        ram_port->all_pkt_cnt += buf->length / ram_port->pkt_size;
    }

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 _tsio_r_ram_port_put_buffer_impl(struct tsio_r_ram_port *ram_port, const tsio_buffer *buf)
{
    hi_u32 start_blk_idx, blk_cnt;
    hi_u32 index;

    start_blk_idx = (hi_u32)buf->buf_offset / ram_port->blk_size;
    blk_cnt       = (buf->length + ram_port->blk_size - 1) / ram_port->blk_size;

    osal_mutex_lock(&ram_port->blk_map_lock);

    for (index = start_blk_idx; index < start_blk_idx + blk_cnt; index++) {
        switch (ram_port->blk_map[index].flag) {
            case BLK_ALLOC_FLAG: {
                    TSIO_FATAL_ERROR(ram_port->blk_map[index].ref != 1);

                    ram_port->blk_map[index].ref--;

                    ram_port->blk_map[index].flag          = BLK_FREE_FLAG;
                    ram_port->blk_map[index].head_blk_idx  = BLK_INVALID_HEAD_IDX;
                    ram_port->blk_map[index].req_len       = 0;

                    ram_port->free_blk_nr++;

                    break;
                }

            case BLK_ADD_INT_FLAG: {
                    TSIO_FATAL_ERROR(ram_port->blk_map[index].ref == 0);

                    if (ram_port->blk_map[index].ref == 1) { /* blk can be safe release now */
                        ram_port->blk_map[index].flag         = BLK_FREE_FLAG;
                        ram_port->blk_map[index].head_blk_idx = BLK_INVALID_HEAD_IDX;
                        ram_port->blk_map[index].ref          = 0;
                        ram_port->blk_map[index].req_len      = 0;

                        ram_port->free_blk_nr++;
                    } else if (ram_port->blk_map[index].ref > 1) { /* blk will be release after int processed */
                        ram_port->blk_map[index].flag = BLK_ADD_RELCAIM_INT_FLAG;
                        ram_port->blk_map[index].ref--;
                    }

                    break;
                }

            default:
                HI_FATAL_TSIO("blk flag is unknown, blk flag is:%u\n", ram_port->blk_map[index].flag);
        }

        TSIO_FATAL_ERROR(ram_port->free_blk_nr > ram_port->total_blk_nr);
    }

    ram_port->wait_cond = 1;
    wake_up_interruptible(&ram_port->wait_queue);

    osal_mutex_unlock(&ram_port->blk_map_lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_ram_port_put_buffer_impl(struct tsio_r_ram_port *ram_port, tsio_buffer *buf)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    ram_port->put_count++;

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    /* buf->phy_addr and buf->length will be update after function call. */
    ret = check_put_buffer(ram_port, buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = _tsio_r_ram_port_put_buffer_impl(ram_port, buf);

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 _tsio_r_ram_port_close_impl(struct tsio_r_ram_port *ram_port)
{
    osal_mutex_lock(&ram_port->blk_map_lock);

    /* disable ramport. */
    tsio_hal_dis_ram_port(ram_port->base.mgmt, ram_port->base.id);

    osal_mutex_unlock(&ram_port->blk_map_lock);

    cancel_work_sync(&ram_port->flow_control_worker);
    cancel_work_sync(&ram_port->dsc_end_worker);

    /* hw suggest this experience value. */
    osal_msleep(TEN_MSECS);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_ram_port_close_impl(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    tsio_mem_info mmu_buf = {0};
    tsio_mem_info  mmz_buf = {0};

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    ret = _tsio_r_ram_port_close_impl(ram_port);
    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, ram_port->blk_map);
        ram_port->blk_map = HI_NULL;

        mmz_buf.buf_vir_addr = ram_port->dsc_ker_addr;
        mmz_buf.buf_phy_addr = ram_port->dsc_phy_addr;
        mmz_buf.buf_size = DEFAULT_RAM_DSC_DEPTH * DEFAULT_RAM_DSC_SIZE;
        mmz_buf.handle = ram_port->dsc_handle;
        mmz_buf.tsio_buf_obj = ram_port->dsc_buf_obj;
        mmz_buf.is_usr_map = HI_FALSE;

        tsio_unmap_and_release_mmz(&mmz_buf);

        ram_port->dsc_ker_addr = HI_NULL;
        ram_port->dsc_phy_addr = 0;
        ram_port->dsc_handle = -1;
        ram_port->dsc_buf_obj = HI_NULL;

        mmu_buf.buf_vir_addr = ram_port->buf_ker_addr;
        mmu_buf.buf_phy_addr = ram_port->buf_phy_addr;
        mmu_buf.buf_size = ram_port->buf_size;
        mmu_buf.handle = ram_port->buf_handle;
        mmu_buf.tsio_buf_obj = ram_port->buf_obj;
        mmu_buf.is_usr_map = (ram_port->buf_usr_addr != HI_NULL) ? HI_TRUE : HI_FALSE;

        tsio_unmap_and_release_buf(&mmu_buf);

        ram_port->buf_ker_addr = HI_NULL;
        ram_port->buf_phy_addr = 0;
        ram_port->buf_size = 0;
        ram_port->buf_handle = -1;
        ram_port->buf_obj = HI_NULL;

        ram_port->staled = HI_TRUE;
    }

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_reset_buffer_impl(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    ret = _tsio_r_ram_port_close_impl(ram_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("close ram port failed, ret:%d\n", ret);
    }

    ret = _tsio_r_ram_port_open_impl(ram_port);

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

#define IVRESTART_HEAD_LEN 16

static hi_s32 begin_push_ivrestart(struct tsio_r_ram_port *ram_port, tsio_key_type key, const hi_u8 *IV)
{
    hi_s32 ret;
    hi_u64 buf_phy_addr = 0;
    struct list_head *node = HI_NULL;
    tsio_buffer buf = {0};
    hi_u32 offset = 0;
    hi_u8 ivrestart_pkt[BULK_PKT_SIZE] = {
        [17] = key == TSIO_KEY_EVEN ? 0 : 1, /* 17 means if key is TSIO_KEY_EVEN, ivrestart_pkt[17] is 0 */
        [18] = 0x55, /* 18 ivrestart_pkt[18] is 0x55 */
        [19] = 0xff, /* 19 means ivrestart_pkt[19] is 0xff */
        [20] = 0xff, /* 20 means ivrestart_pkt[20] is 0xff */
        [21] = 0xff, /* 21 means ivrestart_pkt[21] is 0xff */
        [22] = 0, /* 22 means ivrestart_pkt[22] is 0 */
        [23] = 0, /* 23 means ivrestart_pkt[23] is 0 */
        [24] = 0, /* 24 means ivrestart_pkt[24] is 0 */
        [25 ...(BULK_PKT_SIZE - 1) ] = 0x55, /* 25 mean ivrestart_pkt[25] ~ ivrestart_pkt[BULK_PKT_SIZE - 1] is 0x55 */
    }; /* defined by nagra spec 10.12.3.4. */

    /* update IV. */
    ret = memcpy_s(ivrestart_pkt + 1, sizeof(ivrestart_pkt) - 1, IV, IVRESTART_HEAD_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("memcpy_s failed, ret(%d)\n", ret);
        goto out0;
    }

    ret = wait_dsc_sync(ram_port, 1);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    osal_mutex_lock(&ram_port->bulk_srv_lock);

    if (ram_port->bulk_srv_cnt == 0) {
        ret = HI_SUCCESS;
        goto out1;
    }

    /* get buffer */
    ret = _tsio_r_ram_port_get_buffer_impl(ram_port, sizeof(ivrestart_pkt) * ram_port->bulk_srv_cnt, &buf_phy_addr);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    buf.data = buf_phy_addr - ram_port->buf_phy_addr + ram_port->buf_ker_addr;
    buf.buf_handle = ram_port->buf_handle;
    buf.buf_offset = buf_phy_addr - ram_port->buf_phy_addr;
    buf.length = sizeof(ivrestart_pkt) * ram_port->bulk_srv_cnt;

    /* prepare ivrestart pkt. */
    list_for_each(node, &ram_port->bulk_srv_list) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, bulk_srv_node);

        ret = memcpy_s(buf.data + offset, sizeof(ivrestart_pkt), ivrestart_pkt, sizeof(ivrestart_pkt));
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("memcpy_s failed, ret(%d)\n", ret);
            goto out1;
        }

        *(buf.data + offset + 0) = TO_SE_HW_ID(se->base.id);

        offset += sizeof(ivrestart_pkt);
    }

    /* push out ivrestart. */
    push_ivr_buffer(ram_port, buf_phy_addr, buf.length);

    /* put buffer */
    ret = _tsio_r_ram_port_put_buffer_impl(ram_port, &buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("ram port put buffer failed, ret(%d)\n", ret);
    }

out1:
    osal_mutex_unlock(&ram_port->bulk_srv_lock);
out0:
    return ret;
}

static hi_s32 end_push_ivrestart(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    struct tsio_r_se *se = ram_port->ivrestart_se;

    ret = wait_dsc_sync(ram_port, 1);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    push_flush_op(ram_port, se);

    ret = wait_event_interruptible(se->wait_flush_queue, se->wait_flush_cond);
    if (ret < 0) {
        /* exit after get signal. */
        ret = HI_ERR_TSIO_INTR;
    }

out:
    return ret;
}

static inline hi_s32 push_bulk_ivrestart(struct tsio_r_ram_port *ram_port, tsio_key_type key, const hi_u8 *IV)
{
    hi_s32 ret;

    ret = begin_push_ivrestart(ram_port, key, IV);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = end_push_ivrestart(ram_port);

out:
    return ret;
}

static hi_s32 begin_flush_bulk_data(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    osal_mutex_lock(&ram_port->bulk_srv_lock);

    if (ram_port->bulk_srv_cnt == 0) {
        ret = HI_SUCCESS;
        goto out;
    }

    ret = wait_bulk_dsc_sync(ram_port);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* do flush to all target */
    list_for_each(node, &ram_port->bulk_srv_list) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, bulk_srv_node);

        push_flush_op(ram_port, se);
    }

out:
    osal_mutex_unlock(&ram_port->bulk_srv_lock);

    return ret;
}

static hi_s32 end_flush_bulk_data(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    osal_mutex_lock(&ram_port->bulk_srv_lock);

    if (ram_port->bulk_srv_cnt == 0) {
        ret = HI_SUCCESS;
        goto out;
    }

    list_for_each(node, &ram_port->bulk_srv_list) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, bulk_srv_node);

        ret = wait_event_interruptible_timeout(se->wait_flush_queue,
            se->wait_flush_cond, msecs_to_jiffies(TEN_MSECS));
        if (ret == 0) {
            ret = HI_ERR_TSIO_TIMEOUT;
            goto out;
        } else if (ret < 0) { /* signal */
            ret = HI_ERR_TSIO_INTR;
            goto out;
        }
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->bulk_srv_lock);

    return ret;
}

static hi_s32 flush_bulk_data(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret = HI_FAILURE;

    /*
     * after BeginFlushBulkData, BulkSrvList maybe changed, new added se's WaitFlushCond maybe
     * has no chance to handled by isr, so we roll loop do begin + end operation.
     */
    do {
        ret = begin_flush_bulk_data(ram_port);
        if (ret != HI_SUCCESS) {
            break;
        }

        ret = end_flush_bulk_data(ram_port);
        if (ret == HI_ERR_TSIO_INTR) { /* exit after signal. */
            break;
        }

        cond_resched();
    } while (ret != HI_SUCCESS);

    return ret;
}

static hi_s32 tsio_r_ram_port_begin_bulk_seg_impl(struct tsio_r_ram_port *ram_port, tsio_key_type key, const hi_u8 *IV)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (unlikely(ram_port->source_type != TSIO_SOURCE_BULK)) {
        HI_ERR_TSIO("only support bulk source.\n");
        ret = HI_ERR_TSIO_NOT_SUPPORT;
        goto out;
    }

    if (unlikely((key != TSIO_KEY_EVEN) && (key != TSIO_KEY_ODD))) {
        HI_ERR_TSIO("invalid key type(%d).\n", key);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (unlikely(ram_port->ivrestart_se == HI_NULL)) {
        HI_ERR_TSIO("ivrestart path not setup.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = push_bulk_ivrestart(ram_port, key, IV);

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_end_bulk_seg_impl(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (unlikely(ram_port->source_type != TSIO_SOURCE_BULK)) {
        HI_ERR_TSIO("only support bulk source.\n");
        ret = HI_ERR_TSIO_NOT_SUPPORT;
        goto out;
    }

    ret = flush_bulk_data(ram_port);

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

/*
 * hold RamPort->Lock maybe deadlock when ramport flow control is enable and waiting dsc ready.
 * when secure engine del channel ,which still want this lock.
 * reference count can ensure the validity of ramport , it's ok for BulkSrvLock instread of RamPort->Lock.
 */
static hi_s32 tsio_r_ram_port_add_bulk_target_impl(struct tsio_r_ram_port *ram_port, struct tsio_r_se *se)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = HI_NULL;
    hi_bool found = HI_FALSE;

    osal_mutex_lock(&ram_port->bulk_srv_lock);

    /* avoid repeat add same secure engine. */
    list_for_each(node, &ram_port->bulk_srv_list) {
        struct tsio_r_se *saved_se = list_entry(node, struct tsio_r_se, bulk_srv_node);

        if (se->base.id == saved_se->base.id) {
            found = HI_TRUE;
        }
    }

    if (found == HI_FALSE) {
        /* tips: if new add se wait_flush_cond is not 0 after begin_flush_bulk_data,
        end_flush_bulk_data maybe think it flush done. */
        se->wait_flush_cond = 0;

        list_add_tail(&se->bulk_srv_node, &ram_port->bulk_srv_list);
        ram_port->bulk_srv_cnt++;
        ret = HI_SUCCESS;
    }

    osal_mutex_unlock(&ram_port->bulk_srv_lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_del_bulk_target_impl(struct tsio_r_ram_port *ram_port, struct tsio_r_se *se)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = HI_NULL;
    hi_bool found = HI_FALSE;

    osal_mutex_lock(&ram_port->bulk_srv_lock);

    /* avoid repeat del same secure engine. */
    list_for_each(node, &ram_port->bulk_srv_list) {
        struct tsio_r_se *saved_se = list_entry(node, struct tsio_r_se, bulk_srv_node);

        if (se->base.id == saved_se->base.id) {
            found = HI_TRUE;
            break;
        }
    }

    if (found == HI_TRUE) {
        ram_port->bulk_srv_cnt--;
        list_del(&se->bulk_srv_node);
        ret = HI_SUCCESS;
    }

    osal_mutex_unlock(&ram_port->bulk_srv_lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_add_ivr_target_impl(struct tsio_r_ram_port *ram_port, struct tsio_r_se *se)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port->ivrestart_se != HI_NULL) {
        HI_ERR_TSIO("ram_port already setup ivrestart path.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ram_port->ivrestart_se = se;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_del_ivr_target_impl(struct tsio_r_ram_port *ram_port, const struct tsio_r_se *se)
{
    osal_mutex_lock(&ram_port->lock);

    ram_port->ivrestart_se = HI_NULL;

    osal_mutex_unlock(&ram_port->lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_ram_port_pre_mmap_impl(struct tsio_r_ram_port *ram_port,
    hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    *buf_handle = ram_port->buf_handle;
    *buf_size = ram_port->buf_size;
    *usr_addr = ram_port->buf_usr_addr;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_pst_mmap_impl(struct tsio_r_ram_port *ram_port, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&ram_port->lock);

    if (ram_port_staled(ram_port)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (ram_port->buf_usr_addr != HI_NULL) {
        HI_FATAL_TSIO("ram_port->buf_usr_addr is not null, ram_port->buf_usr_addr = %p!\n", ram_port->buf_usr_addr);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ram_port->buf_usr_addr = buf_usr_addr;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_suspend_impl(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    osal_mutex_lock(&ram_port->lock);

    TSIO_FATAL_ERROR(ram_port->staled == HI_TRUE);

    ret = _tsio_r_ram_port_close_impl(ram_port);

    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static hi_s32 tsio_r_ram_port_resume_impl(struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    osal_mutex_lock(&ram_port->lock);

    TSIO_FATAL_ERROR(ram_port->staled == HI_TRUE);

    ret = _tsio_r_ram_port_open_impl(ram_port);

    osal_mutex_unlock(&ram_port->lock);

    return ret;
}

static struct tsio_r_ram_port_ops g_tsio_ram_port_ops = {
    .open            = tsio_r_ram_port_open_impl,
    .get_attrs       = tsio_r_ram_port_get_attrs_impl,
    .set_attrs       = tsio_r_ram_port_set_attrs_impl,
    .get_status      = tsio_r_ram_port_get_status_impl,
    .get_buffer      = tsio_r_ram_port_get_buffer_impl,
    .push_buffer     = tsio_r_ram_port_push_buffer_impl,
    .put_buffer      = tsio_r_ram_port_put_buffer_impl,
    .close           = tsio_r_ram_port_close_impl,
    .reset_buffer    = tsio_r_ram_port_reset_buffer_impl,

    .begin_bulk_seg  = tsio_r_ram_port_begin_bulk_seg_impl,
    .end_bulk_seg    = tsio_r_ram_port_end_bulk_seg_impl,
    .add_bulk_target = tsio_r_ram_port_add_bulk_target_impl,
    .del_bulk_target = tsio_r_ram_port_del_bulk_target_impl,

    .add_ivr_target  = tsio_r_ram_port_add_ivr_target_impl,
    .del_ivr_target  = tsio_r_ram_port_del_ivr_target_impl,

    .pre_mmap        = tsio_r_ram_port_pre_mmap_impl,
    .pst_mmap        = tsio_r_ram_port_pst_mmap_impl,

    .suspend         = tsio_r_ram_port_suspend_impl,
    .resume          = tsio_r_ram_port_resume_impl,
};

/*
 * TSIO Pid channel functions.
 */
static hi_s32 tsio_r_pid_channel_open_impl(struct tsio_r_pid_channel *pid_channel)
{
    struct tsio_r_se *se = pid_channel->se;
    struct tsio_en_pidch_para pidch_para = {0};

    if (se == HI_NULL) {
        HI_FATAL_TSIO("pid_channel->se is null!\n");
        return HI_ERR_TSIO_INVALID_PARA;
    }

    se->pkt_size      = TS_PKT_SIZE;
    se->valid_buf_size = se->buf_size - se->buf_size % se->pkt_size;

    osal_mutex_lock(&pid_channel->lock);

    if (IS_RAMPORT(pid_channel->port)) {
        pidch_para.id = pid_channel->base.id;
        pidch_para.pid = pid_channel->pid;
        pidch_para.port_id = pid_channel->port->id;
        pidch_para.sid = pid_channel->se->base.id;
        pidch_para.is_live_ts = 0;
        tsio_hal_en_pid_channel(pid_channel->base.mgmt, &pidch_para);
    } else if (IS_TSIPORT(pid_channel->port)) {
        pidch_para.id = pid_channel->base.id;
        pidch_para.pid = pid_channel->pid;
        pidch_para.port_id = pid_channel->port->id;
        pidch_para.sid = pid_channel->se->base.id;
        pidch_para.is_live_ts = 1;
        tsio_hal_en_pid_channel(pid_channel->base.mgmt, &pidch_para);
    } else {
        HI_FATAL_TSIO("tsio port check error!\n");
    }

    osal_mutex_unlock(&pid_channel->lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_pid_channel_get_status_impl(
    struct tsio_r_pid_channel *pid_channel, tsio_pid_channel_status *status)
{
    osal_mutex_lock(&pid_channel->lock);

    status->pid = pid_channel->pid;

    osal_mutex_unlock(&pid_channel->lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_pid_channel_close_impl(struct tsio_r_pid_channel *pid_channel)
{
    TSIO_FATAL_ERROR(pid_channel->se == HI_NULL);

    osal_mutex_lock(&pid_channel->lock);

    tsio_hal_dis_pid_channel(pid_channel->base.mgmt, pid_channel->base.id);

    osal_mutex_unlock(&pid_channel->lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_r_pid_channel_connect_impl(struct tsio_r_pid_channel *pid_channel, struct tsio_r_se *se)
{
    hi_s32 ret;

    /* detect conflict */
    if (!list_empty(&se->chn_head)) {
        struct obj_node_helper *first_entry = list_first_entry(&se->chn_head, struct obj_node_helper, node);

        if (IS_PIDCHANNEL(first_entry->key)) {
            struct list_head *node = HI_NULL;

            /* detect port conflict,
            HW not support one obuf or demux fq do flow control to mutile ram port concurrently. */
            if (pid_channel->port != ((struct tsio_r_pid_channel *)first_entry->key)->port) {
                HI_ERR_TSIO("not allow add pid channel from different port.\n");
                ret = HI_ERR_TSIO_INVALID_PARA;
                goto out0;
            }

            /* detect pid conflict */
            list_for_each(node, &se->chn_head) {
                struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);
                struct tsio_r_pid_channel *tmp = (struct tsio_r_pid_channel *)obj->key;

                if (tmp != pid_channel && tmp->pid == pid_channel->pid) {
                    HI_ERR_TSIO("not allow repeat add same pid(%d) channel.\n", pid_channel->pid);
                    ret = HI_ERR_TSIO_INVALID_PARA;
                    goto out0;
                }
            }
        } else if (IS_RAWCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add raw channel and pid channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_SPCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add sp channel and pid channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_IVRCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add ivrestart channel and pid channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else {
            HI_FATAL_TSIO("first_entry->key check channel type error!\n");
        }
    }

    osal_mutex_lock(&pid_channel->lock);

    /* not support connect to multi secure engine */
    if (pid_channel->se != HI_NULL) {
        HI_ERR_TSIO("the pid channel(%d) is busy.\n", pid_channel->pid);
        ret = HI_ERR_TSIO_BUSY;
        goto out1;
    }

    pid_channel->se = se;

    ret = HI_SUCCESS;

out1:
    osal_mutex_unlock(&pid_channel->lock);
out0:
    return ret;
}

static hi_s32 tsio_r_pid_channel_dis_connect_impl(struct tsio_r_pid_channel *pid_channel)
{
    osal_mutex_lock(&pid_channel->lock);

    pid_channel->se = HI_NULL;

    osal_mutex_unlock(&pid_channel->lock);

    return HI_SUCCESS;
}

/*
 * TSIO_R_PIDChannel_Close_Impl will be called after corresponding secure engine suspend.
 * it no need extra operations.
 */
static hi_s32 tsio_r_pid_channel_suspend_impl(const struct tsio_r_pid_channel *pid_channel)
{
    return HI_SUCCESS;
}

/*
 * TSIO_R_PIDChannel_Open_Impl will be called after corresponding secure engine resume.
 * it no need extra operations.
 */
static hi_s32 tsio_r_pid_channel_resume_impl(const struct tsio_r_pid_channel *pid_channel)
{
    return HI_SUCCESS;
}

static struct tsio_r_pid_channel_ops g_tsio_pid_channel_ops = {
    .open        = tsio_r_pid_channel_open_impl,
    .get_status  = tsio_r_pid_channel_get_status_impl,
    .close       = tsio_r_pid_channel_close_impl,

    .connect     = tsio_r_pid_channel_connect_impl,
    .dis_connect = tsio_r_pid_channel_dis_connect_impl,

    .suspend     = tsio_r_pid_channel_suspend_impl,
    .resume      = tsio_r_pid_channel_resume_impl,
};

/*
 * TSIO Raw channel functions.
 */
static hi_s32 tsio_r_raw_channel_open_impl(struct tsio_r_raw_channel *raw_channel)
{
    hi_s32 ret;
    struct tsio_r_se *se = raw_channel->se;
    struct tsio_r_ram_port *ram_port = (struct tsio_r_ram_port *)raw_channel->port;

    if (se == HI_NULL) {
        HI_FATAL_TSIO("raw_channel->se is null!\n");
        return HI_ERR_TSIO_INVALID_PARA;
    }

    se->pkt_size      = BULK_PKT_SIZE;
    se->valid_buf_size = se->buf_size - se->buf_size % se->pkt_size;

    osal_mutex_lock(&raw_channel->lock);

    TSIO_FATAL_ERROR(!IS_RAMPORT(raw_channel->port));

    ret = ram_port->ops->add_bulk_target(ram_port, raw_channel->se);

    osal_mutex_unlock(&raw_channel->lock);

    return ret;
}

static hi_s32 tsio_r_raw_channel_close_impl(struct tsio_r_raw_channel *raw_channel)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = (struct tsio_r_ram_port *)raw_channel->port;

    if (raw_channel->se == HI_NULL) {
        HI_FATAL_TSIO("raw_channel->se is null!\n");
        return HI_ERR_TSIO_INVALID_PARA;
    }

    osal_mutex_lock(&raw_channel->lock);

    TSIO_FATAL_ERROR(!IS_RAMPORT(ram_port));

    ret = ram_port->ops->del_bulk_target(ram_port, raw_channel->se);

    osal_mutex_unlock(&raw_channel->lock);

    return ret;
}

static hi_s32 tsio_r_raw_channel_connect_impl(struct tsio_r_raw_channel *raw_channel, struct tsio_r_se *se)
{
    hi_s32 ret;

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        HI_ERR_TSIO("not support output raw stream to demux.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out0;
    }

    /* detect conflict */
    if (!list_empty(&se->chn_head)) {
        struct obj_node_helper *first_entry = list_first_entry(&se->chn_head, struct obj_node_helper, node);

        if (IS_PIDCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add pid channel and raw channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_RAWCHANNEL(first_entry->key)) {
            if (first_entry->key != raw_channel) {
                HI_ERR_TSIO("not allow add raw channel and raw channel in same service.\n");
                ret = HI_ERR_TSIO_INVALID_PARA;
                goto out0;
            }
        } else if (IS_SPCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add sp channel and raw channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_IVRCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add ivrestart channel and raw channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else {
            HI_FATAL_TSIO("first_entry->key check channel type error!\n");
        }
    }

    osal_mutex_lock(&raw_channel->lock);

    /* not support connect to multi secure engine */
    if (raw_channel->se != HI_NULL) {
        HI_ERR_TSIO("the raw channel is busy.\n");
        ret = HI_ERR_TSIO_BUSY;
        goto out1;
    }

    raw_channel->se = se;

    ret = HI_SUCCESS;

out1:
    osal_mutex_unlock(&raw_channel->lock);
out0:
    return ret;
}

static hi_s32 tsio_r_raw_channel_dis_connect_impl(struct tsio_r_raw_channel *raw_channel)
{
    osal_mutex_lock(&raw_channel->lock);

    TSIO_FATAL_ERROR(raw_channel->se->mod != TSIO_SECURE_ENGINE_OUTPUT2RAM);

    raw_channel->se = HI_NULL;

    osal_mutex_unlock(&raw_channel->lock);

    return HI_SUCCESS;
}

/*
 * TSIO_R_RAWChannel_Close_Impl will be called after corresponding secure engine suspend.
 * it no need extra operations.
 */
static hi_s32 tsio_r_raw_channel_suspend_impl(const struct tsio_r_raw_channel *raw_channel)
{
    return HI_SUCCESS;
}

/*
 * TSIO_R_RAWChannel_Open_Impl will be called after corresponding secure engine resume.
 * it no need extra operations.
 */
static hi_s32 tsio_r_raw_channel_resume_impl(const struct tsio_r_raw_channel *raw_channel)
{
    return HI_SUCCESS;
}

static struct tsio_r_raw_channel_ops g_tsio_raw_channel_ops = {
    .open        = tsio_r_raw_channel_open_impl,
    .close       = tsio_r_raw_channel_close_impl,

    .connect     = tsio_r_raw_channel_connect_impl,
    .dis_connect = tsio_r_raw_channel_dis_connect_impl,

    .suspend     = tsio_r_raw_channel_suspend_impl,
    .resume      = tsio_r_raw_channel_resume_impl,
};

/*
 * TSIO Sp channel functions.
 */
static hi_s32 tsio_r_sp_channel_open_impl(struct tsio_r_sp_channel *sp_channel)
{
    hi_s32 ret;
    struct tsio_r_se *se = sp_channel->se;
    struct tsio_r_ram_port *ram_port = (struct tsio_r_ram_port *)sp_channel->port;

    if (se == HI_NULL) {
        HI_FATAL_TSIO("sp_channel->se is null!\n");
        return HI_ERR_TSIO_INVALID_PARA;
    }

    se->pkt_size      = SP_PKT_SIZE;
    se->valid_buf_size = se->buf_size - se->buf_size % se->pkt_size;

    osal_mutex_lock(&sp_channel->lock);

    TSIO_FATAL_ERROR(!IS_RAMPORT(sp_channel->port));

    /* sp save must enabled before add bulk target, otherwise se maybe receive 192 pkt. */
    tsio_hal_en_sp_save(sp_channel->base.mgmt, sp_channel->se->base.id);

    ret = ram_port->ops->add_bulk_target(ram_port, sp_channel->se);
    if (ret != HI_SUCCESS) {
        tsio_hal_dis_sp_save(sp_channel->base.mgmt, sp_channel->se->base.id);
    }

    osal_mutex_unlock(&sp_channel->lock);

    return ret;
}

static hi_s32 tsio_r_sp_channel_close_impl(struct tsio_r_sp_channel *sp_channel)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = (struct tsio_r_ram_port *)sp_channel->port;

    if (sp_channel->se == HI_NULL) {
        HI_FATAL_TSIO("sp_channel->se is null!\n");
        return HI_ERR_TSIO_INVALID_PARA;
    }

    osal_mutex_lock(&sp_channel->lock);

    TSIO_FATAL_ERROR(!IS_RAMPORT(ram_port));

    ret = ram_port->ops->del_bulk_target(ram_port, sp_channel->se);

    tsio_hal_dis_sp_save(sp_channel->base.mgmt, sp_channel->se->base.id);

    osal_mutex_unlock(&sp_channel->lock);

    return ret;
}

static hi_s32 tsio_r_sp_channel_connect_impl(struct tsio_r_sp_channel *sp_channel, struct tsio_r_se *se)
{
    hi_s32 ret;

    if (TSIO_SECURE_ENGINE_OUTPUT2DMX == se->mod) {
        HI_ERR_TSIO("not support output sp stream to demux.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out0;
    }

    /* detect conflict */
    if (!list_empty(&se->chn_head)) {
        struct obj_node_helper *first_entry = list_first_entry(&se->chn_head, struct obj_node_helper, node);

        if (IS_PIDCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add pid channel and sp channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_RAWCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add raw channel and sp channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_SPCHANNEL(first_entry->key)) {
            if (first_entry->key != sp_channel) {
                HI_ERR_TSIO("not allow add sp channel and sp channel in same service.\n");
                ret = HI_ERR_TSIO_INVALID_PARA;
                goto out0;
            }
        } else if (IS_IVRCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add ivrestart channel and sp channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else {
            HI_FATAL_TSIO("first_entry->key check channel type error!\n");
        }
    }

    osal_mutex_lock(&sp_channel->lock);

    /* not support connect to multi secure engine */
    if (sp_channel->se != HI_NULL) {
        HI_ERR_TSIO("the sp channel is busy.\n");
        ret = HI_ERR_TSIO_BUSY;
        goto out1;
    }

    sp_channel->se = se;

    ret = HI_SUCCESS;

out1:
    osal_mutex_unlock(&sp_channel->lock);
out0:
    return ret;
}

static hi_s32 tsio_r_sp_channel_dis_connect_impl(struct tsio_r_sp_channel *sp_channel)
{
    osal_mutex_lock(&sp_channel->lock);

    TSIO_FATAL_ERROR(TSIO_SECURE_ENGINE_OUTPUT2RAM != sp_channel->se->mod);

    sp_channel->se = HI_NULL;

    osal_mutex_unlock(&sp_channel->lock);

    return HI_SUCCESS;
}

/*
 * TSIO_R_SPChannel_Close_Impl will be called after corresponding secure engine suspend.
 * it no need extra operations.
 */
static hi_s32 tsio_r_sp_channel_suspend_impl(const struct tsio_r_sp_channel *sp_channel)
{
    return HI_SUCCESS;
}

/*
 * TSIO_R_SPChannel_Open_Implwill be called after corresponding secure engine resume.
 * it no need extra operations.
 */
static hi_s32 tsio_r_sp_channel_resume_impl(const struct tsio_r_sp_channel *sp_channel)
{
    return HI_SUCCESS;
}

static struct tsio_r_sp_channel_ops g_tsio_sp_channel_ops = {
    .open        = tsio_r_sp_channel_open_impl,
    .close       = tsio_r_sp_channel_close_impl,

    .connect     = tsio_r_sp_channel_connect_impl,
    .dis_connect = tsio_r_sp_channel_dis_connect_impl,

    .suspend     = tsio_r_sp_channel_suspend_impl,
    .resume      = tsio_r_sp_channel_resume_impl,
};

/*
 * TSIO IVRestart channel functions.
 */
static hi_s32 tsio_r_ivr_channel_open_impl(struct tsio_r_ivr_channel *ivr_channel)
{
    hi_s32 ret;
    struct tsio_r_se *se = ivr_channel->se;
    struct tsio_r_ram_port *ram_port = (struct tsio_r_ram_port *)ivr_channel->port;

    if (se == HI_NULL) {
        HI_FATAL_TSIO("ivr_channel->se is null!\n");
        return HI_ERR_TSIO_INVALID_PARA;
    }

    se->pkt_size      = BULK_PKT_SIZE;
    se->valid_buf_size = se->buf_size - se->buf_size % se->pkt_size;

    osal_mutex_lock(&ivr_channel->lock);

    TSIO_FATAL_ERROR(!IS_RAMPORT(ivr_channel->port));

    ret = ram_port->ops->add_ivr_target(ram_port, ivr_channel->se);

    osal_mutex_unlock(&ivr_channel->lock);

    return ret;
}

static hi_s32 tsio_r_ivr_channel_close_impl(struct tsio_r_ivr_channel *ivr_channel)
{
    hi_s32 ret;
    struct tsio_r_ram_port *ram_port = (struct tsio_r_ram_port *)ivr_channel->port;

    if (ivr_channel->se == HI_NULL) {
        HI_FATAL_TSIO("ivr_channel->se is null!\n");
        return HI_ERR_TSIO_INVALID_PARA;
    }

    osal_mutex_lock(&ivr_channel->lock);

    TSIO_FATAL_ERROR(!IS_RAMPORT(ram_port));

    ret = ram_port->ops->del_ivr_target(ram_port, ivr_channel->se);

    osal_mutex_unlock(&ivr_channel->lock);

    return ret;
}

static hi_s32 tsio_r_ivr_channel_connect_impl(struct tsio_r_ivr_channel *ivr_channel, struct tsio_r_se *se)
{
    hi_s32 ret;

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        HI_ERR_TSIO("not support output ivr stream to demux.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out0;
    }

    /* detect conflict */
    if (!list_empty(&se->chn_head)) {
        struct obj_node_helper *first_entry = list_first_entry(&se->chn_head, struct obj_node_helper, node);

        if (IS_PIDCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add pid channel and ivrestart channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_RAWCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add raw channel and ivrestart channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_SPCHANNEL(first_entry->key)) {
            HI_ERR_TSIO("not allow add sp channel and ivrestart channel in same service.\n");
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out0;
        } else if (IS_IVRCHANNEL(first_entry->key)) {
            struct list_head *node = HI_NULL;

            /* each bulk ram port has its own one ivrestart channel connect to same secure engine. */
            list_for_each(node, &se->chn_head) {
                struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);
                struct tsio_r_ivr_channel *tmp = (struct tsio_r_ivr_channel *)obj->key;

                if (tmp != ivr_channel && tmp->port == ivr_channel->port) {
                    HI_ERR_TSIO("not allow repeat add same ram port.\n");
                    ret = HI_ERR_TSIO_INVALID_PARA;
                    goto out0;
                }
            }
        } else {
            HI_FATAL_TSIO("first_entry->key check channel type error!\n");
        }
    }

    osal_mutex_lock(&ivr_channel->lock);

    /* not support connect to multi secure engine */
    if (ivr_channel->se != HI_NULL) {
        HI_ERR_TSIO("the ivrestart channel is busy.\n");
        ret = HI_ERR_TSIO_BUSY;
        goto out1;
    }

    ivr_channel->se = se;

    ret = HI_SUCCESS;

out1:
    osal_mutex_unlock(&ivr_channel->lock);
out0:
    return ret;
}

static hi_s32 tsio_r_ivr_channel_dis_connect_impl(struct tsio_r_ivr_channel *ivr_channel)
{
    osal_mutex_lock(&ivr_channel->lock);

    TSIO_FATAL_ERROR(ivr_channel->se->mod != TSIO_SECURE_ENGINE_OUTPUT2RAM);

    ivr_channel->se = HI_NULL;

    osal_mutex_unlock(&ivr_channel->lock);

    return HI_SUCCESS;
}

/*
 * TSIO_R_PIDChannel_Close_Impl will be called after corresponding secure engine suspend.
 * it no need extra operations.
 */
static hi_s32 tsio_r_ivr_channel_suspend_impl(const struct tsio_r_ivr_channel *ivr_channel)
{
    return HI_SUCCESS;
}

/*
 * TSIO_R_IVRChannel_Open_Impl will be called after corresponding secure engine resume.
 * it no need extra operations.
 */
static hi_s32 tsio_r_ivr_channel_resume_impl(const struct tsio_r_ivr_channel *ivr_channel)
{
    return HI_SUCCESS;
}

static struct tsio_r_ivr_channel_ops g_tsio_ivr_channel_ops = {
    .open        = tsio_r_ivr_channel_open_impl,
    .close       = tsio_r_ivr_channel_close_impl,

    .connect     = tsio_r_ivr_channel_connect_impl,
    .dis_connect = tsio_r_ivr_channel_dis_connect_impl,

    .suspend     = tsio_r_ivr_channel_suspend_impl,
    .resume      = tsio_r_ivr_channel_resume_impl,
};

/*
 * obj is staled after PstClose, it should discard all possible call request after that.
 */
static inline hi_bool se_staled(const struct tsio_r_se *se)
{
    if (unlikely(se->staled == HI_TRUE)) {
        HI_ERR_TSIO("secure engine is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 add_pid_channel(struct tsio_r_se *se, struct tsio_r_pid_channel *pid_channel)
{
    hi_s32 ret;

    /* establish reference to channel obj. */
    tsio_r_get_raw((struct tsio_r_base *)pid_channel);

    ret = pid_channel->ops->connect(pid_channel, se);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (se->stat == TSIO_SECURE_ENGINE_OPENED) {
        ret = pid_channel->ops->open(pid_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("open pid(%d) channel(%d) failed.\n", pid_channel->pid, pid_channel->base.id);
            goto out1;
        }
    }

    return HI_SUCCESS;

out1:
    pid_channel->ops->dis_connect(pid_channel);
out0:
    tsio_r_put((struct tsio_r_base *)pid_channel);
    return ret;
}

static inline hi_s32 del_pid_channel(const struct tsio_r_se *se, struct tsio_r_pid_channel *pid_channel)
{
    hi_s32 ret;

    /* not care about return error code, maybe it has not opened. */
    pid_channel->ops->close(pid_channel);

    ret = pid_channel->ops->dis_connect(pid_channel);
    if (ret == HI_SUCCESS) {
        tsio_r_put((struct tsio_r_base *)pid_channel);
    }

    return ret;
}

static hi_s32 add_raw_channel(struct tsio_r_se *se, struct tsio_r_raw_channel *raw_channel)
{
    hi_s32 ret;

    /* establish reference to channel obj. */
    tsio_r_get_raw((struct tsio_r_base *)raw_channel);

    ret = raw_channel->ops->connect(raw_channel, se);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (se->stat == TSIO_SECURE_ENGINE_OPENED) {
        ret = raw_channel->ops->open(raw_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("open raw channel(%d) failed.\n", raw_channel->base.id);
            goto out1;
        }
    }

    return HI_SUCCESS;

out1:
    raw_channel->ops->dis_connect(raw_channel);
out0:
    tsio_r_put((struct tsio_r_base *)raw_channel);
    return ret;
}

static inline hi_s32 del_raw_channel(const struct tsio_r_se *se, struct tsio_r_raw_channel *raw_channel)
{
    hi_s32 ret;

    /* not care about return error code, maybe it has not opened. */
    raw_channel->ops->close(raw_channel);

    ret = raw_channel->ops->dis_connect(raw_channel);
    if (ret == HI_SUCCESS) {
        tsio_r_put((struct tsio_r_base *)raw_channel);
    }

    return ret;
}

static hi_s32 add_sp_channel(struct tsio_r_se *se, struct tsio_r_sp_channel *sp_channel)
{
    hi_s32 ret;

    /* establish reference to channel obj. */
    tsio_r_get_raw((struct tsio_r_base *)sp_channel);

    ret = sp_channel->ops->connect(sp_channel, se);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (se->stat == TSIO_SECURE_ENGINE_OPENED) {
        ret = sp_channel->ops->open(sp_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("open sp channel(%d) failed.\n", sp_channel->base.id);
            goto out1;
        }
    }

    return HI_SUCCESS;

out1:
    sp_channel->ops->dis_connect(sp_channel);
out0:
    tsio_r_put((struct tsio_r_base *)sp_channel);
    return ret;
}

static inline hi_s32 del_sp_channel(const struct tsio_r_se *se, struct tsio_r_sp_channel *sp_channel)
{
    hi_s32 ret;

    /* not care about return error code, maybe it has not opened. */
    sp_channel->ops->close(sp_channel);

    ret = sp_channel->ops->dis_connect(sp_channel);
    if (ret == HI_SUCCESS) {
        tsio_r_put((struct tsio_r_base *)sp_channel);
    }

    return ret;
}

static hi_s32 add_ivr_channel(struct tsio_r_se *se, struct tsio_r_ivr_channel *ivr_channel)
{
    hi_s32 ret;

    /* establish reference to channel obj. */
    tsio_r_get_raw((struct tsio_r_base *)ivr_channel);

    ret = ivr_channel->ops->connect(ivr_channel, se);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (se->stat == TSIO_SECURE_ENGINE_OPENED) {
        ret = ivr_channel->ops->open(ivr_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("open ivrestart channel(%d) failed.\n", ivr_channel->base.id);
            goto out1;
        }
    }

    return HI_SUCCESS;

out1:
    ivr_channel->ops->dis_connect(ivr_channel);
out0:
    tsio_r_put((struct tsio_r_base *)ivr_channel);
    return ret;
}

static inline hi_s32 del_ivr_channel(const struct tsio_r_se *se, struct tsio_r_ivr_channel *ivr_channel)
{
    hi_s32 ret;

    /* not care about return error code, maybe it has not opened. */
    ivr_channel->ops->close(ivr_channel);

    ret = ivr_channel->ops->dis_connect(ivr_channel);
    if (ret == HI_SUCCESS) {
        tsio_r_put((struct tsio_r_base *)ivr_channel);
    }

    return ret;
}

static hi_s32 open_all_channels(struct tsio_r_se *se)
{
    hi_s32 ret = HI_SUCCESS;
    struct list_head *node = HI_NULL;

    list_for_each(node, &se->chn_head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);
        struct tsio_r_base *chn = obj->key;

        if (IS_PIDCHANNEL(chn)) {
            struct tsio_r_pid_channel *pid_channel = (struct tsio_r_pid_channel *)chn;

            ret = pid_channel->ops->open(pid_channel);
        } else if (IS_RAWCHANNEL(chn)) {
            struct tsio_r_raw_channel *raw_channel = (struct tsio_r_raw_channel *)chn;

            ret = raw_channel->ops->open(raw_channel);
        } else if (IS_SPCHANNEL(chn)) {
            struct tsio_r_sp_channel *sp_channel = (struct tsio_r_sp_channel *)chn;

            ret = sp_channel->ops->open(sp_channel);
        } else if (IS_IVRCHANNEL(chn)) {
            struct tsio_r_ivr_channel *ivr_channel = (struct tsio_r_ivr_channel *)chn;

            ret = ivr_channel->ops->open(ivr_channel);
        } else {
            HI_FATAL_TSIO("channel is unknown!\n");
        }

        TSIO_FATAL_ERROR(ret != HI_SUCCESS);
    }

    return ret;
}

static hi_s32 close_all_channels(struct tsio_r_se *se)
{
    hi_s32 ret = HI_SUCCESS;
    struct list_head *node = HI_NULL;

    list_for_each(node, &se->chn_head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);
        struct tsio_r_base *chn = obj->key;

        if (IS_PIDCHANNEL(chn)) {
            struct tsio_r_pid_channel *pid_channel = (struct tsio_r_pid_channel *)chn;

            ret = pid_channel->ops->close(pid_channel);
        } else if (IS_RAWCHANNEL(chn)) {
            struct tsio_r_raw_channel *raw_channel = (struct tsio_r_raw_channel *)chn;

            ret = raw_channel->ops->close(raw_channel);
        } else if (IS_SPCHANNEL(chn)) {
            struct tsio_r_sp_channel *sp_channel = (struct tsio_r_sp_channel *)chn;

            ret = sp_channel->ops->close(sp_channel);
        } else if (IS_IVRCHANNEL(chn)) {
            struct tsio_r_ivr_channel *ivr_channel = (struct tsio_r_ivr_channel *)chn;

            ret = ivr_channel->ops->close(ivr_channel);
        } else {
            HI_FATAL_TSIO("channel is unknown!\n");
        }

        TSIO_FATAL_ERROR(ret != HI_SUCCESS);
    }

    return ret;
}

static hi_s32 del_all_channels(struct tsio_r_se *se)
{
    hi_s32 ret = HI_SUCCESS;
    struct list_head *node = HI_NULL;
    struct list_head *tmp = HI_NULL;

    list_for_each_safe(node, tmp, &se->chn_head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);
        struct tsio_r_base *chn = obj->key;

        if (IS_PIDCHANNEL(chn)) {
            ret = del_pid_channel(se, (struct tsio_r_pid_channel *)chn);
        } else  if (IS_RAWCHANNEL(chn)) {
            ret = del_raw_channel(se, (struct tsio_r_raw_channel *)chn);
        } else if (IS_SPCHANNEL(chn)) {
            ret = del_sp_channel(se, (struct tsio_r_sp_channel *)chn);
        } else if (IS_IVRCHANNEL(chn)) {
            ret = del_ivr_channel(se, (struct tsio_r_ivr_channel *)chn);
        } else {
            HI_FATAL_TSIO("channel is unknown!\n");
        }

        TSIO_FATAL_ERROR(ret != HI_SUCCESS);

        list_del(node);

        HI_KFREE(HI_ID_TSIO, obj);
    }

    /* updated when new channel added. */
    se->pkt_size      = 0;
    se->valid_buf_size = 0;

    return ret;
}

/*
 * this algorithm is ok until buffer overflow when CurHwWrite wrap up to CurHwRead.
 * it should reset buffer after overflow, refer to tsio_r_se_HandleBufOvfl_Impl.
 */
static hi_void get_avail_cont_data(const struct tsio_r_se *se, hi_u32 *data_no_wrap_len, hi_u32 *data_wrap_len)
{
    /* hw only use pkt align size of buf_size. */
    hi_u32 cur_hw_write = tsio_hal_get_obuf_write(se->base.mgmt, se->base.id);

    TSIO_FATAL_ERROR(se->valid_buf_size == 0);

    /*
     * [--- CurHwRead --- CurSwRead --- CurHwWrite ---]
     * [--- CurSwRead --- CurHwWrite --- CurHwRead ---]
     */
    if (se->cur_sw_read <= cur_hw_write) {
        *data_no_wrap_len = cur_hw_write - se->cur_sw_read;
        *data_wrap_len    = *data_no_wrap_len;
    } else if (se->cur_sw_read > cur_hw_write) { /* [--- cur_hw_write --- cur_hw_read --- cur_sw_read ---] */
        if (se->cur_sw_read == se->valid_buf_size) {
            *data_no_wrap_len = cur_hw_write;
            *data_wrap_len    = *data_no_wrap_len;
        } else {
            *data_no_wrap_len = se->valid_buf_size - se->cur_sw_read;
            *data_wrap_len    = *data_no_wrap_len + cur_hw_write;
        }
    }
}

/*
 * the difference between GetAvailContData and GetHwAvailContData is that the available data obtained by the former
 * is subset of the latter. the latter feedback is the real situation of the buffer.
 */
static hi_void get_hw_avail_cont_data(const struct tsio_r_se *se, hi_u32 *data_no_wrap_len, hi_u32 *data_wrap_len)
{
    /* hw only use pkt align size of buf_size. */
    hi_u32 cur_hw_read  = tsio_hal_get_obuf_read(se->base.mgmt, se->base.id);
    hi_u32 cur_hw_write = tsio_hal_get_obuf_write(se->base.mgmt, se->base.id);

    TSIO_FATAL_ERROR(se->valid_buf_size == 0);

    /*
     * [--- CurHwRead --- CurHwWrite ---]
     * [--- CurHwWrite --- CurHwRead ---]
     */
    if (cur_hw_read <= cur_hw_write) {
        *data_no_wrap_len = cur_hw_write - cur_hw_read;
        *data_wrap_len    = *data_no_wrap_len;
    } else if (cur_hw_read > cur_hw_write) { /* [--- cur_hw_write --- cur_hw_read ---] */
        if (cur_hw_read == se->valid_buf_size) {
            *data_no_wrap_len = cur_hw_write;
            *data_wrap_len    = *data_no_wrap_len;
        } else {
            *data_no_wrap_len = se->valid_buf_size - cur_hw_read;
            *data_wrap_len    = *data_no_wrap_len + cur_hw_write;
        }
    }
}

static hi_s32 check_rel_buffer(const struct tsio_r_se *se, const tsio_buffer *buf)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 cur_hw_read = tsio_hal_get_obuf_read(se->base.mgmt, se->base.id);
    if (unlikely(cur_hw_read != buf->buf_offset)) {
        HI_ERR_TSIO("invalid buffer[0x%llx,%d] range.\n", se->buf_phy_addr + buf->buf_offset, buf->length);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out0;
    }

    /* hw restrict that len must align with pkt size */
    if (unlikely(buf->length % se->pkt_size)) {
        HI_ERR_TSIO("buffer length(%d) not align with pkt size(%d).\n", buf->length, se->pkt_size);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out0;
    }

    /* buf can't beyond se->cur_sw_read. */
    if (cur_hw_read < se->cur_sw_read) {
        if (se->cur_sw_read - cur_hw_read < buf->length) {
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out1;
        }
    } else if (cur_hw_read > se->cur_sw_read) {
        if (cur_hw_read == se->valid_buf_size) {
            if (se->cur_sw_read < buf->length) {
                ret = HI_ERR_TSIO_INVALID_PARA;
                goto out1;
            }
        } else {
            if (se->valid_buf_size - cur_hw_read + se->cur_sw_read < buf->length) {
                ret = HI_ERR_TSIO_INVALID_PARA;
                goto out1;
            }
        }
    }

    return HI_SUCCESS;

out1:
    HI_ERR_TSIO("ring buffer abnormal status(%d, %d, %d).\n", cur_hw_read, se->cur_sw_read, buf->length);
out0:
    return ret;
}

static hi_void enable_obuf(struct tsio_r_se *se)
{
    se->cur_sw_read = 0;

    /* en output buffer, hw write will be reset. */
    tsio_hal_en_obuf(se->base.mgmt, se->base.id, se->buf_phy_addr, se->buf_size);

    tsio_hal_en2ram_srv(se->base.mgmt, se->base.id);
}

static hi_void disable_obuf(struct tsio_r_se *se)
{
    tsio_hal_dis2ram_srv(se->base.mgmt, se->base.id);

    /* hw inner cache pkt maybe transmit to obuf after tsio_hal_dis2ram_srv. */
    osal_mdelay(1);

    tsio_hal_dis_obuf(se->base.mgmt, se->base.id);

    se->cur_sw_read  = 0;
}

static hi_void obuf_ovfl_handler(struct work_struct *work)
{
    struct tsio_r_se *se =  container_of(work, struct tsio_r_se, buf_ovfl_worker);

    se->ops->handle_buf_ovfl(se);
}

static hi_s32 tsio_r_se_pre_open_impl(struct tsio_r_se *se)
{
    hi_s32 ret = HI_FAILURE;
    hi_char buf_name[TSIO_BUF_NAME_LEN] = "tsio_se_buf";
    hi_u32 buf_name_index = sizeof("tsio_se_buf") - 1;
    tsio_mem_info mmu_buf = {0};

    osal_mutex_lock(&se->lock);

    TSIO_FATAL_ERROR(se->staled == HI_TRUE);
    TSIO_FATAL_ERROR(se->stat != TSIO_SECURE_ENGINE_CLOSED);

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        ret = HI_SUCCESS;
    } else if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        /* alloc buf, hw restrict that it must align with 16 bytes. */
        buf_name[buf_name_index] = '0' + se->base.id / ALIGN_LEN % ALIGN_LEN;
        buf_name[buf_name_index + 1] = '0' + se->base.id % ALIGN_LEN;
        ret = tsio_alloc_and_map_buf(buf_name, se->buf_size, HI_FALSE, &mmu_buf);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("mmu malloc 0x%x failed\n", se->buf_size);
            goto out;
        }

        TSIO_FATAL_ERROR(mmu_buf.buf_phy_addr % PHY_ADDR_ALIGN_BYTES || se->buf_size != mmu_buf.buf_size);

        se->buf_handle   = mmu_buf.handle;
        se->buf_obj      = mmu_buf.tsio_buf_obj;
        se->buf_usr_addr = HI_NULL;
        se->buf_ker_addr = mmu_buf.buf_vir_addr;
        se->buf_phy_addr = mmu_buf.buf_phy_addr;
    } else {
        HI_FATAL_TSIO("tsio_r_se's mod mismatch, tsio_r_se's mod is:%u\n", se->mod);
    }

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 _tsio_r_se_open_impl(struct tsio_r_se *se)
{
    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        hi_u32 dmx_port_id = se->dest_port - DMX_TSIO_PORT_0 ;

        tsio_hal_en2dmx_srv(se->base.mgmt, se->base.id, dmx_port_id);
    } else if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        se->acq_count       = 0;
        se->acq_valid_count = 0;
        se->rel_count       = 0;

        enable_obuf(se);
    } else {
        HI_FATAL_TSIO("tsio_r_se's mod mismatch, tsio_r_se's mod is:%u\n", se->mod);
    }

    return open_all_channels(se);
}

static hi_s32 tsio_r_se_open_impl(struct tsio_r_se *se)
{
    hi_s32 ret;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (unlikely(se->stat == TSIO_SECURE_ENGINE_OPENED)) {
        ret = HI_SUCCESS;
        goto out;
    }

    ret = _tsio_r_se_open_impl(se);
    if (ret == HI_SUCCESS) {
        se->stat = TSIO_SECURE_ENGINE_OPENED;
    }

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 tsio_r_se_get_attrs_impl(struct tsio_r_se *se, tsio_secure_engine_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    attrs->mod = se->mod;

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        attrs->dest_port = se->dest_port;
    } else if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        attrs->buffer_size = se->buf_size;
    } else {
        HI_FATAL_TSIO("tsio_r_se's mod mismatch, tsio_r_se's mod is:%u\n", se->mod);
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

/*
 * WARNNING:
 * there are some constraints if you need change configuration here.
 * please refer to TSIO_Mgmt_CreateSe_Impl.
 */
static hi_s32 tsio_r_se_set_attrs_impl(const struct tsio_r_se *se, const tsio_secure_engine_attrs *attrs)
{
    HI_ERR_TSIO("not support alter secure engine configuration.\n");

    return HI_ERR_TSIO_NOT_SUPPORT;
}

static hi_s32 tsio_r_se_get_status_impl(struct tsio_r_se *se, tsio_secure_engine_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    status->sid    = se->base.id + TSIO_SID_0;
    status->state  = se->stat;

    status->buffer_used = 0;
    status->buffer_size = se->buf_size;

    if ((se->stat == TSIO_SECURE_ENGINE_OPENED) && (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM)) {
        if (!list_empty(&se->chn_head)) {
            hi_u32 data_no_wrap_len = 0;
            hi_u32 data_wrap_len    = 0;

            get_hw_avail_cont_data(se, &data_no_wrap_len, &data_wrap_len);

            status->buffer_used = data_wrap_len;
            status->buffer_size = se->valid_buf_size;
        }
    }

    status->dma_end_int_cnt = se->dma_end_int_cnt;
    status->overflow_cnt    = se->overflow_cnt;

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 tsio_r_se_add_channel_impl(struct tsio_r_se *se, struct tsio_r_base *chn)
{
    hi_s32 ret = HI_FAILURE;
    struct obj_node_helper *new_obj = HI_NULL;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out0;
    }

    /* add reference between se and channel. */
    new_obj = HI_KZALLOC(HI_ID_TSIO, sizeof(struct obj_node_helper), GFP_KERNEL);
    if (new_obj == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out0;
    }

    /* bind channel to secure engine and open if necessary. */
    if (IS_PIDCHANNEL(chn)) {
        ret = add_pid_channel(se, (struct tsio_r_pid_channel *)chn);
    } else if (IS_RAWCHANNEL(chn)) {
        ret = add_raw_channel(se, (struct tsio_r_raw_channel *)chn);
    } else if (IS_SPCHANNEL(chn)) {
        ret = add_sp_channel(se, (struct tsio_r_sp_channel *)chn);
    } else if (IS_IVRCHANNEL(chn)) {
        ret = add_ivr_channel(se, (struct tsio_r_ivr_channel *)chn);
    } else {
        HI_ERR_TSIO("invalid channel handle.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
    }

    if (ret != HI_SUCCESS) {
        goto out1;
    }

    new_obj->key = (hi_void *)chn;
    list_add_tail(&new_obj->node, &se->chn_head);

    osal_mutex_unlock(&se->lock);

    return HI_SUCCESS;

out1:
    HI_KFREE(HI_ID_TSIO, new_obj);
out0:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 _tsio_r_se_del_channel_impl(struct tsio_r_se *se, struct tsio_r_base *chn)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = HI_NULL;
    struct list_head *tmp = HI_NULL;

    list_for_each_safe(node, tmp, &se->chn_head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);

        if (obj->key == (hi_void *)chn) {
            if (IS_PIDCHANNEL(chn)) {
                ret = del_pid_channel(se, (struct tsio_r_pid_channel *)chn);
            } else if (IS_RAWCHANNEL(chn)) {
                ret = del_raw_channel(se, (struct tsio_r_raw_channel *)chn);
            } else if (IS_SPCHANNEL(chn)) {
                ret = del_sp_channel(se, (struct tsio_r_sp_channel *)chn);
            } else if (IS_IVRCHANNEL(chn)) {
                ret = del_ivr_channel(se, (struct tsio_r_ivr_channel *)chn);
            } else {
                HI_FATAL_TSIO("channel is mismatch!\n");
            }

            if (ret == HI_SUCCESS) {
                list_del(node);

                HI_KFREE(HI_ID_TSIO, obj);
            }

            break;
        }
    }

    return ret;
}

static hi_s32 tsio_r_se_del_channel_impl(struct tsio_r_se *se, struct tsio_r_base *chn)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *node = HI_NULL;
    struct list_head *tmp = HI_NULL;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    /* ensure chn is valid. */
    list_for_each_safe(node, tmp, &se->chn_head) {
        struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);

        if (obj->key == (hi_void *)chn) {
            ret = HI_SUCCESS;
            break;
        }
    }

    if (ret != HI_SUCCESS) {
        ret = HI_ERR_TSIO_INVALID_PARA;
        HI_ERR_TSIO("invalid channel handle.\n");
        goto out;
    }

    ret = _tsio_r_se_del_channel_impl(se, chn);

    /* updated when new channel added. */
    if (list_empty(&se->chn_head)) {
        se->pkt_size      = 0;
        se->valid_buf_size = 0;
    }

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 tsio_r_se_del_all_channels_impl(struct tsio_r_se *se)
{
    hi_s32 ret;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    ret = del_all_channels(se);

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 _tsio_r_se_acquire_buffer_impl(const struct tsio_r_se *se, hi_u32 req_len, tsio_buffer *buf,
                                             hi_u32 time_out)
{
    hi_s32 ret;
    hi_u32 data_no_wrap_len = 0;
    hi_u32 data_wrap_len = 0;

    get_avail_cont_data(se, &data_no_wrap_len, &data_wrap_len);
    if (data_no_wrap_len == 0) {
        ret = HI_ERR_TSIO_NOAVAILABLE_DATA;
        goto out;
    }

    buf->data     = se->buf_usr_addr ? se->buf_usr_addr + se->cur_sw_read : se->buf_ker_addr + se->cur_sw_read;
    buf->buf_handle = se->buf_handle;
    buf->buf_offset = se->cur_sw_read;

    if (data_no_wrap_len >= req_len) {
        buf->length = req_len;
    } else { /* data_no_wrap_len < req_len */
        if (time_out) {
            ret = HI_ERR_TSIO_NOAVAILABLE_DATA;
            goto out;
        } else {
            buf->length = data_no_wrap_len;
        }
    }

    /*
     * overflow may cause Buf->length not aligned with Se->PktSize.
     * for example, pbcc pipeline not support flow control, overflow probability becomes larger.
     * return HI_ERR_TSIO_NOAVAILABLE_DATA until tsio_r_se_HandleBufOvfl_Impl reset buffer.
     */
    if (buf->length % se->pkt_size) {
        HI_ERR_TSIO("please check to see if there is an overflow log in the vicinity,"
            "if you can ignore this log, otherwise it is a fatal error.\n");
        ret = HI_ERR_TSIO_NOAVAILABLE_DATA;
        goto out;
    }

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_r_se_acquire_buffer_impl(struct tsio_r_se *se, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&se->lock);

    se->acq_count++;

    do {
        if (se_staled(se)) {
            ret = HI_ERR_TSIO_STALED;
            goto out;
        }

        if (unlikely(se->stat != TSIO_SECURE_ENGINE_OPENED)) {
            HI_ERR_TSIO("secure engine not started.\n");
            ret = HI_ERR_TSIO_NOT_READY;
            goto out;
        }

        if (unlikely(se->mod != TSIO_SECURE_ENGINE_OUTPUT2RAM)) {
            HI_ERR_TSIO("only OUTPUT2RAM mode support acquire buffer.\n");
            ret = HI_ERR_TSIO_NOT_SUPPORT;
            goto out;
        }

        /* secure engine opened ,but no bind any channel. */
        if (unlikely(list_empty(&se->chn_head))) {
            HI_ERR_TSIO("secure engine not bind any channel.\n");
            ret = HI_ERR_TSIO_NOT_READY;
            goto out;
        }

        /* hw restrict that len must align with pkt size */
        if (unlikely(req_len > se->valid_buf_size || req_len % se->pkt_size)) {
            HI_ERR_TSIO("invalid request length(%d).\n", req_len);
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }

        ret = _tsio_r_se_acquire_buffer_impl(se, req_len, buf, time_out);
        if (ret != HI_SUCCESS) {
            if (time_out == 0) {
                ret = HI_ERR_TSIO_NOAVAILABLE_DATA;
                goto out;
            }

            osal_mutex_unlock(&se->lock);

            se->wait_cond = 0;
            ret = wait_event_interruptible_timeout(se->wait_queue, se->wait_cond, msecs_to_jiffies(time_out));

            osal_mutex_lock(&se->lock);

            if (ret == 0) {
                /* timeout ,return available data */
            } else if (ret < 0) {
                goto out;
            }

            time_out = jiffies_to_msecs(ret);
        } else {
            se->acq_valid_count++;
            break;
        }
    } while (1);

    /* update sw read offset. */
    se->cur_sw_read = (se->cur_sw_read + buf->length >= se->valid_buf_size) ? 0 : se->cur_sw_read + buf->length;

out:
    osal_mutex_unlock(&se->lock);
    return ret;
}

static hi_s32 tsio_r_se_release_buffer_impl(struct tsio_r_se *se, const tsio_buffer *buf)
{
    hi_s32 ret;
    hi_u32 cur_hw_read;

    osal_mutex_lock(&se->lock);

    se->rel_count++;

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (unlikely(se->stat != TSIO_SECURE_ENGINE_OPENED)) {
        HI_ERR_TSIO("secure engine not started.\n");
        ret = HI_ERR_TSIO_NOT_READY;
        goto out;
    }

    if (unlikely(se->mod != TSIO_SECURE_ENGINE_OUTPUT2RAM)) {
        HI_ERR_TSIO("only OUTPUT2RAM mode support release buffer.\n");
        ret = HI_ERR_TSIO_NOT_SUPPORT;
        goto out;
    }

    /* secure engine no bind any channel. */
    if (unlikely(list_empty(&se->chn_head))) {
        HI_ERR_TSIO("secure engine not bind any channel.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = check_rel_buffer(se, buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* update hw read offset */
    cur_hw_read = tsio_hal_get_obuf_read(se->base.mgmt, se->base.id);
    cur_hw_read = (cur_hw_read + buf->length >= se->valid_buf_size) ? 0 : cur_hw_read + buf->length;

    tsio_hal_set_obuf_read(se->base.mgmt, se->base.id, cur_hw_read);

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 _tsio_r_se_close_impl(struct tsio_r_se *se)
{
    close_all_channels(se);

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        tsio_hal_dis2dmx_srv(se->base.mgmt, se->base.id);
    } else if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        disable_obuf(se);
    } else {
        HI_FATAL_TSIO("tsio_r_se's mod mismatch, tsio_r_se's mod is:%u\n", se->mod);
    }

    return HI_SUCCESS;
}

static hi_s32 tsio_r_se_close_impl(struct tsio_r_se *se)
{
    hi_s32 ret;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (unlikely(se->stat == TSIO_SECURE_ENGINE_CLOSED)) {
        ret = HI_SUCCESS;
        goto out;
    }

    ret = _tsio_r_se_close_impl(se);
    if (ret == HI_SUCCESS) {
        se->stat = TSIO_SECURE_ENGINE_CLOSED;
    }

out:
    osal_mutex_unlock(&se->lock);

    /* hold se->lock call this maybe deadlock. */
    if (ret == HI_SUCCESS) {
        cancel_work_sync(&se->buf_ovfl_worker);
    }

    return ret;
}

static hi_s32 tsio_r_se_pst_close_impl(struct tsio_r_se *se)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&se->lock);

    TSIO_FATAL_ERROR(se->stat != TSIO_SECURE_ENGINE_CLOSED);

    del_all_channels(se);

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        ret = HI_SUCCESS;
    } else if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        tsio_mem_info mmu_buf = {0};

        mmu_buf.buf_phy_addr = se->buf_phy_addr;
        mmu_buf.buf_vir_addr = se->buf_ker_addr;
        mmu_buf.buf_size = se->buf_size;
        mmu_buf.handle = se->buf_handle;
        mmu_buf.tsio_buf_obj = se->buf_obj;
        mmu_buf.is_usr_map = (se->buf_usr_addr != HI_NULL) ? HI_TRUE : HI_FALSE;

        tsio_unmap_and_release_buf(&mmu_buf);

        se->buf_handle = -1;
        se->buf_obj = HI_NULL;
        se->buf_phy_addr = 0;
        se->buf_ker_addr = HI_NULL;
        se->buf_usr_addr = HI_NULL;

        ret = HI_SUCCESS;
    } else {
        HI_FATAL_TSIO("tsio_r_se's mod mismatch, tsio_r_se's mod is:%u\n", se->mod);
    }

    se->staled = HI_TRUE;

    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 tsio_r_se_pre_mmap_impl(struct tsio_r_se *se, hi_handle *buf_handle,
    hi_u32 *buf_size, hi_void **usr_addr)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        ret = HI_ERR_TSIO_NOT_SUPPORT;
    } else if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        *buf_handle = se->buf_handle;
        *buf_size = se->buf_size;
        *usr_addr = se->buf_usr_addr;
        ret = HI_SUCCESS;
    } else {
        HI_FATAL_TSIO("tsio_r_se's mod mismatch, tsio_r_se's mod is:%u\n", se->mod);
    }

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 tsio_r_se_pst_mmap_impl(struct tsio_r_se *se, hi_void *buf_usr_addr)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&se->lock);

    if (se_staled(se)) {
        ret = HI_ERR_TSIO_STALED;
        goto out;
    }

    if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        ret = HI_ERR_TSIO_NOT_SUPPORT;
    } else if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        if (se->buf_usr_addr != HI_NULL) {
            HI_FATAL_TSIO("se->buf_usr_addr is not null, se->buf_usr_addr = %p !\n", se->buf_usr_addr);
            ret = HI_ERR_TSIO_INVALID_PARA;
            goto out;
        }

        se->buf_usr_addr = buf_usr_addr;
        ret = HI_SUCCESS;
    } else {
        HI_FATAL_TSIO("tsio_r_se's mod mismatch, tsio_r_se's mod is:%u\n", se->mod);
    }

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 tsio_r_se_suspend_impl(struct tsio_r_se *se)
{
    hi_s32 ret = HI_FAILURE;

    TSIO_FATAL_ERROR(se->staled == HI_TRUE);

    osal_mutex_lock(&se->lock);

    if (se->stat == TSIO_SECURE_ENGINE_OPENED) {
        ret = _tsio_r_se_close_impl(se);
    } else {
        ret = HI_SUCCESS;
    }

    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_s32 tsio_r_se_resume_impl(struct tsio_r_se *se)
{
    hi_s32 ret = HI_FAILURE;

    TSIO_FATAL_ERROR(se->staled == HI_TRUE);

    osal_mutex_lock(&se->lock);

    if (se->stat == TSIO_SECURE_ENGINE_OPENED) {
        ret = _tsio_r_se_open_impl(se);
    } else {
        ret = HI_SUCCESS;
    }

    osal_mutex_unlock(&se->lock);

    return ret;
}

static hi_void tsio_r_se_handle_buf_ovfl_impl(struct tsio_r_se *se)
{
    osal_mutex_lock(&se->lock);

    TSIO_FATAL_ERROR(TSIO_SECURE_ENGINE_OUTPUT2RAM != se->mod);

    if (se->stat == TSIO_SECURE_ENGINE_OPENED) {
        HI_ERR_TSIO("obuf(%d) overflowed and reseted.\n", se->base.id);

        disable_obuf(se);

        enable_obuf(se);
    }

    osal_mutex_unlock(&se->lock);
}

static hi_s32 tsio_r_se_handle_flow_control_impl(struct tsio_r_se *se, struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret = HI_SUCCESS;
    struct list_head *node = HI_NULL;
    hi_u32 data_no_wrap_len = 0;
    hi_u32 data_wrap_len = 0;

    osal_mutex_lock(&se->lock);

    if ((se->stat == TSIO_SECURE_ENGINE_OPENED) && (se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM)) {
        list_for_each(node, &se->chn_head) {
            struct obj_node_helper *obj = list_entry(node, struct obj_node_helper, node);

            if (IS_PIDCHANNEL(obj->key)) {
                struct tsio_r_pid_channel *pid_channel = (struct tsio_r_pid_channel *)obj->key;

                if (IS_RAMPORT(pid_channel->port) && ram_port == (struct tsio_r_ram_port *)pid_channel->port) {
                    /* within obuf thresh , it can't reset flow control, otherwise overflow occurs. */
                    get_hw_avail_cont_data(se, &data_no_wrap_len, &data_wrap_len);
                    if (data_wrap_len > se->valid_buf_size - DEFAULT_SE_OBUF_THRESH) {
                        ret = HI_FAILURE;
                        goto out;
                    }
                }
            } else if (IS_RAWCHANNEL(obj->key)) {
                struct tsio_r_raw_channel *raw_channel = (struct tsio_r_raw_channel *)obj->key;

                TSIO_FATAL_ERROR(!IS_RAMPORT(raw_channel->port));

                if (ram_port == (struct tsio_r_ram_port *)raw_channel->port) {
                    /* within obuf thresh , it can't reset flow control, otherwise overflow occurs. */
                    get_hw_avail_cont_data(se, &data_no_wrap_len, &data_wrap_len);
                    if (data_wrap_len > se->valid_buf_size - DEFAULT_SE_OBUF_THRESH) {
                        ret = HI_FAILURE;
                        goto out;
                    }
                }
            } else if (IS_SPCHANNEL(obj->key)) {
                struct tsio_r_sp_channel *sp_channel = (struct tsio_r_sp_channel *)obj->key;

                TSIO_FATAL_ERROR(!IS_RAMPORT(sp_channel->port));

                if (ram_port == (struct tsio_r_ram_port *)sp_channel->port) {
                    /* within obuf thresh , it can't reset flow control, otherwise overflow occurs. */
                    get_hw_avail_cont_data(se, &data_no_wrap_len, &data_wrap_len);
                    if (data_wrap_len > se->valid_buf_size - DEFAULT_SE_OBUF_THRESH) {
                        ret = HI_FAILURE;
                        goto out;
                    }
                }
            } else if (IS_IVRCHANNEL(obj->key)) {
                /* ivrestart channel is control path, flow control is not necessary. */
            } else {
                HI_FATAL_TSIO("obj node helper's key is mismatch!\n");
            }
        }
    }

out:
    osal_mutex_unlock(&se->lock);

    return ret;
}

static struct tsio_r_se_ops g_tsio_se_ops = {
    .pre_open         = tsio_r_se_pre_open_impl,
    .open             = tsio_r_se_open_impl,
    .get_attrs        = tsio_r_se_get_attrs_impl,
    .set_attrs        = tsio_r_se_set_attrs_impl,
    .get_status       = tsio_r_se_get_status_impl,
    .add_channel      = tsio_r_se_add_channel_impl,
    .del_channel      = tsio_r_se_del_channel_impl,
    .del_all_channels = tsio_r_se_del_all_channels_impl,
    .acquire_buffer   = tsio_r_se_acquire_buffer_impl,
    .release_buffer   = tsio_r_se_release_buffer_impl,
    .close            = tsio_r_se_close_impl,
    .pst_close        = tsio_r_se_pst_close_impl,

    .pre_mmap         = tsio_r_se_pre_mmap_impl,
    .pst_mmap         = tsio_r_se_pst_mmap_impl,

    .suspend          = tsio_r_se_suspend_impl,
    .resume           = tsio_r_se_resume_impl,

    .handle_buf_ovfl     = tsio_r_se_handle_buf_ovfl_impl,
    .handle_flow_control = tsio_r_se_handle_flow_control_impl,
};

/*
 * TSIO CC functions.
 */
#define CC_UCHR2U32_UNIT 4
#define CC_U322UCHR_UNIT 4

static hi_void send_ccmd_out(const struct tsio_r_cc *ccsingleton, const hi_uchar *cmd, hi_u32 cmd_len)
{
    hi_u32 slot = 0;
    hi_u32 index;

    for (index = 0; index < cmd_len; index += CC_UCHR2U32_UNIT) {
        hi_u32 valid_len = cmd_len - index > CC_UCHR2U32_UNIT ? CC_UCHR2U32_UNIT : cmd_len - index;
        hi_u32 ccdata   = 0;

        if (memcpy_s(&ccdata, sizeof(ccdata), cmd + index, valid_len)) {
            HI_ERR_TSIO("memcpy_s failed\n");
            return;
        }

        tsio_hal_set_ccslot(ccsingleton->mgmt, slot++, ccdata);
    }

    tsio_hal_send_ccout(ccsingleton->mgmt, cmd_len);
}

#define TSIO_100_MSECONDS 100
#define TSIO_50_MSECONDS 50

/*
 * There is no effective way to confirm that a CC's receiving process is over when CC error flag occurred.
 * Had to detect the abnormal interrupt count in a short period of time.
 * 100ms is hw suggest value.
 */
static hi_s32 wait_ccerr_handle_end(const struct tsio_r_cc *ccsingleton)
{
    hi_s32 ret;

    /* check exception count. */
    while (ccsingleton->ccerr_int_cnt) {
        hi_u32 prev_ccerr_cnt = ccsingleton->ccerr_int_cnt;

        osal_msleep(TSIO_100_MSECONDS);

        if (prev_ccerr_cnt == ccsingleton->ccerr_int_cnt) {
            osal_msleep(TSIO_50_MSECONDS);

            /* double checking */
            if (prev_ccerr_cnt == ccsingleton->ccerr_int_cnt) {
                HI_ERR_TSIO("CC flag error(0x%x) occurred.\n", ccsingleton->ccerr_type);

                ret = HI_ERR_TSIO_CC_ERROR;
                goto out;
            }
        }
    }

    /* check conflict status.(first-mid-last-last) */
    if (tsio_hal_get_cconflict_status(ccsingleton->mgmt)) {
        HI_ERR_TSIO("CC conflict occurred.\n");

        ret = HI_ERR_TSIO_CC_ERROR;
        goto out;
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 recv_ccresp(const struct tsio_r_cc *ccsingleton, hi_uchar *resp, hi_u32 resp_buf_len, hi_u32 *resp_len)
{
    hi_s32 ret;
    hi_u32 slot = 0;
    hi_u32 resp_byte_len = 0;
    hi_u32 index;

    ret = wait_ccerr_handle_end(ccsingleton);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* try to receive valid cc resp */
    tsio_hal_get_ccresp_len(ccsingleton->mgmt, &resp_byte_len);
    if (!(resp_byte_len >= MIN_CC_CMD_LEN && resp_byte_len <= MAX_CC_CMD_LEN)) {
        HI_ERR_TSIO("invalid CC response.\n");
        ret = HI_FAILURE;
        goto out;
    }

    *resp_len = resp_byte_len > resp_buf_len ? resp_buf_len : resp_byte_len;

    for (index = 0; index < *resp_len; index += CC_U322UCHR_UNIT) {
        hi_u32 valid_len = *resp_len - index > CC_U322UCHR_UNIT ? CC_U322UCHR_UNIT : *resp_len - index;
        hi_u32 ccdata   = 0;

        tsio_hal_get_ccslot(ccsingleton->mgmt, slot++, &ccdata);

        ret = memcpy_s(resp + index, valid_len, (hi_uchar *)&ccdata, valid_len);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("memcpy_s failed, ret(%d)\n", ret);
            goto out;
        }
    }

    /* CC error flag maybe occurred after receive resp succeed ,such as (first-mid-last-last). */
    ret = wait_ccerr_handle_end(ccsingleton);

out:
    return ret;
}

static inline hi_void recv_ccresp_done(const struct tsio_r_cc *ccsingleton)
{
    tsio_hal_recv_ccdone(ccsingleton->mgmt);

    tsio_hal_clr_cconflict_status(ccsingleton->mgmt);
}

/*
 * if former cc send out and return too fast(TimeOut is too small).
 * current cc maybe send failed and recv former cc resp.
 */
static hi_s32 tsio_r_cc_send_impl(struct tsio_r_cc *ccsingleton, const struct tsio_r_cc_para *cc_para, hi_u32 time_out)
{
    hi_s32 ret = HI_FAILURE;
    const hi_uchar *cmd = cc_para->cmd;
    hi_u32 cmd_len = cc_para->cmd_len;
    hi_uchar *resp = cc_para->resp;
    hi_u32 *resp_len = cc_para->resp_len;

    osal_mutex_lock(&ccsingleton->lock);

    /* avoid the impact of the prev operation. */
    recv_ccresp_done(ccsingleton);

    ccsingleton->ccerr_int_cnt = 0;
    ccsingleton->ccerr_type    = 0;
    ccsingleton->wait_cond     = 0;

    send_ccmd_out(ccsingleton, cmd, cmd_len);

    if (time_out) {
        ret = wait_event_interruptible_timeout(ccsingleton->wait_queue,
            ccsingleton->wait_cond, msecs_to_jiffies(time_out));
        if (ret == 0) {
            ret = HI_ERR_TSIO_TIMEOUT;
            goto out;
        } else if (ret < 0) {
            goto out;
        }
    } else { /* wait forever */
        ret = wait_event_interruptible(ccsingleton->wait_queue, ccsingleton->wait_cond);
        if (ret < 0) {
            goto out;
        }
    }

    ret = recv_ccresp(ccsingleton, resp, *resp_len, resp_len);

out:
    recv_ccresp_done(ccsingleton);

    osal_mutex_unlock(&ccsingleton->lock);

    return ret;
}

static struct tsio_r_cc_ops g_tsio_cc_ops = {
    .send = tsio_r_cc_send_impl,
};

/*
 * TSIO management functions.
 */
static hi_s32 tsio_isr(int irq, struct tsio_mgmt *priv)
{
    struct tsio_mgmt *mgmt = priv;
    hi_u32 flag;
    hi_u32 status;
    hi_size_t lock_flag;

    flag = tsio_hal_get_int_flag(mgmt);
    if (TSIO_TX_CC_DONE_INT_MASK & flag) {
        tsio_hal_clr_tx_cc_done_int(mgmt);
    }

    if (TSIO_RX_CC_DONE_INT_MASK & flag) {
        struct tsio_r_cc *ccsingleton = mgmt->ccsingleton;

        tsio_hal_clr_rx_cc_done_int(mgmt);

        ccsingleton->wait_cond  = 1;
        wake_up_interruptible(&ccsingleton->wait_queue);
    }

    /* this is not an exception interrupt, just clear it. */
    if (TSIO_RX_CTS_INT_MASK & flag) {
        tsio_hal_clr_rx_cts_int(mgmt);
    }

    if (TSIO_RX_ROUTE_FIFO_OVFL_INT_MASK & flag) {
        tsio_hal_clr_rx_route_fifo_ovfl_int(mgmt);

        HI_ERR_TSIO("unhandled rx route fifo overflow error.\n");
    }

    if (TSIO_RX_PARSER_ERR_INT_MASK & flag) {
        hi_u32 rx_parser_err_int_flag = tsio_hal_get_rx_parser_err_int_flag(mgmt);

        tsio_hal_clr_rx_parser_err_int_flag(mgmt, rx_parser_err_int_flag);

        if (TSIO_RX_PHY_SP_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx phy sp error.\n");
        }

        if (TSIO_RX_FIFO_OVFL_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx fifo overflow error.\n");
        }

        if (TSIO_RX_SP_SYNC_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx sp sync error.\n");
        }

        if (TSIO_RX_SP_RFU0_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx sp rfu0 error.\n");
        }

        if (TSIO_RX_SP_DMA_END_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_FATAL_TSIO("rx error int flag do not contain tsio_rx_sp_dma_end_err_int_mask\n");
        }

        if (TSIO_RX_SP_ENCRY_EN_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx sp encry en error.\n");
        }

        if (TSIO_RX_SP_TSID_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_FATAL_TSIO("rx error int flag do not contain tsio_rx_sp_tsid_err_int_mask\n");
        }

        if (TSIO_RX_SP_SOC_DEFINE_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx sp soc define error.\n");
        }

        if (TSIO_RX_SP_RFU1_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx sp rfu1 error.\n");
        }

        if (TSIO_RX_SP_RFU2_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx sp rfu2 error.\n");
        }

        if (TSIO_RX_SP_STUFF_LOAD_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_ERR_TSIO("unhandled rx sp stuff load error.\n");
        }

        if (TSIO_RX_SP_SC_GEN_ERR_INT_MASK & rx_parser_err_int_flag) {
            HI_FATAL_TSIO("rx error int flag do not contain tsio_rx_sp_sc_gen_err_int_mask\n");
        }

        if (TSIO_RX_CC_ERR_INT_MASK & rx_parser_err_int_flag) {
            struct tsio_r_cc *ccsingleton = mgmt->ccsingleton;

            ccsingleton->ccerr_int_cnt++;
            ccsingleton->ccerr_type = TSIO_RX_CC_ERR_INT_MASK & rx_parser_err_int_flag;

            ccsingleton->wait_cond = 1;
            wake_up_interruptible(&ccsingleton->wait_queue);
        }
    }

    if (TSIO_TX_RR_ERR_INT_MASK & flag) {
        hi_u32 tx_err_int_flag = tsio_hal_get_tx_rr_err_int_flag(mgmt);

        tsio_hal_clr_tx_rr_err_int_flag(mgmt, tx_err_int_flag);

        if (TSIO_TX_LIVE0_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_LIVE1_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_LIVE2_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_LIVE3_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_LIVE4_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_LIVE5_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_LIVE6_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_LIVE7_BFIFO_OVFL_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx live0 bfifo overflow error.\n");
        }

        if (TSIO_TX_FIFO_EMPTY_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx fifo empty error.\n");
        }

        if (TSIO_CPU_WRITE_CC_CONFLICT_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled cpu write cc conflit error.\n");
        }

        if (TSIO_TX_CC_LENGTH_ERR_INT_MASK & tx_err_int_flag) {
            HI_ERR_TSIO("unhandled tx cc length error.\n");
        }
    }

    if (TSIO_PID_FILTER_ERR_INT_MASK & flag) {
        hi_u32 pid_filter_int_flag = tsio_hal_get_pid_filter_err_int_flag(mgmt);

        tsio_hal_clr_pid_filter_err_int_flag(mgmt, pid_filter_int_flag);

        if (TSIO_DVB0_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb0 fifo overflow error.\n");
        }

        if (TSIO_DVB1_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb1 fifo overflow error.\n");
        }

        if (TSIO_DVB2_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb2 fifo overflow error.\n");
        }

        if (TSIO_DVB3_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb3 fifo overflow error.\n");
        }

        if (TSIO_DVB4_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb4 fifo overflow error.\n");
        }

        if (TSIO_DVB5_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb5 fifo overflow error.\n");
        }

        if (TSIO_DVB6_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb6 fifo overflow error.\n");
        }

        if (TSIO_DVB7_FIFO_OVFL_INT_MASK & pid_filter_int_flag) {
            HI_ERR_TSIO("unhandled dvb7 fifo overflow error.\n");
        }
    }

    if (TSIO_DMA_INT_MASK & flag) {
        hi_u32 dma_int_flag = tsio_hal_get_dma_int_flag(mgmt);
        if (TSIO_DMA_CHNL_PEND_INT_MASK & dma_int_flag) {
            struct list_head *node = HI_NULL;

            /* avoid cpu dead loop responding irq. */
            tsio_hal_mask_all_dma_int(mgmt);

            /* trigger this irq after coal time cycle. */
            tsio_hal_un_mask_all_dma_int(mgmt);

            status = tsio_hal_get_dma_chn_pend_status(mgmt);

            osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);

            list_for_each(node, &mgmt->ram_port_head) {
                struct tsio_r_ram_port *ram_port = list_entry(node, struct tsio_r_ram_port, node);

                if ((1 << TO_RAMPORT_HW_ID(ram_port->base.id)) & status) {
                    schedule_work(&ram_port->flow_control_worker);
                }
            }

            osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);
        }

        if (TSIO_DMA_PACK_INT_MASK & dma_int_flag) {
            struct list_head *node = HI_NULL;
            hi_u32 status2;

            /* 0 ~ 31 */
            status = tsio_hal_get_dma_pack_int_status_l(mgmt);

            tsio_hal_clr_dma_pack_int_status_l(mgmt, status);

            /* 32 ~ 63 */
            status2 = tsio_hal_get_dma_pack_int_status_h(mgmt);

            tsio_hal_clr_dma_pack_int_status_h(mgmt, status2);

            osal_spin_lock_irqsave(&mgmt->se_list_lock2, &lock_flag);

            list_for_each(node, &mgmt->se_head) {
                struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

                if (TO_SE_HW_ID(se->base.id) < SE_LOW_NUM) {
                    if ((1 << TO_SE_HW_ID(se->base.id)) & status) {
                        se->wait_cond = 1;
                        wake_up_interruptible(&se->wait_queue);
                    }
                } else {
                    if ((1 << (TO_SE_HW_ID(se->base.id) - SE_LOW_NUM)) & status2) {
                        se->wait_cond = 1;
                        wake_up_interruptible(&se->wait_queue);
                    }
                }
            }

            osal_spin_unlock_irqrestore(&mgmt->se_list_lock2, &lock_flag);
        }

        if (TSIO_DMA_OBUF_NR_FULL_INT_MASK & dma_int_flag) {
            /* avoid cpu dead loop responding irq. */
            tsio_hal_mask_all_dma_int(mgmt);

            /* trigger this irq after coal time cycle. */
            tsio_hal_un_mask_all_dma_int(mgmt);
        }

        if (TSIO_DMA_END_INT_MASK & dma_int_flag) {
            struct list_head *node = HI_NULL;
            hi_u32 status2;

            /* 0 ~ 31 */
            status = tsio_hal_get_dma_end_int_status_l(mgmt);

            tsio_hal_clr_dma_end_int_status_l(mgmt, status);

            /* 32 ~ 63 */
            status2 = tsio_hal_get_dma_end_int_status_h(mgmt);

            tsio_hal_clr_dma_end_int_status_h(mgmt, status2);

            osal_spin_lock_irqsave(&mgmt->se_list_lock2, &lock_flag);

            list_for_each(node, &mgmt->se_head) {
                struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

                if (TO_SE_HW_ID(se->base.id) < SE_LOW_NUM) {
                    if ((1 << TO_SE_HW_ID(se->base.id)) & status) {
                        se->dma_end_int_cnt++;
                    }
                } else {
                    if ((1 << (TO_SE_HW_ID(se->base.id) - SE_LOW_NUM)) & status2) {
                        se->dma_end_int_cnt++;
                    }
                }
            }

            osal_spin_unlock_irqrestore(&mgmt->se_list_lock2, &lock_flag);
        }

        if (TSIO_DMA_FLUSH_INT_MASK & dma_int_flag) {
            struct list_head *node = HI_NULL;
            hi_u32 status2;

            /* 0 ~ 31 */
            status = tsio_hal_get_dma_flush_status_l(mgmt);

            tsio_hal_clr_dma_flush_status_l(mgmt, status);

            /* 32 ~ 63 */
            status2 = tsio_hal_get_dma_flush_status_h(mgmt);

            tsio_hal_clr_dma_flush_status_h(mgmt, status2);

            osal_spin_lock_irqsave(&mgmt->se_list_lock2, &lock_flag);

            list_for_each(node, &mgmt->se_head) {
                struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

                if (TO_SE_HW_ID(se->base.id) < SE_LOW_NUM) {
                    if ((1 << TO_SE_HW_ID(se->base.id)) & status) {
                        se->wait_flush_cond = 1;
                        wake_up_interruptible(&se->wait_flush_queue);
                    }
                } else {
                    if ((1 << (TO_SE_HW_ID(se->base.id) - SE_LOW_NUM)) & status2) {
                        se->wait_flush_cond = 1;
                        wake_up_interruptible(&se->wait_flush_queue);
                    }
                }
            }

            osal_spin_unlock_irqrestore(&mgmt->se_list_lock2, &lock_flag);
        }

        if (TSIO_DMA_DES_END_INT_MASK & dma_int_flag) {
            struct list_head *node = HI_NULL;

            status = tsio_hal_get_dma_des_end_status(mgmt);

            /* clear int status as soon as possible */
            tsio_hal_clr_dma_des_end_status(mgmt, status);

            osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);

            list_for_each(node, &mgmt->ram_port_head) {
                struct tsio_r_ram_port *ram_port = list_entry(node, struct tsio_r_ram_port, node);

                if ((1 << TO_RAMPORT_HW_ID(ram_port->base.id)) & status) {
                    schedule_work(&ram_port->dsc_end_worker);
                }
            }

            osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);
        }

        if (TSIO_DMA_BID_INT_MASK & dma_int_flag) {
            HI_ERR_TSIO("unhandled dma bid error.\n");
        }

        if (TSIO_DMA_LIVE_OFLW_ERR_INT_MASK & dma_int_flag) {
            HI_ERR_TSIO("unhandled live overflow error.\n");
        }

        if (TSIO_DMA_CHK_CODE_ERR_INT_MASK & dma_int_flag) {
            HI_ERR_TSIO("unhandled dma check code error.\n");
        }

        if (TSIO_DMA_OBUF_OFLW_ERR_INT_MASK & dma_int_flag) {
            struct list_head *node = HI_NULL;
            hi_u32 status2;

            /* 0 ~ 31 */
            status = tsio_hal_get_dma_obuf_ovflw_status_l(mgmt);

            tsio_hal_clr_dma_obuf_ovflw_status_l(mgmt, status);

            /* 32 ~ 63 */
            status2 = tsio_hal_get_dma_obuf_ovflw_status_h(mgmt);

            tsio_hal_clr_dma_obuf_ovflw_status_h(mgmt, status2);

            osal_spin_lock_irqsave(&mgmt->se_list_lock2, &lock_flag);

            list_for_each(node, &mgmt->se_head) {
                struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

                if (TO_SE_HW_ID(se->base.id) < SE_LOW_NUM) {
                    if ((1 << TO_SE_HW_ID(se->base.id)) & status) {
                        se->overflow_cnt++;

                        schedule_work(&se->buf_ovfl_worker);
                    }
                } else {
                    if ((1 << (TO_SE_HW_ID(se->base.id) - SE_LOW_NUM)) & status2) {
                        se->overflow_cnt++;

                        schedule_work(&se->buf_ovfl_worker);
                    }
                }
            }

            osal_spin_unlock_irqrestore(&mgmt->se_list_lock2, &lock_flag);
        }

        if (TSIO_DMA_DES_TYPE_ERR_INT_MASK & dma_int_flag) {
            /* desc memory illegally write. */
            HI_FATAL_TSIO("rx error int flag do not contain tsio_dma_des_type_err_int_mask\n");
        }

        if (TSIO_DMA_ICHL_WPTR_OFLW_ERR_INT_MASK & dma_int_flag) {
            HI_ERR_TSIO("unhandled dma ichl wptr overflow error.\n");
        }

        if (TSIO_DMA_OVERFLOW_ERR_INT_MASK & dma_int_flag) {
            HI_FATAL_TSIO("unhandled dma overflow error.\n");

            HI_FATAL_TSIO("rx error int flag do not contain tsio_dma_overflow_err_int_mask\n");
        }

        if (TSIO_DMA_TLB_ERR_INT_MASK & dma_int_flag) {
            HI_FATAL_TSIO("unhandled tlb error.\n");

            HI_FATAL_TSIO("rx error int flag do not contain tsio_dma_tlb_err_int_mask\n");
        }

        if (TSIO_DMA_BUS_ERR_INT_MASK & dma_int_flag) {
            HI_FATAL_TSIO("unhandled bus error.\n");

            HI_FATAL_TSIO("rx error int flag do not contain tsio_dma_bus_err_int_mask\n");
        }
    }

    return IRQ_HANDLED;
}

/*
 * some frequently irq triggered maybe cause system hang.
 * so here mask these irqs and setup timer monitor.
 */
static hi_void rx_parser_irq_monitor(struct tsio_mgmt *mgmt)
{
    hi_u32 rx_parser_err_int_flag = tsio_hal_get_org_rx_parser_err_int_flag(mgmt);
    if (TSIO_RX_SP_TSID_ERR_INT_MASK & rx_parser_err_int_flag) {
        HI_ERR_TSIO("rx sp tsid error occurred.\n");

        tsio_hal_clr_rx_parser_err_int_flag(mgmt, TSIO_RX_SP_TSID_ERR_INT_MASK);
    }

    if (TSIO_RX_SP_SC_GEN_ERR_INT_MASK & rx_parser_err_int_flag) {
        HI_ERR_TSIO("rx sp sc gen error occurred.\n");

        tsio_hal_clr_rx_parser_err_int_flag(mgmt, TSIO_RX_SP_SC_GEN_ERR_INT_MASK);
    }
}

static hi_s32 tsio_monitor(hi_void *ptr)
{
    struct tsio_mgmt *mgmt = (struct tsio_mgmt *)ptr;

    while (!osal_kthread_should_stop()) {
        rx_parser_irq_monitor(mgmt);

        osal_msleep(1000); /* dealy 1000ms */
    }

    return HI_SUCCESS;
}

#ifdef TSIO_PROC_SUPPORT
static hi_s32 tsio_port_proc_read(hi_void *p, hi_void *v)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct list_head *node = HI_NULL;
    hi_s32 ret;

    osal_proc_print(p, "  id   all_pkt_cnt   src   type  buf_size"
        "    buf_used       get(try/ok)    push     put     sort\n");

    /* tsi port */
    osal_mutex_lock(&mgmt->tsi_port_list_lock);

    list_for_each(node, &mgmt->tsi_port_head) {
        struct tsio_r_tsi_port *tsi_port = list_entry(node, struct tsio_r_tsi_port, node);
        tsio_tsi_port_status status = {0};

        ret = tsio_r_tsi_port_get_status_impl(tsi_port, &status);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("tsi get port status faild, ret:%d\n", ret);
        }
        osal_proc_print(p, " 0x%-4x 0x%-8x 0x%-4x %-4s %-10s %-10s %8s/%-8s %-8s %-8s t\n",
            status.port_id, status.pkt_num, tsi_port->source_port, "ts", "--", "--", "--", "--", "--", "--");
    }

    osal_mutex_unlock(&mgmt->tsi_port_list_lock);

    /*
     * if ram port is in the state of flow control. TSIO_R_RAMPort_GetStatus_Impl take too much time for RamPort->Lock.
     * we discard acquiring ramport->lock, maybe lead to inaccure proc info.
     */
    /* ram port */
    osal_mutex_lock(&mgmt->ram_port_list_lock);

    list_for_each(node, &mgmt->ram_port_head) {
        struct tsio_r_ram_port *ram_port = list_entry(node, struct tsio_r_ram_port, node);
        tsio_ram_port_status status = {0};

        ret = _tsio_r_ram_port_get_status_impl(ram_port, &status);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("ram get port status faild, ret:%d\n", ret);
        }
        osal_proc_print(p, " 0x%-4x 0x%-8x %-6s %-4s %-8u %8u%% %8u/%-8u %-8u %-8u r\n",
            status.port_id, status.pkt_num, "--", ram_port->source_type == TSIO_SOURCE_TS ? "ts" : "bulk",
            status.buffer_size, (status.buffer_used * 100) / status.buffer_size, ram_port->get_count, /* 100 times */
            ram_port->get_valid_count, ram_port->push_count, ram_port->put_count);
    }

    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_chan_proc_read(hi_void *p, hi_void *v)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct list_head *node = HI_NULL;

    osal_proc_print(p, "  id   pid   port_id  se_id  sort\n");

    /* pid channel */
    osal_mutex_lock(&mgmt->pid_channel_list_lock);

    list_for_each(node, &mgmt->pid_channel_head) {
        struct tsio_r_pid_channel *pid_channel = list_entry(node, struct tsio_r_pid_channel, node);
        struct tsio_r_base *port = HI_NULL;
        struct tsio_r_se *se = HI_NULL;

        osal_mutex_lock(&pid_channel->lock);

        port = pid_channel->port;
        se   = pid_channel->se;

        if (IS_RAMPORT(port)) {
            if (se != HI_NULL) {
                osal_proc_print(p, " 0x%-3x 0x%-4x 0x%-4x 0x%-4x p\n", pid_channel->base.id,
                    pid_channel->pid, port->id + TSIO_RAM_PORT_0, se->base.id + TSIO_SID_0);
            } else {
                osal_proc_print(p, " 0x%-3x 0x%-4x 0x%-4x %-6s p\n", pid_channel->base.id,
                    pid_channel->pid, port->id + TSIO_RAM_PORT_0, "--");
            }
        } else {
            if (se != HI_NULL) {
                osal_proc_print(p, " 0x%-3x 0x%-4x 0x%-4x 0x%-4x p\n", pid_channel->base.id,
                    pid_channel->pid, port->id + TSIO_TSI_PORT_0, se->base.id + TSIO_SID_0);
            } else {
                osal_proc_print(p, " 0x%-3x 0x%-4x 0x%-4x %-6s p\n", pid_channel->base.id,
                    pid_channel->pid, port->id + TSIO_TSI_PORT_0, "--");
            }
        }

        osal_mutex_unlock(&pid_channel->lock);
    }

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    /* raw channel */
    osal_mutex_lock(&mgmt->raw_channel_list_lock);

    list_for_each(node, &mgmt->raw_channel_head) {
        struct tsio_r_raw_channel *raw_channel = list_entry(node, struct tsio_r_raw_channel, node);
        struct tsio_r_base *port = HI_NULL;
        struct tsio_r_se *se = HI_NULL;

        osal_mutex_lock(&raw_channel->lock);

        port = raw_channel->port;
        se   = raw_channel->se;

        if (se != HI_NULL) {
            osal_proc_print(p, " 0x%-3x %-6s 0x%-4x 0x%-4x r\n", raw_channel->base.id, "--",
                port->id + TSIO_RAM_PORT_0, se->base.id + TSIO_SID_0);
        } else {
            osal_proc_print(p, " 0x%-3x %-6s 0x%-4x %-6s r\n",
                raw_channel->base.id, "--", port->id + TSIO_RAM_PORT_0, "--");
        }

        osal_mutex_unlock(&raw_channel->lock);
    }

    osal_mutex_unlock(&mgmt->raw_channel_list_lock);

    /* sp channel */
    osal_mutex_lock(&mgmt->sp_channel_list_lock);

    list_for_each(node, &mgmt->sp_channel_head) {
        struct tsio_r_sp_channel *sp_channel = list_entry(node, struct tsio_r_sp_channel, node);
        struct tsio_r_base *port = HI_NULL;
        struct tsio_r_se *se = HI_NULL;

        osal_mutex_lock(&sp_channel->lock);

        port = sp_channel->port;
        se   = sp_channel->se;

        if (se != HI_NULL) {
            osal_proc_print(p, " 0x%-3x %-6s 0x%-4x 0x%-4x s\n", sp_channel->base.id,
                "--", port->id + TSIO_RAM_PORT_0, se->base.id + TSIO_SID_0);
        } else {
            osal_proc_print(p, " 0x%-3x %-6s 0x%-4x %-6s s\n", sp_channel->base.id,
                "--", port->id + TSIO_RAM_PORT_0, "--");
        }

        osal_mutex_unlock(&sp_channel->lock);
    }

    osal_mutex_unlock(&mgmt->sp_channel_list_lock);

    /* ivr channel */
    osal_mutex_lock(&mgmt->ivr_channel_list_lock);

    list_for_each(node, &mgmt->ivr_channel_head) {
        struct tsio_r_ivr_channel *ivr_channel = list_entry(node, struct tsio_r_ivr_channel, node);
        struct tsio_r_base *port = HI_NULL;
        struct tsio_r_se *se = HI_NULL;

        osal_mutex_lock(&ivr_channel->lock);

        port = ivr_channel->port;
        se   = ivr_channel->se;

        if (se != HI_NULL) {
            osal_proc_print(p, " 0x%-3x %-6s 0x%-4x 0x%-4x i\n", ivr_channel->base.id,
                "--", port->id + TSIO_RAM_PORT_0, se->base.id + TSIO_SID_0);
        } else {
            osal_proc_print(p, " 0x%-3x %-6s 0x%-4x %-6s i\n", ivr_channel->base.id,
                "--", port->id + TSIO_RAM_PORT_0, "--");
        }

        osal_mutex_unlock(&ivr_channel->lock);
    }

    osal_mutex_unlock(&mgmt->ivr_channel_list_lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_se_proc_read(hi_void *p, hi_void *v)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct list_head *node = HI_NULL;
    hi_s32 ret;

    osal_proc_print(p, "  id  status  dst  buf_size    buf_used       acq(try/ok)    rel    overflow\n");

    osal_mutex_lock(&mgmt->se_list_lock);

    list_for_each(node, &mgmt->se_head) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);
        tsio_secure_engine_status status;

        ret = tsio_r_se_get_status_impl(se, &status);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("se get status failed, ret:%d\n", ret);
        }

        osal_mutex_lock(&se->lock);

        if (se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
            osal_proc_print(p, " 0x%-3x %-6s 0x%-2x %-10s %-10s %8s/%-8s %-8s  %-8s\n",
                se->base.id + TSIO_SID_0, se->stat == TSIO_SECURE_ENGINE_OPENED ? "open" : "close",
                se->dest_port, "--", "--", "--", "--", "--", "--");
        } else {
            osal_proc_print(p, " 0x%-3x %-6s %-4s 0x%-8x  %8u%% %8u/%-8u %-8u  %-8u\n",
                se->base.id + TSIO_SID_0, se->stat == TSIO_SECURE_ENGINE_OPENED ? "open" : "close",
                "--", status.buffer_size, (status.buffer_used * 100) / status.buffer_size, /* 100 times */
                se->acq_count, se->acq_valid_count, se->rel_count, status.overflow_cnt);
        }

        osal_mutex_unlock(&se->lock);
    }

    osal_mutex_unlock(&mgmt->se_list_lock);

    return HI_SUCCESS;
}

static hi_void add_debug_proc(struct tsio_mgmt *mgmt)
{
    osal_proc_entry *proc_item = HI_NULL;

    proc_item = osal_proc_add("tsio_port", strlen("tsio_port"));
    if (proc_item == HI_NULL) {
        HI_ERR_TSIO("add tsio port proc failed.\n");
    } else {
        proc_item->read = tsio_port_proc_read;
    }

    proc_item = osal_proc_add("tsio_chan", strlen("tsio_chan"));
    if (proc_item == HI_NULL) {
        HI_ERR_TSIO("add tsio chan proc failed.\n");
    } else {
        proc_item->read = tsio_chan_proc_read;
    }

    proc_item = osal_proc_add("tsio_se", strlen("tsio_se"));
    if (proc_item == HI_NULL) {
        HI_ERR_TSIO("add tsio se proc failed.\n");
    } else {
        proc_item->read = tsio_se_proc_read;
    }
}

static inline hi_void del_debug_proc(struct tsio_mgmt *mgmt)
{
    osal_proc_remove("tsio_se", strlen("tsio_se"));
    osal_proc_remove("tsio_chan", strlen("tsio_chan"));
    osal_proc_remove("tsio_port", strlen("tsio_port"));
}
#else
static inline hi_void add_debug_proc(struct tsio_mgmt *mgmt) {}
static inline hi_void del_debug_proc(struct tsio_mgmt *mgmt) {}
#endif

static hi_s32 tsio_dmx_fq_bp_tsio_port(hi_u32 dmx_id, hi_u32 fq_id, hi_u32 dmx_tsio_port_id,
    hi_s32(*op)(hi_u32 dmx_id, hi_u32 dmx_tsio_port_id, hi_u32 fq_id, hi_u32 tsio_ram_port_id))
{
    hi_s32 ret = HI_FAILURE;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();
    struct list_head *se_node = HI_NULL;

    TSIO_FATAL_ERROR(dmx_tsio_port_id >= mgmt->tsi_port_cnt);

    osal_mutex_lock(&mgmt->se_list_lock);

    list_for_each(se_node, &mgmt->se_head) {
        struct tsio_r_se *se = list_entry(se_node, struct tsio_r_se, node);

        osal_mutex_lock(&se->lock);

        if ((se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) &&
            (dmx_tsio_port_id == se->dest_port - DMX_TSIO_PORT_0)) {
            if (!list_empty(&se->chn_head)) {
                struct obj_node_helper *obj = list_first_entry(&se->chn_head, struct obj_node_helper, node);

                if (IS_PIDCHANNEL(obj->key)) {
                    struct tsio_r_pid_channel *pid_channel = (struct tsio_r_pid_channel *)obj->key;

                    if (IS_RAMPORT(pid_channel->port)) {
                        ret = op(dmx_id, dmx_tsio_port_id, fq_id, TO_RAMPORT_HW_ID(pid_channel->port->id));
                    } else if (IS_TSIPORT(pid_channel->port)) { /* skip tsi port. */
                        ret = HI_SUCCESS;
                    } else {
                        HI_FATAL_TSIO("pid_channel's port is unknown!\n");
                    }
                }
            }
        }

        osal_mutex_unlock(&se->lock);
    }

    osal_mutex_unlock(&mgmt->se_list_lock);

    return ret;
}

static tsio_export_func_s g_tsio_export_funcs = {
    .dmx_fq_bp_tsio_port = tsio_dmx_fq_bp_tsio_port,
};

static hi_s32 __tsio_mgmt_init_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret;

    mgmt->band_width   = TSIO_BW_BUTT;
    mgmt->stuff_sid    = TSIO_SID_BUTT;
    mgmt->sync_thres   = TSIO_SYNC_THRES_BUTT;

    /* TSI port */
    mgmt->tsi_port_cnt = TSIO_TSIPORT_CNT;
    osal_mutex_init(&mgmt->tsi_port_list_lock);
    INIT_LIST_HEAD(&mgmt->tsi_port_head);
    bitmap_zero(mgmt->tsi_port_bitmap, mgmt->tsi_port_cnt);

    /* RAM port */
    mgmt->ram_port_cnt = TSIO_RAMPORT_CNT;
    osal_mutex_init(&mgmt->ram_port_list_lock);
    osal_spin_lock_init(&mgmt->ram_port_list_lock2);
    INIT_LIST_HEAD(&mgmt->ram_port_head);
    bitmap_zero(mgmt->ram_port_bitmap, mgmt->ram_port_cnt);

    /* PID channel */
    mgmt->pid_channel_cnt = TSIO_PID_CHANNEL_CNT;
    osal_mutex_init(&mgmt->pid_channel_list_lock);
    INIT_LIST_HEAD(&mgmt->pid_channel_head);
    bitmap_zero(mgmt->pid_channel_bitmap, mgmt->pid_channel_cnt);

    /* RAW channel */
    mgmt->raw_channel_cnt = TSIO_RAW_CHANNEL_CNT;
    osal_mutex_init(&mgmt->raw_channel_list_lock);
    INIT_LIST_HEAD(&mgmt->raw_channel_head);
    bitmap_zero(mgmt->raw_channel_bitmap, mgmt->raw_channel_cnt);

    /* SP channel */
    mgmt->sp_channel_cnt = TSIO_SP_CHANNEL_CNT;
    osal_mutex_init(&mgmt->sp_channel_list_lock);
    INIT_LIST_HEAD(&mgmt->sp_channel_head);
    bitmap_zero(mgmt->sp_channel_bitmap, mgmt->sp_channel_cnt);

    /* ivrestart channel */
    mgmt->ivr_channel_cnt = TSIO_IVR_CHANNEL_CNT;
    osal_mutex_init(&mgmt->ivr_channel_list_lock);
    INIT_LIST_HEAD(&mgmt->ivr_channel_head);
    bitmap_zero(mgmt->ivr_channel_bitmap, mgmt->ivr_channel_cnt);

    /* SE */
    mgmt->se_cnt = TSIO_SE_CNT;
    osal_mutex_init(&mgmt->se_list_lock);
    osal_spin_lock_init(&mgmt->se_list_lock2);
    INIT_LIST_HEAD(&mgmt->se_head);
    bitmap_zero(mgmt->se_bitmap, mgmt->se_cnt);

    /* CC HB singleton */
    mgmt->ccsingleton = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_cc), GFP_KERNEL);
    if (mgmt->ccsingleton == HI_NULL) {
        HI_ERR_TSIO("init CC singleton failed.\n");
        ret = HI_FAILURE;
        goto out0;
    }
    osal_mutex_init(&mgmt->ccsingleton->lock);
    mgmt->ccsingleton->ops = &g_tsio_cc_ops;
    mgmt->ccsingleton->mgmt = mgmt;
    init_waitqueue_head(&mgmt->ccsingleton->wait_queue);

    ret = osal_exportfunc_register(HI_ID_TSIO, "HI_TSIO", (hi_void *)&g_tsio_export_funcs);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("register TSIO module failed\n");
        goto out1;
    }

    add_debug_proc(mgmt);

    return HI_SUCCESS;

out1:
    HI_KFREE(HI_ID_TSIO, mgmt->ccsingleton);
    mgmt->ccsingleton = HI_NULL;
out0:
    return ret;
}

static inline hi_void __tsio_mgmt_exit_impl(struct tsio_mgmt *mgmt)
{
    del_debug_proc(mgmt);

    osal_exportfunc_unregister(HI_ID_TSIO);

    osal_mutex_destory(&mgmt->ccsingleton->lock);

    /* CC HB singleton */
    HI_KFREE(HI_ID_TSIO, mgmt->ccsingleton);
    mgmt->ccsingleton = HI_NULL;
}

static hi_s32 _tsio_mgmt_init_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret;
    hi_ulong cb_ttbr = 0;
    hi_ulong err_rd_addr = 0;
    hi_ulong err_wr_addr = 0;

    TSIO_FATAL_ERROR(0 != osal_atomic_read(&mgmt->ref_count));
    TSIO_FATAL_ERROR(mgmt->state != TSIO_MGMT_CLOSED);

    ret = __tsio_mgmt_init_impl(mgmt);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    mgmt->io_base = osal_ioremap_nocache(TSIO_REGS_BASE, TSIO_REGS_SIZE);
    if (mgmt->io_base == HI_NULL) {
        HI_FATAL_TSIO("osal_ioremap_nocache error!\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out1;
    }

    ret = tsio_hal_init_ctrl(mgmt);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    hi_drv_nssmmu_get_page_table_addr(&cb_ttbr, &err_rd_addr, &err_wr_addr);
    mgmt->cb_ttbr = (hi_u32)cb_ttbr;

    /* hw restrict cb_ttbr must be 16bytes align. */
    TSIO_FATAL_ERROR(cb_ttbr % PHY_ADDR_ALIGN_BYTES);

    tsio_hal_en_mmu(mgmt);

    if (hi_drv_osal_request_irq(TSIO_IRQ_NUM, (irq_handler_t)tsio_isr, IRQF_SHARED, "int_tsio", (hi_void *)mgmt) != 0) {
        HI_ERR_TSIO("osal_request irq(%d) failed.\n", TSIO_IRQ_NUM);
        ret = HI_FAILURE;
        goto out3;
    }

    if (hi_drv_sys_set_irq_affinity(HI_ID_TSIO, TSIO_IRQ_NUM, "int_tsio") != HI_SUCCESS) {
        HI_WARN_TSIO("set tsio irq affinity failed.\n");
    }

    tsio_hal_en_all_int(mgmt);

    tsio_hal_set_dma_cnt_unit(mgmt);

    /* exception monitor */
    mgmt->monitor = osal_kthread_create(tsio_monitor, mgmt, "tsio_monitor", 0);
    if (mgmt->monitor == HI_NULL) {
        HI_ERR_TSIO("create kthread failed!\n");
        goto out3;
    }

    mgmt->state = TSIO_MGMT_OPENED;

    return HI_SUCCESS;

out3:
    tsio_hal_de_init_ctrl(mgmt);
out2:
    osal_iounmap(mgmt->io_base);
out1:
    __tsio_mgmt_exit_impl(mgmt);
out0:
    return ret;
}

static hi_s32 tsio_mgmt_init_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state == TSIO_MGMT_CLOSED) {
        ret = _tsio_mgmt_init_impl(mgmt);
        if (ret == HI_SUCCESS) {
            if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
                HI_DBG_TSIO("inc ref_count err.!\n");
            }
        }
    } else if (mgmt->state == TSIO_MGMT_OPENED) {
        TSIO_FATAL_ERROR(osal_atomic_read(&mgmt->ref_count) == 0);

        if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
            HI_DBG_TSIO("inc ref_count err.\n");
        }

        ret = HI_SUCCESS;
    } else {
        HI_FATAL_TSIO("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
    }

    osal_mutex_unlock(&mgmt->lock);

    return ret;
}

hi_void __tsio_mgmt_mutex_destory_impl(struct tsio_mgmt *mgmt)
{
    osal_mutex_destory(&mgmt->tsi_port_list_lock);
    osal_mutex_destory(&mgmt->ram_port_list_lock);
    osal_mutex_destory(&mgmt->pid_channel_list_lock);
    osal_mutex_destory(&mgmt->raw_channel_list_lock);
    osal_mutex_destory(&mgmt->sp_channel_list_lock);
    osal_mutex_destory(&mgmt->ivr_channel_list_lock);
    osal_mutex_destory(&mgmt->se_list_lock);
    osal_spin_lock_destory(&mgmt->ram_port_list_lock2);
    osal_spin_lock_destory(&mgmt->se_list_lock2);
}

static hi_s32 _tsio_mgmt_exit_impl(struct tsio_mgmt *mgmt)
{
    TSIO_FATAL_ERROR(osal_atomic_read(&mgmt->ref_count) != 1);
    TSIO_FATAL_ERROR(mgmt->state != TSIO_MGMT_OPENED);

    mgmt->state = TSIO_MGMT_CLOSED;

    /* mask phy & ctrl int. */
    if (mgmt->sync_thres != TSIO_SYNC_THRES_BUTT) {
        tsio_hal_dis_phy_int(mgmt);
    }
    tsio_hal_dis_all_int(mgmt);

    /* phy */
    if (mgmt->sync_thres != TSIO_SYNC_THRES_BUTT) {
        tsio_hal_de_init_phy(mgmt);
    }

    /* ctrl */
    osal_kthread_destroy(mgmt->monitor, HI_TRUE);

    hi_drv_osal_free_irq(TSIO_IRQ_NUM, "tsio", (hi_void *)mgmt);

    tsio_hal_de_init_ctrl(mgmt);

    osal_iounmap(mgmt->io_base);

    __tsio_mgmt_exit_impl(mgmt);

    __tsio_mgmt_mutex_destory_impl(mgmt);

    return HI_SUCCESS;
}

static inline hi_s32 tsio_mgmt_rel_and_exit(struct tsio_mgmt *mgmt)
{
    if (list_empty(&mgmt->tsi_port_head) && list_empty(&mgmt->ram_port_head) &&
        list_empty(&mgmt->pid_channel_head) && list_empty(&mgmt->raw_channel_head) &&
        list_empty(&mgmt->sp_channel_head) && list_empty(&mgmt->ivr_channel_head) &&
        list_empty(&mgmt->se_head)) {
        return _tsio_mgmt_exit_impl(mgmt);
    }

    return HI_FAILURE;
}

static hi_s32 tsio_mgmt_exit_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long start = jiffies;
    unsigned long end = start + HZ; /* 1s */

    do {
        osal_mutex_lock(&mgmt->lock);

        if (mgmt->state == TSIO_MGMT_OPENED) {
            TSIO_FATAL_ERROR(osal_atomic_read(&mgmt->ref_count) == 0);

            if (osal_atomic_read(&mgmt->ref_count) == 1) {
                ret = tsio_mgmt_rel_and_exit(mgmt);
                if (ret == HI_SUCCESS) {
                    if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
                        HI_DBG_TSIO("ref_count is not 0.\n");
                    }
                }
            } else {
                if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
                    HI_DBG_TSIO("ref_count is not 0.\n");
                }
                ret = HI_SUCCESS;
            }
        } else if (mgmt->state == TSIO_MGMT_CLOSED) {
            TSIO_FATAL_ERROR(osal_atomic_read(&mgmt->ref_count));

            ret = HI_SUCCESS;
        } else {
            HI_FATAL_TSIO("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
        }

        osal_mutex_unlock(&mgmt->lock);

        osal_msleep(TEN_MSECS);
    } while (ret != HI_SUCCESS && time_in_range(jiffies, start, end));

    return ret;
}

static hi_s32 tsio_mgmt_get_config_impl(struct tsio_mgmt *mgmt, tsio_config *config)
{
    osal_mutex_lock(&mgmt->lock);

    config->band_width   = mgmt->band_width;
    config->stuff_sid    = mgmt->stuff_sid;
    config->sync_thres   = mgmt->sync_thres;

    osal_mutex_unlock(&mgmt->lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_mgmt_set_config_impl(struct tsio_mgmt *mgmt, const tsio_config *config)
{
    hi_u32 id;

    osal_mutex_lock(&mgmt->lock);

    mgmt->band_width = config->band_width;

    mgmt->stuff_sid = config->stuff_sid;
    id = config->stuff_sid - TSIO_SID_0;
    set_bit(id, mgmt->se_bitmap);
    tsio_hal_en_stuff_srv(mgmt, id);

    /* initialize PHY depend mgmt->band_width. */
    mgmt->sync_thres = config->sync_thres;
    tsio_hal_init_phy(mgmt);

    osal_mutex_unlock(&mgmt->lock);

    return HI_SUCCESS;
}

static hi_s32 tsio_mgmt_get_cap_impl(struct tsio_mgmt *mgmt, tsio_capability *cap)
{
    cap->tsi_port_cnt = mgmt->tsi_port_cnt;
    cap->ram_port_cnt = mgmt->ram_port_cnt;
    cap->pid_chan_cnt = mgmt->pid_channel_cnt;
    cap->raw_chan_cnt = mgmt->raw_channel_cnt;
    cap->sp_chan_cnt  = mgmt->sp_channel_cnt;
    cap->ivr_chan_cnt = mgmt->ivr_channel_cnt;

    /* stuff sid consume one sid, so here -1. */
    cap->sec_eng_cnt  = mgmt->se_cnt - 1;

    return HI_SUCCESS;
}

static hi_s32 tsio_mgmt_get_out_of_sync_cnt_impl(struct tsio_mgmt *mgmt, hi_u32 *cnt)
{
    *cnt = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_DESYNC_CNT);

    return HI_SUCCESS;
}

static hi_s32 _tsio_mgmt_suspend_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    /* tsi port */
    osal_mutex_lock(&mgmt->tsi_port_list_lock);

    list_for_each(node, &mgmt->tsi_port_head) {
        struct tsio_r_tsi_port *tsi_port = list_entry(node, struct tsio_r_tsi_port, node);

        ret = tsi_port->ops->suspend(tsi_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("suspend tsi port(%d) failed.\n", tsi_port->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->tsi_port_list_lock);

    /* ram port */
    osal_mutex_lock(&mgmt->ram_port_list_lock);

    list_for_each(node, &mgmt->ram_port_head) {
        struct tsio_r_ram_port *ram_port = list_entry(node, struct tsio_r_ram_port, node);

        ret = ram_port->ops->suspend(ram_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("suspend ram port(%d) failed.\n", ram_port->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    /* pid channel */
    osal_mutex_lock(&mgmt->pid_channel_list_lock);

    list_for_each(node, &mgmt->pid_channel_head) {
        struct tsio_r_pid_channel *pid_channel = list_entry(node, struct tsio_r_pid_channel, node);

        ret = pid_channel->ops->suspend(pid_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("suspend pid channel(%d) failed.\n", pid_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    /* raw channel */
    osal_mutex_lock(&mgmt->raw_channel_list_lock);

    list_for_each(node, &mgmt->raw_channel_head) {
        struct tsio_r_raw_channel *raw_channel = list_entry(node, struct tsio_r_raw_channel, node);

        ret = raw_channel->ops->suspend(raw_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("suspend raw channel(%d) failed.\n", raw_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->raw_channel_list_lock);

    /* sp channel */
    osal_mutex_lock(&mgmt->sp_channel_list_lock);

    list_for_each(node, &mgmt->sp_channel_head) {
        struct tsio_r_sp_channel *sp_channel = list_entry(node, struct tsio_r_sp_channel, node);

        ret = sp_channel->ops->suspend(sp_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("suspend sp channel(%d) failed.\n", sp_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->sp_channel_list_lock);

    /* ivrestart channel */
    osal_mutex_lock(&mgmt->ivr_channel_list_lock);

    list_for_each(node, &mgmt->ivr_channel_head) {
        struct tsio_r_ivr_channel *ivr_channel = list_entry(node, struct tsio_r_ivr_channel, node);

        ret = ivr_channel->ops->suspend(ivr_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("suspend ivr channel(%d) failed.\n", ivr_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->ivr_channel_list_lock);

    /* se */
    osal_mutex_lock(&mgmt->se_list_lock);

    list_for_each(node, &mgmt->se_head) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

        ret = se->ops->suspend(se);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("suspend secure engine(%d) failed.\n", se->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->se_list_lock);

    /* mask phy & ctrl int. */
    tsio_hal_dis_phy_int(mgmt);
    tsio_hal_dis_all_int(mgmt);

    /* phy */
    tsio_hal_de_init_phy(mgmt);

    /* ctrl */
    tsio_hal_de_init_ctrl(mgmt);

    return HI_SUCCESS;
}

/*
 * Success must be returned regardless of whether MGMT is initialized.
 * otherwise it will affect the entire standby process.
 */
static hi_s32 tsio_mgmt_suspend_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        ret = HI_ERR_TSIO_NOT_INIT;
    } else {
        if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
            HI_DBG_TSIO("inc ref_count error.\n");
        }

        ret = HI_SUCCESS;
    }

    osal_mutex_unlock(&mgmt->lock);

    if (ret == HI_SUCCESS) {
        _tsio_mgmt_suspend_impl(mgmt);

        if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
            HI_DBG_TSIO("dec ref_count error.\n");
        }
    }

    HI_PRINT("TSIO suspend ok\n");

    return HI_SUCCESS;
}

static hi_s32 _tsio_mgmt_resume_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    /* ctrl */
    tsio_hal_init_ctrl(mgmt);

    tsio_hal_en_mmu(mgmt);

    tsio_hal_en_all_int(mgmt);

    tsio_hal_en_stuff_srv(mgmt, mgmt->stuff_sid - TSIO_SID_0);

    /* phy */
    tsio_hal_init_phy(mgmt);

    /* se */
    osal_mutex_lock(&mgmt->se_list_lock);

    list_for_each_prev(node, &mgmt->se_head) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

        ret = se->ops->resume(se);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("resume secure engine(%d) failed.\n", se->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->se_list_lock);

    /* ivrestart channel */
    osal_mutex_lock(&mgmt->ivr_channel_list_lock);

    list_for_each_prev(node, &mgmt->ivr_channel_head) {
        struct tsio_r_ivr_channel *ivr_channel = list_entry(node, struct tsio_r_ivr_channel, node);

        ret = ivr_channel->ops->resume(ivr_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("resume ivr channel(%d) failed.\n", ivr_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->ivr_channel_list_lock);

    /* sp channel */
    osal_mutex_lock(&mgmt->sp_channel_list_lock);

    list_for_each_prev(node, &mgmt->sp_channel_head) {
        struct tsio_r_sp_channel *sp_channel = list_entry(node, struct tsio_r_sp_channel, node);

        ret = sp_channel->ops->resume(sp_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("resume sp channel(%d) failed.\n", sp_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->sp_channel_list_lock);

    /* raw channel */
    osal_mutex_lock(&mgmt->raw_channel_list_lock);

    list_for_each_prev(node, &mgmt->raw_channel_head) {
        struct tsio_r_raw_channel *raw_channel = list_entry(node, struct tsio_r_raw_channel, node);

        ret = raw_channel->ops->resume(raw_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("resume raw channel(%d) failed.\n", raw_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->raw_channel_list_lock);

    /* pid channel */
    osal_mutex_lock(&mgmt->pid_channel_list_lock);

    list_for_each_prev(node, &mgmt->pid_channel_head) {
        struct tsio_r_pid_channel *pid_channel = list_entry(node, struct tsio_r_pid_channel, node);

        ret = pid_channel->ops->resume(pid_channel);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("resume pid channel(%d) failed.\n", pid_channel->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    /* ram port */
    osal_mutex_lock(&mgmt->ram_port_list_lock);

    list_for_each_prev(node, &mgmt->ram_port_head) {
        struct tsio_r_ram_port *ram_port = list_entry(node, struct tsio_r_ram_port, node);

        ret = ram_port->ops->resume(ram_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("resume ram port(%d) failed.\n", ram_port->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    /* tsi port */
    osal_mutex_lock(&mgmt->tsi_port_list_lock);

    list_for_each_prev(node, &mgmt->tsi_port_head) {
        struct tsio_r_tsi_port *tsi_port = list_entry(node, struct tsio_r_tsi_port, node);

        ret = tsi_port->ops->resume(tsi_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_TSIO("resume tsi port(%d) failed.\n", tsi_port->base.id);
        }
    }

    osal_mutex_unlock(&mgmt->tsi_port_list_lock);

    return HI_SUCCESS;
}

/*
 * Success must be returned regardless of whether MGMT is initialized.
 * otherwise it will affect the entire standby process.
 */
static hi_s32 tsio_mgmt_resume_impl(struct tsio_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        ret = HI_ERR_TSIO_NOT_INIT;
    } else {
        if (osal_atomic_inc_return(&mgmt->ref_count) == 0) {
            HI_DBG_TSIO("inc ref_count err.\n");
        }
        ret = HI_SUCCESS;
    }

    osal_mutex_unlock(&mgmt->lock);

    if (ret == HI_SUCCESS) {
        _tsio_mgmt_resume_impl(mgmt);

        if (osal_atomic_dec_return(&mgmt->ref_count) != 0) {
            HI_DBG_TSIO("ref_count is not 0.\n");
        }
    }

    HI_PRINT("TSIO resume ok\n");

    return HI_SUCCESS;
}

static hi_s32 _tsio_mgmt_create_tsi_port_impl(struct tsio_mgmt *mgmt, tsio_port port,
    const tsio_tsi_port_attrs_ex *attrs, struct tsio_r_tsi_port **tsi_port)
{
    hi_s32 ret;
    hi_u32 id;
    struct tsio_r_tsi_port *new_tsi_port = HI_NULL;

    if (port == TSIO_ANY_TSI_PORT) { /* choose any available */
        id = find_first_zero_bit(mgmt->tsi_port_bitmap, mgmt->tsi_port_cnt);
        if (!(id < mgmt->tsi_port_cnt)) {
            HI_ERR_TSIO("there is no available tsi port now!\n");
            ret = HI_ERR_TSIO_NO_RESOURCE;
            goto out;
        }
    } else if (port >= TSIO_TSI_PORT_0 && port < TSIO_TSI_PORT_BUTT) {
        unsigned long mask, *p;

        id = port - TSIO_TSI_PORT_0;

        mask = BIT_MASK(id);
        p = ((unsigned long *)mgmt->tsi_port_bitmap) + BIT_WORD(id);
        if (*p & mask) {
            HI_ERR_TSIO("the TSI port(%d) is busy now!.\n", port);
            ret = HI_ERR_TSIO_BUSY;
            goto out;
        }
    } else {
        HI_FATAL_TSIO("port is mismatch, port is:%u!\n", port);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    new_tsi_port = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_tsi_port), GFP_KERNEL);
    if (new_tsi_port == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = osal_atomic_init(&new_tsi_port->base.ref_count);
    if (ret != 0) {
        HI_ERR_TSIO("ref_count init error!\n");
        goto out;
    }

    osal_atomic_set(&new_tsi_port->base.ref_count, 1);
    new_tsi_port->base.ops     = get_tsio_r_base_ops();
    new_tsi_port->base.release = tsio_mgmt_destroy_tsi_port;
    new_tsi_port->base.id      = id;
    new_tsi_port->base.mgmt    = mgmt;

    osal_mutex_init(&new_tsi_port->lock);
    new_tsi_port->ops = &g_tsio_tsi_port_ops;
    list_add_tail(&new_tsi_port->node, &mgmt->tsi_port_head);

    new_tsi_port->source_port      = attrs->source_port;
    new_tsi_port->dmx_if_port_cnt  = attrs->dmx_if_port_cnt;
    new_tsi_port->dmx_tsi_port_cnt = attrs->dmx_tsi_port_cnt;

    *tsi_port = new_tsi_port;

    HI_DBG_TSIO("#    robj(0x%x) created.\n", new_tsi_port);

    set_bit(id, mgmt->tsi_port_bitmap);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_create_tsi_port_impl(struct tsio_mgmt *mgmt, tsio_port port,
    const tsio_tsi_port_attrs_ex *attrs, struct tsio_r_tsi_port **tsi_port)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->tsi_port_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_create_tsi_port_impl(mgmt, port, attrs, tsi_port);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->tsi_port_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_destroy_tsi_port_impl(struct tsio_mgmt *mgmt, struct tsio_r_tsi_port *tsi_port)
{
    hi_s32 ret;
    unsigned long mask, *p;

    mask = BIT_MASK(tsi_port->base.id);
    p = ((unsigned long *)mgmt->tsi_port_bitmap) + BIT_WORD(tsi_port->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("TSI port(%d) is invalid.\n", tsi_port->base.id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(tsi_port->base.id, mgmt->tsi_port_bitmap);
    osal_mutex_destory(&tsi_port->lock);
    osal_atomic_destory(&tsi_port->base.ref_count);
    list_del(&tsi_port->node);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_destroy_tsi_port_impl(struct tsio_mgmt *mgmt, struct tsio_r_tsi_port *tsi_port)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->tsi_port_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_destroy_tsi_port_impl(mgmt, tsi_port);
    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, tsi_port);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->tsi_port_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_create_ram_port_impl(struct tsio_mgmt *mgmt, tsio_port port,
    const tsio_ram_port_attrs_ex *attrs, struct tsio_r_ram_port **ram_port)
{
    hi_s32 ret;
    hi_u32 id;
    struct tsio_r_ram_port *new_ram_port = HI_NULL;
    hi_size_t lock_flag;

    if (port == TSIO_ANY_RAM_PORT) { /* choose any available */
        id = find_first_zero_bit(mgmt->ram_port_bitmap, mgmt->ram_port_cnt);
        if (!(id < mgmt->ram_port_cnt)) {
            HI_ERR_TSIO("there is no available ram port now!\n");
            ret = HI_ERR_TSIO_NO_RESOURCE;
            goto out;
        }
    } else if (port >= TSIO_RAM_PORT_0 && port < TSIO_RAM_PORT_BUTT) {
        unsigned long mask, *p;

        id = port - TSIO_RAM_PORT_0;

        mask = BIT_MASK(id);
        p = ((unsigned long *)mgmt->ram_port_bitmap) + BIT_WORD(id);
        if (*p & mask) {
            HI_ERR_TSIO("the RAM port(%d) is busy now!.\n", port);
            ret = HI_ERR_TSIO_BUSY;
            goto out;
        }
    } else {
        HI_FATAL_TSIO("port is mismatch, port is:%u!\n", port);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    new_ram_port = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_ram_port), GFP_KERNEL);
    if (new_ram_port == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = osal_atomic_init(&new_ram_port->base.ref_count);
    if (ret != 0) {
        HI_ERR_TSIO("ref_count init error!\n");
        goto out;
    }

    osal_atomic_set(&new_ram_port->base.ref_count, 1);
    new_ram_port->base.ops     = get_tsio_r_base_ops();
    new_ram_port->base.release = tsio_mgmt_destroy_ram_port;
    new_ram_port->base.id      = id;
    new_ram_port->base.mgmt    = mgmt;

    osal_mutex_init(&new_ram_port->lock);
    new_ram_port->ops = &g_tsio_ram_port_ops;

    osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
    list_add_tail(&new_ram_port->node, &mgmt->ram_port_head);
    osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);

    new_ram_port->port          = port;
    new_ram_port->source_type   = attrs->source_type;
    new_ram_port->max_data_rate = attrs->max_data_rate;
    new_ram_port->pace          = attrs->pace;

    new_ram_port->pkt_size      = (TSIO_SOURCE_TS == new_ram_port->source_type) ? TS_PKT_SIZE : BULK_PKT_SIZE;
    new_ram_port->blk_size      = new_ram_port->pkt_size * BLK_DEFAULT_PKT_NUM;
    new_ram_port->buf_size      = attrs->buffer_size - attrs->buffer_size % new_ram_port->blk_size;
    new_ram_port->buf_handle    = -1;
    new_ram_port->buf_obj       = HI_NULL;
    new_ram_port->dsc_handle    = -1;
    new_ram_port->dsc_buf_obj   = HI_NULL;

    osal_mutex_init(&new_ram_port->blk_map_lock);

    init_waitqueue_head(&new_ram_port->wait_queue);
    new_ram_port->wait_cond = 0;

    INIT_WORK(&new_ram_port->flow_control_worker, flow_control_handler);
    INIT_WORK(&new_ram_port->dsc_end_worker, dsc_end_irq_handler);

    osal_mutex_init(&new_ram_port->bulk_srv_lock);
    INIT_LIST_HEAD(&new_ram_port->bulk_srv_list);
    new_ram_port->bulk_srv_cnt = 0;

    *ram_port = new_ram_port;

    HI_DBG_TSIO("#    robj(0x%x) created.\n", new_ram_port);

    set_bit(id, mgmt->ram_port_bitmap);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_create_ram_port_impl(struct tsio_mgmt *mgmt, tsio_port port,
    const tsio_ram_port_attrs_ex *attrs, struct tsio_r_ram_port **ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->ram_port_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_create_ram_port_impl(mgmt, port, attrs, ram_port);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_destroy_ram_port_impl(struct tsio_mgmt *mgmt, struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;
    unsigned long mask, *p;
    hi_size_t lock_flag;

    mask = BIT_MASK(ram_port->base.id);
    p = ((unsigned long *)mgmt->ram_port_bitmap) + BIT_WORD(ram_port->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("RAM port(%d) is invalid.\n", ram_port->base.id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(ram_port->base.id, mgmt->ram_port_bitmap);

    osal_mutex_destory(&ram_port->bulk_srv_lock);
    osal_mutex_destory(&ram_port->blk_map_lock);
    osal_mutex_destory(&ram_port->lock);
    osal_atomic_destory(&ram_port->base.ref_count);

    osal_spin_lock_irqsave(&mgmt->ram_port_list_lock2, &lock_flag);
    list_del(&ram_port->node);
    osal_spin_unlock_irqrestore(&mgmt->ram_port_list_lock2, &lock_flag);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_destroy_ram_port_impl(struct tsio_mgmt *mgmt, struct tsio_r_ram_port *ram_port)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->ram_port_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_destroy_ram_port_impl(mgmt, ram_port);
    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, ram_port);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->ram_port_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_create_pid_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_base *port, hi_u32 pid,
    struct tsio_r_pid_channel **pid_channel)
{
    hi_s32 ret;
    hi_u32 id;
    struct tsio_r_pid_channel *new_pid_channel = HI_NULL;

    id = find_first_zero_bit(mgmt->pid_channel_bitmap, mgmt->pid_channel_cnt);
    if (!(id < mgmt->pid_channel_cnt)) {
        HI_ERR_TSIO("there is no available pid channel now!\n");
        ret = HI_ERR_TSIO_NO_RESOURCE;
        goto out;
    }

    new_pid_channel = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_pid_channel), GFP_KERNEL);
    if (new_pid_channel == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = osal_atomic_init(&new_pid_channel->base.ref_count);
    if (ret != 0) {
        HI_ERR_TSIO("ref_count init error!\n");
        goto out;
    }

    osal_atomic_set(&new_pid_channel->base.ref_count, 1);
    new_pid_channel->base.ops     = get_tsio_r_base_ops();
    new_pid_channel->base.release = tsio_mgmt_destroy_pid_channel;
    new_pid_channel->base.id      = id;
    new_pid_channel->base.mgmt    = mgmt;

    osal_mutex_init(&new_pid_channel->lock);
    new_pid_channel->ops = &g_tsio_pid_channel_ops;
    list_add_tail(&new_pid_channel->node, &mgmt->pid_channel_head);

    new_pid_channel->pid  = pid;
    new_pid_channel->port = port;
    new_pid_channel->se   = HI_NULL;

    *pid_channel = new_pid_channel;

    HI_DBG_TSIO("#    robj(0x%x) created.\n", new_pid_channel);

    set_bit(id, mgmt->pid_channel_bitmap);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_create_pid_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_base *port, hi_u32 pid,
    struct tsio_r_pid_channel **pid_channel)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    osal_mutex_lock(&mgmt->pid_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    /* one PID is linked to 1 SID,
    but different sids can have pids with the same number (but different content). from NAGRA. */
    list_for_each(node, &mgmt->pid_channel_head) {
        struct tsio_r_pid_channel *pid_channel = list_entry(node, struct tsio_r_pid_channel, node);

        if (pid_channel->port == port && pid_channel->pid == pid) {
            HI_ERR_TSIO("not support add duplicate port(%d) and pid(%d).\n", port->id, pid);
            ret = HI_ERR_TSIO_BUSY;
            goto out;
        }
    }

    ret = _tsio_mgmt_create_pid_channel_impl(mgmt, port, pid, pid_channel);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_destroy_pid_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_pid_channel *pid_channel,
    struct tsio_r_base **port)
{
    hi_s32 ret;
    unsigned long mask, *p;

    mask = BIT_MASK(pid_channel->base.id);
    p = ((unsigned long *)mgmt->pid_channel_bitmap) + BIT_WORD(pid_channel->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("pid channel id(%d) is invalid.\n", pid_channel->base.id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(pid_channel->base.id, mgmt->pid_channel_bitmap);
    osal_mutex_destory(&pid_channel->lock);
    osal_atomic_destory(&pid_channel->base.ref_count);

    list_del(&pid_channel->node);

    *port = pid_channel->port;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_destroy_pid_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_pid_channel *pid_channel)
{
    hi_s32 ret;
    struct tsio_r_base *port = HI_NULL;

    osal_mutex_lock(&mgmt->pid_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_destroy_pid_channel_impl(mgmt, pid_channel, &port);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->pid_channel_list_lock);

    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, pid_channel);

        tsio_r_put(port);
    }

    return ret;
}

static hi_s32 _tsio_mgmt_create_raw_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_base *port,
    struct tsio_r_raw_channel **raw_channel)
{
    hi_s32 ret;
    hi_u32 id;
    struct tsio_r_raw_channel *new_raw_channel = HI_NULL;

    id = find_first_zero_bit(mgmt->raw_channel_bitmap, mgmt->raw_channel_cnt);
    if (!(id < mgmt->raw_channel_cnt)) {
        HI_ERR_TSIO("there is no available raw channel now!\n");
        ret = HI_ERR_TSIO_NO_RESOURCE;
        goto out;
    }

    new_raw_channel = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_raw_channel), GFP_KERNEL);
    if (new_raw_channel == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = osal_atomic_init(&new_raw_channel->base.ref_count);
    if (ret != 0) {
        HI_ERR_TSIO("ref_count init error!\n");
        goto out;
    }

    osal_atomic_set(&new_raw_channel->base.ref_count, 1);
    new_raw_channel->base.ops     = get_tsio_r_base_ops();
    new_raw_channel->base.release = tsio_mgmt_destroy_raw_channel;
    new_raw_channel->base.id      = id;
    new_raw_channel->base.mgmt    = mgmt;

    osal_mutex_init(&new_raw_channel->lock);
    new_raw_channel->ops = &g_tsio_raw_channel_ops;
    list_add_tail(&new_raw_channel->node, &mgmt->raw_channel_head);

    new_raw_channel->port = port;
    new_raw_channel->se   = HI_NULL;

    *raw_channel = new_raw_channel;

    HI_DBG_TSIO("#    robj(0x%x) created.\n", new_raw_channel);

    set_bit(id, mgmt->raw_channel_bitmap);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_create_raw_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_base *port,
    struct tsio_r_raw_channel **raw_channel)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->raw_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_create_raw_channel_impl(mgmt, port, raw_channel);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->raw_channel_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_destroy_raw_channel_impl(struct tsio_mgmt *mgmt,
    struct tsio_r_raw_channel *raw_channel, struct tsio_r_base **port)
{
    hi_s32 ret;
    unsigned long mask, *p;

    mask = BIT_MASK(raw_channel->base.id);
    p = ((unsigned long *)mgmt->raw_channel_bitmap) + BIT_WORD(raw_channel->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("raw channel id(%d) is invalid.\n", raw_channel->base.id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(raw_channel->base.id, mgmt->raw_channel_bitmap);
    osal_mutex_destory(&raw_channel->lock);
    osal_atomic_destory(&raw_channel->base.ref_count);

    list_del(&raw_channel->node);

    *port = raw_channel->port;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_destroy_raw_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_raw_channel *raw_channel)
{
    hi_s32 ret;
    struct tsio_r_base *port = HI_NULL;

    osal_mutex_lock(&mgmt->raw_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_destroy_raw_channel_impl(mgmt, raw_channel, &port);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->raw_channel_list_lock);

    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, raw_channel);

        tsio_r_put(port);
    }

    return ret;
}

static hi_s32 _tsio_mgmt_create_sp_channel_impl(struct tsio_mgmt *mgmt,
    struct tsio_r_base *port, struct tsio_r_sp_channel **sp_channel)
{
    hi_s32 ret;
    hi_u32 id;
    struct tsio_r_sp_channel *new_sp_channel = HI_NULL;

    id = find_first_zero_bit(mgmt->sp_channel_bitmap, mgmt->sp_channel_cnt);
    if (!(id < mgmt->sp_channel_cnt)) {
        HI_ERR_TSIO("there is no available sp channel now!\n");
        ret = HI_ERR_TSIO_NO_RESOURCE;
        goto out;
    }

    new_sp_channel = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_sp_channel), GFP_KERNEL);
    if (new_sp_channel == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = osal_atomic_init(&new_sp_channel->base.ref_count);
    if (ret != 0) {
        HI_ERR_TSIO("ref_count init error!\n");
        goto out;
    }

    osal_atomic_set(&new_sp_channel->base.ref_count, 1);
    new_sp_channel->base.ops     = get_tsio_r_base_ops();
    new_sp_channel->base.release = tsio_mgmt_destroy_sp_channel;
    new_sp_channel->base.id      = id;
    new_sp_channel->base.mgmt    = mgmt;

    osal_mutex_init(&new_sp_channel->lock);
    new_sp_channel->ops = &g_tsio_sp_channel_ops;
    list_add_tail(&new_sp_channel->node, &mgmt->sp_channel_head);

    new_sp_channel->port = port;
    new_sp_channel->se   = HI_NULL;

    *sp_channel = new_sp_channel;

    HI_DBG_TSIO("#    robj(0x%x) created.\n", new_sp_channel);

    set_bit(id, mgmt->sp_channel_bitmap);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_create_sp_channel_impl(struct tsio_mgmt *mgmt,
    struct tsio_r_base *port, struct tsio_r_sp_channel **sp_channel)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->sp_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_create_sp_channel_impl(mgmt, port, sp_channel);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->sp_channel_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_destroy_sp_channel_impl(struct tsio_mgmt *mgmt,
    struct tsio_r_sp_channel *sp_channel, struct tsio_r_base **port)
{
    hi_s32 ret;
    unsigned long mask, *p;

    mask = BIT_MASK(sp_channel->base.id);
    p = ((unsigned long *)mgmt->sp_channel_bitmap) + BIT_WORD(sp_channel->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("sp channel id(%d) is invalid.\n", sp_channel->base.id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(sp_channel->base.id, mgmt->sp_channel_bitmap);
    osal_mutex_destory(&sp_channel->lock);
    osal_atomic_destory(&sp_channel->base.ref_count);

    list_del(&sp_channel->node);

    *port = sp_channel->port;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_destroy_sp_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_sp_channel *sp_channel)
{
    hi_s32 ret;
    struct tsio_r_base *port = HI_NULL;

    osal_mutex_lock(&mgmt->sp_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_destroy_sp_channel_impl(mgmt, sp_channel, &port);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->sp_channel_list_lock);

    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, sp_channel);

        tsio_r_put(port);
    }

    return ret;
}

static hi_s32 _tsio_mgmt_create_ivr_channel_impl(struct tsio_mgmt *mgmt,
    struct tsio_r_base *port, struct tsio_r_ivr_channel **ivr_channel)
{
    hi_s32 ret;
    hi_u32 id;
    struct tsio_r_ivr_channel *new_ivr_channel = HI_NULL;

    id = find_first_zero_bit(mgmt->ivr_channel_bitmap, mgmt->ivr_channel_cnt);
    if (!(id < mgmt->ivr_channel_cnt)) {
        HI_ERR_TSIO("there is no available ivrestart channel now!\n");
        ret = HI_ERR_TSIO_NO_RESOURCE;
        goto out;
    }

    new_ivr_channel = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_ivr_channel), GFP_KERNEL);
    if (new_ivr_channel == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = osal_atomic_init(&new_ivr_channel->base.ref_count);
    if (ret != 0) {
        HI_ERR_TSIO("ref_count init error!\n");
        goto out;
    }

    osal_atomic_set(&new_ivr_channel->base.ref_count, 1);
    new_ivr_channel->base.ops     = get_tsio_r_base_ops();
    new_ivr_channel->base.release = tsio_mgmt_destroy_ivr_channel;
    new_ivr_channel->base.id      = id;
    new_ivr_channel->base.mgmt    = mgmt;

    osal_mutex_init(&new_ivr_channel->lock);
    new_ivr_channel->ops = &g_tsio_ivr_channel_ops;
    list_add_tail(&new_ivr_channel->node, &mgmt->ivr_channel_head);

    new_ivr_channel->port = port;
    new_ivr_channel->se   = HI_NULL;

    *ivr_channel = new_ivr_channel;

    HI_DBG_TSIO("#    robj(0x%x) created.\n", new_ivr_channel);

    set_bit(id, mgmt->ivr_channel_bitmap);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_create_ivr_channel_impl(struct tsio_mgmt *mgmt,
    struct tsio_r_base *port, struct tsio_r_ivr_channel **ivr_channel)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->ivr_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_create_ivr_channel_impl(mgmt, port, ivr_channel);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->ivr_channel_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_destroy_ivr_channel_impl(struct tsio_mgmt *mgmt,
    struct tsio_r_ivr_channel *ivr_channel, struct tsio_r_base **port)
{
    hi_s32 ret;
    unsigned long mask, *p;

    mask = BIT_MASK(ivr_channel->base.id);
    p = ((unsigned long *)mgmt->ivr_channel_bitmap) + BIT_WORD(ivr_channel->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("ivrestart channel id(%d) is invalid.\n", ivr_channel->base.id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(ivr_channel->base.id, mgmt->ivr_channel_bitmap);
    osal_mutex_destory(&ivr_channel->lock);
    osal_atomic_destory(&ivr_channel->base.ref_count);

    list_del(&ivr_channel->node);

    *port = ivr_channel->port;

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_destroy_ivr_channel_impl(struct tsio_mgmt *mgmt, struct tsio_r_ivr_channel *ivr_channel)
{
    hi_s32 ret;
    struct tsio_r_base *port = HI_NULL;

    osal_mutex_lock(&mgmt->ivr_channel_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_destroy_ivr_channel_impl(mgmt, ivr_channel, &port);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->ivr_channel_list_lock);

    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, ivr_channel);

        tsio_r_put(port);
    }

    return ret;
}

static hi_s32 _tsio_mgmt_create_se_impl(struct tsio_mgmt *mgmt,
    tsio_sid sid, const tsio_secure_engine_attrs *attrs, struct tsio_r_se **se)
{
    hi_s32 ret;
    hi_u32 id;
    struct tsio_r_se *new_se = HI_NULL;
    hi_size_t lock_flag;

    if (sid == TSIO_ANY_SID) { /* choose any available */
        id = find_first_zero_bit(mgmt->se_bitmap, mgmt->se_cnt);
        if (!(id < mgmt->se_cnt)) {
            HI_ERR_TSIO("there is no available secure engine now!\n");
            ret = HI_ERR_TSIO_NO_RESOURCE;
            goto out;
        }
    } else if (sid < TSIO_SID_BUTT) {
        unsigned long mask;
        unsigned long *p = HI_NULL;

        id = sid - TSIO_SID_0;

        mask = BIT_MASK(id);
        p = ((unsigned long *)mgmt->se_bitmap) + BIT_WORD(id);
        if (*p & mask) {
            HI_ERR_TSIO("the secure engine(%d) is busy now!.\n", sid);
            ret = HI_ERR_TSIO_BUSY;
            goto out;
        }
    } else {
        HI_FATAL_TSIO("tsio sid is mismatch, tsio sid is:%u!\n", sid);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    new_se = HI_KZALLOC(HI_ID_TSIO, sizeof(struct tsio_r_se), GFP_KERNEL);
    if (new_se == HI_NULL) {
        ret = HI_ERR_TSIO_NO_MEMORY;
        goto out;
    }

    ret = osal_atomic_init(&new_se->base.ref_count);
    if (ret != 0) {
        HI_ERR_TSIO("ref_count init error!\n");
        goto out;
    }

    osal_atomic_set(&new_se->base.ref_count, 1);
    new_se->base.ops     = get_tsio_r_base_ops();
    new_se->base.release = tsio_mgmt_destroy_se;
    new_se->base.id      = id;
    new_se->base.mgmt    = mgmt;

    osal_mutex_init(&new_se->lock);
    new_se->ops = &g_tsio_se_ops;

    osal_spin_lock_irqsave(&mgmt->se_list_lock2, &lock_flag);
    list_add_tail(&new_se->node, &mgmt->se_head);
    osal_spin_unlock_irqrestore(&mgmt->se_list_lock2, &lock_flag);

    new_se->stat = TSIO_SECURE_ENGINE_CLOSED;
    new_se->mod  = attrs->mod;
    if (new_se->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        new_se->dest_port = attrs->dest_port;
    } else if (new_se->mod == TSIO_SECURE_ENGINE_OUTPUT2RAM) {
        new_se->buf_size = attrs->buffer_size;
    } else {
        HI_FATAL_TSIO("tsio se's mode is mismatch, tsio se's mode is:%u\n", new_se->mod);
    }
    new_se->buf_handle = -1;
    new_se->buf_obj = HI_NULL;

    INIT_LIST_HEAD(&new_se->chn_head);

    INIT_WORK(&new_se->buf_ovfl_worker, obuf_ovfl_handler);

    new_se->wait_cond = 0;
    init_waitqueue_head(&new_se->wait_queue);

    INIT_LIST_HEAD(&new_se->bulk_srv_node);
    new_se->wait_flush_cond = 0;
    init_waitqueue_head(&new_se->wait_flush_queue);

    *se = new_se;

    HI_DBG_TSIO("#    robj(0x%x) created.\n", new_se);
    set_bit(id, mgmt->se_bitmap);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_create_se_impl(struct tsio_mgmt *mgmt, tsio_sid sid,
    const tsio_secure_engine_attrs *attrs, struct tsio_r_se **se)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->se_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    /* it's hard to do flow control if allow multiple secure engine output data into same demux port. */
    if (attrs->mod == TSIO_SECURE_ENGINE_OUTPUT2DMX) {
        struct list_head *node = HI_NULL;

        list_for_each(node, &mgmt->se_head) {
            struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

            if (se->mod == attrs->mod && se->dest_port == attrs->dest_port) {
                HI_ERR_TSIO("not support multiple secure engine output data into same demux port(%d).\n",
                    attrs->dest_port);
                ret = HI_ERR_TSIO_BUSY;
                goto out;
            }
        }
    }

    ret = _tsio_mgmt_create_se_impl(mgmt, sid, attrs, se);

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->se_list_lock);

    return ret;
}

static hi_s32 _tsio_mgmt_destroy_se_impl(struct tsio_mgmt *mgmt, struct tsio_r_se *se)
{
    hi_s32 ret;
    unsigned long mask, *p;
    hi_size_t lock_flag;

    mask = BIT_MASK(se->base.id);
    p = ((unsigned long *)mgmt->se_bitmap) + BIT_WORD(se->base.id);
    if (!(*p & mask)) {
        HI_ERR_TSIO("secure engine id(%d) is invalid.\n", se->base.id);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    clear_bit(se->base.id, mgmt->se_bitmap);

    osal_mutex_destory(&se->lock);
    osal_atomic_destory(&se->base.ref_count);

    osal_spin_lock_irqsave(&mgmt->se_list_lock2, &lock_flag);
    list_del(&se->node);
    osal_spin_unlock_irqrestore(&mgmt->se_list_lock2, &lock_flag);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 tsio_mgmt_destroy_se_impl(struct tsio_mgmt *mgmt, struct tsio_r_se *se)
{
    hi_s32 ret;

    osal_mutex_lock(&mgmt->se_list_lock);

    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = _tsio_mgmt_destroy_se_impl(mgmt, se);
    if (ret == HI_SUCCESS) {
        HI_KFREE(HI_ID_TSIO, se);
    }

out:
    osal_mutex_unlock(&mgmt->lock);

    osal_mutex_unlock(&mgmt->se_list_lock);

    return ret;
}

static hi_void _tsio_mgmt_show_info_impl(struct tsio_mgmt *mgmt)
{
    struct list_head *node = HI_NULL;

    HI_ERR_TSIO("* mgmt refer count:%d.\n", mgmt->ref_count);

    /* TSI port */
    list_for_each(node, &mgmt->tsi_port_head) {
        struct tsio_r_tsi_port *tsi_port = list_entry(node, struct tsio_r_tsi_port, node);

        HI_ERR_TSIO("* TsiPort:0x%x, id:%d, source_port:%d, refer_cnt:%d \n", tsi_port, tsi_port->base.id,
                    tsi_port->source_port, tsi_port->base.ref_count);
        TSIO_UNUSED(tsi_port);
    }

    /* RAM port */
    list_for_each(node, &mgmt->ram_port_head) {
        struct tsio_r_ram_port *ram_port = list_entry(node, struct tsio_r_ram_port, node);

        HI_ERR_TSIO("* RamPort:0x%x, id:%d, source_type:%d, refer_cnt:%d  \n", ram_port, ram_port->base.id,
                    ram_port->source_type, ram_port->base.ref_count);
        TSIO_UNUSED(ram_port);
    }

    /* PID channel */
    list_for_each(node, &mgmt->pid_channel_head) {
        struct tsio_r_pid_channel *pid_channel = list_entry(node, struct tsio_r_pid_channel, node);

        HI_ERR_TSIO("* pid_channel:0x%x id:%d, pid:%d, refer_cnt:%d  \n",
                    pid_channel, pid_channel->base.id, pid_channel->pid, pid_channel->base.ref_count);
        TSIO_UNUSED(pid_channel);
    }

    /* raw channel */
    list_for_each(node, &mgmt->raw_channel_head) {
        struct tsio_r_raw_channel *raw_channel = list_entry(node, struct tsio_r_raw_channel, node);

        HI_ERR_TSIO("* raw_channel:0x%x id:%d, refer_cnt:%d  \n", raw_channel, raw_channel->base.id,
                    raw_channel->base.ref_count);
        TSIO_UNUSED(raw_channel);
    }

    /* SP channel */
    list_for_each(node, &mgmt->sp_channel_head) {
        struct tsio_r_sp_channel *sp_channel = list_entry(node, struct tsio_r_sp_channel, node);

        HI_ERR_TSIO("* SpChannel:0x%x id:%d, refer_cnt:%d  \n", sp_channel,
                    sp_channel->base.id, sp_channel->base.ref_count);
        TSIO_UNUSED(sp_channel);
    }

    /* ivr channel */
    list_for_each(node, &mgmt->ivr_channel_head) {
        struct tsio_r_ivr_channel *ivr_channel = list_entry(node, struct tsio_r_ivr_channel, node);

        HI_ERR_TSIO("* ivr_channel:0x%x id:%d, refer_cnt:%d  \n", ivr_channel, ivr_channel->base.id,
                    ivr_channel->base.ref_count);
        TSIO_UNUSED(ivr_channel);
    }

    /* secure engine */
    list_for_each(node, &mgmt->se_head) {
        struct tsio_r_se *se = list_entry(node, struct tsio_r_se, node);

        HI_ERR_TSIO("* se:0x%x id:%d, refer_cnt:%d  \n", se, se->base.id, se->base.ref_count);
        TSIO_UNUSED(se);
    }
}

static hi_void tsio_mgmt_show_info_impl(struct tsio_mgmt *mgmt)
{
    osal_mutex_lock(&mgmt->lock);

    if (mgmt->state != TSIO_MGMT_OPENED) {
        HI_ERR_TSIO("mgmt has not opened.\n");
        goto out;
    }

    _tsio_mgmt_show_info_impl(mgmt);

out:
    osal_mutex_unlock(&mgmt->lock);
}

static struct tsio_mgmt_ops g_tsio_mgmt_ops = {
    .init                = tsio_mgmt_init_impl,
    .exit                = tsio_mgmt_exit_impl,

    .get_config          = tsio_mgmt_get_config_impl,
    .set_config          = tsio_mgmt_set_config_impl,

    .get_cap             = tsio_mgmt_get_cap_impl,
    .get_out_of_sync_cnt = tsio_mgmt_get_out_of_sync_cnt_impl,

    .suspend             = tsio_mgmt_suspend_impl,
    .resume              = tsio_mgmt_resume_impl,

    .create_tsi_port     = tsio_mgmt_create_tsi_port_impl,
    .destroy_tsi_port    = tsio_mgmt_destroy_tsi_port_impl,

    .create_ram_port     = tsio_mgmt_create_ram_port_impl,
    .destroy_ram_port    = tsio_mgmt_destroy_ram_port_impl,

    .create_pid_channel  = tsio_mgmt_create_pid_channel_impl,
    .destroy_pid_channel = tsio_mgmt_destroy_pid_channel_impl,

    .create_raw_channel  = tsio_mgmt_create_raw_channel_impl,
    .destroy_raw_channel = tsio_mgmt_destroy_raw_channel_impl,

    .create_sp_channel   = tsio_mgmt_create_sp_channel_impl,
    .destroy_sp_channel  = tsio_mgmt_destroy_sp_channel_impl,

    .create_ivr_channel  = tsio_mgmt_create_ivr_channel_impl,
    .destroy_ivr_channel = tsio_mgmt_destroy_ivr_channel_impl,

    .create_se           = tsio_mgmt_create_se_impl,
    .destroy_se          = tsio_mgmt_destroy_se_impl,

    .show_info           = tsio_mgmt_show_info_impl,
};

static struct tsio_mgmt g_tsio_mgmt = {
    .state     = TSIO_MGMT_CLOSED,
    .ops       = &g_tsio_mgmt_ops,
};

struct tsio_mgmt *__get_tsio_mgmt(hi_void)
{
    return &g_tsio_mgmt;
}

/*
 * it needs to be guaranteed that TsioMgmt has inited here.
 */
struct tsio_mgmt *get_tsio_mgmt(hi_void)
{
    struct tsio_mgmt *mgmt = __get_tsio_mgmt();

    return mgmt;
}
