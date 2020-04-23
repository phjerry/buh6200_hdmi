/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_in.c source file vpss in
 * Author: zhangjunyu
 * Create: 2016/06/13
 */
#include "drv_vpss_in.h"
#include "vpss_policy.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static inline hi_bool vpss_src_is_field_all(vpss_src_data *data)
{
    return (data->comm_frame.field_mode == HI_DRV_FIELD_ALL);
}

static inline hi_bool vpss_src_is_top_first(vpss_src_data *data)
{
    return data->comm_frame.top_field_first;
}

static inline hi_bool vpss_src_is_full(vpss_src *src)
{
    return osal_list_empty_careful(&(src->empty_src_list));
}

hi_bool vpss_src_is_empty(vpss_src *src)
{
    return osal_list_empty_careful(&(src->ful_src_list));
}

static inline hi_bool vpss_src_is_some_space_left(vpss_src *src, hi_u32 numb)
{
    hi_u32 cnt = 0;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    osal_list_for_each_safe(pos, n, &(src->empty_src_list)) {
        cnt++;
    }
    return (cnt >= numb);
}

static inline hi_bool vpss_src_is_undo_data_left(vpss_src *src)
{
    return (src->src_target_1->next != &(src->ful_src_list));
}
static inline vpss_src_data *vpss_src_get_data(list *node_ptr)
{
    vpss_src_node *node;
    node = osal_list_entry(node_ptr, vpss_src_node, node);
    return &(node->src_data);
}
static inline hi_void vpss_src_add_ful_node(vpss_src *src, vpss_src_node *data_node)
{
    osal_list_add_tail(&(data_node->node), &(src->ful_src_list));
}
static inline hi_void vpss_src_add_empty_node(vpss_src *src, vpss_src_node *data_node)
{
    osal_list_add_tail(&(data_node->node), &(src->empty_src_list));
}

static vpss_src_node *vpss_src_del_empty_node(vpss_src *src)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_src_node *target = HI_NULL;
    osal_list_for_each_safe(pos, n, &(src->empty_src_list)) {
        target = osal_list_entry(pos, vpss_src_node, node);
        osal_list_del_init(pos);
        break;
    }

    if (target != HI_NULL) {
        memset(&(target->src_data), 0, sizeof(vpss_src_data));
        return target;
    } else {
        return HI_NULL;
    }
}

hi_void release_src_target_node(vpss_src *src, list *target_node)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    list *head = HI_NULL;
    vpss_src_node *node = HI_NULL;
    head = &(src->ful_src_list);

    for (pos = (head)->next, n = pos->next;
         pos != target_node
         && pos != head;
         pos = n, n = pos->next) {
        node = osal_list_entry(pos, vpss_src_node, node);

        if (pos == src->src_target_1) {
            src->src_target_1 = src->src_target_1->prev;
        }

        osal_list_del_init(&(node->node));
        vpss_src_add_empty_node(src, node);
    }
}

hi_s32 vpss_src_rls_done_full_node(vpss_src *src)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    list *head = HI_NULL;
    list *list_new_first_node = HI_NULL;
    vpss_src_data *fst_data = HI_NULL;
    vpss_src_data *snd_data = HI_NULL;
    head = &(src->ful_src_list);

    if ((src->mode == SRC_MODE_FIELD) ||
        (src->mode == SRC_MODE_NTSC) ||
        (src->mode == SRC_MODE_PAL)) {
        pos = (head)->next;
        n = pos->next;
        list_new_first_node = n->next;

        if (pos == head || n == head) {
            vpss_error("can't get first two released nodes\n");
            return HI_FAILURE;
        }

        fst_data = vpss_src_get_data(src->ful_src_list.next);
        snd_data = vpss_src_get_data(src->ful_src_list.next->next);
        if (fst_data->comm_frame.frame_index != snd_data->comm_frame.frame_index) {
            vpss_error("not same frame(%d), (%d)!\n",
                       fst_data->comm_frame.frame_index, snd_data->comm_frame.frame_index);
            return HI_FAILURE;
        }

        if (fst_data->comm_frame.field_mode == HI_DRV_FIELD_TOP) {
            src->rls_src_priv_image(src->h_src_module, fst_data);
        } else {
            src->rls_src_priv_image(src->h_src_module, snd_data);
        }
    } else {
        pos = (head)->next;
        list_new_first_node = pos->next;

        if (pos == head) {
            vpss_error("can't get first one released nodes\n");
            return HI_FAILURE;
        }

        fst_data = vpss_src_get_data(src->ful_src_list.next);
        src->rls_src_priv_image(src->h_src_module, fst_data);
    }

    release_src_target_node(src, list_new_first_node);
    src->release_src_count++;
    src->release_src_total++;
    src->ful_src_list_num--;
    return HI_SUCCESS;
}

