/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_out.c source file vpss out
 * Author: zhangjunyu
 * Create: 2016/06/13
 */
#include "drv_vpss_out.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static hi_void vpss_out_init_default_cfg(vpss_out *frame_list)
{
    frame_list->get_total = 0;
    frame_list->get_success = 0;
    frame_list->get_success_total = 0;
    frame_list->rel_total = 0;
    frame_list->rel_success = 0;
    frame_list->rel_success_total = 0;
    frame_list->start = jiffies;
    frame_list->get_hz = 0;
    frame_list->get_last = 0;

    return;
}

hi_s32 vpss_out_init(vpss_out *frame_list, vpss_out_attr *attr)
{
    hi_u32 count;
    hi_drv_win_vpss_meta_info *meta_buff = HI_NULL;

    vpss_check_null(frame_list);
    vpss_check_null(attr);

    if (frame_list->init) {
        vpss_error("out has been inited\n");
        return HI_SUCCESS;
    }

    frame_list->attr = *attr;
    OSAL_INIT_LIST_HEAD(&frame_list->empty_frm_list);
    OSAL_INIT_LIST_HEAD(&frame_list->ful_frm_list);
    if (vpss_comm_init_spin(&frame_list->ful_buf_spin) != HI_SUCCESS) {
        vpss_error("Init ful_buf_spin failed.\n");
        return HI_FAILURE;
    }
    if (vpss_comm_init_spin(&frame_list->empty_buf_spin) != HI_SUCCESS) {
        vpss_comm_destory_spin(&frame_list->ful_buf_spin);
        vpss_error("Init empty_buf_spin failed.\n");
        return HI_FAILURE;
    }

    for (count = 0; count < attr->buf_num; count++) {
        memset(&(frame_list->out_node[count]), 0, sizeof(vpss_out_node));

        meta_buff = (hi_drv_win_vpss_meta_info *)vpss_kmalloc(sizeof(hi_drv_win_vpss_meta_info), OSAL_GFP_KERNEL);
        if (meta_buff == HI_NULL) {
            vpss_error("out node meta malloc failed! \n");
            goto __ERR_OUT_INIT_MALLOC;
        }

        memset(meta_buff, 0, sizeof(hi_drv_win_vpss_meta_info));
        frame_list->out_node[count].metedata.vir_addr = (hi_u8 *)meta_buff;

        osal_list_add_tail(&frame_list->out_node[count].node, &frame_list->empty_frm_list);
    }

    frame_list->out_target_1 = &(frame_list->ful_frm_list);
    frame_list->init = HI_TRUE;
    vpss_out_init_default_cfg(frame_list);

    return HI_SUCCESS;

__ERR_OUT_INIT_MALLOC:
    for (count = 0; count < attr->buf_num; count++) {
        if (frame_list->out_node[count].metedata.vir_addr != HI_NULL) {
            vpss_kfree(frame_list->out_node[count].metedata.vir_addr);
            frame_list->out_node[count].metedata.vir_addr = HI_NULL;
        }
    }

    vpss_comm_destory_spin(&frame_list->ful_buf_spin);
    vpss_comm_destory_spin(&frame_list->empty_buf_spin);

    frame_list->init = HI_FALSE;
    return HI_FAILURE;
}

