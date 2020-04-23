/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_diesad.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_diesad.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void deisad_cal_buf_size(vpss_stt_channel_attr *sad_y_channel_attr,
    vpss_stt_channel_attr *sad_c_channel_attr, vpss_stt_comm_attr *attr)
{
    sad_y_channel_attr->width = attr->width;
    sad_c_channel_attr->width = attr->width / 2; /* 2:para */
    sad_y_channel_attr->height = attr->height / 2; /* 2:para */

    switch (attr->pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21: {
            sad_c_channel_attr->height = attr->height / 2 / 2; /* 2,2:para */
            break;
        }
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1: {
            sad_c_channel_attr->height = attr->height / 2; /* 2:para */
            break;
        }
        default: {
            vpss_error("Unsupport PixFormat %d.\n", attr->pixel_format);
            return;
        }
    }

    sad_y_channel_attr->stride = vpss_align_8bit_ystride(sad_y_channel_attr->width);
    sad_c_channel_attr->stride = vpss_align_8bit_ystride(sad_c_channel_attr->width);
    sad_y_channel_attr->size = (sad_y_channel_attr->stride) * sad_y_channel_attr->height;
    sad_c_channel_attr->size = (sad_c_channel_attr->stride) * sad_c_channel_attr->height;
    return;
}

hi_s32 vpss_stt_deisad_init(vpss_stt_deisad *dei_sad, vpss_stt_comm_attr *comm_attr)
{
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if ((dei_sad == HI_NULL) || (comm_attr == HI_NULL)) {
        vpss_error("Vpss DieSad init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_sad->init == HI_TRUE) {
        (hi_void) vpss_stt_deisad_deinit(dei_sad);
    }

    memset(dei_sad, 0, sizeof(vpss_stt_deisad));
    deisad_cal_buf_size(&dei_sad->sad_y_channel_attr, &dei_sad->sad_c_channel_attr, comm_attr);
    attr.name = "vpss_sttDeiSadBuf";
    attr.size = dei_sad->sad_y_channel_attr.size + dei_sad->sad_c_channel_attr.size;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(dei_sad->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS DieSad init alloc memory failed.\n");
        return HI_FAILURE;
    }

    dei_sad->init = HI_TRUE;
    dei_sad->cnt = 0;
    return HI_SUCCESS;
}

// RGME运动信息队列去初始化
hi_s32 vpss_stt_deisad_deinit(vpss_stt_deisad *dei_sad)
{
    if (dei_sad == HI_NULL) {
        vpss_error("Vpss DieSad DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_sad->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(dei_sad->vpss_mem));
    }

    if (dei_sad->init == HI_FALSE) {
        vpss_warn("Vpss DieSad DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(dei_sad, 0, sizeof(vpss_stt_deisad));
    return HI_SUCCESS;
}

// 获取RGME运动信息
hi_s32 vpss_stt_deisad_get_cfg(vpss_stt_deisad *dei_sad,
    vpss_stt_deisad_cfg *dei_sad_y_cfg, vpss_stt_deisad_cfg *dei_sad_c_cfg)
{
    if ((dei_sad == HI_NULL) || (dei_sad_y_cfg == HI_NULL) || (dei_sad_c_cfg == HI_NULL)) {
        vpss_error("Vpss DieSad GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_sad->init == HI_FALSE) {
        vpss_error("Vpss DieSad GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    dei_sad_y_cfg->read_addr = dei_sad->vpss_mem.phy_addr;
    dei_sad_y_cfg->read_vir_addr = dei_sad->vpss_mem.vir_addr;
    dei_sad_y_cfg->write_addr = dei_sad_y_cfg->read_addr;
    dei_sad_y_cfg->write_vir_addr = dei_sad_y_cfg->read_vir_addr;
    dei_sad_y_cfg->height = dei_sad->sad_y_channel_attr.height;
    dei_sad_y_cfg->width = dei_sad->sad_y_channel_attr.width;
    dei_sad_y_cfg->stride = dei_sad->sad_y_channel_attr.stride;
    dei_sad_y_cfg->size = dei_sad->sad_y_channel_attr.size;
    dei_sad_c_cfg->read_addr = dei_sad->vpss_mem.phy_addr + dei_sad->sad_y_channel_attr.size;
    dei_sad_c_cfg->read_vir_addr = dei_sad->vpss_mem.vir_addr + dei_sad->sad_y_channel_attr.size;
    dei_sad_c_cfg->write_addr = dei_sad_c_cfg->read_addr;
    dei_sad_c_cfg->write_vir_addr = dei_sad_c_cfg->read_vir_addr;
    dei_sad_c_cfg->height = dei_sad->sad_c_channel_attr.height;
    dei_sad_c_cfg->width = dei_sad->sad_c_channel_attr.width;
    dei_sad_c_cfg->stride = dei_sad->sad_c_channel_attr.stride;
    dei_sad_c_cfg->size = dei_sad->sad_c_channel_attr.size;
    return HI_SUCCESS;
}

// RGME运动信息队列轮转
hi_s32 vpss_stt_deisad_complete(vpss_stt_deisad *dei_sad)
{
    if (dei_sad == HI_NULL) {
        vpss_error("Vpss DieSad complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_sad->init == HI_FALSE) {
        vpss_error("Vpss DieSad complete error(not init).\n");
        return HI_FAILURE;
    }

    dei_sad->cnt++;
    return HI_SUCCESS;
}

// RGME运动信息队列reset
hi_s32 vpss_stt_deisad_reset(vpss_stt_deisad *dei_sad)
{
    if (dei_sad == HI_NULL) {
        vpss_warn("Vpss DieSad complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_sad->init == HI_FALSE) {
        vpss_warn("Vpss DieSad complete error(not init).\n");
        return HI_FAILURE;
    }

    dei_sad->cnt = 0;

    if ((dei_sad->vpss_mem.vir_addr != HI_NULL) &&
        ((dei_sad->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (dei_sad->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(dei_sad->vpss_mem.vir_addr, 0, dei_sad->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

