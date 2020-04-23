/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description:  common list function
* Author: xdp
* Create: 2019-08-12
 */

#include "hi_type.h"
#include "hi_errno.h"
#include "osal_list.h"
#include "drv_xdp_list.h"


hi_s32 common_list_init(common_list *buf_list)
{
    OSAL_INIT_LIST_HEAD(&(buf_list->buf_node_head));
    return HI_SUCCESS;
}

hi_s32 common_list_add_node(common_list *buf_list, struct osal_list_head *list_node)

{
    osal_list_add_tail(list_node, &buf_list->buf_node_head);
    return HI_SUCCESS;
}

hi_s32 common_list_del_node(common_list *buf_list, struct osal_list_head *list_node)

{
    struct osal_list_head *pos = HI_NULL;
    struct osal_list_head *n   = HI_NULL;

    osal_list_for_each_safe(pos, n, &(buf_list->buf_node_head)) {
        if (pos == list_node) {
            osal_list_del(list_node);
            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 common_list_get_del_node(common_list *buf_list,
                                struct osal_list_head **list_node)
{
    if (osal_list_empty(&buf_list->buf_node_head)) {
        *list_node = HI_NULL;
        return HI_FAILURE;
    }

    *list_node = buf_list->buf_node_head.next;
    osal_list_del(*list_node);
    return HI_SUCCESS;
}

hi_s32 common_list_get_node(common_list *buf_list, struct osal_list_head **list_node)
{
    if (osal_list_empty(&buf_list->buf_node_head)) {
        *list_node = HI_NULL;
        return HI_FAILURE;
    }

    *list_node = buf_list->buf_node_head.next;
    return HI_SUCCESS;
}

hi_s32 common_list_get_node_cnt(common_list *pstBuffer, hi_u32 *node_cnt)
{
    struct osal_list_head *pos = HI_NULL, *n = HI_NULL;

    *node_cnt = 0;
    osal_list_for_each_safe(pos, n, &(pstBuffer->buf_node_head)) {
        (*node_cnt) ++;
    }

    return HI_SUCCESS;
}
