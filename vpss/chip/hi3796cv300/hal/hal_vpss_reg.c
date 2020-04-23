/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_reg.c source file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */
#include "vpss_comm.h"
#include "hal_vpss_reg.h"
#include "vpss_define.h"
#include "hal_vpss_cmp_frm_nr.h"
#include "hal_vpss_cmp_line_out0.h"
#ifndef __VPSS_UT__
#include "hi_reg_common.h"

static volatile hi_reg_crg *g_reg_crg = HI_NULL;
#endif

#define MMU_OFFSET           (0x800 / 4)
#define READ_CHAN_OFFSET     (0x80 / 4)
#define READ_STT_CHAN_OFFSET (0x20 / 4)

#define WRITE_CHAN_OFFSET     (0x40 / 4)
#define WRITE_STT_CHAN_OFFSET (0x20 / 4)

#define OFFSET_4 4

#define VPSS_RCHN_MAX ((hi_u32)(VPSS_MAC_RCHN_MAX))
#define VPSS_WCHN_MAX ((hi_u32)(VPSS_MAC_WCHN_MAX))

static const hi_u32 g_gu32_vpss_rchn_addr[VPSS_RCHN_MAX] = {
    (0x0080 / OFFSET_4), /* img  rchn_cf */
    (0x0100 / OFFSET_4), /* img  rchn_di_cf */
    (0x0180 / OFFSET_4), /* img  rchn_di_p1 */
    (0x0200 / OFFSET_4), /* img  rchn_di_p2 */
    (0x0280 / OFFSET_4), /* img  rchn_di_p3 */
    (0x0300 / OFFSET_4), /* img  rchn_di_p3i */
    (0x0380 / OFFSET_4), /* img  rchn_cc_pr0 */
    (0x0400 / OFFSET_4), /* img  rchn_cc_pr4 */
    (0x0480 / OFFSET_4), /* img  rchn_cc_pr8 */
    (0x0500 / OFFSET_4), /* img  rchn_nr_ref */
    (0x0580 / OFFSET_4), /* solo rchn_di_rsady */
    (0x05a0 / OFFSET_4), /* solo rchn_di_rsadc */
    (0x05c0 / OFFSET_4), /* solo rchn_di_rhism */
    (0x05e0 / OFFSET_4), /* solo rchn_di_p1mv */
    (0x0600 / OFFSET_4), /* solo rchn_di_p2mv */
    (0x0620 / OFFSET_4), /* solo rchn_di_p3mv */
    (0x0640 / OFFSET_4), /* solo rchn_cc_rccnt */
    (0x0660 / OFFSET_4), /* solo rchn_cc_rycnt */
    (0x0680 / OFFSET_4), /* solo rchn_nr_rmad */
    (0x06a0 / OFFSET_4), /* solo rchn_snr_rmad */
    (0x06c0 / OFFSET_4), /* solo rchn_nr_cfmv */
    (0x06e0 / OFFSET_4), /* solo rchn_nr_p1rgmv */
    (0x0700 / OFFSET_4), /* solo rchn_nr_p2rgmv */
    (0x0720 / OFFSET_4), /* solo rchn_nr_rcnt */
    (0x0740 / OFFSET_4), /* solo rchn_rg_rprjh */
    (0x0760 / OFFSET_4), /* solo rchn_rg_rprjv */
    (0x0780 / OFFSET_4), /* solo rchn_rg_p1rgmv */
    (0x07a0 / OFFSET_4), /* solo rchn_rg_p2rgmv */
    (0x07c0 / OFFSET_4), /* solo rchn_dmcnt */
    ((0xe000 - 0x5800 - 0x1100 + 0x1100) / OFFSET_4), /* img  rchn_me_cf */
    ((0xe000 - 0x5800 - 0x1100 + 0x1180) / OFFSET_4), /* img  rchn_me_ref */
    ((0xe000 - 0x5800 - 0x1100 + 0x1200) / OFFSET_4), /* solo rchn_me_p1mv */
    ((0xe000 - 0x5800 - 0x1100 + 0x1220) / OFFSET_4), /* solo rchn_me_prmv */
    ((0xe000 - 0x5800 - 0x1100 + 0x1240) / OFFSET_4), /* solo rchn_me_p1gmv */
    ((0xe000 - 0x5800 - 0x1100 + 0x1260) / OFFSET_4), /* solo rchn_me_prgmv */
    ((0xe000 - 0x5800 - 0x1100 + 0x1280) / OFFSET_4), /* solo rchn_me_p1rgmv */
    ((0xe000 - 0x5800 - 0x1100 + 0x12a0) / OFFSET_4)  /* solo rchn_me_cfrgmv */
};

static const hi_u32 g_gu32_vpss_wchn_addr[VPSS_WCHN_MAX] = {
    (0x0800 / OFFSET_4), /* img  wchn_out0 */
#ifdef TOBEMODIFY /* todo */
    (0x0b40 / OFFSET_4), /* img  wchn_out1 todo new add */
    (0x0b80 / OFFSET_4), /* img  wchn_out2 todo new add */
#endif
    (0x0840 / OFFSET_4), /* img  wchn_out3 */
    (0x0880 / OFFSET_4), /* img  wchn_nr_rfr */
    (0x08c0 / OFFSET_4), /* img  wchn_nr_rfrh */
    (0x0900 / OFFSET_4), /* img  wchn_nr_rfrhv */
    (0x0940 / OFFSET_4), /* img  wchn_cc_rfr */
    (0x0980 / OFFSET_4), /* img  wchn_cc_rfr1 */
    (0x09c0 / OFFSET_4), /* img  wchn_di_rfr */
    (0x0a00 / OFFSET_4), /* solo wchn_nr_wmad */
    (0x0a20 / OFFSET_4), /* solo wchn_nr_wcnt */
    (0x0a40 / OFFSET_4), /* solo wchn_cc_wccnt */
    (0x0a60 / OFFSET_4), /* solo wchn_cc_wycnt */
    (0x0a80 / OFFSET_4), /* solo wchn_di_wsady */
    (0x0aa0 / OFFSET_4), /* solo wchn_di_wsadc */
    (0x0ac0 / OFFSET_4), /* solo wchn_di_whism */
    (0x0ae0 / OFFSET_4), /* solo wchn_rg_wprjh */
    (0x0b00 / OFFSET_4), /* solo wchn_rg_wprjv */
    (0x0b20 / OFFSET_4), /* solo wchn_rg_cfrgmv */
    (0x0bc0 / OFFSET_4), /* solo wchn wdmcnt */
    ((0xe1C0 - 0x5800 - 0x012c0 + 0x012c0) / OFFSET_4), /* solo wchn_me_cfmv */
    ((0xe1C0 - 0x5800 - 0x012c0 + 0x012e0) / OFFSET_4)  /* solo wchn_me_cfgmv */
};

/* channel max reso */
const hi_u32 g_vpss_rchn_max_wth[VPSS_RCHN_MAX] = {
    8192,  /* img  rchn_cf */
    4096,  /* img  rchn_di_cf */
    1920,  /* img  rchn_di_p1 */
    1920,  /* img  rchn_di_p2 */
    1920,  /* img  rchn_di_p3 */
    1920,  /* img  rchn_di_p3i */
    960,   /* img  rchn_cc_pr0 */
    960,   /* img  rchn_cc_pr4 */
    960,   /* img  rchn_cc_pr8 */
    3840,  /* img  rchn_nr_ref */
    1920,  /* solo rchn_di_rsady */
    960,   /* solo rchn_di_rsadc */
    480,   /* solo rchn_di_rhism */
    240,   /* solo rchn_di_p1mv */
    240,   /* solo rchn_di_p2mv */
    240,   /* solo rchn_di_p3mv */
    960,   /* solo rchn_cc_rccnt */
    960,   /* solo rchn_cc_rycnt */
    3840,  /* solo rchn_nr_rmad */
    3840,  /* solo rchn_snr_rmad */
    240,   /* solo rchn_nr_cfmv */
    32,    /* solo rchn_nr_p1rgmv */
    32,    /* solo rchn_nr_p2rgmv */
    1920,  /* solo rchn_nr_rcnt */
    480,   /* solo rchn_rg_rprjh */
    480,   /* solo rchn_rg_rprjv */
    32,    /* solo rchn_rg_p1rgmv */
    32,    /* solo rchn_rg_p2rgmv */
    1024,  /* solo rchn_dmcnt */
    1920, /* img  rchn_me_cf */
    1920, /* img  rchn_me_ref */
    240,  /* solo rchn_me_p1mv */
    240,  /* solo rchn_me_prmv */
    32,   /* solo rchn_me_p1gmv */
    32,   /* solo rchn_me_prgmv */
    32,   /* solo rchn_me_p1rgmv */
    32    /* solo rchn_me_cfrgmv */
};

const hi_u32 g_vpss_rchn_max_hgt[VPSS_RCHN_MAX] = {
    2160,  /* img  rchn_cf */
    2160,  /* img  rchn_di_cf */
    1080,  /* img  rchn_di_p1 */
    1080,  /* img  rchn_di_p2 */
    1080,  /* img  rchn_di_p3 */
    1080,  /* img  rchn_di_p3i */
    576,   /* img  rchn_cc_pr0 */
    576,   /* img  rchn_cc_pr4 */
    576,   /* img  rchn_cc_pr8 */
    2160,  /* img  rchn_nr_ref */
    540,   /* solo rchn_di_rsady */
    540,   /* solo rchn_di_rsadc */
    540,   /* solo rchn_di_rhism */
    256,   /* solo rchn_di_p1mv */
    256,   /* solo rchn_di_p2mv */
    256,   /* solo rchn_di_p3mv */
    576,   /* solo rchn_cc_rccnt */
    576,   /* solo rchn_cc_rycnt */
    2160,  /* solo rchn_nr_rmad */
    2160,  /* solo rchn_snr_rmad */
    256,   /* solo rchn_nr_cfmv */
    128,   /* solo rchn_nr_p1rgmv */
    128,   /* solo rchn_nr_p2rgmv */
    1080,  /* solo rchn_nr_rcnt */
    256,   /* solo rchn_rg_rprjh */
    256,   /* solo rchn_rg_rprjv */
    128,   /* solo rchn_rg_p1rgmv */
    128,   /* solo rchn_rg_p2rgmv */
    576,   /* solo rchn_dmcnt */
    1080, /* img  rchn_me_cf */
    1080, /* img  rchn_me_ref */
    256,  /* solo rchn_me_p1mv */
    256,  /* solo rchn_me_prmv */
    128,  /* solo rchn_me_p1gmv */
    128,  /* solo rchn_me_prgmv */
    128,  /* solo rchn_me_p1rgmv */
    128   /* solo rchn_me_cfrgmv */
};

const hi_u32 g_vpss_wchn_max_wth[VPSS_WCHN_MAX] = {
    4096,  /* img  wchn_out0 */
#ifdef TOBEMODIFY /* todo */
    1920,  /* img  wchn_out1 */
    4096,  /* img  wchn_out2 */
#endif
    720,   /* img  wchn_out3 */
    3840,  /* img  wchn_nr_rfr */
    960,   /* img  wchn_nr_rfrh */
    960,   /* img  wchn_nr_rfrhv */
    960,   /* img  wchn_cc_rfr */
    960,   /* img  wchn_cc_rfr1 */
    1920,  /* img  wchn_di_rfr */
    3840,  /* solo wchn_nr_wmad */
    1920,  /* solo wchn_nr_wcnt */
    960,   /* solo wchn_cc_wccnt */
    960,   /* solo wchn_cc_wycnt */
    1920,  /* solo wchn_di_wsady */
    960,   /* solo wchn_di_wsadc */
    480,   /* solo wchn_di_whism */
    480,   /* solo wchn_rg_wprjh */
    480,   /* solo wchn_rg_wprjv */
    32,    /* solo wchn_rg_cfrgmv */
    1024,  /* solo wchn_dmcnt */
    240, /* solo wchn_me_cfmv */
    32   /* solo wchn_me_cfgmv */
};

const hi_u32 g_vpss_wchn_max_hgt[VPSS_WCHN_MAX] = {
    2160,  /* img  wchn_out0 */
#ifdef TOBEMODIFY /* todo */
    1080,  /* img  wchn_out1 */
    2160,  /* img  wchn_out2 */
#endif
    480,   /* img  wchn_out3 */
    2160,  /* img  wchn_nr_rfr */
    1080,  /* img  wchn_nr_rfrh */
    540,   /* img  wchn_nr_rfrhv */
    576,   /* img  wchn_cc_rfr */
    576,   /* img  wchn_cc_rfr1 */
    540,   /* img  wchn_di_rfr */
    2160,  /* solo wchn_nr_wmad */
    1080,  /* solo wchn_nr_wcnt */
    576,   /* solo wchn_cc_wccnt */
    576,   /* solo wchn_cc_wycnt */
    540,   /* solo wchn_di_wsady */
    540,   /* solo wchn_di_wsadc */
    540,   /* solo wchn_di_whism */
    256,   /* solo wchn_rg_wprjh */
    256,   /* solo wchn_rg_wprjv */
    128,   /* solo wchn_rg_cfrgmv */
    576,   /* solo wchn_dmcnt */
    256, /* solo wchn_me_cfmv */
    128  /* solo wchn_me_cfgmv */
};

#define VPSS_RCHN_IMG_START_OFFSET  (g_gu32_vpss_rchn_addr[VPSS_MAC_RCHN_CF])
#define VPSS_WCHN_IMG_START_OFFSET  (g_gu32_vpss_wchn_addr[VPSS_MAC_WCHN_OUT0])
#define VPSS_RCHN_SOLO_START_OFFSET (g_gu32_vpss_rchn_addr[VPSS_MAC_RCHN_DI_RSADY])
#define VPSS_WCHN_SOLO_START_OFFSET (g_gu32_vpss_wchn_addr[VPSS_MAC_WCHN_NR_WMAD])

hi_void vpss_reg_write(volatile hi_u32 *a, hi_u32 b)
{
    *a = b;
}

hi_u32 vpss_reg_read(volatile hi_u32 *a)
{
    return (*(a));
}

hi_void vpss_stt_get_lbd_space(vpss_stt_reg_type *vpss_stt_regs, hi_u32 *top, hi_u32 *bottom, hi_u32 *left,
                               hi_u32 *right)
{
    *top = vpss_stt_regs->vpss_wstt_lbd_stt_space_0.bits.space_top;
    *bottom = vpss_stt_regs->vpss_wstt_lbd_stt_space_0.bits.space_bot + 1;
    *right = vpss_stt_regs->vpss_wstt_lbd_stt_space_1.bits.space_right + 1;
    *left = vpss_stt_regs->vpss_wstt_lbd_stt_space_1.bits.space_left;
    return;
}

hi_void vpss_stt_get_lbd_shift(vpss_stt_reg_type *vpss_stt_regs, hi_u32 *top, hi_u32 *bottom, hi_u32 *left,
                               hi_u32 *right)
{
    *top = vpss_stt_regs->vpss_wstt_lbd_stt_space_2.bits.temp_top_shift;
    *bottom = vpss_stt_regs->vpss_wstt_lbd_stt_space_2.bits.temp_bot_shift;
    *right = vpss_stt_regs->vpss_wstt_lbd_stt_space_2.bits.temp_right_shift;
    *left = vpss_stt_regs->vpss_wstt_lbd_stt_space_2.bits.temp_left_shift;
    return;
}

hi_void vpss_sys_set_int_mask(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask)
{
    u_vpss_intmsk vpss_intmsk;
    vpss_intmsk.u32 = mask;
    vpss_reg_write((&(vpss_regs->vpss_intmsk.u32) + addr_offset), vpss_intmsk.u32);
    return;
}

hi_u32 vpss_sys_get_int_state(vpss_reg_type *vpss_regs, hi_u32 addr_offset)
{
    hi_u32 get_data;
    u_vpss_intsta vpss_intsta;
    vpss_intsta.u32 = vpss_reg_read((&(vpss_regs->vpss_intsta.u32) + addr_offset));
    get_data = vpss_intsta.u32;
    return get_data;
}

hi_void vpss_sys_set_int_clr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 data1)
{
    u_vpss_mskintsta vpss_mskintsta;
    vpss_mskintsta.u32 = data1;
    vpss_reg_write((&(vpss_regs->vpss_mskintsta.u32) + addr_offset), vpss_mskintsta.u32);
    return;
}

hi_void vpss_sys_set_ck_gt_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 ck_gt_en)
{
    u_vpss_lowpower_ctrl vpss_lowpower_ctrl;
    vpss_lowpower_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_lowpower_ctrl.u32) + addr_offset));
    vpss_lowpower_ctrl.bits.ck_gt_en = ck_gt_en;
    vpss_reg_write((&(vpss_regs->vpss_lowpower_ctrl.u32) + addr_offset), vpss_lowpower_ctrl.u32);
    return;
}

hi_void vpss_sys_set_node_rst_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 node_rst_en)
{
    u_vpss_ftconfig vpss_ftconfig;
    vpss_ftconfig.u32 = vpss_reg_read((&(vpss_regs->vpss_ftconfig.u32) + addr_offset));
    vpss_ftconfig.bits.node_rst_en = node_rst_en;
    vpss_reg_write((&(vpss_regs->vpss_ftconfig.u32) + addr_offset), vpss_ftconfig.u32);
    return;
}

hi_void vpss_sys_set_scan_rst_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_rst_en)
{
    u_vpss_ftconfig vpss_ftconfig;
    vpss_ftconfig.u32 = vpss_reg_read((&(vpss_regs->vpss_ftconfig.u32) + addr_offset));
    vpss_ftconfig.bits.scan_rst_en = scan_rst_en;
    vpss_reg_write((&(vpss_regs->vpss_ftconfig.u32) + addr_offset), vpss_ftconfig.u32);
    return;
}

hi_void vpss_sys_set_pnext(vpss_reg_type *vpss_regs, hi_u32 task_id, hi_u32 pnext_h, hi_u32 pnext_l)
{
    u_vpss_pnext_addr_h vpss_pnext_addr_h;
    u_vpss_pnext_addr_l vpss_pnext_addr_l;
    vpss_pnext_addr_h.u32 = 0;
    vpss_pnext_addr_h.bits.task_id = task_id;
    vpss_pnext_addr_h.bits.pnext_addr_h = pnext_h;
    vpss_pnext_addr_l.u32 = 0;
    vpss_pnext_addr_l.bits.task_id = task_id;
    vpss_pnext_addr_l.bits.pnext_addr_l = pnext_l;
    vpss_reg_write((&(vpss_regs->vpss_pnext_addr_h.u32)), vpss_pnext_addr_h.u32);
    vpss_reg_write((&(vpss_regs->vpss_pnext_addr_l.u32)), vpss_pnext_addr_l.u32);
    return;
}

// ======== example of the reg not in linklist ===========
hi_void vpss_sys_set_vpss_start(vpss_reg_type *vpss_regs, hi_u32 task_id, hi_u32 start)
{
    u_vpss_start vpss_start;
    vpss_start.u32 = vpss_reg_read((&(vpss_regs->vpss_start.u32)));
    vpss_start.bits.task_id = task_id;
    vpss_start.bits.start = start;
    vpss_reg_write((&(vpss_regs->vpss_start.u32)), vpss_start.u32);
    return;
}
hi_void vpss_sys_set_snr_mad_disable(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 snr_mad_disable)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.snr_mad_disable = snr_mad_disable;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}
hi_void vpss_sys_set_hfr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hfr_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.hfr_en = hfr_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}
hi_void vpss_sys_set_lbd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lbd_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.lbd_en = lbd_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}
hi_void vpss_sys_set_tnr_rec_8bit_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 tnr_rec_8bit_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.tnr_rec_8bit_en = tnr_rec_8bit_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_hcti_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hcti_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.hcti_en = hcti_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_scd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scd_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.scd_en = scd_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_vc1_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.vc1_en = vc1_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_rotate_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rotate_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.rotate_en = rotate_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_rotate_angle(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rotate_angle)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.rotate_angle = rotate_angle;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_snr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 snr_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.snr_en = snr_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_cccl_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 cccl_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.cccl_en = cccl_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_get_rgmv_addr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u64 *rgmv_addr)
{
    hi_u32 rgmv_addr_read_l;
    hi_u32 rgmv_addr_read_h;

    rgmv_addr_read_l = vpss_reg_read((&(vpss_regs->vpss_rg_cfrgmv_addr_low.u32) + addr_offset));
    rgmv_addr_read_h = vpss_reg_read((&(vpss_regs->vpss_rg_cfrgmv_addr_high.u32) + addr_offset));

    *rgmv_addr = (hi_u64)rgmv_addr_read_l | ((hi_u64)rgmv_addr_read_h << VPSS_REG_SHIFT);
    return;
}

hi_bool vpss_sys_get_cccl_en(vpss_reg_type *vpss_regs)
{
    return (hi_bool)vpss_regs->vpss_ctrl.bits.cccl_en;
}
hi_bool vpss_sys_get_vc1_en(vpss_reg_type *vpss_regs)
{
    return (hi_bool)vpss_regs->vpss_ctrl.bits.vc1_en;
}

hi_void vpss_sys_set_meds_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 meds_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.meds_en = meds_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_tnr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 tnr_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.tnr_en = tnr_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_mcnr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mcnr_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.mcnr_en = mcnr_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_dei_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 dei_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.dei_en = dei_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_mcdi_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mcdi_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.mcdi_en = mcdi_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_ifmd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 ifmd_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.ifmd_en = ifmd_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_igbm_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 igbm_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.igbm_en = igbm_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_rgme_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rgme_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.rgme_en = rgme_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_me_version(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_version)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.me_version = me_version;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_ma_mac_sel(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 ma_mac_sel)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.ma_mac_sel = ma_mac_sel;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_vpss3drs_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vpss_3drs_en)
{
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.vpss_3drs_en = vpss_3drs_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
    return;
}

hi_void vpss_sys_set_slow(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 slow)
{
    u_regload_cfg regload_cfg;
    regload_cfg.u32 = vpss_reg_read((&(vpss_regs->regload_cfg.u32) + addr_offset));
    regload_cfg.bits.slow = slow;
    vpss_reg_write((&(vpss_regs->regload_cfg.u32) + addr_offset), regload_cfg.u32);
    return;
}

hi_void vpss_sys_set_mask0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask0)
{
    u_regload_mask0 regload_mask0;
    regload_mask0.u32 = vpss_reg_read((&(vpss_regs->regload_mask0.u32) + addr_offset));
    regload_mask0.bits.mask0 = mask0;
    vpss_reg_write((&(vpss_regs->regload_mask0.u32) + addr_offset), regload_mask0.u32);
    return;
}

hi_void vpss_sys_set_mask1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask1)
{
    u_regload_mask1 regload_mask1;
    regload_mask1.u32 = vpss_reg_read((&(vpss_regs->regload_mask1.u32) + addr_offset));
    regload_mask1.bits.mask1 = mask1;
    vpss_reg_write((&(vpss_regs->regload_mask1.u32) + addr_offset), regload_mask1.u32);
    return;
}

hi_void vpss_sys_set_mask2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask2)
{
    u_regload_mask2 regload_mask2;
    regload_mask2.u32 = vpss_reg_read((&(vpss_regs->regload_mask2.u32) + addr_offset));
    regload_mask2.bits.mask2 = mask2;
    vpss_reg_write((&(vpss_regs->regload_mask2.u32) + addr_offset), regload_mask2.u32);
    return;
}

hi_void vpss_sys_set_mask3(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask3)
{
    u_regload_mask3 regload_mask3;
    regload_mask3.u32 = vpss_reg_read((&(vpss_regs->regload_mask3.u32) + addr_offset));
    regload_mask3.bits.mask3 = mask3;
    vpss_reg_write((&(vpss_regs->regload_mask3.u32) + addr_offset), regload_mask3.u32);
    return;
}

hi_void vpss_sys_set_mask4(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask4)
{
    u_regload_mask4 regload_mask4;
    regload_mask4.u32 = vpss_reg_read((&(vpss_regs->regload_mask4.u32) + addr_offset));
    regload_mask4.bits.mask4 = mask4;
    vpss_reg_write((&(vpss_regs->regload_mask4.u32) + addr_offset), regload_mask4.u32);
    return;
}

hi_void vpss_sys_set_mask5(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask5)
{
    u_regload_mask5 regload_mask5;
    regload_mask5.u32 = vpss_reg_read((&(vpss_regs->regload_mask5.u32) + addr_offset));
    regload_mask5.bits.mask5 = mask5;
    vpss_reg_write((&(vpss_regs->regload_mask5.u32) + addr_offset), regload_mask5.u32);
    return;
}

hi_void vpss_sys_set_mask6(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask6)
{
    u_regload_mask6 regload_mask6;
    regload_mask6.u32 = vpss_reg_read((&(vpss_regs->regload_mask6.u32) + addr_offset));
    regload_mask6.bits.mask6 = mask6;
    vpss_reg_write((&(vpss_regs->regload_mask6.u32) + addr_offset), regload_mask6.u32);
    return;
}

hi_void vpss_sys_set_mask7(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask7)
{
    u_regload_mask7 regload_mask7;
    regload_mask7.u32 = vpss_reg_read((&(vpss_regs->regload_mask7.u32) + addr_offset));
    regload_mask7.bits.mask7 = mask7;
    vpss_reg_write((&(vpss_regs->regload_mask7.u32) + addr_offset), regload_mask7.u32);
    return;
}

hi_void vpss_sys_set_stt_raddr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 stt_addr_h, hi_u32 stt_addr_l)
{
    u_vpss_stt_r_addr_high vpss_stt_r_addr_high;
    u_vpss_stt_r_addr_low vpss_stt_r_addr_low;
    vpss_stt_r_addr_high.u32 = stt_addr_h;
    vpss_stt_r_addr_low.u32 = stt_addr_l;
    vpss_reg_write((&(vpss_regs->vpss_stt_r_addr_high.u32)), vpss_stt_r_addr_high.u32);
    vpss_reg_write((&(vpss_regs->vpss_stt_r_addr_low.u32)), vpss_stt_r_addr_low.u32);
    return;
}
hi_void vpss_sys_set_stt_waddr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 stt_addr_h, hi_u32 stt_addr_l)
{
    u_vpss_stt_w_addr_high vpss_stt_w_addr_high;
    u_vpss_stt_w_addr_low vpss_stt_w_addr_low;
    vpss_stt_w_addr_high.u32 = stt_addr_h;
    vpss_stt_w_addr_low.u32 = stt_addr_l;
    vpss_reg_write((&(vpss_regs->vpss_stt_w_addr_high.u32)), vpss_stt_w_addr_high.u32);
    vpss_reg_write((&(vpss_regs->vpss_stt_w_addr_low.u32)), vpss_stt_w_addr_low.u32);
    return;
}

hi_void vpss_sys_set_rupd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rupd_en)
{
    u_vpss_rupd_ctrl vpss_rupd_ctrl;
    vpss_rupd_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_rupd_ctrl.u32) + addr_offset));
    vpss_rupd_ctrl.bits.vpss_rupd_en = rupd_en;
    vpss_reg_write((&(vpss_regs->vpss_rupd_ctrl.u32) + addr_offset), vpss_rupd_ctrl.u32);
    return;
}

hi_void vpss_scan_set_scan_start_addr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_start_addr_h,
                                      hi_u32 scan_start_addr_l)
{
    u_scan_start_addr_h scan_start_addr_h_0;
    u_scan_start_addr_l scan_start_addr_l_0;
    scan_start_addr_h_0.u32 = 0;
    scan_start_addr_l_0.u32 = 0;
    scan_start_addr_h_0.bits.scan_start_addr_h = scan_start_addr_h;
    scan_start_addr_l_0.bits.scan_start_addr_l = scan_start_addr_l;
    vpss_reg_write((&(vpss_regs->scan_start_addr_h.u32)), scan_start_addr_h_0.u32);
    vpss_reg_write((&(vpss_regs->scan_start_addr_l.u32)), scan_start_addr_l_0.u32);
    return;
}

hi_void vpss_scan_set_scan_pnext_addr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_pnext_addr_h,
                                      hi_u32 scan_pnext_addr_l)
{
    u_scan_pnext_addr_h scan_pnext_addr_h_0;
    u_scan_pnext_addr_l scan_pnext_addr_l_0;
    scan_pnext_addr_h_0.u32 = 0;
    scan_pnext_addr_l_0.u32 = 0;
    scan_pnext_addr_h_0.bits.scan_pnext_addr_h = scan_pnext_addr_h;
    scan_pnext_addr_l_0.bits.scan_pnext_addr_l = scan_pnext_addr_l;
    vpss_reg_write((&(vpss_regs->scan_pnext_addr_h.u32) + addr_offset), scan_pnext_addr_h_0.u32);
    vpss_reg_write((&(vpss_regs->scan_pnext_addr_l.u32) + addr_offset), scan_pnext_addr_l_0.u32);
    return;
}

