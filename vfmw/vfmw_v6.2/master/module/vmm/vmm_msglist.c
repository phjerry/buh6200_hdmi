/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vmm_msglist.h"
#include "vmm_list.h"
#include "vfmw_osal.h"

#define INIT_NODE_NUM    (200)
#define MAX_DATA_BUF_NUM (5)

VMM_PRIVATE vmm_list *g_msg_list_ctx = HI_NULL;

VMM_PRIVATE hi_s32 msg_list_move_from_free_to_head(vmm_msg_data *data, vmm_cmd_prio proir)
{
    hi_s32 ret = LIST_FAILURE;
    vmm_msg_node *msg_node = HI_NULL;

    LIST_LOCK(g_msg_list_ctx);

    msg_node = OS_LIST_FIRST_ENTRY(&g_msg_list_ctx->free, vmm_msg_node, list);
    if (msg_node != HI_NULL) {
        OS_LIST_DEL(&msg_node->list);

        VFMW_CHECK_SEC_FUNC(memcpy_s(&msg_node->data, sizeof(vmm_msg_data), data, sizeof(vmm_msg_data)));

        if (proir == VMM_CMD_PRIO_MAX) {
            OS_LIST_ADD(&msg_node->list, &g_msg_list_ctx->head);
        } else {
            OS_LIST_ADD_TAIL(&msg_node->list, &g_msg_list_ctx->head);
        }

        ret = LIST_SUCCESS;
    }

    LIST_UNLOCK(g_msg_list_ctx);

    return ret;
}

VMM_PRIVATE hi_s32 msg_list_move_from_head_to_free(vmm_msg_data *data)
{
    hi_s32 ret = LIST_FAILURE;
    vmm_msg_node *msg_node = HI_NULL;

    LIST_LOCK(g_msg_list_ctx);

    msg_node = OS_LIST_FIRST_ENTRY(&g_msg_list_ctx->head, vmm_msg_node, list);
    if (msg_node != HI_NULL) {
        OS_LIST_DEL(&msg_node->list);

        VFMW_CHECK_SEC_FUNC(memcpy_s(data, sizeof(vmm_msg_data), &msg_node->data, sizeof(vmm_msg_data)));

        OS_LIST_ADD_TAIL(&msg_node->list, &g_msg_list_ctx->free);

        ret = LIST_SUCCESS;
    }

    LIST_UNLOCK(g_msg_list_ctx);

    return ret;
}

VMM_PRIVATE hi_void print_error_detail_msg_data(vmm_msg_data *data)
{
    hi_s64 priv_id = 0xffffffff;
    vmm_rls_burst *rls_burst = HI_NULL;
    vmm_buffer *buf_rec = HI_NULL;

    switch (data->cmd_id) {
        case VMM_CMD_RELEASE_BURST: {
            rls_burst = &data->param.rls_burst;
            priv_id = rls_burst->priv_id;

            PRINT(PRN_ERROR, "%s,%d, ERROR: CurCmd = VMM_CMD_RELEASE_BURST, priv_mask = %d is invalid!\n",
                  __func__, __LINE__, rls_burst->priv_mask);
        }
        break;

        case VMM_CMD_ALLOC: {
            buf_rec = &data->param.buf_rec;
            priv_id = buf_rec->priv_id;

            PRINT(PRN_ERROR,
                  "%s,%d, ERROR: mem_type(%d) should < %d,cache(%d) should < 2, map(%d) should < 2, \
                sec_flag(%d) should < 2, size(%d) should != 0\n",
                  __func__, __LINE__, buf_rec->mem_type,
                  VMM_MEM_TYPE_MAX, buf_rec->cache, buf_rec->map, buf_rec->sec_flag, buf_rec->size);
        }
        break;

        case VMM_CMD_RELEASE: {
            buf_rec = &data->param.buf_rec;
            priv_id = buf_rec->priv_id;

            PRINT(PRN_ERROR,
                  "%s,%d, ERROR: mem_type(%d) should < %d,cache(%d) should < 2, map(%d) should < 2, \
                sec_flag(%d) should < 2, size(%d) should != 0\n",
                  __func__, __LINE__, buf_rec->mem_type,
                  VMM_MEM_TYPE_MAX, buf_rec->cache, buf_rec->map, buf_rec->sec_flag, buf_rec->size);
            PRINT(PRN_ERROR, "              Current start_phy_addr(0x%x) start_vir_addr(0x%lx)\n",
                  buf_rec->start_phy_addr, buf_rec->start_vir_addr);
        }
        break;

        default:
            PRINT(PRN_ERROR, "Unknow Command ID %d\n", data->cmd_id);
            break;
    }
}

