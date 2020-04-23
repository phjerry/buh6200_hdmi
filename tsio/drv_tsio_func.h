/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio basic function impl.
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_TSIO_FUNCTION_H__
#define __DRV_TSIO_FUNCTION_H__

#include "drv_tsio_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    TSIO_LIVE_PORT_IF,
    TSIO_LIVE_PORT_TSI,

    TSIO_LIVE_PORT_MAX
} tsio_live_port_type;

hi_s32 tsio_tsi_open_port(tsio_port port, const tsio_tsi_port_attrs *attrs, hi_handle *handle,
                          struct tsio_session *session);
hi_s32 tsio_tsi_get_port_attrs(hi_handle handle, tsio_tsi_port_attrs *attrs);
hi_s32 tsio_tsi_set_port_attrs(hi_handle handle, const tsio_tsi_port_attrs *attrs);
hi_s32 tsio_tsi_get_port_status(hi_handle handle, tsio_tsi_port_status *status);
hi_s32 tsio_tsi_close_port(hi_handle handle);

hi_s32 tsio_ram_open_port(tsio_port port, const tsio_ram_port_attrs_ex *attrs, hi_handle *handle,
                          struct tsio_session *session);
hi_s32 tsio_ram_get_port_attrs(hi_handle handle, tsio_ram_port_attrs *attrs);
hi_s32 tsio_ram_set_port_attrs(hi_handle handle, const tsio_ram_port_attrs_ex *attrs);
hi_s32 tsio_ram_get_port_status(hi_handle handle, tsio_ram_port_status *status);
hi_s32 tsio_ram_begin_bulk_seg(hi_handle handle, tsio_key_type key, const hi_u8 *IV);
hi_s32 tsio_ram_end_bulk_seg(hi_handle handle);
hi_s32 tsio_ram_get_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out);
hi_s32 tsio_ram_push_buffer(hi_handle handle, tsio_buffer *buf);
hi_s32 tsio_ram_put_buffer(hi_handle handle, tsio_buffer *buf);
hi_s32 tsio_ram_reset_buffer(hi_handle handle);
hi_s32 tsio_ram_close_port(hi_handle handle);
hi_s32 tsio_ram_pre_mmap(hi_handle handle, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr);
hi_s32 tsio_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);

hi_s32 tsio_pid_create_channel(hi_handle pt_handle, hi_u32 pid, hi_handle *handle, struct tsio_session *session);
hi_s32 tsio_pid_get_channel_status(hi_handle handle, tsio_pid_channel_status *status);
hi_s32 tsio_pid_destroy_channel(hi_handle handle);

hi_s32 tsio_raw_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session);
hi_s32 tsio_raw_destroy_channel(hi_handle handle);

hi_s32 tsio_sp_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session);
hi_s32 tsio_sp_destroy_channel(hi_handle handle);

hi_s32 tsio_ivr_create_channel(hi_handle pt_handle, hi_handle *handle, struct tsio_session *session);
hi_s32 tsio_ivr_destroy_channel(hi_handle handle);

hi_s32 tsio_se_create(tsio_sid sid, const tsio_secure_engine_attrs *attrs, hi_handle *handle,
                      struct tsio_session *session);
hi_s32 tsio_se_open(hi_handle handle);
hi_s32 tsio_se_get_attrs(hi_handle handle, tsio_secure_engine_attrs *attrs);
hi_s32 tsio_se_set_attrs(hi_handle handle, const tsio_secure_engine_attrs *attrs);
hi_s32 tsio_se_get_status(hi_handle handle, tsio_secure_engine_status *status);
hi_s32 tsio_se_add_channel(hi_handle handle, hi_handle ch_handle);
hi_s32 tsio_se_del_channel(hi_handle handle, hi_handle ch_handle);
hi_s32 tsio_se_del_all_channels(hi_handle handle);
hi_s32 tsio_se_acquire_buffer(hi_handle handle, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out);
hi_s32 tsio_se_release_buffer(hi_handle handle, const tsio_buffer *buf);
hi_s32 tsio_se_close(hi_handle handle);
hi_s32 tsio_se_destroy(hi_handle handle);
hi_s32 tsio_se_pre_mmap(hi_handle handle, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr);
hi_s32 tsio_se_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);

hi_s32 tsio_cc_send(const hi_uchar *cmd, hi_u32 cmd_len, hi_uchar *resp, hi_u32 *resp_len, hi_u32 time_out);

hi_s32  tsio_mgmt_init(hi_void);
hi_void tsio_mgmt_exit(hi_void) ;

hi_s32  tsio_mgmt_get_config(tsio_config *config);
hi_s32  tsio_mgmt_set_config(const tsio_config *config);

hi_s32  tsio_mgmt_get_cap(tsio_capability *cap);
hi_s32  tsio_mgmt_get_out_of_sync_cnt(hi_u32 *cnt);

hi_s32  tsio_mgmt_suspend(hi_void);
hi_s32  tsio_mgmt_resume(hi_void);

hi_s32  tsio_mgmt_create_tsi_port(tsio_port port, const tsio_tsi_port_attrs *attrs, struct tsio_r_tsi_port **tsi_port);
hi_s32  tsio_mgmt_destroy_tsi_port(struct tsio_r_base *obj);
hi_s32  tsio_mgmt_create_ram_port(tsio_port port, const tsio_ram_port_attrs_ex *attrs,
                                  struct tsio_r_ram_port **ram_port);
hi_s32  tsio_mgmt_destroy_ram_port(struct tsio_r_base *obj);
hi_s32  tsio_mgmt_create_pid_channel(struct tsio_r_base *port, hi_u32 pid, struct tsio_r_pid_channel **pid_channel);
hi_s32  tsio_mgmt_destroy_pid_channel(struct tsio_r_base *obj);
hi_s32  tsio_mgmt_create_raw_channel(struct tsio_r_base *port, struct tsio_r_raw_channel **raw_channel);
hi_s32  tsio_mgmt_destroy_raw_channel(struct tsio_r_base *obj);
hi_s32  tsio_mgmt_create_sp_channel(struct tsio_r_base *port, struct tsio_r_sp_channel **sp_channel);
hi_s32  tsio_mgmt_destroy_sp_channel(struct tsio_r_base *obj);
hi_s32  tsio_mgmt_create_ivr_channel(struct tsio_r_base *port, struct tsio_r_ivr_channel **ivr_channel);
hi_s32  tsio_mgmt_destroy_ivr_channel(struct tsio_r_base *obj);
hi_s32  tsio_mgmt_create_se(tsio_sid sid, const tsio_secure_engine_attrs *attrs, struct tsio_r_se **se);
hi_s32  tsio_mgmt_destroy_se(struct tsio_r_base *obj);

struct tsio_mgmt *__get_tsio_mgmt(hi_void);
struct tsio_mgmt *get_tsio_mgmt(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_TSIO_FUNCTION_H__