hi_void vpss_scan_set_me_cf_online_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_cf_online_en)
{
    u_scan_ctrl scan_ctrl;
    scan_ctrl.u32 = vpss_reg_read((&(vpss_regs->scan_ctrl.u32) + addr_offset));
    scan_ctrl.bits.me_cf_online_en = me_cf_online_en;
    vpss_reg_write((&(vpss_regs->scan_ctrl.u32) + addr_offset), scan_ctrl.u32);
    return;
}

hi_void vpss_scan_set_me_vc1_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_vc1_en)
{
    u_scan_ctrl scan_ctrl;
    scan_ctrl.u32 = vpss_reg_read((&(vpss_regs->scan_ctrl.u32) + addr_offset));
    scan_ctrl.bits.me_vc1_en = me_vc1_en;
    vpss_reg_write((&(vpss_regs->scan_ctrl.u32) + addr_offset), scan_ctrl.u32);
    return;
}

hi_void vpss_scan_set_me_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_en)
{
    u_scan_ctrl scan_ctrl;
    scan_ctrl.u32 = vpss_reg_read((&(vpss_regs->scan_ctrl.u32) + addr_offset));
    scan_ctrl.bits.me_en = me_en;
    vpss_reg_write((&(vpss_regs->scan_ctrl.u32) + addr_offset), scan_ctrl.u32);
    return;
}

hi_void vpss_scan_set_me_mv_upsmp_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_mv_upsmp_en)
{
    u_scan_ctrl scan_ctrl;
    scan_ctrl.u32 = vpss_reg_read((&(vpss_regs->scan_ctrl.u32) + addr_offset));
    scan_ctrl.bits.me_mv_upsmp_en = me_mv_upsmp_en;
    vpss_reg_write((&(vpss_regs->scan_ctrl.u32) + addr_offset), scan_ctrl.u32);
    return;
}

hi_void vpss_scan_set_me_mv_dnsmp_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_mv_dnsmp_en)
{
    u_scan_ctrl scan_ctrl;
    scan_ctrl.u32 = vpss_reg_read((&(vpss_regs->scan_ctrl.u32) + addr_offset));
    scan_ctrl.bits.me_mv_dnsmp_en = me_mv_dnsmp_en;
    vpss_reg_write((&(vpss_regs->scan_ctrl.u32) + addr_offset), scan_ctrl.u32);
    return;
}

hi_void vpss_scan_set_me_layer(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_layer)
{
    u_scan_ctrl scan_ctrl;
    scan_ctrl.u32 = vpss_reg_read((&(vpss_regs->scan_ctrl.u32) + addr_offset));
    scan_ctrl.bits.me_layer = me_layer;
    vpss_reg_write((&(vpss_regs->scan_ctrl.u32) + addr_offset), scan_ctrl.u32);
    return;
}

hi_void vpss_scan_set_me_scan(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_scan)
{
    u_scan_ctrl scan_ctrl;
    scan_ctrl.u32 = vpss_reg_read((&(vpss_regs->scan_ctrl.u32) + addr_offset));
    scan_ctrl.bits.me_scan = me_scan;
    vpss_reg_write((&(vpss_regs->scan_ctrl.u32) + addr_offset), scan_ctrl.u32);
    return;
}
hi_void vpss_scan_set_me_vid_default_cfg(vpss_reg_type *vpss_regs, hi_u32 addr_offset)
{
    u_vpss_me_cf_vid_read_ctrl vpss_me_cf_vid_read_ctrl;
    u_vpss_me_cf_vid_mac_ctrl vpss_me_cf_vid_mac_ctrl;
    u_vpss_me_cf_vid_mute_bk vpss_me_cf_vid_mute_bk;
    u_vpss_me_cf_vid_smmu_bypass vpss_me_cf_vid_smmu_bypass;
    u_vpss_me_cf_vid_tunl_ctrl vpss_me_cf_vid_tunl_ctrl;
    vpss_me_cf_vid_read_ctrl.u32 = 0x120000;     /* 0x120000 : reg default para */
    vpss_me_cf_vid_mac_ctrl.u32 = 0x80000000;    /* 0x80000000 : reg default para */
    vpss_me_cf_vid_mute_bk.u32 = 0x00080200;     /* 0x00080200 : reg default para */
    vpss_me_cf_vid_smmu_bypass.u32 = 0x0000000F; /* 0x0000000F : reg default para */
    vpss_me_cf_vid_tunl_ctrl.u32 = 0x00010000;   /* 0x00010000 : reg default para */
    vpss_reg_write((&(vpss_regs->vpss_me_cf_vid_read_ctrl.u32) + addr_offset), vpss_me_cf_vid_read_ctrl.u32);
    vpss_reg_write((&(vpss_regs->vpss_me_cf_vid_mac_ctrl.u32) + addr_offset), vpss_me_cf_vid_mac_ctrl.u32);
    vpss_reg_write((&(vpss_regs->vpss_me_cf_vid_mute_bk.u32) + addr_offset), vpss_me_cf_vid_mute_bk.u32);
    vpss_reg_write((&(vpss_regs->vpss_me_cf_vid_smmu_bypass.u32) + addr_offset), vpss_me_cf_vid_smmu_bypass.u32);
    vpss_reg_write((&(vpss_regs->vpss_me_cf_vid_tunl_ctrl.u32) + addr_offset), vpss_me_cf_vid_tunl_ctrl.u32);
    return;
}

hi_void vpss_mac_set_cf_cconvert(vpss_reg_type *vdp_reg, hi_u32 cf_c_convert)
{
    u_vpss_chn_cfg_ctrl vpss_chn_cfg_ctrl;
    vpss_chn_cfg_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_chn_cfg_ctrl.u32)));
    vpss_chn_cfg_ctrl.bits.cf_c_convert = cf_c_convert;
    vpss_reg_write((&(vdp_reg->vpss_chn_cfg_ctrl.u32)), vpss_chn_cfg_ctrl.u32);
    return;
}

hi_void vpss_mac_get_cf_cconvert(vpss_reg_type *vdp_reg, hi_u32 *cf_c_convert)
{
    u_vpss_chn_cfg_ctrl vpss_chn_cfg_ctrl;
    vpss_chn_cfg_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_chn_cfg_ctrl.u32)));
    *cf_c_convert = vpss_chn_cfg_ctrl.bits.cf_c_convert;
    return;
}

hi_void vpss_mac_set_img_pro_mode(vpss_reg_type *vdp_reg, hi_u32 img_pro_mode)
{
    u_vpss_chn_cfg_ctrl vpss_chn_cfg_ctrl;
    vpss_chn_cfg_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_chn_cfg_ctrl.u32)));
    vpss_chn_cfg_ctrl.bits.img_pro_mode = img_pro_mode;
    vpss_reg_write((&(vdp_reg->vpss_chn_cfg_ctrl.u32)), vpss_chn_cfg_ctrl.u32);
    return;
}

hi_void vpss_mac_set_prot(vpss_reg_type *vdp_reg, hi_u32 prot)
{
    u_vpss_chn_cfg_ctrl vpss_chn_cfg_ctrl;
    vpss_chn_cfg_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_chn_cfg_ctrl.u32)));
    vpss_chn_cfg_ctrl.bits.prot = prot;
    vpss_reg_write((&(vdp_reg->vpss_chn_cfg_ctrl.u32)), vpss_chn_cfg_ctrl.u32);
    return;
}

hi_void vpss_mac_get_prot(vpss_reg_type *vdp_reg, hi_u32 *prot)
{
    u_vpss_chn_cfg_ctrl vpss_chn_cfg_ctrl;

    vpss_chn_cfg_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_chn_cfg_ctrl.u32)));
    *prot = vpss_chn_cfg_ctrl.bits.prot;

    return;
}

hi_void vpss_mac_set_cf_rtunl_en(vpss_reg_type *vdp_reg, hi_u32 cf_rtunl_en)
{
    u_vpss_cf_rtunl_ctrl vpss_cf_rtunl_ctrl;
    vpss_cf_rtunl_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_cf_rtunl_ctrl.u32)));
    vpss_cf_rtunl_ctrl.bits.cf_rtunl_en = cf_rtunl_en;
    vpss_reg_write((&(vdp_reg->vpss_cf_rtunl_ctrl.u32)), vpss_cf_rtunl_ctrl.u32);
    return;
}

hi_void vpss_mac_set_cf_rtunl_addr_h(vpss_reg_type *vdp_reg, hi_u32 cf_rtunl_addr_h)
{
    u_vpss_cf_rtunl_addr_high vpss_cf_rtunl_addr_high;
    vpss_cf_rtunl_addr_high.u32 = vpss_reg_read((&(vdp_reg->vpss_cf_rtunl_addr_high.u32)));
    vpss_cf_rtunl_addr_high.bits.cf_rtunl_addr_h = cf_rtunl_addr_h;
    vpss_reg_write((&(vdp_reg->vpss_cf_rtunl_addr_high.u32)), vpss_cf_rtunl_addr_high.u32);
    return;
}

hi_void vpss_mac_set_cf_rtunl_addr_l(vpss_reg_type *vdp_reg, hi_u32 cf_rtunl_addr_l)
{
    u_vpss_cf_rtunl_addr_low vpss_cf_rtunl_addr_low;
    vpss_cf_rtunl_addr_low.u32 = vpss_reg_read((&(vdp_reg->vpss_cf_rtunl_addr_low.u32)));
    vpss_cf_rtunl_addr_low.bits.cf_rtunl_addr_l = cf_rtunl_addr_l;
    vpss_reg_write((&(vdp_reg->vpss_cf_rtunl_addr_low.u32)), vpss_cf_rtunl_addr_low.u32);
    return;
}

hi_void vpss_mac_set_me_stt_waddr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 me_stt_w_addr_h)
{
    u_vpss_me_stt_w_addr_high vpss_me_stt_w_addr_high;
    vpss_me_stt_w_addr_high.u32 = vpss_reg_read((&(vdp_reg->vpss_me_stt_w_addr_high.u32) + offset));
    vpss_me_stt_w_addr_high.bits.me_stt_w_addr_h = me_stt_w_addr_h;
    vpss_reg_write((&(vdp_reg->vpss_me_stt_w_addr_high.u32) + offset), vpss_me_stt_w_addr_high.u32);
    return;
}

hi_void vpss_mac_set_me_stt_waddr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 me_stt_w_addr_l)
{
    u_vpss_me_stt_w_addr_low vpss_me_stt_w_addr_low;
    vpss_me_stt_w_addr_low.u32 = vpss_reg_read((&(vdp_reg->vpss_me_stt_w_addr_low.u32) + offset));
    vpss_me_stt_w_addr_low.bits.me_stt_w_addr_l = me_stt_w_addr_l;
    vpss_reg_write((&(vdp_reg->vpss_me_stt_w_addr_low.u32) + offset), vpss_me_stt_w_addr_low.u32);
    return;
}

/* write chn */
hi_void vpss_wchn_set_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 en)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.en = en;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_tunl_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 tunl_en)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.tunl_en = tunl_en;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_dbypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 bypass)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.d_bypass = bypass;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_hbypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 hypass)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.h_bypass = hypass;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}
hi_void vpss_wchn_set_lm_rmode(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 lm_rmode)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.lm_rmode = lm_rmode;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_dither_mode(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 dither_mode)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.dither_mode = dither_mode;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_dither_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 dither_en)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.dither_en = dither_en;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_flip(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 flip)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.flip = flip;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_mirror(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mirror)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.mirror = mirror;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_uv_invert(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 uv_invert)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.uv_invert = uv_invert;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_cmp_mode(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 cmp_mode)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.cmp_mode = cmp_mode;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_bitw(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 bitw)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.bitw = bitw;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_type(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 type)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.type = type;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_set_ver_offset(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 ver_offset)
{
    u_vpss_out0_offset vpss_out0_offset;
    vpss_out0_offset.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_offset.u32) + offset));
    vpss_out0_offset.bits.out0_ver_offset = ver_offset;
    vpss_reg_write((&(vpss_reg->vpss_out0_offset.u32) + offset), vpss_out0_offset.u32);
    return;
}

hi_void vpss_wchn_set_hor_offset(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 hor_offset)
{
    u_vpss_out0_offset vpss_out0_offset;
    vpss_out0_offset.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_offset.u32) + offset));
    vpss_out0_offset.bits.out0_hor_offset = hor_offset;
    vpss_reg_write((&(vpss_reg->vpss_out0_offset.u32) + offset), vpss_out0_offset.u32);
    return;
}

hi_void vpss_wchn_set_yaddr_l(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 y_addr_l)
{
    u_vpss_out0_y_addr_low vpss_out0_y_addr_low;
    vpss_out0_y_addr_low.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_y_addr_low.u32) + offset));
    vpss_out0_y_addr_low.bits.out0y_addr_l = y_addr_l;
    vpss_reg_write((&(vpss_reg->vpss_out0_y_addr_low.u32) + offset), vpss_out0_y_addr_low.u32);
    return;
}

hi_void vpss_wchn_set_yaddr_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 y_addr_h)
{
    u_vpss_out0_y_addr_high vpss_out0_y_addr_high;
    vpss_out0_y_addr_high.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_y_addr_high.u32) + offset));
    vpss_out0_y_addr_high.bits.out0y_addr_h = y_addr_h;
    vpss_reg_write((&(vpss_reg->vpss_out0_y_addr_high.u32) + offset), vpss_out0_y_addr_high.u32);
    return;
}

hi_void vpss_wchn_set_caddr_l(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_l)
{
    u_vpss_out0_c_addr_low vpss_out0_c_addr_low;
    vpss_out0_c_addr_low.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_c_addr_low.u32) + offset));
    vpss_out0_c_addr_low.bits.out0c_addr_l = addr_l;
    vpss_reg_write((&(vpss_reg->vpss_out0_c_addr_low.u32) + offset), vpss_out0_c_addr_low.u32);
    return;
}

hi_void vpss_wchn_set_caddr_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_h)
{
    u_vpss_out0_c_addr_high vpss_out0_c_addr_high;
    vpss_out0_c_addr_high.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_c_addr_high.u32) + offset));
    vpss_out0_c_addr_high.bits.out0c_addr_h = addr_h;
    vpss_reg_write((&(vpss_reg->vpss_out0_c_addr_high.u32) + offset), vpss_out0_c_addr_high.u32);
    return;
}

hi_void vpss_wchn_set_cstride(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 stride)
{
    u_vpss_out0_stride vpss_out0_stride;
    vpss_out0_stride.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_stride.u32) + offset));
    vpss_out0_stride.bits.out0c_stride = stride;
    vpss_reg_write((&(vpss_reg->vpss_out0_stride.u32) + offset), vpss_out0_stride.u32);
    return;
}

hi_void vpss_wchn_set_ystride(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 stride_y)
{
    u_vpss_out0_stride vpss_out0_stride;
    vpss_out0_stride.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_stride.u32) + offset));
    vpss_out0_stride.bits.out0y_stride = stride_y;
    vpss_reg_write((&(vpss_reg->vpss_out0_stride.u32) + offset), vpss_out0_stride.u32);
    return;
}

hi_void vpss_wchn_env_set_format(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 format)
{
    u_vpss_base_wr_img_ctrl vpss_base_wr_img_ctrl;
    vpss_base_wr_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_ctrl.u32) + offset));
    vpss_base_wr_img_ctrl.bits.format = format;
    vpss_reg_write((&(vpss_reg->vpss_out0_ctrl.u32) + offset), vpss_base_wr_img_ctrl.u32);
    return;
}

hi_void vpss_wchn_env_set_height(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 height)
{
    u_vpss_out0_size vpss_out0_size;
    vpss_out0_size.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_size.u32) + offset));
    vpss_out0_size.bits.out0_height = height;
    vpss_reg_write((&(vpss_reg->vpss_out0_size.u32) + offset), vpss_out0_size.u32);
    return;
}

hi_void vpss_wchn_env_set_width(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 width)
{
    u_vpss_out0_size vpss_out0_size;
    vpss_out0_size.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_size.u32) + offset));
    vpss_out0_size.bits.out0_width = width;
    vpss_reg_write((&(vpss_reg->vpss_out0_size.u32) + offset), vpss_out0_size.u32);
    return;
}

hi_void vpss_wchn_env_set_finfo_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 finfo_h)
{
    u_vpss_out0_finfo_hign vpss_out0_finfo_hign;
    vpss_out0_finfo_hign.u32 = vpss_reg_read((&(vpss_reg->vpss_out0_finfo_hign.u32) + offset));
    vpss_out0_finfo_hign.bits.out0_finfo_h = finfo_h;
    vpss_reg_write((&(vpss_reg->vpss_out0_finfo_hign.u32) + offset), vpss_out0_finfo_hign.u32);
    return;
}

/* vpss mac wchn hal solo */
hi_void vpss_wchn_solo_set_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 en)
{
    u_vpss_base_wr_solo_ctrl vpss_base_wr_solo_ctrl;
    vpss_base_wr_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset));
    vpss_base_wr_solo_ctrl.bits.en = en;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset), vpss_base_wr_solo_ctrl.u32);
    return;
}

hi_void vpss_wchn_solo_set_tunl_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 tunl_en)
{
    u_vpss_base_wr_solo_ctrl vpss_base_wr_solo_ctrl;
    vpss_base_wr_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset));
    vpss_base_wr_solo_ctrl.bits.tunl_en = tunl_en;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset), vpss_base_wr_solo_ctrl.u32);
    return;
}

hi_void vpss_wchn_solo_set_dbypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 bypass)
{
    u_vpss_base_wr_solo_ctrl vpss_base_wr_solo_ctrl;
    vpss_base_wr_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset));
    vpss_base_wr_solo_ctrl.bits.d_bypass = bypass;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset), vpss_base_wr_solo_ctrl.u32);
    return;
}

hi_void vpss_wchn_solo_set_flip(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 flip)
{
    u_vpss_base_wr_solo_ctrl vpss_base_wr_solo_ctrl;
    vpss_base_wr_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset));
    vpss_base_wr_solo_ctrl.bits.flip = flip;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset), vpss_base_wr_solo_ctrl.u32);
    return;
}

hi_void vpss_wchn_solo_set_mirror(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mirror)
{
    u_vpss_base_wr_solo_ctrl vpss_base_wr_solo_ctrl;
    vpss_base_wr_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset));
    vpss_base_wr_solo_ctrl.bits.mirror = mirror;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_ctrl.u32) + offset), vpss_base_wr_solo_ctrl.u32);
    return;
}

hi_void vpss_wchn_solo_set_addr_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_h)
{
    u_vpss_nr_wmad_addr_high vpss_nr_wmad_addr_high;
    vpss_nr_wmad_addr_high.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_addr_high.u32) + offset));
    vpss_nr_wmad_addr_high.bits.nr_wmad_addr_h = addr_h;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_addr_high.u32) + offset), vpss_nr_wmad_addr_high.u32);
    return;
}

hi_void vpss_wchn_solo_set_addr_l(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_l)
{
    u_vpss_nr_wmad_addr_low vpss_nr_wmad_addr_low;
    vpss_nr_wmad_addr_low.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_addr_low.u32) + offset));
    vpss_nr_wmad_addr_low.bits.nr_wmad_addr_l = addr_l;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_addr_low.u32) + offset), vpss_nr_wmad_addr_low.u32);
    return;
}

hi_void vpss_wchn_solo_set_stride(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 stride)
{
    u_vpss_nr_wmad_stride vpss_nr_wmad_stride;
    vpss_nr_wmad_stride.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_stride.u32) + offset));
    vpss_nr_wmad_stride.bits.nr_wmad_stride = stride;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_stride.u32) + offset), vpss_nr_wmad_stride.u32);
    return;
}

hi_void vpss_wchn_solo_env_set_height(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 height)
{
    u_vpss_nr_wmad_size vpss_nr_wmad_size;
    vpss_nr_wmad_size.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_size.u32) + offset));
    vpss_nr_wmad_size.bits.nr_wmad_height = height;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_size.u32) + offset), vpss_nr_wmad_size.u32);
    return;
}

hi_void vpss_wchn_solo_env_set_width(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 width)
{
    u_vpss_nr_wmad_size vpss_nr_wmad_size;
    vpss_nr_wmad_size.u32 = vpss_reg_read((&(vpss_reg->vpss_nr_wmad_size.u32) + offset));
    vpss_nr_wmad_size.bits.nr_wmad_width = width;
    vpss_reg_write((&(vpss_reg->vpss_nr_wmad_size.u32) + offset), vpss_nr_wmad_size.u32);
    return;
}

/* RCHN */
hi_void vpss_rchn_set_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 en)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.en = en;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_tunl_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 tunl_en)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.tunl_en = tunl_en;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_mute_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mute_en)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.mute_en = mute_en;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_dbypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 bypass)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.d_bypass = bypass;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_hbypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 h_bypass)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.h_bypass = h_bypass;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set2b_bypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 bypass)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.b_bypass = bypass;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_lm_rmode(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 lm_rmode)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.lm_rmode = lm_rmode;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_flip(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 flip)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.flip = flip;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_mirror(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mirror)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.mirror = mirror;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_uv_invert(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 uv_invert)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.uv_invert = uv_invert;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_dcmp_mode(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 dcmp_mode)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.dcmp_mode = dcmp_mode;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_bitw(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 bitw)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.bitw = bitw;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_order(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 order)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.order = order;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_type(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 type)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.type = type;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_format(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 format)
{
    u_vpss_base_rd_img_ctrl vpss_base_rd_img_ctrl;
    vpss_base_rd_img_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_ctrl.u32) + offset));
    vpss_base_rd_img_ctrl.bits.format = format;
    vpss_reg_write((&(vpss_reg->vpss_cf_ctrl.u32) + offset), vpss_base_rd_img_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_height(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 height)
{
    u_vpss_cf_size vpss_cf_size;
    vpss_cf_size.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_size.u32) + offset));
    vpss_cf_size.bits.cf_height = height;
    vpss_reg_write((&(vpss_reg->vpss_cf_size.u32) + offset), vpss_cf_size.u32);
    return;
}

hi_void vpss_rchn_set_width(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 width)
{
    u_vpss_cf_size vpss_cf_size;
    vpss_cf_size.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_size.u32) + offset));
    vpss_cf_size.bits.cf_width = width;
    vpss_reg_write((&(vpss_reg->vpss_cf_size.u32) + offset), vpss_cf_size.u32);
    return;
}

hi_void vpss_rchn_set_ver_offset(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 ver_offset)
{
    u_vpss_cf_offset vpss_cf_offset;
    vpss_cf_offset.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_offset.u32) + offset));
    vpss_cf_offset.bits.cf_ver_offset = ver_offset;
    vpss_reg_write((&(vpss_reg->vpss_cf_offset.u32) + offset), vpss_cf_offset.u32);
    return;
}

hi_void vpss_rchn_set_hor_offset(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 hor_offset)
{
    u_vpss_cf_offset vpss_cf_offset;
    vpss_cf_offset.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_offset.u32) + offset));
    vpss_cf_offset.bits.cf_hor_offset = hor_offset;
    vpss_reg_write((&(vpss_reg->vpss_cf_offset.u32) + offset), vpss_cf_offset.u32);
    return;
}

hi_void vpss_rchn_set_yaddr_l(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 y_addr_l)
{
    u_vpss_cfy_addr_low vpss_cfy_addr_low;
    vpss_cfy_addr_low.u32 = vpss_reg_read((&(vpss_reg->vpss_cfy_addr_low.u32) + offset));
    vpss_cfy_addr_low.bits.cfy_addr_l = y_addr_l;
    vpss_reg_write((&(vpss_reg->vpss_cfy_addr_low.u32) + offset), vpss_cfy_addr_low.u32);
    return;
}

hi_void vpss_rchn_set_yaddr_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 y_addr_h)
{
    u_vpss_cfy_addr_high vpss_cfy_addr_high;
    vpss_cfy_addr_high.u32 = vpss_reg_read((&(vpss_reg->vpss_cfy_addr_high.u32) + offset));
    vpss_cfy_addr_high.bits.cfy_addr_h = y_addr_h;
    vpss_reg_write((&(vpss_reg->vpss_cfy_addr_high.u32) + offset), vpss_cfy_addr_high.u32);
    return;
}

hi_void vpss_rchn_set_caddr_l(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_l)
{
    u_vpss_cfc_addr_low vpss_cfc_addr_low;
    vpss_cfc_addr_low.u32 = vpss_reg_read((&(vpss_reg->vpss_cfc_addr_low.u32) + offset));
    vpss_cfc_addr_low.bits.cfc_addr_l = addr_l;
    vpss_reg_write((&(vpss_reg->vpss_cfc_addr_low.u32) + offset), vpss_cfc_addr_low.u32);
    return;
}

hi_void vpss_rchn_set_cr_addr_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 cr_addr_h)
{
    u_vpss_cfcr_addr_high vpss_cfcr_addr_high;
    vpss_cfcr_addr_high.u32 = vpss_reg_read((&(vpss_reg->vpss_cfcr_addr_high.u32) + offset));
    vpss_cfcr_addr_high.bits.cfcr_addr_h = cr_addr_h;
    vpss_reg_write((&(vpss_reg->vpss_cfcr_addr_high.u32) + offset), vpss_cfcr_addr_high.u32);
    return;
}

hi_void vpss_rchn_set_cr_addr_l(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 cr_addr_l)
{
    u_vpss_cfcr_addr_low vpss_cfcr_addr_low;
    vpss_cfcr_addr_low.u32 = vpss_reg_read((&(vpss_reg->vpss_cfcr_addr_low.u32) + offset));
    vpss_cfcr_addr_low.bits.cfcr_addr_l = cr_addr_l;
    vpss_reg_write((&(vpss_reg->vpss_cfcr_addr_low.u32) + offset), vpss_cfcr_addr_low.u32);
    return;
}

hi_void vpss_rchn_set_caddr_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_h)
{
    u_vpss_cfc_addr_high vpss_cfc_addr_high;
    vpss_cfc_addr_high.u32 = vpss_reg_read((&(vpss_reg->vpss_cfc_addr_high.u32) + offset));
    vpss_cfc_addr_high.bits.cfc_addr_h = addr_h;
    vpss_reg_write((&(vpss_reg->vpss_cfc_addr_high.u32) + offset), vpss_cfc_addr_high.u32);
    return;
}

hi_void vpss_rchn_set_cstride(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 stride)
{
    u_vpss_cf_stride vpss_cf_stride;
    vpss_cf_stride.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_stride.u32) + offset));
    vpss_cf_stride.bits.cfc_stride = stride;
    vpss_reg_write((&(vpss_reg->vpss_cf_stride.u32) + offset), vpss_cf_stride.u32);
    return;
}

hi_void vpss_rchn_set_ystride(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 stride_y)
{
    u_vpss_cf_stride vpss_cf_stride;
    vpss_cf_stride.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_stride.u32) + offset));
    vpss_cf_stride.bits.cfy_stride = stride_y;
    vpss_reg_write((&(vpss_reg->vpss_cf_stride.u32) + offset), vpss_cf_stride.u32);
    return;
}

hi_void vpss_set_hvhdaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdaddr_h)
{
    u_vpss_cf_vid_head_addr_h vpss_cf_vid_head_addr_h;
    vpss_cf_vid_head_addr_h.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_head_addr_h.u32) + addr_offset));
    vpss_cf_vid_head_addr_h.bits.h_vhdaddr_h = h_vhdaddr_h;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_head_addr_h.u32) + addr_offset), vpss_cf_vid_head_addr_h.u32);
    return;
}

hi_void vpss_set_hvhdaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdaddr_l)
{
    u_vpss_cf_vid_head_addr_l vpss_cf_vid_head_addr_l;
    vpss_cf_vid_head_addr_l.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_head_addr_l.u32) + addr_offset));
    vpss_cf_vid_head_addr_l.bits.h_vhdaddr_l = h_vhdaddr_l;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_head_addr_l.u32) + addr_offset), vpss_cf_vid_head_addr_l.u32);
    return;
}

hi_void vpss_set_hvhdcaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdcaddr_h)
{
    u_vpss_cf_vid_head_caddr_h vpss_cf_vid_head_caddr_h;
    vpss_cf_vid_head_caddr_h.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_head_caddr_h.u32) + addr_offset));
    vpss_cf_vid_head_caddr_h.bits.h_vhdcaddr_h = h_vhdcaddr_h;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_head_caddr_h.u32) + addr_offset), vpss_cf_vid_head_caddr_h.u32);
    return;
}

