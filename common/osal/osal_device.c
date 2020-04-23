/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/kmod.h>
#include <linux/freezer.h>
#include <linux/rwlock.h>
#include <linux/rwlock_types.h>
#include "hi_osal.h"
#include "himedia.h"
#include "linux/hisilicon/securec.h"

typedef struct osal_dev_info_ {
    osal_dev *dev;
    pm_device meida;
    struct osal_list_head node;
} osal_dev_info;

typedef struct osal_fileops_info_ {
    void *private_data;
    osal_fileops *fops;
}osal_fileops_info;

static OSAL_LIST_HEAD(dev_list);

static DEFINE_RWLOCK(g_dev_rwlock);

osal_pmops *osal_get_pmops(pm_basedev *pm_base)
{
    osal_dev_info *dev_node = NULL;

    if (pm_base == NULL) {
        return NULL;
    }

    read_lock(&g_dev_rwlock);

    osal_list_for_each_entry(dev_node, &dev_list, node) {
        if (osal_strncmp(dev_node->dev->name, strlen(dev_node->dev->name),
            pm_base->name, strlen(dev_node->dev->name)) == 0)  {
            read_unlock(&g_dev_rwlock);
            return dev_node->dev->pmops;
        }
    }

    read_unlock(&g_dev_rwlock);

    return NULL;
}

static int osal_open(struct inode *inode, struct file *file)
{
    int ret = 0;
    osal_fileops *fops = NULL;
    osal_dev *dev = NULL;
    osal_fileops_info *fileops_node = NULL;

    dev = (osal_dev *)file->private_data;
    if (dev == NULL) {
        osal_printk("%s - get himedia device error!\n", __FUNCTION__);
        return -1;
    }

    fileops_node = (osal_fileops_info *)kzalloc(sizeof(osal_fileops_info), GFP_KERNEL);
    if (fileops_node == NULL) {
        osal_printk("%s - fileops_node kmalloc failed!\n", __FUNCTION__);
        return -1;
    }

    fops = dev->fops;
    if ((fops != NULL) && (fops->open != NULL)) {
        ret = fops->open((void *)&(fileops_node->private_data));
    }

    if (ret != 0) {
        kfree(fileops_node);
        return ret;
    }

    fileops_node->fops = fops;
    file->private_data = fileops_node;

    return ret;
}

static ssize_t osal_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    int ret = 0;
    osal_fileops *fops = NULL;
    osal_fileops_info *fileops_node = NULL;

    fileops_node = file->private_data;
    if (fileops_node == NULL) {
        osal_printk("%s - fileops_node NULL!\n", __FUNCTION__);
        return -1;
    }

    fops = fileops_node->fops;
    if (fops == NULL) {
        osal_printk("%s - fops NULL!\n", __FUNCTION__);
        return -1;
    }

    if (fops->read != NULL) {
        ret = fops->read(buf, (int)size, (long *)offset, (void *)&(fileops_node->private_data));
    }

    return ret;
}

static ssize_t osal_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    return 0;
}

static loff_t osal_llseek(struct file *file, loff_t offset, int whence)
{
    return 0;
}

static int osal_release(struct inode *inode, struct file *file)
{
    int ret = 0;
    osal_fileops *fops = NULL;
    osal_fileops_info *fileops_node = NULL;

    fileops_node = file->private_data;
    if (fileops_node == NULL) {
        osal_printk("%s - fileops_node NULL!\n", __FUNCTION__);
        return -1;
    }

    fops = fileops_node->fops;
    if (fops == NULL) {
        osal_printk("%s - fops NULL!\n", __FUNCTION__);
        return -1;
    }

    if (fops->release != NULL) {
        ret = fops->release((void *)&(fileops_node->private_data));
    }

    kfree(fileops_node);
    file->private_data = NULL;

    return ret;
}

#define ARG_BUF_TEMP_SIZE 256
#define CMD_NUM_MAX       2048

