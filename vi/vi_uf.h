/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi uf
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_UF_H__
#define __VI_UF_H__

#include "hi_drv_video.h"

#include "vi_buf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_void *buf_handle_p;
    hi_u32 depth;
} vi_uf_attr;

hi_s32 vi_uf_init(vi_uf_attr *uf_attr_p, hi_void **uf_handle_pp);
hi_s32 vi_uf_deinit(hi_void *uf_handle_p);

hi_s32 vi_uf_prepare(hi_void *uf_handle_p, hi_u32 *save_cnt);
hi_s32 vi_uf_unprepare(hi_void *uf_handle_p);
hi_s32 vi_uf_acquire_frm(hi_void *uf_handle_p, hi_u32 index, vi_buf_node *vi_node_p);
hi_s32 vi_uf_releasefrm(hi_void *uf_handle_p, vi_buf_node *vi_node_p);
hi_s32 vi_uf_sendfrm(hi_void *uf_handle_p, vi_buf_node *vi_node_p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DRV_UFH__ */