hi_void vpss_set_hvhdcaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdcaddr_l)
{
    u_vpss_cf_vid_head_caddr_l vpss_cf_vid_head_caddr_l;
    vpss_cf_vid_head_caddr_l.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_head_caddr_l.u32) + addr_offset));
    vpss_cf_vid_head_caddr_l.bits.h_vhdcaddr_l = h_vhdcaddr_l;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_head_caddr_l.u32) + addr_offset), vpss_cf_vid_head_caddr_l.u32);
    return;
}

hi_void vpss_set_bvhdaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdaddr_h)
{
    u_vpss_cf_vid_2_bit_addr_h vpss_cf_vid_2_bit_addr_h;
    vpss_cf_vid_2_bit_addr_h.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_2_bit_addr_h.u32) + addr_offset));
    vpss_cf_vid_2_bit_addr_h.bits.vhdaddr_h = vhdaddr_h;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_2_bit_addr_h.u32) + addr_offset), vpss_cf_vid_2_bit_addr_h.u32);
    return;
}

hi_void vpss_set_bvhdaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdaddr_l)
{
    u_vpss_cf_vid_2_bit_addr_l vpss_cf_vid_2_bit_addr_l;
    vpss_cf_vid_2_bit_addr_l.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_2_bit_addr_l.u32) + addr_offset));
    vpss_cf_vid_2_bit_addr_l.bits.vhdaddr_l = vhdaddr_l;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_2_bit_addr_l.u32) + addr_offset), vpss_cf_vid_2_bit_addr_l.u32);
    return;
}

hi_void vpss_set_bvhdcaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdcaddr_h)
{
    u_vpss_cf_vid_2_bit_caddr_h vpss_cf_vid_2_bit_caddr_h;
    vpss_cf_vid_2_bit_caddr_h.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_2_bit_caddr_h.u32) + addr_offset));
    vpss_cf_vid_2_bit_caddr_h.bits.vhdcaddr_h = vhdcaddr_h;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_2_bit_caddr_h.u32) + addr_offset), vpss_cf_vid_2_bit_caddr_h.u32);
    return;
}

hi_void vpss_set_bvhdcaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdcaddr_l)
{
    u_vpss_cf_vid_2_bit_caddr_l vpss_cf_vid_2_bit_caddr_l;
    vpss_cf_vid_2_bit_caddr_l.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_2_bit_caddr_l.u32) + addr_offset));
    vpss_cf_vid_2_bit_caddr_l.bits.vhdcaddr_l = vhdcaddr_l;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_2_bit_caddr_l.u32) + addr_offset), vpss_cf_vid_2_bit_caddr_l.u32);
    return;
}

hi_void vpss_set_chm_tile_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 chm_tile_stride)
{
    u_vpss_cf_vid_2_bit_stride vpss_cf_vid_2_bit_stride;
    vpss_cf_vid_2_bit_stride.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_2_bit_stride.u32) + addr_offset));
    vpss_cf_vid_2_bit_stride.bits.chm_tile_stride = chm_tile_stride;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_2_bit_stride.u32) + addr_offset), vpss_cf_vid_2_bit_stride.u32);
    return;
}

hi_void vpss_set_lm_tile_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lm_tile_stride)
{
    u_vpss_cf_vid_2_bit_stride vpss_cf_vid_2_bit_stride;
    vpss_cf_vid_2_bit_stride.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_2_bit_stride.u32) + addr_offset));
    vpss_cf_vid_2_bit_stride.bits.lm_tile_stride = lm_tile_stride;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_2_bit_stride.u32) + addr_offset), vpss_cf_vid_2_bit_stride.u32);
    return;
}

hi_void vpss_set_chm_head_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 chm_head_stride)
{
    u_vpss_cf_vid_head_stride vpss_cf_vid_head_stride;
    vpss_cf_vid_head_stride.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_head_stride.u32) + addr_offset));
    vpss_cf_vid_head_stride.bits.chm_head_stride = chm_head_stride;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_head_stride.u32) + addr_offset), vpss_cf_vid_head_stride.u32);
    return;
}

hi_void vpss_set_lm_head_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lm_head_stride)
{
    u_vpss_cf_vid_head_stride vpss_cf_vid_head_stride;
    vpss_cf_vid_head_stride.u32 = vpss_reg_read((&(vpss_regs->vpss_cf_vid_head_stride.u32) + addr_offset));
    vpss_cf_vid_head_stride.bits.lm_head_stride = lm_head_stride;
    vpss_reg_write((&(vpss_regs->vpss_cf_vid_head_stride.u32) + addr_offset), vpss_cf_vid_head_stride.u32);
    return;
}

/* vpss mac rchn hal solo */
hi_void vpss_rchn_solo_set_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 en)
{
    u_vpss_base_rd_solo_ctrl vpss_base_rd_solo_ctrl;
    vpss_base_rd_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset));
    vpss_base_rd_solo_ctrl.bits.en = en;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset), vpss_base_rd_solo_ctrl.u32);
    return;
}

hi_void vpss_rchn_solo_set_tunle_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 tunl_en)
{
    u_vpss_base_rd_solo_ctrl vpss_base_rd_solo_ctrl;
    vpss_base_rd_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset));
    vpss_base_rd_solo_ctrl.bits.tunl_en = tunl_en;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset), vpss_base_rd_solo_ctrl.u32);
    return;
}

hi_void vpss_rchn_solo_set_mute_en(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mute_en)
{
    u_vpss_base_rd_solo_ctrl vpss_base_rd_solo_ctrl;
    vpss_base_rd_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset));
    vpss_base_rd_solo_ctrl.bits.mute_en = mute_en;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset), vpss_base_rd_solo_ctrl.u32);
    return;
}

hi_void vpss_rchn_solo_set_dbypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 bypass)
{
    u_vpss_base_rd_solo_ctrl vpss_base_rd_solo_ctrl;
    vpss_base_rd_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset));
    vpss_base_rd_solo_ctrl.bits.d_bypass = bypass;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset), vpss_base_rd_solo_ctrl.u32);
    return;
}

hi_void vpss_rchn_solo_set_flip(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 flip)
{
    u_vpss_base_rd_solo_ctrl vpss_base_rd_solo_ctrl;
    vpss_base_rd_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset));
    vpss_base_rd_solo_ctrl.bits.flip = flip;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset), vpss_base_rd_solo_ctrl.u32);
    return;
}

hi_void vpss_rchn_solo_set_mirror(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mirror)
{
    u_vpss_base_rd_solo_ctrl vpss_base_rd_solo_ctrl;
    vpss_base_rd_solo_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset));
    vpss_base_rd_solo_ctrl.bits.mirror = mirror;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_ctrl.u32) + offset), vpss_base_rd_solo_ctrl.u32);
    return;
}

hi_void vpss_rchn_solo_set_height(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 height)
{
    u_vpss_di_rsady_size vpss_di_rsady_size;
    vpss_di_rsady_size.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_size.u32) + offset));
    vpss_di_rsady_size.bits.di_rsady_height = height;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_size.u32) + offset), vpss_di_rsady_size.u32);
    return;
}

hi_void vpss_rchn_solo_set_width(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 width)
{
    u_vpss_di_rsady_size vpss_di_rsady_size;
    vpss_di_rsady_size.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_size.u32) + offset));
    vpss_di_rsady_size.bits.di_rsady_width = width;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_size.u32) + offset), vpss_di_rsady_size.u32);
    return;
}

hi_void vpss_rchn_solo_set_addr_h(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_h)
{
    u_vpss_di_rsady_addr_high vpss_di_rsady_addr_high;
    vpss_di_rsady_addr_high.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_addr_high.u32) + offset));
    vpss_di_rsady_addr_high.bits.di_rsady_addr_h = addr_h;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_addr_high.u32) + offset), vpss_di_rsady_addr_high.u32);
    return;
}

hi_void vpss_rchn_solo_set_addr_l(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 addr_l)
{
    u_vpss_di_rsady_addr_low vpss_di_rsady_addr_low;
    vpss_di_rsady_addr_low.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_addr_low.u32) + offset));
    vpss_di_rsady_addr_low.bits.di_rsady_addr_l = addr_l;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_addr_low.u32) + offset), vpss_di_rsady_addr_low.u32);
    return;
}

hi_void vpss_rchn_solo_set_stride(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 stride)
{
    u_vpss_di_rsady_stride vpss_di_rsady_stride;
    vpss_di_rsady_stride.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_stride.u32) + offset));
    vpss_di_rsady_stride.bits.di_rsady_stride = stride;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_stride.u32) + offset), vpss_di_rsady_stride.u32);
    return;
}

hi_void vpss_rchn_solo_set_mute_val0(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mute_val0)
{
    u_vpss_di_rsady_mute_val0 vpss_di_rsady_mute_val0;
    vpss_di_rsady_mute_val0.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_mute_val0.u32) + offset));
    vpss_di_rsady_mute_val0.bits.di_rsady_mute_val0 = mute_val0;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_mute_val0.u32) + offset), vpss_di_rsady_mute_val0.u32);
    return;
}

hi_void vpss_rchn_solo_set_mute_val1(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mute_val1)
{
    u_vpss_di_rsady_mute_val1 vpss_di_rsady_mute_val1;
    vpss_di_rsady_mute_val1.u32 = vpss_reg_read((&(vpss_reg->vpss_di_rsady_mute_val1.u32) + offset));
    vpss_di_rsady_mute_val1.bits.di_rsady_mute_val1 = mute_val1;
    vpss_reg_write((&(vpss_reg->vpss_di_rsady_mute_val1.u32) + offset), vpss_di_rsady_mute_val1.u32);
    return;
}

hi_void vpss_rchn_set_cmute_val(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 mute_val)
{
    u_vpss_cf_mute_val vpss_cf_mute_val;
    vpss_cf_mute_val.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_mute_val.u32) + offset));
    vpss_cf_mute_val.bits.cfc_mute_val = mute_val;
    vpss_reg_write((&(vpss_reg->vpss_cf_mute_val.u32) + offset), vpss_cf_mute_val.u32);
    return;
}

hi_void vpss_rchn_set_ymute_val(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 y_mute_val)
{
    u_vpss_cf_mute_val vpss_cf_mute_val;
    vpss_cf_mute_val.u32 = vpss_reg_read((&(vpss_reg->vpss_cf_mute_val.u32) + offset));
    vpss_cf_mute_val.bits.cfy_mute_val = y_mute_val;
    vpss_reg_write((&(vpss_reg->vpss_cf_mute_val.u32) + offset), vpss_cf_mute_val.u32);
    return;
}
/* VZME */
hi_void vpss_vzme_set_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 ck_gt_en)
{
    u_vpss_wr1_hvds_zme_vinfo vpss_wr1_hvds_zme_vinfo;
    vpss_wr1_hvds_zme_vinfo.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vinfo.u32) + offset_addr));
    vpss_wr1_hvds_zme_vinfo.bits.vpss_wr1_hvds_ck_gt_en = ck_gt_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vinfo.u32) + offset_addr), vpss_wr1_hvds_zme_vinfo.u32);
    return;
}

hi_void vpss_vzme_set_out_fmt(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 out_fmt)
{
    u_vpss_wr1_hvds_zme_vinfo vpss_wr1_hvds_zme_vinfo;
    vpss_wr1_hvds_zme_vinfo.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vinfo.u32) + offset_addr));
    vpss_wr1_hvds_zme_vinfo.bits.vpss_wr1_hvds_out_fmt = out_fmt;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vinfo.u32) + offset_addr), vpss_wr1_hvds_zme_vinfo.u32);
    return;
}

hi_void vpss_vzme_set_out_height(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 out_height)
{
    u_vpss_wr1_hvds_zme_vinfo vpss_wr1_hvds_zme_vinfo;
    vpss_wr1_hvds_zme_vinfo.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vinfo.u32) + offset_addr));
    vpss_wr1_hvds_zme_vinfo.bits.vpss_wr1_hvds_out_height = out_height;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vinfo.u32) + offset_addr), vpss_wr1_hvds_zme_vinfo.u32);
    return;
}

hi_void vpss_vzme_set_lvfir_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 lvfir_en)
{
    u_vpss_wr1_hvds_zme_vsp vpss_wr1_hvds_zme_vsp;
    vpss_wr1_hvds_zme_vsp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr));
    vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_lvfir_en = lvfir_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr), vpss_wr1_hvds_zme_vsp.u32);
    return;
}

hi_void vpss_vzme_set_cvfir_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 cvfir_en)
{
    u_vpss_wr1_hvds_zme_vsp vpss_wr1_hvds_zme_vsp;
    vpss_wr1_hvds_zme_vsp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr));
    vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_cvfir_en = cvfir_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr), vpss_wr1_hvds_zme_vsp.u32);
    return;
}

hi_void vpss_vzme_set_lvmid_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 lvmid_en)
{
    u_vpss_wr1_hvds_zme_vsp vpss_wr1_hvds_zme_vsp;
    vpss_wr1_hvds_zme_vsp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr));
    vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_lvmid_en = lvmid_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr), vpss_wr1_hvds_zme_vsp.u32);
    return;
}

hi_void vpss_vzme_set_cvmid_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 cvmid_en)
{
    u_vpss_wr1_hvds_zme_vsp vpss_wr1_hvds_zme_vsp;
    vpss_wr1_hvds_zme_vsp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr));
    vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_cvmid_en = cvmid_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr), vpss_wr1_hvds_zme_vsp.u32);
    return;
}

hi_void vpss_vzme_set_lvfir_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 lvfir_mode)
{
    u_vpss_wr1_hvds_zme_vsp vpss_wr1_hvds_zme_vsp;
    vpss_wr1_hvds_zme_vsp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr));
    vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_lvfir_mode = lvfir_mode;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr), vpss_wr1_hvds_zme_vsp.u32);
    return;
}

hi_void vpss_vzme_set_cvfir_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 cvfir_mode)
{
    u_vpss_wr1_hvds_zme_vsp vpss_wr1_hvds_zme_vsp;
    vpss_wr1_hvds_zme_vsp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr));
    vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_cvfir_mode = cvfir_mode;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr), vpss_wr1_hvds_zme_vsp.u32);
    return;
}

hi_void vpss_vzme_set_vratio(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vratio)
{
    u_vpss_wr1_hvds_zme_vsp vpss_wr1_hvds_zme_vsp;
    vpss_wr1_hvds_zme_vsp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr));
    vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_vratio = vratio;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vsp.u32) + offset_addr), vpss_wr1_hvds_zme_vsp.u32);
    return;
}

hi_void vpss_vzme_set_vluma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vluma_offset)
{
    u_vpss_wr1_hvds_zme_voffset vpss_wr1_hvds_zme_voffset;
    vpss_wr1_hvds_zme_voffset.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_voffset.u32) + offset_addr));
    vpss_wr1_hvds_zme_voffset.bits.vpss_wr1_hvds_vluma_offset = vluma_offset;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_voffset.u32) + offset_addr), vpss_wr1_hvds_zme_voffset.u32);
    return;
}

hi_void vpss_vzme_set_vchroma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vchroma_offset)
{
    u_vpss_wr1_hvds_zme_voffset vpss_wr1_hvds_zme_voffset;
    vpss_wr1_hvds_zme_voffset.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_voffset.u32) + offset_addr));
    vpss_wr1_hvds_zme_voffset.bits.vpss_wr1_hvds_vchroma_offset = vchroma_offset;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_voffset.u32) + offset_addr), vpss_wr1_hvds_zme_voffset.u32);
    return;
}

hi_void vpss_vzme_set_vbluma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vbluma_offset)
{
    u_vpss_wr1_hvds_zme_vboffset vpss_wr1_hvds_zme_vboffset;
    vpss_wr1_hvds_zme_vboffset.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vboffset.u32) + offset_addr));
    vpss_wr1_hvds_zme_vboffset.bits.vpss_wr1_hvds_vbluma_offset = vbluma_offset;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vboffset.u32) + offset_addr), vpss_wr1_hvds_zme_vboffset.u32);
    return;
}

hi_void vpss_vzme_set_vbchroma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vbchroma_offset)
{
    u_vpss_wr1_hvds_zme_vboffset vpss_wr1_hvds_zme_vboffset;
    vpss_wr1_hvds_zme_vboffset.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vboffset.u32) + offset_addr));
    vpss_wr1_hvds_zme_vboffset.bits.vpss_wr1_hvds_vbchroma_offset = vbchroma_offset;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vboffset.u32) + offset_addr), vpss_wr1_hvds_zme_vboffset.u32);
    return;
}

hi_void vpss_vzme_set_vl_shootctrl_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_shootctrl_en)
{
    u_vpss_wr1_hvds_zme_vl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vl_shootctrl_en = vl_shootctrl_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vl_shootctrl_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_shootctrl_mode)
{
    u_vpss_wr1_hvds_zme_vl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vl_shootctrl_mode = vl_shootctrl_mode;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vl_flatdect_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_flatdect_mode)
{
    u_vpss_wr1_hvds_zme_vl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vl_flatdect_mode = vl_flatdect_mode;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vl_coringadj_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_coringadj_en)
{
    u_vpss_wr1_hvds_zme_vl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vl_coringadj_en = vl_coringadj_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vl_gain(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_gain)
{
    u_vpss_wr1_hvds_zme_vl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vl_gain = vl_gain;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vl_coring(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_coring)
{
    u_vpss_wr1_hvds_zme_vl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vl_coring = vl_coring;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vc_shootctrl_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_shootctrl_en)
{
    u_vpss_wr1_hvds_zme_vc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vc_shootctrl_en = vc_shootctrl_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vc_shootctrl_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_shootctrl_mode)
{
    u_vpss_wr1_hvds_zme_vc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vc_shootctrl_mode = vc_shootctrl_mode;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vc_flatdect_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_flatdect_mode)
{
    u_vpss_wr1_hvds_zme_vc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vc_flatdect_mode = vc_flatdect_mode;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vc_coringadj_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_coringadj_en)
{
    u_vpss_wr1_hvds_zme_vc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vc_coringadj_en = vc_coringadj_en;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vc_gain(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_gain)
{
    u_vpss_wr1_hvds_zme_vc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vc_gain = vc_gain;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_vzme_set_vc_coring(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_coring)
{
    u_vpss_wr1_hvds_zme_vc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr1_hvds_vc_coring = vc_coring;
    vpss_reg_write((&(vpss_reg->vpss_wr1_hvds_zme_vc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

/* HZME */
hi_void vpss_hzme_set_out_fmt(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 out_fmt)
{
    u_vpss_wr0_hds_zme_hinfo vpss_wr0_hds_zme_hinfo;
    vpss_wr0_hds_zme_hinfo.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hinfo.u32) + offset_addr));
    vpss_wr0_hds_zme_hinfo.bits.vpss_wr0_hds_out_fmt = out_fmt;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hinfo.u32) + offset_addr), vpss_wr0_hds_zme_hinfo.u32);
    return;
}

hi_void vpss_hzme_set_ck_gt_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 ck_gt_en)
{
    u_vpss_wr0_hds_zme_hinfo vpss_wr0_hds_zme_hinfo;
    vpss_wr0_hds_zme_hinfo.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hinfo.u32) + offset_addr));
    vpss_wr0_hds_zme_hinfo.bits.vpss_wr0_hds_ck_gt_en = ck_gt_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hinfo.u32) + offset_addr), vpss_wr0_hds_zme_hinfo.u32);
    return;
}

hi_void vpss_hzme_set_out_width(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 out_width)
{
    u_vpss_wr0_hds_zme_hinfo vpss_wr0_hds_zme_hinfo;
    vpss_wr0_hds_zme_hinfo.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hinfo.u32) + offset_addr));
    vpss_wr0_hds_zme_hinfo.bits.vpss_wr0_hds_out_width = out_width;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hinfo.u32) + offset_addr), vpss_wr0_hds_zme_hinfo.u32);
    return;
}

hi_void vpss_hzme_set_lhfir_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhfir_en)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_lhfir_en = lhfir_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_chfir_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chfir_en)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_chfir_en = chfir_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_lhmid_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhmid_en)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_lhmid_en = lhmid_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_chmid_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chmid_en)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_chmid_en = chmid_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_non_lnr_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 non_lnr_en)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_non_lnr_en = non_lnr_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_lhfir_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhfir_mode)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_lhfir_mode = lhfir_mode;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_chfir_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chfir_mode)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_chfir_mode = chfir_mode;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_hfir_order(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hfir_order)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_hfir_order = hfir_order;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_hratio(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hratio)
{
    u_vpss_wr0_hds_zme_hsp vpss_wr0_hds_zme_hsp;
    vpss_wr0_hds_zme_hsp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr));
    vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_hratio = hratio;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hsp.u32) + offset_addr), vpss_wr0_hds_zme_hsp.u32);
    return;
}

hi_void vpss_hzme_set_lhfir_offset(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhfir_offset)
{
    u_vpss_wr0_hds_zme_hloffset vpss_wr0_hds_zme_hloffset;
    vpss_wr0_hds_zme_hloffset.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hloffset.u32) + offset_addr));
    vpss_wr0_hds_zme_hloffset.bits.vpss_wr0_hds_lhfir_offset = lhfir_offset;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hloffset.u32) + offset_addr), vpss_wr0_hds_zme_hloffset.u32);
    return;
}

hi_void vpss_hzme_set_chfir_offset(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chfir_offset)
{
    u_vpss_wr0_hds_zme_hcoffset vpss_wr0_hds_zme_hcoffset;
    vpss_wr0_hds_zme_hcoffset.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hcoffset.u32) + offset_addr));
    vpss_wr0_hds_zme_hcoffset.bits.vpss_wr0_hds_chfir_offset = chfir_offset;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hcoffset.u32) + offset_addr), vpss_wr0_hds_zme_hcoffset.u32);
    return;
}

hi_void vpss_hzme_set_zone0_delta(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone0_delta)
{
    u_vpss_wr0_hds_zme_hzone0_delta para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hzone0_delta.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_zone0_delta = zone0_delta;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hzone0_delta.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_zone2_delta(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone2_delta)
{
    u_vpss_wr0_hds_zme_hzone2_delta para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hzone2_delta.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_zone2_delta = zone2_delta;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hzone2_delta.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_zone1_end(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone1_end)
{
    u_vpss_wr0_hds_zme_hzoneend vpss_wr0_hds_zme_hzoneend;
    vpss_wr0_hds_zme_hzoneend.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hzoneend.u32) + offset_addr));
    vpss_wr0_hds_zme_hzoneend.bits.vpss_wr0_hds_zone1_end = zone1_end;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hzoneend.u32) + offset_addr), vpss_wr0_hds_zme_hzoneend.u32);
    return;
}

hi_void vpss_hzme_set_zone0_end(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone0_end)
{
    u_vpss_wr0_hds_zme_hzoneend vpss_wr0_hds_zme_hzoneend;
    vpss_wr0_hds_zme_hzoneend.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hzoneend.u32) + offset_addr));
    vpss_wr0_hds_zme_hzoneend.bits.vpss_wr0_hds_zone0_end = zone0_end;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hzoneend.u32) + offset_addr), vpss_wr0_hds_zme_hzoneend.u32);
    return;
}

hi_void vpss_hzme_set_hl_shootctrl_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_shootctrl_en)
{
    u_vpss_wr0_hds_zme_hl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hl_shootctrl_en = hl_shootctrl_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hl_shootctrl_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_shootctrl_mode)
{
    u_vpss_wr0_hds_zme_hl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hl_shootctrl_mode = hl_shootctrl_mode;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hl_flatdect_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_flatdect_mode)
{
    u_vpss_wr0_hds_zme_hl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hl_flatdect_mode = hl_flatdect_mode;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hl_coringadj_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_coringadj_en)
{
    u_vpss_wr0_hds_zme_hl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hl_coringadj_en = hl_coringadj_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hl_gain(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_gain)
{
    u_vpss_wr0_hds_zme_hl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hl_gain = hl_gain;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hl_coring(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_coring)
{
    u_vpss_wr0_hds_zme_hl_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hl_coring = hl_coring;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hl_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hc_shootctrl_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_shootctrl_en)
{
    u_vpss_wr0_hds_zme_hc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hc_shootctrl_en = hc_shootctrl_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hc_shootctrl_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_shootctrl_mode)
{
    u_vpss_wr0_hds_zme_hc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hc_shootctrl_mode = hc_shootctrl_mode;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hc_flatdect_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_flatdect_mode)
{
    u_vpss_wr0_hds_zme_hc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hc_flatdect_mode = hc_flatdect_mode;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hc_coringadj_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_coringadj_en)
{
    u_vpss_wr0_hds_zme_hc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hc_coringadj_en = hc_coringadj_en;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hc_gain(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_gain)
{
    u_vpss_wr0_hds_zme_hc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hc_gain = hc_gain;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

hi_void vpss_hzme_set_hc_coring(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_coring)
{
    u_vpss_wr0_hds_zme_hc_shootctrl para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr));
    para_tmp.bits.vpss_wr0_hds_hc_coring = hc_coring;
    vpss_reg_write((&(vpss_regs->vpss_wr0_hds_zme_hc_shootctrl.u32) + offset_addr), para_tmp.u32);
    return;
}

/* Pzme */
hi_void vpss_pzme_set_pzme_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_en)
{
    u_vpss_pzme_ctrl vpss_pzme_ctrl;
    vpss_pzme_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_pzme_ctrl.u32) + addr_offset));
    vpss_pzme_ctrl.bits.pzme_en = pzme_en;
    vpss_reg_write((&(vpss_regs->vpss_pzme_ctrl.u32) + addr_offset), vpss_pzme_ctrl.u32);
    return;
}

hi_void vpss_pzme_set_pzme_out_img_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 height)
{
    u_vpss_pzme_out_img vpss_pzme_out_img;
    vpss_pzme_out_img.u32 = vpss_reg_read((&(vpss_regs->vpss_pzme_out_img.u32) + addr_offset));
    vpss_pzme_out_img.bits.pzme_out_img_height = height - 1;
    vpss_reg_write((&(vpss_regs->vpss_pzme_out_img.u32) + addr_offset), vpss_pzme_out_img.u32);
    return;
}

hi_void vpss_pzme_set_pzme_out_img_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 width)
{
    u_vpss_pzme_out_img vpss_pzme_out_img;
    vpss_pzme_out_img.u32 = vpss_reg_read((&(vpss_regs->vpss_pzme_out_img.u32) + addr_offset));
    vpss_pzme_out_img.bits.pzme_out_img_width = width - 1;
    vpss_reg_write((&(vpss_regs->vpss_pzme_out_img.u32) + addr_offset), vpss_pzme_out_img.u32);
    return;
}

hi_void vpss_pzme_set_pzme_hstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_h_step)
{
    u_vpss_pzme_h_step vpss_pzme_h_step;
    vpss_pzme_h_step.u32 = vpss_reg_read((&(vpss_regs->vpss_pzme_h_step.u32) + addr_offset));
    vpss_pzme_h_step.bits.pzme_h_step = pzme_h_step;
    vpss_reg_write((&(vpss_regs->vpss_pzme_h_step.u32) + addr_offset), vpss_pzme_h_step.u32);
    return;
}

hi_void vpss_pzme_set_pzme_wstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_w_step)
{
    u_vpss_pzme_w_step vpss_pzme_w_step;
    vpss_pzme_w_step.u32 = vpss_reg_read((&(vpss_regs->vpss_pzme_w_step.u32) + addr_offset));
    vpss_pzme_w_step.bits.pzme_w_step = pzme_w_step;
    vpss_reg_write((&(vpss_regs->vpss_pzme_w_step.u32) + addr_offset), vpss_pzme_w_step.u32);
    return;
}
/* Out0Pzme */
hi_void vpss_pzme_set_out0_pzme_mode(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_mode)
{
    u_vpss_out0_pzme_ctrl vpss_out0_pzme_ctrl;
    vpss_out0_pzme_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_pzme_ctrl.u32) + addr_offset));
    vpss_out0_pzme_ctrl.bits.vpss_out0_pzme_mode = pzme_mode;
    vpss_reg_write((&(vpss_regs->vpss_out0_pzme_ctrl.u32) + addr_offset), vpss_out0_pzme_ctrl.u32);
    return;
}

hi_void vpss_pzme_set_out0_pzme_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_en)
{
    u_vpss_out0_pzme_ctrl vpss_out0_pzme_ctrl;
    vpss_out0_pzme_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_pzme_ctrl.u32) + addr_offset));
    vpss_out0_pzme_ctrl.bits.vpss_out0_pzme_en = pzme_en;
    vpss_reg_write((&(vpss_regs->vpss_out0_pzme_ctrl.u32) + addr_offset), vpss_out0_pzme_ctrl.u32);
    return;
}

hi_void vpss_pzme_set_out0_pzme_out_img_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 height)
{
    u_vpss_out0_pzme_out_img vpss_out0_pzme_out_img;
    vpss_out0_pzme_out_img.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_pzme_out_img.u32) + addr_offset));
    vpss_out0_pzme_out_img.bits.vpss_out0_pzme_out_img_height = height - 1;
    vpss_reg_write((&(vpss_regs->vpss_out0_pzme_out_img.u32) + addr_offset), vpss_out0_pzme_out_img.u32);
    return;
}

