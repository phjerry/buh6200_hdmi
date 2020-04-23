/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: klad basic utils impl.
* Author: SDK team
* Create: 2019-09-17
*/

#ifndef __DRV_KLAD_HW_UTILS_H__
#define __DRV_KLAD_HW_UTILS_H__

#include "drv_klad_hw_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32  klad_hw_session_create(struct klad_hw_session **new_hw_session);
hi_s32  klad_hw_session_add_slot(struct klad_hw_session *hw_session, struct klad_r_base *slot);
hi_s32  klad_hw_session_del_slot(struct klad_hw_session *hw_session, struct klad_r_base *slot);
hi_s32  klad_hw_session_destroy(struct klad_hw_session *hw_session);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_KLAD_HW_UTILS_H__