hi_s32 vpss_src_flush(vpss_src *src)
{
    vpss_src_node *fst_node = HI_NULL;
    vpss_src_node *snd_node = HI_NULL;
    vpss_src_data *fst_data = HI_NULL;
    vpss_src_data *snd_data = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;

    vpss_check_null(src);

    if (!src->init) {
        vpss_dbg("Delint failed(not init).\n");
        return HI_FAILURE;
    }

    osal_list_for_each_safe(pos, n, &(src->ful_src_list)) {
        if ((src->mode == SRC_MODE_FRAME) || (src->mode == SRC_MODE_SPEC_FIELD)) {
            fst_node = osal_list_entry(pos, vpss_src_node, node);
            fst_data = vpss_src_get_data(pos);

            src->rls_src_priv_image(src->h_src_module, fst_data);
            src->release_src_total++;
        } else {
            fst_node = osal_list_entry(pos, vpss_src_node, node);
            snd_node = osal_list_entry(n, vpss_src_node, node);
            fst_data = vpss_src_get_data(pos);
            snd_data = vpss_src_get_data(n);
            if (fst_data->comm_frame.frame_index != snd_data->comm_frame.frame_index) {
                vpss_error("not same frame(%d), (%d)!\n",
                    fst_data->comm_frame.frame_index, snd_data->comm_frame.frame_index);
                src->rls_src_priv_image(src->h_src_module, fst_data);
                src->rls_src_priv_image(src->h_src_module, snd_data);
            } else {
                if (fst_data->comm_frame.field_mode == HI_DRV_FIELD_TOP) {
                    src->rls_src_priv_image(src->h_src_module, fst_data);
                } else {
                    src->rls_src_priv_image(src->h_src_module, snd_data);
                }
            }

            src->release_src_total++;
            n = n->next;
        }
    }
    return HI_SUCCESS;
}

hi_s32 vpss_src_reset(vpss_src *src)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_src_node *src_node = HI_NULL;
    vpss_check_null(src);

    if (!src->init) {
        vpss_dbg("VPSS src reset failed(not init).\n");
        return HI_FAILURE;
    }

    osal_list_for_each_safe(pos, n, &(src->ful_src_list)) {
        src_node = osal_list_entry(pos, vpss_src_node, node);
        osal_list_del_init(pos);
        vpss_src_add_empty_node(src, src_node);
    }
    src->src_target_1 = &(src->ful_src_list);
    src->put_src_count = 0;
    src->complete_src_count = 0;
    src->release_src_count = 0;
    src->ful_src_list_num = 0;
    return HI_SUCCESS;
}

hi_s32 put_interlace_frame(vpss_src *src, vpss_src_data *data, hi_bool topfirst)
{
    vpss_src_node *fst_dst_node = HI_NULL;
    vpss_src_node *snd_dst_node = HI_NULL;
    /* del 1st node from empty node, fill src_node_info, move to src full node list */
    fst_dst_node = vpss_src_del_empty_node(src);
    if (fst_dst_node == HI_NULL) {
        return HI_FAILURE;
    }

    memcpy(&(fst_dst_node->src_data), data, sizeof(vpss_src_data));
    vpss_src_add_ful_node(src, fst_dst_node);

    /* del 2nd node from empty node, fill src_node_info, move to src full node list */
    snd_dst_node = vpss_src_del_empty_node(src);
    if (snd_dst_node == HI_NULL) {
        return HI_FAILURE;
    }

    memcpy(&(snd_dst_node->src_data), data, sizeof(vpss_src_data));
    vpss_src_add_ful_node(src, snd_dst_node);

    if (data->comm_frame.video_private.last_flag == HI_DRV_LAST_ERROR_FLAG) {
        fst_dst_node->src_data.comm_frame.video_private.last_flag = HI_DRV_LAST_ERROR_FLAG;
    }

    if (topfirst) {
        fst_dst_node->src_data.comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        fst_dst_node->src_data.comm_frame.field_mode = HI_DRV_FIELD_TOP;
        snd_dst_node->src_data.comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        snd_dst_node->src_data.comm_frame.field_mode = HI_DRV_FIELD_BOTTOM;
    } else {
        fst_dst_node->src_data.comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        fst_dst_node->src_data.comm_frame.field_mode = HI_DRV_FIELD_BOTTOM;
        snd_dst_node->src_data.comm_frame.sample_type = HI_DRV_SAMPLE_TYPE_INTERLACE;
        snd_dst_node->src_data.comm_frame.field_mode = HI_DRV_FIELD_TOP;
    }

    fst_dst_node->src_data.comm_frame.frame_rate *= 2;  /* output framerate is input framerate * 2 when interlace */
    snd_dst_node->src_data.comm_frame.frame_rate *= 2;  /* output framerate is input framerate * 2 when interlace */
#ifdef DPT
    snd_dst_node->src_data.comm_frame.pts = fst_dst_node->src_data.comm_frame.pts +
        data->vdec_frame_info.inter_pts_delta;
#else
    snd_dst_node->src_data.comm_frame.pts = fst_dst_node->src_data.comm_frame.pts + data->comm_frame.src_pts;
#endif
    return HI_SUCCESS;
}

