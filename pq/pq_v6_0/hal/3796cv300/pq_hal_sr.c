/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2019-09-29
 */

#include "pq_hal_comm.h"
#include "pq_hal_sr_regset.h"
#include "pq_hal_sr.h"

#define SR_DEFAULT_SCALE_MODE 0x8

static vdp_regs_type *g_vdp_sr_reg = HI_NULL;

static hi_u32 g_pq_sr8k_split_offset_reg[PQ_8K_SR_SPLIT_MAX] = {
    (0x500 / 4), /* 8k left split offset addr, 4 is pointer offset*/
    (0xA00 / 4), /* 8k right split offset addr 4 is pointer offset*/
};

static hi_void pq_hal_sr_set_dbsr_bicubic(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    hi_u32 dbsr_bicubic_phase03 = -4;
    hi_u32 dbsr_bicubic_phase02 = 54;
    hi_u32 dbsr_bicubic_phase01 = 16;
    hi_u32 dbsr_bicubic_phase00 = -2;
    hi_u32 dbsr_bicubic_phase13 = -1;
    hi_u32 dbsr_bicubic_phase12 = 13;
    hi_u32 dbsr_bicubic_phase11 = 57;
    hi_u32 dbsr_bicubic_phase10 = -5;

    pq_reg_sr_setdbsrbicubicphase03(vdp_reg, offset, dbsr_bicubic_phase03);
    pq_reg_sr_setdbsrbicubicphase02(vdp_reg, offset, dbsr_bicubic_phase02);
    pq_reg_sr_setdbsrbicubicphase01(vdp_reg, offset, dbsr_bicubic_phase01);
    pq_reg_sr_setdbsrbicubicphase00(vdp_reg, offset, dbsr_bicubic_phase00);
    pq_reg_sr_setdbsrbicubicphase13(vdp_reg, offset, dbsr_bicubic_phase13);
    pq_reg_sr_setdbsrbicubicphase12(vdp_reg, offset, dbsr_bicubic_phase12);
    pq_reg_sr_setdbsrbicubicphase11(vdp_reg, offset, dbsr_bicubic_phase11);
    pq_reg_sr_setdbsrbicubicphase10(vdp_reg, offset, dbsr_bicubic_phase10);
    return;
}

