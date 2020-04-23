/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver dsc controller source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include "linux/hisilicon/securec.h"
#include "hi_osal.h"
#include "drv_hdmitx.h"
#include "hal_hdmitx_io.h"
#include "drv_hdmitx_connector.h"
#include "hal_hdmitx_dsc_reg.h"
#include "hal_hdmitx_ctrl_reg.h"
#include "hal_hdmitx_video_path_reg.h"
#include "hal_hdmitx_dsc.h"
#include "hal_hdmitx_ctrl.h"

#define DSC_SRST_REQ_MASK         (0x1 << 8)
#define NUMBER_2                   2
#define NUMBER_4                   4
#define NUMBER_8                   8
#define NUMBER_15                  15
#define NUMBER_16                  16
#define NUMBER_64                  64
#define NUMBER_128                 128
#define NUMBER_192                 192

/* compute sync dealy time */
static hi_s32 dsc_delay_time(struct dsc_ctrl *ctrl, hi_u32 hblank)
{
    hi_u32 sync_delay_time = 0;
    hi_u32 bpp;
    hi_u32 tmp;
    struct dsc_cfg_s *cfg = &ctrl->cfg_s;

    bpp = cfg->bits_per_pixel;

    if (cfg->slice_width < cfg->pic_width / NUMBER_2) {
        tmp = NUMBER_128 * NUMBER_192 * NUMBER_8;
    } else {
        tmp = NUMBER_128 * NUMBER_192 * NUMBER_4;
    }

    if (cfg->native_420) {
        sync_delay_time = tmp / ((bpp / NUMBER_2 + NUMBER_15) /
                          NUMBER_16) / NUMBER_64 / NUMBER_2;
    } else if (cfg->native_422) {
        sync_delay_time = tmp / ((bpp / NUMBER_2 + NUMBER_15) /
                          NUMBER_16) / NUMBER_64;
    } else {
        sync_delay_time = tmp / ((bpp + NUMBER_15) /
                          NUMBER_16) / NUMBER_64;
    }

    if (cfg->native_420) {
        sync_delay_time = sync_delay_time + hblank / NUMBER_64;
    } else {
        sync_delay_time = sync_delay_time + hblank / NUMBER_64 * NUMBER_2;
    }

    return sync_delay_time;
}

static hi_void dsc_hw_set_timing_info(struct dsc_ctrl *ctrl)
{
    struct dsc_timing_info *timing_info = &ctrl->timing_info;

    hdmi_clrset(ctrl->regs, REG_DSC_CTRL, REG_DSC_SYNC_DELAY_TIME_M,
        reg_dsc_sync_delay_time(dsc_delay_time(ctrl, timing_info->hblank)));

    hdmi_clrset(ctrl->regs, REG_DSC_OUT_INTF_HOR, REG_OUT_INTF_HFB_M | REG_OUT_INTF_HBB_M,
        reg_out_intf_hbb(timing_info->hback) | reg_out_intf_hfb(timing_info->hfront));

    hdmi_clrset(ctrl->regs, REG_DSC_OUT_INTF_VER, REG_OUT_INTF_VBB_M | REG_OUT_INTF_VFB_M,
        reg_out_intf_vbb(timing_info->vback) | reg_out_intf_vfb(timing_info->vfront));

    hdmi_clrset(ctrl->regs, REG_DSC_OUT_INTF_HBLANK, REG_OUT_INTF_HACTIVE_M | REG_OUT_INTF_HBLANK_M,
        reg_out_intf_hactive(timing_info->hcactive) | reg_out_intf_hblank(timing_info->hcblank));

    hdmi_clrset(ctrl->regs, REG_DSC_OUT_INTF_VSYNC, REG_OUT_INTF_VACTIVE_M | REG_OUT_INTF_VSYNC_M,
        reg_out_intf_vactive(timing_info->vactive) | reg_out_intf_vsync(timing_info->vsync));

    hdmi_clrset(ctrl->regs, REG_DSC_OUT_INTF_POLAR,
        REG_OUT_INTF_IDV_M | REG_OUT_INTF_IHS_M | REG_OUT_INTF_IVS_M,
        reg_out_intf_idv(timing_info->de_polarity) | reg_out_intf_ihs(timing_info->h_polarity) |
        reg_out_intf_ivs(timing_info->v_polarity));
}

