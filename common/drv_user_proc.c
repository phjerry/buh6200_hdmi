/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: user proc function.
 * Author: guoqingbo
 * Create: 2013-7-1
 */

#include <linux/rbtree.h>
#include "../../fs/proc/internal.h"

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_osal.h"

#include "drv_user_proc.h"

#include "drv_common_ioctl.h"
#include "hi_drv_dev.h"
#include "hi_drv_mem.h"

#define USER_PROC_K_LOCK(sema)   osal_sem_down(&(sema))
#define USER_PROC_K_UNLOCK(sema) osal_sem_up(&(sema))

typedef struct {
    hi_u32                  entry_name_hash;
    struct rb_node          node;
    struct proc_dir_entry  *parent;
    hi_user_proc_entry_info entry_info;
    hi_char                 entry_name[HI_USER_PROC_MAX_NAME_LEN + 1];
} user_proc_entry;

typedef struct {
    hi_u32                 dir_name_hash;
    struct rb_node         node;
    struct rb_root         entry_root;
    hi_void               *file;
    struct proc_dir_entry *entry;
    struct proc_dir_entry *parent;
    hi_char                dir_name[HI_USER_PROC_MAX_NAME_LEN + 12]; /* name size is HI_USER_PROC_MAX_NAME_LEN + 12 */
} user_proc_dir;

typedef struct {
    osal_semaphore        sem_general;
    osal_semaphore        sem_dir_entry;
    osal_semaphore        sem_read_write;
    struct rb_root        root;
    wait_queue_head_t     wq_for_read;
    wait_queue_head_t     wq_for_write;
    hi_s32                busy;
    hi_user_proc_cmd_info current_cmd;
    osal_atomic           open_cnt;
} user_proc_k_para;

static user_proc_k_para g_user_proc_k_para = {
    .root     = RB_ROOT,
};

void *osal_get_hisi_entry(void);
void *osal_get_msp_entry(void);

static user_proc_dir *g_hisi_dirent = HI_NULL;
static user_proc_dir *g_msp_dirent = HI_NULL;

static hi_void drv_user_proc_remove_dir_forcibly(user_proc_dir *dir);
static hi_void drv_user_proc_remove_entry(user_proc_dir *dir, user_proc_entry *entry);

extern unsigned int __pure full_name_hash(const hi_void *salt, const char *, unsigned int);

static inline hi_s32 drv_user_proc_full_name_hash(const unsigned char *name, unsigned int len)
{
    return full_name_hash(HI_NULL, name, strlen(name)) & 0x7fffffffU;
}

static user_proc_dir *drv_user_proc_rb_find_dir(user_proc_dir *parent, const hi_char *name)
{
    hi_u32 hash = drv_user_proc_full_name_hash(name, strlen(name));
    struct rb_node *node = g_user_proc_k_para.root.rb_node;

    while (node != HI_NULL) {
        hi_s32 result;
        user_proc_dir *this = rb_entry(node, user_proc_dir, node);

        if (hash != this->dir_name_hash) {
            result = hash - this->dir_name_hash;
        } else {
            result = strncmp(name, this->dir_name, sizeof(this->dir_name));
        }

        if (result < 0) {
            node = node->rb_left;
        } else if (result > 0) {
            node = node->rb_right;
        } else {
            return this;
        }
    }

    return HI_NULL;
}

static hi_s32 drv_user_proc_rb_insert_dir(user_proc_dir *parent_dir, user_proc_dir *dir)
{
    struct rb_root *root = &g_user_proc_k_para.root;
    struct rb_node **new = &(root->rb_node);
    struct rb_node *parent = HI_NULL;

    while (*new) {
        hi_s32 result;
        user_proc_dir *this = rb_entry(*new, user_proc_dir, node);
        parent = *new;

        if (dir->dir_name_hash != this->dir_name_hash) {
            result = dir->dir_name_hash - this->dir_name_hash;
        } else {
            result = strncmp(dir->dir_name, this->dir_name, sizeof(dir->dir_name));
        }

        if (result < 0) {
            new = &((*new)->rb_left);
        } else if (result > 0) {
            new = &((*new)->rb_right);
        } else {
            HI_ERR_UPROC("dirent(%s) has existed.", dir->dir_name);
            return HI_FAILURE;
        }
    }

    rb_link_node(&dir->node, parent, new);
    rb_insert_color(&dir->node, root);

    return HI_SUCCESS;
}

static hi_void drv_user_proc_rb_erase_dir(user_proc_dir *parent, user_proc_dir *dir)
{
    struct rb_root *root = &g_user_proc_k_para.root;

    rb_erase(&(dir->node), root);
}

static user_proc_entry *drv_user_proc_rb_find_entry(user_proc_dir *dir, const hi_char *name)
{
    hi_u32 hash = drv_user_proc_full_name_hash(name, strlen(name));
    struct rb_node *node = dir->entry_root.rb_node;

    while (node != HI_NULL) {
        hi_s32 result;
        user_proc_entry *this = rb_entry(node, user_proc_entry, node);

        if (hash != this->entry_name_hash) {
            result = hash - this->entry_name_hash;
        } else {
            result = strncmp(name, this->entry_name, sizeof(this->entry_name));
        }

        if (result < 0) {
            node = node->rb_left;
        } else if (result > 0) {
            node = node->rb_right;
        } else {
            return this;
        }
    }

    return HI_NULL;
}

static hi_s32 drv_user_proc_rb_insert_entry(user_proc_dir *dir, user_proc_entry *entry)
{
    struct rb_root *root = &(dir->entry_root);
    struct rb_node **new = &(root->rb_node);
    struct rb_node *parent = NULL;

    while (*new != HI_NULL) {
        hi_s32 result;
        user_proc_entry *this = rb_entry(*new, user_proc_entry, node);
        parent = *new;

        if (entry->entry_name_hash != this->entry_name_hash) {
            result = entry->entry_name_hash - this->entry_name_hash;
        } else {
            result = strncmp(entry->entry_name, this->entry_name, sizeof(entry->entry_name));
        }

        if (result < 0) {
            new = &((*new)->rb_left);
        } else if (result > 0) {
            new = &((*new)->rb_right);
        } else {
            HI_ERR_UPROC("entry(%s) has existed.", entry->entry_name);
            return HI_FAILURE;
        }
    }

    rb_link_node(&entry->node, parent, new);
    rb_insert_color(&entry->node, root);

    return HI_SUCCESS;
}

