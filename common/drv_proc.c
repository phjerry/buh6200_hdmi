/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2006-8-2
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <linux/file.h>
#include <uapi/linux/major.h>

#include "hi_type.h"

#include "hi_drv_module.h"
#include "drv_common_ioctl.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"

#define PROC_MAX_ENTRIES 256

struct proc_dir_entry *g_proc_cmpi = HI_NULL;
struct proc_dir_entry *g_proc_hisi = HI_NULL;
static hi_proc_item g_proc_items[PROC_MAX_ENTRIES] = {{{0}}};

static DEFINE_MUTEX(g_proc_mutex_lock);

/* for some ttyXXX device major number */
#ifndef PROC_SERIAL_MAJOR_NUM
#define PROC_SERIAL_MAJOR_NUM 204
#endif

static hi_proc_module_fn *g_proc_intf_para = HI_NULL;

hi_s32 hi_drv_proc_register_para(hi_proc_module_fn *para)
{
    if (para == HI_NULL) {
        return HI_FAILURE;
    }
    if ((para->add_module_fn == HI_NULL) || (para->remove_module_fn == HI_NULL)) {
        return HI_FAILURE;
    }
    g_proc_intf_para = para;
    return HI_SUCCESS;
}

hi_void hi_drv_proc_unregister_para(hi_void)
{
    g_proc_intf_para = HI_NULL;
    return;
}

hi_proc_item *hi_drv_proc_add_module(const hi_char *entry_name, hi_proc_fn_set *fn_set, hi_void *data)
{
    if (g_proc_intf_para) {
        if (g_proc_intf_para->add_module_fn) {
            return g_proc_intf_para->add_module_fn(entry_name, fn_set, data);
        }
    }
    return HI_NULL;
}

hi_void hi_drv_proc_remove_module(const hi_char *entry_name)
{
    if (g_proc_intf_para) {
        if (g_proc_intf_para->remove_module_fn) {
            g_proc_intf_para->remove_module_fn(entry_name);
        }
    }
    return;
}

#ifdef HI_PROC_SUPPORT
/*
 * echo string to current terminal display(serial console or tty).
 * this implement implicit that current task file handle '0' must be terminal device file.
 * otherwise do nothing.
 */
hi_void hi_drv_proc_echo_helper_vargs(hi_char *buf, hi_u32 size, const hi_char *fmt, va_list args)
{
#define PROC_DEFAULT_ECHO_DEVICE_HANDLE (0)

    struct kstat stat;
    hi_s32 ret;

    if ((buf == HI_NULL) || (size == 0)) {
        HI_PRINT("Invalid argument buf or size!\n");
        return;
    }

    ret = vfs_fstat(PROC_DEFAULT_ECHO_DEVICE_HANDLE, &stat);
    if (ret) {
        HI_PRINT("Default echo device handle(%u) invalid!\n", PROC_DEFAULT_ECHO_DEVICE_HANDLE);
        return;
    }

    vsnprintf(buf, size, fmt, args);

    /*
     * echo device must be chrdev and major number must be PROC_SERIAL_MAJOR_NUM or
     * TTYAUX_MAJOR or UNIX98_PTY_SLAVE_MAJOR
     */
    if (S_ISCHR(stat.mode) && (MAJOR(stat.rdev) == PROC_SERIAL_MAJOR_NUM || MAJOR(stat.rdev) == TTYAUX_MAJOR ||
                               MAJOR(stat.rdev) == UNIX98_PTY_SLAVE_MAJOR)) {
        struct file *file = fget(PROC_DEFAULT_ECHO_DEVICE_HANDLE);
        if (file != HI_NULL) {
            mm_segment_t old_fs = get_fs();
            loff_t pos = 0; /* file pos is invalid for chrdev */

            set_fs(KERNEL_DS);
            ret = vfs_write(file, buf, strlen(buf), &pos);
            if (ret < 0) {
                HI_PRINT("write to echo device failed(%d)!\n", ret);
            }
            set_fs(old_fs);

            fput(file);
        }
    } else {
        HI_PRINT("Default echo device is invalid!\n");
    }
}
EXPORT_SYMBOL(hi_drv_proc_echo_helper_vargs);

/* general echo helper function */
hi_void hi_drv_proc_echo_helper(const hi_char *fmt, ...)
{
    va_list args;
    hi_char *buf = HI_NULL;

    if (fmt == HI_NULL) {
        HI_PRINT("Invalid argument fmt!\n");
        return;
    }
    buf = HI_KZALLOC(HI_ID_PROC, HI_USER_PROC_BUF_SIZE, GFP_KERNEL);
    if (buf == HI_NULL) {
        HI_PRINT("Memory allocate failed for proc\n");
        return;
    }

    va_start(args, fmt);
    hi_drv_proc_echo_helper_vargs(buf, HI_USER_PROC_BUF_SIZE, fmt, args);
    va_end(args);

    HI_KFREE(HI_ID_PROC, buf);
    buf = HI_NULL;
}
EXPORT_SYMBOL(hi_drv_proc_echo_helper);
#endif

