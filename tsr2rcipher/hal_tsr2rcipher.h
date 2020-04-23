/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher hw hal function decl.
 */

#ifndef __HAL_TSR2RCIPHER_H__
#define __HAL_TSR2RCIPHER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_tsr2rcipher_define.h"

/* TSR2RCIPHER RX hal level functions. */

hi_void tsc_hal_rx_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type);
hi_void tsc_hal_rx_de_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_rx_set_buf(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 src_buf_addr, hi_u32 src_buf_len);
hi_bool tsc_hal_rx_get_dsp_tor_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_rx_set_buf_addr(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 src_addr);
hi_void tsc_hal_rx_set_buf_len(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u32 src_len);
hi_void tsc_hal_rx_add_dsc(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_rx_set_ctrl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type, hi_u32 rx_pkg_th);
hi_void tsc_hal_rx_set_int(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_bool dsc_rd, hi_bool rx_pkt);
hi_u32  tsc_hal_rx_get_dsc_rd_total_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_rx_get_pkt_cnt_total_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_rx_get_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_rx_cls_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_u32  tsc_hal_rx_get_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_rx_cls_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_rx_get_dsc_rd_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array);
hi_void tsc_hal_rx_get_pkt_cnt_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array);

hi_void tsc_hal_set_mode_ctl(struct tsr2rcipher_mgmt *mgmt, struct tsr2rcipher_r_ch *rch,
                             enum tsc_crypt_type crypt_type);

hi_void tsc_hal_en_mode_ctl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_dis_mode_ctl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);


/* TSR2RCIPHER TX hal level functions. */

hi_void tsc_hal_tx_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type);
hi_void tsc_hal_tx_de_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_tx_set_buf(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 dst_buf_addr, hi_u32 dst_buf_len);
hi_bool tsc_hal_tx_get_dsp_tor_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_tx_set_buf_addr(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 dst_addr);
hi_void tsc_hal_tx_set_buf_len(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u32 dst_len);
hi_void tsc_hal_tx_add_dsc(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_tx_set_ctrl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type, hi_u32 tx_pkg_th);
hi_void tsc_hal_tx_set_int(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_bool dsc_rd, hi_bool tx_pkt);
hi_u32  tsc_hal_tx_get_dsc_rd_total_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_tx_get_pkt_cnt_total_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_tx_get_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_tx_cls_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_u32  tsc_hal_tx_get_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_tx_cls_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id);
hi_void tsc_hal_tx_get_dsc_rd_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array);
hi_void tsc_hal_tx_get_pkt_cnt_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array);

/* TSR2RCIPHER TOP hal level functions. */
hi_void tsc_hal_top_set_int(struct tsr2rcipher_mgmt *mgmt, hi_bool rx_int, hi_bool tx_int, hi_bool cipher_int);
hi_u32  tsc_hal_top_get_rx_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_top_get_rx_raw_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_void tsc_hal_top_cls_rx_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_top_get_tx_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_top_get_tx_raw_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_void tsc_hal_top_cls_tx_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_top_get_cipher_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_u32  tsc_hal_top_get_cipher_raw_int_status(struct tsr2rcipher_mgmt *mgmt);
hi_void tsc_hal_top_cls_cipher_int_status(struct tsr2rcipher_mgmt *mgmt);

/* empty */
hi_void tsc_hal_init_hw(struct tsr2rcipher_mgmt *mgmt);
hi_void tsc_hal_de_init_hw(struct tsr2rcipher_mgmt *mgmt);

hi_void tsc_hal_mdscset_en(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_bool ca_en, hi_bool ca_chan_scr_en);

#ifdef HI_SMMU_SUPPORT
hi_void tsc_hal_en_mmu(struct tsr2rcipher_mgmt *mgmt);
hi_void tsc_hal_dis_mmu(struct tsr2rcipher_mgmt *mgmt);
#else
static inline hi_void tsc_hal_en_mmu(struct tsr2rcipher_mgmt *mgmt) {}
static inline hi_void tsc_hal_dis_mmu(struct tsr2rcipher_mgmt *mgmt) {}
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_TSR2RCIPHER_H__ */
