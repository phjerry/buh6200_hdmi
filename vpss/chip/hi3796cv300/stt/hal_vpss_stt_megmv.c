/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_megmv.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_megmv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void megmv_cal_buf_size(hi_u32 *size, hi_u32 *stride)
{
    hi_u32 width = 8;
    hi_u32 height = 15;

    if ((size == HI_NULL) || (stride == HI_NULL)) {
        vpss_error("VPSS_STT_CalMegmvBufSize para null !!!!!\n");
        return;
    }

    *stride = (width * 64 + 127) / 128 * 16; /* 64,127,128,16:para */
    *size = (*stride) * height;
    return;
}

hi_s32 vpss_stt_megmv_init(vpss_stt_megmv *megmv)
{
    list *list_head = HI_NULL;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if (megmv == HI_NULL) {
        vpss_error("Vpss MeGMV init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (megmv->init == HI_TRUE) {
        (hi_void) vpss_stt_megmv_deinit(megmv);
    }

    memset(megmv, 0, sizeof(vpss_stt_megmv));
    megmv_cal_buf_size(&(megmv->size), &(megmv->stride));
    attr.name = "vpss_sttMegmvBuf";
    attr.size = megmv->size * VPSS_MEGMV_MAX_NODE;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(megmv->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS MeGMV init alloc memory failed.\n");
        return HI_FAILURE;
    }

    phy_addr = megmv->vpss_mem.phy_addr;
    vir_addr = megmv->vpss_mem.vir_addr;
    list_head = &(megmv->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    megmv->first_ref = list_head;
    osal_list_add_tail(&(megmv->data_list[1].node), list_head);
    megmv->data_list[0].phy_addr = phy_addr;
    megmv->data_list[0].vir_addr = vir_addr;
    phy_addr = phy_addr + megmv->size;
    vir_addr = vir_addr + megmv->size;
    megmv->data_list[1].phy_addr = phy_addr;
    megmv->data_list[1].vir_addr = vir_addr;
    megmv->init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_megmv_deinit(vpss_stt_megmv *megmv)
{
    if (megmv == HI_NULL) {
        vpss_error("Vpss MeGMV DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (megmv->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(megmv->vpss_mem));
    }

    if (megmv->init == HI_FALSE) {
        vpss_warn("Vpss MeGMV DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(megmv, 0, sizeof(vpss_stt_megmv));
    return HI_SUCCESS;
}

hi_s32 vpss_stt_megmv_get_cfg(vpss_stt_megmv *megmv, vpss_stt_megmv_cfg *megmv_cfg)
{
    vpss_stt_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((megmv == HI_NULL) || (megmv_cfg == HI_NULL)) {
        vpss_error("Vpss MeGMV GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (megmv->init == HI_FALSE) {
        vpss_error("Vpss MeGMV GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = megmv->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    megmv_cfg->read_addr = ref_node_data->phy_addr;
    megmv_cfg->read_vir_addr = ref_node_data->vir_addr;
    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    megmv_cfg->write_addr = ref_node_data->phy_addr;
    megmv_cfg->write_vir_addr = ref_node_data->vir_addr;
    megmv_cfg->stride = megmv->stride;
    megmv_cfg->size = megmv->size;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_megmv_complete(vpss_stt_megmv *megmv)
{
    if (megmv == HI_NULL) {
        vpss_error("Vpss MeGMV complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (megmv->init == HI_FALSE) {
        vpss_error("Vpss MeGMV complete error(not init).\n");
        return HI_FAILURE;
    }

    megmv->cnt++;
    megmv->first_ref = megmv->first_ref->next;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_megmv_reset(vpss_stt_megmv *megmv)
{
    if (megmv == HI_NULL) {
        vpss_warn("Vpss MeGMV complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (megmv->init == HI_FALSE) {
        vpss_warn("Vpss MeGMV complete error(not init).\n");
        return HI_FAILURE;
    }

    megmv->cnt = 0;

    if ((megmv->vpss_mem.vir_addr != HI_NULL) &&
        ((megmv->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (megmv->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(megmv->vpss_mem.vir_addr, 0, megmv->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