static hi_void dsc_hw_set_alg_params_part_one(struct dsc_ctrl *ctrl)
{
    struct dsc_cfg_s *dsc_cfg = &ctrl->cfg_s;

    hdmi_clrset(ctrl->regs, REG_DSC_RC_BITS, REG_RCB_BITS_M, reg_rcb_bits(dsc_cfg->rcb_bits));
    hdmi_clrset(ctrl->regs, REG_DSC_BPC, REG_BITS_PER_COMPONENT_M,
        reg_bits_per_component(dsc_cfg->bits_per_component));

    /* input pixel encoding */
    hdmi_clrset(ctrl->regs, REG_DSC_SAMPLE,
                REG_CONVERT_RGB_M | REG_NATIVE_420_M | REG_NATIVE_422_M | REG_NATIVE_444_M,
                reg_convert_rgb(dsc_cfg->convert_rgb) | reg_native_420(dsc_cfg->native_420) |
                reg_native_422(dsc_cfg->native_422) | reg_native_444(dsc_cfg->native_444));

    /* slice resolution */
    hdmi_clrset(ctrl->regs, REG_DSC_SLC_BPG,
                REG_SLICE_BPG_OFFSET_M, reg_slice_bpg_offset(dsc_cfg->slice_bpg_offset));
    hdmi_clrset(ctrl->regs, REG_DSC_SLC_RESO,
                REG_SLICE_WIDTH_M | REG_SLICE_HEIGHT_M,
                reg_slice_width(dsc_cfg->slice_width) | reg_slice_height(dsc_cfg->slice_height));

    /* input image resolution */
    hdmi_clrset(ctrl->regs, REG_DSC_PIC_RESO,
                REG_PIC_WIDTH_M | REG_PIC_HEIGHT_M,
                reg_pic_width(dsc_cfg->pic_width) | reg_pic_height(dsc_cfg->pic_height));

    hdmi_clrset(ctrl->regs, REG_DSC_RC_QUA_TGT,
                REG_RC_TGT_OFFSET_LO_M | REG_RC_TGT_OFFSET_HI_M |
                REG_RC_QUANT_INCR_LIMIT0_M | REG_RC_QUANT_INCR_LIMIT1_M,
                reg_rc_tgt_offset_lo(dsc_cfg->rc_tgt_offset_lo) |
                reg_rc_tgt_offset_hi(dsc_cfg->rc_tgt_offset_hi) |
                reg_rc_quant_incr_limit1(dsc_cfg->rc_quant_incr_limit1) |
                reg_rc_quant_incr_limit0(dsc_cfg->rc_quant_incr_limit0));

    /* bits_per_pixel */
    hdmi_clrset(ctrl->regs, REG_DSC_BPP_CHK,
                REG_BITS_PER_PIXEL_M | REG_CHUNK_SIZE_M,
                reg_bits_per_pixel(dsc_cfg->bits_per_pixel) |
                reg_chunk_size(dsc_cfg->chunk_size));
}