static hi_void pq_hal_sr_set_dbsr_graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 width, hi_u32 height)
{
    hi_u32 difth_graph[4] = { 8, 24, 64, 128 }; /* 4, 8, 24, 64, 128 is a number */
    hi_u32 corek_graph[3] = { 32, 16, 6 }; /* 3, 32, 16, 6 is a number */
    hi_u32 x_kk_graph[3] = { 16, 64, 192 }; /* 3, 16, 64, 192 is a number */
    hi_u32 g_kk_graph[4] = { 312, 256, 128, 64 }; /* 4, 312, 256, 128, 64 is a number */
    hi_u32 k_kk_graph[3] = { 43, 16, 24 }; /* 3, 43, 16, 24 is a number */
    hi_u32 k_graph[3] = { 24, 80, 128 }; /* 3, 24, 80, 128 is a number */
    hi_u32 w_graph = 0;

    hi_u32 c0_graph = 0;
    hi_u32 c1_graph = width;
    hi_u32 r0_graph = 0;
    hi_u32 r1_graph = height;
    hi_u32 c0_graph_split = 0;
    hi_u32 c1_graph_split = width;

    pq_reg_sr_setdifth0graph(vdp_reg, offset, difth_graph[0]);
    pq_reg_sr_setdifth1graph(vdp_reg, offset, difth_graph[1]);
    pq_reg_sr_setdifth2graph(vdp_reg, offset, difth_graph[2]); /* 2 is a number */
    pq_reg_sr_setdifth3graph(vdp_reg, offset, difth_graph[3]); /* 3 is a number */
    pq_reg_sr_setcorek1graph(vdp_reg, offset, corek_graph[0]);
    pq_reg_sr_setcorek2graph(vdp_reg, offset, corek_graph[1]);
    pq_reg_sr_setcorek3graph(vdp_reg, offset, corek_graph[2]); /* 2 is a number */
    pq_reg_sr_setx0kkgraph(vdp_reg, offset, x_kk_graph[0]);
    pq_reg_sr_setx1kkgraph(vdp_reg, offset, x_kk_graph[1]);
    pq_reg_sr_setx2kkgraph(vdp_reg, offset, x_kk_graph[2]); /* 2 is a number */
    pq_reg_sr_setg0kkgraph(vdp_reg, offset, g_kk_graph[0]);
    pq_reg_sr_setg1kkgraph(vdp_reg, offset, g_kk_graph[1]);
    pq_reg_sr_setg2kkgraph(vdp_reg, offset, g_kk_graph[2]); /* 2 is a number */
    pq_reg_sr_setg3kkgraph(vdp_reg, offset, g_kk_graph[3]); /* 3 is a number */
    pq_reg_sr_setk3kkgraph(vdp_reg, offset, k_kk_graph[2]); /* 2 is a number */
    pq_reg_sr_setk1kkgraph(vdp_reg, offset, k_kk_graph[0]);
    pq_reg_sr_setk2kkgraph(vdp_reg, offset, k_kk_graph[1]);
    pq_reg_sr_setk1graph(vdp_reg, offset, k_graph[0]);
    pq_reg_sr_setk2graph(vdp_reg, offset, k_graph[1]);
    pq_reg_sr_setk3graph(vdp_reg, offset, k_graph[2]); /* 2 is a number */
    pq_reg_sr_setwgraph(vdp_reg, offset, w_graph);

    pq_reg_sr_setc0graph(vdp_reg, offset, c0_graph);
    pq_reg_sr_setc1graph(vdp_reg, offset, c1_graph);
    pq_reg_sr_setc0graphsplit(vdp_reg, offset, c0_graph_split);
    pq_reg_sr_setc1graphsplit(vdp_reg, offset, c1_graph_split);
    pq_reg_sr_setr0graph(vdp_reg, offset, r0_graph);
    pq_reg_sr_setr1graph(vdp_reg, offset, r1_graph);
    return;
}

static hi_void pq_hal_sr_set_dbsr_mag_dbw(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    hi_u32 x1_mag_dbw_bld = 96;
    hi_u32 x0_mag_dbw_bld = 16;
    hi_u32 k1_mag_dbw_bld = 64;
    hi_u32 k0_mag_dbw_bld = 192;
    hi_u32 k2_mag_dbw_bld = 32;
    hi_u32 g2_mag_dbw_bld = 16;
    hi_u32 g1_mag_dbw_bld = 256;
    hi_u32 g0_mag_dbw_bld = 64;

    hi_u32 x1_mag_dbw = 80;
    hi_u32 x0_mag_dbw = 16;
    hi_u32 k1_mag_dbw = 64;
    hi_u32 k0_mag_dbw = 192;
    hi_u32 k2_mag_dbw = 64;
    hi_u32 g2_mag_dbw = 16;
    hi_u32 g1_mag_dbw = 256;
    hi_u32 g0_mag_dbw = 64;

    pq_reg_sr_setx1magdbwbld(vdp_reg, offset, x1_mag_dbw_bld);
    pq_reg_sr_setx0magdbwbld(vdp_reg, offset, x0_mag_dbw_bld);
    pq_reg_sr_setk1magdbwbld(vdp_reg, offset, k1_mag_dbw_bld);
    pq_reg_sr_setk0magdbwbld(vdp_reg, offset, k0_mag_dbw_bld);
    pq_reg_sr_setk2magdbwbld(vdp_reg, offset, k2_mag_dbw_bld);
    pq_reg_sr_setg2magdbwbld(vdp_reg, offset, g2_mag_dbw_bld);
    pq_reg_sr_setg1magdbwbld(vdp_reg, offset, g1_mag_dbw_bld);
    pq_reg_sr_setg0magdbwbld(vdp_reg, offset, g0_mag_dbw_bld);

    pq_reg_sr_setx1magdbw(vdp_reg, offset, x1_mag_dbw);
    pq_reg_sr_setx0magdbw(vdp_reg, offset, x0_mag_dbw);
    pq_reg_sr_setk1magdbw(vdp_reg, offset, k1_mag_dbw);
    pq_reg_sr_setk0magdbw(vdp_reg, offset, k0_mag_dbw);
    pq_reg_sr_setk2magdbw(vdp_reg, offset, k2_mag_dbw);
    pq_reg_sr_setg2magdbw(vdp_reg, offset, g2_mag_dbw);
    pq_reg_sr_setg1magdbw(vdp_reg, offset, g1_mag_dbw);
    pq_reg_sr_setg0magdbw(vdp_reg, offset, g0_mag_dbw);
    return;
}

