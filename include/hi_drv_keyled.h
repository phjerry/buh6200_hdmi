#ifndef  __HI_DRV_KEYLED_H__
#define  __HI_DRV_KEYLED_H__

#include "hi_unf_keyled.h"

#include "hi_debug.h"
#include "hi_module.h"

#define HI_FATAL_KEYLED(fmt...) HI_FATAL_PRINT(HI_ID_KEYLED, fmt)

#define HI_ERR_KEYLED(fmt...)   HI_ERR_PRINT(HI_ID_KEYLED, fmt)

#define HI_WARN_KEYLED(fmt...)  HI_WARN_PRINT(HI_ID_KEYLED, fmt)

#define HI_INFO_KEYLED(fmt...)  HI_INFO_PRINT(HI_ID_KEYLED, fmt)

#define KEYLED_ADDR_SIZE    0X1000


#endif  /*  __HI_DRV_KEYLED_H__ */