hi_s32 vpss_src_put_image(vpss_src *src, vpss_src_data *data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_bool field_all = HI_FALSE;
    hi_bool topfirst;
    hi_bool enough_space;
    vpss_src_node *dst_node = HI_NULL;

    vpss_check_null(src);

    if (!src->init) {
        vpss_error("vpss_src_put_image delint.\n");
        return HI_FAILURE;
    }

    src->put_src_count++;
    src->put_src_total++;
    field_all = vpss_src_is_field_all(data);
    topfirst = vpss_src_is_top_first(data);

    if ((data->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) || (field_all == HI_FALSE)) {
        enough_space = vpss_src_is_some_space_left(src, DEF_SRC_MODE_FRAME_NUMB);
    } else {
        enough_space = vpss_src_is_some_space_left(src, DEF_SRC_MODE_FIELD_NUMB);
    }

    if (!enough_space) {
        vpss_error("there is not enough space.\n");
        return HI_FAILURE;
    }

    if ((data->comm_frame.sample_type == HI_DRV_SAMPLE_TYPE_PROGRESSIVE) || (field_all == HI_FALSE)) {
        dst_node = vpss_src_del_empty_node(src);
        if (dst_node == HI_NULL) {
            return HI_FAILURE;
        }

        memcpy(&(dst_node->src_data), data, sizeof(vpss_src_data));
        vpss_src_add_ful_node(src, dst_node);
    } else {
        ret = put_interlace_frame(src, data, topfirst);
    }

    src->ful_src_list_num++;
    return ret;
}

