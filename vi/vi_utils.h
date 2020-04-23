/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi utils
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_UTILS_H__
#define __VI_UTILS_H__

#include "hi_drv_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VI_DUMP_NAME_LEN 256

typedef enum {
    VI_SAVEFRAME_BITWIDTH_8BIT = 0,
    VI_SAVEFRAME_BITWIDTH_10BIT,
    VI_SAVEFRAME_BITWIDTH_12BIT,
    VI_SAVEFRAME_BITWIDTH_BUTT
} vi_saveframe_bitwidth;

hi_s32 vi_utils_write_frame(hi_char *file_p, vi_buf_node *node_p, vi_saveframe_bitwidth bit_width);
hi_s32 vi_utils_generate_frame_name(hi_drv_video_frame *frame_p, hi_s8 *file_name_p, hi_u32 size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
