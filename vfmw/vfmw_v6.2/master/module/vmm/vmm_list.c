/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vmm_list.h"

typedef struct {
    struct OS_LIST_HEAD list;
    hi_void *vir_addr;
} list_buf_rec;

STATIC hi_s32 check_list_cfg(const vmm_list_config *list_cfg)
{
    hi_u32 err_flag = 0;

    D_VMM_CHECK_PTR_RET(list_cfg, ERR_LIST_NULL_PTR);

    err_flag |= (list_cfg->max_expansion_factor == 0);
    err_flag |= (list_cfg->node_num == 0);
    err_flag |= (list_cfg->per_node_size == 0);
    err_flag |= (list_cfg->mem_alloc_mode >= ALLOC_MODE_MAX);
    err_flag |= (list_cfg->lock_mode >= LOCK_MODE_MAX);

    return (err_flag == 0) ? LIST_SUCCESS : LIST_FAILURE;
}

STATIC hi_void *list_alloc_vir_mem(hi_u8 *name, vmm_alloc_mode alloc_mode, hi_u32 size)
{
    hi_void *vir_mem = HI_NULL;

    switch (alloc_mode) {
        case ALLOC_MODE_VMALLOC:
            vir_mem = (hi_void *)OS_ALLOC_VIR(name, size);
            break;

        case ALLOC_MODE_KMALLOC_ATOMIC:
            vir_mem = (hi_void *)OS_ALLOC_VIR(name, size);
            break;

        default:
            PRINT(PRN_ERROR, "%s,%d, not support this alloc mode(%d),alloc size = %d\n",
                  __func__, __LINE__, alloc_mode, size);
            break;
    }

    return vir_mem;
}

STATIC hi_void list_free_vir_mem(vmm_alloc_mode alloc_mode, hi_void *vir_mem)
{
    switch (alloc_mode) {
        case ALLOC_MODE_VMALLOC:
            OS_FREE_VIR(vir_mem);
            break;

        case ALLOC_MODE_KMALLOC_ATOMIC:
            OS_FREE_VIR(vir_mem);
            break;

        default:
            PRINT(PRN_ERROR, "%s,%d, not support this free mode(%d),free vir_mem = %p\n",
                  __func__, __LINE__, alloc_mode, vir_mem);
            break;
    }

    return;
}

STATIC hi_void *list_take_out_data_rec(vmm_list *vmm_list)
{
    list_buf_rec *buf_rec = HI_NULL;
    hi_void *vir_addr = HI_NULL;

    D_VMM_CHECK_PTR_RET(vmm_list, HI_NULL);

    LIST_LOCK(vmm_list);

    if (!list_is_empty(vmm_list, LIST_TYPE_INTERNAL)) {
        buf_rec = OS_LIST_FIRST_ENTRY(&vmm_list->data, list_buf_rec, list);
        vir_addr = buf_rec->vir_addr;
        OS_LIST_DEL(&buf_rec->list);
    }

    LIST_UNLOCK(vmm_list);

    if (buf_rec != HI_NULL) {
        list_free_vir_mem(vmm_list->cfg.mem_alloc_mode, buf_rec);
    }

    return vir_addr;
}

