/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiSilicon VI ioctl declaration.
 * Author: ATV team
 * Create: 2019-11-23
 */

#ifndef __DRV_VI_IOCTL_H__
#define __DRV_VI_IOCTL_H__

#include "drv_vi_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_handle vi_handle;
    hi_vi_attr attr;
} vi_attr;

typedef struct {
    hi_handle vi_handle;
    hi_handle vo_handle;
} vi_attach_attr;

typedef struct {
    hi_handle vi_handle;
    hi_vi_config config;
} vi_config;

typedef struct {
    hi_handle vi_handle;
    hi_vi_nonstd_info info;
} vi_nstd_info;

typedef struct {
    hi_handle vi_handle;
    hi_vi_quantization_range range;
} vi_src_quantization_range;

typedef struct {
    hi_handle vi_handle; /* current vi handle */
    hi_drv_video_frame video_frame;
} vi_path_info;

typedef struct {
    hi_handle vi_handle; /* current vi handle */
    hi_vi_dbg_info dbg_info;
} vi_path_dbg_info;

typedef enum {
    VI_IOC_CREATE = 0,
    VI_IOC_DESTROY,
    VI_IOC_ATTACH,
    VI_IOC_DETACH,
    VI_IOC_START,
    VI_IOC_STOP,

    VI_IOC_ACQUIRE_FRAME,
    VI_IOC_RELEASE_FRAME,

    VI_IOC_UPDATE_NSTD,
    VI_IOC_SET_SRC_QUANTIZATION_RANGE,

    VI_IOC_SET_CONFIG,
    VI_IOC_GET_CONFIG,
    VI_IOC_GET_ATTR,

    VI_IOC_UPDATA_DBG_INFO,
} vi_ioc;

#define VI_CMD_CREATE _IOWR(HI_ID_VI, VI_IOC_CREATE, vi_attr)
#define VI_CMD_DESTROY _IOWR(HI_ID_VI, VI_IOC_DESTROY, hi_handle)

#define VI_CMD_ATTACH _IOWR(HI_ID_VI, VI_IOC_ATTACH, vi_attach_attr)
#define VI_CMD_DETACH _IOWR(HI_ID_VI, VI_IOC_DETACH, vi_attach_attr)

#define VI_CMD_START _IOWR(HI_ID_VI, VI_IOC_START, hi_handle)
#define VI_CMD_STOP _IOWR(HI_ID_VI, VI_IOC_STOP, hi_handle)

#define VI_CMD_ACQUIRE_FRAME _IOWR(HI_ID_VI, VI_IOC_ACQUIRE_FRAME, vi_path_info)
#define VI_CMD_RELEASE_FRAME _IOWR(HI_ID_VI, VI_IOC_RELEASE_FRAME, vi_path_info)

#define VI_CMD_UPDATE_NSTD _IOWR(HI_ID_VI, VI_IOC_UPDATE_NSTD, vi_nstd_info)
#define VI_CMD_SET_SRC_QUANTIZATION_RANGE _IOWR(HI_ID_VI, VI_IOC_SET_SRC_QUANTIZATION_RANGE, vi_src_quantization_range)

#define VI_CMD_SET_CONFIG _IOWR(HI_ID_VI, VI_IOC_SET_CONFIG, vi_config)
#define VI_CMD_GET_CONFIG _IOWR(HI_ID_VI, VI_IOC_GET_CONFIG, vi_config)
#define VI_CMD_GET_ATTR _IOWR(HI_ID_VI, VI_IOC_GET_ATTR, vi_attr)

#define VI_CMD_UPDATA_DBG_INFO _IOWR(HI_ID_VI, VI_IOC_UPDATA_DBG_INFO, vi_path_dbg_info)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_VI_IOCTL_H__ */
