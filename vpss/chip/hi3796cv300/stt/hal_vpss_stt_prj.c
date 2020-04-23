/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_prj.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_prj.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void rgme_cal_buf_size(vpss_stt_channel_attr *prjv_channel_attr,
    vpss_stt_channel_attr *prjh_channel_attr, vpss_stt_comm_attr *attr)
{
    if (attr->width > 960) { /* 960:para */
        prjh_channel_attr->width = (attr->width / 2 + 33) / 64 * 128; /* 2,33,64,128:para */
        prjv_channel_attr->width = (attr->width / 2 + 33) / 64 * 32; /* 2,33,64,32:para */
    } else {
        prjh_channel_attr->width = (attr->width / 1 + 33) / 64 * 128; /* 1,33,64,128:para */
        prjv_channel_attr->width = (attr->width / 1 + 33) / 64 * 32; /* 1,33,64,32:para */
    }

    if (attr->interlace == HI_TRUE) {
        prjv_channel_attr->height = (attr->height / 2 + 9) / 16; /* 2,9,16:para */
    } else {
        prjv_channel_attr->height = (attr->height + 9) / 16; /* 9,16:para */
    }

    prjh_channel_attr->height = prjv_channel_attr->height;
    prjh_channel_attr->stride = (prjh_channel_attr->width * 16 + 127) / 128 * 16; /* 16,127,128,16:para */
    prjv_channel_attr->stride = (prjv_channel_attr->width * 16 + 127) / 128 * 16; /* 16,127,128,16:para */
    prjh_channel_attr->size = (prjh_channel_attr->stride) * prjh_channel_attr->height;
    prjv_channel_attr->size = (prjv_channel_attr->stride) * prjv_channel_attr->height;
    return;
}

hi_s32 vpss_stt_prj_init(vpss_stt_prj *prj, vpss_stt_comm_attr *comm_attr)
{
    list *list_head = HI_NULL;
    hi_u32 node_buff_size;
    hi_u32 node_num = 0;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if ((prj == HI_NULL) || (comm_attr == HI_NULL)) {
        vpss_error("Vpss Rgme init error(null pointer).\n");
        return HI_FAILURE;
    }

    if (prj->init == HI_TRUE) {
        (hi_void) vpss_stt_prj_deinit(prj);
    }

    memset(prj, 0, sizeof(vpss_stt_prj));
    prj->interlace = comm_attr->interlace;
    rgme_cal_buf_size(&(prj->prjv_channel_attr), &(prj->prjh_channel_attr), comm_attr);

    if (comm_attr->interlace == HI_TRUE) {
        node_num = 3; /* 3:para */
    } else {
        node_num = 2; /* 2:para */
    }

    node_buff_size = prj->prjh_channel_attr.size + prj->prjv_channel_attr.size;
    attr.name = "vpss_sttRgmeBuf";
    attr.size = node_buff_size * node_num;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(prj->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS Rgme init alloc memory failed.\n");
        return HI_FAILURE;
    }

    phy_addr = prj->vpss_mem.phy_addr;
    vir_addr = prj->vpss_mem.vir_addr;
    list_head = &(prj->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    prj->first_ref = list_head;
    osal_list_add_tail(&(prj->data_list[1].node), list_head);
    prj->data_list[0].phy_addr = phy_addr;
    prj->data_list[0].vir_addr = vir_addr;
    phy_addr = phy_addr + node_buff_size;
    vir_addr = vir_addr + node_buff_size;
    prj->data_list[1].phy_addr = phy_addr;
    prj->data_list[1].vir_addr = vir_addr;

    if (comm_attr->interlace == HI_TRUE) {
        osal_list_add_tail(&(prj->data_list[2].node), list_head); /* 2:array num */
        phy_addr = phy_addr + node_buff_size;
        vir_addr = vir_addr + node_buff_size;
        prj->data_list[2].phy_addr = phy_addr; /* 2:array num */
        prj->data_list[2].vir_addr = vir_addr; /* 2:array num */
    }

    prj->init = HI_TRUE;
    return HI_SUCCESS;
}

// RGME运动信息队列去初始化
hi_s32 vpss_stt_prj_deinit(vpss_stt_prj *prj)
{
    if (prj == HI_NULL) {
        vpss_error("Vpss Rgme DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (prj->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(prj->vpss_mem));
    }

    if (prj->init == HI_FALSE) {
        vpss_warn("Vpss Rgme DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(prj, 0, sizeof(vpss_stt_prj));
    return HI_SUCCESS;
}

// 获取RGME运动信息
hi_s32 vpss_stt_prj_get_cfg(vpss_stt_prj *prj, vpss_stt_prj_cfg *prjh_cfg, vpss_stt_prj_cfg *prjv_cfg)
{
    vpss_stt_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((prj == HI_NULL) || (prjh_cfg == HI_NULL) || (prjv_cfg == HI_NULL)) {
        vpss_error("Vpss Rgme GetRefInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (prj->init == HI_FALSE) {
        vpss_error("Vpss Rgme GetRefInfo error(not init).\n");
        return HI_FAILURE;
    }

    ref_node = prj->first_ref;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    prjh_cfg->read_addr = ref_node_data->phy_addr;
    prjh_cfg->read_vir_addr = ref_node_data->vir_addr;
    prjv_cfg->read_addr = ref_node_data->phy_addr + prj->prjh_channel_attr.size;
    prjv_cfg->read_vir_addr = prjh_cfg->read_vir_addr + prj->prjh_channel_attr.size;

    if (prj->interlace == HI_TRUE) {
        ref_node = ref_node->next;
    }

    ref_node = ref_node->next;
    ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
    prjh_cfg->write_addr = ref_node_data->phy_addr;
    prjh_cfg->write_vir_addr = ref_node_data->vir_addr;
    prjv_cfg->write_addr = ref_node_data->phy_addr + prj->prjh_channel_attr.size;
    prjv_cfg->write_vir_addr = ref_node_data->vir_addr + prj->prjh_channel_attr.size;
    prjh_cfg->stride = prj->prjh_channel_attr.stride;
    prjh_cfg->height = prj->prjh_channel_attr.height;
    prjh_cfg->width = prj->prjh_channel_attr.width;
    prjh_cfg->size = prj->prjh_channel_attr.size;
    prjv_cfg->stride = prj->prjv_channel_attr.stride;
    prjv_cfg->height = prj->prjv_channel_attr.height;
    prjv_cfg->width = prj->prjv_channel_attr.width;
    prjv_cfg->size = prj->prjv_channel_attr.size;
    return HI_SUCCESS;
}

// RGME运动信息队列轮转
hi_s32 vpss_stt_prj_complete(vpss_stt_prj *prj)
{
    if (prj == HI_NULL) {
        vpss_error("Vpss Rgme complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (prj->init == HI_FALSE) {
        vpss_error("Vpss Rgme complete error(not init).\n");
        return HI_FAILURE;
    }

    prj->cnt++;
    prj->first_ref = prj->first_ref->next;
    return HI_SUCCESS;
}

// RGME运动信息队列reset
hi_s32 vpss_stt_prj_reset(vpss_stt_prj *prj)
{
    if (prj == HI_NULL) {
        vpss_warn("Vpss Rgme complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (prj->init == HI_FALSE) {
        vpss_warn("Vpss Rgme complete error(not init).\n");
        return HI_FAILURE;
    }

    prj->cnt = 0;

    if ((prj->vpss_mem.vir_addr != HI_NULL) &&
        ((prj->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (prj->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(prj->vpss_mem.vir_addr, 0, prj->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