hi_void vpss_pzme_set_out0_pzme_out_img_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 width)
{
    u_vpss_out0_pzme_out_img vpss_out0_pzme_out_img;
    vpss_out0_pzme_out_img.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_pzme_out_img.u32) + addr_offset));
    vpss_out0_pzme_out_img.bits.vpss_out0_pzme_out_img_width = width - 1;
    vpss_reg_write((&(vpss_regs->vpss_out0_pzme_out_img.u32) + addr_offset), vpss_out0_pzme_out_img.u32);
    return;
}

hi_void vpss_pzme_set_out0_pzme_hstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_h_step)
{
    u_vpss_out0_pzme_h_step vpss_out0_pzme_h_step;
    vpss_out0_pzme_h_step.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_pzme_h_step.u32) + addr_offset));
    vpss_out0_pzme_h_step.bits.vpss_out0_pzme_h_step = pzme_h_step;
    vpss_reg_write((&(vpss_regs->vpss_out0_pzme_h_step.u32) + addr_offset), vpss_out0_pzme_h_step.u32);
    return;
}

hi_void vpss_pzme_set_out0_pzme_wstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_w_step)
{
    u_vpss_out0_pzme_w_step vpss_out0_pzme_w_step;
    vpss_out0_pzme_w_step.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_pzme_w_step.u32) + addr_offset));
    vpss_out0_pzme_w_step.bits.vpss_out0_pzme_w_step = pzme_w_step;
    vpss_reg_write((&(vpss_regs->vpss_out0_pzme_w_step.u32) + addr_offset), vpss_out0_pzme_w_step.u32);
    return;
}

/* MASTER */
hi_void vpss_master_set_mstr0_woutstanding(vpss_reg_type *reg, hi_u32 mstr0_woutstanding)
{
    u_vpss_mst_outstanding vpss_mst_outstanding;
    vpss_mst_outstanding.u32 = vpss_reg_read((&(reg->vpss_mst_outstanding.u32)));
    vpss_mst_outstanding.bits.mstr0_woutstanding = mstr0_woutstanding;
    vpss_reg_write((&(reg->vpss_mst_outstanding.u32)), vpss_mst_outstanding.u32);
    return;
}

hi_void vpss_master_set_mstr0_routstanding(vpss_reg_type *reg, hi_u32 mstr0_routstanding)
{
    u_vpss_mst_outstanding vpss_mst_outstanding;
    vpss_mst_outstanding.u32 = vpss_reg_read((&(reg->vpss_mst_outstanding.u32)));
    vpss_mst_outstanding.bits.mstr0_routstanding = mstr0_routstanding;
    vpss_reg_write((&(reg->vpss_mst_outstanding.u32)), vpss_mst_outstanding.u32);
    return;
}

/* HFR */
hi_void vpss_mac_set_nr_refy_bd_det_en(vpss_reg_type *reg, hi_u32 nr_refy_bd_det_en)
{
    u_vpss_fdcmp_nr_refy_fhd_y_glb_info vpss_fdcmp_nr_refy_fhd_y_glb_info;
    vpss_fdcmp_nr_refy_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)));
    vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_bd_det_en = nr_refy_bd_det_en;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)), vpss_fdcmp_nr_refy_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_refy_frame_bitdepth)
{
    u_vpss_fdcmp_nr_refy_fhd_y_glb_info vpss_fdcmp_nr_refy_fhd_y_glb_info;
    vpss_fdcmp_nr_refy_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)));
    vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_frame_bitdepth = nr_refy_frame_bitdepth;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)), vpss_fdcmp_nr_refy_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_pixel_format(vpss_reg_type *reg, hi_u32 nr_refy_pixel_format)
{
    u_vpss_fdcmp_nr_refy_fhd_y_glb_info vpss_fdcmp_nr_refy_fhd_y_glb_info;
    vpss_fdcmp_nr_refy_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)));
    vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_pixel_format = nr_refy_pixel_format;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)), vpss_fdcmp_nr_refy_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_cmp_mode(vpss_reg_type *reg, hi_u32 nr_refy_cmp_mode)
{
    u_vpss_fdcmp_nr_refy_fhd_y_glb_info vpss_fdcmp_nr_refy_fhd_y_glb_info;
    vpss_fdcmp_nr_refy_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)));
    vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_cmp_mode = nr_refy_cmp_mode;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)), vpss_fdcmp_nr_refy_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_is_lossless(vpss_reg_type *reg, hi_u32 nr_refy_is_lossless)
{
    u_vpss_fdcmp_nr_refy_fhd_y_glb_info vpss_fdcmp_nr_refy_fhd_y_glb_info;
    vpss_fdcmp_nr_refy_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)));
    vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_is_lossless = nr_refy_is_lossless;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)), vpss_fdcmp_nr_refy_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_dcmp_en(vpss_reg_type *reg, hi_u32 nr_refy_dcmp_en)
{
    u_vpss_fdcmp_nr_refy_fhd_y_glb_info vpss_fdcmp_nr_refy_fhd_y_glb_info;
    vpss_fdcmp_nr_refy_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)));
    vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_dcmp_en = nr_refy_dcmp_en;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.u32)), vpss_fdcmp_nr_refy_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_frame_height(vpss_reg_type *reg, hi_u32 nr_refy_frame_height)
{
    u_vpss_fdcmp_nr_refy_fhd_y_frame_size vpss_fdcmp_nr_refy_fhd_y_frame_size;
    vpss_fdcmp_nr_refy_fhd_y_frame_size.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_frame_size.u32)));
    vpss_fdcmp_nr_refy_fhd_y_frame_size.bits.nr_refy_frame_height = nr_refy_frame_height;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_frame_size.u32)), vpss_fdcmp_nr_refy_fhd_y_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_frame_width(vpss_reg_type *reg, hi_u32 nr_refy_frame_width)
{
    u_vpss_fdcmp_nr_refy_fhd_y_frame_size vpss_fdcmp_nr_refy_fhd_y_frame_size;
    vpss_fdcmp_nr_refy_fhd_y_frame_size.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_frame_size.u32)));
    vpss_fdcmp_nr_refy_fhd_y_frame_size.bits.nr_refy_frame_width = nr_refy_frame_width;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_frame_size.u32)), vpss_fdcmp_nr_refy_fhd_y_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_buffer_fullness_thr(vpss_reg_type *reg, hi_u32 nr_refy_buffer_fullness_thr)
{
    u_vpss_fdcmp_nr_refy_fhd_y_bd_det0 vpss_fdcmp_nr_refy_fhd_y_bd_det0;
    vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32)));
    vpss_fdcmp_nr_refy_fhd_y_bd_det0.bits.nr_refy_buffer_fullness_thr = nr_refy_buffer_fullness_thr;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32)), vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_refy_buffer_init_bits)
{
    u_vpss_fdcmp_nr_refy_fhd_y_bd_det0 vpss_fdcmp_nr_refy_fhd_y_bd_det0;
    vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32)));
    vpss_fdcmp_nr_refy_fhd_y_bd_det0.bits.nr_refy_buffer_init_bits = nr_refy_buffer_init_bits;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32)), vpss_fdcmp_nr_refy_fhd_y_bd_det0.u32);
    return;
}

hi_void vpss_mac_set_nr_refy_budget_mb_bits(vpss_reg_type *reg, hi_u32 nr_refy_budget_mb_bits)
{
    u_vpss_fdcmp_nr_refy_fhd_y_bd_det1 vpss_fdcmp_nr_refy_fhd_y_bd_det1;
    vpss_fdcmp_nr_refy_fhd_y_bd_det1.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refy_fhd_y_bd_det1.u32)));
    vpss_fdcmp_nr_refy_fhd_y_bd_det1.bits.nr_refy_budget_mb_bits = nr_refy_budget_mb_bits;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refy_fhd_y_bd_det1.u32)), vpss_fdcmp_nr_refy_fhd_y_bd_det1.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_bd_det_en(vpss_reg_type *reg, hi_u32 nr_refc_bd_det_en)
{
    u_vpss_fdcmp_nr_refc_fhd_c_glb_info vpss_fdcmp_nr_refc_fhd_c_glb_info;
    vpss_fdcmp_nr_refc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)));
    vpss_fdcmp_nr_refc_fhd_c_glb_info.bits.nr_refc_bd_det_en = nr_refc_bd_det_en;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)), vpss_fdcmp_nr_refc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_refc_frame_bitdepth)
{
    u_vpss_fdcmp_nr_refc_fhd_c_glb_info vpss_fdcmp_nr_refc_fhd_c_glb_info;
    vpss_fdcmp_nr_refc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)));
    vpss_fdcmp_nr_refc_fhd_c_glb_info.bits.nr_refc_frame_bitdepth = nr_refc_frame_bitdepth;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)), vpss_fdcmp_nr_refc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_pixel_format(vpss_reg_type *reg, hi_u32 nr_refc_pixel_format)
{
    u_vpss_fdcmp_nr_refc_fhd_c_glb_info vpss_fdcmp_nr_refc_fhd_c_glb_info;
    vpss_fdcmp_nr_refc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)));
    vpss_fdcmp_nr_refc_fhd_c_glb_info.bits.nr_refc_pixel_format = nr_refc_pixel_format;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)), vpss_fdcmp_nr_refc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_cmp_mode(vpss_reg_type *reg, hi_u32 nr_refc_cmp_mode)
{
    u_vpss_fdcmp_nr_refc_fhd_c_glb_info vpss_fdcmp_nr_refc_fhd_c_glb_info;
    vpss_fdcmp_nr_refc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)));
    vpss_fdcmp_nr_refc_fhd_c_glb_info.bits.nr_refc_cmp_mode = nr_refc_cmp_mode;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)), vpss_fdcmp_nr_refc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_is_lossless(vpss_reg_type *reg, hi_u32 nr_refc_is_lossless)
{
    u_vpss_fdcmp_nr_refc_fhd_c_glb_info vpss_fdcmp_nr_refc_fhd_c_glb_info;
    vpss_fdcmp_nr_refc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)));
    vpss_fdcmp_nr_refc_fhd_c_glb_info.bits.nr_refc_is_lossless = nr_refc_is_lossless;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)), vpss_fdcmp_nr_refc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_dcmp_en(vpss_reg_type *reg, hi_u32 nr_refc_dcmp_en)
{
    u_vpss_fdcmp_nr_refc_fhd_c_glb_info vpss_fdcmp_nr_refc_fhd_c_glb_info;
    vpss_fdcmp_nr_refc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)));
    vpss_fdcmp_nr_refc_fhd_c_glb_info.bits.nr_refc_dcmp_en = nr_refc_dcmp_en;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.u32)), vpss_fdcmp_nr_refc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_frame_height(vpss_reg_type *reg, hi_u32 nr_refc_frame_height)
{
    u_vpss_fdcmp_nr_refc_fhd_c_frame_size vpss_fdcmp_nr_refc_fhd_c_frame_size;
    vpss_fdcmp_nr_refc_fhd_c_frame_size.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_frame_size.u32)));
    vpss_fdcmp_nr_refc_fhd_c_frame_size.bits.nr_refc_frame_height = nr_refc_frame_height;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_frame_size.u32)), vpss_fdcmp_nr_refc_fhd_c_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_frame_width(vpss_reg_type *reg, hi_u32 nr_refc_frame_width)
{
    u_vpss_fdcmp_nr_refc_fhd_c_frame_size vpss_fdcmp_nr_refc_fhd_c_frame_size;
    vpss_fdcmp_nr_refc_fhd_c_frame_size.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_frame_size.u32)));
    vpss_fdcmp_nr_refc_fhd_c_frame_size.bits.nr_refc_frame_width = nr_refc_frame_width;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_frame_size.u32)), vpss_fdcmp_nr_refc_fhd_c_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_buffer_fullness_thr(vpss_reg_type *reg, hi_u32 nr_refc_buffer_fullness_thr)
{
    u_vpss_fdcmp_nr_refc_fhd_c_bd_det0 vpss_fdcmp_nr_refc_fhd_c_bd_det0;
    vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32)));
    vpss_fdcmp_nr_refc_fhd_c_bd_det0.bits.nr_refc_buffer_fullness_thr = nr_refc_buffer_fullness_thr;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32)), vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_refc_buffer_init_bits)
{
    u_vpss_fdcmp_nr_refc_fhd_c_bd_det0 vpss_fdcmp_nr_refc_fhd_c_bd_det0;
    vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32)));
    vpss_fdcmp_nr_refc_fhd_c_bd_det0.bits.nr_refc_buffer_init_bits = nr_refc_buffer_init_bits;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32)), vpss_fdcmp_nr_refc_fhd_c_bd_det0.u32);
    return;
}

hi_void vpss_mac_set_nr_refc_budget_mb_bits(vpss_reg_type *reg, hi_u32 nr_refc_budget_mb_bits)
{
    u_vpss_fdcmp_nr_refc_fhd_c_bd_det1 vpss_fdcmp_nr_refc_fhd_c_bd_det1;
    vpss_fdcmp_nr_refc_fhd_c_bd_det1.u32 = vpss_reg_read((&(reg->vpss_fdcmp_nr_refc_fhd_c_bd_det1.u32)));
    vpss_fdcmp_nr_refc_fhd_c_bd_det1.bits.nr_refc_budget_mb_bits = nr_refc_budget_mb_bits;
    vpss_reg_write((&(reg->vpss_fdcmp_nr_refc_fhd_c_bd_det1.u32)), vpss_fdcmp_nr_refc_fhd_c_bd_det1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_force_en(vpss_reg_type *reg, hi_u32 nr_rfry_qp_force_en)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_qp_force_en = nr_rfry_qp_force_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_grph_en(vpss_reg_type *reg, hi_u32 nr_rfry_grph_en)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_grph_en = nr_rfry_grph_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_part_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfry_part_cmp_en)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_part_cmp_en = nr_rfry_part_cmp_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_pixel_format(vpss_reg_type *reg, hi_u32 nr_rfry_pixel_format)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_pixel_format = nr_rfry_pixel_format;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_rfry_frame_bitdepth)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_frame_bitdepth = nr_rfry_frame_bitdepth;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_cmp_mode(vpss_reg_type *reg, hi_u32 nr_rfry_cmp_mode)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_cmp_mode = nr_rfry_cmp_mode;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_is_lossless(vpss_reg_type *reg, hi_u32 nr_rfry_is_lossless)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_is_lossless = nr_rfry_is_lossless;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfry_cmp_en)
{
    u_vpss_fcmp_nr_rfry_fhd_y_glb_info vpss_fcmp_nr_rfry_fhd_y_glb_info;
    vpss_fcmp_nr_rfry_fhd_y_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)));
    vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_cmp_en = nr_rfry_cmp_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.u32)), vpss_fcmp_nr_rfry_fhd_y_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_frame_height(vpss_reg_type *reg, hi_u32 nr_rfry_frame_height)
{
    u_vpss_fcmp_nr_rfry_fhd_y_frame_size vpss_fcmp_nr_rfry_fhd_y_frame_size;
    vpss_fcmp_nr_rfry_fhd_y_frame_size.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_frame_size.u32)));
    vpss_fcmp_nr_rfry_fhd_y_frame_size.bits.nr_rfry_frame_height = nr_rfry_frame_height;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_frame_size.u32)), vpss_fcmp_nr_rfry_fhd_y_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_frame_width(vpss_reg_type *reg, hi_u32 nr_rfry_frame_width)
{
    u_vpss_fcmp_nr_rfry_fhd_y_frame_size vpss_fcmp_nr_rfry_fhd_y_frame_size;
    vpss_fcmp_nr_rfry_fhd_y_frame_size.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_frame_size.u32)));
    vpss_fcmp_nr_rfry_fhd_y_frame_size.bits.nr_rfry_frame_width = nr_rfry_frame_width;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_frame_size.u32)), vpss_fcmp_nr_rfry_fhd_y_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_pcmp_end_hpos(vpss_reg_type *reg, hi_u32 nr_rfry_pcmp_end_hpos)
{
    u_vpss_fcmp_nr_rfry_fhd_y_pcmp vpss_fcmp_nr_rfry_fhd_y_pcmp;
    vpss_fcmp_nr_rfry_fhd_y_pcmp.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_pcmp.u32)));
    vpss_fcmp_nr_rfry_fhd_y_pcmp.bits.nr_rfry_pcmp_end_hpos = nr_rfry_pcmp_end_hpos;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_pcmp.u32)), vpss_fcmp_nr_rfry_fhd_y_pcmp.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_pcmp_start_hpos(vpss_reg_type *reg, hi_u32 nr_rfry_pcmp_start_hpos)
{
    u_vpss_fcmp_nr_rfry_fhd_y_pcmp vpss_fcmp_nr_rfry_fhd_y_pcmp;
    vpss_fcmp_nr_rfry_fhd_y_pcmp.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_pcmp.u32)));
    vpss_fcmp_nr_rfry_fhd_y_pcmp.bits.nr_rfry_pcmp_start_hpos = nr_rfry_pcmp_start_hpos;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_pcmp.u32)), vpss_fcmp_nr_rfry_fhd_y_pcmp.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_min_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfry_min_mb_bits)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg0 vpss_fcmp_nr_rfry_fhd_y_rc_cfg0;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.bits.nr_rfry_min_mb_bits = nr_rfry_min_mb_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfry_mb_bits)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg0 vpss_fcmp_nr_rfry_fhd_y_rc_cfg0;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.bits.nr_rfry_mb_bits = nr_rfry_mb_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg0.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_first_col_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfry_first_col_adj_bits)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg1 vpss_fcmp_nr_rfry_fhd_y_rc_cfg1;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.bits.nr_rfry_first_col_adj_bits = nr_rfry_first_col_adj_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_first_row_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfry_first_row_adj_bits)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg1 vpss_fcmp_nr_rfry_fhd_y_rc_cfg1;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.bits.nr_rfry_first_row_adj_bits = nr_rfry_first_row_adj_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_smooth_status_thr(vpss_reg_type *reg, hi_u32 nr_rfry_smooth_status_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg1 vpss_fcmp_nr_rfry_fhd_y_rc_cfg1;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.bits.nr_rfry_smooth_status_thr = nr_rfry_smooth_status_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_first_mb_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfry_first_mb_adj_bits)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg1 vpss_fcmp_nr_rfry_fhd_y_rc_cfg1;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.bits.nr_rfry_first_mb_adj_bits = nr_rfry_first_mb_adj_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_diff_thr(vpss_reg_type *reg, hi_u32 nr_rfry_diff_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg2 vpss_fcmp_nr_rfry_fhd_y_rc_cfg2;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.bits.nr_rfry_diff_thr = nr_rfry_diff_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_big_grad_thr(vpss_reg_type *reg, hi_u32 nr_rfry_big_grad_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg2 vpss_fcmp_nr_rfry_fhd_y_rc_cfg2;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.bits.nr_rfry_big_grad_thr = nr_rfry_big_grad_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_still_thr(vpss_reg_type *reg, hi_u32 nr_rfry_still_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg2 vpss_fcmp_nr_rfry_fhd_y_rc_cfg2;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.bits.nr_rfry_still_thr = nr_rfry_still_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_smth_thr(vpss_reg_type *reg, hi_u32 nr_rfry_smth_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg2 vpss_fcmp_nr_rfry_fhd_y_rc_cfg2;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.bits.nr_rfry_smth_thr = nr_rfry_smth_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_noise_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfry_noise_pix_num_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg3 vpss_fcmp_nr_rfry_fhd_y_rc_cfg3;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.bits.nr_rfry_noise_pix_num_thr = nr_rfry_noise_pix_num_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_still_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfry_still_pix_num_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg3 vpss_fcmp_nr_rfry_fhd_y_rc_cfg3;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.bits.nr_rfry_still_pix_num_thr = nr_rfry_still_pix_num_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_smth_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfry_smth_pix_num_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg3 vpss_fcmp_nr_rfry_fhd_y_rc_cfg3;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.bits.nr_rfry_smth_pix_num_thr = nr_rfry_smth_pix_num_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg3.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_dec2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_dec2_bits_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg4 vpss_fcmp_nr_rfry_fhd_y_rc_cfg4;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.bits.nr_rfry_qp_dec2_bits_thr = nr_rfry_qp_dec2_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_dec1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_dec1_bits_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg4 vpss_fcmp_nr_rfry_fhd_y_rc_cfg4;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.bits.nr_rfry_qp_dec1_bits_thr = nr_rfry_qp_dec1_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_inc2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_inc2_bits_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg4 vpss_fcmp_nr_rfry_fhd_y_rc_cfg4;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.bits.nr_rfry_qp_inc2_bits_thr = nr_rfry_qp_inc2_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_inc1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_inc1_bits_thr)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg4 vpss_fcmp_nr_rfry_fhd_y_rc_cfg4;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.bits.nr_rfry_qp_inc1_bits_thr = nr_rfry_qp_inc1_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_grph_bits_penalty(vpss_reg_type *reg, hi_u32 nr_rfry_grph_bits_penalty)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg5 vpss_fcmp_nr_rfry_fhd_y_rc_cfg5;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg5.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg5.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg5.bits.nr_rfry_grph_bits_penalty = nr_rfry_grph_bits_penalty;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg5.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg5.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_buf_fullness_thr_reg0(vpss_reg_type *reg, hi_u32 nr_rfry_buf_fullness_thr_reg0)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg6 vpss_fcmp_nr_rfry_fhd_y_rc_cfg6;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg6.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg6.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg6.bits.nr_rfry_buf_fullness_thr_reg0 = nr_rfry_buf_fullness_thr_reg0;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg6.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg6.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_buf_fullness_thr_reg1(vpss_reg_type *reg, hi_u32 nr_rfry_buf_fullness_thr_reg1)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg7 vpss_fcmp_nr_rfry_fhd_y_rc_cfg7;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg7.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg7.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg7.bits.nr_rfry_buf_fullness_thr_reg1 = nr_rfry_buf_fullness_thr_reg1;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg7.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg7.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_buf_fullness_thr_reg2(vpss_reg_type *reg, hi_u32 nr_rfry_buf_fullness_thr_reg2)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg8 vpss_fcmp_nr_rfry_fhd_y_rc_cfg8;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg8.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg8.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg8.bits.nr_rfry_buf_fullness_thr_reg2 = nr_rfry_buf_fullness_thr_reg2;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg8.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg8.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_rge_reg0(vpss_reg_type *reg, hi_u32 nr_rfry_qp_rge_reg0)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg9 vpss_fcmp_nr_rfry_fhd_y_rc_cfg9;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg9.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg9.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg9.bits.nr_rfry_qp_rge_reg0 = nr_rfry_qp_rge_reg0;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg9.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg9.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_rge_reg1(vpss_reg_type *reg, hi_u32 nr_rfry_qp_rge_reg1)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg10 vpss_fcmp_nr_rfry_fhd_y_rc_cfg10;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg10.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg10.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg10.bits.nr_rfry_qp_rge_reg1 = nr_rfry_qp_rge_reg1;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg10.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg10.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_qp_rge_reg2(vpss_reg_type *reg, hi_u32 nr_rfry_qp_rge_reg2)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg11 vpss_fcmp_nr_rfry_fhd_y_rc_cfg11;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg11.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg11.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg11.bits.nr_rfry_qp_rge_reg2 = nr_rfry_qp_rge_reg2;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg11.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg11.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_bits_offset_reg0(vpss_reg_type *reg, hi_u32 nr_rfry_bits_offset_reg0)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg12 vpss_fcmp_nr_rfry_fhd_y_rc_cfg12;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg12.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg12.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg12.bits.nr_rfry_bits_offset_reg0 = nr_rfry_bits_offset_reg0;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg12.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg12.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_bits_offset_reg1(vpss_reg_type *reg, hi_u32 nr_rfry_bits_offset_reg1)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg13 vpss_fcmp_nr_rfry_fhd_y_rc_cfg13;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg13.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg13.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg13.bits.nr_rfry_bits_offset_reg1 = nr_rfry_bits_offset_reg1;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg13.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg13.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_bits_offset_reg2(vpss_reg_type *reg, hi_u32 nr_rfry_bits_offset_reg2)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg14 vpss_fcmp_nr_rfry_fhd_y_rc_cfg14;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg14.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg14.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg14.bits.nr_rfry_bits_offset_reg2 = nr_rfry_bits_offset_reg2;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg14.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg14.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_grph_ideal_bits(vpss_reg_type *reg, hi_u32 nr_rfry_grph_ideal_bits)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg15 vpss_fcmp_nr_rfry_fhd_y_rc_cfg15;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg15.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg15.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg15.bits.nr_rfry_grph_ideal_bits = nr_rfry_grph_ideal_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg15.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg15.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_est_err_gain_map(vpss_reg_type *reg, hi_u32 nr_rfry_est_err_gain_map)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg16 vpss_fcmp_nr_rfry_fhd_y_rc_cfg16;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg16.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg16.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg16.bits.nr_rfry_est_err_gain_map = nr_rfry_est_err_gain_map;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg16.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg16.u32);
    return;
}

hi_void vpss_mac_set_nr_rfry_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_rfry_buffer_init_bits)
{
    u_vpss_fcmp_nr_rfry_fhd_y_rc_cfg17 vpss_fcmp_nr_rfry_fhd_y_rc_cfg17;
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg17.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg17.u32)));
    vpss_fcmp_nr_rfry_fhd_y_rc_cfg17.bits.nr_rfry_buffer_init_bits = nr_rfry_buffer_init_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfry_fhd_y_rc_cfg17.u32)), vpss_fcmp_nr_rfry_fhd_y_rc_cfg17.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_force_en(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_force_en)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_qp_force_en = nr_rfrc_qp_force_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_grph_en(vpss_reg_type *reg, hi_u32 nr_rfrc_grph_en)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_grph_en = nr_rfrc_grph_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_part_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfrc_part_cmp_en)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_part_cmp_en = nr_rfrc_part_cmp_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_pixel_format(vpss_reg_type *reg, hi_u32 nr_rfrc_pixel_format)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_pixel_format = nr_rfrc_pixel_format;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_rfrc_frame_bitdepth)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_frame_bitdepth = nr_rfrc_frame_bitdepth;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_cmp_mode(vpss_reg_type *reg, hi_u32 nr_rfrc_cmp_mode)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_cmp_mode = nr_rfrc_cmp_mode;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_is_lossless(vpss_reg_type *reg, hi_u32 nr_rfrc_is_lossless)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_is_lossless = nr_rfrc_is_lossless;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfrc_cmp_en)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_glb_info vpss_fcmp_nr_rfrc_fhd_c_glb_info;
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_cmp_en = nr_rfrc_cmp_en;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32)), vpss_fcmp_nr_rfrc_fhd_c_glb_info.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_frame_height(vpss_reg_type *reg, hi_u32 nr_rfrc_frame_height)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_frame_size vpss_fcmp_nr_rfrc_fhd_c_frame_size;
    vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_frame_size.bits.nr_rfrc_frame_height = nr_rfrc_frame_height;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32)), vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_frame_width(vpss_reg_type *reg, hi_u32 nr_rfrc_frame_width)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_frame_size vpss_fcmp_nr_rfrc_fhd_c_frame_size;
    vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_frame_size.bits.nr_rfrc_frame_width = nr_rfrc_frame_width;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32)), vpss_fcmp_nr_rfrc_fhd_c_frame_size.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_pcmp_end_hpos(vpss_reg_type *reg, hi_u32 nr_rfrc_pcmp_end_hpos)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_pcmp vpss_fcmp_nr_rfrc_fhd_c_pcmp;
    vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_pcmp.bits.nr_rfrc_pcmp_end_hpos = nr_rfrc_pcmp_end_hpos;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32)), vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_pcmp_start_hpos(vpss_reg_type *reg, hi_u32 nr_rfrc_pcmp_start_hpos)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_pcmp vpss_fcmp_nr_rfrc_fhd_c_pcmp;
    vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_pcmp.bits.nr_rfrc_pcmp_start_hpos = nr_rfrc_pcmp_start_hpos;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32)), vpss_fcmp_nr_rfrc_fhd_c_pcmp.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_min_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_min_mb_bits)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.bits.nr_rfrc_min_mb_bits = nr_rfrc_min_mb_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_mb_bits)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.bits.nr_rfrc_mb_bits = nr_rfrc_mb_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg0.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_first_col_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_first_col_adj_bits)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.bits.nr_rfrc_first_col_adj_bits = nr_rfrc_first_col_adj_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_first_row_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_first_row_adj_bits)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.bits.nr_rfrc_first_row_adj_bits = nr_rfrc_first_row_adj_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_smooth_status_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_smooth_status_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.bits.nr_rfrc_smooth_status_thr = nr_rfrc_smooth_status_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_first_mb_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_first_mb_adj_bits)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.bits.nr_rfrc_first_mb_adj_bits = nr_rfrc_first_mb_adj_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg1.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_diff_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_diff_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.bits.nr_rfrc_diff_thr = nr_rfrc_diff_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_big_grad_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_big_grad_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.bits.nr_rfrc_big_grad_thr = nr_rfrc_big_grad_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_still_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_still_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.bits.nr_rfrc_still_thr = nr_rfrc_still_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_smth_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_smth_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.bits.nr_rfrc_smth_thr = nr_rfrc_smth_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg2.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_noise_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_noise_pix_num_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.bits.nr_rfrc_noise_pix_num_thr = nr_rfrc_noise_pix_num_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_still_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_still_pix_num_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.bits.nr_rfrc_still_pix_num_thr = nr_rfrc_still_pix_num_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_smth_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_smth_pix_num_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.bits.nr_rfrc_smth_pix_num_thr = nr_rfrc_smth_pix_num_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg3.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_dec2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_dec2_bits_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.bits.nr_rfrc_qp_dec2_bits_thr = nr_rfrc_qp_dec2_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_dec1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_dec1_bits_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.bits.nr_rfrc_qp_dec1_bits_thr = nr_rfrc_qp_dec1_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_inc2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_inc2_bits_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.bits.nr_rfrc_qp_inc2_bits_thr = nr_rfrc_qp_inc2_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_inc1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_inc1_bits_thr)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.bits.nr_rfrc_qp_inc1_bits_thr = nr_rfrc_qp_inc1_bits_thr;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg4.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_grph_bits_penalty(vpss_reg_type *reg, hi_u32 nr_rfrc_grph_bits_penalty)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg5 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg5;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg5.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg5.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg5.bits.nr_rfrc_grph_bits_penalty = nr_rfrc_grph_bits_penalty;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg5.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg5.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_buf_fullness_thr_reg0(vpss_reg_type *reg, hi_u32 nr_rfrc_buf_fullness_thr_reg0)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg6 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg6;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg6.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg6.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg6.bits.nr_rfrc_buf_fullness_thr_reg0 = nr_rfrc_buf_fullness_thr_reg0;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg6.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg6.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_buf_fullness_thr_reg1(vpss_reg_type *reg, hi_u32 nr_rfrc_buf_fullness_thr_reg1)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg7 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg7;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg7.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg7.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg7.bits.nr_rfrc_buf_fullness_thr_reg1 = nr_rfrc_buf_fullness_thr_reg1;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg7.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg7.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_buf_fullness_thr_reg2(vpss_reg_type *reg, hi_u32 nr_rfrc_buf_fullness_thr_reg2)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg8 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg8;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg8.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg8.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg8.bits.nr_rfrc_buf_fullness_thr_reg2 = nr_rfrc_buf_fullness_thr_reg2;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg8.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg8.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_rge_reg0(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_rge_reg0)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg9 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg9;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg9.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg9.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg9.bits.nr_rfrc_qp_rge_reg0 = nr_rfrc_qp_rge_reg0;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg9.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg9.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_rge_reg1(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_rge_reg1)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg10 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg10;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg10.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg10.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg10.bits.nr_rfrc_qp_rge_reg1 = nr_rfrc_qp_rge_reg1;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg10.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg10.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_qp_rge_reg2(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_rge_reg2)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg11 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg11;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg11.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg11.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg11.bits.nr_rfrc_qp_rge_reg2 = nr_rfrc_qp_rge_reg2;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg11.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg11.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_bits_offset_reg0(vpss_reg_type *reg, hi_u32 nr_rfrc_bits_offset_reg0)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg12 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg12;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg12.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg12.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg12.bits.nr_rfrc_bits_offset_reg0 = nr_rfrc_bits_offset_reg0;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg12.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg12.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_bits_offset_reg1(vpss_reg_type *reg, hi_u32 nr_rfrc_bits_offset_reg1)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg13 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg13;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg13.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg13.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg13.bits.nr_rfrc_bits_offset_reg1 = nr_rfrc_bits_offset_reg1;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg13.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg13.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_bits_offset_reg2(vpss_reg_type *reg, hi_u32 nr_rfrc_bits_offset_reg2)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg14 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg14;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg14.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg14.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg14.bits.nr_rfrc_bits_offset_reg2 = nr_rfrc_bits_offset_reg2;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg14.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg14.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_grph_ideal_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_grph_ideal_bits)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg15 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg15;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg15.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg15.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg15.bits.nr_rfrc_grph_ideal_bits = nr_rfrc_grph_ideal_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg15.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg15.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_est_err_gain_map(vpss_reg_type *reg, hi_u32 nr_rfrc_est_err_gain_map)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg16 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg16;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg16.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg16.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg16.bits.nr_rfrc_est_err_gain_map = nr_rfrc_est_err_gain_map;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg16.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg16.u32);
    return;
}

