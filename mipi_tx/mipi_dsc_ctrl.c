/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi dsc ctrl module
* Author: sdk
* Create: 2019-11-20
*/
#include <linux/slab.h>

#include "hi_type.h"
#include "mipi_dsc_reg.h"
#include "mipi_dsc_ctrl.h"
#include "hi_drv_mipi.h"
#include "drv_mipi_tx_define.h"
#include "hi_drv_osal.h"

#define MIPI_DSC_REGS_ADDR    0x016D0000
#define MIPI_DSC_REGS_SIZE    0x100
#define MIPI_DSC_IRQ          (222 + 32)
#define MIPI_DSC_IRQ_NAME     "int_mipitx_dsc"
#define MAX_DSC_CTRL_NUM      1

volatile mipi_dsc_regs_type *g_mipi_dsc_regs[MAX_DSC_CTRL_NUM] = {HI_NULL};
mipi_dsc_controller *g_mipi_dsc_ctr_cfg[MAX_DSC_CTRL_NUM] = {HI_NULL};
hi_u32 g_mipi_dsc_irq_satus[MAX_DSC_CTRL_NUM] = {0}; /* 0: not initialized, 1:initialized */

static hi_s32 mipi_dsc_isr(hi_s32 irq, hi_void *dev_id)
{
    hi_u32 rc_err_type;
    mipi_dsc_int_sta dsc_int_sta_reg = g_mipi_dsc_regs[0]->dsc_int_sta;
    mipi_dsc_enc_0_debug dsc_enc_0_debug_reg = g_mipi_dsc_regs[0]->dsc_enc_0_debug;

    if (dev_id == HI_NULL) {
        hi_err_mipi("irq(%d) func err.\n", irq);
    }

    if (dsc_int_sta_reg.bits.enc0_rc_err_sta == 0x1) {
        rc_err_type = dsc_enc_0_debug_reg.bits.rc_err_type_0;

        if (rc_err_type == 0x1) {
            g_mipi_dsc_ctr_cfg[0]->rc_err_count.buffer_fullnes_exceed_num++;
        } else if (rc_err_type == 0x2) {
            g_mipi_dsc_ctr_cfg[0]->rc_err_count.buffer_fullnes_over_flow_num++;
        } else if (rc_err_type == 0x3) {
            g_mipi_dsc_ctr_cfg[0]->rc_err_count.buffer_fullnes_under_flow_num++;
        } else {
            g_mipi_dsc_ctr_cfg[0]->rc_err_count.buffer_fullnes_err_type++;
        }

        dsc_int_sta_reg.bits.enc0_rc_err_sta = 0x1;
    }

    if (dsc_int_sta_reg.bits.dsc_frm_done_sta == 0x1) {
        ; /* do nothing */
        dsc_int_sta_reg.bits.dsc_frm_done_sta = 0x1;
    }

    g_mipi_dsc_regs[0]->dsc_int_sta = dsc_int_sta_reg; /* rw1c */

    return IRQ_HANDLED;
}

static hi_void mipi_dsc_enable_int(mipi_dsc_id dsc_id, hi_s32 enable)
{
    if (enable == 0x1) {
        g_mipi_dsc_ctr_cfg[dsc_id]->rc_err_count.buffer_fullnes_exceed_num = 0;
        g_mipi_dsc_ctr_cfg[dsc_id]->rc_err_count.buffer_fullnes_over_flow_num = 0;
        g_mipi_dsc_ctr_cfg[dsc_id]->rc_err_count.buffer_fullnes_under_flow_num = 0;
        g_mipi_dsc_ctr_cfg[dsc_id]->rc_err_count.buffer_fullnes_err_type = 0;
    }

    g_mipi_dsc_regs[dsc_id]->dsc_int_mask.bits.enc0_rc_err_mask = enable & 0x1;
    g_mipi_dsc_regs[dsc_id]->dsc_int_mask.bits.dsc_frm_done_mask = enable & 0x1;
}

