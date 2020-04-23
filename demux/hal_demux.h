/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux hw hal function decl.
 * Author: sdk
 * Create: 2017-06-05
 */

#ifndef __HAL_DEMUX_H__
#define __HAL_DEMUX_H__

#include "drv_demux_define.h"
#include "drv_demux_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void dmx_hal_init_hw(hi_void);
hi_void dmx_hal_deinit_hw(hi_void);
#ifdef DMX_SMMU_SUPPORT
hi_void dmx_hal_en_mmu(struct dmx_mgmt *mgmt);
hi_void dmx_hal_dis_mmu(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_buf_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_pid_copy_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 pcid);
hi_void dmx_hal_ram_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 id);
#else
static inline hi_void dmx_hal_en_mmu(const struct dmx_mgmt *mgmt) {}
static inline hi_void dmx_hal_dis_mmu(const struct dmx_mgmt *mgmt) {}
static inline hi_void dmx_hal_buf_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 id) {}
static inline hi_void dmx_hal_pid_copy_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 pcid) {}
static inline hi_void dmx_hal_ram_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 id) {}
#endif

/*
 * general int definition for demux begin.
 */
#define DMX_INF2CPU_INT_MASK                (0x1 << 0)
#define DMX_SWH2CPU_INT_MASK                (0x1 << 1)
#define DMX_PAR2CPU_INT_MASK                (0x1 << 2)
#define DMX_SCD2CPU_INT_MASK                (0x1 << 3)
#define DMX_FLT2CPU_INT_MASK                (0x1 << 4)
#define DMX_DAV2CPU_INT_MASK                (0x1 << 5)
#define DMX_DMX_INT_MASK                    (0x1 << 8)  /* band */

/* PAR2CPU sub int definition. */
#define DMX_PAR_TEI_INT_MASK                (0x1U << 0)
#define DMX_PAR_CC_INT_MASK                 (0x1U << 16)
#define DMX_PAR_DSC_INT_MASK                (0x1U << 24)

/* DAV2CPU sub int definition. */
#define DMX_DAV_TIMEOUT_INT_MASK            (0x1U << 0)  /*  */
#define DMX_DAV_TS_BUF_INT_MASK             (0x1U << 1)
#define DMX_DAV_SEOP_INT_MASK               (0x1U << 2)
#define DMX_DAV_PRS_OVFL_INT_MASK           (0x1U << 3)
#define DMX_DAV_PC_OVFL_INT_MASK            (0x1U << 4)
#define DMX_DAV_FQ_RD_INT_MASK              (0x1U << 5)
#define DMX_DAV_FQ_CHECK_FAILED_INT_MASK    (0x1U << 6)
#define DMX_DAV_AP_INT_MASK                 (0x1U << 7)
#define DMX_DAV_PRS_CLRCHN_INT_MASK         (0x1U << 8)
#define DMX_DAV_IP_CLRCHN_INT_MASK          (0x1U << 9)
#define DMX_DAV_IP_SYNC_INT_FLG_MASK        (0x1U << 10)
#define DMX_DAV_IP_LOSS_INT_FLG_MASK        (0x1U << 11)
#define DMX_DAV_PC_CLRCHN_INT_FLG_MASK      (0x1U << 12)
#define DMX_DAV_PC_AP_INT_FLG_MASK          (0x1U << 13)


