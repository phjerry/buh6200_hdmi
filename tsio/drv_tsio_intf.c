/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio interface of module.
* Author: guoqingbo
* Create: 2016-08-12
*/
#include "linux/hisilicon/securec.h"
#include "hi_drv_sys.h"
#include "hi_drv_dev.h"
#include "hi_drv_mem.h"
#include "hi_type.h"
#include "hi_drv_tsio.h"
#include "drv_tsio_ioctl.h"
#include "drv_tsio_utils.h"
#include "drv_tsio_func.h"

static hi_s32 tsio_open(hi_void *private_data)
{
    hi_s32 ret;
    struct tsio_session *session = HI_NULL;

    ret = hi_drv_tsio_create_session(&session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    *((struct tsio_session **)private_data) = session;

    return ret;

out:
    return ret;
}

static hi_s32 tsio_release(hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    if (*((struct tsio_session **)private_data) != HI_NULL) {
        struct tsio_session *session = *((struct tsio_session **)private_data);
        ret = hi_drv_tsio_destroy_session(session);
        if (ret == HI_SUCCESS) {
            *((struct tsio_session **)private_data) = HI_NULL;
        }
    }

    return ret;
}

static hi_s32 tsio_ioctl_gbl_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSIO_IOCTL_GBL_GETCFG: {
                tsio_config_info *info = (tsio_config_info *)arg;

                ret = hi_drv_tsio_get_config(&info->config);

                break;
            }

        case TSIO_IOCTL_GBL_SETCFG: {
                tsio_config_info *info = (tsio_config_info *)arg;

                ret = hi_drv_tsio_set_config(&info->config);

                break;
            }

        case TSIO_IOCTL_GBL_GETCAP: {
                tsio_capability *info = (tsio_capability *)arg;

                ret = hi_drv_tsio_get_capability(info);

                break;
            }

        case TSIO_IOCTL_GBL_GETOUTOFSYNCCNT: {
                hi_u32 *count = (hi_u32 *)arg;

                ret = hi_drv_tsio_get_out_of_sync_cnt(count);

                break;
            }

        default: {
                HI_ERR_TSIO("unknown cmd: 0x%x\n", cmd);
            }
    }

    return ret;
}

