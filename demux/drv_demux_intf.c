/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux interface of module.
 * Author: sdk
 * Create: 2017-06-05
 */

#include <linux/uaccess.h>
#include <linux/module.h>
#include "linux/compat.h"
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include "linux/hisilicon/securec.h"
#include "hi_drv_dev.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"

#include "hi_drv_demux.h"
#include "drv_demux_ioctl.h"
#include "drv_demux_utils.h"
#include "drv_demux_define.h"
#include "drv_demux_func.h"
#include "drv_demux_plyfct.h"
#include "drv_demux_recfct.h"
#include "drv_demux_dscfct.h"
#include "drv_demux_rmxfct.h"
#include "hal_demux.h"

static hi_s32 dmx_open(hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_session *session = HI_NULL;

    ret = hi_drv_dmx_create_session(&session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    *(struct dmx_session **)private_data = session;

out:
    return ret;
}

static hi_s32 dmx_release(hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    if ((*(struct dmx_session **)private_data) != HI_NULL) {
        struct dmx_session *session = *(struct dmx_session **)private_data;

        ret = hi_drv_dmx_destroy_session(session);
        if (ret == HI_SUCCESS) {
            *(struct dmx_session **)private_data = HI_NULL;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_glb_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_GLB_GETCAP: {
            dmx_capability *info = (dmx_capability *)arg;

            ret = hi_drv_dmx_get_capability(info);

            break;
        }

        case DMX_IOCTL_GLB_GET_PACKETNUM: {
            dmx_port_packet_info *info = (dmx_port_packet_info *)arg;

            ret = hi_drv_dmx_port_get_packet_num(info->port, &info->port_stat);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 _dmx_ioctl_if_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    switch (cmd) {
        case DMX_IOCTL_IF_GETPORTATTRS: {
            dmx_get_port_attrs *info = (dmx_get_port_attrs *)arg;

            ret = hi_drv_dmx_if_get_port_attrs(info->port, &info->attrs);

            break;
        }

        case DMX_IOCTL_IF_SETPORTATTRS: {
            dmx_set_port_attrs *info = (dmx_set_port_attrs *)arg;

            ret = hi_drv_dmx_if_set_port_attrs(info->port, &info->attrs);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);

            ret = HI_FAILURE;

            break;
        }
    }

    return ret;
}

static hi_s32 _dmx_ioctl_tsi_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    switch (cmd) {
        case DMX_IOCTL_TSI_GETPORTATTRS: {
            dmx_get_port_attrs *info = (dmx_get_port_attrs *)arg;

            ret = hi_drv_dmx_tsi_get_port_attrs(info->port, &info->attrs);

            break;
        }

        case DMX_IOCTL_TSI_SETPORTATTRS: {
            dmx_set_port_attrs *info = (dmx_set_port_attrs *)arg;

            ret = hi_drv_dmx_tsi_set_port_attrs(info->port, &info->attrs);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);

            ret = HI_FAILURE;

            break;
        }
    }

    return ret;
}

static hi_s32 _dmx_ioctl_tag_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    switch (cmd) {
        case DMX_IOCTL_TAG_GETPORTATTRS: {
            dmx_get_tag_port_attrs *info = (dmx_get_tag_port_attrs *)arg;

            ret = hi_drv_dmx_tag_get_port_attrs(info->port, &info->attrs);

            break;
        }

        case DMX_IOCTL_TAG_SETPORTATTRS: {
            dmx_set_tag_port_attrs *info = (dmx_set_tag_port_attrs *)arg;

            ret = hi_drv_dmx_tag_set_port_attrs(info->port, &info->attrs);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);

            ret = HI_FAILURE;

            break;
        }
    }

    return ret;
}