static hi_void dsc_hw_set_alg_params_part_two(struct dsc_ctrl *ctrl)
{
    struct dsc_cfg_s *dsc_cfg = &ctrl->cfg_s;

    hdmi_clrset(ctrl->regs, REG_DSC_RC_MOD_EDGE,
                REG_RC_EDGE_FACTOR_M | REG_RC_MODEL_SIZE_M,
                reg_rc_edge_factor(dsc_cfg->rc_edge_factor) |
                reg_rc_model_size(dsc_cfg->rc_model_size));

    /* Slice initialization transfer delay */
    hdmi_clrset(ctrl->regs, REG_DSC_INIT_XMIT_DLY,
                REG_INITIAL_XMIT_DELAY_M, reg_initial_xmit_delay(dsc_cfg->initial_xmit_delay));
    hdmi_clrset(ctrl->regs, REG_DSC_INIT_DEC_DLY,
                REG_INITIAL_DEC_DELAY_M, reg_initial_dec_delay(dsc_cfg->initial_dec_delay));

    hdmi_clrset(ctrl->regs, REG_DSC_ALG_CTRL,
                REG_FULL_ICH_ERR_PRECISION_M | REG_BLOCK_PRED_ENABLE_M,
                reg_full_ich_err_precision(dsc_cfg->full_ich_err_precision) |
                reg_block_pred_enable(dsc_cfg->block_pred_enable));

    hdmi_clrset(ctrl->regs, REG_DSC_FIRST_BPG,
                REG_NFL_BPG_OFFSET_M | REG_FIRST_LINE_BPG_OFS_M,
                reg_nfl_bpg_offset(dsc_cfg->nfl_bpg_offset) |
                reg_first_line_bpg_ofs(dsc_cfg->first_line_bpg_ofs));

    hdmi_clrset(ctrl->regs, REG_DSC_SECOND_BPG,
                REG_NSL_BPG_OFFSET_M | REG_SECOND_LINE_BPG_OFS_M,
                reg_nsl_bpg_offset(dsc_cfg->nsl_bpg_offset) |
                reg_second_line_bpg_ofs(dsc_cfg->second_line_bpg_ofs));

    hdmi_clrset(ctrl->regs, REG_DSC_INIT_FINL_OFS,
                REG_FINAL_OFFSET_M | REG_INITIAL_OFFSET_M,
                reg_final_offset(dsc_cfg->final_offset) |
                reg_initial_offset(dsc_cfg->initial_offset));
    hdmi_clrset(ctrl->regs, REG_DSC_INIT_FINL_OFS,
                REG_FINAL_OFFSET_M, reg_final_offset(dsc_cfg->final_offset));
}

static hi_void dsc_hw_set_alg_params_part_three(struct dsc_ctrl *ctrl)
{
    struct dsc_cfg_s *dsc_cfg = &ctrl->cfg_s;
    hi_u32 i;

    hdmi_clrset(ctrl->regs, REG_DSC_RC_THRE_01,
                REG_RC_BUF_THRESH_00_M | REG_RC_BUF_THRESH_01_M |
                REG_RC_BUF_THRESH_02_M | REG_RC_BUF_THRESH_03_M,
                reg_rc_buf_thresh_00(dsc_cfg->rc_buf_thresh[0]) | /* rc buffer thresh 0. */
                reg_rc_buf_thresh_01(dsc_cfg->rc_buf_thresh[1]) | /* rc buffer thresh 1. */
                reg_rc_buf_thresh_02(dsc_cfg->rc_buf_thresh[2]) | /* rc buffer thresh 2. */
                reg_rc_buf_thresh_03(dsc_cfg->rc_buf_thresh[3])); /* rc buffer thresh 3. */
    hdmi_clrset(ctrl->regs, REG_DSC_RC_THRE_03,
                REG_RC_BUF_THRESH_04_M | REG_RC_BUF_THRESH_05_M |
                REG_RC_BUF_THRESH_06_M | REG_RC_BUF_THRESH_07_M,
                reg_rc_buf_thresh_04(dsc_cfg->rc_buf_thresh[4]) | /* rc buffer thresh 4. */
                reg_rc_buf_thresh_05(dsc_cfg->rc_buf_thresh[5]) | /* rc buffer thresh 5. */
                reg_rc_buf_thresh_06(dsc_cfg->rc_buf_thresh[6]) | /* rc buffer thresh 6. */
                reg_rc_buf_thresh_07(dsc_cfg->rc_buf_thresh[7])); /* rc buffer thresh 7. */
    hdmi_clrset(ctrl->regs, REG_DSC_RC_THRE_03,
                REG_RC_BUF_THRESH_08_M | REG_RC_BUF_THRESH_09_M |
                REG_RC_BUF_THRESH_10_M | REG_RC_BUF_THRESH_11_M,
                reg_rc_buf_thresh_08(dsc_cfg->rc_buf_thresh[8]) | /* rc buffer thresh 8. */
                reg_rc_buf_thresh_09(dsc_cfg->rc_buf_thresh[9]) | /* rc buffer thresh 9. */
                reg_rc_buf_thresh_10(dsc_cfg->rc_buf_thresh[10]) | /* rc buffer thresh 10. */
                reg_rc_buf_thresh_11(dsc_cfg->rc_buf_thresh[11])); /* rc buffer thresh 11. */
    hdmi_clrset(ctrl->regs, REG_DSC_RC_THRE_04,
                REG_RC_BUF_THRESH_12_M | REG_RC_BUF_THRESH_13_M,
                reg_rc_buf_thresh_12(dsc_cfg->rc_buf_thresh[12]) | /* rc buffer thresh 12. */
                reg_rc_buf_thresh_13(dsc_cfg->rc_buf_thresh[13])); /* rc buffer thresh 13. */

    for (i = 0; i < NUM_BUF_RANGES; i++) {
        hdmi_clrset(ctrl->regs, reg_dsc_rc_param(i),
                    REG_RANGE_MIN_QP_M | REG_RANGE_MAX_QP_M | REG_RANGE_BPG_OFFSET_M,
                    reg_range_min_qp(dsc_cfg->rc_range_parameters[i].range_min_qp) |
                    reg_range_max_qp(dsc_cfg->rc_range_parameters[i].range_max_qp) |
                    reg_range_bpg_offset(dsc_cfg->rc_range_parameters[i].range_bpg_offset));
    }

    hdmi_clrset(ctrl->regs, REG_DSC_FLAT_RANGE,
                REG_FLATNESS_MAX_QP_M | REG_FLATNESS_MIN_QP_M,
                reg_flatness_max_qp(dsc_cfg->flatness_max_qp) |
                reg_flatness_min_qp(dsc_cfg->flatness_min_qp));

    hdmi_clrset(ctrl->regs, REG_DSC_INIT_SCALE,
                REG_INITIAL_SCALE_VALUE_M, reg_initial_scale_value(dsc_cfg->initial_scale_value));
    hdmi_clrset(ctrl->regs, REG_DSC_SECOND_ADJ,
                REG_SECOND_LINE_OFS_ADJ_M, reg_second_line_ofs_adj(dsc_cfg->second_line_ofs_adj));

    hdmi_clrset(ctrl->regs, REG_DSC_SCALE_INTVAL,
                REG_SCALE_INCREMENT_INTERVAL_M | REG_SCALE_DECREMENT_INTERVAL_M,
                reg_scale_increment_interval(dsc_cfg->scale_increment_interval) |
                reg_scale_decrement_interval(dsc_cfg->scale_decrement_interval));
}