hi_s32 vpss_src_get_process_image(vpss_src *src, vpss_src_data **data)
{
    hi_s32 ret = HI_SUCCESS;
    vpss_check_null(src);

    if (!src->init) {
        vpss_dbg("vpss_src_get_process_image delint.\n");
        return HI_FAILURE;
    }

    if (vpss_src_is_undo_data_left(src)) {
        *data = vpss_src_get_data(src->src_target_1->next);
        ret = HI_SUCCESS;
    } else {
        *data = HI_NULL;
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 vpss_src_move_next(vpss_src *src, hi_u32 next_count)
{
    if (!src->init) {
        vpss_error("vpss_src_get_pre_img_info delint.\n");
        return HI_FAILURE;
    }

    if ((&(src->ful_src_list) == src->src_target_1->next) ||
        (&(src->ful_src_list) == src->src_target_1->next->next)) {
        vpss_dbg("vpss_src_move_next error.\n");
        return HI_FAILURE;
    }

    src->src_target_1 = src->src_target_1->next;
    return HI_SUCCESS;
}

hi_s32 vpss_src_complete_priv_image(vpss_src *src)
{
    hi_s32 ret = HI_SUCCESS;
    vpss_check_null(src);
    vpss_check_null(src->rls_src_priv_image);

    if (!src->init) {
        vpss_error("vpss_src_complete_priv_image delint.\n");
        return HI_FAILURE;
    }

    if (vpss_src_is_empty(src)) {
        vpss_error("srclist is empty\n");
        return HI_FAILURE;
    }

    src->complete_src_count++;
    src->src_target_1 = src->src_target_1->next;

    if (!vpss_src_is_undo_data_left(src)) {
        if (vpss_src_is_full(src)) {
            ret = vpss_src_rls_done_full_node(src);
        }
    }

    if (ret == HI_FAILURE) {
        vpss_error("complete_image failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_src_show_status(vpss_src *src)
{
    hi_bool find_target = HI_FALSE;
    vpss_src_node *target = HI_NULL;
    vpss_src_data *frame = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;

    osal_printk("\n-----------vpss src buffer status----------\n");
    osal_list_for_each_safe(pos, n, &(src->ful_src_list)) {
        target = osal_list_entry(pos, vpss_src_node, node);
        frame = &target->src_data;

        if (src->src_target_1->next == pos) {
            find_target = HI_TRUE;
        }

        if (find_target) {
            osal_printk("src waiting buffer:\n");
        } else {
            osal_printk("src complete buffer:\n");
        }

        osal_printk("index=%d\n"
                    "L:Y %llx C %llx YH %llx CH %llx YS %d CS %d \n"
                    "R:Y %llx C %llx YH %llx CH %llx YS %d CS %d \n",
                    frame->comm_frame.frame_index,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_y,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_c,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_y_head,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].phy_addr_c_head,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_y,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_LEFT].stride_c,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].phy_addr_y,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].phy_addr_c,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].phy_addr_y_head,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].phy_addr_c_head,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].stride_y,
                    frame->vpss_buf_addr[HI_DRV_3D_EYE_RIGHT].stride_c);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_src_init(vpss_src *src, vpss_src_attr attr)
{
    hi_u32 cnt = 0;
    vpss_check_null(src);
    vpss_check_null(attr.rls_src_priv_image);

    if (src->init) {
        vpss_error("src has been inited\n");
        return HI_SUCCESS;
    }

    memset(src, 0, sizeof(vpss_src));
    src->mode = attr.mode;

    switch (attr.mode) {
        case SRC_MODE_FRAME:
            src->buf_num = DEF_SRC_MODE_FRAME_NUMB;
            break;
        case SRC_MODE_FIELD:
            src->buf_num = DEF_SRC_MODE_FIELD_NUMB;
            break;
        case SRC_MODE_SPEC_FIELD:
            src->buf_num = DEF_SRC_MODE_SPEC_FIELD_NUMB;
            break;
        case SRC_MODE_NTSC:
            src->buf_num = DEF_SRC_MODE_NTSC_NUMB;
            break;
        case SRC_MODE_PAL:
            src->buf_num = DEF_SRC_MODE_PAL_NUMB;
            break;
        default:
            vpss_error("para en_mode %d is invalid.\n", attr.mode);
            return HI_FAILURE;
    }

    src->mode = attr.mode;
    src->h_src_module = attr.h_src_module;
    src->rls_src_priv_image = attr.rls_src_priv_image;
    OSAL_INIT_LIST_HEAD(&(src->empty_src_list));
    OSAL_INIT_LIST_HEAD(&(src->ful_src_list));

    for (cnt = 0; cnt < src->buf_num; cnt++) {
        osal_list_add_tail(&(src->ast_src_node[cnt].node), &(src->empty_src_list));
    }

    src->src_target_1 = &(src->ful_src_list);
    src->init = HI_TRUE;
    return HI_SUCCESS;
}

static hi_void vpss_src_onenode_deinit(vpss_src *src, list *pos)
{
    vpss_src_node *fst_node = HI_NULL;

    fst_node = osal_list_entry(pos, vpss_src_node, node);

    src->rls_src_priv_image(src->h_src_module, &fst_node->src_data);
    osal_list_del_init(pos);
    vpss_src_add_empty_node(src, fst_node);

    return;
}

