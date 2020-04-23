/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_cmp_frm_nr.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */
#ifndef __HAL_VPSS_CMP_FRM_NR_H__
#define __HAL_VPSS_CMP_FRM_NR_H__

#include "hal_vpss_ice_define.h"
#include "hal_vpss_reg.h"
#include "hal_vpss_cmp_hqv4_config_2ppc.h"

hi_void vpss_mac_set_dcmp_frm_nr(vpss_reg_type *reg, ice_frm_cfg *ice_cfg);
hi_void vpss_mac_set_cmp_frm_nr(vpss_reg_type *reg, ice_frm_cfg *ice_cfg);
hi_void vpss_ice_set_dcmp_frm_alone_y(vpss_reg_type *reg, dcmp_hqv4_alone_parm_t *cmp_parm, hi_bool dcmp_en);
hi_void vpss_ice_set_dcmp_frm_alone_c(vpss_reg_type *reg, dcmp_hqv4_alone_parm_t *cmp_parm, hi_bool dcmp_en);
hi_void vpss_ice_set_cmp_frm_alone_y(vpss_reg_type *reg, cmp_hqv4_alone_parm_t *cmp_parm, hi_bool cmp_en);
hi_void vpss_ice_set_cmp_frm_alone_c(vpss_reg_type *reg, cmp_hqv4_alone_parm_t *cmp_parm, hi_bool cmp_en);

#endif