hi_void dmx_hal_en_all_int(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_dis_all_int(const struct dmx_mgmt *mgmt);
hi_u32  dmx_hal_get_int_flag(const struct dmx_mgmt *mgmt);

/* dav begin */
hi_void dmx_hal_mask_all_dav_int(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_un_mask_all_dav_int(const struct dmx_mgmt *mgmt);
hi_u32  dmx_hal_get_dav_int_flag(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_clr_dav_timeout_int_flag(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_clr_dav_ts_int_flag(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_clr_dav_seop_int_flag(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_clr_dav_prs_ovfl_int_flag(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_clr_dav_ip_loss_int_status(const struct dmx_mgmt *mgmt);

/* demux port hal level functions begin. */
hi_void dmx_hal_dvb_port_set_share_clk(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, dmx_port share_clk_port);
hi_void dmx_hal_dvb_port_open(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id);
hi_void dmx_hal_dvb_port_close(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id);
hi_void dmx_hal_dvb_port_set_inf_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_bool is_serial,
    dmx_sync_mode sync_mode, hi_bool force_valid, hi_bool clk_mode);
hi_void dmx_hal_dvb_port_set_inf_sub_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_u32 fifo_rate,
    hi_u32 sync_on, hi_u32 sync_off);
hi_void dmx_hal_dvb_port_set_serial_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id,
    dmx_serial_bit_mode bit_mode, hi_u32 bit_select);
hi_void dmx_hal_dvb_port_set_parallel_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_u32 bit_select);
hi_void dmx_hal_dvb_port_set_clk_in_pol(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_bool tuner_in_clk);
hi_void dmx_hal_dvb_set_ts_cnt_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_u32 ctrl_value);
hi_u32  dmx_hal_dvb_get_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id);
hi_u32  dmx_hal_dvb_get_err_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id);
hi_u32  dmx_hal_dvb_get_sync_err_cnt(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id);
hi_void dmx_hal_dvb_port_set_sync_len(const struct dmx_mgmt *mgmt, hi_u32 tsi_raw_id, hi_u32 tag_len);
hi_void dmx_hal_dvb_port_set_dummy_force(const struct dmx_mgmt *mgmt, hi_u32 tsi_raw_id, hi_bool dummy_force);
hi_void dmx_hal_dvb_port_set_dummy_sync(const struct dmx_mgmt *mgmt, hi_u32 tsi_raw_id, hi_bool dummy_sync);

/* tag port function */
typedef struct {
    hi_u32 low;
    hi_u32 mid;
    hi_u32 high;
} dmx_tag_value;

typedef struct {
    hi_bool enable;
    hi_u32 tsi_raw_id;
    hi_u32 sync_mode;
    hi_u32 tag_len;
} dmx_tag_dual_ctrl;

hi_void dmx_hal_tag_port_set_attr(const struct dmx_mgmt *mgmt, hi_u32 tag_dual_id, hi_u32 tag_idx,
    const dmx_tag_value *tag_value);
hi_void dmx_hal_tag_port_set_ctrl(const struct dmx_mgmt *mgmt, hi_u32 tag_dual_id,
    const dmx_tag_dual_ctrl *tag_dual_ctrl);
hi_void dmx_hal_tag_port_set_clk_gt(const struct dmx_mgmt *mgmt, hi_u32 tag_dual_id, hi_bool clk_gt_en);

