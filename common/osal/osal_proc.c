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
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/major.h>
#include <linux/uaccess.h>
#include <linux/file.h>

#include "hi_osal.h"
#include "linux/hisilicon/securec.h"

#define OSAL_PROC_MODE 0440
#define OSAL_PROC_PRINT_BUF_SIZE 256

static struct osal_list_head g_msp_list;
osal_proc_entry *g_msp_dir = NULL;

static struct osal_list_head g_hisi_list;
static osal_proc_entry *g_hisi_dir = NULL;

DEFINE_MUTEX(mutex);

static int osal_seq_show(struct seq_file *s, void *p)
{
    osal_proc_entry *entry = s->private;

    entry->seqfile = s;

    entry->read((void*)entry->seqfile, entry->private);
    return 0;
}

static void osal_proc_parse_para(const char *buf,
                                 char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                 unsigned int count,
                                 unsigned int *argc)
{
    int i = 0;
    int j = 0;
    int k = 0;

    for (i = 0; i < count; i++) {
        /* ignore SPACE */
        while (buf[i] == 0x20) {
            i++;
        }

        if (buf[i] == 0xa) {
            break;
        }

        if ((k >= (PROC_CMD_SINGEL_LENGTH_MAX - 1)) || (j >= (PROC_CMD_NUM_MAX - 1))) {
            return;
        }

        argv[j][k] = buf[i];
        k++;

        if ((buf[i + 1] == 0x20) || (buf[i + 1] == 0xa)) {
            argv[j][k] = '\0';
            k = 0;
            j++;
        }
    }

    *argc = j;
}

static int osal_proc_cmd_call(osal_proc_entry *entry, unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    unsigned int i = 0;

    for (i = 0; i < entry->cmd_cnt; i++) {
        if (osal_strncmp(entry->cmd_list[i].name, strlen(entry->cmd_list[i].name), argv[0], strlen(argv[0])) == 0) {
            if (entry->cmd_list[i].handler != NULL) {
                return entry->cmd_list[i].handler(argc, argv, entry->private);
            }
        }
    }

    return -1;
}

static ssize_t osal_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int ret = -1;
    unsigned int argc = 0;
    char proc_buf[PROC_CMD_ALL_LENGTH_MAX] = {0};
    char argv[PROC_CMD_NUM_MAX][PROC_CMD_SINGEL_LENGTH_MAX];
    osal_proc_entry *entry = NULL;

    if ((buf == NULL) || (file == NULL)) {
        return -1;
    }

    if ((count >= PROC_CMD_ALL_LENGTH_MAX) || (count == 0)) {
        printk("ERROR:count (%lu) is to large or Zero!\n", (unsigned long)count);
        return -1;
    }

    if (copy_from_user(proc_buf, buf, count)) {
        return -1;
    }

    proc_buf[PROC_CMD_ALL_LENGTH_MAX - 1] = '\0';

    (void)memset_s(&argv[0][0], PROC_CMD_NUM_MAX*PROC_CMD_SINGEL_LENGTH_MAX, 0,
        PROC_CMD_NUM_MAX*PROC_CMD_SINGEL_LENGTH_MAX);
    osal_proc_parse_para(proc_buf, &argv[0], count, &argc);
    if (argc == 0) {
        printk("ERROR:not vaild arg!\n");
        return -1;
    }

    entry = ((struct seq_file *)(file->private_data))->private;
    if (entry == NULL) {
        printk("ERROR:can not find entry!\n");
        return -1;
    }

    ret = osal_proc_cmd_call(entry, argc, argv);
    if (ret != 0) {
        printk("cmd is not find or param is wrong!\n");
        argc = 1;
        (void)strncpy_s(argv[0], PROC_CMD_SINGEL_LENGTH_MAX, "help", strlen("help"));
        (void)osal_proc_cmd_call(entry, argc, argv);
    }

    return count;
}

static int osal_proc_open(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
    osal_proc_entry *item = PDE(inode)->data;
#else
    osal_proc_entry *item = PDE_DATA(inode);
#endif

    if ((item != NULL) && (item->read != NULL)) {
        return single_open(file, osal_seq_show, item);
    }

    return -ENOSYS;
}


static struct file_operations g_osal_proc_ops = {
    .owner = THIS_MODULE,
    .open = osal_proc_open,
    .read = seq_read,
    .write = osal_proc_write,
    .llseek = seq_lseek,
    .release = single_release,
};


osal_proc_entry *osal_proc_find_node(const char *name, struct osal_list_head *list)
{
    osal_proc_entry *node = NULL;

    osal_list_for_each_entry(node, list, node) {
        if (osal_strncmp(node->name, strlen(node->name), name, strlen(name)) == 0) {
            return node;
        }
    }

    return NULL;
}

