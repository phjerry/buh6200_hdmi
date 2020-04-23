/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm.h
 * Author: sm_rm
 * Create: 2015/11/25
 */

#ifndef __DRV_RM_H__
#define __DRV_RM_H__

#include "drv_rm_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* RM上下文数据结构 */
typedef struct {
    hi_handle              rm_handle;
    hi_void                *event_handle_p;
    hi_bool                chn_enable;
    hi_bool                list_empty;
    hi_u32                 cur_index;
    hi_u32                 queue_cnt;
    hi_u32                 try_queue_cnt;
    hi_u32                 free_node;
    hi_u32                 used_node;
    hi_u32                 cur_pid;
    hi_drv_rm_win_event    cur_event;
    osal_wait              wait_queue;
    hi_u32                 cur_query_time;
    hi_u32                 lst_query_time;
    hi_u32                 query_time_diff;
    hi_bool                idle;
} rm_drv_path_ctx;

typedef struct {
    hi_u32                 rm_path_count;
    hi_u32                 wake_up_cnt;
    hi_u32                 wake_up_time;
    hi_u32                 normal_cnt;
    hi_u32                 path_recycle_cnt;
    hi_u32                 win_owner_id;
    hi_u32                 chn_create_cnt[RM_DRV_CHN_MAX_NUM];
    hi_u32                 chn_destroy_cnt[RM_DRV_CHN_MAX_NUM];
    hi_s32                 rm_available_win_count;
    osal_wait              wait_queue;
    hi_bool                wake_up_flags;
    hi_bool                waiting_event;
    hi_drv_win_owner       wind_owner;
} rm_drv_golbal_ctx;

hi_void rm_drv_comm_get_golbal_ctx(rm_drv_golbal_ctx **rm_drv_golbal_ctx_p);
hi_void rm_drv_comm_get_path_ctx(hi_u32 rm_path_id, rm_drv_path_ctx **rm_drv_path_ctx_p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_RM_H__ */