/* ramport begin */
hi_u32  dmx_hal_ram_port_get_dsc_rd_int_status(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_ram_port_cls_dsc_rd_int_status(const struct dmx_mgmt *mgmt, hi_u32 status);
hi_u32  dmx_hal_ram_port_get_dsc_tread_int_status(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_ram_port_cls_dsc_tread_int_status(const struct dmx_mgmt *mgmt, hi_u32 status);

/*
 * DEMUX RAM port hal level functions begin.
 */
hi_void dmx_hal_ram_port_set_desc(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_phy_addr, hi_u32 dsc_depth);
hi_void dmx_hal_ram_port_set_out_int(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable);
hi_void dmx_hal_ram_port_config_rate(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rate);
hi_void dmx_hal_ram_port_enable_mmu(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_ram_port_start_stream(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable);
hi_void dmx_hal_ram_port_set_dsc(const struct dmx_mgmt *mgmt, hi_u32 id, struct dmx_ram_port_dsc *dsc);
hi_void dmx_hal_ram_port_add_dsc(const struct dmx_mgmt *mgmt, const hi_u32 id, const hi_u32 desc_num);
hi_u32  dmx_hal_ram_port_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_ram_port_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_bool dmx_hal_ram_port_enabled(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_ram_port_set_check_code(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_ram_port_clr_port(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_ram_port_set_sync_len(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 ram_len1, hi_u32 ram_len2);
hi_void dmx_hal_ram_port_set_ipcfg(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 sync_type, hi_u32 sync_th,
                                   hi_u32 lost_th, hi_bool sync_inten, hi_bool loss_inten);
hi_void dmx_hal_ram_port_set_vir_ram_port(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_ram_port_cls_vir_ram_port(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_ram_port_cls_ap_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_ram_port_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_phy_addr, hi_u32 dsc_depth,
                                hi_u32 rate);
hi_void dmx_hal_ram_port_de_config(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_ram_port_ip_crc_en(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32 dmx_hal_ram_port_get_ts_packets_cnt(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32 dmx_hal_ram_port_get_ip_loss_int_status(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_ram_port_cls_ip_loss_int_status(const struct dmx_mgmt *mgmt, hi_u32 status);

/*
 * DEMUX RmxFct hal level functions begin.
 */
hi_void dmx_hal_rmx_set_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_port_id, enum dmx_port_type_e port_mode,
                             hi_u32 port_id);
hi_void dmx_hal_rmx_unset_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_port_id);
hi_void dmx_hal_rmx_set_port_all_pass(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_port_id);
hi_void dmx_hal_rmx_un_set_port_all_pass(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_port_id);
hi_void dmx_hal_rmx_en_detect_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_port_id);
hi_void dmx_hal_rmx_dis_detect_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_port_id);
hi_u32  dmx_hal_rmx_get_port_overflow_count(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_port_id);
hi_void dmx_hal_rmx_set_pid_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pump_id, hi_u32 rmx_port_id, hi_u32 pid);
hi_void dmx_hal_rmx_set_remap_pid_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pump_id, hi_u32 rmx_port_id,
                                      hi_u32 pid, hi_u32 new_pid);
hi_void dmx_hal_rmx_un_set_pid_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pump_id);

/*
 * DEMUX SWH(Band) hal level functions end.
 */
enum dmx_pcr_src_type_e {
    DMX_PCR_SRC_TYPE_BAND       =    0,
    DMX_PCR_SRC_TYPE_PORT       =    1,
    DMX_PCR_SRC_TYPE_BUTT
};

struct dmx_hal_pcr_info {
    const struct dmx_mgmt *mgmt;
    hi_u32 pcr_id;
    hi_u32 pcr_pid;
    enum dmx_pcr_src_type_e pcr_src;
    enum dmx_port_type_e port_type;
    hi_u32 raw_port_id;
    hi_u32 band;
};

hi_void dmx_hal_band_en_all_tsrec(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_band_dis_all_tsrec(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_band_attach_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 port_type, hi_u32 port_id);
hi_void dmx_hal_band_detach_port(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_band_get_qam_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 if_id);
hi_void dmx_hal_band_set_pcr_scr(const struct dmx_mgmt *mgmt, hi_bool pcr_scr_en);
hi_void dmx_hal_band_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 id);

hi_void dmx_hal_pcr_int_en(const struct dmx_mgmt *mgmt, hi_u32 pcr_id);
hi_void dmx_hal_pcr_int_dis(const struct dmx_mgmt *mgmt, hi_u32 pcr_id);
hi_u32  dmx_hal_pcr_get_int_status(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_pcr_cls_int_status(const struct dmx_mgmt *mgmt, hi_u32 status);

hi_void dmx_hal_pcr_en(const struct dmx_hal_pcr_info *pcr_info);
hi_void dmx_hal_pcr_dis(const struct dmx_mgmt *mgmt, const hi_u32 pcr_id);
hi_void dmx_hal_pcr_get_pcr_value(const struct dmx_mgmt *mgmt, const hi_u32 pcr_id, hi_u64 *pcr_val);
hi_void dmx_hal_pcr_get_scr_value(const struct dmx_mgmt *mgmt, const hi_u32 pcr_id, hi_u64 *scr_val);
hi_void dmx_hal_pcr_get_cur_scr(const struct dmx_mgmt *mgmt, hi_u32 *scr_clk);

/*
* DEMUX PAR hal level functions begin.
*/
hi_u32 dmx_hal_get_par_int_type(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_par_set_cc_int(const struct dmx_mgmt *mgmt, hi_bool cc_en);
hi_bool dmx_hal_par_check_cc_int(const struct dmx_mgmt *mgmt);
hi_u32 dmx_hal_par_get_cc_int_status(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_par_cls_cc_int_status(const struct dmx_mgmt *mgmt, hi_u32 status);
hi_u32 dmx_hal_par_get_tei_int_status(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_par_cls_tei_int_status(const struct dmx_mgmt *mgmt, hi_u32 status);

/*
 * DEMUX SCD hal level functions begin.
 */
hi_void dmx_hal_scd_set_tts_27m_en(const struct dmx_mgmt *mgmt, hi_bool tts_27m_en);
hi_void dmx_hal_scd_set_ts_rec_cfg(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ts_rec_en, hi_u32 buf_id);
hi_void dmx_hal_scd_set_av_pes_cfg(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 chan_en, hi_u32 mode,
                                   hi_u32 pesh_idx_ena);
hi_u64 dmx_hal_scd_get_rec_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 rec_id);
hi_void dmx_hal_scd_set_buf_id(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id);
hi_void dmx_hal_scd_en(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool scd_en);
hi_void dmx_hal_scd_set_rec_tab(const struct dmx_mgmt *mgmt, hi_u32 id,
    hi_bool tpit_en, hi_bool pes_en, hi_bool es_long_en);
hi_void dmx_hal_scd_set_play_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool pes_en, hi_bool es_long_en);
hi_void dmx_hal_scd_set_flt_en(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool flt_en);
hi_s32  dmx_hal_set_scd_range_filter(const struct dmx_mgmt *mgmt, hi_u32 flt_id, hi_u8 high, hi_u8 low, hi_u8 mask);
hi_void dmx_hal_scd_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool is_index);
hi_void dmx_hal_scd_set_rec_chn_crc(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool crc_en);
hi_void dmx_hal_scd_set_rec_avpes_len_dis(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool avpes_len_dis);
hi_void dmx_hal_scd_set_rec_avpes_cut_dis(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool avpes_cut_dis);
hi_void dmx_hal_scd_set_rec_avpes_drop_en(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool avpes_drop_en);
hi_void dmx_hal_scd_set_rec_ctrl_mode(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ctrl_edit_dis);

/*
 * DEMUX PAR(Pid) hal level functions begin.
 */
enum dmx_pid_chn_flag_e {
    DMX_PID_CHN_TEE_LOCK            = (0x1 << 0),  /* 1: tee lock, 0: none tee lock */
    DMX_PID_CHN_PIDCOPY_FLAG        = (0x1 << 8),  /* 8 bits */
    DMX_PID_CHN_CW_FLAG             = (0x1 << 10), /* 10 bits */
    DMX_PID_CHN_WHOLE_TS_FLAG       = (0x1 << 12), /* 12 bits */
    DMX_PID_CHN_PES_SEC_FLAG        = (0x1 << 14), /* 14 bits */
    DMX_PID_CHN_AVPES_FLAG          = (0x1 << 16), /* 16 bits */
    DMX_PID_CHN_REC_FLAG            = (0x1 << 18), /* 18 bits */
    DMX_PID_CHN_TS_SCD_FLAG         = (0x1 << 22), /* 22 bits */
    DMX_PID_CHN_PES_SCD_FLAG        = (0x1 << 24), /* 24 bits */
    DMX_PID_CHN_DATA_MASK           = DMX_PID_CHN_WHOLE_TS_FLAG | DMX_PID_CHN_PES_SEC_FLAG |
                                      DMX_PID_CHN_AVPES_FLAG | DMX_PID_CHN_REC_FLAG,
};

enum dmx_pid_flt_rec_e {
    DMX_PID_TYPE_REC_SCD   =    0,
    DMX_PID_TYPE_FLT       =    1,
    DMX_PID_FLT_REC_BUTT
};

enum dmx_pid_pes_sec_type_e {
    DMX_PID_TYPE_SECTION   =    0,
    DMX_PID_TYPE_PES       =    1,
    DMX_PID_PES_SEC_BUTT
};

enum dmx_full_ts_out_type_e {
    DMX_FULL_TS_OUT_TYPE_DAV             = 0,
    DMX_FULL_TS_OUT_TYPE_DSC_REC_SCD     = 2,
    DMX_FULL_TS_OUT_TYPE_FLT             = 3,
    DMX_FULL_TS_OUT_TYPE_BUTT
};

hi_void dmx_hal_pid_tab_flt_set(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 band, hi_u32 pid, hi_u32 mark_id);
hi_void dmx_hal_pid_tab_flt_en(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_pid_tab_flt_dis(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_pid_tab_ctl_en_set(const struct dmx_mgmt *mgmt, hi_u32 id, enum dmx_pid_chn_flag_e en_ch_type);
hi_void dmx_hal_pid_tab_ctl_dis_set(const struct dmx_mgmt *mgmt, hi_u32 id, enum dmx_pid_chn_flag_e en_ch_type);
hi_void dmx_hal_pid_tab_set_sub_play_chan_id(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 sub_play_chan_id);
hi_void dmx_hal_pid_tab_set_pcid(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pcid);
hi_void dmx_hal_pid_tab_set_ccdrop(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ccerr_drop, hi_bool ccrepeat_drop);
hi_void dmx_hal_pid_set_rec_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rec_id);
hi_void dmx_hal_pid_set_rec_dsc_mode(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool descramed);
hi_void dmx_hal_pid_set_scd_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 ts_scd_id, hi_u32 pes_scd_id);
hi_void dmx_hal_pid_set_whole_tstab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id, hi_bool no_afcheck);
hi_void dmx_hal_pid_set_av_pes_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id,
                                   enum dmx_pid_flt_rec_e en_flt_rec, hi_bool pusi_en, hi_bool av_pes_len_chk);
hi_void dmx_hal_pid_set_pes_sec_tab(const struct dmx_mgmt *mgmt, hi_u32 id, enum dmx_pid_pes_sec_type_e en_pes_sec_type,
                                    hi_bool pusi_en, hi_bool pes_len_chk);
hi_void dmx_hal_pid_whole_tsout_flt_sel(const struct dmx_mgmt *mgmt, hi_bool flt_en);
hi_void dmx_hal_pid_set_ts_parse(const struct dmx_mgmt *mgmt, hi_bool af_chk, hi_bool all_af_drop);
hi_void dmx_hal_pid_clear_ram_state(const struct dmx_mgmt *mgmt, hi_u32 id);

/* descrambler releated */
hi_void dmx_hal_pid_cw_en_set(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool cw_en);
hi_void dmx_hal_pid_set_cw_id(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 cw_id);
hi_void dmx_hal_pid_en_pcmark_valid(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_u32 mark_valid);
hi_void dmx_hal_pid_dis_pcmark_valid(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_u32 mark_valid);
hi_void dmx_hal_pid_set_pcbuf_id(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_u32 buf_id);
hi_void dmx_hal_pid_set_dsc_type(const struct dmx_mgmt *mgmt, hi_u32 dsc_id, hi_bool ts_desc_en, hi_bool pes_desc_en);
hi_void dmx_hal_pid_get_dsc_type(const struct dmx_mgmt *mgmt, hi_u32 dsc_id, hi_dmx_scrambled_flag *scramble_flag);
hi_void dmx_hal_pid_ch_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 band, hi_u32 pid, hi_u32 mark_id);
hi_void dmx_hal_pid_ch_de_config(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32 dmx_hal_pid_ch_get_id(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32 dmx_hal_pid_ch_pkt_cnt(const struct dmx_mgmt *mgmt, hi_u32 id);

hi_void dmx_hal_set_full_tsout_flt(const struct dmx_mgmt *mgmt, enum dmx_full_ts_out_type_e full_ts_out);
hi_void dmx_hal_set_full_tsctl_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id, hi_bool ccpush_save,
                                   hi_bool tei_drop);
hi_void dmx_hal_set_ts_tei_ctl(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool tei_drop);
hi_void dmx_hal_set_ts_pusi_ctl(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ccpusi_save);

/*
 * DEMUX FLT hal level functions begin.
 */
hi_void dmx_hal_flt_un_lock(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_flt_lock(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_flt_enable(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_flt_disable(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_flt_set_pes_sec_id(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id);
hi_void dmx_hal_flt_set_buf_id(const struct dmx_mgmt *mgmt, hi_u32 buf_id);
hi_void dmx_hal_flt_set_ctl_attrs(const struct dmx_mgmt *mgmt, hi_u32 flt_min, hi_u32 flt_num, hi_bool err_pes_drop);
hi_void dmx_hal_flt_set_ctl_crc(const struct dmx_mgmt *mgmt, dmx_flt_crc_mode crc_mode);
hi_void dmx_hal_flt_set_flt_id(const struct dmx_mgmt *mgmt, hi_u32 index, hi_u32 flt_id);
hi_void dmx_hal_flt_set_flt_byte(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 byte_inx,
    hi_bool wdata_mode, hi_u32 wdata_content, hi_u32 wdata_mask);
hi_void dmx_hal_flt_attrs_config(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id, hi_u32 flt_num);
hi_void dmx_hal_flt_attach_config(const struct dmx_mgmt *mgmt, hi_u32 index, hi_u32 flt_id, hi_u32 pes_sec_id,
                                  hi_u32 buf_id, dmx_flt_crc_mode crc_mode);
hi_void dmx_hal_flt_detach_config(const struct dmx_mgmt *mgmt, hi_u32 index, hi_u32 flt_id, hi_u32 pes_sec_id);
hi_void dmx_hal_flt_clear_pes_sec_chan(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_flt_clear_av_pes_chan(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_flt_set_pes_default_attr(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id, hi_u32 buf_id);
hi_void dmx_hal_flt_set_sec_default_attr(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id, hi_u32 buf_id);
hi_void dmx_hal_flt_set_sec_no_pusi(const struct dmx_mgmt *mgmt, hi_bool  no_pusi);

/*
 * DEMUX DAV(Buf) hal level functions begin.
 */
hi_u32  dmx_hal_buf_get_timeout_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_cls_timeout_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_buf_get_ts_cnt_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_cls_ts_cnt_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_buf_get_seop_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_cls_seop_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_buf_get_ovfl_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_cls_ovfl_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_buf_get_raw_ap_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_cls_ap_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);

hi_void dmx_hal_buf_set_int_th(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 timeout_th, hi_u32 ts_th,
    hi_u32 seop_th, hi_bool timeout_int, hi_bool ovfl_int);
hi_void dmx_hal_buf_set_base_int_th(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 base_ts_th, hi_u32 base_seop_th);

hi_void dmx_hal_buf_en_mq_pes(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_dis_mq_pes(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_buf_get_data_type(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_buf_get_seop_addr(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_mq_set_addr_depth(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_addr, hi_u32 dsc_depth);
hi_void dmx_hal_mq_set_ap_th(const struct dmx_mgmt *mgmt, hi_u32 ap_th);
hi_u32  dmx_hal_mq_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_mq_set_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 read_idx);
hi_u32  dmx_hal_mq_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_mq_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_addr, hi_u32 dsc_depth);
hi_void dmx_hal_mq_de_config(const struct dmx_mgmt *mgmt, hi_u32 id);

hi_void dmx_hal_buf_enable(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_disable(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_set_start_addr(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 start_addr);
hi_void dmx_hal_buf_set_buf_size(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_size);
hi_u32  dmx_hal_buf_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_set_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 read_idx);
hi_u32  dmx_hal_buf_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_u32  dmx_hal_buf_get_cur_end_idx(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_buf_set_buf_sec_attrs(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool secure);
hi_void dmx_hal_buf_set_ap_th(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 ap_th);

hi_void dmx_hal_buf_open(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 time_int_th, hi_u32 ts_cnt_int_th,
                         hi_u32 seop_int_th);
hi_void dmx_hal_buf_close(const struct dmx_mgmt *mgmt, hi_u32 id);

hi_void dmx_hal_buf_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 buf_start_addr, hi_u32 buf_size,
    hi_bool secure);
hi_void dmx_hal_buf_de_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 buf_start_addr, hi_u32 buf_size,
    hi_bool secure);

hi_void dmx_hal_pid_copy_buf_lock(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_pid_copy_buf_set_id(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_pid_copy_buf_un_lock(const struct dmx_mgmt *mgmt);
hi_void dmx_hal_pid_copy_buf_set_start_addr(const struct dmx_mgmt *mgmt, hi_u64 start_addr);
hi_void dmx_hal_pid_copy_buf_set_buf_size(const struct dmx_mgmt *mgmt, hi_u32 buf_size);
hi_u32  dmx_hal_pid_copy_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 pcid);
hi_u32  dmx_hal_pid_copy_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 pcid);
hi_void dmx_hal_pid_copy_tx_en_set(const struct dmx_mgmt *mgmt, hi_bool tx_pc_en);
hi_void dmx_hal_pid_copy_sec_en_set(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_bool sec_pc_en);
hi_void dmx_hal_pid_copy_rx_en_set(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_bool rx_pc_en);
hi_void dmx_hal_pid_copy_ap_en_set(const struct dmx_mgmt *mgmt, hi_u32 ap_th);
hi_u32  dmx_hal_pid_copy_get_raw_ap_ram_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_pid_copy_cls_ap_ram_int_status(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_pid_copy_cls_ap_status(const struct dmx_mgmt *mgmt, hi_u32 pcid);
hi_void dmx_hal_pid_copy_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 pcid);
hi_void dmx_hal_dav_tx_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_void dmx_hal_pid_copy_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 start_addr, hi_u32 buf_size);
hi_void dmx_hal_pid_copy_de_config(const struct dmx_mgmt *mgmt, hi_u32 id);
hi_s32 dmx_hal_buf_set_tsio_ap(const struct dmx_mgmt *mgmt, hi_u32 chan_id, hi_bool enable, hi_u32 tsio_chan);
hi_void dmx_hal_buf_pc_set_tsio_ap(const struct dmx_mgmt *mgmt, hi_u32 chan_id, hi_bool enable, hi_u32 tsio_chan);
hi_void dmx_hal_buf_clr_tsio_ap(const struct dmx_mgmt *mgmt, hi_u32 tsio_chan);
hi_u32 dmx_hal_buf_get_pc_tsio_ap_status(const struct dmx_mgmt *mgmt, hi_u32 tsio_chan);
hi_u32 dmx_hal_buf_get_tx_tsio_ap_status(const struct dmx_mgmt *mgmt, hi_u32 tsio_chan);

/*
 * DEMUX MDSC hal level functions begin.
 */
hi_void dmx_hal_mdscset_encrypt_even_odd(const struct dmx_mgmt *mgmt, hi_u32 id, dmx_dsc_key_type even_odd);
hi_void dmx_hal_mdscset_entropy_reduction(const struct dmx_mgmt *mgmt, hi_u32 id, dmx_dsc_entropy entropy_reduction);
hi_void dmx_hal_mdscset_en(const struct dmx_mgmt *mgmt, hi_bool ca_en, hi_bool ts_ctrl_dsc_change_en, hi_bool cw_iv_en);
hi_void dmx_hal_mdscdis_cpd_core(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool dis_core);
hi_void dmx_hal_mdscdis_ca_core(const struct dmx_mgmt *mgmt, hi_bool dis_core);
hi_void dmx_hal_mdscdis_cps_core(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool dis_core);
hi_void dmx_hal_mdsc_multi2_sys_key_cfg(const struct dmx_mgmt *mgmt, hi_u8 *key, hi_u32 key_len);
/*
 * DEMUX MDSC hal level functions end.
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* __HAL_DEMUX_H__ */