static hi_void drv_user_proc_rb_erase_entry(user_proc_dir *dir, user_proc_entry *entry)
{
    struct rb_root *root = &(dir->entry_root);

    rb_erase(&(entry->node), root);
}

static user_proc_entry *drv_user_proc_rb_find_proc_entry(struct proc_dir_entry *proc_entry)
{
    user_proc_dir *dir = HI_NULL;
    user_proc_entry *entry = HI_NULL;

    if (!proc_entry || !strlen(proc_entry->parent->name) || !strlen(proc_entry->name)) {
        HI_ERR_UPROC("invalid proc entry.");
        goto out;
    }

    dir = drv_user_proc_rb_find_dir(HI_NULL, proc_entry->parent->name);
    if (dir == HI_NULL) {
        HI_ERR_UPROC("Can't find dirent:%p\n", proc_entry->parent->name);
        goto out;
    }

    entry = drv_user_proc_rb_find_entry(dir, proc_entry->name);
    if (entry == HI_NULL) {
        HI_ERR_UPROC("Can't find entry:%p\n", proc_entry->name);
        goto out;
    }

    return entry;

out:
    return HI_NULL;
}

hi_s32 drv_user_proc_open(hi_void *file)
{
    HI_INFO_UPROC("Open User Mode Proc:%s,%d\n", current->comm, current->pid);

    if (osal_atomic_inc_return(&g_user_proc_k_para.open_cnt) == 1) {
        memset(&g_user_proc_k_para.current_cmd, 0, sizeof(hi_user_proc_cmd_info));
        init_waitqueue_head(&g_user_proc_k_para.wq_for_read);
        init_waitqueue_head(&g_user_proc_k_para.wq_for_write);
        g_user_proc_k_para.busy = 0;
    }

    *((user_proc_k_para **)file) = &g_user_proc_k_para;
    return 0;
}

hi_s32 drv_user_proc_close(hi_void *file)
{
    struct rb_node *node = HI_NULL;

    if (osal_atomic_dec_return(&g_user_proc_k_para.open_cnt) < 0) {
        goto out;
    }

scratch_dirent:
    for (node = rb_first(&(g_user_proc_k_para.root)); node; node = rb_next(node)) {
        user_proc_dir *dirent = rb_entry(node, user_proc_dir, node);
        if (dirent->file == file) {
            drv_user_proc_remove_dir_forcibly(dirent);
            goto scratch_dirent;
        } else if (dirent->file == HI_NULL) { /* for files /proc/msp/xxx or /proc/hisi/xxx */
            struct rb_node *entry_node = HI_NULL;

scratch_entry:
            for (entry_node = rb_first(&(dirent->entry_root)); entry_node; entry_node = rb_next(entry_node)) {
                user_proc_entry *entry = rb_entry(entry_node, user_proc_entry, node);
                if (entry->entry_info.file == file) {
                    drv_user_proc_remove_entry(dirent, entry);
                    goto scratch_entry;
                }
            }
        }
    }

out:
    HI_INFO_UPROC("Close User Mode Proc\n");
    return 0;
}

static hi_s32 drv_user_proc_seq_show(struct seq_file *m, hi_void *unused)
{
    hi_s32 ret;
    struct proc_dir_entry *proc_entry = (struct proc_dir_entry *)(m->private);
    user_proc_k_para *proc = ((struct proc_dir_entry *)(m->private))->data;
    user_proc_entry *entry = HI_NULL;
    DEFINE_WAIT(wait);

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    entry = drv_user_proc_rb_find_proc_entry(proc_entry);
    if (entry == HI_NULL) {
        HI_ERR_UPROC("Can't find entry:%p\n", proc_entry->name);
        ret = -1;
        goto out;
    } else if (entry->entry_info.show_func == HI_NULL) {
        HI_ERR_UPROC("Entry don't support read.\n");
        ret = -1;
        goto out;
    }

    proc->current_cmd.entry = &(entry->entry_info);
    strncpy(proc->current_cmd.cmd, HI_USER_PROC_READ_CMD, sizeof(proc->current_cmd.cmd) - 1);

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    /* Wait write data over */
    prepare_to_wait(&proc->wq_for_read, &wait, TASK_INTERRUPTIBLE);
    schedule();
    finish_wait(&proc->wq_for_read, &wait);

    /* Find it again, pstEntry may be removed when wait event */
    entry = drv_user_proc_rb_find_proc_entry(proc_entry);
    if (entry == HI_NULL) {
        HI_ERR_UPROC("Can't find entry:%p\n", proc_entry->name);
        ret = -1;
        return ret;
    }

    if (entry->entry_info.show_func != HI_NULL) {
        HI_INFO_UPROC("User Mode Proc Show entry=0x%p, proc=0x%p\n", proc_entry, proc);

        USER_PROC_K_LOCK(g_user_proc_k_para.sem_read_write);
        if (entry->entry_info.read != HI_NULL) {
            osal_proc_print(m, "%s", (hi_char *)entry->entry_info.read);
            HI_KFREE(HI_ID_PROC, entry->entry_info.read);
            entry->entry_info.read = HI_NULL;
        }
        USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);
    proc->current_cmd.entry = HI_NULL;
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    ret = 0;
    return ret;

out:
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return ret;
}

static hi_s32 drv_user_proc_seq_open(struct inode *inode, struct file *file)
{
    hi_s32 ret;
    user_proc_k_para *proc = PDE(inode)->data;

    HI_INFO_UPROC("drv_user_proc_seq_open 0x%p,%d\n", proc, proc->busy);

    if (proc->busy) {
        return -EAGAIN;
    }

    proc->busy = 1;
    ret = single_open(file, drv_user_proc_seq_show, PDE(inode));
    if (ret) {
        proc->busy = 0;
    }

    return ret;
}