hi_s32 vpss_out_deinit(vpss_out *frame_list)
{
    vpss_out_node *target = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_u32 del_count;
    drv_vpss_mem_info *mem_info = HI_NULL;

    vpss_check_null(frame_list);
    del_count = 0;
    /* spinlock can't use VFREE */
    osal_list_for_each_safe(pos, n, &(frame_list->empty_frm_list)) {
        target = osal_list_entry(pos, vpss_out_node, node);
        mem_info = &(target->buffer.buff_info);
        if ((mem_info->phy_addr != 0 || mem_info->vir_addr != HI_NULL) && (mem_info->size != 0)) {
            vpss_comm_mem_free(&(target->buffer.buff_info));
        }

        if (target->metedata.vir_addr != HI_NULL) {
            vpss_kfree(target->metedata.vir_addr);
            target->metedata.vir_addr = HI_NULL;
        }

        osal_list_del_init(pos);
        del_count++;
    }

    osal_list_for_each_safe(pos, n, &(frame_list->ful_frm_list)) {
        target = osal_list_entry(pos, vpss_out_node, node);
        mem_info = &(target->buffer.buff_info);
        if ((mem_info->phy_addr != 0 || mem_info->vir_addr != HI_NULL)
            && (mem_info->size != 0)) {
            vpss_comm_mem_free(&(target->buffer.buff_info));
        }

        if (target->metedata.vir_addr != HI_NULL) {
            vpss_kfree(target->metedata.vir_addr);
            target->metedata.vir_addr = HI_NULL;
        }

        osal_list_del_init(pos);
        del_count++;
    }

    if (del_count != frame_list->attr.buf_num) {
        vpss_error("vpss buffer destory error already delete %d total %d.\n",
                   del_count, frame_list->attr.buf_num);
    }

    if (frame_list->ful_buf_spin.lock != HI_NULL) {
        vpss_comm_destory_spin(&frame_list->ful_buf_spin);
    }

    if (frame_list->empty_buf_spin.lock != HI_NULL) {
        vpss_comm_destory_spin(&frame_list->empty_buf_spin);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_out_get_ful_frm(vpss_out *frame_list, hi_drv_video_frame *frame)
{
    vpss_out_node *frm_node = HI_NULL;
    hi_drv_video_frame *frm = HI_NULL;
    list *next_node = HI_NULL;
    unsigned long flags;
    hi_s32 ret;

    vpss_check_null(frame_list);
    vpss_check_null(frame);
    vpss_check_init(frame_list->init);
    frame_list->get_total++;

    if (jiffies - frame_list->start >= HZ) {
        frame_list->start = jiffies;
        frame_list->get_hz = frame_list->get_total - frame_list->get_last;
        frame_list->get_last = frame_list->get_total;
    }

    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    next_node = (frame_list->out_target_1)->next;

    if (next_node != &(frame_list->ful_frm_list)) {
        frm_node = osal_list_entry(next_node, vpss_out_node, node);
        frm = &(frm_node->out_frame);

        memcpy(frame, frm, sizeof(hi_drv_video_frame));

        frame_list->out_target_1 = next_node;
        frame_list->get_success++;
        frame_list->get_success_total++;
        ret = HI_SUCCESS;
    } else {
        ret = HI_FAILURE;
    }
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    return ret;
}

hi_s32 vpss_out_rel_ful_frm(vpss_out *frame_list, hi_drv_video_frame *frame)
{
    vpss_out_node *target = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_s32 get_frm = HI_FAILURE;
    hi_u32 count = 0;
    unsigned long flags;

    vpss_check_null(frame_list);
    vpss_check_null(frame);
    vpss_check_init(frame_list->init);
    frame_list->rel_total++;

    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    for (pos = (frame_list->ful_frm_list).next, n = pos->next;
         pos != &(frame_list->ful_frm_list) && pos != (frame_list->out_target_1)->next;
         pos = n, n = pos->next) {
        if (count > DEF_OUT_NODE_MAX_NUM) {
            vpss_error("rel_ful_frm_buf error,full list error count:%d \n", count);
        }

        count++;
        target = osal_list_entry(pos, vpss_out_node, node);
        if (target->out_frame.buf_addr[0].dma_handle == frame->buf_addr[0].dma_handle) {
            if (target->out_frame.frame_index != frame->frame_index) {
                vpss_error("\n rel error (full id:%d, addr:0x%llx) (rel id:%d, left_dma:0x%llx, right_dma:0x%llx) \n",
                           target->out_frame.frame_index, target->buffer.buff_info.phy_addr,
                           frame->frame_index, frame->buf_addr[0].dma_handle, frame->buf_addr[1].dma_handle);
                get_frm = HI_FAILURE;
                break;
            }

            if (frame_list->out_target_1 == pos) {
                frame_list->out_target_1 = (frame_list->out_target_1)->prev;
            }

            osal_list_del_init(pos);
            get_frm = HI_SUCCESS;
            break;
        }
    }
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    if (get_frm == HI_FAILURE) {
        vpss_error("can't get rel_frm id %d,addr %x \n", frame->frame_index, frame->buf_addr[0].dma_handle);
    } else {
        frame_list->rel_success++;
        frame_list->rel_success_total++;
        vpss_out_add_empty_frm_buf(frame_list, target);
    }

    return get_frm;
}

hi_s32 vpss_out_add_ful_frm_buf(vpss_out *frame_list, vpss_out_node *node)
{
    unsigned long flags;

    vpss_check_init(frame_list->init);

    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    osal_list_add_tail(&(node->node), &(frame_list->ful_frm_list));
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    return HI_SUCCESS;
}

hi_bool vpss_out_move_target(vpss_out *frame_list)
{
    unsigned long flags;

    vpss_check_init(frame_list->init);

    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    frame_list->out_target_1 = frame_list->out_target_1->next;
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    return HI_SUCCESS;
}

vpss_out_node *malloc_node_buffer(vpss_out *frame_list,
                                  vpss_out_node *target, vpss_out_frame_info *out_attr)
{
    hi_s32 ret;
    drv_vpss_mem_info *mem_info = HI_NULL;
    drv_vpss_mem_attr attr = { 0 };
    hi_bool loss_cmp = vpss_comm_is_loss_compress(out_attr->cmp_info.cmp_mode);
    vpss_buffer_attr buffer_attr = { 0 };
    hi_drv_ssm_buf_attach_info attach_info;
    hi_u64 sec_info_addr;

    memset(&(target->out_priv_frame), 0, sizeof(hi_drv_vpss_video_frame));
    if (out_attr->vpss_process == HI_TRUE) {
        mem_info = &(target->buffer.buff_info);
        buffer_attr.width = out_attr->width;
        buffer_attr.height = out_attr->height;
        vpss_comm_cal_buf_size(&buffer_attr, out_attr->pixel_format, out_attr->cmp_info,
                               out_attr->bit_width, loss_cmp, out_attr->cmp_info.ycmp_rate);

        if (out_attr->video_3d_type != HI_DRV_3D_NONE) {
            buffer_attr.buf_size = buffer_attr.buf_size * 2;  /* bufsize must multply by 2 when 3d */
        }

        if ((mem_info->size == 0) || (target->buffer.stride_y == 0) || (buffer_attr.buf_size != mem_info->size)) {
            if ((mem_info->phy_addr != 0) && (mem_info->size != 0)) {
                vpss_comm_mem_free(mem_info);
                mem_info->size = 0;
                target->buffer.stride_y = 0;
                target->buffer.stride_c = 0;
            }

            attr.name = "vpss_frm_buf";
            attr.size = buffer_attr.buf_size;
            attr.mode = out_attr->secure ? OSAL_SECSMMU_TYPE : OSAL_NSSMMU_TYPE; /* just smmu support */
            attr.is_map_viraddr = HI_FALSE;
            attr.is_cache = HI_FALSE;
            ret = vpss_comm_mem_alloc(&attr, mem_info);
            if (ret != HI_SUCCESS) {
                mem_info->size = 0;
                target->buffer.stride_y = 0;
                target->buffer.stride_c = 0;
                mem_info->phy_addr = 0;
                mem_info->vir_addr = HI_NULL;
                mem_info->dma_handle = HI_NULL;
                vpss_out_add_empty_frm_buf(frame_list, target);
                vpss_error("dynamic alloc buffer failed.buf_size %#x\n", buffer_attr.buf_size);
                return HI_NULL;
            }

            attach_info.session_handle = 0x0FFFFFFF; /* 0x0FFFFFFF: unvalid */
            attach_info.dma_buf_addr = mem_info->dma_handle;
            attach_info.buf_id = HI_SSM_BUFFER_ID_VPSS_OUTPUT_BUF;
            attach_info.module_handle = 0x5CFFFFFF; /* 0x5CFFFFFF: vpss handle, 0x5C valid */
            vpss_comm_attach_buf(out_attr->secure, attach_info, &sec_info_addr);
        }

        target->buffer.stride_y = buffer_attr.stride_y;
        target->buffer.stride_c = buffer_attr.stride_c;
    }

    return target;
}

vpss_out_node *vpss_out_get_empty_buf(vpss_out *frame_list, vpss_out_frame_info *attr)
{
    vpss_out_node *target = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    unsigned long flags = 0;

    target = HI_NULL;

    if (frame_list->init == HI_FALSE) {
        vpss_error("out has not been init\n");
        return target;
    }

    vpss_comm_down_spin(&(frame_list->empty_buf_spin), &flags);
    osal_list_for_each_safe(pos, n, &(frame_list->empty_frm_list)) {
        target = osal_list_entry(pos, vpss_out_node, node);
        osal_list_del_init(pos);
        break;
    }
    vpss_comm_up_spin(&(frame_list->empty_buf_spin), &flags);

    if (target == HI_NULL) {
        return target;
    }

    return malloc_node_buffer(frame_list, target, attr);
}

hi_s32 vpss_out_add_empty_frm_buf(vpss_out *frame_list, vpss_out_node *node)
{
    unsigned long flags;

    vpss_check_null(frame_list);
    vpss_check_init(frame_list->init);

    vpss_comm_down_spin(&(frame_list->empty_buf_spin), &flags);
    osal_list_add_tail(&(node->node), &(frame_list->empty_frm_list));
    vpss_comm_up_spin(&(frame_list->empty_buf_spin), &flags);

    return HI_SUCCESS;
}
hi_bool vpss_out_check_empty_buf(vpss_out *frame_list)
{
    list *empty_list;
    unsigned long flags;
    hi_bool empty = HI_FALSE;

    empty_list = &(frame_list->empty_frm_list);

    vpss_check_init(frame_list->init);

    vpss_comm_down_spin(&(frame_list->empty_buf_spin), &flags);
    if (empty_list->next != empty_list) {
        empty = HI_TRUE;
    }
    vpss_comm_up_spin(&(frame_list->empty_buf_spin), &flags);

    return empty;
}
hi_s32 vpss_out_reset(vpss_out *frame_list)
{
    vpss_out_node *target = HI_NULL;
    vpss_out_node *rel_target[DEF_OUT_NODE_MAX_NUM] = { HI_NULL };
    list *pos = HI_NULL;
    list *n = HI_NULL;
    hi_u32 count;
    hi_u32 rel_count;
    unsigned long flags;
    hi_s32 ret;

    vpss_check_init(frame_list->init);
    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    rel_count = 0;

    for (pos = (frame_list->out_target_1)->next, n = pos->next;
         pos != &(frame_list->ful_frm_list);
         pos = n, n = pos->next) {
        if (rel_count >= DEF_OUT_NODE_MAX_NUM) {
            vpss_error("reset error\n");
            break;
        }

        target = osal_list_entry(pos, vpss_out_node, node);
        osal_list_del_init(pos);
        rel_target[rel_count] = target;
        rel_count++;
    }
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    if (rel_count >= DEF_OUT_NODE_MAX_NUM) {
        vpss_error("reset error\n");
    }

    for (count = 0; count < rel_count; count++) {
        ret = vpss_out_add_empty_frm_buf(frame_list, rel_target[count]);
        if (ret != HI_SUCCESS) {
            vpss_error("add empty frame buf error\n");
        }
    }

    frame_list->get_total = 0;
    frame_list->get_success = 0;
    frame_list->rel_total = 0;
    frame_list->rel_success = 0;
    frame_list->get_hz = 0;
    frame_list->get_last = 0;

    return HI_SUCCESS;
}

hi_bool vpss_out_check_frame_index_repeat(vpss_out *frame_list, hi_u32 frame_index)
{
    hi_u32 count = 0;
    vpss_out_node *fb_node = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    unsigned long flags = 0;
    hi_bool frame_index_repeat = HI_FALSE;

    vpss_check_init(frame_list->init);

    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    osal_list_for_each_safe(pos, n, &(frame_list->ful_frm_list)) {
        fb_node = osal_list_entry(pos, vpss_out_node, node);

        if (count >= DEF_OUT_NODE_MAX_NUM) {
            vpss_error("ful_frm_list get fb_list error\n");
            break;
        }

        if (frame_index == fb_node->out_frame.frame_index) {
            frame_index_repeat = HI_TRUE;
            break;
        }

        fb_node = HI_NULL;
        count++;
    }
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    return frame_index_repeat;
}

hi_s32 vpss_out_get_state(vpss_out *frame_list, vpss_fb_state *state)
{
    hi_u32 count = 0;
    vpss_out_node *fb_node = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;
    unsigned long flags = 0;

    vpss_check_init(frame_list->init);
    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    if (frame_list->out_target_1 != &(frame_list->ful_frm_list)) {
        fb_node = osal_list_entry(frame_list->out_target_1, vpss_out_node, node);
        state->out_rate = fb_node->out_frame.frame_rate;
    }

    osal_list_for_each_safe(pos, n, &(frame_list->ful_frm_list)) {
        if (count >= DEF_OUT_NODE_MAX_NUM) {
            vpss_error("ful_frm_list get fb_list error\n");
            break;
        }

        if (frame_list->out_target_1 == pos) {
            state->wait_sink_rls_numb = count + 1;
        }

        count++;
    }
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    state->ful_list_numb = count;
    count = 0;

    vpss_comm_down_spin(&(frame_list->empty_buf_spin), &flags);
    osal_list_for_each_safe(pos, n, &(frame_list->empty_frm_list)) {
        if (count >= DEF_OUT_NODE_MAX_NUM) {
            vpss_error("empty_frm_list get fb_list error\n");
            break;
        }

        count++;
    }
    vpss_comm_up_spin(&(frame_list->empty_buf_spin), &flags);

    state->empty_list_numb = count;
    state->total_numb = frame_list->attr.buf_num;
    state->get_hz = frame_list->get_hz;
    state->get_total = frame_list->get_total;
    state->get_success = frame_list->get_success;
    state->get_success_total = frame_list->get_success_total;
    state->rel_total = frame_list->rel_total;
    state->rel_success = frame_list->rel_success;
    state->rel_success_total = frame_list->rel_success_total;

    return HI_SUCCESS;
}

hi_s32 vpss_out_show_status(vpss_out *frame_list)
{
    hi_bool find_target = HI_FALSE;
    unsigned long flags = 0;
    vpss_out_node *target = HI_NULL;
    hi_drv_video_frame *frame = HI_NULL;
    list *pos = HI_NULL;
    list *n = HI_NULL;

    vpss_check_init(frame_list->init);
    vpss_comm_down_spin(&(frame_list->ful_buf_spin), &flags);
    osal_printk("\n-----------vpss port buffer status----------\n");
    osal_list_for_each_safe(pos, n, &(frame_list->ful_frm_list)) {
        target = osal_list_entry(pos, vpss_out_node, node);
        frame = &target->out_frame;

        if (frame_list->out_target_1->next == pos) {
            find_target = HI_TRUE;
        }

        if (find_target) {
            osal_printk("waiting buffer:\n");
        } else {
            osal_printk("releasing buffer:\n");
        }

        osal_printk("index=%d, mmz=0x%llx \n"
                    "(L:dma:%llx) (yoff:0x%x) (coff:0x%x) (ystr:0x%x) (cstr:0x%x) \n"
                    "(R:dma:%llx) (yoff:0x%x) (coff:0x%x) (ystr:0x%x) (cstr:0x%x) \n",
                    frame->frame_index, target->buffer.buff_info.phy_addr,
                    frame->buf_addr[HI_DRV_3D_EYE_LEFT].dma_handle,
                    frame->buf_addr[HI_DRV_3D_EYE_LEFT].y_offset,
                    frame->buf_addr[HI_DRV_3D_EYE_LEFT].c_offset,
                    frame->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y,
                    frame->buf_addr[HI_DRV_3D_EYE_LEFT].stride_c,
                    frame->buf_addr[HI_DRV_3D_EYE_RIGHT].dma_handle,
                    frame->buf_addr[HI_DRV_3D_EYE_RIGHT].y_offset,
                    frame->buf_addr[HI_DRV_3D_EYE_RIGHT].c_offset,
                    frame->buf_addr[HI_DRV_3D_EYE_RIGHT].stride_y,
                    frame->buf_addr[HI_DRV_3D_EYE_RIGHT].stride_c);
    }
    vpss_comm_up_spin(&(frame_list->ful_buf_spin), &flags);

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

