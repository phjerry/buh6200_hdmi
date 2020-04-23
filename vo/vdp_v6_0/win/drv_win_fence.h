#ifndef __DRV_VO_SYNC_H__
#define __DRV_VO_SYNC_H__
#include "hi_type.h"
#include "drv_xdp_list.h"
#include "hi_osal.h"

#define WIN_FENCE_TIMELINE_NUM 32
#define WIN_FENCE_ROTATE_NUM 4

typedef struct {
    common_list   fence_empty_list;
    common_list   fence_full_list;
    osal_spinlock    fence_lock;
} drv_fence_fence_array;

typedef struct {
    hi_char timeline_name[WIN_FENCE_TIMELINE_NUM];
    hi_s32 create_fence_index;
    hi_s32 destroy_fence_index;

    hi_u32 fence_seqno;
    hi_u64 fence_context;
    spinlock_t fence_lock;
    drv_fence_fence_array fence_array;

    hi_u8 reg_update_cnt;
    hi_u32 fence_value;
    hi_u32 time_line;
    hi_s32 release_fence_fd;
    hi_u32 frame_end_flag;
    spinlock_t lock;

    wait_queue_head_t frame_end_event;
    hi_u32 is_busy;
} drv_win_timeline;


hi_s32 drv_fence_init(hi_void);
hi_s32 drv_fence_deinit(hi_void);
hi_s32 drv_fence_create_instance(hi_void);
hi_s32 drv_fence_destroy_instance(hi_handle fence_instance_handle);
hi_s32 drv_fence_create_fence(hi_handle fence_instance_handle, hi_u32 frame_addr,hi_u64 *fence_descp);
hi_s32 drv_fence_signal(hi_handle fence_instance_handle, hi_void *fence_descp);
hi_s32 drv_fence_flush(hi_void);

#define DRV_HI_WIN_FENCE_NAME "hi_win_fence"


#endif