static hi_s32 drv_user_proc_seq_release(struct inode *inode, struct file *file)
{
    user_proc_k_para *proc = PDE(inode)->data;

    HI_INFO_UPROC("drv_user_proc_seq_release %d\n", proc->busy);

    proc->busy = 0;
    return single_release(inode, file);
}

static hi_s32 drv_user_proc_strip_string(hi_char *string, hi_u32 size)
{
    hi_char *p = string;
    hi_u32 index = 0;

    if ((string == HI_NULL) || (size == 0)) {
        return HI_FAILURE;
    }

    /* strip '\n' as string end character */
    for (; index < size; index++) {
        if (*(p + index) == '\n') {
            *(p + index) = '\0';
        }
    }

    if (strlen(string)) {
        return HI_SUCCESS;
    } else {
        return HI_FAILURE;
    }
}

static ssize_t drv_user_proc_seq_write(struct file *file, const char __user *buf, size_t size, loff_t *pos)
{
    hi_s32 ret;
    struct proc_dir_entry *proc_entry = PDE(file->f_path.dentry->d_inode);
    user_proc_k_para *proc = proc_entry->data;
    user_proc_entry *entry = HI_NULL;
    DEFINE_WAIT(wait);

    if (buf == HI_NULL) {
        HI_ERR_UPROC("buf is null.\n");
        ret = HI_FAILURE;
        return ret;
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    entry = drv_user_proc_rb_find_proc_entry(proc_entry);
    if ((entry == HI_NULL) || (size > sizeof(proc->current_cmd.cmd))) {
        HI_ERR_UPROC("Can't find entry:%p\n", proc_entry->name);
        ret = HI_FAILURE;
        goto out;
    } else if (entry->entry_info.cmd_func == HI_NULL) {
        HI_ERR_UPROC("Entry don't support write.\n");
        ret = HI_FAILURE;
        goto out;
    }

    memset(proc->current_cmd.cmd, 0, sizeof(proc->current_cmd.cmd));

    if (osal_copy_from_user(proc->current_cmd.cmd, buf, size)) {
        HI_ERR_UPROC("get cmd failed.\n");
        ret = -EIO;
        goto out;
    }
    proc->current_cmd.cmd[size > 1 ? size - 1 : 0] = '\0';

    if (drv_user_proc_strip_string(proc->current_cmd.cmd, size) == HI_FAILURE) {
        HI_WARN_UPROC("echo string invalid.\n");
        ret = -EINVAL;
        goto out;
    }

    proc->current_cmd.entry = &(entry->entry_info);

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    /* Wait write data over */
    prepare_to_wait(&proc->wq_for_write, &wait, TASK_INTERRUPTIBLE);
    schedule();
    finish_wait(&proc->wq_for_write, &wait);

    /* if buffer not empty, try echo to current terminal */
    entry = drv_user_proc_rb_find_proc_entry(proc_entry);
    if (entry != HI_NULL && entry->entry_info.cmd_func) {
        HI_INFO_UPROC("drv_user_proc_seq_write: proc=%p, entry=%p %d bytes\n", proc, proc_entry, size);

        USER_PROC_K_LOCK(g_user_proc_k_para.sem_read_write);
        if (entry->entry_info.write != HI_NULL) {
            if (strlen((hi_char *)entry->entry_info.write)) {
                osal_printk((hi_char *)entry->entry_info.write);
            }

            HI_KFREE(HI_ID_PROC, entry->entry_info.write);
            entry->entry_info.write = HI_NULL;
        }
        USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);
    proc->current_cmd.entry = HI_NULL;
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    return size;

out:
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return ret;
}

user_proc_dir *drv_user_proc_add_dir(const hi_char *name, hi_user_proc_parent_dir dir_position,
    hi_void *file, hi_bool *is_exist)
{
    user_proc_dir *dir = HI_NULL;
    struct proc_dir_entry *parent = HI_NULL;

    if ((name == HI_NULL) || (is_exist == HI_NULL) || (strlen(name) == 0) ||
        (strlen(name) > HI_USER_PROC_MAX_NAME_LEN)) {
        HI_ERR_UPROC("Invalid name\n");
        return HI_NULL;
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    /* Find directory node, if exist, return success directlly */
    dir = drv_user_proc_rb_find_dir(HI_NULL, name);
    if (dir != HI_NULL) {
        HI_INFO_UPROC("Dir %s exist\n", name);
        *is_exist = HI_TRUE;
        goto out0;
    }

    /* Determine parent directory */
    if (dir_position == HI_USER_PROC_PARENT_DEFAULT) {
        parent = (struct proc_dir_entry *)osal_get_hisi_entry();
    } else {
        parent = (struct proc_dir_entry *)osal_get_msp_entry();
    }

    /* Alloc directory resource */
    dir = HI_KZALLOC(HI_ID_PROC, sizeof(user_proc_dir), GFP_KERNEL);
    if (dir == HI_NULL) {
        HI_ERR_UPROC("kmalloc fail\n");
        goto out0;
    }

    /* Init directory parameter */
    snprintf(dir->dir_name, sizeof(dir->dir_name), "%s", name);
    dir->dir_name_hash = drv_user_proc_full_name_hash(name, strlen(name));
    dir->entry_root    = RB_ROOT;
    dir->parent        = parent;
    dir->file          = file;

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    dir->entry = proc_mkdir(dir->dir_name, dir->parent);

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);
    if (dir->entry == HI_NULL) {
        HI_ERR_UPROC("proc_mkdir fail\n");
        goto out1;
    }

    HI_INFO_UPROC("Proc add dir %s, file=%p, entry=0x%p\n", dir->dir_name, file, dir->entry);

    /* Add directory to rbtree */
    if (drv_user_proc_rb_insert_dir(HI_NULL, dir) != HI_SUCCESS) {
        HI_ERR_UPROC("Insert new dirent failed.\n");
        goto out2;
    }

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return dir;

out2:
    remove_proc_entry(dir->dir_name, dir->parent);

out1:
    HI_KFREE(HI_ID_PROC, dir);
    dir = HI_NULL;

out0:
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_NULL;
}

hi_s32 drv_user_proc_remove_dir(user_proc_dir *dir)
{
    if (dir == HI_NULL) {
        HI_ERR_UPROC("Invalid name\n");
        return HI_FAILURE;
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    /* If there are entries in this directory, remove fail */
    if (dir->entry_root.rb_node) {
        HI_ERR_UPROC("dir %s non-null\n", dir->dir_name);
        goto out;
    }
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    remove_proc_entry(dir->dir_name, dir->parent);
    HI_INFO_UPROC("Proc remove dir %s\n", dir->dir_name);

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    /* Remove directory from rbtree */
    drv_user_proc_rb_erase_dir(HI_NULL, dir);

    HI_KFREE(HI_ID_PROC, dir);

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_SUCCESS;

out:
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_FAILURE;
}

hi_s32 drv_user_proc_remove_dir_by_name(const hi_char *name, hi_user_proc_parent_dir dir_position)
{
    hi_char dir_name[HI_USER_PROC_MAX_NAME_LEN + 12]; /* name size is HI_USER_PROC_MAX_NAME_LEN + 12 */
    user_proc_dir *dir = HI_NULL;
    struct proc_dir_entry *parent = HI_NULL;

    if ((name == HI_NULL) || (strlen(name) == 0) || (strlen(name) > HI_USER_PROC_MAX_NAME_LEN)) {
        HI_ERR_UPROC("Invalid name\n");
        return HI_FAILURE;
    }

    if (dir_position == HI_USER_PROC_PARENT_DEFAULT) {
        parent = (struct proc_dir_entry *)osal_get_hisi_entry();;
    } else {
        parent = (struct proc_dir_entry *)osal_get_msp_entry();
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    snprintf(dir_name, sizeof(dir_name), "%s", name);

    /* Find directory node */
    dir = drv_user_proc_rb_find_dir(HI_NULL, dir_name);
    if (dir == HI_NULL || parent != dir->parent) {
        HI_ERR_UPROC("Find dir %s fail\n", dir_name);
        goto out;
    }

    /* If there are entries in this directory, remove fail */
    if (dir->entry_root.rb_node) {
        HI_ERR_UPROC("dir %s non-null\n", dir->dir_name);
        goto out;
    }
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    remove_proc_entry(dir->dir_name, dir->parent);
    HI_INFO_UPROC("Proc remove dir %s\n", dir->dir_name);

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    /* Remove directory from rbtree */
    drv_user_proc_rb_erase_dir(HI_NULL, dir);

    HI_KFREE(HI_ID_PROC, dir);

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_SUCCESS;

out:
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_FAILURE;
}

user_proc_dir *drv_user_proc_add_private_dir(const hi_char *name, struct proc_dir_entry *proc_entry)
{
    user_proc_dir *dir = HI_NULL;

    if ((name == HI_NULL) || (strlen(name) > HI_USER_PROC_MAX_NAME_LEN) || (proc_entry == HI_NULL)) {
        goto out;
    }

    /* Alloc directory resource */
    dir = HI_KZALLOC(HI_ID_PROC, sizeof(user_proc_dir), GFP_KERNEL);
    if (dir == HI_NULL) {
        HI_ERR_UPROC("kmalloc fail\n");
        goto out;
    }

    /* Init other parameter */
    strncpy(dir->dir_name, name, sizeof(dir->dir_name) - 1);

    dir->dir_name_hash = drv_user_proc_full_name_hash(name, strlen(name));
    dir->entry_root    = RB_ROOT;
    dir->entry         = proc_entry;
    dir->parent        = HI_NULL;
    dir->file          = HI_NULL;

    /* Add directory to rbtree */
    if (drv_user_proc_rb_insert_dir(HI_NULL, dir) != HI_SUCCESS) {
        HI_ERR_UPROC("Insert new dirent failed.\n");
        goto out1;
    }

    return dir;

out1:
    HI_KFREE(HI_ID_PROC, dir);
    dir = HI_NULL;

out:
    return HI_NULL;
}

hi_s32 drv_user_proc_remove_private_dir(const hi_char *name)
{
    user_proc_dir *dir = HI_NULL;

    if (name == HI_NULL) {
        return HI_FAILURE;
    }

    /* Find directory node */
    dir = drv_user_proc_rb_find_dir(HI_NULL, name);
    if (dir == HI_NULL) {
        HI_ERR_UPROC("Find dir %s fail\n", name);
        return HI_FAILURE;
    }

    /* Remove directory from rbtree */
    drv_user_proc_rb_erase_dir(HI_NULL, dir);

    HI_KFREE(HI_ID_PROC, dir);

    return HI_SUCCESS;
}

static struct file_operations g_user_proc_seq_fops;

user_proc_entry *drv_user_proc_add_entry(const hi_user_proc_entry_info *entry_info, hi_bool *is_exist)
{
    user_proc_entry *entry = HI_NULL;
    user_proc_dir *dir = HI_NULL;
    hi_char dir_name[HI_USER_PROC_MAX_NAME_LEN + 12]; /* name size is HI_USER_PROC_MAX_NAME_LEN + 12 */
    hi_u32 entry_name_len;

    if (entry_info == HI_NULL || is_exist == HI_NULL) {
        return HI_NULL;
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);
    entry_name_len = strlen(entry_info->name);
    if ((entry_name_len == 0) || (entry_name_len > HI_USER_PROC_MAX_NAME_LEN)) {
        HI_ERR_UPROC("Invalid name\n");
        goto out0;
    }

    if (strlen(entry_info->parent_dir) == 0) {
        strlcpy(dir_name, "hisi", sizeof(dir_name) - 1);
    } else {
        snprintf(dir_name, sizeof(dir_name), "%s", entry_info->parent_dir);
    }

    /* Find directory node, if don't exist, return fail */
    dir = drv_user_proc_rb_find_dir(HI_NULL, dir_name);
    if (dir == HI_NULL) {
        HI_ERR_UPROC("Dir %s don't exist\n", entry_info->parent_dir);
        goto out0;
    }

    /* Find entry in the directory, if exist, return success directlly */
    entry = drv_user_proc_rb_find_entry(dir, entry_info->name);
    if (entry != HI_NULL) {
        HI_INFO_UPROC("Entry %s exist\n", entry_info->name);
        *is_exist = HI_TRUE;
        goto out0;
    }

    /* Alloc entry resource */
    entry = HI_KZALLOC(HI_ID_PROC, sizeof(user_proc_entry), GFP_KERNEL);
    if (entry == HI_NULL) {
        HI_ERR_UPROC("kmalloc fail\n");
        goto out0;
    }

    memset(entry, 0, sizeof(user_proc_entry));

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    entry->entry_info.entry = proc_create(entry_info->name, 0, dir->entry, &g_user_proc_seq_fops);

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);
    if (entry->entry_info.entry == HI_NULL) {
        HI_FATAL_UPROC("create_proc_entry fail\n");
        goto out1;
    }

    HI_INFO_UPROC("Proc add entry %s, file=%p, entry=0x%p\n", entry_info->name, entry_info->file,
                  entry->entry_info.entry);

    /* Init other parameter */
    strncpy(entry->entry_name, entry_info->name, sizeof(entry->entry_name) - 1);

    entry->entry_name_hash         = drv_user_proc_full_name_hash(entry_info->name, strlen(entry_info->name));
    entry->parent                  = dir->entry;
    entry->entry_info.file         = entry_info->file;
    entry->entry_info.show_func    = entry_info->show_func;
    entry->entry_info.cmd_func     = entry_info->cmd_func;
    entry->entry_info.private_data = entry_info->private_data;
    entry->entry_info.read         = HI_NULL;
    entry->entry_info.write        = HI_NULL;

    /* Add entry to rbtree */
    if (drv_user_proc_rb_insert_entry(dir, entry) != HI_SUCCESS) {
        HI_ERR_UPROC("Insert new file entry failed.\n");
        goto out2;
    }

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return entry;

out2:
    remove_proc_entry(entry->entry_name, entry->parent);
out1:
    HI_KFREE(HI_ID_PROC, entry);
out0:
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_NULL;
}

hi_void drv_user_proc_remove_entry(user_proc_dir *dir, user_proc_entry *entry)
{
    if (entry == HI_NULL || dir == HI_NULL) {
        return;
    }

    remove_proc_entry(entry->entry_name, entry->parent);

    HI_INFO_UPROC("Proc remove entry %s\n", entry->entry_name);

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);
    if (entry->entry_info.read != HI_NULL) {
        HI_KFREE(HI_ID_PROC, entry->entry_info.read);
        entry->entry_info.read = HI_NULL;
    }

    if (entry->entry_info.write != HI_NULL) {
        HI_KFREE(HI_ID_PROC, entry->entry_info.write);
        entry->entry_info.write = HI_NULL;
    }

    /* Remove entry from rbtree */
    drv_user_proc_rb_erase_entry(dir, entry);

    /* If current command belongs to this entry, clear it. */
    if (g_user_proc_k_para.current_cmd.entry == (hi_void *)&(entry->entry_info)) {
        g_user_proc_k_para.current_cmd.entry = HI_NULL;

        memset(&g_user_proc_k_para.current_cmd, 0, sizeof(g_user_proc_k_para.current_cmd));
    }

    HI_KFREE(HI_ID_PROC, entry);

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return;
}

hi_s32 hi_drv_userproc_remove_entry_by_name(const hi_char *entry_name, const hi_char *parent_name)
{
    user_proc_entry *entry = HI_NULL;
    user_proc_dir *dir = HI_NULL;
    hi_char dir_name[HI_USER_PROC_MAX_NAME_LEN + 12]; /* name size is HI_USER_PROC_MAX_NAME_LEN + 12 */

    if ((entry_name == HI_NULL) || (strlen(entry_name) > HI_USER_PROC_MAX_NAME_LEN)) {
        HI_ERR_UPROC("Invalid name\n");
        return HI_FAILURE;
    }

    if ((parent_name == HI_NULL) || (strlen(parent_name) > HI_USER_PROC_MAX_NAME_LEN)) {
        HI_ERR_UPROC("Invalid parent name\n");
        return HI_FAILURE;
    }

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    if (strlen(parent_name) == 0) {
        strlcpy(dir_name, "hisi", sizeof(dir_name) - 1);
    } else {
        snprintf(dir_name, sizeof(dir_name), "%s", parent_name);
    }

    /* Find directory node, if don't exist, return fail */
    dir = drv_user_proc_rb_find_dir(HI_NULL, dir_name);
    if (dir == HI_NULL) {
        HI_ERR_UPROC("Dir %s don't exist\n", parent_name);
        goto out;
    }

    /* Find entry in the directory, if don't exist, return fail */
    entry = drv_user_proc_rb_find_entry(dir, entry_name);
    if (entry == HI_NULL) {
        HI_WARN_UPROC("Entry %s don't exist\n", entry_name);
        goto out;
    }

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);

    remove_proc_entry(entry->entry_name, entry->parent);
    HI_INFO_UPROC("Proc remove entry %s\n", entry->entry_name);

    USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

    if (entry->entry_info.read != HI_NULL) {
        HI_KFREE(HI_ID_PROC, entry->entry_info.read);
        entry->entry_info.read = HI_NULL;
    }

    if (entry->entry_info.write != HI_NULL) {
        HI_KFREE(HI_ID_PROC, entry->entry_info.write);
        entry->entry_info.write = HI_NULL;
    }

    /* Remove entry from rbtree */
    drv_user_proc_rb_erase_entry(dir, entry);

    /* If current command belongs to this entry, clear it. */
    if (g_user_proc_k_para.current_cmd.entry == (hi_void *)&(entry->entry_info)) {
        g_user_proc_k_para.current_cmd.entry = HI_NULL;

        memset(&g_user_proc_k_para.current_cmd, 0, sizeof(g_user_proc_k_para.current_cmd));
    }

    HI_KFREE(HI_ID_PROC, entry);

    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_SUCCESS;

out:
    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
    return HI_FAILURE;
}