static int osal_ioctl_copy_from_user(unsigned int cmd, unsigned long arg,
    void **ioctl_arg, unsigned int arg_size, char *arg_buf)
{
    int ret = -1;

    /*  Copy arguments into temp kernel buffer  */
    switch (_IOC_DIR(cmd)) {
        case _IOC_NONE:
            *ioctl_arg = NULL;
            break;
        case _IOC_READ:
        case _IOC_WRITE:
        case (_IOC_WRITE | _IOC_READ):
            if (arg_size <= ARG_BUF_TEMP_SIZE) {
                *ioctl_arg = arg_buf;
            } else {
                /* too big to allocate from stack; <16K use kmalloc */
                *ioctl_arg = kmalloc(arg_size, GFP_KERNEL);
                if (*ioctl_arg == NULL) {
                    printk("kmalloc cmd buffer failed\n");
                    return -ENOMEM;
                }
            }

            if (_IOC_DIR(cmd) & _IOC_WRITE) {
                ret = copy_from_user(*ioctl_arg, (void __user *)(uintptr_t)arg, arg_size);
                if (ret != 0) {
                    printk("copy_from_user failed, cmd=0x%x\n", cmd);
                    return -1;
                }
            }

            break;
    }

    return 0;
}

static int osal_ioctl_call_fun(unsigned int cmd, void *ioctl_arg, osal_fileops *fops, void *private_data)
{
    unsigned int i = 0;

    /* call func; only used inosal_ioctl; no need check cmd, ioctl_arg, fops */
    for (i = 0; i < fops->cmd_cnt; i++) {
        if (fops->cmd_list[i].cmd == cmd) {
            if (fops->cmd_list[i].handler != NULL) {
                return fops->cmd_list[i].handler(cmd, ioctl_arg, private_data);
            }
        }
    }

    return -1;
}

static int osal_ioctl_copy_to_user(unsigned int cmd, unsigned long arg, const void *ioctl_arg, unsigned int arg_size)
{
    /*  Copy results into user buffer  */
    switch (_IOC_DIR(cmd)) {
        case _IOC_READ:
        case (_IOC_WRITE | _IOC_READ):
            {
                return copy_to_user((void __user *)(uintptr_t)arg, ioctl_arg, arg_size);
            }
        break;
    }

    return 0;
}

static long osal_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret;
    unsigned int arg_size;
    char arg_buf[ARG_BUF_TEMP_SIZE]; /* temp to prevent tiny vmalloc */
    void *ioctl_arg = NULL;
    osal_fileops *fops = NULL;
    osal_fileops_info *fileops_node = NULL;

    if (file == NULL) {
        osal_printk("%s - file NULL!\n", __FUNCTION__);
        return -1;
    }

    arg_size = _IOC_SIZE(cmd);

    if (_IOC_DIR(cmd) != _IOC_NONE) {
        if ((arg_size == 0) || (((char *)(uintptr_t)arg) == NULL)) {
            return -1; /* arg_size max 0x4000, no need to check max */
        }
    }

    fileops_node = file->private_data;
    if (fileops_node == NULL) {
        return -1;
    }

    fops = fileops_node->fops;
    if (fops == NULL) {
        return -1;
    }

    if ((fops->cmd_cnt > CMD_NUM_MAX) || (fops->cmd_cnt == 0) || (fops->cmd_list == NULL)) {
        return -1;
    }

    ret = osal_ioctl_copy_from_user(cmd, arg, &ioctl_arg, arg_size, arg_buf);
    if (ret != 0) {
        goto OUT;
    }

    ret = osal_ioctl_call_fun(cmd, ioctl_arg, fops, (void *)&(fileops_node->private_data));
    if (ret != 0) {
        goto OUT;
    }

    ret = osal_ioctl_copy_to_user(cmd, arg, ioctl_arg, arg_size);
    if (ret != 0) {
        goto OUT;
    }

OUT:
    if ((ioctl_arg != NULL) && (arg_size > ARG_BUF_TEMP_SIZE)) {
        kfree(ioctl_arg);
    }

    return ret;
}

static unsigned int __osal_poll(struct file *file, struct poll_table_struct *table)
{
    unsigned int ret = 0;
    osal_poll t;
    osal_fileops *fops = NULL;
    osal_fileops_info *fileops_node = NULL;

    t.poll_table = table;
    t.data = file;

    fileops_node = file->private_data;
    if (fileops_node == NULL) {
        osal_printk("%s - fileops_node NULL!\n", __FUNCTION__);
        return -1;
    }

    fops = fileops_node->fops;
    if (fops == NULL) {
        osal_printk("%s - fops NULL!\n", __FUNCTION__);
        return -1;
    }

    if (fops->poll != NULL) {
        ret = fops->poll(&t, (void *)&(fileops_node->private_data));
    }

    return ret;
}

