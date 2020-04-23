/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_memv.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_memv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void memv_cal_buf_size(vpss_stt_channel_attr *memv_channel_attr, vpss_stt_comm_attr *attr)
{
    if (attr->width > 960) { /* 960:para */
        memv_channel_attr->width = (attr->width / 2 + 7) / 8; /* 2,7,8:para */
    } else {
        memv_channel_attr->width = (attr->width / 1 + 7) / 8; /* 1,7,8:para */
    }

    if (attr->interlace == HI_TRUE) {
        memv_channel_attr->height = (attr->height / 2 + 3) / 4; /* 2,3,4:para */
    } else {
        memv_channel_attr->height = (attr->height + 3) / 4; /* 3,4:para */
    }

    memv_channel_attr->stride = (memv_channel_attr->width * 64 + 127) / 128 * 16; /* 64,127,128,16:para */
    memv_channel_attr->size = memv_channel_attr->stride * memv_channel_attr->height;
    return;
}

hi_s32 vpss_stt_memv_init(vpss_stt_memv *memv, vpss_stt_comm_attr *comm_attr)
{
    list *list_head = HI_NULL;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };
    drv_vpss_mem_attr attr_for_tnr = { 0 };

    if ((memv == HI_NULL) || (comm_attr == HI_NULL)) {
        vpss_error("Vpss MeMV init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (memv->init == HI_TRUE) {
        (hi_void) vpss_stt_memv_deinit(memv);
    }

    memset(memv, 0, sizeof(vpss_stt_memv));
    memv_cal_buf_size(&(memv->memv_channel_attr), comm_attr);
    attr.name = "vpss_sttMemvBuf";
    attr.size = memv->memv_channel_attr.size * VPSS_MEMV_MAX_NODE;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(memv->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS MeMV init alloc memory failed.\n");
        return HI_FAILURE;
    }

    phy_addr = memv->vpss_mem.phy_addr;
    vir_addr = memv->vpss_mem.vir_addr;
    list_head = &(memv->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    memv->first_ref = list_head;
    osal_list_add_tail(&(memv->data_list[1].node), list_head);
    osal_list_add_tail(&(memv->data_list[2].node), list_head); /* 2:array num */
    osal_list_add_tail(&(memv->data_list[3].node), list_head); /* 3:array num */
    memv->data_list[0].phy_addr = phy_addr;
    memv->data_list[0].vir_addr = vir_addr;
    phy_addr = phy_addr + memv->memv_channel_attr.size;
    vir_addr = vir_addr + memv->memv_channel_attr.size;
    memv->data_list[1].phy_addr = phy_addr;
    memv->data_list[1].vir_addr = vir_addr;
    phy_addr = phy_addr + memv->memv_channel_attr.size;
    vir_addr = vir_addr + memv->memv_channel_attr.size;
    memv->data_list[2].phy_addr = phy_addr; /* 2:array num */
    memv->data_list[2].vir_addr = vir_addr; /* 2:array num */
    phy_addr = phy_addr + memv->memv_channel_attr.size;
    vir_addr = vir_addr + memv->memv_channel_attr.size;
    memv->data_list[3].phy_addr = phy_addr; /* 3:array num */
    memv->data_list[3].vir_addr = vir_addr; /* 3:array num */
    attr_for_tnr.name = "vpss_memvForTnrBuf";
    attr_for_tnr.size = memv->memv_channel_attr.size;
    attr_for_tnr.mode = OSAL_NSSMMU_TYPE;
    attr_for_tnr.is_map_viraddr = HI_TRUE;
    attr_for_tnr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr_for_tnr, &(memv->vpss_mem_for_tnr));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS MemvForTnrBuf init alloc memory failed.\n");
        vpss_comm_mem_free(&(memv->vpss_mem));
        memset(memv, 0, sizeof(vpss_stt_memv));
        return HI_FAILURE;
    }

    memv->init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_memv_deinit(vpss_stt_memv *memv)
{
    if (memv == HI_NULL) {
        vpss_error("Vpss MeMV DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (memv->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(memv->vpss_mem));
    }

    if (memv->vpss_mem_for_tnr.phy_addr != 0) {
        vpss_comm_mem_free(&(memv->vpss_mem_for_tnr));
    }

    if (memv->init == HI_FALSE) {
        vpss_warn("Vpss MeMV DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(memv, 0, sizeof(vpss_stt_memv));
    return HI_SUCCESS;
}

hi_s32 vpss_stt_memv_get_cfg(vpss_stt_memv *memv, vpss_stt_memv_cfg *memv_cfg)
{
    vpss_stt_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((memv == HI_NULL) || (memv_cfg == HI_NULL)) {
        vpss_error("Vpss MeMV GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (memv->init == HI_FALSE) {
        vpss_error("Vpss MeMV GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = memv->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    memv_cfg->p3_read_addr = ref_node_data->phy_addr;
    memv_cfg->p3_read_vir_addr = ref_node_data->vir_addr;
    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    memv_cfg->p2_read_addr = ref_node_data->phy_addr;
    memv_cfg->p2_read_vir_addr = ref_node_data->vir_addr;
    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    memv_cfg->p1_read_addr = ref_node_data->phy_addr;
    memv_cfg->p1_read_vir_addr = ref_node_data->vir_addr;
    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    memv_cfg->cur_write_addr = ref_node_data->phy_addr;
    memv_cfg->write_vir_addr = ref_node_data->vir_addr;
    memv_cfg->width = memv->memv_channel_attr.width;
    memv_cfg->height = memv->memv_channel_attr.height;
    memv_cfg->stride = memv->memv_channel_attr.stride;
    memv_cfg->size = memv->memv_channel_attr.size;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_memv_for_tnr_get_cfg(vpss_stt_memv *memv, vpss_stt_memvfortnr_cfg *memv_for_tnr_cfg)
{
    if ((memv == HI_NULL) || (memv_for_tnr_cfg == HI_NULL)) {
        vpss_error("Vpss MeMV getcfg error(null pointer).\n");
        return HI_FAILURE;
    }

    if (memv->init == HI_FALSE) {
        vpss_error("Vpss MeMV getcfg error(not init).\n");
        return HI_FAILURE;
    }

    memv_for_tnr_cfg->read_addr = memv->vpss_mem_for_tnr.phy_addr;
    memv_for_tnr_cfg->write_addr = memv_for_tnr_cfg->read_addr;
    memv_for_tnr_cfg->width = memv->memv_channel_attr.width;
    memv_for_tnr_cfg->height = memv->memv_channel_attr.height;
    memv_for_tnr_cfg->stride = memv->memv_channel_attr.stride;
    memv_for_tnr_cfg->size = memv->memv_channel_attr.size;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_memv_complete(vpss_stt_memv *memv)
{
    if (memv == HI_NULL) {
        vpss_error("Vpss MeMV complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (memv->init == HI_FALSE) {
        vpss_error("Vpss MeMV complete error(not init).\n");
        return HI_FAILURE;
    }

    memv->cnt++;
    memv->first_ref = memv->first_ref->next;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_memv_reset(vpss_stt_memv *memv)
{
    if (memv == HI_NULL) {
        vpss_warn("Vpss MeMV complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (memv->init == HI_FALSE) {
        vpss_warn("Vpss MeMV complete error(not init).\n");
        return HI_FAILURE;
    }

    memv->cnt = 0;

    if ((memv->vpss_mem.vir_addr != HI_NULL) &&
        ((memv->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (memv->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(memv->vpss_mem.vir_addr, 0, memv->vpss_mem.size);
    }

    if ((memv->vpss_mem_for_tnr.vir_addr != HI_NULL) &&
        ((memv->vpss_mem_for_tnr.mode != OSAL_SECMMZ_TYPE) ||
         (memv->vpss_mem_for_tnr.mode != OSAL_SECSMMU_TYPE))) {
        memset(memv->vpss_mem_for_tnr.vir_addr, 0, memv->vpss_mem_for_tnr.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