hi_void vpss_mac_set_nr_rfrc_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_buffer_init_bits)
{
    u_vpss_fcmp_nr_rfrc_fhd_c_rc_cfg17 vpss_fcmp_nr_rfrc_fhd_c_rc_cfg17;
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg17.u32 = vpss_reg_read((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg17.u32)));
    vpss_fcmp_nr_rfrc_fhd_c_rc_cfg17.bits.nr_rfrc_buffer_init_bits = nr_rfrc_buffer_init_bits;
    vpss_reg_write((&(reg->vpss_fcmp_nr_rfrc_fhd_c_rc_cfg17.u32)), vpss_fcmp_nr_rfrc_fhd_c_rc_cfg17.u32);
    return;
}

hi_void vpss_mmu_set_glb_bypass(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 glb_bypass)
{
    u_vpss0_smmu_scr vpss0_smmu_scr;
    vpss0_smmu_scr.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_scr.u32) + layer * MMU_OFFSET));
    vpss0_smmu_scr.bits.glb_bypass = glb_bypass;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_scr.u32) + layer * MMU_OFFSET), vpss0_smmu_scr.u32);
    return;
}
hi_void vpss_mmu_set_auto_clk_gt_en(vpss_reg_type *vdp_reg, hi_u32 layer, hi_u32 auto_clk_gt_en)
{
    u_vpss0_smmu_lp_ctrl vpss0_smmu_lp_ctrl;
    vpss0_smmu_lp_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss0_smmu_lp_ctrl.u32) + layer * MMU_OFFSET));
    vpss0_smmu_lp_ctrl.bits.auto_clk_gt_en = auto_clk_gt_en;
    vpss_reg_write((&(vdp_reg->vpss0_smmu_lp_ctrl.u32) + layer * MMU_OFFSET), vpss0_smmu_lp_ctrl.u32);
    return;
}
hi_void vpss_mmu_set_int_en(vpss_reg_type *vdp_reg, hi_u32 layer, hi_u32 int_en)
{
    u_vpss0_smmu_ctrl vpss0_smmu_ctrl;
    vpss0_smmu_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss0_smmu_ctrl.u32) + layer * MMU_OFFSET));
    vpss0_smmu_ctrl.bits.int_en = int_en;
    vpss_reg_write((&(vdp_reg->vpss0_smmu_ctrl.u32) + layer * MMU_OFFSET), vpss0_smmu_ctrl.u32);
    return;
}
hi_void vpss_mmu_set_ptw_pf(vpss_reg_type *vdp_reg, hi_u32 layer, hi_u32 ptw_pf)
{
    u_vpss0_smmu_ctrl vpss0_smmu_ctrl;
    vpss0_smmu_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss0_smmu_ctrl.u32) + layer * MMU_OFFSET));
    vpss0_smmu_ctrl.bits.ptw_pf = ptw_pf;
    vpss_reg_write((&(vdp_reg->vpss0_smmu_ctrl.u32) + layer * MMU_OFFSET), vpss0_smmu_ctrl.u32);
    return;
}
hi_void vpss_mmu_set_scb_ttbr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 scb_ttbr)
{
    u_vpss0_smmu_scb_ttbr vpss0_smmu_scb_ttbr;
    vpss0_smmu_scb_ttbr.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_scb_ttbr.u32) + layer * MMU_OFFSET));
    vpss0_smmu_scb_ttbr.bits.scb_ttbr = scb_ttbr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_scb_ttbr.u32) + layer * MMU_OFFSET), vpss0_smmu_scb_ttbr.u32);
    return;
}
hi_void vpss_mmu_set_scb_ttbr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 scb_ttbr_h)
{
    u_vpss0_smmu_scb_ttbr_h vpss0_smmu_scb_ttbr_h;
    vpss0_smmu_scb_ttbr_h.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_scb_ttbr_h.u32) + layer * MMU_OFFSET));
    vpss0_smmu_scb_ttbr_h.bits.scb_ttbr_h = scb_ttbr_h;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_scb_ttbr_h.u32) + layer * MMU_OFFSET), vpss0_smmu_scb_ttbr_h.u32);
    return;
}

hi_void vpss_mmu_set_err_srd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_rd_addr)
{
    u_vpss0_smmu_err_rdaddr vpss0_smmu_err_rdaddr;
    vpss0_smmu_err_rdaddr.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_rdaddr.u32) + layer * MMU_OFFSET));
    vpss0_smmu_err_rdaddr.bits.err_s_rd_addr = err_s_rd_addr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_rdaddr.u32) + layer * MMU_OFFSET), vpss0_smmu_err_rdaddr.u32);
    return;
}
hi_void vpss_mmu_set_err_srd_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_rd_addr_h)
{
    u_vpss0_smmu_err_rdaddr_h vpss0_smmu_err_rdaddr_h;
    vpss0_smmu_err_rdaddr_h.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_rdaddr_h.u32) + layer * MMU_OFFSET));
    vpss0_smmu_err_rdaddr_h.bits.err_s_rd_addr_h = err_s_rd_addr_h;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_rdaddr_h.u32) + layer * MMU_OFFSET), vpss0_smmu_err_rdaddr_h.u32);
    return;
}
hi_void vpss_mmu_get_err_srd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_s_rd_addr)
{
    *err_s_rd_addr = vpss_reg_read((&(vpss_reg->vpss0_smmu_fault_addr_rd.u32) + layer * MMU_OFFSET));
    return;
}

hi_void vpss_mmu_set_err_swr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_wr_addr)
{
    u_vpss0_smmu_err_wraddr vpss0_smmu_err_wraddr;
    vpss0_smmu_err_wraddr.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_wraddr.u32) + layer * MMU_OFFSET));
    vpss0_smmu_err_wraddr.bits.err_s_wr_addr = err_s_wr_addr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_wraddr.u32) + layer * MMU_OFFSET), vpss0_smmu_err_wraddr.u32);
    return;
}
hi_void vpss_mmu_set_err_swr_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_wr_addr_h)
{
    u_vpss0_smmu_err_wraddr_h vpss0_smmu_err_wraddr_h;
    vpss0_smmu_err_wraddr_h.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_wraddr_h.u32) + layer * MMU_OFFSET));
    vpss0_smmu_err_wraddr_h.bits.err_s_wr_addr_h = err_s_wr_addr_h;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_wraddr_h.u32) + layer * MMU_OFFSET), vpss0_smmu_err_wraddr_h.u32);
    return;
}
hi_void vpss_mmu_get_err_swr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_s_wr_addr)
{
    *err_s_wr_addr = vpss_reg_read((&(vpss_reg->vpss0_smmu_fault_addr_wr.u32) + layer * MMU_OFFSET));
    return;
}
hi_void vpss_mmu_set_sintmask(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 mask)
{
    u_vpss0_smmu_intmask vpss0_smmu_intmask_;
    vpss0_smmu_intmask_.u32 = mask;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_.u32);
    return;
}
hi_void vpss_mmu_set_nsintmask(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 mask)
{
    u_vpss0_smmu_intmask_ns vpss0_smmu_intmask_ns;
    vpss0_smmu_intmask_ns.u32 = mask;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_ns.u32);
    return;
}

hi_void vpss_mmu_set_cb_ttbr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 cb_ttbr)
{
    u_vpss0_smmu_cb_ttbr vpss0_smmu_cb_ttbr;
    vpss0_smmu_cb_ttbr.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_cb_ttbr.u32) + layer * MMU_OFFSET));
    vpss0_smmu_cb_ttbr.bits.cb_ttbr = cb_ttbr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_cb_ttbr.u32) + layer * MMU_OFFSET), vpss0_smmu_cb_ttbr.u32);
    return;
}
hi_void vpss_mmu_set_cb_ttbr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 cb_ttbr_h)
{
    u_vpss0_smmu_cb_ttbr_h vpss0_smmu_cb_ttbr_h;
    vpss0_smmu_cb_ttbr_h.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_cb_ttbr_h.u32) + layer * MMU_OFFSET));
    vpss0_smmu_cb_ttbr_h.bits.cb_ttbr_h = cb_ttbr_h;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_cb_ttbr_h.u32) + layer * MMU_OFFSET), vpss0_smmu_cb_ttbr_h.u32);
    return;
}

hi_void vpss_mmu_set_err_ns_rd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_rd_addr)
{
    u_vpss0_smmu_err_rdaddr_ns vpss0_smmu_err_rdaddr_ns;
    vpss0_smmu_err_rdaddr_ns.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_rdaddr_ns.u32) + layer * MMU_OFFSET));
    vpss0_smmu_err_rdaddr_ns.bits.err_ns_rd_addr = err_ns_rd_addr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_rdaddr_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_err_rdaddr_ns.u32);
    return;
}
hi_void vpss_mmu_set_err_ns_rd_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_rd_addr_h)
{
    u_vpss0_smmu_err_rdaddr_h_ns para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_rdaddr_h_ns.u32) + layer * MMU_OFFSET));
    para_tmp.bits.err_ns_rd_addr_h = err_ns_rd_addr_h;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_rdaddr_h_ns.u32) + layer * MMU_OFFSET), para_tmp.u32);
    return;
}
hi_void vpss_mmu_get_err_ns_rd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_ns_rd_addr)
{
    *err_ns_rd_addr = vpss_reg_read((&(vpss_reg->vpss0_smmu_fault_addr_rd_ns.u32) + layer * MMU_OFFSET));
    return;
}

hi_void vpss_mmu_set_err_ns_wr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_wr_addr)
{
    u_vpss0_smmu_err_wraddr_ns vpss0_smmu_err_wraddr_ns;
    vpss0_smmu_err_wraddr_ns.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_wraddr_ns.u32) + layer * MMU_OFFSET));
    vpss0_smmu_err_wraddr_ns.bits.err_ns_wr_addr = err_ns_wr_addr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_wraddr_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_err_wraddr_ns.u32);
    return;
}
hi_void vpss_mmu_set_err_ns_wr_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_wr_addr_h)
{
    u_vpss0_smmu_err_wraddr_h_ns para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_err_wraddr_h_ns.u32) + layer * MMU_OFFSET));
    para_tmp.bits.err_ns_wr_addr_h = err_ns_wr_addr_h;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_err_wraddr_h_ns.u32) + layer * MMU_OFFSET), para_tmp.u32);
    return;
}
hi_void vpss_mmu_get_err_ns_wr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_ns_wr_addr)
{
    *err_ns_wr_addr = vpss_reg_read((&(vpss_reg->vpss0_smmu_fault_addr_wr_ns.u32) + layer * MMU_OFFSET));
    return;
}

hi_u32 vpss_mmu_get_ints_stat(vpss_reg_type *vpss_reg, hi_u32 layer)
{
    u_vpss0_smmu_intstat vpss0_smmu_intstat_;
    vpss0_smmu_intstat_.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_intstat_.u32) + layer * MMU_OFFSET));
    return vpss0_smmu_intstat_.u32;
}
hi_void vpss_mmu_set_ints_clr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 ints_clr)
{
    u_vpss0_smmu_intclr vpss0_smmu_intclr_;
    vpss0_smmu_intclr_.u32 = ints_clr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_intclr_.u32) + layer * MMU_OFFSET), vpss0_smmu_intclr_.u32);
    return;
}
hi_u32 vpss_mmu_get_intns_stat(vpss_reg_type *vpss_reg, hi_u32 layer)
{
    u_vpss0_smmu_intstat_ns vpss0_smmu_intstat_ns;
    vpss0_smmu_intstat_ns.u32 = vpss_reg_read((&(vpss_reg->vpss0_smmu_intstat_ns.u32) + layer * MMU_OFFSET));
    return vpss0_smmu_intstat_ns.u32;
}
hi_void vpss_mmu_set_intns_clr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 intns_clr)
{
    u_vpss0_smmu_intclr_ns vpss0_smmu_intclr_ns;
    vpss0_smmu_intclr_ns.u32 = intns_clr;
    vpss_reg_write((&(vpss_reg->vpss0_smmu_intclr_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_intclr_ns.u32);
    return;
}

hi_void vpss_vc1_set_vc1_mapc(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapc)
{
    u_vpss_vc1_ctrl vpss_vc1_ctrl;
    vpss_vc1_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset));
    vpss_vc1_ctrl.bits.vc1_mapc = vc1_mapc;
    vpss_reg_write((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset), vpss_vc1_ctrl.u32);
    return;
}

hi_void vpss_vc1_set_vc1_mapy(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapy)
{
    u_vpss_vc1_ctrl vpss_vc1_ctrl;
    vpss_vc1_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset));
    vpss_vc1_ctrl.bits.vc1_mapy = vc1_mapy;
    vpss_reg_write((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset), vpss_vc1_ctrl.u32);
    return;
}

hi_void vpss_vc1_set_vc1_mapcflg(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapcflg)
{
    u_vpss_vc1_ctrl vpss_vc1_ctrl;
    vpss_vc1_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset));
    vpss_vc1_ctrl.bits.vc1_mapcflg = vc1_mapcflg;
    vpss_reg_write((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset), vpss_vc1_ctrl.u32);
    return;
}

hi_void vpss_vc1_set_vc1_mapyflg(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapyflg)
{
    u_vpss_vc1_ctrl vpss_vc1_ctrl;
    vpss_vc1_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset));
    vpss_vc1_ctrl.bits.vc1_mapyflg = vc1_mapyflg;
    vpss_reg_write((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset), vpss_vc1_ctrl.u32);
    return;
}

hi_void vpss_vc1_set_vc1_rangedfrm(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_rangedfrm)
{
    u_vpss_vc1_ctrl vpss_vc1_ctrl;
    vpss_vc1_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset));
    vpss_vc1_ctrl.bits.vc1_rangedfrm = vc1_rangedfrm;
    vpss_reg_write((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset), vpss_vc1_ctrl.u32);
    return;
}

hi_void vpss_vc1_set_vc1_profile(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_profile)
{
    u_vpss_vc1_ctrl vpss_vc1_ctrl;
    vpss_vc1_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset));
    vpss_vc1_ctrl.bits.vc1_profile = vc1_profile;
    vpss_reg_write((&(vpss_regs->vpss_vc1_ctrl.u32) + addr_offset), vpss_vc1_ctrl.u32);
    return;
}

hi_s32 vpss_reg_set_rotate_cfg(vpss_reg_type *vpss_reg, hi_drv_vpss_rotation angle, hi_bool rotate_y)
{
    hi_u32 rotate_angle = 0;
    hi_u32 img_pro_mode = 0;

    if (angle == HI_DRV_VPSS_ROTATION_180) {
        vpss_sys_set_rotate_en(vpss_reg, 0, 0);
        return HI_SUCCESS;
    }

    switch (angle) {
        case HI_DRV_VPSS_ROTATION_90:
            rotate_angle = 0;
            break;
        case HI_DRV_VPSS_ROTATION_270:
            rotate_angle = 1;
            break;
        default:
            osal_printk("Ro Error  %d\n", angle);
            break;
    }

    if (rotate_y) {
        img_pro_mode = 1;
    } else {
        img_pro_mode = 2; /* 2:para */
    }

    vpss_sys_set_rotate_angle(vpss_reg, 0, rotate_angle);
    vpss_mac_set_img_pro_mode(vpss_reg, img_pro_mode);
    vpss_sys_set_rotate_en(vpss_reg, 0, 1);
    return HI_SUCCESS;
}
hi_bool vpss_get_rchn_class_type(vpss_mac_rchn layer)
{
    hi_bool is_solo_data = HI_FALSE;

    if (layer >= VPSS_MAC_RCHN_MAX) {
        osal_printk("VpssGetRchnClassType error enLayer %d\n", layer);
        return HI_FALSE;
    }

    if ((layer == VPSS_MAC_RCHN_ME_CF) || (layer == VPSS_MAC_RCHN_ME_REF)) {
        is_solo_data = HI_FALSE;
    } else if (layer < VPSS_MAC_RCHN_DI_RSADY) {
        is_solo_data = HI_FALSE;
    } else {
        is_solo_data = HI_TRUE;
    }

    return is_solo_data;
}

hi_s32 vpss_mac_init_rchn_cfg(vpss_mac_rchn_cfg *rchn_cfg)
{
    memset(rchn_cfg, 0, sizeof(vpss_mac_rchn_cfg));
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y = 0x0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u = 0x0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v = 0x0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y = 0x0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c = 0x0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_v = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_y = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_c = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_addr = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_addr = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_str = 0;
    rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_str = 0;
    rchn_cfg->en = HI_FALSE;
    rchn_cfg->data_type = XDP_DATA_TYPE_SP_LINEAR;
    rchn_cfg->data_fmt = XDP_PROC_FMT_SP_420;
    rchn_cfg->pkg_fmt = XDP_PKG_FMT_YUYV;
    rchn_cfg->rd_mode = XDP_RMODE_PROGRESSIVE;
    rchn_cfg->data_width = XDP_DATA_WTH_8;
    rchn_cfg->in_rect.x = 0;
    rchn_cfg->in_rect.y = 0;
    rchn_cfg->in_rect.wth = 128; /* 128:para */
    rchn_cfg->in_rect.hgt = 64; /* 64:para */
    rchn_cfg->flip_en = HI_FALSE;
    rchn_cfg->mirror_en = HI_FALSE;
    rchn_cfg->uv_inv_en = HI_FALSE;
    rchn_cfg->tunl_en = HI_FALSE;
#ifdef HI_VPSS_SMMU_SUPPORT
    rchn_cfg->mmu_bypass = HI_FALSE;
#else
    rchn_cfg->mmu_bypass = HI_TRUE;
#endif
    rchn_cfg->mute_cfg.mute_en = HI_FALSE;
    rchn_cfg->mute_cfg.mute_y = 0xff;
    rchn_cfg->mute_cfg.mute_c = 0xff;
    rchn_cfg->dcmp_cfg.cmp_type = XDP_CMP_TYPE_OFF;
    rchn_cfg->dcmp_cfg.is_raw_en = HI_FALSE;
    rchn_cfg->dcmp_cfg.is_lossy_y = HI_TRUE;
    rchn_cfg->dcmp_cfg.is_lossy_c = HI_TRUE;
    rchn_cfg->dcmp_cfg.cmp_ratio_y = VPSS_SUPPORT_CMP_RATE;
    rchn_cfg->dcmp_cfg.cmp_ratio_c = VPSS_SUPPORT_CMP_RATE;
    rchn_cfg->dcmp_cfg.cmp_cfg_mode = ICE_REG_CFG_MODE_TYP;
    rchn_cfg->set_flag = HI_FALSE;
    return HI_SUCCESS;
}

hi_s32 vpss_mac_init_wchn_cfg(vpss_mac_wchn_cfg *wchn_cfg)
{
    memset(wchn_cfg, 0, sizeof(vpss_mac_wchn_cfg));
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y = 0x0;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_u = 0x0;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_v = 0x0;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_y = 0x0;
    wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_c = 0x0;
    wchn_cfg->en = HI_FALSE;
    wchn_cfg->data_type = XDP_DATA_TYPE_SP_LINEAR;
    wchn_cfg->data_fmt = XDP_PROC_FMT_SP_420;
    wchn_cfg->rd_mode = XDP_RMODE_PROGRESSIVE;
    wchn_cfg->data_width = XDP_DATA_WTH_8;
    wchn_cfg->out_rect.x = 0;
    wchn_cfg->out_rect.y = 0;
    wchn_cfg->out_rect.wth = 1920; /* 1920:para */
    wchn_cfg->out_rect.hgt = 128; /* 128:para */
    wchn_cfg->flip_en = HI_FALSE;
    wchn_cfg->mirror_en = HI_FALSE;
    wchn_cfg->uv_inv_en = HI_FALSE;
    wchn_cfg->tunl_en = HI_FALSE;
#ifdef HI_VPSS_SMMU_SUPPORT
    wchn_cfg->mmu_bypass = HI_FALSE;
#else
    wchn_cfg->mmu_bypass = HI_TRUE;
#endif
    wchn_cfg->dither_cfg.en = HI_FALSE;
    wchn_cfg->dither_cfg.mode = VPSS_DITHER_MODE_ROUND;
    wchn_cfg->cmp_cfg.cmp_type = XDP_CMP_TYPE_OFF;
    wchn_cfg->cmp_cfg.is_raw_en = HI_FALSE;
    wchn_cfg->cmp_cfg.is_lossy_y = HI_TRUE;
    wchn_cfg->cmp_cfg.is_lossy_c = HI_TRUE;
    wchn_cfg->cmp_cfg.cmp_ratio_y = VPSS_SUPPORT_CMP_RATE;
    wchn_cfg->cmp_cfg.cmp_ratio_c = VPSS_SUPPORT_CMP_RATE;
    wchn_cfg->cmp_cfg.cmp_cfg_mode = ICE_REG_CFG_MODE_TYP;
    wchn_cfg->set_flag = HI_FALSE;
    return HI_SUCCESS;
}
hi_void get_ice_cfg(ice_frm_cfg *ice_cfg, vpss_mac_rchn_cfg *rchn_cfg)
{
    if (rchn_cfg->dcmp_cfg.cmp_type == XDP_CMP_TYPE_OFF) {
        ice_cfg->cmp_en = HI_FALSE;
    } else {
        ice_cfg->cmp_en = HI_TRUE;
    }

    ice_cfg->frm_cnt = rchn_cfg->dcmp_cfg.frm_cnt;
    ice_cfg->frame_wth = rchn_cfg->in_rect.wth;
    ice_cfg->frame_hgt = rchn_cfg->in_rect.hgt;

    if (XDP_DATA_WTH_8 == rchn_cfg->data_width) {
        ice_cfg->bit_depth = ICE_BIT_DEPTH_8;
    } else if (XDP_DATA_WTH_10 == rchn_cfg->data_width) {
        ice_cfg->bit_depth = ICE_BIT_DEPTH_10;
    } else {
        ice_cfg->bit_depth = ICE_BIT_DEPTH_8;
        osal_printk("cmp cfg error bitwidth %d\n", rchn_cfg->data_width);
    }

    ice_cfg->is_raw_en = rchn_cfg->dcmp_cfg.is_raw_en;
    ice_cfg->is_lossy_y = rchn_cfg->dcmp_cfg.is_lossy_y;
    ice_cfg->is_lossy_c = rchn_cfg->dcmp_cfg.is_lossy_c;
    ice_cfg->cmp_ratio_y = rchn_cfg->dcmp_cfg.cmp_ratio_y;
    ice_cfg->cmp_ratio_c = rchn_cfg->dcmp_cfg.cmp_ratio_c;
    ice_cfg->cmp_cfg_mode = rchn_cfg->dcmp_cfg.cmp_cfg_mode;

    if (ice_cfg->cmp_en) {
        switch (rchn_cfg->data_fmt) {
            case XDP_PROC_FMT_SP_420:
                ice_cfg->data_fmt = ICE_DATA_FMT_YUV420;
                break;
            case XDP_PROC_FMT_SP_422:
                ice_cfg->data_fmt = ICE_DATA_FMT_YUV422;
                break;
            default:
                osal_printk("cmp cfg error fmt %d\n", rchn_cfg->data_fmt);
        }

        switch (rchn_cfg->rd_mode) {
            case XDP_RMODE_PROGRESSIVE: {
                ice_cfg->frame_wth = rchn_cfg->in_rect.wth;
                ice_cfg->frame_hgt = rchn_cfg->in_rect.hgt;
                break;
            }
            case XDP_RMODE_TOP:
            case XDP_RMODE_BOTTOM:
            case XDP_RMODE_PRO_TOP:
            case XDP_RMODE_PRO_BOTTOM: {
                ice_cfg->frame_wth = rchn_cfg->in_rect.wth;
                ice_cfg->frame_hgt = rchn_cfg->in_rect.hgt / 2; /* 2:para */
                break;
            }
            default:
                osal_printk("read error mode %d\n", rchn_cfg->rd_mode);
        }
    }

    return;
}

