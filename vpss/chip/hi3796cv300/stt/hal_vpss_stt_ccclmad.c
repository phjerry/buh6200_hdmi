/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_ccclmad.c source file vpss wbc
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_stt_ccclmad.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void cccl_cnt_cal_buf_size(vpss_stt_channel_attr *cnt_y_channel_attr,
    vpss_stt_channel_attr *cnt_c_channel_attr, vpss_stt_comm_attr *comm_attr)
{
    cnt_y_channel_attr->width = comm_attr->width;
    cnt_c_channel_attr->width = comm_attr->width;
    cnt_y_channel_attr->height = comm_attr->height;
    cnt_c_channel_attr->height = comm_attr->height;

    if (comm_attr->interlace == HI_TRUE) {
        cnt_y_channel_attr->height /= 2; /* 2:para */
        cnt_c_channel_attr->height /= 2; /* 2:para */
    }

    cnt_y_channel_attr->stride = (cnt_y_channel_attr->width * 4 + 127) / 128 * 16; /* 4,127,128,16:para */
    cnt_c_channel_attr->stride = (cnt_c_channel_attr->width * 8 + 127) / 128 * 16; /* 8,127,128,16:para */
    cnt_y_channel_attr->size = (cnt_y_channel_attr->stride) * cnt_y_channel_attr->height;
    cnt_c_channel_attr->size = (cnt_c_channel_attr->stride) * cnt_c_channel_attr->height;
    return;
}

hi_s32 vpss_stt_cccl_cnt_init(vpss_stt_ccclcnt *cccl_cnt, vpss_stt_comm_attr *comm_attr)
{
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if ((cccl_cnt == HI_NULL) || (comm_attr == HI_NULL)) {
        vpss_error("Vpss CcclCnt init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_cnt->init == HI_TRUE) {
        (hi_void) vpss_stt_cccl_cnt_deinit(cccl_cnt);
    }

    memset(cccl_cnt, 0, sizeof(vpss_stt_ccclcnt));
    memcpy(&(cccl_cnt->attr), comm_attr, sizeof(vpss_stt_comm_attr));
    cccl_cnt_cal_buf_size(&(cccl_cnt->cnt_y_channel_attr), &(cccl_cnt->cnt_c_channel_attr), comm_attr);
    attr.name = "vpss_sttCcclCntBuf";
    attr.size = (cccl_cnt->cnt_y_channel_attr.size + cccl_cnt->cnt_c_channel_attr.size) * 2; /* 2:para */
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(cccl_cnt->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS CcclCnt init alloc memory failed.\n");
        return HI_FAILURE;
    }

    cccl_cnt->init = HI_TRUE;
    cccl_cnt->cnt = 0;
    cccl_cnt->buffer_addr = cccl_cnt->vpss_mem.phy_addr;
    cccl_cnt->buffer_vir_addr = cccl_cnt->vpss_mem.vir_addr;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_cccl_cnt_deinit(vpss_stt_ccclcnt *cccl_cnt)
{
    if (cccl_cnt == HI_NULL) {
        vpss_error("Vpss CcclCnt DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_cnt->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(cccl_cnt->vpss_mem));
    }

    if (cccl_cnt->init == HI_FALSE) {
        vpss_warn("Vpss CcclCnt DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(cccl_cnt, 0, sizeof(vpss_stt_ccclcnt));
    return HI_SUCCESS;
}

hi_s32 vpss_stt_cccl_cnt_get_cfg(vpss_stt_ccclcnt *cccl_cnt,
                                 vpss_stt_ccclcnt_cfg *cccl_cnt_y_cfg, vpss_stt_ccclcnt_cfg *cccl_cnt_c_cfg)
{
    if ((cccl_cnt == HI_NULL) || (cccl_cnt_y_cfg == HI_NULL)
        || (cccl_cnt_c_cfg == HI_NULL)) {
        vpss_error("Vpss CcclCnt GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_cnt->init == HI_FALSE) {
        vpss_error("Vpss CcclCnt GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    cccl_cnt_y_cfg->read_addr = cccl_cnt->buffer_addr;
    cccl_cnt_y_cfg->read_vir_addr = cccl_cnt->buffer_vir_addr;
    cccl_cnt_y_cfg->write_addr = cccl_cnt_y_cfg->read_addr;
    cccl_cnt_y_cfg->write_vir_addr = cccl_cnt->buffer_vir_addr;
    cccl_cnt_y_cfg->height = cccl_cnt->cnt_y_channel_attr.height;
    cccl_cnt_y_cfg->stride = cccl_cnt->cnt_y_channel_attr.stride;
    cccl_cnt_y_cfg->width = cccl_cnt->cnt_y_channel_attr.width;
    cccl_cnt_y_cfg->size = cccl_cnt->cnt_y_channel_attr.size;
    cccl_cnt_c_cfg->read_addr = cccl_cnt->buffer_addr
                                + cccl_cnt->cnt_y_channel_attr.size;
    cccl_cnt_c_cfg->read_vir_addr = cccl_cnt->buffer_vir_addr
                                    + cccl_cnt->cnt_y_channel_attr.size;
    cccl_cnt_c_cfg->write_addr = cccl_cnt_c_cfg->read_addr;
    cccl_cnt_c_cfg->write_vir_addr = cccl_cnt_c_cfg->read_vir_addr;
    cccl_cnt_c_cfg->height = cccl_cnt->cnt_c_channel_attr.height;
    cccl_cnt_c_cfg->stride = cccl_cnt->cnt_c_channel_attr.stride;
    cccl_cnt_c_cfg->width = cccl_cnt->cnt_c_channel_attr.width;
    cccl_cnt_c_cfg->size = cccl_cnt->cnt_c_channel_attr.size;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_cccl_cnt_complete(vpss_stt_ccclcnt *cccl_cnt)
{
    if (cccl_cnt == HI_NULL) {
        vpss_error("Vpss CcclCnt complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_cnt->init == HI_FALSE) {
        vpss_error("Vpss CcclCnt complete error(not init).\n");
        return HI_FAILURE;
    }

    cccl_cnt->cnt++;

    if (cccl_cnt->buffer_addr == cccl_cnt->vpss_mem.phy_addr) {
        cccl_cnt->buffer_addr = cccl_cnt->vpss_mem.phy_addr +
                                cccl_cnt->cnt_y_channel_attr.size + cccl_cnt->cnt_c_channel_attr.size;
        cccl_cnt->buffer_vir_addr = cccl_cnt->vpss_mem.vir_addr +
                                    cccl_cnt->cnt_y_channel_attr.size + cccl_cnt->cnt_c_channel_attr.size;
    } else {
        cccl_cnt->buffer_addr = cccl_cnt->vpss_mem.phy_addr;
        cccl_cnt->buffer_vir_addr = cccl_cnt->vpss_mem.vir_addr;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_stt_cccl_cnt_reset(vpss_stt_ccclcnt *cccl_cnt)
{
    if (cccl_cnt == HI_NULL) {
        vpss_warn("Vpss CcclCnt complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (cccl_cnt->init == HI_FALSE) {
        vpss_warn("Vpss CcclCnt complete error(not init).\n");
        return HI_FAILURE;
    }

    cccl_cnt->cnt = 0;

    if ((cccl_cnt->vpss_mem.vir_addr != HI_NULL) &&
        ((cccl_cnt->vpss_mem.mode != OSAL_SECSMMU_TYPE) ||
         (cccl_cnt->vpss_mem.mode != OSAL_SECMMZ_TYPE))) {
        memset(cccl_cnt->vpss_mem.vir_addr, 0, cccl_cnt->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

