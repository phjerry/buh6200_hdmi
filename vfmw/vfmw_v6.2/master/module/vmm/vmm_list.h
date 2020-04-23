/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VMM_LIST_COMMON_H__
#define __VMM_LIST_COMMON_H__

#include "vmm_ext.h"
#include "vmm_dbg.h"

#define LIST_SUCCESS                (hi_s32)(0)
#define LIST_FAILURE                (hi_s32)(0xFFFFFFFF)
#define ERR_LIST_ALLOC_FAILED       (hi_s32)(0xFF010000)
#define ERR_LIST_INIT_FAILED        (hi_s32)(0xFF010001)
#define ERR_LIST_INVALID_PARA       (hi_s32)(0xFF010002)
#define ERR_LIST_NULL_PTR           (hi_s32)(0xFF010003)
#define ERR_LIST_NOT_SUPPORT        (hi_s32)(0xFF010004)
#define ERR_LIST_FULL               (hi_s32)(0xFF010005)
#define ERR_LIST_EMPTY              (hi_s32)(0xFF010006)
#define ERR_LIST_NOT_INIT           (hi_s32)(0xFF010007)
#define ERR_LIST_NODE_ALREADY_EXIST (hi_s32)(0xFF010008)
#define ERR_LIST_LOCATION_CONFLICT  (hi_s32)(0xFF010009)
#define ERR_LIST_NODE_NOT_EXIST     (hi_s32)(0xFF01000A)

#define LIST_TRUE  (hi_s32)(1)
#define LIST_FALSE (hi_s32)(0)

#define LIST_LOCK(list)                                         \
    do {                                                        \
        if (list->cfg.lock_mode == LOCK_MODE_MUTEX) {           \
            hi_s32 ret;                                         \
            ret = OS_SEMA_DOWN(list->mutex);                    \
        } else if (list->cfg.lock_mode == LOCK_MODE_SPINLOCK) { \
            OS_SPIN_LOCK(list->lock, &list->lock_flags);        \
        }                                                       \
    } while (0)

#define LIST_UNLOCK(list)                                       \
    do {                                                        \
        if (list->cfg.lock_mode == LOCK_MODE_MUTEX) {           \
                (hi_void)OS_SEMA_UP(list->mutex);                     \
        } else if (list->cfg.lock_mode == LOCK_MODE_SPINLOCK) { \
            OS_SPIN_UNLOCK(list->lock, &list->lock_flags);      \
        }                                                       \
    } while (0)

typedef enum {
    LIST_TYPE_HEAD,
    LIST_TYPE_FREE,
    LIST_TYPE_INTERNAL,
    LIST_TYPE_MAX
} vmm_list_type;

typedef enum {
    ALLOC_MODE_VMALLOC,
    ALLOC_MODE_KMALLOC_ATOMIC,
    ALLOC_MODE_MAX
} vmm_alloc_mode;

typedef enum {
    LOCK_MODE_MUTEX,
    LOCK_MODE_SPINLOCK,
    LOCK_MODE_MAX
} vmm_lock_mode;

typedef enum {
    LIST_MEM,
    LIST_MSG,
    LIST_MAX
} vmm_list_class;

typedef struct {
    vmm_list_class type;
    hi_u32 max_expansion_factor;
    hi_u32 node_num;
    hi_u32 per_node_size;
    hi_u32 mark_no; /* priv reserve */

    vmm_alloc_mode mem_alloc_mode;
    vmm_lock_mode lock_mode;
} vmm_list_config;

typedef struct {
    vmm_list_config cfg;
    hi_u32 cur_node_num;
    hi_u32 cur_expansion_factor;

    struct OS_LIST_HEAD data;
    struct OS_LIST_HEAD head;
    struct OS_LIST_HEAD free;

    OS_LOCK lock;
    hi_ulong lock_flags;
    OS_SEMA mutex;
} vmm_list;

vmm_list *vmm_init(const vmm_list_config *list_cfg);

hi_void list_deinit(vmm_list *list);

hi_s32 list_is_empty(vmm_list *list, vmm_list_type type);

hi_s32 list_expand_len(vmm_list *list);

#endif /* __VMM_LIST_COMMON_H__ */
