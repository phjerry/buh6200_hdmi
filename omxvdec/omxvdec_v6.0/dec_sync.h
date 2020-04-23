
#ifndef __DEC_SYNC_H__
#define __DEC_SYNC_H__

#include <linux/sync_file.h>
#include "hi_type.h"
#include "omxvdec.h"

/***************************** Macro Definition **********************************/
#define DEC_SYNC_NAME                   "dec_fence"

/********************** Global Variable declaration *********************************/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
#define hi_sync_fence                           sync_file
#define hi_sync_fence_create(name, sync_pt)     sync_file_create(&sync_pt->base)
#define hi_sync_fence_install(fence, fence_fd)  fd_install(fence_fd, fence->file)
#define hi_sync_fence_wait(fence, timeout)      dma_fence_wait_timeout(fence, true, timeout)
#define hi_sync_fence_fdget                     sync_file_get_fence
#define hi_sync_fence_put(fence)                dma_fence_put(fence)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
#define hi_sync_fence                           sync_file
#define hi_sync_fence_create(sync_pt)           sync_file_create(&sync_pt->base)
#define hi_sync_fence_install(fence,fence_fd)   fd_install(fence_fd, fence->file)
#define hi_sync_fence_wait(fence,timeout)       fence_wait_timeout(fence, true, timeout)
#define hi_sync_fence_fdget                     sync_file_get_fence
#define hi_sync_fence_put(fence)                fence_put(fence)
#else
#define hi_sync_fence                           sync_fence
#define hi_sw_sync_timeline_create              sw_sync_timeline_create
#define hi_sync_fence_create(sync_pt)           sync_fence_create(DEC_SYNC_NAME, sync_pt)
#define hi_sync_timeline_destroy                sync_timeline_destroy
#define hi_sync_fence_install(fence,fence_fd)   sync_fence_install(fence, fence_fd)
#define hi_sync_fence_wait(fence,timeout)       sync_fence_wait(fence, timeout)
#define hi_sw_sync_timeline_inc                 sw_sync_timeline_inc
#define hi_sw_sync_pt_create                    sw_sync_pt_create
#define hi_sync_pt_free                         sync_pt_free
#define hi_sync_fence_fdget                     sync_fence_fdget
#define hi_sync_fence_put(fence)                sync_fence_put(fence)
#endif

typedef struct {
    hi_u32 fence_value;
    hi_u32 time_line_value;
    hi_s32 release_fence_fd;
    spinlock_t lock;
    hi_bool lock_inited;
    // struct sw_sync_timeline *time_line;
} dec_sync_info;

/******************************* API declaration *********************************/
hi_s32  drv_dec_fence_init(hi_s32 id, hi_void *args_in);
hi_void drv_dec_fence_deinit(hi_s32 id, hi_void *args_in);
hi_s32  drv_dec_fence_create(hi_s32 id, struct hi_sync_fence **file);
hi_void drv_dec_fence_destroy(hi_s32 fence_fd);
hi_void drv_dec_fence_bind(hi_s32 fd, struct hi_sync_fence *fence);
hi_s32 drv_dec_get_fd(hi_s32 id);

#endif

