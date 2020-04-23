/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver dsc reg header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_DSC_REG_H__
#define __HAL_HDMITX_DSC_REG_H__

/* DSCE module field info */
#define REG_DSC_CTRL               0x9000
#define reg_dsc_sync_delay_time(x) (((x) & 0x3ff) << 20) /* [29:20] */
#define REG_DSC_SYNC_DELAY_TIME_M  (0x3ff << 20)
#define reg_ck_gt_en_core(x)       (((x) & 0xffff) << 4) /* [19:4] */
#define REG_CK_GT_EN_CORE_M        (0xffff << 4)
#define reg_frame_packed_3d_en(x)  (((x) & 1) << 3) /* [3] */
#define REG_FRAME_PACKED_3D_EN_M   (1 << 3)
#define reg_ck_gt_en_com(x)        (((x) & 1) << 1) /* [1] */
#define REG_CK_GT_EN_COM_M         (1 << 1)
#define reg_dsc_enable(x)          (((x) & 1) << 0) /* [0] */
#define REG_DSC_ENABLE_M           (1 << 0)

#define REG_DSC_REG_UPDATE 0x9004
#define reg_dsc_rupd(x) (((x) & 1) << 0) /* [0] */
#define REG_DSC_RUPD_M  (1 << 0)

#define REG_DSC_INT_STA         0x9008
#define reg_dsc_rc_err_sta(x)   (((x) & 0xffff) << 4) /* [19:4] */
#define REG_DSC_RC_ERR_STA_M    (0xffff << 4)
#define reg_dsc_frm_done_sta(x) (((x) & 1) << 0) /* [0] */
#define REG_DSC_FRM_DONE_STA_M  (1 << 0)

#define REG_DSC_INT_MASK         0x900C
#define reg_dsc_rc_err_mask(x)   (((x) & 0xffff) << 4) /* [19:4] */
#define REG_DSC_RC_ERR_MASK_M    (0xffff << 4)
#define reg_dsc_frm_done_mask(x) (((x) & 1) << 0) /* [0] */
#define REG_DSC_FRM_DONE_MASK_M  (1 << 0)

#define REG_DSC_OUT_INTF_HOR 0x9010
#define reg_out_intf_hbb(x)  (((x) & 0xffff) << 16) /* [31:16] */
#define REG_OUT_INTF_HBB_M   (0xffff << 16)
#define reg_out_intf_hfb(x)  (((x) & 0xffff) << 0) /* [15:0] */
#define REG_OUT_INTF_HFB_M   (0xffff << 0)

#define REG_DSC_OUT_INTF_VER 0x9014
#define reg_out_intf_vbb(x)  (((x) & 0xffff) << 16) /* [31:16] */
#define REG_OUT_INTF_VBB_M   (0xffff << 16)
#define reg_out_intf_vfb(x)  (((x) & 0xffff) << 0) /* [15:0] */
#define REG_OUT_INTF_VFB_M   (0xffff << 0)

#define REG_DSC_OUT_INTF_HBLANK 0x9018
#define reg_out_intf_hactive(x) (((x) & 0xffff) << 16) /* [31:16] */
#define REG_OUT_INTF_HACTIVE_M  (0xffff << 16)
#define reg_out_intf_hblank(x)  (((x) & 0xffff) << 0) /* [15:0] */
#define REG_OUT_INTF_HBLANK_M   (0xffff << 0)

#define REG_DSC_OUT_INTF_VSYNC  0x901C
#define reg_out_intf_vactive(x) (((x) & 0xffff) << 16) /* [31:16] */
#define REG_OUT_INTF_VACTIVE_M  (0xffff << 16)
#define reg_out_intf_vsync(x)   (((x) & 0xffff) << 0) /* [15:0] */
#define REG_OUT_INTF_VSYNC_M    (0xffff << 0)

#define REG_DSC_OUT_INTF_POLAR 0x9020
#define reg_out_intf_idv(x) (((x) & 1) << 2) /* [2] */
#define REG_OUT_INTF_IDV_M  (1 << 2)
#define reg_out_intf_ihs(x) (((x) & 1) << 1) /* [1] */
#define REG_OUT_INTF_IHS_M  (1 << 1)
#define reg_out_intf_ivs(x) (((x) & 1) << 0) /* [0] */
#define REG_OUT_INTF_IVS_M  (1 << 0)