static hi_s32 drv_proc_cmpi_open(struct inode *inode, struct file *file)
{
    hi_proc_item *item = PDE_DATA(inode);

    if (item && item->read) {
        return single_open(file, item->read, item);
    }

    return -ENOSYS;
}

static ssize_t drv_proc_cmpi_write(struct file *file, const hi_char __user *buf, size_t count, loff_t *pos)
{
#ifdef HI_PROC_WRITE_SUPPORT
    struct seq_file *s = file->private_data;
    hi_proc_item *item = s->private;

    if (item->write) {
        return item->write(file, buf, count, pos);
    }
#endif
    return -ENOSYS;
}

static hi_slong drv_proc_cmpi_unlocked_ioctl(struct file *file, hi_u32 cmd, hi_ulong arg)
{
    struct seq_file *s = file->private_data;
    hi_proc_item *item = s->private;

    if (item->ioctl) {
        return item->ioctl(s, cmd, arg);
    }

    return 0;
}

#ifdef CONFIG_COMPAT
static hi_slong drv_proc_cmpi_compat_ioctl(struct file *file, hi_u32 cmd, hi_ulong arg)
{
    return -ENOSYS;
}
#endif

static struct file_operations g_proc_cmpi_ops __attribute__((unused)) = {
    .owner          = THIS_MODULE,
    .open           = drv_proc_cmpi_open,
    .read           = seq_read,
    .write          = drv_proc_cmpi_write,
    .llseek         = seq_lseek,
    .release        = single_release,
    .unlocked_ioctl = drv_proc_cmpi_unlocked_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl   = drv_proc_cmpi_compat_ioctl,
#endif
};

static hi_bool drv_proc_is_hex(hi_char *p)
{
    hi_s32 i, len;

    len = strlen(p);
    if (len <= 0x2) {
        return HI_FALSE;
    }

    p += 0x2;
    for (i = 0; i < len - 0x2; i++) {
        if ((p[i] < '0') || p[i] > 'f') {
            return HI_FALSE;
        }

        if ((p[i] > '9') && p[i] < 'a') {
            return HI_FALSE;
        }
    }
    return HI_TRUE;
}

static hi_s32 drv_proc_str_to_digit(unsigned char *str, hi_u32 *digit)
{
    int i, len;
    hi_u32 val = 0;

    len = strlen(str);
    if (len <= 0x2) {
        return HI_FAILURE;
    }

    str += 0x2;
    for (i = 0; i < len - 0x2; i++) {
        val <<= 0x4;
        if (str[i] <= '9') {
            val += str[i] - '0';
        } else {
            val += str[i] - 'a' + 0x0a;
        }
    }
    *digit = val;

    return HI_SUCCESS;
}

hi_s32 drv_proc_cmpi_parse_cmd(char *ptr, hi_u32 *para1, hi_u32 *para2)
{
    hi_s32 i;
    hi_char the_args[2][64]; /* row 64, col 2 */

    while (*ptr == ' ' && *ptr++ != '\0') {
    }

    /* covert into lowercase string */
    for (i = 0; i < strlen(ptr); i++) {
        ptr[i] = tolower(ptr[i]);
    }

    for (i = strlen(ptr); i > 0; i--) {
        if ((*(ptr + i - 1) < '0') || (*(ptr + i - 1) > 'f')) {
            *(ptr + i - 1) = '\0';
        } else if ((*(ptr + i - 1) > '9') && (*(ptr + i - 1) < 'a')) {
            *(ptr + i - 1) = '\0';
        } else {
            break;
        }
    }

    for (i = 0; i < 0x2; i++) {
        hi_s32 j = 0;
        while (*ptr == ' ' && *ptr++ != '\0');
        while ((*ptr != ' ') && (*ptr != '\0')) {
            the_args[i][j++] = *ptr++;
        }

        the_args[i][j] = '\0';
        if (*ptr == '\0') {
            i++;
            break;
        }
    }

    if ((the_args[0][0] != '0') || (the_args[0][1] != 'x') ||
        (the_args[1][0] != '0') || (the_args[1][1] != 'x')) {
        return -1;
    }

    if ((!drv_proc_is_hex(the_args[0])) || (!drv_proc_is_hex(the_args[1]))) {
        return -1;
    }

    if (drv_proc_str_to_digit(the_args[0], para1) != HI_SUCCESS) {
        return -1;
    }

    if (drv_proc_str_to_digit(the_args[1], para2) != HI_SUCCESS) {
        return -1;
    }

    return i;
}

static hi_void drv_proc_write_show_help(hi_void)
{
    hi_drv_proc_echo_helper("\nPLS type \"echo 0xxxxxxxxx 0xxxxxxxxx > /proc/msp/demux\"\n");
    hi_drv_proc_echo_helper("E.g.: \"echo 0x40002 0x4010 > /proc/msp/demux\"\n");
}

