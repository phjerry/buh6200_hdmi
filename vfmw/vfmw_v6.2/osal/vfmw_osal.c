/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vfmw_osal.h"
#include "vfmw_proc.h"
#include "dbg.h"
#include "vfmw_define.h"

vfmw_osal_ops g_vfmw_osal_ops;

extern hi_void osal_intf_init(hi_void);
extern hi_void osal_intf_exit(hi_void);

#define OS_ALLOC_VIR_MEM g_vfmw_osal_ops.alloc_vir_mem
#define OS_FREE_VIR_MEM  g_vfmw_osal_ops.free_vir_mem

/* #define VIR_ARRAY_SUPPORT */
#ifdef VIR_ARRAY_SUPPORT
#define MAX_VIR_NAME (20)
#define MAX_VIR_NODE (1000)

typedef struct {
    hi_bool is_used;
    hi_void *vir_addr;
    hi_s32 size;
    hi_u8 name[MAX_VIR_NAME];
} os_vir_node;

static hi_s32 g_total_num = 0;
static hi_s32 g_total_size = 0;
static os_vir_node g_vir_array[MAX_VIR_NODE];
static OS_SEMA g_vir_sem;

hi_void os_init_vir_array(hi_void)
{
    g_total_num = 0;
    g_total_size = 0;
    (void)memset_s(g_vir_array, sizeof(g_vir_array), 0, sizeof(g_vir_array));
    OS_SEMA_INIT(&g_vir_sem);
}

hi_s32 os_add_vir_node(hi_u8 *name, hi_void *vir_addr, hi_s32 size)
{
    hi_u32 i;
    hi_s32 ret = OSAL_ERR;
    os_vir_node *node = HI_NULL;

    OS_SEMA_DOWN(g_vir_sem);

    for (i = 0; i < MAX_VIR_NODE; i++) {
        node = &(g_vir_array[i]);
        if (node->is_used == HI_FALSE) {
            node->is_used = HI_TRUE;
            node->vir_addr = vir_addr;
            node->size = size;
            if (snprintf_s(node->name, MAX_VIR_NAME, MAX_VIR_NAME, name) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            node->name[MAX_VIR_NAME - 1] = '\0';
            g_total_num++;
            g_total_size += size;
            ret = OSAL_OK;
            break;
        }
    }

    OS_SEMA_UP(g_vir_sem);

    return ret;
}

hi_s32 os_del_vir_node(hi_void *vir_addr)
{
    hi_u32 i;
    hi_s32 ret = OSAL_ERR;
    os_vir_node *node = HI_NULL;

    OS_SEMA_DOWN(g_vir_sem);

    for (i = 0; i < MAX_VIR_NODE; i++) {
        node = &(g_vir_array[i]);
        if (node->is_used == HI_TRUE && node->vir_addr == vir_addr) {
            node->is_used = HI_FALSE;
            g_total_num = (g_total_num > 0) ? g_total_num - 1 : 0;
            g_total_size = (g_total_size - node->size >= 0) ? (g_total_size - node->size) : 0;
            ret = OSAL_OK;
            break;
        }
    }

    OS_SEMA_UP(g_vir_sem);

    return ret;
}

hi_void os_find_used_node(os_vir_node **pp_node, hi_u32 *idx)
{
    hi_u32 i;
    hi_u32 pos = *idx;
    os_vir_node *node = HI_NULL;

    *pp_node = HI_NULL;
    for (i = pos; i < MAX_VIR_NODE; i++) {
        node = &g_vir_array[i];
        if (node->is_used == HI_TRUE) {
            *pp_node = node;
            *idx = i + 1;
            break;
        }
    }
}

hi_void os_read_vir_array(hi_void *buf, hi_s32 len, hi_s32 *used)
{
    hi_u32 idx = 0;
    os_vir_node *node0 = HI_NULL;
    os_vir_node *node1 = HI_NULL;

    OS_SEMA_DOWN(g_vir_sem);

    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_HEAD, "VIR");

    do {
        os_find_used_node(&node0, &idx);
        if (node0 == HI_NULL) {
            break;
        }
        os_find_used_node(&node1, &idx);
        if (node1 == HI_NULL) {
            OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D, node0->name, node0->size);
            break;
        }

        OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, node0->name, node0->size, node1->name, node1->size);
    } while (node1 != HI_NULL);

    OS_DUMP_PROC(buf, len, used, 0, "  Total num: %d / Total size: %d (%dKB)\n", g_total_num, g_total_size,
                 (g_total_size / 1024)); /* 1024 :a number */
    OS_DUMP_PROC(buf, len, used, 0, "\n");

    OS_SEMA_UP(g_vir_sem);
}
#endif

hi_void os_read_osal(hi_void *buf, hi_s32 len, hi_s32 *used)
{
#ifdef VIR_ARRAY_SUPPORT
    os_read_vir_array(buf, len, used);
#endif
}

hi_void os_intf_exit(hi_void)
{
    osal_intf_exit();
}

hi_void *os_alloc_vir(hi_char *name, hi_s32 size)
{
    hi_void *vir_addr = HI_NULL;

    VFMW_ASSERT_RET(name != HI_NULL, HI_NULL);
    VFMW_ASSERT_RET_PRNT(size > 0, HI_NULL, "Alloc vir %s failed of size %d invalid!\n", name, size);

    vir_addr = OS_ALLOC_VIR_MEM(size);
    if (vir_addr == HI_NULL) {
        return HI_NULL;
    }

#ifdef VIR_ARRAY_SUPPORT
    if (os_add_vir_node(name, vir_addr, size) != OSAL_OK) {
        dprint(PRN_ERROR, "Alloc vir %s failed of vir array is full\n", name);
        OS_FREE_VIR_MEM(vir_addr);
        return HI_NULL;
    }
#endif

    return vir_addr;
}

hi_void os_free_vir(hi_void *vir_addr)
{
#ifdef VIR_ARRAY_SUPPORT
    os_del_vir_node(vir_addr);
#endif
    OS_FREE_VIR_MEM(vir_addr);
}

hi_void list_add_(struct OS_LIST_HEAD *new, struct OS_LIST_HEAD *prev, struct OS_LIST_HEAD *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

hi_void list_del_(struct OS_LIST_HEAD *prev, struct OS_LIST_HEAD *next)
{
    next->prev = prev;
    prev->next = next;
}

hi_void os_intf_init(hi_void)
{
    osal_intf_init();
    g_vfmw_osal_ops.read_osal = os_read_osal;
    g_vfmw_osal_ops.alloc_vir = os_alloc_vir;
    g_vfmw_osal_ops.free_vir = os_free_vir;
#ifdef VIR_ARRAY_SUPPORT
    os_init_vir_array();
#endif
}


