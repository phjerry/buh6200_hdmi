#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/path.h>
#include <linux/seq_file.h>
#include <linux/namei.h>
#include <linux/stat.h>
#include <linux/version.h>

#include "hi_type.h"

#include "hi_drv_sys.h"
#include "hi_drv_log.h"
#include "hi_drv_file.h"
#include "drv_log.h"

#define PARENT_DIR_INODE_LOCK(d_inode)      down_write_trylock(&(d_inode)->i_rwsem)
#define PARENT_DIR_INODE_UNLOCK(d_inode)    up_write(&(d_inode)->i_rwsem)

struct file* hi_drv_file_open(const hi_char *name, hi_u32 flags)
{
    struct file *file = NULL;

    if (name == NULL) {
        return NULL;
    }

    if (flags == 0) {
        flags = O_RDONLY;
    } else {
        flags = O_WRONLY | O_CREAT | O_APPEND;
    }

    file = filp_open(name, flags | O_LARGEFILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (IS_ERR(file)) {
        return NULL;
    }

    return file;
}

hi_void hi_drv_file_close(struct file *file)
{
    if (file != NULL) {
        filp_close(file, NULL);
    }
}

hi_s32 hi_drv_file_read(struct file *file, hi_u8 *buf, const hi_u32 len)
{
    hi_s32 read_len;
    mm_segment_t old_fs = {0};

    if ((file == NULL) || (buf == NULL)) {
        return -ENOENT; /* No such file or directory */
    }

    if (((file->f_flags & O_ACCMODE) & (O_RDONLY | O_RDWR)) != 0) {
        return -EACCES; /* Permission denied */
    }

    /* saved the original file space */
    old_fs = get_fs();

    /* extend to the kernel data space */
    set_fs(KERNEL_DS);

    read_len = vfs_read(file, buf, len, &file->f_pos);

    /* Restore the original file space */
    set_fs(old_fs);

    return read_len;
}

hi_s32 hi_drv_file_write(struct file *file, const hi_u8 *buf, const hi_u32 len)
{
    hi_s32 write_len;
    mm_segment_t old_fs = {0};

    if ((file == NULL) || (buf == NULL)) {
        return -ENOENT; /* No such file or directory */
    }

    if (((file->f_flags & O_ACCMODE) & (O_WRONLY | O_RDWR)) == 0) {
        return -EACCES; /* Permission denied */
    }

    old_fs = get_fs();

    set_fs(KERNEL_DS);

    write_len = vfs_write(file, buf, len, &file->f_pos);

    set_fs(old_fs);

    return write_len;
}

loff_t hi_drv_file_lseek(struct file *file, const hi_s32 offset, const hi_s32 whence)
{
    return vfs_llseek(file, offset, whence);
}

hi_s32 hi_drv_file_get_store_path(hi_char *path, const hi_u32 len)
{
    if ((path == NULL) || (len == 0)) {
        return HI_FAILURE;
    }

    return hi_drv_log_get_store_path(path, len);
}

EXPORT_SYMBOL(hi_drv_file_open);
EXPORT_SYMBOL(hi_drv_file_close);
EXPORT_SYMBOL(hi_drv_file_read);
EXPORT_SYMBOL(hi_drv_file_write);
EXPORT_SYMBOL(hi_drv_file_lseek);
EXPORT_SYMBOL(hi_drv_file_get_store_path);

