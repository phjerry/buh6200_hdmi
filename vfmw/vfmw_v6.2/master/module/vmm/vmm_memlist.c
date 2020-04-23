/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vmm_memlist.h"
#include "vmm_list.h"
#include "vfmw_osal.h"

#define INIT_MEM_NODE_NUM    (128)
#define MAX_MEM_DATA_BUF_NUM (8)

VMM_PRIVATE vmm_list *g_mem_list_ctx = HI_NULL;

VMM_PRIVATE hi_void mem_list_add_node_and_sort(mem_node_info *mem_node, struct OS_LIST_HEAD *list_head)
{
    mem_node_info *mem_node_check = HI_NULL;
    struct OS_LIST_HEAD *curr = &mem_node->list;
    struct OS_LIST_HEAD *check = HI_NULL;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    /* ordering rule: head->tail   size large->small */
    if (vmm_mem_list_is_empty()) {
        OS_LIST_ADD(curr, list_head);
    } else {
        OS_LIST_FOR_EACH_SAFE(pos, n, list_head) {
            mem_node_check = OS_LIST_ENTRY(pos, mem_node_info, list);
            check = &mem_node_check->list;

            if (mem_node->data.buf_info.size >= mem_node_check->data.buf_info.size) {
                /* current Node size larger than CheckNode, add current before CheckNode */
                struct OS_LIST_HEAD *last_list = check->prev;

                curr->next = check;
                curr->prev = last_list;

                check->prev = curr;
                last_list->next = curr;

                break;
            } else if (check->next == list_head) {
                /* already reach the list last Node, current Node size is smallest */
                OS_LIST_ADD_TAIL(curr, list_head);
            }
        }
    }

    return;
}

