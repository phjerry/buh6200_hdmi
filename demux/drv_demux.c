/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux export interface definition.
 * Author: sdk
 * Create: 2017-05-31
 */

#include "linux/fs.h"

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_drv_mem.h"

#include "hi_drv_demux.h"
#include "drv_demux_func.h"
#include "drv_demux_rmxfct.h"
#include "drv_demux_plyfct.h"
#include "drv_demux_recfct.h"
#include "drv_demux_dscfct.h"
#include "drv_demux_utils.h"

hi_s32 hi_drv_dmx_get_capability(dmx_capability *cap)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(cap, out);

    ret = dmx_mgmt_get_cap(cap);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_get_capability);


hi_s32 hi_drv_dmx_create_session(struct dmx_session **new_session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(new_session, out);

    ret = dmx_session_create(new_session);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_create_session);

hi_s32 hi_drv_dmx_destroy_session(struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_session_destroy(session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_destroy_session);

/******************* port begin ***********************/
hi_s32 hi_drv_dmx_tsi_get_port_attrs(dmx_port port, dmx_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_PORT_GOTO(port, out);

    ret = drv_dmx_tsi_get_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_tsi_get_port_attrs);

hi_s32 hi_drv_dmx_tsi_set_port_attrs(dmx_port port, const dmx_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_PORT_GOTO(port, out);

    ret = drv_dmx_tsi_set_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_tsi_set_port_attrs);

hi_s32 hi_drv_dmx_if_get_port_attrs(dmx_port port, dmx_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_PORT_GOTO(port, out);

    ret = drv_dmx_if_get_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_if_get_port_attrs);

hi_s32 hi_drv_dmx_if_set_port_attrs(dmx_port port, const dmx_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_PORT_GOTO(port, out);

    ret = drv_dmx_if_set_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_if_set_port_attrs);

hi_s32 hi_drv_dmx_tso_get_port_attrs(dmx_port port, dmx_tso_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_PORT_GOTO(port, out);

    ret = drv_dmx_tso_get_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_tso_get_port_attrs);

hi_s32 hi_drv_dmx_tso_set_port_attrs(dmx_port port, const dmx_tso_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_PORT_GOTO(port, out);

    ret = drv_dmx_tso_set_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_tso_set_port_attrs);

hi_s32 hi_drv_dmx_tag_get_port_attrs(dmx_port port, dmx_tag_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_TAG_PORT_GOTO(port, out);

    ret = drv_dmx_tag_get_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_tag_get_port_attrs);

hi_s32 hi_drv_dmx_tag_set_port_attrs(dmx_port port, const dmx_tag_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_INVALID_TAG_PORT_GOTO(port, out);

    ret = drv_dmx_tag_set_port_attrs(port, attrs);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_tag_set_port_attrs);

/***************** ramport begin **********************/
hi_s32 hi_drv_dmx_ram_open_port(dmx_port port, const dmx_ram_port_attr *attrs, hi_handle *handle,
                                struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_ram_open_port(port, attrs, handle, session);

    DMX_GLB_TRACE_EXIT();
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_open_port);

hi_s32 hi_drv_dmx_ram_get_port_attrs(hi_handle handle, dmx_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_get_port_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_get_port_attrs);

hi_s32 hi_drv_dmx_ram_set_port_attrs(hi_handle handle, const dmx_port_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_set_port_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_set_port_attrs);

hi_s32 hi_drv_dmx_ram_get_port_status(hi_handle handle, dmx_ram_port_status *status)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(status, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_get_port_status(handle, status);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_get_port_status);

hi_s32 hi_drv_dmx_ram_get_buffer(hi_handle handle, hi_u32 req_len, dmx_ram_buffer *buf, hi_u32 time_out) /* ms */
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf, out);
    DMX_FATAL_CON_GOTO(req_len == 0, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_get_buffer(handle, req_len, buf, time_out);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_get_buffer);

hi_s32 hi_drv_dmx_ram_push_buffer(hi_handle handle, dmx_ram_buffer *buf)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_push_buffer(handle, buf);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_push_buffer);