hi_s32 list_expand_len(vmm_list *list)
{
    vmm_list_config *list_cfg = HI_NULL;
    list_buf_rec *buf_rec = HI_NULL;
    hi_void *node = HI_NULL;
    struct OS_LIST_HEAD *new = HI_NULL;
    hi_u32 node_num = 0;

    D_VMM_CHECK_PTR_RET(list, ERR_LIST_NULL_PTR);
    D_VMM_CHECK_PTR_RET(list, ERR_LIST_NULL_PTR);

    list_cfg = &list->cfg;

    if (check_list_cfg(list_cfg) != LIST_SUCCESS) {
        PRINT(PRN_ERROR, "%s,%d,check_list_cfg failed!\n", __func__, __LINE__);
        return ERR_LIST_INVALID_PARA;
    }

    if (list->cur_expansion_factor >= list_cfg->max_expansion_factor) {
        PRINT(PRN_ERROR, "List(No.%d) cur_expansion_factor(%d) is >= max_expansion_factor(%d). Can not be expanded!\n",
              list_cfg->mark_no, list->cur_expansion_factor, list_cfg->max_expansion_factor);
        return ERR_LIST_FULL;
    }

    PRINT(PRN_DBG, "%s,%d,===> List(No.%d) cur_expansion_factor(%d), max_expansion_factor(%d), cur_node_num = %d\n",
          __func__, __LINE__, list_cfg->mark_no, list->cur_expansion_factor, list_cfg->max_expansion_factor,
          list->cur_node_num);

    /* alloc initial node buffer */
    node = list_alloc_vir_mem("VMM_Node", list_cfg->mem_alloc_mode,
                              (list_cfg->per_node_size * list_cfg->node_num));
    if (!node) {
        PRINT(PRN_ERROR, "%s,%d,List(No.%d) alloc vir mem (size = %d)failed!\n",
              __func__, __LINE__, list_cfg->mark_no, (list_cfg->per_node_size * list_cfg->node_num));
        return ERR_LIST_ALLOC_FAILED;
    }

    /* alloc data node buffer */
    buf_rec = (list_buf_rec *)list_alloc_vir_mem("VMM_Rec", list_cfg->mem_alloc_mode, sizeof(list_buf_rec));
    if (!buf_rec) {
        PRINT(PRN_ERROR, "%s,%d,List(No.%d) alloc vir mem (size = %d)failed!\n",
              __func__, __LINE__, list_cfg->mark_no, sizeof(list_buf_rec));
        list_free_vir_mem(list_cfg->mem_alloc_mode, node);
        return ERR_LIST_ALLOC_FAILED;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(node, (list_cfg->per_node_size * list_cfg->node_num),
                                 0, (list_cfg->per_node_size * list_cfg->node_num)));

    buf_rec->vir_addr = node;

    LIST_LOCK(list);

    OS_LIST_ADD(&buf_rec->list, &list->data);

    list->cur_expansion_factor++;

    list->cur_node_num += list_cfg->node_num;

    for (node_num = list_cfg->node_num; node_num > 0; node_num--) {
        new = (struct OS_LIST_HEAD *)node;

        OS_LIST_ADD(new, &list->free);

        node = (hi_s8 *)node + list_cfg->per_node_size;
    }

    LIST_UNLOCK(list);

    return LIST_SUCCESS;
}

hi_s32 list_is_empty(vmm_list *list, vmm_list_type type)
{
    struct OS_LIST_HEAD *target_list = HI_NULL;

    D_VMM_CHECK_PTR_RET(list, LIST_TRUE);

    switch (type) {
        case LIST_TYPE_HEAD:
            target_list = &list->head;
            break;

        case LIST_TYPE_FREE:
            target_list = &list->free;
            break;

        case LIST_TYPE_INTERNAL:
            target_list = &list->data;
            break;

        default:
            PRINT(PRN_ERROR, "type '%d' is not support.\n", type);
            break;
    }

    if (target_list == HI_NULL) {
        return LIST_TRUE;
    }

    return OS_LIST_EMPTY(target_list);
}

vmm_list *vmm_init(const vmm_list_config *list_cfg)
{
    vmm_list *list = HI_NULL;

    if (check_list_cfg(list_cfg) != LIST_SUCCESS) {
        PRINT(PRN_ERROR, "%s,%d,check_list_cfg failed!\n", __func__, __LINE__);
        return HI_NULL;
    }

    list = (vmm_list *)list_alloc_vir_mem((list_cfg->type == LIST_MEM) ? "VMM_MemLst" : "VMM_MsgLst",
                                          list_cfg->mem_alloc_mode, sizeof(vmm_list));
    if (!list) {
        PRINT(PRN_ERROR, "%s,%d,alloc vir mem (size = %d)failed!\n", __func__, __LINE__, sizeof(vmm_list));
        return HI_NULL;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(list, sizeof(vmm_list), 0, sizeof(vmm_list)));
    VFMW_CHECK_SEC_FUNC(memcpy_s(&list->cfg, sizeof(vmm_list_config), list_cfg, sizeof(vmm_list_config)));

    OS_SPIN_LOCK_INIT(&list->lock);
    OS_SEMA_INIT(&list->mutex);

    OS_LIST_INIT_HEAD(&list->head);
    OS_LIST_INIT_HEAD(&list->free);
    OS_LIST_INIT_HEAD(&list->data);

    if (list_expand_len(list) != LIST_SUCCESS) {
        list_free_vir_mem(list_cfg->mem_alloc_mode, list);
        return HI_NULL;
    }

    return list;
}

hi_void list_deinit(vmm_list *list)
{
    hi_u32 time_cnt = 0;
    hi_void *vir_addr = HI_NULL;
    vmm_alloc_mode mem_alloc_mode;

    D_VMM_CHECK_PTR(list);

    mem_alloc_mode = list->cfg.mem_alloc_mode;

    do {
        vir_addr = list_take_out_data_rec(list);
        if (vir_addr != HI_NULL) {
            list_free_vir_mem(mem_alloc_mode, vir_addr);
        }
    } while ((vir_addr != HI_NULL) && (time_cnt > list->cfg.max_expansion_factor));

    list_free_vir_mem(mem_alloc_mode, list);

    return;
}


