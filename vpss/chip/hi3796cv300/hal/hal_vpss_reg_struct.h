/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_reg_struct.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __HAL_VPSS_REG_STRUCT_H__
#define __HAL_VPSS_REG_STRUCT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VPSS_REG_PORT0 = 0,   /* port 0; main logic port */
    VPSS_REG_PORT1,       /* 3d detect port; npu port */
    VPSS_REG_PORT_VIR,    /* vir port */

    VPSS_REG_PORT_MAX
} vpss_reg_port;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