#define REG_DSC_RC_BITS 0x9030
#define reg_rcb_bits(x) (((x) & 0xffff) << 0) /* [15:0] */
#define REG_RCB_BITS_M  (0xffff << 0)

#define REG_DSC_ALG_CTRL              0x9034
#define reg_block_pred_enable(x)      (((x) & 1) << 1) /* [1] */
#define REG_BLOCK_PRED_ENABLE_M       (1 << 1)
#define reg_full_ich_err_precision(x) (((x) & 1) << 0) /* [0] */
#define REG_FULL_ICH_ERR_PRECISION_M  (1 << 0)

#define REG_DSC_BPC               0x9038
#define reg_bits_per_component(x) (((x) & 0xf) << 0) /* [3:0] */
#define REG_BITS_PER_COMPONENT_M  (0xf << 0)

#define REG_DSC_SAMPLE     0x903C
#define reg_native_444(x)  (((x) & 1) << 3) /* [3] */
#define REG_NATIVE_444_M   (1 << 3)
#define reg_native_422(x)  (((x) & 1) << 2) /* [2] */
#define REG_NATIVE_422_M   (1 << 2)
#define reg_native_420(x)  (((x) & 1) << 1) /* [1] */
#define REG_NATIVE_420_M   (1 << 1)
#define reg_convert_rgb(x) (((x) & 1) << 0) /* [0] */
#define REG_CONVERT_RGB_M  (1 << 0)

#define REG_DSC_BPP_CHK       0x9040
#define reg_chunk_size(x)     (((x) & 0xffff) << 16) /* [31:16] */
#define REG_CHUNK_SIZE_M      (0xffff << 16)
#define reg_bits_per_pixel(x) (((x) & 0x3ff) << 0) /* [9:0] */
#define REG_BITS_PER_PIXEL_M  (0x3ff << 0)

#define REG_DSC_PIC_RESO  0x9044
#define reg_pic_height(x) (((x) & 0xffff) << 16) /* [31:16] */
#define REG_PIC_HEIGHT_M  (0xffff << 16)
#define reg_pic_width(x)  (((x) & 0xffff) << 0) /* [15:0] */
#define REG_PIC_WIDTH_M   (0xffff << 0)

#define REG_DSC_SLC_RESO    0x9048
#define reg_slice_height(x) (((x) & 0xffff) << 16) /* [31:16] */
#define REG_SLICE_HEIGHT_M  (0xffff << 16)
#define reg_slice_width(x)  (((x) & 0xffff) << 0) /* [15:0] */
#define REG_SLICE_WIDTH_M   (0xffff << 0)

#define REG_DSC_INIT_XMIT_DLY     0x904C
#define reg_initial_xmit_delay(x) (((x) & 0x3ff) << 0) /* [9:0] */
#define REG_INITIAL_XMIT_DELAY_M  (0x3ff << 0)

#define REG_DSC_INIT_DEC_DLY     0x9050
#define reg_initial_dec_delay(x) (((x) & 0xffff) << 0) /* [15:0] */
#define REG_INITIAL_DEC_DELAY_M  (0xffff << 0)

#define REG_DSC_INIT_SCALE         0x9054
#define reg_initial_scale_value(x) (((x) & 0X3f) << 0) /* [5:0] */
#define REG_INITIAL_SCALE_VALUE_M  (0X3f << 0)

#define REG_DSC_SCALE_INTVAL            0x9058
#define reg_scale_increment_interval(x) (((x) & 0xffff) << 16) /* [31:16] */
#define REG_SCALE_INCREMENT_INTERVAL_M  (0xffff << 16)
#define reg_scale_decrement_interval(x) (((x) & 0xfff) << 0) /* [11:0] */
#define REG_SCALE_DECREMENT_INTERVAL_M  (0xfff << 0)

