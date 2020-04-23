/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LNB base function
 * Author: SDK
 * Created: 2017-06-30
 */
#ifndef __DRV_LNBCTRL_H__
#define __DRV_LNBCTRL_H__

#include "drv_frontend.h"

typedef struct lnbctrl_dev {
    hi_u32                  tuner_port;
    hi_u32                  i2c_num;
    hi_u8                   dev_addr;
    hi_bool                 inited;
    frontend_lnb_out_level     lnb_out;
    struct lnbctrl_dev*     next;
} lnbctrl_dev_param;

lnbctrl_dev_param* lnbctrl_queue_insert(hi_u32 tuner_port, hi_u32 i2c_num, hi_u8 dev_addr);
lnbctrl_dev_param* lnbctrl_queue_get(hi_u32 tuner_port);
hi_void            lnbctrl_queue_remove(hi_u32 tuner_port);
hi_s32 lnb_read_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 *reg_val);
hi_s32 lnb_write_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 reg_val);

hi_s32 drv_fe_adp_lnb_none_regist_func(drv_fe_lnb_ops *lnb_ops);

#endif /* __DRV_LNBCTRL_H__ */

