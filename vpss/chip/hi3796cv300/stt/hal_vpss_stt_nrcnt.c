/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_nrcnt.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_nrcnt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void nrcnt_cal_channel_attr(vpss_stt_channel_attr *stt_channel_attr, vpss_stt_comm_attr *attr)
{
    stt_channel_attr->width = (attr->width + 7) / 8; /* 7,8:para */

    if (attr->interlace == HI_TRUE) {
        stt_channel_attr->height = attr->height / 2; /* 2:para */
    } else {
        stt_channel_attr->height = attr->height;
    }

    stt_channel_attr->stride = (((attr->width + 7) / 8 * 4) + 127) / 128 * 16; /* 7,8,4,127,128,16:para */
    stt_channel_attr->size = (stt_channel_attr->stride) * stt_channel_attr->height;
    return;
}

hi_s32 vpss_stt_nrcnt_init(vpss_nrcntinfo *nr_cnt_info, vpss_stt_comm_attr *comm_attr)
{
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if (nr_cnt_info == HI_NULL) {
        vpss_error("Vpss NrCntInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_cnt_info->init == HI_TRUE) {
        (hi_void) vpss_stt_nrcnt_deinit(nr_cnt_info);
    }

    memset(nr_cnt_info, 0, sizeof(vpss_nrcntinfo));
    nrcnt_cal_channel_attr(&nr_cnt_info->nr_cnt_channel_attr, comm_attr);
    attr.name = "vpss_sttNrCntBuf";
    attr.size = nr_cnt_info->nr_cnt_channel_attr.size;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(nr_cnt_info->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS NrCnt Alloc memory failed.\n");
        return HI_FAILURE;
    }

    nr_cnt_info->init = HI_TRUE;
    nr_cnt_info->cnt = 0;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrcnt_deinit(vpss_nrcntinfo *nr_cnt_info)
{
    if (nr_cnt_info == HI_NULL) {
        vpss_error("Vpss Nr DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_cnt_info->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(nr_cnt_info->vpss_mem));
    }

    if (nr_cnt_info->init == HI_FALSE) {
        vpss_warn("Vpss Nr DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(nr_cnt_info, 0, sizeof(vpss_nrcntinfo));
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrcnt_get_info(vpss_nrcntinfo *nr_cnt_info, vpss_nrcntcfg *nr_cnt_cfg)
{
    if ((nr_cnt_info == HI_NULL) || (nr_cnt_cfg == HI_NULL)) {
        vpss_error("Vpss sttwbc getcfg error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_cnt_info->init == HI_FALSE) {
        vpss_error("Vpss sttwbc getcfg error(not init).\n");
        return HI_FAILURE;
    }

    nr_cnt_cfg->width = nr_cnt_info->nr_cnt_channel_attr.width;
    nr_cnt_cfg->height = nr_cnt_info->nr_cnt_channel_attr.height;
    nr_cnt_cfg->u32stride = nr_cnt_info->nr_cnt_channel_attr.stride;
    nr_cnt_cfg->size = nr_cnt_info->nr_cnt_channel_attr.size;
    nr_cnt_cfg->read_addr = nr_cnt_info->vpss_mem.phy_addr;
    nr_cnt_cfg->read_vir_addr = nr_cnt_info->vpss_mem.vir_addr;
    nr_cnt_cfg->write_addr = nr_cnt_cfg->read_addr;
    nr_cnt_cfg->write_vir_addr = nr_cnt_cfg->read_vir_addr;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrcnt_complete(vpss_nrcntinfo *nr_cnt_info)
{
    if (nr_cnt_info == HI_NULL) {
        vpss_error("Vpss Nr complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_cnt_info->init == HI_FALSE) {
        vpss_error("Vpss Nr complete error(not init).\n");
        return HI_FAILURE;
    }

    nr_cnt_info->cnt++;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrcnt_reset(vpss_nrcntinfo *nr_cnt_info)
{
    if (nr_cnt_info == HI_NULL) {
        vpss_warn("Vpss Nr complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_cnt_info->init == HI_FALSE) {
        vpss_warn("Vpss Nr complete error(not init).\n");
        return HI_FAILURE;
    }

    nr_cnt_info->cnt = 0;

    if ((nr_cnt_info->vpss_mem.vir_addr != HI_NULL) &&
        ((nr_cnt_info->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (nr_cnt_info->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(nr_cnt_info->vpss_mem.vir_addr, 0, nr_cnt_info->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