hi_s32 vpss_src_deinit(vpss_src *src)
{
    vpss_src_node *fst_node = HI_NULL;
    vpss_src_node *snd_node = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    list *tmp = HI_NULL;

    if (src->init != HI_TRUE) {
        return HI_FAILURE;
    }

    osal_list_for_each_safe(pos, n, &(src->ful_src_list)) {
        if ((src->mode == SRC_MODE_FRAME) ||
            (src->mode == SRC_MODE_SPEC_FIELD)) {
            vpss_src_onenode_deinit(src, pos);
        } else {
            fst_node = osal_list_entry(pos, vpss_src_node, node);
            snd_node = osal_list_entry(n, vpss_src_node, node);
            if (fst_node->src_data.comm_frame.frame_index != snd_node->src_data.comm_frame.frame_index) {
                vpss_error("not same frame(%d), (%d)!\n",
                           fst_node->src_data.comm_frame.frame_index, snd_node->src_data.comm_frame.frame_index);
                src->rls_src_priv_image(src->h_src_module, &fst_node->src_data);
                src->rls_src_priv_image(src->h_src_module, &snd_node->src_data);
            } else {
                if (fst_node->src_data.comm_frame.field_mode == HI_DRV_FIELD_TOP) {
                    src->rls_src_priv_image(src->h_src_module, &fst_node->src_data);
                } else {
                    src->rls_src_priv_image(src->h_src_module, &snd_node->src_data);
                }
            }

            tmp = n->next;
            osal_list_del_init(pos);
            osal_list_del_init(n);
            vpss_src_add_empty_node(src, fst_node);
            vpss_src_add_empty_node(src, snd_node);
            n = tmp;
        }
    }
    memset(src, 0, sizeof(vpss_src));

    return HI_SUCCESS;
}

hi_s32 vpss_in_init(vpss_in_attr *attr, vpss_in *in)
{
    hi_u32 i;

    vpss_check_null(in);
    vpss_check_null(attr);
    vpss_check_null(attr->rls_in_undo_priv_image);

    if (in->init) {
        vpss_info("IN has been inited\n");
        return HI_SUCCESS;
    }

    if (vpss_comm_init_spin(&in->src_in_lock) != HI_SUCCESS) {
        vpss_error("Init src_in_lock failed.\n");
        return HI_FAILURE;
    }
    OSAL_INIT_LIST_HEAD(&in->busy_list);
    OSAL_INIT_LIST_HEAD(&in->free_list);
    OSAL_INIT_LIST_HEAD(&in->release_list);
    OSAL_INIT_LIST_HEAD(&in->wait_rel_list);

    for (i = 0; i < DEF_IN_NODE_MAX_NUM; i++) {
        osal_list_add_tail(&in->src_node[i].node, &in->free_list);
    }

    in->init = HI_TRUE;
    in->queue_cnt = 0;
    in->queue_total_cnt = 0;
    in->dequeue_cnt = 0;
    in->dequeue_total_cnt = 0;
    in->acquire_cnt = 0;
    in->acquire_total_cnt = 0;
    in->release_cnt = 0;
    in->release_total_cnt = 0;
    in->h_src_module = attr->h_src_module;
    in->rls_in_undo_priv_image = attr->rls_in_undo_priv_image;

    return HI_SUCCESS;
}