osal_proc_entry *osal_proc_create_dir(const char *name, osal_proc_entry *parent, struct osal_list_head *list)
{
    struct proc_dir_entry *proc = NULL;
    osal_proc_entry *dir_proc = NULL;
    errno_t err;

    if ((name == NULL) || (list == NULL)) {
        return NULL;
    }

    mutex_lock(&mutex);

    dir_proc = osal_proc_find_node(name, list);
    if (dir_proc != NULL) {
        osal_printk("%s - name repeat\n", __FUNCTION__);
        mutex_unlock(&mutex);
        return NULL;
    }

    dir_proc = kmalloc(sizeof(osal_proc_entry), GFP_KERNEL);
    if (dir_proc == NULL) {
        osal_printk("%s - kmalloc failed!\n", __FUNCTION__);
        mutex_unlock(&mutex);
        return NULL;
    }

    (void)memset_s(dir_proc, sizeof(osal_proc_entry), 0, sizeof(osal_proc_entry));

    err = strncpy_s(dir_proc->name, sizeof(dir_proc->name), name, strlen(name));
    if (err != EOK) {
        osal_printk("secure func call error\n");
    }

    if (parent != NULL) {
        proc = proc_mkdir(name, parent->proc_dir_entry);
    } else {
        proc = proc_mkdir(name, NULL);
    }
    if (proc == NULL) {
        osal_printk("%s - proc_mkdir failed!\n", __FUNCTION__);
        kfree(dir_proc);
        dir_proc = NULL;
        mutex_unlock(&mutex);
        return NULL;
    }
    dir_proc->proc_dir_entry = proc;

    osal_list_add_tail(&(dir_proc->node), list);

    mutex_unlock(&mutex);

    return dir_proc;
}


osal_proc_entry *osal_proc_create_entry(const char *name, osal_proc_entry *parent, struct osal_list_head *list)
{
    struct proc_dir_entry *entry = NULL;
    osal_proc_entry *sentry = NULL;
    errno_t err;

    if ((name == NULL) || (list == NULL)) {
        return NULL;
    }

    mutex_lock(&mutex);
    sentry = osal_proc_find_node(name, list);
    if (sentry != NULL) {
        osal_printk("%s - name repeat\n", __FUNCTION__);
        mutex_unlock(&mutex);
        return NULL;
    }

    sentry = kmalloc(sizeof(osal_proc_entry), GFP_KERNEL);
    if (sentry == NULL) {
        osal_printk("%s - kmalloc failed!\n", __FUNCTION__);
        mutex_unlock(&mutex);
        return NULL;
    }

    (void)memset_s(sentry, sizeof(osal_proc_entry), 0, sizeof(osal_proc_entry));

    err = strncpy_s(sentry->name, sizeof(sentry->name), name, strlen(name));
    if (err != EOK) {
        osal_printk("secure func call error\n");
        kfree(sentry);
        mutex_unlock(&mutex);
        return NULL;
    }

    if (parent == NULL) {
        entry = proc_create_data(name, OSAL_PROC_MODE, NULL, &g_osal_proc_ops, sentry);
    } else {
        entry = proc_create_data(name, OSAL_PROC_MODE, parent->proc_dir_entry, &g_osal_proc_ops, sentry);
    }

    if (entry == NULL) {
        osal_printk("%s - create_proc_entry failed!\n", __FUNCTION__);
        kfree(sentry);
        sentry = NULL;
        mutex_unlock(&mutex);
        return NULL;
    }

    sentry->proc_dir_entry = entry;

    osal_list_add_tail(&(sentry->node), list);

    mutex_unlock(&mutex);

    return sentry;
}

void osal_proc_destory_node(const char *name, osal_proc_entry *parent, struct osal_list_head *list)
{
    osal_proc_entry *dir_proc = NULL;

    if ((name == NULL) || (list == NULL)) {
        return;
    }

    mutex_lock(&mutex);

    dir_proc = osal_proc_find_node(name, list);
    if (dir_proc == NULL) {
        mutex_unlock(&mutex);
        return;
    }

    osal_list_del(&(dir_proc->node));

    if (parent != NULL) {
        remove_proc_entry(name, parent->proc_dir_entry);
    } else {
        remove_proc_entry(name, NULL);
    }

    kfree(dir_proc);

    mutex_unlock(&mutex);
}

void *osal_get_hisi_entry(void)
{
    if (g_hisi_dir != NULL) {
        return (void *)g_hisi_dir->proc_dir_entry;
    }

    return NULL;
}

void *osal_get_msp_entry(void)
{
    if (g_msp_dir != NULL) {
        return (void *)g_msp_dir->proc_dir_entry;
    }

    return NULL;
}


/* for user space */
osal_proc_entry *osal_proc_user_create_dir(const char *name)
{
    return osal_proc_create_dir(name, g_hisi_dir, &g_hisi_list);
}
EXPORT_SYMBOL(osal_proc_user_create_dir);

void osal_proc_user_destory_dir(const char *name)
{
    return osal_proc_destory_node(name, g_hisi_dir, &g_hisi_list);
}
EXPORT_SYMBOL(osal_proc_user_destory_dir);