VMM_PRIVATE hi_s32 msg_date_check(vmm_msg_data *data)
{
    hi_s64 priv_id = 0xffffffff;
    vmm_rls_burst *rls_burst = HI_NULL;
    vmm_buffer *buf_rec = HI_NULL;
    hi_s32 err_flag = 0;

    switch (data->cmd_id) {
        case VMM_CMD_RELEASE_BURST: {
            rls_burst = &data->param.rls_burst;
            priv_id = rls_burst->priv_id;

            err_flag |= (rls_burst->priv_mask == 0);
        }
        break;

        case VMM_CMD_ALLOC: {
            buf_rec = &data->param.buf_rec;
            priv_id = buf_rec->priv_id;

            err_flag |= (buf_rec->mem_type >= VMM_MEM_TYPE_MAX);
            err_flag |= (buf_rec->cache >= 2); /* 2 :a number */
            err_flag |= (buf_rec->map >= 2); /* 2 :a number */
            err_flag |= (buf_rec->sec_flag >= 2); /* 2 :a number */
            err_flag |= (buf_rec->size == 0);

            if (buf_rec->cache != 0) {
                err_flag |= (buf_rec->map == 0);
            }
        }
        break;

        case VMM_CMD_RELEASE: {
            buf_rec = &data->param.buf_rec;
            priv_id = buf_rec->priv_id;

            err_flag |= (buf_rec->mem_type >= VMM_MEM_TYPE_MAX);
            err_flag |= (buf_rec->cache >= 2); /* 2 :a number */
            err_flag |= (buf_rec->map >= 2); /* 2 :a number */
            err_flag |= (buf_rec->sec_flag >= 2); /* 2 :a number */
            err_flag |= (buf_rec->size == 0);
            err_flag |= (buf_rec->start_phy_addr == 0) || (buf_rec->start_phy_addr == 0xffffffff);

            if (buf_rec->map != 0) {
                err_flag |= (buf_rec->start_vir_addr == 0) || (buf_rec->start_vir_addr == (-1));
            } else {
                err_flag |= (buf_rec->start_vir_addr != 0) && (buf_rec->start_phy_addr != 0xffffffff);
            }

            if (buf_rec->cache != 0) {
                err_flag |= (buf_rec->map == 0);
            }
        }
        break;

        default:
            PRINT(PRN_ERROR, "Unknow Command ID %d\n", data->cmd_id);
            break;
    }

    if (priv_id != data->priv_id) {
        err_flag |= (priv_id != data->priv_id);
        PRINT(PRN_ERROR, "%s,%d, MsgData priv_id conflict!(%d,%d)\n", priv_id, data->priv_id);
    }

    if (err_flag != 0) {
        print_error_detail_msg_data(data);
        return LIST_FAILURE;
    }

    return LIST_SUCCESS;
}

VMM_PRIVATE hi_void get_default_msg_list_cfg(vmm_list_config *list_cfg)
{
    VFMW_CHECK_SEC_FUNC(memset_s(list_cfg, sizeof(vmm_list_config), 0, sizeof(vmm_list_config)));

    list_cfg->max_expansion_factor = MAX_DATA_BUF_NUM;
    list_cfg->node_num = INIT_NODE_NUM;
    list_cfg->per_node_size = sizeof(vmm_msg_node);
    list_cfg->mark_no = 0;
    list_cfg->mem_alloc_mode = ALLOC_MODE_KMALLOC_ATOMIC;
    list_cfg->lock_mode = LOCK_MODE_SPINLOCK;

    return;
}

VMM_PRIVATE hi_void check_and_delete_node(hi_u64 target_priv_id, vmm_msg_delete_info *msg_delete,
                                          vmm_msg_node *msg_node)
{
    hi_u64 cur_priv_id;
    hi_u32 delete_flag = 1;

    cur_priv_id = (hi_u64)msg_node->data.priv_id;
    cur_priv_id &= msg_delete->priv_mask;

    if (msg_delete->priv_id_valid) {
        delete_flag &= (cur_priv_id == target_priv_id);
    }

    if (msg_delete->cmd_valid) {
        delete_flag &= (msg_node->data.cmd_id == msg_delete->cmd_id);
    }

    if (delete_flag) {
        OS_LIST_DEL(&msg_node->list);
        OS_LIST_ADD_TAIL(&msg_node->list, &g_msg_list_ctx->free);
    }

    return;
}

hi_s32 vmm_msg_list_read_proc(hi_void *p)
{
    vmm_msg_node *msg_node = HI_NULL;
    vmm_msg_data *data = HI_NULL;
    hi_s32 used_size = 0;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;
    /* 16 :a number */
    hi_s8 msg_cmd_tab[VMM_CMD_TYPE_MAX + 1][16] = { { "Alloc" }, { "Release" }, { "ReleaseBurst" }, { "Unknown" } };

    D_VMM_CHECK_PTR_RET(g_msg_list_ctx, ERR_LIST_NOT_INIT);

    /* statistics info */
    OS_DUMP_PROC(p, 0, &used_size, 0,
                 "================================================= \
        MsgList Status ========================================================= \n");
    OS_DUMP_PROC(p, 0, &used_size, 0, "Curnode_num               : %d\n", g_msg_list_ctx->cur_node_num);
    OS_DUMP_PROC(p, 0, &used_size, 0, "per_node_size             : %d (B)\n", g_msg_list_ctx->cfg.per_node_size);
    OS_DUMP_PROC(p, 0, &used_size, 0, "ExpansionFactor(Cur/Max)  : %d/%d\n", g_msg_list_ctx->cur_expansion_factor,
                 g_msg_list_ctx->cfg.max_expansion_factor);

    LIST_LOCK(g_msg_list_ctx);

    if (list_is_empty(g_msg_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_msg_list_ctx);
        return LIST_SUCCESS;
    }

    OS_DUMP_PROC(p, 0, &used_size, 0,
                 "------------------------------------------------- \
        MsgNode Info ----------------------------------------------------------- \n");
    OS_DUMP_PROC(p, 0, &used_size, 0, "\tpriv_id\t    cmd_id\t\n");
    OS_DUMP_PROC(p, 0, &used_size, 0,
                 "-------------------------------------------------- \
        ------------------------------------------------------------------------ \n");

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_msg_list_ctx->head) {
        msg_node = OS_LIST_ENTRY(pos, vmm_msg_node, list);
        data = &msg_node->data;
        OS_DUMP_PROC(p, 0, &used_size, 0, "0x%-16llx %-16s \n", data->priv_id, msg_cmd_tab[data->cmd_id]);
    }

    LIST_UNLOCK(g_msg_list_ctx);

    return LIST_SUCCESS;
}