VMM_PRIVATE hi_s32 mem_list_move_from_free_to_head(mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;

    LIST_LOCK(g_mem_list_ctx);

    mem_node = OS_LIST_FIRST_ENTRY(&g_mem_list_ctx->free, mem_node_info, list);
    if (mem_node != HI_NULL) {
        OS_LIST_DEL(&mem_node->list);

        VFMW_CHECK_SEC_FUNC(memcpy_s(&mem_node->data, sizeof(mem_data_info), data, sizeof(mem_data_info)));

        mem_list_add_node_and_sort(mem_node, &g_mem_list_ctx->head);

        ret = LIST_SUCCESS;
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

VMM_PRIVATE hi_s32 mem_list_move_from_head_to_free(mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;

    LIST_LOCK(g_mem_list_ctx);

    mem_node = OS_LIST_FIRST_ENTRY(&g_mem_list_ctx->head, mem_node_info, list);
    if (mem_node != HI_NULL) {
        OS_LIST_DEL(&mem_node->list);

        VFMW_CHECK_SEC_FUNC(memcpy_s(data, sizeof(mem_data_info), &mem_node->data, sizeof(mem_data_info)));

        OS_LIST_ADD_TAIL(&mem_node->list, &g_mem_list_ctx->free);

        ret = LIST_SUCCESS;
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

#if 0
VMM_PRIVATE hi_s32 mem_list_move_from_head_to_free(mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;

    LIST_LOCK(g_mem_list_ctx);

    mem_node = list_first_entry(&g_mem_list_ctx->head, mem_node_info, list);
    if (mem_node != HI_NULL) {
        OSAL_FP_list_del(&mem_node->list);

        OSAL_FP_memcpy(data, &mem_node->data, sizeof(mem_data_info));

        OSAL_FP_list_add_tail(&mem_node->list, &mem_list_ctxmem_list_ctx->head);

        ret = LIST_SUCCESS;
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

/* the node which be taken out should be moved from head list to free list */
hi_s32  vmm_mem_list_takeout_node(mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        return ERR_LIST_EMPTY;
    }

    ret = mem_list_move_from_head_to_free(data);

    return ret;
}

#endif

VMM_PRIVATE hi_s32 check_location(vmm_location location)
{
    return (location >= VMM_LOCATION_MAX) ? LIST_FAILURE : LIST_SUCCESS;
}

VMM_PRIVATE hi_s32 check_status(mem_node_status status)
{
    return (status >= NODE_STATUS_MAX) ? LIST_FAILURE : LIST_SUCCESS;
}

VMM_PRIVATE hi_void print_error_detail_mem_data(const mem_data_info *data)
{
    hi_s32 err_flag = 0;

    err_flag |= (data->status >= NODE_STATUS_MAX);
    err_flag |= (data->location >= VMM_LOCATION_MAX);
    err_flag |= (data->buf_info.map > 1);
    err_flag |= (data->buf_info.cache > 1);
    err_flag |= (data->buf_info.sec_flag > 1);
    err_flag |= ((data->buf_info.start_phy_addr == 0) || (data->buf_info.start_phy_addr == 0xffffffff));
    err_flag |= (data->buf_info.size == 0);
    err_flag |= (data->buf_info.mem_type >= VMM_MEM_TYPE_MAX);

    if (err_flag) {
        PRINT(PRN_ERROR, "%s,%d, Check ErrConfig:status(%d)>=%d, location(%d)>=%d,Map(%d)>%d,"
              " Cache(%d)>%d, sec_flag(%d)>%d, start_phy_addr = 0x%x, size = %d, mem_type(%d)>=%d\n",
              __func__, __LINE__,
              data->status, NODE_STATUS_MAX,
              data->location, VMM_LOCATION_MAX,
              data->buf_info.map, 1,
              data->buf_info.cache, 1,
              data->buf_info.sec_flag, 1,
              data->buf_info.start_phy_addr, data->buf_info.size,
              data->buf_info.mem_type, VMM_MEM_TYPE_MAX);
    }

    if (((data->location == VMM_LOCATION_INSIDE) && (data->ref_cnt != 0)) ||
        ((data->location == VMM_LOCATION_OUTSIDE) && (data->ref_cnt == 0))) {
        PRINT(PRN_ERROR, "%s,%d, Error:location = %d, RefCnt(%d) invalid!\n",
              __func__, __LINE__, data->location, data->ref_cnt);
    }

    if (data->buf_info.map == 1) {
        if ((data->buf_info.start_vir_addr == 0) || (data->buf_info.start_vir_addr == (-1))) {
            PRINT(PRN_ERROR, "%s,%d, Error:Map = %d, start_vir_addr(%llu) invalid!\n",
                  __func__, __LINE__, data->buf_info.map, data->buf_info.start_vir_addr);
        }
    } else {
        if (data->buf_info.start_vir_addr != 0) {
            PRINT(PRN_ERROR, "%s,%d, Error:Map = %d, start_vir_addr(%llu) invalid!\n",
                  __func__, __LINE__, data->buf_info.map, data->buf_info.start_vir_addr);
        }
    }

    if ((data->buf_info.cache == 1) && (data->buf_info.map != 1)) {
        PRINT(PRN_ERROR, "%s,%d, Error:Cache = %d, Map(%llu) invalid!\n",
              __func__, __LINE__, data->buf_info.cache, data->buf_info.map);
    }
}

VMM_PRIVATE hi_s32 check_mem_data(const mem_data_info *data)
{
    hi_s32 err_flag = 0;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;
    mem_node_info *mem_node = HI_NULL;
    UADDR target_phy_addr = data->buf_info.start_phy_addr;

    err_flag |= (data->status >= NODE_STATUS_MAX);
    err_flag |= (data->location >= VMM_LOCATION_MAX);
    err_flag |= (data->buf_info.map > 1);
    err_flag |= (data->buf_info.cache > 1);
    err_flag |= (data->buf_info.sec_flag > 1);
    err_flag |= ((data->buf_info.start_phy_addr == 0) || (data->buf_info.start_phy_addr == 0xffffffff));
    err_flag |= (data->buf_info.size == 0);
    err_flag |= (data->buf_info.mem_type >= VMM_MEM_TYPE_MAX);

    if (data->location == VMM_LOCATION_INSIDE) {
        err_flag |= (data->ref_cnt != 0);
    }

    if (data->location == VMM_LOCATION_OUTSIDE) {
        err_flag |= (data->ref_cnt == 0);

        if (data->ref_cnt != 1) {
            PRINT(PRN_IMP, "%s,%d, location = %d, RefCnt = %d becareful!\n",
                  __func__, __LINE__, data->location, data->ref_cnt);
        }
    }

    if (data->buf_info.map == 1) {
        err_flag |= ((data->buf_info.start_vir_addr == 0) || (data->buf_info.start_vir_addr == (-1)));
    } else {
        err_flag |= (data->buf_info.start_vir_addr != 0);
    }

    if (data->buf_info.cache == 1) {
        err_flag |= (data->buf_info.map != 1);
    }

    if (err_flag) {
        print_error_detail_mem_data(data);

        return ERR_LIST_INVALID_PARA;
    }

    LIST_LOCK(g_mem_list_ctx);

    if (vmm_mem_list_is_empty() != LIST_TRUE) {
        OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head)
        {
            mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
            if (target_phy_addr == mem_node->data.buf_info.start_phy_addr) {
                err_flag |= 1;
                PRINT(PRN_ERROR, "%s,%d, Curent Phyaddr 0x%x is already exist!\n",
                      __func__, __LINE__, target_phy_addr);
                break;
            }
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return (err_flag == 0) ? LIST_SUCCESS : ERR_LIST_NODE_ALREADY_EXIST;
}

VMM_PRIVATE hi_void get_default_mem_list_cfg(vmm_list_config *list_cfg)
{
    VFMW_CHECK_SEC_FUNC(memset_s(list_cfg, sizeof(vmm_list_config), 0, sizeof(vmm_list_config)));

    list_cfg->max_expansion_factor = MAX_MEM_DATA_BUF_NUM;
    list_cfg->node_num = INIT_MEM_NODE_NUM;
    list_cfg->per_node_size = sizeof(mem_node_info);
    list_cfg->mark_no = 1;
    list_cfg->mem_alloc_mode = ALLOC_MODE_KMALLOC_ATOMIC;
    list_cfg->lock_mode = LOCK_MODE_SPINLOCK;

    return;
}

VMM_PRIVATE hi_s32 is_mem_node_can_be_rls(const mem_data_info *data)
{
    return (data->status != NODE_STATUS_INUSE) && (data->location != VMM_LOCATION_OUTSIDE);
}

hi_s32 vmm_mem_list_add_node(mem_data_info *data)
{
    hi_s32 ret;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    ret = check_mem_data(data);
    if (ret != LIST_SUCCESS) {
        return ret;
    }

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_FREE)) {
        ret = list_expand_len(g_mem_list_ctx);
        if (ret != LIST_SUCCESS) {
            return ERR_LIST_FULL;
        }
    }

    ret = mem_list_move_from_free_to_head(data);

    return ret;
}

hi_s32 vmm_mem_list_set_specified_node_no_use(vmm_rls_burst *rls_burst)
{
    mem_node_info *mem_node = HI_NULL;
    hi_u64 target_priv_id;
    hi_u64 cur_priv_id = 0;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(rls_burst, ERR_LIST_NULL_PTR);

    target_priv_id = (hi_u64)rls_burst->priv_id;
    target_priv_id &= rls_burst->priv_mask;

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return LIST_SUCCESS;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);

        cur_priv_id = (hi_u64)mem_node->data.buf_info.priv_id;
        cur_priv_id &= rls_burst->priv_mask;

        if (cur_priv_id == target_priv_id) {
            mem_node->data.status = NODE_STATUS_NOUSE;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return LIST_SUCCESS;
}

hi_s32 vmm_mem_list_is_empty(hi_void)
{
    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);

    return list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD);
}

hi_s32 vmm_mem_list_find_node_by_phyaddr(UADDR target_phy_addr, mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;
    UADDR phy_addr;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    if (target_phy_addr == 0 || target_phy_addr == 0xffffffff) {
        return ERR_LIST_INVALID_PARA;
    }

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        phy_addr = mem_node->data.buf_info.start_phy_addr;

        if (phy_addr == target_phy_addr) {
            VFMW_CHECK_SEC_FUNC(memcpy_s(data, sizeof(mem_data_info), &mem_node->data, sizeof(mem_data_info)));
            ret = LIST_SUCCESS;

            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_update_buf_rec(vmm_buffer *buf_rec)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;
    vmm_buffer *buf_node = HI_NULL;
    UADDR target_phy_addr;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(buf_rec, ERR_LIST_NULL_PTR);

    target_phy_addr = buf_rec->start_phy_addr;

    if (target_phy_addr == 0 || target_phy_addr == 0xffffffff) {
        return ERR_LIST_INVALID_PARA;
    }

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        buf_node = &mem_node->data.buf_info;

        if (buf_node->start_phy_addr == target_phy_addr) {
            buf_node->cache = buf_rec->cache;
            buf_node->map = buf_rec->map;
            buf_node->start_vir_addr = buf_rec->start_vir_addr;

            ret = LIST_SUCCESS;

            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_takeout_node(mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        return ERR_LIST_EMPTY;
    }

    ret = mem_list_move_from_head_to_free(data);

    return ret;
}

hi_s32 vmm_mem_list_delete_node_by_phyaddr(UADDR target_phy_addr)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;
    UADDR phy_addr;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);

    if (target_phy_addr == 0 || target_phy_addr == 0xffffffff) {
        return ERR_LIST_INVALID_PARA;
    }

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        phy_addr = mem_node->data.buf_info.start_phy_addr;

        if (phy_addr == target_phy_addr) {
            OS_LIST_DEL(&mem_node->list);
            OS_LIST_ADD_TAIL(&mem_node->list, &g_mem_list_ctx->free);
            ret = LIST_SUCCESS;

            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_mark_location(UADDR target_phy_addr, vmm_location location)
{
    hi_s32 ret = ERR_LIST_NODE_NOT_EXIST;
    mem_node_info *mem_node = HI_NULL;
    UADDR phy_addr;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);

    if (target_phy_addr == 0 || target_phy_addr == 0xffffffff) {
        PRINT(PRN_ERROR, "%s,%d, Phyaddr = 0x%x invalid!\n", __func__, __LINE__, target_phy_addr);
        return ERR_LIST_INVALID_PARA;
    }

    if (check_location(location) != LIST_SUCCESS) {
        PRINT(PRN_ERROR, "%s,%d, location = %d limit exceeded!\n", __func__, __LINE__, location);
        return ERR_LIST_INVALID_PARA;
    }

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        phy_addr = mem_node->data.buf_info.start_phy_addr;

        if (phy_addr == target_phy_addr) {
            if (location == VMM_LOCATION_OUTSIDE) {
                mem_node->data.ref_cnt++;
                mem_node->data.location = VMM_LOCATION_OUTSIDE;
            } else {
                if (mem_node->data.ref_cnt == 0) {
                    PRINT(PRN_IMP, "%s,%d, MemNode Phyaddr = 0x%x, already inside!", __func__, __LINE__, phy_addr);
                }

                if (mem_node->data.ref_cnt > 0) {
                    mem_node->data.ref_cnt--;
                }

                if (mem_node->data.ref_cnt == 0) {
                    mem_node->data.location = VMM_LOCATION_INSIDE;
                }
            }

            ret = LIST_SUCCESS;

            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_mark_status(UADDR target_phy_addr, mem_node_status status)
{
    hi_s32 ret = ERR_LIST_NODE_NOT_EXIST;
    mem_node_info *mem_node = HI_NULL;
    UADDR phy_addr;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);

    if (target_phy_addr == 0 || target_phy_addr == 0xffffffff) {
        PRINT(PRN_ERROR, "%s,%d, Phyaddr = 0x%x invalid!\n", __func__, __LINE__, target_phy_addr);
        return ERR_LIST_INVALID_PARA;
    }

    if (check_status(status) != LIST_SUCCESS) {
        PRINT(PRN_ERROR, "%s,%d, Status = 0x%x limit exceeded!\n", __func__, __LINE__, status);
        return ERR_LIST_INVALID_PARA;
    }

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        phy_addr = mem_node->data.buf_info.start_phy_addr;

        if (phy_addr == target_phy_addr) {
            VMM_ASSERT(mem_node->data.status != status);

            mem_node->data.status = status;
            ret = LIST_SUCCESS;

            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_print_node_status(hi_void)
{
    mem_node_info *mem_node = HI_NULL;
    mem_data_info *data = HI_NULL;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;
    /* 8 :a number */
    hi_s8 status_tab[NODE_STATUS_MAX + 1][8] = { { "Idle" }, { "InUse" }, { "NoUse" }, { "Unknown" } };
    /* 8 :a number */
    hi_s8 location_tab[VMM_LOCATION_MAX + 1][8] = { { "Inside" }, { "Outside" }, { "Unknown" } };
    /* 10 :a number */
    hi_s8 mem_type_tab[VMM_MEM_TYPE_MAX + 1][10] = { { "SysAdapt" }, { "ProceMMZ" },
                                                     { "PerAlloc" }, { "ExtConfig" }, { "Unknown" } };

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return LIST_SUCCESS;
    }

    PRINT(PRN_ALWS,
          "-------------------------------------- MemNode Infon ---------------------------------------- \n");
    PRINT(PRN_ALWS,
        "\tpriv_id\t \tName\t \tPhyaddr\t \tViraddr\t size(KB) Cache/Map/Sec Type Status location(RefCnt) Time(us)\n");
    PRINT(PRN_ALWS,
          "--------------------------------------------------------------------------------------------- \n");
    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        data = &mem_node->data;
        PRINT(PRN_ALWS, "0x%-16llx %-16s   0x%-8x   0x%-16lx %-6d    %d  %d  %d    %-10s %-8s %-8s(%-2d) %-8d\n",
              data->buf_info.priv_id, data->buf_info.buf_name,
              /* 1024 :a number */
              data->buf_info.start_phy_addr, data->buf_info.start_vir_addr, data->buf_info.size / 1024,
              data->buf_info.cache, data->buf_info.map, data->buf_info.sec_flag,
              mem_type_tab[(data->buf_info.mem_type > VMM_MEM_TYPE_MAX) ? VMM_MEM_TYPE_MAX : data->buf_info.mem_type],
              status_tab[(data->status > NODE_STATUS_MAX) ? NODE_STATUS_MAX : data->status],
              location_tab[(data->location > VMM_LOCATION_MAX) ? VMM_LOCATION_MAX : data->location],
              data->ref_cnt, data->dbg_info.alloc_time_consume_in_us);
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return LIST_SUCCESS;
}

hi_s32 vmm_mem_list_read_proc(hi_void *page)
{
    mem_node_info *mem_node = HI_NULL;
    mem_data_info *data = HI_NULL;
    hi_s32 UsedSize = 0;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;
    /* 8 :a number */
    hi_s8 status_tab[NODE_STATUS_MAX + 1][8] = { { "Idle" }, { "InUse" }, { "NoUse" }, { "Unknown" } };
    hi_s8 location_tab[VMM_LOCATION_MAX + 1][8] = { { "Inside" }, { "Outside" }, { "Unknown" } }; /* 8 :a number */
    /* 10 :a number */
    hi_s8 mem_type_tab[VMM_MEM_TYPE_MAX + 1][10] = { { "SysAdapt" }, { "ProceMMZ" },
                                                     { "PerAlloc" }, { "ExtConfig" }, { "Unknown" } };

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);

    /* statistics info */
    OS_DUMP_PROC(page, 0, &UsedSize, 0,
                 "============================== MemList Status ============================= \n");
    OS_DUMP_PROC(page, 0, &UsedSize, 0, "Curnode_num                : %d\n", g_mem_list_ctx->cur_node_num);
    OS_DUMP_PROC(page, 0, &UsedSize, 0, "per_node_size               : %d (B)\n", g_mem_list_ctx->cfg.per_node_size);
    OS_DUMP_PROC(page, 0, &UsedSize, 0, "ExpansionFactor(Cur/Max)  : %d/%d\n", g_mem_list_ctx->cur_expansion_factor,
                 g_mem_list_ctx->cfg.max_expansion_factor);

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return LIST_SUCCESS;
    }

    OS_DUMP_PROC(page, 0, &UsedSize, 0,
                 "----------------------------- MemNode Info ------------------------------ \n");
    OS_DUMP_PROC(page, 0, &UsedSize, 0, "\tpriv_id\t   \tName\t \tPhyaddr\t \tViraddr\t     size(KB)"
                 " Cache/Map/Sec Type      Status  location(RefCnt)  Time(us)\n");
    OS_DUMP_PROC(page, 0, &UsedSize, 0,
                 "------------------------------------------------------------------------- \n");
    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        data = &mem_node->data;
        OS_DUMP_PROC(page, 0, &UsedSize, 0, "0x%-16llx  %-16s   0x%-8x   0x%-16lx   %-6d   %d "
                     "  %d   %d   %-10s   %-8s   %-8s(%-2d) %-8d\n",
                     data->buf_info.priv_id, data->buf_info.buf_name,
                     data->buf_info.start_phy_addr, data->buf_info.start_vir_addr,
                     /* 1024 :a number */
                     data->buf_info.size / 1024, data->buf_info.cache, data->buf_info.map, data->buf_info.sec_flag,
                     mem_type_tab[(data->buf_info.mem_type >
                        VMM_MEM_TYPE_MAX) ? VMM_MEM_TYPE_MAX : data->buf_info.mem_type],
                     status_tab[(data->status > NODE_STATUS_MAX) ? NODE_STATUS_MAX : data->status],
                     location_tab[(data->location > VMM_LOCATION_MAX) ? VMM_LOCATION_MAX : data->location],
                     data->ref_cnt, data->dbg_info.alloc_time_consume_in_us);
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return LIST_SUCCESS;
}

hi_s32 vmm_mem_list_find_one_node_can_be_rls(mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        if (is_mem_node_can_be_rls(&mem_node->data) == VMM_TRUE) {
            /* PRINT(PRN_ALWS, "%s, %d, pMemNode->data.BufInfo.start_phy_addr = 0x%x\n",
             __func__, __LINE__, pMemNode->data.BufInfo.start_phy_addr); */
            VFMW_CHECK_SEC_FUNC(memcpy_s(data, sizeof(mem_data_info), &mem_node->data, sizeof(mem_data_info)));
            ret = LIST_SUCCESS;

            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_takeout_one_node_can_be_rls(vmm_rls_burst *rls_burst, mem_data_info *data)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;
    hi_u64 target_priv_id;
    hi_u64 cur_priv_id = 0;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    target_priv_id = (hi_u64)rls_burst->priv_id;
    target_priv_id &= rls_burst->priv_mask;

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);

        cur_priv_id = (hi_u64)mem_node->data.buf_info.priv_id;
        cur_priv_id &= rls_burst->priv_mask;

        if ((is_mem_node_can_be_rls(&mem_node->data) == VMM_TRUE) && (cur_priv_id == target_priv_id)) {
            /*
             * PRINT(PRN_ALWS, "%s, %d, pMemNode->data.BufInfo.start_phy_addr = 0x%x\n",
             * __func__, __LINE__, pMemNode->data.BufInfo.start_phy_addr);
              */
            VFMW_CHECK_SEC_FUNC(memcpy_s(data, sizeof(mem_data_info), &mem_node->data, sizeof(mem_data_info)));

            OS_LIST_DEL(&mem_node->list);
            OS_LIST_ADD_TAIL(&mem_node->list, &g_mem_list_ctx->free);

            ret = LIST_SUCCESS;

            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_is_one_node_can_be_rls(UADDR target_phy_addr)
{
    hi_s32 ret = LIST_FAILURE;
    mem_node_info *mem_node = HI_NULL;
    UADDR phy_addr;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_mem_list_ctx, ERR_LIST_NOT_INIT);

    if (target_phy_addr == 0 || target_phy_addr == 0xffffffff) {
        return ERR_LIST_INVALID_PARA;
    }

    LIST_LOCK(g_mem_list_ctx);

    if (list_is_empty(g_mem_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_mem_list_ctx);

        return ERR_LIST_EMPTY;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_mem_list_ctx->head) {
        mem_node = OS_LIST_ENTRY(pos, mem_node_info, list);
        phy_addr = mem_node->data.buf_info.start_phy_addr;

        if ((phy_addr == target_phy_addr) &&
#ifndef MEM_REFCNT_SUPPORT
            (is_mem_node_can_be_rls(&mem_node->data))) {
#else
            (mem_node->data.status != NODE_STATUS_INUSE)) {
#endif
            /* PRINT(PRN_ALWS, "%s, %d, phy_addr = 0x%x\n", __func__, __LINE__, phy_addr); */
            ret = LIST_SUCCESS;
            break;
        }
    }

    LIST_UNLOCK(g_mem_list_ctx);

    return ret;
}

hi_s32 vmm_mem_list_init(hi_void)
{
    vmm_list *vmm_list_ctx = HI_NULL;
    vmm_list_config mem_list_cfg;

    get_default_mem_list_cfg(&mem_list_cfg);

    if (g_mem_list_ctx == HI_NULL) {
        mem_list_cfg.type = LIST_MEM;
        vmm_list_ctx = vmm_init(&mem_list_cfg);
        if (!vmm_list_ctx) {
            return ERR_LIST_INIT_FAILED;
        }

        g_mem_list_ctx = vmm_list_ctx;
    }

    return LIST_SUCCESS;
}

hi_void vmm_mem_list_deinit(hi_void)
{
    D_VMM_CHECK_PTR(g_mem_list_ctx);

    list_deinit(g_mem_list_ctx);

    g_mem_list_ctx = HI_NULL;

    return;
}