static hi_s32 tsio_ioctl_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSIO_IOCTL_TSI_OPENPORT: {
                tsio_open_tsi_port_info *info = (tsio_open_tsi_port_info *)arg;

                ret = hi_drv_tsio_tsi_open_port(info->port, &info->attrs,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_TSI_GETPORTATTRS: {
                tsio_get_tsi_port_attr_info *info = (tsio_get_tsi_port_attr_info *)arg;

                ret = hi_drv_tsio_tsi_get_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_TSI_SETPORTATTRS: {
                tsio_set_tsi_port_attr_info *info = (tsio_set_tsi_port_attr_info *)arg;

                ret = hi_drv_tsio_tsi_set_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_TSI_GETPORTSTATUS: {
                tsio_get_tsi_port_status_info *info = (tsio_get_tsi_port_status_info *)arg;

                ret = hi_drv_tsio_tsi_get_port_status(info->handle, &info->status);

                break;
            }

        case TSIO_IOCTL_TSI_CLOSEPORT: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_tsi_close_port(*handle);

                break;
            }

        case TSIO_IOCTL_RAM_OPENPORT: {
                tsio_open_ram_port_info *info = (tsio_open_ram_port_info *)arg;

                ret = hi_drv_tsio_ram_open_port(info->port, &info->attrs,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_RAM_GETPORTATTRS: {
                tsio_get_ram_port_attr_info *info = (tsio_get_ram_port_attr_info *)arg;

                ret = hi_drv_tsio_ram_get_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_RAM_SETPORTATTRS: {
                tsio_set_ram_port_attr_info *info = (tsio_set_ram_port_attr_info *)arg;

                ret = hi_drv_tsio_ram_set_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_RAM_GETPORTSTATUS: {
                tsio_get_ram_port_status_info *info = (tsio_get_ram_port_status_info *)arg;

                ret = hi_drv_tsio_ram_get_port_status(info->handle, &info->status);

                break;
            }

        case TSIO_IOCTL_RAM_BEGINBULKSEG: {
                tsio_begin_bulk_seg_info *info = (tsio_begin_bulk_seg_info *)arg;

                ret = hi_drv_tsio_ram_begin_bulk_seg(info->handle, info->key, info->iv);

                break;
            }

        case TSIO_IOCTL_RAM_ENDBULKSEG: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_ram_end_bulk_seg(*handle);

                break;
            }

        case TSIO_IOCTL_RAM_GETBUFFER: {
                tsio_get_ram_buffer_info *info = (tsio_get_ram_buffer_info *)arg;

                ret = hi_drv_tsio_ram_get_buffer(info->handle, info->req_len, &info->buf, info->time_out);

                break;
            }

        case TSIO_IOCTL_RAM_PUSHBUFFER: {
                tsio_push_ram_buffer_info *info = (tsio_push_ram_buffer_info *)arg;

                ret = hi_drv_tsio_ram_push_buffer(info->handle, &info->buf);

                break;
            }

        case TSIO_IOCTL_RAM_PUTBUFFER: {
                tsio_put_ram_buffer_info *info = (tsio_put_ram_buffer_info *)arg;

                ret = hi_drv_tsio_ram_put_buffer(info->handle, &info->buf);

                break;
            }

        case TSIO_IOCTL_RAM_RESETBUFFER: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_ram_reset_buffer(*handle);

                break;
            }

        case TSIO_IOCTL_RAM_CLOSEPORT: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_ram_close_port(*handle);

                break;
            }

        case TSIO_IOCTL_RAM_PREMMAP: {
                tsio_pre_mmap_info *info = (tsio_pre_mmap_info *)arg;

                ret = hi_drv_tsio_ram_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &info->usr_addr);

                break;
            }

        case TSIO_IOCTL_RAM_PSTMMAP: {
                tsio_pst_mmap_info *info = (tsio_pst_mmap_info *)arg;

                ret = hi_drv_tsio_ram_pst_mmap(info->handle, info->usr_addr);

                break;
            }

        default: {
                HI_ERR_TSIO("unknown cmd: 0x%x\n", cmd);
            }
    }

    return ret;
}

static hi_s32 tsio_ioctl_chan_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSIO_IOCTL_PID_CREATECHANNEL: {
                tsio_create_pid_channel_info *info = (tsio_create_pid_channel_info *)arg;

                ret = hi_drv_tsio_pid_create_channel(info->pt_handle, info->pid, &info->handle,
                                                     *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_PID_GETCHANNELSTATUS: {
                tsio_get_pid_channel_status_info *info = (tsio_get_pid_channel_status_info *)arg;

                ret = hi_drv_tsio_pid_get_channel_status(info->handle, &info->status);

                break;
            }

        case TSIO_IOCTL_PID_DESTROYCHANNEL: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_pid_destroy_channel(*handle);

                break;
            }

        case TSIO_IOCTL_RAW_CREATECHANNEL: {
                tsio_create_raw_channel_info *info = (tsio_create_raw_channel_info *)arg;

                ret = hi_drv_tsio_raw_create_channel(info->pt_handle,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_RAW_DESTROYCHANNEL: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_raw_destroy_channel(*handle);

                break;
            }

        case TSIO_IOCTL_SP_CREATECHANNEL: {
                tsio_create_sp_channel_info *info = (tsio_create_sp_channel_info *)arg;

                ret = hi_drv_tsio_sp_create_channel(info->pt_handle,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_SP_DESTROYCHANNEL: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_sp_destroy_channel(*handle);

                break;
            }

        case TSIO_IOCTL_IVR_CREATECHANNEL: {
                tsio_create_ivr_channel_info *info = (tsio_create_ivr_channel_info *)arg;

                ret = hi_drv_tsio_ivr_create_channel(info->pt_handle,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_IVR_DESTROYCHANNEL: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_ivr_destroy_channel(*handle);

                break;
            }

        default: {
                HI_ERR_TSIO("unknown cmd: 0x%x\n", cmd);
            }
    }

    return ret;
}

static hi_s32 tsio_ioctl_se_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSIO_IOCTL_SE_CREATE: {
                tsio_create_se_info *info = (tsio_create_se_info *)arg;

                ret = hi_drv_tsio_se_create(info->sid, &info->attrs,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_SE_OPEN: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_open(*handle);

                break;
            }

        case TSIO_IOCTL_SE_GETATTRS: {
                tsio_get_se_attr_info *info = (tsio_get_se_attr_info *)arg;

                ret = hi_drv_tsio_se_get_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_SE_SETATTRS: {
                tsio_set_se_attr_info *info = (tsio_set_se_attr_info *)arg;

                ret = hi_drv_tsio_se_set_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_SE_GETSTATUS: {
                tsio_get_se_status_info *info = (tsio_get_se_status_info *)arg;

                ret = hi_drv_tsio_se_get_status(info->handle, &info->status);

                break;
            }

        case TSIO_IOCTL_SE_ADDCHANNEL: {
                tsio_add_se_channel_info *info = (tsio_add_se_channel_info *)arg;

                ret = hi_drv_tsio_se_add_channel(info->handle, info->ch_handle);

                break;
            }

        case TSIO_IOCTL_SE_DELCHANNEL: {
                tsio_del_se_channel_info *info = (tsio_del_se_channel_info *)arg;

                ret = hi_drv_tsio_se_del_channel(info->handle, info->ch_handle);

                break;
            }

        case TSIO_IOCTL_SE_DELALLCHANNELS: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_del_all_channels(*handle);

                break;
            }

        case TSIO_IOCTL_SE_ACQBUFFER: {
                tsio_acq_se_buffer_info *info = (tsio_acq_se_buffer_info *)arg;

                ret = hi_drv_tsio_se_acquire_buffer(info->handle, info->req_len, &info->buf, info->time_out);

                break;
            }

        case TSIO_IOCTL_SE_RELBUFFER: {
                tsio_rel_se_buffer_info *info = (tsio_rel_se_buffer_info *)arg;

                ret = hi_drv_tsio_se_release_buffer(info->handle, &info->buf);

                break;
            }

        case TSIO_IOCTL_SE_CLOSE: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_close(*handle);

                break;
            }

        case TSIO_IOCTL_SE_DESTROY: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_destroy(*handle);

                break;
            }

        case TSIO_IOCTL_SE_PREMMAP: {
                tsio_pre_mmap_info *info = (tsio_pre_mmap_info *)arg;

                ret = hi_drv_tsio_se_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &info->usr_addr);

                break;
            }

        case TSIO_IOCTL_SE_PSTMMAP: {
                tsio_pst_mmap_info *info = (tsio_pst_mmap_info *)arg;

                ret = hi_drv_tsio_se_pst_mmap(info->handle, info->usr_addr);

                break;
            }

        default: {
                HI_ERR_TSIO("unknown cmd: 0x%x\n", cmd);
            }
    }

    return ret;
}

