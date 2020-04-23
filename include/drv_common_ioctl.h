/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2007-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2007-1-31
 */

#ifndef  __DRV_SYS_IOCTL_H__
#define  __DRV_SYS_IOCTL_H__

#include "hi_debug.h"
#include "hi_type.h"

#include "hi_drv_sys.h"
#include "hi_drv_module.h"
#include "hi_drv_stat.h"

/* macro definition of module */
#define MODULE_NAME_MAX_LEN 16


/* macro definition of log */
#define LOG_MAX_TRACE_LEN 256
#define LOG_CFG_BUF_SIZE (1024 * 16)

#define LOG_SECOND (1000)
#define LOG_MINUTE (60 * LOG_SECOND)
#define LOG_HOUR   (60 * LOG_MINUTE)
#define LOG_DAY    (24 * LOG_HOUR)

#define LOG_MAGIC_NUM 0x5A5AA5A5 /* define the LOG_MAGIC_NUM to ensure the userbuffer is correct */

#ifdef ANDROID
#define LOG_PRINT_FORMAT "[%s-%s]:%s[%d]:%s", g_debug_level_name[level], \
    g_log_cfg_info[module_id].module_name, fn_name, line_num, log_str
#else
#define LOG_PRINT_FORMAT "[%02u:%02u:%02u:%03u %s-%s]:%s[%d]:%s", hour, minute, second, millisecond, \
    g_debug_level_name[level], g_log_cfg_info[module_id].module_name, fn_name, line_num, log_str
#endif


/* macro definition of user_proc */
#define HI_FATAL_UPROC(fmt...) HI_FATAL_PRINT(HI_ID_PROC, fmt)
#define HI_ERR_UPROC(fmt...)   HI_ERR_PRINT(HI_ID_PROC, fmt)
#define HI_WARN_UPROC(fmt...)  HI_WARN_PRINT(HI_ID_PROC, fmt)
#define HI_INFO_UPROC(fmt...)  HI_INFO_PRINT(HI_ID_PROC, fmt)

#define HI_USER_PROC_MAX_NAME_LEN 127
#define HI_USER_PROC_MAX_CMD_LEN  255
#define HI_USER_PROC_BUF_SIZE     4096

#define HI_USER_PROC_DEV_NAME HI_DEV_PREFIX "userproc"
#define HI_USER_PROC_READ_CMD "__read"


/* struct definition of sys */
typedef struct {
    hi_u32 base_addr;
    hi_u32 reg_value;
} hi_sys_io_reg;

typedef struct {
    hi_s32 tm_sec;
    hi_s32 tm_min;
    hi_s32 tm_hour;
    hi_s32 tm_mday;
    hi_s32 tm_mon;
    hi_s32 tm_year;
} hi_sys_build_time;

typedef struct {
    hi_chip_type    chip_type;
    hi_chip_version chip_version;
    hi_char         version[80];
    hi_char         boot_version[80];
    hi_char         sos_version[96];
} hi_sys_version;

typedef struct {
    hi_u64 chip_id_64;
    hi_u32 chip_id_32;
} hi_sys_chip_id;

/* struct definition of stat */
typedef struct {
    hi_stat_event event;
    hi_u32        value;
} stat_event_info;


/* struct definition of module */
typedef struct {
    hi_u32  module_id;
    hi_char module_name[MODULE_NAME_MAX_LEN];
} module_info;


/* struct definition of log */
typedef struct {
    hi_u8 module_name[16 + 12]; /* module name 16 + '_' 1 + pid 10 */
    hi_u8 log_level;            /* log level */
    hi_u8 log_print_pos;        /* log output location, 0: serial port, 1: network, 2: u-disk */
    hi_u8 udisk_flag;           /* u-disk log flag */
} log_cfg_info;                 /* structure of module log level */

typedef struct {
    hi_u8 *msg_addr;
    hi_u32 buf_len;
    hi_u32 copy_len;
    hi_u32 magic_num;
} log_read_buf;

typedef struct {
    hi_u32 msg_addr;
    hi_u32 buf_len;
    hi_u32 copy_len;
    hi_u32 magic_num;
} log_compat_read_buf;

typedef struct {
    hi_u8 *msg_addr;
    hi_u32 msg_len;
} log_write_buf;

typedef struct {
    hi_u32 msg_addr;
    hi_u32 msg_len;
} log_compat_write_buf;

typedef struct {
    hi_char *path;
    hi_u32   len;
} hi_log_path;

typedef struct {
    hi_u32 path;
    hi_u32 len;
} hi_log_compat_path;

typedef struct {
    hi_char *path;
    hi_u32   len;
} hi_log_store_path;

