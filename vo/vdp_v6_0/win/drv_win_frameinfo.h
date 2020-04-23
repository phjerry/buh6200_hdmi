/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image_libin
* Create: 2019-03-28
 */

#ifndef __HI_DRV_FRAME_INFOR_H__
#define __HI_DRV_FRAME_INFOR_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32 frame_index;
    hi_u32 ai_refcount;

    hi_void *ai_data;
} win_frame_ai_result;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
