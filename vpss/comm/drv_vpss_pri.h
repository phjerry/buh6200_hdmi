/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_pri.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#ifndef __DRV_VPSS_PRI_H__
#define __DRV_VPSS_PRI_H__

#include "hi_type.h"
#include "hi_drv_video.h"

/* video frame buffer physical address */
typedef struct {
    /* Y address */
    hi_u64  phy_addr_y_head; /* only for compress format */
    hi_u64  phy_addr_y;

    /* C OR cb address */
    hi_u64  phy_addr_c_head;
    hi_u64  phy_addr_c;

    /* cr address */
    hi_u64  phy_addr_cr_head;
    hi_u64  phy_addr_cr;

    hi_u32  phy_addr_y_head_offset;
    hi_u32  phy_addr_y_offset;
    hi_u32  phy_addr_c_head_offset;
    hi_u32  phy_addr_c_offset;
    hi_u32  phy_addr_cr_head_offset;
    hi_u32  phy_addr_cr_offset;

    hi_u32  stride_y;
    hi_u32  stride_c;
    hi_u32  stride_cr;

    hi_u32  head_stride;
    hi_u32  head_size;

    hi_u64  vir_addr_y;
    hi_u64  vir_addr_c;
} hi_drv_vpss_vid_frame_addr;

typedef struct {
    hi_drv_video_frame comm_frame;

    hi_drv_vpss_vid_frame_addr     vpss_buf_addr[HI_DRV_3D_EYE_MAX];
    hi_drv_vpss_vid_frame_addr     vpss_buf_addr_lb[HI_DRV_3D_EYE_MAX];
} hi_drv_vpss_video_frame;

#endif /* __DRV_VPSS_PRI_H__ */