static hi_void pq_hal_sr_set_dbsr_mag(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    hi_u32 g1_mag_dirw_adj = 20;
    hi_u32 k1_mag_dirw_adj = 4;
    hi_u32 g0_mag_dirw_adj = 64;
    hi_u32 x0_mag_dirw_adj = 16;
    hi_u32 g1_mag_ssw_adj = 16;
    hi_u32 k1_mag_ssw_adj = 4;
    hi_u32 g0_mag_ssw_adj = 32;
    hi_u32 x0_mag_ssw_adj = 16;
    hi_u32 g1_mag_ssw_kadj = 0;
    hi_u32 k1_mag_ssw_kadj = 8;
    hi_u32 g0_mag_ssw_kadj = 64;
    hi_u32 x0_mag_ssw_kadj = 96;

    hi_u32 k1_magk_conf = 16;
    hi_u32 g0_magk_conf = 16;
    hi_u32 x0_magk_conf = 8;
    hi_u32 k1_magw_conf = 16;
    hi_u32 g0_magw_conf = 0;
    hi_u32 x0_magw_conf = 4;

    pq_reg_sr_setg1magdirwadj(vdp_reg, offset, g1_mag_dirw_adj);
    pq_reg_sr_setk1magdirwadj(vdp_reg, offset, k1_mag_dirw_adj);
    pq_reg_sr_setg0magdirwadj(vdp_reg, offset, g0_mag_dirw_adj);
    pq_reg_sr_setx0magdirwadj(vdp_reg, offset, x0_mag_dirw_adj);

    pq_reg_sr_setg1magsswadj(vdp_reg, offset, g1_mag_ssw_adj);
    pq_reg_sr_setk1magsswadj(vdp_reg, offset, k1_mag_ssw_adj);
    pq_reg_sr_setg0magsswadj(vdp_reg, offset, g0_mag_ssw_adj);
    pq_reg_sr_setx0magsswadj(vdp_reg, offset, x0_mag_ssw_adj);
    pq_reg_sr_setg1magsswkadj(vdp_reg, offset, g1_mag_ssw_kadj);
    pq_reg_sr_setk1magsswkadj(vdp_reg, offset, k1_mag_ssw_kadj);
    pq_reg_sr_setg0magsswkadj(vdp_reg, offset, g0_mag_ssw_kadj);
    pq_reg_sr_setx0magsswkadj(vdp_reg, offset, x0_mag_ssw_kadj);

    pq_reg_sr_setk1magkconf(vdp_reg, offset, k1_magk_conf);
    pq_reg_sr_setg0magkconf(vdp_reg, offset, g0_magk_conf);
    pq_reg_sr_setx0magkconf(vdp_reg, offset, x0_magk_conf);
    pq_reg_sr_setk1magwconf(vdp_reg, offset, k1_magw_conf);
    pq_reg_sr_setg0magwconf(vdp_reg, offset, g0_magw_conf);
    pq_reg_sr_setx0magwconf(vdp_reg, offset, x0_magw_conf);

    pq_hal_sr_set_dbsr_mag_dbw(vdp_reg, offset);
    return;
}