hi_void drv_user_proc_remove_dir_forcibly(user_proc_dir *dir)
{
    struct rb_node *node = HI_NULL;
    user_proc_entry *this = HI_NULL;

    if (dir == HI_NULL) {
        return;
    }

    HI_INFO_UPROC("Proc remove dir: %s\n", dir->dir_name);

    while (dir->entry_root.rb_node) {
        node = rb_first(&(dir->entry_root));
        this = rb_entry(node, user_proc_entry, node);
        drv_user_proc_remove_entry(dir, this);
    }

    if (drv_user_proc_remove_dir(dir) != HI_SUCCESS) {
        HI_ERR_UPROC("drv_user_proc_remove_dir failed!\n");
    }
}

static struct file_operations g_user_proc_seq_fops = {
    .open    = drv_user_proc_seq_open,
    .read    = seq_read,
    .write   = drv_user_proc_seq_write,
    .llseek  = seq_lseek,
    .release = drv_user_proc_seq_release,
};

hi_s32 drv_user_proc_ioctl(hi_u32 cmd, hi_void *arg, hi_void *file)
{
    hi_s32 ret = HI_SUCCESS;
    user_proc_k_para *proc = *((user_proc_k_para **)file);

    switch (cmd) {
        case USER_PROC_ADD_ENTRY: {
            hi_bool is_exist = HI_FALSE;
            hi_user_proc_entry_info *entry_info = (hi_user_proc_entry_info *)arg;
            user_proc_entry *entry = HI_NULL;

            entry_info->name[sizeof(entry_info->name) - 1] = 0;
            entry_info->parent_dir[sizeof(entry_info->parent_dir) - 1] = 0;
            entry_info->file = file;

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_dir_entry);
            entry = drv_user_proc_add_entry(entry_info, &is_exist);
            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_dir_entry);

            if (entry == HI_NULL) {
                if (is_exist == HI_TRUE) {
                    ret = HI_ERR_COMMON_RECREATE_ENTRY;
                } else {
                    ret = HI_FAILURE;
                }
                break;
            }

            ((struct proc_dir_entry *)entry->entry_info.entry)->data = proc;
            break;
        }
        case USER_PROC_REMOVE_ENTRY: {
            hi_user_proc_entry_info *entry_info = (hi_user_proc_entry_info *)arg;

            entry_info->name[sizeof(entry_info->name) - 1] = 0;
            entry_info->parent_dir[sizeof(entry_info->parent_dir) - 1] = 0;

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_dir_entry);
            ret = hi_drv_userproc_remove_entry_by_name(entry_info->name, entry_info->parent_dir);
            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_dir_entry);
            break;
        }
        case USER_PROC_ADD_DIR: {
            hi_bool is_exist = HI_FALSE;
            hi_user_proc_dir_info *dir_info = (hi_user_proc_dir_info *)arg;
            user_proc_dir *dir = HI_NULL;

            dir_info->dir_name[sizeof(dir_info->dir_name) - 1] = 0;

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_dir_entry);
            dir = drv_user_proc_add_dir(dir_info->dir_name, dir_info->parent, file, &is_exist);
            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_dir_entry);
            if (dir == HI_NULL) {
                if (is_exist == HI_TRUE) {
                    ret = HI_ERR_COMMON_RECREATE_DIR;
                } else {
                    ret = HI_FAILURE;
                }
            }
            break;
        }
        case USER_PROC_REMOVE_DIR: {
            hi_user_proc_dir_info *dir_info = (hi_user_proc_dir_info *)arg;

            dir_info->dir_name[sizeof(dir_info->dir_name) - 1] = 0;

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_dir_entry);
            ret = drv_user_proc_remove_dir_by_name(dir_info->dir_name, dir_info->parent);
            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_dir_entry);
            break;
        }
        case USER_PROC_GET_CMD: {
            hi_user_proc_info *proc_info = (hi_user_proc_info *)arg;

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

            /* If there is a command, it must belong to a entry and the entry must belong to this file(this process). */
            if ((strlen(proc->current_cmd.cmd) > 0) && (proc->current_cmd.entry != HI_NULL) &&
                    (file == ((hi_user_proc_entry_info *)proc->current_cmd.entry)->file)) {
                memcpy(&(proc_info->cmd_info), &(proc->current_cmd), sizeof(hi_user_proc_cmd_info));

                memcpy(&(proc_info->entry_info), proc->current_cmd.entry, sizeof(hi_user_proc_entry_info));

                memset(proc->current_cmd.cmd, 0, sizeof(proc->current_cmd.cmd));
            } else {
                ret = HI_FAILURE;
            }

            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
            break;
        }
        case USER_PROC_WAKE_READ_TASK: {
            hi_user_proc_show_buf *show_buf = (hi_user_proc_show_buf *)arg;
            hi_user_proc_entry_info *entry_info_ptr = (hi_user_proc_entry_info *)proc->current_cmd.entry;

            if (entry_info_ptr == HI_NULL) {
                HI_ERR_UPROC("entry_info_ptr is NULL!\n");
                ret = HI_FAILURE;
                break;
            }

            if (show_buf->size <= HI_USER_PROC_BUF_SIZE && show_buf->buf != HI_NULL) {
                USER_PROC_K_LOCK(g_user_proc_k_para.sem_read_write);
                if (entry_info_ptr->read != HI_NULL) {
                    HI_ERR_UPROC("entry_info_ptr->read is not NULL!\n");
                    ret = HI_FAILURE;
                    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
                    break;
                }
                entry_info_ptr->read = HI_KZALLOC(HI_ID_PROC, show_buf->size, GFP_KERNEL);
                if (entry_info_ptr->read) {
                    if (osal_copy_from_user(entry_info_ptr->read, (hi_void __user *)show_buf->buf, show_buf->size)) {
                        HI_KFREE(HI_ID_PROC, entry_info_ptr->read);
                        entry_info_ptr->read = HI_NULL;
                    }
                }
                USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
            }

            wake_up_interruptible(&(proc->wq_for_read));
            break;
        }
        case USER_PROC_WAKE_WRITE_TASK: {
            hi_user_proc_show_buf *show_buf = (hi_user_proc_show_buf *)arg;
            hi_user_proc_entry_info *entry_info_ptr = (hi_user_proc_entry_info *)proc->current_cmd.entry;

            if (entry_info_ptr == HI_NULL) {
                HI_ERR_UPROC("entry_info_ptr is NULL!\n");
                ret = HI_FAILURE;
                break;
            }

            if (show_buf->size <= HI_USER_PROC_BUF_SIZE && show_buf->buf != HI_NULL) {
                USER_PROC_K_LOCK(g_user_proc_k_para.sem_read_write);
                if (entry_info_ptr->write != HI_NULL) {
                    HI_ERR_UPROC("entry_info_ptr->write is not NULL!\n");
                    ret = HI_FAILURE;
                    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
                    break;
                }
                entry_info_ptr->write = HI_KZALLOC(HI_ID_PROC, show_buf->size, GFP_KERNEL);
                if (entry_info_ptr->write) {
                    if (osal_copy_from_user(entry_info_ptr->write, (hi_void __user *)show_buf->buf, show_buf->size)) {
                        HI_KFREE(HI_ID_PROC, entry_info_ptr->write);
                        entry_info_ptr->write = HI_NULL;
                    }
                }
                USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
            }

            wake_up_interruptible(&(proc->wq_for_write));
            break;
        }
        default:
            ret = HI_FAILURE;
            HI_ERR_SYS("unknown command 0x%x\n", cmd);
            break;
    }

    return ret;
}

