/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: PVR tee process.
 * Author: sdk
 * Create: 2019-08-16
 */

#include "hi_drv_pvr.h"
#include "teek_client_api.h"

#ifndef __DRV_PVR_TEE_FUNC_H__
#define __DRV_PVR_TEE_FUNC_H__

#ifdef HI_TEE_PVR_SUPPORT
#define PVR_TEE_REC_MAX_CHN_NUM         PVR_REC_MAX_CHN_NUM
#define REE_PVR_BUFFER_MAX_LEN          (2 * 47 * 1024 * 1024)
#define REE_PVR_BUFFER_MIN_LEN          (4 * 188 * 256)
#define REE_PVR_BUFFER_ALIGN            (47 * 1024)

typedef enum {
    TEE_PVR_CMD_REC_OPEN_CHANNEL    = 0,
    TEE_PVR_CMD_REC_CLOSE_CHANNEL   = 1,
    TEE_PVR_CMD_REC_PROCESS_DATA    = 2,
    TEE_PVR_CMD_REC_GET_TEE_STATE   = 4,
    TEE_PVR_CMD_REC_COPY_TO_REE     = 8,
    TEE_PVR_CMD_REC_GET_ADDRINFO    = 16,
    TEE_PVR_CMD_PLAY_COPY_FROM_REE  = 32,
    TEE_PVR_CMD_PLAY_PROCESS_DATA   = 64,
    TEE_PVR_CMD_TYPE_INVALID        = 0xffffffff,
} tee_pvr_cmd_type;

typedef struct tagpvr_drv_teec_info_ {
    hi_s32 init_cnt;
    TEEC_Context context;
    TEEC_Session session;
} pvr_drv_teec_info;

typedef struct tagpvr_drv_tee_rec_chan_ {
    hi_bool is_use;
    hi_u32 tee_id;
    pid_t owner;
} pvr_drv_tee_rec_chan;

hi_s32 pvr_teec_init(hi_void);
hi_s32 pvr_teec_deinit(hi_void);
hi_s32 pvr_tee_release(hi_void *private_data);
hi_s32 pvr_tee_rec_open_chn(pvr_tee_gen_value_args *args);
hi_s32 pvr_tee_rec_close_chn(pvr_tee_gen_value_args *args);
hi_s32 pvr_tee_rec_copy_to_ree(pvr_tee_gen_value_args *args);
hi_s32 pvr_tee_rec_process_data(pvr_tee_gen_value_args *args);
hi_s32 pvr_tee_rec_get_state(pvr_tee_gen_value_args *args);
hi_s32 pvr_tee_rec_get_addr_info(pvr_tee_gen_value_args *args);
hi_s32 pvr_tee_play_copy_from_ree(pvr_tee_gen_value_args *args);
hi_s32 pvr_tee_play_process_data(pvr_tee_gen_value_args *args);
hi_s32 hi_drv_pvr_teec_ioctl(hi_void *private_data, hi_u32 cmd, hi_void *arg);
#endif
#endif