static hi_void pq_hal_sr_set_dbsr_cur(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    hi_u32 k_dif_of = 4;
    hi_u32 x_confw = 16;
    hi_u32 k_confw = 48;
    hi_u32 x_ofw = 32;
    hi_u32 k_ofw = 16;
    hi_u32 x_pdifw = 24;
    hi_u32 k_pdifw = 64;

    pq_reg_sr_setkdifof(vdp_reg, offset, k_dif_of);
    pq_reg_sr_setxconfw(vdp_reg, offset, x_confw);
    pq_reg_sr_setkconfw(vdp_reg, offset, k_confw);
    pq_reg_sr_setxofw(vdp_reg, offset, x_ofw);
    pq_reg_sr_setkofw(vdp_reg, offset, k_ofw);
    pq_reg_sr_setxpdifw(vdp_reg, offset, x_pdifw);
    pq_reg_sr_setkpdifw(vdp_reg, offset, k_pdifw);
    return;
}

static hi_void pq_hal_sr_set_dbsr_wgt(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    hi_u32 k_ssw_dirw_adj = 16;
    hi_u32 k_ssw_ssw_adj = 32;
    hi_u32 x_dbw_ssw_adj = 31;

    hi_u32 k_bp_dbw_bld = 68;
    hi_u32 x_bp_dbw_bld = 8;

    hi_u32 k_minw_dbw_bld = 6;
    hi_u32 k_idbw_ssw_bld = 18;
    hi_u32 x_idbw_ssw_bld = 3;

    pq_reg_sr_setksswdirwadj(vdp_reg, offset, k_ssw_dirw_adj);
    pq_reg_sr_setksswsswadj(vdp_reg, offset, k_ssw_ssw_adj);
    pq_reg_sr_setxdbwsswadj(vdp_reg, offset, x_dbw_ssw_adj);

    pq_reg_sr_setkbpdbwbld(vdp_reg, offset, k_bp_dbw_bld);
    pq_reg_sr_setxbpdbwbld(vdp_reg, offset, x_bp_dbw_bld);

    pq_reg_sr_setkminwdbwbld(vdp_reg, offset, k_minw_dbw_bld);
    pq_reg_sr_setkidbwsswbld(vdp_reg, offset, k_idbw_ssw_bld);
    pq_reg_sr_setxidbwsswbld(vdp_reg, offset, x_idbw_ssw_bld);
    return;
}

static hi_void pq_hal_sr_set_dbsr_bld(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    hi_u32 k_ssw_bld = 16;
    hi_u32 b_ssw_bld = 0;
    hi_u32 k_dbw_bld = 16;
    hi_u32 b_dbw_bld = 0;

    pq_reg_sr_setksswbld(vdp_reg, offset, k_ssw_bld);
    pq_reg_sr_setbsswbld(vdp_reg, offset, b_ssw_bld);
    pq_reg_sr_setkdbwbld(vdp_reg, offset, k_dbw_bld);
    pq_reg_sr_setbdbwbld(vdp_reg, offset, b_dbw_bld);
    return;
}

static hi_void pq_hal_sr_set_dbsr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 width, hi_u32 height)
{
    hi_u32 graph_soft_en = 0;
    hi_u32 para_mode = 0;

    hi_u32 fix_iw_en_dbsr = 0;
    hi_u32 fix_iw_dbsr = 256;

    hi_u32 wsum_lmt = 2048;
    hi_u32 core_kmag_pdif = 8;
    hi_u32 core_mad_pdif = 1;

    hi_u32 lmt_conf_lst = 512;
    hi_u32 k_lmt_conf0 = 8;

    hi_u32 k_lpf_bp_db = 8;
    hi_u32 coring_bp_db = 1;

    pq_reg_sr_setgraphsoften(vdp_reg, offset, graph_soft_en);
    pq_reg_sr_setparamode(vdp_reg, offset, para_mode);

    pq_reg_sr_setfixiwendbsr(vdp_reg, offset, fix_iw_en_dbsr);
    pq_reg_sr_setfixiwdbsr(vdp_reg, offset, fix_iw_dbsr);

    pq_reg_sr_setwsumlmt(vdp_reg, offset, wsum_lmt);

    pq_reg_sr_setcorekmagpdif(vdp_reg, offset, core_kmag_pdif);
    pq_reg_sr_setcoremadpdif(vdp_reg, offset, core_mad_pdif);

    pq_reg_sr_setlmtconflst(vdp_reg, offset, lmt_conf_lst);
    pq_reg_sr_setklmtconf0(vdp_reg, offset, k_lmt_conf0);

    pq_reg_sr_setklpfbpdb(vdp_reg, offset, k_lpf_bp_db);
    pq_reg_sr_setcoringbpdb(vdp_reg, offset, coring_bp_db);

    pq_hal_sr_set_dbsr_bicubic(vdp_reg, offset);
    pq_hal_sr_set_dbsr_graph(vdp_reg, offset, width, height);
    pq_hal_sr_set_dbsr_mag(vdp_reg, offset);
    pq_hal_sr_set_dbsr_cur(vdp_reg, offset);
    pq_hal_sr_set_dbsr_wgt(vdp_reg, offset);
    pq_hal_sr_set_dbsr_bld(vdp_reg, offset);
    return;
}

