/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio kernel export interface definition.
* Author: sdk
* Create: 2016-08-12
*/

#ifndef __HI_DRV_TSIO_H__
#define __HI_DRV_TSIO_H__

#include "hi_type.h"
#include "drv_tsio_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hi_s32 (*bp_tsio_port)(hi_u32 dmx_id, hi_u32 fq_id, hi_u32 dmx_tsio_port_id, hi_s32(*op)(hi_u32 dmx_id, hi_u32 dmx_tsio_port_id, hi_u32 fq_id, hi_u32 tsio_ram_port_id));

typedef struct {
    bp_tsio_port dmx_fq_bp_tsio_port;
} tsio_export_func_s;

hi_s32 hi_drv_tsio_get_capability(tsio_capability *cap);
hi_s32 hi_drv_tsio_get_out_of_sync_cnt(hi_u32 *cnt);

struct tsio_session;
hi_s32 hi_drv_tsio_create_session(struct tsio_session **new_session);
hi_s32 hi_drv_tsio_destroy_session(struct tsio_session *session);

hi_s32 hi_drv_tsio_get_config(tsio_config *config);
hi_s32 hi_drv_tsio_set_config(const tsio_config *config);

hi_s32 hi_drv_tsio_tsi_open_port(tsio_port port, const tsio_tsi_port_attrs *attrs, hi_handle *handle, struct tsio_session *session);
hi_s32 hi_drv_tsio_tsi_get_port_attrs(hi_handle handle, tsio_tsi_port_attrs *attrs);
hi_s32 hi_drv_tsio_tsi_set_port_attrs(hi_handle handle, const tsio_tsi_port_attrs *attrs);
hi_s32 hi_drv_tsio_tsi_get_port_status(hi_handle handle, tsio_tsi_port_status *status);
hi_s32 hi_drv_tsio_tsi_close_port(hi_handle handle);

hi_s32 hi_drv_tsio_ram_open_port(tsio_port port, const tsio_ram_port_attrs_ex *attrs, hi_handle *handle, struct tsio_session *session);
hi_s32 hi_drv_tsio_ram_get_port_attrs(hi_handle handle, tsio_ram_port_attrs *attrs);
hi_s32 hi_drv_tsio_ram_set_port_attrs(hi_handle handle, const tsio_ram_port_attrs_ex *attrs);
hi_s32 hi_drv_tsio_ram_get_port_status(hi_handle handle, tsio_ram_port_status *status);
hi_s32 hi_drv_tsio_ram_begin_bulk_seg(hi_handle handle, tsio_key_type key, const hi_u8 *IV);
hi_s32 hi_drv_tsio_ram_end_bulk_seg(hi_handle handle);
hi_s32 hi_drv_tsio_ram_get_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out/* ms */);
hi_s32 hi_drv_tsio_ram_push_buffer(hi_handle handle, tsio_buffer *buf);
hi_s32 hi_drv_tsio_ram_put_buffer(hi_handle handle, tsio_buffer *buf);
hi_s32 hi_drv_tsio_ram_reset_buffer(hi_handle handle);
hi_s32 hi_drv_tsio_ram_close_port(hi_handle handle);
hi_s32 hi_drv_tsio_ram_pre_mmap(hi_handle handle, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr);
hi_s32 hi_drv_tsio_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);

hi_s32 hi_drv_tsio_pid_create_channel(hi_handle pt_handle, hi_u32 pid, hi_handle *handle, struct tsio_session *session);
hi_s32 hi_drv_tsio_pid_get_channel_status(hi_handle handle, tsio_pid_channel_status *status);
hi_s32 hi_drv_tsio_pid_destroy_channel(hi_handle handle);

hi_s32 hi_drv_tsio_raw_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session);
hi_s32 hi_drv_tsio_raw_destroy_channel(hi_handle handle);

hi_s32 hi_drv_tsio_sp_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session);
hi_s32 hi_drv_tsio_sp_destroy_channel(hi_handle handle);

hi_s32 hi_drv_tsio_ivr_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session);
hi_s32 hi_drv_tsio_ivr_destroy_channel(hi_handle handle);

hi_s32 hi_drv_tsio_se_create(tsio_sid sid, const tsio_secure_engine_attrs *attrs,
                                       hi_handle *handle, struct tsio_session *session);
hi_s32 hi_drv_tsio_se_open(hi_handle handle);
hi_s32 hi_drv_tsio_se_get_attrs(hi_handle handle, tsio_secure_engine_attrs *attrs);
hi_s32 hi_drv_tsio_se_set_attrs(hi_handle handle, const tsio_secure_engine_attrs *attrs);
hi_s32 hi_drv_tsio_se_get_status(hi_handle handle, tsio_secure_engine_status *status);
hi_s32 hi_drv_tsio_se_add_channel(hi_handle handle, hi_handle ch_handle);
hi_s32 hi_drv_tsio_se_del_channel(hi_handle handle, hi_handle ch_handle);
hi_s32 hi_drv_tsio_se_del_all_channels(hi_handle handle);
hi_s32 hi_drv_tsio_se_acquire_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out);
hi_s32 hi_drv_tsio_se_release_buffer(hi_handle handle, const tsio_buffer *buf);
hi_s32 hi_drv_tsio_se_close(hi_handle handle);
hi_s32 hi_drv_tsio_se_destroy(hi_handle handle);
hi_s32 hi_drv_tsio_se_pre_mmap(hi_handle handle, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr);
hi_s32 hi_drv_tsio_se_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);

hi_s32 hi_drv_tsio_cc_send(const hi_uchar *cmd, hi_u32 cmd_len, hi_uchar *resp, hi_u32 *resp_len, hi_u32 time_out);

hi_s32 hi_drv_tsio_suspend(hi_void);
hi_s32 hi_drv_tsio_resume(hi_void);

hi_s32 tsio_mod_init(hi_void);
hi_void tsio_mod_exit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __HI_DRV_TSIO_H__