hi_void vpss_mac_set_rchn_cf_cfg(vpss_reg_type *reg, vpss_mac_rchn layer, vpss_mac_rchn_cfg *rchn_cfg)
{
    hi_u32 real_offset = 0;
    hi_bool is_solo_data = HI_FALSE;
    ice_frm_cfg ice_cfg;
    memset(&ice_cfg, 0, sizeof(ice_frm_cfg));
    is_solo_data = vpss_get_rchn_class_type(layer);
    if (is_solo_data != HI_TRUE) {
        real_offset = (g_gu32_vpss_rchn_addr[layer] - VPSS_RCHN_IMG_START_OFFSET);
        /* Cfg */
        vpss_rchn_set_en(reg, real_offset, rchn_cfg->en);
        vpss_rchn_set_type(reg, real_offset, rchn_cfg->data_type);
        vpss_rchn_set_format(reg, real_offset, rchn_cfg->data_fmt);
        vpss_rchn_set_order(reg, real_offset, rchn_cfg->pkg_fmt);
        vpss_rchn_set_lm_rmode(reg, real_offset, rchn_cfg->rd_mode);
        vpss_rchn_set_bitw(reg, real_offset, rchn_cfg->data_width);
        vpss_rchn_set_width(reg, real_offset, rchn_cfg->in_rect.wth - 1);
        vpss_rchn_set_height(reg, real_offset, rchn_cfg->in_rect.hgt - 1);
        vpss_rchn_set_hor_offset(reg, real_offset, rchn_cfg->in_rect.x);
        vpss_rchn_set_ver_offset(reg, real_offset, rchn_cfg->in_rect.y);
        vpss_rchn_set_flip(reg, real_offset, rchn_cfg->flip_en);
        vpss_rchn_set_mirror(reg, real_offset, rchn_cfg->mirror_en); /* todo add */
        vpss_rchn_set_uv_invert(reg, real_offset, rchn_cfg->uv_inv_en);
        vpss_rchn_set_dbypass(reg, real_offset, rchn_cfg->mmu_bypass);
        vpss_rchn_set_hbypass(reg, real_offset, rchn_cfg->mmu_bypass);
        vpss_rchn_set2b_bypass(reg, real_offset, rchn_cfg->mmu_bypass);
        vpss_rchn_set_mute_en(reg, real_offset, rchn_cfg->mute_cfg.mute_en);
        vpss_rchn_set_cmute_val(reg, real_offset, rchn_cfg->mute_cfg.mute_y);
        vpss_rchn_set_ymute_val(reg, real_offset, rchn_cfg->mute_cfg.mute_c);
        /* 32:para */
        vpss_rchn_set_yaddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y >> 32) & 0xffffffff));
        /* 32:para */
        vpss_rchn_set_caddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u >> 32) & 0xffffffff));
        /* 32:para */
        vpss_rchn_set_cr_addr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v >> 32) & 0xffffffff));
        vpss_rchn_set_yaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y) & 0xffffffff));
        vpss_rchn_set_caddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u) & 0xffffffff));
        vpss_rchn_set_cr_addr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v) & 0xffffffff));
        vpss_rchn_set_ystride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y);
        vpss_rchn_set_cstride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c);

        /* todo : new add */
        if (layer == VPSS_MAC_RCHN_CF) {
            if (layer == VPSS_MAC_RCHN_ME_CF) {
                real_offset = (0xE300 - 0x6800) / 4; /* 0,300,0,6800,4:para */
            }

            /* 32:para */
            vpss_set_bvhdaddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y >> 32) & 0xffffffff));
            /* 32:para */
            vpss_set_bvhdcaddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u >> 32) & 0xffffffff));
            vpss_set_bvhdaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y) & 0xffffffff));
            vpss_set_bvhdcaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u) & 0xffffffff));
            vpss_set_lm_tile_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_y);
            vpss_set_chm_tile_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_c);
            vpss_set_hvhdaddr_h(reg, real_offset,
                                ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_addr >> 32) & 0xffffffff)); /* 32:para */
            vpss_set_hvhdcaddr_h(reg, real_offset,
                                 ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_addr >> 32) & 0xffffffff)); /* 32:para */
            vpss_set_hvhdaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_addr) & 0xffffffff));
            vpss_set_hvhdcaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_addr) & 0xffffffff));
            vpss_set_lm_head_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_str);
            vpss_set_chm_head_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_str);
        }

        real_offset = (g_gu32_vpss_rchn_addr[layer] - VPSS_RCHN_IMG_START_OFFSET);
        /* Dcmp */
        vpss_rchn_set_dcmp_mode(reg, real_offset, rchn_cfg->dcmp_cfg.cmp_type);

        /* Assertion */
        if (rchn_cfg->data_type == XDP_DATA_TYPE_PACKAGE) {
            vpss_assert_not_ret(rchn_cfg->data_fmt == XDP_PROC_FMT_SP_422);
        } else if (rchn_cfg->data_type == XDP_DATA_TYPE_PLANAR) {
            vpss_assert_not_ret((rchn_cfg->data_fmt == XDP_PROC_FMT_SP_420) ||
                            (rchn_cfg->data_fmt == XDP_PROC_FMT_SP_422) ||
                            (rchn_cfg->data_fmt == XDP_PROC_FMT_SP_400));
        }

        vpss_assert_not_ret(rchn_cfg->in_rect.wth >= 31); /* 31:min reso */
        vpss_assert_not_ret(rchn_cfg->in_rect.hgt >= 31); /* 31:min reso */

        if ((layer != VPSS_MAC_RCHN_ME_CF) && (layer != VPSS_MAC_RCHN_ME_REF)) {
            vpss_assert_not_ret((rchn_cfg->in_rect.wth % 2) == 0); /* 2:align */
            vpss_assert_not_ret((rchn_cfg->in_rect.hgt % 2) == 0); /* 2:align */
        }

        if (rchn_cfg->mirror_en == HI_TRUE) {
            vpss_assert_not_ret(rchn_cfg->in_rect.x == 0);
        }
    }

#ifdef __VPSS_UT__DEBUG__
    if (rchn_cfg->en == HI_TRUE) {
        vpss_error("[vpss_rchn] vpss_rchn                        = %d\n", layer);
        vpss_error("[vpss_rchn] bIsSoloData                      = %d\n", is_solo_data);
        vpss_error("[vpss_rchn] u32RealOffset                    = %d\n", real_offset * 4); /* 32,4:para */
        vpss_error("[vpss_rchn] pstRchnCfg->bEn                  = %d\n", rchn_cfg->en);
        vpss_error("[vpss_rchn] pstRchnCfg->enDataType           = %d\n", rchn_cfg->data_type);
        vpss_error("[vpss_rchn] pstRchnCfg->enDataFmt            = %d\n", rchn_cfg->data_fmt);
        vpss_error("[vpss_rchn] pstRchnCfg->enPkgFmt             = %d\n", rchn_cfg->pkg_fmt);
        vpss_error("[vpss_rchn] pstRchnCfg->enRdMode             = %d\n", rchn_cfg->rd_mode);
        vpss_error("[vpss_rchn] pstRchnCfg->enDataWidth          = %d\n", rchn_cfg->data_width);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32Wth      = %d\n", rchn_cfg->in_rect.wth);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32Hgt      = %d\n", rchn_cfg->in_rect.hgt);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32X        = %d\n", rchn_cfg->in_rect.x);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32Y        = %d\n", rchn_cfg->in_rect.y);
        vpss_error("[vpss_rchn] pstRchnCfg->bFlipEn              = %d\n", rchn_cfg->flip_en);
        vpss_error("[vpss_rchn] pstRchnCfg->bMirrorEn            = %d\n", rchn_cfg->mirror_en);
        vpss_error("[vpss_rchn] pstRchnCfg->bUvInvEn             = %d\n", rchn_cfg->uv_inv_en);
        vpss_error("[vpss_rchn] pstRchnCfg->bMmuBypass           = %d\n", rchn_cfg->mmu_bypass);
        vpss_error("[vpss_rchn] pstRchnCfg->bTunlEn              = %d\n", rchn_cfg->tunl_en);
        vpss_error("[vpss_rchn] pstRchnCfg->stMuteCfg.bMuteEn    = %d\n", rchn_cfg->mute_cfg.mute_en);
        vpss_error("[vpss_rchn] pstRchnCfg->stMuteCfg.u32MuteY   = %d\n", rchn_cfg->mute_cfg.mute_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stMuteCfg.u32MuteC   = %d\n", rchn_cfg->mute_cfg.mute_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u64YAddr     = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u64UAddr     = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u64VAddr     = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u32YStr      = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u32CStr      = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.enCmpType                      = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_type);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.bIsRawEn                       = %d\n",
                   rchn_cfg->dcmp_cfg.is_raw_en);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.bIsLossyY                      = %d\n",
                   rchn_cfg->dcmp_cfg.is_lossy_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.bIsLossyC                      = %d\n",
                   rchn_cfg->dcmp_cfg.is_lossy_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.u32CmpRatioY                   = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_ratio_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.u32CmpRatioC                   = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_ratio_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.enCmpCfgMode                   = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_cfg_mode);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u64YAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u64UAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u64VAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u32YStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u32CStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].str_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u64YAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u64UAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u64VAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u32YStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u32CStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].str_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u64YAddr       = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u64UAddr       = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u64VAddr       = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u32YStr        = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u32CStr        = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_c);
    }
#endif
}

hi_void vpss_mac_set_rchn_cfg(vpss_reg_type *reg, vpss_mac_rchn layer, vpss_mac_rchn_cfg *rchn_cfg)
{
    hi_u32 real_offset = 0;
    hi_bool is_solo_data = HI_FALSE;
    ice_frm_cfg ice_cfg;
    memset(&ice_cfg, 0, sizeof(ice_frm_cfg));
    is_solo_data = vpss_get_rchn_class_type(layer);
    if (is_solo_data != HI_TRUE) {
        real_offset = (g_gu32_vpss_rchn_addr[layer] - VPSS_RCHN_IMG_START_OFFSET);
        /* Cfg */
        vpss_rchn_set_en(reg, real_offset, rchn_cfg->en);
        vpss_rchn_set_type(reg, real_offset, rchn_cfg->data_type);
        vpss_rchn_set_format(reg, real_offset, rchn_cfg->data_fmt);
        vpss_rchn_set_order(reg, real_offset, rchn_cfg->pkg_fmt);
        vpss_rchn_set_lm_rmode(reg, real_offset, rchn_cfg->rd_mode);
        vpss_rchn_set_bitw(reg, real_offset, rchn_cfg->data_width);
        vpss_rchn_set_width(reg, real_offset, rchn_cfg->in_rect.wth - 1);
        vpss_rchn_set_height(reg, real_offset, rchn_cfg->in_rect.hgt - 1);
        vpss_rchn_set_hor_offset(reg, real_offset, rchn_cfg->in_rect.x);
        vpss_rchn_set_ver_offset(reg, real_offset, rchn_cfg->in_rect.y);
        vpss_rchn_set_flip(reg, real_offset, rchn_cfg->flip_en);
        vpss_rchn_set_mirror(reg, real_offset, rchn_cfg->mirror_en); /* todo add */
        vpss_rchn_set_uv_invert(reg, real_offset, rchn_cfg->uv_inv_en);
        vpss_rchn_set_tunl_en(reg, real_offset, rchn_cfg->tunl_en);
        vpss_rchn_set_dbypass(reg, real_offset, rchn_cfg->mmu_bypass);
        vpss_rchn_set_hbypass(reg, real_offset, rchn_cfg->mmu_bypass);
        vpss_rchn_set2b_bypass(reg, real_offset, rchn_cfg->mmu_bypass);
        vpss_rchn_set_mute_en(reg, real_offset, rchn_cfg->mute_cfg.mute_en);
        vpss_rchn_set_cmute_val(reg, real_offset, rchn_cfg->mute_cfg.mute_y);
        vpss_rchn_set_ymute_val(reg, real_offset, rchn_cfg->mute_cfg.mute_c);
        /* 32:para */
        vpss_rchn_set_yaddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y >> 32) & 0xffffffff));
        /* 32:para */
        vpss_rchn_set_caddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u >> 32) & 0xffffffff));
        /* 32:para */
        vpss_rchn_set_cr_addr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v >> 32) & 0xffffffff));
        vpss_rchn_set_yaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y) & 0xffffffff));
        vpss_rchn_set_caddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u) & 0xffffffff));
        vpss_rchn_set_cr_addr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v) & 0xffffffff));
        vpss_rchn_set_ystride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y);
        vpss_rchn_set_cstride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c);

        /* todo : new add */
        if ((layer == VPSS_MAC_RCHN_CF) || (layer == VPSS_MAC_RCHN_ME_CF)) {
            if (layer == VPSS_MAC_RCHN_ME_CF) {
                real_offset = (0xE300 - 0x6800) / 4; /* 0,300,0,6800,4:para */
            }

            /* 32:para */
            vpss_set_bvhdaddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y >> 32) & 0xffffffff));
            /* 32:para */
            vpss_set_bvhdcaddr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u >> 32) & 0xffffffff));
            vpss_set_bvhdaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y) & 0xffffffff));
            vpss_set_bvhdcaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u) & 0xffffffff));
            vpss_set_lm_tile_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_y);
            vpss_set_chm_tile_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_c);
            vpss_set_hvhdaddr_h(reg, real_offset,
                                ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_addr >> 32) & 0xffffffff)); /* 32:para */
            vpss_set_hvhdcaddr_h(reg, real_offset,
                                 ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_addr >> 32) & 0xffffffff)); /* 32:para */
            vpss_set_hvhdaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_addr) & 0xffffffff));
            vpss_set_hvhdcaddr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_addr) & 0xffffffff));
            vpss_set_lm_head_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].y_head_str);
            vpss_set_chm_head_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].c_head_str);
        }

        real_offset = (g_gu32_vpss_rchn_addr[layer] - VPSS_RCHN_IMG_START_OFFSET);
        /* Dcmp */
        vpss_rchn_set_dcmp_mode(reg, real_offset, rchn_cfg->dcmp_cfg.cmp_type);

        if (layer == VPSS_MAC_RCHN_CF) {
            if (rchn_cfg->dcmp_cfg.cmp_type != XDP_CMP_TYPE_OFF) {
                vpss_assert_not_ret(rchn_cfg->dcmp_cfg.cmp_type == XDP_CMP_TYPE_SEG);
                vpss_assert_not_ret(rchn_cfg->data_type == XDP_DATA_TYPE_SP_TILE);
            }
        }

        if (layer == VPSS_MAC_RCHN_NR_REF) {
            /* todo cmp */
            get_ice_cfg(&ice_cfg, rchn_cfg);
            vpss_mac_set_dcmp_frm_nr(reg, &ice_cfg);
        }

        /* Assertion */
        if (rchn_cfg->data_type == XDP_DATA_TYPE_PACKAGE) {
            vpss_assert_not_ret(rchn_cfg->data_fmt == XDP_PROC_FMT_SP_422);
        } else if (rchn_cfg->data_type == XDP_DATA_TYPE_PLANAR) {
            vpss_assert_not_ret((rchn_cfg->data_fmt == XDP_PROC_FMT_SP_420) ||
                            (rchn_cfg->data_fmt == XDP_PROC_FMT_SP_422) ||
                            (rchn_cfg->data_fmt == XDP_PROC_FMT_SP_400));
        }

        vpss_assert_not_ret(rchn_cfg->in_rect.wth >= 31); /* 31:min reso */
        vpss_assert_not_ret(rchn_cfg->in_rect.hgt >= 31); /* 31:min reso */

        if ((layer != VPSS_MAC_RCHN_ME_CF) && (layer != VPSS_MAC_RCHN_ME_REF)) {
            vpss_assert_not_ret((rchn_cfg->in_rect.wth % 2) == 0); /* 2:align */
            vpss_assert_not_ret((rchn_cfg->in_rect.hgt % 2) == 0); /* 2:align */
        }

        if (rchn_cfg->mirror_en == HI_TRUE) {
            vpss_assert_not_ret(rchn_cfg->in_rect.x == 0);
        }
    } else {
        real_offset = (g_gu32_vpss_rchn_addr[layer] - VPSS_RCHN_SOLO_START_OFFSET);
        /* Cfg reso */
        vpss_rchn_solo_set_en(reg, real_offset, rchn_cfg->en);
        vpss_rchn_solo_set_width(reg, real_offset, rchn_cfg->in_rect.wth - 1);
        vpss_rchn_solo_set_height(reg, real_offset, rchn_cfg->in_rect.hgt - 1);
        vpss_rchn_solo_set_mute_en(reg, real_offset, rchn_cfg->mute_cfg.mute_en);
        vpss_rchn_solo_set_mute_val0(reg, real_offset, rchn_cfg->mute_cfg.mute_y);
        vpss_rchn_solo_set_mute_val1(reg, real_offset, rchn_cfg->mute_cfg.mute_c);
        vpss_rchn_solo_set_tunle_en(reg, real_offset, rchn_cfg->tunl_en);
        vpss_rchn_solo_set_dbypass(reg, real_offset, rchn_cfg->mmu_bypass);
        vpss_rchn_solo_set_flip(reg, real_offset, rchn_cfg->flip_en);
        vpss_rchn_solo_set_mirror(reg, real_offset, rchn_cfg->mirror_en);
        /* 32:para */
        vpss_rchn_solo_set_addr_h(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y >> 32) & 0xffffffff));
        vpss_rchn_solo_set_addr_l(reg, real_offset, ((rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y) & 0xffffffff));
        vpss_rchn_solo_set_stride(reg, real_offset, rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y);
    }

#ifdef __VPSS_UT__DEBUG__
    if (rchn_cfg->en == HI_TRUE) {
        vpss_error("[vpss_rchn] vpss_rchn                        = %d\n", layer);
        vpss_error("[vpss_rchn] bIsSoloData                      = %d\n", is_solo_data);
        vpss_error("[vpss_rchn] u32RealOffset                    = %d\n", real_offset * 4); /* 32,4:para */
        vpss_error("[vpss_rchn] pstRchnCfg->bEn                  = %d\n", rchn_cfg->en);
        vpss_error("[vpss_rchn] pstRchnCfg->enDataType           = %d\n", rchn_cfg->data_type);
        vpss_error("[vpss_rchn] pstRchnCfg->enDataFmt            = %d\n", rchn_cfg->data_fmt);
        vpss_error("[vpss_rchn] pstRchnCfg->enPkgFmt             = %d\n", rchn_cfg->pkg_fmt);
        vpss_error("[vpss_rchn] pstRchnCfg->enRdMode             = %d\n", rchn_cfg->rd_mode);
        vpss_error("[vpss_rchn] pstRchnCfg->enDataWidth          = %d\n", rchn_cfg->data_width);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32Wth      = %d\n", rchn_cfg->in_rect.wth);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32Hgt      = %d\n", rchn_cfg->in_rect.hgt);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32X        = %d\n", rchn_cfg->in_rect.x);
        vpss_error("[vpss_rchn] pstRchnCfg->stInRect.u32Y        = %d\n", rchn_cfg->in_rect.y);
        vpss_error("[vpss_rchn] pstRchnCfg->bFlipEn              = %d\n", rchn_cfg->flip_en);
        vpss_error("[vpss_rchn] pstRchnCfg->bMirrorEn            = %d\n", rchn_cfg->mirror_en);
        vpss_error("[vpss_rchn] pstRchnCfg->bUvInvEn             = %d\n", rchn_cfg->uv_inv_en);
        vpss_error("[vpss_rchn] pstRchnCfg->bMmuBypass           = %d\n", rchn_cfg->mmu_bypass);
        vpss_error("[vpss_rchn] pstRchnCfg->bTunlEn              = %d\n", rchn_cfg->tunl_en);
        vpss_error("[vpss_rchn] pstRchnCfg->stMuteCfg.bMuteEn    = %d\n", rchn_cfg->mute_cfg.mute_en);
        vpss_error("[vpss_rchn] pstRchnCfg->stMuteCfg.u32MuteY   = %d\n", rchn_cfg->mute_cfg.mute_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stMuteCfg.u32MuteC   = %d\n", rchn_cfg->mute_cfg.mute_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u64YAddr     = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u64UAddr     = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u64VAddr     = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u32YStr      = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_DATA].u32CStr      = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_DATA].str_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.enCmpType                      = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_type);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.bIsRawEn                       = %d\n",
                   rchn_cfg->dcmp_cfg.is_raw_en);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.bIsLossyY                      = %d\n",
                   rchn_cfg->dcmp_cfg.is_lossy_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.bIsLossyC                      = %d\n",
                   rchn_cfg->dcmp_cfg.is_lossy_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.u32CmpRatioY                   = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_ratio_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.u32CmpRatioC                   = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_ratio_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stDcmpCfg.enCmpCfgMode                   = %d\n",
                   rchn_cfg->dcmp_cfg.cmp_cfg_mode);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u64YAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u64UAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u64VAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u32YStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_TOP].u32CStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_TOP].str_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u64YAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u64UAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u64VAddr = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u32YStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_HEAD_BOT].u32CStr  = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_HEAD_BOT].str_c);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u64YAddr       = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u64UAddr       = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_u);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u64VAddr       = 0x%llx\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].addr_v);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u32YStr        = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_y);
        vpss_error("[vpss_rchn] pstRchnCfg->stAddr[VPSS_RCHN_ADDR_2B].u32CStr        = %d\n",
                   rchn_cfg->addr[VPSS_RCHN_ADDR_2B].str_c);
    }
#endif
}

hi_bool vpss_get_wchn_class_type(vpss_mac_wchn layer)
{
    hi_bool is_solo_data = HI_FALSE;

    switch (layer) {
        case VPSS_MAC_WCHN_OUT0:
            is_solo_data = HI_FALSE;
            break;
#ifdef VPSS_96CV300_CS_SUPPORT
        case VPSS_MAC_WCHN_OUT1:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_OUT2:
            is_solo_data = HI_FALSE;
            break;
#endif
        case VPSS_MAC_WCHN_OUT3:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_NR_RFR:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_NR_RFRH:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_NR_RFRHV:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_CC_RFR:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_CC_RFR1:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_DI_RFR:
            is_solo_data = HI_FALSE;
            break;
        case VPSS_MAC_WCHN_NR_WMAD:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_NR_WCNT:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_CC_WCCNT:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_CC_WYCNT:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_DI_WSADY:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_DI_WSADC:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_DI_WHISM:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_RG_WPRJH:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_RG_WPRJV:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_RG_CFRGMV:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_DMCNT:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_ME_CFMV:
            is_solo_data = HI_TRUE;
            break;
        case VPSS_MAC_WCHN_ME_CFGMV:
            is_solo_data = HI_TRUE;
            break;
        default:
            is_solo_data = HI_FALSE;
    }

    return is_solo_data;
}