/* DSC ctrl init */
struct dsc_ctrl *hdmi_dsc_init(struct hdmi_controller *ctrl)
{
    struct dsc_ctrl *dsc = HI_NULL;
    struct hisilicon_hdmi *hdmi = HI_NULL;
    struct source_capability *cap = HI_NULL;

    if (ctrl == HI_NULL) {
        HDMI_ERR("ptr is null.\n");
        return HI_NULL;
    }

    hdmi = ctrl->parent;
    cap = &hdmi->connector->src_cap;

    dsc = osal_kmalloc(HI_ID_HDMITX, sizeof(struct dsc_ctrl), OSAL_GFP_KERNEL);
    if (dsc == HI_NULL) {
        HDMI_ERR("osal_kmalloc fail.\n");
        return HI_NULL;
    }

    if (memset_s(dsc, sizeof(struct dsc_ctrl), 0, sizeof(struct dsc_ctrl))) {
        HDMI_ERR("memset_s fail.\n");
        osal_kfree(HI_ID_HDMITX, dsc);
        return HI_NULL;
    }

    /* dsc cap init */
    dsc->caps.dsc_1p2 = cap->hdmi.dsc_support;
    dsc->caps.dsc_all_bpp = false;
    dsc->caps.dsc_10bpc = cap->hdmi.dsc_10bpc;
    dsc->caps.dsc_12bpc = cap->hdmi.dsc_12bpc;
    dsc->caps.dsc_16bpc = false;
    dsc->caps.dsc_max_frl_rate = cap->hdmi.max_dsc_frl_rate;
    dsc->caps.dsc_native420 = cap->hdmi.native_y420;
    dsc->caps.max_chunk_byte = 0;
    dsc->caps.max_pixel_clk_per_slice = cap->hdmi.max_pixel_clk_per_slice;
    dsc->caps.max_silce_cout = cap->hdmi.max_silce_count;