static hi_s32 mipi_dsc_register_irq(mipi_dsc_id dsc_id)
{
    hi_s32 ret;

    if (dsc_id != 0) {
        hi_err_mipi("dsc_id(%d) not support.\n", dsc_id);
        return HI_FAILURE;
    }

    if (g_mipi_dsc_irq_satus[dsc_id] == 1) {
        hi_err_mipi("mipi_dsc(%d) irq already initialized.\n", dsc_id);
        return HI_SUCCESS;
    }

    mipi_dsc_enable_int(dsc_id, 0x1);
    ret = hi_drv_osal_request_irq(MIPI_DSC_IRQ, (irq_handler_t)mipi_dsc_isr, IRQF_SHARED,
                                  MIPI_DSC_IRQ_NAME, mipi_dsc_isr);
    if (ret < 0) {
        hi_err_mipi("dsc register irq error.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_void mipi_dsc_unregister_irq(mipi_dsc_id dsc_id)
{
    if (dsc_id != 0) {
        hi_err_mipi("dsc_id(%d) not support.\n", dsc_id);
        return;
    }

    if (g_mipi_dsc_irq_satus[dsc_id] == 0) {
        hi_err_mipi("mipi_dsc(%d) irq not initialized.\n", dsc_id);
        return;
    }

    hi_drv_osal_free_irq(MIPI_DSC_IRQ, MIPI_DSC_IRQ_NAME, mipi_dsc_isr);

    mipi_dsc_enable_int(0x0, 0x0);
}

/* compute sync dealy time */
static hi_s32 mipi_dsc_delay_time(const dsc_cfg_s *cfg)
{
    hi_s32 delay_time;

    delay_time = (cfg->native_422 | cfg->native_420) ?
                (((cfg->bits_per_pixel / 16) < 26) ? 3840 : 3072) : /* 16,26,3840,3072: ref to vesa manual */
                (((cfg->bits_per_pixel / 16) < 12) ? 3840 : 2560); /* 12,16,3840,2560: ref to vesa manual */

    return delay_time;
}

/* DSC ctrl init */
hi_s32 hi_mipi_dsc_init(mipi_dsc_id dsc_id)
{
    hi_s32 ret;

    if (dsc_id >= MIPI_DSC_ID_MAX) {
        hi_err_mipi("dsc_id(%d) invalid.\n", dsc_id);
        return HI_FAILURE;
    }

    if (g_mipi_dsc_ctr_cfg[dsc_id] != HI_NULL) {
        hi_err_mipi("dsc_id(%d) already inited\n", dsc_id);
        return HI_SUCCESS;
    }

    g_mipi_dsc_ctr_cfg[dsc_id] = kzalloc(sizeof(mipi_dsc_controller), GFP_KERNEL);
    if (g_mipi_dsc_ctr_cfg[dsc_id] == HI_NULL) {
        hi_err_mipi("all g_mipi_dsc_ctr_cfg(%d) failed\n", dsc_id);
        return HI_FAILURE;
    }

    g_mipi_dsc_ctr_cfg[dsc_id]->version = 1;

    /* dsc reg init */
    g_mipi_dsc_regs[dsc_id] = (volatile mipi_dsc_regs_type *)ioremap_nocache(MIPI_DSC_REGS_ADDR, MIPI_DSC_REGS_SIZE);
    if (g_mipi_dsc_regs[dsc_id] == HI_NULL) {
        hi_err_mipi("remap mipi dsc reg failed\n");
        return HI_FAILURE;
    }

    ret = hi_mipi_dsc_get_status(dsc_id);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("hi_mipi_dsc_get_status failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void mipi_enable_dsc(mipi_dsc_id dsc_id)
{
    /* check if need to add delay time to this function */
    mipi_dsc_ctrl dsc_ctrl_reg;

    dsc_ctrl_reg = g_mipi_dsc_regs[dsc_id]->dsc_ctrl;
    dsc_ctrl_reg.bits.dsc_enable = 0x1;
    dsc_ctrl_reg.bits.dsc_ck_gt_en = 0x1;
    dsc_ctrl_reg.bits.timing_polarity = 0x1;
    g_mipi_dsc_regs[dsc_id]->dsc_ctrl = dsc_ctrl_reg;
}

static hi_void mipi_disable_dsc(mipi_dsc_id dsc_id)
{
    /* check if need to add delay time to this function */
    mipi_dsc_ctrl dsc_ctrl_reg;

    dsc_ctrl_reg = g_mipi_dsc_regs[dsc_id]->dsc_ctrl;
    dsc_ctrl_reg.bits.dsc_enable = 0x0;
    dsc_ctrl_reg.bits.dsc_ck_gt_en = 0x0;
    dsc_ctrl_reg.bits.timing_polarity = 0x0;
    g_mipi_dsc_regs[dsc_id]->dsc_ctrl = dsc_ctrl_reg;
}

/* DSC ctrl enable */
hi_s32 hi_mipi_dsc_enable(mipi_dsc_id dsc_id, para_input_s *inpara)
{
    hi_s32 ret;
    dsc_cfg_s actual_cfg;

    if (dsc_id >= MIPI_DSC_ID_MAX) {
        hi_err_mipi("dsc_id(%d) invalid.\n", dsc_id);
        return HI_FAILURE;
    }

    /* need to configure mipi tx reg OPERATION_MODE->dsc_soft_rst????? */
    /* step 1: Configure parameters to logic */
    hi_dsc_cfg_compute(inpara, &actual_cfg); /* the function need come from dsc group */
    ret = hi_mipi_set_dsc_config(dsc_id, &actual_cfg);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("set_dsc_config failed.\n");
        return ret;
    }

    /* step 2: enable dsc_ctr */
    mipi_enable_dsc(dsc_id);

    /* step 3: register irq */
    ret = mipi_dsc_register_irq(dsc_id);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("set_dsc_config failed.\n");
        return ret;
    }

    return HI_SUCCESS;
}

/* DSC ctrl disable */
hi_s32 hi_mipi_dsc_disable(mipi_dsc_id dsc_id)
{
    if (dsc_id >= MIPI_DSC_ID_MAX) {
        hi_err_mipi("dsc_id(%d) invalid.\n", dsc_id);
        return HI_FAILURE;
    }

    /* step 1: unregister irq */
    mipi_dsc_unregister_irq(dsc_id);

    /* step 2: disable dsc_ctr */
    mipi_disable_dsc(dsc_id);

    return HI_SUCCESS;
}

static hi_s32 set_dsc_config_1(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    mipi_dsc_rc_bits dsc_rc_bits_reg;
    mipi_dsc_alg_ctrl dsc_alg_ctrl_reg;
    mipi_dsc_bpc dsc_bpc_reg;
    mipi_dsc_sample dsc_sample_reg;
    mipi_dsc_bpp_chk dsc_bpp_chk_reg;
    mipi_dsc_pic_reso dsc_pic_reso_reg;
    mipi_dsc_slc_reso dsc_slc_reso_reg;
    mipi_dsc_init_xmit_dly dsc_init_xmit_dly_reg;
    mipi_dsc_init_dec_dly dsc_init_dec_dly_reg;

    dsc_rc_bits_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_bits.u32;
    dsc_rc_bits_reg.bits.rcb_bits = dsc_cfg->rcb_bits;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_bits.u32 = dsc_rc_bits_reg.u32;

    dsc_alg_ctrl_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_alg_ctrl.u32;
    dsc_alg_ctrl_reg.bits.full_ich_err_precision = dsc_cfg->full_ich_err_precision;
    dsc_alg_ctrl_reg.bits.block_pred_enable = dsc_cfg->block_pred_enable;
    g_mipi_dsc_regs[dsc_id]->dsc_alg_ctrl.u32 = dsc_alg_ctrl_reg.u32;

    dsc_bpc_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_bpc.u32;
    dsc_bpc_reg.bits.bits_per_component = dsc_cfg->bits_per_component;
    g_mipi_dsc_regs[dsc_id]->dsc_bpc.u32 = dsc_bpc_reg.u32;

    dsc_sample_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_sample.u32;
    dsc_sample_reg.bits.convert_rgb = dsc_cfg->convert_rgb;
    dsc_sample_reg.bits.native_420 = dsc_cfg->native_420;
    dsc_sample_reg.bits.native_422 = dsc_cfg->native_422;
    g_mipi_dsc_regs[dsc_id]->dsc_sample.u32 = dsc_sample_reg.u32;

    dsc_bpp_chk_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_bpp_chk.u32;
    dsc_bpp_chk_reg.bits.bits_per_pixel = dsc_cfg->bits_per_pixel;
    dsc_bpp_chk_reg.bits.chunk_size = dsc_cfg->chunk_size;
    g_mipi_dsc_regs[dsc_id]->dsc_bpp_chk.u32 = dsc_bpp_chk_reg.u32;

    dsc_pic_reso_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_pic_reso.u32;
    dsc_pic_reso_reg.bits.pic_width = dsc_cfg->pic_width;
    dsc_pic_reso_reg.bits.pic_height = dsc_cfg->pic_height;
    g_mipi_dsc_regs[dsc_id]->dsc_pic_reso.u32 = dsc_pic_reso_reg.u32;

    dsc_slc_reso_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_slc_reso.u32;
    dsc_slc_reso_reg.bits.slice_width = dsc_cfg->slice_width;
    dsc_slc_reso_reg.bits.slice_height = dsc_cfg->slice_height;
    g_mipi_dsc_regs[dsc_id]->dsc_slc_reso.u32 = dsc_slc_reso_reg.u32;

    dsc_init_xmit_dly_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_init_xmit_dly.u32;
    dsc_init_xmit_dly_reg.bits.initial_xmit_delay = dsc_cfg->initial_xmit_delay;
    g_mipi_dsc_regs[dsc_id]->dsc_init_xmit_dly.u32 = dsc_init_xmit_dly_reg.u32;

    dsc_init_dec_dly_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_init_dec_dly.u32;
    dsc_init_dec_dly_reg.bits.initial_dec_delay = dsc_cfg->initial_dec_delay;
    g_mipi_dsc_regs[dsc_id]->dsc_init_dec_dly.u32 = dsc_init_dec_dly_reg.u32;

    return HI_SUCCESS;
}

static hi_s32 set_dsc_config_2(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    mipi_dsc_init_scale dsc_init_scale_reg;
    mipi_dsc_scale_intval dsc_scale_intval_reg;
    mipi_dsc_first_bpg dsc_first_bpg_reg;
    mipi_dsc_second_bpg dsc_second_bpg_reg;

    dsc_init_scale_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_init_scale.u32;
    dsc_init_scale_reg.bits.initial_scale_value = dsc_cfg->initial_scale_value;
    g_mipi_dsc_regs[dsc_id]->dsc_init_scale.u32 = dsc_init_scale_reg.u32;

    dsc_scale_intval_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_scale_intval.u32;
    dsc_scale_intval_reg.bits.scale_decrement_interval = dsc_cfg->scale_decrement_interval;
    dsc_scale_intval_reg.bits.scale_increment_interval = dsc_cfg->scale_increment_interval;
    g_mipi_dsc_regs[dsc_id]->dsc_scale_intval.u32 = dsc_scale_intval_reg.u32;

    dsc_first_bpg_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_first_bpg.u32;
    dsc_first_bpg_reg.bits.nfl_bpg_offset = dsc_cfg->nfl_bpg_offset;
    dsc_first_bpg_reg.bits.first_line_bpg_ofs = dsc_cfg->first_line_bpg_ofs;
    g_mipi_dsc_regs[dsc_id]->dsc_first_bpg.u32 = dsc_first_bpg_reg.u32;

    dsc_second_bpg_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_second_bpg.u32;
    dsc_second_bpg_reg.bits.nsl_bpg_offset = dsc_cfg->nsl_bpg_offset;
    dsc_second_bpg_reg.bits.second_line_bpg_ofs = dsc_cfg->second_line_bpg_ofs;
    g_mipi_dsc_regs[dsc_id]->dsc_second_bpg = dsc_second_bpg_reg;

    return HI_SUCCESS;
}

static hi_s32 set_dsc_config_3(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    mipi_dsc_second_adj dsc_second_adj_reg;
    mipi_dsc_init_finl_ofs dsc_init_finl_ofs_reg;
    mipi_dsc_slc_bpg dsc_slc_bpg_reg;
    mipi_dsc_flat_range dsc_flat_range_reg;
    mipi_dsc_rc_mod_edge dsc_rc_mod_edge_reg;
    mipi_dsc_rc_qua_tgt dsc_rc_qua_tgt_reg;

    dsc_second_adj_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_second_adj.u32;
    dsc_second_adj_reg.bits.second_line_ofs_adj = dsc_cfg->second_line_ofs_adj;
    g_mipi_dsc_regs[dsc_id]->dsc_second_adj.u32 = dsc_second_adj_reg.u32;

    dsc_init_finl_ofs_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_init_finl_ofs.u32;
    dsc_init_finl_ofs_reg.bits.final_offset = dsc_cfg->final_offset;
    dsc_init_finl_ofs_reg.bits.initial_offset = dsc_cfg->initial_offset;
    g_mipi_dsc_regs[dsc_id]->dsc_init_finl_ofs.u32 = dsc_init_finl_ofs_reg.u32;

    dsc_slc_bpg_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_slc_bpg.u32;
    dsc_slc_bpg_reg.bits.slice_bpg_offset = dsc_cfg->slice_bpg_offset;
    g_mipi_dsc_regs[dsc_id]->dsc_slc_bpg.u32 = dsc_slc_bpg_reg.u32;

    dsc_flat_range_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_flat_range.u32;
    dsc_flat_range_reg.bits.flatness_max_qp = dsc_cfg->flatness_max_qp;
    dsc_flat_range_reg.bits.flatness_min_qp = dsc_cfg->flatness_min_qp;
    g_mipi_dsc_regs[dsc_id]->dsc_flat_range.u32 = dsc_flat_range_reg.u32;

    dsc_rc_mod_edge_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_mod_edge.u32;
    dsc_rc_mod_edge_reg.bits.rc_edge_factor = dsc_cfg->rc_edge_factor;
    dsc_rc_mod_edge_reg.bits.rc_model_size = dsc_cfg->rc_model_size;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_mod_edge.u32 = dsc_rc_mod_edge_reg.u32;

    dsc_rc_qua_tgt_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_qua_tgt.u32;
    dsc_rc_qua_tgt_reg.bits.rc_quant_incr_limit0 = dsc_cfg->rc_quant_incr_limit0;
    dsc_rc_qua_tgt_reg.bits.rc_quant_incr_limit1 = dsc_cfg->rc_quant_incr_limit1;
    dsc_rc_qua_tgt_reg.bits.rc_tgt_offset_hi = dsc_cfg->rc_tgt_offset_hi;
    dsc_rc_qua_tgt_reg.bits.rc_tgt_offset_lo = dsc_cfg->rc_tgt_offset_lo;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_qua_tgt.u32 = dsc_rc_qua_tgt_reg.u32;

    return HI_SUCCESS;
}

static hi_s32 set_dsc_config_4(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    hi_u32 i;

    mipi_dsc_rc_thre_01 dsc_rc_thre_01_reg;
    mipi_dsc_rc_thre_02 dsc_rc_thre_02_reg;
    mipi_dsc_rc_thre_03 dsc_rc_thre_03_reg;
    mipi_dsc_rc_thre_04 dsc_rc_thre_04_reg;

    i = 0;
    dsc_rc_thre_01_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_01.u32;
    dsc_rc_thre_01_reg.bits.rc_buf_thresh_00 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_01_reg.bits.rc_buf_thresh_01 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_01_reg.bits.rc_buf_thresh_02 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_01_reg.bits.rc_buf_thresh_03 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);

    dsc_rc_thre_02_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_02.u32;
    dsc_rc_thre_02_reg.bits.rc_buf_thresh_04 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_02_reg.bits.rc_buf_thresh_05 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_02_reg.bits.rc_buf_thresh_06 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_02_reg.bits.rc_buf_thresh_07 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);

    dsc_rc_thre_03_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_03.u32;
    dsc_rc_thre_03_reg.bits.rc_buf_thresh_08 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_03_reg.bits.rc_buf_thresh_09 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_03_reg.bits.rc_buf_thresh_10 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_03_reg.bits.rc_buf_thresh_11 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);

    dsc_rc_thre_04_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_04.u32;
    dsc_rc_thre_04_reg.bits.rc_buf_thresh_12 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);
    dsc_rc_thre_04_reg.bits.rc_buf_thresh_13 = (dsc_cfg->rc_buf_thresh[i++] / 0x40);

    g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_01.u32 = dsc_rc_thre_01_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_02.u32 = dsc_rc_thre_02_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_03.u32 = dsc_rc_thre_03_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_thre_04.u32 = dsc_rc_thre_04_reg.u32;

    return HI_SUCCESS;
}

