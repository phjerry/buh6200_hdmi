/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio hw hal function definition.
* Author: guoqingbo 00277009
* Create: 2016-09-07
*/

#ifndef __HAL_TSIO_H__
#define __HAL_TSIO_H__

#include "drv_tsio_define.h"
#include "drv_tsio_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32  tsio_hal_init_ctrl(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_de_init_ctrl(const struct tsio_mgmt *mgmt);

hi_void tsio_hal_init_phy(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_de_init_phy(const struct tsio_mgmt *mgmt);

struct tsio_en_pidch_para {
    hi_u32 id;
    hi_u32 pid;
    hi_u32 port_id;
    hi_u32 sid;
    hi_bool is_live_ts;
};

hi_void tsio_hal_en_pid_channel(const struct tsio_mgmt *mgmt, const struct tsio_en_pidch_para *para);
hi_void tsio_hal_dis_pid_channel(const struct tsio_mgmt *mgmt, hi_u32 id);

hi_void tsio_hal_en_tsi_port(const struct tsio_mgmt *mgmt, hi_u32 id,
    hi_u32 dvb_port_id, tsio_live_port_type port_type);
hi_void tsio_hal_dis_tsi_port(const struct tsio_mgmt *mgmt, hi_u32 id);

hi_void tsio_hal_send_ccout(const struct tsio_mgmt *mgmt, hi_u32 cclen);
hi_void tsio_hal_set_ccslot(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 ccdata);
hi_void tsio_hal_get_ccresp_len(const struct tsio_mgmt *mgmt, hi_u32 *ccresp_len);
hi_void tsio_hal_get_ccslot(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 *ccdata);
hi_void tsio_hal_recv_ccdone(const struct tsio_mgmt *mgmt);
hi_u32  tsio_hal_get_cconflict_status(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_cconflict_status(const struct tsio_mgmt *mgmt);

hi_void tsio_hal_en_stuff_srv(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_void tsio_hal_en2dmx_srv(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 dmx_port_id);
hi_void tsio_hal_dis2dmx_srv(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_void tsio_hal_en2ram_srv(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_void tsio_hal_dis2ram_srv(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_void tsio_hal_en_sp_save(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_void tsio_hal_dis_sp_save(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_u32  tsio_hal_get_srv_pkt_count(const struct tsio_mgmt *mgmt, hi_u32 id);

/* general int definition. */
#define TSIO_TX_CC_DONE_INT_MASK             (0x1 << 0)
#define TSIO_RX_CC_DONE_INT_MASK             (0x1 << 1)
#define TSIO_RX_CTS_INT_MASK                 (0x1 << 2)
#define TSIO_RX_ROUTE_FIFO_OVFL_INT_MASK     (0x1 << 24)
#define TSIO_RX_PARSER_ERR_INT_MASK          (0x1 << 28)
#define TSIO_TX_RR_ERR_INT_MASK              (0x1 << 29)
#define TSIO_PID_FILTER_ERR_INT_MASK         (0x1 << 30)
#define TSIO_DMA_INT_MASK                    (0x1 << 31)

/* RX_PARSER_ERR sub int definition. */
#define TSIO_RX_PHY_SP_ERR_INT_MASK          (0x1 << 0)
#define TSIO_RX_FIFO_OVFL_INT_MASK           (0x1 << 1)
#define TSIO_RX_SP_SYNC_ERR_INT_MASK         (0x1 << 3)
#define TSIO_RX_SP_RFU0_ERR_INT_MASK         (0x1 << 4)
#define TSIO_RX_SP_DMA_END_ERR_INT_MASK      (0x1 << 5)
#define TSIO_RX_SP_ENCRY_EN_ERR_INT_MASK     (0x1 << 6)
#define TSIO_RX_SP_TSID_ERR_INT_MASK         (0x1 << 7)
#define TSIO_RX_SP_SOC_DEFINE_ERR_INT_MASK   (0x1 << 8)
#define TSIO_RX_SP_RFU1_ERR_INT_MASK         (0x1 << 9)
#define TSIO_RX_SP_RFU2_ERR_INT_MASK         (0x1 << 10)
#define TSIO_RX_SP_STUFF_LOAD_ERR_INT_MASK   (0x1 << 11)
#define TSIO_RX_SP_SC_GEN_ERR_INT_MASK       (0x1 << 12)
#define TSIO_RX_CC_ERR_INT_MASK              (0x1 << 16 | 0x1 << 17 | 0x1 << 18 | 0x1 << 19)

/* TX_RR_ERR sub int definition. */
#define TSIO_TX_LIVE0_BFIFO_OVFL_INT_MASK    (0x1 << 0)
#define TSIO_TX_LIVE1_BFIFO_OVFL_INT_MASK    (0x1 << 1)
#define TSIO_TX_LIVE2_BFIFO_OVFL_INT_MASK    (0x1 << 2)
#define TSIO_TX_LIVE3_BFIFO_OVFL_INT_MASK    (0x1 << 3)
#define TSIO_TX_LIVE4_BFIFO_OVFL_INT_MASK    (0x1 << 4)
#define TSIO_TX_LIVE5_BFIFO_OVFL_INT_MASK    (0x1 << 5)
#define TSIO_TX_LIVE6_BFIFO_OVFL_INT_MASK    (0x1 << 6)
#define TSIO_TX_LIVE7_BFIFO_OVFL_INT_MASK    (0x1 << 7)
#define TSIO_TX_FIFO_EMPTY_INT_MASK          (0x1 << 16)
#define TSIO_CPU_WRITE_CC_CONFLICT_INT_MASK  (0x1 << 24)
#define TSIO_TX_CC_LENGTH_ERR_INT_MASK       (0x1 << 25)

/* PID_FILTER_ERR sub int definition. */
#define TSIO_DVB0_FIFO_OVFL_INT_MASK         (0x1 << 0)
#define TSIO_DVB1_FIFO_OVFL_INT_MASK         (0x1 << 1)
#define TSIO_DVB2_FIFO_OVFL_INT_MASK         (0x1 << 2)
#define TSIO_DVB3_FIFO_OVFL_INT_MASK         (0x1 << 3)
#define TSIO_DVB4_FIFO_OVFL_INT_MASK         (0x1 << 4)
#define TSIO_DVB5_FIFO_OVFL_INT_MASK         (0x1 << 5)
#define TSIO_DVB6_FIFO_OVFL_INT_MASK         (0x1 << 6)
#define TSIO_DVB7_FIFO_OVFL_INT_MASK         (0x1 << 7)

/* DMA sub int definition. */
#define TSIO_DMA_CHNL_PEND_INT_MASK          (0x1 << 1)
#define TSIO_DMA_PACK_INT_MASK               (0x1 << 2)
#define TSIO_DMA_OBUF_NR_FULL_INT_MASK       (0x1 << 3)
#define TSIO_DMA_END_INT_MASK                (0x1 << 4)
#define TSIO_DMA_FLUSH_INT_MASK              (0x1 << 5)
#define TSIO_DMA_DES_END_INT_MASK            (0x1 << 6)
#define TSIO_DMA_BID_INT_MASK                (0x1 << 16)
#define TSIO_DMA_LIVE_OFLW_ERR_INT_MASK      (0x1 << 17)
#define TSIO_DMA_CHK_CODE_ERR_INT_MASK       (0x1 << 18)
#define TSIO_DMA_OBUF_OFLW_ERR_INT_MASK      (0x1 << 19)
#define TSIO_DMA_DES_TYPE_ERR_INT_MASK       (0x1 << 20)
#define TSIO_DMA_ICHL_WPTR_OFLW_ERR_INT_MASK (0x1 << 21)
#define TSIO_DMA_OVERFLOW_ERR_INT_MASK       (0x1 << 24)
#define TSIO_DMA_TLB_ERR_INT_MASK            (0x1 << 25)
#define TSIO_DMA_BUS_ERR_INT_MASK            (0x1 << 26)

hi_void tsio_hal_en_all_int(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_dis_all_int(const struct tsio_mgmt *mgmt);

hi_void tsio_hal_en_phy_int(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_dis_phy_int(const struct tsio_mgmt *mgmt);

hi_void tsio_hal_mask_all_dma_int(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_un_mask_all_dma_int(const struct tsio_mgmt *mgmt);

hi_u32  tsio_hal_get_int_flag(const struct tsio_mgmt *mgmt);

hi_void tsio_hal_clr_tx_cc_done_int(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_rx_cc_done_int(const struct tsio_mgmt *mgmt);

hi_void tsio_hal_clr_rx_cts_int(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_rx_route_fifo_ovfl_int(const struct tsio_mgmt *mgmt);

hi_u32  tsio_hal_get_org_rx_parser_err_int_flag(const struct tsio_mgmt *mgmt);
hi_u32  tsio_hal_get_rx_parser_err_int_flag(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_rx_parser_err_int_flag(const struct tsio_mgmt *mgmt, hi_u32 flag);

hi_u32  tsio_hal_get_tx_rr_err_int_flag(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_tx_rr_err_int_flag(const struct tsio_mgmt *mgmt, hi_u32 flag);

hi_u32  tsio_hal_get_pid_filter_err_int_flag(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_pid_filter_err_int_flag(const struct tsio_mgmt *mgmt, hi_u32 flag);

hi_u32  tsio_hal_get_dma_int_flag(const struct tsio_mgmt *mgmt);

hi_u32  tsio_hal_get_dma_des_end_status(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_des_end_status(const struct tsio_mgmt *mgmt, hi_u32 status);
hi_u32  tsio_hal_get_dma_chn_pend_status(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_chn_pend_status(const struct tsio_mgmt *mgmt, hi_u32 status);

hi_u32  tsio_hal_get_dma_pack_int_status_l(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_pack_int_status_l(const struct tsio_mgmt *mgmt, hi_u32 status);
hi_u32  tsio_hal_get_dma_pack_int_status_h(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_pack_int_status_h(const struct tsio_mgmt *mgmt, hi_u32 status);

hi_u32  tsio_hal_get_dma_end_int_status_l(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_end_int_status_l(const struct tsio_mgmt *mgmt, hi_u32 status);
hi_u32  tsio_hal_get_dma_end_int_status_h(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_end_int_status_h(const struct tsio_mgmt *mgmt, hi_u32 status);

hi_u32  tsio_hal_get_dma_obuf_ovflw_status_l(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_obuf_ovflw_status_l(const struct tsio_mgmt *mgmt, hi_u32 status);
hi_u32  tsio_hal_get_dma_obuf_ovflw_status_h(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_obuf_ovflw_status_h(const struct tsio_mgmt *mgmt, hi_u32 status);

hi_u32  tsio_hal_get_dma_flush_status_l(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_flush_status_l(const struct tsio_mgmt *mgmt, hi_u32 status);
hi_u32  tsio_hal_get_dma_flush_status_h(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_clr_dma_flush_status_h(const struct tsio_mgmt *mgmt, hi_u32 status);
hi_void tsio_hal_set_dma_cnt_unit(const struct tsio_mgmt *mgmt);

hi_void tsio_hal_en_mmu(const struct tsio_mgmt *mgmt);
hi_void tsio_hal_en_ram_port(const struct tsio_mgmt *mgmt, hi_u32 id,
    hi_u64 dsc_phy_addr, hi_u32 dsc_depth, hi_u32 pace);
hi_void tsio_hal_dis_ram_port(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_bool tsio_hal_ram_port_enabled(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_void tsio_hal_set_ram_port_rate(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 pace);

struct tsio_dsc_base_info {
    hi_u32 id;
    hi_u64 buf_phy_addr;
    hi_u32 pkt_cnt;
};

hi_void tsio_hal_setup_ts_dsc(const struct tsio_mgmt *mgmt, const struct tsio_dsc_base_info *base_info,
                              hi_bool desepon, hi_u32 *cur_dsc_addr);
hi_void tsio_hal_setup_bulk_dsc(const struct tsio_mgmt *mgmt, const struct tsio_dsc_base_info *base_info,
                                hi_bool desepon, hi_u32 *cur_dsc_addr, hi_u32 sid);
hi_void tsio_hal_setup_bulk_flush_dsc(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 *cur_dsc_addr, hi_u32 sid);
hi_void tsio_hal_add_dsc(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 write_idx);
hi_u32  tsio_hal_get_ram_port_cur_read_idx(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_u32  tsio_hal_get_ram_port_cur_write_idx(const struct tsio_mgmt *mgmt, hi_u32 id);

hi_void tsio_hal_en_obuf(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u64 buf_phy_addr, hi_u32 buf_size);
hi_void tsio_hal_dis_obuf(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_u32  tsio_hal_get_obuf_write(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_u32  tsio_hal_get_obuf_read(const struct tsio_mgmt *mgmt, hi_u32 id);
hi_void tsio_hal_set_obuf_read(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 cur_read);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __HAL_TSIO_H__