hi_s32 vpss_mac_set_wchn_cfg(vpss_reg_type *reg, vpss_mac_wchn layer, vpss_mac_wchn_cfg *wchn_cfg)
{
    hi_u32 real_offset = 0;
    hi_bool is_solo_data = HI_FALSE;
    ice_frm_cfg ice_cfg = { 0 };
    memset(&ice_cfg, 0, sizeof(ice_frm_cfg));
    is_solo_data = vpss_get_wchn_class_type(layer);
    if (is_solo_data == HI_FALSE) {
        real_offset = (g_gu32_vpss_wchn_addr[layer] - VPSS_WCHN_IMG_START_OFFSET);
        /* Cfg */
        vpss_wchn_set_en(reg, real_offset, wchn_cfg->en);
        vpss_wchn_set_type(reg, real_offset, wchn_cfg->data_type);
        vpss_wchn_set_flip(reg, real_offset, wchn_cfg->flip_en);
        vpss_wchn_set_mirror(reg, real_offset, wchn_cfg->mirror_en);
        vpss_wchn_set_uv_invert(reg, real_offset, wchn_cfg->uv_inv_en);
        vpss_wchn_set_dither_en(reg, real_offset, wchn_cfg->dither_cfg.en);
        vpss_wchn_set_dither_mode(reg, real_offset, wchn_cfg->dither_cfg.mode);
        vpss_wchn_set_hor_offset(reg, real_offset, wchn_cfg->out_rect.x);
        vpss_wchn_set_ver_offset(reg, real_offset, wchn_cfg->out_rect.y);
        vpss_wchn_set_tunl_en(reg, real_offset, wchn_cfg->tunl_en);
        vpss_wchn_set_dbypass(reg, real_offset, wchn_cfg->mmu_bypass);
        vpss_wchn_set_hbypass(reg, real_offset, wchn_cfg->mmu_bypass);
        vpss_wchn_set_bitw(reg, real_offset, wchn_cfg->data_width);
        /* 32:para */
        vpss_wchn_set_yaddr_h(reg, real_offset, ((wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y >> 32) & 0xffffffff));
        /* 32:para */
        vpss_wchn_set_caddr_h(reg, real_offset, ((wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_u >> 32) & 0xffffffff));
        vpss_wchn_set_yaddr_l(reg, real_offset, ((wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y) & 0xffffffff));
        vpss_wchn_set_caddr_l(reg, real_offset, ((wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_u) & 0xffffffff));
        vpss_wchn_set_ystride(reg, real_offset, wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_y);
        vpss_wchn_set_cstride(reg, real_offset, wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_c);
        /* Cmp */
        vpss_wchn_set_cmp_mode(reg, real_offset, wchn_cfg->cmp_cfg.cmp_type);

        if (wchn_cfg->cmp_cfg.cmp_type == XDP_CMP_TYPE_OFF) {
            ice_cfg.cmp_en = HI_FALSE;
        } else {
            ice_cfg.cmp_en = HI_TRUE;
        }

        ice_cfg.frm_cnt = wchn_cfg->cmp_cfg.frm_cnt;
        ice_cfg.node_cnt = wchn_cfg->cmp_cfg.node_cnt;
        ice_cfg.frame_wth = wchn_cfg->out_rect.wth;
        ice_cfg.frame_hgt = wchn_cfg->out_rect.hgt;

        if (XDP_DATA_WTH_8 == wchn_cfg->data_width) {
            ice_cfg.bit_depth = ICE_BIT_DEPTH_8;
        } else if (XDP_DATA_WTH_10 == wchn_cfg->data_width) {
            ice_cfg.bit_depth = ICE_BIT_DEPTH_10;
        } else {
            ice_cfg.bit_depth = ICE_BIT_DEPTH_8;
            vpss_error("cmp cfg error bitwidth %d, chn %d\n", wchn_cfg->data_width, layer);
        }

        ice_cfg.is_raw_en = wchn_cfg->cmp_cfg.is_raw_en;
        ice_cfg.is_lossy_y = wchn_cfg->cmp_cfg.is_lossy_y;
        ice_cfg.is_lossy_c = wchn_cfg->cmp_cfg.is_lossy_c;
        ice_cfg.cmp_ratio_y = wchn_cfg->cmp_cfg.cmp_ratio_y;
        ice_cfg.cmp_ratio_c = wchn_cfg->cmp_cfg.cmp_ratio_c;
        ice_cfg.cmp_cfg_mode = wchn_cfg->cmp_cfg.cmp_cfg_mode;

        if (ice_cfg.cmp_en) {
            switch (wchn_cfg->data_fmt) {
                case XDP_PROC_FMT_SP_420:
                    ice_cfg.data_fmt = ICE_DATA_FMT_YUV420;
                    break;
                case XDP_PROC_FMT_SP_422:
                    ice_cfg.data_fmt = ICE_DATA_FMT_YUV422;
                    break;
                default:
                    vpss_assert_ret(0);
            }

            switch (wchn_cfg->rd_mode) {
                case XDP_RMODE_PROGRESSIVE: {
                    ice_cfg.frame_wth = wchn_cfg->out_rect.wth;
                    ice_cfg.frame_hgt = wchn_cfg->out_rect.hgt;
                    break;
                }
                case XDP_RMODE_PRO_TOP:
                case XDP_RMODE_PRO_BOTTOM:
                case XDP_RMODE_TOP:
                case XDP_RMODE_BOTTOM: {
                    vpss_assert_ret((wchn_cfg->out_rect.hgt % 2) == 0); /* 2:para */
                    ice_cfg.frame_wth = wchn_cfg->out_rect.wth;
                    ice_cfg.frame_hgt = wchn_cfg->out_rect.hgt / 2; /* 2:para */
                    break;
                }
                default:
                    vpss_assert_ret(0);
            }
        }

        if (layer == VPSS_MAC_WCHN_OUT0) {
            vpss_mac_set_cmp_line_out0(reg, &ice_cfg);
        } else if (layer == VPSS_MAC_WCHN_NR_RFR) {
            vpss_mac_set_cmp_frm_nr(reg, &ice_cfg);
        }

        vpss_wchn_env_set_format(reg, real_offset, wchn_cfg->data_fmt);
        vpss_wchn_set_lm_rmode(reg, real_offset, wchn_cfg->rd_mode);
        vpss_wchn_env_set_width(reg, real_offset, wchn_cfg->out_rect.wth - 1);
        vpss_wchn_env_set_height(reg, real_offset, wchn_cfg->out_rect.hgt - 1);
        vpss_wchn_env_set_finfo_h(reg, real_offset, ((wchn_cfg->finfo >> 32) & 0xffffffff)); /* 32,0:para */
        vpss_wchn_env_set_finfo_l(reg, real_offset, ((wchn_cfg->finfo) & 0xffffffff));
    } else {
        real_offset = (g_gu32_vpss_wchn_addr[layer] - VPSS_WCHN_SOLO_START_OFFSET);
        /* Cfg */
        vpss_wchn_solo_set_en(reg, real_offset, wchn_cfg->en);
        vpss_wchn_solo_set_flip(reg, real_offset, wchn_cfg->flip_en);
        vpss_wchn_solo_set_mirror(reg, real_offset, wchn_cfg->mirror_en);
        vpss_wchn_solo_set_tunl_en(reg, real_offset, wchn_cfg->tunl_en);
        vpss_wchn_solo_set_dbypass(reg, real_offset, wchn_cfg->mmu_bypass);
        /* 32:para */
        vpss_wchn_solo_set_addr_h(reg, real_offset, ((wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y >> 32) & 0xffffffff));
        vpss_wchn_solo_set_addr_l(reg, real_offset, ((wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y) & 0xffffffff));
        vpss_wchn_solo_set_stride(reg, real_offset, wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_y);
        vpss_wchn_solo_env_set_width(reg, real_offset, wchn_cfg->out_rect.wth - 1);
        vpss_wchn_solo_env_set_height(reg, real_offset, wchn_cfg->out_rect.hgt - 1);
    }

    /* Assertion */
    vpss_assert_ret(wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_y % 16 == 0); /* 16:para */
    vpss_assert_ret(wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_c % 16 == 0); /* 16:para */

    if (wchn_cfg->dither_cfg.en) {
        vpss_assert_ret(wchn_cfg->data_width == XDP_DATA_WTH_8);
    }

    if (wchn_cfg->cmp_cfg.cmp_type == XDP_CMP_TYPE_FRM) {
        vpss_assert_ret(wchn_cfg->flip_en == HI_FALSE);
        vpss_assert_ret(wchn_cfg->mirror_en == HI_FALSE);
    }

#ifdef __VPSS_UT__DEBUG__
    if (wchn_cfg->en == HI_TRUE) {
        vpss_error("[vpss_wchn] enLayer                                          = %d\n", layer);
        vpss_error("[vpss_wchn] bIsSoloData                                      = %d\n", is_solo_data);
        vpss_error("[vpss_wchn] u32RealOffset                                    = %d\n", real_offset * 4); /* 4:para */
        vpss_error("[vpss_wchn] pstWchnCfg->bEn                                  = %d\n", wchn_cfg->en);
        vpss_error("[vpss_wchn] pstWchnCfg->enDataType                           = %d\n", wchn_cfg->data_type);
        vpss_error("[vpss_wchn] pstWchnCfg->enDataWidth                          = %d\n", wchn_cfg->data_width);
        vpss_error("[vpss_wchn] pstWchnCfg->stOutRect.u32X                       = %d\n", wchn_cfg->out_rect.x);
        vpss_error("[vpss_wchn] pstWchnCfg->stOutRect.u32Y                       = %d\n", wchn_cfg->out_rect.y);
        vpss_error("[vpss_wchn] pstWchnCfg->bFlipEn                              = %d\n", wchn_cfg->flip_en);
        vpss_error("[vpss_wchn] pstWchnCfg->bMirrorEn                            = %d\n", wchn_cfg->mirror_en);
        vpss_error("[vpss_wchn] pstWchnCfg->bUvInvEn                             = %d\n", wchn_cfg->uv_inv_en);
        vpss_error("[vpss_wchn] pstWchnCfg->bTunlEn                              = %d\n", wchn_cfg->tunl_en);
        vpss_error("[vpss_wchn] pstWchnCfg->bMmuBypass                           = %d\n", wchn_cfg->mmu_bypass);
        vpss_error("[vpss_wchn] pstWchnCfg->stDitherCfg.bEn                      = %d\n", wchn_cfg->dither_cfg.en);
        vpss_error("[vpss_wchn] pstWchnCfg->stDitherCfg.enMode                   = %d\n", wchn_cfg->dither_cfg.mode);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_DATA].u64YAddr = 0x%llx\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_y);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_DATA].u64UAddr = 0x%llx\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_u);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_DATA].u64VAddr = 0x%llx\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].addr_v);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_DATA].u32YStr  = %d\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_y);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_DATA].u32CStr  = %d\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_DATA].str_c);
        vpss_error("[vpss_wchn] pstWchnCfg->stCmpCfg.enCmpType        = %d\n", wchn_cfg->cmp_cfg.cmp_type);
        vpss_error("[vpss_wchn] pstWchnCfg->stCmpCfg.bIsRawEn         = %d\n", wchn_cfg->cmp_cfg.is_raw_en);
        vpss_error("[vpss_wchn] pstWchnCfg->stCmpCfg.bIsLossyY        = %d\n", wchn_cfg->cmp_cfg.is_lossy_y);
        vpss_error("[vpss_wchn] pstWchnCfg->stCmpCfg.bIsLossyC        = %d\n", wchn_cfg->cmp_cfg.is_lossy_c);
        vpss_error("[vpss_wchn] pstWchnCfg->stCmpCfg.u32CmpRatioY     = %d\n", wchn_cfg->cmp_cfg.cmp_ratio_y);
        vpss_error("[vpss_wchn] pstWchnCfg->stCmpCfg.u32CmpRatioC     = %d\n", wchn_cfg->cmp_cfg.cmp_ratio_c);
        vpss_error("[vpss_wchn] pstWchnCfg->stCmpCfg.enCmpCfgMode     = %d\n", wchn_cfg->cmp_cfg.cmp_cfg_mode);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_HEAD].u64YAddr = 0x%llx\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_HEAD].addr_y);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_HEAD].u64UAddr = 0x%llx\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_HEAD].addr_u);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_HEAD].u64VAddr = 0x%llx\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_HEAD].addr_v);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_HEAD].u32YStr  = %d\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_HEAD].str_y);
        vpss_error("[vpss_wchn] pstWchnCfg->stAddr[VPSS_WCHN_ADDR_HEAD].u32CStr  = %d\n",
                   wchn_cfg->addr[VPSS_WCHN_ADDR_HEAD].str_c);
        vpss_error("[env_wchn]  pstWchnCfg->stOutRect.u32Wth                     = %d\n", wchn_cfg->out_rect.wth);
        vpss_error("[env_wchn]  pstWchnCfg->stOutRect.u32Hgt                     = %d\n", wchn_cfg->out_rect.hgt);
        vpss_error("[env_wchn]  pstWchnCfg->enDataFmt                            = %d\n", wchn_cfg->data_fmt);
        vpss_error("[env_wchn]  pstWchnCfg->enRdMode                             = %d\n", wchn_cfg->rd_mode);
        vpss_error("[env_wchn]  pstWchnCfg->u64Finfo                             = %lld\n", wchn_cfg->finfo);
    }
#endif
    return HI_SUCCESS;
}

hi_void vpss_mac_set_reg_load(vpss_reg_type *reg)
{
    /* REG LOAD */
    vpss_sys_set_slow(reg, 0x0, 0x1);
    vpss_sys_set_mask0(reg, 0x0, 0x07ffffff); /* 0x5000~0x6FFC */
    vpss_sys_set_mask1(reg, 0x0, 0x3eff000e); /* 0x7000~0x8FFC */
    vpss_sys_set_mask2(reg, 0x0, 0x003f001f); /* 0x9000~0xAFFC */
    vpss_sys_set_mask3(reg, 0x0, 0xffffffff); /* 0xB000~0xCFFC */
    vpss_sys_set_mask4(reg, 0x0, 0x0000f00f); /* 0xD000~0xDFFC */
    vpss_sys_set_mask5(reg, 0x0, 0x0);
    vpss_sys_set_mask6(reg, 0x0, 0x0);
    vpss_sys_set_mask7(reg, 0x0, 0x0);
    /* REG LOAD 1 */
    vpss_sys_set_slow(reg, 0x20, 0x1);
    vpss_sys_set_mask0(reg, 0x20, 0xffff);
    vpss_sys_set_mask1(reg, 0x20, 0x0);
    vpss_sys_set_mask2(reg, 0x20, 0x0);
    vpss_sys_set_mask3(reg, 0x20, 0x0);
    vpss_sys_set_mask4(reg, 0x20, 0x0);
    vpss_sys_set_mask5(reg, 0x20, 0x0);
    vpss_sys_set_mask6(reg, 0x20, 0x0);
    vpss_sys_set_mask7(reg, 0x20, 0x0);
}

hi_s32 vpss_crg_set_clock_en(hi_u32 vpss_ip, hi_bool clk_en)
{
#ifndef __VPSS_UT__
    U_PERI_CRG335 peri_crg335;
    U_PERI_CRG180 peri_crg180;
#ifdef HI_FPGA
    return HI_SUCCESS;
#endif
    /* crg addr 0x00a0053c */
    g_reg_crg = hi_drv_sys_get_crg_reg_ptr();

    switch (vpss_ip) {
        case 0: /* 0:ip0 */
            /* clk */
            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss0_ppc_cken = clk_en;
            peri_crg335.bits.vpss0_ppc_div2_cken = clk_en;
            peri_crg335.bits.vpss0_cfg_cken = clk_en;
            peri_crg335.bits.vpss0_apb_cken = clk_en;
            peri_crg335.bits.vpss0_axi_cken = clk_en;
            peri_crg335.bits.vpss0_srst_req = clk_en;
            peri_crg335.bits.vpss0_cksel = 0; /* 0:648M; 1:710M; 2:600M; 3:24M */
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;
            /* smmu, power up/down */
            peri_crg180.u32 = g_reg_crg->PERI_CRG180.u32;
            peri_crg180.bits.vpss0_smmu_cken = clk_en;
            peri_crg180.bits.vpss0_smmu_srst_req = 1;
            g_reg_crg->PERI_CRG180.u32 = peri_crg180.u32;
            osal_udelay(1);
            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss0_srst_req = 0;
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;
            osal_udelay(1);
            peri_crg180.u32 = g_reg_crg->PERI_CRG180.u32;
            peri_crg180.bits.vpss0_smmu_srst_req = 0;
            g_reg_crg->PERI_CRG180.u32 = peri_crg180.u32;
            osal_udelay(1);
            break;
        case 1: /* 1:ip1 */
            /* clk */
            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss1_ppc_cken = clk_en;
            peri_crg335.bits.vpss1_ppc_div2_cken = clk_en;
            peri_crg335.bits.vpss1_cfg_cken = clk_en;
            peri_crg335.bits.vpss1_apb_cken = clk_en;
            peri_crg335.bits.vpss1_axi_cken = clk_en;
            peri_crg335.bits.vpss1_srst_req = 1;
            peri_crg335.bits.vpss1_cksel = 0; /* 0:648M; 1:710M; 2:600M; 3:24M */
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;
            /* smmu, power up/down */
            peri_crg180.u32 = g_reg_crg->PERI_CRG180.u32;
            peri_crg180.bits.vpss1_smmu_cken = clk_en;
            peri_crg180.bits.vpss1_smmu_srst_req = 1;
            g_reg_crg->PERI_CRG180.u32 = peri_crg180.u32;
            osal_udelay(1);
            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss1_srst_req = 0;
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;
            osal_udelay(1);
            peri_crg180.u32 = g_reg_crg->PERI_CRG180.u32;
            peri_crg180.bits.vpss1_smmu_srst_req = 0;
            g_reg_crg->PERI_CRG180.u32 = peri_crg180.u32;
            osal_udelay(1);
            break;
        default:
            osal_printk("This chip can't support vpss id (%d)\n", vpss_ip);
    }
#endif

    return HI_SUCCESS;
}

static hi_void vpss_soft_reset_wait_finish(vpss_reg_type *reg)
{
    hi_u32 vpss_rst_sta = 0;
    hi_u32 wait_time = 0;

    while (vpss_rst_sta != 1) {
        osal_udelay(1);      /* delay 1 ms */
        wait_time += 1; /* wait time add 1 */

        vpss_rst_sta = vpss_reg_read((&(reg->vpss_start.u32) + 0x10000 / 4)); /* 0x10000:reg addr */ /* 4:32bit */
        /* vpss_rst_sta:1,reset finish flag */
        if ((vpss_rst_sta == 1) || (wait_time > VPSS_SOFTRESET_WAIT_MAX_TIME_US)) { /* reset finished */
            if (wait_time > VPSS_SOFTRESET_WAIT_MAX_TIME_US) {
                vpss_error("soft reset waittime us=%d\n", wait_time);
            }
            break;
        }
    }

    return;
}

hi_void vpss_soft_reset(vpss_ip ip, vpss_reg_type *reg, hi_bool rst_en)
{
#ifndef __VPSS_UT__
    U_PERI_CRG335 peri_crg335;
#ifdef HI_FPGA
    return;
#endif
    g_reg_crg = hi_drv_sys_get_crg_reg_ptr();

    switch (ip) {
        case VPSS_IP_0:
            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss0_srst_req = rst_en;
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;

            vpss_soft_reset_wait_finish(reg);

            osal_udelay(1);
            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss0_srst_req = 0;
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;
            osal_udelay(1);
            break;
        case VPSS_IP_1:
            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss1_srst_req = rst_en;
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;

            vpss_soft_reset_wait_finish(reg);

            peri_crg335.u32 = g_reg_crg->PERI_CRG335.u32;
            peri_crg335.bits.vpss1_srst_req = 0;
            g_reg_crg->PERI_CRG335.u32 = peri_crg335.u32;
            osal_udelay(1);
            vpss_error("This chip can't support vpss id (%d)\n", ip);
            break;
        default:
            osal_printk("This chip can't support vpss id (%d)\n", ip);
    }
#endif
    return;
}

hi_void vpss_rchn_set_ctop_head_addr_h(vpss_reg_type *reg, hi_u32 offset, hi_u32 top_head_addr_h)
{
    vpss_reg_write((&(reg->vpss_cf_vid_head_caddr_h.u32)), top_head_addr_h);
    return;
}

hi_void vpss_rchn_set_ytop_head_addr_l(vpss_reg_type *reg, hi_u32 offset, hi_u32 y_top_head_addr_l)
{
    vpss_reg_write((&(reg->vpss_cf_vid_head_addr_l.u32)), y_top_head_addr_l);
    return;
}

hi_void vpss_mmu_set_stag_rd_ctrl(vpss_reg_type *reg, hi_u32 layer, hi_u32 enable)
{
    u_vpss0_smmu_stag_rd_ctrl vpss0_smmu_stag_rd_ctrl;
    vpss0_smmu_stag_rd_ctrl.u32 = vpss_reg_read((&(reg->vpss0_smmu_stag_rd_ctrl.u32) + layer * MMU_OFFSET));
    vpss0_smmu_stag_rd_ctrl.bits.scmd_tag_rd_en = enable;
    vpss_reg_write((&(reg->vpss0_smmu_stag_rd_ctrl.u32) + layer * MMU_OFFSET), vpss0_smmu_stag_rd_ctrl.u32);
    return;
}

hi_void vpss_sys_set_vhd0_lba_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_height)
{
    u_vpss_vhd0_lba_dsize vpss_vhd0_lba_dsize;

    vpss_vhd0_lba_dsize.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_lba_dsize.u32) + addr_offset));
    vpss_vhd0_lba_dsize.bits.vhd0_lba_height = vhd0_lba_height;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_lba_dsize.u32) + addr_offset), vpss_vhd0_lba_dsize.u32);
    return;
}

hi_void vpss_mmu_set_ints_tlbinvalid_rd_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 ints_tlbinvalid_rd_msk)
{
    u_vpss0_smmu_intmask vpss0_smmu_intmask_;
    vpss0_smmu_intmask_.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_.bits.ints_tlbinvalid_rd_msk = ints_tlbinvalid_rd_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_.u32);
    return;
}

hi_u32 vpss_hal_get_get_int_status(vpss_reg_type *vpss_regs)
{
    return vpss_regs->vpss_intsta.u32;
}

/* add by sdk */
hi_void vpss_rchn_set_ctop_head_addr_l(vpss_reg_type *reg, hi_u32 offset, hi_u32 top_head_addr_l)
{
    vpss_reg_write((&(reg->vpss_cf_vid_head_caddr_l.u32)), top_head_addr_l);
    return;
}

hi_void vpss_rchn_set_y2b_stride(vpss_reg_type *reg, hi_u32 offset, hi_u32 y_2b_stride)
{
    u_vpss_cf_vid_2_bit_stride vpss_cf_vid_2_bit_stride;
    vpss_cf_vid_2_bit_stride.u32 = vpss_reg_read((&(reg->vpss_cf_vid_2_bit_stride.u32)));
    vpss_cf_vid_2_bit_stride.bits.lm_tile_stride = y_2b_stride;
    vpss_reg_write((&(reg->vpss_cf_vid_2_bit_stride.u32)), vpss_cf_vid_2_bit_stride.u32);
    return;
}

hi_void vpss_mmu_set_ints_tlbmiss_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 ints_tlbmiss_msk)
{
    u_vpss0_smmu_intmask vpss0_smmu_intmask_;
    vpss0_smmu_intmask_.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_.bits.ints_tlbmiss_msk = ints_tlbmiss_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_.u32);
    return;
}

hi_void vpss_master_set_split_mode(vpss_reg_type *reg, hi_u32 split_mode)
{
    u_vpss_mst_ctrl vpss_mst_ctrl;
    vpss_mst_ctrl.u32 = vpss_reg_read((&(reg->vpss_mst_ctrl.u32)));
    vpss_mst_ctrl.bits.split_mode = split_mode;
    vpss_reg_write((&(reg->vpss_mst_ctrl.u32)), vpss_mst_ctrl.u32);
    return;
}

hi_void vpss_mmu_set_intns_tlbmiss_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_tlbmiss_msk)
{
    u_vpss0_smmu_intmask_ns vpss0_smmu_intmask_ns;
    vpss0_smmu_intmask_ns.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_ns.bits.intns_tlbmiss_msk = intns_tlbmiss_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_ns.u32);
    return;
}

hi_void vpss_sys_set_four_pixel_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 four_pixel_en)
{
    u_vpss_ctrl1 vpss_ctrl1;

    vpss_ctrl1.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl1.u32) + addr_offset));
    vpss_ctrl1.bits.four_pixel_en = four_pixel_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl1.u32) + addr_offset), vpss_ctrl1.u32);
    return;
}

hi_void vpss_sys_set_vhd0_lba_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lbox_en)
{
    u_vpss_ctrl1 vpss_ctrl1;

    vpss_ctrl1.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl1.u32) + addr_offset));
    vpss_ctrl1.bits.lbox_en = lbox_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl1.u32) + addr_offset), vpss_ctrl1.u32);
    return;
}


hi_void vpss_sys_setdbm_430_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 dbm_430_en)
{
    u_vpss_ctrl1 vpss_ctrl1;

    vpss_ctrl1.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl1.u32) + addr_offset));
    vpss_ctrl1.bits.dbm_430_en = dbm_430_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl1.u32) + addr_offset), vpss_ctrl1.u32);

    return ;
}

hi_void vpss_sys_sethdr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hdr_en)
{
    u_vpss_ctrl1 vpss_ctrl1;

    vpss_ctrl1.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl1.u32) + addr_offset));
    vpss_ctrl1.bits.hdr_en = hdr_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl1.u32) + addr_offset), vpss_ctrl1.u32);

    return ;
}

hi_void vpss_sys_set_vhd0_crop_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 crop_en)
{
    u_vpss_ctrl1 vpss_ctrl1;

    vpss_ctrl1.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl1.u32) + addr_offset));
    vpss_ctrl1.bits.crop_en = crop_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl1.u32) + addr_offset), vpss_ctrl1.u32);
    return;
}

hi_void vpss_sys_set_out_pro_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out_pro_en)
{
    return;
}

hi_void vpss_sys_set_vhd0_crop_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_height)
{
    u_vpss_vhd0_crop_size vpss_vhd0_crop_size;

    vpss_vhd0_crop_size.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_crop_size.u32) + addr_offset));
    vpss_vhd0_crop_size.bits.vhd0_crop_height = vhd0_crop_height;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_crop_size.u32) + addr_offset), vpss_vhd0_crop_size.u32);
    return;
}

hi_void vpss_rchn_set_lmhead_stride(vpss_reg_type *reg, hi_u32 offset, hi_u32 lm_head_stride)
{
    u_vpss_cf_vid_head_stride vpss_cf_vid_head_stride;
    vpss_cf_vid_head_stride.u32 = vpss_reg_read((&(reg->vpss_cf_vid_head_stride.u32)));
    vpss_cf_vid_head_stride.bits.lm_head_stride = lm_head_stride;
    vpss_reg_write((&(reg->vpss_cf_vid_head_stride.u32)), vpss_cf_vid_head_stride.u32);
    return;
}

hi_void vpss_rchn_set_chmhead_stride(vpss_reg_type *reg, hi_u32 offset, hi_u32 chm_head_stride)
{
    u_vpss_cf_vid_head_stride vpss_cf_vid_head_stride;
    vpss_cf_vid_head_stride.u32 = vpss_reg_read((&(reg->vpss_cf_vid_head_stride.u32)));
    vpss_cf_vid_head_stride.bits.chm_head_stride = chm_head_stride;
    vpss_reg_write((&(reg->vpss_cf_vid_head_stride.u32)), vpss_cf_vid_head_stride.u32);
    return;
}

hi_void vpss_rchn_set_cf_vid_default_cfg(vpss_reg_type *reg)
{
    u_vpss_cf_vid_read_ctrl vpss_cf_vid_read_ctrl;
    u_vpss_cf_vid_mac_ctrl vpss_cf_vid_mac_ctrl;
    u_vpss_cf_vid_mute_bk vpss_cf_vid_mute_bk;
    u_vpss_cf_vid_smmu_bypass vpss_cf_vid_smmu_bypass;
    u_vpss_cf_vid_tunl_ctrl vpss_cf_vid_tunl_ctrl;
    vpss_cf_vid_read_ctrl.u32 = 0x00120000;   /* 0x00120000 : reg default para */
    vpss_cf_vid_mac_ctrl.u32 = 0x80000000;    /* 0x80000000 : reg default para */
    vpss_cf_vid_mute_bk.u32 = 0x00080200;     /* 0x00080200 : reg default para */
    vpss_cf_vid_smmu_bypass.u32 = 0x0000000F; /* 0x0000000F : reg default para */
    vpss_cf_vid_tunl_ctrl.u32 = 0x00010000;   /* 0x00010000 : reg default para */
    vpss_reg_write((&(reg->vpss_cf_vid_read_ctrl.u32)), vpss_cf_vid_read_ctrl.u32);
    vpss_reg_write((&(reg->vpss_cf_vid_mac_ctrl.u32)), vpss_cf_vid_mac_ctrl.u32);
    vpss_reg_write((&(reg->vpss_cf_vid_mute_bk.u32)), vpss_cf_vid_mute_bk.u32);
    vpss_reg_write((&(reg->vpss_cf_vid_smmu_bypass.u32)), vpss_cf_vid_smmu_bypass.u32);
    vpss_reg_write((&(reg->vpss_cf_vid_tunl_ctrl.u32)), vpss_cf_vid_tunl_ctrl.u32);
    return;
}

hi_void vpss_rchn_set_ytop_head_addr_h(vpss_reg_type *reg, hi_u32 offset, hi_u32 y_top_head_addr_h)
{
    vpss_reg_write((&(reg->vpss_cf_vid_head_addr_h.u32)), y_top_head_addr_h);
    return;
}

hi_void vpss_sys_set_four_pix_en(vpss_reg_type *vpss_regs, hi_u32 four_pix_en)
{
    return;
}

hi_void vpss_mmu_set_ints_tlbinvalid_wr_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 ints_tlbinvalid_wr_msk)
{
    u_vpss0_smmu_intmask vpss0_smmu_intmask_;
    vpss0_smmu_intmask_.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_.bits.ints_tlbinvalid_wr_msk = ints_tlbinvalid_wr_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_.u32);
    return;
}

hi_void vpss_mmu_set_intns_tlbinvalid_wr_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_tlbinvalid_wr_msk)
{
    u_vpss0_smmu_intmask_ns vpss0_smmu_intmask_ns;
    vpss0_smmu_intmask_ns.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_ns.bits.intns_tlbinvalid_wr_msk = intns_tlbinvalid_wr_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_ns.u32);
    return;
}

hi_void vpss_sys_set_vhd0_crop_y(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_y)
{
    u_vpss_vhd0_crop_pos vpss_vhd0_crop_pos;

    vpss_vhd0_crop_pos.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_crop_pos.u32) + addr_offset));
    vpss_vhd0_crop_pos.bits.vhd0_crop_y = vhd0_crop_y;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_crop_pos.u32) + addr_offset), vpss_vhd0_crop_pos.u32);
    return;
}

hi_void vpss_rchn_set_yhead_stride(vpss_reg_type *reg, hi_u32 offset, hi_u32 head_stride)
{
    u_vpss_base_rd_img_stride vpss_base_rd_img_stride;
    vpss_base_rd_img_stride.u32 = vpss_reg_read((&(reg->vpss_cf_vid_head_stride.u32)));
    vpss_base_rd_img_stride.bits.stride_y = head_stride;
    vpss_reg_write((&(reg->vpss_cf_vid_head_stride.u32)), vpss_base_rd_img_stride.u32);
    return;
}

hi_void vpss_rchn_set_chead_stride(vpss_reg_type *reg, hi_u32 offset, hi_u32 head_stride)
{
    u_vpss_base_rd_img_stride vpss_base_rd_img_stride;
    vpss_base_rd_img_stride.u32 = vpss_reg_read((&(reg->vpss_cf_vid_head_stride.u32)));
    vpss_base_rd_img_stride.bits.stride_c = head_stride;
    vpss_reg_write((&(reg->vpss_cf_vid_head_stride.u32)), vpss_base_rd_img_stride.u32);
    return;
}

hi_void vpss_sys_set_bfield_first(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 bfield_first)
{
#ifdef DPT
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.bfield_first = bfield_first;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
#endif
    return;
}

hi_void vpss_sys_set_vhd0_crop_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_width)
{
    u_vpss_vhd0_crop_size vpss_vhd0_crop_size;

    vpss_vhd0_crop_size.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_crop_size.u32) + addr_offset));
    vpss_vhd0_crop_size.bits.vhd0_crop_width = vhd0_crop_width;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_crop_size.u32) + addr_offset), vpss_vhd0_crop_size.u32);
    return;
}

hi_void vpss_sys_set_hdr_in(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hdr_in)
{
#ifdef DPT
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.hdr_in = hdr_in;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
#endif
    return;
}

hi_void vpss_sys_set_mad_vfir_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mad_vfir_en)
{
#ifdef DPT
    u_vpss_ctrl vpss_ctrl;
    vpss_ctrl.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl.u32) + addr_offset));
    vpss_ctrl.bits.mad_vfir_en = mad_vfir_en;
    vpss_reg_write((&(vpss_regs->vpss_ctrl.u32) + addr_offset), vpss_ctrl.u32);
#endif
    return;
}

hi_void vpss_sys_set_start(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 start)
{
    u_vpss_start vpss_start;
    vpss_start.u32 = vpss_reg_read((&(vpss_regs->vpss_start.u32) + addr_offset));
    vpss_start.bits.start = start;
    vpss_reg_write((&(vpss_regs->vpss_start.u32) + addr_offset), vpss_start.u32);
    return;
}

hi_void vpss_sys_set_vhd0_lba_yfpos(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_yfpos)
{
    u_vpss_vhd0_lba_vfpos vpss_vhd0_lba_vfpos;

    vpss_vhd0_lba_vfpos.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_lba_vfpos.u32) + addr_offset));
    vpss_vhd0_lba_vfpos.bits.vhd0_lba_yfpos = vhd0_lba_yfpos;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_lba_vfpos.u32) + addr_offset), vpss_vhd0_lba_vfpos.u32);
    return;
}

hi_void vpss_sys_set_vhd0_crop_x(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_x)
{
    u_vpss_vhd0_crop_pos vpss_vhd0_crop_pos;
    vpss_vhd0_crop_pos.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_crop_pos.u32) + addr_offset));
    vpss_vhd0_crop_pos.bits.vhd0_crop_x = vhd0_crop_x;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_crop_pos.u32) + addr_offset), vpss_vhd0_crop_pos.u32);
    return;
}

