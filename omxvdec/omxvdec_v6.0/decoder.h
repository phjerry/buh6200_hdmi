/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */


#ifndef __DECODER_H__
#define __DECODER_H__

#include "channel.h"

#define LAST_FRAME_BUF_ID   (0xFFFFFFEE)
#define LAST_FRAME_BUF_SIZE (20)
#define INVALID_IMAGE_ID    (0xFFFFFFFF)


typedef enum {
    DEC_CMD_PAUSE,
    DEC_CMD_RESUME,
    DEC_CMD_FLUSH,
    DEC_CMD_CLEAR_STREAM,
    DEC_CMD_BIND_MEM,
    DEC_CMD_UNBIND_MEM,
    DEC_CMD_RELEASE_IMAGE,
    DEC_CMD_SET_FRAME_NUM,
    DEC_CMD_CFG_INST,
    DEC_CMD_BUTT,
    DEC_CMD_BIND_ES_BUF,
} decoder_cmd;

hi_s32 decoder_init(hi_void);

hi_s32 decoder_exit(hi_void);

hi_s32 decoder_create_inst(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pdrv_cfg);

hi_s32 decoder_release_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 decoder_start_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 decoder_stop_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 decoder_reset_inst_with_option(OMXVDEC_CHAN_CTX *pchan);

hi_s32 decoder_command_handler(OMXVDEC_CHAN_CTX *pchan, decoder_cmd e_cmd, hi_void *p_args, hi_u32 param_length);

hi_s32 decoder_get_stream_ex(hi_s32 chan_id, hi_void *stream_data);

hi_s32 decoder_release_stream_ex(hi_s32 chan_id, hi_void *stream_data);

hi_s32 decoder_suspend(hi_void);

hi_s32 decoder_resume(hi_void);
hi_s32 decoder_init_trusted(hi_void);
hi_s32 decoder_exit_trusted(hi_void);

#endif