    dsc->regs = hdmi->hdmi_regs;
    dsc->ctrl_crg_regs = hdmi->ctrl_crg_regs;
    dsc->version = 1;
    dsc->parent = ctrl;

    hi_hdmi_dsc_get_status(dsc);

    return dsc;
}

/* DSC ctrl enable */
hi_s32 hi_hdmi_dsc_enable(struct dsc_ctrl *ctrl)
{
    hdmi_clrset(ctrl->regs, REG_VID_ADAPTER,
                REG_VID_IN_DSC_MODE_M, reg_vid_in_dsc_mode(true));

    if (ctrl->cfg_s.native_422 == true)
        hdmi_clrset(ctrl->regs, REG_VID_ADAPTER,
                    REG_VID_IN_422_MODE_M, reg_vid_in_422_mode(false));

    /* step 1: CRG reset */
    hdmi_set(ctrl->regs, REG_TX_PWD_RST_CTRL, REG_TX_CSC_DITHER_SRST_REQ_M);
    osal_udelay(1);
    hdmi_clr(ctrl->regs, REG_TX_PWD_RST_CTRL, REG_TX_CSC_DITHER_SRST_REQ_M);

    hdmi_set(ctrl->ctrl_crg_regs, 0, DSC_SRST_REQ_MASK);
    osal_udelay(1);
    hdmi_clr(ctrl->ctrl_crg_regs, 0, DSC_SRST_REQ_MASK);

    /* step 2: Configure the DSC enable register to 1 */
    hdmi_clrset(ctrl->regs, REG_DSC_CTRL, REG_DSC_ENABLE_M, reg_dsc_enable(true));

    /* step 3: Configure the DSC parameter refresh register to 1 */
    hdmi_clrset(ctrl->regs, REG_DSC_REG_UPDATE, REG_DSC_RUPD_M, reg_dsc_rupd(true));

    return 0;
}

/* DSC ctrl disable */
hi_s32 hi_hdmi_dsc_disable(struct dsc_ctrl *ctrl)
{
    hdmi_clrset(ctrl->regs, REG_VID_ADAPTER,
                REG_VID_IN_DSC_MODE_M, reg_vid_in_dsc_mode(false));

    /* step 1: Configure the DSC enable register to 0 */
    hdmi_clrset(ctrl->regs, REG_DSC_CTRL, REG_DSC_ENABLE_M, reg_dsc_enable(false));

    /* step 2: Configure the DSC parameter refresh register to 1 */
    hdmi_clrset(ctrl->regs, REG_DSC_REG_UPDATE, REG_DSC_RUPD_M, reg_dsc_rupd(true));

    return 0;
}

/* DSC ctrl config */
hi_s32 hi_hdmi_dsc_config(struct dsc_ctrl *ctrl)
{
    dsc_hw_set_timing_info(ctrl);
    dsc_hw_set_alg_params_part_one(ctrl);
    dsc_hw_set_alg_params_part_two(ctrl);
    dsc_hw_set_alg_params_part_three(ctrl);

    return 0;
}

/* get DSC ctrl status */
hi_s32 hi_hdmi_dsc_get_status(struct dsc_ctrl *ctrl)
{
    struct dsc_status *sta = &ctrl->status;
    hi_s32 val;

    val = hdmi_readl(ctrl->regs, REG_DSC_CTRL);
    sta->enable = val & REG_DSC_ENABLE_M;

    val = hdmi_readl(ctrl->regs, REG_DSC_ENC_8_DEBUG);
    sta->isync_fifo_full = (val & REG_ISYNC_FIFO_FULL_M) >> 8; /* isync_fifo_full need right shift 8 */
    sta->o_intf_low_bw = (val & REG_O_INTF_LOW_BW_M) >> 9; /* o_intf_low_bw need right shift 9 */
    sta->rc_error_vld = (val & REG_RC_ERR_VLD_M) >> 12; /* rc_error_vld need right shift 12 */

    return 0;
}

/* DSC ctrl deinit */
void hi_hdmi_dsc_deinit(struct dsc_ctrl *ctrl)
{
    osal_kfree(HI_ID_HDMITX, ctrl);
}