#ifdef CONFIG_COMPAT
hi_s32 drv_user_proc_compat_ioctl(hi_u32 cmd, hi_void *arg, hi_void *file)
{
    hi_s32 ret = HI_SUCCESS;
    user_proc_k_para *proc = *((user_proc_k_para **)file);

    switch (cmd) {
        case USER_PROC_ADD_COMPAT_ENTRY: {
            hi_bool is_exist = HI_FALSE;
            hi_user_proc_compat_entry_info *compat_entry_info = (hi_user_proc_compat_entry_info *)arg;
            hi_user_proc_entry_info entry_info = {{0}};
            user_proc_entry *entry = HI_NULL;

            memcpy(entry_info.name, compat_entry_info->name, sizeof(entry_info.name));
            memcpy(entry_info.parent_dir, compat_entry_info->parent_dir, sizeof(entry_info.parent_dir));

            entry_info.name[sizeof(entry_info.name) - 1] = 0;
            entry_info.parent_dir[sizeof(entry_info.parent_dir) - 1] = 0;
            entry_info.show_func    = (hi_user_proc_show)compat_ptr(compat_entry_info->show_func);
            entry_info.cmd_func     = (hi_user_proc_cmd)compat_ptr(compat_entry_info->cmd_func);
            entry_info.private_data = (hi_void *)compat_ptr(compat_entry_info->private_data);
            entry_info.entry        = (hi_void *)compat_ptr(compat_entry_info->entry);
            entry_info.file         = file;
            entry_info.read         = (hi_void *)compat_ptr(compat_entry_info->read);
            entry_info.write        = (hi_void *)compat_ptr(compat_entry_info->write);

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_dir_entry);
            entry = drv_user_proc_add_entry(&entry_info, &is_exist);
            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_dir_entry);
            if (entry == HI_NULL) {
                if (is_exist == HI_TRUE) {
                    ret = HI_ERR_COMMON_RECREATE_ENTRY;
                } else {
                    ret = HI_FAILURE;
                }
                break;
            }

            ((struct proc_dir_entry *)entry->entry_info.entry)->data = proc;
            break;
        }
        case USER_PROC_REMOVE_COMPAT_ENTRY: {
            hi_user_proc_compat_entry_info *compat_entry_info = (hi_user_proc_compat_entry_info *)arg;
            hi_user_proc_entry_info entry_info = {{0}};

            memcpy(entry_info.name, compat_entry_info->name, sizeof(entry_info.name));
            memcpy(entry_info.parent_dir, compat_entry_info->parent_dir, sizeof(entry_info.parent_dir));

            entry_info.name[sizeof(entry_info.name) - 1] = 0;
            entry_info.parent_dir[sizeof(entry_info.parent_dir) - 1] = 0;
            entry_info.show_func    = (hi_user_proc_show)compat_ptr(compat_entry_info->show_func);
            entry_info.cmd_func     = (hi_user_proc_cmd)compat_ptr(compat_entry_info->cmd_func);
            entry_info.private_data = (hi_void *)compat_ptr(compat_entry_info->private_data);
            entry_info.entry        = (hi_void *)compat_ptr(compat_entry_info->entry);
            entry_info.file         = file;
            entry_info.read         = (hi_void *)compat_ptr(compat_entry_info->read);
            entry_info.write        = (hi_void *)compat_ptr(compat_entry_info->write);

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_dir_entry);
            ret = hi_drv_userproc_remove_entry_by_name((hi_char *)entry_info.name, (hi_char *)entry_info.parent_dir);
            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_dir_entry);
            break;
        }
        case USER_PROC_COMPAT_GET_CMD: {
            hi_user_proc_compat_info *compat_proc_info = (hi_user_proc_compat_info *)arg;

            USER_PROC_K_LOCK(g_user_proc_k_para.sem_general);

            /* If there is a command, it must belong to a entry and the entry must belong to this file(this process). */
            if ((strlen(proc->current_cmd.cmd) > 0) && (proc->current_cmd.entry != HI_NULL) &&
                    (file == ((hi_user_proc_entry_info *)proc->current_cmd.entry)->file)) {
                hi_user_proc_compat_entry_info tmp_entry_info = {{0}};

                memcpy(compat_proc_info->cmd_info.cmd, proc->current_cmd.cmd, sizeof(proc->current_cmd.cmd));

                tmp_entry_info.show_func    = ptr_to_compat(((hi_user_proc_entry_info *)
                                                            proc->current_cmd.entry)->show_func);
                tmp_entry_info.cmd_func     = ptr_to_compat(((hi_user_proc_entry_info *)
                                                            proc->current_cmd.entry)->cmd_func);
                tmp_entry_info.private_data = ptr_to_compat(((hi_user_proc_entry_info *)
                                                            proc->current_cmd.entry)->private_data);

                memcpy(&(compat_proc_info->entry_info), &tmp_entry_info, sizeof(hi_user_proc_compat_entry_info));

                memset(proc->current_cmd.cmd, 0, sizeof(proc->current_cmd.cmd));
            } else {
                ret = HI_FAILURE;
            }

            USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_general);
            break;
        }
        case USER_PROC_COMPAT_WAKE_READ_TASK: {
            hi_user_proc_compat_show_buf *show_buf = (hi_user_proc_compat_show_buf *)arg;
            hi_user_proc_entry_info *entry_info_ptr = (hi_user_proc_entry_info *)proc->current_cmd.entry;

            if (entry_info_ptr == HI_NULL) {
                HI_ERR_UPROC("entry_info_ptr is null\n");
                ret = HI_FAILURE;
                break;
            }

            if (show_buf->size <= HI_USER_PROC_BUF_SIZE && (hi_void __user *)compat_ptr(show_buf->buf) != HI_NULL) {
                USER_PROC_K_LOCK(g_user_proc_k_para.sem_read_write);
                if (entry_info_ptr->read != HI_NULL) {
                    HI_ERR_UPROC("entry_info_ptr->read is not NULL!\n");
                    ret = HI_FAILURE;
                    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
                    break;
                }
                entry_info_ptr->read = HI_KZALLOC(HI_ID_PROC, show_buf->size, GFP_KERNEL);
                if (entry_info_ptr->read) {
                    if (osal_copy_from_user(entry_info_ptr->read, (hi_void __user *)compat_ptr(show_buf->buf),
                                            show_buf->size)) {
                        HI_KFREE(HI_ID_PROC, entry_info_ptr->read);
                        entry_info_ptr->read = HI_NULL;
                    }
                }
                USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
            }

            wake_up_interruptible(&(proc->wq_for_read));
            break;
        }
        case USER_PROC_COMPAT_WAKE_WRITE_TASK: {
            hi_user_proc_compat_show_buf *show_buf = (hi_user_proc_compat_show_buf *)arg;
            hi_user_proc_entry_info *entry_info_ptr = (hi_user_proc_entry_info *)proc->current_cmd.entry;

            if (entry_info_ptr == HI_NULL) {
                HI_ERR_UPROC("entry_info_ptr is NULL!\n");
                ret = HI_FAILURE;
                break;
            }

            if (show_buf->size <= HI_USER_PROC_BUF_SIZE && (hi_void __user *)compat_ptr(show_buf->buf) != HI_NULL) {
                USER_PROC_K_LOCK(g_user_proc_k_para.sem_read_write);
                if (entry_info_ptr->write != HI_NULL) {
                    HI_ERR_UPROC("entry_info_ptr->write is not NULL!\n");
                    ret = HI_FAILURE;
                    USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
                    break;
                }

                entry_info_ptr->write = HI_KZALLOC(HI_ID_PROC, show_buf->size, GFP_KERNEL);
                if (entry_info_ptr->write) {
                    if (osal_copy_from_user(entry_info_ptr->write, (hi_void __user *)compat_ptr(show_buf->buf),
                                            show_buf->size)) {
                        HI_KFREE(HI_ID_PROC, entry_info_ptr->write);
                        entry_info_ptr->write = HI_NULL;
                    }
                }
                USER_PROC_K_UNLOCK(g_user_proc_k_para.sem_read_write);
            }

            wake_up_interruptible(&(proc->wq_for_write));
            break;
        }
        default:
            return drv_user_proc_ioctl(cmd, arg, file);
    }

    return ret;
}
#endif