static hi_void pq_hal_sr_set_demo(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    hi_u32 dbsr_demo_en = 0;
    hi_u32 dbsr_demo_mode = 0;
    pq_reg_sr_setdbsrdemoen(vdp_reg, offset, dbsr_demo_en);
    pq_reg_sr_setdbsrdemomode(vdp_reg, offset, dbsr_demo_mode);
    return;
}

static hi_void pq_hal_sr_basic_cfg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 width, hi_u32 height)
{
    hi_u32 outresult = 0;

    pq_reg_sr_setoutresult(vdp_reg, offset, outresult);

    pq_hal_sr_set_dbsr(vdp_reg, offset, width, height);
    pq_hal_sr_set_demo(vdp_reg, offset);
    return;
}

static hi_s32 pq_hal_sr_4k_cfg(vdp_regs_type *vdp_reg)
{
    hi_u32 offset;
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);
    offset = (SR_4K_START_ADDR - SR_CFG_BASE_ADDR) / 4; /* 4 is pointer offset */
    pq_hal_sr_basic_cfg(vdp_reg, offset , 0, 0);

    return HI_SUCCESS;
}

static hi_void pq_hal_sr_splite_alg_8ksr_demo(hi_u32 width, pq_8k_sr_split_out *split_out)
{
    hi_u32 over_lap = 30;
    hi_u32 iw = width / 2; /* 2 is width divide */

    hi_u32 left_width = (width / 2 + 11) / 12 * 12; /* 2,11,12 is align strategy. */
    hi_u32 right_width = width - left_width;

    hi_u32 demo_en = 0;
    hi_u32 demo_mode = 0;
    hi_u32 demo_pos = 0;
    hi_u32 i;

    if (demo_en == 0) {
        for (i = 0; i < PQ_8K_SR_SPLIT_MAX; i++) {
            split_out->demo_en[i] = demo_en;
            split_out->demo_pos[i] = demo_pos;
            split_out->demo_mode[i] = demo_mode;
        }
    } else {
        if (demo_mode == 0) {
            if (demo_pos <= (left_width * 2)) { /* 2 is rate */
                split_out->demo_pos[0] = demo_pos;
                split_out->demo_mode[0] = demo_mode;

                split_out->demo_en[1] = 0;
                split_out->demo_pos[1] = demo_pos;
            } else {
                split_out->demo_pos[0] = 0;
                split_out->demo_mode[0] = 1;

                split_out->demo_en[1] = demo_en;
                split_out->demo_pos[1] = right_width * 2 + over_lap * 2 - iw * 2 + demo_pos; /* 2 is rate */
            }
            split_out->demo_en[0] = demo_en;
            split_out->demo_mode[1] = demo_mode;
        } else {
            if (demo_pos < (left_width * 2)) { /* 2 is rate */
                split_out->demo_en[0] = demo_en;
                split_out->demo_pos[1] = 0;
            } else {
                split_out->demo_en[0] = 0;
                split_out->demo_pos[1] = right_width * 2 + over_lap * 2 - iw * 2 + demo_pos; /* 2 is rate */
            }
            split_out->demo_pos[0] = demo_pos;
            split_out->demo_mode[0] = demo_mode;
            split_out->demo_en[1] = demo_en;
            split_out->demo_mode[1] = demo_mode;
        }
    }
    return;
}

