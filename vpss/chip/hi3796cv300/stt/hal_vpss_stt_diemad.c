/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_ccclmad.c source file vpss wbc
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_stt_diemad.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void deimad_cal_buf_size(vpss_stt_channel_attr *dei_mad_chan_attr, vpss_stt_comm_attr *attr)
{
    dei_mad_chan_attr->width = (attr->width + 3) / 4; /* 3:align, 4:alg use */
    dei_mad_chan_attr->height = attr->height / 2; /* 2:alg use */
    dei_mad_chan_attr->stride = (dei_mad_chan_attr->width * 16 + 127) / 128 * 16; /* 16,127, 128, 16 : align */
    dei_mad_chan_attr->size = dei_mad_chan_attr->stride * dei_mad_chan_attr->height;
    return;
}

hi_s32 vpss_stt_deimad_init(vpss_stt_deimad *dei_mad, vpss_stt_comm_attr *comm_attr)
{
    list *list_head = HI_NULL;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if ((dei_mad == HI_NULL) || (comm_attr == HI_NULL)) {
        vpss_error("Vpss DieMad init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_mad->init == HI_TRUE) {
        (hi_void) vpss_stt_deimad_deinit(dei_mad);
    }

    memset(dei_mad, 0, sizeof(vpss_stt_deimad));
    deimad_cal_buf_size(&(dei_mad->dei_mad_chan_attr), comm_attr);
    attr.name = "vpss_sttDeiMadBuf";
    attr.size = dei_mad->dei_mad_chan_attr.size * VPSS_DEIMAD_MAX_NODE;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(dei_mad->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS DieMad init alloc memory failed.\n");
        return HI_FAILURE;
    }

    phy_addr = dei_mad->vpss_mem.phy_addr;
    vir_addr = dei_mad->vpss_mem.vir_addr;
    list_head = &(dei_mad->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    dei_mad->first_ref = list_head;
    osal_list_add_tail(&(dei_mad->data_list[1].node), list_head);
    dei_mad->data_list[0].phy_addr = phy_addr;
    dei_mad->data_list[0].vir_addr = vir_addr;
    phy_addr = phy_addr + dei_mad->dei_mad_chan_attr.size;
    vir_addr = vir_addr + dei_mad->dei_mad_chan_attr.size;
    dei_mad->data_list[1].phy_addr = phy_addr;
    dei_mad->data_list[1].vir_addr = vir_addr;
    dei_mad->init = HI_TRUE;
    dei_mad->cnt = 0;
    return HI_SUCCESS;
}

// RGME运动信息队列去初始化
hi_s32 vpss_stt_deimad_deinit(vpss_stt_deimad *dei_mad)
{
    if (dei_mad == HI_NULL) {
        vpss_error("Vpss DieMad DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_mad->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(dei_mad->vpss_mem));
    }

    if (dei_mad->init == HI_FALSE) {
        vpss_warn("Vpss DieMad DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(dei_mad, 0, sizeof(vpss_stt_deimad));
    return HI_SUCCESS;
}

// 获取RGME运动信息
hi_s32 vpss_stt_deimad_get_cfg(vpss_stt_deimad *dei_mad, vpss_stt_deimad_cfg *dei_mad_cfg)
{
    vpss_stt_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((dei_mad == HI_NULL) || (dei_mad_cfg == HI_NULL)) {
        vpss_error("Vpss DieMad GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_mad->init == HI_FALSE) {
        vpss_error("Vpss DieMad GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = dei_mad->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    dei_mad_cfg->read_addr = ref_node_data->phy_addr;
    dei_mad_cfg->read_vir_addr = ref_node_data->vir_addr;
    dei_mad_cfg->write_addr = dei_mad_cfg->read_addr;
    dei_mad_cfg->write_vir_addr = ref_node_data->vir_addr;
    dei_mad_cfg->height = dei_mad->dei_mad_chan_attr.height;
    dei_mad_cfg->width = dei_mad->dei_mad_chan_attr.width;
    dei_mad_cfg->stride = dei_mad->dei_mad_chan_attr.stride;
    dei_mad_cfg->size = dei_mad->dei_mad_chan_attr.size;
    return HI_SUCCESS;
}

// RGME运动信息队列轮转
hi_s32 vpss_stt_deimad_complete(vpss_stt_deimad *dei_mad)
{
    if (dei_mad == HI_NULL) {
        vpss_error("Vpss DieMad complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_mad->init == HI_FALSE) {
        vpss_error("Vpss DieMad complete error(not init).\n");
        return HI_FAILURE;
    }

    dei_mad->cnt++;
    dei_mad->first_ref = dei_mad->first_ref->next;
    return HI_SUCCESS;
}

// RGME运动信息队列reset
hi_s32 vpss_stt_deimad_reset(vpss_stt_deimad *dei_mad)
{
    if (dei_mad == HI_NULL) {
        vpss_warn("Vpss DieMad complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dei_mad->init == HI_FALSE) {
        vpss_warn("Vpss DieMad complete error(not init).\n");
        return HI_FAILURE;
    }

    dei_mad->cnt = 0;

    if ((dei_mad->vpss_mem.vir_addr != HI_NULL) &&
        ((dei_mad->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (dei_mad->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(dei_mad->vpss_mem.vir_addr, 0, dei_mad->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