int osal_valid_mmap_phys_addr_range(unsigned long pfn, size_t size)
{
    /* check physical addr greater than the max addr supported by the system */
    if ((pfn + (size >> PAGE_SHIFT)) > (1 + ((~0UL) >> PAGE_SHIFT))) {
        osal_printk("--%s - %d--!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    /* It's necessary for the variable "size" to align 4k(page_size). */
#define PAGE_SIZE_MASK 0xfffffffffffff000ULL
    if ((unsigned long)size & (~PAGE_SIZE_MASK)) {
        osal_printk("--%s - %d--!\n", __FUNCTION__, __LINE__);
        return 0;
    }
#undef PAGE_SIZE_MASK

    return 1;
}

static int osal_mmap(struct file *file, struct vm_area_struct *vm)
{
    int ret = 0;
    osal_vm osal_vm;
    osal_fileops *fops = NULL;
    osal_fileops_info *fileops_node = NULL;

    if (!osal_valid_mmap_phys_addr_range(vm->vm_pgoff, vm->vm_end - vm->vm_start)) {
        osal_printk("\n%s - invalid argument   size=%ld!!!\n", __FUNCTION__, vm->vm_end - vm->vm_start);
        return -EINVAL;
    }

    fileops_node = file->private_data;
    if (fileops_node == NULL) {
        osal_printk("%s - fileops_node NULL!\n", __FUNCTION__);
        return -1;
    }

    fops = fileops_node->fops;
    if (fops == NULL) {
        osal_printk("%s - fops NULL!\n", __FUNCTION__);
        return -1;
    }

    osal_vm.vm = vm;
    if (fops->mmap != NULL) {
        ret = fops->mmap(&osal_vm, vm->vm_start, vm->vm_end, vm->vm_pgoff, (void *)file);
    }

    return ret;
}

static struct file_operations g_osal_fops = {
    .owner = THIS_MODULE,
    .open = osal_open,
    .read = osal_read,
    .write = osal_write,
    .llseek = osal_llseek,
    .unlocked_ioctl = osal_ioctl,
    .release = osal_release,
    .poll = __osal_poll,
    .mmap = osal_mmap,
#ifdef CONFIG_COMPAT
    .compat_ioctl = osal_ioctl,
#endif
};

static void osal_pm_shutdown(pm_basedev *pm_base)
{
    osal_pmops *pmops = NULL;

    pmops = osal_get_pmops(pm_base);
    if (pmops == NULL) {
        return;
    }

    if (pmops->pm_poweroff != NULL) {
        (void)pmops->pm_poweroff(pmops->private_data);
    }
}

static int osal_pm_suspend(pm_basedev *pm_base, pm_message_t state)
{
    osal_pmops *pmops = NULL;

    pmops = osal_get_pmops(pm_base);
    if (pmops == NULL) {
        return 0;
    }

    if (pmops->pm_suspend != NULL) {
        return pmops->pm_suspend(pmops->private_data);
    }

    return -1;
}

static int osal_pm_resume(pm_basedev *pm_base)
{
    osal_pmops *pmops = NULL;

    pmops = osal_get_pmops(pm_base);
    if (pmops == NULL) {
        return 0;
    }

    if (pmops->pm_resume != NULL) {
        return pmops->pm_resume(pmops->private_data);
    }

    return -1;
}

static int osal_pm_resume_early(pm_basedev *pm_base)
{
    osal_pmops *pmops = NULL;

    pmops = osal_get_pmops(pm_base);
    if (pmops == NULL) {
        return 0;
    }

    if (pmops->pm_resume_early != NULL) {
        return pmops->pm_resume_early(pmops->private_data);
    }

    return 0;
}

void osal_pm_lowpower_enter(void)
{
    osal_dev_info *dev_node = NULL;

    osal_list_for_each_entry(dev_node, &dev_list, node) {
        if ((dev_node->dev != NULL) && (dev_node->dev->pmops != NULL)
            && (dev_node->dev->pmops->pm_lowpower_enter != NULL)) {
            dev_node->dev->pmops->pm_lowpower_enter(dev_node->dev->pmops->private_data);
        }
    }
}
EXPORT_SYMBOL(osal_pm_lowpower_enter);

void osal_pm_lowpower_exit(void)
{
    osal_dev_info *dev_node = NULL;

    osal_list_for_each_entry_reverse(dev_node, &dev_list, node) {
        if ((dev_node->dev != NULL) && (dev_node->dev->pmops != NULL)
            && (dev_node->dev->pmops->pm_lowpower_exit != NULL)) {
            dev_node->dev->pmops->pm_lowpower_exit(dev_node->dev->pmops->private_data);
        }
    }
}
EXPORT_SYMBOL(osal_pm_lowpower_exit);

static pm_baseops g_osal_pmops = {
    .probe = NULL,
    .remove = NULL,
    .shutdown = osal_pm_shutdown,
    .prepare = NULL,
    .complete = NULL,
    .suspend = osal_pm_suspend,
    .suspend_late = NULL,
    .resume_early = osal_pm_resume_early,
    .resume = osal_pm_resume,
};

int osal_dev_register(osal_dev *dev)
{
    int ret;
    osal_dev_info *dev_node = NULL;
    pm_device *meida = NULL;
    errno_t err;

    if (dev == NULL) {
        osal_printk("%s - dev invalid!\n", __FUNCTION__);
        return -1;
    }

    dev_node = (osal_dev_info *)kmalloc(sizeof(osal_dev_info), GFP_KERNEL);
    if (dev_node == NULL) {
        osal_printk("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }

    err = memset_s(dev_node, sizeof(osal_dev_info), 0, sizeof(osal_dev_info));
    if (err != EOK) {
        kfree(dev_node);
        dev_node = NULL;
        osal_printk("memset_s is failed.\n");
        return -1;
    }
    dev_node->dev = dev;
    dev->dev = (void *)dev_node;

    meida = &dev_node->meida;

    if (dev->minor != 0) {
        meida->minor = dev->minor;
    } else {
        meida->minor = HIMEDIA_DYNAMIC_MINOR;
    }

    meida->owner = THIS_MODULE;
    meida->app_ops = &g_osal_fops;
    meida->base_ops = &g_osal_pmops;
    meida->name = dev->name;
    meida->dev = dev;

    ret =  hi_drv_pm_register(meida);
    if (ret != 0) {
        kfree(dev_node);
        return -1;
    }

    write_lock(&g_dev_rwlock);
    osal_list_add(&dev_node->node, &dev_list);
    write_unlock(&g_dev_rwlock);

    return 0;
}
EXPORT_SYMBOL(osal_dev_register);

void osal_dev_unregister(osal_dev *dev)
{
    osal_dev_info *dev_node = NULL;

    if (dev == NULL || dev->dev == NULL) {
        osal_printk("%s - dev invalid!\n", __FUNCTION__);
        return;
    }

    dev_node = (osal_dev_info *)(dev->dev);

    hi_drv_pm_un_register(&dev_node->meida);

    write_lock(&g_dev_rwlock);
    osal_list_del(&dev_node->node);
    write_unlock(&g_dev_rwlock);

    kfree(dev_node);
    dev->dev = NULL;
}
EXPORT_SYMBOL(osal_dev_unregister);

void osal_poll_wait(osal_poll *table, osal_wait *wait)
{
    if (table == NULL) {
        osal_printk("%s - table(NULL) invalid!\n", __FUNCTION__);
        return;
    }

    if (wait == NULL) {
        osal_printk("%s - wait(NULL) invalid!\n", __FUNCTION__);
        return;
    }

    poll_wait((struct file *)table->data, (wait_queue_head_t *)(wait->wait), table->poll_table);
}
EXPORT_SYMBOL(osal_poll_wait);

int osal_remap_pfn_range(osal_vm *vm, unsigned long addr, unsigned long pfn, unsigned long size, unsigned int cached)
{
    struct vm_area_struct *v = NULL;

    if ((vm == NULL) || (vm->vm == NULL)) {
        osal_printk("%s - vm(NULL) invalid!\n", __FUNCTION__);
        return -1;
    }

    if (size == 0) {
        osal_printk("%s - size(0) invalid!\n", __FUNCTION__);
        return -1;
    }

    v = (struct vm_area_struct *)(vm->vm);

    return remap_pfn_range(v, addr, pfn, size, v->vm_page_prot);
}
EXPORT_SYMBOL(osal_remap_pfn_range);

int osal_try_to_freeze(void)
{
    return try_to_freeze();
}
EXPORT_SYMBOL(osal_try_to_freeze);

int osal_set_freezable(void)
{
    return set_freezable();
}
EXPORT_SYMBOL(osal_set_freezable);