static hi_void pq_hal_sr_splite_alg_8ksr_demo_dbsr(hi_u32 width, pq_8k_sr_split_out *split_out)
{
    hi_u32 over_lap = 30;
    hi_u32 iw = width / 2; /* 2 is width divide */

    hi_u32 dbsr_demo_en = 0;
    hi_u32 dbsr_demo_mode = 0;
    hi_u32 dbsr_demo_pos = 0;

    hi_u32 left_w = (width / 2 + 11) / 12 * 12; /* 2,11,12 is align strategy. */
    hi_u32 right_w = width - left_w;
    hi_u32 i;

    if (dbsr_demo_en == 0) {
        for (i = 0; i < PQ_8K_SR_SPLIT_MAX; i++) {
            split_out->dbsr_demo_en[i] = dbsr_demo_en;
            split_out->dbsr_demo_pos[i] = dbsr_demo_pos;
            split_out->dbsr_demo_mode[i] = dbsr_demo_mode;
        }
    } else {
        if (dbsr_demo_mode == 0) {
            if (dbsr_demo_pos <= (left_w * 2)) { /* 2 is rate */
                split_out->dbsr_demo_pos[0] = dbsr_demo_pos;
                split_out->dbsr_demo_mode[0] = dbsr_demo_mode;
                split_out->dbsr_demo_en[1] = 0;
                split_out->dbsr_demo_pos[1] = dbsr_demo_pos;
            } else {
                split_out->dbsr_demo_pos[0] = 0;
                split_out->dbsr_demo_mode[0] = 1;
                split_out->dbsr_demo_en[1] = dbsr_demo_en;
                split_out->dbsr_demo_pos[1] = right_w * 2 + over_lap * 2 - iw * 2 + dbsr_demo_pos; /* 2 is rate */
            }
            split_out->dbsr_demo_en[0] = dbsr_demo_en;
            split_out->dbsr_demo_mode[1] = dbsr_demo_mode;
        } else {
            if (dbsr_demo_pos < (left_w * 2)) { /* 2 is rate */
                split_out->dbsr_demo_en[0] = dbsr_demo_en;
                split_out->dbsr_demo_pos[1] = 0;
            } else {
                split_out->dbsr_demo_en[0] = 0;
                split_out->dbsr_demo_pos[1] = right_w * 2 + over_lap * 2 - iw * 2 + dbsr_demo_pos; /* 2 is rate */
            }
            split_out->dbsr_demo_pos[0] = dbsr_demo_pos;
            split_out->dbsr_demo_mode[0] = dbsr_demo_mode;
            split_out->dbsr_demo_en[1] = dbsr_demo_en;
            split_out->dbsr_demo_mode[1] = dbsr_demo_mode;
        }
    }

    return;
}


static hi_void pq_hal_sr_splite_alg_8ksr(hi_u32 width, pq_8k_sr_split_out *split_out)
{
    hi_u32 over_lap = 30;

    hi_u32 c0_graph = 0;
    hi_u32 c1_graph = width;
    hi_u32 left_width = (width / 2 + 11) / 12 * 12; /* 2,11,12 is align strategy. */

    pq_hal_sr_splite_alg_8ksr_demo(width, split_out);
    pq_hal_sr_splite_alg_8ksr_demo_dbsr(width, split_out);

    if ((c0_graph <= left_width) && (c1_graph <= left_width)) {
        split_out->c0_graph_split[0] = c0_graph;
        split_out->c1_graph_split[0] = c1_graph;
        split_out->c0_graph_split[1] = 0;
        split_out->c1_graph_split[1] = 0;
    } else if ((c0_graph >= left_width) && (c1_graph >= left_width)) {
        split_out->c0_graph_split[0] = 0;
        split_out->c1_graph_split[0] = 0;
        split_out->c0_graph_split[1] = c0_graph - left_width + over_lap;
        split_out->c1_graph_split[1] = c1_graph - left_width + over_lap;
    } else if ((c0_graph <= left_width) && (c1_graph >= left_width)) {
        split_out->c0_graph_split[0] = c0_graph;
        split_out->c1_graph_split[0] = left_width;
        split_out->c0_graph_split[1] = over_lap;
        split_out->c1_graph_split[1] = c1_graph - left_width + over_lap;
    }

    return;
}