hi_s32 vpss_in_deinit(vpss_in *in)
{
    vpss_check_null(in);

    if (!in->init) {
        vpss_info("IN has been deinited\n");
        return HI_SUCCESS;
    }

    OSAL_INIT_LIST_HEAD(&in->busy_list);
    OSAL_INIT_LIST_HEAD(&in->free_list);
    OSAL_INIT_LIST_HEAD(&in->release_list);
    OSAL_INIT_LIST_HEAD(&in->wait_rel_list);
    in->init = HI_FALSE;
    in->queue_cnt = 0;
    in->queue_total_cnt = 0;
    in->dequeue_cnt = 0;
    in->dequeue_total_cnt = 0;
    in->acquire_cnt = 0;
    in->acquire_total_cnt = 0;
    in->release_cnt = 0;
    in->release_total_cnt = 0;
    in->h_src_module = 0;
    in->rls_in_undo_priv_image = HI_NULL;

    if (in->src_in_lock.lock != HI_NULL) {
        vpss_comm_destory_spin(&in->src_in_lock);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_in_reset(vpss_in *in)
{
    unsigned long flag = 0;
    vpss_in_node *frame_node = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_drv_vpss_video_frame *priv_image = HI_NULL;

    vpss_check_null(in);

    if (!in->init) {
        vpss_error("SRCIN has been deinited\n");
        return HI_SUCCESS;
    }

    priv_image = (hi_drv_vpss_video_frame *)vpss_kmalloc(sizeof(hi_drv_vpss_video_frame), OSAL_GFP_KERNEL);
    if (priv_image == HI_NULL) {
        vpss_error("priv_frame malloc failed! \n");
        return HI_FAILURE;
    }
    memset(priv_image, 0, sizeof(hi_drv_vpss_video_frame));

    vpss_comm_down_spin(&in->src_in_lock, &flag);
    osal_list_for_each_safe(pos, n, &(in->busy_list)) {
        frame_node = osal_list_entry(pos, vpss_in_node, node);
        meta_info = (hi_drv_win_vpss_meta_info *)frame_node->in_data.video_private.win_private.data;
        meta_info->vpss_reset_frame = HI_TRUE;

        vpss_comm_cvt_comm_to_privite_nomap(&(frame_node->in_data), priv_image);

        if (in->rls_in_undo_priv_image != HI_NULL) {
            in->rls_in_undo_priv_image(in->h_src_module, priv_image);
        }

        osal_list_del_init(pos);
        osal_list_add_tail(pos, &in->release_list);
    }
    vpss_comm_up_spin(&in->src_in_lock, &flag);

    in->queue_cnt = 0;
    in->dequeue_cnt = 0;
    in->acquire_cnt = 0;
    in->release_cnt = 0;

    vpss_kfree(priv_image);

    return HI_SUCCESS;
}

static vpss_in_node *buff_have_repeat_frame(list *frame_list, hi_u64 phy_addr_y)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    vpss_in_node *frame_node = HI_NULL;
    osal_list_for_each_safe(pos, n, frame_list) {
        frame_node = osal_list_entry(pos, vpss_in_node, node);
        list_entry_times++;

        if (list_entry_times > DEF_IN_NODE_MAX_NUM) {
            vpss_error("list error\n");
            frame_node = HI_NULL;
            break;
        }

        if (phy_addr_y == frame_node->in_data.buf_addr[0].dma_handle) {
            break;
        }

        frame_node = HI_NULL;
    }

    return frame_node;
}

static vpss_in_node *buff_have_repeat_metadata(list *frame_list, hi_u64 meta_addr)
{
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    vpss_in_node *frame_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list) {
        frame_node = osal_list_entry(pos, vpss_in_node, node);
        list_entry_times++;

        if (list_entry_times > DEF_IN_NODE_MAX_NUM) {
            vpss_error("list error\n");
            frame_node = HI_NULL;
            break;
        }

        if (meta_addr == frame_node->in_data.video_private.win_private.data) {
            break;
        }

        frame_node = HI_NULL;
    }

    return frame_node;
}