static hi_s32 set_dsc_config_5(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    hi_u32 i;

    mipi_dsc_rc_param_00 dsc_rc_param_00_reg;
    mipi_dsc_rc_param_01 dsc_rc_param_01_reg;
    mipi_dsc_rc_param_02 dsc_rc_param_02_reg;
    mipi_dsc_rc_param_03 dsc_rc_param_03_reg;
    mipi_dsc_rc_param_04 dsc_rc_param_04_reg;
    mipi_dsc_rc_param_05 dsc_rc_param_05_reg;

    i = 0;
    dsc_rc_param_00_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_00.u32;
    dsc_rc_param_00_reg.bits.range_bpg_offset_00 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_00_reg.bits.range_max_qp_00 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_00_reg.bits.range_min_qp_00 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_01_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_01.u32;
    dsc_rc_param_01_reg.bits.range_bpg_offset_01 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_01_reg.bits.range_max_qp_01 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_01_reg.bits.range_min_qp_01 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_02_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_02.u32;
    dsc_rc_param_02_reg.bits.range_bpg_offset_02 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_02_reg.bits.range_max_qp_02 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_02_reg.bits.range_min_qp_02 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_03_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_03.u32;
    dsc_rc_param_03_reg.bits.range_bpg_offset_03 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_03_reg.bits.range_max_qp_03 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_03_reg.bits.range_min_qp_03 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_04_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_04.u32;
    dsc_rc_param_04_reg.bits.range_bpg_offset_04 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_04_reg.bits.range_max_qp_04 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_04_reg.bits.range_min_qp_04 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_05_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_05.u32;
    dsc_rc_param_05_reg.bits.range_bpg_offset_05 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_05_reg.bits.range_max_qp_05 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_05_reg.bits.range_min_qp_05 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_00.u32 = dsc_rc_param_00_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_01.u32 = dsc_rc_param_01_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_02.u32 = dsc_rc_param_02_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_03.u32 = dsc_rc_param_03_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_04.u32 = dsc_rc_param_04_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_05.u32 = dsc_rc_param_05_reg.u32;

    return HI_SUCCESS;
}

