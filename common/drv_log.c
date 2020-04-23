/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2009-12-17
 */

#include <linux/ion.h>
#include <linux/dma-buf.h>
#include <linux/hisilicon/securec.h>

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_osal.h"

#include "hi_drv_dev.h"
#include "hi_drv_module.h"
#include "hi_drv_log.h"
#include "hi_drv_sys.h"
#include "hi_drv_file.h"
#include "drv_log.h"
#include "drv_common_ioctl.h"

typedef struct {
    struct dma_buf *dmabuf;
    hi_u8          *virt_addr;
} log_ion_buf;

#define LOG_MSG_BUF_RESERVE (4 * 1024)

static log_ion_buf   g_log_cfg_buf;
static log_cfg_info *g_log_cfg_info; /* kernel-state pointer of log control info */ /* CNcomment: 打印控制信息的内核态指针 */

static hi_bool g_log_init = HI_FALSE;

#define LOG_MAX_FILE_SIZE     120U /* 120M */
#define LOG_MIN_FILE_SIZE     1U   /* 1M */
#define LOG_DEFAULT_FILE_SIZE 12U  /* 12M */

#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
#define LOG_MAX_FILE_CNT (0x10000U) /* 65536 */
static log_ion_buf  g_log_msg_buf;
static log_buf_info g_log_msg_info;
#endif

static osal_semaphore g_log_k_mutex;

#define LOG_FILE_LOCK()   osal_sem_down_interruptible(&g_log_k_mutex)
#define LOG_FILE_UNLOCK() osal_sem_up(&g_log_k_mutex)

/* this variable will be used by /kmod/load script */
#if defined(CHIP_TYPE_HI3716MV430)
static hi_s32 g_log_buf_size = 16 * LOG_MSG_BUF_RESERVE; /* Multiply by 16 means buf_size is 64K */
#else
static hi_s32 g_log_buf_size = 256 * LOG_MSG_BUF_RESERVE; /* Multiply by 256 means buf_size is 1M */
#endif

static hi_char g_log_path_buf[LOG_MAX_FILE_NAME_LENTH] = {0};
static hi_char *g_udisk_log_file = g_log_path_buf;

static hi_bool g_set_log_file_flag = HI_FALSE;

static hi_char g_store_path_buf[LOG_MAX_FILE_NAME_LENTH] = "/mnt";
hi_char *g_store_path = g_store_path_buf;

static hi_s32 g_default_log_size = LOG_DEFAULT_FILE_SIZE * 1024 * 1024; /* Multiply by 1024 means buf_size is 16M */

hi_char *g_debug_level_name[HI_LOG_LEVEL_MAX + 1] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE", "MAX" };

#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
static hi_void drv_log_reset_buf(hi_void)
{
    hi_ulong flags;

    local_irq_save(flags);
    g_log_msg_info.read = g_log_msg_info.write;
    g_log_msg_info.reset_cnt++;
    local_irq_restore(flags);
}

static hi_s32 drv_log_wait_condition(const hi_void *param)
{
    return (((log_buf_info *)param)->write != ((log_buf_info *)param)->read);
}

hi_s32 hi_drv_log_read_buf(hi_u8 *buf, hi_u32 buf_len, hi_u32 *copy_len, hi_bool is_kernel_copy)
{
    log_buf_info cur_msg_info = {0};
    hi_u32 buf_used;
    hi_u32 data_len1;
    hi_u32 data_len2;
    hi_u32 tmp_copy_len;
    hi_u32 new_read_addr;
    hi_ulong flags;

    if (g_log_msg_info.buf_size == 0) {
        HI_ERR_SYS("Log Buffer size is 0, Please confige the Buffer size, for example:");
        HI_ERR_SYS("    Config buffer size 500K: insmod hi_cmpi.ko g_log_buf_size = 0x80000");
        return HI_FAILURE;
    }

    if (buf == HI_NULL) {
        HI_ERR_SYS("buf is null\n");
        return HI_FAILURE;
    }

    if (g_log_msg_info.write == g_log_msg_info.read) {
        if (g_set_log_file_flag == HI_TRUE) {
            return HI_FAILURE;
        } else {
            /* the following code segment will pending when reboot or reload ko */
            if (osal_wait_timeout_interruptible(&g_log_msg_info.wq_no_data, drv_log_wait_condition,
                &g_log_msg_info, 500) == HI_FALSE) { /* 500ms */
                return HI_FAILURE;
            }
        }
    }

    local_irq_save(flags);
    memcpy(&cur_msg_info, &g_log_msg_info, sizeof(g_log_msg_info));
    local_irq_restore(flags);

    if (cur_msg_info.write < cur_msg_info.read) {
        buf_used  = cur_msg_info.buf_size - cur_msg_info.read + cur_msg_info.write;
        data_len1 = cur_msg_info.buf_size - cur_msg_info.read;
        data_len2 = cur_msg_info.write;
    } else {
        buf_used  = cur_msg_info.write - cur_msg_info.read;
        data_len1 = buf_used;
        data_len2 = 0;
    }

    if (buf_len <= (data_len1 + data_len2)) {
        tmp_copy_len = buf_len;
    } else {
        tmp_copy_len = data_len1 + data_len2;
    }

    if (data_len1 >= tmp_copy_len) {
        if (is_kernel_copy == HI_FALSE) {
            if (osal_copy_to_user(buf, (cur_msg_info.read + cur_msg_info.start_virt_addr), tmp_copy_len)) {
                HI_ERR_SYS("copy_to_user error\n");
                return HI_FAILURE;
            }
        } else {
            memcpy(buf, (cur_msg_info.read + cur_msg_info.start_virt_addr), tmp_copy_len);
        }
        new_read_addr = cur_msg_info.read + tmp_copy_len;
    } else {
        if (is_kernel_copy == HI_FALSE) {
            if (osal_copy_to_user(buf, (cur_msg_info.read + cur_msg_info.start_virt_addr), data_len1)) {
                HI_ERR_SYS("copy_to_user error\n");
                return HI_FAILURE;
            }
        } else {
            memcpy(buf, (cur_msg_info.read + cur_msg_info.start_virt_addr), data_len1);
        }

        if (is_kernel_copy == HI_FALSE) {
            if (osal_copy_to_user((buf + data_len1), cur_msg_info.start_virt_addr, (tmp_copy_len - data_len1))) {
                HI_ERR_SYS("copy_to_user error\n");
                return HI_FAILURE;
            }
        } else {
            memcpy((buf + data_len1), cur_msg_info.start_virt_addr, (tmp_copy_len - data_len1));
        }
        new_read_addr = tmp_copy_len - data_len1;
    }

    *copy_len = tmp_copy_len;

    if (new_read_addr >= cur_msg_info.buf_size) {
        new_read_addr = 0;
    }

    local_irq_save(flags);
    if (cur_msg_info.reset_cnt == g_log_msg_info.reset_cnt) {
        g_log_msg_info.read = new_read_addr;
    }
    local_irq_restore(flags);

    return HI_SUCCESS;
}