static hi_s32 tsio_ioctl_cc_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSIO_IOCTL_CC_SEND: {
                tsio_send_ccmd_info *info = (tsio_send_ccmd_info *)arg;

                ret = hi_drv_tsio_cc_send(info->cmd, info->cmd_len, info->resp, &info->resp_len, info->time_out);

                break;
            }

        default: {
                HI_ERR_TSIO("unknown cmd: 0x%x\n", cmd);
            }
    }

    return ret;
}

#ifdef CONFIG_COMPAT
static hi_s32 tsio_compat_ioctl_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSIO_IOCTL_TSI_OPENPORT: {
                tsio_open_tsi_port_info *info = (tsio_open_tsi_port_info *)arg;

                ret = hi_drv_tsio_tsi_open_port(info->port, &info->attrs,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_TSI_GETPORTATTRS: {
                tsio_get_tsi_port_attr_info *info = (tsio_get_tsi_port_attr_info *)arg;

                ret = hi_drv_tsio_tsi_get_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_TSI_SETPORTATTRS: {
                tsio_set_tsi_port_attr_info *info = (tsio_set_tsi_port_attr_info *)arg;

                ret = hi_drv_tsio_tsi_set_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_TSI_GETPORTSTATUS: {
                tsio_get_tsi_port_status_info *info = (tsio_get_tsi_port_status_info *)arg;

                ret = hi_drv_tsio_tsi_get_port_status(info->handle, &info->status);

                break;
            }

        case TSIO_IOCTL_TSI_CLOSEPORT: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_tsi_close_port(*handle);

                break;
            }

        case TSIO_IOCTL_RAM_OPENPORT: {
                tsio_open_ram_port_info *info = (tsio_open_ram_port_info *)arg;

                ret = hi_drv_tsio_ram_open_port(info->port, &info->attrs,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_RAM_GETPORTATTRS: {
                tsio_get_ram_port_attr_info *info = (tsio_get_ram_port_attr_info *)arg;

                ret = hi_drv_tsio_ram_get_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_RAM_SETPORTATTRS: {
                tsio_set_ram_port_attr_info *info = (tsio_set_ram_port_attr_info *)arg;

                ret = hi_drv_tsio_ram_set_port_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_RAM_GETPORTSTATUS: {
                tsio_get_ram_port_status_info *info = (tsio_get_ram_port_status_info *)arg;

                ret = hi_drv_tsio_ram_get_port_status(info->handle, &info->status);

                break;
            }

        case TSIO_IOCTL_RAM_BEGINBULKSEG: {
                tsio_begin_bulk_seg_info *info = (tsio_begin_bulk_seg_info *)arg;

                ret = hi_drv_tsio_ram_begin_bulk_seg(info->handle, info->key, info->iv);

                break;
            }

        case TSIO_IOCTL_RAM_ENDBULKSEG: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_ram_end_bulk_seg(*handle);

                break;
            }

        case TSIO_COMPAT_IOCTL_RAM_GETBUFFER: {
                tsio_compat_get_ram_buffer_info *info = (tsio_compat_get_ram_buffer_info *)arg;
                tsio_buffer buf = {0};

                ret = hi_drv_tsio_ram_get_buffer(info->handle, info->req_len, &buf, info->time_out);
                if (ret == HI_SUCCESS) {
                    info->buf.data = ptr_to_compat(buf.data);
                    info->buf.length = buf.length;
                    info->buf.buf_handle = buf.buf_handle;
                    info->buf.buf_offset = buf.buf_offset;
                }

                break;
            }

        case TSIO_COMPAT_IOCTL_RAM_PUSHBUFFER: {
                tsio_compat_push_ram_buffer_info *info = (tsio_compat_push_ram_buffer_info *)arg;
                tsio_buffer buf = {
                    .data = compat_ptr(info->buf.data),
                    .length = info->buf.length,
                    .buf_handle = info->buf.buf_handle,
                    .buf_offset = info->buf.buf_offset,
                };

                ret = hi_drv_tsio_ram_push_buffer(info->handle, &buf);

                break;
            }

        case TSIO_COMPAT_IOCTL_RAM_PUTBUFFER: {
                tsio_compat_put_ram_buffer_info *info = (tsio_compat_put_ram_buffer_info *)arg;
                tsio_buffer buf = {
                    .data = compat_ptr(info->buf.data),
                    .length = info->buf.length,
                    .buf_handle = info->buf.buf_handle,
                    .buf_offset = info->buf.buf_offset,
                };

                ret = hi_drv_tsio_ram_put_buffer(info->handle, &buf);

                break;
            }

        case TSIO_IOCTL_RAM_RESETBUFFER: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_ram_reset_buffer(*handle);

                break;
            }

        case TSIO_IOCTL_RAM_CLOSEPORT: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_ram_close_port(*handle);

                break;
            }

        case TSIO_COMPAT_IOCTL_RAM_PREMMAP: {
                tsio_compat_pre_mmap_info *info = (tsio_compat_pre_mmap_info *)arg;
                hi_void *usr_addr = HI_NULL;

                ret = hi_drv_tsio_ram_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &usr_addr);
                info->usr_addr = ptr_to_compat(usr_addr);

                break;
            }

        case TSIO_COMPAT_IOCTL_RAM_PSTMMAP: {
                tsio_compat_pst_mmap_info *info = (tsio_compat_pst_mmap_info *)arg;
                hi_u8 *usr_addr = compat_ptr(info->usr_addr);

                ret = hi_drv_tsio_ram_pst_mmap(info->handle, usr_addr);

                break;
            }

        default: {
                HI_ERR_TSIO("unknown cmd: 0x%x\n", cmd);
            }
    }

    return ret;
}

static hi_s32 tsio_compat_ioctl_se_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSIO_IOCTL_SE_CREATE: {
                tsio_create_se_info *info = (tsio_create_se_info *)arg;

                ret = hi_drv_tsio_se_create(info->sid, &info->attrs,
                    &info->handle, *((struct tsio_session **)private_data));

                break;
            }

        case TSIO_IOCTL_SE_OPEN: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_open(*handle);

                break;
            }

        case TSIO_IOCTL_SE_GETATTRS: {
                tsio_get_se_attr_info *info = (tsio_get_se_attr_info *)arg;

                ret = hi_drv_tsio_se_get_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_SE_SETATTRS: {
                tsio_set_se_attr_info *info = (tsio_set_se_attr_info *)arg;

                ret = hi_drv_tsio_se_set_attrs(info->handle, &info->attrs);

                break;
            }

        case TSIO_IOCTL_SE_GETSTATUS: {
                tsio_get_se_status_info *info = (tsio_get_se_status_info *)arg;

                ret = hi_drv_tsio_se_get_status(info->handle, &info->status);

                break;
            }

        case TSIO_IOCTL_SE_ADDCHANNEL: {
                tsio_add_se_channel_info *info = (tsio_add_se_channel_info *)arg;

                ret = hi_drv_tsio_se_add_channel(info->handle, info->ch_handle);

                break;
            }

        case TSIO_IOCTL_SE_DELCHANNEL: {
                tsio_del_se_channel_info *info = (tsio_del_se_channel_info *)arg;

                ret = hi_drv_tsio_se_del_channel(info->handle, info->ch_handle);

                break;
            }

        case TSIO_IOCTL_SE_DELALLCHANNELS: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_del_all_channels(*handle);

                break;
            }

        case TSIO_COMPAT_IOCTL_SE_ACQBUFFER: {
                tsio_compat_acq_se_buffer_info *info = (tsio_compat_acq_se_buffer_info *)arg;
                tsio_buffer buf = {0};

                ret = hi_drv_tsio_se_acquire_buffer(info->handle, info->req_len, &buf, info->time_out);
                if (ret == HI_SUCCESS) {
                    info->buf.data = ptr_to_compat(buf.data);
                    info->buf.length = buf.length;
                    info->buf.buf_handle = buf.buf_handle;
                    info->buf.buf_offset = buf.buf_offset;
                }

                break;
            }

        case TSIO_COMPAT_IOCTL_SE_RELBUFFER: {
                tsio_compat_rel_se_buffer_info *info = (tsio_compat_rel_se_buffer_info *)arg;
                tsio_buffer buf = {
                    .data = compat_ptr(info->buf.data),
                    .length = info->buf.length,
                    .buf_handle = info->buf.buf_handle,
                    .buf_offset = info->buf.buf_offset,
                };

                ret = hi_drv_tsio_se_release_buffer(info->handle, &buf);

                break;
            }

        case TSIO_IOCTL_SE_CLOSE: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_close(*handle);

                break;
            }

        case TSIO_IOCTL_SE_DESTROY: {
                hi_handle *handle = (hi_handle *)arg;

                ret = hi_drv_tsio_se_destroy(*handle);

                break;
            }

        case TSIO_COMPAT_IOCTL_SE_PREMMAP: {
                tsio_compat_pre_mmap_info *info = (tsio_compat_pre_mmap_info *)arg;
                hi_void *usr_addr = HI_NULL;

                ret = hi_drv_tsio_se_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &usr_addr);
                info->usr_addr = ptr_to_compat(usr_addr);
                break;
            }

        case TSIO_COMPAT_IOCTL_SE_PSTMMAP: {
                tsio_compat_pst_mmap_info *info = (tsio_compat_pst_mmap_info *)arg;
                hi_u8 *usr_addr = compat_ptr(info->usr_addr);

                ret = hi_drv_tsio_se_pst_mmap(info->handle, usr_addr);

                break;
            }

        default: {
                HI_ERR_TSIO("unknown cmd: 0x%x\n", cmd);
            }
    }

    return ret;
}
#endif

