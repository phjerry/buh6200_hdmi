/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq param table define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __DRV_PQ_TABLE_H__
#define __DRV_PQ_TABLE_H__

#include "hi_drv_pq.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32 start_pos;
    hi_u32 end_pos;
} hi_pq_bin_module_pos;

hi_s32 pq_table_deinit_pqtable(hi_void);

hi_s32 pq_table_init_pqtable(pq_bin_param *pq_param, hi_bool pq_para_use_table);

hi_s32 pq_table_load_phy_list(pq_reg_type reg_type, pq_source_mode source_mode, pq_output_mode output_mode,
                              hi_u32 handle_id);

hi_s32 pq_table_load_multi_list(pq_reg_type reg_type, pq_source_mode source_mode, pq_output_mode output_mode);

hi_s32 pq_table_init_phy_list(hi_u32 id, hi_pq_module_type module, pq_source_mode source_mode,
                              pq_output_mode output_mode);

hi_u32 pq_table_get_soft_table(hi_u32 lut, pq_source_mode source_mode, pq_output_mode output_mode,
                               hi_u32 default_value);

hi_s32 pq_table_find_bin_pos(hi_pq_module_type module, hi_u32 *start_pos, hi_u32 *end_pos);

hi_s32 pq_table_load_alg_phy_list(hi_pq_module_type module, pq_source_mode source_mode,
                                  pq_output_mode output_mode);

hi_s32 pq_table_init_soft_table(hi_bool init);

hi_s32 pq_table_set_table_reg_default(hi_void);

hi_u32 pq_table_get_phy_list_num(hi_void);

hi_u32 pq_table_get_soft_list_num(hi_void);

hi_s32 pq_table_set_sel_vdp_reg(hi_u32 addr, hi_u8 lsb, hi_u8 msb, hi_u32 value);

hi_s32 pq_table_set_vpss_reg(hi_u32 addr, hi_u8 lsb, hi_u8 msb, hi_u32 value);

hi_s32 pq_table_set_vdp_reg(hi_u64 addr, hi_u8 lsb, hi_u8 msb, hi_u32 value);

pq_bin_phy_reg *pq_table_get_phy_reg(hi_void);

pq_bin_phy_reg *pq_table_get_soft_reg(hi_void);

hi_s32 pq_table_search_reg_value(pq_mixed_key_reg *mixed_key_reg_array, hi_u32 array_len, hi_u64 key, hi_u32 *value);
pq_mixed_key_reg *pq_table_get_mixed_key_reg_array(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