static hi_s32 set_dsc_config_6(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    hi_u32 i = 6; /* start index 6 */

    mipi_dsc_rc_param_06 dsc_rc_param_06_reg;
    mipi_dsc_rc_param_07 dsc_rc_param_07_reg;
    mipi_dsc_rc_param_08 dsc_rc_param_08_reg;
    mipi_dsc_rc_param_09 dsc_rc_param_09_reg;

    dsc_rc_param_06_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_06.u32;
    dsc_rc_param_06_reg.bits.range_bpg_offset_06 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_06_reg.bits.range_max_qp_06 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_06_reg.bits.range_min_qp_06 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_07_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_07.u32;
    dsc_rc_param_07_reg.bits.range_bpg_offset_07 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_07_reg.bits.range_max_qp_07 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_07_reg.bits.range_min_qp_07 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_08_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_08.u32;
    dsc_rc_param_08_reg.bits.range_bpg_offset_08 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_08_reg.bits.range_max_qp_08 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_08_reg.bits.range_min_qp_08 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_09_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_09.u32;
    dsc_rc_param_09_reg.bits.range_bpg_offset_09 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_09_reg.bits.range_max_qp_09 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_09_reg.bits.range_min_qp_09 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_06.u32 = dsc_rc_param_06_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_07.u32 = dsc_rc_param_07_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_08.u32 = dsc_rc_param_08_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_09.u32 = dsc_rc_param_09_reg.u32;

    return HI_SUCCESS;
}