static hi_s32 _dmx_ioctl_tso_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    switch (cmd) {
        case DMX_IOCTL_TSO_GETPORTATTRS: {
            dmx_get_tso_port_attrs *info = (dmx_get_tso_port_attrs *)arg;

            ret = hi_drv_dmx_tso_get_port_attrs(info->port, &info->attrs);

            break;
        }

        case DMX_IOCTL_TSO_SETPORTATTRS: {
            dmx_set_tso_port_attrs *info = (dmx_set_tso_port_attrs *)arg;

            ret = hi_drv_dmx_tso_set_port_attrs(info->port, &info->attrs);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);

            ret = HI_FAILURE;

            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_general_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    switch (cmd) {
        case DMX_IOCTL_IF_GETPORTATTRS:
        case DMX_IOCTL_IF_SETPORTATTRS: {
            ret = _dmx_ioctl_if_port_impl(cmd, arg, *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_TSI_GETPORTATTRS:
        case DMX_IOCTL_TSI_SETPORTATTRS: {
            ret = _dmx_ioctl_tsi_port_impl(cmd, arg, *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_TAG_GETPORTATTRS:
        case DMX_IOCTL_TAG_SETPORTATTRS: {
            ret = _dmx_ioctl_tag_port_impl(cmd, arg, *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_TSO_GETPORTATTRS:
        case DMX_IOCTL_TSO_SETPORTATTRS: {
            ret = _dmx_ioctl_tso_port_impl(cmd, arg, *(struct dmx_session **)private_data);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);

            ret = HI_FAILURE;

            break;
        }
    }

    return ret;
}
static hi_s32 dmx_ioctl_ram_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_RAM_OPENPORT: {
            dmx_open_ram_port_info *info = (dmx_open_ram_port_info *)arg;

            ret = hi_drv_dmx_ram_open_port(info->port, &info->attrs, &info->handle,
                                           *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTATTRS: {
            dmx_get_ram_port_attrs *info = (dmx_get_ram_port_attrs *)arg;

            ret = hi_drv_dmx_ram_get_port_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RAM_SETPORTATTRS: {
            dmx_set_ram_port_attrs *info = (dmx_set_ram_port_attrs *)arg;

            ret = hi_drv_dmx_ram_set_port_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTSTATUS: {
            dmx_get_ram_port_status_info *info = (dmx_get_ram_port_status_info *)arg;

            ret = hi_drv_dmx_ram_get_port_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_RAM_GETBUFFER: {
            dmx_get_ram_buffer_info *info = (dmx_get_ram_buffer_info *)arg;

            ret = hi_drv_dmx_ram_get_buffer(info->handle, info->req_len, &info->buf, info->time_out);

            break;
        }

        case DMX_IOCTL_RAM_PUSHBUFFER: {
            dmx_push_ram_buffer_info *info = (dmx_push_ram_buffer_info *)arg;

            ret = hi_drv_dmx_ram_push_buffer(info->handle, &info->buf);

            break;
        }

        case DMX_IOCTL_RAM_PUTBUFFER: {
            dmx_put_ram_buffer_info *info = (dmx_put_ram_buffer_info *)arg;

            ret = hi_drv_dmx_ram_put_buffer(info->handle, info->valid_datalen, info->start_pos);

            break;
        }

        case DMX_IOCTL_RAM_RELEASEBUFFER: {
            dmx_release_ram_buffer_info *info = (dmx_release_ram_buffer_info *)arg;

            ret = hi_drv_dmx_ram_release_buffer(info->handle, &info->buf);

            break;
        }

        case DMX_IOCTL_RAM_FLUSHBUFFER: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_ram_flush_buffer(*handle);

            break;
        }

        case DMX_IOCTL_RAM_RESETBUFFER: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_ram_reset_buffer(*handle);

            break;
        }

        case DMX_IOCTL_RAM_CLOSEPORT: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_ram_close_port(*handle);

            break;
        }

        case DMX_IOCTL_RAM_PREMMAP: {
            dmx_pre_mmap_info *info = (dmx_pre_mmap_info *)arg;
            ret = hi_drv_dmx_ram_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &info->usr_addr);

            break;
        }

        case DMX_IOCTL_RAM_PSTMMAP: {
            dmx_pst_mmap_info *info = (dmx_pst_mmap_info *)arg;

            ret = hi_drv_dmx_ram_pst_mmap(info->handle, info->usr_addr);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTID: {
            dmx_get_ram_portid_info *info = (dmx_get_ram_portid_info *)arg;

            ret = hi_drv_dmx_ram_get_portid(info->handle, &info->port);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTHANDLE: {
            dmx_get_ram_porthandle_info *info = (dmx_get_ram_porthandle_info *)arg;

            ret = hi_drv_dmx_ram_get_porthandle(info->port, &info->handle);

            break;
        }

        case DMX_IOCTL_RAM_GETBUFHANDLE: {
            dmx_get_ram_buf_handle *info = (dmx_get_ram_buf_handle *)arg;

            ret = hi_drv_dmx_ram_get_bufhandle(info->handle,
                &info->buf_handle, &info->dsc_buf_handle, &info->flush_buf_handle);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_band_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_BAND_OPEN: {
            dmx_open_band_info *info = (dmx_open_band_info *)arg;

            ret = hi_drv_dmx_band_open(info->band, &info->attrs, &info->handle,
                                       *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_BAND_REFINC: {
            dmx_open_band_info *info = (dmx_open_band_info *)arg;

            ret = hi_drv_dmx_band_ref_inc(info->band, &info->attrs, &info->handle,
                                          *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_BAND_ATTACHPORT: {
            dmx_band_attach_port_info *info = (dmx_band_attach_port_info *)arg;

            ret = hi_drv_dmx_band_attach_port(info->handle, info->port);

            break;
        }

        case DMX_IOCTL_BAND_DETACHPORT: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_band_detach_port(*handle);

            break;
        }

        case DMX_IOCTL_BAND_GETATTRS: {
            dmx_get_band_attr_info *info = (dmx_get_band_attr_info *)arg;

            ret = hi_drv_dmx_band_get_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_BAND_SETATTRS: {
            dmx_set_band_attr_info *info = (dmx_set_band_attr_info *)arg;

            ret = hi_drv_dmx_band_set_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_BAND_GETSTATUS: {
            dmx_get_band_status_info *info = (dmx_get_band_status_info *)arg;

            ret = hi_drv_dmx_band_get_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_BAND_CLOSE: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_band_close(*handle);

            break;
        }

        case DMX_IOCTL_BAND_REFDEC: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_band_ref_dec(*handle);

            break;
        }

        case DMX_IOCTL_BAND_TEI_SET: {
            dmx_tei_set_info *info = (dmx_tei_set_info *)arg;

            ret = hi_drv_dmx_band_tei_set(info->band_handle, info->tei);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_rmx_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_RMXFCT_CREATE: {
            dmx_create_rmx_fct_info *info = (dmx_create_rmx_fct_info *)arg;

            ret = hi_drv_dmx_rmx_create(&info->attrs, &info->rmx_handle, *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_RMXFCT_OPEN: {
            hi_handle *rmx_handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rmx_open(*rmx_handle);

            break;
        }

        case DMX_IOCTL_RMXFCT_GETATTRS: {
            dmx_get_rmx_fct_attr_info *info = (dmx_get_rmx_fct_attr_info *)arg;

            ret = hi_drv_dmx_rmx_get_attrs(info->rmx_handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RMXFCT_SETATTRS: {
            dmx_set_rmx_fct_attr_info *info = (dmx_set_rmx_fct_attr_info *)arg;

            ret = hi_drv_dmx_rmx_set_attrs(info->rmx_handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RMXFCT_GETSTATUS: {
            dmx_get_rmx_fct_status_info *info = (dmx_get_rmx_fct_status_info *)arg;

            ret = hi_drv_dmx_rmx_get_status(info->rmx_handle, &info->status);

            break;
        }

        case DMX_IOCTL_RMXFCT_ADDPUMP: {
            dmx_rmx_fct_add_pump_info *info = (dmx_rmx_fct_add_pump_info *)arg;

            ret = hi_drv_dmx_rmx_add_pump(info->rmx_handle, &info->pump_attrs, &info->rmx_pump_handle,
                                          *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_RMXFCT_DELPUMP: {
            hi_handle *rmx_pump_handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rmx_del_pump(*rmx_pump_handle);

            break;
        }

        case DMX_IOCTL_RMXFCT_DELALLPUMP: {
            hi_handle *rmx_handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rmx_del_all_pump(*rmx_handle);

            break;
        }

        case DMX_IOCTL_RMXFCT_GETPUMPATTRS: {
            dmx_get_rmx_pump_attr_info *info = (dmx_get_rmx_pump_attr_info *)arg;

            ret = hi_drv_dmx_rmx_get_pump_attrs(info->rmx_pump_handle, &info->pump_attrs);

            break;
        }

        case DMX_IOCTL_RMXFCT_SETPUMPATTRS: {
            dmx_set_rmx_pump_attr_info *info = (dmx_set_rmx_pump_attr_info *)arg;

            ret = hi_drv_dmx_rmx_set_pump_attrs(info->rmx_pump_handle, &info->pump_attrs);

            break;
        }

        case DMX_IOCTL_RMXFCT_CLOSE: {
            hi_handle *rmx_handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rmx_close(*rmx_handle);

            break;
        }

        case DMX_IOCTL_RMXFCT_DESTROY: {
            hi_handle *rmx_handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rmx_destroy(*rmx_handle);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_pid_ch_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_PIDCHANNEL_CREATE: {
            dmx_create_pid_ch_info *info = (dmx_create_pid_ch_info *)arg;

            ret = hi_drv_dmx_pid_ch_create(info->band_handle, info->pid, &info->handle,
                                           *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_REFINC: {
            dmx_create_pid_ch_info *info = (dmx_create_pid_ch_info *)arg;

            ret = hi_drv_dmx_pid_ch_ref_inc(info->band_handle, info->pid, &info->handle,
                                            *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_GETHANDLE: {
            dmx_get_pid_ch_handle_info *info = (dmx_get_pid_ch_handle_info *)arg;

            ret = hi_drv_dmx_pid_ch_get_handle(info->band_handle, info->pid, &info->handle);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_GETSTATUS: {
            dmx_get_pid_ch_status_info *info = (dmx_get_pid_ch_status_info *)arg;

            ret = hi_drv_dmx_pid_ch_get_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_LOCKOUT: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_pid_ch_lock_out(*handle);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_UNLOCKOUT: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_pid_ch_un_lock_out(*handle);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_pid_ch_destroy(*handle);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_REFDEC: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_pid_ch_ref_dec(*handle);

            break;
        }

        case DMX_IOCTL_PIDCHANNEL_GETFREECNT: {
            hi_u32 *free_cnt = (hi_u32 *)arg;

            ret = hi_drv_dmx_pid_ch_get_free_cnt(free_cnt);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_play_fct_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_PLAYFCT_CREATE: {
            dmx_create_play_fct_info *info = (dmx_create_play_fct_info *)arg;

            ret = hi_drv_dmx_play_create(&info->attrs, &info->handle, *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_PLAYFCT_OPEN: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_open(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETATTRS: {
            dmx_get_play_fct_attr_info *info = (dmx_get_play_fct_attr_info *)arg;

            ret = hi_drv_dmx_play_get_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_SETATTRS: {
            dmx_set_play_fct_attr_info *info = (dmx_set_play_fct_attr_info *)arg;

            ret = hi_drv_dmx_play_set_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETSTATUS: {
            dmx_get_play_fct_status_info *info = (dmx_get_play_fct_status_info *)arg;

            ret = hi_drv_dmx_play_get_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GET_SCRAMBLEDFLAG: {
            dmx_play_fct_get_scrambled_flag_info *info = (dmx_play_fct_get_scrambled_flag_info *)arg;

            ret = hi_drv_dmx_play_get_scrambled_flag(info->handle, &info->scramble_flag);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETPACKET: {
            dmx_get_play_fct_packet_info *info = (dmx_get_play_fct_packet_info *)arg;

            ret = hi_drv_dmx_play_get_packet_num(info->handle, &info->chn_stat);

            break;
        }

        case DMX_IOCTL_PLAYFCT_ATTACHPIDCH: {
            dmx_play_fct_attach_pid_ch_info *info = (dmx_play_fct_attach_pid_ch_info *)arg;

            ret = hi_drv_dmx_play_attach_pid_ch(info->handle, info->pid_ch_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DETACHPIDCH: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_detach_pid_ch(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_CREATEFILTER: {
            dmx_play_fct_create_flt_info *info = (dmx_play_fct_create_flt_info *)arg;

            ret = hi_drv_dmx_play_create_filter(&info->attrs, &info->flt_handle,
                                                *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_PLAYFCT_ADDFILTER: {
            dmx_play_fct_add_flt_info *info = (dmx_play_fct_add_flt_info *)arg;

            ret = hi_drv_dmx_play_add_filter(info->handle, info->flt_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DELFILTER: {
            dmx_play_fct_del_flt_info *info = (dmx_play_fct_del_flt_info *)arg;

            ret = hi_drv_dmx_play_del_filter(info->handle, info->flt_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DELALLFILTER: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_del_all_filter(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_UPDATEFILTER: {
            dmx_play_fct_update_flt_info *info = (dmx_play_fct_update_flt_info *)arg;

            ret = hi_drv_dmx_play_update_filter(info->flt_handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETFILTER: {
            dmx_play_fct_get_flt_info *info = (dmx_play_fct_get_flt_info *)arg;

            ret = hi_drv_dmx_play_get_filter(info->flt_handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETHANDLE: {
            dmx_play_fct_get_handle_info *info = (dmx_play_fct_get_handle_info *)arg;

            ret = hi_drv_dmx_play_get_handle_by_filter(info->flt_handle, &info->play_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GET_FREEFLT_CNT: {
            hi_u32 *free_flt_cnt = (hi_u32 *)arg;

            ret = hi_drv_dmx_play_get_free_filter_cnt(free_flt_cnt);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DESTROYFILTER: {
            hi_handle *flt_handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_destroy_filter(*flt_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_ACQUIREBUF: {
            dmx_play_fct_acquire_buf_info *info = (dmx_play_fct_acquire_buf_info *)arg;
            dmx_buffer *buf_tmp = HI_NULL;
            dmx_buffer *usr_buf_tmp = HI_NULL;
            hi_u32 acquire_num = info->acq_num;

            if (acquire_num >= 1 && acquire_num <= DMX_PLAY_FCT_BUF_NUM) {
                buf_tmp = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_buffer) * acquire_num, GFP_KERNEL);
                if (buf_tmp == HI_NULL) {
                    HI_FATAL_DEMUX("malloc failed.\n");
                    ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                    break;
                }
            } else {
                HI_FATAL_DEMUX("invalid acquire_num[%#x].\n", acquire_num);
                ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                break;
            }

            ret = hi_drv_dmx_play_acquire_buf(info->handle, info->acq_num, info->time_out, &info->acqed_num, buf_tmp);
            if (ret == HI_SUCCESS && info->acqed_num) {
                dmx_buffer usr_buf;
                hi_s32 i;

                usr_buf_tmp = info->play_fct_buf;
                for (i = 0; i < info->acqed_num; i++) {
                    memcpy(&usr_buf, &buf_tmp[i], sizeof(dmx_buffer));

                    if (osal_copy_to_user(&usr_buf_tmp[i], &usr_buf, sizeof(dmx_buffer))) {
                        HI_ERR_DEMUX("copy data buf to usr failed\n");
                        ret = HI_FAILURE;
                        break;
                    }
                }
            }

            HI_KFREE(HI_ID_DEMUX, buf_tmp);
            break;
        }

        case DMX_IOCTL_PLAYFCT_RELEASEBUF: {
            dmx_play_fct_release_buf_info *info = (dmx_play_fct_release_buf_info *)arg;
            dmx_buffer *buf_tmp = HI_NULL;
            dmx_buffer *usr_buf_tmp = HI_NULL;
            dmx_buffer  usr_buf;
            hi_u32 rel_num = info->rel_num;
            hi_s32 i;

            if (rel_num <= DMX_PLAY_FCT_BUF_NUM) {
                buf_tmp = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_buffer) * rel_num, GFP_KERNEL);
                if (buf_tmp == NULL) {
                    HI_FATAL_DEMUX("malloc failed.\n");
                    ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                    break;
                }
            } else {
                HI_FATAL_DEMUX("invalid rel_num[%#x].\n", rel_num);
                ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                break;
            }

            usr_buf_tmp = info->play_fct_buf;
            for (i = 0; i < rel_num; i++) {
                if (osal_copy_from_user(&usr_buf, &usr_buf_tmp[i], sizeof(dmx_buffer))) {
                    HI_ERR_DEMUX("copy data buf from user failed\n");
                }

                memcpy(&buf_tmp[i], &usr_buf, sizeof(dmx_buffer));
            }

            ret = hi_drv_dmx_play_release_buf(info->handle, rel_num, buf_tmp);

            HI_KFREE(HI_ID_DEMUX, buf_tmp);

            break;
        }

        case DMX_IOCTL_PLAYFCT_RESETBUF: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_reset_buf(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_STARTINDEX: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_start_idx(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_RECVINDEX: {
            dmx_play_index_info *info = (dmx_play_index_info *)arg;
            dmx_index_data *index_tmp = HI_NULL;
            dmx_index_data *usr_index_tmp = HI_NULL;
            hi_u32 acquire_num = info->req_num;

            /* check the point */
            DMX_NULL_POINTER_BREAK(info->index_data);

            if (acquire_num <= DMX_MAX_REC_INDEX_NUM) {
                index_tmp = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_index_data) * acquire_num, GFP_KERNEL);
                if (index_tmp == HI_NULL) {
                    HI_FATAL_DEMUX("malloc failed.\n");
                    ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                    break;
                }
            } else {
                HI_FATAL_DEMUX("invalid acquire_num[%#x].\n", acquire_num);
                ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                break;
            }

            ret = hi_drv_dmx_play_recv_idx(info->handle, info->req_num, info->time_out, &info->reqed_num, index_tmp);
            if (ret == HI_SUCCESS && info->reqed_num) {
                hi_s32 i;

                usr_index_tmp = info->index_data;

                for (i = 0; i < info->reqed_num; i++) {
                    if (osal_copy_to_user(&usr_index_tmp[i], &index_tmp[i], sizeof(dmx_index_data))) {
                        HI_ERR_DEMUX("copy data buf to usr failed\n");
                        ret = HI_FAILURE;
                        break;
                    }
                }
            }

            HI_KFREE(HI_ID_DEMUX, index_tmp);
            break;
        }

        case DMX_IOCTL_PLAYFCT_STOPINDEX: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_stop_idx(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_CLOSE: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_close(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_destroy(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GET_DATAHANDLE: {
            dmx_get_data_flag *param = (dmx_get_data_flag *)arg;
            dmx_capability cap = {0};
            hi_u32 valid_num = 0;

            if (hi_drv_dmx_get_capability(&cap) != HI_SUCCESS) {
                HI_ERR_DEMUX("get capability failed.\n");
                break;
            }

            if (param->watch_num <= cap.channel_num) {
                hi_handle valid_array[param->watch_num];

                if (memset_s(valid_array, sizeof(valid_array), 0, sizeof(valid_array)) != HI_SUCCESS) {
                    HI_ERR_DEMUX("memset_s failed.\n");
                    break;
                }

                ret = hi_drv_dmx_play_get_data_handle(valid_array, &valid_num, param->watch_num,
                                                      param->time_out_ms);
                if (ret == HI_SUCCESS) {
                    put_user(valid_num, param->valid_num);

                    if (param->valid_channel == HI_NULL) {
                        HI_ERR_DEMUX("user buffer is null!\n");
                        ret = HI_ERR_DMX_NULL_PTR;
                        break;
                    }

                    if (osal_copy_to_user(param->valid_channel, valid_array, sizeof(hi_handle) * valid_num)) {
                        HI_ERR_DEMUX("osal_copy_to_user failed.\n");
                        ret = HI_FAILURE;
                    }
                }
            }else {
                HI_ERR_DEMUX("%d channels are too many.\n", param->watch_num);
            }
            break;
        }

        case DMX_IOCTL_PLAYFCT_PREMMAP: {
            dmx_pre_mmap_info *info = (dmx_pre_mmap_info *)arg;

            ret = hi_drv_dmx_play_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &info->usr_addr);

            break;
        }

        case DMX_IOCTL_PLAYFCT_PSTMMAP: {
            dmx_pst_mmap_info *info = (dmx_pst_mmap_info *)arg;

            ret = hi_drv_dmx_play_pst_mmap(info->handle, info->usr_addr);

            break;
        }

        case DMX_IOCTL_PLAYFCT_SETEOS: {
            hi_handle handle = *(hi_handle *)arg;

            ret = hi_drv_dmx_play_set_eos_flag(handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_SELECT_DATAHANDLE: {
            dmx_select_data_flag *param = (dmx_select_data_flag *)arg;
            dmx_capability cap = {0};
            hi_u32 valid_chan_num = 0;

            if (hi_drv_dmx_get_capability(&cap) != HI_SUCCESS) {
                HI_ERR_DEMUX("get capability failed.\n");
                break;
            }

            if (param->watch_num <= cap.channel_num) {
                hi_u32 watch_chan_num = param->watch_num;
                hi_handle watch_chan[param->watch_num];
                hi_handle valid_chan[param->watch_num];

                if (memset_s(valid_chan, sizeof(valid_chan), 0, sizeof(valid_chan)) != HI_SUCCESS) {
                    HI_ERR_DEMUX("mem set failed.\n");
                    break;
                }

                if (osal_copy_from_user(watch_chan, param->watch_array, sizeof(hi_handle) * watch_chan_num)) {
                    HI_ERR_DEMUX("osal_copy_from_user failed.\n");
                    break;
                }

                ret = hi_drv_dmx_play_select_data_handle(watch_chan, watch_chan_num, valid_chan, &valid_chan_num,
                                                         param->time_out_ms);
                if (ret == HI_SUCCESS) {
                    put_user(valid_chan_num, param->valid_num);

                    if (param->valid_channel == HI_NULL) {
                        HI_ERR_DEMUX("user buffer is null!\n");
                        ret = HI_ERR_DMX_NULL_PTR;
                        break;
                    }

                    if (osal_copy_to_user(param->valid_channel, valid_chan, sizeof(hi_handle) * valid_chan_num)) {
                        HI_ERR_DEMUX("osal_copy_to_user failed.\n");
                        ret = HI_FAILURE;
                    }
                }
            }else {
                HI_ERR_DEMUX("too many ch num to watch, %u > %u.\n", param->watch_num, cap.channel_num);
            }
            break;
        }

        case DMX_IOCTL_PLAYFCT_GETPIDCH: {
            dmx_play_fct_get_pid_ch_info *info = (dmx_play_fct_get_pid_ch_info *)arg;

            ret = hi_drv_dmx_play_get_pid_ch(info->handle, &info->pid_ch_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GET_BUFHANDLE: {
            dmx_play_get_buf_handle *info = (dmx_play_get_buf_handle *)arg;

            ret = hi_drv_dmx_play_get_bufhandle(info->handle, &info->buf_handle, &info->type);

            break;
        }

        case DMX_IOCTL_PLAYFCT_CC_REPEAT_SET: {
            dmx_set_chan_cc_repeat_info *info = (dmx_set_chan_cc_repeat_info *)arg;

            ret = hi_drv_dmx_play_cc_repeat_set(info->chan_handle, info->cc_repeat_mode);

            break;
        }

        case DMX_IOCTL_PLAYFCT_PUSI_SET: {
            dmx_pusi_set_info *info = (dmx_pusi_set_info *)arg;

            ret = hi_drv_dmx_play_pusi_set(info->no_pusi);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_rec_fct_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_RECFCT_CREATE: {
            dmx_create_rec_fct_info *info = (dmx_create_rec_fct_info *)arg;

            ret = hi_drv_dmx_rec_create(&info->attrs, &info->handle, *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_RECFCT_OPEN: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_open(*handle);

            break;
        }

        case DMX_IOCTL_RECFCT_GETATTRS: {
            dmx_get_rec_fct_attr_info *info = (dmx_get_rec_fct_attr_info *)arg;

            ret = hi_drv_dmx_rec_get_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RECFCT_SETATTRS: {
            dmx_set_rec_fct_attr_info *info = (dmx_set_rec_fct_attr_info *)arg;

            ret = hi_drv_dmx_rec_set_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RECFCT_GETSTATUS: {
            dmx_get_rec_fct_status_info *info = (dmx_get_rec_fct_status_info *)arg;

            ret = hi_drv_dmx_rec_get_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_RECFCT_ADDCH: {
            dmx_rec_fct_add_del_ch_info *info = (dmx_rec_fct_add_del_ch_info *)arg;

            ret = hi_drv_dmx_rec_add_ch(info->handle, info->ch_handle);

            break;
        }

        case DMX_IOCTL_RECFCT_DELCH: {
            dmx_rec_fct_add_del_ch_info *info = (dmx_rec_fct_add_del_ch_info *)arg;

            ret = hi_drv_dmx_rec_del_ch(info->handle, info->ch_handle);

            break;
        }

        case DMX_IOCTL_RECFCT_DELALLCH: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_del_all_ch(*handle);

            break;
        }

        case DMX_IOCTL_RECFCT_ACQUIREBUF: {
            dmx_rec_fct_acquire_buf_info *info = (dmx_rec_fct_acquire_buf_info *)arg;
            dmx_buffer buf_tmp = {0};

            ret = hi_drv_dmx_rec_acquire_buf(info->handle, info->req_len, info->time_out, &buf_tmp);
            if (ret == HI_SUCCESS) {
                if (osal_copy_to_user(info->rec_fct_buf, &buf_tmp, sizeof(dmx_buffer))) {
                    HI_ERR_DEMUX("copy data buf to user failed\n");
                    ret = HI_FAILURE;
                }
            }

            break;
        }

        case DMX_IOCTL_RECFCT_RELEASEBUF: {
            dmx_rec_fct_release_buf_info *info = (dmx_rec_fct_release_buf_info *)arg;
            dmx_buffer buf_tmp;

            if (osal_copy_from_user(&buf_tmp, info->rec_fct_buf, sizeof(dmx_buffer))) {
                HI_ERR_DEMUX("copy data buf from user failed\n");
            }

            ret = hi_drv_dmx_rec_release_buf(info->handle, &buf_tmp);

            break;
        }

        case DMX_IOCTL_RECFCT_RECVINDEX: {
            dmx_rec_index_info *info = (dmx_rec_index_info *)arg;
            dmx_index_data *index_tmp = HI_NULL;
            dmx_index_data *usr_index_tmp = HI_NULL;
            hi_u32 acquire_num = info->req_num;

            /* check the point */
            DMX_NULL_POINTER_BREAK(info->index_data);

            if (acquire_num <= DMX_MAX_REC_INDEX_NUM) {
                index_tmp = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_index_data) * acquire_num, GFP_KERNEL);
                if (index_tmp == HI_NULL) {
                    HI_FATAL_DEMUX("malloc failed.\n");
                    ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                    break;
                }
            } else {
                HI_FATAL_DEMUX("invalid acquire_num[%#x].\n", acquire_num);
                ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                break;
            }

            ret = hi_drv_dmx_rec_recv_idx(info->handle, info->req_num, info->time_out, &info->reqed_num, index_tmp);
            if (ret == HI_SUCCESS && info->reqed_num) {
                hi_s32 i;

                usr_index_tmp = info->index_data;

                for (i = 0; i < info->reqed_num; i++) {
                    if (osal_copy_to_user(&usr_index_tmp[i], &index_tmp[i], sizeof(dmx_index_data))) {
                        HI_ERR_DEMUX("copy data buf to usr failed\n");
                        ret = HI_FAILURE;
                        break;
                    }
                }
            }

            HI_KFREE(HI_ID_DEMUX, index_tmp);
            break;
        }

        case DMX_IOCTL_RECFCT_PEEK_IDX_BUF: {
            dmx_rec_peek_idx_buf_info *info = (dmx_rec_peek_idx_buf_info *)arg;

            ret = hi_drv_dmx_rec_peek_idx_and_buf(info->handle, info->time_out, &info->index_num, &info->rec_data_len);

            break;
        }

        case DMX_IOCTL_RECFCT_CLOSE: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_close(*handle);

            break;
        }

        case DMX_IOCTL_RECFCT_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_destroy(*handle);

            break;
        }

        case DMX_IOCTL_RECFCT_PREMMAP: {
            dmx_pre_mmap_info *info = (dmx_pre_mmap_info *)arg;

            ret = hi_drv_dmx_rec_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &info->usr_addr);

            break;
        }

        case DMX_IOCTL_RECFCT_PSTMMAP: {
            dmx_pst_mmap_info *info = (dmx_pst_mmap_info *)arg;

            ret = hi_drv_dmx_rec_pst_mmap(info->handle, info->usr_addr);

            break;
        }

        case DMX_IOCTL_RECFCT_SETEOS: {
            dmx_rec_set_eos_info *info = (dmx_rec_set_eos_info *)arg;

            ret = hi_drv_dmx_rec_set_eos_flag(info->handle, info->eos_flag);

            break;
        }

        case DMX_IOCTL_RECFCT_GETBUFHANDLE: {
            dmx_rec_get_buf_handle *info = (dmx_rec_get_buf_handle *)arg;

            ret = hi_drv_dmx_rec_get_bufhandle(info->handle, &info->buf_handle);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_dsc_fct_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_DSCFCT_CREATE: {
            dmx_create_dsc_fct_info *info = (dmx_create_dsc_fct_info *)arg;

            ret = hi_drv_dmx_dsc_create(&info->attrs, &info->handle, *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_DSCFCT_GETATTRS: {
            dmx_get_dsc_fct_attr_info *info = (dmx_get_dsc_fct_attr_info *)arg;

            ret = hi_drv_dmx_dsc_get_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_DSCFCT_SETATTRS: {
            dmx_set_dsc_fct_attr_info *info = (dmx_set_dsc_fct_attr_info *)arg;

            ret = hi_drv_dmx_dsc_set_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_DSCFCT_ATTACH: {
            dmx_dsc_fct_attach_info *info = (dmx_dsc_fct_attach_info *)arg;

            ret = hi_drv_dmx_dsc_attach(info->handle, info->target_handle);

            break;
        }

        case DMX_IOCTL_DSCFCT_DETACH: {
            dmx_dsc_fct_detach_info *info = (dmx_dsc_fct_detach_info *)arg;

            ret = hi_drv_dmx_dsc_detach(info->handle, info->target_handle);

            break;
        }

        case DMX_IOCTL_DSCFCT_ATTACH_KEYSLOT: {
            dmx_dsc_fct_attach_info *info = (dmx_dsc_fct_attach_info *)arg;

            ret = hi_drv_dmx_dsc_attach_keyslot(info->handle, info->target_handle);

            break;
        }

        case DMX_IOCTL_DSCFCT_DETACH_KEYSLOT: {
            dmx_dsc_fct_detach_info *info = (dmx_dsc_fct_detach_info *)arg;

            ret = hi_drv_dmx_dsc_detach_keyslot(info->handle);

            break;
        }

        case DMX_IOCTL_DSCFCT_GET_KS_HANDLE: {
            dmx_dsc_fct_get_ks_handle_info *info = (dmx_dsc_fct_get_ks_handle_info *)arg;

            ret = hi_drv_dmx_dsc_get_keyslot_handle(info->handle, &info->ks_handle);

            break;
        }

        case DMX_IOCTL_DSCFCT_SETEVENIV: {
            dmx_dsc_fct_set_key_info *info = (dmx_dsc_fct_set_key_info *)arg;

            ret = hi_drv_dmx_dsc_set_even_iv(info->handle, info->key, info->len);

            break;
        }

        case DMX_IOCTL_DSCFCT_SETODDIV: {
            dmx_dsc_fct_set_key_info *info = (dmx_dsc_fct_set_key_info *)arg;

            ret = hi_drv_dmx_dsc_set_odd_iv(info->handle, info->key, info->len);

            break;
        }

        case DMX_IOCTL_DSCFCT_SETEVENKEY: {
            dmx_dsc_fct_set_key_info *info = (dmx_dsc_fct_set_key_info *)arg;

            ret = hi_drv_dmx_dsc_set_even_key(info->handle, info->key, info->len);

            break;
        }

        case DMX_IOCTL_DSCFCT_SETODDKEY: {
            dmx_dsc_fct_set_key_info *info = (dmx_dsc_fct_set_key_info *)arg;

            ret = hi_drv_dmx_dsc_set_odd_key(info->handle, info->key, info->len);

            break;
        }

        case DMX_IOCTL_DSCFCT_SETSYSKEY: {
            dmx_dsc_fct_set_sys_key_info *info = (dmx_dsc_fct_set_sys_key_info *)arg;

            ret = hi_drv_dmx_dsc_set_sys_key(info->handle, info->key, info->len);

            break;
        }

        case DMX_IOCTL_DSCFCT_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_dsc_destroy(*handle);

            break;
        }

        case DMX_IOCTL_DSCFCT_GET_KEY_HANDLE: {
            dmx_dsc_fct_get_key_handle_info *info = (dmx_dsc_fct_get_key_handle_info *)arg;

            ret = hi_drv_dmx_dsc_get_dsc_key_handle(info->pid_ch_handle, &info->dsc_handle);

            break;
        }

        case DMX_IOCTL_DSCFCT_GET_FREE_CNT: {
            hi_u32 *free_cnt = (hi_u32 *)arg;

            ret = hi_drv_dmx_dsc_get_free_cnt(free_cnt);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_ioctl_pcr_fct_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_PCRFCT_CREATE: {
            dmx_create_pcr_fct_info *info = (dmx_create_pcr_fct_info *)arg;

            ret = hi_drv_dmx_pcr_create(info->band_handle, info->pid, &info->handle,
                                        *(struct dmx_session **)private_data);

            break;
        }

        case DMX_IOCTL_PCRFCT_ATTACH: {
            dmx_pcr_fct_attach_info *info = (dmx_pcr_fct_attach_info *)arg;

            ret = hi_drv_dmx_pcr_attach(info->handle, info->sync_handle);

            break;
        }

        case DMX_IOCTL_PCRFCT_DETACH: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_pcr_detach(*handle);

            break;
        }

        case DMX_IOCTL_PCRFCT_GETSTATUS: {
            dmx_get_pcr_fct_status_info *info = (dmx_get_pcr_fct_status_info *)arg;

            ret = hi_drv_dmx_pcr_get_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_PCRFCT_GETPCRSCR: {
            dmx_pcr_fct_get_pcr_scr_info *info = (dmx_pcr_fct_get_pcr_scr_info *)arg;

            ret = hi_drv_dmx_pcr_get_pcr_scr(info->handle, &info->pcr_ms, &info->scr_ms);

            break;
        }

        case DMX_IOCTL_PCRFCT_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_pcr_destroy(*handle);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

#ifdef CONFIG_COMPAT
static hi_s32 dmx_compat_ioctl_ram_port_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_RAM_OPENPORT: {
            dmx_open_ram_port_info *info = (dmx_open_ram_port_info *)arg;

            ret = hi_drv_dmx_ram_open_port(info->port, &info->attrs, &info->handle,
                                           (struct dmx_session *)private_data);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTATTRS: {
            dmx_get_ram_port_attrs *info = (dmx_get_ram_port_attrs *)arg;

            ret = hi_drv_dmx_ram_get_port_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RAM_SETPORTATTRS: {
            dmx_set_ram_port_attrs *info = (dmx_set_ram_port_attrs *)arg;

            ret = hi_drv_dmx_ram_set_port_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTSTATUS: {
            dmx_get_ram_port_status_info *info = (dmx_get_ram_port_status_info *)arg;

            ret = hi_drv_dmx_ram_get_port_status(info->handle, &info->status);

            break;
        }

        case DMX_COMPAT_IOCTL_RAM_GETBUFFER: {
            dmx_compat_get_ram_buffer_info *info = (dmx_compat_get_ram_buffer_info *)arg;
            dmx_ram_buffer buf = {0};

            ret = hi_drv_dmx_ram_get_buffer(info->handle, info->req_len, &buf, info->time_out);
            if (ret == HI_SUCCESS) {
                info->buf.data    = ptr_to_compat(buf.data);
                info->buf.length  = buf.length;
                info->buf.phy_addr = buf.phy_addr;
                info->buf.buf_handle = buf.buf_handle;
                info->buf.offset = buf.offset;
            }

            break;
        }

        case DMX_COMPAT_IOCTL_RAM_PUSHBUFFER: {
            dmx_compat_push_ram_buffer_info *info = (dmx_compat_push_ram_buffer_info *)arg;
            dmx_ram_buffer buf = {
                .data    = compat_ptr(info->buf.data),
                .length  = info->buf.length,
                .phy_addr = info->buf.phy_addr,
                .buf_handle = info->buf.buf_handle,
                .offset = info->buf.offset
            };

            ret = hi_drv_dmx_ram_push_buffer(info->handle, &buf);

            break;
        }

        case DMX_COMPAT_IOCTL_RAM_PUTBUFFER: {
            dmx_compat_put_ram_buffer_info *info = (dmx_compat_put_ram_buffer_info *)arg;

            ret = hi_drv_dmx_ram_put_buffer(info->handle, info->valid_datalen, info->start_pos);

            break;
        }

        case DMX_COMPAT_IOCTL_RAM_RELEASEBUFFER: {
            dmx_compat_release_ram_buffer_info *info = (dmx_compat_release_ram_buffer_info *)arg;
            dmx_ram_buffer buf = {
                .data    = compat_ptr(info->buf.data),
                .length  = info->buf.length,
                .phy_addr = info->buf.phy_addr,
                .buf_handle = info->buf.buf_handle,
                .offset = info->buf.offset
            };

            ret = hi_drv_dmx_ram_release_buffer(info->handle, &buf);

            break;
        }

        case DMX_IOCTL_RAM_FLUSHBUFFER: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_ram_reset_buffer(*handle);

            break;
        }

        case DMX_IOCTL_RAM_RESETBUFFER: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_ram_reset_buffer(*handle);

            break;
        }

        case DMX_IOCTL_RAM_CLOSEPORT: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_ram_close_port(*handle);

            break;
        }

        case DMX_COMPAT_IOCTL_RAM_PREMMAP: {
            hi_void *usr_addr = HI_NULL;
            dmx_compat_pre_mmap_info *info = (dmx_compat_pre_mmap_info *)arg;

            ret = hi_drv_dmx_ram_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &usr_addr);
            if (ret == HI_SUCCESS) {
                info->usr_addr = ptr_to_compat(usr_addr);
            }

            break;
        }

        case DMX_COMPAT_IOCTL_RAM_PSTMMAP: {
            dmx_compat_pst_mmap_info *info = (dmx_compat_pst_mmap_info *)arg;
            hi_u8 *usr_addr = compat_ptr(info->usr_addr);

            ret = hi_drv_dmx_ram_pst_mmap(info->handle, usr_addr);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTID: {
            dmx_get_ram_portid_info *info = (dmx_get_ram_portid_info *)arg;

            ret = hi_drv_dmx_ram_get_portid(info->handle, &info->port);

            break;
        }

        case DMX_IOCTL_RAM_GETPORTHANDLE: {
            dmx_get_ram_porthandle_info *info = (dmx_get_ram_porthandle_info *)arg;

            ret = hi_drv_dmx_ram_get_porthandle(info->port, &info->handle);

            break;
        }

        case DMX_IOCTL_RAM_GETBUFHANDLE: {
            dmx_get_ram_buf_handle *info = (dmx_get_ram_buf_handle *)arg;

            ret = hi_drv_dmx_ram_get_bufhandle(info->handle,
                &info->buf_handle, &info->dsc_buf_handle, &info->flush_buf_handle);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 dmx_compat_ioctl_play_fct_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_PLAYFCT_CREATE: {
            dmx_create_play_fct_info *info = (dmx_create_play_fct_info *)arg;

            ret = hi_drv_dmx_play_create(&info->attrs, &info->handle, (struct dmx_session *)private_data);

            break;
        }

        case DMX_IOCTL_PLAYFCT_OPEN: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_open(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETATTRS: {
            dmx_get_play_fct_attr_info *info = (dmx_get_play_fct_attr_info *)arg;

            ret = hi_drv_dmx_play_get_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_SETATTRS: {
            dmx_set_play_fct_attr_info *info = (dmx_set_play_fct_attr_info *)arg;

            ret = hi_drv_dmx_play_set_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETSTATUS: {
            dmx_get_play_fct_status_info *info = (dmx_get_play_fct_status_info *)arg;

            ret = hi_drv_dmx_play_get_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GET_SCRAMBLEDFLAG: {
            dmx_play_fct_get_scrambled_flag_info *info = (dmx_play_fct_get_scrambled_flag_info *)arg;

            ret = hi_drv_dmx_play_get_scrambled_flag(info->handle, &info->scramble_flag);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETPACKET: {
            dmx_get_play_fct_packet_info *info = (dmx_get_play_fct_packet_info *)arg;

            ret = hi_drv_dmx_play_get_packet_num(info->handle, &info->chn_stat);

            break;
        }

        case DMX_IOCTL_PLAYFCT_ATTACHPIDCH: {
            dmx_play_fct_attach_pid_ch_info *info = (dmx_play_fct_attach_pid_ch_info *)arg;

            ret = hi_drv_dmx_play_attach_pid_ch(info->handle, info->pid_ch_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DETACHPIDCH: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_detach_pid_ch(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_CREATEFILTER: {
            dmx_play_fct_create_flt_info *info = (dmx_play_fct_create_flt_info *)arg;

            ret = hi_drv_dmx_play_create_filter(&info->attrs, &info->flt_handle,
                                                (struct dmx_session *)private_data);

            break;
        }

        case DMX_IOCTL_PLAYFCT_ADDFILTER: {
            dmx_play_fct_add_flt_info *info = (dmx_play_fct_add_flt_info *)arg;

            ret = hi_drv_dmx_play_add_filter(info->handle, info->flt_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DELFILTER: {
            dmx_play_fct_del_flt_info *info = (dmx_play_fct_del_flt_info *)arg;

            ret = hi_drv_dmx_play_del_filter(info->handle, info->flt_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DELALLFILTER: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_del_all_filter(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_UPDATEFILTER: {
            dmx_play_fct_update_flt_info *info = (dmx_play_fct_update_flt_info *)arg;

            ret = hi_drv_dmx_play_update_filter(info->flt_handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETFILTER: {
            dmx_play_fct_get_flt_info *info = (dmx_play_fct_get_flt_info *)arg;

            ret = hi_drv_dmx_play_get_filter(info->flt_handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GETHANDLE: {
            dmx_play_fct_get_handle_info *info = (dmx_play_fct_get_handle_info *)arg;

            ret = hi_drv_dmx_play_get_handle_by_filter(info->flt_handle, &info->play_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GET_FREEFLT_CNT: {
            hi_u32 *free_flt_cnt = (hi_u32 *)arg;

            ret = hi_drv_dmx_play_get_free_filter_cnt(free_flt_cnt);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DESTROYFILTER: {
            hi_handle *flt_handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_destroy_filter(*flt_handle);

            break;
        }

        case DMX_COMPAT_IOCTL_PLAYFCT_ACQUIREBUF: {
            dmx_compat_play_fct_acquire_buf_info *info = (dmx_compat_play_fct_acquire_buf_info *)arg;
            dmx_buffer *buf_tmp = HI_NULL;
            dmx_compat_buffer *usr_compat_buf_tmp = HI_NULL;
            hi_u32 acquire_num = info->acq_num;

            if (acquire_num >= 1 && acquire_num <= DMX_PLAY_FCT_BUF_NUM) {
                buf_tmp = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_buffer) * acquire_num, GFP_KERNEL);
                if (buf_tmp == HI_NULL) {
                    HI_FATAL_DEMUX("malloc failed.\n");
                    ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                    break;
                }
            } else {
                HI_FATAL_DEMUX("invalid acquire_num[%#x].\n", acquire_num);
                ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                break;
            }

            ret = hi_drv_dmx_play_acquire_buf(info->handle, info->acq_num, info->time_out,
                                              &info->acqed_num, buf_tmp);
            if (ret == HI_SUCCESS && info->acqed_num) {
                dmx_compat_buffer usr_compat_buf;
                hi_s32 i;

                usr_compat_buf_tmp = (dmx_compat_buffer *)compat_ptr(info->play_fct_buf);
                for (i = 0; i < info->acqed_num; i++) {
                    usr_compat_buf.data = ptr_to_compat(buf_tmp[i].data);
                    usr_compat_buf.phy_addr = buf_tmp[i].phy_addr;
                    usr_compat_buf.buf_handle = buf_tmp[i].buf_handle;
                    usr_compat_buf.offset = buf_tmp[i].offset;
                    usr_compat_buf.length = buf_tmp[i].length;
                    usr_compat_buf.data_type = buf_tmp[i].data_type;
                    usr_compat_buf.pts      = buf_tmp[i].pts;

                    /* for pvr dispctl */
                    usr_compat_buf.disp_enable_flag = buf_tmp[i].disp_enable_flag;
                    usr_compat_buf.disp_frame_distance = buf_tmp[i].disp_frame_distance;
                    usr_compat_buf.disp_time = buf_tmp[i].disp_time;
                    usr_compat_buf.distance_before_first_frame = buf_tmp[i].distance_before_first_frame;
                    usr_compat_buf.gop_num = buf_tmp[i].gop_num;

                    if (osal_copy_to_user(&usr_compat_buf_tmp[i], &usr_compat_buf, sizeof(dmx_compat_buffer))) {
                        HI_ERR_DEMUX("copy data buf to usr failed\n");
                        ret = HI_FAILURE;
                        break;
                    }
                }
            }

            HI_KFREE(HI_ID_DEMUX, buf_tmp);
            break;
        }

        case DMX_COMPAT_IOCTL_PLAYFCT_RELEASEBUF: {
            dmx_compat_play_fct_release_buf_info *info = (dmx_compat_play_fct_release_buf_info *)arg;
            dmx_buffer *buf_tmp = HI_NULL;
            dmx_compat_buffer *usr_compat_buf_tmp = HI_NULL;
            dmx_compat_buffer  usr_compat_buf;
            hi_u32 rel_num = info->rel_num;
            hi_s32 i;

            if (rel_num <= DMX_PLAY_FCT_BUF_NUM) {
                buf_tmp = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_buffer) * rel_num, GFP_KERNEL);
                if (buf_tmp == HI_NULL) {
                    HI_FATAL_DEMUX("malloc failed.\n");
                    ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                    break;
                }
            } else {
                HI_FATAL_DEMUX("invalid rel_num[%#x].\n", rel_num);
                ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                break;
            }

            usr_compat_buf_tmp = (dmx_compat_buffer *)compat_ptr(info->play_fct_buf);
            for (i = 0; i < rel_num; i++) {
                if (osal_copy_from_user(&usr_compat_buf, &usr_compat_buf_tmp[i], sizeof(dmx_compat_buffer))) {
                    HI_ERR_DEMUX("copy data buf from usr failed\n");
                }

                buf_tmp[i].data = (hi_u8 *)compat_ptr(usr_compat_buf.data);
                buf_tmp[i].buf_handle = usr_compat_buf.buf_handle;
                buf_tmp[i].offset = usr_compat_buf.offset;
                buf_tmp[i].phy_addr = usr_compat_buf.phy_addr;
                buf_tmp[i].length = usr_compat_buf.length;
                buf_tmp[i].data_type   = usr_compat_buf.data_type;
                buf_tmp[i].pts        = usr_compat_buf.pts;
            }

            ret = hi_drv_dmx_play_release_buf(info->handle, rel_num, buf_tmp);

            HI_KFREE(HI_ID_DEMUX, buf_tmp);

            break;
        }

        case DMX_IOCTL_PLAYFCT_CLOSE: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_close(*handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_play_destroy(*handle);

            break;
        }

        case DMX_COMPAT_IOCTL_PLAYFCT_GET_DATAHANDLE: {
            dmx_compat_get_data_flag *param = (dmx_compat_get_data_flag *)arg;
            dmx_capability cap = {0};
            hi_u32 valid_num = 0;

            if (hi_drv_dmx_get_capability(&cap) != HI_SUCCESS) {
                HI_ERR_DEMUX("get capability failed.\n");
                break;
            }

            if (param->watch_num <= cap.channel_num) {
                hi_handle valid_array[param->watch_num];
                if (memset_s(valid_array, sizeof(valid_array), 0, sizeof(valid_array)) != HI_SUCCESS) {
                    HI_ERR_DEMUX("mem set failed.\n");
                    break;
                }

                ret = hi_drv_dmx_play_get_data_handle(valid_array, &valid_num, param->watch_num,
                                                      param->time_out_ms);
                if (ret == HI_SUCCESS) {
                    put_user(valid_num, (hi_u32 *)compat_ptr(param->valid_num));

                    if (compat_ptr(param->valid_channel) == HI_NULL) {
                        HI_ERR_DEMUX("user buffer is null!\n");
                        ret = HI_ERR_DMX_NULL_PTR;
                        break;
                    }

                    if (osal_copy_to_user(compat_ptr(param->valid_channel), valid_array, sizeof(hi_handle) * valid_num
)) {
                        HI_ERR_DEMUX("osal_copy_to_user failed.\n");
                        ret = HI_FAILURE;
                    }
                }
            }else {
                HI_ERR_DEMUX("too many ch num to watch, %u > %u.\n", param->watch_num, cap.channel_num);
            }
            break;
        }

        case DMX_COMPAT_IOCTL_PLAYFCT_PREMMAP: {
            hi_void *usr_addr = HI_NULL;
            dmx_compat_pre_mmap_info *info = (dmx_compat_pre_mmap_info *)arg;

            ret = hi_drv_dmx_play_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &usr_addr);
            if (ret == HI_SUCCESS) {
                info->usr_addr = ptr_to_compat(usr_addr);
            }

            break;
        }

        case DMX_COMPAT_IOCTL_PLAYFCT_PSTMMAP: {
            dmx_compat_pst_mmap_info *info = (dmx_compat_pst_mmap_info *)arg;
            hi_u8 *usr_addr = compat_ptr(info->usr_addr);

            ret = hi_drv_dmx_play_pst_mmap(info->handle, usr_addr);

            break;
        }

        case DMX_IOCTL_PLAYFCT_SETEOS: {
            hi_handle handle = *(hi_handle *)arg;

            ret = hi_drv_dmx_play_set_eos_flag(handle);

            break;
        }

        case DMX_COMPAT_IOCTL_PLAYFCT_SELECT_DATAHANDLE: {
            dmx_compat_select_data_flag *param = (dmx_compat_select_data_flag *)arg;
            dmx_capability cap = {0};
            hi_u32 valid_num = 0;

            if (hi_drv_dmx_get_capability(&cap) != HI_SUCCESS) {
                HI_ERR_DEMUX("get capability failed.\n");
                break;
            }

            if (param->watch_num <= cap.channel_num) {
                hi_u32 watch_chan_num = param->watch_num;
                hi_handle watch_chan[param->watch_num];
                hi_handle valid_chan[param->watch_num];

                if (memset_s(valid_chan, sizeof(valid_chan), 0, sizeof(valid_chan)) != HI_SUCCESS) {
                    HI_ERR_DEMUX("mem set failed.\n");
                    break;
                }

                if (osal_copy_from_user(watch_chan, compat_ptr(param->watch_array), sizeof(hi_handle) * watch_chan_num
)) {
                    HI_ERR_DEMUX("osal_copy_from_user failed.\n");
                    break;
                }

                ret = hi_drv_dmx_play_select_data_handle(watch_chan, watch_chan_num, valid_chan,
                                                         &valid_num, param->time_out_ms);
                if (ret == HI_SUCCESS) {
                    put_user(valid_num, (hi_u32 *)compat_ptr(param->valid_num));

                    if (compat_ptr(param->valid_channel) == HI_NULL) {
                        HI_ERR_DEMUX("user buffer is null!\n");
                        ret = HI_ERR_DMX_NULL_PTR;
                        break;
                    }

                    if (osal_copy_to_user(compat_ptr(param->valid_channel), valid_chan, sizeof(hi_handle) * valid_num)
) {
                        HI_ERR_DEMUX("osal_copy_to_user failed.\n");
                        ret = HI_FAILURE;
                    }
                }
            }else {
                HI_ERR_DEMUX("too many ch num to watch, %u > %u.\n", param->watch_num, cap.channel_num);
            }
            break;
        }

        case DMX_IOCTL_PLAYFCT_GETPIDCH: {
            dmx_play_fct_get_pid_ch_info *info = (dmx_play_fct_get_pid_ch_info *)arg;

            ret = hi_drv_dmx_play_get_pid_ch(info->handle, &info->pid_ch_handle);

            break;
        }

        case DMX_IOCTL_PLAYFCT_GET_BUFHANDLE: {
            dmx_play_get_buf_handle *info = (dmx_play_get_buf_handle *)arg;

            ret = hi_drv_dmx_play_get_bufhandle(info->handle, &info->buf_handle, &info->type);

            break;
        }

        case DMX_IOCTL_PLAYFCT_CC_REPEAT_SET: {
            dmx_set_chan_cc_repeat_info *info = (dmx_set_chan_cc_repeat_info *)arg;

            ret = hi_drv_dmx_play_cc_repeat_set(info->chan_handle, info->cc_repeat_mode);

            break;
        }

        case DMX_IOCTL_PLAYFCT_PUSI_SET: {
            dmx_pusi_set_info *info = (dmx_pusi_set_info *)arg;

            hi_drv_dmx_play_pusi_set(info->no_pusi);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}


static hi_s32 dmx_compat_ioctl_rec_fct_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case DMX_IOCTL_RECFCT_CREATE: {
            dmx_create_rec_fct_info *info = (dmx_create_rec_fct_info *)arg;

            ret = hi_drv_dmx_rec_create(&info->attrs, &info->handle, (struct dmx_session *)private_data);

            break;
        }

        case DMX_IOCTL_RECFCT_OPEN: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_open(*handle);

            break;
        }

        case DMX_IOCTL_RECFCT_GETATTRS: {
            dmx_get_rec_fct_attr_info *info = (dmx_get_rec_fct_attr_info *)arg;

            ret = hi_drv_dmx_rec_get_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RECFCT_SETATTRS: {
            dmx_set_rec_fct_attr_info *info = (dmx_set_rec_fct_attr_info *)arg;

            ret = hi_drv_dmx_rec_set_attrs(info->handle, &info->attrs);

            break;
        }

        case DMX_IOCTL_RECFCT_GETSTATUS: {
            dmx_get_rec_fct_status_info *info = (dmx_get_rec_fct_status_info *)arg;

            ret = hi_drv_dmx_rec_get_status(info->handle, &info->status);

            break;
        }

        case DMX_IOCTL_RECFCT_ADDCH: {
            dmx_rec_fct_add_del_ch_info *info = (dmx_rec_fct_add_del_ch_info *)arg;

            ret = hi_drv_dmx_rec_add_ch(info->handle, info->ch_handle);

            break;
        }

        case DMX_IOCTL_RECFCT_DELCH: {
            dmx_rec_fct_add_del_ch_info *info = (dmx_rec_fct_add_del_ch_info *)arg;

            ret = hi_drv_dmx_rec_del_ch(info->handle, info->ch_handle);

            break;
        }

        case DMX_IOCTL_RECFCT_DELALLCH: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_del_all_ch(*handle);

            break;
        }

        case DMX_COMPAT_IOCTL_RECFCT_ACQUIREBUF: {
            dmx_compat_rec_fct_acquire_buf_info *info = (dmx_compat_rec_fct_acquire_buf_info *)arg;
            dmx_buffer buf_tmp = {0};
            dmx_compat_buffer buf_compat_tmp;
            dmx_compat_buffer *user_compat_buffer = HI_NULL;

            ret = hi_drv_dmx_rec_acquire_buf(info->handle, info->req_len, info->time_out, &buf_tmp);
            if (ret == HI_SUCCESS) {
                buf_compat_tmp.data = ptr_to_compat(buf_tmp.data);
                buf_compat_tmp.phy_addr = buf_tmp.phy_addr;
                buf_compat_tmp.buf_handle = buf_tmp.buf_handle;
                buf_compat_tmp.offset     = buf_tmp.offset;
                buf_compat_tmp.length = buf_tmp.length;
                buf_compat_tmp.data_type = buf_tmp.data_type;
                buf_compat_tmp.pts      = buf_tmp.pts;

                user_compat_buffer = (dmx_compat_buffer *)compat_ptr(info->rec_fct_buf);
                if (osal_copy_to_user(user_compat_buffer, &buf_compat_tmp, sizeof(dmx_compat_buffer))) {
                    HI_ERR_DEMUX("copy data buf to user failed\n");
                    ret = HI_FAILURE;
                }
            }

            break;
        }

        case DMX_COMPAT_IOCTL_RECFCT_RELEASEBUF: {
            dmx_compat_rec_fct_release_buf_info *info = (dmx_compat_rec_fct_release_buf_info *)arg;
            dmx_buffer buf_tmp;
            dmx_compat_buffer buf_compat_tmp;
            dmx_compat_buffer *user_compat_buf = HI_NULL;

            user_compat_buf = (dmx_compat_buffer *)compat_ptr(info->rec_fct_buf);
            if (osal_copy_from_user(&buf_compat_tmp, user_compat_buf, sizeof(dmx_compat_buffer))) {
                HI_ERR_DEMUX("copy data buf from user failed\n");
            }

            buf_tmp.data = (hi_u8 *)compat_ptr(buf_compat_tmp.data);
            buf_tmp.phy_addr = buf_compat_tmp.phy_addr;
            buf_tmp.buf_handle = buf_compat_tmp.buf_handle;
            buf_tmp.offset     = buf_compat_tmp.offset;
            buf_tmp.length = buf_compat_tmp.length;
            buf_tmp.data_type   = buf_compat_tmp.data_type;
            buf_tmp.pts        = buf_compat_tmp.pts;

            ret = hi_drv_dmx_rec_release_buf(info->handle, &buf_tmp);

            break;
        }

        case DMX_COMPAT_IOCTL_RECFCT_RECVINDEX: {
            dmx_compat_rec_index_info *info = (dmx_compat_rec_index_info *)arg;
            dmx_index_data *index_tmp = HI_NULL;
            dmx_index_data *usr_index_tmp = HI_NULL;
            hi_u32 acquire_num = info->req_num;

            /* check the point */
            DMX_NULL_POINTER_BREAK(info->index_data);

            if (acquire_num <= DMX_MAX_REC_INDEX_NUM) {
                index_tmp = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_index_data) * acquire_num, GFP_KERNEL);
                if (index_tmp == HI_NULL) {
                    HI_FATAL_DEMUX("malloc failed.\n");
                    ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                    break;
                }
            } else {
                HI_FATAL_DEMUX("invalid acquire_num[%#x].\n", acquire_num);
                ret = HI_ERR_DMX_ALLOC_MEM_FAILED;
                break;
            }

            ret = hi_drv_dmx_rec_recv_idx(info->handle, info->req_num, info->time_out, &info->reqed_num, index_tmp);
            if (ret == HI_SUCCESS && info->reqed_num) {
                hi_s32 i;

                usr_index_tmp = (dmx_index_data *)compat_ptr(info->index_data);

                for (i = 0; i < info->reqed_num; i++) {
                    if (osal_copy_to_user(&usr_index_tmp[i], &index_tmp[i], sizeof(dmx_index_data))) {
                        HI_ERR_DEMUX("copy data buf to usr failed\n");
                        ret = HI_FAILURE;
                        break;
                    }
                }
            }

            HI_KFREE(HI_ID_DEMUX, index_tmp);
            break;
        }

        case DMX_IOCTL_RECFCT_PEEK_IDX_BUF: {
            dmx_rec_peek_idx_buf_info *info = (dmx_rec_peek_idx_buf_info *)arg;

            ret = hi_drv_dmx_rec_peek_idx_and_buf(info->handle, info->time_out, &info->index_num, &info->rec_data_len);

            break;
        }

        case DMX_IOCTL_RECFCT_CLOSE: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_close(*handle);

            break;
        }

        case DMX_IOCTL_RECFCT_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;

            ret = hi_drv_dmx_rec_destroy(*handle);

            break;
        }

        case DMX_COMPAT_IOCTL_RECFCT_PREMMAP: {
            hi_void *usr_addr = HI_NULL;
            dmx_compat_pre_mmap_info *info = (dmx_compat_pre_mmap_info *)arg;

            ret = hi_drv_dmx_rec_pre_mmap(info->handle, &info->buf_handle, &info->buf_size, &usr_addr);
            if (ret == HI_SUCCESS) {
                info->usr_addr = ptr_to_compat(usr_addr);
            }
            break;
        }

        case DMX_COMPAT_IOCTL_RECFCT_PSTMMAP: {
            dmx_compat_pst_mmap_info *info = (dmx_compat_pst_mmap_info *)arg;

            ret = hi_drv_dmx_rec_pst_mmap(info->handle, (hi_void *)compat_ptr(info->usr_addr));

            break;
        }

        case DMX_IOCTL_RECFCT_SETEOS: {
            dmx_rec_set_eos_info *info = (dmx_rec_set_eos_info *)arg;

            ret = hi_drv_dmx_rec_set_eos_flag(info->handle, info->eos_flag);

            break;
        }

        case DMX_IOCTL_RECFCT_GETBUFHANDLE: {
            dmx_rec_get_buf_handle *info = (dmx_rec_get_buf_handle *)arg;

            ret = hi_drv_dmx_rec_get_bufhandle(info->handle, &info->buf_handle);

            break;
        }

        default: {
            HI_ERR_DEMUX("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}
#endif

/* demux_trace begin */
#ifdef HI_DEMUX_PROC_SUPPORT

#define  ALL_TRACE       "all"
#define  GLB_TRACE       "glb"
#define  MGMT_TRACE      "mgmt"
#define  RAMPORT_TRACE   "ramport"
#define  PID_TRACE       "pid"
#define  BAND_TRACE      "band"

#define SAVE_ES_STR "save es"
#define SAVE_ALLTS_STR "save allts"
#define SAVE_IPTS_STR "save ipts"
#define SAVE_DMXTS_STR "save dmxts"
#define ES_STR "es"
#define ALLTS_STR "allts"
#define IPTS_STR "ipts"
#define DMXTS_STR "dmxts"
#define HELP_STR "help"
#define START_STR "start"
#define STOP_STR "stop"
#ifdef DEMUX_DUMP_PROC
#define ADDRESS_STR "address";
#endif

static hi_void dmx_debug_show_help(hi_void)
{
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- begin save es\n", SAVE_ES_STR, START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save es\n", SAVE_ES_STR, STOP_STR);
    hi_drv_proc_echo_helper("echo %s %s x[portid] > /proc/msp/demux_trace  -- begin save allts\n", SAVE_ALLTS_STR,
                            START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save allts\n", SAVE_ALLTS_STR, STOP_STR);
    hi_drv_proc_echo_helper("echo %s %s x[ram portid]> /proc/msp/demux_trace  -- begin save ram port ts\n",
                            SAVE_IPTS_STR, START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save ram port ts\n", SAVE_IPTS_STR, STOP_STR);
    hi_drv_proc_echo_helper("echo %s %s x[dmxid] > /proc/msp/demux_trace  -- begin save dmx ts\n",
                            SAVE_DMXTS_STR, START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save dmx ts\n", SAVE_DMXTS_STR, STOP_STR);
#ifdef DEMUX_DUMP_PROC
    hi_drv_proc_echo_helper("echo %s > /proc/msp/demux_dump_reg  -- Input address\n", ADDRESS_STR);
#endif
    hi_drv_proc_echo_helper("echo %s > /proc/msp/demux_trace  -- show help info\n", HELP_STR);
}

#define STRSKIPBLANK(str)      \
        while (str[0] == ' ') {\
            (str)++;\
        }\

static hi_s32 dmx_proc_trace_read(hi_void *p, hi_void *v)
{
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to start save es\n", SAVE_ES_STR, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop save es\n", SAVE_ES_STR, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s x[portid] > /proc/msp/demux_trace\" to start save all ts\n",
        SAVE_ALLTS_STR, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop save all ts\n", SAVE_ALLTS_STR, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s x[ram portid] > /proc/msp/demux_trace\" to start save ip ts\n",
        SAVE_IPTS_STR, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop save ip ts\n", SAVE_IPTS_STR, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s x[dmxid] > /proc/msp/demux_trace\" to start save dmx ts\n",
        SAVE_DMXTS_STR, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop save dmx ts\n", SAVE_DMXTS_STR, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to start all trace\n", ALL_TRACE, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop all trace\n", ALL_TRACE, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to start global trace\n", GLB_TRACE, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop global trace\n", GLB_TRACE, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to start mgmt trace\n", MGMT_TRACE, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop mgmt trace\n", MGMT_TRACE, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to start ramport trace\n",
        RAMPORT_TRACE, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop ramport trace\n", RAMPORT_TRACE, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to start pid trace\n", PID_TRACE, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop pid trace\n", PID_TRACE, STOP_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to start band trace\n", BAND_TRACE, START_STR);
    osal_proc_print(p, "type \"echo %s %s > /proc/msp/demux_trace\" to stop band trace\n", BAND_TRACE, STOP_STR);

    osal_proc_print(p, "type \"echo help > /proc/msp/demux_trace\" to get more help information\n");

    return HI_SUCCESS;
}

hi_s32 dmx_proc_write_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- begin save es\n", SAVE_ES_STR, START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save es\n", SAVE_ES_STR, STOP_STR);
    hi_drv_proc_echo_helper("echo %s %s x[portid] > /proc/msp/demux_trace  -- begin save allts\n", SAVE_ALLTS_STR,
                            START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save allts\n", SAVE_ALLTS_STR, STOP_STR);
    hi_drv_proc_echo_helper("echo %s %s x[ram portid]> /proc/msp/demux_trace  -- begin save ram port ts\n",
                            SAVE_IPTS_STR, START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save ram port ts\n", SAVE_IPTS_STR, STOP_STR);
    hi_drv_proc_echo_helper("echo %s %s x[dmxid] > /proc/msp/demux_trace  -- begin save dmx ts\n",
                            SAVE_DMXTS_STR, START_STR);
    hi_drv_proc_echo_helper("echo %s %s > /proc/msp/demux_trace  -- stop save dmx ts\n", SAVE_DMXTS_STR, STOP_STR);
#ifdef DEMUX_DUMP_PROC
    hi_drv_proc_echo_helper("echo %s > /proc/msp/demux_dump_reg  -- Input address\n", ADDRESS_STR);
#endif
    hi_drv_proc_echo_helper("echo %s > /proc/msp/demux_trace  -- show help info\n", HELP_STR);

    return HI_SUCCESS;
}

hi_s32 dmx_proc_write_save(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 cmd = 0xff;
    hi_u32 param = 0;
    hi_bool needparam = HI_FALSE;
    DMX_TRACE_CMD_CTRl cmdctrl;
    hi_char *endp = HI_NULL;

    if (argc < 0x3) {
        HI_ERR_DEMUX("invalid save command!\n");
        dmx_proc_write_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (argc > 0x4) {
        HI_ERR_DEMUX("command is not correct!\n");
        dmx_debug_show_help();
        return HI_FAILURE;;
    } else if (argc >= 0x3) {
        if (osal_strncmp(argv[1], strlen(ES_STR), ES_STR, strlen(ES_STR)) == 0) {
            cmd = DMX_TRACE_CMD_SAVE_ES;
            needparam = HI_FALSE;
        } else if (osal_strncmp(argv[1], strlen(ALLTS_STR), ALLTS_STR, strlen(ALLTS_STR)) == 0) {
            cmd = DMX_TRACE_CMD_SAVE_ALLTS;
            needparam = HI_TRUE;
        } else if (osal_strncmp(argv[1], strlen(IPTS_STR), IPTS_STR, strlen(IPTS_STR)) == 0) {
            cmd = DMX_TRACE_CMD_SAVE_IPTS;
            needparam = HI_TRUE;
        } else if (osal_strncmp(argv[1], strlen(DMXTS_STR), DMXTS_STR, strlen(DMXTS_STR)) == 0) {
            cmd = DMX_TRACE_CMD_SAVE_DMXTS;
            needparam = HI_TRUE;
        } else {
            HI_ERR_DEMUX("unknow command!\n");
            dmx_debug_show_help();
            return HI_FAILURE;;
        }

        if (argc == 0x4) {
            /* 10 means 10 hex */
            param = (hi_u32)osal_strtoul(argv[0x3], &endp, 10);
        }
    } else {
        HI_ERR_DEMUX("unknow command!\n");
        dmx_debug_show_help();
        return HI_FAILURE;;
    }

    if (osal_strncmp(argv[0x2], strlen(START_STR), START_STR, strlen(START_STR)) == 0) {
        cmdctrl = DMX_TRACE_CMD_START;
    } else if (osal_strncmp(argv[0x2], strlen(STOP_STR), STOP_STR, strlen(STOP_STR)) == 0) {
        cmdctrl = DMX_TRACE_CMD_STOP;
        needparam = HI_FALSE;
    } else {
        HI_ERR_DEMUX("command is not correct!\n");
        dmx_debug_show_help();
        return HI_FAILURE;
    }

    ret = drv_dmx_osr_debug_ctrl(cmd, cmdctrl, param);
    if (ret != HI_SUCCESS) {
        HI_PRINT("command is not correct!\n");
        dmx_debug_show_help();
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 dmx_proc_write_other(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 cmd = 0xff;
    DMX_TRACE_CMD_CTRl cmdctrl;

    if (argc != 0x2) {
        HI_ERR_DEMUX("unknown command!\n");
        dmx_proc_write_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[0], strlen(ALL_TRACE), ALL_TRACE, strlen(ALL_TRACE)) == 0) {
        cmd = DMX_TRACE_CMD_ALL_TRACE;
    } else if (osal_strncmp(argv[0], strlen(GLB_TRACE), GLB_TRACE, strlen(GLB_TRACE)) == 0) {
        cmd = DMX_TRACE_CMD_GLB_TRACE;
    } else if (osal_strncmp(argv[0], strlen(MGMT_TRACE), MGMT_TRACE, strlen(MGMT_TRACE)) == 0) {
        cmd = DMX_TRACE_CMD_MGMT_TRACE;
    } else if (osal_strncmp(argv[0], strlen(RAMPORT_TRACE), RAMPORT_TRACE, strlen(RAMPORT_TRACE)) == 0) {
        cmd = DMX_TRACE_CMD_RAMPORT_TRACE;
    } else if (osal_strncmp(argv[0], strlen(PID_TRACE), PID_TRACE, strlen(PID_TRACE)) == 0) {
        cmd = DMX_TRACE_CMD_PID_TRACE;
    } else if (osal_strncmp(argv[0], strlen(BAND_TRACE), BAND_TRACE, strlen(BAND_TRACE)) == 0) {
        cmd = DMX_TRACE_CMD_BAND_TRACE;
    } else {
        HI_ERR_DEMUX("command is not correct!\n");
        dmx_debug_show_help();
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen(START_STR), START_STR, strlen(START_STR)) == 0) {
        cmdctrl = DMX_TRACE_CMD_START;
    } else if (osal_strncmp(argv[1], strlen(STOP_STR), STOP_STR, strlen(STOP_STR)) == 0) {
        cmdctrl = DMX_TRACE_CMD_STOP;
    } else {
        HI_ERR_DEMUX("command is not correct!\n");
        dmx_debug_show_help();
        return HI_FAILURE;
    }

    ret = drv_dmx_osr_debug_ctrl(DMX_TRACE_CMD_ALL_TRACE, cmdctrl, 0);
    if (ret != HI_SUCCESS) {
        HI_PRINT("command is not correct!\n");
        dmx_debug_show_help();
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef DEMUX_DUMP_PROC
hi_s32 dmx_proc_write_address(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    return HI_SUCCESS;
}
#endif

static osal_proc_cmd g_dmx_proc_cmd[] = {
    {"help", dmx_proc_write_help},
    {"save", dmx_proc_write_save},
    {"all", dmx_proc_write_other},
    {"glb", dmx_proc_write_other},
    {"mgmt", dmx_proc_write_other},
    {"ramport", dmx_proc_write_other},
    {"pid", dmx_proc_write_other},
    {"band", dmx_proc_write_other},
#ifdef DEMUX_DUMP_PROC
    {"address", dmx_proc_write_address}
#endif
};
#endif

static hi_s32 demux_suspend(hi_void *private_data)
{
    hi_s32 ret;

    ret = hi_drv_dmx_suspend();
    if (ret == HI_SUCCESS) {
        HI_PRINT("demux suspend ok.\n");
    }

    return ret;
}

static hi_s32 demux_resume(hi_void *private_data)
{
    hi_s32 ret;

    ret = hi_drv_dmx_resume();
    if (ret == HI_SUCCESS) {
        HI_PRINT("demux resume ok.\n");
    }

    return ret;
}

static osal_ioctl_cmd g_dmx_ioctl_info[] = {
    {DMX_IOCTL_GLB_GETCAP,             dmx_ioctl_glb_impl},
    {DMX_IOCTL_GLB_GET_PACKETNUM,      dmx_ioctl_glb_impl},

    {DMX_IOCTL_IF_GETPORTATTRS,      dmx_ioctl_general_port_impl},
    {DMX_IOCTL_IF_SETPORTATTRS,      dmx_ioctl_general_port_impl},
    {DMX_IOCTL_TSI_GETPORTATTRS,     dmx_ioctl_general_port_impl},
    {DMX_IOCTL_TSI_SETPORTATTRS,     dmx_ioctl_general_port_impl},
    {DMX_IOCTL_TAG_GETPORTATTRS,     dmx_ioctl_general_port_impl},
    {DMX_IOCTL_TAG_SETPORTATTRS,     dmx_ioctl_general_port_impl},
    {DMX_IOCTL_TSO_GETPORTATTRS,     dmx_ioctl_general_port_impl},
    {DMX_IOCTL_TSO_SETPORTATTRS,     dmx_ioctl_general_port_impl},

    {DMX_IOCTL_RAM_OPENPORT,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_GETPORTATTRS,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_SETPORTATTRS,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_GETPORTSTATUS,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_GETBUFFER,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_PUSHBUFFER,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_PUTBUFFER,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_RELEASEBUFFER,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_FLUSHBUFFER,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_RESETBUFFER,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_CLOSEPORT,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_PREMMAP,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_PSTMMAP,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_GETPORTID,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_GETPORTHANDLE,      dmx_ioctl_ram_port_impl},
    {DMX_IOCTL_RAM_GETBUFHANDLE,      dmx_ioctl_ram_port_impl},

    {DMX_IOCTL_BAND_OPEN,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_REFINC,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_ATTACHPORT,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_DETACHPORT,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_GETATTRS,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_SETATTRS,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_GETSTATUS,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_CLOSE,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_REFDEC,      dmx_ioctl_band_impl},
    {DMX_IOCTL_BAND_TEI_SET,      dmx_ioctl_band_impl},

    {DMX_IOCTL_RMXFCT_CREATE,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_OPEN,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_GETATTRS,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_SETATTRS,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_GETSTATUS,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_ADDPUMP,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_DELPUMP,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_DELALLPUMP,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_GETPUMPATTRS,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_SETPUMPATTRS,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_CLOSE,      dmx_ioctl_rmx_impl},
    {DMX_IOCTL_RMXFCT_DESTROY,      dmx_ioctl_rmx_impl},

    {DMX_IOCTL_PIDCHANNEL_CREATE,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_REFINC,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_GETHANDLE,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_GETSTATUS,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_LOCKOUT,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_UNLOCKOUT,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_DESTROY,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_REFDEC,      dmx_ioctl_pid_ch_impl},
    {DMX_IOCTL_PIDCHANNEL_GETFREECNT,      dmx_ioctl_pid_ch_impl},

    {DMX_IOCTL_PLAYFCT_CREATE,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_OPEN,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GETATTRS,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_SETATTRS,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GETSTATUS,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GET_SCRAMBLEDFLAG,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GETPACKET,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_ATTACHPIDCH,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_DETACHPIDCH,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_CREATEFILTER,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_ADDFILTER,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_DELFILTER,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_DELALLFILTER,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_UPDATEFILTER,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GETFILTER,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GETHANDLE,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GET_FREEFLT_CNT,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_DESTROYFILTER,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_ACQUIREBUF,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_RELEASEBUF,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_RESETBUF,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_STARTINDEX,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_RECVINDEX,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_STOPINDEX,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_CLOSE,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_DESTROY,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GET_DATAHANDLE,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_PREMMAP,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_PSTMMAP,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_SETEOS,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_SELECT_DATAHANDLE,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GETPIDCH,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_GET_BUFHANDLE,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_CC_REPEAT_SET,      dmx_ioctl_play_fct_impl},
    {DMX_IOCTL_PLAYFCT_PUSI_SET,      dmx_ioctl_play_fct_impl},

    {DMX_IOCTL_RECFCT_CREATE,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_OPEN,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_GETATTRS,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_SETATTRS,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_GETSTATUS,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_ADDCH,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_DELCH,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_DELALLCH,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_ACQUIREBUF,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_RELEASEBUF,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_RECVINDEX,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_PEEK_IDX_BUF,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_CLOSE,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_DESTROY,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_PREMMAP,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_PSTMMAP,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_SETEOS,      dmx_ioctl_rec_fct_impl},
    {DMX_IOCTL_RECFCT_GETBUFHANDLE,      dmx_ioctl_rec_fct_impl},

    {DMX_IOCTL_DSCFCT_CREATE,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_GETATTRS,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_SETATTRS,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_ATTACH,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_DETACH,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_ATTACH_KEYSLOT,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_DETACH_KEYSLOT,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_GET_KS_HANDLE,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_SETEVENIV,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_SETODDIV,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_SETEVENKEY,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_SETODDKEY,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_SETSYSKEY,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_DESTROY,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_GET_KEY_HANDLE,      dmx_ioctl_dsc_fct_impl},
    {DMX_IOCTL_DSCFCT_GET_FREE_CNT,      dmx_ioctl_dsc_fct_impl},

    {DMX_IOCTL_PCRFCT_CREATE,      dmx_ioctl_pcr_fct_impl},
    {DMX_IOCTL_PCRFCT_ATTACH,      dmx_ioctl_pcr_fct_impl},
    {DMX_IOCTL_PCRFCT_DETACH,      dmx_ioctl_pcr_fct_impl},
    {DMX_IOCTL_PCRFCT_GETSTATUS,      dmx_ioctl_pcr_fct_impl},
    {DMX_IOCTL_PCRFCT_GETPCRSCR,      dmx_ioctl_pcr_fct_impl},
    {DMX_IOCTL_PCRFCT_DESTROY,      dmx_ioctl_pcr_fct_impl},
#ifdef CONFIG_COMPAT
    {DMX_COMPAT_IOCTL_RAM_GETBUFFER,     dmx_compat_ioctl_ram_port_impl},
    {DMX_COMPAT_IOCTL_RAM_PUSHBUFFER,    dmx_compat_ioctl_ram_port_impl},
    {DMX_COMPAT_IOCTL_RAM_PUTBUFFER,     dmx_compat_ioctl_ram_port_impl},
    {DMX_COMPAT_IOCTL_RAM_RELEASEBUFFER, dmx_compat_ioctl_ram_port_impl},
    {DMX_COMPAT_IOCTL_RAM_PREMMAP,       dmx_compat_ioctl_ram_port_impl},
    {DMX_COMPAT_IOCTL_RAM_PSTMMAP,       dmx_compat_ioctl_ram_port_impl},
    {DMX_COMPAT_IOCTL_PLAYFCT_ACQUIREBUF,      dmx_compat_ioctl_play_fct_impl},
    {DMX_COMPAT_IOCTL_PLAYFCT_RELEASEBUF,      dmx_compat_ioctl_play_fct_impl},
    {DMX_COMPAT_IOCTL_PLAYFCT_GET_DATAHANDLE,      dmx_compat_ioctl_play_fct_impl},
    {DMX_COMPAT_IOCTL_PLAYFCT_PREMMAP,      dmx_compat_ioctl_play_fct_impl},
    {DMX_COMPAT_IOCTL_PLAYFCT_PSTMMAP,      dmx_compat_ioctl_play_fct_impl},
    {DMX_COMPAT_IOCTL_PLAYFCT_SELECT_DATAHANDLE,      dmx_compat_ioctl_play_fct_impl},
    {DMX_COMPAT_IOCTL_RECFCT_ACQUIREBUF,      dmx_compat_ioctl_rec_fct_impl},
    {DMX_COMPAT_IOCTL_RECFCT_RELEASEBUF,      dmx_compat_ioctl_rec_fct_impl},
    {DMX_COMPAT_IOCTL_RECFCT_RECVINDEX,      dmx_compat_ioctl_rec_fct_impl},
    {DMX_COMPAT_IOCTL_RECFCT_PREMMAP,      dmx_compat_ioctl_rec_fct_impl},
    {DMX_COMPAT_IOCTL_RECFCT_PSTMMAP,      dmx_compat_ioctl_rec_fct_impl},
#endif
};

static demux_func_export g_dmx_export_func = {
    .dmx_get_capability = hi_drv_dmx_get_capability,
    .dmx_get_tuner_port_pkt_cnt = HI_NULL,
    .dmx_set_callback = hi_drv_dmx_set_callback,
};

static osal_fileops g_dmx_fops = {
    .open           = dmx_open,
    .release        = dmx_release,
    .cmd_list       = g_dmx_ioctl_info,
    .cmd_cnt        = sizeof(g_dmx_ioctl_info) / sizeof(g_dmx_ioctl_info[0]),
};

static osal_pmops g_demux_pm_ops = {
    .pm_suspend        = demux_suspend,
    .pm_resume         = demux_resume,
};

static osal_dev g_demux_dev = {
    .name = HI_DEV_DEMUX_NAME,
    .minor = HI_DEV_DEMUX_MINOR,
    .fops = &g_dmx_fops,
    .pmops = &g_demux_pm_ops,

};

static hi_s32 dmx_register_dev(hi_void)
{
    hi_s32 ret;

    ret = osal_dev_register(&g_demux_dev);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("osal_dev_register failed\n");
        goto out;
    }

    ret = osal_exportfunc_register(HI_ID_DEMUX, DEMUX_NAME, (hi_void *)&g_dmx_export_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("register demux module failed\n");
        goto out1;
    }

    return ret;

out1:
    osal_dev_unregister(&g_demux_dev);
out:
    return ret;
}

static hi_void dmx_unregister_dev(hi_void)
{
    osal_exportfunc_unregister(HI_ID_DEMUX);

    osal_dev_unregister(&g_demux_dev);
}

hi_s32 dmx_mod_init(hi_void)
{
    hi_s32 ret;
    osal_proc_entry *item;

    ret = dmx_register_dev();
    if (ret != HI_SUCCESS) {
        HI_FATAL_DEMUX("register demux device failed\n");
        goto out;
    }

#ifdef HI_DEMUX_PROC_SUPPORT
    item = osal_proc_add("demux_trace", strlen("demux_trace"));
    if (item == NULL) {
        HI_ERR_DEMUX("add proc demux_trace failed\n");
    } else {
        item->read = dmx_proc_trace_read;
        item->cmd_list = g_dmx_proc_cmd;
        item->cmd_cnt = sizeof(g_dmx_proc_cmd) / sizeof(g_dmx_proc_cmd[0]);
    }
#endif

    /* configure the hardware clock */
    dmx_hal_init_hw();

    /* mgmt init */
    ret = dmx_mgmt_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dmx_mgmt_init failed\n");
        goto out;
    }

    ret = dmx_slot_table_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("dmx_mgmt_init failed\n");
        goto out;
    }

    /* open tee demux */
    dmx_teec_open();

#ifdef MODULE
    HI_PRINT("Load hi_demux.ko success.  \t(%s)\n", VERSION_STRING);
#endif

out:
    return HI_SUCCESS;
}

hi_void dmx_mod_exit(hi_void)
{
    dmx_play_fct_stop_save_es();
    dmx_ram_port_stop_save_ip_ts();
    dmx_rec_fct_stop_save_all_ts();
    dmx_rec_fct_stop_save_dmx_ts();

    /* close tee demux */
    dmx_teec_close();

    dmx_slot_table_destory();
    /* mgmt exit */
    dmx_mgmt_exit();

    osal_proc_remove("demux_trace", strlen("demux_trace"));

    dmx_unregister_dev();

    /* deconfigure the hardware clock */
    dmx_hal_deinit_hw();

    HI_PRINT("remove hi_demux.ko success.\n");
}

#ifdef MODULE
module_init(dmx_mod_init);
module_exit(dmx_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