hi_s32 hi_drv_log_write_buf(hi_u8 *buf, hi_u32 msg_len, hi_u32 msg_from_pos)
{
    hi_u32 copy_len1;
    hi_u32 copy_len2;
    hi_u32 new_write_addr;
    hi_ulong flags;

    if (g_log_msg_info.buf_size == 0) {
        return HI_FAILURE;
    }

    if (buf == HI_NULL || msg_len >= g_log_buf_size) {
        HI_ERR_SYS("print data length too long or buf is null.\n");
        return HI_FAILURE;
    }

    /*
     * protect with semaphore while two module write at the same time
     * CNcomment: 两个模块同时写要做信号量保护
     */
    local_irq_save(flags);

    if (g_log_msg_info.write < g_log_msg_info.read) {
        if ((g_log_msg_info.read - g_log_msg_info.write) < LOG_MSG_BUF_RESERVE) {
            drv_log_reset_buf();
        }
    } else {
        if ((g_log_msg_info.write - g_log_msg_info.read) > (g_log_msg_info.buf_size - LOG_MSG_BUF_RESERVE)) {
            drv_log_reset_buf();
        }
    }

    if ((msg_len + g_log_msg_info.write) >= g_log_msg_info.buf_size) {
        copy_len1 = g_log_msg_info.buf_size - g_log_msg_info.write;
        copy_len2 = msg_len - copy_len1;
        new_write_addr = copy_len2;
    } else {
        copy_len1 = msg_len;
        copy_len2 = 0;
        new_write_addr = msg_len + g_log_msg_info.write;
    }

    if (copy_len1 > 0) {
        if (msg_from_pos == LOG_MSG_FROM_KERNEL) {
            memcpy((g_log_msg_info.write + g_log_msg_info.start_virt_addr), buf, copy_len1);
        } else {
            if (osal_copy_from_user((g_log_msg_info.write + g_log_msg_info.start_virt_addr), buf, copy_len1)) {
                HI_ERR_SYS("copy_from_user error\n");
            }
        }
    }

    if (copy_len2 > 0) {
        if (msg_from_pos == LOG_MSG_FROM_KERNEL) {
            memcpy(g_log_msg_info.start_virt_addr, (buf + copy_len1), copy_len2);
        } else {
            if (osal_copy_from_user(g_log_msg_info.start_virt_addr, (buf + copy_len1), copy_len2)) {
                HI_ERR_SYS("copy_from_user error\n");
            }
        }
    }

    g_log_msg_info.write = new_write_addr;

    if (g_set_log_file_flag != HI_TRUE) {
        osal_wait_wakeup(&g_log_msg_info.wq_no_data);
    }

    local_irq_restore(flags);

    return HI_SUCCESS;
}

static hi_s32 drv_log_print_to_buf(const hi_char *format, ...)
{
    hi_char log_str[LOG_MAX_TRACE_LEN] = {0};
    hi_u32 msg_len;
    va_list args;

    if (g_log_init == HI_FALSE) {
        HI_ERR_SYS("Log is not init!\n");
        return HI_FAILURE;
    }

    va_start(args, format);
    msg_len = vsnprintf(log_str, LOG_MAX_TRACE_LEN - 1, format, args);
    va_end(args);

    if (msg_len >= (LOG_MAX_TRACE_LEN - 0x1)) {
        log_str[LOG_MAX_TRACE_LEN - 0x1] = '\0'; /* even the 'vsnprintf' commond will do it */
        log_str[LOG_MAX_TRACE_LEN - 0x2] = '\n';
        log_str[LOG_MAX_TRACE_LEN - 0x3] = '.';
        log_str[LOG_MAX_TRACE_LEN - 0x4] = '.';
        log_str[LOG_MAX_TRACE_LEN - 0x5] = '.';
    }

    return hi_drv_log_write_buf((hi_u8 *)log_str, msg_len, LOG_MSG_FROM_KERNEL);
}
#endif

static hi_u32 drv_log_get_time_ms(hi_void)
{
    struct timeval tv;
    do_gettimeofday(&tv);
    return (((hi_u32)tv.tv_sec) * 1000 + ((hi_u32)tv.tv_usec) / 1000); /* Multiply and devide by 1000 to get ms */
}

hi_s32 hi_drv_log_add_module(const hi_char *name, hi_u32 module_id)
{
    if (g_log_cfg_info == HI_NULL || module_id >= LOG_CFG_BUF_SIZE / sizeof(log_cfg_info)) {
        return HI_FAILURE;
    }

    g_log_cfg_info[module_id].log_level = HI_LOG_LEVEL_DEFAULT;
    snprintf(g_log_cfg_info[module_id].module_name, sizeof(g_log_cfg_info[module_id].module_name), "%s", name);

    return HI_SUCCESS;
}

