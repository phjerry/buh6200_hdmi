/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2005-2018. All rights reserved.
 * Description: Common data types of the system.
 *              CNcomment: 系统共用的数据类型定义 CNend
 * Author: Hisilicon multimedia software group
 * Create: 2005-4-23
 */

#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__

#if defined(__KERNEL__)
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36))
#define DECLARE_MUTEX DEFINE_SEMAPHORE
#endif
#endif

/*--------------------------------------------------------------------------------------------------------------*
 * Defintion of basic data types. The data types are applicable to both the application layer and kernel codes. *
 * CNcomment: 基本数据类型定义，应用层和内核代码均使用  CNend                                                   *
 *--------------------------------------------------------------------------------------------------------------*/
/*************************** Structure Definition ****************************/
/** \addtogroup      Common_TYPE */
/** @{ */  /** <!-- [Common_TYPE] */

/** Constant Definition */
/** CNcomment: 常量定义 */
typedef enum {
    HI_FALSE    = 0,
    HI_TRUE     = 1,
} hi_bool;

#ifndef NULL
#define NULL                0L
#endif

#define HI_NULL             0L
#define HI_NULL_PTR         0L

#define HI_SUCCESS          0
#define HI_FAILURE          (-1)

#define HI_INVALID_HANDLE   (0xffffffff)

#define HI_INVALID_PTS      (0xffffffff)
#define HI_INVALID_TIME     (0xffffffff)

#define HI_OS_LINUX 0xabcd
#define HI_OS_WIN32 0xcdef

#ifdef _WIN32
#define HI_OS_TYPE HI_OS_WIN32
#else
#define __OS_LINUX__
#define HI_OS_TYPE HI_OS_LINUX
#endif

#ifdef HI_ADVCA_SUPPORT
#define __INIT__
#define __EXIT__
#else
#define __INIT__ __init
#define __EXIT__ __exit
#endif

typedef unsigned char           hi_uchar;
typedef unsigned char           hi_u8;
typedef unsigned short          hi_u16;
typedef unsigned int            hi_u32;
typedef unsigned long long      hi_u64;
typedef unsigned long           hi_ulong;

typedef char                    hi_char;
typedef signed char             hi_s8;
typedef short                   hi_s16;
typedef int                     hi_s32;
typedef long long               hi_s64;
typedef long                    hi_slong;

typedef float                   hi_float;
typedef double                  hi_double;

typedef void                    hi_void;

typedef unsigned long           hi_size_t;
typedef unsigned long           hi_length_t;

typedef unsigned long           hi_uintptr_t;

typedef hi_u32                  hi_handle;

typedef unsigned long long      hi_mem_size_t;
typedef long long               hi_mem_handle_t;

typedef struct {
    hi_mem_handle_t mem_handle; /* Handle fo buffer header address */ /* CNcomment: 内存句柄，模块之间传递的唯一标识 */
    hi_mem_size_t addr_offset;  /* buffer offset */ /* CNcomment: 地址偏移 */
} hi_mem_handle;

#ifdef CONFIG_ARCH_LP64_MODE
typedef unsigned int hi_phys_addr_t;
typedef unsigned long long hi_virt_addr_t;
#else
typedef unsigned int hi_phys_addr_t;
typedef unsigned int hi_virt_addr_t;
#endif

/*
 * define of HI_HANDLE :
 * bit31                                                                bit0
 *   |<----- 8bit ----->|<----- 8bit ----->|<---------- 16bit ---------->|
 *   |-------------------------------------------------------------------|
 *   |     hi_mod_id    | mod defined data |           chan_id           |
 *   |-------------------------------------------------------------------|
 * mod defined data: private data define by each module(for example: sub-mod id), usually, set to 0.
 */
#define HI_HANDLE_INIT(mod, private_data, chan_id) (hi_handle)((((mod) & 0xff) << 24) | ((((private_data) & 0xff) << 16)) | (((chan_id) & 0xffff)))

#define HI_HANDLE_GET_MODULE_ID(handle)    (((handle) >> 24) & 0xff)
#define HI_HANDLE_GET_PRIVATE_DATA(handle) (((handle) >> 16) & 0xff)
#define HI_HANDLE_GET_CHAN_ID(handle)      (((handle)) & 0xffff)

#define HI_UNUSED(x) ((x)=(x))

#endif /* __HI_TYPE_H__ */