static hi_s32 tsio_suspend(hi_void *private_data)
{
    HI_PRINT("tsio suspend ok.\n");

    return HI_SUCCESS;
}

static hi_s32 tsio_resume(hi_void *private_data)
{
    HI_PRINT("tsio resume ok.\n");

    return HI_SUCCESS;
}

static osal_ioctl_cmd g_tsio_ioctl_info[] = {
    {TSIO_IOCTL_GBL_GETCFG,           tsio_ioctl_gbl_impl},
    {TSIO_IOCTL_GBL_SETCFG,           tsio_ioctl_gbl_impl},
    {TSIO_IOCTL_GBL_GETCAP,           tsio_ioctl_gbl_impl},
    {TSIO_IOCTL_GBL_GETOUTOFSYNCCNT,  tsio_ioctl_gbl_impl},

    {TSIO_IOCTL_TSI_OPENPORT,         tsio_ioctl_port_impl},
    {TSIO_IOCTL_TSI_GETPORTATTRS,     tsio_ioctl_port_impl},
    {TSIO_IOCTL_TSI_SETPORTATTRS,     tsio_ioctl_port_impl},
    {TSIO_IOCTL_TSI_GETPORTSTATUS,    tsio_ioctl_port_impl},
    {TSIO_IOCTL_TSI_CLOSEPORT,        tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_OPENPORT,         tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_GETPORTATTRS,     tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_SETPORTATTRS,     tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_GETPORTSTATUS,    tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_BEGINBULKSEG,     tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_ENDBULKSEG,       tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_GETBUFFER,        tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_PUSHBUFFER,       tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_PUTBUFFER,        tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_RESETBUFFER,      tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_CLOSEPORT,        tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_PREMMAP,          tsio_ioctl_port_impl},
    {TSIO_IOCTL_RAM_PSTMMAP,          tsio_ioctl_port_impl},

    {TSIO_IOCTL_PID_CREATECHANNEL,    tsio_ioctl_chan_impl},
    {TSIO_IOCTL_PID_GETCHANNELSTATUS, tsio_ioctl_chan_impl},
    {TSIO_IOCTL_PID_DESTROYCHANNEL,   tsio_ioctl_chan_impl},
    {TSIO_IOCTL_RAW_CREATECHANNEL,    tsio_ioctl_chan_impl},
    {TSIO_IOCTL_RAW_DESTROYCHANNEL,   tsio_ioctl_chan_impl},
    {TSIO_IOCTL_SP_CREATECHANNEL,     tsio_ioctl_chan_impl},
    {TSIO_IOCTL_SP_DESTROYCHANNEL,    tsio_ioctl_chan_impl},
    {TSIO_IOCTL_IVR_CREATECHANNEL,    tsio_ioctl_chan_impl},
    {TSIO_IOCTL_IVR_DESTROYCHANNEL,   tsio_ioctl_chan_impl},

    {TSIO_IOCTL_SE_CREATE,            tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_OPEN,              tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_GETATTRS,          tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_SETATTRS,          tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_GETSTATUS,         tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_ADDCHANNEL,        tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_DELCHANNEL,        tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_DELALLCHANNELS,    tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_ACQBUFFER,         tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_RELBUFFER,         tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_CLOSE,             tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_DESTROY,           tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_PREMMAP,           tsio_ioctl_se_impl},
    {TSIO_IOCTL_SE_PSTMMAP,           tsio_ioctl_se_impl},
    {TSIO_IOCTL_CC_SEND,              tsio_ioctl_cc_impl},
#ifdef CONFIG_COMPAT
    {TSIO_COMPAT_IOCTL_RAM_GETBUFFER,  tsio_compat_ioctl_port_impl},
    {TSIO_COMPAT_IOCTL_RAM_PUSHBUFFER, tsio_compat_ioctl_port_impl},
    {TSIO_COMPAT_IOCTL_RAM_PUTBUFFER,  tsio_compat_ioctl_port_impl},
    {TSIO_COMPAT_IOCTL_RAM_PREMMAP,    tsio_compat_ioctl_port_impl},
    {TSIO_COMPAT_IOCTL_RAM_PSTMMAP,    tsio_compat_ioctl_port_impl},
    {TSIO_COMPAT_IOCTL_SE_ACQBUFFER,  tsio_compat_ioctl_se_impl},
    {TSIO_COMPAT_IOCTL_SE_RELBUFFER,  tsio_compat_ioctl_se_impl},
    {TSIO_COMPAT_IOCTL_SE_PREMMAP,    tsio_compat_ioctl_se_impl},
    {TSIO_COMPAT_IOCTL_SE_PSTMMAP,    tsio_compat_ioctl_se_impl},
#endif
};