hi_s32 hi_drv_log_remove_module(hi_u32 module_id)
{
    if (g_log_cfg_info == HI_NULL || module_id >= LOG_CFG_BUF_SIZE / sizeof(log_cfg_info)) {
        return HI_FAILURE;
    }

    g_log_cfg_info[module_id].log_level = HI_LOG_LEVEL_DEFAULT;
    snprintf(g_log_cfg_info[module_id].module_name, sizeof(g_log_cfg_info[module_id].module_name), "Invalid");

    return HI_SUCCESS;
}

/* HI_TRUE: need print; HI_FALSE: not print */
static hi_bool drv_log_check_level(hi_u32 level, hi_u32 module_id)
{
    if (module_id < LOG_CFG_BUF_SIZE / sizeof(log_cfg_info)) {
        if ((g_log_init == HI_TRUE) && (level <= g_log_cfg_info[module_id].log_level) &&
                (g_log_cfg_info[module_id].log_level != HI_LOG_LEVEL_TRACE)) {
            return HI_TRUE;
        }
        if ((g_log_init == HI_TRUE) && (level == g_log_cfg_info[module_id].log_level) &&
                (g_log_cfg_info[module_id].log_level == HI_LOG_LEVEL_TRACE)) {
            return HI_TRUE;
        }
        if ((g_log_init == HI_FALSE) && (level <= HI_LOG_LEVEL_DEFAULT)) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

/* 0: seria port, 1: network */
static hi_u32 drv_log_get_print_pos(hi_u32 module_id)
{
    hi_u32 pos = 0;

    if (module_id >= LOG_CFG_BUF_SIZE / sizeof(log_cfg_info)) {
        return LOG_OUTPUT_SERIAL;
    }

    if (g_log_init == HI_FALSE) {
        return LOG_OUTPUT_SERIAL;
    }

    if (g_set_log_file_flag == HI_TRUE) {
        pos = LOG_OUTPUT_UDISK;
    } else {
        pos = g_log_cfg_info[module_id].log_print_pos;
    }

    return pos;
}

#ifdef LOG_UDISK_SUPPORT
hi_s32 drv_log_udisk_save(const hi_s8 *file_name, hi_s8 *data, hi_u32 data_len)
{
    hi_s32 write_len;
    struct file *file = HI_NULL;
    static hi_u64 file_index = 0;
    hi_s8 cur_file_name[LOG_MAX_FILE_NAME_LENTH] = {0};

    snprintf(cur_file_name, sizeof(cur_file_name) - 1, "%s%08llu", file_name, file_index % LOG_MAX_FILE_CNT);

    file = hi_drv_file_open(cur_file_name, 1);
    if (file == HI_NULL) {
        HI_ERR_SYS("hi_drv_file_open %s failure...................\n", file_name);
        return HI_FAILURE;
    }

    write_len = hi_drv_file_write(file, data, data_len);

    hi_drv_file_close(file);
    return HI_SUCCESS;
}

static osal_task *g_udisk_task = HI_NULL;
static hi_u8 g_udisk_buf[700] = {0}; /* array size is 700 */
static hi_u8 g_udisk_file_name[LOG_MAX_FILE_NAME_LENTH] = {0};

hi_s32 drv_log_udisk_write_thread(hi_void *arg)
{
    hi_u32 read_len = 0;
    hi_s32 ret = 0;
    hi_bool set_file_flag = HI_FALSE;

    while (1) {
        ret = LOG_FILE_LOCK();
        if (ret != 0) {
            HI_WARN_SYS("down_interruptible failed\n");
            osal_msleep(10); /* interval 10ms */
            continue;
        }

        set_file_flag = g_set_log_file_flag;
        snprintf(g_udisk_file_name, sizeof(g_udisk_file_name) - 1, "%s/stb.log", (const hi_s8 *)g_udisk_log_file);

        LOG_FILE_UNLOCK();

        set_current_state(TASK_INTERRUPTIBLE);
        if (osal_kthread_should_stop()) {
            break;
        }

        if (set_file_flag == HI_FALSE) {
            osal_msleep(10); /* 10ms polling */
            continue;
        }

        memset(g_udisk_buf, 0, sizeof(g_udisk_buf));

        ret = hi_drv_log_read_buf(g_udisk_buf, sizeof(g_udisk_buf) - 1, &read_len, HI_TRUE);
        if (ret == HI_SUCCESS) {
            drv_log_udisk_save((const hi_s8 *)g_udisk_file_name, g_udisk_buf, read_len);
        }

        osal_msleep(100); /* save log every 100ms */
    }

    return HI_SUCCESS;
}

hi_s32 drv_log_udisk_init(const hi_u8 *disk_folder)
{
    if (disk_folder == HI_NULL) {
        return HI_FAILURE;
    }

    if (g_udisk_task == HI_NULL) {
        g_udisk_task = osal_kthread_create(drv_log_udisk_write_thread, (hi_void *)disk_folder, "g_udisk_task", 0);
        if (g_udisk_task == HI_NULL) {
            HI_ERR_SYS("create new kernel thread failed\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_log_udisk_exit(hi_void)
{
    if (g_udisk_task) {
        osal_kthread_destroy(g_udisk_task, HI_TRUE);
        g_udisk_task = HI_NULL;
    }

    return HI_SUCCESS;
}
#endif

hi_void hi_log_print(hi_u32 level, hi_u32 module_id, hi_u8 *fn_name, hi_u32 line_num, const hi_char *format, ...)
{
    va_list args;
    hi_u32  msg_len = 0;
    hi_bool is_print, level_check;
    hi_s32  pos;
    hi_char log_str[LOG_MAX_TRACE_LEN] = {'a'};

    level_check = drv_log_check_level(level, module_id);
    pos = drv_log_get_print_pos(module_id);
    if (pos == LOG_OUTPUT_NETWORK || pos == LOG_OUTPUT_UDISK) {
        if (level <= HI_LOG_LEVEL_INFO) {
            is_print = HI_TRUE;
        } else {
            is_print = HI_FALSE;
        }
    } else {
        is_print = level_check;
    }

    if (is_print) {
        log_str[LOG_MAX_TRACE_LEN - 0x1] = 'b';
        log_str[LOG_MAX_TRACE_LEN - 0x2] = 'c';

        va_start(args, format);
        msg_len = vsnprintf(log_str, LOG_MAX_TRACE_LEN, format, args);
        va_end(args);

        if (msg_len >= LOG_MAX_TRACE_LEN) {
            log_str[LOG_MAX_TRACE_LEN - 0x1] = '\0'; /* even the 'vsnprintf' commond will do it */
            log_str[LOG_MAX_TRACE_LEN - 0x2] = '\n';
            log_str[LOG_MAX_TRACE_LEN - 0x3] = '.';
            log_str[LOG_MAX_TRACE_LEN - 0x4] = '.';
            log_str[LOG_MAX_TRACE_LEN - 0x5] = '.';
        }

        if (g_log_init == HI_TRUE) {
#ifndef ANDROID
            hi_u32 time_ms     __attribute__((unused)) = drv_log_get_time_ms() % LOG_DAY;
            hi_u32 hour        __attribute__((unused)) = time_ms / LOG_HOUR;
            hi_u32 minute      __attribute__((unused)) = (time_ms % LOG_HOUR) / LOG_MINUTE;
            hi_u32 second      __attribute__((unused)) = ((time_ms % LOG_HOUR) % LOG_MINUTE) / LOG_SECOND;
            hi_u32 millisecond __attribute__((unused)) = time_ms % LOG_SECOND;
#endif
            switch (pos) {
#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
                case LOG_OUTPUT_NETWORK:
                case LOG_OUTPUT_UDISK: {
                    drv_log_print_to_buf(LOG_PRINT_FORMAT);
                    if (level_check == HI_FALSE) {
                        break;
                    }
                }
#endif
                case LOG_OUTPUT_SERIAL: {
                    if ((module_id != (hi_u32)HI_ID_VSYNC) && (module_id != (hi_u32)HI_ID_ASYNC)) {
                        HI_PRINT(LOG_PRINT_FORMAT);
                    }
                    break;
                }
                default:
                    break;
            }
        } else {
            HI_PRINT("[%s-Unknow]: %s[%d]:%s\n", g_debug_level_name[level], fn_name, line_num, log_str);
        }
    }
}

static hi_s32 drv_log_set_path(hi_char *path, hi_u32 path_len, hi_bool is_user_state)
{
    hi_s32 ret;

    if (path == HI_NULL || path_len >= sizeof(g_log_path_buf)) {
        return HI_FAILURE;
    }

    ret = LOG_FILE_LOCK();
    if (ret != 0) {
        HI_ERR_SYS("down_interruptible failed\n");
        return HI_FAILURE;
    }

    memset(g_log_path_buf, 0, sizeof(g_log_path_buf));

    if (is_user_state == HI_TRUE) {
        ret = osal_copy_from_user(g_log_path_buf, path, path_len);
    } else {
        ret = memcpy_s(g_log_path_buf, sizeof(g_log_path_buf), path, path_len);
    }

    if (ret != 0) {
        LOG_FILE_UNLOCK();
        return HI_FAILURE;
    }

    if (memcmp(g_log_path_buf, "/dev/null", strlen("/dev/null")) == 0) {
        g_set_log_file_flag = HI_FALSE;
    } else {
        g_set_log_file_flag = HI_TRUE;
    }

    g_log_cfg_info->udisk_flag = (hi_u8)g_set_log_file_flag;

    g_udisk_log_file = g_log_path_buf;

    LOG_FILE_UNLOCK();

    return HI_SUCCESS;
}

hi_s32 hi_drv_log_set_path(hi_char *path, hi_u32 path_len)
{
    return drv_log_set_path(path, path_len, HI_FALSE);
}

hi_s32 hi_drv_log_get_path(hi_s8 *buf, hi_u32 buf_len)
{
    hi_s32 path_len;

    if (g_udisk_log_file == HI_NULL) {
        HI_ERR_SYS("g_udisk_log_file is NULL!\n");
        return HI_FAILURE;
    }

    path_len = strlen(g_udisk_log_file) + 1;
    if (path_len > buf_len) {
        HI_ERR_SYS("path len is lager than buf len!\n");
        return HI_FAILURE;
    }
    if (path_len == 1) {
        HI_INFO_SYS("no redirect to file!\n");
        return HI_SUCCESS;
    }

    memcpy(buf, g_udisk_log_file, path_len);

    return HI_SUCCESS;
}

static hi_s32 drv_log_set_store_path(hi_char *path, hi_u32 path_len, hi_bool is_user_state)
{
    hi_s32 ret;

    if (path == HI_NULL || path_len >= sizeof(g_store_path_buf)) {
        return HI_FAILURE;
    }

    ret = LOG_FILE_LOCK();
    if (ret != 0) {
        HI_ERR_SYS("down_interruptible failed\n");
        return HI_FAILURE;
    }

    memset(g_store_path_buf, 0, sizeof(g_store_path_buf));

    if (is_user_state == HI_TRUE) {
        ret = osal_copy_from_user(g_store_path_buf, path, path_len);
    } else {
        ret = memcpy_s(g_store_path_buf, sizeof(g_store_path_buf), path, path_len);
    }

    if (ret != 0) {
        LOG_FILE_UNLOCK();
        return HI_FAILURE;
    }

    g_store_path = g_store_path_buf;
    LOG_FILE_UNLOCK();

    return HI_SUCCESS;
}

hi_s32 hi_drv_log_set_store_path(hi_char *path, hi_u32 path_len)
{
    return drv_log_set_store_path(path, path_len, HI_FALSE);
}

hi_s32 hi_drv_log_get_store_path(hi_s8 *buf, hi_u32 buf_len)
{
    hi_s32 path_len;

    if (g_store_path == HI_NULL) {
        return HI_FAILURE;
    }

    path_len = strlen(g_store_path) + 1;
    if (path_len > buf_len || path_len <= 1) {
        return HI_FAILURE;
    }

    memcpy(buf, g_store_path, path_len);

    return HI_SUCCESS;
}

hi_s32 hi_drv_log_set_size(hi_u32 size)
{
    hi_s32 ret;

    if (size > LOG_MAX_FILE_SIZE || size < LOG_MIN_FILE_SIZE) {
        HI_ERR_SYS("Logsize(%dMB) is out of range(%dMB~%dMB)\n", size, LOG_MIN_FILE_SIZE, LOG_MAX_FILE_SIZE);
        return HI_FAILURE;
    }

    ret = LOG_FILE_LOCK();
    if (ret != 0) {
        HI_ERR_SYS("down_interruptible failed\n");
        return HI_FAILURE;
    }

    g_default_log_size = size * 1024 * 1024; /* 1024 * 1024 means 1k */

    LOG_FILE_UNLOCK();

    HI_INFO_SYS("set log file size as %d MB\n", g_default_log_size);
    return HI_SUCCESS;
}

hi_s32 hi_drv_log_get_level(hi_u32 module_id, hi_log_level *log_level)
{
    if (g_log_init == HI_FALSE || module_id >= LOG_CFG_BUF_SIZE / sizeof(log_cfg_info) || log_level == HI_NULL) {
        return HI_FAILURE;
    }

    *log_level = (hi_log_level)g_log_cfg_info[module_id].log_level;

    return HI_SUCCESS;
}

static hi_s32 drv_log_cfg_info_init(hi_void)
{
    hi_u32 i;

    g_log_cfg_buf.dmabuf = hi_ion_alloc(LOG_CFG_BUF_SIZE, 1 << ION_HEAP_ID_CMA, 0, "CMN_LogInfo", HI_NULL, 0);
    if (IS_ERR_OR_NULL(g_log_cfg_buf.dmabuf)) {
        HI_ERR_SYS("hi_ion_alloc failed!\n");
        return HI_FAILURE;
    }

    g_log_cfg_buf.virt_addr = (hi_u8 *)dma_buf_kmap(g_log_cfg_buf.dmabuf, 0);
    if (IS_ERR_OR_NULL(g_log_cfg_buf.virt_addr)) {
        hi_ion_free(g_log_cfg_buf.dmabuf);
        HI_ERR_SYS("dma_buf_kmap failed!\n");
        return HI_FAILURE;
    }

    dma_buf_begin_cpu_access(g_log_cfg_buf.dmabuf, DMA_FROM_DEVICE);

    memset(g_log_cfg_buf.virt_addr, 0, LOG_CFG_BUF_SIZE);

    g_log_cfg_info = (log_cfg_info *)g_log_cfg_buf.virt_addr;
    g_log_cfg_info->udisk_flag = 0;

    /* max debug module number: 8192/28 = 341/292 */
    for (i = 0; i < LOG_CFG_BUF_SIZE / sizeof(log_cfg_info); i++) {
        g_log_cfg_info[i].log_level = HI_LOG_LEVEL_DEFAULT;
        g_log_cfg_info[i].log_print_pos = LOG_OUTPUT_SERIAL;
        snprintf(g_log_cfg_info[i].module_name, sizeof(g_log_cfg_info[i].module_name), "Invalid");
    }

    return HI_SUCCESS;
}

static hi_s32 drv_log_cfg_info_exit(hi_void)
{
    if (g_log_cfg_buf.virt_addr != 0) {
        dma_buf_kunmap(g_log_cfg_buf.dmabuf, 0, (hi_void *)g_log_cfg_buf.virt_addr);
        hi_ion_free(g_log_cfg_buf.dmabuf);
        g_log_cfg_info = HI_NULL;
    }

    return HI_SUCCESS;
}

#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
static hi_s32 drv_log_buf_init(hi_u32 size)
{
    memset(&g_log_msg_info, 0, sizeof(g_log_msg_info));

    g_log_msg_info.buf_size = size;

    osal_wait_init(&g_log_msg_info.wq_no_data);

    g_log_msg_buf.dmabuf = hi_ion_alloc(size, 1 << ION_HEAP_ID_CMA, 0, "CMN_LogTrace", HI_NULL, 0);
    if (IS_ERR_OR_NULL(g_log_msg_buf.dmabuf)) {
        HI_ERR_SYS("hi_ion_alloc failed!\n");
        osal_wait_destroy(&g_log_msg_info.wq_no_data);
        return HI_FAILURE;
    }

    g_log_msg_buf.virt_addr = (hi_u8 *)dma_buf_kmap(g_log_msg_buf.dmabuf, 0);
    if (IS_ERR_OR_NULL(g_log_msg_buf.virt_addr)) {
        HI_ERR_SYS("dma_buf_kmap failed!\n");
        hi_ion_free(g_log_msg_buf.dmabuf);
        osal_wait_destroy(&g_log_msg_info.wq_no_data);
        return HI_FAILURE;
    }

    memset(g_log_msg_buf.virt_addr, 0, size);

    g_log_msg_info.start_virt_addr = g_log_msg_buf.virt_addr;

    return HI_SUCCESS;
}

static hi_s32 drv_log_buf_exit(hi_void)
{
    if (g_log_msg_buf.virt_addr != HI_NULL) {
        dma_buf_kunmap(g_log_msg_buf.dmabuf, 0, (hi_void *)g_log_msg_buf.virt_addr);
        hi_ion_free(g_log_msg_buf.dmabuf);
        osal_wait_destroy(&g_log_msg_info.wq_no_data);
        g_log_msg_info.start_virt_addr = HI_NULL;
    }

    return HI_SUCCESS;
}
#endif

static hi_s32 drv_log_get_cfg_buf_fd(hi_mem_handle_t *fd)
{
    hi_s32 ret = HI_FAILURE;
    hi_mem_handle_t buf_fd;

    if (g_log_init == HI_TRUE && g_log_cfg_buf.dmabuf != HI_NULL) {
        buf_fd = hi_dma_buf_fd(g_log_cfg_buf.dmabuf, O_CLOEXEC);
        if (buf_fd < 0) {
            HI_ERR_SYS("hi_dma_buf_fd failed!\n");
            *fd = 0;
        } else {
            *fd = buf_fd;
            ret = HI_SUCCESS;
        }
    } else {
        HI_ERR_SYS("Log is not init!\n");
        *fd = 0;
    }

    return ret;
}

static hi_s32 drv_log_put_cfg_buf_fd(hi_mem_handle_t fd)
{
    if (fd < 0) {
        HI_ERR_SYS("invalid fd!\n");
        return HI_FAILURE;
    }

    hi_close_fd(fd);
    return HI_SUCCESS;
}

hi_s32 drv_log_init(hi_void)
{
    osal_sem_init(&g_log_k_mutex, 1);

    if (drv_log_cfg_info_init() != HI_SUCCESS) {
        osal_sem_destory(&g_log_k_mutex);
        return HI_FAILURE;
    }

#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
    g_log_buf_size = (g_log_buf_size < LOG_MSG_BUF_RESERVE) ? LOG_MSG_BUF_RESERVE : g_log_buf_size;

    if (drv_log_buf_init(g_log_buf_size) != HI_SUCCESS) {
        drv_log_cfg_info_exit();
        osal_sem_destory(&g_log_k_mutex);
        return HI_FAILURE;
    }
#endif

#ifdef LOG_UDISK_SUPPORT
    if (drv_log_udisk_init(g_udisk_log_file) != HI_SUCCESS) {
#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
        drv_log_buf_exit();
#endif
        drv_log_cfg_info_exit();
        osal_sem_destory(&g_log_k_mutex);
        return HI_FAILURE;
    }
#endif

    if (hi_drv_log_add_module("ASYNC", (hi_u32)HI_ID_ASYNC) != HI_SUCCESS) {
#ifdef LOG_UDISK_SUPPORT
        drv_log_udisk_exit();
#endif
#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
        drv_log_buf_exit();
#endif
        drv_log_cfg_info_exit();
        osal_sem_destory(&g_log_k_mutex);
        return HI_FAILURE;
    }

    if (hi_drv_log_add_module("VSYNC", (hi_u32)HI_ID_VSYNC) != HI_SUCCESS) {
        hi_drv_log_remove_module((hi_u32)HI_ID_ASYNC);
#ifdef LOG_UDISK_SUPPORT
        drv_log_udisk_exit();
#endif
#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
        drv_log_buf_exit();
#endif
        drv_log_cfg_info_exit();
        osal_sem_destory(&g_log_k_mutex);
        return HI_FAILURE;

    }

    g_log_init = HI_TRUE;

    return HI_SUCCESS;
}

hi_void drv_log_exit(hi_void)
{
    g_log_init = HI_FALSE;

#ifdef LOG_UDISK_SUPPORT
    if (drv_log_udisk_exit() != HI_SUCCESS) {
        HI_ERR_SYS("drv_log_udisk_exit error!\n");
        osal_sem_destory(&g_log_k_mutex);
        return;
    }
#endif

#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
    if (drv_log_buf_exit() != HI_SUCCESS) {
        HI_ERR_SYS("drv_log_buf_exit error!\n");
        osal_sem_destory(&g_log_k_mutex);
        return;
    }
#endif

    if (drv_log_cfg_info_exit() != HI_SUCCESS) {
        HI_ERR_SYS("drv_log_cfg_info_exit error!\n");
        osal_sem_destory(&g_log_k_mutex);
        return;
    }

    osal_sem_destory(&g_log_k_mutex);
}

#ifndef MODULE
/* Legacy boot options - nonmodular */
static hi_s32 __init drv_log_get_buf_size(hi_char *str)
{
    g_log_buf_size = simple_strtol(str, HI_NULL, 0);
    HI_INFO_SYS("g_log_buf_size = 0x%x\n", g_log_buf_size);
    return 1;
}
__setup("g_log_buf_size=", drv_log_get_buf_size);
#endif

#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
EXPORT_SYMBOL(hi_drv_log_read_buf);
EXPORT_SYMBOL(hi_drv_log_write_buf);
#endif

EXPORT_SYMBOL(hi_drv_log_get_store_path);
EXPORT_SYMBOL(hi_drv_log_get_level);
EXPORT_SYMBOL(hi_log_print);

#ifdef MODULE
module_param(g_log_buf_size, int, S_IRUGO);
module_param(g_udisk_log_file, charp, S_IRUGO);
module_param(g_store_path, charp, S_IRUGO);
EXPORT_SYMBOL(g_store_path);
#endif

#ifdef HI_PROC_SUPPORT
static hi_s32 drv_log_find_module(hi_char *s)
{
    hi_s32 i = 0;
    hi_s32 cnt = LOG_CFG_BUF_SIZE / sizeof(log_cfg_info);

    for (i = 0; i < cnt; i++) {
        if (!strncasecmp(g_log_cfg_info[i].module_name, s, sizeof(g_log_cfg_info[i].module_name))) {
            return i;
        }
    }

    return -1;
}

static hi_s32 drv_log_process_proc_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (g_log_init == HI_FALSE) {
        HI_ERR_SYS("Log is not init!\n");
        return HI_FAILURE;
    }

    osal_printk("To modify the level, use command line in shell: \n");
    osal_printk("    echo loglevel module level > /proc/msp/log\n");
    osal_printk("    level: 0-fatal, 1-error, 2-warning, 3-info\n");
    osal_printk("    example: 'echo loglevel HI_DEMUX 3 > /proc/msp/log'\n");
    osal_printk("    will change log levle of module \"HI_DEMUX\" to 3, then, \n");
    osal_printk("all message with level higher than \"info\" will be printed.\n");
    osal_printk("Use 'echo \"loglevel all x\" > /proc/msp/log' to change all modules.\n");

    osal_printk("\n\nTo modify the log size, use command line in shell: \n");
    osal_printk("Use 'echo \"logsize xxx\" > /proc/msp/log' to set log size as xxx MB.\n");
    osal_printk("Use 'echo \"logsize 12\" > /proc/msp/log' to set log size as 12 MB.\n");
    osal_printk("    example: 'echo logsize 12 > /proc/msp/log'\n");

    osal_printk("\n\nTo modify the log path, use command line in shell: \n");
    osal_printk("Use 'echo \"logpath x\" > /proc/msp/log' to set log path.\n");
    osal_printk("Use 'echo \"logpath /dev/null\" > /proc/msp/log' to close log udisk output.\n");
    osal_printk("    example: 'echo logpath /home > /proc/msp/log'\n");

    osal_printk("\n\nTo modify the debug file store path, use command line in shell: \n");
    osal_printk("Use 'echo \"storepath x\" > /proc/msp/log' to set debug file path.\n");
    osal_printk("    example: 'echo storepath /tmp > /proc/msp/log'\n");

    return HI_SUCCESS;
}

static hi_s32 drv_log_process_proc_level(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_char *module = HI_NULL;
    hi_u8 level;
    hi_s32 idx;

    if (g_log_init == HI_FALSE) {
        HI_ERR_SYS("Log is not init!\n");
        return HI_FAILURE;
    }
    if (argc != 3) { /* the number of required paras is 3 */
        HI_ERR_SYS("argc is invalid!\n");
        return HI_FAILURE;
    }

    module = argv[1]; /* the first para(1) is module name */
    level = argv[2][0] - '0'; /* the second para(2) is log level */
    if (level >= HI_LOG_LEVEL_MAX) {
        HI_ERR_SYS("level is invalid!\n");
        return HI_FAILURE;
    }

    if (!strncasecmp("all", module, sizeof("all"))) {
        hi_s32 i = 0;
        hi_u32 total = LOG_CFG_BUF_SIZE / sizeof(log_cfg_info);
        for (i = 0; i < total; i++) {
            g_log_cfg_info[i].log_level = level;
        }
        return HI_SUCCESS;
    }

    idx = drv_log_find_module(module);
    if (idx == -1) {
        HI_ERR_SYS("%s is not registered!\n", module);
        return HI_FAILURE;
    }
    g_log_cfg_info[idx].log_level = level;

    return HI_SUCCESS;
}

static hi_s32 drv_log_process_proc_size(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 log_size = 0;

    if (g_log_init == HI_FALSE) {
        HI_ERR_SYS("Log is not init!\n");
        return HI_FAILURE;
    }
    if (argc != 2) { /* the number of required paras is 2 */
        HI_ERR_SYS("argc is invalid!\n");
        return HI_FAILURE;
    }

    if (strstr(argv[1], "0x") || strstr(argv[1], "0X")) {
        log_size = simple_strtoul(argv[1], HI_NULL, 16); /* the first para(1) is log size, 16 base conversion */
    } else {
        log_size = simple_strtoul(argv[1], HI_NULL, 10); /* the first para(1) is log size, 10 base conversion */
    }

    if (log_size > LOG_MAX_FILE_SIZE || log_size < LOG_MIN_FILE_SIZE) {
        HI_ERR_SYS("size is invalid!\n");
        return HI_FAILURE;
    }

    return hi_drv_log_set_size(log_size);
}

static hi_s32 drv_log_process_proc_path(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_char *log_path = HI_NULL;

    if (g_log_init == HI_FALSE) {
        HI_ERR_SYS("Log is not init!\n");
        return HI_FAILURE;
    }
    if (argc != 2) { /* the number of required paras is 2 */
        HI_ERR_SYS("argc is invalid!\n");
        return HI_FAILURE;
    }

    log_path = argv[1]; /* the first para(1) is log path */

    if (strlen(log_path) >= LOG_MAX_FILE_NAME_LENTH) {
        HI_ERR_SYS("log path is invalid!\n");
        return HI_FAILURE;
    }

    return hi_drv_log_set_path(log_path, strlen(log_path));
}

static hi_s32 drv_log_process_proc_store_path(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
    hi_void *private)
{
    hi_char *store_path = HI_NULL;

    if (g_log_init == HI_FALSE) {
        HI_ERR_SYS("Log is not init!\n");
        return HI_FAILURE;
    }
    if (argc != 2) { /* the number of required paras is 2 */
        HI_ERR_SYS("argc is invalid!\n");
        return HI_FAILURE;
    }

    store_path = argv[1]; /* the first para(1) is store path */

    if (strlen(store_path) >= LOG_MAX_FILE_NAME_LENTH) {
        HI_ERR_SYS("store path is invalid!\n");
        return HI_FAILURE;
    }

    return hi_drv_log_set_store_path(store_path, strlen(store_path));
}

static osal_proc_cmd g_proc_cmd[] = {
    {"help",      drv_log_process_proc_help},
    {"loglevel",  drv_log_process_proc_level},
    {"logsize",   drv_log_process_proc_size},
    {"logpath",   drv_log_process_proc_path},
    {"storepath", drv_log_process_proc_store_path}
};

hi_s32 drv_log_read_proc(hi_void *s, hi_void *arg)
{
    hi_u32 i;
    hi_u8 level;
    hi_char path[LOG_MAX_FILE_NAME_LENTH] = {0};
    hi_u32 cnt = LOG_CFG_BUF_SIZE / sizeof(log_cfg_info);

    if (g_log_init == HI_FALSE) {
        osal_proc_print(s, "Log is not init!\n");
        return HI_SUCCESS;
    }

    if (hi_drv_log_get_path(path, sizeof(path)) != HI_SUCCESS) {
        HI_ERR_SYS("hi_drv_log_get_path failed!\n");
        return HI_FAILURE;
    }

    osal_proc_print(s, "---------------- Log Path ------------------------\n");
    osal_proc_print(s, "log path:  %s\n", path);

    if (hi_drv_log_get_store_path(path, sizeof(path)) != HI_SUCCESS) {
        HI_ERR_SYS("hi_drv_log_get_store_path failed!\n");
        return HI_FAILURE;
    }

    osal_proc_print(s, "---------------- Store Path ----------------------\n");
    osal_proc_print(s, "store path:  %s\n", path);

    osal_proc_print(s, "---------------- Module Log Level ----------------\n");
    osal_proc_print(s, "Log module\t  Level\n");
    osal_proc_print(s, "--------------------------\n");

    for (i = 0; i < cnt; i++) {
        if (strncmp(g_log_cfg_info[i].module_name, "Invalid", 0x8)) {
            level = g_log_cfg_info[i].log_level;
            osal_proc_print(s, "%-16s  %d(%s)\n", g_log_cfg_info[i].module_name, level, g_debug_level_name[level]);
        }
    }

    osal_proc_print(s, "\necho loglevel HI_AVPLAY 2 > /proc/msp/log\n");
    osal_proc_print(s, "echo logsize XXX > /proc/msp/log\n");
    osal_proc_print(s, "    (the unit is MB, current logsize is (%d)MB, range(%d(MB) ~ %d(MB)))\n",
        (g_default_log_size / 1024 / 1024), LOG_MIN_FILE_SIZE, LOG_MAX_FILE_SIZE); /* divide twice 1024 to MB */
    osal_proc_print(s, "echo logpath /mnt > /proc/msp/log\n");
    osal_proc_print(s, "echo storepath /mnt > /proc/msp/log\n");

    return HI_SUCCESS;
}

hi_s32 drv_log_add_proc(hi_void)
{
    osal_proc_entry *item = HI_NULL;

    item = osal_proc_add(HI_MOD_LOG_NAME, strlen(HI_MOD_LOG_NAME));
    if (item == HI_NULL) {
        HI_ERR_SYS("add proc failed!\n");
        return HI_FAILURE;
    }

    item->read = drv_log_read_proc;
    item->cmd_cnt = sizeof(g_proc_cmd) / sizeof(osal_proc_cmd);
    item->cmd_list = g_proc_cmd;

    return HI_SUCCESS;
}

hi_void drv_log_remove_proc(hi_void)
{
    osal_proc_remove(HI_MOD_LOG_NAME, strlen(HI_MOD_LOG_NAME));
}
#endif

hi_s32 drv_log_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    switch (cmd) {
        case LOG_CMPI_INIT: {
            hi_mem_handle_t *fd = (hi_mem_handle_t *)arg;

            return drv_log_get_cfg_buf_fd(fd);
        }
        case LOG_CMPI_EXIT: {
            hi_mem_handle_t *fd = (hi_mem_handle_t *)arg;

            return drv_log_put_cfg_buf_fd(*fd);
        }
#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
        case LOG_CMPI_READ: {
            log_read_buf *para = (log_read_buf *)arg;

            if (para->msg_addr == HI_NULL) {
                HI_ERR_SYS("User buffer is null!\n");
                return HI_FAILURE;
            }
            if (para->magic_num != LOG_MAGIC_NUM) {
                HI_ERR_SYS("User buffer is likely illegal!\n");
                return HI_FAILURE;
            }

            return hi_drv_log_read_buf(para->msg_addr, para->buf_len, &para->copy_len, HI_FALSE);
        }
        case LOG_CMPI_WRITE: {
            log_write_buf *para = (log_write_buf *)arg;

            if (para->msg_addr == HI_NULL) {
                HI_ERR_SYS("User buffer is null!\n");
                return HI_FAILURE;
            }

            return hi_drv_log_write_buf(para->msg_addr, para->msg_len, LOG_MSG_FROM_USER);
        }
#endif
        case LOG_CMPI_SET_PATH: {
            hi_log_path *log_path = (hi_log_path *)arg;

            return drv_log_set_path(log_path->path, log_path->len, HI_TRUE);
        }
        case LOG_CMPI_SET_STORE_PATH: {
            hi_log_store_path *store_path = (hi_log_store_path *)arg;

            return drv_log_set_store_path(store_path->path, store_path->len, HI_TRUE);
        }
        case LOG_CMPI_SET_SIZE: {
            hi_u32 *size = (hi_u32 *)arg;

            return hi_drv_log_set_size(*size);
        }
        default:
            HI_ERR_SYS("unknown command 0x%x\n", cmd);
            return HI_FAILURE;
    }
}

#ifdef CONFIG_COMPAT
hi_s32 drv_log_compat_ioctl(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    switch (cmd) {
#if defined(LOG_NETWORK_SUPPORT) || defined(LOG_UDISK_SUPPORT)
        case LOG_CMPI_COMPAT_READ: {
            log_compat_read_buf *para = (log_compat_read_buf *)arg;

            if ((hi_u8 *)compat_ptr(para->msg_addr) == HI_NULL) {
                HI_ERR_SYS("User buffer is null!\n");
                return HI_FAILURE;
            }
            if (para->magic_num != LOG_MAGIC_NUM) {
                HI_ERR_SYS("User buffer is likely illegal!\n");
                return HI_FAILURE;
            }

            return hi_drv_log_read_buf((hi_u8 *)compat_ptr(para->msg_addr), para->buf_len, &para->copy_len, HI_FALSE);
        }
        case LOG_CMPI_COMPAT_WRITE: {
            log_compat_write_buf *para = (log_compat_write_buf *)arg;

            if ((hi_u8 *)compat_ptr(para->msg_addr) == HI_NULL) {
                HI_ERR_SYS("User buffer is null!\n");
                return HI_FAILURE;
            }

            return hi_drv_log_write_buf((hi_u8 *)compat_ptr(para->msg_addr), para->msg_len, LOG_MSG_FROM_USER);
        }
#endif
        case LOG_CMPI_COMPAT_SET_PATH: {
            hi_log_compat_path *log_path = (hi_log_compat_path *)arg;

            return drv_log_set_path((hi_char *)compat_ptr(log_path->path), log_path->len, HI_TRUE);
        }
        case LOG_CMPI_COMPAT_SET_STORE_PATH: {
            hi_log_compat_store_path *store_path = (hi_log_compat_store_path *)arg;

            return drv_log_set_store_path((hi_char *)compat_ptr(store_path->path), store_path->len, HI_TRUE);
        }
        default:
            return drv_log_ioctl(cmd, arg, private_data);
    }
    return -ENOIOCTLCMD;
}
#endif

