/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_global.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_global.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 vpss_stt_global_init(vpss_stt_global *stt, vpss_stt_mode stt_mode)
{
    hi_s32 ret;
    hi_u32 size;
    hi_u64 phy_addr;
    list *list_head = HI_NULL;
    drv_vpss_mem_attr attr = { 0 };

    if (stt == HI_NULL) {
        vpss_error("Vpss STT Init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (stt->init == HI_TRUE) {
        vpss_stt_global_deinit(stt);
    }

    if (stt_mode == STT_MODE_GLOBAL) {
        attr.name = "vpss_sttGlobalBuf";
        size = VPSS_STTGLOBAL_SIZE;
    } else {
        attr.name = "vpss_sttMeBuf";
        size = VPSS_STTME_SIZE;
    }

    attr.size = size * 2; /* 2:para */
    attr.mode = OSAL_MMZ_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(stt->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS STT init alloc memory failed.\n");
        return HI_FAILURE;
    }

    phy_addr = stt->vpss_mem.phy_addr;
    list_head = &(stt->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    stt->first_ref = list_head;
    osal_list_add_tail(&(stt->data_list[1].node), list_head);
    stt->data_list[0].phy_addr = phy_addr;
    stt->data_list[0].vir_addr = stt->vpss_mem.vir_addr;
    phy_addr = phy_addr + size;
    stt->data_list[1].phy_addr = phy_addr;
    stt->data_list[1].vir_addr = stt->vpss_mem.vir_addr + size;
    stt->stt_mode = stt_mode;
    stt->init = HI_TRUE;
    stt->cnt = 0;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_global_deinit(vpss_stt_global *stt)
{
    if (stt == HI_NULL) {
        vpss_error("Vpss STT DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (stt->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(stt->vpss_mem));
    }

    if (stt->init == HI_FALSE) {
        vpss_warn("Vpss STT DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(stt, 0, sizeof(vpss_stt_global));
    return HI_SUCCESS;
}

hi_s32 vpss_stt_global_reset(vpss_stt_global *stt)
{
    if (stt == HI_NULL) {
        vpss_error("Vpss STT Complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (stt->init == HI_FALSE) {
        vpss_warn("Vpss STT Complete error(not init).\n");
        return HI_FAILURE;
    }

    stt->cnt = 0;

    if ((stt->vpss_mem.vir_addr != HI_NULL) &&
        ((stt->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (stt->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(stt->vpss_mem.vir_addr, 0, stt->vpss_mem.size);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_stt_global_complete(vpss_stt_global *stt)
{
    if (stt == HI_NULL) {
        vpss_error("Vpss STT Complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (stt->init == HI_FALSE) {
        vpss_error("Vpss STT Complete error(not init).\n");
        return HI_SUCCESS;
    }

    // keep interface
    stt->cnt++;
    stt->first_ref = stt->first_ref->next;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_global_get_info(vpss_stt_global *stt, vpss_stt_global_cfg *stt_global_cfg)
{
    vpss_sttglobal_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((stt == HI_NULL) || (stt_global_cfg == HI_NULL)) {
        vpss_error("Vpss STT getcfg error(null pointer).\n");
        return HI_FAILURE;
    }

    if (stt->init == HI_FALSE) {
        vpss_error("Vpss STT getcfg error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = stt->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_sttglobal_data, node);
    stt_global_cfg->read_phy_addr = ref_node_data->phy_addr;
    stt_global_cfg->read_vir_addr = ref_node_data->vir_addr;
    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_sttglobal_data, node);
    stt_global_cfg->write_phy_addr = ref_node_data->phy_addr;
    stt_global_cfg->write_vir_addr = ref_node_data->vir_addr;
    stt_global_cfg->size = (stt->stt_mode == STT_MODE_GLOBAL) ? VPSS_STTGLOBAL_SIZE : VPSS_STTME_SIZE;
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


