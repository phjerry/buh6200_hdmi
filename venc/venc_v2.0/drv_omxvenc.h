/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_OMXVENC_H__
#define __DRV_OMXVENC_H__

#include "drv_venc.h"

hi_s32 venc_drv_queue_frame_omx(vedu_efl_enc_para *venc_chn, hi_venc_user_buf *frame_info);

hi_s32 venc_drv_queue_stream_omx(vedu_efl_enc_para *venc_chn, hi_venc_user_buf *frame_info);

hi_s32 venc_drv_get_message_omx(vedu_efl_enc_para *venc_chn, hi_venc_msg_info *pmsg_info);

hi_s32 venc_drv_mmz_map_omx(vedu_efl_enc_para *venc_chn, venc_ioctl_mmz_map *mmb_info);
hi_s32 venc_drv_mmz_ummap_omx(vedu_efl_enc_para *venc_chn, venc_ioctl_mmz_map *mmb_info);

hi_s32 venc_drv_flush_port_omx(vedu_efl_enc_para *venc_chn, hi_u32 port_index, hi_u32 inter_flag);

#endif /* __DRV_OMXVENC_H__ */
