/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"

#include <linux/slab.h>
#include <linux/spinlock.h>

int hi_drv_module_register(unsigned int module_id, const char *pu8_module_name, void *p_func);
int hi_drv_module_unregister(unsigned int module_id);
int hi_drv_module_get_func(unsigned int module_id, void **pp_func);

typedef struct exportfunc_node_ {
    unsigned int module_id;
    void *func;
    struct osal_list_head node;
}exportfunc_node;

static DEFINE_SPINLOCK(g_func_lock);
static OSAL_LIST_HEAD(g_func_list);

void *osal_exportfunc_find_node(unsigned int module_id, struct osal_list_head *list)
{
    exportfunc_node *func_node = NULL;

    osal_list_for_each_entry(func_node, list, node) {
        if (func_node->module_id == module_id) {
            return func_node;
        }
    }

    return NULL;
}

int osal_exportfunc_register(unsigned int module_id, const char *name, void *func)
{
    unsigned long flags;
    exportfunc_node *func_node_new  = NULL;
    exportfunc_node *func_node_find = NULL;

    return hi_drv_module_register(module_id, name, func);

    func_node_new = kmalloc(sizeof(exportfunc_node), GFP_KERNEL);
    if (func_node_new == NULL) {
        return -1;
    }

    spin_lock_irqsave(&g_func_lock, flags);

    func_node_find = osal_exportfunc_find_node(module_id, &g_func_list);
    if (func_node_find != NULL) {
        kfree(func_node_new);
        spin_unlock_irqrestore(&g_func_lock, flags);
        return -1;
    }

    func_node_new->func = func;
    func_node_new->module_id = module_id;

    osal_list_add_tail(&(func_node_new->node), &g_func_list);

    spin_unlock_irqrestore(&g_func_lock, flags);

    return 0;
}
EXPORT_SYMBOL(osal_exportfunc_register);

int osal_exportfunc_unregister(unsigned int module_id)
{
    unsigned long flags;
    exportfunc_node *func_node_find = NULL;

    return hi_drv_module_unregister(module_id);

    spin_lock_irqsave(&g_func_lock, flags);

    func_node_find = osal_exportfunc_find_node(module_id, &g_func_list);
    if (func_node_find != NULL) {
        spin_unlock_irqrestore(&g_func_lock, flags);
        return -1;
    }

    osal_list_del(&func_node_find->node);
    kfree(func_node_find);

    spin_unlock_irqrestore(&g_func_lock, flags);

    return 0;
}
EXPORT_SYMBOL(osal_exportfunc_unregister);

int osal_exportfunc_get(unsigned int module_id, void **func)
{
    unsigned long flags;
    exportfunc_node *func_node_find = NULL;

    if (func == NULL) {
        return -1;
    }

    return hi_drv_module_get_func(module_id, func);

    spin_lock_irqsave(&g_func_lock, flags);

    func_node_find = osal_exportfunc_find_node(module_id, &g_func_list);
    if (func_node_find == NULL) {
        *func = NULL;
        spin_unlock_irqrestore(&g_func_lock, flags);
        return -1;
    }

    *func = func_node_find->func;

    spin_unlock_irqrestore(&g_func_lock, flags);

    return 0;
}
EXPORT_SYMBOL(osal_exportfunc_get);