osal_proc_entry *osal_proc_user_add(const char *name, osal_proc_entry *dir)
{
    return osal_proc_create_entry(name, dir, &g_hisi_list);
}
EXPORT_SYMBOL(osal_proc_user_add);

void osal_proc_user_remove(const char *name, osal_proc_entry *dir)
{
    return osal_proc_destory_node(name, g_hisi_dir, &g_hisi_list);
}
EXPORT_SYMBOL(osal_proc_user_remove);


int osal_proc_init(void)
{
    OSAL_INIT_LIST_HEAD(&g_msp_list);
    g_msp_dir = osal_proc_create_dir("msp", OSAL_NULL, &g_msp_list);
    if (g_msp_dir == OSAL_NULL) {
        osal_printk("proc msp mkdir error!\n");
    }

    OSAL_INIT_LIST_HEAD(&g_hisi_list);
    g_hisi_dir = osal_proc_create_dir("hisi", OSAL_NULL, &g_hisi_list);
    if (g_hisi_dir == OSAL_NULL) {
        osal_printk("proc hisi mkdir error!\n");
    }

    return 0;
}
EXPORT_SYMBOL(osal_proc_init);


void osal_proc_exit(void)
{
    osal_proc_destory_node("msp", OSAL_NULL, &g_msp_list);
    osal_proc_destory_node("hisi", OSAL_NULL, &g_hisi_list);
}
EXPORT_SYMBOL(osal_proc_exit);

osal_proc_entry *osal_proc_add(const char *name, unsigned long name_size)
{
    if ((name_size >= OSAL_PROC_NAME_LENGTH) || (strlen(name) > name_size)) {
        return NULL;
    }

    return osal_proc_create_entry(name, g_msp_dir, &g_msp_list);
}
EXPORT_SYMBOL(osal_proc_add);

void osal_proc_remove(const char *name, unsigned long name_size)
{
    if ((name_size >= OSAL_PROC_NAME_LENGTH) || (strlen(name) > name_size)) {
        return;
    }

    return osal_proc_destory_node(name, g_msp_dir, &g_msp_list);
}
EXPORT_SYMBOL(osal_proc_remove);

/*
 * echo string to current terminal display(serial console or tty).
 * this implement implicit that current task file handle '0' must be terminal device file.
 * otherwise do nothing.
 */
void osal_proc_print_vargs(char *buf, unsigned int size, const char *fmt, va_list args)
{
#if !(0 == HI_PROC_SUPPORT)

#define DEFAULT_ECHO_DEVICE_HANDLE (0)

    struct kstat stat;
    int ret;

    if (!buf || size == 0) {
        return;
    }

    ret = vfs_fstat(DEFAULT_ECHO_DEVICE_HANDLE, &stat);
    if (ret) {
        printk("default echo device handle(%u) invalid!\n", DEFAULT_ECHO_DEVICE_HANDLE);
        return;
    }

    ret = vsnprintf_s(buf, size, size - 1, fmt, args);
    if (ret < 0) {
        printk("snprintf_s failed\n");
        return;
    }

    /* echo device must be chrdev and major number must be TTYAUX_MAJOR or UNIX98_PTY_SLAVE_MAJOR */
    if (S_ISCHR(stat.mode) && (MAJOR(stat.rdev) == TTYAUX_MAJOR || MAJOR(stat.rdev) == UNIX98_PTY_SLAVE_MAJOR)) {
        struct file *file = fget(DEFAULT_ECHO_DEVICE_HANDLE);
        if (file) {
            mm_segment_t st_old_fs = get_fs();
            /* file pos is invalid for chrdev */
            loff_t pos = 0;

            set_fs(KERNEL_DS);
            ret = vfs_write(file, buf, strlen(buf), &pos);
            if (ret < 0) {
                printk("write to echo device failed(%d)!\n", ret);
            }
            set_fs(st_old_fs);

            fput(file);
        }
    } else {
        printk("default echo device is invalid!\n");
    }

#endif
}

int osal_proc_print(void *seqfile, const char *fmt, ...)
{
    va_list args;
    int r = 0;

    va_start(args, fmt);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 3, 0)
    r = seq_vprintf(seqfile, fmt, args);
#else
    seq_vprintf(seqfile, fmt, args);
#endif
    va_end(args);

    return r;
}
EXPORT_SYMBOL(osal_proc_print);

void osal_printk(const char *fmt, ...)
{
    va_list args;

    if (!fmt) {
        return;
    }

    va_start(args, fmt);
    vprintk(fmt, args);
    va_end(args);
}
EXPORT_SYMBOL(osal_printk);

/* use for echo device */
void osal_proc_echo(const char *fmt, ...)
{
    va_list args;
    char buf[OSAL_PROC_PRINT_BUF_SIZE];

    if (!fmt) {
        return;
    }

    va_start(args, fmt);
    osal_proc_print_vargs(buf, sizeof(buf), fmt, args);
    va_end(args);
}
EXPORT_SYMBOL(osal_proc_echo);