static hi_s32 set_dsc_config_7(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    hi_u32 i = 10; /* start index 10 */

    mipi_dsc_rc_param_10 dsc_rc_param_10_reg;
    mipi_dsc_rc_param_11 dsc_rc_param_11_reg;
    mipi_dsc_rc_param_12 dsc_rc_param_12_reg;
    mipi_dsc_rc_param_13 dsc_rc_param_13_reg;
    mipi_dsc_rc_param_14 dsc_rc_param_14_reg;

    dsc_rc_param_10_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_10.u32;
    dsc_rc_param_10_reg.bits.range_bpg_offset_10 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_10_reg.bits.range_max_qp_10 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_10_reg.bits.range_min_qp_10 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_11_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_11.u32;
    dsc_rc_param_11_reg.bits.range_bpg_offset_11 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_11_reg.bits.range_max_qp_11 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_11_reg.bits.range_min_qp_11 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_12_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_12.u32;
    dsc_rc_param_12_reg.bits.range_bpg_offset_12 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_12_reg.bits.range_max_qp_12 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_12_reg.bits.range_min_qp_12 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_13_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_13.u32;
    dsc_rc_param_13_reg.bits.range_bpg_offset_13 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_13_reg.bits.range_max_qp_13 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_13_reg.bits.range_min_qp_13 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    dsc_rc_param_14_reg.u32 = g_mipi_dsc_regs[dsc_id]->dsc_rc_param_14.u32;
    dsc_rc_param_14_reg.bits.range_bpg_offset_14 = dsc_cfg->rc_range_parameters[i].range_bpg_offset;
    dsc_rc_param_14_reg.bits.range_max_qp_14 = dsc_cfg->rc_range_parameters[i].range_max_qp;
    dsc_rc_param_14_reg.bits.range_min_qp_14 = dsc_cfg->rc_range_parameters[i++].range_min_qp;

    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_10.u32 = dsc_rc_param_10_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_11.u32 = dsc_rc_param_11_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_12.u32 = dsc_rc_param_12_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_13.u32 = dsc_rc_param_13_reg.u32;
    g_mipi_dsc_regs[dsc_id]->dsc_rc_param_14.u32 = dsc_rc_param_14_reg.u32;

    return HI_SUCCESS;
}