hi_s32 vmm_msg_list_add_node(vmm_msg_data *data, vmm_cmd_prio proir)
{
    hi_s32 ret = LIST_FAILURE;

    D_VMM_CHECK_PTR_RET(g_msg_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    if (proir > VMM_CMD_PRIO_MAX) {
        PRINT(PRN_ERROR, "%s,%d, proir not support!\n", __func__, __LINE__);
        return ERR_LIST_INVALID_PARA;
    }

    if (msg_date_check(data) != LIST_SUCCESS) {
        PRINT(PRN_ERROR, "%s cmd_id %d MsgDateCheck failed.\n", __func__, data->cmd_id);
        return ERR_LIST_INVALID_PARA;
    }

    if (list_is_empty(g_msg_list_ctx, LIST_TYPE_FREE)) {
        ret = list_expand_len(g_msg_list_ctx);
        if (ret != LIST_SUCCESS) {
            return ERR_LIST_FULL;
        }
    }

    ret = msg_list_move_from_free_to_head(data, proir);

    return ret;
}

/* the node which be taken out should be moved from head list to free list */
hi_s32 vmm_msg_list_take_out_node(vmm_msg_data *data)
{
    hi_s32 ret = LIST_FAILURE;

    D_VMM_CHECK_PTR_RET(g_msg_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(data, ERR_LIST_NULL_PTR);

    if (list_is_empty(g_msg_list_ctx, LIST_TYPE_HEAD)) {
        return ERR_LIST_EMPTY;
    }

    ret = msg_list_move_from_head_to_free(data);

    return ret;
}

hi_s32 vmm_msg_list_delete_specified_node(vmm_msg_delete_info *msg_delete)
{
    vmm_msg_node *msg_node = HI_NULL;
    hi_u64 target_priv_id;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    D_VMM_CHECK_PTR_RET(g_msg_list_ctx, ERR_LIST_NOT_INIT);
    D_VMM_CHECK_PTR_RET(msg_delete, ERR_LIST_NULL_PTR);

    VMM_ASSERT((msg_delete->priv_id_valid == 1) || (msg_delete->cmd_valid == 1));

    target_priv_id = (hi_u64)msg_delete->priv_id;
    target_priv_id &= msg_delete->priv_mask;

    LIST_LOCK(g_msg_list_ctx);

    if (list_is_empty(g_msg_list_ctx, LIST_TYPE_HEAD)) {
        LIST_UNLOCK(g_msg_list_ctx);
        return LIST_SUCCESS;
    }

    OS_LIST_FOR_EACH_SAFE(pos, n, &g_msg_list_ctx->head) {
        msg_node = OS_LIST_ENTRY(pos, vmm_msg_node, list);
        check_and_delete_node(target_priv_id, msg_delete, msg_node);
    }

    LIST_UNLOCK(g_msg_list_ctx);

    return LIST_SUCCESS;
}

hi_s32 vmm_msg_list_is_empty(hi_void)
{
    D_VMM_CHECK_PTR_RET(g_msg_list_ctx, ERR_LIST_NOT_INIT);

    return list_is_empty(g_msg_list_ctx, LIST_TYPE_HEAD);
}

hi_s32 vmm_msg_list_init(hi_void)
{
    vmm_list *msg_list_ctx = HI_NULL;
    vmm_list_config msg_list_cfg;

    get_default_msg_list_cfg(&msg_list_cfg);

    if (g_msg_list_ctx == HI_NULL) {
        msg_list_cfg.type = LIST_MSG;
        msg_list_ctx = vmm_init(&msg_list_cfg);
        if (!msg_list_ctx) {
            return ERR_LIST_INIT_FAILED;
        }

        g_msg_list_ctx = msg_list_ctx;
    }

    return LIST_SUCCESS;
}

hi_void vmm_msg_list_deinit(hi_void)
{
    D_VMM_CHECK_PTR(g_msg_list_ctx);

    list_deinit(g_msg_list_ctx);

    g_msg_list_ctx = HI_NULL;

    return;
}