hi_s32 hi_drv_dmx_ram_put_buffer(hi_handle handle, hi_u32 valid_datalen, hi_u32 start_pos)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_put_buffer(handle, valid_datalen, start_pos);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_put_buffer);

hi_s32 hi_drv_dmx_ram_release_buffer(hi_handle handle, dmx_ram_buffer *buf)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_release_buffer(handle, buf);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_release_buffer);

hi_s32 hi_drv_dmx_ram_flush_buffer(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_flush_buffer(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_flush_buffer);

hi_s32 hi_drv_dmx_ram_reset_buffer(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_reset_buffer(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_reset_buffer);

hi_s32 hi_drv_dmx_ram_close_port(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_close_port(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_close_port);

hi_s32 hi_drv_dmx_ram_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_handle, out);
    DMX_NULL_POINTER_GOTO(buf_size, out);
    DMX_NULL_POINTER_GOTO(buf_usr_addr, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_pre_mmap(handle, buf_handle, buf_size, buf_usr_addr);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_pre_mmap);

hi_s32 hi_drv_dmx_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_usr_addr, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_pst_mmap(handle, buf_usr_addr);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_pst_mmap);

hi_s32 hi_drv_dmx_ram_get_portid(hi_handle handle, dmx_port *port)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(port, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_get_port_id(handle, port);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_get_portid);

hi_s32 hi_drv_dmx_ram_get_porthandle(dmx_port port, hi_handle *handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(handle, out);

    if (port < DMX_RAM_PORT_0 || port >= DMX_ANY_RAM_PORT) {
        HI_ERR_DEMUX("invalid ram port(%u).\n", port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_get_port_handle(port, handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_get_porthandle);

hi_s32 hi_drv_dmx_port_get_packet_num(dmx_port port, dmx_port_packet_num *port_stat)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(port_stat, out);

    if (!((port >= DMX_IF_PORT_0 && port < DMX_IF_PORT_MAX) ||
        (port >= DMX_TSI_PORT_0 && port < DMX_TSI_PORT_MAX) ||
        (port >= DMX_RAM_PORT_0 && port < DMX_RAM_PORT_MAX))) {
        HI_ERR_DEMUX("invalid port(%u).\n", port);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_port_get_packet_num(port, port_stat);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_port_get_packet_num);

hi_s32 hi_drv_dmx_ram_get_bufhandle(hi_handle handle,
    hi_mem_handle_t *buf_handle, hi_mem_handle_t *dsc_buf_handle, hi_mem_handle_t *flush_buf_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_handle, out);
    DMX_NULL_POINTER_GOTO(dsc_buf_handle, out);
    DMX_NULL_POINTER_GOTO(flush_buf_handle, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_ram_get_bufhandle(handle, buf_handle, dsc_buf_handle, flush_buf_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_ram_get_bufhandle);

/* rmx begin */
hi_s32 hi_drv_dmx_rmx_create(const dmx_rmx_attrs *attrs, hi_handle *rmx_handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(rmx_handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_rmx_fct_create(attrs, rmx_handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_create);

hi_s32 hi_drv_dmx_rmx_open(hi_handle rmx_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_open(rmx_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_open);

hi_s32 hi_drv_dmx_rmx_get_attrs(hi_handle rmx_handle, dmx_rmx_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_get_attrs(rmx_handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_get_attrs);

hi_s32 hi_drv_dmx_rmx_set_attrs(hi_handle rmx_handle, const dmx_rmx_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx_handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_set_attrs(rmx_handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_set_attrs);

hi_s32 hi_drv_dmx_rmx_get_status(hi_handle rmx_handle, dmx_rmx_status *status)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(status, out);

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx_handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_get_status(rmx_handle, status);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_get_status);

hi_s32 hi_drv_dmx_rmx_add_pump(hi_handle rmx_handle, dmx_rmx_pump_attrs *pump_attrs, hi_handle *rmx_pump_handle,
                               struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(pump_attrs, out);
    DMX_NULL_POINTER_GOTO(rmx_pump_handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_add_pump(rmx_handle, pump_attrs, rmx_pump_handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_add_pump);

hi_s32 hi_drv_dmx_rmx_del_pump(hi_handle rmx_pump_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(rmx_pump_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx_pump_handle(0x%x).\n", rmx_pump_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_del_pump(rmx_pump_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_del_pump);

hi_s32 hi_drv_dmx_rmx_del_all_pump(hi_handle rmx_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_del_all_pump(rmx_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_del_all_pump);

hi_s32 hi_drv_dmx_rmx_get_pump_attrs(hi_handle rmx_pump_handle, dmx_rmx_pump_attrs *pump_attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(pump_attrs, out);

    ret = CHECK_HANDLE(rmx_pump_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx_pump handle(0x%x).\n", rmx_pump_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_pump_get_attrs(rmx_pump_handle, pump_attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_get_pump_attrs);

hi_s32 hi_drv_dmx_rmx_set_pump_attrs(hi_handle rmx_pump_handle, const dmx_rmx_pump_attrs *pump_attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(pump_attrs, out);

    ret = CHECK_HANDLE(rmx_pump_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx_pump handle(0x%x).\n", rmx_pump_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_pump_set_attrs(rmx_pump_handle, pump_attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_set_pump_attrs);

hi_s32 hi_drv_dmx_rmx_close(hi_handle rmx_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_close(rmx_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_close);

hi_s32 hi_drv_dmx_rmx_destroy(hi_handle rmx_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(rmx_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid rmx handle(0x%x).\n", rmx_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rmx_fct_destroy(rmx_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rmx_destroy);

/* band begin */
hi_s32 hi_drv_dmx_band_open(dmx_band band, const dmx_band_attr *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    /* Logic supports 32 bands, but only 16 bands are open to users */
    DMX_FATAL_CON_GOTO(band >= DMX_BAND_MAX, HI_ERR_DMX_INVALID_PARA, out);
    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_band_open(band, attrs, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_open);

hi_s32 hi_drv_dmx_band_ref_inc(dmx_band band, const dmx_band_attr *attrs, hi_handle *handle,
                               struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_band_ref_inc(band, attrs, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_ref_inc);

hi_s32 hi_drv_dmx_band_attach_port(hi_handle handle, dmx_port port)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_FATAL_CON_GOTO(port >= DMX_TSO_PORT_MAX, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_attach_port(handle, port);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_attach_port);

hi_s32 hi_drv_dmx_band_detach_port(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_detach_port(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_detach_port);

hi_s32 hi_drv_dmx_band_get_attrs(hi_handle handle, dmx_band_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_get_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_get_attrs);

hi_s32 hi_drv_dmx_band_set_attrs(hi_handle handle, const dmx_band_attr *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_set_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_set_attrs);

hi_s32 hi_drv_dmx_band_get_status(hi_handle handle, dmx_band_status *status)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(status, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_get_status(handle, status);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_get_status);

hi_s32 hi_drv_dmx_band_close(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_close(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_close);

hi_s32 hi_drv_dmx_band_ref_dec(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_ref_dec(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_ref_dec);

hi_s32 hi_drv_dmx_band_tei_set(hi_handle handle, hi_bool tei)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_band_tei_set(handle, tei);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_band_tei_set);

/* pid_ch begin */
hi_s32 hi_drv_dmx_pid_ch_create(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);
    DMX_FATAL_CON_GOTO(pid > DMX_MAX_PID, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(band_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", band_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_create(band_handle, pid, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_create);

hi_s32 hi_drv_dmx_pid_ch_ref_inc(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);
    DMX_FATAL_CON_GOTO(pid > DMX_MAX_PID, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(band_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("Invalid handle(0x%x).\n", band_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_ref_inc(band_handle, pid, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_ref_inc);

hi_s32 hi_drv_dmx_pid_ch_get_handle(hi_handle band_handle, hi_u32 pid, hi_handle *handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_FATAL_CON_GOTO(pid > DMX_MAX_PID, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(band_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", band_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_get_handle(band_handle, pid, handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_get_handle);

hi_s32 hi_drv_dmx_pid_ch_get_status(hi_handle handle, dmx_pidch_status *status)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(status, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_get_status(handle, status);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_get_status);

hi_s32 hi_drv_dmx_pid_ch_lock_out(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_lock_out(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_lock_out);

hi_s32 hi_drv_dmx_pid_ch_un_lock_out(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_un_lock_out(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_un_lock_out);

hi_s32 hi_drv_dmx_pid_ch_destroy(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_destroy(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_destroy);

hi_s32 hi_drv_dmx_pid_ch_ref_dec(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("Invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pid_ch_ref_dec(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_ref_dec);

hi_s32 hi_drv_dmx_pid_ch_get_free_cnt(hi_u32 *free_cnt)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(free_cnt, out);

    ret = dmx_pid_ch_get_free_cnt(free_cnt);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pid_ch_get_free_cnt);

/* play_fct begin */
hi_s32 hi_drv_dmx_play_create(const dmx_play_attrs *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_play_fct_create(attrs, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_create);

hi_s32 hi_drv_dmx_play_open(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_open(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_open);

hi_s32 hi_drv_dmx_play_get_attrs(hi_handle handle, dmx_play_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_get_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_attrs);

hi_s32 hi_drv_dmx_play_set_attrs(hi_handle handle, const dmx_play_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_set_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_set_attrs);

hi_s32 hi_drv_dmx_play_get_status(hi_handle handle, dmx_play_status *status)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(status, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_get_status(handle, status);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_status);

hi_s32 hi_drv_dmx_play_get_scrambled_flag(hi_handle handle, hi_dmx_scrambled_flag *scramble_flag)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(scramble_flag, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_get_scrambled_flag(handle, scramble_flag);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_scrambled_flag);

hi_s32 hi_drv_dmx_play_get_packet_num(hi_handle handle, dmx_chan_packet_num *chn_stat)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(chn_stat, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_get_packet_num(handle, chn_stat);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_packet_num);

hi_s32 hi_drv_dmx_play_get_pid_ch(hi_handle handle, hi_handle *pid_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(pid_handle, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_get_pid_ch(handle, pid_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_pid_ch);

hi_s32 hi_drv_dmx_play_attach_pid_ch(hi_handle handle, hi_handle pid_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(pid_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", pid_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_attach_pid_ch(handle, pid_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_attach_pid_ch);

hi_s32 hi_drv_dmx_play_detach_pid_ch(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_detach_pid_ch(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_detach_pid_ch);

hi_s32 hi_drv_dmx_play_create_filter(const dmx_filter_attrs *attrs, hi_handle *flt_handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(flt_handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_play_fct_create_filter(attrs, flt_handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_create_filter);


hi_s32 hi_drv_dmx_play_add_filter(hi_handle handle, hi_handle flt_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(flt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid flthandle(0x%x).\n", flt_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_add_filter(handle, flt_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_add_filter);

hi_s32 hi_drv_dmx_play_del_filter(hi_handle handle, hi_handle flt_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(flt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid flthandle(0x%x).\n", flt_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_del_filter(handle, flt_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_del_filter);

hi_s32 hi_drv_dmx_play_del_all_filter(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_del_all_filter(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_del_all_filter);

hi_s32 hi_drv_dmx_play_update_filter(hi_handle flt_handle, const dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(flt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", flt_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_update_filter(flt_handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_update_filter);

hi_s32 hi_drv_dmx_play_get_filter(hi_handle flt_handle, dmx_filter_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(flt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", flt_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_get_filter_attrs(flt_handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_filter);

hi_s32 hi_drv_dmx_play_get_handle_by_filter(hi_handle flt_handle, hi_handle *play_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(play_handle, out);

    ret = CHECK_HANDLE(flt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", flt_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_get_handle_by_filter(flt_handle, play_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_handle_by_filter);

hi_s32 hi_drv_dmx_play_get_free_filter_cnt(hi_u32 *free_flt_cnt)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(free_flt_cnt, out);

    ret = dmx_mgmt_get_free_flt_cnt(free_flt_cnt);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_free_filter_cnt);

hi_s32 hi_drv_dmx_play_destroy_filter(hi_handle flt_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(flt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid flthandle(0x%x).\n", flt_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_destroy_filter(flt_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_destroy_filter);

hi_s32 hi_drv_dmx_play_acquire_buf(hi_handle handle, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
                                   dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(acqed_num, out);
    DMX_NULL_POINTER_GOTO(play_fct_buf, out);
    DMX_FATAL_CON_GOTO(time_out > DMX_MAX_TIME_OUT, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_acquire_buf(handle, acq_num, time_out, acqed_num, play_fct_buf);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_acquire_buf);

hi_s32 hi_drv_dmx_play_release_buf(hi_handle handle, hi_u32 rel_num, dmx_buffer *play_fct_buf)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(play_fct_buf, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_release_buf(handle, rel_num, play_fct_buf);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_release_buf);

hi_s32 hi_drv_dmx_play_reset_buf(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_reset_buf(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_reset_buf);

hi_s32 hi_drv_dmx_play_start_idx(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_start_idx(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_start_idx);

hi_s32 hi_drv_dmx_play_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
    dmx_index_data *index)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(reqed_num, out);
    DMX_NULL_POINTER_GOTO(index, out);
    DMX_FATAL_CON_GOTO(time_out > DMX_MAX_TIME_OUT, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_recv_idx(handle, req_num, time_out, reqed_num, index);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_recv_idx);

hi_s32 hi_drv_dmx_play_stop_idx(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_stop_idx(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_stop_idx);

hi_s32 hi_drv_dmx_play_close(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_close(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_close);

hi_s32 hi_drv_dmx_play_destroy(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_destroy(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_destroy);

hi_s32 hi_drv_dmx_play_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_handle, out);
    DMX_NULL_POINTER_GOTO(buf_size, out);
    DMX_NULL_POINTER_GOTO(buf_usr_addr, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_pre_mmap(handle, buf_handle, buf_size, buf_usr_addr);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_pre_mmap);

hi_s32 hi_drv_dmx_play_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_usr_addr, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_pst_mmap(handle, buf_usr_addr);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_pst_mmap);

hi_s32 hi_drv_dmx_play_get_data_handle(hi_handle *valid_array, hi_u32 *valid_num, hi_u32 watch_num,
    hi_u32 time_out_ms)
{
    hi_u32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(valid_array, out);
    DMX_NULL_POINTER_GOTO(valid_num, out);

    ret = dmx_play_fct_get_data_handle(valid_array, valid_num, watch_num, time_out_ms);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_data_handle);

hi_s32 hi_drv_dmx_play_select_data_handle(hi_handle *watch_array, hi_u32 watch_num, hi_handle *valid_array,
    hi_u32 *valid_num, hi_u32 time_out_ms)
{
    hi_s32 ret;
    hi_u32 i;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(valid_array, out);
    DMX_NULL_POINTER_GOTO(valid_num, out);
    DMX_NULL_POINTER_GOTO(watch_array, out);

    for (i = 0; i < watch_num; i++) {
        ret = CHECK_HANDLE(watch_array[i]);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("invalid handle(0x%x).\n", watch_array[i]);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }
    }

    ret = dmx_play_fct_select_data_handle(watch_array, watch_num, valid_array, valid_num, time_out_ms);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_select_data_handle);

hi_s32 hi_drv_dmx_play_set_eos_flag(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_set_eos_flag(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_set_eos_flag);

hi_s32 hi_drv_dmx_play_get_bufhandle(hi_handle handle, hi_handle *buf_handle, dmx_play_type *type)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();
    DMX_NULL_POINTER_GOTO(buf_handle, out);
    DMX_NULL_POINTER_GOTO(type, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_fct_get_bufhandle(handle, buf_handle, type);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_get_bufhandle);

hi_s32 hi_drv_dmx_play_cc_repeat_set(hi_handle handle, dmx_chan_cc_repeat_mode mode)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_play_cc_repeat_set(handle, mode);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_play_cc_repeat_set);

hi_s32 hi_drv_dmx_play_pusi_set(hi_bool no_pusi)
{
    DMX_GLB_TRACE_ENTER();

    dmx_play_pusi_set(no_pusi);

    DMX_GLB_TRACE_EXIT();

    return HI_SUCCESS;
}
EXPORT_SYMBOL(hi_drv_dmx_play_pusi_set);

/* rec_fct begin */
hi_s32 hi_drv_dmx_rec_create(const dmx_rec_attrs *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_rec_fct_create(attrs, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_create);

hi_s32 hi_drv_dmx_rec_open(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_open(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_open);

hi_s32 hi_drv_dmx_rec_get_attrs(hi_handle handle, dmx_rec_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_get_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_get_attrs);

hi_s32 hi_drv_dmx_rec_set_attrs(hi_handle handle, const dmx_rec_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_set_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_set_attrs);

hi_s32 hi_drv_dmx_rec_set_eos_flag(hi_handle handle, hi_bool eos_flag)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_set_eos_flag(handle, eos_flag);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_set_eos_flag);

hi_s32 hi_drv_dmx_rec_get_status(hi_handle handle, dmx_rec_status *status)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(status, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_get_status(handle, status);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_get_status);

hi_s32 hi_drv_dmx_rec_add_ch(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(ch_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid ch_handle(0x%x).\n", ch_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_add_ch(handle, ch_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_add_ch);

hi_s32 hi_drv_dmx_rec_del_ch(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(ch_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid ch_handle(0x%x).\n", ch_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_del_ch(handle, ch_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_del_ch);

hi_s32 hi_drv_dmx_rec_del_all_ch(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_del_all_ch(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_del_all_ch);

hi_s32 hi_drv_dmx_rec_acquire_buf(hi_handle handle, hi_u32 req_len, hi_u32 time_out, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(rec_fct_buf, out);
    DMX_FATAL_CON_GOTO(time_out > DMX_MAX_TIME_OUT, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_acquire_buf(handle, req_len, time_out, rec_fct_buf);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_acquire_buf);

hi_s32 hi_drv_dmx_rec_release_buf(hi_handle handle, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(rec_fct_buf, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_release_buf(handle, rec_fct_buf);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_release_buf);

hi_s32 hi_drv_dmx_rec_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
                               dmx_index_data *index)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(reqed_num, out);
    DMX_NULL_POINTER_GOTO(index, out);
    DMX_FATAL_CON_GOTO(req_num > DMX_MAX_INDEX_CNT, HI_ERR_DMX_INVALID_PARA, out);
    DMX_FATAL_CON_GOTO(time_out > DMX_MAX_TIME_OUT, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_recv_idx(handle, req_num, time_out, reqed_num, index);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_recv_idx);

hi_s32 hi_drv_dmx_rec_peek_idx_and_buf(hi_handle handle, hi_u32 time_out, hi_u32 *index_num, hi_u32 *rec_data_len)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(index_num, out);
    DMX_NULL_POINTER_GOTO(rec_data_len, out);
    DMX_FATAL_CON_GOTO(time_out > DMX_MAX_TIME_OUT, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = drv_rec_fct_peek_idx_and_buf(handle, time_out, index_num, rec_data_len);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_peek_idx_and_buf);

hi_s32 hi_drv_dmx_rec_close(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_close(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_close);

hi_s32 hi_drv_dmx_rec_destroy(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_destroy(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_destroy);

hi_s32 hi_drv_dmx_rec_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_user_addr)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_handle, out);
    DMX_NULL_POINTER_GOTO(buf_size, out);
    DMX_NULL_POINTER_GOTO(buf_user_addr, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_pre_mmap(handle, buf_handle, buf_size, buf_user_addr);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_pre_mmap);

hi_s32 hi_drv_dmx_rec_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_usr_addr, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_pst_mmap(handle, buf_usr_addr);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_pst_mmap);

hi_s32 hi_drv_dmx_rec_get_bufhandle(hi_handle handle, hi_handle *buf_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(buf_handle, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_fct_get_bufhandle(handle, buf_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_rec_get_bufhandle);

/*********dsc_fct begin************/
hi_s32 hi_drv_dmx_dsc_create(const dmx_dsc_attrs *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);
    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);

    ret = dmx_dsc_fct_create(attrs, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_create);

hi_s32 hi_drv_dmx_dsc_get_attrs(hi_handle handle, dmx_dsc_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_get_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_get_attrs);

hi_s32 hi_drv_dmx_dsc_set_attrs(hi_handle handle, const dmx_dsc_attrs *attrs)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(attrs, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_set_attrs(handle, attrs);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_set_attrs);

hi_s32 hi_drv_dmx_dsc_attach(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(ch_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid ch_handle(0x%x).\n", ch_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_attach(handle, ch_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_attach);

hi_s32 hi_drv_dmx_dsc_detach(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(ch_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid ch_handle(0x%x).\n", ch_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_detach(handle, ch_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_detach);

hi_s32 hi_drv_dmx_dsc_attach_keyslot(hi_handle handle, hi_handle ks_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_KEYSLOT_HANDLE(ks_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid ks_handle(0x%x).\n", ks_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_attach_keyslot(handle, ks_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_attach_keyslot);

hi_s32 hi_drv_dmx_dsc_detach_keyslot(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_detach_keyslot(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_detach_keyslot);

hi_s32 hi_drv_dmx_dsc_get_keyslot_handle(hi_handle handle, hi_handle *ks_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_get_keyslot_handle(handle, ks_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_get_keyslot_handle);

hi_s32 hi_drv_dmx_dsc_set_even_key(hi_handle handle, const hi_u8 *key, hi_u32 len)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(key, out);
    DMX_FATAL_CON_GOTO(len > DMX_MAX_KEY_LEN, HI_ERR_DMX_INVALID_PARA, out);
    DMX_FATAL_CON_GOTO(len < DMX_MIN_KEY_LEN, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_set_key(handle, DMX_DSC_KEY_EVEN, key, len);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_set_even_key);

hi_s32 hi_drv_dmx_dsc_set_odd_key(hi_handle handle, const hi_u8 *key, hi_u32 len)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(key, out);
    DMX_FATAL_CON_GOTO(len > DMX_MAX_KEY_LEN, HI_ERR_DMX_INVALID_PARA, out);
    DMX_FATAL_CON_GOTO(len < DMX_MIN_KEY_LEN, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_set_key(handle, DMX_DSC_KEY_ODD, key, len);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_set_odd_key);

hi_s32 hi_drv_dmx_dsc_set_sys_key(hi_handle handle, const hi_u8 *key, hi_u32 len)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(key, out);
    DMX_FATAL_CON_GOTO(len != DMX_SYS_KEY_LEN, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_set_key(handle, DMX_DSC_KEY_SYS, key, len);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_set_sys_key);

hi_s32 hi_drv_dmx_dsc_set_even_iv(hi_handle handle, const hi_u8 *iv, hi_u32 len)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(iv, out);
    DMX_FATAL_CON_GOTO(len < DMX_MIN_KEY_LEN || len > DMX_MAX_KEY_LEN, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_set_iv(handle, DMX_DSC_KEY_EVEN, iv, len);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_set_even_iv);

hi_s32 hi_drv_dmx_dsc_set_odd_iv(hi_handle handle, const hi_u8 *iv, hi_u32 len)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(iv, out);
    DMX_FATAL_CON_GOTO(len < DMX_MIN_KEY_LEN || len > DMX_MAX_KEY_LEN, HI_ERR_DMX_INVALID_PARA, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_set_iv(handle, DMX_DSC_KEY_ODD, iv, len);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_set_odd_iv);

hi_s32 hi_drv_dmx_dsc_destroy(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_fct_destroy(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_destroy);

hi_s32 hi_drv_dmx_dsc_get_dsc_key_handle(hi_handle pid_ch_handle, hi_handle *dsc_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(dsc_handle, out);

    ret = CHECK_HANDLE(pid_ch_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", pid_ch_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_dsc_get_dsc_key_handle(pid_ch_handle, dsc_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_get_dsc_key_handle);

hi_s32 hi_drv_dmx_dsc_get_free_cnt(hi_u32 *free_cnt)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(free_cnt, out);

    ret = dmx_dsc_get_free_cnt(free_cnt);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_dsc_get_free_cnt);

/*********pcr_fct begin************/
hi_s32 hi_drv_dmx_pcr_create(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(band_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", band_handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    DMX_NULL_POINTER_GOTO(handle, out);
    DMX_NULL_POINTER_GOTO(session, out);
    DMX_FATAL_CON_GOTO(pid > DMX_MAX_PID, HI_ERR_DMX_INVALID_PARA, out);

    ret = dmx_pcr_fct_create(band_handle, pid, handle, session);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pcr_create);

hi_s32 hi_drv_dmx_pcr_attach(hi_handle handle, hi_handle sync_handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    if (sync_handle == HI_INVALID_HANDLE) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pcr_fct_attach(handle, sync_handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pcr_attach);

hi_s32 hi_drv_dmx_pcr_detach(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pcr_fct_detach(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pcr_detach);

hi_s32 hi_drv_dmx_pcr_get_status(hi_handle handle, dmx_pcr_status *status)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(status, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pcr_fct_get_status(handle, status);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pcr_get_status);

hi_s32 hi_drv_dmx_pcr_get_pcr_scr(hi_handle handle, hi_u64 *pcr_ms, hi_u64 *scr_ms)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    DMX_NULL_POINTER_GOTO(pcr_ms, out);
    DMX_NULL_POINTER_GOTO(scr_ms, out);

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pcr_fct_get_pcr_scr(handle, pcr_ms, scr_ms);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pcr_get_pcr_scr);

hi_s32 hi_drv_dmx_pcr_destroy(hi_handle handle)
{
    hi_s32 ret;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_pcr_fct_destroy(handle);

    DMX_GLB_TRACE_EXIT();

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_pcr_destroy);

/* callback begin */
hi_s32 hi_drv_dmx_set_callback(hi_handle handle, hi_handle user_handle, drv_dmx_callback cb)
{
    hi_s32 ret;
    struct dmx_r_base *obj = HI_NULL;

    DMX_GLB_TRACE_ENTER();

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_r_get(handle, &obj);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get obj failed, ret 0x%08x.\n", ret);
        goto out;
    }

    if (IS_PLAYFCT(obj)) {
        ret = dmx_play_fct_set_callback(handle, user_handle, cb);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set play fct callback info failed, ret 0x%08x.\n", ret);
            goto out1;
        }
    } else if (IS_PCR_FCT(obj)) {
        ret = dmx_pcr_fct_set_callback(handle, user_handle, cb);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("set pcr fct callback info failed, ret 0x%08x.\n", ret);
            goto out1;
        }
    } else {
        HI_ERR_DEMUX("unknown fct type, handle 0x%08x.\n", handle);
        ret = HI_FAILURE;
    }

    DMX_GLB_TRACE_EXIT();

out1:
    dmx_r_put(obj);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_dmx_set_callback);

hi_s32 hi_drv_dmx_suspend(hi_void)
{
    return dmx_mgmt_suspend();
}
EXPORT_SYMBOL(hi_drv_dmx_suspend);

hi_s32 hi_drv_dmx_resume(hi_void)
{
    return dmx_mgmt_resume();
}
EXPORT_SYMBOL(hi_drv_dmx_resume);