/* DSC ctrl config */
hi_s32 hi_mipi_set_dsc_config(mipi_dsc_id dsc_id, const dsc_cfg_s *dsc_cfg)
{
    hi_s32 dsc_delay;
    mipi_dsc_ctrl dsc_ctrl_reg;

    if (dsc_cfg == HI_NULL) {
        hi_err_mipi("dsc_cfg is null.\n");
        return HI_FAILURE;
    }

    dsc_delay = mipi_dsc_delay_time(dsc_cfg);
    dsc_ctrl_reg = g_mipi_dsc_regs[dsc_id]->dsc_ctrl;
    dsc_ctrl_reg.bits.dsc_delay = dsc_delay;
    g_mipi_dsc_regs[dsc_id]->dsc_ctrl = dsc_ctrl_reg;

    if (set_dsc_config_1(dsc_id, dsc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (set_dsc_config_2(dsc_id, dsc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (set_dsc_config_3(dsc_id, dsc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (set_dsc_config_4(dsc_id, dsc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (set_dsc_config_5(dsc_id, dsc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (set_dsc_config_6(dsc_id, dsc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (set_dsc_config_7(dsc_id, dsc_cfg) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* Get DSC ctrl status */
hi_s32 hi_mipi_dsc_get_status(mipi_dsc_id dsc_id)
{
    mipi_dsc_status *sta = HI_NULL;
    mipi_dsc_ctrl val;

    sta = &(g_mipi_dsc_ctr_cfg[dsc_id]->status);
    val.u32 = g_mipi_dsc_regs[dsc_id]->dsc_ctrl.u32;

    sta->dsc_enable = (val.bits.dsc_enable == 0x1) ? HI_TRUE : HI_FALSE;
    sta->dsc_ck_gt_en = (val.bits.dsc_ck_gt_en == 0x1) ? HI_TRUE : HI_FALSE;

    return HI_SUCCESS;
}

/* DSC ctrl deinit */
void hi_mipi_dsc_deinit(mipi_dsc_id dsc_id)
{
    if (dsc_id >= MIPI_DSC_ID_MAX) {
        hi_err_mipi("dsc_id(%d) invalid.\n", dsc_id);
        return ;
    }

    if (g_mipi_dsc_ctr_cfg[dsc_id] != HI_NULL) {
        kfree(g_mipi_dsc_ctr_cfg[dsc_id]);
    }

    if (g_mipi_dsc_regs[dsc_id] != HI_NULL) {
        iounmap((hi_void *)g_mipi_dsc_regs[dsc_id]);
    }
}
