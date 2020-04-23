/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal_vpss_stt_dmcnt.h hander file
 * Author: zhangjunyu
 * Create: 2019/11/21
 */

#include "hal_vpss_stt_dmcnt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void dmcnt_get_channel_attr(vpss_stt_channel_attr *dmcnt_channel_attr, vpss_stt_comm_attr *attr)
{
    dmcnt_channel_attr->width = (attr->width + 3) / 4; /* 3:roundup; 4:align */
    dmcnt_channel_attr->height = (attr->height + 3) / 4; /* 3:roundup; 4:align */

    /* 8,7,15,16:roundup,align */
    dmcnt_channel_attr->stride = ((dmcnt_channel_attr->width * 8 + 7) / 8 + 15) / 16 * 16;
    dmcnt_channel_attr->size = dmcnt_channel_attr->stride * dmcnt_channel_attr->height;

    return;
}

hi_u32 dmcnt_get_total_buf_size(vpss_stt_comm_attr *attr, hi_u32 node_buf_size)
{
    return node_buf_size * VPSS_DMCNT_MAX_NODE;
}

hi_s32 vpss_stt_dmcnt_init(vpss_dmcnt_info *dmcnt_info, vpss_stt_comm_attr *comm_attr)
{
    list *list_head = HI_NULL;
    hi_u32 total_buff_size;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if (dmcnt_info == HI_NULL) {
        vpss_error("Vpss NrMadInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dmcnt_info->init == HI_TRUE) {
        (hi_void)vpss_stt_dmcnt_deinit(dmcnt_info);
    }

    memset(dmcnt_info, 0, sizeof(vpss_dmcnt_info));
    list_head = &(dmcnt_info->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    dmcnt_info->first_ref = list_head;

    dmcnt_get_channel_attr(&dmcnt_info->dmcnt_channel_attr, comm_attr);
    total_buff_size = dmcnt_get_total_buf_size(comm_attr, dmcnt_info->dmcnt_channel_attr.size);
    attr.name = "vpss_sttDmCntBuf";
    attr.size = total_buff_size;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(dmcnt_info->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_sttDmCntBuf alloc memory failed. size:%d \n", total_buff_size);
        return HI_FAILURE;
    }

    phy_addr = dmcnt_info->vpss_mem.phy_addr;
    vir_addr = dmcnt_info->vpss_mem.vir_addr;

    osal_list_add_tail(&(dmcnt_info->data_list[1].node), list_head);
    dmcnt_info->data_list[0].phy_addr = phy_addr;
    dmcnt_info->data_list[0].vir_addr = vir_addr;

    phy_addr = phy_addr + dmcnt_info->dmcnt_channel_attr.size;
    vir_addr = vir_addr + dmcnt_info->dmcnt_channel_attr.size;
    dmcnt_info->data_list[1].phy_addr = phy_addr;
    dmcnt_info->data_list[1].vir_addr = vir_addr;

    dmcnt_info->init = HI_TRUE;
    dmcnt_info->cnt = 0;

    return HI_SUCCESS;
}

hi_s32 vpss_stt_dmcnt_deinit(vpss_dmcnt_info *dmcnt_info)
{
    if (dmcnt_info == HI_NULL) {
        vpss_error("vpss_stt_dmcnt_deinit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dmcnt_info->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(dmcnt_info->vpss_mem));
    }

    if (dmcnt_info->init == HI_FALSE) {
        vpss_warn("vpss_stt_dmcnt_deinit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(dmcnt_info, 0, sizeof(vpss_dmcnt_info));

    return HI_SUCCESS;
}

hi_s32 vpss_stt_dmcnt_get_info(vpss_dmcnt_info *dmcnt_info, vpss_dmcnt_cfg *dmcnt_cfg)
{
    vpss_stt_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((dmcnt_info == HI_NULL) || (dmcnt_cfg == HI_NULL)) {
        vpss_error("vpss_stt_dmcnt_get_info error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dmcnt_info->init == HI_FALSE) {
        vpss_error("vpss_stt_dmcnt_get_info error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = dmcnt_info->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    dmcnt_cfg->read_phy_addr = ref_node_data->phy_addr;
    dmcnt_cfg->read_vir_addr = ref_node_data->vir_addr;

    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    dmcnt_cfg->write_phy_addr = ref_node_data->phy_addr;
    dmcnt_cfg->write_vir_addr = ref_node_data->vir_addr;

    dmcnt_cfg->width = dmcnt_info->dmcnt_channel_attr.width;
    dmcnt_cfg->height = dmcnt_info->dmcnt_channel_attr.height;
    dmcnt_cfg->stride = dmcnt_info->dmcnt_channel_attr.stride;
    dmcnt_cfg->size = dmcnt_info->dmcnt_channel_attr.size;

    return HI_SUCCESS;
}

hi_s32 vpss_stt_dmcnt_complete(vpss_dmcnt_info *dmcnt_info)
{
    if (dmcnt_info == HI_NULL) {
        vpss_error("vpss_stt_dmcnt_complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dmcnt_info->init == HI_FALSE) {
        vpss_error("vpss_stt_dmcnt_complete error(not init).\n");
        return HI_FAILURE;
    }

    dmcnt_info->cnt++;
    dmcnt_info->first_ref = dmcnt_info->first_ref->next;

    return HI_SUCCESS;
}

hi_s32 vpss_stt_dmcnt_reset(vpss_dmcnt_info *dmcnt_info)
{
    if (dmcnt_info == HI_NULL) {
        vpss_warn("vpss_stt_dmcnt_reset error(null pointer).\n");
        return HI_FAILURE;
    }

    if (dmcnt_info->init == HI_FALSE) {
        vpss_warn("vpss_stt_dmcnt_reset error(not init).\n");
        return HI_FAILURE;
    }

    dmcnt_info->cnt = 0;

    if ((dmcnt_info->vpss_mem.vir_addr != HI_NULL) &&
        ((dmcnt_info->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (dmcnt_info->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(dmcnt_info->vpss_mem.vir_addr, 0, dmcnt_info->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

