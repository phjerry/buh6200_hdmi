/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function decl.
 * Author: sdk
 * Create: 2017-06-05
 */
#ifndef __DRV_DEMUX_TEE_H__
#define __DRV_DEMUX_TEE_H__

#include "hi_osal.h"
#include "hi_drv_demux.h"
#include "drv_demux_index.h"

#ifdef DMX_TEE_SUPPORT
#include "teek_client_type.h"
#include "teek_client_api.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * global dmx resource management.
 */
typedef struct {
    hi_handle buf_handle;
    hi_u32  buf_size;
    hi_u32  flush_buf_size;
    hi_u32  dsc_buf_size;

    hi_u64  buf_phy_addr;
    hi_u64  flush_buf_phy_addr;
    hi_u64  dsc_buf_phy_addr;
} dmx_tee_ramport_info;

typedef struct {
    hi_u64 buf_phy_addr;
    hi_u32 buf_len;
    hi_bool desep;
    hi_bool flush_flag;
    hi_bool sync_data_flag;
    hi_u32  write_index;
} dmx_tee_ramport_dsc;

typedef struct {
    hi_handle handle;
    hi_u32  buf_id;
    hi_u32  buf_size;
    hi_u64  buf_phy_addr;
} dmx_tee_mem_info;

typedef struct {
    hi_u32  chan_id;
    hi_bool is_descram;
    hi_bool is_video_index;
    hi_u32  index_src_pid;
    hi_u32  raw_pidch_id;
    hi_u32  master_raw_pidch_id;
} dmx_tee_rec_attach_info;

typedef struct {
    hi_u32  chan_id;
    hi_bool is_rec_only;
    hi_bool is_descram;
    hi_bool is_video_index;
    hi_u32  index_src_pid;
    hi_u32  raw_pidch_id;
} dmx_tee_rec_detach_info;

#ifdef DMX_TEE_SUPPORT
#define TEEC_CMD_INIT                      0
#define TEEC_CMD_DEINIT                    1
#define TEEC_CMD_CREATE_RAMPORT            2
#define TEEC_CMD_DESTROY_RAMPORT           3
#define TEEC_CMD_SET_RAMPORT_DSC           4
#define TEEC_CMD_CREATE_PLAY_CHAN          5
#define TEEC_CMD_DESTROY_PLAY_CHAN         6
#define TEEC_CMD_ATTACH_PLAY_CHAN          7
#define TEEC_CMD_DETACH_PLAY_CHAN          8
#define TEEC_CMD_CREATE_REC_CHAN           9
#define TEEC_CMD_DESTROY_REC_CHAN          10
#define TEEC_CMD_ATTACH_REC_CHAN           11
#define TEEC_CMD_DETACH_REC_CHAN           12
#define TEEC_CMD_UPDATE_PLAY_READ_IDX      13
#define TEEC_CMD_UPDATE_REC_READ_IDX       14
#define TEEC_CMD_ACQUIRE_SECBUF_ID         15
#define TEEC_CMD_RELEASE_SECBUF_ID         16
#define TEEC_CMD_DETACH_RAW_PIDCH          17
#define TEEC_CMD_FIXUP_HEVC_INDEX          18
#define TEEC_CMD_CONFIG_SECBUF             19
#define TEEC_CMD_DECONFIG_SECBUF           20

struct dmx_r_teec {
    TEEC_Context context;
    TEEC_Session session;
    hi_bool  connected;
    osal_mutex   lock;
};

typedef struct {
    hi_u32 magic      : 16;
    hi_u32 head_size  : 16;
    hi_u8  version[32]; /* size 32 */
} dmx_ree_tee_version;

hi_s32  dmx_teec_init(struct dmx_r_teec *rteec);

hi_void dmx_teec_deinit(struct dmx_r_teec *rteec);

hi_void dmx_teec_open(hi_void);

hi_void dmx_teec_close(hi_void);

hi_s32  dmx_mgmt_send_cmd_to_ta(hi_u32 cmd_id, TEEC_Operation *operation, hi_u32 *ret_origin);

hi_s32  dmx_tee_create_ramport(hi_u32 ram_id, hi_u32 buf_size, hi_u32 flush_buf_size, hi_u32 dsc_buf_size,
    dmx_tee_ramport_info *ramport_info_ptr);

hi_void dmx_tee_destroy_ramport(hi_u32 ram_id, const dmx_tee_ramport_info *ramport_info_ptr);

hi_s32 dmx_tee_set_ramport_dsc(hi_u32 ram_id, const dmx_tee_ramport_dsc *ram_port_dsc);

hi_s32  dmx_tee_create_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 buf_size,
    dmx_tee_mem_info *buf_info_ptr);

hi_void dmx_tee_destroy_play_chan(hi_u32 chan_id, dmx_play_type play_type, const dmx_tee_mem_info *buf_info_ptr);

hi_s32  dmx_tee_attach_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 raw_pidch_id,
    hi_u32 master_raw_pidch_id);

hi_void dmx_tee_detach_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 raw_pidch_id);