#define REG_DSC_FIRST_BPG         0x905C
#define reg_first_line_bpg_ofs(x) (((x) & 0x1f) << 16) /* [20:16] */
#define REG_FIRST_LINE_BPG_OFS_M  (0x1f << 16)
#define reg_nfl_bpg_offset(x)     (((x) & 0xffff) << 0) /* [15:0] */
#define REG_NFL_BPG_OFFSET_M      (0xffff << 0)

#define REG_DSC_SECOND_BPG         0x9060
#define reg_second_line_bpg_ofs(x) (((x) & 0x1f) << 16) /* [20:16] */
#define REG_SECOND_LINE_BPG_OFS_M  (0x1f << 16)
#define reg_nsl_bpg_offset(x)      (((x) & 0xffff) << 0) /* [15:0] */
#define REG_NSL_BPG_OFFSET_M       (0xffff << 0)

#define REG_DSC_SECOND_ADJ         0x9064
#define reg_second_line_ofs_adj(x) (((x) & 0xffff) << 0) /* [15:0] */
#define REG_SECOND_LINE_OFS_ADJ_M  (0xffff << 0)

#define REG_DSC_INIT_FINL_OFS 0x9068
#define reg_initial_offset(x) (((x) & 0xffff) << 16) /* [31:16] */
#define REG_INITIAL_OFFSET_M  (0xffff << 16)
#define reg_final_offset(x)   (((x) & 0xffff) << 0) /* [15:0] */
#define REG_FINAL_OFFSET_M    (0xffff << 0)

#define REG_DSC_SLC_BPG         0x906C
#define reg_slice_bpg_offset(x) (((x) & 0xffff) << 0) /* [15:0] */
#define REG_SLICE_BPG_OFFSET_M  (0xffff << 0)

#define REG_DSC_FLAT_RANGE     0x9070
#define reg_flatness_min_qp(x) (((x) & 0x1f) << 8) /* [12:8] */
#define REG_FLATNESS_MIN_QP_M  (0x1f << 8)
#define reg_flatness_max_qp(x) (((x) & 0x1f) << 0) /* [4:0] */
#define REG_FLATNESS_MAX_QP_M  (0x1f << 0)

#define REG_DSC_RC_MOD_EDGE   0x9074
#define reg_rc_model_size(x)  (((x) & 0xffff) << 16) /* [31:16] */
#define REG_RC_MODEL_SIZE_M   (0xffff << 16)
#define reg_rc_edge_factor(x) (((x) & 0xf) << 0) /* [3:0] */
#define REG_RC_EDGE_FACTOR_M  (0xf << 0)

#define REG_DSC_RC_QUA_TGT          0x9078
#define reg_rc_tgt_offset_lo(x)     (((x) & 0xf) << 20) /* [23:20] */
#define REG_RC_TGT_OFFSET_LO_M      (0xf << 20)
#define reg_rc_tgt_offset_hi(x)     (((x) & 0xf) << 16) /* [19:16] */
#define REG_RC_TGT_OFFSET_HI_M      (0xf << 16)
#define reg_rc_quant_incr_limit1(x) (((x) & 0x1f) << 8) /* [12:8] */
#define REG_RC_QUANT_INCR_LIMIT1_M  (0x1f << 8)
#define reg_rc_quant_incr_limit0(x) (((x) & 0x1f) << 0) /* [4:0] */
#define REG_RC_QUANT_INCR_LIMIT0_M  (0x1f << 0)

#define REG_DSC_RC_THRE_01      0x907C
#define reg_rc_buf_thresh_03(x) (((x) & 0xff) << 24) /* [31:24] */
#define REG_RC_BUF_THRESH_03_M  (0xff << 24)
#define reg_rc_buf_thresh_02(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_RC_BUF_THRESH_02_M  (0xff << 16)
#define reg_rc_buf_thresh_01(x) (((x) & 0xff) << 8) /* [15:8] */
#define REG_RC_BUF_THRESH_01_M  (0xff << 8)
#define reg_rc_buf_thresh_00(x) (((x) & 0xff) << 0) /* [7:0] */
#define REG_RC_BUF_THRESH_00_M  (0xff << 0)