static osal_fileops g_tsio_fops = {
    .open            = tsio_open,
    .release         = tsio_release,
    .cmd_list        = g_tsio_ioctl_info,
    .cmd_cnt         = sizeof(g_tsio_ioctl_info) / sizeof(osal_ioctl_cmd),
};

static osal_pmops g_tsio_pm_ops = {
    .pm_suspend        = tsio_suspend,
    .pm_resume         = tsio_resume,
};

static osal_dev g_tsio_dev = {
    .name = HI_DEV_TSIO_NAME,
    .minor = HI_DEV_TSIO_MINOR,
    .fops = &g_tsio_fops,
    .pmops = &g_tsio_pm_ops,
};

static hi_s32 tsio_dev_register(hi_void)
{
    return osal_dev_register(&g_tsio_dev);
}

static hi_void tsio_dev_unregister(hi_void)
{
    osal_dev_unregister(&g_tsio_dev);
}

hi_s32 tsio_mod_init(hi_void)
{
    hi_s32 ret;
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    ret = tsio_dev_register();
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("register TSIO dev failed.\n");
        goto out;
    }

#ifdef MODULE
    HI_PRINT("Load hi_tsio.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    osal_atomic_init(&mgmt->ref_count);
    osal_mutex_init(&mgmt->lock);

    ret = tsio_slot_table_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_TSIO("tsio_slot_table_init failed\n");
        goto out;
    }

out:
    return ret;
}

hi_void tsio_mod_exit(hi_void)
{
    struct tsio_mgmt *mgmt = get_tsio_mgmt();

    tsio_slot_table_destory();

    osal_atomic_destory(&mgmt->ref_count);
    osal_mutex_destory(&mgmt->lock);

    tsio_dev_unregister();
    HI_PRINT("Unload hi_tsio.ko success.\n");

    return;
}

#ifdef MODULE
module_init(tsio_mod_init);
module_exit(tsio_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
