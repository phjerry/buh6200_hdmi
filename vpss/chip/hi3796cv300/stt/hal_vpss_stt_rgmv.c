/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_rgmv.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_rgmv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void rgmv_cal_buf_size(vpss_stt_channel_attr *rgmv_chan_attr, vpss_stt_comm_attr *attr)
{
    if (attr->width > 960) { /* 960:para */
        rgmv_chan_attr->width = (attr->width / 2 + 33) / 64; /* 2,33,64:para */
    } else {
        rgmv_chan_attr->width = (attr->width / 1 + 33) / 64; /* 1,33,64:para */
    }

    if (attr->interlace == HI_TRUE) {
        rgmv_chan_attr->height = (attr->height / 2 + 9) / 16; /* 2,9,16:para */
    } else {
        rgmv_chan_attr->height = (attr->height + 9) / 16; /* 9,16:para */
    }

    rgmv_chan_attr->stride = (rgmv_chan_attr->width * 64 + 127) / 128 * 16; /* 64,127,128,16:para */
    rgmv_chan_attr->size = (rgmv_chan_attr->stride) * rgmv_chan_attr->height;
    return;
}

hi_s32 vpss_stt_rgmv_init(vpss_stt_rgmv *rgmv, vpss_stt_comm_attr *comm_attr)
{
    list *list_head = HI_NULL;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if ((rgmv == HI_NULL) || (comm_attr == HI_NULL)) {
        vpss_error("Vpss RGMV init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (rgmv->init == HI_TRUE) {
        (hi_void) vpss_stt_rgmv_deinit(rgmv);
    }

    memset(rgmv, 0, sizeof(vpss_stt_rgmv));
    rgmv_cal_buf_size(&(rgmv->rgmv_channel_attr), comm_attr);
    attr.name = "vpss_sttRgmvBuf";
    attr.size = rgmv->rgmv_channel_attr.size * VPSS_RGME_MAX_NODE;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(rgmv->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS RGMV init alloc memory failed.\n");
        return HI_FAILURE;
    }

    phy_addr = rgmv->vpss_mem.phy_addr;
    vir_addr = rgmv->vpss_mem.vir_addr;
    list_head = &(rgmv->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    rgmv->first_ref = list_head;
    osal_list_add_tail(&(rgmv->data_list[1].node), list_head);
    osal_list_add_tail(&(rgmv->data_list[2].node), list_head); /* 2:array num */
    rgmv->data_list[0].phy_addr = phy_addr;
    rgmv->data_list[0].vir_addr = vir_addr;
    phy_addr = phy_addr + rgmv->rgmv_channel_attr.size;
    vir_addr = vir_addr + rgmv->rgmv_channel_attr.size;
    rgmv->data_list[1].phy_addr = phy_addr;
    rgmv->data_list[1].vir_addr = vir_addr;
    phy_addr = phy_addr + rgmv->rgmv_channel_attr.size;
    vir_addr = vir_addr + rgmv->rgmv_channel_attr.size;
    rgmv->data_list[2].phy_addr = phy_addr; /* 2:array num */
    rgmv->data_list[2].vir_addr = vir_addr; /* 2:array num */
    rgmv->init = HI_TRUE;
    return HI_SUCCESS;
}

// RGME运动信息队列去初始化
hi_s32 vpss_stt_rgmv_deinit(vpss_stt_rgmv *rgmv)
{
    if (rgmv == HI_NULL) {
        vpss_error("Vpss RGMV DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (rgmv->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(rgmv->vpss_mem));
    }

    if (rgmv->init == HI_FALSE) {
        vpss_warn("Vpss RGMV DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(rgmv, 0, sizeof(vpss_stt_rgmv));
    return HI_SUCCESS;
}

// 获取RGME运动信息
hi_s32 vpss_stt_rgmv_get_cfg(vpss_stt_rgmv *rgmv, vpss_stt_rgmv_cfg *rgme_cfg)
{
    vpss_stt_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((rgmv == HI_NULL) || (rgme_cfg == HI_NULL)) {
        vpss_error("Vpss RGMV GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (rgmv->init == HI_FALSE) {
        vpss_error("Vpss RGMV GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = rgmv->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    rgme_cfg->p2_read_addr = ref_node_data->phy_addr;
    rgme_cfg->p2_read_vir_addr = ref_node_data->vir_addr;
    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    rgme_cfg->p1_read_addr = ref_node_data->phy_addr;
    rgme_cfg->p1_read_vir_addr = ref_node_data->vir_addr;
    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    rgme_cfg->cur_write_addr = ref_node_data->phy_addr;
    rgme_cfg->cur_write_vir_addr = ref_node_data->vir_addr;
    rgme_cfg->width = rgmv->rgmv_channel_attr.width;
    rgme_cfg->height = rgmv->rgmv_channel_attr.height;
    rgme_cfg->stride = rgmv->rgmv_channel_attr.stride;
    rgme_cfg->size = rgmv->rgmv_channel_attr.size;
    return HI_SUCCESS;
}

// RGME运动信息队列轮转
hi_s32 vpss_stt_rgmv_complete(vpss_stt_rgmv *rgmv)
{
    if (rgmv == HI_NULL) {
        vpss_error("Vpss RGMV complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (rgmv->init == HI_FALSE) {
        vpss_error("Vpss RGMV complete error(not init).\n");
        return HI_FAILURE;
    }

    rgmv->cnt++;
    rgmv->first_ref = rgmv->first_ref->next;
    return HI_SUCCESS;
}

// RGME运动信息队列reset
hi_s32 vpss_stt_rgmv_reset(vpss_stt_rgmv *rgmv)
{
    if (rgmv == HI_NULL) {
        vpss_warn("Vpss RGMV complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (rgmv->init == HI_FALSE) {
        vpss_warn("Vpss RGMV complete error(not init).\n");
        return HI_FAILURE;
    }

    rgmv->cnt = 0;

    if ((rgmv->vpss_mem.vir_addr != HI_NULL) &&
        ((rgmv->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (rgmv->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(rgmv->vpss_mem.vir_addr, 0, rgmv->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

