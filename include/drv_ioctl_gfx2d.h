/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: ioctl command
 * Author: sdk
 * Create: 2018-12-10
 */

#ifndef  __DRV_IOCTL_GFX2D_H__
#define  __DRV_IOCTL_GFX2D_H__

#include <linux/ioctl.h>
#include "hi_type.h"
#include "hi_drv_module.h"
#include "drv_gfx2d_struct.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define GFX2D_SUPPORT_MARK 0xFEDCBA9876543210

typedef struct {
    hi_u32 back_color;    /* CNcomment: 叠加背景色 */
    hi_u32 compose_cnt;   /* CNcomment: 叠加层数量 */
    hi_u64 compose;       /* CNcomment: 叠加层链表,按照由下到上的顺序排列 */
} gfx2d_compose_list;

typedef struct {
    hi_bool work_sync;
    hi_u32 time_out;
    hi_u64 is_compose_support;
    drv_gfx2d_dev_id dev_id;
    gfx2d_compose_list compose_list;
    drv_gfx2d_compose_surface dest_surface;
} gfx2d_cmd_compose;

typedef struct {
    hi_u32 time_out;
    hi_u64 is_compose_support;
    drv_gfx2d_dev_id dev_id;
} gfx2d_cmd_wait_done;

#define DRV_GFX2D_CMD_COMPOSE        _IOWR(HI_ID_GFX2D, 0, gfx2d_cmd_compose)
#define DRV_GFX2D_CMD_WAIT_DONE      _IOW(HI_ID_GFX2D, 1, gfx2d_cmd_wait_done)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_IOCTL_GFX2D_H__ */