hi_s32 drv_user_proc_init(hi_void)
{
    osal_sem_init(&g_user_proc_k_para.sem_general, 1);
    osal_sem_init(&g_user_proc_k_para.sem_dir_entry, 1);
    osal_sem_init(&g_user_proc_k_para.sem_read_write, 1);

    osal_atomic_init(&g_user_proc_k_para.open_cnt);
    osal_atomic_set(&g_user_proc_k_para.open_cnt, 0);

    g_hisi_dirent = drv_user_proc_add_private_dir("hisi", (struct proc_dir_entry *)osal_get_hisi_entry());
    if (!g_hisi_dirent) {
        HI_ERR_UPROC("add 'hisi' directory failed.\n");
        goto out0;
    }
    g_msp_dirent = drv_user_proc_add_private_dir("msp", (struct proc_dir_entry *)osal_get_msp_entry());
    if (!g_msp_dirent) {
        HI_ERR_UPROC("add 'msp' directory failed.\n");
        goto out1;
    }

    return HI_SUCCESS;

out1:
    drv_user_proc_remove_private_dir(g_hisi_dirent->dir_name);
    g_hisi_dirent = HI_NULL;
out0:
    osal_atomic_destory(&g_user_proc_k_para.open_cnt);

    osal_sem_destory(&g_user_proc_k_para.sem_read_write);
    osal_sem_destory(&g_user_proc_k_para.sem_dir_entry);
    osal_sem_destory(&g_user_proc_k_para.sem_general);

    return HI_FAILURE;
}

hi_void drv_user_proc_exit(hi_void)
{
    if (g_msp_dirent) {
        drv_user_proc_remove_private_dir(g_msp_dirent->dir_name);
        g_msp_dirent = HI_NULL;
    }
    if (g_hisi_dirent) {
        drv_user_proc_remove_private_dir(g_hisi_dirent->dir_name);
        g_hisi_dirent = HI_NULL;
    }

    osal_atomic_destory(&g_user_proc_k_para.open_cnt);

    osal_sem_destory(&g_user_proc_k_para.sem_read_write);
    osal_sem_destory(&g_user_proc_k_para.sem_dir_entry);
    osal_sem_destory(&g_user_proc_k_para.sem_general);
}

