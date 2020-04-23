/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc_nrrfr.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __HAL_VPSS_WBC_NRRFR_H__
#define __HAL_VPSS_WBC_NRRFR_H__

#include "vpss_comm.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_WBC_NRRFR_MAX_NODE 5

typedef struct {
    hi_drv_vpss_video_frame wbc_frame;  // 帧信息
    list node;                     // 链表节点
} vpss_wbc_nrrfr_data;

typedef enum {
    VPSS_WBC_NRRFR_MODE_NORMAL = 2,  // 回写帧模式，回写队列开辟2个帧存节点
    VPSS_WBC_NRRFR_MODE_3FIELD = 3,  // 回写三场模式，回写队列开辟3个帧存节点
    VPSS_WBC_NRRFR_MODE_4FIELD = 4,  // 回写三场模式，回写队列开辟4个帧存节点
    VPSS_WBC_NRRFR_MODE_5FIELD = 5,  // 回写五场模式，回写队列开辟5个帧存节点
    VPSS_WBC_NRRFR_MODE_MAX
} vpss_wbc_nrrfr_mode;

typedef enum {
    /* 获取参考帧时，当回写帧次数不够时，未回写的参考帧赋值为初始值，
    回写的参考帧正常赋值 */
    VPSS_WBC_NRRFR_REF_MODE_INIT = 1,
    /* 获取参考帧时，当回写帧次数不够时，未回写的参考帧赋值为无效，
    回写的参考帧正常赋值 */
    VPSS_WBC_NRRFR_REF_MODE_NULL = 2,
    VPSS_WBC_NRRFR_REF_MODE_MAX
} vpss_wbc_nrrfr_ref_mode;

typedef struct {
    hi_u32 width;                      // 帧信息宽
    hi_u32 height;                     // 帧信息高
    hi_drv_pixel_format pixel_format;  // 帧信息像素格式
    hi_drv_compress_info cmp_info;
    hi_drv_pixel_bitwidth bit_width;  // 帧信息图像bit位,10 bit or 8 bit
} vpss_wbc_nrrfr_calc;

typedef struct {
    vpss_wbc_nrrfr_mode mode;          // 回写队列工作模式
    vpss_wbc_nrrfr_ref_mode ref_mode;  // 模式值使用模式
    hi_u32 width;                      // 帧信息宽
    hi_u32 height;                     // 帧信息高
    hi_bool interlace;
    hi_drv_pixel_format pixel_format;  // 帧信息像素格式
    hi_drv_compress_info cmp_info;
    hi_drv_pixel_bitwidth bit_width;  // 帧信息图像bit位,10 bit or 8 bit
    hi_u8 *mem_name;                  // 内存的名称
    hi_bool secure;
} vpss_wbc_nrrfr_attr;

typedef struct {
    hi_bool init;                                            // 初始化标识
    hi_u32 complete_count;                                   // 回写链表轮转次数
    vpss_wbc_nrrfr_data data_list[VPSS_WBC_NRRFR_MAX_NODE];  // 回写链表结构
    vpss_wbc_nrrfr_attr wbc_attr;                            // 回写链表属性
    drv_vpss_mem_info vpss_mem;                                // 帧存信息
    list *first_ref;                                         // 指向链表第一个参考帧
} vpss_wbc_nrrfr;

hi_s32 vpss_wbc_nrrfr_init(vpss_wbc_nrrfr *wbc, vpss_wbc_nrrfr_attr *in_attr);

hi_s32 vpss_wbc_nrrfr_deinit(vpss_wbc_nrrfr *wbc);

hi_s32 vpss_wbc_nrrfr_get_ref_info(vpss_wbc_nrrfr *wbc, hi_drv_vpss_video_frame **data_list);

hi_s32 vpss_wbc_nrrfr_get_wbc_info(vpss_wbc_nrrfr *wbc, hi_drv_vpss_video_frame **data);

hi_s32 vpss_wbc_nrrfr_complete(vpss_wbc_nrrfr *wbc);

hi_s32 vpss_wbc_nrrfr_reset(vpss_wbc_nrrfr *wbc);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