ssize_t hi_drv_proc_module_write(struct file *file, const char __user *buf, size_t count,
    loff_t *pos, hi_proc_ctrl_fn ctrl_fn)
{
    char tmp_buf[64] = {0}; /* array num is 64 */
    char *tmp_buf_ptr = tmp_buf;
    hi_u32 para1, para2;

    if ((buf == HI_NULL) || (count >= sizeof(tmp_buf))) {
        drv_proc_write_show_help();
        goto out;
    }

    if (copy_from_user(tmp_buf_ptr, buf, count)) {
        return -EFAULT;
    }

    tmp_buf[count > 1 ? count - 1 : 0] = '\0' ;

    if (drv_proc_cmpi_parse_cmd(tmp_buf, &para1, &para2) > 0) {
        ctrl_fn(para1, para2);
    } else {
        drv_proc_write_show_help();
    }

out:
    *pos = count;
    return count;
}

static hi_proc_item *drv_proc_add_module(const hi_char *entry_name, hi_proc_fn_set *fn_set, hi_void *data)
{
    struct proc_dir_entry *entry = HI_NULL;
    hi_s32 i;

    if ((entry_name == HI_NULL) || (strlen(entry_name) > HI_PROC_MAX_ENTRY_NAME_LEN)) {
        return HI_NULL;
    }

    mutex_lock(&g_proc_mutex_lock);
    for (i = 0; i < PROC_MAX_ENTRIES; i++) {
        if (!g_proc_items[i].entry) {
            break;
        }
    }

    if (i == PROC_MAX_ENTRIES) {
        mutex_unlock(&g_proc_mutex_lock);
        HI_PRINT("ERROR: add proc entry %s over LIMIT:%#x\n", entry_name, PROC_MAX_ENTRIES);
        return HI_NULL;
    }

    strncpy(g_proc_items[i].entry_name, entry_name, sizeof(g_proc_items[i].entry_name) - 1);

    if (fn_set != HI_NULL) {
        g_proc_items[i].read  = fn_set->read;
        g_proc_items[i].write = fn_set->write;
        g_proc_items[i].ioctl = fn_set->ioctl;
    } else {
        g_proc_items[i].read  = HI_NULL;
        g_proc_items[i].write = HI_NULL;
        g_proc_items[i].ioctl = HI_NULL;
    }

    g_proc_items[i].data = data;

    entry = proc_create_data(entry_name, 0, g_proc_cmpi, &g_proc_cmpi_ops, &g_proc_items[i]);
    if (!entry) {
        mutex_unlock(&g_proc_mutex_lock);
        return HI_NULL;
    }

    g_proc_items[i].entry = entry;

    mutex_unlock(&g_proc_mutex_lock);

    return &g_proc_items[i];
}

static hi_void drv_proc_remove_module(const hi_char *entry_name)
{
    hi_s32 i;

    mutex_lock(&g_proc_mutex_lock);
    for (i = 0; i < PROC_MAX_ENTRIES; i++) {
        if (!strncmp(g_proc_items[i].entry_name, entry_name, sizeof(g_proc_items[i].entry_name))) {
            break;
        }
    }

    if (i == PROC_MAX_ENTRIES) {
        mutex_unlock(&g_proc_mutex_lock);
        HI_PRINT("Not find the entry:%s\n", entry_name);
        return;
    }
    remove_proc_entry(g_proc_items[i].entry_name, g_proc_cmpi);
    g_proc_items[i].entry = HI_NULL;

    mutex_unlock(&g_proc_mutex_lock);
}

static hi_proc_module_fn g_proc_para = {
    .add_module_fn = drv_proc_add_module,
    .remove_module_fn = drv_proc_remove_module,
};

hi_s32 drv_proc_init(hi_void)
{
    if (hi_drv_proc_register_para(&g_proc_para) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    g_proc_hisi = proc_mkdir("hisin", HI_NULL);
    g_proc_cmpi = proc_mkdir("msp", g_proc_hisi);

    proc_mkdir("graphics", g_proc_hisi);
    proc_symlink("mspn", HI_NULL, "hisin/msp");
    proc_symlink("graphics", HI_NULL, "hisin/graphics");

    return HI_SUCCESS;
}

hi_void drv_proc_exit(hi_void)
{
    hi_drv_proc_unregister_para();

    remove_proc_entry("mspn", HI_NULL);
    remove_proc_entry("graphics", HI_NULL);
    remove_proc_entry("msp", g_proc_hisi);
    remove_proc_entry("graphics", g_proc_hisi);
    remove_proc_entry("hisin", HI_NULL);

    return;
}

EXPORT_SYMBOL(hi_drv_proc_module_write);
EXPORT_SYMBOL(hi_drv_proc_register_para);
EXPORT_SYMBOL(hi_drv_proc_unregister_para);
EXPORT_SYMBOL(hi_drv_proc_add_module);
EXPORT_SYMBOL(hi_drv_proc_remove_module);

