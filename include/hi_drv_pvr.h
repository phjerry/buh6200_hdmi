/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: PVR drv process.
 * Author: sdk
 * Create: 2019-8-16
 */

#ifndef __HI_DRV_PVR_H__
#define __HI_DRV_PVR_H__

#include "hi_type.h"

#ifdef HI_TEE_PVR_SUPPORT
typedef struct tagpvr_tee_gen_value_args_ {
    hi_u64 args[12]; /* Input:from 0 -->11; output:from 11-->0 */
} pvr_tee_gen_value_args;
#endif

typedef struct {
    hi_bool is_use;
    hi_u32  ability_used;
    pid_t   owner;
} pvr_chan_info;

typedef struct {
    hi_u32 chn;
    hi_u32 value;
} pvr_decode_ability_info;

/* definition of max play channel */
#define PVR_PLAY_MAX_CHN_NUM            5

/* definition of max record channel */
#define PVR_REC_MAX_CHN_NUM             10
#define PVR_REC_START_NUM               PVR_PLAY_MAX_CHN_NUM

#define CMD_PVR_INIT_PLAY               _IOR(HI_ID_PVR, 0x01, hi_u32)
#define CMD_PVR_CREATE_PLAY_CHN         _IOR(HI_ID_PVR, 0x02, hi_u32)
#define CMD_PVR_DESTROY_PLAY_CHN        _IOW(HI_ID_PVR, 0x03, hi_u32)
#define CMD_PVR_GET_DECODE_ABILITY      _IOR(HI_ID_PVR, 0x04, hi_u32)
#define CMD_PVR_ACQUIRE_DECODE_ABILITY  _IOWR(HI_ID_PVR, 0x05, pvr_decode_ability_info)
#define CMD_PVR_RELEASE_DECODE_ABILITY  _IOW(HI_ID_PVR, 0x06, pvr_decode_ability_info)

#define CMD_PVR_INIT_REC                _IOR(HI_ID_PVR, 0x11, hi_u32)
#define CMD_PVR_CREATE_REC_CHN          _IOR(HI_ID_PVR, 0x12, hi_u32)
#define CMD_PVR_DESTROY_REC_CHN         _IOW(HI_ID_PVR, 0x13, hi_u32)
#ifdef HI_TEE_PVR_SUPPORT
#define CMD_PVR_TEE_REC_OPEN            _IOWR(HI_ID_PVR, 0x20, pvr_tee_gen_value_args)
#define CMD_PVR_TEE_REC_CLOSE           _IOW(HI_ID_PVR, 0x21, hi_u32)
#define CMD_PVR_TEE_REC_COPY_REE_TEST   _IOWR(HI_ID_PVR, 0x22, pvr_tee_gen_value_args)
#define CMD_PVR_TEE_REC_PROCESS_TS_DATA _IOW(HI_ID_PVR, 0x23, pvr_tee_gen_value_args)
#define CMD_PVR_TEE_REC_GET_STATE       _IOW(HI_ID_PVR, 0x24, pvr_tee_gen_value_args)
#define CMD_PVR_TEE_REC_GET_ADDRINFO    _IOWR(HI_ID_PVR, 0x25, pvr_tee_gen_value_args)
#define CMD_PVR_TEE_PLAY_COPY_FROM_REE  _IOW(HI_ID_PVR, 0x30, pvr_tee_gen_value_args)
#define CMD_PVR_TEE_PLAY_PROCESS_DATA   _IOW(HI_ID_PVR, 0x31, pvr_tee_gen_value_args)
#endif

#define PVR_PLAY_DECODE_ABILITY       (120 * 7680 * 4320LL)

hi_s32 pvr_drv_mod_init(hi_void);
hi_void pvr_drv_mod_exit(hi_void);
#endif
