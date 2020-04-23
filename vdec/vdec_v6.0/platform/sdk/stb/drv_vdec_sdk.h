#ifndef __DRV_VDEC_SDK_H__
#define __DRV_VDEC_SDK_H__

#include "drv_vdec_osal.h"
#include "hi_debug.h"
#include "hi_drv_module.h"
#include "hi_type.h"
#include "hi_drv_video.h"

#define VDEC_PRINT_FATAL(fmt...)  HI_FATAL_PRINT(HI_ID_VDEC, fmt)
#define VDEC_PRINT_ERR(fmt...)    HI_ERR_PRINT(HI_ID_VDEC, fmt)
#define VDEC_PRINT_WARN(fmt...)   HI_WARN_PRINT(HI_ID_VDEC, fmt)
#define VDEC_PRINT_INFO(fmt...)   HI_INFO_PRINT(HI_ID_VDEC, fmt)
#define VDEC_PRINT_DBG(fmt...)    HI_DBG_PRINT(HI_ID_VDEC, fmt)
#define VDEC_PRINT_TRACE(fmt...)  HI_TRACE_PRINT(HI_ID_VDEC, fmt)

typedef hi_s32 (*fn_vdec_proc_read)(hi_void *pf, hi_void *data);
typedef hi_s32 (*fn_vdec_proc_write)(struct file *file, const char __user *buf, size_t count, loff_t *pos);

hi_s32  vdec_drv_get_vfmw_func(hi_void **func);
hi_s32  vdec_drv_register(hi_void *func);
hi_void vdec_drv_unregister(hi_void);
hi_void vdec_drv_stat_event(hi_u32 event_id, hi_u32 size);
hi_s32 vdec_drv_create_proc(hi_char *name, fn_vdec_proc_read read_fn, osal_proc_cmd *cmd_list, hi_u32 cmd_cnt);
hi_void vdec_drv_destroy_proc(hi_char *name);
hi_s64  vdec_drv_get_fd(hi_void *buf);
hi_void vdec_drv_put_fd(hi_s64 fd);
hi_void*  vdec_drv_get_dma_buf(hi_s64 fd);
hi_void vdec_drv_put_dma_buf(hi_void *buf);
hi_u64  vdec_drv_get_phy(hi_void *buf, hi_bool is_sec);
hi_void vdec_drv_put_phy(hi_void *buf, hi_u64 phy, hi_bool is_sec);

#endif