#define REG_DSC_RC_THRE_02      0x9080
#define reg_rc_buf_thresh_07(x) (((x) & 0xff) << 24) /* [31:24] */
#define REG_RC_BUF_THRESH_07_M  (0xff << 24)
#define reg_rc_buf_thresh_06(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_RC_BUF_THRESH_06_M  (0xff << 16)
#define reg_rc_buf_thresh_05(x) (((x) & 0xff) << 8) /* [15:8] */
#define REG_RC_BUF_THRESH_05_M  (0xff << 8)
#define reg_rc_buf_thresh_04(x) (((x) & 0xff) << 0) /* [7:0] */
#define REG_RC_BUF_THRESH_04_M  (0xff << 0)

#define REG_DSC_RC_THRE_03      0x9084
#define reg_rc_buf_thresh_11(x) (((x) & 0xff) << 24) /* [31:24] */
#define REG_RC_BUF_THRESH_11_M  (0xff << 24)
#define reg_rc_buf_thresh_10(x) (((x) & 0xff) << 16) /* [23:16] */
#define REG_RC_BUF_THRESH_10_M  (0xff << 16)
#define reg_rc_buf_thresh_09(x) (((x) & 0xff) << 8) /* [15:8] */
#define REG_RC_BUF_THRESH_09_M  (0xff << 8)
#define reg_rc_buf_thresh_08(x) (((x) & 0xff) << 0) /* [7:0] */
#define REG_RC_BUF_THRESH_08_M  (0xff << 0)

#define REG_DSC_RC_THRE_04      0x9088
#define reg_rc_buf_thresh_13(x) (((x) & 0xff) << 8) /* [15:8] */
#define REG_RC_BUF_THRESH_13_M  (0xff << 8)
#define reg_rc_buf_thresh_12(x) (((x) & 0xff) << 0) /* [7:0] */
#define REG_RC_BUF_THRESH_12_M  (0xff << 0)

#define reg_dsc_rc_param(n) (0x908C + (n) * 4)

#define reg_range_min_qp(x)     (((x) & 0x1f) << 16) /* [20:16] */
#define REG_RANGE_MIN_QP_M      (0x1f << 16)
#define reg_range_max_qp(x)     (((x) & 0x1f) << 8) /* [12:8] */
#define REG_RANGE_MAX_QP_M      (0x1f << 8)
#define reg_range_bpg_offset(x) (((x) & 0X3f) << 0) /* [5:0] */
#define REG_RANGE_BPG_OFFSET_M  (0X3f << 0)

#define REG_DSC_CMC_MODE_LO 0x90C8

#define REG_DSC_CMC_MODE_HI 0x90CC

#define reg_dsc_enc_ctrl(n)  (0x90D0 + (n) * 0xc)
#define reg_dsc_enc_eco(n)   (0x90D4 + (n) * 0xc)
#define reg_dsc_enc_debug(n) (0x90D8 + (n) * 0xc)

#define reg_rc_wrr_type(x) ((x) & 0xff)

#define REG_DSC_ENC_8_DEBUG    0x9138
#define reg_debug_enc8(x)      (((x) & 0x3f) << 26) /* [31:26] */
#define REG_DEBUG_ENC8_M       (0x3f << 26)
#define reg_fifo_err_type(x)   (((x) & 0x3) << 24) /* [25:24] */
#define REG_FIFO_ERR_TYPE_M    (0x3 << 24)
#define reg_core_pix_bs_sta(x) (((x) & 0xf) << 20) /* [23:20] */
#define REG_CORE_PIX_BS_STA_M  (0xf << 20)
#define reg_rc_err_vld(x)      (((x) & 0xff) << 12) /* [19:12] */
#define REG_RC_ERR_VLD_M       (0xff << 12)
#define reg_o_intf_low_bw(x)   (((x) & 1) << 9) /* [9] */
#define REG_O_INTF_LOW_BW_M    (1 << 9)
#define reg_isync_fifo_full(x) (((x) & 1) << 8) /* [8] */
#define REG_ISYNC_FIFO_FULL_M  (1 << 8)

#endif
