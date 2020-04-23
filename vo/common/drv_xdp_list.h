#ifndef __DRV_XDP_LIST_H__
#define __DRV_XDP_LIST_H__

#include "hi_type.h"

#ifdef __cplusplus
    #if __cplusplus
    extern "C" {
    #endif
#endif

typedef enum {
    LIST_TYPE_FRAME_TYPE = 0,
    LIST_TYPE_BUTT_TYPE,
}common_list_type;


typedef struct {
    struct osal_list_head  buf_node_head;
    common_list_type       list_type;
} common_list;

#define COMMON_GET_NODE_CONTENT(list_node, node_type, content_ptr) do {\
    content_ptr = (osal_list_entry(list_node, node_type, list_node));\
} while(0)

hi_s32 common_list_init(common_list *buf_list);
hi_s32 common_list_add_node(common_list *buf_list, struct osal_list_head *list_node);
hi_s32 common_list_del_node(common_list *buf_list, struct osal_list_head *list_node);
hi_s32 common_list_get_del_node(common_list *buf_list, struct osal_list_head **list_node);
hi_s32 common_list_get_node(common_list *buf_list, struct osal_list_head **list_node);
hi_s32 common_list_get_node_cnt(common_list *buf_list, hi_u32 *node_cnt);

#ifdef __cplusplus
    #if __cplusplus
    }
    #endif
#endif

#endif
