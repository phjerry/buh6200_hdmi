/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt_nrmad.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#include "hal_vpss_stt_nrmad.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void nrmad_cal_channel_attr(vpss_stt_channel_attr *nr_mad_channel_attr, vpss_stt_comm_attr *attr)
{
    nr_mad_channel_attr->width = attr->width;

    if (attr->interlace == HI_TRUE) {
        nr_mad_channel_attr->height = attr->height / 2; /* 2:para */
    } else {
        nr_mad_channel_attr->height = attr->height;
    }

    /* 3,4,4,5,127,128,16:para */
    nr_mad_channel_attr->stride = (((nr_mad_channel_attr->width + 3) / 4) * 4 * 5 + 127) / 128 * 16;
    nr_mad_channel_attr->size = nr_mad_channel_attr->stride * nr_mad_channel_attr->height;

    return;
}

hi_u32 nrmad_get_total_buf_size(vpss_stt_comm_attr *attr, hi_u32 node_buf_size)
{
    if (attr->interlace == HI_TRUE) {
        return node_buf_size * 3; /* 3:para */
    } else {
        return node_buf_size * 1;
    }
}

hi_s32 vpss_stt_nrmad_init(vpss_nrmadinfo *nr_mad_info, vpss_stt_comm_attr *comm_attr)
{
    list *list_head = HI_NULL;
    hi_u32 total_buff_size;
    hi_u64 phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_s32 ret;
    drv_vpss_mem_attr attr = { 0 };

    if (nr_mad_info == HI_NULL) {
        vpss_error("Vpss NrMadInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_mad_info->init == HI_TRUE) {
        (hi_void) vpss_stt_nrmad_deinit(nr_mad_info);
    }

    memset(nr_mad_info, 0, sizeof(vpss_nrmadinfo));
    nr_mad_info->interlace = comm_attr->interlace;
    list_head = &(nr_mad_info->data_list[0].node);
    OSAL_INIT_LIST_HEAD(list_head);
    nr_mad_info->first_ref = list_head;
    nrmad_cal_channel_attr(&nr_mad_info->nr_mad_channel_attr, comm_attr);
    total_buff_size = nrmad_get_total_buf_size(comm_attr, nr_mad_info->nr_mad_channel_attr.size);
    attr.name = "vpss_sttNrMadBuf";
    attr.size = total_buff_size;
    attr.mode = OSAL_NSSMMU_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = vpss_comm_mem_alloc(&attr, &(nr_mad_info->vpss_mem));
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS NrMad Alloc memory failed.\n");
        return HI_FAILURE;
    }

    phy_addr = nr_mad_info->vpss_mem.phy_addr;
    vir_addr = nr_mad_info->vpss_mem.vir_addr;

    if (comm_attr->interlace == HI_TRUE) {
        osal_list_add_tail(&(nr_mad_info->data_list[1].node), list_head);
        osal_list_add_tail(&(nr_mad_info->data_list[2].node), list_head); /* 2:array num */
        nr_mad_info->data_list[0].phy_addr = phy_addr;
        nr_mad_info->data_list[0].vir_addr = vir_addr;
        phy_addr = phy_addr + nr_mad_info->nr_mad_channel_attr.size;
        vir_addr = vir_addr + nr_mad_info->nr_mad_channel_attr.size;
        nr_mad_info->data_list[1].phy_addr = phy_addr;
        nr_mad_info->data_list[1].vir_addr = vir_addr;
        phy_addr = phy_addr + nr_mad_info->nr_mad_channel_attr.size;
        vir_addr = vir_addr + nr_mad_info->nr_mad_channel_attr.size;
        nr_mad_info->data_list[2].phy_addr = phy_addr; /* 2:array num */
        nr_mad_info->data_list[2].vir_addr = vir_addr; /* 2:array num */
    }

    nr_mad_info->init = HI_TRUE;
    nr_mad_info->cnt = 0;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrmad_deinit(vpss_nrmadinfo *nr_mad_info)
{
    if (nr_mad_info == HI_NULL) {
        vpss_error("Vpss NrMad DeInit error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_mad_info->vpss_mem.phy_addr != 0) {
        vpss_comm_mem_free(&(nr_mad_info->vpss_mem));
    }

    if (nr_mad_info->init == HI_FALSE) {
        vpss_warn("Vpss NrMad DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    memset(nr_mad_info, 0, sizeof(vpss_nrmadinfo));
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrmad_get_info(vpss_nrmadinfo *nr_mad_info, vpss_nrmadcfg *nr_mad_cfg)
{
    vpss_stt_data *ref_node_data = HI_NULL;
    list *ref_node = HI_NULL;

    if ((nr_mad_info == HI_NULL) || (nr_mad_cfg == HI_NULL)) {
        vpss_error("Vpss NrMad GetInfo error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_mad_info->init == HI_FALSE) {
        vpss_error("Vpss NrMad GetInfo error(not init).\n");
        return HI_FAILURE;
    }

    if (nr_mad_info->interlace == HI_TRUE) {
        ref_node = nr_mad_info->first_ref;
        ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
        nr_mad_cfg->tnr_read_addr = ref_node_data->phy_addr;
        nr_mad_cfg->tnr_read_vir_addr = ref_node_data->vir_addr;
        ref_node = ref_node->next;  // NX1
        ref_node = ref_node->next;  // NX2
        ref_node_data = osal_list_entry(ref_node, vpss_stt_data, node);
        nr_mad_cfg->tnr_write_addr = ref_node_data->phy_addr;
        nr_mad_cfg->write_vir_addr = ref_node_data->vir_addr;
        nr_mad_cfg->snr_read_addr = nr_mad_cfg->tnr_read_addr;
        nr_mad_cfg->snr_read_vir_addr = ref_node_data->vir_addr;
    } else {
        nr_mad_cfg->tnr_read_addr = nr_mad_info->vpss_mem.phy_addr;
        nr_mad_cfg->tnr_read_vir_addr = nr_mad_info->vpss_mem.vir_addr;
        nr_mad_cfg->tnr_write_addr = nr_mad_cfg->tnr_read_addr;
        nr_mad_cfg->write_vir_addr = nr_mad_info->vpss_mem.vir_addr;
        nr_mad_cfg->snr_read_addr = nr_mad_cfg->tnr_read_addr;
        nr_mad_cfg->snr_read_vir_addr = nr_mad_info->vpss_mem.vir_addr;
    }

    nr_mad_cfg->width = nr_mad_info->nr_mad_channel_attr.width;
    nr_mad_cfg->height = nr_mad_info->nr_mad_channel_attr.height;
    nr_mad_cfg->u32stride = nr_mad_info->nr_mad_channel_attr.stride;
    nr_mad_cfg->size = nr_mad_info->nr_mad_channel_attr.size;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrmad_complete(vpss_nrmadinfo *nr_mad_info)
{
    if (nr_mad_info == HI_NULL) {
        vpss_error("Vpss NrMad complete error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_mad_info->init == HI_FALSE) {
        vpss_error("Vpss NrMad complete error(not init).\n");
        return HI_FAILURE;
    }

    nr_mad_info->cnt++;
    nr_mad_info->first_ref = nr_mad_info->first_ref->next;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_nrmad_reset(vpss_nrmadinfo *nr_mad_info)
{
    if (nr_mad_info == HI_NULL) {
        vpss_warn("Vpss NrMad Reset error(null pointer).\n");
        return HI_FAILURE;
    }

    if (nr_mad_info->init == HI_FALSE) {
        vpss_warn("Vpss NrMad Reset error(not init).\n");
        return HI_FAILURE;
    }

    nr_mad_info->cnt = 0;

    if ((nr_mad_info->vpss_mem.vir_addr != HI_NULL) &&
        ((nr_mad_info->vpss_mem.mode != OSAL_SECMMZ_TYPE) ||
         (nr_mad_info->vpss_mem.mode != OSAL_SECSMMU_TYPE))) {
        memset(nr_mad_info->vpss_mem.vir_addr, 0, nr_mad_info->vpss_mem.size);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

