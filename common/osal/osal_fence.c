/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"
#include "linux/hisilicon/securec.h"
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/spinlock.h>

#define FENCE_READY    1
#define FENCE_NOREADY  0
typedef struct osal_fence_{
    int module_id;
    int fd;
    wait_queue_head_t queue;
    int condition;
    void *private;
    unsigned int private_size;
    unsigned int ref;
    struct osal_list_head node;
} osal_fence;

int g_fd_no = 0; /* 临时占用。1秒 1000个，可使用49天 */

static DEFINE_SPINLOCK(lock);
static OSAL_LIST_HEAD(fence_list);

#define FENCE_PRIVATE_MAX_SIZE 0x100

void osal_fence_try_del(osal_fence *fence)
{
    if (fence->ref > 0) {
        return;
    }

    osal_list_del(&fence->node);

    if (fence->private != NULL) {
        kfree(fence->private);
        fence->private = NULL;
    }

    kfree(fence);
}

osal_fence *osal_fence_find_node(int fd, struct osal_list_head *list)
{
    osal_fence *fence_node = NULL;

    osal_list_for_each_entry(fence_node, list, node) {
        if (fence_node->fd == fd) {
            return fence_node;
        }
    }

    return NULL;
}

int osal_fence_create(unsigned int module_id, int *fd, unsigned int private_size)
{
    osal_fence *fence_node = NULL;
    unsigned long flags;
    errno_t err;

    if (fd == NULL) {
        return -1;
    }

    if (private_size > FENCE_PRIVATE_MAX_SIZE) {
        return -1;
    }

    fence_node = kmalloc(sizeof(osal_fence), GFP_KERNEL);
    if (fence_node == NULL) {
        return -1;
    }

    err = memset_s(fence_node, sizeof(osal_fence), 0, sizeof(osal_fence));
    if (err != EOK) {
        kfree(fence_node);
        return -1;
    }

    fence_node->private_size = private_size;
    if (private_size != 0) {
        fence_node->private = kmalloc(private_size, GFP_KERNEL);
        if (fence_node->private == NULL) {
            kfree(fence_node);
            return -1;
        }
    } else {
        fence_node->private = NULL;
    }

    fence_node->module_id = module_id;
    fence_node->condition = FENCE_NOREADY;
    fence_node->ref = 1;

    init_waitqueue_head(&fence_node->queue);

    spin_lock_irqsave(&lock, flags);
    fence_node->fd = g_fd_no;
    g_fd_no++;

    osal_list_add_tail(&(fence_node->node), &fence_list);

    *fd = fence_node->fd;

    spin_unlock_irqrestore(&lock, flags);

    return 0;
}
EXPORT_SYMBOL(osal_fence_create);

int osal_fence_destroy(unsigned int module_id, int fd)
{
    osal_fence *fence_node = NULL;
    unsigned long flags;

    spin_lock_irqsave(&lock, flags);

    fence_node = osal_fence_find_node(fd, &fence_list);
    if (fence_node == NULL) {
        spin_unlock_irqrestore(&lock, flags);
        return -1;
    }

    fence_node->ref--;

    osal_fence_try_del(fence_node);

    spin_unlock_irqrestore(&lock, flags);

    return 0;
}
EXPORT_SYMBOL(osal_fence_destroy);

int osal_fence_acquire_private(unsigned int module_id, int fd, void **private)
{
    osal_fence *fence_node = NULL;
    unsigned long flags;

    if (private == NULL) {
        return -1;
    }

    spin_lock_irqsave(&lock, flags);

    fence_node = osal_fence_find_node(fd, &fence_list);
    if (fence_node == NULL) {
        spin_unlock_irqrestore(&lock, flags);
        return -1;
    }

    fence_node->ref++;

    spin_unlock_irqrestore(&lock, flags);

    *private = fence_node->private;

    return 0;
}
EXPORT_SYMBOL(osal_fence_acquire_private);

int osal_fence_release_private(unsigned int module_id, int fd, void *private)
{
    osal_fence *fence_node = NULL;
    unsigned long flags;

    spin_lock_irqsave(&lock, flags);

    fence_node = osal_fence_find_node(fd, &fence_list);
    if (fence_node == NULL) {
        spin_unlock_irqrestore(&lock, flags);
        return -1;
    }

    fence_node->ref--;

    osal_fence_try_del(fence_node);

    spin_unlock_irqrestore(&lock, flags);

    return 0;
}
EXPORT_SYMBOL(osal_fence_release_private);

int osal_fence_signal(int fd)
{
    osal_fence *fence_node = NULL;
    unsigned long flags;

    spin_lock_irqsave(&lock, flags);

    fence_node = osal_fence_find_node(fd, &fence_list);
    if (fence_node == NULL) {
        spin_unlock_irqrestore(&lock, flags);
        return -1;
    }

    fence_node->condition = FENCE_READY;

    wake_up(&fence_node->queue);

    spin_unlock_irqrestore(&lock, flags);

    return 0;
}
EXPORT_SYMBOL(osal_fence_signal);

int osal_fence_wait(int fd, unsigned int ms)
{
    osal_fence *fence_node = NULL;
    long timeout;
    unsigned long flags;

    spin_lock_irqsave(&lock, flags);

    fence_node = osal_fence_find_node(fd, &fence_list);
    if (fence_node == NULL) {
        spin_unlock_irqrestore(&lock, flags);
        return -1;
    }

    fence_node->ref++;

    spin_unlock_irqrestore(&lock, flags);

    timeout = wait_event_interruptible_timeout(fence_node->queue,
                                               (fence_node->condition == FENCE_READY),
                                               msecs_to_jiffies(ms));
    spin_lock_irqsave(&lock, flags);
    fence_node->ref--;
    osal_fence_try_del(fence_node);
    spin_unlock_irqrestore(&lock, flags);

    if (timeout == 0) {
        return -1;
    }

    return 0;
}
EXPORT_SYMBOL(osal_fence_wait);

int osal_fence_trywait(int fd)
{
    osal_fence *fence_node = NULL;
    unsigned long flags;

    spin_lock_irqsave(&lock, flags);

    fence_node = osal_fence_find_node(fd, &fence_list);
    if (fence_node == NULL) {
        spin_unlock_irqrestore(&lock, flags);
        return -1;
    }

    if (fence_node->condition == FENCE_READY) {
        spin_unlock_irqrestore(&lock, flags);
        return 0;
    }

    spin_unlock_irqrestore(&lock, flags);

    return OSAL_FENCE_ERR_NOT_READY; /* not ready */
}
EXPORT_SYMBOL(osal_fence_trywait);

int osal_fence_get_num(void)
{
    osal_fence *fence_node = NULL;
    unsigned long flags;
    int num = 0;

    spin_lock_irqsave(&lock, flags);

    osal_list_for_each_entry(fence_node, &fence_list, node) {
        num++;
    }

    spin_unlock_irqrestore(&lock, flags);

    return num;
}
EXPORT_SYMBOL(osal_fence_get_num);