hi_s32  dmx_tee_create_rec_chan(hi_u32 chan_id, hi_u32 buf_size, dmx_tee_mem_info *buf_info_ptr);

hi_void dmx_tee_destroy_rec_chan(hi_u32 chan_id, const dmx_tee_mem_info *buf_info_ptr);

hi_s32  dmx_tee_attach_rec_chan(const dmx_tee_rec_attach_info *rec_attach_info);

hi_void dmx_tee_detach_rec_chan(const dmx_tee_rec_detach_info *rec_detach_info);

hi_s32  dmx_tee_update_play_buf_read_idx(hi_u32 buf_id, dmx_play_type play_type, hi_u32 read_idx);

hi_s32  dmx_tee_update_rec_buf_read_idx(hi_u32 buf_id, hi_u32 read_idx);

hi_s32  dmx_tee_acquire_bufid(hi_u32 *bufid);

hi_void dmx_tee_release_bufid(const hi_u32 bufid);
hi_s32  dmx_tee_config_buf(hi_u32 chan_id, dmx_play_type chan_type);
hi_void dmx_tee_deconfig_buf(hi_u32 chan_id, dmx_play_type chan_type);

hi_void dmx_tee_detach_raw_pidch(hi_u32 raw_pidch);

hi_s32 dmx_tee_fixup_hevc_es_index(hi_u32 rec_id, hi_u32 idx_pid, hi_u32 parse_offsec,
    const findex_scd *pst_fidx, dmx_index_data *cur_frame);
#else
typedef struct {} TEEC_Operation;
struct dmx_r_teec {
    osal_mutex lock;
};

static inline hi_s32 dmx_teec_init(struct dmx_r_teec *rteec)
{
    return HI_SUCCESS;
}
static inline hi_void dmx_teec_deinit(struct dmx_r_teec *rteec) {}

static inline hi_void dmx_teec_open(hi_void) {}

static inline hi_void dmx_teec_close(hi_void) {}

static inline hi_s32 dmx_mgmt_send_cmd_to_ta(hi_u32 cmd_id, TEEC_Operation *operation, hi_u32 *ret_origin)
{
    return HI_SUCCESS;
}

static inline hi_s32 dmx_tee_create_ramport(hi_u32 ram_id, hi_u32 buf_size, hi_u32 flush_buf_size, hi_u32 dsc_buf_size,
    dmx_tee_ramport_info *ramport_info_ptr)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

static inline hi_void dmx_tee_destroy_ramport(hi_u32 ram_id, const dmx_tee_ramport_info *ramport_info_ptr) {}

static inline hi_s32 dmx_tee_set_ramport_dsc(hi_u32 ram_id, const dmx_tee_ramport_dsc *ram_port_dsc)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

static inline hi_s32 dmx_tee_create_play_chan(hi_u32 chan_id, dmx_play_type play_type,
    hi_u32 buf_size, dmx_tee_mem_info *buf_info_ptr)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}
static inline hi_void dmx_tee_destroy_play_chan(hi_u32 chan_id, dmx_play_type play_type,
    const dmx_tee_mem_info *buf_info_ptr) {}

static inline hi_s32  dmx_tee_attach_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 raw_pidch_id,
    hi_u32 master_raw_pidch_id)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

static inline hi_void dmx_tee_detach_play_chan(hi_u32 chan_id, dmx_play_type play_type, hi_u32 raw_pidch_id) {}


static inline hi_s32 dmx_tee_create_rec_chan(hi_u32 chan_id, hi_u32 buf_size, dmx_tee_mem_info *buf_info_ptr)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}
static inline hi_void dmx_tee_destroy_rec_chan(hi_u32 chan_id, const dmx_tee_mem_info *buf_info_ptr) {}

static inline hi_s32  dmx_tee_attach_rec_chan(const dmx_tee_rec_attach_info *rec_attach_ptr)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

static inline hi_void dmx_tee_detach_rec_chan(const dmx_tee_rec_detach_info *rec_attach_ptr) {}

static inline hi_s32 dmx_tee_update_play_buf_read_idx(hi_u32 buf_id, dmx_play_type play_type, hi_u32 read_idx)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}
static inline hi_s32 dmx_tee_update_rec_buf_read_idx(hi_u32 buf_id, hi_u32 read_idx)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

static inline hi_s32 dmx_tee_acquire_bufid(hi_u32 *bufid)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}
static inline hi_void dmx_tee_release_bufid(const hi_u32 bufid) {}

static inline hi_s32 dmx_tee_config_buf(hi_u32 chan_id, dmx_play_type chan_type)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}
static inline hi_void dmx_tee_deconfig_buf(hi_u32 chan_id, dmx_play_type chan_type) {}

static inline hi_void dmx_tee_detach_raw_pidch(hi_u32 raw_pidch) {}

static inline hi_s32 dmx_tee_fixup_hevc_es_index(hi_u32 rec_id, hi_u32 idx_pid, hi_u32 parse_offset,
    const findex_scd *pst_fidx, dmx_index_data *cur_frame)
{
    return HI_ERR_DMX_NOT_SUPPORT;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_TEE_H__

