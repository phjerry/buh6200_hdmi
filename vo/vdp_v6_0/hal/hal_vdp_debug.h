/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_DEBUG_H__
#define __HAL_VDP_DEBUG_H__

#include "hi_type.h"

#define VDP_HAL_PRINT(fmt, arg...)      printk(fmt, ##arg);
#define VDP_HAL_INFO(fmt, arg...)       printk("info:"fmt, ##arg);
#define VDP_HAL_ERR(fmt, arg...)       printk("error:"fmt, ##arg);

#define VDP_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            VDP_HAL_PRINT("\nASSERT failed at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
                __FUNCTION__, __LINE__, #expr); \
        } \
    } while (0)

#endif

