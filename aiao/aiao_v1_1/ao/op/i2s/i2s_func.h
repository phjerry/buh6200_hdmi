/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port i2s include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __I2S_FUNC_H__
#define __I2S_FUNC_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#if defined(HI_SND_I2S_SUPPORT)
hi_void i2s_op_register_driver(struct osal_list_head *head);
#else
#define i2s_op_register_driver(head)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __I2S_FUNC_H__ */