hi_void vpss_mmu_set_intns_ptw_trans_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_ptw_trans_msk)
{
    u_vpss0_smmu_intmask_ns vpss0_smmu_intmask_ns;
    vpss0_smmu_intmask_ns.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_ns.bits.intns_ptw_trans_msk = intns_ptw_trans_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_ns.u32);
    return;
}

hi_void vpss_mmu_set_intns_tlbinvalid_rd_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_tlbinvalid_rd_msk)
{
    u_vpss0_smmu_intmask_ns vpss0_smmu_intmask_ns;
    vpss0_smmu_intmask_ns.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_ns.bits.intns_tlbinvalid_rd_msk = intns_tlbinvalid_rd_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_ns.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_ns.u32);
    return;
}

hi_void vpss_wchn_env_set_finfo_l(vpss_reg_type *reg, hi_u32 offset, hi_u32 finfo_l)
{
    u_vpss_base_wr_img_finfo_low vpss_base_wr_img_finfo_low;
    vpss_base_wr_img_finfo_low.u32 = vpss_reg_read((&(reg->vpss_out0_ctrl.u32) + 0xe + offset));
    vpss_base_wr_img_finfo_low.bits.finfo_l = finfo_l;
    vpss_reg_write((&(reg->vpss_out0_ctrl.u32) + 0xe + offset), vpss_base_wr_img_finfo_low.u32);
    return;
}

hi_void vpss_sys_set_blk_det_en(vpss_reg_type *vpss_regs, hi_u32 blk_det_en)
{
    return;
}

hi_void vpss_rchn_set_c2b_stride(vpss_reg_type *reg, hi_u32 offset, hi_u32 stride)
{
    u_vpss_cf_vid_2_bit_stride vpss_cf_vid_2_bit_stride;
    vpss_cf_vid_2_bit_stride.u32 = vpss_reg_read((&(reg->vpss_cf_vid_2_bit_stride.u32)));
    vpss_cf_vid_2_bit_stride.bits.chm_tile_stride = stride;
    vpss_reg_write((&(reg->vpss_cf_vid_2_bit_stride.u32)), vpss_cf_vid_2_bit_stride.u32);
    return;
}

hi_void vpss_sys_set_vhd0_vbk_y(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_vbk_y)
{
    u_vpss_vhd0_lba_bk vpss_vhd0_lba_bk;
    vpss_vhd0_lba_bk.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_lba_bk.u32) + addr_offset));
    vpss_vhd0_lba_bk.bits.vhd0_vbk_y = vhd0_vbk_y;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_lba_bk.u32) + addr_offset), vpss_vhd0_lba_bk.u32);
    return;
}

hi_void vpss_sys_set_vhd0_vbk_cb(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_vbk_cb)
{
    u_vpss_vhd0_lba_bk vpss_vhd0_lba_bk;
    vpss_vhd0_lba_bk.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_lba_bk.u32) + addr_offset));
    vpss_vhd0_lba_bk.bits.vhd0_vbk_cb = vhd0_vbk_cb;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_lba_bk.u32) + addr_offset), vpss_vhd0_lba_bk.u32);
    return;
}

hi_void vpss_sys_set_vhd0_vbk_cr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_vbk_cr)
{
    u_vpss_vhd0_lba_bk vpss_vhd0_lba_bk;
    vpss_vhd0_lba_bk.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_lba_bk.u32) + addr_offset));
    vpss_vhd0_lba_bk.bits.vhd0_vbk_cr = vhd0_vbk_cr;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_lba_bk.u32) + addr_offset), vpss_vhd0_lba_bk.u32);
    return;
}

hi_void vpss_rchn_set_c2b_addr_l(vpss_reg_type *reg, hi_u32 offset, hi_u32 addr_l)
{
    vpss_reg_write((&(reg->vpss_cf_vid_2_bit_caddr_l.u32)), addr_l);
    return;
}

hi_u32 vpss_hal_get_get_raw_int_status(vpss_reg_type *vpss_regs)
{
    return vpss_regs->vpss_intsta.u32;
}

hi_void vpss_rchn_set_y2b_addr_l(vpss_reg_type *reg, hi_u32 offset, hi_u32 y_2b_addr_l)
{
    vpss_reg_write((&(reg->vpss_cf_vid_2_bit_addr_l.u32)), y_2b_addr_l);
    return;
}

hi_void vpss_mmu_set_stag_wr_ctrl(vpss_reg_type *reg, hi_u32 layer, hi_u32 enable)
{
    u_vpss0_smmu_stag_wr_ctrl vpss0_smmu_stag_wr_ctrl;
    vpss0_smmu_stag_wr_ctrl.u32 = vpss_reg_read((&(reg->vpss0_smmu_stag_wr_ctrl.u32) + layer * MMU_OFFSET));
    vpss0_smmu_stag_wr_ctrl.bits.scmd_tag_wr_en = enable;
    vpss_reg_write((&(reg->vpss0_smmu_stag_wr_ctrl.u32) + layer * MMU_OFFSET), vpss0_smmu_stag_wr_ctrl.u32);
    return;
}

hi_void vpss_mmu_set_ints_ptw_trans_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 ints_ptw_trans_msk)
{
    u_vpss0_smmu_intmask vpss0_smmu_intmask_;
    vpss0_smmu_intmask_.u32 = vpss_reg_read((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET));
    vpss0_smmu_intmask_.bits.ints_ptw_trans_msk = ints_ptw_trans_msk;
    vpss_reg_write((&(reg->vpss0_smmu_intmask_.u32) + layer * MMU_OFFSET), vpss0_smmu_intmask_.u32);
    return;
}

hi_void vpss_sys_set_vhd0_lba_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_width)
{
    u_vpss_vhd0_lba_dsize vpss_vhd0_lba_dsize;

    vpss_vhd0_lba_dsize.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_lba_dsize.u32) + addr_offset));
    vpss_vhd0_lba_dsize.bits.vhd0_lba_width = vhd0_lba_width;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_lba_dsize.u32) + addr_offset), vpss_vhd0_lba_dsize.u32);
    return;
}

hi_void vpss_sys_set_vhd0_lba_xfpos(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_xfpos)
{
    u_vpss_vhd0_lba_vfpos vpss_vhd0_lba_vfpos;

    vpss_vhd0_lba_vfpos.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_lba_vfpos.u32) + addr_offset));
    vpss_vhd0_lba_vfpos.bits.vhd0_lba_xfpos = vhd0_lba_xfpos;
    vpss_reg_write((&(vpss_regs->vpss_vhd0_lba_vfpos.u32) + addr_offset), vpss_vhd0_lba_vfpos.u32);
    return;
}

hi_void vpss_set_out0y_bit_depth(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_bit_depth)
{
    u_vpss_out0_y_v3_r2_line_cmp_glb_info glb_info;
    glb_info.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    glb_info.bits.out0y_bit_depth = out0y_bit_depth;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset), glb_info.u32);
    return;
}

hi_void vpss_set_out0y_esl_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_esl_qp)
{
    u_vpss_out0_y_v3_r2_line_cmp_glb_info glb_info;
    glb_info.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    glb_info.bits.out0y_esl_qp = out0y_esl_qp;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset), glb_info.u32);
    return;
}

hi_void vpss_set_out0y_chroma_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_chroma_en)
{
    u_vpss_out0_y_v3_r2_line_cmp_glb_info glb_info;
    glb_info.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    glb_info.bits.out0y_chroma_en = out0y_chroma_en;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset), glb_info.u32);
    return;
}

hi_void vpss_set_out0y_is_lossless(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_is_lossless)
{
    u_vpss_out0_y_v3_r2_line_cmp_glb_info glb_info;
    glb_info.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    glb_info.bits.out0y_is_lossless = out0y_is_lossless;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset), glb_info.u32);
    return;
}

hi_void vpss_set_out0y_cmp_mode(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_cmp_mode)
{
    u_vpss_out0_y_v3_r2_line_cmp_glb_info glb_info;
    glb_info.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    glb_info.bits.out0y_cmp_mode = out0y_cmp_mode;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset), glb_info.u32);
    return;
}

hi_void vpss_set_out0y_ice_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_ice_en)
{
    u_vpss_out0_y_v3_r2_line_cmp_glb_info glb_info;
    glb_info.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    glb_info.bits.out0y_ice_en = out0y_ice_en;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_info.u32) + addr_offset), glb_info.u32);
    return;
}

hi_void vpss_set_out0y_frame_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_frame_height)
{
    u_vpss_out0_y_v3_r2_line_cmp_frame_size cmp_frame_size;
    cmp_frame_size.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_frame_size.u32) + addr_offset));
    cmp_frame_size.bits.out0y_frame_height = out0y_frame_height;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_frame_size.u32) + addr_offset), cmp_frame_size.u32);
    return;
}

hi_void vpss_set_out0y_frame_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_frame_width)
{
    u_vpss_out0_y_v3_r2_line_cmp_frame_size para;
    para.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_frame_size.u32) + addr_offset));
    para.bits.out0y_frame_width = out0y_frame_width;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_frame_size.u32) + addr_offset), para.u32);
    return;
}

hi_void vpss_set_out0y_min_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_min_mb_bits)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg0 para;
    para.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset));
    para.bits.out0y_min_mb_bits = out0y_min_mb_bits;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset), para.u32);
    return;
}

hi_void vpss_set_out0y_budget_mb_bits_last(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg0 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset));
    para_tmp.bits.out0y_budget_mb_bits_last = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_budget_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg0 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset));
    para_tmp.bits.out0y_budget_mb_bits = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_max_mb_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0y_max_mb_qp = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_noise_pix_num_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0y_noise_pix_num_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_smooth_status_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0y_smooth_status_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_diff_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_diff_thr)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0y_diff_thr = out0y_diff_thr;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_big_grad_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_big_grad_thr)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0y_big_grad_thr = out0y_big_grad_thr;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_qp_dec2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0y_qp_dec2_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_qp_dec1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0y_qp_dec1_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_qp_inc2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0y_qp_inc2_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_qp_inc1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0y_qp_inc1_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_buf_fullness_thr_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg3 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg3.u32) + addr_offset));
    para_tmp.bits.out0y_buf_fullness_thr_reg0 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg3.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_buf_fullness_thr_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg4 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg4.u32) + addr_offset));
    para_tmp.bits.out0y_buf_fullness_thr_reg1 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg4.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_buf_fullness_thr_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg5 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg5.u32) + addr_offset));
    para_tmp.bits.out0y_buf_fullness_thr_reg2 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg5.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_qp_rge_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_qp_rge_reg0)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg6 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg6.u32) + addr_offset));
    para_tmp.bits.out0y_qp_rge_reg0 = out0y_qp_rge_reg0;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg6.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_qp_rge_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_qp_rge_reg1)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg7 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg7.u32) + addr_offset));
    para_tmp.bits.out0y_qp_rge_reg1 = out0y_qp_rge_reg1;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg7.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_qp_rge_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_qp_rge_reg2)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg8 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg8.u32) + addr_offset));
    para_tmp.bits.out0y_qp_rge_reg2 = out0y_qp_rge_reg2;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg8.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_bits_offset_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg9 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg9.u32) + addr_offset));
    para_tmp.bits.out0y_bits_offset_reg0 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg9.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_bits_offset_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg10 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg10.u32) + addr_offset));
    para_tmp.bits.out0y_bits_offset_reg1 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg10.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_bits_offset_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg11 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg11.u32) + addr_offset));
    para_tmp.bits.out0y_bits_offset_reg2 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg11.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_est_err_gain_map(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg12 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg12.u32) + addr_offset));
    para_tmp.bits.out0y_est_err_gain_map = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg12.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_buffer_size(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_buffer_size)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg13 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset));
    para_tmp.bits.out0y_buffer_size = out0y_buffer_size;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_buffer_init_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg13 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset));
    para_tmp.bits.out0y_buffer_init_bits = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_reserve_para0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_reserve_para0)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg14 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg14.u32) + addr_offset));
    para_tmp.bits.out0y_reserve_para0 = out0y_reserve_para0;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg14.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_reserve_para1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_reserve_para1)
{
    u_vpss_out0_y_v3_r2_line_cmp_rc_cfg15 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg15.u32) + addr_offset));
    para_tmp.bits.out0y_reserve_para1 = out0y_reserve_para1;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_rc_cfg15.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_adpqp_thr0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_adpqp_thr0)
{
    u_vpss_out0_y_v3_r2_line_cmp_adpqp_thr0 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_adpqp_thr0.u32) + addr_offset));
    para_tmp.bits.out0y_adpqp_thr0 = out0y_adpqp_thr0;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_adpqp_thr0.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_adpqp_thr1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_adpqp_thr1)
{
    u_vpss_out0_y_v3_r2_line_cmp_adpqp_thr1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_adpqp_thr1.u32) + addr_offset));
    para_tmp.bits.out0y_adpqp_thr1 = out0y_adpqp_thr1;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_adpqp_thr1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_smooth_deltabits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_smth_deltabits_thr para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_smth_deltabits_thr.u32) + addr_offset));
    para_tmp.bits.out0y_smooth_deltabits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_smth_deltabits_thr.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_max_left_bits_buffer(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_y_v3_r2_line_cmp_glb_st para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_st.u32) + addr_offset));
    para_tmp.bits.out0y_max_left_bits_buffer = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_glb_st.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0y_debug_info(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_debug_info)
{
    u_vpss_out0_y_v3_r2_line_cmp_dbg_reg para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_dbg_reg.u32) + addr_offset));
    para_tmp.bits.out0y_debug_info = out0y_debug_info;
    vpss_reg_write((&(vpss_regs->vpss_out0_y_v3_r2_line_cmp_dbg_reg.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_bit_depth(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_bit_depth)
{
    u_vpss_out0_c_v3_r2_line_cmp_glb_info para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    para_tmp.bits.out0c_bit_depth = out0c_bit_depth;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_esl_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_esl_qp)
{
    u_vpss_out0_c_v3_r2_line_cmp_glb_info para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    para_tmp.bits.out0c_esl_qp = out0c_esl_qp;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_chroma_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_chroma_en)
{
    u_vpss_out0_c_v3_r2_line_cmp_glb_info para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    para_tmp.bits.out0c_chroma_en = out0c_chroma_en;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_is_lossless(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_is_lossless)
{
    u_vpss_out0_c_v3_r2_line_cmp_glb_info para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    para_tmp.bits.out0c_is_lossless = out0c_is_lossless;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_cmp_mode(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_cmp_mode)
{
    u_vpss_out0_c_v3_r2_line_cmp_glb_info para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    para_tmp.bits.out0c_cmp_mode = out0c_cmp_mode;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_ice_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_ice_en)
{
    u_vpss_out0_c_v3_r2_line_cmp_glb_info para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset));
    para_tmp.bits.out0c_ice_en = out0c_ice_en;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_info.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_frame_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_frame_height)
{
    u_vpss_out0_c_v3_r2_line_cmp_frame_size para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_frame_size.u32) + addr_offset));
    para_tmp.bits.out0c_frame_height = out0c_frame_height;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_frame_size.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_frame_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_frame_width)
{
    u_vpss_out0_c_v3_r2_line_cmp_frame_size para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_frame_size.u32) + addr_offset));
    para_tmp.bits.out0c_frame_width = out0c_frame_width;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_frame_size.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_min_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_min_mb_bits)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg0 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset));
    para_tmp.bits.out0c_min_mb_bits = out0c_min_mb_bits;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_budget_mb_bits_last(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg0 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset));
    para_tmp.bits.out0c_budget_mb_bits_last = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_budget_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg0 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset));
    para_tmp.bits.out0c_budget_mb_bits = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg0.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_max_mb_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_max_mb_qp)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0c_max_mb_qp = out0c_max_mb_qp;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_noise_pix_num_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0c_noise_pix_num_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_smooth_status_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0c_smooth_status_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_diff_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_diff_thr)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0c_diff_thr = out0c_diff_thr;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_big_grad_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_big_grad_thr)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset));
    para_tmp.bits.out0c_big_grad_thr = out0c_big_grad_thr;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_qp_dec2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0c_qp_dec2_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_qp_dec1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0c_qp_dec1_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_qp_inc2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0c_qp_inc2_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_qp_inc1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg2 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset));
    para_tmp.bits.out0c_qp_inc1_bits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg2.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_buf_fullness_thr_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg3 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg3.u32) + addr_offset));
    para_tmp.bits.out0c_buf_fullness_thr_reg0 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg3.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_buf_fullness_thr_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg4 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg4.u32) + addr_offset));
    para_tmp.bits.out0c_buf_fullness_thr_reg1 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg4.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_buf_fullness_thr_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg5 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg5.u32) + addr_offset));
    para_tmp.bits.out0c_buf_fullness_thr_reg2 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg5.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_qp_rge_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg6 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg6.u32) + addr_offset));
    para_tmp.bits.out0c_qp_rge_reg0 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg6.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_qp_rge_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg7 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg7.u32) + addr_offset));
    para_tmp.bits.out0c_qp_rge_reg1 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg7.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_qp_rge_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg8 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg8.u32) + addr_offset));
    para_tmp.bits.out0c_qp_rge_reg2 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg8.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_bits_offset_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg9 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg9.u32) + addr_offset));
    para_tmp.bits.out0c_bits_offset_reg0 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg9.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_bits_offset_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg10 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg10.u32) + addr_offset));
    para_tmp.bits.out0c_bits_offset_reg1 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg10.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_bits_offset_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg11 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg11.u32) + addr_offset));
    para_tmp.bits.out0c_bits_offset_reg2 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg11.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_est_err_gain_map(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg12 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg12.u32) + addr_offset));
    para_tmp.bits.out0c_est_err_gain_map = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg12.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_buffer_size(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg13 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset));
    para_tmp.bits.out0c_buffer_size = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_buffer_init_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg13 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset));
    para_tmp.bits.out0c_buffer_init_bits = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg13.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_reserve_para0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg14 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg14.u32) + addr_offset));
    para_tmp.bits.out0c_reserve_para0 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg14.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_reserve_para1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_rc_cfg15 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg15.u32) + addr_offset));
    para_tmp.bits.out0c_reserve_para1 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_rc_cfg15.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_adpqp_thr0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_adpqp_thr0 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_adpqp_thr0.u32) + addr_offset));
    para_tmp.bits.out0c_adpqp_thr0 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_adpqp_thr0.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_adpqp_thr1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_adpqp_thr1 para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_adpqp_thr1.u32) + addr_offset));
    para_tmp.bits.out0c_adpqp_thr1 = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_adpqp_thr1.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_smooth_deltabits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_smth_deltabits_thr para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_smth_deltabits_thr.u32) + addr_offset));
    para_tmp.bits.out0c_smooth_deltabits_thr = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_smth_deltabits_thr.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_max_left_bits_buffer(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_glb_st para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_st.u32) + addr_offset));
    para_tmp.bits.out0c_max_left_bits_buffer = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_glb_st.u32) + addr_offset), para_tmp.u32);
    return;
}

hi_void vpss_set_out0c_debug_info(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para)
{
    u_vpss_out0_c_v3_r2_line_cmp_dbg_reg para_tmp;
    para_tmp.u32 = vpss_reg_read((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_dbg_reg.u32) + addr_offset));
    para_tmp.bits.out0c_debug_info = para;
    vpss_reg_write((&(vpss_regs->vpss_out0_c_v3_r2_line_cmp_dbg_reg.u32) + addr_offset), para_tmp.u32);
    return;
}

/* ck_gt_en : ip , except zme, me */
hi_void vpss_sys_set_allip_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 ck_gt_en)
{
    u_vpss_hcti_ctrl vpss_hcti_ctrl;
    u_vpss_hipp_vcti_ctrl vpss_hipp_vcti_ctrl;
    u_vpss_tchdr_ctrl tchdr_ctrl;
    u_vpss_vhdr_cm_ctrl vhdr_cm_ctrl;
    u_vpss_db_imap_ctrl db_imap_ctrl;
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;
    u_vpss_hfir_ctrl vpss_hfir_ctrl;

    vpss_hcti_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_hcti_ctrl.u32) + addr_offset));
    vpss_hipp_vcti_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_hipp_vcti_ctrl.u32) + addr_offset));
    tchdr_ctrl.u32 = vpss_reg_read((&(vpss_reg->tchdr_ctrl.u32) + addr_offset));
    vhdr_cm_ctrl.u32 = vpss_reg_read((&(vpss_reg->vhdr_cm_ctrl.u32) + addr_offset));
    db_imap_ctrl.u32 = vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_cacm_reg0.u32 = vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    vpss_hfir_ctrl.u32 = vpss_reg_read((&(vpss_reg->vpss_hfir_ctrl.u32) + addr_offset));

    vpss_hcti_ctrl.bits.cti_ck_gt_en = ck_gt_en;
    vpss_hipp_vcti_ctrl.bits.vcti_ck_gt_en = ck_gt_en;
    tchdr_ctrl.bits.tchdr_ck_gt_en = ck_gt_en;
    vhdr_cm_ctrl.bits.vhdr_cm_ck_gt_en = ck_gt_en;
    db_imap_ctrl.bits.im_ck_gt_en = ck_gt_en;
    db_imap_cacm_reg0.bits.im_cm_ck_gt_en = ck_gt_en;
    vpss_hfir_ctrl.bits.ck_gt_en = ck_gt_en;

    vpss_reg_write((&(vpss_reg->vpss_hcti_ctrl.u32) + addr_offset), vpss_hcti_ctrl.u32);
    vpss_reg_write((&(vpss_reg->vpss_hipp_vcti_ctrl.u32) + addr_offset), vpss_hipp_vcti_ctrl.u32);
    vpss_reg_write((&(vpss_reg->tchdr_ctrl.u32) + addr_offset), tchdr_ctrl.u32);
    vpss_reg_write((&(vpss_reg->vhdr_cm_ctrl.u32) + addr_offset), vhdr_cm_ctrl.u32);
    vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);
    vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);
    vpss_reg_write((&(vpss_reg->vpss_hfir_ctrl.u32) + addr_offset), vpss_hfir_ctrl.u32);

    return;
}

hi_void vpss_ifir_set_mode(vpss_reg_type *vdp_reg, hi_u32 data, hal_ifir_mode mode)
{
    u_vpss_hfir_ctrl vpss_hfir_ctrl;

    vpss_hfir_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_hfir_ctrl.u32)));
    vpss_hfir_ctrl.bits.hfir_mode = mode;
    vpss_reg_write((&(vdp_reg->vpss_hfir_ctrl.u32)), vpss_hfir_ctrl.u32);

    return ;
}

hi_void vpss_ifir_set_mid_en(vpss_reg_type *vdp_reg, hi_u32 data, hi_u32 en)
{
    u_vpss_hfir_ctrl vpss_hfir_ctrl;

    vpss_hfir_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_hfir_ctrl.u32)));
    vpss_hfir_ctrl.bits.mid_en    = en;
    vpss_reg_write((&(vdp_reg->vpss_hfir_ctrl.u32)), vpss_hfir_ctrl.u32);
    return ;
}

hi_void vpss_ifir_set_ck_gt_en(vpss_reg_type *vdp_reg, hi_u32 data, hi_u32 en)
{
    u_vpss_hfir_ctrl vpss_hfir_ctrl;

    vpss_hfir_ctrl.u32 = vpss_reg_read((&(vdp_reg->vpss_hfir_ctrl.u32)));
    vpss_hfir_ctrl.bits.ck_gt_en  = en;
    vpss_reg_write((&(vdp_reg->vpss_hfir_ctrl.u32)), vpss_hfir_ctrl.u32);
    return ;
}

hi_void vpss_ifir_set_coef(vpss_reg_type *vdp_reg, hi_u32 data, hi_s32 *coef, hi_u32 coef_num)
{
    u_vpss_ifircoef01 vpss_ifircoef01;
    u_vpss_ifircoef23 vpss_ifircoef23;
    u_vpss_ifircoef45 vpss_ifircoef45;
    u_vpss_ifircoef67 vpss_ifircoef67;

    if (coef_num > 8) { /* 8:max num */
        vpss_error("set max num is %d \n", coef_num);
        return;
    }

    vpss_ifircoef01.u32 = vpss_reg_read((&(vdp_reg->vpss_ifircoef01.u32)));
    vpss_ifircoef23.u32 = vpss_reg_read((&(vdp_reg->vpss_ifircoef23.u32)));
    vpss_ifircoef45.u32 = vpss_reg_read((&(vdp_reg->vpss_ifircoef45.u32)));
    vpss_ifircoef67.u32 = vpss_reg_read((&(vdp_reg->vpss_ifircoef67.u32)));

    vpss_ifircoef01.bits.coef0 = coef[0];
    vpss_ifircoef01.bits.coef1 = coef[1];
    vpss_ifircoef23.bits.coef2 = coef[2]; /* 2:array index */
    vpss_ifircoef23.bits.coef3 = coef[3]; /* 3:array index */
    vpss_ifircoef45.bits.coef4 = coef[4]; /* 4:array index */
    vpss_ifircoef45.bits.coef5 = coef[5]; /* 5:array index */
    vpss_ifircoef67.bits.coef6 = coef[6]; /* 6:array index */
    vpss_ifircoef67.bits.coef7 = coef[7]; /* 7:array index */

    vpss_reg_write((&(vdp_reg->vpss_ifircoef01.u32)), vpss_ifircoef01.u32);
    vpss_reg_write((&(vdp_reg->vpss_ifircoef23.u32)), vpss_ifircoef23.u32);
    vpss_reg_write((&(vdp_reg->vpss_ifircoef45.u32)), vpss_ifircoef45.u32);
    vpss_reg_write((&(vdp_reg->vpss_ifircoef67.u32)), vpss_ifircoef67.u32);

    return ;
}

hi_void vpss_dfir_set422_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_ctrl vpss_dfir_ctrl;

    vpss_dfir_ctrl.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_ctrl.u32));
    vpss_dfir_ctrl.bits.dfir_422_en = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_ctrl.u32), vpss_dfir_ctrl.u32);

    return ;
}

hi_void vpss_dfir_set420_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_ctrl vpss_dfir_ctrl;

    vpss_dfir_ctrl.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_ctrl.u32));
    vpss_dfir_ctrl.bits.dfir_420_en = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_ctrl.u32), vpss_dfir_ctrl.u32);

    return ;
}

hi_void vpss_dfir_set420_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_ctrl vpss_dfir_ctrl;

    vpss_dfir_ctrl.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_ctrl.u32));
    vpss_dfir_ctrl.bits.dfir_420_mode = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_ctrl.u32), vpss_dfir_ctrl.u32);

    return ;
}

hi_void vpss_dfir_set_hfir_coef0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_coef0 vpss_dfir_coef0;

    vpss_dfir_coef0.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_coef0.u32));
    vpss_dfir_coef0.bits.hfir_coef0 = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_coef0.u32), vpss_dfir_coef0.u32);

    return ;
}

hi_void vpss_dfir_set_hfir_coef1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_coef0 vpss_dfir_coef0;

    vpss_dfir_coef0.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_coef0.u32));
    vpss_dfir_coef0.bits.hfir_coef1 = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_coef0.u32), vpss_dfir_coef0.u32);

    return ;
}

hi_void vpss_dfir_set_hfir_coef2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_coef1 vpss_dfir_coef1;

    vpss_dfir_coef1.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_coef1.u32));
    vpss_dfir_coef1.bits.hfir_coef2 = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_coef1.u32), vpss_dfir_coef1.u32);

    return ;
}

hi_void vpss_dfir_set_hfir_coef3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_coef1 vpss_dfir_coef1;

    vpss_dfir_coef1.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_coef1.u32));
    vpss_dfir_coef1.bits.hfir_coef3 = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_coef1.u32), vpss_dfir_coef1.u32);

    return ;
}

hi_void vpss_dfir_set_hfir_coef4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_coef2 vpss_dfir_coef2;

    vpss_dfir_coef2.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_coef2.u32));
    vpss_dfir_coef2.bits.hfir_coef4 = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_coef2.u32), vpss_dfir_coef2.u32);

    return ;
}

hi_void vpss_dfir_set_hfir_coef5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_coef2 vpss_dfir_coef2;

    vpss_dfir_coef2.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_coef2.u32));
    vpss_dfir_coef2.bits.hfir_coef5 = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_coef2.u32), vpss_dfir_coef2.u32);

    return ;
}

hi_void vpss_dfir_set_hfir_coef6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data)
{
    u_vpss_dfir_coef3 vpss_dfir_coef3;

    vpss_dfir_coef3.u32 = vpss_reg_read(&(vpss_reg->vpss_dfir_coef3.u32));
    vpss_dfir_coef3.bits.hfir_coef6 = data;
    vpss_reg_write(&(vpss_reg->vpss_dfir_coef3.u32), vpss_dfir_coef3.u32);

    return ;
}

