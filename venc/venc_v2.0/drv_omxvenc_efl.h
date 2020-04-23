/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_OMXVENC_EFL_H__
#define __DRV_OMXVENC_EFL_H__

#include "drv_venc_efl.h"

#define INPUT_PORT   0
#define OUTPUT_PORT  1
#define ALL_PORT    0xFFFFFFFF

hi_s32 venc_drv_efl_put_msg_omx(queue_info *queue, hi_u32 msg_id, hi_u32 status, void *msg_data);
hi_s32 venc_drv_efl_get_msg_omx(queue_info *queue, hi_venc_msg_info *msg_info);

hi_s32 venc_drv_efl_mmap_to_kernel_omx(vedu_efl_enc_para *enc_para, venc_ioctl_mmz_map *mmb_info);
hi_s32 venc_drv_efl_ummap_to_kernel_omx(vedu_efl_enc_para *enc_para, venc_ioctl_mmz_map *mmb_info);

hi_s32 venc_drv_efl_flush_port_omx(vedu_efl_enc_para *enc_handle, hi_u32 port_index, hi_bool intra);

#endif /* __DRV_OMXVENC_EFL_H__ */