static hi_bool vpss_in_check_have_repeat_frame(vpss_in *in, hi_drv_video_frame *frame)
{
    vpss_in_node *target_node = HI_NULL;
    list *frame_list = HI_NULL;

    frame_list = &in->busy_list;
    target_node = buff_have_repeat_frame(frame_list, frame->buf_addr[0].dma_handle);
    if (target_node != HI_NULL) {
        vpss_error("repeat frame in vpss busy list index:%d, addr:0x%llx\n",
                   frame->frame_index, frame->buf_addr[0].dma_handle);
        return HI_TRUE;
    }

    frame_list = &in->release_list;
    target_node = buff_have_repeat_frame(frame_list, frame->buf_addr[0].dma_handle);
    if (target_node != HI_NULL) {
        vpss_error("repeat frame in vpss release list index:%d, addr:0x%llx\n",
                   frame->frame_index, frame->buf_addr[0].dma_handle);
        return HI_TRUE;
    }

    frame_list = &in->wait_rel_list;
    target_node = buff_have_repeat_frame(frame_list, frame->buf_addr[0].dma_handle);
    if (target_node != HI_NULL) {
        vpss_error("repeat frame in vpss wait_rel_list list index:%d, addr:0x%llx\n",
                   frame->frame_index, frame->buf_addr[0].dma_handle);
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_bool vpss_in_check_have_repeat_metadata(vpss_in *in, hi_drv_video_frame *frame)
{
    vpss_in_node *target_node = HI_NULL;
    list *frame_list = HI_NULL;

    frame_list = &in->busy_list;
    target_node = buff_have_repeat_metadata(frame_list, frame->video_private.win_private.data);
    if (target_node != HI_NULL) {
        vpss_error("repeat meta in vpss busy list index:%d, addr:0x%llx\n",
                   frame->frame_index, frame->video_private.win_private.data);
        return HI_TRUE;
    }

    frame_list = &in->release_list;
    target_node = buff_have_repeat_metadata(frame_list, frame->video_private.win_private.data);
    if (target_node != HI_NULL) {
        vpss_error("repeat meta in vpss release list index:%d, addr:0x%llx\n",
                   frame->frame_index, frame->video_private.win_private.data);
        return HI_TRUE;
    }

    frame_list = &in->wait_rel_list;
    target_node = buff_have_repeat_metadata(frame_list, frame->video_private.win_private.data);
    if (target_node != HI_NULL) {
        vpss_error("repeat meta in vpss wait_rel_list list index:%d, addr:0x%llx\n",
                   frame->frame_index, frame->video_private.win_private.data);
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_s32 vpss_in_queue_frame(vpss_in *in, hi_drv_video_frame *frame)
{
    unsigned long flag;
    vpss_in_node *date_tmp = HI_NULL;
    list *list_tmp = HI_NULL;
    hi_u32 buf_ref_cnt;

    vpss_check_null(in);
    vpss_check_null(frame);
    vpss_check_null((hi_drv_win_vpss_meta_info *)frame->video_private.win_private.data);
    vpss_check_null(frame->buf_addr[0].dma_handle);

    if (!in->init) {
        vpss_error("IN has been deinited\n");
        return HI_FAILURE;
    }

    vpss_comm_down_spin(&in->src_in_lock, &flag);
    buf_ref_cnt = file_count(((struct dma_buf *)frame->buf_addr[0].dma_handle)->file);
    if (buf_ref_cnt <= 0) {
        vpss_comm_up_spin(&in->src_in_lock, &flag);
        vpss_error("in queue faild! idex:%d dma_handle:0x%llx ref_cnt:%d \n",
                   frame->frame_index, frame->buf_addr[0].dma_handle, buf_ref_cnt);
        return HI_FAILURE;
    }

    if (osal_list_empty(&in->free_list) == HI_TRUE) {
        vpss_comm_up_spin(&in->src_in_lock, &flag);
        vpss_error("IN no node to fill\n");
        return HI_FAILURE;
    }

    if (vpss_in_check_have_repeat_frame(in, frame) == HI_TRUE) {
        vpss_comm_up_spin(&in->src_in_lock, &flag);
        vpss_error("repeat frame\n");
        return HI_FAILURE;
    }

    if (vpss_in_check_have_repeat_metadata(in, frame) == HI_TRUE) {
        vpss_comm_up_spin(&in->src_in_lock, &flag);
        vpss_error("repeat metadata \n");
        return HI_FAILURE;
    }

    list_tmp = in->free_list.next;
    osal_list_del(list_tmp);
    date_tmp = (vpss_in_node *)osal_list_entry(list_tmp, vpss_in_node, node);
    memcpy(&(date_tmp->in_data), frame, sizeof(hi_drv_video_frame));
    osal_list_add_tail(&date_tmp->node, &in->busy_list);
    in->queue_cnt++;
    in->queue_total_cnt++;
    vpss_comm_up_spin(&in->src_in_lock, &flag);

    return HI_SUCCESS;
}

hi_s32 vpss_in_dequeue_frame(vpss_in *in, hi_drv_video_frame *frame)
{
    unsigned long flag;
    vpss_in_node *date_tmp = HI_NULL;
    list *list_tmp = HI_NULL;

    vpss_check_null(in);
    vpss_check_null(frame);

    if (!in->init) {
        vpss_error("IN has been deinited\n");
        return HI_FAILURE;
    }

    vpss_comm_down_spin(&in->src_in_lock, &flag);

    if (osal_list_empty(&in->release_list) == HI_TRUE) {
        vpss_comm_up_spin(&in->src_in_lock, &flag);
        vpss_dbg("IN no node to dequeue\n");
        return HI_FAILURE;
    }

    list_tmp = in->release_list.next;
    osal_list_del(list_tmp);
    date_tmp = (vpss_in_node *)osal_list_entry(list_tmp, vpss_in_node, node);
    memcpy(frame, &(date_tmp->in_data), sizeof(hi_drv_video_frame));
    osal_list_add_tail(&date_tmp->node, &in->free_list);
    in->dequeue_cnt++;
    in->dequeue_total_cnt++;
    vpss_comm_up_spin(&in->src_in_lock, &flag);

    return HI_SUCCESS;
}

hi_s32 vpss_in_acquire_frame(vpss_in *in, hi_drv_video_frame *frame)
{
    unsigned long flag;
    vpss_in_node *date_tmp = HI_NULL;
    list *list_tmp = HI_NULL;

    vpss_check_null(in);
    vpss_check_null(frame);

    if (!in->init) {
        vpss_error("IN has been deinited\n");
        return HI_FAILURE;
    }

    vpss_comm_down_spin(&in->src_in_lock, &flag);
    if (osal_list_empty(&in->busy_list) == HI_TRUE) {
        vpss_comm_up_spin(&in->src_in_lock, &flag);
        vpss_dbg("IN no node to get\n");
        return HI_FAILURE;
    }

    list_tmp = in->busy_list.next;
    osal_list_del(list_tmp);
    date_tmp = (vpss_in_node *)osal_list_entry(list_tmp, vpss_in_node, node);
    in->target = date_tmp;
    memcpy(frame, &(date_tmp->in_data), sizeof(hi_drv_video_frame));
    in->acquire_cnt++;
    in->acquire_total_cnt++;
    osal_list_add_tail(&date_tmp->node, &in->wait_rel_list);
    vpss_comm_up_spin(&in->src_in_lock, &flag);

    return HI_SUCCESS;
}

hi_s32 vpss_in_release_frame(vpss_in *in, hi_drv_video_frame *frame)
{
    hi_bool find_frm = HI_FALSE;
    unsigned long flag = 0;
    vpss_in_node *date_tmp = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_drv_win_vpss_meta_info *meta_info = HI_NULL;
    hi_drv_win_vpss_meta_info *meta_tmp_info = HI_NULL;

    if (!in->init) {
        vpss_error("SRCIN has been deinited\n");
        return HI_FAILURE;
    }

    vpss_check_null(in->target);

    meta_info = (hi_drv_win_vpss_meta_info *)frame->video_private.win_private.data;
    if (meta_info->vpss_reset_frame == HI_TRUE) {
        return HI_SUCCESS;
    }

    vpss_comm_down_spin(&in->src_in_lock, &flag);
    osal_list_for_each_safe(pos, n, &(in->wait_rel_list)) {
        date_tmp = (vpss_in_node *)osal_list_entry(pos, vpss_in_node, node);
        if ((date_tmp->in_data.buf_addr[0].dma_handle == frame->buf_addr[0].dma_handle) &&
            (date_tmp->in_data.frame_index == frame->frame_index)) {
            osal_list_del(pos);
            find_frm = HI_TRUE;
            break;
        }
    }

    if (find_frm != HI_TRUE) {
        vpss_comm_up_spin(&in->src_in_lock, &flag);
        vpss_error("Can't find rel frame index=%d,addr:0x%llx\n", frame->frame_index, frame->buf_addr[0].dma_handle);
        return HI_FAILURE;
    }

    meta_tmp_info = (hi_drv_win_vpss_meta_info *)date_tmp->in_data.video_private.win_private.data;

    if (meta_info->freeze_frame == HI_TRUE) {
        osal_list_add_tail(&date_tmp->node, &in->free_list);
    } else {
#ifdef DPT
        if (meta_info->vpss_process == HI_TRUE) {
            frame->hardware_buffer.dst_id = HI_DRV_HARDWARE_BUFFER_ID_VPSS_IN;
            frame->hardware_buffer.backward_tunnel_enable = frame->hardware_buffer.forward_tunnel_enable;
        }

        date_tmp->in_data.hardware_buffer.dst_id = frame->hardware_buffer.dst_id;
        date_tmp->in_data.hardware_buffer.backward_tunnel_enable = frame->hardware_buffer.forward_tunnel_enable;
#endif
        osal_list_add_tail(&date_tmp->node, &in->release_list);
    }

    in->release_cnt++;
    in->release_total_cnt++;
    vpss_comm_up_spin(&in->src_in_lock, &flag);

    return HI_SUCCESS;
}

hi_s32 vpss_in_get_buf_num(vpss_in *in, hi_u32 *out_buf_num)
{
    hi_u32 buf_num = 0;
    unsigned long flag = 0;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    vpss_check_null(in);
    vpss_check_null(out_buf_num);
    vpss_comm_down_spin(&in->src_in_lock, &flag);
    osal_list_for_each_safe(pos, n, &(in->busy_list)) {
        buf_num++;
    }
    *out_buf_num = buf_num;
    vpss_comm_up_spin(&in->src_in_lock, &flag);
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