typedef struct {
    hi_u32 path;
    hi_u32 len;
} hi_log_compat_store_path;


/* struct definition of user_proc */
typedef struct {
    hi_u8 *buf;
    hi_u32 size;
    hi_u32 offset;
} hi_user_proc_show_buf;

typedef struct {
    hi_u32 buf;
    hi_u32 size;
    hi_u32 offset;
} hi_user_proc_compat_show_buf;

typedef hi_s32 (*hi_user_proc_show)(hi_user_proc_show_buf *show_buf, hi_void *private_data);
typedef hi_s32 (*hi_user_proc_cmd)(hi_user_proc_show_buf *show_buf, hi_u32 argc, hi_u8 *argv[], hi_void *private_data);

typedef struct {
    hi_char          *entry_name;
    hi_char          *dir_name;
    hi_user_proc_show show_proc;
    hi_user_proc_cmd  cmd_proc;
    hi_void          *private_data;
} hi_user_proc_entry;

typedef enum {
    HI_USER_PROC_PARENT_DEFAULT, /* default is /proc/hisi/ */
    HI_USER_PROC_PARENT_MSP
} hi_user_proc_parent_dir;

typedef struct {
    hi_char dir_name[HI_USER_PROC_MAX_NAME_LEN + 1];
    hi_user_proc_parent_dir parent;
} hi_user_proc_dir_info;

typedef struct {
    hi_char           name[HI_USER_PROC_MAX_NAME_LEN + 1];
    hi_char           parent_dir[HI_USER_PROC_MAX_NAME_LEN + 1];
    hi_user_proc_show show_func;
    hi_user_proc_cmd  cmd_func;
    hi_void          *private_data;
    hi_void          *entry;
    hi_void          *file;
    hi_void          *read;
    hi_void          *write;
} hi_user_proc_entry_info;

typedef struct {
    hi_char name[HI_USER_PROC_MAX_NAME_LEN + 1];
    hi_char parent_dir[HI_USER_PROC_MAX_NAME_LEN + 1];
    hi_u32  show_func;
    hi_u32  cmd_func;
    hi_u32  private_data;
    hi_u32  entry;
    hi_u32  file;
    hi_u32  read;
    hi_u32  write;
} hi_user_proc_compat_entry_info;

typedef struct {
    hi_void *entry; /* The type is hi_user_proc_entry_info* */
    hi_s32   write;
    hi_char  cmd[HI_USER_PROC_MAX_CMD_LEN + 1];
} hi_user_proc_cmd_info;

typedef struct {
    hi_u32  entry;  /* The type is hi_user_proc_entry_info* */
    hi_s32  write;
    hi_char cmd[HI_USER_PROC_MAX_CMD_LEN + 1];
} hi_user_proc_compat_cmd_info;

typedef struct {
    hi_user_proc_cmd_info   cmd_info;
    hi_user_proc_entry_info entry_info;
} hi_user_proc_info;

typedef struct {
    hi_user_proc_compat_cmd_info   cmd_info;
    hi_user_proc_compat_entry_info entry_info;
} hi_user_proc_compat_info;

typedef struct {
    hi_u32 mem_size;
    hi_u32 mmz_size;
} sys_mem_info;

typedef struct {
    hi_sys_policy_type policy_type;
    hi_sys_policy policy_info;
} hi_sys_policy_info;

/* ioctl cmd mask */
#define COMMON_CMD_MASK         0xE0
#define COMMON_SYS_CMD          0x20
#define COMMON_STAT_CMD         0x40
#define COMMON_MODULE_CMD       0x60
#define COMMON_LOG_CMD          0x80
#define COMMON_USER_PROC_CMD    0xA0

