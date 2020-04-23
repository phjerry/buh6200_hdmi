/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_define.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __HAL_VPSS_DEFINE_H__
#define __HAL_VPSS_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define VPSS_WOUTSTANDING 15
#define VPSS_ROUTSTANDING 31
#define VPSS_INT_MASK 0xA74
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
