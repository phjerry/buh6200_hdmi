/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Error number definition.
 */

#ifndef __HI_ERROR_H__
#define __HI_ERROR_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Error type definition
 * CNcomment:错误类型定义
 */
#define ERR_TYPE_UNKNOWN                        0x00
#define ERR_TYPE_INIT                           0x01
#define ERR_TYPE_DEINIT                         0x02
#define ERR_TYPE_OPEN                           0x03
#define ERR_TYPE_CLOSE                          0x04
#define ERR_TYPE_CREATE                         0x05
#define ERR_TYPE_DESTORY                        0x06
#define ERR_TYPE_CONFIG                         0x07
#define ERR_TYPE_ATTACH                         0x08
#define ERR_TYPE_DETACH                         0x09
#define ERR_TYPE_ENABLE                         0x0a
#define ERR_TYPE_DISABLE                        0x0b
#define ERR_TYPE_START                          0x0c
#define ERR_TYPE_STOP                           0x0d
#define ERR_TYPE_ALLOC                          0x0e
#define ERR_TYPE_RELEASE                        0x0f
#define ERR_TYPE_FIND                           0x10
#define ERR_TYPE_FILE                           0x11

/*
 * Error code definition
 * CNcomment: 错误码定义
 */
#define ERR_CODE_UNKNOWN                        0x00

/*
 * 0x01-0x1F: API call process error
 * CNcomment: 0x00-0x1F 调用流程错误
 */
#define ERR_CODE_NOT_INIT                       0x01
#define ERR_CODE_NOT_DEINIT                     0x02
#define ERR_CODE_NOT_OPEN                       0x03
#define ERR_CODE_NOT_CLOSE                      0x04
#define ERR_CODE_NOT_DESTORY                    0x05
#define ERR_CODE_NOT_CONFIG                     0x06
#define ERR_CODE_NOT_ATTACH                     0x07
#define ERR_CODE_NOT_DETACH                     0x08
#define ERR_CODE_NOT_ENABLE                     0x09
#define ERR_CODE_NOT_DISABLE                    0x0a
#define ERR_CODE_NOT_START                      0x0b
#define ERR_CODE_NOT_STOP                       0x0c

/*
 * 0x20-0x9F: ordinary error
 * CNcomment: 0x20-0x9F 具体错误
 */
#define ERR_CODE_PARA_INVALID                   0x20 /* 参数非法 */
#define ERR_CODE_PTR_NULL                       0x21 /* 参数空指针 */
#define ERR_CODE_ID_NOT_EXIST                   0x22 /* 没有创建的实例，直接使用 */
#define ERR_CODE_ID_EMPTY                       0x23 /* 超过最大实例数目，分配不出实例 */
#define ERR_CODE_ID_INVALID                     0x24 /* 实例号非法，超过最大值，或者ID构造非法 */
#define ERR_CODE_FILE_NOT_EXIST                 0x25 /* 文件或设备不存在 */
#define ERR_CODE_OPEN_FAILED                    0x26 /* 文件或设备打开失败 */
#define ERR_CODE_CLOSE_FAILED                   0x27 /* 文件或设备关闭失败 */
#define ERR_CODE_WRITE_FAILED                   0x28 /* 文件或设备写失败 */
#define ERR_CODE_READ_FAILED                    0x29 /* 文件或设备读失败 */
#define ERR_CODE_SEEK_FAILED                    0x2a /* 文件或设备seek失败 */
#define ERR_CODE_NOT_SUPPORT                    0x2b /* 不支持的操作 */
#define ERR_CODE_MEM_ALLOC_FAILED               0x2c /* 内存分配失败 */
#define ERR_CODE_MAP_FAILED                     0x2d /* 映射设备或内存失败 */
#define ERR_CODE_BUFFER_EMPTY                   0x2e /* 缓冲区空，没有足够的内存或资源轮转 */
#define ERR_CODE_BUFFER_FULL                    0x2f /* 缓冲区满，导致无法送入更多数据或数据溢出 */
#define ERR_CODE_DEVICE_BUSY                    0x30 /* 设备正在工作，不能打断 */
#define ERR_CODE_OPERATION_TIMEOUT              0x31 /* 操作超时 */

/* 0xA0-0xE0: Special error code difined by modules */
#define ERR_CODE_RECREATE_ENTRY                 0xA0
#define ERR_CODE_RECREATE_DIR                   0xA1

/* get error code from error number */
#define ERR_CODE_OF(err_no) (hi_u8)((err_no) & 0xff)

/* get error type from error number */
#define ERR_TYPE_OF(err_no) (hi_u8)(((err_no) & 0xff00) >> 8)

/*
 * Used inner
 * Error number format: 0x80MMTTCC
 * MM: 8bits module ID, defined in "hi_module.h"
 * TT: 8bits error type, defined as ERR_TYPE_XXX
 * CC: 8bits error code, defined as ERR_CODE_XXX
 * For example: ERR_NUMBER(HI_ID_DISP, ERR_TYPE_INIT_FAILED, ERR_CODE_PARA_INVALID)
 */
#define ERR_NUMBER(module, type, code) (hi_s32)((((((0x80 << 8) | (module)) << 8) | (type)) << 8) | (code))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of __HI_ERROR_H__ */

