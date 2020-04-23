/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio export interface definition.
* Author: guoqingbo
* Create: 2016-08-12
*/

#include "hi_type.h"

#include "hi_drv_tsio.h"
#include "drv_tsio_utils.h"
#include "drv_tsio_func.h"

hi_s32 hi_drv_tsio_get_capability(tsio_capability *cap)
{
    return tsio_mgmt_get_cap(cap);
}
EXPORT_SYMBOL(hi_drv_tsio_get_capability);

hi_s32 hi_drv_tsio_get_out_of_sync_cnt(hi_u32 *cnt)
{
    return tsio_mgmt_get_out_of_sync_cnt(cnt);
}
EXPORT_SYMBOL(hi_drv_tsio_get_out_of_sync_cnt);

hi_s32 hi_drv_tsio_create_session(struct tsio_session **new_session)
{
    hi_s32 ret;

    if (new_session == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_session_create(new_session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_create_session);

hi_s32 hi_drv_tsio_destroy_session(struct tsio_session *session)
{
    hi_s32 ret;

    if (session == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_session_destroy(session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_destroy_session);

hi_s32 hi_drv_tsio_get_config(tsio_config *config)
{
    return tsio_mgmt_get_config(config);
}
EXPORT_SYMBOL(hi_drv_tsio_get_config);

hi_s32 hi_drv_tsio_set_config(const tsio_config *config)
{
    return tsio_mgmt_set_config(config);
}
EXPORT_SYMBOL(hi_drv_tsio_set_config);

hi_s32 hi_drv_tsio_tsi_open_port(tsio_port port, const tsio_tsi_port_attrs *attrs, hi_handle *handle,
                                 struct tsio_session *session)
{
    hi_s32 ret;

    if ((attrs == HI_NULL) || (handle == HI_NULL) || (session == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_NULL_PTR;
        goto out;
    }

    ret = tsio_tsi_open_port(port, attrs, handle, session);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_tsi_open_port);

hi_s32 hi_drv_tsio_tsi_get_port_attrs(hi_handle handle, tsio_tsi_port_attrs *attrs)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (attrs == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_tsi_get_port_attrs(handle, attrs);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_tsi_get_port_attrs);

hi_s32 hi_drv_tsio_tsi_set_port_attrs(hi_handle handle, const tsio_tsi_port_attrs *attrs)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (attrs == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_tsi_set_port_attrs(handle, attrs);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_tsi_set_port_attrs);

hi_s32 hi_drv_tsio_tsi_get_port_status(hi_handle handle, tsio_tsi_port_status *status)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (status == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_tsi_get_port_status(handle, status);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_tsi_get_port_status);

hi_s32 hi_drv_tsio_tsi_close_port(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_tsi_close_port(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_tsi_close_port);

hi_s32 hi_drv_tsio_ram_open_port(tsio_port port, const tsio_ram_port_attrs_ex *attrs, hi_handle *handle,
                                 struct tsio_session *session)
{
    hi_s32 ret;

    if ((attrs == HI_NULL) || (handle == HI_NULL) || (session == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_NULL_PTR;
        goto out;
    }

    ret = tsio_ram_open_port(port, attrs, handle, session);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_open_port);

hi_s32 hi_drv_tsio_ram_get_port_attrs(hi_handle handle, tsio_ram_port_attrs *attrs)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (attrs == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_get_port_attrs(handle, attrs);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_get_port_attrs);

hi_s32 hi_drv_tsio_ram_set_port_attrs(hi_handle handle, const tsio_ram_port_attrs_ex *attrs)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (attrs == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_set_port_attrs(handle, attrs);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_set_port_attrs);

hi_s32 hi_drv_tsio_ram_get_port_status(hi_handle handle, tsio_ram_port_status *status)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (status == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_get_port_status(handle, status);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_get_port_status);

hi_s32 hi_drv_tsio_ram_begin_bulk_seg(hi_handle handle, tsio_key_type key, const hi_u8 *IV)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (IV == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_begin_bulk_seg(handle, key, IV);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_begin_bulk_seg);

hi_s32 hi_drv_tsio_ram_end_bulk_seg(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_end_bulk_seg(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_end_bulk_seg);

/* time_out:ms */
hi_s32 hi_drv_tsio_ram_get_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if ((req_len == 0) || (buf == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_get_buffer(handle, req_len, buf, time_out);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_get_buffer);

hi_s32 hi_drv_tsio_ram_push_buffer(hi_handle handle, tsio_buffer *buf)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (buf == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_push_buffer(handle, buf);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_push_buffer);

hi_s32 hi_drv_tsio_ram_put_buffer(hi_handle handle, tsio_buffer *buf)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (buf == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_put_buffer(handle, buf);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_put_buffer);

hi_s32 hi_drv_tsio_ram_reset_buffer(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_reset_buffer(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_reset_buffer);

hi_s32 hi_drv_tsio_ram_close_port(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_close_port(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_close_port);

hi_s32 hi_drv_tsio_ram_pre_mmap(hi_handle handle, hi_handle *buf_handle,
    hi_u32 *buf_size, hi_void **usr_addr)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (buf_handle == HI_NULL || buf_size == HI_NULL || usr_addr == HI_NULL) {
        HI_ERR_TSIO("null pointer, buf_handle(%p), buf_size(%p), usr_addr(%p).\n", buf_handle, buf_size, usr_addr);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_pre_mmap(handle, buf_handle, buf_size, usr_addr);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_pre_mmap);

hi_s32 hi_drv_tsio_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ram_pst_mmap(handle, buf_usr_addr);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ram_pst_mmap);

hi_s32 hi_drv_tsio_pid_create_channel(hi_handle pt_handle, hi_u32 pid, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(pt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", pt_handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (pid > INVALID_PID) {
        HI_ERR_TSIO("invalid pid(%d).\n", pid);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if ((handle == HI_NULL) || (session == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_pid_create_channel(pt_handle, pid, handle, session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_pid_create_channel);

hi_s32 hi_drv_tsio_pid_get_channel_status(hi_handle handle, tsio_pid_channel_status *status)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (status == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_pid_get_channel_status(handle, status);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_pid_get_channel_status);

hi_s32 hi_drv_tsio_pid_destroy_channel(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_pid_destroy_channel(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_pid_destroy_channel);

hi_s32 hi_drv_tsio_raw_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(pt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", pt_handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if ((handle == HI_NULL) || (session == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_raw_create_channel(pt_handle, handle, session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_raw_create_channel);

hi_s32 hi_drv_tsio_raw_destroy_channel(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_raw_destroy_channel(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_raw_destroy_channel);

hi_s32 hi_drv_tsio_sp_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(pt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", pt_handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if ((handle == HI_NULL) || (session == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_sp_create_channel(pt_handle, handle, session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_sp_create_channel);

hi_s32 hi_drv_tsio_sp_destroy_channel(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_sp_destroy_channel(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_sp_destroy_channel);

hi_s32 hi_drv_tsio_ivr_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(pt_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", pt_handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if ((handle == HI_NULL) || (session == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ivr_create_channel(pt_handle, handle, session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ivr_create_channel);

hi_s32 hi_drv_tsio_ivr_destroy_channel(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_ivr_destroy_channel(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_ivr_destroy_channel);

hi_s32 hi_drv_tsio_se_create(tsio_sid sid, const tsio_secure_engine_attrs *attrs, hi_handle *handle,
                             struct tsio_session *session)
{
    hi_s32 ret;

    if ((attrs == HI_NULL) || (handle == HI_NULL) || (session == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_create(sid, attrs, handle, session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_create);

hi_s32 hi_drv_tsio_se_open(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_open(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_open);

hi_s32 hi_drv_tsio_se_get_attrs(hi_handle handle, tsio_secure_engine_attrs *attrs)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (attrs == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_get_attrs(handle, attrs);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_get_attrs);

hi_s32 hi_drv_tsio_se_set_attrs(hi_handle handle, const tsio_secure_engine_attrs *attrs)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (attrs == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_set_attrs(handle, attrs);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_set_attrs);

hi_s32 hi_drv_tsio_se_get_status(hi_handle handle, tsio_secure_engine_status *status)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (status == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_get_status(handle, status);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_get_status);

hi_s32 hi_drv_tsio_se_add_channel(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(ch_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", ch_handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_add_channel(handle, ch_handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_add_channel);

hi_s32 hi_drv_tsio_se_del_channel(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = CHECK_HANDLE(ch_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", ch_handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_del_channel(handle, ch_handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_del_channel);

hi_s32 hi_drv_tsio_se_del_all_channels(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_del_all_channels(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_del_all_channels);

hi_s32 hi_drv_tsio_se_acquire_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if ((req_len == 0) || (buf == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_acquire_buffer(handle, req_len, buf, time_out);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_acquire_buffer);

hi_s32 hi_drv_tsio_se_release_buffer(hi_handle handle, const tsio_buffer *buf)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (buf == HI_NULL) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_release_buffer(handle, buf);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_release_buffer);

hi_s32 hi_drv_tsio_se_close(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_close(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_close);

hi_s32 hi_drv_tsio_se_destroy(hi_handle handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_destroy(handle);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_destroy);

hi_s32 hi_drv_tsio_se_pre_mmap(hi_handle handle, hi_handle *buf_handle,
    hi_u32 *buf_size, hi_void **usr_addr)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if (buf_handle == HI_NULL || buf_size == HI_NULL || usr_addr == HI_NULL) {
        HI_ERR_TSIO("null pointer, buf_handle(%p), buf_size(%p), usr_addr(%p).\n", buf_handle, buf_size, usr_addr);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_pre_mmap(handle, buf_handle, buf_size, usr_addr);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_pre_mmap);

hi_s32 hi_drv_tsio_se_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_se_pst_mmap(handle, buf_usr_addr);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_se_pst_mmap);

hi_s32 hi_drv_tsio_cc_send(const hi_uchar *cmd, hi_u32 cmd_len, hi_uchar *resp, hi_u32 *resp_len, hi_u32 time_out)
{
    hi_s32 ret;

    if ((cmd == HI_NULL) || (cmd_len == 0) || (resp == HI_NULL) || (resp_len == HI_NULL)) {
        HI_ERR_TSIO("invalid parameter.\n");
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    if ((cmd_len < MIN_CC_CMD_LEN || *resp_len < MIN_CC_CMD_LEN) ||
            (cmd_len > MAX_CC_CMD_LEN || *resp_len > MAX_CC_CMD_LEN)) {
        HI_ERR_TSIO("cmd buffer len(%d) or resp buffer len(%d) is invalid.\n", cmd_len, *resp_len);
        ret = HI_ERR_TSIO_INVALID_PARA;
        goto out;
    }

    ret = tsio_cc_send(cmd, cmd_len, resp, resp_len, time_out);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsio_cc_send);

hi_s32 hi_drv_tsio_suspend(hi_void)
{
    return tsio_mgmt_suspend();
}
EXPORT_SYMBOL(hi_drv_tsio_suspend);

hi_s32 hi_drv_tsio_resume(hi_void)
{
    return tsio_mgmt_resume();
}
EXPORT_SYMBOL(hi_drv_tsio_resume);