static hi_s32 pq_hal_sr_8k_cfg(vdp_regs_type *vdp_reg)
{
    hi_u32 split_offset;
    hi_u32 offset;
    hi_u32 outresult = 0;
    hi_u32 para_mode = 0;
    hi_u32 graph_soft_en = 0;
    hi_u32 i;
    hi_u32 width;
    hi_u32 height;
    pq_8k_sr_split_out split_out = {0};

    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    offset = (SR_8K_START_ADDR - SR_CFG_BASE_ADDR) / 4; /* 4 is pointer offset */
    width = 0;
    height = 0;

    pq_hal_sr_splite_alg_8ksr(width, &split_out);
    pq_hal_sr_basic_cfg(vdp_reg, offset, width, height);

    for (i = 0; i < PQ_8K_SR_SPLIT_MAX; i++) {
        split_offset = offset + g_pq_sr8k_split_offset_reg[i];
        pq_reg_sr_setsren(vdp_reg, split_offset, HI_TRUE);
        pq_reg_sr_setoutresult(vdp_reg, split_offset, outresult);
        pq_reg_sr_sethippsrckgten(vdp_reg, split_offset, HI_TRUE);
        pq_reg_sr_setparamode(vdp_reg, split_offset, para_mode);
        pq_reg_sr_setgraphsoften(vdp_reg, split_offset, graph_soft_en);
        pq_reg_sr_setdbsrdemoen(vdp_reg, split_offset, split_out.dbsr_demo_en[i]);
        pq_reg_sr_setdbsrdemomode(vdp_reg, split_offset, split_out.dbsr_demo_mode[i]);
        pq_reg_sr_setc0graphsplit(vdp_reg, split_offset, split_out.c0_graph_split[i]);
        pq_reg_sr_setc1graphsplit(vdp_reg, split_offset, split_out.c1_graph_split[i]);
    }
    return HI_SUCCESS;
}

hi_s32 pq_hal_sr_init(hi_void)
{
    hi_s32 ret;

    g_vdp_sr_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sr_reg);

    ret = pq_hal_sr_4k_cfg(g_vdp_sr_reg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = pq_hal_sr_8k_cfg(g_vdp_sr_reg);

    return ret;
}

hi_s32 pq_hal_sr_deinit(hi_void)
{
    g_vdp_sr_reg = HI_NULL;
    return HI_SUCCESS;
}

hi_s32 pq_hal_sr_set_scale_mode(pq_sr_id id, hi_u32 scale_mode)
{
    hi_u32 offset;
    hi_u32 split_offset;
    hi_u32 i;
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_sr_reg);

    if (id == SR_ID_4K) {
        offset = (SR_4K_START_ADDR - SR_CFG_BASE_ADDR) / 4; /* 4 is pointer offset */
        pq_reg_sr_setscalemode(g_vdp_sr_reg, offset, scale_mode);
    } else {
        offset = (SR_8K_START_ADDR - SR_CFG_BASE_ADDR) / 4; /* 4 is pointer offset */
        pq_reg_sr_setscalemode(g_vdp_sr_reg, offset, scale_mode);

        for (i = 0; i < PQ_8K_SR_SPLIT_MAX; i++) {
            split_offset = offset + g_pq_sr8k_split_offset_reg[i];
            pq_reg_sr_setscalemode(g_vdp_sr_reg, split_offset, scale_mode);
        }
    }
    return HI_SUCCESS;
}