/* ioctl cmd of sys */
#define SYS_GET_VERSION                 _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x00, hi_sys_version)
#define SYS_GET_TIME_STAMP_MS           _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x01, hi_u32)
#define SYS_GET_DOLBY_SUPPORT           _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x02, hi_u32)
#define SYS_GET_DTS_SUPPORT             _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x03, hi_u32)
#define SYS_GET_ADVCA_SUPPORT           _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x04, hi_u32)
#define SYS_GET_MACRO_VISION_SUPPORT    _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x05, hi_u32)
#define SYS_GET_HDR10_SUPPORT           _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x06, hi_u32)
#define SYS_GET_DOLBY_VISION_SUPPORT    _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x07, hi_u32)
#define SYS_GET_DDR_CFG                 _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x08, sys_mem_info)
#define SYS_GET_DIE_ID_64               _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x09, hi_u64)
#define SYS_GET_DIE_ID_32               _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x0A, hi_u32)
#define SYS_GET_CHIP_PACKAGE_TYPE       _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x0B, hi_u32)
#define SYS_GET_SMP_SUPPORT             _IOR (HI_ID_SYS, COMMON_SYS_CMD + 0x0C, hi_bool)
#define SYS_DISABLE_VIDEO_CODEC         _IOW (HI_ID_SYS, COMMON_SYS_CMD + 0x0D, hi_sys_video_codec)
#define SYS_WRITE_REGISTER              _IOW (HI_ID_SYS, COMMON_SYS_CMD + 0x0E, hi_sys_io_reg)
#define SYS_READ_REGISTER               _IOWR(HI_ID_SYS, COMMON_SYS_CMD + 0x0F, hi_sys_io_reg)
#define SYS_SET_POLICY_INFO             _IOW (HI_ID_SYS, COMMON_SYS_CMD + 0x10, hi_sys_policy_info)
#define SYS_GET_POLICY_INFO             _IOWR(HI_ID_SYS, COMMON_SYS_CMD + 0x11, hi_sys_policy_info)

/* ioctl cmd of stat */
#define STAT_CMPI_EVENT                 _IOW (HI_ID_STAT, COMMON_STAT_CMD + 0x00, stat_event_info)
#define STAT_CMPI_LD_EVENT              _IOW (HI_ID_STAT, COMMON_STAT_CMD + 0x01, hi_stat_ld_event_info)

/* ioctl cmd of module */
#define MODULE_ADD_INFO                 _IOWR(HI_ID_MODULE, COMMON_MODULE_CMD + 0x00, module_info)
#define MODULE_DEL_INFO                 _IOW (HI_ID_MODULE, COMMON_MODULE_CMD + 0x01, hi_u32)

/* ioctl cmd of log */
#define LOG_CMPI_INIT                   _IOR (HI_ID_LOG, COMMON_LOG_CMD + 0x00, hi_mem_handle_t)
#define LOG_CMPI_EXIT                   _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x01, hi_mem_handle_t)
#define LOG_CMPI_READ                   _IOWR(HI_ID_LOG, COMMON_LOG_CMD + 0x02, log_read_buf)
#define LOG_CMPI_COMPAT_READ            _IOWR(HI_ID_LOG, COMMON_LOG_CMD + 0x02, log_compat_read_buf)
#define LOG_CMPI_WRITE                  _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x03, log_write_buf)
#define LOG_CMPI_COMPAT_WRITE           _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x03, log_compat_write_buf)
#define LOG_CMPI_SET_PATH               _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x04, hi_log_path)
#define LOG_CMPI_COMPAT_SET_PATH        _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x04, hi_log_compat_path)
#define LOG_CMPI_SET_STORE_PATH         _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x05, hi_log_store_path)
#define LOG_CMPI_COMPAT_SET_STORE_PATH  _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x05, hi_log_compat_store_path)
#define LOG_CMPI_SET_SIZE               _IOW (HI_ID_LOG, COMMON_LOG_CMD + 0x06, hi_u32)

/* ioctl cmd of user_proc */
#define USER_PROC_ADD_ENTRY              _IOWR(HI_ID_PROC, COMMON_USER_PROC_CMD + 0x00, hi_user_proc_entry_info)
#define USER_PROC_ADD_COMPAT_ENTRY       _IOWR(HI_ID_PROC, COMMON_USER_PROC_CMD + 0x00, hi_user_proc_compat_entry_info)
#define USER_PROC_REMOVE_ENTRY           _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x01, hi_user_proc_entry_info)
#define USER_PROC_REMOVE_COMPAT_ENTRY    _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x01, hi_user_proc_compat_entry_info)
#define USER_PROC_ADD_DIR                _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x03, hi_user_proc_dir_info)
#define USER_PROC_REMOVE_DIR             _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x04, hi_user_proc_dir_info)
#define USER_PROC_GET_CMD                _IOR (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x05, hi_user_proc_info)
#define USER_PROC_COMPAT_GET_CMD         _IOR (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x05, hi_user_proc_compat_info)
#define USER_PROC_WAKE_READ_TASK         _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x06, hi_user_proc_show_buf)
#define USER_PROC_COMPAT_WAKE_READ_TASK  _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x06, hi_user_proc_compat_show_buf)
#define USER_PROC_WAKE_WRITE_TASK        _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x07, hi_user_proc_show_buf)
#define USER_PROC_COMPAT_WAKE_WRITE_TASK _IOW (HI_ID_PROC, COMMON_USER_PROC_CMD + 0x07, hi_user_proc_compat_show_buf)

#endif  /* __DRV_SYS_IOCTL_H__ */

